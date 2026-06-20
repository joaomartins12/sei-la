
#pragma once 

class cBattleEvol : public cBaseWindow
{
public:
	cBattleEvol();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_BATTLE_EVOL; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();
	virtual void			ResetMap(){}

public:
	void					ResetTarget();
	void					ResetEPBar( int nCurEP );

protected:
	void					_DeleteCharImageList();	

protected:
	CDigimonEvolution::sBATTLE_EVO_INFO*	m_pFT_BattleEvo;
	cProgressBar*				m_pBar;
	cProgressBar*				m_pBarCharge;
	cSprite*					m_pDeviceBar;
	cSpriteAni*					m_pAniBG;
	cSpriteAni*					m_pAniInside;

	cButton*					m_pBtnMask;

	int				m_nDeviceBarCnt;
	CsPoint			m_ptDeviceBarPosArray[ FT_BATTLE_EVO_ELEMENT_CNT ];

	CsVectorPB< sCHAR_IMAGE* >	m_vpCharImage;
	int							m_nRenderIndex;
	CsTimeSeq					m_RenderTimeSeq;

	CsPoint				m_ptRangeEP;
	bool				m_bMaxEP;
};