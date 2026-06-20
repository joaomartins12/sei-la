#include "stdafx.h"
#include "WebControl.h"
#include "..\\..\\resource.h"

CWebContainer::CWebContainer( HWND hwnd )
{
   m_cRef = 0;
   m_hwnd = hwnd;
   m_punk = NULL;

   SetRectEmpty( &m_rect );

   m_pEvent = new CWebEventSink();

   m_pEvent->AddRef();
}

CWebContainer::~CWebContainer()
{
   m_pEvent->Release();

   m_punk->Release();
   
}

void CWebContainer::add()
{
   CLSID clsid;

   CLSIDFromString( _T( "Shell.Explorer" ), &clsid );

   CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER | CLSCTX_LOCAL_SERVER, IID_IUnknown, (PVOID *) &m_punk );

   IOleObject *pioo;

   m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

   pioo->SetClientSite( this );

   pioo->Release();

   IPersistStreamInit *ppsi;

   m_punk->QueryInterface( IID_IPersistStreamInit, (PVOID *) &ppsi );

   ppsi->InitNew();
   ppsi->Release();

   ConnectEvents();

   setVisible( true );

   setFocus( true );
}

/**************************************************************************

   CWebContainer::remove()

**************************************************************************/

void CWebContainer::remove()
{
   IOleObject *pioo;
   
   m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

   pioo->Close( OLECLOSE_NOSAVE );

   pioo->SetClientSite( NULL );

   pioo->Release();

   IOleInPlaceObject *pipo;

   m_punk->QueryInterface( IID_IOleInPlaceObject, (PVOID *) &pipo );

   pipo->UIDeactivate();

   pipo->InPlaceDeactivate();

   pipo->Release();

   DisconnectEvents();
}

/**************************************************************************

   CWebContainer::setLocation()

**************************************************************************/

void CWebContainer::setLocation( int x, int y, int cx, int cy )
{
   m_rect.left   = x;
   m_rect.top    = y;
   m_rect.right  = cx;
   m_rect.bottom = cy;

   IOleInPlaceObject *pipo;

   m_punk->QueryInterface( IID_IOleInPlaceObject, (PVOID *) &pipo );

   pipo->SetObjectRects( &m_rect, &m_rect );

   pipo->Release();
}

/**************************************************************************

   CWebContainer::setVisible()

**************************************************************************/

void CWebContainer::setVisible( bool bVisible )
{
   IOleObject *pioo;

   m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

   if ( bVisible )
   {
      pioo->DoVerb( OLEIVERB_INPLACEACTIVATE, NULL, this, 0, m_hwnd, &m_rect );
      pioo->DoVerb( OLEIVERB_SHOW, NULL, this, 0, m_hwnd, &m_rect );
   }
   else
      pioo->DoVerb( OLEIVERB_HIDE, NULL, this, 0, m_hwnd, NULL );

   pioo->Release();
}

/**************************************************************************

   CWebContainer::setFocus()

**************************************************************************/

void CWebContainer::setFocus( bool bFocus )
{
   IOleObject *pioo;

   if ( bFocus )
   {
      m_punk->QueryInterface( IID_IOleObject, (PVOID *) &pioo );

      pioo->DoVerb( OLEIVERB_UIACTIVATE, NULL, this, 0, m_hwnd, &m_rect );

      pioo->Release();
   }
}

void CWebContainer::ConnectEvents()
{
   IConnectionPoint *pcp = GetConnectionPoint( DIID_DWebBrowserEvents2 );

   pcp->Advise( m_pEvent, &m_eventCookie );

   pcp->Release();
}

void CWebContainer::DisconnectEvents()
{
   IConnectionPoint *pcp;

   pcp = GetConnectionPoint( DIID_DWebBrowserEvents2 );

   pcp->Unadvise( m_eventCookie );

   pcp->Release();
}

IConnectionPoint* CWebContainer::GetConnectionPoint( REFIID riid )
{
   IConnectionPointContainer *pcpc;
   
   m_punk->QueryInterface( IID_IConnectionPointContainer, (PVOID *) &pcpc );

   IConnectionPoint *pcp;

   pcpc->FindConnectionPoint( riid, &pcp );

   pcpc->Release();

   return pcp;
}

HRESULT CWebContainer::getWeb( IWebBrowser2 **ppweb )
{
   return m_punk->QueryInterface( IID_IWebBrowser2, (PVOID *) ppweb );
}

// WebCtrl

CWebCtrl::CWebCtrl( HWND hwnd )
{
   m_pContainer = NULL;
   m_pweb = NULL;
   m_hwnd = hwnd;
   m_cxScroll = GetSystemMetrics( SM_CXHSCROLL ) + 2;
   m_cyScroll = GetSystemMetrics( SM_CYVSCROLL ) + 2;
}

LRESULT CALLBACK WebCtrlProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{	
   CWebCtrl *pthis = (CWebCtrl *) GetProp( hwnd, _T( "this" ) );

   if ( !pthis )
   {
      if ( uMsg != WM_CREATE )
         return DefWindowProc( hwnd, uMsg, wParam, lParam );

      pthis = new CWebCtrl( hwnd );

      SetProp( hwnd, _T( "this" ), (HANDLE) pthis );
   }

   LRESULT lResult = pthis->WebCtrlProc( uMsg, wParam, lParam );

   if ( uMsg == WM_DESTROY )
   {
	   RemoveProp( hwnd, _T( "this" ) );
      delete pthis;
   }

   return lResult;
}

void CWebCtrl::Navigate( LPTSTR psz )
{
   if ( !psz )
      return;

   if ( !*psz )
      return;

   int len = lstrlen( psz ) + 1;

   WCHAR *pszW;

#ifndef UNICODE

   pszW = new WCHAR[len];

   MultiByteToWideChar( CP_ACP, 0, psz, -1, pszW, len );

#else

   pszW = psz;

#endif

   VARIANT v;

   VariantInit( &v );

   v.vt = VT_BSTR;
   v.bstrVal = SysAllocString( pszW );

#ifndef UNICODE

   delete []pszW;

#endif

   m_pweb->Navigate2( &v, NULL, NULL, NULL, NULL );

   VariantClear( &v );
}

LRESULT CWebCtrl::WebCtrlProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
   LRESULT lResult = 0;  

   switch ( uMsg )
   {
      case WM_CREATE:

         m_pContainer = new CWebContainer( m_hwnd );

         m_pContainer->AddRef();

         m_pContainer->add();

         m_pContainer->getWeb( &m_pweb );

         CREATESTRUCT *pcs;

         pcs = (CREATESTRUCT *) lParam;

         Navigate( (LPTSTR) pcs->lpszName );
		 x = pcs->cx;
		 y = pcs->cy;
         break;

      case WM_SETTEXT:
         Navigate( (LPTSTR) lParam );
         break;

	  case WM_ERASEBKGND:
	  case WM_ACTIVATE:
	  case WM_NCHITTEST:
		  return 0;
		  break;

      case WM_SIZE:
         //m_pContainer->setLocation( 0, 0, 800,600);// LOWORD(lParam) + m_cxScroll, HIWORD(lParam) + m_cyScroll );
		 //m_pContainer->setLocation(0,0, x, y );//LOWORD(lParam) + m_cxScroll, HIWORD(lParam) + m_cyScroll );
         break;

	  case WM_DESTROY:
		  if(m_pweb != NULL)
		  {
			  m_pweb->Release();
			  m_pweb = NULL;
		  }
		  if(m_pContainer != NULL)
		  {
			  m_pContainer->remove();
			  m_pContainer->Release();
			  m_pContainer = NULL;
		  }
		  break;
      default:

         lResult = DefWindowProc( m_hwnd, uMsg, wParam, lParam );

         break;
   }

   return lResult;
}

bool WINAPI InitWebCtrl()
{
   CoInitialize( NULL );

   WNDCLASS wc;

   HINSTANCE hinst = GetModuleHandle( NULL );

   if ( !GetClassInfo( hinst, WC_WEBCTRL, &wc ) )
   {
      memset( &wc, 0, sizeof(wc) );

	  wc.style         = CS_DBLCLKS | CS_GLOBALCLASS | CS_NOCLOSE;
      wc.lpfnWndProc   = ::WebCtrlProc;
      wc.hInstance     = hinst;
      wc.hCursor       = LoadCursor( NULL, (LPCWSTR)IDC_CURSOR );
      wc.lpszClassName = WC_WEBCTRL;

      return RegisterClass( &wc ) ? true : false;
   }
   
   return true;
}

bool WINAPI UninitWebCtrl()
{
   WNDCLASS wc;

   bool bResult = false;

   if ( GetClassInfo( GetModuleHandle( NULL ), WC_WEBCTRL, &wc ) )
      bResult = UnregisterClass( WC_WEBCTRL, wc.hInstance ) ? true : false;

   CoUninitialize();

   return bResult;
}

#ifdef WEBCTRL_AUTOINIT

HWND WINAPI CreateWebControl( LPCTSTR pszText, DWORD dwStyles, int x, int y, int cx, int cy, HWND hwndParent, UINT uId )
{
#pragma warning( disable : 4312 )
   return CreateWindow( WC_WEBCTRL, pszText, dwStyles, x, y, cx, cy, hwndParent, (HMENU) uId, GetModuleHandle( NULL ), NULL );   
#pragma warning( default : 4312 )
}

static class AutoInit
{
public:

   AutoInit()
   {
      InitWebCtrl();
   }

   ~AutoInit()
   {
      UninitWebCtrl();
   }

} a;

#endif