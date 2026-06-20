#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "EventContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

#include "../Adapt/AdaptTutorialQuest.h"

EventContents::sExpEventData::sExpEventData():
m_nExpRate(0), m_nNextDayExpRate(0), m_nExpTarget(0),
m_nSpecialExp(0),m_HideExpEventUI(false)
{
	
}

EventContents::sExpEventData::~sExpEventData()
{

}

void EventContents::sExpEventData::ResetData()
{
	m_nExpRate = 0;
	m_nNextDayExpRate = 0;
	m_nExpTarget = 0;;
	m_nSpecialExp = 0;
	m_HideExpEventUI = false;	
}
//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
EventContents::sDailyEventData::sDailyEventData():m_nEventNo(-1),m_nRemainedTimeTS(0),m_nTotalTime(0),m_nWeek(0xFF),m_nRecvServerTimeTS(0)
{
	
}

EventContents::sDailyEventData::~sDailyEventData()
{
}

void EventContents::sDailyEventData::ResetData()
{
	m_nWeek = 0xFF;
	m_nEventNo = -1;				// DM_Event-매일 참조 - 이벤트 번호
	m_nRemainedTimeTS = 0;		// 해당 이벤트 아이템을 받기위해 남은 시간
	m_nRecvServerTimeTS = 0;
	m_nTotalTime = 0;			// 해당 이벤트 아이템을 받기위한 총 시간
	m_ItemLists.clear();
}

int EventContents::sDailyEventData::GetTimeLineCount() const
{
	if(m_nWeek == 0xFF)
		return 0;

	int nResult = 4;// 기본이 4개
	if( m_nWeek % 6 == 0)		//0-일요일 6-토요일
		nResult = 5;

	return nResult;
}

int EventContents::sDailyEventData::GetWeekStartIdx() const
{
	int weekStart = 20;	//토요일 시작위치
	if( m_nWeek % 6 == 0)		//0-일요일 6-토요일
	{
		if(m_nWeek == 0)
			weekStart = 25;
	}
	else
		weekStart = (m_nWeek-1) * 4;

	return weekStart;
}

int	EventContents::sDailyEventData::GetCompleteCount() const
{
	int nCount = GetTimeLineCount();
	int weekStart = GetWeekStartIdx();

	if(m_nEventNo != -1)	return m_nEventNo - weekStart;
	else					return nCount;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

EventContents::sMonthlyEventData::sMonthlyEventData():m_nEventNo(0xFF),m_nAttendCnt(0),m_bNotify(false),monthInfo(NULL)
{
}

EventContents::sMonthlyEventData::~sMonthlyEventData()
{
}

void EventContents::sMonthlyEventData::ResetData()
{
	m_nEventNo = 0xFF;				
	m_nAttendCnt = 0;		
	m_bNotify = false;
	monthInfo = NULL;
}

bool EventContents::sMonthlyEventData::SetUpdateData( byte const& nEventNo, byte const& attendCnt, bool const& bNotify )
{
	bool bUpdate = false;

	if( m_nEventNo != nEventNo )
	{
		m_nEventNo = nEventNo;
		bUpdate = true;
	}

	if( m_nAttendCnt != attendCnt )
	{
		m_nAttendCnt = attendCnt;
		bUpdate = true;
	}

	if( m_bNotify != bNotify )
	{
		m_bNotify = bNotify;
		bUpdate = true;
	}

	if( 0xFF == m_nEventNo )
		monthInfo = NULL;
	else
		monthInfo = nsCsFileTable::g_pEventMng->GetMapMonthly( CsEventTable::ET_MONTHLY, m_nEventNo);

	return bUpdate;
}

bool EventContents::sMonthlyEventData::IsHaveEvent() const
{
	if( 0xFF == m_nEventNo )
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
void EventContents::sHotTimeEventData::ResetData()
{
	nEventState = nsHotTimeEventState::NO_EVENT;
	nCurrentTableIdx = 0;
	nNextTableIdx = 0;
	bIsGetItem = false;
	nStartTimeLeftTs = 0;
	nEndTimeLeftTs = 0;	
	szStartDate = L"";
	szEndDate = L"";

	bWaitRecv = false;

	pHotTimeCur = NULL;
	pHotTimeNext = NULL;
}

bool EventContents::sHotTimeEventData::IsHaveEvent() const
{
	return nEventState != nsHotTimeEventState::NO_EVENT;
}

bool EventContents::sHotTimeEventData::IsGetItemTime() const// 아이템을 받을 수 있는 상태인지 확인
{
	if( nsHotTimeEventState::NO_EVENT == nEventState )
		return false;
	
	if( nsHotTimeEventState::NOT_INTIME == nEventState )
	{
		uint nCurrentTimeTs = net::game->GetTimeTS();
		if( nStartTimeLeftTs > nCurrentTimeTs )
			return false;
	}

	if( nsHotTimeEventState::INTIME == nEventState )
	{
		uint nCurrentTimeTs = net::game->GetTimeTS();
		if( nEndTimeLeftTs < nCurrentTimeTs )
			return false;
	}

	return true;
}

bool EventContents::sHotTimeEventData::IsGetItem() const	// 아이템을 받았는지 검사 
{
	return bIsGetItem;
}

void EventContents::sHotTimeEventData::SetGetItem(bool bValue)
{
	bIsGetItem = bValue;
}

int  EventContents::sHotTimeEventData::GetCurrentItemTIDX() const
{
	return nCurrentTableIdx;
}

int EventContents::sHotTimeEventData::GetNextItemTIDX() const
{
	return nNextTableIdx;
}

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
int const EventContents::IsContentsIdentity(void)
{
	return E_CT_EVENT_CONTENTS;
}

EventContents::EventContents(void) : m_pAdaptTutorial(NULL)
{	
	//////////////////////////////////////////////////////////////////////////
	// Exp Event Function
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_EXP_EVENT_DATA, this, &EventContents::Recv_ExpEventData );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::DISABLE_EXP_EVENT_UI, this, &EventContents::Recv_ExpEventDisable);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::HIDE_EXP_EVENT_UI, this, &EventContents::Hide_ExpEventData );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SHOW_EXP_EVENT_UI, this, &EventContents::Show_ExpEventData );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_EXP_EVENT_NEXT_EXPRATE, this, &EventContents::Get_NextExpRate);	
	//////////////////////////////////////////////////////////////////////////

	GAME_EVENT_ST.AddEvent( EVENT_CODE::REWARD_UI_TOGGLE, this, &EventContents::RewardUI_Toggle); 
	//////////////////////////////////////////////////////////////////////////
	// Daily Event Function
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_DAILY_EVENT_DATA, this, &EventContents::Recv_DailyEventData);	
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Monthly Event Function
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_MONTHLY_EVENT_DATA, this, &EventContents::Recv_MonthlyEventData);	
	//////////////////////////////////////////////////////////////////////////
	

	//////////////////////////////////////////////////////////////////////////
	// HotTime Event Function
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_NTF_HOTTIME_EVENT_INFO, this, &EventContents::Recv_NTF_HotTimeEventInfo);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_HOTTIME_EVENT_ITEM_GET, this, &EventContents::Recv_HotTimeEventItemGet);	
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// DailyCheck Event Function
	GAME_EVENT_ST.AddEvent( EVENT_CODE::UPDATE_DAILYCHECKEVENT_TIMER,		this, &EventContents::Update_DailyCheckEvent_Timer );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_DAILYCHEKEVENT_INFO_RESULT,	this, &EventContents::Recv_DailyCheckEventInfo_Request );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_DAILYCHECKEVENT_RESULT_ITEM,	this, &EventContents::Recv_DailyCheckEvent_ItemGet );
	//////////////////////////////////////////////////////////////////////////
}

EventContents::~EventContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);

	m_pAdaptTutorial = NULL;
}

int const EventContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool EventContents::Initialize(void)
{
	return true;
}

void EventContents::UnInitialize(void)
{
}

bool EventContents::IntraConnection(ContentsSystem& System)
{
	m_pAdaptTutorial = new AdaptTutorialQuest;
	if( m_pAdaptTutorial && !m_pAdaptTutorial->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;

	return true;
}

void EventContents::Update(float const& fElapsedTime)
{
// 	if(fCurrentTime != -1.0f)
// 		fCurrentTime = fCurrentTime + fElapsedTime;
}

void EventContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void EventContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

void EventContents::MakeMainActorData(void)
{
	// Test Code
//	Recv_DailyCheckEventInfo_Request( NULL );
}

void EventContents::ClearMainActorData(void)
{
}

void EventContents::ClearWorldData(void)
{
	m_ExpEventData.ResetData();
	m_DailyEventData.ResetData();
	m_MonthlyEventData.ResetData();
	m_HotTimeEventData.ResetData();
	m_DailyCheckEventData.ResetData();
	RemoveExpEventTimer();
}

void EventContents::MakeWorldData(void)
{
	SetExpEventTimer();
#ifdef LJW_DAILYCHECKEVENT_191030
	Send_DailyCheckEventInfo_Request();		// 일일 출석부 이벤트 정보 요청
#endif
}

//////////////////////////////////////////////////////////////////////////
// Exp Event Function
//////////////////////////////////////////////////////////////////////////
unsigned int EventContents::GetCurrentExpRate() const
{
	return m_ExpEventData.m_nExpRate;
}

unsigned int EventContents::GetExpRateType() const
{
	return m_ExpEventData.m_nExpTarget;
}

unsigned int EventContents::GetNextExpRate() const
{
	return m_ExpEventData.m_nNextDayExpRate;
}

void EventContents::Get_NextExpRate(void* pData)
{
	SAFE_POINTER_RET(pData);
	unsigned int* pGetData = static_cast<unsigned int*>(pData);

	*pGetData = GetNextExpRate();
}

void EventContents::Recv_ExpEventData(void* pData)
{
	SAFE_POINTER_RET(pData);

	GS2C_RECV_EXP_EVENT_DATA * pRecv = static_cast<GS2C_RECV_EXP_EVENT_DATA*>(pData);

	m_ExpEventData.m_nExpRate = pRecv->m_nExpRate;// 경험치 비율
	m_ExpEventData.m_nNextDayExpRate = pRecv->m_nNextDayExpRate;// 다음날 접속시 받게되는 경험치 비율
	m_ExpEventData.m_nExpTarget = pRecv->m_nExpTarget;// 경험치 종류 1: 일반 2: 부스터
	// 현재 사용안하는 변수
	m_ExpEventData.m_nSpecialExp = pRecv->m_nSpecialExp;// 해당 유저가 스폐셜 경험치 부여 대상이라면 0이 아닌 값이 들어 있습니다. 최대 500 이상은 되지 않습니다.
	
	if( m_ExpEventData.m_HideExpEventUI )
		return;
	
	bool bIsShow = g_pGameIF->IsActiveWindow( cBaseWindow::WT_EXP_EVENT_UI );
	if( bIsShow )
		Notify( eEvent::eUPDATE_EXP_EVENT_DATA );
	else
	{
#ifdef LJW_TUTORIAL_RENEWAL_20191128
		if( m_pAdaptTutorial && m_pAdaptTutorial->IsTutorialPlaying_ClientMode() )
			return;
#endif

		g_pGameIF->GetDynamicIF( cBaseWindow::WT_EXP_EVENT_UI );
	}
}

void EventContents::Recv_ExpEventDisable( void* pData )
{
	m_ExpEventData.ResetData();
	bool bIsShow = g_pGameIF->IsActiveWindow( cBaseWindow::WT_EXP_EVENT_UI );
	if( bIsShow )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_EXP_EVENT_UI );
}

void EventContents::Hide_ExpEventData(void* pData)
{
	if( m_ExpEventData.m_HideExpEventUI )
		return;

	m_ExpEventData.m_HideExpEventUI = true;

	bool bIsShow = g_pGameIF->IsActiveWindow( cBaseWindow::WT_EXP_EVENT_UI );
	if( bIsShow )
	{
		cBaseWindow* pExpUI = g_pGameIF->GetDynamicIF( cBaseWindow::WT_EXP_EVENT_UI );
		if( pExpUI )
			pExpUI->SetShowWindow( false );
	}		
}

void EventContents::Show_ExpEventData(void* pData)
{
	if( !m_ExpEventData.m_HideExpEventUI )
		return;

	m_ExpEventData.m_HideExpEventUI = false;

	Notify( eEvent::eUPDATE_EXP_EVENT_DATA );

	bool bIsShow = g_pGameIF->IsActiveWindow( cBaseWindow::WT_EXP_EVENT_UI );
	if( !bIsShow )
	{
		cBaseWindow* pExpUI = g_pGameIF->GetDynamicIF( cBaseWindow::WT_EXP_EVENT_UI );
		pExpUI->SetShowWindow( true );
	}
}


#define IDEVENT_CHECK_EXP_TIMER			100
#define UPDATE_EXP_TIME					5 * 60 // 5분	

void CALLBACK EventContents::ExpEventCheckTimerProc( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime )
{
	switch( idEvent )
	{
	case IDEVENT_CHECK_EXP_TIMER:
		{
			if(FLOWMGR_ST.GetCurTopFlowID() == Flow::CFlow::FLW_LOADING )
				return;
			if( net::game )
				net::game->Send_NTF_ExpEventTimeCheck();
		}		
		break;
	default:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_DAILYCHECKEVENT_TIMER, &idEvent );
		}
		break;
	}
}

void EventContents::SetExpEventTimer()
{
	::SetTimer( GAMEAPP_ST.GetHWnd(), IDEVENT_CHECK_EXP_TIMER, UPDATE_EXP_TIME * 1000, (TIMERPROC)(EventContents::ExpEventCheckTimerProc) );
}

void EventContents::RemoveExpEventTimer()
{
	::KillTimer( GAMEAPP_ST.GetHWnd(), IDEVENT_CHECK_EXP_TIMER );
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Daily Event Function
//////////////////////////////////////////////////////////////////////////

int EventContents::GetDailyEventNo() const
{
	return m_DailyEventData.m_nEventNo;
}

byte EventContents::GetDailyEventWeek() const
{
	return m_DailyEventData.m_nWeek;
}

unsigned int EventContents::GetDailyEventTotalTs() const
{
	return m_DailyEventData.m_nTotalTime;
}

unsigned int EventContents::GetDailyEventCurrentTs() const
{
	return m_DailyEventData.m_nTotalTime - m_DailyEventData.m_nRemainedTimeTS;
}

unsigned int EventContents::GetDailyEventRemainedTs() const
{
	return m_DailyEventData.m_nRemainedTimeTS;
}

// unsigned int EventContents::GetDailyEventCurrentTime() const
// {
// 	return m_DailyEventData.m_nTotalTime - m_DailyEventData.m_nRemainedTimeTS + fCurrentTime;
// }

int EventContents::GetDailyEventPlayTimeMinute() const
{
	int curTs = (net::game->GetTimeTS() - m_DailyEventData.m_nRecvServerTimeTS) + (m_DailyEventData.m_nTotalTime - m_DailyEventData.m_nRemainedTimeTS);

	return curTs / 60;
}

int EventContents::GetDailyEventTimeLineCount() const
{
	return m_DailyEventData.GetTimeLineCount();
}

int EventContents::GetDailyEventCompleteCount() const
{
	return m_DailyEventData.GetCompleteCount();
}


int EventContents::GetDailyEventWeekStartIdx() const
{
	return m_DailyEventData.GetWeekStartIdx();
}


std::list<EventContents::sDailyEventData::sItems> const& EventContents::GetDailyEventItems() const
{
	return m_DailyEventData.m_ItemLists;
}

//////////////////////////////////////////////////////////////////////////

byte EventContents::GetMonthlyEventNo() const
{
	return m_MonthlyEventData.m_nEventNo;
}

byte EventContents::GetMonthlyEventAttendCnt() const
{
	return m_MonthlyEventData.m_nAttendCnt;
}

bool EventContents::GetMonthlyEventNotify() const
{
	return m_MonthlyEventData.m_bNotify;
}

void EventContents::Recv_DailyEventData(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_DAILY_EVENT_DATA* pRecv = static_cast<GS2C_RECV_DAILY_EVENT_DATA*>(pData);
	
	m_DailyEventData.m_nEventNo = pRecv->m_nEventNo;
	m_DailyEventData.m_nRemainedTimeTS = pRecv->m_nRemainedTimeTS;
	m_DailyEventData.m_nTotalTime = pRecv->m_nTotalTime;
	m_DailyEventData.m_nWeek = pRecv->m_nWeek;
	m_DailyEventData.m_ItemLists.clear();
	m_DailyEventData.m_nRecvServerTimeTS = net::game->GetTimeTS();

	if( -1 != pRecv->m_nEventNo )
	{
		CsEventTable::sEVENT* pItem = nsCsFileTable::g_pEventMng->GetMap( CsEventTable::ET_DAILY, pRecv->m_nEventNo );
		if( pItem )	// 이벤트 테이블에서 이벤트 번호를 찾을 수 없음.
		{
			for( int n = 0; n < EVENT_ITEM_MAXCNT; ++n )
			{
				if( pItem->s_nItemID[n] == 0 )
					break;

				sDailyEventData::sItems addItem;
				addItem.m_nItemCode = pItem->s_nItemID[n];
				addItem.m_nItemCount = pItem->s_nItemCnt[n];
				m_DailyEventData.m_ItemLists.push_back( addItem );
			}
		}
	}

	if(pRecv->m_nEventNo == -1)// 이벤트 종료
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DAILY_EVENT_UI );
	else
	{
#ifdef LJW_TUTORIAL_RENEWAL_20191128
		if( m_pAdaptTutorial && m_pAdaptTutorial->IsTutorialPlaying_ClientMode() )
			return;
#endif

		g_pGameIF->GetDynamicIF(cBaseWindow::WT_DAILY_EVENT_UI);
	}

	Notify(eDailyEvent_Update);
}

void EventContents::Recv_MonthlyEventData(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MONTHLY_EVENT_DATA* pRecv = static_cast<GS2C_RECV_MONTHLY_EVENT_DATA*>(pData);
	bool bUpdate = m_MonthlyEventData.SetUpdateData( pRecv->m_nEventNo, pRecv->m_nAttendCnt, pRecv->m_bNotify );
	if(bUpdate)
		Notify(eMonthlyEvent_Update);
}

void EventContents::CloseDailyEventUI()
{
	m_DailyEventData.ResetData();
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_DAILY_EVENT_UI );
}

void EventContents::CloseRewardEventUI()
{
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_REWARD_EVENT_UI );
}

bool EventContents::AcceptButtonEvent(int sel, int selSub /*= 0*/)
{
	switch(sel)
	{
	case 0:	GAME_EVENT_ST.OnEvent( EVENT_CODE::REWARD_ITEM_TOGGLE );	return false;
	case 1: GAME_EVENT_ST.OnEvent( EVENT_CODE::CASH_TOGGLE );			return false;
	case 2:	GAME_EVENT_ST.OnEvent( EVENT_CODE::REWARD_ITEM_TOGGLE );	return false;
	case 3: Send_DailyCheckEvent_ItemGet( selSub );						return false;
	default:
		return false;
	}
}

void EventContents::RewardUI_Toggle(void* pData)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_REWARD_EVENT_UI, 0 ) )					
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_REWARD_EVENT_UI, 0 );
	else
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_REWARD_EVENT_UI, 0 );
}

int	EventContents::GetMonthItemID(int no)
{
	SAFE_POINTER_RETVAL( m_MonthlyEventData.monthInfo, 0 );
	if( MONTHLY_ITEM_MAXCNT  <= no )
		return 0;
	return m_MonthlyEventData.monthInfo->s_nItemID[no];
}

USHORT EventContents::GetMonthItemCnt(int no)
{
	SAFE_POINTER_RETVAL( m_MonthlyEventData.monthInfo, 0 );
	if( MONTHLY_ITEM_MAXCNT <= no )
		return 0;
	return m_MonthlyEventData.monthInfo->s_nItemCnt[no];
}

void EventContents::SetMonthMessage(cText* text)
{
	SAFE_POINTER_RET( m_MonthlyEventData.monthInfo);
	text->SetText(m_MonthlyEventData.monthInfo->s_szMessage);
}


EventContents::sMonthlyEventData const& EventContents::GetMonthlyEventData() const
{
	return m_MonthlyEventData;
}

void EventContents::Recv_NTF_HotTimeEventInfo(void* pData)
{
	SAFE_POINTER_RET( pData );

	GS2C_NTF_HOTTIME_EVENT_INFO* pRecv = static_cast<GS2C_NTF_HOTTIME_EVENT_INFO*>(pData);
	
	m_HotTimeEventData.nEventState = pRecv->nEventState;
	m_HotTimeEventData.nCurrentTableIdx = pRecv->nCurrentTableIdx;
	m_HotTimeEventData.nNextTableIdx = pRecv->nNextTableIdx;
	m_HotTimeEventData.bIsGetItem = pRecv->bIsGetItem > 0 ? true : false;
	m_HotTimeEventData.nStartTimeLeftTs = pRecv->nStartTimeLeftTs + net::game->GetTimeTS();
	m_HotTimeEventData.nEndTimeLeftTs = pRecv->nEndTimeLeftTs + net::game->GetTimeTS();

	CsEventTable::sHotTimeEvent const* pCur = nsCsFileTable::g_pEventMng->GetHotTime( m_HotTimeEventData.nCurrentTableIdx );
	SAFE_POINTER_RET( pCur );
	m_HotTimeEventData.pHotTimeCur = pCur;
	m_HotTimeEventData.szStartDate = pCur->s_szStartDate;
	m_HotTimeEventData.szEndDate = pCur->s_szEndDate;

	CsEventTable::sHotTimeEvent const* pNext = nsCsFileTable::g_pEventMng->GetHotTime( m_HotTimeEventData.nNextTableIdx );
	SAFE_POINTER_RET( pNext );
	m_HotTimeEventData.pHotTimeNext = pNext;
	m_HotTimeEventData.szStartDate = pNext->s_szStartDate;
	m_HotTimeEventData.szEndDate = pNext->s_szEndDate;

	Notify( eHotTimeEvent_Update );
}

void EventContents::Recv_HotTimeEventItemGet(void* pData)
{
	SAFE_POINTER_RET( pData );

	GS2C_RECV_HOTTIME_GET_RESULT* pRecv = static_cast<GS2C_RECV_HOTTIME_GET_RESULT*>(pData);

	m_HotTimeEventData.bWaitRecv = false;

	if( nsHotTimeResult::SUCCESS == pRecv->nResult )
		m_HotTimeEventData.SetGetItem(true);
	else
		cPrintMsg::PrintMsg(pRecv->nResult);

	Notify( eHotTimeEvent_Update );
}

bool EventContents::Send_HotTimeEvent_ItemGet()
{
	if( m_HotTimeEventData.bWaitRecv )
		return false;

	if( !m_HotTimeEventData.IsHaveEvent() )// 이벤트가 없다
	{
		cPrintMsg::PrintMsg( nsHotTimeResult::NOT_TIME );	
		return false;
	}

	if( !m_HotTimeEventData.IsGetItemTime() )
	{
		cPrintMsg::PrintMsg( nsHotTimeResult::NOT_TIME );
		return false;
	}

	if( m_HotTimeEventData.IsGetItem() )// 이미 아이템을 받았을 경우
	{
		cPrintMsg::PrintMsg( nsHotTimeResult::ALREADY_GET );
		return false;
	}

	net::game->SendHotTimeGetItemRequest( m_HotTimeEventData.GetCurrentItemTIDX() );
	m_HotTimeEventData.bWaitRecv = true;
	return true;
}

EventContents::sHotTimeEventData const& EventContents::GetHotTimeEvent_Info() const
{
	return m_HotTimeEventData;
}

bool EventContents::IsHaveHotTimeEvent() const
{
	return m_HotTimeEventData.IsHaveEvent();
}

std::wstring EventContents::GetItemName( DWORD const& dwItemCode )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, L"" );
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemCode );
	SAFE_POINTER_RETVAL( pItem, L"" );
	SAFE_POINTER_RETVAL( pItem->GetInfo(), L"" );
	return pItem->GetInfo()->s_szName;	
}

NiColor EventContents::GetItemColor( DWORD const& dwItemCode )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, FONT_WHITE );
	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemCode );
	SAFE_POINTER_RETVAL( pItem->GetInfo(), FONT_WHITE );
	return g_pDataMng->GetItemColor( pItem->GetInfo() );
}

//////////////////////////////////////////////////////////////////////////
// Daily Check Event
// 출석부 이벤트 정보 요청
void EventContents::Send_DailyCheckEventInfo_Request()
{
	SAFE_POINTER_RET( net::game );
	net::game->SendDailyCheckEventRequest();
}

// 출석부 이벤트 정보 업데이트
void EventContents::Recv_DailyCheckEventInfo_Request(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_DAILY_CHECK_EVENT_INFO recv = *static_cast< GS2C_RECV_DAILY_CHECK_EVENT_INFO* >( pData );

	m_DailyCheckEventData.ResetData();

	std::list< nsDailyCheckEvent::sDailyCheckData >::iterator it = recv.lEventData.begin();
	for( ; it != recv.lEventData.end(); ++it )
		m_DailyCheckEventData.InitDailyCheckList( *it );

	Notify( eDailyCheckEvent_UpdateInfo );
#endif
}

// 아이템 수령 요청
void EventContents::Send_DailyCheckEvent_ItemGet(int const& nGroupIndex)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( net::game );

	// 서버에 이미 보내고 요청 대기중
	if( m_DailyCheckEventData.bWaitRecv )
		return;

	uint nGroupID = m_DailyCheckEventData.GetGroupID( nGroupIndex );
	if( 0 == nGroupID )
	{
		cPrintMsg::PrintMsg( nsDailyCheckEventResult::NOT_TIME );
		return;
	}

	if( !m_DailyCheckEventData.IsCheckable( nGroupIndex ) )
	{
		cPrintMsg::PrintMsg( nsDailyCheckEventResult::ALREADY_GET );
		return;
	}

	if( !CheckInventory( m_DailyCheckEventData.GetCheckableItemInfo( nGroupIndex ) ) )
	{
		cPrintMsg::PrintMsg( nsDailyCheckEventResult::INVEN_FULL );
		return;
	}

	net::game->SendDailyCheckEventItemGet( nGroupID );
#endif
}
// 아이템 수령 결과
void EventContents::Recv_DailyCheckEvent_ItemGet(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_DAILY_CHECK_EVENT_ITEM_REQUEST_RESULT recv = *static_cast< GS2C_RECV_DAILY_CHECK_EVENT_ITEM_REQUEST_RESULT* >( pData );

	m_DailyCheckEventData.bWaitRecv = false;

	if( nsDailyCheckEventResult::SUCCESS != recv.nResult )
	{
		cPrintMsg::PrintMsg( recv.nResult );
		// 해당 이벤트가 시간이 아닐 때
		if( nsDailyCheckEventResult::NOT_TIME == recv.nResult )
		{
			m_DailyCheckEventData.ResetCheckListData( recv.nGroupNo );
			Notify( eDailyCheckEvent_NotTime );		// 이벤트 정보 클리어
		}
		return;
	}

	// 출석 체크
	m_DailyCheckEventData.SetDailyCheckList( recv.nGroupNo, recv.nCurrentDay, recv.nNextLeftTS ); // 서버에서 초로 날아옴

	// 인벤에 아이템 등록
	InsertItemToInventory( recv.lCheckedItems );

	Notify( eDailyCheckEvent_UpdateItem );
#endif
}

void EventContents::Update_DailyCheckEvent_Timer(void* pData)
{
	SAFE_POINTER_RET( pData );
	UINT nGroupID = *static_cast< UINT* >( pData );

	nGroupID -= IDEVENT_CHECK_EXP_TIMER;
	if( m_DailyCheckEventData.SetNextDailyCheck( nGroupID ) )
	{
		Notify( eDailyCheckEvent_UpdateItem );
		return;
	}

	// 출석부 이벤트 종료
	m_DailyCheckEventData.ResetCheckListData( nGroupID );
	Notify( eDailyCheckEvent_NotTime );
}

bool EventContents::CheckInventory(EventContents::sCheckItemInfo const* pCheckItem)
{
	SAFE_POINTER_RETVAL( pCheckItem, false );
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	int nNeedSlot = 0;
	std::map< uint, uint >::const_iterator cit = pCheckItem->GetCheckItems().begin();
	for( ; cit != pCheckItem->GetCheckItems().end(); ++cit )
	{
		int nInvenIndex = pInven->GetSlot_Item_ID_ReverseToSlot( -1, cit->first );

		// 인벤에 빈자리 필요
		if( cData_Inven::INVALIDE_INVEN_INDEX == nInvenIndex )
			nNeedSlot++;

		if( !pInven->IsSlot( cit->first, cit->second ) )
			return false;
	}

	if( nNeedSlot > pInven->GetEmptySlotCount() )
		return false;

	return true;
}

#if COMMON_LIB_FIXED
void EventContents::InsertItemToInventory(std::list< nsDailyCheckEvent::sDailyCheckedItem >& lCheckedItems)
{
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	std::list< nsDailyCheckEvent::sDailyCheckedItem >::iterator it = lCheckedItems.begin();
	for( ; it != lCheckedItems.end(); ++it )
		pInven->SetInvenItemData( it->nItemPos, it->cItem );
}
#endif

bool EventContents::IsHaveDailyCheckEvent() const
{
	return m_DailyCheckEventData.GetGroupCount() == 0 ? false : true;
}

uint EventContents::GetDailyCheckEventCount() const
{
	return m_DailyCheckEventData.GetGroupCount();
}

void EventContents::GetDailyCheckEventInfo(int const& nGroupIndex, std::wstring& wsTitle, std::wstring& wsDesc, std::wstring& wsStart, std::wstring& wsEnd)
{
	SAFE_POINTER_RET( nsCsFileTable::g_pEventMng );
	uint nGroupID = m_DailyCheckEventData.GetGroupID( nGroupIndex );
	CsEventTable::sDailyCheckTime const* pDCheck = nsCsFileTable::g_pEventMng->GetDailyCheck( nGroupID );
	SAFE_POINTER_RET( pDCheck );
	wsTitle = pDCheck->wsEventTitle;
	wsDesc = pDCheck->wsEventDescript;
	wsStart = pDCheck->wsStartTime;
	wsEnd = pDCheck->wsEndTime;
}

EventContents::sCheckGroupInfo const* EventContents::GetDailyCheckGroupInfo(int const& nGroupIndex) const
{
	return m_DailyCheckEventData.GetCheckGroupInfo( nGroupIndex );
}

std::wstring EventContents::GetSubTitleName(int const& nMainIndex, int const& nSubIndex) const
{
	switch( nMainIndex )
	{
	case 3:	// 출석부 이벤트
		return m_DailyCheckEventData.GetGroupName( nSubIndex );
	}

	return L"";
}

//////////////////////////////////////////////////////////////////////////
// 출석부 아이템 정보
void EventContents::sCheckItemInfo::SetCheckItem(std::map< uint, uint >& mapItems, std::wstring const& wsDate, bool const& bCheck)
{
	bChecked = bCheck;
	wsCheckDate = wsDate;

	std::swap( mapItems, mapCheckItems );
}

void EventContents::sCheckItemInfo::SetCheckState(bool const& bCheck)
{
	bChecked = bCheck;
}

std::map< uint, uint > const& EventContents::sCheckItemInfo::GetCheckItems() const
{
	return mapCheckItems;
}

std::wstring EventContents::sCheckItemInfo::GetCheckDate() const
{
	return wsCheckDate;
}

bool EventContents::sCheckItemInfo::GetCheckState() const
{
	return bChecked;
}

//////////////////////////////////////////////////////////////////////////
// 출석부 그룹 정보 - 아이템 리스트
#if COMMON_LIB_FIXED
void EventContents::sCheckGroupInfo::InitCheckInfo(nsDailyCheckEvent::sDailyCheckData& cDailyInfo)
{
	nCheckGroupID = cDailyInfo.nGroupCode;
	nCheckIndex = cDailyInfo.nCurrentDay;

	SetCheckNextTimer( cDailyInfo.nNextLeftTS );

	// 파일 테이블로부터 출석부 정보 세팅
	SAFE_POINTER_RET( nsCsFileTable::g_pEventMng );
	CsEventTable::sDailyCheckTime const* pDCheck = nsCsFileTable::g_pEventMng->GetDailyCheck( nCheckGroupID );
	SAFE_POINTER_RET( pDCheck );
	CsEventTable::LIST_DCHECK_ITEM_CIT it = pDCheck->lDailyRewards.begin();
	for( int i = 1; it != pDCheck->lDailyRewards.end(); ++it, ++i )
	{
		std::map< uint, sCheckItemInfo >::iterator itItem = mapCheckInfos.find( i );
		if( itItem != mapCheckInfos.end() )
			continue;

		auto ib = mapCheckInfos.insert( std::make_pair( i, sCheckItemInfo() ) );
		if( !ib.second )
			continue;

		itItem = ib.first;
		// 나중에 여러 개 세팅할 때를 대비해서
		std::map< uint, uint > mapItems;
		mapItems.insert( std::make_pair( it->nRewardItemID, it->nRewardItemCnt ) );
		itItem->second.SetCheckItem( mapItems, it->wsCheckData, cDailyInfo.cEventInfo.IsCheck( i ) );
	}
}
#endif

void EventContents::sCheckGroupInfo::SetCheckNextTimer(uint const& nLeftTS)
{
	nCheckNextLeftTS = nLeftTS;
	::SetTimer( GAMEAPP_ST.GetHWnd(), IDEVENT_CHECK_EXP_TIMER + nCheckGroupID, nCheckNextLeftTS * 1000, (TIMERPROC)(EventContents::ExpEventCheckTimerProc) );
}

bool EventContents::sCheckGroupInfo::SetCheckNextIndex()
{
	// 더 이상 출석할 일이 없음
	if( mapCheckInfos.size() == nCheckIndex )
	{
		ResetCheckInfo();
		return false;
	}

	nCheckIndex++;
	nCheckNextLeftTS = 86403 /* a day + delay time */;
	::SetTimer( GAMEAPP_ST.GetHWnd(), IDEVENT_CHECK_EXP_TIMER + nCheckGroupID, nCheckNextLeftTS * 1000, (TIMERPROC)(EventContents::ExpEventCheckTimerProc) );
	return true;
}

void EventContents::sCheckGroupInfo::SetCheckInfo(uint const& nGroupID, uint const& nTodayIndex, uint const& nNextLeftTS)
{
	if( nCheckGroupID != nGroupID )
		return;

	nCheckIndex = nTodayIndex;
	SetCheckNextTimer( nNextLeftTS );

	std::map< uint, sCheckItemInfo >::iterator it = mapCheckInfos.find( nCheckIndex );
	if( it == mapCheckInfos.end() )
		return;

	it->second.SetCheckState( true );
}

void EventContents::sCheckGroupInfo::ResetCheckInfo()
{
	::KillTimer( GAMEAPP_ST.GetHWnd(), IDEVENT_CHECK_EXP_TIMER + nCheckGroupID );
}

EventContents::sCheckItemInfo const* EventContents::sCheckGroupInfo::GetCheckItemInfo(uint const& nIndex) const
{
	std::map< uint, sCheckItemInfo >::const_iterator cit = mapCheckInfos.find( nIndex );
	if( cit == mapCheckInfos.end() )
		return NULL;

	return &cit->second;
}

EventContents::sCheckItemInfo const* EventContents::sCheckGroupInfo::GetCheckableItemInfo() const
{
	std::map< uint, sCheckItemInfo >::const_iterator cit = mapCheckInfos.find( nCheckIndex );
	if( cit == mapCheckInfos.end() )
		return NULL;

	return &cit->second;
}

uint EventContents::sCheckGroupInfo::GetCheckItemListCount() const
{
	return mapCheckInfos.size();
}

uint EventContents::sCheckGroupInfo::GetCheckIndex() const
{
	return nCheckIndex;
}

bool EventContents::sCheckGroupInfo::IsCurrentGroupID(uint const& nGroupID) const
{
	return nCheckGroupID == nGroupID ? true : false;
}

bool EventContents::sCheckGroupInfo::IsCheckable() const
{
	std::map< uint, sCheckItemInfo >::const_iterator cit = mapCheckInfos.find( nCheckIndex );
	if( cit == mapCheckInfos.end() )
		return false;

	return !( cit->second.GetCheckState() );
}

bool EventContents::sCheckGroupInfo::IsNextCheckTime()
{
	if( nCheckIndex >= mapCheckInfos.size() )
		return false;

	return cLeftTimer.IsEnable();
}

//////////////////////////////////////////////////////////////////////////
// 출석부 이벤트 정보
#if COMMON_LIB_FIXED
void EventContents::sDailyCheckEventData::InitDailyCheckList(nsDailyCheckEvent::sDailyCheckData& cDailyInfo)
{
	sCheckGroupInfo sGroupInfo;
	sGroupInfo.InitCheckInfo( cDailyInfo );
	lDailyCheck.push_back( sGroupInfo );
}
#endif

void EventContents::sDailyCheckEventData::SetDailyCheckList(uint const& nGroupID, uint const& nTodayIndex, uint const& nNextLeftTS)
{
	std::list< sCheckGroupInfo >::iterator it = lDailyCheck.begin();
	for( ; it != lDailyCheck.end(); ++it )
		it->SetCheckInfo( nGroupID, nTodayIndex, nNextLeftTS );
}

bool EventContents::sDailyCheckEventData::SetNextDailyCheck(uint const& nGroupID)
{
	std::list< sCheckGroupInfo >::iterator it = lDailyCheck.begin();
	for( ; it != lDailyCheck.end(); ++it )
	{
		if( !it->IsCurrentGroupID( nGroupID ) )
			continue;

		if( it->SetCheckNextIndex() )
			return true;
	}
	return false;
}

void EventContents::sDailyCheckEventData::ResetData()
{
	std::list< sCheckGroupInfo >::iterator it = lDailyCheck.begin();
	for( ; it != lDailyCheck.end(); ++it )
		it->ResetCheckInfo();
	lDailyCheck.clear();
}

void EventContents::sDailyCheckEventData::ResetCheckListData(uint const& nGroupID)
{
	std::list< sCheckGroupInfo >::iterator it = lDailyCheck.begin();
	for( ; it != lDailyCheck.end(); )
	{
		if( !it->IsCurrentGroupID( nGroupID ) )
		{
			++it;
			continue;
		}

		it->ResetCheckInfo();		// Kill Timer
		it = lDailyCheck.erase( it );
		break;
	}
}

uint EventContents::sDailyCheckEventData::GetGroupID(int const& nIndex) const
{
	std::list< sCheckGroupInfo >::const_iterator it = lDailyCheck.begin();
	for( int i = 0; it != lDailyCheck.end(); ++it, ++i )
	{
		if( nIndex == i )
			return it->nCheckGroupID;
	}
	return 0;
}

uint EventContents::sDailyCheckEventData::GetGroupCount() const
{
	return lDailyCheck.size();
}

std::wstring EventContents::sDailyCheckEventData::GetGroupName(int const& nIndex) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pEventMng, L"" );
	CsEventTable::sDailyCheckTime const* pDCheck = nsCsFileTable::g_pEventMng->GetDailyCheck( GetGroupID( nIndex ) );
	SAFE_POINTER_RETVAL( pDCheck, L"" );
	return pDCheck->wsEventTab;
}

EventContents::sCheckGroupInfo const* EventContents::sDailyCheckEventData::GetCheckGroupInfo(int const& nIndex) const
{
	std::list< sCheckGroupInfo >::const_iterator it = lDailyCheck.begin();
	for( int i = 0; it != lDailyCheck.end(); ++it, ++i )
	{
		if( nIndex == i )
			return &(*it);
	}
	return NULL;
}

EventContents::sCheckItemInfo const* EventContents::sDailyCheckEventData::GetCheckableItemInfo(int const& nIndex) const
{
	sCheckGroupInfo const* pGroupInfo = GetCheckGroupInfo( nIndex );
	SAFE_POINTER_RETVAL( pGroupInfo, NULL );
	return pGroupInfo->GetCheckableItemInfo();
}

bool EventContents::sDailyCheckEventData::IsCheckable(int const& nIndex) const
{
	sCheckGroupInfo const* pGroupInfo = GetCheckGroupInfo( nIndex );
	SAFE_POINTER_RETVAL( pGroupInfo, NULL );
	return pGroupInfo->IsCheckable();
}

bool EventContents::sDailyCheckEventData::IsHaveDailyCheckEvent() const
{
	return lDailyCheck.size() > 0 ? true : false;
}
