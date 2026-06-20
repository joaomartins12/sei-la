//--------------------------------------------------------------------------------------
// Ini File Class
//
// 
//--------------------------------------------------------------------------------------

#if !defined(AFX_INIFILE_H__2DD56E9F_7493_4583_957D_07A084B4B1EA__INCLUDED_)
#define AFX_INIFILE_H__2DD56E9F_7493_4583_957D_07A084B4B1EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <stdio.h>

class CIniFile  
{
	const TCHAR* lpFileName;
public:
	BOOL WriteIni( const TCHAR* lpAppName, const TCHAR* lpKeyName, int nInt);
	BOOL WriteString( const TCHAR* lpAppName, const TCHAR* lpKeyName, const TCHAR* lpString);
	int GetInt( const TCHAR* lpAppName, const TCHAR* lpKeyName, int defaultvalue = -1);
	void Init( const TCHAR* lpFile);
	CString GetString( const TCHAR* lpAppName, const TCHAR* lpKeyName);
	CIniFile();
	virtual ~CIniFile();

};

#endif // !defined(AFX_INIFILE_H__2DD56E9F_7493_4583_957D_07A084B4B1EA__INCLUDED_)
