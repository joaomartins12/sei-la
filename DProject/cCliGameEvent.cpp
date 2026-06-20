

#include "stdafx.h"

#include "common_vs2019/pGame.h"
#include "common_vs2019/pEvent.h"

#include "network/cCliGame.h"

void cCliGame::SendAttendance(void)		// 출석 체크 요청
{
	newp(pEvent::Attendance);
	endp(pEvent::Attendance);
	send();
}


void cCliGame::RecvAttendance(void)	// 출석 인정 여부 + 아이템 지급 여부
{
	n4 nResCode;
	pop(nResCode);

	switch(nResCode)
	{
	case 1 :
		// 이미 종료되었거나 관계된 출석 이벤트가 없습니다.
		return;
	case 2 :
		// 아직 시작되지 아니하였습니다.
		return;
	case 3 :
		// 이미 출석 체크 완료 하였습니다.
		return;
	case 4 :
		// 출석 인정 시간이 부족합니다.
		return;
	case 5:
		// 출석 이벤트 시간체크를 시작 합니다.
		return;
	case 6:
		// 출석 이벤트가 리셋 되었습니다.
		return;
	case 100 :
		// 금일 출석 인정
		break;
	default :
		xstop1(false, "nResCode : %d Error", nResCode);
		return;
	}

	u4 nGiveItemNo;		// 아이템 지급 번호
	n4 nWorkDayHistory;	// 출석 기록 - bit flag

	pop(nGiveItemNo);
	pop(nWorkDayHistory);

	if(nGiveItemNo<32)
	{
		// nGiveItemNo : 아이템 지급 번호(DM_Event-출석-No)
	}
	else
	{
		// 아이템 지급 아니함
	}
}



void cCliGame::RecvGameEvent(void)
{
	u2 nEventNo = 0;
	pop(nEventNo);	// 이벤트 번호
	u4 nIDX = 0;
	pop(nIDX);	// 디지몬 또는 테이머 IDX

// 	int nUIDX = GetIDX( nIDX );
// 	switch( GetClass( nIDX ) )
// 	{
// 	case nClass::Tamer:
// 		{
// 			CTamer* pTamer = NULL;			
// 			pTamer = g_pCharMng->GetTamer( nUIDX );
// 			if( pTamer )
// 			{
// #ifdef FIREWORK
// 				return;
// #else
// 				pTamer->SetEvent( nEventNo );	
// #endif
// 			}
// 		}
// 		break;
// 	case nClass::Digimon:
// 		{
// 			CDigimon* pDigimon = NULL;
// 			pDigimon = g_pCharMng->GetDigimon( nUIDX );
// 			if( pDigimon )
// 			{
// #ifdef FIREWORK
// 				return;
// #else
// 				pDigimon->SetEvent( nEventNo );	
// #endif
// 			}	
// 		}
// 		break;
// 	default:
// 		assert_cs( false );
// 		break;
// 	}	
}



void cCliGame::RecvEventDailyCheck(void)
{



}


void cCliGame::SendEventDailyCheck(void)
{
	cPacket p;
	p.newp(pEvent::DailyCheck);
	p.endp(pEvent::DailyCheck);
	DoSend(p);
}

// 일정 시간마다 서버에 경험치 시간 업데이트 체크(서버에 보내기만 한다)
void cCliGame::Send_NTF_ExpEventTimeCheck(void)
{
	cPacket p;
	p.newp(pEvent::CeckExpEventTime);
	p.endp(pEvent::CeckExpEventTime);
	DoSend(p);
}

void cCliGame::RecvChangeGameState(void)
{	
	u4 nEventType;
	pop(nEventType);

	if(nEventType & nGame::ExpEvent)
	{
		GS2C_RECV_EXP_EVENT_DATA recv;
		pop( recv.m_nExpRate );	// 경험치 비율
		pop( recv.m_nExpTarget );	// 경험치 종류 1: 일반 2: 부스터
		pop( recv.m_nSpecialExp );	// 해당 유저가 스폐셜 경험치 부여 대상이라면 0이 아닌 값이 들어 있습니다. 최대 500 이상은 되지 않습니다.O
		pop( recv.m_nNextDayExpRate );	// 다음날 접속시 받게되는 경험치 비율

		GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_EXP_EVENT_DATA, &recv );
	}
	else
	{
		GAME_EVENT_ST.OnEvent( EVENT_CODE::DISABLE_EXP_EVENT_UI );
	}
}

// 누군가가 나를 친구 초대함
void cCliGame::RecvAddFriendly()
{
	std::wstring tamerName;
	pop( tamerName );
	memset(m_szTamerName, 0, sizeof(TCHAR) * Language::pLength::name);
	_tcscpy_s( m_szTamerName, Language::pLength::name, tamerName.c_str() );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_FRIENDLY_ADD_INFO, m_szTamerName );
}

// 친구 초대 결과 및 나를 초대한 결과
void cCliGame::RecvAddFriendlyAnswer()
{
	ST_RECV_RecommendResultInfo RecommendResultInfo;
	pop( RecommendResultInfo.m_nAnswer );
	pop( RecommendResultInfo.m_nRecommendType );	
	pop( RecommendResultInfo.m_strTamerName );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_FRIENDLY_ADD_ANSWER_INFO, &RecommendResultInfo );
}

// 친구가 목표를 달성함
void cCliGame::RecvFriendlyMark()
{
	ST_RECV_PurposeCompleteInfo PurposeCompleteInfo;

	pop( PurposeCompleteInfo.m_nRecommendType );
	pop( PurposeCompleteInfo.m_nMarkIndex );
	pop( PurposeCompleteInfo.m_strTamerName );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_FRIENDLY_MARK_INFO, &PurposeCompleteInfo );
}

// 목표 달성 보상 아이템 요청 결과
void cCliGame::RecvFriendlyMarkItem()
{
	ST_RECV_RewardItemInfo RewardItemInfo;
	std::string kName;
	pop( RewardItemInfo.m_nRecommendType );
	pop( RewardItemInfo.m_nMarkIndex );
	pop( RewardItemInfo.m_nResult );
	pop( kName);

	DmCS::StringFn::From( RewardItemInfo.m_strTamerName, kName.c_str() );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_FRIENDLY_MARKITEM_INFO, &RewardItemInfo );
}

// szName 대상을 친구 초대 신청
void cCliGame::SendAddFriendly(TCHAR const * szName)
{
	std::wstring friendName(szName);
	if( friendName.empty() )
		return;

	newp( pEvent::AddFriendly );
		push( friendName );
	endp( pEvent::AddFriendly );
	send();
}

// szName 대상을 친구 초대 수락 여부 결과 전송
void cCliGame::SendAddFriendlyAnswer(u2 nAnswer, TCHAR const* szName)
{
	SAFE_POINTER_RET( szName );

	// 결과 값이 수락도 아니고 실패도 아니면 서버로 전송하지 아니함
	if( nAnswer != nEvent::ANSWER_REJECT && nAnswer != nEvent::ANSWER_OK )
		return;

	std::wstring friendName(szName);
	newp( pEvent::AddFriendlyAnswer );
		push( nAnswer );
		push( friendName );		
	endp( pEvent::AddFriendlyAnswer );
	send();
}

// 친구 초대 이벤트 보상 아이템을 요청함
// 친구 초대 이벤트 타입, 테이블 인덱스, 내가 추천 대상인 경우 이름도 같이 전송
void cCliGame::SendMarkItem(u1 nFriendlyType, u2 nMarkIndex, TCHAR const * szName)
{
	newp( pEvent::FriendlyMarkItem );
		push( nFriendlyType );
		push( nMarkIndex );
		if( nFriendlyType == nEvent::Friendle_Type2 )
		{
			std::wstring name(szName);
			push( name );
		}
	endp( pEvent::FriendlyMarkItem );
	send();	
}

void cCliGame::SendFriendInfo()
{
	newp( pEvent::FriendlyInfo );
	endp( pEvent::FriendlyInfo );
	send();	
}

void cCliGame::RecvFriendlyStateChange()
{
	u1 nState = 0;
	pop( nState );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_FRIENDLY_CHANGESTATE_INFO, &nState );
}

// RecvInitGameData에서 분리되어 나옴
void cCliGame::RecvFriendlyInfo()
{
	ST_RECV_RecommendEvent stRecommendEvent;

	pop( stRecommendEvent.m_nResult );		// 0 = 이벤트 기간이 아니다, 1 = 이벤트 기간
	if ( stRecommendEvent.m_nResult == 0 )
	{
		n2 nErrorCode = 0;
		pop( nErrorCode );

		switch (nErrorCode)
		{
		case nEvent::FRIENDLY_NOT_EVENT_DATE:
			{				
			}
			break;
		}
	}

	if( stRecommendEvent.m_nResult == 1 )
	{
#ifdef VERSION_USA
		u2 nPaneltyType = 0;		
		u4 CashTime = 0;			
		u4 NormalTime = 0;			
		u4 nBasePenaltyTime = 0;	
		u4 nBaseFullTime = 0;

		pop(nBaseFullTime);
		pop(nBasePenaltyTime);
		pop(nPaneltyType);
		pop(CashTime);	
		pop(NormalTime);
#endif // VERSION_USA
		////////////////////////////////////////////////////////////////////////
		// 친구 초대 이벤트 데이터
		/////////////////////////////////////////////////////

		pop( stRecommendEvent.m_nMyRecommendState );	// 0, 3, 4  내가 추천 가능한 상태인지의 값
		for(int i = 0; i < nLimit::MAX_FRIENDLE; i++)
		{
			ST_RECV_RecommendEvent::sInfo stTamerInfo;
			char szTamerName[ Language::pLength::name + 1 ] = {0,};

			pop( stTamerInfo.m_nFriendlyType );			// 1, 2 내가 추천한 타입인지 내가 추천받은 타입인지
			pop( szTamerName, sizeof(char) * (Language::pLength::name + 1) );
			DmCS::StringFn::From( stTamerInfo.m_szTamerName, szTamerName );
			pop( stTamerInfo.m_nFriendlyMark, sizeof(char) * nLimit::MAX_FRIENDLY_MARK );

			stRecommendEvent.AddTamerInfo( stTamerInfo );
		}
	}

	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RECV_RECOMMEND_EVENT_INFO, &stRecommendEvent ); // 추천 이벤트 정보 설정
}

void cCliGame::RecvFriendlyNameChange()
{	
	n1 nFriendlyType = 0;								// 추천타입	
	std::wstring strBefoleName;
	std::wstring strNewName;

	pop( nFriendlyType );
	pop( strBefoleName );
	pop( strNewName );

	ContentsStream kStream;
	kStream << nFriendlyType << strBefoleName << strNewName;

	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::RECV_FRIENDLY_CHANGE_NAME, &kStream );
}

void cCliGame::SendMastersMatchOpen( u4 nTamerIDX, u4 nNPCIDX )
{
	newp( pEvent::MastersMatchingOpen );
	push( nTamerIDX );
	push( nNPCIDX );
	endp( pEvent::MastersMatchingOpen );
	send();
}

void cCliGame::SendMastersMatchInsert( u4 nUserID, u4 nNpcID, u4 nSlot, u4 nGiveCnt, u1 nFTNpcIdx )
{
	newp( pEvent::MastersMatching );
	push( nUserID );
	push( nNpcID );
	push( nSlot );
	push( nGiveCnt );
	push( nFTNpcIdx );
	endp( pEvent::MastersMatching );
	send();
}

void cCliGame::SendPinokimonInfoReq()
{
	// 투표수 목록 요청( 빈패킷 )
	newp( pEvent::PinochiGetInfo );
	endp( pEvent::PinochiGetInfo );
	send();
}
void cCliGame::SendPinokimonVote( u1 nCardIdx, u4 nVoteCnt, u4 nNpcID )
{
	// 투표 요청
	newp( pEvent::PinochiVote );
	push( nCardIdx );
	push( nVoteCnt );
	push( nNpcID );
	endp( pEvent::PinochiVote );
	send();
}

//////////////////////////////////////////////////////////////////////////
void cCliGame::RecvNtfHotTimeEvent(void)
{
	GS2C_NTF_HOTTIME_EVENT_INFO recv;
	pop( recv.nEventState );
	pop( recv.nCurrentTableIdx );
	pop( recv.nNextTableIdx );
	pop( recv.bIsGetItem );
	pop( recv.nStartTimeLeftTs );
	pop( recv.nEndTimeLeftTs );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_NTF_HOTTIME_EVENT_INFO, &recv );
}

void cCliGame::RecvHotTimeGetItemResult(void)
{
	GS2C_RECV_HOTTIME_GET_RESULT recv;
	pop( recv.nResult );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_HOTTIME_EVENT_ITEM_GET, &recv );
}

void cCliGame::SendHotTimeGetItemRequest(int const& nTableIDX)
{
	C2GS_SEND_HOTTIME_EVENT_GET packet;
	packet.nTableIdx = nTableIDX;
	newp( packet.GetProtocol() );
	push( packet.nTableIdx );
	endp( packet.GetProtocol() );
	send();
}
//////////////////////////////////////////////////////////////////////////
// Daily Check Event

// 출석부 이벤트 정보 요청
void cCliGame::SendDailyCheckEventRequest()
{
#if COMMON_LIB_FIXED
	C2GS_SEND_DAILY_CHECK_EVENT_INFO packet;
	newp( packet.GetProtocol() );
	endp( packet.GetProtocol() );
	send();
#endif
}

// 출석부 이벤트 정보
void cCliGame::RecvDailyCheckEventResult()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_DAILY_CHECK_EVENT_INFO recv;
	pop( recv.nCount );

	for( int i = 0; i < recv.nCount; ++i )
	{
		nsDailyCheckEvent::sDailyCheckData sData;
		pop( sData.nGroupCode );
		pop( sData.nCurrentDay );
		pop( sData.nNextLeftTS );
		pop( sData.cEventInfo.pData, sizeof( char ) * ( ((sData.nCurrentDay-1)/8)+1 ) );
		recv.lEventData.push_back( sData );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_DAILYCHEKEVENT_INFO_RESULT, &recv );
#endif
}

// 출석부 이벤트 아이템 수령 요청
void cCliGame::SendDailyCheckEventItemGet(n4 nGroupID)
{
#if COMMON_LIB_FIXED
	C2GS_SEND_DAILY_CHECK_EVENT_ITEM_REQUEST packet;
	packet.nGroupNo = nGroupID;

	newp( packet.GetProtocol() );
	push( packet.nGroupNo );
	endp( packet.GetProtocol() );
	send();
#endif
}

// 출석부 이벤트 아이템 수령 요청 결과
void cCliGame::RecvDailyCheckEventItemGetResult()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_DAILY_CHECK_EVENT_ITEM_REQUEST_RESULT recv;
	pop( recv.nResult );
	pop( recv.nGroupNo );
	pop( recv.nCurrentDay );
	pop( recv.nNextLeftTS );
	pop( recv.nItemCount );
	for( int i = 0; i < recv.nItemCount; ++i )
	{
		nsDailyCheckEvent::sDailyCheckedItem sItem;
		pop( sItem.nItemPos );
		pop( sItem.cItem );
		recv.lCheckedItems.push_back( sItem );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_DAILYCHECKEVENT_RESULT_ITEM, &recv );
#endif
}
