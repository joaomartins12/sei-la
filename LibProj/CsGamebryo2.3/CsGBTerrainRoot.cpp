

#include "stdafx.h"
#include "CsGBTerrainRoot.h"

CsGBTerrainRoot::sINFO::sINFO()
{
	Reset();
}

CsGBTerrainRoot::sINFO::~sINFO()
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CsGBTerrainRoot::SetWireframe(bool bWireframe)
{
	for( int n=0; n < m_Info.s_nTotalLeafCount; ++n )
	{
		CsGBTerrainLeaf* pLeaf = GetNode( n );
		if( NULL == pLeaf )
			continue;

		NiNode* pTerrainNode = pLeaf->GetNiNode();
		if( NULL == pTerrainNode )
			continue;

		NiWireframeProperty* pkWireframeProp = (NiWireframeProperty*)pTerrainNode->GetProperty( NiWireframeProperty::GetType() );
		if( NULL == pkWireframeProp)
		{
			pkWireframeProp = NiNew NiWireframeProperty();
			pTerrainNode->AttachProperty(pkWireframeProp);
			pTerrainNode->UpdateProperties();
		}

		pkWireframeProp->SetWireframe(bWireframe);

		MAP_GBTERRAIN_OBJECT* pMap = pLeaf->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		for( ; it != pMap->end(); ++it )
		{
			if( NULL == it->second )
				continue;

			NiNode* pNode = it->second->GetNiNode();
			if( NULL == pNode )
				continue;

			NiWireframeProperty* pkWireframeProp = (NiWireframeProperty*)pNode->GetProperty( NiWireframeProperty::GetType() );
			if( NULL == pkWireframeProp)
			{
				pkWireframeProp = NiNew NiWireframeProperty();
				pNode->AttachProperty(pkWireframeProp);
				pNode->UpdateProperties();
			}

			pkWireframeProp->SetWireframe(bWireframe);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CsGBTerrainRoot::CsGBTerrainRoot():m_pBaseNiNode(NULL),m_pfBackupHeightField(NULL),m_pfOrgHeightField(NULL),m_pFogProp(NULL),m_pSpeedCell(NULL),m_pWireframeProp(NULL),
m_pToolInfo(NULL),m_ppEmrBlock(NULL),m_fInverseGrid(0.0f),m_fInverseXNodeSize(0.0f),m_fInverseYNodeSize(0.0f),m_fWidth(0.0f),m_fHeight(0.0f),
m_fMinimapScale(0.0f),m_fFogFactorObject(0.0f),m_fFogFactorTerrain(0.0f),m_fOrgClipObject_Far(0.0f),m_fOrgFogObject_Start(0.0f),m_fOrgFogObject_End(0.0f),
m_nEmrBlockWidth(0),m_nEmrBlockHeight(0),m_nEmrBlockScale(0)
{
	m_Info.Reset();
	memset( m_cFilePath, 0, sizeof(char)*MAX_PATH );
}

CsGBTerrainRoot::~CsGBTerrainRoot()
{

}

void CsGBTerrainRoot::Delete()
{
	// Object
	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapNoPick.begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoPick.end();
		for( ; it != itEnd; ++it )
		{
			it->second->DecreaseRefCount();
		}
		m_mapNoPick.clear();
	}

	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapNoClipping.begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoClipping.end();
		for( ; it != itEnd; ++it )
		{
			it->second->DecreaseRefCount();
		}
		m_mapNoClipping.clear();
	}

	{
		m_listActorObj.clear();
	}


	if( m_pSpeedCell != NULL )
		m_pSpeedCell->Destroy();
	SAFE_DELETE( m_pSpeedCell );

	m_pBaseNiNode = 0;
	m_RootNode.Delete();
	m_Info.Reset();
	m_Info.Delete();
	m_FieldInfo.Delete();
	m_TrPathInfo.Delete();

	SAFE_DELETE_ARRAY( m_pfBackupHeightField );
	SAFE_DELETE_ARRAY( m_pfOrgHeightField );	

	// SkyBox
	m_SkyBox.Delete();

	// Fog
	_DeleteFog();

	// Light
	m_LightMng.Delete();

	m_pWireframeProp = 0;

	// ToolInfo
	SAFE_DELETE( m_pToolInfo );

	// Emr Info
	_DeleteEmrFile();

	m_fInverseGrid = 0.0f;
	m_fInverseXNodeSize = 0.0f;
	m_fInverseYNodeSize = 0.0f;
	m_fWidth = 0.0f;
	m_fHeight = 0.0f;
	m_fMinimapScale = 0.0f;
	m_fFogFactorObject = 0.0f;
	m_fFogFactorTerrain = 0.0f;
	m_fOrgClipObject_Far = 0.0f;
	m_fOrgFogObject_Start = 0.0f;
	m_fOrgFogObject_End = 0.0f;
	m_nEmrBlockWidth = 0;
	m_nEmrBlockHeight = 0;
	m_nEmrBlockScale = 0;
}

void CsGBTerrainRoot::Init()
{
	assert_cs( m_ppEmrBlock == NULL );
	assert_cs( m_pBaseNiNode == NULL );
	assert_cs( m_pfBackupHeightField == NULL );
	assert_cs( m_pfOrgHeightField == NULL );

	memset( m_cFilePath, 0, sizeof( char )*MAX_PATH );
	m_Info.Reset();
	m_FieldInfo.Reset();
	m_TrPathInfo.Reset();
	m_RootNode.GetInfo()->Reset();

	m_dwOrgVersion = 0;

	// Region
	m_nOldCheckRegionID = CR_INVALIDE;

	m_pToolInfo = NULL;

	// Light
	m_LightMng.Init();
}

void CsGBTerrainRoot::ReloadMap()
{
	// Region
	m_nOldCheckRegionID = CR_INVALIDE;
}

void CsGBTerrainRoot::CreateFromFile( CsNiNodePtr pNiNode, nsCsGBTerrain::eCREATE_TYPE ct )
{
	nsCsGBTerrain::g_eCreateType = ct;

	if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD )
	{
		assert_cs( m_pToolInfo == NULL );
		m_pToolInfo = csnew sTOOL_INFO;
	}

	assert_cs( pNiNode != NULL );
	assert_cs( m_RootNode.GetNiNode() == NULL );

	// Light
	m_LightMng.LoadExtraData( pNiNode );
	// Fog
	_CreateFog();

	m_pBaseNiNode = pNiNode;
	m_RootNode.Create( CsGBTerrainNode::QI_ROOT, m_pBaseNiNode, &m_RootNode, NULL, m_Info.s_nTreeLevel, 0 );

	_InitDataValue();

	//======================================================================================================
	//
	//		셀 오브젝트
	//
	//======================================================================================================
	assert_cs( m_pSpeedCell == NULL );
	m_pSpeedCell = NiNew CsGBSpeedCell;	
	m_pSpeedCell->LoadExtraData( pNiNode );	
}

void CsGBTerrainRoot::CreateFromMake( sINFO* pRootInfo )
{
	nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_MAKE;

	assert_cs( m_pToolInfo == NULL );
	m_pToolInfo = csnew sTOOL_INFO;

	assert_cs( m_RootNode.GetNiNode() == NULL );
	assert_cs( nsCsGBTerrain::g_Device.g_bPixelShader2 == true );

	// Fog
	_CreateFog();

	memcpy( &m_Info, pRootInfo, sizeof( sINFO ) );
	m_RootNode.Create( CsGBTerrainNode::QI_ROOT, m_pBaseNiNode, &m_RootNode, NULL, m_Info.s_nTreeLevel, 0 );

	_InitDataValue();	

	m_dwOrgVersion = MAP_RES_VERSION;

	// Path Engine
	g_pCsPathEngine->CreateMesh( "ToolData\\Ground_Dummy.xml" );

	assert_cs( m_pSpeedCell == NULL );
	m_pSpeedCell = NiNew CsGBSpeedCell;	
	m_pSpeedCell->Init();
}

void CsGBTerrainRoot::Update( float fElapsedTime )
{
	CAMERA_ST.SetFarFrustumPlane( m_Info.s_fClipTerrain_Far );
	m_RootNode.GetNiNode()->Update( 0.0f, false );
	m_RootNode.UpdateTerrain( fElapsedTime );
	m_SkyBox.Update( fElapsedTime );

	CAMERA_ST.SetFarFrustumPlane( m_Info.s_fClipObject_Far );	
	m_RootNode.UpdateObject( fElapsedTime );

	MAP_GBTERRAIN_OBJECT_IT it = m_mapNoClipping.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoClipping.end();
	for( ; it!=itEnd; ++it )
		it->second->Update( fElapsedTime );

	if( nsCsGBTerrain::g_bSpeedCellRender == true )
		m_pSpeedCell->Update( fElapsedTime );
}

void CsGBTerrainRoot::Update_For_Shadow( float fElapsedTime )
{
	CAMERA_ST.SetDefaultFarFrustumPlane();
	m_RootNode.GetNiNode()->Update( fElapsedTime, false );	
	m_RootNode.UpdateObject_For_Shadow( fElapsedTime );	
}

void CsGBTerrainRoot::Update_For_Shadow_ExceptionObject( float fElapsedTime, CsGBObject* pExceptionObj )
{
	CAMERA_ST.SetDefaultFarFrustumPlane();
	m_RootNode.GetNiNode()->Update( fElapsedTime, false );
	m_RootNode.UpdateObject_For_Shadow_ExceptionObject( fElapsedTime, pExceptionObj );
}

void CsGBTerrainRoot::Update_ByTool( float fElapsedTime )
{
	CAMERA_ST.SetDefaultFarFrustumPlane();

	m_SkyBox.Update( fElapsedTime );
	m_RootNode.GetNiNode()->Update( 0.0f, false );
	m_RootNode.UpdateTerrain( fElapsedTime );
	m_RootNode.UpdateObject( fElapsedTime );

	MAP_GBTERRAIN_OBJECT_IT it = m_mapNoPick.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoPick.end();
	CsGBObject* pObject = NULL;
	for( ; it!=itEnd; ++it )
	{
		pObject = it->second;
		pObject->ResetCallReady();
		pObject->Update( fElapsedTime );
	}

	m_pSpeedCell->Update_ByTool( fElapsedTime );
}

void CsGBTerrainRoot::Render( bool bClearGetter )
{
	if( CAMERA_ST.SetDefaultFarFrustum() == true )
		nsCsGBTerrain::g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );
	m_SkyBox.Render();
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	nsCsGBTerrain::g_pRenderer->SetMaxFogValue( m_fFogFactorTerrain );	
	m_RootNode.Render();

	nsCsGBTerrain::g_pRenderer->SetMaxFogValue( m_fFogFactorObject );
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_F1, true );
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_NOCLIPPING, bClearGetter );
	g_Sorting.RenderTRObject( bClearGetter );

	if( nsCsGBTerrain::g_bSpeedCellRender == true )
		m_pSpeedCell->Render();
}

void CsGBTerrainRoot::Render_Shader( bool bClearGetter, char* cSkinShader, char* cObjectShader )
{
	if( CAMERA_ST.SetDefaultFarFrustum() == true )
		nsCsGBTerrain::g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	//m_SkyBox.Render_Shader( cSkinShader, cObjectShader );
	
	m_RootNode.BeginRenderTR();
	m_RootNode.Render_Shader( cObjectShader );
	

	/*
	nsCsGBTerrain::g_pRenderer->SetMaxFogValue( m_fFogFactorObject );
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_F1, true );
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_NOCLIPPING, bClearGetter );
	g_Sorting.RenderTRObject( bClearGetter );

	if( nsCsGBTerrain::g_bSpeedCellRender == true )
		m_pSpeedCell->Render();*/
}

void CsGBTerrainRoot::RenderTerrain()
{
	if( CAMERA_ST.SetDefaultFarFrustum() == true )
		nsCsGBTerrain::g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	nsCsGBTerrain::g_pRenderer->SetMaxFogValue( m_fFogFactorTerrain );
	m_SkyBox.Render();
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );
	m_RootNode.Render();
}

void CsGBTerrainRoot::RenderObject( bool bClearGetter )
{
	if( CAMERA_ST.SetDefaultFarFrustum() == true )
		nsCsGBTerrain::g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	nsCsGBTerrain::g_pRenderer->SetMaxFogValue( m_fFogFactorObject );	

	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_F1, true );
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_NOCLIPPING, bClearGetter );
	g_Sorting.RenderTRObject( bClearGetter );

	if( nsCsGBTerrain::g_bSpeedCellRender == true )
		m_pSpeedCell->Render();
}

void CsGBTerrainRoot::RenderTerrain_ByTool( bool bRenderSkyBox )
{
	if( CAMERA_ST.SetDefaultFarFrustum() == true )
		nsCsGBTerrain::g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );
	nsCsGBTerrain::g_pRenderer->SetMaxFogValue( m_fFogFactorTerrain );
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );

	if( bRenderSkyBox == true )
	{
		m_SkyBox.Render();
	}
	m_RootNode.Render();
}

void CsGBTerrainRoot::RenderObject_ByTool( float fElapsedTime, bool bClearGetter )
{
	if( m_bRenderObject == false )
		return;

	if( CAMERA_ST.SetDefaultFarFrustum() == true )
		nsCsGBTerrain::g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	nsCsGBTerrain::g_pRenderer->SetMaxFogValue( m_fFogFactorObject );

	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_F1, bClearGetter );
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_NOCLIPPING, bClearGetter );
	g_Sorting.RenderTRObject( bClearGetter );

	m_pSpeedCell->Render();
}

//==========================================================================================
//
//		Node
//
//==========================================================================================


//==========================================================================================
//
//		Data
//
//==========================================================================================

void CsGBTerrainRoot::_InitDataValue()
{	
	m_fInverseGrid = 1.0f/(float)m_Info.s_nGridSize;
	m_fWidth = (float)( ( m_Info.s_nXVertCount-1 )*m_Info.s_nGridSize );
	m_fHeight = (float)( ( m_Info.s_nYVertCount-1 )*m_Info.s_nGridSize );
	m_fInverseXNodeSize = 1.0f / (float)( ( m_Info.s_nXVertCountInNode-1 )*m_Info.s_nGridSize );
	m_fInverseYNodeSize = 1.0f / (float)( ( m_Info.s_nYVertCountInNode-1 )*m_Info.s_nGridSize );

	m_fMinimapScale = 512.0f/m_fHeight;

	CalFogDateValue();	
}

//==========================================================================================
//
//		NiNode
//
//==========================================================================================

void CsGBTerrainRoot::SetBaseNiNode( NiTriStrips* pNiTriObject )
{
	assert_cs( m_pBaseNiNode == NULL );
	m_pBaseNiNode = NiNew CsNiNode;
	m_pBaseNiNode->AttachChild( pNiTriObject );
}

//==========================================================================================
//
//		Object
//
//==========================================================================================

void CsGBTerrainRoot::Insert_No_Pick( CsGBObject* pObject )
{
	assert_cs( pObject->UD_IsNoPick() );
	if( m_mapNoPick.find( pObject->GetInfo()->s_dwUniqID ) == m_mapNoPick.end() )
	{
		pObject->IncreaseRefCount();
		m_mapNoPick[ pObject->GetInfo()->s_dwUniqID ] = pObject;
	}
}

void CsGBTerrainRoot::Delete_No_Pick( CsGBObject* pObject )
{
	assert_cs( m_mapNoPick.find( pObject->GetInfo()->s_dwUniqID ) != m_mapNoPick.end() );

	pObject->DecreaseRefCount();
	m_mapNoPick.erase( pObject->GetInfo()->s_dwUniqID );
}

void CsGBTerrainRoot::Insert_No_Clipping( CsGBObject* pObject )
{
	assert_cs( m_mapNoClipping.find( pObject->GetInfo()->s_dwUniqID ) == m_mapNoClipping.end() );
	pObject->IncreaseRefCount();
	m_mapNoClipping[ pObject->GetInfo()->s_dwUniqID ] = pObject;
}

CsGBObject* CsGBTerrainRoot::ResistObject( CsGBObject::sINFO* pObjectInfo )
{
	// 툴에서만
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );

	pObjectInfo->s_dwUniqID = m_Info.GetNewObjectID();
	CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( pObjectInfo );

	// TB 오브젝트 없어졌음
	//if( pObject->UD_IsTerrainBase() == true )
	//{
	//	// 느리다면 주석으로 바까치기
	//	//MakeHeightField_TBObject( pObject, true );
	//	MakeHeightField();
	//}

	g_Sorting.SetID( pObject->GetGetterID() );
	if( pObject->UD_IsNoPick() == false )
	{
		// 화면을 벗어 나서 배치가 안되었다면
		if( m_RootNode.ResistObject( pObject ) == false )
		{
			// 강제 배치
			_Resist_OutRegionObject( pObject );
		}
	}
	else
	{
		Insert_No_Pick( pObject );
	}
	
	return pObject;
}

void CsGBTerrainRoot::ResistObject_ChangeRotation( CsGBObject::sINFO* pObjectInfo )
{
	// 툴에서만
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );

	CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( pObjectInfo );

	// TB 오브젝트 없어졌음
	//if( pObject->UD_IsTerrainBase() == true )
	//{
	//	// 느려도 어쩔수 없다 필수적
	//	MakeHeightField();
	//}

	g_Sorting.SetID( pObject->GetGetterID() );
	if( pObject->UD_IsNoPick() == false )
	{
		// 화면을 벗어 나서 배치가 안되었다면
		if( m_RootNode.ResistObject( pObject ) == false )
		{
			// 강제 배치
			_Resist_OutRegionObject( pObject );
		}
	}
	else
	{
		Insert_No_Pick( pObject );
	}
}

void CsGBTerrainRoot::_Resist_OutRegionObject( CsGBObject* pObject )
{
	float tx = (float)m_Info.CalSizeX();
	float ty = (float)m_Info.CalSizeY();
	float nx = (float)m_Info.CalNodeSizeX();
	float ny = (float)m_Info.CalNodeSizeY();

	// 가장근처의 노드에 강제 배치
	float x = pObject->GetInfo()->s_trObject.m_Translate.x;
	float y = pObject->GetInfo()->s_trObject.m_Translate.y;
	int nFindXIdx = 0;
	int nFindYIdx = 0;
	if( y < 0.0f )
	{
		if( x < 0.0f )
		{
			nFindXIdx = 0;
			nFindYIdx = 0;			
		}
		else if( x > tx )
		{
			nFindXIdx = m_Info.s_nNodeCountInSide-1;
			nFindYIdx = 0;
		}
		else
		{
			nFindXIdx = CsFloat2Int(x/nx);
			nFindYIdx = 0;
		}
	}
	else if( y > ty )
	{
		if( x < 0.0f )
		{
			nFindXIdx = 0;
			nFindYIdx = m_Info.s_nNodeCountInSide-1;
		}
		else if( x > tx )
		{
			nFindXIdx = m_Info.s_nNodeCountInSide-1;
			nFindYIdx = m_Info.s_nNodeCountInSide-1;
		}
		else
		{
			nFindXIdx = CsFloat2Int(x/nx);
			nFindYIdx = m_Info.s_nNodeCountInSide-1;		
		}
	}
	else
	{
		if( x < 0.0f )
		{
			nFindXIdx = 0;
			nFindYIdx = CsFloat2Int(y/ny);
		}
		else if( x > tx )
		{
			nFindXIdx = m_Info.s_nNodeCountInSide-1;
			nFindYIdx = CsFloat2Int(y/ny);
		}
		else
		{
			assert_cs( false );
			return;
		}
	}

	CsGBTerrainLeaf* pTerrainLeaf = GetNode( nFindXIdx, nFindYIdx );
	if( pTerrainLeaf )
		pTerrainLeaf->ForceResistObject( pObject );
}

CsGBObject* CsGBTerrainRoot::FindCsGBObject( NiNode* pNiNode )
{
	// 툴에서만
	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapNoPick.begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoPick.end();
		for( ; it!=itEnd; ++it )
		{
			if( it->second->GetNiNode() == pNiNode )
				return it->second;
		}
	}

	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );
	return m_RootNode.FindCsGBObject( pNiNode );
}

void CsGBTerrainRoot::ObjectResetCallReady()
{
	m_RootNode.ObjectResetCallReady();
	MAP_GBTERRAIN_OBJECT_IT it = m_mapNoClipping.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoClipping.end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second )
			it->second->ResetCallReady();
	}
}


CsGBObject* CsGBTerrainRoot::FindCsGBObject( DWORD dwUniqObjectID )
{
	// 툴에서만
	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapNoPick.begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoPick.end();
		for( ; it!=itEnd; ++it )
		{
			SAFE_POINTER_CON( it->second );
			if( it->second->GetInfo()->s_dwUniqID == dwUniqObjectID )
				return it->second;
		}
	}

// 	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );
	return m_RootNode.FindCsGBObject( dwUniqObjectID );
}

void CsGBTerrainRoot::DeleteCsGBObject( DWORD dwUniqObjectID )
{
	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapNoPick.begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoPick.end();
		for( ; it!=itEnd; ++it )
		{
			SAFE_POINTER_CON( it->second );
			if( it->second->GetInfo()->s_dwUniqID == dwUniqObjectID )
			{
				Delete_No_Pick( it->second );
				return;
			}
		}
	}

	m_RootNode.DeleteCsGBObject( dwUniqObjectID );
}

DWORD CsGBTerrainRoot::GetPortalTotalCount()
{
	DWORD dwCount = 0;
	int nLeafCount = m_Info.s_nTotalLeafCount;
	ObjectResetCallReady();
	MAP_GBTERRAIN_OBJECT* pMapObject = NULL;
	MAP_GBTERRAIN_OBJECT_IT it;
	for( int i=0; i<nLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = GetNode( i );
		SAFE_POINTER_CON( pLeaf );

		pMapObject = pLeaf->GetPortalObjMap();
		SAFE_POINTER_CON( pMapObject );

		it = pMapObject->begin();
		for( ; it!=pMapObject->end(); ++it )
		{
			if( it->second->IsCallReady() == true )
				++dwCount;
		}		
	}
	return dwCount;
}

CsGBObject* CsGBTerrainRoot::GetPortal( DWORD dwPortalID )
{
	int nLeafCount = m_Info.s_nTotalLeafCount;	
	MAP_GBTERRAIN_OBJECT* pMapObject = NULL;
	MAP_GBTERRAIN_OBJECT_IT it;
	for( int i=0; i<nLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = GetNode( i );
		SAFE_POINTER_CON( pLeaf );

		pMapObject = pLeaf->GetPortalObjMap();
		SAFE_POINTER_CON( pMapObject );

		it = pMapObject->begin();
		for( ; it!=pMapObject->end(); ++it )
		{
			if( ( (CsGBObj_Portal*)it->second )->GetPortalID() == dwPortalID )
				return it->second;
		}
	}
	return NULL;
}

//2015.03.18(lks007)	위탁상점이 NPC 주변 및 포탈 주변의 가까운 위치에 생성되지 않도록... 
bool CsGBTerrainRoot::GetPotalCheck( NiPoint2 pUserPos, float fDist )
{
	int nLeafCount = m_Info.s_nTotalLeafCount;	
	MAP_GBTERRAIN_OBJECT* pMapObject = NULL;
	MAP_GBTERRAIN_OBJECT_IT it;
	for( int i=0; i<nLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = GetNode( i );
		SAFE_POINTER_CON( pLeaf );

		pMapObject = pLeaf->GetPortalObjMap();
		SAFE_POINTER_CON( pMapObject );

		it = pMapObject->begin();
		for( ; it!=pMapObject->end(); ++it )
		{
			NiPoint2 fLentgh = ( (CsGBObj_Portal*)it->second )->GetSrcTarget()->s_ptCenterPos;
			if( (fLentgh - pUserPos).Length() < fDist )
				return true;
		}
	}
	return false;
}

void CsGBTerrainRoot::ResetOption()
{
	ObjectResetCallReady();
	MAP_GBTERRAIN_OBJECT* pMapObject = NULL;
	MAP_GBTERRAIN_OBJECT_IT it;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = GetNode( i );
		SAFE_POINTER_CON( pLeaf );

		pMapObject = pLeaf->GetObjectMap();
		SAFE_POINTER_CON( pMapObject );

		it = pMapObject->begin();
		for( ; it!=pMapObject->end(); ++it )
		{
			if( it->second->IsCallReady() == true )
			{
				CsGBObjOptFactor* pOptFactor = it->second->GetOptionFactor();
				if( pOptFactor != NULL )
					pOptFactor->ResetOption();
			}
		}		
	}
}

void CsGBTerrainRoot::ConvertPortalID()
{
	ObjectResetCallReady();
	for( int i=0; i<nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{			
			MAP_GBTERRAIN_OBJECT* pPortalMap = pLeaf->GetPortalObjMap();
			MAP_GBTERRAIN_OBJECT_IT itPortal = pPortalMap->begin();
			for( ;itPortal != pPortalMap->end(); ++itPortal )
			{
				assert_cs( itPortal->second->GetType() == CsGBObject::OT_PORTAL );
				CsGBObj_Portal* pPortalObj = (CsGBObj_Portal*)itPortal->second;
				if( pPortalObj->IsCallReady() == true )
				{
					assert_cs( pPortalObj->GetPortalID() != 0 );

					// 현재 셋팅 아이디 맵포탈아이디로 변경
					DWORD dwPortal = PORTAL_TO_MAPPORTAL( m_Info.s_dwMapID, pPortalObj->GetPortalID() );
					assert_cs( dwPortal > PORTAL_MIN_LIMIT );
					pPortalObj->SetPortalID( dwPortal );						
					pPortalObj->CalSrcTarget();
				}
			}
		}
	}
}

void CsGBTerrainRoot::SetActor_Animation( DWORD nSeqID, char* cKey )
{
	LIST_GBTERRAIN_OBJECT_IT it = m_listActorObj.begin();
	LIST_GBTERRAIN_OBJECT_IT itEnd = m_listActorObj.end();
	for( ; it!=itEnd; ++it )
	{
		CsGBObjProp_Actor* pProp = (CsGBObjProp_Actor*)(*it)->GetProp( CsGBObjProp::PROP_ACTOR );
		if( _stricmp( pProp->GetKey(), cKey ) == 0 )
		{
			pProp->SetAnimation( nSeqID );
			nsCsGBTerrain::g_bReloadMap = true;
		}
	}
}

CsGBObject* CsGBTerrainRoot::GetActor_Object_NearPos( char* cKey, NiPoint2 pos )
{
	CsGBObject* pObj = NULL;
	CsGBObject* pReturn = NULL;
	float fLength = FLT_MAX;
	LIST_GBTERRAIN_OBJECT_IT it = m_listActorObj.begin();
	LIST_GBTERRAIN_OBJECT_IT itEnd = m_listActorObj.end();
	for( ; it!=itEnd; ++it )
	{
		pObj = *it;
		CsGBObjProp_Actor* pProp = (CsGBObjProp_Actor*)pObj->GetProp( CsGBObjProp::PROP_ACTOR );
		if( _stricmp( pProp->GetKey(), cKey ) != 0 )
			continue;

		NiPoint3 v = pObj->GetCsNode()->m_pNiNode->GetWorldTranslate();
		float fl = ( pos - NiPoint2( v.x, v.y ) ).Length();
		if( fl < fLength )
		{
			pReturn = pObj;
			fLength = fl;
		}
	}

	return pReturn;
}

bool CsGBTerrainRoot::GetActor_Object_2DPick( NiPoint2 & pickPos, char const* cKey, NiPoint2 const& StartPos, NiPoint2 const& TargetPos )
{
	if( m_listActorObj.empty() )
		return false;

	LIST_GBTERRAIN_OBJECT_IT it = m_listActorObj.begin();
	for( ; it != m_listActorObj.end(); ++it )
	{
		CsGBObjProp* pProp = (*it)->GetProp( CsGBObjProp::PROP_ACTOR );
		if( NULL == pProp )
			continue;

		CsGBObjProp_Actor* pPropActor = (CsGBObjProp_Actor*)pProp;
		if( _stricmp( pPropActor->GetKey(), cKey ) != 0 )
			continue;

		DWORD dwAniID = pPropActor->GetAnimation();
		if( dwAniID != CsGBObjProp_Actor::ACT_ANIMATION1 )	// 닫혀있는 상태면 
			continue;

		CsNodeObj* pNodeObj = (*it)->GetCsNode();
		if( NULL == pNodeObj || NULL == pNodeObj->m_pNiNode )
			continue;

		float fObjHeight = pNodeObj->m_pNiNode->GetWorldTransform().m_Translate.z;
		NiPoint3 sPos = NiPoint3(StartPos.x, StartPos.y, fObjHeight + 50);
		NiPoint3 ePos = NiPoint3(TargetPos.x, TargetPos.y, fObjHeight+ 50);
		
		CsGBPick pick( false, NiPick::SORT );
		(*it)->ResetPicking();

		NiPoint3 targetDir = ePos - sPos;
		float fLen = targetDir.Length();
		targetDir.Unitize();

		(*it)->Picking( &pick, sPos, targetDir );
		NiPick::Record* pRecord = pick.GetRecord();
		if( pRecord )
		{
			float fPickDist = pRecord->GetDistance();
			if( fPickDist <= fLen )
			{
				NiPoint3 pos = sPos + (targetDir * (fPickDist - 80.0f));
				pickPos.x = pos.x;
				pickPos.y = pos.y;
				return true;
			}
		}
	}

	return false;
}

NiPick::Record* CsGBTerrainRoot::Pick_ActorObj( char const* cKey, NiPoint3 const& origin, NiPoint3 const&  dir)
{
	if( m_listActorObj.empty() )
		return NULL;

	LIST_GBTERRAIN_OBJECT_IT it = m_listActorObj.begin();
	for( ; it != m_listActorObj.end(); ++it )
	{
		CsGBObjProp* pProp = (*it)->GetProp( CsGBObjProp::PROP_ACTOR );
		if( NULL == pProp )
			continue;

		CsGBObjProp_Actor* pPropActor = (CsGBObjProp_Actor*)pProp;
		if( _stricmp( pPropActor->GetKey(), cKey ) != 0 )
			continue;

		DWORD dwAniID = pPropActor->GetAnimation();
		if( dwAniID != CsGBObjProp_Actor::ACT_ANIMATION1 )	// 닫혀있는 상태면 
			continue;

		CsNodeObj* pNodeObj = (*it)->GetCsNode();
		if( NULL == pNodeObj || NULL == pNodeObj->m_pNiNode )
			continue;

		CsGBPick pick( false, NiPick::SORT );
		(*it)->ResetPicking();

		(*it)->Picking( &pick, origin, dir );
		return pick.GetRecord();
	}

	return NULL;
}

NiPick::Record* CsGBTerrainRoot::Pick_ActorObj( char const* cKey, NiPoint3 const& origin, NiPoint3 const&  dir, float const& dist) 
{
	NiPick::Record* pRecord = Pick_ActorObj( cKey, origin, dir );
	if( NULL == pRecord )
		return NULL;

	if( dist >= pRecord->GetDistance() )
		return pRecord;

	return NULL;
}

void CsGBTerrainRoot::DoAct_Object( CsGBObject::eACT_TYPE act, char* key )
{
	DWORD nKey = (DWORD)CsFPS::GetHashCode( key );

	CsGBObject* pObject = NULL;
	MAP_GBTERRAIN_OBJECT* pMapObject = NULL;
	MAP_GBTERRAIN_OBJECT_IT it;
	MAP_GBTERRAIN_OBJECT_IT itEnd;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pMapObject = GetNode( i )->GetObjectMap();
		it = pMapObject->begin();
		itEnd = pMapObject->end();
		for( ; it!=itEnd; ++it )
		{
			pObject = it->second;
			if( pObject->GetUserDefine()->IsData( sUDD_ACT::Type() ) == false )
				continue;

			sUDD_ACT* pUDD = (sUDD_ACT*)pObject->GetUserDefine()->GetData( sUDD_ACT::Type() );
			if( pUDD->s_nHashCode != nKey )
				continue;

			switch( act )
			{
			case CsGBObject::ACT_DELETE:
				DeleteCsGBObject( pObject->GetInfo()->s_dwUniqID );
				DoAct_Object( act, key );
				nsCsGBTerrain::g_bReloadMap = true;
				return;
			default:
				assert_cs( false );
			}
		}		
	}

	{
		it = m_mapNoPick.begin();
		itEnd = m_mapNoPick.end();
		for( ; it!=itEnd; ++it )
		{
			pObject = it->second;
			if( pObject->GetUserDefine()->IsData( sUDD_ACT::Type() ) == false )
				continue;

			sUDD_ACT* pUDD = (sUDD_ACT*)pObject->GetUserDefine()->GetData( sUDD_ACT::Type() );
			if( pUDD->s_nHashCode != nKey )
				continue;

			switch( act )
			{
			case CsGBObject::ACT_DELETE:
				Delete_No_Pick( it->second );
				DoAct_Object( act, key );
				nsCsGBTerrain::g_bReloadMap = true;
				return;
			default:
				assert_cs( false );
			}
		}
	}
}


void CsGBTerrainRoot::DoPath_Object( CsGBObject::ePATH_TYPE type, char* key )
{
	DWORD nKey = (DWORD)CsFPS::GetHashCode( key );

	CsGBObject* pObject = NULL;
	MAP_GBTERRAIN_OBJECT* pMapObject = NULL;
	MAP_GBTERRAIN_OBJECT_IT it;
	MAP_GBTERRAIN_OBJECT_IT itEnd;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pMapObject = GetNode( i )->GetObjectMap();
		it = pMapObject->begin();
		itEnd = pMapObject->end();
		for( ; it!=itEnd; ++it )
		{
			pObject = it->second;
			if( pObject->GetUserDefine()->IsData( sUDD_PATH::Type() ) == false )
				continue;

			sUDD_PATH* pUDD = (sUDD_PATH*)pObject->GetUserDefine()->GetData( sUDD_PATH::Type() );
			if( pUDD->s_nHashCode != nKey )
				continue;

			switch( type )
			{
			case CsGBObject::PATH_DELETE:
				g_pCsPathEngine->DelDynamicShape( pUDD->s_pDynamicAgent );
				pUDD->Delete();
				DeleteCsGBObject( pObject->GetInfo()->s_dwUniqID );
				DoPath_Object( type, key );
				nsCsGBTerrain::g_bReloadMap = true;
				return;
			default:
				assert_cs( false );
			}
		}		
	}
}

//==========================================================================================
//
//		Picking
//
//==========================================================================================


//==========================================================================================
//
//		RootInfo
//
//==========================================================================================

void CsGBTerrainRoot::Alloc_HField_Normal()
{
	assert_cs( m_FieldInfo.s_pfHeightField == NULL );
	m_FieldInfo.s_pfHeightField = csnew float[ m_Info.s_nXVertCount*m_Info.s_nYVertCount ];
	memset( m_FieldInfo.s_pfHeightField, 0, sizeof( float )*m_Info.s_nXVertCount*m_Info.s_nYVertCount );
}


void CsGBTerrainRoot::MakeOrgHeightField()
{
	if( m_pfOrgHeightField == NULL )
		m_pfOrgHeightField = csnew float[ m_Info.s_nXVertCount*m_Info.s_nYVertCount ];

	// 필드상의 헤이트 필드 적용
	CsGBTerrainLeaf* pLeaf;
	CsGBTerrainLeaf::sNiTriINFO* pLeafInfo;
	int nxTemp, nyTemp, nObjX, nObjY;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		nObjX = i%m_Info.s_nNodeCountInSide;
		nObjY = i/m_Info.s_nNodeCountInSide;
		nxTemp = nObjX*( m_Info.s_nXVertCountInNode - 1 );
		nyTemp = nObjY*( m_Info.s_nYVertCountInNode - 1 );
		if( pLeaf->IsExistTerrain() == true )
		{
			pLeafInfo = pLeaf->GetNiTriInfo();
			for( int y=0; y<m_Info.s_nYVertCountInNode; ++y )
			{
				for( int x=0; x<m_Info.s_nXVertCountInNode; ++x )
				{
					m_pfOrgHeightField[ ( y+nyTemp )*m_Info.s_nXVertCount + x + nxTemp ] =
						pLeafInfo->s_pfHeight[ y*m_Info.s_nXVertCountInNode + x ];
				}
			}
		}
	}
}


void CsGBTerrainRoot::MakeHeightField(bool UpdateObjectPos)
{
	assert_cs( m_FieldInfo.s_pfHeightField != NULL );

	// 오브젝트 위치변화값 적용시키기 위한 백업
	BackupHeightField();

	// 필드상의 헤이트 필드 적용
	CsGBTerrainLeaf* pLeaf;
	CsGBTerrainLeaf::sNiTriINFO* pLeafInfo;
	int nxTemp, nyTemp, nObjX, nObjY;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		nObjX = i%m_Info.s_nNodeCountInSide;
		nObjY = i/m_Info.s_nNodeCountInSide;
		nxTemp = nObjX*( m_Info.s_nXVertCountInNode - 1 );
		nyTemp = nObjY*( m_Info.s_nYVertCountInNode - 1 );
		if( pLeaf->IsExistTerrain() == true )
		{
			pLeafInfo = pLeaf->GetNiTriInfo();
			for( int y=0; y<m_Info.s_nYVertCountInNode; ++y )
			{
				for( int x=0; x<m_Info.s_nXVertCountInNode; ++x )
				{
					m_FieldInfo.s_pfHeightField[ ( y+nyTemp )*m_Info.s_nXVertCount + x + nxTemp ] =
						pLeafInfo->s_pfHeight[ y*m_Info.s_nXVertCountInNode + x ];
				}
			}
		}
	}

	// 오브젝트 호출 초기화
	ObjectResetCallReady();

	MAP_GBTERRAIN_OBJECT* pMapBaseObj = NULL;
	MAP_GBTERRAIN_OBJECT_IT itObject;

	NiPoint3 ptObjectPos;	
	assert_cs( m_pfOrgHeightField != NULL );
	// Terrain Base Object 변화량 적용		
	/*for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pMapBaseObj = GetNode( i )->GetBaseObjMap();
		itObject = pMapBaseObj->begin();
		for( ; itObject != pMapBaseObj->end(); ++itObject )
		{
			if( itObject->second->IsCallReady() == false )
				continue;

			assert_cs( itObject->second->UD_IsTerrainBase() == true );
			ptObjectPos = itObject->second->GetInfo()->s_trObject.m_Translate;
			itObject->second->Reposition( GetHeight_NoneTBP( ptObjectPos.x, ptObjectPos.y ) -
										GetHeight_NoneTBP( ptObjectPos.x, ptObjectPos.y, m_pfOrgHeightField ) );
		}
	}*/

	// 원본 데이터 세이브
	SaveOrgHeightField();

	// Terrain Base Object 높이 필드에 적용		
	/*for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pMapBaseObj = GetNode( i )->GetBaseObjMap();
		itObject = pMapBaseObj->begin();
		for( ; itObject != pMapBaseObj->end(); ++itObject )
		{
			MakeHeightField_TBObject( itObject->second, false );			
		}
	}*/

	if(UpdateObjectPos)
	{
		// 배치된 Base 오브젝트가 아닌 오브젝트에 적용
		for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
		{
			pMapBaseObj = GetNode( i )->GetObjectMap();
			itObject = pMapBaseObj->begin();
			for( ; itObject != pMapBaseObj->end(); ++itObject )
			{
				if( itObject->second->IsCallReady() == false )
					continue;

				ptObjectPos = itObject->second->GetInfo()->s_trObject.m_Translate;
				if( IsInTerrain( ptObjectPos.x, ptObjectPos.y ) == false )
					continue;
				
				itObject->second->Reposition(	GetHeight_NoneTBP( ptObjectPos.x, ptObjectPos.y ) -
												GetHeight_NoneTBP( ptObjectPos.x, ptObjectPos.y, m_pfBackupHeightField ) );
			}
		}
	}

}

void CsGBTerrainRoot::BackupHeightField()
{
	if( m_pfBackupHeightField == NULL )
		m_pfBackupHeightField = csnew float[ m_Info.s_nXVertCount*m_Info.s_nYVertCount ];
	memcpy( m_pfBackupHeightField, m_FieldInfo.s_pfHeightField, sizeof( float )*m_Info.s_nXVertCount*m_Info.s_nYVertCount );
}

void CsGBTerrainRoot::SaveOrgHeightField()
{
	if( m_pfOrgHeightField == NULL )
		m_pfOrgHeightField = csnew float[ m_Info.s_nXVertCount*m_Info.s_nYVertCount ];
	memcpy( m_pfOrgHeightField, m_FieldInfo.s_pfHeightField, sizeof( float )*m_Info.s_nXVertCount*m_Info.s_nYVertCount );
}

//void CsGBTerrainRoot::MakeHeightField_TBObject( CsGBObject* pTBObject, bool bOtherObjectReposition )
//{
//	if( bOtherObjectReposition == true )
//	{
//		BackupHeightField();
//	}
//
//	CsGBPick pick;
//	NiPoint3 origin, dir = NiPoint3( 0.0f, 0.0f, -1.0f );
//	origin.z = 100000.0f;
//
//	NiBound boundObject = pTBObject->GetNiNode()->GetWorldBound();	
//	int minX = (int)( boundObject.GetCenter().x - boundObject.GetRadius() )/m_Info.s_nGridSize;
//	int minY = (int)( boundObject.GetCenter().y - boundObject.GetRadius() )/m_Info.s_nGridSize;
//	int maxX = (int)( boundObject.GetCenter().x + boundObject.GetRadius() )/m_Info.s_nGridSize + 2; // 여유분 +1 에 인덱스이기문에 +1
//	int maxY = (int)( boundObject.GetCenter().y + boundObject.GetRadius() )/m_Info.s_nGridSize + 2;
//
//	if( minX < 0 )						minX = 0;
//	if( minY < 0 )						minY = 0;	
//	if( maxX > m_Info.s_nXVertCount )	maxX = m_Info.s_nXVertCount;
//	if( maxY > m_Info.s_nYVertCount )	maxY = m_Info.s_nYVertCount;
//
//	int nHIndex;
//	float fRecordHeight;
//	for( int y=minY; y<maxY; ++y )
//	{
//		for( int x=minX; x<maxX; ++x )
//		{
//			origin.x = (float)x*m_Info.s_nGridSize;
//			origin.y = (float)y*m_Info.s_nGridSize;
//			nHIndex = y*m_Info.s_nXVertCount + x;
//			if( pick.Pick( pTBObject->GetNiNode(), origin, dir, false ) == true )
//			{
//				NiPick::Record* pRecord = pick.GetRecord();
//				assert_cs( pRecord != NULL );
//				fRecordHeight = pRecord->GetIntersection().z;
//				if( m_pfOrgHeightField[ nHIndex ] < fRecordHeight )
//					m_FieldInfo.s_pfHeightField[ nHIndex ] = fRecordHeight;
//			}
//		}
//	}
//
//	if( bOtherObjectReposition == true )
//	{
//		// 오브젝트 포지션 변경한다고 알린다
//		ObjectResetCallReady();
//
//		nsCSGBFUNC::sTERRAIN_BOUND region;
//		region.s_ptMin = NiPoint2( (float)minX*m_Info.s_nGridSize, (float)minY*m_Info.s_nGridSize );
//		region.s_ptMax = NiPoint2( (float)maxX*m_Info.s_nGridSize, (float)maxY*m_Info.s_nGridSize );
//
//		MAP_GBTERRAIN_OBJECT* pMapBaseObj = NULL;
//		MAP_GBTERRAIN_OBJECT_IT itObject;
//
//		NiPoint3 ptObjectPos;
//		// 배치된 Base 오브젝트가 아닌 오브젝트에 적용
//		for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
//		{
//			pMapBaseObj = GetNode( i )->GetObjectMap();
//			itObject = pMapBaseObj->begin();
//			for( ; itObject != pMapBaseObj->end(); ++itObject )
//			{
//				if( itObject->second->UD_IsTerrainBase() == true )
//					continue;
//
//				if( itObject->second->IsCallReady() == false )
//					continue;
//
//				ptObjectPos = itObject->second->GetInfo()->s_trObject.m_Translate;
//				if( region.InBoundCheck( ptObjectPos.x, ptObjectPos.y ) == true )
//				{
//					itObject->second->Reposition(	GetHeight_NoneTBP( ptObjectPos.x, ptObjectPos.y ) -
//						GetHeight_NoneTBP( ptObjectPos.x, ptObjectPos.y, m_pfBackupHeightField ) );
//				}
//			}
//		}
//	}
//}

//void CsGBTerrainRoot::ResetHeightField_TBObject( CsGBObject* pTBObject )
//{
//	assert_cs( pTBObject->UD_IsTerrainBase() == true );
//
//	BackupHeightField();
//
//	NiBound boundObject = pTBObject->GetNiNode()->GetWorldBound();
//	int minX = (int)( boundObject.GetCenter().x - boundObject.GetRadius() )/m_Info.s_nGridSize;
//	int minY = (int)( boundObject.GetCenter().y - boundObject.GetRadius() )/m_Info.s_nGridSize;
//	int maxX = (int)( boundObject.GetCenter().x + boundObject.GetRadius() )/m_Info.s_nGridSize + 2; // 여유분 +1 에 인덱스이기문에 +1
//	int maxY = (int)( boundObject.GetCenter().y + boundObject.GetRadius() )/m_Info.s_nGridSize + 2;
//
//	if( minX < 0 )						minX = 0;
//	if( minY < 0 )						minY = 0;	
//	if( maxX > m_Info.s_nXVertCount )	maxX = m_Info.s_nXVertCount;
//	if( maxY > m_Info.s_nYVertCount )	maxY = m_Info.s_nYVertCount;
//
//	CsGBPick pick;
//	NiPoint3 origin, dir = NiPoint3( 0.0f, 0.0f, -1.0f );
//	origin.z = 100000.0f;
//
//	int nHIndex;
//	for( int y=minY; y<maxY; ++y )
//	{
//		for( int x=minX; x<maxX; ++x )
//		{
//			origin.x = (float)x*m_Info.s_nGridSize;
//			origin.y = (float)y*m_Info.s_nGridSize;
//			nHIndex = y*m_Info.s_nXVertCount + x;
//			if( pick.Pick( pTBObject->GetNiNode(), origin, dir, false ) == true )
//			{
//				m_FieldInfo.s_pfHeightField[ nHIndex ] = m_pfOrgHeightField[ nHIndex ];
//			}
//		}
//	}
//
//	// 오브젝트 포지션 변경한다고 알린다
//	ObjectResetCallReady();
//
//	nsCSGBFUNC::sTERRAIN_BOUND region;
//	region.s_ptMin = NiPoint2( (float)minX*m_Info.s_nGridSize, (float)minY*m_Info.s_nGridSize );
//	region.s_ptMax = NiPoint2( (float)maxX*m_Info.s_nGridSize, (float)maxY*m_Info.s_nGridSize );
//
//	MAP_GBTERRAIN_OBJECT* pMapBaseObj = NULL;
//	MAP_GBTERRAIN_OBJECT_IT itObject;
//
//	NiPoint3 ptObjectPos;
//	// 배치된 Base 오브젝트가 아닌 오브젝트에 적용
//	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
//	{
//		pMapBaseObj = GetNode( i )->GetObjectMap();
//		itObject = pMapBaseObj->begin();
//		for( ; itObject != pMapBaseObj->end(); ++itObject )
//		{
//			if( itObject->second->UD_IsTerrainBase() == true )
//				continue;
//
//			if( itObject->second->IsCallReady() == false )
//				continue;
//
//			ptObjectPos = itObject->second->GetInfo()->s_trObject.m_Translate;
//			if( region.InBoundCheck( ptObjectPos.x, ptObjectPos.y ) == true )
//			{
//				itObject->second->Reposition(	GetHeight_NoneTBP( ptObjectPos.x, ptObjectPos.y ) -
//					GetHeight_NoneTBP( ptObjectPos.x, ptObjectPos.y, m_pfBackupHeightField ) );
//			}
//		}
//	}
//}

//void CsGBTerrainRoot::CalculateNormal()
//{
//	assert_cs( m_FieldInfo.s_pptNormalField != NULL );
//
//	CsGBTerrainLeaf* pLeaf = NULL;
//	CsGBTerrainLeaf::sNiTriINFO* pNiTriInfo = NULL;
//	NiTriStrips* pTriPatch = NULL;
//	NiTriStripsData* pTriPatchData = NULL;
//	CsGBTerrainLeaf* pMeetLeaf = NULL;
//	NiTriStrips* pMeetTriObject = NULL;
//	NiPoint3 MeetNormal[ 3 ];
//	NiPoint3* pPos = NULL;
//
//	// 노말 정보를 담을 곳 메모리 할당
//	NiPoint3** ppptNormal = csnew NiPoint3*[ m_Info.s_nTotalLeafCount ];	
//
//	// 노멀 계산
//	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
//	{
//		pLeaf = GetNode( i );
//		ppptNormal[ i ] = NULL;
//		if( pLeaf->IsExistTerrain() == true )
//		{
//			pTriPatch = pLeaf->GetNiTriStrip();
//			pTriPatchData = NiDynamicCast( NiTriStripsData, pTriPatch->GetModelData() );
//			assert_cs( NiIsKindOf( NiTriStripsData, pTriPatchData ) == true );
//			pNiTriInfo = pLeaf->GetNiTriInfo();
//			pPos = pTriPatch->GetVertices();
//
//			// 노말 템프
//			ppptNormal[ i ] = NiNew NiPoint3[ pNiTriInfo->s_nVertCount ];
//			memset( ppptNormal[ i ], 0, sizeof( NiPoint3 )*pNiTriInfo->s_nVertCount );
//
//			unsigned short ui0, ui1, ui2;
//			unsigned short usStrips;
//			const unsigned short* pusStripLengths;
//			const unsigned short* pStripLists;
//			unsigned int uiStripLengthSum;
//
//			pTriPatchData->GetStripData( usStrips, pusStripLengths, pStripLists, uiStripLengthSum );
//			//  Cycle through all the strips
//			for ( unsigned short ii = 0; ii < usStrips; ii++ )
//			{
//				for ( unsigned short jj = 2; jj < pusStripLengths[ii]; jj++ )
//				{
//					// get indices to triangle vertices (depends on curr tri)
//					if ( ( jj % 2 ) == 0 )
//					{
//						ui0 = pStripLists[jj - 2];
//						ui1 = pStripLists[jj - 1];
//						ui2 = pStripLists[jj - 0];
//					}
//					else
//					{
//						ui0 = pStripLists[jj - 2];
//						ui1 = pStripLists[jj - 0];
//						ui2 = pStripLists[jj - 1];
//					}
//					// Skip degenerate triangles
//					if (ui0 == ui1 || ui1 == ui2 ||	ui2 == ui0)
//					{
//						continue;
//					}
//
//					// compute normal vector
//					NiPoint3& kVertex0 = pPos[ui0];
//					NiPoint3& kVertex1 = pPos[ui1];
//					NiPoint3& kVertex2 = pPos[ui2];
//					NiPoint3 e1(kVertex1 - kVertex0);
//					NiPoint3 e2(kVertex2 - kVertex1);
//					NiPoint3 normal(e1.Cross(e2));
//
//					NiPoint3::UnitizeVector(normal);
//
//					// update the running sum of normals at the various vertices
//					ppptNormal[ i ][ui0] += normal;
//					ppptNormal[ i ][ui1] += normal;
//					ppptNormal[ i ][ui2] += normal;
//				}
//
//				pStripLists += pusStripLengths[ii];
//			}
//
//			NiPoint3::UnitizeVectors( ppptNormal[ i ], pNiTriInfo->s_nVertCount, sizeof( NiPoint3 ) );
//		}
//	}
//
//	// 경계지역 스무딩
//	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
//	{
//		int nX = i%m_Info.s_nNodeCountInSide;
//		int nY = i/m_Info.s_nNodeCountInSide;
//
//		pLeaf = GetNode( i );
//		if( pLeaf->IsExistTerrain() == true )
//		{
//			// 좌단
//			for( int nVert=0; nVert<m_Info.s_nYVertCountInNode; ++nVert )
//			{
//				int nTempIndex = nVert*m_Info.s_nXVertCountInNode;
//				_CalculateNormal_Side( i, ppptNormal, nTempIndex, CNS_LEFT );
//			}
//			// 우단
//			for( int nVert=0; nVert<m_Info.s_nYVertCountInNode; ++nVert )
//			{
//				int nTempIndex = nVert*m_Info.s_nXVertCountInNode + m_Info.s_nXVertCountInNode-1;
//				_CalculateNormal_Side( i, ppptNormal, nTempIndex, CNS_RIGHT );
//			}
//			// 하단
//			for( int nVert=0; nVert<m_Info.s_nXVertCountInNode; ++nVert )
//			{
//				int nTempIndex = nVert;
//				_CalculateNormal_Side( i, ppptNormal, nTempIndex, CNS_BOTTOM );
//			}
//			// 상단
//			for( int nVert=0; nVert<m_Info.s_nXVertCountInNode; ++nVert )
//			{
//				int nTempIndex = nVert + ( m_Info.s_nXVertCountInNode*(m_Info.s_nYVertCountInNode-1) );
//				_CalculateNormal_Side( i, ppptNormal, nTempIndex, CNS_TOP );
//			}		
//		}
//	}
//
//	// 데이터 복사
//	assert_cs( m_FieldInfo.s_pptNormalField != NULL );
//	int nObjX, nObjY, nxTemp, nyTemp;
//	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
//	{
//		pLeaf = GetNode( i );
//		nObjX = i%m_Info.s_nNodeCountInSide;
//		nObjY = i/m_Info.s_nNodeCountInSide;
//		nxTemp = nObjX*( m_Info.s_nXVertCountInNode - 1 );
//		nyTemp = nObjY*( m_Info.s_nYVertCountInNode - 1 );
//		if( pLeaf->IsExistTerrain() == true )
//		{
//			for( int y=0; y<m_Info.s_nYVertCountInNode; ++y )
//			{				
//				for( int x=0; x<m_Info.s_nXVertCountInNode; ++x )
//				{
//					m_FieldInfo.s_pptNormalField[ ( y+nyTemp )*m_Info.s_nXVertCount + x + nxTemp ] =
//						ppptNormal[ i ][ y*m_Info.s_nXVertCountInNode + x ];
//
//					m_FieldInfo.s_pptNormalField[ ( y+nyTemp )*m_Info.s_nXVertCount + x + nxTemp ].Unitize();
//				}
//			}
//		}
//	}
//
//	// 템프 데이터 제거
//	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
//	{
//		if( ppptNormal[ i ] != NULL )
//		{
//			NiDelete[] ppptNormal[ i ];
//		}
//	}
//	SAFE_DELETE_ARRAY( ppptNormal );
//}
//
//void CsGBTerrainRoot::_CalculateNormal_Side( int nNodeIndex, NiPoint3** ppNormal, int nVertIndex, eCALCULATE_NORMAL_SIDE eCns )
//{
//	int nX = nNodeIndex%m_Info.s_nNodeCountInSide;
//	int nY = nNodeIndex/m_Info.s_nNodeCountInSide;
//
//	int nDestNodeIndex;
//	int nDestVertIndex;
//	int nXVITemp = m_Info.s_nXVertCountInNode - 1;
//	int nYVITemp = ( m_Info.s_nYVertCountInNode - 1 )*m_Info.s_nXVertCountInNode;
//	switch( eCns )
//	{
//	case CNS_LEFT:
//		if( nX == 0 )	return;
//		nDestNodeIndex = nNodeIndex - 1;
//		nDestVertIndex = nVertIndex + nXVITemp;
//		break;
//	case CNS_RIGHT:
//		if( nX == m_Info.s_nNodeCountInSide-1 )		return;
//		nDestNodeIndex = nNodeIndex + 1;
//		nDestVertIndex = nVertIndex - nXVITemp;
//		break;
//	case CNS_BOTTOM:
//		if( nY == 0 )	return;
//		nDestNodeIndex = nNodeIndex - m_Info.s_nNodeCountInSide;
//		nDestVertIndex = nVertIndex + nYVITemp;
//		break;
//	case CNS_TOP:
//		if( nY == m_Info.s_nNodeCountInSide-1 )		return;
//		nDestNodeIndex = nNodeIndex + m_Info.s_nNodeCountInSide;
//		nDestVertIndex = nVertIndex - nYVITemp;
//		break;
//	}
//
//	// 노드간의 만나는 부분 노말값 평균화
//	NiPoint3 sumNormal = ppNormal[ nNodeIndex ][ nVertIndex ]*0.5f + ppNormal[ nDestNodeIndex ][ nDestVertIndex ]*0.5f;
//	NiPoint3::UnitizeVector( sumNormal );
//	ppNormal[ nNodeIndex ][ nVertIndex ] = sumNormal;
//	ppNormal[ nDestNodeIndex ][ nDestVertIndex ] = sumNormal;
//}

//void CsGBTerrainRoot::CheckInTerrainObject()
//{
//	ObjectResetCallReady();
//
//	MAP_GBTERRAIN_OBJECT* pMapBaseObj = NULL;
//	MAP_GBTERRAIN_OBJECT_IT itObject, itNext;
//	NiPoint3 ptObjectPos;
//	// 배치된 Base 오브젝트가 아닌 오브젝트에 적용
//	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
//	{
//		pMapBaseObj = GetNode( i )->GetObjectMap();
//		itObject = itNext = pMapBaseObj->begin();
//		while( itNext != pMapBaseObj->end() )		
//		{
//			itObject = itNext;
//			++itNext;
//
//			if( itObject->second->IsCallReady() == false )
//				continue;
//
//			ptObjectPos = itObject->second->GetInfo()->s_trObject.m_Translate;
//			if( IsInTerrain( ptObjectPos.x, ptObjectPos.y ) == false )
//			{
//				int nMessage = CsMessageBoxA( MB_YESNOCANCEL, "%s\n오브젝트가 지형범위를 벋어 났습니다.\nx = %f, y = %f,\n\n\nYes = 오브젝트 삭제\nNo = 오브젝트를 0점으로 이동\nCancel = 오브젝트를 현재위치에 그냥 존재 시킨다.",
//								itObject->second->GetInfo()->s_cObjectPath, ptObjectPos.x, ptObjectPos.y );
//
//				switch( nMessage )
//				{
//				case IDYES:
//					{
//						std::list< CsGBObject::sCHILD_INFO* >* pChildList = itObject->second->GetChildList();
//						std::list< CsGBObject::sCHILD_INFO* >::iterator itChild = pChildList->begin();
//						for( ; itChild!=pChildList->end(); ++itChild )
//						{
//							DeleteCsGBObject( ( *itChild )->s_dwChildUniqID );
//						}
//						DeleteCsGBObject( itObject->second->GetInfo()->s_dwUniqID );
//					}
//					break;
//				case IDNO:
//					{
//						NiTransform tr;
//						float f = GetHeight( 0, 0 );
//						std::list< CsGBObject::sCHILD_INFO* >* pChildList = itObject->second->GetChildList();
//						std::list< CsGBObject::sCHILD_INFO* >::iterator itChild = pChildList->begin();
//						for( ; itChild!=pChildList->end(); ++itChild )
//						{
//							CsGBObject* pChild = FindCsGBObject( ( *itChild )->s_dwChildUniqID );
//							tr = pChild->GetInfo()->s_trObject;
//							tr.m_Translate = NiPoint3( 0.0f, 0.0f, f );
//							pChild->SetTransform( tr );
//						}
//						tr = itObject->second->GetInfo()->s_trObject;
//						tr.m_Translate = NiPoint3( 0.0f, 0.0f, f );
//						itObject->second->SetTransform( tr );
//					}
//					break;
//				case IDCANCEL:
//					break;
//				}				
//			}
//		}
//	}
//}

void CsGBTerrainRoot::ChangeMapUVSize( float fMapSize )
{
	if( m_Info.s_fDetailMapSize == fMapSize )
		return;

	m_Info.s_fDetailMapSize = fMapSize;

	// 원본 교체
	NiTriStrips* pTriStrip = (NiTriStrips*)m_pBaseNiNode->GetAt( 0 );
	assert_cs( NiIsKindOf( NiTriStrips, pTriStrip ) == true );
	_ChangeUV( pTriStrip->GetTextures() );
	pTriStrip->GetModelData()->MarkAsChanged( NiGeometryData::TEXTURE_MASK );
	pTriStrip->Update( 0.0f );

	// 눈에 보이는거 교체
	CsGBTerrainLeaf* pLeaf = NULL;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == false )
			continue;

		pTriStrip = pLeaf->GetNiTriStrip();
		_ChangeUV( pTriStrip->GetTextures() );
		pTriStrip->GetModelData()->MarkAsChanged( NiGeometryData::TEXTURE_MASK );
		pTriStrip->Update( 0.0f );
	}
}
void CsGBTerrainRoot::UpdateMapUVSize()
{
	int SizeCnt = (int)std::pow(2.0f, m_Info.s_nTreeLevel);

	CsGBTerrainLeaf* pkLeaf = NULL;
	NiTriStrips* pkTriStrip = NULL;
	for(int y =0 ; y < SizeCnt; ++y)
	{
		for(int x = 0; x < SizeCnt; ++x )
		{
			int iDx = y * SizeCnt + x;
			pkLeaf = GetNode( iDx );
			if( pkLeaf->IsExistTerrain() == false )
				continue;
			pkTriStrip = pkLeaf->GetNiTriStrip();
			_ChangeUV2( x , y, pkTriStrip->GetTextures() );
			pkTriStrip->GetModelData()->MarkAsChanged( NiGeometryData::TEXTURE_MASK );
			pkTriStrip->Update( 0.0f );
		}
	}
}
void CsGBTerrainRoot::_ChangeUV( NiPoint2* pUV )
{
	int nXCount = m_Info.s_nXVertCountInNode;
	int nYCount = m_Info.s_nYVertCountInNode;

	for( int y=0; y<nYCount; ++y )
	{
		for( int x=0; x<nXCount; ++x )
		{
			pUV[ y*nXCount + x ] = NiPoint2( m_Info.s_fDetailMapSize*x, m_Info.s_fDetailMapSize*y );
		}
	}
}
void CsGBTerrainRoot::_ChangeUV2( int iX, int iY, NiPoint2* pUV )
{
	int iStartX = m_Info.s_nXVertCountInNode * iX;
	int iStartY = m_Info.s_nYVertCountInNode * iY;
	int iMaxSize =  m_Info.s_nYVertCountInNode*m_Info.s_nXVertCountInNode + m_Info.s_nXVertCountInNode;
	
	//float MaxX = m_Info.s_nXVertCount;
	//float MaxY = m_Info.s_nYVertCount;

	//BHPRT("iX[%d] iY[%d]", iX, iY);
	for( int y=0; y<m_Info.s_nYVertCountInNode; ++y )
	{
		for( int x=0; x<m_Info.s_nXVertCountInNode; ++x )
		{
			int uvX = x + iStartX - iX;
			int uvY = y + iStartY - iY;
			int CellIdx = y*m_Info.s_nXVertCountInNode + x;
			if(0 <= CellIdx && CellIdx <= iMaxSize)
			{
				//pUV[CellIdx] = NiPoint2((uvX / MaxX) * m_Info.s_fDetailMapSize, (uvY / MaxY) * m_Info.s_fDetailMapSize);
				pUV[CellIdx] = NiPoint2(uvX* m_Info.s_fDetailMapSize, uvY* m_Info.s_fDetailMapSize);
				//BHPRT("CellIdx[%i] uvX[%f] uvY[%f] U[%f] V[%f]", CellIdx, uvX, uvY, pUV[CellIdx].x, pUV[CellIdx].y);
				//pUV[CellIdx] = NiPoint2( 0.0f,0.0f);
			}
		}
	}
}


char* CsGBTerrainRoot::GetMinimapName( char* pOut, size_t siBufferSize )
{
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_MAKE );

	memset( pOut, 0, sizeof( char )*siBufferSize );
	assert_cs( siBufferSize >= strlen( m_cFilePath ) );
	char cName[ MAX_FILENAME ];
	strcpy_s( cName, MAX_FILENAME, nsCSFILE::GetFileName( m_cFilePath ) );	
	size_t siLen = strlen( cName );
	strncpy_s( pOut, siBufferSize, cName, siLen - 4 );
	strcat_s( pOut, siBufferSize, "_Mini.tga" );
	return pOut;
}

char* CsGBTerrainRoot::GetZonemapName( int nMapID, char* pOut, size_t siBufferSize )
{
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_MAKE );

	memset( pOut, 0, sizeof( char )*siBufferSize );
	assert_cs( siBufferSize >= strlen( m_cFilePath ) );
	char cName[ MAX_FILENAME ];
	strcpy_s( cName, MAX_FILENAME, nsCSFILE::GetFileName( nsCsMapTable::g_pMapListMng->GetList( nMapID )->GetInfo()->s_cMapPath.c_str() ) );	
	size_t siLen = strlen( cName );
	strncpy_s( pOut, siBufferSize, cName, siLen - 4 );
	strcat_s( pOut, siBufferSize, "_Zone.tga" );
	return pOut;
}

char* CsGBTerrainRoot::GetEMRPath( char* pOut, size_t siBufferSize )
{
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_MAKE );

	memset( pOut, 0, sizeof( char )*siBufferSize );
	assert_cs( siBufferSize >= strlen( m_cFilePath ) );
	size_t siLen = strlen( m_cFilePath );
	strncpy_s( pOut, siBufferSize, m_cFilePath, siLen - 3 );
	strcat_s( pOut, siBufferSize, "emr" );
	return pOut;
}

char* CsGBTerrainRoot::GetMapName( char* pOut, size_t siBufferSize )
{
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_MAKE );
	
	memset( pOut, 0, sizeof( char )*siBufferSize );
	strcpy_s( pOut, siBufferSize, nsCSFILE::GetFileName( m_cFilePath ) );
	size_t siLen = strlen( pOut );
	if( siLen >= 4 )
	{
		pOut[ siLen - 4 ] = NULL;
	}
	return pOut;
}

//==========================================================================================
//
//		SkyBox
//
//==========================================================================================

//==========================================================================================
//
//		Fog
//
//==========================================================================================

void CsGBTerrainRoot::_DeleteFog()
{
	m_pFogProp = 0;
}

void CsGBTerrainRoot::_CreateFog()
{
	// Fog
	assert_cs( m_pFogProp == NULL );
	m_pFogProp = NiNew NiFogProperty;
	m_pFogProp->SetFog( true );
	m_pFogProp->SetFogColor( m_Info.s_FogColor );
}

void CsGBTerrainRoot::CalFogDateValue()
{
	float fDepth = CAMERA_ST.FogStartDistToFogDepth( m_Info.s_fFogStart );
	m_fFogFactorObject = CAMERA_ST.FogEndDistToMaxFogFactor( m_Info.s_fFogEndObject, fDepth );
	m_fFogFactorTerrain = CAMERA_ST.FogEndDistToMaxFogFactor( m_Info.s_fFogEndTerrain, fDepth );
	if( m_pFogProp )
		m_pFogProp->SetDepth( fDepth );
}

//==========================================================================================
//
//		Texture Filter
//
//==========================================================================================
void CsGBTerrainRoot::ResetTextureFilterMode()
{
	BeginRender();

	m_RootNode.ResetTextureFilterMode();

	BeginRender();
}

//==========================================================================================
//
//		Shader
//
//==========================================================================================
void CsGBTerrainRoot::ResetShader()
{
	BeginRender();

	m_RootNode.ResetShader();

	BeginRender();
}

//==========================================================================================
//
//		Clip
//
//==========================================================================================

//void CsGBTerrainRoot::_CalClipViewProj()
//{
//	/*D3DXMATRIX matViewProj, matInvViewProj;	
//	D3DXMatrixMultiply( &matViewProj,	(D3DXMATRIX*)&nsCsGBTerrain::g_pRenderer->GetD3DView(),
//		(D3DXMATRIX*)&nsCsGBTerrain::g_pRenderer->GetD3DProj() );
//
//	D3DXMatrixInverse( &matInvViewProj, NULL, &matViewProj );
//	D3DXMatrixTranspose( &m_matInvPoseViewProj, &matInvViewProj );*/
//}
//
//void CsGBTerrainRoot::_SetFarClipPlane( float fFarClipDist, bool bSetShader )
//{
//	//static D3DXPLANE planWorld;
//	//static D3DXPLANE planViewProj;	
//
//	//if( bSetShader == true )
//	//{
//	//	/*NiPoint3 ptCameraDir = CAMERA_ST.GetWorldDirection();
//	//	NiPoint3 ptWorld = CAMERA_ST.GetWorldTranslate() + ptCameraDir*fFarClipDist;
//	//	D3DXPlaneFromPointNormal( &planWorld, (D3DXVECTOR3*)&ptWorld, (D3DXVECTOR3*)&-ptCameraDir );
//	//	D3DXPlaneTransform( &planViewProj, &planWorld, &m_matInvPoseViewProj );
//	//	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_f4ClipFarPlane", sizeof( float )*4, &planViewProj );*/
//	//}
//	//else
//	//{
//	//	NiPoint3 ptCameraDir = CAMERA_ST.GetWorldDirection();
//	//	NiPoint3 ptWorld = CAMERA_ST.GetWorldTranslate() + ptCameraDir*fFarClipDist;
//	//	D3DXPlaneFromPointNormal( &planWorld, (D3DXVECTOR3*)&ptWorld, (D3DXVECTOR3*)&-ptCameraDir );
//	//	D3DXPlaneTransform( &planViewProj, &planWorld, &m_matInvPoseViewProj );
//	//	nsCsGBTerrain::g_pRenderer->GetD3DDevice()->SetClipPlane( 0, (float*)&planViewProj );		
//	//}	
//}

//==========================================================================================
//
//		Light
//
//==========================================================================================


//==========================================================================================
//
//		Terrain Path
//
//==========================================================================================

int CsGBTerrainRoot::ApplyTerrainPath()
{
	int nErrorIndex = -1;

	CsGBObject::sPATHINFO* pPathInfo = &m_TrPathInfo.s_PathInfo;
	UINT uiGroup = pPathInfo->s_uiPEGroupCount;

	int nOffset = 0;
	for( UINT i=0; i<uiGroup; ++i )
	{
		switch( m_TrPathInfo.s_pType[ i ] )
		{
		case CsGBTerrainRoot::sTR_PATHINFO::MESH4:
			if( g_pCsPathEngine->SetShape( pPathInfo->s_pptTranse[ i ].x, pPathInfo->s_pptTranse[ i ].y, 4, &pPathInfo->s_pData[ nOffset ] ) == false )
			{
				nErrorIndex = i;
			}
			nOffset += 8;
			break;
		case CsGBTerrainRoot::sTR_PATHINFO::MESH6:
			if( g_pCsPathEngine->SetShape( pPathInfo->s_pptTranse[ i ].x, pPathInfo->s_pptTranse[ i ].y, 6, &pPathInfo->s_pData[ nOffset ] ) == false )
			{
				nErrorIndex = i;
			}
			nOffset += 12;
			break;
		case CsGBTerrainRoot::sTR_PATHINFO::MESH8:
			if( g_pCsPathEngine->SetShape( pPathInfo->s_pptTranse[ i ].x, pPathInfo->s_pptTranse[ i ].y, 8, &pPathInfo->s_pData[ nOffset ] ) == false )
			{
				nErrorIndex = i;
			}
			nOffset += 16;
			break;
		default:
			assert_cs( false );
		}
	}

	return nErrorIndex;
}

//==========================================================================================
//
//		Emr File
//
//==========================================================================================
void CsGBTerrainRoot::_DeleteEmrFile()
{
	if( m_ppEmrBlock != NULL )
	{
		for(UINT i=0; i<m_nEmrBlockWidth; ++i)
		{
			SAFE_DELETE_ARRAY( m_ppEmrBlock[ i ] );
		}
		SAFE_DELETE_ARRAY( m_ppEmrBlock );
	}	
}

void GetTmpEmrPath(const char *szSrcFile, char *szDstFile, unsigned nBuffSize)
{
	char *tmp = strrchr((char*)szSrcFile, '\\');
	if(!tmp)
	{
//		xstop1(tmp, "GetTmpEmrPath ERROR szSrcFile(%s)", szSrcFile);
		__asm int 3;
	}

	strcpy_s(szDstFile, nBuffSize, ".\\emr");
	size_t nLen = strlen(szDstFile);
	szDstFile += nLen;

	strcpy_s(szDstFile, nBuffSize-10, tmp);

	tmp = strrchr(szDstFile, '.');
	if(!tmp)
	{
//		xstop1(tmp, "GetTmpEmrPath ERROR2 szSrcFile(%s)", szSrcFile);
		__asm int 3;
	}

	strcpy_s(tmp+1, nBuffSize-20, "emr");

	szDstFile -= nLen;
}


void CsGBTerrainRoot::LoadEmrFile( const char* cFile )
{
	char cEmr[ MAX_PATH ];
	GetEMRPath( cEmr, MAX_PATH );
//	GetTmpEmrPath(cFile, cEmr, sizeof(cEmr));
	
	if( CsFPS::CsFPSystem::IsExistOnlyPack( 0, cEmr ) )
	{
		if( CsFPS::CsFPSystem::SeekLock(0) )
		{
			int nHandle = CsFPS::CsFPSystem::GetFileHandle( 0, cEmr );
			struct sEMR_INFO
			{
				int	s_nWidth;
				int	s_nHeight;
				int	s_nGridSize;
				int	s_nXFieldCount;
			} ei;

			_read( nHandle, &ei, sizeof( sEMR_INFO ) );

			m_nEmrBlockScale  = ei.s_nGridSize;
			m_nEmrBlockWidth  = ei.s_nWidth / m_nEmrBlockScale + 1;
			m_nEmrBlockHeight = ei.s_nHeight / m_nEmrBlockScale + 1;
			m_ppEmrBlock = csnew char*[ m_nEmrBlockWidth ];
			for(UINT i=0; i<m_nEmrBlockWidth; ++i)
			{
				m_ppEmrBlock[ i ] = csnew char[m_nEmrBlockHeight];
			}

			UINT x, y;
			for(y=0; y<m_nEmrBlockHeight; ++y)
			{
				for(x=0; x<m_nEmrBlockWidth; ++x)
				{
					_read( nHandle, &m_ppEmrBlock[x][y], sizeof( char ) );
					m_ppEmrBlock[x][y] = !m_ppEmrBlock[x][y];
				}
			}
			CsFPS::CsFPSystem::SeekUnLock(0);
		}
	}
	else
	{
		FILE* fp = NULL;
		fopen_s( &fp, cEmr, "rb" );

		if( fp == NULL )
		{
			CsMessageBoxA( MB_OK, "EMR 파일 로드 실패, 이동이 되지 않습니다. \n\n%s", cEmr );
			return;
		}

		struct sEMR_INFO
		{
			int	s_nWidth;
			int	s_nHeight;
			int	s_nGridSize;
			int	s_nXFieldCount;
		} ei;

		fread( &ei, sizeof( sEMR_INFO ), 1, fp );

		m_nEmrBlockScale  = ei.s_nGridSize;
		m_nEmrBlockWidth  = ei.s_nWidth / m_nEmrBlockScale + 1;
		m_nEmrBlockHeight = ei.s_nHeight / m_nEmrBlockScale + 1;

		m_ppEmrBlock = csnew char*[ m_nEmrBlockWidth ];

		for(UINT i=0; i<m_nEmrBlockWidth; ++i)
		{
			m_ppEmrBlock[ i ] = csnew char[m_nEmrBlockHeight];
		}

		UINT x, y;
		for(y=0; y<m_nEmrBlockHeight; ++y)
		{
			for(x=0; x<m_nEmrBlockWidth; ++x)
			{
				fread( &m_ppEmrBlock[x][y], sizeof( char ), 1, fp );
				m_ppEmrBlock[x][y] = !m_ppEmrBlock[x][y];
			}
		}

		fclose( fp );
	}	
}

bool CsGBTerrainRoot::IsReachableEmr( UINT x1, UINT y1, UINT x2, UINT y2 )
{
	assert_cs( m_ppEmrBlock != NULL );

	int bx1 = x1/m_nEmrBlockScale;
	int by1 = y1/m_nEmrBlockScale;

	int bx2 = x2/m_nEmrBlockScale;
	int by2 = y2/m_nEmrBlockScale;

	if(!IsBValidEmr(bx1, by1))
	{
		return false;
	}

	if(!IsBValidEmr(bx2, by2))
	{
		return false;
	}

	if(bx1==bx2 && by1==by2)
	{
		return true;
	}

	// 직선을 만들어야만 한다. 거쳐가는 모든 타일 캐치해야 한다.

	int x, y;
	int min, max;

	if(bx1 == bx2) // 수직 x = n;
	{
		min = by2>by1 ? by1 : by2;
		max = by2>by1 ? by2 : by1;

		x = bx1;
		for(y=min; y<=max; y++)
		{
			if(!IsBValidEmr(x, y))
			{
				return false;
			}
		}
		return true;
	}
	if(by1 == by2) // 수평 y = n;
	{
		min = bx2>bx1 ? bx1 : bx2;
		max = bx2>bx1 ? bx2 : bx1;

		y = by1;
		for(x=min; x<=max; x++)
		{
			if(!IsBValidEmr(x, y))
			{
				return false;
			}
		}
		return true;
	}


	float a = ((float)(by2-by1)) / ((float)(bx2-bx1)); // slope
	float b = by1 - a * bx1;

	if(abs((int)(bx2-bx1)) > abs((int)(by2-by1)))
	{
		min = bx2>bx1 ? bx1 : bx2;
		max = bx2>bx1 ? bx2 : bx1;

		for(x=min; x<=max; x++)
		{
			y = CsFloat2Int(a * x + b);
			if(!IsBValidEmr(x, y))
			{
				return false;
			}
		}
	}
	else
	{
		min = by2>by1 ? by1 : by2;
		max = by2>by1 ? by2 : by1;

		for(y=min; y<=max; y++)
		{
			x = CsFloat2Int((y-b)/a);
			if(!IsBValidEmr(x, y))
			{
				return false;
			}
		}
	}

	return true;
}

//==========================================================================================
//
//		Extra Data
//
//==========================================================================================

void CsGBTerrainRoot::_SaveExtraData()
{
	// 기존에 것을 제거
	m_pBaseNiNode->RemoveAllExtraData();
	assert_cs( m_pBaseNiNode->GetExtraDataSize() == 0 );

	// 버젼 갱신
	assert_cs( m_Info.s_dwMapResVersion == MAP_RES_VERSION );

	// ==== 인포 저장	
	assert_cs( sizeof( char ) == 1 );


	//=============================================================================================
	//
	//		루트 인포
	//
	//=============================================================================================
	UINT uiSize = sizeof( sINFO );
	// 메모리 할당
	char* pRootInfo = NiAlloc( char, uiSize );
	memcpy( pRootInfo, &m_Info, sizeof( sINFO ) );
	NiBinaryExtraData* pExtData = NiNew NiBinaryExtraData( uiSize, pRootInfo );
	m_pBaseNiNode->AddExtraData( CsGBTerrainRootExtraKey, pExtData );

	//=============================================================================================
	//
	//		필드 인포
	//
	//=============================================================================================
	int nVertCount = m_Info.s_nXVertCount*m_Info.s_nYVertCount;
	uiSize = sizeof( float )*nVertCount;
	// 메모리 할당
	char* pFieldInfo = NiAlloc( char, uiSize );

	// Height Field	
	memcpy( pFieldInfo, m_FieldInfo.s_pfHeightField, sizeof( float )*nVertCount );

	pExtData = NiNew NiBinaryExtraData( uiSize, pFieldInfo );
	m_pBaseNiNode->AddExtraData( CsGBTR_FieldExtraKey, pExtData );

	//=============================================================================================
	//
	//		노 픽 오브젝트
	//
	//=============================================================================================
	int nObjCount = (int)m_mapNoPick.size();
	if( nObjCount )
	{
		uiSize = sizeof( int );

		// 사이즈 체크
		MAP_GBTERRAIN_OBJECT_IT it = m_mapNoPick.begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoPick.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->ResetCallReady();
			uiSize += it->second->GetSaveSize();
		}
		
		// 메모리 할당
		char* pNoPickInfo = NiAlloc( char, uiSize );
		
		UINT nNoPickOffset = 0;
		// 갯수 기입		
		memcpy( pNoPickInfo, &nObjCount, sizeof( int ) );
		nNoPickOffset += sizeof( int );

		it = m_mapNoPick.begin();
		for( ; it!=itEnd; ++it )
		{
			it->second->PreSaveExtraData();
			it->second->SaveExtraData( pNoPickInfo, nNoPickOffset );
		}

		pExtData = NiNew NiBinaryExtraData( uiSize, pNoPickInfo );
		m_pBaseNiNode->AddExtraData( CsGBTR_NoPickObjKey, pExtData );
	}

	//=============================================================================================
	//
	//		패쓰 인포
	//
	//=============================================================================================
	UINT uiPEGroupCount = m_TrPathInfo.s_PathInfo.s_uiPEGroupCount;
	uiSize =  sizeof( sTR_PATHINFO ) + sizeof( NiPoint2 )*uiPEGroupCount + sizeof( long )*m_TrPathInfo.s_nDataTotalCount + sizeof( char )*uiPEGroupCount;
	// 메모리 할당
	char* pPathInfo = NiAlloc( char, uiSize );

	int nOffset = 0;
	
	memcpy( &pPathInfo[ nOffset ], &m_TrPathInfo, sizeof( sTR_PATHINFO ) );
	nOffset += sizeof( sTR_PATHINFO );

	memcpy( &pPathInfo[ nOffset ], m_TrPathInfo.s_pType, sizeof( char )*uiPEGroupCount );
	nOffset += ( sizeof( char )*uiPEGroupCount );
	memcpy( &pPathInfo[ nOffset ], m_TrPathInfo.s_PathInfo.s_pptTranse, sizeof( NiPoint2 )*uiPEGroupCount );
	nOffset += ( sizeof( NiPoint2 )*uiPEGroupCount );
	memcpy( &pPathInfo[ nOffset ], m_TrPathInfo.s_PathInfo.s_pData, sizeof( long )*m_TrPathInfo.s_nDataTotalCount );

	pExtData = NiNew NiBinaryExtraData( uiSize, pPathInfo );
	m_pBaseNiNode->AddExtraData( CsGBTR_PathExtraKey, pExtData );

	//=============================================================================================
	//
	//		스카이 박스
	//
	//=============================================================================================	
	m_SkyBox.SaveExtraData( m_pBaseNiNode );

	//=============================================================================================
	//
	//		라이트
	//
	//=============================================================================================
	m_LightMng.SaveExtraData( m_pBaseNiNode );

	//======================================================================================================
	//
	//		셀 오브젝트
	//
	//======================================================================================================
	m_pSpeedCell->SaveExtraData( m_pBaseNiNode );

	//=============================================================================================
	//
	//		하위 노드들
	//
	//=============================================================================================
	// 오브젝트 호출 초기화
	ObjectResetCallReady();
	m_RootNode.SaveExtraData( m_pBaseNiNode );	
}

void CsGBTerrainRoot::_SaveToolExtraData()
{
	// 버젼 갱신
	assert_cs( m_pToolInfo->s_dwMapToolResVersion == MAPTOOL_RES_VERSION );

	// ==== 인포 저장	
	assert_cs( sizeof( char ) == 1 );
	// 저장할 사이즈
	unsigned int uiSize = sizeof( sTOOL_INFO );

	// 메모리 할당
	char* pRootInfo = NiAlloc( char, uiSize );

	// 값할당
	memcpy( pRootInfo, m_pToolInfo, sizeof( sTOOL_INFO ) );	

	NiBinaryExtraData* pExtData = NiNew NiBinaryExtraData( uiSize, (char*)pRootInfo );
	m_pBaseNiNode->AddExtraData( CsGBTerrainRootToolExtraKey, pExtData );

	// 오브젝트 호출 초기화
	ObjectResetCallReady();
	m_RootNode.SaveToolExtraData( m_pBaseNiNode );
}

bool CsGBTerrainRoot::LoadExtraData( CsNiNodePtr pNiNode, const char* cFile )
{
	if( pNiNode->GetExtraDataSize() == 0 )
	{
		CsMessageBox( MB_OK, _T( "RootNode 의 ExtraData 가 존재 하지 않습니다.\n맵툴에서 생성한 지형 데이터가 아닙니다." ) );		
		return false;
	}

	NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootExtraKey );
	if( pExtData == NULL )
	{
		CsMessageBox( MB_OK, _T( "RootNode 의 ExtraData 가 존재 하지 않습니다.\n맵툴에서 생성한 지형 데이터가 아닙니다." ) );
		return false;
	}

	unsigned int uiSize;
	char* pData;
	pExtData->GetValue( uiSize, pData );

	// 실제 파일 버젼 기입
	if( m_dwOrgVersion == 0 )
	{
		m_dwOrgVersion = ((sINFO*)pData)->s_dwMapResVersion;
	}

	if( ((sINFO*)pData)->s_dwMapResVersion != MAP_RES_VERSION )
	{
		if( ((sINFO*)pData)->s_dwMapResVersion > MAP_RES_VERSION )
		{
			TCHAR szCurVersion[ 128 ] = {0, };
			TCHAR szOldVersion[ 128 ] = {0, };
			VERSION_STRING( MAP_RES_VERSION, szCurVersion, 128 );
			VERSION_STRING( ((sINFO*)pData)->s_dwMapResVersion, szOldVersion, 128 );
			assert_csm2( false, _T( "클라이언트 버젼이 맵버젼 보다 낮아서 로드 실패.\n Current %s\n Loaded %s" ), szCurVersion, szOldVersion );
			return false;
		}
		if( ((sINFO*)pData)->s_dwMapResVersion < 0x00000402 )
		{
			TCHAR szCurVersion[ 128 ] = {0, };
			TCHAR szOldVersion[ 128 ] = {0, };
			VERSION_STRING( MAP_RES_VERSION, szCurVersion, 128 );
			VERSION_STRING( ((sINFO*)pData)->s_dwMapResVersion, szOldVersion, 128 );
			assert_csm2( false, _T( "RootNode 의 ExtraData는 예전 버젼이어서 로드 실패.\n Current %s\n Loaded %s" ), szCurVersion, szOldVersion );
			return false;
		}
		return _VersionUp( pNiNode, cFile, ((sINFO*)pData)->s_dwMapResVersion );
	}

	if( ( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD )&&( nsCsGBTerrain::g_pTRMng->GetBackupMapID() == 0 ) )
	{
		MessageBox( NULL, _T( "현재 맵은 임시맵으로 저장되어 있어서 띄울수 없습니다." ), NULL, MB_OK );
		return false;
	}

	//======================================================================================================
	//
	//		루트 인포
	//
	//======================================================================================================		
	memcpy( &m_Info, pData, sizeof( sINFO ) );
	m_Info.s_dwMapID = nsCsGBTerrain::g_pTRMng->GetBackupMapID();

	m_fOrgClipObject_Far = m_Info.s_fClipObject_Far;
	m_fOrgFogObject_Start = m_Info.s_fFogStart;
	m_fOrgFogObject_End = m_Info.s_fFogEndObject;

	//======================================================================================================
	//
	//		필드 인포
	//
	//======================================================================================================
	pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTR_FieldExtraKey );
	assert_cs( pExtData != NULL );	
	pExtData->GetValue( uiSize, pData );

	Alloc_HField_Normal();
	int nVertCount = m_Info.s_nXVertCount*m_Info.s_nYVertCount;	
	// Height Field		
	memcpy( m_FieldInfo.s_pfHeightField, pData, sizeof( float )*nVertCount );

	//=============================================================================================
	//
	//		노 픽 오브젝트
	//
	//=============================================================================================
	assert_cs( m_mapNoPick.empty() == true );
	pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTR_NoPickObjKey );
	if( pExtData )
	{
		UINT nNoPickOffset = 0;
		pExtData->GetValue( uiSize, pData );

		int nObjCount;
		memcpy( &nObjCount, &pData[ nNoPickOffset ], sizeof( int ) );
		nNoPickOffset += sizeof( int );

		for( int i=0; i<nObjCount; ++i )
		{
			CsGBObject::sFILE_HEADER fh;
			memcpy( &fh, &pData[ nNoPickOffset ], sizeof( CsGBObject::sFILE_HEADER ) );
			nNoPickOffset += sizeof( CsGBObject::sFILE_HEADER );
			
			assert_cs( fh.s_dwSaveType == CsGBObject::ST_FIRST_OBJECT );
			CsGBObject::sINFO oi;
			CsGBObject::LoadObjectInfo( &oi, pData, nNoPickOffset );
			CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( &oi );
			if( pObject != NULL )
			{
				assert_cs( pObject->GetUserDefine() != NULL );
				pObject->LoadExtraData( pData, nNoPickOffset, oi.s_dwUserDefinePlag );
				g_Sorting.SetID( pObject->GetGetterID() );
				Insert_No_Pick( pObject );
				pObject->CheckShader();
			}
			else
			{
				// 사용 되어 지지 않은 oi의 제거
				oi.s_PathInfo.Delete();
				if( ( oi.s_dwUserDefinePlag & sUSER_DEFINE::UD_PORTAL ) != 0 )
				{
					CsGBObj_Portal obj;
					obj.ResetCallReady();
					obj.LoadExtraData( pData, nNoPickOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}
				else
				{
					CsGBObject obj;
					obj.ResetCallReady();
					obj.LoadExtraData( pData, nNoPickOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}					
			}
		}
	}

	//======================================================================================================
	//
	//		패쓰 인포
	//
	//======================================================================================================
	pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTR_PathExtraKey );
	assert_cs( pExtData != NULL );	
	pExtData->GetValue( uiSize, pData );

	int nOffset = 0;
	memcpy( &m_TrPathInfo, &pData[ nOffset ], sizeof( sTR_PATHINFO ) );
	nOffset += sizeof( sTR_PATHINFO );

	UINT uiPEGroupCount = m_TrPathInfo.s_PathInfo.s_uiPEGroupCount;
	m_TrPathInfo.s_pType = csnew char[ uiPEGroupCount ];
	m_TrPathInfo.s_PathInfo.s_pptTranse = NiNew NiPoint2[ uiPEGroupCount ];
	m_TrPathInfo.s_PathInfo.s_pData = csnew long[ m_TrPathInfo.s_nDataTotalCount ];

	memcpy( m_TrPathInfo.s_pType, &pData[ nOffset ], sizeof( char )*uiPEGroupCount );
	nOffset += sizeof( char )*uiPEGroupCount;
	memcpy( m_TrPathInfo.s_PathInfo.s_pptTranse, &pData[ nOffset ], sizeof( NiPoint2 )*uiPEGroupCount );
	nOffset += sizeof( NiPoint2 )*uiPEGroupCount;
	memcpy( m_TrPathInfo.s_PathInfo.s_pData, &pData[ nOffset ], sizeof( long )*m_TrPathInfo.s_nDataTotalCount );

	//======================================================================================================
	//
	//		스카이 박스
	//
	//======================================================================================================
	m_SkyBox.LoadExtraData( pNiNode );		

	return true;
}


void CsGBTerrainRoot::LoadToolExtraData( CsNiNodePtr pNiNode, const char* cFile )
{
	NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBTerrainRootToolExtraKey );
	if( pExtData == NULL )
	{
		return;
	}

	unsigned int uiSize;
	char* pData;
	pExtData->GetValue( uiSize, pData );

	memcpy( m_pToolInfo, pData, uiSize );
	assert_cs( m_pToolInfo->s_dwMapToolResVersion == MAPTOOL_RES_VERSION );

	ObjectResetCallReady();
	m_RootNode.LoadToolExtraData( m_pBaseNiNode );
}

//==========================================================================================
//
//		Picking
//
//==========================================================================================


float CsGBTerrainRoot::GetHeight_NoneTBP( float xWorld, float yWorld )
{
	assert_csm4( IsInTerrain( xWorld, yWorld ) == true, _T( "Width = %f, Height = %f\nx = %f, y = %f" ), GetMapSizeWidth(), GetMapSizeHeight(), xWorld, yWorld );

	static int nx, ny, index, width;
	static float rx, ry;
	static float* pfHeight = NULL;

	xWorld = xWorld*m_fInverseGrid;
	yWorld = yWorld*m_fInverseGrid;
	assert_cs( xWorld >= 0.0f );
	assert_cs( yWorld >= 0.0f );

	nx = (int)xWorld;
	ny = (int)yWorld;
	if( ( rx = xWorld-nx ) < 0.00001f )
		rx = 0.0f;	
	if( ( ry = yWorld-ny ) < 0.00001f )
		ry = 0.0f;

	width = m_Info.s_nXVertCount;
	pfHeight = m_FieldInfo.s_pfHeightField;
	index = ny*width + nx;	
	if( ( rx != 0.0f )&&( ry != 0.0f ) )
	{
		return	pfHeight[ index ]*(1.0f-rx)*(1.0f-ry) + pfHeight[ index + 1 ]*rx*(1.0f-ry) +
			pfHeight[ index + width ]*(1.0f-rx)*ry + pfHeight[ index + width + 1 ]*rx*ry;
	}
	else if( rx != 0.0f )
	{
		return pfHeight[ index ]*(1.0f-rx) + pfHeight[ index + 1 ]*rx;					
	}
	else if( ry != 0.0f )
	{
		return pfHeight[ index ]*(1.0f-ry) + pfHeight[ index + width ]*ry;
	}

	return pfHeight[ index ];
}

float CsGBTerrainRoot::GetHeight_NoneTBP( float xWorld, float yWorld, float* pfHeightField )
{
	assert_cs( IsInTerrain( xWorld, yWorld ) == true );

	static int nx, ny, index, width;
	static float rx, ry;
	static float* pfHeight = NULL;
	static float fTBPickHeight;
	static float fTHeight;

	xWorld = xWorld*m_fInverseGrid;
	yWorld = yWorld*m_fInverseGrid;
	assert_cs( xWorld >= 0.0f );
	assert_cs( yWorld >= 0.0f );

	nx = (int)xWorld;
	ny = (int)yWorld;
	if( ( rx = xWorld-nx ) < 0.00001f )
		rx = 0.0f;	
	if( ( ry = yWorld-ny ) < 0.00001f )
		ry = 0.0f;

	width = m_Info.s_nXVertCount;
	pfHeight = pfHeightField;
	index = ny*width + nx;	
	if( ( rx != 0.0f )&&( ry != 0.0f ) )
	{
		fTHeight = pfHeight[ index ]*(1.0f-rx)*(1.0f-ry) + pfHeight[ index + 1 ]*rx*(1.0f-ry) +
			pfHeight[ index + width ]*(1.0f-rx)*ry + pfHeight[ index + width + 1 ]*rx*ry;
	}
	else if( rx != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-rx) + pfHeight[ index + 1 ]*rx;					
	}
	else if( ry != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-ry) + pfHeight[ index + width ]*ry;
	}
	else
	{
		fTHeight = pfHeight[ index ];
	}	

	return fTHeight;
}

//==========================================================================================
//
//		WireFrame
//
//==========================================================================================

void CsGBTerrainRoot::ToggleWireFrame()
{
	if( m_pWireframeProp == NULL )
	{
		m_pWireframeProp = NiNew NiWireframeProperty;
		m_pWireframeProp->SetWireframe( true );
	}

	int nLeafCount = m_Info.s_nTotalLeafCount;
	CsGBTerrainLeaf* pLeaf = NULL;
	if( IsWireFrame() == true )
	{
		for( int i=0; i<nLeafCount; ++i )
		{
			pLeaf = GetNode( i );
			if( pLeaf->IsExistTerrain() == true )
				pLeaf->GetNiTriStrip()->RemoveProperty( NiWireframeProperty::GetType() );
		}
	}
	else
	{
		for( int i=0; i<nLeafCount; ++i )
		{
			pLeaf = GetNode( i );
			if( pLeaf->IsExistTerrain() == true )
				pLeaf->GetNiTriStrip()->AttachProperty( m_pWireframeProp );
		}
	}
	m_RootNode.GetNiNode()->UpdateProperties();
}

bool CsGBTerrainRoot::IsWireFrame()
{
	int nLeafCount = m_Info.s_nTotalLeafCount;
	CsGBTerrainLeaf* pLeaf = NULL;
	for( int i=0; i<nLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{
			return ( pLeaf->GetNiTriStrip()->GetProperty( NiWireframeProperty::GetType() ) != NULL );
		}
	}
	assert_cs( false );
	return false;
}

DWORD CsGBTerrainRoot::CheckPortal( float fElapsedTime, NiPoint3 ptPos, NiBoundingVolume* pNiBoundingVolume )
{
	CsGBTerrainLeaf* pkLeaf = GetNode( ptPos );
	SAFE_POINTER_RETVAL(pkLeaf, 0);
	return pkLeaf->CheckPortal(fElapsedTime, pNiBoundingVolume);
}

int CsGBTerrainRoot::CheckRegion( NiPoint3 ptPos, std::wstring& szDiscript, std::string& cBGSound )
{
	if( nsCsMapTable::g_pMapListMng->IsList( m_Info.s_dwMapID ) == false )
		return CR_INVALIDE;

	int nListIndex = 0;
	CsMapRegion::sINFO* pInfo = NULL;
	float fLength;
	NiPoint2 ptSrc = NiPoint2( ptPos.x, ptPos.y );
	CsMapRegionGroup* pGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( m_Info.s_dwMapID );
	CsMapRegion::LIST* pList = pGroup->GetRegionList();
	CsMapRegion::LIST_IT it = pList->begin();
	for( ; it!=pList->end(); ++it )
	{
		pInfo = (*it)->GetInfo();
		fLength = ( ptSrc - NiPoint2( (float)pInfo->s_nCenterX, (float)pInfo->s_nCenterY ) ).Length();
		if( fLength < pInfo->s_nRadius )
		{			
			return _WriteCheckRegion( pInfo, m_nOldCheckRegionID, nListIndex, szDiscript, cBGSound );
		}
		++nListIndex;
	}

	return _WriteCheckRegion( NULL, m_nOldCheckRegionID, CR_WORLD, szDiscript, cBGSound );;
}

int CsGBTerrainRoot::_WriteCheckRegion( CsMapRegion::sINFO* pResInfo,
									   int& nOldResIndex, int nCurResIndex, std::wstring& szDiscript, std::string& cBGSound )
{
	if( nOldResIndex == nCurResIndex )
		return CR_INVALIDE;
	nOldResIndex = nCurResIndex;

	if( pResInfo == NULL )
	{
		CsMapList::sINFO* pListInfo = nsCsMapTable::g_pMapListMng->GetList( m_Info.s_dwMapID )->GetInfo();
		szDiscript = pListInfo->s_szMapDiscript;
		cBGSound = pListInfo->s_cBGSound;
		return CR_WORLD;
	}
	else
	{
		CsMapList::sINFO* pListInfo = nsCsMapTable::g_pMapListMng->GetList( m_Info.s_dwMapID )->GetInfo();
		if( pResInfo->s_szDiscript[ 0 ] == NULL )
			szDiscript = pListInfo->s_szMapDiscript;
		else
			szDiscript = pResInfo->s_szDiscript;

		if( pResInfo->s_cBGSound[ 0 ] == NULL )
			cBGSound = pListInfo->s_cBGSound;
		else
			cBGSound = pResInfo->s_cBGSound;
	}	
	return nCurResIndex;
}

float CsGBTerrainRoot::GetHeight( float xWorld, float yWorld )
{
	assert_csm4( IsInTerrain( xWorld, yWorld ) == true, _T( "Width = %f, Height = %f\nx = %f, y = %f" ), GetMapSizeWidth(), GetMapSizeHeight(), xWorld, yWorld );

	int nx, ny, index, width;
	float rx, ry, fTBPickHeight, fTHeight;
	float* pfHeight = NULL;	
	bool bTBPick;

	CsGBTerrainLeaf* pLeaf = GetNode( NiPoint3( xWorld, yWorld, 0.0f ) );
	pLeaf->Reset_TBP_Picking();
	bTBPick = pLeaf->GetTBPickHeight( NiPoint2( xWorld, yWorld ), fTBPickHeight );

	xWorld = xWorld*m_fInverseGrid;
	yWorld = yWorld*m_fInverseGrid;
	assert_cs( xWorld >= 0.0f );
	assert_cs( yWorld >= 0.0f );

	nx = (int)xWorld;
	ny = (int)yWorld;
	rx = xWorld-nx;
	ry = yWorld-ny;

	width = m_Info.s_nXVertCount;
	pfHeight = m_FieldInfo.s_pfHeightField;

	index = ny*width + nx;

	if( ( rx != 0.0f )&&( ry != 0.0f ) )
	{
		NiPoint3 kV1( 0, 0, pfHeight[ index ] );
		NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ] );
		NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ] );
		if( rx < ry )
		{
			kV2 = NiPoint3( 1, 1, pfHeight[ index + width + 1 ] );
			kV3 = NiPoint3( 0, 1, pfHeight[ index + width ] );
		}

		NiPoint3 kDir( 0, 0, -1 );
		NiPoint3 kOrigin( rx, ry, 100000.0f );
		NiPoint3 kEdge1 = kV2 - kV1;
		NiPoint3 kEdge2 = kV3 - kV1;

		float fDet = kEdge1 * kDir.Cross(kEdge2);
		float r = ( kEdge2 * ( kOrigin - kV1 ).Cross(kEdge1) ) / fDet;
		fTHeight = ( kOrigin + r * kDir ).z;
	}
	else if( rx != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-rx) + pfHeight[ index + 1 ]*rx;					
	}
	else if( ry != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-ry) + pfHeight[ index + width ]*ry;
	}
	else
	{
		fTHeight = pfHeight[ index ];
	}	

	return bTBPick ? CsMax( fTBPickHeight, fTHeight ) : fTHeight;
}


float CsGBTerrainRoot::GetHeight_Normal( float xWorld, float yWorld, NiPoint3& Normal )
{
	assert_csm4( IsInTerrain( xWorld, yWorld ) == true, _T( "Width = %f, Height = %f\nx = %f, y = %f" ), GetMapSizeWidth(), GetMapSizeHeight(), xWorld, yWorld );

	int nx, ny, index, width;
	float rx, ry, fTBPickHeight, fTHeight;
	float* pfHeight = NULL;	
	bool bTBPick;

	CsGBTerrainLeaf* pLeaf = GetNode( NiPoint3( xWorld, yWorld, 0.0f ) );
	pLeaf->Reset_TBP_Picking();
	bTBPick = pLeaf->GetTBPickHeight( NiPoint2( xWorld, yWorld ), fTBPickHeight );

	xWorld = xWorld*m_fInverseGrid;
	yWorld = yWorld*m_fInverseGrid;
	assert_cs( xWorld >= 0.0f );
	assert_cs( yWorld >= 0.0f );

	nx = (int)xWorld;
	ny = (int)yWorld;
	rx = xWorld-nx;
	ry = yWorld-ny;

	width = m_Info.s_nXVertCount;
	pfHeight = m_FieldInfo.s_pfHeightField;

	index = ny*width + nx;

	if( ( rx != 0.0f )&&( ry != 0.0f ) )
	{
		NiPoint3 kV1( 0, 0, pfHeight[ index ] );
		NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ] );
		NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ] );
		if( rx < ry )
		{
			kV2 = NiPoint3( 1, 1, pfHeight[ index + width + 1 ] );
			kV3 = NiPoint3( 0, 1, pfHeight[ index + width ] );
		}
		

		NiPoint3 kDir( 0, 0, -1 );
		NiPoint3 kOrigin( rx, ry, 100000.0f );
		NiPoint3 kEdge1 = kV2 - kV1;
		NiPoint3 kEdge2 = kV3 - kV1;

		float fDet = kEdge1 * kDir.Cross(kEdge2);
		float r = ( kEdge2 * ( kOrigin - kV1 ).Cross(kEdge1) ) / fDet;
		fTHeight = ( kOrigin + r * kDir ).z;
	}
	else if( rx != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-rx) + pfHeight[ index + 1 ]*rx;
	}
	else if( ry != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-ry) + pfHeight[ index + width ]*ry;
	}
	else
	{
		fTHeight = pfHeight[ index ];
	}

	if( ( nx < m_Info.s_nXVertCount - 1 )&&( ny < m_Info.s_nYVertCount - 1 ) )
	{
		if( ( rx != 0.0f )&&( ry != 0.0f ) )
		{
			NiPoint3 kV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
			NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ]*m_fInverseGrid );
			NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
			if( rx > ry )
			{
				Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );
			}
			else if( rx < ry )
			{
				kV2 = NiPoint3( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
				kV3 = NiPoint3( 0, 1, pfHeight[ index + width ]*m_fInverseGrid );

				Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );
			}
			else
			{
				assert_cs( rx == ry );

				Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );

				NiPoint3 kNV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 0, 1, pfHeight[ index + width ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}
		}
		else if( rx != 0.0f )
		{
			NiPoint3 kV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
			NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ]*m_fInverseGrid );
			NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
			Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );

			if( ny > 0 )
			{
				int nNewIndex = index - width;
				NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 0, 1, pfHeight[ nNewIndex + width ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}
		}
		else if( ry != 0.0f )
		{
			NiPoint3 kV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
			NiPoint3 kV2( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
			NiPoint3 kV3( 0, 1, pfHeight[ index + width ]*m_fInverseGrid );
			Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );

			if( nx > 0 )
			{
				int nNewIndex = index - 1;
				NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 0, pfHeight[ nNewIndex + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}
		}
		else
		{
			NiPoint3 kV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
			NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ]*m_fInverseGrid );
			NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
			Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );

			{
				NiPoint3 kNV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 0, 1, pfHeight[ index + width ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}		

			if( ny > 0 )
			{
				int nNewIndex = index - width;
				NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 0, 1, pfHeight[ nNewIndex + width ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}

			if( nx > 0 )
			{
				int nNewIndex = index - 1;
				NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 0, pfHeight[ nNewIndex + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}

			if( ( nx > 0 )&&( ny > 0 ) )
			{
				int nNewIndex = index - 1 - width;
				{
					NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
					NiPoint3 kNV2( 1, 0, pfHeight[ nNewIndex + 1 ]*m_fInverseGrid );
					NiPoint3 kNV3( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
					Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
				}
				{
					NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
					NiPoint3 kNV2( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
					NiPoint3 kNV3( 0, 1, pfHeight[ nNewIndex + width ]*m_fInverseGrid );
					Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
				}
			}
		}
	}
	else
	{
		Normal = NiPoint3( 0, 0, 1 );
	}
	Normal.Unitize();
	return bTBPick ? CsMax( fTBPickHeight, fTHeight ) : fTHeight;	
}

float CsGBTerrainRoot::GetHeight( float xWorld, float yWorld, float* pfHeightField )
{
	assert_cs( IsInTerrain( xWorld, yWorld ) == true );

	int nx, ny, index, width;
	float rx, ry, fTBPickHeight, fTHeight;
	float* pfHeight = NULL;	
	bool bTBPick;

	CsGBTerrainLeaf* pLeaf = GetNode( NiPoint3( xWorld, yWorld, 0.0f ) );
	pLeaf->Reset_TBP_Picking();
	bTBPick = pLeaf->GetTBPickHeight( NiPoint2( xWorld, yWorld ), fTBPickHeight );	

	xWorld = xWorld*m_fInverseGrid;
	yWorld = yWorld*m_fInverseGrid;
	assert_cs( xWorld >= 0.0f );
	assert_cs( yWorld >= 0.0f );

	nx = (int)xWorld;
	ny = (int)yWorld;
	rx = xWorld-nx;
	ry = yWorld-ny;

	width = m_Info.s_nXVertCount;
	pfHeight = pfHeightField;
	index = ny*width + nx;	
	if( ( rx != 0.0f )&&( ry != 0.0f ) )
	{
		NiPoint3 kV1( 0, 0, pfHeight[ index ] );
		NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ] );
		NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ] );
		if( rx < ry )
		{
			kV2 = NiPoint3( 1, 1, pfHeight[ index + width + 1 ] );
			kV3 = NiPoint3( 0, 1, pfHeight[ index + width ] );
		}

		NiPoint3 kDir( 0, 0, -1 );
		NiPoint3 kOrigin( rx, ry, 100000.0f );
		NiPoint3 kEdge1 = kV2 - kV1;
		NiPoint3 kEdge2 = kV3 - kV1;

		float fDet = kEdge1 * kDir.Cross(kEdge2);
		float r = ( kEdge2 * ( kOrigin - kV1 ).Cross(kEdge1) ) / fDet;
		fTHeight = ( kOrigin + r * kDir ).z;
	}
	else if( rx != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-rx) + pfHeight[ index + 1 ]*rx;					
	}
	else if( ry != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-ry) + pfHeight[ index + width ]*ry;
	}
	else
	{
		fTHeight = pfHeight[ index ];
	}	

	return bTBPick ? CsMax( fTBPickHeight, fTHeight ) : fTHeight;
}



float CsGBTerrainRoot::GetHeight_Normal( float xWorld, float yWorld, NiPoint3& Normal, float* pfHeightField )
{
	assert_csm4( IsInTerrain( xWorld, yWorld ) == true, _T( "Width = %f, Height = %f\nx = %f, y = %f" ), GetMapSizeWidth(), GetMapSizeHeight(), xWorld, yWorld );

	int nx, ny, index, width;
	float rx, ry, fTBPickHeight, fTHeight;
	float* pfHeight = NULL;	
	bool bTBPick;

	CsGBTerrainLeaf* pLeaf = GetNode( NiPoint3( xWorld, yWorld, 0.0f ) );
	pLeaf->Reset_TBP_Picking();
	bTBPick = pLeaf->GetTBPickHeight( NiPoint2( xWorld, yWorld ), fTBPickHeight );

	xWorld = xWorld*m_fInverseGrid;
	yWorld = yWorld*m_fInverseGrid;
	assert_cs( xWorld >= 0.0f );
	assert_cs( yWorld >= 0.0f );

	nx = (int)xWorld;
	ny = (int)yWorld;
	rx = xWorld-nx;
	ry = yWorld-ny;

	width = m_Info.s_nXVertCount;
	pfHeight = pfHeightField;

	index = ny*width + nx;

	if( ( rx != 0.0f )&&( ry != 0.0f ) )
	{
		NiPoint3 kV1( 0, 0, pfHeight[ index ] );
		NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ] );
		NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ] );
		if( rx < ry )
		{
			kV2 = NiPoint3( 1, 1, pfHeight[ index + width + 1 ] );
			kV3 = NiPoint3( 0, 1, pfHeight[ index + width ] );
		}


		NiPoint3 kDir( 0, 0, -1 );
		NiPoint3 kOrigin( rx, ry, 100000.0f );
		NiPoint3 kEdge1 = kV2 - kV1;
		NiPoint3 kEdge2 = kV3 - kV1;

		float fDet = kEdge1 * kDir.Cross(kEdge2);
		float r = ( kEdge2 * ( kOrigin - kV1 ).Cross(kEdge1) ) / fDet;
		fTHeight = ( kOrigin + r * kDir ).z;
	}
	else if( rx != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-rx) + pfHeight[ index + 1 ]*rx;
	}
	else if( ry != 0.0f )
	{
		fTHeight = pfHeight[ index ]*(1.0f-ry) + pfHeight[ index + width ]*ry;
	}
	else
	{
		fTHeight = pfHeight[ index ];
	}

	if( ( nx < m_Info.s_nXVertCount - 1 )&&( ny < m_Info.s_nYVertCount - 1 ) )
	{
		if( ( rx != 0.0f )&&( ry != 0.0f ) )
		{
			NiPoint3 kV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
			NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ]*m_fInverseGrid );
			NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
			if( rx > ry )
			{
				Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );
			}
			else if( rx < ry )
			{
				kV2 = NiPoint3( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
				kV3 = NiPoint3( 0, 1, pfHeight[ index + width ]*m_fInverseGrid );

				Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );
			}
			else
			{
				assert_cs( rx == ry );

				Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );

				NiPoint3 kNV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 0, 1, pfHeight[ index + width ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}
		}
		else if( rx != 0.0f )
		{
			NiPoint3 kV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
			NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ]*m_fInverseGrid );
			NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
			Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );

			if( ny > 0 )
			{
				int nNewIndex = index - width;
				NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 0, 1, pfHeight[ nNewIndex + width ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}
		}
		else if( ry != 0.0f )
		{
			NiPoint3 kV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
			NiPoint3 kV2( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
			NiPoint3 kV3( 0, 1, pfHeight[ index + width ]*m_fInverseGrid );
			Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );

			if( nx > 0 )
			{
				int nNewIndex = index - 1;
				NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 0, pfHeight[ nNewIndex + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}
		}
		else
		{
			NiPoint3 kV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
			NiPoint3 kV2( 1, 0, pfHeight[ index + 1 ]*m_fInverseGrid );
			NiPoint3 kV3( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
			Normal = ( kV2 - kV1 ).Cross( kV3 - kV1 );

			{
				NiPoint3 kNV1( 0, 0, pfHeight[ index ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 1, pfHeight[ index + width + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 0, 1, pfHeight[ index + width ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}		

			if( ny > 0 )
			{
				int nNewIndex = index - width;
				NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 0, 1, pfHeight[ nNewIndex + width ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}

			if( nx > 0 )
			{
				int nNewIndex = index - 1;
				NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
				NiPoint3 kNV2( 1, 0, pfHeight[ nNewIndex + 1 ]*m_fInverseGrid );
				NiPoint3 kNV3( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
				Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
			}

			if( ( nx > 0 )&&( ny > 0 ) )
			{
				int nNewIndex = index - 1 - width;
				{
					NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
					NiPoint3 kNV2( 1, 0, pfHeight[ nNewIndex + 1 ]*m_fInverseGrid );
					NiPoint3 kNV3( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
					Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
				}
				{
					NiPoint3 kNV1( 0, 0, pfHeight[ nNewIndex ]*m_fInverseGrid );
					NiPoint3 kNV2( 1, 1, pfHeight[ nNewIndex + width + 1 ]*m_fInverseGrid );
					NiPoint3 kNV3( 0, 1, pfHeight[ nNewIndex + width ]*m_fInverseGrid );
					Normal += ( kNV2 - kNV1 ).Cross( kNV3 - kNV1 );
				}
			}
		}
	}
	else
	{
		Normal = NiPoint3( 0, 0, 1 );
	}
	Normal.Unitize();
	return bTBPick ? CsMax( fTBPickHeight, fTHeight ) : fTHeight;	
}

bool CsGBTerrainRoot::CheckActorObject( NiPoint3 niStartPos, NiPoint3 niEndPos )
{
	if( m_listActorObj.empty() )
		return true;

	nsCSGBFUNC::sTERRAIN_BOUND vv;
	vv.s_ptMin.x = niStartPos.x;
	vv.s_ptMin.y = niStartPos.y;
	vv.s_ptMax.x = niEndPos.x;
	vv.s_ptMax.y = niEndPos.y;

	LIST_GBTERRAIN_OBJECT_IT it = m_listActorObj.begin();
	LIST_GBTERRAIN_OBJECT_IT itEnd = m_listActorObj.end();
	
	for( ; it!=itEnd; it++ )
	{
		CsGBObject* pObject = *it;
		CsGBObjProp_Actor* pProp = (CsGBObjProp_Actor*)(pObject)->GetProp( CsGBObjProp::PROP_ACTOR );

		DWORD dwAniID = pProp->GetAnimation();
		if( dwAniID == CsGBObjProp_Actor::ACT_ANIMATION1 )
		{
			if( pObject->GetInfo()->s_Bound.CrossBoundCheck( vv ) )
				return false;
		}
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////

int	CsGBTerrainRoot::sINFO::GetTextureRightFigure( int nMPPG )
{
	return s_nXVertCountInNode*nMPPG - 1;
}

int	CsGBTerrainRoot::sINFO::GetTextureTopFigure( int nMPPG )
{
	return ( s_nInitMapSize - s_nYVertCountInNode )*nMPPG;
}

int	CsGBTerrainRoot::sINFO::GetTextureBottomFigure( int nMPPG )
{
	return ( s_nInitMapSize - 1 )*nMPPG - 1;
}

int	CsGBTerrainRoot::sINFO::GetTextureLeftFigure( int nMPPG )
{
	return nMPPG;
}

int CsGBTerrainRoot::sINFO::CalNodeSizeX() const
{ 
	return (s_nXVertCountInNode-1)*s_nGridSize; 
}

int CsGBTerrainRoot::sINFO::CalNodeSizeY() const
{ 
	return (s_nYVertCountInNode-1)*s_nGridSize; 
}

int CsGBTerrainRoot::sINFO::CalSizeX() const
{ 
	return (s_nXVertCount-1)*s_nGridSize; 
}

int CsGBTerrainRoot::sINFO::CalSizeY() const
{ 
	return (s_nYVertCount-1)*s_nGridSize; 
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CsGBTerrainRoot::IsInTerrain( float xWorld, float yWorld )
{
	return m_RootNode.GetInfo()->s_Bound.InBoundCheck( xWorld, yWorld );
}

float CsGBTerrainRoot::GetMapSizeWidth() const
{ 
	return m_fWidth; 
}

float CsGBTerrainRoot::GetMapSizeHeight() const
{ 
	return m_fHeight; 
}

float CsGBTerrainRoot::GetMinimapScale() const
{ 
	return m_fMinimapScale; 
}

CsGBTerrainLeaf* CsGBTerrainRoot::GetNode( int nObjectIndex )
{
	assert_cs( nObjectIndex < m_Info.s_nTotalLeafCount );
	return m_RootNode.GetNode( nObjectIndex % m_Info.s_nNodeCountInSide, nObjectIndex / m_Info.s_nNodeCountInSide );
}

CsGBTerrainLeaf* CsGBTerrainRoot::GetNode( int nX, int nY )
{
	return m_RootNode.GetNode( nX, nY );
}

CsGBTerrainLeaf* CsGBTerrainRoot::GetNode( NiPoint3 WorldPos )
{	
	return m_RootNode.GetNode( (int)( WorldPos.x*m_fInverseXNodeSize ), (int)( WorldPos.y*m_fInverseYNodeSize ) );
}

//==========================================================================================
//
//		Node
//
//==========================================================================================

CsNiNodePtr CsGBTerrainRoot::GetCloneBaseNiNode()
{
	return NiDynamicCast( CsNiNode, m_pBaseNiNode->Clone() );
}

CsNiNodePtr CsGBTerrainRoot::GetDeepCloneBaseNiNode()
{
	return NiDynamicCast( CsNiNode, m_pBaseNiNode->CreateDeepCopy() );
}

void	CsGBTerrainRoot::SetViewerPos( NiPoint3 ptPos )
{
	m_RootNode.SetViewerPos( NiPoint2( ptPos.x, ptPos.y ) );
	m_SkyBox.SetViewerPos( ptPos );
}

void	CsGBTerrainRoot::SetViewerPos_Tool( NiPoint3 ptPos )
{	
	m_SkyBox.SetViewerPos( ptPos );
}


//==========================================================================================
//
//		Picking
//
//==========================================================================================
bool CsGBTerrainRoot::Pick_IsEnable( NiCamera* pNiCamera, int MouseX, int MouseY )
{
	NiPoint3 origin, dir;
	pNiCamera->WindowPointToRay( MouseX, MouseY, origin, dir );

	return Pick_IsEnable( origin, dir );
}

bool CsGBTerrainRoot::Pick_IsEnable( NiPoint3 origin, NiPoint3 dir )
{
	if( m_mapNoPick.empty() == true
		&& m_listActorObj.size() == 0 )
		return true;

	// Actor Object 검사
	CsGBPick pick;
	// 	LIST_GBTERRAIN_OBJECT* ListActor = nsCsGBTerrain::g_pCurRoot->GetList_ActorObj();
	// 	if( ListActor->empty() == false )
	// 	{
	// 		LIST_GBTERRAIN_OBJECT_IT it = ListActor->begin();
	// 		LIST_GBTERRAIN_OBJECT_IT itEnd = ListActor->end();
	// 		CsGBObject* pObject = NULL;
	// 		for( ; it!=itEnd; ++it )
	// 		{
	// 			pObject = *it;
	// 			CsGBObjProp_Actor* pProp = (CsGBObjProp_Actor*)(pObject)->GetProp( CsGBObjProp::PROP_ACTOR );
	// 
	// 			DWORD dwAniID = pProp->GetAnimation();
	// 			if( dwAniID == CsGBObjProp_Actor::ACT_ANIMATION2
	// 				|| dwAniID == CsGBObjProp_Actor::ACT_ANIMATION3 )	// 열린 상태이면
	// 				break;	// 검사 안함
	// 
	// 			int nColl = (int)pObject->GetInfo()->s_Bound.InBoundCheck( origin.x, origin.y );
	// 			if( nColl == 1 )
	// 				return false;
	// 		}
	// 	}

	MAP_GBTERRAIN_OBJECT_IT it = m_mapNoPick.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapNoPick.end();
	CsGBObject* pObject = NULL;
	for( ; it!=itEnd; ++it )
	{
		pObject = it->second;
		pObject->ResetPicking();
		pObject->Picking( &pick, origin, dir );
		if( pick.GetRecord() )
		{
			return false;
		}
	}
	return true;
}

NiPick::Record* CsGBTerrainRoot::Pick_Terrain( NiPoint3 origin, NiPoint3 dir )
{
	if( Pick_IsEnable( origin, dir ) == false )
		return NULL;

 	CsGBPick pick;
	m_RootNode.Reset_TB_TBP_CAP_Picking();
	m_RootNode.Pick_T_TB_TBP_CAP( &pick, origin, dir );

// 	m_RootNode.Reset_TB_TBP_Picking();
// 	m_RootNode.Pick_Terrain( &pick, origin, dir );
	return pick.GetRecord();
}

NiPick::Record* CsGBTerrainRoot::Pick_Terrain( NiCamera* pNiCamera, int MouseX, int MouseY )
{
	NiPoint3 origin, dir;
	pNiCamera->WindowPointToRay( MouseX, MouseY, origin, dir );

	return Pick_Terrain( origin, dir );
}

NiPick::Record* CsGBTerrainRoot::Pick_TBP( NiPoint3 origin, NiPoint3 dir )
{
	m_RootNode.Reset_TBP_Picking();

	CsGBPick pick;
	m_RootNode.Pick_TBP( &pick, origin, dir );

	return pick.GetRecord();
}

NiPick::Record* CsGBTerrainRoot::Pick_T_TB_TBP_ByTool( NiPoint3 origin, NiPoint3 dir )
{
	m_RootNode.Reset_TB_TBP_Picking();

	CsGBPick pick;
	m_RootNode.Pick_T_TB_TBP_ByTool( &pick, origin, dir );

	return pick.GetRecord();
}

NiPick::Record* CsGBTerrainRoot::Pick_T_TB_TBP_ByTool( NiCamera* pNiCamera, int MouseX, int MouseY )
{
	m_RootNode.Reset_TB_TBP_Picking();

	NiPoint3 origin, dir;
	pNiCamera->WindowPointToRay( MouseX, MouseY, origin, dir );

	CsGBPick pick;
	m_RootNode.Pick_T_TB_TBP_ByTool( &pick, origin, dir );

	return pick.GetRecord();
}

NiPick::Record* CsGBTerrainRoot::Pick_T_ByTool( NiCamera* pNiCamera, int MouseX, int MouseY )
{
	NiPoint3 origin, dir;
	pNiCamera->WindowPointToRay( MouseX, MouseY, origin, dir );

	CsGBPick pick;
	m_RootNode.Pick_T_ByTool( &pick, origin, dir );

	return pick.GetRecord();
}

NiPick::Record* CsGBTerrainRoot::Pick_T_TB_TBP_CAP( NiPoint3 origin, NiPoint3 dir )
{
	m_RootNode.Reset_TB_TBP_CAP_Picking();

	CsGBPick pick;
	m_RootNode.Pick_T_TB_TBP_CAP( &pick, origin, dir );

	return pick.GetRecord();
}

NiPick::Record* CsGBTerrainRoot::Pick_T_TB_TBP_CAP_ByTool( NiPoint3 origin, NiPoint3 dir )
{
	m_RootNode.Reset_TB_TBP_CAP_Picking();

	CsGBPick pick;
	m_RootNode.Pick_T_TB_TBP_CAP_ByTool( &pick, origin, dir );

	return pick.GetRecord();
}
