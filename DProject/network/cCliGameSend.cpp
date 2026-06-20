
#include "stdafx.h"

#include "nlib/base.h"

#include "cCliGame.h"
#include "cNetwork.h"

#include "common_vs2019/pPass2.h"
#include "nlib/cSHA1.h"

#include "common_vs2019/pServer.h."
#include "common_vs2019/pSession.h"

#include "common_vs2019/pSkill.h"
#include "common_vs2019/pItem.h"
#include "common_vs2019/pGame.h"
#include "common_vs2019/pMaster.h"

#include "nlib/md5.h"

#include "common_vs2019/pTrade.h"
#include "common_vs2019/pScanner.h"
#include "common_vs2019/pmonster.h"
#include "common_vs2019/cVersion.h"
#include "common_vs2019/pInsMap.h"

#include "../Flow/Flow.h"
#include "../Flow/FlowMgr.h"

//extern uint g_nNetVersion;
	
void cCliGame::SendAccessCode(void)
{
	xassert(net::access_code, "access_code is null");
	xassert(net::account_idx, "account_idx is null");

	//LOG("(%s) acnt_idx:%d code:%d", m_szMapName, net::account_idx, net::access_code);

	newp(pSvr::AccessCode);
		push((uint)net::game->port^net::access_code|net::account_idx);
		push(net::account_idx);
		push(net::access_code);
		push(g_nNetVersion);
	endp(pSvr::AccessCode);
	send();
}

void cCliGame::SendChannelInfo(void)
{
	newp(pSvr::ChannelInfo);
	endp(pSvr::ChannelInfo);
	send();
}


void cCliGame::SendAwayTime(void)
{
	newp(pGame::AwayTime);
	endp(pGame::AwayTime);
	send();
}


void cCliGame::SendEmoticon(int nType, int nVal)
{
#pragma todo("이모티콘 송신")
	newp(pGame::Emoticon);
		push(nType);
		push(nVal);
	endp(pGame::Emoticon);
	send();
}

// Ao usar portais regulares e portais dentro da zona
void cCliGame::SendSelectPortal(uint nPortalNo)
{
	if( 0 == nPortalNo )
		return;
	
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	if( g_pResist->IsEnablePortal() == false )
		return;
	
	g_pResist->SetMovePortal(true);	//2017-04-12-nova

	if(nsCsGBTerrain::g_nSvrLibType !=  nLIB::SVR_DUNGEON)
		g_pResist->SetEnablePortal( false );		

	if(m_bPortalRequesting)
		return;

	m_bPortalRequesting = true;

	newp(pSvr::SelectPortal);
		// todo:: nPortalNo 값은 encoding되어야 할듯...
		push(nPortalNo);
	endp(pSvr::SelectPortal);
	send();
	
	cClient::SetSendBlock(true);
#ifdef KEYBOARD_MOVE
	g_pCharMng->GetTamerUser()->SetFollowPortalMove( false );
#endif
}


#include "nlib/cLock.h"
extern cLock g_StartLock;

void cCliGame::SendChangeServer(void)
{
	bool bSendBlock = cClient::IsSendBlock();
	cClient::SetSendBlock(false);

	// game server -> game server
	newp(pSvr::Change);
	endp(pSvr::Change);
	send();

	cClient::SetSendBlock(bSendBlock);
}


void cCliGame::SendGoBackGate(void)
{
	assert_cs(false);

	if(m_bPortalRequesting)
	{
		return;
	}

	newp(pSvr::GoBackGate);
	endp(pSvr::GoBackGate);
	send();

	//Sleep(333);
	g_pMngCollector->ResetMap();
	//SetNextGameStep(GAME_CHARACTER_SELECT);
	
	strcpy(net::ip, net::gate->ip);
	net::port = net::gate->port;

	SendChangeServer();

	FLOWMGR_ST.ChangePopAllFlow( Flow::CFlow::FLW_CHARSEL );
}


void cCliGame::SendReady(nSync::Pos &DigimonStartPos)
{
#ifndef _GIVE
	LOG("SendReady(%d,%d)", DigimonStartPos.m_nX, DigimonStartPos.m_nY);
#endif //_GIVE

	// loading 이 완료 되었는지

	newp(pGame::Ready);
	push(DigimonStartPos);
//#ifdef VERSION_USA	// 게임서버에서도 버전 체크 추가 chu8820
//	int nGameSvr_Version = NET_VERSION_GAME;
//	push( nGameSvr_Version );
//#endif
	push( g_nNetVersion );
	endp(pGame::Ready);
	send();
	m_bIsReadyCompleted = true;
}


void cCliGame::SendUserMessageConfirm(void)
{
	newp(pSession::MessageConfirm);
	endp(pSession::MessageConfirm);
	send();
}


void cCliGame::RecvTamerMessageConfirm(void)
{
	uint nResult = 0;
	pop( nResult );

	switch(nResult)
	{
	case 1:		// PC방 이용 기간이 만료되어 해당 메시지가 폐기 되었습니다.
		cPrintMsg::PrintMsg( 20203 );
		return;
	case 100:	// 정상 처리
		break;
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_CASH_WAREHOUSE ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_NEW_CASH_WAREHOUSE );	
}


void cCliGame::SendTamerMessageConfirm(void)
{
	newp(pTamer::MessageConfirm);
	endp(pTamer::MessageConfirm);
	send();
}


void cCliGame::SendCardScan(wchar* szCode, nSync::Pos &pos)
{
	if(nBase::strlen(szCode) != nScanner::CardLength)
	{
		__asm int 3;
		return;
	}

	newp(pScanner::Read);
		push( szCode );
		push(pos);
	endp(pScanner::Read);
	DoSend();
}


bool cCliGame::SendScannerEquip(wchar* szCode)
{
	const char *buf = nBase::w2m(szCode).c_str();

	for(int i=0; i<nScanner::CodeLength; i++)
	{
		if(buf[i] >= '0' && buf[i] <= '9')
		{
			continue;
		}
		if(buf[i] >= 'A' && buf[i] <= 'Z')
		{
			continue;
		}
		if(buf[i] >= 'a' && buf[i] <= 'z')
		{
			continue;
		}
		return false;
	}

	newp(pScanner::Equip);
		push(buf);
	endp(pScanner::Equip);
	DoSend();

	return true;
}


void cCliGame::SendScannerOn(wchar *szCode)
{
	newp(pScanner::On);
	push(szCode);
	endp(pScanner::On);
	DoSend();

}


void cCliGame::SendScannerOff(void)
{
	newp(pScanner::Off);
	endp(pScanner::Off);

	DoSend();
}


void cCliGame::SendScannerRegister(wchar *szCode)
{
	// RecvInitGameData 이후 아무때나 호출 가능
	if(nBase::strlen(szCode) != nScanner::CodeLength)
	{
		__asm int 3;
		return;
	}

	newp(pScanner::Register);
		push(szCode);
	endp(pScanner::Register);
	send();
}

void cCliGame::SendScannerRelease(wchar *szCode)
{
	// RecvInitGameData 이후 아무때나 호출 가능

	if(nBase::strlen(szCode) != nScanner::CodeLength)
	{
		__asm int 3;
		return;
	}

	newp(pScanner::Release);
		push(szCode);
	endp(pScanner::Release);
	send();
}


void cCliGame::SendChangeTamerName(int nInvenSlotNo, wchar const* name)
{
	newp(pTamer::ChangeName);
		push(nInvenSlotNo);	// 테이머 이름 변경 아이템 위치
		push( name );
	endp(pTamer::ChangeName);
	send();
}

void cCliGame::SendChangeDigimonName(int nInvenSlotNo, wchar const* name)
{
	newp(pDigimon::ChangeName);
		push(nInvenSlotNo);	// 디지몬 이름 변경 아이템 위치
		push( name );
	endp(pDigimon::ChangeName);
	send();
}

void cCliGame::SendRClickDigimonHatch( int nType, int nInvenSlotNo, wchar const* name )
{
	newp(pItem::DirectHatch);
		push(nType);				// 아이템 번호
		push(nInvenSlotNo);			// 아이템의 인벤 위치
		push( name );	// 생성된 디지몬 이름
	endp(pItem::DirectHatch);
	send();
}

void cCliGame::SendOpenRegion(int nOpenRegionIDX)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	xstop(nOpenRegionIDX<nLimit::Region, "nOpenRegionIDX값은 0~14 이어야만한다.");

	newp(pGame::OpenRegion);
		push((char)nOpenRegionIDX);
	endp(pGame::OpenRegion);
	send();
}


void cCliGame::SendMsgAllByItem(wchar *msg, n4 nItemSlotNo)	// 현재 서버 전체에 송신(루체섭만, 리바섭만...)
{
	nsCsFileTable::g_pCuidMng->CheckChat( msg );	

	newp(pGame::MsgAllByItem);
		push( msg );
		push(nItemSlotNo);
	endp(pGame::MsgAllByItem);
	send();	

	cItemInfo* pItme = g_pDataMng->GetInven()->GetData( nItemSlotNo );	
	int nType = nsCsFileTable::g_pItemMng->GetItem( pItme->GetType() )->GetInfo()->s_nType_L;
	switch( nType )
	{
	case nItem::Cash_Shouter_N:
	case nItem::Cash_Shouter_S:
		pItme->DecreaseCount( 1 );
		break;
	case nItem::Cash_Shouter_NT:
	case nItem::Cash_Shouter_ST:
	case nItem::GM_Shouter_T:
		{						
			CsItem::sINFO* pFTInfo = nsCsFileTable::g_pItemMng->GetItem( pItme->GetType() )->GetInfo();		
			if( pFTInfo->s_nUseTimeGroup != 0 )
			{
				// 쿨타임 적용
				pFTInfo->GetCoolTimeSeq()->Start();
			}
		}
		break;
	}	
}

void cCliGame::SendMsgShout( wchar const* msg )
{
	newp(pGame::MsgShout);
		push( msg );
	endp(pGame::MsgShout);
	send();
}

void cCliGame::SendMsgWhisper( TCHAR const* name, TCHAR const* msg )
{
	SAFE_POINTER_RET( name );
	SAFE_POINTER_RET( msg );

	std::wstring whisperTamerName(name);
	if( Language::SUCCEEDED != Language::CheckTamerName( whisperTamerName ) )
	{
		cPrintMsg::PrintMsg( 11101 );
		return;
	}

	newp(pGame::Whisper);
		push( whisperTamerName );
		push( msg );
	endp(pGame::Whisper);
	send();
}


static int s_nCnt = 0;
static u4 s_nUID = 0;
static int s_nPX = 0, s_nPY = 0;

void cCliGame::SendMoveTo(u4 uid, nSync::Pos sp, nSync::Pos tp, float fDirect)
{
	SendMoveTo(uid, tp, fDirect);
	return;

	if(m_bPortalRequesting)
	{
		return;
	}

	if(s_nUID==uid && s_nPX==tp.m_nX && s_nPY==tp.m_nY)
	{
		return;
	}
	s_nUID = uid;
	s_nPX = tp.m_nX;
	s_nPY = tp.m_nY;

	
// 암호화 처리 필요

	newp(pGame::NewPath);
		push((u4)GetTickCount()-m_nConnectedTickCount);
		push(uid);
		push(sp);
		push(tp);
		push(fDirect);
	endp(pGame::NewPath);
	send();

//	net::chat("MoveTo4 Digimon(%d,%d) %d", tx, ty, s_nCnt++);
}

void cCliGame::SendMoveTo(u4 uid, nSync::Pos tp, float fDirect)
{
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_EVENT )
		return;

	if(m_bPortalRequesting)
	{
		return;
	}

	if(s_nUID==uid && s_nPX==tp.m_nX && s_nPY==tp.m_nY)
	{
		return;
	}
	s_nUID = uid;
	s_nPX = tp.m_nX;
	s_nPY = tp.m_nY;

// 암호화 처리 필요
	newp(pGame::MoveTo);
		push((u4)GetTickCount()-m_nConnectedTickCount);
		push(uid);
		push(tp);
		push(fDirect);
	endp(pGame::MoveTo);
	send();

//	net::chat("MoveTo2 Digimon(%d,%d) %d", tx, ty, s_nCnt++);
}

void cCliGame::SendMoveToKnockBack( u4 uid, nSync::Pos tp )
{
	if(m_bPortalRequesting)
	{
		return;
	}

	s_nUID = uid;
	s_nPX = tp.m_nX;
	s_nPY = tp.m_nY;

	newp(pGame::KnockBack);
	push((u4)GetTickCount()-m_nConnectedTickCount);
	push(uid);
	push(tp);
	endp(pGame::KnockBack);
	send();

	//	net::chat("MoveTo2 Digimon(%d,%d) %d", tx, ty, s_nCnt++);
}

void cCliGame::SendChatMsg(wchar_t* msg)
{
	static DWORD dwSendChatTime = GetTickCount();
	static std::wstring szOld;

	std::wstring wmsg(msg);
	size_t nLen = wmsg.length();
	DWORD dwCurTime = GetTickCount();

	if ((wmsg == szOld) && ((dwCurTime - dwSendChatTime) < 3000))
	{
		ST_CHAT_PROTOCOL CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		CProtocol.m_wStr = UISTRING_TEXT("CHAT_FAST_TIME_SAME_TEXT");
		GAME_EVENT_STPTR->OnEvent(EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol);
		return;
	}

	dwSendChatTime = dwCurTime;
	szOld = wmsg;

	xassert(wmsg.length() < Language::pLength::str, "msg is not valid...");

	// Converting wchar_t to char* using wcstombs
	char new_Msg[LIMIT_IME_LENGTH] = { 0, };
	wcstombs(new_Msg, msg, LIMIT_IME_LENGTH);

	newp(pGame::Chat);
	push(new_Msg);
	endp(pGame::Chat);
	send();

}

void cCliGame::SendJump( int x, int y )
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	if(m_bPortalRequesting)
	{
		return;
	}

	nSync::Pos dp = nSync::Pos( x, y );

	if(nsCsGBTerrain::g_pCurRoot->IsValidEmr2(dp.m_nX, dp.m_nY))
	{
		newp(pMaster::Jump);
		push(dp);
		endp(pMaster::Jump);
		send();
	}
	else
	{
		//CHAT_TEXT( _LAN( "이동 불가능 지역입니다." ) );
	}
}

void cCliGame::SendItemSplit(n2 nSrcPos, n2 nDstPos, u2 nCount)
{
	if(m_bPortalRequesting)
	{
		g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
		return;
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
	{
		cPrintMsg::PrintMsg( 30028 );
		g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
		return;
	}
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
	{
		cPrintMsg::PrintMsg( 30357 );
		g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
		return;
	}

#ifdef CROSSWARS_SYSTEM
#pragma warning( disable : 4806 ) // bool == 3과 같은 코드에 대해 경고, 사용할 수는 있지만 true가 될 수 없음.
	// 크로스에서 인밴이외는 fail
	if( ( TO_CONSTANT( nSrcPos ) == SERVER_DATA_INVENCROSS_CONSTANT )&&
		(TO_CONSTANT( nDstPos ) != SERVER_DATA_INVEN_CONSTANT && 
		!TO_CONSTANT( nDstPos ) == SERVER_DATA_INVENCROSS_CONSTANT) )
	{
		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcPos );
		CsItem::sINFO* pSrcFT1 = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

		if( nItem::CrossWars == pSrcFT1->s_nType_L )
		{			
			g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
			return;
		}


	}
	else if(  ( TO_CONSTANT( nSrcPos ) == SERVER_DATA_INVENCROSS_CONSTANT )&&		
		(TO_CONSTANT( nDstPos ) == SERVER_DATA_INVEN_CONSTANT) )
	{		
		cItemInfo* pDest = g_pDataMng->SrvID2ItemInfo( nDstPos );
		if(NULL != pDest)
		{
			CsItem::sINFO* pDestFT1 = nsCsFileTable::g_pItemMng->GetItem( pDest->GetType() )->GetInfo();
			if( nItem::CrossWars != pDestFT1->s_nType_L )
			{			
				g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
				return;
			}
		}
	}


	// 인밴에서 크로스 이외에는 fail
	if( ( TO_CONSTANT( nSrcPos ) == SERVER_DATA_INVEN_CONSTANT )&&
		( TO_CONSTANT( nDstPos ) != SERVER_DATA_INVENCROSS_CONSTANT && 
		( !TO_CONSTANT( nDstPos ) == SERVER_DATA_INVEN_CONSTANT)) )
	{

		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcPos );
		CsItem::sINFO* pSrcFT1 = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

		if( nItem::CrossWars == pSrcFT1->s_nType_L )
		{			
			g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
			return;
		}
	}
	else if(  ( TO_CONSTANT( nSrcPos ) == SERVER_DATA_INVEN_CONSTANT )&&		
		(TO_CONSTANT( nDstPos ) == SERVER_DATA_INVENCROSS_CONSTANT) )
	{
		cItemInfo* pDest = g_pDataMng->SrvID2ItemInfo( nDstPos );
		if(NULL != pDest)
		{
			CsItem::sINFO* pDestFT1 = nsCsFileTable::g_pItemMng->GetItem( pDest->GetType() )->GetInfo();
			if( nItem::CrossWars != pDestFT1->s_nType_L )
			{			
				g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
				return;
			}
		}
	}

	/*
	// 크로스에서 아무대나.
	if( ( TO_CONSTANT( nSrcPos ) == SERVER_DATA_INVENCROSS_CONSTANT )&&
		( TO_CONSTANT( nDstPos ) != SERVER_DATA_INVENCROSS_CONSTANT ) )
	{
		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcPos );
		CsItem::sINFO* pSrcFT1 = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

		if( nItem::CrossWars == pSrcFT1->s_nType_L )
		{			
			g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
			return;
		}		
	}
	else if( ( TO_CONSTANT( nSrcPos ) != SERVER_DATA_INVENCROSS_CONSTANT )&&
		( TO_CONSTANT( nDstPos ) == SERVER_DATA_INVENCROSS_CONSTANT ) )
	{
		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcPos );
		CsItem::sINFO* pSrcFT1 = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

		if( nItem::CrossWars != pSrcFT1->s_nType_L )
		{			
			g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
			return;
		}		
	}
	*/
#pragma warning( default : 4806 )
#endif

#ifdef NEW_SHARESTASH
	// 인벤에서 계정 창고로
	if( ( TO_CONSTANT( nSrcPos ) == SERVER_DATA_INVEN_CONSTANT )&&
		( TO_CONSTANT( nDstPos ) == SERVER_DATA_SHARESTASH_CONSTANT ) )
	{
		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcPos );	// SrvID2ItemInfo함수로 아이템 정보를 얻는다.
		CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
		if( (pSrc && ( pSrc->m_nLimited != 0 )) || pSrcFT->s_nBelonging != 0 )
		{				

			cPrintMsg::PrintMsg( 30501 );
			g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
			return;

		}
	}
	//	창고에서 계정 창고로
	else if( ( TO_CONSTANT( nSrcPos ) == SERVER_DATA_WAREHOUSE_CONSTANT )&&
		( TO_CONSTANT( nDstPos ) == SERVER_DATA_SHARESTASH_CONSTANT ) )
	{
		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcPos );	// SrvID2ItemInfo함수로 아이템 정보를 얻는다.
		CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
		if( (pSrc && ( pSrc->m_nLimited != 0 )) || pSrcFT->s_nBelonging != 0 )
		{				

			cPrintMsg::PrintMsg( 30501 );
			g_pDataMng->ServerItemMoveFailed( nSrcPos, nDstPos );
			return;

		}
	}
#endif

	newp(pItem::Split);
		push(nSrcPos);
		push(nDstPos);
		push(nCount);
	endp(pItem::Split);
	send();
}

void cCliGame::SendItemMove(short nSrcItemPos, short nDstItemPos, bool bCheckBelonging /*=true*/ )
{
	if(m_bPortalRequesting)
	{
		g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
		return;
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
	{
		cPrintMsg::PrintMsg( 30028 );
		g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
		return;
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
	{
		cPrintMsg::PrintMsg( 30357 );
		g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
		return;
	}	
#ifdef CROSSWARS_SYSTEM
#pragma warning( disable : 4806 ) // bool == 3과 같은 코드에 대해 경고, 사용할 수는 있지만 true가 될 수 없음.
	// 크로스에서 인밴이외는 fail
	if( ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVENCROSS_CONSTANT )&&
		(TO_CONSTANT( nDstItemPos ) != SERVER_DATA_INVEN_CONSTANT && 
		!TO_CONSTANT( nDstItemPos ) == SERVER_DATA_INVENCROSS_CONSTANT) )
	{
		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );
		CsItem::sINFO* pSrcFT1 = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

		if( nItem::CrossWars == pSrcFT1->s_nType_L )
		{			
			g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
			return;
		}
		

	}
	else if(  ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVENCROSS_CONSTANT )&&		
		 (TO_CONSTANT( nDstItemPos ) == SERVER_DATA_INVEN_CONSTANT) )
	{		
		cItemInfo* pDest = g_pDataMng->SrvID2ItemInfo( nDstItemPos );
		if(NULL != pDest)
		{
			CsItem::sINFO* pDestFT1 = nsCsFileTable::g_pItemMng->GetItem( pDest->GetType() )->GetInfo();
			if( nItem::CrossWars != pDestFT1->s_nType_L )
			{			
				g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
				return;
			}
		}
	}


	// 인밴에서 크로스 이외에는 fail
	if( ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT )&&
		( TO_CONSTANT( nDstItemPos ) != SERVER_DATA_INVENCROSS_CONSTANT && 
		( !TO_CONSTANT( nDstItemPos ) == SERVER_DATA_INVEN_CONSTANT)) )
	{
		
		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );
		CsItem::sINFO* pSrcFT1 = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

		if( nItem::CrossWars == pSrcFT1->s_nType_L )
		{			
			g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
			return;
		}
	}
	else if(  ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT )&&		
		(TO_CONSTANT( nDstItemPos ) == SERVER_DATA_INVENCROSS_CONSTANT) )
	{
		cItemInfo* pDest = g_pDataMng->SrvID2ItemInfo( nDstItemPos );
		if(NULL != pDest)
		{
			CsItem::sINFO* pDestFT1 = nsCsFileTable::g_pItemMng->GetItem( pDest->GetType() )->GetInfo();
			if( nItem::CrossWars != pDestFT1->s_nType_L )
			{			
				g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
				return;
			}
		}
	}

/*
	// 크로스에서 아무대나.
	if( ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVENCROSS_CONSTANT )&&
		( TO_CONSTANT( nDstItemPos ) != SERVER_DATA_INVENCROSS_CONSTANT ) )
	{
		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );
		CsItem::sINFO* pSrcFT1 = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

		if( nItem::CrossWars == pSrcFT1->s_nType_L )
		{			
			g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
			return;
		}		
	}
	else if( ( TO_CONSTANT( nSrcItemPos ) != SERVER_DATA_INVENCROSS_CONSTANT )&&
		( TO_CONSTANT( nDstItemPos ) == SERVER_DATA_INVENCROSS_CONSTANT ) )
	{
		cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );


		CsItem::sINFO* pSrcFT1 = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
		if( nItem::CrossWars != pSrcFT1->s_nType_L )
		{
			g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
			return;
		}
		cItemInfo* pDest = g_pDataMng->SrvID2ItemInfo( nDstItemPos );
		if(NULL != pDest){
			CsItem::sINFO* pDestFT1 = nsCsFileTable::g_pItemMng->GetItem( pDest->GetType() )->GetInfo();
			if( nItem::CrossWars == pSrcFT1->s_nType_L && nItem::CrossWars == pDestFT1->s_nType_L )
			{
				g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
				return;
			}
		}

	}
*/
#pragma warning( default : 4806 )
#endif
	if( bCheckBelonging )
	{
		if( ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT )&&
			( TO_CONSTANT( nDstItemPos ) == SERVER_DATA_CHIPSET_CONSTANT ) )
		{
			cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );
			CsItem::sINFO* pSrcFT1 = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
			// pSrcFT1->s_nType_L 값이 
			if( 0 != pSrcFT1->s_nType_S )
			{
				if(g_pDataMng->GetDigivice()->DoYouHaveJointProgressChipset()) // 디지바이스에 조그레스 아이템이 있으면 리턴.
				{
					g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
					return;
				}
			}
			
			// 아직 귀속이 아닌 아이템일 경우
			if( pSrc && ( pSrc->m_nLimited == 0 ) )
			{
				CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
				// 사용시 귀속이라면
				if( pSrcFT->s_nBelonging == 1 )
				{
					assert_cs( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT );
					
#ifdef ITEM_USE_TIME_PASS
					assert_cs( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT );
					if(pSrc->m_nEndTime == 0xFFFFFFFF)			//시간제 아이템이 아닌경우
						cPrintMsg::PrintMsg( 30175, pSrcFT->s_szName );				
					else if(pSrc->m_nEndTime == 0xFFFFFFF0)		//시간제 아이템일 경우
						cPrintMsg::PrintMsg( 30176, pSrcFT->s_szName );
#else
					cPrintMsg::PrintMsg( 30175, pSrcFT->s_szName );
#endif

					assert_cs( cMessageBox::GetFirstMessageBox() );
					cMessageBox::GetFirstMessageBox()->SetValue1( nSrcItemPos );
					cMessageBox::GetFirstMessageBox()->SetValue2( nDstItemPos );
					return;
				}
			}
		}
		else if( ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_CHIPSET_CONSTANT )&&
			( TO_CONSTANT( nDstItemPos ) == SERVER_DATA_INVEN_CONSTANT ) )
		{
			cItemInfo* pDest = g_pDataMng->SrvID2ItemInfo( nDstItemPos );
			// 아직 귀속이 아닌 아이템일 경우
			if( pDest && ( pDest->m_nLimited == 0 ) )
			{
				CsItem::sINFO* pDestFT = nsCsFileTable::g_pItemMng->GetItem( pDest->GetType() )->GetInfo();
				// 사용시 귀속이라면
				if( pDestFT->s_nBelonging == 1 )
				{
					assert_cs( TO_CONSTANT( nDstItemPos ) == SERVER_DATA_INVEN_CONSTANT );
					cPrintMsg::PrintMsg( 30175, pDestFT->s_szName );
					assert_cs( cMessageBox::GetFirstMessageBox() );
					cMessageBox::GetFirstMessageBox()->SetValue1( nSrcItemPos );
					cMessageBox::GetFirstMessageBox()->SetValue2( nDstItemPos );
					return;
				}
			}
		}
#ifdef NEW_SHARESTASH
		// 인벤에서 계정 창고로
		else if( ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT )&&
			( TO_CONSTANT( nDstItemPos ) == SERVER_DATA_SHARESTASH_CONSTANT ) )
		{
			cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );	// SrvID2ItemInfo함수로 아이템 정보를 얻는다.
			SAFE_POINTER_RET( pSrc );

			//	CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
			//	if( (pSrc && ( pSrc->m_nLimited != 0 )) || pSrcFT->s_nBelonging != 0 )
			if( pSrc && ( pSrc->m_nLimited != 0 ) )
			{				

				cPrintMsg::PrintMsg( 30501 );
				g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
				return;

			}
		}
		//	창고에서 계정 창고로
		else if( ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_WAREHOUSE_CONSTANT )&&
			( TO_CONSTANT( nDstItemPos ) == SERVER_DATA_SHARESTASH_CONSTANT ) )
		{
			cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );	// SrvID2ItemInfo함수로 아이템 정보를 얻는다.
			SAFE_POINTER_RET( pSrc );

			//	CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
			//	if( (pSrc && ( pSrc->m_nLimited != 0 )) || pSrcFT->s_nBelonging != 0 )
			if( pSrc && ( pSrc->m_nLimited != 0 ) )
			{				

				cPrintMsg::PrintMsg( 30501 );
				g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
				return;

			}
		}		
		// 계정 창고에서 인벤으로
		else if( ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_SHARESTASH_CONSTANT )&&
			( TO_CONSTANT( nDstItemPos ) == SERVER_DATA_INVEN_CONSTANT ) )
		{
			cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );
			cItemInfo* pDest = g_pDataMng->SrvID2ItemInfo( nDstItemPos );	// SrvID2ItemInfo함수로 아이템 정보를 얻는다.

			// 원점과 목표점에 다 아이템이 있고 원점에는 노말 목표에는 귀속이면 이동실패
			if((NULL != pSrc && NULL != pDest) )
			{
				CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
				CsItem::sINFO* pDestFT = nsCsFileTable::g_pItemMng->GetItem( pDest->GetType() )->GetInfo();
				if( (0 == pSrcFT->s_nBelonging) && (0 != pDestFT->s_nBelonging) )
				{
					cPrintMsg::PrintMsg( 30501 );
					g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
					return;
				}
			}
			
		}
		//	계정 창고에서 창고로
		else if( ( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_SHARESTASH_CONSTANT )&&
			( TO_CONSTANT( nDstItemPos ) == SERVER_DATA_WAREHOUSE_CONSTANT ) )
		{
			cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );
			cItemInfo* pDest = g_pDataMng->SrvID2ItemInfo( nDstItemPos );	// SrvID2ItemInfo함수로 아이템 정보를 얻는다.

			// 원점과 목표점에 다 아이템이 있고 원점에는 노말 목표에는 귀속이면 이동실패
			if((NULL != pSrc && NULL != pDest) )
			{
				CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
				CsItem::sINFO* pDestFT = nsCsFileTable::g_pItemMng->GetItem( pDest->GetType() )->GetInfo();
				if( (0 == pSrcFT->s_nBelonging) && (0 != pDestFT->s_nBelonging) )
				{
					cPrintMsg::PrintMsg( 30501 );
					g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
					return;
				}
			}
		}		
#endif
		////착귀템 귀속 여부 변경 chu8820
		else if( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT &&
			TO_CONSTANT( nDstItemPos ) == SERVER_DATA_TEQUIP_CONSTANT)
		{
			cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );
			CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();

			// ** 문제가 발생하면 define을 적용합니다. ** //
 			// 공통은 아예 체크 하지 않음.
 			if( false == g_pModelDataMng->CostumeCategory_DProject(g_pCharMng->GetTamerUser()->GetFTID(), pSrcFT->s_nType_S) )
 			{
 				switch(pSrcFT->s_nType_S)
 				{	
				case 0:
//					{
// #ifdef ITEM_USE_TIME_PASS
// 						//2017-04-04-nova
// 						if(pSrc->m_nEndTime != 0xFFFFFFF0)
// 						{	
// 							if( net::game )
// 							{
// 								net::game->SendItemMove( TO_INVEN_SID( nSrcItemPos ), nDstItemPos, false );
// 							}
// 						}
// 						else
// 						{
// 							cPrintMsg::PrintMsg( 30177, pSrcFT->s_szName );
// 							cMessageBox::GetFirstMessageBox()->SetValue1( nSrcItemPos );
// 							cMessageBox::GetFirstMessageBox()->SetValue2( nDstItemPos );
// 						}						
// 						return;
// #endif						
// 					}
					break;
 				case COSTUME_ID_2602:case COSTUME_ID_2603:
					{
						CsTamer*		s_pFTTamer = nsCsFileTable::g_pTamerMng->GetTamer( g_pCharMng->GetTamerUser()->GetFTID() );
						cPrintMsg::PrintMsg( 30556, s_pFTTamer->GetInfo()->s_szPart );
						g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
						return;
					}
					break;
 
 				case COSTUME_ID_2604:case COSTUME_ID_2605:	
 					{
				#ifdef COSTUME_GENDER	// 메시지 관련 멤버로 인해 예외 메시지는 define이 선언되었을 때만 나옵니다.
 						CsTamer*		s_pFTTamer = nsCsFileTable::g_pTamerMng->GetTamer( g_pCharMng->GetTamerUser()->GetFTID() );
 						cPrintMsg::PrintMsg( 30556, s_pFTTamer->GetInfo()->s_szGender );
				#endif
 						g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
 						return;
 					}
 					break;
 
				case COSTUME_ID_2606:case COSTUME_ID_2607:case COSTUME_ID_2608:case COSTUME_ID_2609:
				case COSTUME_ID_2610:case COSTUME_ID_2611:case COSTUME_ID_2612:case COSTUME_ID_2613:
 				case COSTUME_ID_2614:case COSTUME_ID_2615:
				case COSTUME_ID_2616:case COSTUME_ID_2617:
					{
				#ifdef COSTUME_TAMER_ONLY	

						DWORD dwTamerCode = g_pModelDataMng->CostumeToTamerCode(pSrcFT->s_nType_S);
						if( dwTamerCode != 0 )
						{
							CsTamer*		s_pFTTamer = nsCsFileTable::g_pTamerMng->GetTamer( dwTamerCode );
						// s_szName 테이머만 착용할 수 있습니다.
						 cPrintMsg::PrintMsg( 30557, s_pFTTamer->GetInfo()->s_szName );
						}
				#endif
						g_pDataMng->ServerItemMoveFailed( nSrcItemPos, nDstItemPos );
						return;
					}
					break;
 
 				}
 			}// Costume If

			// 아직 귀속이 아닌 아이템일 경우
			if( pSrc && ( pSrc->m_nLimited == 0 ) )
			{
				//CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
				// 사용시 귀속이라면
				if( pSrcFT->s_nBelonging == 1 )
				{	
#ifdef ITEM_USE_TIME_PASS
					assert_cs( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT );
					if(pSrc->m_nEndTime == 0xFFFFFFFF)			//시간제 아이템이 아닌경우
						cPrintMsg::PrintMsg( 30175, pSrcFT->s_szName );				
					else if(pSrc->m_nEndTime == 0xFFFFFFF0)		//시간제 아이템일 경우
						cPrintMsg::PrintMsg( 30176, pSrcFT->s_szName );
#else
					cPrintMsg::PrintMsg( 30175, pSrcFT->s_szName );
#endif
					cMessageBox* pMsg = cMessageBox::GetFirstMessageBox();
					if( pMsg )
					{
						pMsg->SetValue1( nSrcItemPos );
						pMsg->SetValue2( nDstItemPos );
					}
					return;
				}
			}
		}
		else if( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT &&
		TO_CONSTANT( nDstItemPos ) == SERVER_DATA_DIGIVICE_CONSTANT)
		{
			cItemInfo* pSrc = g_pDataMng->SrvID2ItemInfo( nSrcItemPos );
			CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
			// 아직 귀속이 아닌 아이템일 경우
			if( pSrc && ( pSrc->m_nLimited == 0 ) )
			{
				//CsItem::sINFO* pSrcFT = nsCsFileTable::g_pItemMng->GetItem( pSrc->GetType() )->GetInfo();
				// 사용시 귀속이라면
				if( pSrcFT->s_nBelonging == 1 )
				{	
#ifdef ITEM_USE_TIME_PASS
					assert_cs( TO_CONSTANT( nSrcItemPos ) == SERVER_DATA_INVEN_CONSTANT );
					if(pSrc->m_nEndTime == 0xFFFFFFFF)			//시간제 아이템이 아닌경우
						cPrintMsg::PrintMsg( 30175, pSrcFT->s_szName );				
					else if(pSrc->m_nEndTime == 0xFFFFFFF0)		//시간제 아이템일 경우
						cPrintMsg::PrintMsg( 30176, pSrcFT->s_szName );
#else
					cPrintMsg::PrintMsg( 30175, pSrcFT->s_szName );
#endif

					assert_cs( cMessageBox::GetFirstMessageBox() );
					cMessageBox::GetFirstMessageBox()->SetValue1( nSrcItemPos );
					cMessageBox::GetFirstMessageBox()->SetValue2( nDstItemPos );
					return;
				}
			}
		}
	}

	// digivice 관련 연산시 에러, 연결 종료됨
	newp(pItem::Move);
		push(nSrcItemPos); // nSrcItemPos 값은 (tab, array idx) 의 조합
		push(nDstItemPos);
	endp(pItem::Move);
	send();
	// 응답 성공시 RecvItemMove, 실패시 RecvItemMoveFailure 
}


void cCliGame::SendItemDrop(short nInvenPos, nSync::Pos &FieldPos, u2 nCount)
{
	if(m_bPortalRequesting)
	{
		return;
	}	

	newp(pItem::Drop);
		push(nInvenPos);
		push(FieldPos);
		push(nCount);
	endp(pItem::Drop);
	send();
}


void cCliGame::SendItemCrop(n4 nItemIDX)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	if(m_bPortalRequesting)
	{
		return;
	}

	// digivice 관련 연산시 에러
	newp(pItem::Crop);
		push(nItemIDX);
	endp(pItem::Crop);
	send();
}
#ifdef CROSSWARS_SYSTEM
void cCliGame::SendItemUseCross(u4 nUID, n2 nInvenPos)
{

	// digivice 관련 연산시 에러
	newp(pItem::CrossWarsUse);
		push(nUID);		// target unique id
		push( nInvenPos );
	endp(pItem::CrossWarsUse);
	send();
}
#endif
void cCliGame::SendItemUse(u4 nUID, n2 nInvenPos)
{
	// 파트너몬 변경시에 실패
	/*if( g_pDataMng->GetServerSync()->IsChageDigimon() )
	{
		cPrintMsg::PrintMsg( 30024 );
		g_pDataMng->ServerItemUseFailed( nInvenPos );
		return;
	}*/

	if(m_bPortalRequesting)
	{
		g_pDataMng->ServerItemUseFailed( nInvenPos );
		return;
	}
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
	{
		cPrintMsg::PrintMsg( 30028 );
		g_pDataMng->ServerItemUseFailed( nInvenPos );
		return;
	}
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
	{
		cPrintMsg::PrintMsg( 30357 );
		g_pDataMng->ServerItemUseFailed( nInvenPos );
		return;
	}	

	// digivice 관련 연산시 에러
	newp(pItem::Use);
		push(nUID);		// target unique id
		push(nInvenPos);
	endp(pItem::Use);
	send();
}


// nInvenSrcPos위치의 소켓아이템을 nInvenDstPos위치의 장비 아이템에 장착한다.
void cCliGame::SendSocketIn(u4 nNpcIDX, u2 nInvenSrcPos, u2 nInvenDstPos, u1 nSocketOrder, u4 nInvenPortablePos, bool bVip )
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	newp(pItem::SocketIn);

#ifdef SDM_VIP_SYSTEM_20181105
		push(bVip);
#endif

#ifdef ITEM_USE_TIME_PASS
		push(nInvenPortablePos);
#endif
		push(nNpcIDX);
		push(nInvenSrcPos);
		push(nInvenDstPos);
		push(nSocketOrder);	// nInvenDstPos위치의 장비아이템에서 사용할 소켓의 위치(left==0, right=1...)
	endp(pItem::SocketIn);
	send();
}


// nInvenPos위치의 장비 아이템을 nOrder(left->right, 0~n) 위치에 박힌 소켓 아이템 분리
void cCliGame::SendSocketOut(u4 nNpcIDX, u2 nInvenSrcPos, u2 nInvenDstPos, u1 nOrder, u4 nInvenPortablePos, bool bVip)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );	

	newp(pItem::SocketOut);

#ifdef SDM_VIP_SYSTEM_20181105
		push(bVip);
#endif

#ifdef ITEM_USE_TIME_PASS
	push(nInvenPortablePos);
#endif
		push(nNpcIDX);
		push(nInvenSrcPos);
		push(nInvenDstPos);
		push(nOrder);
	endp(pItem::SocketOut);
	send();
}


void cCliGame::SendSocketClear(u4 nNpcIDX, u2 nInvenPos, u1 nSocketOrder)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );	

	newp(pItem::SocketClear);
		push(nNpcIDX);
		push(nInvenPos);	// 소켓이 박혀있는 아이템 인벤 위치
		push(nSocketOrder); // 좌측부터 소켓의 순서(0~n)
	endp(pItem::SocketClear);
	send();
}


void cCliGame::SendItemAnalysis(u4 nNpcIDX, u2 nInvenPos, u4 nInvenPortablePos, bool bVip)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );	

	newp(pItem::Analysis);
#ifdef SDM_VIP_SYSTEM_20181105
		push(bVip);
#endif
#ifdef ITEM_USE_TIME_PASS
		push(nInvenPortablePos);
#endif
		push(nNpcIDX);
		push(nInvenPos);
	endp(pItem::Analysis);
	send();
}


void cCliGame::SendAttack(u4 nHitterUID, u4 nTargetUID)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	if(m_bPortalRequesting)
	{
		return;
	}

	newp(pGame::Attack);
		push(nHitterUID); // digimon uid
		push(nTargetUID);
	endp(pGame::Attack);
	send();

//	net::chat("SendAttack(%d->%d)", (int)nHitterUID, (int)nTargetUID);
}


bool cCliGame::SendSkill(u1 nSkillArrIDX, u4 nHitterUID, u4 nTargetUID)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
		return false;

	if(m_bPortalRequesting)
	{
		return false;
	}


#ifndef _GIVE
	TCHAR szSkill[ MAX_PATH ];
	_stprintf( szSkill, L"SkillUse = %d ( %s )", nSkillArrIDX, g_pCharMng->GetDigimonUser( 0 )->GetSkillMng()->GetFTSkill( nSkillArrIDX )->GetInfo()->s_szName );
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::NOTICE;
	CProtocol.m_wStr = szSkill;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
#endif

	xstop1(nSkillArrIDX<5, "nSkillArrIDX:%d", nSkillArrIDX);

	newp(pGame::Skill);
	push(nSkillArrIDX);
	push(nHitterUID);
	push(nTargetUID);
	endp(pGame::Skill);
	send();

	return true;
}

void cCliGame::SendActiveSkill(u4 nSkillArrIDX, u4 nTargetUID)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	if(m_bPortalRequesting)
	{
		//return false;
		assert_cs(false);
	}

	newp(pTamer::Skill);
	push(nSkillArrIDX);
	push(nTargetUID);
	endp(pTamer::Skill);
	send();
}

/*
void cCliGame::SendSkill(u1 nSkillArrIDX, u2 nSrcUID, nSync::Pos &TargetPosition)
{
	if(m_bPortalRequesting)
	{
		return;
	}

	newp(pGame::Skill); // 특정 영역에 스킬을 사용할 경우
		push(nSkillArrIDX);
		push(nSrcUID);
		push(TargetPosition);
	endp(pGame::Skill);
	send();
}
//*/


void cCliGame::SendSetTarget(u4 nHitterUID, u4 nTargetUID)
{	
	assert_cs( (g_pGameIF == NULL )||(g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false) );

	if(m_bPortalRequesting)
	{
		return;
	}

// 	if( g_pResist->IsTutorial() == true )
// 	{
// 		return;
// 	}

// 	assert_csm1(nHitterUID<32768, _T( "%d" ), nHitterUID);
// 	assert_csm1(nTargetUID<32768, _T( "%d" ), nTargetUID);

	// 상대가 타겟으로 설정되면
	// 이후 상대의 hp,sp상태값 정보를 얻을 수 있다.

	newp(pGame::SetTarget);
		push(nHitterUID); // my digimon uid
		push(nTargetUID); // target monster uid
	endp(pGame::SetTarget);
	send();
}


void cCliGame::SendTamerAchievement(u2 nAchievement)
{
	newp(pSync::Achievement);
	push(nAchievement);
	endp(pSync::Achievement);
	DoSend();
}


void cCliGame::SendTamerOption(u4 nOption)
{
	newp(pTamer::Option);
		push(nOption);
	endp(pTamer::Option);
	send();
}

void cCliGame::SendDieConfirm(void)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	if( g_pResist->IsEnablePortal() == false ) // 포탈 활성화가 아니면 리턴
	{
		return;
	}

	cClient::SetSendBlock(false); // 이후 송신은 모두 무시된다.

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = _T("SendDieConfirm");
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	newp(pGame::DieConfirm);
		push(net::game->port^net::access_code|net::account_idx);
		push(net::account_idx);
		push(net::access_code);
		push(g_nNetVersion);
	endp(pGame::DieConfirm);
	send();

	cClient::SetSendBlock(true);
}


void cCliGame::SendEvolution(uint nDigimonUID, uint nSlotIDX)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	newp(pGame::Evolution);
	push(nDigimonUID);
	push((u1)nSlotIDX);
	endp(pGame::Evolution);
	send();
}


void cCliGame::SendEvoCard(uint nDigimonUID, uint nInvenSlot)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	newp(pGame::EvoCard);
	push(nDigimonUID);
	push((u2)nInvenSlot);
	endp(pGame::EvoCard);
	send();
}


void cCliGame::SendEvoCrest(uint nDigimonUID, uint nInvenSlot)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	newp(pGame::EvoCrest);
	push(nDigimonUID);
	push((u2)nInvenSlot);
	endp(pGame::EvoCrest);
	send();
}



void cCliGame::SendStatUp(uint nUID, uint nStat)
{
	assert_cs( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) == false );

	switch(GetClass(nUID))
	{
	case nClass::Tamer   :
	case nClass::Digimon : break;
	default : xassert(false, "nUID is not valid");
	}
	
	switch(nStat)
	{
	case MHP :
	case MDS :
	case AP  :
	case DP  :	break;
	default  :	xassert1(false, "stat(%d) is not valid", nStat);
	}

	newp(pGame::StatUp);
		push(nUID);
		push((u1)nStat);
	endp(pGame::StatUp);
	send();
}


void cCliGame::SendDigimonSkillLevelUp(uint nUID, uint nEvoUnitArrIDX, uint nSkillArrIDX)
{
	switch(GetClass(nUID))
	{
	case nClass::Digimon : break;
	default : xassert(false, "nUID is not valid");
	}

	newp(pSkill::LevelUp);
		push(nUID);
		push((u1)nEvoUnitArrIDX);
		push((u1)nSkillArrIDX);
	endp(pSkill::LevelUp);
	send();
}


void cCliGame::SendChangeTamer(int nTamerType, int nInvenSlotNo)
{
	newp(pTamer::Change);
		push(nTamerType);	// 해당 값으로 테이머 타입을 변경한다.
		push(nInvenSlotNo);
	endp(pTamer::Change);
	send();
}



void cCliGame::SendStopAttack(u4 nUID)
{
	newp(pGame::StopAttack);
	push(nUID);
	endp(pGame::StopAttack);
	send();
}


void cCliGame::SendRefreshScreen(void)
{
	newp(pGame::RefreshScreen);
	endp(pGame::RefreshScreen);
	send();
}


void cCliGame::SendDigitamaToMoney(u4 nNpcIDX, u4 nInvenPos, u4 nInvenPortablePos, bool bVip)
{
	newp(pItem::DigitamaToMoney);

#ifdef SDM_VIP_SYSTEM_20181105
		push(bVip);
#endif

#ifdef ITEM_USE_TIME_PASS			//2017-04-05-nova
		push(nInvenPortablePos);
#endif
		push(nNpcIDX);
		push(nInvenPos);
	endp(pItem::DigitamaToMoney);
	send();

	GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_SELL_COMPLETE, NULL );
}


#pragma todo("구슬을 아이템으로 교환 요청")
void cCliGame::SendCapsuleToItem(u4 nNpcIDX, u4 nInvenPos)
{
	newp(pItem::CapsuleToItem);
		push(nNpcIDX);
		push(nInvenPos);
	endp(pItem::CapsuleToItem);
	send();
}


void cCliGame::SendDigitamaToItem(u4 nNpcIDX, u4 nInvenPos, u4 nInvenPortablePos, bool bVip)
{
	newp(pItem::DigitamaToItem);

#ifdef SDM_VIP_SYSTEM_20181105
		push(bVip);
#endif
#ifdef ITEM_USE_TIME_PASS			//2017-04-05-nova
		push(nInvenPortablePos);
#endif
		push(nNpcIDX);
		push(nInvenPos);
		
	endp(pItem::DigitamaToItem);
	send();
}

void cCliGame::SendDigitamaScanItem(bool bIsVip, u4 uPortableIdx, u4 uNpcIdx, u4 uInvenIdx, u2 uScanCount)
{
#ifdef UI_ITEMSCAN_RENEWAL
	C2GS_SEND_ITEMSCAN sendPacket;
	sendPacket.bVIPMemberMode = bIsVip;
	sendPacket.uInvenActiveItemSlot = uPortableIdx;
	sendPacket.uNpcIDX = uNpcIdx;
	sendPacket.uInvenPos = uInvenIdx;
	sendPacket.u2ScanCount = uScanCount;

	newp( sendPacket.GetProtocol() );
#ifdef SDM_VIP_SYSTEM_20181105
		push( sendPacket.bVIPMemberMode );
#endif
#ifdef ITEM_USE_TIME_PASS
		push( sendPacket.uInvenActiveItemSlot );
#endif
		push( sendPacket.uNpcIDX );
		push( sendPacket.uInvenPos );
		push( sendPacket.u2ScanCount );
	endp( sendPacket.GetProtocol() );
	send();
#endif
}

// 용병 부화용 디지타마 등록
void cCliGame::SendHatchIn(uint nInvenSlotNo, n4 nNpcIDX, bool bVip)
{
	newp(pGame::HatchIn);
#ifdef SDM_VIP_SYSTEM_20181105
	push(bVip);
#endif
	push(nInvenSlotNo);
	push(nNpcIDX);
	endp(pGame::HatchIn);
	send();
}

//백업디스크 등록 서버로 보낸다.

void cCliGame::SendHatchBackupDisk(uint nInvenSlotNo, n4 nNpcIDX, bool bVip)
{
	newp( pItem::HatchBackupDiskAdd );
#ifdef SDM_VIP_SYSTEM_20181105
	push(bVip);
#endif
	push( nInvenSlotNo );			
	push( nNpcIDX );
	endp( pItem::HatchBackupDiskAdd );
	send();
}

// 부화 디지몬 데이타 입력
void cCliGame::SendHatchUp(n4 nNpcIDX, n1 nDataLv /*= 0*/, bool bVip)
{
	newp(pGame::HatchUp);
#ifdef SDM_VIP_SYSTEM_20181105
	push(bVip);
#endif
	push(nNpcIDX);
#ifdef MAKE_TACTICS_UI_UPGRADE
	push(nDataLv);
#endif
	endp(pGame::HatchUp);
	send();
}

// 부화 확인 요청
void cCliGame::SendHatchOut(wchar* szName, n4 nNpcIDX, u4 nInvenPortablePos, bool bVip)
{
	if( NULL == szName )
		return;

	newp(pGame::HatchOut);

#ifdef SDM_VIP_SYSTEM_20181105
		push(bVip);
#endif
#ifdef ITEM_USE_TIME_PASS
		push( nInvenPortablePos );
#endif
		std::wstring name(szName);
		push( name );			// 새 용병의 이름
		push(nNpcIDX);
	endp(pGame::HatchOut);
	send();
}
#ifdef CROSSWARS_SYSTEM
// 크로스 부화 확인 요청
void cCliGame::SendCrossWarsHatch(u2 IDX, wchar* szName)
{
	if( NULL == szName )
		return;
	std::wstring name(szName);

	newp(pDigimon::CrossWarsHatch);
	push(IDX);
	push( name );			// 새 용병의 이름	
	endp(pDigimon::CrossWarsHatch);
	send();
}
void cCliGame::SendCrossWarsRecover(void)
{
	newp(pDigimon::CrossWarsRecover);	
	endp(pDigimon::CrossWarsRecover);
	send();
}
void cCliGame::SendCrossChangePartner( int nArrIDX)
{
	//xstop1(nArrIDX<nLimit::Digimon, "nArrIDX:%d error", nArrIDX);

	newp(pDigimon::CrossWarsChangeOne);
	push((u1)nArrIDX);
	endp(pDigimon::CrossWarsChangeOne);
	send();
}

void cCliGame::SendCrossChangePartner2( int nArrIDX, int nArr[])
{
	//xstop1(nArrIDX<nLimit::Digimon, "nArrIDX:%d error", nArrIDX);

	newp(pDigimon::CrossWarsCombine);

	push((u1)nArrIDX);
	for(u1 i =0; i < nArrIDX; i++)
	{
		push((u1)nArr[i]);
	}

	endp(pDigimon::CrossWarsCombine);
	send();
}
#endif
void cCliGame::SendChangePartner(u1 nArrIDX)
{
	xstop1(nArrIDX<nLimit::DigimonBaseSlot, "nArrIDX:%d error", nArrIDX);

	newp(pGame::ChangePartner);
	push((u1)nArrIDX);
	endp(pGame::ChangePartner);
	send();
}


void cCliGame::RecvDeletePartner(void)
{
	int nArrIDX;
	pop(nArrIDX);

	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::DELETE_TACTICS, 0 );					

	switch(nArrIDX)
	{
	case -1 : // 2차 비번 인증 실패
		cPrintMsg::PrintMsg( 10043 );
		//g_pSecondPassword->SetPassCheck( false );
		return;
	case -2 : // 주민번호 뒷 7자리 인증 실패
		{
			if( GLOBALDATA_ST.Is2ndPassType( GData::e2ndNumberPass ) )
				cPrintMsg::PrintMsg( 10043 );
			else if( GLOBALDATA_ST.Is2ndPassType( GData::eAccountPass ) )
				cPrintMsg::PrintMsg( 10044 );
			else if( GLOBALDATA_ST.Is2ndPassType( GData::eEmail ) )
				cPrintMsg::PrintMsg( 10045 );
			else if( GLOBALDATA_ST.Is2ndPassType( GData::eStringPass ) )
				cPrintMsg::PrintMsg( 10046 );
		}
		return;
	}

	cPrintMsg::PrintMsg( 20100, g_pDataMng->GetTactics()->GetTactics( nArrIDX-1 )->s_szName );
	g_pDataMng->GetTactics()->DeleteTactics( nArrIDX - 1 );						

	int NoUseParameter = 0;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_HIDE_VIEWER, &NoUseParameter);

	//g_pSecondPassword->SetPassCheck( true );
	//g_pSecondPassword->Close();
}


// 2차 비번 활성화시, pass2는 2차비번
// 아닌 경우, 미국 버전 ==> 이메일, 한국 버전==> 주민번호 뒷7자리
void cCliGame::SendDeletePartner(u1 nArrIDX, char *pass2)	// 2차 비번 처리
{
	xstop1(nArrIDX<nLimit::DigimonBaseSlot, "nArrIDX:%d error", nArrIDX);
	xstop(nArrIDX>0, "nArrIDX:0 현재 선택중인 디지몬 제거 불가");

	SAFE_POINTER_RET( pass2 );

	newp(pGame::DeletePartner);
	push((u1)nArrIDX);

	std::string strPass(pass2);

	if( GLOBALDATA_ST.Is2ndPassEncry() )
	{
		char LastPass[1024] = {0,};
		md5( const_cast<char*>(strPass.c_str()), LastPass );
		strPass = LastPass;
	}

	push(strPass);	

// 	switch(GLOBALDATA_ST.Get2ndPassType())
// 	{
// 	case GData::eEmail:			push(strPass);	break;
// 	case GData::eStringPass:	push(strPass);	break;
// 	case GData::eAccountPass:	
// 	case GData::e2ndNumberPass:
// 		{
// 			char LastPass[Language::pLength::SecondPass + 1] = {0,};
// 			if( GLOBALDATA_ST.IsCountry( GData::Thailand ) )
// 				strPass.copy( LastPass, sizeof(char)*Language::pLength::SecondPass );
// 			else
// 				md5( const_cast<char*>(strPass.c_str()), LastPass );
// 			push(LastPass);	
// 		}
// 		break;
// 	}

	endp(pGame::DeletePartner);
	send();
}


void cCliGame::SendHatchCancel(n4 nNpcIDX, bool bVip)
{
// 용병 부화 데이타 투입 1회 이상 성공인 상황이면, 용병알 제거
// 용병 부화 데이타 투입이 없었다면, 인벤으로 되돌려줌
	newp(pGame::HatchCancel);
#ifdef SDM_VIP_SYSTEM_20181105
	push(bVip);
#endif
	push(nNpcIDX);
	endp(pGame::HatchCancel);
	send();
}
void cCliGame::SendHatchBackupDiskCancel(n4 nNpcIDX, bool bVip)
{
	//백업디스크 등록 취소
	newp(pItem::HatchBackupDiskCancel);
#ifdef SDM_VIP_SYSTEM_20181105
	push(bVip);
#endif
	push(nNpcIDX);
	endp(pItem::HatchBackupDiskCancel);
	send();
}

void cCliGame::SendCropGiftItem(n2 nGiftItemSlot)
{
	u2 count = 1;

	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GET_REWARD, count );

	newp(pItem::CropGiftItem);
		push(count);
		push(nGiftItemSlot);
	endp(pItem::CropGiftItem);
	send();
}

// void cCliGame::SendCropGiftItemAll()
// {
// 	//2017-03-15-nova
// 	GS2C_RECV_REWARD_SEND_TYPE recv;
// 	recv.iType = 1;
// 	recv.bSendAll = true;
// 	recv.iIndex = -1;
// 	GAME_EVENT_ST.OnEvent( EVENT_CODE::REWARD_SEND_ACCPET_DATA, &recv );	
// }


void cCliGame::SendCropCashItem(n2 nCashItemSlot)
{
	newp(pItem::CropCashItem);
		push(nCashItemSlot);
	endp(pItem::CropCashItem);
	send();
}


void cCliGame::SendOpenRideMode(uint nEvoUnitArrIDX, int nItemType)
{
	newp(pGame::OpenRideMode);
		push(nEvoUnitArrIDX);
		push(nItemType);		// 어떤 아이템으로 오픈하는 것인지
	endp(pGame::OpenRideMode);
	send();
}

// CAPSULE_EVOLUTION - 캡슐 진화 슬롯 오픈 시 파라메터 추가 chu8820
void cCliGame::SendOpenEvoSlot( uint nEvoUnitArrIDX, n2 nInvenIdx )
{
	newp(pGame::OpenEvoSlot);
		push(nEvoUnitArrIDX);
		if( nInvenIdx != -1 )	// 캡슐진화일 경우 사용할 캡슐 아이템 인벤 번호 보내줌 (캡슐진화일 때만 nInvenIdx 값 넣음)
			push( nInvenIdx );
	endp(pGame::OpenEvoSlot);
	send();
}


void cCliGame::SendDigimonCareSlotList(u4 nNpcType, u4 nInvenIdx, bool bVip)
{
	newp(pDigimon::CareSlotList);	// 용병 보관소 - 용병 목록

#ifdef SDM_VIP_SYSTEM_20181105
	push(bVip);
#endif

#ifdef ITEM_USE_TIME_PASS
	push( nInvenIdx );		//2017-04-04-nova
#endif		

	push(nNpcType);
	endp(pDigimon::CareSlotList);
	send();
}


void cCliGame::SendDigimonMoveTo(int nSlot1, int nSlot2, u4 nNpcType, bool bVip)
{
	if(nSlot1 < 1000)	// 파트너 용병 슬롯
	{
		xassert1(nSlot1<nLimit::DigimonBaseSlot, "nSlot1:%d값이 너무 큼", nSlot1);
	}
	else
	{
		xassert1(nSlot1-1000<nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse, "nSlot1:%d값이 너무 큼", nSlot1);
	}
	if(nSlot2 < 1000)
	{
		xassert1(nSlot2<nLimit::DigimonBaseSlot, "nSlot2:%d값이 너무 큼", nSlot2);
	}
	else
	{
		xassert1(nSlot2-1000<nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxTacticsHouse, "nSlot2:%d값이 너무 큼", nSlot2);
	}

	newp(pDigimon::MoveTo);
#ifdef SDM_VIP_SYSTEM_20181105
		push(bVip);
#endif
		push(nSlot1);
		push(nSlot2);
		push(nNpcType);
	endp(pDigimon::MoveTo);
	send();
}
void cCliGame::SendDigimonMoveBetweenArchive(int Src, int Dest, u4 nNpcType, bool bVip)
{
	if(Src < 1000 || Dest < 1000)
	{
		return;
	}
	else
	{
		newp(pDigimon::CareDigimonMove);
#ifdef SDM_VIP_SYSTEM_20181105
		push(bVip);
#endif
		push(Src);
		push(Dest);
		push(nNpcType);
		endp(pDigimon::CareDigimonMove);
		send();
	}
}


void cCliGame::SendJumpGate(u2 nInvenPos, u2 nOrderIDX, bool bVip)
{
	if( g_pResist->IsEnablePortal() == false )
	{
		return;
	}

	newp(pGame::JumpGate);

#ifdef SDM_VIP_SYSTEM_20181105
		push(bVip);
#endif
		push(nInvenPos);	// 인벤토리 슬롯 위치
		push(nOrderIDX);	// dm_item_list-jumpbooster-map_id 값
	endp(pGame::JumpGate);
	send();

	if( g_pDataMng && !bVip)
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
		{
			cItemInfo* pItemInfo = pInven->GetData( nInvenPos );
			if( pItemInfo )
			{
				pItemInfo->DecreaseCount( 1 );
				cData_QuickSlot::CheckItemCount_AllUser();
			}
		}		
	}	
}

// nRequestType 요청 타입 1: 충전 2: 사용 
//void cCliGame::SendHatchLostGageProgress(u4 nRequestType, n4 npcIDX, u4 ItemType)
//{
//	// nRequestType == 2 의 게이지 사용의 경우 아이템 타입을 보내지 않아도 무방 합니다. 
//
//	newp( pItem::HatchOptionAdd );
//		push( nRequestType );
//		push( npcIDX );
//		push( ItemType );
//	endp( pItem::HatchOptionAdd );
//	send();
//}

void cCliGame::SendClientOptionSave(u4 nOption)
{
	newp( pGame::SaveClientOption );
		push( nOption );
	endp( pGame::SaveClientOption );
	send();
}

void cCliGame::SendCardEventStart()	// 카드 이벤트 시작을 서버에 알림
{
	newp( pMiniEvent::PiemonGameStart );
	endp( pMiniEvent::PiemonGameStart );
	send();
}

void cCliGame::SendCardEventEnd()	// 카드 이벤트 종료를 서버에 알림
{
	newp( pMiniEvent::PiemonGameEnd );
	endp( pMiniEvent::PiemonGameEnd );
	send();
}

void cCliGame::SendGetNatureItem( n1 nElementType, n1 nNatureType )
{

	newp( pMonster::GetNatualItem );
	push( nElementType );
	push( nNatureType );
	endp( pMonster::GetNatualItem );
	send();
}

void cCliGame::SendTamerSummonSkill(TCHAR const* szName, u4 nIndex, u1 nCase)
{
	if( NULL == szName )
		return;

	std::wstring tamerName(szName);

	newp( pSkill::Summon );
	push( nCase );
	push( tamerName );
	push( nIndex );	// Tamer_Skill.xls 의 인덱스
	endp( pSkill::Summon );
	send();
}

void cCliGame::SendTamerSummonAccept(u1 nCase, u1 nValue)
{
	newp( pSkill::Summon );
	push( nCase );
	push( nValue );
	endp( pSkill::Summon );
	send();
}

void cCliGame::SendTamerSummon_StateCheck(u1 nCase, u1 nValue, u1 nOK )
{
	newp( pSkill::Summon );
	push( nCase );
	push( nValue );
	push( nOK );
	endp( pSkill::Summon );
	send();
}

void cCliGame::SendTamerSummon_StateCheck2(u1 nCase, u1 nValue, u4 nSkillCode )
{
	newp( pSkill::Summon );
	push( nCase );
	push( nValue );
	push( nSkillCode );
	endp( pSkill::Summon );
	send();
}

void cCliGame::SendTamerActiveSkill( u4 nSkillIdx )
{
	newp( pTamer::Skill );
		push(nSkillIdx);
	endp( pTamer::Skill );
	send();
}

void cCliGame::SendTamerActiveSkillEnd(u4 nSkillIdx )
{
	newp( pTamer::SkillEnd );
	push( nSkillIdx );
	endp( pTamer::SkillEnd );
	send();
}

void cCliGame::SendTamerCashSkillDelete( u4 nSkillIndex )
{
	newp( pTamer::ItemSkillRemove );
	push( nSkillIndex );				// Tamer_Skill.xls 의 스킬 Index
	endp( pTamer::ItemSkillRemove );
	send();
}

#ifdef MINIGAME
void cCliGame::SendMinigameStart( u1 nGameKind, u1 nlevel )
{
	newp(pMiniGame::MiniGameStart);
		push(nGameKind);	//어디서 띄운 게임인가 (부화보정 / 강화 / ..)
		push(nlevel);			//게임레벨
	endp(pMiniGame::MiniGameStart);
	send();
}
void cCliGame::SendMinigameClickBtn( u1 nGameKind, u2 nBarIndex )
{
	newp(pMiniGame::MiniGameClickBtn);
		push(nGameKind);	//어디서 뛰운 게임인가
		push(nBarIndex);		//바 인덱스
	endp(pMiniGame::MiniGameClickBtn);
	send();
}
void cCliGame::SendMinigameTimeOut()
{
	newp(pMiniGame::MiniGameTimeOut);
	endp(pMiniGame::MiniGameTimeOut);
	send();
}
void cCliGame::SendMinigameEnd( u1 nEndResult )
{
	newp(pMiniGame::MiniGameEnd);
	push( nEndResult );		//정상 종료 : 1 , 비정상 정료 : 0
	endp(pMiniGame::MiniGameEnd);
	send();
}
void cCliGame::SendMiniGameIsStartReq()
{
	newp(pMiniGame::MiniGameIsStartReq);
	endp(pMiniGame::MiniGameIsStartReq);
	send();
}
#endif


void cCliGame::SendTamerJumpGate( u2 nItemID, u2 nMapID )
{
	if( g_pResist->IsEnablePortal() == false )
	{
		return;
	}

	newp(pSkill::RickySkill);
	push(nItemID);	// 아이템 id
	push(nMapID);	// 맵 id
	endp(pSkill::RickySkill);
	send();
}


void cCliGame::SendGotchaOpen(u4 nNpcIDX)
{
	assert_cs(nNpcIDX != NULL);
	newp(pItem::GotchaOpen);
	push(nNpcIDX);
	endp(pItem::GotchaOpen);
	send();
}
void cCliGame::SendGotchaGet( u4 nUIDX , u4 nNpcIDX, u4 nInvenIndex, u4 nResetCnt )
{
	newp(pItem::GotchaExecute);
	push(nUIDX);
	push(nNpcIDX);
	push(nInvenIndex);
 	push(nResetCnt);
	endp(pItem::GotchaExecute);
	send();
}

void cCliGame::SendGotchaItem( u4 nNpcIDX, u4 nItemID, /*u4 nItemCnt,*/ u4 nAllItemCnt, u4 nInvenSlotCnt, u4* nDecreaseItemSlot/*, n4 nGroupID*/ )
{

}

#ifdef EGG_PREFERMENT
void cCliGame::SendMysteryGet( u4 nUIDX, u4 nNpcIDX, std::map< int, int >& mapItems, u4 nItemID )
#else
void cCliGame::SendMysteryGet( u4 nUIDX, u4 nNpcIDX, std::map< int, int >& mapItems )
#endif
{
	newp( pItem::MysteryMachine );
	push( nUIDX );
	push(nNpcIDX);
#ifdef EGG_PREFERMENT
	push( nItemID );
#endif
	push( static_cast<u4>(mapItems.size()) );
	std::map< int, int >::iterator it = mapItems.begin();
	std::map< int, int >::iterator itEnd = mapItems.end();
	for( ; it != itEnd; ++it )
	{
		push( static_cast<u4>(it->first) );
		push( static_cast<u4>(it->second) );
	}
	endp( pItem::MysteryMachine );
	send();
}

// #ifdef FIREWORK
// void cCliGame::SendFireWork( u2 nUIDX, u4 nItemID, u2 nInvenPos )
// {
// 	newp(pItem::FireWork);
// 	push(nUIDX);
// 	push(nItemID);
// 	push(nInvenPos);
// 	endp(pItem::FireWork);
// 	send();
// }
// #endif

void cCliGame::SendAccessoryCheck( u4 nUID, u2 nInvenPos )
{
	newp( pItem::AccessoryCheck );
 	push( nUID );
 	push( nInvenPos );
	endp( pItem::AccessoryCheck );
	send();
}
void cCliGame::SendAccessoryEnchant( u4 nUID, u2 nItemPos, u2 nAccPos, int nCombIdx )
{
	newp( pItem::AccStoneUse );
	push( nUID );
	push( nItemPos );
	push( nAccPos );
	if( nCombIdx != -1 )
		push( (u1)nCombIdx );
	endp( pItem::AccStoneUse );
	send();
}

void cCliGame::SendIncubaterList()						//	인큐베이터 오픈
{
	newp( pDigimon::IncubaterList );
	endp( pDigimon::IncubaterList );
	send();
}

void cCliGame::SendIncubatorFacilitation( u1 nIncuSlot, u4 nHouseSlot, u4 nInvenSlot )			//	촉진
{
	newp( pDigimon::Facilitation );
	push(nIncuSlot);
	push(nHouseSlot);
	push(nInvenSlot);
	endp( pDigimon::Facilitation );
	send();
}

void cCliGame::SendIncubatorExtraction( u1 nIncuSlot )			//	추출 ( 꺼내기 )
{
	newp( pDigimon::Extraction );
	push(nIncuSlot);
	endp( pDigimon::Extraction );
	send();
}

void cCliGame::SendIncubaterSlotChange( n4 nIncuSlot, n4 nHouseSlot )	// 촉진 중인 인큐베이터 디지몬 교체
{
	newp( pDigimon::IncubaterSlotChange );
	push(nIncuSlot);
	push(nHouseSlot);
	endp( pDigimon::IncubaterSlotChange );
	send();
}

void cCliGame::SendTacticsOpen( u4 nUID, n2 nInvenPos )
{
// 	newp( pDigimon::TacticsOpen );
// 	push(nUID);
// 	push(nInvenPos);
// 	endp( pDigimon::TacticsOpen );
// 	send();

}

void cCliGame::SendEncyclopediaOpen()
{
 	newp( pDigimon::DigimonBookInfo );
 	endp( pDigimon::DigimonBookInfo );
 	send();
}

void cCliGame::SendEncyclopediaGetItem( u4 nDigimonID )
{
	newp( pDigimon::DigimonBookItem );
	push( nDigimonID );
	endp( pDigimon::DigimonBookItem );
	send();
}

void cCliGame::SendEncyclopediaGetDeck( u4 nDeckIdx )
{
	newp( pDigimon::DigimonBookDeckUse );
	push( nDeckIdx );
	endp( pDigimon::DigimonBookDeckUse );
	send();
}

void cCliGame::SendSetMasterCardLeader( u2 nCardID )
{
	newp( pDigimon::SetMasterCardLeader );
	push(nCardID);
	endp( pDigimon::SetMasterCardLeader );
	send();
}

void cCliGame::SendUnSetMasterCardLeader( u2 nCardID )
{
	newp( pDigimon::UnSetMasterCardLeader );
	push(nCardID);
	endp( pDigimon::UnSetMasterCardLeader );
	send();
}

void cCliGame::SendCardUnSeal( u2 nInven_SealIdx )
{
	newp(pItem::CardUnSeal);
	push(nInven_SealIdx);
	endp(pItem::CardUnSeal);
	send();
}

void cCliGame::SendCardDrop( short nInvenPos, u2 nCount )
{
	newp(pItem::CardDrop);
	push(nInvenPos);
	push(nCount);
	endp(pItem::CardDrop);
	send();
}


void cCliGame::SendReCardSeal(u2 nCardID, u4 nCardAmount)
{
	newp(pItem::CardReSeal);
	push(nCardID);
	push(nCardAmount);
	endp(pItem::CardReSeal);
	send();
}

void cCliGame::SendCardBookmark(u2 nCardCode, u1 nBookmark)
{
	newp( pTamer::CardBookmark );
	push( nCardCode );
	push( nBookmark );
	endp( pTamer::CardBookmark );
	send();
}

// 디지몬 캐시 스킬 삭제
void cCliGame::SendDigimonSkillDelete( u4 nSkillCode )
{
	newp( pSkill::RemoveSkillChip );
	push( nSkillCode );
	endp( pSkill::RemoveSkillChip );
	send();
}

void cCliGame::SendUseDigimonMemorySkill( u4 nDigimonUID, u1 nEvoStep, u4 nSkillCode, u4 nTargetUID )
{
	newp( pSkill::UseSkillChip );
	push( nDigimonUID );	// 스킬 사용 디지몬 UID
	push( nEvoStep );		// 스킬 사용 디지몬 진화 단계
	push( nSkillCode );		// 스킬 코드
	push( nTargetUID );		// 타겟 UID
	endp( pSkill::UseSkillChip );
	send();
}

// 아이템에서 디지몬으로
void cCliGame::SendSpirit_ItemToDigimon( int nModelID, std::wstring const& szName, int nNpcID )
{
	newp( pDigimon::SpiritToDigimon );
	push( nModelID );
	push( szName );			// 새 용병의 이름
	push( nNpcID );
	endp( pDigimon::SpiritToDigimon );
	send();
}

// 디지몬에서 아이템으로
void cCliGame::SendSpirit_DigimonToSpirit( u1 nDelSlot, char const* pass2, int nNpcID )
{
	xstop1(nDelSlot<nLimit::DigimonBaseSlot, "nDelSlot:%d error", nDelSlot);
	xstop(nDelSlot>0, "nDelSlot:0 현재 선택중인 디지몬 제거 불가");
	SAFE_POINTER_RET( pass2 );

	newp( pDigimon::DigimonToSpirit );
	push( nDelSlot );

	std::string strPass(pass2);
	if( GLOBALDATA_ST.Is2ndPassEncry() )
	{
		char LastPass[1024] = {0,};
		md5( const_cast<char*>(strPass.c_str()), LastPass );
		strPass = LastPass;
	}

	push(strPass);	

	push( (u4)nNpcID );
	endp( pDigimon::DigimonToSpirit );
	send();
}

// 디지몬에서 디지몬으로
void cCliGame::SendSpirit_AncientSpiritEvolution( u1 nDelSlot, u1 nDelSlot2, char* pass2, TCHAR* szName, int nNpcID )
{
	xstop1(nDelSlot<nLimit::DigimonBaseSlot, "nDelSlot:%d error", nDelSlot);
	xstop(nDelSlot>0, "nDelSlot:0 현재 선택중인 디지몬 제거 불가");

	xstop1(nDelSlot2<nLimit::DigimonBaseSlot, "nDelSlot2:%d error", nDelSlot2);
	xstop(nDelSlot2>0, "nDelSlot2:0 현재 선택중인 디지몬 제거 불가");

	SAFE_POINTER_RET( szName );
	SAFE_POINTER_RET( pass2 );

	newp( pDigimon::AncientSpiritEvolution );
	push( nDelSlot );
	push( nDelSlot2 );

	std::string strPass(pass2);
	if( GLOBALDATA_ST.Is2ndPassEncry() )
	{
		char LastPass[1024] = {0,};
		md5( const_cast<char*>(strPass.c_str()), LastPass );
		strPass = LastPass;
	}

	push(strPass);	

	std::wstring wstrName(szName);
	push( wstrName );			// 새 용병의 이름
	push( (u4)nNpcID );
	endp( pDigimon::AncientSpiritEvolution );
	send();
}

void cCliGame::SendDigimonDataChange( u4 nNPCIDX, nDigimon::eDataChangeType eType, u1 nSrcArrIDX, u1 nDstArrIDX )
{
	newp( pDigimon::DigimonDataChange );
	push( nNPCIDX );
	push( (n4)eType );	
	push( nSrcArrIDX );
	push( nDstArrIDX );	
	endp( pDigimon::DigimonDataChange );
	send();
}

void cCliGame::SendItemActive(u2 invenPos)
{
	newp(pItem::UseActive);
	push(invenPos); 
	endp(pItem::UseActive);
	send();
} 

void cCliGame::SendViewEventSeen()
{
	newp( pGame::ViewEventSeen );		
	endp( pGame::ViewEventSeen );
	send();	
}

//////////////////////////////////////////////////////////////////////////
// 아레나 랭킹 관련 Send 함수
//////////////////////////////////////////////////////////////////////////

void cCliGame::Send_ArenaRequestRank(unsigned char const& nRequestRankType)
{
	C2GS_RECV_ARENA_REQUEST_RANK packet;
	packet.nType = nRequestRankType;
	newp( packet.GetProtocol() );
	push( packet.nType );
	endp( packet.GetProtocol() );
	send();
	BHPRT( "Send - C2GS_RECV_ARENA_REQUEST_RANK : %d", nRequestRankType );
}

void cCliGame::Send_ArenaRequestOldRank(unsigned char const& nRequestRankType)
{
	C2GS_RECV_ARENA_REQUEST_OLD_RANK packet;
	packet.nType = nRequestRankType;
	newp( packet.GetProtocol() );
	push( packet.nType );
	endp( packet.GetProtocol() );
	send();
	BHPRT( "Send - C2GS_RECV_ARENA_REQUEST_OLD_RANK : %d", nRequestRankType );
}

void cCliGame::Send_ArenaRequestFame(unsigned short const& nRequestSeasonIdx)
{
	C2GS_RECV_ARENA_REQUEST_FAME packet;
	packet.nSeason = nRequestSeasonIdx;//요청할 시즌 번호 , 0을넣을경우 최신시즌으로 요청
	newp( packet.GetProtocol() );
	push( packet.nSeason );
	endp( packet.GetProtocol() );
	send();
	BHPRT( "Send - C2GS_RECV_ARENA_REQUEST_FAME : %d", nRequestSeasonIdx );
}

void cCliGame::Send_ArenaRequestFameSeasonList()
{
	C2GS_RECV_ARENA_REQUEST_FAME_LIST packet;
	newp( packet.GetProtocol() );
	endp( packet.GetProtocol() );
	send();
	BHPRT( "Send - C2GS_RECV_ARENA_REQUEST_FAME_LIST" );
}

void cCliGame::Send_VipMemberShipData()
{
#ifdef SDM_VIP_SYSTEM_20181105
	C2GS_REQ_VIP_SYSTEM_DATA packet;
	newp( packet.GetProtocol() );
	endp( packet.GetProtocol() );
	send();
#endif
}

void cCliGame::Send_ItemMake( DWORD const& dwNpcIDX, int const& nMakeCount, int const& nMakeItemIDX, DWORD const& dwRateItemIDX, DWORD const& dwProtectItemIDX )
{
	C2GS_SEND_ITEM_PRODUCTION packet;
	packet.nNpcIdx = dwNpcIDX;
	packet.nMakeIDX = nMakeItemIDX;
	packet.nCount = nMakeCount;
	packet.nRateItem = dwRateItemIDX;
	packet.nProtectItem = dwProtectItemIDX;

	newp( packet.GetProtocol() );
	push( packet.nNpcIdx );			//NPCIdx
	push( packet.nMakeIDX );		//제작IDX
	push( packet.nCount );			//몇번 시도
	push( packet.nRateItem );		//확률보정할경우 아이템IDX
	push( packet.nProtectItem );	//아이템 보호할경우 아이템IDX
	endp( packet.GetProtocol() );
	send();
}

// 초월 관련 패킷
void cCliGame::SendDigimonTranscendenceChargeExp(u1 const& nIsVip, u4 const& invenSlotIdx, DWORD const& dwNpcIDX, u1 const& nChargeType
												 , u1 const& nChargeDigimonTaticsIdx, std::list<u2> const& listMaterialDigimons
												 , std::list<stTranscendenceItem> const& nSubMaterialItems )
{
#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
	C2GS_SEND_DigimonTranscendenceChargeEXP packet;
	packet.m_u1VIPMemberMode = nIsVip;           // VIP Mode
	packet.m_uInvenActiveItemSlot = invenSlotIdx;
	packet.m_uNpcIDX = dwNpcIDX;                   // NPC
	packet.m_u1Flag = nChargeType;                    // 선택 (일반 충전, 고급 충전)
	packet.m_u1DigimonTranscendencePos = nChargeDigimonTaticsIdx; // 초월하는 디지몬 슬롯 번호 0, 1, 2, 3
	packet.m_u2DigimonCount = (u2)listMaterialDigimons.size();            // 초월 재료 디지몬 수
	packet.m_listDigimonPos = listMaterialDigimons; // 초월 재료 디지몬 위치
	packet.m_u2ItemCount = (u2)nSubMaterialItems.size();               // 초월 재료 아이템 수
	packet.m_lItemList = nSubMaterialItems;	

	newp( packet.GetProtocol() );
#ifdef SDM_VIP_SYSTEM_20181105
	push( packet.m_u1VIPMemberMode );
#endif
#ifdef ITEM_USE_TIME_PASS
	push( packet.m_uInvenActiveItemSlot );
#endif
	push( packet.m_uNpcIDX );
	push( packet.m_u1Flag );
	push( packet.m_u1DigimonTranscendencePos );

	push( packet.m_u2DigimonCount );	
	std::list<u2>::const_iterator it = listMaterialDigimons.begin();
	for( ; it != listMaterialDigimons.end(); ++it )
		push( (*it) );

	push( packet.m_u2ItemCount );
	std::list<stTranscendenceItem>::const_iterator it2 = packet.m_lItemList.begin();
	for( ; it2 != packet.m_lItemList.end(); ++it2 )
		push( (*it2) );

	endp( packet.GetProtocol() );
	send();
#endif
}

void cCliGame::SendDigimonTranscendence(u1 const& nIsVip, u4 const& invenSlotIdx, DWORD const& dwNpcIDX, u1 const& nDigimonTaticsIdx,u8 const& nNeedMoney)
{
#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
	C2GS_SEND_DigimonTranscendence packet;
	packet.m_u1VIPMemberMode = nIsVip;
	packet.m_uInvenActiveItemSlot = invenSlotIdx;
	packet.m_uNpcIDX = dwNpcIDX;
	packet.m_u1DigimonTranscendencePos = nDigimonTaticsIdx;
	packet.m_n8Money = nNeedMoney;

	newp( packet.GetProtocol() );
#ifdef SDM_VIP_SYSTEM_20181105
	push( packet.m_u1VIPMemberMode );
#endif
#ifdef ITEM_USE_TIME_PASS
	push( packet.m_uInvenActiveItemSlot );
#endif
	push( packet.m_uNpcIDX );
	push( packet.m_u1DigimonTranscendencePos );
	push( packet.m_n8Money );
	endp( packet.GetProtocol() );
	send();
#endif
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void cCliGame::NTF_Send_nProtect_BotDetect( UINT unUniqueIdx, BYTE* byBotPacket, DWORD dwSize)
{
#ifdef DEF_CORE_NPROTECT
	// 	bool bSendBlock = cClient::IsSendBlock();
	// 	cClient::SetSendBlock(false);
	// 
	// 	BYTE bPacket[4096] = {0,};
	// 	memcpy(bPacket, byBotPacket, dwSize*sizeof(BYTE));
	// 
	// 	newp( pGameTwo::GameGuardCheck );
	//	push( unUniqueIdx );
	// 	push( bPacket,4096 );
	// 	push( dwSize );
	// 	endp( pGameTwo::GameGuardCheck );
	// 	send();
	// 
	// 	cClient::SetSendBlock( bSendBlock );	
#endif
}

void cCliGame::ACK_Send_nProtect( UINT unUniqueIdx, BYTE* byPacket, DWORD dwSize )
{
#ifdef DEF_CORE_NPROTECT
	bool bSendBlock = cClient::IsSendBlock();
	cClient::SetSendBlock(false);

	BYTE bPacket[4096] = {0,};
	memcpy(bPacket, byPacket, dwSize*sizeof(BYTE));

	newp( pGameTwo::GameGuardCheck );
	push( unUniqueIdx );
	push( bPacket,4096 );
	push( dwSize );
	endp( pGameTwo::GameGuardCheck );
	send();

	cClient::SetSendBlock( bSendBlock );	
#endif
}

void cCliGame::NTF_Send_Xigncode( char* pBuffer, SIZE_T BufferSize )
{
#ifdef SDM_DEF_XIGNCODE3_20181107
	bool bSendBlock = cClient::IsSendBlock();
	cClient::SetSendBlock(false);

	C2GS_SEND_XignCodePacket sendPacket;
	sendPacket.nSize = BufferSize;
	memcpy( sendPacket.szPacketData, pBuffer, BufferSize );

	newp( sendPacket.GetProtocol() );
	push( sendPacket.szPacketData, BufferSize );
	push( sendPacket.nSize );
	endp( sendPacket.GetProtocol() );
	send();
	cClient::SetSendBlock( bSendBlock );
#endif
}

void cCliGame::SendItemSort(u1 eSortFlag)
{
	C2GS_SEND_ITEMSORT sendPacket;
	sendPacket.m_u1SortFlag = eSortFlag;

	newp( sendPacket.GetProtocol() );
	push( sendPacket.m_u1SortFlag );
	endp( sendPacket.GetProtocol() );
	send();
}

void cCliGame::SendChangeDigimonEffect(u2 nItemPos, u4 nItemType, n2 nItemCount)
{
#if 0 // our common lib doesn't handle it
	C2GS_SEND_DIGIMON_EFFECT_CHANGE packet;
	packet.nItemPos = nItemPos;
	packet.nItemType = nItemType;
	packet.nItemCount = nItemCount;

	newp( packet.GetProtocol() );
	push( packet.nItemPos );
	push( packet.nItemType );
	push( packet.nItemCount );
	endp( packet.GetProtocol() );
	send();
#endif
}
