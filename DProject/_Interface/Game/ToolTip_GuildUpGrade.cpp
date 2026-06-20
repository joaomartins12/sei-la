
#include "stdafx.h"
#include "ToolTip.h"


void cTooltip::_MakeTooltip_GuildUpGrade()
{	
	CsBaseMng::sGuild* pGuildInfo = nsCsFileTable::g_pBaseMng->GetGuildInfo( static_cast<int>(m_nMainID) + 1 );
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13 );

	cString* pString;

	//=================================================================================================================
	//
	//		타이틀
	//
	//=================================================================================================================	
	ti.s_Color = POPUP_MENU_COLOR;
	std::wstring wsUpgrade = UISTRING_TEXT( "TOOLTIP_GUILD_UPGRADE_UPGRADE_CONDITION_LEVEL" );
	DmCS::StringFn::Replace( wsUpgrade, L"#Num#", m_nMainID + 1 );
	ti.SetText( wsUpgrade.c_str() );	

	pString = NiNew cString;
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine( false );		

	// 길마 조건
	pString = NiNew cString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color =  NiColor( 0, 1, 0 );
	ti.SetText( UISTRING_TEXT( "TOOLTIP_GUILD_UPGRADE_GUILD_MASTER_LV" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = (int)( TOOLTIP_TAB_SIZE * 3.5 );

	cCommunityContent::sGUILD_PERSON_INFO myInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILD_MYINFO, &myInfo );
	if( myInfo.s_nPersonLevel >= pGuildInfo->s_nMasterLevel )
		ti.s_Color = FONT_WHITE;
	else
		ti.s_Color = FONT_RED;

	std::wstring wsNeed;
	DmCS::StringFn::Format( wsNeed, L"%d %s", pGuildInfo->s_nMasterLevel, UISTRING_TEXT( "TOOLTIP_MORE_THAN" ).c_str() );
	ti.SetText( wsNeed.c_str() );	
	pString->AddText( &ti );

	m_StringList.AddTail( pString );

	// 필요 길드원수
	pString = NiNew cString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color =  NiColor( 0, 1, 0 );
	ti.SetText( UISTRING_TEXT( "TOOLTIP_GUILD_UPGRADE_MEMBER_COUNT" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = (int)( TOOLTIP_TAB_SIZE * 3.5 );

	cCommunityContent::sGUILD_INFO guildInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
	if( guildInfo.s_nCurPersonCount >= pGuildInfo->s_nNeedPerson )
		ti.s_Color = FONT_WHITE;
	else
		ti.s_Color = FONT_RED;

	std::wstring wsNeedPerson;
	DmCS::StringFn::Format( wsNeedPerson, L"%d %s", pGuildInfo->s_nNeedPerson, UISTRING_TEXT( "TOOLTIP_MORE_THAN" ).c_str() );
	ti.SetText( wsNeedPerson.c_str() );
	pString->AddText( &ti );

	m_StringList.AddTail( pString );

	// 필요 명성
	pString = NiNew cString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color =  NiColor( 0, 1, 0 );
	ti.SetText( UISTRING_TEXT( "TOOLTIP_GUILD_UPGRADE_GUILD_REPUTATION" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = (int)( TOOLTIP_TAB_SIZE * 3.5 );

	if( guildInfo.s_nExp >= (int)pGuildInfo->s_nFame )
		ti.s_Color = FONT_WHITE;
	else
		ti.s_Color = FONT_RED;

	std::wstring wsNeedFame;
	DmCS::StringFn::Format( wsNeedFame, L"%d %s", pGuildInfo->s_nFame, UISTRING_TEXT( "TOOLTIP_MORE_THAN" ).c_str() );
	ti.SetText( wsNeedFame.c_str() );	
	pString->AddText( &ti );

	m_StringList.AddTail( pString );

	// 승급 필요 아이템
	if( pGuildInfo->s_nItemCount1 > 0 )
	{
		pString = NiNew cString;
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color =  NiColor( 0, 1, 0 );
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_REQUIRE_ITEM" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = (int)( TOOLTIP_TAB_SIZE * 3.5 );

		if( g_pDataMng->GetInven()->GetCount_Item_ID( pGuildInfo->s_nItemNo1 ) >= pGuildInfo->s_nItemCount1 )			
			ti.s_Color = FONT_WHITE;
		else
			ti.s_Color = FONT_RED;

		std::wstring wsItemName;
		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pGuildInfo->s_nItemNo1 );
		if( pFTItem )
			wsItemName = pFTItem->GetName();
		if( wsItemName.empty() )
			wsItemName = L"???";

		std::wstring wsMsg = UISTRING_TEXT( "TOOLTIP_ITEM_NAME_COUNT" );
		DmCS::StringFn::Replace( wsMsg, L"#Name#", wsItemName.c_str() );
		DmCS::StringFn::Replace( wsMsg, L"#Count#", pGuildInfo->s_nItemCount1 );

		ti.SetText( wsMsg.c_str() );	
		pString->AddText( &ti );		

		m_StringList.AddTail( pString );
	}

	if( pGuildInfo->s_nItemCount2 > 0 )
	{
		pString = NiNew cString;
		ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
		ti.s_Color =  NiColor( 0, 1, 0 );
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_REQUIRE_ITEM" ).c_str() );
		pString->AddText( &ti )->s_ptSize.x = (int)( TOOLTIP_TAB_SIZE * 3.5 );

		if( g_pDataMng->GetInven()->GetCount_Item_ID( pGuildInfo->s_nItemNo2 ) >= pGuildInfo->s_nItemCount2 )			
			ti.s_Color = FONT_WHITE;
		else
			ti.s_Color = FONT_RED;

		std::wstring wsItemName;
		CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pGuildInfo->s_nItemNo2 );
		if( pFTItem )
			wsItemName = pFTItem->GetName();
		if( wsItemName.empty() )
			wsItemName = L"???";

		std::wstring wsMsg = UISTRING_TEXT( "TOOLTIP_ITEM_NAME_COUNT" );
		DmCS::StringFn::Replace( wsMsg, L"#Name#", wsItemName.c_str() );
		DmCS::StringFn::Replace( wsMsg, L"#Count#", pGuildInfo->s_nItemCount2 );

		ti.SetText( wsMsg.c_str() );
		pString->AddText( &ti );		

		m_StringList.AddTail( pString );
	}	
}	