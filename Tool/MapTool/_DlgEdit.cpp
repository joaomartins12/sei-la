// _DlgEdit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgEdit.h"


// _DlgEdit 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgEdit, CDialog)

_DlgEdit::_DlgEdit( TCHAR* pszCaption, TCHAR* pszEdit, TCHAR* pszInitEdit, int nMaxEditLen, CWnd* pParent /*=NULL*/)
	: CDialog(_DlgEdit::IDD, pParent)
{
	m_pszCaption = pszCaption;
	m_pszEdit = pszEdit;
	m_pszInitEdit = pszInitEdit;
	m_nMaxEditLen = nMaxEditLen;	
}

_DlgEdit::~_DlgEdit()
{
}

void _DlgEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DATA, m_edit_Data);
}


BEGIN_MESSAGE_MAP(_DlgEdit, CDialog)
	ON_BN_CLICKED(IDC_BTN_OK, &_DlgEdit::OnBnClickedBtnOk)
END_MESSAGE_MAP()


// _DlgEdit 메시지 처리기입니다.

void _DlgEdit::OnBnClickedBtnOk()
{
	m_edit_Data.GetWindowText( m_pszEdit, m_nMaxEditLen );
	if( m_pszEdit[ 0 ] == NULL )
	{
		MessageBox( L"에디트 박스 안에 내용을 기입하여 주십시오." );
		return;
	}
	CDialog::OnOK();
}

BOOL _DlgEdit::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowText( m_pszCaption );
	if( m_pszInitEdit != NULL )
	{
		m_edit_Data.SetWindowText( m_pszInitEdit );
	}

	return TRUE;
}
