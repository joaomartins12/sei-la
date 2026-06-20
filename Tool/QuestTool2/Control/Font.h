

#pragma once 

// 프리타입 폰트
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_OUTLINE_H

class cFont
{
public:
	cFont(){ m_bInitialize = false; }
	virtual ~cFont(){ Release(); }
	//=======================================================================================
	//
	//		전역
	//
	//=======================================================================================
protected:
	static FT_Library	m_library;
	static int			m_nLogPixelSX;
	static int			m_nLogPixelSY;
public:
	static void			GlobalRelease();
	static void			GlobalInit();


	//=======================================================================================
	//
	//		베이스
	//
	//=======================================================================================
protected:
	bool				m_bInitialize;
	bool				m_bKerning;
public:
	bool				IsInitialize(){ return m_bInitialize; }
	void				Release();
	void				Init( char* cFontPath );


	//=======================================================================================
	//
	//		페이스
	//
	//=======================================================================================
public:
	// 2개는 동일 하게 매치시켜 주어야 한다 
	enum eFACE_SIZE{
		FS_8 = 8,
		FS_9 = 9,
		FS_10 = 10,
		FS_11 = 11,
		FS_12 = 12,
		FS_13 = 13,
		FS_14 = 14,
		FS_16 = 16,
		FS_20 = 20,
		FS_24 = 24,
		FS_32 = 32,
		FS_52 = 52,
		FS_64 = 64,
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
		FI_52,
		FI_64,
		FI_MAXCOUNT
	};

protected:
	int					GetSize_FromFaceIndex( int nIndex );
	int					GetIndex_FromFaceSize( int nSize );
	
	FT_Face				m_Face[ FI_MAXCOUNT ];
	int					m_nSize[ FI_MAXCOUNT ];

public:
	FT_Face				GetFace( int nFontSize ){ return m_Face[ GetIndex_FromFaceSize( nFontSize ) ]; }
	int					GetHeight( int nFontSize ){ return m_nSize[ GetIndex_FromFaceSize( nFontSize ) ]; }
	bool				CheckFontGlyph(TCHAR* str);
};

extern cFont*	g_pFont;