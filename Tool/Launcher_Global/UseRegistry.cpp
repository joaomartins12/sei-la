
#include "stdafx.h"
#include "UseRegistry.h"

/*---------------------------------------------------------------------
	레지스트리로 부터 key값이 존재하는지 체크 
----------------------------------------------------------------------*/
BOOL REG::IsRegstry( HKEY hKey, LPCTSTR lpKey )
{
	HKEY key;
	LONG lResult = RegOpenKeyEx( hKey, lpKey, 0, KEY_READ, &key );

	if( lResult != ERROR_SUCCESS )
		lResult = RegOpenKeyEx( hKey, lpKey, 0, KEY_READ | KEY_WOW64_64KEY, &key );

	if( lResult != ERROR_SUCCESS )
		return FALSE;

	RegCloseKey( key );
	return TRUE;
}

/*---------------------------------------------------------------------
레지스트리로 부터 Int형 값을 읽어들이는 함수
----------------------------------------------------------------------*/
UINT REG::RegReadInt( HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, INT nDefault )
{
	HKEY key;
	LONG lResult = RegOpenKeyEx(hKey, lpKey, 0, KEY_READ, &key);
	if (lResult != ERROR_SUCCESS)
		lResult = RegOpenKeyEx(hKey, lpKey, 0, KEY_READ | KEY_WOW64_64KEY, &key);

	if (lResult != ERROR_SUCCESS)
		return 0;

	UINT Result;
	DWORD Size = sizeof( LONG );
	if( RegQueryValueEx( key, lpValue, 0, NULL, (LPBYTE)&Result, &Size ) != ERROR_SUCCESS )
	{
		Result = nDefault;
	}

	RegCloseKey( key );
	return Result;
}

/*---------------------------------------------------------------------
레지스트리로 부터 String형 값을 읽어들이는 함수
----------------------------------------------------------------------*/
BOOL REG::RegReadString( HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpDefault, LPTSTR lpRet, DWORD nSize )
{
	if( IsRegstry( hKey, lpKey ) == FALSE )
		return FALSE;

	HKEY key;
	LONG lResult = RegOpenKeyEx(hKey, lpKey, 0, KEY_READ, &key);
	if (lResult != ERROR_SUCCESS)
		lResult = RegOpenKeyEx(hKey, lpKey, 0, KEY_READ | KEY_WOW64_64KEY, &key);

	if (lResult != ERROR_SUCCESS)
		return FALSE;

	DWORD Size = nSize;

	if( RegQueryValueEx( key, lpValue, 0, NULL, (LPBYTE)lpRet, &Size ) != ERROR_SUCCESS )
	{		
		return FALSE;
	}

	if( _tcslen( lpRet ) == 0 )
		return FALSE;

	RegCloseKey( key );
	return TRUE;
}
// 
// /*---------------------------------------------------------------------
// 레지스트리로 Int형 값을 쓰는 함수
// ----------------------------------------------------------------------*/
// BOOL REG::RegWriteInt( HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, UINT nData )
// {
// 	HKEY key;
// 	DWORD dwDisp;
// 	LONG lResult = RegCreateKeyEx( hKey, lpKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &dwDisp );
// 	if( lResult != ERROR_SUCCESS )
// 		lResult = RegCreateKeyEx( hKey, lpKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_WOW64_64KEY, NULL, &key, &dwDisp );
// 
// 	if( lResult != ERROR_SUCCESS )
// 		return FALSE;
// 
// 	if( RegSetValueEx( key, lpValue, 0, REG_DWORD, (LPBYTE)&nData, (int)sizeof(UINT) ) != ERROR_SUCCESS )
// 		return FALSE;
// 
// 	RegCloseKey( key );
// 	return TRUE;
// }
// 
// /*---------------------------------------------------------------------
// 레지스트리로 String형 값을 쓰는 함수
// ----------------------------------------------------------------------*/
// BOOL REG::RegWriteString( HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue, LPCTSTR lpData )
// {
// 	HKEY key;
// 	DWORD dwDisp;
// 	LONG lResult = RegCreateKeyEx( hKey, lpKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &dwDisp );
// 	if( lResult != ERROR_SUCCESS )
// 		lResult = RegCreateKeyEx( hKey, lpKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_WOW64_64KEY, NULL, &key, &dwDisp );
// 	if( lResult != ERROR_SUCCESS )
// 		return FALSE;
// 
// 	if( RegSetValueEx( key, lpValue, 0, REG_SZ, (LPBYTE)lpData, (int)( sizeof( TCHAR ) * ( _tcslen( lpData ) + 1 ) ) ) != ERROR_SUCCESS )
// 		return FALSE;
// 
// 	RegCloseKey( key );
// 	return TRUE;
// }
// 
// /*---------------------------------------------------------------------
// 레지스트리로 Binary형 값을 쓰는 함수
// ----------------------------------------------------------------------*/
// BOOL REG::RegWriteBinary( HKEY hKey,	LPCTSTR lpKey,	LPCTSTR lpValue, LPCTSTR lpData, int size )
// {
// 	HKEY key;
// 	DWORD dwDisp;
// 
// 	LONG lResult = RegCreateKeyEx( hKey, lpKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &key, &dwDisp );
// 	if( lResult != ERROR_SUCCESS )
// 		lResult = RegCreateKeyEx( hKey, lpKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE | KEY_WOW64_64KEY, NULL, &key, &dwDisp );
// 	if( lResult != ERROR_SUCCESS )
// 		return FALSE;
// 
// 	if( RegSetValueEx( key, lpValue, 0, REG_BINARY, (LPBYTE)lpData, size ) != ERROR_SUCCESS )
// 		return FALSE;
// 
// 	RegCloseKey( key );
// 	return TRUE;
// }
// 
// //레지스트리 삭제함수
// BOOL REG::RegRemoveValue( HKEY hKey, LPCTSTR lpKey, LPCTSTR lpValue ) 
// {
// 	HKEY key;
// 
// 	LONG lResult = RegOpenKeyEx( hKey, lpKey, 0, KEY_ALL_ACCESS,	&key);
// 	if( lResult != ERROR_SUCCESS )
// 		lResult = RegOpenKeyEx( hKey, lpKey, 0, KEY_ALL_ACCESS | KEY_WOW64_64KEY,	&key);
// 	if( lResult != ERROR_SUCCESS )
// 		return FALSE;
// 
// 	if(RegDeleteValue(key, lpValue) != ERROR_SUCCESS)
// 		return FALSE;
// 
// 	RegCloseKey( key );
// 	return TRUE;
// }

