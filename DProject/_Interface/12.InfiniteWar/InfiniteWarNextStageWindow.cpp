#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "InfiniteWarNextStageWindow.h"


CInfiniteWarNextStagedWindow::CInfiniteWarNextStagedWindow():m_pWait(NULL),m_pOk(NULL),m_pCloseBtn(NULL)
{

}

CInfiniteWarNextStagedWindow::~CInfiniteWarNextStagedWindow()
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE CInfiniteWarNextStagedWindow::GetWindowType()
{ 
	return WT_INFINITEWAR_NEXTSTAGE_WINDOW; 
}

void CInfiniteWarNextStagedWindow::Destroy()
{
	cBaseWindow::Delete();	
}

void CInfiniteWarNextStagedWindow::DeleteResource()
{
	DeleteScript();	
}

void CInfiniteWarNextStagedWindow::Create(int nValue /* = 0  */)
{	
	if( cBaseWindow::Init() == false )
		return;
	CsPoint winPos( (g_nScreenWidth >> 1) - (482 >> 1), 100 );
	InitScript( "CommonUI\\CommonUI_popup.tga", winPos, CsPoint( 482, 288 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	AddTitle( UISTRING_TEXT("INFINITEWAR_NEXTSTAGE_WINDOW_TITLE").c_str(), CsPoint(0,20), CFont::FS_16 );

	bool bMaster = GetSystem()->IsPartyMaster();

	m_pCloseBtn = AddButton( CsPoint( 430, 21 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );	
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarNextStagedWindow::_WaitBtnEvent );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( L"" );
	ti.SetLineGabHeight(5);
	cText* pMsg = AddText( &ti, CsPoint( 241, 75 ));
	if( pMsg )
	{
		std::wstring msg;
		if( bMaster )	msg = UISTRING_TEXT("INFINITEWAR_NEXTSTAGE_WINDOW_MASTER_MESSAGE");
		else			msg = UISTRING_TEXT("INFINITEWAR_NEXTSTAGE_WINDOW_MEMBER_MESSAGE");
		
		std::wstring value = DmCS::StringFn::getNumberFormatW( GetSystem()->GetTotalHavePoint() );
		DmCS::StringFn::ReplaceAll( msg, L"#totalpoint#", value );
		
		std::wstring winPoint = DmCS::StringFn::getNumberFormatW( GetSystem()->GetNextStageWinPoint() );
		DmCS::StringFn::ReplaceAll( msg, L"#winPoint#", winPoint );
		
		std::wstring losePoint = DmCS::StringFn::getNumberFormatW( GetSystem()->GetNextStageLosePoint() );
		DmCS::StringFn::ReplaceAll( msg, L"#losePoint#", losePoint );

		DmCS::StringFn::ReplaceAll( msg, L"#curStage#", GetSystem()->GetLastClearStageIdx() );

		pMsg->SetText( msg.c_str(), 400);
	}

	m_pOk = AddButton( CsPoint( 482/2 - 150, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );
	if( m_pOk )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("INFINITEWAR_NEXTSTAGE_WINDOW_OK_BTN").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pOk->SetText( &ti );
		m_pOk->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarNextStagedWindow::_SendTryNextStageBtnEvent );
	}

	m_pWait = AddButton( CsPoint( 482/2 + 13, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );
	if( m_pWait )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("INFINITEWAR_NEXTSTAGE_WINDOW_WAIT_BTN").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pWait->SetText( &ti );
		m_pWait->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarNextStagedWindow::_WaitBtnEvent );
	}

	if( !bMaster )
	{
		if( m_pOk )
			m_pOk->SetEnable(false);
		if( m_pWait )
			m_pWait->SetEnable(false);
	}
}

void CInfiniteWarNextStagedWindow::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE CInfiniteWarNextStagedWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	//if( muReturn == MUT_OUT_WINDOW )	
	//	return muReturn;

	if( m_pOk->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;
	if( m_pWait->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;
	if( m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;
	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 482, 55 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

void CInfiniteWarNextStagedWindow::_WaitBtnEvent( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->OpenCloseNextStageTryWindow(false);
}

void CInfiniteWarNextStagedWindow::_SendTryNextStageBtnEvent(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->SendInfiniteWarTryNextStage();
}

void CInfiniteWarNextStagedWindow::Render()
{
	RenderScript();
}

void CInfiniteWarNextStagedWindow::ResetDevice()
{	
	ResetDeviceScript();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CInfiniteWarNextStagedWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CInfiniteWarNextStagedWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
// 	switch(iNotifiedEvt)
// 	{
// 	}
}