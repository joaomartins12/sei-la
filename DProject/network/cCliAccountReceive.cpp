

#include "stdafx.h"

#include "cCliAccount.h"

#include "cNetwork.h"
#include "common_vs2019/pLogin.h"
#include "common_vs2019/pPass2.h"

#include "../Flow/Flow.h"
#include "../Flow/FlowMgr.h"

#include "../nProtect/Client_nProtect.h"

#ifdef SDM_DEF_XIGNCODE3_20181107
#include "../xigncode3/Client_Xigncode3.h"
#endif



void cCliAccount::RecvLoginWaitTime(void)
{
	u4 nWaitTime;	// ms
	pop(nWaitTime);

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_LOGIN_WAIT_TIME, &nWaitTime);
}

// Login Request
void cCliAccount::RecvLoginRequest(void)
{
	DBG("\nSending Login request\n");
	GS2C_RECV_LOGIN_REQUEST recv;
	pop( recv.nResult );
	pop( recv.nSubType );
#ifdef DEBUG_NETWORK
	DBG("lOGIN REQUEST:\n RESULT -> %d\m SUBTYPE -> %d\n", recv.nResult, recv.nSubType);
#endif
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_LOGIN_REQUEST, &recv );

#ifdef DEF_CORE_NPROTECT
	if( 0 == recv.nResult )
	{
		std::wstring userID;
		DmCS::StringFn::ToMB( userID, GLOBALDATA_ST.GetAccountID() );
		nProtect_SendID(userID.c_str());
	}
#endif
	DBG("Login request received\n");
}

void cCliAccount::RecvHandshake487(void) {
	short HS;
	pop(HS);
	//DBG("\nReceived auth server handshake\n");
}

void cCliAccount::RecvAccountBan(void)
{
	u4 nBanSecond;
	std::wstring szBanMessage;	
	pop(nBanSecond);
	pop(szBanMessage);

	DoDisconnect();

	std::wstring banMsg = UISTRING_TEXT("CONNECTION_LIMIT_SERVER_BAN_MESSAGE");
	std::wstring wsHour;
	DmCS::StringFn::From( wsHour, nBanSecond/3600 );
	DmCS::StringFn::Replace( banMsg, L"#Hour#", wsHour );

	std::wstring wsMin;
	DmCS::StringFn::From( wsMin, (nBanSecond/60)%60 );
	DmCS::StringFn::Replace( banMsg, L"#Min#", wsMin );

	std::wstring wsSec;
	DmCS::StringFn::From( wsSec, nBanSecond%60 );
	DmCS::StringFn::Replace( banMsg, L"#Sec#", wsSec );

	DmCS::StringFn::Replace( banMsg, L"#Servermessage#", szBanMessage );

	CsMessageBox( MB_OK, banMsg.c_str());
	PostQuitMessage(0);
}

void cCliAccount::RecvBirthday(void)
{
	GS2C_RECV_Shutdown_Birthday recv;
	pop(recv.szBirthDay);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_SHUTDOWNSYSTEM_DATA, &recv );
}

void cCliAccount::RecvCheck2ndPass(void)
{
	DBG("\nSending SecondPassword check\n");
	GS2C_RECV_2ndPass_Certify recv;
	pop( recv.nResult );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_SECONDPASS_CHECK, &recv );
	DBG("SecondPassword check received\n");
}

void cCliAccount::Recv2ndPassRegister(void)
{
	DBG("\nSending SecondPassword register\n");
	GS2C_RECV_2ndPass_Register recv;
	pop( recv.nResult );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_SECONDPASS_REGISTER, &recv );
	DBG("SecondPassword register received\n");
}

void cCliAccount::Recv2ndPassChange(void)
{
	DBG("\nSending SecondPassword change\n");
	GS2C_RECV_2ndPass_Change recv;
	pop( recv.nResult );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_SECONDPASS_CHANGE, &recv );
	DBG("SecondPassword change received\n");
}

void cCliAccount::RecvClusterList(void)
{
	DBG("\nGetting ServerList\n");
	cMessageBox::DelMsg( 10019, false );

	if( g_pResist )
	{
		g_pResist->m_AccountGlobal.s_nLastConnetServer = 0;
		std::wstring szUserID;
		DmCS::StringFn::From( szUserID, GLOBALDATA_ST.GetAccountID() );
		g_pResist->LoadAccountGlobal( szUserID.c_str() );
		g_pResist->CheckTalk();
	}

	u1 cnt;
	pop(cnt);
	GS2C_RECV_SERVERLIST recv;
	for(uint i=0; i<cnt; i++)
	{	
		sServerInfo addInfo;
		pop(addInfo.m_nIDX);
		pop(addInfo.m_szClusterName);
		pop(addInfo.m_nDisable); // 아직 미사용 서버 이거나, 가동 중이지 아니한 서버		
		pop(addInfo.m_nClusterState);// 0 => 원할, 1 => 혼잡, 2 => FULL
		pop(addInfo.m_nTamerNumber);
		pop(addInfo.m_bNewServer);

		addInfo.m_nOpenTamerSlot = nLimit::OpenedTamerSlot;
		addInfo.m_nMaxTamerSlot = nLimit::MaxTamerSlot;

		if( addInfo.m_bNewServer )
			recv.m_listServer.push_front(addInfo);
		else
			recv.m_listServer.push_back(addInfo);
		if( _tcsicmp( g_pResist->m_AccountGlobal.s_szServerName, addInfo.m_szClusterName.c_str() ) == 0 )
			g_pResist->m_AccountGlobal.s_nLastConnetServer = addInfo.m_nIDX;
	}

	//DBG("Getting eCountry\n");

	if( GLOBALDATA_ST.IsCountry(GData::eCountry_GSP) || GLOBALDATA_ST.IsCountry(GData::eCountry_Hongkong) || GLOBALDATA_ST.IsCountry(GData::eCountry_Taiwan ) )
	{
		std::string mailAddress;
		pop( mailAddress );
		std::string AccessToken;
		pop( AccessToken );
		std::string Ticketingtime;
		pop( Ticketingtime );

		GLOBALDATA_ST.SetMailAddress( mailAddress );
		GLOBALDATA_ST.SetAccessToken( AccessToken );
		GLOBALDATA_ST.SetTicketingtime( Ticketingtime );
	}

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_SERVER_LIST, &recv);
	DBG("ServerList received\n");
}

void cCliAccount::RecvGateInfo(void)
{
	DBG("\nGetting CharacterServer Info\n");
	pop(net::account_idx);
	pop(net::access_code);

	pop(net::ip);
	pop(net::port);

#ifdef VERSION_KOR
	bool IsPcBang = false;
	pop( IsPcBang );

	g_pDataMng->GetPCbang()->SetPcBang( IsPcBang );
#endif

	net::cmd = Cmd::ConnectGateServer;
	DBG("CharacterServer Info received\n");
}

void cCliAccount::RecvVersionError(void)
{
	uint nSvrVersion;
	char msg[255];

	pop(nSvrVersion);
	pop(msg);

	char *r = strstr(msg, "\\n");
	while(r)
	{
		r[0] = '\n';
		for(int i=1; i<255; i++)
		{
			if(!r[i])
			{
				break;
			}
			r[i] = r[i+1];
		}

		r = strstr(msg, "\\n");
	}

	cPrintMsg::PrintMsg( cPrintMsg::SERVER_VERSION_ERROR );


#ifdef VERSION_ERROR_ROOLBACK	
	int nVersion = GLOBALDATA_ST.GetDownloadPatchVersion();
	int nNewDownVersion = nVersion - 2;
	cProfile x( VERSION_FILE );
	x.Write( "VERSION", "version", CsMax( 0, nNewDownVersion ) );
#endif
}

//////////////////////////////////////////////////////////////////////////

void cCliAccount::Recv_NProtectCheck()
{
#ifdef DEF_CORE_NPROTECT
	BYTE bPacket[4096] = {0,};
	DWORD dwPacketSize = 0;
	UINT unUniqueIdx = 0;

	pop( unUniqueIdx );
	pop( bPacket );
	pop( dwPacketSize );

	CSAuth3( unUniqueIdx, bPacket, dwPacketSize, 0 );
#endif
}

void cCliAccount::Recv_WeekShutDownData()
{
	sShutdownWeek WeekShutDownData;
	pop( WeekShutDownData );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_WEEK_SHUTDOWN_DATA, &WeekShutDownData );
}

void cCliAccount::RecvHashValue()
{
	DBG("\nGetting Hash");
	u2 bufferSize;
	pop( bufferSize );
	
	char *infos = new char[bufferSize + 1];
	memset( infos, 0, bufferSize + 1);

	pop( infos, bufferSize );
	
	GS2C_RECV_Resource_Hash recv;
	recv.szHash = infos;
	//DBG("\nHASH Code: %d", recv);
	delete infos;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::START_RESOURCECHECKER, &recv);
	DBG("\nHash Received\n");
}

void cCliAccount::RecvSelectCluster()
{
	DBG("Getting SelectCluster\n");
	AC2C_RECV_SERVERSELECT_CLUSTER recv;
	pop( recv.nResult );
#ifdef DEBUG_NETWORK
	DBG("RECV select cluster -> %d\n", recv.nResult);
#endif
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_SERVERSELECT_CLUSTER, &recv );
}

void cCliAccount::Recv_XignCodeCheck(void)
{
	#ifdef SDM_DEF_XIGNCODE3_20181107
		GS2C_RECV_XignCodePacket recv;
		pop( recv.szPacketData, XIGNCODE_PACKET_SIZE );
		pop( recv.nSize );
		XignCode_ProbeEx( recv.szPacketData, recv.nSize, NULL );
	#endif
}