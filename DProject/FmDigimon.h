

#pragma once

namespace FMDigimon
{
	struct sDINFO
	{
		int		s_nDigimonID;
		int		s_nLevel;
		float	s_fGameOrgScale;		
	};

	int		_GetSkillApplyValue( sDINFO* pInfo, CsSkill::sINFO::sAPPLY* pApplyArray, nTable_Parameter eA );
	int		_CalChipset( sDINFO* pInfo, nTable_Parameter eA );
	__int64		GetMaxExp( int nLevel );
	__int64		GetMaxSkillExp( int nDigimonID, int nSkillExpLevel );
	//==========================================================================================================
	//	기본 스탯
	//==========================================================================================================
	int		GetBaseMaxHP( sDINFO* pInfo );
	int		GetBaseMaxDS( sDINFO* pInfo );
	int		GetBaseAtt( sDINFO* pInfo );
	int		GetBaseDef( sDINFO* pInfo );
	int		GetBaseEv( sDINFO* pInfo );	
	int		GetBaseCr( sDINFO* pInfo );
	int		GetBaseHt( sDINFO* pInfo );	

	//==========================================================================================================
	//	칩셋 스탯
	//==========================================================================================================
	int		GetChipsetMaxHP( sDINFO* pInfo );
	int		GetChipsetMaxDS( sDINFO* pInfo );
	int		GetChipsetAtt( sDINFO* pInfo );
	int		GetChipsetDef( sDINFO* pInfo );
	int		GetChipsetEv( sDINFO* pInfo );
	int		GetChipsetCr( sDINFO* pInfo );
	int		GetChipsetHt( sDINFO* pInfo );
	int		GetChipsetAs( sDINFO* pInfo );

	//==========================================================================================================
	//	에볼루션 스탯
	//==========================================================================================================
	float	_GetEvolRate( nsCsDigimonTable::eEVOLUTION_TYPE eEvol );

	int		GetEvolMaxHP( sDINFO* pInfo );
	int		GetEvolMaxDS( sDINFO* pInfo );
	int		GetEvolAtt( sDINFO* pInfo );
	int		GetEvolDef( sDINFO* pInfo );

	//==========================================================================================================
	//	기타
	//==========================================================================================================
	int		GetCostEvlove( DWORD dwSrcDigimonID, int nSrcLevel, DWORD dwDestDigimonID );
	float	GetAttackRange( DWORD nDigimonID, float fScale );
	float	GetAttackSpeed( DWORD nDigimonID );

	__int64	GetTamerEquipValue( sDINFO* pInfo, nTable_Parameter eA, int const& nPart );
	__int64	GetAccessoryEquipValue( nTable_Parameter eType, int nEquipType );
	//int		GetBuffDigimonStatValue( nTable_Parameter eType );
	__int64	GetBuffDigimonStatValue_N( nTable_Parameter eType, __int64 nOrgValue = 0 );
	__int64	GetDeBuffDigimonStatValue_N( nTable_Parameter eType, __int64 nOrgValue = 0 );
	__int64	GetSysBuffDigimonStatValue_N( nTable_Parameter eType, __int64 nOrgValue = 0 );
	//int		GetBuffTamerStatValue( nTable_Parameter eType );	
	

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


