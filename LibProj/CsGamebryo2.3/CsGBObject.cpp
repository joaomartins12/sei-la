
#include "stdafx.h"
#include "CsGBObject.h"

CSGBMEMPOOL_CPP( CsGBObject );

CsGBObject::CsGBObject()
{
	m_pUserDefine = NULL;	
	m_pToolInfo = NULL;

	m_pPropFactor = NULL;
	m_pOptionFactor = NULL;
	m_dwParentUniqID = 0;
	m_bCallReady = false;
	m_bEnableRender = false;
	m_nGetID = 0;
	m_bCallPicked = false;


	SetEnableUse( true );
}

CsGBObject::~CsGBObject()
{

}

void CsGBObject::Delete()
{
	if( m_pPropFactor != NULL )
	{
		m_pPropFactor->Delete();
		CsGBObjPropFactor::DeleteInstance( m_pPropFactor );
		m_pPropFactor = NULL;
	}

	if( m_pOptionFactor != NULL )
	{
		m_pOptionFactor->Delete();
		CsGBObjOptFactor::DeleteInstance( m_pOptionFactor );
		m_pOptionFactor = NULL;
	}

	m_Info.s_PathInfo.Delete();
	m_Info.s_PathInfo.Reset();
	m_dwParentUniqID = 0;

	m_Node.Delete();
	m_pUserDefine = NULL;

	SetEnableUse( true );

	// 차일드
	std::list< sCHILD_INFO* >::iterator itChild = m_listChild.begin();
	for( ; itChild!=m_listChild.end(); ++itChild )
	{
		SAFE_DELETE( *itChild );
	}
	m_listChild.clear();	

	// Tool Info
	SAFE_DELETE( m_pToolInfo );
}


void CsGBObject::Init( NiNode* pNiNode, int nGetID, sINFO* pObjectInfo, sUSER_DEFINE* pUserDefine )
{
	assert_cs( m_pPropFactor == NULL );
	assert_cs( m_pOptionFactor == NULL );
	assert_cs( pNiNode != NULL );

	m_dwRefCount = 0;
	m_fAniTime = 0.0f;

	m_nGetID = nGetID;

	// User Define
	memcpy( &m_Info, pObjectInfo, sizeof( sINFO ) );
	assert_cs( m_pUserDefine == NULL );
	m_pUserDefine = pUserDefine;
	m_Info.s_dwUserDefinePlag = pUserDefine->s_dwPlag;

	assert_cs( m_Node.m_pNiNode == NULL );
	m_Node.SetNiObject( pNiNode, CGeometry::UseTR );
	m_Node.m_pNiNode->SetLocalTransform( m_Info.s_trObject );

	_ApplyUserDefine();
	
	m_Node.m_pNiNode->UpdateEffects();
	m_Node.m_pNiNode->UpdateProperties();

	if( UDF_IsAnimation() == false )
	{
		m_Node.m_pNiNode->Update( 0.0f, false );
	}
	else
	{
		if( m_Info.s_bCheckDoAnimation == true )
		{
			m_Node.m_pNiNode->Update( 0.0f );
		}
		else
		{
			m_Node.m_pNiNode->Update( m_Info.s_fStopAniTime );
		}
	}	

	m_bCallReady = true;
	m_bEnableRender = false;
	SetEnableUse( true );

	// ToolInfo
	if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD )
	{
		assert_cs( m_pToolInfo == NULL );
		m_pToolInfo = csnew sTOOL_INFO;
		m_dwParentUniqID = 0;
	}
}

void CsGBObject::DecreaseRefCount()
{
	assert_cs( m_dwRefCount > 0 );
	if( --m_dwRefCount == 0 )
	{
		nsCsGBTerrain::g_ObjectResMgr.DeleteObject( m_Info.s_dwUniqID );
	}
}

void CsGBObject::Update( float fElapsedTime, bool bCulling /*=true*/ )
{
	assert_cs( m_Node.m_pNiNode != NULL );
	if( m_bCallReady == false )
		return;
	m_bCallReady = false;

	if( IsEnableUse() == false )
		return;

	if( m_Info.s_bCheckDoAnimation == true )
		m_fAniTime += fElapsedTime*m_Info.s_fAniSpeed;

	if( m_pPropFactor != NULL )
		m_pPropFactor->Update( fElapsedTime, m_fAniTime );

	if( ( bCulling == true )&&( _UpdateCulling( fElapsedTime, false ) == false ) )
	{
		return;
	}	

	m_bEnableRender = true;	

	if( m_pPropFactor != NULL )
	{
		CsGBObjProp_Alpha* pAlphaProp = (CsGBObjProp_Alpha*)m_pPropFactor->GetProp( CsGBObjProp::PROP_ALPHA );
		if( pAlphaProp != NULL )
		{
			m_Node.SetTempAlpha( pAlphaProp->GetAlphaValue() != 1.0f );
		}
	}	

	if( ( UD_IsNoClipping() || UDF_IsFirstRender() ) == false )
		g_Sorting.GetterTRObject( &m_Node, m_nGetID, CsGBSorting::GT_NONE );
	else if( UDF_IsFirstRender() )
		g_Sorting.GetterTRObject( &m_Node, m_nGetID, CsGBSorting::GT_F1 );
	else if( UD_IsNoClipping() )
		g_Sorting.GetterTRObject( &m_Node, m_nGetID, CsGBSorting::GT_NOCLIPPING );
	else
		assert_cs( false );
}

bool CsGBObject::_UpdateCulling( float fElapsedTime, bool bUpdate )
{
	if( UD_IsAlwaysAnimation() == false )
	{
		if( ( UD_IsNoClipping() == false )&&( CAMERA_ST.UserCulling( m_Node.m_pNiNode ) == false ) )
		{
			return false;
		}

		//if( ( UDF_IsDynamic_Pos() == true )&&
		if( m_Info.s_bCheckDoAnimation == true )
		{
			m_Node.m_pNiNode->Update( m_fAniTime );
			CsGBVisible::OnVisible( &m_Node, m_Node.m_pNiNode, m_pUserDefine->s_dwVisiblePlag, m_fAniTime );
		}
		else
		{
			if( bUpdate == true )
				m_Node.m_pNiNode->Update( 0.0f, false );
			CsGBVisible::OnVisible( &m_Node, m_Node.m_pNiNode, m_pUserDefine->s_dwVisiblePlag, 0.0f );
		}		
	}
	else
	{
		if( m_Info.s_bCheckDoAnimation == true )
		{			
			m_Node.m_pNiNode->Update( m_fAniTime );
		}
		CsGBVisible::OnVisible( &m_Node, m_Node.m_pNiNode, m_pUserDefine->s_dwVisiblePlag, m_fAniTime );
		return UD_IsNoClipping() || CAMERA_ST.UserCulling( m_Node.m_pNiNode );
	}
	return true;
}

//===========================================================================================
//
//	Light
//
//===========================================================================================
void CsGBObject::ApplyLight( CsGBLight* pLight )
{
	if( UD_IsNoLight() == true )
		return;

	m_Node.m_pNiNode->AttachEffect( pLight->GetNiLight() );
	m_Node.m_pNiNode->UpdateEffects();
}

void CsGBObject::ReleaseLight( CsGBLight* pLight )
{
	if( UD_IsNoLight() == true )
		return;

	m_Node.m_pNiNode->DetachEffect( pLight->GetNiLight() );
	m_Node.m_pNiNode->UpdateEffects();
}

//===========================================================================================
//
//	Info
//
//===========================================================================================

void CsGBObject::SetScale( float fScale )
{
	m_Info.s_trObject.m_fScale = fScale;
	m_Node.m_pNiNode->SetScale( fScale );
	m_Node.m_pNiNode->Update( 0.0f, false );
}

CsGBObject*	CsGBObject::Clone()
{
	CsGBObject* pRetun = nsCsGBTerrain::g_ObjectResMgr.GetObject( m_Info.s_dwUniqID );
	memcpy( pRetun->GetInfo(), &m_Info, sizeof( sINFO ) );
	pRetun->IncreaseRefCount();
	
	return pRetun;
}


//===========================================================================================
//
//	User Define
//
//===========================================================================================
void CsGBObject::SetTextureType( BYTE btGroup, BYTE btTextureType )
{
	GlobalSetTextureType( m_Node.m_pNiNode, &m_Info, m_pUserDefine, btGroup, btTextureType );
}

void CsGBObject::GlobalSetTextureType( NiNode* pNiNode, sINFO* pInfo, sUSER_DEFINE* pUserDefine, BYTE btGroup, BYTE btTextureType )
{
	assert_cs( pNiNode != NULL );
	assert_cs( pInfo != NULL );
	assert_cs( pUserDefine != NULL );
	if( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_USE_TEXTYPE ) == 0 )
		return;
	// 같은 텍스쳐라면 리턴
	if( pInfo->GetTextureTypeIndex( btGroup ) == btTextureType )
		return;

	char cCurPath[ MAX_PATH ] = {0, };
	char cTexPath[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( cCurPath, OBJECT_PATH_LEN, pInfo->s_cObjectPath );

	NiAVObject* pChild = nsCSGBFUNC::GetEnableFirstNode( pNiNode );	
	

	sUDD_TexType::sTEXTURE_GROUP* pTexGrop = NULL;
	NiTriBasedGeom* pTriObject = NULL;
	
	if( NiIsKindOf( NiNode, pChild ) == true )
	{
		pTexGrop = ( (sUDD_TexType*)pUserDefine->GetData( sUDD_TexType::Type() ) )->s_mapTextureGroup[ btGroup ];
		pTriObject = (NiTriBasedGeom*)( (NiNode*)pChild )->GetAt( pTexGrop->s_btMaterialID );
		NiTexturingProperty* pOrgProp = NiDynamicCast( NiTexturingProperty, pTriObject->GetProperty( NiTexturingProperty::GetType() ) );
		// 텍스쳐 경로
		sprintf_s( cTexPath, MAX_PATH, "%s\\%s", cCurPath, pTexGrop->s_mapFileName[ btTextureType ]->s_cFileName );
		assert_csm1( _access( cTexPath, 0 ) == 0, "로드할 텍스쳐가 존재하지 않습니다.\n\n%s", cTexPath );
		NiTexturingPropertyPtr pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture( cTexPath, pOrgProp->GetApplyMode() );

		NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( *pTexProp->GetBaseMap() );
		pOrgProp->SetBaseMap( pMap );
		pTriObject->UpdateProperties();

		pTexProp = 0;
	}
	else
	{
		pTexGrop = ( (sUDD_TexType*)pUserDefine->GetData( sUDD_TexType::Type() ) )->s_mapTextureGroup[ btGroup ];
		assert_cs( btGroup == 0 );
		pTriObject = (NiTriBasedGeom*)pChild;
		assert_cs( NiIsKindOf( NiTriBasedGeom, pTriObject ) == true );
		NiTexturingProperty* pOrgProp = NiDynamicCast( NiTexturingProperty, pTriObject->GetProperty( NiTexturingProperty::GetType() ) );
		// 텍스쳐 경로
		sprintf_s( cTexPath, MAX_PATH, "%s\\%s", cCurPath, pTexGrop->s_mapFileName[ btTextureType ]->s_cFileName );
		assert_csm1( _access( cTexPath, 0 ) == 0, "로드할 텍스쳐가 존재하지 않습니다.\n\n%s", cTexPath );
		NiTexturingPropertyPtr pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture( cTexPath, pOrgProp->GetApplyMode() );
		
		NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( *pTexProp->GetBaseMap() );
		pOrgProp->SetBaseMap( pMap );
		pTriObject->UpdateProperties();

		pTexProp = 0;
	}

	pInfo->SetTextureTypeIndex( btGroup, btTextureType );
}

void CsGBObject::_ApplyUserDefine()
{
	// ===== 카메라 알파		
	if( UDF_IsCameraAlpha() == true )
	{
		_GetPropFactor()->InsertProp( CsGBObjProp::PROP_ALPHA, this );

		// 바운딩 볼륨 포인터 저장
		if( UD_IsCameraAlpha() == true )
		{
			m_pPropFactor->InsertProp( CsGBObjProp::PROP_ABV, this );
		}
	}

	// 카메라 픽킹 ( 픽킹오브젝트 포인터 저장 )
	if( UDF_IsCameraPick() == true )
	{
		_GetPropFactor()->InsertProp( CsGBObjProp::PROP_AV_CAP, this );
	}

	// 터레인 하이드 픽
	if( UD_IsTerrainHidePick() == true )
	{
		_GetPropFactor()->InsertProp( CsGBObjProp::PROP_AV_TBP, this );
	}

	// 쉐도우 픽
	/*if( UD_IsShadowPick() == true )
	{
		_GetPropFactor()->InsertProp( CsGBObjProp::PROP_AV_SP, this );
	}*/

	if( UD_IsNoFog() == false )
	{
		m_Node.m_pNiNode->AttachProperty( nsCsGBTerrain::g_pCurRoot->GetFogProperty() );
	}
	
	if( UD_IsNoLight() == false )
	{
		nsCsGBTerrain::g_pCurRoot->GetLightMng()->ApplyObject( m_Node.m_pNiNode );
		//m_Node.m_pNiNode->AttachEffect( nsCsGBTerrain::g_pCurRoot->GetLight_Main() );
	}

	// 랜덤 애니메이션
	if( UD_IsAnimationRandom() == true )
	{
		sUDD_ANI_RANDOM* pUDD = (sUDD_ANI_RANDOM*)m_pUserDefine->GetData( sUDD_ANI_RANDOM::Type() );
		m_fAniTime = ( pUDD->s_nStartFrame + rand()%( pUDD->s_nEndFrame - pUDD->s_nStartFrame ) )*0.001f;
	}

	// 사운드
	if( UD_IsSound() == true )
	{
		_GetPropFactor()->InsertProp( CsGBObjProp::PROP_SOUND, this );
	}

	// ActorMng
	if( UD_IsActorManager() == true )
	{
		CsGBObjProp_Actor* pProp = (CsGBObjProp_Actor*)_GetPropFactor()->InsertProp( CsGBObjProp::PROP_ACTOR, this );
		sUDD_ACTOR* pUDD = (sUDD_ACTOR*)m_pUserDefine->GetData( sUDD_ACTOR::Type() );
		pProp->InitActor( GetInfo()->s_cObjectPath, pUDD );
	}

	// Path
	if( UD_IsPath() == true )
	{
		sUDD_PATH* pUDD = (sUDD_PATH*)m_pUserDefine->GetData( sUDD_PATH::Type() );
		pUDD->s_pDynamicAgent = g_pCsPathEngine->SetDynamicShape( (float)pUDD->s_nPos[ 0 ], (float)pUDD->s_nPos[ 1 ], (long)pUDD->s_nData[ 0 ], (long)pUDD->s_nData[ 1 ] );
	}
	

	GlobalApplyUserDefine( m_Node.m_pNiNode, &m_Info, m_pUserDefine );
}

void CsGBObject::GlobalApplyUserDefine( CsGBObject* pCsGBObject )
{
	GlobalApplyUserDefine( pCsGBObject->GetNiNode(), pCsGBObject->GetInfo(), pCsGBObject->GetUserDefine() );
}

void CsGBObject::GlobalApplyUserDefine( NiNode* pNiNode, sINFO* pInfo, sUSER_DEFINE* pUserDefine )
{
	assert_cs( pNiNode != NULL );
	assert_cs( pInfo != NULL );
	assert_cs( pUserDefine != NULL );
	if( ( ( pUserDefine->s_dwPlag & sUSER_DEFINE::UD_USE_TEXTYPE ) != 0 )&&( pInfo->s_dwTexTypePlag != 0 ) )
	{
		static char cCurPath[ MAX_PATH ] = {0, };
		static char cTexPath[ MAX_PATH ] = {0, };
		nsCSFILE::GetFilePathOnly( cCurPath, OBJECT_PATH_LEN, pInfo->s_cObjectPath );

		NiAVObject* pChild = nsCSGBFUNC::GetEnableFirstNode( pNiNode );
		
		BYTE btTexTypeGroupCount = (BYTE)( (sUDD_TexType*)pUserDefine->GetData( sUDD_TexType::Type() ) )->s_mapTextureGroup.size();
		assert_cs( btTexTypeGroupCount > 0 );
		sUDD_TexType::sTEXTURE_GROUP* pTexGrop = NULL;
		NiTriBasedGeom* pTriObject = NULL;
		
		if( NiIsKindOf( NiNode, pChild ) == true )
		{
			for( BYTE btGroup=0; btGroup<btTexTypeGroupCount; ++btGroup )
			{
				int nTextureTypeIndex = pInfo->GetTextureTypeIndex( btGroup );
				if( nTextureTypeIndex == 0 )
					continue;

				pTexGrop = ( (sUDD_TexType*)pUserDefine->GetData( sUDD_TexType::Type() ) )->s_mapTextureGroup[ btGroup ];
				pTriObject = (NiTriBasedGeom*)( (NiNode*)pChild )->GetAt( pTexGrop->s_btMaterialID );
				assert_csm1( NiIsKindOf( NiTriBasedGeom, pTriObject ) == true, "올바른 형식의 오브젝트가 아닙니다.\n오브젝트는 단일 메쉬여야 합니다.\n\n%s", pInfo->s_cObjectPath );
				NiTexturingProperty* pOrgProp = NiDynamicCast( NiTexturingProperty, pTriObject->GetProperty( NiTexturingProperty::GetType() ) );
				assert_csm1( pOrgProp != NULL, "오브젝트에 텍스쳐가 없는상태로 익스포트 하면 안됩니다.\n폴더안에 해당 텍스쳐가 있을필요는 없습니다.\n\n%s", pInfo->s_cObjectPath );
				// 텍스쳐 경로
				sprintf_s( cTexPath, MAX_PATH, "%s\\%s", cCurPath, pTexGrop->s_mapFileName[ nTextureTypeIndex ]->s_cFileName );
				//assert_csm1( _access( cTexPath, 0 ) == 0, "로드할 텍스쳐가 존재하지 않습니다.\n\n%s", cTexPath );
				NiTexturingPropertyPtr pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture( cTexPath, pOrgProp->GetApplyMode() );
				NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( *pTexProp->GetBaseMap() );
				pOrgProp->SetBaseMap( pMap );
				pTriObject->UpdateProperties();

				pTexProp = 0;
			}			
		}
		else
		{
			pTexGrop = ( (sUDD_TexType*)pUserDefine->GetData( sUDD_TexType::Type() ) )->s_mapTextureGroup[ 0 ];
			assert_csm1( btTexTypeGroupCount == 1, "멀티앤 서브 오브젝트가 아닌것은 TextureType 형식이 하나여야 합니다.\n\n%s", pInfo->s_cObjectPath );
			assert_csm1( NiIsKindOf( NiTriBasedGeom, pChild ) == true, "올바른 형식의 오브젝트가 아닙니다.\n오브젝트는 단일 메쉬여야 합니다.\n\n%s", pInfo->s_cObjectPath );
			assert_csm1( pTexGrop->s_btMaterialID == 0 , "멀티앤 서브 오브젝트가 아닌것은 Material ID 가 1 이여야만 합니다.\n\n%s", pInfo->s_cObjectPath );
			pTriObject = (NiTriBasedGeom*)pChild;
			NiTexturingProperty* pOrgProp = NiDynamicCast( NiTexturingProperty, pTriObject->GetProperty( NiTexturingProperty::GetType() ) );
			assert_csm1( pOrgProp != NULL, "오브젝트에 텍스쳐가 없는상태로 익스포트 하면 안됩니다.\n폴더안에 해당 텍스쳐가 있을필요는 없습니다.\n\n%s", pInfo->s_cObjectPath );
			// 텍스쳐 경로
			sprintf_s( cTexPath, MAX_PATH, "%s\\%s", cCurPath, pTexGrop->s_mapFileName[ pInfo->GetTextureTypeIndex( 0 ) ]->s_cFileName );
			//assert_csm1( _access( cTexPath, 0 ) == 0, "로드할 텍스쳐가 존재하지 않습니다.\n\n%s", cTexPath );			
			NiTexturingPropertyPtr pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture( cTexPath, pOrgProp->GetApplyMode() );
			NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( *pTexProp->GetBaseMap() );
			pOrgProp->SetBaseMap( pMap );
			pTriObject->UpdateProperties();

			pTexProp = 0;
		}		
	}
}

void CsGBObject::Reposition( float fDeltaHeight )
{
	if( fDeltaHeight == 0.0f )
		return;

	m_Info.s_trObject.m_Translate.z += fDeltaHeight;
	m_Node.m_pNiNode->SetLocalTransform( m_Info.s_trObject );
	m_Node.m_pNiNode->Update( 0.0f );
}

void CsGBObject::SetTransform( NiTransform tr )
{
	m_Info.s_trObject = tr;
	m_Node.m_pNiNode->SetLocalTransform( tr );
	m_Node.m_pNiNode->Update( 0.0f );
}

//==========================================================================================
//
//		Shader
//
//==========================================================================================
void CsGBObject::SetShader( NiSingleShaderMaterial* pShader )
{
	int nSize = m_Node.m_vpGeom.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_Node.m_vpGeom[ i ]->m_pGeometry->ApplyAndSetActiveMaterial( pShader );
	}
}

//==========================================================================================
//
//		Child
//
//==========================================================================================

void CsGBObject::DeleteChild_FromUniqID( DWORD dwUniqID )
{	
	std::list< sCHILD_INFO* >::iterator it = m_listChild.begin();
	for( ; it!=m_listChild.end(); ++it )
	{
		if( (*it)->s_dwChildUniqID == dwUniqID )
		{
			SAFE_DELETE( *it );
			m_listChild.erase( it );
			return;
		}		
	}
	assert_cs( false );
}

void CsGBObject::InsertChild( sCHILD_INFO ci )
{
	sCHILD_INFO* pci = csnew sCHILD_INFO;
	*pci = ci;
	m_listChild.push_back( pci );
}

CsGBObject::sCHILD_INFO* CsGBObject::GetChild( int nChildIndex )
{
	assert_cs( nChildIndex >= 0 );
	std::list< sCHILD_INFO* >::iterator it = m_listChild.begin();
	for( ; it!=m_listChild.end(); ++it )
	{
		if( nChildIndex == 0 )
			return (*it);
		--nChildIndex;
	}
	assert_cs( false );
	return NULL;
}

CsGBObject::sCHILD_INFO* CsGBObject::GetChild_FormSlotID( int nSlotID )
{
	assert_cs( nSlotID >= 0 );
	std::list< sCHILD_INFO* >::iterator it = m_listChild.begin();
	for( ; it!=m_listChild.end(); ++it )
	{
		if( (*it)->s_dwSlotID == nSlotID )
			return (*it);		
	}
	return NULL;
}

//==========================================================================================
//
//		Extra Data
//
//==========================================================================================

void CsGBObject::PreSaveExtraData()
{
	if( m_bCallReady == false )
		return;

	// Option 체크
	if( UD_IsDarkMap() == true )
	{
		if( _GetOptFactor()->IsOption( CsGBObjOpt::OPT_DARKMAP ) == false )
		{
			CsGBObjOpt_DarkMap* pOption = (CsGBObjOpt_DarkMap*)m_pOptionFactor->InsertOption( CsGBObjOpt::OPT_DARKMAP, this );
			pOption->CalDMObjectData();
		}
	}
}

UINT CsGBObject::GetSaveSize()
{
	if( m_bCallReady == true )
	{
		UINT uiReturn = 0;

		// 기본 정보
		uiReturn +=	sizeof( sFILE_HEADER ) +
					sizeof( sINFO );

		// 패쓰엔진
		uiReturn +=	sizeof( NiPoint2 )*m_Info.s_PathInfo.s_uiPEGroupCount +
					sizeof( USHORT )*m_Info.s_PathInfo.s_uiPEGroupCount +
					sizeof( long )*2*m_Info.s_PathInfo.s_uiTotalPECount;

		// 옵션
		uiReturn += _GetOptFactor()->GetSaveSize();

		return uiReturn;
	}

	// 이미 호출된 적이 있다면
	return	sizeof( sFILE_HEADER ) +
			sizeof( sEXIST_INFO );
}

bool CsGBObject::SaveExtraData( char* pData, UINT& uiOffset )
{
	sFILE_HEADER fh;
	sEXIST_INFO ei;

	fh.s_dwInfoSize = GetSaveSize();
	if( IsCallReady() == false )
	{
		fh.s_dwSaveType = ST_EXIST_OBJECT;
		memcpy( &pData[ uiOffset ], &fh, sizeof( sFILE_HEADER ) );
		uiOffset += sizeof( sFILE_HEADER );

		ei.s_dwUniqID = m_Info.s_dwUniqID;
		memcpy( &pData[ uiOffset ], &ei, sizeof( CsGBObject::sEXIST_INFO ) );
		uiOffset += sizeof( CsGBObject::sEXIST_INFO );

		return false; 
	}

	fh.s_dwSaveType = CsGBObject::ST_FIRST_OBJECT;

	memcpy( &pData[ uiOffset ], &fh, sizeof( sFILE_HEADER ) );
	uiOffset += sizeof( sFILE_HEADER );

	memcpy( &pData[ uiOffset ], &m_Info, sizeof( sINFO ) );
	uiOffset += sizeof( sINFO );

	// Path Engine
	UINT uiTotalPECount = m_Info.s_PathInfo.s_uiTotalPECount;
	if( uiTotalPECount != 0 )
	{
		UINT uiPEGroup = m_Info.s_PathInfo.s_uiPEGroupCount;
		assert_cs( uiPEGroup != 0 );
		memcpy( &pData[ uiOffset ], m_Info.s_PathInfo.s_pptTranse, sizeof( NiPoint2 )*uiPEGroup );
		uiOffset += ( sizeof( NiPoint2 )*uiPEGroup );
		memcpy( &pData[ uiOffset ], m_Info.s_PathInfo.s_pPECount, sizeof( USHORT )*uiPEGroup );
		uiOffset += ( sizeof( USHORT )*uiPEGroup );
		memcpy( &pData[ uiOffset ], m_Info.s_PathInfo.s_pData, sizeof( long )*2*uiTotalPECount );
		uiOffset += ( sizeof( long )*2*uiTotalPECount );
	}

	_GetOptFactor()->Save( pData, uiOffset );	

	return true;
}

void CsGBObject::LoadExtraData( char* pData, UINT& uiOffset, DWORD dwCheckPlag )
{
	// 다르다면 분석
	if( dwCheckPlag != m_Info.s_dwUserDefinePlag )
	{
		// 다크맵 사용으로 저장 되었다면
		if( ( dwCheckPlag & sUSER_DEFINE::UD_DARKMAP ) != 0 )
		{
			// 현재 유저 디판인도 다크맵이라면 적용
			if( ( m_pUserDefine != NULL )&&( UD_IsDarkMap() == true ) )
			{
				CsGBObjOpt* pOption = _GetOptFactor()->InsertOption( CsGBObjOpt::OPT_DARKMAP, this );
				pOption->SetData( pData, uiOffset );
			}
			// 유저 디파인은 다크맵이 아니라면 옵셋 건너 뛰자
			else
			{
				UINT uiUVCount, uiPixelCount;
				memcpy( &uiUVCount, &pData[ uiOffset ], sizeof( UINT ) );
				uiOffset += sizeof( UINT );
				uiOffset += sizeof( NiPoint2 )*uiUVCount;
				// pixel Save
				memcpy( &uiPixelCount, &pData[ uiOffset ], sizeof( UINT ) );			
				uiOffset += sizeof( UINT );
				uiOffset += sizeof( BYTE )*3*uiPixelCount;
			}
		}
		//else if( UD_IsDarkMap() == true )
		//{
		//	UINT uiUVCount, uiPixelCount;
		//	memcpy( &uiUVCount, &pData[ uiOffset ], sizeof( UINT ) );
		//	uiOffset += sizeof( UINT );
		//	uiOffset += sizeof( NiPoint2 )*uiUVCount;
		//	// pixel Save
		//	memcpy( &uiPixelCount, &pData[ uiOffset ], sizeof( UINT ) );			
		//	uiOffset += sizeof( UINT );
		//	uiOffset += sizeof( BYTE )*uiPixelCount;
		//}
		
		// 유저디파인이 포탈 상태도 아니면서 포탈상태로 저장 되었다면 옵셋 건너 뛰자
		if( ( GetType() != OT_PORTAL )&&( ( dwCheckPlag & sUSER_DEFINE::UD_PORTAL ) != 0 ) )
		{
			uiOffset += CsGBObj_Portal::SelfSaveSize();
		}
	}
	// 같다면 적용
	else
	{
		assert_cs( m_pUserDefine != NULL );
		assert_cs( m_pUserDefine->s_dwPlag == m_Info.s_dwUserDefinePlag );
		if( ( dwCheckPlag & sUSER_DEFINE::UD_DARKMAP ) != 0 )
		{
			CsGBObjOpt* pOption = _GetOptFactor()->InsertOption( CsGBObjOpt::OPT_DARKMAP, this );
			pOption->SetData( pData, uiOffset );
		}
	}	
}

void CsGBObject::LoadObjectInfo( sINFO* pInfo, char* pData, UINT& uiOffset )
{
	assert_cs( pInfo != NULL );
	memcpy( pInfo, &pData[ uiOffset ], sizeof( sINFO ) );
	pInfo->s_dwUniqID += CsGBTerrainLeaf::m_dwObjectUniqIDConstant;
	uiOffset += sizeof( sINFO );
	pInfo->s_PathInfo.s_pData = NULL;
	// Path Engine
	if( pInfo->s_PathInfo.s_uiPEGroupCount != 0 )
	{
		pInfo->s_PathInfo.s_pPECount = csnew USHORT[ pInfo->s_PathInfo.s_uiPEGroupCount ];
		pInfo->s_PathInfo.s_pptTranse = NiNew NiPoint2[ pInfo->s_PathInfo.s_uiPEGroupCount ];
		pInfo->s_PathInfo.s_pData = csnew long[ 2*pInfo->s_PathInfo.s_uiTotalPECount ];
		memcpy( pInfo->s_PathInfo.s_pptTranse, &pData[ uiOffset ], sizeof( NiPoint2 )*pInfo->s_PathInfo.s_uiPEGroupCount );
		uiOffset += ( sizeof( NiPoint2 )*pInfo->s_PathInfo.s_uiPEGroupCount );
		memcpy( pInfo->s_PathInfo.s_pPECount, &pData[ uiOffset ], sizeof( USHORT )*pInfo->s_PathInfo.s_uiPEGroupCount );
		uiOffset += ( sizeof( USHORT )*pInfo->s_PathInfo.s_uiPEGroupCount );
		memcpy( pInfo->s_PathInfo.s_pData, &pData[ uiOffset ], sizeof( long )*2*pInfo->s_PathInfo.s_uiTotalPECount );
		uiOffset += ( sizeof( long )*2*pInfo->s_PathInfo.s_uiTotalPECount );
	}
}


UINT CsGBObject::GetSaveToolSize()
{
	if( m_bCallReady == true )
	{
		if( m_listChild.size() == 0 )
			return 0;

		return	sizeof( sTOOL_INFO ) + (UINT)m_listChild.size()*sizeof( sCHILD_INFO );
	}
	return 0;
}

bool CsGBObject::SaveToolExtraData( char* pData, UINT& uiOffset )
{
	if( IsCallReady() == true )
	{
		sTOOL_INFO ti;
		ti.s_nChildCount = (int)m_listChild.size();
		if( ti.s_nChildCount == 0 )
			return false;

		ti.s_dwHaveChildUniqID = m_Info.s_dwUniqID;
		memcpy( &pData[ uiOffset ], &ti, sizeof( sTOOL_INFO ) );
		uiOffset += sizeof( sTOOL_INFO );

		std::list< sCHILD_INFO* >::iterator it = m_listChild.begin();
		for( ; it!=m_listChild.end(); ++it )
		{
			memcpy( &pData[ uiOffset ], (*it), sizeof( sCHILD_INFO ) );
			uiOffset += sizeof( sCHILD_INFO );
		}
	}
	return true;
}


void CsGBObject::LoadToolExtraData( char* pData, UINT& uiOffset )
{
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );	
	assert_cs( m_bCallReady == true );
	m_bCallReady = false;

	memcpy( m_pToolInfo, &pData[ uiOffset ], sizeof( sTOOL_INFO ) );
	m_pToolInfo->s_dwHaveChildUniqID += CsGBTerrainLeaf::m_dwObjectUniqIDConstant;
	uiOffset += sizeof( sTOOL_INFO );

	assert_cs( m_pToolInfo->s_dwHaveChildUniqID == m_Info.s_dwUniqID );
	for( int i=0; i<m_pToolInfo->s_nChildCount; ++i )
	{
		uiOffset += sizeof( sCHILD_INFO );

		/*sCHILD_INFO* ci = csnew sCHILD_INFO;
		memcpy( ci, &pData[ uiOffset ], sizeof( sCHILD_INFO ) );
		ci->s_dwChildUniqID += CsGBTerrainLeaf::m_dwObjectUniqIDConstant;
		uiOffset += sizeof( sCHILD_INFO );

		nsCsGBTerrain::g_pCurRoot->FindCsGBObject( ci->s_dwChildUniqID )->SetParentID( m_Info.s_dwUniqID );

		m_listChild.push_back( ci );*/
	}
}


void CsGBObject::CheckShader()
{
	if( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD )
		return;

	// 동적 그림자 DestObject
	if( ( UD_IsTerrainBasePick() == true )||( UD_IsTerrainHidePick() == true ) )
	{
		if( ( nsCsGBTerrain::g_bShadowRender == true )&&( g_pShadow->GetShadowLight() ) )
		{
			NiAVObject* pAvObject = nsCSGBFUNC::GetFirst_HaveTexProp( GetCsNode()->m_pNiNode );
			while( pAvObject )
			{
				char shader[ 32 ] = "ZS_TO_";
				NiTexturingProperty* pTexProp = (NiTexturingProperty*)pAvObject->GetProperty( NiTexturingProperty::GetType() );

				NiTexturingProperty::ShaderMap* pMap = NiNew NiTexturingProperty::ShaderMap( g_pShadow->GetZBufferTex(), 0 );
				pTexProp->SetShaderMap( 0, pMap );

				assert_cs( pTexProp );
				if( pTexProp->GetDarkMap() == NULL )			
					strcat_s( shader, 32, "No" );			
				strcat_s( shader, 32, "Dark_" );

				if( pTexProp->GetApplyMode() != NiTexturingProperty::APPLY_REPLACE )
				{
					strcat_s( shader, 32, nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetShaderTrExt() );
				}
				else
				{
					strcat_s( shader, 32, "Replace" );
				}

				assert_cs( NiIsKindOf( NiGeometry, pAvObject ) == true );
				( (NiGeometry*)pAvObject )->ApplyAndSetActiveMaterial( NiSingleShaderMaterial::Create( shader ) );
				pAvObject = nsCSGBFUNC::GetNext_HaveTexProp( GetCsNode()->m_pNiNode, pAvObject );
			}
		}
		else
		{
			NiAVObject* pAvObject = nsCSGBFUNC::GetFirst_HaveTexProp( GetCsNode()->m_pNiNode );
			while( pAvObject )
			{
				NiTexturingProperty* pTexProp = (NiTexturingProperty*)pAvObject->GetProperty( NiTexturingProperty::GetType() );
				pTexProp->SetShaderMap( 0, NULL );

				assert_cs( NiIsKindOf( NiGeometry, pAvObject ) == true );
				( (NiGeometry*)pAvObject )->ApplyAndSetActiveMaterial( nsCsGBTerrain::g_pRenderer->GetDefaultMaterial() );
				pAvObject = nsCSGBFUNC::GetNext_HaveTexProp( GetCsNode()->m_pNiNode, pAvObject );
			}
		}
	}
}

void CsGBObject::ResetTextureFilterMode()
{
	// 모든 텍스쳐에 TEXTURE_FILTER_TYPE 설정
	nsCSGBFUNC::SetTextureFilterMode( m_Node.m_pNiNode, nsCsGBTerrain::g_eTexFilter );
}
//////////////////////////////////////////////////////////////////////////

void CsGBObject::PropAvCap_Pick( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir, bool bAppend/*=true*/ )
{
	CsGBObjProp* pObjProp = m_pPropFactor->GetProp( CsGBObjProp::PROP_AV_CAP );
	if( NULL == pObjProp )
		return;

	CsGBObjProp_AvCap* pAvCap = (CsGBObjProp_AvCap*)pObjProp;
	pAvCap->Pick( pCsPick, origin, dir, bAppend );
}

void CsGBObject::PropAvTbp_Pick( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir, bool bAppend/*=true*/ )
{	
	CsGBObjProp* pObjProp = m_pPropFactor->GetProp( CsGBObjProp::PROP_AV_TBP );
	if( NULL == pObjProp )
		return;

	CsGBObjProp_AvTbp* pTbp = (CsGBObjProp_AvTbp*)pObjProp;
	pTbp->Pick( pCsPick, origin, dir, bAppend );
}

//===========================================================================================
//
//	Collision
//
//===========================================================================================
CsGBObject::eCOLLISION_TYPE CsGBObject::InBound( NiPoint3 ptCenter, float fRadius )
{
	/*if( m_bUpdatedCollision == true )
		return CT_CALLED_OBJECT;
	
	m_bUpdatedCollision = true;*/
	NiBound bound = m_Node.m_pNiNode->GetWorldBound();
	if( bound.GetRadius() + fRadius > ( bound.GetCenter() - ptCenter ).Length() )
		return CT_IN_BOUND;
	return CT_OUT_BOUND;
}

CsGBObject::eCOLLISION_TYPE CsGBObject::InBound( NiPoint2 ptCenter, float fRadius )
{
	/*if( m_bUpdatedCollision == true )
		return CT_CALLED_OBJECT;
	m_bUpdatedCollision = true;*/

	NiBound bound = m_Node.m_pNiNode->GetWorldBound();
	NiPoint2 ptSrcCenter = NiPoint2( bound.GetCenter().x, bound.GetCenter().y );
	if( bound.GetRadius() + fRadius > ( ptSrcCenter - ptCenter ).Length() )
		return CT_IN_BOUND;
	return CT_OUT_BOUND;
}

void CsGBObject::Picking( CsGBPick* pCsPick, NiPoint3 origin, NiPoint3 dir )
{
	if( m_bCallPicked == true )
		return;
		
	m_bCallPicked = true;

	pCsPick->Pick( m_Node.m_pNiNode, origin, dir, true );
}
