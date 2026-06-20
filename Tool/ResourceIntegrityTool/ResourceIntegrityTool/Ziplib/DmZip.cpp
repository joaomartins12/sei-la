#include "stdafx.h"
#include "DmZip.h"


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 압축 관련 함수
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

BOOL CDmZip::ZipDirectory( LPTSTR pszCompressPath )
{
	// 현재 디렉토리 임시 저장
	TCHAR szOldCurrentPath[MAX_PATH] = _T("");
	GetCurrentDirectory( _countof(szOldCurrentPath), szOldCurrentPath );

	TCHAR szDrive[_MAX_DRIVE] = _T("");
	TCHAR szDir[_MAX_DIR] = _T("");
	TCHAR szFName[_MAX_FNAME] = _T("");
	TCHAR szExt[_MAX_EXT] = _T("");

	TCHAR szZipFileName[_MAX_FNAME]=_T("");
	TCHAR szZipFileFullPath[_MAX_PATH]=_T("");

	_tsplitpath_s(pszCompressPath, szDrive, szDir, szFName, szExt);
	_tmakepath_s( szZipFileName, NULL, NULL, szFName,_T("zip") );

	_tmakepath_s( szZipFileFullPath, szDrive, szDir, szFName, _T("zip") );

	// 압축할 파일을 생성
	m_hZip = CreateZip( szZipFileFullPath, 0 );
	if ( m_hZip == INVALID_HANDLE_VALUE )
	{	// 압축할 파일을 생성 못함.
		SetCurrentDirectory( szOldCurrentPath );
		return FALSE;
	}

	// 현재 폴더를 압출 후 나올 폴더로 셋팅
	if ( !SetCurrentDirectory( pszCompressPath ) )
		return FALSE;

	InternalCompressDirectory( &m_hZip, _T("") );

	CloseZip ( m_hZip );

	SetCurrentDirectory ( szOldCurrentPath );
	return TRUE;
}


BOOL CDmZip::ZipDirectory( LPTSTR pszCompressPath,		// 압축할 폴더
						   LPTSTR pszZipCreationPath,	// 압축후 파일이 생성되는 폴더
						   LPTSTR pszZipName )			// 압축파일 이름.
{
	// 현재 디렉토리 임시 저장
	TCHAR szOldCurrentPath[MAX_PATH] = _T("");
	GetCurrentDirectory( _countof(szOldCurrentPath), szOldCurrentPath );

	// 압축파일이 나올 폴더 셋팅
	TCHAR szZipCreationFullPath[MAX_PATH] = _T("");
	if (!GetFullPathName(pszZipCreationPath, MAX_PATH, szZipCreationFullPath, NULL))
		return FALSE;

	// 현재 폴더를 압출 후 나올 폴더로 셋팅
	if ( !SetCurrentDirectory( szZipCreationFullPath ) )
		return FALSE;

	// 압축할 파일을 생성
	m_hZip = CreateZip( pszZipName, 0 );
	if ( m_hZip == INVALID_HANDLE_VALUE )
	{	// 압축할 파일을 생성 못함.
		SetCurrentDirectory( szOldCurrentPath );
		return FALSE;
	}

	// 압축할 폴더명 셋팅
	TCHAR szCompressFullPath[MAX_PATH] = _T("");
	if (!GetFullPathName(pszCompressPath, MAX_PATH, szCompressFullPath, NULL))
	{
		SetCurrentDirectory( szOldCurrentPath );
		return FALSE;
	}

	// 압축할 디렉토리로 변경
	if( !SetCurrentDirectory( pszCompressPath ) )
	{
		SetCurrentDirectory( szOldCurrentPath );
		return FALSE;
	}

	InternalCompressDirectory( &m_hZip, _T("") );

	CloseZip ( m_hZip );

	SetCurrentDirectory ( szOldCurrentPath );
	return TRUE;
}

BOOL CDmZip::InternalCompressDirectory(HZIP* phZip, LPTSTR pszFilePath)
{
	WIN32_FIND_DATA stWinFindData;
	TCHAR szFind[ MAX_PATH ] = {0, };
	_stprintf_s( szFind, MAX_PATH, _T("%s*.*"), pszFilePath );
	HANDLE hSrch = FindFirstFile ( szFind, &stWinFindData );
	if( hSrch == INVALID_HANDLE_VALUE )
		return FALSE;
	do 
	{
		if ( stWinFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if ( _tcscmp ( stWinFindData.cFileName, _T(".") )  && _tcscmp ( stWinFindData.cFileName, _T("..") ) )
			{
				TCHAR szDirPath[MAX_PATH] ={0,};
				_stprintf_s ( szDirPath, _T("%s%s"), pszFilePath, stWinFindData.cFileName );
				ZipAddFolder(*phZip, szDirPath);

				TCHAR szNewDirPath[MAX_PATH] ={0,};
				_stprintf_s ( szNewDirPath, _T("%s\\"), szDirPath );
				InternalCompressDirectory ( phZip, szNewDirPath );
			}
		} 
		else
		{
			TCHAR szFilePath[MAX_PATH] ={0,};
			_stprintf_s ( szFilePath, _T("%s%s"), pszFilePath, stWinFindData.cFileName );
			ZipAdd ( *phZip, szFilePath, szFilePath );
		}
	} while ( TRUE == ::FindNextFile ( hSrch, &stWinFindData ) );

	FindClose(hSrch);
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 압축 해체 관련 함수
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

// 폴더 및 파일명이 같이 포함되어 있는 경우
BOOL CDmZip::UnZip( LPTSTR pszUnZipPath, bool bIgnoreFilePath )
{
	TCHAR szUnZipFullPath[MAX_PATH] = _T("");
	if(!GetFullPathName(pszUnZipPath, MAX_PATH, szUnZipFullPath, NULL))
		return FALSE;

	m_hZip = OpenZip(szUnZipFullPath,0);
	if( m_hZip == INVALID_HANDLE_VALUE )
		return FALSE;

	TCHAR szDrive[_MAX_DRIVE] = _T("");
	TCHAR szDir[_MAX_DIR] = _T("");
	TCHAR szFName[_MAX_FNAME] = _T("");
	TCHAR szExt[_MAX_EXT] = _T("");
	TCHAR szUnzipPath[_MAX_PATH]=_T("");

	_tsplitpath_s(szUnZipFullPath, szDrive, szDir, szFName, szExt);
	if( bIgnoreFilePath )
	{
		_tmakepath_s( szUnzipPath, szDrive, szDir, szFName, NULL );
		::CreateDirectory(szUnzipPath, NULL);
	}
	else
		_tmakepath_s( szUnzipPath, szDrive, szDir, NULL, NULL );

	SetUnzipBaseDir( m_hZip, szUnzipPath );

	ZIPENTRY ze; 
	GetZipItem(m_hZip,-1,&ze); 
	int numitems=ze.index;
	for (int i=0; i<numitems; i++)
	{ 
		GetZipItem(m_hZip,i,&ze);
		UnzipItem(m_hZip,i,ze.name);
	}

	CloseZip(m_hZip);

	return TRUE;
}

BOOL CDmZip::UnZip( LPTSTR pszUnZipPath,	// 압축파일이 있는 폴더
					LPTSTR pszUnZipName,	// 압축을 풀 파일명
					bool bIgnoreFilePath )
{
	// 압축파일이 있는 폴더 셋팅
	TCHAR szUnZipFullPath[MAX_PATH] = _T("");
	if(!GetFullPathName(pszUnZipPath, MAX_PATH, szUnZipFullPath, NULL))
		return FALSE;

	TCHAR  szZipFileFullPath[MAX_PATH] = _T("");
	TCHAR  szDrive[_MAX_DRIVE] = _T("");
	TCHAR  szDir[_MAX_DIR] = _T("");
	TCHAR  szFileName[_MAX_FNAME] = _T("");
	TCHAR  szFileEx[_MAX_EXT] = _T("");
	_tsplitpath_s( pszUnZipName, szDrive, szDir, szFileName, szFileEx);
	_tmakepath_s( szZipFileFullPath, NULL, szUnZipFullPath, szFileName, szFileEx );

	return UnZip( szZipFileFullPath, bIgnoreFilePath);
}
