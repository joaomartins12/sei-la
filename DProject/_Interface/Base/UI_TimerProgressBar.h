#pragma once 


class cUI_TimerProgressBar : public cUI_ProgressBar
{
public:
	enum eTimerType
	{
		eNone,
		eHourMinSec,	// 시분초 보여준다.
		eMinSec,		// 분초 보여준다.
		eSec,			// 초만 보여준다.
		eFirstTime,		// 
		eAutoSimple,	// 자동으로 0인 시간은 제거하여 남은 시간만 표시해 준다.
	};

	enum eShowValueType
	{
		ePassingingTime,	// 지나간 시간 표시
		eRemainingTime,		// 남은 시간 표시
	};

	cUI_TimerProgressBar();
	virtual ~cUI_TimerProgressBar();

	virtual void	Update(float const& fDeltaTime);
	virtual	void	UpdateShowString();
	virtual eTYPE	GetType(){ return T_Timer_Progress; }

	void			SetTimerType( eTimerType eType );
	void			SetShowValueType( eShowValueType eValueType );

	void			SetTimer( float fCurrentTime, float fTotalTime );
	void			Start();
	void			Stop();
	void			Reset();
	void			SetTimerText( std::wstring const& sDay, std::wstring const& sHour, std::wstring const& sMin, std::wstring const& sSec );
	void			SetHeadTailText( std::wstring const& sHeader, std::wstring const& sTail );
	bool			IsPlay() const;

private:
	void			EndTimer();

private:
	bool			m_bPlay;
	float			m_fTickTime;

	eTimerType		m_eTimerType;
	eShowValueType	m_eShowValueType;

	std::wstring	m_HeaderText;
	std::wstring	m_TailText;
	std::wstring	m_textHour;
	std::wstring	m_textMin;
	std::wstring	m_textSec;
	std::wstring	m_textDay;
};