
#include "stdafx.h"
#include "CsGBTerrainNode.h"


CsGBTerrainNode::CsGBTerrainNode():m_nQuadKey(0),m_eQuadID(QI_NONE),m_pRootNode(NULL),m_pParentNode(NULL)
,m_pNiNode(NULL),m_bVisible(false),m_bEnableRender(false)
{
	memset(m_cQuadKey,0, sizeof(char)*32);

	for( int i=QI_LD; i<QI_COUNT; ++i )
		m_pChildNode[ i ] = NULL;
}

CsGBTerrainNode::~CsGBTerrainNode()
{

}

void CsGBTerrainNode::Delete()
{
	// ==== QuadTree
	m_eQuadID		= QI_NONE;
	m_pRootNode		= NULL;
	m_pParentNode	= NULL;
	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] != NULL )
		{
			m_pChildNode[ i ]->Delete();
			SAFE_NIDELETE( m_pChildNode[ i ] );
		}	
	}
	// ==== NiNode
	memset(m_cQuadKey,0, sizeof(char)*32);
	m_pNiNode = 0;	
	m_Info.Reset();
	m_bVisible = false;	
	m_bEnableRender = false;
}

void CsGBTerrainNode::_Init()
{
	assert_cs( m_eQuadID == QI_NONE );
	assert_cs( m_pRootNode == NULL );
	assert_cs( m_pParentNode == NULL );
	assert_cs( m_pChildNode[ QI_LD ] == NULL );
	assert_cs( m_pChildNode[ QI_RD ] == NULL );
	assert_cs( m_pChildNode[ QI_LT ] == NULL );
	assert_cs( m_pChildNode[ QI_RT ] == NULL );
	assert_cs( m_pNiNode == NULL );

	memset(m_cQuadKey,0, sizeof(char)*32);
	m_bVisible = true;	
	m_bEnableRender = false;
	
	m_Info.Reset();
}

void CsGBTerrainNode::UpdateTerrain( float fElapsedTime )
{
	if( IsVisible() == false )
		return;

	if( CAMERA_ST.UserCulling( m_pNiNode ) == false )
		return;

	m_bEnableRender = true;

	if( IsLeaf() )
		return;

	for( int n = QI_LD; n < QI_COUNT; ++n )
	{
		if( m_pChildNode[ n ] )
			m_pChildNode[ n ]->UpdateTerrain( fElapsedTime );
	}
}

void CsGBTerrainNode::UpdateObject( float fElapsedTime )
{
	assert_cs( IsLeaf() == false );

	if( IsEnableRender() == false )
		return;

	if( CAMERA_ST.UserCulling( m_pNiNode ) == false )
		return;

	for( int n = QI_LD; n < QI_COUNT; ++n )
	{
		if( m_pChildNode[ n ] )
			m_pChildNode[ n ]->UpdateObject( fElapsedTime );
	}
}

void CsGBTerrainNode::UpdateObject_For_Shadow( float fElapsedTime )
{
	assert_cs( IsLeaf() == false );

	for( int n = QI_LD; n < QI_COUNT; ++n )
	{
		if( m_pChildNode[ n ] )
			m_pChildNode[ n ]->UpdateObject_For_Shadow( fElapsedTime );
	}
}

void CsGBTerrainNode::UpdateObject_For_Shadow_ExceptionObject( float fElapsedTime, CsGBObject* pExceptionObj )
{
	assert_cs( IsLeaf() == false );

	for( int n = QI_LD; n < QI_COUNT; ++n )
	{
		if( m_pChildNode[ n ] )
			m_pChildNode[ n ]->UpdateObject_For_Shadow_ExceptionObject( fElapsedTime, pExceptionObj );
	}
}

//void CsGBTerrainNode::Update_RenderObject( float fElapsedTime )
//{
//	assert_cs( IsLeaf() == false );
//	if( m_bEnableRender == false )
//		return;
//
//	m_pChildNode[ QI_LD ]->Update_RenderObject( fElapsedTime );
//	m_pChildNode[ QI_RD ]->Update_RenderObject( fElapsedTime );
//	m_pChildNode[ QI_LT ]->Update_RenderObject( fElapsedTime );
//	m_pChildNode[ QI_RT ]->Update_RenderObject( fElapsedTime );
//}
//
//void CsGBTerrainNode::Update_RenderObject_TBP( float fElapsedTime )
//{
//	assert_cs( IsLeaf() == false );
//	if( m_bEnableRender == false )
//		return;
//
//	m_pChildNode[ QI_LD ]->Update_RenderObject_TBP( fElapsedTime );
//	m_pChildNode[ QI_RD ]->Update_RenderObject_TBP( fElapsedTime );
//	m_pChildNode[ QI_LT ]->Update_RenderObject_TBP( fElapsedTime );
//	m_pChildNode[ QI_RT ]->Update_RenderObject_TBP( fElapsedTime );
//}
//
//void CsGBTerrainNode::Update_RenderObject_NoClipping( float fElapsedTime )
//{
//	assert_cs( IsLeaf() == false );
//	if( m_bEnableRender == false )
//		return;
//
//	m_pChildNode[ QI_LD ]->Update_RenderObject_NoClipping( fElapsedTime );
//	m_pChildNode[ QI_RD ]->Update_RenderObject_NoClipping( fElapsedTime );
//	m_pChildNode[ QI_LT ]->Update_RenderObject_NoClipping( fElapsedTime );
//	m_pChildNode[ QI_RT ]->Update_RenderObject_NoClipping( fElapsedTime );
//}

CsGBTerrainNode* CsGBTerrainNode::Create( eQUADT_ID const& eQuadID, CsNiNodePtr pBaseNiNode, CsGBTerrainNode* pRootNode, CsGBTerrainNode* pParentNode,
										 int const& nTreeLevel, int const& nNodeLevel )
{
	assert_cs( nTreeLevel > nNodeLevel );
	assert_cs( m_eQuadID == QI_NONE );
	assert_cs( m_pNiNode == NULL );

	_Init();

	m_eQuadID = eQuadID;
	m_pRootNode = pRootNode;
	m_pParentNode = pParentNode;
	m_nNodeLevel = nNodeLevel;
	m_nSerchIndexConstant = pow( 2, nTreeLevel - m_nNodeLevel - 1 );

	_CreateNiNode( m_eQuadID );

	if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_MAKE )
	{
		// ExtraData Load
		_LoadExtraData( pBaseNiNode );
	}
	_CreateChild( nTreeLevel, pBaseNiNode );

	return this;
}


void CsGBTerrainNode::Render()
{
	CsNiDrawTerrainScene();
}

void CsGBTerrainNode::Render_Shader( char* cShader )
{
	CsNiDrawTerrainScene( cShader );
}

CsGBTerrainLeaf* CsGBTerrainNode::GetNode( int const& nX, int const& nY )
{
	// 가진자식이 리프 노드면 1이다
	if( m_nSerchIndexConstant == 1 ){
		assert_cs( m_pChildNode[ 0 ]->IsLeaf() == true );
		if( nY < m_nSerchIndexConstant )
		{
			if( nX < m_nSerchIndexConstant )	return (CsGBTerrainLeaf*)m_pChildNode[ QI_LD ];
			else								return (CsGBTerrainLeaf*)m_pChildNode[ QI_RD ];
		}
		else
		{
			if( nX < m_nSerchIndexConstant )	return (CsGBTerrainLeaf*)m_pChildNode[ QI_LT ];
			else								return (CsGBTerrainLeaf*)m_pChildNode[ QI_RT ];
		}
	}

	if( nY < m_nSerchIndexConstant )
	{
		if( nX < m_nSerchIndexConstant )
			return m_pChildNode[ QI_LD ]->GetNode( nX, nY );
		else
			return m_pChildNode[ QI_RD ]->GetNode( nX - m_nSerchIndexConstant, nY );
	}
	else{
		if( nX < m_nSerchIndexConstant )
			return m_pChildNode[ QI_LT ]->GetNode( nX, nY - m_nSerchIndexConstant );		
		else
			return m_pChildNode[ QI_RT ]->GetNode( nX - m_nSerchIndexConstant, nY - m_nSerchIndexConstant );
	}
}

void CsGBTerrainNode::_CreateChild( int const& nTreeLevel, CsNiNodePtr pBaseNiNode )
{	
	if( nTreeLevel > m_nNodeLevel + 1 )
	{
		CsGBTerrainNode* pNewNode = NiNew CsGBTerrainNode;
		m_pChildNode[ QI_LD ] = pNewNode->Create( QI_LD, pBaseNiNode, m_pRootNode, this, nTreeLevel, m_nNodeLevel + 1 );
		pNewNode = NiNew CsGBTerrainNode;
		m_pChildNode[ QI_RD ] = pNewNode->Create( QI_RD, pBaseNiNode, m_pRootNode, this, nTreeLevel, m_nNodeLevel + 1 );
		pNewNode = NiNew CsGBTerrainNode;
		m_pChildNode[ QI_LT ] = pNewNode->Create( QI_LT, pBaseNiNode, m_pRootNode, this, nTreeLevel, m_nNodeLevel + 1 );
		pNewNode = NiNew CsGBTerrainNode;
		m_pChildNode[ QI_RT ] = pNewNode->Create( QI_RT, pBaseNiNode, m_pRootNode, this, nTreeLevel, m_nNodeLevel + 1 );
	}
	else
	{
		assert_cs( nTreeLevel == ( m_nNodeLevel + 1 ) );
		CsGBTerrainLeaf* pNewLeaf = NiNew CsGBTerrainLeaf;
		m_pChildNode[ QI_LD ] = pNewLeaf->Create( QI_LD, pBaseNiNode, m_pRootNode, this, nTreeLevel, m_nNodeLevel + 1 );
		pNewLeaf = NiNew CsGBTerrainLeaf;
		m_pChildNode[ QI_RD ] = pNewLeaf->Create( QI_RD, pBaseNiNode, m_pRootNode, this, nTreeLevel, m_nNodeLevel + 1 );
		pNewLeaf = NiNew CsGBTerrainLeaf;
		m_pChildNode[ QI_LT ] = pNewLeaf->Create( QI_LT, pBaseNiNode, m_pRootNode, this, nTreeLevel, m_nNodeLevel + 1 );
		pNewLeaf = NiNew CsGBTerrainLeaf;
		m_pChildNode[ QI_RT ] = pNewLeaf->Create( QI_RT, pBaseNiNode, m_pRootNode, this, nTreeLevel, m_nNodeLevel + 1 );
	}
}

//==========================================================================================
//
//		Node State
//
//==========================================================================================

void CsGBTerrainNode::SetViewerPos( NiPoint2 ptPos )
{
	assert_cs( IsLeaf() == false );
	if( IsExistTerrain() == false )
		return;

	// 비지블 거리를 넘어 섰다	
	if( ( m_Info.s_ptCenterPos - ptPos ).Length() > m_Info.s_fHalfRadius + nsCsGBTerrain::g_TRView.g_fViewVisibleDist )
	{		
		SetVisible( false );
		return;
	}
	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->SetViewerPos( ptPos );
	}
}


//==========================================================================================
//
//		Terrain
//
//==========================================================================================
void CsGBTerrainNode::SetVisible( bool bVisible )
{
	if( IsExistTerrain() == false )
		return;

	if( m_bVisible == bVisible )
		return;

	m_bVisible = bVisible;
	if( bVisible == true )
	{
		if( m_pParentNode != NULL )
		{
		#ifdef _DEBUG
			for( unsigned int i=0; i<m_pParentNode->GetNiNode()->GetArrayCount(); ++i )
			{
				assert_cs( m_pParentNode->GetNiNode()->GetAt( i ) != m_pNiNode );
			}
		#endif /*_DEBUG*/
			m_pParentNode->GetNiNode()->AttachChild( m_pNiNode );
			m_pParentNode->SetVisible( m_bVisible );
		}
	}
	else
	{
		if( m_pParentNode != NULL )
		{
			m_pParentNode->GetNiNode()->DetachChild( m_pNiNode );
		}

		if( IsLeaf() == false )
		{
			for( int i=QI_LD; i<QI_COUNT; ++i )
			{
				if( m_pChildNode[ i ] )
					m_pChildNode[ i ]->SetVisible( m_bVisible );
			}
		}
	}
}
 
bool CsGBTerrainNode::IsVisible()
{ 
	return m_bVisible; 
}

//==========================================================================================
//
//		NiNode
//
//==========================================================================================

void CsGBTerrainNode::BeginRenderTR()
{
	if( CAMERA_ST.UserCulling( m_pNiNode ) == false )
		return;

	m_bEnableRender = true;

	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->BeginRenderTR();
	}
}

void CsGBTerrainNode::EndRender()
{
	if( m_bEnableRender == false )
		return;

	m_bEnableRender = false;

	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->EndRender();
	}
}
void CsGBTerrainNode::UpdateBuffer()
{ 
	assert_cs( false ); 
}

CsNiNodePtr CsGBTerrainNode::GetNiNode()
{ 
	return m_pNiNode; 
}

CsGBTerrainNode* CsGBTerrainNode::GetChildNode( DWORD dwChildID )
{ 
	if( QI_COUNT <= dwChildID )
		return NULL;

	return m_pChildNode[ dwChildID ]; 
}

bool CsGBTerrainNode::IsEnableRender()
{ 
	return m_bEnableRender; 
}

void CsGBTerrainNode::_CreateNiNode( eQUADT_ID const& eQuadID )
{
	m_pNiNode = 0;

	m_pNiNode = NiNew CsNiNode;
	m_pNiNode->SetParentCsTNode( this );

	if( m_pParentNode != NULL )
	{
		m_pParentNode->GetNiNode()->AttachChild( m_pNiNode );		
		sprintf_s( m_cQuadKey, 32, "%s_%d", m_pParentNode->GetCharQuadKey(), eQuadID );		// QuadKey
		m_nQuadKey = m_pParentNode->GetIntQuadKey()*10 + eQuadID;
	}
	else
	{
		sprintf_s( m_cQuadKey, 32, "_%d", eQuadID );									// QuadKey
		m_nQuadKey = eQuadID;
	}

	// 이름 입력
	char szName[ 32 ] = {0, };
	switch( eQuadID )
	{
	case QI_ROOT:	sprintf_s( szName, 32, "%d_QI_ROOT", m_nNodeLevel );	break;
	case QI_LD:		sprintf_s( szName, 32, "%d_QI_LD", m_nNodeLevel );		break;
	case QI_RD:		sprintf_s( szName, 32, "%d_QI_RD", m_nNodeLevel );		break;
	case QI_LT:		sprintf_s( szName, 32, "%d_QI_LT", m_nNodeLevel );		break;
	case QI_RT:		sprintf_s( szName, 32, "%d_QI_RT", m_nNodeLevel );		break;
	}
	strcat_s( szName, 32, m_cQuadKey );	
	m_pNiNode->SetName( szName );	
}

//==========================================================================================
//
//		Texture Filter Mode
//
//==========================================================================================

void CsGBTerrainNode::ResetTextureFilterMode()
{
	assert_cs( IsLeaf() == false );
	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->ResetTextureFilterMode();
	}
}

//==========================================================================================
//
//		Shader
//
//==========================================================================================

void CsGBTerrainNode::ResetShader()
{
	assert_cs( IsLeaf() == false );
	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->ResetShader();
	}
}

//==========================================================================================
//
//		Object
//
//==========================================================================================

//void CsGBTerrainNode::ResistObjectVisibleArray( NiCamera* pNiCamera, NiCullingProcess* pCuller )
//{
//	if( m_bRendered == false )
//		return;
//
//	assert_cs( IsLeaf() == false );
//	for( int i=0; i<QI_COUNT; ++i )
//	{
//		assert_cs( m_pChildNode[ i ] != NULL );
//		m_pChildNode[ i ]->ResistObjectVisibleArray( pNiCamera, pCuller );
//	}
//}

bool CsGBTerrainNode::ResistObject( CsGBObject* pObject )
{
	assert_cs( IsLeaf() == false );
	if( !m_Info.s_Bound.CrossBoundCheck( pObject->GetInfo()->s_Bound ) )
		return false;

	bool bReturn = false;
	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		SAFE_POINTER_CON( m_pChildNode[ i ] );
		if( m_pChildNode[ i ]->ResistObject(pObject) )
			bReturn = true;
	}
	return bReturn;
}

CsGBObject* CsGBTerrainNode::FindCsGBObject( NiNode* pNiNode )
{
	SAFE_POINTER_RETVAL( pNiNode, NULL );

	assert_cs( IsLeaf() == false );

	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		SAFE_POINTER_CON( m_pChildNode[ i ] );
		CsGBObject* pObject = m_pChildNode[ i ]->FindCsGBObject(pNiNode);
		if( pObject )
			return pObject;
	}

	return NULL;
}

CsGBObject* CsGBTerrainNode::FindCsGBObject( DWORD dwUniqObjectID )
{
	assert_cs( IsLeaf() == false );

	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		SAFE_POINTER_CON( m_pChildNode[ i ] );
		CsGBObject* pObject = m_pChildNode[ i ]->FindCsGBObject(dwUniqObjectID);
		if( pObject )
			return pObject;
	}

	return NULL;
}

void CsGBTerrainNode::DeleteCsGBObject( DWORD dwUniqObjectID )
{
	assert_cs( IsLeaf() == false );
	for( int i=QI_LD; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->DeleteCsGBObject( dwUniqObjectID );
	}
}

//void CsGBTerrainNode::ObjectResetCallRender_ForRender()
//{
//	assert_cs( IsLeaf() == false );
//	for( int i=0; i<QI_COUNT; ++i )
//	{
//		assert_cs( m_pChildNode[ i ] != NULL );
//		m_pChildNode[ i ]->ObjectResetCallRender_ForRender();
//	}
//}
//
//void CsGBTerrainNode::ObjectResetCallRender_ForUpdate()
//{
//	assert_cs( IsLeaf() == false );
//	for( int i=0; i<QI_COUNT; ++i )
//	{
//		assert_cs( m_pChildNode[ i ] != NULL );
//		m_pChildNode[ i ]->ObjectResetCallRender_ForUpdate();
//	}
//}

//==========================================================================================
//
//		Light
//
//==========================================================================================
void CsGBTerrainNode::ApplyLight_Object( CsGBLight* pLight )
{
	assert_cs( IsLeaf() == false );
	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->ApplyLight_Object( pLight );
	}
}

void CsGBTerrainNode::ReleaseLight_Object( CsGBLight* pLight )
{
	assert_cs( IsLeaf() == false );
	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->ReleaseLight_Object( pLight );
	}
}

//==========================================================================================
//
//		Info
//
//==========================================================================================
void CsGBTerrainNode::_SetExistTerrain()
{
	if( m_Info.s_bExistChild == true )
		return;

	m_Info.s_bExistChild = true;
	if( m_pParentNode != NULL )
	{
		m_pParentNode->_SetExistTerrain();
	}
}

void CsGBTerrainNode::CalBound( nsCSGBFUNC::sTERRAIN_BOUND Bound )
{
	if( m_Info.s_Bound.s_ptMin.x > Bound.s_ptMin.x )
		m_Info.s_Bound.s_ptMin.x = Bound.s_ptMin.x;
	if( m_Info.s_Bound.s_ptMax.x < Bound.s_ptMax.x )
		m_Info.s_Bound.s_ptMax.x = Bound.s_ptMax.x;
	if( m_Info.s_Bound.s_ptMin.y > Bound.s_ptMin.y )
		m_Info.s_Bound.s_ptMin.y = Bound.s_ptMin.y;
	if( m_Info.s_Bound.s_ptMax.y < Bound.s_ptMax.y )
		m_Info.s_Bound.s_ptMax.y = Bound.s_ptMax.y;

	m_Info.CalCenter();

	if( m_pParentNode != NULL )
	{
		m_pParentNode->CalBound( m_Info.s_Bound );
	}

	assert_cs( m_Info.s_Bound.s_ptMin.x < m_Info.s_Bound.s_ptMax.x );
	assert_cs( m_Info.s_Bound.s_ptMin.y < m_Info.s_Bound.s_ptMax.y );
}

//==========================================================================================
//
//		Picking
//
//==========================================================================================


//==========================================================================================
//
//		Quad Tree
//
//==========================================================================================



//==========================================================================================
//
//		Extra Data
//
//==========================================================================================

void CsGBTerrainNode::SaveExtraData( CsNiNodePtr pBaseNiNode )
{
	NiBinaryExtraData* pExtData = NULL;
	unsigned int uiSize = 0;

	// 인포 저장
	sINFO* pNodeInfo = NiAlloc( sINFO, 1 );
	uiSize = sizeof( sINFO );
	memcpy( pNodeInfo, &m_Info, uiSize );
	pExtData = NiNew NiBinaryExtraData( uiSize, (char*)pNodeInfo );	
	pBaseNiNode->AddExtraData( m_cQuadKey, pExtData );

	// 자식들도 전부 저장
	if( IsLeaf() == false )
	{
		for( int i=0; i<QI_COUNT; ++i )
		{
			assert_cs( m_pChildNode[ i ] != NULL );
			m_pChildNode[ i ]->SaveExtraData( pBaseNiNode );
		}
	}
}

void CsGBTerrainNode::_LoadExtraData( CsNiNodePtr pBaseNiNode )
{
	NiBinaryExtraData* pExtData = NULL;
	unsigned int uiSize = 0;
	pExtData = (NiBinaryExtraData*)pBaseNiNode->GetExtraData( m_cQuadKey );
	assert_cs( pExtData != NULL );
	char* pData;
	pExtData->GetValue( uiSize, pData );
	assert_cs( uiSize == sizeof( sINFO ) );
	memcpy( &m_Info, pData, uiSize );	
}


void CsGBTerrainNode::SaveToolExtraData( CsNiNodePtr pBaseNiNode )
{	
	assert_cs( IsLeaf() == false );

	for( int i=0; i<QI_COUNT; ++i )
	{
		assert_cs( m_pChildNode[ i ] != NULL );
		m_pChildNode[ i ]->SaveToolExtraData( pBaseNiNode );
	}
}


void CsGBTerrainNode::LoadToolExtraData( CsNiNodePtr pBaseNiNode )
{	
	assert_cs( IsLeaf() == false );

	for( int i=0; i<QI_COUNT; ++i )
	{
		assert_cs( m_pChildNode[ i ] != NULL );
		m_pChildNode[ i ]->LoadToolExtraData( pBaseNiNode );
	}
}

//////////////////////////////////////////////////////////////////////////

void CsGBTerrainNode::ObjectResetCallReady()
{
	assert_cs( IsLeaf() == false );
	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->ObjectResetCallReady();
	}
}

void CsGBTerrainNode::SaveObjectResetCallReady()
{
	assert_cs( IsLeaf() == false );
	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->SaveObjectResetCallReady();
	}
}

void CsGBTerrainNode::LoadObjectResetCallReady()
{
	assert_cs( IsLeaf() == false );
	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->LoadObjectResetCallReady();
	}
}

// Quest
void CsGBTerrainNode::CheckQuestObject()
{
	assert_cs( IsLeaf() == false );
	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->CheckQuestObject();
	}
}

void CsGBTerrainNode::CheckQuestObject( int nCompQuest )
{
	assert_cs( IsLeaf() == false );
	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->CheckQuestObject( nCompQuest );
	}
}


//==========================================================================================
//
//		Picking
//
//==========================================================================================

void CsGBTerrainNode::Pick_Terrain( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	assert_cs( IsLeaf() == false );
	if( ( m_bVisible == false )||( IsExistTerrain() == false ) )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Pick_Terrain( pCsPick, origin, dir );
	}
}

void CsGBTerrainNode::Pick_TBP( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	assert_cs( IsLeaf() == false );
	if( m_bVisible == false )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Pick_TBP( pCsPick, origin, dir );
	}
}

void CsGBTerrainNode::Pick_T_TB_TBP_ByTool( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	assert_cs( IsLeaf() == false );
	if( ( m_bVisible == false )||( IsExistTerrain() == false ) )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Pick_T_TB_TBP_ByTool( pCsPick, origin, dir );
	}
}

void CsGBTerrainNode::Pick_T_ByTool( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	assert_cs( IsLeaf() == false );
	if( ( m_bVisible == false )||( IsExistTerrain() == false ) )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Pick_T_ByTool( pCsPick, origin, dir );
	}
}

void CsGBTerrainNode::Pick_T_TB_TBP_CAP( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	assert_cs( IsLeaf() == false );
	if( ( m_bVisible == false )||( IsExistTerrain() == false ) )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Pick_T_TB_TBP_CAP( pCsPick, origin, dir );
	}
}

void CsGBTerrainNode::Pick_T_TB_TBP_CAP_ByTool( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	assert_cs( IsLeaf() == false );
	if( ( m_bVisible == false )||( IsExistTerrain() == false ) )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Pick_T_TB_TBP_CAP_ByTool( pCsPick, origin, dir );
	}
}

void CsGBTerrainNode::ResetObject_Picking()
{
	assert_cs( IsLeaf() == false );
	if( IsExistTerrain() == false )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->ResetObject_Picking();
	}
}

void CsGBTerrainNode::Reset_TBP_Picking()
{
	assert_cs( IsLeaf() == false );
	if( IsExistTerrain() == false )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Reset_TBP_Picking();
	}
}


void CsGBTerrainNode::Reset_TBP_CAP_Picking()
{
	assert_cs( IsLeaf() == false );
	if( IsExistTerrain() == false )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Reset_TBP_CAP_Picking();
	}
}

void CsGBTerrainNode::Reset_TB_TBP_Picking()
{
	assert_cs( IsLeaf() == false );
	if( IsExistTerrain() == false )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Reset_TB_TBP_Picking();
	}
}

void CsGBTerrainNode::Reset_TB_TBP_CAP_Picking()
{
	assert_cs( IsLeaf() == false );
	if( IsExistTerrain() == false )
		return;

	for( int i=0; i<QI_COUNT; ++i )
	{
		if( m_pChildNode[ i ] )
			m_pChildNode[ i ]->Reset_TB_TBP_CAP_Picking();
	}
}