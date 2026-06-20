
#include "stdafx.h"
#include "FmDigimon.h"

int FMDigimon::_GetSkillApplyValue( sDINFO* pInfo, CsSkill::sINFO::sAPPLY* pApplyArray, nTable_Parameter eA )
{
	int nReturnValue = 0;
	int nBase102 = 0;
	switch( eA )
	{
	case APPLY_MAXHP:		nBase102 = GetBaseMaxHP( pInfo );			break;
	case APPLY_MAXDS:		nBase102 = GetBaseMaxDS( pInfo );			break;
	case APPLY_AP:			nBase102 = GetBaseAtt( pInfo );				break;
	case APPLY_DP:			nBase102 = GetBaseDef( pInfo );				break;
	case APPLY_EV:			nBase102 = GetBaseEv( pInfo );				break;
	case APPLY_CA:			nBase102 = GetBaseCr( pInfo );				break;
	case APPLY_AS:			nBase102 = GetAttackSpeed( pInfo->s_nDigimonID ) * 1000; break;
// 	default:
// 		return 0;
	}

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
				bCheck = false;
				break;
			}
		}

		if( bCheck && ( pApplyArray[ ap ].s_nA != eA ) )
			continue;

		switch( pApplyArray[ ap ].s_nID )
		{
		case nSkill::Me_101:	nReturnValue += pApplyArray[ ap ].s_nB;		break;
		case nSkill::Me_102:	nReturnValue += CsFloat2Int( nBase102*pApplyArray[ ap ].s_nB*0.01f );		break;
		case nSkill::Me_403:	nReturnValue += (pApplyArray[ ap ].s_nB * 100);		break;
		case nSkill::Me_404:	nReturnValue += (pApplyArray[ ap ].s_nB * 100);		break;
		case nSkill::Me_105:	nReturnValue += (int)(nBase102 * (pApplyArray[ ap ].s_nB * 0.01f));	break;
		case nSkill::Me_103:
			assert_cs( false );
			break;
		case nSkill::Me_104:
			assert_cs( false );
			break;
		default:
			assert_cs( false );
		}
		if( eA == APPLY_AS )
			nReturnValue *= -1;
	}
	return nReturnValue;
}

__int64 FMDigimon::GetTamerEquipValue( sDINFO* pInfo, nTable_Parameter eA, int const& part )
{
	__int64 nReturnValue = 0;

	cData_TEquip* pTEquip = g_pDataMng->GetTEquip();
	SAFE_POINTER_RETVAL( pTEquip, nReturnValue );

	cItemInfo* pItemInfo;
	if( nTamer::MaxParts == part )
		pItemInfo = g_pDataMng->GetTEquip()->GetDigiviceItem();
	else
		pItemInfo = g_pDataMng->GetTEquip()->GetData( part );

	SAFE_POINTER_RETVAL( pItemInfo, nReturnValue );
	if( !pItemInfo->IsEnable() )
		return nReturnValue;

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() );
	SAFE_POINTER_RETVAL( pFTItem, nReturnValue );

	CsItem::sINFO* pEItemInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL( pEItemInfo, nReturnValue );

	//아이템 사용대상 구분
	switch( pEItemInfo->s_nUseCharacter )
	{
	case 1:break;		//테이머 & 디지몬 동시 사용
	case 2:break;		//디지몬 전용
	case 3:				//테이머 전용
		{
			if( nTamer::MaxParts != part )
				return nReturnValue;
		}
		break;
	case 0:				//해당사항없음
	default:
		return nReturnValue;
	}

	if( pEItemInfo->s_dwSkill != 0 )
	{
		CsSkill::sINFO* pSkillFT = nsCsFileTable::g_pSkillMng->GetSkill( pEItemInfo->s_dwSkill )->GetInfo();
		if(pSkillFT)
		{
			assert_cs( pSkillFT->s_nFamilyType == 0 );
			if( pItemInfo->GetSkillRate() )
				nReturnValue +=	(__int64)( _GetSkillApplyValue( pInfo, pSkillFT->s_Apply, eA )*pItemInfo->m_nRate*0.01f );			
		}

	}
	
	// 소켓 아이템
	for( int s=0; s<nLimit::SocketSlot; ++s )
	{
		if( pItemInfo->m_nSockItemType[ s ] == 0 )
			continue;

		assert_cs( pItemInfo->m_nSockAppRate[ s ] > 0 );

		CsItem* pSockFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->m_nSockItemType[ s ] );
		SAFE_POINTER_CON( pSockFTItem );

		CsItem::sINFO* pSockItemInfo = pFTItem->GetInfo();
		SAFE_POINTER_CON( pSockItemInfo );

		if( pSockItemInfo->s_dwSkill != 0 )
		{
			CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pSockItemInfo->s_dwSkill );
			SAFE_POINTER_CON( pSkill );

			CsSkill::sINFO* pSkillFT = pSkill->GetInfo();
			SAFE_POINTER_CON( pSkillFT );

			assert_cs( pSkillFT->s_nFamilyType == 0 );
			if( pItemInfo->GetSocketRate() )
				nReturnValue += (__int64)(_GetSkillApplyValue( pInfo, pSkillFT->s_Apply, eA )*pItemInfo->m_nRate*0.01f );
		}
	}

	return nReturnValue;
}

int FMDigimon::_CalChipset( sDINFO* pInfo, nTable_Parameter eA )
{
	int nReturnValue = 0;
	SAFE_POINTER_RETVAL( pInfo, nReturnValue );

	cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
	SAFE_POINTER_RETVAL( pDigivice, nReturnValue );

	CTamerUser* pTUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTUser, nReturnValue );

	int nTamerLv = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();

	int nFamily[ DIGIMON_MAX_FAMILY ];
	for( int i=0; i<DIGIMON_MAX_FAMILY; ++i )
		nFamily[ i ] = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo()->s_eFamilyType[ i ];

	for( int i=0; i<nLimit::Chipset; ++i )
	{
		cItemInfo* pChipsetInfo = pDigivice->GetChipset( i );
		SAFE_POINTER_CON( pChipsetInfo );

		if( !pChipsetInfo->IsEnable() )
			continue;

		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pChipsetInfo->GetType() );
		SAFE_POINTER_CON( pFTItem );

		CsItem::sINFO* pItemInfo = pFTItem->GetInfo();
		SAFE_POINTER_CON( pItemInfo );

		if( !pItemInfo->IsEnableDigimonLv( pInfo->s_nLevel ) )
			continue;

		if( !pItemInfo->IsEnableTamerLv( nTamerLv ) )
			continue;

		DWORD dwSkillFTID = pItemInfo->s_dwSkill;
		if( dwSkillFTID == 0 )
			continue;

		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( dwSkillFTID );
		SAFE_POINTER_CON( pSkill );

		CsSkill::sINFO* pSkillFT = pSkill->GetInfo();
		SAFE_POINTER_CON( pSkillFT );

		assert_cs( pSkillFT->s_nFamilyType != 0 );

		bool bSameFamily = false;
		for( int i=0; i<DIGIMON_MAX_FAMILY; ++i )
		{
			if( ( nFamily[ i ] == pSkillFT->s_nFamilyType )||( pSkillFT->s_nFamilyType == 17 ) )
			{
				bSameFamily = true;
				break;
			}
		}
		if( bSameFamily == false )
			continue;

#ifndef _GIVE
		for( int ap=0; ap<SKILL_APPLY_MAX_COUNT; ++ap )
			assert_cs( pSkillFT->s_Apply[ ap ].s_nID != 103 );
#endif
		nReturnValue += CsFloat2Int( _GetSkillApplyValue( pInfo, pSkillFT->s_Apply, eA )*pChipsetInfo->m_nRate*0.01f );
	}
	return nReturnValue;
}

__int64 FMDigimon::GetMaxExp( int nLevel )
{
#ifndef _GIVE
	if( net::game == NULL )
		return 0;
#endif
	return CsFloat2Int64( nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, CsBaseMng::BT_DIGIMON_EXP )->GetInfo()->s_dwExp*0.01f );	
}

__int64 FMDigimon::GetMaxSkillExp( int nDigimonID, int nSkillExpLevel )
{
#ifdef SDM_DIGIMON_SHOW_SKILLEXP_TABLE_20190212
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pDigimonMng,	0 );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng,	0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon,	0 );
	CsDigimon::sINFO* pInfo = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pInfo,	0 );

	DWORD dwMaxExp = nsCsFileTable::g_pBaseMng->GetDigimonSkillMaxExp( pInfo->s_eEvolutionType, nSkillExpLevel );
	return CsFloat2Int64( dwMaxExp * 0.01f);
#else
	int nLevel = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo()->s_dwBaseLevel + nSkillExpLevel;
 	return CsFloat2Int64( nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, CsBaseMng::BT_DIGIMON_EXP )->GetInfo()->s_dwExp*0.01f );	
#endif
}

//==========================================================================================================
//
//	기본 스탯
//
//==========================================================================================================

int FMDigimon::GetBaseMaxHP( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return pBaseInfo->s_nHP + (int)( pDigimonFT->s_nBaseStat_HP*pInfo->s_fGameOrgScale );

//	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
//	return nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nHP + CsFloat2Int( pDigimonFT->s_nBaseStat_HP*pInfo->s_fGameOrgScale );
}

int FMDigimon::GetBaseMaxDS( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return pBaseInfo->s_nDS + pDigimonFT->s_nBaseStat_DS;

//	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
//	return nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nDS + pDigimonFT->s_nBaseStat_DS;
}

int FMDigimon::GetBaseDef( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return pBaseInfo->s_nDefence + (int)( pDigimonFT->s_nBaseStat_DefPower*pInfo->s_fGameOrgScale );

	//CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
	//return nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nDefence + CsFloat2Int( pDigimonFT->s_nBaseStat_DefPower*pInfo->s_fGameOrgScale );
}

int FMDigimon::GetBaseEv( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return pBaseInfo->s_nEvasion + pDigimonFT->s_nBaseStat_Evasion;
// 	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
// 	int nValue = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nEvasion + pDigimonFT->s_nBaseStat_Evasion;
// 	return nValue;
}

int FMDigimon::GetBaseCr( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return pBaseInfo->s_nCritical + (int)(pDigimonFT->s_nBaseStat_CriticalRate*pInfo->s_fGameOrgScale);
// 	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
// 	int nValue = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nCritical + CsFloat2Int( pDigimonFT->s_nBaseStat_CriticalRate*pInfo->s_fGameOrgScale );
// 	return nValue;
}

int FMDigimon::GetBaseAtt( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return pBaseInfo->s_nAttack + (int)(pDigimonFT->s_nBaseStat_AttPower*pInfo->s_fGameOrgScale);

//	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();	
//	return nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nAttack + CsFloat2Int( pDigimonFT->s_nBaseStat_AttPower*pInfo->s_fGameOrgScale );
}

int FMDigimon::GetBaseHt( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return pBaseInfo->s_nHitRate + pDigimonFT->s_nBaseStat_HitRate;

// 	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
// 	int nValue = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nHitRate + pDigimonFT->s_nBaseStat_HitRate;
// 	return nValue;
}

//==========================================================================================================
//
//	에볼루션 스탯
//
//==========================================================================================================

float FMDigimon::_GetEvolRate( nsCsDigimonTable::eEVOLUTION_TYPE eEvol )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pBaseMng, 0.0f );
	int nValue = nsCsFileTable::g_pBaseMng->GetEvoApplyValue( (int)eEvol );
	return (nValue - 100) * 0.01f;
}

int FMDigimon::GetEvolMaxHP( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return (int)(pBaseInfo->s_nHP * _GetEvolRate( pDigimonFT->s_eEvolutionType ));

	//CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
	//return CsFloat2Int( nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nHP * _GetEvolRate( pDigimonFT->s_eEvolutionType ) );
}

int FMDigimon::GetEvolMaxDS( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return (int)(pBaseInfo->s_nDS * _GetEvolRate( pDigimonFT->s_eEvolutionType ));
//	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
//	return CsFloat2Int( nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nDS * _GetEvolRate( pDigimonFT->s_eEvolutionType ) );
}

int FMDigimon::GetEvolDef( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return (int)(pBaseInfo->s_nDefence * _GetEvolRate( pDigimonFT->s_eEvolutionType ));
//	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
//	return CsFloat2Int( nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nDefence * _GetEvolRate( pDigimonFT->s_eEvolutionType ) );
}

int FMDigimon::GetEvolAtt( sDINFO* pInfo )
{
	SAFE_POINTER_RETVAL( pInfo, 0 );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID );
	SAFE_POINTER_RETVAL( pDigimon, 0 );
	CsDigimon::sINFO* pDigimonFT = pDigimon->GetInfo();
	SAFE_POINTER_RETVAL( pDigimonFT, 0 );

	CsBase* pBase = nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType );
	SAFE_POINTER_RETVAL( pBase, 0 );

	CsBase::sINFO* pBaseInfo = pBase->GetInfo();
	SAFE_POINTER_RETVAL( pBaseInfo, 0 );

	return (int)(pBaseInfo->s_nAttack * _GetEvolRate( pDigimonFT->s_eEvolutionType ));
//	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_nDigimonID )->GetInfo();
//	return CsFloat2Int( nsCsFileTable::g_pBaseMng->GetDigimonBase( pInfo->s_nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nAttack * _GetEvolRate( pDigimonFT->s_eEvolutionType ) );
}

//==========================================================================================================
//
//	칩셋 스탯
//
//==========================================================================================================

int FMDigimon::GetChipsetMaxHP( sDINFO* pInfo )
{
	return _CalChipset( pInfo, APPLY_MAXHP );
}

int FMDigimon::GetChipsetMaxDS( sDINFO* pInfo )
{
	return _CalChipset( pInfo, APPLY_MAXDS );
}

int FMDigimon::GetChipsetAtt( sDINFO* pInfo )
{
	return _CalChipset( pInfo, APPLY_AP );
}

int FMDigimon::GetChipsetDef( sDINFO* pInfo )
{
	return _CalChipset( pInfo, APPLY_DP );
}

int FMDigimon::GetChipsetEv( sDINFO* pInfo )
{
	return _CalChipset( pInfo, APPLY_EV );
}

int FMDigimon::GetChipsetCr( sDINFO* pInfo )
{
	return _CalChipset( pInfo, APPLY_CA );
}

int FMDigimon::GetChipsetHt( sDINFO* pInfo )
{
	return _CalChipset( pInfo, APPLY_HT );
}

int FMDigimon::GetChipsetAs( sDINFO* pInfo )
{
	return _CalChipset( pInfo, APPLY_AS );
}

//==========================================================================================================
//
//	기타
//
//==========================================================================================================
int FMDigimon::GetCostEvlove( DWORD dwSrcDigimonID, int nSrcLevel, DWORD dwDestDigimonID )
{
	CsDigimon::sINFO* pSrcFTD = nsCsFileTable::g_pDigimonMng->GetDigimon( dwSrcDigimonID )->GetInfo();
	SAFE_POINTER_RETVAL( pSrcFTD, 0 );
	CsDigimon::sINFO* pDestFTD = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDestDigimonID )->GetInfo();
	SAFE_POINTER_RETVAL( pDestFTD, 0 );
	
	int nNextStep = pDestFTD->s_eEvolutionType;
	int nCurrStep = pSrcFTD->s_eEvolutionType;

	// 캡슐진화체 진화 DS 공식
	if( pDestFTD->s_eEvolutionType == nsCsDigimonTable::ET_CAPSULE )
	{
		return
		( 
		( ( nsCsDigimonTable::ET_ULTIMATE*nsCsDigimonTable::ET_ULTIMATE ) -
		( nCurrStep*nCurrStep ) - 5 )*
		( ( pDestFTD->s_dwBaseLevel - nSrcLevel + 110 )/5 )
		) + 20;
	}

#ifdef SDM_TAMER_XGUAGE_20180628
	if (nsCsDigimonTable::ET_X_ROOKIE <= pDestFTD->s_eEvolutionType && pDestFTD->s_eEvolutionType <= nsCsDigimonTable::ET_X_JOGRESS)
	{
		int nStep = abs(nNextStep - nCurrStep);
		int nLv   = (30 + pDestFTD->s_dwBaseLevel) / 5;
		int nConsumeXG = 20 + nStep * nLv;
		return nConsumeXG;
	}
#endif

	if( pDestFTD->s_eEvolutionType == nsCsDigimonTable::ET_JOINTPROGRESSMODE )
	{
		int nStep = abs( nNextStep*nNextStep - nCurrStep*nCurrStep ) - 5;
		return ( nStep * ( nSrcLevel / 5 )	) + 20;
	}
	// 조그레스에서 스피릿 진화체 퇴화시 예외 처리.. 
	// 추후 수정 필요.. 꼭...
	else if( nNextStep == nsCsDigimonTable::ET_SPIRIT && nCurrStep == nsCsDigimonTable::ET_JOINTPROGRESSMODE )
	{
		return 0;
	}
#ifdef SDM_TAMER_XGUAGE_20180628
	else if( nNextStep == nsCsDigimonTable::ET_MIX )
		return 0;
#endif
	else
	{
		int nStep = ( nNextStep*nNextStep - nCurrStep*nCurrStep ) - 5;
		return	( nStep * ( ( pDestFTD->s_dwBaseLevel - nSrcLevel + 110 )/5 ) ) + 20;
	}
}

float FMDigimon::GetAttackRange( DWORD nDigimonID, float fScale )
{
	CsDigimon* pFTD = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID );
	SAFE_POINTER_RETVAL( pFTD, 0.0f );

	CsDigimon::sINFO* pInfo = pFTD->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, 0.0f );

	// 근접
	if( pInfo->s_nDigimonType == 1 )
		return (float)( pInfo->s_nBaseStat_AttRange )*fScale;

	// 원거리
	return (float)( pInfo->s_nBaseStat_AttRange );
}


float FMDigimon::GetAttackSpeed( DWORD nDigimonID )
{
	CsDigimon* pFTD = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID );
	SAFE_POINTER_RETVAL( pFTD, 0.0f );
	CsDigimon::sINFO* pInfo = pFTD->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, 0.0f );

	float fRes = ( pInfo->s_nBaseStat_AttSpeed )*0.001f;

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		fRes *= (1.0f/BATTLE_SPEED_NORMAL);

	return fRes;
}

__int64 FMDigimon::GetAccessoryEquipValue( nTable_Parameter eType, int nEquipType )
{
	__int64 nValue = 0;

	int nOpt = 0;
	switch( eType ) //현재 툴팁을 보여줘야 하는 애가 뭐냐
	{
	case APPLY_MAXHP:		nOpt = nItem::MAXHP;	break;
	case APPLY_MAXDS:		nOpt = nItem::MAXDS;	break;
	case APPLY_AP:			nOpt = nItem::AP;		break;
	case APPLY_AS:			nOpt = nItem::AS;		break;
	case APPLY_CA:			nOpt = nItem::CA;		break;
	case APPLY_HT:			nOpt = nItem::HT;		break;
	case APPLY_DP:			nOpt = nItem::DP;		break;
	case APPLY_BL:			nOpt = nItem::BL;		break;
	case APPLY_EV:			nOpt = nItem::EV;		break;
	default:
		return nValue;
	}

	cData_TEquip* pEquipInfo = g_pDataMng->GetTEquip();
	SAFE_POINTER_RETVAL( pEquipInfo, nValue );

	cItemInfo* pEquipItem = pEquipInfo->GetData( nEquipType );
	SAFE_POINTER_RETVAL( pEquipItem, nValue );

	if( !pEquipItem->IsEnable() )
		return nValue;

	CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( pEquipItem->GetType() );
	SAFE_POINTER_RETVAL( pItem, nValue );

	CsItem::sINFO* pInfo = pItem->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, nValue );

	CDigimonUser* pdigimonUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RETVAL( pdigimonUser, nValue );

	int nDLevel = pdigimonUser->GetBaseStat()->GetLevel();	//디지몬 레벨
	int nTLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();		//테이머 레벨

	//디지몬,테이머 레벨이 제한 레벨 이상일 때만
	if( !pInfo->IsEnableTamerLv( nTLevel ) || !pInfo->IsEnableDigimonLv( nDLevel ) )
		return nValue;

	if( pInfo->s_nUseTime_Min != 0 )
	{
		if( ( pEquipItem->m_nEndTime == ITEMINFO_INVALIDE_TIME )||( pEquipItem->m_nEndTime <= _TIME_TS ) )
			return nValue;
	}

	// 		: 아이템 사용대상 구분
// 	switch( pInfo->s_nUseCharacter )
// 	{
// 	case 1:				//테이머 & 디지몬 동시 사용
// 	case 2:				//디지몬 전용
// 		break;
// 	case 0:				//해당사항없음
// 	case 3:				//테이머 전용
// 	default:
// 		return nValue;		
// 	}

	for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION ; i++ )
	{
		if( pEquipItem->m_nAccOption[ i ] == 0 )
			break;

		if( pEquipItem->m_nAccOption[ i ] == nOpt ) // 현재 툴팁 보여줘야되는 옵션의 값만 가져와
		{
			float fValue = ( pEquipItem->m_nAccValues[ i ] * pEquipItem->m_nRate * 0.01 );
			switch( nOpt )
			{
			case nItem::AS:// 공격 속도 증가는 -로 증가되기 때문에 -1를 곱한다.
				{					
					int nBase = FMDigimon::GetAttackSpeed( pdigimonUser->GetFTID() ) * 1000;
					nValue += (__int64)(nBase * (fValue * 0.0001) * -1);
				}
				break;
			case nItem::CA:
			case nItem::EV:
			case nItem::BL:
				nValue += (__int64)(fValue * 100);
				break;
			default:
				nValue += (__int64)fValue;
				break;
			}
		}
	}

	return nValue;
}

__int64 FMDigimon::GetBuffDigimonStatValue_N( nTable_Parameter eType, __int64 nOrgValue /*= 0 */ )
{
	__int64 nValue = 0;
	if( nOrgValue <= 0 )
		nOrgValue = 1;

	CDigimonUser* pDigimon = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pDigimon, 0 );

	cBuffData* pBuffData = pDigimon->GetBuff();
	SAFE_POINTER_RETVAL( pBuffData, 0 );

	cBuffData::LIST_BUFF * pBufflist = pBuffData->GetBuff();
	SAFE_POINTER_RETVAL( pBufflist, 0 );

	cBuffData::LIST_BUFF_IT it = pBufflist->begin();
	cBuffData::LIST_BUFF_IT itEnd = pBufflist->end();	
	for( ; it != itEnd; ++it )
	{
		SAFE_POINTER_CON( it->s_pBuffTable );

		CsBuff::sINFO* pInfo = it->s_pBuffTable->GetInfo();
		SAFE_POINTER_CON( pInfo );

		DWORD dwSkillCode = 0;
		for( int n = 0; n < 2; ++n )
		{
			if( n == 0 )
				dwSkillCode = pInfo->s_dwSkillCode;
			else
				dwSkillCode = pInfo->s_dwDigimonSkillCode;

			if( 0 == dwSkillCode )
				continue;

			CsSkill* pFindSkill = nsCsFileTable::g_pSkillMng->GetSkill( dwSkillCode );
			SAFE_POINTER_CON( pFindSkill );

			CsSkill::sINFO* pSkillInfo = pFindSkill->GetInfo();	
			SAFE_POINTER_CON( pSkillInfo );

			int nIdx = 0;
			if( pFindSkill->IsDigimonSkill() )
				nIdx = 1;

			for( int i = nIdx; i < SKILL_APPLY_MAX_COUNT; i++ )
			{
				if( pSkillInfo->s_Apply[ i ].s_nA != eType )
					continue;

				switch( pSkillInfo->s_Apply[ i ].s_nID )
				{
				case nSkill::Me_101:
					{
						nValue += pSkillInfo->s_Apply[ i ].s_nB + pSkillInfo->s_Apply[ i ].s_nIncrease_B_Point * it->s_nBuffCurLV;
					}
					break;
				case nSkill::Me_105:// A=A+(A*B/100)
					{
						//__int64 nTmpVal = pSkillInfo->s_Apply[ i ].s_nB;
						nValue += (__int64)( nOrgValue * pSkillInfo->s_Apply[ i ].s_nB ) / 100;
					}break;
				case nSkill::Me_106://A=A+(B+(Lv×증가수치값÷100))
					{
						__int64 nTmpVal = pSkillInfo->s_Apply[ i ].s_nB + pSkillInfo->s_Apply[ i ].s_nIncrease_B_Point * it->s_nBuffCurLV;
						// 						if( nTmpVal % 100 > 49 )
						// 						{
						// 							nValue += (__int64)( nOrgValue * nTmpVal ) / 100 + 1;
						// 						}
						// 						else
						nValue += (__int64)( nOrgValue * nTmpVal ) / 100;
					}
					break;
				case nSkill::Me_107:
					{
						__int64 nTmpVal = pSkillInfo->s_Apply[ i ].s_nB + pSkillInfo->s_Apply[ i ].s_nIncrease_B_Point * it->s_nBuffCurLV;
						// 						if( nTmpVal % 100 > 49 )
						// 						{
						// 							nValue += (__int64)( nOrgValue * nTmpVal ) / 100 + 1;
						// 						}
						// 						else
						nValue -= (__int64)( nOrgValue * nTmpVal ) / 100;
					}
					break;
				case 206:/*nSkill::Me_206*/
					{
						__int64 nTmpVal = pSkillInfo->s_Apply[ i ].s_nB + pSkillInfo->s_Apply[ i ].s_nIncrease_B_Point * (it->s_nBuffCurLV - 1);
						// 수리몬(플라이비몬) 스킬 버프 적용 시 디지몬 스테이터스 툴팁 오류로 인한 수정( 14.11.17 chu8820 )
						// 14.8월에 수정된 내용으로, 스킬 버프로 인해 상승하는 수치를 리턴하던 함수에서
						// '현재 적용 중인 스탯 값*상승률' 을 리턴 시키도록 수정된 코드였지만 이부분에서 비정상 동작을 함.
						// 수정 이유를 찾지 못해 일단 주석처리함... ㅠㅠㅠㅠㅠ
						//						double fPer = nTmpVal * 0.01;
						//						nTmpVal = (__int64)( nOrgValue * fPer );
						nValue += nTmpVal;
					}
					break;
				case 207/*nSkill::Me_207*/:
					{
						nValue += pSkillInfo->s_Apply[ i ].s_nB + pSkillInfo->s_Apply[ i ].s_nIncrease_B_Point * (it->s_nBuffCurLV - 1);
						nValue *= 100;
					}
					break;
				default:
					continue;
// 				default:
// 					//nValue += pSkillInfo->s_Apply[ i ].s_nB + pSkillInfo->s_Apply[ i ].s_nIncrease_B_Point * it->s_nBuffCurLV;
// 					assert_cs( false );
// 					break;
				}
			}
		}

	}
	return nValue;
}

__int64 FMDigimon::GetDeBuffDigimonStatValue_N( nTable_Parameter eType, __int64 nOrgValue /*= 0 */ )
{
	__int64 nValue = 0;

	CDigimonUser* pDiUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pDiUser, nValue );

	cBuffData* pBuffData = pDiUser->GetBuff();
	SAFE_POINTER_RETVAL( pBuffData, nValue );

	cBuffData::LIST_BUFF * pDData = pBuffData->GetDeBuff();
	cBuffData::LIST_BUFF_IT it = pDData->begin();
	for( ; it != pDData->end(); ++it )
	{
		SAFE_POINTER_CON( it->s_pBuffTable );
		CsBuff::sINFO* pInfo = it->s_pBuffTable->GetInfo();
		SAFE_POINTER_CON( pInfo );

		if( 0 == pInfo->s_dwSkillCode )
			continue;

		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pInfo->s_dwSkillCode );
		SAFE_POINTER_CON( pSkill );

		CsSkill::sINFO* pSkillInfo = pSkill->GetInfo();	
		SAFE_POINTER_CON( pSkillInfo );

		for( int i = 0; i < SKILL_APPLY_MAX_COUNT; i++ )
		{
			if( pSkillInfo->s_Apply[ i ].s_nA == eType )
				nValue += pSkillInfo->s_Apply[ i ].s_nIncrease_B_Point * it->s_nBuffCurLV;
		}
	}
	
	return nValue;
}

__int64 FMDigimon::GetSysBuffDigimonStatValue_N( nTable_Parameter eType, __int64 nOrgValue /*= 0 */ )
{
	__int64 nValue = 0;

	CDigimonUser* pDiUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pDiUser, nValue );

	cBuffData* pBuffData = pDiUser->GetBuff();
	SAFE_POINTER_RETVAL( pBuffData, nValue );

	cBuffData::LIST_BUFF * pDData = pBuffData->GetSystemBuff();
	cBuffData::LIST_BUFF_IT it = pDData->begin();
	for( ; it != pDData->end(); ++it )
	{
		SAFE_POINTER_CON( it->s_pBuffTable );
		CsBuff::sINFO* pInfo = it->s_pBuffTable->GetInfo();
		SAFE_POINTER_CON( pInfo );

		if( 0 == pInfo->s_dwSkillCode )
			continue;

		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pInfo->s_dwSkillCode );
		SAFE_POINTER_CON( pSkill );

		CsSkill::sINFO* pSkillInfo = pSkill->GetInfo();	
		SAFE_POINTER_CON( pSkillInfo );

		for( int i = 0; i < SKILL_APPLY_MAX_COUNT; i++ )
		{
			if( pSkillInfo->s_Apply[ i ].s_nA == eType )
				nValue += pSkillInfo->s_Apply[ i ].s_nIncrease_B_Point * it->s_nBuffCurLV;
		}
	}

	return nValue;
}
