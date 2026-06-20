

#include "stdafx.h"
#include "ToolTip.h"

#define TOOLTIP_TACTICS_NAME_COLOR				NiColor( 247/255.0f, 211/255.0f, 153/255.0f )

namespace
{
	inline TCHAR const* SafeTooltipText( TCHAR const* pText )
	{
		return ( pText != NULL ) ? pText : _T( "" );
	}
}

#ifdef SIMPLE_TOOLTIP
void cTooltip::_MakeTooltip_UIHelp()
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );

	cString* pString;

	//=================================================================================================================
	//
	//		이름
	//
	//=================================================================================================================
	SAFE_POINTER_RET( nsCsFileTable::g_pTalkMng );
	CsTalk_Tooltip* pTalkTooltip = nsCsFileTable::g_pTalkMng->GetTalk_Tooltip( static_cast<int>(m_nMainID) );
	SAFE_POINTER_RET( pTalkTooltip );
	CsTalk_Tooltip::sINFO* pInfo = pTalkTooltip->GetInfo();
	SAFE_POINTER_RET( pInfo );
	pString = NiNew cString;
	ti.SetText( SafeTooltipText( pInfo->s_Title ) );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine( true );

	//pString = NiNew cString;
	//ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	
	//ti.SetText( pInfo->s_Message );
	//pString->AddText( &ti );
	//m_StringList.AddTail( pString );
//////////////////////////////////////////////////////////


	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color = TOOLTIP_COLOR1;
	TCHAR szComment[ FT_TOOLTIP_MSG_BODY_LEN ];
	_tcscpy_s( szComment, FT_TOOLTIP_MSG_BODY_LEN, SafeTooltipText( pInfo->s_Message ) );
	g_pStringAnalysis->Cut_Parcing( &m_StringList, TOOLTIP_CUT_SIZE, szComment, &ti );	

}
#endif
void cTooltip::_MakeTooltip_Tactics()
{
	// MainID = 용병 슬롯 인덱스

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, TOOLTIP_TACTICS_NAME_COLOR );

	cString* pString;

	//=================================================================================================================
	//
	//		이름
	//
	//=================================================================================================================
	SAFE_POINTER_RET( g_pDataMng );
	cData_Tactics* pTactics = g_pDataMng->GetTactics();
	SAFE_POINTER_RET( pTactics );
	cData_PostLoad::sDATA* pData = pTactics->GetTactics( static_cast<int>(m_nMainID) );
	SAFE_POINTER_RET( pData );
	pString = NiNew cString;
	ti.SetText( SafeTooltipText( pData->s_szName ) );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine( true );

	//=================================================================================================================
	//
	//		레벨
	//
	//=================================================================================================================
	SAFE_POINTER_RET( nsCsFileTable::g_pDigimonMng );
	CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pData->s_dwBaseDigimonID );
	if( pDigimon )
	{
		CsDigimon::sINFO* pFTDigimonInfo = pDigimon->GetInfo();
		if( pFTDigimonInfo )
		{
			// 이름	
			pString = NiNew cString;
			ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
			ti.s_Color = FONT_GOLD;
			ti.SetText( SafeTooltipText( pFTDigimonInfo->s_szName ) );
			pString->AddText( &ti );
			pString->AddSizeY( 3 );
			m_StringList.AddTail( pString );
		}
	} 	

	pString = NiNew cString;
	ti.s_eFontSize = TOOLTIP_FONT_1_SIZE;
	ti.s_Color = FONT_GOLD;			
	ti.SetText( UISTRING_TEXT( "COMMON_TXT_LEVEL" ).c_str() );
	pString->AddText( &ti )->s_ptSize.x = TOOLTIP_TAB_SIZE * 2;

	ti.s_Color = FONT_WHITE;
	ti.SetText( pData->s_nLevel );
	pString->AddText( &ti );
	m_StringList.AddTail( pString );
}
