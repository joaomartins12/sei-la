

#include "stdafx.h"
#include "ToolTip.h"

void cTooltip::_MakeTooltip_Skill()
{
	// MainID = 스킬 인덱스
	// SubID_1 = 소유 아이디 - 0:테이머, 1~:소유 디지몬 인덱스 ( 1 기반 )
	// SubID_2 = 현재 모델 ID - 진화나 퇴화시 툴팁을 바꾸기 위한 체크
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTUser );
	cSkill* pSkillMng = NULL;
	CsSkill* pFTSkill = NULL;
	CDigimonUser* pDigimonUser = NULL;
	if( m_nSubID_1 == 0 )
	{
		pSkillMng = pTUser->GetSkillMng();
		SAFE_POINTER_RET( pSkillMng );
		pFTSkill = pSkillMng->GetFTSkill( static_cast<int>(m_nMainID) );
	}
	else
	{		
		pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
		pSkillMng = pDigimonUser->GetSkillMng();
		
		CDigimonEvolveInfo* pFTEvolInfo = g_pCharMng->GetDigimonUser( 0 )->GetFTEvol();
		DWORD dwDigimonId = pFTEvolInfo->GetEvolveObjByEvoSlot( static_cast<int>(m_nSubID_1) )->m_dwID;
		
		pFTSkill = pSkillMng->GetFTSkill( dwDigimonId, static_cast<int>(m_nMainID) );	

		if( pFTSkill == NULL )	// 캐쉬 스킬이기 때문에 Base 스킬 검색으로는 스킬 정보가 나오지 않아 추가 검색.
		{
			cSkill::sINFO* pInfo = pSkillMng->GetSkill( static_cast<int>(m_nMainID) );

			if( pInfo )
				pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( pInfo->s_pFTSkill->GetInfo()->s_dwID );
		}
	}	
	
	if( pFTSkill == NULL )
		return;
	CsSkill::sINFO* pFTInfo = pFTSkill->GetInfo();

	cString* pString;
	TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ]={0};

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13 );

	//=================================================================================================================
	//
	//		타이틀
	//
	//=================================================================================================================
	// 이름
	ti.s_eFontSize = CFont::FS_13;
	ti.s_Color = NiColor( 1, 1, 0 );

	cSkill::sINFO* pTSkill = pSkillMng->GetSkill( static_cast<int>(m_nMainID) );
	SAFE_POINTER_RET( pTSkill );

	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RET( pTactics );

	switch( pFTInfo->s_nIcon )
	{
	case CsSkill::IT_CHANGE1:
	case CsSkill::IT_CHANGE2:
	case CsSkill::IT_CHANGE3:
		{
			pString = _AddIcon( ICONITEM::SKILL_MASK, pFTInfo->s_nIcon );
			cData_PostLoad::sDATA* pTacticsData = pTactics->GetTactics( pTSkill->s_nTacticsIndex );
			if( pTacticsData )
			{
				if( _tcslen( pTacticsData->s_szName ) > 0 )		
					_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%s (%s)" ), pFTInfo->s_szName, pTacticsData->s_szName );
				else
					_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%s" ), pFTInfo->s_szName );	
				ti.SetText( sz );
				pString->AddText( &ti );
			}
		}
		break;
	default:		
		{
			if( pFTInfo->s_nIcon >= 4000 )
				pString = _AddIcon( ICONITEM::SKILL4, pFTInfo->s_nIcon );
			else if( pFTInfo->s_nIcon >= 3000 )
				pString = _AddIcon( ICONITEM::SKILL3, pFTInfo->s_nIcon );
			else if( pFTInfo->s_nIcon >= 2000 )
				pString = _AddIcon( ICONITEM::SKILL2, pFTInfo->s_nIcon );
			else
				pString = _AddIcon( ICONITEM::SKILL1, pFTInfo->s_nIcon );
			ti.SetText( pFTInfo->s_szName );
			pString->AddText( &ti );
		}
		break;
	}	
	m_StringList.AddTail( pString );

	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;

	// 스킬 레벨
	int nBaseSkillCnt;
	if( pTactics->_GetOpenSlot() >= 3 )	// 3 마리 이상 오픈 중 이면
		nBaseSkillCnt = pTactics->_GetOpenSlot() + 5;	// 디지몬 교체스킬 3번 추가로 8개
	else
		nBaseSkillCnt= 7;	// 디지몬 교체스킬 3번 없으니 7개
	if( pFTInfo->s_nMaxLevel > 0 && m_nMainID < nBaseSkillCnt )		// m_nMainID 가 7이상이면 Active Skill 또는 Pasive Skill 임.	기브 버전에선 동작 Emotion 카운트 안함.
	{
		_AddLine( false );

		pString = NiNew cString;		
		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_LV" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color = FONT_WHITE;
		if( m_nSubID_1 == 0 )
		{		
			_stprintf_s( sz, sizeof(sz), _T( "1 / %d" ), pFTInfo->s_nMaxLevel );
		}
		else if(pFTInfo->s_nMaxLevel==1)	// MaxLevel이 1인경우 디폴트 값 1과 같으므로 그대로 표시해준다.	//	스킬 메모리 예외처리.
		{
			_stprintf_s( sz, sizeof(sz), _T( "1 / 1" ) );
		}
		else
		{
			cEvoUnit* pEvoUnit = pDigimonUser->GetAttributeEvoUnit( static_cast<int>(m_nSubID_1) );
#ifdef SDM_DIGIMONSKILL_LV_EXPEND_20181206
			_stprintf_s( sz, sizeof(sz), _T( "%d / %d" ), pEvoUnit->m_nSkillLevel[ m_nMainID ], pEvoUnit->m_nSkillMaxLevel[ m_nMainID ] );
#else
			_stprintf_s( sz, sizeof(sz), _T( "%d / %d" ), pEvoUnit->m_nSkillLevel[ m_nMainID ], pFTInfo->s_nMaxLevel );
#endif
		}

		ti.SetText( sz );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}

	// 소모 DS
	if( pFTInfo->s_nMaxLevel > 0 && m_nMainID < nBaseSkillCnt)		// m_nMainID 가 7이상이면 Active Skill 또는 Pasive Skill 임.	기브 버전에선 동작 Emotion 카운트 안함.
	{
		pString = NiNew cString;
		ti.s_Color = FONT_GOLD;
		int nValue;
		if( pFTInfo->s_nUseDS )
		{		
			ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_DS_CONSUMED" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
			nValue = pFTInfo->s_nUseDS;
		}
		else
		{
			assert_cs( pFTInfo->s_nUseHP );
			if( m_nSubID_1 == 0 )
				ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_SP_CONSUMED" ).c_str() );
			else
				ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_HP_CONSUMED" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
			nValue = pFTInfo->s_nUseHP;
		}	
		ti.s_Color = FONT_WHITE;
		ti.SetText( nValue );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}

	// 스킬 속성
	if( pFTInfo->s_nMaxLevel > 0 && m_nMainID < nBaseSkillCnt && pFTInfo->s_nNatureType )		// m_nMainID 가 7이상이면 Active Skill 또는 Pasive Skill 임.	기브 버전에선 동작 Emotion 카운트 안함.
	{
		pString = NiNew cString;
		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_ATTRIBUTE" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, pFTInfo->s_nNatureType-nsCsDigimonTable::NT_ICE, CsPoint( 0, -2 ) );

		ti.s_Color = NiColor( 1, 1, 0 );
		GetNatureString( sz, TOOLTIP_MAX_TEXT_LEN, pFTInfo->s_nNatureType );
		ti.SetText( sz );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}

	// 스킬 포인트
	if( pFTInfo->s_nMaxLevel > 0 && m_nMainID < nBaseSkillCnt)		// m_nMainID 가 7이상이면 Active Skill 또는 Pasive Skill 임.	기브 버전에선 동작 Emotion 카운트 안함.
	{
		pString = NiNew cString;
		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_REQUIRE_SKILL_POINT" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

		ti.s_Color = FONT_WHITE;		
		ti.SetText( pFTInfo->s_nLevelupPoint );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}

	if( pFTInfo->s_nMemorySkill )
	{
		pString = NiNew cString;
		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_NEED_MEMORY_CHIP" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

		ti.s_Color = FONT_WHITE;

		std::wstring wsItem;
		DmCS::StringFn::Format( wsItem, L"%d %s", pFTInfo->s_nReq_Item, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );

		ti.SetText( wsItem.c_str() );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}


	if( pFTSkill->IsActive() || pFTSkill->IsPasive() || pFTSkill->GetInfo()->s_nMemorySkill )
	{
		
		// 현재 스킬
		if( m_nMainID < nBaseSkillCnt )		// m_nMainID 가 7이상이면 Active Skill 또는 Pasive Skill 임.	기브 버전에선 동작 Emotion 카운트 안함.
			_AddSkillState( pFTInfo );
			
		// 쿨타임
		if( pFTSkill->IsActive() || pFTSkill->GetInfo()->s_nMemorySkill )
		{
			_AddLine( true );
			pString = NiNew cString;
			//ti.s_Color = FONT_GOLD;
			ti.s_Color = FONT_WHITE;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_COOL_TIME" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_Color = FONT_WHITE;
			int nCooldown = (int)( pFTInfo->s_fCooldownTime*0.001f );
			int nHour = nCooldown/3600;
			nCooldown = nCooldown%3600;
			int nMin = nCooldown/60;			
			int nSec = nCooldown%60;

			std::wstring wsTime;
			if( nHour != 0 )
			{
				std::wstring wsHour;
				DmCS::StringFn::Format( wsHour, L"%d %s ", nHour, UISTRING_TEXT( "COMMON_TXT_HOUR" ).c_str() );
				wsTime += wsHour;
			}
			if( nMin != 0 )
			{
				std::wstring wsMin;
				DmCS::StringFn::Format( wsMin, L"%d %s ", nMin, UISTRING_TEXT( "COMMON_TXT_MINUTE" ).c_str() );
				wsTime += wsMin;
			}
			if( nSec != 0 )
			{
				std::wstring wsSec;
				DmCS::StringFn::Format( wsSec, L"%d %s ", nSec, UISTRING_TEXT( "COMMON_TXT_SECOND" ).c_str() );
				wsTime += wsSec;
			}
			ti.SetText( wsTime.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}

		if( pTSkill->s_nLifeTime != 0 )
		{
			pString = NiNew cString;
			ti.s_Color = FONT_GOLD;	
			ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_EXPIRY_TIME" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_Color = FONT_WHITE;
			ti.SetText( _T( " " ) );
			sUPDATE_TIME ut;
			ut.s_pText = pString->AddText( &ti );
			ut.s_nTimeTS = pTSkill->s_nLifeTime;
			m_vpEditElement.PushBack( ut );

			m_StringList.AddTail( pString );
		}

		if( m_nSubID_3 == 1000 )	// 1000 이면 기본 스킬에 등록되어 있으나 레벨 부족으로 아직 배우지 못한 digimon skill의 툴팁을 표시
		{
			pString = NiNew cString;
			ti.s_Color = FONT_RED;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_OBTAINED_LEVEL" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_Color = FONT_RED;		
			ti.SetText( pFTInfo->s_nLimitLevel );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
	}

	// 내용
	_AddLine( true );

	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color = FONT_WHITE;

	std::wstring wsComment = pFTInfo->s_szComment;
	switch( pFTInfo->s_nIcon )
	{
	case CsSkill::IT_CHANGE1:
	case CsSkill::IT_CHANGE2:
	case CsSkill::IT_CHANGE3:
		DmCS::StringFn::ReplaceAll( wsComment, L"#Number#", pTSkill->s_nTacticsIndex + 1 );
		break;
	default:
		break;
	}

// 	TCHAR szComment[ ITEM_COMMENT_LEN ];
// 	_tcscpy_s( szComment, ITEM_COMMENT_LEN, pFTInfo->s_szComment );	

	int nCutSize;
#ifdef VERSION_USA
	nCutSize = 330;
#else
	nCutSize = TOOLTIP_CUT_SIZE;
#endif

	g_pStringAnalysis->Cut_Parcing( &m_StringList, nCutSize, wsComment.c_str()/*szComment*/, &ti );	

	// 기타

	switch( m_nSubID_2 )
	{
	case cBaseWindow::WT_NEW_TAMERSTATUS:
	case cBaseWindow::WT_NEW_DIGIMONSTATUS:
		{
			_AddLine( true );
			ti.s_eFontSize = CFont::FS_10;
			if( pFTSkill->IsPasive() )
			{
				ti.s_Color = TOOLTIP_CANNOT_COLOR;
				g_pStringAnalysis->Cut( &m_StringList, nCutSize, UISTRING_TEXT( "TOOLTIP_SKILL_SHORTCUT_NOT_REGIST" ).c_str(), &ti );
			}
			else
			{
				ti.s_Color = NiColor( 0, 1, 0 );
				g_pStringAnalysis->Cut( &m_StringList, nCutSize, UISTRING_TEXT( "TOOLTIP_SKILL_SHORTCUT_REGIST" ).c_str(), &ti );
			}
		}
		break;
	}	
}


void cTooltip::_AddSkillState( CsSkill::sINFO* pFTInfo )
{
	if( pFTInfo->s_nMemorySkill )
		return;
	_AddLine( true );

	SAFE_POINTER_RET( g_pCharMng );
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser(0);
	SAFE_POINTER_RET( pDUser );

	TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ] = {0, };
	cString* pString = NULL;
	int nCurSkillLevel = 1;
	if( m_nSubID_1 != 0 )
	{
		//nCurSkillLevel = g_pCharMng->GetDigimonUser( m_nSubID_1 - 1 )->GetCurAttributeEvoUnit()->m_nSkillLevel[ m_nMainID ];
		nCurSkillLevel = pDUser->GetAttributeEvoUnit( static_cast<int>(m_nSubID_1) )->m_nSkillLevel[ m_nMainID ];
	}

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, TOOLTIP_FONT_1_SIZE );

	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		if( pFTInfo->s_Apply[ i ].s_nID == 0 )
			continue;

		ti.s_Color = FONT_WHITE;

		pString = NiNew cString;

		int nSkillAtt = FMCommon::GetSkillAtt( pFTInfo->s_dwID, nCurSkillLevel, i );

		int nSubStr = 0;

		switch( pFTInfo->s_Apply[ i ].s_nA )
		{
		// HP/DS 회복스킬(스킬툴팁 추가)
		case APPLY_HP:
			{
				if(  pFTInfo->s_Apply[ i ].s_nID == nSkill::Me_002 )//디지몬 공격 스킬 효과 일때만
				{
					ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" ).c_str() );
				}
				else
				{
					ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_SKILL_EFFECT" ).c_str() );
					nSubStr = 1;
				}
			}
			break;
		case APPLY_DS:
			{
				ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_SKILL_EFFECT" ).c_str() );
				nSubStr = 1;
			}
			break;
		case APPLY_AP:
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" ).c_str() );
			break;			
		case APPLY_DP:
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_DEFENSE" ).c_str() );
			break;	
		case APPLY_DOT:		// 디버프 지속 데미지
		case APPLY_DOT2:	// 디버프 지연 데미지
		case APPLY_STUN:	// 스턴 
		case APPLY_SCD:		// 스킬 추가 효과
		case APPLY_UB:		// 무적 기능
		// 데미지 흡수/반사 버프 스킬 추가_14.04.24		chu8820
		case APPLY_DR:		// 데미지 반사 버프
		case APPLY_AB:		// 데미지 흡수 버프
		// 크리티컬 / 회피 증가 스킬 추가 14.05.28 chu8820
		case APPLY_CA:		// 크리티컬 증가 버프
		case APPLY_EV:		// 회피 증가 버프
			ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_SKILL_EFFECT" ).c_str() );
			break;
		case APPLY_MAXHP:
			break;
		case APPLY_CAT:			// 크리티컬 공격력
			ti.SetText( UISTRING_TEXT("SKILL_APPLY_TYPE_NAME_CAT").c_str() );
			break;
		case APPLY_RDD:			// 받는 데미지 감소
			ti.SetText( UISTRING_TEXT("SKILL_APPLY_TYPE_NAME_RDD").c_str() );
			break;
		default:
			assert_cs( false );
			break;
		}
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;


		switch( pFTInfo->s_Apply[ i ].s_nID )
		{
		case nSkill::Me_002:
			{	
				CDigimon::sENCHENT_STAT* pEStat = pDUser->GetEnchantStat();
				if( pEStat->GetEnchantLv( ET_AT ) > 0 )
				{
					ti.s_Color = FONT_GREEN;
					ti.SetText( ( (int)(pEStat->GetEnchantValue( ET_AT ) / 3.3f + 100 ) * nSkillAtt ) / 100 );
					pString->AddText( &ti )->s_ptSize.x += 5;

					pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::Stat, 8, 0, CsPoint( 0, -2 ) );

					ti.SetText( ( (int)(pEStat->GetEnchantValue( ET_AT ) / 3.3f ) * nSkillAtt ) / 100 );
					pString->AddText( &ti );					
				}								
				else
				{
					ti.SetText( nSkillAtt );
					pString->AddText( &ti );
				}			

#ifdef TOOLTIP_SKILL_ADD_BASEATT
				if( 0 != m_nSubID_1 )
				{
					CDigimonUser::sBASE_STAT* pStat = pDUser->GetBaseStat();
					if( pStat )
					{
						std::wstring wsAtt;
						DmCS::StringFn::Format( wsAtt, L" + %d", pStat->GetAtt() );
						ti.SetText( wsAtt.c_str() );
						pString->AddText( &ti );
					}
				}
#endif

				if( g_pDataMng->GetTEquip()->IsExistItem( nTamer::Ring ) ) // 반지 장비 중 일때
				{
					cItemInfo* pEquipItem = g_pDataMng->GetTEquip()->GetData( nTamer::Ring );
					//해당 반지의 디지몬 레벨 제한 확인
					CsItem::sINFO* pItemInfo = nsCsFileTable::g_pItemMng->GetItem( pEquipItem->GetType() )->GetInfo();
					int nDLevel = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();	//디지몬 레벨
					int nTLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();		//테이머 레벨
					//디지몬,테이머 레벨이 제한 레벨 이상일 때만
					if( pItemInfo->IsEnableTamerLv( nTLevel ) && pItemInfo->IsEnableDigimonLv( nDLevel ) )
					{
						float nVal = 0;

						for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION ; i++ )
						{
							if( pEquipItem->m_nAccOption[ i ] == 0 )
								break;
							if( pEquipItem->m_nAccOption[ i ] == nItem::SkillAP ) //스킬데미지 값만 더해주자
								nVal += (float)pEquipItem->m_nAccValues[ i ];
						}

						if( nVal != 0)//스킬 데미지 추가 옵션이 있으면 0이 아니겠지
						{
							nVal = nVal * (float)pEquipItem->m_nRate * 0.01f; // 디지터리 값 곱해서 실제 추가 값 구해
							_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( " + %d" ), (int)nVal );
							ti.SetText( sz );
							pString->AddText( &ti );
						}
					}
				}

				if( g_pDataMng->GetTEquip()->IsExistItem( nTamer::Necklace ) ) // 목걸이 장비 중 일때
				{
					cItemInfo* pEquipItem = g_pDataMng->GetTEquip()->GetData( nTamer::Necklace );
					//해당 목걸이의 디지몬 레벨 제한 확인
					CsItem::sINFO* pItemInfo = nsCsFileTable::g_pItemMng->GetItem( pEquipItem->GetType() )->GetInfo();
					int nDLevel = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();		//디지몬 레벨
					int nTLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();		//테이머 레벨
					//디지몬,테이머 레벨이 제한 레벨 이상일 때만
					if( pItemInfo->IsEnableTamerLv( nTLevel ) && pItemInfo->IsEnableDigimonLv( nDLevel ) )
					{
						float nVal = 0;

						for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION ; i++ )
						{
							if( pEquipItem->m_nAccOption[ i ] == 0 )
								break;
							if( pEquipItem->m_nAccOption[ i ] == nItem::SkillAP ) //스킬데미지 값만 더해주자
								nVal += (float)pEquipItem->m_nAccValues[ i ];
						}

						if( nVal != 0)//스킬 데미지 추가 옵션이 있으면 0이 아니겠지
						{
							nVal = nVal * (float)pEquipItem->m_nRate * 0.01f; // 디지터리 값 곱해서 실제 추가 값 구해
							_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( " + %d" ), (int)nVal );
							ti.SetText( sz );
							pString->AddText( &ti );
						}
					}
				}

				if( g_pDataMng->GetTEquip()->IsExistItem( nTamer::Earring ) ) // 귀걸이 장비 중 일때
				{
					cItemInfo* pEquipItem = g_pDataMng->GetTEquip()->GetData( nTamer::Earring );
					//해당 귀걸이 디지몬 레벨 제한 확인
					CsItem::sINFO* pItemInfo = nsCsFileTable::g_pItemMng->GetItem( pEquipItem->GetType() )->GetInfo();
					int nDLevel = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();		//디지몬 레벨
					int nTLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();		//테이머 레벨
					//디지몬,테이머 레벨이 제한 레벨 이상일 때만
					if( pItemInfo->IsEnableTamerLv( nTLevel ) && pItemInfo->IsEnableDigimonLv( nDLevel ) )
					{
						float nVal = 0;

						for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION ; i++ )
						{
							if( pEquipItem->m_nAccOption[ i ] == 0 )
								break;
							if( pEquipItem->m_nAccOption[ i ] == nItem::SkillAP ) //스킬데미지 값만 더해주자
								nVal += (float)pEquipItem->m_nAccValues[ i ];
						}

						if( nVal != 0)//스킬 데미지 추가 옵션이 있으면 0이 아니겠지
						{
							nVal = nVal * (float)pEquipItem->m_nRate * 0.01f; // 디지터리 값 곱해서 실제 추가 값 구해
							_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( " + %d" ), (int)nVal );
							ti.SetText( sz );
							pString->AddText( &ti );
						}
					}
				}
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
				if( g_pDataMng->GetTEquip()->IsExistItem( nTamer::Bracelet ) ) // 팔찌 장비 중 일때
				{
					cItemInfo* pEquipItem = g_pDataMng->GetTEquip()->GetData( nTamer::Bracelet );
					//해당 귀걸이 디지몬 레벨 제한 확인
					CsItem::sINFO* pItemInfo = nsCsFileTable::g_pItemMng->GetItem( pEquipItem->GetType() )->GetInfo();
					int nDLevel = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();		//디지몬 레벨
					int nTLevel = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();		//테이머 레벨
					//디지몬,테이머 레벨이 제한 레벨 이상일 때만
					if( pItemInfo->IsEnableTamerLv( nTLevel ) && pItemInfo->IsEnableDigimonLv( nDLevel ) )
					{
						float nVal = 0;

						for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION ; i++ )
						{
							if( pEquipItem->m_nAccOption[ i ] == 0 )
								break;
							if( pEquipItem->m_nAccOption[ i ] == nItem::SkillAP ) //스킬데미지 값만 더해주자
								nVal += (float)pEquipItem->m_nAccValues[ i ];
						}

						if( nVal != 0)//스킬 데미지 추가 옵션이 있으면 0이 아니겠지
						{
							nVal = nVal * (float)pEquipItem->m_nRate * 0.01f; // 디지터리 값 곱해서 실제 추가 값 구해
							_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( " + %d" ), (int)nVal );
							ti.SetText( sz );
							pString->AddText( &ti );
						}
					}
				}
#endif
			}
			break;
		case nSkill::Me_101:	// 수치 B만큼 회복
			{
				switch( nSubStr )
				{
				case 0:		_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d" ), nSkillAtt  );		break;
				case 1:
					{
						std::wstring wsRecovery = UISTRING_TEXT( "TOOLTIP_SKILL_RECOVERY_VALUE" );
						DmCS::StringFn::Replace( wsRecovery, L"#Value#", nSkillAtt );
						_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, wsRecovery.c_str() );
					}
					break;
				default:	assert_cs( false );
				}
				ti.SetText( sz );
				pString->AddText( &ti );
			}
			break;
		case nSkill::Me_102:
			{
				std::wstring wsIncrease;
				DmCS::StringFn::Format( wsIncrease, L"%d %%%s", nSkillAtt, UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );
				ti.SetText( wsIncrease.c_str() );
				pString->AddText( &ti );
			}
			break;
		case nSkill::Me_103:
			{
				std::wstring wsDecrease;
				DmCS::StringFn::Format( wsDecrease, L"%d %%%s", nSkillAtt, UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );
				ti.SetText( wsDecrease.c_str() );
				pString->AddText( &ti );
			}
			break;			

		case nSkill::Me_205:	// % 확률로 발동하는 도트 데미지
		case 206/*nSkill::Me_206*/:	// % 확률로 발동한 버프, 스킬 사용 시 B값 추가
		case 207/*nSkill::Me_207*/:	// % 확률로 발동한 버프, 스킬 사용 시 + B% 추가
		case 208/*nSkill::Me_208*/:	// % 스킬 레벨에 따른 지속 시간 증가 연산 - 무적 / 행동불능 연산 방식
			{
				//ACTIVE_SKILL - HP/DS 회복스킬(스킬툴팁 추가)

				// 특수스킬 공식 수정으로 재계산
// 				nCurSkillLevel -= 1;
// 				nSkillAtt = FMCommon::GetSkillAtt( pFTInfo->s_dwID, nCurSkillLevel, i );

				//확률값 있을 땐 확률 표시
				if( pFTInfo->s_Apply[i].s_nInvoke_Rate != 0 )
				{
					std::wstring wsProb = UISTRING_TEXT( "TOOLTIP_ACTIVATION_PROBABILITY" );
					DmCS::StringFn::Replace( wsProb, L"#Prob#", (pFTInfo->s_Apply[i].s_nInvoke_Rate / 100) );
					ti.SetText( sz );
					pString->AddText( &ti );
				}

				TCHAR szVal[TOOLTIP_MAX_TEXT_LEN] = {0};

				switch( pFTInfo->s_Apply[i].s_nA )
				{
				case APPLY_HP:
				case APPLY_DS:
					{
						if( pFTInfo->s_Apply[i].s_nA == APPLY_HP )
							ti.SetText( UISTRING_TEXT( "COMMON_TXT_HP" ).c_str() );
						else if( pFTInfo->s_Apply[i].s_nA == APPLY_DS )
							ti.SetText( UISTRING_TEXT( "COMMON_TXT_DS" ).c_str() );

						pString->AddText( &ti );
						pString->TailAddSizeX( 2 );

						if( pFTInfo->s_Apply[ i ].s_nID == 207 )//+ B% 
							_stprintf_s( szVal, TOOLTIP_MAX_TEXT_LEN, _T( "%d%% " ), nSkillAtt  );
						else // + B
							_stprintf_s( szVal, TOOLTIP_MAX_TEXT_LEN, _T( "%d " ), nSkillAtt  );

						switch( pFTInfo->s_Apply[ i ].s_nBuffCode )//기본효과(디지몬 공격 스킬)가 아니라면 s_nBuffCode 는 효과적용 타겟
						{
						case nTarget::MY_ALL:
						case nTarget::MY_PALL:
						case nTarget::TAMER:
						case nTarget::DIGIMON:		
							{
								_tcscat_s( szVal, TOOLTIP_MAX_TEXT_LEN, UISTRING_TEXT( "TOOLTIP_SKILL_RECOVERY" ).c_str() );
							}
							break;
						default:	
							{
								_tcscat_s( szVal, TOOLTIP_MAX_TEXT_LEN, UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );
							}
							break;
						}
						//_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szVal );
						ti.SetText( szVal );
						pString->AddText( &ti );
					}
					break;
				// 크리티컬 / 회피 증가 스킬 추가 14.05.28 chu8820
				case APPLY_CA:
				case APPLY_EV:
					{
						if( pFTInfo->s_Apply[i].s_nA == APPLY_CA )
							ti.SetText( UISTRING_TEXT( "COMMON_TXT_CRITICAL" ).c_str() );
						else if( pFTInfo->s_Apply[i].s_nA == APPLY_EV )
							ti.SetText( UISTRING_TEXT( "COMMON_TXT_AVOID" ).c_str() );

						pString->AddText( &ti );
						pString->TailAddSizeX( 2 );

						if( pFTInfo->s_Apply[ i ].s_nID == 207 )//+ B% 
							nSkillAtt *= 100;
						_stprintf_s( szVal, TOOLTIP_MAX_TEXT_LEN, _T( "%.2f%% " ), (float)nSkillAtt*0.01f  );
						switch( pFTInfo->s_Apply[ i ].s_nBuffCode )	// 효과적용 타겟
						{
						case nTarget::MY_ALL:
						case nTarget::MY_PALL:
						case nTarget::TAMER:
						case nTarget::DIGIMON:		
							{
								_tcscat_s( szVal, TOOLTIP_MAX_TEXT_LEN, UISTRING_TEXT( "COMMON_TXT_RISE" ).c_str() );
							}
							break;
						default:	
							{
								_tcscat_s( szVal, TOOLTIP_MAX_TEXT_LEN, UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );
							}
							break;
						}
						//_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szVal );
						ti.SetText( szVal );
						pString->AddText( &ti );
					}
					break;
				default:
					{
						CsBuff::MAP* BuffMap = nsCsFileTable::g_pBuffMng->GetBuffMap();
						CsBuff::MAP_IT iter = BuffMap->begin();
						CsBuff::MAP_IT iter_End = BuffMap->end();

						for(; iter != iter_End; ++iter)
						{
							CsBuff* pFTBuff = iter->second;
							SAFE_POINTER_CON( pFTBuff );
							CsBuff::sINFO* pFTBuffInfo = pFTBuff->GetInfo();
							SAFE_POINTER_CON( pFTBuffInfo );
							if( pFTInfo->s_dwID == pFTBuffInfo->s_dwSkillCode )
							{
								std::wstring wsEffect;
								DmCS::StringFn::Format( wsEffect, L"%s %s", pFTBuffInfo->s_szName, UISTRING_TEXT( "TOOLTIP_SKILL_BUFF_EFFECT" ).c_str() );
								ti.SetText( wsEffect.c_str() );
								pString->AddText( &ti );
								break;
							}
						}

					}
					break;
				}

 			}
			break;
		case nSkill::Me_106:
				break;
		default:
			assert_cs( false );
		}
		pString->CalMaxSize();
		m_StringList.AddTail( pString );
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


void cTooltip::_MakeTooltip_SimpleSKill()
{
	// MainID = 스킬 인덱스
	// SubID_1 = 소유 아이디 - 0:테이머, 1~:소유 디지몬 인덱스 ( 1 기반 )

	CsSkill* pFTSkill = nsCsFileTable::g_pSkillMng->GetSkill( m_nMainID );
	SAFE_POINTER_RET( pFTSkill );

	CsSkill::sINFO* pFTInfo = pFTSkill->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	//=================================================================================================================
	//
	//		타이틀
	//
	//=================================================================================================================
	{
		ICONITEM::eTYPE iConType = ICONITEM::SKILL1;
		switch( pFTInfo->s_nIcon )
		{
		case CsSkill::IT_CHANGE1:
		case CsSkill::IT_CHANGE2:
		case CsSkill::IT_CHANGE3:
			iConType = ICONITEM::SKILL_MASK;
			break;
		default:
			{
				if( pFTInfo->s_nIcon >= 4000 )	iConType = ICONITEM::SKILL4;
				else if( pFTInfo->s_nIcon >= 3000 )	iConType = ICONITEM::SKILL3;
				else if( pFTInfo->s_nIcon >= 2000 )	iConType = ICONITEM::SKILL2;
				else	iConType = ICONITEM::SKILL1;
			}
			break;
		}

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13, NiColor( 1, 1, 0 ) );
		ti.SetText( pFTInfo->s_szName );
		cString* pString = _AddIcon( iConType, pFTInfo->s_nIcon );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}

	//=================================================================================================================
	//
	//		스킬 레벨
	//
	//=================================================================================================================

	if( pFTInfo->s_nMaxLevel > 1 )
	{
		_AddLine( false );
		cString* pString = NiNew cString;	
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 1, 1, 0 ) );	
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_LV" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );

			std::wstring msg;
			DmCS::StringFn::Format( msg, L"%d / %d", pFTInfo->s_nMaxLevel, pFTInfo->s_nMaxLevel );
			ti.SetText( msg.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}		
	}

	//=================================================================================================================
	//
	//		소모 DS
	//
	//=================================================================================================================

	if( pFTInfo->s_nUseDS > 0 )
	{		
		cString* pString = NiNew cString;		
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_DS_CONSUMED" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			ti.SetText( pFTInfo->s_nUseDS );
			pString->AddText( &ti );
		}
		m_StringList.AddTail( pString );
	}

	//=================================================================================================================
	//
	//		소모 HP
	//
	//=================================================================================================================
	if( pFTInfo->s_nUseHP > 0 )
	{
		cString* pString = NiNew cString;		
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_HP_CONSUMED" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			ti.SetText( pFTInfo->s_nUseHP );
			pString->AddText( &ti );
		}
		m_StringList.AddTail( pString );
	}	


	//=================================================================================================================
	//
	//		스킬 속성
	//
	//=================================================================================================================

 	if( pFTInfo->s_nNatureType > 0 )
	{
		cString * pString = NiNew cString;
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_ATTRIBUTE" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, pFTInfo->s_nNatureType-nsCsDigimonTable::NT_ICE, CsPoint( 0, -2 ) );

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 1, 1, 0 ) );
			TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ]={0};
			GetNatureString( sz, TOOLTIP_MAX_TEXT_LEN, pFTInfo->s_nNatureType );
			ti.SetText( sz );
			pString->AddText( &ti );
		}

		m_StringList.AddTail( pString );
	}


	//=================================================================================================================
	//
	//		필요 메모리 칩
	//
	//=================================================================================================================
	if( pFTInfo->s_nMemorySkill > 0 && pFTInfo->s_nReq_Item > 0 )
	{
		cString* pString = NiNew cString;
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_NEED_MEMORY_CHIP" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			std::wstring msg;
			DmCS::StringFn::Format( msg, L"%d %s", pFTInfo->s_nReq_Item, UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );
			ti.SetText( msg.c_str() );
			pString->AddText( &ti );
		}

		m_StringList.AddTail( pString );
	}

	if( m_nSubID_1 == 1 ) // 디지몬 스킬일 경우에만
		_SimpleAddSkillState( pFTInfo, pFTInfo->s_nMaxLevel );

	//=================================================================================================================
	//
	//		재사용시간
	//
	//=================================================================================================================

	if( pFTSkill->IsActive() || pFTInfo->s_nMemorySkill )
	{
		_AddLine( true );

		cString* pString = NiNew cString;

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_COOL_TIME" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			int nCooldown = (int)( pFTInfo->s_fCooldownTime*0.001f );
			int nHour = nCooldown/3600;
			nCooldown = nCooldown%3600;
			int nMin = nCooldown/60;			
			int nSec = nCooldown%60;

			TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ]={0};
			TCHAR szTime[ 16 ];
			memset( sz, 0, sizeof( TCHAR )*TOOLTIP_MAX_TEXT_LEN );
			if( nHour != 0 )
			{
				std::wstring wsHour;
				DmCS::StringFn::Format( wsHour, L"%d %s", nHour, UISTRING_TEXT( "COMMON_TXT_HOUR" ).c_str() );
				_stprintf_s( szTime, 16, wsHour.c_str() );
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szTime ) ;
			}
			if( nMin != 0 )
			{
				std::wstring wsMin;
				DmCS::StringFn::Format( wsMin, L"%d %s", nMin, UISTRING_TEXT( "COMMON_TXT_MINUTE" ).c_str() );
				_stprintf_s( szTime, 16, wsMin.c_str() );
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szTime ) ;
			}
			if( nSec != 0 )
			{
				std::wstring wsSec;
				DmCS::StringFn::Format( wsSec, L"%d %s", nSec, UISTRING_TEXT( "COMMON_TXT_SECOND" ).c_str() );
				_stprintf_s( szTime, 16, wsSec.c_str() );
				_tcscat_s( sz, TOOLTIP_MAX_TEXT_LEN, szTime ) ;
			}
			ti.SetText( sz );
			pString->AddText( &ti );
		}

		
		m_StringList.AddTail( pString );
	}

	//=================================================================================================================
	//
	//		습득 레벨
	//
	//=================================================================================================================
	if( pFTInfo->s_nLimitLevel > 1 )
	{
		cString* pString = NiNew cString;
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_OBTAINED_LEVEL" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );				
			ti.SetText( pFTInfo->s_nLimitLevel );
			pString->AddText( &ti );
		}

		m_StringList.AddTail( pString );
	}

	//=================================================================================================================
	//
	//		스킬 설명 글
	//
	//=================================================================================================================
 	_AddLine( true );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );

	TCHAR szComment[ ITEM_COMMENT_LEN ];
	_tcscpy_s( szComment, ITEM_COMMENT_LEN, pFTInfo->s_szComment );	

#ifdef VERSION_USA
	int nCutSize = 330;
#else
	int nCutSize = TOOLTIP_CUT_SIZE;
#endif
	g_pStringAnalysis->Cut_Parcing( &m_StringList, nCutSize, szComment, &ti );		
}


void cTooltip::_SimpleAddSkillState( CsSkill::sINFO* pFTInfo, int const& nSkillLv )
{
	if( pFTInfo->s_nMemorySkill )
		return;

	_AddLine( true );

	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		if( pFTInfo->s_Apply[ i ].s_nID == 0 )
			continue;

		int nSkillAtt = FMCommon::GetSkillAtt( pFTInfo->s_dwID, nSkillLv, i );
		if( 0 == nSkillAtt )
			continue;

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, TOOLTIP_FONT_1_SIZE, FONT_WHITE );	

		cString* pString = NiNew cString;
		int nSubStr = 0;
		switch( pFTInfo->s_Apply[ i ].s_nA )
		{
		// HP/DS 회복스킬(스킬툴팁 추가)
		case APPLY_HP:
			{
				if(  pFTInfo->s_Apply[ i ].s_nID == nSkill::Me_002 )//디지몬 공격 스킬 효과 일때만
					ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" ).c_str() );
				else
				{
					ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_SKILL_EFFECT" ).c_str() );
					nSubStr = 1;
				}
			}
			break;
		case APPLY_DS:
			{
				ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_SKILL_EFFECT" ).c_str() );
				nSubStr = 1;
			}
			break;
		case APPLY_AP:
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" ).c_str() );
			break;			
		case APPLY_DP:
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_DEFENSE" ).c_str() );
			break;	
		case APPLY_DOT:		// 디버프 지속 데미지
		case APPLY_DOT2:	// 디버프 지연 데미지
		case APPLY_STUN:	// 스턴 
		case APPLY_SCD:		// 스킬 추가 효과
		case APPLY_UB:		// 무적 기능
			// 데미지 흡수/반사 버프 스킬 추가_14.04.24		chu8820
		case APPLY_DR:		// 데미지 반사 버프
		case APPLY_AB:		// 데미지 흡수 버프
			// 크리티컬 / 회피 증가 스킬 추가 14.05.28 chu8820
		case APPLY_CA:		// 크리티컬 증가 버프
		case APPLY_EV:		// 회피 증가 버프
			ti.SetText( UISTRING_TEXT( "TOOLTIP_SKILL_SKILL_EFFECT" ).c_str() );
			break;
		case APPLY_MAXHP:
			break;
		case APPLY_CAT:			// 크리티컬 공격력
			ti.SetText( UISTRING_TEXT("SKILL_APPLY_TYPE_NAME_CAT").c_str() );
			break;
		case APPLY_RDD:			// 받는 데미지 감소
			ti.SetText( UISTRING_TEXT("SKILL_APPLY_TYPE_NAME_RDD").c_str() );
			break;
		default:
			assert_cs( false );
			break;
		}
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;


		switch( pFTInfo->s_Apply[ i ].s_nID )
		{
		case nSkill::Me_002:
		case nSkill::Me_010:
			{	
				ti.SetText( nSkillAtt );
				pString->AddText( &ti );
			}
			break;
		case nSkill::Me_101:	// 수치 B만큼 회복
			{
				std::wstring msg;
				switch( nSubStr )
				{
				case 0:		
					{
						ti.SetText( nSkillAtt );   
						pString->AddText( &ti );
					}
					break;
				case 1:		
					{
						std::wstring msg = UISTRING_TEXT( "TOOLTIP_SKILL_RECOVERY_VALUE" );
						DmCS::StringFn::Replace( msg, L"#Value#", nSkillAtt );
						ti.SetText( msg.c_str() );
						pString->AddText( &ti );
					}
					break;
				}
			}
			break;
		case nSkill::Me_102:
			{
				std::wstring msg;
				DmCS::StringFn::Format( msg, L"%d %% %s", nSkillAtt, UISTRING_TEXT( "COMMON_TXT_INCREASE" ).c_str() );
				ti.SetText( msg.c_str() ); 
				pString->AddText( &ti );
			}
			break;
		case nSkill::Me_103:
			{
				std::wstring msg;
				DmCS::StringFn::Format( msg, L"%d %% %s", nSkillAtt, UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );
				ti.SetText( msg.c_str() ); 
				pString->AddText( &ti );
			}
			break;			

		case nSkill::Me_205:	// % 확률로 발동하는 도트 데미지
		case 206/*nSkill::Me_206*/:	// % 확률로 발동한 버프, 스킬 사용 시 B값 추가
		case 207/*nSkill::Me_207*/:	// % 확률로 발동한 버프, 스킬 사용 시 + B% 추가
		case 208/*nSkill::Me_208*/:	// % 스킬 레벨에 따른 지속 시간 증가 연산 - 무적 / 행동불능 연산 방식
			{
				//확률값 있을 땐 확률 표시
				if( pFTInfo->s_Apply[i].s_nInvoke_Rate != 0 )
				{
					std::wstring msg = UISTRING_TEXT( "TOOLTIP_ACTIVATION_PROBABILITY" );
					DmCS::StringFn::Replace( msg, L"#Prob#", (pFTInfo->s_Apply[i].s_nInvoke_Rate / 100) );
					ti.SetText( msg.c_str() ); 
					pString->AddText( &ti );
				}

				switch( pFTInfo->s_Apply[i].s_nA )
				{
				case APPLY_HP:
				case APPLY_DS:
					{
						if( pFTInfo->s_Apply[i].s_nA == APPLY_HP )
							ti.SetText( UISTRING_TEXT( "COMMON_TXT_HP" ).c_str() );
						else if( pFTInfo->s_Apply[i].s_nA == APPLY_DS )
							ti.SetText( UISTRING_TEXT( "COMMON_TXT_DS" ).c_str() );

						pString->AddText( &ti );
						pString->TailAddSizeX( 2 );

						std::wstring msg;
						if( pFTInfo->s_Apply[ i ].s_nID == 207 )//+ B% 
							DmCS::StringFn::Format( msg,  _T( "%d%% " ), nSkillAtt);
						else
							DmCS::StringFn::Format( msg,  _T( "%d " ), nSkillAtt);

						switch( pFTInfo->s_Apply[ i ].s_nBuffCode )//기본효과(디지몬 공격 스킬)가 아니라면 s_nBuffCode 는 효과적용 타겟
						{
						case nTarget::MY_ALL:
						case nTarget::MY_PALL:
						case nTarget::TAMER:
						case nTarget::DIGIMON:		msg.append( UISTRING_TEXT( "TOOLTIP_SKILL_RECOVERY" ).c_str() );	break;
						default:					msg.append( UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );	break;
						}
						ti.SetText( msg.c_str() );
						pString->AddText( &ti );
					}
					break;
					// 크리티컬 / 회피 증가 스킬 추가 14.05.28 chu8820
				case APPLY_CA:
				case APPLY_EV:
					{
						if( pFTInfo->s_Apply[i].s_nA == APPLY_CA )
							ti.SetText( UISTRING_TEXT( "COMMON_TXT_CRITICAL" ).c_str() );
						else if( pFTInfo->s_Apply[i].s_nA == APPLY_EV )
							ti.SetText( UISTRING_TEXT( "COMMON_TXT_AVOID" ).c_str() );

						pString->AddText( &ti );
						pString->TailAddSizeX( 2 );

						if( pFTInfo->s_Apply[ i ].s_nID == 207 )//+ B% 
							nSkillAtt *= 100;

						std::wstring msg;
						DmCS::StringFn::Format( msg, _T( "%.2f%% " ), (float)nSkillAtt*0.01f );
						switch( pFTInfo->s_Apply[ i ].s_nBuffCode )	// 효과적용 타겟
						{
						case nTarget::MY_ALL:
						case nTarget::MY_PALL:
						case nTarget::TAMER:
						case nTarget::DIGIMON:	msg.append( UISTRING_TEXT( "COMMON_TXT_RISE" ).c_str() );	break;
						default:				msg.append( UISTRING_TEXT( "COMMON_TXT_DECREASE" ).c_str() );	break;
						}
						ti.SetText( msg.c_str() );
						pString->AddText( &ti );
					}
					break;
				default:
					break;
				}

			}
			break;

		case nSkill::Me_106:
			break;
		default:
			//assert_cs( false );
			break;
		}
		m_StringList.AddTail( pString );
	}
}