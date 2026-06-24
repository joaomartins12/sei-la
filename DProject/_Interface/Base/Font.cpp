#include "stdafx.h"
#include "Font.h"

#include <string>
#include <list>


//=======================================================================================
//
//		Pack01 font helpers
//
//=======================================================================================

namespace
{
	static std::string NormalizeForCompareA(const std::string& path)
	{
		std::string result = path;

		for (size_t i = 0; i < result.size(); ++i)
		{
			if (result[i] == '/')
				result[i] = '\\';

			if (result[i] >= 'A' && result[i] <= 'Z')
				result[i] = result[i] - 'A' + 'a';
		}

		return result;
	}

	static std::string GetFileNameOnlyA(const std::string& path)
	{
		size_t pos1 = path.find_last_of('\\');
		size_t pos2 = path.find_last_of('/');

		size_t pos = std::string::npos;

		if (pos1 == std::string::npos)
			pos = pos2;
		else if (pos2 == std::string::npos)
			pos = pos1;
		else
			pos = (pos1 > pos2) ? pos1 : pos2;

		if (pos == std::string::npos)
			return path;

		return path.substr(pos + 1);
	}

	static bool EndsWithA(const std::string& text, const std::string& suffix)
	{
		if (text.size() < suffix.size())
			return false;

		return text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
	}

	static std::string ToSlashA(const std::string& path)
	{
		std::string result = path;

		for (size_t i = 0; i < result.size(); ++i)
		{
			if (result[i] == '\\')
				result[i] = '/';
		}

		return result;
	}

	static std::string ToBackSlashA(const std::string& path)
	{
		std::string result = path;

		for (size_t i = 0; i < result.size(); ++i)
		{
			if (result[i] == '/')
				result[i] = '\\';
		}

		return result;
	}

	static bool TryPackCandidateExactA(const std::string& candidate, std::string& resolvedPath)
	{
		if (candidate.empty())
			return false;

		if (CsFPS::CsFPSystem::IsExistOnlyPack(0, candidate.c_str()))
		{
			// IMPORTANTE:
			// devolver exatamente o caminho que o pack aceitou.
			// Não converter / para \ aqui, porque o hash do Pack01 pode usar '/'.
			resolvedPath = candidate;
			return true;
		}

		return false;
	}

	static bool TryPackCandidateAllSlashModesA(const std::string& candidate, std::string& resolvedPath)
	{
		if (TryPackCandidateExactA(candidate, resolvedPath))
			return true;

		std::string slash = ToSlashA(candidate);
		if (slash != candidate && TryPackCandidateExactA(slash, resolvedPath))
			return true;

		std::string backSlash = ToBackSlashA(candidate);
		if (backSlash != candidate && TryPackCandidateExactA(backSlash, resolvedPath))
			return true;

		return false;
	}

	static bool FindFontInPack01FileListA(const std::string& requestedPath, std::string& resolvedPath)
	{
		resolvedPath.clear();

		std::string lowerRequest = NormalizeForCompareA(requestedPath);
		std::string lowerFileName = NormalizeForCompareA(GetFileNameOnlyA(requestedPath));

		std::list<std::string> fileList;
		CsFPS::CsFPSystem::GetFileList(0, fileList);

		if (fileList.empty())
			return false;

		OutputDebugStringA("[FONT] Searching Pack01 file list for font. Requested=");
		OutputDebugStringA(requestedPath.c_str());
		OutputDebugStringA("\n");

		// 1) Match exato ignorando maiúsculas/minúsculas e slash.
		for (std::list<std::string>::iterator it = fileList.begin(); it != fileList.end(); ++it)
		{
			std::string packPathOriginal = *it;
			std::string lowerPackPath = NormalizeForCompareA(packPathOriginal);

			if (lowerPackPath == lowerRequest)
			{
				resolvedPath = packPathOriginal; // manter caminho original do pack
				return true;
			}
		}

		// 2) Pasta font + nome do ficheiro.
		for (std::list<std::string>::iterator it = fileList.begin(); it != fileList.end(); ++it)
		{
			std::string packPathOriginal = *it;
			std::string lowerPackPath = NormalizeForCompareA(packPathOriginal);

			if (EndsWithA(lowerPackPath, "\\font\\" + lowerFileName))
			{
				resolvedPath = packPathOriginal; // manter caminho original do pack
				return true;
			}
		}

		// 3) Só nome do ficheiro.
		for (std::list<std::string>::iterator it = fileList.begin(); it != fileList.end(); ++it)
		{
			std::string packPathOriginal = *it;
			std::string lowerPackPath = NormalizeForCompareA(packPathOriginal);

			if (EndsWithA(lowerPackPath, "\\" + lowerFileName))
			{
				resolvedPath = packPathOriginal; // manter caminho original do pack
				return true;
			}
		}

		// 4) Fallback específico.
		for (std::list<std::string>::iterator it = fileList.begin(); it != fileList.end(); ++it)
		{
			std::string packPathOriginal = *it;
			std::string lowerPackPath = NormalizeForCompareA(packPathOriginal);

			if (lowerPackPath.find("tahoma.ttf") != std::string::npos)
			{
				resolvedPath = packPathOriginal; // manter caminho original do pack
				return true;
			}
		}

		return false;
	}

	static bool ResolvePack01FontPathA(const char* requestedFileName, std::string& resolvedPath)
	{
		resolvedPath.clear();

		if (requestedFileName == NULL || requestedFileName[0] == '\0')
			return false;

		std::string requested = requestedFileName;
		std::string fileName = GetFileNameOnlyA(requested);

		// 1) O caminho original exatamente como veio do GlobalData.
		if (TryPackCandidateAllSlashModesA(requested, resolvedPath))
			return true;

		// 2) Variações comuns. Incluo / e \ porque o Pack01 pode ter hash com '/'.
		std::string candidates[] =
		{
			"Data/font/" + fileName,
			"data/font/" + fileName,
			"Data/Font/" + fileName,
			"data/Font/" + fileName,

			"Data\\font\\" + fileName,
			"data\\font\\" + fileName,
			"Data\\Font\\" + fileName,
			"data\\Font\\" + fileName,

			"Data/font/tahoma.ttf",
			"data/font/tahoma.ttf",
			"Data/Font/tahoma.ttf",
			"data/Font/tahoma.ttf",

			"Data\\font\\tahoma.ttf",
			"data\\font\\tahoma.ttf",
			"Data\\Font\\tahoma.ttf",
			"data\\Font\\tahoma.ttf"
		};

		for (int i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i)
		{
			if (TryPackCandidateAllSlashModesA(candidates[i], resolvedPath))
				return true;
		}

		// 3) Procurar na lista real do Pack01.
		if (FindFontInPack01FileListA(requested, resolvedPath))
			return true;

		return false;
	}

	static bool LoadFaceFromMemoryA(FT_Library library, char* pFontData, size_t nDataSize, int nSize, int nLogPixelSX, int nLogPixelSY, FT_Face* pOutFace)
	{
		if (pOutFace == NULL)
			return false;

		*pOutFace = NULL;

		if (library == NULL || pFontData == NULL || nDataSize == 0)
			return false;

		FT_Error error = FT_New_Memory_Face(
			library,
			(const FT_Byte*)pFontData,
			(FT_Long)nDataSize,
			0,
			pOutFace
		);

		if (error != 0 || *pOutFace == NULL)
			return false;

#ifdef VERSION_USA	// 텍스트 겹침 현상 수정
		error = FT_Set_Char_Size(*pOutFace, nSize * 68, nSize * 64, nLogPixelSX, nLogPixelSY);
#else
		error = FT_Set_Char_Size(*pOutFace, nSize * 64, nSize * 64, nLogPixelSX, nLogPixelSY);
#endif

		if (error != 0)
		{
			FT_Done_Face(*pOutFace);
			*pOutFace = NULL;
			return false;
		}

		return true;
	}

	static bool LoadFaceFromDiskA(FT_Library library, const char* cFileName, int nSize, int nLogPixelSX, int nLogPixelSY, FT_Face* pOutFace)
	{
		if (pOutFace == NULL)
			return false;

		*pOutFace = NULL;

		if (library == NULL || cFileName == NULL || cFileName[0] == '\0')
			return false;

		FT_Error error = FT_New_Face(library, cFileName, 0, pOutFace);

		if (error != 0 || *pOutFace == NULL)
			return false;

#ifdef VERSION_USA	// 텍스트 겹침 현상 수정
		error = FT_Set_Char_Size(*pOutFace, nSize * 68, nSize * 64, nLogPixelSX, nLogPixelSY);
#else
		error = FT_Set_Char_Size(*pOutFace, nSize * 64, nSize * 64, nLogPixelSX, nLogPixelSY);
#endif

		if (error != 0)
		{
			FT_Done_Face(*pOutFace);
			*pOutFace = NULL;
			return false;
		}

		return true;
	}

	static void CalculateFontMetricsA(FT_Face face, int& nOutSize, int& nOutHBase)
	{
		nOutSize = 0;
		nOutHBase = 0;

		if (face == NULL || face->glyph == NULL)
			return;

		FT_ULong charcode;
		TCHAR szCheck[] = _T("bg불");
		int nLen = 3;
		FT_GlyphSlot slot = face->glyph;
		FT_Glyph_Metrics* pMetrics = NULL;
		int nHeight = 0;
		int By = 0;

		for (int code = 0; code < nLen; ++code)
		{
			charcode = szCheck[code];

			if (FT_Load_Char(face, charcode, FT_LOAD_RENDER) != 0)
				continue;

			pMetrics = &(slot->metrics);
			nHeight = pMetrics->height;
			By = (pMetrics->vertAdvance - pMetrics->horiBearingY) - (pMetrics->vertAdvance / 6);

			if (nOutSize < By + nHeight)
				nOutSize = By + nHeight;
		}

		nOutSize = (nOutSize >> 6) + 2;
		nOutHBase = (int)(nOutSize * 0.75f);
	}

	static void ReleaseFaceArrayA(FT_Face* pFaceArray, int nCount)
	{
		if (pFaceArray == NULL)
			return;

		for (int i = 0; i < nCount; ++i)
		{
			if (pFaceArray[i] != NULL)
			{
				FT_Done_Face(pFaceArray[i]);
				pFaceArray[i] = NULL;
			}
		}
	}
}


//=======================================================================================
//
//		전역
//
//=======================================================================================

FT_Library  CFont::m_library;
int			CFont::m_nLogPixelSX;
int			CFont::m_nLogPixelSY;
bool		CFont::m_bGlobalInit = false;

void CFont::GlobalRelease()
{
	assert_cs(m_bGlobalInit == true);

	FT_Done_FreeType(m_library);
	m_bGlobalInit = false;
}

void CFont::GlobalInit()
{
	assert_cs(m_bGlobalInit == false);


	FT_Error  error;
	error = FT_Init_FreeType(&m_library);

	assert_cs(error == 0);

	HDC hdc = GetDC(GAMEAPP_ST.GetHWnd());
	if (hdc)
	{
		m_nLogPixelSX = GetDeviceCaps(hdc, LOGPIXELSX);
		m_nLogPixelSY = GetDeviceCaps(hdc, LOGPIXELSY);
		ReleaseDC(GAMEAPP_ST.GetHWnd(), hdc);
	}
	else
	{
		m_nLogPixelSX = 96;
		m_nLogPixelSY = 96;
	}
	m_bGlobalInit = true;
}



//=======================================================================================
//
//		베이스
//
//=======================================================================================

CFont::CFont()
{
	m_bInitialize = false;
	m_pMemoryData = NULL;

	for (int i = 0; i < FI_MAXCOUNT; ++i)
	{
		m_Face[i] = NULL;
		m_nSize[i] = 0;
		m_nHBase[i] = 0;
	}
}

void CFont::Release()
{
	if (m_bInitialize == false)
		return;

	m_bInitialize = false;

	ReleaseFaceArrayA(m_Face, FI_MAXCOUNT);

	SAFE_DELETE_ARRAY(m_pMemoryData);
	m_pMemoryData = NULL;
}

bool CFont::Init(char const* cFileName)
{
	assert_cs(m_bInitialize == false);

	if (cFileName == NULL || cFileName[0] == '\0')
		return false;

	m_pMemoryData = NULL;

	for (int i = 0; i < FI_MAXCOUNT; ++i)
	{
		m_Face[i] = NULL;
		m_nSize[i] = 0;
		m_nHBase[i] = 0;
	}

	bool bUsePackFont = false;
	size_t nDataSize = 0;
	std::string resolvedPackPath;

	// Primeiro: resolver e carregar diretamente do Pack01.
	if (ResolvePack01FontPathA(cFileName, resolvedPackPath))
	{
		nDataSize = CsFPS::CsFPSystem::Allocate_GetFileData(0, &m_pMemoryData, resolvedPackPath.c_str());

		if (m_pMemoryData != NULL && nDataSize > 0)
		{
			bUsePackFont = true;

			OutputDebugStringA("[FONT] Loaded font from Pack01: ");
			OutputDebugStringA(resolvedPackPath.c_str());
			OutputDebugStringA("\n");
		}
		else
		{
			OutputDebugStringA("[FONT] Found font path but Allocate_GetFileData failed: ");
			OutputDebugStringA(resolvedPackPath.c_str());
			OutputDebugStringA("\n");

			SAFE_DELETE_ARRAY(m_pMemoryData);
			m_pMemoryData = NULL;
			nDataSize = 0;
		}
	}
	else
	{
		OutputDebugStringA("[FONT] Font not found in Pack01. Fallback to disk: ");
		OutputDebugStringA(cFileName);
		OutputDebugStringA("\n");
	}

	for (int i = 0; i < FI_MAXCOUNT; ++i)
	{
#ifdef VERSION_TH
		FT_F26Dot6 nSize = GetSize_FromFaceIndex(i);
#else
		int nSize = GetSize_FromFaceIndex(i);
#endif

		bool bFaceLoaded = false;

		if (bUsePackFont)
			bFaceLoaded = LoadFaceFromMemoryA(m_library, m_pMemoryData, nDataSize, nSize, m_nLogPixelSX, m_nLogPixelSY, &m_Face[i]);
		else
			bFaceLoaded = LoadFaceFromDiskA(m_library, cFileName, nSize, m_nLogPixelSX, m_nLogPixelSY, &m_Face[i]);

		if (bFaceLoaded == false)
		{
			OutputDebugStringA("[FONT] Failed to create FT_Face. Requested=");
			OutputDebugStringA(cFileName);
			OutputDebugStringA(" PackPath=");
			OutputDebugStringA(resolvedPackPath.c_str());
			OutputDebugStringA("\n");

			ReleaseFaceArrayA(m_Face, FI_MAXCOUNT);
			SAFE_DELETE_ARRAY(m_pMemoryData);
			m_pMemoryData = NULL;
			m_bInitialize = false;
			return false;
		}

		CalculateFontMetricsA(m_Face[i], m_nSize[i], m_nHBase[i]);

		// Fallback defensivo para evitar fonte com altura 0.
		if (m_nSize[i] <= 0)
		{
			m_nSize[i] = nSize + 2;
			m_nHBase[i] = (int)(m_nSize[i] * 0.75f);
		}
	}

	m_bInitialize = true;
	return true;
}


//=======================================================================================
//
//		페이스
//
//=======================================================================================
int CFont::GetSize_FromFaceIndex(int nIndex) const
{
	switch (nIndex)
	{
	case FI_8:		return FS_8;
	case FI_9:		return FS_9;
	case FI_10:		return FS_10;
	case FI_11:		return FS_11;
	case FI_12:		return FS_12;
	case FI_13:		return FS_13;
	case FI_14:		return FS_14;
	case FI_16:		return FS_16;
	case FI_20:		return FS_20;
	case FI_24:		return FS_24;
	case FI_32:		return FS_32;
	}
	assert_cs(false);
	return 0;
}

int CFont::GetIndex_FromFaceSize(int nSize) const
{
	switch (nSize)
	{
	case FS_8:		return 	FI_8;
	case FS_9:		return 	FI_9;
	case FS_10:		return 	FI_10;
	case FS_11:		return 	FI_11;
	case FS_12:		return 	FI_12;
	case FS_13:		return 	FI_13;
	case FS_14:		return 	FI_14;
	case FS_16:		return 	FI_16;
	case FS_20:		return 	FI_20;
	case FS_24:		return 	FI_24;
	case FS_32:		return 	FI_32;
	}
	assert_cs(false);
	return 0;
}

bool CFont::CheckFontGlyph(TCHAR* str)
{
	if (str == NULL || m_Face[0] == NULL)
		return false;

	for (int i = 0; i < (int)wcslen(str); i++)
	{
		FT_ULong charcode = str[i];


		FT_UInt glyph_index = FT_Get_Char_Index(m_Face[0], charcode);
#if ( defined VERSION_TW || defined VERSION_HK )
		// msjh.ttc 폰트에서 'j' 문자가 연속되면 공백이 전문자를 가리는 현상으로 'j'문자 일때 자동 힌팅 제거
		if (FT_Load_Glyph(m_Face[0], glyph_index, FT_LOAD_DEFAULT | FT_LOAD_NO_AUTOHINT) != 0)
			return false;
#else
		if (FT_Load_Glyph(m_Face[0], glyph_index, FT_LOAD_DEFAULT) != 0)
			return false;
#endif
		if (FT_Render_Glyph(m_Face[0]->glyph, FT_RENDER_MODE_NORMAL) != 0)
			return false;

		if (!glyph_index)
		{
			return false;
		}
	}
	return true;
};
