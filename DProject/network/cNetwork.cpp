
#include "stdafx.h"

#include "cNetwork.h"

#include "common_vs2019/pGame.h"
#include "common_vs2019/pLogin.h"
#include "common_vs2019/pNetwork.h"
#include "common_vs2019/pSession.h"

#include "common_vs2019/pPass2.h"

extern void StartNetSync(void);
extern void EndNetSync(void);

bool IsNetOff(void)
{
	return false;
//	return net::disable;
}

void CHAT_LOG(char *msg)
{
#ifdef _GIVE 
	return;
#endif // _GIVE

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	DmCS::StringFn::From( CProtocol.m_wStr, msg );

	CProtocol.m_wStr = nBase::m2w(msg);

	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void CHAT_LOG(TCHAR *msg)
{
#ifdef _GIVE 
	return;
#endif // _GIVE

	LOG(nBase::w2m(msg).c_str());

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = msg;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

//void chat(uint type, char *fmt, ...)
//{
//	char msg[nBase::eMaxStrSize];
//
//	va_list args;
//	va_start(args, fmt);
//	vsprintf(msg, fmt, args);
//	va_end(args);
//
//	net::chat(msg);
//}

//void chat_system(char *fmt, ...)
//{
//	char msg[nBase::eMaxStrSize];
//
//	va_list args;
//	va_start(args, fmt);
//	vsprintf(msg, fmt, args);
//	va_end(args);
//
//	net::chat(msg);
//}


void SetWindowTitle(int IsOnline, int nMapID)
{
#ifdef USE_DMO_INI

	static int s_nMapID = 0;
	static int s_nIsOnline = 0;

	if(nMapID > -1)
	{
		s_nMapID = nMapID;
	}

	if(IsOnline > -1)
	{
		s_nIsOnline = IsOnline;
	}


	char szWindowText[222];
#ifdef _DEBUG
	char* szText2 = "debug";
#else
	char* szText2 = "release";
#endif
	sprintf(szWindowText, "Digimon Masters(%d) + %s [%s]", s_nMapID, szText2, __TIMESTAMP__);
	SetWindowTextA(GAMEAPP_ST.GetHWnd(), szWindowText);

#endif
}


void SendChangeServer(void);

namespace net
{
	cCliGame *game           = null;
	cCliGame *game_tmp       = null;

	cCliGate *gate           = null;
	cCliGate *gate_tmp       = null;

	cCliAccount *account     = null;
	cCliAccount *account_tmp = null;

	cClient *net = null; // current client session

	char ip[Language::pLength::ip+1] = {0, };
	uint port = 0;	// tmp ip and port
	uint cmd = Cmd::None;

	uint nIsMoveMapByCommand = true;

	bool bInitGameDataReady = false;
	volatile LONG g_lInitGameDataReady = 0;

	uint account_idx = 0;
	uint access_code = 0;
	//bool IsAeria = false;
	uint IsLogin = 0;
	//bool IsWebLoginVersion = false;

	//char id[Language::pLength::id + 1]     = {0, };
	//char pass[Language::pLength::pass+ 1] = {0, };
	//char Auth_Code[Language::pLength::str*2] = {0, };
	//char secondPass[Language::pLength::pass+ 1] = {0,};
	//char cAccessToken[Language::pLength::AccessToken + 1] = {0,};
	//char MailAddress[Language::pLength::email + 1] = {0,};	// 유져 메일 주소
	//char cTicketingtime[MAX_ANAL_LEN] = {0,};

	//char szUsrType[8] = {0, };

	int UsrType = 0;

	uint next_map_no = 0;
	uint prev_map_no = 0;
	bool bPortalMove = false;
	bool bChangeChannel = false;

	//char account_ip[Language::pLength::ip+1];
	//uint account_port = 0;

	uint cluster = 0;
	int  svr_type = net::type::none;

	bool isLogOut = true;

	//bool disable = false;	// network disable
	bool receive_active = true;
#ifdef DEF_CORE_NPROTECT
	UINT	nProtect_UniqueIdx;
	DWORD	nProtect_dwServerNumber;
	DWORD	nProtectBot_dwPacketSize;
	BYTE	nProtectBot_bPacket[4096];
	DWORD	nProtect_dwPacketSize;
	BYTE	nProtect_bPacket[4096];	
#endif

// function
	void start(void)
	{
		StartNetSync();
#ifdef DEF_CORE_NPROTECT
		nProtect_UniqueIdx = 0;
		nProtect_dwServerNumber = 0;
		nProtect_dwPacketSize = 0;
		memset(nProtect_bPacket,0,sizeof(BYTE)*4096);
		nProtectBot_dwPacketSize = 0;
		memset(nProtectBot_bPacket,0,sizeof(BYTE)*4096);		
#endif
	}
	
	void end(void)
	{
		EndNetSync();
	}

	void load(void)
	{
		/*cProfile x("./dmo.ini");

		strcpy_s(net::account_ip, x.GetStr("network", "ip"));
		net::account_port = x.GetInt("network", "port");

		if(_stricmp("on", x.GetStr("network", "power")))
		{
			SetWindowTitle(false);
			net::disable = true;
			return;
		}

		SetWindowTitle(true);*/

//		strcpy_s(net::account_ip, "0.0.0.1");
//		net::account_port = 1111;
	} // net::load

	// ------------------------------------------------------------------------------------------------------------

	bool start(int type, char const *ip, uint port)
	{
		if(IsNetOff())
			return false;
		
		receive_active = true;

		if(net)
		{
			switch(svr_type)
			{
			case type::game:
				//xdelete(game_tmp);

				((cCliGame*)net)->m_nIsChangeServer = true;
				net->cClient::DoDisconnect();

				Sleep(100);
				break;
			case type::gate:
				xdelete(gate_tmp);
				
				((cCliGate*)net)->m_nIsChangeServer = true;
				net->cClient::DoDisconnect();
		
				Sleep(100);
				break;
			case type::account:
				xdelete(account_tmp);

				((cCliAccount*)net)->m_nIsChangeServer = true;
				net->cClient::DoDisconnect();
		
				Sleep(100);
				break;
			}			
		}

		int res = -1;
		net::svr_type = type;

		switch (type)
		{
			case type::game:
				if (game == nullptr)
				{
					if (game_tmp != nullptr)
						xdelete(game_tmp);
				
					game = xnew cCliGame();

					strcpy(game->ip, ip);
					game->port = port;

					net = game;
					res = game->start();

					net::isLogOut = false;
				}
				else
				{
					xdelete(game_tmp);

					game = xnew cCliGame();

					strcpy(game->ip, ip);
					game->port = port;

					net = game;
					res = game->start();

					net::isLogOut = false;
				}
				break;

			case type::gate:
				xdelete(gate_tmp);

				if (net::isLogOut == false)
				{
					gate = xnew cCliGate();

					net = gate;
					strcpy(gate->ip, ip);

					gate->port = port;
					res = gate->start();
				}
				else
				{
					//if (gate != nullptr)
						//xdelete(gate_tmp);
					
					gate = xnew cCliGate();

					strcpy(gate->ip, ip);
					gate->port = port;

					net = gate;
					res = gate->start();
				}
				break;

			case type::account:
				if (net::isLogOut == false)
				{
					account = xnew cCliAccount();

					strcpy(account->ip, ip);
					account->port = port;

					net = account;
					res = account->start();

					Sleep(100);
				}
				else
				{
					if (account != nullptr)
						xdelete(account_tmp);
					
					Sleep(100);

					account = xnew cCliAccount();

					strcpy(account->ip, ip);
					account->port = port;

					net = account;
					res = account->start();
				}
				break;

			default:
			{
				xassert(false, "type is not valid");
				return false;
			}
		}

		if(res == true)
			return true;
		
//		xdelete(net::net);

		if(res == false)
		{
			cPrintMsg::PrintMsg(10051);
			g_CriticalLog.Log(_T("\n[SISTEMA] res = %d", res));

			return false;
		}

		xassert(false, "type is not valid");
		return false;
	}

	void stop(void)
	{
		// `xdelete` is just `delete` — it does NOT null the pointer.  Without
		// this, dangling globals fool the `if(net::account && ...)` guard in
		// CheckConnectAccountSever after logout.
		net = null;
		xdelete(game);        game        = null;
		xdelete(gate);        gate        = null;
		xdelete(account);     account     = null;
		xdelete(game_tmp);    game_tmp    = null;
		xdelete(gate_tmp);    gate_tmp    = null;
		xdelete(account_tmp); account_tmp = null;
	} // net::stop

	void execute(void)
	{
		if(!net)
			return;

#ifdef DEF_CORE_NPROTECT
		// nProtect 승인
		if( nProtect_dwPacketSize )
		{
			nProtect_Send();

			memset(nProtect_bPacket,0,sizeof(BYTE)*4096);
			nProtect_dwServerNumber = 0;
			nProtect_dwPacketSize = 0;
		}
		// nProtect 봇 탐지
		if( nProtectBot_dwPacketSize )
		{
			nProtect_BotDetect();

			memset(nProtectBot_bPacket,0,sizeof(BYTE)*4096);
			nProtectBot_dwPacketSize = 0;
		}
#endif
		net->DoExecute();

		if(cmd == Cmd::None)
			return;

		switch(cmd)
		{
		case Cmd::ConnectAccountServer:
			break;
		case Cmd::ConnectGateServer:
			start(type::gate, ip, port);
			break;
		case Cmd::ConnectGameServer:
			start(type::game, ip, port);
			break;
		}
		cmd = Cmd::None;
	} // net::execute

	void exit(uint code)
	{
		char str[222] = { 0, };

		switch(code)
		{
		case nErr::Bind		:
			sprintf(str, "bind(%s:%d) failure     ", net::account->ip, net::account->port);
			MessageBoxA(null, str, "dmo network", MB_OK);
			break;
		case nErr::Connect	:
			sprintf(str, "connect(%s:%d) failure     ", net::account->ip, net::account->port);
			MessageBoxA(null, str, "dmo network", MB_OK);
			break;
		case nErr::Run		:
			MessageBoxA(null, "worker-run failure       ", "dmo network", MB_OK);
			break;
		}

		net = null;

		DBG("game : 0x%p", game);
		DBG("game_tmp : 0x%p", game_tmp);
		xdelete(game);
		xdelete(game_tmp);

		DBG("gate : 0x%p", gate);
		DBG("gate_tmp : 0x%p", gate_tmp);
		xdelete(gate);
		xdelete(gate_tmp);

		DBG("account : 0x%p", account);
		DBG("account_tmp : 0x%p", account_tmp);
		xdelete(account);
		xdelete(account_tmp);
	} // net::exit

	// ------------------------------------------------------------------------------------------------------------

	void Xigncode_SendPacket(char* Buffer, SIZE_T BufferSize)
	{
		switch(svr_type)
		{
			case type::game:	((cCliGame*)net)->NTF_Send_Xigncode( Buffer, BufferSize );		break;
			case type::gate:	((cCliGate*)net)->NTF_Send_Xigncode( Buffer, BufferSize );		break;
			case type::account:	((cCliAccount*)net)->NTF_Send_Xigncode( Buffer, BufferSize );	break;
		}
	}

#ifdef DEF_CORE_NPROTECT
	void nProtect_BotDetect( UINT unUniqueIdx, const BYTE* Buffer, DWORD BufferSize )
	{
		if( NULL == Buffer )
			return;
		if( BufferSize > sizeof(nProtectBot_bPacket) )
			return;

		nProtect_UniqueIdx = unUniqueIdx;
		nProtectBot_dwPacketSize = BufferSize;
		memcpy( nProtectBot_bPacket, Buffer , sizeof(BYTE)*BufferSize);
	}

	void nProtect_BotDetect()
	{
		if( NULL == nProtectBot_bPacket )
			return;

		switch(svr_type)
		{
		case type::game :	((cCliGame*)net)->NTF_Send_nProtect_BotDetect( nProtect_UniqueIdx, nProtectBot_bPacket, nProtectBot_dwPacketSize );		break;
		case type::gate :	((cCliGate*)net)->NTF_Send_nProtect_BotDetect( nProtect_UniqueIdx, nProtectBot_bPacket, nProtectBot_dwPacketSize );		break;
		case type::account :((cCliAccount*)net)->NTF_Send_nProtect_BotDetect( nProtect_UniqueIdx, nProtectBot_bPacket, nProtectBot_dwPacketSize );	break;
		}
	}

	void nProtect_Send( UINT unUniqueIdx, const BYTE* Buffer, DWORD BufferSize ,DWORD GameServerIndex )
	{
		if( NULL == Buffer )
			return;
		if( BufferSize > sizeof(nProtect_bPacket) )
			return;

		nProtect_UniqueIdx = unUniqueIdx;
		nProtect_dwServerNumber = GameServerIndex;
		nProtect_dwPacketSize = BufferSize;
		memcpy( nProtect_bPacket, Buffer , sizeof(BYTE)*BufferSize);
	}

	void nProtect_Send()
	{
		if( NULL == nProtect_bPacket )
			return;

		switch(svr_type)
		{
		case type::game :	((cCliGame*)net)->ACK_Send_nProtect( nProtect_UniqueIdx, nProtect_bPacket, nProtect_dwPacketSize );		break;
		case type::gate :	((cCliGate*)net)->ACK_Send_nProtect( nProtect_UniqueIdx, nProtect_bPacket, nProtect_dwPacketSize );		break;
		case type::account :((cCliAccount*)net)->ACK_Send_nProtect( nProtect_UniqueIdx, nProtect_bPacket, nProtect_dwPacketSize );	break;
		}
	}
#endif
}; // namespace net


void SendChangeServer(void)
{
	static int nCnt = 0;
	if(net::svr_type == net::type::game)
	{
		LOG("게임 서버 변경 %d", ++nCnt);
		((cCliGame*)net::net)->SendChangeServer();
	}

	if(net::svr_type == net::type::gate)
	{
		LOG("게이트 서버 변경 %d", ++nCnt);
		((cCliGate*)net::net)->SendChangeServer();
	}

	Sleep(33); Sleep(33); Sleep(33);
}
