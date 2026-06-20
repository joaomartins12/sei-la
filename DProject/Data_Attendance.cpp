
#include "stdafx.h"
#include "Data_Attendance.h"


void cData_Attendance::Delete()
{
}

void cData_Attendance::Init()
{
}

void cData_Attendance::InitNetOff()
{
}

void cData_Attendance::SetTodayAttendanceTimeTS( u4 nTodayAttendanceTimeTS )
{
	m_bEnableEvent = false;
	m_bEnableAttendance = false;

	u4 timeTS = _TIME_TS;

	m_nTodayAttendanceTimeTS = nTodayAttendanceTimeTS;
	if( m_nTodayAttendanceTimeTS && (m_nTodayAttendanceTimeTS <= timeTS) )
	{
		// 출석 체크 가능 시간입니다.
		m_bEnableEvent = true;
		m_bEnableAttendance = true;
	}
	// 이미 출석 체크 완료 상태이거나, 출석 이벤트와 연관이 없습니다.
	else if( m_nTodayAttendanceTimeTS == 0 )
	{
		// 출석 체크 진행중인지의 여부		
		time_t start	= mktime( &nsCsFileTable::g_pEventMng->GetAttendanceInfo()->s_StartTime );
		time_t end		= mktime( &nsCsFileTable::g_pEventMng->GetAttendanceInfo()->s_EndTime );
		if( ( timeTS >= start )&&( timeTS < end ) )
		{
			m_bEnableEvent = true;
			m_bEnableAttendance = true;
		}
		else
		{
			m_bEnableEvent = false;
			m_bEnableAttendance = false;
		}
	}
	else
	{
		// 아직 출석 요청 대기 시간이 남아있습니다. (cClient::GetTiemTS()-nTodayAttendanceTimeTS
		m_bEnableEvent = true;
		m_bEnableAttendance = false;
	}


	if( m_bEnableEvent == true )
	{
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_ATTENDANCE_EVENT ) == false )
		{
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_ATTENDANCE_EVENT, cBGSprite::ATTENDANCE_EVENT );
		}
	}
}

