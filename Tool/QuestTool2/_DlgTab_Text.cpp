// _DlgTab_Text.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgTab_Text.h"


// _DlgTab_Text 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgTab_Text, CDialog)

_DlgTab_Text::_DlgTab_Text(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgTab_Text::IDD, pParent)
{

}

_DlgTab_Text::~_DlgTab_Text()
{
}

void _DlgTab_Text::_Init()
{
	m_edit_Title.SetLimitText( QUEST_TITLE_LEN - 1 );
	m_edit_Tab.SetLimitText( QUEST_TITLE_LEN - 1 );
	m_edit_Body.SetLimitText( QUEST_BODY_LEN - 1 );
	m_edit_Ing.SetLimitText( QUEST_HELPER_LEN - 1 );
	m_edit_Comp.SetLimitText( QUEST_COMPLEATE_LEN - 1 );
	m_edit_Helper.SetLimitText( QUEST_HELPER_LEN - 1 );
	m_edit_Ext.SetLimitText( QUEST_HELPER_LEN - 1 );
	m_edit_Simple.SetLimitText( QUEST_SIMPLE_LEN - 1 );
}

void _DlgTab_Text::_SetQuest( cQuestObj* pQuestObj )
{
	if( pQuestObj == NULL )
	{
		// 퀘스트 ID
		m_edit_UniqID.ShowWindow( SW_HIDE );

		// 제목
		m_edit_Title.ShowWindow( SW_HIDE );

		// 탭
		m_edit_Tab.ShowWindow( SW_HIDE );

		// 바디
		m_edit_Body.ShowWindow( SW_HIDE );
		m_edit_BodyLen.ShowWindow( SW_HIDE );

		// 퀘스트 중
		m_edit_Ing.ShowWindow( SW_HIDE );		
		m_edit_IngLen.ShowWindow( SW_HIDE );

		// 퀘스트 완료
		m_edit_Comp.ShowWindow( SW_HIDE );
		m_edit_CompLen.ShowWindow( SW_HIDE );

		// 퀘스트 확장
		m_edit_Ext.ShowWindow( SW_HIDE );
		m_edit_ExtLen.ShowWindow( SW_HIDE );

		// 퀘스트 간략화
		m_edit_Simple.ShowWindow( SW_HIDE );
		m_edit_SimpleLen.ShowWindow( SW_HIDE );

		// 퀘스트 도우미
		m_edit_Helper.ShowWindow( SW_HIDE );
		m_edit_HelperLen.ShowWindow( SW_HIDE );
	}
	else
	{
		CsQuest* pFTQuest = pQuestObj->GetFT();

		// 퀘스트 ID
		m_edit_UniqID.ShowWindow( SW_SHOW );
		nsCsMfcFunc::Int2Control( &m_edit_UniqID, pFTQuest->GetUniqID() );

		// 퀘스트 제목
		m_edit_Title.ShowWindow( SW_SHOW );
		m_edit_Title.SetWindowText( pFTQuest->m_szTitleText );

		// 퀘스트 탭
		m_edit_Tab.ShowWindow( SW_SHOW );
		m_edit_Tab.SetWindowText( pFTQuest->m_szTitleTab );

		// 바디
		m_edit_Body.ShowWindow( SW_SHOW );
		m_edit_BodyLen.ShowWindow( SW_SHOW );
		m_edit_Body.SetWindowText( pFTQuest->m_szBodyText );
		OnEnChangeEditBody();

		// 퀘스트 중
		m_edit_Ing.ShowWindow( SW_SHOW );
		m_edit_IngLen.ShowWindow( SW_SHOW );
		m_edit_Ing.SetWindowText( pFTQuest->m_szProcess );
		OnEnChangeEditIng();

		// 퀘스트 완료
		m_edit_Comp.ShowWindow( SW_SHOW );
		m_edit_CompLen.ShowWindow( SW_SHOW );
		m_edit_Comp.SetWindowText( pFTQuest->m_szCompleate );
		OnEnChangeEditComp();

		// 퀘스트 확장
		m_edit_Ext.ShowWindow( SW_SHOW );
		m_edit_ExtLen.ShowWindow( SW_SHOW );
		m_edit_Ext.SetWindowText( pFTQuest->m_szExpert );
		OnEnChangeEditExt();

		// 퀘스트 간략화
		m_edit_Simple.ShowWindow( SW_SHOW );
		m_edit_SimpleLen.ShowWindow( SW_SHOW );
		m_edit_Simple.SetWindowText( pFTQuest->m_szSimpleText );
		OnEnChangeEditSimple();

		// 퀘스트 도우미
		m_edit_Helper.ShowWindow( SW_SHOW );
		m_edit_HelperLen.ShowWindow( SW_SHOW );
		m_edit_Helper.SetWindowText( pFTQuest->m_szHelperText );
		OnEnChangeEditHelper();
	}
}

void _DlgTab_Text::OnEnChangeEditTitle()
{
	cQuestObj* pQuestObj = g_pDiscript->_GetSelQuest();
	CsQuest* pFTQuest = pQuestObj->GetFT();
	m_edit_Title.GetWindowText( pFTQuest->m_szTitleText, QUEST_TITLE_LEN );

	pQuestObj->ResetControl();
}

void _DlgTab_Text::OnEnChangeEditTab()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	m_edit_Tab.GetWindowText( pFTQuest->m_szTitleTab, QUEST_TITLE_LEN );
}


void _DlgTab_Text::OnEnChangeEditBody()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	m_edit_Body.GetWindowText( pFTQuest->m_szBodyText, QUEST_BODY_LEN );

	int nLen = (int)_tcslen( pFTQuest->m_szBodyText );
	TCHAR sz[ 32 ];
	_stprintf_s( sz, 32, L"%d / %d", nLen, QUEST_BODY_LEN - 1 );
	m_edit_BodyLen.SetWindowText( sz );
}

void _DlgTab_Text::OnEnChangeEditIng()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();

	m_edit_Ing.GetWindowText( pFTQuest->m_szProcess, QUEST_HELPER_LEN );

	int nLen = (int)_tcslen( pFTQuest->m_szProcess );
	TCHAR sz[ 32 ];
	_stprintf_s( sz, 32, L"%d / %d", nLen, QUEST_HELPER_LEN - 1 );
	m_edit_IngLen.SetWindowText( sz );
}


void _DlgTab_Text::OnEnChangeEditComp()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();

	m_edit_Comp.GetWindowText( pFTQuest->m_szCompleate, QUEST_COMPLEATE_LEN );

	int nLen = (int)_tcslen( pFTQuest->m_szCompleate );
	TCHAR sz[ 32 ];
	_stprintf_s( sz, 32, L"%d / %d", nLen, QUEST_COMPLEATE_LEN - 1 );
	m_edit_CompLen.SetWindowText( sz );
}

void _DlgTab_Text::OnEnChangeEditExt()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();

	m_edit_Ext.GetWindowText( pFTQuest->m_szExpert, QUEST_HELPER_LEN );

	int nLen = (int)_tcslen( pFTQuest->m_szExpert );
	TCHAR sz[ 32 ];
	_stprintf_s( sz, 32, L"%d / %d", nLen, QUEST_HELPER_LEN - 1 );
	m_edit_ExtLen.SetWindowText( sz );
}

void _DlgTab_Text::OnEnChangeEditSimple()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();

	m_edit_Simple.GetWindowText( pFTQuest->m_szSimpleText, QUEST_SIMPLE_LEN );

	int nLen = (int)_tcslen( pFTQuest->m_szSimpleText );
	TCHAR sz[ 32 ];
	_stprintf_s( sz, 32, L"%d / %d", nLen, QUEST_SIMPLE_LEN - 1 );
	m_edit_SimpleLen.SetWindowText( sz );
}

void _DlgTab_Text::OnEnChangeEditHelper()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();

	m_edit_Helper.GetWindowText( pFTQuest->m_szHelperText, QUEST_HELPER_LEN );

	int nLen = (int)_tcslen( pFTQuest->m_szHelperText );
	TCHAR sz[ 32 ];
	_stprintf_s( sz, 32, L"%d / %d", nLen, QUEST_HELPER_LEN - 1 );
	m_edit_HelperLen.SetWindowText( sz );
}















void _DlgTab_Text::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_UNIQID, m_edit_UniqID);
	DDX_Control(pDX, IDC_EDIT_TITLE, m_edit_Title);
	DDX_Control(pDX, IDC_EDIT_TAB, m_edit_Tab);
	DDX_Control(pDX, IDC_EDIT_BODY, m_edit_Body);
	DDX_Control(pDX, IDC_EDIT_BODY_LEN, m_edit_BodyLen);
	DDX_Control(pDX, IDC_EDIT_ING, m_edit_Ing);
	DDX_Control(pDX, IDC_EDIT_ING_LEN, m_edit_IngLen);
	DDX_Control(pDX, IDC_EDIT_COMP, m_edit_Comp);
	DDX_Control(pDX, IDC_EDIT_COMP_LEN, m_edit_CompLen);
	DDX_Control(pDX, IDC_EDIT_EXT, m_edit_Ext);
	DDX_Control(pDX, IDC_EDIT_EXT_LEN, m_edit_ExtLen);
	DDX_Control(pDX, IDC_EDIT_SIMPLE, m_edit_Simple);
	DDX_Control(pDX, IDC_EDIT_SIMPLE_LEN, m_edit_SimpleLen);
	DDX_Control(pDX, IDC_EDIT_HELPER, m_edit_Helper);
	DDX_Control(pDX, IDC_EDIT_HELPER_LEN, m_edit_HelperLen);
}


BEGIN_MESSAGE_MAP(_DlgTab_Text, CDialog)
	ON_EN_CHANGE(IDC_EDIT_BODY, &_DlgTab_Text::OnEnChangeEditBody)
	ON_EN_CHANGE(IDC_EDIT_ING, &_DlgTab_Text::OnEnChangeEditIng)
	ON_EN_CHANGE(IDC_EDIT_COMP, &_DlgTab_Text::OnEnChangeEditComp)
	ON_EN_CHANGE(IDC_EDIT_EXT, &_DlgTab_Text::OnEnChangeEditExt)
	ON_EN_CHANGE(IDC_EDIT_SIMPLE, &_DlgTab_Text::OnEnChangeEditSimple)
	ON_EN_CHANGE(IDC_EDIT_HELPER, &_DlgTab_Text::OnEnChangeEditHelper)
	ON_EN_CHANGE(IDC_EDIT_TITLE, &_DlgTab_Text::OnEnChangeEditTitle)
	ON_EN_CHANGE(IDC_EDIT_TAB, &_DlgTab_Text::OnEnChangeEditTab)
END_MESSAGE_MAP()


// _DlgTab_Text 메시지 처리기입니다.

