
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

cSprite::cSprite()
{
	m_spScreen = NULL;
	m_dwLimitPlag = LIMIT_NONE;
}


void cSprite::Init( cWindow* pParent, CsPoint pos, CsPoint size, char* cImagePath, bool bApplyWindowSize, NiColor color /*NiColor::WHITE*/, bool bUseWorkingFolder /*=true*/ )
{
	assert_cs( cImagePath != NULL );

	if( bUseWorkingFolder == true )
	{
		char filename[MAX_PATH];
		sprintf_s( filename, MAX_PATH, "%s%s", m_cWorkingFolder, cImagePath );
		//assert_csm1( _access_s( filename, 0 ) == 0, "이미지 파일이 존재 하지 않습니다\n%s", filename );
		Init( pParent, pos, size, NiSourceTexture::Create( filename ), bApplyWindowSize, color );
	}
	else
	{
		//assert_csm1( _access_s( cImagePath, 0 ) == 0, "이미지 파일이 존재 하지 않습니다\n%s", cImagePath );
		Init( pParent, pos, size, NiSourceTexture::Create( cImagePath ), bApplyWindowSize, color );
	}
}

void cSprite::Init( cWindow* pParent, CsPoint pos, CsPoint size, NiTexture* pTexture, bool bApplyWindowSize, NiColor color /*NiColor::WHITE*/ )
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

void cSprite::Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize, NiColor color /*=NiColor::WHITE*/ )
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

void cSprite::InitTex2( cWindow* pParent, CsPoint pos, CsPoint size, char* cImagePath, bool bApplyWindowSize )
{
	assert_cs( cImagePath != NULL );

	char filename[MAX_PATH];
	sprintf_s( filename, MAX_PATH, "%s%s", m_cWorkingFolder, cImagePath );
	assert_csm1( _access_s( filename, 0 ) == 0, "이미지 파일이 존재 하지 않습니다\n%s", cImagePath );
	NiSourceTexture* pTexture = NiSourceTexture::Create( filename );	

	cWindow::Init( pParent, pos, size, bApplyWindowSize );
	assert_cs( m_pAlphaProp != NULL );
	assert_cs( pTexture != NULL );
	m_fInvTexWidth = 1.0f/(float)pTexture->GetWidth();
	m_fInvTexHeight = 1.0f/(float)pTexture->GetHeight();

	assert_cs( m_spScreen == NULL );
	m_spScreen = NiNew NiScreenElements( NiNew NiScreenElementsData( false, true, 2 ) );
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

	m_Color = NiColorA::WHITE;
	m_spScreen->SetColors( 0, m_Color );
	m_spScreen->Update( 0.0f, false );	
}

void cSprite::SetRotation( float fAngle )
{
	float witdh = 1.0f / (float)g_nScreenWidth;
	float height = 1.0f / (float)g_nScreenHeight;

	float fX = witdh * (float)GetClient().x;
	float fY = height * (float)GetClient().y;

	NiPoint2 v;

	float fSizeX = 16;
	float fSizeY = 16;

	v.x = -fSizeX;
	v.y = -fSizeY;

	float cos = cosf( fAngle );
	float sin = sinf( fAngle );

	float xx = v.x * cos - v.y * sin;
	float yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	m_spScreen->SetVertex( 0, 0, v );

	v.x = -fSizeX;
	v.y = fSizeY;

	xx = v.x * cos - v.y * sin;
	yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	m_spScreen->SetVertex( 0, 1, v );

	v.x = fSizeX;
	v.y = fSizeY;

	xx = v.x * cos - v.y * sin;
	yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	m_spScreen->SetVertex( 0, 2, v );

	v.x = fSizeX;
	v.y = -fSizeY;

	xx = v.x * cos - v.y * sin;
	yy = v.x * sin + v.y * cos;

	v.x	= xx * witdh + fX;
	v.y	= yy * height + fY;

	m_spScreen->SetVertex( 0, 3, v );
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

		m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
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

		m_spScreen->SetTextures( 0, 0, texLT.x, texLT.y, texRB.x, texRB.y );
		m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
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

		m_spScreen->SetTextures( 0, 0, texLT.x, texLT.y, texRB.x, texRB.y );
		m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
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

		m_spScreen->SetTextures( 0, 0, texLT.x, texLT.y, texRB.x, texRB.y );
		m_spScreen->SetRectangle( 0, fX, fY, fSX, fSY );
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
	m_spScreen->SetColors( 0, m_Color );
}

void cSprite::SetColor( NiColor color )
{
	if( m_Color == NiColorA( color.r, color.g, color.b, m_Color.a ) )
		return;
	m_Color = NiColorA( color.r, color.g, color.b, m_Color.a );
	m_spScreen->SetColors( 0, m_Color );
}

void cSprite::SetAlpha( float fAlpha )
{
	if( m_Color.a == fAlpha )
		return;
	m_Color.a = fAlpha;
	m_spScreen->SetColors( 0, m_Color );
}

void cSprite::SetPos( CsPoint ptPos )
{
	cWindow::SetPos( ptPos );

	_CalRectangle();
}

void cSprite::SetSize( CsPoint ptSize )
{
	cWindow::SetSize( ptSize );	

	_CalRectangle();
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
	
	m_fInvTexWidth = 1.0f/(float)pTexture->GetWidth();
	m_fInvTexHeight = 1.0f/(float)pTexture->GetHeight();

	( (NiTexturingProperty*)m_spScreen->GetProperty( NiTexturingProperty::GetType() ) )->SetBaseTexture( pTexture );
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

void cSprite::RenderTriangle( int x, int y, const NiPoint2& a, const NiPoint2& b, const NiPoint2& c )
{
	float witdh = 1.0f / (float)g_nScreenWidth;
	float height = 1.0f / (float)g_nScreenHeight;

	NiPoint2 v = a;

	v.x = witdh * (v.x+x);
	v.y = height * (v.y+y);

	m_spScreen->SetVertex( 0, 0, v );

	v = b;

	v.x = witdh * (v.x+x);
	v.y = height * (v.y+y);

	m_spScreen->SetVertex( 0, 1, v );

	v = c;

	v.x = witdh * (v.x+x);
	v.y = height * (v.y+y);

	m_spScreen->SetVertex( 0, 2, v );

	Render();
}