
#include "stdafx.h"
// 
// #include "MainFrame_TacticsUI.h"
// 
// cTacticsMainFrame::cTacticsMainFrame()
// {
// }
// 
// cTacticsMainFrame::~cTacticsMainFrame()
// {
// }
// 
// void cTacticsMainFrame::Destroy()
// {
// 	cBaseWindow::Delete();	
// }
// 
// void cTacticsMainFrame::DeleteResource()
// {
// 	DeleteScript();
// }
// 
// cBaseWindow::eWINDOW_TYPE cTacticsMainFrame::GetWindowType()
// { 
// 	return WT_TACTICS_WINDOW; 
// }
// 
// void cTacticsMainFrame::Create( int nValue /*=0*/ )
// {
// 	if( cBaseWindow::Init() == false )
// 		return;
// 
// 	InitScript( "TargetWindow\\TacticsTarget.tga", CsPoint( 76, 158 ), CsPoint( 36, 36 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
// 
// 
// }
// 
// void cTacticsMainFrame::Update(float const& fDeltaTime)
// {
// }
// 
// cBaseWindow::eMU_TYPE cTacticsMainFrame::Update_ForMouse()
// {
// 	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
// 	if( muReturn == MUT_OUT_WINDOW )
// 		return muReturn;
// 
// 	return muReturn;
// }
// 
// void cTacticsMainFrame::Render()
// {
// 	RenderScript();
// }
// 
// void cTacticsMainFrame::OnLButtonUp( CsPoint pos )
// {
// }
// 
// void cTacticsMainFrame::OnRButtonUp( CsPoint pos )
// {
// }
// 
// void cTacticsMainFrame::ResetDevice()
// {
// 	ResetDeviceScript();
// 
// }
// 
// 
// //////////////////////////////////////////////////////////////////////////
// 
// bool cTacticsMainFrame::Construct(void)
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
// void cTacticsMainFrame::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
// {
// }