#include "stdafx.h"

#include "DownloadManager.h"

namespace CsDownloadMng
{
	eResult	DownloadToString( std::string const& downloadUrl, std::string & outString )
	{
		// 인터넷 연결
		HINTERNET hSession = InternetOpen( L"Digitalic", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );	
		if ( hSession == NULL ) 
		{	// 인터넷 연결 실패
			BHPRT( "InternetOpen False" );
			return INTERNET_NOT_CONNECT;
		}

		HINTERNET hHttpFile = InternetOpenUrlA( hSession, downloadUrl.c_str(), NULL, 0, INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_RELOAD | INTERNET_FLAG_NO_CACHE_WRITE, 0 );
		if( hHttpFile == NULL )
		{			
			InternetCloseHandle( hSession );
			BHPRT( "InternetOpenUrlA False" );
			return FILE_DOWNLOAD_FAIL;
		}

		// 연결 정보 확인
		//TCHAR szStatusCode[ 128 ] = {0,};
		DWORD dwData;
		DWORD dwBufferLength = sizeof(dwData);		
		BOOL bRet = HttpQueryInfo( hHttpFile, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &dwData, &dwBufferLength, NULL );
		if(!bRet || dwData != HTTP_STATUS_OK)
		{
			InternetCloseHandle( hHttpFile );	
			InternetCloseHandle( hSession );	
			BHPRT( "HTTP_QUERY_STATUS_CODE False" );
			return INTERNET_STATUS_ERROR;
		}

		// 파일 용량 확인
		bRet = HttpQueryInfo( hHttpFile, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &dwData, &dwBufferLength, NULL);
		if(!bRet || dwData == 0)
		{
			InternetCloseHandle( hHttpFile );	
			InternetCloseHandle( hSession );	
			BHPRT( "HTTP_QUERY_CONTENT_LENGTH False Result(%d)", dwData );
			return FILE_SIZE_ZERO;
		}

		char* pBuffer = new char[dwData];
	
		INTERNET_BUFFERSA ib;
		ib.dwStructSize = sizeof(INTERNET_BUFFERSA);
		ib.lpvBuffer = pBuffer;
		ib.dwBufferLength = dwData;

		DWORD dwPosition = 0;
		bool bResult;
		do 
		{
			bResult = InternetReadFileExA(hHttpFile, &ib, WININET_API_FLAG_ASYNC, 1);
			if( !bResult && GetLastError() == ERROR_IO_PENDING )
			{
				BHPRT( "InternetReadFileExA ERROR_IO_PENDING");
				bResult = true;
			}

			dwPosition += ib.dwBufferLength;
			if( ib.dwBufferLength > 0 )
				outString += std::string((char*)ib.lpvBuffer, ib.dwBufferLength);

		} while ( bResult && ib.dwBufferLength != 0 );
		
// 		DWORD dwPosition = 0;
// 		do 
// 		{
// 			DWORD dwRead = 0;
// 			DWORD dwReadSize = 0;		
// 			InternetQueryDataAvailable( hHttpFile, &dwReadSize, 0, 0 );
// 			InternetReadFile( hHttpFile, pBuffer, dwReadSize, &dwRead );
// 
// 			dwPosition += dwRead;
// 
// 			if( dwRead > 0 )
// 				outString += std::string(pBuffer, dwRead);
// 			else
// 				break;
// 
// 		} while ( dwData > dwPosition );

		SAFE_DELETE_ARRAY( pBuffer );

		InternetCloseHandle( hHttpFile );	
		InternetCloseHandle( hSession );

		if( dwData != dwPosition )
		{
			BHPRT( "dwData(%d) != dwPosition(%d)", dwData, dwPosition );
			return FILE_DOWNLOAD_SIZE_NOT_SAME;
		}

		return SUCCESS;
	}

	eResult	DownloadToStringWhile( std::string const& downloadUrl, std::string & outString )
	{
		outString.clear();
		eResult re;
		int nErrorCount = 0;
		do 
		{
			re = DownloadToString( downloadUrl, outString );
			if( re == FILE_DOWNLOAD_SIZE_NOT_SAME )
			{
				nErrorCount++;
				Sleep(10);
				outString.clear();
			}

			if( nErrorCount > 5 )// 5회 시도 후에도 안되면 에러
			{
				BHPRT( "nErrorCount Over %d", nErrorCount );
				outString.clear();
				return FILE_DOWNLOAD_FAIL;
			}
		} while ( re == FILE_DOWNLOAD_SIZE_NOT_SAME );
		return re;
	}
}