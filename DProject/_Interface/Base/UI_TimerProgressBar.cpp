
#include "stdafx.h"
#include "UI_TimerProgressBar.h"

cUI_TimerProgressBar::cUI_TimerProgressBar():
m_bPlay(false),m_fTickTime(0.0f)
,m_eTimerType(eAutoSimple),m_eShowValueType(ePassingingTime)
{
}

cUI_TimerProgressBar::~cUI_TimerProgressBar()
{
}


void cUI_TimerProgressBar::Update(float const& fDeltaTime)
{
	if( !m_bPlay )
		return;

	m_fTickTime += fDeltaTime;

	if( m_fTickTime < m_fStep )
		return;

	switch( m_DirType )
	{
	case RIGHT_LEFT:	AdjustProgress( -m_fTickTime );	break;
	default:			AdjustProgress( m_fTickTime );	break;
	}

	if( IsMaxGuage() )
		EndTimer();

	while( m_fTickTime > m_fStep )
		m_fTickTime -= m_fStep;
}

void cUI_TimerProgressBar::UpdateShowString()
{
	m_ProgressString.clear();

	if( m_ViewTextType == NONE )
		return;

	UINT nTime = 0;
	switch( m_eShowValueType )
	{
	case ePassingingTime:	nTime = (UINT)GetCurrentVale();		break;
	case eRemainingTime:	nTime = (UINT)GetRemainVale();		break;
	}

	if( !m_HeaderText.empty() )
		m_ProgressString = m_HeaderText;

	std::wstring time;
	switch( m_eTimerType )
	{
	case eHourMinSec:	// 시분초 보여준다.
		{
			UINT nHour = nTime / 3600;	
			UINT nRemain = nTime % 3600;
			UINT nMin = nRemain / 60;
			UINT nSec = nRemain % 60 ;
			DmCS::StringFn::Format( time, L"%02u%s%02u%s%02u%s", nHour, m_textHour.c_str(), nMin, m_textMin.c_str(), nSec, m_textSec.c_str() );
		}break;
	case eMinSec:		// 분초 보여준다.
		{
			UINT nMin = nTime / 60;
			UINT nSec = nTime % 60;
			DmCS::StringFn::Format( time, L"%02u%s%02u%s", nMin, m_textMin.c_str(), nSec, m_textSec.c_str() );
		}break;
	case eSec:			// 초만 보여준다.
		{
			DmCS::StringFn::Format( time, L"%02u%s", nTime, m_textSec.c_str() );
		}break;
	case eAutoSimple:	// 자동으로 0인 시간은 제거하여 남은 시간만 표시해 준다.
		{
			UINT nHour = nTime / 3600;	
			UINT nRemain = nTime % 3600;
			UINT nMin = nRemain / 60;
			UINT nSec = nRemain % 60 ;

			if( nHour > 0 )
				DmCS::StringFn::Format( time, L"%02u%s%02u%s%02u%s", nHour, m_textHour.c_str(), nMin, m_textMin.c_str(), nSec, m_textSec.c_str() );
			else if( nMin > 0 )
				DmCS::StringFn::Format( time, L"%02u%s%02u%s", nMin, m_textMin.c_str(), nSec, m_textSec.c_str() );
			else
				DmCS::StringFn::Format( time, L"%02u%s", nSec, m_textSec.c_str() );
		}
		break;
	case eFirstTime:
		{
			UINT nDay = nTime / (3600 * 24);
			UINT nHour = nTime / 3600;				
			UINT nRemain = nTime % 3600;
			UINT nMin = nRemain / 60;
			UINT nSec = nRemain % 60 ;
			if( nDay > 0 )
				DmCS::StringFn::Format( time, L"%02u%s", nDay, m_textDay.c_str() );
			else if( nHour > 0 )
				DmCS::StringFn::Format( time, L"%02u%s", nHour, m_textHour.c_str() );
			else if( nMin > 0 )
				DmCS::StringFn::Format( time, L"%02u%s", nMin, m_textMin.c_str() );
			else
				DmCS::StringFn::Format( time, L"%02u%s", nSec, m_textSec.c_str() );
		}
		break;
	}

	m_ProgressString += time;

	if( !m_TailText.empty() )
		m_ProgressString += m_TailText;

	if( m_pRenderText )
		m_pRenderText->SetText( m_ProgressString.c_str() );
}

void cUI_TimerProgressBar::SetTimer( float fCurrentTime, float fTotalTime )
{
	SetRange( fTotalTime );
	SetProgressPos( fCurrentTime );
}

void cUI_TimerProgressBar::Start()
{
	m_bPlay = true;
	m_fTickTime = 0.0f;
}

void cUI_TimerProgressBar::Stop()
{
	m_bPlay = false;
}

void cUI_TimerProgressBar::Reset()
{
	m_bPlay = false;
}

void cUI_TimerProgressBar::EndTimer()
{
	Stop();
}

void cUI_TimerProgressBar::SetTimerType( eTimerType eType )
{
	m_eTimerType = eType;
}

void cUI_TimerProgressBar::SetShowValueType( eShowValueType eValueType )
{
	m_eShowValueType = eValueType;
}

void cUI_TimerProgressBar::SetTimerText( std::wstring const& sDay, std::wstring const& sHour, std::wstring const& sMin, std::wstring const& sSec )
{
	m_textDay = sDay;
	m_textHour = sHour;
	m_textMin = sMin;
	m_textSec = sSec;
}

void cUI_TimerProgressBar::SetHeadTailText( std::wstring const& sHeader, std::wstring const& sTail )
{
	m_HeaderText = sHeader;
	m_TailText = sTail;
}

bool cUI_TimerProgressBar::IsPlay() const
{
	return m_bPlay;
}