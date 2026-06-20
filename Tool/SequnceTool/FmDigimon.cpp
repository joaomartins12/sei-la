
#include "stdafx.h"
#include "FmDigimon.h"


int FMDigimon::GetMaxExp( int nLevel )
{
	return (int)( nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, CsBaseMng::BT_DIGIMON_EXP )->GetInfo()->s_dwExp*0.01f );
}

int FMDigimon::GetMaxSkillExp( int nDigimonID, int nSkillExpLevel )
{
	assert_cs( nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo()->s_dwBaseLevel > 3 );
	int nLevel = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo()->s_dwBaseLevel - 3 + nSkillExpLevel;
	return (int)( nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, CsBaseMng::BT_DIGIMON_EXP )->GetInfo()->s_dwExp*0.01f );
}

//==========================================================================================================
//
//	기본 스탯
//
//==========================================================================================================

int FMDigimon::GetBaseMaxHP( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	return nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nHP + pDigimonFT->s_nBaseStat_HP;
}

int FMDigimon::GetBaseMaxDS( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	return nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nDS + pDigimonFT->s_nBaseStat_DS;
}

int FMDigimon::GetBaseDef( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	return nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nDefence + pDigimonFT->s_nBaseStat_DefPower;
}

int FMDigimon::GetBaseEv( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	int nValue = nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nEvasion + pDigimonFT->s_nBaseStat_Evasion;
	return (int)( nValue*0.01f );
}

int FMDigimon::GetBaseCr( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	int nValue = nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nCritical + pDigimonFT->s_nBaseStat_CriticalRate;
	return (int)( nValue*0.01f );
}

int FMDigimon::GetBaseAtt( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	return nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nAttack + pDigimonFT->s_nBaseStat_AttPower;
}

//==========================================================================================================
//
//	에볼루션 스탯
//
//==========================================================================================================

float FMDigimon::_GetEvolRate( nsCsDigimonTable::eEVOLUTION_TYPE eEvol )
{
	// LS*EvolRate - LS
	// => LS*( EvolRate - 1.0f )
	switch( eEvol )
	{
	case nsCsDigimonTable::ET_INTRAINING:
		return 0.0f;
	case nsCsDigimonTable::ET_ROOKIE:
		return 0.0f;
	case nsCsDigimonTable::ET_CHAMPION:
		return 0.5f;
	case nsCsDigimonTable::ET_ULTIMATE:
		return 0.85f;
	case nsCsDigimonTable::ET_MEGA:
		return 1.0f;
	case nsCsDigimonTable::ET_BUSTERMODE:
		return 1.5f;
	}
	assert_cs( false );
	return 1.0f;
}

int FMDigimon::GetEvolMaxHP( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	return (int)( nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nHP * _GetEvolRate( pDigimonFT->s_eEvolutionType ) );
}

int FMDigimon::GetEvolMaxDS( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	return (int)( nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nDS * _GetEvolRate( pDigimonFT->s_eEvolutionType ) );
}

int FMDigimon::GetEvolDef( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	return (int)( nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nDefence * _GetEvolRate( pDigimonFT->s_eEvolutionType ) );
}

int FMDigimon::GetEvolAtt( int nDigimonID, int nLevel )
{
	CsDigimon::sINFO* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo();
	return (int)( nsCsFileTable::g_pBaseMng->GetDigimonBase( nLevel, pDigimonFT->s_nDigimonType )->GetInfo()->s_nAttack * _GetEvolRate( pDigimonFT->s_eEvolutionType ) );
}

//==========================================================================================================
//
//	기타
//
//==========================================================================================================
int FMDigimon::GetCostEvlove( DWORD dwSrcDigimonID, int nSrcLevel, DWORD dwDestDigimonID )
{
	CsDigimon::sINFO* pSrcFTD = nsCsFileTable::g_pDigimonMng->GetDigimon( dwSrcDigimonID )->GetInfo();
	CsDigimon::sINFO* pDestFTD = nsCsFileTable::g_pDigimonMng->GetDigimon( dwDestDigimonID )->GetInfo();

	return
	( 
	( ( pDestFTD->s_eEvolutionType*pDestFTD->s_eEvolutionType ) -
	  ( pSrcFTD->s_eEvolutionType*pSrcFTD->s_eEvolutionType ) - 5 )*
	  ( ( pDestFTD->s_dwBaseLevel - nSrcLevel + 110 )/5 )
	  ) + 20;
}

float FMDigimon::GetAttackRange( DWORD nDigimonID, float fScale )
{
	CsDigimon* pFTD = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID );

	// 근접
	if( pFTD->GetInfo()->s_nDigimonType == 1 )
		return (float)( pFTD->GetInfo()->s_nBaseStat_AttRange )*fScale;

	// 원거리
	return (float)( pFTD->GetInfo()->s_nBaseStat_AttRange );
}


float FMDigimon::GetAttackSpeed( DWORD nDigimonID )
{
	CsDigimon* pFTD = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID );
	return ( pFTD->GetInfo()->s_nBaseStat_AttSpeed )*0.001f;
}

int FMDigimon::GetSkillAtt( DWORD nSkillID, int nSkillLevel )
{
	CsSkill::sINFO* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillID )->GetInfo();
	return pFTSkill->s_Apply[ 0 ].s_nB + nSkillLevel*pFTSkill->s_Apply[ 0 ].s_nIncrease_B_Point;
}


//int FMDigimon::GetSkillExp( cDigimonUser* pChar, UINT dwEvoID )
//{
//	/*CDigimonEvolveObj* pObj = g_DigimonEvolution.GetEvolveObj(dwEvoID);	
//	CsDigimon* pCsDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwEvoID );		
//	CsBase* pCsDigimonBase = nsCsFileTable::g_pBaseMng->GetDigimonBase(pCsDigimon->GetInfo()->s_dwBaseLevel-3 + pChar->m_EvoUnit[pObj->m_nEvoSlot].m_nSkillExpLevel, CsBaseMng::BT_DIGIMON_EXP);
//	CsBase::sINFO* pInfo = pCsDigimonBase->GetInfo();
//
//	int nExp = 0;
//	if(pInfo)
//	{			
//		nExp = (int)(pChar->m_EvoUnit[pObj->m_nEvoSlot].m_nSkillExp * 100.f / (float)pInfo->s_dwExp);
//
//		if((float)pInfo->s_dwExp <= 0.f)
//			nExp = 0;
//			
//	}
//
//	return nExp;*/
//
//	return 0;
//}
//
//int FMDigimon::GetSkillAttackPower( cDigimonUser* pChar, UINT dwEvoID, UINT dwSkillCode, int nSkillLevel )
//{
//	int nSkillAP = 0;
//	int nAP = GetAttackPower( pChar, dwEvoID );
////	int nTotalAP = 0;
//
//	CsSkill* pCsSkill = nsCsFileTable::g_pSkillMng->GetSkill(dwSkillCode);
//	assert_cs(pCsSkill);
//
//	CsSkill::sINFO* pInfo = pCsSkill->GetInfo();
//	assert_cs(pInfo);
//
//	
//	switch( pInfo->s_Apply[0].s_dwID )
//	{
//	case 102:
//		nSkillAP = nAP + pInfo->s_Apply[0].s_nB;
//		nSkillAP += (pInfo->s_Apply[0].s_nIncrease_B_Point * (nSkillLevel-1))/100;
//		break;
//	}
//
//	return nSkillAP;
//}
//
//int FMDigimon::GetNextSkillAttackPower( cDigimonUser* pChar, UINT dwEvoID, UINT dwSkillCode, int nSkillLevel  )
//{
//	int nSkillAP = 0;
//	int nAP =GetAttackPower( pChar, dwEvoID );
//
//	CsSkill* pCsSkill = nsCsFileTable::g_pSkillMng->GetSkill(dwSkillCode);
//	assert_cs(pCsSkill);
//	CsSkill::sINFO* pInfo = pCsSkill->GetInfo();
//	assert_cs(pInfo);
//
//	switch( pInfo->s_Apply[0].s_dwID )
//	{
//	case 102:
//		nSkillAP = nAP + pInfo->s_Apply[0].s_nB;
//		nSkillAP += (pInfo->s_Apply[0].s_nIncrease_B_Point * (nSkillLevel))/100;
//		break;
//	}
//
//	return nSkillAP;
//}
