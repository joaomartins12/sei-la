// WinTop.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "WinTop.h"



WinTop*		g_pTop = NULL;

// WinTop

IMPLEMENT_DYNCREATE(WinTop, CFormView)

WinTop::WinTop()
	: CFormView(WinTop::IDD)
{

}

WinTop::~WinTop()
{
}


void WinTop::_Init()
{
	m_combo_MiddleType.InsertString( cQuestObj::MT_LINE, L"라인" );
	m_combo_MiddleType.InsertString( cQuestObj::MT_JUMP, L"점프" );
	m_combo_MiddleType.InsertString( cQuestObj::MT_LINEJUMP, L"라인&점프" );

	m_combo_MiddleType.SetCurSel( cQuestObj::MT_LINEJUMP );
	OnCbnSelchangeComboMiddlttailType();	

	m_combo_ViewType.InsertString( WinViewer::VS_VIEWER, L"GUI" );
	m_combo_ViewType.InsertString( WinViewer::VS_LIST, L"List" );
	m_combo_ViewType.InsertString( WinViewer::VS_SIMPLE, L"Simple" );

	m_combo_ViewType.SetCurSel( WinViewer::VS_VIEWER );
	OnCbnSelchangeComboViewtype();

	nsCsMfcFunc::Float2Control( &m_edit_ExpMul, 1.0f, 4 );

	// 미니맵 - 기본 보기로
	m_chk_ViewMinimap.SetCheck( BST_CHECKED );
}

void WinTop::_SetViewState( WinViewer::eVIEWSTATE eState )
{
	if( (int)eState == m_combo_ViewType.GetCurSel() )
		return;

	m_combo_ViewType.SetCurSel( eState );
	OnCbnSelchangeComboViewtype();
}

void WinTop::OnCbnSelchangeComboMiddlttailType()
{
	g_pViewer->_GetQuestMng()->CreateMiddleType( (cQuestObj::eMIDDLETAIL_TYPE)m_combo_MiddleType.GetCurSel() );
	g_pViewer->SetFocus();
}


void WinTop::OnCbnSelchangeComboViewtype()
{
	g_pViewer->_SetViewState( (WinViewer::eVIEWSTATE)m_combo_ViewType.GetCurSel() );
	g_pViewer->SetFocus();
}


void WinTop::OnBnClickedButtonFindwin()
{
	_DlgModal_Find dlg;
	dlg.DoModal();
}

void WinTop::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MIDDLTTAIL_TYPE, m_combo_MiddleType);
	DDX_Control(pDX, IDC_CHECK_VIEWMINIMAP, m_chk_ViewMinimap);
	DDX_Control(pDX, IDC_COMBO_VIEWTYPE, m_combo_ViewType);
	DDX_Control(pDX, IDC_EDIT_EXP_MUL, m_edit_ExpMul);
}

BEGIN_MESSAGE_MAP(WinTop, CFormView)
	ON_CBN_SELCHANGE(IDC_COMBO_MIDDLTTAIL_TYPE, &WinTop::OnCbnSelchangeComboMiddlttailType)
	ON_CBN_SELCHANGE(IDC_COMBO_VIEWTYPE, &WinTop::OnCbnSelchangeComboViewtype)
	ON_BN_CLICKED(IDC_BUTTON_FINDWIN, &WinTop::OnBnClickedButtonFindwin)
END_MESSAGE_MAP()


