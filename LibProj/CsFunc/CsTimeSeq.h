
#pragma once 

class CsTimeSeq
{
public:
	CsTimeSeq(){ m_dwTime = m_dwSetDeltaTime = 0; m_bEnableTime = true; }

private:
	bool			m_bEnableTime;
	DWORD			m_dwTime;
	DWORD			m_dwSetDeltaTime;

public:
	void			SetEnableTime( bool bEnableTime ){ m_bEnableTime = bEnableTime; }
	bool			IsEnableTime(){ return m_bEnableTime; }

	void			SetDeltaTime( DWORD dwTick ){ m_dwSetDeltaTime = dwTick; }	
	void			Reset(){ m_dwTime = 0; }
	void			OffReset(){ m_dwTime = GetTickCount(); }
	DWORD			CalDeltaTime(){ return ( GetTickCount() - m_dwTime ); }
	DWORD			RemainDeltaTime(){ return m_dwSetDeltaTime >= CalDeltaTime() ? m_dwSetDeltaTime - CalDeltaTime() : 0; }
	void			PassDeltaTime( int nSec ){ m_dwSetDeltaTime -= CsMin( m_dwSetDeltaTime, (UINT)( nSec*1000 ) ); }

	bool			IsEnable(){
		DWORD dwCurTime = GetTickCount();
		if( ( dwCurTime - m_dwTime ) < m_dwSetDeltaTime )
			return false;

		m_dwTime = dwCurTime;
		return true;
	}
};

class CsOnceTimeSeq
{
public:
	CsOnceTimeSeq(){ m_dwTime = m_dwSetDeltaTime = 0; m_bEnable = false; }

private:
	DWORD			m_dwTime;
	DWORD			m_dwSetDeltaTime;
	bool			m_bEnable;

public:
	void			SetDeltaTime( DWORD dwTick ){ m_dwSetDeltaTime = dwTick; }
	void			Reset(){ m_dwTime = GetTickCount(); m_bEnable = true; }
	void			OffEnable(){ m_dwTime = 0; m_bEnable = false; }

	bool			IsEnable(){
		if( m_bEnable == false )
			return false;

		DWORD dwCurTime = GetTickCount();
		if( ( dwCurTime - m_dwTime ) < m_dwSetDeltaTime )
			return false;

		m_bEnable = false;
		return true;
	}
};

class CsEnableTimeSeq
{
public:
	CsEnableTimeSeq(){ m_dwTime = m_dwSetDeltaTime = 0; }

private:
	DWORD			m_dwTime;
	DWORD			m_dwSetDeltaTime;

public:
	void			SetDeltaTime( DWORD dwTick ){ m_dwSetDeltaTime = dwTick; }
	void			SetDisable(){ m_dwTime = GetTickCount() + m_dwSetDeltaTime; }
	void			SetEnable(){ m_dwTime = 0; }

	bool			IsEnable(){
		if( GetTickCount() < m_dwTime )
			return false;
		return true;
	}
};

class CsCheckInTimeSeq
{
public:
	CsCheckInTimeSeq(){ m_dwTime = m_dwSetDeltaTime = 0; m_bEnable = false; m_nValue = -1; }

private:
	DWORD			m_dwTime;
	DWORD			m_dwSetDeltaTime;
	bool			m_bEnable;
	int				m_nValue;

public:
	void			SetDeltaTime( DWORD dwTick ){ m_dwSetDeltaTime = dwTick; }
	void			SetDisable(){ m_bEnable = false; }
	void			SetEnable(){ m_bEnable = true; m_dwTime = GetTickCount() + m_dwSetDeltaTime; }
	void			SetValue( int nValue ){ m_nValue = nValue; }
	bool			IsSameValue( int nValue ){ return ( m_nValue == nValue ); }

	bool			IsEnable(){
		if( ( m_bEnable == false )||( GetTickCount() < m_dwTime ) )
			return false;

		m_bEnable = false;
		return true;
	}
};

class CsCoolTimeSeq
{
public:
	CsCoolTimeSeq()
	{ 
		Reset(); 
		m_fEndSec = 0; 
		m_bIsReset = false;
	}

private:
	double		m_fStartTime;
	double		m_fEndSec;
	UINT		m_nResetTick;	// 초기화 발동 시간
	bool		m_bIsReset;		// 스킬 쿨타임 초기화 발동 유무

public:
	void		SetEndTime( double fSec ){ m_fEndSec = fSec; }
	void		Start( double fOffset = 0 ){ m_fStartTime = g_fAccumTime - fOffset; }
	void		Reset(){ m_fStartTime = -100000000; }
	bool		IsCoolTime(){ return ( ( g_fAccumTime - m_fStartTime ) < m_fEndSec ); }

	double		GetEndTime(){ return m_fEndSec; }
	double		GetProcTime(){ assert_cs( IsCoolTime() ); return ( g_fAccumTime - m_fStartTime ); }
	double		GetRemainTime(){ assert_cs( IsCoolTime() ); return m_fEndSec - ( g_fAccumTime - m_fStartTime ); }

	void		SetCoolReset( bool bReset ){ m_bIsReset = bReset; if( bReset ) { m_nResetTick = GetTickCount(); } }
	bool		IsCoolReset(){ return m_bIsReset; }
	UINT		GetResetProcTick(){ if(m_bIsReset) return ( GetTickCount() - m_nResetTick ); else return 0; }
};

#ifdef SERVER_WAITING_LINE
class CWaitingLineTimeSeq
{
public:
	CWaitingLineTimeSeq(){  m_bEnableTime = false; m_dwBothTime = m_dwSetedTime = 0; m_nCount = m_nMaxCount = 0; }

private:

	bool			m_bEnableTime;
	DWORD			m_dwBothTime, m_dwSetedTime;	
	int				m_nCount, m_nMaxCount;

public:

	void			SetEnableTime( bool bEnableTime ){ m_bEnableTime = bEnableTime; }
	bool			IsEnableTime(){ return m_bEnableTime; }

	void			Reset(){ m_nCount = 0; }

	void			SetEvent( int nCount, DWORD dwBothTime = 5000) // 이밴트 설정, 이밴트 개수, 시간간격으로 이밴트
	{
		m_nMaxCount = nCount;
		m_dwBothTime = dwBothTime;
		m_dwSetedTime = GetTickCount();

	}
	int			UpdatWaitingLine()
	{
		DWORD dwEvent = m_dwSetedTime + m_dwBothTime;

		if(GetTickCount() > dwEvent && (m_nMaxCount > m_nCount))
		{
			m_dwSetedTime = GetTickCount();
			m_nCount++;
		}
		return m_nMaxCount - m_nCount;
	}
	
};
#endif