#include "stdafx.h"
#include <afxinet.h>
#include "DownloadManager.h"

namespace CsDownloadMng
{
	eResult	DownloadToString( CString const& downloadUrl, std::string & outString )
	{
		// 인터넷 연결
		HINTERNET hSession = InternetOpen( L"Digitalic", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );	
		if ( hSession == NULL ) 
		{	// 인터넷 연결 실패
			return INTERNET_NOT_CONNECT;
		}

		HINTERNET hHttpFile = InternetOpenUrl( hSession, downloadUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
		if( hHttpFile == NULL )
		{			
			InternetCloseHandle( hHttpFile );
			InternetCloseHandle( hSession );
			return FILE_DOWNLOAD_FAIL;
		}

		// 연결 정보 확인
		TCHAR szStatusCode[ 128 ];
		DWORD dwCodeSize = 128;
		HttpQueryInfo( hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwCodeSize, NULL );
		long nStatusCode = _ttol( szStatusCode );
		if( nStatusCode != HTTP_STATUS_OK )
		{						
			InternetCloseHandle( hHttpFile );	
			InternetCloseHandle( hSession );	
			return INTERNET_STATUS_ERROR;
		}

		// 파일 용량 확인
		DWORD dwSize;
		InternetQueryDataAvailable( hHttpFile, &dwSize, 0, 0 );
		if( dwSize <= 0 )
		{
			InternetCloseHandle( hHttpFile );	
			InternetCloseHandle( hSession );	
			return FILE_SIZE_ZERO;
		}

		DWORD dwRead = 0;
		do 
		{
			char sz[ 65536 ] = { 0, };
			InternetReadFile( hHttpFile, sz, 65535, &dwRead );
			if( dwRead > 0 )
			{
				sz[ dwRead ] = 0; 
				outString += sz;
			}

		} while ( dwRead > 0 );

		InternetCloseHandle( hHttpFile );	
		InternetCloseHandle( hSession );

		return SUCCESS;
	}

	eResult	DownloadToString( CString const& downloadUrl, CString& outString )
	{
		std::string out;
		eResult result = DownloadToString( downloadUrl, out );

		outString = CA2CT(out.c_str());
				
		return result;
	}

	eResult	DownloadToFile( CString const& downloadUrl, CString const & outFileName )
	{
		// 인터넷 연결
		HINTERNET hSession = InternetOpen( L"Digitalic", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );	
		if ( hSession == NULL ) 
		{	// 인터넷 연결 실패
			return INTERNET_NOT_CONNECT;
		}

		HINTERNET hHttpFile = InternetOpenUrl( hSession, downloadUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
		if( hHttpFile == NULL )
		{			
			InternetCloseHandle( hHttpFile );
			InternetCloseHandle( hSession );
			return FILE_DOWNLOAD_FAIL;
		}

		// 연결 정보 확인
		TCHAR szStatusCode[ 128 ];
		DWORD dwCodeSize = 128;
		HttpQueryInfo( hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwCodeSize, NULL );
		long nStatusCode = _ttol( szStatusCode );
		if( nStatusCode != HTTP_STATUS_OK )
		{						
			InternetCloseHandle( hHttpFile );	
			InternetCloseHandle( hSession );	
			return INTERNET_STATUS_ERROR;
		}

		// 파일 용량 확인
		DWORD dwSize;
		InternetQueryDataAvailable( hHttpFile, &dwSize, 0, 0 );
		if( dwSize <= 0 )
		{
			InternetCloseHandle( hHttpFile );	
			InternetCloseHandle( hSession );	
			return FILE_SIZE_ZERO;
		}

		CFile pFile;	
		BOOL bSuccess = pFile.Open( outFileName, CFile::modeCreate | CFile::modeWrite );
		if( bSuccess == FALSE )
		{
			InternetCloseHandle( hHttpFile );	
			InternetCloseHandle( hSession );
			return FILE_CREATE_ERROR;
		}

		DWORD dwRead = 0;
		TCHAR* data = (TCHAR*)malloc( 65536 );
		do 
		{
			InternetReadFile( hHttpFile, data, 65535, &dwRead );
			if( dwRead > 0 )
				pFile.Write( data, dwRead );

		} while ( dwRead > 0 );

		free(data);

		InternetCloseHandle( hHttpFile );	
		InternetCloseHandle( hSession );

		return SUCCESS;
	}
}