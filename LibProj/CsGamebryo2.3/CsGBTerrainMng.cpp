

#include "stdafx.h"
#include "CsGBTerrainMng.h"

CsGBTerrainMng*					nsCsGBTerrain::g_pTRMng = NULL;
nsCsGBTerrain::sSERVERTIME		nsCsGBTerrain::g_ServerTime;

CsGBTerrainMng::CsGBTerrainMng():m_pTerrainMake(NULL),m_pOldRoot(NULL),m_dwBackupMapID(0)
{
}

CsGBTerrainMng::~CsGBTerrainMng()
{

}

void CsGBTerrainMng::DeleteMempool()
{
	THREAD_MEMPOOL_DELETE( sNODE_SHADER_BLUREFFECT )
	THREAD_MEMPOOL_DELETE( CGeometryData )
	THREAD_MEMPOOL_DELETE( CGeomCharOutLine )
	THREAD_MEMPOOL_DELETE( CGeomMeshOutLine )
	THREAD_MEMPOOL_DELETE( CGeomCharOutLine_Gray )
	THREAD_MEMPOOL_DELETE( CGeomMeshOutLine_Gray )

	CSGBMEMPOOL_DELETE( CsGBSkyBox::sGEOM_INFO );
	CSGBMEMPOOL_DELETE( sUSER_DEFINE );
	CSGBMEMPOOL_DELETE( CsGBObjPropFactor );
	CSGBMEMPOOL_DELETE( CsGBObjOptFactor );
	CSGBMEMPOOL_DELETE( CsGBObjOpt_DarkMap );
	CSGBMEMPOOL_DELETE( CsGBObject );
	CSGBMEMPOOL_DELETE( CsGBObj_Portal );
	CSGBMEMPOOL_DELETE( CsGBObj_Rot );
	CSGBMEMPOOL_DELETE( CsGBObj_Eff );
	CSGBMEMPOOL_DELETE( CsGBObjProp_Alpha );
	CSGBMEMPOOL_DELETE( CsGBObjProp_ABV );
	CSGBMEMPOOL_DELETE( CsGBObjProp_AvCap );
	CSGBMEMPOOL_DELETE( CsGBObjProp_AvTbp );
	CSGBMEMPOOL_DELETE( CsGBObjProp_Sound );
	CSGBMEMPOOL_DELETE( CsGBObjProp_Actor );
}

void CsGBTerrainMng::InitMempool()
{	
	CSGBMEMPOOL_INIT( sUSER_DEFINE, 128 );
	CSGBMEMPOOL_INIT( CsGBSkyBox::sGEOM_INFO, 16 );
	CSGBMEMPOOL_INIT( CsGBObjPropFactor, 128 );
	CSGBMEMPOOL_INIT( CsGBObjOptFactor, 128 );
	CSGBMEMPOOL_INIT( CsGBObjOpt_DarkMap, 16 );
	CSGBMEMPOOL_INIT( CsGBObject, 256 );
	CSGBMEMPOOL_INIT( CsGBObj_Portal, 10 );
	CSGBMEMPOOL_INIT( CsGBObj_Rot, 3 );
	CSGBMEMPOOL_INIT( CsGBObj_Eff, 5 );
	CSGBMEMPOOL_INIT( CsGBObjProp_Alpha, 32 );
	CSGBMEMPOOL_INIT( CsGBObjProp_ABV, 32 );
	CSGBMEMPOOL_INIT( CsGBObjProp_AvCap, 8 );
	CSGBMEMPOOL_INIT( CsGBObjProp_AvTbp, 8 );
	CSGBMEMPOOL_INIT( CsGBObjProp_Sound, 16 );
	CSGBMEMPOOL_INIT( CsGBObjProp_Actor, 4 );
}

//CsGBTerrainMng::sMAPPORTAL* CsGBTerrainMng::GetNearPortal( NiPoint2 ptSrc )
//{
//	DWORD dwCurMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
//	if( dwCurMapID == 0 )
//		return NULL;
//
//	sMAPPORTAL* pReturn = NULL;
//	float fShortDist = FLT_MAX;
//	float fCurDist;
//	MAP_MAPPORTAL_IT it = m_mapMapPortal.begin();
//	for( ; it!=m_mapMapPortal.end(); ++it )
//	{
//		if( it->second->s_bUsed == true )
//			continue;
//
//		if( it->second->s_dwSrcMapID == dwCurMapID )
//		{
//			fCurDist = ( it->second->s_ptSrcTarget - ptSrc ).Length();
//			if( fCurDist < fShortDist )
//			{
//				fShortDist = fCurDist;
//				pReturn = it->second;				
//			}
//		}	
//	}
//
//	if( pReturn != NULL )
//		pReturn->s_bUsed = true;
//
//	return pReturn;
//}
//
//DWORD CsGBTerrainMng::GetPortalCount( DWORD dwMapID )
//{
//	DWORD dwCount = 0;
//	MAP_MAPPORTAL_IT it = m_mapMapPortal.begin();
//	for( ; it!=m_mapMapPortal.end(); ++it )
//	{
//		if( it->second->s_dwSrcMapID == dwMapID )
//			++dwCount;
//	}
//	return dwCount;
//}

void CsGBTerrainMng::Delete()
{
	/*if( nsCsGBTerrain::g_pObjectVisibleArray != NULL )
	{
		NiDelete nsCsGBTerrain::g_pObjectVisibleArray;
		nsCsGBTerrain::g_pObjectVisibleArray = NULL;
	}*/

	/*if( nsCsGBTerrain::g_pFirst1ObjVisibleArray != NULL )
	{
		NiDelete nsCsGBTerrain::g_pFirst1ObjVisibleArray;
		nsCsGBTerrain::g_pFirst1ObjVisibleArray = NULL;
	}*/


	//m_STMng.Delete();

	CsGBShadow::GlobalShotDown();
	CsGBWeather::GlobalShotDown();

	if( m_pTerrainMake != NULL )
	{
		m_pTerrainMake->Delete();
		delete m_pTerrainMake;
		m_pTerrainMake = NULL;
	}

	DeleteRoot( &nsCsGBTerrain::g_pCurRoot );
	DeleteRoot( &m_pOldRoot );

	nsCsGBTerrain::g_ObjectResMgr.Delete();
	nsCsGBTerrain::g_pRenderer = NULL;

	DeleteMempool();

	nsCsGBTerrain::g_pTRMng = NULL;

	SAFE_NIDELETE( g_pCsPathEngine );
}

void CsGBTerrainMng::DeleteRoot( CsGBTerrainRoot** ppRoot )
{
	if( *ppRoot == NULL )
		return;

	( *ppRoot )->Delete();
	SAFE_NIDELETE( *ppRoot );
}

void CsGBTerrainMng::_NewRoot()
{
	assert_csm( ( nsCsGBTerrain::g_pCurRoot != NULL )||( m_pOldRoot == NULL ), _T( "CurRoot가 존재 하지 않는다면 OldRoot는 반드시 존재 해서는 안된다" ) );
	// 생성시 이미 지형이 존재 한다면
	if( nsCsGBTerrain::g_pCurRoot != NULL )
	{
		// 예전 지형 제거
		//_DeleteRoot( &m_pOldRoot );

		// 현재 지형 예전지형으로 대입
		//m_pOldRoot = nsCsGBTerrain::g_pCurRoot;
		nsCsGBTerrain::g_pCurRoot->Delete();
		SAFE_NIDELETE( nsCsGBTerrain::g_pCurRoot );
	}

	nsCsGBTerrain::g_ObjectResMgr.Delete();	
	nsCsGBTerrain::g_ObjectResMgr.Init();
	
	nsCsGBTerrain::g_pCurRoot = NiNew CsGBTerrainRoot;
	nsCsGBTerrain::g_pCurRoot->Init();	
}

void CsGBTerrainMng::Init( CsGBDX9Renderer* pRenderer, int nMultiTextureCount, bool bPixelShader2, bool bUseMyShader,
							float fViewDetailDist /*= 10000.0f*/, float fViewVisibleDist /*= 40000.0f*/ )
{
	g_Sorting.PreMapLoad();

	nsCsGBTerrain::g_pTRMng = this;

	assert_cs( g_pCsPathEngine == NULL );
	g_pCsPathEngine = NiNew CsPathEngine;
	g_pCsPathEngine->init();

	InitMempool();

	//nsCsGBTerrain::g_pObjectVisibleArray = NiNew NiVisibleArray( 256, 64 );
	//nsCsGBTerrain::g_pFirst1ObjVisibleArray = NiNew NiVisibleArray( 64, 32 );

	assert_cs( pRenderer != NULL );
	assert_cs( nsCsGBTerrain::g_pRenderer == NULL );	
	nsCsGBTerrain::g_pRenderer = pRenderer;	
	nsCsGBTerrain::g_Device.g_nMultiTextureCount = nMultiTextureCount;
	nsCsGBTerrain::g_Device.g_bPixelShader2 = bPixelShader2;
	nsCsGBTerrain::g_Device.g_bUseMyShader = bUseMyShader;
	nsCsGBTerrain::g_TRView.g_fViewDetailDist = fViewDetailDist;
	nsCsGBTerrain::g_TRView.g_fViewVisibleDist = fViewVisibleDist;
	nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_NONE;
	assert_cs( ( nsCsGBTerrain::g_Device.g_nMultiTextureCount == 4 )||( nsCsGBTerrain::g_Device.g_nMultiTextureCount == 8 ) );

	nsCsGBTerrain::g_ObjectResMgr.Init();
	NiRegisterStream(CsNiNode);

	CsGBShadow::GlobalShotInit();
	CsGBWeather::GlobalShotInit();

	//_NewRoot();

	/*assert_cs( CAMERA_ST.GetCameraObj() != NULL );
	CsSpeedTreeMng::sFRUSTUM_INFO fi;
	fi.s_fAspectRatio = CAMERA_ST.GetAspectRatio();
	fi.s_fFieldOfView = CAMERA_ST.GetCameraInfo()->s_fFOV;
	fi.s_fNearClip = CAMERA_ST.GetCameraInfo()->s_fNearPlane;
	fi.s_fFarClip = CAMERA_ST.GetCameraInfo()->s_fFarPlane;
	m_STMng.Create( nsCsGBTerrain::g_pRenderer->GetD3DDevice(), &fi );
	float fColor[ 3 ] = { 1.0f, 1.0f, 1.0f };
	m_STMng.SetFog( 1000.0f, 100000.0f, fColor );*/
}

bool CsGBTerrainMng::CreateRoot_ByTool( const char* cFile, nsCsGBTerrain::eCREATE_TYPE ct, DWORD dwMapID )
{
	// 테이블에서 MapID 가져 온다.
	m_dwBackupMapID = dwMapID;	

	_NewRoot();
	g_Sorting.PreMapLoad();
	bool bSuccess = nsCsGBTerrain::g_pCurRoot->Load( cFile, ct );
	g_Sorting.PostMapLoad();
	_UpdateOclusionObject();
	return bSuccess;
}

bool CsGBTerrainMng::CreateRoot( const char* cFile, nsCsGBTerrain::eCREATE_TYPE ct )
{
	// 테이블에서 MapID 가져 온다.
	m_dwBackupMapID = 0;
	CsMapList* pFTMapList = nsCsMapTable::g_pMapListMng->GetList( cFile );
	if( pFTMapList )
	{
		m_dwBackupMapID = pFTMapList->GetInfo()->s_dwMapID;
	}

	_NewRoot();
	g_Sorting.PreMapLoad();
	bool bSuccess = nsCsGBTerrain::g_pCurRoot->Load( cFile, ct );
	g_Sorting.PostMapLoad();
	_UpdateOclusionObject();
	return bSuccess;
}

void CsGBTerrainMng::_UpdateOclusionObject()
{
	if( CAMERA_STPTR )
		CAMERA_STPTR->RestOcclusionGeometry();

	SAFE_POINTER_RET( nsCsGBTerrain::g_pCurRoot );

	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	int nTotalNodeCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;	
	for( int n=0; n<nTotalNodeCount; ++n )
	{
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( n );
		MAP_GBTERRAIN_OBJECT* pMap = pLeaf->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		for( ; it!=pMap->end(); ++it )
		{
			SAFE_POINTER_CON(it->second);

			if( it->second->IsCallReady() == false )
				continue;

			if( it->second->GetParentID() != 0 )
				continue;

			if( CAMERA_STPTR )
				CAMERA_ST.AddOcclusionGeometry( it->second->GetNiNode() );
		}
	}
}

bool CsGBTerrainMng::CreateRoot( DWORD dwMapID )
{
	if( nsCsGBTerrain::g_bReloadMap == false )
	{
		if( ( nsCsGBTerrain::g_pCurRoot != NULL )&&( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID == dwMapID ) )
		{
			nsCsGBTerrain::g_pCurRoot->ReloadMap();
			return true;
		}		
	}	

	DWORD backupMapID = m_dwBackupMapID;
	m_dwBackupMapID = dwMapID;
	assert_cs( m_dwBackupMapID != 0 );

	CsMapList* pMapList	= nsCsMapTable::g_pMapListMng->GetList( dwMapID );
	CsMapList::sINFO* pMapInfo = pMapList->GetInfo();

	_NewRoot();
	g_Sorting.PreMapLoad();
	bool bSuccess = nsCsGBTerrain::g_pCurRoot->Load( pMapInfo->s_cMapPath.c_str(), nsCsGBTerrain::CT_FILELOAD );
	g_Sorting.PostMapLoad();


	nsCsGBTerrain::g_bReloadMap = false;

	_UpdateOclusionObject();

	return bSuccess;
}

bool CsGBTerrainMng::CreateRoot_ByTool( DWORD dwMapID )
{
	if( ( nsCsGBTerrain::g_pCurRoot != NULL )&&( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID == dwMapID ) )
	{
		nsCsGBTerrain::g_pCurRoot->ReloadMap();
		return true;
	}

	m_dwBackupMapID = dwMapID;
	assert_cs( m_dwBackupMapID != 0 )
	
	CsMapList* pMapList = nsCsMapTable::g_pMapListMng->GetList( dwMapID );
	CsMapList::sINFO* pInfo = pMapList->GetInfo();

	_NewRoot();
	g_Sorting.PreMapLoad();
	bool bSuccess = nsCsGBTerrain::g_pCurRoot->Load( pInfo->s_cMapPath.c_str(),
													nsCsGBTerrain::CT_FILELOAD_BYTOOL );
	g_Sorting.PostMapLoad();

	_UpdateOclusionObject();

	return bSuccess;
}

void CsGBTerrainMng::RenderTerrain()
{
	nsCsGBTerrain::g_pCurRoot->RenderTerrain();

	/*D3DXMATRIX mat;
	NiD3DUtility::GetD3DFromNi( mat, CAMERA_ST.GetTargetObj()->GetRotate(), CAMERA_ST.GetCameraObj()->GetWorldTranslate(), CAMERA_ST.GetTargetObj()->GetScale() );
	m_STMng.Render( nsCsGBTerrain::g_pRenderer->GetD3DDevice(), (float*)&CAMERA_ST.GetWorldTranslate(),
					nsCsGBTerrain::g_pRenderer->GetD3DProj(), mat, (float*)&CAMERA_ST.GetTargetObj()->GetWorldTranslate() );*/
}

void CsGBTerrainMng::RenderTerrain_ByTool( bool bShowSkyBox )
{
	nsCsGBTerrain::g_pCurRoot->RenderTerrain_ByTool( bShowSkyBox );
}

void CsGBTerrainMng::RenderObject_ByTool( float fElapsedTime, bool bClearGetter /*=true*/ )
{
	nsCsGBTerrain::g_pCurRoot->RenderObject_ByTool( fElapsedTime, bClearGetter );
}

CsGBTerrainMake* CsGBTerrainMng::_GetTerrainMake()
{
	if( m_pTerrainMake != NULL )
		return m_pTerrainMake;

	return m_pTerrainMake = NiNew CsGBTerrainMake;
}

void CsGBTerrainMng::CreateMakeTerrain( CsGBTerrainRoot::sINFO* pRootInfo, LPCTSTR szBaseDetailMap )
{
	_NewRoot();
	g_Sorting.PreMapLoad();
	nsCsGBTerrain::g_pCurRoot->CreateFromMake( pRootInfo );	
	_GetTerrainMake()->Create( szBaseDetailMap );
	g_Sorting.PostMapLoad();
}

bool CsGBTerrainMng::ApplyHeightMap( LPCTSTR szMapPath )
{
	return _GetTerrainMake()->ApplyHeightMap( szMapPath );
}

//==========================================================================================
//
//		File
//
//==========================================================================================

void CsGBTerrainMng::DeleteTerrainFile( LPCTSTR szFile )
{
	// nif제거
	DeleteFile( szFile );

	TCHAR sz[ MAX_PATH ];
	size_t len = _tcslen( szFile );	
	// 알파맵 제거
	_tcscpy_s( sz, MAX_PATH, szFile );
	_stprintf_s( &sz[ len - 4 ], MAX_PATH - (len-4), _T( "_AM.cst" ) );
	DeleteFile( sz );
	// 알파맵 소프트웨어 제거
	_tcscpy_s( sz, MAX_PATH, szFile );
	_stprintf_s( &sz[ len - 4 ], MAX_PATH - (len-4), _T( "_AMsw.cst" ) );
	DeleteFile( sz );
	// 베이스 맵 제거
	_tcscpy_s( sz, MAX_PATH, szFile );
	_stprintf_s( &sz[ len - 4 ], MAX_PATH - (len-4), _T( "_BM.cst" ) );
	DeleteFile( sz );
	// 이펙트 맵 제거
	_tcscpy_s( sz, MAX_PATH, szFile );
	_stprintf_s( &sz[ len - 4 ], MAX_PATH - (len-4), _T( "_EM.cst" ) );
	DeleteFile( sz );
	// 쉐도우맵 제거
	_tcscpy_s( sz, MAX_PATH, szFile );
	_stprintf_s( &sz[ len - 4 ], MAX_PATH - (len-4), _T( "_SM.cst" ) );
	DeleteFile( sz );
	// 오리지널 쉐도우맵 제거
	_tcscpy_s( sz, MAX_PATH, szFile );
	_stprintf_s( &sz[ len - 4 ], MAX_PATH - (len-4), _T( "_SMorg.cst" ) );
	DeleteFile( sz );
}


