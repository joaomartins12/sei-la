#include "StdAfx.h"
#include "SecondPasswordContents.h"
#include "common_vs2019/pPass2.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

// 하루 동안 2차 비밀번호 설정 페이지가 안보이게
//#define SKIP_TIME_CHECK_DAY		(24 * 60 * 60)

int const SecondPasswordContents::IsContentsIdentity(void)
{
	return E_CT_SECOND_PASSWORD;
}

SecondPasswordContents::SecondPasswordContents(void) : m_ePassWindowType(eEnd)
{
	m_strCurrPW.clear();
	m_strPrePW.clear();
	m_strConfirmPW.clear();

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SECONDPASS_CHECK,		this,	&SecondPasswordContents::Recv2ndPasswordCheck );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SECONDPASS_CHANGE,		this,	&SecondPasswordContents::Recv2ndPasswordChange );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SECONDPASS_REGISTER,	this,	&SecondPasswordContents::Recv2ndPasswordRegister );
	
	EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_SecondPassword_Type, this );
}

SecondPasswordContents::~SecondPasswordContents(void)
{
	GAME_EVENT_ST.DeleteEventAll( this );
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
}

int const SecondPasswordContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool SecondPasswordContents::Initialize(void)
{
	m_strCurrPW.clear();
	m_strPrePW.clear();
	m_strConfirmPW.clear();
	return true;
}

void SecondPasswordContents::UnInitialize(void)
{
}

bool SecondPasswordContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void SecondPasswordContents::Update(float const& fElapsedTime)
{
}

void SecondPasswordContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void SecondPasswordContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_SecondPassword_Type:
		{
			u1 nType;
			kStream >> nType;
			switch( nType )
			{
			case nsLogin::eLOGINSUBTYPE::CHECK2NDPASS:		// 2차 비밀 번호 확인
				m_ePassWindowType = eWindowType_AskPW;
				break;
			case nsLogin::eLOGINSUBTYPE::SET2NDPASS:		// 2차 비밀 번호 등록 
				m_ePassWindowType = eWindowType_Regist;
				break;
			}
		}
		break;
	}
}

void SecondPasswordContents::MakeWorldData(void)
{
}

void SecondPasswordContents::ClearWorldData(void)
{
}

void SecondPasswordContents::MakeMainActorData(void)
{
}

void SecondPasswordContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

bool SecondPasswordContents::AddPasswordData( TCHAR const& pNumber )
{
#ifndef SDM_SECONDPASSWORD_REINFORCE_20180330
	if(pNumber < _T('0') || pNumber > _T('9'))
		return false;
#endif
	if( m_strCurrPW.length() >= 8 )
		return false;

	m_strCurrPW += pNumber;
	Notify( eSet_ConversionStr );
	return true;
}

std::wstring SecondPasswordContents::GetCurrPassword() const
{
	return m_strCurrPW;
}

void SecondPasswordContents::ClearPasswordData()
{
	m_strCurrPW.clear();
}

// 비밀 번호가 유효한 값인지 검사
bool SecondPasswordContents::_CheckPasswordUnabled( std::wstring const& pw, size_t nMinSize, size_t nMaxSize )
{
	if( pw.empty() )
	{
		cPrintMsg::PrintMsg( 20054 );	
		return false;
	}

	std::wstring checkString = pw;
	size_t nLen = checkString.length();		

	bool bCheck = true;
	if( !(nLen>=nMinSize && nLen<=nMaxSize) )
		bCheck = false;

	size_t i = 0;
	for( i = 1; i < nLen; i++ )
	{
		if( checkString[ 0 ] != checkString[ i ] ) 
			break;
		else if( i == nLen - 1 )
				bCheck = false;
	}		

	if( !bCheck )	
		cPrintMsg::PrintMsg( 20054 );	

	return bCheck;
}

//////////////////////////////////////////////////////////////////////////
// 2차 비밀 번호 확인 요청
bool SecondPasswordContents::SendCurrent2ndPassword()
{
	if( !net::account )
		return false;

	bool bResult = _CheckPasswordUnabled( m_strCurrPW, 4, 8 );
	if( bResult )
	{
		std::string pw;
		DmCS::StringFn::ToMB( m_strCurrPW, pw );
		GLOBALDATA_ST.Set2ndPassword( pw );
		GLOBALDATA_ST.Set2ndPassType( GData::e2ndNumberPass );
		net::account->SendSecondPassCertified( pw.c_str() );	
	}

	ClearInputPassword();
	return bResult;
}

// 2차 비밀 번호로 등록 비밀 번호 저장
void SecondPasswordContents::SaveRegistPassword()
{
	std::wstring checkPw = m_strCurrPW;

	ClearInputPassword();

	if( !_CheckPasswordUnabled( checkPw, 6, 8 ) )
		return;

	m_strPrePW = checkPw;
	Notify( eCreate_RegistConfirmWindow );
}

// 2차 비밀 번호 등록 첫번째 창 확인.
void SecondPasswordContents::CheckRegistPasswordAndSend()
{
	SAFE_POINTER_RET( net::account );
	std::wstring checkPw = m_strCurrPW;

	ClearInputPassword();

	if( !_CheckPasswordUnabled( checkPw, 6, 8 ) )
		return;

	if( 0 != checkPw.compare( m_strPrePW ) )
	{
		cPrintMsg::PrintMsg( cPrintMsg::SECOND_PASS_DISACCORD );// 비밀번호 일치 하지 않음..
		return;
	}

	std::string pw;
	DmCS::StringFn::ToMB( checkPw, pw );
	net::account->SendRegister2ndPass( const_cast<char*>(pw.c_str()) );

	g_pResist->m_Global.SetSkip2ndPassword( FALSE );
	GLOBALDATA_ST.Set2ndPassType( GData::e2ndNumberPass );

	ClearPasswordData();
	Notify( eCreate_AskPasswordWindow );
}

void SecondPasswordContents::OpendChangeWindow()
{
	ClearInputPassword();
	Notify( eCreate_CurrentPasswordWindow );
}

// 비밀 번호 변경 사용
// 현재 사용 하고 있는 비밀 번호 저장
void SecondPasswordContents::SaveCurrentPassword()
{
	std::wstring checkPw = m_strCurrPW;

	ClearInputPassword();

	if( !_CheckPasswordUnabled( checkPw, 4, 8 ) )
		return;

	m_strPrePW = checkPw;
	Notify( eCreate_PwChangeWindow );
}

void SecondPasswordContents::SaveChangePassword()
{
	std::wstring checkPw = m_strCurrPW;
	ClearInputPassword();

	if( !_CheckPasswordUnabled( checkPw, 6, 8 ) )
		return;

	if( 0 == m_strPrePW.compare( checkPw ) )// 
	{
		return;
	}

	m_strConfirmPW = checkPw;
	Notify( eCreate_PwChangeConfirmWindow );
}

void SecondPasswordContents::CheckChangePasswordAndSend()
{
	std::wstring checkPw = m_strCurrPW;

	ClearInputPassword();

	if( !_CheckPasswordUnabled( checkPw, 6, 8 ) )
		return;

	// 현재 비밀 번호와 변경할 비밀 번호가 같은 경우 에러
	if( 0 != checkPw.compare( m_strConfirmPW ) )
	{
		cPrintMsg::PrintMsg( cPrintMsg::SECOND_PASS_DISACCORD );// 비밀번호 일치 하지 않음..
		return;
	}

	std::string PrevPass;
	DmCS::StringFn::ToMB( m_strPrePW, PrevPass );

	std::string ChangePass;
	DmCS::StringFn::ToMB( checkPw, ChangePass );

	net::account->SendChange2ndPass( const_cast<char*>(PrevPass.c_str()), const_cast<char*>(ChangePass.c_str()) );

	ClearPasswordData();
	//Notify( eCreate_AskPasswordWindow );
}

void SecondPasswordContents::ClearInputPassword()
{
	ClearPasswordData();
	Notify( eSet_ConversionStr );
}


void SecondPasswordContents::DeleteOnePassNumber()
{
	if( m_strCurrPW.empty() )
		return;

	std::wstring::size_type Length = m_strCurrPW.length();
	m_strCurrPW.erase( Length - 1 );
	Notify( eSet_ConversionStr );
}

const int SecondPasswordContents::GetPassWindowType() const
{
	return m_ePassWindowType;
}

void SecondPasswordContents::SkipInputSecondPassword(bool bHideOneDay)
{
	if( g_pResist )
	{
		g_pResist->m_Global.SetSkip2ndPassword( bHideOneDay );
		GLOBALDATA_ST.SetSkip2ndPass();
	}

	// 2차비밀번호 생성을 하지 않을 경우 임시 패스워드 셋팅.. 서버에서는 확인 안한다.
	if(net::account)
		net::account->SendSkipSecondPass();
}

// 2차 비밀 번호 입력 창에서 취소시 처리
void SecondPasswordContents::GotoBackFromAskWindow()
{
	if( GLOBALDATA_ST.IsLoginSkiped() )
		PostQuitMessage(0);
	else
	{
		GLOBALDATA_ST.ResetAccountInfo();
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_LOGIN);
	}
}

// 2차 비밀 번호 변경 창에서 취소시 처리
void SecondPasswordContents::Cancel2ndPassChange()
{
	FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);// 로그인 성공
}

// 2차 비밀 번호 변경 창에서 취소시 처리
void SecondPasswordContents::Cancel2ndPassChangingWindow()
{
	std::string str2Pss = GLOBALDATA_ST.Get2ndPassword();
	if( str2Pss.empty() )
	{
		ClearPasswordData();
		Notify( eCreate_AskPasswordWindow );
	}
	else
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);// 로그인 성공
}

void SecondPasswordContents::Cancel2ndPassRegistWindow()
{
	if( GLOBALDATA_ST.IsLoginSkiped() )
		PostQuitMessage(0);
	else
	{
		GLOBALDATA_ST.ResetAccountInfo();
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_LOGIN);
	}
}
//////////////////////////////////////////////////////////////////////////
// 3D Object 맵 로드
//////////////////////////////////////////////////////////////////////////
bool SecondPasswordContents::Load3DEffect(std::string const& loadFileName)
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

void SecondPasswordContents::RenderBackgroundMap()
{
	m_Effect.Render();
}

void SecondPasswordContents::UpdateBackgroundMap(float const& fAccumTime)
{
	if( m_Effect.m_pNiNode )
	{
		m_Effect.m_pNiNode->Update(fAccumTime);
		CsGBVisible::OnVisible( &m_Effect, m_Effect.m_pNiNode, CsGBVisible::VP_BILLBOARD, fAccumTime );
	}
}

void SecondPasswordContents::Load3DBackgroundData()
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


void SecondPasswordContents::Remove3DBackgroundData()
{
	m_Effect.Delete();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 2차 비밀 번호에 요청에 대한 결과값
void SecondPasswordContents::Recv2ndPasswordCheck( void* pData )
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_2ndPass_Certify* result = static_cast< GS2C_RECV_2ndPass_Certify* >(pData);

	cMessageBox::DelMsg( 10019, false );

	if( 0 != result->nResult )
	{// 2차 비밀번호 오류
		cPrintMsg::PrintMsg( result->nResult );
		GLOBALDATA_ST.Clear2ndPass();
		return;
	}

	if( FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_SERVERSEL) )
		net::account->SendReqClusterList();
	else if( FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_LOGIN) )
		FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);// 로그인 성공
	else if( FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_SECURITY) )
	{
		cPrintMsg::PrintMsg( cPrintMsg::SECOND_PASS_CONFIRM );
		std::string strPw = GLOBALDATA_ST.Get2ndPassword();
		if( !strPw.empty() && strPw.size() < 6 ) // 기존 2차 비밀 번호의 길이가 6보다 작으면 2차 비밀 번호 변경 요청 창의 띄운다.
			Notify( eCreate_AskPwChangeWindow );
		else
			FLOWMGR_ST.ChangeFlow(Flow::CFlow::FLW_SERVERSEL);// 로그인 성공
	}
}

void SecondPasswordContents::Recv2ndPasswordChange( void* pData )
{
	cMessageBox::DelMsg( 10019, false );
	SAFE_POINTER_RET( pData );
	GS2C_RECV_2ndPass_Change* result = static_cast< GS2C_RECV_2ndPass_Change* >(pData);

	if( 0 != result->nResult )// 2차 비밀번호 변경 오류
	{
		// 2차 비밀 번호 변경 오류 시 현재 비밀 번호 입력 창으로 이동
		cPrintMsg::PrintMsg( result->nResult );
		Notify( eCreate_CurrentPasswordWindow );
	}
	else
	{
		// 2차 비밀 번호 변경 성공 및 오류시		
		cPrintMsg::PrintMsg( cPrintMsg::SECOND_PASS_SET_COMPLETE );
		Notify( eCreate_AskPasswordWindow );
	}
}

void SecondPasswordContents::Recv2ndPasswordRegister( void* pData )
{
	cMessageBox::DelMsg( 10019, false );
	SAFE_POINTER_RET( pData );
	GS2C_RECV_2ndPass_Register* result = static_cast< GS2C_RECV_2ndPass_Register* >(pData);

	if( 0 != result->nResult )
		cPrintMsg::PrintMsg( result->nResult );// 2차 비밀번호 등록 오류

	cPrintMsg::PrintMsg( cPrintMsg::SECOND_PASS_SET_COMPLETE );
	// 2차 비밀 번호 등록 성공 및 실패시
	// 2차 비밀 번호 등록 창으로 이동함. 
	Notify( eCreate_AskPasswordWindow );
}