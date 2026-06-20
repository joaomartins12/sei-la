


#pragma once 

class cData_Attendance : public NiMemObject
{
	//===========================================================================
	//
	//		º£ÀÌ½º
	//
	//===========================================================================
public:
	void		Delete();
	void		Init();
	void		InitNetOff();

protected:
	n4			m_nWorkDayHistory;
	u4			m_nTodayAttendanceTimeTS;

	bool		m_bEnableAttendance;
	bool		m_bEnableEvent;

public:
	void		SetWorkDayHistory( n4 nWorkDayHistory ){ m_nWorkDayHistory = nWorkDayHistory; }
	void		SetTodayAttendanceTimeTS( u4 nTodayAttendanceTimeTS );

	u4			GetTodayAttendanceTimeTS(){ return m_nTodayAttendanceTimeTS; }
	bool		IsEnableAttendance(){ return m_bEnableAttendance; }

	void		SetEnableAttendance( bool bEnable ){ m_bEnableAttendance = bEnable; }
};