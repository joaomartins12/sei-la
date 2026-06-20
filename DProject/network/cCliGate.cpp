

#include "StdAfx.h"

#include "cCliGate.h"
#include "cNetwork.h"

#include "common_vs2019/pMaster.h"
#include "common_vs2019/pGate.h"
#include "common_vs2019/pNetwork.h"
#include "common_vs2019/pServer.h"
#include "common_vs2019/pSession.h"
#include "common_vs2019/cVersion.h"

#include "common_vs2019/pPass2.h"


#include "../Flow/Flow.h"
#include "../Flow/FlowMgr.h"


cCliGate::cCliGate(void) : cClient(nSession::User)
{
	m_nIsChangeServer = false;


}


cCliGate::~cCliGate(void)
{
	cClient::Stop();

	DBG("destroyed...");
}


bool cCliGate::start(void)
{
	if(IsNetOff())
	{
		DBG("NETWORK IS DISABLED...");
//		MessageBoxA(null, "\n서버와 연결하지 않고 시작합니다.   ", "digimon masters", MB_OK);
		return true;
	}

	int nConnectTryCount = 0;

nextwork:
	if(Bind() == false)
	{
		DBG("gate bind failure");
		PostQuitMessage(nErr::Bind);
		return false;
	}

	EnableEncryption( cVersion::GetVersion() );

	if(!Connect(ip, port))
	{
		if(++nConnectTryCount < 5)
		{
			DBG("connect(%s:%d) try:%d failure", ip, port, nConnectTryCount);
			Sleep( 999 );
			goto nextwork;
		}

		PostQuitMessage(nErr::Connect);
		DBG("connect(%s:%d) failure...", ip, port);
		return false;
	}

//	if(Run(2) == false) // 2 work thread
	if(Run(0) == false)
	{
		DBG("run failure");
		PostQuitMessage(nErr::Run);
		return false;
	}

	return true;
}



void cCliGate::OnConnected(void)
{
#ifndef _GIVE
	LOG("게이트 서버 접속");
#endif //_GIVE

	SendAccessCode();
	//SetWindowTitle(true);

//#ifndef _GIVE
	//Sleep(333);
//#endif //_GIVE
}


void cCliGate::OnDisconnected(char *msg)
{
#ifdef _DEBUG
	CsMessageBoxA( MB_OK, "서버 끊김 : %s", msg );
#endif

#ifndef _GIVE
	LOG("게임 서버 접속 종료");
#endif //_GIVE

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


void cCliGate::OnExecute(void)
{
	switch(GetProtocol())
	{
#ifdef HANDSHAKE_DISABLE
	case 65535: RecvHandshake487(); return;
#endif
	case pSvr::Change			: RecvChangeServer();			return;
	case pMaster::BanUser		: RecvBanUser();				return;
	case pMaster::DiscUser		: RecvDiscUser();				return;
	case pTamer::Remove			: RecvRemoveTamer();			return;

	case pTamer::List			: RecvTamerList();				return;
	case pTamer::CreateSuccess	: RecvTamerCreateSuccess();		return;
	case pTamer::CreateFailure	: RecvTamerCreateFailure();		return;	

	case pTamer::SelectSuccess	: RecvTamerSelectSuccess();		return;
	case pTamer::SelectFailure	: RecvTamerSelectFailure();		return;

	case pTamer::CheckDoubleName: RecvCheckDoubleName();		return;
	case pTamer::CreateWaitTime	: RecvTamerCreateWaitTime();	return;
#ifdef SERVER_WAITING_LINE
	case pGate::WaitTime		: RecvWaitTime();		return;
#endif
	}


	DBG("unknown protocol(%d)", GetProtocol());
	xassert1(false, "unknown protocol(%d)", GetProtocol());
}








