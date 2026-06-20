
#include "stdafx.h"
#include "AniProperty.h"

THREAD_MEMPOOL_CPP( CsC_AttackProp )
THREAD_MEMPOOL_CPP( CsC_AttackProp::sHIT_INFO )
THREAD_MEMPOOL_CPP( CsC_AttackProp::sDAMAGE_INFO )
THREAD_MEMPOOL_CPP( CsC_AttackProp::sEVENT_INFO )

#define PARENT_CALSS		CsC_Property

CALLBACK_DamageNumber		CsC_AttackProp::g_CallBackDamageNumber = NULL;

CsC_AttackProp::CsC_AttackProp()
{
	m_eType = CsC_Property::ATTACK;	
	m_pHitTarget = NULL;
	m_pFollowTarget = NULL;
	m_pLastPartnerDamageInfo = NULL;
#ifdef LEVELUP_EFFECT_LOOP
	m_nTickTime = 0;
#endif
	m_fFollowRange = 0.0f;
	m_eFollowIterator = FI_SKILL1;
	m_bAttackingTarget = false;
}

CsC_AttackProp::~CsC_AttackProp()
{
	m_pHitTarget = NULL;
	m_pFollowTarget = NULL;	
	m_pLastPartnerDamageInfo = NULL;
}

void CsC_AttackProp::Delete()
{
	PARENT_CALSS::Delete();

	m_eType = CsC_Property::ATTACK;	
	m_pHitTarget = NULL;
	m_pFollowTarget = NULL;
	m_pLastPartnerDamageInfo = NULL;
#ifdef LEVELUP_EFFECT_LOOP
	m_nTickTime = 0;
#endif
	m_fFollowRange = 0.0f;
	m_eFollowIterator = FI_SKILL1;
	m_bAttackingTarget = false;
}

void CsC_AttackProp::PreDelete()
{
	DeleteHDList();
}

void CsC_AttackProp::DeleteHDList()
{
	std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
	std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
	for( ; it!=itEnd; ++it )
	{
		switch( (*it)->s_nBaseType )
		{
		case HD_HIT:	sHIT_INFO::DeleteInstance( (sHIT_INFO*)(*it) );			break;
		case HD_DAMAGE:	sDAMAGE_INFO::DeleteInstance( (sDAMAGE_INFO*)(*it) );	break;
		case HD_EVENT:	sEVENT_INFO::DeleteInstance( (sEVENT_INFO*)(*it) );		break;
		default:		assert_cs( false );
		}
	}
	m_listHDProcess.clear();
}

void CsC_AttackProp::Init( CsC_AvObject* pParent )
{
	PARENT_CALSS::Init( pParent );

	assert_cs( m_pHitTarget == NULL );
	assert_cs( m_pFollowTarget == NULL );
	assert_cs( m_listHDProcess.size() == 0 );

	// 0.5초 셋팅
	m_TargetMoveTimeSeq.SetDeltaTime( 300 );
	m_TargetChangeTimeSeq.SetDeltaTime( 700 );

	m_bAttackingTarget = false;
}

void CsC_AttackProp::PreUpdate( float fDeltaTime )
{
	CsC_AniProp* pParentAniProp = m_pParent->GetProp_Animation();
	assert_cs( pParentAniProp != NULL );
	assert_cs( m_pParent != NULL );

	DWORD dwCurTick = GetTickCount();

	std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
	std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
	while( it!=itEnd )
	{
		switch( (*it)->s_nBaseType )
		{
		case HD_HIT:
			{
				sHIT_INFO* pHit = (sHIT_INFO*)(*it);

				bool bEnableProcess = false;

				switch( pHit->s_eHitType )
				{
				case HT_Skill:
					bEnableProcess = pParentAniProp->IsEnableSkill();
					break;
				case HT_Skill_NoAni:
					bEnableProcess = false;
					break;
				default:
					bEnableProcess = pParentAniProp->IsEnableNormalAttack();
					m_pLastPartnerDamageInfo = pHit->s_pPartnerDamageInfo;
				}

				if( bEnableProcess == true )
				{
					// 스킬 사용
					if( pHit->s_eHitType == HT_Skill )
					{
						// 평타 리는중인가
						switch( m_pParent->GetProp_Animation()->GetAnimationID() )
						{
						case ANI::ATTACK_ATTACK1:
						case ANI::ATTACK_ATTACK2:
							{
								// 때려야 하는 데미지가 남아 있다면
								if( m_pParent->GetProp_Animation()->GetSequenceMng()->IsEmptyNAttEvent() == false )
								{
									// 평타 즉시 발동
									if( m_pLastPartnerDamageInfo != NULL )
									{
										m_pLastPartnerDamageInfo->s_eActive = AT_JUST_NOW;
										sDAMAGE_INFO* pChild = m_pLastPartnerDamageInfo->s_pChildDamageInfo;
										while( pChild )
										{
											pChild->s_eActive = AT_JUST_NOW;
											pChild = pChild->s_pChildDamageInfo;
										}
									}
								}
							}
							break;
						}
					}
					if( pHit->s_pPartnerDamageInfo != NULL )
					{
						pHit->s_pPartnerDamageInfo->s_eActive = AT_ACTIVE;
						sDAMAGE_INFO* pChild = pHit->s_pPartnerDamageInfo->s_pChildDamageInfo;
						while( pChild )
						{
							pChild->s_eActive = AT_ACTIVE;
							pChild = pChild->s_pChildDamageInfo;
						}
					}

					m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );					
					pParentAniProp->SetAnimation( pHit->s_dwAniID, pHit->s_eHitType == HT_Critical, pHit->s_fAniAccelRate );

					// 매번 공격 할때 타겟 지정
					//멀티 타겟일 경우는 이미 타겟으로 추가했기 때문에 넣지 않는다
					if(pHit->s_MultyTargetID == -1)
						SetHitTarget( pHit->s_pHitTarget );

					m_pParent->SetPause( CsC_AvObject::PAUSE_PATH, true );
					m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, true );

					// 타겟지점으로 회전
					if( pHit->s_bRotation == true )
					{
						m_pParent->SetRotation_Ani( pHit->s_vDamagePos );
					}					
					
					if( pParentAniProp->GetSequenceMng() )
					{
						// 이벤트 옵션 넣어주자
						CsC_AniProp_Sequence::sEVENT_OPTION eo;
						eo.s_eHitType = pHit->s_eHitType;
						eo.s_fVelocity = pHit->s_fRangeVelocity;
						eo.s_fAccelVelocity = pHit->s_fRangeAccelVelocity;
						eo.s_bRangeAttack = pHit->s_bRangeAttack;
						eo.s_nAttackEffectIndex = pHit->s_nAttackEffectIndex;
						eo.s_fAniAccelRate = pParentAniProp->GetAniAccelRate();
						// 데미지 반사 버프 스킬 추가_14.05.14		chu8820
						eo.s_nReflectType = pHit->s_nReflectType;
						if( pHit->s_eHitType == HT_Skill )
						{
							#define CSLAST_ERROR
							g_CsLastError.m_nUValue = eo.s_nAttackEffectIndex;	
						}
						if(pHit->s_MultyTargetID > -1){
							eo.s_eSKillType = ANI::SKILLTYPE::RANDOMMULTY;
						}
						else
							eo.s_eSKillType = ANI::SKILLTYPE::NORMAL;

						pParentAniProp->GetSequenceMng()->SetEventOption( &eo );
					}
					sHIT_INFO::DeleteInstance( pHit );
					it = m_listHDProcess.erase( it );
				}
				else
				{
					if( pHit->s_eHitType == HT_Skill_NoAni
						&& pHit->s_pPartnerDamageInfo != NULL )
					{
						pHit->s_pPartnerDamageInfo->s_eActive = AT_ACTIVE;
						sDAMAGE_INFO* pChild = pHit->s_pPartnerDamageInfo->s_pChildDamageInfo;
						while( pChild )
						{
							pChild->s_eActive = AT_ACTIVE;
							pChild = pChild->s_pChildDamageInfo;
						}
						sHIT_INFO::DeleteInstance( pHit );
						it = m_listHDProcess.erase( it );
					}
					else
						++it;
				}
			}
			break;
		case HD_DAMAGE:
			{
				sDAMAGE_INFO* pDamage = (sDAMAGE_INFO*)(*it);

				// 활성화 되었는가
				switch( pDamage->s_eActive )
				{
				case AT_ACTIVE:
					// 이벤트 타임이 다 되었는가
					if( ( pDamage->s_fHitEventTime -= fDeltaTime ) <= 0.0f )
					{
						bool bErase = false;
						switch( pDamage->s_eDamageType )
						{
							// 맞았다~
						case DT_Normal:
						case DT_Block:
						case DT_KnockBack:
						case DT_Damage_Dot:		
						case DT_Recover:	
						case DT_Absorb:	// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
							{
								// 마블 이펙트
								if( pDamage->s_eMarbleType != MB_NONE )
								{									
									m_pParent->SetMarble( pDamage->s_eMarbleType, pDamage->s_pHitter, -pDamage->s_nValue[ 0 ]/20 );
								}

								// 데미지 띄워주자
								if( g_CallBackDamageNumber != NULL && pDamage->s_nValue[ 0 ] != 0 )
								{
									NiPoint3 vPos = m_pParent->GetPos();
									vPos.z += m_pParent->GetToolHeight() - 100.0f;
									g_CallBackDamageNumber( pDamage->s_nNumEffect, pDamage->s_nNumType, -pDamage->s_nValue[ 0 ], vPos, pDamage->s_fDamageRot );
								}

								// Value값 적용
								for( int i=1; i<5; ++i )
								{
									if( pDamage->s_nValue[ i ] == 0 )
										continue;

									switch( i )
									{
									case 1:			// HP
										m_pParent->GetBaseStat()->SetDeltaHP( pDamage->s_nValue[ i ] );
										break;
									case 2:			// DS
										m_pParent->GetBaseStat()->SetDeltaDS( pDamage->s_nValue[ i ] );
										break;
									case 3:			// MHP
										m_pParent->GetBaseStat()->SetDeltaMaxHP( pDamage->s_nValue[ i ] );
										break;
									case 4:			// MDS
										m_pParent->GetBaseStat()->SetDeltaMaxDS( pDamage->s_nValue[ i ] );
										break;
									}
								}

								ANI::eTYPE ani = ANI::INVALIDE_ID;

								switch( pDamage->s_eDamageType )
								{
								case DT_Normal:
									ani = pParentAniProp->IsEnableDamage();
									break;
								case DT_Absorb:	// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
									ani = ANI::INVALIDE_ID;
									break;
								case DT_Block:
									m_pParent->GetProp_Effect()->AddEffect_FT( "System\\Block.nif" );
									ani = pParentAniProp->IsEnableBlock();
									break;
								case DT_KnockBack:
									m_pParent->KnockBack( pDamage->s_vHitterPos );									
									break;								
								case DT_Damage_Dot:
									{
										if( pDamage->s_pHitter )
											m_pParent->SetRotation_Ani( pDamage->s_pHitter->GetPos() );

										ani = pParentAniProp->IsEnableDamage(); //맞는모션후에 도트이펙트 붙으므로 break 하면 안됨!!!
									}
								case DT_Recover:
									{
										// 관련 도트 이펙트 
										CsBuff* pBuff = nsCsFileTable::g_pBuffMng->GetBuff( (USHORT)pDamage->s_dwResistTime );
										if( pBuff->GetInfo()->s_szEffectFile[ 0 ] != NULL )
										{
											char cEffectPath[ MAX_PATH ] = {0, };
											sprintf_s( cEffectPath, MAX_PATH, "System\\Buff\\%s.nif", pBuff->GetInfo()->s_szEffectFile );
											m_pParent->GetProp_Effect()->AddEffect_FT( cEffectPath );		
										}
									}
									break;
								default:
									assert_cs( false );
								}								

								if( ani != ANI::INVALIDE_ID )
								{
									pParentAniProp->SetAnimation( ani );
								}
								
								bErase = true;
							}
							break;
						case DT_Condition:
							{						
								// 관련 도트 이펙트 
								CsBuff* pBuff = nsCsFileTable::g_pBuffMng->GetBuff( (USHORT)pDamage->s_dwResistTime );
								if( pBuff->GetInfo()->s_szEffectFile[ 0 ] != NULL )
								{
									char cEffectPath[ MAX_PATH ] = {0, };
									sprintf_s( cEffectPath, MAX_PATH, "System\\Buff\\%s.nif", pBuff->GetInfo()->s_szEffectFile );
									m_pParent->GetProp_Effect()->AddEffect_FT( cEffectPath );		
								}						
								pParentAniProp->SetAnimation( pParentAniProp->IsEnableDamage() );

								if( pDamage->s_pHitter )
									m_pParent->SetRotation_Ani( pDamage->s_pHitter->GetPos() );
							
								bErase = true;
							}
							break;
							// 피했다
						case DT_Miss:
							{
								m_pParent->GetProp_Effect()->AddEffect_FT( "System\\Miss.nif" );

								// 애니메이션 플레이
								ANI::eTYPE ani = pParentAniProp->IsEnableEvade();
								if( ani != ANI::INVALIDE_ID )
								{
									pParentAniProp->SetAnimation( ani );
								}
								bErase = true;
							}
							break;
							// 맞고 죽었다
						case DT_Dead:
							{
								bool bDead = true;
								sDAMAGE_INFO* pDamageCheck;
								std::list< sHD_BASE* >::iterator itDamage = m_listHDProcess.begin();
								std::list< sHD_BASE* >::iterator itDamageEnd = m_listHDProcess.end();
								for( ; itDamage!=itDamageEnd; ++itDamage )
								{
									if( (*itDamage)->s_nBaseType == HD_DAMAGE )
									{
										pDamageCheck = (sDAMAGE_INFO*)(*itDamage);
										if( pDamageCheck->s_eDamageType != DT_Dead )
										{
// 											CHAT_TEXT_DEGUG( _T("m_listHDProcess Cnt = %d"), m_listHDProcess.size() );
 											bDead = false;
										}
									}
								}

// 								if( ( bDead == true )&&
// 									( pParentAniProp->IsEnableSkill() == true )&&
// 									( pParentAniProp->IsEnableNormalAttack() == true ) )
								if( bDead == true && pParentAniProp->IsEnableDieAni() )
								{
									// 애니메이션 잠금 풀자
									m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
									SetHitTarget( NULL );

									m_pParent->SetDie();

									// 이동 잠금, 애니 잠금, 회전 잠금
									m_pParent->SetPauseAll( true );

									bErase = true;
								}
							}
							break;
						default:
							assert_cs( false );
						}

						if( bErase == true )
						{
							// 지워주자
							sDAMAGE_INFO::DeleteInstance( pDamage );
							it = m_listHDProcess.erase( it );
							continue;
						}
					}
					break;					
				case AT_JUST_NOW:
					{
						bool bErase = false;
						switch( pDamage->s_eDamageType )
						{
							// 맞았다~
						case DT_Normal:
						case DT_Block:
						case DT_Absorb:	// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
							{
								// Value값 적용
								for( int i=1; i<5; ++i )
								{
									if( pDamage->s_nValue[ i ] == 0 )
										continue;

									switch( i )
									{
									case 1:			// HP
										m_pParent->GetBaseStat()->SetDeltaHP( pDamage->s_nValue[ i ] );
										break;
									case 2:			// DS
										m_pParent->GetBaseStat()->SetDeltaDS( pDamage->s_nValue[ i ] );
										break;
									case 3:			// MHP
										m_pParent->GetBaseStat()->SetDeltaMaxHP( pDamage->s_nValue[ i ] );
										break;
									case 4:			// MDS
										m_pParent->GetBaseStat()->SetDeltaMaxDS( pDamage->s_nValue[ i ] );
										break;
									}
								}
								bErase = true;
							}
							break;						
							// 피했다
						case DT_Miss:
							{
								bErase = true;
							}
							break;
							// 맞고 죽었다
						case DT_Dead:
							{
								bool bDead = true;
								sDAMAGE_INFO* pDamageCheck;
								std::list< sHD_BASE* >::iterator itDamage = m_listHDProcess.begin();
								std::list< sHD_BASE* >::iterator itDamageEnd = m_listHDProcess.end();
								for( ; itDamage!=itDamageEnd; ++itDamage )
								{
									if( (*itDamage)->s_nBaseType == HD_DAMAGE )
									{
										pDamageCheck = (sDAMAGE_INFO*)(*itDamage);
										if( pDamageCheck->s_eDamageType != DT_Dead )
										{
											bDead = false;
										}
									}
								}
// 
// 								if( ( bDead == true )&&
// 									( pParentAniProp->IsEnableSkill() == true )&&
// 									( pParentAniProp->IsEnableNormalAttack() == true ) )
								if( bDead == true && pParentAniProp->IsEnableDieAni() )
								{
									// 애니메이션 잠금 풀자
									m_pParent->SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
									SetHitTarget( NULL );

									m_pParent->SetDie();

									// 이동 잠금, 애니 잠금, 회전 잠금
									m_pParent->SetPauseAll( true );

									bErase = true;
								}
							}
							break;
						default:
							assert_cs( false );
						}

						if( bErase == true )
						{
							// 지워주자
							sDAMAGE_INFO::DeleteInstance( pDamage );
							it = m_listHDProcess.erase( it );
							continue;
						}
					}
					break;
				case AT_NONE_ACTIVE:
					{
						// 활성화 안된채로 오랜시간이 지났는가
						if( pDamage->s_dwResistTime < dwCurTick )
						{
							pDamage->s_eActive = AT_ACTIVE;
						}
					}
					break;
				}
				++it;
			}
			break;
		case HD_EVENT:
			{
#ifdef LEVELUP_EFFECT_LOOP
				int nCount = m_pParent->GetProp_Effect()->GetEffectCount();
				CsC_EffectObject* pEffect = NULL;
				sEVENT_INFO* pInfo = (sEVENT_INFO*)*it;
				bool bLevelEffect = false;
				for( int i=0; i<nCount; ++i )
				{
					pEffect = (CsC_EffectObject*)m_pParent->GetProp_Effect()->GetEffect( i );

					// 현재 레벨업 이펙트가 렌더중이고 pInfo->s_eEventType가 레벨업 이벤트라면
					if( (pEffect->GetLevelEffect() == CsC_EffectObject::eLeveUp) && (pInfo->s_eEventType == EVENT_LEVEL_UP) )
					{
						bLevelEffect = true;
						break;
					}
				}

				// 현재 레벨업 이펙트가 렌더된지 0.7초가 지났다면 다음 레벨업 이펙트를 그릴 수 있다.
				if( bLevelEffect && (GetTickCount() - m_nTickTime > 700) )
				{
					bLevelEffect = false;

					// 현재 보여줘야 할 레벨보다 작은 레벨이 이벤트 리스트에 있는지 검사
					std::list< sHD_BASE* >::iterator it_Level = m_listHDProcess.begin();
					std::list< sHD_BASE* >::iterator itEnd_Level = m_listHDProcess.end();

					for( ; it_Level!= itEnd_Level; ++it_Level )
					{
						if( pInfo->s_nLevel > ((sEVENT_INFO*)*it_Level)->s_nLevel )
						{
							bLevelEffect = true;
							break;
						}						
					}
				}

				if( pParentAniProp->IsEnableSkill() == true && bLevelEffect == false )
				{					
					m_pParent->SceneAttackPropEvent( (sEVENT_INFO*)*it );
					
					// 레벨업 이벤트라면 add 시간 저장
					if( pInfo->s_eEventType == EVENT_LEVEL_UP )
					{
						m_nTickTime  = GetTickCount();
					}

					// 지워주자
					sEVENT_INFO::DeleteInstance( (sEVENT_INFO*)*it );
					it = m_listHDProcess.erase( it );
					continue;
				}
				++it;
#else
				if( pParentAniProp->IsEnableSkill() == true )
				{					
					m_pParent->SceneAttackPropEvent( (sEVENT_INFO*)*it );

					// 지워주자
					sEVENT_INFO::DeleteInstance( (sEVENT_INFO*)*it );
					it = m_listHDProcess.erase( it );
					continue;
				}
				++it;
#endif
			}			
			break;
		default:
			assert_cs( false );
		}
	}
}

bool CsC_AttackProp::IsSkillProcess()
{
	std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
	std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_nBaseType == HD_HIT )
		{
			if( ( (sHIT_INFO*)(*it) )->s_eHitType == HT_Skill 
				|| ( (sHIT_INFO*)(*it) )->s_eHitType == HT_Skill_NoAni )
				return true;
		}
	}
	return false;
}

bool CsC_AttackProp::IsAttack_SkillProcess()
{
	std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
	std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_nBaseType == HD_HIT )
		{
			switch( ( (sHIT_INFO*)(*it) )->s_eHitType )
			{
			case HT_Normal:
			case HT_Critical:
			case HT_Miss:
			case HT_Skill:
			case HT_Absorb:	// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
			case HT_Skill_NoAni:	
				return true;
			}
		}
	}
	return false;
}

bool CsC_AttackProp::IsConditionProcess()
{
	std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
	std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_nBaseType == HD_DAMAGE )
		{
			if( ( (sDAMAGE_INFO*)(*it) )->s_eDamageType == DT_Condition )
				return true;
		}
	}
	return false;
}

void CsC_AttackProp::AbsoluteSkillProcess()
{
	std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
	std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
	while( it!=itEnd )
	{
		if( (*it)->s_nBaseType == HD_HIT )
		{
			sHIT_INFO* pHitInfo = (sHIT_INFO*)(*it);
			switch( pHitInfo->s_eHitType )
			{
			case HT_Skill:
			case HT_Skill_NoAni:	
				{
					if( pHitInfo->s_pPartnerDamageInfo != NULL )
					{
						pHitInfo->s_pPartnerDamageInfo->s_eActive = AT_JUST_NOW;
						sDAMAGE_INFO* pChild = pHitInfo->s_pPartnerDamageInfo->s_pChildDamageInfo;
						while( pChild )
						{
							pChild->s_eActive = AT_JUST_NOW;
							pChild = pChild->s_pChildDamageInfo;
						}
					}
					it = m_listHDProcess.erase( it );
					continue;
				}
				break;
			}
		}
		++it;
	}
}

void CsC_AttackProp::ClearDamageValue()
{
	std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
	std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
	sDAMAGE_INFO* pDamageInfo;
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->s_nBaseType == HD_HIT )
			continue;

		pDamageInfo = (sDAMAGE_INFO*)(*it);

		if( pDamageInfo->s_eDamageType == DT_Normal )
		{
			memset( &pDamageInfo->s_nValue[ 1 ], 0, sizeof( int )*4 );
		}
	}
}

CsC_AvObject* CsC_AttackProp::GetHitTarget()
{
	return ( m_pHitTarget&&m_pHitTarget->IsLoad() ) ? m_pHitTarget : NULL;
}
void	CsC_AttackProp::AddMultyTarget(int MultyTargetID, CsC_AvObject* pkObject)
{
	MultyTargetIter kIter = mMultiTargets.find(MultyTargetID);
	if(kIter == mMultiTargets.end())
	{
		mMultiTargets.insert(MultyTargets::value_type(MultyTargetID, pkObject));
	}
	else
		kIter->second = pkObject;
}


void CsC_AttackProp::SetHitTarget( CsC_Object* pTarget )
{
	m_pHitTarget = (CsC_AvObject*)pTarget;
}

void CsC_AttackProp::SetFollowTarget( CsC_Object* pTarget, float fFollowRange, eFOLLOW_ITERATOR eFollowIterator, bool bIteratorRange )
{
	if( ( m_pFollowTarget != pTarget )||( m_eFollowIterator != eFollowIterator ) )
		ReleaseAttackingTarget();

	m_pFollowTarget = (CsC_AvObject*)pTarget;
	m_fFollowRange = fFollowRange;
	m_eFollowIterator = eFollowIterator;
	m_bFollowIteratorRange = bIteratorRange;
}

CsC_AttackProp::ePROCESS_STATE CsC_AttackProp::UpdateAttack( float fDeltaTime, eFOLLOW_ITERATOR& eFollowIterator )
{
	CsC_AvObject* pFollowTarget = GetFollowTarget();
	assert_cs( pFollowTarget != NULL );

	eFollowIterator = m_eFollowIterator;

	//// 때릴놈이 있을때
	//if( GetHitTarget() != NULL )
	//{
	//	// 따라 다니는놈과 때릴넘이 틀리면
	//	if( GetHitTarget() != GetFollowTarget() )
	//	{
	//		// 시간 체크 - 너무 자주 보내지 말자
	//		if( m_TargetChangeTimeSeq.IsEnable() == true )
	//		{
	//			m_bAttackingTarget = false;
	//			return CsC_AttackProp::SUCCESS_CHANGE_TARGET;
	//		}			
	//	}
	//}

	// 공격자와의 거리	
	NiPoint2 vSrc = NiPoint2( m_pParent->GetPos().x, m_pParent->GetPos().y );
	NiPoint2 vDest = NiPoint2( pFollowTarget->GetPos().x, pFollowTarget->GetPos().y );
	float fTargetDist = ( vSrc - vDest ).Length();

	// 공격 가능거리 - 내 공격 거리 + 상대방의 어택 몸 사이즈
	float fEnableAttackDist = GetEnableFollowDist();

	//===============================================================================================
	// 타겟이 사거리 내에 들어왔다면
	//===============================================================================================
	if( fTargetDist < fEnableAttackDist )		
	{
		m_TargetMoveTimeSeq.Reset();

		if( _IsAttackBlocking() == true )
		{
			m_bAttackingTarget = false;
			return CsC_AttackProp::FAILE_BLOCKING;
		}
		
		// 기존에 공격 중이 아니였다면 첫공격
		if( m_bAttackingTarget == false )
		{
			m_bAttackingTarget = true;
			return CsC_AttackProp::SUCCESS_ATTACK_START;
		}

		// 공격중이다
		return CsC_AttackProp::SUCCESS_ATTACK_ING;
	}

	// 공격 중이 아니기 때문에 취소
	m_bAttackingTarget = false;

	//===============================================================================================
	// 타겟이 사거리 안에 들어와 있지 않다.
	//===============================================================================================
	// 움직이는 중이다 -  이미 거리는 벗어 났다~ 쪼까가자
	if( pFollowTarget->GetProp_Path()->IsMove() == true )
	{
		// 나도 움직이는 중이 아니라면
		if( m_pParent->GetProp_Path()->IsMove() == false )
		{
			// 멈춰 있다면 무조건 움직이자
			m_TargetMoveTimeSeq.Reset();
		}

		// 너무 자주 계산하면 서버에서 화낸다~ 시간 봐서 자주된다면 보류
		if( m_TargetMoveTimeSeq.IsEnable() == true )
		{
			return FAILE_POS;
		}
		return FAILE_HOLDBACK;
	}

	// 상대가 멈춰 있다
	if( m_pParent->GetProp_Path()->IsMove() == false )
	{
		// 나도 멈춰 있다 - 나 움직여야지
		return FAILE_POS;
	}

	// 적을 잡으러 내가 움직이고 있다
	return SUCCESS_MOVING;
}

bool CsC_AttackProp::_IsAttackBlocking()
{
	if( nsCsGBTerrain::g_pCurRoot->IsReachableEmr( m_pParent->GetPos(), GetFollowTarget()->GetPos() ) )
	{
		return false;
	}
	return true;
}

float CsC_AttackProp::GetEnableFollowDist()
{
	return m_fFollowRange + GetFollowTarget()->GetAttackCharSize();
}

void CsC_AttackProp::ReleaseFollowTarget()
{ 
	m_pFollowTarget = NULL; 
	m_eFollowIterator = FI_ATTACK; 
	ReleaseAttackingTarget(); 
}

void CsC_AttackProp::ReleaseAttackingTarget()
{ 
	m_bAttackingTarget = false; 
}

bool CsC_AttackProp::IsAttackingTarget() const
{ 
	return m_bAttackingTarget; 
}

CsC_AttackProp::eFOLLOW_ITERATOR CsC_AttackProp::GetFollowIterator() const
{ 
	return m_eFollowIterator; 
}

bool CsC_AttackProp::IsFollowIteratorRange() const
{ 
	return m_bFollowIteratorRange; 
}
//==========================================================================
//
//	Hit, Damage
//
//==========================================================================

void CsC_AttackProp::InsertHit( sHIT_INFO* pSrcHitInfo )
{
	sHIT_INFO* pHitInfo = sHIT_INFO::NewInstance();
	memcpy( pHitInfo, pSrcHitInfo, sizeof( sHIT_INFO ) );

	// 스킬이면 정보 날려 준다
	if( pHitInfo->s_eHitType == HT_Skill 
		|| pHitInfo->s_eHitType == HT_Skill_NoAni )
	{
		std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
		std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
		while( it!=itEnd )
		{
			// 일단 아래거 보류.. 스킬이 존재 한다면 뻑내자
			switch( (*it)->s_nBaseType )
			{
			case HD_HIT:
				{
					sHIT_INFO* pHit = (sHIT_INFO*)(*it);

					// 기존에 평타 데미지 한방에 넣자
					if( pHit->s_pPartnerDamageInfo != NULL )
					{
						pHit->s_pPartnerDamageInfo->s_eActive = AT_JUST_NOW;
						sDAMAGE_INFO* pChild = pHit->s_pPartnerDamageInfo->s_pChildDamageInfo;
						while( pChild )
						{
							pChild->s_eActive = AT_JUST_NOW;
							pChild = pChild->s_pChildDamageInfo;
						}
					}

					sHIT_INFO::DeleteInstance( pHit );
					it = m_listHDProcess.erase( it );
				}				
				continue;
			}
			++it;
		}
	}

	m_listHDProcess.push_back( pHitInfo );
}
void	CsC_AttackProp::InsertMultiHit(sHIT_INFO* pSrcHitInfo)
{
	sHIT_INFO* pHitInfo = sHIT_INFO::NewInstance();
	memcpy( pHitInfo, pSrcHitInfo, sizeof( sHIT_INFO ) );

	// 스킬이면 정보 날려 준다
	if( pHitInfo->s_eHitType == HT_Skill 
		|| pHitInfo->s_eHitType == HT_Skill_NoAni )
	{
		std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
		std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
		while( it!=itEnd )
		{
			// 일단 아래거 보류.. 스킬이 존재 한다면 뻑내자
			switch( (*it)->s_nBaseType )
			{
			case HD_HIT:
				{
					sHIT_INFO* pHit = (sHIT_INFO*)(*it);

					// 기존에 평타 데미지 한방에 넣자
					if( pHit->s_pPartnerDamageInfo != NULL )
					{
						pHit->s_pPartnerDamageInfo->s_eActive = AT_JUST_NOW;
						sDAMAGE_INFO* pChild = pHit->s_pPartnerDamageInfo->s_pChildDamageInfo;
						while( pChild )
						{
							pChild->s_eActive = AT_JUST_NOW;
							pChild = pChild->s_pChildDamageInfo;
						}
					}
				}				
			}
			++it;
		}
	}

	m_listHDProcess.push_back( pHitInfo );
}
//void CsC_AttackProp::InsertHitEnd( CsC_AvObject* pHitTarget, eHIT_TYPE ht )
//{
//	sHIT_INFO* pHitInfo = sHIT_INFO::NewInstance();
//	pHitInfo->s_bKill = true;
//	pHitInfo->s_eHitType = ht;
//	pHitInfo->s_pHitTarget = pHitTarget;
//	m_listHDProcess.push_back( pHitInfo );
//}

CsC_AttackProp::sDAMAGE_INFO* CsC_AttackProp::InsertDamage( sDAMAGE_INFO* pSrcDamageInfo )
{
	sDAMAGE_INFO* pDamageInfo = sDAMAGE_INFO::NewInstance();
	memcpy( pDamageInfo, pSrcDamageInfo, sizeof( sDAMAGE_INFO ) );
	m_listHDProcess.push_back( pDamageInfo );

	return pDamageInfo;
}

void CsC_AttackProp::InsertDamageEnd()
{
	sDAMAGE_INFO* pDamageInfo = sDAMAGE_INFO::NewInstance();
	pDamageInfo->s_eDamageType = DT_Dead;	
	pDamageInfo->s_eActive = AT_ACTIVE;
	
	m_listHDProcess.push_back( pDamageInfo );
}

void CsC_AttackProp::InsertEvent( eEVENT_TYPE eType, int nValue1, int nValue2, UINT nUnsignValue1, UINT nUnsignValue2 )
{
	sEVENT_INFO* pEvent = sEVENT_INFO::NewInstance();
	pEvent->s_eEventType = eType;
	pEvent->s_nValue1 = nValue1;
	pEvent->s_nValue2 = nValue2;
	pEvent->s_nUnSignValue1 = nUnsignValue1;
	pEvent->s_nUnSignValue2 = nUnsignValue2;

	m_listHDProcess.push_back( pEvent );
}

void CsC_AttackProp::DeleteHitEvent()
{
	std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
	std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
	while( it != itEnd )
	{
		// 일단 아래거 보류.. 스킬이 존재 한다면 뻑내자
		switch( (*it)->s_nBaseType )
		{
		case HD_HIT:
			{
				sHIT_INFO* pHit = (sHIT_INFO*)(*it);
				sHIT_INFO::DeleteInstance( pHit );
				it = m_listHDProcess.erase( it );
			}	
			break;
		default:
			 ++it;
			 break;
		}
	}
}

void CsC_AttackProp::DeleteDemageEvent()
{
	std::list< sHD_BASE* >::iterator it = m_listHDProcess.begin();
	std::list< sHD_BASE* >::iterator itEnd = m_listHDProcess.end();
	while( it != itEnd )
	{
		// 일단 아래거 보류.. 스킬이 존재 한다면 뻑내자
		switch( (*it)->s_nBaseType )
		{
		case HD_DAMAGE:
			{
				sDAMAGE_INFO* pHit = (sDAMAGE_INFO*)(*it);
				sDAMAGE_INFO::DeleteInstance( pHit );
				it = m_listHDProcess.erase( it );
			}	
			break;
		default:
			++it;
			break;
		}
	}
}

void sRECV_HITTER_INFO::CalInfo( CsC_AvObject* pHitter )
{
	CsC_AniProp* pHitterAniProp = pHitter->GetProp_Animation();
	CsC_AniProp_Sequence* pHitterAniPropSeq = pHitterAniProp->GetSequenceMng();
	assert_cs( pHitterAniPropSeq );

	s_bRangeAttack = pHitter->IsAttackRangeType();
	s_AniID = pHitterAniPropSeq->CalNormalAttackAni( s_HitType );
	
	g_CsLastError.m_eType = CsLastError::__attack;
	g_CsLastError.m_nIntValue1 = pHitter->GetModelID();
	g_CsLastError.m_nIntValue2 = s_AniID;	
	g_CsLastError.m_nIntValue3 = pHitter->IsLoad();
	g_CsLastError.m_nIntValue4 = pHitter->__GetRefLoadCnt();

	g_CsLastError.m_pValue1 = pHitterAniPropSeq->GetLoadSequence();
	g_CsLastError.m_pValue2 = pHitterAniProp->GetSequenceMng();		

	g_CsLastError.m_nIntValue5 = (int)pHitterAniPropSeq->GetLoadSequence()->GetSequenceMap()->size();

#ifdef CRASHDUMP_0326			//chu8820 - 시퀀스 로드 실패 ( C008 )	// 140327 - 시퀀스 로드 실패 부분 추가 수정 lks007
	CsC_LoadSequence::sSEQUENCE_INFO* pSequence = pHitterAniPropSeq->GetSequence( s_AniID );

	// 10%의 아이들 여유시간을 두겠다.
	assert_cs( pSequence != NULL );

	if( pSequence == NULL )
	{
		DUMPLOG( " C008 - %d, %d ", pHitter->GetFTID(), s_AniID );
		g_CsLastError.Release();
		return;
	}

	float fEndTime = pSequence->s_fEndTime;
	s_fAniAccelRate = pHitterAniProp->CalAniAccelRate( s_AniID, CsMin(fEndTime, pHitter->GetAttackSpeed()) );// 

	g_CsLastError.Release();
#else
	// 10%의 아이들 여유시간을 두겠다.
	assert_cs( pHitterAniPropSeq->IsSequence( s_AniID ) == true );

	float fEndTime = pHitterAniPropSeq->GetSequence( s_AniID )->s_fEndTime;
	// 서버와 공격속도 동기화시 애니메시션 속도가 서버의 다음 어텍 속도보다 느리기 때문에
	// 실제 공격 속도보다 조금 빠르게 애니메시션 플레이를 시키기 위해 0.98을 곱함. 
	// 애니메이션 속보다 0.02% 빠르게 애니 플레이 시킴
	s_fAniAccelRate = pHitterAniProp->CalAniAccelRate( s_AniID, CsMin(fEndTime, pHitter->GetAttackSpeed()) );

	g_CsLastError.Release();
#endif
}


NiPoint2 CsC_AttackProp::CalKnockBackPos( NiPoint2 vHitter, NiPoint2 vTarget, CsC_PathProp* pTargetPathProp, float fDist )
{	
	NiPoint2 vDir =	( vTarget - vHitter );
	vDir.Unitize();
	NiPoint2 vDst = vTarget + vDir*fDist;

	
	CsPath* pPath = pTargetPathProp->GetPath();
	if( g_pCsPathEngine->PathTestLine( pPath, vDst.x, vDst.y ) == false )
	{
		// 충돌
		CsPoint res;
		if( g_pCsPathEngine->GetFirstCollisionPoint( pPath, vDst.x, vDst.y, res ) == true )
		{
			// 첫 충돌지점 검출
			NiPoint2 vCollision( (float)res.x - vTarget.x, (float)res.y - vTarget.y );			
			vDst = vTarget + vDir*vCollision.Length();
		}

		while( true )
		{
			if( ( nsCsGBTerrain::g_pCurRoot->IsValidEmr( vDst ) == true )&&
				( pTargetPathProp->PathTestLine( vDst ) == true ) )
			{
				// 임시 셋팅~!!
				break;
			}
			// 도착 방향의 반대로 계속 이동시켜 주면서 검사
			vDst.x -= vDir.x * 25.f;
			vDst.y -= vDir.y * 25.f;

			// 가야할 거리보다 더 멀어진다면~ 리턴
			if( fDist < ( vDst - vTarget ).Length() )
			{
				return vTarget;
			}
		}
	}
	return vDst;
}


#undef PARENT_CLASS
