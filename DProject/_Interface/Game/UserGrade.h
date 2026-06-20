
#pragma once


class cUserGrade : public NiMemObject
{
public:	
	static void			BeginLogo();

#ifdef VERSION_ENGLISH
	static void			BeginWemadeLogo();
#else
	static void			BeginGrade( bool bIncludeString );
#endif

	cUserGrade();
	virtual ~cUserGrade(){ Delete(); }

public:
	void				Delete();

#ifdef VERSION_ENGLISH
	void				LoopWemadeLogo();
#else
	void				InitGrade( bool bIncludeString );
#endif
	void				LoopLogo();	
	
	void				Render();

	void				ResetDevice();

	int					GetAlphaState(){ return m_nAlphaState; }
	bool				IsEnd(){ return m_EndTime.IsEnable(); }

protected:
	void				_UpdateLogo();

protected:
	CsEnableTimeSeq		m_EndTime;
	float				m_fCurAlpha;
	int					m_nAlphaState;
	float				m_fTimeValue;

	cStringList			m_StringList;
	cSprite*			m_pLogo1;
	cSprite*			m_pLogo2;

	// Å¸ÀÓ°ü·Ã
	DWORD				m_dwOldTime;
	float				m_fDeltaTime;

protected:
	void				_UpdateDeltaTime();
};

extern cUserGrade*		g_pUserGrade;
