#include "stdafx.h"
#include <comutil.h>
#include <exdisp.h>
#include <process.h>    /* _beginthread, _endthread */
#include <stddef.h>

#pragma comment(lib, "comsupp.lib")

// simple data structure for our browser settings
typedef struct
{
  char zUrl[1024];
  int nWidth;
  int nHeight;
	HICON hIcon;
}BROWSER_INFO;

BROWSER_INFO g_BrowserInfo;

// This thread demonstrates how to initialize OLE such that we have an exclusive COM apartment for this
// thread separate from the main thread.
//
// It also shows how to manipulate the IWebBrowser interface to do some interesting things such as:
//   1.  Make the IE window resizable or static (non-resizable) based on IE version number.
//	 2.  Turn off the address bar, menu bar, status bar, and tool bar buttons.
//	 3.  Set the width and height of the resulting window.
//	 4.  Change the icon of the MSIE window to one of your own.
//
void BrowserThread( void* pParams )
{
	OleInitialize(NULL);
	BROWSER_INFO *pBrowserInfo = (BROWSER_INFO *)pParams;
  	IWebBrowser2* m_pInetExplorer;

    HRESULT hr;
//	HICON hIcon;
    CLSID clsid;
    LPUNKNOWN punk=NULL;
    CLSIDFromProgID (OLESTR("InternetExplorer.Application"), &clsid);
    hr = CoCreateInstance (clsid, NULL, CLSCTX_SERVER, IID_IUnknown, (LPVOID *) &punk);
    if (SUCCEEDED(hr))
	{
		CoAllowSetForegroundWindow(punk, NULL);
		punk->QueryInterface (IID_IWebBrowser2, (LPVOID *) &m_pInetExplorer);
		punk->Release();
		if( m_pInetExplorer )
		{
			m_pInetExplorer->put_ToolBar(FALSE);
			m_pInetExplorer->put_AddressBar(VARIANT_FALSE);
			m_pInetExplorer->put_MenuBar(VARIANT_FALSE);
			m_pInetExplorer->put_StatusBar(VARIANT_FALSE);
			m_pInetExplorer->put_Width(pBrowserInfo->nWidth);
			m_pInetExplorer->put_Height(pBrowserInfo->nHeight);
			m_pInetExplorer->put_Resizable(VARIANT_FALSE);

			VARIANT vars[4];
			memset(vars,0,sizeof(vars));
			BSTR BStrURL = _com_util::ConvertStringToBSTR((const char *)(pBrowserInfo->zUrl));
			HRESULT hrie = m_pInetExplorer->Navigate(BStrURL,vars,vars+1,vars+2,vars+3);
			SysFreeString(BStrURL);
			if (SUCCEEDED(hrie))
				m_pInetExplorer->put_Visible(VARIANT_TRUE);
			else
				m_pInetExplorer->Quit();

			m_pInetExplorer->Release();
		}		
	}

    OleUninitialize();
}

/*
 * Spawn the M browser
 */
void spawn_browser(
  const char *uri,	// URL
  int nWidth,				// Window Width
  int nHeight,			// Window Height
	HICON hIcon)			// Handle to the icon
{
	g_BrowserInfo.nWidth = nWidth;
	g_BrowserInfo.nHeight = nHeight;
	memset(g_BrowserInfo.zUrl,0,(sizeof(g_BrowserInfo.zUrl)*sizeof(char)));
	strncpy(g_BrowserInfo.zUrl,uri,min(strlen(uri),(sizeof(g_BrowserInfo.zUrl)*sizeof(char))));
	g_BrowserInfo.hIcon = hIcon;
 	HANDLE hThread = (HANDLE) _beginthread( BrowserThread, 0, &g_BrowserInfo );
 
 	// Wait until IE is done loading before we return.
    WaitForSingleObject( hThread, INFINITE );
}