
#include "stdafx.h"
#include "Font.h"


//=======================================================================================
//
//		전역
//
//=======================================================================================

FT_Library  CFreeFont::m_library;
int			CFreeFont::m_nLogPixelSX;
int			CFreeFont::m_nLogPixelSY;
bool		CFreeFont::m_bGlobalInit = false;

void CFreeFont::GlobalRelease()
{
	assert_cs( m_bGlobalInit == true );

	FT_Done_FreeType( m_library );
	m_bGlobalInit = false;
}

void CFreeFont::GlobalInit()
{
	assert_cs( m_bGlobalInit == false );

	FT_Error  error;
	error = FT_Init_FreeType( &m_library );
	assert_cs( error == 0 );

	/*
	g_Log.Log( L"FT_Init_FreeType  = %d", (int)error );
	m_nLogPixelSX = GetDeviceCaps( GetDC(g_hWnd), LOGPIXELSX );
	g_Log.Log( L"GetDeviceCapsX" );
	m_nLogPixelSY = GetDeviceCaps( GetDC(g_hWnd), LOGPIXELSY );
	g_Log.Log( L"GetDeviceCapsY" );
	*/
	m_bGlobalInit = true;
	
}



//=======================================================================================
//
//		베이스
//
//=======================================================================================

CFreeFont::CFreeFont()
{
	m_bInitialize = false;

	for( int i=0; i<FI_MAXCOUNT; ++i )
	{
		m_Face[ i ] = NULL;
	}
}

void CFreeFont::Release()
{
	if( m_bInitialize == false )
		return;	

	m_bInitialize = false;

	for( int i=0; i<FI_MAXCOUNT; ++i )
	{
		FT_Done_Face( m_Face[ i ] );
	}
	SAFE_DELETE_ARRAY( m_pMemoryData );
}

bool CFreeFont::Init( char* cFileName )
{
	assert_cs( m_bInitialize == false );
	m_pMemoryData = NULL;
	m_bInitialize = true;

	FT_Error  error;
	FT_ULong charcode;
	TCHAR szCheck[ 4 ] = L"bg불";
	int nLen = 3;
	FT_GlyphSlot slot;
	FT_Glyph_Metrics *pMetrics;
	int nHeight, By;	

	// 파일 패킹
	if( g_pFilePackMng )
	{		
		size_t nDataSize = g_pFilePackMng->Allocate_GetFileData( 0, &m_pMemoryData, cFileName );

		for( int i=0; i<FI_MAXCOUNT; ++i )
		{
			int nSize = GetSize_FromFaceIndex( i );
			error = FT_New_Memory_Face( m_library, (const FT_Byte*)m_pMemoryData, (int)nDataSize, 0, &m_Face[ i ] );
			assert_cs( error == 0 );


			error = FT_Set_Char_Size( m_Face[ i ], nSize*64, nSize*64, m_nLogPixelSX, m_nLogPixelSY );
			assert_cs( error == 0 );

			slot = m_Face[ i ]->glyph;

			m_nSize[ i ] = 0;
			for( int code=0; code<nLen ; ++code )
			{
				charcode = szCheck[ code ];

				FT_Load_Char( m_Face[ i ], charcode, FT_LOAD_RENDER );
				pMetrics = &(slot->metrics);
				nHeight = pMetrics->height;
				By = (pMetrics->vertAdvance - pMetrics->horiBearingY) - (pMetrics->vertAdvance / 6);

				if( m_nSize[ i ] < By + nHeight)
					m_nSize[ i ] = By + nHeight;
			}
			m_nSize[ i ] = ( m_nSize[ i ] >> 6 ) + 2;
		}
	}
	else
	{
		for( int i=0; i<FI_MAXCOUNT; ++i )
		{
			int nSize = GetSize_FromFaceIndex( i );
			if( FT_New_Face( m_library, cFileName, 0, &m_Face[ i ] ) != 0 )
			{
				return false;
			}
			if( FT_Set_Char_Size( m_Face[ i ], nSize*64, nSize*64, m_nLogPixelSX, m_nLogPixelSY ) != 0 )
			{
				return false;
			}

			slot = m_Face[ i ]->glyph;

			m_nSize[ i ] = 0;
			for( int code=0; code<nLen ; ++code )
			{
				charcode = szCheck[ code ];

				FT_Load_Char( m_Face[ i ], charcode, FT_LOAD_RENDER );
				pMetrics = &(slot->metrics);
				nHeight = pMetrics->height;
				By = (pMetrics->vertAdvance - pMetrics->horiBearingY) - (pMetrics->vertAdvance / 6);

				if( m_nSize[ i ] < By + nHeight)
					m_nSize[ i ] = By + nHeight;
			}
			m_nSize[ i ] = ( m_nSize[ i ] >> 6 ) + 2;
		}
	}	
	return true;
}


//=======================================================================================
//
//		페이스
//
//=======================================================================================
int CFreeFont::GetSize_FromFaceIndex( int nIndex )
{
	switch( nIndex )
	{
	case FI_8:		return FS_8;
	case FI_9:		return FS_9;
	case FI_10:		return FS_10;
	case FI_12:		return FS_12;
	case FI_13:		return FS_13;
	case FI_14:		return FS_14;
	case FI_16:		return FS_16;
	case FI_20:		return FS_20;
	case FI_24:		return FS_24;
	case FI_32:		return FS_32;
	}
	assert_cs( false );
	return 0;
}

int CFreeFont::GetIndex_FromFaceSize( int nSize )
{
	switch( nSize )
	{
	case FS_8:		return 	FI_8;
	case FS_9:		return 	FI_9;
	case FS_10:		return 	FI_10;
	case FS_12:		return 	FI_12;
	case FS_13:		return 	FI_13;
	case FS_14:		return 	FI_14;
	case FS_16:		return 	FI_16;
	case FS_20:		return 	FI_20;
	case FS_24:		return 	FI_24;
	case FS_32:		return 	FI_32;
	}
	assert_cs( false );
	return 0;
}

bool CFreeFont::CheckFontGlyph(TCHAR* str)
{
	for( int i = 0; i < (int)wcslen(str); i++ )
	{
		FT_ULong charcode = str[i];


		FT_UInt glyph_index = FT_Get_Char_Index( m_Face[ 0 ], charcode );
		FT_Load_Glyph( m_Face[ 0 ], glyph_index, FT_LOAD_DEFAULT );
		FT_Render_Glyph( m_Face[ 0 ]->glyph, FT_RENDER_MODE_NORMAL );

		if( !glyph_index )
		{
			return false;
		}
	}
	return true;
};



