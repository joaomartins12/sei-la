//// Dlg_Type_DigiCore.cpp : 구현 파일입니다.
////
//
#include "stdafx.h"
//#include "NpcTool.h"
//#include "_Dlg_Type_DigiCore.h"
//
//
//// CDlg_Type_DigiCore 대화 상자입니다.
//
//IMPLEMENT_DYNAMIC(CDlg_Type_DigiCore, CDialog)
//
//CDlg_Type_DigiCore::CDlg_Type_DigiCore(CWnd* pParent /*=NULL*/)
//	: CDialog(CDlg_Type_DigiCore::IDD, pParent)
//{
//
//}
//
//CDlg_Type_DigiCore::~CDlg_Type_DigiCore()
//{
//}
//
//void CDlg_Type_DigiCore::DoDataExchange(CDataExchange* pDX)
//{
//	CDialog::DoDataExchange(pDX);
//	DDX_Control(pDX, IDC_LIST_DIGICORETABLE, m_List_DigiCore);
//	DDX_Control(pDX, IDC_LIST_MAINLIST, m_List_Trade);
//}
//
//
//BEGIN_MESSAGE_MAP(CDlg_Type_DigiCore, CDialog)
//	ON_BN_CLICKED(IDC_BUTTON_ADDDIGICORE, &CDlg_Type_DigiCore::OnBnClickedButtonAdddigicore)
//	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_DIGICORETABLE, &CDlg_Type_DigiCore::OnLvnColumnclickListDigicoretable)
//	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_MAINLIST, &CDlg_Type_DigiCore::OnLvnColumnclickListMainlist)
//	ON_BN_CLICKED(IDC_BUTTON_DELDIGICORE, &CDlg_Type_DigiCore::OnBnClickedButtonDeldigicore)
//END_MESSAGE_MAP()
//
//
////=======================================================================================================
////
////		Base
////
////=======================================================================================================
//
//BOOL CDlg_Type_DigiCore::OnInitDialog()
//{
//	CDialog::OnInitDialog();
//
//	__Init();
//
//	return TRUE;  // return TRUE unless you set the focus to a control
//}
//
//void CDlg_Type_DigiCore::__Init()
//{
//	__InitItem();
//}
//
//void CDlg_Type_DigiCore::SetNpcType( sNPC_TYPE_BASE* pCurType )
//{
//	m_pCurDigiCore = (sNPC_TYPE_DIGICORE*)pCurType;
//	__ResetTrade();
//}
//
//void CDlg_Type_DigiCore::__InsertList( CListCtrl* pListCtrl, CsItem* pItem )
//{
//	TCHAR szMsg[ 256 ];
//	int nListIndex = pListCtrl->GetItemCount();
//
//	CsItem::sINFO* pInfo = pItem->GetInfo();	
//
//	_stprintf_s( szMsg, 256, L"%d", pInfo->s_dwItemID );
//	pListCtrl->InsertItem( nListIndex, szMsg );
//
//	pListCtrl->SetItem( nListIndex, 1, LVIF_TEXT, pInfo->s_szName, 0, 0, 0, 0 );
//	pListCtrl->SetItem( nListIndex, 2, LVIF_TEXT, nsCSFILE::GetNumberStr( szMsg, 256, pInfo->s_dwDigiCorePrice ), 0, 0, 0, 0 );
//	pListCtrl->SetItem( nListIndex, 3, LVIF_TEXT, nsCSFILE::GetNumberStr( szMsg, 256, pInfo->s_dwSale ), 0, 0, 0, 0 );
//	pListCtrl->SetItemData( nListIndex, pInfo->s_dwItemID );
//}
//
////=======================================================================================================
////
////		Item
////
////=======================================================================================================
//
//void CDlg_Type_DigiCore::__InitItem()
//{
//	DWORD dwExStyle = m_List_DigiCore.GetExtendedStyle();
//	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
//	m_List_DigiCore.SetExtendedStyle( dwExStyle );
//	m_List_DigiCore.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
//	m_List_DigiCore.InsertColumn( 1, L"이름", LVCFMT_LEFT, 100 );
//	m_List_DigiCore.InsertColumn( 2, L"필요디지코어", LVCFMT_RIGHT, 85 );
//	m_List_DigiCore.InsertColumn( 3, L"판매가격", LVCFMT_RIGHT, 85 );
//
//	m_List_Trade.SetExtendedStyle( dwExStyle );
//	m_List_Trade.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
//	m_List_Trade.InsertColumn( 1, L"이름", LVCFMT_LEFT, 100 );
//	m_List_Trade.InsertColumn( 2, L"필요디지코어", LVCFMT_RIGHT, 85 );
//	m_List_Trade.InsertColumn( 3, L"판매가격", LVCFMT_RIGHT, 85 );
//
//	__SortList( 0, true );	
//}
//
//void CDlg_Type_DigiCore::__SortList( int nColume, bool bIncrease )
//{
//	m_List_DigiCore.DeleteAllItems();
//
//	CsItem::MAP* pMap = nsCsFileTable::g_pItemMng->GetItemMap();
//	CsItem::MAP_IT it = pMap->begin();
//	CsItem::MAP_IT itEnd = pMap->end();
//
//	std::list< CsItem* > listSort;
//	for( ; it!=itEnd; ++it )
//	{
//		listSort.push_back( it->second );
//	}
//
//	while( listSort.size() != 0 )
//	{
//		std::list< CsItem* >::iterator itInput = listSort.begin();
//		std::list< CsItem* >::iterator it = itInput;
//		for( ; it!=listSort.end(); ++it )
//		{
//			switch( nColume )
//			{
//			case 0:
//				if( ( (*it)->GetInfo()->s_dwItemID < (*itInput)->GetInfo()->s_dwItemID ) == bIncrease )
//					itInput = it;
//				break;
//			case 1:
//				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
//					itInput = it;
//				break;
//			case 2:
//				if( ( (*it)->GetInfo()->s_dwDigiCorePrice < (*itInput)->GetInfo()->s_dwDigiCorePrice ) == bIncrease )
//					itInput = it;
//				break;
//			case 3:
//				if( ( (*it)->GetInfo()->s_dwSale < (*itInput)->GetInfo()->s_dwSale ) == bIncrease )
//					itInput = it;
//				break;
//			default:
//				assert_cs( false );
//			}
//		}
//
//		if( (*itInput)->GetInfo()->s_dwDigiCorePrice > 0 )
//			__InsertList( &m_List_DigiCore, (*itInput ) );
//		listSort.erase( itInput );
//	}
//}
//
////=======================================================================================================
////
////		Trade
////
////=======================================================================================================
//
//void CDlg_Type_DigiCore::__ResetTrade()
//{
//	m_List_Trade.DeleteAllItems();
//
//	int nCount = m_pCurDigiCore->GetItemCount();
//	std::list< int > Invalidelist;
//	for( int c=0; c<nCount; ++c )
//	{
//		CsItem* pItem = m_pCurDigiCore->GetItem( c );
//		if( pItem == NULL )
//		{
//			CsMessageBox( MB_OK, L"Item.xls 에 ID - %d 아이템이 존재 하지 않습니다.", m_pCurDigiCore->GetItemID( c ) );
//			Invalidelist.push_back( c );
//			continue;
//		}
//		__InsertList( &m_List_Trade, pItem );		
//	}
//
//	// 존재 하지 않는 아이템은 지워주자
//	std::list< int >::iterator it = Invalidelist.begin();
//	std::list< int >::iterator itEnd = Invalidelist.end();
//	for( ; it!=itEnd; ++it )
//	{		
//		m_pCurDigiCore->DelItem( m_pCurDigiCore->GetItem( *it )->GetInfo()->s_dwItemID );
//	}
//}
//
//void CDlg_Type_DigiCore::__ApplyNpc( CsItem* pItem )
//{
//	m_pCurDigiCore->AddItem( pItem->GetInfo()->s_dwItemID );
//}
//
//bool CDlg_Type_DigiCore::__IsExistTrade( DWORD dwSrcItemID )
//{
//	int nCount = m_List_Trade.GetItemCount();
//	for( int c=0; c<nCount; ++c )
//	{
//		DWORD dwItemID = (DWORD)m_List_Trade.GetItemData( c );
//		if( dwItemID == dwSrcItemID )
//			return true;
//	}
//	return false;
//}
//
//void CDlg_Type_DigiCore::__SortTrade( int nColume, bool bIncrease )
//{
//	std::list< CsItem* > listSort;
//
//	int nCount = m_List_Trade.GetItemCount();
//	for( int c=0; c<nCount; ++c )
//	{
//		DWORD dwItemID = (DWORD)m_List_Trade.GetItemData( c );
//		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemID );
//		assert_cs( pItem != NULL );
//
//		listSort.push_back( pItem );
//	}
//
//	m_List_Trade.DeleteAllItems();
//
//	while( listSort.size() != 0 )
//	{
//		std::list< CsItem* >::iterator itInput = listSort.begin();
//		std::list< CsItem* >::iterator it = itInput;
//		for( ; it!=listSort.end(); ++it )
//		{
//			switch( nColume )
//			{
//			case 0:
//				if( ( (*it)->GetInfo()->s_dwItemID < (*itInput)->GetInfo()->s_dwItemID ) == bIncrease )
//					itInput = it;
//				break;
//			case 1:
//				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
//					itInput = it;
//				break;
//			case 2:
//				if( ( (*it)->GetInfo()->s_dwDigiCorePrice < (*itInput)->GetInfo()->s_dwDigiCorePrice ) == bIncrease )
//					itInput = it;
//				break;
//			case 3:
//				if( ( (*it)->GetInfo()->s_dwSale < (*itInput)->GetInfo()->s_dwSale ) == bIncrease )
//					itInput = it;
//				break;
//			default:
//				assert_cs( false );
//			}
//		}
//
//		__InsertList( &m_List_Trade, (*itInput) );
//		listSort.erase( itInput );
//	}
//}
//
//
//// CDlg_Type_DigiCore 메시지 처리기입니다.
//
//void CDlg_Type_DigiCore::OnBnClickedButtonAdddigicore()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//
//	POSITION pos = m_List_DigiCore.GetFirstSelectedItemPosition();
//	while( pos )
//	{
//		int nIndex = m_List_DigiCore.GetNextSelectedItem( pos );
//		DWORD dwItemID = (DWORD)m_List_DigiCore.GetItemData( nIndex );
//		
//		if( __IsExistTrade( dwItemID ) == true )
//			continue;
//
//		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( dwItemID );
//		assert_cs( pItem != NULL );
//		__InsertList( &m_List_Trade, pItem );
//		__ApplyNpc( pItem );		
//	}
//}
//
//
//void CDlg_Type_DigiCore::OnLvnColumnclickListDigicoretable(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//
//	static bool bIncrease = true;
//	bIncrease = !bIncrease;
//
//	__SortList( pNMLV->iSubItem, bIncrease );
//
//	*pResult = 0;
//}
//
//void CDlg_Type_DigiCore::OnLvnColumnclickListMainlist(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//
//	static bool bIncrease = true;
//	bIncrease = !bIncrease;
//
//	__SortTrade( pNMLV->iSubItem, bIncrease );
//
//	*pResult = 0;
//}
//
//void CDlg_Type_DigiCore::OnBnClickedButtonDeldigicore()
//{
//	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
//
//	int nInsertIndex = m_List_Trade.GetItemCount();
//
//	std::stack< int > stackDel;
//	POSITION pos = m_List_Trade.GetFirstSelectedItemPosition();
//	while( pos )
//	{
//		stackDel.push( m_List_Trade.GetNextSelectedItem( pos ) );
//	}
//
//	while( stackDel.size() != 0 )
//	{
//		int nDelIndex = stackDel.top();
//		stackDel.pop();
//		m_pCurDigiCore->DelItem( (DWORD)m_List_Trade.GetItemData( nDelIndex ) );
//		m_List_Trade.DeleteItem( nDelIndex );		
//	}
//}
