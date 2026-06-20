
#include "stdafx.h"
#include "Monster.h"

THREAD_MEMPOOL_CPP( CMonster )
#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CMonster, RTTI_MONSTER )

#define	NEW_CREATE_TIME		2.f

CMonster::CMonster()
{
	m_BuffData.Init( this );
	m_pCharImage = NULL;
	m_pFT_Monster = NULL;
	m_pMonsterIcon = NULL;
}

void CMonster::Delete()
{
	PARENT_CLASS::Delete();

	m_eMonsterState = MONSTER_DIE;
	m_pFT_Monster = NULL;	
	m_nExpireTimeTS = 0;
	m_nTotalExpireTime = 0;
}

void CMonster::PreDelete()
{
	g_pCharResMng->CharImageResDelete( &m_pCharImage );

	PARENT_CLASS::PreDelete();
	m_BuffData.Delete();
	m_Option.Delete();
	m_SkillMng.Delete();
	m_Condition.Delete();
	SAFE_NIDELETE( m_pMonsterIcon );
}

void CMonster::Init( uint nUID, uint nFileTableID, nSync::Pos sPos, float fRot, sCREATEINFO* pProp, eMONSTER_STATE MonsterState )
{
	NiPoint3 vPos = NiPoint3( (float)sPos.m_nX, (float)sPos.m_nY, 0.0f );
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	Init( nUID, nFileTableID, vPos, fRot, pProp, MonsterState );
}

void CMonster::Init( uint nUID, uint nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp, eMONSTER_STATE MonsterState )
{
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
#ifdef COMPAT_487_2
	if (m_pFT_Monster == NULL) {
		return;
	}
#endif
	CsMonster::sINFO * pMonsterInfo = m_pFT_Monster->GetInfo();
	m_fScaleFT = pMonsterInfo->s_fScale;

	m_SkillMng.SetDigimon( this, m_pFT_Digimon );
	m_Condition.Init( this );

	// 초기화
	PARENT_CLASS::Init( nUID, nFileTableID, vPos, fRot, pProp );

	// 보스몬스터는 오라 달아주자
	if( pMonsterInfo->s_nClass == 4 )
		GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BOSS, "system\\LevelEffect\\BossEffect.nif" );
	else if( pMonsterInfo->s_nClass == 7 || pMonsterInfo->s_nClass == 8 )
		GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_BOSS, "system\\D_BlackWheel\\D_BlackWheel_E.nif" );

	_Init( MonsterState );
#ifdef MONSTER_SKILL_GROWTH
	_SetOrgScale( PARENT_CLASS::GetGameScale() );	//	기본 스케일 셋팅( 몬스터 커졌다가 줄어들 때 이값 사용 )
#endif
	m_bIsCasting = false;
}

void CMonster::_Init( eMONSTER_STATE MonsterState )
{
	m_pBaseStat->SetLevel( m_pFT_Monster->GetInfo()->s_nLevel );
	m_pBaseStat->SetMoveSpeed( (float)m_pFT_Monster->GetInfo()->s_nMoveSpeed );
	m_pBaseStat->SetAttackSpeed( m_pFT_Monster->GetInfo()->s_nAttSpeed );
	
	m_eMonsterState = MonsterState;
	switch( m_eMonsterState )
	{
	case MONSTER_CREATE:		
		// 생성 원 설정
		_InitCreateScene();
		break;
	}

	TCHAR szName[ MAX_FILENAME ];
	//테이블에 저장되있는 정보를 가져온다
	_stprintf_s( szName, MAX_FILENAME, _T( "%s %s" ), m_pFT_Monster->GetInfo()->s_szName, m_pFT_Monster->GetInfo()->s_szPenName );

	m_Option.Init( this, szName );
	m_Option.SetIconMark( m_pFT_Monster->GetInfo()->s_dwMonsterID );
	m_nOwnerID = 0;

	m_pCharImage = g_pCharResMng->CharImageResLoad( GetModelID() );

	if( 0 == m_pBaseStat->GetMoveSpeed() )
		SetDoNotMoveObject( true );
}


void CMonster::Update( float fDeltaTime, bool bAnimation )
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

	PARENT_CLASS::Update( fDeltaTime, bAnimation );

	switch( m_eMonsterState )
	{
	case MONSTER_CREATE:		
		_UpdateCreateScene( fDeltaTime );
		break;
	default:
		SetHeightConstant( 0 );
		g_pHpBar->SetBuddy( cHpBar::HP, GetUniqID() );
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


	// 몬스터 상태가 die 인데 애니가 공격 애니인 예외 상황이 발생( SetDie() 함수에서 죽는애니로 변경하나 변경이 안됨(?) )
	// 애니가 공격 애니일경우 죽는 애니로 변경
	if( m_eMonsterState == MONSTER_DIE )
	{
		if( GetProp_Animation()->GetAnimationID() != ANI::DAMAGE_DEAD )
			SetAnimation( ANI::DAMAGE_DEAD );
	}
}

void CMonster::Render()
{
	PARENT_CLASS::Render();
}

void CMonster::Render_PostChar()
{
	if( GetProp_Alpha()->IsHideDistOut() == false )
	{
		m_Option.ResetNameColor();
		m_Option.RenderName();
	}
}

//=============================================================================================================
//
//	제거 대기 상태
//
//=============================================================================================================
void CMonster::SetDeleteState( eDELETE_STATE ds )
{
	PARENT_CLASS::SetDeleteState( ds );
	switch( m_eDeleteState )
	{
	case DS_COSTOM_01:
		{
			DeletePath();

			g_pEffectMng->AddEffect( "System/TimeOver.nif", GetPos(), GetToolWidth()*0.027f );

			CsC_AlphaProp* pProp = GetProp_Alpha();
			pProp->SetType( CsC_AlphaProp::AT_ALPHA_DECREASE );
		}
		break;
	}
}

bool CMonster::UpdateDelete( float fDeltaTime )
{
	switch( m_eDeleteState )
	{
	case DS_COSTOM_01:
		if( GetProp_Alpha()->GetCurAlphaValue() == 0 )
			return false;
		return true;
	}
	return PARENT_CLASS::UpdateDelete( fDeltaTime );
}

//====================================================================================
//	파일 테이블
//====================================================================================
CsDigimon* CMonster::GetDigimonFT()
{ 
	return m_pFT_Digimon;
}
CsMonster* CMonster::GetMonsterFT()
{ 
	return m_pFT_Monster;
}
//=============================================================================================================
//
//	CallBack
//
//=============================================================================================================

bool CMonster::CallBack_EndofSequence( DWORD dwAniID )
{
	switch( dwAniID )
	{
	case ANI::DAMAGE_DEAD:
		g_pCharResMng->InsertDeleteReady( this );
		g_pEffectMng->AddEffect( EFFECT_DEAD, GetPos(), GetToolWidth()*0.01f*EFFECT_DEAD_CONSTANT );		
		PlaySound( SOUND_DEAD );
		return true;
	}
	return PARENT_CLASS::CallBack_EndofSequence( dwAniID );
}

//=============================================================================================================
//
//	상태
//
//=============================================================================================================
CMonster::eMONSTER_STATE CMonster::GetMonsterState()
{ 
	return m_eMonsterState;
}

bool CMonster::IsMonsterState(eMONSTER_STATE eMonState)
{
	return (m_eMonsterState == eMonState) ? true : false;
}

void CMonster::SetBattle()
{
	m_eMonsterState = MONSTER_BATTLE;
	SetDefault_IdleAnimation();
}
void CMonster::SetBattleEnd()
{
	if( m_eMonsterState == MONSTER_DIE )
		return;

	m_eMonsterState = MONSTER_NORMAL;
	SetDefault_IdleAnimation();
#ifdef MONSTER_SKILL_GROWTH
	if( m_BuffData.IsBuffData( nsBuff::BK_MONSTER_GROWTH ) )	//	전투 끝났는데 성장 버프 있으면 제거
	{
		m_BuffData.ReleaseBuff( nsBuff::BK_MONSTER_GROWTH );
#ifndef _GIVE
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::NORMAL_TEXT;
		DmCS::StringFn::Format(CProtocol.m_wStr, _T( "몬스터-%d 전투종료 :: %d 스킬 버프해제" ), m_pFT_Monster->GetInfo()->s_dwMonsterID, nsBuff::BK_MONSTER_GROWTH); 
		//.CProtocol.m_wStr = GetVAString( _T( "몬스터-%d 전투종료 :: %d 스킬 버프해제" ), m_pFT_Monster->GetInfo()->s_dwMonsterID, nsBuff::BK_MONSTER_GROWTH );
	
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
#endif
	}
#endif
}

void CMonster::SetDie()
{
	PARENT_CLASS::SetDie();

	m_BuffData.Delete();
	m_eMonsterState = MONSTER_DIE;
	SetPause( PAUSE_ANIMATION, false );
	SetAnimation( ANI::DAMAGE_DEAD );
	SetPauseAll( true );
	// 해당 몬스터가 시전 중에 죽었을 경우 징표 삭제.. 서버에서 ID 주는게 아니라서 다 찾아봐야됨;
	
	std::map< DWORD, CDigimon* >* pVector = g_pCharMng->GetDigimonMap();
	std::map< DWORD, CDigimon* >::iterator it = pVector->begin();
	for( ; it != pVector->end(); ++it )
	{
		CDigimon* pDigimon = it->second;
		SAFE_POINTER_CON( pDigimon );
		CsC_EffectProp::SKILL_EFF_MAP* pMap = pDigimon->GetProp_Effect()->_GetLoopSkillEffectMap();
		CsC_EffectProp::SKILL_EFF_MAP_IT ItEff = pMap->begin();
		CsC_EffectProp::SKILL_EFF_MAP_IT ItEffEnd = pMap->end();

		while( ItEff != ItEffEnd )
		{
			CsC_AvObject* pBuffEffect = ItEff->second;
			pMap->erase(ItEff);
			pDigimon->GetProp_Effect()->DeleteEffect(pBuffEffect);

			ItEff = pMap->begin();
			ItEffEnd = pMap->end();
		}
	}
}

bool CMonster::IsCasting_Monster()
{ 
	return m_bIsCasting;
}
void CMonster::SetCasting_Monster( bool bCasting )
{ 
	m_bIsCasting = bCasting;
}

//=============================================================================================================
//
//	Path
//
//=============================================================================================================
void CMonster::DeletePath()
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
void CMonster::_SetDefault_IdleAnimation_Stand()
{	
	switch( m_eMonsterState )
	{
	case MONSTER_CREATE:		
		break;
	case MONSTER_NORMAL:
		GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
		break;
	case MONSTER_BATTLE:
		GetProp_Animation()->SetAnimation( ANI::IDLE_ATTACK );
		break;
	case MONSTER_DIE:
		break;
	default:
		assert_cs( false );
	}
}

void CMonster::_SetDefault_IdleAnimation_Move()
{
	// 데미지 애니메이션 중일는 움직이는 애니메이션 잠시 패스
	if( GetProp_Animation()->IsDamage() == true )
		return;
	switch( m_eMonsterState )
	{
	case MONSTER_CREATE:		
		break;
	case MONSTER_NORMAL:
		GetProp_Animation()->SetAnimation( GetProp_Path()->GetMoveState() == CsC_PathProp::MS_WALK ? ANI::MOVE_WALK : ANI::MOVE_RUN );
		break;
	case MONSTER_BATTLE:
		GetProp_Animation()->SetAnimation( ANI::MOVE_ATTACKRUN );
		break;
	case MONSTER_DIE:
		break;
	default:
		assert_cs( false );
	}
}
//====================================================================================
//	스킬
//====================================================================================
cSkill* CMonster::GetSkillMng()
{ 
	return &m_SkillMng;
}

//====================================================================================
//	컨디션
//====================================================================================
cCondition* CMonster::GetCondition()
{
	return &m_Condition;
}
void CMonster::SetCondition( uint nCondition )
{ 
	m_Condition.SetCondition( nCondition );
}
//====================================================================================
//	버프
//====================================================================================
cBuffData* CMonster::GetBuff()
{ 
	return &m_BuffData;
}
//====================================================================================
//	스탯	- 항상 동적으로 구하겠다 - 공식을 변경하고 싶으면 이부분을 바꾸라
//====================================================================================
float CMonster::GetAttackRange()
{ 
	return m_pFT_Monster->GetInfo()->s_nAttRange*GetScale();
}
float CMonster::GetAttackCharSize()
{ 
	return m_pFT_Digimon->GetInfo()->s_dwCharSize*GetScale();
}
// float CMonster::GetAttackSpeed()
// { 
// 	return m_pFT_Monster->GetInfo()->s_nAttSpeed*0.001f;
// }
//=============================================================================================================
//
//	등장 연출
//
//=============================================================================================================

void CMonster::_InitCreateScene()
{
	m_fCreateSceneTime = NEW_CREATE_TIME;
	m_vCreateSceneBackupPos = m_vPos;	

	SetHeightConstant( -1000.0f );
	SetPos( m_vCreateSceneBackupPos );

	// 원은 설정 해 놓으면 알아서 랜더링 되고 사라진다
	g_pCharResMng->MonsterCreateSceneInsert( m_vCreateSceneBackupPos, GetToolWidth()*0.012f, this );	

	SetAnimation( ANI::IDLE_SHAKE );
}

void CMonster::_UpdateCreateScene( float fDeltaTime )
{
	m_fCreateSceneTime -= fDeltaTime;
	SetHeightConstant( GetHeightConstant() + 1000.0f * fDeltaTime / NEW_CREATE_TIME );	

	if( m_fCreateSceneTime < 0.0f )
	{
		m_eMonsterState = MONSTER_NORMAL;
		SetHeightConstant( 0 );
	}

	SetPos( m_vCreateSceneBackupPos );
}
//====================================================================================
//		Sound
//====================================================================================
char* CMonster::GetSoundDir()
{ 
	return m_pFT_Digimon->GetInfo()->s_cSoundDirName;
}
void* CMonster::PlaySound( char* cSoundPath )
{
	return g_pSoundMgr->PlaySystemSound( cSoundPath, GetPos(), this );	// 3D 효과음 
	//return g_pSoundMgr->PlaySound( cSoundPath, GetPos(), this );		// 3D 환경음
}
void* CMonster::PlayStaticSound( char* cSoundPath )
{ 
	return g_pSoundMgr->PlayStaticSound( cSoundPath, GetPos() );
}
//====================================================================================
//
//		부가 정보 로드
//
//====================================================================================
bool CMonster::IsPickEnable()
{ 
	if( !CsC_AvObject::IsPickEnable() )
		return false;

	if( !IsLoad() )
		return false;

	switch( m_eMonsterState )
	{
	case MONSTER_NORMAL:
	case MONSTER_BATTLE:
		return true;
	}
	return false;
}

UINT CMonster::GetClass(){ return nClass::Monster; }
UINT CMonster::GetUniqID(){ cType type; type.m_nClass = GetClass(); type.m_nIDX = GetIDX(); return type.m_nUID; }
NiColor CMonster::GetNameColor()
{
	int nLevelGap = 0;
	if( g_pCharMng->GetDigimonUser( 0 ) != NULL )
		nLevelGap = GetBaseStat()->GetLevel() - g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetLevel();
	else
		nLevelGap = GetBaseStat()->GetLevel() - g_pDataMng->GetPostLoad()->GetDigimonData()->s_nLevel;

	switch( nLevelGap )
	{	
	case 8:		return NiColor( 255.0f/255.0f, 125.0f/255.0f, 125.0f/255.0f );
	case 7:		return NiColor( 255.0f/255.0f, 125.0f/255.0f, 125.0f/255.0f );	
	case 6:		return NiColor( 255.0f/255.0f, 125.0f/255.0f, 125.0f/255.0f );	
	case 5:		return NiColor( 255.0f/255.0f, 178.0f/255.0f, 178.0f/255.0f );			
	case 4:		return NiColor( 255.0f/255.0f, 178.0f/255.0f, 178.0f/255.0f );	
	case 3:		return NiColor( 255.0f/255.0f, 178.0f/255.0f, 178.0f/255.0f );	
	case 2:		return NiColor( 255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f );	
	case 1:		return NiColor( 255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f );	
	case 0:		return NiColor( 255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f );	
	case -1:	return NiColor( 255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f );	
	case -2:	return NiColor( 255.0f/255.0f, 255.0f/255.0f, 255.0f/255.0f );	
	case -3:	return NiColor( 0.717f, 0.733f, 1 );	
	case -4:	return NiColor( 0.717f, 0.733f, 1 );	
	case -5:	return NiColor( 0.717f, 0.733f, 1 );	
	case -6:	return NiColor( 0.58f, 0.666f, 0.996f );	
	case -7:	return NiColor( 0.58f, 0.666f, 0.996f );	
	case -8:	return NiColor( 0.58f, 0.666f, 0.996f );
	case -9:	return NiColor( 0.054f, 0.094f, 0.792f );
	}

	if( nLevelGap > 0 )
		return NiColor( 255.0f/255.0f, 67.0f/255.0f, 67.0f/255.0f );

	return NiColor( 0.745f, 0.745f, 0.745f );
}


UINT CMonster::GetModelID() const
{ 
	SAFE_POINTER_RETVAL( m_pFT_Monster, 0 );
	CsMonster::sINFO* pInfo = m_pFT_Monster->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, 0 );

	return pInfo->s_dwModelID; 
}

CCharOption* CMonster::GetCharOption(){ return &m_Option; }
sCHAR_IMAGE* CMonster::GetCharImage(){ return m_pCharImage; }

TCHAR* CMonster::GetName(){ return m_Option.GetName(); }
bool CMonster::IsRenderName(){ return g_pResist->m_Global.s_bMonsterRenderName && IsEnableObject(); }
float CMonster::_GetAniTimeMoveDist_Walk(){ return m_pFT_Digimon->GetInfo()->s_fWakkLen; }
float CMonster::_GetAniTimeMoveDist_Run(){ return m_pFT_Digimon->GetInfo()->s_fRunLen; }
float CMonster::_GetAniTimeMoveDist_ARun(){ return m_pFT_Digimon->GetInfo()->s_fARunLen; }
float CMonster::CalIdleAniTime(){ return 3.0f + (float)( rand()%2 ); }
ANI::eTYPE CMonster::GetRandomIdle(){ return ( rand()%5 == 0 ) ? ANI::IDLE_SHAKE : ANI::IDLE_CHAR; }
int CMonster::GetAttackRangeType(){ return m_pFT_Digimon->GetInfo()->s_nDigimonType; }
void CMonster::SetOwnerID( uint nUID ){ m_nOwnerID = nUID; }
uint CMonster::GetOwnerID(){ return m_nOwnerID; }
u4 CMonster::GetExpireTimeTS(){ return m_nExpireTimeTS; }
u4 CMonster::GetTotalExpireTime(){ return m_nTotalExpireTime; }
void CMonster::SetExpireTimeTS( n4 nTimeTS ){ m_nExpireTimeTS = nTimeTS; }
void CMonster::SetTotalExpireTime( n4 nTime ){ m_nTotalExpireTime = nTime; }
void CMonster::AttachCamera_Critical( CsC_AvObject* pTarget )
{
	if( pTarget->GetLeafRTTI() != RTTI_DIGIMON_USER )
		return;

	if( g_pResist->m_Global.s_bCameraEnemyCritical == FALSE )
		return;

	CAMERA_ST.AttachCamera( "Critical.nif", false );	
}
bool CMonster::CheckTamerDist(float Dist)
{
	float fCheckDist = Dist;
	fCheckDist *= 3.0f;

	return ( ( GetPos2D() - g_pCharMng->GetTamerUser()->GetPos2D() ).Length() < fCheckDist );
}
#ifdef MONSTER_SKILL_GROWTH
void CMonster::_SetOrgScale( float fScale )
{ 
	m_fOrgScale = fScale;
}
float CMonster::_GetOrgScale()
{ 
	return m_fOrgScale;
}
#endif
#undef		PARENT_CLASS