#include "stdafx.h"
#include "WindowVersionChecker.h"


namespace WVS
{
	

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	APPCOMPATFLAG WINAPI GetAppCompatFlag(LPCTSTR szFileName)   
	{   
		HKEY   hKey;   
		APPCOMPATFLAG apcfRet = apcfNone;

		LPCTSTR szActName[14] = {_T("WIN95"), _T("WIN98"), _T("NT4SP5"), _T("WWIN2000"), _T("WINXPSP2"), _T("WINXPSP3"), _T("WINSRV03SP1"), _T("WINSRV08SP1"), _T("VISTARTM"), _T("VISTASP1"), _T("VISTASP2"), _T("WIN7RTM"), _T("WIN8RTM"), NULL};

		LPCTSTR szRegAppCompatLayerKey = _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers");   
		if(RegOpenKey(HKEY_CURRENT_USER, szRegAppCompatLayerKey, &hKey) == ERROR_SUCCESS)   
		{  
			TCHAR szCompatType[MAX_PATH] = {0};   
			DWORD dwType;   
			DWORD dwSize = sizeof szCompatType;   
			if(RegQueryValueEx(hKey, szFileName, NULL, &dwType, (LPBYTE) szCompatType, &dwSize) == ERROR_SUCCESS)   
			{     
				for(int i=0; szActName[i] != NULL; ++i)     
				{      
					if(_tcsicmp(szCompatType, szActName[i]) == 0)      
					{       
						apcfRet = (APPCOMPATFLAG) i;       
						break;      
					}     
				}   
			}   

			RegCloseKey(hKey);   
		}
		return apcfRet;   
	}  

	BOOL WINAPI SetAppCompatFlag(LPCTSTR szFileName, APPCOMPATFLAG apcfNewType)   
	{   
		HKEY hKey;   
		LPCTSTR szRegAppCompatLayerKey =_T("Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers");   
		BOOL bRet = FALSE;   

		LPCTSTR szActName[14] = {_T("WIN95"), _T("WIN98"), _T("NT4SP5"), _T("WWIN2000"), _T("WINXPSP2"), _T("WINXPSP3"), _T("WINSRV03SP1"), _T("WINSRV08SP1"), _T("VISTARTM"), _T("VISTASP1"), _T("VISTASP2"), _T("WIN7RTM"), _T("WIN8RTM"), NULL};

		if( RegOpenKey(HKEY_CURRENT_USER, szRegAppCompatLayerKey, &hKey) != ERROR_SUCCESS )
		{
			DWORD dwDisp;
			LONG lResult = RegCreateKeyEx( HKEY_CURRENT_USER, szRegAppCompatLayerKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp );
			if( lResult != ERROR_SUCCESS )
				return bRet;
		}

		TCHAR szNewType[MAX_PATH] = {0};   
		_tcscpy_s( szNewType, MAX_PATH, szActName[apcfNewType] );

		if(_tcsicmp(szNewType, _T("")) == 0)   
		{     
			if(RegDeleteValue(hKey, szFileName) == ERROR_SUCCESS)      
				bRet = TRUE;   
		}   
		else   
		{     
			if(RegSetValueEx(hKey, szFileName, 0, REG_SZ, (LPBYTE) szNewType, (int)( sizeof( TCHAR ) * ( _tcslen( szNewType ) + 1 ) ) ) == ERROR_SUCCESS)      
				bRet = TRUE;     
		}   

		RegCloseKey(hKey);   
		return bRet;   
	}  


	BOOL WINAPI RemoveAppCompatFlag(LPCTSTR szFileName)
	{   
		HKEY hKey;   
		LPCTSTR szRegAppCompatLayerKey =_T("Software\\Microsoft\\Windows NT\\CurrentVersion\\AppCompatFlags\\Layers");   
		BOOL bRet = FALSE;   

		if( RegOpenKey(HKEY_CURRENT_USER, szRegAppCompatLayerKey, &hKey) != ERROR_SUCCESS )
		{
			DWORD dwDisp;
			LONG lResult = RegCreateKeyEx( HKEY_CURRENT_USER, szRegAppCompatLayerKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, &dwDisp );
			if( lResult != ERROR_SUCCESS )
				return bRet;
		}

		if(RegDeleteValue(hKey, szFileName) == ERROR_SUCCESS)      
			bRet = TRUE;   

		RegCloseKey(hKey);   
		return bRet;   
	}  
}