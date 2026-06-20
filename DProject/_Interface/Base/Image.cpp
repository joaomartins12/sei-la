
#include "stdafx.h"
#include "Image.h"

cImage::cImage():m_bEnable(false), m_bClick(false), m_bIsMouseOver(false),m_nXMaxFrame(1),m_nYMaxFrame(1),m_nCurrentFrame(-1),m_texTokenSizeY(1.0f),m_texTokenSizeX(1.0f)
,m_pMouseOverImg(NULL)
{

}

cImage::~cImage()
{
	if( m_pMouseOverImg )
		SAFE_NIDELETE( m_pMouseOverImg );
}

cWindow::eTYPE cImage::GetType() const
{ 
	return T_Image; 
}


void cImage::ResetDevice()
{
	cSprite::ResetDevice();

	if( m_pMouseOverImg )
		m_pMouseOverImg->ResetDevice();
}

void cImage::SetEnable(bool bEnable)
{
	m_bEnable = bEnable;
}

bool cImage::GetEnable() const
{
	return m_bEnable;
}

void cImage::Init( cWindow* pParent, CsPoint pos, CsPoint size, const char* cImagePath, bool bApplyWindowSize, bool bUseWorkingFolder /*=true*/ )
{
	cSprite::Init( pParent, pos, size, cImagePath, bApplyWindowSize, FONT_WHITE, bUseWorkingFolder );
}


int cImage::Update_ForMouse()
{
	if( !m_bVisible )
		return ACTION_NONE;

	CURSOR::eTYPE nType = Click();

	switch( nType )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
		{
			m_bClick = false;
		}
		else if( m_bClick == true )
		{
			return ACTION_PRESS;
		}
		return ACTION_NONE;
	case CURSOR::LBUTTON_DOWN:
		if( m_eSoundType != cWindow::SD_NONE )
		{
			PlaySound( m_eSoundType );
		}
		m_bClick = true;
		return ACTION_DOWN;
	case CURSOR::LBUTTON_PRESS:
		if( m_bClick == true )
		{
			return ACTION_PRESS;
		}
		break;
	case CURSOR::LBUTTON_UP:
		if( m_bClick == true )
		{
			OnEvent( IMAGE_LBUP_EVENT, NULL );
			return ACTION_CLICK;
		}
		break;
	default:
		{
			m_bClick = false;
			m_bIsMouseOver = true;
		}
		break;
	}
	return ACTION_ON;
}

void cImage::SetTexToken( CsPoint tex )
{	
	m_texTokenSizeX = (tex.x <= 0) ? 1.0f : tex.x * m_fInvTexWidth;
	m_texTokenSizeY = (tex.y <= 0) ? 1.0f : tex.y * m_fInvTexHeight;

	m_nXMaxFrame = 1.0f / m_texTokenSizeX;
	if( 0 == m_nXMaxFrame )
		m_nXMaxFrame = 1;

	m_nYMaxFrame = 1.0f / m_texTokenSizeY;
	if( 0 == m_nYMaxFrame )
		m_nYMaxFrame = 1;
	SetState( 0 );
}

void cImage::Render()
{
	if( !m_bVisible )
		return;

	cSprite::Render();

	if( m_bIsMouseOver && m_bEnable )
	{
		if( m_pMouseOverImg )
			m_pMouseOverImg->Render( m_ptClient );
		m_bIsMouseOver = false;
	}
}

void cImage::Render( CsPoint pos )
{
	if( !m_bVisible )
		return;
	cSprite::Render( pos );

	if( m_bIsMouseOver && m_bEnable )
	{
		if( m_pMouseOverImg )
			m_pMouseOverImg->Render( m_ptClient + pos );
		m_bIsMouseOver = false;
	}
}

void cImage::Render( CsPoint pos, int nImageIndex )
{ 
	SetState( nImageIndex ); 
	Render( pos ); 
}

void cImage::Render( int nImageIndex )
{ 
	SetState( nImageIndex ); 
	Render(); 
}

void cImage::SetState( int const& state )
{ 	
	if( m_nCurrentFrame == state )
		return;

	m_nCurrentFrame = state;

	int nXFramePos = state % m_nXMaxFrame; // X Frame
	int nYFramePos = (state / m_nXMaxFrame) % m_nYMaxFrame; // Y Frame	

	float left = nXFramePos * m_texTokenSizeX;
	float top = nYFramePos * m_texTokenSizeY;
	float right = left + m_texTokenSizeX;
	float bottom = top + m_texTokenSizeY;

	if( m_spScreen )
		m_spScreen->SetTextures( 0, 0, left, top, right, bottom );
}

int cImage::GetState() const
{ 
	return m_nCurrentFrame; 
}

void cImage::SetMouseOverImg(const char* cImagePath)
{
	if( !m_pMouseOverImg )
	{
		ResetDevice();
		m_pMouseOverImg = NiNew cSprite;
		m_pMouseOverImg->Init( NULL, CsPoint::ZERO, m_ptSize, cImagePath, false );
		return;
	}
}
