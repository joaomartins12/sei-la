
#include "stdafx.h"
#include "HelpTextBtn.h"

cHelpTextBtn::cHelpTextBtn()
{
}

void cHelpTextBtn::Destroy()
{
	cBaseWindow::Delete();	
}

void cHelpTextBtn::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pMask );
}

void cHelpTextBtn::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint vInitPos = CsPoint( 2, 160 );		
	InitScript( NULL, CsPoint( 492, 489 ), CsPoint( 40 ,40 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::BOTTOM, true );
	m_pOpenHelpText = AddButton( CsPoint::ZERO, CsPoint( 40, 40 ), CsPoint( 40, 0 ), "Help\\Button.tga" );

	m_bIncreaseAlpha = true;
	m_fAlpha = 0.0f;

	m_pMask = NiNew cSprite;
	m_pMask->Init( m_pRoot, CsPoint( -3, -3 ), CsPoint( 46, 46 ), "Help\\Button_Mask.tga", true );
	m_pMask->GetAlphaProp()->SetDestBlendMode( NiAlphaProperty::ALPHA_ONE );
	m_pMask->SetAlpha( m_fAlpha );
}

void cHelpTextBtn::Update(float const& fDeltaTime)
{
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

cBaseWindow::eMU_TYPE
cHelpTextBtn::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pOpenHelpText->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		cHelpText* pHelpText = (cHelpText*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_HELPTEXT );
		pHelpText->GetTitle()->SetSelectTitle( m_nHelpTextUniqID );
		pHelpText->GetBody()->SetLine( m_nBodyLine );
		Close();
	}

	return muReturn;
}

void cHelpTextBtn::Render()
{
	RenderScript();

	m_pMask->Render();
}

void cHelpTextBtn::ResetDevice()
{	
	ResetDeviceScript();

	m_pMask->ResetDevice();
}