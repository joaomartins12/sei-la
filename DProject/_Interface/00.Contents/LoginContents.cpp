#include "StdAfx.h"
#include "LoginContents.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

int const LoginContents::IsContentsIdentity(void)
{
	return E_CT_LOGIN;
}

LoginContents::LoginContents(void) : m_bShowVideo(false)
{
#ifdef VERSION_USA
	m_LogInTimeSeq.SetDeltaTime( 1000 );
#else
	m_LogInTimeSeq.SetDeltaTime( 1000 );
#endif

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RESET_LOGIN_TIME_SEQ,	this,	&LoginContents::LogInTimeSeqReset );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_LOGIN_REQUEST,		this,	&LoginContents::RecvLogInRequest );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_LOGIN_WAIT_TIME,	this,	&LoginContents::RecvLoginWaitTime );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_USE_BARCODE_FALSE,	this,	&LoginContents::RecvUseBarcodeFalse );
}

LoginContents::~LoginContents(void)
{
	GAME_EVENT_ST.DeleteEventAll( this );
}

int const LoginContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool LoginContents::Initialize(void)
{

	return true;
}

void LoginContents::UnInitialize(void)
{
}

bool LoginContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void LoginContents::Update(float const& fElapsedTime)
{
// 	cData_Account* pAccount = g_pDataMng->GetAccount();
// 	SAFE_POINTER_RET(pAccount);
// 
// #ifdef SERVER_WAITING_LINE
// 	// 이쪽에 대기열 업데이트
// 	if(pAccount->m_WaitEventTime.IsEnableTime() == true )
// 	{
// 		int nWait = pAccount->m_WaitEventTime.UpdatWaitingLine();
// 		cPrintMsg::PrintMsg( 30537, &nWait );
// 	}
// #endif
// 	if( pAccount->m_WaitTime.IsEnableTime() == true )
// 	{		
// 		if( g_pSystemMsg->GetSysyemMessageSize() == 0 )
// 		{
// 			if( pAccount->m_WaitTime.IsEnable() == false )
// 			{
// 				int nRemain = static_cast<int>( (pAccount->m_WaitTime.RemainDeltaTime() / 1000) / 60 );
// 				if( nRemain > 0 )
// 				{
// 					//남은 시간 표시
// 					cPrintMsg::PrintMsg( 10005, &nRemain );
// 				}
// 				else
// 				{
// 					//남은 시간 표시
// 					nRemain++;
// 					cPrintMsg::PrintMsg( 10006, &nRemain );
// 				}
// 			}			
// 		}
// 	}
}

void LoginContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void LoginContents::MakeMainActorData(void)
{
}

void LoginContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

void LoginContents::GameExit(void) const
{
	::SendMessage(GAMEAPP_ST.GetHWnd(), WM_DESTROY, 0 ,0);
}

void LoginContents::RecvLoginWaitTime(void* pData)
{
// 	SAFE_POINTER_RET(pData);
// 	u4 waitTime = *static_cast<u4*>(pData);
// 	cPrintMsg::PrintMsg( 10005, &waitTime );


// 	LOG("nWaitTime : %u", nWaitTime);
// 	g_pDataMng->GetAccount()->m_WaitTime.OffReset();
// 	g_pDataMng->GetAccount()->m_WaitTime.SetDeltaTime( nWaitTime + 50000 );	
// 	g_pDataMng->GetAccount()->m_WaitTime.SetEnableTime( true );
}

void LoginContents::LogInTimeSeqReset(void* pData)
{ 
	cMessageBox::DelMsg( 10020, false ); 
}

void LoginContents::RecvLogInRequest(void* pData)
{
	cMessageBox::DelMsg( 10019, false );
#ifdef DEBUG_NETWORK
	DBG("LOGIN REQ CONTENTS ENTER\n");
#endif
	SAFE_POINTER_RET( pData );
	GS2C_RECV_LOGIN_REQUEST * pRecv = static_cast<GS2C_RECV_LOGIN_REQUEST*>(pData);
	if( 0 != pRecv->nResult )
	{
		cPrintMsg::PrintMsg( pRecv->nResult );
		return;
	}

#ifdef DEBUG_NETWORK
	DBG("LOGIN REQ CONTENTS SUBTYPE -> %d\n", pRecv->nSubType);
#endif

	switch( pRecv->nSubType )
	{
	case nsLogin::eLOGINSUBTYPE::LOGIN:
		{
			if( FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_SERVERSEL) )
				net::account->SendReqClusterList();
			else
				FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);	
		}break;// 로그인 성공
	case nsLogin::eLOGINSUBTYPE::CHECK2NDPASS:		// 2차 비밀 번호 확인
		{	
#ifdef USE_DMO_INI
			cProfile x("./dmo.ini");	
			std::string pw = x.GetStr("NETWORK", "2pw");
			if( !pw.empty() )
				GLOBALDATA_ST.Set2ndPassword( pw );
#endif
			std::string strPw = GLOBALDATA_ST.Get2ndPassword();
			if( strPw.empty() )
			{
				ContentsStream ktmp;
				ktmp << pRecv->nSubType;
				NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_SecondPassword_Type, ktmp);
				FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SECURITY);
			}
			else
				net::account->SendSecondPassCertified( strPw.c_str() );	
		}break;
	case nsLogin::eLOGINSUBTYPE::SET2NDPASS:		// 2차 비밀 번호 등록 
		{
			if( GLOBALDATA_ST.Is2ndPassSkiped() )// 2차 비밀 번호 입력을 스킵 할 수 있는지 검사
			{
				if(net::account)
					net::account->SendSkipSecondPass();
			}
			else
			{
				ContentsStream ktmp;
				ktmp << pRecv->nSubType;
				NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_SecondPassword_Type, ktmp);
				FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SECURITY);
			}
		}		
		break;
	default:
		return;
	}
}

void LoginContents::RecvUseBarcodeFalse(void* pData)
{
	ToggleUseBarcode(false);
	Notify(eRequest_Login);
}

void LoginContents::ShowIntro()
{
	SAFE_POINTER_RET(g_pRenderer);

	g_pRenderer->SetBackgroundColor( NiColorA( 16.0f/255.0f, 16.0f/255.0f, 16.0f/255.0f, 1 ) );
	g_pRenderer->BeginFrame();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->EndFrame();
	g_pRenderer->DisplayFrame();

	if( nsDIRECTSHOW::InitDShowWindow( _T( "Data\\NP\\Media\\Tutorial_End.wmv" ), CsPoint::ZERO, CsPoint::ZERO, true ) == true )
		nsDIRECTSHOW::WaitForKey_ForWindow_AniKey();
	g_pRenderer->SetBackgroundColor( NiColor::BLACK );
}

void LoginContents::ShowOpening()
{
	SAFE_POINTER_RET(g_pRenderer);

	g_pRenderer->SetBackgroundColor( NiColorA( 16.0f/255.0f, 16.0f/255.0f, 16.0f/255.0f, 1 ) );
	g_pRenderer->BeginFrame();
	g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
	g_pRenderer->EndUsingRenderTargetGroup();
	g_pRenderer->EndFrame();
	g_pRenderer->DisplayFrame();

	if( nsDIRECTSHOW::InitDShowWindow( _T( "Data\\NP\\Media\\Tutorial_Open.wmv" ), CsPoint::ZERO, CsPoint::ZERO, true ) == true )
		nsDIRECTSHOW::WaitForKey_ForWindow_AniKey();
	g_pRenderer->SetBackgroundColor( NiColor::BLACK );
}

bool LoginContents::StartSelectServer(wchar *id, wchar *pass, bool bSelectServer )
{
	SAFE_POINTER_RETVAL( id, false );
	SAFE_POINTER_RETVAL( pass, false );

	if( !GLOBALDATA_ST.IsLoginSkiped() )
	{
		std::wstring userID(id);
		if( Language::SUCCEEDED != Language::CheckID( userID ) )
		{
			cPrintMsg::PrintMsg( 10002 );
			 
			return false;
		}

		std::wstring userPw(pass);
		if( Language::SUCCEEDED != Language::CheckPw( userPw ) )
		{
			cPrintMsg::PrintMsg( 10002 );
		 
			return false;
		}
	}

	GLOBALDATA_ST.LoadInfo_AccountIP_Port();

#ifdef DEBUG_NETWORK
	DBG("Port -> %d\n", GLOBALDATA_ST.GetAccountPort());
#endif

	if (net::start(net::type::account, GLOBALDATA_ST.GetAccountIP().c_str(), GLOBALDATA_ST.GetAccountPort()) == false) {
#ifdef DEBUG_NETWORK
		DBG(" Sockstart fail");
#endif 
	// start account client
		return false;
	}

	// 아이디 저장 한다면 아이디 저장한다
	std::string cID;
	std::string cPW;
	DmCS::StringFn::ToMB( id, cID );
	DmCS::StringFn::ToMB( pass, cPW );
#ifdef DEBUG_NETWORK
	//DBG("Upw after DmCS -> %s\n", cPW);
#endif 
	GLOBALDATA_ST.SetAccountID( cID );
	GLOBALDATA_ST.SetAccountPW(cPW  );

#ifdef USE_ID_SAVE
	if( g_pResist->m_Global.s_bSaveID == true )
		_tcscpy_s( g_pResist->m_Global.s_szLastConnectID, MAX_FILENAME, id );
#endif	

	return true;
}

void LoginContents::TryToLogin( const TCHAR* strID, const TCHAR* strPass )
{
	// 로그인 체크
#ifdef USE_BARCODE_REDER
	if(!CheckBarcodeReader())
		return;
#endif

	if( IsEnableLoginTime() )
		StartSelectServer( const_cast<TCHAR*>(strID), const_cast<TCHAR*>(strPass), false );
}

#ifdef USE_BARCODE_REDER
bool LoginContents::CheckBarcodeReader()
{
	// 로그인 체크
	if( g_pResist->m_Global.s_bUseBarcode )
	{
		// 바코드 프로그램 인스톨 되었는지 체크
		switch( nsBARCODE::LoadLibrary() )
		{
		case nsBARCODE::LL_NOT_INSTALL:
			cPrintMsg::PrintMsg( 20038 );
			return false;
		case nsBARCODE::LL_LOAD_FAIL_DLL_FILE:
			nsBARCODE::FreeLibrary();
			CsMessageBox(MB_OK, _T( "Barcode LoadError : Dll File" ) );
			return false;
		case nsBARCODE::LL_LOAD_FAIL_FUNC_ADDRESS:
			nsBARCODE::FreeLibrary();
			CsMessageBox( MB_OK, _T( "Barcode LoadError : Func Address" ) );
			return false;
		}
	}
	return true;
}
#endif

bool LoginContents::IsEnableLoginTime()
{
	return m_LogInTimeSeq.IsEnable();
}

void LoginContents::ResetLoginTime()
{
	m_LogInTimeSeq.Reset();
}

bool LoginContents::IsShowVideo() const
{
	return m_bShowVideo;
}

void LoginContents::SetShowVideo(bool bShow)
{
	m_bShowVideo = bShow;
}

void LoginContents::ToggleIDSaveCheckBox(bool bCheck)
{
	g_pResist->m_Global.s_bSaveID = bCheck;
}

void LoginContents::ToggleUseBarcode(bool bCheck)
{
	g_pResist->m_Global.s_bUseBarcode = bCheck;
}


//////////////////////////////////////////////////////////////////////////
// 3D Object 맵 로드
//////////////////////////////////////////////////////////////////////////
bool LoginContents::Load3DEffect(std::string const& loadFileName)
{
	NiStream kStream;
	if( !kStream.Load( loadFileName.c_str() ) )
		return false;

	NiNodePtr pNode = (NiNode*)kStream.GetObjectAt(0);
	nsCSGBFUNC::InitAnimation( pNode, NiTimeController::APP_INIT, NiTimeController::LOOP );
	m_Effect.SetNiObject( pNode, CGeometry::Normal );	

	NiTimeController::StartAnimations( pNode, 0.0f );
	pNode->UpdateEffects();
	pNode->Update(0.0f);
	return true;
}

void LoginContents::RenderBackgroundMap()
{
	m_Effect.Render();
}

void LoginContents::UpdateBackgroundMap(float const& fAccumTime)
{
	if( m_Effect.m_pNiNode )
	{
		m_Effect.m_pNiNode->Update(fAccumTime);
		CsGBVisible::OnVisible( &m_Effect, m_Effect.m_pNiNode, CsGBVisible::VP_BILLBOARD, fAccumTime );
	}
}

void LoginContents::Load3DBackgroundData()
{
	sCAMERAINFO ci;
	ci.s_fDist = 2800.0f;
	ci.s_fFarPlane = 100000.0f;
	ci.s_fInitPitch = 0.0f;
	CAMERA_ST.Reset( &ci );
	CAMERA_ST.ReleaseDistRange();
	CAMERA_ST.ReleaseRotationLimit();
	CAMERA_ST.SetDeltaHeight( 0.0f );
	CAMERA_ST._UpdateCamera();

	Load3DEffect("Data\\Interface\\Lobby\\BackgroundEff.nif");
}


void LoginContents::Remove3DBackgroundData()
{
	m_Effect.Delete();
}