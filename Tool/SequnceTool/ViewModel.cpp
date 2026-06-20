
#include "stdafx.h"
#include "ViewModel.h"



CViewModel	g_ViewModel;

CViewModel::CViewModel()
{
	m_pModel = NULL;
	m_pDestModel = NULL;
	m_bRenderClientBox = false;
}


bool CViewModel::CALLBACK_ThreadDelete( sTCUnit* pUnit )
{
	CsC_Object* pObject = pUnit->s_pLoadedObject;

	UINT nFTID = pObject->GetFTID();
	pObject->Delete();

	// 파트
	if( nFTID / 10000 == 8 )
	{
		CPartChar::DeleteInstance( (CPartChar*)pObject );
		return true;
	}
	else
	{
		CNormalChar::DeleteInstance( (CNormalChar*)pObject );
		return true;
	}

	return false;
}

void CViewModel::Delete()
{
	nsCsFileTable::g_FileTableMng.Delete();

	m_Eff0.Delete();
	m_Eff1.Delete();
	m_Plan.Delete();
	m_OffsetMesh.Delete();

	m_ClientBox.Delete();
	CAMERA_ST.Delete();	

	DESTROY_SINGLETON( CAMERA_STPTR );

	DeleteSrcModel();
	DeleteDestModel();

	CsC_Thread::GlobalShotDown();	
	_DeleteDefaultLight();

	THREAD_MEMPOOL_DELETE( CNormalChar )
	THREAD_MEMPOOL_DELETE( CPartChar )
	CsGBTerrainMng::DeleteMempool();
}

void CViewModel::Init()
{
	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapList = false;
	nsCsMapTable::g_bUseMapPortal = false;
	nsCsMapTable::g_bUseMapNpc = false;
	nsCsMapTable::g_bUseMapRegion = false;
	nsCsMapTable::g_bUseMapResurrection = false;
	//nsCsMapTable::g_bUseMapAreaBuff = false;
	nsCsMapTable::g_bUseMapCharLight = false;
	nsCsMapTable::g_bUseWeather = false;
	nsCsMapTable::g_bUseTableObj = false;
	nsCsMapTable::g_bUseActObject = false;

	nsCsFileTable::AllFalse();
	nsCsFileTable::g_bUseDigimon = true;
	nsCsFileTable::g_bUseNpc = true;
	nsCsFileTable::g_bUseBase = true;
	nsCsFileTable::g_bUseSkill = true;


	Csini x( ".\\__ToolLocal.ini" );
	nsCsFileTable::eLANGUAGE lan = nsCsFileTable::KOREA_TRANSLATION;

#ifdef VERSION_USA	//GSP 버전은 고정으로 영문버전
	lan = nsCsFileTable::ENGLISH;
	CsMessageBox( MB_OK, L"이것은 영문 버젼이에요.~!" );

#elif VERSION_HK	// 홍콩 버전 _ 컨텐츠가 다르므로 별도 관리
	lan = nsCsFileTable::HONGKONG;
	CsMessageBox( MB_OK, L"이것은 홍콩 버젼이에요.~!" );

#elif VERSION_TW	// 대만 버전 _ 컨텐츠가 다르므로 별도 관리
	lan = nsCsFileTable::TAIWAN;
	CsMessageBox( MB_OK, L"이것은 대만 버젼이에요.~!" );

#elif VERSION_TH
	lan = nsCsFileTable::THAILAND;
	CsMessageBox( MB_OK, L"สวัสดีครับ ยินดีที่ได้รู้จักครับ~!" );

#else
	lan = nsCsFileTable::KOREA_TRANSLATION;
	//CsMessageBox( MB_OK, L"이것은 한국 버젼이에요.~!" );		
#endif


#ifdef _DEBUG
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, lan );
#else
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, lan );	
#endif
	CsC_Thread::GlobalInit();
	g_pThread->UseThread( false );

	// 콜백 등록
	g_pThread->ResistCallBack_DeleteUnit( CALLBACK_ThreadDelete );
	_CreateDefaultLight();

	char cShader[ MAX_PATH ] = ".\\Data\\Shaders";
	char* apcShaderDirectories[1];
	apcShaderDirectories[0] = cShader;
	m_ShaderHelper.SetupShaderSystem( NULL, 0, apcShaderDirectories, 1 );

	CREATE_SINGLETON( CsGBCamera );

	sCAMERAINFO info;
	info.s_fFarPlane = 10000.0f;
	info.s_fInitPitch = 0.0f;
	info.s_fInitRoll = -NI_HALF_PI;
	info.s_fDist = 700.0f;
	CAMERA_ST.Create( g_pRenderer, &info );
	CAMERA_ST.SetDistRange( 100.0f, 5000.0f );
	CAMERA_ST.SetDistAccel( 0.7f );
	CAMERA_ST.SetTranslate( NiPoint3( 0, 0, 0 ) );
	CAMERA_ST.SetTranslateAccel( 0.27f );


	CsGeomBox::sINFO GeomInfo;
	GeomInfo.s_fAlpha = 1.0f;
	GeomInfo.s_Color = NiColor( 1.0f, 0, 1.0f );
	GeomInfo.s_bTwoSide = true;
	GeomInfo.s_bWireFrame = true;
	m_ClientBox.SetNiObject( CsGeomBox::CreateBox( NiPoint3( 0, 0, 0 ), NiPoint3( 0, 0, 0 ), &GeomInfo ), CGeometry::Normal );	

	{
		NiStream stream;
		stream.Load( "ToolData\\Ef0.nif" );
		NiNode* pNode = (NiNode*)stream.GetObjectAt( 0 );
		pNode->AttachEffect( m_pDefaultLight );
		pNode->UpdateEffects();
		m_Eff0.SetNiObject( pNode, CGeometry::Normal );
	}

	{
		NiStream stream;
		stream.Load( "ToolData\\Ef1.nif" );
		NiNode* pNode = (NiNode*)stream.GetObjectAt( 0 );
		pNode->AttachEffect( m_pDefaultLight );
		pNode->UpdateEffects();
		m_Eff1.SetNiObject( pNode, CGeometry::Normal );
	}

	{
		NiStream stream;
		stream.Load( "ToolData\\SequenceTool_Plan.nif" );
		NiNode* pNode = (NiNode*)stream.GetObjectAt( 0 );
		pNode->AttachEffect( m_pDefaultLight );
		pNode->UpdateEffects();
		m_Plan.SetNiObject( pNode, CGeometry::Normal );
		m_Plan.m_pNiNode->UpdateProperties();
		m_Plan.m_pNiNode->Update( 0.0f );
	}

	{
		NiStream stream;
		stream.Load( "ToolData\\OffsetMesh.nif" );
		NiNode* pNode = (NiNode*)stream.GetObjectAt( 0 );
		pNode->AttachEffect( m_pDefaultLight );
		pNode->UpdateEffects();
		m_OffsetMesh.SetNiObject( pNode, CGeometry::Normal );
		m_OffsetMesh.m_pNiNode->UpdateProperties();
		m_OffsetMesh.m_pNiNode->Update( 0.0f );
	}

	m_bRenderSpline = false;
}

void CViewModel::DeleteSrcModel()
{
	if( m_pModel != NULL )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = m_pModel;
		g_pThread->DeleteChar( pUnit );
		m_pModel = NULL;
	}
}

void CViewModel::DeleteDestModel()
{
	if( m_pDestModel != NULL )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = m_pDestModel;
		g_pThread->DeleteChar( pUnit );
		m_pDestModel = NULL;
	}
}

CsC_AvObject* CViewModel::SetSrcModel( DWORD dwModelID, bool bABVDraw )
{
	DeleteSrcModel();

	CsModelData::sINFO* pMD = g_pModelDataMng->GetData( dwModelID )->GetInfo();

	if( dwModelID / 10000 == 8 )
	{
		sCREATEINFO prop;
		prop.s_dwPropPlag =	PR_ATTACK_ENABLE | PR_ANI_ENABLE | PR_EFFECT_ENABLE;
		prop.s_dwOptionPlag = OP_LIGHT;
		if( bABVDraw == true )
			prop.s_dwOptionPlag |= OP_ABVWIRE;

		prop.s_eInsertGeomType = CGeometry::CharOutLine;

		m_pModel = CPartChar::NewInstance();

		CsC_PartObject::sCHANGE_PART_INFO pi[ nsPART::MAX_TOTAL_COUNT ];
		for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
			pi[ i ].s_nPartIndex = i;

		( (CsC_PartObject*)m_pModel )->Init( 0, dwModelID, NiPoint3::ZERO, -NI_HALF_PI, &prop );
		m_pModel->GetProp_Animation()->SetStopFrame( false );
		m_pModel->GetProp_Animation()->SetUseIdleAni( false );
		m_pModel->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 1.0f );

		( (CsC_PartObject*)m_pModel )->ThreadLoad_Part( pi, true );
	}
	else
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::KFM_File );
		pUnit->s_Prop.s_dwPropPlag = PR_ATTACK_ENABLE | PR_ANI_ENABLE | PR_EFFECT_ENABLE;
		pUnit->s_Prop.s_dwOptionPlag = OP_LIGHT;
		if( bABVDraw == true )
			pUnit->s_Prop.s_dwOptionPlag |= OP_ABVWIRE;

		pUnit->s_Prop.s_eInsertGeomType = CGeometry::CharOutLine;

		m_pModel = CNormalChar::NewInstance();
		m_pModel->Init( 0, dwModelID, NiPoint3::ZERO, -NI_HALF_PI, &pUnit->s_Prop );
		m_pModel->GetProp_Animation()->SetStopFrame( false );
		m_pModel->GetProp_Animation()->SetUseIdleAni( false );
		m_pModel->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 1.0f );

		strcpy_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, pMD->s_cKfmPath );
		pUnit->s_pLoadedObject = m_pModel;
		g_pThread->LoadChar( pUnit );
	}

	assert_cs( m_pModel != NULL );
	m_pModel->GetProp_Animation()->GetSequenceMng()->GetEventOption()->s_eHitType = (rand()%10 == 0 ) ? CsC_AttackProp::HT_Critical : CsC_AttackProp::HT_Normal;
	NiPoint3 vLTMinZ = NiPoint3( -pMD->s_fWidth*50.0f, pMD->s_fWidth*50.0f, 0 ) * pMD->s_fScale;
	NiPoint3 vRBMaxZ = NiPoint3( pMD->s_fWidth*50.0f, -pMD->s_fWidth*50.0f, pMD->s_fHeight*100.0f ) * pMD->s_fScale;
	CsGeomBox::ResetVertexPos( m_ClientBox.m_pNiGeom, vLTMinZ, vRBMaxZ );

	g_pMainWnd->_GetDlgEtc()->_CheckAction();

	return m_pModel;
}

CsC_AvObject* CViewModel::SetDestModel( DWORD dwModelID )
{
	DeleteDestModel();

	CsModelData::sINFO* pMD = g_pModelDataMng->GetData( dwModelID )->GetInfo();

	if( dwModelID / 10000 == 8 )
	{
		sCREATEINFO prop;
		prop.s_dwPropPlag =	PR_ATTACK_ENABLE | PR_ANI_ENABLE | PR_EFFECT_ENABLE;
		prop.s_dwOptionPlag = OP_LIGHT;
		prop.s_eInsertGeomType = CGeometry::CharOutLine;

		m_pDestModel = CPartChar::NewInstance();

		CsC_PartObject::sCHANGE_PART_INFO pi[ nsPART::MAX_TOTAL_COUNT ];
		for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
			pi[ i ].s_nPartIndex = i;

		( (CsC_PartObject*)m_pDestModel )->Init( 1, dwModelID, NiPoint3::ZERO, NI_HALF_PI, &prop );
		m_pDestModel->GetProp_Animation()->SetStopFrame( false );
		m_pDestModel->GetProp_Animation()->SetUseIdleAni( false );
		m_pDestModel->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 1.0f );

		( (CsC_PartObject*)m_pDestModel )->ThreadLoad_Part( pi, true );
	}
	else
	{
		m_pDestModel = CNormalChar::NewInstance();
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::KFM_File );
		pUnit->s_Prop.s_dwPropPlag = PR_ATTACK_ENABLE | PR_ANI_ENABLE | PR_EFFECT_ENABLE;
		pUnit->s_Prop.s_dwOptionPlag = OP_LIGHT;
		pUnit->s_Prop.s_eInsertGeomType = CGeometry::CharOutLine;

		m_pDestModel->Init( 1, dwModelID, NiPoint3( 0, 0, 0 ), NI_HALF_PI, &pUnit->s_Prop );
		m_pDestModel->GetProp_Animation()->SetStopFrame( false );
		m_pDestModel->GetProp_Animation()->SetUseIdleAni( false );
		m_pDestModel->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 1.0f );
		CsModelData::sINFO* pMD = g_pModelDataMng->GetData( dwModelID )->GetInfo();
		strcpy_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, pMD->s_cKfmPath );
		pUnit->s_pLoadedObject = m_pDestModel;
		g_pThread->LoadChar( pUnit );
	}

	assert_cs( m_pDestModel != NULL );
	m_pDestModel->GetProp_Animation()->GetSequenceMng()->GetEventOption()->s_eHitType = (rand()%10 == 0 ) ? CsC_AttackProp::HT_Critical : CsC_AttackProp::HT_Normal;

	return m_pDestModel;
}

void CViewModel::Update()
{
	float fDeltaTime = g_pMainWnd->_GetDlgAnimation()->_GetDeltaTime();
	CAMERA_ST.ApplyFrustumPlane( false );

	if( m_pModel == NULL )
		return;

	float fAniTime = g_pMainWnd->_GetDlgAnimation()->_GetAniTime();
	g_pSoundMgr->Update( fDeltaTime );

	m_pModel->Update( fAniTime );

	if( g_pMainWnd->_GetDlgController()->_IsDestView() == true )
	{
		/*if( m_pDestModel != NULL )
		{
			m_pDestModel->SetAnimation( g_pMainWnd->_GetDlgController()->_GetDestAni() );
		}*/
	}	

	if( m_pDestModel != NULL )
	{		
		m_pDestModel->SetPos( NiPoint3( g_pMainWnd->_GetDlgController()->_GetDestDist(), 0, 0 ) );
		m_pDestModel->Update( fAniTime );
	}	
}

void CViewModel::Render()
{
	g_pRenderer->BeginFrame();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
	g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	if( g_pMainWnd->_GetDlgAnimation()->_IsPlanView() == true )
		m_Plan.Render();

	if( m_pModel != NULL )
	{
		if( g_pMainWnd->_GetDlgAnimation()->_IsUseOffset() && g_pMainWnd->_GetDlgAnimation()->_IsViewOffset() )
		{
			NiMatrix3 mat;
			mat.FromEulerAnglesXYZ( 0, 0, m_pModel->GetCurRot() );
			NiPoint3 vOffset = mat*g_pMainWnd->_GetDlgAnimation()->_GetOffset();
			m_OffsetMesh.m_pNiNode->SetTranslate( vOffset );
			m_OffsetMesh.m_pNiNode->Update( 0.0f );
			m_OffsetMesh.Render();
		}

		m_pModel->Render();

		if( g_pMainWnd->_GetDlgController()->_IsDestView() == true )
		{
			if( m_pDestModel != NULL )
			{
				m_pDestModel->Render();
			}			
		}

		if( m_bRenderSpline == true )
		{
			NiPoint3 vResult = m_pModel->GetCsNode()->m_pNiNode->GetWorldTransform()*m_vEff0;
			m_Eff0.m_pNiNode->SetTranslate( vResult );
			m_Eff0.m_pNiNode->Update( 0.0f );
			vResult = m_pDestModel->GetCsNode()->m_pNiNode->GetWorldTransform()*m_vEff1;
			m_Eff1.m_pNiNode->SetTranslate( vResult );
			m_Eff1.m_pNiNode->Update( 0.0f );

			m_Eff0.Render();
			m_Eff1.Render();
		}

		g_Sorting.Render( CAMERA_ST.GetCameraObj() );

		if( ( m_pModel->IsLoad() == true )&&( m_bRenderClientBox == true ) )
		{
			m_ClientBox.Render();
		}
	}

	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->EndFrame();
	g_pRenderer->DisplayFrame();
}


void CViewModel::ApplyScale()
{
	assert_cs( m_pModel != NULL );

	CsModelData::sINFO* pMD = g_pModelDataMng->GetData( m_pModel->GetModelID() )->GetInfo();

	m_pModel->GetCsNode()->m_pNiNode->SetScale( pMD->s_fScale );	

	NiPoint3 vLTMinZ = NiPoint3( -pMD->s_fWidth*50.0f, pMD->s_fWidth*50.0f, 0 ) * pMD->s_fScale;
	NiPoint3 vRBMaxZ = NiPoint3( pMD->s_fWidth*50.0f, -pMD->s_fWidth*50.0f, pMD->s_fHeight*100.0f ) * pMD->s_fScale;
	CsGeomBox::ResetVertexPos( m_ClientBox.m_pNiGeom, vLTMinZ, vRBMaxZ );
}


void CViewModel::ChangePart( int nPartIndex, TCHAR* szPart )
{
	/*assert_cs( m_pModel->GetModelID() / 10000 == 8 );

	char cPart[ OBJECT_NAME_LEN ];
	W2M( cPart, szPart, OBJECT_NAME_LEN );

	( (CPartChar*)m_pModel )->ChangePart_ByTool( cPart, nPartIndex );*/
}

//=====================================================================================
//
//	플랜
//
//=====================================================================================
void CViewModel::SetPlan_TwoSide( bool bEnable )
{
	NiStencilProperty* pProp = (NiStencilProperty*)m_Plan.m_vpGeom[ 0 ]->m_pGeometry->GetProperty( NiStencilProperty::GetType() );
	if( pProp == NULL )
	{
		pProp = NiNew NiStencilProperty;
		m_Plan.m_vpGeom[ 0 ]->m_pGeometry->AttachProperty( pProp );
		m_Plan.m_vpGeom[ 0 ]->m_pGeometry->UpdateProperties();
	}
	if( bEnable == true )
		pProp->SetDrawMode( NiStencilProperty::DRAW_BOTH );
	else
		pProp->SetDrawMode( NiStencilProperty::DRAW_CCW );
}

void CViewModel::SetPlan_WireFrame( bool bEnable )
{
	NiWireframeProperty* pProp = (NiWireframeProperty*)m_Plan.m_vpGeom[ 0 ]->m_pGeometry->GetProperty( NiWireframeProperty::GetType() );
	if( pProp == NULL )
	{
		pProp = NiNew NiWireframeProperty;
		m_Plan.m_vpGeom[ 0 ]->m_pGeometry->AttachProperty( pProp );
		m_Plan.m_vpGeom[ 0 ]->m_pGeometry->UpdateProperties();
	}
	pProp->SetWireframe( bEnable );
}

//=====================================================================================
//
//	더미 오브젝트
//
//=====================================================================================

void CViewModel::SetSplinePos( NiPoint3 v1, NiPoint3 v2 )
{
	if( m_pModel == NULL )
	{
		ReleaseSpline();
		return;
	}
	if( m_pDestModel == NULL )
	{
		ReleaseSpline();
		return;
	}

	m_bRenderSpline = true;
	m_vEff0 = v1*0.1f;
	m_vEff1 = v2*0.1f;	
}

void CViewModel::ReleaseSpline()
{
	m_bRenderSpline = false;
}

//=====================================================================================
//
//	디폴트 라이트
//
//=====================================================================================
void CViewModel::_CreateDefaultLight()
{
	assert_cs( m_pDefaultLight == NULL );
	m_pDefaultLight = NiNew NiDirectionalLight;
	m_pDefaultLight->SetDiffuseColor( NiColor(0.3f, 0.3f, 0.3f) );
	m_pDefaultLight->SetAmbientColor( NiColor(0.9f, 0.9f, 0.9f) );	

	NiMatrix3 mat;
	mat.MakeYRotation( -NI_HALF_PI*0.5f );
	m_pDefaultLight->SetRotate( mat );
	m_pDefaultLight->Update( 0.0f );

	// 쓰레드 리소스 매니져에 연결
	g_pThread->GetResMng()->SetDefaultLight( m_pDefaultLight );
}

void CViewModel::_DeleteDefaultLight()
{
	m_pDefaultLight = 0;
}
