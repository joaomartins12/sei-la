
#include "stdafx.h"
#include "PlayTime.h"

#define IF_PLAYTIME_HEIGHT		13


cPlayTime::cPlayTime() : m_pText(NULL), m_nRenderBarIndex(0), m_nServerOldDay(0)
{
	for( int i=0; i<3; ++i )
		m_pBG[ i ] = NULL;

	for( int i=0; i<IF_PLAYTIME_BAR_CNT; ++i )
		m_pBar[ i ] = NULL;
}

void cPlayTime::Destroy()
{
	cBaseWindow::Delete();	
}

void cPlayTime::DeleteResource()
{
	DeleteScript();

	for( int i=0; i<3; ++i )
		SAFE_NIDELETE( m_pBG[ i ] );

	for( int i=0; i<IF_PLAYTIME_BAR_CNT; ++i )
		SAFE_NIDELETE( m_pBar[ i ] );

	SAFE_NIDELETE( m_pText );
}

void cPlayTime::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;
	
	int nCX = DEFAULT_SCREEN_HEIGHT - 155;
	InitScript( NULL, CsPoint( 0, DEFAULT_SCREEN_HEIGHT - 10 - IF_PLAYTIME_HEIGHT ), CsPoint( nCX, IF_PLAYTIME_HEIGHT ), false, IFREGION_X::LEFT, IFREGION_Y::BOTTOM, true );

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_9, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	m_pText = NiNew cText;
	m_pText->Init( NULL, CsPoint::ZERO, &ti, false );

	RecreateBG();

	m_DayTimeSeq.SetDeltaTime( 20000 );
	m_DayTimeSeq.SetEnableTime( false );
}

void cPlayTime::RecreateBG()
{
#ifdef PLAY_PANELTY

	int nCX = g_nScreenWidth - 155;
	m_pRoot->SetSize( CsPoint( nCX, IF_PLAYTIME_HEIGHT ) );

	for( int i=0; i<3; ++i )
		SAFE_NIDELETE( m_pBG[ i ] );

	m_pBG[ 0 ] = NiNew cSprite;
	m_pBG[ 0 ]->Init( m_pRoot, CsPoint::ZERO, CsPoint( 3, IF_PLAYTIME_HEIGHT ), "Bar_G\\PlayTime\\BGWin_1.tga", false );

	m_pBG[ 1 ] = NiNew cSprite;
	m_pBG[ 1 ]->Init( m_pRoot, CsPoint( 3, 0 ), CsPoint( nCX - 6, IF_PLAYTIME_HEIGHT ), "Bar_G\\PlayTime\\BGWin_2.tga", false );

	m_pBG[ 2 ] = NiNew cSprite;
	m_pBG[ 2 ]->Init( m_pRoot, CsPoint( nCX - 3, 0 ), CsPoint( 3, IF_PLAYTIME_HEIGHT ), "Bar_G\\PlayTime\\BGWin_3.tga", false );


	for( int i=0; i<IF_PLAYTIME_BAR_CNT; ++i )
		SAFE_NIDELETE( m_pBar[ i ] );

	m_pBar[ 0 ] = NiNew cProgressBar;
	m_pBar[ 0 ]->Init( m_pRoot, cProgressBar::BAR_1IMAGE, CsPoint( 2, 2 ), CsPoint( nCX - 4, IF_PLAYTIME_HEIGHT - 4 ), CsPoint( 2, IF_PLAYTIME_HEIGHT - 4 ), "Bar_G\\PlayTime\\Bar_Cash.tga", false );	

	m_pBar[ 1 ] = NiNew cProgressBar;
	m_pBar[ 1 ]->Init( m_pRoot, cProgressBar::BAR_1IMAGE, CsPoint( 2, 2 ), CsPoint( nCX - 4, IF_PLAYTIME_HEIGHT - 4 ), CsPoint( 2, IF_PLAYTIME_HEIGHT - 4 ), "Bar_G\\PlayTime\\Bar_Y.tga", false );

	m_pBar[ 2 ] = NiNew cProgressBar;
	m_pBar[ 2 ]->Init( m_pRoot, cProgressBar::BAR_1IMAGE, CsPoint( 2, 2 ), CsPoint( nCX - 4, IF_PLAYTIME_HEIGHT - 4 ), CsPoint( 2, IF_PLAYTIME_HEIGHT - 4 ), "Bar_G\\PlayTime\\Bar_R.tga", false );

	m_pBar[ 3 ] = NiNew cProgressBar;
	m_pBar[ 3 ]->Init( m_pRoot, cProgressBar::BAR_1IMAGE, CsPoint( 2, 2 ), CsPoint( nCX - 4, IF_PLAYTIME_HEIGHT - 4 ), CsPoint( 2, IF_PLAYTIME_HEIGHT - 4 ), "Bar_G\\PlayTime\\Bar_Disable.tga", false );

	Update();	
#endif
}

void cPlayTime::Update(float const& fDeltaTime)
{
#ifdef PLAY_PANELTY
	_CheckServerDay();
	

	int nFullTime = g_pDataMng->PlayTime_Get_BaseFullTime();
	int nPTime = g_pDataMng->PlayTime_Get_NormalTime();	

	switch( g_pDataMng->PlayTime_Get_PaneltyType() )
	{
	case nPlayTime::NO_PANELTY:
		m_nRenderBarIndex = 1;
		m_pBar[ m_nRenderBarIndex ]->SetRange( CsPoint( 0, nFullTime ) );
		m_pBar[ m_nRenderBarIndex ]->SetPos( nPTime );
		break;
	case nPlayTime::LEVEL1_PANELTY:
		m_nRenderBarIndex = 2;
		m_pBar[ m_nRenderBarIndex ]->SetRange( CsPoint( 0, nFullTime ) );
		m_pBar[ m_nRenderBarIndex ]->SetPos( nPTime );
		break;
	case nPlayTime::LEVEL2_PANELTY:
		m_nRenderBarIndex = 3;
		m_pBar[ m_nRenderBarIndex ]->SetRange( CsPoint( 0, nFullTime ) );
		m_pBar[ m_nRenderBarIndex ]->SetPos( g_pDataMng->PlayTime_Get_BaseFullTime() );
		break;
	case nPlayTime::USE_CASH:
		m_nRenderBarIndex = 0;
		m_pBar[ m_nRenderBarIndex ]->SetRange( CsPoint( 0, nFullTime ) );
		m_pBar[ m_nRenderBarIndex ]->SetPos( nPTime );
		break;
	default:
		assert_csm1( false, L"Panelty Type = %d", g_pDataMng->PlayTime_Get_PaneltyType() );
		m_nRenderBarIndex = 0;
		m_pBar[ m_nRenderBarIndex ]->SetRange( CsPoint( 0, nFullTime ) );
	}	
#endif
}

cBaseWindow::eMU_TYPE
cPlayTime::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 15, 0 ), 200, cTooltip::PLAYTIME, 0 );

	return muReturn;
}

void cPlayTime::Render()
{
	RenderScript();

	for( int i=0; i<3; ++i )
	{
		if( m_pBG[ i ] )
			m_pBG[ i ]->Render();
	}

	if( m_pBar[ m_nRenderBarIndex ] )
		m_pBar[ m_nRenderBarIndex ]->Render();

	_RenderText();
}

void cPlayTime::_RenderText()
{
#ifdef PLAY_PANELTY
	u4 nRT = 0;
	// Ä³½¬
	if( g_pDataMng->PlayTime_Get_PaneltyType() == 100 )
	{
		nRT = g_pDataMng->PlayTime_Get_CashTime();
	}
	// ÀÏ¹Ý
	else
	{
		nRT = g_pDataMng->PlayTime_Get_NormalTime();
	}

	int nHour = nRT/3600;
	int nTemp = nRT%3600;
	int nMin = nTemp/60;

	if( g_pDataMng->PlayTime_Get_PaneltyType() != nPlayTime::MAX_PENALTY )
	{
		if( ( nHour == 0 )&&( nMin == 0 ) )
			nMin = 1;
	}

	std::wstring wsTime;
	DmCS::StringFn::Format( wsTime, L"%.2d  :  %.2d", nHour, nMin );
	m_pText->SetText( wsTime.c_str() );
	m_pText->Render( CsPoint( g_nScreenWidth/2, g_nScreenHeight - 9 - IF_PLAYTIME_HEIGHT ) );
#endif
}

void cPlayTime::ResetDevice()
{
	ResetDeviceScript();

	RecreateBG();
}

void cPlayTime::CalServerDay()
{
	assert_cs( net::game );

	tm time;
	nBase::GetTime2TM( net::game->GetTimeTS(), time );
	m_nServerOldDay = time.tm_yday;

	m_DayTimeSeq.OffReset();
}

void cPlayTime::_CheckServerDay()
{
#ifdef PLAY_PANELTY
	if( net::game == NULL )
		return;

	if( m_DayTimeSeq.IsEnableTime() == false )
		CalServerDay();

	if( m_DayTimeSeq.IsEnable() == false )
	{
		return;
	}

	tm time;
	nBase::GetTime2TM( net::game->GetTimeTS(), time );
	int nCurDay = time.tm_yday;

	if( m_nServerOldDay == nCurDay )
		return;

	CalServerDay();

	g_pDataMng->PlayTime_Set_NormalTime( g_pDataMng->PlayTime_Get_BaseFullTime() );
	if( g_pDataMng->PlayTime_Get_PaneltyType() != nPlayTime::USE_CASH )
		g_pDataMng->PlayTime_Set_PaneltyType( nPlayTime::NO_PANELTY );
#endif
}