
#pragma once 

class cDigiviceQuest : public cBaseWindow
{
public:
	cDigiviceQuest();

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_DIGIVICEQUEST; }
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetDevice();

	void					RestartSpriteAni();
	bool					OpenDigiviceTalk();

private:
	bool					_IhaveCompleteQuest();

	// 메모리잡힌 컨트롤
private:
	cButton*		m_pQuestBtn;
	cSpriteAni*		m_pAniSprite;

	enum eSPRITE_STATE{ SS_CREATE, SS_MOVING, SS_STOP, };
	eSPRITE_STATE	m_eSpriteState;
	float			m_fSpriteTime;
	CsPoint			m_ptSpritePos;	
};

