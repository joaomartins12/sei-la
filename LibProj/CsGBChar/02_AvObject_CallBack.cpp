
#include "stdafx.h"
#include "02_AvObject.h"

bool		CsC_AvObject::g_bEnableVoice = true;

//====================================================================================
//
//		CallBack
//
//====================================================================================

void CsC_AvObject::CallBack_StartSequence( DWORD dwAniID )
{
	switch( dwAniID )
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
	case ANI::DAMAGE_DEAD:
		PlayAniSound( dwAniID );
		return;
	}
}

bool CsC_AvObject::CallBack_EndofSequence( DWORD dwAniID )
{
	switch( dwAniID )
	{
	case ANI::IDLE_CHAR:
	case ANI::IDLE_SHAKE:
		SetAnimation( ANI::IDLE_NORMAL );
		return true;

	case ANI::DIGIVICE_CHAGE:
		SetPauseAll( false );
		SetDefault_IdleAnimation();
		return true;

	case ANI::ATTACK_ATTACK1:
	case ANI::ATTACK_ATTACK2:
	case ANI::ATTACK_MISS:
		GetProp_Attack()->SetHitTarget( NULL );
		SetPauseAll( false );
		SetDefault_IdleAnimation();
		return true;

	// 스킬쪽은
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
	case ANI::ACT_BUFF:
	case ANI::BERSERK_SKILL:
		GetProp_Attack()->SetHitTarget( NULL );
		// 잠겼던거 다 풀자
		SetPauseAll( false );
		SetDefault_IdleAnimation();
		return true;
	case ANI::DAMAGE_DAMAGE1:
	case ANI::DAMAGE_DAMAGE2:
	case ANI::DAMAGE_EVADE1:
	case ANI::DAMAGE_BLOCK:
		SetDefault_IdleAnimation();
		return true;	

		// NPC관련
	case ANI::NPC_NO:
	case ANI::NPC_YES:
	case ANI::NPC_TALK:
		SetAnimation( ANI::IDLE_NORMAL );
		return true;	

		// 앉기
	case ANI::IDLE_SIT_DOWN:
		{			
			SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
			SetAnimation( ANI::IDLE_SIT );
		}
		return true;
	case ANI::IDLE_SIT_UP:
		{
			SetPause( CsC_AvObject::PAUSE_ANIMATION, false );
			SetDefault_IdleAnimation();
		}		
		return true;	
	case ANI::ATTACK_MON_SKILL1_START:
	case ANI::ATTACK_MON_SKILL1_LOOP:
		GetProp_Animation()->SetAnimation( ANI::ATTACK_MON_SKILL1_LOOP );
		return true;
	case ANI::ATTACK_MON_SKILL2_START:
	case ANI::ATTACK_MON_SKILL2_LOOP:
		GetProp_Animation()->SetAnimation( ANI::ATTACK_MON_SKILL2_LOOP );
		return true;
	case ANI::ATTACK_MON_SKILL3_START:
	case ANI::ATTACK_MON_SKILL3_LOOP:
		GetProp_Animation()->SetAnimation( ANI::ATTACK_MON_SKILL3_LOOP );
		return true;
	case ANI::ATTACK_MON_SKILL4_START:
	case ANI::ATTACK_MON_SKILL4_LOOP:
		GetProp_Animation()->SetAnimation( ANI::ATTACK_MON_SKILL4_LOOP );
		return true;
	case ANI::ATTACK_MON_SKILL5_START:
	case ANI::ATTACK_MON_SKILL5_LOOP:
		GetProp_Animation()->SetAnimation( ANI::ATTACK_MON_SKILL5_LOOP );
		return true;
	}
	return false;
}

void CsC_AvObject::CallBack_EventSequence( DWORD dwAniID, CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption )
{	
	switch( pEvent->s_eType )
	{
	case CsC_LoadSequence::sTEXTKEY_EVENT::ET_E_FILE:
		{
			bool bBackupRange = pOption->s_bRangeAttack;
			pOption->s_bRangeAttack = false;
			if( m_CharProp.s_dwPropPlag & PR_EFFECT_ENABLE )
 				GetProp_Effect()->AddEffect( pEvent->s_cText, pEvent, pOption, pEvent->s_fEffectScale );
 			pOption->s_bRangeAttack = bBackupRange;
			break;
		}
		break;
	case CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC:
		{
			switch( pEvent->s_nStaticIndex )
			{
			case nsSTATIC_EVENT::DYE_NORMAL_ATTACK:
				{
					if( IsProperty( CsC_Property::ATTACK ) )
						Event_NormalAttack_Clash( pEvent, pOption, GetProp_Attack()->GetHitTarget() );
				}
				break;
			case nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE:
				{
					if( IsProperty( CsC_Property::ATTACK ) )
						Event_NormalAttack_Clash( pEvent, pOption, GetProp_Attack()->GetHitTarget() );
				}
				break;
			case nsSTATIC_EVENT::DYE_RANGE_ATTACK_START:
				{
					if( IsProperty( CsC_Property::ATTACK ) )
						Event_RangeAttact_Start( pEvent, pOption );
				}
				break;
			case nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE:
				{
					if( IsProperty( CsC_Property::ATTACK ) )
					{
						CsC_AvObject* pTarget = GetProp_Attack()->GetHitTarget();
						if( pTarget && pTarget->IsLoad() )
						{							
							if( pOption->s_nAttackEffectIndex == 0 )
							{
								pTarget->GetProp_Effect()->AddEffect( pEvent->s_cText, pEvent, pOption, pEvent->s_fEffectScale );
							}
							else
							{
								Event_NormalAttack_Clash( pEvent, pOption, pTarget );
							}
						}
					}										
				}
				break;
			}
		}
		break;

	case CsC_LoadSequence::sTEXTKEY_EVENT::ET_S_FILE:
	case CsC_LoadSequence::sTEXTKEY_EVENT::ET_DS_STATIC:
		if( IsProperty( CsC_Property::ATTACK ) == true )
		{
			Event_Sound( pEvent, pOption, GetProp_Attack()->GetHitTarget() != NULL ? GetProp_Attack()->GetHitTarget() : this );
		}
		else
		{
			Event_Sound( pEvent, pOption, this );
		}
		break;

	default:
		assert_cs( false );
	}
}


#define KOR_TR			"Language\\Kor_Tr\\"
#define KOR_ORG			"Language\\Kor_Org\\"
#define KOR_COMMON		"Digimon\\"

void CsC_AvObject::Event_Sound( CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption, CsC_AvObject* pTarget )
{
	assert_cs( pTarget != NULL );

	switch( pEvent->s_eType )
	{
	case CsC_LoadSequence::sTEXTKEY_EVENT::ET_DS_STATIC:
		{
			switch( pEvent->s_nStaticIndex )
			{
			case nsSTATIC_EVENT::DYS_T_Foot_N_01:
				{
					PlaySound( ".\\Game\\T_foot_N_01.wav" );					
				}
				break;
			case nsSTATIC_EVENT::DYS_T_Foot_N_02:
				{
					PlaySound( ".\\Game\\T_foot_N_02.wav" );
				}
				break;
			case nsSTATIC_EVENT::DYS_RANGE_ATTACK_CLASH:
				{					
					if( pTarget )
					{
						switch( pOption->s_eHitType )
						{
						case CsC_AttackProp::HT_Miss:
							pTarget->PlaySound( ".\\Game\\D_att_miss.wav" );
							break;
						default:
							pTarget->PlaySound( pEvent->s_cText );
						}						
					}
				}
				break;
			case nsSTATIC_EVENT::DYS_USER_ONLY:
				{
					if( IsUser() == true )
					{
						PlaySound( pEvent->s_cText );
					}
				}
				break;
			case nsSTATIC_EVENT::DYS_USER_ONLY_LANGUAGE_SOUND:
				if( IsUser() == false )
					break;
				// 아니라면 아래거 실행
			case nsSTATIC_EVENT::DYS_LANGUAGE_SOUND:
				{
					if( g_bEnableVoice == true )
					{
						char cPath[ MAX_TEXTKEY_LEN ];
						if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION )
							sprintf_s( cPath, MAX_TEXTKEY_LEN, "%s%s", KOR_TR, pEvent->s_cText );
						else
							sprintf_s( cPath, MAX_TEXTKEY_LEN, "%s%s2", KOR_ORG, pEvent->s_cText );

						PlaySound( cPath );
					}
				}
				break;
			case nsSTATIC_EVENT::DYS_USER_ONLY_LANGUAGE_COMMON:
				if( IsUser() == false )
					break;
				// 아니라면 아래거 실행
			case nsSTATIC_EVENT::DYS_LANGUAGE_COMMON:
				{
					char cPath[ MAX_TEXTKEY_LEN ];
					sprintf_s( cPath, MAX_TEXTKEY_LEN, "%s%s", KOR_COMMON, pEvent->s_cText );
					PlaySound( cPath );
				}
				break;
			default:
				{
					if( pTarget )
					{
						switch( pOption->s_eHitType )
						{
						case CsC_AttackProp::HT_Miss:
							pTarget->PlaySound( ".\\Game\\D_att_miss.wav" );
							break;
						case CsC_AttackProp::HT_Critical:						
							if( GetProp_Animation()->GetSequenceMng()->IsLastNAtt_Sound() == true )
							{
								// 막타만 크리 사운드 틀어주자
								pTarget->PlaySound( ".\\Game\\D_att_cri.wav" );		// 크리와 함꼐 아래 사운드 같이 틀어 주자
							}
						case CsC_AttackProp::HT_Skill:
						case CsC_AttackProp::HT_Normal:
							{
								switch( pEvent->s_nStaticIndex )
								{
								case nsSTATIC_EVENT::DYS_DMG_ATT_L:			pTarget->PlaySound( ".\\Game\\D_att_L.wav" );			break;
								case nsSTATIC_EVENT::DYS_DMG_ATT_M:			pTarget->PlaySound( ".\\Game\\D_att_M.wav" );			break;
								case nsSTATIC_EVENT::DYS_DMG_ATT_H:			pTarget->PlaySound( ".\\Game\\D_att_H.wav" );			break;
								case nsSTATIC_EVENT::DYS_DMG_ATT_METAL:		pTarget->PlaySound( ".\\Game\\D_att_metal.wav" );		break;
								case nsSTATIC_EVENT::DYS_DMG_ATT_THRUST:	pTarget->PlaySound( ".\\Game\\D_att_thrust.wav" );		break;
								case nsSTATIC_EVENT::DYS_DMG_ATT_WIP:		pTarget->PlaySound( ".\\Game\\D_att_wip.wav" );			break;
								case nsSTATIC_EVENT::DYS_DMG_ATT_STICK:		pTarget->PlaySound( ".\\Game\\D_att_stick.wav" );		break;
								case nsSTATIC_EVENT::DYS_DMG_ATT_POWER:		pTarget->PlaySound( ".\\Game\\D_att_power.wav" );		break;
								}
							}
							break;
						// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
						case CsC_AttackProp::HT_Absorb:			// 흡수 효과음? 
						// MONSTER_SKILL_RENEWAL
						case CsC_AttackProp::HT_Skill_NoAni:	// 몬스터 스킬 (씨앗깔기) 효과음
							break;
						}
					}					
				}
			}			
		}
		break;
	case CsC_LoadSequence::sTEXTKEY_EVENT::ET_S_FILE:
		{		
			int	AniID = GetProp_Animation()->GetAnimationID();

			if( AniID != ANI::MOVE_WALK )	// 기본 걷기 사운드 제외
				PlaySound( pEvent->s_cText );
		}
		break;
	}
}


void CsC_AvObject::Event_RangeAttact_Start( CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption )
{
	CsC_AniProp_Sequence* pSequenceMng = GetProp_Animation()->GetSequenceMng();
	CsC_LoadSequence::sTEXTKEY_EVENT* pNext = NULL;
	std::vector<CsC_EffectObject*> Effects;
	CsC_EffectObject* pMainEffect = NULL;
	while( true )
	{
		pNext = pSequenceMng->NextEvent();

		if( pNext == NULL )
			break;

		switch( pNext->s_eType )
		{
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC:
			switch( pNext->s_nStaticIndex )
			{
			case nsSTATIC_EVENT::DYE_RANGE_ATTACK_FILE:
				{
					assert_cs( pMainEffect == NULL );
					switch(pOption->s_eSKillType)
					{
					case ANI::SKILLTYPE::RANDOMMULTY:
						{
							std::map<int, CsC_AvObject*>* pkMultyTargets = GetProp_Attack()->GetMultyTarget();
							std::map<int, CsC_AvObject*>::iterator kIter = pkMultyTargets->begin();
							int iIdx = 0;
							//NiPoint3 PrevPos = NiPoint3::ZERO;
							for(; kIter != pkMultyTargets->end(); ++kIter)
							{
								//++iIdx;
								//if(PrevPos == NiPoint3::ZERO)
								//	
								//else
								//	pMainEffect = GetProp_Effect()->AddEffect( PrevPos, pNext->s_cText, pNext, pOption, pNext->s_fEffectScale );
								pMainEffect = GetProp_Effect()->AddEffect( pNext->s_cText, pNext, pOption, pNext->s_fEffectScale );
								pMainEffect->SetHitTarget((CsC_AvObject*)kIter->second);
								pMainEffect->SetClashConstantTime( pNext->s_fEventTime);
								pMainEffect->SetDelayTime(0.0f);
						
								////////////////////////////////////////// NiNide 로딩
								Effects.push_back(pMainEffect);
								//PrevPos = kIter->second->GetPos();
							}
						}
						break;
					default:
						{
							pMainEffect = GetProp_Effect()->AddEffect( pNext->s_cText, pNext, pOption, pNext->s_fEffectScale );
							pMainEffect->SetHitTarget( GetProp_Attack()->GetHitTarget() );
							pMainEffect->SetClashConstantTime( pNext->s_fEventTime );
						}
						break;

					}


				}
				break;
			case nsSTATIC_EVENT::DYE_NORMAL_ATTACK:
			case nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE:
			case nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE:
				{
					switch(pOption->s_eSKillType)
					{
					case ANI::SKILLTYPE::RANDOMMULTY:
						{
							for( unsigned int i= 0 ; i < Effects.size(); ++i)
							{
								Effects[i]->AddClashEvent(pNext);
							}
						}
						break;
					default:
						if( pMainEffect != NULL )
							pMainEffect->AddClashEvent( pNext );
						break;
					}

				}
				break;
			case nsSTATIC_EVENT::DYE_RANGE_ATTACK_END:
				return;				
			}
			break;
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_DS_STATIC:
			switch( pNext->s_nStaticIndex )
			{
			case nsSTATIC_EVENT::DYS_RANGE_ATTACK_START:
				{
					PlaySound( pNext->s_cText );
				}
				break;
			default:
				{
					if( pMainEffect != NULL )
						pMainEffect->AddClashSound( pNext );
				}
			}
			break;
		}
	}
}

CsC_LoadSequence::sTEXTKEY_EVENT g_TempTextKeyEvent;
CsC_LoadSequence::sTEXTKEY_EVENT* CalEvent( CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AvObject* pOwner, CsC_AvObject* pTarget )
{
	g_TempTextKeyEvent = *pEvent;

	if( pTarget != NULL )
	{
		g_TempTextKeyEvent.s_vOffset = pTarget->GetPos();
		g_TempTextKeyEvent.s_vOffset.z += pTarget->GetToolHeight()*0.5f;
	}
	else
	{
		g_TempTextKeyEvent.s_vOffset = pOwner->GetCenter() + pOwner->GetDir()*pOwner->GetAttackRange();
	}
	g_TempTextKeyEvent.s_vOffset -= pOwner->GetPos();

	NiPoint3 vRandom = NiPoint3( (float)( rand()%100 - 50 ), (float)( rand()%110 - 55 ), ( float )( rand()%20 - 10 ) );
	vRandom.Unitize();

	if( pTarget )
	{
		int nToolWidth = (int)pTarget->GetToolWidth();
		if( nToolWidth == 0 )
			nToolWidth = 100;
		vRandom *= (float)( rand()%nToolWidth )*1.5f;
	}
	else
	{
		vRandom *= (float)( rand()%60 );
	}
	g_TempTextKeyEvent.s_vOffset += vRandom;
	g_TempTextKeyEvent.s_dwPlag = nsEFFECT::LIVE_ONCE | nsEFFECT::POS_NORMAL | nsEFFECT::DIR_NONE | nsEFFECT::OFFSET_USE;
	return &g_TempTextKeyEvent;
}

void CsC_AvObject::Event_NormalAttack_Clash( CsC_LoadSequence::sTEXTKEY_EVENT* pEvent, CsC_AniProp_Sequence::sEVENT_OPTION* pOption, CsC_AvObject* pTarget )
{
	if( pOption->s_nAttackEffectIndex == -1 )
		return;

	if( pTarget && ( pTarget->IsLoad() == false ) )
		return;

	static CsC_AniProp_Sequence::sEVENT_OPTION eo;
	eo = *pOption;
	eo.s_bRangeAttack = false;	

	switch( eo.s_eHitType )
	{
	case CsC_AttackProp::HT_Critical:
		{
			// 막타만 크리 이펙트 터트려 주자
			if( GetProp_Animation()->GetSequenceMng()->IsLastNAtt_Effect() == true )
			{
				// 카메라 애니메이션
				if( pTarget != NULL )
					AttachCamera_Critical( pTarget );

				if( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE )
					pTarget->GetProp_Effect()->AddEffect( pEvent->s_cText, pEvent, &eo, pEvent->s_fEffectScale );
				else
				{
					CsC_LoadSequence::sTEXTKEY_EVENT* te = CalEvent( pEvent, this, pTarget );
					GetProp_Effect()->AddEffect( "System\\CriticalAttack.nif", te, &eo, GetToolWidth()*0.01f*te->s_fEffectScale );
				}

				if( eo.s_nAttackEffectIndex != 0 )
				{
					CsC_LoadSequence::sTEXTKEY_EVENT* te = CalEvent( pEvent, this, pTarget );

					char cAttack[ 64 ] = {0,};
					sprintf_s( cAttack, 64, "System\\Attack%d.nif", eo.s_nAttackEffectIndex );
					GetProp_Effect()->AddEffect( cAttack, te, &eo, GetToolWidth()*0.01f*te->s_fEffectScale );
				}				

				NiPoint3 vPos = GetPos();
				vPos.z += GetToolHeight()*0.5f;
				GetProp_Effect()->AddEffect( "System\\Critical_Text.nif", vPos, 1.4f );

// 				// 카메라 애니메이션
// 				if( pTarget != NULL )
// 				{
// 					AttachCamera_Critical( pTarget );
// 				}
// 
// 				if( eo.s_nAttackEffectIndex == 0 )
// 				{
// 					if( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE )
// 					{
// 						pTarget->GetProp_Effect()->AddEffect( pEvent->s_cText, pEvent, &eo, pEvent->s_fEffectScale );
// 					}
// 					else
// 					{
// 						CsC_LoadSequence::sTEXTKEY_EVENT* te = CalEvent( pEvent, this, pTarget );
// 						GetProp_Effect()->AddEffect( "System\\CriticalAttack.nif", te, &eo, GetToolWidth()*0.01f*te->s_fEffectScale );
// 					}
// 				}
// 				else
// 				{
// 					CsC_LoadSequence::sTEXTKEY_EVENT* te = CalEvent( pEvent, this, pTarget );
// 
// 					char cAttack[ 32 ];
// 					sprintf_s( cAttack, 32, "System\\Attack%d.nif", eo.s_nAttackEffectIndex );
// 					GetProp_Effect()->AddEffect( cAttack, te, &eo, GetToolWidth()*0.01f*te->s_fEffectScale );
// 				}				
// 
// 				NiPoint3 vPos = GetPos();
// 				vPos.z += GetToolHeight()*0.5f;
// 				GetProp_Effect()->AddEffect( "System\\Critical_Text.nif", vPos, 1.4f );

				break;
			}
		}		
		// 아니면 아래 일반 어택쪽으로 터트려 주자
	case CsC_AttackProp::HT_Skill:
	case CsC_AttackProp::HT_Normal:
	case CsC_AttackProp::HT_Skill_NoAni:	// 몬스터 스킬( 씨앗깔기, 광폭화 )
		{
			g_CsLastError.m_nIntValue1 = eo.s_eHitType;

			if( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE )
			{	
				g_CsLastError.m_nIntValue3 = eo.s_nAttackEffectIndex;

				strcpy_s( g_CsLastError.m_szChar, MAX_PATH, pEvent->s_cText );
				pTarget->GetProp_Effect()->AddEffect( pEvent->s_cText, pEvent, &eo, pEvent->s_fEffectScale );

				if( eo.s_nAttackEffectIndex != 0 )
				{
					CsC_LoadSequence::sTEXTKEY_EVENT* te = CalEvent( pEvent, this, pTarget );
					char cAttack[ MAX_TEXTKEY_LEN ];
					sprintf_s( cAttack, MAX_TEXTKEY_LEN, "System\\Attack%d.nif", eo.s_nAttackEffectIndex );					
					sprintf_s( g_CsLastError.m_szChar, MAX_PATH, "%s", cAttack );
					GetProp_Effect()->AddEffect( cAttack, te, &eo, GetToolWidth()*0.01f*te->s_fEffectScale );					
				}

// 				if( eo.s_nAttackEffectIndex == 0 )
// 				{					
// 					strcpy_s( g_CsLastError.m_szChar, MAX_PATH, pEvent->s_cText );			
// 
// 					pTarget->GetProp_Effect()->AddEffect( pEvent->s_cText, pEvent, &eo, pEvent->s_fEffectScale );
// 				}
// 				else
// 				{
// 					CsC_LoadSequence::sTEXTKEY_EVENT* te = CalEvent( pEvent, this, pTarget );
// 
// 					char cAttack[ MAX_TEXTKEY_LEN ];
// 					sprintf_s( cAttack, MAX_TEXTKEY_LEN, "System\\Attack%d.nif", eo.s_nAttackEffectIndex );					
// 					sprintf_s( g_CsLastError.m_szChar, MAX_PATH, "%s", cAttack );					
// 
// 					GetProp_Effect()->AddEffect( cAttack, te, &eo, GetToolWidth()*0.01f*te->s_fEffectScale );
// 				}
			}
			else
			{
				char cAttack[ MAX_TEXTKEY_LEN ] = {0,};
				if( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_NORMAL_ATTACK )
					strcpy_s( cAttack, MAX_TEXTKEY_LEN, "System\\Attack.nif" );
				else if( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE )
					strcpy_s( cAttack, MAX_TEXTKEY_LEN, pEvent->s_cText );

				CsC_LoadSequence::sTEXTKEY_EVENT* te = CalEvent( pEvent, this, pTarget );
				GetProp_Effect()->AddEffect( cAttack, te, &eo, GetToolWidth()*0.01f*te->s_fEffectScale );

				if( eo.s_nAttackEffectIndex != 0 )
				{
					sprintf_s( cAttack, MAX_TEXTKEY_LEN, "System\\Attack%d.nif", eo.s_nAttackEffectIndex );
					CsC_LoadSequence::sTEXTKEY_EVENT* te = CalEvent( pEvent, this, pTarget );
					GetProp_Effect()->AddEffect( cAttack, te, &eo, GetToolWidth()*0.01f*te->s_fEffectScale );
				}


// 				char cAttack[ MAX_TEXTKEY_LEN ];
// 				if( eo.s_nAttackEffectIndex == 0 )
// 				{
// 					if( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_NORMAL_ATTACK )
// 						strcpy_s( cAttack, MAX_TEXTKEY_LEN, "System\\Attack.nif" );
// 					else if( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE )
// 						strcpy_s( cAttack, MAX_TEXTKEY_LEN, pEvent->s_cText );
// 				}
// 				else
// 				{
// 					sprintf_s( cAttack, MAX_TEXTKEY_LEN, "System\\Attack%d.nif", eo.s_nAttackEffectIndex );
// 				}
// 				
// 				CsC_LoadSequence::sTEXTKEY_EVENT* te = CalEvent( pEvent, this, pTarget );
// 				GetProp_Effect()->AddEffect( cAttack, te, &eo, GetToolWidth()*0.01f*te->s_fEffectScale );
			}
		}
		break;
	case CsC_AttackProp::HT_Miss:
	// BUFF_SKILL - 데미지 흡수 버프 스킬 추가_14.05.14		chu8820
	case CsC_AttackProp::HT_Absorb:
		{		
		}
		break;
	}

// #ifdef BUFF_SKILL	// 데미지 반사 버프 스킬 추가_14.05.14		chu8820
// 	//반사 버프 종류에 따라 이펙트 추가
// 	NiPoint3 vPos = GetPos();
// 	vPos.z += GetToolHeight()*0.5f;
// 
// 	switch( eo.s_nReflectType)	// 버프 클래스
// 	{
// 	case 471:/*화염 반사 데미지*/	GetProp_Effect()->AddEffect( "System\\Debuff\\Debuff_Fire.nif", vPos, 0.7f );		break;
// 	default:	/*기본 반사 데미지*/	GetProp_Effect()->AddEffect( "System\\Debuff\\DeBuff01_Damage.nif", vPos, 0.7f );		break;
// 	}
// #endif
}