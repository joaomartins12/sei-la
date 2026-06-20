#include "stdafx.h"
#include <NiMain.h>
#include <NiDebug.h>

#include "Global.h"
#include "CFrame.h"
 
//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//											     											//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
bool CFrame::MeasureTime()
{

    if (!m_bUseFixedTime)
    {
        // start performance measurements
        if (m_fLastTime == -1.0f)
        {
            m_fLastTime = NiGetCurrentTimeInSec();
            m_fAccumTime = 0.0f;
            m_iClicks = 0;
        }

        // measure time
        m_fCurrentTime = NiGetCurrentTimeInSec();
        float fDeltaTime = m_fCurrentTime - m_fLastTime;

#if defined(WIN32)
        // NVPerfHUD support!
        if (m_bNVPerfHUD && fDeltaTime == 0.0f)
            return true;
#endif

        if (fDeltaTime < 0.0f)
            fDeltaTime = 0.0f;
        m_fLastTime = m_fCurrentTime;
        m_fAccumTime += fDeltaTime;

        // frame rate limiter
        if (m_fAccumTime < (m_fLastFrame + m_fMinFramePeriod))
            return false;

        m_fFrameTime = m_fAccumTime - m_fLastFrame;
        m_fLastFrame = m_fAccumTime;

        return true;
    }
    else
    {
        m_fCurrentTime += m_fFixedTimeIncrement;
        m_fAccumTime = ((float)m_iClicks) * m_fFixedTimeIncrement;      
        m_fFrameTime = m_fAccumTime - m_fLastFrame;
        m_fLastFrame = m_fAccumTime;

        return false;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
bool CFrame::UpdateFrame()
{
	m_fFrameRate = 1.0f / m_fFrameTime;

    if( m_fAccumTime >= NI_INFINITY )
    {
        return false;
    }

	return true;
}



//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
float CFrame::GetFrameRate()
{
	static float s_fFrameRate  = 0.f;
	static uint  s_nFrameCount = 0;
	static uint  s_nPrevTick   = nBase::GetTickCount();

	++ s_nFrameCount;

	uint nElapseTick = nBase::GetTickCount() - s_nPrevTick;
	if(nElapseTick < 1000)
	{
		return s_fFrameRate;
	}

	s_fFrameRate = ((1000.f*s_nFrameCount) / nElapseTick);

//	s_fFrameRate = s_nFrameCount;
//	DBG("fps %d / sec", s_nFrameCount);


/*
    if (--m_iTimer == 0)
    {
        if (m_fAccumTime > 0.0f)
        {
            if (m_fAccumTime > m_fLastFrameRateTime)
            {
                m_fFrameRate = (m_iClicks - m_iLastFrameRateClicks) / 
                    (m_fAccumTime - m_fLastFrameRateTime);
            }
        }
        else
        {
            m_fFrameRate = 0.0f;
        }

        m_iTimer = m_iMaxTimer;
        m_fLastFrameRateTime = m_fAccumTime;
        m_iLastFrameRateClicks = m_iClicks;
    }

	if( m_fFrameRate )
	{
		s_fFrameRate = m_fFrameRate;
	}
//*/

	s_nFrameCount = 0;
	s_nPrevTick = nBase::GetTickCount();

	return s_fFrameRate;
}