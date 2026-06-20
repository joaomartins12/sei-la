#include "stdafx.h"
#include "TutorialDigimonUser.h"

#define		PARENT_CLASS	CDigimonUser
CsCRTTI_CPP( PARENT_CLASS, CTutorialDigimonUser, RTTI_TUTORIAL_DIGIMON )

CTutorialDigimonUser::CTutorialDigimonUser()
{

}

CTutorialDigimonUser::~CTutorialDigimonUser()
{

}

void CTutorialDigimonUser::Init(uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, int nUserIndex, bool bInitPos)
{
	NiPoint3 vPos = NiPoint3( (float)sPos.m_nX, (float)sPos.m_nY, 0.0f );
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	Init( nIDX, nFileTableID, dwBaseDigimonID, vPos, fRot, szName, pProp, nUserIndex, bInitPos );
}

void CTutorialDigimonUser::Init(uint nIDX, uint nFileTableID, DWORD dwBaseDigimonID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, int nUserIndex, bool bInitPos)
{
	assert_cs( m_pBaseStat == NULL );
	assert_cs( pProp->IsOptionPlag( OP_STAT ) );
	m_pBaseStat = NiNew sUSER_STAT( this );

	CDigimon::Init( nIDX, nFileTableID, vPos, fRot, szName, pProp );

	m_dwBaseDigimonID = dwBaseDigimonID;
	m_dwPrevBaseDigimonID = dwBaseDigimonID;
	m_nDigimonUserIndex = nUserIndex;	
	m_UserServerMove.Init( this );

	if( bInitPos == true )
		_InitStartPos();

	m_pFT_Evol = nsCsFileTable::g_pEvolMng->GetEvolveInfo( dwBaseDigimonID );

	m_IdleCheckTimeSeq.SetDeltaTime( 60000 );
	_Init();

	m_MoveCoolTimeSeq.SetEndTime( 0.3f );
	m_MoveOnceTimeSeq.SetDeltaTime( 200 );
	m_BattleEPDecrTimeSeq.SetDeltaTime( 5000 );
	m_BattleEPDecrTimeSeq.SetEnable();

	// 상태
	m_pFindTarget = NULL;

	switch( nsCsGBTerrain::g_nSvrLibType )
	{
	case nLIB::SVR_DUNGEON:
	case nLIB::SVR_GAME:
#ifdef KEYBOARD_MOVE
		if( g_pResist->m_Global.s_bMoveDigimon )
		{
			m_eDigimonUserState = DU_WAIT;
			m_eDigimonUserState_NoneSkill = DU_WAIT;
		}
		else
		{
			m_eDigimonUserState = DU_FOLLOW;
			m_eDigimonUserState_NoneSkill = DU_FOLLOW;
		}
#else
		m_eDigimonUserState = DU_FOLLOW;
		m_eDigimonUserState_NoneSkill = DU_FOLLOW;
#endif
		break;
	case nLIB::SVR_BATTLE:
		m_eDigimonUserState = DU_MANUAL_CONTROL;
		m_eDigimonUserState_NoneSkill = DU_MANUAL_CONTROL;
		break;
	default:
		assert_cs( false );
	}	

	m_SkillRevTimeSeq.SetDeltaTime( 3000 );
	m_SkillRevTimeSeq.OffEnable();

	//스킬 쿨 초기화 옵션 관련 타임 변수
	m_ResetTime.SetEnableTime(false);
	m_ResetTime.Reset();
}

void CTutorialDigimonUser::_Init()
{
	// 여긴 모델 변경, 진화, 초기화 다 들어 온다.
	if( net::game == NULL )
		_NetOffInit();

	m_bWarningHP = m_bWarningDS = false;
	m_IdleCheckTimeSeq.OffReset();

	m_fClientAttackTime = 8000.0f;

	// connect ui
	SAFE_POINTER_RET( g_pGameIF );

	// set partner digimon window
	cMainDigimonWindow* pMainDigimonWin = g_pGameIF->GetMainDigimonWindow();
	if( pMainDigimonWin )
		pMainDigimonWin->SetTargetObject( this );

	// set digimon quick slot ui, data
	cQuickSlot* pQuickSlot = g_pGameIF->GetQuickSlot( cQuickSlot::Q_DIGIMON );
	if( pQuickSlot )
	{
		pQuickSlot->SetTargetObject( this );
		pQuickSlot->LockSlotAll();
	}
	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( m_nDigimonUserIndex + 1 );
	if( pDataQuickSlot )
	{
		pDataQuickSlot->SetChar( this );
		m_pQuickSlot = pDataQuickSlot->GetData( GetFTEvolCurObj()->m_nEvoSlot );
	}

	// set digimon buff info
	cBuffInfo* pDigimonBuff = g_pGameIF->GetBuffInfo( IF_BUFF_MY_D );
	if( pDigimonBuff )
		pDigimonBuff->SetBuffData( NULL );
}

void CTutorialDigimonUser::Update(float fDeltaTime, bool bAnimation /*= true */)
{
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTUser );
	CsC_AniProp* pTUAniProp = pTUser->GetProp_Animation();
	SAFE_POINTER_RET( pTUAniProp );

	bool bBackupAni = CsC_AniProp::g_bUpdateActor;
	CsC_AniProp::g_bUpdateActor = true;

#ifdef KEYBOARD_MOVE
	if( !g_pMngCollector->IsSceneState( CMngCollector::eSCENE_TUTORIAL_EVENT ) 
		|| pTUAniProp->GetAnimationID() == ANI::DIGIVICE_CHAGE
		|| pTUAniProp->GetAnimationID() == ANI::DIGIMON_EVOLUTION
		|| pTUAniProp->GetAnimationID() == ANI::DIGIMON_EVOLUTION_DOWN
		|| pTUAniProp->GetAnimationID() == ANI::DIGIMON_EVOLUTION_END )
	{
		if( GetProp_Path()->IsMove() )
			GetProp_Path()->DeletePath();
	}

	if( GetProp_Path()->IsMove() == false && GetProp_Animation()->GetAnimationID() == ANI::MOVE_RUN
		&& IsRide() == false )
		GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
#endif

	// 디지몬 유저 스테이트 적용 가능하면 적용
	if( _IsApplyDUState( m_eDigimonUserState ) &&( IsRide() == false ) )
	{
		switch( m_eDigimonUserState )
		{
		case DU_WAIT:
			{
				assert_cs( GetProp_Attack()->GetFollowTarget() == NULL );
				_UpdateTamerDist();
			}
			break;		
		case DU_FOLLOW:
			{
				assert_cs( GetProp_Attack()->GetFollowTarget() == NULL );
				_UpdateMove();
			}
			break;
		case DU_SKILL:
		case DU_ATTACK:
			{
				switch( nsCsGBTerrain::g_nSvrLibType )
				{
				case nLIB::SVR_DUNGEON:
				case nLIB::SVR_GAME:
					{
						if( g_pMngCollector && g_pMngCollector->IsSceneState( CMngCollector::eSCENE_TUTORIAL_EVENT ) )
							_UpdateAttack_ClientMode( fDeltaTime );
					}
					break;
				case nLIB::SVR_BATTLE:
					break;
				default:
					assert_cs( false );
				}				
			}
			break;
		case DU_MANUAL_CONTROL:
		case DU_WAIT_REV_SKILL:
		case DU_NPCFIND:
			break;
#ifdef KEYBOARD_MOVE
		case DU_KEYBOARD_MOVE:
			{
				if( g_pServerMoveOwner->GetCurState() == g_pServerMoveOwner->GetBackupState() )
				{
					//_UpdatePortal(fDeltaTime);

					DWORD nKeyCheck = m_UserServerMove.GetKeyCheck();
					if( nKeyCheck & CUserServerMove::KEY_LEFT ||
						nKeyCheck & CUserServerMove::KEY_RIGHT ||
						nKeyCheck & CUserServerMove::KEY_UP ||
						nKeyCheck & CUserServerMove::KEY_DOWN )
					{
						if( g_pCharMng->GetTamerUser()->IsFollowPortalMove() == false
							&& cMessageBox::IsMessageBox() )
							g_pCharMng->GetTamerUser()->SetFollowPortalMove( true );

						_StopAttack( false );
						GetProp_Attack()->ReleaseFollowTarget();
						if(	GetProp_Animation()->GetAnimationID() != ANI::MOVE_RUN )
							GetProp_Animation()->SetAnimation( ANI::MOVE_RUN );
					}
					else
					{
						m_eDigimonUserState = DU_WAIT;
						GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
					}
				}
				else if( GetProp_Animation()->GetAnimationID() == ANI::MOVE_RUN )
				{
					GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
				}
			}
			break;
#endif
		default:
			assert_cs( false );
		}
	}	

	// HP, DS 체크
	if( ( m_bWarningHP == false )&&( GetBaseStat()->GetHPRate() < 0.3f ) )
	{
		cDigimonTalk::Print( 13000 );
		m_bWarningHP = true;
	}
	if( GetBaseStat()->GetHPRate() > 0.3f )
	{
		m_bWarningHP = false;
	}
	if( ( m_bWarningDS == false )&&( GetBaseStat()->GetDSRate() < 0.1f ) )
	{
		cDigimonTalk::Print( 13005 );
		m_bWarningDS = true;
	}
	if( GetBaseStat()->GetDSRate() > 0.1f )
	{
		m_bWarningDS = false;
	}

	if( IsRide() == false )
	{
		g_pHpBar->SetBuddy( cHpBar::HPDS, GetUniqID() );
	}	

	CDigimon::Update( fDeltaTime, bAnimation );

	// 서버 이동
	if( IsPause( PAUSE_PATH ) == false )
		m_UserServerMove.SetServerMove( GetUniqID(), GetPos().x, GetPos().y, GetCurRot(), GetProp_Path()->GetPath()->m_iPath );
	m_UserServerMove.Update_SendMove_Server();

	if( GetDigimonState() == CDigimon::DIGIMON_DIE )
	{
		if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_GAME || nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_DUNGEON )
		{
			if( ( cMessageBox::IsMessageBox( cPrintMsg::GAME_DEAD ) == false )&&( net::game->IsSendBlock() == false ) )
			{
				cPrintMsg::PrintMsg(cPrintMsg::GAME_DEAD);
			}
		}		
	}

	CsC_AniProp::g_bUpdateActor = bBackupAni;
}

void CTutorialDigimonUser::_UpdateAttack_ClientMode(float fDeltaTime)
{
	m_IdleCheckTimeSeq.OffReset();
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

			// 스킬 공격력 구한다
			DWORD dwSkillCode = 0;
			cSkill::sINFO* pSkillInfo = GetSkillMng()->GetSkill( hi.s_nSkillIndex );
			if( pSkillInfo )
			{
				CsSkill* pFTSkill = pSkillInfo->s_pFTSkill;
				if( pFTSkill && pFTSkill->GetInfo() )
					dwSkillCode = pFTSkill->GetInfo()->s_dwID;
			}
			int nSkillAtt = FMCommon::GetSkillAtt( dwSkillCode, nLimit::SkillLevel, 0 );
			ti.s_nValue[0] = -nSkillAtt;

			m_fClientAttackTime = 8000.0f;

			if( pTarget->GetClientPlag() & CP_DMG_SKILL )
			{
				int nDamage = 0;
				// 스킬 공격
				switch( eFollowIterator )
				{
				case CsC_AttackProp::FI_SKILL1:		nDamage = -77;			break;
				case CsC_AttackProp::FI_SKILL2:		nDamage = -102;			break;
				case CsC_AttackProp::FI_SKILL3:		nDamage = -255;			break;
				case CsC_AttackProp::FI_SKILL4:		nDamage = -60;			break;
				case CsC_AttackProp::FI_SKILL5:		nDamage = -70;			break;
				}
				pTarget->GetBaseStat()->SetDeltaHP( nDamage );	// 비율로 깎는다 255분율
				if( pTarget->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER )
				{
					CTutorialMonster* pTutoMon = dynamic_cast<CTutorialMonster*>(pTarget);
					if( !pTutoMon->IsMonsterState( CMonster::MONSTER_BATTLE ) )
						pTutoMon->SetBattle();
				}
			}
			else
			{
				ti.s_nValue[ 0 ] = 0;
				pTarget->GetBaseStat()->SetDeltaHP( 0 );
			}

			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::USE_DIGIMON_SKILL, 0, &eFollowIterator );
			cCliGame::SetSkillInfo( &hi, &ti, false );

			if( pTarget->GetBaseStat()->GetHP() == 0 )
				cCliGame::SetDieInfo( GetProp_Attack()->GetFollowTarget() );
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
					info.s_nDigimonID		= GetFTID();
					info.s_nLevel			= GetBaseStat()->GetLevel();
					info.s_fGameOrgScale	= GetOrgGameScale();
					target.s_nValue[0] = FMDigimon::GetBaseAtt( &info ) * -1;

					if( pTarget->GetClientPlag() & CP_DMG_NORMAL )
					{
						// 기본 공격
						pTarget->GetBaseStat()->SetDeltaHP( -5 );
						if( pTarget->GetLeafRTTI() == RTTI_TUTORIAL_MONSTER )
						{
							CTutorialMonster* pTutoMon = dynamic_cast<CTutorialMonster*>(pTarget);
							if( !pTutoMon->IsMonsterState( CMonster::MONSTER_BATTLE ) )
								pTutoMon->SetBattle();
						}
					}
					else
					{
						target.s_nValue[ 0 ] = 0;
						pTarget->GetBaseStat()->SetDeltaHP( 0 );
					}

					cCliGame::SetAttackInfo( &hitter, &target, false );

					if( pTarget->GetBaseStat()->GetHP() == 0 )
						cCliGame::SetDieInfo( GetProp_Attack()->GetFollowTarget() );

					m_fClientAttackTime = 0.0f;
				}
			}
		}
		break;

		// 나 이동중이구나~ 대기
	case CsC_AttackProp::SUCCESS_MOVING:			
		break;

		// 릴라는데 장애물이 등장
	case CsC_AttackProp::FAILE_BLOCKING:
		cPrintMsg::PrintMsg( cPrintMsg::TARGET_EYE_OUT );
		SetDigimonUserState( DU_FOLLOW );
		break;

		// 위치가 틀리다 - 다시 움직이자
	case CsC_AttackProp::FAILE_POS:		
		DoAttackTargetPos();
		break;

		// 계속 패킷 보내기 미안해서... 잘못瑩嗤?홀드
	case CsC_AttackProp::FAILE_HOLDBACK:
		SetRotation_Ani( pRotationTarget );		
		break;

		// 머야 이상한값 - 한번더 확인하기 위해서 재호출 하자 ( 디버깅 할때를 위해 )
	case CsC_AttackProp::FAILE_ASSERT:
		assert_cs( false );
		GetProp_Attack()->GetFollowTarget();
		break;

	default:
		assert_cs( false );
	}

	// 테이머와의 거리 체크
	_UpdateTamerDist();
}