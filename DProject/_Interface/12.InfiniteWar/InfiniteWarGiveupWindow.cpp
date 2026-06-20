#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "InfiniteWarGiveupWindow.h"


CInfiniteWarGiveupWindow::CInfiniteWarGiveupWindow():m_pGiveupCancel(NULL),m_pGiveupOk(NULL),m_pCloseBtn(NULL)
{

}

CInfiniteWarGiveupWindow::~CInfiniteWarGiveupWindow()
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE CInfiniteWarGiveupWindow::GetWindowType()
{ 
	return WT_INFINITEWAR_GIVEUP_WINDOW; 
}

void CInfiniteWarGiveupWindow::Destroy()
{
	cBaseWindow::Delete();	
}

void CInfiniteWarGiveupWindow::DeleteResource()
{
	DeleteScript();	
}

void CInfiniteWarGiveupWindow::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint winPos( (g_nScreenWidth >> 1) - (482 >> 1), 100 );
	InitScript( "CommonUI\\CommonUI_popup_Warring.tga", winPos, CsPoint( 482, 288 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	AddTitle( UISTRING_TEXT("INFINITEWAR_GIVEUP_WINDOW_TITLE").c_str(), CsPoint(0,20), CFont::FS_16 );

	m_pCloseBtn = AddButton( CsPoint( 430, 21 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );	
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarGiveupWindow::_CloseWindow );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_13, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( L"" );
	ti.SetLineGabHeight(5);
	cText* pMsg = AddText( &ti, CsPoint( 241, 110 ));
	if( pMsg )
	{
		std::wstring msg = UISTRING_TEXT("INFINITEWAR_GIVEUP_WINDOW_MESSAGE");
		std::wstring value = DmCS::StringFn::getNumberFormatW( GetSystem()->GetTotalHavePoint() );
		DmCS::StringFn::ReplaceAll( msg, L"#totalpoint#", value );

		std::wstring returnMapName = GetSystem()->GetReturnMapName();
		DmCS::StringFn::ReplaceAll( msg, L"#returnmapname#", returnMapName );
		pMsg->SetText( msg.c_str(), 400);
	}

	m_pGiveupOk = AddButton( CsPoint( 482/2 - 150, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );
	if( m_pGiveupOk )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("INFINITEWAR_GIVEUP_WINDOW_OK_BTN").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pGiveupOk->SetText( &ti );
		m_pGiveupOk->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarGiveupWindow::_GiveupBtnEvent );
	}

	m_pGiveupCancel = AddButton( CsPoint( 482/2 + 13, 230), CsPoint( 137, 46 ), CsPoint( 0, 46 ), "CommonUI\\Simple_btn_S.tga", cWindow::SD_Bu3 );
	if( m_pGiveupCancel )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("INFINITEWAR_GIVEUP_WINDOW_CANCEL_BTN").c_str());
		ti.s_eTextAlign = DT_CENTER;
		m_pGiveupCancel->SetText( &ti );
		m_pGiveupCancel->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteWarGiveupWindow::_CloseWindow );
	}
}

void CInfiniteWarGiveupWindow::Update(float const& fDeltaTime)
{
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

bool CInfiniteWarGiveupWindow::OnEscapeKey()
{
	_CloseWindow(NULL, NULL);
	return true;
}

cBaseWindow::eMU_TYPE CInfiniteWarGiveupWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	//if( muReturn == MUT_OUT_WINDOW )	
	//	return muReturn;

	if( m_pGiveupOk->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;
	if( m_pGiveupCancel->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;
	if( m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 482, 55 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();	

	return muReturn;
}

void CInfiniteWarGiveupWindow::_CloseWindow( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->OpenCloseGiveupWindow(false);
}

void CInfiniteWarGiveupWindow::_GiveupBtnEvent(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->SendInfiniteWarGiveup();
}

void CInfiniteWarGiveupWindow::Render()
{
	RenderScript();
}

void CInfiniteWarGiveupWindow::ResetDevice()
{	
	ResetDeviceScript();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CInfiniteWarGiveupWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CInfiniteWarGiveupWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
// 	switch(iNotifiedEvt)
// 	{
// 	}
}