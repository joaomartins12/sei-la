// _DlgInputKey.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgInputKey.h"


// _DlgInputKey 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgInputKey, CDialog)

_DlgInputKey::_DlgInputKey(cConfig::sKEYSET::sKEYINFO* pKeyInfo, CWnd* pParent /*=NULL*/)
	: CDialog(_DlgInputKey::IDD, pParent)
{
	m_pKeyInfo = pKeyInfo;
}

_DlgInputKey::~_DlgInputKey()
{
}

void _DlgInputKey::__PushKey( DWORD vCode )
{
	if( ( ( vCode >= 0x30 )&&( vCode <= 0x39 ) )||
		( ( vCode >= 0x41 )&&( vCode <= 0x5A ) )||		
		// 방향키 위 6개 키
		( vCode == VK_PRIOR )||
		( vCode == VK_NEXT )||
		( vCode == VK_HOME )||
		( vCode == VK_END )||				
		( vCode == VK_INSERT )||
		( vCode == VK_DELETE )||
		// 방향 키
		( vCode == VK_LEFT )||
		( vCode == VK_UP )||
		( vCode == VK_RIGHT )||
		( vCode == VK_DOWN )||		
		// 숫자패드
		( vCode == VK_NUMPAD0 )||
		( vCode == VK_NUMPAD1 )||
		( vCode == VK_NUMPAD2 )||
		( vCode == VK_NUMPAD3 )||
		( vCode == VK_NUMPAD4 )||
		( vCode == VK_NUMPAD5 )||
		( vCode == VK_NUMPAD6 )||
		( vCode == VK_NUMPAD7 )||
		( vCode == VK_NUMPAD8 )||
		( vCode == VK_NUMPAD9 )||
		( vCode == VK_MULTIPLY )||
		( vCode == VK_ADD )||
		( vCode == VK_SUBTRACT )||
		( vCode == VK_DECIMAL )||
		( vCode == VK_DIVIDE )||
		( vCode == VK_NUMLOCK )||
		// F1~F12
		( vCode == VK_F1 )||
		( vCode == VK_F2 )||
		( vCode == VK_F3 )||
		( vCode == VK_F4 )||
		( vCode == VK_F5 )||
		( vCode == VK_F6 )||
		( vCode == VK_F7 )||
		( vCode == VK_F8 )||
		( vCode == VK_F9 )||
		( vCode == VK_F10 )||
		( vCode == VK_F11 )||
		( vCode == VK_F12 )||
		// 특수 문자
		( vCode == VK_OEM_NEC_EQUAL )||		// =
		( vCode == VK_OEM_1 )||				// ;
		( vCode == VK_OEM_PLUS )||			// +
		( vCode == VK_OEM_COMMA )||			// ,
		( vCode == VK_OEM_MINUS )||			// -
		( vCode == VK_OEM_PERIOD )||		// .
		( vCode == VK_OEM_2 )||				// /
		( vCode == VK_OEM_3 )||				// ~
		( vCode == VK_OEM_4 )||				// [
		( vCode == VK_OEM_5 )||				//* \ */
		( vCode == VK_OEM_6 )||				// ]
		( vCode == VK_OEM_7 )||				// "
		// 기타
		( vCode == VK_BACK )||		// 백스페이스
		( vCode == VK_TAB )||		// 탭
		( vCode == VK_SPACE )||		// 스페이스
		( vCode == VK_CAPITAL )		// 캡스 ?
		)
	{
		m_pKeyInfo->s_btKey = (BYTE)vCode;

		if( GET_MV_VIEW->_IsShift() == true )
			m_pKeyInfo->s_dwPlag = KP_SHIPT;
		else if( GET_MV_VIEW->_IsControl() == true )
			m_pKeyInfo->s_dwPlag = KP_CONTROL;
		else if( GET_MV_VIEW->_IsAlt() == true )
			m_pKeyInfo->s_dwPlag = KP_ALT;
		else
			m_pKeyInfo->s_dwPlag = 0;

		OnOK();
		return;
	}
}

void _DlgInputKey::DoDataExchange(CDataExchange* pDX)
{	
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(_DlgInputKey, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL, &_DlgInputKey::OnBnClickedButtonCancel)
END_MESSAGE_MAP()

BOOL _DlgInputKey::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if( pMsg->message == WM_KEYDOWN )
	{
		__PushKey( (DWORD)pMsg->wParam );
	}
	else if( pMsg->message == WM_SYSKEYDOWN )
	{
		__PushKey( (DWORD)pMsg->wParam );
	}	
	return CDialog::PreTranslateMessage(pMsg);
}

void _DlgInputKey::OnBnClickedButtonCancel()
{
	OnCancel();
}
