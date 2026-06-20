#include "stdafx.h"
// 
// #include "MainFrame_DigimonUI.h"
// 
// cDigimonMainFrame::cDigimonMainFrame()
// {
// }
// 
// cDigimonMainFrame::~cDigimonMainFrame()
// {
// }
// 
// cBaseWindow::eWINDOW_TYPE cDigimonMainFrame::GetWindowType()
// { 
// 	return WT_MAIN_DIGIMON_WINDOW; 
// }
// 
// void cDigimonMainFrame::Destroy()
// {	
// 	cBaseWindow::Delete();	
// }
// 
// void cDigimonMainFrame::DeleteResource()
// {
// 	DeleteScript();
// }
// 
// void cDigimonMainFrame::ResetDevice()
// {
// 	ResetDeviceScript();
// }
// 
// void cDigimonMainFrame::Create( int nValue /*=0*/ )
// {
// 	if( cBaseWindow::Init() == false )
// 		return;
// 
// 	InitScript( NULL, CsPoint( 22, 96 ), CsPoint( 214, 65 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
// 
// 	AddSprite( CsPoint::ZERO, CsPoint( 214, 65 ), "TargetWindow\\DigimonTarget.tga" );
// 	// 디지몬 레벨 배경
// 	AddSprite( CsPoint( -5, 42 ), CsPoint( 28, 27 ), "TargetWindow\\DigimnMonsterlevel.tga" );
// 
// 	{// 이름
// 		cText::sTEXTINFO ti;
// 		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
// 		ti.s_eTextAlign = DT_CENTER;		
// 		m_pNameText = AddText( &ti, CsPoint( 139, 7 ) );
// 	}
// 
// 	{
// 		cText::sTEXTINFO ti;
// 		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
// 		ti.s_eTextAlign = DT_CENTER;		
// 		m_pLevelText = AddText( &ti, CsPoint( 9, 49 ) );// 레벨
// 	}
// 
// 	// hp
// 	m_pHPBar = NiNew cProgressBar;
// 	m_pHPBar->Init( m_pRoot, cProgressBar::BAR_3IMAGE, CsPoint( 80, 28 ), CsPoint( 126, 8 ), CsPoint( 2, 2 ), "TargetWindow\\HpBar1.tga", false );		
// 	m_pHPBar->SetRenderPosText( cProgressBar::TEXT_VALUE );
// 	AddChildControl( m_pHPBar );
// 	// Ds
// 	m_pDSBar = NiNew cProgressBar;
// 	m_pDSBar->Init( m_pRoot, cProgressBar::BAR_3IMAGE, CsPoint( 80, 42 ), CsPoint( 126, 8 ), CsPoint( 2, 2 ), "TargetWindow\\DsBar1.tga", false );
// 	m_pDSBar->SetRenderPosText( cProgressBar::TEXT_VALUE );
// 	AddChildControl( m_pDSBar );
// 
// 	// Exp
// 	m_pExpBar = NiNew cProgressBar;
// 	m_pExpBar->Init( m_pRoot, cProgressBar::BAR_1IMAGE, CsPoint( 55, 56 ), CsPoint( 157, 5 ), CsPoint::ZERO, "TargetWindow\\Expbar.tga", false );
// 	AddChildControl( m_pExpBar );
// 
// 	{
// 		cText::sTEXTINFO ti;
// 		ti.Init( &g_pEngine->m_FontText, CFont::FS_9 );
// 		ti.s_eTextAlign = DT_RIGHT;
// 		ti.SetText( _T( "" ) );
// 		m_pExpPercentText = AddText( &ti, CsPoint( 212, 64 ) );
// 	}
// 
// 	// 전투 표시
// 	m_pBattleSprite = AddSprite( CsPoint::ZERO, CsPoint( 214, 65 ), "TargetWindow\\DigimonTarget_Mask.tga" );
// 	if( m_pBattleSprite )
// 		m_pBattleSprite->SetVisible(false);
// 
// // 	m_bBattle = false;
// // 	m_fBattleAlpha = -1.0f;
// }
// 
// void cDigimonMainFrame::Update(float const& fDeltaTime)
// {
// }
// 
// cBaseWindow::eMU_TYPE cDigimonMainFrame::Update_ForMouse()
// {
// 	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
// 	if( muReturn == MUT_OUT_WINDOW )
// 		return muReturn;
// 
// 	return muReturn;
// }
// 
// void cDigimonMainFrame::Render()
// {
// 	RenderScript();
// }
// 
// void cDigimonMainFrame::OnLButtonUp( CsPoint pos )
// {
// 	if( IsInWindow( pos ) == false )
// 		return;
// 
// }
// 
// 
// //////////////////////////////////////////////////////////////////////////
// 
// bool cDigimonMainFrame::Construct(void)
// {
// 	if( !CONTENTSSYSTEM_PTR )
// 		return false;
// 
// 	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
// 	if( !GetSystem() )
// 		return false;
// 	return true;
// }
// 
// void cDigimonMainFrame::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
// {
// }