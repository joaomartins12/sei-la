
#include "stdafx.h"
#include "CsFile.h"

#include <commdlg.h>

TCHAR* nsCSFILE::ToLowerCase( TCHAR* In )
{
	int nLen = (int)_tcslen( In );
	for( int i=0; i<nLen; ++i )
	{
		if( ( In[ i ] >= L'A' )&&( In[ i ] <= L'Z' ) )
		{
			In[ i ] += 32;
		}
	}
	return In;
}

TCHAR* nsCSFILE::NewLine( TCHAR* Out, int nOutLen, TCHAR* In )
{
	_tcscpy_s( Out, nOutLen, In );
	assert_cs( (int)_tcslen( In ) <= nOutLen );

	TCHAR* pFind = _tcsstr( In, _T( "\\n" ) );
	if( pFind != NULL )
	{
		int nTotalLen = (int)_tcslen( In );
		int nFindLen = (int)_tcslen( pFind );
		int nFindPosLen = nTotalLen - nFindLen;
		assert_cs( nTotalLen >= nFindLen );

		Out[ nFindPosLen ] = _T( '\n' );
		Out[ nFindPosLen + 1 ] = NULL;
		if( nFindPosLen + 2 < nTotalLen )
			_tcscat_s( Out, nOutLen, &In[ nFindPosLen + 2 ] );
	}

	return Out;
}

LPCTSTR	nsCSFILE::GetFileExt( LPCTSTR In )
{
	int nLen = (int)_tcslen( In );
	for( int i=nLen-1; i>-1; --i )
	{
		if(In[ i ] == L'.')
		{
			return &In[ i + 1 ];
		}
	}
	return In;
}

LPCSTR	nsCSFILE::GetFileExt( LPCSTR In )
{
	int nLen = (int)strlen( In );
	for( int i=nLen-1; i>-1; --i )
	{
		if(In[ i ] == '.')
		{
			return &In[ i + 1 ];
		}
	}
	return In;
}

LPCTSTR	nsCSFILE::GetFileName( LPCTSTR In )
{
	int nLen = (int)_tcslen( In );
	for( int i=nLen-1; i>-1; --i )
	{
		if( (In[ i ] == L'/')||(In[ i ] == L'\\') )
		{
			return &In[ i + 1 ];
		}
	}
	return In;
}

LPCSTR nsCSFILE::GetFileName( LPCSTR In )
{
	int nLen = (int)strlen( In );
	for( int i=nLen-1; i>-1; --i )
	{
		if( (In[ i ] == '/')||(In[ i ] == '\\') )
		{
			return &In[ i + 1 ];
		}
	}
	return In;
}

LPTSTR nsCSFILE::GetFilePathOnly( LPTSTR Out, int nOutLen, LPCTSTR In, bool bIncludeFileName /*=false*/ )
{
	int nLen = (int)_tcslen( In );
	assert_csm2( nOutLen >= nLen, _T( "nsCSFILE::GetFilePathOnly : 입력 스트링이 출력 스트링보다 크기가 큽니다.\n입력 = %d\n출력 = %d" ), nLen, nOutLen );
	for( int i=nLen-1; i>-1; --i )
	{
		if( In[ i ] == L'.' )
		{
			bIncludeFileName = true;
		}
		if( ( bIncludeFileName == true )&&( (In[ i ] == L'/')||(In[ i ] == L'\\') ) )
		{
			_tcsncpy_s( Out, nOutLen, In, i );
			Out[ i ] = NULL;
			return Out;
		}
	}

	if( bIncludeFileName == false )
		_tcscpy_s( Out, nOutLen, In );
	else 
		_tcscpy_s( Out, nOutLen, _T( ".\\" ) );
	return Out;
}

LPSTR nsCSFILE::GetFilePathOnly( LPSTR Out, int nOutLen, LPCSTR In, bool bIncludeFileName /*=false*/ )
{
	int nLen = (int)strlen( In );
	assert_csm2( nOutLen >= nLen, _T( "nsCSFILE::GetFilePathOnly : 입력 스트링이 출력 스트링보다 크기가 큽니다.\n입력 = %d\n출력 = %d" ), nLen, nOutLen );
	for( int i=nLen-1; i>-1; --i )
	{
		if( In[ i ] == '.' )
		{
			bIncludeFileName = true;
		}
		if( ( bIncludeFileName == true )&&( (In[ i ] == '/')||(In[ i ] == '\\') ) )
		{
			strncpy_s( Out, nOutLen, In, i );
			Out[ i ] = NULL;
			return Out;
		}
	}
	if( bIncludeFileName == false )
		strcpy_s( Out, nOutLen, In );
	else
		strcpy_s( Out, nOutLen, ".\\" );
	
	return Out;
}

bool nsCSFILE::IsSameString( LPCTSTR str1, LPCTSTR str2 )
{
	static int nTemp = L'a' - L'A';
	int nLen1 = (int)_tcslen( str1 );
	int nLen2 = (int)_tcslen( str2 );
	if( nLen1 != nLen2 )
		return false;

	int nValue;
	for( int i=0; i<nLen1; ++i )
	{
		nValue = abs( str1[ i ] - str2[ i ] );
		if( ( nValue != 0 )&&( nValue != nTemp ) )
			return false;
	}
	return true;
}

bool nsCSFILE::IsSameString( LPCSTR str1, LPCSTR str2 )
{
	static int nTemp = 'a' - 'A';
	int nLen1 = (int)strlen( str1 );
	int nLen2 = (int)strlen( str2 );
	if( nLen1 != nLen2 )
		return false;

	int nValue;
	for( int i=0; i<nLen1; ++i )
	{
		nValue = abs( str1[ i ] - str2[ i ] );
		if( ( nValue != 0 )&&( nValue != nTemp ) )
			return false;
	}
	return true;
}

LPTSTR nsCSFILE::GetNumberStr( LPTSTR Out, int nOutLen, int nInNumber )
{
	TCHAR szNumber[ 16 ];
	_stprintf_s( szNumber, 16, _T( "%d" ), nInNumber );
	return GetNumberStr( Out, nOutLen, szNumber );
}

LPTSTR nsCSFILE::GetNumberStr( LPTSTR Out, int nOutLen, LPCTSTR In )
{
	size_t siSrc = _tcslen( In );
	size_t siComma = (siSrc-1)/3;

	int nFirstComma = (int)( siSrc%3 );
	if( nFirstComma == 0 )
		nFirstComma = 3;

	int nSrcIdx = 0;
	int nDestIdx = 0;
	_tcsncpy_s( Out, nOutLen, In, nFirstComma );
	Out[ nFirstComma ] = L',';
	nDestIdx += nFirstComma + 1;
	nSrcIdx += nFirstComma;

	int nMaxCommaIdx = (int)siComma + 1;
	for( int i=1; i<nMaxCommaIdx; ++i )
	{
		_tcsncpy_s( &Out[ nDestIdx ], nOutLen-nDestIdx, &In[ nSrcIdx ], 3 );
		Out[ nDestIdx + 3 ] = L',';
		nDestIdx += 4;
		nSrcIdx += 3;
		assert_cs( nDestIdx < nOutLen );
	}
	Out[ nDestIdx - 1 ] = NULL;
	return Out;
}

#include <shlobj.h>
//HRESULT SHPathToPidl( LPCTSTR szPath, int nPathLen, LPITEMIDLIST* ppidl )
//{
//	LPSHELLFOLDER pShellFolder = NULL;
//	OLECHAR wszPath[MAX_PATH] = {0};
//	ULONG nCharsParsed = 0;
//
//	HRESULT hr = SHGetDesktopFolder( &pShellFolder );
//	if( FAILED(hr) )
//		return FALSE;
//
//	_tcscpy_s( wszPath, nPathLen, szPath );
//
//	hr = pShellFolder->ParseDisplayName( NULL, NULL, wszPath, &nCharsParsed, ppidl, NULL );
//	pShellFolder->Release();
//	return hr;
//}

TCHAR g_szBrowerInitPath[ MAX_PATH ];
int CALLBACK BrowseCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)            // 초기화 될 때
	{
		// 시작 폴더 지정
		// 레지스트리에서 로드할 경우.     // 전역 스트링 등 다른 곳에서 가져와도 무방 ^^;
		// 시작 폴더 선택 메시지 // hwnd(아마 리스트 컨트롤)에 선택 메시지 날림.
#pragma warning( disable : 4311 )
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (DWORD)g_szBrowerInitPath);
#pragma warning( default : 4311 )
	}
	return 0;
}


bool nsCSFILE::DirectoryOpen( LPTSTR szOutPath, UINT nOutPathLen, LPCTSTR szInitDir )
{
	assert_cs( szOutPath );

	memset( szOutPath, 0, sizeof( TCHAR )*nOutPathLen );
	_tcscpy_s( g_szBrowerInitPath, MAX_PATH, szInitDir );

	BROWSEINFO bi;
	memset( &bi, 0, sizeof( BROWSEINFO ));

	bi.hwndOwner		= NULL;
	bi.ulFlags			= BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN | BIF_VALIDATE;
	bi.pszDisplayName	= szOutPath;
	bi.pidlRoot			= NULL;
	bi.lpfn				= BrowseCallback;

	LPITEMIDLIST pildBrowse = ::SHBrowseForFolder(&bi);
	if( pildBrowse == NULL )
		return false;

	SHGetPathFromIDList(pildBrowse, szOutPath);
	if( szOutPath[ 0 ] == NULL )
		return false;
	return true;
}


/*
nsCSFILE::FileOpen( szPath, MAX_PATH, _T( "Image File(bmp,tga,dds)\0*.bmp;*.tga;*.dds\0" ), 시작 디렉토리 );
*/
bool nsCSFILE::FileOpen(	LPTSTR		szOutPath,
							UINT		nOutPathLen,
							LPCTSTR		szFilter,		// ext. "Height Map File(*.bmp)\0*.bmp\0"
							LPCTSTR		szInitDir,
							LPTSTR		szOutFile		/*=NULL*/,
							UINT		nOutFileLen		/*=0*/,
							LPTSTR		szOutTitle		/*=NULL*/,
							UINT		nOutTitleLen	/*=0*/,
							DWORD		dwAddFlags		/*=0*/	)
{
	assert_cs( szOutPath );

	memset( szOutPath, 0, sizeof( TCHAR )*nOutPathLen );

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof( OPENFILENAME ));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner		= NULL;
	ofn.lpstrFile		= szOutPath;
	ofn.nMaxFile		= nOutPathLen;
	ofn.lpstrFilter		= szFilter;	
	ofn.lpstrInitialDir = szInitDir;
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.Flags			|= dwAddFlags;

	if( GetOpenFileName(&ofn) == FALSE )
		return false;

	if( szOutPath[0] == NULL)
		return false;

	if( szOutFile != NULL )
	{
		LPCTSTR psz = GetFileName( szOutPath );
		assert_cs( _tcslen( psz ) < nOutFileLen );
		_stprintf_s( szOutFile, nOutFileLen, psz );
	}
	if( szOutTitle != NULL )
	{
		LPCTSTR psz = GetFileName( szOutPath );
		assert_cs( _tcslen( psz ) < nOutTitleLen );
		_stprintf_s( szOutTitle, nOutTitleLen, psz );

		int nLen = (int)_tcslen( szOutTitle );
		for( int i=nLen-1; i>-1; --i )
		{
			if( szOutTitle[ i ] == L'.' )
			{
				szOutTitle[ i ] = NULL;
				break;
			}
		}
	}

	return true;
}

bool nsCSFILE::FileSave(	LPTSTR		szOutPath,
							UINT		nOutPathLen,
							LPCTSTR		szFilter,		// ext. "Height Map File(*.bmp)\0*.bmp\0"
							LPCTSTR		szInitDir,
							LPTSTR		szOutFile		/*=NULL*/,
							UINT		nOutFileLen		/*=0*/,
							LPTSTR		szOutTitle		/*=NULL*/,
							UINT		nOutTitleLen	/*=0*/,
							DWORD		dwAddFlags		/*=0*/	)
{
	assert_cs( szOutPath );

	OPENFILENAME ofn;
	memset( &ofn, 0, sizeof( OPENFILENAME ));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner		= NULL;
	ofn.lpstrFile		= szOutPath;
	ofn.nMaxFile		= nOutPathLen;
	ofn.lpstrFilter		= szFilter;	
	ofn.lpstrInitialDir = szInitDir;
	ofn.Flags			= OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
	ofn.Flags			|= dwAddFlags;
	if( GetSaveFileName(&ofn) == FALSE )
		return false;

	if( szOutPath[0] == NULL)
		return false;

	if( szOutFile != NULL )
	{
		LPCTSTR psz = GetFileName( szOutPath );
		assert_cs( _tcslen( psz ) < nOutFileLen );
		_stprintf_s( szOutFile, nOutFileLen, psz );
	}
	if( szOutTitle != NULL )
	{
		LPCTSTR psz = GetFileName( szOutPath );
		assert_cs( _tcslen( psz ) < nOutTitleLen );
		_stprintf_s( szOutTitle, nOutTitleLen, psz );

		int nLen = (int)_tcslen( szOutTitle );
		for( int i=nLen-1; i>-1; ++i )
		{
			if( szOutTitle[ i ] == L'.' )
			{
				szOutTitle[ i ] = NULL;
				break;
			}
		}
	}

	// 한종류의 파일이 있을경우만 해당 ==============================================

	// Ext 검사후 없다면 넣어줘라
	TCHAR ext[ 10 ] = {0, };
	int nLen = (int)_tcslen( szFilter );
	for( int i=nLen-1; i>-1; --i )
	{
		if( szFilter[ i ] == L'.')
		{
			_sntprintf_s( ext, 10, nLen - i - 2, &szFilter[ i + 1 ] );		// 끝부분의 ) 제외
			break;
		}
	}

	nLen = (int)_tcslen( szOutPath );
	for( int i=nLen-1; i>-1; --i )
	{
		if( (szOutPath[ i ] == L'/')||(szOutPath[ i ] == L'\\') )
		{
			_tcscat_s( szOutPath, nOutPathLen, _T( "." ) );
			_tcscat_s( szOutPath, nOutPathLen, ext );
			break;
		}
		else if( szOutPath[ i ] == L'.')
		{
			break;
		}
	}

	return true;
}

void nsCSFILE::CreateDirectory( TCHAR* szRelativePath )
{
	while( _taccess_s( szRelativePath, 0 ) != 0 ) 
	{
		TCHAR szFolderSrc[ MAX_PATH ];
		TCHAR szFolderDest[ MAX_PATH ];
		nsCSFILE::GetFilePathOnly( szFolderSrc, MAX_PATH, szRelativePath, true );
		_tcscpy_s( szFolderDest, MAX_PATH, szRelativePath );
		while( _taccess_s( szFolderSrc, 0 ) != 0 )
		{
			_tcscpy_s( szFolderDest, MAX_PATH, szFolderSrc );
			nsCSFILE::GetFilePathOnly( szFolderSrc, MAX_PATH, szFolderDest, true );
		}
		::CreateDirectory( szFolderDest, NULL );
	}
}

void nsCSFILE::CreateDirectory( char* cRelativePath )
{
	while( _access_s( cRelativePath, 0 ) != 0 ) 
	{
		char cFolderSrc[ MAX_PATH ];
		char cFolderDest[ MAX_PATH ];
		nsCSFILE::GetFilePathOnly( cFolderSrc, MAX_PATH, cRelativePath, true );
		strcpy_s( cFolderDest, MAX_PATH, cRelativePath );
		while( _access_s( cFolderSrc, 0 ) != 0 )
		{
			strcpy_s( cFolderDest, MAX_PATH, cFolderSrc );
			nsCSFILE::GetFilePathOnly( cFolderSrc, MAX_PATH, cFolderDest, true );
		}
		::CreateDirectoryA( cFolderDest, NULL );
	}
}

BOOL nsCSFILE::DeleteFile(  TCHAR* szDelFile )
{
	if( _waccess_s( szDelFile, 0 ) != 0 )
		return false;

	::SetFileAttributesW( szDelFile, FILE_ATTRIBUTE_NORMAL );
	return	::DeleteFileW( szDelFile );	
}

BOOL nsCSFILE::DeleteFileA( char const* szDelFile )
{
	if( _access_s( szDelFile, 0 ) != 0 )
		return false;

	::SetFileAttributesA( szDelFile, FILE_ATTRIBUTE_NORMAL );
	return	::DeleteFileA( szDelFile );	
}

BOOL nsCSFILE::DeletePathTillRoot( std::list<std::string> const& delFileList, LPCSTR pRootName )
{	
	char szFind[ MAX_PATH ] = {0,};

	std::list<std::string> findFile;

	std::list<std::string>::const_iterator it = delFileList.begin();
	for( ; it != delFileList.end(); ++it )
	{
		if( _access_s( (*it).c_str(), 0 ) != 0 )
			return FALSE;

		::SetFileAttributesA( (*it).c_str(), FILE_ATTRIBUTE_NORMAL );
		::DeleteFileA( (*it).c_str() );	
				
		GetFilePathOnly( szFind, MAX_PATH, (*it).c_str(), true );
		DirectoryFindFilesA( findFile, szFind );
		if( findFile.empty() )
		{
			char szDelDir[MAX_PATH] = {0,};
			do 
			{
				DeleteDirectoryA( szFind );
 				nsCSFILE::GetFilePathOnly( szDelDir, MAX_PATH, szFind, true );
 				strcpy_s(szFind, MAX_PATH, szDelDir );	
				DirectoryFindFilesA( findFile, szFind );
				if( !findFile.empty() )
					break;

			} while ( !IsSameString( szFind, pRootName ) );
		}
		findFile.clear();
	}

	return TRUE;
}

void nsCSFILE::DeleteDirectory( TCHAR* szRelativePath )
{
	TCHAR path[ MAX_PATH ];
	_stprintf_s( path, MAX_PATH,  L"%s/*.*", szRelativePath );

	int nLen;
	WIN32_FIND_DATA wfd;
	HANDLE hSrch = ::FindFirstFileW( path, &wfd );
	TCHAR dir[ MAX_PATH ] = {0, };	

	if( hSrch != INVALID_HANDLE_VALUE )
	{
		do 
		{
			// 디렉토리가 아니라면
			if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FALSE )
			{
				nLen = (int)_tcslen( path ) - 3;
				_tcsncpy_s( dir, MAX_PATH, path, nLen );
				_tcscat_s( dir, MAX_PATH, wfd.cFileName );						

				DeleteFile( dir );
			}
			// 디렉토리라면 재귀호출
			else
			{
				if( ( _tcsicmp( wfd.cFileName, _T( "." ) ) != 0 )&&
					( _tcsicmp( wfd.cFileName, _T( ".." ) ) != 0 ) )
				{
					nLen = (int)_tcslen( path ) - 3;
					_tcsncpy_s( dir, MAX_PATH, path, nLen );
					_tcscat_s( dir, MAX_PATH, wfd.cFileName );

					DeleteDirectory( dir );
				}
			}
		} while ( TRUE == ::FindNextFileW( hSrch, &wfd ) );
		::FindClose( hSrch );
	}

	::RemoveDirectoryW( szRelativePath );
}

void nsCSFILE::DeleteDirectoryA( LPCSTR szRelativePath )
{
	char path[ MAX_PATH ];
	sprintf_s( path, MAX_PATH,  "%s/*.*", szRelativePath );

	int nLen;
	WIN32_FIND_DATAA wfd;
	HANDLE hSrch = ::FindFirstFileA( path, &wfd );
	char dir[ MAX_PATH ] = {0, };	

	if( hSrch != INVALID_HANDLE_VALUE )
	{
		do 
		{
			// 디렉토리가 아니라면
			if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FALSE )
			{
				nLen = (int)strlen( path ) - 3;
				strncpy_s( dir, MAX_PATH, path, nLen );
				strcat_s( dir, MAX_PATH, wfd.cFileName );						

				DeleteFileA( dir );
			}
			// 디렉토리라면 재귀호출
			else
			{
				
				if( ( _stricmp( wfd.cFileName, "." ) != 0 )&&
					( _stricmp( wfd.cFileName, ".." ) != 0 ) )
				{
					nLen = (int)strlen( path ) - 3;
					strncpy_s( dir, MAX_PATH, path, nLen );
					strcat_s( dir, MAX_PATH, wfd.cFileName );

					DeleteDirectoryA( dir );
				}
			}
		} while ( TRUE == ::FindNextFileA( hSrch, &wfd ) );
		::FindClose( hSrch );
	}

	::RemoveDirectoryA( szRelativePath );
}

bool nsCSFILE::IsSubDirectory( LPCTSTR szCheckPath )
{
	assert_cs( szCheckPath != NULL );
	
	TCHAR szCurDir[ MAX_PATH ] = {0, };
	GetCurrentDirectory( MAX_PATH, szCurDir );
	int nCurDirLen = (int)_tcslen( szCurDir );

	if( (int)_tcslen( szCheckPath ) < nCurDirLen )
		return false;

	for( int i=0; i<nCurDirLen; ++i )
	{
		if( szCurDir[ i ] != szCheckPath[ i ] )
		{
			// 대소문자 구별 안한다.
			if( abs( szCurDir[ i ] - szCheckPath[ i ] ) != 32 )
			{				
				return false;
			}			
		}
	}
	return true;
}

LPTSTR nsCSFILE::GetRelativePath( LPTSTR szOutPath, int nOutSize, LPCTSTR szSubDirectory )
{
	//if( ( szSubDirectory[ 0 ] == L'.' )||( szSubDirectory[ 0 ] == L'\\' ) )
	if( szSubDirectory[ 1 ] != L':' )
	{
		_tcscpy_s( szOutPath, nOutSize, szSubDirectory );
		return szOutPath;
	}

	assert_cs( szOutPath != NULL );
	assert_cs( szSubDirectory != NULL );

	TCHAR szCurDir[ MAX_PATH ] = {0, };
	GetCurrentDirectory( MAX_PATH, szCurDir );
	int nCurDirLen = (int)_tcslen( szCurDir );

	if( nCurDirLen == _tcslen( szSubDirectory ) )
	{
		_stprintf_s( szOutPath, nOutSize, _T( "" ) );
		return szOutPath;
	}
	
	_stprintf_s( szOutPath, nOutSize, _T( "%s" ), &szSubDirectory[ nCurDirLen + 1 ] );
	return szOutPath;
}

char* nsCSFILE::GetRelativePath( char* cOutPath, int nOutSize, char* cSubDirectory )
{
	if( cSubDirectory[ 1 ] != ':' )
	{
		strcpy_s( cOutPath, nOutSize, cSubDirectory );
		return cOutPath;
	}

	assert_cs( cOutPath!= NULL );
	assert_cs( cSubDirectory != NULL );

	char cCurDir[ MAX_PATH ] = {0, };
	GetCurrentDirectoryA( MAX_PATH, cCurDir );
	int nCurDirLen = (int)strlen( cCurDir );

	if( nCurDirLen == strlen( cSubDirectory ) )
	{
		sprintf_s( cOutPath, nOutSize, "" );
		return cOutPath;
	}

	sprintf_s( cOutPath, nOutSize, "%s", &cSubDirectory[ nCurDirLen + 1 ] );
	return cOutPath;
}

LPCTSTR nsCSFILE::GetSubPath( LPCTSTR szIn, LPCTSTR szSubDirectory )
{
	int nOffset = 0;
	if( szIn[ 0 ] == L'.' )
		nOffset += 2;
	if( _tcsnicmp( szSubDirectory, &szIn[ nOffset ], _tcslen( szSubDirectory ) ) == 0 )
		nOffset += (int)_tcslen( szSubDirectory );

	return &szIn[ nOffset ];
}

#include "ShellApi.h"
int nsCSFILE::DeleteFolder( TCHAR* cFolder )
{
	SHFILEOPSTRUCT FileOp = {0};//쉘 구조체 
	ZeroMemory (&FileOp, sizeof(SHFILEOPSTRUCT));

	TCHAR delFolder[_MAX_PATH];
	_tcscpy_s( delFolder, _MAX_PATH, cFolder );

	FileOp.hwnd = NULL;
	FileOp.wFunc = FO_DELETE;//지우고 
	FileOp.pFrom = delFolder;//인자로 받은 폴더

	FileOp.pTo = NULL;//복사할때 사용
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT ;//화면에 메세지 표시 않함

	//뭔지 알고 싶으시면 지우고 해보세요
	FileOp.fAnyOperationsAborted = false;//나머진 뭔지 모르겠네요
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = NULL;

	return SHFileOperation(&FileOp);//구조체를 실행
}

int nsCSFILE::DeleteFolder( char const* cFolder )
{
	SHFILEOPSTRUCTA FileOp;//쉘 구조체 
	ZeroMemory (&FileOp, sizeof(SHFILEOPSTRUCTA));

	CHAR delFolder[_MAX_PATH];
	strcpy_s( delFolder, _MAX_PATH, cFolder );
	
	FileOp.hwnd = NULL;
	FileOp.wFunc = FO_DELETE;//지우고 
	FileOp.pFrom = delFolder;//인자로 받은 폴더

	FileOp.pTo = NULL;//복사할때 사용
	FileOp.fFlags = FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT ;//화면에 메세지 표시 않함

	//뭔지 알고 싶으시면 지우고 해보세요
	FileOp.fAnyOperationsAborted = false;//나머진 뭔지 모르겠네요
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = NULL;

	return SHFileOperationA(&FileOp);//구조체를 실행
}


char* _FindNumber( char* sz, char cToken )
{
	int nLen = (int)strlen( sz );
	for( int i=0; i<nLen; ++i )
	{
		if( sz[ i ] == cToken )
		{
			return &sz[ i + 1 ];
		}
	}
	return sz;
}

tm nsCSFILE::Str2Time( char* sz, tm& tm )
{
	tm.tm_year = abs( atoi( sz ) );
	tm.tm_year -= 1900;

	sz = _FindNumber( sz, '-' );
	tm.tm_mon = abs( atoi( sz ) );
	tm.tm_mon -= 1;

	sz = _FindNumber( sz, '-' );
	tm.tm_mday = abs( atoi( sz ) );

	sz = _FindNumber( sz, ' ' );
	tm.tm_hour = abs( atoi( sz ) );

	sz = _FindNumber( sz, ':' );
	tm.tm_min = abs( atoi( sz ) );

	sz = _FindNumber( sz, ':' );
	tm.tm_sec = abs( atoi( sz ) );

	return tm;
}

UINT nsCSFILE::Tm2Sec( tm time )
{
	UINT nRes = 0;
	nRes += time.tm_sec;
	nRes += time.tm_min*60;
	nRes += time.tm_hour*60*60;
	nRes += time.tm_mday*60*60*24;
	nRes += time.tm_mon*60*60*24*31;
	nRes += time.tm_year*60*60*24*31*12;
	return nRes;
}

nsCSFILE::sTOKEN_c::sTOKEN_c( const char* cSrc )
{
	assert_cs( cSrc != NULL );
	s_pSrc = cSrc;
	s_nSrcBufferLen = (int)strlen( s_pSrc );
	s_nSrcBufferIndex = 0;

	s_AppState = APP_NONE;
}

nsCSFILE::sTOKEN_c::sTOKEN_c( const char* cSrc, const char* cStartApp, const char* cEndApp )
{
	assert_cs( cSrc != NULL );
	s_pSrc = cSrc;
	s_nSrcBufferLen = (int)strlen( s_pSrc );
	s_nSrcBufferIndex = 0;

	s_cStartApp = cStartApp;
	s_cEndApp = cEndApp;
	s_AppState = APP_NONE_START;
}

char* nsCSFILE::sTOKEN_c::Next( char* cOut, int nOutBufferLen )
{
	static int nSrcChar;
	int nOutBufferIndex = 0;
	while( s_nSrcBufferIndex < s_nSrcBufferLen )
	{
		nSrcChar = s_pSrc[ s_nSrcBufferIndex ];
		++s_nSrcBufferIndex;
		if( ( nSrcChar == ',' )||( nSrcChar == '\n' ) )
		{
			cOut[ nOutBufferIndex ] = NULL;
			return cOut;
		}
		// 숫자와 영문자만 인정
		/*if( ( ( nSrcChar >= 'A' )&&( nSrcChar <= 'z') ) ||
			( ( nSrcChar >= '0' )&&( nSrcChar <= '9') ) ||
			( nSrcChar == '_' ) ||
			( nSrcChar == '.' )		)*/
		{
			assert_csm( nOutBufferIndex < nOutBufferLen, _T( "sTOKEN_c::Next의 출력 버퍼 사이즈가 너무 작습니다." ) );
			cOut[ nOutBufferIndex ] = nSrcChar;
			++nOutBufferIndex;
		}	
	}

	cOut[ nOutBufferIndex ] = NULL;
	return nOutBufferIndex == 0 ? NULL : cOut;
}

char* nsCSFILE::sTOKEN_c::Next( char* cOut, int nOutBufferLen, const char* cKey, const char* cSeparation, size_t siGarbageSize /*=0*/ )
{
	if( s_AppState == APP_END )
		return NULL;

	assert_cs( cSeparation != NULL );


	size_t siKeyLen = strlen( cKey );

	while( s_nSrcBufferIndex < s_nSrcBufferLen )
	{
		if( s_AppState != APP_NONE )
		{
			if( ( s_AppState == APP_NONE_START )&&
				( strncmp( &s_pSrc[ s_nSrcBufferIndex ], s_cStartApp, strlen( s_cStartApp ) ) == 0 ) )
			{
				s_AppState = APP_START;
				s_nSrcBufferIndex += (int)strlen( s_cStartApp );
			}
			else if( ( s_AppState == APP_START )&&
					 ( strncmp( &s_pSrc[ s_nSrcBufferIndex ], s_cEndApp, strlen( s_cEndApp ) ) == 0 ) )
			{
				s_AppState = APP_END;
				return NULL;
			}
		}

		if( ( ( s_AppState == APP_NONE )||( s_AppState == APP_START ) )&&
			( strncmp( &s_pSrc[ s_nSrcBufferIndex ], cKey, siKeyLen ) == 0 ) )
		{
			s_nSrcBufferIndex += (int)siKeyLen;
			int nStartIndex = 0;
			int nEndIndex = 0;
			while( s_nSrcBufferIndex < s_nSrcBufferLen )
			{
				if( strncmp( &s_pSrc[ s_nSrcBufferIndex ], cSeparation, strlen( cSeparation ) ) == 0 )
				{
					nStartIndex = s_nSrcBufferIndex + 1;
					++s_nSrcBufferIndex;
					break;
				}
				++s_nSrcBufferIndex;
			}
			while( s_nSrcBufferIndex < s_nSrcBufferLen )
			{
				if( strncmp( &s_pSrc[ s_nSrcBufferIndex ], cSeparation, strlen( cSeparation ) ) == 0 )
				{
					nEndIndex = s_nSrcBufferIndex;
					++s_nSrcBufferIndex;
					break;
				}
				++s_nSrcBufferIndex;
			}
			assert_cs( nStartIndex != 0 );
			assert_cs( nEndIndex != 0 );
			strncpy_s( cOut, nOutBufferLen, &s_pSrc[ nStartIndex+siGarbageSize ], nEndIndex - nStartIndex );
			return cOut;
		}

		++s_nSrcBufferIndex;
	}
	return NULL;
}

nsCSFILE::sTOKEN::sTOKEN( const TCHAR* cSrc )
{
	assert_cs( cSrc != NULL );
	s_pSrc = cSrc;
	s_nSrcBufferLen = (int)_tcslen( s_pSrc );
	s_nSrcBufferIndex = 0;
}

TCHAR* nsCSFILE::sTOKEN::Next( TCHAR* cOut, int nOutBufferLen )
{
	static int nSrcChar;
	int nOutBufferIndex = 0;
	while( s_nSrcBufferIndex < s_nSrcBufferLen )
	{
		nSrcChar = s_pSrc[ s_nSrcBufferIndex ];
		++s_nSrcBufferIndex;
		if( ( nSrcChar == L',' )||( nSrcChar == L'\n' ) )
		{
			cOut[ nOutBufferIndex ] = NULL;
			return cOut;
		}
		//// 숫자와 영문자만 인정
		//if( ( ( nSrcChar >= L'A' )&&( nSrcChar <= L'z') ) ||
		//	( ( nSrcChar >= L'0' )&&( nSrcChar <= L'9') ) ||
		//	( nSrcChar == L'_' ) ||
		//	( nSrcChar == L'.' )		)
		{
			assert_csm( nOutBufferIndex < nOutBufferLen, _T( "sTOKEN::Next의 출력 버퍼 사이즈가 너무 작습니다." ) );
			cOut[ nOutBufferIndex ] = nSrcChar;
			++nOutBufferIndex;
		}	
	}

	cOut[ nOutBufferIndex ] = NULL;
	return nOutBufferIndex == 0 ? NULL : cOut;
}

TCHAR* nsCSFILE::sTOKEN::Next_NewLine( TCHAR* cOut, int nOutBufferLen )
{
	static int nSrcChar;
	int nOutBufferIndex = 0;
	while( s_nSrcBufferIndex < s_nSrcBufferLen )
	{
		nSrcChar = s_pSrc[ s_nSrcBufferIndex ];
		++s_nSrcBufferIndex;
		if( nSrcChar == L',' )
		{
			cOut[ nOutBufferIndex ] = NULL;
			return cOut;
		}
		//// 숫자와 영문자만 인정
		//if( ( ( nSrcChar >= L'A' )&&( nSrcChar <= L'z') ) ||
		//	( ( nSrcChar >= L'0' )&&( nSrcChar <= L'9') ) ||
		//	( nSrcChar == L'_' ) ||
		//	( nSrcChar == L'.' )		)
		{
			assert_csm( nOutBufferIndex < nOutBufferLen, _T( "sTOKEN::Next의 출력 버퍼 사이즈가 너무 작습니다." ) );
			cOut[ nOutBufferIndex ] = nSrcChar;
			++nOutBufferIndex;
		}	
	}

	cOut[ nOutBufferIndex ] = NULL;
	return nOutBufferIndex == 0 ? NULL : cOut;
}

int nsCSFILE::sTOKEN::NextInt( TCHAR* cOut, int nOutBufferLen )
{
	Next( cOut, nOutBufferLen );
	if( cOut[ 0 ] != NULL )
		return _ttoi( cOut );
	return 0;
}

TCHAR* nsCSFILE::GetClipboardFont( HWND hWnd, TCHAR* szReturn, int nReturnSize )
{
	szReturn[ 0 ] = NULL;

	if( OpenClipboard( hWnd ) == false )
	{		
		return szReturn;
	}

	// 클립보드상에 텍스트가 존재 한다면
	if( IsClipboardFormatAvailable( CF_TEXT ) || IsClipboardFormatAvailable( CF_OEMTEXT ) )
	{
		HANDLE hClipBoardData = GetClipboardData( CF_TEXT );
		assert_cs( hClipBoardData != NULL );
		char* cData = (char*)GlobalLock( hClipBoardData );
		GlobalUnlock( hClipBoardData );

		if( (int)strlen( cData ) + 1 < nReturnSize )
		{
			M2W( szReturn, cData, nReturnSize );
		}
		else
		{
			char* cSrc = csnew char[ nReturnSize ];
			strncpy_s( cSrc, nReturnSize, cData, nReturnSize-1 );
			M2W( szReturn, cSrc, nReturnSize );
			delete[] cSrc;
		}
	}
	CloseClipboard();

	return szReturn;
}

void nsCSFILE::SetClipboardFont( HWND hWnd, TCHAR* szInput, int nInputSize )
{
	if( OpenClipboard( hWnd ) == false )
		return;

	EmptyClipboard();
	HGLOBAL hClipboardData = GlobalAlloc( GMEM_DDESHARE, nInputSize );
	char* cInput = (char*)GlobalLock( hClipboardData );
	W2M( cInput, szInput, nInputSize );
	GlobalUnlock( hClipboardData );
	SetClipboardData( CF_TEXT, hClipboardData );

	CloseClipboard();
}

void nsCSFILE::DirectoryFindFilesA( std::list<std::string>& findFileList, char const* pFindFolder )
{
	std::string folder = pFindFolder;
	folder += "\\*.*";

	WIN32_FIND_DATAA kFindData;
	HANDLE hFindFile = ::FindFirstFileA( folder.c_str(), &kFindData );
	if( hFindFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( (kFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)// 디렉토리일 때
			{
				if(strcmp(kFindData.cFileName,".") && strcmp(kFindData.cFileName,".."))
				{
					std::string strFindFile = pFindFolder;
					strFindFile += "\\";
					strFindFile += kFindData.cFileName;
					DirectoryFindFilesA( findFileList, strFindFile.c_str() );
				}
			}
			else
			{
				std::string strFindFile = pFindFolder;
				strFindFile += "\\";
				strFindFile += kFindData.cFileName;
				findFileList.push_back(strFindFile);
			}
		}while( TRUE == ::FindNextFileA( hFindFile, &kFindData ) );

		::FindClose( hFindFile );
	}
}

void nsCSFILE::DirectoryFindFilesW( std::list<std::wstring>& findFileList, wchar_t const* pFindFolder )
{
	std::wstring folder = pFindFolder;
	folder += L"\\*.*";

	WIN32_FIND_DATAW kFindData;
	HANDLE hFindFile = ::FindFirstFileW( folder.c_str(), &kFindData );
	if( hFindFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( (kFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)// 디렉토리일 때
			{
				if(wcscmp(kFindData.cFileName,L".") && wcscmp(kFindData.cFileName,L".."))
				{
					std::wstring strFindFile = pFindFolder;
					strFindFile += L"\\";
					strFindFile += kFindData.cFileName;
					DirectoryFindFilesW( findFileList, strFindFile.c_str() );
				}
			}
			else
			{
				std::wstring strFindFile = pFindFolder;
				strFindFile += L"\\";
				strFindFile += kFindData.cFileName;
				findFileList.push_back(strFindFile);
			}
		}while( TRUE == ::FindNextFileW( hFindFile, &kFindData ) );

		::FindClose( hFindFile );
	}
}

void nsCSFILE::FindFilesA( char const* pFindFolder, std::list<std::string>& findFiles)
{
	std::string folder = pFindFolder;
	folder += "\\*.*";
	WIN32_FIND_DATAA kFindData;
	HANDLE hFindFile = ::FindFirstFileA( folder.c_str(), &kFindData );
	if( hFindFile != INVALID_HANDLE_VALUE )
	{
		do
		{
			if( (kFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)// 파일일 때
			{
				std::string strFindFile = pFindFolder;
				strFindFile += "\\";
				strFindFile += kFindData.cFileName;
				findFiles.push_back(strFindFile);
			}
		}while( TRUE == ::FindNextFileA( hFindFile, &kFindData ) );

		::FindClose( hFindFile );
	}
}