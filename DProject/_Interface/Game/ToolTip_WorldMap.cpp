

#include "stdafx.h"
#include "ToolTip.h"

#define TOOLTIP_WORLDMAP_NAME_COLOR				NiColor( 247/255.0f, 211/255.0f, 153/255.0f )

namespace
{
	inline TCHAR const* SafeTooltipText( TCHAR const* pText )
	{
		return ( pText != NULL ) ? pText : _T( "" );
	}
}

void cTooltip::_MakeTooltip_WorldMap()
{
	// MainID = ¸Ê Å¸ÀÔ eWORLDMAP
	// Sub1 = ID

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13, TOOLTIP_WORLDMAP_NAME_COLOR );

	cString* pString;

	//=================================================================================================================
	//
	//		¸ÊÀÌ¸§
	//
	//=================================================================================================================
	SAFE_POINTER_RET( nsCsFileTable::g_pWorldMapMng );
	SAFE_POINTER_RET( g_pStringAnalysis );

	switch( m_nMainID )
	{
	case WORLDMAP_AREA:
		{	
			CsAreaMap* pArea = nsCsFileTable::g_pWorldMapMng->GetArea( static_cast<int>(m_nSubID_1) );
			SAFE_POINTER_RET( pArea );
			CsAreaMap::sINFO* pFTArea = pArea->GetInfo();
			SAFE_POINTER_RET( pFTArea );
			pString = NiNew cString;
			ti.SetText( SafeTooltipText( pFTArea->s_szName ) );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			_AddLine( true );

			ti.s_eFontSize = CFont::FS_12;
			ti.s_Color = FONT_WHITE;
			g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, SafeTooltipText( pFTArea->s_szComment ), &ti );
		}
		break;
	case WORLDMAP_WORLD:
		{
			CsWorldMap* pWorld = nsCsFileTable::g_pWorldMapMng->GetWorld( static_cast<int>(m_nSubID_1)  );
			SAFE_POINTER_RET( pWorld );
			CsWorldMap::sINFO* pFTWorld = pWorld->GetInfo();
			SAFE_POINTER_RET( pFTWorld );
			pString = NiNew cString;
			ti.SetText( SafeTooltipText( pFTWorld->s_szName ) );
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			_AddLine( true );

			ti.s_eFontSize = CFont::FS_12;
			ti.s_Color = FONT_WHITE;
			g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, SafeTooltipText( pFTWorld->s_szComment ), &ti );
		}
		break;
	case WORLDMAP_MAIN:
		{			
			pString = NiNew cString;
			switch( m_nSubID_1 )
			{
			case 1:		ti.SetText( UISTRING_TEXT( "TOOLTIP_WORLD_MAP_REAL_WORLD" ).c_str() );				break;
			case 2:		ti.SetText( UISTRING_TEXT( "TOOLTIP_WORLD_MAP_DIGITAL_WORLD" ).c_str() );			break;
			default:	assert_cs( false );
			}
			
			pString->AddText( &ti );
			m_StringList.AddTail( pString );

			_AddLine( true );

			std::wstring wsMsg;
			switch( m_nSubID_1 )
			{
			case 1:
				wsMsg = UISTRING_TEXT( "TOOLTIP_WORLD_MAP_REAL_WORLD_EXPLAIN" );
				break;
			case 2:
				wsMsg = UISTRING_TEXT( "TOOLTIP_WORLD_MAP_DIGITAL_WORLD_EXPLAIN" );
				break;
			default:
				assert_cs( false );
			}

			ti.s_eFontSize = CFont::FS_12;
			ti.s_Color = FONT_WHITE;
			g_pStringAnalysis->Cut( &m_StringList, TOOLTIP_CUT_SIZE, SafeTooltipText( wsMsg.c_str() ), &ti );
		}
		break;
	default:
		assert_cs( false );
	}
}
