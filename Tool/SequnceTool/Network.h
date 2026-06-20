

#pragma once 

namespace nsNETWORK
{
	void	SetAttackInfo(	CsC_AvObject* pHitter, 
							CsC_AvObject* pTarget, 
							DWORD dwAniID,
							CsC_AttackProp::eHIT_TYPE ht,
							CsC_AttackProp::eDAMAGE_TYPE dt );

	void	SetSkillInfo(	CsC_AvObject* pHitter, 
							CsC_AvObject* pTarget,
							int nSkillType				);
}