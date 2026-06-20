#include "StdAfx.h"
#include "cNetwork.h"
#include "cCliGame.h"
#include "common_vs2019/pGame.h"
#include "common_vs2019/pItem.h"
#include "common_vs2019/pServer.h"
#include "common_vs2019/pSession.h"
#include "common_vs2019/pTrade.h"
#include "common_vs2019/pScanner.h"
#include "common_vs2019/pSkill.h"
#include "common_vs2019/pMaster.h"
#include "common_vs2019/pTamer.h"
#include "common_vs2019/pPass2.h"
#include "common_vs2019/pBuddy.h"
#include "common_vs2019/pParty.h"
#include "common_vs2019/pGuild.h"
#include "common_vs2019/pEvent.h"
#include "common_vs2019/pDigimon.h"
#include "common_vs2019/pCashShop.h"
#include "common_vs2019/pManager.h"
#include "common_vs2019/pBuff.h"

void cCliGame::RecvBuffCreate()
{
	u4 nUID = 0;			// 버프 적용 대상의 UID
	u2 nBuffCode =0;		// 버프 코드
	u2 nBuffClassLevel = 0;	// 버프 레벨
	u4 nEndTS = 0;			// 종료 시간
	u4 nHitterUID = 0;		// 나에게 버프를 건 대상 ( 자신일 수도 있음 )
	u4 dwSkillCode = 0;		// 같은 버프코드를 사용하는 버프가 있어 구분을 위해 스킬코드 저장.

	pop(nUID);	
	pop(nBuffCode);
	pop(nBuffClassLevel);
	pop(nEndTS);
	pop( dwSkillCode );

	CsC_AvObject* pObject = g_pMngCollector->GetObject( nUID );	
	if( pObject == NULL )
		return;	

	if( (nEndTS - _TIME_TS) <= 0.f )
		return;

	switch( pObject->GetLeafRTTI() )
	{
// 버프 동기화 안하기 때문에 자신이 아닌 UID일 경우 무시
// GSP 타겟 버프 비정상 출력 버그 관련 chu8820 14.04.03
// - 버프 싱크 적용 05.27 패치
 	case RTTI_TAMER:
	case RTTI_TAMER_USER:
		( (CTamer*)pObject )->GetBuff()->SetBuff( nBuffCode, nHitterUID, nEndTS, nBuffClassLevel, dwSkillCode );
		break;
 	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
		( (CDigimon*)pObject )->GetBuff()->SetBuff( nBuffCode, nHitterUID, nEndTS, nBuffClassLevel, dwSkillCode );
		break;
	case RTTI_MONSTER:
		( (CMonster*)pObject )->GetBuff()->SetBuff( nBuffCode, nHitterUID, nEndTS, nBuffClassLevel, dwSkillCode );
		break;
	}

	TCHAR sz[333];
	_stprintf_s( sz, 333, L"%d %s 등록", nUID, nsCsFileTable::g_pBuffMng->GetBuff( nBuffCode )->GetInfo()->s_szName );
	
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = sz;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cCliGame::RecvBuffChange()
{
	u4 nUID = 0;			// 버프 적용 대상의 UID
	u2 nBeforeCode = 0;		// 이전 적용되어 있던 버프 코드
	u2 nBuffCode =0;		// 버프 코드
	u2 nBuffClassLevel = 0;	// 버프 레벨
	u4 nEndTS = 0;			// 종료 시간
	u4 nHitterUID = 0;		// 나에게 버프를 건 대상 ( 자신일 수도 있음 )
	u4 dwSkillCode = 0;		// 같은 버프코드를 사용하는 버프가 있어 구분을 위해 스킬코드 저장.

	pop(nUID);
	pop(nBeforeCode);
	pop(nBuffCode);
	pop(nBuffClassLevel);
	pop(nEndTS);
	pop( dwSkillCode );

	CsC_AvObject* pObject = g_pMngCollector->GetObject( nUID );	
	if( pObject == NULL )
		return;

	switch( pObject->GetLeafRTTI() )
	{
// 버프 동기화 안하기 때문에 자신이 아닌 UID일 경우 무시
// GSP 타겟 버프 비정상 출력 버그 관련 chu8820 14.04.03
// - 버프 싱크 적용 05.27 패치
 	case RTTI_TAMER:
	case RTTI_TAMER_USER:
		( (CTamer*)pObject )->GetBuff()->ReleaseBuff( nBeforeCode );
		( (CTamer*)pObject )->GetBuff()->SetBuff( nBuffCode, nHitterUID, nEndTS, nBuffClassLevel, dwSkillCode );		
		break;
 	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
		( (CDigimon*)pObject )->GetBuff()->ReleaseBuff( nBeforeCode );
		( (CDigimon*)pObject )->GetBuff()->SetBuff( nBuffCode, nHitterUID, nEndTS, nBuffClassLevel, dwSkillCode );		
		break;
	case RTTI_MONSTER:
		( (CMonster*)pObject )->GetBuff()->ReleaseBuff( nBeforeCode );
		( (CMonster*)pObject )->GetBuff()->SetBuff( nBuffCode, nHitterUID, nEndTS, nBuffClassLevel, dwSkillCode );
		break;
	}
}

void cCliGame::RecvBuffClear()
{
	u4 nUID = 0;
	u2 nCount = 0;		// 처리할 개수
	u2 nBuffCode = 0;	// 만료된 버프 코드

	pop(nUID);	

	CsC_AvObject* pObject = g_pMngCollector->GetObject( nUID );	
	if( pObject == NULL )
		return;

	pop(nCount);

	switch( pObject->GetLeafRTTI() )
	{
	case RTTI_TAMER:
	case RTTI_TAMER_USER:
		{			
			for( int i = 0; i < nCount; i++ ) 
			{
				pop(nBuffCode);
				( (CTamer*)pObject )->GetBuff()->ReleaseBuff( nBuffCode );
			}
		}
		break;
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
		{			
			for( int i = 0; i < nCount; i++ )
			{
				pop(nBuffCode);
				( (CDigimon*)pObject )->GetBuff()->ReleaseBuff( nBuffCode );
			}
		}		
		break;
	case RTTI_MONSTER:
		{			
			for( int i = 0; i < nCount; i++ )
			{
				if( pObject->GetAniPause() )
					pObject->SetAniPause( false );
				pop(nBuffCode);
				( (CMonster*)pObject )->GetBuff()->ReleaseBuff( nBuffCode );
			}
		}		
		break;
	}	

	TCHAR sz[512];
	_stprintf_s( sz, 512, L"%d %s 해제", nUID, nsCsFileTable::g_pBuffMng->GetBuff( nBuffCode )->GetInfo()->s_szName );
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = sz;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );	
}

// 디지몬 전용 버프 스킬 , 버프 싱크 시 Around로 보냄
void cCliGame::RecvBuffSkill()
{
	u4 nTamerUID = 0;		// 스킬 버프 대상 테이머 - 버프 시전자
	u4 nDigimonUID = 0;		// 디지몬 UID - pop 안함
	u2 nBuffCode = 0;		// 버프 코드
	u1 nBuffClassLevel = 0;	// 버프 레벨(스킬 레벨)
	u4 nEndTS = 0;			// 종료 시간
	u4 nSkillCode = 0;	// 스킬 코드


	pop(nTamerUID);
	//pop(nDigimonUID);		// 스킬 버프 대상 디지몬 - 필요 없음, 테이머UID만 알면 GetDigimonLink로 파트너몬 UID를 알 수 있음.
	pop(nBuffCode);
	pop(nBuffClassLevel);
	pop(nEndTS);
	pop(nSkillCode);		// 스킬 코드	- 필요 없음, 오직 디지몬 버프류 스킬만 Recv함
	


	// 테이머 찾기
	CTamer* pTamer = dynamic_cast<CTamer*>(g_pMngCollector->GetObject( nTamerUID ));
	if( pTamer == NULL )
		return;

	// 테이머의 파트너몬 인덱스
	nDigimonUID = pTamer->GetDigimonLink();
	
	// 디지몬 찾기
	CDigimon* pDigimon = g_pCharMng->GetDigimon( nDigimonUID );
	if( pDigimon == NULL )
	{
		assert_csm(pDigimon, "디지몬을 찾지 못했습니다.");
		return;	
	}

	// 디지몬 UID 저장
	nDigimonUID = pDigimon->GetUniqID();

	//* ----- 버프 스킬 등록 -> 이미 등록된 버프가 있다면 삭제하고 다시 등록합니다. ----- *//
	switch( pDigimon->GetLeafRTTI() )
	{
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
		{
			cBuffData* pDigimonBuff = pDigimon->GetBuff();
			pDigimonBuff->ReleaseBuff( nBuffCode );
			pDigimonBuff->SetOnePlayEffect(true);
			pDigimonBuff->SetBuff( nBuffCode, nDigimonUID, nEndTS, nBuffClassLevel, nSkillCode );
			pDigimonBuff->SetOnePlayEffect(false);
		}
		break;
	}
}

void cCliGame::SendManualRemove(u1 nType,u2 nBuffIDX)
{	// nType 1: 테이머 2: 디지몬	
	newp( pBuff::ManualRemove );
		push( nType );
		push( nBuffIDX );
	endp( pBuff::ManualRemove );
	send();
}

void cCliGame::RecvBuffApplyFail()
{	
	u1 nType = 0;
	n2 nResult = 0;
	u4 nSlot = 0;

	pop( nType );
	pop( nResult );

	switch( nType )
	{ 
	case nBuff::ITEM_BUFF_FAIL:
		{
			pop(nSlot); // 아이템 슬롯 번호
			g_pDataMng->ServerItemUseFailed( nSlot );
		}		
		break;
	case nBuff::SKILL_BUFF_FAIL:
		cPrintMsg::PrintMsg( 30452 );
		break;
	case nBuff::MANUL_REMOVE_FAIL :
		break;
	}
	
	switch( nResult )
	{
	case nBuff::APPLY_FAIL_CODE_ALREADY_HIGH_LEVEL	:			// 상위 레벨 버프가 적용되어 있습니다.
		cPrintMsg::PrintMsg( 30452 );
		break;
	case nBuff::APPLY_FAIL_CODE_TARGET_LEVEL_ISLOW	:			// 적용 대상의 레벨이 버프 적용 레벨보다 낮습니다.
		break;
	case nBuff::APPLY_FAIL_CODE_NOT_MANUAL_REMOVE_BUFF :		// 수동으로 제거 할 수 없는 버프 제거 요청		
	case nBuff::APPLY_FAIL_CODE_NOT_MANUAL_NOT_FOUND :	// 수동 제거 요청한 버프를 찾을 수 없음(이미 만료 되었을수도 있음)
		cPrintMsg::PrintMsg( 30454 );
		break;	
	}	
}
#ifdef MONSTER_SKILL_GROWTH
void cCliGame::RecvMonBuffSet()
{
	u4 nMonIDX = 0;			//	성장 스킬 사용중인 몬스터 UID
	u4 nMonSkill_Idx = 0;		// 몬스터스킬 인덱스
	u4 nHitterUID = 0;		// 나에게 버프를 건 대상 ( 자신일 수도 있음 )
	u4 nEndTS = UINT_MAX;			// 종료 시간
	u2 nBuffClassLevel = 0;	// 버프 레벨
	u4 nSkillCode = 0;		// 버프 스킬 코드
	u2 nEffectVal = 0;			// 현재 버프 효과 적용치
	int nStack = 1;	// 중첩수

	// 버프 대상 오브젝트 선택
 	pop( nMonIDX );
	CsC_AvObject* pObject = g_pMngCollector->GetObject( nMonIDX );	
	if( pObject == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T( "존재하지 않는 몬스터 \n UID : %d" ), nMonIDX  );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T( "존재하지 않는 몬스터 \n UID : %d" ), nMonIDX);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}
	CMonster*	pMonster = (CMonster*)pObject;

	// 몬스터스킬 인덱스
	pop( nMonSkill_Idx );
	pop( nStack );

	// 해당 스킬 정보
	CsMonsterSkill::sINFO* pSkillInfo =  nsCsFileTable::g_pMonsterMng->GetMonsterSkill( nMonSkill_Idx );

	if( pSkillInfo == NULL || nStack == 0 )	// 몬스터 스킬이 아니거나 중첩이 0인 경우
		return;

	switch( pSkillInfo->s_nSkillType )	// 스킬 효과별로 체크
	{
	case CsMonsterSkill::GROWTH:	// 몬스터 성장 스킬
		{
			if( nStack > (int)( pSkillInfo->s_dwEff_Val_Max ) )	// 이미 최대 중첩이야
				break; 

			int nBuffCode = nsBuff::BK_MONSTER_GROWTH;	// 성장 버프코드

			//해당 버프 없으면 새로 셋팅
			if( pMonster->GetBuff()->IsBuffData( nBuffCode ) == false )
				pMonster->GetBuff()->SetBuff( nBuffCode, nHitterUID, nEndTS, nBuffClassLevel/*, nSkillCode*/ );
			else	// 이미 버프 받고 있는놈이면 중첩 증가
				pMonster->GetBuff()->GetBuffData( nBuffCode )->m_nStack = nStack;

			//크기 키우기
			float fScale = nsCsFileTable::g_pMonsterMng->GetEffectVal_Scale( nMonSkill_Idx );

			if( fScale == 0.0f )
			{
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
				//CProtocol.m_wStr = GetVAString( _T( "스케일 증가값 0\n\nSkill_Idx : %d" ), nMonSkill_Idx );
				DmCS::StringFn::Format(CProtocol.m_wStr, _T( "스케일 증가값 0\n\nSkill_Idx : %d" ), nMonSkill_Idx);
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
				break;
			}
			else
			{
				fScale = ( fScale * (float)nStack ) + 1.0f;	// 크기 증가값 * 스택 수 + 1.0f 
				pMonster->SetScale( fScale );
			}

			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString( _T( "%s 스킬 ( %d )중첩 스케일 : %.2f" ), nsCsFileTable::g_pBuffMng->GetBuff( nBuffCode )->GetInfo()->s_szName, nStack, fScale );
			DmCS::StringFn::Format(CProtocol.m_wStr, _T( "%s 스킬 ( %d )중첩 스케일 : %.2f" ), nsCsFileTable::g_pBuffMng->GetBuff( nBuffCode )->GetInfo()->s_szName, nStack, fScale);
			
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}
		break;
	case CsMonsterSkill::SUMMON_MONSTER:		// 몬스터 소환
	case CsMonsterSkill::HP_VAL_DECREASE:			// HP x 감소
		break;		//버프 없는 스킬들이니 여기 올 일은 없음
	default:
		{
			wstring str;
			//str = GetVAString( _T( "알수 없는 효과(Effect)타입\n\n Idx : %d \n Effect : %d" ), nMonSkill_Idx, pSkillInfo->s_nSkillType );
			DmCS::StringFn::Format(str, _T( "알수 없는 효과(Effect)타입\n\n Idx : %d \n Effect : %d" ), nMonSkill_Idx, pSkillInfo->s_nSkillType);

			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			CProtocol.m_wStr = str;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
			return;
		}
	}

	pMonster->GetProp_Path()->DeletePath();
	pMonster->GetProp_Animation()->SetAnimation( pSkillInfo->s_dwSequenceID, true );
}

void cCliGame::RecvMonBuffRelease()
{
	u2 nIDX = 0;			//	성장 스킬 사용중인 몬스터 UID
	u2 nMonSkill_Idx = 0;		// 몬스터스킬 인덱스

	int nBuffCode = nsBuff::BK_MONSTER_GROWTH;	//	버프코드

	pop( nIDX );
//	nIDX = g_pCharMng->GetMonster( 1 )->GetUniqID();
	CsC_AvObject* pObject = g_pMngCollector->GetObject( nIDX );	

	// 몬스터스킬 인덱스
	pop( nMonSkill_Idx );

	// 해당 스킬 정보
	CsMonsterSkill::sINFO* pSkillInfo =  nsCsFileTable::g_pMonsterMng->GetMonsterSkill( nMonSkill_Idx );

	if( pSkillInfo == NULL )
		return;

	switch( pSkillInfo->s_nSkillType )	// 스킬 효과별로 체크
	{
	case CsMonsterSkill::GROWTH:	// 몬스터 성장 스킬
		{
			// 버프 해제
			( (CMonster*)pObject )->GetBuff()->ReleaseBuff( nBuffCode );
		}
		break;
	default:
		break;
	}
	wstring str;
	//GetVAString( _T( "%s 스킬 버프해제" ), nsCsFileTable::g_pBuffMng->GetBuff( nBuffCode )->GetInfo()->s_szName );	
	DmCS::StringFn::Format(str, _T( "%s 스킬 버프해제" ), nsCsFileTable::g_pBuffMng->GetBuff( nBuffCode )->GetInfo()->s_szName);
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = str;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}
#endif	// MONSTER_SKILL_GROWTH