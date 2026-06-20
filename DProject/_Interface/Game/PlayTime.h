
#pragma once 

#define IF_PLAYTIME_BAR_CNT			4

class cPlayTime : public cBaseWindow
{
public:
	cPlayTime();

public:
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_PLAYTIME; }
	virtual void				Destroy();
	virtual void				DeleteResource();
	virtual void				Create( int nValue = 0 );

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();

	virtual void				Render();	
	virtual void				ResetDevice();

	// 메모리잡힌 컨트롤
protected:

	// 포인터만 있는 컨트롤
protected:	
	cSprite*		m_pBG[ 3 ];
	cProgressBar*	m_pBar[ IF_PLAYTIME_BAR_CNT ];
	int				m_nRenderBarIndex;
	int				m_nServerOldDay;
	CsTimeSeq		m_DayTimeSeq;
	cText*			m_pText;

public:
	void			RecreateBG();
	void			CalServerDay();

protected:
	void			_RenderText();
	void			_CheckServerDay();
};




