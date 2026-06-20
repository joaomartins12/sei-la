#pragma once

class cSkillForster : public cBaseWindow
{
public:
	cSkillForster(){}

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_SKILL_FORSTER; }
	virtual void			Destroy();
	virtual void			DeleteResource();	

	virtual void			ResetMap() { Close( false ); }
	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );

	virtual void			Update(float const& fDeltaTime);	
	virtual eMU_TYPE		Update_ForMouse();	

	virtual void			Render();		
	virtual	void			OnMoveWindow();

	void					SetTarget( CsC_AvObject* pTarget ) { m_pTarget = pTarget; }

protected:
	void					_ResetSkillList();

	void					_SetActiveSkillList();
	void					_DeleteActiveSkill();
	void					_SetPassiveSkillList();
	void					_DeletePassiveSkill();
	void					_AddSkill( CsSkill* pSkill, int nIdx );

	void					_SkillDisc( DWORD dwID );

protected:	
	cSprite*				m_pMouseOnMask;	
	cImage*					m_pExpandImage;
	cMoneyIF*				m_pMoney;

	bool					m_bExpandActive;
	int						m_nMaxActiveSkill;
	bool					m_bExpandPassive;
	int						m_nMaxPassiveSkill;

	cStringList				m_ListSkill;
	cStringList				m_ListSkillDisc;

	//cScript »ç¿ë
protected:
	cButton*				m_pCancelButton;	//´Ý±â	
	cScrollBar*				m_pScrollBar;

	cButton*				m_pLearn;
	cButton*				m_pClosed;

protected:
	int						m_nMouseOnMaskIndex;
	bool					m_bSelect;

	int						m_nSelectSkill;
	CsC_AvObject*			m_pTarget;

};