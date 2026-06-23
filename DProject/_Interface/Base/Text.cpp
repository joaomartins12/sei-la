
#include "stdafx.h"
#include "Text.h"

namespace
{
	static const size_t TEXT_INPUT_MAX_CHARS = 1024;

	static const int TEXT_TEXTURE_MAX_WIDTH = 1024;
	static const int TEXT_TEXTURE_MAX_HEIGHT = 512;
	static const int TEXT_TEXTURE_MAX_PIXELS = TEXT_TEXTURE_MAX_WIDTH * TEXT_TEXTURE_MAX_HEIGHT;

	static bool g_bEnableTextPerfLog = true;

	static int g_nTextSetCount = 0;
	static int g_nTextSetWidthCount = 0;
	static int g_nText3DSetCount = 0;
	static int g_nMyTextSetCount = 0;

	static DWORD g_dwLastTextPerfLog = 0;

	static void TextPerfCount(const char* tag)
	{
		if (!g_bEnableTextPerfLog)
			return;

		if (tag == NULL)
			return;

		if (strcmp(tag, "cText::SetText") == 0)
			++g_nTextSetCount;
		else if (strcmp(tag, "cText::SetTextWidth") == 0)
			++g_nTextSetWidthCount;
		else if (strcmp(tag, "cText3D::SetText") == 0)
			++g_nText3DSetCount;
		else if (strcmp(tag, "cMyText::SetText") == 0)
			++g_nMyTextSetCount;

		DWORD now = GetTickCount();

		if (g_dwLastTextPerfLog == 0)
			g_dwLastTextPerfLog = now;

		if (now - g_dwLastTextPerfLog >= 1000)
		{
			char log[256] = { 0 };

			sprintf_s(
				log,
				"[TEXT_PERF] cText=%d cTextWidth=%d cText3D=%d cMyText=%d\n",
				g_nTextSetCount,
				g_nTextSetWidthCount,
				g_nText3DSetCount,
				g_nMyTextSetCount
			);

			OutputDebugStringA(log);

			g_nTextSetCount = 0;
			g_nTextSetWidthCount = 0;
			g_nText3DSetCount = 0;
			g_nMyTextSetCount = 0;

			g_dwLastTextPerfLog = now;
		}
	}

	static bool IsFaceReady(FT_Face face)
	{
		if (face == NULL)
			return false;

		__try
		{
			if (face->glyph == NULL)
				return false;

			if (face->size == NULL)
				return false;

			if (face->num_faces <= 0 || face->num_faces > 64)
				return false;

			if (face->face_index < 0 || face->face_index >= face->num_faces)
				return false;

			if (face->num_glyphs <= 0 || face->num_glyphs > 300000)
				return false;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}

		return true;
	}

	static bool IsWhiteChar(FT_ULong charcode)
	{
		return charcode == L' ' || charcode == L'\t';
	}

	static int GetFallbackCharWidthPx(FT_Face face, FT_ULong charcode)
	{
		int h = 14;

		__try
		{
			if (face != NULL && face->size != NULL && face->size->metrics.y_ppem > 0)
				h = face->size->metrics.y_ppem;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			h = 14;
		}

		if (charcode == L'\t')
			return CsMax(12, h);

		if (charcode == L' ')
			return CsMax(4, h / 3);

		return CsMax(6, h / 2);
	}

	static FT_UInt GetGlyphIndexSafe(FT_Face face, FT_ULong charcode)
	{
		if (!IsFaceReady(face))
			return 0;

		FT_UInt glyphIndex = 0;

		__try
		{
			glyphIndex = FT_Get_Char_Index(face, charcode);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			glyphIndex = 0;
		}

		return glyphIndex;
	}

	static bool LoadGlyphSafe(FT_Face face, FT_ULong charcode, FT_Int32 flags)
	{
		if (!IsFaceReady(face))
			return false;

		FT_UInt glyphIndex = GetGlyphIndexSafe(face, charcode);
		if (glyphIndex == 0)
		{
			if (IsWhiteChar(charcode))
				return true;

			return false;
		}

		FT_Error error = 1;

		__try
		{
			error = FT_Load_Glyph(face, glyphIndex, flags);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}

		return error == 0 && IsFaceReady(face);
	}

	static bool LoadCharSafe(FT_Face face, FT_ULong charcode, FT_Int32 flags)
	{
		if (!IsFaceReady(face))
			return false;

		FT_Error error = 1;

		__try
		{
			error = FT_Load_Char(face, charcode, flags);
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			return false;
		}

		return error == 0 && IsFaceReady(face);
	}

	static int GetCharAdvancePxSafe(FT_Face face, FT_ULong charcode)
	{
		if (charcode == 0x000d || charcode == 0x000a)
			return 0;

		if (!IsFaceReady(face))
			return GetFallbackCharWidthPx(face, charcode);

		if (!LoadGlyphSafe(face, charcode, FT_LOAD_DEFAULT | FT_LOAD_FORCE_AUTOHINT))
			return GetFallbackCharWidthPx(face, charcode);

		if (!IsFaceReady(face) || face->glyph == NULL)
			return GetFallbackCharWidthPx(face, charcode);

		int adv = 0;

		__try
		{
			adv = face->glyph->advance.x >> 6;
		}
		__except (EXCEPTION_EXECUTE_HANDLER)
		{
			adv = 0;
		}

		if (adv <= 0 || adv > 256)
			adv = GetFallbackCharWidthPx(face, charcode);

		return adv;
	}

	static int GetCharAdvance26Safe(FT_Face face, FT_ULong charcode)
	{
		return GetCharAdvancePxSafe(face, charcode) << 6;
	}

	static bool LoadRenderedGlyphSafe(FT_Face face, FT_ULong charcode)
	{
		if (!IsFaceReady(face))
			return false;

		if (!LoadCharSafe(face, charcode, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT))
			return false;

		if (!IsFaceReady(face))
			return false;

		if (face->glyph == NULL)
			return false;

		return true;
	}

	static bool RenderGlyphSafe(FT_Face face)
	{
		// Não usar FT_Render_Glyph separado.
		return false;
	}

	static bool EmboldenGlyphSafe(FT_Face face, int nBoldSize)
	{
		// Desativado temporariamente para evitar corrupção de glyph/heap.
		return true;
	}

	static bool IsSafeTextTextureSize(const CsPoint& size)
	{
		if (size.x <= 0 || size.y <= 0)
			return false;

		if (size.x > TEXT_TEXTURE_MAX_WIDTH || size.y > TEXT_TEXTURE_MAX_HEIGHT)
			return false;

		if (size.x > (TEXT_TEXTURE_MAX_PIXELS / size.y))
			return false;

		return true;
	}

	static int SafeMul4(const CsPoint& size)
	{
		if (!IsSafeTextTextureSize(size))
			return 0;

		const int pixels = size.x * size.y;

		if (pixels <= 0 || pixels > TEXT_TEXTURE_MAX_PIXELS)
			return 0;

		return pixels * 4;
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
	return s_strText.c_str();
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

void cText::Init(cWindow* pParent, CsPoint pos, sTEXTINFO* pTextInfo, bool bApplyWindowSize)
{
	if (pTextInfo == NULL)
		return;

	if (pTextInfo->s_pFont == NULL)
		return;

	if (pTextInfo->s_pFont->IsInitialize() == false)
		return;

	m_TextInfo = *pTextInfo;
	m_ptStrSize = CsPoint::ZERO;

	if (_AllocData() == false)
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		SAFE_DELETE_ARRAY(m_pFTData);

		m_Sprite.Init(pParent, pos, CsPoint::ZERO, bApplyWindowSize, m_TextInfo.s_Color);
		return;
	}

	_CreateTexture();

	if (m_pTexture == NULL)
	{
		m_pPixelData = 0;
		SAFE_DELETE_ARRAY(m_pFTData);

		m_Sprite.Init(pParent, pos, CsPoint::ZERO, bApplyWindowSize, m_TextInfo.s_Color);
		return;
	}

	m_Sprite.Init(pParent, pos, m_ptStrSize, m_pTexture, bApplyWindowSize, m_TextInfo.s_Color);
}

void cText::InitStencil(cWindow* pParent, CsPoint pos, sTEXTINFO* pTextInfo, bool bApplyWindowSize, NiStencilProperty* pPropStencil)
{
	if (pTextInfo == NULL)
		return;

	if (pTextInfo->s_pFont == NULL)
		return;

	if (pTextInfo->s_pFont->IsInitialize() == false)
		return;

	m_TextInfo = *pTextInfo;
	m_ptStrSize = CsPoint::ZERO;

	if (_AllocData() == false)
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		SAFE_DELETE_ARRAY(m_pFTData);

		m_Sprite.InitStencil(pParent, pos, CsPoint::ZERO, bApplyWindowSize, pPropStencil, m_TextInfo.s_Color);
		return;
	}

	_CreateTexture();

	if (m_pTexture == NULL)
	{
		m_pPixelData = 0;
		SAFE_DELETE_ARRAY(m_pFTData);

		m_Sprite.InitStencil(pParent, pos, CsPoint::ZERO, bApplyWindowSize, pPropStencil, m_TextInfo.s_Color);
		return;
	}

	m_Sprite.InitStencil(pParent, pos, m_ptStrSize, m_pTexture, bApplyWindowSize, pPropStencil, m_TextInfo.s_Color);
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

bool cText::GetStringSize(CsPoint& size, int& hBase, TCHAR* szStr, std::wstring& wsResult, int nWidth)
{
	size = CsPoint::ZERO;
	hBase = 0;
	wsResult.clear();

	if (szStr == NULL)
		return false;

	if (NULL == m_TextInfo.s_pFont)
		return false;

	if (!m_TextInfo.s_pFont->IsInitialize())
		return false;

	FT_Face face = m_TextInfo.GetFace();
	if (!IsFaceReady(face))
		return false;

	int nLen = 0;

	__try
	{
		nLen = (int)_tcslen(szStr);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	if (nLen <= 0)
		return false;

	if (nLen > (int)TEXT_INPUT_MAX_CHARS)
		nLen = (int)TEXT_INPUT_MAX_CHARS;

	if (nWidth <= 0)
		return false;

	if (nWidth > TEXT_TEXTURE_MAX_WIDTH)
		nWidth = TEXT_TEXTURE_MAX_WIDTH;

	int nLimitWidth = nWidth << 6;
	int nMaxWidth = 0;
	int nMaxHeight = m_TextInfo.GetHeight();

	if (nMaxHeight <= 0 || nMaxHeight > TEXT_TEXTURE_MAX_HEIGHT)
		return false;

	int x = 0;

	for (int i = 0; i < nLen; ++i)
	{
		if (szStr[i] == 0x000a || szStr[i] == 0x000d)
		{
			wsResult += szStr[i];

			if (nMaxWidth < x)
				nMaxWidth = x;

			x = 0;
			nMaxHeight += (m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight());

			if (nMaxHeight > TEXT_TEXTURE_MAX_HEIGHT)
				break;

			continue;
		}

		FT_ULong charcode = m_bUseMark ? (FT_ULong)m_szMark : (FT_ULong)szStr[i];
		int advance = GetCharAdvance26Safe(face, charcode);

		if (x + advance > nLimitWidth)
		{
			wsResult += _T("\n");
			wsResult += szStr[i];

			if (nMaxWidth < x)
				nMaxWidth = x;

			x = advance;
			nMaxHeight += (m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight());

			if (nMaxHeight > TEXT_TEXTURE_MAX_HEIGHT)
				break;

			continue;
		}

		wsResult += szStr[i];
		x += advance;

		if (nMaxWidth < x)
			nMaxWidth = x;
	}

	size.x = (nMaxWidth >> 6) + 2;
	size.y = nMaxHeight;
	hBase = (int)(m_TextInfo.GetHeight() * 0.75f) << 6;

	if (!IsSafeTextTextureSize(size))
	{
		size = CsPoint::ZERO;
		hBase = 0;
		return false;
	}

	return size.x > 2;
}

bool cText::GetStringSize(CsPoint& size, int& hBase, TCHAR* szStr)
{
	size = CsPoint::ZERO;
	hBase = 0;

	if (szStr == NULL)
		return false;

	if (m_TextInfo.s_pFont == NULL)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	FT_Face face = m_TextInfo.GetFace();
	if (!IsFaceReady(face))
		return false;

	int nLen = 0;

	__try
	{
		nLen = (int)_tcslen(szStr);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	if (nLen <= 0)
		return false;

	if (nLen > (int)TEXT_INPUT_MAX_CHARS)
		nLen = (int)TEXT_INPUT_MAX_CHARS;

	int x = 0;

	for (int i = 0; i < nLen; ++i)
	{
		if (szStr[i] == 0x000d)
			continue;

		if (szStr[i] == 0x000a)
			break;

		FT_ULong charcode = m_bUseMark ? (FT_ULong)m_szMark : (FT_ULong)szStr[i];

		x += GetCharAdvance26Safe(face, charcode);

		if ((x >> 6) > TEXT_TEXTURE_MAX_WIDTH)
			break;
	}

	size.x = (x >> 6) + 2;
	size.y = m_TextInfo.GetHeight();
	hBase = (int)(size.y * 0.75f) << 6;

	if (!IsSafeTextTextureSize(size))
	{
		size = CsPoint::ZERO;
		hBase = 0;
		return false;
	}

	return size.x > 2;
}

void cText::GetStringSize(sTEXTINFO* TextInfo, CsPoint& size, int& hBase, const TCHAR* szStr)
{
	size = CsPoint::ZERO;
	hBase = 0;

	if (TextInfo == NULL)
		return;

	if (szStr == NULL)
		return;

	if (TextInfo->s_pFont == NULL)
		return;

	if (TextInfo->s_pFont->IsInitialize() == false)
		return;

	FT_Face face = TextInfo->GetFace();
	if (!IsFaceReady(face))
		return;

	int nLen = 0;

	__try
	{
		nLen = (int)_tcslen(szStr);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}

	if (nLen <= 0)
		return;

	if (nLen > (int)TEXT_INPUT_MAX_CHARS)
		nLen = (int)TEXT_INPUT_MAX_CHARS;

	int x = 0;

	for (int i = 0; i < nLen; ++i)
	{
		if (szStr[i] == 0x000d)
			continue;

		if (szStr[i] == 0x000a)
			break;

		x += GetCharAdvance26Safe(face, szStr[i]);

		if ((x >> 6) > TEXT_TEXTURE_MAX_WIDTH)
			break;
	}

	size.x = (x >> 6) + 2;
	size.y = TextInfo->GetHeight();
	hBase = (int)(size.y * 0.75f) << 6;

	if (!IsSafeTextTextureSize(size))
	{
		size = CsPoint::ZERO;
		hBase = 0;
		return;
	}
}

int cText::GetCharWidth(FT_Face face, FT_ULong charcode)
{
	return GetCharAdvancePxSafe(face, charcode);
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

bool cText::SetText(TCHAR const* szStr /*부하가크다*/)
{
	TextPerfCount("cText::SetText");

	if (NULL == m_TextInfo.s_pFont)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	cText::sTEXTINFO safeInfo;
	safeInfo.SetText(szStr);
	std::wstring safeInput = safeInfo.s_strText;

	if (safeInput.empty())
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		m_ptStrSize = CsPoint::ZERO;
		m_TextInfo.SetText(_T(""));
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	if (safeInput == m_TextInfo.GetText())
		return false;

	m_TextInfo.SetText(safeInput.c_str());

	m_pPixelData = 0;
	m_pTexture = 0;
	SAFE_DELETE_ARRAY(m_pFTData);

	if (_AllocData() == false)
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	_CreateTexture();

	if (m_pTexture == NULL)
	{
		m_pPixelData = 0;
		m_ptStrSize = CsPoint::ZERO;
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	m_Sprite.ChangeTexture(m_ptStrSize, m_pTexture);
	m_Sprite.SetColor(m_TextInfo.s_Color);

	return true;
}

bool cText::SetText(TCHAR const* szStr /*부하가크다*/, int nWidth)
{
	TextPerfCount("cText::SetTextWidth");

	if (NULL == m_TextInfo.s_pFont)
		return false;

	if (!m_TextInfo.s_pFont->IsInitialize())
		return false;

	if (nWidth <= 0)
		return false;

	cText::sTEXTINFO safeInfo;
	safeInfo.SetText(szStr);
	std::wstring safeInput = safeInfo.s_strText;

	if (safeInput.empty())
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		m_ptStrSize = CsPoint::ZERO;
		m_TextInfo.SetText(_T(""));
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	if (safeInput == m_TextInfo.GetText())
		return false;

	m_TextInfo.SetText(safeInput.c_str());

	m_pPixelData = 0;
	m_pTexture = 0;
	SAFE_DELETE_ARRAY(m_pFTData);

	if (!_AllocData(nWidth))
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	_CreateTexture();

	if (m_pTexture == NULL)
	{
		m_pPixelData = 0;
		m_ptStrSize = CsPoint::ZERO;
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	m_Sprite.ChangeTexture(m_ptStrSize, m_pTexture);
	m_Sprite.SetColor(m_TextInfo.s_Color);

	return true;
}
//=======================================================================================
//
//		텍스쳐
//
//=======================================================================================

bool cText::_AllocData()
{
	if (GetStringSize(m_ptStrSize, m_nFTSize_HBase, (TCHAR*)m_TextInfo.GetText()) == false)
		return false;

	if (!IsSafeTextTextureSize(m_ptStrSize))
	{
		m_ptStrSize = CsPoint::ZERO;
		return false;
	}

	if (m_pPixelData != NULL)
	{
		m_pPixelData = 0;
	}

	if (m_pFTData != NULL)
	{
		SAFE_DELETE_ARRAY(m_pFTData);
	}

	m_pPixelData = NiNew NiPixelData(m_ptStrSize.x, m_ptStrSize.y, NiPixelFormat::RGBA32);
	if (m_pPixelData == NULL)
		return false;

	int nFTSize = SafeMul4(m_ptStrSize);
	if (nFTSize <= 0)
	{
		m_pPixelData = 0;
		return false;
	}

	m_pFTData = xnew BYTE[nFTSize];
	if (m_pFTData == NULL)
	{
		m_pPixelData = 0;
		return false;
	}

	memset(m_pFTData, 0, sizeof(BYTE) * nFTSize);

	_FTBmpToFTData();

	return true;
}

bool cText::_AllocData(int nWidth)
{
	if (nWidth <= 0)
		return false;

	if (nWidth > TEXT_TEXTURE_MAX_WIDTH)
		nWidth = TEXT_TEXTURE_MAX_WIDTH;

	std::wstring result;

	if (!GetStringSize(m_ptStrSize, m_nFTSize_HBase, (TCHAR*)m_TextInfo.GetText(), result, nWidth))
		return false;

	if (result.length() > TEXT_INPUT_MAX_CHARS)
		result.resize(TEXT_INPUT_MAX_CHARS);

	m_TextInfo.SetText(result.c_str());

	if (!IsSafeTextTextureSize(m_ptStrSize))
	{
		m_ptStrSize = CsPoint::ZERO;
		return false;
	}

	if (m_pPixelData != NULL)
	{
		m_pPixelData = 0;
	}

	if (m_pFTData != NULL)
	{
		SAFE_DELETE_ARRAY(m_pFTData);
	}

	m_pPixelData = NiNew NiPixelData(m_ptStrSize.x, m_ptStrSize.y, NiPixelFormat::RGBA32);
	if (m_pPixelData == NULL)
		return false;

	int nFTSize = SafeMul4(m_ptStrSize);
	if (nFTSize <= 0)
	{
		m_pPixelData = 0;
		return false;
	}

	m_pFTData = xnew BYTE[nFTSize];
	if (m_pFTData == NULL)
	{
		m_pPixelData = 0;
		return false;
	}

	memset(m_pFTData, 0, sizeof(BYTE) * nFTSize);

	_FTBmpToFTData_MultiLine();

	return true;
}

void cText::_CreateTexture()
{
	m_pTexture = NULL;

	if (m_pPixelData == NULL)
		return;

	if (m_pFTData == NULL)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	if (m_TextInfo.s_bOutLine == true)
	{
		_StringToData_OutLine();
	}
	else
	{
		_StringToData();
	}

	NiPixelData* pClonePixelData = NiDynamicCast(NiPixelData, m_pPixelData->Clone());
	if (pClonePixelData == NULL)
		return;

	NiTexture::FormatPrefs kPrefs;
	kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
	kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::BINARY;

	m_pTexture = NiSourceTexture::Create(pClonePixelData, kPrefs);
}

void cText::_FTBmpToFTData()
{
#ifdef VERSION_TH
	m_nFTSize_HBase += 128;
#endif

	if (m_pFTData == NULL)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	const TCHAR* str = m_TextInfo.GetText();
	if (str == NULL)
		return;

	int nStrLen = 0;

	__try
	{
		nStrLen = (int)_tcslen(str);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}

	if (nStrLen <= 0)
		return;

	if (nStrLen > (int)TEXT_INPUT_MAX_CHARS)
		nStrLen = (int)TEXT_INPUT_MAX_CHARS;

	FT_Face face = m_TextInfo.GetFace();
	if (!IsFaceReady(face))
		return;

	int nStartSize = 1;

	for (int i = 0; i < nStrLen; ++i)
	{
		if (str[i] == 0x000d || str[i] == 0x000a)
			break;

		FT_ULong charcode = m_bUseMark ? (FT_ULong)m_szMark : (FT_ULong)str[i];

		if (charcode == 0)
			continue;

		int nAdvance = GetCharAdvancePxSafe(face, charcode);

		if (!IsWhiteChar(charcode) && LoadRenderedGlyphSafe(face, charcode))
		{
			if (IsFaceReady(face) && face->glyph != NULL)
			{
				FT_GlyphSlot slot = face->glyph;

				if (slot->bitmap.buffer != NULL &&
					slot->bitmap.width > 0 &&
					slot->bitmap.rows > 0 &&
					slot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
				{
					const int dstX = nStartSize + slot->bitmap_left;
					const int dstY = (m_nFTSize_HBase >> 6) - slot->bitmap_top;

					_ReadFTBmp(
						&slot->bitmap,
						dstX,
						dstY,
						slot->bitmap.width,
						slot->bitmap.rows
					);
				}
			}
		}

		nStartSize += nAdvance;

		if (nStartSize > m_ptStrSize.x + 64)
			break;
	}
}

void cText::_FTBmpToFTData_MultiLine()
{
#ifdef VERSION_TH
	m_nFTSize_HBase += 128;
#endif

	if (m_pFTData == NULL)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	const TCHAR* str = m_TextInfo.GetText();
	if (str == NULL)
		return;

	int nStrLen = 0;

	__try
	{
		nStrLen = (int)_tcslen(str);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}

	if (nStrLen <= 0)
		return;

	if (nStrLen > (int)TEXT_INPUT_MAX_CHARS)
		nStrLen = (int)TEXT_INPUT_MAX_CHARS;

	FT_Face face = m_TextInfo.GetFace();
	if (!IsFaceReady(face))
		return;

	int nStartSize = 1;
	int nStartSizeY = 0;
	const int nLineHeight = m_TextInfo.GetHeight() + m_TextInfo.GetGabHeight();

	for (int i = 0; i < nStrLen; ++i)
	{
		if (str[i] == 0x000d || str[i] == 0x000a)
		{
			nStartSize = 1;
			nStartSizeY += nLineHeight;

			if (nStartSizeY >= m_ptStrSize.y)
				break;

			continue;
		}

		FT_ULong charcode = m_bUseMark ? (FT_ULong)m_szMark : (FT_ULong)str[i];

		if (charcode == 0)
			continue;

		int nAdvance = GetCharAdvancePxSafe(face, charcode);

		if (nStartSize + nAdvance >= m_ptStrSize.x)
		{
			nStartSize = 1;
			nStartSizeY += nLineHeight;

			if (nStartSizeY >= m_ptStrSize.y)
				break;
		}

		if (!IsWhiteChar(charcode) && LoadRenderedGlyphSafe(face, charcode))
		{
			if (IsFaceReady(face) && face->glyph != NULL)
			{
				FT_GlyphSlot slot = face->glyph;

				if (slot->bitmap.buffer != NULL &&
					slot->bitmap.width > 0 &&
					slot->bitmap.rows > 0 &&
					slot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
				{
					const int dstX = nStartSize + slot->bitmap_left;
					const int dstY = nStartSizeY + ((m_nFTSize_HBase >> 6) - slot->bitmap_top);

					_ReadFTBmp(
						&slot->bitmap,
						dstX,
						dstY,
						slot->bitmap.width,
						slot->bitmap.rows
					);
				}
			}
		}

		nStartSize += nAdvance;
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
	if (m_pPixelData == NULL)
		return;

	if (m_pFTData == NULL)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	DWORD* pOrgData = (DWORD*)m_pPixelData->GetPixels();
	if (pOrgData == NULL)
		return;

	const int width = m_ptStrSize.x;
	const int height = m_ptStrSize.y;
	const int totalSize = width * height;

	if (totalSize <= 0)
		return;

	memset(pOrgData, 0, sizeof(DWORD) * totalSize);

	for (int y = 0; y < height - 1; ++y)
	{
		for (int x = 0; x < width - 1; ++x)
		{
			const int srcIndex = y * width + x;

			if (srcIndex < 0 || srcIndex >= totalSize)
				continue;

			const int alpha = m_pFTData[srcIndex];

			if (alpha > 100)
			{
				const int dstIndex = (y + 1) * width + x + 1;

				if (dstIndex < 0 || dstIndex >= totalSize)
					continue;

				pOrgData[dstIndex] = 0xff101010;
			}
		}
	}

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			const int index = y * width + x;

			if (index < 0 || index >= totalSize)
				continue;

			const int alpha = m_pFTData[index];

			if (alpha > 70)
			{
				pOrgData[index] = ((alpha) << 24) | 0x00ffffff;
			}
		}
	}

	m_pPixelData->MarkAsChanged();
}

void cText::_StringToData()
{
	if (m_pPixelData == NULL)
		return;

	if (m_pFTData == NULL)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	DWORD* pData = (DWORD*)m_pPixelData->GetPixels();
	if (pData == NULL)
		return;

	const int width = m_ptStrSize.x;
	const int height = m_ptStrSize.y;
	const int totalSize = width * height;

	if (totalSize <= 0)
		return;

	memset(pData, 0, sizeof(DWORD) * totalSize);

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			const int index = y * width + x;

			if (index < 0 || index >= totalSize)
				continue;

			const int alpha = m_pFTData[index];

			if (alpha > 0)
			{
				pData[index] = ((alpha) << 24) | 0x00ffffff;
			}
		}
	}

	m_pPixelData->MarkAsChanged();
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

bool cText3D::Init3D(cText::sTEXTINFO* pTextInfo)
{
	if (pTextInfo == NULL)
		return false;

	if (pTextInfo->s_pFont == NULL)
		return false;

	if (pTextInfo->s_pFont->IsInitialize() == false)
		return false;

	if (pTextInfo->GetText() == NULL || pTextInfo->GetText()[0] == NULL)
		return false;

	m_TextInfo = *pTextInfo;

	m_pPixelData = 0;
	m_pTexture = 0;
	m_ptStrSize = CsPoint::ZERO;
	m_bUseMark = false;
	SAFE_DELETE_ARRAY(m_pFTData);

	if (_AllocData() == false)
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		m_ptStrSize = CsPoint::ZERO;
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	_CreateTexture();

	if (m_pTexture == NULL)
	{
		m_pPixelData = 0;
		m_ptStrSize = CsPoint::ZERO;
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		m_ptStrSize = CsPoint::ZERO;
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	if (m_pBillBoardText == NULL)
	{
		m_pBillBoardText = NiNew CBillboard;

		if (m_pBillBoardText == NULL)
			return false;

		m_pBillBoardText->CreateTexture(
			m_ptStrSize.x * 0.5f,
			m_ptStrSize.y * 0.5f,
			m_pTexture
		);
	}
	else
	{
		m_pBillBoardText->ChangeTexture(
			m_ptStrSize * 0.5f,
			m_pTexture
		);
	}

	m_pBillBoardText->SetColor(m_TextInfo.s_Color);

	return true;
}

void cText3D::Render(NiPoint3 vPos, float fX, float fY)
{
	if (m_pBillBoardText == NULL)
		return;

	if (m_pTexture == NULL)
		return;

	m_pBillBoardText->Render(vPos, fX, fY);

	float PosX = m_pBillBoardText->GetSizeX();
	float PosY = 0;

	for (int i = 0; i < m_pbBillBoard.Size(); ++i)
	{
		if (m_pbBillBoard.IsExistElement(i))
		{
			if (m_pbBillBoard[i] != NULL)
				m_pbBillBoard[i]->Render(vPos, PosX, PosY);
		}
	}
}

void cText3D::Render(NiPoint3 vPos, float fX, float fY, float fScale)
{
	if (m_pBillBoardText == NULL)
		return;

	if (m_pTexture == NULL)
		return;

	m_pBillBoardText->SetScale(fScale);
	m_pBillBoardText->Render(vPos, fX, fY);

	float PosX = m_pBillBoardText->GetSizeX();
	float PosY = 0;

	for (int i = 0; i < m_pbBillBoard.Size(); ++i)
	{
		if (m_pbBillBoard.IsExistElement(i))
		{
			if (m_pbBillBoard[i] != NULL)
				m_pbBillBoard[i]->Render(vPos, PosX, PosY, fScale);
		}
	}
}

bool cText3D::SetText( int nStr )
{
	TCHAR szNum[ 16 ];
	_stprintf_s( szNum, 16, _T( "%d" ), nStr );
	return SetText( szNum );
}

bool cText3D::SetText(TCHAR const* szStr)
{
	TextPerfCount("cText3D::SetText");

	if (szStr == NULL)
		return false;

	if (m_TextInfo.s_pFont == NULL)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	if (szStr[0] == NULL)
		return false;

	if (_tcscmp(szStr, m_TextInfo.GetText()) == 0)
		return false;

	// Guarda o estado anterior para não deixar o billboard inválido
	// se a nova textura falhar.
	NiSourceTexturePtr pOldTexture = m_pTexture;
	CsPoint ptOldSize = m_ptStrSize;

	BYTE* pOldFTData = m_pFTData;
	m_pFTData = NULL;

	m_TextInfo.SetText(szStr);

	m_pPixelData = 0;
	m_pTexture = 0;
	m_ptStrSize = CsPoint::ZERO;

	if (_AllocData() == false)
	{
		SAFE_DELETE_ARRAY(m_pFTData);

		m_pFTData = pOldFTData;
		m_pTexture = pOldTexture;
		m_ptStrSize = ptOldSize;

		return false;
	}

	_CreateTexture();

	if (m_pTexture == NULL || m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
	{
		SAFE_DELETE_ARRAY(m_pFTData);

		m_pFTData = pOldFTData;
		m_pTexture = pOldTexture;
		m_ptStrSize = ptOldSize;

		return false;
	}

	SAFE_DELETE_ARRAY(pOldFTData);

	if (m_pBillBoardText == NULL)
	{
		m_pBillBoardText = NiNew CBillboard;

		if (m_pBillBoardText == NULL)
			return false;

		m_pBillBoardText->CreateTexture(
			m_ptStrSize.x * 0.5f,
			m_ptStrSize.y * 0.5f,
			m_pTexture
		);

		m_pBillBoardText->SetColor(m_TextInfo.s_Color);
	}
	else
	{
		m_pBillBoardText->ChangeTexture(
			m_ptStrSize * 0.5f,
			m_pTexture
		);
	}

	return true;
}

void cText3D::SetAlpha(float fAlpha)
{
	if (m_pBillBoardText)
	{
		m_pBillBoardText->SetAlpha(fAlpha);
	}

	for (int i = 0; i < m_pbBillBoard.Size(); ++i)
	{
		if (m_pbBillBoard.IsExistElement(i))
		{
			if (m_pbBillBoard[i] != NULL && m_pbBillBoard[i]->s_pBillboard != NULL)
				m_pbBillBoard[i]->s_pBillboard->SetAlpha(fAlpha);
		}
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

bool cMyText::GetStringSize(CsPoint& size, int& hBase, TCHAR* szStr)
{
	size = CsPoint::ZERO;
	hBase = 0;

	if (szStr == NULL)
		return false;

	if (m_TextInfo.s_pFont == NULL)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	FT_Face face = m_TextInfo.GetFace();
	if (!IsFaceReady(face))
		return false;

	int nLen = 0;

	__try
	{
		nLen = (int)_tcslen(szStr);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return false;
	}

	if (nLen <= 0)
		return false;

	if (nLen > (int)TEXT_INPUT_MAX_CHARS)
		nLen = (int)TEXT_INPUT_MAX_CHARS;

	int x = 0;
	int y = m_TextInfo.GetHeight();
	int fx = 0;

	if (y <= 0 || y > TEXT_TEXTURE_MAX_HEIGHT)
		return false;

	for (int i = 0; i < nLen; ++i)
	{
		if (szStr[i] == 0x000d)
			continue;

		if (szStr[i] == 0x000a)
		{
			x = 0;
			y += m_TextInfo.GetHeight();

			if (y > TEXT_TEXTURE_MAX_HEIGHT)
				break;

			continue;
		}

		FT_ULong charcode = m_bUseMark ? (FT_ULong)m_szMark : (FT_ULong)szStr[i];
		int adv = GetCharAdvancePxSafe(face, charcode);

		x += adv;

		if (m_bSet)
		{
			if ((m_ptPixelSize.x - 2) >= x)
			{
				if (fx < x)
					fx = x;
			}
			else
			{
				x = adv;
				y += m_TextInfo.GetHeight();

				if (y > TEXT_TEXTURE_MAX_HEIGHT)
					break;
			}
		}

		if (x > TEXT_TEXTURE_MAX_WIDTH)
			break;
	}

	if (m_bSet)
		size.x = fx + 2;
	else
		size.x = x + 2;

	size.y = y;
	hBase = (int)(m_TextInfo.GetHeight() * 0.75f) << 6;

	if (!IsSafeTextTextureSize(size))
	{
		size = CsPoint::ZERO;
		hBase = 0;
		return false;
	}

	return size.x > 2;
}

int cMyText::GetCharWidth(FT_Face face, FT_ULong charcode)
{
	return GetCharAdvancePxSafe(face, charcode);
}

bool cMyText::SetText( int nStr )
{
	TCHAR szNum[ 16 ];
	_stprintf_s( szNum, 16, _T( "%d" ), nStr );
	return SetText( szNum );
}

bool cMyText::SetText(TCHAR* szStr /*부하가크다*/, bool bSet)
{
	TextPerfCount("cMyText::SetText");

	if (m_TextInfo.s_pFont == NULL)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	cText::sTEXTINFO safeInfo;
	safeInfo.SetText(szStr);
	std::wstring safeInput = safeInfo.s_strText;

	if (safeInput.empty())
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		m_ptStrSize = CsPoint::ZERO;
		m_TextInfo.SetText(_T(""));
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	if (safeInput == m_TextInfo.GetText())
		return false;

	m_TextInfo.SetText(const_cast<TCHAR*>(safeInput.c_str()));

	m_pPixelData = 0;
	m_pTexture = 0;
	SAFE_DELETE_ARRAY(m_pFTData);

	m_bSet = bSet;

	if (_AllocData() == false)
	{
		m_pPixelData = 0;
		m_pTexture = 0;
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

	_CreateTexture();

	if (m_pTexture == NULL)
	{
		m_pPixelData = 0;
		m_ptStrSize = CsPoint::ZERO;
		SAFE_DELETE_ARRAY(m_pFTData);
		return false;
	}

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
	if (GetStringSize(m_ptStrSize, m_nFTSize_HBase, (TCHAR*)m_TextInfo.GetText()) == false)
		return false;

	if (!IsSafeTextTextureSize(m_ptStrSize))
	{
		m_ptStrSize = CsPoint::ZERO;
		return false;
	}

	if (m_ptPixelSize.x <= 0 || m_ptPixelSize.y <= 0)
	{
		m_ptPixelSize = m_ptStrSize;
	}

	if (m_ptStrSize.x > m_ptPixelSize.x)
		m_ptPixelSize.x = m_ptStrSize.x;

	if (m_ptStrSize.y > m_ptPixelSize.y)
		m_ptPixelSize.y = m_ptStrSize.y;

	if (!IsSafeTextTextureSize(m_ptPixelSize))
	{
		m_ptPixelSize.x = CsMin(m_ptPixelSize.x, TEXT_TEXTURE_MAX_WIDTH);
		m_ptPixelSize.y = CsMin(m_ptPixelSize.y, TEXT_TEXTURE_MAX_HEIGHT);

		if (!IsSafeTextTextureSize(m_ptPixelSize))
			return false;
	}

	if (m_pPixelData != NULL)
	{
		m_pPixelData = 0;
	}

	if (m_pFTData != NULL)
	{
		SAFE_DELETE_ARRAY(m_pFTData);
	}

	m_pPixelData = NiNew NiPixelData(m_ptPixelSize.x, m_ptPixelSize.y, NiPixelFormat::RGBA32);
	if (m_pPixelData == NULL)
		return false;

	const int pixels = m_ptStrSize.x * m_ptStrSize.y;
	if (pixels <= 0 || pixels > TEXT_TEXTURE_MAX_PIXELS)
	{
		m_pPixelData = 0;
		return false;
	}

	m_pFTData = xnew BYTE[pixels];
	if (m_pFTData == NULL)
	{
		m_pPixelData = 0;
		return false;
	}

	memset(m_pFTData, 0, sizeof(BYTE) * pixels);

	_FTBmpToFTData();

	return true;
}

//2  m_pFTData 에다 데이터를 넣는것 같은데..글자수 만큼
void cMyText::_FTBmpToFTData()
{
	if (m_pFTData == NULL)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	if (m_ptPixelSize.x <= 0 || m_ptPixelSize.y <= 0)
		return;

	const TCHAR* str = m_TextInfo.GetText();
	if (str == NULL)
		return;

	int nStrLen = 0;

	__try
	{
		nStrLen = (int)_tcslen(str);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return;
	}

	if (nStrLen <= 0)
		return;

	if (nStrLen > (int)TEXT_INPUT_MAX_CHARS)
		nStrLen = (int)TEXT_INPUT_MAX_CHARS;

	FT_Face face = m_TextInfo.GetFace();
	if (!IsFaceReady(face))
		return;

	int nStartSize = 1;
	int nStartSizeY = 0;
	int x = 0;
	const int lineHeight = m_TextInfo.GetHeight();

	for (int i = 0; i < nStrLen; ++i)
	{
		if (str[i] == 0x000d || str[i] == 0x000a)
		{
			nStartSize = 1;
			nStartSizeY += lineHeight;
			x = 0;

			if (nStartSizeY >= m_ptStrSize.y)
				break;

			continue;
		}

		FT_ULong charcode = m_bUseMark ? (FT_ULong)m_szMark : (FT_ULong)str[i];

		if (charcode == 0)
			continue;

		int nAdvance = GetCharAdvancePxSafe(face, charcode);

		if ((m_ptPixelSize.x - 2) < (x + nAdvance))
		{
			nStartSize = 1;
			nStartSizeY += lineHeight;
			x = 0;

			if (nStartSizeY >= m_ptStrSize.y)
				break;
		}

		if (!IsWhiteChar(charcode) && LoadRenderedGlyphSafe(face, charcode))
		{
			if (IsFaceReady(face) && face->glyph != NULL)
			{
				FT_GlyphSlot slot = face->glyph;

				if (slot->bitmap.buffer != NULL &&
					slot->bitmap.width > 0 &&
					slot->bitmap.rows > 0 &&
					slot->bitmap.pixel_mode == FT_PIXEL_MODE_GRAY)
				{
					const int dstX = nStartSize + slot->bitmap_left;
					const int dstY = nStartSizeY + ((m_nFTSize_HBase >> 6) - slot->bitmap_top);

					_ReadFTBmp(
						&slot->bitmap,
						dstX,
						dstY,
						slot->bitmap.width,
						slot->bitmap.rows
					);
				}
			}
		}

		nStartSize += nAdvance;
		x += nAdvance;

		if (nStartSize > m_ptStrSize.x + 64)
			break;
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
	m_pTexture = NULL;

	if (m_pPixelData == NULL)
		return;

	if (m_pFTData == NULL)
		return;

	if (m_ptPixelSize.x <= 0 || m_ptPixelSize.y <= 0)
		return;

	if (m_TextInfo.s_bOutLine == true)
	{
		_StringToData_OutLine();
	}
	else
	{
		_StringToData();
	}

	NiPixelData* pClonePixelData = NiDynamicCast(NiPixelData, m_pPixelData->Clone());
	if (pClonePixelData == NULL)
		return;

	NiTexture::FormatPrefs kPrefs;
	kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
	kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;

	m_pTexture = NiSourceTexture::Create(pClonePixelData, kPrefs);
}


// 아웃라인 픽셀을 찾아서 색을 입힌것같음.
void cMyText::_StringToData_OutLine()
{
	if (m_pPixelData == NULL)
		return;

	if (m_pFTData == NULL)
		return;

	if (m_ptPixelSize.x <= 0 || m_ptPixelSize.y <= 0)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	DWORD* pOrgData = (DWORD*)m_pPixelData->GetPixels();
	if (pOrgData == NULL)
		return;

	const int pixelWidth = m_ptPixelSize.x;
	const int pixelHeight = m_ptPixelSize.y;
	const int pixelTotal = pixelWidth * pixelHeight;

	const int textWidth = m_ptStrSize.x;
	const int textHeight = m_ptStrSize.y;
	const int textTotal = textWidth * textHeight;

	if (pixelTotal <= 0 || textTotal <= 0)
		return;

	memset(pOrgData, 0, sizeof(DWORD) * pixelTotal);

	for (int y = 0; y < pixelHeight; ++y)
	{
		for (int x = 0; x < pixelWidth; ++x)
		{
			const int dstIndex = y * pixelWidth + x;

			if (dstIndex < 0 || dstIndex >= pixelTotal)
				continue;

			pOrgData[dstIndex] = 0xff6ad2c8;
		}
	}

	int offsetX = 0;

	if (m_TextInfo.s_eTextAlign == DT_CENTER)
	{
		offsetX = (pixelWidth - textWidth) / 2;

		if (offsetX < 0)
			offsetX = 0;
	}

	for (int y = 0; y < textHeight - 1; ++y)
	{
		if (y + 1 >= pixelHeight)
			break;

		for (int x = 0; x < textWidth - 1; ++x)
		{
			const int srcIndex = y * textWidth + x;

			if (srcIndex < 0 || srcIndex >= textTotal)
				continue;

			const int alpha = m_pFTData[srcIndex];

			if (alpha > 100)
			{
				const int dstX = x + 1 + offsetX;
				const int dstY = y + 1;

				if (dstX < 0 || dstX >= pixelWidth)
					continue;

				if (dstY < 0 || dstY >= pixelHeight)
					continue;

				const int dstIndex = dstY * pixelWidth + dstX;

				if (dstIndex < 0 || dstIndex >= pixelTotal)
					continue;

				pOrgData[dstIndex] = 0xff101010;
			}
		}
	}

	for (int y = 0; y < textHeight; ++y)
	{
		if (y >= pixelHeight)
			break;

		for (int x = 0; x < textWidth; ++x)
		{
			const int srcIndex = y * textWidth + x;

			if (srcIndex < 0 || srcIndex >= textTotal)
				continue;

			const int alpha = m_pFTData[srcIndex];

			if (alpha > 70)
			{
				const int dstX = x + offsetX;
				const int dstY = y;

				if (dstX < 0 || dstX >= pixelWidth)
					continue;

				if (dstY < 0 || dstY >= pixelHeight)
					continue;

				const int dstIndex = dstY * pixelWidth + dstX;

				if (dstIndex < 0 || dstIndex >= pixelTotal)
					continue;

				pOrgData[dstIndex] = ((alpha) << 24) | 0x00ffffff;
			}
		}
	}

	m_pPixelData->MarkAsChanged();
}

void cMyText::_StringToData()
{
	if (m_pPixelData == NULL)
		return;

	if (m_pFTData == NULL)
		return;

	if (m_ptPixelSize.x <= 0 || m_ptPixelSize.y <= 0)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	DWORD* pData = (DWORD*)m_pPixelData->GetPixels();
	if (pData == NULL)
		return;

	const int pixelWidth = m_ptPixelSize.x;
	const int pixelHeight = m_ptPixelSize.y;
	const int pixelTotal = pixelWidth * pixelHeight;

	const int textWidth = m_ptStrSize.x;
	const int textHeight = m_ptStrSize.y;
	const int textTotal = textWidth * textHeight;

	if (pixelTotal <= 0 || textTotal <= 0)
		return;

	memset(pData, 0, sizeof(DWORD) * pixelTotal);

	int offsetX = 0;

	if (m_TextInfo.s_eTextAlign == DT_CENTER)
	{
		offsetX = (pixelWidth - textWidth) / 2;

		if (offsetX < 0)
			offsetX = 0;
	}

	for (int y = 0; y < textHeight; ++y)
	{
		if (y >= pixelHeight)
			break;

		for (int x = 0; x < textWidth; ++x)
		{
			const int srcIndex = y * textWidth + x;

			if (srcIndex < 0 || srcIndex >= textTotal)
				continue;

			const int alpha = m_pFTData[srcIndex];

			if (alpha > 70)
			{
				const int dstX = x + offsetX;
				const int dstY = y;

				if (dstX < 0 || dstX >= pixelWidth)
					continue;

				if (dstY < 0 || dstY >= pixelHeight)
					continue;

				const int dstIndex = dstY * pixelWidth + dstX;

				if (dstIndex < 0 || dstIndex >= pixelTotal)
					continue;

				pData[dstIndex] = ((alpha) << 24) | 0x00ffffff;
			}
		}
	}

	m_pPixelData->MarkAsChanged();
}