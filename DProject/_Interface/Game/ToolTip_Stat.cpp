
#include "stdafx.h"
#include "ToolTip.h"

#define IF_TOOLTIP_STAT_STRLEN			64
#define IF_TOOLTIP_STAT_ICON_SIZE CsPoint( 32, 32 )

//=================================================================================================================
//
//		타이틀
//
//=================================================================================================================
cString* cTooltip::_MakeTooltip_Stat_Title( int const& nStatType, bool& bFloat, bool& bFloat2 )
{
	cString* pString = NULL;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
	
	ti.s_Color = NiColor( 1, 1, 0 );	

	// 스탯 이름
	bFloat = false;
	bFloat2 = false;
	switch( nStatType )
	{
	case nTable_Parameter::APPLY_MAXHP:	pString = _AddIcon( ICONITEM::Stat, 0, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_MAX_HP" ).c_str() );	break;
	case nTable_Parameter::APPLY_MAXDS:	pString = _AddIcon( ICONITEM::Stat, 1, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_MAX_DS" ).c_str() );	break;
	case nTable_Parameter::APPLY_DP:	pString = _AddIcon( ICONITEM::Stat, 2, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_DEFENSE" ).c_str() );			break;
	case nTable_Parameter::APPLY_EV:	pString = _AddIcon( ICONITEM::Stat, 3, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_AVOID" ).c_str() );	bFloat2 = true;		break;
	case nTable_Parameter::APPLY_CA:	pString = _AddIcon( ICONITEM::Stat, 4, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_CRITICAL_HIT" ).c_str() );	bFloat2 = true;		break;
	case nTable_Parameter::APPLY_HT:	pString = _AddIcon( ICONITEM::Stat, 5, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_HIT_RATE" ).c_str() );		break;
	case nTable_Parameter::APPLY_AP:	pString = _AddIcon( ICONITEM::Stat, 6, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" ).c_str() );		break;
	case nTable_Parameter::APPLY_AS:	pString = _AddIcon( ICONITEM::Stat, 7, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_ATTACK_SPEED" ).c_str() );	bFloat = true;	break;
	case nTable_Parameter::APPLY_BL:	pString = _AddIcon( ICONITEM::Stat, 9, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_BLOCK" ).c_str() );	bFloat2 = true;	break;
	case nTable_Parameter::APPLY_UB:	pString = _AddIcon( ICONITEM::Stat, 9, IF_TOOLTIP_STAT_ICON_SIZE, 60 );	ti.SetText( UISTRING_TEXT( "COMMON_TXT_UNBEATABLE" ).c_str() );	bFloat2 = true;	break;
	default:			assert_csm( false, L"정의 되지 않은 스탯값임 pParameter참조!!" )	return NULL;
	}

	pString->AddText( &ti );
	return pString;
}

cString* cTooltip::_MakeAddStat(bool bNew, ICONITEM::eTYPE const& type, int nIconIdx, CFont::eFACE_SIZE eFontSize, NiColor s_Color, int const& nVale, bool const& bFloat, bool const& bFloat2 )
{
	cString* pString = _AddIcon( type, nIconIdx, CsPoint( 26, 26 ), 61, CsPoint( 12, -5 ) );
	SAFE_POINTER_RETVAL( pString, NULL );
	pString->GetElement( 0 )->s_ptSize.y = 20;

	_MakeAddValueText( pString, bNew, eFontSize, s_Color, nVale, bFloat, bFloat2 );
	return pString;
}

void cTooltip::_MakeAddValueText( cString* pStr, bool bNew, CFont::eFACE_SIZE eFontSize, NiColor s_Color, __int64 const& nVale, bool const& bFloat, bool const& bFloat2 )
{
	SAFE_POINTER_RET( pStr );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, eFontSize );
	ti.s_Color = s_Color;

	std::wstring txt;
	if( bNew )
	{
		if( bFloat )		DmCS::StringFn::Format( txt, _T( "%.3f" ), (float)(nVale*0.001f)  );
		else if( bFloat2 )	DmCS::StringFn::Format( txt, _T( "%.2f%%" ), (float)(nVale*0.01f)  );
		else				DmCS::StringFn::Format( txt, _T( "%I64d" ), nVale );
	}
	else
	{
		if( nVale > 0 )
		{
			if( bFloat )		DmCS::StringFn::Format( txt, _T( "(+%.3f)" ), (float)(nVale*0.001f)  );
			else if( bFloat2 )	DmCS::StringFn::Format( txt, _T( "(+%.2f%%)" ), (float)(nVale*0.01f)  );
			else				DmCS::StringFn::Format( txt, _T( "(+%I64d)" ), nVale );
		}
		else
		{
			if( bFloat )		DmCS::StringFn::Format( txt, _T( "(%.3f)" ), (float)(nVale*0.001f)  );
			else if( bFloat2 )	DmCS::StringFn::Format( txt, _T( "(%.2f%%)" ), (float)(nVale*0.01f)  );
			else				DmCS::StringFn::Format( txt, _T( "(%I64d)" ), nVale );
		}
	}

	ti.SetText( txt.c_str() );
	pStr->AddText( &ti );
	pStr->CalMaxSize();
}

void cTooltip::_MakeTooltip_Stat()
{
	SAFE_POINTER_RET( m_pData );
	bool bTamer = (((CsC_AvObject*)m_pData)->GetLeafRTTI() == RTTI_TAMER_USER)?true:false;

	// MainID = 스탯 타입, SubID_1 = 기본 스탯,	SubID_2 = 추가 스탯_1, SubID_3 = 추가 스탯_2
	if( bTamer )
		_MakeTooltip_Stat_Tamer();
	else
		_MakeTooltip_Stat_Digimon();
}

void cTooltip::_MakeTooltip_Stat_Tamer()
{
	// 스탯 이름
	bool bFloat = false;// 표시 타입이 float인지 체크 100분률
	bool bFloat2 = false;// 표시 타입이 float인지 체크 10분률

	cString* pString = _MakeTooltip_Stat_Title( m_nMainID, bFloat, bFloat2 );
	SAFE_POINTER_RET( pString );
	m_StringList.AddTail( pString );

	_AddLine( true );

	int nTotalResult = 0;
	// 기본 스텟 정보
	cString * pBaseStat = _MakeAddStat( true, ICONITEM::Stat, 10, CFont::FS_11, FONT_WHITE, m_nSubID_1, bFloat, bFloat2 );
	SAFE_POINTER_RET( pBaseStat );
	m_StringList.AddTail( pBaseStat );

	nTotalResult += m_nSubID_1;

	// 추가 스탯 1
	if( m_nSubID_2 != 0 )// 테이머일 경우 장비 추가 스텟
	{
		NiColor color = m_nSubID_2 > 0 ? NiColor( 0, 1, 0 ) : NiColor( 1, 0, 0 );
		cString * pBaseStat = _MakeAddStat( true, ICONITEM::Stat, 13, CFont::FS_11, color, m_nSubID_2, bFloat, bFloat2 );
		SAFE_POINTER_RET( pBaseStat );
		m_StringList.AddTail( pBaseStat );

		nTotalResult += m_nSubID_2;
	}

	_AddLine( true );

	// 최종값
	cString * pTotalStat = _MakeAddStat( true, ICONITEM::Stat, 14, CFont::FS_11, FONT_WHITE, nTotalResult, bFloat, bFloat2 );
	SAFE_POINTER_RET( pTotalStat );
	m_StringList.AddTail( pTotalStat );
}

void cTooltip::_MakeTooltip_Stat_Digimon()
{
	// 스탯 이름
	bool bFloat = false;// 표시 타입이 float인지 체크 100분률
	bool bFloat2 = false;// 표시 타입이 float인지 체크 10분률

	cString* pString = _MakeTooltip_Stat_Title( m_nMainID, bFloat, bFloat2 );
	SAFE_POINTER_RET( pString );
	m_StringList.AddTail( pString );

	_AddLine( true );

	int nTotalResult = m_nSubID_1;

	CDigimonUser* pDigiUser = static_cast<CDigimonUser*>(m_pData);
	SAFE_POINTER_RET( pDigiUser );
	SAFE_POINTER_RET( pDigiUser->GetBaseStat() );

	// 기본 스텟 정보
	cString * pBaseStat = _MakeAddStat( true, ICONITEM::Stat, 11, CFont::FS_11, FONT_WHITE, nTotalResult, bFloat, bFloat2 );
	SAFE_POINTER_RET( pBaseStat );
	m_StringList.AddTail( pBaseStat );

	// 버프로 증가되는 스텟 정보
	__int64 nBuffVal = FMDigimon::GetBuffDigimonStatValue_N( (nTable_Parameter)m_nMainID, m_nSubID_1 );
	nBuffVal += FMDigimon::GetDeBuffDigimonStatValue_N( (nTable_Parameter)m_nMainID, m_nSubID_1 );
	nBuffVal += FMDigimon::GetSysBuffDigimonStatValue_N( (nTable_Parameter)m_nMainID, m_nSubID_1 );

	if( nBuffVal != 0 )
	{
		NiColor color = nBuffVal > 0 ? FONT_GREEN : FONT_RED;
		_MakeAddValueText( pBaseStat, false, CFont::FS_11, color, nBuffVal, bFloat, bFloat2 );
	}

	nTotalResult += nBuffVal;

	{
		__int64 accValue = FMDigimon::GetAccessoryEquipValue( (nTable_Parameter)m_nMainID, nTamer::Ring );
		accValue += FMDigimon::GetAccessoryEquipValue( (nTable_Parameter)m_nMainID, nTamer::Necklace );
		accValue += FMDigimon::GetAccessoryEquipValue( (nTable_Parameter)m_nMainID, nTamer::Earring );
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
		accValue += FMDigimon::GetAccessoryEquipValue( (nTable_Parameter)m_nMainID, nTamer::Bracelet );
#endif
		if( accValue )
		{
			cString * pAccStat = _MakeAddStat( true, ICONITEM::Stat, 13, CFont::FS_11, FONT_GREEN, accValue, bFloat, bFloat2 );
			SAFE_POINTER_RET( pAccStat );
			m_StringList.AddTail( pAccStat );
			nTotalResult += accValue;
		}		


		FMDigimon::sDINFO info;
		info.s_nDigimonID = pDigiUser->GetFTID();
		info.s_nLevel = pDigiUser->GetBaseStat()->GetLevel();
		info.s_fGameOrgScale = pDigiUser->GetOrgGameScale();
		
		__int64 equipValue = FMDigimon::GetTamerEquipValue( &info, (nTable_Parameter)m_nMainID, nsPART::Costume );
		equipValue += FMDigimon::GetTamerEquipValue( &info, (nTable_Parameter)m_nMainID, nTamer::MaxParts );		// 디지바이스
		if( equipValue )
		{
			cString* pEquipStat = _MakeAddStat( true, ICONITEM::Stat, 13, CFont::FS_11, FONT_GREEN, equipValue, bFloat, bFloat2 );
			SAFE_POINTER_RET( pEquipStat );
			m_StringList.AddTail( pEquipStat );
			nTotalResult += equipValue;
		}
	}


	// 추가 스탯 1
	if( m_nSubID_2 != 0 )// 디지몬일 경우 칩셋
	{
		NiColor color = m_nSubID_2 > 0 ? FONT_GREEN : FONT_RED;
		cString * pBaseStat = _MakeAddStat( true, ICONITEM::Stat, 12, CFont::FS_11, color, m_nSubID_2, bFloat, bFloat2 );
		SAFE_POINTER_RET( pBaseStat );
		m_StringList.AddTail( pBaseStat );

		nTotalResult += m_nSubID_2;
	}

	// 추가 스탯 2
	if( m_nSubID_3 != 0 ) // 친화력
	{
		NiColor color = m_nSubID_3 > 0 ? FONT_GREEN : FONT_RED;
		cString * pBaseStat = _MakeAddStat( true, ICONITEM::Stat, 10, CFont::FS_11, color, m_nSubID_3, bFloat, bFloat2 );
		SAFE_POINTER_RET( pBaseStat );
		m_StringList.AddTail( pBaseStat );

		nTotalResult += m_nSubID_3;
	}

	// 디지몬 강화 정보
	std::pair<int,__int64> enchantInfo;	
	CDigimon::sENCHENT_STAT* pEnchantInfo = pDigiUser->GetEnchantStat();
	if( pEnchantInfo )
	{
		switch( m_nMainID )
		{
		case nTable_Parameter::APPLY_AP:	
			enchantInfo.first = pEnchantInfo->GetEnchantLv( ET_AT );
			enchantInfo.second = m_nSubID_1 * (pEnchantInfo->GetEnchantValue( ET_AT ) * 0.01f);
			break;// 공격력 증가
		case nTable_Parameter::APPLY_CA:	
			enchantInfo.first = pEnchantInfo->GetEnchantLv( ET_CR );
			enchantInfo.second = m_nSubID_1 * (pEnchantInfo->GetEnchantValue( ET_CR ) * 0.01f);
			break;// 크리티컬 대미지 증가
		case nTable_Parameter::APPLY_BL:	
			enchantInfo.first = pEnchantInfo->GetEnchantLv( ET_BL );	
			enchantInfo.second = m_nSubID_1 + (pEnchantInfo->GetEnchantValue( ET_BL ) * 100);
			break;// 방어력 증가
		case nTable_Parameter::APPLY_EV:	
			enchantInfo.first = pEnchantInfo->GetEnchantLv( ET_EV );
			enchantInfo.second = m_nSubID_1 * (pEnchantInfo->GetEnchantValue( ET_EV ) * 0.01f);
			break;// 회피력 증가
		case nTable_Parameter::APPLY_MAXHP:
			enchantInfo.first = pEnchantInfo->GetEnchantLv( ET_HP );
			enchantInfo.second = m_nSubID_1 * (pEnchantInfo->GetEnchantValue( ET_HP ) * 0.01f);
			break;// 최대 생명력 증가
		case nTable_Parameter::APPLY_AS:	
			//			enchantInfo.first = pEnchantInfo->GetAttSpeed_Enchant_LV();
			//			enchantInfo.second = m_nSubID_1 * (pEnchantInfo->GetAttSpeed_Enchant() * 0.01f);
			break;// 공격속도 (사용 안함)
		case nTable_Parameter::APPLY_HT:	
			//			enchantInfo.first = pEnchantInfo->GetHitRate_Enchant_LV();
			//			enchantInfo.second = m_nSubID_1 + (pEnchantInfo->GetHitRate_Enchant() * 0.01f);
			break;// 적중 강화량 (사용 안함)
		}

		if( enchantInfo.first > 0 )
		{
			cString* pString = _AddIcon( ICONITEM::Stat, 8, CsPoint( 26, 26 ), 61, CsPoint( 12, -5 ) );
			if( pString )
			{
				pString->AddIcon( CsPoint( 23, 18 ), ICONITEM::Enchant_Lv, enchantInfo.first, 0, CsPoint( 0, -2 ) );
				pString->TailAddSizeX( 10 );

				_MakeAddValueText( pString, true, CFont::FS_11, FONT_GREEN, enchantInfo.second, bFloat, bFloat2 );
				SAFE_POINTER_RET( pString );
				m_StringList.AddTail( pString );
			}
		}
		nTotalResult += enchantInfo.second;
	}

	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	CDigimonUser::sUSER_STAT* pUserStat = (CDigimonUser::sUSER_STAT*)pDigimonUser->GetBaseStat();

	nTable_Parameter stateType = static_cast<nTable_Parameter>(m_nMainID);
	std::pair<nTable_Parameter, int> PlusPair;
	PlusPair.first = stateType;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_SEALMASTER_PLUS_STATE, &PlusPair );

	if(PlusPair.second > 0)
	{
		cString* pString = _AddIcon( ICONITEM::Stat, 15, CsPoint( 26, 26 ), 61, CsPoint( 12, -5 ) );
		SAFE_POINTER_RET(pString);
		_MakeAddValueText( pString, true, CFont::FS_11, FONT_GREEN, PlusPair.second, bFloat, bFloat2 );
		m_StringList.AddTail(pString);
		nTotalResult = nTotalResult + PlusPair.second;
	}

	_AddLine( true );

	// 최종값
	cString * pTotalStat = _MakeAddStat( true, ICONITEM::Stat, 14, CFont::FS_11, FONT_WHITE, nTotalResult, bFloat, bFloat2 );
	SAFE_POINTER_RET( pTotalStat );

	// 피로도 페널티
	if( (nTable_Parameter)m_nMainID == nTable_Parameter::APPLY_AS )
	{
		CsC_AvObject::sBASE_STAT* pStat = pDigiUser->GetBaseStat();
		if( pStat && pStat->IsFatigue() )
		{
			int fPenalty = nTotalResult * 0.5f;
			_MakeAddValueText( pTotalStat, false, CFont::FS_11, FONT_RED, fPenalty, bFloat, bFloat2 );
		}
	}

	m_StringList.AddTail( pTotalStat );
}

void cTooltip::_MakeTooltip_Stat_Explain()
{
	// MainID = 스탯 타입		
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );

	//=================================================================================================================
	//
	//		타이틀
	//
	//=================================================================================================================
	// 이름		
	ti.s_Color = NiColor( 1, 1, 0 );

	switch( m_nMainID )
	{
	case 0:
		pString = _AddIcon( ICONITEM::Stat, 6, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_ATTACK" ).c_str() );
		break;	
	case 1:
		pString = _AddIcon( ICONITEM::Stat, 9, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_BLOCK" ).c_str() );
		break;		
	case 2:
		pString = _AddIcon( ICONITEM::Stat, 4, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_CRITICAL_HIT" ).c_str() );
		break;		
	case 3:
		pString = _AddIcon( ICONITEM::Stat, 3, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_AVOID" ).c_str() );
		break;
	case 4:
		pString = _AddIcon( ICONITEM::Stat, 2, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_DEFENSE" ).c_str() );		
		break;	
	case 5:
		pString = _AddIcon( ICONITEM::Stat, 7, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ATTACK_SPEED_SEC" ).c_str() );		
		break;
	case 6:
		pString = _AddIcon( ICONITEM::Stat, 5, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_STAT_HIT_RATE" ).c_str() );
		break;			
	default:
		return;
	}
	pString->AddText( &ti );
	m_StringList.AddTail( pString );	

	_AddLine( true );

	ti.s_Color = FONT_WHITE;
	std::wstring wsExplain;
	switch( m_nMainID )
	{
	case 0:
		wsExplain = UISTRING_TEXT( "TOOLTIP_STAT_ATTACK_EXPLAIN" );
		break;	
	case 1:
		wsExplain = UISTRING_TEXT( "TOOLTIP_STAT_BLOCK_EXPLAIN" );
		break;	
	case 2:
		wsExplain = UISTRING_TEXT( "TOOLTIP_STAT_CRITICAL_HIT_EXPLAIN" );
		break;		
	case 3:	
		wsExplain = UISTRING_TEXT( "TOOLTIP_STAT_AVOID_EXPLAIN" );
		break;
	case 4:
		wsExplain = UISTRING_TEXT( "TOOLTIP_STAT_DEFENSE_EXPLAIN" );
		break;	
	case 5:
		wsExplain = UISTRING_TEXT( "TOOLTIP_ATTACK_SPEED_SEC_EXPLAIN" );
		break;	
	case 6:
		wsExplain = UISTRING_TEXT( "TOOLTIP_STAT_HIT_RATE_EXPLAIN" );
		break;	
	default:
		return;
	}	

	g_pStringAnalysis->Cut( &m_StringList, 280, wsExplain.c_str(), &ti );
}

void cTooltip::_MakeTooltip_Enchant_Explain()
{
	// MainID = 스탯 타입		
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );

	//=================================================================================================================
	//
	//		타이틀
	//
	//=================================================================================================================
	// 이름		
	ti.s_Color = NiColor( 1, 1, 0 );

	switch( m_nMainID )
	{
	case nExtendState::ET_AT:
		pString = _AddIcon( ICONITEM::Stat, 6, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_TITLE_AT" ).c_str() );		// 공격력
		break;	
	case nExtendState::ET_BL:
		pString = _AddIcon( ICONITEM::Stat, 9, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_TITLE_BL" ).c_str() );		// 블럭
		break;		
	case nExtendState::ET_CR:
		pString = _AddIcon( ICONITEM::Stat, 4, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_TITLE_CR" ).c_str() );		// 치명타
		break;		
// 	case nExtendState::ET_AS:
// 		pString = _AddIcon( ICONITEM::Stat, 7, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
// 		ti.SetText( _LAN( "공격속도(초)" ) );
// 		break;
	case nExtendState::ET_EV:
		pString = _AddIcon( ICONITEM::Stat, 3, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_TITLE_EV" ).c_str() );		// 회피
		break;	
// 	case nExtendState::ET_HT:
// 		pString = _AddIcon( ICONITEM::Stat, 5, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
// 		ti.SetText( _LAN( "공격성공률" ) );
// 		break;
	case nExtendState::ET_HP:
		pString = _AddIcon( ICONITEM::Stat, 0, IF_TOOLTIP_STAT_ICON_SIZE, 60 );
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_TITLE_HP" ).c_str() );		// 체력
		break;
	default:
		return;
	}
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine( true );

	ti.s_Color = FONT_WHITE;
	std::wstring wsExplain;
	switch( m_nMainID )
	{
	case ET_AT:
		wsExplain = UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_AT" );
//		_stprintf_s( sz, 256, _LAN( "해당 수치가 높을수록 적에게 보다 높은 평타 데미지와 스킬데미지를 줄 수 있습니다." ) );
		break;	
	case ET_BL:
		wsExplain = UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_BL" );
//		_stprintf_s( sz, 256, _LAN( "해당능력은 디지몬 강화를 통해 발동되며 수치가 높을 수록 일정확률로 데미지를 감소하는 블럭효과가 발생합니다." ) );
		break;	
	case ET_CR:
		wsExplain = UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_CR" );
//		_stprintf_s( sz, 256, _LAN( "해당 수치가 높을수록 일정 확률로 적에게 기본공격력 보다 더 큰 데미지를 줄 수 있습니다." ) );
		break;		
// 	case ET_AS:		
// 		_stprintf_s( sz, 256, _LAN( "해당 수치가 높을수록 적의 공격을 피할 확률이 상승 합니다." ) );
// 		break;
	case ET_EV:
		wsExplain = UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_EV" );
//		_stprintf_s( sz, 256, _LAN( "해당 수치가 높을수록 적에게 받는 데미지를 줄일 수 있습니다." ) );		
		break;	
// 	case ET_HT:	
// 		_stprintf_s( sz, 256, _LAN( "해당 수치가 높을수록 적에게 더욱 빠른 공격을 가하게 됩니다." ) );		
// 		break;	
	case ET_HP:
		wsExplain = UISTRING_TEXT( "TOOLTIP_ENCHANT_EXPLAIN_HP" );
//		_stprintf_s( sz, 256, _LAN( "해당 수치가 높을수록 적이 자신의 공격을 피하는 확률이 줄어 듭니다." ) );
		break;	
	default:
		return;
	}	

	g_pStringAnalysis->Cut( &m_StringList, 280, wsExplain.c_str(), &ti );
}
