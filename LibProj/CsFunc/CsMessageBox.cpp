
#include "stdafx.h"
#include "CsMessageBox.h"

int CsMessageBox( UINT uType, LPCTSTR szApp , ... )
{
	TCHAR App[CSMESSAGEBOX_LEN] = {0 };
	va_list args;
	va_start(args, szApp);
	_vsntprintf_s( App, CSMESSAGEBOX_LEN, szApp, args );
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	return MessageBox( NULL , App, _T( "CsMessage" ), uType );	
}

int CsMessageBoxA( UINT uType, char const* cApp , ... )
{
	char App[CSMESSAGEBOX_LEN] = {0 };
	va_list args;
	va_start(args, cApp);
	_vsnprintf_s( App, CSMESSAGEBOX_LEN, cApp, args );
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	return MessageBoxA( NULL , App, "CsMessage", uType );	
}

int CsMessageBox_Title( UINT uType, LPCTSTR szCaption, LPCTSTR szApp , ... )
{
	TCHAR App[CSMESSAGEBOX_LEN] = {0 };
	va_list args;
	va_start(args, szApp);
	_vsntprintf_s( App, CSMESSAGEBOX_LEN, szApp, args );
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	return MessageBox( NULL , App, szCaption, uType );	
}

int CsMessageBoxA_Title( UINT uType, char const* cCaption, char const* cApp , ... )
{
	char App[CSMESSAGEBOX_LEN] = {0 };
	va_list args;
	va_start(args, cApp);
	_vsnprintf_s( App, CSMESSAGEBOX_LEN, cApp, args );
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	return MessageBoxA( NULL , App, cCaption, uType );
}

//////////////////////////////////////////////////////////////////////////

int CsMessageBox(HWND hWnd, UINT uType, LPCTSTR szApp, ...)
{
	TCHAR App[CSMESSAGEBOX_LEN] = { 0 };
	va_list args;
	va_start(args, szApp);
	_vsntprintf_s(App, CSMESSAGEBOX_LEN, szApp, args);
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	return MessageBox(hWnd, App, _T("CsMessage"), uType);
}

int CsMessageBoxA(HWND hWnd, UINT uType, char const* cApp, ...)
{
	char App[CSMESSAGEBOX_LEN] = { 0 };
	va_list args;
	va_start(args, cApp);
	_vsnprintf_s(App, CSMESSAGEBOX_LEN, cApp, args);
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	return MessageBoxA(hWnd, App, "CsMessage", uType);
}

int CsMessageBox_Title(HWND hWnd, UINT uType, LPCTSTR szCaption, LPCTSTR szApp, ...)
{
	TCHAR App[CSMESSAGEBOX_LEN] = { 0 };
	va_list args;
	va_start(args, szApp);
	_vsntprintf_s(App, CSMESSAGEBOX_LEN, szApp, args);
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	return MessageBox(hWnd, App, szCaption, uType);
}

int CsMessageBoxA_Title(HWND hWnd, UINT uType, char const* cCaption, char const* cApp, ...)
{
	char App[CSMESSAGEBOX_LEN] = { 0 };
	va_list args;
	va_start(args, cApp);
	_vsnprintf_s(App, CSMESSAGEBOX_LEN, cApp, args);
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	return MessageBoxA(hWnd, App, cCaption, uType);
}

