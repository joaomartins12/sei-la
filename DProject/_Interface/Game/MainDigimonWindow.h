
#pragma once 


class cMainDigimonWindow : public cBaseWindow
{
public:
	cMainDigimonWindow();

public:
	virtual eWINDOW_TYPE		GetWindowType(){ return WT_MAIN_DIGIMON_WINDOW; }
	virtual void				Destroy();
	virtual void				DeleteResource();
	virtual void				Create( int nValue = 0 );

	virtual void				PlayOpenSound(){}
	virtual void				PlayCloseSound(){}

	virtual void				Update(float const& fDeltaTime);
	virtual eMU_TYPE			Update_ForMouse();
	virtual void				Render();	
	virtual void				ResetDevice();

	virtual	void				OnLButtonUp( CsPoint pos );

protected:
	void						_UpdateBattle();

protected:
	CsC_AvObject*	m_pTargetObject;
	
	// 메모리잡힌 컨트롤
protected:
	cSprite*		m_pWindowBG;	
	cSprite*		m_pLevelBG_Normal;	
	cText*			m_pBattleTagNum;

#ifdef SDM_TAMER_XGUAGE_20180628
	cSprite*		m_pWindowBGXAnti;	
	cSprite*		m_pLevelBG_XAnti;	
#endif

	// 포인터만 있는 컨트롤
protected:
	cText*			m_pNameText;
	cText*			m_pLevelText;
	cProgressBar*	m_pHPBar;
	cProgressBar*	m_pDSBar;
	cProgressBar*	m_pExpBar;

	CsRect			m_rcExpRegion;

	cSprite*		m_pBattleSprite;

	cText*			m_pExpPercentText;	

protected:
	CsRect			m_rcBattleTag;
	float			m_fBattleAlpha;
	bool			m_bBattle;
	enum { BAT_INCREASE, BAT_DECREASE };
	int				m_nBattleAlphaType;

public:
	void			SetTargetObject( CsC_AvObject* pTarget );


	void			SetBattle(){ m_bBattle = true; }
	void			SetBattleEnd(){ m_bBattle = false; }

	bool			IsBattle(){ return m_bBattle; }
};