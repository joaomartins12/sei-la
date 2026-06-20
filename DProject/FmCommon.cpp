
#include "stdafx.h"
#include "FmCommon.h"

float FMCommon::CalDist_2D( CsC_AvObject* pSrc, CsC_AvObject* pDest )
{
	return ( pSrc->GetPos2D() - pDest->GetPos2D() ).Length();
}

float FMCommon::CalDist( CsC_AvObject* pSrc, CsC_AvObject* pDest )
{
	return ( pSrc->GetPos() - pDest->GetPos() ).Length();
}

int FMCommon::GetSkillAtt( DWORD nSkillID, int nSkillLevel, int nApplyIndex )
{
	if( SKILL_APPLY_MAX_COUNT <= nApplyIndex )
		return 0;

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pSkillMng, 0 );

	CsSkill* pCsSkill = nsCsFileTable::g_pSkillMng->GetSkill( nSkillID );
	SAFE_POINTER_RETVAL( pCsSkill, 0 );

	CsSkill::sINFO* pFTSkill = pCsSkill->GetInfo();
	SAFE_POINTER_RETVAL( pFTSkill, 0 );
	
	bool bDigimonSkill = pCsSkill->IsDigimonSkill();
	if( !bDigimonSkill ||	// 디지몬 스킬이 아닌경우(아이템/테이머) 와
		nApplyIndex == 0 )	// 디지몬 공격스킬 효과는 기존 공식 사용
		return pFTSkill->s_Apply[ nApplyIndex ].s_nB + nSkillLevel*pFTSkill->s_Apply[ nApplyIndex ].s_nIncrease_B_Point;

	//디지몬 특수스킬 효과는 레벨을 1 빼주고 계산
	switch( pFTSkill->s_Apply[ nApplyIndex ].s_nA )
	{
	// 크리티컬 / 회피 증가 스킬 추가 14.05.28 chu8820
	case APPLY_CA:
	case APPLY_EV:
		{
			if( pFTSkill->s_Apply[ nApplyIndex ].s_nID == 207/*nSkill::Me_207*/ )
			{
				int nValue = pFTSkill->s_Apply[ nApplyIndex ].s_nB + (nSkillLevel - 1)*pFTSkill->s_Apply[ nApplyIndex ].s_nIncrease_B_Point;
				return ( nValue * 100 );
			}
			else //if( pFTSkill->s_Apply[ nApplyIndex ].s_nID == 206/*nSkill::Me_206*/ )
				return pFTSkill->s_Apply[ nApplyIndex ].s_nB + (nSkillLevel - 1)*pFTSkill->s_Apply[ nApplyIndex ].s_nIncrease_B_Point;
		}
	default:
		return pFTSkill->s_Apply[ nApplyIndex ].s_nB + (nSkillLevel - 1)*pFTSkill->s_Apply[ nApplyIndex ].s_nIncrease_B_Point;
	}

	return pFTSkill->s_Apply[ nApplyIndex ].s_nB + nSkillLevel*pFTSkill->s_Apply[ nApplyIndex ].s_nIncrease_B_Point;
}

bool FMCommon::GetSkillAttStr( int nA, int nLen, TCHAR* sz, int nB /*= 0*/ )
{	
	//스킬코드A 명칭
	switch( nA )
	{
	case 0:		
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_EVENT_ITEM" ).c_str() );
		return false;
	case APPLY_HP:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_HP" ).c_str() );
		return true;
	case APPLY_DS:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_DS" ).c_str() );
		return true;
	case APPLY_MAXHP:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_STAT_MAX_HP" ).c_str() );
		return true;
	case APPLY_MAXDS:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_STAT_MAX_DS" ).c_str() );
		return true;
	case APPLY_AP:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" ).c_str() );
		return true;
	case APPLY_CA:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_STAT_CRITICAL_HIT" ).c_str() );
		return true;
	case APPLY_DP:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_STAT_DEFENSE" ).c_str() );
		return true;
	case APPLY_EV:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_EVASION" ).c_str() );
		return true;
	case APPLY_MS:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_SPEED" ).c_str() );
		return true;
	case APPLY_AS:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_ATTACK_SPEED" ).c_str() );
		return true;
	case APPLY_AR:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_ATTACK_RANGE" ).c_str() );
		return true;
	case APPLY_HT:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_STAT_HIT_RATE" ).c_str() );
		return true;
	case APPLY_FP:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_TIRED_DECREASE" ).c_str() );
		return false;
	case APPLY_FS:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_FRIENDLINESS" ).c_str() );
		return true;	
	case APPLY_EXP:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_EXP" ).c_str() );
		return true;
	case APPLY_POWERAPPLYRATE:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_APPLY_ATTRIBUTE" ).c_str() );
		return true;
// 	case nTable_Parameter::APPLY_BL:
// 		_stprintf_s( sz, nLen, _LAN( "크기변경" ) );
// 		return true;
	case APPLY_BL:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_BLOCK" ).c_str() );
		return true;	
	case APPLY_AllParam:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_ALL_PARAM" ).c_str() );
		return true;
	case APPLY_ER:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_EVASION" ).c_str() );
		return true;
	case APPLY_HR:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_STAT_HIT_RATE" ).c_str() );
		return true;
	case APPLY_DA:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_DAMAGE_QUANTITY" ).c_str() );
		return true;
	case APPLY_HRR:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_HP_RECOVERY" ).c_str() );
		return true;
	case APPLY_DRR:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_DS_RECOVERY" ).c_str() );
		return true;
	case APPLY_MDA:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_NORMAL_DAMAGE_DECREASE" ).c_str() );
		return true;
	case APPLY_SER:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_SKILL_EVASION" ).c_str() );
		return true;
	case APPLY_SDR:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_SKILL_DAMAGE_DECREASE" ).c_str() );
		return true;
	case APPLY_SRR:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_SKILL_ACCURACY_RATE" ).c_str() );
		return true;
	case APPLY_SCD:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_SKILL_DAMAGE" ).c_str() );
		return true;
	case APPLY_SCR:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_SKILL_CRITIAL_INCIDENCE" ).c_str() );
		return true;
	case APPLY_DSN:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_DS_NATURAL_RECOVERY" ).c_str() );
		return true;
	case APPLY_HPN:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_HP_NATURAL_RECOVERY" ).c_str() );
		return true;
	case APPLY_STA:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_CROWD_CONTROL" ).c_str() );	// 홍콩 대만 번역 이상함...
		return true;
	case APPLY_UB:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_UNBEATABLE" ).c_str() );
		return true;
	case APPLY_ATTRIBUTTE:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_ATTRIBUTE_EXP" ).c_str() );
		return true;
	case APPLY_CC:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_CODECROWN_GAUGE" ).c_str() );
		return true;
	case APPLY_CR:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_CROSSLOADER" ).c_str() );
		return true;
	case APPLY_DOT:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_DOT_DAMAGE" ).c_str() );
		return true;
	case APPLY_DOT2:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_DELAY_DAMAGE" ).c_str() );
		return true;
	case APPLY_STUN:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_STUN" ).c_str() );
		return true;
	case APPLY_DR:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_DAMAGE_REFLECT" ).c_str() );
		return true;
	case APPLY_AB:
		_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_DAMAGE_ABSORB" ).c_str() );
		return true;

	case APPLY_ATDMG:
		switch( nB )
		{
		case nsCsDigimonTable::NT_FIRE/*18*/:		// 불
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_FIRE_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		case nsCsDigimonTable::NT_ICE/*16*/:		// 얼음
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_ICE_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		case nsCsDigimonTable::NT_WATER/*17*/:		// 물
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_WATER_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		case nsCsDigimonTable::NT_EARTH/*19*/:		// 땅
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_EARTH_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		case nsCsDigimonTable::NT_WIND/*20*/:		// 바람
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_WIND_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		case nsCsDigimonTable::NT_WOOD/*21*/:		// 나무
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_WOOD_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		case nsCsDigimonTable::NT_LIGHT/*22*/:		// 빛
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_LIGHT_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		case nsCsDigimonTable::NT_DARKNESS/*23*/:	// 어둠
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_DARKNESS_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		case nsCsDigimonTable::NT_THUNDER/*24*/:	// 전기
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_THUNDER_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		case nsCsDigimonTable::NT_STEEL/*25*/:		// 강철
			_stprintf_s( sz, nLen, UISTRING_TEXT( "COMMON_TXT_STEEL_ATTRIBUTE_DAMAGE" ).c_str() );
			break;
		}
		return true;

	case APPLY_WEAKNESS:		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_WEAKNESS" ).c_str() );	return true;// 쇠약 = 받는 피해량
	case APPLY_ATTRIBUTEDAMAGE:	_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_ATTRIBUTE_DAMAGE" ).c_str() );	return true;// 속성 피해량	
	case APPLY_ATTRIBUTEDR:		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_ATTRIBUTE_DR" ).c_str() );	return true;// 속성 반사 = 속성 피해량 반사
	case APPLY_ATTRIBUTEDS:		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_ATTRIBUTE_DS" ).c_str() );	return true;// 속성 흡수 = 속성 피해량 흡수
	case APPLY_SILENCE:			_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_SILENCE" ).c_str() );return true;// 침묵 = 스킬 사용 불가
	case APPLY_CAT:				_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_CRITICAL_DAMAGE" ).c_str() );	return true;

	case APPLY_AllSkillDamageDecrease:		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_PROTECTION_SKILL_DAMAGE" ).c_str() );	return true;//모든 스킬데미지 감소
	case APPLY_SpecificSkillDamageDecrease: _stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_PROTECTION_PARTIAL_SKILL" ).c_str() );	return true;//특정 스킬데미지 감소
	case APPLY_SpecificSkillDamageIncrease: _stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_INCREMENT_PARTIAL_SKILL" ).c_str() );	return true;//특정 스킬데미지 증가
	case APPLY_RDD:							_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_DAMAGE_DECREASE" ).c_str() );	return true;//데미지 감소
	case GUILD_CROPBITUP:					_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_ADDITIONAL_ACQUISITION_MONEY" ).c_str() );	return true;//사냥시 돈 획득 증가
	case APPLY_NatureShield:				_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_NATURE_SHIELD" ).c_str() );	return true;//특정 속성 데미지 감소
	case APPLY_XGuage:						_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_XGUAGE" ).c_str() );	return true;// X Guage증가
	case APPLY_XCrystal:					_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_XCRYSTAL" ).c_str() );	return true;// 크리스탈 증가
	case APPLY_RegionBuffDummy:				_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_NEUTRALIZE" ).c_str() );return true;// 지역 버프 방어 증가
	case APPLY_EvoTypeDamageIncrease:		_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_EVOLUTION_DAMAGE_INCREASE" ).c_str() );return true;// 지역 버프 방어 증가
	case APPLY_NatureTypeDamageIncrease:	_stprintf_s( sz, nLen, UISTRING_TEXT( "FMCOMMON_ATTRIBUTE_DAMAGE_INCREASE" ).c_str() );return true;// 지역 버프 방어 증가
	case APPLY_FixMoveSpeed:				_stprintf_s( sz, nLen, UISTRING_TEXT( "BUFF_APPLY_FixMoveSpeed" ).c_str() );return true;// 이동속도 보정 문자열
	default:
		assert_csm( false, L"정의 되지 않은 스탯값임 pParameter참조!!" )
		break;
	}

	_stprintf_s( sz, nLen, _T( " " ) );
	return false;
}

bool FMCommon::GetStrApply_ID( TCHAR* sz, int nStrLen, int nID, bool bMinus )
{
	switch( nID )
	{
#ifdef CROSSWARS_SYSTEM
	case nSkill::Me_104:
#endif
	case nSkill::Me_101:	// 수치 B만큼 회복
		{
			if( bMinus == false )
				_tcscat_s( sz, nStrLen, UISTRING_TEXT( "COMMON_TXT_RISE" ).c_str() );
			else
				_tcscat_s( sz, nStrLen, UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );
		}
		return true;
	case nSkill::Me_102:
	case nSkill::Me_105:	
	case nSkill::Me_106:
	case nSkill::Me_403:
		{
			if( bMinus == false )
			{
				std::wstring wsIncrease;
				DmCS::StringFn::Format( wsIncrease, L"%% %s", UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );
				_tcscat_s( sz, nStrLen, wsIncrease.c_str() );
			}
			else
			{
				std::wstring wsDecrease;
				DmCS::StringFn::Format( wsDecrease, L"%% %s", UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );
				_tcscat_s( sz, nStrLen, wsDecrease.c_str() );
			}
		}
		return true;
#ifdef DEBUFF_ITEM
	case nSkill::Me_301:
		{
			if( bMinus == false )
				_tcscat_s( sz, nStrLen, UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );
			else
				_tcscat_s( sz, nStrLen, UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );
		}
		return true;


	case nSkill::Me_302:
	case nSkill::Me_404:
		{
			if( bMinus == false )
			{
				std::wstring wsDecrease;
				DmCS::StringFn::Format( wsDecrease, L"%% %s", UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );
				_tcscat_s( sz, nStrLen, wsDecrease.c_str() );
			}
			else
			{
				std::wstring wsIncrease;
				DmCS::StringFn::Format( wsIncrease, L"%% %s", UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );
				_tcscat_s( sz, nStrLen, wsIncrease.c_str() );
			}
		}
		return true;
#endif
	case 303:
		{
			std::wstring wsApplied;
			DmCS::StringFn::Format( wsApplied, L"%% %s", UISTRING_TEXT( "COMMON_TXT_APPLIED" ).c_str() );
			_tcscat_s( sz, nStrLen, wsApplied.c_str() );
		}
		return true;
	case 206/*nSkill::Me_206*/:
		{
			std::wstring wsIncrease;
			DmCS::StringFn::Format( wsIncrease, L"%% %s", UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );
			_tcscat_s( sz, nStrLen, wsIncrease.c_str() );
		}
		return true;
	case 207/*nSkill::Me_207*/:
		{
			std::wstring wsIncrease;
			DmCS::StringFn::Format( wsIncrease, L"%% %s", UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );
			_tcscat_s( sz, nStrLen, wsIncrease.c_str() );
		}
		return true;
	case 215/*nSkill::Me_215*/:	// 무적

	case 208:	// 지속 시간 증가 연산
	case nSkill::Me_401:	// 이벤트 동기화 // 폭죽등
	case nSkill::Me_402:	// 디지몬 스케일 임시 변경 // 수수께끼 성장열매 같은거
	case nSkill::Me_002:
	case nSkill::Me_107:
	case 200/*nSkill::Me_200*/:
	case nSkill::Me_205:
		return false;	
	default:		assert_cs( false );
	}
	return false;
}

