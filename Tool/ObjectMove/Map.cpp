
#include "stdafx.h"
#include "Map.h"

CsGBTerrainMng*		g_pTerrainMng = NULL;

void cMap::GlobalShotDown()
{
	if( g_pTerrainMng != NULL )
	{
		g_pTerrainMng->Delete();
		SAFE_DELETE( g_pTerrainMng );
	}

	g_Sorting.Delete();
	DESTROY_SINGLETON( CAMERA_STPTR );
	CsGBPick::Delete();
}

void cMap::GlobalInit()
{
	// Ä«¸Þ¶ó
	CsGBPick::Create( NiPick::FIND_ALL, NiPick::WORLD_COORDINATES, true, false );	
	CREATE_SINGLETON( CsGBCamera );
	CAMERA_ST.Create( g_pRenderer );
	CAMERA_ST.ChangeStandardFov( (float)PROC_3D_WIN_WIDTH, (float)PROC_3D_WIN_HEIGHT );	

	assert_cs( g_pTerrainMng == NULL );
	g_pTerrainMng = NiNew CsGBTerrainMng;
	g_pTerrainMng->Init( g_pRenderer, 8, true, true, 15000.0f, 50000.0f );
}

void cMap::Delete()
{
	m_MoveMng.Delete();
	m_CurMapID = 0;
}

void cMap::Init()
{
	m_MoveMng.Init();
	m_CurMapID = 0;
}

void cMap::LoadMap( int nMapID )
{
	g_pThread->GetResMng()->ReleaseConnetTerrain();

	std::string cMapPath = nsCsMapTable::g_pMapListMng->GetList( nMapID )->GetInfo()->s_cMapPath;
	bool bSuccess = g_pTerrainMng->CreateRoot( cMapPath.c_str(), nsCsGBTerrain::CT_FILELOAD_BYTOOL );
	assert_cs( bSuccess == true );
	nsCsGBTerrain::g_pCurRoot->LoadEmrFile( cMapPath.c_str() );	
	nsCsGBTerrain::g_pCurRoot->GetFogProperty()->SetFog( false );
	
	g_pThread->GetResMng()->ApplyConnetTerrain();
	m_MoveMng.DeleteObject();

	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	sCAMERAINFO ci;
	float fHalfHeight = (pInfo->s_nYVertCount-1)*pInfo->s_nGridSize*0.5f;
	ci.s_ptInitPos = NiPoint3( (pInfo->s_nXVertCount-1)*pInfo->s_nGridSize*0.5f, fHalfHeight, 0.0f );
	ci.s_fDist = fHalfHeight/tan( CsD2R( CAMERA_ST.GetCameraInfo()->s_fFOV*0.5f ) );	
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.SetAttachActor_ByTool( false );
	CAMERA_ST.SetDAAConstant( CAMERA_DIST_ANI_ACCEL_CONSTANT_TOOL );
	CAMERA_ST.SetDistRange( 10, 100000 );

	SetCameraAccel();

	g_pWinRTop->_ResetMap( nMapID );
	g_pWinLBottom->_ResetMap( nMapID );
	m_CurMapID = nMapID;
}

void cMap::Update()
{
	if( nsCsGBTerrain::g_pCurRoot == NULL )
		return;

	g_pTerrainMng->Update_ByTool( g_fDeltaTime );
	CAMERA_ST.Update_ByTool( g_fDeltaTime );
	m_MoveMng.Update();
}

void cMap::Render()
{
	if( nsCsGBTerrain::g_pCurRoot == NULL )
		return;

	g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	g_pTerrainMng->BeginRender();
	g_pTerrainMng->RenderObject_ByTool( g_fDeltaTime, true );
	g_pTerrainMng->RenderTerrain_ByTool( false );
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );
	g_pTerrainMng->EndRender();

	m_MoveMng.Render();
}

void cMap::ResetDevice()
{
	Ni2DBuffer* pkBackbuffer = g_pRenderer->GetDefaultBackBuffer();
	CAMERA_ST.ChangeStandardFov( (float)pkBackbuffer->GetWidth(), (float)pkBackbuffer->GetHeight() );
}

void cMap::_SetCameraAccel( bool bSpeedUp )
{
	if( bSpeedUp == false )
	{ 
		CAMERA_ST.SetTranslateAccel( 5.0f );
		CAMERA_ST.SetDistAccel( 1.5f );
	}
	else
	{
		CAMERA_ST.SetTranslateAccel( 15.0f );
		CAMERA_ST.SetDistAccel( 4.5f );
	}
}
