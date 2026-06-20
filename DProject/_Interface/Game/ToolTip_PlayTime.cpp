
#include "stdafx.h"
#include "ToolTip.h"

void cTooltip::_MakeTooltip_PlayTime()
{
#ifdef PLAY_PANELTY
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

	cString* pString;
	TCHAR sz[ TOOLTIP_MAX_TEXT_LEN ];

	ti.s_Color = FONT_GOLD;	
	
	pString = NiNew cString;	

	ti.SetText( UISTRING_TEXT( "TOOLTIP_PLAYTIME_REMAINING_PENALTY" ).c_str() );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );


	int nCurT = g_pDataMng->PlayTime_Get_NormalTime();
	int nFullT = g_pDataMng->PlayTime_Get_BaseFullTime();
	int nPenelT = g_pDataMng->PlayTime_Get_BasePaneltyTime();

	int nLevel = nPlayTime::NO_PANELTY;
	if( nCurT == 0 )
	{
		nLevel = nPlayTime::LEVEL2_PANELTY;
	}
	else if( nCurT <= nPenelT )
	{
		nLevel = nPlayTime::LEVEL1_PANELTY;
	}

	bool bCenter = false;
	switch( nLevel )
	{
	case nPlayTime::NO_PANELTY:
		{
			bCenter = true;
			ti.s_Color = FONT_WHITE;

			u4 nRT = g_pDataMng->PlayTime_Get_NormalTime() - g_pDataMng->PlayTime_Get_BasePaneltyTime();
			int nHour = nRT/3600;
			int nTemp = nRT%3600;
			int nMin = nTemp/60;
			if( ( nHour == 0 )&&( nMin == 0 ) )
				nMin = 1;
			_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%.2d:%.2d" ), nHour, nMin );
		}
		break;
	case nPlayTime::LEVEL1_PANELTY:
		{
			bCenter = true;
			ti.s_Color = TOOLTIP_CANNOT_COLOR;

			u4 nRT = g_pDataMng->PlayTime_Get_NormalTime();
			int nHour = nRT/3600;
			int nTemp = nRT%3600;
			int nMin = nTemp/60;
			if( ( nHour == 0 )&&( nMin == 0 ) )
				nMin = 1;
			_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%.2d:%.2d" ), nHour, nMin );			
		}
		break;
	case nPlayTime::LEVEL2_PANELTY:
		{
			ti.s_Color = TOOLTIP_CANNOT_COLOR;

			_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, UISTRING_TEXT( "TOOLTIP_PLAYTIME_CANNOT_ACQUIRE_EXP_ITEMS" ).c_str() );			
		}
		break;	
	}
	
	pString = NiNew cString;

	if( bCenter == true )
	{
		ti.SetText( L"" );
		if( g_pDataMng->PlayTime_Get_PaneltyType() == nPlayTime::USE_CASH )
			pString->AddText( &ti )->s_ptSize.x = 27;
		else
			pString->AddText( &ti )->s_ptSize.x = 55;
	}	

	ti.SetText( sz );
	pString->AddText( &ti );

	if( g_pDataMng->PlayTime_Get_PaneltyType() == nPlayTime::USE_CASH )
	{
		ti.s_Color = FONT_GREEN;

		u4 nRT = g_pDataMng->PlayTime_Get_CashTime();
		int nHour = nRT/3600;
		int nTemp = nRT%3600;
		int nMin = nTemp/60;
		if( ( nHour == 0 )&&( nMin == 0 ) )
			nMin = 1;
		_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( " (+%.2d:%.2d)" ), nHour, nMin );
		ti.SetText( sz );
		pString->AddText( &ti );		
	}

	if( bCenter == true )
	{
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_MIDDLE_MIN" ).c_str() );
		pString->AddText( &ti );
	}	

	m_StringList.AddTail( pString );

	_AddLine( true );

	

	ti.s_Color = FONT_GOLD;

	std::wstring wsReset = UISTRING_TEXT( "TOOLTIP_PLAYTIME_REMAINING_RESET_TIME" );
	ti.SetText( wsReset.c_str() );
	pString = NiNew cString;
	pString->AddText( &ti );
	m_StringList.AddTail( pString );


	ti.s_Color = FONT_WHITE;
	pString = NiNew cString;
	ti.SetText( L"" );
	pString->AddText( &ti )->s_ptSize.x = 55;

	u4 nRT = g_pDataMng->PlayTime_Get_GameResetTime();
	int nHour = nRT/3600;
	int nTemp = nRT%3600;
	int nMin = nTemp/60;
	if( ( nHour == 0 )&&( nMin == 0 ) )
		nMin = 1;
	_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, L"%.2d:%.2d %s", nHour, nMin, UISTRING_TEXT( "COMMON_TXT_MIDDLE_MIN" ).c_str() );
	ti.SetText( sz );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );


	_AddLine( true );

	int nExp = 0;
	int nItem = 0;

	if( g_pDataMng->PlayTime_Get_PaneltyType() != nPlayTime::USE_CASH )
	{
		CsBaseMng::sPLAY_PANELTY* pInfo = nsCsFileTable::g_pBaseMng->GetPlayPanelty( nLevel );
		nExp = 100 - pInfo->s_nExp;
		nItem = 100 - pInfo->s_nDrop;
	}


	pString = NiNew cString;

	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_PLAYTIME_EXP_PENELTY" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

	if( nExp == 0 )
	{
		_tcscpy_s( sz, TOOLTIP_MAX_TEXT_LEN, UISTRING_TEXT( "TOOLTIP_PLAYTIME_EXP_NONE" ).c_str() );
	}
	else
	{
		ti.s_Color = TOOLTIP_CANNOT_COLOR;
		_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d%%" ), nExp );
	}

	ti.SetText( sz );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );


	pString = NiNew cString;

	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "TOOLTIP_PLAYTIME_ITEM_PENELTY" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 3;

	if( nExp == 0 )
	{
		_tcscpy_s( sz, TOOLTIP_MAX_TEXT_LEN, UISTRING_TEXT( "TOOLTIP_PLAYTIME_EXP_NONE" ).c_str() );
	}
	else
	{
		ti.s_Color = TOOLTIP_CANNOT_COLOR;
		_stprintf_s( sz, TOOLTIP_MAX_TEXT_LEN, _T( "%d%%" ), nItem );
	}

	ti.SetText( sz );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );
	
#endif
}