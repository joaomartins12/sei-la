

#pragma once

namespace FMDigimon
{
	int		GetMaxExp( int nLevel );
	int		GetMaxSkillExp( int nDigimonID, int nSkillExpLevel );

	//==========================================================================================================
	//	기본 스탯
	//==========================================================================================================
	int		GetBaseMaxHP( int nDigimonID, int nLevel );
	int		GetBaseMaxDS( int nDigimonID, int nLevel );
	int		GetBaseAtt( int nDigimonID, int nLevel );
	int		GetBaseDef( int nDigimonID, int nLevel );
	int		GetBaseEv( int nDigimonID, int nLevel );	
	int		GetBaseCr( int nDigimonID, int nLevel );

	

	//==========================================================================================================
	//	에볼루션 스탯
	//==========================================================================================================
	float	_GetEvolRate( nsCsDigimonTable::eEVOLUTION_TYPE eEvol );

	int		GetEvolMaxHP( int nDigimonID, int nLevel );
	int		GetEvolMaxDS( int nDigimonID, int nLevel );
	int		GetEvolAtt( int nDigimonID, int nLevel );
	int		GetEvolDef( int nDigimonID, int nLevel );

	//==========================================================================================================
	//	기타
	//==========================================================================================================
	int		GetCostEvlove( DWORD dwSrcDigimonID, int nSrcLevel, DWORD dwDestDigimonID );
	float	GetAttackRange( DWORD nDigimonID, float fScale );
	float	GetAttackSpeed( DWORD nDigimonID );
	int		GetSkillAtt( DWORD nSkillID, int nSkillLevel );

	/*int		GetMaxExp( cAttObject* pChar );
	int		GetMaxHP( cAttObject* pChar, DWORD dwDestModel );
	int		GetMaxDS( cAttObject* pChar, DWORD dwDestModel );
	int		GetDefence( cAttObject* pChar, DWORD dwDestModel );
	int		GetEvasion( cAttObject* pChar, DWORD dwDestModel );	
	int		GetCritical( cAttObject* pChar, DWORD dwDestModel );
	int		GetAttackPower( cAttObject* pChar, DWORD dwDestModel );

	int		GetMoveSpeed( DWORD dwDestModel );
	float	GetAttackSpeed( DWORD dwDestModel );
	float	GetAttackRange( DWORD dwDestModel, float fScale );
	
	

	int		GetSkillExp( cDigimonUser* pChar, UINT dwEvoID );
	int		GetSkillAttackPower( cDigimonUser* pChar, UINT dwEvoID, UINT dwSkillCode, int nSkillLevel  );
	int		GetNextSkillAttackPower( cDigimonUser* pChar, UINT dwEvoID, UINT dwSkillCode, int nSkillLevel  );	*/
}


