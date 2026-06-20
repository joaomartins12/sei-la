

#include "stdafx.h"
#include "ViewSetUp.h"
CViewSetUp::CViewSetUp()
{

}

CViewSetUp::~CViewSetUp()
{

}

void CViewSetUp::_Init()
{
	CViewBase::_Init();

	CRect rc;
	this->GetWindowRect( &rc );	
	m_DlgSetUp.Create( _DlgSetup::IDD, this );
	m_DlgSetUp.SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(), 0 );
	m_DlgSetUp.ShowWindow( SW_SHOW );
}