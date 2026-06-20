
#include "stdafx.h"
#include "Tamer.h"

THREAD_MEMPOOL_CPP( CTamer )
#define		PARENT_CLASS		CsC_PartObject
CsCRTTI_CPP( PARENT_CLASS, CTamer, RTTI_TAMER )

// #ifdef COSTUME_EFFECT
// #define	MOON_RABBIT_COSTUME_COUNT 3
// 
// #ifdef VERSION_TH
// int Moon_Rabbit_Costume[MOON_RABBIT_COSTUME_COUNT] = {9461, 9462, 9463}; //9461 1일, 9462 7일, 9463 30일
// #else
// int Moon_Rabbit_Costume[MOON_RABBIT_COSTUME_COUNT] = {24, 17279, 75090}; //24:테스트용 달토끼 코스튬 //17279:캐쉬템 달토끼 코스튬
// #endif
// 
// #ifdef VERSION_USA
// #define	FOR_U_COSTUME_F 11031
// #define	FOR_U_COSTUME_O 11032
// #define	FOR_U_COSTUME_R 11033
// #define	FOR_U_COSTUME_U 11034
// #else
// #define	FOR_U_COSTUME_F 9728
// #define	FOR_U_COSTUME_O 9729
// #define	FOR_U_COSTUME_R 9730
// #define	FOR_U_COSTUME_U 9731
// #endif
// #endif															  

CTamer::CTamer()
{	
	m_BuffData.Init( this );
	m_pCharImage = NULL;
	m_dwSyncOption = 0;
	m_OrgCostumePartInfo.s_nPartIndex = nsPART::Costume;
	m_OrgCostumePartInfo.s_nFileTableID = 0;
	m_OrgCostumePartInfo.s_nRemainTime = 0;
	m_nDigimonLinkID = 0;
	m_pPat = NULL;
	m_dwMatchTeam = NONE;
	m_dwTamerIcon = 0;
}

void CTamer::Delete()
{
	PARENT_CLASS::Delete();
}

void CTamer::PreDelete()
{
	DeletePat();

	g_pCharResMng->CharImageResDelete( &m_pCharImage );

	PARENT_CLASS::PreDelete();

	m_Option.Delete();
	m_SkillMng.Delete();
	m_Condition.Delete();
	m_BuffData.Delete();

	m_dwSyncOption = 0;
	m_OrgCostumePartInfo.s_nPartIndex = nsPART::Costume;
	m_OrgCostumePartInfo.s_nFileTableID = 0;
	m_OrgCostumePartInfo.s_nRemainTime = 0;
	m_nDigimonLinkID = 0;
	m_nPcbangCostume = 0;
}

void CTamer::Init( uint nIDX, uint nFileTableID, nSync::Pos sPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwSyncOption )
{
	NiPoint3 vPos = NiPoint3( (float)sPos.m_nX, (float)sPos.m_nY, 0.0f );
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	Init( nIDX, nFileTableID, vPos, fRot, szName, pProp, dwSyncOption );
}

void CTamer::Init( uint nIDX, uint nFileTableID, NiPoint3 vPos, float fRot, LPCTSTR szName, sCREATEINFO* pProp, DWORD dwSyncOption )
{
	m_dwSyncOption_OnThreadBackup = m_dwSyncOption = dwSyncOption;

	assert_cs( nsCsFileTable::g_pTamerMng->IsTamer( nFileTableID ) );
	m_pFT_Tamer = nsCsFileTable::g_pTamerMng->GetTamer( nFileTableID );	
	m_SkillMng.SetTamer( this, m_pFT_Tamer );
	m_Condition.Init( this );

	PARENT_CLASS::Init( nIDX, nFileTableID, vPos, fRot, pProp );
	_Init( szName );

	m_eTamerState = TAMER_NORMAL;
	m_eTamerMove = TM_NONE;
	m_nOrgModelID = GetModelID();
	m_nPcbangCostume = 0;
#ifdef GM_CLOCKING
	m_bIsClocking = false;
#endif
}

void CTamer::_Init( LPCTSTR szName )
{
	m_Option.Init( this, szName );	

	//테이머 추가시 팀이 정해진 놈이면 
	if( szName != NULL )
		m_Option.SetMatchIcon( m_dwMatchTeam );//마스터즈매칭 아이콘 세팅

	m_pCharImage = g_pCharResMng->CharImageResLoad( GetModelID() );
}

void CTamer::Update( float fDeltaTime, bool bAnimation )
{
	// 버프 데이타 업데이트
	m_BuffData.Update();

	// pc방 옷 체크
	_CheckPcbangCostume();

	// 디지몬 링크 업데이트
	_UpdateDigimonLink();

	assert_cs( IsProperty( CsC_Property::PATH ) == true );
	if( (GetProp_Path()->IsMove() == true )&&( IsPause( PAUSE_PATH ) == false ) )
	{
		_SetDefault_IdleAnimation_Move();
	}		

	switch( m_eTamerMove )
	{
	case TM_NONE:
		break;
	case TM_RIDE:
		{
			int nLink = GetDigimonLink();
			if( nLink != 0 )
			{
				CDigimon* pDigimon = g_pCharMng->GetDigimon( nLink );
				if( pDigimon )
				{
					SetPos( pDigimon->GetRidePos() );
					SetRotation( pDigimon->GetCurRot(), true );

					if( pDigimon->GetProp_Path()->IsMove() && ( pDigimon->IsPause( PAUSE_PATH ) == false ) )
					{
						_SetDefault_IdleAnimation_Move();
					}
				}
			}
		}
		break;
	default:
		assert_cs( false );
	}

#ifdef TAMERLEVEL_EFFECT
	if( this->GetBaseStat()->GetLevel() == 99 )
	{
		CsC_EffectProp* pProp = this->GetProp_Effect();		
		{
			// 이펙트 아직 없다면 이펙트 추가
			if( pProp->IsLoopEffect( CsC_EffectProp::LE_PCBANG ) == false )
				pProp->AddLoopEffect( CsC_EffectProp::LE_PCBANG, "system\\PC_Tamer_Effect.nif" );
		}
	}
#endif

	PARENT_CLASS::Update( fDeltaTime, bAnimation );	
}

void CTamer::Render()
{
#ifdef GM_CLOCKING
	if( IsClocking() && GetLeafRTTI() != RTTI_TAMER_USER )
		return;
#endif
	PARENT_CLASS::Render();
}

void CTamer::Render_PostChar()
{
#ifdef GM_CLOCKING
	if( IsClocking() && GetLeafRTTI() != RTTI_TAMER_USER )
		return;
#endif

	if(GetProp_Alpha()->IsHideDistOut() == false )
	{
		m_Option.RenderName();
	}

}
void CTamer::SetEvent( int nType )
{
	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nType )->GetInfo();
	char cEffectPath[ MAX_PATH ] = {0, };
	sprintf_s( cEffectPath, MAX_PATH, "Event\\%s.nif", pFTItem->s_cNif );

	GetProp_Animation()->SetAnimation( ANI::NPC_YES );			
	DWORD dwPlag = nsEFFECT::POS_NORMAL;
	GetProp_Effect()->AddFireWorkEffect( cEffectPath, 1.0f, dwPlag );
}


//====================================================================================
//
//		Pat
//
//====================================================================================
void CTamer::AddPat( uint nFileTableID, float fScale )
{
	assert_cs( m_pPat == NULL );
	m_pPat = g_pCharMng->AddPat( this, GetIDX(), nFileTableID, GetPos(), GetCurRot() );
	m_pPat->SetScale( fScale );
}

void CTamer::DeletePat()
{
	if( m_pPat == NULL )
		return;
	g_pCharMng->DeletePat( GetIDX() );
	m_pPat = NULL;
}
CPat* CTamer::GetPat()
{
	return m_pPat;
}

//====================================================================================
//	파츠
//====================================================================================
uint CTamer::GetOrgModelID()
{
	return m_nOrgModelID; 
}	
void CTamer::SetPcbangCostume( uint dwID ) 
{ 
	m_nPcbangCostume = dwID;
}
uint CTamer::GetPcbangCostume() 
{ 
	return m_nPcbangCostume;
}
void CTamer::ThreadLoad_Part( sCHANGE_PART_INFO* pItemCodeArray, bool bIncludeBaseKFM /*=true*/ )
{
	if( IsSyncOption( nTamer::Option::CostumeOff ) == true )
	{
		m_OrgCostumePartInfo = pItemCodeArray[ nsPART::Costume ];		
		pItemCodeArray[ nsPART::Costume ].s_nFileTableID = 0;
	}

	PARENT_CLASS::ThreadLoad_Part( pItemCodeArray, bIncludeBaseKFM );

	for( int i=0; i<nsPART::MAX_TOTAL_COUNT/*nLimit::Equip*/; ++i )
	{
		_CheckPart( &pItemCodeArray[ i ] );
	}

	CheckBuff();
}

void CTamer::ChangePart( sCHANGE_PART_INFO* pPartInfo, bool bCompel/*옵션에 상관없이 강제로 파티 변경*/ )
{
	//아바타 바꾸는 부분
	bool bChangePart = true;	

	if( pPartInfo->s_nPartIndex == nsPART::Costume )
	{
		if( !bCompel && IsSyncOption( nTamer::Option::CostumeOff ) == true )
		{
			if( pPartInfo->s_nFileTableID )
			{
				m_OrgCostumePartInfo = *pPartInfo;
				bChangePart = false;
			}	
		}
	}

	if( bChangePart == true )
		PARENT_CLASS::ChangePart( pPartInfo );
	_CheckPart( pPartInfo );
	CheckBuff();
}

void CTamer::DeletePart( int nPartIndex )
{
	PARENT_CLASS::DeletePart( nPartIndex );

	sCHANGE_PART_INFO cpi;
	cpi.s_nPartIndex = nPartIndex;	
	_CheckPart( &cpi );
}

void CTamer::_CheckPart( sCHANGE_PART_INFO* pPartInfo )
{
	CsC_EffectProp::eLOOP_EFFECT le;
	switch( pPartInfo->s_nPartIndex )
	{
	case nsPART::EquipAura:	le = CsC_EffectProp::LE_AURA;		break;
	case nsPART::Costume:	le = CsC_EffectProp::LE_COSEFFECT;	break;
#ifdef LJW_ENCHANT_OPTION_DIGIVICE_190904
	case nsPART::Digivice:	le = CsC_EffectProp::LE_DIGIVICE;	break;
#endif
#ifdef SDM_TAMER_XGUAGE_20180628
	case nsPART::XAI:		le = CsC_EffectProp::LE_XAI;		break;
#endif
	default:
		return;
	}

	CsC_EffectProp* pProp = GetProp_Effect();
	SAFE_POINTER_RET( pProp );	

	// 기존 이펙트 제거
	if( pProp->IsLoopEffect( le ) )
		pProp->DeleteLoopEffect( le );

	DWORD dwPartID = m_SettingPart[pPartInfo->s_nPartIndex].s_nSettingID;
	if( 0 == dwPartID )
		return;

	if( nsCsFileTable::g_pEffectTBMng )
	{
		std::string strEffect = nsCsFileTable::g_pEffectTBMng->GetEquipEffectFile( dwPartID );
		if( strEffect.empty() )
			return;

		pProp->AddLoopEffect( le, strEffect.c_str() );
	}
}

void CTamer::_CheckPcbangCostume()
{
	if( GetPcbangCostume() == 0 )
		return;	

	if( m_SettingPart[ nsPART::Costume ].s_nSettingID != GetPcbangCostume() )
	{
		sCHANGE_PART_INFO cp;
		cp.s_nFileTableID = GetPcbangCostume();
		cp.s_nPartIndex = nsPART::Costume;
		cp.s_nRemainTime = INT_MAX;

		ChangePart( &cp );
	}

}
void CTamer::CallBack_StartSequence( DWORD dwAniID )
{
	switch( dwAniID )
	{
	case ANI::NPC_NO:
		PlayAniSound( dwAniID );
		return;
	}

	PARENT_CLASS::CallBack_StartSequence( dwAniID );
}


bool CTamer::CallBack_EndofSequence( DWORD dwAniID )
{
	// 이모션 플레이
	if( ( dwAniID >= ANI::EMOTION_START )&&( dwAniID <= ANI::EMOTION_END ) )
	{
		SetDefault_IdleAnimation();
		return true;
	}

	switch( dwAniID )
	{
	case ANI::BARCODE_CONNECT:
	case ANI::IDLE_ITEMCROP:
	case ANI::BARCODE_CARD_SUCCESS:
	case ANI::BARCODE_CARD_FAIL1:
	case ANI::BARCODE_CARD_FAIL2:
	case ANI::BARCODE_CARD_OVERCOUNT:
		SetPauseAll( false );
		SetDefault_IdleAnimation();
		return true;
	}

	return PARENT_CLASS::CallBack_EndofSequence( dwAniID );
}

void CTamer::PostLinkObject()
{
	PARENT_CLASS::PostLinkObject();

	ApplySyncOption( m_dwSyncOption_OnThreadBackup );
}

//====================================================================================
//
//		Path
//
//====================================================================================
void CTamer::DeletePath()
{
	bool bOldMove = GetProp_Path()->IsMove();
	PARENT_CLASS::DeletePath();

	if( bOldMove == true )
	{
		_SetDefault_IdleAnimation_Stand();
	}
}

//====================================================================================
//
//		Property
//
//====================================================================================
void CTamer::_SetDefault_IdleAnimation_Stand()
{
#ifdef KEYBOARD_MOVE
	if( GetCondition()->IsCondition( nSync::Away ) == false )
#endif
	{
		switch( GetProp_Animation()->GetAnimationID() )
		{
		case ANI::IDLE_SIT:
		case ANI::IDLE_SIT_DOWN:
			GetProp_Animation()->SetAnimation( ANI::IDLE_SIT_UP );
			return;
		}
	}

	switch( m_eTamerMove )
	{
	case TM_NONE:
		{
			if( GetCondition()->IsCondition( nSync::Away ) == false )
			{
				switch( m_eTamerState )
				{
				case TAMER_NORMAL:
					{
// #ifdef KEYBOARD_MOVE
// 						bool bStop = true;
// 						if( GetLeafRTTI() == RTTI_TAMER_USER )
// 						{
// 							if( g_pResist->m_Global.s_bMove_KeyBoard )
// 							{
// 								DWORD dwKeyCheck = g_pCharMng->GetTamerUser()->GetUserServerMove()->GetKeyCheck();
// 								if( dwKeyCheck & CUserServerMove::KEY_LEFT ||
// 									dwKeyCheck & CUserServerMove::KEY_RIGHT ||
// 									dwKeyCheck & CUserServerMove::KEY_UP ||
// 									dwKeyCheck & CUserServerMove::KEY_DOWN )
// 									bStop = false;
// 							}
// 						}
// 						if( bStop )
// 						{
// 							if( GetBaseStat()->IsFatigue() == false )
// 								GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
// 							else
// 								GetProp_Animation()->SetAnimation( ANI::IDLE_FATIGUE );
// 						}
// #else
						if( GetBaseStat()->IsFatigue() == false )
							GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
						else
							GetProp_Animation()->SetAnimation( ANI::IDLE_FATIGUE );
/*#endif*/
					}		
					break;
				case TAMER_BATTLE:
					GetProp_Animation()->SetAnimation( ANI::IDLE_ATTACK );
					break;
				case TAMER_DIE:
					break;
				default:
					assert_cs( false );
				}
			}
			else
			{
				switch( m_eTamerState )
				{
				case TAMER_NORMAL:
				case TAMER_BATTLE:
#ifdef KEYBOARD_MOVE
					if( GetProp_Animation()->GetAnimationID() != ANI::IDLE_SIT
						&& GetProp_Animation()->GetAnimationID() != ANI::IDLE_SIT_DOWN )
#endif
					{
						SetPause( PAUSE_ANIMATION, false );
						GetProp_Animation()->SetAnimation( ANI::IDLE_SIT_DOWN );
					}
					//SetPause( PAUSE_ANIMATION, true );
					break;
				case TAMER_DIE:
					break;
				default:
					assert_cs( false );
				}
			}
		}
		break;
	case TM_RIDE:
		break;
	default:
		assert_cs( false );
	}	
}

void CTamer::_SetDefault_IdleAnimation_Move()
{
	// 데미지 애니메이션 중일는 움직이는 애니메이션 잠시 패스
	if( GetProp_Animation()->IsDamage() == true )
		return;

	switch( m_eTamerMove )
	{
	case TM_NONE:
		{
			switch( m_eTamerState )
			{
			case TAMER_NORMAL:
			case TAMER_BATTLE:
				{
					if( GetBaseStat()->IsFatigue() == false )
						GetProp_Animation()->SetAnimation( ANI::MOVE_RUN );
					else
						GetProp_Animation()->SetAnimation( ANI::MOVE_FATIGUE );
				}		
				break;	
			case TAMER_DIE:
				break;
			default:
				assert_cs( false );
			}
		}
		break;

	case TM_RIDE:
		break;

	default:
		assert_cs( false );
	}	
}

//====================================================================================
//
//		상태
//
//====================================================================================
DWORD CTamer::GetSyncOption()
{ 
	return m_dwSyncOption;
}
bool CTamer::IsSyncOption( DWORD dwPlag )
{ 
	return ( ( m_dwSyncOption & dwPlag ) != 0 );
}
void CTamer::ApplySyncOption( DWORD dwSyncOption )
{
	m_dwSyncOption_OnThreadBackup = dwSyncOption;
	if( GetRefOnThreadLoad() || GetRefLinkObject() || GetRefLinkPart() )
		return;

	if( m_dwSyncOption == dwSyncOption )
		return;

	m_dwSyncOption = dwSyncOption;
	assert_cs( m_OrgCostumePartInfo.s_nPartIndex == nsPART::Costume );
	if( IsSyncOption( nTamer::Option::CostumeOff ) == true )
	{
		// 쓰레드 상황이 아니라서 m_SettingPart 써도 됨
		m_OrgCostumePartInfo.s_nFileTableID = m_SettingPart[ nsPART::Costume ].s_nSettingID;
		m_OrgCostumePartInfo.s_nRemainTime = m_SettingPart[ nsPART::Costume ].s_nRemainTime;

		if( m_OrgCostumePartInfo.s_nFileTableID != 0 )
		{
			sCHANGE_PART_INFO cpi;
			cpi.s_nPartIndex = nsPART::Costume;
			cpi.s_nFileTableID = 0;
			cpi.s_nRemainTime = 0;
			ChangePart( &cpi );
		}
	}
	else
	{
		if( m_OrgCostumePartInfo.s_nFileTableID != 0 )
		{			
			ChangePart( &m_OrgCostumePartInfo );
		}
	}
}

void CTamer::ItemCropSound( int nType )
{
}

void CTamer::ItemPickUpAnimation()
{
	SetAnimation( ANI::IDLE_ITEMCROP, 2.0f );
}

void CTamer::PickUpItemGotoInven(cItemData* pItemData)
{

}

void CTamer::SetBattle()
{
	if( m_eTamerState != TAMER_DIE )
	{
		m_eTamerState = TAMER_BATTLE;
		SetDefault_IdleAnimation();
	}
}

void CTamer::SetBattleEnd()
{
	if( m_eTamerState != TAMER_DIE )
	{
		m_eTamerState = TAMER_NORMAL;
		SetDefault_IdleAnimation();
	}
}

bool CTamer::IsBattle()
{
	return ( m_eTamerState == TAMER_BATTLE );
}
#ifdef GM_CLOCKING
bool CTamer:IsClocking(){ return m_bIsClocking; }
void CTamer:SetClocking( bool bClocking ) { m_bIsClocking = bClocking; }
#endif
//====================================================================================
//	Digimon Link
//====================================================================================
void CTamer::SetDigimonLink( int nDigimonIDX )
{ 
	m_nDigimonLinkID = nDigimonIDX;
}
int	 CTamer::GetDigimonLink()
{ 
	return m_nDigimonLinkID;
}
void CTamer::_UpdateDigimonLink()
{
	CDigimon* pDigimon = g_pCharMng->GetDigimon( m_nDigimonLinkID );
	if( pDigimon == NULL )
		return;

	if( pDigimon->IsEvolDegener() )
		return;
#ifdef GM_CLOCKING
	pDigimon->SetClocking( IsClocking() );
#endif

	if( pDigimon->IsBattle() )
	{
		// 전투 중이 아니였으면 전투로
		if( IsBattle() == false )
		{
			SetBattle();
		}
	}
	else
	{
		// 전투 중이 였으면 전투 종료로
		if( IsBattle() == true )
		{
			SetBattleEnd();
		}
	}
}

//====================================================================================
//	씬
//====================================================================================
void CTamer::SceneLevelUp( int nLevel )
{
	PARENT_CLASS::SceneLevelUp( nLevel );

	PlaySound( SOUND_LEVEL_UP );
	DWORD dwPlag = nsEFFECT::POS_CHARPOS;
#ifdef LEVELUP_EFFECT_LOOP
	GetProp_Effect()->AddEffect( EFFECT_TAMER_LEVELUP, GetToolWidth()*0.02f, dwPlag, CsC_EffectObject::eLeveUp );
#else
	GetProp_Effect()->AddEffect( EFFECT_TAMER_LEVELUP, GetToolWidth()*0.02f, dwPlag );
#endif
}

void CTamer::Scene_Evol( CsC_AvObject* pDigimon, int nDigimonFTID )
{
	SetPause( PAUSE_ANIMATION, false );
	SetPause( PAUSE_ROTATION, false );

#ifdef KEYBOARD_MOVE
	// 키보드 이동하는중에 디지몬 교체 시 테이머 이동되지 않는 현상 수정
	if(g_pServerMoveOwner->GetKeyCheck() )
		g_pServerMoveOwner->SetKeyCheck( CUserServerMove::KEY_NONE );
#endif

	SetAnimation( ANI::DIGIVICE_CHAGE );
	GetProp_Effect()->AddEffect( EFFECT_CHARGE, 1.0f, 0 );

	if( pDigimon != NULL )
	{
		SetRotation_Ani( pDigimon );
	}

	SetPause( PAUSE_ANIMATION, true );
	SetPause( PAUSE_PATH, true );
}

void CTamer::Scene_Barcode_Connect()
{
	if( IsPause( PAUSE_ANIMATION ) == false )
	{
		DeletePath();
		SetAnimation( ANI::BARCODE_CONNECT );
		SetPauseAll( true );
	}
}

void CTamer::Scene_Barcode_CardScratch_Success()
{
	if( IsPause( PAUSE_ANIMATION ) == false )
	{
		DeletePath();
		SetAnimation( ANI::BARCODE_CARD_SUCCESS );
		SetPauseAll( true );
	}
}

void CTamer::Scene_Barcode_CardScratch_Faile()
{
	if( IsPause( PAUSE_ANIMATION ) == false )
	{
		DeletePath();
		SetAnimation( rand()%2 ? ANI::BARCODE_CARD_FAIL1 : ANI::BARCODE_CARD_FAIL2 );
		SetPauseAll( true );
	}
}

void CTamer::Scene_Barcode_CardScratch_OverCount()
{
	if( IsPause( PAUSE_ANIMATION ) == false )
	{
		DeletePath();
		SetAnimation( ANI::BARCODE_CARD_OVERCOUNT );
		SetPauseAll( true );
	}
}

void CTamer::Scene_Aura( const CsC_PartObject::sCHANGE_PART_INFO* pInfo ) // 오라템 착용시 chu8820 
{
	m_SettingPart[ nsPART::EquipAura ].s_nSettingID = pInfo->s_nFileTableID;
	if (m_SettingPart[ nsPART::EquipAura ].s_nSettingID == 0)
		return;

	int nItemType = nsCsFileTable::g_pItemMng->GetItem( m_SettingPart[ nsPART::EquipAura ].s_nSettingID )->GetInfo()->s_nType_L;
	if(nItemType == 31)
	{
		GetProp_Effect()->AddEffect( EFFECT_AURA_DIGI, 1.0f, 0 );
		PlaySound("digimon\\skill\\casting_10.wav"); //소리 강제 재생... effectsound dat파일 설정이 뭔가 잘못된 것 같은데 잘 모르겠음
		PlaySound("game\\Character_levelup_01.wav");
	}
}

//====================================================================================
//	스킬
//====================================================================================
cSkill* CTamer::GetSkillMng()
{
	return &m_SkillMng;
}
//====================================================================================
//	컨디션
//====================================================================================
cCondition* CTamer::GetCondition()
{ 
	return &m_Condition;
}
void CTamer::SetCondition( uint nCondition )
{ 
	m_Condition.SetCondition( nCondition );
}
//void CTamer::SetExpCondition( uint nExpCondition ){ m_Condition.SetExpCon( nExpCondition ); }
//void CTamer::AddExpCondition( uint nExpCondition ){ m_Condition.AddExpCon( nExpCondition ); }
//void CTamer::ReleaseExpCondition( uint nExpCondition ){ m_Condition.ReleaseExpCon( nExpCondition ); }


//====================================================================================
//	버프
//====================================================================================
cBuffData* CTamer::GetBuff()
{
	return &m_BuffData;
}
void CTamer::CheckBuff()
{
	m_BuffData.ReleaseBuff( nsBuff::BK_EQUIP_FRIENDSHIP );
	m_BuffData.ReleaseBuff( nsBuff::BK_EQUIP_EXP );
	m_BuffData.ReleaseBuff( nsBuff::BK_EQUIP_PC );

	for( int i=0; i<nsPART::MAX_TOTAL_COUNT; ++i )
	{

		if( m_SettingPart[ i ].s_nSettingID == 0 )
		{
#ifdef GM_COSTUME
			if( i == nsPART::Costume )
			{
				CsC_EffectProp* pProp = GetProp_Effect();
				if( pProp->IsLoopEffect( CsC_EffectProp::LE_PCBANG ) )
					pProp->DeleteLoopEffect( CsC_EffectProp::LE_PCBANG );
			}			
#endif

			continue;
		}

		if( i == nsPART::Costume && m_nPcbangCostume != 0 )
		{
			if( m_BuffData.IsBuffData( nsBuff::BK_EQUIP_PC ) == false )
				m_BuffData.SetBuff( nsBuff::BK_EQUIP_PC, 0, UINT_MAX, 0 );

			continue;
		}

		CsItem* pEquipItem = nsCsFileTable::g_pItemMng->GetItem( m_SettingPart[ i ].s_nSettingID );
		SAFE_POINTER_CON( pEquipItem );

		CsItem::sINFO* pItemInfo = pEquipItem->GetInfo();
		SAFE_POINTER_CON( pItemInfo );

		DWORD dwSkillCode = pItemInfo->s_dwSkill;	

		if( nsCsFileTable::g_pSkillMng->IsSkill( dwSkillCode ) == false )
			continue;

		CsSkill* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( dwSkillCode );
		SAFE_POINTER_CON( pFTSkill );
		CsSkill::sINFO* pFTSkillInfo = pFTSkill->GetInfo();
		SAFE_POINTER_CON( pFTSkillInfo );
		for( int skill=0; skill<SKILL_APPLY_MAX_COUNT; ++skill ) 
		{
			if( pFTSkillInfo->s_Apply[ skill ].s_nID == 0 )
				continue;

			switch( pFTSkillInfo->s_Apply[ skill ].s_nA )
			{
			case 14:
				{	
					if( m_SettingPart[ i ].s_nRemainTime > 0 )
					{
						if( m_BuffData.IsBuffData( nsBuff::BK_EQUIP_FRIENDSHIP ) == false )						
							m_BuffData.SetBuff( nsBuff::BK_EQUIP_FRIENDSHIP, 0, UINT_MAX, 0  );												
					}								
				}				
				break;
			case 15:				
				{
					if( m_SettingPart[ i ].s_nRemainTime > 0 )
					{
						if( m_BuffData.IsBuffData( nsBuff::BK_EQUIP_EXP ) == false )
							m_BuffData.SetBuff( nsBuff::BK_EQUIP_EXP, 0, UINT_MAX, 0  );						
					}					
				}
				break;
			}			
		}
#ifdef GM_COSTUME
		if( i == nsPART::Costume )
		{			
			CsC_EffectProp* pProp = GetProp_Effect();		
			if( m_SettingPart[ i ].s_nSettingID == 50 )
			{
				// 이펙트 아직 없다면 이펙트 추가
				if( pProp->IsLoopEffect( CsC_EffectProp::LE_PCBANG ) == false )
					pProp->AddLoopEffect( CsC_EffectProp::LE_PCBANG, "system\\PC_Tamer_Effect.nif" );
			}
			else
			{
				pProp->DeleteLoopEffect( CsC_EffectProp::LE_PCBANG );
			}
		}		
#endif
	}
}
//====================================================================================
//		Sound
//====================================================================================
char* CTamer::GetSoundDir()
{ 
	return m_pFT_Tamer->GetInfo()->s_cSoundDirName;
}
void* CTamer::PlaySound( char* cSoundPath )
{ 
	return g_pSoundMgr->PlaySystemSound( cSoundPath, GetPos(), this );
}
void* CTamer::PlayStaticSound( char* cSoundPath )
{ 
	return g_pSoundMgr->PlayStaticSound( cSoundPath, GetPos() );
}
//====================================================================================
//
//		부가 정보 로드
//
//====================================================================================
UINT CTamer::GetClass()
{ 
	return nClass::Tamer;
}
UINT CTamer::GetUniqID()
{ 
	cType type; type.m_nClass = GetClass();
	type.m_nIDX = GetIDX();
	return type.m_nUID;
}
NiColor CTamer::GetNameColor()
{
	std::pair<int, bool> isMember;
	isMember.first = GetUniqID();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_PARTYMEMBER_FROM_TAMERUIDX, &isMember );
	if( isMember.second )
 		return NiColor( 28/255.0f, 102/255.0f, 193/255.0f );

	return NiColor( 40.0f/255.0f, 215.0f/255.0f, 0.0f/255.0f );	
}


CCharOption* CTamer::GetCharOption()
{ 
	return &m_Option; 
}
sCHAR_IMAGE* CTamer::GetCharImage()
{ 
	return m_pCharImage;
}
TCHAR* CTamer::GetName()
{ 
	return m_Option.GetName();
}
bool CTamer::IsRenderName()
{ 
	return g_pResist->m_Global.s_bOtherRenderName && IsEnableObject();
}
float CTamer::GetToolHeight()
{ 
	return IsRide() == false ? m_fToolHeight*GetScale() : m_fToolHeight*GetScale()*0.6f;
}
UINT CTamer::_GetTimeTS()
{
	return _TIME_TS;
}

//=============================================================================================================
//테이머 아이콘
//=============================================================================================================
DWORD CTamer::GetMatchTeam()
{ 
	return m_dwMatchTeam;
}
void CTamer::SetMatchTeam( u1 nTeam )
{
	m_dwMatchTeam = nTeam;
}
//=============================================================================================================
//
//	라이딩
//
//=============================================================================================================

bool CTamer::SetRide()
{
	if( m_eTamerState == TAMER_DIE )
		return false;

	if( IsRide() == true )
		return false;

	CDigimon* pDigimon = g_pCharMng->GetDigimon( GetDigimonLink() );
	if( pDigimon == NULL )
		return false;

	if( pDigimon->GetFTRide() == NULL )
		return false;

	SetPause( PAUSE_PATH, true );
	m_eTamerMove = TM_RIDE;
	SetDefault_IdleAnimation();

	return true;
}

bool CTamer::SetRideEnd()
{
	if( m_eTamerState == TAMER_DIE )
		return false;

	if( IsRide() == false )
		return false;

	SetRotX( 0.0f );
	SetPause( PAUSE_PATH, false );

	m_eTamerMove = TM_NONE;
	SetDefault_IdleAnimation();

	CDigimon* pDigimon = g_pCharMng->GetDigimon( GetDigimonLink() );
	if( pDigimon )
	{
		SetPos( pDigimon->GetPos() );
	}

	GetProp_Path()->ResetAgent();
	return true;
}

bool CTamer::IsRide()
{
	return ( m_eTamerMove == TM_RIDE );
}



#undef		PARENT_CLASS