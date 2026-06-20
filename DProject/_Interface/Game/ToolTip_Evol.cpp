
#include "stdafx.h"
#include "ToolTip.h"
#include "../Base/UITextSafe.h"


void cTooltip::_MakeTooltip_QuickEvol()
{
	// MainID = 소유 인터페이스
	// SubID_1 = 슬롯 인덱스

	SAFE_POINTER_RET( g_pCharMng );
	SAFE_POINTER_RET( g_pDataMng );
	CDigimonUser* pDigimonUser = g_pCharMng->GetDigimonUser( 0 );
	SAFE_POINTER_RET( pDigimonUser );
	SAFE_POINTER_RET( pDigimonUser->GetBaseStat() );
	SAFE_POINTER_RET( nsCsFileTable::g_pEvolMng );
	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( pDigimonUser->GetBaseDigimonFTID() );
	SAFE_POINTER_RET( pFTEvolInfo );
	CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByEvoSlot( static_cast<int>(m_nSubID_1) );
	SAFE_POINTER_RET( pFTEvolObj );
	DWORD dwDestModelID = pFTEvolObj->m_dwID;
	SAFE_POINTER_RET( nsCsFileTable::g_pDigimonMng );
	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	SAFE_POINTER_RET( nsCsFileTable::g_pQuestMng );
	CsDigimon* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID );
	SAFE_POINTER_RET( pFTDigimon );
	CsDigimon::sINFO* pFTDigimonInfo = pFTDigimon->GetInfo();
	SAFE_POINTER_RET( pFTDigimonInfo );

	cString* pString;
	TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ];
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );

	// 상태 - 이름색
	DWORD nEvoValue = pDigimonUser->GetEvoValue( pFTEvolObj->m_nEvoSlot, CDigimonUser::eActive_Mask );
	switch( nEvoValue )
	{
	case CDigimonUser::eActive:
		ti.s_Color = FONT_GOLD;
		break;
	case CDigimonUser::eInActive:	
		ti.s_Color = TOOLTIP_CANNOT_COLOR;
		break;
	case CDigimonUser::eNone:
	case CDigimonUser::eClosed:
		ti.s_Color = NiColor( 0.5f, 0.5f, 0.5f );	
		break;
	default:
		assert_cs( false );
	}

	// 여유 줄
	pString = NiNew cString;
	pString->AddSizeY( 1 );
	m_StringList.AddTail( pString );

	// 이름	
	pString = NiNew cString;
	ti.SetText( UiTextSafe::Safe( pFTDigimonInfo->s_szName ) );	
	pString->AddText( &ti );
	pString->AddSizeY( 3 );
	m_StringList.AddTail( pString );

	_AddLine( false );

	// Evol속성
	pString = NiNew cString;
#ifdef VERSION_USA
	pString->AddIcon( CsPoint( 80, 19 ), ICONITEM::DEvol, pFTDigimonInfo->s_eEvolutionType );
#else
	pString->AddIcon( CsPoint( 60, 19 ), ICONITEM::DEvol, pFTDigimonInfo->s_eEvolutionType );
#endif
	
	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	// Attribute
	pString = NiNew cString;
	// 0기반 인덱스로 전환

	// - 성장 속성
	int nIndex = pFTDigimonInfo->s_nDigimonType - 1;
	if( nIndex < 0 ) nIndex = 0;
	pString->AddIcon( CsPoint( 30, 19 ), ICONITEM::DGrowType, nIndex );

	// - 디지몬 속성
	nIndex = pFTDigimonInfo->s_eAttributeType - nsCsDigimonTable::AT_NONE;
	if( nIndex < 0 ) nIndex = 0;
	pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, nIndex );
	// - 패밀리 속성
	for( int i=0; i<DIGIMON_MAX_FAMILY; ++i )
	{
		if( pFTDigimonInfo->s_eFamilyType[ i ] != 0 )
		{
			nIndex = pFTDigimonInfo->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
			if( nIndex < 0 ) nIndex = 0;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Family, nIndex );
		}
	}

	// - 자연 속성
	if( pFTDigimonInfo->s_eBaseNatureTypes[0] != 0 )
	{
		nIndex = pFTDigimonInfo->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE;
		if( nIndex < 0 ) nIndex = 0;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
	}
	// - 자연 속성
	if( pFTDigimonInfo->s_eBaseNatureTypes[1] != 0 )
	{
		nIndex = pFTDigimonInfo->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE;
		if( nIndex < 0 ) nIndex = 0;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
	}
	// - 자연 속성
	if( pFTDigimonInfo->s_eBaseNatureTypes[2] != 0 )
	{
		nIndex = pFTDigimonInfo->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE;
		if( nIndex < 0 ) nIndex = 0;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
	}
		
	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	_AddLine( true );

	// 소모 DS
	pString = NiNew cString;
	ti.s_Color = FONT_GOLD;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_TAMER_DS" ).c_str() );
#ifdef SDM_TAMER_XGUAGE_20180628
	if(	nsCsDigimonTable::ET_X_ROOKIE <= pFTDigimonInfo->s_eEvolutionType &&
		pFTDigimonInfo->s_eEvolutionType <= nsCsDigimonTable::ET_X_JOGRESS)
		ti.SetText(  UISTRING_TEXT("XEVOLUTION_TAMER_CONSUME_XG").c_str() );
#endif
	pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

	int nReqDS = CsMax( 0, FMDigimon::GetCostEvlove( pDigimonUser->GetFTID(), pDigimonUser->GetBaseStat()->GetLevel(), dwDestModelID ) );
	_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d" ), nReqDS );

	ti.s_Color = NiColor( 1, 0, 0 );
	CTamerUser* pTUser = g_pCharMng->GetTamerUser();
	if( pTUser )
	{
		CTamerUser::sUSER_STAT* pStat = dynamic_cast<CTamerUser::sUSER_STAT*>(pTUser->GetBaseStat());
		if( pStat )
		{
#ifdef SDM_TAMER_XGUAGE_20180628
			if(	nsCsDigimonTable::ET_X_ROOKIE <= pFTDigimonInfo->s_eEvolutionType &&
				pFTDigimonInfo->s_eEvolutionType <= nsCsDigimonTable::ET_X_JOGRESS)
			{
				if( pStat->GetCurrentXGuage() >= nReqDS )
					ti.s_Color = FONT_WHITE;
			}
			else
#endif
			if( pStat->GetDS() >= nReqDS )
				ti.s_Color = FONT_WHITE;
		}
	}
	ti.SetText( sz );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	cData_Inven* pInven = g_pDataMng->GetInven();
	cData_Digivice* pDigivice = g_pDataMng->GetDigivice();
	// 진화시 필요아이템 (조그레스)
	if( pFTEvolObj->m_nChipsetType != 0 && pInven && pDigivice )
	{					
		int nHaveCnt1 = pInven->GetCount_Item_TypeL( pFTEvolObj->m_nChipsetType );	
		int nHaveCnt2 = pDigivice->GetCount_Item_TypeL( pFTEvolObj->m_nChipsetType );	

		pString = NiNew cString;
		ti.s_Color = FONT_GOLD;
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_REQUIRE_ITEM" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

		std::wstring wsNeedItem;
		wsNeedItem = UISTRING_TEXT( "TOOLTIP_ITEM_NAME_COUNT" );
		// 소모형 칩셋 5203 == 크로스 칩셋
		if( pFTEvolObj->m_nChipsetTypeC % (nItem::Chipset * 100) == 3 )
		{
			bool bEnable = false;
			int nReqIndex = pDigivice->GetChipsetIndex_TypeLT( pFTEvolObj->m_nChipsetType, pFTEvolObj->m_nChipsetTypeC );
			if( nReqIndex != cData_Digivice::INVALIDE_DIGIVICE_INDEX )
			{
				cItemInfo* pChipset = pDigivice->GetChipset( nReqIndex );
				if( pChipset && pChipset->IsEnable() )
					bEnable = pChipset->GetCount() < pFTEvolObj->m_nChipsetNum ? false : true;
			}
			ti.s_Color = bEnable ? FONT_WHITE : TOOLTIP_CANNOT_COLOR;
			DmCS::StringFn::Replace( wsNeedItem, L"#Name#", UISTRING_TEXT( "TOOLTIP_EVOL_CROSS_CHIP" ).c_str() );
		}
		else
		{
			ti.s_Color = (nHaveCnt1 >= pFTEvolObj->m_nChipsetNum) || (nHaveCnt2 >= pFTEvolObj->m_nChipsetNum) ?  FONT_WHITE : TOOLTIP_CANNOT_COLOR;
			DmCS::StringFn::Replace( wsNeedItem, L"#Name#", UISTRING_TEXT( "TOOLTIP_EVOL_JOGRESS_CHIP" ).c_str() );
		}
		DmCS::StringFn::Replace( wsNeedItem, L"#Count#", pFTEvolObj->m_nChipsetNum );

		ti.SetText( wsNeedItem.c_str() );
		pString->AddText( &ti );

		m_StringList.AddTail( pString );
	}

	// 진화시 필요아이템
		if( pFTEvolObj->m_nUseItem != 0 )
		{
		pString = NiNew cString;

		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_CONSUME_ITEM" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem(	pFTEvolObj->m_nUseItem );
		if( pItem == NULL )
			return;

		std::wstring wsNeedItem;
		wsNeedItem = UISTRING_TEXT( "TOOLTIP_ITEM_NAME_COUNT" );
		DmCS::StringFn::Replace( wsNeedItem, L"#Name#", pItem->GetName() );
		DmCS::StringFn::Replace( wsNeedItem, L"#Count#", pFTEvolObj->m_nUseItemNum );

			if( pInven == NULL )
				return;

			if( pInven->GetCount_Item_ID( pFTEvolObj->m_nUseItem ) < pFTEvolObj->m_nUseItemNum )
				ti.s_Color = TOOLTIP_CANNOT_COLOR;
			else
				ti.s_Color = FONT_WHITE;

		ti.SetText( wsNeedItem.c_str() );
		pString->AddText( &ti );

		m_StringList.AddTail( pString );
	}

	// 진화시 필요 친밀도
	if( pFTEvolObj->m_nIntimacy != 0 )
	{
		pString = NiNew cString;

		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_FRIENDLINESS" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

		std::wstring wsNeed;
		DmCS::StringFn::Format( wsNeed, L"%d %s", pFTEvolObj->m_nIntimacy, UISTRING_TEXT( "TOOLTIP_MORE_THAN" ).c_str() );

		if( g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetFriendShip() < pFTEvolObj->m_nIntimacy )				
			ti.s_Color = TOOLTIP_CANNOT_COLOR;						
		else
			ti.s_Color = FONT_WHITE;						

		ti.SetText( wsNeed.c_str() );
		pString->AddText( &ti );

		m_StringList.AddTail( pString );
	}

	//NewEvo
	if( pDigimonUser->IsEvoPlag( pFTEvolObj->m_nEvoSlot, CDigimonUser::eActive ) == false )
	{
		_AddLine( true );

		// 오픈 자격
		if( pFTEvolObj->m_nOpenQualification != NEED_QUALITICATION::_NO_NEED )
		{
			pString = NiNew cString;

			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_OPEN_QUALIFICATION" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			if( pFTEvolObj->m_nOpenQualification == NEED_QUALITICATION::_PARTNERMON)
			{
				// 파트너몬 인지 체크해서 색 선택
				ti.s_Color = TOOLTIP_CANNOT_COLOR;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_PARTNER_DIGIMON_ONLY" ).c_str() );
				pString->AddText( &ti );
			}
			else if( pFTEvolObj->m_nOpenQualification == NEED_QUALITICATION::_ROYAL_KNIGHT)
			{
				// 로얄나이츠 인지 체크해서 색 선택
				ti.s_Color = TOOLTIP_CANNOT_COLOR;

				ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_ROYAL_KNIGHT_ONLY" ).c_str() );
				pString->AddText( &ti );
			}
#ifdef SDM_TAMER_XGUAGE_20180628
			else if( pFTEvolObj->m_nOpenQualification == NEED_QUALITICATION::_XAI_SYSTEM)
			{
				// 로얄나이츠 인지 체크해서 색 선택
				ti.s_Color = TOOLTIP_CANNOT_COLOR;

				ti.SetText( UISTRING_TEXT("XEVOLUTION_ONLY_XAYSYSTEM").c_str() );
				pString->AddText( &ti );
			}
#endif
			m_StringList.AddTail( pString );
		} 
		// 오픈 레벨
		if( pFTEvolObj->m_nOpenLevel != 0 )
		{
			pString = NiNew cString;

			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_OPEN_LEVEL" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_Color = pDigimonUser->GetBaseStat()->GetLevel() >= pFTEvolObj->m_nOpenLevel ? FONT_WHITE : TOOLTIP_CANNOT_COLOR;
			ti.SetText( pFTEvolObj->m_nOpenLevel );
			pString->AddText( &ti );

			m_StringList.AddTail( pString );
		}		
		// 오픈 퀘스트
		if( pFTEvolObj->m_nOpenQuest != 0 )
		{
			pString = NiNew cString;

			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_OPEN_QUEST" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				CsQuest* pQuestInfo = nsCsFileTable::g_pQuestMng->GetQuest( pFTEvolObj->m_nOpenQuest );
				if( pQuestInfo )
				{
					if( g_pDataMng->GetQuest() && g_pDataMng->GetQuest()->IsCompleate( pFTEvolObj->m_nOpenQuest ) == true )
						ti.s_Color = FONT_WHITE;
					else
						ti.s_Color = TOOLTIP_CANNOT_COLOR;

				ti.SetText( UiTextSafe::Safe( pQuestInfo->m_szTitleText ) );
				pString->AddText( &ti );

				m_StringList.AddTail( pString );
			}
		}				
	}




	
	switch( nEvoValue )
	{
	case CDigimonUser::eActive:
	case CDigimonUser::eInActive:
		{						

			// 단축키 - 디지몬창 의 진화 창에서 오는 툴팁이면 단축키 무시
			switch( m_nMainID )
			{
			case cBaseWindow::WT_QUICKEVOL:
				{
					if( m_nSubID_3 >= 5 && m_nSubID_3 < 8 )
						break;

					if( m_nSubID_3 == 8 )
						m_nSubID_3 = 5;

					pString = NiNew cString;
					ti.s_Color = FONT_GOLD;
					ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
					ti.SetText( UISTRING_TEXT( "MAINOPTION_BTN_SHORTCUT" ).c_str() );
					pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

					GLOBALINPUT_ST.GetHotKeyPtr()->GetText( (DMKEY::eTYPE)( DMKEY::EVOL_0 + m_nSubID_3 ), sz, TOOLTIP_MAX_TEXT_LEN );
					ti.s_Color = FONT_WHITE;
					ti.SetText( sz );
					pString->AddText( &ti );

					m_StringList.AddTail( pString );

					if( m_nSubID_3 == 5 )
						m_nSubID_3 = 8;
				}
				break;
			case cBaseWindow::WT_NEW_DIGIMONSTATUS:
			case cBaseWindow::WT_SPIRIT_EVOLUTION:
				break;
			default:
				assert_cs( false );
			}
		}
		break;	

	case CDigimonUser::eClosed:
		{			
			_AddLine( true );
			int	nCurTree = g_pCharMng->GetDigimonUser( 0 )->GetEvoTree();
			/* 테이블에 정의된 현재슬롯 트리 != 현재 찍은 트리*/
			if( g_pCharMng->GetDigimonUser( 0 )->IsJogressTree( pFTEvolObj->m_nEvolutionTree ) )
			{
				if(pFTEvolObj->m_nEvolutionTree != nCurTree && ( CDigimonUser::TREE_COMMON != nCurTree))
				{
					pString = NiNew cString;
					ti.s_Color = TOOLTIP_CANNOT_COLOR;
					ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
					ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_UNUSABLE" ).c_str() );
					pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

					m_StringList.AddTail( pString );

					break;
				}
			}

			if(pFTEvolObj->m_nOpenQuest)
			{
				pString = NiNew cString;
				ti.s_Color = FONT_GOLD;
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.SetText( UISTRING_TEXT( "COMMON_TXT_REQUIRE_QUEST" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				CsQuest* pQuest = nsCsFileTable::g_pQuestMng->GetQuest( pFTEvolObj->m_nOpenQuest );
				if( pQuest == NULL )
					break;
				TCHAR const* szQuest = pQuest->m_szTitleText;
				if( szQuest == NULL )
					szQuest = UISTRING_TEXT( "COMMON_TXT_REQUIRE_QUEST" ).c_str();

				if( g_pDataMng && g_pDataMng->GetQuest() && g_pDataMng->GetQuest()->IsCompleate( pFTEvolObj->m_nOpenQuest ) == true )
					ti.s_Color = FONT_WHITE;				
				else
					ti.s_Color = TOOLTIP_CANNOT_COLOR;

				ti.SetText( szQuest );				
				pString->AddText( &ti );

				m_StringList.AddTail( pString );				
			}
			if(pFTEvolObj->m_nOpenItemTypeS)
			{
				int nTypeL, nTypeS;			
				CsItem::TypeS_to_TypeLS( pFTEvolObj->m_nOpenItemTypeS, nTypeL, nTypeS );
				if( g_pDataMng->GetInven() == NULL )
					break;
				int nHaveCnt = g_pDataMng->GetInven()->GetCount_Item_TypeLS( nTypeL, nTypeS );			

				pString = NiNew cString;
				ti.s_Color = FONT_GOLD;
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.SetText( UISTRING_TEXT( "COMMON_TXT_REQUIRE_ITEM" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
				auto* pQuestData = g_pDataMng->GetQuest();
				if( pFTEvolObj->m_nJoGressQuestCheck && pQuestData && pQuestData->IsProcess( pFTEvolObj->m_nJoGressQuestCheck ) )
					ti.s_Color = FONT_GLAY;			
				else if(nHaveCnt >= pFTEvolObj->m_nOpenItemNum)
					ti.s_Color = FONT_WHITE;
				else
					ti.s_Color = TOOLTIP_CANNOT_COLOR;

				//ti.s_Color = nHaveCnt >= pFTEvolObj->m_nOpenItemNum ?  FONT_WHITE : TOOLTIP_CANNOT_COLOR;

				std::wstring wsNeedItem;
				DWORD dwDispID = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pFTEvolObj->m_nOpenItemTypeS );
				CsItem* pDispItem = nsCsFileTable::g_pItemMng->GetItem( dwDispID );
				if( pDispItem == NULL )
					break;
				CsItem::sINFO* pFTDisp = pDispItem->GetInfo();
				if( pFTDisp == NULL )
					break;
				if( pFTEvolObj->m_nEvolutionTree == CDigimonUser::TREE_CAPSULE )
				{
					wsNeedItem = UISTRING_TEXT( "TOOLTIP_ITEM_NAME_COUNT" );
					DmCS::StringFn::Replace( wsNeedItem, L"#Name#", nsCsFileTable::g_pItemMng->GetTypeName( pFTEvolObj->m_nOpenItemTypeS ) );
					DmCS::StringFn::Replace( wsNeedItem, L"#Count#", pFTEvolObj->m_nOpenItemNum );
				}
				else
				{
					wsNeedItem = UISTRING_TEXT( "TOOLTIP_ITEM_NAME_COUNT" );
					DmCS::StringFn::Replace( wsNeedItem, L"#Name#", pFTDisp->s_szName );
					DmCS::StringFn::Replace( wsNeedItem, L"#Count#", pFTEvolObj->m_nOpenItemNum );
				}

				ti.SetText( wsNeedItem.c_str() );
				pString->AddText( &ti );

				m_StringList.AddTail( pString );

				pString = NiNew cString;
				ti.s_Color = FONT_GOLD;
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_POSSESSION_COUNT" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				if( pFTEvolObj->m_nJoGressQuestCheck && pQuestData && pQuestData->IsProcess( pFTEvolObj->m_nJoGressQuestCheck ) )
					ti.s_Color = FONT_GLAY;
				else
					ti.s_Color = FONT_WHITE;

				ti.SetText( nHaveCnt );
				pString->AddText( &ti );

				m_StringList.AddTail( pString );

			}
		}
		break;
	case CDigimonUser::eNone:
		break;
	default:
		assert_cs( false );
	}	

	// 라이딩 오픈 상태인지
	if( nsCsFileTable::g_pDigimonMng->IsRide( dwDestModelID ) )
	{
		CsRide* pRide = nsCsFileTable::g_pDigimonMng->GetRide( dwDestModelID );
		SAFE_POINTER_RET( pRide );
		SAFE_POINTER_RET( pRide->GetInfo() );
		_AddLine( true );

		ti.s_Color = FONT_GOLD;
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_RIDING_MODE" ).c_str() );

		pString = NiNew cString;
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;


		// 현재 열려 있는지
		if( g_pCharMng->GetDigimonUser( 0 )->IsEvoPlag( pFTEvolObj->m_nEvoSlot, CDigimonUser::eRide ) )
		{
			ti.s_Color = FONT_WHITE;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_USABLE" ).c_str() );
			pString->AddText( &ti );

			m_StringList.AddTail( pString );
		}
		else
		{
			ti.s_Color = FONT_RED;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_UNUSABLE" ).c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_REQUIRE_ITEM" ).c_str() );
			pString = NiNew cString;
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			DWORD nRideNeedItemID1 = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pRide->GetInfo()->s_OpenInfo[ 0 ].s_nItemType_S );
			DWORD nRideNeedItemID2 = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pRide->GetInfo()->s_OpenInfo[ 1 ].s_nItemType_S );
			int nRideNeedItemCnt1 = pRide->GetInfo()->s_OpenInfo[ 0 ].s_nNeedCount;
			int nRideNeedItemCnt2 = pRide->GetInfo()->s_OpenInfo[ 1 ].s_nNeedCount;
			if( nRideNeedItemID1 == 0 )
			{
				assert_cs( nRideNeedItemID2 != 0 );
				nRideNeedItemID1 = nRideNeedItemID2;
				nRideNeedItemCnt1 = nRideNeedItemCnt2;
				nRideNeedItemID2 = 0;
				nRideNeedItemCnt2 = 0;
			}

			assert_cs( nRideNeedItemID1 != 0 );

			std::wstring wsItemName;
			if( nsCsFileTable::g_pItemMng )
			{
				CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nRideNeedItemID1 );
				if( pFTItem )
					wsItemName = pFTItem->GetName();
			}

			std::wstring wsNeedItem = UISTRING_TEXT( "TOOLTIP_ITEM_NAME_COUNT" );
			DmCS::StringFn::Replace( wsNeedItem, L"#Name#", wsItemName.c_str() );
			DmCS::StringFn::Replace( wsNeedItem, L"#Count#", nRideNeedItemCnt1 );

			if( nRideNeedItemID2 != 0 )
			{
				std::wstring wsOr = L" " + UISTRING_TEXT( "TOOLTIP_EVOL_OR" );
				wsNeedItem.append( wsOr.c_str() );
			}

			ti.s_Color = FONT_WHITE;
			ti.SetText( wsNeedItem.c_str() );
			pString->AddText( &ti );

			m_StringList.AddTail( pString );

			if( nRideNeedItemID2 != 0 )
			{
				pString = NiNew cString;
				ti.SetText( _T( " " ) );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				std::wstring wsItemName;
				CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nRideNeedItemID2 );
				if( pFTItem )
				{
					wsItemName = pFTItem->GetName();
					if( wsItemName.empty() )
						wsItemName = L"Unknown Item";
				}
				std::wstring wsNeedItem2;
				DmCS::StringFn::Format( wsNeedItem2, L"%s (%d)", wsItemName.c_str(), nRideNeedItemCnt2 );
				ti.SetText( wsNeedItem2.c_str() );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void cTooltip::_Simple_MakeTooltip_QuickEvol()
{
	// MainID = 소유 인터페이스
	// SubID_1 = 슬롯 인덱스

	// MainID = BaseDigimonFTID
	// SubID_1 = TargetDIgimonFTID

	SAFE_POINTER_RET( nsCsFileTable::g_pEvolMng );
	SAFE_POINTER_RET( nsCsFileTable::g_pDigimonMng );
	CDigimonEvolveInfo* pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( m_nMainID );
	SAFE_POINTER_RET( pFTEvolInfo );
	CDigimonEvolveObj* pFTEvolObj = pFTEvolInfo->GetEvolveObjByID( m_nSubID_1 );
	SAFE_POINTER_RET( pFTEvolObj );
	CsDigimon* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pFTEvolObj->m_dwID );
	SAFE_POINTER_RET( pFTDigimon );
	CsDigimon::sINFO* pFTDigimonInfo = pFTDigimon->GetInfo();
	SAFE_POINTER_RET( pFTDigimonInfo );

	//////////////////////////////////////////////////////////////////////////
	//
	//	디지몬 이름
	//
	//////////////////////////////////////////////////////////////////////////

	{
		cString* pString = NiNew cString;
		pString->AddSizeY( 1 );
		m_StringList.AddTail( pString );
	}

	// 이름	
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_GOLD );
		cString* pString = NiNew cString;
		ti.SetText( UiTextSafe::Safe( pFTDigimonInfo->s_szName ) );	
		pString->AddText( &ti );
		pString->AddSizeY( 3 );
		m_StringList.AddTail( pString );
	}

	_AddLine( false );

	// Evol속성
	{
		cString* pString = NiNew cString;
#ifdef VERSION_USA
		pString->AddIcon( CsPoint( 80, 19 ), ICONITEM::DEvol, pFTDigimonInfo->s_eEvolutionType );
#else
		pString->AddIcon( CsPoint( 60, 19 ), ICONITEM::DEvol, pFTDigimonInfo->s_eEvolutionType );
#endif
		pString->AddSizeY( 5 );
		m_StringList.AddTail( pString );
	}

	{
		// Attribute
		cString* pString = NiNew cString;
		// 0기반 인덱스로 전환

		// - 성장 속성
		int nIndex = pFTDigimonInfo->s_nDigimonType - 1;
		if( nIndex < 0 ) nIndex = 0;
		pString->AddIcon( CsPoint( 30, 19 ), ICONITEM::DGrowType, nIndex );

		// - 디지몬 속성
		nIndex = pFTDigimonInfo->s_eAttributeType - nsCsDigimonTable::AT_NONE;
		if( nIndex < 0 ) nIndex = 0;
		pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, nIndex );
		// - 패밀리 속성
		for( int i=0; i<DIGIMON_MAX_FAMILY; ++i )
		{
			if( pFTDigimonInfo->s_eFamilyType[ i ] != 0 )
			{
				nIndex = pFTDigimonInfo->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
				if( nIndex < 0 ) nIndex = 0;
				pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Family, nIndex );
			}
		}

		// - 자연 속성
		if( pFTDigimonInfo->s_eBaseNatureTypes[0] != 0 )
		{
			nIndex = pFTDigimonInfo->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE;
			if( nIndex < 0 ) nIndex = 0;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
		}
		// - 자연 속성
		if( pFTDigimonInfo->s_eBaseNatureTypes[1] != 0 )
		{
			nIndex = pFTDigimonInfo->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE;
			if( nIndex < 0 ) nIndex = 0;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
		}
		// - 자연 속성
		if( pFTDigimonInfo->s_eBaseNatureTypes[2] != 0 )
		{
			nIndex = pFTDigimonInfo->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE;
			if( nIndex < 0 ) nIndex = 0;
			pString->AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );
		}

		pString->AddSizeY( 5 );
		m_StringList.AddTail( pString );
	}

	_AddLine( true );


	// 소모 DS
	{
		cString* pString = NiNew cString;
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
			ti.s_Color = FONT_GOLD;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_TAMER_DS" ).c_str() );
#ifdef SDM_TAMER_XGUAGE_20180628
			if(nsCsDigimonTable::ET_X_ROOKIE <= pFTDigimonInfo->s_eEvolutionType &&
				pFTDigimonInfo->s_eEvolutionType <= nsCsDigimonTable::ET_X_JOGRESS)
				ti.SetText( UISTRING_TEXT("XEVOLUTION_TAMER_CONSUME_XG").c_str() );
#endif
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			int nReqDS = CsMax( 0, FMDigimon::GetCostEvlove( m_nMainID, 1, m_nSubID_1 ) );
			ti.SetText( nReqDS );
			pString->AddText( &ti );
		}

		m_StringList.AddTail( pString );
	}

	// 진화시 필요아이템 (조그레스)
	if( pFTEvolObj->m_nChipsetType != 0 )
	{
		cString* pString = NiNew cString;
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
			ti.s_Color = FONT_GOLD;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_REQUIRE_ITEM" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );		
			std::wstring wsMsg = UISTRING_TEXT( "TOOLTIP_ITEM_NAME_COUNT" );
			DmCS::StringFn::Replace( wsMsg, L"#Name#", UISTRING_TEXT( "TOOLTIP_EVOL_JOGRESS_CHIP" ).c_str() );
			DmCS::StringFn::Replace( wsMsg, L"#Count#", pFTEvolObj->m_nChipsetNum );
			ti.SetText( wsMsg.c_str() );
			pString->AddText( &ti );
		}
		m_StringList.AddTail( pString );
	}

	// 진화시 필요아이템
	if( pFTEvolObj->m_nUseItem != 0 )
	{
		cString* pString = NiNew cString;
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );		
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_CONSUME_ITEM" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem(	pFTEvolObj->m_nUseItem );
		if( pItem )
		{
			CsItem::sINFO* pItemInfo = pItem->GetInfo();
			if( pItemInfo )
			{
				std::wstring wsMsg = UISTRING_TEXT( "TOOLTIP_ITEM_NAME_COUNT" );
			DmCS::StringFn::Replace( wsMsg, L"#Name#", UiTextSafe::Safe( pItemInfo->s_szName ) );
				DmCS::StringFn::Replace( wsMsg, L"#Count#", pFTEvolObj->m_nUseItemNum );
				cText::sTEXTINFO ti;
				ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );	
				ti.SetText( wsMsg.c_str() );
				pString->AddText( &ti );
			}
		}

		m_StringList.AddTail( pString );
	}

	// 진화시 필요 친밀도
	if( pFTEvolObj->m_nIntimacy != 0 )
	{
		cString* pString = NiNew cString;
		
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );	
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_FRIENDLINESS" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			std::wstring msg;
			DmCS::StringFn::Format( msg, L"%d %s", pFTEvolObj->m_nIntimacy, UISTRING_TEXT( "TOOLTIP_MORE_THAN" ).c_str() );
			ti.SetText( msg.c_str() );
			pString->AddText( &ti );
		}

		m_StringList.AddTail( pString );
	}

	//NewEvo
	_AddLine( true );

	// 오픈 자격
	if( pFTEvolObj->m_nOpenQualification != NEED_QUALITICATION::_NO_NEED )
	{
		cString* pString = NiNew cString;

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_OPEN_QUALIFICATION" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}

		if( pFTEvolObj->m_nOpenQualification == NEED_QUALITICATION::_PARTNERMON )
		{
			// 파트너몬 인지 체크해서 색 선택
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, TOOLTIP_CANNOT_COLOR );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_PARTNER_DIGIMON_ONLY" ).c_str() );
			pString->AddText( &ti );
		}
		else if( pFTEvolObj->m_nOpenQualification == NEED_QUALITICATION::_ROYAL_KNIGHT )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, TOOLTIP_CANNOT_COLOR );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_ROYAL_KNIGHT_ONLY" ).c_str() );
			pString->AddText( &ti );
		}
#ifdef SDM_TAMER_XGUAGE_20180628
		else if( pFTEvolObj->m_nOpenQualification == NEED_QUALITICATION::_XAI_SYSTEM )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, TOOLTIP_CANNOT_COLOR );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_APPLY_XAI_SYSTEM" ).c_str() );
			pString->AddText( &ti );
		}
#endif
		m_StringList.AddTail( pString );
	} 

	// 오픈 레벨
	if( pFTEvolObj->m_nOpenLevel != 0 )
	{
		cString* pString = NiNew cString;
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_OPEN_LEVEL" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		}
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			ti.SetText( pFTEvolObj->m_nOpenLevel );
			pString->AddText( &ti );
		}

		m_StringList.AddTail( pString );
	}		

	// 오픈 퀘스트
	if( pFTEvolObj->m_nOpenQuest != 0 )
	{
		CsQuest* pQuest = nsCsFileTable::g_pQuestMng->GetQuest( pFTEvolObj->m_nOpenQuest );
		if( pQuest )
		{
			cString* pString = NiNew cString;
			{
				cText::sTEXTINFO ti;
				ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_GOLD );
				ti.SetText( UISTRING_TEXT( "TOOLTIP_EVOL_OPEN_QUEST" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
			}

			{
				cText::sTEXTINFO ti;
				ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
				ti.SetText( UiTextSafe::Safe( pQuest->m_szTitleText ) );
				pString->AddText( &ti );
			}

			m_StringList.AddTail( pString );
		}	
	}
}
