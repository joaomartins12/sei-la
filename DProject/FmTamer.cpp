
#include "stdafx.h"
#include "FmTamer.h"

int FMTamer::_GetSkillApplyValue( sTINFO* pInfo, CsSkill::sINFO::sAPPLY* pApplyArray, nTable_Parameter eA, int nRate1, int nRate2 )
{
	int nReturnValue = 0;
	int nBase102 = 0;
	for( int ap=0; ap<SKILL_APPLY_MAX_COUNT; ++ap )
	{
		if( pApplyArray[ ap ].s_nID == 0 )
			continue;

		bool bCheck = true;
		if( pApplyArray[ ap ].s_nA == APPLY_AllParam )
		{			
			switch( eA )
			{
			case APPLY_MAXHP:
			case APPLY_MAXDS:
			case APPLY_AP:
			case APPLY_DP:
				bCheck = true;
				break;
			}
		}

		if( bCheck && ( pApplyArray[ ap ].s_nA != eA ) )
			continue;

		switch( pApplyArray[ ap ].s_nID )
		{
		case nSkill::Me_101:
			nReturnValue +=pApplyArray[ ap ].s_nB;
			break;
		case nSkill::Me_102:
			{
				// A = A + (A*B)%100
				if( nBase102 == 0 )
				{
					switch( eA )
					{
					case APPLY_MAXHP:
						nBase102 = GetBaseMaxHP( pInfo );
						break;
					case APPLY_MAXDS:
						nBase102 = GetBaseMaxDS( pInfo );
						break;
					case APPLY_AP:
						nBase102 = GetBaseAtt( pInfo );
						break;
					case APPLY_DP:
						nBase102 = GetBaseDef( pInfo );
						break;
					default:
						assert_cs( false );
					}
				}
				nReturnValue += CsFloat2Int( nBase102*pApplyArray[ ap ].s_nB*0.01f );
			}
			break;
		default:
			assert_cs( false );
		}
	}

	assert_cs( nRate1 > 0 );
	if( nRate2 == -1 )
		return CsFloat2Int( nReturnValue*nRate1*0.01f );

	return CsFloat2Int( CsFloat2Int( nReturnValue*nRate2*0.01f ) * nRate1 * 0.01f );
}

int FMTamer::_CalEquip( sTINFO* pInfo, nTable_Parameter eA )
{
	int nReturnValue = 0;
	int nBaseMaxHP = 0;

	DWORD dwSkillFTID;
	cItemInfo* pItemInfo;
	CsSkill::sINFO* pSkillFT;
	CsItem* pFTItem;

	for( int i=0; i<nLimit::Equip; ++i )
	{
		pItemInfo = g_pDataMng->GetTEquip()->GetData( i );
		if( pItemInfo->IsEnable() == false )
			continue;

		pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() );
		if( pFTItem->GetInfo()->IsAccessory() )
			continue;
//#ifdef VERSION_USA	// 장비 기간 만료 시 테이머 스탯에서 제외
		if( pFTItem->GetInfo()->s_nUseTime_Min != 0 )
		{
			if( ( pItemInfo->m_nEndTime == ITEMINFO_INVALIDE_TIME )||( pItemInfo->m_nEndTime <= _TIME_TS ) )
				continue;
		}

// 		: 아이템 사용대상 구분
		switch( pFTItem->GetInfo()->s_nUseCharacter )
		{
		case 0:continue;	//해당사항없음
		case 1:break;		//테이머 & 디지몬 동시 사용
		case 2:continue;	//디지몬 전용
		case 3:break;		//테이머 전용
		}

		dwSkillFTID = pFTItem->GetInfo()->s_dwSkill;
		if( dwSkillFTID != 0 )
		{
			pSkillFT = nsCsFileTable::g_pSkillMng->GetSkill( dwSkillFTID )->GetInfo();
			assert_cs( pSkillFT->s_nFamilyType == 0 );
			if( pItemInfo->GetSkillRate() )
			{
				nReturnValue += _GetSkillApplyValue( pInfo, pSkillFT->s_Apply, eA, pItemInfo->GetSkillRate(), -1 );
			}
		}
//#endif
// 		// 국내는 기간 만료 시 기본 능력치는 적용 안되고 소켓 능력치만 적용
// 		bool bTimeCheck = true;
// 		if( pFTItem->GetInfo()->s_nUseTime_Min != 0 )
// 		{
// 			if( ( pItemInfo->m_nEndTime == ITEMINFO_INVALIDE_TIME )||( pItemInfo->m_nEndTime <= _TIME_TS ) )
// 			{
// 				bTimeCheck = false;
// 			}
// 		}
//
// 		if( bTimeCheck )
// 		{
// 			dwSkillFTID = pFTItem->GetInfo()->s_dwSkill;
// 			if( dwSkillFTID != 0 )
// 			{
// 				pSkillFT = nsCsFileTable::g_pSkillMng->GetSkill( dwSkillFTID )->GetInfo();
// 				assert_cs( pSkillFT->s_nFamilyType == 0 );
// 				if( pItemInfo->GetSkillRate() )
// 				{
// 					nReturnValue += _GetSkillApplyValue( pInfo, pSkillFT->s_Apply, eA, pItemInfo->GetSkillRate(), -1 );
// 				}
// 			}
// 		}

		// 소켓 아이템
		for( int s=0; s<nLimit::SocketSlot; ++s )
		{
			if( pItemInfo->m_nSockItemType[ s ] == 0 )
				continue;

			assert_cs( pItemInfo->m_nSockAppRate[ s ] > 0 );
			pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->m_nSockItemType[ s ] );
			dwSkillFTID = pFTItem->GetInfo()->s_dwSkill;
			if( dwSkillFTID != 0 )
			{
				pSkillFT = nsCsFileTable::g_pSkillMng->GetSkill( dwSkillFTID )->GetInfo();
				assert_cs( pSkillFT->s_nFamilyType == 0 );
				if( pItemInfo->GetSocketRate() )
				{
					nReturnValue += _GetSkillApplyValue( pInfo, pSkillFT->s_Apply, eA, pItemInfo->GetSocketRate(), pItemInfo->m_nSockAppRate[ s ] );
				}				
			}
		}

	}
	return nReturnValue;
}
__int64 FMTamer::GetMaxExp( int nLevel )
{
	return CsFloat2Int64( nsCsFileTable::g_pBaseMng->GetTamerBase( nLevel, CsBaseMng::BT_TAMER_EXP )->GetInfo()->s_dwExp*0.01f );
	//return nsCsFileTable::g_pBaseMng->GetTamerBase( nLevel, CsBaseMng::BT_TAMER_EXP )->GetInfo()->s_dwExp;
}

//==========================================================================================================
//
//	기본 스탯
//
//==========================================================================================================
int FMTamer::GetBaseMaxHP( sTINFO* pInfo )
{
	int nTamerType = nsCsFileTable::g_pTamerMng->GetTamer( pInfo->s_nTamerID )->GetInfo()->s_nTamerType;
	return nsCsFileTable::g_pBaseMng->GetTamerBase( pInfo->s_nLevel, nTamerType )->GetInfo()->s_nHP;
}

int FMTamer::GetBaseMaxDS( sTINFO* pInfo )
{
	int nTamerType = nsCsFileTable::g_pTamerMng->GetTamer( pInfo->s_nTamerID )->GetInfo()->s_nTamerType;
	return nsCsFileTable::g_pBaseMng->GetTamerBase( pInfo->s_nLevel, nTamerType )->GetInfo()->s_nDS;
}

int FMTamer::GetBaseAtt( sTINFO* pInfo )
{
	int nTamerType = nsCsFileTable::g_pTamerMng->GetTamer( pInfo->s_nTamerID )->GetInfo()->s_nTamerType;
	return nsCsFileTable::g_pBaseMng->GetTamerBase( pInfo->s_nLevel, nTamerType )->GetInfo()->s_nAttack;
}

int FMTamer::GetBaseDef( sTINFO* pInfo )
{
	int nTamerType = nsCsFileTable::g_pTamerMng->GetTamer( pInfo->s_nTamerID )->GetInfo()->s_nTamerType;
	return nsCsFileTable::g_pBaseMng->GetTamerBase( pInfo->s_nLevel, nTamerType )->GetInfo()->s_nDefence;
}


//==========================================================================================================
//
//	장비 스탯
//
//==========================================================================================================
int FMTamer::GetEquipMaxHP( sTINFO* pInfo )
{
	return _CalEquip( pInfo, APPLY_MAXHP );
}

int FMTamer::GetEquipMaxDS( sTINFO* pInfo )
{
	return _CalEquip( pInfo, APPLY_MAXDS );
}

int FMTamer::GetEquipAtt( sTINFO* pInfo )
{
	return _CalEquip( pInfo, APPLY_AP );
}

int FMTamer::GetEquipDef( sTINFO* pInfo )
{
	return _CalEquip( pInfo, APPLY_DP );
}


//==========================================================================================================
//
//	스탯 총합
//
//==========================================================================================================

int FMTamer::GetTotalMaxHP( sTINFO* pInfo )
{
	return GetBaseMaxHP( pInfo ) + GetEquipMaxHP( pInfo );
}

int FMTamer::GetTotalMaxDS( sTINFO* pInfo )
{
	return GetBaseMaxDS( pInfo ) + GetEquipMaxDS( pInfo );
}

int FMTamer::GetTotalAtt( sTINFO* pInfo )
{
	return GetBaseAtt( pInfo ) + GetEquipAtt( pInfo );
}

int FMTamer::GetTotalDef( sTINFO* pInfo )
{
	return GetBaseDef( pInfo ) + GetEquipDef( pInfo );
}


int FMTamer::GetTotalMaxHP_FriendShip( sTINFO* pInfo )
{
	return CsFloat2Int( (GetBaseMaxHP( pInfo ) + GetEquipMaxHP( pInfo ))*pInfo->s_nDigimonFriendShip*0.01f );
}

int FMTamer::GetTotalMaxDS_FriendShip( sTINFO* pInfo )
{
	return CsFloat2Int( (GetBaseMaxDS( pInfo ) + GetEquipMaxDS( pInfo ))*pInfo->s_nDigimonFriendShip*0.01f );
}

int FMTamer::GetTotalAtt_FriendShip( sTINFO* pInfo )
{
	return CsFloat2Int( (GetBaseAtt( pInfo ) + GetEquipAtt( pInfo ))*pInfo->s_nDigimonFriendShip*0.01f );
}

int FMTamer::GetTotalDef_FriendShip( sTINFO* pInfo )
{
	return CsFloat2Int( (GetBaseDef( pInfo ) + GetEquipDef( pInfo ))*pInfo->s_nDigimonFriendShip*0.01f );
}

