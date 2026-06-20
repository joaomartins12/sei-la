

#include "StdAfx.h"

#include "cNetwork.h"
#include "cCliAccount.h"

#include "common_vs2019/pLogin.h"
#include "common_vs2019/pServer.h"
#include "common_vs2019/pSession.h"
#include "common_vs2019/pNetwork.h"
#include "common_vs2019/pGate.h"
#include "common_vs2019/pPass2.h"
#include "common_vs2019/pShutdown.h"
#include "common_vs2019/cVersion.h"


#include "../Flow/Flow.h"
#include "../Flow/FlowMgr.h"

cCliAccount::cCliAccount(void) : cClient(nSession::User)
{
	m_nIsChangeServer = false;
	

}


cCliAccount::~cCliAccount(void)
{
	cClient::Stop();

	DBG("destroyed...");
}


bool cCliAccount::start(void)
{
	if(IsNetOff())
	{
		LOG("NETWORK IS DISABLED...");
		return true;
	}
	
	int nConnectTryCount = 0;

	if(Bind() == false)
	{
		LOG("bind failure");
		return false;
	}

	EnableEncryption( cVersion::GetVersion() );

	if(!Connect(ip, port))
	{
		LOG("connect(%s:%d) failure...", ip, port);

		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RESET_LOGIN_TIME_SEQ );
		return false;
	}

	if(Run(0) == false)
	{
		DBG("run failure");
		return false;
	}

	return true;
}

void cCliAccount::OnConnected(void)
{
	//SetWindowTitle(true);

#ifdef DEBUG_NETWORK
	DBG("On cli account connected\n");
#endif

	SendLogin();
}


void cCliAccount::OnDisconnected(char *msg)
{
//#ifndef _GIVE
	LOG(msg);
//#endif //_GIVE

	if(stricmp(msg, "111") == 0)//이미 접속 중인 계정일 경우
	{
		cPrintMsg::PrintMsg( 10014 );
		return;
	}

	if(stricmp(msg, "999") == 0)
	{
		cPrintMsg::PrintMsg( 10013 );
		return;
	}

	if(m_nIsChangeServer)
	{
		return;
	}

	SetWindowTitle(false);
	cPrintMsg::PrintMsg( cPrintMsg::SERVER_DISCONNET );
}


void cCliAccount::OnExecute(void)
{
	switch(GetProtocol())
	{

#ifdef HANDSHAKE_DISABLE
	case 65535: RecvHandshake487(); return;
#endif
	case pLogin::WaitTime	: RecvLoginWaitTime();	return;

	case pLogin::Request	: RecvLoginRequest();	return;
	case pLogin::AccountBan : RecvAccountBan();		return;
	case pLogin::Birthday	: RecvBirthday();		return;
	case pPass2::Check		: RecvCheck2ndPass();			return;	// 2차 비밀 번호 입력에 대한 결과값
	case pPass2::Register	: Recv2ndPassRegister();		return;	// 2차 비밀 번호 등록에 대한 결과값
	case pPass2::ChangePass	: Recv2ndPassChange();			return;	// 2차 비밀 번호 변경에 대한 결과값
	case pCertify::IntegrityHash:	RecvHashValue();		return;
	
	//case pLogin::Success	: RecvLoginSuccess();	return;
	//case pLogin::Failure	: RecvLoginFailure();	return;
	//case pLogin::OnTheAnvil	: RecvOnTheAnvil();		return;
	//GSP
	//case pLogin::FailureGSP : RecvLoginFailureGSP(); return;

	case pGate::SvrInfo		: RecvGateInfo();		return;
	case pSvr::ClusterList	: RecvClusterList();	return;
	case pNet::VersionError	: RecvVersionError();	return;
	//case pPass2::Change		: RecvPassChange();				return;
	case pShutdown::WeekCheck: Recv_WeekShutDownData();		return;
	case pShutdown::WeekCheckError:							
		return;
	case pSvr::SelectCluster: RecvSelectCluster();	return;
	}


	xassert1(false, "unknown protocol(%d)", GetProtocol());
}






