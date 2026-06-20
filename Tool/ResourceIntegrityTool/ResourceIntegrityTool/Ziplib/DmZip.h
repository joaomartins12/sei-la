#pragma once
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include "zip.h"
#include "unzip.h"

class CDmZip
{
public:
	CDmZip(){};
	~CDmZip(){};

	BOOL ZipDirectory (LPTSTR pszCompressPath );
	BOOL ZipDirectory (LPTSTR pszCompressPath, LPTSTR pszZipCreationPath, LPTSTR pszZipName );
/*
	ZipDirectory : 압축할 디렉토리 경로와 생성할 ZIP 파일의 이름을 받아 
	                       현재 디렉터리에 압축파일을 생성.

	HZIP* phZip : ZIP HANDLE의 포인터. 
	LPTSTR pszCompresspath :압축할 디렉터리 경로
	LPTSTR pszZipCreationPath : 압축 후 생성될 ZIP 파일의 경로
	LPTSTR pszZipName : 압축 후 생성될 ZIP 파일의 이름

	Return Value : 실패하면 FALSE, 성공하면 TRUE
*/
	BOOL UnZip( LPTSTR pszUnZipPath,			// 압축파일이 있는 폴더
				LPTSTR pszUnZipName,			// 압축을 풀 파일명
				bool bIgnoreFilePath = false);  // 압축파일명의 폴더를 생성해서 압축을 해제할것가

	BOOL UnZip( LPTSTR pszUnZipPath,			// 압축을 풀 파일명
				bool bIgnoreFilePath  = false);	// 압축파일명의 폴더를 생성해서 압축을 해제할것가	

private:
	BOOL InternalCompressDirectory(HZIP* phZip, LPTSTR pszFilePath);
	/*
		InternalCompressDirectory : SetCompressDirectory 가 내부적으로 호출하는 함수
									디렉터리를 검색하면서 ZipAdd, ZipAddFolder 를 호출

		HZIP* phZip : ZIP HANDLE의 포인터. 
		LPTSTR pszFilePath : 압축할 디렉터리의 이름

		Return Value : 실패하면 FALSE, 성공하면 TRUE
	*/

private:
	HZIP		m_hZip;
};