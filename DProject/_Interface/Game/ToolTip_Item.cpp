

#include "stdafx.h"
#include "ToolTip.h"
#include "../Base/UITextSafe.h"

void cTooltip::_MakeTooltip_Item(bool compare)
{
	// MainID = 아이템 FT_ID
	// SubID_1 = 해당 인터페이스 창 ( 필요한 경우만 사용 ) - ex.상점
	CsItem* pMainItem = nsCsFileTable::g_pItemMng->GetItem( static_cast<int>(m_nMainID) );
	SAFE_POINTER_RET( pMainItem );
	CsItem::sINFO* pFTInfo = pMainItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13 );

	cString* pString;
	TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ];


	//=================================================================================================================
	//
	//		타이틀
	//
	//=================================================================================================================
	// 이름

	//////////////////////////////////////////////////////////
	//2017-01-19-nova 비교창일경우 아이템 이름에 장착중 이름 더해서 문자열 생성
	std::wstring totalMsg;
	DmCS::StringFn::Format( totalMsg, L"%s (%s)", pFTInfo->s_szName, UISTRING_TEXT("TOOLTIP_EQUIP_ITEM").c_str() );

	ti.s_eFontSize = CFont::FS_12;
	ti.s_Color = g_pDataMng->GetItemColor( pFTInfo );
	ti.SetText( UiTextSafe::Safe( pFTInfo->s_szName ) );

	sSTR_INFO si;	
	si.s_dwPlag = sSTR_INFO::SF_NORMAL;
	m_StringList_Name.SetStrInfo( &si );			

	//////////////////////////////////////////////////////////
	//2017-01-19-nova 비교창일경우(장착중) 글자 출력
	if(compare)
		g_pStringAnalysis->AddCut( &m_StringList_Name, 9999, totalMsg.c_str(), &ti );	
	else
		g_pStringAnalysis->AddCut( &m_StringList_Name, 9999, UiTextSafe::Safe( pFTInfo->s_szName ), &ti );	
	//////////////////////////////////////////////////////////

	// 아이콘		
	_AddItemIcon( static_cast<int>(m_nMainID) );
	_AddLine( false );

#ifndef _GIVE
	if( m_pData != NULL )
	{
		pString = NiNew cString;
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color =  NiColor( 0, 1, 0 );
		
		CsItem* pAccItem = nsCsFileTable::g_pItemMng->GetItem( ( (cItemInfo*)m_pData )->GetType() );
		if( pAccItem == NULL )
			return;
		CsItem::sINFO* pFTAcc = pAccItem->GetInfo();
		if( pFTAcc == NULL )
			return;

		if( pFTAcc->IsAccessory() )//악세서리 일 때
		{
			std::wstring wsText;
			DmCS::StringFn::Format( wsText, L"%s - %s", UISTRING_TEXT( "COMMON_TXT_ACCESSORY" ).c_str(), UISTRING_TEXT( "COMMON_TXT_DIGITARY_POWER" ).c_str() );
			ti.SetText( wsText.c_str() );
		}
		else
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_TEST_ATTRIBUTE" ).c_str() );
		pString->AddText( &ti );

		_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d%%" ), ( (cItemInfo*)m_pData )->GetSocketRate() );
		ti.SetText( sz );
		pString->AddText( &ti );

		m_StringList.AddTail( pString );
	}

	pString = NiNew cString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color =  NiColor( 0, 1, 0 );
	ti.SetText( _DEBUG_MSG( "Item ID = " ) );
	pString->AddText( &ti );

	_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d" ), m_nMainID );
	ti.SetText( sz );
	pString->AddText( &ti );

	m_StringList.AddTail( pString );

#endif

	//=================================================================================================================
	//
	//		내용
	//
	//=================================================================================================================	

	if( m_pData != NULL )
	{
		CsItem* pAccItem = nsCsFileTable::g_pItemMng->GetItem( ( (cItemInfo*)m_pData )->GetType() );
		CsItem::sINFO* pFTAcc = ( pAccItem != NULL ) ? pAccItem->GetInfo() : NULL;
		if( pFTAcc != NULL && pFTAcc->IsAccessory() && ( (cItemInfo*)m_pData )->m_nRate != 0 )//악세서리 일 때 && 감정된 놈일 때
		{
			pString = NiNew cString;
			ti.s_eFontSize = CFont::FS_14;
			ti.s_Color = FONT_WHITE;

			ti.SetText( UISTRING_TEXT( "COMMON_TXT_DIGITARY_POWER" ).c_str() );
			//디지터리 수치
			int nRate = ( (cItemInfo*)m_pData )->GetSocketRate();
			if( nRate < 100 )
				pString->TailAddSizeX( 3 );
			_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( " %d%%" ), nRate );

			ti.AddText( sz );
			pString->AddText( &ti );
			int nStrSize = TOOLTIP_CUT_SIZE - pString->GetMaxSize().x; // 우측정렬
			pString->HeadAddSizeX( nStrSize );

			m_StringList.AddTail( pString );
		}
	}

	// 귀속 여부 표시
	switch( m_nSubID_1 )
	{
		// 가진상태의 귀속 여부 체크
	case cBaseWindow::WT_INVENTORY:
	case cBaseWindow::WT_WAREHOUSE:
#ifdef LJW_EQUIPSYSTEM_190724
	case cBaseWindow::WT_EQUIPMENT_SYSTEM:
#else
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
	case cBaseWindow::WT_ELEMENTITEM_CHECK:
#endif
	case cBaseWindow::WT_NEW_TAMERSTATUS:
	case cBaseWindow::WT_TRADE:
	case cBaseWindow::WT_MAKE_TACTICS:
	case cBaseWindow::WT_ENCHANT_TACTICS:
	case cBaseWindow::WT_CARDIDENTIFY:
	case cBaseWindow::WT_CARDRESEAL:
	case cBaseWindow::WT_DIGITAMA_SELL:
	case cBaseWindow::WT_MAIL:
		{
			// 귀속 가능한 아이템
			if( pFTInfo->s_nBelonging != 0 )
			{				
				if( m_pData && ( ( (cItemInfo*)m_pData )->m_nLimited == 0 ) )
				{
					pString = NiNew cString;
					ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
					ti.s_Color =  NiColor( 0, 1, 0 );
					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_USE_AFTER_BOUNDED" ).c_str() );
					pString->AddText( &ti );
					m_StringList.AddTail( pString );
				}
				else
				{
					pString = NiNew cString;
					ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
					ti.s_Color =  TOOLTIP_CANNOT_COLOR;
					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_CHARACTER_BOUND" ).c_str() );
					pString->AddText( &ti );
					m_StringList.AddTail( pString );
				}
			}			
		}
		break;

		// 사전 귀속 여부 체크
	case cBaseWindow::WT_PCBANGINVEN:
	case cBaseWindow::WT_STORE:	
	case cBaseWindow::WT_DIGITAMA_SCAN:
	case cBaseWindow::WT_QUEST_REV:
	case cBaseWindow::WT_QUEST_COMP:
	case cBaseWindow::WT_QUEST_DISCRIPT:
	//case cBaseWindow::WT_CASH:
	case cBaseWindow::WT_NEW_CASH_WAREHOUSE:
	case cBaseWindow::WT_PERSONSTORE:
	case cBaseWindow::WT_ITEM_SEPARATE:
	case cBaseWindow::WT_UNIONSTORE:
	case cBaseWindow::WT_ITEM_PRODUCTION_SHOP:
	case cBaseWindow::WT_NEW_GOTCHA_MACHINE:
	case cBaseWindow::WT_MASTERS_MATCHING:
	case cBaseWindow::WT_ENCHANT_OPTION:
	case cBaseWindow::WT_DIGIMON_INCUBATOR:
	case cBaseWindow::WT_SPIRIT_EVOLUTION:
		{
			switch( pFTInfo->s_nBelonging )
			{
			case 0:
				break;
			case 1:
				pString = NiNew cString;
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color =  NiColor( 0, 1, 0 );
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_USE_AFTER_BOUNDED" ).c_str() );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );
				break;
			case 2:
				pString = NiNew cString;
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color =  NiColor( 0, 1, 0 );
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_OBTAIN_AFTER_BOUNDED" ).c_str() );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	case cBaseWindow::WT_QUICKSLOT:
	case cBaseWindow::WT_NONE:
		break;
	default:
		break;
		//assert_cs( pFTInfo->s_nBelonging == 0 );
	}

	// 종류
	pString = NiNew cString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color = FONT_GOLD;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_KIND" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color = FONT_WHITE;	
	ti.SetText( UiTextSafe::Safe( pFTInfo->s_szTypeComment ) );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	// 적용 시간
	if( m_pData != NULL )
	{
		cItemInfo* pInfo = (cItemInfo*)m_pData;
		switch( m_nSubID_1 )
		{
		//case cBaseWindow::WT_CASH:
		case cBaseWindow::WT_NEW_CASH_WAREHOUSE:
			{			
				CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() );
				if( pItem == NULL || pItem->GetInfo() == NULL )
					break;
				CsItem::sINFO* pFTItem = pItem->GetInfo();
				
				if( cItemInfo::IsUseItemTime( pFTInfo ) )
				{
					pString = NiNew cString;
					ti.s_Color = FONT_GOLD;	
					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REMAINING" ).c_str() );
					pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

					ti.SetText( cTooltip::Time2Str( pFTInfo->s_nUseTime_Min*60, 0, TT_SIMPLE ) );
					pString->AddText( &ti );

					m_StringList.AddTail( pString );
				}
				else
				{
					if( pFTInfo->s_nUseTime_Min != 0 )
					{
						pString = NiNew cString;
						ti.s_Color = FONT_GOLD;	
						ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REMAINING" ).c_str() );
						pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
						
						ti.SetText( cTooltip::Time2Str( pFTInfo->s_nUseTime_Min*60, 0, TT_FULL ) );	
						pString->AddText( &ti );

						m_StringList.AddTail( pString );	
					}
				}				
			}
			break;
		default:			
			if(	cItemInfo::IsUseItemTime( pFTInfo ) )
			{
				pString = NiNew cString;
				ti.s_Color = FONT_GOLD;	
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REMAINING" ).c_str() );

#ifdef ITEM_USE_TIME_PASS
				if(pFTInfo->s_bUseTimeType == 1)	//시간제아이템일 경우
				{
					if(pInfo->m_nEndTime == 0xFFFFFFF0 )
					{
						ti.SetText( UISTRING_TEXT( "TOOLTIP_UNUSED" ).c_str() );
					}

					pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;					

					if(pInfo->m_nEndTime != 0xFFFFFFF0)
					{
						ti.s_Color = FONT_WHITE;
						ti.SetText( _T( " " ) );
						sUPDATE_TIME ut;
						ut.s_pText = pString->AddText( &ti );
						ut.s_nTimeTS = pInfo->m_nEndTime;
						m_vpEditElement.PushBack( ut );					
					}	
				}
				else
				{
					pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

					ti.s_Color = FONT_WHITE;
					ti.SetText( _T( " " ) );
					sUPDATE_TIME ut;
					ut.s_pText = pString->AddText( &ti );
					ut.s_nTimeTS = pInfo->m_nEndTime;
					m_vpEditElement.PushBack( ut );
				}
#else
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REMAINING" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				ti.s_Color = FONT_WHITE;
				ti.SetText( _T( " " ) );
				sUPDATE_TIME ut;
				ut.s_pText = pString->AddText( &ti );
				ut.s_nTimeTS = pInfo->m_nEndTime;
				m_vpEditElement.PushBack( ut );
#endif

				m_StringList.AddTail( pString );
			}
			else
			{
				if( pFTInfo->s_nUseTime_Min != 0 )
				{
					pString = NiNew cString;
					ti.s_Color = FONT_GOLD;	
					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REMAINING" ).c_str() );
					pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

					ti.s_Color = FONT_WHITE;
					ti.SetText( cTooltip::Time2Str( pFTInfo->s_nUseTime_Min*60, 0, TT_FULL ) );	
					pString->AddText( &ti );

					m_StringList.AddTail( pString );	
				}			
			}
			break;
		}
	}

	// 등급
	pString = NiNew cString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color = FONT_GOLD;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_ITEM_CLASS" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color = FONT_WHITE;
	switch( pFTInfo->s_nClass )
	{
	case 1:		ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_MISC_ITEM" ).c_str() );		break;
	case 2:		ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_NORMAL_ITEM" ).c_str() );			break;
	case 3:		ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_MAGIC_ITEM" ).c_str() );			break;
	case 4:		ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_RARE_ITEM" ).c_str() );			break;
	case 5:		ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_EPIC_ITEM" ).c_str() );			break;
	case 6:		ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_UNIQUE_ITEM" ).c_str() );			break;
	case 7:		ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_LEGEND_ITEM" ).c_str() );			break;

	case 9:		ti.SetText( UISTRING_TEXT( "COMMON_TXT_EVENT_ITEM" ).c_str() );			break;
	case 10:
		{
			if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_AERIA_POINT" ).c_str() );
			else
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_CASH_ITEM" ).c_str() );
		}
		break;
	case 11:	ti.SetText( _T( "Admin Item" ) );				break;
	case 12:    ti.SetText( _ONLY_KOR( "PC방 전용아이템") );	break;

	case 13:	ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_CASH_ITEM_SECOND" ).c_str() );			break;

	default:	assert_cs( false );
	}
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	// 대상
	switch( pFTInfo->s_nType_L )
	{
	case nItem::Potion:
	case nItem::Portal:
		{
			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_TARGET" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_WHITE;
			// 대상
			switch( pFTInfo->s_nUseCharacter )
			{
			case 0:
			case 1:			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_APPLY_TO_ALL" ).c_str() );		break;
			case 2:			ti.SetText( UISTRING_TEXT( "COMMON_TXT_DIGIMON" ).c_str() );			break;
			case 3:			ti.SetText( UISTRING_TEXT( "COMMON_TXT_TAMER" ).c_str() );			break;
			default:		assert_cs( false );
			}
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;
	}

	// 제한Lv
	switch( pFTInfo->s_nType_L )
	{
		// 칩셋은 레벨 검사 안함
	case nItem::Chipset:
		break;
		// 장비 아이템이면서 케쉬템은 레벨 검사 안함
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	case nItem::Costume:
	case nItem::Glass:
	case nItem::Earring:
	case nItem::Necklace:
	case nItem::Ring:
#ifdef SDM_TAMER_EQUIP_ADD_BRACELET_20181031
	case nItem::Bracelet:
#endif
	case nItem::EquipAura: //오라 추가 chu8820
		if( pFTInfo->IsCashType() )
			break;
		// 아니라면 레벨 체크
	default:
		{
			int nTamerLimitLevel_Min = 0;			int nTamerLimitLevel_Max = 0;
			int nDigimonLimitLevel_Min = 0;			int nDigimonLimitLevel_Max = 0;
			bool bEnableTamerLv = false;			bool bEnableDigimonLv = false;
			int nTLv = 1;
			int nDLv = 1;
			if( g_pCharMng && g_pCharMng->GetTamerUser() && g_pCharMng->GetTamerUser()->GetBaseStat() )
				nTLv = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();
			if( g_pCharMng && g_pCharMng->GetDigimonUser( 0 ) && g_pCharMng->GetDigimonUser( 0 )->GetBaseStat() )
				nDLv = g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetLevel();
			if( m_pData != NULL )
			{
				cItemInfo* pInfo = (cItemInfo*)m_pData;
				pInfo->GetTamerReqLevel( nTamerLimitLevel_Min, nTamerLimitLevel_Max );
				pInfo->GetDigimonReqLevel( nDigimonLimitLevel_Min, nDigimonLimitLevel_Max );
				bEnableTamerLv = pInfo->IsEnableTamerLv(nTLv);
				bEnableDigimonLv = pInfo->IsEnableDigimonLv(nDLv);
			}
			else
			{
				pFTInfo->GetTamerReqLv( nTamerLimitLevel_Min, nTamerLimitLevel_Max );
				pFTInfo->GetDigimonReqLv( nDigimonLimitLevel_Min, nDigimonLimitLevel_Max );
				bEnableTamerLv = pFTInfo->IsEnableTamerLv(nTLv);
				bEnableDigimonLv = pFTInfo->IsEnableTamerLv(nDLv);
			}


			if( 1 < nTamerLimitLevel_Min || 1 < nTamerLimitLevel_Max )
			{
				pString = NiNew cString;
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color = FONT_GOLD;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_TAMER_LIMIT_LV" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color = bEnableTamerLv == true ? FONT_WHITE : TOOLTIP_CANNOT_COLOR;
				if( nTamerLimitLevel_Min == nTamerLimitLevel_Max )
				{
					ti.SetText( nTamerLimitLevel_Min );
				}
				else
				{
					std::wstring msg;
					DmCS::StringFn::Format( msg, L"%d ~ %d", nTamerLimitLevel_Min, nTamerLimitLevel_Max );
					ti.SetText( msg.c_str() );
				}
				
				pString->AddText( &ti );
				m_StringList.AddTail( pString );
			}

			if( 1 < nDigimonLimitLevel_Min || 1 < nDigimonLimitLevel_Max )
			{
				pString = NiNew cString;
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color = FONT_GOLD;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_DIGIMON_LIMITED_LV" ).c_str() );
#if ( defined VERSION_TW || defined VERSION_HK )
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 4;
#else
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
#endif

				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color = bEnableDigimonLv == true ? FONT_WHITE : TOOLTIP_CANNOT_COLOR;				

				if( nDigimonLimitLevel_Min == nDigimonLimitLevel_Max )
				{
					ti.SetText( nDigimonLimitLevel_Min );
				} 
				else
				{
					std::wstring msg;
					DmCS::StringFn::Format( msg, L"%d ~ %d", nDigimonLimitLevel_Min, nDigimonLimitLevel_Max );
					ti.SetText( msg.c_str() );
				}

				pString->AddText( &ti );
				m_StringList.AddTail( pString );
			}
		}
	}

	if( pFTInfo->s_dwSkill )	// item 정보에 스킬 코드가 있어야 함.
	{
		// 습득 제한
		pString = NiNew cString;
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;

		switch (pFTInfo->s_nDoNotUseType)
		{
		case 0:
			{
				ti.s_Color = FONT_GOLD;
				ti.SetText(UISTRING_TEXT("TOOLTIP_ITEM_LIMIT_OBTAIN").c_str());
				pString->AddText(&ti)->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				ti.s_Color = FONT_WHITE;
				ti.SetText(UISTRING_TEXT("TOOLTIP_ITEM_NO_LIMIT_OBTAIN").c_str());
			}
			break;
		case 1:
			{
				ti.s_Color = FONT_GOLD;
				ti.SetText(UISTRING_TEXT("TOOLTIP_ITEM_LIMIT_OBTAIN").c_str());
				pString->AddText(&ti)->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				ti.s_Color = FONT_RED;
				std::wstring showMsg;
				if( nsCsFileTable::g_pBaseMng )
				{
					showMsg = nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_JOINTPROGRESSMODE);
#ifdef CROSSWARS_SYSTEM
					if( !showMsg.empty() )
						showMsg += L", ";
					showMsg += UISTRING_TEXT( "COMMON_TXT_XROS_WARS" );
#endif
#ifdef SDM_TAMER_XGUAGE_20180628
					if( !showMsg.empty() )
						showMsg += L", ";
					showMsg += nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_X_JOGRESS);

					if( !showMsg.empty() )
						showMsg += L", ";
					showMsg += nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_MIX);
#endif
				}

				ti.SetText( showMsg.c_str() );
			}
			break;
		case 2:
			{
				ti.s_Color = FONT_GOLD;
				ti.SetText(UISTRING_TEXT("TOOLTIP_ITEM_UNLIMIT_OBTAIN").c_str());
				pString->AddText(&ti)->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				ti.s_Color = FONT_GREEN;
				std::wstring showMsg;
				if( nsCsFileTable::g_pBaseMng )
				{
					showMsg = nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_JOINTPROGRESSMODE);
#ifdef SDM_TAMER_XGUAGE_20180628
					if( !showMsg.empty() )
						showMsg += L", ";
					showMsg += nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_X_JOGRESS);
					if( !showMsg.empty() )
						showMsg += L", ";
					showMsg += nsCsFileTable::g_pBaseMng->GetEvoTypeName(nsCsDigimonTable::ET_MIX);
#endif
				}
				ti.SetText( showMsg.c_str() );
			}
			break;
		default:
			assert_cs( false );
		}

		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}

	// 제한퀘스트
	if( pFTInfo->s_nQuestRequire )
	{
		bool bLimit = true;
		if( g_pDataMng && g_pDataMng->GetQuest() )
			bLimit = ( g_pDataMng->GetQuest()->IsCompleate( pFTInfo->s_nQuestRequire ) == false );
		CsQuest* pRequireQuest = nsCsFileTable::g_pQuestMng ? nsCsFileTable::g_pQuestMng->GetQuest( pFTInfo->s_nQuestRequire ) : NULL;
		if( pRequireQuest )
		{
			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_REQUIRE_QUEST" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = bLimit == false ? FONT_WHITE : TOOLTIP_CANNOT_COLOR;
			ti.SetText( UiTextSafe::Safe( pRequireQuest->m_szTitleText ) );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
	}

	// 갯수
	switch( pFTInfo->s_nType_L )
	{
	case nItem::Money:
		if( m_pData )
		{
			cItemInfo* pInfo = (cItemInfo*)m_pData;

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REWARDED_MONEY" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_WHITE;
			std::wstring wsMega;
			DmCS::StringFn::Format( wsMega, L"%d %s", pInfo->GetCount(), UISTRING_TEXT( "TOOLTIP_ITEM_MEGA" ).c_str() );
			ti.SetText( wsMega.c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;
	case nItem::GoldBar:
		if( m_pData )
		{
			cItemInfo* pInfo = (cItemInfo*)m_pData;

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REWARDED_MONEY" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_WHITE;
			
			if( pInfo->GetCount() < 1000 )
				_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d Bit" ), pInfo->GetCount() * pFTInfo->s_dwSale );
			else
				_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d Mega" ), (pInfo->GetCount() * pFTInfo->s_dwSale) / 1000 );

			ti.SetText( sz );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;
	default:
		if(  m_pData && ( pFTInfo->s_nOverlap > 1 ) )
		{
			cItemInfo* pInfo = (cItemInfo*)m_pData;

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_COUNT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_WHITE;
			ti.SetText( pInfo->GetCount() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
	}	

	// 사용제한
	switch( pFTInfo->s_nType_L )
	{
	case nItem::Potion:
	case nItem::Portal:
		{
			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_LIMITED_USE" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_WHITE;

			switch( pFTInfo->s_nUseMode )
			{
			case 0:			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_USABLE_AT_ALL_TIMES" ).c_str() );			break;
			case 1:			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_NOT_USABLE_DURING_BATTLE" ).c_str() );			break;
			case 2:			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_ONLY_USABLE_DURING_BATTLE" ).c_str() );		break;
			default:		assert_cs( false );
			}
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;
	}

	// 분석 여부 표시
	switch( pFTInfo->s_nType_L )
	{
	case nItem::EItem_All:
	case nItem::EItem_Glass:			
	case nItem::EItem_Coat:			
	case nItem::EItem_Glove:			
	case nItem::EItem_Pants:			
	case nItem::EItem_Shoes:
		switch( m_nSubID_1 )
		{
			// 가진상태의 분석 여부 체크
		case cBaseWindow::WT_INVENTORY:
		case cBaseWindow::WT_WAREHOUSE:
#ifdef LJW_EQUIPSYSTEM_190724
		case cBaseWindow::WT_EQUIPMENT_SYSTEM:
#else
		case cBaseWindow::WT_ELEMENTITEM_ATTACH:
		case cBaseWindow::WT_ELEMENTITEM_CHECK:
#endif
			case cBaseWindow::WT_ITEM_SEPARATE:
			case cBaseWindow::WT_PERSONSTORE:
				{
					if( m_pData == NULL )
					{
						pString = NiNew cString;
						ti.s_eFontSize = CFont::FS_10;
						ti.s_Color = TOOLTIP_CANNOT_COLOR;
						ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_MERGE_NOT_POSSIBLE" ).c_str() );
						pString->AddText( &ti );
						m_StringList.AddTail( pString );
						break;
					}

					cItemInfo* pInfo = (cItemInfo*)m_pData;
					if( pInfo->m_nRate == 0 )
					{
					pString = NiNew cString;
					ti.s_eFontSize = CFont::FS_10;
					ti.s_Color = TOOLTIP_CANNOT_COLOR;
					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_MERGE_NOT_POSSIBLE" ).c_str() );
					pString->AddText( &ti );
					m_StringList.AddTail( pString );
				}
				else
				{
					pString = NiNew cString;
					ti.s_eFontSize = CFont::FS_10;
					ti.s_Color = NiColor( 0, 1, 0 );
					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_MERGE_POSSIBLE" ).c_str() );
					pString->AddText( &ti );
					m_StringList.AddTail( pString );
				}
			}
			break;

		default:
			pString = NiNew cString;
			ti.s_eFontSize = CFont::FS_10;
			ti.s_Color = TOOLTIP_CANNOT_COLOR;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_MERGE_NOT_POSSIBLE" ).c_str() );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;
	case nItem::Ring:
		{
			if( m_pData != NULL )
			{			
				if( ( (cItemInfo*)m_pData )->m_nRate == 0 )
				{
					pString = NiNew cString;
					ti.s_eFontSize = CFont::FS_10;
					ti.s_Color = TOOLTIP_CANNOT_COLOR;
	// 				pString->HeadAddSizeX( TOOLTIP_TAB_SIZE*3 );
					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_OPTION_UNIDENTIFIED" ).c_str() );
					pString->AddText( &ti );
					m_StringList.AddTail( pString );
				}
				else
				{
					pString = NiNew cString;
					ti.s_eFontSize = CFont::FS_10;
					ti.s_Color = NiColor( 0, 1, 0 );

					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_OPTION_IDENTIFIED" ).c_str() );
					pString->AddText( &ti );
					m_StringList.AddTail( pString );
				}
			}
		}
		break;
	}

	//=================================================================================================================
	//
	//		재사용 시간 & 남은 시간
	//
	//=================================================================================================================
	if( ( pFTInfo->s_nUseTimeGroup > 0 )&&( pFTInfo->s_nUseTimeGroup <= 32 ) )
	{
		_AddLine( true );

		pString = NiNew cString;
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "TOOLTIP_COOL_TIME" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

		ti.s_Color = FONT_WHITE;
		CsCoolTimeSeq* pSeq = pFTInfo->GetCoolTimeSeq();
		ti.SetText( cTooltip::Time2Str( (int)pSeq->GetEndTime(), 0, TT_FULL ) );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );

		if( pSeq->IsCoolTime() )
		{
			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_TIME_REMAINING" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_Color = FONT_WHITE;
			ti.SetText( _T( " " ) );
			sUPDATE_TIME ut;
			ut.s_pText = pString->AddText( &ti );
			ut.s_nTimeTS = (int)pSeq->GetRemainTime() + _TIME_TS;
			m_vpEditElement.PushBack( ut );

			m_StringList.AddTail( pString );
		}
	}

	//=================================================================================================================
	//
	//		칩셋
	//
	//=================================================================================================================
	// 칩셋일 경우 - 적용 가능한지
	switch( pFTInfo->s_nType_L )
	{
	case nItem::Chipset:
		int nFamily[ DIGIMON_MAX_FAMILY ];
		SAFE_POINTER_RET( g_pCharMng );
		SAFE_POINTER_RET( g_pCharMng->GetDigimonUser( 0 ) );
		SAFE_POINTER_RET( g_pCharMng->GetDigimonUser( 0 )->GetFTDigimon() );
		SAFE_POINTER_RET( g_pCharMng->GetDigimonUser( 0 )->GetFTDigimon()->GetInfo() );
		for( int i=0; i<DIGIMON_MAX_FAMILY; ++i )
		{			
			nFamily[ i ] = g_pCharMng->GetDigimonUser( 0 )->GetFTDigimon()->GetInfo()->s_eFamilyType[ i ];
		}

		CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTInfo->s_dwSkill );
		if( pSkill == NULL || pSkill->GetInfo() == NULL )
			return;
		CsSkill::sINFO* pFTSkill = pSkill->GetInfo();
		assert_cs( pFTSkill->s_nFamilyType != 0 );

		_AddLine( true );

		bool bSameFamily = false;
		for( int i=0; i<DIGIMON_MAX_FAMILY; ++i )
		{
			if( ( nFamily[ i ] == pFTSkill->s_nFamilyType )||( pFTSkill->s_nFamilyType == 17 ) )
			{
				bSameFamily = true;
				break;
			}
		}

		pString = NiNew cString;
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_CHIPSET_ATTRIBUTE" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		pString->AddIcon( CsPoint( 21, 19 ), ICONITEM::DAttribute, cIconMng::Family, pFTSkill->s_nFamilyType - nsCsDigimonTable::FT_DR, CsPoint( 0, -3 ) );
		m_StringList.AddTail( pString );

		pString = NiNew cString;
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_PARTNER_ATTRIBUTE" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
		for( int i=0; i<DIGIMON_MAX_FAMILY; ++i )
		{
			if( nFamily[ i ] != 0 )
			{
				pString->AddIcon( CsPoint( 21, 19 ), ICONITEM::DAttribute, cIconMng::Family, nFamily[ i ] - nsCsDigimonTable::FT_DR, CsPoint( 0, -3 ) );
			}
		}
		m_StringList.AddTail( pString );

		int nTamerLimitLevel_Min = 0;			int nTamerLimitLevel_Max = 0;
		int nDigimonLimitLevel_Min = 0;			int nDigimonLimitLevel_Max = 0;
		bool bEnableTamerLv = false;			bool bEnableDigimonLv = false;
		int nTLv = 1;
		int nDLv = 1;
		if( g_pCharMng && g_pCharMng->GetTamerUser() && g_pCharMng->GetTamerUser()->GetBaseStat() )
			nTLv = g_pCharMng->GetTamerUser()->GetBaseStat()->GetLevel();
		if( g_pCharMng && g_pCharMng->GetDigimonUser( 0 ) && g_pCharMng->GetDigimonUser( 0 )->GetBaseStat() )
			nDLv = g_pCharMng->GetDigimonUser( 0 )->GetBaseStat()->GetLevel();
	
		pFTInfo->GetTamerReqLv( nTamerLimitLevel_Min, nTamerLimitLevel_Max );
		pFTInfo->GetDigimonReqLv( nDigimonLimitLevel_Min, nDigimonLimitLevel_Max );
		bEnableTamerLv = pFTInfo->IsEnableTamerLv(nTLv);
		bEnableDigimonLv = pFTInfo->IsEnableTamerLv(nDLv);

		bool bEnableLevel = ( bEnableTamerLv && bEnableDigimonLv ) ? true : false;
		if( 1 < nTamerLimitLevel_Min || 1 < nTamerLimitLevel_Max )
		{
			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_TAMER_LIMIT_LV" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
			ti.s_Color = bEnableTamerLv ? FONT_WHITE : TOOLTIP_CANNOT_COLOR;
			if( nTamerLimitLevel_Min == nTamerLimitLevel_Max )
			{
				ti.SetText( nTamerLimitLevel_Min );
			}
			else
			{
				std::wstring msg;
				DmCS::StringFn::Format( msg, L"%d ~ %d", nTamerLimitLevel_Min, nTamerLimitLevel_Max );
				ti.SetText( msg.c_str() );
			}

			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}

		if( 1 < nDigimonLimitLevel_Min || 1 < nDigimonLimitLevel_Max )
		{
			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_DIGIMON_LIMITED_LV" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
			if( nDigimonLimitLevel_Min == nDigimonLimitLevel_Max )
			{
				ti.SetText( nDigimonLimitLevel_Min );
			}
			else
			{
				std::wstring msg;
				DmCS::StringFn::Format( msg, L"%d ~ %d", nDigimonLimitLevel_Min, nDigimonLimitLevel_Max );
				ti.SetText( msg.c_str() );
			}

			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}

		pString = NiNew cString;
		if( bSameFamily && bEnableLevel )
		{				
			ti.s_Color =  NiColor( 0, 1, 0 );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_ABLE_TO_APPLY_CHIPSET_ABILITY" ).c_str() );
		}
		else
		{
			ti.s_Color =  TOOLTIP_CANNOT_COLOR;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_UNABLE_TO_APPLY_CHIPSET_ABILITY" ).c_str() );				
		}
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
		break;
	}

	//=================================================================================================================
	//
	//		능력치
	//
	//=================================================================================================================
	if( pFTInfo->s_dwSkill != 0 && pFTInfo->s_nType_L != nItem::Item_TamerSKillConsume )
	{
#ifdef CASH_ITEM_TOOLTIP
		bool bSkillApply = false;
		if( !pFTInfo->IsAccessory() && !pFTInfo->IsAccStone() )	// 악세사리가 아니라면..
		{
			CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTInfo->s_dwSkill );
			if( pSkill == NULL || pSkill->GetInfo() == NULL )
				break;
			CsSkill::sINFO* pFTSkill = pSkill->GetInfo();
			for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
			{
				if( pFTSkill->s_Apply[ i ].s_nID == 0 )
					continue;

				if( pFTSkill->s_Apply[i].s_nA == 28 )
					bSkillApply = true;
			}
		}

		if( bSkillApply == false )
		{
			if( pFTInfo->s_nType_L != nItem::DigimonSkillChipATK && 
				pFTInfo->s_nType_L != nItem::DigimonSkillChipDEF && 
				pFTInfo->s_nType_L != nItem::DigimonSkillChipAST )	// 메모리 스킬관련 아이템이 아니라면
				_AddLine( true );

			// 케쉬샾 아닐때
			switch( m_nSubID_1 )
			{
			//case cBaseWindow::WT_CASH:
			case cBaseWindow::WT_NEW_CASH_WAREHOUSE:
				g_pStringAnalysis->ItemComment_Parcing( &m_StringList, pFTInfo->s_dwItemID );
				break;
			default:
				switch( pFTInfo->s_btCodeTag )
				{
				case 0:
				case 1:
				case 2:
					{
						if( m_pData != NULL )
							g_pStringAnalysis->ItemComment_Parcing( &m_StringList, (cItemInfo*)m_pData );
						else
							g_pStringAnalysis->ItemComment_Parcing( &m_StringList, pFTInfo->s_dwItemID );
					}
					break;
				case 3:
					g_pStringAnalysis->ItemComment_Parcing( &m_StringList, pFTInfo->s_dwItemID );
					break;
				default:
					assert_cs( false );
				}
			}
		}
#else

		if( pFTInfo->IsAccessory() || pFTInfo->IsAccStone() )	// 악세사리 및 스톤 타입 유형은 스킬 코드를 따로 사용하기 때문에 제외.
		{
			_AddLine( true );
		}
		else
		{
			CsSkill* pTempSkill = nsCsFileTable::g_pSkillMng->GetSkill( pFTInfo->s_dwSkill );
			if( pTempSkill && pTempSkill->GetInfo() && !pTempSkill->GetInfo()->s_nMemorySkill )
				_AddLine( true );
		}

		// 케쉬샾 아닐때
		switch( m_nSubID_1 )
		{
		//case cBaseWindow::WT_CASH:
		case cBaseWindow::WT_NEW_CASH_WAREHOUSE:
			g_pStringAnalysis->ItemComment_Parcing( &m_StringList, pFTInfo->s_dwItemID );
			break;
		default:
			switch( pFTInfo->s_btCodeTag )
			{
			case 0:
			case 1:	// 코드 태그 1 , 소분류 6230 > 업적 달성 아이템
				{
					if( pFTInfo->s_nType_L == nItem::NoQuick_Consume 
						&& pFTInfo->s_nType_S == 30 )
					{
						pString = NiNew cString;
						ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
						ti.s_Color = FONT_WHITE;

						_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%s" ), pFTInfo->s_szName );

						ti.SetText( sz );
						pString->AddText( &ti );
						m_StringList.AddTail( pString );
					}
				}//break;	< 0,1,2 가 같이 묶여있었기 때문에 업적 아이템만 스트링을 추가해줍니다.
			case 2:
				{
					if( m_pData != NULL )
						g_pStringAnalysis->ItemComment_Parcing( &m_StringList, (cItemInfo*)m_pData );
					else
						g_pStringAnalysis->ItemComment_Parcing( &m_StringList, pFTInfo->s_dwItemID );
				}
				break;
			case 3:
				g_pStringAnalysis->ItemComment_Parcing( &m_StringList, pFTInfo->s_dwItemID );
				break;
			default:
				assert_cs( false );
			}
		}	
#endif
	}

	//=================================================================================================================
	//
	//		소켓
	//
	//=================================================================================================================
	switch( pFTInfo->s_nType_L )
	{
	case nItem::Digivice:
		{
			assert_cs( pFTInfo->s_nDigiviceSkillSlot > 0 );
			assert_cs( pFTInfo->s_nDigiviceChipsetSlot > 0 );

			_AddLine( true );

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;			
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_SKILL_SLOT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_Color = FONT_WHITE;
			ti.SetText( pFTInfo->s_nDigiviceSkillSlot );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );


			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;			
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_CHIPSET_SLOT" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			ti.s_Color = FONT_WHITE;
			ti.SetText( pFTInfo->s_nDigiviceChipsetSlot );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;
	case nItem::Head:
	case nItem::Coat:
	case nItem::Glove:			
	case nItem::Pants:			
	case nItem::Shoes:
	case nItem::Glass:
	case nItem::EquipAura: //오라 추가 chu8820
		{
			if( pFTInfo->s_nSocketCount > 0 )
			{
				// 현재 소지하고 있는 아이템이라면
				if( m_pData != NULL )
				{
					int nRate = ( (cItemInfo*)m_pData )->GetSocketRate();
					_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d%%" ), nRate );
				}
				else
				{
					_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d%%" ), cItemInfo::GetSocketRate( pFTInfo->s_dwItemID ) );
				}

				_AddLine( true );

				pString = NiNew cString;
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color = FONT_GOLD;
				ti.SetText( UISTRING_TEXT( "COMMON_TXT_APPLY_ATTRIBUTE" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;

				bool bCashEnd = false;
				if( m_pData )
				{
					if( ((cItemInfo*)m_pData)->IsCashEndTime( pFTInfo ) )
						bCashEnd = true;
				}

				if( bCashEnd )
					ti.s_Color = TOOLTIP_CANNOT_COLOR;
				else
					ti.s_Color = FONT_WHITE;
				
				
				ti.SetText( sz );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );

				if( m_pData != NULL )
				{
					cItemInfo* pInfo = (cItemInfo*)m_pData;
					for( int i=0; i<pFTInfo->s_nSocketCount; ++i )
					{
						pString = NiNew cString;
						ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
						ti.s_Color = FONT_GOLD;
						std::wstring wsSlot;
						DmCS::StringFn::Format( wsSlot, L"%s %d", UISTRING_TEXT( "OPTION_MACRO_SLOT" ).c_str(), i + 1 );
						ti.SetText( wsSlot.c_str() );
						pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

						// 빈소켓
						if( pInfo->m_nSockItemType[ i ] == 0 )
						{
							ti.s_eFontSize = CFont::FS_10;
							ti.s_Color = NiColor( 0.6f, 0.6f, 0.6f );

							pString->AddSprite( m_pEmptySlot )->s_ptSize.x = 18;

							ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_EMPTY_SLOT" ).c_str() );
							pString->AddText( &ti );
							m_StringList.AddTail( pString );
						}
						else
						{
							cString::sICON* pIcon = pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::ARMOR_1, pInfo->m_nSockItemType[ i ], 1 );
							pIcon->s_ptSize = CsPoint( 18, 12 );
							pIcon->s_nIndex_3 = 1;
							g_pStringAnalysis->Equip_EItemComment_Parcing( &m_StringList, pString, pInfo, i, bCashEnd );
						}					
					}
				}
			}			
		}
		break;
	}


	//=================================================================================================================
	//
	//		코멘트
	//
	//=================================================================================================================

	if( pFTInfo->s_szComment && pFTInfo->s_szComment[ 0 ] )
	{
		_AddLine( true );

		TCHAR szComment[ ITEM_COMMENT_LEN ];
		_tcscpy_s( szComment, ITEM_COMMENT_LEN, pFTInfo->s_szComment );	
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color = FONT_WHITE;
		g_pStringAnalysis->Cut_Parcing( &m_StringList, TOOLTIP_CUT_SIZE, szComment, &ti );
	}

	//=================================================================================================================
	//
	//		회수 처리
	//
	//=================================================================================================================
	switch( pFTInfo->s_nType_L )
	{
	case nItem::Digitama1:	
		{
			_AddLine( true );

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REFUND_COST" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
			int nMaxY = pString->GetMaxSize().y;
			pString->AddMoneyIF( cMoneyIF::DISP_SIMPLE_1, pFTInfo->s_dwSale, FONT_WHITE, TOOLTIP_FONT_1_SIZE, DT_LEFT );
			pString->SetSizeY( nMaxY );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_SCAN_COST" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_WHITE;
			ti.SetText( _T( "X" ) );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );
		}
		break;
	case nItem::Digitama2:
		{
			_AddLine( true );

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REFUND_COST" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
			int nMaxY = pString->GetMaxSize().y;
			pString->AddMoneyIF( cMoneyIF::DISP_SIMPLE_1, pFTInfo->s_dwSale, FONT_WHITE, TOOLTIP_FONT_1_SIZE, DT_LEFT );
			pString->SetSizeY( nMaxY );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_SCAN_COST" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			pString->AddMoneyIF( cMoneyIF::DISP_SIMPLE_1, pFTInfo->s_dwScanPrice, FONT_WHITE, TOOLTIP_FONT_1_SIZE, DT_LEFT );
			pString->SetSizeY( nMaxY );
			m_StringList.AddTail( pString );
		}
		break;
	case nItem::Digitama3:
		{
			_AddLine( true );

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REFUND_COST" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
			int nMaxY = pString->GetMaxSize().y;
			pString->AddMoneyIF( cMoneyIF::DISP_SIMPLE_1, pFTInfo->s_dwSale, FONT_WHITE, TOOLTIP_FONT_1_SIZE, DT_LEFT );
			pString->SetSizeY( nMaxY );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_SCAN_COST" ).c_str() );
			pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

			pString->AddMoneyIF( cMoneyIF::DISP_SIMPLE_1, pFTInfo->s_dwScanPrice, FONT_WHITE, TOOLTIP_FONT_1_SIZE, DT_LEFT );
			pString->SetSizeY( nMaxY );

			m_StringList.AddTail( pString );

			_AddLine( true );

			CsTactics* pTactics = NULL;
			if( nsCsFileTable::g_pTacticsMng->IsTactics( static_cast<int>(m_nMainID) ) == true )
			{
				pTactics = nsCsFileTable::g_pTacticsMng->GetTactics( static_cast<int>(m_nMainID) );
				SAFE_POINTER_BEK(pTactics);

 				int nTypeL, nTypeS;
 
 				CsItem::TypeS_to_TypeLS( pTactics->GetInfo()->s_nReqItemS_Type[m_nSubID_2], nTypeL, nTypeS );
 
 				int nDispItemID = nsCsFileTable::g_pItemMng->TypeT_to_Disp( pTactics->GetInfo()->s_nReqItemS_Type[m_nSubID_2] );
 				int nCount = pTactics->GetInfo()->s_nReqItemCount[m_nSubID_2];

				pString = NiNew cString;

				// 필요 아이템
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color = FONT_GOLD;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REQUIRE_DATA" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;				

				ti.s_Color = FONT_WHITE;
				CsItem* pReqItem = nsCsFileTable::g_pItemMng->GetItem( nDispItemID );
				if( pReqItem == NULL || pReqItem->GetInfo() == NULL )
					break;
				ti.SetText( UiTextSafe::Safe( pReqItem->GetInfo()->s_szName ) );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );

				// 필요 갯수
				pString = NiNew cString;
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color = FONT_GOLD;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_REQUIRE_QUANTITY" ).c_str() );
				pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

				int nHaveCount = g_pDataMng->GetInven()->GetCount_Item_TypeLS( nTypeL, nTypeS );

				if( nCount <= nHaveCount )
				{
					ti.s_Color = FONT_WHITE;			
					ti.SetText( nCount );
					pString->AddText( &ti );

					ti.s_Color = NiColor( 0, 1, 0 );
					std::wstring wsEnableCount = UISTRING_TEXT( "TOOLTIP_ITEM_ENABLE_COUNT" );
					DmCS::StringFn::Replace( wsEnableCount, L"#Count#", nHaveCount/nCount );
					ti.SetText( wsEnableCount.c_str() );
					pString->AddText( &ti );

					m_StringList.AddTail( pString );
				}
				else
				{
					ti.s_Color = NiColor( 1, 0, 0 );
					std::wstring wsDisableCount;
					DmCS::StringFn::Format( wsDisableCount, L"%d (%s)", nCount, UISTRING_TEXT( "TOOLTIP_ITEM_INSUFFICIENT_INGREDIENT" ).c_str() );
					ti.SetText( wsDisableCount.c_str() );
					pString->AddText( &ti );
					m_StringList.AddTail( pString );
				}
			}
			// 오류 : DM_Evolution_Digitama.xls 에 해당 알의 정보가 없음
			else
			{
				ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
				ti.s_Color = FONT_RED;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_NOT_POSSIBLE_HATCH" ).c_str() );

				pString = NiNew cString;
				pString->AddText( &ti );
				m_StringList.AddTail( pString );
			}
		}
		break;
	}

	//=================================================================================================================
	//
	//		부가 처리
	//
	//=================================================================================================================

	switch( m_nSubID_1 )
	{
	case cBaseWindow::WT_INVENTORY:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_STORE ) )
			{
				_AddLine( true );

				if( ( pFTInfo->s_dwSale > 0)&&( pFTInfo->s_nType_L/10 != 9 ) )
				{
					pString = NiNew cString;
					ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
					ti.s_Color = FONT_GOLD;
					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_SELLING_PRICE" ).c_str() );
					pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

					pString->AddMoneyIF( cMoneyIF::DISP_SIMPLE_1, pFTInfo->s_dwSale, FONT_WHITE, TOOLTIP_FONT_1_SIZE, DT_LEFT );
					pString->AddSizeY( 6 );
					m_StringList.AddTail( pString );

					// 상점 설명
					ti.s_eFontSize = CFont::FS_10;
					ti.s_Color = NiColor( 0, 1, 0 );
					g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, UISTRING_TEXT( "TOOLTIP_ITEM_STORE_EXPLAIN" ).c_str(), &ti );
				}
				else
				{
					pString = NiNew cString;
					ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
					ti.s_Color = TOOLTIP_CANNOT_COLOR;
					ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_NOT_FOR_SALE" ).c_str() );
					pString->AddText( &ti );
					m_StringList.AddTail( pString );
				}
			}

			if( m_nSubID_2 == CLOSE_SLOT )
			{
				_AddLine( true );

				ti.s_eFontSize = CFont::FS_10;
				ti.s_Color = NiColor( 0, 1, 0 );
				g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, UISTRING_TEXT( "TOOLTIP_ITEM_SLOT_HAS_EXPIRED_INVENTORY" ).c_str(), &ti );
			}
		}
		break;	
	case cBaseWindow::WT_WAREHOUSE:
		{
			if( m_nSubID_2 == CLOSE_SLOT )
			{
				_AddLine( true );

				ti.s_eFontSize = CFont::FS_10;
				ti.s_Color = NiColor( 0, 1, 0 );
				g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, UISTRING_TEXT( "TOOLTIP_ITEM_SLOT_HAS_EXPIRED_WAREHOUSE" ).c_str(), &ti );
			}
		}
		break;
	//case cBaseWindow::WT_CASH:
	case cBaseWindow::WT_NEW_CASH_WAREHOUSE:
		{
			_AddLine( true );
			ti.s_eFontSize = CFont::FS_10;
			ti.s_Color = NiColor( 0, 1, 0 );
			g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, UISTRING_TEXT( "TOOLTIP_ITEM_CASH_WAREHOUSE_EXPLAIN" ).c_str(), &ti );
		}
		break;
	} 

	//=================================================================================================================
	//
	//		용병알
	//
	//=================================================================================================================
	// 칩셋일 경우 - 적용 가능한지
	switch( pFTInfo->s_nType_L )
	{
	case nItem::Digitama3:
		{
			bool nIsEnable = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_ISENABLE, &nIsEnable);
			
			if( nIsEnable == true )
			{
				int nCount = 0;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_COUNT, &nCount);

				int nType = 0;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_MAKETACTICS_DIGITAMA_TYPE, &nType);

				_AddLine( true );

				pString = NiNew cString;
				ti.s_Color = FONT_GOLD;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_ITEM_WAITING_TO_BE_HATCHED" ).c_str() );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );

				std::wstring wsItemName;
				if( nsCsFileTable::g_pItemMng )
				{
					CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nType );
					if( pFTItem )
						wsItemName = pFTItem->GetName();
				}

				pString = NiNew cString;
				std::wstring wsHatchLevel;
				DmCS::StringFn::Format( wsHatchLevel, L"%s - %s:%d", wsItemName.c_str(), UISTRING_TEXT( "TOOLTIP_ITEM_HATCH_LEVEL" ).c_str(), nCount );
				//_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _LAN( "    %s - 단계:%d" ), nsCsFileTable::g_pItemMng->GetItem( nType )->GetInfo()->s_szName, nCount );
				ti.s_Color = FONT_WHITE;
				ti.SetText( wsHatchLevel.c_str() );
				pString->AddText( &ti );

				m_StringList.AddTail( pString );
			}
		}
		break;
	}


#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
	if( m_pData != NULL )
	{
		cItemInfo* pInfo = (cItemInfo*)m_pData;
		if( cData_Inven::IsTradeLimitedItem(pInfo) )
		{
			_AddLine( true );

			{
				cText::sTEXTINFO ti;
				ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
				cString * pString = NiNew cString;
				ti.s_Color = FONT_RED;
				ti.SetText( UISTRING_TEXT( "TOOLTIP_TRADE_LIMITED_MESSAGE" ).c_str() );
				pString->AddText( &ti );
				m_StringList.AddTail( pString );
			}

// 			{
// 				cText::sTEXTINFO ti;
// 				ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
// 				cString* pString = NiNew cString;
// 				ti.s_Color = FONT_NEWGOLD;	
// 				ti.SetText( UISTRING_TEXT( "TOOLTIP_TRADE_LIMITED_HEAD" ).c_str() );
// 				pString->AddText( &ti );//->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;
// 				m_StringList.AddTail( pString );
// 			}
// 			{
// 				cText::sTEXTINFO ti;
// 				ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
// 				cString* pString = NiNew cString;
// 				ti.s_Color = FONT_WHITE;
// 				ti.SetText( _T( " " ) );
// 				sUPDATE_TIME ut;
// 				ut.s_pText = pString->AddText( &ti );
// 				ut.s_nTimeTS = pInfo->m_nRemainTradeLimitTime + TRADE_LIMITE_TIME_DALAY;
// 				m_vpEditElement.PushBack( ut );
// 
// 				m_StringList.AddTail( pString );
// 			}
		}
	}	
#endif
}
