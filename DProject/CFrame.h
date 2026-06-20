#ifndef __FAME_H__
#define __FAME_H__

class CFrame
{
public:
	CFrame()
	{
		m_bNVPerfHUD = false;
		m_bUseFixedTime = false;

		m_fLastTime = -1.0f;
		m_fAccumTime = 0.0f;
		m_fCurrentTime = 0.0f;
		m_fFrameTime = 0.0f;
		m_iClicks = 0;
		m_iTimer = 1;
		m_iMaxTimer = 30;
		m_fFrameRate = 0.0f;
		m_fLastFrameRateTime = 0.0f;
		m_iLastFrameRateClicks = 0;

		m_fMinFramePeriod = 1.0f / 300.0f;
		m_fLastFrame = 0.0f;

		m_fFixedTimeIncrement = 1.0f / 60.0f;
	}

	virtual ~CFrame()
	{
	}

	bool MeasureTime();
	bool UpdateFrame();
	float GetFrameRate();

public:
	bool m_bNVPerfHUD;
	bool m_bUseFixedTime;

    // frame rate counting and display
    int m_iMaxTimer;
    int m_iTimer;
    float m_fCurrentTime; // Time reported by system
    float m_fLastTime; // Last time reported by system
    float m_fAccumTime; // Time elapsed since application start
    float m_fFrameTime; // Time elapsed since previous frame
    int m_iClicks;
    float m_fFrameRate;
    float m_fLastFrameRateTime;
    int m_iLastFrameRateClicks;

    float m_fMinFramePeriod;
    float m_fLastFrame;
	
	float m_fFixedTimeIncrement;
};

#endif