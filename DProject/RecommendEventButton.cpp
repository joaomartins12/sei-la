#include "StdAfx.h"
#include "RecommendEventButton.h"

CRecommendEventButton::CRecommendEventButton(void)
{
}

CRecommendEventButton::~CRecommendEventButton(void)
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void CRecommendEventButton::Create( int nValue /*= 0 */ )
{
	if( cBaseWindow::Init() == false )
		return; 

	InitScript( NULL, CsPoint( 966, 543 ), CsPoint( 41, 41 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM, true );
	m_EventButton.Init( GetRoot(), CsPoint::ZERO, CsPoint( 41, 41 ), "RecommendEvent\\FriendE_Btn.tga", false );
	m_EventButton.SetTexToken( CsPoint( 78, 0 ) );
	m_EventButton.AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CRecommendEventButton::PressEventButton );
}

void CRecommendEventButton::Render()
{
	RenderScript();
	m_EventButton.Render();
}

cBaseWindow::eMU_TYPE CRecommendEventButton::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	m_EventButton.Update_ForMouse();
	return muReturn;
}

void CRecommendEventButton::PressEventButton( void* pSender, void* pData )
{
	if( GetSystem() )
		GetSystem()->OpenRecommendEventWindow();
}

bool CRecommendEventButton::Construct( void )
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CRecommendEventButton::Notify( int const& iNotifiedEvt, ContentsStream const& kStream )
{

}

void CRecommendEventButton::ResetDevice()
{
	ResetDeviceScript();
	m_EventButton.ResetDevice();
}

cBaseWindow::eWINDOW_TYPE CRecommendEventButton::GetWindowType()
{
	return cBaseWindow::WT_RECOMMENDEVENT_BUTTON;
}

void CRecommendEventButton::Destroy()
{
	cBaseWindow::Delete();	
}

void CRecommendEventButton::DeleteResource()
{		
	DeleteScript(); 
}

void CRecommendEventButton::Update(float const& fDeltaTime)
{

}
