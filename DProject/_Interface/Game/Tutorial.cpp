
#include "stdafx.h"
// #include "Tutorial.h"
// 
// 
// cTutorial::cTutorial()
// {
// }
// 
// void cTutorial::Destroy()
// {
// 	cBaseWindow::Delete();	
// }
// 
// void cTutorial::DeleteResource()
// {
// 	DeleteScript();	
// }
// 
// void cTutorial::Create( int nValue /*=0*/ )
// {
// 	if( cBaseWindow::Init() == false )
// 		return;
// 
// 	InitScript( "_Etc\\CameraCon_Win.tga", CsPoint( 274, 110 ), CsPoint( 475, 547 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );	
// 
// 	m_pStaticCameraBtn = AddButton( CsPoint( 316, 360 ), CsPoint( 76, 26 ), cButton::IMAGE_NOR_1, _LAN( "고정 시점" ) );	
// 	m_pFreeCameraBtn = AddButton( CsPoint( 77, 360 ), CsPoint( 76, 26 ), cButton::IMAGE_NOR_1, _LAN( "자유 시점" ) );
// }
// 
// void cTutorial::Update(float const& fDeltaTime)
// {	
// }
// 
// cBaseWindow::eMU_TYPE
// cTutorial::Update_ForMouse()
// {
// 	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
// 	if( muReturn == MUT_OUT_WINDOW )	
// 		return muReturn;
// 
// 	if( m_pStaticCameraBtn->Update_ForMouse() == cButton::ACTION_CLICK )
// 	{
// 		g_pResist->m_Character.s_bStaticCamera = true;
// 		g_pResist->m_Character.s_nCameraMaxDistLevel = 4;
// 		Close();
// 		return muReturn;
// 	}
// 
// 	if( m_pFreeCameraBtn->Update_ForMouse() == cButton::ACTION_CLICK )
// 	{
// 		g_pResist->m_Character.s_bStaticCamera = false;
// 		g_pResist->m_Character.s_nCameraMaxDistLevel = 4;
// 		Close();
// 		return muReturn;
// 	}
// 
// 	return muReturn;
// }
// 
// void cTutorial::Render()
// {
// 	RenderScript();	
// }
// 
// 
// void cTutorial::ResetDevice()
// {	
// 	ResetDeviceScript();
// }
// 
// 
// 
