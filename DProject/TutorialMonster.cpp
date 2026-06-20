#include "stdafx.h"
#include "TutorialMonster.h"


#define		PARENT_CLASS		CMonster
CsCRTTI_CPP( PARENT_CLASS, CTutorialMonster, RTTI_TUTORIAL_MONSTER )

CTutorialMonster::CTutorialMonster() : m_fClientAttackTime(0.0f), m_nAIType(0)
{

}

CTutorialMonster::~CTutorialMonster()
{

}

void CTutorialMonster::Init(uint nUID, uint nFileTableID, nSync::Pos sPos, float fRot, sCREATEINFO* pProp, eMONSTER_STATE MonsterState)
{
	NiPoint3 vPos = NiPoint3( (float)sPos.m_nX, (float)sPos.m_nY, 0.0f );
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	m_nExpireTimeTS = 0;
	m_nTotalExpireTime = 0;

	// 파일 테이블
	assert_cs( m_pFT_Monster == NULL );
	m_pFT_Monster = nsCsFileTable::g_pMonsterMng->GetMonster( nFileTableID );
	assert_csm1( m_pFT_Monster != NULL, _T( "테이블 아이디%d" ), nFileTableID );
	m_pFT_Digimon = nsCsFileTable::g_pDigimonMng->GetDigimon( GetModelID() );
	m_eNatureType = m_pFT_Digimon->GetInfo()->s_eBaseNatureType;
	memcpy(&m_eNatureTypes, m_pFT_Digimon->GetInfo()->s_eBaseNatureTypes, sizeof(nsCsDigimonTable::eNATURE_TYPE)*3);
	// 파일테일블에서의 스케일

	CsMonster::sINFO * pMonsterInfo = m_pFT_Monster->GetInfo();
	m_fScaleFT = pMonsterInfo->s_fScale;

	m_SkillMng.SetDigimon( this, m_pFT_Digimon );
	m_Condition.Init( this );

	// 초기화
	CsC_AvObject::Init( nUID, nFileTableID, vPos, fRot, pProp );

	GetBaseStat()->SetHP( 255 );

	// 보스몬스터는 오라 달아주자
	if( pMonsterInfo->s_nClass == 4 )
		GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BOSS, "system\\LevelEffect\\BossEffect.nif" );
	else if( pMonsterInfo->s_nClass == 7 || pMonsterInfo->s_nClass == 8 )
		GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BOSS, "system\\D_BlackWheel\\D_BlackWheel_E.nif" );

	_Init( MonsterState );
#ifdef MONSTER_SKILL_GROWTH
	_SetOrgScale( CsC_AvObject::GetGameScale() );	//	기본 스케일 셋팅( 몬스터 커졌다가 줄어들 때 이값 사용 )
#endif
	m_bIsCasting = false;
}

void CTutorialMonster::Update(float fDeltaTime, bool bAnimation /*= true */)
{
	// 버프 데이타 업데이트
	m_BuffData.Update();

	if( IsProperty( CsC_Property::PATH ) )
	{
		if( ( GetProp_Path()->IsMove() == true )&&( IsPause( PAUSE_PATH ) == false ) )
		{
			_SetDefault_IdleAnimation_Move();
		}
	}	

	CsC_AvObject::Update( fDeltaTime, bAnimation );

	// 죽었는데도 공격 애니 들어와서 server die 이면
	// 죽는 애니로 변경 후 다른 업데이트 안 함
	if (IsServerDie())
	{
		if (GetProp_Animation()->GetAnimationID() != ANI::DAMAGE_DEAD)
			SetAnimation(ANI::DAMAGE_DEAD);
		return;
	}

	if( _CheckSight() )
		_Ting();

	switch( m_eMonsterState )
	{
	case MONSTER_CREATE:
		_UpdateCreateScene( fDeltaTime );
		break;
	case MONSTER_BATTLE:
		_UpdateAttack_ClientMode( fDeltaTime );
		break;
	case CMonster::MONSTER_DIE:
		break;
	default:
		{
			SetHeightConstant( 0 );
			g_pHpBar->SetBuddy( cHpBar::HP, GetUniqID() );
		}
		break;
	}	
#ifdef MONSTER_SKILL_GROWTH
	if( m_BuffData.IsBuffData( nsBuff::BK_MONSTER_GROWTH ) == false	//	성장 버프 없으면 또는
		|| m_eMonsterState == MONSTER_NORMAL )	//	전투 풀리면 크기 줄이자
	{
		float fCurScale = PARENT_CLASS::GetGameScale();
		if( _GetOrgScale() < fCurScale )
		{
			fCurScale -= 0.03f;
			if( fCurScale < _GetOrgScale() )
				PARENT_CLASS::SetScale( _GetOrgScale() );
			else
				PARENT_CLASS::SetScale( fCurScale );
		}
	}
#endif
}

void CTutorialMonster::_UpdateAttack_ClientMode(float fDeltaTime)
{
	// 전투중 인가
	CsC_AvObject* pTarget = GetProp_Attack()->GetFollowTarget();
	SAFE_POINTER_RET( pTarget );

	CsC_AvObject* pRotationTarget = GetProp_Attack()->GetHitTarget();
	if( pRotationTarget == NULL )
		pRotationTarget = pTarget;

	CsC_AttackProp::eFOLLOW_ITERATOR eFollowIterator;
	switch( GetProp_Attack()->UpdateAttack( fDeltaTime, eFollowIterator ) )
	{	
		// 공격 시작
	case CsC_AttackProp::SUCCESS_ATTACK_START:
		{
			SetRotation_Ani( pRotationTarget );

			sRECV_HITTER_INFO hi;
			hi.s_nUID = GetUniqID();
			hi.s_HitType = CsC_AttackProp::HT_Skill;

			sRECV_TARGET_INFO ti;
			ti.s_nUID = GetProp_Attack()->GetFollowTarget()->GetUniqID();
			ti.s_DamageType = CsC_AttackProp::DT_Normal;
			ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;

			switch( eFollowIterator )
			{
			case CsC_AttackProp::FI_SKILL1:		hi.s_nSkillIndex = 0;			break;
			case CsC_AttackProp::FI_SKILL2:		hi.s_nSkillIndex = 1;			break;
			case CsC_AttackProp::FI_SKILL3:		hi.s_nSkillIndex = 2;			break;
			case CsC_AttackProp::FI_SKILL4:		hi.s_nSkillIndex = 3;			break;
			case CsC_AttackProp::FI_SKILL5:		hi.s_nSkillIndex = 4;			break;
			default:															return;
			}

			m_fClientAttackTime = 8000.0f;

			if( pTarget->GetClientPlag() & CP_DMG_SKILL )
			{
				if (pTarget->GetBaseStat()->GetHPRate() <= 0.1f)
				{
					ti.s_nValue[0] = 0;
					ti.s_DamageType = CsC_AttackProp::DT_Miss;
				}
				else
				{
					ti.s_nValue[0] = -150;
					pTarget->GetBaseStat()->SetDeltaHP(-150);
				}
			}
			else
			{
				ti.s_nValue[ 0 ] = 0;
				pTarget->GetBaseStat()->SetDeltaHP( 0 );
			}

//			if( pTarget->GetBaseStat()->GetHP() == 0 )
//				cCliGame::SetDieInfo( GetProp_Attack()->GetFollowTarget() );
		}		
		break;

		// 공격중
	case CsC_AttackProp::SUCCESS_ATTACK_ING:
		{			
			SetRotation_Ani( pRotationTarget );

			if( eFollowIterator == CsC_AttackProp::FI_ATTACK )
			{
				m_fClientAttackTime += g_fDeltaTime;

				if( ( m_fClientAttackTime > GetAttackSpeed() )&&( pTarget->GetBaseStat()->GetHP() > 0 ) )
				{
					sRECV_HITTER_INFO hitter;
					hitter.s_nUID = GetUniqID();	
					hitter.s_HitType = CsC_AttackProp::HT_Normal;					

					sRECV_TARGET_INFO target;
					target.s_nUID = pTarget->GetUniqID();
					target.s_DamageType = CsC_AttackProp::DT_Normal;
					target.s_nNumEffectType = NUMTYPE::ET_NORMAL;

					// 기본 공격력 구한다
					FMDigimon::sDINFO info;
					CsDigimon::sINFO* pFTInfo = GetDigimonFT()->GetInfo();
					if( pFTInfo )
					{
						info.s_nDigimonID		= pFTInfo->s_dwDigimonID;
						info.s_nLevel			= GetBaseStat()->GetLevel();
						info.s_fGameOrgScale	= GetOrgGameScale();
						target.s_nValue[0] = FMDigimon::GetBaseAtt( &info ) * -1;
					}
					else
						target.s_nValue[0] = -150;

					if( pTarget->GetClientPlag() & CP_DMG_NORMAL )
					{
						if (pTarget->GetBaseStat()->GetHPRate() <= 0.1f)
						{
							target.s_nValue[0] = 0;
							target.s_DamageType = CsC_AttackProp::DT_Miss;
						}
						else
							pTarget->GetBaseStat()->SetDeltaHP(target.s_nValue[0]);
					}
					else
					{
						target.s_nValue[ 0 ] = 0;
						pTarget->GetBaseStat()->SetDeltaHP( 0 );
					}

					cCliGame::SetAttackInfo( &hitter, &target, false );

//					if( pTarget->GetBaseStat()->GetHP() == 0 )
//					{
//						cCliGame::SetDieInfo( GetProp_Attack()->GetFollowTarget() );
//						SetBattleEnd();
//					}

					m_fClientAttackTime = 0.0f;
				}
			}
		}
		break;

		// 추적 중
	case CsC_AttackProp::SUCCESS_MOVING:
		{
			// 시야에 안 들어오면 타겟팅 해제
			if( !_CheckSight() )
				SetBattleEnd();
		}
		break;

		// 위치가 틀리다 - 다시 움직이자
	case CsC_AttackProp::FAILE_POS:		
		_MoveToAttackTargetPos();
		break;
	}
}

void CTutorialMonster::_SkillAttack(ushort const& nSkillIdx)
{
	SAFE_POINTER_RET( nsCsFileTable::g_pMonsterMng );
	CsMonsterSkill::sINFO* pSkillInfo =  nsCsFileTable::g_pMonsterMng->GetMonsterSkill( nSkillIdx );
	SAFE_POINTER_RET( pSkillInfo );

	//=============================================================================
	// 스킬 사용하는 몬스터 애니 셋팅
	//=============================================================================
	if( pSkillInfo->s_nCastCheck == 1 )
	{
		SetPause( CsC_AvObject::PAUSE_PATH, true );
		//시전 애니
		GetProp_Animation()->ResetAnimation();
		SetAnimation(  pSkillInfo->s_dwSequenceID + 20 );
		SetCasting_Monster( true );
	}
}

bool CTutorialMonster::_CheckSight()
{
	if( m_eMonsterState == MONSTER_CREATE || m_eMonsterState == MONSTER_DIE )
		return false;

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RETVAL( pDUser, false );

	SAFE_POINTER_RETVAL( m_pFT_Monster, false );
	CsMonster::sINFO* pFTInfo = m_pFT_Monster->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, false );

	// 비선공
	if( nMonster::Guarder == pFTInfo->s_nBattle - 1 )
		return false;

	if( pDUser->CheckTargetDist( this, pFTInfo->s_nSight ) )
		return true;

	return false;
}

void CTutorialMonster::_Ting()
{
	if( m_eMonsterState == MONSTER_BATTLE )
		return;

	DWORD dwPlag = nsEFFECT::POS_CHARPOS | nsEFFECT::OFFSET_USE;
	NiPoint3 vOffset = NiPoint3( 0, 0, GetToolHeight() + 30.0f );
	CsC_EffectObject* pEffect = GetProp_Effect()->AddEffect( "system\\Mark_Surprise.nif", sqrt( GetToolWidth()*0.01f ), dwPlag );
	if( pEffect )
		pEffect->SetOffset( vOffset );
	PlaySound( "System\\Enemy_warning.wav" );

	PARENT_CLASS::SetBattle();

	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pDUser );

	SAFE_POINTER_RET( m_pFT_Monster );
	CsMonster::sINFO* pFTInfo = m_pFT_Monster->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	GetProp_Attack()->SetFollowTarget( pDUser, pFTInfo->s_nAttRange, CsC_AttackProp::FI_ATTACK, IsAttackRangeType() );

	if( nMonster::Guarder != pFTInfo->s_nBattle - 1 )
		_MoveToAttackTargetPos();
}

void CTutorialMonster::_MoveToAttackTargetPos()
{
	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pDUser );
	NiPoint2 pos( pDUser->GetPos2D().x, pDUser->GetPos2D().y );
	if( GetProp_Path()->PathTestLine( pos ) )
	{
		GetProp_Path()->SetMoveState( CsC_PathProp::MS_RUN );
		GetProp_Path()->SetPath( (float)pos.x, (float)pos.y );
	}
}

void CTutorialMonster::AttachCamera_Critical(CsC_AvObject* pTarget)
{
	SAFE_POINTER_RET( pTarget );
	if( pTarget->GetLeafRTTI() != RTTI_TUTORIAL_DIGIMON )
		return;

	if( g_pResist->m_Global.s_bCameraEnemyCritical == FALSE )
		return;

	CAMERA_ST.AttachCamera( "Critical.nif", false );
}

void CTutorialMonster::SetBattle()
{
	_Ting();
}

void CTutorialMonster::SetBattleEnd()
{
	PARENT_CLASS::SetBattleEnd();

	GetProp_Attack()->ReleaseFollowTarget();
	DeletePath();
}