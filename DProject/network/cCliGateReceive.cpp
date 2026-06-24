


#include "Stdafx.h"

#include "cCliGate.h"
#include "cNetwork.h"

#include "common_vs2019/pPass2.h"

#include "../Flow/Flow.h"
#include "../Flow/FlowMgr.h"

#include "../nProtect/Client_nProtect.h"

#ifdef SDM_DEF_XIGNCODE3_20181107
#include "../xigncode3/Client_Xigncode3.h"
#endif

void cCliGate::RecvChangeServer(void)
{
	net::cmd = Cmd::ConnectGameServer;
}


void cCliGate::RecvTamerCreateWaitTime(void)
{
	u4 nWaitTick;
	pop(nWaitTick);	// ms

#pragma todo("테이머 생성시 대기 시간 처리")	
}

void cCliGate::RecvHandshake487(void) {
	short HS;
	pop(HS);
	//DBG("Received char server handshake\n");
}


void cCliGate::RecvTamerList(void)
{
	DBG("\nReceiving tamer list\n");

	n1 nSlotNo = 99;
	pop(nSlotNo);

	GS2C_RECV_CHARACTER_LIST recv;
	
	while(nSlotNo != 99)
	{
		sSimpleCharacterInfo add;
		add.m_nSlotNumber = nSlotNo;
		DBG("\nCharacter slot: %d\n", add.m_nSlotNumber);

		pop( add.m_nMapID );
		pop( add.m_nTamerObjectTBIdx );
		pop( add.m_nTamerLevel );
		pop( add.m_szTamerName );
		pop(add.m_TamerEquip, sizeof(add.m_TamerEquip));

		DBG("MapID: %d | TamerLevel: %d | TamerName: %d\n", add.m_nMapID, add.m_nTamerLevel, add.m_szTamerName);

		pop( add.m_nDigimonObjectTBIdx );
		pop( add.m_nDigimonLevel );
		pop( add.m_szDigimonName );
		pop( add.m_nDigimonScale );
#ifdef SDM_DIGIMON_PARTSSYSTEM_20200115
		pop( add.m_nDigimonEffectType );// 디지몬 이펙트 타입 추가
#endif
		pop( add.m_nPat_Leader );// 카드리더 설정
#ifdef SDM_TAMER_SERVERRELOCATE_20170911
		pop(add.m_nRelocateState);
#else
		int NOP;
		pop(NOP);
#endif
		
		recv.m_listCharacterInfo.push_back( add );
		pop(nSlotNo);
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CHAR_SELETECT_LIST, &recv );
	DBG("\nTamer list received\n");
}

void cCliGate::RecvTamerCreateSuccess(void)
{
	u8 nTamerIDX;
	u8 nDigimonIDX; // partner digimon...

	pop(nTamerIDX);
	pop(nDigimonIDX);

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	n1 nSlotNo = 99;
	pop(nSlotNo);

	GS2C_RECV_NEW_CHARACTER_INFO recv;

	recv.m_NewCharInfo.m_nSlotNumber = nSlotNo;
	pop( recv.m_NewCharInfo.m_nMapID );
	pop( recv.m_NewCharInfo.m_nTamerObjectTBIdx );
	pop( recv.m_NewCharInfo.m_nTamerLevel );
	pop( recv.m_NewCharInfo.m_szTamerName );
	pop( recv.m_NewCharInfo.m_TamerEquip, sizeof(recv.m_NewCharInfo.m_TamerEquip) );
	pop( recv.m_NewCharInfo.m_nDigimonObjectTBIdx );
	pop( recv.m_NewCharInfo.m_nDigimonLevel );
	pop( recv.m_NewCharInfo.m_szDigimonName );
	pop( recv.m_NewCharInfo.m_nDigimonScale );
	pop( recv.m_NewCharInfo.m_nPat_Leader );// 카드리더 설정

	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RECV_ADD_NEW_CHARACTER, &recv );

	int nResult = 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RECV_CHAR_CREATE_RESULT, &nResult );
}

void cCliGate::RecvTamerCreateFailure(void)
{
	uint uiResult;
	pop(uiResult);

	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RECV_CHAR_CREATE_RESULT, &uiResult );

	xassert1(false, "unknown err_code(%d)", uiResult);
}

void cCliGate::RecvTamerSelectSuccess(void)
{
	pop(net::ip);
	pop(net::port);
	pop(net::next_map_no);

	char log[512] = { 0 };
	sprintf_s(
		log,
		"[GATE][RECV] RecvTamerSelectSuccess ip=%s port=%d next_map_no=%d\n",
		net::ip,
		net::port,
		net::next_map_no
	);
	OutputDebugStringA(log);

	// IMPORTANTE:
	// O server atual envia o IP/porta do GameServer neste packet, mas pode não enviar
	// o packet separado RecvChangeServer(). Sem isto, o client muda para Loading,
	// mas nunca tenta abrir socket para o GameServer.
	net::cmd = Cmd::ConnectGameServer;

	OutputDebugStringA("[GATE][RECV] RecvTamerSelectSuccess set net::cmd = Cmd::ConnectGameServer\n");

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_CHAR_SELECT_RESULT);
}

void cCliGate::RecvTamerSelectFailure(void)
{
	u4 mapID;
	pop( mapID );

	cMessageBox::DelMsg( 10019, false );

	cPrintMsg::PrintMsg( cPrintMsg::CHARCREATE_SELECT_CHAR_ERROR, &mapID );
}

void cCliGate::RecvCheckDoubleName(void)
{
	uint nRes;
	pop(nRes);

	GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_CHECK_DOUBLE_NAME, &nRes );
}

void cCliGate::RecvDiscUser(void)
{
	u4 nDiscSecond;
	char szDiscMessage[Language::pLength::str];

	pop(nDiscSecond);
	pop(szDiscMessage);

	DoDisconnect();

	CsMessageBoxA( MB_OK, "잠시 연결을 종료합니다.\n\n남은시간 : %02d시 %02d분 %02d초\n\n종료사유 : %s    ", nDiscSecond/3600, (nDiscSecond/60)%60, nDiscSecond%60, szDiscMessage);
	PostQuitMessage(0);
}

void cCliGate::RecvBanUser(void)
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

// 	u4 nBanSecond;
// 	std::wstring szBanMessage;
// 
// 	pop(nBanSecond);
// 	pop(szBanMessage);
// 
// 	DoDisconnect();
// #ifdef VERSION_USA
// 	CsMessageBox( MB_OK, L"Limiting access to Game.\n\nTime : %02dHour %02dMin %02dSec    \n\nReason : %s    ", nBanSecond/3600, (nBanSecond/60)%60, nBanSecond%60, szBanMessage.c_str());
// #elif( defined VERSION_TW || defined VERSION_HK )
// 	CsMessageBox( MB_OK, L"現在連線已被限制\n\n剩餘時間 : %02d小時 %02d分 %02d秒    \n\n限制理由 : %s      ", nBanSecond/3600, (nBanSecond/60)%60, nBanSecond%60, szBanMessage.c_str() );
// #else
// 	CsMessageBoxA( MB_OK, "접속 제한 되었습니다\n\n남은시간 : %02d시 %02d분 %02d초    \n\n제한사유 : %s    ", nBanSecond/3600, (nBanSecond/60)%60, nBanSecond%60, szBanMessage.c_str());
// #endif // ( defined VERSION_TW || defined VERSION_HK )
// 	PostQuitMessage(0);
}

void cCliGate::RecvRemoveTamer(void)
{
	std::pair<int, std::string> pairRes;
	pop(pairRes.first);
	switch( pairRes.first )
	{
	case 3:	// 캐릭터 생성 후 일주일안에 삭제할 수 없습니다.
		{
			char szDate[32] = {0, };
			pop( szDate );
			pairRes.second = szDate;
		}break;
	default:
		break;
	}	
	
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_CHAR_DELETE_RESULT, &pairRes );
}

#ifdef SERVER_WAITING_LINE
void cCliGate::RecvWaitTime(void)
{
	u2	uWaitLine;

	pop(uWaitLine);

	
// 	int nTemp = uWaitLine;
// 
// 	//nTemp = 2;
// 	if( 0 == nTemp)
// 		nTemp =1;
// 	g_pDataMng->GetAccount()->m_WaitEventTime.Reset();
// 	g_pDataMng->GetAccount()->m_WaitEventTime.SetEvent(nTemp, 10000); // 10초동안	
// 	g_pDataMng->GetAccount()->m_WaitEventTime.SetEnableTime(true);

}
#endif

void cCliGate::Recv_NProtectCheck()
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

void cCliGate::Recv_XignCodeCheck(void)
{
#ifdef SDM_DEF_XIGNCODE3_20181107
	GS2C_RECV_XignCodePacket recv;
	pop( recv.szPacketData, XIGNCODE_PACKET_SIZE );
	pop( recv.nSize );
	XignCode_ProbeEx( recv.szPacketData, recv.nSize, NULL );
#endif
}