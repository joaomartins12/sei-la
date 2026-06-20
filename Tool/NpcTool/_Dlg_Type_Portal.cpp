// _Dlg_Type_Portal.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NpcTool.h"
#include "_Dlg_Type_Portal.h"


// _Dlg_Type_Portal 대화 상자입니다.

IMPLEMENT_DYNAMIC(_Dlg_Type_Portal, CDialog)

_Dlg_Type_Portal::_Dlg_Type_Portal(CWnd* pParent /*=NULL*/)
	: CDialog(_Dlg_Type_Portal::IDD, pParent)
{

}

_Dlg_Type_Portal::~_Dlg_Type_Portal()
{
}

void _Dlg_Type_Portal::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_DISAPPROVE, m_combo_Disapprove);
	DDX_Control(pDX, IDC_EDIT_DISCRITP, m_edit_Discript);
	DDX_Control(pDX, IDC_EDIT_ENABLEID, m_edit_EnableID);
	DDX_Control(pDX, IDC_EDIT_ENABLECOUNT, m_edit_EnableCount);
	DDX_Control(pDX, IDC_LIST_PORTAL, m_list_Portal);
	DDX_Control(pDX, IDC_LIST_REQ, m_list_Req);
	DDX_Control(pDX, IDC_EDIT_EVENTID, m_edit_EventID);
	DDX_Control(pDX, IDC_RADIO1, m_Radio_Train);
	DDX_Control(pDX, IDC_RADIO2, m_Radio_Normal);
	DDX_Control(pDX, IDC_RADIO3, m_Radio_None);
}


BEGIN_MESSAGE_MAP(_Dlg_Type_Portal, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_TABLE, &_Dlg_Type_Portal::OnBnClickedButtonTable)
	ON_EN_CHANGE(IDC_EDIT_ENABLEID, &_Dlg_Type_Portal::OnEnChangeEditEnableid)
	ON_EN_CHANGE(IDC_EDIT_ENABLECOUNT, &_Dlg_Type_Portal::OnEnChangeEditEnablecount)
	ON_CBN_SELCHANGE(IDC_COMBO_DISAPPROVE, &_Dlg_Type_Portal::OnCbnSelchangeComboDisapprove)
	ON_BN_CLICKED(IDC_BUTTON_ADDPORTAL, &_Dlg_Type_Portal::OnBnClickedButtonAddportal)
	ON_BN_CLICKED(IDC_BUTTON_DELPORTAL, &_Dlg_Type_Portal::OnBnClickedButtonDelportal)
	ON_NOTIFY(NM_CLICK, IDC_LIST_PORTAL, &_Dlg_Type_Portal::OnNMClickListPortal)
	ON_NOTIFY(NM_CLICK, IDC_LIST_REQ, &_Dlg_Type_Portal::OnNMClickListReq)
	ON_EN_CHANGE(IDC_EDIT_EVENTID, &_Dlg_Type_Portal::OnEnChangeEditEventid)
	ON_BN_CLICKED(IDC_RADIO1, &_Dlg_Type_Portal::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &_Dlg_Type_Portal::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &_Dlg_Type_Portal::OnBnClickedRadio3)
END_MESSAGE_MAP()


// _Dlg_Type_Portal 메시지 처리기입니다.

BOOL _Dlg_Type_Portal::OnInitDialog()
{
	m_pCurType = NULL;
	m_pCurReq = NULL;

	CDialog::OnInitDialog();

	m_combo_Disapprove.InsertString( sNPC_TYPE_PORTAL::None, L"없음" );	
	m_combo_Disapprove.InsertString( sNPC_TYPE_PORTAL::Item, L"아이템" );
	m_combo_Disapprove.InsertString( sNPC_TYPE_PORTAL::Quest, L"퀘스트" );
	m_combo_Disapprove.InsertString( sNPC_TYPE_PORTAL::TMinLevel, L"최소 테이머 레벨" );
	m_combo_Disapprove.InsertString( sNPC_TYPE_PORTAL::Money, L"돈" );
	m_combo_Disapprove.InsertString( sNPC_TYPE_PORTAL::TMaxLevel, L"최대 테이머 레벨" );
	m_combo_Disapprove.InsertString( sNPC_TYPE_PORTAL::DMinLevel, L"최소 디지몬 레벨" );
	m_combo_Disapprove.InsertString( sNPC_TYPE_PORTAL::DMaxLevel, L"최대 디지몬 레벨" );

	DWORD dwExStyle = m_list_Portal.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Portal.SetExtendedStyle( dwExStyle );
	m_list_Portal.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 80 );
	m_list_Portal.InsertColumn( 1, L"포탈 요구 선행 조건", LVCFMT_LEFT, 160 );
	m_list_Portal.InsertColumn( 2, L"이벤트가동", LVCFMT_LEFT, 100 );

	dwExStyle = m_list_Req.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Req.SetExtendedStyle( dwExStyle );
	m_list_Req.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
	m_list_Req.InsertColumn( 1, L"타입", LVCFMT_LEFT, 120 );
	m_list_Req.InsertColumn( 2, L"ID", LVCFMT_LEFT, 80 );
	m_list_Req.InsertColumn( 3, L"갯수", LVCFMT_LEFT, 80 );

	m_Radio_Train.SetCheck(BST_UNCHECKED);	
	m_Radio_Normal.SetCheck(BST_UNCHECKED);
	m_Radio_None.SetCheck(BST_UNCHECKED);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void _Dlg_Type_Portal::_SetNpcType( sNPC_TYPE_BASE* pCurType )
{
	m_pCurType = (sNPC_TYPE_PORTAL*)pCurType;
	__Reset();
}

void _Dlg_Type_Portal::__Reset()
{
	m_pCurReq = NULL;

	m_list_Portal.DeleteAllItems();
	m_list_Req.DeleteAllItems();
	m_Radio_Train.SetCheck(BST_UNCHECKED);	
	m_Radio_Normal.SetCheck(BST_UNCHECKED);
	m_Radio_None.SetCheck(BST_UNCHECKED);

	switch(m_pCurType->s_nPortalType)
	{
	case 0:
		m_Radio_Train.SetCheck(BST_CHECKED);
		break;
	case 1:
		m_Radio_Normal.SetCheck(BST_CHECKED);
		break;
	case 2:
		m_Radio_None.SetCheck(BST_CHECKED);
		break;
	}

	for( int i=0; i<m_pCurType->s_nPortalCount; ++i )
	{
		__InsertPortal( m_pCurType->GetPortal( i ) );
	}
	m_edit_EventID.SetWindowText( L"" );
}

void _Dlg_Type_Portal::__InsertPortal( sNPC_TYPE_PORTAL::sPORTAL* pPortal )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_Portal.GetItemCount();

	_stprintf_s( szMsg, 256, L"%d", nListIndex );
	m_list_Portal.InsertItem( nListIndex, szMsg );

	int nReqCount = 0;
	for( int i=0; i<NPC_TYPE_PORTAL_MAX_REQ_COUNT; ++i )
	{
		if( pPortal->s_ReqArray[ i ].s_eEnableType != sNPC_TYPE_PORTAL::None )
			++nReqCount;
	}
	_stprintf_s( szMsg, 256, L"%d 개", nReqCount );
	m_list_Portal.SetItem( nListIndex, 1, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	_stprintf_s( szMsg, 256, L"%d", pPortal->s_dwEventID );
	m_list_Portal.SetItem( nListIndex, 2, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	m_list_Portal.SetItemData( nListIndex, (DWORD_PTR)pPortal );
}

void _Dlg_Type_Portal::__InsertReq( sNPC_TYPE_PORTAL::sPORTAL_REQ* pReq )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_Req.GetItemCount();

	_stprintf_s( szMsg, 256, L"%d", nListIndex );
	m_list_Req.InsertItem( nListIndex, szMsg );

	switch( pReq->s_eEnableType )
	{
	case sNPC_TYPE_PORTAL::None:
		m_list_Req.SetItem( nListIndex, 1, LVIF_TEXT, L"   x   ", 0, 0, 0, 0 );
		pReq->s_nEnableID = 0;
		pReq->s_nEnableCount = 0;
		break;
	case sNPC_TYPE_PORTAL::Item:
		m_list_Req.SetItem( nListIndex, 1, LVIF_TEXT, L"아이템감소", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::TMaxLevel:
		m_list_Req.SetItem( nListIndex, 1, LVIF_TEXT, L"최대 테이머 레벨", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::Quest:
		m_list_Req.SetItem( nListIndex, 1, LVIF_TEXT, L"퀘스트", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::Money:
		m_list_Req.SetItem( nListIndex, 1, LVIF_TEXT, L"돈", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::TMinLevel:
		m_list_Req.SetItem( nListIndex, 1, LVIF_TEXT, L"최소 테이머 레벨", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::DMinLevel:
		m_list_Req.SetItem( nListIndex, 1, LVIF_TEXT, L"최소 디지몬 레벨", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::DMaxLevel:
		m_list_Req.SetItem( nListIndex, 1, LVIF_TEXT, L"최대 디지몬 레벨", 0, 0, 0, 0 );
		break;
	default:
		assert_cs( false );
	}

	_stprintf_s( szMsg, 256, L"%d", pReq->s_nEnableID );
	m_list_Req.SetItem( nListIndex, 2, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	_stprintf_s( szMsg, 256, L"%d", pReq->s_nEnableCount );
	m_list_Req.SetItem( nListIndex, 3, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	m_list_Req.SetItemData( nListIndex, (DWORD_PTR)pReq );
}

void _Dlg_Type_Portal::OnNMClickListPortal(NMHDR *pNMHDR, LRESULT *pResult)
{	
	m_list_Req.DeleteAllItems();
	m_pCurReq = NULL;

	POSITION pos = m_list_Portal.GetFirstSelectedItemPosition();
	if( pos == NULL )
	{		
		m_edit_EventID.SetWindowText( L"" );
		return;
	}

	int nIndex = m_list_Portal.GetNextSelectedItem( pos );
	for( int i=0; i<NPC_TYPE_PORTAL_MAX_REQ_COUNT; ++i )
	{
		__InsertReq( &m_pCurType->GetPortal( nIndex )->s_ReqArray[ i ] );
	}
	
	nsCsMfcFunc::Int2Control( &m_edit_EventID, m_pCurType->GetPortal( nIndex )->s_dwEventID );

	*pResult = 0;
}

void _Dlg_Type_Portal::OnBnClickedButtonAddportal()
{
	m_pCurType->NewPortal( true );

	__Reset();
}

void _Dlg_Type_Portal::OnBnClickedButtonDelportal()
{
	POSITION pos = m_list_Portal.GetFirstSelectedItemPosition();
	if( pos == NULL )
		return;

	int nIndex = m_list_Portal.GetNextSelectedItem( pos );
	m_pCurType->DelPortal( nIndex );

	__Reset();
}

void _Dlg_Type_Portal::__ResetReq()
{
	m_combo_Disapprove.SetCurSel( m_pCurReq->s_eEnableType );
	nsCsMfcFunc::Int2Control( &m_edit_EnableID, m_pCurReq->s_nEnableID );
	nsCsMfcFunc::Int2Control( &m_edit_EnableCount, m_pCurReq->s_nEnableCount );
}

void _Dlg_Type_Portal::__ResetReqItemSet()
{
	TCHAR szMsg[ 256 ];

	switch( m_pCurReq->s_eEnableType )
	{
	case sNPC_TYPE_PORTAL::None:
		m_list_Req.SetItem( m_nCurReqIndex, 1, LVIF_TEXT, L"   x   ", 0, 0, 0, 0 );
		m_pCurReq->s_nEnableID = 0;
		m_pCurReq->s_nEnableCount = 0;
		break;
	case sNPC_TYPE_PORTAL::Item:
		m_list_Req.SetItem( m_nCurReqIndex, 1, LVIF_TEXT, L"아이템감소", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::TMaxLevel:
		m_list_Req.SetItem( m_nCurReqIndex, 1, LVIF_TEXT, L"최대 테이머 레벨", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::Quest:
		m_list_Req.SetItem( m_nCurReqIndex, 1, LVIF_TEXT, L"퀘스트", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::Money:
		m_list_Req.SetItem( m_nCurReqIndex, 1, LVIF_TEXT, L"돈", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::TMinLevel:
		m_list_Req.SetItem( m_nCurReqIndex, 1, LVIF_TEXT, L"최소 테이머 레벨", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::DMinLevel:
		m_list_Req.SetItem( m_nCurReqIndex, 1, LVIF_TEXT, L"최소 디지몬 레벨", 0, 0, 0, 0 );
		break;
	case sNPC_TYPE_PORTAL::DMaxLevel:
		m_list_Req.SetItem( m_nCurReqIndex, 1, LVIF_TEXT, L"최대 디지몬 레벨", 0, 0, 0, 0 );
		break;
	default:
		assert_cs( false );
	}

	_stprintf_s( szMsg, 256, L"%d", m_pCurReq->s_nEnableID );
	m_list_Req.SetItem( m_nCurReqIndex, 2, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	_stprintf_s( szMsg, 256, L"%d", m_pCurReq->s_nEnableCount );
	m_list_Req.SetItem( m_nCurReqIndex, 3, LVIF_TEXT, szMsg, 0, 0, 0, 0 );	
}

void _Dlg_Type_Portal::OnNMClickListReq(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_pCurReq = NULL;

	POSITION pos = m_list_Req.GetFirstSelectedItemPosition();
	if( pos == NULL )
		return;

	m_nCurReqIndex = m_list_Req.GetNextSelectedItem( pos );
	m_pCurReq = (sNPC_TYPE_PORTAL::sPORTAL_REQ*)m_list_Req.GetItemData( m_nCurReqIndex );
	
	__ResetReq();

	*pResult = 0;
}

void _Dlg_Type_Portal::OnCbnSelchangeComboDisapprove()
{
	if( m_pCurReq == NULL )
		return;

	m_pCurReq->s_eEnableType = (sNPC_TYPE_PORTAL::eENABLE_TYPE)m_combo_Disapprove.GetCurSel();
	OnEnChangeEditEnableid();
	//__ResetReqItemSet();
}

void _Dlg_Type_Portal::OnBnClickedButtonTable()
{
	if( m_pCurReq == NULL )
		return;

	_DlgModal_List::eLIST_TYPE lt;
	switch( m_combo_Disapprove.GetCurSel() )
	{
	case sNPC_TYPE_PORTAL::None:
		return;
	case sNPC_TYPE_PORTAL::Quest:
		lt = _DlgModal_List::LT_QUEST;
		break;
	case sNPC_TYPE_PORTAL::Item:
		lt = _DlgModal_List::LT_ITEM;
		break;
	case sNPC_TYPE_PORTAL::TMaxLevel:
	case sNPC_TYPE_PORTAL::TMinLevel:
	case sNPC_TYPE_PORTAL::DMinLevel:
	case sNPC_TYPE_PORTAL::DMaxLevel:
		return;
	case sNPC_TYPE_PORTAL::Money:
		return;
	default:
		assert_cs( false );
	}

	DWORD dwReturn;
	_DlgModal_List dlgList( lt, &dwReturn );
	if( dlgList.DoModal() == IDOK )
	{
		nsCsMfcFunc::Int2Control( &m_edit_EnableID, dwReturn );
	}
}

void _Dlg_Type_Portal::OnEnChangeEditEnableid()
{
	if( m_pCurReq == NULL )
		return;

	DWORD dwID = _GetID();

	switch( m_combo_Disapprove.GetCurSel() )
	{
	case sNPC_TYPE_PORTAL::None:
		m_edit_Discript.SetWindowText( L"None" );
		m_pCurReq->s_nEnableID = 0;
		break;
	case sNPC_TYPE_PORTAL::Quest:
		{
			if( nsCsFileTable::g_pQuestMng->IsQuest( dwID ) == true )
			{
				CsQuest* pFTQuest = nsCsFileTable::g_pQuestMng->GetQuest( dwID );
				m_edit_Discript.SetWindowText( pFTQuest->m_szTitleText );
			}
			else
			{
				m_edit_Discript.SetWindowText( L"Empty ID !!" );
			}
		}
		break;
	case sNPC_TYPE_PORTAL::Item:
		{
			if( nsCsFileTable::g_pItemMng->IsItem( dwID ) == true )
			{
				CsItem::sINFO* pInfo = nsCsFileTable::g_pItemMng->GetItem( dwID )->GetInfo();
				m_edit_Discript.SetWindowText( pInfo->s_szName );
			}
			else
			{
				m_edit_Discript.SetWindowText( L"Empty ID !!" );
			}
		}
		break;	
	case sNPC_TYPE_PORTAL::TMaxLevel:
	case sNPC_TYPE_PORTAL::TMinLevel:
	case sNPC_TYPE_PORTAL::DMaxLevel:
	case sNPC_TYPE_PORTAL::DMinLevel:
	case sNPC_TYPE_PORTAL::Money:
		{
			m_edit_Discript.SetWindowText( L"" );
		}
		break;
	default:
		assert_cs( false );
	}

	m_pCurReq->s_nEnableID = dwID;
	__ResetReqItemSet();
}

void _Dlg_Type_Portal::OnEnChangeEditEnablecount()
{
	if( m_pCurReq == NULL )
		return;

	switch( m_combo_Disapprove.GetCurSel() )
	{
	case sNPC_TYPE_PORTAL::None:
		m_pCurReq->s_nEnableCount = 0;		
		return;
	case sNPC_TYPE_PORTAL::Quest:
		m_pCurReq->s_nEnableCount = 0;
		return;
	case sNPC_TYPE_PORTAL::Item:
		m_pCurReq->s_nEnableCount = _GetCount();
		break;
	case sNPC_TYPE_PORTAL::TMaxLevel:
	case sNPC_TYPE_PORTAL::TMinLevel:
	case sNPC_TYPE_PORTAL::DMaxLevel:
	case sNPC_TYPE_PORTAL::DMinLevel:
		m_pCurReq->s_nEnableCount = _GetCount();
		break;
	case sNPC_TYPE_PORTAL::Money:
		m_pCurReq->s_nEnableCount = _GetCount();
		break;
	default:
		assert_cs( false );
	}

	__ResetReqItemSet();
}


void _Dlg_Type_Portal::OnEnChangeEditEventid()
{
	if( m_pCurType == NULL )
		return;

	POSITION pos = m_list_Portal.GetFirstSelectedItemPosition();
	if( pos == NULL )
		return;

	int nIndex = m_list_Portal.GetNextSelectedItem( pos );
	m_pCurType->GetPortal( nIndex )->s_dwEventID = nsCsMfcFunc::Control2Int( &m_edit_EventID );
}

void _Dlg_Type_Portal::OnBnClickedRadio1()
{
	m_Radio_Train.SetCheck(BST_CHECKED);
	m_Radio_Normal.SetCheck(BST_UNCHECKED);
	m_Radio_None.SetCheck(BST_UNCHECKED);

	m_pCurType->s_nPortalType = 0;
}

void _Dlg_Type_Portal::OnBnClickedRadio2()
{
	m_Radio_Train.SetCheck(BST_UNCHECKED);
	m_Radio_Normal.SetCheck(BST_CHECKED);
	m_Radio_None.SetCheck(BST_UNCHECKED);

	m_pCurType->s_nPortalType = 1;
}

void _Dlg_Type_Portal::OnBnClickedRadio3()
{
	m_Radio_Train.SetCheck(BST_UNCHECKED);
	m_Radio_Normal.SetCheck(BST_UNCHECKED);
	m_Radio_None.SetCheck(BST_CHECKED);

	m_pCurType->s_nPortalType = 2;
}

