// MyHtmlView.cpp : implementation file
//

#include "stdafx.h"
#include "MyHtmlView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyHtmlView

IMPLEMENT_DYNCREATE(CMyHtmlView, CHtmlView)

CMyHtmlView::CMyHtmlView()
{
	//{{AFX_DATA_INIT(CMyHtmlView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CMyHtmlView::~CMyHtmlView()
{
}

void CMyHtmlView::DoDataExchange(CDataExchange* pDX)
{
	CHtmlView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMyHtmlView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMyHtmlView, CHtmlView)
	//{{AFX_MSG_MAP(CMyHtmlView)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyHtmlView diagnostics

#ifdef _DEBUG
void CMyHtmlView::AssertValid() const
{
	CHtmlView::AssertValid();
}

void CMyHtmlView::Dump(CDumpContext& dc) const
{
	CHtmlView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMyHtmlView message handlers

HRESULT CMyHtmlView::OnGetHostInfo(DOCHOSTUIINFO *pInfo) 
{
	pInfo->dwFlags = DOCHOSTUIFLAG_SCROLL_NO ;
	return CHtmlView::OnGetHostInfo(pInfo);
}

void CMyHtmlView::OnDocumentComplete(LPCTSTR lpszURL) 
{
	CHtmlView::OnDocumentComplete(lpszURL);
}

BOOL CMyHtmlView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CHtmlView::OnEraseBkgnd(pDC);
}
