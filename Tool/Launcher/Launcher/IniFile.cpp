// IniFile.cpp: implementation of the CIniFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIniFile::CIniFile()
{

}

CIniFile::~CIniFile()
{

}

//---------------------------------------------------------------------
void CIniFile::Init( const TCHAR* lpFile)
{
	lpFileName = lpFile;
}


//---------------------------------------------------------------------
CString CIniFile::GetString( const TCHAR* lpAppName, const TCHAR* lpKeyName)
{
	int nRet;

	TCHAR	pBuf[1000];
	_tcscpy_s(pBuf, 1000, L"");

	TCHAR	strInitFileName[1000];
	TCHAR	strFilePath[1000];
	GetCurrentDirectory(sizeof(strFilePath), strFilePath);
	_stprintf_s(strInitFileName, 1000, L"%s/%s", strFilePath, lpFileName);

	nRet = GetPrivateProfileString(lpAppName, lpKeyName, NULL, pBuf, sizeof(pBuf), strInitFileName);

	CString strBuf;
	strBuf.Format(L"%s", pBuf);
	return strBuf;
}

int CIniFile::GetInt( const TCHAR* lpAppName, const TCHAR* lpKeyName, int defaultvalue)
{
	int nRet;

	TCHAR	strInitFileName[1000];
	TCHAR	strFilePath[1000];
	GetCurrentDirectory(sizeof(strFilePath), strFilePath);
	_stprintf_s(strInitFileName, 1000, L"%s/%s", strFilePath, lpFileName);

	nRet = GetPrivateProfileInt(lpAppName, lpKeyName, defaultvalue, strInitFileName);

	return nRet;
}

BOOL CIniFile::WriteString( const TCHAR* lpAppName, const TCHAR* lpKeyName, const TCHAR* lpString)
{
	TCHAR	strInitFileName[1000];
	TCHAR	strFilePath[1000];
	GetCurrentDirectory(sizeof(strFilePath), strFilePath);
	_stprintf_s(strInitFileName, 1000, L"%s/%s", strFilePath, lpFileName);

	BOOL bRet = WritePrivateProfileString(lpAppName, lpKeyName, lpString, strInitFileName);

	return bRet;
}

BOOL CIniFile::WriteIni( const TCHAR* lpAppName, const TCHAR* lpKeyName, int nInt)
{
	TCHAR	strInitFileName[1000];
	TCHAR	strFilePath[1000];
	GetCurrentDirectory(sizeof(strFilePath), strFilePath);
	_stprintf_s(strInitFileName, 1000, L"%s/%s", strFilePath, lpFileName);

	TCHAR buffer[1000];
	_stprintf_s(buffer, 1000, L"%d", nInt);
	BOOL bRet = WritePrivateProfileString(lpAppName, lpKeyName, buffer, strInitFileName);

	return bRet;
}
