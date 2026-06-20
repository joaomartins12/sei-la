#include "stdafx.h"
#include "AdaptMainFrame.h"
#include "../00.Contents/MainFrame_Contents.h"

AdaptMainFrame::AdaptMainFrame(void) : m_pContents( NULL )
{

}

AdaptMainFrame::~AdaptMainFrame(void)
{

}

bool AdaptMainFrame::SetContents(IUIContentBase* pContents)
{
	SAFE_POINTER_RETVAL( pContents, false );
	if( E_CT_MAINFRAME_CONTENTS != pContents->GetContentsIdentity() )
		return false;

	m_pContents = static_cast< CMainFrameContents* >( pContents );

	return true;
}

CsC_AvObject* AdaptMainFrame::GetTargetObject() const
{
	SAFE_POINTER_RETVAL( m_pContents, NULL );
	return m_pContents->GetTargetObject();
}

