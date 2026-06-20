// WinDiscript.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "resource.h"
#include "WinDiscript.h"


WinDiscript*		g_pDiscript = NULL;


// WinDiscript

IMPLEMENT_DYNCREATE(WinDiscript, CFormView)

WinDiscript::WinDiscript()
	: CFormView(WinDiscript::IDD)
{

}

WinDiscript::~WinDiscript()
{
}


void WinDiscript::_Init()
{
	m_MainTab.InsertItem( TT_SETTING, L"셋팅" );
	m_MainTab.InsertItem( TT_TEXT, L"텍스트" );
	m_MainTab.InsertItem( TT_EVENT, L"이벤트" );
	m_MainTab.SetCurSel( TT_SETTING );


	CRect rc;
	GetWindowRect( &rc );
	int nHeight = rc.Height() - 25;

	// 윈도우 연결
	m_DlgSetting.Create( _DlgTab_Setting::IDD, this );
	m_DlgSetting.SetWindowPos( NULL, 0, 25, rc.Width(), nHeight, 0 );
	m_DlgSetting.ShowWindow( SW_SHOW );
	m_DlgSetting._Init();

	m_DlgText.Create( _DlgTab_Text::IDD, this );
	m_DlgText.SetWindowPos( NULL, 0, 25, rc.Width(), nHeight, 0 );
	m_DlgText.ShowWindow( SW_HIDE );
	m_DlgText._Init();

	m_DlgEvent.Create( _DlgTab_Event::IDD, this );
	m_DlgEvent.SetWindowPos( NULL, 0, 25, rc.Width(), nHeight, 0 );
	m_DlgEvent.ShowWindow( SW_HIDE );
	m_DlgEvent._Init();

	m_pSelQuest = NULL;
}

// WinDiscript 메시지 처리기입니다.

void WinDiscript::OnTcnSelchangeTabMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	switch( m_MainTab.GetCurSel() )
	{
	case TT_SETTING:
		m_DlgSetting._SetQuest( m_pSelQuest );

		m_DlgSetting.ShowWindow( SW_SHOW );
		m_DlgText.ShowWindow( SW_HIDE );
		m_DlgEvent.ShowWindow( SW_HIDE );
		break;
	case TT_TEXT:
		m_DlgText._SetQuest( m_pSelQuest );

		m_DlgSetting.ShowWindow( SW_HIDE );
		m_DlgText.ShowWindow( SW_SHOW );
		m_DlgEvent.ShowWindow( SW_HIDE );
		break;
	case TT_EVENT:
		m_DlgEvent._SetQuest( m_pSelQuest );

		m_DlgSetting.ShowWindow( SW_HIDE );
		m_DlgText.ShowWindow( SW_HIDE );
		m_DlgEvent.ShowWindow( SW_SHOW );
		break;
	default:
		assert_cs( false );
	}

	*pResult = 0;
}


void WinDiscript::_SetSelQuest( cQuestObj* pQuestObj )
{
	m_pSelQuest = pQuestObj;
	
	switch( m_MainTab.GetCurSel() )
	{
	case TT_SETTING:
		m_DlgSetting._SetQuest( m_pSelQuest );
		break;
	case TT_TEXT:
		m_DlgText._SetQuest( m_pSelQuest );
		break;
	case TT_EVENT:
		m_DlgEvent._SetQuest( m_pSelQuest );
		break;
	default:
		assert_cs( false );
	}
}





void WinDiscript::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_MAIN, m_MainTab);
}

BEGIN_MESSAGE_MAP(WinDiscript, CFormView)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_MAIN, &WinDiscript::OnTcnSelchangeTabMain)
END_MESSAGE_MAP()


// WinDiscript 진단입니다.

#ifdef _DEBUG
void WinDiscript::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void WinDiscript::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


