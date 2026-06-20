

#pragma once 

class cTacticsWindow : public cBaseWindow
{
public:
	cTacticsWindow();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TACTICS_WINDOW; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			PlayOpenSound(){}
	virtual void			PlayCloseSound(){}

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnRButtonUp( CsPoint pos );

	// 필요 변수
protected:
	int				m_nTargetDigimonFTID;
	int				m_nTacticsIndex;
	bool			m_bClickMouse;
	bool			m_bDisable;

	// 메모리잡힌 컨트롤
protected:
	cSprite*		m_pTargetImgae;
	cSprite*		m_pBattleMask;
#ifdef SDM_TAMER_XGUAGE_20180628 
	cSprite*		m_pkBG;
	bool			m_bisXAnti;
#endif

	cSprite*		m_pTactics_Lock;

	// 포인터만 있는 컨트롤
protected:

public:
	void			SetTargetObject( int nDigimonFTID );
	cSprite*		GetTargetImage() { return m_pTargetImgae; }
	int				GetTargetDigimonFTID()	{return m_nTargetDigimonFTID;}
	void			SetDisable();
	bool			IsDisable() {return m_bDisable;}
};