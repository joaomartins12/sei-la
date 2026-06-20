
#include "stdafx.h"
#include "Text.h"


void cText::sTEXTINFO::Init( int nFontSize /*=CFont::FS_10*/, NiColor color /*=NiColor::WHITE*/ )
{
	s_pFont = g_pFont;
	s_nFontSize = nFontSize;
	s_Color = color;
	
	s_bOutLine = true;
	s_eBoldLevel = BL_NONE;

	s_eTextAlign = DT_LEFT;
	s_szText[ 0 ] = NULL;
}

int cText::sTEXTINFO::GetBoldSize()
{
	switch( s_eBoldLevel )
	{
	case BL_NONE:	return 0;
	case BL_1:		return 4;
	case BL_2:		return 5;
	case BL_3:		return 6;
	case BL_4:		return 7;
	case BL_5:		return 8;
	}
	assert_csm1( false, L"BoldLevel = %d", s_eBoldLevel );
	return 0;
}

cText::cText()
{
	m_pTexture		=	NULL;
	m_pPixelData	=	NULL;
	m_pFTData		=	NULL;

	m_bUseMark		=	false;
}


//=======================================================================================
//
//		베이스
//
//=======================================================================================
void cText::Delete()
{
	m_Sprite.Delete();

	m_pPixelData	= 0;
	m_pTexture		= 0;
	SAFE_DELETE_ARRAY( m_pFTData );
}

void cText::Init( cWindow* pParent, CsPoint pos, sTEXTINFO* pTextInfo, bool bApplyWindowSize )
{
	assert_cs( _tcslen( pTextInfo->GetText() ) < MAX_PATH );	
	m_TextInfo = *pTextInfo;

	if( m_TextInfo.s_pFont->IsInitialize() == false )
	{
		return;
	}

	// 값 초기화
	m_ptStrSize = CsPoint::ZERO;	

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if( _AllocData() == false )
	{
		m_Sprite.Init( pParent, pos, m_ptStrSize, bApplyWindowSize, m_TextInfo.s_Color );
		return;
	}

	// 텍스쳐 생성
	_CreateTexture();
	m_Sprite.Init( pParent, pos, m_ptStrSize, m_pTexture, bApplyWindowSize, m_TextInfo.s_Color );
}

void cText::Render( CsPoint pos, UINT Align )
{
	if( m_pTexture == NULL )
		return;

	switch(Align)
	{
	case DT_LEFT:													break;
	case DT_CENTER:		pos.x -= (int)( m_ptStrSize.x*0.5f );		break;
	case DT_RIGHT:		pos.x -= m_ptStrSize.x;						break;
	}	
	m_Sprite.Render( pos );
}

void cText::Render( CsPoint pos )
{
	if( m_pTexture == NULL )
		return;

	m_Sprite.Render( pos );
}

void cText::Render()
{
	if( m_pTexture == NULL )
		return;
	
	switch( m_TextInfo.s_eTextAlign )
	{
	case DT_LEFT:		m_Sprite.Render();						break;
	case DT_CENTER:		Render( CsPoint::ZERO, DT_CENTER );		break;
	case DT_RIGHT:		Render( CsPoint::ZERO, DT_RIGHT );		break;
	}
}


//=======================================================================================
//
//		스트링
//
//=======================================================================================

bool cText::GetStringSize( CsPoint& size, int& hBase, TCHAR* szStr )
{	
	int x = 0;
	FT_ULong charcode;
	int nLen = (int)_tcslen( szStr );
	FT_Face face = m_TextInfo.GetFace();
	FT_GlyphSlot slot = face->glyph;

	if( m_bUseMark == false )
	{
		for( int i=0; i<nLen ; ++i )
		{
			if( szStr[ i ] == 0x000d )
				continue;

			charcode = szStr[ i ];
			FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT );
			x += slot->advance.x;
		}
	}
	else
	{
		for( int i=0; i<nLen ; ++i )
		{
			if( szStr[ i ] == 0x000d )
				continue;

			charcode = m_szMark;

			FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT );
			x += slot->advance.x;
		}
	}	

	size.x = ( x >> 6 ) + 2;
	size.y = m_TextInfo.GetHeight();
	hBase = (int)( size.y*0.75f ) << 6;
	
	return ( size.x > 2 );
}

int cText::GetCharWidth( FT_Face face, FT_ULong charcode )
{
	if( charcode == 0x000d )
		return 0;

	FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT );
	return face->glyph->advance.x >> 6;
}

bool cText::SetText( int nStr )
{
	TCHAR szNum[ 16 ];
	_stprintf_s( szNum, 16, L"%d", nStr );
	return SetText( szNum );
}

bool cText::SetText( TCHAR* szStr /*부하가크다*/ )
{
	if( m_TextInfo.s_pFont->IsInitialize() == false )
	{
		return false;
	}

	// 같은 글자라면 패스
	if( _tcscmp( szStr, m_TextInfo.GetText() ) == 0 )
		return false;

	if( szStr[ 0 ] == NULL )
	{
		if( m_pTexture )
		{
			m_pPixelData		= 0;
			m_pTexture			= 0;
			m_ptStrSize			= CsPoint::ZERO;
			m_TextInfo.SetText( L"" );
			return true;
		}
		return false;
	}	

	m_TextInfo.SetText( szStr );
	assert_cs( _tcslen( m_TextInfo.GetText() ) < MAX_PATH );

	// 지우고 새로 생성
	m_pPixelData	= 0;
	m_pTexture		= 0;
	SAFE_DELETE_ARRAY( m_pFTData );

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if( _AllocData() == false )
	{
		return false;
	}
	// 텍스쳐 생성
	_CreateTexture();
	m_Sprite.ChangeTexture( m_ptStrSize, m_pTexture );

	return true;
}

//=======================================================================================
//
//		텍스쳐
//
//=======================================================================================

bool cText::_AllocData()
{
	if( GetStringSize( m_ptStrSize, m_nFTSize_HBase, m_TextInfo.GetText() ) == false )
		return false;
	
	assert_cs( ( m_ptStrSize.x != 0 )&&( m_ptStrSize.y != 0 ) );

	assert_cs( m_pPixelData == NULL );
	m_pPixelData = NiNew NiPixelData( m_ptStrSize.x, m_ptStrSize.y, NiPixelFormat::RGBA32 );

	assert_cs( m_pFTData == NULL );
	int nFTSize = m_ptStrSize.Mul()*4;
	m_pFTData = csnew BYTE[ nFTSize ];
	memset( m_pFTData, 0, sizeof( BYTE )*nFTSize );

	_FTBmpToFTData();

	return true;
}

void cText::_CreateTexture()
{
	if( m_TextInfo.s_bOutLine == true )
	{
		_StringToData_OutLine();
	}
	else
	{
		_StringToData();
	}

	NiTexture::FormatPrefs kPrefs;
	kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
	kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::BINARY;
	m_pTexture = NiSourceTexture::Create( NiDynamicCast( NiPixelData, m_pPixelData->Clone() ), kPrefs );
}

void cText::_FTBmpToFTData()
{
	int nStartSize = 1;
	TCHAR* str = m_TextInfo.GetText();
	int nStrLen = (int)_tcslen( str );
	FT_ULong charcode;
	FT_Face face = m_TextInfo.GetFace();
	FT_GlyphSlot slot = face->glyph;
	FT_Glyph_Metrics *pMetrics;
	FT_Error error;
	int By;

	if( m_bUseMark == false )
	{
		if( m_TextInfo.GetBoldLevel() == sTEXTINFO::BL_NONE )
		{
			for( int i = 0; i<nStrLen; ++i )
			{
				// 엔터
				if( str[ i ] == 0x000d )
					break;

				charcode = FT_Get_Char_Index( face, str[ i ] );
				// 탭키
				if( charcode == 0 )
					continue;
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT );
				assert_cs( error == 0 );
				FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );

				pMetrics = &(slot->metrics);
				By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );
				_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), By>>6, pMetrics->width >> 6, pMetrics->height >> 6 );

				nStartSize += ( pMetrics->horiAdvance >> 6 );		
			}
		}
		else
		{
			int nBoldSize = m_TextInfo.GetBoldSize();
			for( int i = 0; i<nStrLen; ++i )
			{
				// 엔터
				if( str[ i ] == 0x000d )
					break;

				charcode = FT_Get_Char_Index( face, str[ i ] );
				// 탭키
				if( charcode == 0 )
					continue;
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT );
				assert_cs( error == 0 );
				FT_Outline_Embolden( &face->glyph->outline, (face->size->metrics.x_ppem*nBoldSize/100)*64 );
				FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL );

				pMetrics = &(slot->metrics);
				By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );
				_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), By>>6, pMetrics->width >> 6, pMetrics->height >> 6 );

				nStartSize += ( pMetrics->horiAdvance >> 6 );		
			}
		}		
	}
	else
	{
		for( int i = 0; i<nStrLen; ++i )
		{
			if( str[ i ] == 0x000d )
				break;

			error = FT_Load_Char( face, m_szMark, FT_LOAD_RENDER );
			assert_cs( error == 0 );

			pMetrics = &(slot->metrics);
			By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );
			_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), By>>6, pMetrics->width >> 6, pMetrics->height >> 6 );

			nStartSize += ( pMetrics->horiAdvance >> 6 );		
		}
	}	
}

void cText::_ReadFTBmp( FT_Bitmap* bitmap, int x, int y, int sx, int sy )
{
	if( bitmap->buffer == NULL )
		return;

	int i, j, p, q;
	int Width = m_ptStrSize.x;
	int BitWidth = bitmap->width;

	for( i=y, p=0; p<sy; ++i, ++p )
	{
		for( j = x, q = 0 ; q<sx ; ++j, ++q )
		{
			m_pFTData[ i*Width + j ] = bitmap->buffer[ p*BitWidth + q ];
		}
	}
}

void cText::_StringToData_OutLine()
{
	assert_cs( m_pPixelData != NULL );

	DWORD* pOrgData = (DWORD*)m_pPixelData->GetPixels();
	int nIndex, alpha;
	D3DXCOLOR c;
	

	memset( pOrgData, 0, sizeof( DWORD )*m_ptStrSize.Mul() );

	int height = m_ptStrSize.y;
	int width = m_ptStrSize.x;

	for(int y=0; y<height-1; ++y )
	{
		for( int x=0; x<width-1 ; ++x )
		{
			nIndex = y*width + x;
			alpha = m_pFTData[ nIndex ];

			if( alpha > 100 )
			{
				pOrgData[ (y+1)*width + x + 1 ] = 0xff101010;
			}
		}
	}

	for(int y=0; y<height; ++y )
	{
		for( int x=0; x<width ; ++x )
		{
			nIndex = y*width + x;
			alpha = m_pFTData[ nIndex ];

			if( alpha > 70 )
			{
				pOrgData[ nIndex ] = ( (alpha)<<24 )|0x00ffffff;
			}
		}
	}	
}

void cText::_StringToData()
{
	assert_cs( m_pPixelData != NULL );

	DWORD* pData = (DWORD*)m_pPixelData->GetPixels();	

	int height = m_ptStrSize.y;
	int width = m_ptStrSize.x;
	for( int y=0; y<height; ++y )
	{
		for( int x=0; x<width ; ++x )
		{
			*pData = ( (m_pFTData[ y*width + x ])<<24 )|0x00ffffff;
			++pData;
		}
	}
}

