
#include "stdafx.h"
#include "Sprite.h"

//=======================================================================================================================
//
//		static
//
//=======================================================================================================================

NiAlphaProperty*			cSprite::m_pAlphaProp = NULL;
NiTexturingProperty*		cSprite::m_pTexProp = NULL;
NiZBufferProperty*			cSprite::m_pZBufferProp = NULL;
NiVertexColorProperty*		cSprite::m_pVertexColorProp = NULL;

void cSprite::GlobalInit()
{
	assert_cs( m_pAlphaProp == NULL );
	m_pAlphaProp = NiNew NiAlphaProperty;
	m_pAlphaProp->SetAlphaBlending(true);

	assert_cs( m_pTexProp == NULL );
	m_pTexProp = NiNew NiTexturingProperty;	
	m_pTexProp->SetBaseFilterMode(NiTexturingProperty::FILTER_BILERP);
	m_pTexProp->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);

	assert_cs( m_pZBufferProp == NULL );
	m_pZBufferProp = NiNew NiZBufferProperty;
	m_pZBufferProp->SetZBufferTest( false );
	m_pZBufferProp->SetZBufferWrite( false );

	assert_cs( m_pVertexColorProp == NULL );
	m_pVertexColorProp = NiNew NiVertexColorProperty;
	m_pVertexColorProp->SetSourceMode( NiVertexColorProperty::SOURCE_EMISSIVE );
	m_pVertexColorProp->SetLightingMode( NiVertexColorProperty::LIGHTING_E );	
}

void cSprite::GlobalShotDown()
{
	SAFE_NIDELETE( m_pAlphaProp );
	SAFE_NIDELETE( m_pTexProp );	
	SAFE_NIDELETE( m_pZBufferProp );
	SAFE_NIDELETE( m_pVertexColorProp );
}

//=======================================================================================================================
//
//		Base
//
//=======================================================================================================================

cSprite::cSprite():m_spScreen(NULL),m_dwLimitPlag(LIMIT_NONE)
,m_pTexter(NULL), m_bClick(false)
{
}

cSprite::~cSprite()
{	
	Delete(); 
}

void cSprite::Delete()
{ 
	m_spScreen = 0; 
	m_dwLimitPlag = LIMIT_NONE;
	RESOURCEMGR_ST.RemoveTexture( m_pTexter );
}

void cSprite::Init( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize, NiColor color /*FONT_WHITE*/, bool bUseWorkingFolder /*=true*/ )
{
	assert_cs( cImagePath != NULL );

	m_strFileName.assign(cImagePath);

	std::string strFile;
	if( bUseWorkingFolder == true )
		DmCS::StringFn::FormatA( strFile, "%s%s", m_cWorkingFolder, cImagePath );
	else
		strFile = cImagePath;

	m_pTexter = RESOURCEMGR_ST.GetTexture( strFile.c_str());
	Init( pParent, pos, size, m_pTexter->GetTexture(), bApplyWindowSize, color );
}

void cSprite::Init( cWindow* pParent, CsPoint pos, CsPoint size, CsRect TextureRect, char const* cImagePath, bool bApplyWindowSize, NiColor color /*FONT_WHITE*/, bool bUseWorkingFolder /*=true*/ )
{
	assert_cs( cImagePath != NULL );

	m_strFileName.assign(cImagePath);

	std::string strFile;
	if( bUseWorkingFolder == true )
		DmCS::StringFn::FormatA( strFile, "%s%s", m_cWorkingFolder, cImagePath );
	else
		strFile = cImagePath;

	m_pTexter = RESOURCEMGR_ST.GetTexture( strFile.c_str());
	Init( pParent, pos, size, TextureRect, m_pTexter->GetTexture(), bApplyWindowSize, color );
}

void cSprite::Init( cWindow* pParent, CsPoint pos, CsPoint size, NiTexture* pTexture, bool bApplyWindowSize, NiColor color /*FONT_WHITE*/ )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );

	assert_cs( m_pAlphaProp != NULL );
	assert_cs( pTexture != NULL );
	
	m_fInvTexWidth = 1.0f/(float)pTexture->GetWidth();
	m_fInvTexHeight = 1.0f/(float)pTexture->GetHeight();

	assert_cs( m_spScreen == NULL );
	m_spScreen = NiNew NiScreenElements( NiNew NiScreenElementsData( false, true, 1 ) );
	assert_cs( m_spScreen != NULL );
	m_spScreen->Insert(4);	

	_CalRectangle();
	m_spScreen->SetTextures( 0, 0, 0.0f, 0.0f, 1.0f, 1.0f );

	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pAlphaProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pTexProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pZBufferProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pVertexColorProp->Clone() ) );

	( (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ) )->SetBaseTexture( pTexture );

	m_spScreen->UpdateProperties();	

	m_Color = NiColorA( color.r, color.g, color.b, 1.0f );
	m_spScreen->SetColors( 0, m_Color );
	m_spScreen->Update( 0.0f, false );	
}

void cSprite::Init( cWindow* pParent, CsPoint pos, CsPoint size, CsRect TextureRect, NiTexture* pTexture, bool bApplyWindowSize, NiColor color /*FONT_WHITE*/ )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );

	assert_cs( m_pAlphaProp != NULL );
	assert_cs( pTexture != NULL );

	m_fInvTexWidth = 1.0f/(float)pTexture->GetWidth();
	m_fInvTexHeight = 1.0f/(float)pTexture->GetHeight();

	assert_cs( m_spScreen == NULL );
	m_spScreen = NiNew NiScreenElements( NiNew NiScreenElementsData( false, true, 1 ) );
	assert_cs( m_spScreen != NULL );
	m_spScreen->Insert(4);	

	_CalRectangle();

	float fUVx = (float)TextureRect.left / (float)pTexture->GetWidth();
	float fUVy = (float)TextureRect.top / (float)pTexture->GetHeight();
	float fUVw = (float)TextureRect.right / (float)pTexture->GetWidth();
	float fUVh = (float)TextureRect.bottom / (float)pTexture->GetHeight();

	m_spScreen->SetTextures( 0, 0, fUVx, fUVy, fUVw, fUVh );

	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pAlphaProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pTexProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pZBufferProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pVertexColorProp->Clone() ) );

	( (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ) )->SetBaseTexture( pTexture );

	m_spScreen->UpdateProperties();	

	m_Color = NiColorA( color.r, color.g, color.b, 1.0f );
	m_spScreen->SetColors( 0, m_Color );
	m_spScreen->Update( 0.0f, false );	
}


void cSprite::Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize, NiColor color /*=FONT_WHITE*/ )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );

	assert_cs( m_pAlphaProp != NULL );

	m_fInvTexWidth = 0.0f;
	m_fInvTexHeight = 0.0f;

	assert_cs( m_spScreen == NULL );
	m_spScreen = NiNew NiScreenElements( NiNew NiScreenElementsData( false, true, 1 ) );
	assert_cs( m_spScreen != NULL );
	m_spScreen->Insert(4);	

	_CalRectangle();
	m_spScreen->SetTextures( 0, 0, 0.0f, 0.0f, 1.0f, 1.0f );

	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pAlphaProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pTexProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pZBufferProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pVertexColorProp->Clone() ) );
	m_spScreen->UpdateProperties();	

	m_Color = NiColorA( color.r, color.g, color.b, 1.0f );
	m_spScreen->SetColors( 0, m_Color );
	m_spScreen->Update( 0.0f, false );
}

void cSprite::Init( cWindow* pParent, CsPoint pos, CsPoint size, NiColorA colorA, bool bApplyWindowSize )
{
	assert_cs( m_spScreen == NULL );

	NiPixelData* pPixelData = NiNew NiPixelData( 1, 1, NiPixelFormat::RGBA32 );
	DWORD* pPixel = (DWORD*)pPixelData->GetPixels();
	pPixel[ 0 ] = 0xffffffff;
	Init( pParent, pos, size, NiSourceTexture::Create( pPixelData ), bApplyWindowSize );
	SetColorA( colorA );
}


void cSprite::InitStencil( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize, NiStencilProperty* pStencilProp, NiColor color /* = FONT_WHITE */, bool bUseWorkingFolder /* = true  */)
{
	assert_cs( cImagePath != NULL );

	char filename[MAX_PATH];
	sprintf_s( filename, MAX_PATH, "Data\\Interface\\%s", cImagePath );
	RESOURCEMGR_ST.RemoveTexture( m_pTexter );
	m_pTexter = RESOURCEMGR_ST.GetTexture( filename );
	InitStencil( pParent, pos, size, m_pTexter->GetTexture(), bApplyWindowSize, pStencilProp, color );
}

void cSprite::InitStencil( cWindow* pParent, CsPoint pos, CsPoint size, NiTexture* pTexture, bool bApplyWindowSize, NiStencilProperty* pStencilProp, NiColor color /*FONT_WHITE*/ )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );

	assert_cs( m_pAlphaProp != NULL );
	assert_cs( pTexture != NULL );

	m_fInvTexWidth = 1.0f/(float)pTexture->GetWidth();
	m_fInvTexHeight = 1.0f/(float)pTexture->GetHeight();

	assert_cs( m_spScreen == NULL );
	m_spScreen = NiNew NiScreenElements( NiNew NiScreenElementsData( false, true, 1 ) );
	assert_cs( m_spScreen != NULL );
	m_spScreen->Insert(4);	

	_CalRectangle();
	m_spScreen->SetTextures( 0, 0, 0.0f, 0.0f, 1.0f, 1.0f );

	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pAlphaProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pTexProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pZBufferProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pVertexColorProp->Clone() ) );
	m_spScreen->AttachProperty( pStencilProp );

	( (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ) )->SetBaseTexture( pTexture );

	m_spScreen->UpdateProperties();	

	m_Color = NiColorA( color.r, color.g, color.b, 1.0f );

	m_spScreen->SetColors( 0, m_Color );
	m_spScreen->Update( 0.0f, false );	
}

void cSprite::InitStencil( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize, NiStencilProperty* pStencilProp, NiColor color /* = FONT_WHITE  */)
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );

	assert_cs( m_pAlphaProp != NULL );

	m_fInvTexWidth = 0.0f;
	m_fInvTexHeight = 0.0f;

	assert_cs( m_spScreen == NULL );
	m_spScreen = NiNew NiScreenElements( NiNew NiScreenElementsData( false, true, 1 ) );
	assert_cs( m_spScreen != NULL );
	m_spScreen->Insert(4);	

	_CalRectangle();
	m_spScreen->SetTextures( 0, 0, 0.0f, 0.0f, 1.0f, 1.0f );

	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pAlphaProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pTexProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pZBufferProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pVertexColorProp->Clone() ) );	
	m_spScreen->AttachProperty( pStencilProp );

	m_spScreen->UpdateProperties();	

	m_Color = NiColorA( color.r, color.g, color.b, 1.0f );
	m_spScreen->SetColors( 0, m_Color );
	m_spScreen->Update( 0.0f, false );
}

// void cSprite::InitTex2( cWindow* pParent, CsPoint pos, CsPoint size, char const* cImagePath, bool bApplyWindowSize )
// {
// 	assert_cs( cImagePath != NULL );
// 
// 	char filename[MAX_PATH];
// 	sprintf_s( filename, MAX_PATH, "%s%s", m_cWorkingFolder, cImagePath );
// 	m_pTexter = RESOURCEMGR_ST.GetTexture( filename );
// 	NiTexturePtr pTexture = m_pTexter->GetTexture();
// 
// 	cWindow::Init( pParent, pos, size, bApplyWindowSize );
// 	assert_cs( m_pAlphaProp != NULL );
// 	assert_cs( pTexture != NULL );
// 	m_fInvTexWidth = 1.0f/(float)pTexture->GetWidth();
// 	m_fInvTexHeight = 1.0f/(float)pTexture->GetHeight();
// 
// 	assert_cs( m_spScreen == NULL );
// 	m_spScreen = NiNew NiScreenElements( NiNew NiScreenElementsData( false, true, 2 ) );
// 	assert_cs( m_spScreen != NULL );
// 	m_spScreen->Insert(4);	
// 
// 	_CalRectangle();
// 	m_spScreen->SetTextures( 0, 0, 0.0f, 0.0f, 1.0f, 1.0f );
// 
// 	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pAlphaProp->Clone() ) );
// 	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pTexProp->Clone() ) );
// 	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pZBufferProp->Clone() ) );
// 	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pVertexColorProp->Clone() ) );
// 
// 	( (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ) )->SetBaseTexture( pTexture );
// 	m_spScreen->UpdateProperties();	
// 
// 	m_Color = NiColorA::WHITE;
// 	m_spScreen->SetColors( 0, m_Color );
// 	m_spScreen->Update( 0.0f, false );	
// }

void cSprite::SetRotation( float fAngle, CsPoint ptSize )
{
	float witdh = 1.0f / (float)g_nScreenWidth;
	float height = 1.0f / (float)g_nScreenHeight;

	float fX = witdh * (float)GetClient().x;
	float fY = height * (float)GetClient().y;

	NiPoint2 v;

	float fSizeX = (float)ptSize.x;
	float fSizeY = (float)ptSize.y;

	v.x = -fSizeX;
	v.y = -fSizeY;

	float cos = cosf( fAngle );
	float sin = sinf( fAngle );

	float xx = v.x * cos - v.y * sin;
	float yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	if( m_spScreen )
		m_spScreen->SetVertex( 0, 0, v );

	v.x = -fSizeX;
	v.y = fSizeY;

	xx = v.x * cos - v.y * sin;
	yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	if( m_spScreen )
		m_spScreen->SetVertex( 0, 1, v );

	v.x = fSizeX;
	v.y = fSizeY;

	xx = v.x * cos - v.y * sin;
	yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	if( m_spScreen )
		m_spScreen->SetVertex( 0, 2, v );

	v.x = fSizeX;
	v.y = -fSizeY;

	xx = v.x * cos - v.y * sin;
	yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	if( m_spScreen )
		m_spScreen->SetVertex( 0, 3, v );
}

void cSprite::SetRotation2( float fAngle )
{
	m_ptClient.x += m_ptSize.x/2;
	m_ptClient.y += m_ptSize.y/2;
	float witdh = 1.0f / (float)g_nScreenWidth;
	float height = 1.0f / (float)g_nScreenHeight;

	float fX = witdh * (float)GetClient().x;
	float fY = height * (float)GetClient().y;

	NiPoint2 v;

	float fSizeX = (float)m_ptSize.x/2;
	float fSizeY = (float)m_ptSize.y/2;

	v.x = -fSizeX;
	v.y = -fSizeY;

	float cos = cosf( fAngle );
	float sin = sinf( fAngle );

	float xx = v.x * cos - v.y * sin;
	float yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	if( m_spScreen )
		m_spScreen->SetVertex( 0, 0, v );

	v.x = -fSizeX;
	v.y = fSizeY;

	xx = v.x * cos - v.y * sin;
	yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	if( m_spScreen )
		m_spScreen->SetVertex( 0, 1, v );

	v.x = fSizeX;
	v.y = fSizeY;

	xx = v.x * cos - v.y * sin;
	yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	if( m_spScreen )
		m_spScreen->SetVertex( 0, 2, v );

	v.x = fSizeX;
	v.y = -fSizeY;

	xx = v.x * cos - v.y * sin;
	yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	if( m_spScreen )
		m_spScreen->SetVertex( 0, 3, v );
}

int cSprite::Update_ForMouse()
{
	if( !m_bVisible )
		return ACTION_NONE;

	CURSOR::eTYPE nType = Click();

	switch( nType )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		{
			if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_PRESS )
			{
				m_bClick = false;
			}
			else if( m_bClick == true )
			{
				return ACTION_PRESS;
			}
		}
		return ACTION_NONE;
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			m_bClick = true;
		}
		return ACTION_DOWN;
	case CURSOR::LBUTTON_PRESS:
	case CURSOR::RBUTTON_PRESS:
		{
			if( m_bClick == true )
			{
				return ACTION_PRESS;
			}
		}
		break;
	case CURSOR::LBUTTON_UP:
		{
			if( m_bClick == true )
			{
				OnEvent( SPRITE_LBUP_EVENT, NULL );
				return ACTION_CLICK;
			}
		}
		break;
	case CURSOR::RBUTTON_UP:
		{
			if( m_bClick == true )
			{
				OnEvent( SPRITE_RBUP_EVENT, NULL );
				return ACTION_CLICK;
			}
		}
		break;
	default:
		{
			m_bClick = false;
		}
		break;
	}
	return ACTION_ON;
}

void cSprite::Render()
{ 
	if( !m_bVisible )
		return;

	if( m_spScreen )
		m_spScreen->Draw( g_pRenderer ); 
}

void cSprite::Render( CsPoint pos /*느리다*/ )
{
	if( m_ptTempPos != pos )
	{
		m_ptTempPos = pos;		
		float fX = ( 1.0f / (float)g_nScreenWidth ) * (float)( m_ptClient.x + m_ptTempPos.x );
		float fY = ( 1.0f / (float)g_nScreenHeight ) * (float)( m_ptClient.y + m_ptTempPos.y );
		float fSX = ( 1.0f / (float)g_nScreenWidth ) * (float)m_ptSize.x;
		float fSY = ( 1.0f / (float)g_nScreenHeight ) * (float)m_ptSize.y;

		if( m_spScreen )
			m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
	}

	Render();
} 
void cSprite::Render( CsPoint pos, CsPoint size /*느리다*/ )
{
	if( ( m_ptTempPos != pos )||( m_ptSize != size ) )
	{
		m_ptTempPos = pos;
		m_ptSize = size;

		float fX = ( 1.0f / (float)g_nScreenWidth ) * (float)( m_ptClient.x + m_ptTempPos.x );
		float fY = ( 1.0f / (float)g_nScreenHeight ) * (float)( m_ptClient.y + m_ptTempPos.y );
		float fSX = ( 1.0f / (float)g_nScreenWidth ) * (float)m_ptSize.x;
		float fSY = ( 1.0f / (float)g_nScreenHeight ) * (float)m_ptSize.y;

		if( m_spScreen )
			m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
	}

	Render();
}

void cSprite::SetTextureUV( NiPoint2 texLT, NiPoint2 texRB )
{
	if( m_vTempTex != texLT )
	{
		m_vTempTex = texLT;
		if( m_spScreen )
			m_spScreen->SetTextures( 0, 0, texLT.x, texLT.y, texRB.x, texRB.y );
	}
}

void cSprite::Render( NiPoint2 texLT, NiPoint2 texRB )
{
	if( m_vTempTex != texLT )
	{
		m_vTempTex = texLT;
		if( m_spScreen )
			m_spScreen->SetTextures( 0, 0, texLT.x, texLT.y, texRB.x, texRB.y );
	}

	Render();
}

void cSprite::Render( CsPoint pos, NiPoint2 texLT, NiPoint2 texRB )
{
	if( ( m_ptTempPos != pos )||( m_vTempTex != texLT ) )
	{
		m_ptTempPos = pos;
		m_vTempTex = texLT;
		float fX = ( 1.0f / (float)g_nScreenWidth ) * (float)( m_ptClient.x + m_ptTempPos.x );
		float fY = ( 1.0f / (float)g_nScreenHeight ) * (float)( m_ptClient.y + m_ptTempPos.y );
		float fSX = ( 1.0f / (float)g_nScreenWidth ) * (float)m_ptSize.x;
		float fSY = ( 1.0f / (float)g_nScreenHeight ) * (float)m_ptSize.y;

		if( m_spScreen )
		{
			m_spScreen->SetTextures( 0, 0, texLT.x, texLT.y, texRB.x, texRB.y );
			m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
		}
	}

	Render();
}

void cSprite::Render( CsPoint pos, CsPoint size, NiPoint2 texLT, NiPoint2 texRB )
{
	if( ( m_ptTempPos != pos )||( m_ptSize != size )||( m_vTempTex != texLT ) )
	{
		m_ptTempPos = pos;
		m_vTempTex = texLT;
		m_ptSize = size;

		float fX = ( 1.0f / (float)g_nScreenWidth ) * (float)( m_ptClient.x + m_ptTempPos.x );
		float fY = ( 1.0f / (float)g_nScreenHeight ) * (float)( m_ptClient.y + m_ptTempPos.y );
		float fSX = ( 1.0f / (float)g_nScreenWidth ) * (float)m_ptSize.x;
		float fSY = ( 1.0f / (float)g_nScreenHeight ) * (float)m_ptSize.y;

		if( m_spScreen )
		{
			m_spScreen->SetTextures( 0, 0, texLT.x, texLT.y, texRB.x, texRB.y );
			m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
		}
	}

	Render();	//ERROR
}

void cSprite::Render( CsPoint pos, CsPoint size, NiPoint2 texLT, NiPoint2 texRB , float fAlpha )
{
	if( ( m_ptTempPos != pos )||( m_ptSize != size )||( m_vTempTex != texLT ) )
	{
		m_ptTempPos = pos;
		m_vTempTex = texLT;
		m_ptSize = size;

		float fX = ( 1.0f / (float)g_nScreenWidth ) * (float)( m_ptClient.x + m_ptTempPos.x );
		float fY = ( 1.0f / (float)g_nScreenHeight ) * (float)( m_ptClient.y + m_ptTempPos.y );
		float fSX = ( 1.0f / (float)g_nScreenWidth ) * (float)m_ptSize.x;
		float fSY = ( 1.0f / (float)g_nScreenHeight ) * (float)m_ptSize.y;

		if( m_spScreen )
		{
			m_spScreen->SetTextures( 0, 0, texLT.x, texLT.y, texRB.x, texRB.y );
			m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
		}
	}

	Render();
}

void cSprite::RenderLimit( CsPoint pos )
{
	if( m_ptTempPos != pos )
	{
		m_ptTempPos = pos;

		NiPoint2 texLT = NiPoint2::ZERO;
		NiPoint2 texRB = NiPoint2( 1, 1 );

		float fX = ( 1.0f / (float)g_nScreenWidth ) * (float)( m_ptClient.x + m_ptTempPos.x );
		float fY = ( 1.0f / (float)g_nScreenHeight ) * (float)( m_ptClient.y + m_ptTempPos.y );
		float fSX = ( 1.0f / (float)g_nScreenWidth ) * (float)m_ptSize.x;
		float fSY = ( 1.0f / (float)g_nScreenHeight ) * (float)m_ptSize.y;
		NiPoint2 vOrgSize = NiPoint2( fSX, fSY );
		if( m_dwLimitPlag & LIMIT_LEFT )
		{
			float fLimit = m_fLimit[ 0 ];
			if( fX < fLimit )
			{
				texLT.x += ( fLimit - fX ) / vOrgSize.x;
				fSX += fX - fLimit;
				fX = fLimit;
				if( fSX <= 0.0f )
				{
					m_ptTempPos = CsPoint::ZERO;
					return;
				}
			}
		}
		if( m_dwLimitPlag & LIMIT_RIGHT )
		{
			float fLimit = m_fLimit[ 1 ];
			if( (fX + fSX) > fLimit )
			{
				texRB.x += ( fLimit - (fX + fSX) ) / vOrgSize.x;
				fSX -= (fX + fSX) - fLimit;
				if( fSX <= 0.0f )
				{
					m_ptTempPos = CsPoint::ZERO;
					return;
				}
			}
		}

		if( m_spScreen )
		{
			m_spScreen->SetTextures( 0, 0, texLT.x, texLT.y, texRB.x, texRB.y );
			m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
		}
	}

	Render();
}


//=======================================================================================================================
//
//		status
//
//=======================================================================================================================

void cSprite::SetColorA( NiColorA color )
{
	if( m_Color == color )
		return;
	m_Color = color;
	
	if( m_spScreen )
		m_spScreen->SetColors( 0, m_Color );
}

void cSprite::SetColor( NiColor color )
{
	if( m_Color == NiColorA( color.r, color.g, color.b, m_Color.a ) )
		return;
	m_Color = NiColorA( color.r, color.g, color.b, m_Color.a );

	if( m_spScreen )
		m_spScreen->SetColors( 0, m_Color );
}

void cSprite::SetAlpha( float fAlpha )
{
	if( m_Color.a == fAlpha )
		return;
	m_Color.a = fAlpha;

	if( m_spScreen )
		m_spScreen->SetColors( 0, m_Color );
}

void cSprite::SetPos( CsPoint ptPos )
{
	cWindow::SetPos( ptPos );

	_CalRectangle();
}

void cSprite::SetPos_Float( NiPoint2 vPos )
{
	assert_cs( m_pParentWindow == NULL );		// 아직 처리 안되어 있음

	cWindow::SetPos( CsPoint( (int)vPos.x, (int)vPos.y ) );

	float fX = ( 1.0f / (float)g_nScreenWidth ) * vPos.x;
	float fY = ( 1.0f / (float)g_nScreenHeight ) * vPos.y;
	float fSX = ( 1.0f / (float)g_nScreenWidth ) * (float)m_ptSize.x;
	float fSY = ( 1.0f / (float)g_nScreenHeight ) * (float)m_ptSize.y;

	m_ptTempPos = CsPoint::ZERO;

	if( m_spScreen )
		m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
}

void cSprite::SetSize( CsPoint ptSize )
{
	cWindow::SetSize( ptSize );	

	_CalRectangle();
}

CsPoint cSprite::GetPos()
{
	return GetClient();
}

void cSprite::SetPosSize_Float( NiPoint2 vPos, NiPoint2 vSize )
{
	assert_cs( m_pParentWindow == NULL );		// 아직 처리 안되어 있음

	cWindow::SetPos( CsPoint( (int)vPos.x, (int)vPos.y ) );
	cWindow::SetSize( CsPoint( (int)vSize.x, (int)vSize.y ) );

	float fX = ( 1.0f / (float)g_nScreenWidth ) * vPos.x;
	float fY = ( 1.0f / (float)g_nScreenHeight ) * vPos.y;
	float fSX = ( 1.0f / (float)g_nScreenWidth ) * vSize.x;
	float fSY = ( 1.0f / (float)g_nScreenHeight ) * vSize.y;

	m_ptTempPos = CsPoint::ZERO;

	if( m_spScreen )
		m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
}

void cSprite::SetPosSize( CsPoint ptPos, CsPoint ptSize )
{
	cWindow::SetPos( ptPos );
	cWindow::SetSize( ptSize );

	_CalRectangle();
}

void cSprite::ChangeTexture( CsPoint size, NiTexture* pTexture )
{
	cWindow::SetSize( size );
	_CalRectangle();

	RESOURCEMGR_ST.RemoveTexture( m_pTexter );

	m_fInvTexWidth = 1.0f/(float)pTexture->GetWidth();
	m_fInvTexHeight = 1.0f/(float)pTexture->GetHeight();

	if( m_spScreen )
		( (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ) )->SetBaseTexture( pTexture );
}

void cSprite::ChangeTexture( char const* pTextureFileName, bool bUseWorkingFolder )
{
	SAFE_POINTER_RET( pTextureFileName );
	SAFE_POINTER_RET( m_spScreen );

	NiTexturingProperty* pProperty = ( (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ) );
	SAFE_POINTER_RET( pProperty );

	std::string file;
	if( bUseWorkingFolder == true )
		file = m_cWorkingFolder;

	file += pTextureFileName;

	RESOURCEMGR_ST.RemoveTexture( m_pTexter );
	m_pTexter = RESOURCEMGR_ST.GetTexture( file.c_str() );
	NiTexturePtr pTexture = m_pTexter->GetTexture();
	m_fInvTexWidth = 1.0f/(float)pTexture->GetWidth();
	m_fInvTexHeight = 1.0f/(float)pTexture->GetHeight();

	pProperty->SetBaseTexture( pTexture );

	m_spScreen->UpdateProperties();	
}

void cSprite::ChangeTexture( char const* pTextureFileName, CsRect const& rect, bool bUseWorkingFolder )
{
	SAFE_POINTER_RET( pTextureFileName );
	SAFE_POINTER_RET( m_spScreen );

	NiTexturingProperty* pProperty = ( (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ) );
	SAFE_POINTER_RET( pProperty );

	std::string file;
	if( bUseWorkingFolder == true )
		file = m_cWorkingFolder;

	file += pTextureFileName;

	RESOURCEMGR_ST.RemoveTexture( m_pTexter );
	m_pTexter = RESOURCEMGR_ST.GetTexture( file.c_str() );
	NiTexturePtr pTexture = m_pTexter->GetTexture();

	m_fInvTexWidth = 1.0f/(float)pTexture->GetWidth();
	m_fInvTexHeight = 1.0f/(float)pTexture->GetHeight();

	float fUVx = (float)rect.left / (float)pTexture->GetWidth();
	float fUVy = (float)rect.top / (float)pTexture->GetHeight();
	float fUVw = (float)rect.right / (float)pTexture->GetWidth();
	float fUVh = (float)rect.bottom / (float)pTexture->GetHeight();

	m_spScreen->SetTextures( 0, 0, fUVx, fUVy, fUVw, fUVh );

	pProperty->SetBaseTexture( pTexture );

	m_spScreen->UpdateProperties();	

}

void cSprite::_CalRectangle()
{
	float fX = ( 1.0f / (float)g_nScreenWidth ) * (float)m_ptClient.x;
	float fY = ( 1.0f / (float)g_nScreenHeight ) * (float)m_ptClient.y;
	float fSX = ( 1.0f / (float)g_nScreenWidth ) * (float)m_ptSize.x;
	float fSY = ( 1.0f / (float)g_nScreenHeight ) * (float)m_ptSize.y;

	m_ptTempPos = CsPoint::ZERO;

	if( m_spScreen )
		m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
}

//======================================================================================
//
//	ResetDevice
//
//======================================================================================
void cSprite::ResetDevice( CsPoint pos )
{
	cWindow::ResetDevice( pos );
	_ResetDevice();
}

void cSprite::ResetDevice()
{
	cWindow::ResetDevice();
	_ResetDevice();
}

void cSprite::_ResetDevice()
{
	_CalRectangle();
}

//======================================================================================
//
//	트라이앵글
//
//======================================================================================
void cSprite::CreateTriangle( NiColorA color )
{
	m_spScreen = NiNew NiScreenElements( NiNew NiScreenElementsData(false, true, 1, 1, 1, 3, 1, 1, 1) );

	m_spScreen->Insert(3);

	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pAlphaProp->Clone() ) );
	m_spScreen->AttachProperty( NiDynamicCast( NiProperty, m_pVertexColorProp->Clone() ) );
	m_spScreen->UpdateProperties();

	SetColorA( color );
}
#ifdef CROSSWARS_SYSTEM
void cSprite::RenderSquare( CsPoint pos, const NiPoint2& a, const NiPoint2& b, const NiPoint2& c, const NiPoint2& d )
{
	//CHAT_TEXT_DEGUG(L"X: %d, Y: %d",pos.x,pos.y);

	//if( ( m_ptTempPos != pos ) )
	{
		m_ptTempPos = pos;		



		float witdh = 1.0f / (float)g_nScreenWidth;
		float height = 1.0f / (float)g_nScreenHeight;

		NiPoint2 uv;
		NiPoint2 v = a;

		v.x = witdh * (v.x+m_ptTempPos.x);
		v.y = height * (v.y+m_ptTempPos.y);

		

		uv.x = a.x / m_ptSize.x;
		uv.y = a.y / m_ptSize.y;

		if( m_spScreen )
		{
			m_spScreen->SetTexture( 0, 0, 0, uv );
			m_spScreen->SetVertex( 0, 0, v );
		}
		


		v = b;

		v.x = witdh * (v.x+m_ptTempPos.x);
		v.y = height * (v.y+m_ptTempPos.y);

		uv.x = b.x / m_ptSize.x;
		uv.y = b.y / m_ptSize.y;

		if( m_spScreen )
		{
			m_spScreen->SetTexture( 0, 1, 0, uv );
			m_spScreen->SetVertex( 0, 1, v );
		}
		

		v = c;

		v.x = witdh * (v.x+m_ptTempPos.x);
		v.y = height * (v.y+m_ptTempPos.y);

		uv.x = c.x / m_ptSize.x;
		uv.y = c.y / m_ptSize.y;

		if( m_spScreen )
		{
			m_spScreen->SetTexture( 0, 2, 0, uv );
			m_spScreen->SetVertex( 0, 2, v );
		}
		
		
		v = d;

		v.x = witdh * (v.x+m_ptTempPos.x);
		v.y = height * (v.y+m_ptTempPos.y);

		uv.x = d.x / m_ptSize.x;
		uv.y = d.y / m_ptSize.y;

		if( m_spScreen )
		{
			m_spScreen->SetTexture( 0, 3, 0, uv );
			m_spScreen->SetVertex( 0, 3, v );
		}		
	}

	Render();
}
#endif
void cSprite::RenderTriangle( int x, int y, const NiPoint2& a, const NiPoint2& b, const NiPoint2& c )
{
	float witdh = 1.0f / (float)g_nScreenWidth;
	float height = 1.0f / (float)g_nScreenHeight;

	NiPoint2 v = a;

	v.x = witdh * (v.x+x);
	v.y = height * (v.y+y);

	if( m_spScreen )
		m_spScreen->SetVertex( 0, 0, v );

	v = b;

	v.x = witdh * (v.x+x);
	v.y = height * (v.y+y);
	
	if( m_spScreen )
		m_spScreen->SetVertex( 0, 1, v );

	v = c;

	v.x = witdh * (v.x+x);
	v.y = height * (v.y+y);
	
	if( m_spScreen )
		m_spScreen->SetVertex( 0, 2, v );

	Render();
}

float cSprite::GetInvTextWidth() const
{
	return m_fInvTexWidth;
};

float cSprite::GetInvTextHeight() const
{
	return m_fInvTexHeight;
};