
#pragma once 

namespace FMTamer
{
	struct sTINFO
	{
		int		s_nTamerID;
		int		s_nLevel;
		int		s_nDigimonFriendShip;
	};

	int		_GetSkillApplyValue( sTINFO* pInfo, CsSkill::sINFO::sAPPLY* pApplyArray, nTable_Parameter eA, int nRate1, int nRate2 );
	int		_CalEquip( sTINFO* pInfo, nTable_Parameter eA );
	__int64		GetMaxExp( int nLevel );
	//==========================================================================================================
	//	±âº» ½ºÅÈ
	//==========================================================================================================
	int		GetBaseMaxHP( sTINFO* pInfo );
	int		GetBaseMaxDS( sTINFO* pInfo );
	int		GetBaseAtt( sTINFO* pInfo );
	int		GetBaseDef( sTINFO* pInfo );

	//==========================================================================================================
	//	Àåºñ ½ºÅÈ
	//==========================================================================================================
	int		GetEquipMaxHP( sTINFO* pInfo );
	int		GetEquipMaxDS( sTINFO* pInfo );
	int		GetEquipAtt( sTINFO* pInfo );
	int		GetEquipDef( sTINFO* pInfo );

	//==========================================================================================================
	//	½ºÅÈ ÃÑÇÕ
	//==========================================================================================================
	int		GetTotalMaxHP( sTINFO* pInfo );
	int		GetTotalMaxDS( sTINFO* pInfo );
	int		GetTotalAtt( sTINFO* pInfo );
	int		GetTotalDef( sTINFO* pInfo );

	int		GetTotalMaxHP_FriendShip( sTINFO* pInfo );
	int		GetTotalMaxDS_FriendShip( sTINFO* pInfo );
	int		GetTotalAtt_FriendShip( sTINFO* pInfo );
	int		GetTotalDef_FriendShip( sTINFO* pInfo );
}
