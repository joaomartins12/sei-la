#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "InfiniteWarNoticeBoard.h"
#include "InfiniteWarNoticeBoard_Daily.h"
#include "InfiniteWarNoticeBoard_Week.h"
#include "InfiniteWarNoticeBoard_Monthly.h"
#include "InfiniteWarNoticeBoard_Season.h"
#include "InfiniteWarNoticeBoard_HallOfFame.h"


CInfiniteWarNoticeBoardWindow::CInfiniteWarNoticeBoardWindow():
m_pCloseBtn(NULL),m_DailyUI(NULL),m_WeekUI(NULL),
m_MonthUI(NULL),m_SeasonUI(NULL),m_pTabMenu(NULL),
m_HallOfFameUI(NULL)
{

}

CInfiniteWarNoticeBoardWindow::~CInfiniteWarNoticeBoardWindow()
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE CInfiniteWarNoticeBoardWindow::GetWindowType()
{ 
	return WT_INFINITEWAR_NOTICEBOARD_WINDOW; 
}

void CInfiniteWarNoticeBoardWindow::Destroy()
{
	cBaseWindow::Delete();	
}

void CInfiniteWarNoticeBoardWindow::ResetMap()
{
	_CloseWindow( NULL, NULL );	
}

void CInfiniteWarNoticeBoardWindow::DeleteResource()
{
	DeleteScript();	
}

void CInfiniteWarNoticeBoardWindow::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint winPos( (g_nScreenWidth >> 1) - (1024 >> 1), (g_nScreenHeight>>1) - (614>>1) );
	InitScript( "InfiniteNoticeBoard\\Infinite_NoticeBoard_window.tga", winPos, CsPoint( 1024, 614 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	AddTitle( UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_TITLE").c_str(), CsPoint(0,12), CFont::FS_16 );

	m_pCloseBtn = AddButton( CsPoint( 980, 16 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );	
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarNoticeBoardWindow::_CloseWindow );

	m_pTabMenu = AddRadioButton( CsPoint(22,60), CsPoint( 168, 38 ), CsPoint(0, 38), "CommonUI\\Menu_tap_btn.tga" );
	if( m_pTabMenu )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_TAB_POINT_TEXT").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pTabMenu->SetCheckBoxText( &ti );
		m_pTabMenu->AddEvent( cRadioButton::RadioButton_LBUP_EVENT, this, &CInfiniteWarNoticeBoardWindow::_ChangeTab );
		m_pTabMenu->SetUserData( new sTabType( eReward_Daily ) );

		cRadioButton* pAddBtn = m_pTabMenu->AddNode( CsPoint( 22 + 168, 60 ), CsPoint( 168, 38 ), CsPoint(0, 38), "CommonUI\\Menu_tap_btn.tga" );
		if( pAddBtn )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.SetText(UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_TAB_WEEKRANK_TEXT").c_str());
			ti.s_eTextAlign = DT_CENTER;
			pAddBtn->SetCheckBoxText( &ti );
			pAddBtn->SetEnable(false);
			pAddBtn->SetUserData( new sTabType( eReward_Week ) );
		}
		pAddBtn = m_pTabMenu->AddNode( CsPoint( 22 + 336, 60 ), CsPoint( 168, 38 ), CsPoint(0, 38), "CommonUI\\Menu_tap_btn.tga" );
		if( pAddBtn )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.SetText(UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_TAB_MONTHLYRANK_TEXT").c_str());
			ti.s_eTextAlign = DT_CENTER;
			pAddBtn->SetCheckBoxText( &ti );
			pAddBtn->SetEnable(false);
			pAddBtn->SetUserData( new sTabType( eReward_Month ) );
		}
		pAddBtn = m_pTabMenu->AddNode( CsPoint( 22 + 504, 60 ), CsPoint( 168, 38 ), CsPoint(0, 38), "CommonUI\\Menu_tap_btn.tga" );
		if( pAddBtn )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.SetText(UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_TAB_SEASONRANK_TEXT").c_str());
			ti.s_eTextAlign = DT_CENTER;
			pAddBtn->SetCheckBoxText( &ti );
			pAddBtn->SetEnable(false);
			pAddBtn->SetUserData( new sTabType( eReward_Season ) );
		}
		pAddBtn = m_pTabMenu->AddNode( CsPoint( 22 + 672, 60 ), CsPoint( 168, 38 ), CsPoint(0, 38), "CommonUI\\Menu_tap_btn.tga" );
		if( pAddBtn )
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.SetText(UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_TAB_HALLOFFAME_TEXT").c_str());
			ti.s_eTextAlign = DT_CENTER;
			pAddBtn->SetCheckBoxText( &ti );
			pAddBtn->SetEnable(false);
			pAddBtn->SetUserData( new sTabType( eHallOfFame ) );
		}
		m_pTabMenu->SetCheckIndex(0);
	}


#ifdef SDM_INFINITEWAR_RANKING_SYSTEM_20180312
	cRadioButton* pBtn = m_pTabMenu->GetNode( 1 );
	if( pBtn )
		pBtn->SetEnable(true);
	pBtn = m_pTabMenu->GetNode( 2 );
	if( pBtn )
		pBtn->SetEnable(true);
	pBtn = m_pTabMenu->GetNode( 3 );
	if( pBtn )
		pBtn->SetEnable(true);
	pBtn = m_pTabMenu->GetNode( 4 );
	if( pBtn )
		pBtn->SetEnable(true);
#endif

	_CreateChildWindow();
	_ShowTabWindow( eReward_Daily );
}

void CInfiniteWarNoticeBoardWindow::_CreateChildWindow()
{
	m_DailyUI = NiNew CInfinitewarNoticeBoardDailyUI;
	if(m_DailyUI && m_DailyUI->Construct() )
	{
		m_DailyUI->Create(GetRoot());
		m_DailyUI->SetVisible(false);

		AddChildScriptUI( m_DailyUI );
	}
	m_WeekUI = NiNew CInfinitewarNoticeBoardWeekUI(TB::eInfiniteWarRewardType::eWeek);
	if(m_WeekUI && m_WeekUI->Construct() )
	{
		m_WeekUI->Create(GetRoot());
		m_WeekUI->SetVisible(false);

		AddChildScriptUI( m_WeekUI );
	}
	m_MonthUI = NiNew CInfinitewarNoticeBoardMonthlyUI(TB::eInfiniteWarRewardType::eMonth);
	if(m_MonthUI && m_MonthUI->Construct() )
	{
		m_MonthUI->Create(GetRoot());
		m_MonthUI->SetVisible(false);

		AddChildScriptUI( m_MonthUI );
	}
	m_SeasonUI = NiNew CInfinitewarNoticeBoardSeasonUI(TB::eInfiniteWarRewardType::eSeason);
	if(m_SeasonUI && m_SeasonUI->Construct() )
	{
		m_SeasonUI->Create(GetRoot());
		m_SeasonUI->SetVisible(false);

		AddChildScriptUI( m_SeasonUI );
	}

	m_HallOfFameUI = NiNew CInfinitewarNoticeBoardHallOfFameUI; 
	if(m_HallOfFameUI && m_HallOfFameUI->Construct() )
	{
		m_HallOfFameUI->Create(GetRoot());
		m_HallOfFameUI->SetVisible(false);

		AddChildScriptUI( m_HallOfFameUI );
	}
}

void CInfiniteWarNoticeBoardWindow::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
	if( GetSystem() )
	{
		CsC_AvObject* pTarget = GetSystem()->GetTargetNpc();	
		if( pTarget )
		{
			assert_cs( pTarget->GetLeafRTTI() == RTTI_NPC );
			if( ( (CNpc*)pTarget )->CheckTamerDist() == false )
				_CloseWindow(NULL, NULL);
		}
	}

	_UpdateMoveWindow();
}

bool CInfiniteWarNoticeBoardWindow::OnEscapeKey()
{
	_CloseWindow(NULL, NULL);
	return true;
}

cBaseWindow::eMU_TYPE CInfiniteWarNoticeBoardWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pCloseBtn && m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pTabMenu && m_pTabMenu->Update_ForMouse() != cRadioButton::INVALIDE_RADIO_CLICK )
		return muReturn;

	if( m_DailyUI && m_DailyUI->UpdateMouse() )
		return muReturn;

	if( m_WeekUI && m_WeekUI->UpdateMouse() )
		return muReturn;

	if( m_MonthUI && m_MonthUI->UpdateMouse() )
		return muReturn;

	if( m_SeasonUI && m_SeasonUI->UpdateMouse() )
		return muReturn;

	if( m_HallOfFameUI && m_HallOfFameUI->UpdateMouse() )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 1024, 48 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();			

	return muReturn;
}

void CInfiniteWarNoticeBoardWindow::_CloseWindow( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->OpenCloseNoticeWindow(false);
}

void CInfiniteWarNoticeBoardWindow::_ChangeTab(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pData);
	cRadioButton* pRadioButton = static_cast<cRadioButton*>(pData);
	SAFE_POINTER_RET(pRadioButton);
	sTabType* userData = dynamic_cast<sTabType*>(pRadioButton->GetUserData());
	SAFE_POINTER_RET(userData);
	_ShowTabWindow( userData->eRewardType );	
}

void CInfiniteWarNoticeBoardWindow::_ShowTabWindow( eTabType const& changeTabType )
{
	if( m_DailyUI )
		m_DailyUI->SetVisible(false);
	if( m_WeekUI )
		m_WeekUI->SetVisible(false);
	if( m_MonthUI )
		m_MonthUI->SetVisible(false);
	if( m_SeasonUI )
		m_SeasonUI->SetVisible(false);
	if( m_HallOfFameUI )
		m_HallOfFameUI->SetVisible(false);

	switch( changeTabType )
	{
	case eReward_Daily:
		if( m_DailyUI )
			m_DailyUI->SetVisible(true);
		break;
	case eReward_Week:
		if( m_WeekUI )
		{
			m_WeekUI->SetVisible(true);
		}
		break;
	case eReward_Month:
		if( m_MonthUI )
		{
			m_MonthUI->SetVisible(true);
		}
		break;
	case eReward_Season:
		if( m_SeasonUI )
		{
			m_SeasonUI->SetVisible(true);
		}
		break;
	case eHallOfFame:
		if( m_HallOfFameUI )
		{
			m_HallOfFameUI->SetVisible(true);
		}
		break;
	}
}

void CInfiniteWarNoticeBoardWindow::Render()
{
	RenderScript();

	EndRenderScript();
}

void CInfiniteWarNoticeBoardWindow::ResetDevice()
{	
	ResetDeviceScript();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CInfiniteWarNoticeBoardWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CInfiniteWarNoticeBoardWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
// 	switch(iNotifiedEvt)
// 	{
// 	}
}