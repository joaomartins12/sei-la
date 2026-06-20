// _DlgResourceView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ResourceIntegrityTool.h"
#include "_DlgResourceView.h"


// _DlgResourceView 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgResourceView, CDialog)

_DlgResourceView::_DlgResourceView(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgResourceView::IDD, pParent)
{

}

_DlgResourceView::~_DlgResourceView()
{
}

void _DlgResourceView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_list);
	DDX_Control(pDX, IDC_LIST_DEL, m_listDel);	
	DDX_Control(pDX, IDC_STATIC_TOTAL_COUNT, m_OrgCount);	
	DDX_Control(pDX, IDC_STATIC_CURRENT_COUNT, m_DelCount);		
}


BEGIN_MESSAGE_MAP(_DlgResourceView, CDialog)
	ON_BN_CLICKED(IDCANCEL, &_DlgResourceView::OnBnClickedCancel)
	ON_NOTIFY( LVN_KEYDOWN, IDC_LIST2, &_DlgResourceView::OnListKeyDown )
	ON_NOTIFY( LVN_KEYDOWN, IDC_LIST_DEL, &_DlgResourceView::OnDelListKeyDown )
	ON_BN_CLICKED(IDC_ORG_BTN, &_DlgResourceView::OnBnClickedOrgBtn)
	ON_BN_CLICKED(ID_HASH_UPDATE, &_DlgResourceView::OnBnClickedHashUpdate)
END_MESSAGE_MAP()

BOOL _DlgResourceView::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwExStyle = m_list.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ;
	m_list.SetExtendedStyle( dwExStyle );

	dwExStyle = m_listDel.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES ;
	m_listDel.SetExtendedStyle( dwExStyle );

	m_list.InsertColumn( 0, L"File Name", LVCFMT_LEFT, 350 );
	m_list.InsertColumn( 1, L"Sha256", LVCFMT_LEFT, 500 );
	m_listDel.InsertColumn( 0, L"File Name", LVCFMT_LEFT, 350 );
	m_listDel.InsertColumn( 1, L"Sha256", LVCFMT_LEFT, 500 );

	OnBnClickedOrgBtn();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


// _DlgResourceView 메시지 처리기입니다.

void _DlgResourceView::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnCancel();
}

void _DlgResourceView::SetHashData( MAP_HASH_CONTANER const & RecvHashData )
{
	recvHash = RecvHashData;
}

void _DlgResourceView::OnListKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

	if( pTVKeyDown->wVKey == VK_DELETE )
		OnSelectedItemDelete();

	*pResult = 0;
}

void _DlgResourceView::OnDelListKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;

	if( pTVKeyDown->wVKey == VK_DELETE )
 		OnDelListSelectedItemDelete();

	*pResult = 0;
}

void _DlgResourceView::OnSelectedItemDelete()
{
	POSITION pos = m_list.GetFirstSelectedItemPosition();
	while( NULL != pos )
	{
		int nSelectItem = m_list.GetNextSelectedItem( pos );
		CString sKey = m_list.GetItemText( nSelectItem, 0 );
		CString sValue = m_list.GetItemText( nSelectItem, 1 );
		int nTotalCount = m_listDel.GetItemCount();
		m_listDel.InsertItem( nTotalCount, sKey );
		m_listDel.SetItem( nTotalCount, 1, LVIF_TEXT, sValue,0,0,0,0 );

		m_list.DeleteItem( nSelectItem );
		pos = m_list.GetFirstSelectedItemPosition();
	}
	_UpdateItemCount();
}

void _DlgResourceView::OnDelListSelectedItemDelete()
{
	POSITION pos = m_listDel.GetFirstSelectedItemPosition();
	while( NULL != pos )
	{
		int nSelectItem = m_listDel.GetNextSelectedItem( pos );
		CString sKey = m_listDel.GetItemText( nSelectItem, 0 );
		CString sValue = m_listDel.GetItemText( nSelectItem, 1 );
		int nTotalCount = m_list.GetItemCount();
		m_list.InsertItem( nTotalCount, sKey );
		m_list.SetItem( nTotalCount, 1, LVIF_TEXT, sValue,0,0,0,0 );

		m_listDel.DeleteItem( nSelectItem );
		pos = m_listDel.GetFirstSelectedItemPosition();
	}

	_UpdateItemCount();
}


void _DlgResourceView::OnBnClickedOrgBtn()
{
	m_list.DeleteAllItems();
	m_listDel.DeleteAllItems();

	MAP_HASH_CONTANER_CIT it = recvHash.begin();
	for( int n = 0; it != recvHash.end(); ++it, ++n )
	{
		std::wstring wKey;
		DmCS::StringFn::From( wKey, it->first );
		m_list.InsertItem(n, wKey.c_str());
		std::wstring wName;
		DmCS::StringFn::From( wName, it->second );
		m_list.SetItem( n, 1, LVIF_TEXT, wName.c_str(),0,0,0,0 );
	}	

	_UpdateItemCount();
}

void _DlgResourceView::_UpdateItemCount()
{
	int nOrgCount = m_list.GetItemCount();
	std::wstring wsMsg;
	DmCS::StringFn::Format( wsMsg, L"Org Count : %d", nOrgCount );
	m_OrgCount.SetWindowText( wsMsg.c_str() );

	int nCurrentCount = m_listDel.GetItemCount();
	std::wstring wsMsg2;
	DmCS::StringFn::Format( wsMsg2, L"Delect Count : %d", nCurrentCount );
	m_DelCount.SetWindowText( wsMsg2.c_str() );
}

void _DlgResourceView::OnBnClickedHashUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	MAP_HASH_CONTANER changeHash;
	int nOrgCount = m_list.GetItemCount();
	for( int n = 0; n < nOrgCount; ++n )
	{
		std::wstring wsKey = m_list.GetItemText( n, 0 );
		std::wstring wsValue = m_list.GetItemText( n, 1 );
		std::string sKey;
		DmCS::StringFn::ToMB( wsKey, sKey );
		
		std::string sName;
		DmCS::StringFn::ToMB( wsValue, sName );

		changeHash.insert( std::make_pair( sKey, sName ) );
	}

	if( changeHash.empty() )
	{
		CsMessageBoxA( MB_OK, "변경할 데이터가 없다." );
		return;
	}

	CWnd* pWin = GetParent();
	if( NULL == pWin )
	{
		CsMessageBoxA( MB_OK, "부모 윈도우가 없다." );
		return;
	}

	CIntegrityTool* pParentWin = dynamic_cast<CIntegrityTool*>(pWin);
	if( NULL == pParentWin )
	{
		CsMessageBoxA( MB_OK, "부모 윈도우가 없다." );
		return;
	}

	pParentWin->ChangeHashData( changeHash );
}
