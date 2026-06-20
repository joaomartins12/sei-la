// _DlgEventSpeed.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgEventSpeed.h"


// _DlgEventSpeed 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgEventSpeed, CDialog)

_DlgEventSpeed::_DlgEventSpeed(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgEventSpeed::IDD, pParent)
{

}

_DlgEventSpeed::~_DlgEventSpeed()
{
}

void _DlgEventSpeed::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDITSPEED, m_EditEventSpeed);
	DDX_Control(pDX, IDC_COMBO_MOVEMOTION, m_ComboMoveMotion);
	DDX_Control(pDX, IDC_SPIN_EVENTSPEED, m_spinEventSpeed);
}


BEGIN_MESSAGE_MAP(_DlgEventSpeed, CDialog)
	ON_BN_CLICKED(IDOK, &_DlgEventSpeed::OnBnClickedOk)
	ON_CBN_SELENDOK(IDC_COMBO_MOVEMOTION, &_DlgEventSpeed::OnCbnSelendokComboMovemotion)
END_MESSAGE_MAP()


// _DlgEventSpeed 메시지 처리기입니다.

BOOL _DlgEventSpeed::OnInitDialog()
{	
	CDialog::OnInitDialog();

	m_spinEventSpeed.SetCsBuddy( &m_EditEventSpeed, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_EditEventSpeed, 0.0f, 3 );	

	int idx = 0;
	if( g_pMainWnd->_GetDlgController()->_GetTarget()->GetProp_Animation()->GetSequenceMng()->IsSequence( ANI::MOVE_RUN ) )
	{		
		m_ComboMoveMotion.InsertString( idx, L"뛰기" );				
		m_ComboMoveMotion.SetItemData( idx, ANI::MOVE_RUN );
		idx++;
	}
	if( g_pMainWnd->_GetDlgController()->_GetTarget()->GetProp_Animation()->GetSequenceMng()->IsSequence( ANI::MOVE_WALK )	 )
	{
		m_ComboMoveMotion.InsertString( idx, L"걷기" );
		m_ComboMoveMotion.SetItemData( idx, ANI::MOVE_WALK );		
		idx++;
	}
	if(	g_pMainWnd->_GetDlgController()->_GetTarget()->GetProp_Animation()->GetSequenceMng()->IsSequence( ANI::MOVE_ATTACKRUN ) )
	{
		m_ComboMoveMotion.InsertString( idx, L"공격뛰기" );
		m_ComboMoveMotion.SetItemData( idx, ANI::MOVE_ATTACKRUN );		
		idx++;
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgEventSpeed::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIndex = m_ComboMoveMotion.GetCurSel();
	if( nIndex != CB_ERR )
	{
		g_pMainWnd->_GetDlgController()->_SetAniEvent( (DWORD)m_ComboMoveMotion.GetItemData( nIndex ) );			
	}	
	g_pMainWnd->_GetDlgController()->_SetEventMoveSpeed( nsCsMfcFunc::Control2Float( &m_EditEventSpeed ) );

	OnOK();
}

void _DlgEventSpeed::OnCbnSelendokComboMovemotion()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIndex = m_ComboMoveMotion.GetCurSel();
	DWORD dwID = (DWORD)m_ComboMoveMotion.GetItemData( nIndex );
	
	switch( dwID )
	{
	case ANI::MOVE_RUN:
		nsCsMfcFunc::Float2Control( &m_EditEventSpeed, 300.0f, 3 );
		break;
	case ANI::MOVE_WALK:
		nsCsMfcFunc::Float2Control( &m_EditEventSpeed, 120.0f, 3 );	
		break;
	case ANI::MOVE_ATTACKRUN:
		nsCsMfcFunc::Float2Control( &m_EditEventSpeed, 350.0f, 3 );	
		break;
	}	
	
}
