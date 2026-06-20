// _DlgEventTime.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgEventTime.h"


// _DlgEventTime 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgEventTime, CDialog)

_DlgEventTime::_DlgEventTime(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgEventTime::IDD, pParent)
{

}

_DlgEventTime::~_DlgEventTime()
{
}

void _DlgEventTime::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TIME, m_EditEventtime);
	DDX_Control(pDX, IDC_SPIN_EVENTTIME, m_spinEventtime);
	DDX_Control(pDX, IDC_EDIT_TIME2, m_Edit_StartTime);
	DDX_Control(pDX, IDC_SPIN_EVENTTIME2, m_spinStartTime);
}


BEGIN_MESSAGE_MAP(_DlgEventTime, CDialog)
	ON_BN_CLICKED(IDOK, &_DlgEventTime::OnBnClickedOk)		
END_MESSAGE_MAP()


// _DlgEventTime 메시지 처리기입니다.

BOOL _DlgEventTime::OnInitDialog()
{	
	CDialog::OnInitDialog();

	m_spinEventtime.SetCsBuddy( &m_EditEventtime, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_EditEventtime, 0.0f, 3 );

	m_spinStartTime.SetCsBuddy( &m_Edit_StartTime, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_Edit_StartTime, 0.0f, 3 );
	
	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgEventTime::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	g_pMainWnd->_GetDlgController()->_SetEventTime( nsCsMfcFunc::Control2Float( &m_Edit_StartTime ), nsCsMfcFunc::Control2Float( &m_EditEventtime ) );

	OnOK();
}


