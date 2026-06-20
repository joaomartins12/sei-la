
#include "stdafx.h"
#include "MainFrame_TamerUI.h"
// 
// cTamerMainFrame::cTamerMainFrame()
// :m_pHPBarMin(NULL),m_pHPBarMax(NULL),m_pFPBar(NULL),m_pHostSprite(NULL),m_pTamerImg(NULL)
// {
// }
// 
// cTamerMainFrame::~cTamerMainFrame()
// {
// 
// }
// 
// cBaseWindow::eWINDOW_TYPE cTamerMainFrame::GetWindowType()
// {
// 	return WT_TAMER_WINDOW;
// }
// 
// void cTamerMainFrame::Destroy()
// {
// }
// 
// void cTamerMainFrame::DeleteResource()
// {
// 	DeleteScript();
// }
// 
// void cTamerMainFrame::Create( int nValue /* = 0  */)
// {
// 	if( cBaseWindow::Init() == false )
// 		return;
// 
// 	InitScript( NULL, CsPoint::ZERO, CsPoint( 236, 96 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
// 	AddSprite( CsPoint::ZERO, CsPoint( 236, 96 ), "TargetWindow\\TamerTarget.tga" );
// 
// 	cText::sTEXTINFO ti;
// 	ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
// 	ti.s_eTextAlign = DT_CENTER;	
// 	m_pNameText = AddText( &ti, CsPoint( 162, 20 ) );// ÀÌ¸§	
// 
// 	AddSprite( CsPoint( 0, 69 ), CsPoint( 35, 35 ), "TargetWindow\\TamerLevel.tga" );
// 	m_pLevelText = AddText( &ti, CsPoint( 18, 78 ) );// ·¹º§
// 
// 	std::string sFileName = GetSystem()->GetTamerImgFile();
// 	m_pTamerImg = AddSprite( CsPoint( 6, 6 ), CsPoint( 84, 84 ), sFileName.c_str() );
// }
// 
// void cTamerMainFrame::Update(float const& fDeltaTime)
// {	
// }
// 
// cBaseWindow::eMU_TYPE cTamerMainFrame::Update_ForMouse()
// {
// 	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
// 	if( muReturn == MUT_OUT_WINDOW )
// 		return muReturn;
// 
// 	return muReturn;
// }
// 
// void cTamerMainFrame::Render()
// {
// 	RenderScript();
// }
// 
// void cTamerMainFrame::OnRButtonUp( CsPoint pos )
// {
// }
// 
// void cTamerMainFrame::OnLButtonUp( CsPoint pos )
// {
// }
// 
// void cTamerMainFrame::ResetDevice()
// {
// 	ResetDeviceScript();
// }
// 
// //////////////////////////////////////////////////////////////////////////
// 
// bool cTamerMainFrame::Construct(void)
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
// void cTamerMainFrame::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
// {
// }