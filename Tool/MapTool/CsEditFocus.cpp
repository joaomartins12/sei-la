
#include "stdafx.h"
#include "CsEditFocus.h"

//#ifdef _DEBUG
//	#define new DEBUG_NEW
//#endif

BEGIN_MESSAGE_MAP(CsEditFosus, CsEdit)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CsEditFosus::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	CFrameBase* pViewFrm = (*theApp._GetMapFrame())[ nsMAPTOOL::MV_VIEWER ];
	if( pViewFrm == NULL )
		return;

	CViewViewer* pView = (CViewViewer*)pViewFrm->_GetView();
	if( pView == NULL )
		return;

	pView->_EnableConfigKey( false );
}

void CsEditFosus::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	CFrameBase* pViewFrm = (*theApp._GetMapFrame())[ nsMAPTOOL::MV_VIEWER ];
	if( pViewFrm == NULL )
		return;

	CViewViewer* pView = (CViewViewer*)pViewFrm->_GetView();
	if( pView == NULL )
		return;

	pView->_EnableConfigKey( true );
}
