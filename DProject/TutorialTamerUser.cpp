#include "stdafx.h"
#include "TutorialTamerUser.h"

#define		PARENT_CLASS		CTamerUser
CsCRTTI_CPP( PARENT_CLASS, CTutorialTamerUser, RTTI_TUTORIAL_TAMER )
CTutorialTamerUser::CTutorialTamerUser()
{

}

CTutorialTamerUser::~CTutorialTamerUser()
{

}

void CTutorialTamerUser::Init(uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwServerOption)
{
	NiPoint3 vPos = NiPoint3( (float)sPos.m_nX, (float)sPos.m_nY, 0.0f );
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	Init( nIDX, nFileTableID, vPos, fRot, szName, pProp, dwServerOption );
}

void CTutorialTamerUser::Init(uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwServerOption)
{
	assert_cs( m_pArriveTarget == NULL );
	assert_cs( m_pBaseStat == NULL );
	assert_cs( pProp->IsOptionPlag( OP_STAT ) );
	m_pBaseStat = NiNew sUSER_STAT( this );

	CTamer::Init( nIDX, nFileTableID, vPos, fRot, szName, pProp, dwServerOption );

	m_UserServerMove.Init( this );
	m_eTamerUserState = TU_FOLLOW_DIGIMON; //2016-11-17-nova TU_NONE->TU_FOLLOW_DIGIMON 마우스로 선택하지 않으면 상태가 변하지 않아서 키보드로만 움직였을경우 따라오지 않음 
	m_eFatigueTalkState = FTS_NONE;
	m_TimeSeqFatigueTalk.SetDeltaTime( 10000 );	

	_Init();

	m_SkillRevTimeSeq.SetDeltaTime( 5000 );
	m_SkillRevTimeSeq.OffEnable();
}

void CTutorialTamerUser::Update(float fDeltaTime, bool bAnimation /*= true */)
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

		if( (!g_pMngCollector->IsSceneState( CMngCollector::eSCENE_NONE ) && !g_pMngCollector->IsSceneState( CMngCollector::eSCENE_TUTORIAL_EVENT ))
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

	CTamer::Update( fDeltaTime, bAnimation );

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

void CTutorialTamerUser::_Init()
{
	if( net::game == NULL )
		_NetOffInit();

	m_bServerSendSkill = false;
	m_dwServerOption = 0;

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

	// connect ui
	SAFE_POINTER_RET( g_pGameIF );

	// set tamer window
	cTamerWindow* pTamerWin = g_pGameIF->GetTamerWindow();
	if( pTamerWin )
		pTamerWin->SetTargetObject( this );

	// set tamer quick slot ui, data
	cQuickSlot* pQuickSlot = g_pGameIF->GetQuickSlot( cQuickSlot::Q_TAMER );
	if( pQuickSlot )
		pQuickSlot->SetTargetObject( this );
	cData_QuickSlot* pDataQuickSlot = g_pDataMng->GetQuickSlot( 0 );
	if( pDataQuickSlot )
	{
		pDataQuickSlot->SetChar( this );
		if( pDataQuickSlot->GetQuickSlot() != NULL )	// 퀵슬롯 복사
			memcpy( g_pDataMng->GetTamerCashQuickSlot(), pDataQuickSlot->GetData(0), sizeof(cResist::sCHAR_QUICKSLOT::sCHAR) );

		m_pQuickSlot = pDataQuickSlot->GetData( 0 );
	}

	// set tamer & system buff info
	cBuffInfo* pTamerBuff = g_pGameIF->GetBuffInfo( IF_BUFF_MY_T );
	if( pTamerBuff )
		pTamerBuff->SetBuffData( NULL );
	cBuffInfo* pSystemBuff = g_pGameIF->GetBuffInfo( IF_BUFF_SYSTEM );
	if( pSystemBuff )
		pSystemBuff->SetBuffData( NULL );
}

void CTutorialTamerUser::_UpdatePortal(const float& fDeltaTime)
{
	if( isEnablePortal() == false )
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
		if(m_dwLastNearPortalID != dwNearPortalID)
		{
			if( nsCsGBTerrain::g_pCurRoot )
			{
				CsGBObject* pPortal = nsCsGBTerrain::g_pCurRoot->GetPortal( dwNearPortalID );
				if( pPortal && !pPortal->IsEnableUse() )
					return;
			}

			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PORTAL_MOVE, &dwNearPortalID );
			m_dwLastNearPortalID = dwNearPortalID;
		}
	}
}

