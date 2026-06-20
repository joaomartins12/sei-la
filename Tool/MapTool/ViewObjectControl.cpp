
#include "stdafx.h"
#include "ViewObjectControl.h"

CViewObjectControl::CViewObjectControl()
{

}

CViewObjectControl::~CViewObjectControl()
{

}

void CViewObjectControl::_Init()
{
	CViewBase::_Init();

	CRect rc;
	this->GetWindowRect( &rc );
	m_DlgObjectControl.Create( _DlgObjectControl::IDD, this );
	m_DlgObjectControl.SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(), 0 );
	m_DlgObjectControl.ShowWindow( SW_SHOW );
}