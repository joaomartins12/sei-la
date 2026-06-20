
#include "stdafx.h"
#include "Text.h"

namespace
{
	static const size_t TEXT_INPUT_MAX_CHARS = 4096;

	static int ClampInt(int v, int minv, int maxv)
	{
		if (v < minv) return minv;
		if (v > maxv) return maxv;
		return v;
	}

	static int FallbackCharWidthFromHeight(int height, wchar_t ch)
	{
		if (height <= 0)
			height = 12;

		if (ch == L'\r')
			return 0;

		if (ch == L'\n')
			return 0;

		if (ch == L'\t')
			return height * 2;

		if (ch == L' ')
			return CsMax(3, height / 3);

		// CJK/Hangul/Thai e caracteres largos.
		if (ch >= 0x2E80)
			return CsMax(8, height);

		// Letras estreitas.
		if (ch == L'i' || ch == L'I' || ch == L'l' || ch == L'!' || ch == L'.' || ch == L',' || ch == L':' || ch == L';' || ch == L'|' || ch == L'\'')
			return CsMax(3, height / 3);

		// Letras largas.
		if (ch == L'W' || ch == L'M' || ch == L'w' || ch == L'm')
			return CsMax(7, (height * 3) / 4);

		return CsMax(6, (height * 55) / 100);
	}

	static int FallbackStringWidth(const wchar_t* text, int height)
	{
		if (text == NULL)
			return 0;

		int width = 0;
		int maxWidth = 0;

		for (const wchar_t* p = text; *p; ++p)
		{
			if (*p == L'\n' || *p == L'\r')
			{
				if (width > maxWidth)
					maxWidth = width;
				width = 0;
				continue;
			}

			width += FallbackCharWidthFromHeight(height, *p);
		}

		if (width > maxWidth)
			maxWidth = width;

		return maxWidth;
	}

	static void BuildMarkedText(std::wstring& outText, const wchar_t* srcText, wchar_t mark)
	{
		outText.clear();

		if (srcText == NULL)
			return;

		for (const wchar_t* p = srcText; *p; ++p)
		{
			if (*p == L'\r' || *p == L'\n')
				outText += *p;
			else
				outText += mark;
		}
	}

	static HFONT CreateFallbackFont(int fontHeight, bool bold)
	{
		if (fontHeight <= 0)
			fontHeight = 12;

		return CreateFontW(
			-fontHeight,
			0,
			0,
			0,
			bold ? FW_BOLD : FW_NORMAL,
			FALSE,
			FALSE,
			FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			L"Tahoma"
		);
	}

	static bool RenderTextToAlphaBuffer(const wchar_t* text, int width, int height, int fontHeight, bool bold, bool multiLine, BYTE* outAlpha)
	{
		if (text == NULL)
			return false;

		if (outAlpha == NULL)
			return false;

		if (width <= 0 || height <= 0)
			return false;

		memset(outAlpha, 0, sizeof(BYTE) * width * height);

		BITMAPINFO bmi;
		ZeroMemory(&bmi, sizeof(bmi));
		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB;

		void* pBits = NULL;
		HDC hdc = CreateCompatibleDC(NULL);
		if (hdc == NULL)
			return false;

		HBITMAP hBmp = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
		if (hBmp == NULL || pBits == NULL)
		{
			DeleteDC(hdc);
			return false;
		}

		HGDIOBJ hOldBmp = SelectObject(hdc, hBmp);
		HFONT hFont = CreateFallbackFont(fontHeight, bold);
		HGDIOBJ hOldFont = NULL;

		if (hFont)
			hOldFont = SelectObject(hdc, hFont);

		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = width;
		rc.bottom = height;

		HBRUSH hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
		FillRect(hdc, &rc, hBrush);

		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(255, 255, 255));

		UINT flags = DT_LEFT | DT_TOP | DT_NOPREFIX;
		if (multiLine)
			flags |= DT_WORDBREAK;
		else
			flags |= DT_SINGLELINE;

		DrawTextW(hdc, text, -1, &rc, flags);

		DWORD* pixels = (DWORD*)pBits;
		int total = width * height;

		for (int i = 0; i < total; ++i)
		{
			DWORD c = pixels[i];
			BYTE b = (BYTE)(c & 0xff);
			BYTE g = (BYTE)((c >> 8) & 0xff);
			BYTE r = (BYTE)((c >> 16) & 0xff);
			outAlpha[i] = (BYTE)CsMin(255, ((int)r + (int)g + (int)b) / 3);
		}

		if (hOldFont)
			SelectObject(hdc, hOldFont);

		if (hFont)
			DeleteObject(hFont);

		SelectObject(hdc, hOldBmp);
		DeleteObject(hBmp);
		DeleteDC(hdc);

		return true;
	}

	static bool IsFaceReady(FT_Face face)
	{
		return (face != NULL && face->glyph != NULL && face->size != NULL);
	}

	static bool LoadGlyphSafe(FT_Face face, FT_ULong charcode, FT_Int32 flags)
	{
		// Sistema antigo mantido só por compatibilidade com chamadas que ainda consultem largura.
		// A renderização real foi movida para GDI em _FTBmpToFTData para evitar crashes do FreeType.
		if (!IsFaceReady(face))
			return false;

		FT_UInt glyphIndex = FT_Get_Char_Index(face, charcode);
		if (glyphIndex == 0)
			return false;

		return FT_Load_Glyph(face, glyphIndex, FT_LOAD_DEFAULT | FT_LOAD_NO_AUTOHINT) == 0;
	}

	static bool RenderGlyphSafe(FT_Face face)
	{
		// Não chamar FT_Render_Glyph nesta build.
		// Ele estava a lançar 0x80000026 long jump dentro da FreeType.
		return false;
	}
}


// A partir daqui, qualquer chamada direta antiga neste ficheiro passa automaticamente
// pela versão protegida. Assim não precisas substituir manualmente todas as chamadas.


cText::sTEXTINFO::sTEXTINFO() :
	s_pFont(NULL), s_eFontSize(CFont::FS_10), s_Color(FONT_WHITE),
	s_bOutLine(true), s_eBoldLevel(BL_NONE), s_eTextAlign(DT_LEFT),
	s_nLineGabHeight(0)
{
}

cText::sTEXTINFO::~sTEXTINFO()
{
	s_pFont = NULL;
}

void cText::sTEXTINFO::Init(CFont::eFACE_SIZE eFontSize /*=CFont::FS_10*/, NiColor color /*=FONT_WHITE*/)
{
	Init(&g_pEngine->m_FontSystem, eFontSize, color);
}

void cText::sTEXTINFO::Init(CFont* pFont, CFont::eFACE_SIZE eFontSize /*=CFont::FS_10*/, NiColor color /*=FONT_WHITE*/)
{
	s_pFont = pFont;
	s_eFontSize = eFontSize;
	s_Color = color;

	s_bOutLine = true;
	s_eBoldLevel = BL_NONE;

	s_eTextAlign = DT_LEFT;
}

void cText::sTEXTINFO::Init(CsHelp::sTEXT* pHelpText)
{
	Init(FONT_SIZE_CAL(pHelpText->s_nTextSize), NiColor(pHelpText->s_Color[0] / 255.0f, pHelpText->s_Color[1] / 255.0f, pHelpText->s_Color[2] / 255.0f));
	SetBoldLevel((cText::sTEXTINFO::eBOLD_LEVEL)pHelpText->s_nBold);
}

int cText::sTEXTINFO::GetBoldSize() const
{
	switch (s_eBoldLevel)
	{
	case BL_NONE:	return 0;
	case BL_1:		return 4;
	case BL_2:		return 5;
	case BL_3:		return 6;
	case BL_4:		return 7;
	case BL_5:		return 8;
	}
	assert_csm1(false, _T("BoldLevel = %d"), s_eBoldLevel);
	return 0;
}

void cText::sTEXTINFO::SetText_Reduce(TCHAR const* szText, int nLen, int nDotNum /* = 2 */)
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

cText::sTEXTINFO& cText::sTEXTINFO::AddString(TCHAR const* szText)
{
	s_strText += szText;
	return *this;
}

cText::sTEXTINFO& cText::sTEXTINFO::SetString(TCHAR const* szText)
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
	return s_pFont->GetFace(s_eFontSize);
}

int	cText::sTEXTINFO::GetHeight() const
{
	return s_pFont->GetHeight(s_eFontSize);
}

int	cText::sTEXTINFO::GetHBase() const
{
	return s_pFont->GetHBase(s_eFontSize);
}

int cText::sTEXTINFO::GetGabHeight() const
{
	return s_nLineGabHeight;
}

void cText::sTEXTINFO::SetLineGabHeight(int const& nHeight)
{
	s_nLineGabHeight = nHeight;
}

void cText::sTEXTINFO::SetText(TCHAR const* szText)
{
	if (szText == NULL)
	{
		s_strText.clear();
		return;
	}

	__try
	{
		s_strText = szText;
		if (s_strText.length() > TEXT_INPUT_MAX_CHARS)
			s_strText.resize(TEXT_INPUT_MAX_CHARS);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		s_strText.clear();
	}
}

void cText::sTEXTINFO::SetText(int nText)
{
	TCHAR sz[16] = { 0, };
	_stprintf_s(sz, 16, _T("%d"), nText);
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

void cText::sTEXTINFO::AddText(TCHAR const* szTex)
{
	if (szTex == NULL)
		return;

	__try
	{
		s_strText.append(szTex);
		if (s_strText.length() > TEXT_INPUT_MAX_CHARS)
			s_strText.resize(TEXT_INPUT_MAX_CHARS);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}
}

void cText::sTEXTINFO::SetBoldLevel(eBOLD_LEVEL Level)
{
	s_eBoldLevel = Level;
}

cText::sTEXTINFO::eBOLD_LEVEL cText::sTEXTINFO::GetBoldLevel() const
{
	return s_eBoldLevel;
}

int cText::sTEXTINFO::GetTextWidth()
{
	FT_Face face = g_pEngine->m_FontSystem.GetFace(s_eFontSize);

	int nWidth = 0;
	for (int i = 0; i < s_strText.length(); ++i)
		nWidth += GetCharWidth(face, s_strText[i]);

	return nWidth;
}

//=======================================================================================
//
//		베이스
//
//=======================================================================================

cText::cText() :m_pTexture(NULL), m_pPixelData(NULL), m_pFTData(NULL), m_bUseMark(false)
{
}

void cText::Delete()
{
	m_Sprite.Delete();

	m_pPixelData = 0;
	m_pTexture = 0;
	SAFE_DELETE_ARRAY(m_pFTData);
}

void cText::Init(cWindow* pParent, CsPoint pos, sTEXTINFO* pTextInfo, bool bApplyWindowSize)
{
	if (pTextInfo->s_pFont == NULL)
		return;

	if (pTextInfo->s_pFont->IsInitialize() == false)
		return;

	m_TextInfo = *pTextInfo;
	// 값 초기화
	m_ptStrSize = CsPoint::ZERO;

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if (_AllocData() == false)
	{
		m_Sprite.Init(pParent, pos, m_ptStrSize, bApplyWindowSize, m_TextInfo.s_Color);
		return;
	}

	// 텍스쳐 생성
	_CreateTexture();
	m_Sprite.Init(pParent, pos, m_ptStrSize, m_pTexture, bApplyWindowSize, m_TextInfo.s_Color);
}

void cText::InitStencil(cWindow* pParent, CsPoint pos, sTEXTINFO* pTextInfo, bool bApplyWindowSize, NiStencilProperty* pPropStencil)
{
	if (pTextInfo->s_pFont->IsInitialize() == false)
	{
		return;
	}

	m_TextInfo = *pTextInfo;
	// 값 초기화
	m_ptStrSize = CsPoint::ZERO;

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if (_AllocData() == false)
	{
		m_Sprite.InitStencil(pParent, pos, m_ptStrSize, bApplyWindowSize, pPropStencil, m_TextInfo.s_Color);
		return;
	}

	// 텍스쳐 생성
	_CreateTexture();
	m_Sprite.InitStencil(pParent, pos, m_ptStrSize, m_pTexture, bApplyWindowSize, pPropStencil, m_TextInfo.s_Color);
}

void cText::SetPos(CsPoint ptPos)
{
	m_Sprite.SetPos(ptPos);
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

void cText::Render(CsPoint pos, UINT Align)
{
	if (m_pTexture == NULL)
		return;

	if (Align & DT_CENTER)
		pos.x -= (int)(m_ptStrSize.x * 0.5f);
	else if (Align & DT_RIGHT)
		pos.x -= m_ptStrSize.x;

	if (Align & DT_VCENTER)
		pos.y -= (int)(m_ptStrSize.y * 0.5f);
	else if (Align & DT_BOTTOM)
		pos.y -= m_ptStrSize.y;

	m_Sprite.Render(pos);
}

void cText::Render(CsPoint pos)
{
	if (m_pTexture == NULL)
		return;

	Render(pos, m_TextInfo.s_eTextAlign);
}

void cText::Render()
{
	if (m_pTexture == NULL)
		return;

	Render(CsPoint::ZERO, m_TextInfo.s_eTextAlign);
}

void cText::RenderLimit(CsPoint pos, UINT Align)
{
	if (m_pTexture == NULL)
		return;

	if (Align & DT_CENTER)
		pos.x -= (int)(m_ptStrSize.x * 0.5f);
	else if (Align & DT_RIGHT)
		pos.x -= m_ptStrSize.x;

	if (Align & DT_VCENTER)
		pos.y -= (int)(m_ptStrSize.y * 0.5f);
	else if (Align & DT_BOTTOM)
		pos.y -= m_ptStrSize.y;

	m_Sprite.RenderLimit(pos);
}

void cText::RenderLimit(CsPoint pos)
{
	if (m_pTexture == NULL)
		return;

	m_Sprite.RenderLimit(pos);
}

void cText::SetVisible(bool bValue)
{
	m_Sprite.SetVisible(bValue);
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
	if (m_TextInfo.s_pFont == NULL)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	if (szStr == NULL)
		return false;

	int fontHeight = m_TextInfo.GetHeight();
	if (fontHeight <= 0)
		fontHeight = 12;

	int limitWidth = nWidth;
	if (limitWidth <= 0)
		limitWidth = g_nScreenWidth > 0 ? g_nScreenWidth : 1024;

	int x = 0;
	int maxWidth = 0;
	int totalHeight = fontHeight;

	wsResult.clear();

	int nLen = (int)_tcslen(szStr);
	for (int i = 0; i < nLen; ++i)
	{
		wchar_t ch = szStr[i];

		if (ch == L'\r')
			continue;

		if (ch == L'\n')
		{
			wsResult += ch;
			if (x > maxWidth)
				maxWidth = x;

			x = 0;
			totalHeight += fontHeight + m_TextInfo.GetGabHeight();
			continue;
		}

		int cw = FallbackCharWidthFromHeight(fontHeight, ch);

		if (x > 0 && x + cw > limitWidth)
		{
			wsResult += L'\n';

			if (x > maxWidth)
				maxWidth = x;

			x = 0;
			totalHeight += fontHeight + m_TextInfo.GetGabHeight();
		}

		wsResult += ch;
		x += cw;
	}

	if (x > maxWidth)
		maxWidth = x;

	size.x = maxWidth + 4;
	size.y = totalHeight;
	hBase = (int)(fontHeight * 0.75f) << 6;

	return (size.x > 2 && size.y > 0);
}

bool cText::GetStringSize(CsPoint& size, int& hBase, TCHAR* szStr)
{
	if (m_TextInfo.s_pFont == NULL)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	if (szStr == NULL)
		return false;

	int fontHeight = m_TextInfo.GetHeight();
	if (fontHeight <= 0)
		fontHeight = 12;

	int width = FallbackStringWidth(szStr, fontHeight);

	size.x = width + 4;
	size.y = fontHeight;
	hBase = (int)(fontHeight * 0.75f) << 6;

	return (size.x > 2 && size.y > 0);
}

void cText::GetStringSize(sTEXTINFO* TextInfo, CsPoint& size, int& hBase, const TCHAR* szStr)
{
	if (TextInfo == NULL || TextInfo->s_pFont == NULL)
		return;

	if (TextInfo->s_pFont->IsInitialize() == false)
		return;

	if (szStr == NULL)
		return;

	int fontHeight = TextInfo->GetHeight();
	if (fontHeight <= 0)
		fontHeight = 12;

	int width = FallbackStringWidth(szStr, fontHeight);

	size.x = width + 4;
	size.y = fontHeight;
	hBase = (int)(fontHeight * 0.75f) << 6;
}

int cText::GetCharWidth(FT_Face face, FT_ULong charcode)
{
	return FallbackCharWidthFromHeight(12, (wchar_t)charcode);
}
// [4/21/2016 hyun] 문자열의 넓이를 가져온다
int cText::GetStringWidth(FT_Face face, const std::wstring& str)
{
	int nWidth = 0;

	for (int i = 0; i < str.length(); ++i)
		nWidth += GetCharWidth(face, str[i]);

	return nWidth;
}
// @@ 여기까지
bool cText::SetText(int nStr)
{
	TCHAR szNum[16];
	_stprintf_s(szNum, 16, _T("%d"), nStr);
	return SetText(szNum);
}

bool cText::SetText(TCHAR const* szStr /*부하가크다*/)
{
	if (NULL == m_TextInfo.s_pFont)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
	{
		return false;
	}

	cText::sTEXTINFO safeInfo;
	safeInfo.SetText(szStr);
	std::wstring safeInput = safeInfo.s_strText;

	if (safeInput.empty())
	{
		if (m_pTexture)
		{
			m_pPixelData = 0;
			m_pTexture = 0;
			m_ptStrSize = CsPoint::ZERO;
			m_TextInfo.SetText(_T(""));
			return true;
		}
		return false;
	}

	// 같은 글자라면 패스
	if (safeInput == m_TextInfo.GetText())
		return false;

	m_TextInfo.SetText(safeInput.c_str());

	// 지우고 새로 생성
	m_pPixelData = 0;
	m_pTexture = 0;
	SAFE_DELETE_ARRAY(m_pFTData);

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if (_AllocData() == false)
	{
		return false;
	}
	// 텍스쳐 생성
	_CreateTexture();

	m_Sprite.ChangeTexture(m_ptStrSize, m_pTexture);
	m_Sprite.SetColor(m_TextInfo.s_Color);

	return true;
}

bool cText::SetText(TCHAR const* szStr /*부하가크다*/, int nWidth)
{
	if (NULL == m_TextInfo.s_pFont)
		return false;

	if (!m_TextInfo.s_pFont->IsInitialize())
		return false;

	cText::sTEXTINFO safeInfo;
	safeInfo.SetText(szStr);
	std::wstring safeInput = safeInfo.s_strText;

	if (safeInput.empty())
	{
		if (m_pTexture)
		{
			m_pPixelData = 0;
			m_pTexture = 0;
			m_ptStrSize = CsPoint::ZERO;
			m_TextInfo.SetText(_T(""));
			return true;
		}
		return false;
	}

	// 같은 글자라면 패스
	if (safeInput == m_TextInfo.GetText())
		return false;

	m_TextInfo.SetText(safeInput.c_str());

	// 지우고 새로 생성
	m_pPixelData = 0;
	m_pTexture = 0;
	SAFE_DELETE_ARRAY(m_pFTData);

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if (!_AllocData(nWidth))
		return false;

	// 텍스쳐 생성
	_CreateTexture();

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

	assert_cs((m_ptStrSize.x != 0) && (m_ptStrSize.y != 0));

	assert_cs(m_pPixelData == NULL);
	m_pPixelData = NiNew NiPixelData(m_ptStrSize.x, m_ptStrSize.y, NiPixelFormat::RGBA32);

	assert_cs(m_pFTData == NULL);
	int nFTSize = m_ptStrSize.Mul() * 4;
	m_pFTData = xnew BYTE[nFTSize];
	memset(m_pFTData, 0, sizeof(BYTE) * nFTSize);

	_FTBmpToFTData();

	return true;
}

bool cText::_AllocData(int nWidth)
{
	std::wstring result;
	if (!GetStringSize(m_ptStrSize, m_nFTSize_HBase, (TCHAR*)m_TextInfo.GetText(), result, nWidth))
		return false;

	assert_cs((m_ptStrSize.x != 0) && (m_ptStrSize.y != 0));

	m_TextInfo.SetText(result.c_str());
	assert_cs(m_pPixelData == NULL);
	m_pPixelData = NiNew NiPixelData(m_ptStrSize.x, m_ptStrSize.y, NiPixelFormat::RGBA32);

	assert_cs(m_pFTData == NULL);
	int nFTSize = m_ptStrSize.Mul() * 4;
	m_pFTData = xnew BYTE[nFTSize];
	memset(m_pFTData, 0, sizeof(BYTE) * nFTSize);

	_FTBmpToFTData_MultiLine();

	return true;
}

void cText::_CreateTexture()
{
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

	if (m_pPixelData == NULL)
		return;

	NiTexture::FormatPrefs kPrefs;
	kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
	kPrefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
	kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::BINARY;

	m_pTexture = NiSourceTexture::Create(NiDynamicCast(NiPixelData, m_pPixelData->Clone()), kPrefs);
}

void cText::_FTBmpToFTData()
{
	if (m_pFTData == NULL)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	const TCHAR* str = m_TextInfo.GetText();
	if (str == NULL)
		return;

	std::wstring drawText;

	if (m_bUseMark == false)
	{
		drawText = str;
	}
	else
	{
		BuildMarkedText(drawText, str, m_szMark);
	}

	int fontHeight = m_TextInfo.GetHeight();
	if (fontHeight <= 0)
		fontHeight = 12;

	bool bold = (m_TextInfo.GetBoldLevel() != sTEXTINFO::BL_NONE);

	RenderTextToAlphaBuffer(drawText.c_str(), m_ptStrSize.x, m_ptStrSize.y, fontHeight, bold, false, m_pFTData);
}

void cText::_FTBmpToFTData_MultiLine()
{
	if (m_pFTData == NULL)
		return;

	if (m_ptStrSize.x <= 0 || m_ptStrSize.y <= 0)
		return;

	const TCHAR* str = m_TextInfo.GetText();
	if (str == NULL)
		return;

	std::wstring drawText;

	if (m_bUseMark == false)
	{
		drawText = str;
	}
	else
	{
		BuildMarkedText(drawText, str, m_szMark);
	}

	int fontHeight = m_TextInfo.GetHeight();
	if (fontHeight <= 0)
		fontHeight = 12;

	bool bold = (m_TextInfo.GetBoldLevel() != sTEXTINFO::BL_NONE);

	RenderTextToAlphaBuffer(drawText.c_str(), m_ptStrSize.x, m_ptStrSize.y, fontHeight, bold, true, m_pFTData);
}

void cText::_ReadFTBmp(FT_Bitmap* bitmap, int x, int y, int sx, int sy)
{
	if (bitmap == NULL || bitmap->buffer == NULL)
		return;
	if (sx <= 0 || sy <= 0)
		return;
	if (bitmap->pixel_mode != FT_PIXEL_MODE_GRAY)
		return;

	// Same defensive shape as the bitmap->buffer check above.  m_pFTData is the
	// destination glyph buffer; when an upstream call (eg _AllocData) skipped
	// allocation for a degenerate string (NULL/empty/single-bad-character path),
	// m_pFTData stays NULL and the write below would segfault.  Hits routinely
	// from the post-attack `cDigimonTalk::Print` balloon when the bark string
	// references a missing glyph — pre-existing v487 issue, surfaces on partner
	// skill-cast battle transitions among other places.
	if (m_pFTData == NULL)
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
	if (BitWidth <= 0 || bitmap->rows <= 0)
		return;
	if (Pitch < 0)
		Pitch = -Pitch;
	if (Pitch < BitWidth)
		Pitch = BitWidth;
	int srcTotal = Pitch * (int)bitmap->rows;

	for (i = y, p = 0; p < sy; ++i, ++p)
	{
		for (j = x, q = 0; q < sx; ++j, ++q)
		{
			int destIdx = i * Width + j;
			if (destIdx < 0 || destIdx >= TotalSize)
				continue;
			int srcIdx = p * Pitch + q;
			if (srcIdx < 0 || srcIdx >= srcTotal)
				continue;
			m_pFTData[destIdx] = bitmap->buffer[srcIdx];
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

	int height = m_ptStrSize.y;
	int width = m_ptStrSize.x;
	int totalSize = width * height;

	if (totalSize <= 0)
		return;

	memset(pOrgData, 0, sizeof(DWORD) * totalSize);

	for (int y = 0; y < height - 1; ++y)
	{
		for (int x = 0; x < width - 1; ++x)
		{
			int nIndex = y * width + x;

			if (nIndex < 0 || nIndex >= totalSize)
				continue;

			int alpha = m_pFTData[nIndex];

			if (alpha > 100)
			{
				int outlineIndex = (y + 1) * width + x + 1;

				if (outlineIndex >= 0 && outlineIndex < totalSize)
					pOrgData[outlineIndex] = 0xff101010;
			}
		}
	}

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int nIndex = y * width + x;

			if (nIndex < 0 || nIndex >= totalSize)
				continue;

			int alpha = m_pFTData[nIndex];

			if (alpha > 70)
				pOrgData[nIndex] = ((alpha) << 24) | 0x00ffffff;
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

	int height = m_ptStrSize.y;
	int width = m_ptStrSize.x;
	int totalSize = width * height;

	if (totalSize <= 0)
		return;

	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			int nIndex = y * width + x;

			if (nIndex < 0 || nIndex >= totalSize)
				continue;

			pData[nIndex] = ((m_pFTData[nIndex]) << 24) | 0x00ffffff;
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
cText3D::sBillboard::sBillboard() :s_pBillboard(NULL)
{

}
cText3D::sBillboard::~sBillboard()
{
	Delete();
}

cText3D::cText3D() :m_pBillBoardText(NULL)
{
}

cText3D::~cText3D()
{
	SAFE_NIDELETE(m_pBillBoardText);
	DeleteBillboard();
}

void cText3D::DeleteBillboard()
{
	for (int i = 0; i < m_pbBillBoard.Size(); ++i)
	{
		if (m_pbBillBoard.IsExistElement(i))
		{
			m_pbBillBoard[i]->Delete();
			SAFE_NIDELETE(m_pbBillBoard[i]);
		}
		assert_cs(m_pbBillBoard[i] == NULL);
	}
	m_pbBillBoard.Destroy();
}

bool cText3D::Init3D(cText::sTEXTINFO* pTextInfo)
{
	if (pTextInfo->s_pFont->IsInitialize() == false)
	{
		return false;
	}

	if (pTextInfo->GetText()[0] == NULL)
	{
		pTextInfo->SetText(_T(" "));
		Init3D(pTextInfo);
		return false;
	}

	m_TextInfo = *pTextInfo;
	assert_cs(m_pBillBoardText == NULL);


	// 값 초기화
	m_ptStrSize = CsPoint::ZERO;
	m_bUseMark = false;

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	_AllocData();

	// 텍스쳐 생성
	_CreateTexture();

	m_pBillBoardText = NiNew CBillboard;

	m_pBillBoardText->CreateTexture(m_ptStrSize.x * 0.5f, m_ptStrSize.y * 0.5f, m_pTexture);

	m_pBillBoardText->SetColor(m_TextInfo.s_Color);

	return true;
}

void cText3D::Render(NiPoint3 vPos, float fX, float fY)
{
	if (m_pBillBoardText == NULL)
		return;

	m_pBillBoardText->Render(vPos, fX, fY);

	float PosX = m_pBillBoardText->GetSizeX();
	float PosY = 0;
	for (int i = 0; i < m_pbBillBoard.Size(); ++i)
	{
		if (m_pbBillBoard.IsExistElement(i))
			m_pbBillBoard[i]->Render(vPos, PosX, PosY);
	}

}

void cText3D::Render(NiPoint3 vPos, float fX, float fY, float fScale)
{
	if (m_pBillBoardText == NULL)
		return;

	m_pBillBoardText->SetScale(fScale);
	m_pBillBoardText->Render(vPos, fX, fY);

	float PosX = m_pBillBoardText->GetSizeX();
	float PosY = 0;
	for (int i = 0; i < m_pbBillBoard.Size(); ++i)
	{
		if (m_pbBillBoard.IsExistElement(i))
			m_pbBillBoard[i]->Render(vPos, PosX, PosY, fScale);
	}
}

bool cText3D::SetText(int nStr)
{
	TCHAR szNum[16];
	_stprintf_s(szNum, 16, _T("%d"), nStr);
	return SetText(szNum);
}

bool cText3D::SetText(TCHAR const* szStr)
{
	if (m_TextInfo.s_pFont == NULL)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	// 같은 글자라면 패스
	if (_tcscmp(szStr, m_TextInfo.GetText()) == 0)
		return false;

	if (szStr[0] == NULL)
	{
		if (m_pTexture)
		{
			m_pPixelData = 0;
			m_pTexture = 0;
			m_ptStrSize = CsPoint::ZERO;
			m_TextInfo.SetText(_T(""));
			return true;
		}
		return false;
	}

	m_TextInfo.SetText(szStr);

	// 지우고 새로 생성
	m_pPixelData = 0;
	m_pTexture = 0;
	SAFE_DELETE_ARRAY(m_pFTData);

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if (_AllocData() == false)
	{
		return false;
	}

	// 텍스쳐 생성
	_CreateTexture();

	m_pBillBoardText->ChangeTexture(m_ptStrSize * 0.5f, m_pTexture);
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
			m_pbBillBoard[i]->s_pBillboard->SetAlpha(fAlpha);
	}
}

void cText3D::SetColor(NiColor FontColor /*부하가적다*/)
{
	if (m_pBillBoardText)
	{
		m_pBillBoardText->SetColor(FontColor);
	}
}

void cText3D::AddBillBoard(char const* pFileName, NiPoint2 DeltaPos, NiPoint2 ptSize)
{
	sBillboard* pBillboard = NiNew sBillboard;
	pBillboard->Init(pFileName, DeltaPos, ptSize);

	m_pbBillBoard.PushBack(pBillboard);
}

void cText3D::AddBillBoard(char const* pFileName, NiPoint2 DeltaPos, float fTexX1, float fTexX2, float fTexY1, float fTexY2, NiPoint2 ptSize)
{
	sBillboard* pBillboard = NiNew sBillboard;
	pBillboard->Init(pFileName, fTexX1, fTexX2, fTexY1, fTexY2, DeltaPos, ptSize);

	m_pbBillBoard.PushBack(pBillboard);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 추가 billboard
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void cText3D::sBillboard::Init(char const* pFileName, NiPoint2 DeltaPos, NiPoint2 ptSize)
{
	s_Size = ptSize;
	s_DeltaPos = DeltaPos;

	s_pBillboard = NiNew CBillboard;
	s_pBillboard->CreateFile(pFileName, s_Size.x, s_Size.y);
}

void cText3D::sBillboard::Init(char const* pFileName, float fTexX1, float fTexX2, float fTexY1, float fTexY2, NiPoint2 DeltaPos, NiPoint2 ptSize)
{
	s_Size = ptSize;
	s_DeltaPos = DeltaPos;

	s_pBillboard = NiNew CBillboard;
	s_pBillboard->CreateFile(pFileName, s_Size.x, s_Size.y, fTexX1, fTexX2, fTexY1, fTexY2);
}

void cText3D::sBillboard::Render(NiPoint3 vPos, float& fX, float& fY)
{
	if (s_pBillboard == NULL)
		return;

	if (s_DeltaPos.x > 0)
		s_pBillboard->Render(vPos, (fX + s_DeltaPos.x), s_DeltaPos.y);
	else
		s_pBillboard->Render(vPos, -(fX - s_DeltaPos.x), s_DeltaPos.y);

	fX += s_Size.x + s_DeltaPos.x;
	fY += 0;
}

void cText3D::sBillboard::Render(NiPoint3 vPos, float& fX, float& fY, float fScale)
{
	if (s_pBillboard == NULL)
		return;

	s_pBillboard->SetScale(fScale);
	if (s_DeltaPos.x > 31)
		s_pBillboard->Render(vPos, (fX + s_DeltaPos.x), s_DeltaPos.y);
	else if (1 < s_DeltaPos.x && s_DeltaPos.x < 31)
		s_pBillboard->Render(vPos, (0 + s_DeltaPos.x), s_DeltaPos.y);
	else if (s_DeltaPos.x == 0)
		s_pBillboard->Render(vPos, (0 + s_DeltaPos.x), s_DeltaPos.y);
	else if (-31 < s_DeltaPos.x && s_DeltaPos.x < 0)
		s_pBillboard->Render(vPos, -(0 - s_DeltaPos.x), s_DeltaPos.y);
	else
		s_pBillboard->Render(vPos, -(fX - s_DeltaPos.x), s_DeltaPos.y);

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





void cMyText::sTEXTINFO::Init(CFont::eFACE_SIZE eFontSize /*=CFont::FS_10*/, NiColor color /*=FONT_WHITE*/)
{
	Init(&g_pEngine->m_FontSystem, eFontSize, color);
}

void cMyText::sTEXTINFO::Init(CFont* pFont, CFont::eFACE_SIZE eFontSize /*=CFont::FS_10*/, NiColor color /*=FONT_WHITE*/)
{
	s_pFont = pFont;
	s_eFontSize = eFontSize;
	s_Color = color;

	s_bOutLine = true;
	s_eBoldLevel = BL_NONE;

	s_eTextAlign = DT_LEFT;
}

void cMyText::sTEXTINFO::Init(CsHelp::sTEXT* pHelpText)
{
	Init(FONT_SIZE_CAL(pHelpText->s_nTextSize), NiColor(pHelpText->s_Color[0] / 255.0f, pHelpText->s_Color[1] / 255.0f, pHelpText->s_Color[2] / 255.0f));
	SetBoldLevel((cMyText::sTEXTINFO::eBOLD_LEVEL)pHelpText->s_nBold);
}

int cMyText::sTEXTINFO::GetBoldSize()
{
	switch (s_eBoldLevel)
	{
	case BL_NONE:	return 0;
	case BL_1:		return 4;
	case BL_2:		return 5;
	case BL_3:		return 6;
	case BL_4:		return 7;
	case BL_5:		return 8;
	}
	assert_csm1(false, _T("BoldLevel = %d"), s_eBoldLevel);
	return 0;
}

void cMyText::sTEXTINFO::SetText_Reduce(TCHAR* szText, int nLen, int nDotNum /* = 2 */)
{
	TCHAR t = szText[nLen];
	szText[nLen] = NULL;

	TCHAR szDot[2] = _T(".");
	s_strText = szText;
	szText[nLen] = t;

	for (int i = 0; i < nDotNum; i++)
		s_strText.append(szDot);
}

//=======================================================================================
//
//		베이스
//
//=======================================================================================

cMyText::cMyText()
{
	m_pTexture = NULL;
	m_pPixelData = NULL;
	m_pFTData = NULL;

	m_bUseMark = false;
}

void cMyText::Delete()
{


	m_pPixelData = 0;
	m_pTexture = 0;
	SAFE_DELETE_ARRAY(m_pFTData);
}

void cMyText::Init(cWindow* pParent, CsPoint pos, CsPoint pixel, sTEXTINFO* pTextInfo, bool bApplyWindowSize)
{
	if (pTextInfo->s_pFont == NULL)
		return;

	if (pTextInfo->s_pFont->IsInitialize() == false)
		return;

	m_TextInfo = *pTextInfo;
	// 값 초기화
	m_ptStrSize = CsPoint::ZERO;

	m_ptPixelSize = pixel;

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if (_AllocData() == false)
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
	if (m_TextInfo.s_pFont == NULL)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	if (szStr == NULL)
		return false;

	int fontHeight = m_TextInfo.GetHeight();
	if (fontHeight <= 0)
		fontHeight = 12;

	int x = 0;
	int fx = 0;
	int y = fontHeight;

	int nLen = (int)_tcslen(szStr);

	for (int i = 0; i < nLen; ++i)
	{
		if (szStr[i] == 0x000d)
			continue;

		int cw = FallbackCharWidthFromHeight(fontHeight, m_bUseMark ? m_szMark : szStr[i]);
		x += cw;

		if (m_bSet)
		{
			if ((m_ptPixelSize.x - 2) >= x)
			{
				if (fx < x)
					fx = x;
			}
			else
			{
				x = cw;
				y += fontHeight;
			}
		}
	}

	if (m_bSet)
		size.x = fx + 2;
	else
		size.x = x + 2;

	size.y = y;
	hBase = (int)(fontHeight * 0.75f) << 6;

	return (size.x > 2 && size.y > 0);
}

int cMyText::GetCharWidth(FT_Face face, FT_ULong charcode)
{
	return FallbackCharWidthFromHeight(12, (wchar_t)charcode);
}

bool cMyText::SetText(int nStr)
{
	TCHAR szNum[16];
	_stprintf_s(szNum, 16, _T("%d"), nStr);
	return SetText(szNum);
}

bool cMyText::SetText(TCHAR* szStr /*부하가크다*/, bool bSet)
{
	if (m_TextInfo.s_pFont == NULL)
		return false;

	if (m_TextInfo.s_pFont->IsInitialize() == false)
		return false;

	cText::sTEXTINFO safeInfo;
	safeInfo.SetText(szStr);
	std::wstring safeInput = safeInfo.s_strText;

	if (safeInput.empty())
	{
		if (m_pTexture)
		{
			m_pPixelData = 0;
			m_pTexture = 0;
			m_ptStrSize = CsPoint::ZERO;
			m_TextInfo.SetText(_T(""));
			return true;
		}
		return false;
	}

	// 같은 글자라면 패스
	if (safeInput == m_TextInfo.GetText())
		return false;

	m_TextInfo.SetText(const_cast<TCHAR*>(safeInput.c_str()));

	// 지우고 새로 생성
	m_pPixelData = 0;
	m_pTexture = 0;
	SAFE_DELETE_ARRAY(m_pFTData);
	m_bSet = bSet;

	// 데이터 메모리 할당 및 텍스트를 데이터 화
	if (_AllocData() == false)
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
	if (GetStringSize(m_ptStrSize, m_nFTSize_HBase, (TCHAR*)m_TextInfo.GetText()) == false)
		return false;

	assert_cs((m_ptStrSize.x != 0) && (m_ptStrSize.y != 0));

	assert_cs(m_pPixelData == NULL);

	if (m_ptStrSize.x > m_ptPixelSize.x) { m_ptPixelSize.x = m_ptStrSize.x; }
	if (m_ptStrSize.y > m_ptPixelSize.y) { m_ptPixelSize.y = m_ptStrSize.y; }
	//m_pPixelData = NiNew NiPixelData( m_ptStrSize.x, m_ptStrSize.y, NiPixelFormat::RGBA32 );
	m_pPixelData = NiNew NiPixelData(m_ptPixelSize.x, m_ptPixelSize.y, NiPixelFormat::RGBA32);

	assert_cs(m_pFTData == NULL);
	int nFTSize = m_ptStrSize.Mul();
	//int nFTSize = (100*50);
	m_pFTData = xnew BYTE[nFTSize];
	memset(m_pFTData, 0, sizeof(BYTE) * nFTSize);

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

	const TCHAR* str = m_TextInfo.GetText();
	if (str == NULL)
		return;

	std::wstring drawText;

	if (m_bUseMark == false)
	{
		drawText = str;
	}
	else
	{
		BuildMarkedText(drawText, str, m_szMark);
	}

	int fontHeight = m_TextInfo.GetHeight();
	if (fontHeight <= 0)
		fontHeight = 12;

	bool bold = (m_TextInfo.GetBoldLevel() != sTEXTINFO::BL_NONE);

	RenderTextToAlphaBuffer(drawText.c_str(), m_ptStrSize.x, m_ptStrSize.y, fontHeight, bold, m_bSet, m_pFTData);
}

//2 -2 m_pFTData 에다 데이터를 넣는거 같은데..(시작픽셀x,y)(width,height 픽셀)
void cMyText::_ReadFTBmp(FT_Bitmap* bitmap, int x, int y, int sx, int sy)
{
	if (bitmap == NULL || bitmap->buffer == NULL)
		return;
	if (sx <= 0 || sy <= 0)
		return;
	if (bitmap->pixel_mode != FT_PIXEL_MODE_GRAY)
		return;
	if (m_pFTData == NULL)
		return;

	int i, j, p, q;
	int Width = m_ptStrSize.x;
	int BitWidth = bitmap->width;
	int Pitch = bitmap->pitch;
	int Height = m_ptStrSize.y;
	int TotalSize = Width * Height;
	if (Width <= 0 || Height <= 0 || BitWidth <= 0 || bitmap->rows <= 0)
		return;
	if (Pitch < 0)
		Pitch = -Pitch;
	if (Pitch < BitWidth)
		Pitch = BitWidth;
	int srcTotal = Pitch * (int)bitmap->rows;

	for (i = y, p = 0; p < sy; ++i, ++p)
	{
		for (j = x, q = 0; q < sx; ++j, ++q)
		{
			int destIdx = i * Width + j;
			if (destIdx < 0 || destIdx >= TotalSize)
				continue;
			int srcIdx = p * Pitch + q;
			if (srcIdx < 0 || srcIdx >= srcTotal)
				continue;
			m_pFTData[destIdx] = bitmap->buffer[srcIdx];
		}
	}
}

void cMyText::_CreateTexture()
{
	if (m_pPixelData == NULL)
		return;

	if (m_pFTData == NULL)
		return;

	if (m_ptPixelSize.x <= 0 || m_ptPixelSize.y <= 0)
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

	if (m_pPixelData == NULL)
		return;

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

	int pixelWidth = m_ptPixelSize.x;
	int pixelHeight = m_ptPixelSize.y;
	int pixelTotalSize = pixelWidth * pixelHeight;

	int textWidth = m_ptStrSize.x;
	int textHeight = m_ptStrSize.y;
	int textTotalSize = textWidth * textHeight;

	if (pixelTotalSize <= 0 || textTotalSize <= 0)
		return;

	memset(pOrgData, 0, sizeof(DWORD) * pixelTotalSize);

	for (int y = 0; y < pixelHeight; ++y)
	{
		for (int x = 0; x < pixelWidth; ++x)
		{
			int idx = y * pixelWidth + x;
			if (idx >= 0 && idx < pixelTotalSize)
				pOrgData[idx] = 0xff6ad2c8;
		}
	}

	int first = 0;

	if (m_TextInfo.s_eTextAlign == DT_CENTER)
	{
		int temp = pixelWidth - textWidth;
		if (temp > 0)
			first = (temp / 2) + (temp % 2);
	}

	for (int y = 0; y < textHeight - 1; ++y)
	{
		for (int x = 0; x < textWidth - 1; ++x)
		{
			int textIdx = y * textWidth + x;
			if (textIdx < 0 || textIdx >= textTotalSize)
				continue;

			int alpha = m_pFTData[textIdx];

			if (alpha > 100)
			{
				int destX = x + 1 + first;
				int destY = y + 1;
				int destIdx = destY * pixelWidth + destX;

				if (destX >= 0 && destX < pixelWidth && destY >= 0 && destY < pixelHeight && destIdx >= 0 && destIdx < pixelTotalSize)
					pOrgData[destIdx] = 0xff101010;
			}
		}
	}

	for (int y = 0; y < textHeight; ++y)
	{
		for (int x = 0; x < textWidth; ++x)
		{
			int textIdx = y * textWidth + x;
			if (textIdx < 0 || textIdx >= textTotalSize)
				continue;

			int alpha = m_pFTData[textIdx];

			if (alpha > 70)
			{
				int destX = x + first;
				int destY = y;
				int destIdx = destY * pixelWidth + destX;

				if (destX >= 0 && destX < pixelWidth && destY >= 0 && destY < pixelHeight && destIdx >= 0 && destIdx < pixelTotalSize)
					pOrgData[destIdx] = ((alpha) << 24) | 0x00ffffff;
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

	int pixelWidth = m_ptPixelSize.x;
	int pixelHeight = m_ptPixelSize.y;
	int pixelTotalSize = pixelWidth * pixelHeight;

	int textWidth = m_ptStrSize.x;
	int textHeight = m_ptStrSize.y;
	int textTotalSize = textWidth * textHeight;

	if (pixelTotalSize <= 0 || textTotalSize <= 0)
		return;

	memset(pData, 0, sizeof(DWORD) * pixelTotalSize);

	int first = 0;

	if (m_TextInfo.s_eTextAlign == DT_CENTER)
	{
		int temp = pixelWidth - textWidth;
		if (temp > 0)
			first = (temp / 2) + (temp % 2);
	}

	for (int y = 0; y < textHeight; ++y)
	{
		for (int x = 0; x < textWidth; ++x)
		{
			int textIdx = y * textWidth + x;
			if (textIdx < 0 || textIdx >= textTotalSize)
				continue;

			int alpha = m_pFTData[textIdx];

			if (alpha > 70)
			{
				int destX = x + first;
				int destY = y;
				int destIdx = destY * pixelWidth + destX;

				if (destX >= 0 && destX < pixelWidth && destY >= 0 && destY < pixelHeight && destIdx >= 0 && destIdx < pixelTotalSize)
					pData[destIdx] = ((alpha) << 24) | 0x00ffffff;
			}
		}
	}

	m_pPixelData->MarkAsChanged();
}
