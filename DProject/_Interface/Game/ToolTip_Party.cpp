

#include "stdafx.h"
#include "ToolTip.h"

void cTooltip::_MakeTooltip_Party()
{
	SAFE_POINTER_RET( m_pAdaptParty );
	SMemberInfo const* pInfo = m_pAdaptParty->GetMemberInfo( m_nMainID );
	SAFE_POINTER_RET( pInfo );

	//=================================================================================================================
	//
	//		타이틀
	//
	//=================================================================================================================
	// 이름
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
		if( m_pAdaptParty&& m_pAdaptParty->IsPartyMaster( m_nMainID ) )
		{
			ti.s_Color = NiColor( 1, 1, 0 );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_PARTY_TITLE_LEADER" ).c_str() );
		}
		else
		{
			ti.s_Color = NiColor( 0, 1, 0 );
			ti.SetText( UISTRING_TEXT( "TOOLTIP_PARTY_TITLE_MEMBER" ).c_str() );
		}

		cString* pString = NiNew cString;
		pString->AddText( &ti );
		m_StringList.AddTail( pString );
	}

 	_AddLine( false );
 
	//=================================================================================================================
	//
	//		테이머
	//
	//=================================================================================================================

	
	int nFirstWidth = TOOLTIP_TAB_SIZE * 2;
	int nSecondWidth = TOOLTIP_TAB_SIZE * 3.6f;
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

		FT_Face face = g_pEngine->m_FontSystem.GetFace( CFont::FS_10 );
		
		int TamerNameWidth	 = cText::GetStringWidth(face, pInfo->m_TamerInfo.m_Name);
		int DigimonNameWidth = cText::GetStringWidth(face, pInfo->m_DigimonInfo.m_Name);

		int ResultWidth = TamerNameWidth > DigimonNameWidth ? TamerNameWidth : DigimonNameWidth;
		if(ResultWidth > nSecondWidth)
		{
			int IncTabSize = 30;
			nSecondWidth = ResultWidth + IncTabSize;
		}

		cString* pString = NiNew cString;
		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_TAMER" ).c_str() );	
		pString->AddText( &ti )->s_ptSize.x = nFirstWidth;

		ti.s_Color = FONT_WHITE;
		ti.SetText( pInfo->m_TamerInfo.m_Name.c_str() );	
		pString->AddText( &ti )->s_ptSize.x = nSecondWidth;

		std::wstring lv;
		DmCS::StringFn::Format( lv, L"%s %d", UISTRING_TEXT( "COMMON_TXT_SHORT_LEVEL" ).c_str(), pInfo->m_TamerInfo.m_nLevel );
		ti.SetText( lv.c_str() );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );

		pString->CalMaxSize();
		if(pString->GetMaxSize().x > m_nMaxSizeX)
			m_nMaxSizeX = pString->GetMaxSize().x;
	}

	//=================================================================================================================
	//
	//		디지몬
	//
	//=================================================================================================================
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_DIGIMON" ).c_str() );

		cString* pString = NiNew cString;
		pString->AddText( &ti )->s_ptSize.x = nFirstWidth;

		ti.s_Color = FONT_WHITE;
		ti.SetText( pInfo->m_DigimonInfo.m_Name.c_str() );	
		pString->AddText( &ti )->s_ptSize.x = nSecondWidth;

		std::wstring lv;
		DmCS::StringFn::Format( lv, L"%s %d", UISTRING_TEXT( "COMMON_TXT_SHORT_LEVEL" ).c_str(), pInfo->m_DigimonInfo.m_nLevel );
		ti.SetText( lv.c_str() );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );

		pString->CalMaxSize();
		if(pString->GetMaxSize().x > m_nMaxSizeX)
			m_nMaxSizeX = pString->GetMaxSize().x;
	}

	_AddLine( false );

	//=================================================================================================================
	//
	//		위치
	//
	//=================================================================================================================
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.s_Color = FONT_GOLD;
		ti.SetText( UISTRING_TEXT( "COMMUNITY_BTN_LOCATION" ).c_str() );	

		cString* pString = NiNew cString;
		pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 2;

		CsMapList::sINFO* pFTMapInfo = nsCsMapTable::g_pMapListMng->GetList( pInfo->m_nMapNo )->GetInfo();
		ti.s_Color = FONT_WHITE;

		std::wstring chText;
		DmCS::StringFn::Format( chText, L"%s %d", UISTRING_TEXT( "COMMON_TXT_CHANNEL" ).c_str(), pInfo->m_nChannelNo );
		std::wstring text;
		DmCS::StringFn::Format( text, L"%s(%s)", pFTMapInfo->s_szMapDiscript.c_str(), chText.c_str() );

		ti.SetText( text.c_str() );
		pString->AddText( &ti );

		m_StringList.AddTail( pString );
	}
}