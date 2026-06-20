

#include "stdafx.h"

#define IF_RANK_RANK_POS			CsPoint( 24, 41 )
#define IF_RANK_TAMER_POS		CsPoint( 95, 41 )
#define IF_RANK_DIGIMON_POS	CsPoint( 204, 41 )
#define IF_RANK_DAMAGE_POS		CsPoint( 324, 41 )

void cRaidRank::Destroy()
{	
	cBaseWindow::Delete();		
}

void cRaidRank::DeleteResource()
{	
	DeleteScript();




}
void cRaidRank::ResetRankList()
{
	std::list<cText*>::iterator tIt = m_vpText.begin();
	for( ; tIt != m_vpText.end(); ++tIt )
		SAFE_NIDELETE( (*tIt) );
	m_vpText.clear();

	cText::sTEXTINFO tatle;
	tatle.Init( &g_pEngine->m_FontText, CFont::FS_12 );
	tatle.SetText( UISTRING_TEXT( "RAIDRANK_DAMAGE_RANKING" ).c_str() );
	tatle.s_eTextAlign = DT_CENTER;

	CsPoint pos;
	pos.x = m_ptRootSize.x / 2 + CsPoint::ZERO.x;
	pos.y = 7 + CsPoint::ZERO.y;

	m_pWindowTitle = AddText( &tatle, pos );

	cText::sTEXTINFO ti;
	ti.Init();
	ti.s_eFontSize = CFont::FS_10;
	ti.s_bOutLine = false;
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( UISTRING_TEXT( "RAIDRANK_RANK" ).c_str() );
	AddText( &ti, IF_RANK_RANK_POS );	

	ti.SetText( UISTRING_TEXT( "COMMON_TXT_TAMER" ).c_str() );
	AddText( &ti, IF_RANK_TAMER_POS );	


	ti.SetText( UISTRING_TEXT( "COMMON_TXT_DIGIMON" ).c_str() );
	AddText( &ti, IF_RANK_DIGIMON_POS );	


	ti.SetText( UISTRING_TEXT( "RAIDRANK_DAMAGE" ).c_str() );
	AddText( &ti, IF_RANK_DAMAGE_POS );	
}

void cRaidRank::Create(int nValue /* = 0  */)
{	
	cBaseWindow::Init();

	SetRootClient( CsPoint( (g_nScreenWidth/2)-(398/2), g_nScreenHeight/3 ) );
	InitScript( "Ranking\\Ranking_Raid.bmp", m_ptRootClient , CsPoint( 398, 258 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );

	m_pBtnClose = AddButton( CsPoint( 370, 6 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

	ResetRankList();
}


void cRaidRank::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cRaidRank::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	

	switch( m_pBtnClose->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		m_pRankingString.Delete();
		Close();
		return muReturn;
	default:
		return muReturn;
	}
	
	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 398, 28 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cRaidRank::OnMouseOutWindow()
{
	cBaseWindow::OnMouseOutWindow();	
}

void cRaidRank::Render()
{
	RenderScript();	

	cBaseWindow::EndRenderScript();
}

void cRaidRank::ResetDevice()
{
	ResetDeviceScript();
	m_pRankingString.ResetDevice();
}

bool cRaidRank::OnEscapeKey()
{
	m_pRankingString.Delete();
	return Close();
}

void cRaidRank::SetRanker(int nIndex, int nRank, TCHAR* szTamer, TCHAR* szDigimon, int nDamage, NiColor color)
{
/*
	TCHAR sz[ 128 ];
	int			oy;
	cString* pString = NULL;	

	cText::sTEXTINFO ti;
	ti.Init();
	ti.s_eFontSize = CFont::FS_10;
	ti.s_bOutLine = false;
	ti.s_eTextAlign = DT_CENTER;
	ti.s_Color = color;


	oy = 3 * nIndex;
	_stprintf_s( sz, 128, _T( "%d" ), nRank );
	ti.SetText( sz );	
	pString = NiNew cString;
	pString->AddText( &ti);
	pString->TailAddSizeX( 50 );
	//pString->AddText( &ti, CsPoint( 0, oy) );
	//pString->AddText( &ti )->s_ptSize.x = 30;
	
	
	
	ti.SetText( szTamer );
	pString = NiNew cString;
	pString->AddText( &ti);
	pString->TailAddSizeX( 50 );
	//pString->AddText( &ti, CsPoint( 30, oy) );
	//pString->AddText( &ti )->s_ptSize.x = 110;

	
	
	ti.SetText( szDigimon );
	pString = NiNew cString;
	pString->AddText( &ti);
	pString->TailAddSizeX( 50 );
	//pString->AddText( &ti, CsPoint( 110, oy) );
	//pString->AddText( &ti )->s_ptSize.x = 110;

	

	_stprintf_s( sz, 128, _T( "%d" ), nDamage );
	ti.SetText( sz );
	pString = NiNew cString;
	pString->AddText( &ti);
	pString->TailAddSizeX( 50 );
	//pString->AddText( &ti, CsPoint( 130, oy) );
	//pString->AddText( &ti )->s_ptSize.x = 130;

	m_pRankingString.AddTail(pString);
	*/

	TCHAR sz[ 128 ];
	int			oy;	

	cText::sTEXTINFO ti;
	ti.Init();
	ti.s_eFontSize = CFont::FS_10;
	ti.s_bOutLine = false;
	ti.s_eTextAlign = DT_CENTER;
	ti.s_Color = color;


	oy = 58+(17 * nIndex);

	_stprintf_s( sz, 128, _T( "%d" ), nRank );
	ti.SetText( sz );
	AddText( &ti, CsPoint( 24, oy ) );


	ti.SetText( szTamer );	
	AddText( &ti, CsPoint( 95, oy ) );



	ti.SetText( szDigimon );
	AddText( &ti, CsPoint( 204, oy ) );


	_stprintf_s( sz, 128, _T( "%d" ), nDamage );
	ti.SetText( sz );
	AddText( &ti, CsPoint( 324, oy ) );	
}
