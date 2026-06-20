
#include "stdafx.h"
#include "ViewNode.h"

CViewNode::CViewNode()
{

}

CViewNode::~CViewNode()
{

}

void CViewNode::_Init()
{
	CViewBase::_Init();

	CRect rc;
	this->GetWindowRect( &rc );	
	m_DlgNode.Create( _DlgNode::IDD, this );
	m_DlgNode.SetWindowPos( NULL, 0, 0, rc.Width(), rc.Height(), 0 );
	m_DlgNode.ShowWindow( SW_SHOW );
}