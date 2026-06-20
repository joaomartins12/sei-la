#include "StdAfx.h"
#include "ShutDownContents.h"

#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "../../ContentsSystem/ContentsSystemDef.h"

#define MIN_COMPULSORY_SHUTDOWN_PLAYER_AGE		16	// 강제 셧다운 적용 나이(만)
#define MIN_SELECTION_SHUTDOWN_PLAYER_AGE		18	// 선택적 셧다운 적용 나이(만)

//#define SHUTDOWN_CHECK_START_HOUR	0	// 셧다운 시작 시간
#define SHUTDOWN_CHECK_END_HOUR		6	// 셧다운 끝 시간
#define DAY_TO_SEC			86400		// 하루를 초로 환산한 값 24 * 60 * 60

#define MIN_30		30 * 60
#define MIN_10		10 * 60

inline bool CheckUserAge( CShutDownContents::sPlayerTimeData const& PlayerData, int nCheckPlayerAge )
{
	SAFE_POINTER_RETVAL( net::net, false );

	// 현재 시간
	tm time;
	nBase::GetTime2TM( net::net->GetTimeTS(), time );
	time.tm_year += 1900;
	time.tm_mon += 1;

	int nCurrentAge = time.tm_year - PlayerData.m_nShutDown_Year;
	// 현재 나이가 셧다운 제한 나이보다 작으면 검사 함.
	if( nCurrentAge < nCheckPlayerAge )
		return true;
	else if( nCurrentAge == nCheckPlayerAge )
	{
		// 달 체크
		if( time.tm_mon < PlayerData.m_nShutDown_Month )
			return true;
		else if( time.tm_mon == PlayerData.m_nShutDown_Month )
		{
			// 일 체크
			if( time.tm_mday < PlayerData.m_nShutDown_Day )
				return true;
		}
	}

	return false;
}

int const CShutDownContents::IsContentsIdentity(void)
{
#ifdef USE_SHUTDOWN
	return E_CT_SHUTDOWN_SYSTEM;
#else
	return E_CT_END;
#endif
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CShutDownContents::sPlayerTimeData::SetData( char const* pData)
{
	char cToken[ 16 ] = {0,};
	int nIndex = 0;

	memset( cToken, 0, sizeof( char )*16 );
	for( int i=0; i<4; ++i )
	{
		cToken[ i ] = pData[ nIndex ];
		++nIndex;
	}
	m_nShutDown_Year = atoi( cToken );

	memset( cToken, 0, sizeof( char )*16 );
	for( int i=0; i<2; ++i )
	{
		cToken[ i ] = pData[ nIndex ];
		++nIndex;
	}
	m_nShutDown_Month = atoi( cToken );

	memset( cToken, 0, sizeof( char )*16 );
	for( int i=0; i<2; ++i )
	{
		cToken[ i ] = pData[ nIndex ];
		++nIndex;
	}

	m_nShutDown_Day = atoi( cToken );
}
//////////////////////////////////////////////////////////////////////////
// 강제적 셧다운 데이터 셋팅
void CShutDownContents::sCompulsoryShutdown::Init()
{
	m_bShutDown_Check = true;
}

void CShutDownContents::sCompulsoryShutdown::Check( sPlayerTimeData const& PlayerData )
{
	if( !m_bShutDown_Check )
		return;

	m_bShutDown_Check = CheckUserAge( PlayerData, MIN_COMPULSORY_SHUTDOWN_PLAYER_AGE );
}

bool CShutDownContents::sCompulsoryShutdown::Update_Check( int nLastCheckHour )
{
	if( !m_bShutDown_Check )
		return false;

	// 셧다운 적용 시간 체크
	if( nLastCheckHour < SHUTDOWN_CHECK_END_HOUR )
		return true;

	return false;
}
//////////////////////////////////////////////////////////////////////////
// 선택적 셧다운 데이터 셋팅
void CShutDownContents::sSelectionShutdown::SetData( sShutdownWeek pData )
{
	m_bSelectionShutDown_Check = true;
	m_sSelectionShutDownData = pData;
}

void CShutDownContents::sSelectionShutdown::Check( sPlayerTimeData const& PlayerData )
{
	if( !m_bSelectionShutDown_Check )
		return;

	m_bSelectionShutDown_Check = CheckUserAge( PlayerData, MIN_SELECTION_SHUTDOWN_PLAYER_AGE );
}

bool CShutDownContents::sSelectionShutdown::Update_Check( tm const& unSerTime )
{
	if( m_sSelectionShutDownData.bCheckShutdown <= 0 )
	{
		m_bSelectionShutDown_Check = false;
		return false;
	}

	return m_sSelectionShutDownData.Check( unSerTime );
}

time_t CShutDownContents::sSelectionShutdown::GetShutDownStartTime( tm const& currentTime )
{
	time_t nextTime = 0;
	if( m_sSelectionShutDownData.bCheckShutdown <= 0 )
		return nextTime;

	int mMaxCount = 7 * 24;
	int nCurCount = 0;
	tm tmTime = currentTime;
	tmTime.tm_min = 0;
	tmTime.tm_sec = 0;

	do 
	{
		if( nCurCount > mMaxCount )
		{
			nextTime = 0;
			break;
		}

		tmTime.tm_hour +=1;
		nextTime = mktime( &tmTime );
		nCurCount++;
	} while ( !m_sSelectionShutDownData.Check( tmTime ) );

	return nextTime;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CShutDownContents::CShutDownContents(void)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_SHUTDOWNSYSTEM_DATA, this, &CShutDownContents::RecvShutDownData );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_WEEK_SHUTDOWN_DATA, this, &CShutDownContents::RecvWeekShutDownData );
}

CShutDownContents::~CShutDownContents(void)
{
	GAME_EVENT_ST.DeleteEventAll( this );
	::KillTimer( GAMEAPP_ST.GetHWnd(), SELECTION_TIMER );
	::KillTimer( GAMEAPP_ST.GetHWnd(), COMPULSORY_TIMER );
}

int const CShutDownContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CShutDownContents::Initialize(void)
{
	return true;
}

void CShutDownContents::UnInitialize(void)
{
}

bool CShutDownContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void CShutDownContents::Update(float const& fElapsedTime)
{
}

void CShutDownContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void CShutDownContents::MakeMainActorData(void)
{
}

void CShutDownContents::ClearMainActorData(void)
{
	ClearAllRegistered();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


void CALLBACK CShutDownContents::EventTimerProc( HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime )
{
	switch( idEvent )
	{
	case COMPULSORY_TIMER:
		{
			::KillTimer( GAMEAPP_ST.GetHWnd(), COMPULSORY_TIMER );
			cPrintMsg::PrintMsg( 30445 );// 셧다운 메시지 출력
		}		
		break;
	case COMPULSORY_TIMER_NOTICE:
		{
			::KillTimer( GAMEAPP_ST.GetHWnd(), COMPULSORY_TIMER_NOTICE );
			cPrintMsg::PrintMsg( 30444 );// 셧다운 될꺼라는 메시지 출력
		}
		break;
	case SELECTION_TIMER:
		{
			::KillTimer( GAMEAPP_ST.GetHWnd(), SELECTION_TIMER );
			cPrintMsg::PrintMsg( 30442 );// 셧다운 메시지 출력
		}		
		break;
	case SELECTION_TIMER_NOTICE_10:// 종료 10분전에 출력
		{
			::KillTimer( GAMEAPP_ST.GetHWnd(), SELECTION_TIMER_NOTICE_10 );
			// 10분 후에 SELECTION_TIMER 이벤트 호출
			cPrintMsg::PrintMsg( 30441 );// 셧다운 될꺼라는 메시지 출력
			::SetTimer( GAMEAPP_ST.GetHWnd(), SELECTION_TIMER, 10 * 60 * 1000, (TIMERPROC)(CShutDownContents::EventTimerProc) );
		}		
		break;
	case SELECTION_TIMER_NOTICE_30:// 종료 30분전에 출력
		{
			::KillTimer( GAMEAPP_ST.GetHWnd(), SELECTION_TIMER_NOTICE_30 );
			// 20분 후에 SELECTION_TIMER_NOTICE_10 이벤트 호출
			cPrintMsg::PrintMsg( 30440 );// 셧다운 될꺼라는 메시지 출력
			::SetTimer( GAMEAPP_ST.GetHWnd(), SELECTION_TIMER_NOTICE_10, 20 * 60 * 1000, (TIMERPROC)(CShutDownContents::EventTimerProc) );
		}		
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
// 로그인시 유져 데이터 셋팅
void CShutDownContents::RecvShutDownData(void* pData)
{
	SAFE_POINTER_RET( pData );

	GS2C_RECV_Shutdown_Birthday* pRecv = static_cast<GS2C_RECV_Shutdown_Birthday*>( pData );
	if( pRecv->szBirthDay.empty() )
		return;

	m_sPlayerTimeData.SetData( pRecv->szBirthDay.c_str() );

	m_sCompulsoryShutDown.Init();
	m_sCompulsoryShutDown.Check( m_sPlayerTimeData );
	
	// 강제 셧다운 제도가 적용되는 유저일 경우
	if( m_sCompulsoryShutDown.m_bShutDown_Check )
	{
		uint uServerTime = net::net->GetTimeTS();
		tm time;
		nBase::GetTime2TM( uServerTime, time );

		// 0~6시 안에 들어온 유저이면 강제 종료
		if( m_sCompulsoryShutDown.Update_Check( time.tm_hour ) )
		{
			cPrintMsg::PrintMsg( 30445 );// 강제 셧다운 메시지 출력
		}
		else
		{	
			// 현재 접속한 시간이 23시이면 메시지 출력
			if( time.tm_hour == 23 )
				cPrintMsg::PrintMsg( 30444 );
			else
			{
				// 23시에 출력할 이벤트 등록
				DWORD dwCurSec = (time.tm_hour * 60 * 60) + (time.tm_min * 60);
				UINT uElapse = DAY_TO_SEC - dwCurSec - (60*60);
				::SetTimer( GAMEAPP_ST.GetHWnd(), COMPULSORY_TIMER_NOTICE, uElapse * 1000, (TIMERPROC)(CShutDownContents::EventTimerProc) );
			}

			// 아닐경우 0시에 호출될 이벤트 등록
			DWORD dwCurSec = (time.tm_hour * 60 * 60) + (time.tm_min * 60);
			UINT uElapse = DAY_TO_SEC - dwCurSec;
			::SetTimer( GAMEAPP_ST.GetHWnd(), COMPULSORY_TIMER, uElapse * 1000, (TIMERPROC)(CShutDownContents::EventTimerProc) );
		}
	}
}

// 로그인시 선택적 셧다운 데이터 시간 셋팅
void CShutDownContents::RecvWeekShutDownData(void* pData)
{
	SAFE_POINTER_RET( pData );
	sShutdownWeek* pShutWeekData = static_cast<sShutdownWeek*>(pData);

	m_sSelectionShutDown.SetData( *pShutWeekData );
	m_sSelectionShutDown.Check( m_sPlayerTimeData );

	if( m_sSelectionShutDown.m_bSelectionShutDown_Check )// 선택적 셧다운 데이터가 적용된 유저
	{
		uint uServerTime = net::net->GetTimeTS();
		tm currTime;
		nBase::GetTime2TM( uServerTime, currTime ); // 300초 이전으로 처리 2013/4/4 vf00

		// 현제 선택적 셧다운이 적용된 시간에 접속했는지 체크
		if( m_sSelectionShutDown.Update_Check( currTime ) )
			cPrintMsg::PrintMsg( 30442 );// 셧다운 메시지 출력
		else
		{	// 아닐경우
			time_t shutDownTime = m_sSelectionShutDown.GetShutDownStartTime(currTime);;
			if( shutDownTime > 0 )
			{
				time_t currTimeT = mktime( &currTime );
				double diffTime = difftime( shutDownTime, currTimeT );

				int nEventType = SELECTION_TIMER;
				if( diffTime >= MIN_30 )// 남은 시간이 30분 이상일 경우
				{
					nEventType = SELECTION_TIMER_NOTICE_30;
					diffTime -= MIN_30;
				}else if( diffTime >= MIN_10 )// 남은 시간이 10분 이상일 경우
				{
					nEventType = SELECTION_TIMER_NOTICE_10;
					diffTime -= MIN_10;
				}

				::SetTimer( GAMEAPP_ST.GetHWnd(), nEventType, diffTime * 1000, (TIMERPROC)(CShutDownContents::EventTimerProc) );
			}
		}
	}
}