#include "stdafx.h"
#include "ScreenOutUI.h"

cScreenOutUI::cScreenOutUI()
{

}

cScreenOutUI::~cScreenOutUI()
{

}

cBaseWindow::eWINDOW_TYPE cScreenOutUI::GetWindowType()
{
	return WT_SCREENOUT_UI;
}

void cScreenOutUI::Create(int nValue /*= 0 */)
{
	if( !cBaseWindow::Init() )
		return;

	InitScript( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );
	cSprite* pBlack = NiNew cSprite;
	if( pBlack )
	{
		pBlack->Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), NiColorA( 0, 0, 0, 1 ), false );
		AddChildControl( pBlack );
	}
}

void cScreenOutUI::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

cBaseWindow::eMU_TYPE cScreenOutUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	return muReturn;
}

void cScreenOutUI::Destroy()
{
	cBaseWindow::Delete();
}

void cScreenOutUI::DeleteResource()
{
	DeleteScript();
}

void cScreenOutUI::Render()
{
	RenderScript();
}

void cScreenOutUI::ResetDevice()
{
	ResetDeviceScript();
}

void cScreenOutUI::ResetMap()
{
	Close();
}

