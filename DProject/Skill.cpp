
#include "stdafx.h"
#include "Skill.h"
#include "_Interface/Adapt/AdaptMainFrame.h"
#include "../LibProj/CsFunc/CrashLogger.h"

#define TAMER_BASE_SKILL_COUNT 8

cSkill::cSkill() : m_pParent( NULL ), m_nSkillIndex( 0 ), m_AdaptMainFrame( NULL )
{
	m_AdaptMainFrame = new AdaptMainFrame;
	if( m_AdaptMainFrame )
		m_AdaptMainFrame->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_MAINFRAME_CONTENTS ) );
}

cSkill::~cSkill()
{

}

void cSkill::Delete()
{
	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
		sINFO::DeleteInstance( m_vpInfo[ i ] );
	m_vpInfo.Destroy();
}

CsSkill* cSkill::GetFTSkill( int nSkillIndex )
{ 
	return ( m_vpInfo.IsExistElement( nSkillIndex ) ) ? m_vpInfo[ nSkillIndex ]->s_pFTSkill : NULL; 
}
bool  cSkill::IsSkill( int nSkillIndex )
{ 
	return m_vpInfo.IsExistElement( nSkillIndex ); 
}	

cSkill::sINFO*  cSkill::GetSkill( int nSkillIndex )
{ 
	return ( m_vpInfo.IsExistElement( nSkillIndex ) ) ? m_vpInfo[ nSkillIndex ] : NULL; 
}

void cSkill::SetTamer( CsC_AvObject* pParent, CsTamer* pTamerFT )
{
	assert_cs( m_vpInfo.Size() == 0 );
	m_pParent = pParent;
	
	sINFO* pInfo = NULL;

	pInfo = sINFO::NewInstance();
	pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( 1000001 );
	pInfo->s_eAnimation = ANI::INVALIDE_ID;
	pInfo->s_CoolTime.SetEndTime( 0.2f );
	pInfo->fUseSkill = &cSkill::__Wait_Digimon;
	m_vpInfo.PushBack( pInfo );

	pInfo = sINFO::NewInstance();
	pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( 1000002 );
	pInfo->s_eAnimation = ANI::INVALIDE_ID;
	pInfo->s_CoolTime.SetEndTime( 0.2f );
	pInfo->fUseSkill = &cSkill::__Follow_Digimon;
	m_vpInfo.PushBack( pInfo );

	pInfo = sINFO::NewInstance();
	pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( 1000003 );
	pInfo->s_eAnimation = ANI::INVALIDE_ID;
	pInfo->s_CoolTime.SetEndTime( 0.2f );
	pInfo->fUseSkill = &cSkill::__Attack_Digimon;
	m_vpInfo.PushBack( pInfo );

	pInfo = sINFO::NewInstance();
	pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( 1000004 );
	pInfo->s_eAnimation = ANI::INVALIDE_ID;
	pInfo->s_CoolTime.SetEndTime( 1.0f );
	pInfo->fUseSkill = &cSkill::__Pickup_Digitama;
	m_vpInfo.PushBack( pInfo );

// 	pInfo = sINFO::NewInstance();
// 	pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( 1000005 );
// 	pInfo->s_eAnimation = ANI::INVALIDE_ID;
// 	pInfo->s_CoolTime.SetEndTime( 1.0f );
// 	pInfo->s_nTacticsIndex = 1;
// 	pInfo->fUseSkill = &cSkill::__BattleTag_Digimon1;
// 	m_vpInfo.PushBack( pInfo );
// 
// 	pInfo = sINFO::NewInstance();
// 	pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( 1000006 );
// 	pInfo->s_eAnimation = ANI::INVALIDE_ID;
// 	pInfo->s_CoolTime.SetEndTime( 1.0f );
// 	pInfo->s_nTacticsIndex = 2;
// 	pInfo->fUseSkill = &cSkill::__BattleTag_Digimon2;
// 	m_vpInfo.PushBack( pInfo );
//
//	if( g_pDataMng->GetTactics()->_GetOpenSlot() == 3 )
	if( g_pDataMng && g_pDataMng->GetTactics() )
	{
		for( int i = 0; i < g_pDataMng->GetTactics()->_GetOpenSlot(); ++i )
		{
			pInfo = sINFO::NewInstance();
			pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( 1000005/*1000008*/ );
			pInfo->s_eAnimation = ANI::INVALIDE_ID;
			pInfo->s_CoolTime.SetEndTime( 1.0f );
			pInfo->s_nTacticsIndex = i;
			//pInfo->fUseSkill = &cSkill::__BattleTag_Digimon3;
			pInfo->fUseSkill = &cSkill::__BattleTag_Digimon;
			m_vpInfo.PushBack( pInfo );
		}
	}

	pInfo = sINFO::NewInstance();
	pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( 1000007 );
	pInfo->s_eAnimation = ANI::INVALIDE_ID;
	pInfo->s_CoolTime.SetEndTime( 10.0f );
	pInfo->fUseSkill = &cSkill::__Away;
	m_vpInfo.PushBack( pInfo );

	CsTamer::sINFO* pFTSkill = pTamerFT->GetInfo();
	if( pFTSkill )
	{
		for( int i=0; i<TAMER_MAX_SKILL_COUNT; ++i )
		{
			DWORD dwSkillID = pFTSkill->s_Skill[ i ].s_dwID;
			if( dwSkillID == 0 )
				continue;

			CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( pFTSkill->s_Skill[ i ].s_dwID, -1 );
			SAFE_POINTER_CON( pTamerSkill );

			CsTamerSkill::sTSkill_INFO* pTSKillInfo = pTamerSkill->Get_TSkill_Info();
			SAFE_POINTER_CON( pTSKillInfo );

			if( pTSKillInfo->s_dwSkillCode == 0 )
				continue;

			if( nsCsFileTable::g_pSkillMng->IsSkill( pTSKillInfo->s_dwSkillCode ) == true )
			{
				pInfo = sINFO::NewInstance();
				pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pTSKillInfo->s_dwSkillCode );
				pInfo->s_eAnimation = (ANI::eTYPE)( ANI::ATTACK_SKILL1 + i );
				pInfo->s_CoolTime.SetEndTime( pInfo->s_pFTSkill->GetInfo()->s_fCooldownTime*0.001f + 0.15f );
				m_vpInfo.PushBack( pInfo );
			}
		}
	}

	m_nSkillIndex = -1;	// 디폴트 -1
}

void cSkill::SetTamerCashSkill( u4 nIndex, u4 nCoolTime, u4 nLifeTime )
{
	CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( nIndex, -1 );
	SAFE_POINTER_RET( pTamerSkill );

	CsTamerSkill::sTSkill_INFO * TskillInfo = pTamerSkill->Get_TSkill_Info();
	SAFE_POINTER_RET( TskillInfo );

	if( TskillInfo->s_dwSkillCode == 0 )
		return;
	
	if( nsCsFileTable::g_pSkillMng->IsSkill( TskillInfo->s_dwSkillCode ) == true )
	{
		sINFO* pInfo = pInfo = sINFO::NewInstance();
		pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( TskillInfo->s_dwSkillCode );
		pInfo->s_eAnimation = (ANI::eTYPE)( ANI::ATTACK_SKILL1 );
		
		if( !(nCoolTime < _TIME_TS + 1) )
		{
			pInfo->s_CoolTime.SetEndTime( pInfo->s_pFTSkill->GetInfo()->s_fCooldownTime*0.001f + 0.15f );
			double fOffset = pInfo->s_CoolTime.GetEndTime() - ( nCoolTime - _TIME_TS ) - 1;
			pInfo->s_CoolTime.Start( fOffset );
		}

		pInfo->s_nLifeTime = nLifeTime;

		m_vpInfo.PushBack( pInfo );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}

void cSkill::DelTamerCashSkill( DWORD dwSkill_Index )	// 테이머 캐쉬 스킬 삭제
{
	for( int i=0; i<m_vpInfo.Size(); ++i )
	{
		CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( dwSkill_Index, -1 );

		if( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode != m_vpInfo[i]->s_pFTSkill->GetInfo()->s_dwID )
			continue;

		bool bAfter = false;
		if( m_vpInfo[i] )
			bAfter = true;

		m_vpInfo.DeleteElement_FromData( m_vpInfo[ i ] );

		cResist::sCHAR_QUICKSLOT::sCHAR* pQuick = g_pCharMng->GetTamerUser()->GetQuickSlot();

		for( int j=0; j<8; ++j )
		{
			if( pQuick->s_QuickSlotInfo[j].s_nID == i )
			{
				pQuick->s_QuickSlotInfo[j].s_eType = cResist::NONE;
				pQuick->s_QuickSlotInfo[j].s_nID   = 131071;
				//break;
			}
		}
		
		if( bAfter == true )
		{
			for( int j=0; j<8; ++j )
			{
				if( pQuick->s_QuickSlotInfo[j].s_nID == i + 1 )
				{
					cResist::eTYPE eSlotType = cResist::SKILL;
					pQuick->s_QuickSlotInfo[j].s_eType = eSlotType;
					pQuick->s_QuickSlotInfo[j].s_nID   = i;
					break;
				}
			}
		}
		break;
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_TAMERSTATUS, NULL );
}

int cSkill::CountCashSkill()
{
	int nCount = 0;
	int nSize = m_vpInfo.Size();

	for( int i=0; i<nSize; ++i )
	{
		if( m_vpInfo[i]->s_nLifeTime != 0 )
			nCount++;
	}

	return nCount;
}

void cSkill::SetDigimon( CsC_AvObject* pParent, CsDigimon* pDigimonTB )
{
	assert_cs( m_vpInfo.Size() == 0 );
	assert_cs( pDigimonTB != NULL );

	m_pParent = pParent;

	DWORD dwSkillID;
	for( int i=0; i<DIGIMON_MAX_SKILL_COUNT; ++i )
	{
		dwSkillID = pDigimonTB->GetInfo()->s_Skill[ i ].s_dwID;
		if( dwSkillID == 0 )
			continue;

		if( nsCsFileTable::g_pSkillMng->IsSkill( dwSkillID ) == true )
		{
			sINFO* pInfo = sINFO::NewInstance();
			pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( dwSkillID );
			pInfo->s_eAnimation = (ANI::eTYPE)( ANI::ATTACK_SKILL1 + i );
			pInfo->s_CoolTime.SetEndTime( pInfo->s_pFTSkill->GetInfo()->s_fCooldownTime*0.001f + 0.15f );
			m_vpInfo.PushBack( pInfo );
		}
	}
}

void cSkill::SetSkillCooltime( u4 nSkillCode, u4 nCoolTime )
{
	DWORD dwRemainTS= nCoolTime - _TIME_TS;
	if( dwRemainTS <= 0 )
		return;

	for( int i=0; i<m_vpInfo.Size(); ++i )
	{
		sINFO* pInfo = m_vpInfo[i];
		SAFE_POINTER_CON( pInfo );
		SAFE_POINTER_CON( pInfo->s_pFTSkill );
		CsSkill::sINFO*	pSkill =  pInfo->s_pFTSkill->GetInfo();
		SAFE_POINTER_CON( pSkill );

		if( nSkillCode != pSkill->s_dwID )
			continue;

		pInfo->s_CoolTime.SetEndTime( pSkill->s_fCooldownTime * 0.001f + 0.15f );
		double fOffset = pInfo->s_CoolTime.GetEndTime() - dwRemainTS;
		pInfo->s_CoolTime.Start( fOffset );
		break;
	}
}

void cSkill::SetDigimonCashSkill( u4 nSkillCode, u4 nCoolTime)
{
	if( !nsCsFileTable::g_pSkillMng->IsSkill( nSkillCode ) )
		return;

	sINFO* pInfo = NULL;

	pInfo = sINFO::NewInstance();
	pInfo->s_pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode );
	pInfo->s_eAnimation = (ANI::eTYPE)(ANI::IDLE_NORMAL);//( ANI::ACT_BUFF );
	pInfo->s_nLifeTime = 0;
	// 쿨타임 적용
	if( nCoolTime > _TIME_TS + 1 )
	{
		pInfo->s_CoolTime.SetEndTime( pInfo->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f );
		double fOffset = pInfo->s_CoolTime.GetEndTime() - ( nCoolTime - _TIME_TS ) - 1;
		pInfo->s_CoolTime.Start( fOffset );
	}

	m_vpInfo.PushBack( pInfo );
}

void cSkill::AddDigimonCashSkill( u4 nSkillCode, u4 nCoolTime)
{
	cData_PostLoad::sDATA* sDigimonData = g_pDataMng->GetPostLoad()->GetDigimonData();
	for( int i=0; i<nLimit::EvoUnit; ++i )
	{
		if( g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot == sDigimonData->s_DCashSkill[i].s_nDigimonEvoStatus )
		{
			bool	AddSkill = true;
			for( int j=0; j<nLimit::MAX_ItemSkillDigimon; ++j )
			{
				if( sDigimonData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] == nSkillCode )
					AddSkill = false;
			}

			if( AddSkill )
			{
				for( int j=0; j<nLimit::MAX_ItemSkillDigimon; ++j )
				{
					if( sDigimonData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] == 0 )
					{
						sDigimonData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] = nSkillCode;
						break;
					}
				}
			}
			break;
		}


		if( sDigimonData->s_DCashSkill[i].s_nDigimonEvoStatus != 0 )
			continue;

		for( int j=0; j<nLimit::MAX_ItemSkillDigimon; ++j )
		{
			if( sDigimonData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] != 0 )
				continue;

			sDigimonData->s_DCashSkill[i].s_nDigimonEvoStatus = (u1)g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot;
			sDigimonData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] = nSkillCode;
			sDigimonData->s_DCashSkill[i].s_nSkillCoolTime[j] = 0;

			break;
		}

		break;
	}
}

void cSkill::DelDigimonCashSkill( u4 nSkillCode )
{
	for( int i=0; i<m_vpInfo.Size(); ++i )
	{
		CsSkill::sINFO*	pSkill =  m_vpInfo[i]->s_pFTSkill->GetInfo();

		if( nSkillCode != pSkill->s_dwID )
			continue;

		m_vpInfo.DeleteElement_FromData( m_vpInfo[ i ] );

		cResist::sCHAR_QUICKSLOT::sCHAR* pQuick = g_pCharMng->GetDigimonUser(0)->GetQuickSlot();

		//////////////////////////////////////////////////////////////////////////
		// 퀵 슬롯에 스킬 정보가 있으면 삭제.
		bool bAfter = false;
		if( m_vpInfo[i] )
			bAfter = true;

		for( int j=0; j<8; ++j )
		{
			if( pQuick->s_QuickSlotInfo[j].s_nID == i )
			{
				pQuick->s_QuickSlotInfo[j].s_eType = cResist::NONE;
				pQuick->s_QuickSlotInfo[j].s_nID   = 131071;
				//break;
			}
		}

		if( bAfter == true )
		{
			for( int j=0; j<8; ++j )
			{
				if( pQuick->s_QuickSlotInfo[j].s_nID == i + 1 )
				{
					cResist::eTYPE eSlotType = cResist::SKILL;
					pQuick->s_QuickSlotInfo[j].s_eType = eSlotType;
					pQuick->s_QuickSlotInfo[j].s_nID   = i;
					break;
				}
			}
		}

		// PostData에 등록된 Data 초기화
		cData_PostLoad::sDATA* pData = g_pDataMng->GetPostLoad()->GetDigimonData();
		for( int i=0; i<nLimit::EvoUnit; ++i )
		{
			if( pData->s_DCashSkill[i].s_nDigimonEvoStatus == g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot )			
			{
				int StatCnt = 0;
				for( int j=0; j<nLimit::MAX_ItemSkillDigimon; ++j )				
				{
					u4	SCODE = pData->s_DCashSkill[i].s_nDigimonCashSkillCode[j];
					if( SCODE == nSkillCode )
						pData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] = 0;

					if(pData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] ==0)
					{
						StatCnt++;
						if(StatCnt == nLimit::MAX_ItemSkillDigimon)
							pData->s_DCashSkill[i].s_nDigimonEvoStatus = 0;
					}						
				}
			}
		}

		//cData_PostLoad::sDATA* pData = g_pDataMng->GetPostLoad()->GetDigimonData();
		//for( int i=0; i<nLimit::EvoUnit; ++i )
		//{
		//	if( pData->s_DCashSkill[i].s_nDigimonEvoStatus == g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot )			
		//	{
		//		pData->s_DCashSkill[i].s_nDigimonEvoStatus = 0;
		//		for( int j=0; j<nLimit::MAX_ItemSkillDigimon; ++j )				
		//		{
		//			if( pData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] == nSkillCode )
		//				pData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] = 0;
		//		}
		//	}
		//}

		// 스킬 코드값 정렬.
		//for( int i=0; i<nLimit::EvoUnit; ++i )
		//{
		//	for( int j=0; j<nLimit::MAX_ItemSkillDigimon; ++j )				
		//	{
		//		int next = j+1;
		//		if( pData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] == 0 && next < nLimit::MAX_ItemSkillDigimon )
		//		{
		//			pData->s_DCashSkill[i].s_nDigimonCashSkillCode[j] = pData->s_DCashSkill[i].s_nDigimonCashSkillCode[next];

		//			pData->s_DCashSkill[i].s_nDigimonCashSkillCode[next] = 0;
		//		}
		//	}
		//}

		break;
	}
}

SHORT cSkill::GetMySkill_Index( u4 nSkillCode )
{
	for( int i=0; i<m_vpInfo.Size(); ++i )
	{
		if( m_vpInfo[i]->s_pFTSkill->GetInfo()->s_dwID != nSkillCode )
			continue;

		return i;
	}

	return -1;
}

void cSkill::__UseSkill( int nSkilIndex )
{
	//=============================================================================================================
	//	사망 여부
	//=============================================================================================================
	if( m_pParent->IsServerDie() == true )
	{
		return;
	}

	if ( CheckTamerToUseSkill() == false )
		return;
	
	assert_cs( m_vpInfo.IsExistElement( nSkilIndex ) );

	// 디지몬일 경우에
	switch( m_pParent->GetLeafRTTI() )
	{
	case RTTI_TUTORIAL_DIGIMON:
	case RTTI_DIGIMON_USER:
		// 파트너몬 교체중 스킬 불거
		if( g_pDataMng->GetServerSync()->IsChageDigimon() )			
		{
			cPrintMsg::PrintMsg( 30015 );
			return;
		}
		break;
	case RTTI_TUTORIAL_TAMER:
	case RTTI_TAMER_USER:
		if( g_pDataMng->GetServerSync()->IsChageDigimon() )		// 파트너몬 교체 중 스킬 사용 할 수 없다.
		{
			cPrintMsg::PrintMsg( 30015 );
			return;
		}
		break;
	default:
		assert_cs( false );
	}
	
	// 사용 레벨 체크
	if( IsUseSkill( m_pParent, nSkilIndex ) == false )
	{
		return;
	}

	sINFO* pSkillInfo = m_vpInfo[ nSkilIndex ];
	CsSkill* pFTSkill = pSkillInfo->s_pFTSkill;

	// 쿨타임 중인가
	if( ( pSkillInfo->s_CoolTime.IsCoolTime() == true )&&( net::game ) )
	{
		if(pFTSkill->GetInfo()->s_dwID != 1000004)
			cPrintMsg::PrintMsg( 13002 );

		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER ||
			m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13002 );
		return;
	}

	if( pSkillInfo->fUseSkill == NULL )
	{
		// 사용 대상 체크 - _Target_~ 여기서 서버로 스킬 송신
		switch( pFTSkill->GetInfo()->s_nTarget )
		{
		case 11:			// 테이머 유저와 디지몬 전체(파트너와 용병 디지몬)
			_Target_My_All( nSkilIndex );
			break;
		case 12:			// 테이머 유저에게만..
			_Target_My_Tamer( nSkilIndex );
			break;
		case 13:			// 나의 파트너 디지몬에게만..
			_Target_My_Partner_Digimon( nSkilIndex );
			break;
		case 14:			// 나의 디지몬 전체
			_Target_My_All_Digimon( nSkilIndex );			
			break;
		case 55:			// 다른 유저 테이머 1명 + 해당 유저 디지몬 전체
			_Target_Another_All( nSkilIndex );
			break;
		case 51:			// 적 디지몬 한개체
			_Target_One_Enemy( nSkilIndex );
			break;
		case 22:	// 자신의 파티에 속한 디지몬 한개체
			_Target_One_Digimon_Party( nSkilIndex );
			break;
		case 24:	// 자신의 파티에 속한 디지몬 전체
			_Target_All_Digimon_Party( nSkilIndex );
			break;
		case 25:
			_Target_My_All( nSkilIndex );
			break;
		case 17:	// 파트너몬 주위로 스킬 반경 설정
			_Target_My_Partner_Around( nSkilIndex );
			break;
		case 18:	// 타겟 주의로 스킬 반경 설정
		case 80:	// 타겟 직선 광역 스킬
			_Target_One_Enemy( nSkilIndex );
			break;
		default:
			assert_cs( false );
		}
	}
	else
	{
		( this->*pSkillInfo->fUseSkill )( pSkillInfo );
	}
}

bool cSkill::IsUseSkill( CsC_AvObject* pParent, int nSkillIndex )
{
	if( nSkillIndex >= GetSkillCount() )
		return false;

	switch( pParent->GetLeafRTTI() )
	{
	case RTTI_TUTORIAL_DIGIMON:
	case RTTI_DIGIMON_USER:
		{
			if( ( (CDigimonUser*)pParent )->GetCurAttributeEvoUnit()->m_nSkillLevel == 0 )
				return false;
		}
		break;
	case RTTI_TUTORIAL_TAMER:
	case RTTI_TAMER_USER:
		break;
	default:
		assert_cs( false );
	}

	// 현재는 레벨에 의한 채크 밖에 없다.
	return ( pParent->GetBaseStat()->GetLevel() >= GetFTSkill( nSkillIndex )->GetInfo()->s_nLimitLevel );
}

// 스킬 시전하기 전에 테이머 상태 확인
bool cSkill::CheckTamerToUseSkill()
{
	// 테이머 상태 - 상점 오픈, 상점 준비
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pTamer, false );
	cCondition* pCondition = pTamer->GetCondition();
	SAFE_POINTER_RETVAL( pCondition, false );

	if( pCondition->IsCondition( nSync::Shop ) || pCondition->IsCondition( nSync::ShopReady ) )
	{
		cPrintMsg::PrintMsg( 30015 );
		return false;
	}
	return true;
}

//==================================================================================================
//
//		테이머 스킬
//
//==================================================================================================
cSkill::sINFO* cSkill::GetTamerActiveSkill( int nIndex )
{
	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		assert_cs( m_vpInfo[ i ]->s_pFTSkill != NULL );
		if( m_vpInfo[ i ]->s_pFTSkill->IsActive() )
		{
			if( (--nIndex) < 0 )
				return m_vpInfo[ i ];
		}
	}
	return NULL;
}

int cSkill::GetTamerActiveSkillIndex( int nIndex )
{
	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		assert_cs( m_vpInfo[ i ]->s_pFTSkill != NULL );
		if( m_vpInfo[ i ]->s_pFTSkill->IsActive() )
		{
			if( (--nIndex) < 0 )
				return i;
		}
	}
	return -1;
}

cSkill::sINFO* cSkill::GetTamerPasiveSkill( int nIndex )
{
	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		assert_cs( m_vpInfo[ i ]->s_pFTSkill != NULL );
		if( m_vpInfo[ i ]->s_pFTSkill->IsPasive() )
		{
			if( (--nIndex) < 0 )
				return m_vpInfo[ i ];
		}
	}
	return NULL;
}

int cSkill::GetTamerPasiveSkillIndex( int nIndex )
{
	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		assert_cs( m_vpInfo[ i ]->s_pFTSkill != NULL );
		if( m_vpInfo[ i ]->s_pFTSkill->IsPasive() )
		{
			if( (--nIndex) < 0 )
				return i;
		}
	}
	return -1;
}

cSkill::sINFO* cSkill::GetTamerEmotionSkill( int nIndex )
{
	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		assert_cs( m_vpInfo[ i ]->s_pFTSkill != NULL );
		if( m_vpInfo[ i ]->s_pFTSkill->IsEmotionSkill() )
		{
			if( (--nIndex) < 0 )
				return m_vpInfo[ i ];
		}
	}
	return NULL;
}

int cSkill::GetTamerEmotionSkillIndex( int nIndex )
{
	int nCount = m_vpInfo.Size();
	for( int i=0; i<nCount; ++i )
	{
		assert_cs( m_vpInfo[ i ]->s_pFTSkill != NULL );
		if( m_vpInfo[ i ]->s_pFTSkill->IsEmotionSkill() )
		{
			if( (--nIndex) < 0 )
				return i;
		}
	}
	return -1;
}



//==================================================================================================
//
//		동작
//
//==================================================================================================

bool cSkill::__IsAttackObject( CsC_AvObject* pTarget )
{
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		if( pTarget->GetLeafRTTI() == RTTI_MONSTER || pTarget->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER )
			return true;
		break;
	case nLIB::SVR_BATTLE:
		switch( pTarget->GetLeafRTTI() )
		{
		case RTTI_MONSTER:
			return true;
		case RTTI_DIGIMON:
			{
				if( ( (CDigimon*)pTarget )->GetBattleTeam() != g_pCharMng->GetDigimonUser( 0 )->GetBattleTeam() )
					return true;
			}
			break;
		}
		break;
	default:
		assert_cs( false );
	}

	return false;
}

void cSkill::_Target_Tamer( int nSkilIndex )
{
	CsC_AvObject* pTarget =	g_pCharResMng->GetTargetMark();
	if( pTarget == NULL )
	{
		cPrintMsg::PrintMsg( cPrintMsg::NO_SELECT_TARGET );
		return;
	}

	if( pTarget->GetLeafRTTI() != RTTI_TAMER )
	{
		// 잘못된 타겟임
		return;
	}

	// HP 체크
	int nNeedHP = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nUseHP;
	if( nNeedHP && ( m_pParent->GetBaseStat()->GetHP() < nNeedHP ) )
	{
		// HP 부족
		cPrintMsg::PrintMsg( 14007 );
		return;
	}
	// DS 체크
	int nNeedDS = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nUseDS;
	if( nNeedDS && ( m_pParent->GetBaseStat()->GetDS() < nNeedDS ) )
	{
		// DS 부족
		cPrintMsg::PrintMsg( 13003 );
		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER ||
			m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13003 );
		return;
	}
	// LevelCheck
	if( m_pParent->GetBaseStat()->GetLevel() < m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nLimitLevel )
	{
		return;
	}
}

void cSkill::_Target_My_All_Digimon( int nSkilIndex )	// Target 14
{
	sINFO* pSkill_Info = m_vpInfo[ nSkilIndex ];

	CsC_AvObject* pTarget = NULL;
	CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );
	assert_csm( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode ) == true, _T( "올바른 테이머 스킬코드가 아닙니다.\n 끄지 말고 김우봉씨를 찾아주세요!!" ) );

	// 스킬 이름을 얻어온다
	TCHAR* pSkillName = pSkill_Info->s_pFTSkill->GetInfo()->s_szName;

	if( pTamerSkill->Get_TSkill_Info()->s_nUseState )	// 0 : 항상 사용, 1: 전투 중 사용 불가
	{
		if( g_pCharMng->GetTamerUser()->IsBattle() == true )	// 전투중에는 사용할 수 없다.
		{
			cPrintMsg::PrintMsg( 16002 );
			return;
		}
	}

	switch( pTamerSkill->Get_TSkill_Info()->s_nType )
	{
	case nTamer::BuffSkill:		// 버프 / 디버프 스킬
		if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
			break;

		if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
		{
			// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
			_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
		}
		else
		{	
			// 지역 설정이 없다면 바로 사용하자
			if(net::game)
				net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
		}
		break;

	case nTamer::ScriptSkill:		// 스크립트 스킬
		switch( pTamerSkill->Get_TSkill_Info()->s_dwFactor1 )
		{
		case 1:		// 아이템 사용 방식의 스킬
			if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
				break;

			if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
			{
				// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
				_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
			}
			else
			{	
				// 지역 설정이 없다면 바로 사용하자
				if(net::game)
					net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
			}
			break;
		}
		break;

	default:
		assert_cs( false );
		break;
	}
}

void cSkill::_Target_One_Enemy( int nSkilIndex )	// Target 51
{
	CsC_AvObject* pTarget =	g_pCharResMng->GetTargetMark();
	if( pTarget == NULL )
	{
		cPrintMsg::PrintMsg( cPrintMsg::NO_SELECT_TARGET );
		return;
	}

	
	if( __IsAttackObject( pTarget ) == false )
		return;

	if( pTarget->IsServerDie() == true )
	{
		// 이미 죽은 몬스터임
		cPrintMsg::PrintMsg( 14009 );
		return;
	}
		
	if( m_pParent->GetProp_Animation()->IsEnableSkill() == false )
	{
		// 스킬을 사용 할수 없음
		return;
	}

	// 어택큐에 스킬이 들어 있는지 체크하자
	if( m_pParent->GetProp_Attack()->IsSkillProcess() == true )
	{
		cPrintMsg::PrintMsg( 13002 );

		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER ||
			m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13002 );
		return;
	}
	
	// HP 체크
	int nNeedHP = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nUseHP;
	if( nNeedHP && ( m_pParent->GetBaseStat()->GetHP() < nNeedHP ) )
	{
		// HP 부족
		cPrintMsg::PrintMsg( 14007 );
		return;
	}
	// DS 체크
	int nNeedDS = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nUseDS;
	if( nNeedDS && ( m_pParent->GetBaseStat()->GetDS() < nNeedDS ) )
	{
		// DS 부족
		cPrintMsg::PrintMsg( 13003 );
		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER ||
			m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13003 );
		return;
	}
	// LevelCheck
	if( m_pParent->GetBaseStat()->GetLevel() < m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nLimitLevel )
	{
		return;
	}

	if(  m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nMemorySkill )	// 메모리 스킬이라면...
	{
		// 스킬에 사용 가능한 메모리 칩 개수 파악
		int nCount = g_pDataMng->GetInven()->GetCount_Item_TypeL( nItem::DigimonSkillMemoryChip );
		if(  m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nReq_Item > nCount )
		{
			cPrintMsg::PrintMsg( 30686 );	// 메모리 칩이 부족하여 스킬을 사용할 수 없습니다.
			return;
		}
	}

	float fRange = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_fAttRange;
	bool bRange = true;
	if( m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nAttType == 1 )
	{
		// 스킬 사용거리가 서버와 클라가 맞지 않아 서버의 공식에 클라를 맞춤.
		// fRange *= m_pParent->GetOrgGameScale();	// 원본
		fRange *= ( g_pModelDataMng->GetData( m_pParent->GetModelID() )->GetInfo()->s_fScale * m_pParent->GetOrgGameScale() );
		bRange = false;
	}


	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		switch( m_pParent->GetLeafRTTI() )
		{
		case RTTI_TUTORIAL_DIGIMON:
		case RTTI_DIGIMON_USER:
			{
				CTamerUser* pTamer = g_pCharMng->GetTamerUser();

				if( ( (CDigimonUser*)m_pParent )->IsRide() == false )
				{
					CsC_AttackProp* pAttackProp = m_pParent->GetProp_Attack();
					pAttackProp->ReleaseAttackingTarget();
					//assert_csm1( pTarget->GetLeafRTTI() == RTTI_MONSTER, L"_Target_One_Enemy : 몬스터가 아님 : RTTI = %d", pTarget->GetLeafRTTI() );
					pAttackProp->SetFollowTarget( pTarget, fRange, (CsC_AttackProp::eFOLLOW_ITERATOR)( CsC_AttackProp::FI_SKILL1 + nSkilIndex ), bRange );
					( (CDigimonUser*)m_pParent )->SetDigimonUserState( CDigimonUser::DU_SKILL );
					pTamer->ReleaseArriveTarget();
					pTamer->SetTargetPos( pTarget->GetPos(), CsMax( TAMERUSER_ATTACK_DIST, fRange ), false, false );
				}
				else
				{
					CTamerUser::sRIDE_TARGET_INFO rti;
					rti.s_nValue1 = CsC_AttackProp::FI_SKILL1 + nSkilIndex;
					rti.s_fValue1 = fRange;
					rti.s_bValue1 = bRange;

					pTamer->SetArriveTarget( pTarget );
					pTamer->SetRideEndState( CTamerUser::RES_SKILL, pTarget, &rti );
					pTamer->SetTargetPos( pTarget->GetPos(), CsMax( TAMERUSER_ATTACK_DIST, fRange ), false, false );				
				}
			}
			break;
		case RTTI_TAMER_USER:
			{
				if( ( (CTamerUser*)m_pParent )->IsRide() == true && g_pDataMng)		//	라이딩 중일 땐 내려야 한다
				{
					cData_ServerSync* pServerSync = g_pDataMng->GetServerSync();
					if( pServerSync && pServerSync->IsEmptyRefCount(cData_ServerSync::RIDE_OFF) )							// 한번만 라이딩 해제기능 보내도록 검사.
					{
						( (CTamerUser*)m_pParent )->SetRideEnd();
						pServerSync->SendServer( cData_ServerSync::RIDE_OFF, 0 );
					}
				}

				( (CTamerUser*)m_pParent )->ReleaseArriveTarget();

				CsC_AttackProp* pAttackProp = m_pParent->GetProp_Attack();
				pAttackProp->ReleaseAttackingTarget();
				//assert_csm1( pTarget->GetLeafRTTI() == RTTI_MONSTER, L"_Target_One_Enemy : 몬스터가 아님 : RTTI = %d", pTarget->GetLeafRTTI() );
				pAttackProp->SetFollowTarget( pTarget, fRange, (CsC_AttackProp::eFOLLOW_ITERATOR)( CsC_AttackProp::FI_SKILL1 + nSkilIndex - TAMER_BASE_SKILL_COUNT + 1 ), bRange );

				sINFO* pSkill_Info = m_vpInfo[ nSkilIndex ];

				CsC_AvObject* pTarget = NULL;
				CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );
				assert_csm( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode ) == true, _T( "올바른 테이머 스킬코드가 아닙니다.\n 끄지 말고 김우봉씨를 찾아주세요!!" ) );

				( (CTamerUser*)m_pParent )->SetActiveIDX( pTamerSkill->Get_TSkill_Info()->s_nIndex );	// 스킬 IDX 저장
				( (CTamerUser*)m_pParent )->SetTamerUserState( CTamerUser::TU_SKILL );
			}
			break;
		default:
			assert_cs( false );
		}
		break;
	case nLIB::SVR_BATTLE:
		{
			sINFO* pSkill_Info = m_vpInfo[ nSkilIndex ];

			if( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pSkill_Info->s_pFTSkill->GetInfo()->s_dwID ) )
			{
				CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );
				assert_csm( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode ) == true, _T( "올바른 테이머 스킬코드가 아닙니다.\n 끄지 말고 김우봉씨를 찾아주세요!!" ) );

				if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
				{
					if( pTamerSkill->Get_TSkill_Info()->s_nAvailable == 1 )		// 1: 해당 지역에서 사용 불가능
					{
						if( nsCsFileTable::g_pSkillMng->FindArea_Check( pTamerSkill->Get_TSkill_Info()->s_nIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
						{
							cPrintMsg::PrintMsg( 30555, pSkill_Info->s_pFTSkill->GetInfo()->s_szName );
							return;
						}
					}
					else if( pTamerSkill->Get_TSkill_Info()->s_nAvailable == 2 )	// 2: 해당 지역에서 사용 가능
					{
						if( !nsCsFileTable::g_pSkillMng->FindArea_Check( pTamerSkill->Get_TSkill_Info()->s_nIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
						{
							cPrintMsg::PrintMsg( 30555, pSkill_Info->s_pFTSkill->GetInfo()->s_szName );
							return;
						}
					}
				}
			}

			CsC_AttackProp* pAttackProp = m_pParent->GetProp_Attack();
			pAttackProp->ReleaseAttackingTarget();
			pAttackProp->SetFollowTarget( pTarget, fRange, (CsC_AttackProp::eFOLLOW_ITERATOR)( CsC_AttackProp::FI_SKILL1 + nSkilIndex ), bRange );

			//assert_cs( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER );
			( (CDigimonUser*)m_pParent )->SetDigimonUserState( CDigimonUser::DU_SKILL );

			//assert_cs( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER );
			//// 사거리 측정
			//float fTargetDist = ( m_pParent->GetPos2D() - pTarget->GetPos2D() ).Length();
			//if( fTargetDist > fRange )
			//{
			//	// 사거리 벋어남
			//	return;
			//}
			//m_pParent->SendSkill( CsC_AttackProp::FI_SKILL1 + nSkilIndex, pTarget );
		}		
		break;
	default:
		assert_cs( false );
	}		
}

void cSkill::_Target_My_All( int nSkilIndex )	// Target 11
{
	sINFO* pSkill_Info = m_vpInfo[ nSkilIndex ];

	CsC_AvObject* pTarget = NULL;

	if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
	{
		if( pSkill_Info->s_pFTSkill->GetInfo()->s_nMemorySkill )	// 메모리(캐시) 스킬이라면..
		{
			// DS 체크
			int nNeedDS = pSkill_Info->s_pFTSkill->GetInfo()->s_nUseDS;
			if( nNeedDS && ( m_pParent->GetBaseStat()->GetDS() < nNeedDS ) )
			{
				// DS 부족
				cPrintMsg::PrintMsg( 13003 );
				if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
					cDigimonTalk::Print( 13003 );
				return;
			}

			// 스킬에 사용 가능한 메모리 칩 개수 파악
			int nCount = g_pDataMng->GetInven()->GetCount_Item_TypeL( nItem::DigimonSkillMemoryChip );
			if( pSkill_Info->s_pFTSkill->GetInfo()->s_nReq_Item > nCount )
			{
				cPrintMsg::PrintMsg( 30686 );	// 메모리 칩이 부족하여 스킬을 사용할 수 없습니다.
				return;
			}

			if( net::game )
			{
				int evoSlot = g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot;
				net::game->SendUseDigimonMemorySkill( m_pParent->GetUniqID(), (u1)evoSlot, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID, 0 );
			}
		}
	}
	else
	{
		CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );
		assert_csm( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode ) == true, _T( "올바른 테이머 스킬코드가 아닙니다.\n 끄지 말고 김우봉씨를 찾아주세요!!" ) );

		// 스킬 이름을 얻어온다
		TCHAR* pSkillName = pSkill_Info->s_pFTSkill->GetInfo()->s_szName;

		if( pTamerSkill->Get_TSkill_Info()->s_nUseState )	// 0 : 항상 사용, 1: 전투 중 사용 불가
		{
			if( g_pCharMng->GetTamerUser()->IsBattle() == true )	// 전투중에는 사용할 수 없다.
			{
				cPrintMsg::PrintMsg( 16002 );
				return;
			}
		}

		switch( pTamerSkill->Get_TSkill_Info()->s_nType )
		{
		case nTamer::BuffSkill:		// 버프 / 디버프 스킬
			if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
				break;

			if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
			{
				// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
				_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
			}
			else
			{	
				// 지역 설정이 없다면 바로 사용하자
				if(net::game)
					net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
			}
			break;

		case nTamer::ScriptSkill:		// 스크립트 스킬
			switch( pTamerSkill->Get_TSkill_Info()->s_dwFactor1 )
			{
			case 1:		// 아이템 사용 방식의 스킬
				if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
					break;

				if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
				{
					if( pTamerSkill->Get_TSkill_Info()->s_nAvailable == 1 )		// 가능/불가능( 1: 해당 지역에서 사용 불가능, 2: 해당 지역에서 사용 가능)
					{
						if( nsCsFileTable::g_pSkillMng->FindArea_Check( pTamerSkill->Get_TSkill_Info()->s_nIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
						{
							cPrintMsg::PrintMsg( 30555, pSkillName );
							break;
						}
						else
						{
							if( pTamerSkill->Get_TSkill_Info()->s_dwFactor2 == 9801 )
							{
								if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TAMER_SUMMON ) == true )	// 테이머 소환창이 이미 오픈되어 있다면 빠져나간다
									break;

								if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_JUMPBUSTER ) == false )
								{
									g_pGameIF->GetDynamicIF( cBaseWindow::WT_JUMPBUSTER, 0, 9801 );
									SetSkill_Index( pTamerSkill->Get_TSkill_Info()->s_nIndex );
								}
								break;
							}
	
							if(net::game)
								net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );
						}
					}
					else if( pTamerSkill->Get_TSkill_Info()->s_nAvailable == 2 )	// 가능/불가능( 1: 해당 지역에서 사용 불가능, 2: 해당 지역에서 사용 가능)
					{
						if( nsCsFileTable::g_pSkillMng->FindArea_Check( pTamerSkill->Get_TSkill_Info()->s_nIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
						{
							if( pTamerSkill->Get_TSkill_Info()->s_dwFactor2 == 9801 )
							{
								if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_JUMPBUSTER ) == false )
								{
									g_pGameIF->GetDynamicIF( cBaseWindow::WT_JUMPBUSTER, 0, 9801 );
									SetSkill_Index( pTamerSkill->Get_TSkill_Info()->s_nIndex );
								}
								break;
							}
							if(net::game)
								net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );
						}
						else
						{
							cPrintMsg::PrintMsg( 30555, pSkillName );
							break;
						}

					}
				}
				break;
			}
			break;
		default:
			assert_cs( false );
			break;
		}
	}
}

void cSkill::_Target_My_Tamer( int nSkilIndex )	// Target 12
{
	sINFO* pSkill_Info = m_vpInfo[ nSkilIndex ];

	CsC_AvObject* pTarget = NULL;
	CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );
	assert_csm( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode ) == true, _T( "올바른 테이머 스킬코드가 아닙니다.\n 끄지 말고 김우봉씨를 찾아주세요!!" ) );

	// 스킬 이름을 얻어온다
	TCHAR* pSkillName = pSkill_Info->s_pFTSkill->GetInfo()->s_szName;

	if( pTamerSkill->Get_TSkill_Info()->s_nUseState )	// 0 : 항상 사용, 1: 전투 중 사용 불가
	{
		if( g_pCharMng->GetTamerUser()->IsBattle() == true )	// 전투중에는 사용할 수 없다.
		{
			cPrintMsg::PrintMsg( 16002 );
			return;
		}
	}

	switch( pTamerSkill->Get_TSkill_Info()->s_nType )
	{
	case nTamer::BuffSkill:		// 버프 / 디버프 스킬
		if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
			break;

		if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
		{
			// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
			_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
		}
		else
		{	
			// 지역 설정이 없다면 바로 사용하자
			if(net::game)
				net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
		}
		break;

	case nTamer::ScriptSkill:		// 스크립트 스킬
		switch( pTamerSkill->Get_TSkill_Info()->s_dwFactor1 )
		{
		case 1:		// 아이템 사용 방식의 스킬 (휴대용 디지몬 보관소 소환 등...)
			if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
				break;

			if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
			{
				// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
				_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
			}
			else
			{	
				// 지역 설정이 없다면 바로 사용하자
				if(net::game)
					net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
			}
			break;
		}
		break;

	default:
		assert_cs( false );
		break;
	}

}

void cSkill::_Target_My_Partner_Digimon( int nSkilIndex )	// Target 13
{
	sINFO* pSkill_Info = m_vpInfo[ nSkilIndex ];

	if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
	{
		// DS 체크
		int nNeedDS = pSkill_Info->s_pFTSkill->GetInfo()->s_nUseDS;
		if( nNeedDS && ( m_pParent->GetBaseStat()->GetDS() < nNeedDS ) )
		{
			// DS 부족
			cPrintMsg::PrintMsg( 13003 );
			if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
				cDigimonTalk::Print( 13003 );
			return;
		}
		if( pSkill_Info->s_pFTSkill->GetInfo()->s_nMemorySkill )	// 메모리(캐시) 스킬이라면..
		{
			// 스킬에 사용 가능한 메모리 칩 개수 파악
			int nCount = g_pDataMng->GetInven()->GetCount_Item_TypeL( nItem::DigimonSkillMemoryChip );
			if( pSkill_Info->s_pFTSkill->GetInfo()->s_nReq_Item > nCount )
			{
				cPrintMsg::PrintMsg( 30686 );	// 메모리 칩이 부족하여 스킬을 사용할 수 없습니다.
				return;
			}

			if( net::game )
			{
				int evoSlot = g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot;
				net::game->SendUseDigimonMemorySkill( m_pParent->GetUniqID(), (u1)evoSlot, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID, 0 );
			}
		}
		else
		{
			if( ( (CDigimonUser*)m_pParent )->IsRide() == false )
			{
				CsC_AttackProp* pAttackProp = m_pParent->GetProp_Attack();
				pAttackProp->ReleaseAttackingTarget();
				//assert_csm1( pTarget->GetLeafRTTI() == RTTI_MONSTER, L"_Target_One_Enemy : 몬스터가 아님 : RTTI = %d", pTarget->GetLeafRTTI() );
				pAttackProp->SetFollowTarget( m_pParent, 0.0f, (CsC_AttackProp::eFOLLOW_ITERATOR)( CsC_AttackProp::FI_SKILL1 + nSkilIndex ), false );
				( (CDigimonUser*)m_pParent )->SetDigimonUserState( CDigimonUser::DU_SKILL );
			}
		}
	}
	else
	{
		CsC_AvObject* pTarget = NULL;
		CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );
		assert_csm( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode ) == true, _T( "올바른 테이머 스킬코드가 아닙니다.\n 끄지 말고 김우봉씨를 찾아주세요!!" ) );

		// 스킬 이름을 얻어온다
		TCHAR* pSkillName = pSkill_Info->s_pFTSkill->GetInfo()->s_szName;

		if( pTamerSkill->Get_TSkill_Info()->s_nUseState )	// 0 : 항상 사용, 1: 전투 중 사용 불가
		{
			if( g_pCharMng->GetTamerUser()->IsBattle() == true )	// 전투중에는 사용할 수 없다.
			{
				cPrintMsg::PrintMsg( 16002 );
				return;
			}
		}

		switch( pTamerSkill->Get_TSkill_Info()->s_nType )
		{
		case nTamer::BuffSkill:		// 버프 / 디버프 스킬
			if(net::game)
				net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );
			break;

		case nTamer::ScriptSkill:		// 스크립트 스킬
			switch( pTamerSkill->Get_TSkill_Info()->s_dwFactor1 )
			{
			case 1:		// 아이템 사용 방식의 스킬 ( 휴대용 디지몬 보관소 소환 등...)
				if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
					break;

				if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
				{
					// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
					_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
				}
				else
				{	
					// 지역 설정이 없다면 바로 사용하자
					if(net::game)
						net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
				}
				break;

			case 2:		// 평화 선언
				if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
					break;

				if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
				{
					if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
					{
						// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
						_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
					}
					else
					{	
						// 지역 설정이 없다면 바로 사용하자
						if(net::game)
							net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
					}
				}
				break;
			}
			break;

		default:
			assert_cs( false );
			break;
		}
	}
}

void cSkill::_Target_Another_All( int nSkilIndex )	// Target 55
{
	sINFO* pSkill_Info = m_vpInfo[ nSkilIndex ];
	assert_csm( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pSkill_Info->s_pFTSkill->GetInfo()->s_dwID ) == true, _T( "올바른 테이머 스킬코드가 아닙니다.\n 끄지 말고 김우봉씨를 찾아주세요!!" ) );

	CsC_AvObject* pTarget = NULL;
	CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );

	// 스킬 이름을 얻어온다
	TCHAR* pSkillName = pSkill_Info->s_pFTSkill->GetInfo()->s_szName;

	if( pTamerSkill->Get_TSkill_Info()->s_nUseState )	// 0 : 항상 사용, 1: 전투 중 사용 불가
	{
		if( g_pCharMng->GetTamerUser()->IsBattle() == true )	// 전투중에는 사용할 수 없다.
		{
			cPrintMsg::PrintMsg( 16002 );
			return;
		}
	}

	switch( pTamerSkill->Get_TSkill_Info()->s_nType )
	{
	case nTamer::BuffSkill:		// 버프 / 디버프 스킬
		if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
			break;

		if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
		{
			// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
			_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
		}
		else
		{	
			// 지역 설정이 없다면 바로 사용하자
			if(net::game)
				net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
		}
		break;

	case nTamer::ScriptSkill:		// 스크립트 스킬
		switch( pTamerSkill->Get_TSkill_Info()->s_dwFactor1 )
		{
		case 1:		// 아이템 사용 방식의 스킬 (시공간 폭탄, 휴대용 디지몬 보관소 소환 등...)
			if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
				break;

			if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
			{
				// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
				_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
			}
			else
			{	
				// 지역 설정이 없다면 바로 사용하자
				if(net::game)
					net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
			}
			break;

		case 3:		// 친구 소환
			if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
				break;

			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_JUMPBUSTER ) == true )	// 점프부스터 창이 오픈되어 있다면 빠져나간다
				break;

			if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
			{
				if( pTamerSkill->Get_TSkill_Info()->s_nAvailable == 1 )		// 가능/불가능( 1: 해당 지역에서 사용 불가능, 2: 해당 지역에서 사용 가능)
				{
					if( nsCsFileTable::g_pSkillMng->FindArea_Check( pTamerSkill->Get_TSkill_Info()->s_nIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
					{
						cPrintMsg::PrintMsg( 30555, pSkillName );
						break;
					}
					else
					{
						cCreateName::DeleteInstance();	// 이름 생성창이 오픈되어 있다면 제거
						g_pTamerSummon = (cTamerSummon*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_TAMER_SUMMON );
						g_pGameIF->GetTamerSummon()->SetShowWindow(true);
						SetSkill_Index( nSkilIndex );
					}
				}
				else if( pTamerSkill->Get_TSkill_Info()->s_nAvailable == 2 )	// 가능/불가능( 1: 해당 지역에서 사용 불가능, 2: 해당 지역에서 사용 가능)
				{
					if( nsCsFileTable::g_pSkillMng->FindArea_Check( pTamerSkill->Get_TSkill_Info()->s_nIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
					{
						cCreateName::DeleteInstance();	// 이름 생성창이 오픈되어 있다면 제거
						g_pTamerSummon = (cTamerSummon*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_TAMER_SUMMON );
						g_pGameIF->GetTamerSummon()->SetShowWindow(true);
						SetSkill_Index( nSkilIndex );
					}
					else
					{
						cPrintMsg::PrintMsg( 30555, pSkillName );
						break;
					}
				
				}
			}
			break;
		}
		break;

	default:
		assert_cs( false );
		break;
	}
}

void cSkill::SetTamerSkill_Ani( int nTamerSqID, int nDigimonSqID )
{
	CsC_AvObject* pTarget = g_pCharMng->GetTamerUser();

	if( nTamerSqID != 0 )
		pTarget->SetAnimation( nTamerSqID );

	if( nDigimonSqID != 0)
		g_pCharMng->GetDigimon( ( (CTamer*)pTarget )->GetDigimonLink() )->GetProp_Animation()->SetAnimation( nDigimonSqID );
}

int	cSkill::GetTamerSkill_Index( u4 nSkillIndex )
{
	CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( nSkillIndex, -1 );

	for( int i=0; i<m_vpInfo.Size(); ++i )
	{
		if( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode != m_vpInfo[i]->s_pFTSkill->GetInfo()->s_dwID )
			continue;

		return i;
	}

	return -1;
}

void cSkill::_Tamer_Skill_UseCheck( int nAvailable, int nSkillIndex, TCHAR* rSkillName )
{
	if( nAvailable == 1 )		// 1: 해당 지역에서 사용 불가능
	{
		if( nsCsFileTable::g_pSkillMng->FindArea_Check( nSkillIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
		{
			cPrintMsg::PrintMsg( 30555, rSkillName );
		}
		else
		{
			if(net::game)
				net::game->SendTamerActiveSkill( nSkillIndex );
		}
	}
	else if( nAvailable == 2 )	// 2: 해당 지역에서 사용 가능
	{
		if( nsCsFileTable::g_pSkillMng->FindArea_Check( nSkillIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
		{
			if(net::game)
				net::game->SendTamerActiveSkill( nSkillIndex );
		}
		else
		{
			cPrintMsg::PrintMsg( 30555, rSkillName );
		}

	}
}

void cSkill::_Target_My_Partner_Around( int nSkilIndex )
{
	CsC_AvObject* pTarget =	g_pCharResMng->GetTargetMark();
	if( pTarget == NULL )
	{
		cPrintMsg::PrintMsg( cPrintMsg::NO_SELECT_TARGET );
		return;
	}


	if( __IsAttackObject( pTarget ) == false )
		return;

	if( pTarget->IsServerDie() == true )
	{
		// 이미 죽은 몬스터임
		cPrintMsg::PrintMsg( 14009 );
		return;
	}

	if( m_pParent->GetProp_Animation()->IsEnableSkill() == false )
	{
		// 스킬을 사용 할수 없음
		return;
	}

	// 어택큐에 스킬이 들어 있는지 체크하자
	if( m_pParent->GetProp_Attack()->IsSkillProcess() == true )
	{
		cPrintMsg::PrintMsg( 13002 );

		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13002 );
		return;
	}

	// HP 체크
	int nNeedHP = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nUseHP;
	if( nNeedHP && ( m_pParent->GetBaseStat()->GetHP() < nNeedHP ) )
	{
		// HP 부족
		cPrintMsg::PrintMsg( 14007 );
		return;
	}
	// DS 체크
	int nNeedDS = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nUseDS;
	if( nNeedDS && ( m_pParent->GetBaseStat()->GetDS() < nNeedDS ) )
	{
		// DS 부족
		cPrintMsg::PrintMsg( 13003 );
		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13003 );
		return;
	}
	// LevelCheck
	if( m_pParent->GetBaseStat()->GetLevel() < m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nLimitLevel )
	{
		return;
	}

	if(  m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nMemorySkill )	// 메모리 스킬이라면...
	{
		// 스킬에 사용 가능한 메모리 칩 개수 파악
		int nCount = g_pDataMng->GetInven()->GetCount_Item_TypeL( nItem::DigimonSkillMemoryChip );
		if(  m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nReq_Item > nCount )
		{
			cPrintMsg::PrintMsg( 30686 );	// 메모리 칩이 부족하여 스킬을 사용할 수 없습니다.
			return;
		}
	}

	float fRange = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_fAttRange;
	bool bRange = true;
	if( m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nAttType == 1 )
	{
		fRange *= m_pParent->GetOrgGameScale();
		bRange = false;
	}


	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		switch( m_pParent->GetLeafRTTI() )
		{
		case RTTI_TUTORIAL_DIGIMON:
		case RTTI_DIGIMON_USER:
			{
				CTamerUser* pTamer = g_pCharMng->GetTamerUser();

				if( ( (CDigimonUser*)m_pParent )->IsRide() == false )
				{
					CsC_AttackProp* pAttackProp = m_pParent->GetProp_Attack();
					pAttackProp->ReleaseAttackingTarget();
					//assert_csm1( pTarget->GetLeafRTTI() == RTTI_MONSTER, L"_Target_One_Enemy : 몬스터가 아님 : RTTI = %d", pTarget->GetLeafRTTI() );
					pAttackProp->SetFollowTarget( pTarget, fRange, (CsC_AttackProp::eFOLLOW_ITERATOR)( CsC_AttackProp::FI_SKILL1 + nSkilIndex ), bRange );
					( (CDigimonUser*)m_pParent )->SetDigimonUserState( CDigimonUser::DU_SKILL );
					pTamer->ReleaseArriveTarget();
					pTamer->SetTargetPos( pTarget->GetPos(), CsMax( TAMERUSER_ATTACK_DIST, fRange ), false, false );
				}
				else
				{
					CTamerUser::sRIDE_TARGET_INFO rti;
					rti.s_nValue1 = CsC_AttackProp::FI_SKILL1 + nSkilIndex;
					rti.s_fValue1 = fRange;
					rti.s_bValue1 = bRange;

					pTamer->SetArriveTarget( pTarget );
					pTamer->SetRideEndState( CTamerUser::RES_SKILL, pTarget, &rti );
					pTamer->SetTargetPos( pTarget->GetPos(), CsMax( TAMERUSER_ATTACK_DIST, fRange ), false, false );				
				}
			}
			break;
		default:
			assert_cs( false );
		}
		break;
	case nLIB::SVR_BATTLE:
		{
			CsC_AttackProp* pAttackProp = m_pParent->GetProp_Attack();
			pAttackProp->ReleaseAttackingTarget();
			pAttackProp->SetFollowTarget( pTarget, fRange, (CsC_AttackProp::eFOLLOW_ITERATOR)( CsC_AttackProp::FI_SKILL1 + nSkilIndex ), bRange );

			assert_cs( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER );
			( (CDigimonUser*)m_pParent )->SetDigimonUserState( CDigimonUser::DU_SKILL );
		}		
		break;
	default:
		assert_cs( false );
	}		
}

void cSkill::_Target_One_Digimon_Party( int nSkilIndex )	// 자신의 파티에 속한 디지몬 한개체
{
	CsC_AvObject* pTarget = NULL;
	if( m_AdaptMainFrame )
		pTarget = m_AdaptMainFrame->GetTargetObject();

	if( pTarget == NULL )
	{
		cPrintMsg::PrintMsg( cPrintMsg::NO_SELECT_TARGET );
		return;
	}

	// 	// 타겟이 내 파티인지 검사..
	std::pair<int, bool> isMember;
	isMember.first = pTarget->GetUniqID();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_PARTYMEMBER_FROM_DIGIMONUIDX, &isMember );
	if( !isMember.second )
	{
		// 자신이 대상인지 검사
		if( pTarget->GetLeafRTTI() != RTTI_DIGIMON_USER && m_pParent->GetLeafRTTI() != RTTI_TUTORIAL_DIGIMON )
		{
			cPrintMsg::PrintMsg( 17004 );	// 해당 대상에게는 사용 할 수 없습니다.
			return;
		}
	}

	// 타겟과의 거리 체크
	NiPoint3 vPos = pTarget->GetPos() - m_pParent->GetPos();
	vPos.z = 0;
	float fDist = vPos.Length();

	if( m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_fAttRange < fDist )
	{
		cPrintMsg::PrintMsg( 17005 );	// 사용하실 수 있는 거리가 아닙니다.
		return;
	}

// 	// 현재 내 Sync에 있는지 확인
// 	if( !g_pMngCollector->GetObject( pTarget->GetUniqID() ) )
// 	{
// 		cPrintMsg::PrintMsg( 17005 );	// 사용하실 수 있는 거리가 아닙니다.
// 		return;
// 	}

	if( pTarget->IsServerDie() == true )
	{
		cPrintMsg::PrintMsg( 14009 );	// 타겟이 죽은 상태
		return;
	}

	if( m_pParent->GetProp_Animation()->IsEnableSkill() == false )
	{
		// 스킬을 사용 할수 없음
		return;
	}

	// DS 체크
	int nNeedDS = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nUseDS;
	if( nNeedDS && ( m_pParent->GetBaseStat()->GetDS() < nNeedDS ) )
	{
		// DS 부족
		cPrintMsg::PrintMsg( 13003 );
		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13003 );
		return;
	}

	// 어택큐에 스킬이 들어 있는지 체크하자
	if( m_pParent->GetProp_Attack()->IsSkillProcess() == true )
	{
		cPrintMsg::PrintMsg( 13002 );

		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13002 );
		return;
	}


	if(  m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nMemorySkill )	// 메모리 스킬이라면...
	{
		// 스킬에 사용 가능한 메모리 칩 개수 파악
		int nCount = g_pDataMng->GetInven()->GetCount_Item_TypeL( nItem::DigimonSkillMemoryChip );
		if(  m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nReq_Item > nCount )
		{
			cPrintMsg::PrintMsg( 30686 );	// 메모리 칩이 부족하여 스킬을 사용할 수 없습니다.
			return;
		}

		if( net::game )
			net::game->SendUseDigimonMemorySkill( m_pParent->GetUniqID(), (u1)g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot, 
							m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_dwID, pTarget->GetUniqID() );
	}	
	// 메모리 스킬이 아닐 경우??

}

void cSkill::_Target_All_Digimon_Party( int nSkilIndex )	// 자신의 파티에 속한 디지몬 전체
{
	if( m_pParent->GetProp_Animation()->IsEnableSkill() == false )
	{
		// 스킬을 사용 할수 없음
		return;
	}

	// DS 체크
	int nNeedDS = m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nUseDS;
	if( nNeedDS && ( m_pParent->GetBaseStat()->GetDS() < nNeedDS ) )
	{
		// DS 부족
		cPrintMsg::PrintMsg( 13003 );
		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13003 );
		return;
	}

	// 어택큐에 스킬이 들어 있는지 체크하자
	if( m_pParent->GetProp_Attack()->IsSkillProcess() == true )
	{
		cPrintMsg::PrintMsg( 13002 );

		if( m_pParent->GetLeafRTTI() == RTTI_DIGIMON_USER || m_pParent->GetLeafRTTI() == RTTI_TUTORIAL_DIGIMON )
			cDigimonTalk::Print( 13002 );
		return;
	}

	if(  m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nMemorySkill )	// 메모리 스킬이라면...
	{
		// 스킬에 사용 가능한 메모리 칩 개수 파악
		int nCount = g_pDataMng->GetInven()->GetCount_Item_TypeL( nItem::DigimonSkillMemoryChip );

		if(  m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_nReq_Item > nCount )
		{
			cPrintMsg::PrintMsg( 30686 );	// 메모리 칩이 부족하여 스킬을 사용할 수 없습니다.
			return;
		}

		if( net::game )
			net::game->SendUseDigimonMemorySkill( m_pParent->GetUniqID(), (u1)g_pCharMng->GetDigimonUser( 0 )->GetFTEvolCurObj()->m_nEvoSlot, 
							m_vpInfo[ nSkilIndex ]->s_pFTSkill->GetInfo()->s_dwID, 0 );
	}
	else
	{
		sINFO* pSkill_Info = m_vpInfo[ nSkilIndex ];
		CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );
		assert_csm( nsCsFileTable::g_pSkillMng->IsTamer_Skill( pTamerSkill->Get_TSkill_Info()->s_dwSkillCode ) == true, _T( "올바른 테이머 스킬코드가 아닙니다.\n 끄지 말고 김우봉씨를 찾아주세요!!" ) );

		// 스킬 이름을 얻어온다
		TCHAR* pSkillName = pSkill_Info->s_pFTSkill->GetInfo()->s_szName;

		if( pTamerSkill->Get_TSkill_Info()->s_nUseState )	// 0 : 항상 사용, 1: 전투 중 사용 불가
		{
			if( g_pCharMng->GetTamerUser()->IsBattle() == true )	// 전투중에는 사용할 수 없다.
			{
				cPrintMsg::PrintMsg( 16002 );
				return;
			}
		}

		switch( pTamerSkill->Get_TSkill_Info()->s_nType )
		{
		case nTamer::BuffSkill:		// 버프 / 디버프 스킬
			if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
				break;

			if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
			{
				// 함수 내부가 변경이 필요할 경우 내용 복사하여 이곳에서 수정 사용.
				_Tamer_Skill_UseCheck( pTamerSkill->Get_TSkill_Info()->s_nAvailable, pTamerSkill->Get_TSkill_Info()->s_nIndex, pSkillName );
			}
			else
			{	
				// 지역 설정이 없다면 바로 사용하자
				if(net::game)
					net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );				
			}
			break;

		case nTamer::ScriptSkill:		// 스크립트 스킬
			switch( pTamerSkill->Get_TSkill_Info()->s_dwFactor1 )
			{
			case 1:		// 아이템 사용 방식의 스킬
				if( g_pGameIF->IsOpenNpcWindow( true ) == true )	// 이미 NPC 관련 창이 open 되어 있다면
					break;

				if( pTamerSkill->Get_TSkill_Info()->s_nUse_Are_Check == 1 )		// 지역 설정 사용 여부( 1: 사용, 0: 미사용)
				{
					if( pTamerSkill->Get_TSkill_Info()->s_nAvailable == 1 )		// 가능/불가능( 1: 해당 지역에서 사용 불가능, 2: 해당 지역에서 사용 가능)
					{
						if( nsCsFileTable::g_pSkillMng->FindArea_Check( pTamerSkill->Get_TSkill_Info()->s_nIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
						{
							cPrintMsg::PrintMsg( 30555, pSkillName );
							break;
						}
						else
						{
							if( pTamerSkill->Get_TSkill_Info()->s_dwFactor2 == 9801 )
							{
								if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TAMER_SUMMON ) == true )	// 테이머 소환창이 이미 오픈되어 있다면 빠져나간다
									break;

								if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_JUMPBUSTER ) == false )
								{
									g_pGameIF->GetDynamicIF( cBaseWindow::WT_JUMPBUSTER, 0, 9801 );
									SetSkill_Index( pTamerSkill->Get_TSkill_Info()->s_nIndex );
								}
								break;
							}

							if(net::game)
								net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );
						}
					}
					else if( pTamerSkill->Get_TSkill_Info()->s_nAvailable == 2 )	// 가능/불가능( 1: 해당 지역에서 사용 불가능, 2: 해당 지역에서 사용 가능)
					{
						if( nsCsFileTable::g_pSkillMng->FindArea_Check( pTamerSkill->Get_TSkill_Info()->s_nIndex, (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID) )
						{
							if( pTamerSkill->Get_TSkill_Info()->s_dwFactor2 == 9801 )
							{
								if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_JUMPBUSTER ) == false )
								{
									g_pGameIF->GetDynamicIF( cBaseWindow::WT_JUMPBUSTER, 0, 9801 );
									SetSkill_Index( pTamerSkill->Get_TSkill_Info()->s_nIndex );
								}
								break;
							}

							if(net::game)
								net::game->SendTamerActiveSkill( pTamerSkill->Get_TSkill_Info()->s_nIndex );
						}
						else
						{
							cPrintMsg::PrintMsg( 30555, pSkillName );
							break;
						}

					}
				}
				break;
			}
			break;
		default:
			assert_cs( false );
			break;
		}
	}

	// 메모리 스킬이 아닐 경우??
}
//==================================================================================================
//
//	정의된 스킬
//
//==================================================================================================

void cSkill::__Wait_Digimon( sINFO* pSkillInfo )
{
	CDigimonUser* pDigimonUser;
	for( int i=0; i<MAX_DIGIMONUSER_COUNT; ++i )	
	{
		pDigimonUser = g_pCharMng->GetDigimonUser( i );
		if( pDigimonUser )
		{
			if( pDigimonUser->CheckTamerDist() == false )
				pDigimonUser->SetDigimonUserState( CDigimonUser::DU_FOLLOW );
			else
				pDigimonUser->SetDigimonUserState( CDigimonUser::DU_WAIT );
		}
	}

	pSkillInfo->s_CoolTime.Start();
}

void cSkill::__Follow_Digimon( sINFO* pSkillInfo )
{
	CDigimonUser* pDigimonUser;
	for( int i=0; i<MAX_DIGIMONUSER_COUNT; ++i )	
	{
		pDigimonUser = g_pCharMng->GetDigimonUser( i );
		if( pDigimonUser )
		{
			pDigimonUser->GetMoveCoolTimeSeq()->Reset();
			pDigimonUser->SetDigimonUserState( CDigimonUser::DU_FOLLOW );
			pDigimonUser->GetMoveCoolTimeSeq()->Reset();
		}
	}

	pSkillInfo->s_CoolTime.Start();
}

void cSkill::__Attack_Digimon( sINFO* pSkillInfo )
{
	CsC_AvObject* pTarget =	g_pCharResMng->GetTargetMark();
	if( pTarget == NULL )
	{
		cPrintMsg::PrintMsg( cPrintMsg::NO_SELECT_TARGET );
		// 타겟이 지정되지 않음
		return;
	}

	// 타겟 선택
	if( __IsAttackObject( pTarget ) == false )
		return;

	// 공격할 타겟이 있다면
	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
		g_pCharMng->GetTamerUser()->SetAttackPos( pTarget );
		break;
	case nLIB::SVR_BATTLE:
		g_pCharMng->GetDigimonUser( 0 )->SetDigimonUserState( CDigimonUser::DU_ATTACK, pTarget );
		//{
		//	CDigimonUser* pDigimon = (CDigimonUser*)g_pCharMng->GetDigimonUser( 0 );
		//	float fTargetDist = ( pDigimon->GetPos2D() - pTarget->GetPos2D() ).Length();
		//	// 사거리 체크
		//	float fEnableAttackDist = pDigimon->GetAttackRange() + pTarget->GetAttackCharSize();
		//	if( fTargetDist > fEnableAttackDist )
		//	{
		//		// 사거리 벋어남
		//		return;
		//	}
		//	
		//	if( net::game )
		//	{
		//		net::game->SendAttack( pDigimon->GetUniqID(), pTarget->GetUniqID() );
		//	}			
		//}		
		break;
	default:
		assert_cs( false );
	}

	// 쿨타임
	pSkillInfo->s_CoolTime.Start();
}

void cSkill::__Pickup_Digitama( sINFO* pSkillInfo )
{
	//assert_cs( m_pParent->GetLeafRTTI() == RTTI_TAMER_USER );

	if( ( (CTamerUser*)m_pParent )->IsRide() )
	{
		cPrintMsg::PrintMsg( 30014 );
		return;
	}

	// 주변 15미터 이내의 내 소유 아이템을 체킹
	CItem* pItem = g_pItemMng->PickupItem( m_pParent->GetUniqID(), m_pParent->GetPos2D(), 1500.0f );
	if( pItem == NULL )
		return;

	if( pItem == ( (CTamerUser*)m_pParent )->GetArriveTargetItem() )
		return;

	( (CTamerUser*)m_pParent )->SetArriveTarget( pItem );
	( (CTamerUser*)m_pParent )->SetTargetPos( pItem->GetPos(), 0.0f, false, false );

	pSkillInfo->s_CoolTime.Start();
}

// void cSkill::__BattleTag_Digimon1( sINFO* pSkillInfo )
// {
// 	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );	
// 	if(pDUser->IsJointProgressing() || pDUser->IsCurJointProgress())
// 	{
// 		cPrintMsg::PrintMsg( 30511 );
// 		return;
// 	}
// #ifdef SDM_TAMER_XGUAGE_20180628
// 	if(pDUser->GetFTDigimon()->isX_AntiJogress())
// 	{
// 		cPrintMsg::PrintMsg( 30511 );
// 		return;
// 	}
// #endif
// 
// 	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
// 	if( pTamer )
// 	{
// 		cCondition* pCondition = pTamer->GetCondition();
// 		if( pCondition )
// 		{
// 			if( pCondition->IsCondition( nSync::Shop ) || pCondition->IsCondition( nSync::ShopReady ) )
// 			{
// 				cPrintMsg::PrintMsg( 30058 );
// 				return;
// 			}
// 		}
// 	}
// 
// 	if( g_pDataMng->GetTactics()->IsTactics( 0 ) == false )
// 	{
// 		cPrintMsg::PrintMsg( 13007 );
// 		return;
// 	}
// #ifdef CROSSWARS_SYSTEM
// 	// 현재 디지몬이 크로스 용병이면 일반 채인지 불가
// 	DWORD dwMyDigimon = pDUser->GetBaseDigimonFTID();
// 	if(nsCsFileTable::g_pDigimonMng->IsCrossDigimon(dwMyDigimon))
// 	{
// 		return;
// 	}
// #endif
// 	if( g_pDataMng->GetServerSync()->IsChageDigimon() == true )
// 		return;
// 
// 	if( cData_Tactics::Check_EnableChange() == true )
// 	{		
// 		int nCPIndex = 1;
// 		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CHANGE_PARTNER, 0, &nCPIndex );
// 	}	
// 
// 	pSkillInfo->s_CoolTime.Start();
// }
// 
// void cSkill::__BattleTag_Digimon2( sINFO* pSkillInfo )
// {
// 	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );	
// 	if(pDUser->IsJointProgressing() || pDUser->IsCurJointProgress())
// 	{
// 		cPrintMsg::PrintMsg( 30511 );
// 		return;
// 	}
// #ifdef SDM_TAMER_XGUAGE_20180628
// 	if(pDUser->GetFTDigimon()->isX_AntiJogress())
// 	{
// 		cPrintMsg::PrintMsg( 30511 );
// 		return;
// 	}
// #endif
// 
// 	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
// 	if( pTamer )
// 	{
// 		cCondition* pCondition = pTamer->GetCondition();
// 		if( pCondition )
// 		{
// 			if( pCondition->IsCondition( nSync::Shop ) || pCondition->IsCondition( nSync::ShopReady ) )
// 			{
// 				cPrintMsg::PrintMsg( 30058 );
// 				return;
// 			}
// 		}
// 	}
// 	if( g_pDataMng->GetTactics()->IsTactics( 1 ) == false )
// 	{
// 		cPrintMsg::PrintMsg( 13007 );
// 		return;
// 	}
// #ifdef CROSSWARS_SYSTEM
// 	// 현재 디지몬이 크로스 용병이면 일반 채인지 불가
// 	DWORD dwMyDigimon = pDUser->GetBaseDigimonFTID();
// 	if(nsCsFileTable::g_pDigimonMng->IsCrossDigimon(dwMyDigimon))
// 	{
// 		return;
// 	}
// 
// #endif
// 	if( g_pDataMng->GetServerSync()->IsChageDigimon() == true )
// 		return;
// 
// 	if( cData_Tactics::Check_EnableChange() == true )
// 	{
// 		int nCPIndex = 2;
// 		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CHANGE_PARTNER, 0, &nCPIndex );
// 	}
// 
// 	pSkillInfo->s_CoolTime.Start();
// }
// 
// void cSkill::__BattleTag_Digimon3( sINFO* pSkillInfo )
// {
// 	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );	
// 	if(pDUser->IsJointProgressing() || pDUser->IsCurJointProgress())
// 	{
// 		cPrintMsg::PrintMsg( 30511 );
// 		return;
// 	}
// #ifdef SDM_TAMER_XGUAGE_20180628
// 	if(pDUser->GetFTDigimon()->isX_AntiJogress())
// 	{
// 		cPrintMsg::PrintMsg( 30511 );
// 		return;
// 	}
// #endif
// 	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
// 	if( pTamer )
// 	{
// 		cCondition* pCondition = pTamer->GetCondition();
// 		if( pCondition )
// 		{
// 			if( pCondition->IsCondition( nSync::Shop ) || pCondition->IsCondition( nSync::ShopReady ) )
// 			{
// 				cPrintMsg::PrintMsg( 30058 );
// 				return;
// 			}
// 		}
// 	}
// 
// 	if( g_pDataMng->GetTactics()->IsTactics( 2 ) == false )
// 	{
// 		cPrintMsg::PrintMsg( 13007 );
// 		return;
// 	}
// #ifdef CROSSWARS_SYSTEM
// 	// 현재 디지몬이 크로스 용병이면 일반 채인지 불가
// 	DWORD dwMyDigimon = pDUser->GetBaseDigimonFTID();
// 	if(nsCsFileTable::g_pDigimonMng->IsCrossDigimon(dwMyDigimon))
// 	{
// 		return;
// 	}
// 
// #endif
// 	if( g_pDataMng->GetServerSync()->IsChageDigimon() == true )
// 		return;
// 
// 	if( cData_Tactics::Check_EnableChange() == true )
// 	{
// 		int nCPIndex = 3;
// 		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CHANGE_PARTNER, 0, &nCPIndex );
// 	}
// 
// 	pSkillInfo->s_CoolTime.Start();
// }

void cSkill::__BattleTag_Digimon(sINFO* pSkillInfo)
{
	SAFE_POINTER_RET( pSkillInfo );
	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pDUser );
	if(pDUser->IsJointProgressing() || pDUser->IsCurJointProgress())
	{
		cPrintMsg::PrintMsg( 30511 );
		return;
	}
#ifdef SDM_TAMER_XGUAGE_20180628
	CsDigimon* pFTDigimon = pDUser->GetFTDigimon(); 
	if(pFTDigimon && pFTDigimon->isX_AntiJogress())
	{
		cPrintMsg::PrintMsg( 30511 );
		return;
	}
#endif
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	if( pTamer )
	{
		cCondition* pCondition = pTamer->GetCondition();
		if( pCondition )
		{
			if( pCondition->IsCondition( nSync::Shop ) || pCondition->IsCondition( nSync::ShopReady ) )
			{
				cPrintMsg::PrintMsg( 30058 );
				return;
			}
		}
	}

	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	if( pTactics && pTactics->IsTactics( pSkillInfo->s_nTacticsIndex ) == false )
	{
		cPrintMsg::PrintMsg( 13007 );
		return;
	}
#ifdef CROSSWARS_SYSTEM
	// 현재 디지몬이 크로스 용병이면 일반 채인지 불가
	DWORD dwMyDigimon = pDUser->GetBaseDigimonFTID();
	if(nsCsFileTable::g_pDigimonMng->IsCrossDigimon(dwMyDigimon))
	{
		return;
	}

#endif
	cData_ServerSync* pSrvSync = g_pDataMng->GetServerSync();
	if( pSrvSync && pSrvSync->IsChageDigimon() == true )
		return;

	if( cData_Tactics::Check_EnableChange() == true )
	{
		int nCPIndex = pSkillInfo->s_nTacticsIndex + 1;
		if( pSrvSync )
			pSrvSync->SendServer( cData_ServerSync::CHANGE_PARTNER, 0, &nCPIndex );
	}

	pSkillInfo->s_CoolTime.Start();
}

void cSkill::__Away( sINFO* pSkillInfo )
{
	assert_cs( m_pParent->IsKindOf( RTTI_TAMER ) );

	CTamer* pTamer = (CTamer*)m_pParent;
	if( pTamer->IsServerDie() == true )
	{		
		cPrintMsg::PrintMsg( 20050 );
		return;
	}
	if( pTamer->IsBattle() == true )
	{
		cPrintMsg::PrintMsg( 16002 );
		return;
	}

	// 이동중일때는 사용 불가
	if( pTamer->GetProp_Path()->IsMove() )
	{
		cPrintMsg::PrintMsg( 20068 );
		return;
	}
	if( g_pCharMng->GetDigimonUser( 0 )->GetProp_Path()->IsMove() )
	{
		cPrintMsg::PrintMsg( 20068 );
		return;
	}
	if( pTamer->IsPause( CsC_AvObject::PAUSE_PATH ) == true )
	{
		return;
	}

	pTamer->DeletePath();
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();

	if( net::game )
		net::game->SendAwayTime();

	pSkillInfo->s_CoolTime.Start();
}

//==================================================================================================
//
//	
//
//==================================================================================================

CsSkill* cSkill::GetFTSkill( uint nFTDigimonID, int nSkillIndex )
{
	CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( nFTDigimonID )->GetInfo();
	DWORD dwSkillID = pFTDigimon->s_Skill[ nSkillIndex ].s_dwID;
	if( nsCsFileTable::g_pSkillMng->IsSkill( dwSkillID ) == true )
	{
		return nsCsFileTable::g_pSkillMng->GetSkill( dwSkillID );
	}
	return NULL;
}