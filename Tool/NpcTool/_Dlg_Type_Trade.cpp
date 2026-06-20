// _Dlg_Type_Trade.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NpcTool.h"
#include "_Dlg_Type_Trade.h"


// _Dlg_Type_Trade 대화 상자입니다.

IMPLEMENT_DYNAMIC(_Dlg_Type_Trade, CDialog)

_Dlg_Type_Trade::_Dlg_Type_Trade(CWnd* pParent /*=NULL*/)
	: CDialog(_Dlg_Type_Trade::IDD, pParent)
{

}

_Dlg_Type_Trade::~_Dlg_Type_Trade()
{
}

void _Dlg_Type_Trade::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ITEMTABLE, m_list_ItemTable);
	DDX_Control(pDX, IDC_LIST_ITEMTRADE, m_list_Trade);
}


BEGIN_MESSAGE_MAP(_Dlg_Type_Trade, CDialog)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_ITEMTABLE, &_Dlg_Type_Trade::OnLvnColumnclickListItemtable)
	ON_BN_CLICKED(IDC_BUTTON_ADDITEM, &_Dlg_Type_Trade::OnBnClickedButtonAdditem)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_ITEMTRADE, &_Dlg_Type_Trade::OnLvnColumnclickListItemtrade)
	ON_BN_CLICKED(IDC_BUTTON_DELITEM, &_Dlg_Type_Trade::OnBnClickedButtonDelitem)
END_MESSAGE_MAP()


//=======================================================================================================
//
//		Base
//
//=======================================================================================================

BOOL _Dlg_Type_Trade::OnInitDialog()
{
	CDialog::OnInitDialog();

	__Init();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _Dlg_Type_Trade::__Init()
{
	__InitItem();
}

void _Dlg_Type_Trade::_SetNpcType( sNPC_TYPE_BASE* pCurType )
{
	m_pCurTrade = (sNPC_TYPE_TRADE*)pCurType;
	__ResetTrade();
}

void _Dlg_Type_Trade::__InsertList( CListCtrl* pListCtrl, CsItem* pItem )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = pListCtrl->GetItemCount();

	CsItem::sINFO* pInfo = pItem->GetInfo(); 

	_stprintf_s( szMsg, 256, L"%d", pInfo->s_dwItemID );
	pListCtrl->InsertItem( nListIndex, szMsg );

	pListCtrl->SetItem( nListIndex, 1, LVIF_TEXT, pInfo->s_szName, 0, 0, 0, 0 );
	pListCtrl->SetItem( nListIndex, 2, LVIF_TEXT, nsCSFILE::GetNumberStr( szMsg, 256, pInfo->s_dwScanPrice ), 0, 0, 0, 0 );
	pListCtrl->SetItem( nListIndex, 3, LVIF_TEXT, nsCSFILE::GetNumberStr( szMsg, 256, pInfo->s_dwSale ), 0, 0, 0, 0 );
	pListCtrl->SetItem( nListIndex, 4, LVIF_TEXT, nsCSFILE::GetNumberStr( szMsg, 256, pInfo->s_dwDigiCorePrice ), 0, 0, 0, 0 );
	pListCtrl->SetItem( nListIndex, 5, LVIF_TEXT, nsCSFILE::GetNumberStr( szMsg, 256, pInfo->s_nEventItemType ), 0, 0, 0, 0 );
	pListCtrl->SetItem( nListIndex, 6, LVIF_TEXT, nsCSFILE::GetNumberStr( szMsg, 256, pInfo->s_dwEventItemPrice ), 0, 0, 0, 0 );
	pListCtrl->SetItemData( nListIndex, pInfo->s_dwItemID );
}

//=======================================================================================================
//
//		Item
//
//=======================================================================================================

void _Dlg_Type_Trade::__InitItem()
{
	DWORD dwExStyle = m_list_ItemTable.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_ItemTable.SetExtendedStyle( dwExStyle );
	m_list_Trade.SetExtendedStyle( dwExStyle );

	m_list_ItemTable.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
	m_list_ItemTable.InsertColumn( 1, L"이름", LVCFMT_LEFT, 100 );
	m_list_ItemTable.InsertColumn( 2, L"구입가격", LVCFMT_RIGHT, 85 );
	m_list_ItemTable.InsertColumn( 3, L"판매가격", LVCFMT_RIGHT, 85 );
	m_list_ItemTable.InsertColumn( 4, L"디지코어", LVCFMT_RIGHT, 85 );
	m_list_ItemTable.InsertColumn( 5, L"이벤트아이템", LVCFMT_RIGHT, 105 );
	m_list_ItemTable.InsertColumn( 6, L"필요수량", LVCFMT_RIGHT, 85 );

	m_list_ItemTable.InsertColumn( 7, L"물물교환 필요수량A", LVCFMT_RIGHT, 85 );
	m_list_ItemTable.InsertColumn( 8, L"물물교환 필요수량B", LVCFMT_RIGHT, 85 );
	m_list_ItemTable.InsertColumn( 9, L"물물교환 필요수량C", LVCFMT_RIGHT, 85 );
	m_list_ItemTable.InsertColumn( 10, L"물물교환 필요수량D", LVCFMT_RIGHT, 85 );

	m_list_Trade.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
	m_list_Trade.InsertColumn( 1, L"이름", LVCFMT_LEFT, 100 );
	m_list_Trade.InsertColumn( 2, L"구입가격", LVCFMT_RIGHT, 85 );
	m_list_Trade.InsertColumn( 3, L"판매가격", LVCFMT_RIGHT, 85 );		
	m_list_Trade.InsertColumn( 4, L"디지코어", LVCFMT_RIGHT, 85 );
	m_list_Trade.InsertColumn( 5, L"이벤트아이템", LVCFMT_RIGHT, 105 );
	m_list_Trade.InsertColumn( 6, L"필요수량", LVCFMT_RIGHT, 85 );

	m_list_Trade.InsertColumn( 7, L"물물교환 필요수량A", LVCFMT_RIGHT, 85 );
	m_list_Trade.InsertColumn( 8, L"물물교환 필요수량B", LVCFMT_RIGHT, 85 );
	m_list_Trade.InsertColumn( 9, L"물물교환 필요수량C", LVCFMT_RIGHT, 85 );
	m_list_Trade.InsertColumn( 10, L"물물교환 필요수량D", LVCFMT_RIGHT, 85 );

	__SortList( 0, true );	
}

void _Dlg_Type_Trade::__SortList( int nColume, bool bIncrease )
{
	m_list_ItemTable.DeleteAllItems();

	CsItem::MAP* pMap = nsCsFileTable::g_pItemMng->GetItemMap();
	CsItem::MAP_IT it = pMap->begin();
	CsItem::MAP_IT itEnd = pMap->end();
	
	std::list< CsItem* > listSort;
	for( ; it!=itEnd; ++it )
	{
		listSort.push_back( it->second );
	}

	while( listSort.size() != 0 )
	{
		std::list< CsItem* >::iterator itInput = listSort.begin();
		std::list< CsItem* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( nColume )
			{
			case 0:
				if( ( (*it)->GetInfo()->s_dwItemID < (*itInput)->GetInfo()->s_dwItemID ) == bIncrease )
					itInput = it;
				break;
			case 1:
				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
					itInput = it;
				break;
			case 2:
				if( ( (*it)->GetInfo()->s_dwScanPrice < (*itInput)->GetInfo()->s_dwScanPrice ) == bIncrease )
					itInput = it;
				break;
			case 3:
				if( ( (*it)->GetInfo()->s_dwSale < (*itInput)->GetInfo()->s_dwSale ) == bIncrease )
					itInput = it;
				break;			
			case 4:
				if( ( (*it)->GetInfo()->s_dwDigiCorePrice < (*itInput)->GetInfo()->s_dwDigiCorePrice ) == bIncrease )
					itInput = it;
				break;		
			case 5:
				if( ( (*it)->GetInfo()->s_nEventItemType < (*itInput)->GetInfo()->s_nEventItemType ) == bIncrease )
					itInput = it;
				break;		
			case 6:
				if( ( (*it)->GetInfo()->s_dwEventItemPrice < (*itInput)->GetInfo()->s_dwEventItemPrice ) == bIncrease )
					itInput = it;
				break;
			default:
				assert_cs( false );
			}
		}

		if( (*itInput)->GetInfo()->s_dwScanPrice > 0 || 
			(*itInput)->GetInfo()->s_dwDigiCorePrice > 0 ||
			(*itInput)->GetInfo()->s_dwEventItemPrice > 0 )
			__InsertList( &m_list_ItemTable, (*itInput ) );
		listSort.erase( itInput );
	}
}

void _Dlg_Type_Trade::OnLvnColumnclickListItemtable(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	static bool bIncrease = true;
	bIncrease = !bIncrease;

	__SortList( pNMLV->iSubItem, bIncrease );	

	*pResult = 0;
}

// 추가 버튼 클릭
void _Dlg_Type_Trade::OnBnClickedButtonAdditem()
{
	POSITION pos = m_list_ItemTable.GetFirstSelectedItemPosition();
	while( pos )
	{
		int nIndex = m_list_ItemTable.GetNextSelectedItem( pos );
		DWORD dwItemID = (DWORD)m_list_ItemTable.GetItemData( nIndex );

		if( __IsExistTrade( dwItemID ) == true )
			continue;

		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemID );
		assert_cs( pItem != NULL );
		__InsertList( &m_list_Trade, pItem );
		__ApplyNpc( pItem );
	}
}


//=======================================================================================================
//
//		Trade
//
//=======================================================================================================

void _Dlg_Type_Trade::__ResetTrade()
{
	m_list_Trade.DeleteAllItems();

	int nCount = m_pCurTrade->GetItemCount();
	std::list< int > Invalidelist;
	for( int c=0; c<nCount; ++c )
	{
		CsItem* pItem = m_pCurTrade->GetItem( c );
		if( pItem == NULL )
		{
			CsMessageBox( MB_OK, L"Item.xls 에 ID - %d 아이템이 존재 하지 않습니다.", m_pCurTrade->GetItemID( c ) );
			Invalidelist.push_back( c );
			continue;
		}
		__InsertList( &m_list_Trade, pItem );		
	}

	// 존재 하지 않는 아이템은 지워주자
	std::list< int >::iterator it = Invalidelist.begin();
	std::list< int >::iterator itEnd = Invalidelist.end();
	for( ; it!=itEnd; ++it )
	{		
		m_pCurTrade->DelItem( m_pCurTrade->GetItem( *it )->GetInfo()->s_dwItemID );
	}
}

void _Dlg_Type_Trade::__ApplyNpc( CsItem* pItem )
{
	m_pCurTrade->AddItem( pItem->GetInfo()->s_dwItemID );
}

bool _Dlg_Type_Trade::__IsExistTrade( DWORD dwSrcItemID )
{
	int nCount = m_list_Trade.GetItemCount();
	for( int c=0; c<nCount; ++c )
	{
		DWORD dwItemID = (DWORD)m_list_Trade.GetItemData( c );
		if( dwItemID == dwSrcItemID )
			return true;
	}
	return false;
}

void _Dlg_Type_Trade::__SortTrade( int nColume, bool bIncrease )
{
	std::list< CsItem* > listSort;

	int nCount = m_list_Trade.GetItemCount();
	for( int c=0; c<nCount; ++c )
	{
		DWORD dwItemID = (DWORD)m_list_Trade.GetItemData( c );
		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemID );
		assert_cs( pItem != NULL );
		
		listSort.push_back( pItem );
	}

	m_list_Trade.DeleteAllItems();

	while( listSort.size() != 0 )
	{
		std::list< CsItem* >::iterator itInput = listSort.begin();
		std::list< CsItem* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( nColume )
			{
			case 0:
				if( ( (*it)->GetInfo()->s_dwItemID < (*itInput)->GetInfo()->s_dwItemID ) == bIncrease )
					itInput = it;
				break;
			case 1:
				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
					itInput = it;
				break;
			case 2:
				if( ( (*it)->GetInfo()->s_dwScanPrice < (*itInput)->GetInfo()->s_dwScanPrice ) == bIncrease )
					itInput = it;
				break;
			case 3:
				if( ( (*it)->GetInfo()->s_dwSale < (*itInput)->GetInfo()->s_dwSale ) == bIncrease )
					itInput = it;
				break;
			case 4:
				if( ( (*it)->GetInfo()->s_dwDigiCorePrice < (*itInput)->GetInfo()->s_dwDigiCorePrice ) == bIncrease )
					itInput = it;
				break;		
			case 5:
				if( ( (*it)->GetInfo()->s_nEventItemType < (*itInput)->GetInfo()->s_nEventItemType ) == bIncrease )
					itInput = it;
				break;
			case 6:
				if( ( (*it)->GetInfo()->s_dwEventItemPrice < (*itInput)->GetInfo()->s_dwEventItemPrice ) == bIncrease )
					itInput = it;
				break;
			default:
				assert_cs( false );
			}
		}

		__InsertList( &m_list_Trade, (*itInput) );
		listSort.erase( itInput );
	}
}

// 트레이드 리스트 컬럼 클릭
void _Dlg_Type_Trade::OnLvnColumnclickListItemtrade(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	static bool bIncrease = true;
	bIncrease = !bIncrease;

	__SortTrade( pNMLV->iSubItem, bIncrease );

	*pResult = 0;
}

// 제거 버튼 클릭
void _Dlg_Type_Trade::OnBnClickedButtonDelitem()
{
	int nInsertIndex = m_list_Trade.GetItemCount();

	std::stack< int > stackDel;
	POSITION pos = m_list_Trade.GetFirstSelectedItemPosition();
	while( pos )
	{
		stackDel.push( m_list_Trade.GetNextSelectedItem( pos ) );
	}

	while( stackDel.size() != 0 )
	{
		int nDelIndex = stackDel.top();
		stackDel.pop();
		m_pCurTrade->DelItem( (DWORD)m_list_Trade.GetItemData( nDelIndex ) );
		m_list_Trade.DeleteItem( nDelIndex );		
	}
}
