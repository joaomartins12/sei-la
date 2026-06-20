// _DlgTimeChange.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgTimeChange.h"


// _DlgTimeChange 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgTimeChange, CDialog)

_DlgTimeChange::_DlgTimeChange(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgTimeChange::IDD, pParent)
{

}

_DlgTimeChange::~_DlgTimeChange()
{
}

void _DlgTimeChange::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditTime);
}


BEGIN_MESSAGE_MAP(_DlgTimeChange, CDialog)
	ON_BN_CLICKED(IDOK, &_DlgTimeChange::OnBnClickedOk)
END_MESSAGE_MAP()


// _DlgTimeChange 메시지 처리기입니다.
BOOL _DlgTimeChange::OnInitDialog()
{	
	CDialog::OnInitDialog();

	nsCsMfcFunc::Float2Control( &m_EditTime, 0.0f, 3 );

	return TRUE;  // return TRUE unless you set the focus to a control
}
void _DlgTimeChange::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_fTime = nsCsMfcFunc::Control2Float( &m_EditTime );

	OnOK();
}
