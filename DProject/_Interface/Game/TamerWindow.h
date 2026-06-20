
#pragma once 

class AdaptPartySystem;

class cTamerWindow : public cBaseWindow
{
public:
	cTamerWindow();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_TAMER_WINDOW; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			PlayOpenSound(){}
	virtual void			PlayCloseSound(){}

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();	
	virtual void			Render();

	virtual void			OnRButtonUp( CsPoint pos );
	virtual	void			OnLButtonUp( CsPoint pos );

	virtual void			ResetDevice();
	void					SetPartyMaster(bool bValue);

	// Vip Mebmer Ship 일때 테이머 배경 이미지 변경
	void					ChangeVipMemberShip( bool bValue );

protected:
	CsC_AvObject*	m_pTargetObject;
	
	// 메모리잡힌 컨트롤
protected:
	cSprite*		m_pWindowBG;
	cSprite*		m_pTamerLvBG;
	cProgressBar*	m_pHPBarMin;
	cProgressBar*	m_pHPBarMax;
	cProgressBar*	m_pFPBar;	
	cSprite*		m_pHostSprite;

	// 포인터만 있는 컨트롤
protected:
	cText*			m_pNameText;
	cText*			m_pLevelText;	
	cProgressBar*	m_pDSBar;
	cProgressBar*	m_pExpBar;
	cText*			m_pExpPercentText;
	cText*			m_pHPText;

protected:
	CsRect			m_rcExpRegion;

public:	
	void			SetTargetObject( CsC_AvObject* pTarget );	

private: 
	TOwnershipGuard<AdaptPartySystem> m_pAdaptParty; 
};