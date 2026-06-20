
#include "stdafx.h"
#include "ViewObjectList.h"

CViewObjectList::CViewObjectList()
{

}

CViewObjectList::~CViewObjectList()
{

}

void CViewObjectList::_Init()
{
	CViewBase::_Init();

	CRect rc;
	this->GetWindowRect( &rc );
	m_DlgObjectList.Create( _DlgObjectList::IDD, this );
	m_DlgObjectList.SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(), 0 );
	m_DlgObjectList.ShowWindow( SW_SHOW );
}BEGIN_MESSAGE_MAP(CViewObjectList, CViewBase)
ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()

void CViewObjectList::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CViewBase::OnShowWindow(bShow, nStatus);
}
