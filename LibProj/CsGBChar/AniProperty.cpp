
#include "stdafx.h"
#include "AniProperty.h"

bool		CsC_AniProp::g_bUpdateActor = true;

THREAD_MEMPOOL_CPP( CsC_AniProp )
#define PARENT_CALSS		CsC_Property

CsC_AniProp::CsC_AniProp():m_fAniTime(0.0f),m_pActorMng(NULL),m_dwCurAniID(ANI::INVALIDE_ID)
,m_pSequence(NULL),m_fAniAccelRate(1.0f),m_nBoneLODLevel(0),m_bOldViewSight(true),m_bUseIdleAni(true)
,m_fAniConstantTime_Walk(0.002f),m_fAniConstantTime_Run(0.002f),m_fAniConstantTime_ARun(0.002f),m_bUseStopFrame(true)
{
	m_eType = CsC_Property::ANIMATION;
}

CsC_AniProp::~CsC_AniProp()
{
	m_eType = CsC_Property::ANIMATION;
	m_pActorMng = NULL;
	m_dwCurAniID = ANI::INVALIDE_ID;
	m_pSequence = NULL;
}

void CsC_AniProp::Delete()
{	
	PARENT_CALSS::Delete();

	m_pActorMng = 0;
	m_dwCurAniID = ANI::INVALIDE_ID;
	m_fAniConstantTime_Walk = 0.002f;
	m_fAniConstantTime_Run = 0.002f;
	m_fAniConstantTime_ARun = 0.002f;
	m_bUseIdleAni = true;
	m_fAniTime = 0.0f;
	m_bOldViewSight = true;
	m_fAniAccelRate = 1.0f;
	m_nBoneLODLevel = 0;
	m_bUseStopFrame = true;

	if( m_pSequence != NULL )
	{
		m_pSequence->Delete();
		NiDelete m_pSequence;
		m_pSequence = NULL;
	}
}

void CsC_AniProp::ChangeKFM()
{
	PARENT_CALSS::ChangeKFM();

	assert_cs( m_pActorMng != NULL );
	m_pActorMng = 0;
	assert_cs( m_pSequence != NULL );	
	m_pSequence->Delete();
	NiDelete m_pSequence;
	m_pSequence = NULL;

	m_dwCurAniID = ANI::INVALIDE_ID;
}

void CsC_AniProp::Init( CsC_AvObject* pParent )
{
	m_fAniConstantTime_Walk = 0.002f;
	m_fAniConstantTime_Run = 0.002f;
	m_fAniConstantTime_ARun = 0.002f;

	PARENT_CALSS::Init( pParent );

	assert_cs( m_pActorMng == NULL );
	m_pActorMng = NULL;	
	m_fIdleAniTime = m_pParent->CalIdleAniTime();
	m_bUseIdleAni = true;
	m_fAniTime = 0.0f;
	m_bOldViewSight = true;
	m_fAniAccelRate = 1.0f;

	m_nBoneLODLevel = 0;

	m_bUseStopFrame = true;
}

void CsC_AniProp::CheckModelComp()
{
	PARENT_CALSS::CheckModelComp();

	m_fAniTime = 0.0f;
	m_bOldViewSight = true;

	if( IsKFM() == true )
	{
		// 애니메이션당 가중치 상수값 계산
		CalAniConstant( (CsC_LoadSequence*)NULL );

		_SYNCHRONIZE_THREAD_(&s_CS);
		CsC_LoadSequence* pLoadSequence = m_pSequence->GetLoadSequence();
		pLoadSequence->ApplyModelData( m_pParent->GetApplyModelID() );

		if( m_dwCurAniID == ANI::INVALIDE_ID )
		{
			m_fAniAccelRate = 1.0f;
			m_dwCurAniID = ANI::IDLE_NORMAL;
		}

		m_pSequence->SetTargetAnimation( m_dwCurAniID, false );
		m_pActorMng->SetTargetAnimation( m_dwCurAniID );

		// 차후 경고로 바꾸어 주자
		assert_csm1( m_pActorMng->GetBoneLODController() == NULL, "%s\n 본LOD빼주세요~", m_pActorMng->GetKFMTool()->GetModelPath() );		
	}
}

void CsC_AniProp::Update( float fDeltaTime )
{
	if( m_pParent->IsLoad() == false )
		return;
	
	float fAniAccelTime = m_fAniAccelRate * GetAniConstantTime();
	// 오브젝트의 이동 값이 0일 경우 에니메이션을 안하기 때문에 
	// 애니메이션 AccelTime이 0이 아닐경우에만 기본속도로 에니메이션이되게 수정
	if( fAniAccelTime != 0.0f )
		fDeltaTime *= fAniAccelTime;

	m_fAniTime += fDeltaTime;

	if( IsKFM() == true )
	{
		_UpdateIdleAni( fDeltaTime );
		_UpdateAnimation( fDeltaTime );
	}	
}

void CsC_AniProp::_UpdateAnimation( float fDeltaTime )
{
	bool bChangeAni = m_pSequence->Update( fDeltaTime );
	if( m_pParent->GetViewSight() == true )
	{
		if( g_bUpdateActor == true )
		{
			m_pActorMng->Update( m_fAniTime );
		}
		m_bOldViewSight = true;
	}
	else if( bChangeAni == true )
	{
		m_pActorMng->Update( m_fAniTime );
		CsNodeObj* pNodeObj = m_pParent->GetCsNode();
		if( pNodeObj )
		{
			pNodeObj->m_pNiNode->_UpdateSelect( m_fAniTime, true );
		}
		m_bOldViewSight = false;
	}	
}

void CsC_AniProp::_UpdateIdleAni( float fDeltaTime )
{
	if( m_bUseIdleAni == false )
		return;

	if( m_dwCurAniID != ANI::IDLE_NORMAL )
	{		
		return;
	}

	m_fIdleAniTime -= fDeltaTime;
	if( ( m_fIdleAniTime < 0 ) )
	{
		SetAnimation( m_pParent->GetRandomIdle() );
		m_fIdleAniTime = m_pParent->CalIdleAniTime();
	}
}

void CsC_AniProp::SetAnimation( DWORD dwAniID, bool bUseStopFrame /*=false*/, float fAccelRate /*=1.0f*/ )
{
	if( m_pParent->IsPause( CsC_AvObject::PAUSE_ANIMATION ) == true )
		return;

	_SYNCHRONIZE_THREAD_(&s_CS);

	if( IsKFM() == true )
	{
		if( m_dwCurAniID == dwAniID )
			return;
		
		if( m_pActorMng->SetTargetAnimation( dwAniID ) == true )
		{
			m_dwCurAniID = dwAniID;
			m_pSequence->SetTargetAnimation( dwAniID, m_bUseStopFrame && bUseStopFrame );
		}		
	}
	else
	{
		if( m_dwCurAniID == dwAniID )
			return;

		switch( dwAniID )
		{
		case ANI::ATTACK_ATTACK1:
		case ANI::ATTACK_ATTACK2:
		case ANI::ATTACK_SKILL1:
		case ANI::ATTACK_SKILL2:
		case ANI::ATTACK_SKILL3:
		case ANI::ATTACK_SKILL4:
		case ANI::ATTACK_SKILL5:
		case ANI::ATTACK_MON_SKILL1_START:
		case ANI::ATTACK_MON_SKILL2_START:
		case ANI::ATTACK_MON_SKILL3_START:
		case ANI::ATTACK_MON_SKILL4_START:
		case ANI::ATTACK_MON_SKILL5_START:
		case ANI::ATTACK_MON_SKILL1_LOOP:
		case ANI::ATTACK_MON_SKILL2_LOOP:
		case ANI::ATTACK_MON_SKILL3_LOOP:
		case ANI::ATTACK_MON_SKILL4_LOOP:
		case ANI::ATTACK_MON_SKILL5_LOOP:
		case ANI::ATTACK_MON_SKILL1_USE:
		case ANI::ATTACK_MON_SKILL2_USE:
		case ANI::ATTACK_MON_SKILL3_USE:
		case ANI::ATTACK_MON_SKILL4_USE:
		case ANI::ATTACK_MON_SKILL5_USE:
			break;
		case ANI::NPC_OPEN1:
		case ANI::NPC_OPEN2:
		case ANI::NPC_CLOSE:
			break;
		default:
			m_dwCurAniID = dwAniID;
		}
		assert_cs( m_dwCurAniID != ANI::ATTACK_ATTACK1 );
		assert_cs( m_dwCurAniID != ANI::ATTACK_ATTACK2 );
	}	

	m_fAniAccelRate = fAccelRate;

	// 아이들 지정이면 아이들 랜덤 타임 잰다
	if( m_dwCurAniID == ANI::IDLE_NORMAL )
	{
		m_fIdleAniTime = m_pParent->CalIdleAniTime();
	}
}

void CsC_AniProp::ResetAnimation()
{
	if( m_pParent->IsLoad() == false )
		return;

	if( m_pSequence )
	{
		m_pSequence->ResetAnimation();
		m_pActorMng->GetSequence( m_pActorMng->GetCurAnimation() )->ResetSequence();
	}
	else
	{
		m_fAniTime = 0.0f;
	}
}

float CsC_AniProp::GetAniConstantTime()
{
	float fMoveSpeed = m_pParent->GetBaseStat() ? m_pParent->GetBaseStat()->GetMoveSpeed() : 400.0f;
	if( m_pParent->IsProperty( CsC_Property::PATH ) == true )
	{
		fMoveSpeed = m_pParent->GetProp_Path()->GetMoveSpeed();
	}

	return GetAniConstantTime( fMoveSpeed );	
}

float CsC_AniProp::GetAniConstantTime( float fMoveSpeed )
{
	switch( m_dwCurAniID )
	{
	case ANI::MOVE_WALK:
		return fMoveSpeed*m_fAniConstantTime_Walk/m_pParent->GetScale();
	case ANI::MOVE_RUN:
		return fMoveSpeed*m_fAniConstantTime_Run/m_pParent->GetScale();
	case ANI::MOVE_ATTACKRUN:
		return fMoveSpeed*m_fAniConstantTime_ARun/m_pParent->GetScale();
	}
	return 1.0f;
}

//==========================================================================
//
//	CsC_AniProp_Sequence
//
//==========================================================================

float CsC_AniProp::CalAniAccelRate( DWORD dwAniID, float fAniEndTime )
{
	CsC_LoadSequence::sSEQUENCE_INFO* pSeqInfo = m_pSequence->GetSequence( dwAniID );
	assert_cs( pSeqInfo != NULL );
	// 서버와 공격속도 동기화시 애니메시션 속도가 서버의 다음 어텍 속도보다 느리기 때문에
	// 실제 공격 속도보다 조금 빠르게 애니메시션 플레이를 시키기 위해 0.99을 곱함. 
	// 애니메이션 속보다 0.01% 빠르게 애니 플레이 시킴
	return pSeqInfo->s_fEndTime / (fAniEndTime * 0.99f);
}

bool CsC_AniProp::GetNAttEventIndex( DWORD dwAniID, CsVectorPB< int >** ppOutArray, int& nArrayCount )
{
	CsC_LoadSequence::sSEQUENCE_INFO* pSeqInfo = m_pSequence->GetSequence( dwAniID );	
	if( pSeqInfo == NULL )
	{
		assert_csm3( pSeqInfo != NULL, _T( "model = %d, ani = %d, LoadSeqModelID = %d" ), m_pParent->GetFTID(), dwAniID, m_pSequence->GetLoadSequence()->GetModelID() );
		return false;
	}

	*ppOutArray = &pSeqInfo->s_vpNAtt_EffectIndexArray;
	nArrayCount = pSeqInfo->s_nNAtt_Count;
	return true;
}

float CsC_AniProp::CalEventTime( DWORD dwAniID, int nEventIndex, float fAniAccelRate )
{	
	CsC_LoadSequence::sSEQUENCE_INFO* pSeqInfo = m_pSequence->GetSequence( dwAniID );
	assert_cs( pSeqInfo != NULL );
	assert_cs( pSeqInfo->s_vpTextKeyEvent.IsExistElement( nEventIndex ) == true );
	return pSeqInfo->s_vpTextKeyEvent[ nEventIndex ]->s_fEventTime / fAniAccelRate;
}

//==========================================================================
//
//	Set ActorMng
//
//==========================================================================

void CsC_AniProp::SetActorMng( NiActorManagerPtr pActorMng, CsC_LoadSequence* pLoadSequence )
{
	assert_cs( m_pSequence == NULL );
	m_pSequence = NiNew CsC_AniProp_Sequence;
	m_pSequence->Init( m_pParent, pLoadSequence );
	m_pActorMng = pActorMng;	
}

void CsC_AniProp::CalAniConstant( CsC_LoadSequence* pLoadSequence )
{
#define CSLAST_ERROR
	g_CsLastError.m_eType = CsLastError::__CalAniConstant;
	g_CsLastError.m_pValue1 = pLoadSequence;
	g_CsLastError.m_pValue2 = m_pSequence;

	if( pLoadSequence == NULL)
	{
		assert_cs( m_pSequence != NULL );
		pLoadSequence = m_pSequence->GetLoadSequence();
	}
	g_CsLastError.m_pValue3 = pLoadSequence;

	assert_cs( pLoadSequence != NULL );
	m_fAniConstantTime_Walk = m_fAniConstantTime_Run = m_fAniConstantTime_ARun = 1.0f/550.0f;

	// 애니메이션당 가중치 상수값 계산
	float fValue = m_pParent->_GetAniTimeMoveDist_Walk();
	if( ( fValue != ATMD_INVALIDE )&&( fValue != 0.0f ) )
	{
		g_CsLastError.m_pValue4 = pLoadSequence->GetSequence( ANI::MOVE_WALK );
		g_CsLastError.m_nIntValue1 = m_pParent->GetFTID();
			
		if( pLoadSequence->GetSequence( ANI::MOVE_WALK ) )
			m_fAniConstantTime_Walk = pLoadSequence->GetSequence( ANI::MOVE_WALK )->s_fEndTime/fValue;
	}

	fValue = m_pParent->_GetAniTimeMoveDist_Run();
	if( ( fValue != ATMD_INVALIDE )&&( fValue != 0.0f ) )
	{	
		if( pLoadSequence->GetSequence( ANI::MOVE_RUN ) )
			m_fAniConstantTime_Run = pLoadSequence->GetSequence( ANI::MOVE_RUN )->s_fEndTime/fValue;
	}

	fValue = m_pParent->_GetAniTimeMoveDist_ARun();
	if( ( fValue != ATMD_INVALIDE )&&( fValue != 0.0f ) )
	{		
		if( pLoadSequence->GetSequence( ANI::MOVE_ATTACKRUN ) )
			m_fAniConstantTime_ARun = pLoadSequence->GetSequence( ANI::MOVE_ATTACKRUN )->s_fEndTime/fValue;
	}

#define CSLAST_ERROR
	g_CsLastError.Release();
}

//==========================================================================
//	Ani Enable State
//==========================================================================
bool CsC_AniProp::IsEnableNormalAttack()
{
	if( m_pSequence == NULL )
		return false;

	// 현재 공격 애니메이션이 셋팅되었다면 패스
	switch( m_dwCurAniID )
	{
	case ANI::ATTACK_ATTACK1:
	case ANI::ATTACK_ATTACK2:
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		return false;
	}

	// 시퀀스 매니져 쪽에서 ID 가져오자 - 업데이트 한번이라도 된 ID
	switch( m_pSequence->GetSrcID() )
	{
	case ANI::IDLE_NORMAL:
	case ANI::IDLE_CHAR:
	case ANI::IDLE_SHAKE:
	case ANI::IDLE_ATTACK:	
	case ANI::MOVE_ATTACKRUN:
	case ANI::MOVE_RUN:
	case ANI::MOVE_WALK:
	case ANI::DAMAGE_DAMAGE1:
	case ANI::DAMAGE_DAMAGE2:	
	case ANI::ATTACK_MISS:	
		return true;
	}
	return false;
}

// 죽는 애니메이션이 가능한 애니중인지 체크
bool CsC_AniProp::IsEnableDieAni()
{
	if( m_pSequence == NULL )
		return false;

	// 현재 공격 애니메이션이 셋팅되었다면 패스
	switch( m_dwCurAniID )
	{
	case ANI::ATTACK_ATTACK1:
	case ANI::ATTACK_ATTACK2:
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		return false;
	}

	// 현재 애니메이션
	switch( m_pSequence->GetSrcID() )
	{
	case ANI::DIGIMON_EVOLUTION:
	case ANI::DIGIMON_EVOLUTION_END:
	case ANI::DIGIMON_EVOLUTION_DOWN:
	case ANI::ATTACK_ATTACK1:
	case ANI::ATTACK_ATTACK2:
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		return false;
	}

	return true;
}

bool CsC_AniProp::IsEnableSkill()
{
	// 현재 애벌루션 셋팅되어 잇다면 패스
	switch( m_dwCurAniID )
	{
	case ANI::DIGIMON_EVOLUTION:
	case ANI::DIGIMON_EVOLUTION_END:
	case ANI::DIGIMON_EVOLUTION_DOWN:
		return false;
	}

	// 현재 스킬 애니메이션이 셋팅되어 있다면 패스
	switch( m_dwCurAniID )
	{
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		return false;
	}
	return true;
}


ANI::eTYPE CsC_AniProp::IsEnableDamage()
{
	if( m_pSequence == NULL )
		return (ANI::eTYPE)( ANI::DAMAGE_DAMAGE1+rand()%2 );

	switch( m_pSequence->GetSrcID() )
	{
	case ANI::ATTACK_ATTACK1:
	case ANI::ATTACK_ATTACK2:
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		return ANI::INVALIDE_ID;

	case ANI::DAMAGE_DAMAGE1:		
		return ANI::DAMAGE_DAMAGE2;
	case ANI::DAMAGE_DAMAGE2:
		return ANI::DAMAGE_DAMAGE1;
	}	

	return (ANI::eTYPE)( ANI::DAMAGE_DAMAGE1+rand()%2 );
}

ANI::eTYPE CsC_AniProp::IsEnableEvade()
{
	if( m_pSequence == NULL )
		return ANI::DAMAGE_EVADE1;

	switch( m_pSequence->GetSrcID() )
	{
	case ANI::MOVE_RUN:
	case ANI::MOVE_ATTACKRUN:
	case ANI::MOVE_FATIGUE:
		return ANI::INVALIDE_ID;

	case ANI::ATTACK_ATTACK1:
	case ANI::ATTACK_ATTACK2:
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		return ANI::INVALIDE_ID;
	}	

	return ANI::DAMAGE_EVADE1;
}

ANI::eTYPE CsC_AniProp::IsEnableBlock()
{
	if( m_pSequence == NULL )
		return ANI::DAMAGE_BLOCK;

	switch( m_pSequence->GetSrcID() )
	{
	case ANI::MOVE_RUN:
	case ANI::MOVE_ATTACKRUN:
	case ANI::MOVE_FATIGUE:
		return ANI::INVALIDE_ID;

	case ANI::ATTACK_ATTACK1:
	case ANI::ATTACK_ATTACK2:
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		return ANI::INVALIDE_ID;
	}

	return ANI::DAMAGE_BLOCK;
}

ANI::eTYPE CsC_AniProp::IsEnableKnockBack()
{
	if( m_pSequence == NULL )
		return ANI::DAMAGE_BLOCK;

	switch( m_pSequence->GetSrcID() )
	{
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		return ANI::INVALIDE_ID;
	}

	return ANI::DAMAGE_BLOCK;
}


bool CsC_AniProp::IsEnableAttackMove()
{
	if( m_pSequence == NULL )
		return false;

	switch( m_pSequence->GetSrcID() )
	{
	case ANI::ATTACK_ATTACK1:
	case ANI::ATTACK_ATTACK2:
		return false;
	}

	return true;
}

bool CsC_AniProp::IsEnableEvol()
{
	if( m_pSequence == NULL )
		return false;

	// 현재 스킬 애니메이션이 셋팅되어 있다면 패스
	switch( m_dwCurAniID )
	{
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
	case ANI::DIGIMON_EVOLUTION:
	case ANI::DIGIMON_EVOLUTION_END:
	case ANI::DIGIMON_EVOLUTION_DOWN:
		return false;
	}
	return true;
}

bool CsC_AniProp::IsEnableRide()
{
	if( m_pSequence == NULL )
		return false;

	switch( m_dwCurAniID )
	{
	case ANI::ATTACK_SKILL1:
	case ANI::ATTACK_SKILL2:
	case ANI::ATTACK_SKILL3:
	case ANI::ATTACK_SKILL4:
	case ANI::ATTACK_SKILL5:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
	case ANI::ATTACK_ATTACK1:
	case ANI::ATTACK_ATTACK2:
		return false;
	}
	return true;
}

bool CsC_AniProp::IsDamage()
{
	if( m_pSequence == NULL )
		return false;

	switch( m_dwCurAniID )
	{
	case ANI::DAMAGE_DAMAGE1:		
	case ANI::DAMAGE_DAMAGE2:
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
	case ANI::ATTACK_MON_SKILL2_LOOP:
	case ANI::ATTACK_MON_SKILL3_LOOP:
	case ANI::ATTACK_MON_SKILL4_LOOP:
	case ANI::ATTACK_MON_SKILL5_LOOP:
	case ANI::ATTACK_MON_SKILL1_USE:
	case ANI::ATTACK_MON_SKILL2_USE:
	case ANI::ATTACK_MON_SKILL3_USE:
	case ANI::ATTACK_MON_SKILL4_USE:
	case ANI::ATTACK_MON_SKILL5_USE:
		return ( m_pSequence->IsEndAnimation() == false );
	}
	return false;
}

bool CsC_AniProp::IsBarcodeCardAni()
{
	switch( GetAnimationID() )
	{
	case ANI::BARCODE_CONNECT:
	case ANI::BARCODE_CARD_SUCCESS:
	case ANI::BARCODE_CARD_OVERCOUNT:
	case ANI::BARCODE_CARD_FAIL1:
	case ANI::BARCODE_CARD_FAIL2:
		return true;
	}
	return false;
}
// INSTANT_DUNGEON을 위해 만들것으로 애니번호를 넣으면 이동관련한 애니인지 판단한다.
bool CsC_AniProp::IsMoveObjectAni( DWORD dwAni )
{
	switch( dwAni ) // 더 추가할수있다.
	{
	case ANI::IDLE_NORMAL:
	case ANI::IDLE_SHAKE:
	case ANI::IDLE_CHAR:
	case ANI::NPC_TALK:
	case ANI::NPC_YES:
	case ANI::NPC_NO:
	case ANI::MOVE_WALK:	
	case ANI::MOVE_RUN:
	case ANI::EMOTION_START:
	case ANI::EMOTION_END:
		return true;
	}
	return false;
}

#undef PARENT_CLASS
