
#include "stdafx.h"
#include "TamerUser.h"

#define		PARENT_CLASS		CTamer
CsCRTTI_CPP( PARENT_CLASS, CTamerUser, RTTI_TAMER_USER )


CTamerUser::sUSER_STAT::sUSER_STAT(CsC_AvObject* pParent):sBASE_STAT( pParent ),
s_nFP(CsC_AvObject::INVALIDE_STAT),s_nHP(CsC_AvObject::INVALIDE_STAT),s_nDS(CsC_AvObject::INVALIDE_STAT),
s_nMaxHP(CsC_AvObject::INVALIDE_STAT),s_nMaxDS(CsC_AvObject::INVALIDE_STAT),s_dwExp(CsC_AvObject::INVALIDE_STAT)
,s_nAtt(0),s_nDef(0)
#ifdef CROSSWARS_SYSTEM
,s_nCR(0),s_nMaxCR(0)
#endif
#ifdef SDM_TAMER_XGUAGE_20180628
,s_usMaxCrystal(0),s_usCurrentCrystal(0),s_uiMaxGuage(0),s_uiCurrentGuage(0)
#endif
{
}

CTamerUser::sUSER_STAT::~sUSER_STAT()
{

}

CsC_AvObject::eSTAT	CTamerUser::sUSER_STAT::GetType() const
{ 
	return CsC_AvObject::USER_STAT; 
}

int CTamerUser::sUSER_STAT::GetMaxHP() const
{ 
	return s_nMaxHP; 
}

int	CTamerUser::sUSER_STAT::GetMaxDS() const
{ 
	return s_nMaxDS; 
}

int	CTamerUser::sUSER_STAT::GetHP() const
{ 
	return s_nHP; 
}

int	CTamerUser::sUSER_STAT::GetDS() const
{ 
	return s_nDS; 
}

int	CTamerUser::sUSER_STAT::GetFP() const
{ 
	return s_nFP; 
}

float CTamerUser::sUSER_STAT::GetHPRate() const
{ 
	return s_nHP/(float)s_nMaxHP; 
}

float CTamerUser::sUSER_STAT::GetDSRate() const
{ 
	return s_nDS/(float)s_nMaxDS; 
}

float CTamerUser::sUSER_STAT::GetFPRate() const
{ 
	return s_nFP/(float)s_nMaxHP; 
}

void CTamerUser::sUSER_STAT::SetMaxHP( int nMaxHP )
{ 
	s_nMaxHP = nMaxHP; 
}

void CTamerUser::sUSER_STAT::SetMaxDS( int nMaxDS )
{ 
	s_nMaxDS = nMaxDS; 
}

void CTamerUser::sUSER_STAT::SetDeltaMaxHP( int nDelta )
{ 
	s_nMaxHP = CsMax( 0, s_nMaxHP+nDelta ); 
}

void CTamerUser::sUSER_STAT::SetHP( int nHP )
{ 
	s_nHP = nHP; 
	SetBackupHP( nHP ); 
}

void CTamerUser::sUSER_STAT::SetDS( int nDS )
{ 
	s_nDS = nDS; 
}

void CTamerUser::sUSER_STAT::SetFP( int nFP )
{ 
	s_nFP = CsMin( GetMaxHP(), nFP ); 
}

void CTamerUser::sUSER_STAT::SetDeltaFP( int nDelta )
{ 
	s_nFP = s_nFP + nDelta; 
}

void CTamerUser::sUSER_STAT::SetDeltaHP( int nDelta )
{ 
	s_nHP = CsMin( GetMaxHP(), CsMax( 0, s_nHP+nDelta ) ); 
}

void CTamerUser::sUSER_STAT::SetDeltaBackupHP( int nDelta )
{ 
	s_nHPBackup = CsMin( GetMaxHP(), CsMax( 0, s_nHPBackup+nDelta ) ); 
}

void CTamerUser::sUSER_STAT::SetDeltaDS( int nDelta )
{ 
	s_nDS = CsMin( GetMaxDS(), CsMax( 0, s_nDS+nDelta ) ); 
}

void CTamerUser::sUSER_STAT::SetDeltaBackupDS( int nDelta )
{ 
	s_nDSBackup = CsMin( GetMaxDS(), CsMax( 0, s_nDSBackup+nDelta ) ); 
}

__int64	CTamerUser::sUSER_STAT::GetExp() const
{
	return s_dwExp; 
}

void	CTamerUser::sUSER_STAT::SetExp( __int64 dwExp )
{ 
	s_dwExp = dwExp; 
}

int CTamerUser::sUSER_STAT::GetAtt() const
{ 
	return s_nAtt; 
}

int CTamerUser::sUSER_STAT::GetDef() const
{ 
	return s_nDef; 
}
#ifdef CROSSWARS_SYSTEM
int CTamerUser::sUSER_STAT::GetCR() const
{ 
	return s_nCR; 
}

int CTamerUser::sUSER_STAT::GetMaxCR() const
{ 
	return s_nMaxCR; 
}

float CTamerUser::sUSER_STAT::GetCRRate() const
{ 
	return s_nCR/(float)s_nMaxCR; 
}
#endif

void CTamerUser::sUSER_STAT::SetAtt( int nValue )
{ 
	s_nAtt = nValue; 
}

void CTamerUser::sUSER_STAT::SetDef( int nValue )
{ 
	s_nDef = nValue; 
}

#ifdef CROSSWARS_SYSTEM
void CTamerUser::sUSER_STAT::SetMaxCR( int nValue )
{ 
	s_nMaxCR = nValue; 
}

void CTamerUser::sUSER_STAT::SetCR( int nValue )
{ 
	s_nCR = nValue; 
}
#endif

#ifdef SDM_TAMER_XGUAGE_20180628

void CTamerUser::sUSER_STAT::SetMaxCrystalXGuage(unsigned short const& maxC, unsigned int const& maxGuage )
{
	s_usMaxCrystal = maxC;
	s_uiMaxGuage = maxGuage;	
}

void CTamerUser::sUSER_STAT::SetMaxCrystal(unsigned short const& maxC )
{
	s_usMaxCrystal = maxC;
}

unsigned short CTamerUser::sUSER_STAT::GetMaxCrystal() const
{
	return s_usMaxCrystal;
}

void CTamerUser::sUSER_STAT::SetCurrentCrystal( unsigned short const& currentC)
{
	s_usCurrentCrystal = currentC;
}

unsigned short CTamerUser::sUSER_STAT::GetCurrentCrystal() const
{
	return s_usCurrentCrystal;
}

void CTamerUser::sUSER_STAT::SetCurrentXGuage(unsigned int const& currentGuage)
{
	s_uiCurrentGuage = currentGuage;
}

unsigned int CTamerUser::sUSER_STAT::GetCurrentXGuage() const
{
	return s_uiCurrentGuage;
}

void CTamerUser::sUSER_STAT::SetMaxXGuage(unsigned int const& maxGuage)
{
	s_uiMaxGuage = maxGuage;
}

unsigned int CTamerUser::sUSER_STAT::GetMaxXGuage() const
{
	return s_uiMaxGuage;
}

// bool CTamerUser::sUSER_STAT::IsXGToCrystal() const
// {
// 	if( s_uiMaxGuage == 0 || s_usMaxCrystal == 0 )
// 		return false;
// 
// 	if( s_uiCurrentGuage < s_uiMaxGuage )
// 		return false;
// 
// 	if( s_usCurrentCrystal == s_usMaxCrystal )
// 		return false;
// 
// 	return true;
// }
// 
// bool CTamerUser::sUSER_STAT::IsCrystalToXG() const
// {
// 	if( s_uiMaxGuage == 0 || s_usMaxCrystal == 0 )
// 		return false;
// 
// 	if( s_usCurrentCrystal == 0 )
// 		return false;
// 
// 	if( s_uiCurrentGuage == s_uiMaxGuage )
// 		return false;
// 
// 	return true;
// }
#endif
//////////////////////////////////////////////////////////////////////////

CTamerUser::CTamerUser():m_nSereverRelocateCompletedTime(0)
,m_dwLastNearPortalID(0)
{
	m_pArriveTarget = NULL;
	m_pQuickSlot = NULL;
	m_pRainSound = NULL;
	m_pBoundingVolume = NULL;
	m_pRideTarget = NULL;
	m_eRideEndState = RES_NONE;
#ifdef DEBUFF_ITEM
	m_pDebuffTarget =NULL;
#endif
#ifdef KEYBOARD_MOVE
	m_bFollowPortalMove = false;
#endif
}

void CTamerUser::Delete()
{
	PARENT_CLASS::Delete();

	m_pArriveTarget = NULL;
	m_pArriveTargetItem = NULL;
	m_pQuickSlot = NULL;
	m_pRideTarget = NULL;
	m_eRideEndState = RES_NONE;
#ifdef DEBUFF_ITEM
	m_pDebuffTarget =NULL;
#endif
}

void CTamerUser::PreDelete()
{
	assert_cs( m_pBaseStat != NULL );
	NiDelete (sUSER_STAT*)m_pBaseStat;
	m_pBoundingVolume = NULL;
	m_pBaseStat = NULL;

	PARENT_CLASS::PreDelete();

	m_UserServerMove.Delete();

	if( g_pGameIF )
	{
		g_pGameIF->GetBuffInfo( IF_BUFF_MY_T )->SetBuffData( NULL );
		g_pGameIF->GetBuffInfo( IF_BUFF_SYSTEM )->SetBuffData( NULL );
	}	
}

void CTamerUser::Init( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwServerOption )
{
	NiPoint3 vPos = NiPoint3( (float)sPos.m_nX, (float)sPos.m_nY, 0.0f );
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	Init( nIDX, nFileTableID, vPos, fRot, szName, pProp, dwServerOption );

	if( IsNetOff() )
	{
		cCommunityContent::sGUILD_INFO guildInfo;
		GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
		std::wstring guildName = guildInfo.s_szGuildName;
		m_Option.SetGuildName( (LPCTSTR)guildName.c_str() );
	}
}

void CTamerUser::Init( uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwServerOption )
{
	assert_cs( m_pArriveTarget == NULL );
	assert_cs( m_pBaseStat == NULL );
	assert_cs( pProp->IsOptionPlag( OP_STAT ) );
	m_pBaseStat = NiNew sUSER_STAT( this );

	PARENT_CLASS::Init( nIDX, nFileTableID, vPos, fRot, szName, pProp, dwServerOption );

	//본인 팀 -> InitGameData에서 받은 값으로 셋팅
	if( szName != NULL )
		PARENT_CLASS::GetCharOption()->SetMatchIcon( g_pGameIF->m_sMyInfo.s_eMyTeam );

	m_UserServerMove.Init( this );
	m_eTamerUserState = TU_FOLLOW_DIGIMON; //2016-11-17-nova TU_NONE->TU_FOLLOW_DIGIMON 마우스로 선택하지 않으면 상태가 변하지 않아서 키보드로만 움직였을경우 따라오지 않음 
	m_eFatigueTalkState = FTS_NONE;
	m_TimeSeqFatigueTalk.SetDeltaTime( 10000 );	

	_Init();

	m_SkillRevTimeSeq.SetDeltaTime( 5000 );
	m_SkillRevTimeSeq.OffEnable();

	// 버프창
	g_pGameIF->GetBuffInfo( IF_BUFF_MY_T )->SetBuffData( &m_BuffData );
	g_pGameIF->GetBuffInfo( IF_BUFF_SYSTEM )->SetBuffData( &m_BuffData );
	PARENT_CLASS::GetBuff()->_ReleaseAllEncyBuff();
}

void CTamerUser::_Init()
{
	if( net::game == NULL )
		_NetOffInit();

	m_bServerSendSkill = false;
	m_dwServerOption = 0;

	// 인터페이스에 추가
	g_pGameIF->GetTamerWindow()->SetTargetObject( this );
	g_pGameIF->GetQuickSlot( cQuickSlot::Q_TAMER )->SetTargetObject( this );
	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( 0 );
	pDataQuickSlot->SetChar( this );

	if( pDataQuickSlot->GetQuickSlot() != NULL )	// 퀵슬롯 복사
		memcpy( g_pDataMng->GetTamerCashQuickSlot(), pDataQuickSlot->GetData(0), sizeof(cResist::sCHAR_QUICKSLOT::sCHAR) );

	m_pQuickSlot = pDataQuickSlot->GetData( 0 );
#ifndef UI_INVENTORY_RENEWAL
	cData_QuickSlot::CheckAllSlot( this, m_pQuickSlot, m_SkillMng.GetSkillCount() );
#endif

	// 지역정보
	m_bFirstRegion = true;
	m_szMapDiscript.clear();
	m_cBGSound.clear();
	m_szMainMapDiscript.clear();	

#ifdef MEGAPHONE_BAN
	m_bMegaPhoneBan = false;
#endif

	m_nRegionIndex = CsGBTerrainRoot::CR_INVALIDE;
	m_nCurPosRegionIndex = CsGBTerrainRoot::CR_INVALIDE;
	m_dwLastNearPortalID = 0;
}

void CTamerUser::_NetOffInit()
{
	cProfile x( "dmo.ini" );
	GetBaseStat()->SetMoveSpeed( (float)x.GetInt( "CHAR", "Tspeed" ) );

#ifdef USE_CENTER_BAR
	g_pResist->LoadCenterBar( NULL );
#endif

	/*GetBaseStat()->SetMaxHP( 1000 );
	GetBaseStat()->SetHP( 800 );
	GetBaseStat()->SetFP( 400 );

	GetBaseStat()->SetMaxDS( 1000 );
	GetBaseStat()->SetDS( 800 );*/
}

CsC_AvObject::eCHECK_TYPE CTamerUser::CheckModel()
{
	eCHECK_TYPE ct = PARENT_CLASS::CheckModel();
	if( ct != CHECK_LOAD_COMP )
		return ct;

	assert_cs( m_pBoundingVolume == NULL );
	NiCollisionData* pkCD = NiGetCollisionData( m_Node.m_pNiNode );
	assert_cs( pkCD != NULL );
	m_pBoundingVolume = pkCD->GetModelSpaceABV();
	return ct;
}

void CTamerUser::Update( float fDeltaTime, bool bAnimation )
{
	bool bBackupAni = CsC_AniProp::g_bUpdateActor;
	CsC_AniProp::g_bUpdateActor = true;

	if( m_SkillRevTimeSeq.IsEnable() == true )
	{
		RecvSkill( false, 0 );
	}

	if( _IsApplyTUState() == true )
	{
#ifdef KEYBOARD_MOVE
		if( IsRide() && IsPause( PAUSE_PATH ) == false )
			SetPause( PAUSE_PATH, true );

		if( g_pMngCollector->GetSceneState() != CMngCollector::eSCENE_NONE 
			|| GetProp_Animation()->GetAnimationID() == ANI::DIGIVICE_CHAGE )
		{
			if( GetProp_Path()->IsMove() )
				GetProp_Path()->DeletePath();
		}

		if( GetProp_Path()->IsMove() == false && GetProp_Animation()->GetAnimationID() == ANI::MOVE_RUN )
			GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
#endif
		switch( m_eTamerUserState )
		{
		case TU_NONE:
		case TU_USE_TARGET_ITEM:
		case TU_TARGET_RIDE_END:
			break;

		case TU_SKILL:
			{
				assert_cs( GetProp_Attack()->GetFollowTarget() != NULL );
				_UpdateAttack( fDeltaTime );
			}
			break;
		case TU_WAIT_REV_SKILL:
			{
			}
			break;
#ifdef KEYBOARD_MOVE
		case TU_KEYBOARD_MOVE:
			{
				if( IsPause( PAUSE_PATH ) )
					SetPause( PAUSE_PATH, false );

				if( g_pServerMoveOwner->GetCurState() == g_pServerMoveOwner->GetBackupState() )
				{
					DWORD nKeyCheck = g_pServerMoveOwner->GetKeyCheck();
					if( nKeyCheck & CUserServerMove::KEY_LEFT ||
						nKeyCheck & CUserServerMove::KEY_RIGHT ||
						nKeyCheck & CUserServerMove::KEY_UP ||
						nKeyCheck & CUserServerMove::KEY_DOWN )
					{
						if(	GetProp_Animation()->GetAnimationID() != ANI::MOVE_RUN )
							GetProp_Animation()->SetAnimation( ANI::MOVE_RUN );
					}
					else
					{
						m_eTamerUserState = TU_NONE;
						GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
					}
				}
				else if( GetProp_Animation()->GetAnimationID() == ANI::MOVE_RUN )
				{
					GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
				}
			}
			break;
		case TU_FOLLOW_DIGIMON:
			{
				if( m_bFollowPortalMove )
					_MoveFollowDigimonPortal();
				else
					_MoveFollowDigimon();
			}
			break;
#endif
		default:
			assert_cs( false );
		}
	}	


	PARENT_CLASS::Update( fDeltaTime, bAnimation );	
	
	if( IsPause( PAUSE_PATH ) == false && !IsRide() )
	{
		m_UserServerMove.SetServerMove( GetUniqID(), GetPos().x, GetPos().y, GetCurRot(), GetProp_Path()->GetPath()->m_iPath );
	}
	m_UserServerMove.Update_SendMove_Server();

#ifdef DEBUFF_ITEM
	_UpdateDebuff();
#endif
	_UpdateRegion();
	_UpdatePortal(fDeltaTime);
	_UpdateWeather();
	_UpdateFatiguTalk();

	// 소유 디지몬 죽은거 체크
	assert_cs( g_pCharMng->GetDigimonUser( 0 ) != NULL );
	if( g_pCharMng->GetDigimonUser( 0 )->GetDigimonState() == CDigimon::DIGIMON_DIE )
	{
		SetAnimation( ANI::DAMAGE_DEAD );
		SetPauseAll( true );
	}

	CsC_AniProp::g_bUpdateActor = bBackupAni;

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_GAME || nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_DUNGEON )
		g_pHpBar->SetBuddy( cHpBar::HPDSFP, GetUniqID() );
}

#ifdef DEBUFF_ITEM
void CTamerUser::_UpdateDebuff()
{
	if( m_pDebuffTarget == NULL )
		return;

	//if( ( m_pDebuffTarget->GetPos2D() - GetPos2D() ).Length() < m_fDebuffDist )
	if( ( NiPoint2(m_vDebuffPos.x, m_vDebuffPos.y) - GetPos2D() ).Length() < m_fDebuffDist )
	{
		if( ( m_pDebuffTarget->GetPos2D() - GetPos2D() ).Length() > m_fDebuffDist )
		{
			m_pDebuffTarget = NULL;
			return;
		}

		if( net::game )
			net::game->SendItemUse( GetUniqID(), m_nDebuffInvenIndex );

		m_pDebuffTarget = NULL;
	}
	// 	else
	// 	{
	// 		SetTargetPos( m_pDebuffTarget->GetPos(), m_fDebuffDist, false, false );
	// 	}
}
#endif

void CTamerUser::_UpdateAttack( float fDeltaTime )
{
	// 전투중 인가
	CsC_AvObject* pTarget = GetProp_Attack()->GetFollowTarget();
	assert_cs( pTarget != NULL );

	CsC_AvObject* pRotationTarget = GetProp_Attack()->GetHitTarget();
	if( pRotationTarget == NULL )
		pRotationTarget = pTarget;	

	CsC_AttackProp::eFOLLOW_ITERATOR eFollowIterator;
	switch( GetProp_Attack()->UpdateAttack( fDeltaTime, eFollowIterator ) )
	{
		// 공격 시작
	case CsC_AttackProp::SUCCESS_ATTACK_START:
		SetRotation_Ani( pRotationTarget );
		if( net::game )
		{
			switch( eFollowIterator )
			{
			case CsC_AttackProp::FI_SKILL1:
				SendSkill( m_nActiveIDX, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL2:
				SendSkill( m_nActiveIDX, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL3:
				SendSkill( m_nActiveIDX, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL4:
				SendSkill( m_nActiveIDX, pTarget );
				break;
			case CsC_AttackProp::FI_SKILL5:
				SendSkill( m_nActiveIDX, pTarget );
				break;
			default:
				assert_cs( false );
			}
		}
#ifndef _GIVE
		else
		{
			switch( eFollowIterator )
			{
			case CsC_AttackProp::FI_SKILL1:
				{
					m_bServerSendSkill = true;

					sRECV_HITTER_INFO hi;
					hi.s_nUID = GetUniqID();
					hi.s_nSkillIndex = 0;
					hi.s_HitType = CsC_AttackProp::HT_Skill;

					sRECV_TARGET_INFO ti;
					ti.s_nUID = GetProp_Attack()->GetFollowTarget()->GetUniqID();
					ti.s_DamageType = CsC_AttackProp::DT_Normal;
					ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;
					ti.s_nValue[ 0 ] = -456;

					cCliGame::SetSkillInfo( &hi, &ti, false );
				}
				break;
			case CsC_AttackProp::FI_SKILL2:
				{
					m_bServerSendSkill = true;

					sRECV_HITTER_INFO hi;
					hi.s_nUID = GetUniqID();
					hi.s_nSkillIndex = 1;
					hi.s_HitType = CsC_AttackProp::HT_Skill;

					sRECV_TARGET_INFO ti;
					ti.s_nUID = GetProp_Attack()->GetFollowTarget()->GetUniqID();
					ti.s_DamageType = CsC_AttackProp::DT_Normal;
					ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;
					ti.s_nValue[ 0 ] = -456;

					cCliGame::SetSkillInfo( &hi, &ti, false );
				}
				break;
			case CsC_AttackProp::FI_SKILL3:
				{
					m_bServerSendSkill = true;
					
					sRECV_HITTER_INFO hi;
					hi.s_nUID = GetUniqID();
					hi.s_nSkillIndex = 2;
					hi.s_HitType = CsC_AttackProp::HT_Skill;

					sRECV_TARGET_INFO ti;
					ti.s_nUID = GetProp_Attack()->GetFollowTarget()->GetUniqID();
					ti.s_DamageType = CsC_AttackProp::DT_Normal;
					ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;
					ti.s_nValue[ 0 ] = -456;

					cCliGame::SetSkillInfo( &hi, &ti, false );
				}
				break;
			case CsC_AttackProp::FI_SKILL4:
				{
					m_bServerSendSkill = true;

					sRECV_HITTER_INFO hi;
					hi.s_nUID = GetUniqID();
					hi.s_nSkillIndex = 3;
					hi.s_HitType = CsC_AttackProp::HT_Skill;

					sRECV_TARGET_INFO ti;
					ti.s_nUID = GetProp_Attack()->GetFollowTarget()->GetUniqID();
					ti.s_DamageType = CsC_AttackProp::DT_Normal;
					ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;
					ti.s_nValue[ 0 ] = -456;

					cCliGame::SetSkillInfo( &hi, &ti, false );
				}
				break;
			case CsC_AttackProp::FI_SKILL5:
				{
					m_bServerSendSkill = true;

					sRECV_HITTER_INFO hi;
					hi.s_nUID = GetUniqID();
					hi.s_nSkillIndex = 4;
					hi.s_HitType = CsC_AttackProp::HT_Skill;

					sRECV_TARGET_INFO ti;
					ti.s_nUID = GetProp_Attack()->GetFollowTarget()->GetUniqID();
					ti.s_DamageType = CsC_AttackProp::DT_Normal;
					ti.s_nNumEffectType = NUMTYPE::ET_NORMAL;
					ti.s_nValue[ 0 ] = -456;

					cCliGame::SetSkillInfo( &hi, &ti, false );
				}
				break;
			}
		}
#endif
		break;

		// 나 이동중이구나~ 대기
	case CsC_AttackProp::SUCCESS_MOVING:			
		break;

		// 떄릴라는데 장애물이 등장
	case CsC_AttackProp::FAILE_BLOCKING:
		cPrintMsg::PrintMsg( cPrintMsg::TARGET_EYE_OUT );
		break;

		// 위치가 틀리다 - 다시 움직이자
	case CsC_AttackProp::FAILE_POS:		
		DoAttackSkillPos();
		break;

		// 계속 패킷 보내기 미안해서... 잘못됬지만 홀드
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
}

void CTamerUser::_UpdateRegion()
{
	std::wstring szMapDiscript;
	std::string cBGSound;
	
	m_nRegionIndex = nsCsGBTerrain::g_pCurRoot->CheckRegion( m_vPos, szMapDiscript, cBGSound );
	if( m_nRegionIndex == CsGBTerrainRoot::CR_INVALIDE )
		return;

	if( ( m_szMapDiscript == szMapDiscript )&&( m_cBGSound == cBGSound ) )
		return;

	m_szMapDiscript = szMapDiscript;
	m_cBGSound = cBGSound;

	DWORD dwCurrentMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;

	m_nCurPosRegionIndex = m_nRegionIndex;
	g_pGameIF->GetMiniMap()->SetRegion( m_szMapDiscript.c_str() );	
	g_pDataMng->GetMapRegion()->InRegion( m_nRegionIndex );
	g_pDataMng->GetQuest()->CheckRegion( dwCurrentMapID, m_nRegionIndex );

	if( m_bFirstRegion == false )
	{
		if( m_szMainMapDiscript != szMapDiscript )
		{			
			g_pRegionInfo->SetRegionInfo( m_szMapDiscript.c_str(), cRegionInfo::SUB_RESION );
		}

		if( g_pGameIF->GetBossScene()->IsBoss() == false )
			g_pSoundMgr->PlayMusic( m_cBGSound.c_str() );
		else
			g_pSoundMgr->PlayMusic( "Real_Factory_special_battle.mp3" );
	}
	else
	{
		CsMapList* pFTMap = nsCsMapTable::g_pMapListMng->GetList( dwCurrentMapID );
		m_szMainMapDiscript = pFTMap->GetInfo()->s_szMapDiscript;

		if( g_pGameIF->GetBossScene()->IsBoss() == false )
			g_pSoundMgr->PlayMusic( m_cBGSound.c_str() );
		else
			g_pSoundMgr->PlayMusic( "Real_Factory_special_battle.mp3" );

		g_pRegionInfo->SetRegionInfo( m_szMainMapDiscript.c_str(), cRegionInfo::MAIN_REGION );
		m_bFirstRegion = false;
	}

	if( g_pEventScene->GetCondMap()->size() > 0 )
	{
		std::map< DWORD, DWORD >::iterator it = g_pEventScene->GetCondMap()->begin();
		std::map< DWORD, DWORD >::iterator itEnd = g_pEventScene->GetCondMap()->end();
		for( ; it != itEnd;  )
		{
			int nResion = dwCurrentMapID * 100 + m_nRegionIndex;
			if( it->first == nResion )
			{
				int nSceneNum = it->second;
				it = g_pEventScene->GetCondMap()->erase( it );
				g_pEventScene->EventStart( nSceneNum );
			}
			else
				++it;
		}
	}
}


void CTamerUser::_UpdatePortal(const float& fDeltaTime)
{
	if(isEnablePortal() == false)
		return;

	DWORD dwNearPortalID = GetNearPortalID(fDeltaTime);
	if( m_dwLastNearPortalID != 0 ){
		if( dwNearPortalID == 0 ){
			m_dwLastNearPortalID = 0;
			return;	
		}
	}
	if(m_dwLastNearPortalID == 0)
	{
		if(m_dwLastNearPortalID !=dwNearPortalID)
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PORTAL_MOVE, &dwNearPortalID );
			m_dwLastNearPortalID = dwNearPortalID;
		}
	}
}
bool CTamerUser::isEnablePortal(void)
{
	if( IsRide() == false )
	{
		if( GetProp_Path()->IsMove() == false )
			return false;
	}
	else
	{		
#ifdef KEYBOARD_MOVE
		if( g_pResist->m_Global.s_bMoveDigimon == false )
#endif
			if( g_pCharMng->GetDigimonUser( 0 )->GetProp_Path()->IsMove() == false )
				return false;
	}

	if( g_pDataMng->IsSort() == true )
	{
		return false;
	}

	// 메시지박스가 띄워진 상태면 리턴.
	if(1 <= cMessageBox::GetCount())
		return false;
	
	if( IsLoad() == false )
		return false;	

	return true;
}

const DWORD CTamerUser::GetNearPortalID(const float& fDeltaTime) const
{
	SAFE_POINTER_RETVAL( m_pBoundingVolume, 0);

	NiBoxBV bound;
	bound.UpdateWorldData( *m_pBoundingVolume, m_Node.m_pNiNode->GetWorldTransform() );	
	return nsCsGBTerrain::g_pCurRoot->CheckPortal( fDeltaTime, m_vPos, &bound );
}
void CTamerUser::_UpdateWeather()
{
#ifndef _GIVE
	if( net::game == NULL )
		return;
#endif

	if( nsCsMapTable::g_pWeatherMng->IsEnableCheck() == false )
		return;

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	if( nsCsMapTable::g_pWeatherMng->IsGroup( dwMapID ) == false )
	{
		goto OFF_WEATHER;
	}
	
	CsWeather::sELEMENT* pWeatherElement = nsCsMapTable::g_pWeatherMng->GetGroup( dwMapID )->UpdateWeather( net::game->GetTimeTS(), (int)m_vPos.x, (int)m_vPos.y );
	if( pWeatherElement == NULL )
	{		
		goto OFF_WEATHER;
	}

	switch( pWeatherElement->s_Type )
	{
	case CsWeather::WT_NONE:
		goto OFF_WEATHER;
	case CsWeather::WT_SNOW:
		g_pWeather->SetType( CsGBWeather::eType_Snow );
		g_pWeather->SetLevel( pWeatherElement->s_nLevel );
		break;
	case CsWeather::WT_RAIN:
		if( g_pSoundMgr->IsSound( m_pRainSound  ) == false )
		{
			m_pRainSound = g_pSoundMgr->PlaySystemSound( "game\\rain.mp3" );			
		}
		g_pWeather->SetType( CsGBWeather::eType_Rain );
		g_pWeather->SetLevel( pWeatherElement->s_nLevel );
		break;
	}

	return;

OFF_WEATHER:
	{
		// 날씨 관련 다 끄자
		g_pWeather->SetLevel( 0 );

		if( g_pSoundMgr->IsSound( m_pRainSound  ) )
		{
			assert_cs( m_pRainSound );
			g_pSoundMgr->StopSound( m_pRainSound );	
		}
		m_pRainSound = NULL;
	}	
}

void CTamerUser::_UpdateFatiguTalk()
{
	int nCurFP = GetBaseStat()->GetFP();

	// 피로하지 않을때
	if( m_eFatigueTalkState != FTS_FATIGUE )
	{
		if( GetBaseStat()->IsFatigue() )
		{
			m_eFatigueTalkState = FTS_FATIGUE;
			cDigimonTalk::Print( 50018 );
			m_TimeSeqFatigueTalk.OffReset();
		}
		if( nCurFP == GetOldFatiguePoint() )
			return;
	}

	switch( m_eFatigueTalkState )
	{
	case FTS_NONE:
		if( nCurFP < GetOldFatiguePoint() )
		{
			m_eFatigueTalkState = FTS_DECREASE;
			cDigimonTalk::Print( 50019 );
		}
		else
		{
			if( GetBaseStat()->GetFPRate() < 0.4f )
			{
				m_eFatigueTalkState = FTS_INCREASE_0;
				cDigimonTalk::Print( 50015 );				
			}
			else if( GetBaseStat()->GetFPRate() < 0.8f )
			{
				m_eFatigueTalkState = FTS_INCREASE_40;
				cDigimonTalk::Print( 50016 );				
			}
			else
			{
				m_eFatigueTalkState = FTS_INCREASE_80;
				cDigimonTalk::Print( 50017 );
			}
		}
		break;
	case FTS_DECREASE:
		if( nCurFP > GetOldFatiguePoint() )
		{
			if( GetBaseStat()->GetFPRate() < 0.4f )
			{
				m_eFatigueTalkState = FTS_INCREASE_0;
				cDigimonTalk::Print( 50015 );				
			}
			else if( GetBaseStat()->GetFPRate() < 0.8f )
			{
				m_eFatigueTalkState = FTS_INCREASE_40;
				cDigimonTalk::Print( 50016 );				
			}
			else
			{
				m_eFatigueTalkState = FTS_INCREASE_80;
				cDigimonTalk::Print( 50017 );
			}
		}
		break;
	case FTS_INCREASE_0:
		if( nCurFP < GetOldFatiguePoint() )
		{
			m_eFatigueTalkState = FTS_DECREASE;
			cDigimonTalk::Print( 50019 );
		}
		else
		{
			if( GetBaseStat()->GetFPRate() > 0.4f )
			{
				m_eFatigueTalkState = FTS_INCREASE_40;
				cDigimonTalk::Print( 50016 );
			}			
		}
		break;
	case FTS_INCREASE_40:
		if( nCurFP < GetOldFatiguePoint() )
		{
			m_eFatigueTalkState = FTS_DECREASE;
			cDigimonTalk::Print( 50019 );
		}
		else
		{
			if( GetBaseStat()->GetFPRate() > 0.8f )
			{
				m_eFatigueTalkState = FTS_INCREASE_80;
				cDigimonTalk::Print( 50017 );
			}
		}
		break;
	case FTS_INCREASE_80:
		if( nCurFP < GetOldFatiguePoint() )
		{
			m_eFatigueTalkState = FTS_DECREASE;
			cDigimonTalk::Print( 50019 );
		}
		break;
	case FTS_FATIGUE:
		if( GetBaseStat()->IsFatigue() )
		{
			if( m_TimeSeqFatigueTalk.IsEnable() == true )
			{			
				cDigimonTalk::Print( 50018 );
			}
		}
		else
		{
			m_eFatigueTalkState = FTS_DECREASE;
			cDigimonTalk::Print( 50019 );
		}
		break;
	default:
		assert_cs( false );
	}

	m_nOldFatiguePoint = nCurFP;
}

void CTamerUser::Render()
{
	PARENT_CLASS::Render();	
}

void CTamerUser::ThreadLoad_Part( sCHANGE_PART_INFO* pItemCodeArray, bool bIncludeBaseKFM /*=true*/ )
{	
	PARENT_CLASS::ThreadLoad_Part( pItemCodeArray, bIncludeBaseKFM );	

	//g_pDataMng->GetTEquip()->CheckBuff();
}

void CTamerUser::ChangePart( sCHANGE_PART_INFO* pPartInfo )
{
	// 코스츔 보이지 않기 상태에서 코스츔 입을때 메세지 표현
	if( IsSyncOption( nTamer::Option::CostumeOff ) == true )
	{
		if( ( pPartInfo->s_nPartIndex == nsPART::Costume )&&( pPartInfo->s_nFileTableID ) )
		{
			cPrintMsg::PrintMsg( 20070 );
		}
	}
	PARENT_CLASS::ChangePart( pPartInfo );

	// 이벤트
	// 눈사람
	if( pPartInfo->s_nFileTableID == 17004 )
	{
		GetProp_Effect()->AddEffect( "System\\snowman.nif", 1.0f, nsEFFECT::POS_CHARPOS );
		PlaySound( "digimon\\skill\\casting_11.wav" );
	}

	CheckBuff();
}

//=============================================================================================================
//
//	상태
//
//=============================================================================================================

void CTamerUser::ItemPickUpAnimation()
{
	SetPause( PAUSE_ANIMATION, false );
	PARENT_CLASS::ItemPickUpAnimation();
	SetPauseAll( true );

	m_pArriveTargetItem = NULL;
}

void CTamerUser::PickUpItemGotoInven(cItemData* pItemData)
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	pInven->ItemCrop( pItemData );
	pInven->OutputCropMsg(pItemData);
}

void CTamerUser::RaidItemCropSuccess( cItemData* pItemData, int nTalkID )
{	
	if( ( pItemData != NULL ) )
	{
		assert_csm( pItemData->GetType() != 0, _T( "CTamerUser::RaidItemCropSuccess" ) );
		assert_csm( nsCsFileTable::g_pItemMng->IsItem( pItemData->GetType() ), _T( "CTamerUser::RaidItemCropSuccess" ) );
		int nRemain = g_pDataMng->GetInven()->ItemCrop( pItemData );
		CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemData->GetType() )->GetInfo();		
		cPrintMsg::PrintMsg( nTalkID, pFTItem->s_szName, &g_pDataMng->GetItemColor(pFTItem)); // [4/21/2016 hyun] 컬러값 추가
		if( pFTItem->IsNoticeItem() ) 
		{
			cDigimonTalk::Print( 50008 ); // 고급아이템을 흭득하셨군요.
		}
	}	

	m_pArriveTargetItem = NULL;
}

void CTamerUser::ItemCropSound( int nType )
{
	switch( nType )
	{
	case 9514:			PlaySound( SOUND_MONEY_PICKUP );			break;
	default:			PlaySound( SOUND_DIGITA_PICKUP );
	}
}

void CTamerUser::ItemCropFailed()
{
	SetPauseAll( false );
	m_pArriveTargetItem = NULL;
}

void CTamerUser::MoneyCrop( int nMoney, int TamerUID, int nGuildAddMoney, int nBuffAddMoney )
{
	if( GetUniqID() == TamerUID )
	{
		SetPause( PAUSE_ANIMATION, false );
		PlaySound( SOUND_MONEY_PICKUP );
		SetAnimation( ANI::IDLE_ITEMCROP, 2.0f );
		SetPauseAll( true );

		m_pArriveTargetItem = NULL;
	}

	// 버프로 인해 증가된 값
	u8 nMoneyBuffMoney = nMoney + nBuffAddMoney;
	// 길드 버프로 인해 증가된 최종값
	u8 nTotalGetMoney =  nMoneyBuffMoney + nGuildAddMoney;

	g_pDataMng->GetInven()->IncreaseMoney( nTotalGetMoney, true );

	if( nGuildAddMoney > 0 )// 길드 버프로 인해 증가된 값이 있을 경우에만
		cPrintMsg::PrintMsg( 30222, &sMONEY(nTotalGetMoney), &sMONEY(nMoneyBuffMoney), &sMONEY(nGuildAddMoney)  );	// “길드스킬로 인해 [Money]를 추가로 획득하셨습니다.”
	else// 버프로 증가된 값일 경우에도 일반 획득 메시지 출력해줌.
		cPrintMsg::PrintMsg( cPrintMsg::CROP_MONEY, &sMONEY(nTotalGetMoney) );	// “[Money]를 획득하셨습니다.”

// #ifdef GUILD_RENEWAL
// 	u8 nTotalGetMoney = nMoney;
// 	int nVal = g_pDataMng->GetGuild()->GetUsingSkillValue( GUILD_CROPBITUP );	
// 	if( nVal > 0 )
// 	{
// 		u8 nAddMoney = (u8)( ( nTotalGetMoney * nVal ) / 10000 );
// 		nTotalGetMoney += nAddMoney;
// 		cPrintMsg::PrintMsg( 30222, &sMONEY(nTotalGetMoney), &sMONEY(nMoney), &sMONEY(nAddMoney)  );	// “길드스킬로 인해 [Money]를 추가로 획득하셨습니다.”
// 	}
// 	else
// 		cPrintMsg::PrintMsg( cPrintMsg::CROP_MONEY, &sMONEY(nTotalGetMoney) );	// “[Money]를 획득하셨습니다.”
// 
// 	// 실제 얻은 돈
// 	g_pDataMng->GetInven()->IncreaseMoney( nTotalGetMoney, true );
// #else
// 	g_pDataMng->GetInven()->IncreaseMoney( nMoney, true );
// 
// 	sMONEY money( (u8)nMoney );
// 	cPrintMsg::PrintMsg( cPrintMsg::CROP_MONEY, &money );	
// #endif
}

void CTamerUser::SetBattle()
{
	PARENT_CLASS::SetBattle();
}

void CTamerUser::SetBattleEnd()
{
	PARENT_CLASS::SetBattleEnd();
}

void CTamerUser::SetTamerUserState( eTAMERUSER_STATE tuState, void* pValue /*=NULL*/ )
{
	if( _IsApplyTUState() == true )
	{
		switch( tuState )
		{
		case TU_NONE:
			break;
		case TU_SKILL:
			// 모든 처리는 되서 들어온다.
			DoAttackSkillPos();
			break;
		case TU_WAIT_REV_SKILL:
			m_SkillRevTimeSeq.Reset();
			break;
		case TU_USE_TARGET_ITEM:
			SetArriveTarget( (CsC_AvObject*)pValue );
			SetTargetPos( ( (CsC_AvObject*)pValue )->GetPos(), IF_TARGET_TAMER_MAX_DIST, false, false );
			break;
		case TU_TARGET_RIDE_END:
			SetArriveTarget( (CsC_AvObject*)pValue );
			break;
#ifdef KEYBOARD_MOVE
		case TU_KEYBOARD_MOVE:
		case TU_FOLLOW_DIGIMON:
			break;
#endif

		default:
			assert_cs( false );
		}
	}
	else
	{
		switch( tuState )
		{
		case TU_NONE:
			break;

		case TU_SKILL:
			assert_csm1( ( (CsC_AvObject*)pValue )->GetLeafRTTI() == RTTI_MONSTER, L"SetTamerUserState : 몬스터가 아님 : RTTI = %d", ( (CsC_AvObject*)pValue )->GetLeafRTTI() );
			GetProp_Attack()->SetFollowTarget( (CsC_AvObject*)pValue, GetAttackRange(), CsC_AttackProp::FI_ATTACK, false );
			break;

		case TU_WAIT_REV_SKILL:
			m_SkillRevTimeSeq.Reset();
			break;

		case TU_USE_TARGET_ITEM:
		case TU_TARGET_RIDE_END:
			SetTamerUserState( TU_NONE );
			break;
#ifdef KEYBOARD_MOVE
		case TU_KEYBOARD_MOVE:
		case TU_FOLLOW_DIGIMON:
			break;
#endif
		default:
			assert_cs( false );
		}
	}

	m_eTamerUserState = tuState;
}

void CTamerUser::SetStateValue( eTAMERUSER_STATE tuState, int nValue )
{
	switch( tuState )
	{
	case TU_USE_TARGET_ITEM:
		m_nStatValue_ItemFTID = nValue;
		break;
	default:
		assert_cs( false );
	}
}

bool CTamerUser::_IsApplyTUState()
{
	switch( m_eTamerState )
	{
	case TAMER_DIE:
		return false;
	}
	return ( IsServerDie() == false );
}


//=============================================================================================================
//
//	씬
//
//=============================================================================================================
void CTamerUser::Scene_Evol( CsC_AvObject* pDigimon, int nDigimonFTID )
{
	PARENT_CLASS::Scene_Evol( pDigimon, nDigimonFTID );
	ReleaseArriveTarget();
	DeletePath();

	SetTamerUserState( TU_NONE );

	char* cDir = GetSoundDir();
	if( cDir != NULL )
	{
		char cPath[ MAX_PATH ];
		strcpy_s( cPath, MAX_PATH, cDir );

		int nEvo = nsCsDigimonTable::ET_ROOKIE;
		if( nsCsFileTable::g_pDigimonMng->IsDigimon( nDigimonFTID ) )
		{
			nEvo = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonFTID )->GetInfo()->s_eEvolutionType;
		}

		switch( nEvo )
		{
		case nsCsDigimonTable::ET_ROOKIE:
		case nsCsDigimonTable::ET_CHAMPION:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge01.wav" );
			break;
		case nsCsDigimonTable::ET_ULTIMATE:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge02.wav" );
			break;
		case nsCsDigimonTable::ET_MEGA:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge03.wav" );
			break;
		case nsCsDigimonTable::ET_BUSTERMODE:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge04.wav" );
			break;
		case nsCsDigimonTable::ET_JOINTPROGRESSMODE:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge04.wav" );
			break;
		// 진화 시 테이머 Sound
		case nsCsDigimonTable::ET_CAPSULE:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge02.wav" );
			break;
		case nsCsDigimonTable::ET_SPIRIT:
#ifdef SDM_TAMER_XGUAGE_20180628
		case nsCsDigimonTable::ET_MIX:
#endif
			break;
#ifdef SDM_TAMER_XGUAGE_20180628
		case nsCsDigimonTable::ET_X_ROOKIE:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge01.wav" );
			break;
		case nsCsDigimonTable::ET_X_CHAMPION:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge01.wav" );
			break;
		case nsCsDigimonTable::ET_X_ULTIMATE:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge02.wav" );
			break;
		case nsCsDigimonTable::ET_X_MEGA:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge03.wav" );
			break;
		case nsCsDigimonTable::ET_X_BUSTERMODE:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge04.wav" );
			break;
		case nsCsDigimonTable::ET_X_JOGRESS:
			strcat_s( cPath, MAX_PATH, "\\DS_Charge04.wav" );
			break;
#endif
		default:
			assert_cs( false );
			return;
		}

		PlayStaticSound( cPath );
	}	
}

void CTamerUser::SceneLevelUp( int nLevel )
{
	PARENT_CLASS::SceneLevelUp( nLevel );
	
	cPrintMsg::PrintMsg( 19001, &nLevel );
	cDigimonTalk::Print( 19001 );

	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_GUILD_PERSONLEVEL, &nLevel );

	//==========================================================================================================
	// 퀘스트 진행 체크
	//==========================================================================================================
	g_pDataMng->GetQuest()->CalProcess(CsQuestCompRequire::T_LEVEL);

	//==========================================================================================================
	// 퀘스트 받을 수 있는지 체크
	//==========================================================================================================
	g_pDataMng->GetQuest()->RevQuestCheck();

	// 해당 퀘스트 이름 색 변경
	g_pGameIF->GetQuestHelper()->ResetHelperString();
	if( g_pGameIF->GetQuestList() )
	{
		g_pGameIF->GetQuestList()->ResetQuestList( true );	
	}

	//==========================================================================================================
	// 업적 체크
	//==========================================================================================================
	GS2C_RECV_CHECKTYPE recv;
	recv.nType = AchieveContents::CA_TLevel;
	recv.nValue1 = nLevel;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
}

void CTamerUser::Scene_Barcode_Connect()
{
	if( IsPause( PAUSE_ANIMATION ) == false )
	{
		if( g_pNpcMng->GetZoomNpcType() == cBaseWindow::WT_NONE )
		{
			g_pNpcMng->SetZoomNpc( this );
			g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_BARCODE_CARD_SCRATCH );
		}
	}

	PARENT_CLASS::Scene_Barcode_Connect();
	SetTamerUserState( TU_NONE );	
}

void CTamerUser::Scene_Barcode_CardScratch_Success()
{
	if( IsPause( PAUSE_ANIMATION ) == false )
	{
		if( g_pNpcMng->GetZoomNpcType() == cBaseWindow::WT_NONE )
		{
			g_pNpcMng->SetZoomNpc( this );
			g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_BARCODE_CARD_SCRATCH );
		}
	}

	PARENT_CLASS::Scene_Barcode_CardScratch_Success();

	SetTamerUserState( TU_NONE );
}

void CTamerUser::Scene_Barcode_CardScratch_Faile()
{
	if( IsPause( PAUSE_ANIMATION ) == false )
	{
		if( g_pNpcMng->GetZoomNpcType() == cBaseWindow::WT_NONE )
		{
			g_pNpcMng->SetZoomNpc( this );
			g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_BARCODE_CARD_SCRATCH );
		}
	}

	PARENT_CLASS::Scene_Barcode_CardScratch_Faile();

	SetTamerUserState( TU_NONE );
}

void CTamerUser::Scene_Barcode_CardScratch_OverCount()
{
	if( IsPause( PAUSE_ANIMATION ) == false )
	{
		if( g_pNpcMng->GetZoomNpcType() == cBaseWindow::WT_NONE )
		{
			g_pNpcMng->SetZoomNpc( this );
			g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_BARCODE_CARD_SCRATCH );
		}
	}

	PARENT_CLASS::Scene_Barcode_CardScratch_OverCount();

	SetTamerUserState( TU_NONE );
}

//====================================================================================
//
//		CallBack
//
//====================================================================================

void CTamerUser::CallBack_StartSequence( DWORD dwAniID )
{
	switch( dwAniID )
	{
	case ANI::IDLE_CHAR:
	case ANI::IDLE_SHAKE:
		PlayAniSound( dwAniID );
		return;
	}

	PARENT_CLASS::CallBack_StartSequence( dwAniID );
}


bool CTamerUser::CallBack_EndofSequence( DWORD dwAniID )
{
	switch( dwAniID )
	{
		// 스킬쓰고나서
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:			
		SetTamerUserState( TU_NONE );
		break;
#ifdef KEYBOARD_MOVE
	case ANI::DIGIVICE_CHAGE:
		SetTamerUserState( TU_FOLLOW_DIGIMON );
		break;
#endif
	}

	return PARENT_CLASS::CallBack_EndofSequence( dwAniID );
}

//=============================================================================================================
//
//	Path
//
//=============================================================================================================

void CTamerUser::DeletePath()
{
	PARENT_CLASS::DeletePath();

	m_UserServerMove.DeletePath();		
}

void CTamerUser::ArriveTarget()
{
	// 도착점에 도달
	if( m_pArriveTarget == NULL )
		return;

	// 특정 행동중이면 패스
	if( g_pGameIF->IsSpeacialAct() )
		return;

	// 왜 움직였는지 미리 체크
	switch( m_eTamerUserState )
	{
	case TU_USE_TARGET_ITEM:
		{			
			SetTamerUserState( TU_NONE );
			int nInvenIndex = g_pDataMng->GetInven()->GetFirstSlot_Item_ID( m_nStatValue_ItemFTID );
			if( nInvenIndex != cData_Inven::INVALIDE_INVEN_INDEX )
			{
				g_pDataMng->SendItemUse( TO_INVEN_SID( nInvenIndex ) );
			}
			DeletePath();
			ReleaseArriveTarget();
		}
		return;
	case TU_TARGET_RIDE_END:
		m_pRideTarget = m_pArriveTarget;		
		g_pGameIF->GetQuickEvol()->SetRide( false );
		ReleaseArriveTarget();
		return;
#ifdef KEYBOARD_MOVE
	case TU_KEYBOARD_MOVE:
		{

		}
		break;
#endif
	}	

	switch( m_pArriveTarget->GetClass() )
	{
	case nClass::Item:
		{
			if( g_pDataMng->IsSort() == true )
			{
				cPrintMsg::PrintMsg( 30490 );
				return;
			}

			m_pArriveTargetItem = m_pArriveTarget;

			_SetDefault_IdleAnimation_Stand();
			SetPauseAll( true );
			if( net::game )
				net::game->SendItemCrop( m_pArriveTarget->GetUniqID() );				
			else
				ItemCropFailed();
		}		
		break;
	case nClass::Npc:
		{
			// 테이머와의 거리 체크
			if( ( m_pArriveTarget->GetPos2D() - GetPos2D() ).Length() < IF_TARGET_TAMER_MAX_DIST )
			{
				if( ( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TALK ) == false )&&
					( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_REV ) == false )&&
					( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_COMP ) == false ) &&
					( !g_pDataMng->GetServerSync()->IsChageDigimon()))// 디지몬 변경 중에 NPC 대화 창 못열게 추가함
				{						
					cTalk* pTalk = (cTalk*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_TALK );					
					if( pTalk )
					{
						// 토크 중복 생성 되지 않게
						if( pTalk->GetTarget() != m_pArriveTarget )
							pTalk->SetTarget( m_pArriveTarget );
					}					
				}
			}
		}
		break;
	case nClass::Tamer:
		{
			// 테이머와의 거리 체크
			if( ( m_pArriveTarget->GetPos2D() - GetPos2D() ).Length() < IF_TARGET_TAMER_MAX_DIST )
			{
				// 상대방 테이머가 샾 오픈중이라면
				CTamer* pTamer = (CTamer*)m_pArriveTarget;
				if( pTamer->GetCondition()->IsCondition( nSync::ShopReady ) == true )
				{					
					cPrintMsg::PrintMsg( 30361 );
				}
				else if( pTamer->GetCondition()->IsCondition( nSync::Shop ) == true )
				{
					// 내가 전투중이라면 패스
					if( IsBattle() == true )
						cPrintMsg::PrintMsg( 16002 );
					else
						GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_PERSONSTORE,  &(*m_pArriveTarget) );

// 					else if( g_pDataMng->GetBattle()->IsJoinMatch() )
// 					{
// 						cPrintMsg::PrintMsg( 30455 );
// 					}
				}
			}
		}
		break;
	case nClass::CommissionShop:
		{
			// 테이머와의 거리 체크
			if( ( m_pArriveTarget->GetPos2D() - GetPos2D() ).Length() < IF_TARGET_TAMER_MAX_DIST )
			{
				if( IsBattle()  )
					cPrintMsg::PrintMsg( 16002 );
				else
					GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_COMMISSIONSTORE, &(*m_pArriveTarget) );

// 				else if( g_pDataMng->GetBattle()->IsJoinMatch() )
// 				{
// 					cPrintMsg::PrintMsg( 30455 );
// 				}
			}
		}
		break;
	}

	ReleaseArriveTarget();
}


//=============================================================================================================
//
//	Rotation
//
//=============================================================================================================
void CTamerUser::SetRotation_AniDir( NiPoint3 vDestDir )
{
	PARENT_CLASS::SetRotation_AniDir( vDestDir );
	
	// 카메라 자동 회전
	if( g_pResist->m_Character.s_bStaticCamera == TRUE )
	{
		float fDeltaRot = m_fDestRot + NI_PI - CAMERA_ST.GetCurRoll();
		CsInFi( fDeltaRot );
		float fPositiveDeltaRot = abs( fDeltaRot );
		assert_cs( fPositiveDeltaRot >= 0.0f );
		float fOrgTotalMoveDist = GetProp_Path()->GetOrgTotalMoveDist();
		// 적정 각에서만 반응
		if( ( fPositiveDeltaRot > CsD2R( 20 ) )||( fOrgTotalMoveDist > 2000.0f ) )
		{
			// 적정 거리에서만 반응
			if( fOrgTotalMoveDist > 500.0f + ( NI_PI - fPositiveDeltaRot )*800/NI_PI )
			{
				float fRotSpeed = g_pResist->m_Character.s_fStaticCameraRotSpeed;
				CAMERA_ST.SetAniRotation_Roll( 0.0f, m_fDestRot + NI_PI, fRotSpeed + fPositiveDeltaRot/2.0f );
			}
		}
	}
}
//=============================================================================================================
//
//	스킬
//
//=============================================================================================================
void CTamerUser::UseSkill( int nSkillIndex )
{
	if( m_bServerSendSkill == true )
	{
		cPrintMsg::PrintMsg( 30015 );
		return ;
	}

	m_SkillMng.__UseSkill( nSkillIndex );
}

bool CTamerUser::SendSkill( int nSkillIndex, CsC_AvObject* pTarget )
{
	assert_csm( m_eTamerUserState != TU_WAIT_REV_SKILL, _T( "송명수를 바로 찾아주세요~!! 이게 나온 당신은 이미 영웅~!!\n끄지 말고 꼭 송명수를 찾아주세요" ) );
	assert_csm( m_bServerSendSkill == false, _T( "송명수를 바로 찾아주세요~!! 이게 나온 당신은 이미 영웅~!!\n끄지 말고 꼭 송명수를 찾아주세요" ) );

	m_bServerSendSkill = true;
	
	net::game->SendActiveSkill( nSkillIndex, pTarget->GetUniqID() );
	
	SetTamerUserState( TU_WAIT_REV_SKILL );

	ReleaseArriveTarget();
	DeletePath();

	SetPause( PAUSE_PATH, true );	

	return true;
}

bool CTamerUser::RecvSkill( bool bSuccess, int nSkillIndex )
{
	if( m_bServerSendSkill == false )
	{
		return false;
	}

	ReleaseSrvSendSkill();

	if( bSuccess == true )
	{
		int nSkill_Index = GetSkillMng()->GetTamerSkill_Index( nSkillIndex );
		assert_csm1( nSkill_Index != -1, L"스킬 IDX 값이 잘못 되었습니다. IDX = %d", nSkill_Index );
		if( nSkill_Index < 0 || GetSkillMng() == NULL || GetSkillMng()->GetSkill( nSkill_Index ) == NULL ||
			m_SkillMng.GetSkill( nSkill_Index ) == NULL || m_SkillMng.GetSkill( nSkill_Index )->s_pFTSkill == NULL ||
			m_SkillMng.GetSkill( nSkill_Index )->s_pFTSkill->GetInfo() == NULL || GetBaseStat() == NULL )
		{
			SetTamerUserState( TU_NONE );
			SetPauseAll( false );
			return false;
		}

		CsSkill::sINFO* pFTSkillInfo = m_SkillMng.GetSkill( nSkill_Index )->s_pFTSkill->GetInfo();

		// HP감소
		GetBaseStat()->SetDeltaHP( -pFTSkillInfo->s_nUseHP );
		// DS감소
		GetBaseStat()->SetDeltaDS( -pFTSkillInfo->s_nUseDS );

		// 스킬 쿨타임 셋팅
		GetSkillMng()->GetSkill( nSkill_Index )->s_CoolTime.Start();
	}
	else
	{
		cPrintMsg::PrintMsg( 30015 );
	}

	SetTamerUserState( TU_NONE );	
	
	// 움직임 가능하게
	SetPauseAll( false );

	return true;
}

//=============================================================================================================
//
//	이동
//
//=============================================================================================================

void CTamerUser::SetTargetPos( NiPoint3 vTargetPos, float fConnectLength, bool bRenderMoveModel, bool bTamerAttack )
{
	//=====================================================================================
	// 테이머 이동
	//=====================================================================================
	if( IsRide() == false )
	{
		NiPoint2 vResultPos;
		if( GetProp_Path()->CalTargetPath( vResultPos, vTargetPos, fConnectLength ) == true )
		{	
			if( GetProp_Path()->SetPath( vResultPos ) == true )
			{
				if( bTamerAttack == false )
				{
					SetTamerUserState( TU_NONE );
				}

				//=======================================================================
				//		무브 포인트
				//=======================================================================
				if( bRenderMoveModel == true )
					g_pCharResMng->SetMovePoint( NiPoint3( vResultPos.x, vResultPos.y, nsCsGBTerrain::g_pCurRoot->GetHeight( vResultPos ) ) );

				//=======================================================================
				//	서버 이동
				//=======================================================================
				m_UserServerMove.SetServerMoveReady();
			}
		}
	}			
	else
	{
		g_pCharMng->GetDigimonUser( 0 )->SetTargetPos( vTargetPos, fConnectLength, bRenderMoveModel );
	}
}


void CTamerUser::SetAttackPos( CsC_AvObject* pAttackSrc )
{
	if( IsRide() == false )
	{
		//=====================================================================================
		// 테이머 이동
		//=====================================================================================
		if( m_pArriveTarget == NULL )
		{
			SetTargetPos( pAttackSrc->GetPos(), TAMERUSER_ATTACK_DIST, false, false );
		}	

		//=====================================================================================
		// 디지몬 이동
		//=====================================================================================
		g_pCharMng->GetDigimonUser( 0 )->SetDigimonUserState( CDigimonUser::DU_ATTACK, pAttackSrc );
	}
	else
	{
		SetRideEndState( RES_ATTACK, pAttackSrc, NULL );
		SetTargetPos( pAttackSrc->GetPos(), TAMERUSER_ATTACK_DIST, false, false );
	}	
}
	
#ifdef DEBUFF_ITEM
void CTamerUser::SetDebuffPos( CsC_AvObject* pDebuffSrc, float fDebuffDist, int nInvenIndex )
{
	m_fDebuffDist		= fDebuffDist;	// 디버프 아이템 사용 가능 거리
	m_nDebuffInvenIndex = nInvenIndex;	// 디버프 아이템 인벤 인덱스
	m_pDebuffTarget		= pDebuffSrc;	// 디버프 아이템 사용 타겟
	m_vDebuffPos		= pDebuffSrc->GetPos();	// 디버프 타겟의 포지션

	if( IsRide() == false )
	{
		//=====================================================================================
		// 테이머 이동
		//=====================================================================================
		if( m_pArriveTarget == NULL )
		{
			SetTargetPos( m_vDebuffPos, m_fDebuffDist, false, false );
		}	

		// 		//=====================================================================================
		// 		// 디지몬 이동
		// 		//=====================================================================================
		// 		g_pCharMng->GetDigimonUser( 0 )->SetDigimonUserState( CDigimonUser::DU_ATTACK, m_pDebuffTarget );
	}
	else
	{
		SetRideEndState( RES_ATTACK, m_pDebuffTarget, NULL );
		SetTargetPos( m_pDebuffTarget->GetPos(), m_fDebuffDist, false, false );
	}	
}
#endif

void CTamerUser::DoAttackSkillPos()
{
	NiPoint2 vResultPos;
	CsC_AttackProp* pAttackProp = GetProp_Attack();
	CsC_PathProp* pPathProp = GetProp_Path();

	// 최하거리 0보다는 커야하고 어택 가능거리에서 PATH_IGNORE_DIST 만큼 더 가까이 간다. ( 최하 PATH_IGNORE_DIST 은 움직이지 않으므로 )
	float fAttackDist = CsMax( 0, pAttackProp->GetEnableFollowDist() - PATH_IGNORE_DIST );

	ReleaseArriveTarget();
	SetTargetPos( pAttackProp->GetFollowTarget()->GetPos(), fAttackDist, false, true );
}

#ifdef KEYBOARD_MOVE
void CTamerUser::_MoveFollowDigimonPortal()
{
	// 디지몬 위치로
	GetProp_Path()->SetPath( g_pCharMng->GetDigimonUser(0)->GetPos() );

	m_UserServerMove.SetServerMoveReady();
}

void CTamerUser::_MoveFollowDigimon()
{
	// 앞서가는거 방지
	_MoveStop_ByArriveDist();
	// 테이머가 이동해야 하는지 체크
	if( ( _IsDoMove() == true ) )
	{
		// 디지몬 도착점으로 보낸다.
		DoPos_ByDigimon();
	}
}

bool CTamerUser::_IsDoMove()
{
	//디지몬에서 쓰는거 가져옴, 최소 이동 거리 체크
	float fRemainLen = TAMERUSER_DIGIMON_REMAIN_MOVE_LENGTH_CONSTANT + 100.0f;

	float fDeltaLength = fRemainLen*1.5f*GetToolWidth()*0.01f;

	NiPoint2 vPos;
	vPos.x = m_vPos.x;
	vPos.y = m_vPos.y;

	return (vPos - g_pCharMng->GetDigimonUser(0)->GetPos2D()).Length() > PATH_IGNORE_DIST + fDeltaLength;

// 	NiPoint2 vTarget = g_pCharMng->GetDigimonUser(0)->GetProp_Path()->GetArrivePos();		// 디지몬 도착점
// 
// 	float f1,f2;
// 	f1 = (vTarget - vPos).Length();			// 테이머와 디지몬의 도착점 사이 거리
// 
// 	float fDigimonMoveDist = g_pCharMng->GetDigimonUser(0)->GetProp_Path()->GetMoveDist();	// 디지몬 남은 이동 거리
// 	f2 = fDigimonMoveDist + fDeltaLength;	// 디지몬 남은 이동거리 + 최소 거리
// 
// 	return ( f1 > f2 + PATH_IGNORE_DIST );
}

void CTamerUser::DoPos_ByDigimon()
{
	NiPoint2 niTargetPos = _GetArrivePos_ByDigimon();
	NiPoint2 vResultPos;
	if( GetProp_Path()->CalTargetPath( vResultPos, NiPoint3(niTargetPos.x, niTargetPos.y, 0.0f), 0 ) == true )
	{
		if( GetProp_Path()->SetPath( vResultPos ) == true )
		{
			m_UserServerMove.SetServerMoveReady();
		}
	}
// 	GetProp_Path()->SetPath( _GetArrivePos_ByDigimon() );
// 
// 	//=======================================================================
// 	//	서버 이동
// 	//=======================================================================	
// 	m_UserServerMove.SetServerMoveReady();
}

void CTamerUser::_MoveStop_ByArriveDist()
{
	if( g_pCharMng->GetDigimonUser(0)->GetProp_Path()->IsMove() == false )
		return;

	if( GetProp_Path()->IsMove() == false )
		return;

	// 디지몬도 이동하고 있고 테이머도 이동하고 있다면
	// 디지몬 도착점
	float fDigimonDist = g_pCharMng->GetDigimonUser(0)->GetProp_Path()->GetMoveDist();
	// 디지몬 도착점을 기준으로한 테이머 도착점
	NiPoint2 vDest = _GetArrivePos_ByDigimon();
	NiPoint3 vCurPos = GetPos();	
	float fTamerDist = ( vDest - NiPoint2( vCurPos.x, vCurPos.y ) ).Length();

	if( fTamerDist > fDigimonDist )
	{
		SetRotation_Ani( vDest );
		DeletePath();
	}
}

NiPoint2 CTamerUser::_GetArrivePos_ByDigimon()
{
	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser(0);
	NiPoint2 vTarget = pDigimonUser->GetProp_Path()->GetArrivePos();

	NiPoint3 vPath = NiPoint3( vTarget.x, vTarget.y, 0.0f );
	NiPoint3 vPath2( 0, 0, 0 );
	NiPoint3 vDir = vPath - pDigimonUser->GetPos();	
	vDir.z = 0.0f;
	if( vDir.Unitize() < 1.0f )
	{
		vDir = pDigimonUser->GetDir();
		vDir.z = 0.0f;
		vDir.Unitize();
	}

	vPath -= vDir * TAMERUSER_DIGIMON_REMAIN_MOVE_LENGTH_CONSTANT/2.f;
	vPath2 = vPath;

	NiPoint3 cross = NiPoint3( 0, 0, 1 );
	NiPoint3 v = cross.Cross( vDir );
	vPath -= v * TAMERUSER_DIGIMON_REMAIN_MOVE_LENGTH_CONSTANT/2.f * GetToolWidth() * 0.01f;

	if( GetProp_Path()->PathTest( vPath ) == true )
	{
		return NiPoint2( vPath.x, vPath.y );
	}

	assert_cs( vPath2.x != 0 && vPath2.y != 0 );
	if( GetProp_Path()->PathTest( vPath2 ) == true )
	{
		return NiPoint2( vPath2.x, vPath2.y );
	}

	return vTarget;
}

#endif	// KEYBOARD_MOVE

//=============================================================================================================
//
//	라이딩
//
//=============================================================================================================

void CTamerUser::SetRideEndState( eRIDE_END_STATE res, CsC_AvObject* pTarget, sRIDE_TARGET_INFO* pTargetInfo )
{
	if( pTargetInfo != NULL )
		m_RideTargetInfo = *pTargetInfo;

	m_eRideEndState = res;
	SetTamerUserState( TU_TARGET_RIDE_END, pTarget );
}

bool CTamerUser::SetRide()
{
	if( PARENT_CLASS::SetRide() == false )
		return false;

	//CAMERA_ST.SetDeltaHeight( g_pCharMng->GetDigimonUser( 0 )->GetToolHeight() * 0.5f );
	ReleaseArriveTarget();
	m_pRideTarget = NULL;
	m_eRideEndState = RES_NONE;

	return true;
}

bool CTamerUser::SetRideEnd()
{
	//CAMERA_ST.SetDeltaHeight( 120.0f );
	if( PARENT_CLASS::SetRideEnd() == false )
		return false;

	SetTamerUserState( CTamerUser::TU_FOLLOW_DIGIMON );

	// ==== 타입 추가 할때 이부분 생각해서 보자 - 공격 사거리 들어가기 전에 내리기가 될때
	if( ( m_eRideEndState != RES_NONE )&&( m_pRideTarget == NULL ) )
		m_eRideEndState = RES_STOP;

	switch( m_eRideEndState )
	{
	case RES_NONE:
		{
			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
			// 디지몬이 움직이고 있었다면 - 도착지점으로 테이머도 움직이자
			if( pDigimonUser )
				pDigimonUser->SetDigimonUserState( CDigimonUser::DU_FOLLOW );
			
// 			if( pDigimonUser->GetProp_Path()->IsMove() )
// 			{
// 				NiPoint2 vArrive = pDigimonUser->GetProp_Path()->GetArrivePos();
// 				NiPoint3 v = NiPoint3( vArrive.x, vArrive.y, 0 );
// 				nsCsGBTerrain::g_pCurRoot->GetHeight( v );
// 				SetTargetPos( v, 0, true, false );
// 			}
		}
		break;

	case RES_STOP:
		{
			DeletePath();
			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
			pDigimonUser->SetDigimonUserState( CDigimonUser::DU_FOLLOW );
		}
		break;

	case RES_ATTACK:
		{			
			assert_cs( m_pRideTarget );
			g_pCharMng->GetDigimonUser( 0 )->SetDigimonUserState( CDigimonUser::DU_ATTACK, m_pRideTarget );
		}		
		break;

	case RES_SKILL:
		{
			assert_cs( m_pRideTarget );
			CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );

			CsC_AttackProp* pAttackProp = pDigimonUser->GetProp_Attack();
			pAttackProp->ReleaseAttackingTarget();

			assert_csm1( m_pRideTarget->GetLeafRTTI() == RTTI_MONSTER, L"SetRideEnd : 몬스터가 아님 : RTTI = %d", m_pRideTarget->GetLeafRTTI() );
			pAttackProp->SetFollowTarget(	m_pRideTarget, m_RideTargetInfo.s_fValue1,
											(CsC_AttackProp::eFOLLOW_ITERATOR)m_RideTargetInfo.s_nValue1, m_RideTargetInfo.s_bValue1 );
			pDigimonUser->SetDigimonUserState( CDigimonUser::DU_SKILL );
		}
		break;

	default:
		assert_cs( false );
	}

	m_pRideTarget = NULL;
	m_eRideEndState = RES_NONE;

	return true;
}


#undef		PARENT_CLASS

void CTamerUser::ResetSkillCool()
{
	int nCnt = GetSkillMng()->GetSkillCount();
	for( int i = 0 ; i < nCnt ; i++ )
	{
		if( GetSkillMng()->GetSkill( i )->s_CoolTime.IsCoolTime() )
		{
			GetSkillMng()->GetSkill( i )->s_CoolTime.Reset();
			GetSkillMng()->GetSkill( i )->s_CoolTime.SetCoolReset( true );
		}
	}
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
	CProtocol.m_wStr = _T("스킬 쿨타임 초기화 효과 발동");
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

//////////////////////////////////////////////////////////////////////////
// 서버 이전한 시간을 셋팅 한다.
void CTamerUser::SetServerRelocateCompletedTime( unsigned int const& nLimitTIme )
{
	m_nSereverRelocateCompletedTime = nLimitTIme;
	// 서버 동기화로 실제 데이터 보다 10초 뒤에 활성화 되게 추가함.
	if( nLimitTIme != 0 )
		m_nSereverRelocateCompletedTime += 10;	
}

unsigned int CTamerUser::GetServerRelocateCompletedTime() const
{
	return m_nSereverRelocateCompletedTime;
}

// 서버 이전을 할 수 있는 제한 시간이 지났는지 체크
bool CTamerUser::IsEnableServerRelocate() const
{
	SAFE_POINTER_RETVAL( net::net, false );
	if( m_nSereverRelocateCompletedTime == 0 )
		return true;
	int nResult = m_nSereverRelocateCompletedTime - net::net->GetTimeTS();
	return nResult > 0 ? false : true;
}

void CTamerUser::SetExp( __int64 dwExp )
{
	SAFE_POINTER_RET( m_pBaseStat );
	m_pBaseStat->SetExp( dwExp );
}
