
#include "stdafx.h"
#include "ViewMainTab.h"

CViewMainTab::CViewMainTab()
{

}

CViewMainTab::~CViewMainTab()
{

}

void CViewMainTab::_Init()
{
	CViewBase::_Init();

	CRect rc;
	this->GetWindowRect( &rc );	
	m_DlgMainTab.Create( _DlgMainTab::IDD, this );
	m_DlgMainTab.SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(), 0 );
	m_DlgMainTab.ShowWindow( SW_SHOW );	
}