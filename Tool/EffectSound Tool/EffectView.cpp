#include "StdAfx.h"
#include "EffectView.h"

CEffectView	g_ViewEffect;
CEffectView::CEffectView(void)
{
	m_pEffect = NULL;
	m_pCsSequenceMng = NULL;
}

CEffectView::~CEffectView(void)
{	
}

bool CEffectView::CALLBACK_ThreadDelete( sTCUnit* pUnit )
{
	CsC_Object* pObject = pUnit->s_pLoadedObject;

	UINT nFTID = pObject->GetFTID();
	pObject->Delete();

	return false;
}

void CEffectView::Delete()
{
	//nsCsFileTable::g_FileTableMng.Delete();	

	if( g_EffectSoundMng != NULL )
	{
		g_EffectSoundMng->Delete();
		SAFE_NIDELETE( g_EffectSoundMng );
	}
	
	ClearOption();
	DeleteOption();
	DeleteEffect();
	DeleteDestModel();	

	CAMERA_ST.Delete();
	DESTROY_SINGLETON( CAMERA_STPTR );

	CsC_Thread::GlobalShotDown();	

	_DeleteDefaultLight();
	CsGBTerrainMng::DeleteMempool();
}

void CEffectView::Init()
{
	CsC_Thread::GlobalInit();
	g_pThread->UseThread( false );
	g_pThread->ResistCallBack_DeleteUnit( CALLBACK_ThreadDelete );


//	nsCsMapTable::g_bUseMapStart = false;	
//	nsCsMapTable::g_bUseMapResurrection = false;
//
//	nsCsFileTable::g_bAddExp = false;
//	nsCsFileTable::g_bUseEvent = false;
//	nsCsFileTable::g_bBuffMng = false;
//	nsCsFileTable::g_bSceneDataMng = false;
//	nsCsFileTable::g_bUseMoveObject = false;
//	nsCsFileTable::g_bUseHelp = false;
//	nsCsFileTable::g_bUseCashShop = false;
//	nsCsFileTable::g_bUseQuest = false;
//	nsCsFileTable::g_bUseAchieve = false;
//	nsCsFileTable::g_bReadInactiveQuest = false;
//	nsCsFileTable::g_bUseMap = false;
//
//	nsCsFileTable::eLANGUAGE lan;
//
//#ifdef VERSION_USA	//GSP 버전은 고정으로 영문버전
//	lan = nsCsFileTable::ENGLISH;
//#elif VERSION_HK	// 홍콩 버전 _ 컨텐츠가 다르므로 별도 관리
//	lan = nsCsFileTable::HONGKONG;
//#elif VERSION_TW	// 대만 버전 _ 컨텐츠가 다르므로 별도 관리
//	lan = nsCsFileTable::TAIWAN;
//#elif VERSION_TH
//	lan = nsCsFileTable::THAILAND;
//#else
//	lan = nsCsFileTable::KOREA_TRANSLATION;
//#endif
//
//#ifdef _DEBUG
//	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_BIN, lan );
//#else
//	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, lan );	
//#endif

	// 콜백 등록
	_CreateDefaultLight();

	char cShader[ MAX_PATH ] = ".\\Data\\Shaders";
	char* apcShaderDirectories[1];
	apcShaderDirectories[0] = cShader;
	m_ShaderHelper.SetupShaderSystem( NULL, 0, apcShaderDirectories, 1 );

	CREATE_SINGLETON( CsGBCamera );
	sCAMERAINFO info;
	info.s_fFarPlane = 10000.0f;
	info.s_fInitPitch = -NI_HALF_PI / 4;
	info.s_fInitRoll = 0.0f;	
	info.s_fDist = 1000.0f;
	CAMERA_ST.Create( g_pRenderer, &info );
	CAMERA_ST.SetDistRange( 100.0f, 5000.0f );
	CAMERA_ST.SetDistAccel( 0.7f );
	CAMERA_ST.SetTranslate( NiPoint3( 0, 0, 200 ) );
	CAMERA_ST.SetTranslateAccel( 0.27f );
	CAMERA_ST.ChangeStandardFov( 600, 450 );		

	m_fAniDeltaTime = m_fEffectTime = 0.0f;		
	
	g_EffectSoundMng = NiNew CsEffectSoundMng;
	g_EffectSoundMng->Init();	
	
	m_pOption = NiNew CsEffectSound;
}

void CEffectView::Update()
{
	static float fOldTimeInSec = NiGetCurrentTimeInSec();
	float fCurTimeInSec = NiGetCurrentTimeInSec();	

	g_fAccumTime += fCurTimeInSec - fOldTimeInSec;		
	g_pMainWnd->GetDlgEtc()->SetControltoData();

	if( g_pMainWnd->GetDlgEtc()->IsPlay() )
	{
		m_fDeltaTime = fCurTimeInSec - fOldTimeInSec;
		fOldTimeInSec = fCurTimeInSec;	

		if( m_pEffect != NULL )
		{				
			if( m_fEffectTime < m_fEndAniTime )
			{
				m_fEffectTime += m_fDeltaTime;
				m_pEffect->Update( m_fDeltaTime );					
			}	
			else
			{
				SetEffect( m_strEffect );
				if( m_bLoop )
				{
					m_fEffectTime = 0.0f;					
					g_pMainWnd->GetDlgEtc()->SetSldRange( m_fEndAniTime );	
				}
				else
				{
					m_fEffectTime = 0.0f;	
					g_pMainWnd->GetDlgEtc()->SetPlay( false );
				}									
			}
		}		

		if( m_pOption != NULL )
		{
			EffectSoundPlay( m_fDeltaTime );
		}

		if( g_pMainWnd->GetDlgChar()->IsDestView() == false )
		{
			if( m_pDestModel != NULL )
			{			
				m_pDestModel->Update( m_fDeltaTime );
			}
		}
	}
	else
	{
		fOldTimeInSec = fCurTimeInSec;
		m_fAniDeltaTime = g_pMainWnd->GetDlgEtc()->GetCurTime() - m_fEffectTime; 
		m_fEffectTime = g_pMainWnd->GetDlgEtc()->GetCurTime();		

		if( m_pEffect != NULL )
		{		
			m_pEffect->Update( m_fAniDeltaTime );			
		}

		if( g_pMainWnd->GetDlgChar()->IsDestView() == false )
		{
			if( m_pDestModel != NULL )
			{			
				m_pDestModel->Update( 0.0f );
			}
		}
	}	

	g_pMainWnd->GetDlgEtc()->Update( m_fEffectTime );	

	CAMERA_ST.ApplyFrustumPlane( false );	
	g_pSoundMgr->Update( g_fAccumTime );	
}


void CEffectView::Render()
{	
	g_pRenderer->BeginFrame();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
	g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );

	if( g_pMainWnd->GetDlgChar()->IsDestView() == false )
	{
		if( m_pDestModel != NULL )
		{		
			m_pDestModel->Render();			
		}			
	}	
	
	if( m_pEffect != NULL )
	{			
		m_pEffect->Render();		

		TCHAR sz[ 128 ];		
		_stprintf_s( sz, 128, L"%.3f / %.3f", m_fEffectTime,  m_fEndAniTime );
		g_pMainWnd->GetDlgEtc()->SetTime( sz );
	}

	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->EndFrame();
	g_pRenderer->DisplayFrame();	
}

void CEffectView::EffectSoundPlay( float fDeltaTime )
{
	std::list< CsEffectSound::sSound_Info* >::iterator it = m_pOption->m_listSound.begin();
	for( ; it != m_pOption->m_listSound.end() ; it++ )
	{
		float tt = (*it)->fSec;
		if( (*it)->fSec >= (m_fEffectTime - fDeltaTime) && (*it)->fSec < m_fEffectTime )
		{
			g_pSoundMgr->PlaySound( (*it)->szFileName );
		}
	}
	
}

void CEffectView::DeleteDestModel()
{
	if( m_pDestModel != NULL )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = m_pDestModel;
		g_pThread->DeleteChar( pUnit );
		m_pDestModel = NULL;		
	}
}


CsC_AvObject* CEffectView::SetDestModel( DWORD dwModelID )
{
	DeleteDestModel();

	CsModelData::sINFO* pMD = g_pModelDataMng->GetData( dwModelID )->GetInfo();

	if( dwModelID / 10000 == 8 )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::KFM_File );

		// 초기화 - Prop 초기화 포함
		pUnit->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE | PR_ATTACK_ENABLE;
		pUnit->s_Prop.s_dwOptionPlag = OP_STAT | OP_LIGHT;
		pUnit->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();
		strcpy_s( pUnit->s_cFilePath, pMD->s_cKfmPath );

		m_pDestModel = CsC_PartObject::NewInstance();			
		CsC_PartObject::sCHANGE_PART_INFO pi[ nsPART::MAX_TOTAL_COUNT ];
		for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
			pi[ i ].s_nPartIndex = i;

		( (CsC_PartObject*)m_pDestModel )->Init( 1, dwModelID, NiPoint3::ZERO, 0.0f, &pUnit->s_Prop );
		m_pDestModel->GetProp_Animation()->SetStopFrame( false );
		m_pDestModel->GetProp_Animation()->SetUseIdleAni( false );
		m_pDestModel->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 1.0f );

		( (CsC_PartObject*)m_pDestModel )->ThreadLoad_Part( pi, true );		
	}
	else
	{
		m_pDestModel = CsC_AvObject::NewInstance();

		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::KFM_File );

		// 초기화 - Prop 초기화 포함
		pUnit->s_Prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE | PR_ATTACK_ENABLE;
		pUnit->s_Prop.s_dwOptionPlag = OP_STAT | OP_LIGHT;
		pUnit->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();	
		
		strcpy_s( pUnit->s_cFilePath, pMD->s_cKfmPath );

		m_pDestModel->Init( 1, dwModelID, NiPoint3( 0, 0, 0 ), 0.0f, &pUnit->s_Prop );
		m_pDestModel->GetProp_Animation()->SetStopFrame( false );
		m_pDestModel->GetProp_Animation()->SetUseIdleAni( false );
		m_pDestModel->GetProp_Alpha()->SetCurAlphaValue( CsC_AlphaProp::AT_KEEP, 1.0f );
		
		pUnit->s_pLoadedObject = m_pDestModel;
		g_pThread->LoadChar( pUnit );
	}

	assert_cs( m_pDestModel != NULL );
	m_pDestModel->GetProp_Animation()->GetSequenceMng()->GetEventOption()->s_eHitType
		= (rand()%10 == 0 ) ? CsC_AttackProp::HT_Critical : CsC_AttackProp::HT_Normal;	

	return m_pDestModel;
}

void CEffectView::DeleteEffect()
{
	if( m_pEffect != NULL )
	{
		sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::DeleteFile );
		pUnit->s_pLoadedObject = m_pEffect;
		g_pThread->DeleteChar( pUnit );
		m_pEffect = NULL;

		m_fEffectTime = 0.0f;
	}
}

void CEffectView::SetEffect( CString strFile )
{
	if( m_pDestModel == NULL )
	{
		CsMessageBox( MB_OK, L"캐릭터를 먼저 로드해주세요" );		
		return;
	}	

	DeleteEffect();	

	if( m_strEffect != strFile )
	{				
		m_strEffect = strFile;	
		g_pMainWnd->GetDlgEtc()->SetCurEffect( m_strEffect );
	}

	m_pEffect = CsC_EffectObject::NewInstance();
	sTCUnit* pUnit = sTCUnit::NewInstance( sTCUnit::NIF_File );

	Sleep( 3 );

	// 초기화 - Prop 초기화 포함
	pUnit->s_Prop.s_dwPropPlag = PR_ANI_ENABLE;
	pUnit->s_Prop.s_dwOptionPlag = OP_LIGHT;
	pUnit->s_Prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();	
	pUnit->s_pLoadedObject = m_pEffect;	

	WCHAR *strWC = m_strEffect.GetBuffer(0);
	CHAR strMB[OBJECT_PATH_LEN];
	WideCharToMultiByte( CP_ACP, 0, strWC, -1, strMB, OBJECT_PATH_LEN, NULL, NULL );
	m_strEffect.ReleaseBuffer();

	sprintf_s( pUnit->s_cFilePath, OBJECT_PATH_LEN, "%s", strMB );	

	m_pEffect->Init( m_pDestModel, NiPoint3::ZERO, &pUnit->s_Prop );	
	g_pThread->LoadChar( pUnit );	

	nsCSGBFUNC::EndAnimationTime( m_fEndAniTime, m_pEffect->GetCsNode()->m_pNiNode );
	m_pEffect->GetProp_Animation()->ResetAnimation();	
	g_pMainWnd->GetDlgEtc()->SetSldRange( m_fEndAniTime );	

}

void CEffectView::SetEffectOption()
{
	assert_cs( m_pEffect != NULL );

	m_pEffect->SetEffectPlag( m_pOption->m_sEffectOptin.dwEffectFlag );

	if( ( m_pEffect->GetEffectPlag() & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
	{
		m_pEffect->SetOffset( m_pOption->m_sEffectOptin.vOffset );
	}

	if( m_pOption->m_sEffectOptin.bChrScale == true )
	{
		m_pEffect->SetScale( m_pDestModel->GetToolWidth() * 0.027f * m_pOption->m_sEffectOptin.fScale );				
	}
	else
	{
		m_pEffect->SetScale( m_pOption->m_sEffectOptin.fScale );				
	}
}

bool CEffectView::SetOption( DWORD dwFlag, NiPoint3 vOffset, bool bScale, float fScale )
{
	if( m_pEffect == NULL )
	{		
		return false;
	}	

	assert_cs( m_pEffect != NULL );
	m_pOption->m_sEffectOptin.dwEffectFlag = dwFlag;
	m_pOption->m_sEffectOptin.vOffset = vOffset;	

	int nStrLen = m_strEffect.GetLength();
	int nPathPos = m_strEffect.Find( _T("Effect") );
	CString strPathname = m_strEffect.Mid( nPathPos + 7, nStrLen ); 

	WCHAR *strWC = strPathname.GetBuffer(0);
	CHAR strMB[OBJECT_PATH_LEN];
	WideCharToMultiByte( CP_ACP, 0, strWC, -1, strMB, OBJECT_PATH_LEN, NULL, NULL );
	strPathname.ReleaseBuffer();

	sprintf_s( m_pOption->m_sESInfo.szFilePath, OBJECT_PATH_LEN, "%s", strMB );	

	size_t CodeKey = CsFPS::GetHashCode( m_pOption->m_sESInfo.szFilePath );
	m_pOption->m_sESInfo.HashCode = CodeKey;

	m_pOption->m_sEffectOptin.bChrScale = bScale;
	m_pOption->m_sEffectOptin.fScale = fScale;	

	m_pOption->DeleteSoundList();
	for( int i = 0; i < g_pMainWnd->GetDlgEtc()->GetListCount(); i++ )
	{
		CsEffectSound::sSound_Info* pInfo = NiNew CsEffectSound::sSound_Info;
		g_pMainWnd->GetDlgEtc()->GetSoundListData( pInfo, i );
		m_pOption->m_listSound.push_back( pInfo );
	}	
	m_pOption->m_sESInfo.nSoundNum = (int)m_pOption->m_listSound.size();

	SetEffectOption();	

	return true;
}

void CEffectView::SetSoundList( CString strSound, float fTime )
{
	if( m_pEffect == NULL)
	{
		CsMessageBox( MB_OK, L"이펙트를 먼저 선택해주세요." );
		return;
	}	

	if( fTime == -1.0f )
	{
		g_pMainWnd->GetDlgEtc()->SetSoundList( strSound, m_fEffectTime );		
	}
	else
	{
		g_pMainWnd->GetDlgEtc()->SetSoundList( strSound, fTime );		
	}	

}

void CEffectView::LoadEffect( size_t HashCode )
{	
	g_pMainWnd->GetDlgEtc()->SoundListReset();

	if( g_EffectSoundMng->IsData( HashCode ) )
	{	
		CsEffectSound* pData = g_EffectSoundMng->GetData( HashCode );

		CString str;
		str += L"Data\\Effect\\";
		str += pData->m_sESInfo.szFilePath;
		SetEffect( str );
		if( SetOption( pData->m_sEffectOptin.dwEffectFlag, pData->m_sEffectOptin.vOffset,
			 pData->m_sEffectOptin.bChrScale, pData->m_sEffectOptin.fScale ) == false )
		{
			DeleteEffect();
		}
		else
		{
			g_pMainWnd->GetDlgEtc()->SetOption( m_pOption );
			std::list< CsEffectSound::sSound_Info* >::iterator it = pData->m_listSound.begin();
			std::list< CsEffectSound::sSound_Info* >::iterator itEnd = pData->m_listSound.end();
			for( ; it != itEnd; it++ )
			{
				CString str;				
				str += (*it)->szFileName;
				SetSoundList( str, (*it)->fSec );
			}											

			CsMessageBox( MB_OK, L"로드완료." );
		}		
	}
	else
	{
		assert_cs( false );			
	}
}

void CEffectView::ClearOption()	
{ 	
	g_pMainWnd->GetDlgEtc()->Reset();
}


//=====================================================================================
//	디폴트 라이트
//=====================================================================================
void CEffectView::_CreateDefaultLight()
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

void CEffectView::_DeleteDefaultLight()
{
	m_pDefaultLight = 0;
}

