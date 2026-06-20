
#pragma once 

#define CSMESSAGEBOX_LEN			512

int CsMessageBox(UINT uType, LPCTSTR szApp, ...);
int CsMessageBoxA(UINT uType, char const* cApp, ...);
int CsMessageBox_Title( UINT uType, LPCTSTR szCaption, LPCTSTR szApp , ... );
int CsMessageBoxA_Title(UINT uType, char const* cCaption, char const* cApp, ...);

int CsMessageBox(HWND hWnd, UINT uType, LPCTSTR szApp, ...);
int CsMessageBoxA(HWND hWnd, UINT uType, char const* cApp, ...);
int CsMessageBox_Title(HWND hWnd, UINT uType, LPCTSTR szCaption, LPCTSTR szApp, ...);
int CsMessageBoxA_Title(HWND hWnd, UINT uType, char const* cCaption, char const* cApp, ...);