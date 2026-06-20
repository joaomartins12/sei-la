
#include "stdafx.h"
#include "TempRoot.h"

cTempRoot		g_TempRoot;

cTempRoot::cTempRoot()
{
	m_pOrgRoot = NULL;
	m_pCsPathEngine = NULL;
	

	m_bLoad = false;
}

void cTempRoot::InitTemp()
{
}

void cTempRoot::DeleteTemp()
{
	if( IsLoad() == false )
		return;

	//m_pBaseNiNode = 0;
	//m_RootNode.Delete();
	//m_Info.Delete();

	//m_pDirectionLight = 0;
	//m_pWireframeProp = 0;

	//// SkyBox
	//m_SkyBox.Delete();

	//// Fog
	//_DeleteFog();

	//// Emr Info
	//_DeleteEmrFile();

	//// ToolInfo
	//SAFE_DELETE( m_pToolInfo );

	//SAFE_DELETE_ARRAY( m_pfBackupHeightField );
	//SAFE_DELETE_ARRAY( m_pfOrgHeightField );

	nsCsGBTerrain::g_pCurRoot->Delete();

	switch( m_eTempRootType ) 
	{
	case TT_NONE:
	case TT_PORTALOBJ:
	case TT_NPC_PORTAL:
		nsCsGBTerrain::g_pCurRoot = m_pOrgRoot;
		g_pCsPathEngine = m_pCsPathEngine;
		m_pOrgRoot = NULL;
		m_pCsPathEngine = NULL;
		break;
	default:
		assert_cs( false );
	}

	m_bLoad = false;
}

bool cTempRoot::LoadTemp( const char* cFile, eTEMPROOT_TYPE eTempRootType )
{
	assert_cs( m_pOrgRoot == NULL );
	m_pOrgRoot = nsCsGBTerrain::g_pCurRoot;
	m_pCsPathEngine = g_pCsPathEngine;
	m_eTempRootType = eTempRootType;


	nsCsGBTerrain::g_pCurRoot = this;
	g_pCsPathEngine = NULL;

	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );
	assert_cs( m_RootNode.GetNiNode() == NULL );
	assert_csm1( _access_s( cFile, 0 ) == 0, "맵 데이터가 존재하지 않습니다.\n\n %s", cFile );
	NiStream stream;
	stream.Load( cFile );
	int nNodeIndex;
	if( stream.GetObjectCount() == 1 )
	{
		nNodeIndex = 0;
	}
	else
	{
		assert_cs( stream.GetObjectCount() == 2 );
		nNodeIndex = 1;		
	}

	CsNiNodePtr pNiNode = (CsNiNode*)stream.GetObjectAt( nNodeIndex );
	if( LoadExtraData( pNiNode, cFile ) == false )
	{
		return false;
	}

	// 로드 관련 디바이스 셋팅
	_LoadDeviceSetting();

	// Path Save
	nsCSFILE::GetRelativePath( m_cFilePath, MAX_PATH, (char*)cFile );

	// PathEngine Ground
	nsCsGBTerrain::g_SaveLoad.g_bLoadPathEngineWarning = false;	
	_LoadPathEngine( cFile );
	g_pCsPathEngine = NULL;
	nsCsGBTerrain::g_SaveLoad.g_bLoadPathEngineWarning = true;

	// Create Node
	CreateFromFile( pNiNode, nsCsGBTerrain::CT_FILELOAD_BYTOOL );


	LoadToolExtraData( pNiNode, cFile );
//	CheckInTerrainObject();
	MakeOrgHeightField();
	MakeHeightField();
	//CalculateNormal();
	( nsCsGBTerrain::g_Device.g_bPixelShader2 == true ) ? _LoadAMTexture( cFile ) : _LoadAMswTexture( cFile );
	_LoadSMorgTexture( cFile );
	_LoadEMTexture( cFile );
	m_SkyBox.Create();
	m_RootNode.GetNiNode()->UpdateProperties();
	m_RootNode.GetNiNode()->Update( 0.0f );	
	m_pFogProp->SetFog( false );	

	m_bRenderObject = true;
	m_bLoad = true;

	return true;
}

void cTempRoot::RenderTerrain_ByTool( bool bRenderSkyBox )
{
	CsGBTerrainRoot::RenderTerrain_ByTool( bRenderSkyBox );

	switch( m_eTempRootType )
	{
	case TT_NONE:
		break;
	case TT_PORTALOBJ:
		assert_cs( m_pTarget != NULL );
		if( IsInTerrain( m_pTarget->s_ptCenterPos.x, m_pTarget->s_ptCenterPos.y ) == true )
		{
			g_ToolNiNodeMng.GetRegion()->Render( m_pTarget->s_ptCenterPos, m_pTarget->s_fRadius, false );
		}
		if( IsInTerrain( (float)m_ViewX, (float)m_ViewY ) == true )
		{
			g_ToolNiNodeMng.GetRegion()->Render( NiPoint2( (float)m_ViewX, (float)m_ViewY ), 300.0f, NiColor( 0, 1, 0 ) );
		}
		break;
	case TT_NPC_PORTAL:
		assert_cs( m_pX != NULL );
		if( IsInTerrain( (float)*m_pX, (float)*m_pY ) == true )
		{
			g_ToolNiNodeMng.GetRegion()->Render( NiPoint2( (float)*m_pX, (float)*m_pY ), (float)*m_pRadius, false );
		}
		if( IsInTerrain( (float)m_ViewX, (float)m_ViewY ) == true )
		{
			g_ToolNiNodeMng.GetRegion()->Render( NiPoint2( (float)m_ViewX, (float)m_ViewY ), 300.0f, NiColor( 0, 1, 0 ) );
		}
		break;
	default:
		assert_cs( false );
	}
}

void cTempRoot::OnLButtonDown( CPoint pt )
{
	switch( m_eTempRootType )
	{
	case TT_NONE:
		return;
	case TT_PORTALOBJ:
		_LButtonDown_PortalObj( pt );
		return;
	case TT_NPC_PORTAL:
		_LButtonDown_PortalNpc( pt );
		return;
	}
	assert_cs( false );
}

void cTempRoot::_LButtonDown_PortalObj( CPoint pt )
{
	NiPick::Record* pRecord = Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return;

	NiPoint3 ptIntersect = pRecord->GetIntersection();
	if( GET_MV_VIEW->_IsControl() == true )
	{
		GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetDlgPortal()->_SetViewTargetPos( NiPoint2( ptIntersect.x, ptIntersect.y ) );
	}
	else
	{
		GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetDlgPortal()->_SetDestTargetPos( NiPoint2( ptIntersect.x, ptIntersect.y ) );
	}		
}

void cTempRoot::_LButtonDown_PortalNpc( CPoint pt )
{
	NiPick::Record* pRecord = Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
		return;

	NiPoint3 ptIntersect = pRecord->GetIntersection();
	if( GET_MV_VIEW->_IsControl() == true )
	{
		GET_MV_MAINTAB_DLG->_GetDlgModel()->_GetDlgPortal()->_SetViewTargetPos( (int)ptIntersect.x, (int)ptIntersect.y );
	}
	else
	{
		GET_MV_MAINTAB_DLG->_GetDlgModel()->_GetDlgPortal()->_SetDestTargetPos( (int)ptIntersect.x, (int)ptIntersect.y );
	}	
}