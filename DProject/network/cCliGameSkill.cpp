

#include "stdafx.h"
#include "cCliGame.h"
#include "common_vs2019/pSkill.h"


void cCliGame::RecvTamerBuffActive(void)
{
	//u4 nBuffNo;
	//u4 nExpBuff;
	//u4 nActiveEndTS;	// 비활성 상태로 되기까지 남은 시간

	//pop(nBuffNo);		// 사용된 버프 BIT
	//pop(nExpBuff);		// 현재.. 버프 상태
	//pop(nActiveEndTS);	// 남아있는 버프 시간(초)

	//assert_csm( g_pCharMng->GetTamerUser(), _ERROR_MSG( "로딩중에 버프 정보를 받아서는 아니된다" ) );
	//cCondition* pCondition = g_pCharMng->GetTamerUser()->GetCondition();
	//pCondition->SetExpCon( nExpBuff );

	//pCondition->GetExpBuff( nBuffNo )->s_nCurTimeSec = nActiveEndTS + _TIME_TS;
}


void cCliGame::RecvTamerBuffStart(void)
{
//	u4 nExpBuff;
//	pop(nExpBuff);	// 현재.. 버프 상태
//
//	/*assert_csm( g_pCharMng->GetTamerUser(), _ERROR_MSG( "로딩중에 버프 정보를 받아서는 아니된다" ) );
//	cCondition* pCondition = g_pCharMng->GetTamerUser()->GetCondition();
//	pCondition->SetExpCon( nExpBuff );	*/
}


void cCliGame::RecvTamerBuffInactive(void)
{
	//u4 nBuffNo;
	//u4 nExpBuff;
	//u4 nBuffEndTS = 0;

	//pop(nBuffNo);	// 사용된 버프 BIT
	//pop(nExpBuff);	// 현재.. 버프 상태

	//if( ( nBuffNo & nSync::Pp_Total ) == 0 )	// 파티 버프인 경우
	//{
	//	pop(nBuffEndTS);
	//}

//#ifndef _GIVE	
//	u4 nBuffRemainedTS = nBuffEndTS - _TIME_TS;
//	char msg[222];
//	sprintf_s(msg, "BuffStart %d TS:%d", nBuffNo, nBuffRemainedTS);
//	CHAT_LOG(msg);
//
//	LOG("nBuffNo:%d, 남은시간:%d", nBuffNo, nBuffRemainedTS);
//#endif //_GIVE
//
//	assert_csm( g_pCharMng->GetTamerUser(), _ERROR_MSG( "로딩중에 버프 정보를 받아서는 아니된다" ) );
//	cCondition* pCondition = g_pCharMng->GetTamerUser()->GetCondition();
//	pCondition->SetExpCon( nExpBuff );
//
//	// 정보 입력
//	if( nsCsFileTable::g_pAddExpMng->IsData( nBuffNo ) == false )
//	{
//		assert_csm1( false, _ERROR_MSG( "g_pAddExpMng : Invalide Value = %x" ), nBuffNo );
//		return;
//	}
//	DWORD nSkill = nsCsFileTable::g_pAddExpMng->GetData( nBuffNo )->s_nSkillID;
//	CsSkill::sINFO* pInfo = nsCsFileTable::g_pSkillMng->GetSkill( nSkill )->GetInfo();
//	DWORD nSec = (int)pInfo->s_fDamageTime + pInfo->s_nDamageDay*24*60*60;
//
//
//	cCondition::sBUFF_INFO* pBuffInfo = pCondition->GetExpBuff( nBuffNo );
//	if( pBuffInfo->s_eKind != cCondition::BK_EXP_PARTY_OTHER )
//	{
//		pBuffInfo->s_nRemainDay = nBuffEndTS;
//		pBuffInfo->s_nTotalTimeSec = nSec;
//		pBuffInfo->s_nCurTimeSec = nBuffEndTS;
//	}
//	else
//	{
//		pBuffInfo->s_nRemainDay = 0;
//		pBuffInfo->s_nTotalTimeSec = 0;
//		pBuffInfo->s_nCurTimeSec = 0;
//	}
}



void cCliGame::RecvTamerBuffEnd(void)
{
	//u4 nBuffNo;
	//pop(nBuffNo);

	//u4 nExpBuff;
	//pop(nExpBuff);

	//assert_csm( g_pCharMng->GetTamerUser(), _ERROR_MSG( "로딩중에 버프 정보를 받아서는 아니된다" ) );
	//cCondition* pCondition = g_pCharMng->GetTamerUser()->GetCondition();
	//pCondition->SetExpCon( nExpBuff );
}


void cCliGame::RecvTamerBuffInfo(void)
{
//	// 사용하지 아니합니다. 2011.01.04
//
//	u4 nBuffNo;
//	u4 nBuffEndTS;
//	u4 nBuffElapseTS;	// 값이 0이면, 금일 사용 시간 초과 - inactive 상태임
//
//	pop(nBuffNo);
//	while(nBuffNo)
//	{
//		pop(nBuffEndTS);
//		pop(nBuffElapseTS);		// 금일 버프 사용된 시간
//
//		// nBuffNo, nBuffEndTS  처리 필요
//#ifndef _GIVE
//		LOG("nBuffNo:%d 남은시간:%d", nBuffNo, nBuffEndTS - _TIME_TS);
//#endif //_GIVE
//#pragma todo("현재 적용중인 버프 목록 - 맵이동 또는 처음 접속시 수신됨")
//
//		pop(nBuffNo);
//		//g_pCharMng->GetTamerUser()->AddExpCondition( nBuffNo );
//	}
}


void cCliGame::RecvSkill(void)
{
	assert_csm( false, _T( "여기 들어오면 안되는데. 송명수를 호출해 주세요.!!" ) );

// 	u1 nSkillArrIDX;
// 	u2 nHitterUID;
// 	u2 nTargetUID;
// 
// 	pop(nSkillArrIDX);
// 	pop(nHitterUID);
// 	pop(nTargetUID);
}



void cCliGame::RecvSkillPointUp(void)
{
	u4 nUID = 0;
	pop(nUID);

	u1 nEvoUnitIDX = 0;
	pop(nEvoUnitIDX);
	u1 nSkillPoint = 0;
	pop(nSkillPoint); // 현재 보유하고 있는 총 포인트 수

	u1 nSkillLevel = 0;
	pop(nSkillLevel);
	u4 nSkillExp = 0;
	pop(nSkillExp);

	CsC_AvObject* pObject = g_pMngCollector->GetObject( nUID );
	if( pObject == NULL )
		return;

	assert_cs( pObject->GetLeafRTTI() == RTTI_DIGIMON_USER );
	pObject->GetProp_Attack()->InsertEvent( CsC_AttackProp::EVENT_SKILL_UP, nSkillLevel, nSkillPoint, nSkillExp, nEvoUnitIDX );
}

void cCliGame::RecvWideSkillApply(void)
{
	// 어떤 광역 효과인가?
	u1 bIsItem = 0;
	//pop( bIsItem );

	if( bIsItem == 0 )
	{
		// 광역 스킬
		_RecvWideSkillApplyAround_ExistHitter();
	}
}

void cCliGame::RecvAroundSkillAni(void)
{
	u1 uWarning = 0;
	u4 uLifeTime;
	pop( uWarning );
	if( uWarning == 0 )
	{
		if( RecvMonsterSkill( 0 ) )
			return;
	}
	pop( uLifeTime );
	if(uWarning)
		g_pGameIF->GetBossScene()->SetWarning2((float)uLifeTime);
}
void cCliGame::RecvAroundSkillDmg(void)
{
}

void cCliGame::RecvSkillApplyAround(void)
{
	u1 bIsItem;
	pop( bIsItem );
	
	if( bIsItem == 0 )
	{
		// 스킬
		_RecvSkillApplyAround_ExistHitter();
	}
	else if( bIsItem == 1 )
	{
		// 아이템
		_RecvSkillApplyAround_NoneHitter();
	}
	else if( bIsItem == 2 )
	{
		// 버프류
		_RecvDotSkillAround();
	}
	else if ( bIsItem == 3)
	{

		_RecvSkillApplyME_NoneHitter();
	}
	else
	{
		assert_csm( false, L"잘못된 스킬어라운드값" );
	}
}

void cCliGame::_RecvWideSkillApplyAround_ExistHitter()
{
	u4 nHitterUID = 0;		// 대게 보스인덱스
	u4 nSkillArrIDX = 0;	// 때리는놈 스킬번호
   	u4 nTargetUID = 0;		// 타겟 인덱스, 표현상의 목표
	u4 nSkillSpeed = 0;		// 스킬 속도 추가 lks007
	u4 nSkillAccel = 0;		// 스킬 가속도 추가	lks007
	
	pop(nHitterUID); //16672
	pop(nSkillArrIDX);	// 현무몬 인던 추가 시 몬스터 스킬 수정 ( 스킬 애니 ID -> 몬스터스킬 ID ) 91 이걸로 몬스터랑 해당 애니메이션 시퀀스아디 갖고옴

	CsMonsterSkill::sINFO* pSkillInfo = nsCsFileTable::g_pMonsterMng->GetMonsterSkill( nSkillArrIDX );
	if( pSkillInfo != NULL )
	{
		nSkillSpeed = pSkillInfo->s_nValocity;
		nSkillAccel = pSkillInfo->s_nAccel;

		switch( pSkillInfo->s_nSkillType )
		{
		case CsMonsterSkill::ASSEMBLE:		// 모아맞기		( 대상 수 만큼 데미지 분산 )
		case CsMonsterSkill::DISPERSE:		// 흩어져맞기	( 대상 수 만큼 데미지 중첩 )
			{
				u4 nEffectTargetCnt;
				pop( nEffectTargetCnt );

				// 사용 이펙트 플래그
	#define CENTERFLAG	nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER
	#define FOOTFLAG	nsEFFECT::POS_CHARPOS
	#define HEIGHTFLAG	nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_THEIGHT

				u4 nEffectTargetIDX = 0;
				for( u4 i = 0 ; i < nEffectTargetCnt ; i++ )
				{
					pop( nEffectTargetIDX );
					//스킬 시전 대상 설정:: 기존 광역 스킬은 현재 타겟(일반공격 중인)이 무조건 맞도록 설정되어 있어서
					//						이후 스킬 시전 대상이 일반 공격중인 타겟으로 오게 되어있는데
					//						모아맞기, 흩어맞기일 경우 대상이 따로 존재하기 때문에
					//						스킬 시전 대상을 징표자 중 한명으로 설정함
					nTargetUID = nEffectTargetIDX;

					CsC_AvObject* pTarget = g_pMngCollector->GetObject( nEffectTargetIDX );
					if( pTarget )
					{	// 대상 표시 이펙트 제거
						pTarget->DeleteLoopEffectSkill( nSkillArrIDX );
						pTarget->DeleteLoopEffectSkill( nSkillArrIDX*1000 );// 범위 표시 이펙트
					}
				}
	#undef CENTERFLAG
	#undef FOOTFLAG
	#undef HEIGHTFLAG
			}
			break;
 		case CsMonsterSkill::ATTACH_SEED:	// 씨앗깔기
			break;
		case CsMonsterSkill::BERSERK:		// 광폭화
			return;
		case CsMonsterSkill::HP_VAL_DECREASE:
			{			
				// 스킬 범위 이펙트 삭제
				switch( pSkillInfo->s_nSkillType )
				{
				case CsMonsterSkill::HP_VAL_DECREASE:	// 광역스킬
					{
						// 시전이 있는 근접 광역일 때만
						if( pSkillInfo->s_nCastCheck == 1 )
						{
							CsC_AvObject* pObjcet = g_pMngCollector->GetObject( nHitterUID );
							if( pObjcet )
							{
								pObjcet->DeleteLoopEffectSkill( nSkillArrIDX );
								pObjcet->DeleteLoopEffectSkill( nSkillArrIDX*1000 );// 범위 표시 이펙트
							}
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		}

	}

	u2 nTargetCount = 0;	// 광역입힌 오브젝트의 수
	pop(nTargetCount);

	//스킬 시전 대상 설정:: 기존 광역 스킬은 현재 타겟(일반공격 중인)이 무조건 맞도록 설정되어 있어서
	//						이후 스킬 시전 대상이 일반 공격중인 타겟으로 오게 되어있는데
	//						모아맞기, 흩어맞기일 경우 대상이 따로 존재하기 때문에
	//						스킬 시전 대상을 징표자 중 한명으로 설정함
	if( nTargetUID != 0 )
	{
		u4 nTmpUID = 0;
		pop( nTmpUID );
	}
	else
		pop(nTargetUID);

	//ZLog(L"Count=== %d",nTargetCount );
	for( int i =0; i < nTargetCount; i++ )
	{
		u1 uDead;
		pop(uDead);
		bool bIsDead;

		bIsDead = uDead ? true:false;

		u4 nUserUID = 0;	// 유저 인덱스
		pop(nUserUID);
		n4 nDamage;
		pop(nDamage);

		u1 nTargetHpRate;
		pop(nTargetHpRate);

		CsC_AvObject* pHitter = g_pMngCollector->GetObject( nHitterUID );
		if( NULL != pHitter )
		{

			//assert_csm1( nSkillArrIDX < nLimit::Skill, _T( "nSkillArrayIdx = %d" ), nSkillArrIDX ); // nLimit::Skill 값 5로 일단 스킵		lks007

			sRECV_HITTER_INFO hitter;
			hitter.s_nUID = nHitterUID;	
			hitter.s_nSkillIndex = (int)nSkillArrIDX;
			if( pSkillInfo != NULL )
			{
				switch( pSkillInfo->s_nSkillType )
				{
				case CsMonsterSkill::ATTACH_SEED:	// 씨앗깔기		( 대상 디버프, 시간 경과 후 대상 위치 바닥 생성 )
				case CsMonsterSkill::BERSERK:		// 광폭화		( 공략 시간 초과 )
					hitter.s_HitType = CsC_AttackProp::HT_Skill_NoAni;
					break;
				default:
					hitter.s_HitType = CsC_AttackProp::HT_Skill;
					break;
				}
			}
			else
				hitter.s_HitType = CsC_AttackProp::HT_Skill;

			sRECV_TARGET_INFO target;
			target.s_nUID = nUserUID;
			target.s_DamageType = CsC_AttackProp::DT_Normal;	
			target.s_nNumEffectType = NUMTYPE::ET_NORMAL;		




			n4 nTargetAttribute[ MaxExtStat ] = {0, };
			nTargetAttribute[ HP ] = nDamage;


			target.s_nValue[ 0 ] = nTargetAttribute[ HP ];	// 총 데미지 값


			//ZLog(L"0==ID:%d==nTargetAttribute[ HP ]:%d==nTargetHpRate:%d",nUserUID,  nTargetAttribute[ HP ], nTargetHpRate);
			// 미스
			if( nTargetAttribute[ HP ] == 0 )
			{
				hitter.s_HitType = CsC_AttackProp::HT_Miss;
				target.s_DamageType = CsC_AttackProp::DT_Miss;
			}
			// 데미지
			else
			{
				CsC_AvObject* pTarget = g_pMngCollector->GetObject( nUserUID );
				if( pTarget )
				{
					if( pTarget->GetLeafRTTI() != RTTI_DIGIMON_USER )
					{
						target.s_nValue[ 1 ] = nTargetHpRate - pTarget->GetBaseStat()->GetBackupHP(); // 총 데미지 255분율 퍼센티지값
						pTarget->GetBaseStat()->SetBackupHP( nTargetHpRate );
						//ZLog(L"1==%d===nDamage:%d===ID:%d==s_nValue[1]:%d",pTarget->GetBaseStat()->GetLevel(), nDamage, nUserUID , target.s_nValue[ 1 ]);
					}
					else
					{					
						pTarget->GetBaseStat()->SetDeltaHP( nTargetAttribute[ HP ] );
						pTarget->GetBaseStat()->SetDeltaBackupHP( nTargetAttribute[ HP ] );
						//ZLog(L"2==%d===nDamage:%d===ID:%d==s_nValue[1]:%d",pTarget->GetBaseStat()->GetLevel(), nDamage, nUserUID , target.s_nValue[ 1 ]);
					}

				}
			}


			SetSkillInfo2( &hitter, &target, bIsDead, nTargetUID, (int)nSkillSpeed, (int)nSkillAccel );

			if(bIsDead)
			{
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
				//CProtocol.m_wStr = GetVAString(_T("WideSkillApplyAround :: UID - %d "), nUserUID);
				DmCS::StringFn::Format(CProtocol.m_wStr, _T("WideSkillApplyAround :: UID - %d "), nUserUID);
				GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
				CsC_AvObject* pTarget = g_pMngCollector->GetObject( nUserUID );
				if( pTarget )
				{
					SetDieInfo( pTarget );
				}
			}

		}

	}

	CsC_AvObject* pHitter = g_pMngCollector->GetObject( nHitterUID );
	if( pHitter != NULL )
		((CMonster*)pHitter)->SetCasting_Monster( false );
}

// 스킬 사용 패킷을 받을 때 호출
// 스킬 코드에 맞는 시퀜스를 호출함.
void cCliGame::RecvSkillAction(void) //_RecvWideSkillApplyAround_ExistHitter
{
	u4 nHitterUID = 0;		// 때리는 놈 인덱스
	u4 nSkillArrIDX = 0;	// 스킬 인덱스

	pop( nHitterUID );
	pop( nSkillArrIDX );

	CsC_AvObject* pHitterObjcet = g_pMngCollector->GetObject( nHitterUID );
	SAFE_POINTER_RET( pHitterObjcet );

	CsMonsterSkill::sINFO* pSkillInfo = nsCsFileTable::g_pMonsterMng->GetMonsterSkill( nSkillArrIDX );
	SAFE_POINTER_RET( pSkillInfo );

	CsC_EffectProp* pHitterEffProp = pHitterObjcet->GetProp_Effect();
	SAFE_POINTER_RET( pHitterEffProp );

	if( pHitterObjcet->GetLeafRTTI() == RTTI_MONSTER )
	{
		if( pSkillInfo->s_nTalkId != 0 )// 몬스터 스킬 시전 토크		
			cPrintMsg::PrintMsg( pSkillInfo->s_nTalkId );
	}

	// 기존 스킬 이펙트가 있으면 제거
	pHitterEffProp->DeleteSkillLoopEffect( nSkillArrIDX );
	pHitterEffProp->DeleteSkillLoopEffect( nSkillArrIDX*1000 );	// 범위 표시 이펙트

	CsC_AvObject* pTargetObjcet = NULL;
	switch ( pSkillInfo->s_nActiveType )		// 스킬 적용 대상
	{
	case 0:		// 자신
		{
			CsC_AniProp* pHitterAni = pHitterObjcet->GetProp_Animation();
			if( pHitterAni )
				pHitterAni->SetAnimation( pSkillInfo->s_dwSequenceID );

			if( strlen( pSkillInfo->s_szNoticeEff_Name ) > 0 )
			{
				CsC_EffectProp* pTargetEffectProp = pHitterObjcet->GetProp_Effect();
				SAFE_POINTER_BEK( pTargetEffectProp );

				//바닥 이펙트용 좌표세팅
				char szEff[OBJECT_PATH_LEN] = {0,};				
				DWORD dwFlag = nsEFFECT::POS_CHARPOS | nsEFFECT::LIVE_LOOP | nsEFFECT::OFFSET_USE;			

				// 대상 표시 이펙트
				float fScale = 1.0f;
				if( nsCsFileTable::g_pMonsterMng )
				{
					CsMonsterSkillTerms::sINFO* pSkillTerm = nsCsFileTable::g_pMonsterMng->GetMonsterSkillTerms( pSkillInfo->s_nRangeIdx );
					if( pSkillTerm && pSkillTerm->s_nRange > 0 )
					{
						// 기본적으로 광역 스킬 이펙트는 8M ( 범위 800 ) 기준으로 리소스 제작되므로
						// 리소스를 테이블에서 설정한 범위에 따라 크기 변경하기
						fScale = ((float)pSkillTerm->s_nRange)/800.0f;
					}
				}

				pTargetEffectProp->AddSkillLoopEffect( pSkillInfo->s_szNoticeEff_Name, nSkillArrIDX, dwFlag, false,1.0f, pSkillInfo->s_fNoticeTime * 0.001f );
				pTargetEffectProp->_GetLoopSkillEffect( nSkillArrIDX )->SetScale( fScale );	
			}
		}
		break;
	case 1:		// 타겟
		{
			n4 nTargetCount = 0;
			pop( nTargetCount );
			if( 0 == nTargetCount )
				break;

			float fAniEndTime = 0.0f;
			CsC_AniProp* pHitterAni = pHitterObjcet->GetProp_Animation();

			if( pHitterAni )			
			{
				CsC_AniProp_Sequence* pSeq = pHitterAni->GetSequenceMng();
				if( pSeq )
				{
					CsC_LoadSequence::sSEQUENCE_INFO* pInfo = pSeq->GetSequence(pSkillInfo->s_dwSequenceID);
					if( pInfo )
						fAniEndTime = pInfo->s_fEndTime;
				}
			}

			bool bRangeAtt = pHitterObjcet->IsHaveRangeEvent( pSkillInfo->s_dwSequenceID );

			for ( int i = 0; i < nTargetCount; i++ )
			{
				u4 nTargetUID = 0;			
				pop( nTargetUID );

				CsC_AvObject* pTargetObjcet = g_pMngCollector->GetObject( nTargetUID );
				SAFE_POINTER_CON( pTargetObjcet );

				if( bRangeAtt )
				{	// 원거리 공격일 경우 발사체가 날라가는 시간 계산
					float fDist = ( pHitterObjcet->GetPos2D() - pTargetObjcet->GetPos2D() ).Length();

					float fHitTimeVelocity = pSkillInfo->s_nValocity;
					float fHitTimeAccelVelocity = pSkillInfo->s_nAccel;
					if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
					{
						fHitTimeVelocity *= BATTLE_SPEED_RANGE;
						fHitTimeAccelVelocity *= BATTLE_SPEED_RANGE;
					}

					fAniEndTime += CsC_AttackProp::CalRangeHitTime( fHitTimeVelocity, fHitTimeAccelVelocity, fDist );
				}

				if( strlen( pSkillInfo->s_szNoticeEff_Name ) > 0 )
				{
					CsC_EffectProp* pTargetEffectProp = pTargetObjcet->GetProp_Effect();
					SAFE_POINTER_CON( pTargetEffectProp );

					//바닥 이펙트용 좌표세팅
					char szEff[OBJECT_PATH_LEN] = {0,};				
					DWORD dwFlag = nsEFFECT::POS_CHARPOS | nsEFFECT::LIVE_LOOP | nsEFFECT::OFFSET_USE;			

					// 대상 표시 이펙트
					float fScale = 1.0f;
					if( nsCsFileTable::g_pMonsterMng )
					{
						CsMonsterSkillTerms::sINFO* pSkillTerms = nsCsFileTable::g_pMonsterMng->GetMonsterSkillTerms( pSkillInfo->s_nRangeIdx );
						if( pSkillTerms && pSkillTerms->s_nRange > 0 )
						{
							// 기본적으로 광역 스킬 이펙트는 8M ( 범위 800 ) 기준으로 리소스 제작되므로
							// 리소스를 테이블에서 설정한 범위에 따라 크기 변경하기
							fScale = ((float)pSkillTerms->s_nRange)/800.0f;
						}

					}

					pTargetEffectProp->AddSkillLoopEffect( pSkillInfo->s_szNoticeEff_Name, nSkillArrIDX, dwFlag, false,1.0f, pSkillInfo->s_fNoticeTime * 0.001f );
					pTargetEffectProp->_GetLoopSkillEffect( nSkillArrIDX )->SetScale( fScale );	
				}	

				CsC_AttackProp::sHIT_INFO info;
				info.s_eHitType = CsC_AttackProp::HT_Skill;
				info.s_dwAniID = pSkillInfo->s_dwSequenceID;	
				info.s_fAniAccelRate = 1.0f;
				info.s_pPartnerDamageInfo = NULL;
				info.s_pHitTarget = pTargetObjcet;
				info.s_fRangeVelocity = pSkillInfo->s_nValocity;
				info.s_fRangeAccelVelocity = pSkillInfo->s_nAccel;
				info.s_bRangeAttack = bRangeAtt;
				info.s_bRotation = true;
				info.s_nAttackEffectIndex = 0;
				info.s_vDamagePos = pTargetObjcet->GetPos();
				info.s_nReflectType = 0;

				if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
				{
					info.s_fRangeVelocity *= BATTLE_SPEED_RANGE;
					info.s_fRangeAccelVelocity *= BATTLE_SPEED_RANGE;
					info.s_fAniAccelRate = BATTLE_SPEED_SKILL;
				}

				CsC_AttackProp* pAttackProp= pHitterObjcet->GetProp_Attack();
				if( pAttackProp )
					pAttackProp->InsertHit( &info );	
			}	
		}break;
	case 2:					// 좌표
		{
			n4 nTargetPosCount = 0;
			pop( nTargetPosCount );
			if( 0 == nTargetPosCount )
				break;

			float fAniEndTime = 0.0f;
			CsC_AniProp* pHitterAni = pHitterObjcet->GetProp_Animation();
			
			if( pHitterAni )			
			{
				CsC_AniProp_Sequence* pSeq = pHitterAni->GetSequenceMng();
				if( pSeq )
				{
					CsC_LoadSequence::sSEQUENCE_INFO* pInfo = pSeq->GetSequence(pSkillInfo->s_dwSequenceID);
					if( pInfo )
						fAniEndTime = pInfo->s_fEndTime;
				}
			}

			bool bRangeAtt = pHitterObjcet->IsHaveRangeEvent( pSkillInfo->s_dwSequenceID );
			for ( int i = 0; i < nTargetPosCount; i++ )
			{
				n4 nTargetPosX;
				n4 nTargetPosY;

				pop( nTargetPosX );
				pop( nTargetPosY );

				if( bRangeAtt )
				{
					NiPoint2 targetPos( (float)nTargetPosX, (float)nTargetPosY );
					float fDist = ( pHitterObjcet->GetPos2D() -targetPos ).Length();

					float fHitTimeVelocity = pSkillInfo->s_nValocity;
					float fHitTimeAccelVelocity = pSkillInfo->s_nAccel;
					if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
					{
						fHitTimeVelocity *= BATTLE_SPEED_RANGE;
						fHitTimeAccelVelocity *= BATTLE_SPEED_RANGE;
					}

					fAniEndTime += CsC_AttackProp::CalRangeHitTime( fHitTimeVelocity, fHitTimeAccelVelocity, fDist );
				}

				float fPosZ = nsCsGBTerrain::g_pCurRoot->GetHeight( (float)nTargetPosX, (float)nTargetPosY );
				NiPoint3 vPos = NiPoint3( (float)nTargetPosX, (float)nTargetPosY, fPosZ );
				pTargetObjcet = g_pCharMng->AddTempObject( nHitterUID, vPos, true, fAniEndTime + 5.0f );
				SAFE_POINTER_CON( pTargetObjcet );

				if( strlen( pSkillInfo->s_szNoticeEff_Name ) > 0 )
				{
					CsC_EffectProp* pTargetEffectProp = pTargetObjcet->GetProp_Effect();
					SAFE_POINTER_CON( pTargetEffectProp );

					//바닥 이펙트용 좌표세팅
					char szEff[OBJECT_PATH_LEN] = {0,};				
					DWORD dwFlag = nsEFFECT::POS_NORMAL | nsEFFECT::LIVE_LOOP | nsEFFECT::OFFSET_USE;			

					// 대상 표시 이펙트
					float fScale = 1.0f;
					if( nsCsFileTable::g_pMonsterMng )
					{
						 CsMonsterSkillTerms::sINFO* pSkillTerms = nsCsFileTable::g_pMonsterMng->GetMonsterSkillTerms( pSkillInfo->s_nRangeIdx );
						 if( pSkillTerms && pSkillTerms->s_nRange > 0 )
						 {
							 // 기본적으로 광역 스킬 이펙트는 8M ( 범위 800 ) 기준으로 리소스 제작되므로
							 // 리소스를 테이블에서 설정한 범위에 따라 크기 변경하기
							 fScale = ((float)pSkillTerms->s_nRange)/800.0f;
						 }
					}

					pTargetEffectProp->AddSkillLoopEffect( pSkillInfo->s_szNoticeEff_Name, nSkillArrIDX, dwFlag, false,1.0f, pSkillInfo->s_fNoticeTime * 0.001f );
					pTargetEffectProp->_GetLoopSkillEffect( nSkillArrIDX )->SetScale( fScale );	
				}	

				CsC_AttackProp::sHIT_INFO info;
				info.s_eHitType = CsC_AttackProp::HT_Skill;
				info.s_dwAniID = pSkillInfo->s_dwSequenceID;	
				info.s_fAniAccelRate = 1.0f;
				info.s_pPartnerDamageInfo = NULL;
				info.s_pHitTarget = pTargetObjcet;
				info.s_fRangeVelocity = pSkillInfo->s_nValocity;
				info.s_fRangeAccelVelocity = pSkillInfo->s_nAccel;
				info.s_bRangeAttack = bRangeAtt;
				info.s_bRotation = true;
				info.s_nAttackEffectIndex = 0;
				info.s_vDamagePos = pTargetObjcet->GetPos();
				info.s_nReflectType = 0;

				if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
				{
					info.s_fRangeVelocity *= BATTLE_SPEED_RANGE;
					info.s_fRangeAccelVelocity *= BATTLE_SPEED_RANGE;
					info.s_fAniAccelRate = BATTLE_SPEED_SKILL;
				}

				CsC_AttackProp* pAttackProp= pHitterObjcet->GetProp_Attack();
				if( pAttackProp )
					pAttackProp->InsertHit( &info );	
			}	
		}
		break;
	case 4:					// 랜덤 멀티
		{
			n4 nTargetPosCount = 0;
			pop( nTargetPosCount );
			BHPRT("SkillStart : [%d]", nTargetPosCount);
			if( 0 == nTargetPosCount )
				break;
			float fAniEndTime = 0.0f;
			CsC_AniProp* pHitterAni = pHitterObjcet->GetProp_Animation();

			if( pHitterAni )			
			{
				CsC_AniProp_Sequence* pSeq = pHitterAni->GetSequenceMng();
				if( pSeq )
				{
					CsC_LoadSequence::sSEQUENCE_INFO* pInfo = pSeq->GetSequence(pSkillInfo->s_dwSequenceID);
					if( pInfo )
						fAniEndTime = pInfo->s_fEndTime;
				}
			}
			CsC_AttackProp* pAttackProp= pHitterObjcet->GetProp_Attack();
			if(pAttackProp == NULL)
				return;
			pAttackProp->GetMultyTarget()->clear();
			bool bRangeAtt = pHitterObjcet->IsHaveRangeEvent( pSkillInfo->s_dwSequenceID );

			n4 nTargetPosX;
			n4 nTargetPosY;

			for ( int i = 0; i < nTargetPosCount; i++ )
			{
				pop( nTargetPosX );
				pop( nTargetPosY );
				float currX = nTargetPosX;
				float currY = nTargetPosY;

				if( !nsCsGBTerrain::g_pCurRoot->IsInTerrain( (float)currX, (float)currY ) )
				{
					BHPRT("False Attack Terrain Pos : X [%d] Y[%d]", nTargetPosX,nTargetPosY );
					continue;
				}

				BHPRT("Attack Pos : X [%d] Y[%d]", nTargetPosX,nTargetPosY );

				if( bRangeAtt )
				{
					NiPoint2 targetPos( (float)currX, (float)currY );
					float fDist = ( pHitterObjcet->GetPos2D() -targetPos ).Length();

					float fHitTimeVelocity = pSkillInfo->s_nValocity;
					float fHitTimeAccelVelocity = pSkillInfo->s_nAccel;
					if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
					{
						fHitTimeVelocity *= BATTLE_SPEED_RANGE;
						fHitTimeAccelVelocity *= BATTLE_SPEED_RANGE;
					}

					fAniEndTime += CsC_AttackProp::CalRangeHitTime( fHitTimeVelocity, fHitTimeAccelVelocity, fDist );
				}
				float fPosZ = nsCsGBTerrain::g_pCurRoot->GetHeight( (float)currX, (float)currY );
				NiPoint3 vPos = NiPoint3( (float)currX, (float)currY, fPosZ );
				pTargetObjcet = g_pCharMng->AddTempObject( nHitterUID, vPos, true, fAniEndTime + 5.0f );
				SAFE_POINTER_CON( pTargetObjcet );

				if( strlen( pSkillInfo->s_szNoticeEff_Name ) > 0 )
				{
					CsC_EffectProp* pTargetEffectProp = pTargetObjcet->GetProp_Effect();
					SAFE_POINTER_CON( pTargetEffectProp );

					//바닥 이펙트용 좌표세팅
					char szEff[OBJECT_PATH_LEN] = {0,};				
					DWORD dwFlag = nsEFFECT::POS_NORMAL | nsEFFECT::LIVE_LOOP | nsEFFECT::OFFSET_USE;			

					// 대상 표시 이펙트
					float fScale = 1.0f;
					if( nsCsFileTable::g_pMonsterMng )
					{
						CsMonsterSkillTerms::sINFO* pSkillTerms = nsCsFileTable::g_pMonsterMng->GetMonsterSkillTerms( pSkillInfo->s_nRangeIdx );
						if( pSkillTerms && pSkillTerms->s_nRange > 0 )
						{
							// 기본적으로 광역 스킬 이펙트는 8M ( 범위 800 ) 기준으로 리소스 제작되므로
							// 리소스를 테이블에서 설정한 범위에 따라 크기 변경하기
							fScale = ((float)pSkillTerms->s_nRange)/800.0f;
						}
					}

					pTargetEffectProp->AddSkillLoopEffect( pSkillInfo->s_szNoticeEff_Name, nSkillArrIDX, dwFlag, false,1.0f, pSkillInfo->s_fNoticeTime * 0.001f );
					pTargetEffectProp->_GetLoopSkillEffect( nSkillArrIDX )->SetScale( fScale );	
				}	

				CsC_AttackProp::sHIT_INFO info;
				info.s_eHitType = CsC_AttackProp::HT_Skill;
				info.s_dwAniID = pSkillInfo->s_dwSequenceID;
				info.s_MultyTargetID = i;
				info.s_fAniAccelRate = 1.0f;
				info.s_pPartnerDamageInfo = NULL;
				info.s_pHitTarget = pTargetObjcet;
				info.s_fRangeVelocity = pSkillInfo->s_nValocity;
				info.s_fRangeAccelVelocity = pSkillInfo->s_nAccel;
				info.s_bRangeAttack = bRangeAtt;
				info.s_bRotation = true;
				info.s_nAttackEffectIndex = 0;
				info.s_vDamagePos = pTargetObjcet->GetPos();
				info.s_nReflectType = 0;

				if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
				{
					info.s_fRangeVelocity *= BATTLE_SPEED_RANGE;
					info.s_fRangeAccelVelocity *= BATTLE_SPEED_RANGE;
					info.s_fAniAccelRate = BATTLE_SPEED_SKILL;
				}
				if( pAttackProp )
				{
					pAttackProp->AddMultyTarget(info.s_MultyTargetID, info.s_pHitTarget);
					pAttackProp->InsertMultiHit( &info );	
				}
			}	
		}
	}
}


void cCliGame::RecvSkillHit(void)
{

	u4 nHitterUID = 0;
	u4 nSkillArrIDX = 0;
	pop( nHitterUID );
	pop( nSkillArrIDX );

	u2 nTargetCount = 0; //맞은 놈 수
	pop( nTargetCount );

	for ( int i = 0; i < nTargetCount; i++ )
	{  
		u1 nDead = 0; //생사 체크
		pop( nDead );

		u4 nTargetUID = 0; //타겟 아이디
		pop( nTargetUID );

		n4 nDamage = 0; // 데미지
		pop( nDamage );

		u1 nTargetHpRate = 0; //타겟 체력
		pop( nTargetHpRate );

		CsC_AvObject* pHitter = g_pMngCollector->GetObject( nHitterUID );
		SAFE_POINTER_CON( pHitter );
		CsMonsterSkill::sINFO* pSkillInfo = nsCsFileTable::g_pMonsterMng->GetMonsterSkill( nSkillArrIDX );
		SAFE_POINTER_CON( pSkillInfo );

		sRECV_HITTER_INFO hitter;
		hitter.s_nUID = nHitterUID;	
		hitter.s_nSkillIndex = (int)nSkillArrIDX;

		switch( pSkillInfo->s_nSkillType )
		{
		case CsMonsterSkill::ATTACH_SEED:	// 씨앗깔기		( 대상 디버프, 시간 경과 후 대상 위치 바닥 생성 )
		case CsMonsterSkill::BERSERK:		// 광폭화		( 공략 시간 초과 )
			hitter.s_HitType = CsC_AttackProp::HT_Skill_NoAni;
			break;
		default:
			hitter.s_HitType = CsC_AttackProp::HT_Skill;
			break;
		}

		sRECV_TARGET_INFO target;
		target.s_nUID = nTargetUID;
		target.s_DamageType = CsC_AttackProp::DT_Normal;	
		target.s_nNumEffectType = NUMTYPE::ET_NORMAL;		
		
		n4 nTargetAttribute[ MaxExtStat ] = {0, };
		nTargetAttribute[ HP ] = nDamage;
		target.s_nValue[ 0 ] = nTargetAttribute[ HP ];	// 총 데미지 값

		//ZLog(L"0==ID:%d==nTargetAttribute[ HP ]:%d==nTargetHpRate:%d",nUserUID,  nTargetAttribute[ HP ], nTargetHpRate);
		// 미스
		if( nTargetAttribute[ HP ] == 0 )
		{
			hitter.s_HitType = CsC_AttackProp::HT_Miss;
			target.s_DamageType = CsC_AttackProp::DT_Miss;
		}
		// 데미지
		else
		{
			CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
			SAFE_POINTER_CON( pTarget );

			if( nDead )
				SetDieInfo( pTarget );

			CsC_AvObject::sBASE_STAT* pBaseStat =  pTarget->GetBaseStat();
			SAFE_POINTER_CON( pBaseStat );

			if( pTarget->GetLeafRTTI() != RTTI_DIGIMON_USER )
			{
				target.s_nValue[ 1 ] = nTargetHpRate - pBaseStat->GetBackupHP(); // 총 데미지 255분율 퍼센티지값
				pTarget->GetBaseStat()->SetBackupHP( nTargetHpRate );
				//ZLog(L"1==%d===nDamage:%d===ID:%d==s_nValue[1]:%d",pTarget->GetBaseStat()->GetLevel(), nDamage, nUserUID , target.s_nValue[ 1 ]);
			}
			else
			{					
				pBaseStat->SetDeltaHP( nTargetAttribute[ HP ] );
				pBaseStat->SetDeltaBackupHP( nTargetAttribute[ HP ] );
				//ZLog(L"2==%d===nDamage:%d===ID:%d==s_nValue[1]:%d",pTarget->GetBaseStat()->GetLevel(), nDamage, nUserUID , target.s_nValue[ 1 ]);
			}
		}

		SetDamage(&hitter, &target);
	}
}

void cCliGame::_RecvSkillApplyAround_ExistHitter()
{
	// 스킬
	u4 nHitterUID = 0;
	u4 nTargetUID = 0;
	u4 nSkillArrIDX = 0;
	pop(nHitterUID);
	pop(nTargetUID);
	pop(nSkillArrIDX);

	u1 nHitterHpRate = 0;
	pop( nHitterHpRate );
	u1 nTargetHpRate = 0;
	pop( nTargetHpRate );

	CsC_AvObject* pHitter = g_pMngCollector->GetObject( nHitterUID );
	if( pHitter == NULL )
	{
		return;
	}

	if( pHitter->GetLeafRTTI() != RTTI_TAMER && pHitter->GetLeafRTTI() != RTTI_TAMER_USER )
		assert_csm1( nSkillArrIDX < nLimit::Skill, _T( "nSkillArrayIdx = %d" ), nSkillArrIDX );

	sRECV_HITTER_INFO hitter;
	hitter.s_nUID = nHitterUID;	
	hitter.s_nSkillIndex = nSkillArrIDX;
	hitter.s_HitType = CsC_AttackProp::HT_Skill;

	sRECV_TARGET_INFO target;
	target.s_nUID = nTargetUID;
	target.s_DamageType = CsC_AttackProp::DT_Normal;	
	target.s_nNumEffectType = NUMTYPE::ET_NORMAL;

	// Hitter 변경 파라메타
	u1 nP;		// parameter
	n4 nV;		// value

	n4 nHitterAttribute[ MaxExtStat ] = {0, };

	pop(nP);
	while(nP < MaxExtStat)
	{
		pop(nV);
		nHitterAttribute[ nP ] = nV;
		pop( nP );
	}

	n4 nTargetAttribute[ MaxExtStat ] = {0, };

	// Target 변경 파라메타
	pop(nP);
	while(nP < MaxExtStat)
	{
		pop(nV);
		nTargetAttribute[ nP ] = nV;
		pop( nP );
	}

	u4 nBattleOption;
	pop(nBattleOption);
	hitter.s_HitType = nBattleOption & pBattle::BO_CRITICAL ? CsC_AttackProp::HT_Critical : CsC_AttackProp::HT_Skill;

#ifdef ENABLE_KNOCKBACK
	if( nBattleOption & pBattle::BO_KNOCKBACK )
	{
		target.s_DamageType = CsC_AttackProp::DT_KnockBack;
	}
#endif	// ENABLE_KNOCKBACK

	target.s_nValue[ 0 ] = nTargetAttribute[ HP ];

	// 미스
	if( nTargetAttribute[ HP ] == 0 )
	{
		hitter.s_HitType = CsC_AttackProp::HT_Miss;
		target.s_DamageType = CsC_AttackProp::DT_Miss;
	}
	// 데미지
	else
	{
		CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
		if( pTarget )
		{
			if( pTarget->GetLeafRTTI() != RTTI_DIGIMON_USER )
			{
				target.s_nValue[ 1 ] = nTargetHpRate - pTarget->GetBaseStat()->GetBackupHP();
				pTarget->GetBaseStat()->SetBackupHP( nTargetHpRate );
			}
			else
			{
				pTarget->GetBaseStat()->SetDeltaHP( nTargetAttribute[ HP ] );
				pTarget->GetBaseStat()->SetDeltaBackupHP( nTargetAttribute[ HP ] );
			}
		}
	}
	SetSkillInfo( &hitter, &target, false );
}


void cCliGame::_RecvSkillApplyAround_NoneHitter(void)
{
	// 아이템 사용
	u4 nHitterUID = 0;
	u4 nTargetUID = 0;
	u4 nSkillArrIDX = 0;
	pop(nHitterUID);
	pop(nTargetUID);
	pop(nSkillArrIDX);

	u1 nHitterHpRate = 0;
	u1 nTargetHpRate = 0;
	if( nHitterUID != 0 )
		pop( nHitterHpRate );

	pop( nTargetHpRate );

	sRECV_TARGET_INFO target;
	target.s_nUID = nTargetUID;
	target.s_DamageType = CsC_AttackProp::DT_Normal;	
	target.s_nNumEffectType = NUMTYPE::ET_NORMAL;

	// Hitter 변경 파라메타
	u1 nP;		// parameter
	n4 nV;		// value

	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
	// 타겟도 널이라면 히터도 없기 떄문에 그냥 리턴
	if( pTarget == NULL )
		return;

	pop(nP);
	while(nP < MaxExtStat)
	{
		pop(nV);
		pop(nP);
	}

	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_DIGIMON_USER:
	case RTTI_TAMER_USER:
		{
			n4 nTargetAttribute[ MaxExtStat ] = {0, };
			// Target 변경 파라메타
			pop(nP);
			while(nP < MaxExtStat)
			{
				pop(nV);
				nTargetAttribute[ nP ] = nV;

				switch( nP )
				{
				case HP:
					{
						pTarget->GetBaseStat()->SetDeltaHP( nTargetAttribute[ nP ] );
						pTarget->GetBaseStat()->SetDeltaBackupHP( nTargetAttribute[ nP ] );
						
						//  회복량 띄워주자
						g_pNumber3DMng->Add( -nV, pTarget->GetPos(), CsD2R( rand()%70 - 35 ), NUMTYPE::DIGIMON_RECOVER, NUMTYPE::ET_NORMAL );
					}
					break;
				case DS:
					{
						pTarget->GetBaseStat()->SetDeltaDS( nTargetAttribute[ nP ] );
						pTarget->GetBaseStat()->SetDeltaBackupDS( nTargetAttribute[ nP ] );
					}				
					break;
				case FP:
					{
						pTarget->GetBaseStat()->SetDeltaFP( nTargetAttribute[ nP ] );
					}				
					break;
				default:
					assert_csm1( false, _T( "RevSkillApply Parameter = %d" ), nP );
				}
				pop(nP);
			}
		}
		break;
#ifdef DEBUFF_ITEM
	case RTTI_MONSTER:
		{
			// Target 변경 파라메타
			pop(nP);
			int nTempVV;
			while(nP < MaxExtStat)
			{
				//pop(nV);
				pop(nTempVV);
				switch( nP )
				{
				case HP:
					// 데미지량 띄워주자
					g_pNumber3DMng->Add( -nTempVV, pTarget->GetPos(), CsD2R( rand()%70 - 35 ), NUMTYPE::DIGIMON_ATTACK, NUMTYPE::ET_NORMAL );

					// 이펙트를 띄워주자.
					char cEffectPath[ MAX_PATH ] = {0, };
					sprintf_s( cEffectPath, MAX_PATH, "system\\DeBuff\\%s", nsCsFileTable::g_pItemMng->GetItem( nSkillArrIDX )->GetInfo()->s_cNif );
					//pTarget->GetProp_Effect()->AddEffect( cEffectPath, 1.0f, 0 );
					pTarget->GetProp_Effect()->AddEffect( cEffectPath, 4.0f, nsEFFECT::POS_CHARPOS );

					//pTarget->GetProp_Effect()->AddFireWorkEffect( cEffectPath, 1.0f, 0 );
					//pTarget->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BOSS, "system\\LevelEffect\\BossEffect.nif" );
					break;
				}
				pop(nP);
			}		

			int nDelta = nTargetHpRate - pTarget->GetBaseStat()->GetBackupHP();
			pTarget->GetBaseStat()->SetDeltaHP( nDelta );
			pTarget->GetBaseStat()->SetDeltaBackupHP( nDelta );			
		}
		break;
#endif
	default:
		{			
			// Target 변경 파라메타
			pop(nP);
			while(nP < MaxExtStat)
			{
				pop(nV);
				switch( nP )
				{
				case HP:
					//  회복량 띄워주자
					g_pNumber3DMng->Add( -nV, pTarget->GetPos(), CsD2R( rand()%70 - 35 ), NUMTYPE::DIGIMON_RECOVER, NUMTYPE::ET_NORMAL );
					break;
				}
				pop(nP);
			}		

			int nDelta = nTargetHpRate - pTarget->GetBaseStat()->GetBackupHP();
			pTarget->GetBaseStat()->SetDeltaHP( nDelta );
			pTarget->GetBaseStat()->SetDeltaBackupHP( nDelta );			
		}
	}		
}


void cCliGame::_RecvSkillApplyME_NoneHitter(void)
{
	u4 nHitterUID = 0;
	n4 nSkillArrIDX=0;
	pop(nHitterUID);
	pop(nSkillArrIDX);

	//sRECV_HITTER_INFO hitter;
	//hitter.s_nUID = nHitterUID;	
	//hitter.s_nSkillIndex = nSkillArrIDX;
	//hitter.s_HitType = CsC_AttackProp::HT_Skill;

	CsC_AvObject* pHitter = g_pMngCollector->GetObject( nHitterUID );
	DWORD dwAniID = ANI::ATTACK_SKILL1 + nSkillArrIDX;	// 스킬인덱스 계산
	int nEventCount = 1;
	CsVectorPB< int >* pEventArray;
	// 때리는 넘의 때리는 애니메이션 정보 얻는다.dwAniID 로 아웃 파라미터 2개 얻는다.
	if( pHitter->GetProp_Animation()->GetNAttEventIndex( dwAniID, &pEventArray, nEventCount ) == false )
	{
		g_CriticalLog.Log( _T( "SetSKillInfo : No Have Ani_2 ( model = %d, ani = %d, SkillIndex = %d )" ), pHitter->GetModelID(), dwAniID, nSkillArrIDX );
		assert_csm2( false, L"ModelID = %d, LoadSeqModelID = %d", pHitter->GetModelID(), pHitter->GetProp_Animation()->GetSequenceMng()->GetLoadSequence()->GetModelID() );
		pHitter->RecvSkill( false, nSkillArrIDX );
		return;
	}

	assert_csm3( nEventCount > 0, L"AniID = %d, ModelID = %d, LoadSeqModelID = %d", dwAniID, pHitter->GetModelID(), pHitter->GetProp_Animation()->GetSequenceMng()->GetLoadSequence()->GetModelID() );

	CsVectorPB< float > vpEventTime( 10 );
	float fAccelRate = 1.0f;
	// 배틀 모드시 래이팅값설정
	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		fAccelRate = BATTLE_SPEED_SKILL;

	// 이밴트 카운트 만큼만 
	for( int i=0; i<nEventCount; ++i )
	{
		vpEventTime.PushBack( pHitter->GetProp_Animation()->CalEventTime( dwAniID, pEventArray->GetData( i ), fAccelRate ) );
	}

	//Setting Skill Info
	CsC_AttackProp::sHIT_INFO info;
	info.s_eHitType = CsC_AttackProp::HT_Skill;
	info.s_dwAniID = dwAniID;	
	info.s_fAniAccelRate = fAccelRate;
	info.s_pPartnerDamageInfo = NULL;
	info.s_pHitTarget = NULL;
	info.s_fRangeVelocity = 1.0f;//pFTSkillInfo->s_fSkill_Velocity;
	info.s_fRangeAccelVelocity = 1.0f;//pFTSkillInfo->s_fSkill_Accel;
	info.s_bRangeAttack = false;//pHitterInfo->s_bRangeAttack;
	info.s_bRotation = true;
	info.s_nAttackEffectIndex = 0;

	switch( g_pResist->m_Global.s_eAttackEffectViewMode )
	{
	case cResist::sGLOBAL::AEM_ALL_OFF:			// 랜더링 자체를 안하겠다
		info.s_nAttackEffectIndex = -1;
		break;
	case cResist::sGLOBAL::AEM_ONLY_NORATT_ON:	// 일반 타격이펙트만 랜더링
		info.s_nAttackEffectIndex = 0;
		break;
	}

	info.s_vDamagePos = pHitter->GetPos();

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
	{
		info.s_fRangeVelocity *= BATTLE_SPEED_RANGE;
		info.s_fRangeAccelVelocity *= BATTLE_SPEED_RANGE;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// setting Animation 
	pHitter->GetProp_Attack()->InsertHit( &info );





//Cool Time 등 셋팅
	if(pHitter != NULL)
		pHitter->RecvSkill( true, nSkillArrIDX );	

}
void cCliGame::_RecvDotSkillAround()
{
	u4 nHitTarget = 0;
	u4 nTarget = 0;		// 적용 받는 대상 UID
	u1 nTotalCount = 0;	// 적용 받은 개수

	u1 nParam = 0;
	u2 nBuffCode = 0;
	n4 nApplyValue = 0;
	u1 nTargetHpRate = 0;

	pop(nTarget);
	pop(nTargetHpRate);
	pop(nTotalCount);
	for(int i = 0; i < nTotalCount; i++)
	{	
		pop(nHitTarget);
		pop(nBuffCode);		// 효과를 입힌 버프 코드
		pop(nParam);		// 변경 파라미터
		pop(nApplyValue);	// 효과 받은 값		

		CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTarget );
		if( pTarget == NULL )
		{
			return;
		}		

		sRECV_TARGET_INFO target;
		target.s_nUID = nTarget;
		target.s_DamageType = ( nApplyValue >= 0 ) ? CsC_AttackProp::DT_Recover : CsC_AttackProp::DT_Damage_Dot;				
		target.s_nNumEffectType = NUMTYPE::ET_NORMAL;		
		target.s_nValue[ 0 ] = nApplyValue;

		sRECV_HITTER_INFO hi;
		hi.s_nUID = nHitTarget;
		hi.s_nSkillIndex = nBuffCode;
		hi.s_HitType = CsC_AttackProp::HT_Normal;

		switch( nParam )
		{
		case APPLY_HP:
			{	
				if( pTarget->GetLeafRTTI() == RTTI_DIGIMON_USER ||
					pTarget->GetLeafRTTI() == RTTI_TAMER_USER )
				{
					target.s_nValue[ 1 ] = nApplyValue;	
				}
				else
				{
					target.s_nValue[ 1 ] = nTargetHpRate - pTarget->GetBaseStat()->GetBackupHP();
					pTarget->GetBaseStat()->SetBackupHP( nTargetHpRate );			
				}
			}
			break;
		case APPLY_DS:
			{
				if( pTarget->GetLeafRTTI() != RTTI_DIGIMON_USER )
					return;
				
				target.s_nValue[ 2 ] = nApplyValue;
			}
			break;
		}		
		SetDotInfo( &hi, &target, nBuffCode, false );
	}
	
}


void cCliGame::RecvSkillApply(void)
{
	assert_csm( false, _T( "이제 여기 들어오면 안된다. 송명수 호출" ) );

// 	u2 nHitterUID;
// 	u2 nTargetUID;		// target's uid
// 	u4 nSkillArrIDX;	// 
// 	u1 nHpRate = 0;
// 
// 	pop(nHitterUID);	// 약물 아닌 스킬로 적용 받는 경우
// 	pop(nTargetUID);
// 	pop(nSkillArrIDX);	
// 
// 	if(nHitterUID)
// 	{
// 		pop(nHpRate);
// 	}
// 
// 	u1 nP;				// parameter
// 	n2 nV;				// value
// 
// 
// 	pop(nP);
// 
// 	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
// 
// 	// 약물	
// 	if( nHitterUID == 0 )
// 	{
// 		// 타겟도 널이라면 히터도 없기 떄문에 그냥 리턴
// 		if( pTarget == NULL )
// 			return;
// 
// 		while(nP < MaxExtStat)
// 		{
// 			pop(nV);
// 			nV = -nV;
// 
// 			switch( nP )
// 			{
// 			case HP:
// 				{
// 					pTarget->GetBaseStat()->SetDeltaHP( -nV );
// 					pTarget->GetBaseStat()->SetDeltaBackupHP( -nV );
// 				}								
// 				break;
// 			case DS:
// 				{
// 					pTarget->GetBaseStat()->SetDeltaDS( -nV );
// 					pTarget->GetBaseStat()->SetDeltaBackupDS( -nV );
// 				}				
// 				break;
// 			case FP:
// 				{
// 					pTarget->GetBaseStat()->SetDeltaFP( -nV );
// 				}				
// 				break;
// 			default:
// 				assert_csm1( false, _T( "RevSkillApply Parameter = %d" ), nP );
// 			}
// 
// 			pop( nP );
// 		}
// 	}
// 	else
// 	{
// 		assert_csm1( nSkillArrIDX < nLimit::Skill, _T( "nSkillArrayIdx = %d" ), nSkillArrIDX );
// 
// 		sRECV_HITTER_INFO hitter;
// 		hitter.s_nUID = nHitterUID;	
// 		hitter.s_nSkillIndex = nSkillArrIDX;
// 		hitter.s_HitType = CsC_AttackProp::HT_Skill;
// 
// 		sRECV_TARGET_INFO target;
// 		target.s_nUID = nTargetUID;
// 		target.s_DamageType = CsC_AttackProp::DT_Normal;	
// 		target.s_nNumEffectType = NUMTYPE::ET_NORMAL;
// 
// 		while(nP < MaxExtStat)
// 		{
// 			pop(nV);
// 			nV = -nV;
// 
// 			assert_csm( nV <= 0, _T( "힐링업 스킬은 아직 안되어 있다." ) );
// #ifdef _DEBUG
// 			if( target.s_DamageType == CsC_AttackProp::DT_Miss )
// 			{
// 				assert_csm( nV == 0, _T( "하나의 값이 미스라면 그 발류값들은 전부 미스여야 한다." ) );
// 			}
// #endif
// 			target.s_nValue[ 0 ] = nV;
// 
// 			// 미스
// 			if( nV == 0 )
// 			{				
// 				hitter.s_HitType = CsC_AttackProp::HT_Miss;
// 				target.s_DamageType = CsC_AttackProp::DT_Miss;
// 			}
// 			// 데미지
// 			else
// 			{
// 				switch( nP )
// 				{
// 				case HP:
// 					{
// 						if( pTarget )
// 						{
// 							target.s_nValue[ 1 ] = nHpRate - pTarget->GetBaseStat()->GetBackupHP();
// 							pTarget->GetBaseStat()->SetBackupHP( nHpRate );
// 						}
// 					}
// 					break;
// 				default:
// 					assert_csm1( false, _T( "RevSkillApply Parameter = %d" ), nP );
// 				}
// 			}
// 			pop( nP );
// 		}
// 
// 		SetSkillInfo( &hitter, &target, false );
// 	}
}



void cCliGame::RecvSkillRepeat(void)
{
	assert_csm( false, _T( "여기 들어오면 안되는데. 송명수를 호출해 주세요.!!" ) );

// 	u2 nUID;		// target's uid
// 	u4 nSkillIDX;	// 
// 
// 	pop(nUID);
// 	pop(nSkillIDX);
// 
// 	u1 nP;	// parameter
// 	u2 nV;	// value
// 
// 	pop(nP);
// 	while(nP <= AR)
// 	{
// 		pop(nV);
// 
// 		// apply
// 		if(nP == HP)
// 		{
// 			/*int nType;
// 			CObject* pObject = g_pWorldMgr->FindObject(nUID, nType);
// 			if(!pObject)
// 			{
// 				return;
// 			}
// 
// 			if(nType == OBJECT::ITEM)
// 			{
// 				net::chat("RecvHealUp EEE obj is item");
// 				return;
// 			}
// 			
// 
// 			int nAdd = nV - ((CCharacter*)pObject)->m_nHP;
// 
// 			((CCharacter*)pObject)->m_nHP = nV;
// 			net::chat("uid:%d hp +%d, ds +%d", nUID, nAdd, 0);
// 
// 			//g_PlayerUser.m_pDigimonAttribute[nP] = nV;
// 
// 			*/
// 		}
// 
// 		pop(nP);
// 	}
}


void cCliGame::RecvSkillError(void)
{
	n1 nParameter;
	u2 nValue;
	u2 nValue2;

	pop(nParameter);
	pop(nValue);	

	switch(nParameter)
	{
	case -1		:
		{
			pop(nValue2);
#ifdef _DEBUG
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString(_T( "스킬 거리 오류(요구치:%d, 현재:%d)" ), nValue, nValue2);
			DmCS::StringFn::Format(CProtocol.m_wStr, _T( "스킬 거리 오류(요구치:%d, 현재:%d)" ), nValue, nValue2);
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
#endif //_DEBUG

			SAFE_POINTER_RET( g_pDataMng );
			SAFE_POINTER_RET( g_pCharMng );
			cData_ServerSync* pSync = g_pDataMng->GetServerSync();
			SAFE_POINTER_RET( pSync );

			if( pSync->IsEmptyRefCount( cData_ServerSync::USE_DIGIMON_SKILL ) )
				return;

			int nSkillIndex = pSync->GetSyncData( cData_ServerSync::USE_DIGIMON_SKILL )->s_nEtcValue1;

			pSync->RecvServer( cData_ServerSync::USE_DIGIMON_SKILL, 0 );

			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
			SAFE_POINTER_RET( pDigimonUser );
			cSkill* pSkillMng = pDigimonUser->GetSkillMng();
			SAFE_POINTER_RET( pSkillMng );

			CsSkill* pSkillFT = pSkillMng->GetFTSkill( nSkillIndex );
			SAFE_POINTER_RET( pSkillFT );

			CsSkill::sINFO* pSkillInfos = pSkillFT->GetInfo();
			SAFE_POINTER_RET( pSkillInfos );

			CsC_AttackProp* pAttProp = pDigimonUser->GetProp_Attack();
			SAFE_POINTER_RET( pAttProp );

			CsC_AvObject* pTarget = pAttProp->GetFollowTarget();
			SAFE_POINTER_RET( pTarget );

			float fCurDist = pAttProp->GetEnableFollowDist();
			fCurDist -= pTarget->GetAttackCharSize();
			if( (fCurDist - 20.0f) > 0)
				fCurDist -= 20.0f;
			else
				fCurDist = 0;

			bool bRange = true;
			if( pSkillInfos->s_nAttType == 1 )
				bRange = false;
			pAttProp->ReleaseFollowTarget();
			pAttProp->SetFollowTarget( pTarget, fCurDist, (CsC_AttackProp::eFOLLOW_ITERATOR)( CsC_AttackProp::FI_SKILL1 + nSkillIndex ), bRange );

			// 움직임 가능하게		
			pDigimonUser->SetPauseAll( false );
			pDigimonUser->ReleaseSrvSendSkill();
			pSkillMng->__UseSkill( nSkillIndex );
			return;
		}
		break;
	case null	: 
		break;
	case 1		:
#pragma todo("자신의 거주 영역을 벗어나서 되돌아가고 있는 경우, 스킬 사용 불가")
		break;
	case 99		:
		xassert(false, "해당 스킬이 이미 사용중에 있습니다.");
		break;
	case HP		:
		xassert(false, "hp부족");
		break;
	case DS		:
		xassert(false, "ds부족");		
		break;
	case 100	:
		cPrintMsg::PrintMsg( 30554 );
		if (nValue == 1)// 0 침묵 디버프 1 사성수 던전 지역제한 20170208
		{
			TamerSkillErrInEpicDungeon();
			return;
		}
		break;
	default		:
		xassert1(false, "parameter(%d) error", nParameter);
		assert_csm( false, _T( "여기 들어오면 안되는데. 송명수를 호출해 주세요.!!, 알수 없는 파라미터" ) );
	}

	g_pCharMng->GetDigimonUser( 0 )->RecvSkill( false, 0 );
}

void cCliGame::RecvTamerSkillSuccess(void)
{
	int nWindow = cSkill::NONE;	// 아이템 타입 Script 스킬 사용 시 윈도우 타입

	u4 dwSkillCode;
	u4 nSkill_EndTime;
	pop(dwSkillCode);		// 스킬 인덱스 번호
	pop(nSkill_EndTime);	// 스킬 종료 시간

	CsC_AvObject* pTarget = g_pCharMng->GetTamerUser();	// 테이머 정보 얻어옴

	int nSkill_Index = ((CTamer*)pTarget)->GetSkillMng()->GetTamerSkill_Index( dwSkillCode );

	cSkill::sINFO* pSkill_Info = ((CTamer*)pTarget)->GetSkillMng()->GetSkill( nSkill_Index );	// 사용한 테이머스킬 정보 얻어옴

	CsTamerSkill* pTamerSkill = nsCsFileTable::g_pSkillMng->GetTamer_Skill( -1, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );

	switch( pTamerSkill->Get_TSkill_Info()->s_nType )
	{
	case 1:		// 공격 스킬
		break;

	case 2:		// 버프 디버프 스킬
		pSkill_Info->s_CoolTime.Start();
		pSkill_Info->s_CoolTime.SetEndTime( pSkill_Info->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f );

		((CTamer*)pTarget)->GetSkillMng()->SetTamerSkill_Ani( pTamerSkill->Get_TSkill_Info()->s_dwTamer_SeqID, pTamerSkill->Get_TSkill_Info()->s_dwDigimon_SeqID );
		break;

	case 3:		// 스크립트 스킬
		switch( pTamerSkill->Get_TSkill_Info()->s_dwFactor1 )
		{
		case 1:		// 아이템 사용 방식의 스킬 (시공간 폭탄, 휴대용 디지몬 보관소 소환 등...)
			if( pTamerSkill->Get_TSkill_Info()->s_dwFactor2 == 6010 )	// 휴대용 디지몬 보관소
			{
				if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE ) == true )
					return;

				//g_pGameIF->GetDynamicIF( cBaseWindow::WT_TACTICSHOUSE );
				g_pCharMng->GetTamerUser()->DeletePath(); 

				std::pair<int, NiPoint2> pairData;
				pairData.first = 0;
				pairData.second = g_pCharMng->GetTamerUser()->GetPos2D();
				GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_SHOWVIEWER_BT_ITEM, &pairData);

				nWindow = cSkill::TACTICSHOUSE;
			}

			if( pTamerSkill->Get_TSkill_Info()->s_dwFactor2 == 9631 )	//응원
			{
				g_pCharMng->GetTamerUser()->DeletePath();
			}

			if( pTamerSkill->Get_TSkill_Info()->s_dwFactor2 == 9801 )
			{
				if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_JUMPBUSTER ) == false )	//점프부스터 
		
					g_pCharMng->GetTamerUser()->DeletePath();
					
					nWindow = cSkill::JUMPBOOSTER;

					break;
			}

			pSkill_Info->s_CoolTime.Start();
			pSkill_Info->s_CoolTime.SetEndTime( pSkill_Info->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f );

			((CTamer*)pTarget)->GetSkillMng()->SetTamerSkill_Ani( pTamerSkill->Get_TSkill_Info()->s_dwTamer_SeqID, pTamerSkill->Get_TSkill_Info()->s_dwDigimon_SeqID );

			//=============================================================================
			//	아이템 사용 방식의 스크립트 스킬 사용 후 종료 시 반드시 서버에 종료되는 시점을 알려줘야 하기 때문에 해당 정보 저장
			//=============================================================================
			((CTamer*)pTarget)->GetSkillMng()->SetTamerSkillMap( nWindow, pSkill_Info->s_pFTSkill->GetInfo()->s_dwID );
			break;

		case 2:		// 평화 선언 (초궁 라이딩 상태의 효과)
			pSkill_Info->s_CoolTime.Start();
			pSkill_Info->s_CoolTime.SetEndTime( pSkill_Info->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f );

			if( nSkill_EndTime != 0 /*&& pTamerSkill->Get_TSkill_Info()->s_dwSkillCode == 8000611*/ )
				((CTamer*)pTarget)->GetBuff()->SetBuff( 40148, 0, nSkill_EndTime, 1 );

			((CTamer*)pTarget)->GetSkillMng()->SetTamerSkill_Ani( pTamerSkill->Get_TSkill_Info()->s_dwTamer_SeqID, pTamerSkill->Get_TSkill_Info()->s_dwDigimon_SeqID );
			break;

		case 3:		// 테이머 소환
			pSkill_Info->s_CoolTime.Start();
			pSkill_Info->s_CoolTime.SetEndTime( pSkill_Info->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f );

			((CTamer*)pTarget)->GetSkillMng()->SetTamerSkill_Ani( pTamerSkill->Get_TSkill_Info()->s_dwTamer_SeqID, pTamerSkill->Get_TSkill_Info()->s_dwDigimon_SeqID );
			break;
		}
		break;

	default:
		assert_cs( false );
	}
}

void cCliGame::RecvTamerSummon()
{
	u1		nCase, nValue;
	u4		nIndex;
	uint	nMapID;
	CsMapList* pTempMap = NULL;
	
	pop(nCase);

	if( nCase != 5 )
		pop(nValue);

	std::wstring szTransTamerName;

	switch(nCase)
	{
	case 1:		// 테이머 소환 상태 체크
		if( nValue == 1 )	// 0 소환 불가능, 1 소환 가능
		{
			if( g_pTamerSummon != NULL )
			{
				g_pTamerSummon->OnCancel();
				g_pTamerSummon = NULL;
			}
			pop(szTransTamerName);
			pop( nIndex );	// Tamer_Skill.xls 의 인덱스 번호

			cMiniMap* pMiniMap = g_pGameIF->GetMiniMap();
			cPrintMsg::PrintMsg( 30548, const_cast<TCHAR*>(szTransTamerName.c_str()), pMiniMap->Get_MapName() );
			cMessageBox::GetFirstMessageBox()->SetValue1( nIndex );	// Tamer_Skill.xls 의 인덱스 번호
		}
		else if( nValue == 0 )
		{
			if( g_pTamerSummon != NULL )
			{
				g_pTamerSummon->OnCancel();
				g_pTamerSummon = NULL;
			}
			cPrintMsg::PrintMsg( 30546 );
		}
		else if( nValue == 2 )
			cPrintMsg::PrintMsg( 30555, const_cast<TCHAR*>( UISTRING_TEXT( "MESSAGE_SUMMON_FRIEND" ).c_str() ) );
		break;

	case 2:		// 소환 스킬을 사용한 유저에게 보여줄 메세지
		if( nValue == 1)
		{
			pop(szTransTamerName);
			cMiniMap* pMiniMap = g_pGameIF->GetMiniMap();
			cPrintMsg::PrintMsg( 30549, const_cast<TCHAR*>(szTransTamerName.c_str()), pMiniMap->Get_MapName() );
		}
		else if( nValue == 0)
		{
			cMessageBox::DelMsg(30549, false);
			cPrintMsg::PrintMsg(30551);	// 소환 요청이 취소되었습니다.
		}
		else if( nValue == 2 )	// 소환 당할 유저가 수락 하였을때 보여지는 메세지 삭제...
			cMessageBox::DelMsg(30549, false);
		break;

	case 3:		// 소환 당할 유저에게 보여줄 메세지
		if( nValue )
		{
			pop(nMapID);
			pop(szTransTamerName);
			pTempMap = nsCsMapTable::g_pMapListMng->GetList(nMapID);
			std::wstring wsMapDiscript = pTempMap->GetInfo()->s_szMapDiscript;
			cPrintMsg::PrintMsg( 30550, const_cast<TCHAR*>(szTransTamerName.c_str()),  const_cast<TCHAR*>(wsMapDiscript.c_str()) );
		}
		else
		{
			cMessageBox::DelMsg(30550, false);
			cPrintMsg::PrintMsg(30551);	// 소환 요청이 취소되었습니다.
		}
		break;

	case 4:
		if( nValue == 1)
		{
			bool bIsBattleRegisted = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_BATTLEMATCH_REGISTED, &bIsBattleRegisted );

			// 현재 상황에서는 거래가 되지 않는다. ( Npc이용, 거래중 )
			if( cMessageBox::IsMessageBox() || g_pGameIF->IsOpenNpcWindow() || bIsBattleRegisted /*|| g_pDataMng->GetServerSync()->IsChageDigimon()*/ )
			{
				net::game->SendTamerSummon_StateCheck(4, 1, 0);
			}
			else
				net::game->SendTamerSummon_StateCheck(4, 1, 1);
		}
		else
			assert_cs( false );
		break;

	case 5:
		if( g_pTamerSummon != NULL )
		{
			g_pTamerSummon->OnCancel();
			g_pTamerSummon = NULL;
		}
		cPrintMsg::PrintMsg(30552);		// 상대방이 친구소환 요청을 받을 수 없는 상태입니다.
		break;

	default:
		assert_cs( false );
	}
}

void cCliGame::RecvTamerCashSkillUseResult(void)		// 테이머 캐쉬 스킬 아이템 사용 후 결과 값
{
	u4 nSkillIdx;		// 캐쉬 스킬 인덱스 (Tamer_Skill.xls 의 스킬 인덱스)
	u4 nLifeTime;		// 만료 시간

	pop( nSkillIdx );
	pop( nLifeTime );

	cSkill* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng();
	if( pSkill )
	{
		pSkill->SetTamerCashSkill( nSkillIdx, 0, nLifeTime );
		CsTamerSkill* pSkill_Info = nsCsFileTable::g_pSkillMng->GetTamer_Skill( nSkillIdx, -1 );
		if( pSkill_Info )
		{
			CsSkill* pTotalSkill = nsCsFileTable::g_pSkillMng->GetSkill(pSkill_Info->Get_TSkill_Info()->s_dwSkillCode );
			if( pTotalSkill )
				cPrintMsg::PrintMsg( 30685, pTotalSkill->GetInfo()->s_szName );
		}		
	}		
}

void cCliGame::RecvTamerCashSkillExpire(void)		// 테이머 캐쉬 스킬 만료 시
{
	u1 nExpireCount;
 	u4 dwSkill_Index;	// Tamer_Skill.xls 의 인덱스
 
 	pop( nExpireCount );
 
 	cSkill* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng();
 	for( int i=0; i<nExpireCount; ++i )
 	{
 		pop( dwSkill_Index );

 		pSkill->DelTamerCashSkill( dwSkill_Index );
		CsTamerSkill* pSkill_Info = nsCsFileTable::g_pSkillMng->GetTamer_Skill( dwSkill_Index, -1 );
		CsSkill* pTotalSkill = nsCsFileTable::g_pSkillMng->GetSkill(pSkill_Info->Get_TSkill_Info()->s_dwSkillCode );
		cPrintMsg::PrintMsg( 20029, pTotalSkill->GetInfo()->s_szName );
 	}
}

void cCliGame::RecvTamerCashSkillRemove(void)		// 테이머 캐쉬 스킬 삭제 시
{
	u1 nExpireCount;
	u4 dwSkill_Index;	// Tamer_Skill.xls 의 인덱스

	pop( nExpireCount );

	cSkill* pSkill = g_pCharMng->GetTamerUser()->GetSkillMng();
	for( int i=0; i<nExpireCount; ++i )
	{
		pop( dwSkill_Index );

		pSkill->DelTamerCashSkill( dwSkill_Index );

		// 사용 스킬 이름을 알기 위한 포인터
		CsTamerSkill* pSkill_Info = nsCsFileTable::g_pSkillMng->GetTamer_Skill( dwSkill_Index, -1 );
		CsSkill* pTotalSkill = nsCsFileTable::g_pSkillMng->GetSkill(pSkill_Info->Get_TSkill_Info()->s_dwSkillCode );
		cPrintMsg::PrintMsg( 30584, pTotalSkill->GetInfo()->s_szName );
	}
}

// 디지몬 광역 스킬
void cCliGame::_RecvDigimonSkill_ApplyAround()
{
	// 스킬
	u4 nHitterUID = 0;
	u4 nTargetUID = 0;
	u2 nTargetCount = 0;	// 스킬에 맞은 대상 수
	u4 nSkillArrIDX = 0;
	
	pop(nHitterUID);
	pop(nTargetCount);

	u1 bIsMemory = 0;		// 메모리 스킬 구분 여부 ( 1 : 메모리 광역 스킬, 0 : 일반 광역 스킬 )
	pop( bIsMemory );

	pop(nSkillArrIDX);	// 메모리 스킬 컨텐츠가 들어간 상태에서는 bIsMemory 가 true면 스킬코드, false이면 스킬IDX를 받는다.


	u1 nHitterHpRate = 0;
	pop( nHitterHpRate );

 	CsC_AvObject* pHitter = g_pMngCollector->GetObject( nHitterUID );
	if( pHitter == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		CProtocol.m_wStr = L"UID 없음";
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	sRECV_HITTER_INFO hitter;
	ZeroMemory( &hitter, sizeof(sRECV_HITTER_INFO) );
	hitter.s_nUID = nHitterUID;	
	if( bIsMemory )
	{
		hitter.s_nSkillIndex = ((CDigimon*)pHitter)->GetSkillMng()->GetMySkill_Index( nSkillArrIDX );
		hitter.s_AniID = ANI::ATTACK_ATTACK1;

		if( hitter.s_nSkillIndex == -1 )
			hitter.s_nSkillCode = nSkillArrIDX;		// 싱크를 맞추는 유저는 공격형 메모리 스킬을 사용한 유저가 어떤 스킬을 사용했는지 알 수 없기 때문에 스킬 코드를 저장.

		if( pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER )
		{
			cSkill::sINFO* pSkill_Info = g_pCharMng->GetDigimonUser(0)->GetSkillMng()->GetSkill( hitter.s_nSkillIndex );	// 사용한 메모리 스킬 정보 얻어옴

			// 메모리 칩 감소
			g_pDataMng->GetInven()->DecreaseItem_TypeLS( nItem::DigimonSkillMemoryChip, 0, pSkill_Info->s_pFTSkill->GetInfo()->s_nReq_Item, false, false );
		}
	}
	else
		hitter.s_nSkillIndex = nSkillArrIDX;

	hitter.s_HitType = CsC_AttackProp::HT_Skill;

	// Hitter 변경 파라메타
	u1 nP;		// parameter
	n4 nV;		// value

	n4 nHitterAttribute[ MaxExtStat ] = {0, };


	//=============================================================================
	// 스킬 맞는 대상 Data 처리
	//=============================================================================
	CsC_AvObject* pMark = NULL;
	// 때리는 넘이 내 디지몬이라면
	if( pHitter != NULL )
	{
		if( pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER )
		{
			// 본인이 히터고, 타겟이 다른 유저에게 죽지 않았다면...
			if( g_pCharMng->GetDigimonUser(0)->GetProp_Attack()->GetFollowTarget() != NULL )
				pMark = g_pMngCollector->GetObject( g_pCharMng->GetDigimonUser(0)->GetProp_Attack()->GetFollowTarget()->GetUniqID() );
		}
	}

	bool bNoneTarget = false;	// 본인이 히터고, 타겟이 다른 유저에게 죽었을 때 다른 타겟을 정하기 위한 flag.
	bool bSyncAni	 = false;	// 다른 유저가 광역 스킬을 사용하였을때 타겟 정보가 없어 광역 스킬 애니가 안나오는 현상 관련 임의 타겟 지정 flag
	bool bMaxCount	 = false;	// 본인이 히터고 타겟카운트 마지막까지 실제 스킬을 사용한 대상이 없을 때 타겟을 정하기 위한 flag


	for( int i=0; i<nTargetCount; ++i )
	{
		pop( nTargetUID );
		sRECV_TARGET_INFO target;
		target.s_nUID = nTargetUID;	
		target.s_DamageType =  CsC_AttackProp::DT_Normal;
		target.s_nNumEffectType = NUMTYPE::ET_NORMAL;

		u1 targethprate = 0;
		pop( targethprate );

		n4 targetAttribute[ MaxExtStat ] = { 0, };
		pop( nP );
		while( nP < MaxExtStat )
		{
			pop( nV );
			targetAttribute[ nP ] = nV;
			pop( nP );
		}

		target.s_nValue[ 0 ] = targetAttribute[ HP ];
		// 미스
		if( targetAttribute[ HP ] == 0 )
		{
			hitter.s_HitType = CsC_AttackProp::HT_Miss;
			target.s_DamageType = CsC_AttackProp::DT_Miss;
		}
		// 데미지
		else
		{
			CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
			if( pTarget )
			{
				if( pTarget->GetLeafRTTI() != RTTI_DIGIMON_USER )
				{
					target.s_nValue[ 1 ] = targethprate - pTarget->GetBaseStat()->GetBackupHP();
					pTarget->GetBaseStat()->SetBackupHP( targethprate );
				}
				else
				{
					pTarget->GetBaseStat()->SetDeltaHP( targetAttribute[ HP ] );
					pTarget->GetBaseStat()->SetDeltaBackupHP( targetAttribute[ HP ] );
				}
			}
		}

		// 히터가 몬스터를 대상으로 스킬을 사용하였을때 제 3자의 싱크에서 히터가 실제 스킬을 사용한 대상이 보이지 않을 때
		// 임의의 타겟을 정해서 애니메이션을 보여주기 위함.
		if( pHitter != NULL && pMark == NULL )
		{
			if( pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER && bNoneTarget == false )
			{
				pMark = g_pMngCollector->GetObject( target.s_nUID );
				bNoneTarget = true;
			}
			else if( bSyncAni == false )
			{
				pMark = g_pMngCollector->GetObject( target.s_nUID );
				if( pMark != NULL )	// 보이는 타겟 data만 받는게 아니라 범위내의 모든 타겟 정보를 받기 때문에 보이는 넘을 가려내기 위함
					bSyncAni = true;
			}
		}

		// 본인이 히터일때 맥스 타겟카운트까지 돌았는데 실제 스킬을 사용한 타겟을 찾을 수 없을 때 (타겟이 범위를 벗어났을 때)
		// 스킬 사용 대상 정보가 꼭 있어야 함.
		if( i == (nTargetCount - 1) && bMaxCount == false && pMark != NULL )
		{
			// 마지막 남은 타겟과 실제 스킬을 사용한 타겟 정보가 일치하지 않다면....
			if( target.s_nUID != pMark->GetUniqID() )
				SetSkillInfo( &hitter, &target, pMark, false, bMaxCount, true );
			else
				SetSkillInfo( &hitter, &target, pMark, false, bMaxCount, false );
		}
		else
			SetSkillInfo( &hitter, &target, pMark, false, bMaxCount, false );

// 		if( pMark != NULL )
// 			g_pCharResMng->SetTargetMark( pMark );
	}


	//=============================================================================
	// 죽은 대상 Data 처리
	//=============================================================================
	u2	nDeathCount = 0;		// 죽은 대상 수
	u4	nDeathTargetUID = 0;	// 죽는넘
	n4	nDeathDamage = 0;		// 데미지

	pop( nDeathCount );


	sRECV_HITTER_INFO hi;
	ZeroMemory( &hi, sizeof( sRECV_HITTER_INFO ) );
	hi.s_nUID = nHitterUID;

	if( bIsMemory )
	{
		hi.s_nSkillIndex = ((CDigimon*)pHitter)->GetSkillMng()->GetMySkill_Index( nSkillArrIDX );
		hi.s_AniID = ANI::ATTACK_ATTACK1;

		if( hi.s_nSkillIndex == -1 )
			hi.s_nSkillCode = nSkillArrIDX;		// 싱크를 맞추는 유저는 공격형 메모리 스킬을 사용한 유저가 어떤 스킬을 사용했는지 알 수 없기 때문에 스킬 코드를 저장.
	}
	else
		hi.s_nSkillIndex = nSkillArrIDX;

	hi.s_HitType = CsC_AttackProp::HT_Skill;

	bNoneTarget = false;	// 본인이 히터고, 타겟이 다른 유저에게 죽었을 때 다른 타겟을 정하기 위한 flag.
	bSyncAni	= false;	// 다른 유저가 광역 스킬을 사용하였을때 타겟 정보가 없어 광역 스킬 애니가 안나오는 현상 관련 임의 타겟 지정 flag

	for( int i=0; i<nDeathCount; ++i )
	{
		pop( nDeathTargetUID );
		pop( nDeathDamage );

		CsC_AvObject* pTarget = g_pMngCollector->GetObject( nDeathTargetUID );
		if( pTarget == NULL )
			continue;


		sRECV_TARGET_INFO ti;
		ti.s_nUID = nDeathTargetUID;
		ti.s_DamageType = CsC_AttackProp::DT_Normal;
		ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;
		ti.s_nValue[ 0 ] = -nDeathDamage;
		ti.s_nValue[ 1 ] = -pTarget->GetBaseStat()->GetBackupHP();

		// 히터가 몬스터를 대상으로 스킬을 사용하였을때 제 3자의 싱크에서 히터가 실제 스킬을 사용한 대상이 보이지 않을 때
		// 임의의 타겟을 정해서 애니메이션을 보여주기 위함.
		if( pHitter != NULL && pMark == NULL )
		{
			if( pHitter->GetLeafRTTI() == RTTI_DIGIMON_USER && bNoneTarget == false )
			{
				pMark = g_pMngCollector->GetObject( ti.s_nUID );
				bNoneTarget = true;
			}
			else if( bSyncAni == false )
			{
				pMark = g_pMngCollector->GetObject( ti.s_nUID );
				if( pMark != NULL )
					bSyncAni = true;
			}
		}

		// 본인이 히터일때 맥스 타겟카운트까지 돌았는데 실제 스킬을 사용한 타겟을 찾을 수 없을 때 (타겟이 범위를 벗어났을 때)
		// 스킬 사용 대상 정보가 꼭 있어야 함.
		if( i == (nDeathCount - 1) && bMaxCount == false && pMark != NULL )
		{
			// 마지막 남은 타겟과 실제 스킬을 사용한 타겟 정보가 일치하지 않다면....
			if( ti.s_nUID != pMark->GetUniqID() )
				SetSkillInfo( &hi, &ti, pMark, true, bMaxCount, true );
			else
				SetSkillInfo( &hi, &ti, pMark, true, bMaxCount, false );
		}
		else
			SetSkillInfo( &hi, &ti, pMark, true, bMaxCount, false );

		SetDieInfo( pTarget );
	}
}

void cCliGame::RecvDotSkillSuccess()
{
	u1		nHpRate		= 0;
	u4		nHitterUID	= 0;	// 공격자 UID
	u4		nTargetUID	= 0;	// 타겟 UID
	u2		nBuffCode	= 0;	// 버프 코드
	int		nDamage		= 0;	// 적용된 도트 데미지
	bool	bDie	= false;	// 죽었냐? 살았냐?

	pop( nHitterUID );
	pop( nTargetUID );
	pop( nBuffCode );
	pop( nHpRate );
	pop( nDamage );
	pop( bDie );

	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
	if( pTarget == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T("타겟을 찾지 못찾음( nTargetUID - %d )"), nTargetUID );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("타겟을 찾지 못찾음( nTargetUID - %d )"), nTargetUID);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	if( nBuffCode == 0 )
		return;

	CsBuff::sINFO* pBuffInfo = nsCsFileTable::g_pBuffMng->GetBuff( nBuffCode )->GetInfo();
	if( NULL == pBuffInfo )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T("버프코드 못찾음( BuffCode - %d )"), nBuffCode);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("버프코드 못찾음( BuffCode - %d )"), nBuffCode);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	u4 nSkillCode = pBuffInfo->s_dwSkillCode;
	if( nSkillCode == 0 )
		return;

	CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode );
	if( NULL == pSkill )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T("스킬 못찾음( SkillCode - %d )"), nSkillCode );
		DmCS::StringFn::Format(CProtocol.m_wStr,  _T("스킬 못찾음( SkillCode - %d )"), nSkillCode );
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	CsSkill::sINFO* pSkillInfo = pSkill->GetInfo();
	if( NULL == pSkillInfo )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T("스킬 정보 못찾음( SkillCode - %d )"), nSkillCode);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("스킬 정보 못찾음( SkillCode - %d )"), nSkillCode);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	int nDamageResult = 0;
	bool bAddEffect = true;
	NUMTYPE::eType numEffType = NUMTYPE::DIGIMON_ATTACK;
	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
		{
			nDamageResult = -nDamage;
		}break;
	case RTTI_MONSTER:
		{
			numEffType = NUMTYPE::MONSTER_ATTACK;

			CsC_AvObject::sBASE_STAT* pBaseStat = pTarget->GetBaseStat();
			if( pBaseStat )
				nDamageResult = nHpRate - pBaseStat->GetBackupHP();
		}		
		break;
	}


	// 데미지량 띄워주자
	if( nDamage > 0 )
	{
		NiPoint3 vPos = pTarget->GetPos();
		vPos.z += pTarget->GetToolHeight() - 100.0f;
		g_pNumber3DMng->Add( nDamage, vPos, CsD2R( rand()%70 - 35 ), numEffType, NUMTYPE::ET_NORMAL );
	}

	CsC_AvObject::sBASE_STAT* pBaseStat = pTarget->GetBaseStat();
	if( pBaseStat )
	{
		pBaseStat->SetDeltaHP( nDamageResult );
		pBaseStat->SetDeltaBackupHP( nDamageResult );
	}

	// 죽었다면
	if( bDie )
		SetDieInfo( pTarget );	

	CsC_AttackProp::eDAMAGE_TYPE eDamageType = CsC_AttackProp::DT_Damage_Dot;

	//스킬 효과를 보고 스킬 확인
	for( int i = 0 ; i < SKILL_APPLY_MAX_COUNT ; i++ )
	{
		USHORT nApplyType = pSkillInfo->s_Apply[ i ].s_nA;
		switch( nApplyType )
		{
		case APPLY_DR:	// 데미지 반사
			{
				bAddEffect = false;
			}
			break;
		case APPLY_AB:	// 데미지 흡수
			{
				bAddEffect = false;
				eDamageType = CsC_AttackProp::DT_Absorb;
			}
			break;
		}
	}

	if( bAddEffect )
	{
		if( NULL != pBuffInfo->s_szEffectFile && strlen( pBuffInfo->s_szEffectFile ) > 0 )
		{
			// 이펙트를 띄워주자.
			char cEffectPath[ MAX_PATH ] = {0, };
			if( pBuffInfo->s_nBuffType == 2 )
				sprintf_s( cEffectPath, MAX_PATH, "system\\DeBuff\\%s.nif", pBuffInfo->s_szEffectFile );
			else
				sprintf_s( cEffectPath, MAX_PATH, "system\\Buff\\%s.nif", pBuffInfo->s_szEffectFile );

			CsC_EffectProp* pEffProp = pTarget->GetProp_Effect();
			if( pEffProp )
				pEffProp->AddEffect( cEffectPath, 4.0f, nsEFFECT::POS_CHARPOS );
		}
	}

	sRECV_HITTER_INFO hitter;
	hitter.s_nUID = nHitterUID;	
	hitter.s_HitType = CsC_AttackProp::HT_Normal;
	hitter.s_nSkillIndex = nBuffCode;

	sRECV_TARGET_INFO target;
	target.s_nUID = nTargetUID;
	target.s_DamageType = eDamageType;
	target.s_nNumEffectType = NUMTYPE::ET_NORMAL;
	target.s_nValue[ 0 ] = -nDamage;

	if( g_pGameIF->GetChatBattle() != NULL )
	{
		ContentsStream kStream;
		float nRemainTime=0.0f;
		int nTotalDem = abs(nDamage);

		kStream << hitter << target << nRemainTime << nTotalDem;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_BATTLE_PROCESS, &kStream );
	}

#ifdef DAMAGE_METER
	sDamageMeterInfo damageInfo(nHitterUID, nTargetUID, nDamage);
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ADD_DAMAGEMETER_DATA, &damageInfo );
// 	if( g_pDataMng->GetParty()->IsJoinParty() == true )
// 	{
// 		if( g_pGameIF->GetMainDigimonWindow()->IsBattle() || nTargetUID == g_pCharMng->GetDigimonUser(0)->GetUniqID() )
// 		{
// 			cData_DamageMeter* pDamageMeter = g_pDataMng->GetDamageMeter();
// 			if( pDamageMeter == NULL )
// 				return;
// 			pDamageMeter->_SetDamageInfo( nHitterUID, nTargetUID, abs(nDamage), _TIME_TS );
// 		}
// 	}
#endif

}

void cCliGame::RecvActiveSkill()
{
	u4 nTamerUID = 0;		// 스킬 버프 대상 테이머 - 버프 시전자
	u4 nSkillCode = 0;		// 스킬 코드
	u2 nApplyIdx = 0;		// 스킬 효과 인덱스

	pop(nTamerUID);
	pop(nSkillCode);
	pop(nApplyIdx);

	// 테이머로 디지몬 찾기
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTamerUID );
	if( pTarget == NULL )
		return;

	CDigimon* pDigimon;
	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
	case RTTI_MONSTER:
		{
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString(_T("잘못된 대상( TamerUID - %d )"), nTamerUID );
			DmCS::StringFn::Format(CProtocol.m_wStr, _T("잘못된 대상( TamerUID - %d )"), nTamerUID);
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
			return;
		}
		break;
	case RTTI_TAMER:
		{
			uint nLikiDigimon = ( (CTamer*)pTarget )->GetDigimonLink();
			pDigimon = g_pCharMng->GetDigimon( ( (CTamer*)pTarget )->GetDigimonLink() );
		}
		break;
	case RTTI_TAMER_USER:
		{
			pDigimon = g_pCharMng->GetDigimonUser( 0 );
		}
		break;
	}

	if( pDigimon == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T("디지몬 못찾음( TamerUID - %d )"), nTamerUID );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("디지몬 못찾음( TamerUID - %d )"), nTamerUID);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	//스킬코드로 스킬 종류 확인
	CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode );
	if( pSkill == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T("스킬 못찾음( SkillCode - %d )"), nSkillCode );
		DmCS::StringFn::Format(CProtocol.m_wStr,  _T("스킬 못찾음( SkillCode - %d )"), nSkillCode );
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	if( nApplyIdx >= SKILL_APPLY_MAX_COUNT )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString( _T("스킬효과 인덱스 오류, 3보다 작아야함 ( ApplyIdx - %d )"), nApplyIdx );
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("스킬효과 인덱스 오류, 3보다 작아야함 ( ApplyIdx - %d )"), nApplyIdx );
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	//스킬 효과를 보고 효과별 동작
	USHORT nApplyType = pSkill->GetInfo()->s_Apply[ nApplyIdx ].s_nA;
	switch( nApplyType )
	{
	case APPLY_HP:// HP 회복
		{
			// HP 회복이펙트
			//int nSkillAtt = FMCommon::GetSkillAtt( nSkillCode, nCurSkillLevel, i );
			//g_pNumber3DMng->Add( nSkillAtt, g_pCharMng->GetDigimonUser(0)->GetPos(), CsD2R( rand()%70 - 35 ), NUMTYPE::DIGIMON_SKILL_HP, NUMTYPE::ET_NORMAL );
			//DWORD dwFlag = nsEFFECT::LIVE_ONCE | nsEFFECT::POS_CHARPOS;
			pDigimon->GetProp_Effect()->AddEffect( "System\\Buff\\Buff_HP_Absorb.nif", 1.0f, nsEFFECT::POS_CHARPOS );
			//사운드 추가
			pDigimon->PlaySound( SOUND_ACTIVE_HP_ABSROB );
		}
		break;
	case APPLY_DS:// DS 회복
		{
			//스킬 발동, Flag true로 ( 발동 시 발동시킨 스킬의 소모DS 감소 안시킴 )
			pDigimon->SetBuffSkillFlag( true );
			// DS 회복이펙트
			pDigimon->GetProp_Effect()->AddEffect( "System\\Buff\\Buff_DS_Absorb.nif", 1.0f, nsEFFECT::POS_CHARPOS );
			//사운드 추가
			pDigimon->PlaySound( SOUND_ACTIVE_DS_ABSROB );
		}
		break;
	default:	break; //미구현 효과
	}
}

void cCliGame::RecvStunSkillSuccess(void)
{
	//u2 nHitterUID;
	u4 nTargetUID = 0;
	u2 nBuffID = 0;
	u4 nSkillCode = 0;
	u4 nEndTime = 0;

	//pop( nHitterUID );
	pop( nTargetUID );
	pop( nSkillCode );
	pop( nBuffID    );
	pop( nEndTime   );

	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
	if( pTarget == NULL )
		return;


	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_DIGIMON:
	case RTTI_DIGIMON_USER:
		// 아직 디지몬에 사용되지 않으므로 미구현
		break;

	case RTTI_MONSTER:
		{
			CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode );
			
			if( pSkill == NULL )
				return;

			cBuffData* pBuff = ( (CMonster*)pTarget )->GetBuff();

			if( pBuff->IsBuffData( nBuffID ) )
			{
				pBuff->ReleaseBuff( nBuffID );
			}

			pBuff->SetOnePlayEffect(true);
			pBuff->SetBuff( nBuffID, nTargetUID, nEndTime, 1, nSkillCode ); // 버프 세팅
			pBuff->SetOnePlayEffect(false);

			pTarget->SetAniPause( true );
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			CProtocol.m_wStr = L"애니 정지";
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}
		break;;
	default:
		assert_cs( false );
	}
}

void cCliGame::RecvDigimonAddSkillSuccess()	// 디지몬 스킬 등록 성공
{
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = L"디지몬 스킬 등록 성공";
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	u4 nDigimonUID = 0;
	u4 nSkillCode = 0;
	u4 nItemIDX = 0;

	pop( nDigimonUID );
	pop( nSkillCode );
	pop( nItemIDX );

	CsC_AvObject* pObject = g_pMngCollector->GetObject( nDigimonUID );

	if( pObject == NULL )
		return;

	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemIDX );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	char cEffectPath[ MAX_PATH ] = {0, };

	switch( pObject->GetLeafRTTI() )
	{
	case RTTI_DIGIMON_USER:
		g_pCharMng->GetDigimonUser(0)->GetSkillMng()->SetDigimonCashSkill( nSkillCode, 0 );
		g_pCharMng->GetDigimonUser(0)->GetSkillMng()->AddDigimonCashSkill( nSkillCode, 0 );

		// 메모리 스킬 등록 이펙트 add
		sprintf_s( cEffectPath, MAX_PATH, "system\\Buff\\%s.nif", pFTInfo->s_cNif );
		//g_pCharMng->GetDigimonUser(0)->GetProp_Effect()->AddEffect( cEffectPath, sqrt( g_pCharMng->GetDigimonUser(0)->GetToolWidth()*0.02f ), nsEFFECT::POS_CHARPOS );
		g_pCharMng->GetDigimonUser(0)->GetProp_Effect()->AddEffect_FT( cEffectPath, sqrt( g_pCharMng->GetDigimonUser(0)->GetToolWidth()*0.02f ) );

		cPrintMsg::PrintMsg( 30685, nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode )->GetInfo()->s_szName );

		break;
	case RTTI_DIGIMON:
		// 메모리 스킬 등록 이펙트 add
		sprintf_s( cEffectPath, MAX_PATH, "system\\Buff\\%s.nif", pFTInfo->s_cNif );
		//pObject->GetProp_Effect()->AddEffect( cEffectPath, sqrt( pObject->GetToolWidth()*0.02f ), nsEFFECT::POS_CHARPOS );
		pObject->GetProp_Effect()->AddEffect_FT( cEffectPath, sqrt( pObject->GetToolWidth()*0.02f ) );

		break;
	default:
		assert_cs( false );
	}

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_SKILL_DIGIMONSTATUS, NULL );
}

void cCliGame::RecvDigimonSkillRemove()		// 디지몬 스킬 삭제 성공
{
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = L"디지몬 스킬 삭제 성공";
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	u4 nSkillCode = 0;
	pop( nSkillCode );

	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::MEMORY_SKILL_DELETE, 0 );

	if( nSkillCode == 0 )	// 잘못 된 스킬 값이 들어올 경우. 스킬 삭제 시도 안함.
		return;

	g_pCharMng->GetDigimonUser(0)->GetSkillMng()->DelDigimonCashSkill( nSkillCode );

	cPrintMsg::PrintMsg( 30688, nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode )->GetInfo()->s_szName );
	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_SKILL_DIGIMONSTATUS, NULL );
}

void cCliGame::RecvDigimonMemorySkill_UseSuccess()
{
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = L"디지몬 스킬 사용 성공" ;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	
	u4 nSkillCode = 0;
	u4 nCoolTime = 0;
	u4 nTargetUID = 0;

	pop( nSkillCode );
	pop( nCoolTime );
	
	//CsSkill* pInfo = nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode );

	SHORT nIndex = g_pCharMng->GetDigimonUser(0)->GetSkillMng()->GetMySkill_Index( nSkillCode );
	cSkill::sINFO* pSkill_Info = g_pCharMng->GetDigimonUser(0)->GetSkillMng()->GetSkill( nIndex );	// 사용한 메모리 스킬 정보 얻어옴

	// 쿨타임 시작
	pSkill_Info->s_CoolTime.Start();
	pSkill_Info->s_CoolTime.SetEndTime( pSkill_Info->s_pFTSkill->GetInfo()->s_fCooldownTime * 0.001f + 0.15f );

	// 메모리 칩 감소
	g_pDataMng->GetInven()->DecreaseItem_TypeLS( nItem::DigimonSkillMemoryChip, 0, pSkill_Info->s_pFTSkill->GetInfo()->s_nReq_Item, false, false );

	// 도발 스킬을 경우 Sync를 맞추기 위해 일반 공격 패킷을 보냄
	if( pSkill_Info->s_pFTSkill->GetInfo()->s_Apply[0].s_nA == 46 )
	{
		pop( nTargetUID );

		//g_pCharMng->GetDigimonUser(0)->GetProp_Animation()->SetAnimation( ANI::ATTACK_ATTACK1 );
		CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
		if( pTarget == NULL )
		{
			assert_cs( false );
			return;
		}

		//net::game->SendAttack(g_pCharMng->GetDigimonUser(0)-> GetUniqID(), pTarget->GetUniqID() );
		g_pCharMng->GetTamerUser()->GetSkillMng()->__Normal_Attack_Monster( g_pCharMng->GetTamerUser()->GetSkillMng()->GetSkill(2) );
	}
}

void cCliGame::RecvMemorySkill_EffectSync()
{
	u4 nTargetUID = 0;
	u4 nSkillCode = 0;

	pop( nTargetUID );
	pop( nSkillCode );

	CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillCode );
	SAFE_POINTER_RET(pSkill);

	CsC_AvObject* pTarget = g_pMngCollector->GetObject( nTargetUID );
	SAFE_POINTER_RET(pTarget);

	switch( pSkill->GetInfo()->s_Apply[0].s_nA )
	{
	case 1:		// 생명력 회복
	case 47:	// 타겟 hp가 일정 % 이하 일때 hp 일정 % 증가
		pTarget->GetProp_Effect()->AddEffect_FT( "system\\Buff\\Buff_Hp_Absorb.nif"/*, 1.0f, nsEFFECT::POS_CHARPOS*/ );
		break;
	case 3:		// 최대 hp, ds 증가
	case 9:		// 이속 증가
	case 19:	// 회피 증가
		break;
// 	default:
// 		assert_cs( false );
	}
}

bool cCliGame::RecvMonsterSkill( int nRecvType, int nIdx/*테스트용*/ )
{
	enum eRecvType{ START_CAST = 0, CASTING = 1, USE = 2, };

	u4 nSkillIdx = nIdx;		// 스킬 인덱스
	pop( nSkillIdx );

	u4 nMonsterUID = 0;			// 스킬 시전할 몬스터 UID
 	pop( nMonsterUID );

	if( nMonsterUID == 0 || nSkillIdx == 0 )
		return false;

	CsC_AvObject* pObject = g_pMngCollector->GetObject( nMonsterUID );	
	if( pObject == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T( "RecvMonsterSkill :: Sync 내에 존재하지 않는 몬스터 - UID : %d" ), nMonsterUID);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T( "RecvMonsterSkill :: Sync 내에 존재하지 않는 몬스터 - UID : %d" ), nMonsterUID);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return false;
	}

	CsMonsterSkill::sINFO* pSkillInfo =  nsCsFileTable::g_pMonsterMng->GetMonsterSkill( nSkillIdx );
	if( pSkillInfo == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T( "RecvMonsterSkill :: 존재하지 않는 몬스터스킬 - SkillIdx : %d" ), nSkillIdx);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T( "RecvMonsterSkill :: 존재하지 않는 몬스터스킬 - SkillIdx : %d" ), nSkillIdx);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return false;
	}

// 	CHAT_TEXT_DEGUG( _T("RecvMonsterSkill :: 스킬 시전 시작 ( SkillType - %d ) - TimeTag%d"), pSkillInfo->s_nSkillType, _TIME_TS%100 );
	//=============================================================================
	// 스킬 사용하는 몬스터 애니 셋팅
	//=============================================================================
	// SequenceID : 스킬 사용 애니  /  SequenceID+10 : 스킬 시전 시작 애니  /  SequenceID+20 : 스킬 시전 애니  
	switch( nRecvType )
	{
	case START_CAST:	// 스킬 시전 시작
		if( pSkillInfo->s_nCastCheck == 1 )
		{
			((CMonster*)pObject)->SetPause( CsC_AvObject::PAUSE_ANIMATION, true );
			((CMonster*)pObject)->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
			((CMonster*)pObject)->SetPause( CsC_AvObject::PAUSE_PATH, true );

			pObject->SetAnimation(  pSkillInfo->s_dwSequenceID + 10 );

			//2017-05-31-nova m_pActorMng NULL체크
			NiActorManager* actorMng = pObject->GetProp_Animation()->GetActorMng();

			if(actorMng != NULL)
			{
				actorMng->SetTargetAnimation( pSkillInfo->s_dwSequenceID + 10 );
			}
			
			((CMonster*)pObject)->SetCasting_Monster( true );
		}
		break;
	case CASTING:		// 스킬 시전 중
		if( pSkillInfo->s_nCastCheck == 1 )
		{
			((CMonster*)pObject)->SetPause( CsC_AvObject::PAUSE_ANIMATION, true );
			((CMonster*)pObject)->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );

			pObject->SetPause( CsC_AvObject::PAUSE_PATH, true );
			pObject->SetAnimation(  pSkillInfo->s_dwSequenceID + 20 );
			((CMonster*)pObject)->SetCasting_Monster( true );
		}
		break;
	case USE:			// 스킬 사용
		break;
	}

	//=============================================================================
	// 이펙트
	//=============================================================================
	// 사용 이펙트 플래그
#define CENTERFLAG	nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER
#define FOOTFLAG	nsEFFECT::POS_CHARPOS
#define HEIGHTFLAG	nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_THEIGHT

	DWORD dwFlag = 0;
	float fScale = 1.0f;

	//=============================================================================
	// 스킬 타겟(유저) 이펙트 셋팅
	//=============================================================================
	u4 nCnt = 2;	// 타겟 수
	pop( nCnt );

	// 타겟 이펙트 추가
	while( nCnt )
	{		
		u4 nTargetUID = 0;	// 타겟 UID
		pop( nTargetUID );

		pObject = g_pMngCollector->GetObject( nTargetUID );	
		if( pObject == NULL )
		{
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
			//CProtocol.m_wStr = GetVAString(_T( "RecvMonsterSkill :: Sync 내에 존재하지 않는 대상 - UID : %d" ), nTargetUID);
			DmCS::StringFn::Format(CProtocol.m_wStr, _T( "RecvMonsterSkill :: Sync 내에 존재하지 않는 대상 - UID : %d" ), nTargetUID);
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
			return false;
		}

		// 스킬 상태별 이펙트 셋팅
		switch( nRecvType )
		{
		case START_CAST:
			{
				switch( pSkillInfo->s_nSkillType )
				{
				case CsMonsterSkill::ASSEMBLE:			// 모아맞기
				case CsMonsterSkill::DISPERSE:			// 흩어져맞기
				case CsMonsterSkill::ATTACH_SEED:		// 씨앗깔기
				case CsMonsterSkill::GatheringExt:	// 모아맞기(근/원딜 발사체 나가게 수정)
				case CsMonsterSkill::DisperseExt:		// 흩어져맞기(근/원딜 발사체 나가게 수정)
					{
						char szEff[OBJECT_PATH_LEN] = {0,};	// 이펙트 경로
						// 대상 표시 이펙트
						sprintf_s( szEff, OBJECT_PATH_LEN, "system\\Raid_Skill\\Damage_Mark.nif" );
						dwFlag = nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_THEIGHT | nsEFFECT::LIVE_LOOP | nsEFFECT::OFFSET_USE;
						fScale = 20.0f;
						NiPoint3 vOffset = NiPoint3( 0.0f, 0.0f, 200.0f );
						// 대상 표시 이펙트
						pObject->GetProp_Effect()->AddSkillLoopEffect( szEff, nSkillIdx, dwFlag/*, false, fScale*/ );
						pObject->GetProp_Effect()->_GetLoopSkillEffect( nSkillIdx )->SetScale( fScale );
						CsC_EffectObject* pEffect = (CsC_EffectObject*)pObject->GetProp_Effect()->_GetLoopSkillEffect( nSkillIdx );
						pEffect->SetOffset( vOffset );
					}break;
				}
			}
			break;
		case CASTING:
		case USE:
			fScale = 1.0f;
			break;
		}

		if( dwFlag & nsEFFECT::LIVE_LOOP )
		{

			//=============================================================================
			// 스킬 사용하는 몬스터 이펙트 셋팅
			//=============================================================================
			// 스킬 범위 이펙트
			switch( pSkillInfo->s_nSkillType )
			{
		 	//case CsMonsterSkill::HP_VAL_DECREASE:	// 광역스킬
			case CsMonsterSkill::ASSEMBLE:			// 모아맞기
			case CsMonsterSkill::DISPERSE:			// 흩어져맞기
			case CsMonsterSkill::ATTACH_SEED:		// 씨앗깔기
			case CsMonsterSkill::GatheringExt:	// 모아맞기(근/원딜 발사체 나가게 수정)
			case CsMonsterSkill::DisperseExt:		// 흩어져맞기(근/원딜 발사체 나가게 수정)
				{
					//시전이 있는 스킬일 때만 보여줘야함. 스킬 인덱스와 겹치니까 인덱스 * 1000 해줌
					if( pSkillInfo->s_nCastCheck == 1 )
					{
						char szEff[OBJECT_PATH_LEN] = {0,};	// 이펙트 경로
						if( strlen( pSkillInfo->s_szNoticeEff_Name ) > 0 )
							sprintf_s( szEff, OBJECT_PATH_LEN, pSkillInfo->s_szNoticeEff_Name );
						else
							sprintf_s( szEff, OBJECT_PATH_LEN, "system\\Raid_Skill\\Warning_Circle.nif" );

						if( nsCsFileTable::g_pMonsterMng )
						{
							CsMonsterSkillTerms::sINFO* pSkillTerm = nsCsFileTable::g_pMonsterMng->GetMonsterSkillTerms( pSkillInfo->s_nRangeIdx );
							if( pSkillTerm && pSkillTerm->s_nRange > 0 )
							{
								// 기본적으로 광역 스킬 이펙트는 8M ( 범위 800 ) 기준으로 리소스 제작되므로
								// 리소스를 테이블에서 설정한 범위에 따라 크기 변경하기
								fScale = ((float)pSkillTerm->s_nRange)/800.0f;
							}
						}

						dwFlag = nsEFFECT::POS_CHARPOS | nsEFFECT::LIVE_LOOP | nsEFFECT::OFFSET_USE;
						pObject->GetProp_Effect()->AddSkillLoopEffect( szEff, nSkillIdx*1000, dwFlag/*, false, fScale*/ );
						pObject->GetProp_Effect()->_GetLoopSkillEffect( nSkillIdx*1000 )->SetScale( fScale );
						//몬스터 시전 이펙트, 바닥 이펙트와 겹치지 않도록 위치 조정
						NiPoint3 vOffset = NiPoint3( 0.0f, 0.0f, 50.0f );
						CsC_EffectObject* pEffect = (CsC_EffectObject*)pObject->GetProp_Effect()->_GetLoopSkillEffect( nSkillIdx*1000 );
						pEffect->SetOffset( vOffset );
					}
				}
				break;
			default:
				break;
			}
		}
// 		else
// 		{
// 			// 스킬 이펙트
// 			pObject->GetProp_Effect()->AddEffect( szEff, fScale, dwFlag );
// 		}
		nCnt--;
	}

	// 추가 동작
	switch( nRecvType )
	{
	case START_CAST:	// 스킬 시전 시작
		{
			if( pSkillInfo->s_nTalkId != 0 )
			{
				// 몬스터 스킬 시전 토크
				cPrintMsg::PrintMsg( pSkillInfo->s_nTalkId );
			}
		}
		break;
	case CASTING:		// 스킬 시전 중
	case USE:			// 스킬 사용
		break;
	}

#undef CENTERFLAG
#undef FOOTFLAG
#undef HEIGHTFLAG

	return true;
}
void cCliGame::RecvRaidChainSkill(void)
{
	BHPRT("RecvRaidChainSkill");
	u4 nCasterUID = 0; 
	u4 nCastingSkillIDX = 0;	
	u2 nSkillType;
	pop( nCasterUID );
	pop( nCastingSkillIDX );
	pop( nSkillType );
	CsMonsterSkill::sINFO* pkSkillInfo = nsCsFileTable::g_pMonsterMng->GetMonsterSkill( nCastingSkillIDX );

	CsC_AvObject* pkCaster = g_pMngCollector->GetObject( nCasterUID );
	SAFE_POINTER_RET( pkCaster );
	CsC_EffectProp* pkCasterEffProp = pkCaster->GetProp_Effect();
	CsC_AttackProp* pkCasterAttProp = pkCaster->GetProp_Attack();
	SAFE_POINTER_RET( pkSkillInfo );SAFE_POINTER_RET( pkCasterEffProp );SAFE_POINTER_RET( pkCasterAttProp );

	switch(nSkillType)
	{
	case 1:
		{
			const float fEffectLifeTime = 3.0f;
			const float fEffecDelayTime =0.35f;

			n4 nTargetCnt = 0;
			u4 uTargetUID =0;
			pop( nTargetCnt );
			pop( uTargetUID );

			CsC_AvObject* pkStarterTarget = g_pMngCollector->GetObject( uTargetUID );
			SAFE_POINTER_RET( pkStarterTarget );

			CsC_AvObject* pkPrevTarget = pkStarterTarget;
			NiPoint3 szStartPos= pkStarterTarget->GetPos();
			NiPoint3 szEndPos = NiPoint3::ZERO;
			char cEffectPath[ MAX_PATH ] = {0, };
			for(int i=  1;  i < nTargetCnt; ++i)
			{
				pop( uTargetUID );
				CsC_AvObject* pkTarget = g_pMngCollector->GetObject( uTargetUID );
				SAFE_POINTER_RET( pkTarget );

				CsC_EffectObject* pChainEffect = pkCasterEffProp->AddEffect(szStartPos, "QingLongmon_Raidboss\\chainlightning_test.nif");
				SAFE_POINTER_RET( pChainEffect );

				pChainEffect->SetChainTarget(pkPrevTarget, pkTarget);
				pChainEffect->SetDelayTime(fEffecDelayTime);
				pChainEffect->SetLifeTime(fEffectLifeTime);

				szStartPos = pkPrevTarget->GetPos();
				pkPrevTarget = pkTarget;

				sprintf_s( cEffectPath, MAX_PATH, "System\\Raid_Skill\\Lightning_dmg_loop.nif");
				DWORD dwFlag = nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER | nsEFFECT::LIVE_LOOP | nsEFFECT::OFFSET_USE;			
				 pkTarget->GetProp_Effect()->AddSkillLoopEffect( cEffectPath, nCastingSkillIDX, dwFlag, false, 0.5f, fEffectLifeTime);
				//pDamagedEffect->SetLifeTime(fEffectLifeTime);
				//pDamagedEffect->DeleteLoopEffectSkill()
			}
		}
		break;
	case 2:
		break;
	}
}


void cCliGame::RecvMonsterSkill_Use( int nIdx )
{
	// 씨앗깔기, 광폭화 같이 스킬은 사용하지만 직접적인 데미지가 없을 때 해당 패킷으로 수신

	u4 nMonsterUID = 0;			// 스킬 시전할 몬스터 UID
	pop( nMonsterUID );

	u4 nSkillIdx = nIdx;			// 스킬 인덱스
	pop( nSkillIdx );

	if( nMonsterUID == 0 || nSkillIdx == 0 )
		return;

	CsC_AvObject* pMonsertObject = g_pMngCollector->GetObject( nMonsterUID );	
	if( pMonsertObject == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T( "RecvMonsterSkill_Use :: Sync 내에 존재하지 않는 몬스터 - UID : %d" ), nMonsterUID);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T( "RecvMonsterSkill_Use :: Sync 내에 존재하지 않는 몬스터 - UID : %d" ), nMonsterUID);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	CsMonsterSkill::sINFO* pSkillInfo =  nsCsFileTable::g_pMonsterMng->GetMonsterSkill( nSkillIdx );
	if( pSkillInfo == NULL )
	{
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T( "RecvMonsterSkill_Use :: 존재하지 않는 몬스터스킬 - SkillIdx : %d" ), nSkillIdx);
		DmCS::StringFn::Format(CProtocol.m_wStr,_T( "RecvMonsterSkill_Use :: 존재하지 않는 몬스터스킬 - SkillIdx : %d" ), nSkillIdx);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}

	// 원래 SkillInfo2 에서 데미지 이벤트에 스킬 사용 애니를 넣어야하지만
	// 데미지 이벤트가 씨앗 바닥 위에 있을때마다 호출되기때문에
	// 이놈은 여기서 사용 애니를 설정
	if( pSkillInfo->s_dwSequenceID > 0 )
		pMonsertObject->SetAnimation( pSkillInfo->s_dwSequenceID );


	switch( pSkillInfo->s_nSkillType )
	{
	case CsMonsterSkill::ATTACH_SEED:
		{
// 			CHAT_TEXT_DEGUG( _T("RecvMonsterSkill_Use :: 스킬 사용 ( SkillType - %d ) - TimeTag%d"), pSkillInfo->s_nSkillType, _TIME_TS%100 );
			u4 nEffectTargetCnt;
			pop( nEffectTargetCnt );


			// 사용 이펙트 플래그
#define CENTERFLAG	nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER
#define FOOTFLAG	nsEFFECT::POS_CHARPOS
#define HEIGHTFLAG	nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_THEIGHT

			u4 nEffectTargetIDX = 0;
			for( u4 i = 0 ; i < nEffectTargetCnt ; i++ )
			{
				pop( nEffectTargetIDX );// 타겟 디지몬 UID
				CsC_AvObject* pDigimonTarget = g_pMngCollector->GetObject( nEffectTargetIDX );
				if( pDigimonTarget )
				{
					CsC_EffectProp* pEffProp = pDigimonTarget->GetProp_Effect();
					// 대상 표시 이펙트 제거
					if( pEffProp )
					{
						pEffProp->DeleteSkillLoopEffect( nSkillIdx );
						pEffProp->DeleteSkillLoopEffect( nSkillIdx*1000 );	// 범위 표시 이펙트
					}
				}

				int nPosX, nPosY;
				pop( nPosX );
				pop( nPosY );
				float fPosZ = nsCsGBTerrain::g_pCurRoot->GetHeight( (float)nPosX, (float)nPosY );
				NiPoint3 vPos = NiPoint3( (float)nPosX, (float)nPosY, fPosZ );

				//바닥 이펙트용 더미 오브젝트 생성
				CsC_AvObject* pDummyTarget = g_pCharMng->AddTempObject( nMonsterUID/*nSkillIdx*/, vPos );
				if( pDummyTarget )
				{
					char szEff[OBJECT_PATH_LEN] = {0,};
					if( strlen( pSkillInfo->s_szNoticeEff_Name ) > 0 )
						sprintf_s( szEff, OBJECT_PATH_LEN, pSkillInfo->s_szNoticeEff_Name  );
					else
						sprintf_s( szEff, OBJECT_PATH_LEN, "system\\Raid_Skill\\Dot_Damage_Circle.nif"  );

					CsC_EffectProp* pDummyEffProp = pDummyTarget->GetProp_Effect();
					if( pDummyEffProp )
					{
						DWORD dwFlag = nsEFFECT::POS_CHARPOS | nsEFFECT::SPOS_BOUND_CENTER | nsEFFECT::LIVE_LOOP;
						pDummyEffProp->AddSkillLoopEffect( szEff, nSkillIdx, dwFlag, false,1.0f, pSkillInfo->s_dwEff_Fact_Val[0] * 0.001f );
						CsC_AvObject* pLoopSkillEf = pDummyEffProp->_GetLoopSkillEffect( nSkillIdx );
						if( pLoopSkillEf )
						{
							int nRange = 0;
							CsMonsterSkillTerms::sINFO* pSkillTerms = nsCsFileTable::g_pMonsterMng->GetMonsterSkillTerms( pSkillInfo->s_nRangeIdx );
							if( pSkillTerms )
								nRange = pSkillTerms->s_nRange;

							// 기본적으로 광역 스킬 이펙트는 8M ( 범위 800 ) 기준으로 리소스 제작되므로
							// 리소스를 테이블에서 설정한 범위에 따라 크기 변경하기
							float fScale = 1.0f;
							if( nRange > 0 )
								fScale = (float)nRange/800.0f;
							pLoopSkillEf->SetScale( fScale );
						}
					}
				}
			}
#undef CENTERFLAG
#undef FOOTFLAG
#undef HEIGHTFLAG
		}
		break;
	case CsMonsterSkill::BERSERK:	// 광폭화
		// 몬스터 스킬 시전 토크 -> 원래 시전 토크지만 광폭화는 시전이 없음
		cPrintMsg::PrintMsg( pSkillInfo->s_nTalkId );
		return;
	default:
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		//CProtocol.m_wStr = GetVAString(_T("RecvMonsterSkill_Use :: 잘못된 스킬 타입 ( Type : %d )"), pSkillInfo->s_nSkillType);
		DmCS::StringFn::Format(CProtocol.m_wStr, _T("RecvMonsterSkill_Use :: 잘못된 스킬 타입 ( Type : %d )"), pSkillInfo->s_nSkillType);
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		return;
	}
}

void cCliGame::RecvMonsterSkillSync_PersistEnd()
{
	// 바닥(씨앗) 만료 시
	u4 nMonsterUID = 0;
	pop( nMonsterUID );
	// 몬스터와 연결된 이펙트 오브젝트 제거
	g_pCharMng->DeleteTempObject( nMonsterUID );
}
void cCliGame::TamerSkillErrInEpicDungeon()
{
	// 움직임 가능하게
	g_pCharMng->GetTamerUser()->SetPauseAll( false );
	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::USE_DIGIMON_SKILL, 0 );
	g_pCharMng->GetTamerUser()->ReleaseSrvSendSkill();

}


void cCliGame::SendSkillMaxLvUp( int const& nUseItemInvenPos, int const& nUseItemType, int const& nEvoSlotNum )
{
#ifdef SDM_DIGIMONSKILL_LV_EXPEND_20181206
	C2GS_SEND_DIGIMON_SKILL_LIMIT_OPEN packet;
	packet.nItemPos = nUseItemInvenPos;
	packet.nItemType = nUseItemType;
	packet.nDigimonEvoArray = nEvoSlotNum;

	newp(packet.GetProtocol());
	push(packet.nItemPos);
	push(packet.nItemType);
	push(packet.nDigimonEvoArray);
	endp(packet.GetProtocol());
	send();
#endif
}

void cCliGame::RecvSkillMaxLvUp()
{
#ifdef SDM_DIGIMONSKILL_LV_EXPEND_20181206
	GS2C_RECV_DIGIMON_SKILL_LIMIT_OPEN recv;	
	pop( recv.nResult );

	if( nsSkillLimitOpenResult::Success != recv.nResult )
	{
		cPrintMsg::PrintMsg( recv.nResult );
		return ;
	}

	pop( recv.nDigimonEvoArray );
	pop( &recv.cEvo, sizeof(cEvoUnit) );
	pop( recv.nItemPos );
	pop( recv.nItemType );

	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
		{
			cItemInfo* pItemInfo = pInven->GetData( recv.nItemPos );
			if( pItemInfo && (recv.nItemType == pItemInfo->GetType()) )
				pInven->DecreaseItem( pItemInfo->GetType(), 1, false, false );
		}
	}

	SAFE_POINTER_RET( g_pCharMng );
	SAFE_POINTER_RET( nsCsFileTable::g_pEvolMng );
	SAFE_POINTER_RET( nsCsFileTable::g_pDigimonMng );

	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pDigimonUser );

	cEvoUnit* pEvoUnit = pDigimonUser->GetAttributeEvoUnit( recv.nDigimonEvoArray );
	SAFE_POINTER_RET( pEvoUnit );
	*pEvoUnit = recv.cEvo;

	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( pDigimonUser->GetBaseDigimonFTID() );
	SAFE_POINTER_RET( pFTEvolInfo );

	CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( recv.nDigimonEvoArray );
	SAFE_POINTER_RET( pFTEvolObj );

	CsDigimon* pDigimonFT = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID );
	SAFE_POINTER_RET( pDigimonFT );

	CsDigimon::sINFO* pFTDigimonInfo = pDigimonFT->GetInfo();
	SAFE_POINTER_RET( pFTDigimonInfo );

	int nChangeMaxskillLv = pDigimonUser->GetCurrentEvoMaxSkillLv();
	cPrintMsg::PrintMsg( 11052, &(pFTDigimonInfo->s_szName), &nChangeMaxskillLv );	

	GAME_EVENT_ST.OnEvent( EVENT_CODE::UPDATE_SKILL_DIGIMONSTATUS, NULL );
#endif
}

void cCliGame::NTF_DigimonSkillCoolTime()
{	// 현재 디지몬에 대한 정보가 날라옴

	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pDigimonUser );

	GS2C_SEND_DIGIMON_SKILL_COOL_TIME recv;
	pop( recv.u2IdxDigimon );
	if( recv.u2IdxDigimon != pDigimonUser->GetUniqID() )
		return;

	pop( recv.uEvoType );
	pop( recv.nDigimonSkillCount );
	if( recv.nDigimonSkillCount > 0 )
	{
		CDigimonUser::sSkillCoolTimeReservation	reservationData;
		reservationData.dwDigimonTBID = recv.uEvoType;
		for( int n = 0; n < recv.nDigimonSkillCount; ++n )
		{
			CDigimonUser::sReaminCoolTime addData;
			pop( addData.dwSkillCode );
			pop( addData.dwRemainTime );		
			reservationData.listReaminTime.push_back( addData );
		}

		pDigimonUser->SetReservationSkillReaminCoolTime( reservationData );
	}	
}

void cCliGame::RecvDigimonSkillLevelUp()
{
#if COMMON_LIB_FIXED
	GS2C_RECV_DIGIMON_SKILL_LEVEL_UP recv;
	pop(recv.nResult);
	pop(recv.nDigimonSourceIndex);
	pop(recv.nEvolutionSlotIndex);
	pop(recv.nSkillIndex);
	pop(recv.nSkillPoint);
	pop(recv.nSkillLevel);

	GAME_EVENT_ST.OnEvent(EVENT_CODE::RECV_SKILL_LEVELUP_DIGIMONSTATUS, &recv);
#endif
}