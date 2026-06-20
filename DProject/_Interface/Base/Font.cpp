
#include "stdafx.h"
#include "Font.h"


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
	assert_cs( m_bGlobalInit == true );

	FT_Done_FreeType( m_library );
	m_bGlobalInit = false;
}

void CFont::GlobalInit()
{
	assert_cs( m_bGlobalInit == false );
	 

	FT_Error  error;
	error = FT_Init_FreeType( &m_library );
 
	assert_cs( error == 0 );

	HDC hdc = GetDC( GAMEAPP_ST.GetHWnd() );
	if( hdc )
	{
		m_nLogPixelSX = GetDeviceCaps( hdc, LOGPIXELSX );
		m_nLogPixelSY = GetDeviceCaps( hdc, LOGPIXELSY );
		ReleaseDC( GAMEAPP_ST.GetHWnd(), hdc );
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

	for( int i=0; i<FI_MAXCOUNT; ++i )
	{
		m_Face[ i ] = NULL;
	}
}

void CFont::Release()
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

bool CFont::Init( char const* cFileName )
{
	assert_cs( m_bInitialize == false );
	m_pMemoryData = NULL;
	m_bInitialize = true;

	FT_ULong charcode;
	TCHAR szCheck[] = _T( "bg불" );
	int nLen = 3;
	FT_GlyphSlot slot;
	FT_Glyph_Metrics *pMetrics;
	int nHeight, By;
	 
	// 파일 패킹
	
	if( CsFPS::CsFPSystem::IsExistOnlyPack( 0, cFileName ) )
	{
		FT_Error  error;
		 
		size_t nDataSize = CsFPS::CsFPSystem::Allocate_GetFileData( 0, &m_pMemoryData, cFileName );
		
		for( int i=0; i<FI_MAXCOUNT; ++i )
		{
#ifdef VERSION_TH
			FT_F26Dot6 nSize = GetSize_FromFaceIndex( i );
#else
			int nSize = GetSize_FromFaceIndex( i );
#endif
			
			error = FT_New_Memory_Face( m_library, (const FT_Byte*)m_pMemoryData, nDataSize, 0, &m_Face[ i ] );
			assert_cs( error == 0 );


#ifdef VERSION_USA	// 텍스트 겹침 현상 수정
			error = FT_Set_Char_Size( m_Face[ i ], nSize*68, nSize*64, m_nLogPixelSX, m_nLogPixelSY );
#else
			error = FT_Set_Char_Size( m_Face[ i ], nSize*64, nSize*64, m_nLogPixelSX, m_nLogPixelSY );
#endif
			assert_cs( error == 0 );

			slot = m_Face[ i ]->glyph;

			m_nSize[ i ] = 0;
			for( int code=0; code<nLen ; ++code )
			{
				charcode = szCheck[ code ];

				if( FT_Load_Char( m_Face[ i ], charcode, FT_LOAD_RENDER ) != 0 )
					continue;
				pMetrics = &(slot->metrics);
				nHeight = pMetrics->height;
				By = (pMetrics->vertAdvance - pMetrics->horiBearingY) - (pMetrics->vertAdvance / 6);

				if( m_nSize[ i ] < By + nHeight)
					m_nSize[ i ] = By + nHeight;
			}
			m_nSize[ i ] = ( m_nSize[ i ] >> 6 ) + 2;
			m_nHBase[ i ] = (int)( m_nSize[ i ]*0.75f );
		}
	}
	else
	{
		for( int i=0; i<FI_MAXCOUNT; ++i )
		{
#ifdef VERSION_TH
			FT_F26Dot6 nSize = GetSize_FromFaceIndex( i );
#else
			int nSize = GetSize_FromFaceIndex( i );
#endif
			if( FT_New_Face( m_library, cFileName, 0, &m_Face[ i ] ) != 0 )
			{
				return false;
			}
#ifdef VERSION_USA	// 텍스트 겹침 현상 수정
			if( FT_Set_Char_Size( m_Face[ i ], nSize*68, nSize*64, m_nLogPixelSX, m_nLogPixelSY ) != 0 )
#else
			if( FT_Set_Char_Size( m_Face[ i ], nSize*64, nSize*64, m_nLogPixelSX, m_nLogPixelSY ) != 0 )
#endif
			{
				return false;
			}

			slot = m_Face[ i ]->glyph;

			m_nSize[ i ] = 0;
			for( int code=0; code<nLen ; ++code )
			{
				charcode = szCheck[ code ];

				if( FT_Load_Char( m_Face[ i ], charcode, FT_LOAD_RENDER ) != 0 )
					continue;
				pMetrics = &(slot->metrics);
				nHeight = pMetrics->height;
				By = (pMetrics->vertAdvance - pMetrics->horiBearingY) - (pMetrics->vertAdvance / 6);

				if( m_nSize[ i ] < By + nHeight)
					m_nSize[ i ] = By + nHeight;
			}
			m_nSize[ i ] = ( m_nSize[ i ] >> 6 ) + 2;
			m_nHBase[ i ] = (int)( m_nSize[ i ]*0.75f );
		}
	}	
	return true;
}


//=======================================================================================
//
//		페이스
//
//=======================================================================================
int CFont::GetSize_FromFaceIndex( int nIndex ) const
{
	switch( nIndex )
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
	assert_cs( false );
	return 0;
}

int CFont::GetIndex_FromFaceSize( int nSize ) const
{
	switch( nSize )
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
	assert_cs( false );
	return 0;
}

bool CFont::CheckFontGlyph(TCHAR* str)
{
	if( str == NULL || m_Face[ 0 ] == NULL )
		return false;

	for( int i = 0; i < (int)wcslen(str); i++ )
	{
		FT_ULong charcode = str[i];


		FT_UInt glyph_index = FT_Get_Char_Index( m_Face[ 0 ], charcode );
#if ( defined VERSION_TW || defined VERSION_HK )
		// msjh.ttc 폰트에서 'j' 문자가 연속되면 공백이 전문자를 가리는 현상으로 'j'문자 일때 자동 힌팅 제거
		if( FT_Load_Glyph( m_Face[ 0 ], glyph_index, FT_LOAD_DEFAULT | FT_LOAD_NO_AUTOHINT ) != 0 )
			return false;
#else
		if( FT_Load_Glyph( m_Face[ 0 ], glyph_index, FT_LOAD_DEFAULT ) != 0 )
			return false;
#endif
		if( FT_Render_Glyph( m_Face[ 0 ]->glyph, FT_RENDER_MODE_NORMAL ) != 0 )
			return false;

		if( !glyph_index )
		{
			return false;
		}
	}
	return true;
};

