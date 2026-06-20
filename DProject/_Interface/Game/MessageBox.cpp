#include "stdafx.h"
#include "MessageBox.h"
#include <set>

// Race-handler: the cMessageBox constructor can take ~9ms (texture/font setup) and
// internally hits an alertable wait that lets the network APC for a server response
// run on the same thread. That can fire DelMsg(<type>) BEFORE the constructor's
// push_back completes, so without this the box is inserted AFTER its dismissal and
// stays forever. When DelMsg can't find a box of the given type, we record the type
// here; InsertList drops the just-built box if its type is in this set.
static std::set<int> g_pendingDismissals;

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

#include <shellapi.h>


std::list< cMessageBox* >	cMessageBox::g_MessageBoxList;

static bool MessageBoxListContains(cMessageBox* pBox)
{
	if (pBox == NULL)
		return false;

	std::list< cMessageBox* >::iterator it = cMessageBox::g_MessageBoxList.begin();
	std::list< cMessageBox* >::iterator itEnd = cMessageBox::g_MessageBoxList.end();

	for (; it != itEnd; ++it)
	{
		if ((*it) == pBox)
			return true;
	}

	return false;
}

static void MessageBoxListErasePointer(cMessageBox* pBox)
{
	if (pBox == NULL)
		return;

	std::list< cMessageBox* >::iterator it = cMessageBox::g_MessageBoxList.begin();
	std::list< cMessageBox* >::iterator itEnd = cMessageBox::g_MessageBoxList.end();

	while (it != itEnd)
	{
		if ((*it) == pBox)
			it = cMessageBox::g_MessageBoxList.erase(it);
		else
			++it;
	}
}

void cMessageBox::DeleteList(bool bCalEndMessage)
{
	if (g_MessageBoxList.empty())
		return;

	std::list< cMessageBox* > deleteList;
	deleteList.swap(g_MessageBoxList);

	std::list< cMessageBox* >::iterator it = deleteList.begin();
	std::list< cMessageBox* >::iterator itEnd = deleteList.end();

	for (; it != itEnd; ++it)
	{
		cMessageBox* pMsg = (*it);

		if (pMsg == NULL)
			continue;

		if (bCalEndMessage)
			pMsg->_Cancel(false);

		NiDelete pMsg;
	}

	deleteList.clear();
}

bool cMessageBox::DelMsg(int nMsgType, bool bCallEndMsg)
{
	bool bReturn = false;

	std::list< cMessageBox* >::iterator it = g_MessageBoxList.begin();

	while (it != g_MessageBoxList.end())
	{
		cMessageBox* pMsg = (*it);

		if (pMsg == NULL)
		{
			it = g_MessageBoxList.erase(it);
			continue;
		}

		if (pMsg->GetMessageType() == nMsgType)
		{
			it = g_MessageBoxList.erase(it);

			if (bCallEndMsg == true)
				pMsg->_Cancel(false);

			NiDelete pMsg;
			bReturn = true;
		}
		else
		{
			++it;
		}
	}

	if (!bReturn)
		g_pendingDismissals.insert(nMsgType);

	return bReturn;
}

void cMessageBox::InsertList( TCHAR const* szTitle, TCHAR const* szBody, int nMsgType, int nBoxType )
{
	if( g_MessageBoxList.size() )
	{
		// 이미 버젼 에러라면 리턴
		switch( GetFirstMessageBox()->GetFristType() )
		{
		case cPrintMsg::SERVER_VERSION_ERROR:
			return;
		}
	}

	//DeleteList( true );

	// 앞열 싹 날려주자
	switch( nMsgType )
	{
	case cPrintMsg::USE_MANUAL_MESSAGE_DISCONNECT:	// 게임가드에서 에러 메시지 출력시
	case cPrintMsg::SERVER_VERSION_ERROR:
	case cPrintMsg::SERVER_DISCONNET:
	case cPrintMsg::GAME_DEAD:
	case 30417:
	case 20017:	// 디지몬 삭제(해제) 시 메시지 창 닫기.	
	case 30018:	// 거래중 모든 메시지 창 닫기	
	case 10100:	// 서버 이전 완료 
		{
			DeleteList( true );
		}
		break;
	}

	switch( nMsgType )
	{
	case cPrintMsg::RECOMMENT_ANSWER_CONFIRM:
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::SEND_RECOMMEND_ADD_ANSWER_INFO );
		break;
	}

	cMessageBox* pMsg = NiNew cMessageBox( szTitle, szBody, nMsgType, nBoxType, g_MessageBoxList.size() );

	// Race-handler: if DelMsg for this type fired while the constructor above was
	// running, the dismissal got queued in g_pendingDismissals. Drop this box now
	// so it doesn't end up in the list with no one to remove it.
	std::set<int>::iterator pendIt = g_pendingDismissals.find( nMsgType );
	if( pendIt != g_pendingDismissals.end() )
	{
		g_pendingDismissals.erase( pendIt );
		NiDelete pMsg;
		return;
	}

	g_MessageBoxList.push_back( pMsg );
		
// 	switch( nMsgType )
// 	{
// 	case cPrintMsg::SERVER_VERSION_ERROR:
// 	case cPrintMsg::SERVER_DISCONNET:
// 	case 30417:
// 		{
// 			FLOWMGR_ST.ChangePopAllFlow(Flow::CFlow::FLW_LOGIN);
// 			//SetNextGameStep(GAME_LOGOUT);
// 		}		
// 		break;
// 	}
}

bool cMessageBox::UpdateList()
{
	if (g_MessageBoxList.empty() == true)
		return false;

	cMessageBox* pMsg = GetFirstMessageBox();

	if (pMsg == NULL)
	{
		if (!g_MessageBoxList.empty())
			g_MessageBoxList.pop_back();

		return false;
	}

	eSTATE eBeforeState = pMsg->GetState();

	switch (eBeforeState)
	{
	case MBOX_NONE:
		pMsg->Update(g_fDeltaTime);

		if (GetMessageBox(pMsg->GetMessageType()) != pMsg)
			return true;

		pMsg->Update_ForMouse();

		if (GetMessageBox(pMsg->GetMessageType()) != pMsg)
			return true;
		break;
	}

	if (GetMessageBox(pMsg->GetMessageType()) != pMsg)
		return true;

	switch (pMsg->GetState())
	{
	case MBOX_CALL_OK:
	{
		MessageBoxEraseList(pMsg);
		pMsg->_Ok(true);
		NiDelete pMsg;
	}
	break;

	case MBOX_CALL_CANCEL:
	{
		MessageBoxEraseList(pMsg);
		pMsg->_Cancel(true);
		NiDelete pMsg;
	}
	break;
	}

	return true;
}

void cMessageBox::RenderList()
{
	if (g_MessageBoxList.empty())
		return;

	std::list< cMessageBox* > renderList = g_MessageBoxList;

	std::list< cMessageBox* >::iterator it = renderList.begin();
	std::list< cMessageBox* >::iterator itEnd = renderList.end();

	for (; it != itEnd; ++it)
	{
		cMessageBox* pMsg = (*it);

		if (pMsg == NULL)
			continue;

		if (GetMessageBox(pMsg->GetMessageType()) == pMsg)
			pMsg->Render();
	}
}

bool cMessageBox::OnEnterKeyList()
{
	if( g_MessageBoxList.empty() == true )
		return false;

	cMessageBox* pMsg = GetFirstMessageBox();
	switch( pMsg->GetMessageType() )
	{
	case cPrintMsg::LOG_OUT:
	case cPrintMsg::GAME_EXIT:
	case 30019:
	case 20039:						// 게임스캐너 등록
	case 20040:						// 게임스캐너 해제
	case 20041:						// 게임스캐너 해제내용 확인
	case 20061:						// 바코드 시리얼넘버 오류
	case 30180:						// 테이머 변경
	case 10019:						// 서버 응답 대기
	case 14010:						// 서버 응답 대기
	case 10100:
	case cPrintMsg::GAME_ITEM_DELETE:// 아이템 삭제
		return true;
	}

	pMsg->_Ok( true );
	if( pMsg->ExistOkBtnControl() )
		pMsg->GetOkBtnControl()->PlaySound();
	else if( pMsg->ExistCancelBtnControl() )
		pMsg->GetCancelBtnControl()->PlaySound();
		
	MessageBoxEraseList( pMsg );
	NiDelete pMsg;	
	return true;
}

bool cMessageBox::OnEscapeKeyList()
{
	if( g_MessageBoxList.empty() == true )
		return false;

	cMessageBox* pMsg = GetFirstMessageBox();

	switch( pMsg->GetMessageType() )
	{
	case 20039:						// 게임스캐너 등록
	case 20040:						// 게임스캐너 해제
	case 20041:						// 게임스캐너 해제내용 확인
	case 20061:						// 바코드 시리얼넘버 오류
	case 10019:						// 서버 응답 대기
	case 14010:						// 서버 응답 대기
	case 10100:
	case cPrintMsg::USE_MANUAL_MESSAGE_DISCONNECT:
		return true;
	}

	pMsg->_Cancel( true );
	if( pMsg->ExistCancelBtnControl() )
		pMsg->GetCancelBtnControl()->PlaySound();
	else if( pMsg->ExistOkBtnControl() )
		pMsg->GetOkBtnControl()->PlaySound();

	MessageBoxEraseList( pMsg );
	NiDelete pMsg;	
	return true;
}


bool cMessageBox::OnLButtonUpList( CsPoint pos )
{
	if( g_MessageBoxList.empty() == true )
		return false;

	cMessageBox* pMsg = GetFirstMessageBox();

	// 마우스 외부 처리라 화면안에 들어 온거느 패스
	if( pMsg->IsInWindow( pos ) == true )
		return true;

	if( ( pMsg->ExistOkBtnControl() == false )&&( pMsg->ExistCancelBtnControl() == false ) )
		return true;

	switch( pMsg->GetMessageType() )
	{
	case 30096:						// 인던 입장
	case 30065:						// 파티 초대
	case 10017:						// 채널이동
	case 30018:						// 다른유저가 거래 요청
	case 30019:						// 거래 요청 후 대기중
	case 20017:						// 용병 삭제
	case 30026:						// Npc찾기
	case 20029:						// 사용 기간 만료
	case 20038:						// 게임스캐너 디라이버 미 설치
	case cPrintMsg::GAME_DEAD:
	case cPrintMsg::LOG_OUT:
	case cPrintMsg::GAME_EXIT:
	case 20039:						// 게임스캐너 등록
	case 20040:						// 게임스캐너 해제
	case 20041:						// 게임스캐너 해제내용 확인
	case 30126:						// 길드 가입요청
	case 30135:						// 길드 폐쇠 경고
	case 30136:						// 길드 탈퇴
	case 30145:						// 길드 강제탈퇴
	case 30151:						// 마스터 권한넘기기
	case 30152:						// 길드 폐쇠
	case 30154:						// 길드 생성
	case 30160:						// 크기 재조정
	case 30161:						// 크기 재조정 확인
	case 20061:						// 바코드 시리얼넘버 오류
	case 30180:						// 테이머 변경
	case 30045:						// 친구 삭제
	case 30046:						// 차단 삭제
	case 30443:						// 셧다운제도
	case 30485:						// 결투신청
	case cPrintMsg::RECOMMENT_ANSWER_CONFIRM:						// 친구초대
	case 30550:						// 테이머 소환
	case 30549:						// 테이머 소환 요청중
	case 30701:
		return true;
	}

	if( pMsg->ExistCancelBtnControl() )
	{
		pMsg->_Cancel( true );
		pMsg->GetCancelBtnControl()->PlaySound();

		MessageBoxEraseList( pMsg );
		NiDelete pMsg;		
	}
	else
	{
		pMsg->_Ok( true );
		if( pMsg->ExistOkBtnControl() )
			pMsg->GetOkBtnControl()->PlaySound();

		MessageBoxEraseList( pMsg );
		NiDelete pMsg;		
	}
	return true;
}

bool cMessageBox::OnRButtonUpList( CsPoint pos )
{
	return OnLButtonUpList( pos );
}

void cMessageBox::ResetDeviceList()
{
	if (g_MessageBoxList.empty())
		return;

	std::list< cMessageBox* > resetList = g_MessageBoxList;

	std::list< cMessageBox* >::iterator it = resetList.begin();
	std::list< cMessageBox* >::iterator itEnd = resetList.end();

	for (; it != itEnd; ++it)
	{
		cMessageBox* pMsg = (*it);

		if (pMsg == NULL)
			continue;

		if (GetMessageBox(pMsg->GetMessageType()) == pMsg)
			pMsg->ResetDevice();
	}
}

bool cMessageBox::IsMessageBox( int nMsgType )
{
	std::list< cMessageBox* >::iterator it = g_MessageBoxList.begin();
	std::list< cMessageBox* >::iterator itEnd = g_MessageBoxList.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetMessageType() == nMsgType )
			return true;
	}
	return false;
}

cMessageBox* cMessageBox::GetFirstMessageBox()
{
	while (!g_MessageBoxList.empty())
	{
		cMessageBox* pMsg = g_MessageBoxList.back();

		if (pMsg != NULL)
			return pMsg;

		g_MessageBoxList.pop_back();
	}

	return NULL;
}

cMessageBox* cMessageBox::GetMessageBox( int nMsgType )
{
	std::list< cMessageBox* >::iterator it = g_MessageBoxList.begin();
	std::list< cMessageBox* >::iterator itEnd = g_MessageBoxList.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetMessageType() == nMsgType )
		{
			return (*it);
		}
	}
	return NULL;
}

void cMessageBox::MessageBoxEraseList(cMessageBox* pBox)
{
	if (pBox == NULL)
		return;

	std::list< cMessageBox* >::iterator it = g_MessageBoxList.begin();

	while (it != g_MessageBoxList.end())
	{
		if ((*it) == pBox)
			it = g_MessageBoxList.erase(it);
		else
			++it;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
cMessageBox::cMessageBox( TCHAR const* szTitle, TCHAR const* szBody, int nMsgType, int nBoxType, int nMessageListIndex )
:m_pBody(NULL), m_eMsgType( nMsgType ),m_eState( MBOX_NONE ), m_pValuePointer1(NULL),m_bCallProc(false),
m_ButtonOk(NULL),m_ButtonCancel(NULL),m_pProgress(NULL),m_fValue(0.0f),m_nValue1(0),m_nValue2(0),m_nValue3(0),
m_unValue1(0)
{
	switch( m_eMsgType )
	{
	case cPrintMsg::LOG_OUT:
	case cPrintMsg::GAME_EXIT:
		m_fValue = 5.0f;
		m_nValue1 = -1;
		break;
	case 30018: // 상대의 거래요청
		m_fValue = 30.0f;
		break;
	case 30096:			// 인던 메시지박스
	case 30065:			// 상대방의 파티 요청
	case 30126:			// 길드 가입요청
#ifdef GUILD_RENEWAL
	case 30209:			// 길드 소환 수락 여부
#endif
	case 30485:			// 결투 신청
	
	case cPrintMsg::RECOMMENT_ANSWER_CONFIRM:
	case 30550:			// 테이머 소환 요청
		m_fValue = 10.0f;
		break;
	}

	Init( szTitle, szBody, nBoxType, nMessageListIndex );

}

void cMessageBox::_Ok( bool bAbsolute )
{
	std::pair<int, int> value(0,0);
	// NULL 이면 OK는 없다
	if( ( bAbsolute == false )&&( m_ButtonOk == NULL ) )
	{
		_Cancel( true );
		return;
	}

	if( m_bCallProc == true )
		return;
	m_bCallProc = true;

	switch( m_eMsgType )
	{
	case 30410:		// 튜토리얼 스킵
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::TUTORIALQUEST_SKIP_ACCEPT, NULL );
		}
		break;
#ifdef LJW_TUTORIAL_RENEWAL_20191128
	case cPrintMsg::TUTORIAL_EVENT_SKIP:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SKIP_TUTORIAL_EVENT );
		break;
#endif
	case cPrintMsg::LOGIN_HTTP_SERVER_ERROR:
	case cPrintMsg::USE_MANUAL_MESSAGE://Aeria는 로그인창 없으니까 에러메세지 뜨고나서 꺼야해요
		PostQuitMessage(0);//	SetNextGameStep(GAME_EXIT);
		break;
	case cPrintMsg::USE_MANUAL_MESSAGE_DISCONNECT:
	case 10100:
		PostQuitMessage(0);//SetNextGameStep(GAME_EXIT);
		break;

	case cPrintMsg::USE_MANUAL_MESSAGE_URL:
		{
			ShellExecute(NULL, _T( "open" ), _T( "iexplore" ), m_szValue1, NULL, SW_SHOWNORMAL );
			if( GLOBALDATA_ST.IsLoginSkiped() )
				PostQuitMessage(0);//SetNextGameStep(GAME_EXIT);//Aeria는 로그인창 없으니까 에러메세지 뜨고나서 꺼야해요
		}
		
		break;
	case cPrintMsg::LOGIN_ID_NONE_EXIST:
		{
			if( GLOBALDATA_ST.IsLoginSkiped() )
			{
				CsMessageBox( MB_OK, L"Login ID none exist");
				PostQuitMessage(0);//SetNextGameStep(GAME_EXIT);
			}
		}
		return;

	case 30375:						// 개인상점 판매 종료
		GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_PERSONSTORE_MSGBOX, NULL );
		return;
	case 30180:						// 테이머 변경
		{
			std::pair<int, int> pairData( m_nValue1, m_nValue2 );
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_CHANGETAMER_CHANGE, &pairData );
		}
		return;
	case 20061:						// 바코드 시리얼넘버 오류
		{
			ShellExecute(NULL, _T( "open" ), _T( "explorer.exe" ), _T( "http://www.daewonstyle.com" ), NULL, SW_SHOW );
		}
		return;
	case 30175:						// 귀속 아이템 사용해서 귀속아이템 승락여부
		{
			if( net::game )
				net::game->SendItemMove( m_nValue1, m_nValue2, false );
		}
		return;
	case 32000:
		{
			if( net::game )
				net::game->SendItemMove( m_nValue1, m_nValue2 );
		}
		return;
	case 30176:
	case 30177:
		{
#ifndef ITEM_USE_TIME_PASS
			return;
#endif

			if( net::game )
			{
				cItemInfo* pItem = g_pDataMng->GetInven()->GetData( m_nValue1 );
				int nItemCount = pItem->GetCount();

				if(nItemCount == 1)
				{
					net::game->SendItemActive( m_nValue1 );	
				}
				else
				{
					int nEmptyCnt = g_pDataMng->GetInven()->GetEmptySlotCount();	// 인벤토리 빈슬롯 체크

					if(nEmptyCnt !=0)
					{
						int nEmptySlot = g_pDataMng->GetInven()->FindEmptySlot(-1);

						g_pDataMng->SendItemMove( m_nValue1, TO_INVEN_SID( nEmptySlot ), 1 );

						net::game->SendItemActive( nEmptySlot );	
					}
					else
					{
						cPrintMsg::PrintMsg( 11015 );	// 인벤토리에 여유 공간이 부족합니다.
					}
				}
			}			
		}	
		return;
		
#ifdef VERSION_USA
	case 20079:						// 용병알 스캔
		GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_SCAN_START, NULL );
		return;
#endif
	case 20080:						// 케쉬 용병알 스캔
		GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_SCAN_START, NULL );
		return;
	case 20081:						// 케쉬 용병알 회수
		GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_SELL_START, NULL );
		return;

#ifdef UI_ITEMSCAN_RENEWAL
	case 20099:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_ALLSCAN, NULL );
		return;
#endif

	case 20066:						// 돈 지불해야 하는 곳으로 포탈 이용
		//g_pCharMng->GetTamerUser()->SetPortal( m_nValue1 );
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PORTAL_MOVE, &m_nValue1 );
		return;

	case 20038:						// 드라이버 설치 사이트로 이동
		{
			//ShellExecute(NULL, _T( "open" ), _T( "explorer.exe" ), _T( "http://www.digimonmasters.com/down/main.aspx?" ), NULL, SW_SHOW );
		}
		return;
	case 30055:						// 점프 부스터
		{
			bool bIsBattleRegisted = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_BATTLEMATCH_REGISTED, &bIsBattleRegisted );
			if( bIsBattleRegisted )// 배틀 신청중 인경우 맵이동 불가
			{
				cPrintMsg::PrintMsg( 40013 );	// 배틀 신청중 인경우 맵이동 불가
				return;
			}
			
			if( net::game )
			{
#ifdef SDM_VIP_SYSTEM_20181105
				if( -1 == m_nValue1 )
					net::game->SendJumpGate( 0, m_nValue2, true );
				else
					net::game->SendJumpGate( m_nValue1, m_nValue2 );
#else
				net::game->SendJumpGate( m_nValue1, m_nValue2 );
#endif
			}
		}
		return;
	case 30544:
		{
			cSkill* pSkillMng = g_pCharMng->GetTamerUser()->GetSkillMng();
			if( pSkillMng->GetSkill_Inex() == -1 )
			{
				cPrintMsg::PrintMsg( 30586 );
				return;
			}

			bool bIsBattleRegisted = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_BATTLEMATCH_REGISTED, &bIsBattleRegisted );
			if( bIsBattleRegisted )// 배틀 신청중 인경우 맵이동 불가
			{
				cPrintMsg::PrintMsg( 40013 );	// 배틀 신청중 인경우 맵이동 불가
				return;
			}

			if( net::game )
			{
				net::game->SendTamerActiveSkill( pSkillMng->GetSkill_Inex() );
				net::game->SendTamerJumpGate( 9801, m_nValue2 );		//아이템 번호, 맵번호
			}
		}
		return;
	case 30054:						// 케쉬 용병알 4단계 이상 도전
		{
			if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) )
			{
				g_pGameIF->GetMakeTactics()->SendServer();
				g_pGameIF->GetMakeTactics()->GetDeleteBackupFunction();
			}
		}
		return;
	case 30050:						// 용병 보관소에 보관
		{
			int AccompanySlotIdx = m_nValue1;
			int ArchiveSlotIdx = m_nValue2;
			int nCost = m_nValue3;
			
			CDigimonArchiveContents::ChangeData kData;
			kData.accompanyslotidx = m_nValue1;
			kData.archiveslotidx = m_nValue2;
			kData.changecost = m_nValue3;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_CHANGE_DIGIMON, &kData);
		}
		return;
		
	case 30456:						// 버프 사용.
	case 20021:						// 케쉬 아이템 사용
		{
			if( m_nValue3 != 0 )
			{
				g_pDataMng->GetServerSync()->SendServer( (cData_ServerSync::eSYNC)m_nValue3, 0 );
			}
			g_pDataMng->ItemLock( TO_INVEN_SID( m_nValue1 ) );
			if( net::game )
			{
				net::game->SendItemUse( m_nValue2, m_nValue1 );
			}
		}
		return;
	case 20022:						// 케쉬 캡슐 코인 아이템 사용
		{
			CsC_AvObject* pNpc = (CsC_AvObject*)m_pValuePointer1;
			pNpc->SetAnimation( ANI::NPC_YES );
			CsC_AvObject* pTamer = g_pCharMng->GetTamerUser();
			pTamer->SetPauseAll( true );

			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CASH_CAPSULE_COIN_COMPLEATE, 0 );
			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CASH_CAPSULE_COIN_WAIT, 0 );
			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CASH_CAPSULE_COIN_CLIENT, 0 );			

			g_pDataMng->ItemAllLock();
			if( net::game )
			{
				g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CASH_CAPSULE_COIN_SERVER, 0 );
				net::game->SendCapsuleToItem( pNpc->GetFTID(), m_nValue1 );
			}
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TALK );
		}
		return;
	case 30300:
		{
			// 디지몬 스킬 초기화 아이템 사용
			g_pDataMng->ItemLock( TO_INVEN_SID( m_nValue1 ) );
			if( net::game )
			{
				net::game->SendItemUse( m_nValue2, m_nValue1 );
			}
		}
		return;
	case 20090:						// 케쉬샾 이동
		{
			std::wstring cashShopWebUrl = GLOBALDATA_ST.GetCashShopWebUrl();
			if( !cashShopWebUrl.empty() )
				ShellExecute(NULL, _T( "open" ), _T( "explorer.exe" ), cashShopWebUrl.c_str(), NULL, SW_SHOW );			
		}
		return;
#ifdef VERSION_USA
	case 30600:						// 에보류터로 진화 슬롯 확장 확인
		{
			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::DIGIMON_EVOLUTION_OPEN, 0, &m_nValue1, &m_nValue2, &m_nValue3 );			
		}
		return;
#endif
	case 30034:						// 에볼류터로 진화 슬롯 확장 확인
		{
			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::DIGIMON_EVOLUTION_OPEN, 0, &m_nValue1, &m_nValue2, &m_nValue3 );			
		}
		return;

	case 30041:						// 라이딩 슬롯 확장
		{
			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::DIGIMON_RIDE_OPEN, 0, &m_nValue1, &m_nValue2, &m_nValue3 );			
		}
		break;

	case 10017:						// 채널이동 승인
		{
			GAME_EVENT_ST.OnEvent(EVENT_CODE::SEND_CHANGE_CHANNEL, &m_nValue1 );
		}
		return;
	case 30065:						// 상태의 파티 요청 승인
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_INVITE_ANSWER_ACCEPT );
		}
		return;
	case 30018:						// 상대의 거래요청 승인						
		{
			std::pair<bool, unsigned int> result;
			result.first = true;
			result.second = m_nValue1;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_TRADE_REQUEST_RESULT, &result );
		}			
		return;

	case 30026:						// Npc찾기
		assert_cs( g_pGameIF );
		assert_cs( g_pCharMng );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_NPCFIND );
		g_pCharMng->GetDigimonUser( 0 )->SetDigimonUserState( CDigimonUser::DU_NPCFIND, &m_nValue1 );
		return;

	case 30113:						// 위치찾기
		g_pHelpArrow->ReleaseArrowPoint();
		return;

	case 20051:						// 2차비밀번호 사용
		//g_pSecondPassword->Open( cSecondPassword::eSet, cSecondPassword::ePassRegist );
		return;

	case 30116:						// 상점으로 이동
		{
			std::wstring cashPage = GLOBALDATA_ST.GetWebCashPageUrl();
			if( !cashPage.empty() )
				ShellExecute(NULL, _T( "open" ), _T( "explorer.exe" ), cashPage.c_str(), NULL, SW_SHOW );
		}		
		return;
	case 10051:				// 서버 점검중. 공지사항으로 이동
		{
			std::wstring url = GLOBALDATA_ST.GetNoticeWebUrl();
			if( !url.empty() )
				ShellExecute(NULL, _T( "open" ), _T( "explorer.exe" ), url.c_str(), NULL, SW_SHOW );
			PostQuitMessage(0);//SetNextGameStep(GAME_EXIT);//Aeria는 로그인창 없으니까 메세지 뜨고나서 꺼야해요
		}		
		return;

#ifdef VERSION_TH
	case 10054:
		PostQuitMessage(0);//	g_nGameLoop = GAME_EXIT;
		return;
#endif

	case 30312:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_CASHSHOP_ITEM_PURCHASE, &m_unValue1 );
		}
		return;

	case 30314:
		{
			std::pair<std::wstring, unsigned int> buyItemInfo;
			buyItemInfo.first = m_szValue1;
			buyItemInfo.second = m_unValue1;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_CASHSHOP_ITEM_PRESENT, &buyItemInfo );
		}
		return;

	case cPrintMsg::CHARSELECT_DELETE_CHAR:
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::OPEN_CHARDEL_PASSWORD_WINDOW );		
		//g_pCharacterSelect->InitDeleteChar();
		return;

	case cPrintMsg::GAME_ITEM_DELETE:
		{
			// 개인 상점이 열려 있는 상태에서는 아이템 버리기 불가
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
			{
				cPrintMsg::PrintMsg( 10004 );				
				CURSOR_ST.IconDropCancel();
			}
			else
				CURSOR_ST.IconDropOk();
		}
		return;

	case cPrintMsg::SERVER_VERSION_ERROR:
	case cPrintMsg::SERVER_DISCONNET:
#ifdef IGNORE_DISCONNECT_EXIT			// 종료 시 다시 로그인 창으로 이동됨(KOR- debug, release)
		GLOBALDATA_ST.ResetAccountInfo();
		FLOWMGR_ST.ChangePopAllFlow(Flow::CFlow::FLW_LOGIN);//SetNextGameStep(GAME_LOGOUT);
#else
		PostQuitMessage(0);//SetNextGameStep(GAME_EXIT);
#endif
		return;

	case cPrintMsg::QUEST_DROP_BTN:
		g_pDataMng->GetQuest()->DropProcQuest();
		return;

	case cPrintMsg::LOG_OUT:
		FLOWMGR_ST.ChangePopAllFlow(Flow::CFlow::FLW_SERVERSEL);//SetNextGameStep( GAME_LOGOUT );
		return;

	case cPrintMsg::GAME_EXIT:
	case 30443:						// 셧다운제도
		PostQuitMessage(0);//SetNextGameStep( GAME_EXIT );
		return;

	case cPrintMsg::SAME_MACRO_KEY:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_OPTION_MACRO ) )
			g_pGameIF->GetOptionMacro()->SetBackupKey();
		return;

	case 20004:			// 용병 디지타마 제거
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) )
			g_pGameIF->GetMakeTactics()->DestroyResistDigitama( true );
		return;

	case 20017:			// 용병 삭제	
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQUEST_DELETE_DIGIMON, &m_nValue1 );
		}
		return;

	case 11028:			// 상점에서 아이템 판매
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_STORE ) )
		{
			CsItem::sINFO* pFTItem = g_pDataMng->GetInven()->GetFTData( m_nValue1 );
			if( pFTItem->IsCashType() ) // 캐쉬아이템이면 
			{
				cPrintMsg::PrintMsg( 20082, pFTItem->s_szName );
				int nSellInvenIndex = m_nValue1;
				int nSellItemCount  = m_nValue2;
				cMessageBox::GetFirstMessageBox()->SetValue1( nSellInvenIndex );
				cMessageBox::GetFirstMessageBox()->SetValue2( nSellItemCount );
			}
			else // 일반 아이템이면
			{
				std::pair< int, int > pairSellInfo( m_nValue1, m_nValue2 );
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::SEND_STORE_ITEMSELL, &pairSellInfo );
			}			
		}	
		return;

	case 20082:			// 케쉬 아이템 상점에 판매
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_STORE ) )
		{
			std::pair< int, int > pairSellInfo( m_nValue1, m_nValue2 );
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::SEND_STORE_ITEMSELL, &pairSellInfo );
		}	
		return;

	case 20031:			// 케쉬아이템 찾을때
	case 20032:			// 케쉬아이템 찾을때

		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_CASHSHOP_ITEM_CROP, &m_nValue1 );
// 		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CASH ) )
// 			g_pGameIF->GetCash()->SendCropCashItem( m_nValue1 );
		return;

	case 20085:			// 우편 아이템 찾을때
	case 20086:			// 우편 아이템 찾을때
		{
			GS2C_RECV_REWARD_SEND_TYPE recv;
			recv.iType = 1;
			switch( m_nValue2 )
			{
			case 0:	recv.bSendAll = false;	recv.iIndex = m_nValue1;				break;
			case 1:	recv.bSendAll = true;	recv.iIndex = -1;				break;
			default:
				return;
			}			

			GAME_EVENT_ST.OnEvent( EVENT_CODE::REWARD_SEND_ACCPET_DATA, &recv );

		}
		return;

#ifdef USE_BARCODE_REDER
	case 20039:			// 게임스캐너 등록
		nsBARCODE::SendBarcodeResist();
		return;
	case 20040:			// 게임스캐너 해제
		nsBARCODE::SendBarcodeRelease();
		return;
#endif

	case 30126: //길드 초대
		if( net::game )
		{
			u4 certify = (u4)m_unValue1;
			net::game->SendGuildAllow( m_szValue1, certify );					

			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
			{
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_COMMUNITY );			
			}	
		}
		return;
	case 30135: //길드폐쇠 경고
		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHECK_GUILD_DESTROY, null );
		return;

	case 30152: //길드 폐쇠
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_DESTROY, 0 );
		return;			
	
	case 30136: //길드 탈퇴
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_LEAVE, 0 );
		return;

	case 30145: //길드 강퇴
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_BAN, 0, m_szValue1 );
		return;
		
	case 30151: //마스터 권한 넘기기
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_AP_MASTER, 0, m_szValue1 );
		return;

	case 30154: //길드 생성
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_CREATE, 0, m_szValue1 );	
		return;

	case 30160:		// 크기 재조정
		cPrintMsg::PrintMsg( 30161 );
		cMessageBox::GetFirstMessageBox()->SetValue1( m_nValue1 );
		return;
	case 30161:		// 크기 재조정 확인
		if( net::game )
		{
			// 아이템 롹킹
			g_pDataMng->ItemLock( TO_INVEN_SID( m_nValue1 ) );
			net::game->SendItemUse( g_pCharMng->GetDigimonUser( 0 )->GetUniqID(), m_nValue1 );
		}
		return;

	case 30045:
		{
			std::pair<bool, int> delInfo = pair<bool, int>(true, m_nValue1);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::DELETE_FRIEND_SHUTOUT, &delInfo);
		}
		return;

	case 30046:
		{
			std::pair<bool, int> delInfo = pair<bool, int>(false, m_nValue1);
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::DELETE_FRIEND_SHUTOUT, &delInfo);
		}
		return;

	case 30340:		// 소모성 가속 부스터 중복 사용 확인
		{
			// 아이템 롹킹
			g_pDataMng->ItemLock( TO_INVEN_SID( m_nValue1 ) );
			if( net::game )
				net::game->SendItemUse( m_nValue2, m_nValue1 );
		}
		return;
	case 30326:		// 테이머 신고하기
		{
			unsigned int targetUIDX = m_unValue1;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_MACROPROTECT_QUESTION, &targetUIDX);
// 			if( net::game )
// 				net::game->SendMacroQuestion( m_szValue1 );
		}
		return;
		
	case 30451:
		{
			if( net::game )
			{
				switch( m_nValue1 )
				{
				case RTTI_TAMER_USER: //테이머
					if( g_pCharMng->GetTamerUser()->GetBuff()->IsBuffData( m_nValue2 ) )	//시간 다되고나서 삭제 보내면 서버에서 끊음.. 확인해주자
						net::game->SendManualRemove( 1, m_nValue2 );					
					break;
				case RTTI_DIGIMON_USER: //디지몬
					if( g_pCharMng->GetDigimonUser(0)->GetBuff()->IsBuffData( m_nValue2 ) )	//시간 다되고나서 삭제 보내면 서버에서 끊음.. 확인해주자
						net::game->SendManualRemove( 2, m_nValue2 );					
					break;
				default:
					assert_csm( false, L"잘못된 object타입"  );
					break;
				}
			}				
		}
		return;

	case 30485: //결투신청		
		{
			int nResult = 1;// 승낙
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_DUAL_REQUEST_ANSWER, &nResult );
		}
		return;

	case 30426:		// 강화 초기화
	case 30429:
		{
			g_pGameIF->GetEnchantTactics()->SendWeakServer();
		}
		return;
	case cPrintMsg::RECOMMENT_ANSWER_CONFIRM:
		{
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::SEND_RECOMMEND_ADD_ANSWER_INFO );
			return;
		}
#ifdef MINIGAME
	case 30540:
		{
			int nCount = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_COUNT, &nCount);
			int nMoney = (5000 + 5000*nCount);

			if( nMoney > g_pDataMng->GetInven()->GetMoney().GetInt() )
				cPrintMsg::PrintMsg( 11003 );	// 돈모자르다
			else
				net::game->SendMinigameStart( cMinigame::HATCH, nCount );
		}
		
		return;
#endif
	case 30520:
		if( net::game )
		{			
			net::game->SendGetNatureItem( m_nValue1, m_nValue2 );
		}
		return;
	case 30534:
		{
			// 용병 추가할 공간이 있는가
			if( g_pDataMng->GetTactics()->GetEmptyTactics() == NULL )
			{
				cPrintMsg::PrintMsg( 30535 );
				break;
			}

			// 디지몬 이름 생성 윈도우 띄움
			CsPoint pos( -1, -1 );
			cSeparate::DeleteInstance( GetWindowType(), 0 );
			cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_T_NAME, 0 );	
			cCreateName* pCName = cCreateName::NewInstance( cBaseWindow::WT_CHANGE_D_NAME, 0, pos );
			if( pCName )
			{
				pCName->SetValue1( m_nValue1 );	// 아이템 인벤 위치
				pCName->SetValue2( true );		// true이면 우클릭을 이용한 알부화, false이면 통상적인 알부화
			}
		}
		return;

	case 30538: // 어드벤처 테이머 체인지 아이템 사용시 메세지창
	case 30539: // 세이버즈 테이머 체인지 아이템 사용시 메세지창
		{
			// m_nValue1 == inven index
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CHANGETAMER_WINDOW, &m_nValue1 );
		}
		return;

#ifdef USE_SHUTDOWN
	case 30445:
	case 30442:
		{
			PostQuitMessage(0);//SetNextGameStep( GAME_EXIT );
		}
		return;
#endif
	case 20029:
		{
			switch(m_nValue1)
			{
			case 1 :	// 아이템 제거
				switch( m_nValue2 )
				{
				case nItem::TabInven:
					{				
						g_pDataMng->GetInven()->GetData( m_nValue3 )->Reset();
					}			
					break;
				case nItem::TabEquip:
					{
						g_pDataMng->GetTEquip()->SetData( m_nValue3, NULL, true );				
					}			
					break;
				case nItem::TabWarehouse:
					{
						//g_pDataMng->GetWarehouse()->GetData( m_nValue3 )->Reset();
						value.first = WareHouseContents::WAREHOUSE_NORMAL;
						value.second = m_nValue3;
						GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_RESET, &value);
					}
					break;
#ifdef NEW_SHARESTASH
				case nItem::TabShareStash:
					{
						//						g_pDataMng->GetWarehouse()->GetShareData( m_nValue3 )->Reset();
						value.first = WareHouseContents::WAREHOUSE_SHARE;
						value.second = m_nValue3;
						GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_RESET, &value);

					}
					break;
#endif
				case nItem::TabChipset:
					{
						g_pDataMng->GetDigivice()->GetChipset( m_nValue3 )->Reset();
					}
					break;
				case nItem::TabGiftShop:
					{
						// 투표 아이템
						int nVoteItemID = m_nValue3;
/*기륜 창고
						// 지급창고에서 찾아서 지우기
						cData_Cash::MAP_POSTINFO	PostInfo = g_pDataMng->GetCash()->GetPostDataMap();
						SAFE_POINTER_BEK(PostInfo);
						cData_Cash::MAP_POSTINFO::iterator	it = PostInfo->begin();
						for( it; it!=PostInfo->end(); ++it )
						{
							cItemInfo* pGiftItem = &it->second;
							if( pGiftItem->m_nType == 0 )
								continue;

							if( pGiftItem->m_nType == nVoteItemID )
							{
								//투표 아이템 삭제
								pGiftItem->DecreaseCount( nItemCnt );
							}
						}
*/
					}
					break;
				default:
					assert_cs( false );
				}
				break;
			case 2 :	// 능력치 미적용
				switch( m_nValue2 )
				{
				case nItem::TabInven:
					{				
						g_pDataMng->GetInven()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					}			
					break;
				case nItem::TabEquip:
					{
						g_pDataMng->GetTEquip()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
						CsC_PartObject::sCHANGE_PART_INFO cp;
						cp.s_nPartIndex = m_nValue3;
						if( g_pCharMng && g_pCharMng->GetTamerUser() )
							g_pCharMng->GetTamerUser()->ChangePart( &cp );
					}
					break;
				case nItem::TabWarehouse:
					{
//						g_pDataMng->GetWarehouse()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
						value.first = WareHouseContents::WAREHOUSE_NORMAL;
						value.second = m_nValue3;
						GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ENDTIME, &value);

					}
					break;
#ifdef NEW_SHARESTASH
				case nItem::TabShareStash:
					{
//						g_pDataMng->GetWarehouse()->GetShareData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
						value.first = WareHouseContents::WAREHOUSE_SHARE;
						value.second = m_nValue3;
						GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ENDTIME, &value);
					}
					break;
#endif
				case nItem::TabChipset:
					{
						g_pDataMng->GetDigivice()->GetChipset( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					}
					break;
				default:
					assert_cs( false );
				}
				break;
			case 3:
				// 능력치 미적용
				switch( m_nValue2 )
				{
				case nItem::TabInven:
					g_pDataMng->GetInven()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					break;
				case nItem::TabEquip:
					g_pDataMng->GetTEquip()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;			
					( (CTamer*)g_pCharMng->GetTamerUser() )->CheckBuff();
					break;
				case nItem::TabWarehouse:
					//std::pair<int, int> value(WareHouseContents::WAREHOUSE_NORMAL, m_nValue3); 
					value.first = WareHouseContents::WAREHOUSE_NORMAL;
					value.second = m_nValue3;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ENDTIME, &value);
//					g_pDataMng->GetWarehouse()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					break;
#ifdef NEW_SHARESTASH
				case nItem::TabShareStash:
					//std::pair<int, int> value(WareHouseContents::WAREHOUSE_SHARE, m_nValue3); 
					value.first = WareHouseContents::WAREHOUSE_SHARE;
					value.second = m_nValue3;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ENDTIME, &value);
//					g_pDataMng->GetWarehouse()->GetShareData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					break;
#endif
				case nItem::TabChipset:			
					g_pDataMng->GetDigivice()->GetChipset( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;			
					break;
				default:
					assert_cs( false );
				}
				break;
			default:
				assert_cs( false );
			}

		}
		return;
	case 30548:
		{
			cSkill::sINFO* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkill( g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkill_Inex() );
			if( pSkill == NULL )
			{
				cPrintMsg::PrintMsg( 30586 );
				break;
			}

			if( net::game )
				net::game->SendTamerSummon_StateCheck2(4, 2, m_nValue1 );

			 g_pCharMng->GetTamerUser()->GetSkillMng()->SetSkill_Index( -1 );	// 다시 초기화
		}
		return;

	case 30550:
		{
			bool bValue = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_IS_TUTORIAL_PLAYING, &bValue);
			if( bValue )
			{
				if(net::game)
					net::game->SendTamerSummonAccept(3, 0);
				cPrintMsg::PrintMsg( 30412 );
			}
			else
			{
				if( net::game )
					net::game->SendTamerSummonAccept(3, 1);
			}
			return;
		}
	case 30561:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GET_ITEM_MACHINE, NULL );
		}
		return;
	case 30562:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_MACHINE, NULL );
		}
		return;
	case 30581:
		// 아이템 롹킹
		g_pDataMng->ItemLock( TO_INVEN_SID( m_nValue1 ) );

		if( net::game )
			net::game->SendItemUse( g_pCharMng->GetTamerUser()->GetUniqID(), m_nValue1 );
		else
			assert_cs( false );

		return;

	case 30583:
		if( net::game )
			net::game->SendTamerCashSkillDelete( m_nValue1 );
		return;
	case 30602: // 디지털 스톤의 힘을 적용하여 디지터리 파워를 변경하시겠습니까?
	case 30611: // 디지털 스톤의 힘을 적용하여 리뉴얼 횟수를 변경하시겠습니까?
	case 30605: // 디지털 스톤의 힘을 적용하여 디지어블 능력치를 변경하시겠습니까?
	case 30608: // 디지털 스톤의 힘을 적용하여 디지어블 능력 수치를 변경하시겠습니까?
		GAME_EVENT_ST.OnEvent( EVENT_CODE::START_ENCHANT_OPTION_ENCHANT, NULL );
		return;
	case 30621: //매치 팀 결정
		{
			//인벤 오픈
			bool bSound = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

			//이놈은 무조건 처음 온 녀석이니까 서버에 요청

			//유저인덱스
			u4 nUserID =  g_pCharMng->GetTamerUser()->GetUniqID();

			//Npc번호
			CsC_AvObject* m_pTarget = NULL;
			m_pTarget = g_pGameIF->GetMastersMatchTarget();
			DWORD nNpcID = ((CNpc*)m_pTarget)->GetFTNpc()->GetInfo()->s_dwNpcID;

			//요청
			net::game->SendMastersMatchOpen( nUserID, (u4)nNpcID );

			//데이터 수신중 메세지
			cPrintMsg::PrintMsg( 10019 );
		}
		break;
	case 30625:		// 디지몬 성장 촉진을 시작하시겠습니까?
		{
			int test = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_START_INCUBATING, &test);
		}
		break;
	case 30626:		// 정말로 성장 촉진을 멈추시겠습니까?
		{
			int test = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_STOP_INCUBATING, &test);
		}
		break;
	case 30629:		// 성장 촉진 중인 디지몬을 인큐베이터에서 꺼내겠습니까
		{
			int test = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_UNREGISTER_INCUBATED_DIGIMON, &test);
		}
		break;
	case 30628:		// 성장 촉진 중인 디지몬을 해당 디지몬 보관소의 디지몬과 교체하시겠습니까
		{
			int test = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_CHANGE_DIGIMON_IN_ACTIVATE_INCUSLOT, &test);
		}
		break;
	case 30630:		// 해당 디지몬 보관소의 디지몬을 촉진 중인 인큐베이터에 등록 하시겠습니까
		{
			int test = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_REGISTER_DIGIMON_TO_ACTIVATE_INCUSLOT, &test);
		}
		break;
	case 30670:	// 도감 완료 보상을 받으시겠습니까
		{
			u4 nDigimonID = g_pGameIF->GetEncyclopedia()->_GetSelectDigimonID();
			//데이터 수신중 메세지
			cPrintMsg::PrintMsg( 10019 );
			net::game->SendEncyclopediaGetItem( nDigimonID );
		}
		break;
	case 30672:	// 해당 덱 효과를 적용하시겠습니까
		{
			u4 nDeckIdx = g_pGameIF->GetEncyclopedia()->_GetSelectDeckIdx();
			net::game->SendEncyclopediaGetDeck( nDeckIdx );

			//버프 셋팅
			GS2C_RECV_ENCYCLOPEDIA_USEDECK recv;
			recv.iDeckIdx = nDeckIdx;
			recv.bUse = true;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::ENCYCLOPEDIA_USE_DECK, &recv );

			
		}
		break;
	case 30673:	// 덱 효과를 해제하시겠습니까
		{
			u4 nDeckIdx = g_pGameIF->GetEncyclopedia()->_GetSelectDeckIdx();
			net::game->SendEncyclopediaGetDeck( 0 );

			//버프 해제
			GS2C_RECV_ENCYCLOPEDIA_USEDECK recv;
			recv.iDeckIdx = nDeckIdx;
			recv.bUse = false;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::ENCYCLOPEDIA_USE_DECK, &recv );
		}
		break;
	case 30656:	// 씰을 등록 하시겠습니까? 처리
		{
			if( g_pGameIF->IsActiveWindow(cBaseWindow::WT_CARDIDENTIFY) )
			{
				g_pGameIF->GetCardIdentify()->SendUnSeal();
			}
		}
		break;

	case 30657:	// 밀봉 하시겠습니까? 처리
		{
			if( g_pGameIF->IsActiveWindow(cBaseWindow::WT_CARDRESEAL) )
			{
				g_pGameIF->GetCardReSeal()->SendReSeal();
			}
		}
		break;

#ifdef LOGIN_SCREEN
	case 10009://전화키 인증 실패, 대만
		{		// 로그인 화면으로 이동
			if( net::account )
			{
				net::account->m_nIsChangeServer = true;
				net::account->DoDisconnect();
			}

			GLOBALDATA_ST.ResetAccountInfo();
			FLOWMGR_ST.ChangePopAllFlow(Flow::CFlow::FLW_LOGIN);
		}
	//	g_pLogin->SelectServerDisconnect();
		break;
#endif

#ifdef BUFF_CHECK_MSG
	case 20220:
		if( net::game )
			net::game->SendItemUse( m_nValue1, m_nValue2 );		
		break;
#endif

	case 20067:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PORTAL_MOVE, &m_nValue2 );
		break;

#ifdef MEGAPHONE_BAN
	case 30676:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CHATOPTION ) )
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CHATOPTION );

			if( net::game )
				net::game->SendItemUse( m_nValue1, m_nValue2 );
		}
		break;
#endif
#ifdef GUILD_RENEWAL
	case 30206:	// GSP를 소모하여 [str]스킬을 활성화 하시겠습니까?
	case 30207:	// 해당 효과를 적용할 경우 기존에 적용중이던 [str] 효과는 삭제됩니다.
	case 30216: // 스킬을\n사용하시겠습니까?%
		{
			//부길마의 경우 등급이 바뀌거나 강제 탈퇴 될수 있다.
			cCommunityContent::sGUILD_PERSON_INFO myInfo;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILD_MYINFO, &myInfo );
			if( myInfo.s_eRank != cCommunityContent::eRANK::GR_2NDMASTER && myInfo.s_eRank != cCommunityContent::eRANK::GR_MASTER )
			{
				std::pair<cCommunityContent::eRANK, std::wstring*> guildName;
				guildName.first = cCommunityContent::GR_2NDMASTER;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildName );
				//해당 등급이 아
				cPrintMsg::PrintMsg( 30158, guildName.second );
				return;
			}
			// 스킬 사용 수락 동작
			u4 nSkillCode = m_nValue1;
			CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode );
			for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
			{
				if( pSkill->GetInfo()->s_Apply[i].s_nID == 0 )
					break;
				switch( pSkill->GetInfo()->s_Apply[ i ].s_nA )
				{
				case GUILD_SUMMON:			// 전체 길드원 소환
				case GUILD_NATUREUP:		// 속성 경험치 추가
				case GUILD_SCANREDUCE:		// 스캔 비용 감소
				case GUILD_SCANBITUP:		// 회수 시 Bit 추가 획득
				case GUILD_HATCHUP:			// 부화 확률 증가
				case GUILD_CROPBITUP:		// 사냥 시 Bit 추가 획득
				case GUILD_MOVEUP:			// 이동 속도 증가
				case GUILD_INCHAPAYREDUCE:	// 강화 비용 감소
				case GUILD_INCHARATEUP:		// 강화 확률 증가
					{
						if( net::game )
							net::game->SendUseGuildSkill( pSkill->GetInfo()->s_Apply[ i ].s_nA, &nSkillCode );
					}
					return;
				default:
					break;
				}
			}
		}
		break;
	case 30209:	//길드 마스터가 소환 스킬을 시전했습니다. 해당 위치로 이동하시겠습니까?
		{
			bool bIsBattleRegisted = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_BATTLEMATCH_REGISTED, &bIsBattleRegisted );
			if( bIsBattleRegisted )// 배틀 신청중 인경우 길드 소환에 응할 수 없다.
			{
				cPrintMsg::PrintMsg( 40013 );	// 배틀 신청중 인경우 길드 소환에 응할 수 없다.
				return;
			}

			u4 nSkillCode = m_nValue1;
			if( net::game )
				net::game->SendGuildSkill_Summon( nSkillCode );
		}
		break;
#endif

	case 30680:		// 디지몬 메모리 스킬 등록
		// 아이템 롹킹
		g_pDataMng->ItemLock( TO_INVEN_SID( m_nValue1 ) );

		if( net::game )
			net::game->SendItemUse( g_pCharMng->GetTamerUser()->GetUniqID(), m_nValue1 );
		else
			assert_cs( false );

		return;

	case 30687:		// 디지몬 메모리 스킬 삭제
		{
	 		if( net::game )
				net::game->SendDigimonSkillDelete( m_nValue1 );			
		}

		return;

	case 30701:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::ASK_DIGIMON_TO_ITEM, &m_nValue1 );
		}
		return;

	case 30703:
		switch( m_nValue1 )
		{
		case 1:	// 아이템에서 디지몬으로
			{
				GAME_EVENT_ST.OnEvent( EVENT_CODE::ASK_ITEM_TO_DIGIMON, &m_nValue2 );
			}
			break;
		case 2:	// 디지몬에서 디지몬으로
			{
			}
			break;

		case 3:	// 디지몬에서 아이템으로
			{
				GAME_EVENT_ST.OnEvent( EVENT_CODE::ASK_DIGIMON_TO_ITEM, &m_nValue2 );
			}
			break;
		}
		return;

	case 11048: // 디지몬 max skill lv 업
		{
			SAFE_POINTER_BEK( g_pDataMng );
			if( !g_pDataMng->IsItemUse( TO_INVEN_SID( m_nValue1 ) ) )
				break;

			cData_Inven* pInven = g_pDataMng->GetInven();
			SAFE_POINTER_BEK( pInven );

			cItemInfo* pItem = pInven->GetData( m_nValue1 );
			SAFE_POINTER_BEK( pItem );
			SAFE_POINTER_BEK( nsCsFileTable::g_pItemMng );
			CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItem->m_nType );
			SAFE_POINTER_BEK( pFTItem );
			CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
			SAFE_POINTER_BEK( pFTInfo );

			SAFE_POINTER_BEK( g_pCharMng );

			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
			SAFE_POINTER_BEK( pDigimonUser );

			DWORD dwTotalType = pFTInfo->s_nType_L * 100 + pFTInfo->s_nType_S;
			int nEvoType = pDigimonUser->GetEvoType();
			if( !nsCsFileTable::g_pBaseMng->IsEnableSkillMaxLvUpFromEvoType( nEvoType, dwTotalType ) )
			{
				cPrintMsg::PrintMsg( nsSkillLimitOpenResult::ItemTypeError );// 현재 디지몬에 사용할 수없는 아이템
				break;
			}

			// 이전 단계의 스킬을 오픈 햇는지 여부 체크
			int nSkillMaxLv = pDigimonUser->GetCurrentEvoMaxSkillLv();
			int nCurrentMaxSkillRanke = nsCsFileTable::g_pBaseMng->GetSkillRankFromSkillMaxLv( nEvoType, nSkillMaxLv );
			int nNeedSkillRank = nsCsFileTable::g_pBaseMng->GetBeforOpenSkillRankeFromItemSubType( dwTotalType );
			if( nCurrentMaxSkillRanke < nNeedSkillRank )// 이전 스킬 랭크를 오픈하지 않았다.
			{
				cPrintMsg::PrintMsg( nsSkillLimitOpenResult::SkipBeforeLevel );// 현재 디지몬에 사용할 수없는 아이템
				break;
			}

			if( nCurrentMaxSkillRanke > nNeedSkillRank )// 이미 상위 스킬 랭크롤 오픈 하였다.
			{
				cPrintMsg::PrintMsg( nsSkillLimitOpenResult::AlreadyOpen );// 현재 디지몬에 사용할 수없는 아이템
				break;
			}

			int nSlot = pDigimonUser->GetFTEvolCurObj()->m_nEvoSlot;
 			if( net::game )
 				net::game->SendSkillMaxLvUp( m_nValue1, pItem->m_nType, nSlot );

		}break;

	case MESSAGE_ACTION:
		{
			GAME_EVENT_ST.OnEvent(EVENT_CODE::START_TRADE_DATA, NULL );
		}
		break;

	case cPrintMsg::GAME_ITEM_MAKING:
		{
			GAME_EVENT_ST.OnEvent(EVENT_CODE::SEND_ITEM_PRODUCTION_RUN, NULL );
		}
		break;

		// 장비 시스템 ============================================================
	case cPrintMsg::EQUIPSYS_WITHDRAW_ITEM:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_CHARGE_ITEM_WITHDRAW, NULL );
		break;

	case cPrintMsg::EQUIPSYS_CANCEL_CHARGE:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_CHARGE_ITEM_CANCEL, NULL );
		break;

	case cPrintMsg::EQUIPSYS_UNLOCK_SLOT:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_CHARGE_ITEM_UNLOCK, NULL );
		break;
		// ==========================================================================
	}

 	_Default();
}

void cMessageBox::_Cancel( bool bAbsolute )
{
	std::pair<int, int> value(0,0);

	// NULL 이면 캔슬은 없다
	if( ( bAbsolute == false )&&( m_ButtonCancel == NULL ) )
	{
		_Ok( true );
		return;
	}

	if( m_bCallProc == true )
		return;
	m_bCallProc = true;

	switch( m_eMsgType )
	{
	case 10020:						// 로그인 시도중 시간 오바로 인한 취소 버튼 클릭
		{
			if( net::net )
			{
#ifdef DEBUG_NETWORK
				DBG("------------------About to disconnect---------------------\n");
#endif
				((cCliAccount*)net::net)->m_nIsChangeServer = true;
				net::net->cClient::DoDisconnect();
				Sleep( 100 );				
			}

			xdelete(net::account_tmp);
			net::account_tmp = net::account;
			net::account = null;
			net::net = net::account;
			if( GLOBALDATA_ST.IsLoginSkiped() )
				PostQuitMessage(0);//SetNextGameStep(GAME_EXIT);//Aeria는 로그인창 없으니까 에러메세지 뜨고나서 꺼야해요
		}
		return;

	case 30175:						// 귀속 아이템 사용해서 귀속아이템 승락여부
	case 32000:
		{
			g_pDataMng->ResetItemTryCount();
			g_pDataMng->ItemUnlock( m_nValue1 );
			g_pDataMng->ItemUnlock( m_nValue2 );
		}
		return;

	case 30176:	
	case 30177:	
		{			
#ifndef ITEM_USE_TIME_PASS
			return;
#endif
			g_pDataMng->ResetItemTryCount();
			g_pDataMng->ItemUnlock( m_nValue1 );
			g_pDataMng->ItemUnlock( m_nValue2 );
		}
		return;

	case 30065:						// 상태의 파티 요청 거절
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_INVITE_ANSWER_REJECT );
// 			if( net::game )
// 			{
// 				net::game->SendPartyInviteReject();
// 			}
		}
		return;
	case 30018:						// 상대의 거래요청 거절
		{
			std::pair<bool, unsigned int> result;
			result.first = false;
			result.second = m_nValue1;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_TRADE_REQUEST_RESULT, &result );
			
// 			if( net::game )
// 			{
// 				net::game->SendTradeReject( m_nValue1 );
// 			}
		}
		return;

	case 30019:						// 거래 요청 후 대기중
		{
			std::pair<bool, unsigned int> result;
			result.first = false;
			result.second = m_nValue1;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_TRADE_REQUEST_RESULT, &result );
// 			if( net::game )
// 			{
// 				net::game->SendTradeReject( m_nValue1 );
// 			}
		}
		return;	

	case 30126:
		if( net::game )
		{
			u4 certify = (u4)m_unValue1;
			net::game->SendGuildReject( m_szValue1, certify );
		}
		return;

	case 30485:
		{
			int nResult = 0;// 배틀 매칭 승락 여부(0:거절,1:승락)
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_DUAL_REQUEST_ANSWER, &nResult );
		}
		return;

		
	case 30312:
	case 30314:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_CASH_ITEMBUY_WINDOW );
		}
		return;
	case cPrintMsg::GAME_ITEM_DELETE:
		CURSOR_ST.IconDropCancel();
		return;

	case cPrintMsg::SAME_MACRO_KEY:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_OPTION_MACRO ) )
			g_pGameIF->GetOptionMacro()->ReleaseCheckRadio();
		return;
	case cPrintMsg::RECOMMENT_ANSWER_CONFIRM:
		{
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::SEND_RECOMMEND_ADD_ANSWER_INFO );
			return;
		}
	case 10051:
	case 19997:
		{
			if( GLOBALDATA_ST.IsLoginSkiped() )
				PostQuitMessage(0);//SetNextGameStep(GAME_EXIT);//Aeria는 로그인창 없으니까 메세지 뜨고나서 취소를 눌러도 꺼야해요
		}
		return;
	case 30549:
		if(net::game)
			net::game->SendTamerSummonAccept(2, 0);
		cPrintMsg::PrintMsg( 30551 );
		return;
		
	case 30550:
		if(net::game)
			net::game->SendTamerSummonAccept(3, 0);
		cPrintMsg::PrintMsg( 30551 );
		return;
	case 20029:
		{
			switch(m_nValue1)
			{
			case 1 :	// 아이템 제거
				switch( m_nValue2 )
				{
				case nItem::TabInven:
					{				
						g_pDataMng->GetInven()->GetData( m_nValue3 )->Reset();
					}			
					break;
				case nItem::TabEquip:
					{
						g_pDataMng->GetTEquip()->SetData( m_nValue3, NULL, true );				
					}			
					break;
				case nItem::TabWarehouse:
					{						
						value.first = WareHouseContents::WAREHOUSE_NORMAL;
						value.second = m_nValue3;
						GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_RESET, &value);
//						g_pDataMng->GetWarehouse()->GetData( m_nValue3 )->Reset();
					}
					break;
#ifdef NEW_SHARESTASH
				case nItem::TabShareStash:
					{
						value.first = WareHouseContents::WAREHOUSE_SHARE;
						value.second = m_nValue3;
						GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_RESET, &value);
//						g_pDataMng->GetWarehouse()->GetShareData( m_nValue3 )->Reset();
					}
					break;
#endif
				case nItem::TabChipset:
					{
						g_pDataMng->GetDigivice()->GetChipset( m_nValue3 )->Reset();
					}
					break;
				default:
					assert_cs( false );
				}
				break;
			case 2 :	// 능력치 미적용
				switch( m_nValue2 )
				{
				case nItem::TabInven:
					{				
						g_pDataMng->GetInven()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					}			
					break;
				case nItem::TabEquip:
					{
						g_pDataMng->GetTEquip()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
						CsC_PartObject::sCHANGE_PART_INFO cp;
						cp.s_nPartIndex = m_nValue3;
						if( g_pCharMng && g_pCharMng->GetTamerUser() )
							g_pCharMng->GetTamerUser()->ChangePart( &cp );
					}
					break;
				case nItem::TabWarehouse:
					{
						std::pair<int, int> value(WareHouseContents::WAREHOUSE_NORMAL, m_nValue3); 
						GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ENDTIME, &value);
//						g_pDataMng->GetWarehouse()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					}
					break;
#ifdef NEW_SHARESTASH
				case nItem::TabShareStash:
					{
						std::pair<int, int> value(WareHouseContents::WAREHOUSE_SHARE, m_nValue3); 
						GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ENDTIME, &value);
//						g_pDataMng->GetWarehouse()->GetShareData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					}
					break;
#endif
				case nItem::TabChipset:
					{
						g_pDataMng->GetDigivice()->GetChipset( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					}
					break;
				default:
					assert_cs( false );
				}
				break;
			case 3:
				// 능력치 미적용
				switch( m_nValue2 )
				{
				case nItem::TabInven:
					g_pDataMng->GetInven()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					break;
				case nItem::TabEquip:
					g_pDataMng->GetTEquip()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;			
					( (CTamer*)g_pCharMng->GetTamerUser() )->CheckBuff();
					break;
				case nItem::TabWarehouse:
//					std::pair<int, int> value(WareHouseContents::WAREHOUSE_NORMAL, m_nValue3); 
					value.first = WareHouseContents::WAREHOUSE_NORMAL;
					value.second = m_nValue3;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ENDTIME, &value);
//					g_pDataMng->GetWarehouse()->GetData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					break;
#ifdef NEW_SHARESTASH
				case nItem::TabShareStash:
					//std::pair<int, int> value(WareHouseContents::WAREHOUSE_SHARE, m_nValue3); 
					value.first = WareHouseContents::WAREHOUSE_SHARE;
					value.second = m_nValue3;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_ENDTIME, &value);
//					g_pDataMng->GetWarehouse()->GetShareData( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;
					break;
#endif
				case nItem::TabChipset:			
					g_pDataMng->GetDigivice()->GetChipset( m_nValue3 )->m_nEndTime = ITEMINFO_INVALIDE_TIME;			
					break;
				default:
					assert_cs( false );
				}
				break;
			default:
				assert_cs( false );
			}

		}
		return;

#ifdef BUFF_CHECK_MSG
		case 20220:
			g_pDataMng->ItemUnlock( m_nValue2 );
			break;
#endif
		case 30626:
			{
				int test = 0;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::ON_CANCEL_INCUBATING, &test);
			}
			break;
#ifdef MEGAPHONE_BAN
		case 30676:
			g_pDataMng->ItemUnlock( m_nValue2 );
			break;
#endif

		// 데이터 교환 실행을 취소 할 경우처리는 여기서 한다.
		case MESSAGE_ACTION:
			{
			}
			break;
		case 30680:		// 디지몬 메모리 스킬 등록
			break;
		case 30687:		// 디지몬 메모리 스킬 삭제
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::MEMORY_SKILL_DELETE, 0 );
			break;;
		case cPrintMsg::GAME_ITEM_MAKING:
			{
			}
			break;

		case 20038:		// 2017-07-11-nova 게임스캐너 드라이버 미 설치 아니요를 누르면 체크해제하고 바로 로그인		
			{
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RECV_USE_BARCODE_FALSE);			
			}
			return;
		case 11048:
			g_pDataMng->ItemUnlock( m_nValue1 );
			break;
	}

	_Default();
}

void cMessageBox::_Default()
{
	switch( m_eMsgType )
	{
	case cPrintMsg::GAME_DEAD:
		if( net::game )
		{
			g_pResist->SetEnablePortal(true);	//2017-06-07-nova 몬스터 스킬을 맞아 죽은 경우 포탈 이동 안되는 문제
			net::game->SendDieConfirm();
		}
		return;

	case 10014:	// 이미 접속한 계정일 경우
	case 10013:	// 다른곳에서 동일계정으로 접속 시도
		PostQuitMessage(0);//SetNextGameStep( GAME_EXIT );
		break;

	case 30371:		
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_UNIONSTORE );
		return;
	}
}

void cMessageBox::Destroy()
{
	cBaseWindow::Delete();	
}

void cMessageBox::DeleteResource()
{
	DeleteScript();
	SAFE_NIDELETE( m_pBody );
	SAFE_NIDELETE( m_pProgress );
}

void cMessageBox::Init( TCHAR const* szTitle, TCHAR const* szBody, int nBoxType, int nMessageListIndex )
{
	assert_cs( IsLive() == false );
	cBaseWindow::Init();

	m_ButtonOk = NULL;
	m_ButtonCancel = NULL;

	// 로그인 화면에서 게임로그를 가리지 않기 위해
	int y = 100;
	//if( ( g_pLogin != NULL )&&( g_pFocusEdit != NULL ) )
	if( (FLOWMGR_ST.GetCurTopFlowID() != Flow::CFlow::FLW_LOGIN) &&  (g_pFocusEdit != NULL) )
	{
		y = 270;
	}

	CsPoint size( 356, 157 );
	switch( m_eMsgType )
	{
	case 30463:				// 매칭 신청
		size.y += 30;
		break;
	case 20039:
	case 20040:
	case 20041:
		size = CsPoint( 700, 210 );
		break;
	case 20061:
	case 30621:
		size = CsPoint( 690, 140 );
		break;
	case 30740:
		size = CsPoint( 690, 260 );
	}

	nMessageListIndex = nMessageListIndex%40;
	int x = ( DEFAULT_SCREEN_WIDTH - size.x )/2;
	x += (nMessageListIndex/10*10) + (nMessageListIndex%10)*6;
	y += nMessageListIndex/10*40 + (nMessageListIndex%10)*5;
	
	InitScript( "Control_G\\MessageBox\\MessageBoxBack.tga", CsPoint( x, y ), size, false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );
	AddSprite( CsPoint( 0, 25 ), CsPoint( size.x, 1 ), "Control_G\\MessageBox\\Line.tga" );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( szTitle );
	AddText( &ti, CsPoint( size.x/2, 3 ) );

	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
	m_pBody = NiNew cStringList;
	switch( m_eMsgType )
	{
	case 20039:
		{
			ti.s_eTextAlign = DT_LEFT;
			m_eTextAlign = DT_LEFT;
						
			g_pStringAnalysis->Cut( m_pBody, MAX_ANAL_LEN, UISTRING_TEXT( "GAMESCANNER_NOTICE_REGIST" ).c_str(), &ti );

			AddSprite( CsPoint( 0, size.y - 80 ), CsPoint( size.x, 1 ), "Control_G\\MessageBox\\Line.tga" );

			ti.s_eTextAlign = DT_CENTER;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "GAMESCANNER_REGIST_MESSAGE" ).c_str() );  
			AddText( &ti, CsPoint( size.x/2, size.y - 70 ) );
		}
		break;
	case 20040:
		{
			ti.s_eTextAlign = DT_LEFT;
			m_eTextAlign = DT_LEFT;

			g_pStringAnalysis->Cut( m_pBody, MAX_ANAL_LEN, UISTRING_TEXT( "GAMESCANNER_NOTICE_RELEASE" ).c_str(), &ti );

			AddSprite( CsPoint( 0, size.y - 80 ), CsPoint( size.x, 1 ), "Control_G\\MessageBox\\Line.tga" );

			ti.s_eTextAlign = DT_CENTER;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "GAEMSCANNER_RELEASE_MESSAGE" ).c_str() ); 
			AddText( &ti, CsPoint( size.x/2, size.y - 70 ) );
		}
		break;
	case 20041:
		{
			ti.s_eTextAlign = DT_LEFT;
			m_eTextAlign = DT_LEFT;

			g_pStringAnalysis->Cut( m_pBody, MAX_ANAL_LEN, UISTRING_TEXT( "GAMESCANNER_REQUEST_RELEASE" ).c_str(), &ti );

			AddSprite( CsPoint( 0, size.y - 80 ), CsPoint( size.x, 1 ), "Control_G\\MessageBox\\Line.tga" );

			ti.s_eTextAlign = DT_CENTER;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "GAMESCANNER_NOTICE_RELEASE_REMAIN_TIME" ).c_str() ); 
			AddText( &ti, CsPoint( size.x/2, size.y - 70 ) );
		}
		break;
	case 20061:
		{
			g_pStringAnalysis->Cut( m_pBody, MAX_ANAL_LEN, UISTRING_TEXT( "GAMESCANNER_CONNECT_ERROR_MESSAGE" ).c_str(), &ti );

			ti.s_Color = NiColor( 1, 1, 0 );
			ti.SetText( UISTRING_TEXT( "GAMESCANNER_CONNECT_HOMEPAGE" ).c_str() );
			cString* pString = NiNew cString;
			pString->AddText( &ti );
			m_pBody->AddTail( pString );
		}
		break;
	case 30621:
		{
			ti.s_eTextAlign = DT_LEFT;
			m_eTextAlign = DT_LEFT;
			g_pStringAnalysis->Cut_Parcing( m_pBody, 770, szBody, &ti );
		}
		break;
	case 30740:
		{
			ti.s_eTextAlign = DT_LEFT;
			m_eTextAlign = DT_LEFT;
			g_pStringAnalysis->Cut_Parcing( m_pBody, 643, szBody, &ti );
		}
		break;
	default:
		{
			//ti.s_eTextAlign = DT_CENTER;
			m_eTextAlign = DT_CENTER;
			g_pStringAnalysis->Cut_Parcing( m_pBody, 310, szBody, &ti );
		}
	}	
	
	SetBtnControl( nBoxType );

	if( m_fValue != 0.0f )
	{
		m_pProgress = NiNew cProgressBar;
		m_pProgress->Init( NULL, cProgressBar::BAR_3IMAGE, GetRootClient() + CsPoint( 17, size.y - 45 ), CsPoint( size.x - 19, 3 ), 
			CsPoint( 2, 2 ), "TargetWindow\\DsBar1.tga", false );		
		m_pProgress->SetRenderPosText( cProgressBar::TEXT_NONE );
		m_pProgress->SetRange( CsPoint( 0, (int)m_fValue ) );		
	}
}

void cMessageBox::SetBtnControl( int nBtnType )
{
	SAFE_NIDELETE( m_ButtonOk );
	SAFE_NIDELETE( m_ButtonCancel );

	CsPoint size = GetRoot()->GetSize();
	cButton::eIMAGE_TYPE eBtnType = cButton::IMAGE_NOR_1;
	switch( nBtnType )
	{
	case CsTalk_Message::MT_OK_CANCEL:
		m_ButtonOk = AddButton( CsPoint( size.x/2 - 84, size.y - 31 ), CsPoint( 73, 18 ), eBtnType, UISTRING_TEXT( "MESSAGEBOX_BTN_YES" ).c_str(), cWindow::SD_Np1 );
		m_ButtonCancel = AddButton( CsPoint( size.x/2 + 14, size.y - 31 ), CsPoint( 73, 18 ), eBtnType, UISTRING_TEXT( "MESSAGEBOX_BTN_NO" ).c_str(), cWindow::SD_Bu2 );
		break;	
	case CsTalk_Message::MT_OK:
		m_ButtonOk = AddButton( CsPoint( size.x/2 - 40, size.y - 31 ), CsPoint( 73, 18 ), eBtnType, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str(), cWindow::SD_Bu3 );
		break;
	case CsTalk_Message::MT_CANCEL:
		m_ButtonCancel = AddButton( CsPoint( size.x/2 - 40, size.y - 31 ), CsPoint( 73, 18 ), eBtnType, UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str(), cWindow::SD_Bu2 );
		break;
	case CsTalk_Message::MT_NOBUTTON:
		break;
	default:
		assert_cs( false );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void cMessageBox::Update(float const& fDeltaTime)
{
	// 파트너몬 변경시 닫아주어야 하는 메세지 박스
	if( g_pDataMng->GetServerSync()->IsChageDigimon() == true )
	{
		switch( m_eMsgType )
		{
		case 30160:
		case 30161:
			m_eState = MBOX_CALL_CANCEL;
			cPrintMsg::PrintMsg( 30024 );
			return;
		}
	}

	switch( m_eMsgType )
	{
	case cPrintMsg::LOG_OUT:
	case cPrintMsg::GAME_EXIT:
		{
			if( ( m_fValue -= g_fDeltaTime ) < 0.0f )
			{
				//[상구 : 2018.01.25
				//종료시에 대화 입력창을 꺼줘야 로그아웃 시에 에러가 나지 않습니다.
				//문제 재현 : 아래 주석을 한뒤 한글 입력한 창을 켜둔 상태에서 로그아웃 -> APP 한정상 종료
				//[상구 : 2018.01.31
				//위치를 변경
				//if(g_pGameIF && g_pGameIF->GetChatMain())
				//	g_pGameIF->GetChatMain()->OnEscapeKey();

				m_eState = MBOX_CALL_OK;
				return;
			}
			else if( m_nValue1 != (int)m_fValue )
			{			
				m_nValue1 = (int)m_fValue;

				std::wstring msg;
				if( m_eState == cPrintMsg::LOG_OUT )
					msg = UISTRING_TEXT( "IN_GAME_LOGOUT_MESSAGE");
				else
					msg = UISTRING_TEXT( "IN_GAME_EXIT_MESSAGE");

				DmCS::StringFn::ReplaceAll( msg, L"#time#", m_nValue1 );
				if( m_nValue2 > 0 )
				{
					msg += UISTRING_TEXT( "IN_GAME_NEXT_EXP_MESSAGE"); 
					DmCS::StringFn::ReplaceAll( msg, L"#value#", m_nValue2 );
				}

				SAFE_NIDELETE( m_pBody );
				m_pBody = NiNew cStringList;

				cText::sTEXTINFO ti;
				ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
				//ti.s_eTextAlign = DT_CENTER;			
				g_pStringAnalysis->Cut_Parcing( m_pBody, 310, const_cast<TCHAR*>(msg.c_str()) , &ti );

				m_pProgress->SetBarPos( (int)m_fValue );
			}
		}
		break;
	case 30018:						// 거래 요청 메세지 받음	
		{
			// 30초동안 수락 안누르면 자동 취소
			if( ( m_fValue -= g_fDeltaTime ) < 0.0f )
			{
				m_eState = MBOX_CALL_CANCEL;
				return;
			}

			float fLen = ( g_pCharMng->GetTamerUser()->GetPos2D() - g_pMngCollector->GetObject( m_nValue1 )->GetPos2D() ).Length();
			if( fLen > IF_TARGET_TAMER_MAX_DIST*3 )
			{
				m_eState = MBOX_CALL_CANCEL;
				return;
			}

			m_pProgress->SetBarPos( (int)m_fValue );
		}
		break;
	case cPrintMsg::RECOMMENT_ANSWER_CONFIRM:
		{
			if( ( m_fValue -= g_fDeltaTime ) < 0.0f )
			{
				m_eState = MBOX_CALL_CANCEL;
				return;
			}
		}
		break;
	case 30096:						// 인던
	case 30126:						// 길드 가입요청
	case 30065:						// 파티 초대
#ifdef GUILD_RENEWAL
	case 30209:						// 길드 소환 수락 여부
#endif
	case 30485:						// 결투 신청
	case 30550:						// 테이머 소환 요청
		{
			// 10초동안 수락 안누르면 자동 취소
			if( ( m_fValue -= g_fDeltaTime ) < 0.0f )
			{
				m_eState = MBOX_CALL_CANCEL;			
				return;
			}
			m_pProgress->SetBarPos( (int)m_fValue );
		}
		break;	
	}
}

cBaseWindow::eMU_TYPE
cMessageBox::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if( ( m_ButtonOk != NULL )&&( m_ButtonOk->Update_ForMouse() == cButton::ACTION_CLICK ) )
	{
		m_eState = MBOX_CALL_OK;
		return muReturn;
	}

	if( ( m_ButtonCancel != NULL )&&( m_ButtonCancel->Update_ForMouse() == cButton::ACTION_CLICK ) )
	{
		m_eState = MBOX_CALL_CANCEL;
		return muReturn;
	}
	return muReturn;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void cMessageBox::Render()
{
	RenderScript();

	int y = m_pBody->GetSize() == 1 ? 48 : 33;

	switch( m_eTextAlign )
	{
	case DT_CENTER:
		m_pBody->Render( CsPoint( 178, y ) + GetRootClient(), 2, m_eTextAlign );
		break;
	case DT_LEFT:
		m_pBody->Render( CsPoint( 20, y ) + GetRootClient(), 2 );
		break;
	default:
		assert_cs( false );
	}

	if( m_pProgress )
		m_pProgress->Render();	
}


void cMessageBox::ResetDevice()
{
	ResetDeviceScript();
	if( m_pProgress )
		m_pProgress->ResetDevice();	
}