
#include "stdafx.h"
#include "Window.h"

//=======================================================================================================================
//
//		Base
//
//=======================================================================================================================

void cWindow::Init( cWindow* pParent, CsPoint pos, CsPoint size, bool bApplyWindowSize )
{
	m_pParentWindow = pParent;
	m_bApplyWindowSize = bApplyWindowSize;

	m_ptSize = size;
	m_ptDefault = pos;
	CsPoint xy = m_ptDefault;

	if( m_bApplyWindowSize == true )
	{
		float fRate = (float)m_ptDefault.x/(float)g_ptScreen.x;
		xy.x = (int)( fRate*g_ptScreen.x );
		fRate = (float)m_ptDefault.y/(float)g_ptScreen.y;
		xy.y = (int)( fRate*g_ptScreen.y );
	}

	if( m_pParentWindow != NULL )
	{
		m_ptClient = xy + m_pParentWindow->GetClient();
	}
	else
	{
		m_ptClient = xy;
	}
}

//======================================================================================
//
//		state
//
//======================================================================================
void cWindow::SetPos( CsPoint ptPos )
{
	m_ptDefault = ptPos;
	CsPoint xy = m_ptDefault;

	if( m_bApplyWindowSize == true )
	{
		float fRate = (float)m_ptDefault.x/(float)g_ptScreen.x;
		xy.x = (int)( fRate*g_ptScreen.x );
		fRate = (float)m_ptDefault.y/(float)g_ptScreen.y;
		xy.y = (int)( fRate*g_ptScreen.y );
	}

	if( m_pParentWindow != NULL )
	{
		m_ptClient = xy + m_pParentWindow->GetClient();
	}
	else
	{
		m_ptClient = xy;
	}
}

void cWindow::SetSize( CsPoint ptSize )
{
	m_ptSize = ptSize;
}

//======================================================================================
//
//		µ¿ÀÛ
//
//======================================================================================
void cWindow::SetMove( CsPoint ptDelta )
{
	m_ptDefault += ptDelta;
}

//======================================================================================
//
//		ResetDevice
//
//======================================================================================
void cWindow::ResetDevice( CsPoint pos )
{
	m_ptDefault = pos;
	cWindow::ResetDevice();
}

void cWindow::ResetDevice()
{
	CsPoint xy = m_ptDefault;

	if( m_bApplyWindowSize == true)
	{
		float fRate = (float)m_ptDefault.x/(float)g_ptScreen.x;
		xy.x = (int)( fRate*g_ptScreen.x );
		fRate = (float)m_ptDefault.y/(float)g_ptScreen.y;
		xy.y = (int)( fRate*g_ptScreen.y );
	}

	if( m_pParentWindow != NULL )
	{
		m_ptClient = xy + m_pParentWindow->GetClient();
	}
	else
	{
		m_ptClient = xy;
	}
}



