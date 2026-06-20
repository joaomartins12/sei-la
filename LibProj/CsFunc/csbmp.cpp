
#include "stdafx.h"
#include <math.h>
#include "CsBmp.h"

bool CsBmp::DeleteData( sCS_BMPDATA* pData )
{
	assert_cs( pData != NULL );
	memset( &pData->s_InfoHeader, 0, sizeof( BITMAPINFOHEADER ) );
	SAFE_DELETE_ARRAY( pData->s_pData );

	return true;
}

bool CsBmp::GetBitmapSize( int& nWidth, int& nHeight, LPCTSTR szBitmap )
{
	if( _taccess_s( szBitmap, 0 ) != 0 )
		return false;

	HANDLE hFile = CreateFile( szBitmap, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	DWORD dwFileSize = GetFileSize( hFile, NULL );
	DWORD dwRead;
	BYTE* pFileData = csnew BYTE[ dwFileSize ];
	ReadFile( hFile, pFileData, dwFileSize, &dwRead, NULL );
	CloseHandle( hFile );
	DWORD dwOffSet = 0;
	BITMAPFILEHEADER* fh = (BITMAPFILEHEADER*)&pFileData[ dwOffSet ];
	dwOffSet += sizeof( BITMAPFILEHEADER );
	BITMAPINFOHEADER* ih = (BITMAPINFOHEADER*)&pFileData[ dwOffSet ];
	nWidth = ih->biWidth;
	nHeight = ih->biHeight;
	SAFE_DELETE_ARRAY( pFileData );

	return true;
}

bool CsBmp::GetBitmapBit( int& nBit, LPCTSTR szBitmap )
{
	if( _taccess_s( szBitmap, 0 ) != 0 )
		return false;

	HANDLE hFile = CreateFile( szBitmap, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	DWORD dwFileSize = GetFileSize( hFile, NULL );
	DWORD dwRead;
	BYTE* pFileData = csnew BYTE[ dwFileSize ];
	ReadFile( hFile, pFileData, dwFileSize, &dwRead, NULL );
	CloseHandle( hFile );
	DWORD dwOffSet = 0;
	BITMAPFILEHEADER* fh = (BITMAPFILEHEADER*)&pFileData[ dwOffSet ];
	dwOffSet += sizeof( BITMAPFILEHEADER );
	BITMAPINFOHEADER* ih = (BITMAPINFOHEADER*)&pFileData[ dwOffSet ];
	nBit = ih->biBitCount;
	SAFE_DELETE_ARRAY( pFileData );

	return true;
}

bool CsBmp::IsBmpData_GrayMap( LPCTSTR str )
{
	HANDLE hFile = CreateFile( str, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return false;

	// 사이즈정보 알아 낸다
	DWORD dwFileSize = GetFileSize( hFile, NULL );
	DWORD dwRead;

	// 사이즈 만큼 메모리 할당
	BYTE* pFileData = csnew BYTE[ dwFileSize ];

	// 파일로드
	ReadFile( hFile, pFileData, dwFileSize, &dwRead, NULL );
	CloseHandle( hFile );

	DWORD dwOffSet = 0;
	// 파일 헤더 로드
	BITMAPFILEHEADER* fh = (BITMAPFILEHEADER*)&pFileData[ dwOffSet ];
	dwOffSet += sizeof( BITMAPFILEHEADER );
	// 인포 헤더 로드
	BITMAPINFOHEADER* ih = (BITMAPINFOHEADER*)&pFileData[ dwOffSet ];
	dwOffSet += sizeof( BITMAPINFOHEADER );

	DWORD dwByte = (DWORD)sqrt( (double)ih->biClrUsed )/8;	// 픽셀당 바이트 카운트
	if( ih->biHeight < 0 )
	{
		assert_csm1( ih->biHeight < 0, _T( "탑 다운 : nHeight = %d, 원점 위쪽\n 프로그래머에게 문의~!" ), ih->biHeight );
		SAFE_DELETE_ARRAY( pFileData );
		return false;
	}

	if( dwByte == 0 )
	{
		dwByte = ih->biBitCount / 8;
	}
	if( dwByte != 1 )
	{
		assert_csm1( false, _T( "Height 맵이 1바이트가 아닙니다. Byte = %d" ), dwByte );
		SAFE_DELETE_ARRAY( pFileData );
		return false;
	}	
	SAFE_DELETE_ARRAY( pFileData );
	return true;
}


bool CsBmp::GetBmpData( LPCTSTR str, sCS_BMPDATA* pData )
{
	HANDLE hFile = CreateFile( str, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	assert_csm( hFile != INVALID_HANDLE_VALUE, _T( "비트맵 핸들이 존재 하지 않습니다." ) );

	// 사이즈정보 알아 낸다
	DWORD dwFileSize = GetFileSize( hFile, NULL );
	DWORD dwRead;

	// 사이즈 만큼 메모리 할당
	BYTE* pFileData = csnew BYTE[ dwFileSize ];

	// 파일로드
	ReadFile( hFile, pFileData, dwFileSize, &dwRead, NULL );
	CloseHandle( hFile );

	DWORD dwOffSet = 0;
	// 파일 헤더 로드
	BITMAPFILEHEADER* fh = (BITMAPFILEHEADER*)&pFileData[ dwOffSet ];
	dwOffSet += sizeof( BITMAPFILEHEADER );
	// 인포 헤더 로드
	BITMAPINFOHEADER* ih = (BITMAPINFOHEADER*)&pFileData[ dwOffSet ];
	dwOffSet += sizeof( BITMAPINFOHEADER );

	memcpy( &pData->s_InfoHeader, ih, sizeof( BITMAPINFOHEADER ) );

	DWORD dwByte = (DWORD)sqrt( (double)ih->biClrUsed )/8;	// 픽셀당 바이트 카운트

	if( ih->biHeight < 0 )
	{
		assert_csm1( ih->biHeight < 0, _T( "탑 다운 : nHeight = %d, 원점 위쪽\n 프로그래머에게 문의~!" ), ih->biHeight );
		SAFE_DELETE_ARRAY( pFileData );
		return false;
	}
	if( ih->biClrUsed != 0 )
	{
		assert_csm( false, _T( "인덱스 형식으로 저장 되면 안됩니다, 그레이 스케일 형식으로 저장" ) );
		SAFE_DELETE_ARRAY( pFileData );
		return false;
	}

	if( dwByte == 0 )
	{
		dwByte = ih->biBitCount / 8;
	}	

	// !!! 이후는 전부 1바이트라는 전제하에 간다.
	pData->s_pData = csnew BYTE[ ih->biWidth * ih->biHeight * dwByte ];

	// 실존 데이터 포인터
	BYTE* pRaster = (BYTE*)fh + fh->bfOffBits;

	// 가로 확장 사이즈 = 4의 배수
	DWORD dwWidthQuad = ih->biWidth + (( 4 - ih->biWidth%4 )%4);	

	for( int j=0; j<ih->biHeight; ++j )
	{
		for( int i=0; i<ih->biWidth; ++i )
		{
			for( DWORD bit=0; bit<dwByte; ++bit )
			{
				pData->s_pData[ ( j*ih->biWidth + i )*dwByte + bit ] = pRaster[ ( i + dwWidthQuad*j )*dwByte + bit ];
			}
		}
	}

	SAFE_DELETE_ARRAY( pFileData );
	return true;
}

bool CsBmp::GetBmpData_GrayMap( LPCTSTR str, sCS_BMPDATA* pData )
{
	HANDLE hFile = CreateFile( str, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	assert_csm( hFile != INVALID_HANDLE_VALUE, _T( "비트맵 핸들이 존재 하지 않습니다." ) );

	// 사이즈정보 알아 낸다
	DWORD dwFileSize = GetFileSize( hFile, NULL );
	DWORD dwRead;

	// 사이즈 만큼 메모리 할당
	BYTE* pFileData = csnew BYTE[ dwFileSize ];

	// 파일로드
	ReadFile( hFile, pFileData, dwFileSize, &dwRead, NULL );
	CloseHandle( hFile );

	DWORD dwOffSet = 0;
	// 파일 헤더 로드
	BITMAPFILEHEADER* fh = (BITMAPFILEHEADER*)&pFileData[ dwOffSet ];
	dwOffSet += sizeof( BITMAPFILEHEADER );
	// 인포 헤더 로드
	BITMAPINFOHEADER* ih = (BITMAPINFOHEADER*)&pFileData[ dwOffSet ];
	dwOffSet += sizeof( BITMAPINFOHEADER );

	memcpy( &pData->s_InfoHeader, ih, sizeof( BITMAPINFOHEADER ) );

	DWORD dwByte = (DWORD)sqrt( (double)ih->biClrUsed )/8;	// 픽셀당 바이트 카운트

	if( ih->biHeight < 0 )
	{
		assert_csm1( ih->biHeight < 0, _T( "탑 다운 : nHeight = %d, 원점 위쪽\n 프로그래머에게 문의~!" ), ih->biHeight );
		SAFE_DELETE_ARRAY( pFileData );
		return false;
	}
	if( ih->biClrUsed != 0 )
	{
		assert_csm( false, _T( "인덱스 형식으로 저장 되면 안됩니다, 그레이 스케일 형식으로 저장" ) );
		SAFE_DELETE_ARRAY( pFileData );
		return false;
	}

	if( dwByte == 0 )
	{
		dwByte = ih->biBitCount / 8;
	}
	if( dwByte != 1 )
	{
		assert_csm1( false, _T( "Height 맵이 1바이트가 아닙니다. Byte = %d" ), dwByte );
		SAFE_DELETE_ARRAY( pFileData );
		return false;
	}	

	// !!! 이후는 전부 1바이트라는 전제하에 간다.

	pData->s_pData = csnew BYTE[ ih->biWidth * ih->biHeight ];

	// 실존 데이터 포인터
	BYTE* pRaster = (BYTE*)fh + fh->bfOffBits;

	// 가로 확장 사이즈 = 4의 배수
	DWORD dwWidthQuad = ih->biWidth + (( 4 - ih->biWidth%4 )%4);	

	for( int j=0; j<ih->biHeight; ++j )
	{
		for( int i=0; i<ih->biWidth; ++i )
		{
			pData->s_pData[ j*ih->biWidth + i ] = pRaster[ i + dwWidthQuad*j  ];
		}
	}

	SAFE_DELETE_ARRAY( pFileData );
	return true;
}