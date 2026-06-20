//-----------------------------------------------------------------------------
//
//	$File: cLogFile.cpp $
//	$Author: Cho yong-il, caelus@hanmail.net $
//  
//	$Modtime: 14.01.03 02:12 $
//	$Revision: 0 $
//
//	Description: Implementation of the classes "cLogFile"
//               (wrapper class to write some data to specified file)
//
//-----------------------------------------------------------------------------
 
//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include "cLogFile.h"

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
cLogFile::cLogFile()
{

}
 
cLogFile::~cLogFile()
{
	if (hFileLog) CloseHandle(hFileLog);
}

BOOL cLogFile::OpenLog(LPCTSTR lpLogFile)
{
	BOOL bRet = FALSE;
	hFileLog = CreateFile(lpLogFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileLog)
	{
		SetFilePointer(hFileLog, 0, NULL, FILE_END);
		bRet = TRUE;
	}
	return bRet;
}

bool cLogFile::TruncateFile(LPCTSTR lpLogFile)
{
	bool bRet = FALSE;
	hFileLog = CreateFile(lpLogFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, TRUNCATE_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileLog)
	{
		SetFilePointer(hFileLog, 0, NULL, FILE_END);
		bRet = TRUE;
	}
	return bRet;
}

BOOL cLogFile::WriteLog(LPBYTE lpLog, int nSizeOfLog)
{
	DWORD dwWritten = 0;
	BOOL bRet = WriteFile(hFileLog, (LPCVOID)lpLog, nSizeOfLog, &dwWritten, NULL);
	return bRet;
}

void cLogFile::CloseLog()
{
	CloseHandle(hFileLog);
}

bool GetTimeSTR(LPSTR szData,SIZE_T DateSize, LPSTR szTime, SIZE_T TimeSize)
{	// 최하 14 byte 이하인 경우 처리 안함
	if( szData == NULL || szTime == NULL )
	{
		return false;
	}

	if( DateSize < 14 || TimeSize < 14 )
	{
		return false;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);		
	sprintf_s(szData, DateSize, "%04d-%02d-%02d ", st.wYear, st.wMonth, st.wDay);		
	sprintf_s(szTime, TimeSize, "%02d-%02d-%02d:%02d", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds);	

	return true;
}

bool GetTimeSTR5Min(LPSTR szData,SIZE_T DateSize, LPSTR szTime, SIZE_T TimeSize)
{	// 최하 14 byte 이하인 경우 처리 안함
	if( szData == NULL || szTime == NULL )
	{
		return false;
	}

	if( DateSize < 14 || TimeSize < 14 )
	{
		return false;
	}

	SYSTEMTIME st;
	GetLocalTime(&st);
	st.wMinute = st.wMinute/5 * 5;
	sprintf_s(szData, DateSize, "%04d-%02d-%02d", st.wYear, st.wMonth, st.wDay);		
	sprintf_s(szTime, TimeSize, "%02d-%02d-%02d", st.wHour, st.wMinute, st.wSecond);			

	return true;
}

unsigned int GetTimeMin()
{		
	SYSTEMTIME st;
	GetLocalTime(&st);		
	return st.wMinute;
}