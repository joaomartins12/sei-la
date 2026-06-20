#include "stdafx.h"

#include "cCliGame.h"

#include "nlib/base.h"
#include "nlib/cToken.h"

#include <shellapi.h>

#include "common_vs2019/pGame.h"
#include "common_vs2019/pItem.h"
#include "common_vs2019/pMaster.h"
#include "common_vs2019/pMonster.h"
#include "common_vs2019/pParty.h"

#include "../LibProj/csFileTable/MapListMng.h"


void cCliGame::RecvCheatLevel(void)
{
	int nRes;
	wchar szTamerName[Language::pLength::name+1];

	pop(nRes);
	pop(szTamerName);

	std::wstring wsMsg = UISTRING_TEXT( "CHAT_RESULT_VALUE_ERROR" );

	switch(nRes)
	{
	case 0 :
		{
			wsMsg = UISTRING_TEXT( "CHAT_NOT_FOUND_IN_MAP" );
			DmCS::StringFn::Replace( wsMsg, L"#Name#", szTamerName );
		}
		break;
	case 1 :
		{
			wsMsg = UISTRING_TEXT( "CHAT_ACCOUNT_NOT_EXIST" );
			DmCS::StringFn::Replace( wsMsg, L"#Name#", szTamerName );
		}
		break;
	case 2 :
		{
			wsMsg = UISTRING_TEXT( "CHAT_CHEAT_LEVEL_CHANGED" );
			DmCS::StringFn::Replace( wsMsg, L"#Name#", szTamerName );
		}
		break;
	}
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = wsMsg;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cCliGame::RecvInsertItemToShop(void)
{
	n2 nRes;
	pop(nRes);

	wstring str;

	if( nRes == 0 )
	{
		str = UISTRING_TEXT( "CHAT_SUCCESS_ADDED_TO_CASHSHOP" );
	}
	else if( nRes == 1 ) 
	{
		str = UISTRING_TEXT( "CHAT_NOT_EXIST_ITEMCODE" );
	}
	else if( nRes == 2 ) 
	{
		str = UISTRING_TEXT( "CHAT_MAXIMUM_NUMBER_OF_STORED" );
	}
	else if( nRes == 3 ) 
	{
		str = UISTRING_TEXT( "CHAT_NOTHING_TO_ADD" );
	}
	else if( nRes == 4 ) 
	{
		str = UISTRING_TEXT( "CHAT_NOT_EXIST_ID" );
	}
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = str;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cCliGame::RecvInit2ndPass()
{
	n2 nRes;
	pop(nRes);

	wstring str;

	if( nRes == 0 )
	{
		str = UISTRING_TEXT( "CHAT_SECOND_PASSWORD_RESET_SUCCESS" );
	}
	else if( nRes == 1 ) 
	{
		str = UISTRING_TEXT( "CHAT_NOT_EXIST_ID" );
	}

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = str;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cCliGame::SendChangeChannel(uint nChannelIDX)
{
	newp(pGame::ChangeChannel);
	push(nChannelIDX);
	endp(pGame::ChangeChannel);
	send();
}

void cCliGame::RecvUserInfo(void)
{
	char szTamerName[Language::pLength::name];
	char szID[Language::pLength::id];
	u1 cIP[4];
	n2 nMapIDX;
	u1 nChannelNumber;
	nSync::Pos pos;
	u4 nLoginTimeTS;
	u1 nIsParty;

	pop(szTamerName);
	pop(szID);
	pop(cIP, 4);
	pop(nMapIDX);
	pop(nChannelNumber);
	pop(pos);
	pop(nLoginTimeTS);
	pop(nIsParty);
	

	char msg[222];

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;

	sprintf(msg, "  TamerName : %s", szTamerName);
	CProtocol.m_wStr = nBase::m2w(msg);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	sprintf(msg, "     UserID : %s", szID);
	CProtocol.m_wStr = nBase::m2w(msg);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	sprintf(msg, " IP ADDRESS : %d.%d.%d.%d", cIP[0], cIP[1], cIP[2], cIP[3]);
	CProtocol.m_wStr = nBase::m2w(msg);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	sprintf(msg, "    MapInfo : MapID( %d ) CH( %d ) (%d,%d)", nMapIDX, nChannelNumber, pos.m_nX, pos.m_nY);
	CProtocol.m_wStr = nBase::m2w(msg);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	sprintf(msg, "  LoginTime : %s", _ctime32((__time32_t*)&nLoginTimeTS));
	CProtocol.m_wStr = nBase::m2w(msg);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	if(nIsParty)
		sprintf(msg, " IsParty : On");
	else
		sprintf(msg, " IsParty : Off");
	CProtocol.m_wStr = nBase::m2w(msg);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}


void cCliGame::RecvUserInfoNotFound(void)
{
	char szName[Language::pLength::name]; // id or name
	pop(szName);

	char msg[222];
	sprintf(msg, "(%s) is not found", szName);

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = nBase::m2w(msg);
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}


void cCliGame::RecvUserList(void)
{
	u2 cnt = 0;
	pop(cnt);

	//////////////////////////////////////////////////////////////////////////
#ifdef SDM_CHET_ULIST_LOG_FILE_SAVE_20190110
// 	CsLog log;
// 	log.Init( L"uList", true, true );	
// 	log.Log( L"Total User Count : %d", cnt );
	DmCS::Storage::File saveFile;
	saveFile.Create( L"uList.txt", L"wt, ccs=UNICODE" );
	saveFile.WriteVA( L"Total User Count : %d\n", cnt );
#endif

	std::wstring wsUserID;
	std::wstring wsTamerName;
	std::wstring msg;

	for(u2 i=1; i<=cnt; i++)
	{
		pop(wsUserID);
		pop(wsTamerName);
		DmCS::StringFn::Format( msg, L"[%03d] %s %s\n", i, wsUserID.c_str(), wsTamerName.c_str() );

#ifdef SDM_CHET_ULIST_LOG_FILE_SAVE_20190110
		//log.Log( msg.c_str());
		saveFile.Write( msg );
#endif
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = msg;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	}

	msg.clear();
	DmCS::StringFn::Format( msg, L"[ %03d ] 명 접속중", cnt );	
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = msg;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol ); 

#ifdef SDM_CHET_ULIST_LOG_FILE_SAVE_20190110
	ShellExecute( NULL, _T( "open" ), _T( "notepad.exe" ), L"uList.txt", NULL, SW_SHOW );
#endif
	//////////////////////////////////////////////////////////////////////////
}


void cCliGame::RecvConnCountInfo(void)
{
	u2 nMapIDX;
	u1 nChannelIDX;
	u2 nConnCount;

	char msg[Language::pLength::str];

	pop(nMapIDX);
	while(nMapIDX)
	{
		pop(nChannelIDX);
		while(nChannelIDX != 0xFF)
		{
			pop(nConnCount);

			sprintf(msg, "%04d - %02d : %03d", nMapIDX, nChannelIDX, nConnCount);
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
			CProtocol.m_wStr = nBase::m2w(msg);
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
			pop(nChannelIDX);
		}

		pop(nMapIDX);
	}
}

void cCliGame::RecvDiscUser(void)
{
	u4 nDiscSecond;
	char szDiscMessage[Language::pLength::str];

	pop(nDiscSecond);
	pop(szDiscMessage);

	char msg[1024];
	sprintf(msg, "잠시 연결을 종료합니다.\n\n남은시간 : %02d시 %02d분 %02d초\n\n종료사유 : %s    ", nDiscSecond/3600, (nDiscSecond/60)%60, nDiscSecond%60, szDiscMessage);

	DoDisconnect();
#pragma todo("disc message 처리 필요")
	MessageBoxA(GAMEAPP_ST.GetHWnd(), msg, "Digimon Masters...", MB_OK);
	PostQuitMessage(0);
}

#ifdef CHAT_BAN
void cCliGame::RecvChatBanUser(void)
{
	u4 nBanSecond;
	std::wstring szBanMessage;	
	pop( nBanSecond );
	pop( szBanMessage );

	std::wstring banMsg;
	if( nBanSecond == 0 )	// 채팅 금지 해제
	{
		banMsg = UISTRING_TEXT("CHAT_UNLIMIT_SERVER_BAN_MESSAGE");
	}
	else// 채팅 금지
	{
		banMsg = UISTRING_TEXT("CHAT_LIMIT_SERVER_BAN_MESSAGE");
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
	}

	CsMessageBox( MB_OK, banMsg.c_str());

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_BANTIME;
	CProtocol.m_value= nBanSecond;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );	
	
// 
// 	u4 nBanSecond;
// 	char szBanMessage[Language::pLength::str];
// 	char msg[1024];
// 
// 	pop( nBanSecond );
// 	pop( szBanMessage );
// 
// 	if( nBanSecond == 0 )	// 채팅 금지 해제
// 		sprintf( msg, "채팅금지가 해제되었습니다." );
// 	else							// 채팅 금지
// 		sprintf( msg, "채팅이 금지 되었습니다.\n\n남은시간 : %02d시 %02d분 %02d초\n\n제한사유 : %s    ", nBanSecond/3600, (nBanSecond/60)%60, nBanSecond%60, szBanMessage );
// 
// 	MessageBoxA(GAMEAPP_ST.GetHWnd(), msg, "Digimon Masters...", MB_OK);
// 
// 	ST_CHAT_PROTOCOL	CProtocol;
// 	CProtocol.m_Type = NS_CHAT::SET_BANTIME;
// 	CProtocol.m_value= nBanSecond;
// 	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );	
}
#endif

void cCliGame::RecvBanUser(void)
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
// 	char szBanMessage[Language::pLength::str];
// 
// 	pop(nBanSecond);
// 	pop(szBanMessage);
// #ifdef VERSION_USA
// 	char msg[1024];
// 	sprintf(msg, "Limiting access to Game.\n\nTime : %02dHour %02dMin %02dSec    \n\nReason : %s    ", nBanSecond/3600, (nBanSecond/60)%60, nBanSecond%60, szBanMessage);
// 
// 	DoDisconnect();
// 	MessageBoxA( msg, "Digimon Masters...", MB_OK);
// 	PostQuitMessage(0);
// #else
// #if( defined VERSION_TW || defined VERSION_HK )
// 	TCHAR TempszBanMsg[Language::pLength::str] = {0, };
// 	M2W( TempszBanMsg, szBanMessage, strlen(szBanMessage) );
// 	CsMessageBox( MB_OK, L"現在連線已被限制\n\n剩餘時間 : %02d小時 %02d分 %02d秒    \n\n限制理由 : %s      ", nBanSecond/3600, (nBanSecond/60)%60, nBanSecond%60, TempszBanMsg );
// 
// 	DoDisconnect();
// 	PostQuitMessage(0);
// 
// #else
// 	char msg[1024];
// 	sprintf(msg, "접속 제한 되었습니다\n\n남은시간 : %02d시 %02d분 %02d초\n\n제한사유 : %s    ", nBanSecond/3600, (nBanSecond/60)%60, nBanSecond%60, szBanMessage);
// 
// 	DoDisconnect();
// 	MessageBoxA( msg, "Digimon Masters...", MB_OK);
// 	PostQuitMessage(0);
// #endif // ( defined VERSION_TW || defined VERSION_HK )
// #endif // VERSION_USA

}


void cCliGame::RecvInitSkillStat(void)
{
	u4 nRes = 0;
	pop(nRes);
	std::wstring wsTamerName;
	pop(wsTamerName);
	std::wstring wsDigimonName;
	pop(wsDigimonName);

	std::wstring wsMsg;
	DmCS::StringFn::Format( wsMsg, L"%s tn(%s) dn(%s) ", UISTRING_TEXT( "CHAT_SKILL_LEVEL_POINT_INITIALIZATION" ).c_str(), wsTamerName.c_str(), wsDigimonName.c_str() );
	switch(nRes)
	{
	case 0 :	// 실패 - 다시 시도
		wsMsg += UISTRING_TEXT( "CHAT_SKILL_LEVEL_POINT_INIT_ERROR" );
		break;
	case 1 :	// 성공
		wsMsg += UISTRING_TEXT( "CHAT_SKILL_LEVEL_POINT_INITIALIZATION_COMPLETE" );
		break;
	case 2 :	// 실패 - 해당 정보 오류
		wsMsg += UISTRING_TEXT( "CHAT_SKILL_LEVEL_POINT_INIT_ERROR_TAMER_NOT_EXIST" );
		break;
	case 3 :
		wsMsg += UISTRING_TEXT( "CHAT_SKILL_LEVEL_POINT_INIT_ERROR_ALREADY_COMPLETE" );
		break;
	case 4 :
		wsMsg += UISTRING_TEXT( "CHAT_SKILL_LEVEL_POINT_INIT_ERROR_NOT_LOADED" );
		break;
	case 5 :	// 실패 - 해당 정보 오류
		wsMsg += UISTRING_TEXT( "CHAT_SKILL_LEVEL_POINT_INIT_ERROR_DIGIMON_NOT_EXIST" );
		break;
	}

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = wsMsg;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cCliGame::SendQueryMoneyInven()
{
#ifdef _GIVE
	return;
#endif

	newp( pGame::QueryMoneyInven );
	endp( pGame::QueryMoneyInven );
	send();
}

void cCliGame::RecvQueryMoneyInven()
{
#ifdef _GIVE
	return;
#endif

	n8 Money = 0;
	pop( Money );
	u8 nCurMoney = g_pDataMng->GetInven()->GetMoney().GetInt();

	if( nCurMoney != (u8)Money )
	{
		CsMessageBox( MB_OK, _ERROR_MSG( "pGame::QueryMoneyInven :: Client/Server 돈틀림" ) );
		//CsMessageBox( MB_OK, _ERROR_MSG( "방금전 상황을 기억 하시고 송명수를 찾아주세요 - 돈틀림" ) );
		return;
	}

	// 인벤토리 정보 pop
	cItemInfo ServerInvenInfo;
	cData_Inven* pInven = g_pDataMng->GetInven();
	cItemInfo* pMyInvenInfo = NULL;
	for(int i=0; i<nLimit::Inven; ++i )
	{		
		pop(ServerInvenInfo);
		pMyInvenInfo = pInven->GetData( i );

		if( ServerInvenInfo.m_nAll != pMyInvenInfo->m_nAll )
		{
			CsMessageBox( MB_OK, _ERROR_MSG( "pGame::QueryMoneyInven :: Client/Server  - 아이템 틀림 : InvenSlot = %d" ), i );
			//CsMessageBox( MB_OK, _ERROR_MSG( "방금전 상황을 기억 하시고 송명수를 찾아주세요 - 아이템 틀림 : InvenSlot = %d" ), i );
		}
	}
}

void cCliGame::RecvKillMonster()
{
	u4 nTotalCount = 0;
	
	struct stDropInfo
	{
		u4 ItemType;
		u4 Count;
	};

	stDropInfo ItemDropInfo;
	memset(&ItemDropInfo, 0, sizeof(stDropInfo));

//	CsLog log;
//	log.Init( L"_KillMon", true, true );

	DmCS::Storage::File saveFile;
	saveFile.Create( L"_KillMon.txt", L"wt, ccs=UNICODE" );

	u4 money = 0;
	pop(nTotalCount);
	if( nTotalCount > 0 )
	{
		for(u4 i = 0; i < nTotalCount; i++)
		{
			pop(ItemDropInfo);
			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( ItemDropInfo.ItemType )->GetInfo();
			if( pFTInfo->s_nType_L == 87 )
			{
				money += ItemDropInfo.Count;
			}
			else
			{
				//std::wstring msg;
				//DmCS::StringFn::Format( msg, L"%d\t%s\t\t%d\n", pFTInfo->s_dwItemID, pFTInfo->s_szName, ItemDropInfo.Count );
				saveFile.WriteVA( L"%d\t%s\t\t%d\n", pFTInfo->s_dwItemID, pFTInfo->s_szName, ItemDropInfo.Count );
				//log.Log( L"%d\t%s\t\t%d", pFTInfo->s_dwItemID, pFTInfo->s_szName, ItemDropInfo.Count );
			}
		}
	}
	//std::wstring msg;
	//DmCS::StringFn::Format( msg, L"Money = %d", money );
	saveFile.WriteVA( L"Money = %d", money );
	//log.Log( L"Money = %d", money );
	saveFile.Close();
	ShellExecute( NULL, _T( "open" ), _T( "notepad.exe" ) , L"_KillMon.txt", NULL, SW_SHOW );
}

void cCliGame::RecvPlayTime()
{
	u4 nPlayTime = 0;

	pop(nPlayTime);	// 플레이 시간 변경됨
}

void cCliGame::RecvHatchTry()
{
	u4 nSuccCount[ 6 ] = {0, };
	u4 nFailCount[ 5 ] = {0, };
	u4 nBreakCount[ 5 ] = {0, };
	float fSuccAve[ 5 ] = {0.0f, };

	for( int i = 0; i < 6; ++i )
		pop( nSuccCount[ i ] );
	for( int i = 0; i < 5; ++i )
		pop( nFailCount[ i ] );
	for( int i = 0; i < 5; ++i )
		pop( nBreakCount[ i ] );
	for( int i = 0; i < 5; ++i )
		pop( fSuccAve[ i ] );

// 	CsLog log;
// 	log.Init( L"_HatchTry", true, true );
// 
// 	log.Log( L"SuccCount   LV0 = %d, LV1 = %d, LV2 = %d, LV3 = %d, LV4 = %d, LV5 = %d", nSuccCount[0], nSuccCount[1], nSuccCount[2], nSuccCount[3], nSuccCount[4], nSuccCount[5] );
// 	log.Log( L"FailCount   LV0 = %d, LV1 = %d, LV2 = %d, LV3 = %d, LV4 = %d", nFailCount[0], nFailCount[1], nFailCount[2], nFailCount[3], nFailCount[4] );
// 	log.Log( L"BreakCount  LV0 = %d, LV1 = %d, LV2 = %d, LV3 = %d, LV4 = %d", nBreakCount[0], nBreakCount[1], nBreakCount[2], nBreakCount[3], nBreakCount[4] );
// 	log.Log( L"SuccAverage LV0 = %.2f, LV1 = %.2f, LV2 = %.2f, LV3 = %.2f, LV4 = %.2f", fSuccAve[0], fSuccAve[1], fSuccAve[2], fSuccAve[3], fSuccAve[4] );

	DmCS::Storage::File saveFile;
	saveFile.Create( L"_HatchTry.txt", L"wt, ccs=UNICODE" );
	saveFile.WriteVA( L"SuccCount   LV0 = %d, LV1 = %d, LV2 = %d, LV3 = %d, LV4 = %d, LV5 = %d\n", nSuccCount[0], nSuccCount[1], nSuccCount[2], nSuccCount[3], nSuccCount[4], nSuccCount[5] );
	saveFile.WriteVA( L"FailCount   LV0 = %d, LV1 = %d, LV2 = %d, LV3 = %d, LV4 = %d\n", nFailCount[0], nFailCount[1], nFailCount[2], nFailCount[3], nFailCount[4] );
	saveFile.WriteVA( L"BreakCount  LV0 = %d, LV1 = %d, LV2 = %d, LV3 = %d, LV4 = %d\n", nBreakCount[0], nBreakCount[1], nBreakCount[2], nBreakCount[3], nBreakCount[4] );
	saveFile.WriteVA( L"SuccAverage LV0 = %.2f, LV1 = %.2f, LV2 = %.2f, LV3 = %.2f, LV4 = %.2f\n", fSuccAve[0], fSuccAve[1], fSuccAve[2], fSuccAve[3], fSuccAve[4] );

	ShellExecute( NULL, _T( "open" ), _T( "notepad.exe" ), L"_HatchTry", NULL, SW_SHOW );
}

void cCliGame::RecvMasterConnCount()
{
	u4 nConnCount = 0;
	pop(nConnCount);

#ifdef VERSION_USA
	if( g_pGameIF )
	{
		TCHAR sz[ 256 ];
		_stprintf_s( sz, 256, L"Connect = %d", nConnCount );
		cPrintMsg::PrintMsg( 12000, sz );
	}
#else
	u4 nPlayNetCount = 0;
	pop(nPlayNetCount);
	u4 nTooniLandCount = 0;
	pop(nTooniLandCount);
	if( g_pGameIF )
	{
		TCHAR sz[ 256 ];
		_stprintf_s( sz, 256, L"[Connect = %04d] [PlayNet = %03d] [TooniLand = %03d]", nConnCount,nPlayNetCount,nTooniLandCount );
		cPrintMsg::PrintMsg( 12000, sz );
	}
#endif
}

void cCliGame::RecvMasterPayCash()
{
	n8 nPrimiumCash = 0;
	n8 nNormalCash = 0;

	pop(nPrimiumCash);
	pop(nNormalCash);

	if( g_pGameIF )
	{
		TCHAR sz[ 256 ];
		_stprintf_s( sz, 256, L"[ Prem: %d ]   [ Silk: %d ]", nPrimiumCash, nNormalCash );
		cPrintMsg::PrintMsg( 12000, sz );
	}
}

void cCliGame::RecvGmQuest()
{	// 전체 퀘스트 재 수신
	pop( *g_pDataMng->GetQuest()->GetInfo() );

	g_pDataMng->GetQuest()->PostRevPostLoad( net::next_map_no );
	g_pGameIF->GetQuestHelper()->ResetHelperString();
	if( g_pGameIF->GetQuestList() )
	{
		g_pGameIF->GetQuestList()->ResetQuestList( false );	
	}

	if( g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Exist() )
		g_pDataMng->GetQuestOwner()->IF_DigiviceQuest_Open();
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
	CProtocol.m_wStr = _T( "@<tc:900>Change QUEST state compleat!@</tc>" );
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cCliGame::RecvGMCoolReSet()
{	// 모든 쿨타임 초기화
	u4 nCoolDown[nLimit::CoolDown] = {0,};
	pop( nCoolDown );

	g_pCharMng->GetDigimonUser(0)->ResetSkillCool();	//2017-06-28-nova 디지몬 스킬 쿨타임 리셋
	g_pCharMng->GetTamerUser()->ResetSkillCool();		//2017-06-28-nova 테이머 스킬 쿨타임 리셋

	for( int i=0; i<nLimit::CoolDown; ++i )
	{
		if( nsCsFileTable::g_pItemMng->IsCoolTime( i+1 ) == false )
			continue;

		CsCoolTimeSeq* pSeq = nsCsFileTable::g_pItemMng->GetCoolTime( i+1 )->GetSeq();
		if( nCoolDown[ i ] < _TIME_TS + 1 )
		{
			pSeq->Reset();
			continue;
		}

		double fOffset = pSeq->GetEndTime() - ( nCoolDown[ i ] - _TIME_TS ) - 1;
		pSeq->Start( fOffset );
	}
}

void cCliGame::RecvGuildSkillReSet()
{
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_GUILD_SKILL_RESET, NULL );
}

void cCliGame::RecvRunGotchaMachine()
{
	u4 nTotalCnt = 10;
	pop(nTotalCnt);//아이템 총 갯수

	if( nTotalCnt == 0 )//확인할 수 없음, Npc번호가 없던가 뭐 잘못입력함
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = UISTRING_TEXT( "CHAT_RAREMACHINE_CHEAT_WRONG_INPUT_DATA" );
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}
	struct sGET_INFO	//아이템 몇회 나왔는지 보여줄거야
	{
		u4 nItemID;						//아이템번호
		TCHAR szItemName[ ITEM_NAME_LEN ];	//아이템 이름
		u4 nItemCnt;					//획득 나온 횟수
	};
	std::map < int, sGET_INFO* >			m_mapInfo;		//맵

// 	CsLog log;	//메모장에 넣을거야
// 	log.Init( L"Cheat_RareMachine", true, true );

	DmCS::Storage::File saveFile;
	saveFile.Create( L"Cheat_RareMachine.txt", L"wt, ccs=UNICODE" );

	if( nTotalCnt > 0 )
	{
		u4 nItemNo = 21;
		for(u4 i = 0; i < nTotalCnt; i++)
		{
			sGET_INFO* sInfo2 = new sGET_INFO;

			pop( nItemNo );
			if( nsCsFileTable::g_pItemMng->IsItem( nItemNo ) == false )
			{
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
				CProtocol.m_wStr = UISTRING_TEXT( "CHAT_RAREMACHINE_CHEAT_WRONG_RECV_DATA" );
				DmCS::StringFn::Replace( CProtocol.m_wStr, L"#Item#", nItemNo );
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
				return;
			}
			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( nItemNo )->GetInfo();

			std::map < int, sGET_INFO* >::iterator itMap = m_mapInfo.find( nItemNo );

			cStringList* pStrList = NiNew cStringList;
			cText::sTEXTINFO ti;
			ti.Init();

			if( itMap != m_mapInfo.end() )
				sInfo2->nItemCnt = itMap->second->nItemCnt + 1;
			else
				sInfo2->nItemCnt = 1;

			sInfo2->nItemID = nItemNo;
			_tcscpy_s( sInfo2->szItemName, ITEM_NAME_LEN, pFTInfo->s_szName );

			m_mapInfo[nItemNo] = sInfo2;
		}

		//메모장에 쓸 내용
		//log.Log( L"[아이템번호]\t[획득횟수]\t[확률]\t\t\t[아이템이름]" );
		saveFile.Write( L"[아이템번호]	[획득횟수]	[확률]			[아이템이름]" );

		std::map < int, sGET_INFO* >::iterator it = m_mapInfo.begin();
		std::map < int, sGET_INFO* >::iterator itEnd = m_mapInfo.end();

		for( ; it != itEnd ; ++it)
		{
			if( it->second->nItemID == 0 )
				break;
			int nCnt = it->second->nItemCnt;//갯수
			float fRate = (float)nCnt / (float)nTotalCnt ;//확률
			//log.Log( L" %d\t\t    %d\t\t %.2f%%\t\t %s", it->second->nItemID, it->second->nItemCnt, fRate*100.0f, it->second->szItemName );
			saveFile.WriteVA( L" %d\t\t    %d\t\t %.2f%%\t\t %s", it->second->nItemID, it->second->nItemCnt, fRate*100.0f, it->second->szItemName );
		}
		//log.Log( L"\n\n%s : %d", _T("[총 횟수]"), nTotalCnt );
		saveFile.WriteVA( L"\n\n[총 횟수] : %d", nTotalCnt );
	}
	ShellExecute( NULL, _T( "open" ), _T( "notepad.exe" ), L"Cheat_RareMachine.txt", NULL, SW_SHOW );
}

#ifdef CHEAT_ITEMRANK
void cCliGame::RecvItemRank( )
{
	u2 nCount	= 0;
	pop(nCount);

	if( nCount == 0 )//확인할 수 없음
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = UISTRING_TEXT( "CHAT_ITEMRANK_CHEAT_WRONG_INPUT_DATA" );
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	n4 nTotalCnt = 13;
	pop(nTotalCnt);//아이템 총 갯수

	struct sGET_INFO	//아이템 몇회 나왔는지 보여줄거야
	{
		u4 nItemID;						//아이템번호
		TCHAR szItemName[ ITEM_NAME_LEN ];	//아이템 이름
		u4 nItemCnt;					//획득 나온 횟수
	};
	std::map < int, sGET_INFO* >			m_mapInfo;		//맵

// 	CsLog log;	//메모장에 넣을거야
// 	log.Init( L"Cheat_ItemRank", true, true );

	DmCS::Storage::File saveFile;
	saveFile.Create( L"Cheat_ItemRank.txt", L"wt, ccs=UNICODE" );

	if( nTotalCnt > 0 )
	{
		u4 nItemNo = 21;
		for(int i = 0; i < nTotalCnt; i++)
		{
			sGET_INFO* sInfo2 = new sGET_INFO;
			pop( nItemNo );
			if( nsCsFileTable::g_pItemMng->IsItem( nItemNo ) == false )
			{
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
				CProtocol.m_wStr = UISTRING_TEXT( "CHAT_CHEAT_MESSAGE_ITEMRANK_ERROR" );
				DmCS::StringFn::Replace( CProtocol.m_wStr, L"#Item#", nItemNo);
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
				return;
			}
			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( nItemNo )->GetInfo();

			std::map < int, sGET_INFO* >::iterator itMap = m_mapInfo.find( nItemNo );

			cStringList* pStrList = NiNew cStringList;
			cText::sTEXTINFO ti;
			ti.Init();

			if( itMap != m_mapInfo.end() )
				sInfo2->nItemCnt = itMap->second->nItemCnt + 1;
			else
				sInfo2->nItemCnt = 1;

			sInfo2->nItemID = nItemNo;
			// 			g_pStringAnalysis->Cut_LimitWigth( pStrList, 170, pFTInfo->s_szName, &ti );
			// 			cString::sELEMENT* pEle = pStrList->GetString( 0 )->GetElement( 0 );
			_tcscpy_s( sInfo2->szItemName, ITEM_NAME_LEN, pFTInfo->s_szName/* ( (cString::sTEXT*)pEle )->s_Text.GetText()*/ );

			m_mapInfo[nItemNo] = sInfo2;
		}

		//메모장에 쓸 내용
		//log.Log( L"[아이템번호]\t[획득횟수]\t[확률]\t\t\t[아이템이름]" );
		saveFile.Write( L"[아이템번호]	[획득횟수]	[확률]			[아이템이름]" );

		std::map < int, sGET_INFO* >::iterator it = m_mapInfo.begin();
		std::map < int, sGET_INFO* >::iterator itEnd = m_mapInfo.end();

		for( ; it != itEnd ; ++it)
		{
			if( it->second->nItemID == 0 )
				break;
			int nCnt = it->second->nItemCnt;//갯수
			float fRate = (float)nCnt / (float)nTotalCnt ;//확률

			//log.Log( L" %d\t\t    %d\t\t %.2f%%\t\t %s", it->second->nItemID, it->second->nItemCnt, fRate*100.0f, it->second->szItemName );
			saveFile.WriteVA( L" %d\t\t    %d\t\t %.2f%%\t\t %s", it->second->nItemID, it->second->nItemCnt, fRate*100.0f, it->second->szItemName );
		}
		//log.Log( L"\n\n%s : %d", _T("[총 횟수]"), nCount );
		saveFile.WriteVA( L"\n\n[총 횟수] : %d", nCount );
	}
	ShellExecute( NULL, _T( "open" ), _T( "notepad.exe" ), L"Cheat_ItemRank.txt", NULL, SW_SHOW );
}
#endif

// void cCliGame::SendBattleCheat(char *cmd)
// {
// 	char szTamerName[Language::pLength::name] = {0,};
// 	cToken t;
// 	t.init(cmd," ");
// 	t.next(szTamerName);
// 
// 	if( strlen( szTamerName ) == 0 )
// 	{
// 		if( g_pCharResMng->GetTargetMark()->GetLeafRTTI() == RTTI_TAMER )
// 		{
// 			W2M( szTamerName, g_pCharResMng->GetTargetMark()->GetName(), Language::pLength::name + 1 ); 		
// 		}
// 		else
// 		{
// 			return;
// 		}
// 	}
// 
// 	SendInsMap_Ready( szTamerName );
// }