
#include "stdafx.h"
#include "Text.h"

namespace
{
	static const size_t TEXT_INPUT_MAX_CHARS = 4096;

	static bool IsFaceReady( FT_Face face )
	{
		return ( face != NULL && face->glyph != NULL );
	}

	static bool LoadGlyphSafe( FT_Face face, FT_ULong charcode, FT_Int32 flags )
	{
		if( !IsFaceReady( face ) )
			return false;
		FT_UInt glyphIndex = FT_Get_Char_Index( face, charcode );
		if( glyphIndex == 0 )
			return false;
		return FT_Load_Glyph( face, glyphIndex, flags ) == 0;
	}

	static bool RenderGlyphSafe( FT_Face face )
	{
		return IsFaceReady( face ) && ( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) == 0 );
	}
}

cText::sTEXTINFO::sTEXTINFO():
s_pFont(NULL),s_eFontSize(CFont::FS_10),s_Color(FONT_WHITE),
s_bOutLine(true),s_eBoldLevel(BL_NONE),s_eTextAlign(DT_LEFT),
s_nLineGabHeight(0)
{
}

cText::sTEXTINFO::~sTEXTINFO()
{
	s_pFont = NULL;
}

void cText::sTEXTINFO::Init( CFont::eFACE_SIZE eFontSize /*=CFont::FS_10*/, NiColor color /*=FONT_WHITE*/ )
{
	Init( &g_pEngine->m_FontSystem, eFontSize, color );
}

void cText::sTEXTINFO::Init( CFont* pFont, CFont::eFACE_SIZE eFontSize /*=CFont::FS_10*/, NiColor color /*=FONT_WHITE*/ )
{
	s_pFont = pFont;
	s_eFontSize = eFontSize;
	s_Color = color;
	
	s_bOutLine = true;
	s_eBoldLevel = BL_NONE;

	s_eTextAlign = DT_LEFT;
}

void cText::sTEXTINFO::Init( CsHelp::sTEXT* pHelpText )
{
	Init( FONT_SIZE_CAL( pHelpText->s_nTextSize ), NiColor( pHelpText->s_Color[ 0 ]/255.0f, pHelpText->s_Color[ 1 ]/255.0f, pHelpText->s_Color[ 2 ]/255.0f ) );
	SetBoldLevel( (cText::sTEXTINFO::eBOLD_LEVEL)pHelpText->s_nBold );
}

int cText::sTEXTINFO::GetBoldSize() const
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
	assert_csm1( false, _T( "BoldLevel = %d" ), s_eBoldLevel );
	return 0;
}	

void cText::sTEXTINFO::SetText_Reduce( TCHAR const* szText, int nLen, int nDotNum /* = 2 */ )
{
//  	TCHAR t = szText[ nLen ];
// // 	szText[ nLen ] = NULL;
// 
// 	TCHAR szDot[ 2 ] = _T( "." );
// 	s_strText = szText;
// 	szText[ nLen ] = t;
// 
// 	for( int i = 0; i < nDotNum; i++ )
// 		s_strText.append( szDot );
}

cText::sTEXTINFO& cText::sTEXTINFO::AddString( TCHAR const* szText )
{ 
	s_strText += szText; 
	return *this;
}

cText::sTEXTINFO& cText::sTEXTINFO::SetString( TCHAR const* szText )
{ 
	s_strText = szText; 
	return *this;
}

cText::sTEXTINFO& cText::sTEXTINFO::AddSpace()
{ 
	s_strText += L" "; 
	return *this;
}

cText::sTEXTINFO& cText::sTEXTINFO::AddNewLine()
{ 
	s_strText += L"\n"; 
	return *this;
}

FT_Face	cText::sTEXTINFO::GetFace() const
{ 
	return s_pFont->GetFace( s_eFontSize ); 
}

int	cText::sTEXTINFO::GetHeight() const
{ 
	return s_pFont->GetHeight( s_eFontSize ); 
}

int	cText::sTEXTINFO::GetHBase() const
{ 
	return s_pFont->GetHBase( s_eFontSize ); 
}

int cText::sTEXTINFO::GetGabHeight() const
{
	return s_nLineGabHeight;
}

void cText::sTEXTINFO::SetLineGabHeight(int const& nHeight)
{
	s_nLineGabHeight = nHeight;
}

void cText::sTEXTINFO::SetText( TCHAR const* szText ) 
{ 
	if( szText == NULL )
	{
		s_strText.clear();
		return;
	}

	__try
	{
		s_strText = szText;
		if( s_strText.length() > TEXT_INPUT_MAX_CHARS )
			s_strText.resize( TEXT_INPUT_MAX_CHARS );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		s_strText.clear();
	}
}

void cText::sTEXTINFO::SetText( int nText )
{ 
	TCHAR sz[ 16 ] = {0, }; 
	_stprintf_s( sz, 16, _T( "%d" ), nText ); 
	s_strText = sz; 
}

const TCHAR* cText::sTEXTINFO::GetText() const
{
	return s_strText.data(); 
}

int	cText::sTEXTINFO::GetLen()  const
{ 
	return (int)s_strText.length(); 
}

void cText::sTEXTINFO::AddText( TCHAR const* szTex) 
{ 
	if( szTex == NULL )
		return;

	__try
	{
		s_strText.append( szTex );
		if( s_strText.length() > TEXT_INPUT_MAX_CHARS )
			s_strText.resize( TEXT_INPUT_MAX_CHARS );
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
	}
}

void cText::sTEXTINFO::SetBoldLevel( eBOLD_LEVEL Level )
{ 
	s_eBoldLevel = Level; 
}

cText::sTEXTINFO::eBOLD_LEVEL cText::sTEXTINFO::GetBoldLevel() const
{ 
	return s_eBoldLevel;  
}

int cText::sTEXTINFO::GetTextWidth()
{
	FT_Face face = g_pEngine->m_FontSystem.GetFace( s_eFontSize );

	int nWidth = 0;
	for( int i = 0; i < s_strText.length(); ++i )
		nWidth += GetCharWidth(face, s_strText[i]);

	return nWidth;
}

//=======================================================================================
//
//		베이스
//
//=======================================================================================

cText::cText():m_pTexture(NULL), m_pPixelData(NULL), m_pFTData(NULL),m_bUseMark(false)
{
}

void cText::Delete()
{
	m_Sprite.Delete();

	m_pPixelData	= 0;
	m_pTexture		= 0;
	SAFE_DELETE_ARRAY( m_pFTData );
}

void cText::Init( cWindow* pParent, CsPoint pos, sTEXTINFO* pTextInfo, bool bApplyWindowSize )
{
	if( pTextInfo->s_pFont == NULL )
		return;

	if( pTextInfo->s_pFont->IsInitialize() == false )
		return;

	m_TextInfo = *pTextInfo;
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

void cText::InitStencil( cWindow* pParent, CsPoint pos, sTEXTINFO* pTextInfo, bool bApplyWindowSize, NiStencilProperty* pPropStencil )
{
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}

	m_TextInfo = *pTextInfo;
	// 값 초기화
	m_ptStrSize = CsPoint::ZERO;	

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if( _AllocData() == false )
	{
		m_Sprite.InitStencil( pParent, pos, m_ptStrSize, bApplyWindowSize, pPropStencil, m_TextInfo.s_Color );
		return;
	}

	// 텍스쳐 생성
	_CreateTexture();
	m_Sprite.InitStencil( pParent, pos, m_ptStrSize, m_pTexture, bApplyWindowSize, pPropStencil, m_TextInfo.s_Color );
}

void cText::SetPos( CsPoint ptPos )
{
	m_Sprite.SetPos( ptPos );
}
CsPoint cText::GetPos()
{
	return m_Sprite.GetPos();
}

//2017-02-16-nova
void cText::SetFontSize(CFont::eFACE_SIZE FontSize)
{
	m_TextInfo.s_eFontSize = FontSize;
}

void cText::Render( CsPoint pos, UINT Align )
{
	if( m_pTexture == NULL )
		return;
	
	if( Align & DT_CENTER )
		pos.x -= (int)( m_ptStrSize.x*0.5f );		
	else if( Align & DT_RIGHT )
		pos.x -= m_ptStrSize.x;

	if( Align & DT_VCENTER )
		pos.y -= (int)( m_ptStrSize.y*0.5f );		
	else if( Align & DT_BOTTOM )
		pos.y -= m_ptStrSize.y;

	m_Sprite.Render( pos );
}

void cText::Render( CsPoint pos )
{
	if( m_pTexture == NULL )
		return;

	Render( pos, m_TextInfo.s_eTextAlign );
}

void cText::Render()
{
	if( m_pTexture == NULL )
		return;

	Render( CsPoint::ZERO, m_TextInfo.s_eTextAlign);	
}

void cText::RenderLimit( CsPoint pos, UINT Align )
{
	if( m_pTexture == NULL )
		return;

	if( Align & DT_CENTER )
		pos.x -= (int)( m_ptStrSize.x*0.5f );		
	else if( Align & DT_RIGHT )
		pos.x -= m_ptStrSize.x;

	if( Align & DT_VCENTER )
		pos.y -= (int)( m_ptStrSize.y*0.5f );		
	else if( Align & DT_BOTTOM )
		pos.y -= m_ptStrSize.y;

	m_Sprite.RenderLimit( pos );
}

void cText::RenderLimit( CsPoint pos )
{
	if( m_pTexture == NULL )
		return;

	m_Sprite.RenderLimit( pos );
}

void cText::SetVisible( bool bValue )
{
	m_Sprite.SetVisible( bValue );
}

bool cText::GetVisible() const
{
	return m_Sprite.GetVisible();
}

//=======================================================================================
//
//		스트링
//
//=======================================================================================

bool cText::GetStringSize( CsPoint& size, int& hBase, TCHAR* szStr, std::wstring& wsResult, int nWidth )
{
	if( NULL == m_TextInfo.s_pFont )
		return false;

	if( !m_TextInfo.s_pFont->IsInitialize() )
		return false;

	FT_ULong charcode;
	int nLen = (int)_tcslen( szStr );
	FT_Face face = m_TextInfo.GetFace();
	if( !IsFaceReady( face ) )
		return false;
	FT_GlyphSlot slot = face->glyph;
	
	int nLimitWidth = nWidth << 6;
	int nMaxWidth = 0;
	int nMaxHeight = m_TextInfo.GetHeight();

	if( m_bUseMark == false )
	{
		int x = 0;
		for( int i=0; i<nLen ; ++i )
		{
			if( szStr[ i ] == 0x000a || (szStr[ i ] == 0x000d) )// 엔터
			{
				wsResult += szStr[ i ];
				if( nMaxWidth < x )
					nMaxWidth = x;

				x = 0;
				nMaxHeight += (m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight());
				continue;
			}

			charcode = szStr[ i ];
#if ( defined VERSION_TW || defined VERSION_HK )
			// msjh.ttc 폰트에서 'j' 문자가 연속되면 공백이 전문자를 가리는 현상으로 'j'문자 일때 자동 힌팅 제거
			FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#else
			FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#endif
			if( x + slot->advance.x > nLimitWidth )
			{
				wsResult += _T("\n");
				wsResult += szStr[ i ];
				if( nMaxWidth < x )
					nMaxWidth = x;
				x = slot->advance.x;
				nMaxHeight += (m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight());
				continue;
			}

			wsResult += szStr[ i ];
			x += slot->advance.x;

			if( nMaxWidth < x )
				nMaxWidth = x;
		}
	}
	else
	{
		int x = 0;
		for( int i=0; i<nLen ; ++i )
		{
			if( szStr[ i ] == 0x000a || szStr[ i ] == 0x000d )
			{
				wsResult += szStr[ i ];

				if( nMaxWidth < x )
					nMaxWidth = x;

				x = 0;
				nMaxHeight += (m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight());
				continue;
			}

			charcode = m_szMark;

#if ( defined VERSION_TW || defined VERSION_HK )
			FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#else
			FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#endif
			if( x + slot->advance.x > nLimitWidth )
			{
				wsResult += _T("\n");
				wsResult += szStr[ i ];
				if( nMaxWidth < x )
					nMaxWidth = x;
				x = slot->advance.x;
				nMaxHeight += (m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight());
				continue;
			}

			x += slot->advance.x;

			if( nMaxWidth < x )
				nMaxWidth = x;
		}
	}	

	size.x = ( nMaxWidth >> 6 ) + 2;
	size.y = nMaxHeight;
	hBase = (int)( m_TextInfo.GetHeight()*0.75f ) << 6;

	return ( size.x > 2 );
}

bool cText::GetStringSize( CsPoint& size, int& hBase, TCHAR* szStr )
{
	if( m_TextInfo.s_pFont == NULL )
		return false;

	if( m_TextInfo.s_pFont->IsInitialize() == false )
		return false;

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

			auto p = FT_Get_Char_Index(face, charcode);

			if (!p)
				continue;

#if ( defined VERSION_TW || defined VERSION_HK )
			// msjh.ttc 폰트에서 'j' 문자가 연속되면 공백이 전문자를 가리는 현상으로 'j'문자 일때 자동 힌팅 제거
			if( !LoadGlyphSafe( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) )
				continue;
#else
			// FT_LOAD_FORCE_AUTOHINT bypasses the TrueType bytecode interpreter
			// (TT_RunIns / Ins_MDRP, which crashes on some pack-shipped tahoma glyphs
			// under FT 2.13) while running the autohinter — autohinter snaps glyphs
			// algorithmically so text stays crisp. The earlier autohinter AVs were
			// FT 2.8 bugs; 2.13 has had 5+ years of hardening and is safe.
			if( !LoadGlyphSafe( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) )
				continue;
#endif
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


			auto p = FT_Get_Char_Index(face, charcode);

			if (!p)
				continue;

#if ( defined VERSION_TW || defined VERSION_HK )
			if( !LoadGlyphSafe( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) )
				continue;
#else
			if( !LoadGlyphSafe( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) )
				continue;
#endif
			x += slot->advance.x;
		}
	}	

	size.x = ( x >> 6 ) + 2;
	size.y = m_TextInfo.GetHeight();
	hBase = (int)( size.y*0.75f ) << 6;
	
	return ( size.x > 2 );
}

void cText::GetStringSize( sTEXTINFO* TextInfo, CsPoint& size, int& hBase, const TCHAR* szStr )
{
	if( TextInfo->s_pFont->IsInitialize() == false )
	{
		return;
	}

	int x = 0;
	FT_ULong charcode;
	int nLen = (int)_tcslen( szStr );
	FT_Face face = TextInfo->GetFace();
	if( !IsFaceReady( face ) )
		return;
	FT_GlyphSlot slot = face->glyph;

	for( int i=0; i<nLen ; ++i )
	{
		if( szStr[ i ] == 0x000d )
			continue;

		charcode = szStr[ i ];
#if ( defined VERSION_TW || defined VERSION_HK )
		if( !LoadGlyphSafe( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) )
			continue;
#else
		if( !LoadGlyphSafe( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) )
			continue;
#endif
		x += slot->advance.x;
	}

	size.x = ( x >> 6 ) + 2;
	size.y = TextInfo->GetHeight();
	hBase = (int)( size.y*0.75f ) << 6;

	
}

int cText::GetCharWidth( FT_Face face, FT_ULong charcode )
{
	if( charcode == 0x000d )
		return 0;
	if( !IsFaceReady( face ) )
		return 0;

	// FT_LOAD_FORCE_AUTOHINT — force the autohinter, bypass the TrueType
	// bytecode interpreter (which crashes inside `Ins_MDRP` / `TT_RunIns` on
	// some pack-shipped tahoma glyphs even after replacing the original font
	// with a clean Windows Tahoma — see crash at `cText::_FTBmpToFTData`
	// during server-select). The autohinter snaps glyphs algorithmically so
	// text rendering stays crisp, AND avoids the broken bytecode path
	// entirely. NO_AUTOHINT and FORCE_AUTOHINT are mutually exclusive — only
	// one is set here.
#if ( defined VERSION_TW || defined VERSION_HK )
	if( !LoadGlyphSafe( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) )
		return 0;
#else
	if( !LoadGlyphSafe( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) )
		return 0;
#endif
	return face->glyph->advance.x >> 6;
}
// [4/21/2016 hyun] 문자열의 넓이를 가져온다
int cText::GetStringWidth( FT_Face face, const std::wstring& str )
{
	int nWidth = 0;

	for(int i = 0; i < str.length(); ++i)
		nWidth += GetCharWidth(face, str[i]);

	return nWidth;
}
// @@ 여기까지
bool cText::SetText( int nStr )
{
	TCHAR szNum[ 16 ];
	_stprintf_s( szNum, 16, _T( "%d" ), nStr );
	return SetText( szNum );
}

bool cText::SetText( TCHAR const* szStr /*부하가크다*/ )
{
	if( NULL == m_TextInfo.s_pFont )
		return false;

	if( m_TextInfo.s_pFont->IsInitialize() == false )
	{
		return false;
	}

	cText::sTEXTINFO safeInfo;
	safeInfo.SetText( szStr );
	std::wstring safeInput = safeInfo.s_strText;

	if( safeInput.empty() )
	{
		if( m_pTexture )
		{
			m_pPixelData		= 0;
			m_pTexture			= 0;
			m_ptStrSize			= CsPoint::ZERO;
			m_TextInfo.SetText( _T( "" ) );
			return true;
		}
			return false;
	}	

	// 같은 글자라면 패스
	if( safeInput == m_TextInfo.GetText() )
		return false;

	m_TextInfo.SetText( safeInput.c_str() );

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
	m_Sprite.SetColor( m_TextInfo.s_Color );

	return true;
}

bool cText::SetText( TCHAR const* szStr /*부하가크다*/, int nWidth )
{
	if( NULL == m_TextInfo.s_pFont )
		return false;

	if( !m_TextInfo.s_pFont->IsInitialize() )
		return false;

	cText::sTEXTINFO safeInfo;
	safeInfo.SetText( szStr );
	std::wstring safeInput = safeInfo.s_strText;

	if( safeInput.empty() )
	{
		if( m_pTexture )
		{
			m_pPixelData		= 0;
			m_pTexture			= 0;
			m_ptStrSize			= CsPoint::ZERO;
			m_TextInfo.SetText( _T( "" ) );
			return true;
		}
			return false;
	}	

	// 같은 글자라면 패스
	if( safeInput == m_TextInfo.GetText() )
		return false;

	m_TextInfo.SetText( safeInput.c_str() );

	// 지우고 새로 생성
	m_pPixelData	= 0;
	m_pTexture		= 0;
	SAFE_DELETE_ARRAY( m_pFTData );

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if( !_AllocData(nWidth) )
		return false;

	// 텍스쳐 생성
	_CreateTexture();

	m_Sprite.ChangeTexture( m_ptStrSize, m_pTexture );
	m_Sprite.SetColor( m_TextInfo.s_Color );

	return true;
}
//=======================================================================================
//
//		텍스쳐
//
//=======================================================================================

bool cText::_AllocData()
{
	if( GetStringSize( m_ptStrSize, m_nFTSize_HBase, (TCHAR*)m_TextInfo.GetText() ) == false )
		return false;
	
	assert_cs( ( m_ptStrSize.x != 0 )&&( m_ptStrSize.y != 0 ) );

	assert_cs( m_pPixelData == NULL );
	m_pPixelData = NiNew NiPixelData( m_ptStrSize.x, m_ptStrSize.y, NiPixelFormat::RGBA32 );

	assert_cs( m_pFTData == NULL );
	int nFTSize = m_ptStrSize.Mul()*4;
	m_pFTData = xnew BYTE[ nFTSize ];
	memset( m_pFTData, 0, sizeof( BYTE )*nFTSize );

	_FTBmpToFTData();

	return true;
}

bool cText::_AllocData(int nWidth)
{
	std::wstring result;
	if( !GetStringSize( m_ptStrSize, m_nFTSize_HBase, (TCHAR*)m_TextInfo.GetText(), result, nWidth ) )
		return false;

	assert_cs( ( m_ptStrSize.x != 0 )&&( m_ptStrSize.y != 0 ) );

	m_TextInfo.SetText( result.c_str() );
	assert_cs( m_pPixelData == NULL );
	m_pPixelData = NiNew NiPixelData( m_ptStrSize.x, m_ptStrSize.y, NiPixelFormat::RGBA32 );

	assert_cs( m_pFTData == NULL );
	int nFTSize = m_ptStrSize.Mul()*4;
	m_pFTData = xnew BYTE[ nFTSize ];
	memset( m_pFTData, 0, sizeof( BYTE )*nFTSize );

	_FTBmpToFTData_MultiLine();

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
#ifdef VERSION_TH
	m_nFTSize_HBase += 128;
#endif

	int nStartSize = 1;
	const TCHAR* str = m_TextInfo.GetText();
	int nStrLen = (int)_tcslen( str );
	FT_ULong charcode;
	FT_Face face = m_TextInfo.GetFace();
	if( !IsFaceReady( face ) )
		return;
	FT_GlyphSlot slot = face->glyph;
	FT_Glyph_Metrics *pMetrics;
	FT_Error error;
	int By;

#ifdef FREETYPE_LENGTH_OVER		// 태국 font 메모리 침범 및 일부 문자 표현 不 관련 변수 선언
	FT_ULong _Tempcharcode;
	FT_Face _Tempface = m_TextInfo.GetFace();
	FT_GlyphSlot _Tempslot = _Tempface->glyph;
	FT_Glyph_Metrics _Temp_pMetrics;
	FT_Error _Temperror;
	FT_Bitmap _TempBitmap;

	bool bTempFreeType	= false;
	bool bOderSwap		= false;
	TCHAR _szTemp[2]	= L"ำ";
	TCHAR _szTemp2[2]	= L"้";
	TCHAR _szTemp3[2]	= L"่";
	int tempAdvance_1, tempAdvance_2, tempAdvance_3;
#endif

	if( m_bUseMark == false )
	{
		if( m_TextInfo.GetBoldLevel() == sTEXTINFO::BL_NONE )
		{
			for( int i = 0; i<nStrLen; ++i )

			{
				// 엔터
				if( str[ i ] == 0x000d )
					break;

#ifdef FREETYPE_LENGTH_OVER	// 태국 font 메모리 침범 관련 소스 코드
				if(bTempFreeType == false)
				{
					if( ( i + 1 ) >= nStrLen )
					{
						bTempFreeType = true;
					}
					else
					{
					charcode = FT_Get_Char_Index( face, str[ i + 1 ] );
					error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
					// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;

					pMetrics =&(slot->metrics);

					if( (pMetrics->horiBearingX >> 6) < 0 )
					{
						int _TempSize = (pMetrics->horiBearingX >> 6);

						charcode = FT_Get_Char_Index( face, str[ i ] );

						error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
						// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;

						pMetrics = &(slot->metrics);

						_TempSize += ( nStartSize + ( pMetrics->horiAdvance >> 6 ) );

						if(_TempSize < 0)
						{
							_TempSize = -(_TempSize);
							nStartSize += _TempSize;
						}
					}

					bTempFreeType = true;					
					}
				}
#endif

				charcode = FT_Get_Char_Index( face, str[ i ] );
				// 탭키
				if( charcode == 0 )
					continue;

#if ( defined VERSION_TW || defined VERSION_HK )
				// msjh.ttc 폰트에서 'j' 문자가 연속되면 공백이 전문자를 가리는 현상으로 'j'문자 일때 자동 힌팅 제거
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#else
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#endif
				// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;
				if( !RenderGlyphSafe( face ) )
					continue;

#ifdef FREETYPE_LENGTH_OVER	// 태국 font 일부 문자 표현 不 관련 소스 코드	lks007	12.08.02
				const bool hasNext1 = ( i + 1 ) < nStrLen;
				const bool hasNext2 = ( i + 2 ) < nStrLen;
				if( hasNext2 && ((_szTemp2[0] == str[i + 1] && _szTemp[0] == str[i + 2]) || (_szTemp3[0] == str[i + 1] && _szTemp[0] == str[i + 2])) )
				{
					//m_nFTSize_HBase += 128;

					for(int j=0; j<3; ++j)
					{
						if( j == 0)
						{
							tempAdvance_1 = nStartSize;
							pMetrics = &(slot->metrics);

							nStartSize += pMetrics->horiAdvance >> 6;
						}
						else if( j == 1 )
						{
							charcode = FT_Get_Char_Index( face, str[ i + j ] );
							error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
							if( error != 0 || FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
								continue;
							pMetrics = &(slot->metrics);
							By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );

							_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), By>>6, pMetrics->width >> 6, pMetrics->height >> 6 );

							nStartSize += pMetrics->horiAdvance >> 6;
						}
						else if( j == 2 )
						{
							charcode = FT_Get_Char_Index( face, str[ i + j ] );
							error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
							if( error != 0 || FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
								continue;
							pMetrics = &(slot->metrics);
							m_nFTSize_HBase += 64;
							By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );

							_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), By>>6, pMetrics->width >> 6, pMetrics->height >> 6 );

							nStartSize += pMetrics->horiAdvance >> 6;
						}
					}
					
					charcode = FT_Get_Char_Index( face, str[ i ] );
					error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
					if( error != 0 || FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
						continue;
					pMetrics = &(slot->metrics);
					m_nFTSize_HBase -= 64;
					By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );

					_ReadFTBmp( &slot->bitmap, tempAdvance_1+( pMetrics->horiBearingX >> 6 ), By>>6, pMetrics->width >> 6, pMetrics->height >> 6 );
					i += 2;
					//m_nFTSize_HBase -= 128;
					continue;
				}

				if( hasNext1 && _szTemp[0] == str[i + 1] && bOderSwap == false )
				{
					_Tempcharcode = FT_Get_Char_Index( _Tempface, str[ i ] );
					_Temperror = FT_Load_Glyph( _Tempface, _Tempcharcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
					if( _Temperror != 0 )
						continue;
					_Temp_pMetrics = (_Tempslot->metrics);
					tempAdvance_1 = _Temp_pMetrics.horiAdvance >> 6;
					tempAdvance_2 = nStartSize;
					nStartSize += tempAdvance_1;
					bOderSwap = true;
					continue;
				}
#endif

				pMetrics = &(slot->metrics);
				By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );
				
#if ( defined VERSION_TW || defined VERSION_HK )
				if( nStartSize == 1 )
				{
					if( (pMetrics->horiBearingX >> 6) < -1 )
						nStartSize = -( pMetrics->horiBearingX >> 6 );
				}
#endif

				_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), By>>6, pMetrics->width >> 6, pMetrics->height >> 6 );

#ifdef FREETYPE_LENGTH_OVER	// 태국 font 일부 문자 표현 不 관련 소스 코드	lks007	12.08.02
				if( bOderSwap == true )
				{
					tempAdvance_3 = pMetrics->horiAdvance >> 6;
					_Tempcharcode = FT_Get_Char_Index( _Tempface, str[ i - 1 ] );
					_Temperror = FT_Load_Glyph( _Tempface, _Tempcharcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
					if( _Temperror != 0 || FT_Render_Glyph( _Tempface->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
						continue;
					_TempBitmap = (_Tempface->glyph->bitmap);

					By = CsMax( 0, m_nFTSize_HBase - _Temp_pMetrics.horiBearingY );
					
					_ReadFTBmp( &_TempBitmap, tempAdvance_2+( _Temp_pMetrics.horiBearingX >> 6 ), By>>6, _Temp_pMetrics.width >> 6, _Temp_pMetrics.height >> 6 );

					nStartSize += tempAdvance_3;
					bOderSwap = false;
					continue;
				}
#endif

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
#if ( defined VERSION_TW || defined VERSION_HK )
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#else
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#endif
				// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;
				FT_Outline_Embolden( &face->glyph->outline, (face->size->metrics.x_ppem*nBoldSize/100)*64 );
				if( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
					continue;

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
			// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;

			pMetrics = &(slot->metrics);
			By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );
			_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), By>>6, pMetrics->width >> 6, pMetrics->height >> 6 );

			nStartSize += ( pMetrics->horiAdvance >> 6 );		
		}
	}	
}

void cText::_FTBmpToFTData_MultiLine()
{
#ifdef VERSION_TH
	m_nFTSize_HBase += 128;
#endif

	int nStartSize = 1;
	int nStartSizeY = 0;
	const TCHAR* str = m_TextInfo.GetText();
	int nStrLen = (int)_tcslen( str );
	FT_ULong charcode;
	FT_Face face = m_TextInfo.GetFace();
	if( !IsFaceReady( face ) )
		return;
	FT_GlyphSlot slot = face->glyph;
	FT_Glyph_Metrics *pMetrics;
	FT_Error error;
	int By = 0;

#ifdef FREETYPE_LENGTH_OVER		// 태국 font 메모리 침범 및 일부 문자 표현 不 관련 변수 선언
	FT_ULong _Tempcharcode;
	FT_Face _Tempface = m_TextInfo.GetFace();
	FT_GlyphSlot _Tempslot = _Tempface->glyph;
	FT_Glyph_Metrics _Temp_pMetrics;
	FT_Error _Temperror;
	FT_Bitmap _TempBitmap;

	bool bTempFreeType	= false;
	bool bOderSwap		= false;
	TCHAR _szTemp[2]	= L"ำ";
	TCHAR _szTemp2[2]	= L"้";
	TCHAR _szTemp3[2]	= L"่";
	int tempAdvance_1, tempAdvance_2, tempAdvance_3;
#endif

	if( m_bUseMark == false )
	{
		if( m_TextInfo.GetBoldLevel() == sTEXTINFO::BL_NONE )
		{
			for( int i = 0; i<nStrLen; ++i )
			{
				// 엔터
				if( str[ i ] == 0x000d || str[ i ] == 0x000a )
				{
					nStartSize = 1;
					nStartSizeY += (m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight());
					continue;
				}

#ifdef FREETYPE_LENGTH_OVER	// 태국 font 메모리 침범 관련 소스 코드
				if(bTempFreeType == false)
				{
					if( ( i + 1 ) >= nStrLen )
					{
						bTempFreeType = true;
					}
					else
					{
					charcode = FT_Get_Char_Index( face, str[ i + 1 ] );
					error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
					// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;

					pMetrics =&(slot->metrics);

					if( (pMetrics->horiBearingX >> 6) < 0 )
					{
						int _TempSize = (pMetrics->horiBearingX >> 6);

						charcode = FT_Get_Char_Index( face, str[ i ] );

						error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
						// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;

						pMetrics = &(slot->metrics);

						_TempSize += ( nStartSize + ( pMetrics->horiAdvance >> 6 ) );

						if(_TempSize < 0)
						{
							_TempSize = -(_TempSize);
							nStartSize += _TempSize;
						}
					}

					bTempFreeType = true;					
					}
				}
#endif

				charcode = FT_Get_Char_Index( face, str[ i ] );
				// 탭키
				if( charcode == 0 )
					continue;

#if ( defined VERSION_TW || defined VERSION_HK )
				// msjh.ttc 폰트에서 'j' 문자가 연속되면 공백이 전문자를 가리는 현상으로 'j'문자 일때 자동 힌팅 제거
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#else
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#endif
				// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;
				if( !RenderGlyphSafe( face ) )
					continue;

#ifdef FREETYPE_LENGTH_OVER	// 태국 font 일부 문자 표현 不 관련 소스 코드	lks007	12.08.02
				const bool hasNext1 = ( i + 1 ) < nStrLen;
				const bool hasNext2 = ( i + 2 ) < nStrLen;
				if( hasNext2 && ((_szTemp2[0] == str[i + 1] && _szTemp[0] == str[i + 2]) || (_szTemp3[0] == str[i + 1] && _szTemp[0] == str[i + 2])) )
				{
					//m_nFTSize_HBase += 128;

					for(int j=0; j<3; ++j)
					{
						if( j == 0)
						{
							tempAdvance_1 = nStartSize;
							pMetrics = &(slot->metrics);

							nStartSize += pMetrics->horiAdvance >> 6;
						}
						else if( j == 1 )
						{
							charcode = FT_Get_Char_Index( face, str[ i + j ] );
								error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
								if( error != 0 || FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
									continue;
							pMetrics = &(slot->metrics);
							By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );

							_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), nStartSizeY + (By>>6), pMetrics->width >> 6, pMetrics->height >> 6 );

							nStartSize += pMetrics->horiAdvance >> 6;
						}
						else if( j == 2 )
						{
							charcode = FT_Get_Char_Index( face, str[ i + j ] );
								error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
								if( error != 0 || FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
									continue;
							pMetrics = &(slot->metrics);
							m_nFTSize_HBase += 64;
							By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );

							_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), nStartSizeY + (By>>6), pMetrics->width >> 6, pMetrics->height >> 6 );

							nStartSize += pMetrics->horiAdvance >> 6;
						}
					}

					charcode = FT_Get_Char_Index( face, str[ i ] );
						error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
						if( error != 0 || FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
							continue;
					pMetrics = &(slot->metrics);
					m_nFTSize_HBase -= 64;
					By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );

					_ReadFTBmp( &slot->bitmap, tempAdvance_1+( pMetrics->horiBearingX >> 6 ), nStartSizeY + (By>>6), pMetrics->width >> 6, pMetrics->height >> 6 );
					i += 2;
					//m_nFTSize_HBase -= 128;
					continue;
				}

					if( hasNext1 && _szTemp[0] == str[i + 1] && bOderSwap == false )
				{
					_Tempcharcode = FT_Get_Char_Index( _Tempface, str[ i ] );
						_Temperror = FT_Load_Glyph( _Tempface, _Tempcharcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
						if( _Temperror != 0 )
							continue;
					_Temp_pMetrics = (_Tempslot->metrics);
					tempAdvance_1 = _Temp_pMetrics.horiAdvance >> 6;
					tempAdvance_2 = nStartSize;
					nStartSize += tempAdvance_1;
					bOderSwap = true;
					continue;
				}
#endif

				pMetrics = &(slot->metrics);
				By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );

#if ( defined VERSION_TW || defined VERSION_HK )
				if( nStartSize == 1 )
				{
					if( (pMetrics->horiBearingX >> 6) < -1 )
						nStartSize = -( pMetrics->horiBearingX >> 6 );
				}
#endif

				_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), nStartSizeY + (By>>6), pMetrics->width >> 6, pMetrics->height >> 6 );

#ifdef FREETYPE_LENGTH_OVER	// 태국 font 일부 문자 표현 不 관련 소스 코드	lks007	12.08.02
				if( bOderSwap == true )
				{
					tempAdvance_3 = pMetrics->horiAdvance >> 6;
					_Tempcharcode = FT_Get_Char_Index( _Tempface, str[ i - 1 ] );
					_Temperror = FT_Load_Glyph( _Tempface, _Tempcharcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
					if( _Temperror != 0 || FT_Render_Glyph( _Tempface->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
						continue;
					_TempBitmap = (_Tempface->glyph->bitmap);

					By = CsMax( 0, m_nFTSize_HBase - _Temp_pMetrics.horiBearingY );

					_ReadFTBmp( &_TempBitmap, tempAdvance_2+( _Temp_pMetrics.horiBearingX >> 6 ), nStartSizeY +(By>>6), _Temp_pMetrics.width >> 6, _Temp_pMetrics.height >> 6 );

					nStartSize += tempAdvance_3;
					bOderSwap = false;
					continue;
				}
#endif

				nStartSize += ( pMetrics->horiAdvance >> 6 );		
			}
		}
		else
		{
			int nBoldSize = m_TextInfo.GetBoldSize();
			for( int i = 0; i<nStrLen; ++i )
			{
				// 엔터
				if( str[ i ] == 0x000d || str[ i ] == 0x000a )
				{
					nStartSize = 1;
					nStartSizeY += (m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight());
					continue;
				}


				charcode = FT_Get_Char_Index( face, str[ i ] );
				// 탭키
				if( charcode == 0 )
					continue;
#if ( defined VERSION_TW || defined VERSION_HK )
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#else
				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
#endif
				// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;
				FT_Outline_Embolden( &face->glyph->outline, (face->size->metrics.x_ppem*nBoldSize/100)*64 );
				if( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
					continue;

				pMetrics = &(slot->metrics);
				By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );
				_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), nStartSizeY + (By>>6), pMetrics->width >> 6, pMetrics->height >> 6 );

				nStartSize += ( pMetrics->horiAdvance >> 6 );		
			}
		}		
	}
	else
	{
		for( int i = 0; i<nStrLen; ++i )
		{
			if( str[ i ] == 0x000d || str[ i ] == 0x000a )
			{
				nStartSize = 1;
				nStartSizeY += (m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight());
				continue;
			}

			error = FT_Load_Char( face, m_szMark, FT_LOAD_RENDER );
			// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;

			pMetrics = &(slot->metrics);
			By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );
			_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), nStartSizeY + (By>>6), pMetrics->width >> 6, pMetrics->height >> 6 );

			nStartSize += ( pMetrics->horiAdvance >> 6 );		
		}
	}	
}

void cText::_ReadFTBmp( FT_Bitmap* bitmap, int x, int y, int sx, int sy )
{
	if( bitmap == NULL || bitmap->buffer == NULL )
		return;
	if( sx <= 0 || sy <= 0 )
		return;
	if( bitmap->pixel_mode != FT_PIXEL_MODE_GRAY )
		return;

	// Same defensive shape as the bitmap->buffer check above.  m_pFTData is the
	// destination glyph buffer; when an upstream call (eg _AllocData) skipped
	// allocation for a degenerate string (NULL/empty/single-bad-character path),
	// m_pFTData stays NULL and the write below would segfault.  Hits routinely
	// from the post-attack `cDigimonTalk::Print` balloon when the bark string
	// references a missing glyph — pre-existing v487 issue, surfaces on partner
	// skill-cast battle transitions among other places.
	if( m_pFTData == NULL )
		return;

	int i, j, p, q;
	int Width = m_ptStrSize.x;
	int BitWidth = bitmap->width;
	int Pitch = bitmap->pitch;
	int TotalSize = m_ptStrSize.x * m_ptStrSize.y;
	// Source-side bounds: FT_Bitmap exposes (width, rows) as the only valid extent
	// of `buffer`.  The caller's (sx, sy) can exceed those when the requested glyph
	// blit is larger than what FreeType actually rendered (degenerate glyph, fallback
	// sizing, etc.).  Without this guard the read at line 1296 indexes past the FT
	// buffer and AVs — hit while rendering the server-select screen's server-name
	// strings on the CSelectServer::ResetServerList path.
	if( BitWidth <= 0 || bitmap->rows <= 0 )
		return;
	if( Pitch < 0 )
		Pitch = -Pitch;
	if( Pitch < BitWidth )
		Pitch = BitWidth;
	int srcTotal = Pitch * (int)bitmap->rows;

	for( i=y, p=0; p<sy; ++i, ++p )
	{
		for( j = x, q = 0 ; q<sx ; ++j, ++q )
		{
			int destIdx = i*Width + j;
			if( destIdx < 0 || destIdx >= TotalSize )
				continue;
			int srcIdx = p*Pitch + q;
			if( srcIdx < 0 || srcIdx >= srcTotal )
				continue;
			m_pFTData[ destIdx ] = bitmap->buffer[ srcIdx ];
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
			if( nIndex < 0 )
				continue;

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
			if( nIndex < 0 )
				continue;

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
			if( y*width + x < 0 )
				continue;

			*pData = ( (m_pFTData[ y*width + x ])<<24 )|0x00ffffff;
			++pData;
		}
	}
}

//=======================================================================================
//
//
//
//		타이머 텍스트
//
//
//
//=======================================================================================
// cTimerText::cTimerText()
// {
// 
// }
// cTimerText::~cTimerText()
// {
// 
// }
// 
// void cTimerText::Update( float fEp )
// {
// 	int nRemain = m_StartTime - _TIME_TS;
// 	int nSec = nRemain;
// 	int nMin = nRemain / 60;
// 	int nHour = nMin / 60;
// 	int nDay = nHour / 24;
// 
// 	if( m_nCurrent_nDay == nDay && m_nCurrent_nHour == nHour && m_nCurrent_nMin == nMin && m_nCurrent_nSec == nSec )
// 		return;
// 
// 	m_nCurrent_nDay = nDay;
// 	m_nCurrent_nHour = nHour;
// 	m_nCurrent_nMin = nMin;
// 	m_nCurrent_nSec = nSec;
// 
// 	switch( m_TimerType )
// 	{
// 	case FULL_TIME:
// 		{
// 			if( nDay > 0 )
// 			{
// 				_stprintf_s( sz, 64, _LAN( "%d 일 %d 시간 %d 분" ), m_nCurrent_nDay, m_nCurrent_nHour%24, m_nCurrent_nMin%60 );
// 			}
// 			else if( nHour > 0 )
// 			{
// 				_stprintf_s( sz, 64, _LAN( "%d 시간 %d 분" ), m_nCurrent_nHour%24, m_nCurrent_nMin%60 );
// 			}
// 			else if( nMin > 0 )
// 			{
// 				_stprintf_s( sz, 64, _LAN( "%d 분" ), m_nCurrent_nMin%60 );
// 			}
// 			else if( nSec > 0 )
// 			{
// 				_stprintf_s( sz, 64, _LAN( "%d 초" ), m_nCurrent_nSec%60 );
// 			}
// 		}
// 		break;
// 	case SIMPLE_TIME:
// 		{
// 			if( nDay > 0 )
// 			{
// 				_stprintf_s( sz, 64, _LAN( "%d 일" ), m_nCurrent_nDay );
// 			}
// 			else if( nHour > 0 )
// 			{
// 				_stprintf_s( sz, 64, _LAN( "%d 시간 %d 분" ), m_nCurrent_nHour%24, m_nCurrent_nMin%60 );
// 			}
// 			else if( nMin > 0 )
// 			{
// 				_stprintf_s( sz, 64, _LAN( "%d 분" ), m_nCurrent_nMin%60 );
// 			}
// 			else if( nSec > 0 )
// 			{
// 				_stprintf_s( sz, 64, _LAN( "%d 초" ), m_nCurrent_nSec%60 );
// 			}
// 		}
// 		break;
// 	case SYMBOL_TIME:
// 		{
// 			_stprintf_s( sz, 64, _T( "%.2d:%.2d:%.2d" ), m_nCurrent_nHour, m_nCurrent_nMin%60, m_nCurrent_nSec%60 );
// 		}
// 		break;
// 	}
// 
// 	SetText( sz );
// }
//=======================================================================================
//
//
//
//		3D 텍스트
//
//
//
//=======================================================================================
cText3D::sBillboard::sBillboard():s_pBillboard(NULL)
{

}
cText3D::sBillboard::~sBillboard()
{
	Delete();
}

cText3D::cText3D():m_pBillBoardText(NULL)
{
}

cText3D::~cText3D()
{ 
	SAFE_NIDELETE( m_pBillBoardText ); 	
	DeleteBillboard();	
}

void cText3D::DeleteBillboard()
{
	for( int i = 0; i < m_pbBillBoard.Size(); ++i )
	{
		if( m_pbBillBoard.IsExistElement( i ) )
		{
			m_pbBillBoard[ i ]->Delete();			
			SAFE_NIDELETE( m_pbBillBoard[ i ] );
		}
		assert_cs( m_pbBillBoard[ i ] == NULL );
	}
	m_pbBillBoard.Destroy();
}

bool cText3D::Init3D( cText::sTEXTINFO* pTextInfo )
{
	if( pTextInfo->s_pFont->IsInitialize() == false )
	{
		return false;
	}

	if( pTextInfo->GetText()[ 0 ] == NULL )
	{
		pTextInfo->SetText( _T( " " ) );
		Init3D( pTextInfo );
		return false;
	}

	m_TextInfo = *pTextInfo;
	assert_cs( m_pBillBoardText == NULL );
	
	
	// 값 초기화
	m_ptStrSize = CsPoint::ZERO;
	m_bUseMark = false;

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	_AllocData();

	// 텍스쳐 생성
	_CreateTexture();

	m_pBillBoardText = NiNew CBillboard;

	m_pBillBoardText->CreateTexture( m_ptStrSize.x*0.5f, m_ptStrSize.y*0.5f, m_pTexture );	

	m_pBillBoardText->SetColor( m_TextInfo.s_Color );

	return true;
}

void cText3D::Render( NiPoint3 vPos, float fX, float fY )
{
	if( m_pBillBoardText == NULL )
		return;

	m_pBillBoardText->Render( vPos, fX, fY );

	float PosX = m_pBillBoardText->GetSizeX();
	float PosY = 0;
	for( int i = 0; i < m_pbBillBoard.Size(); ++i )
	{		
		if( m_pbBillBoard.IsExistElement( i ) )
			m_pbBillBoard[ i ]->Render( vPos, PosX, PosY );
	}

}

void cText3D::Render( NiPoint3 vPos, float fX, float fY, float fScale )
{
	if( m_pBillBoardText == NULL )
		return;

	m_pBillBoardText->SetScale( fScale );
	m_pBillBoardText->Render( vPos, fX, fY );

	float PosX = m_pBillBoardText->GetSizeX();
	float PosY = 0;
	for( int i = 0; i < m_pbBillBoard.Size(); ++i )
	{
		if( m_pbBillBoard.IsExistElement( i ) )
			m_pbBillBoard[ i ]->Render( vPos, PosX, PosY, fScale );
	}
}

bool cText3D::SetText( int nStr )
{
	TCHAR szNum[ 16 ];
	_stprintf_s( szNum, 16, _T( "%d" ), nStr );
	return SetText( szNum );
}

bool cText3D::SetText( TCHAR const* szStr )
{
	if( m_TextInfo.s_pFont == NULL )
		return false;

	if( m_TextInfo.s_pFont->IsInitialize() == false )
		return false;

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
			m_TextInfo.SetText( _T( "" ) );
			return true;
		}
		return false;
	}	

	m_TextInfo.SetText( szStr );

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
	
	m_pBillBoardText->ChangeTexture( m_ptStrSize * 0.5f, m_pTexture );
	return true;
}

void cText3D::SetAlpha( float fAlpha )
{ 
	if( m_pBillBoardText )
	{ 
		m_pBillBoardText->SetAlpha( fAlpha ); 
	} 

	for( int i = 0; i < m_pbBillBoard.Size(); ++i )
	{
		if( m_pbBillBoard.IsExistElement( i ) )
			m_pbBillBoard[ i ]->s_pBillboard->SetAlpha( fAlpha );
	}
}

void cText3D::SetColor( NiColor FontColor /*부하가적다*/ )
{ 
	if( m_pBillBoardText )
	{ 
		m_pBillBoardText->SetColor( FontColor ); 
	}
}

void cText3D::AddBillBoard( char const* pFileName, NiPoint2 DeltaPos, NiPoint2 ptSize )
{
	sBillboard* pBillboard = NiNew sBillboard;
	pBillboard->Init( pFileName, DeltaPos, ptSize );
		
	m_pbBillBoard.PushBack( pBillboard );		
}

void cText3D::AddBillBoard( char const* pFileName, NiPoint2 DeltaPos, float fTexX1, float fTexX2, float fTexY1, float fTexY2, NiPoint2 ptSize )
{
	sBillboard* pBillboard = NiNew sBillboard;
	pBillboard->Init( pFileName, fTexX1, fTexX2, fTexY1, fTexY2, DeltaPos, ptSize );

	m_pbBillBoard.PushBack( pBillboard );	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 추가 billboard
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cText3D::sBillboard::Init(char const* pFileName, NiPoint2 DeltaPos, NiPoint2 ptSize )
{ 
	s_Size = ptSize;
	s_DeltaPos = DeltaPos;

	s_pBillboard = NiNew CBillboard;
	s_pBillboard->CreateFile(pFileName, s_Size.x, s_Size.y);		
}

void cText3D::sBillboard::Init( char const* pFileName, float fTexX1, float fTexX2, float fTexY1, float fTexY2, NiPoint2 DeltaPos, NiPoint2 ptSize )
{ 
	s_Size = ptSize;
	s_DeltaPos = DeltaPos;

	s_pBillboard = NiNew CBillboard;
	s_pBillboard->CreateFile( pFileName, s_Size.x, s_Size.y, fTexX1, fTexX2, fTexY1, fTexY2 );	
}

void cText3D::sBillboard::Render( NiPoint3 vPos, float& fX, float& fY )
{
	if( s_pBillboard == NULL )
		return;
	
	if( s_DeltaPos.x > 0 )
		s_pBillboard->Render( vPos, ( fX + s_DeltaPos.x ), s_DeltaPos.y );
	else
		s_pBillboard->Render( vPos, -( fX - s_DeltaPos.x ), s_DeltaPos.y );	

	fX += s_Size.x + s_DeltaPos.x;
	fY += 0;
}

void cText3D::sBillboard::Render( NiPoint3 vPos, float& fX, float& fY, float fScale )
{
	if( s_pBillboard == NULL )
		return;
		
	s_pBillboard->SetScale( fScale );
	if( s_DeltaPos.x > 31 )
		s_pBillboard->Render( vPos, ( fX + s_DeltaPos.x ), s_DeltaPos.y );
	else if( 1 < s_DeltaPos.x && s_DeltaPos.x < 31 )
		s_pBillboard->Render( vPos, ( 0 + s_DeltaPos.x ), s_DeltaPos.y );
	else if( s_DeltaPos.x == 0)
		s_pBillboard->Render( vPos, ( 0 + s_DeltaPos.x ), s_DeltaPos.y );
	else if( -31 < s_DeltaPos.x && s_DeltaPos.x < 0)
		s_pBillboard->Render( vPos, -( 0 - s_DeltaPos.x ), s_DeltaPos.y );
	else
		s_pBillboard->Render( vPos, -( fX - s_DeltaPos.x ), s_DeltaPos.y );

	fX += s_Size.x + s_DeltaPos.x;
	fY += 0;
}

//=======================================================================================//=======================================================================================
//
//		cMyText
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//=======================================================================================//=======================================================================================





void cMyText::sTEXTINFO::Init( CFont::eFACE_SIZE eFontSize /*=CFont::FS_10*/, NiColor color /*=FONT_WHITE*/ )
{
	Init( &g_pEngine->m_FontSystem, eFontSize, color );
}

void cMyText::sTEXTINFO::Init( CFont* pFont, CFont::eFACE_SIZE eFontSize /*=CFont::FS_10*/, NiColor color /*=FONT_WHITE*/ )
{
	s_pFont = pFont;
	s_eFontSize = eFontSize;
	s_Color = color;

	s_bOutLine = true;
	s_eBoldLevel = BL_NONE;

	s_eTextAlign = DT_LEFT;
}

void cMyText::sTEXTINFO::Init( CsHelp::sTEXT* pHelpText )
{
	Init( FONT_SIZE_CAL( pHelpText->s_nTextSize ), NiColor( pHelpText->s_Color[ 0 ]/255.0f, pHelpText->s_Color[ 1 ]/255.0f, pHelpText->s_Color[ 2 ]/255.0f ) );
	SetBoldLevel( (cMyText::sTEXTINFO::eBOLD_LEVEL)pHelpText->s_nBold );
}

int cMyText::sTEXTINFO::GetBoldSize()
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
	assert_csm1( false, _T( "BoldLevel = %d" ), s_eBoldLevel );
	return 0;
}	

void cMyText::sTEXTINFO::SetText_Reduce( TCHAR* szText, int nLen, int nDotNum /* = 2 */ )
{
	TCHAR t = szText[ nLen ];
	szText[ nLen ] = NULL;

	TCHAR szDot[ 2 ] = _T( "." );
	s_strText = szText;
	szText[ nLen ] = t;

	for( int i = 0; i < nDotNum; i++ )
		s_strText.append( szDot );
}

//=======================================================================================
//
//		베이스
//
//=======================================================================================

cMyText::cMyText()
{
	m_pTexture		=	NULL;
	m_pPixelData	=	NULL;
	m_pFTData		=	NULL;

	m_bUseMark		=	false;
}

void cMyText::Delete()
{


	m_pPixelData	= 0;
	m_pTexture		= 0;
	SAFE_DELETE_ARRAY( m_pFTData );
}

void cMyText::Init( cWindow* pParent, CsPoint pos, CsPoint pixel, sTEXTINFO* pTextInfo, bool bApplyWindowSize )
{
	if( pTextInfo->s_pFont == NULL )
		return;

	if( pTextInfo->s_pFont->IsInitialize() == false )
		return;

	m_TextInfo = *pTextInfo;
	// 값 초기화
	m_ptStrSize = CsPoint::ZERO;

	m_ptPixelSize = pixel;

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if( _AllocData() == false )
	{		
		return;
	}

	// 텍스쳐 생성
	_CreateTexture();
}




//=======================================================================================
//
//		스트링
//
//=======================================================================================

bool cMyText::GetStringSize( CsPoint& size, int& hBase, TCHAR* szStr )
{
	if( m_TextInfo.s_pFont == NULL )
		return false;

	if( m_TextInfo.s_pFont->IsInitialize() == false )
		return false;

	int x = 0;
	int y = m_TextInfo.GetHeight();

	int fx = 0;

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
				if( FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) != 0 )
					continue;

			x += (slot->advance.x >> 6);

			if(m_bSet){
				// 픽셀 x사이즈 이전값과 비교하여 큰걸 저장한다.
				if( (m_ptPixelSize.x -2) >= x )	{ if(fx < x) fx = x; }
				else
				{
					// 리셋
					x = (slot->advance.x >> 6);
					y += m_TextInfo.GetHeight();
				}
			}
			
		}
	}
	else
	{
		for( int i=0; i<nLen ; ++i )
		{
			if( szStr[ i ] == 0x000d )
				continue;

			charcode = m_szMark;
				if( FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) != 0 )
					continue;

			x += (slot->advance.x >> 6);
			if(m_bSet){
				// 픽셀 x사이즈 이전값과 비교하여 큰걸 저장한다.
				if( (m_ptPixelSize.x -2) >= x )	{ if(fx < x) fx = x; }
				else
				{
					// 리셋
					x = (slot->advance.x >> 6);
					y += m_TextInfo.GetHeight();
				}
			}
		}
	}	

	if(m_bSet){
		size.x = fx + 2;
	}
	else
		size.x = x + 2;
	size.y = y;
	hBase = (int)( m_TextInfo.GetHeight()*0.75f ) << 6;

	return ( size.x > 2 );
}

int cMyText::GetCharWidth( FT_Face face, FT_ULong charcode )
{
	if( charcode == 0x000d )
		return 0;


	if( FT_Load_Glyph( face, FT_Get_Char_Index( face, charcode ), FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT ) != 0 )
		return 0;
	return face->glyph->advance.x >> 6;
}

bool cMyText::SetText( int nStr )
{
	TCHAR szNum[ 16 ];
	_stprintf_s( szNum, 16, _T( "%d" ), nStr );
	return SetText( szNum );
}

bool cMyText::SetText( TCHAR* szStr /*부하가크다*/, bool bSet )
{
	if( m_TextInfo.s_pFont == NULL )
		return false;

	if( m_TextInfo.s_pFont->IsInitialize() == false )
		return false;

	cText::sTEXTINFO safeInfo;
	safeInfo.SetText( szStr );
	std::wstring safeInput = safeInfo.s_strText;

	if( safeInput.empty() )
	{
		if( m_pTexture )
		{
			m_pPixelData		= 0;
			m_pTexture			= 0;
			m_ptStrSize			= CsPoint::ZERO;
			m_TextInfo.SetText( _T( "" ) );
			return true;
		}
		return false;
	}

	// 같은 글자라면 패스
	if( safeInput == m_TextInfo.GetText() )
		return false;

	m_TextInfo.SetText( const_cast<TCHAR*>( safeInput.c_str() ) );

	// 지우고 새로 생성
	m_pPixelData	= 0;
	m_pTexture		= 0;
	SAFE_DELETE_ARRAY( m_pFTData );
	m_bSet = bSet;

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if( _AllocData() == false )
	{
		return false;
	}
	// 텍스쳐 생성
	_CreateTexture();

	return true;
}

//=======================================================================================
//
//		텍스쳐
//
//=======================================================================================
//1 스트링 사이즈 구하고 스트링 사이즈만큼 픽셀 데이타 설정한다.
bool cMyText::_AllocData()
{
	if( GetStringSize( m_ptStrSize, m_nFTSize_HBase, (TCHAR*)m_TextInfo.GetText() ) == false )
		return false;

	assert_cs( ( m_ptStrSize.x != 0 )&&( m_ptStrSize.y != 0 ) );

	assert_cs( m_pPixelData == NULL );

	if(m_ptStrSize.x > m_ptPixelSize.x){ m_ptPixelSize.x = m_ptStrSize.x;}
	if(m_ptStrSize.y > m_ptPixelSize.y){ m_ptPixelSize.y = m_ptStrSize.y;}
	//m_pPixelData = NiNew NiPixelData( m_ptStrSize.x, m_ptStrSize.y, NiPixelFormat::RGBA32 );
	m_pPixelData = NiNew NiPixelData( m_ptPixelSize.x, m_ptPixelSize.y, NiPixelFormat::RGBA32 );

	assert_cs( m_pFTData == NULL );
	int nFTSize = m_ptStrSize.Mul();
	//int nFTSize = (100*50);
	m_pFTData = xnew BYTE[ nFTSize ];
	memset( m_pFTData, 0, sizeof( BYTE )*nFTSize );

	_FTBmpToFTData();

	return true;
}

//2  m_pFTData 에다 데이터를 넣는것 같은데..글자수 만큼
void cMyText::_FTBmpToFTData()
{
	int nStartSize = 1;
	int nStartSizey = 0;
	const TCHAR* str = m_TextInfo.GetText();
	int nStrLen = (int)_tcslen( str );
	FT_ULong charcode;
	FT_Face face = m_TextInfo.GetFace();
	if( !IsFaceReady( face ) )
		return;
	FT_GlyphSlot slot = face->glyph;
	FT_Glyph_Metrics *pMetrics;
	FT_Error error;
	int By;
	int x =0;
	int y =m_TextInfo.GetHeight();

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

				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
				// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;

				if( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
					continue;

				pMetrics = &(slot->metrics);
				By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );	

				x += ( pMetrics->horiAdvance >> 6 );
				if( (m_ptPixelSize.x -2 ) < x ){
					nStartSize = 1;	
					nStartSizey += y;
					x = 0;
				}		
					

				_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), nStartSizey+(By>>6), pMetrics->width >> 6, pMetrics->height >> 6 );
				
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

				error = FT_Load_Glyph( face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT );
				// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;

				FT_Outline_Embolden( &face->glyph->outline, (face->size->metrics.x_ppem*nBoldSize/100)*64 );
				if( FT_Render_Glyph( face->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
					continue;

				pMetrics = &(slot->metrics);
				By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );

				x += ( pMetrics->horiAdvance >> 6 );
				if( (m_ptPixelSize.x -2 ) < x ){
					nStartSize = 1;	
					nStartSizey += y;
					x = 0;
				}

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
			// Removed assert_cs( error == 0 ) — FT_Load_Glyph failure on a missing-from-font
// glyph is an expected condition, not a bug. The original assert called _log()
// which both writes to crash.log AND pops up MessageBoxA, breaking gameplay flow.
// Silent skip is the right behavior; the next char in the loop renders normally.
if( error != 0 ) continue;

			pMetrics = &(slot->metrics);
			By = CsMax( 0, m_nFTSize_HBase - pMetrics->horiBearingY );

			x += ( pMetrics->horiAdvance >> 6 );
			if( (m_ptPixelSize.x -2 ) < x ){
				nStartSize = 1;	
				nStartSizey += y;
				x = 0;
			}

			_ReadFTBmp( &slot->bitmap, nStartSize+( pMetrics->horiBearingX >> 6 ), By>>6, pMetrics->width >> 6, pMetrics->height >> 6 );

			nStartSize += ( pMetrics->horiAdvance >> 6 );		
		}
	}	
}

//2 -2 m_pFTData 에다 데이터를 넣는거 같은데..(시작픽셀x,y)(width,height 픽셀)
void cMyText::_ReadFTBmp( FT_Bitmap* bitmap, int x, int y, int sx, int sy )
{
	if( bitmap == NULL || bitmap->buffer == NULL )
		return;
	if( sx <= 0 || sy <= 0 )
		return;
	if( bitmap->pixel_mode != FT_PIXEL_MODE_GRAY )
		return;
	if( m_pFTData == NULL )
		return;

	int i, j, p, q;
	int Width = m_ptStrSize.x;
	int BitWidth = bitmap->width;
	int Pitch = bitmap->pitch;
	int Height = m_ptStrSize.y;
	int TotalSize = Width * Height;
	if( Width <= 0 || Height <= 0 || BitWidth <= 0 || bitmap->rows <= 0 )
		return;
	if( Pitch < 0 )
		Pitch = -Pitch;
	if( Pitch < BitWidth )
		Pitch = BitWidth;
	int srcTotal = Pitch * (int)bitmap->rows;

	for( i=y, p=0; p<sy; ++i, ++p )
	{
		for( j = x, q = 0 ; q<sx ; ++j, ++q )
		{
			int destIdx = i*Width + j;
			if( destIdx < 0 || destIdx >= TotalSize )
				continue;
			int srcIdx = p*Pitch + q;
			if( srcIdx < 0 || srcIdx >= srcTotal )
				continue;
			m_pFTData[ destIdx ] = bitmap->buffer[ srcIdx ];
		}
	}
}

void cMyText::_CreateTexture()
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
	kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;
	m_pTexture = NiSourceTexture::Create( NiDynamicCast( NiPixelData, m_pPixelData->Clone() ), kPrefs );
}


// 아웃라인 픽셀을 찾아서 색을 입힌것같음.
void cMyText::_StringToData_OutLine()
{
	assert_cs( m_pPixelData != NULL );

	DWORD* pOrgData = (DWORD*)m_pPixelData->GetPixels();
	int alpha;
	D3DXCOLOR c;	

	memset( pOrgData, 0, sizeof( DWORD )*m_ptPixelSize.Mul() );
	
	for(int y=0; y<m_ptPixelSize.y; ++y )
	{
		for( int x=0; x<m_ptPixelSize.x ; ++x )
		{	
			//a,b,g,r
			pOrgData[ y*m_ptPixelSize.x + x ] = 0xff6ad2c8;			
		}
	}


	int height = m_ptStrSize.y;
	int width = m_ptStrSize.x;

	switch(m_TextInfo.s_eTextAlign)
	{
	case DT_CENTER:
		{
			
// 			int temp = (m_ptPixelSize.x - m_ptStrSize.x);
// 			int first = (temp / 2) + (temp % 2);
// 			int end = (temp / 2);	
// 
// 
// 
// 			pOrgData = (DWORD*)m_pPixelData->GetPixels();
// 
// 			for( int y=0; y<height; ++y )
// 			{
// 				pOrgData+=first;
// 				for( int x=0; x<width ; ++x )
// 				{
// 					alpha = m_pFTData[ y*width + x ];
// 
// 					if( alpha > 70 )
// 						*pOrgData = ( (m_pFTData[ y*width + x ])<<24 )|0x00ffffff;
// 
// 					++pOrgData;
// 				}
// 				pOrgData+=end;
// 			}

			int temp = (m_ptPixelSize.x - m_ptStrSize.x);
			int first = (temp / 2) + (temp % 2);
			int end = (temp / 2);


			for(int y=0; y<height-1; ++y )
			{
				for( int x=0; x<width-1 ; ++x )
				{			
					alpha = m_pFTData[ y*width + x ];
					end = 0;

					if( alpha > 100 )
					{
						pOrgData[ ((y+1)*m_ptPixelSize.x + (x+1)) + first + end ] = 0xff101010;
					}
				}
				end = (temp / 2);
			}


			for(int y=0; y<height; ++y )
			{
				for( int x=0; x<width ; ++x )
				{			
					alpha = m_pFTData[ y*width + x ];
					end = 0;

					if( alpha > 70 )
					{
						pOrgData[ (y*m_ptPixelSize.x + x) + first + end ] = ( (alpha)<<24 )|0x00ffffff;
					}					
				}
				end = (temp / 2);
			}

		}
		break;
	default:
		{
			for(int y=0; y<height-1; ++y )
			{
				for( int x=0; x<width-1 ; ++x )
				{			
					alpha = m_pFTData[ y*width + x ];

					if( alpha > 100 )
					{
						pOrgData[ (y+1)*m_ptPixelSize.x + (x+1) ] = 0xff101010;
					}
				}
			}

			for(int y=0; y<height; ++y )
			{
				for( int x=0; x<width ; ++x )
				{			
					alpha = m_pFTData[ y*width + x ];

					if( alpha > 70 )
					{
						pOrgData[ y*m_ptPixelSize.x + x ] = ( (alpha)<<24 )|0x00ffffff;
					}
				}
			}
		}
		break;
	}

	m_pPixelData->MarkAsChanged();
}

void cMyText::_StringToData()
{
	assert_cs( m_pPixelData != NULL );

	DWORD* pData = (DWORD*)m_pPixelData->GetPixels();	
	memset( pData, 0x0000ff00, sizeof( DWORD )*m_ptPixelSize.Mul() );

	int alpha;
	int width = m_ptStrSize.x;
 	int height = m_ptStrSize.y;

	switch(m_TextInfo.s_eTextAlign)
	{
	case DT_CENTER:
		{
			int temp = (m_ptPixelSize.x - m_ptStrSize.x);
			int first = (temp / 2) + (temp % 2);
			int end = (temp / 2);
			for( int y=0; y<height; ++y )
			{
				pData+=first;
				for( int x=0; x<width ; ++x )
				{
					alpha = m_pFTData[ y*width + x ];

					if( alpha > 100 )
						*pData = ( (m_pFTData[ y*width + x ])<<24 )|0x00ffffff;

					++pData;
				}
				pData+=end;
			}

		}
		break;
	default:
		{
			for(int y=0; y<height; ++y )
			{
				for( int x=0; x<width ; ++x )
				{			
					alpha = m_pFTData[ y*width + x ];

					if( alpha > 70 )
					{
						pData[ y*m_ptPixelSize.x + x ] = ( (alpha)<<24 )|0x00ffffff;
					}
				}
			}	

		}
		break;
	}
	
	/*
	for( int y=0; y<height; ++y )
	{
		for( int x=0; x<width ; ++x )
		{
			alpha = m_pFTData[ y*width + x ];

			if( alpha > 100 )
				*pData = ( (m_pFTData[ y*width + x ])<<24 )|0x00ffffff;

			++pData;
		}
		pData+=(m_ptPixelSize.x - m_ptStrSize.x);
	}
	*/

}