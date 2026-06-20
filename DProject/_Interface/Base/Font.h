

#pragma once 

// 프리타입 폰트
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_OUTLINE_H

class CFont
{
public:
	CFont();
	virtual ~CFont(){ Release(); }
	//=======================================================================================
	//
	//		전역
	//
	//=======================================================================================
protected:
	static FT_Library	m_library;
	static int			m_nLogPixelSX;
	static int			m_nLogPixelSY;
	static bool			m_bGlobalInit;
public:
	static void			GlobalRelease();
	static void			GlobalInit();
	static bool			IsGlobalInit(){ return m_bGlobalInit; }


	//=======================================================================================
	//
	//		베이스
	//
	//=======================================================================================
protected:
	char*				m_pMemoryData;

	bool				m_bInitialize;
	bool				m_bKerning;
public:
	bool				IsInitialize() const{ return m_bInitialize; }
	void				Release();
	bool				Init( char const* cFontPath );


	//=======================================================================================
	//
	//		페이스
	//
	//=======================================================================================
public:
	// 2개는 동일 하게 매치시켜 주어야 한다 
	enum eFACE_SIZE{
		/*FS_8 = 8,
		FS_9 = 9,
		FS_10 = 10,
		FS_12 = 12,
		FS_13 = 13,
		FS_14 = 14,
		FS_16 = 16,
		FS_20 = 20,
		FS_24 = 24,
		FS_32 = 32,*/

		FS_8 = 7,
		FS_9 = 8,
		FS_10 = 9,
		FS_11 = 10,
		FS_12 = 11,
		FS_13 = 12,
		FS_14 = 13,
		FS_16 = 15,
		FS_20 = 19,
		FS_24 = 23,
		FS_32 = 31,
	};
	enum eFACE_INDEX{
		FI_8 = 0,
		FI_9,
		FI_10,
		FI_11,
		FI_12,
		FI_13,
		FI_14,
		FI_16,
		FI_20,
		FI_24,
		FI_32,
		FI_MAXCOUNT
	};

protected:
	int					GetSize_FromFaceIndex( int nIndex ) const;
	int					GetIndex_FromFaceSize( int nSize ) const;
	
	FT_Face				m_Face[ FI_MAXCOUNT ];
	int					m_nSize[ FI_MAXCOUNT ];
	int					m_nHBase[ FI_MAXCOUNT ];

public:
	FT_Face				GetFace( eFACE_SIZE nFontSize ) const { return m_Face[ GetIndex_FromFaceSize( nFontSize ) ]; }
	int					GetHeight( eFACE_SIZE nFontSize ) const { return m_nSize[ GetIndex_FromFaceSize( nFontSize ) ]; }
	int					GetHBase( eFACE_SIZE nFontSize ) const { return m_nHBase[ GetIndex_FromFaceSize( nFontSize ) ]; }

	bool				CheckFontGlyph(TCHAR* str);
};

#define FONT_SIZE_CAL( nSize )		( (CFont::eFACE_SIZE)( nSize - ( 8 - CFont::FS_8 ) ) )		// 폰트 사이즈 변경 있을때
//#define FONT_SIZE_CAL( nSize )	( (CFont::eFACE_SIZE)( nSize ) )							// 폰트 사이즈 변경 없을때

