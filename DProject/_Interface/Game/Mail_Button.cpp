#include "stdafx.h"
#include "Mail_Button.h"

cMailButton::cMailButton():m_pOpenButton(NULL),m_pMask(NULL),m_bIncreaseAlpha(true), m_fAccumTime(0.0f), m_fAlpha(0.0f)
{

}

cMailButton::~cMailButton()
{

}

cBaseWindow::eWINDOW_TYPE	cMailButton::GetWindowType()
{ 
	return WT_MAIL_BUTTON; 
}

void cMailButton::Destroy()
{	
	cBaseWindow::Delete();	
}

void cMailButton::DeleteResource()
{		
	DeleteScript();
	SAFE_NIDELETE( m_pMask );
}

void cMailButton::Create(int nValue /* = 0  */)
{		
	if( cBaseWindow::Init() == false )
		return;

	InitScript( NULL, CsPoint( 483, 520 ), CsPoint( 112 ,66 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::BOTTOM, true );
	m_pOpenButton = AddButton( CsPoint::ZERO, CsPoint( 74, 48 ), CsPoint( 0, 66 ), "Mail\\Post.tga" );	
	if( m_pOpenButton )
		m_pOpenButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cMailButton::_OpenMailWindow );

	m_pMask = NiNew cSprite;
	m_pMask->Init( m_pRoot, CsPoint( -3, -3 ), CsPoint( 74, 48 ), "Help\\Button_Mask.tga", true );
	m_pMask->GetAlphaProp()->SetDestBlendMode( NiAlphaProperty::ALPHA_ONE );
	m_pMask->SetAlpha( m_fAlpha );	
}

void cMailButton::_OpenMailWindow(void* pSender, void* pData)
{
	GetSystem()->OpenMailWindow();
	Close();	
}

cBaseWindow::eMU_TYPE cMailButton::Update_ForMouse()
{	
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	

	if( m_pOpenButton && m_pOpenButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;
		 
	return muReturn;
}

void cMailButton::Update(float const& fDeltaTime)
{
	SAFE_POINTER_RET( m_pMask );

	m_fAccumTime += fDeltaTime; 	
	if( m_bIncreaseAlpha == true )
	{
		m_fAlpha += 0.25f*fDeltaTime;
		if( m_fAlpha >= 0.25f )
		{
			m_fAlpha = 0.25f;
			m_bIncreaseAlpha = false;
		}
	}
	else
	{
		m_fAlpha -= 0.5f*fDeltaTime;
		if( m_fAlpha <= 0.0f )
		{
			m_fAlpha = 0.0f;
			m_bIncreaseAlpha = true;
		}
	}
	m_pMask->SetAlpha( m_fAlpha );
}

void cMailButton::Render()
{
	RenderScript();	
	m_pMask->Render();
}

void cMailButton::ResetDevice()
{	
	ResetDeviceScript();	
	m_pMask->ResetDevice();
}



bool cMailButton::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	return true;
}



void cMailButton::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}