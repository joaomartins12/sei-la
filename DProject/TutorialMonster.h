#pragma once


class CTutorialMonster : public CMonster
{
	CsCRTTI_H

public:
	CTutorialMonster();
	~CTutorialMonster();

	virtual void		Init( uint nUID, uint nFileTableID, nSync::Pos sPos, float fRot, sCREATEINFO* pProp, eMONSTER_STATE MonsterState );
	virtual void		Update( float fDeltaTime, bool bAnimation = true );

	virtual void		AttachCamera_Critical( CsC_AvObject* pTarget );
	virtual void		SetBattle();
	virtual void		SetBattleEnd();

private:
	void				_UpdateAttack_ClientMode( float fDeltaTime );
	void				_SkillAttack( ushort const& nSkillIdx );
	bool				_CheckSight();
	void				_Ting();
	void				_MoveToAttackTargetPos();

private:
	float				m_fClientAttackTime;
	ushort				m_nAIType;
};