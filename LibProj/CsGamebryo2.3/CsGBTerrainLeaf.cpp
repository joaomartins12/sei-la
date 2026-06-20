
#include "stdafx.h"
#include "CsGBTerrainLeaf.h"

DWORD CsGBTerrainLeaf::m_dwObjectUniqIDConstant = 0;

CsGBTerrainLeaf::sNiTriINFO::sNiTriINFO():s_ptOffsetPos(NiPoint2::ZERO),s_nVertCount(0),s_nTemp1(0),s_fTemp2(0.0f),s_usNiTerrainIndex(0),s_dwObjectCount(0),s_nShaderCord(0),s_pfHeight(NULL)
{
	memset( s_cDetail_Map0, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map1, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map2, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map3, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map4, 0, sizeof(char)*MAX_PATH );
}

CsGBTerrainLeaf::sNiTriINFO::~sNiTriINFO()
{

}

void CsGBTerrainLeaf::sNiTriINFO::Reset()
{
	memset( s_cDetail_Map0, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map1, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map2, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map3, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map4, 0, sizeof(char)*MAX_PATH );

	s_dwObjectCount = 0;

	s_pfHeight = NULL;

	s_nShaderCord = 10000;
}

void CsGBTerrainLeaf::sNiTriINFO::Delete()
{
	memset( s_cDetail_Map0, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map1, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map2, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map3, 0, sizeof(char)*MAX_PATH );
	memset( s_cDetail_Map4, 0, sizeof(char)*MAX_PATH );

	s_dwObjectCount = 0;
	s_nShaderCord = 10000;

	SAFE_DELETE_ARRAY( s_pfHeight );
}

void CsGBTerrainLeaf::ObjectResetCallReady()
{
	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itObjectEnd = m_mapObject.end();
	for( ; itObject != itObjectEnd; ++itObject )
	{
		itObject->second->ResetCallReady();
	}
}
//////////////////////////////////////////////////////////////////////////


CsGBTerrainLeaf::CsGBTerrainLeaf()
:m_pLeafToolInfo(NULL),m_pTriStrip(NULL),m_pWorldShaderMaterial(NULL),
m_pDetailShaderMaterial(NULL),m_pDetailShadowShaderMaterial(NULL)
{  
}

CsGBTerrainLeaf::~CsGBTerrainLeaf()
{

}

void CsGBTerrainLeaf::Delete()
{
	CsGBTerrainNode::Delete();
	_DeleteObject();
	m_NiTriInfo.Delete();

	SAFE_DELETE( m_pLeafToolInfo );
}

void CsGBTerrainLeaf::UpdateObject( float fElapsedTime )
{
	if( IsEnableRender() == false )
		return;

	if( CAMERA_ST.UserCulling( m_pNiNode ) == false )
		return;

	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapObject.end();
	for( ; it != itEnd; ++it )
	{
		it->second->Update( fElapsedTime );
	}
}

void CsGBTerrainLeaf::UpdateObject_For_Shadow( float fElapsedTime )
{
	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapObject.end();
	for( ; it != itEnd; ++it )
	{
		if( it->second->UD_IsSkin() == true )
			continue;

		if( ( it->second->UD_IsNoShadow() == false )&&( it->second->UD_IsEffectRender() == false ) )
			it->second->Update( fElapsedTime, false );
	}
}

void CsGBTerrainLeaf::UpdateObject_For_Shadow_ExceptionObject( float fElapsedTime, CsGBObject* pExceptionObj )
{
	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapObject.end();
	for( ; it != itEnd; ++it )
	{
		if( pExceptionObj == it->second )
			continue;

		if( it->second->UD_IsSkin() == true )
			continue;

		if( ( it->second->UD_IsNoShadow() == false )&&( it->second->UD_IsEffectRender() == false ) )
			it->second->Update( fElapsedTime, false );
	}
}

void CsGBTerrainLeaf::_Init()
{
	CsGBTerrainNode::_Init();
	m_eNodeState = NS_NONEVISIBLE;
}

void CsGBTerrainLeaf::MakeNiTerrain( CsNiNodePtr pNiNode, sNiTriINFO* pNiTriInfo )
{
	assert_cs( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_MAKE );
	assert_cs( pNiNode != NULL );
	assert_cs( m_pNiNode == NULL );
	assert_cs( m_pParentNode != NULL );
	assert_cs( m_NiTriInfo.s_pfHeight == NULL );

	m_pNiNode = pNiNode;
	m_pTriStrip = (NiTriStrips*)m_pNiNode->GetAt( 0 );
	assert_cs( NiIsKindOf( NiTriStrips, m_pTriStrip ) == true );
	m_pNiNode->SetParentCsTNode( this );
	m_pParentNode->GetNiNode()->AttachChild( m_pNiNode );

	memcpy( &m_NiTriInfo, pNiTriInfo, sizeof( sNiTriINFO ) );

	_SetNiNodeName();
	_SetNiTriObjectName();
	_SetNiNodeTranslate();

	// ==== 쉐도우맵 생성
	CreateShadowMap_ByTool();

	_SetExistTerrain();
}

CsGBTerrainNode* CsGBTerrainLeaf::Create( eQUADT_ID const& eQuadID, CsNiNodePtr pBaseNiNode, CsGBTerrainNode* pRootNode, 
										 CsGBTerrainNode* pParentNode, int const& nTreeLevel, int const& nNodeLevel )
{
	assert_cs( nTreeLevel == nNodeLevel );
	assert_cs( m_eQuadID == QI_NONE );
	assert_cs( m_pNiNode == NULL );

	_Init();
	m_NiTriInfo.Reset();

	m_eQuadID = eQuadID;
	m_pRootNode = pRootNode;
	m_pParentNode = pParentNode;
	m_nNodeLevel = nNodeLevel;

	// CsNiNode 생성
	switch( nsCsGBTerrain::g_eCreateType )
	{
	case nsCsGBTerrain::CT_MAKE:
		assert_cs( m_pLeafToolInfo == NULL );
		m_pLeafToolInfo = csnew sLEAF_TOOL_INFO;
		return this;
	case nsCsGBTerrain::CT_FILELOAD:

#ifdef TERRAIN_RENDER_DEEPCOPY
		m_pNiNode = nsCsGBTerrain::g_pCurRoot->GetDeepCloneBaseNiNode();
#else
		m_pNiNode = nsCsGBTerrain::g_pCurRoot->GetCloneBaseNiNode();
#endif
		break;
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		assert_cs( m_pLeafToolInfo == NULL );
		m_pLeafToolInfo = csnew sLEAF_TOOL_INFO;
		m_pNiNode = nsCsGBTerrain::g_pCurRoot->GetDeepCloneBaseNiNode();
		( (NiTriStrips*)m_pNiNode->GetAt( 0 ) )->GetModelData()->SetConsistency( NiGeometryData::MUTABLE );
		break;
	}
	m_pTriStrip = (NiTriStrips*)m_pNiNode->GetAt( 0 );
	assert_cs( NiIsKindOf( NiTriStrips, m_pTriStrip ) == true );

	// CsNiNode 연결
	m_pNiNode->SetParentCsTNode( this );
	// 부모에 연결
	m_pParentNode->GetNiNode()->AttachChild( m_pNiNode );

	// NodeName 셋팅 - ExtraData 로드하는데 필요함
	_SetNiNodeName();
	// ExtraData Load
	_LoadExtraData( pBaseNiNode );
	assert_cs( IsExistTerrain() == true );

	switch( nsCsGBTerrain::g_eCreateType )
	{
	case nsCsGBTerrain::CT_FILELOAD:
		_CreateShaderMaterial();
		break;
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		UpdateBuffer();
		_SetNiNodeTexture_ByTool();
		break;
	}

	// Fog Attach
	m_pNiNode->AttachProperty( nsCsGBTerrain::g_pCurRoot->GetFogProperty() );

	_SetNiTriObjectName();
	_SetNiNodeTranslate();
	_SetExistTerrain();

	// 바운드 박스 재계산
	UpdateBuffer();
	m_pTriStrip->GetModelBound().ComputeFromData( m_pTriStrip->GetActiveVertexCount(), m_pTriStrip->GetVertices() );

	return this;
}


//==========================================================================================
//
//		Node State
//
//==========================================================================================

void CsGBTerrainLeaf::SetViewerPos( NiPoint2 ptPos )
{
	if( IsExistTerrain() == false )
		return;

	// 비지블 거리를 넘어 섰다
	float fLength = ( m_Info.s_ptCenterPos - ptPos ).Length();	
	if( fLength > m_Info.s_fHalfRadius + nsCsGBTerrain::g_TRView.g_fViewVisibleDist )
	{		
		SetNodeState( NS_NONEVISIBLE );
	}
	else if( fLength > m_Info.s_fHalfRadius + nsCsGBTerrain::g_TRView.g_fViewDetailDist )
	{
		SetNodeState( NS_WORLD );
	}
	else
	{
		SetNodeState( NS_DETAIL );
	}
}

void CsGBTerrainLeaf::SetNodeState( eNODE_STATE ns )
{
	if( IsExistTerrain() == false )
		return;

	if( m_eNodeState != ns )
	{
		assert_cs( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD );
		if( m_eNodeState == NS_NONEVISIBLE )
		{
			SetVisible( true );
		}
		else if( ns == NS_NONEVISIBLE )
		{
			m_eNodeState = ns;
			SetVisible( false );
			return;
		}
		
		m_eNodeState = ns;

		if( nsCsGBTerrain::g_Device.g_bPixelShader2 == true )
		{
			if( m_eNodeState == NS_WORLD )
			{
				m_pTriStrip->SetActiveMaterial( m_pWorldShaderMaterial );
			}
			else if( nsCsGBTerrain::g_bShadowRender == true )
			{
				m_pTriStrip->SetActiveMaterial( m_pDetailShadowShaderMaterial );
			}
			else
			{
				m_pTriStrip->SetActiveMaterial( m_pDetailShaderMaterial );
			}
		}
	}	
}

//==========================================================================================
//
//		NiNode
//
//==========================================================================================
void CsGBTerrainLeaf::UpdateBuffer()
{
	NiPoint3* pVert = m_pTriStrip->GetVertices();

	int cnt = m_NiTriInfo.s_nVertCount;
	for( int i=0; i<cnt; ++i )
	{
		pVert[ i ].z = m_NiTriInfo.s_pfHeight[ i ];
	}
}

void CsGBTerrainLeaf::BeginRenderTR()
{
	/*if( CAMERA_ST.UserCulling( m_pNiNode ) == false )
		return;*/

	m_bEnableRender = true;
}

void CsGBTerrainLeaf::_SetNiNodeTranslate()
{
	m_pNiNode->SetTranslate( m_NiTriInfo.s_ptOffsetPos.x, m_NiTriInfo.s_ptOffsetPos.y, 0.0f );
}

void CsGBTerrainLeaf::_SetNiNodeName()
{
	// QuadKey
	sprintf_s( m_cQuadKey, 32, "%s_%d", m_pParentNode->GetCharQuadKey(), m_eQuadID );
	m_nQuadKey = m_pParentNode->GetIntQuadKey()*10 + m_eQuadID;

	// 이름 입력
	char szName[ 32 ] = {0, };
	switch( m_eQuadID )
	{
	case QI_LD:		sprintf_s( szName, 32, "%d_QI_LD_LEAP", m_nNodeLevel );		break;
	case QI_RD:		sprintf_s( szName, 32, "%d_QI_RD_LEAP", m_nNodeLevel );		break;
	case QI_LT:		sprintf_s( szName, 32, "%d_QI_LT_LEAP", m_nNodeLevel );		break;
	case QI_RT:		sprintf_s( szName, 32, "%d_QI_RT_LEAP", m_nNodeLevel );		break;
	}
	strcat_s( szName, 32, m_cQuadKey );	
	m_pNiNode->SetName( szName );
}

void CsGBTerrainLeaf::_SetNiTriObjectName()
{
	// 오브젝트 이름 입력
	char szName[ 32 ] = {0, };
	sprintf_s( szName, 32, "%d_TBase", m_NiTriInfo.s_usNiTerrainIndex );	
	m_pTriStrip->SetName( szName );
}

//==========================================================================================
//
//		Object
//
//==========================================================================================

void CsGBTerrainLeaf::_DeleteObject()
{
	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapObject.end();
		for( ; it != itEnd; ++it )
		{
			it->second->DecreaseRefCount();
		}
		m_mapObject.clear();
	}	
	
	m_mapBasePickObj.clear();
	m_mapCameraAlphaObj.clear();
	m_mapPortalObj.clear();
	m_mapCameraCollision.clear();	
	m_mapQuestOn.clear();
	m_mapQuestOff.clear();
}

void CsGBTerrainLeaf::_ResistObject( CsGBObject* pObject )
{
	assert_cs( pObject->UD_IsNoPick() == false );

	if( m_mapObject.find( pObject->GetInfo()->s_dwUniqID ) != m_mapObject.end() )
	{
		return;
	}

	pObject->IncreaseRefCount();

	if( ( pObject->UD_IsTerrainBasePick() == true )||( pObject->UD_IsTerrainHidePick() == true ) )
	{
		m_mapBasePickObj[ pObject->GetInfo()->s_dwUniqID ] = pObject;
	}

	if( pObject->UDF_IsCameraAlpha() == true )
		m_mapCameraAlphaObj[ pObject->GetInfo()->s_dwUniqID ] = pObject;
	if( pObject->UD_IsPortal() == true )
		m_mapPortalObj[ pObject->GetInfo()->s_dwUniqID ] = pObject;
	if( pObject->UD_IsCameraCollision() == true )
		m_mapCameraCollision[ pObject->GetInfo()->s_dwUniqID ] = pObject;

	// 초기 등록시에만 등록
	if( pObject->IsRefCount( 1 ) )
	{
		if( pObject->UD_IsQuestOn() )
		{
			if( sUDD_QuestOn::m_CallBack_QuestCheck != NULL )
				pObject->SetEnableUse( false );

			assert_cs( m_mapQuestOn.find( pObject->GetInfo()->s_dwUniqID ) == m_mapQuestOn.end() );
			m_mapQuestOn[ pObject->GetInfo()->s_dwUniqID ] = pObject;
		}

		if( pObject->UD_IsQuestOff() )
		{
			assert_cs( m_mapQuestOff.find( pObject->GetInfo()->s_dwUniqID ) == m_mapQuestOff.end() );
			m_mapQuestOff[ pObject->GetInfo()->s_dwUniqID ] = pObject;
		}
	}	

	assert_cs( m_mapObject.find( pObject->GetInfo()->s_dwUniqID ) == m_mapObject.end() );
	m_mapObject[ pObject->GetInfo()->s_dwUniqID ] = pObject;
}

bool CsGBTerrainLeaf::ResistObject( CsGBObject* pObject )
{
	if( m_Info.s_Bound.CrossBoundCheck( pObject->GetInfo()->s_Bound ) == true )
	{
		_ResistObject( pObject );
		return true;
	}
	return false;
}

CsGBObject* CsGBTerrainLeaf::FindCsGBObject( NiNode* pNiNode )
{
	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapObject.end();
	for( ; it != itEnd; ++it )
	{
		if( it->second->GetNiNode() == pNiNode )
			return it->second;
	}
	
	return NULL;
}

CsGBObject* CsGBTerrainLeaf::FindCsGBObject( DWORD dwUniqObjectID )
{
	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapObject.end();
	for( ; it != itEnd; ++it )
	{
		if( it->second->GetInfo()->s_dwUniqID == dwUniqObjectID )
			return it->second;
	}	

	return NULL;
}

void CsGBTerrainLeaf::DeleteCsGBObject( DWORD dwUniqObjectID )
{
	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapObject.end();
	for( ; it != itEnd; ++it )
	{
		if( it->second->GetInfo()->s_dwUniqID == dwUniqObjectID )
		{
			it->second->DecreaseRefCount();			
			if( m_mapBasePickObj.find( it->first ) != m_mapBasePickObj.end() )
				m_mapBasePickObj.erase( it->first );
			if( m_mapCameraAlphaObj.find( it->first ) != m_mapCameraAlphaObj.end() )
				m_mapCameraAlphaObj.erase( it->first );
			if( m_mapPortalObj.find( it->first ) != m_mapPortalObj.end() )
				m_mapPortalObj.erase( it->first );
			if( m_mapCameraCollision.find( it->first ) != m_mapCameraCollision.end() )
				m_mapCameraCollision.erase( it->first );
			if( m_mapQuestOn.find( it->first ) != m_mapQuestOn.end() )
				m_mapQuestOn.erase( it->first );
			if( m_mapQuestOff.find( it->first ) != m_mapQuestOff.end() )
				m_mapQuestOff.erase( it->first );

			m_mapObject.erase( it );
			return;
		}
	}
}

//==========================================================================================
//
//		Object
//
//==========================================================================================
DWORD CsGBTerrainLeaf::CheckPortal( float fElapsedTime, NiBoundingVolume* pNiBoundingVolume )
{
	MAP_GBTERRAIN_OBJECT_IT it = m_mapPortalObj.begin();
	for( ; it!=m_mapPortalObj.end(); ++it )
	{
		CsGBObjProp* pProp = it->second->GetProp( CsGBObjProp::PROP_ABV );
		if(NULL == pProp)
			continue;

		CsGBObjProp_ABV* pkABV = (CsGBObjProp_ABV*)pProp;
		if( !pkABV->TestIntersect( fElapsedTime, pNiBoundingVolume ) )
			continue;

		CsGBObj_Portal* pkPortal = (CsGBObj_Portal*)it->second;
		if(NULL == pkPortal)
			continue;

		pProp = it->second->GetProp( CsGBObjProp::PROP_ACTOR );		
		if( pProp )
		{
			CsGBObjProp_Actor* pActorProp = (CsGBObjProp_Actor*)pProp;
			DWORD dwAniID = pActorProp->GetAnimation();
			if( dwAniID != CsGBObjProp_Actor::ACT_ANIMATION3 )// 포탈이고 액터 오브젝트인데 문이 닫혀 있는 경우 포탈 이용 불가
				continue;
		}

		return pkPortal->GetPortalID();
	}
	return 0;
}

void CsGBTerrainLeaf::CheckQuestObject()
{
	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapQuestOn.begin();
		MAP_GBTERRAIN_OBJECT_IT itNext = it;
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapQuestOn.end();
		while( itNext != itEnd )
		{
			it = itNext;
			++itNext;

			sUDD_QuestOn* pUDD = (sUDD_QuestOn*)it->second->GetUserDefine()->GetData( sUSER_DEFINE::UD_QUEST_ON );
			assert_cs( pUDD != NULL );
			if( pUDD->IsOn() == true )
			{
				it->second->SetEnableUse( true );
				m_mapQuestOn.erase( it );
			}
		}
	}

	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapQuestOff.begin();
		MAP_GBTERRAIN_OBJECT_IT itNext = it;
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapQuestOff.end();
		while( itNext != itEnd )
		{
			it = itNext;
			++itNext;

			sUDD_QuestOff* pUDD = (sUDD_QuestOff*)it->second->GetUserDefine()->GetData( sUSER_DEFINE::UD_QUEST_OFF );
			assert_cs( pUDD != NULL );
			if( pUDD->IsOff() == true )
			{
				it->second->SetEnableUse( false );
				m_mapQuestOff.erase( it );
			}
		}
	}
}

void CsGBTerrainLeaf::CheckQuestObject( int nCompQuest )
{
	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapQuestOn.begin();
		MAP_GBTERRAIN_OBJECT_IT itNext = it;
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapQuestOn.end();
		while( itNext != itEnd )
		{
			it = itNext;
			++itNext;

			sUDD_QuestOn* pUDD = (sUDD_QuestOn*)it->second->GetUserDefine()->GetData( sUSER_DEFINE::UD_QUEST_ON );
			assert_cs( pUDD != NULL );
			if( pUDD->IsOn( nCompQuest ) == true )
			{
				it->second->SetEnableUse( true );
				m_mapQuestOn.erase( it );
			}
		}
	}

	{
		MAP_GBTERRAIN_OBJECT_IT it = m_mapQuestOff.begin();
		MAP_GBTERRAIN_OBJECT_IT itNext = it;
		MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapQuestOff.end();
		while( itNext != itEnd )
		{
			it = itNext;
			++itNext;

			sUDD_QuestOff* pUDD = (sUDD_QuestOff*)it->second->GetUserDefine()->GetData( sUSER_DEFINE::UD_QUEST_OFF );
			assert_cs( pUDD != NULL );
			if( pUDD->IsOff( nCompQuest ) == true )
			{
				it->second->SetEnableUse( false );
				m_mapQuestOff.erase( it );
			}
		}
	}
}



//==========================================================================================
//
//		Texture Filter Mode
//
//==========================================================================================

void CsGBTerrainLeaf::ResetTextureFilterMode()
{
	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapObject.end();
	for( ; it != itEnd; ++it )
	{
		if( it->second->IsCallReady() == false )
			continue;

		it->second->ResetTextureFilterMode();
	}
}

//==========================================================================================
//
//		Shader
//
//==========================================================================================

void CsGBTerrainLeaf::ResetShader()
{
	assert_cs( nsCsGBTerrain::g_Device.g_bPixelShader2 == true );
	if( m_eNodeState == NS_WORLD )
	{
		m_pTriStrip->SetActiveMaterial( m_pWorldShaderMaterial );
	}
	else if( nsCsGBTerrain::g_bShadowRender == true )
	{
		m_pTriStrip->SetActiveMaterial( m_pDetailShadowShaderMaterial );
	}
	else
	{
		m_pTriStrip->SetActiveMaterial( m_pDetailShaderMaterial );
	}

	MAP_GBTERRAIN_OBJECT_IT it = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itEnd = m_mapObject.end();
	for( ; it != itEnd; ++it )
	{
		if( it->second->IsCallReady() == false )
			continue;

		it->second->CheckShader();
	}
}

//==========================================================================================
//
//		Light
//
//==========================================================================================
void CsGBTerrainLeaf::ApplyLight_Object( CsGBLight* pLight )
{
	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itObjectEnd = m_mapObject.end();
	for( ; itObject != itObjectEnd; ++itObject )
	{
		itObject->second->ApplyLight( pLight );
	}
}

void CsGBTerrainLeaf::ReleaseLight_Object( CsGBLight* pLight )
{
	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itObjectEnd = m_mapObject.end();
	for( ; itObject != itObjectEnd; ++itObject )
	{
		itObject->second->ReleaseLight( pLight );
	}
}


//==========================================================================================
//
//		Texture
//
//==========================================================================================
void CsGBTerrainLeaf::SetBaseMap( NiPixelData* pData )
{
	assert_cs( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD );

	if( nsCsGBTerrain::g_Device.g_bPixelShader2 == true )
	{
		NiTriStrips* pTriObject = (NiTriStrips*)m_pNiNode->GetAt( 0 );
		NiTexturingPropertyPtr pOldTexProp = NiDynamicCast( NiTexturingProperty, pTriObject->RemoveProperty( NiTexturingProperty::GetType() ) );
		pOldTexProp = 0;
		NiTexturingProperty* pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture( pData );

		assert_cs( m_NiTriInfo.s_cDetail_Map0[ 0 ] != NULL );
		{
			NiTexturingProperty::ShaderMap* pShaderMap = nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map0 );
			pTexProp->SetShaderMap( DETAIL_MAP_0_SHADER_NTM, pShaderMap );
		}
		if( m_NiTriInfo.s_cDetail_Map1[ 0 ] != NULL )
		{
			pTexProp->SetShaderMap( DETAIL_MAP_1_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map1 ) );
		}
		if( m_NiTriInfo.s_cDetail_Map2[ 0 ] != NULL )
		{
			pTexProp->SetShaderMap( DETAIL_MAP_2_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map2 ) );
		}
		if( m_NiTriInfo.s_cDetail_Map3[ 0 ] != NULL )
		{
			pTexProp->SetShaderMap( DETAIL_MAP_3_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map3 ) );
		}
		if( m_NiTriInfo.s_cDetail_Map4[ 0 ] != NULL )
		{
			pTexProp->SetShaderMap( DETAIL_MAP_4_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map4 ) );
		}

		pTriObject->AttachProperty( pTexProp );
	}
	else
	{
		NiTriStrips* pTriObject = (NiTriStrips*)m_pNiNode->GetAt( 0 );
		NiTexturingPropertyPtr pOldTexProp = NiDynamicCast( NiTexturingProperty, pTriObject->RemoveProperty( NiTexturingProperty::GetType() ) );
		pOldTexProp = 0;
		NiTexturingProperty* pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture( pData );
		pTexProp->SetApplyMode( NiTexturingProperty::APPLY_REPLACE );
		pTexProp->GetBaseMap()->SetTextureIndex( 1 );
		pTriObject->AttachProperty( pTexProp );
	}
}

void CsGBTerrainLeaf::SetShaderMap( int nShaderIndex, NiPixelData* pPData )
{
	NiTriStrips* pTriObject = (NiTriStrips*)m_pNiNode->GetAt( 0 );
	NiTexturingProperty* pTexProp = (NiTexturingProperty*)pTriObject->GetProperty( NiTexturingProperty::GetType() );	
	pTexProp->SetShaderMap( nShaderIndex, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( pPData ) );
}

void CsGBTerrainLeaf::SetShaderMap( int nShaderIndex, NiRenderedTexture* pTex )
{
	NiTriStrips* pTriObject = (NiTriStrips*)m_pNiNode->GetAt( 0 );
	NiTexturingProperty* pTexProp = (NiTexturingProperty*)pTriObject->GetProperty( NiTexturingProperty::GetType() );
	NiTexturingProperty::ShaderMap* pMap = NiNew NiTexturingProperty::ShaderMap( pTex, nShaderIndex );
	pTexProp->SetShaderMap( nShaderIndex, pMap );

	//pTexProp->SetShaderMap( nShaderIndex, pTex );
}

void CsGBTerrainLeaf::SetDetail_Map0_ByTool( char* cpTex )
{
	assert_cs( m_pNiNode != NULL );

	strcpy_s( m_NiTriInfo.s_cDetail_Map0, MAX_PATH, cpTex );
	_SetNiNodeTexture_ByTool();	
}

void CsGBTerrainLeaf::SetDetail_Map1_ByTool( char* cpTex )
{
	assert_cs( m_pNiNode != NULL );

	strcpy_s( m_NiTriInfo.s_cDetail_Map1, MAX_PATH, cpTex );
	_SetNiNodeTexture_ByTool();
}

void CsGBTerrainLeaf::SetDetail_Map2_ByTool( char* cpTex )
{
	assert_cs( m_pNiNode != NULL );

	strcpy_s( m_NiTriInfo.s_cDetail_Map2, MAX_PATH, cpTex );
	_SetNiNodeTexture_ByTool();
}

void CsGBTerrainLeaf::SetDetail_Map3_ByTool( char* cpTex )
{
	assert_cs( m_pNiNode != NULL );

	strcpy_s( m_NiTriInfo.s_cDetail_Map3, MAX_PATH, cpTex );
	_SetNiNodeTexture_ByTool();
}

void CsGBTerrainLeaf::SetDetail_Map4_ByTool( char* cpTex )
{
	assert_cs( m_pNiNode != NULL );

	strcpy_s( m_NiTriInfo.s_cDetail_Map4, MAX_PATH, cpTex );
	_SetNiNodeTexture_ByTool();
}


void CsGBTerrainLeaf::CreateShadowMap_ByTool()
{
	assert_cs( m_pNiNode != NULL );
	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	
	// 기존에 만들어 놓은것이 존재 한다면
	{
		NiTriStrips* pTriObject = (NiTriStrips*)m_pNiNode->GetAt( 0 );
		NiTexturingProperty* pTexProp = NiDynamicCast( NiTexturingProperty, pTriObject->GetProperty( NiTexturingProperty::GetType() ) );
		if( pTexProp != NULL )
		{
			NiTexturingProperty::ShaderMap* pShaderMap = pTexProp->GetShaderMap( SHADOW_MAP_SHADER_NTM );
			if( pShaderMap != NULL )
			{
				NiSourceTexture* pSourceTex = (NiSourceTexture*)pShaderMap->GetTexture();
				NiPixelData* pPData = pSourceTex->GetSourcePixelData();
				assert_cs( pPData->GetWidth() == pPData->GetHeight() );
				// 만들어 놓은거랑 만들거랑 사이즈가 같다면 리턴
				if( pPData->GetWidth() == pRootInfo->s_nInitMapSize*pRootInfo->s_nShadowMPPG )
				{
					return;
				}
			}
		}
	}
	_SetNiNodeTexture_ByTool( true );
}

void CsGBTerrainLeaf::_SetNiNodeTexture_ByTool( bool bCreateShadowMap /*=false*/ )
{
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );	
	NiTexturingPropertyPtr pOldTexProp = NiDynamicCast( NiTexturingProperty, m_pTriStrip->RemoveProperty( NiTexturingProperty::GetType() ) );
	NiTexturingProperty* pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture();

	m_NiTriInfo.s_nShaderCord = 0;
	assert_cs( m_NiTriInfo.s_cDetail_Map0[ 0 ] != NULL );
	
	{
		// Detail Map0
		pTexProp->SetShaderMap( DETAIL_MAP_0_SHADER_NTM,
								nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map0 ) );		
		m_NiTriInfo.s_nShaderCord += DMAP_0_SHADER_CORD;
	}	

	// Detail Map1
	if( m_NiTriInfo.s_cDetail_Map1[ 0 ] != NULL )
	{
		pTexProp->SetShaderMap( DETAIL_MAP_1_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map1 ) );
		m_NiTriInfo.s_nShaderCord += DMAP_1_SHADER_CORD;
	}
	// Detail Map2
	if( m_NiTriInfo.s_cDetail_Map2[ 0 ] != NULL )
	{
		pTexProp->SetShaderMap( DETAIL_MAP_2_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map2 ) );
		m_NiTriInfo.s_nShaderCord += DMAP_2_SHADER_CORD;
	}
	// Detail Map3
	if( m_NiTriInfo.s_cDetail_Map3[ 0 ] != NULL )
	{
		pTexProp->SetShaderMap( DETAIL_MAP_3_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map3 ) );
		m_NiTriInfo.s_nShaderCord += DMAP_3_SHADER_CORD;
	}
	// Detail Map4
	if( m_NiTriInfo.s_cDetail_Map4[ 0 ] != NULL )
	{
		pTexProp->SetShaderMap( DETAIL_MAP_4_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( m_NiTriInfo.s_cDetail_Map4 ) );
		m_NiTriInfo.s_nShaderCord += DMAP_4_SHADER_CORD;
	}

	int nAMSize = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nInitMapSize*nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nAlphaMPPG;
	int nSMSize = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nInitMapSize*nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nShadowMPPG;
	int nEMSize = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nInitMapSize*nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nEffectMPPG;

	// Alpha Map
	pTexProp->SetShaderMap( ALPHA_MAP_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( nAMSize, nAMSize, ALPHA_MAP_FORMAT,
							pOldTexProp == NULL ? NULL : pOldTexProp->GetShaderMap( ALPHA_MAP_SHADER_NTM ), 0x00000000 ) );

	// Shadow Map
	pTexProp->SetShaderMap( SHADOW_MAP_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( nSMSize, nSMSize, SHADOW_MAP_FORMAT,
							bCreateShadowMap ? NULL : pOldTexProp->GetShaderMap( SHADOW_MAP_SHADER_NTM ), 0xffffffff ) );

	// Effect Map
	pTexProp->SetShaderMap( EFFECT_MAP_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( nEMSize, nEMSize, EFFECT_MAP_FORMAT,
							pOldTexProp == NULL ? NULL : pOldTexProp->GetShaderMap( EFFECT_MAP_SHADER_NTM ), 0xffffffff ) );

	m_pTriStrip->AttachProperty( pTexProp );
	pOldTexProp = 0;

	m_pNiNode->UpdateProperties();

	// Effect Shader 연결
	/*char sz[ 32 ] = { 0, };
	assert_cs( nsCsGBTerrain::g_Device.g_bPixelShader2 == true );
	assert_cs( nsCsGBTerrain::g_Device.g_nMultiTextureCount == 8 );
	sprintf_s( sz, 32, "TR_TOOL_%d_8", m_NiTriInfo.s_nShaderCord );
	NiShader* pShader = GET_SHADER( sz );*/
	
	NiShader* pShader = GET_SHADER( "TR_TOOL_1111_8" );
	m_pTriStrip->SetShader( pShader );
	m_pNiNode->UpdateEffects();
}

//==========================================================================================	
//
//		Effect
//
//==========================================================================================

void CsGBTerrainLeaf::_CreateShaderMaterial()
{
	assert_cs( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD );	
	if( nsCsGBTerrain::g_Device.g_bPixelShader2 == true )
	{
		m_pWorldShaderMaterial = NiSingleShaderMaterial::Create( "TR_Base" );

		char sz[ 32 ] = { 0, };
		sprintf_s( sz, 32, "TR_ZS_8" );
		m_pDetailShadowShaderMaterial = NiSingleShaderMaterial::Create( sz );
		sprintf_s( sz, 32, "TR_8" );
		m_pDetailShaderMaterial = NiSingleShaderMaterial::Create( sz );

		m_pTriStrip->ApplyMaterial( m_pWorldShaderMaterial );
		m_pTriStrip->ApplyMaterial( m_pDetailShaderMaterial );
		m_pTriStrip->ApplyMaterial( m_pDetailShadowShaderMaterial );
		m_pNiNode->UpdateEffects();
	}	
}

//==========================================================================================
//
//		Picking
//
//==========================================================================================


//==========================================================================================
//
//		Extra Data
//
//==========================================================================================

void CsGBTerrainLeaf::SaveExtraData( CsNiNodePtr pBaseNiNode )
{
	CsGBTerrainNode::SaveExtraData( pBaseNiNode );
	
	NiBinaryExtraData* pExtData = NULL;
	MAP_GBTERRAIN_OBJECT_IT itObject;

	// ==== 세이브전 선행 체크
	for( itObject = m_mapObject.begin(); itObject != m_mapObject.end(); ++itObject )
	{
		itObject->second->PreSaveExtraData();
	}

	// ==== Object 저장
	UINT uiTotalObjectSize = 0;
	m_NiTriInfo.s_dwObjectCount = (DWORD)m_mapObject.size();

	// 저장할 사이즈 체크
	for( itObject = m_mapObject.begin(); itObject != m_mapObject.end(); ++itObject )
	{
		if( itObject->second->UD_IsTableObj() == false )
		{
			uiTotalObjectSize += itObject->second->GetSaveSize();
		}
		else
		{			
			// 테이블 오브젝트라면 카운트 감소
			--m_NiTriInfo.s_dwObjectCount;
		}
	}

	// 메모리 할당
	DWORD nMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	CsMapTableObjGroup* pGroup = nsCsMapTable::g_pMapTableObjMng->GetGroup( nMapID );

	UINT uiFileOffset = 0;
	char* pObjectFile = NiAlloc( char, uiTotalObjectSize );
	for( itObject = m_mapObject.begin(); itObject != m_mapObject.end(); ++itObject )
	{
		if( itObject->second->UD_IsTableObj() == false )
		{
			itObject->second->SaveExtraData( pObjectFile, uiFileOffset );
		}
		else
		{
			UINT nTableOffset = 0;
			// 테이블 오브젝트라면 별도 공간에 기입
			CsMapTableObj::sINFO info;
			info.s_nDataSize = itObject->second->GetSaveSize();
			CsMapTableObj* pAddObj = pGroup->Add( m_nQuadKey, &info );
			pAddObj->AllocData();
			itObject->second->SaveExtraData( pAddObj->GetData(), nTableOffset );
		}
	}

	pExtData = NiNew NiBinaryExtraData( uiTotalObjectSize, (char*)pObjectFile );
	char cObjKey[ MAX_FILENAME ];
	sprintf_s( cObjKey, MAX_FILENAME, "%s_object", m_cQuadKey );
	pBaseNiNode->AddExtraData( cObjKey, pExtData );

	// ==== 인포 저장
	// 저장할 사이즈
	UINT uiSize = sizeof( sNiTriINFO ) + sizeof( float )*m_NiTriInfo.s_nVertCount;

	// 메모리 할당
	char* pLeafInfo = NiAlloc( char, uiSize );

	// 값할당
	int nOffset = 0;
	memcpy( pLeafInfo, &m_NiTriInfo, sizeof( sNiTriINFO ) );
	nOffset += sizeof( sNiTriINFO );

	// Height Field
	memcpy( &pLeafInfo[ nOffset ], m_NiTriInfo.s_pfHeight, sizeof( float )*m_NiTriInfo.s_nVertCount );
	nOffset += ( sizeof( float )*m_NiTriInfo.s_nVertCount );

	pExtData = NiNew NiBinaryExtraData( uiSize, (char*)pLeafInfo );

	char cQuadData[ 32 ] = {0, };
	sprintf_s( cQuadData, 32, "%s_data", m_cQuadKey );
	pBaseNiNode->AddExtraData( cQuadData, pExtData );


	m_NiTriInfo.s_dwObjectCount = (DWORD)m_mapObject.size();
}

void CsGBTerrainLeaf::_LoadExtraData( CsNiNodePtr pBaseNiNode )
{
	assert_cs( m_NiTriInfo.s_pfHeight == NULL );

	CsGBTerrainNode::_LoadExtraData( pBaseNiNode );

	char cQuadData[ 32 ] = {0, };
	sprintf_s( cQuadData, 32, "%s_data", m_cQuadKey );
	
	unsigned int uiSize = 0;
	char* pData = NULL;

	// ==== TriInfo
	NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pBaseNiNode->GetExtraData( cQuadData );
	assert_cs( pExtData != NULL );	
	pExtData->GetValue( uiSize, pData );

	// 값할당
	UINT uiOffset = 0;
	memcpy( &m_NiTriInfo, pData, sizeof( sNiTriINFO ) );
	uiOffset += sizeof( sNiTriINFO );

	// Height Field	
	m_NiTriInfo.s_pfHeight = csnew float[ m_NiTriInfo.s_nVertCount ];
	memcpy( m_NiTriInfo.s_pfHeight, &pData[ uiOffset ], sizeof( float )*m_NiTriInfo.s_nVertCount );
	uiOffset += sizeof( float )*m_NiTriInfo.s_nVertCount;
	

	if( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD )
	{
		if( m_NiTriInfo.s_dwObjectCount )
		{
			_LoadExtraData_Obj( pBaseNiNode );
		}		
		_LoadExtraData_TableObj();
	}
	// 툴에서
	else
	{
		if( m_NiTriInfo.s_dwObjectCount )
		{
			_LoadExtraData_Obj_Tool( pBaseNiNode );
		}
		
		_LoadExtraData_TableObj_Tool();
	}	
}

void CsGBTerrainLeaf::_LoadExtraData_Obj( CsNiNodePtr pBaseNiNode )
{
	// ==== Object 로드
	char cObjKey[ MAX_FILENAME ];
	sprintf_s( cObjKey, MAX_FILENAME, "%s_object", m_cQuadKey );
	NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pBaseNiNode->GetExtraData( cObjKey );

	unsigned int uiSize = 0;
	char* pData = NULL;
	pExtData->GetValue( uiSize, pData );

	UINT uiOffset = 0;
	for( DWORD i=0; i<m_NiTriInfo.s_dwObjectCount; ++i )
	{
		CsGBObject::sFILE_HEADER fh;
		memcpy( &fh, &pData[ uiOffset ], sizeof( CsGBObject::sFILE_HEADER ) );		
		uiOffset += sizeof( CsGBObject::sFILE_HEADER );

		if( fh.s_dwSaveType == CsGBObject::ST_EXIST_OBJECT )
		{
			CsGBObject::sEXIST_INFO ei;
			memcpy( &ei, &pData[ uiOffset ], sizeof( CsGBObject::sEXIST_INFO ) );
			uiOffset += sizeof( CsGBObject::sEXIST_INFO );
			ei.s_dwUniqID += m_dwObjectUniqIDConstant;
			CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( ei.s_dwUniqID );
			if( pObject != NULL )
			{
				if( pObject->UD_IsNoClipping() == false )
				{
					_ResistObject( pObject );
				}					
			}
		}
		else
		{
			assert_cs( fh.s_dwSaveType == CsGBObject::ST_FIRST_OBJECT );
			CsGBObject::sINFO oi;
			CsGBObject::LoadObjectInfo( &oi, pData, uiOffset );
			CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( &oi );
			if( pObject != NULL )
			{
				assert_cs( pObject->GetUserDefine() != NULL );
				pObject->LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
				g_Sorting.SetID( pObject->GetGetterID() );
				if( pObject->UD_IsNoClipping() == false )
				{
					if( pObject->UD_IsActorManager() )
						nsCsGBTerrain::g_pCurRoot->Insert_ActorObj( pObject );

					_ResistObject( pObject );
				}
				else
				{
					nsCsGBTerrain::g_pCurRoot->Insert_No_Clipping( pObject );
				}
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
					obj.LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}
				else
				{
					CsGBObject obj;
					obj.ResetCallReady();
					obj.LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}					
			}
		}
	}
}

void CsGBTerrainLeaf::_LoadExtraData_Obj_Tool( CsNiNodePtr pBaseNiNode )
{
	// ==== Object 로드
	char cObjKey[ MAX_FILENAME ];
	sprintf_s( cObjKey, MAX_FILENAME, "%s_object", m_cQuadKey );
	NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pBaseNiNode->GetExtraData( cObjKey );

	unsigned int uiSize = 0;
	char* pData = NULL;
	pExtData->GetValue( uiSize, pData );

	UINT uiOffset = 0;
	for( DWORD i=0; i<m_NiTriInfo.s_dwObjectCount; ++i )
	{
		CsGBObject::sFILE_HEADER fh;
		memcpy( &fh, &pData[ uiOffset ], sizeof( CsGBObject::sFILE_HEADER ) );
		uiOffset += sizeof( CsGBObject::sFILE_HEADER );

		if( fh.s_dwSaveType == CsGBObject::ST_EXIST_OBJECT )
		{
			CsGBObject::sEXIST_INFO ei;
			memcpy( &ei, &pData[ uiOffset ], sizeof( CsGBObject::sEXIST_INFO ) );
			uiOffset += sizeof( CsGBObject::sEXIST_INFO );
			ei.s_dwUniqID += m_dwObjectUniqIDConstant;
			CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( ei.s_dwUniqID );
			if( pObject != NULL )
			{
				_ResistObject( pObject );
			}
		}
		else
		{
			assert_cs( fh.s_dwSaveType == CsGBObject::ST_FIRST_OBJECT );
			CsGBObject::sINFO oi;
			CsGBObject::LoadObjectInfo( &oi, pData, uiOffset );
			CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( &oi );
			if( pObject != NULL )
			{
				assert_cs( pObject->GetUserDefine() != NULL );
				pObject->LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
				nsCsGBTerrain::g_pCurRoot->GetInfo()->CheckObjectID( oi.s_dwUniqID );
				g_Sorting.SetID( pObject->GetGetterID() );
				_ResistObject( pObject );
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
					obj.LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}
				else
				{
					CsGBObject obj;
					obj.ResetCallReady();
					obj.LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}					
			}
		}
	}
}

void CsGBTerrainLeaf::_LoadExtraData_TableObj()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsMapTable::g_pMapTableObjMng->IsGroup( dwMapID ) == false )
		return;

	CsMapTableObjGroup* pGroup = nsCsMapTable::g_pMapTableObjMng->GetGroup( dwMapID );
	if( pGroup->IsNode( m_nQuadKey ) == false )
		return;

	// 테이블 오브젝트 로드	
	CsMapTableObjGroup::sNODE* pTONode = pGroup->GetNode( m_nQuadKey );
	CsMapTableObj::LIST::iterator itTO = pTONode->s_list.begin();
	CsMapTableObj::LIST::iterator itTOEnd = pTONode->s_list.end();
	m_NiTriInfo.s_dwObjectCount += (int)pTONode->s_list.size();
	for( ; itTO!=itTOEnd; ++itTO )
	{
		char* pData = (*itTO)->GetData();
		UINT uiOffset = 0;

		CsGBObject::sFILE_HEADER fh;
		memcpy( &fh, &pData[ uiOffset ], sizeof( CsGBObject::sFILE_HEADER ) );
		uiOffset += sizeof( CsGBObject::sFILE_HEADER );

		if( fh.s_dwSaveType == CsGBObject::ST_EXIST_OBJECT )
		{
			CsGBObject::sEXIST_INFO ei;
			memcpy( &ei, &pData[ uiOffset ], sizeof( CsGBObject::sEXIST_INFO ) );
			uiOffset += sizeof( CsGBObject::sEXIST_INFO );
			ei.s_dwUniqID += m_dwObjectUniqIDConstant;
			CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( ei.s_dwUniqID );
			if( pObject != NULL )
			{
				if( pObject->UD_IsNoClipping() == false )
				{
					_ResistObject( pObject );
				}
			}
		}
		else
		{
			assert_cs( fh.s_dwSaveType == CsGBObject::ST_FIRST_OBJECT );
			CsGBObject::sINFO oi;
			CsGBObject::LoadObjectInfo( &oi, pData, uiOffset );
			CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( &oi );
			if( pObject != NULL )
			{
				assert_cs( pObject->GetUserDefine() != NULL );
				pObject->LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
				nsCsGBTerrain::g_pCurRoot->GetInfo()->CheckObjectID( oi.s_dwUniqID );
				g_Sorting.SetID( pObject->GetGetterID() );

				if( pObject->UD_IsNoClipping() == false )
				{
					if( pObject->UD_IsActorManager() )
						nsCsGBTerrain::g_pCurRoot->Insert_ActorObj( pObject );

					_ResistObject( pObject );
				}
				else
				{
					nsCsGBTerrain::g_pCurRoot->Insert_No_Clipping( pObject );
				}
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
					obj.LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}
				else
				{
					CsGBObject obj;
					obj.ResetCallReady();
					obj.LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}					
			}
		}
	}
}

void CsGBTerrainLeaf::_LoadExtraData_TableObj_Tool()
{
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsMapTable::g_pMapTableObjMng->IsGroup( dwMapID ) == false )
		return;

	CsMapTableObjGroup* pGroup = nsCsMapTable::g_pMapTableObjMng->GetGroup( dwMapID );
	if( pGroup->IsNode( m_nQuadKey ) == false )
		return;
	
	// 테이블 오브젝트 로드	
	CsMapTableObjGroup::sNODE* pTONode = pGroup->GetNode( m_nQuadKey );
	CsMapTableObj::LIST::iterator itTO = pTONode->s_list.begin();
	CsMapTableObj::LIST::iterator itTOEnd = pTONode->s_list.end();
	m_NiTriInfo.s_dwObjectCount += (int)pTONode->s_list.size();
	for( ; itTO!=itTOEnd; ++itTO )
	{
		char* pData = (*itTO)->GetData();
		UINT uiOffset = 0;

		CsGBObject::sFILE_HEADER fh;
		memcpy( &fh, &pData[ uiOffset ], sizeof( CsGBObject::sFILE_HEADER ) );
		uiOffset += sizeof( CsGBObject::sFILE_HEADER );

		if( fh.s_dwSaveType == CsGBObject::ST_EXIST_OBJECT )
		{
			CsGBObject::sEXIST_INFO ei;
			memcpy( &ei, &pData[ uiOffset ], sizeof( CsGBObject::sEXIST_INFO ) );
			uiOffset += sizeof( CsGBObject::sEXIST_INFO );
			ei.s_dwUniqID += m_dwObjectUniqIDConstant;
			CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( ei.s_dwUniqID );
			if( pObject != NULL )
			{
				_ResistObject( pObject );
			}
		}
		else
		{
			assert_cs( fh.s_dwSaveType == CsGBObject::ST_FIRST_OBJECT );
			CsGBObject::sINFO oi;
			CsGBObject::LoadObjectInfo( &oi, pData, uiOffset );
			CsGBObject* pObject = nsCsGBTerrain::g_ObjectResMgr.GetObject( &oi );
			if( pObject != NULL )
			{
				assert_cs( pObject->GetUserDefine() != NULL );
				pObject->LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
				nsCsGBTerrain::g_pCurRoot->GetInfo()->CheckObjectID( oi.s_dwUniqID );
				g_Sorting.SetID( pObject->GetGetterID() );
				_ResistObject( pObject );
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
					obj.LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}
				else
				{
					CsGBObject obj;
					obj.ResetCallReady();
					obj.LoadExtraData( pData, uiOffset, oi.s_dwUserDefinePlag );
					obj.Delete();
				}					
			}
		}
	}
}

void CsGBTerrainLeaf::SaveToolExtraData( CsNiNodePtr pBaseNiNode )
{
	NiBinaryExtraData* pExtData = NULL;
	MAP_GBTERRAIN_OBJECT_IT itObject;

	// ==== Object 저장
	UINT uiTotalToolInfoSize = 0;

	m_pLeafToolInfo->s_nHaveChildObjectCount = 0;

	// 저장할 사이즈 체크
	uiTotalToolInfoSize += sizeof( sLEAF_TOOL_INFO );
	UINT uiAddSize;
	for( itObject = m_mapObject.begin(); itObject != m_mapObject.end(); ++itObject )
	{
		uiAddSize = itObject->second->GetSaveToolSize();
		if( uiAddSize != 0 )
		{
			uiTotalToolInfoSize += uiAddSize;
			++m_pLeafToolInfo->s_nHaveChildObjectCount;
		}		
	}	

	// 메모리 할당
	UINT uiFileOffset = 0;
	char* pObjectFile = NiAlloc( char, uiTotalToolInfoSize );

	memcpy( pObjectFile, m_pLeafToolInfo, sizeof( sLEAF_TOOL_INFO ) );
	uiFileOffset += sizeof( sLEAF_TOOL_INFO );
	for( itObject = m_mapObject.begin(); itObject != m_mapObject.end(); ++itObject )
	{
		itObject->second->SaveToolExtraData( pObjectFile, uiFileOffset );
	}

	pExtData = NiNew NiBinaryExtraData( uiTotalToolInfoSize, (char*)pObjectFile );
	char cObjKey[ MAX_FILENAME ];
	sprintf_s( cObjKey, MAX_FILENAME, "%s_toolobject", m_cQuadKey );
	pBaseNiNode->AddExtraData( cObjKey, pExtData );	
}


void CsGBTerrainLeaf::LoadToolExtraData( CsNiNodePtr pBaseNiNode )
{
	char cQuadData[ 32 ] = {0, };
	sprintf_s( cQuadData, 32, "%s_toolobject", m_cQuadKey );

	NiBinaryExtraData* pExtData = NULL;
	unsigned int uiSize = 0;
	char* pData = NULL;

	// ==== sLEAF_TOOL_INFO
	pExtData = (NiBinaryExtraData*)pBaseNiNode->GetExtraData( cQuadData );
	if( pExtData == NULL )
		return;

	pExtData->GetValue( uiSize, pData );

	// 값할당
	UINT uiOffset = 0;
	memcpy( m_pLeafToolInfo, &pData[ uiOffset ], sizeof( sLEAF_TOOL_INFO ) );
	uiOffset += sizeof( sLEAF_TOOL_INFO );

	// Have Child
	CsGBObject::sTOOL_INFO objinfo;
	for( int i=0; i<m_pLeafToolInfo->s_nHaveChildObjectCount; ++i )
	{
		memcpy( &objinfo, &pData[ uiOffset ], sizeof( CsGBObject::sTOOL_INFO ) );
		objinfo.s_dwHaveChildUniqID += CsGBTerrainLeaf::m_dwObjectUniqIDConstant;
		assert_cs( m_mapObject.find( objinfo.s_dwHaveChildUniqID ) != m_mapObject.end() );
		m_mapObject[ objinfo.s_dwHaveChildUniqID ]->LoadToolExtraData( pData, uiOffset );
	}
}

//==========================================================================================
//
//		Picking
//
//==========================================================================================

void CsGBTerrainLeaf::Pick_Terrain( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	if( ( m_eNodeState != NS_DETAIL )||( IsExistTerrain() == false ) )
		return;

#ifdef TERRAIN_RENDER_DEEPCOPY
#else
	UpdateBuffer();
#endif
	pCsPick->Pick( m_pNiNode, origin, dir, true );

	/*MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBaseObj.begin();	
	for( ; itObject != m_mapBaseObj.end(); ++itObject )
	{
		itObject->second->Picking( pCsPick, origin, dir );
	}*/

	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		itObject->second->Picking( pCsPick, origin, dir );
	}
}

void CsGBTerrainLeaf::Pick_TBP( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	if( m_eNodeState != NS_DETAIL )
		return;

	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		itObject->second->Picking( pCsPick, origin, dir );
	}
}

void CsGBTerrainLeaf::Pick_T_TB_TBP_ByTool( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	if( IsExistTerrain() == false )
		return;
	
	pCsPick->Pick( m_pNiNode, origin, dir, true );
	/*MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBaseObj.begin();
	for( ; itObject != m_mapBaseObj.end(); ++itObject )
	{
		itObject->second->Picking( pCsPick, origin, dir );
	}*/

	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		itObject->second->Picking( pCsPick, origin, dir );		
	}
}

void CsGBTerrainLeaf::Pick_T_ByTool( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	if( IsExistTerrain() == false )
		return;

	pCsPick->Pick( m_pNiNode, origin, dir, true );
}

void CsGBTerrainLeaf::Pick_T_TB_TBP_CAP( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	if( ( m_eNodeState != NS_DETAIL )||( IsExistTerrain() == false ) )
		return;
#ifdef TERRAIN_RENDER_DEEPCOPY
#else
	UpdateBuffer();
#endif
	pCsPick->Pick( m_pNiNode, origin, dir, true );

	/*MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBaseObj.begin();	
	for( ; itObject != m_mapBaseObj.end(); ++itObject )
	{
		itObject->second->Picking( pCsPick, origin, dir );
	}*/

	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		if( itObject->second->GetProp( CsGBObjProp::PROP_AV_TBP ) == NULL )
			itObject->second->Picking( pCsPick, origin, dir );
		else
			itObject->second->PropAvTbp_Pick( pCsPick, origin, dir );	
	}

	pCsPick->SetObserveAppCull( false );
	itObject = m_mapCameraCollision.begin();
	for( ; itObject != m_mapCameraCollision.end(); ++itObject )
	{
		if( itObject->second->IsCallPicked() == false )
		{
			itObject->second->PropAvCap_Pick( pCsPick, origin, dir );
		}
	}
	pCsPick->SetObserveAppCull( true );
}

void CsGBTerrainLeaf::Pick_T_TB_TBP_CAP_ByTool( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	if( IsExistTerrain() == false )
		return;

	pCsPick->Pick( m_pNiNode, origin, dir, true );
	/*MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBaseObj.begin();	
	for( ; itObject != m_mapBaseObj.end(); ++itObject )
	{
		itObject->second->Picking( pCsPick, origin, dir );
	}*/

	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		if( itObject->second->GetProp( CsGBObjProp::PROP_AV_TBP ) == NULL )
			itObject->second->Picking( pCsPick, origin, dir );
		else
			itObject->second->PropAvTbp_Pick( pCsPick, origin, dir );	
	}

	pCsPick->SetObserveAppCull( false );
	itObject = m_mapCameraCollision.begin();
	for( ; itObject != m_mapCameraCollision.end(); ++itObject )
	{
		if( itObject->second->IsCallPicked() == false )
		{
			itObject->second->PropAvCap_Pick( pCsPick, origin, dir );
		}
	}
	pCsPick->SetObserveAppCull( true );
}

void CsGBTerrainLeaf::ResetObject_Picking()
{
	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapObject.begin();
	MAP_GBTERRAIN_OBJECT_IT itObjectEnd = m_mapObject.end();
	for( ; itObject != itObjectEnd; ++itObject )
	{
		itObject->second->ResetPicking();
	}
}

void CsGBTerrainLeaf::Reset_TBP_Picking()
{
	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		itObject->second->ResetPicking();
	}
}

void CsGBTerrainLeaf::Reset_TBP_CAP_Picking()
{
	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		itObject->second->ResetPicking();
	}

	itObject = m_mapCameraCollision.begin();
	for( ; itObject != m_mapCameraCollision.end(); ++itObject )
	{
		itObject->second->ResetPicking();
	}
}

void CsGBTerrainLeaf::Reset_TB_TBP_Picking()
{
	/*MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBaseObj.begin();	
	for( ; itObject != m_mapBaseObj.end(); ++itObject )
	{
		itObject->second->ResetPicking();
	}*/

	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		itObject->second->ResetPicking();
	}
}

void CsGBTerrainLeaf::Reset_TB_TBP_CAP_Picking()
{
	/*MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBaseObj.begin();	
	for( ; itObject != m_mapBaseObj.end(); ++itObject )
	{
		itObject->second->ResetPicking();
	}*/

	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		itObject->second->ResetPicking();
	}

	itObject = m_mapCameraCollision.begin();
	for( ; itObject != m_mapCameraCollision.end(); ++itObject )
	{
		itObject->second->ResetPicking();
	}
}

bool CsGBTerrainLeaf::GetTBPickHeight( NiPoint2 ptPos, float& fTBPickHeight )
{
	static NiPoint3 dir = NiPoint3( 0.0f, 0.0f, -1.0f );
	NiPoint3 origin = NiPoint3( ptPos.x, ptPos.y, 1000000.0f );
	CsGBPick pick;
	
	MAP_GBTERRAIN_OBJECT_IT itObject = m_mapBasePickObj.begin();
	for( ; itObject != m_mapBasePickObj.end(); ++itObject )
	{
		if( itObject->second->InBound( ptPos, 0.0f ) == CsGBObject::CT_IN_BOUND )
		{
			if( itObject->second->GetProp( CsGBObjProp::PROP_AV_TBP ) == NULL )
				itObject->second->Picking( &pick, origin, dir );
			else
				itObject->second->PropAvTbp_Pick( &pick, origin, dir );			
		}
	}

	NiPick::Record* pRecord = pick.GetRecord();
	if( pRecord == NULL )
		return false;
	fTBPickHeight = pRecord->GetIntersection().z;
	return true;
}
