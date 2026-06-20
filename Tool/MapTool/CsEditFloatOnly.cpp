
#include "stdafx.h"
#include "CsEditFloatOnly.h"

BOOL CsEditFloatOnly::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		DWORD vKey = (DWORD)pMsg->wParam;
		if( ( ( ( vKey >= '0' )&&( vKey <= '9' ) )||( ( vKey >= VK_NUMPAD0 )&&( vKey <= VK_NUMPAD9 ) )||( vKey == VK_DECIMAL )||( vKey == VK_OEM_PERIOD )||( vKey == VK_OEM_MINUS )||
			( vKey == VK_SUBTRACT )||( vKey == VK_LEFT )||( vKey == VK_RIGHT )||( vKey == VK_DELETE )||( vKey == VK_BACK )||
			( vKey == VK_SHIFT )||( vKey == VK_CONTROL ) ) == false )
		{
			if( ( ( GET_MV_VIEW->_IsControl() == true )&&( ( vKey == 'C' )||( vKey == 'V' ) ) ) == false )
			{
				GET_MV_VIEW->SetFocus();
				GET_MV_VIEW->_KeyDown( vKey );
				return true;
			}
		}
	}
	if( pMsg->message == WM_SYSKEYDOWN )
	{
		DWORD vKey = (DWORD)pMsg->wParam;
		if( ( ( ( vKey >= '0' )&&( vKey <= '9' ) )||( ( vKey >= VK_NUMPAD0 )&&( vKey <= VK_NUMPAD9 ) )||( vKey == VK_DECIMAL )||( vKey == VK_OEM_PERIOD )||( vKey == VK_OEM_MINUS )||
			( vKey == VK_SUBTRACT )||( vKey == VK_LEFT )||( vKey == VK_RIGHT )||( vKey == VK_DELETE )||( vKey == VK_BACK )||
			( vKey == VK_SHIFT )||( vKey == VK_CONTROL )||( vKey == VK_MENU ) ) == false )
		{
			if( ( ( GET_MV_VIEW->_IsControl() == true )&&( ( vKey == 'C' )||( vKey == 'V' ) ) ) == false )
			{
				GET_MV_VIEW->SetFocus();
				GET_MV_VIEW->_KeyDown( vKey );
				return true;
			}			
		}
	}

	return CsEditFosus::PreTranslateMessage(pMsg);
}
