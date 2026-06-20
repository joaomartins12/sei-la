

#include "stdafx.h"
#include "Network.h"

//===============================================================================================
//
// 어택정보 설정
//
//===============================================================================================
void nsNETWORK::SetAttackInfo(	CsC_AvObject* pHitter, 
								CsC_AvObject* pTarget,
								DWORD dwAniID,
								CsC_AttackProp::eHIT_TYPE ht,
								CsC_AttackProp::eDAMAGE_TYPE dt )
{
	if( pHitter && ( pHitter->IsLoad() == false ) )
		pHitter = NULL;

	if( pTarget && ( pTarget->IsLoad() == false ) )
		pTarget = NULL;


	//==============================================================================
	//	둘다 안보일때
	//==============================================================================
	if( ( pHitter == NULL )||( pTarget == NULL ) )
		return;	

	bool bRangeAttack = ( pHitter && pHitter->GetAttackRangeType() == CsC_AvObject::ART_RANGER );

	//==============================================================================
	// 리는넘의 애니메이션 셋팅값을 구하자
	//==============================================================================	
	CsC_AniProp* pHitterAniProp = pHitter->GetProp_Animation();
	CsC_AniProp_Sequence* pHitterAniPropSeq = pHitter->GetProp_Animation()->GetSequenceMng();
	assert_cs( pHitterAniPropSeq );

	float fAttSpeed = pHitter->GetAttackSpeed();
	float fAniAccelRate;
		
	// 20%의 아이들 여유시간을 두겠다.
	float fEndTime = pHitterAniPropSeq->GetSequence( dwAniID )->s_fEndTime;
	fAniAccelRate = pHitter->GetProp_Animation()->CalAniAccelRate( dwAniID, CsMin( fEndTime, fAttSpeed*0.8f ) );		

	int nEventCount = 1;
	CsVectorPB< int >* pEventArray;
	pHitter->GetProp_Animation()->GetNAttEventIndex( dwAniID, &pEventArray, nEventCount );
	assert_cs( nEventCount > 0 );

	CsVectorPB< float > vpEventTime( 10 );
	for( int i=0; i<nEventCount; ++i )
	{
		vpEventTime.PushBack( pHitter->GetProp_Animation()->CalEventTime( dwAniID, pEventArray->GetData( i ), fAniAccelRate ) );
	}

	// 레인지 타입이라면
	if( bRangeAttack )
	{
		float fDist = ( pHitter->GetPos2D() - pTarget->GetPos2D() ).Length();
		float fRangeHitTime = CsC_AttackProp::CalRangeHitTime( CsC_AttackProp::GetNormalRangeVelocity(), CsC_AttackProp::GetNormalRangeAccelVelocity(), fDist );
		for( int i=0; i<nEventCount; ++i )
		{
			vpEventTime[ i ] += fRangeHitTime;
		}			
	}

	//==============================================================================
	// 맞는넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sDAMAGE_INFO* pDamageInfo = NULL;
	// 데미지 계산		
	float fMin = 90.0f/(float)nEventCount;
	float fMax = 110.0f/(float)nEventCount;

	// 데미지 날라가는 방향 계산
	NiPoint3 v = pTarget->GetPos() - pHitter->GetPos();
	v.z = 0.0f;
	v.Unitize();
	float fRot = NiACos( NiPoint3( 0.0f, -1.0f, 0.0f ).Dot( v ) );
	if( NiPoint3( 0.0f, -1.0f, 0.0f ).Cross( v ).z > 0 )
		fRot = -fRot;	

	for( int i=nEventCount-1; i>-1; --i )
	{
		CsC_AttackProp::sDAMAGE_INFO di;
		di.s_eDamageType = dt;
		di.s_fHitEventTime = vpEventTime[ i ];
		di.s_fDamageRot = fRot;
		di.s_eActive = CsC_AttackProp::AT_NONE_ACTIVE;
		di.s_dwResistTime = GetTickCount() + (DWORD)( vpEventTime[ i ]*2000.0f );
		di.s_nNumType = 0;
		memset( di.s_nValue, 0, sizeof( int )*5 );		
		di.s_pChildDamageInfo = pDamageInfo;
		pDamageInfo = pTarget->GetProp_Attack()->InsertDamage( &di );
	}

	//==============================================================================
	// 때린넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sHIT_INFO info;
	info.s_eHitType = ht;
	info.s_dwAniID = dwAniID;	
	info.s_fAniAccelRate = fAniAccelRate;
	info.s_pPartnerDamageInfo = pDamageInfo;
	info.s_pHitTarget = pTarget;
	info.s_fRangeVelocity = CsC_AttackProp::GetNormalRangeVelocity();
	info.s_fRangeAccelVelocity = CsC_AttackProp::GetNormalRangeAccelVelocity();
	info.s_bRangeAttack = bRangeAttack;
	info.s_bRotation = true;
	info.s_vDamagePos = pTarget->GetPos();
	info.s_nAttackEffectIndex = pHitter->IsStrongNature( pTarget ) ? pHitter->GetNatureType() : 0;

	pHitter->GetProp_Attack()->InsertHit( &info );
}


//===============================================================================================
//
// 스킬정보 설정
//
//===============================================================================================
void nsNETWORK::SetSkillInfo(	CsC_AvObject* pHitter, 
								CsC_AvObject* pTarget,
								int nSkillType			 )
{
	if( pHitter && ( pHitter->IsLoad() == false ) )
		pHitter = NULL;

	if( pTarget && ( pTarget->IsLoad() == false ) )
		pTarget = NULL;

	//==============================================================================
	//	둘중에 하나라도 안보이면 패스
	//==============================================================================
	if( ( pHitter == NULL )||( pTarget == NULL ) )
		return;

	assert_cs( nSkillType < DIGIMON_MAX_SKILL_COUNT );
	DWORD dwSkill = 0;
	CsSkill::sINFO* pFTSkillInfo = NULL;
	CsTamerSkill::sTSkill_INFO* pTamerSkillInfo = NULL;
	bool bRangeAttack = false;

	if( pHitter->GetFTID() >= 80001 && pHitter->GetFTID() <= 80008 )
	{
		dwSkill = nsCsFileTable::g_pTamerMng->GetTamer( pHitter->GetFTID() )->GetInfo()->s_Skill[ nSkillType ].s_dwID;
		pTamerSkillInfo = nsCsFileTable::g_pSkillMng->GetTamer_Skill( dwSkill, -1 )->Get_TSkill_Info();
		pFTSkillInfo = nsCsFileTable::g_pSkillMng->GetSkill( pTamerSkillInfo->s_dwSkillCode )->GetInfo();
		bRangeAttack = ( pFTSkillInfo->s_nAttType == 2 );
	}
	else
	{
		dwSkill = nsCsFileTable::g_pDigimonMng->GetDigimon( pHitter->GetFTID() )->GetInfo()->s_Skill[ nSkillType ].s_dwID;
		pFTSkillInfo = nsCsFileTable::g_pSkillMng->GetSkill( dwSkill )->GetInfo();
		bRangeAttack = ( pFTSkillInfo->s_nAttType == 2 );
	}

	//==============================================================================
	// 리는넘의 애니메이션 셋팅값을 구하자
	//==============================================================================	
	DWORD dwAniID = ANI::ATTACK_SKILL1 + nSkillType;
	int nEventCount = 1;
	CsVectorPB< int >* pEventArray;
	pHitter->GetProp_Animation()->GetNAttEventIndex( dwAniID, &pEventArray, nEventCount );
	assert_cs( nEventCount > 0 );

	CsVectorPB< float > vpEventTime( 10 );
	for( int i=0; i<nEventCount; ++i )
	{
		vpEventTime.PushBack( pHitter->GetProp_Animation()->CalEventTime( dwAniID, pEventArray->GetData( i ), 1.0f ) );
	}

	// 레인지 타입이라면
	if( bRangeAttack == true )
	{
		float fDist = ( pHitter->GetPos2D() - pTarget->GetPos2D() ).Length();
		float fRangeHitTime = CsC_AttackProp::CalRangeHitTime( pFTSkillInfo->s_fSkill_Velocity, pFTSkillInfo->s_fSkill_Accel, fDist );
		for( int i=0; i<nEventCount; ++i )
		{
			vpEventTime[ i ] += fRangeHitTime;
		}
	}

	//==============================================================================
	// 맞는넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sDAMAGE_INFO* pDamageInfo = NULL;

	// 데미지 날라가는 방향 계산
	NiPoint3 v = pTarget->GetPos() - pHitter->GetPos();
	v.z = 0.0f;
	v.Unitize();
	float fRot = NiACos( NiPoint3( 0.0f, -1.0f, 0.0f ).Dot( v ) );
	if( NiPoint3( 0.0f, -1.0f, 0.0f ).Cross( v ).z > 0 )
		fRot = -fRot;

	// 데미지 계산
	float fMin = 90.0f/(float)nEventCount;
	float fMax = 110.0f/(float)nEventCount;
	for( int i=nEventCount-1; i>-1; --i )
	{
		CsC_AttackProp::sDAMAGE_INFO di;
		di.s_eDamageType = CsC_AttackProp::DT_Normal;
		di.s_fHitEventTime = vpEventTime[ i ];
		di.s_fDamageRot = fRot;		
		di.s_eActive = CsC_AttackProp::AT_NONE_ACTIVE;
		di.s_dwResistTime = GetTickCount() + (DWORD)( vpEventTime[ i ]*2000.0f );
		di.s_nNumType = 0;
		memset( di.s_nValue, 0, sizeof( int )*5 );
		di.s_pChildDamageInfo = pDamageInfo;
		pDamageInfo = pTarget->GetProp_Attack()->InsertDamage( &di );
	}

	//==============================================================================
	// 때린넘의 정보 만들자
	//==============================================================================
	CsC_AttackProp::sHIT_INFO info;
	info.s_eHitType = CsC_AttackProp::HT_Skill;
	info.s_dwAniID = dwAniID;	
	info.s_fAniAccelRate = 1.0f;
	info.s_pPartnerDamageInfo = pDamageInfo;
	info.s_pHitTarget = pTarget;
	info.s_fRangeVelocity = pFTSkillInfo->s_fSkill_Velocity;
	info.s_fRangeAccelVelocity = pFTSkillInfo->s_fSkill_Accel;
	info.s_bRangeAttack = bRangeAttack;
	info.s_bRotation = true;
	info.s_vDamagePos = pTarget->GetPos();
	info.s_nAttackEffectIndex = 0;

	pHitter->GetProp_Attack()->InsertHit( &info );
	pHitter->RecvSkill( true, nSkillType );	
}