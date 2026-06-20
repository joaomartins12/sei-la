// _DlgViewSimple.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgViewSimple.h"


// _DlgViewSimple 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgViewSimple, CDialog)

_DlgViewSimple::_DlgViewSimple(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgViewSimple::IDD, pParent)
{

}

_DlgViewSimple::~_DlgViewSimple()
{
}

void _DlgViewSimple::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAIN, m_list_MainList);
}


BEGIN_MESSAGE_MAP(_DlgViewSimple, CDialog)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CLICK,			IDC_LIST_MAIN, &_DlgViewSimple::OnNMClickListMain)
	ON_NOTIFY(LVN_ITEMCHANGED,	IDC_LIST_MAIN, &_DlgViewSimple::OnLvnItemchangedListMain)
	ON_NOTIFY(LVN_COLUMNCLICK,	IDC_LIST_MAIN, &_DlgViewSimple::OnLvnColumnclickListMain)
	ON_NOTIFY(NM_DBLCLK,		IDC_LIST_MAIN, &_DlgViewSimple::OnNMDblclkListMain)
	ON_NOTIFY(NM_RCLICK,		IDC_LIST_MAIN, &_DlgViewSimple::OnNMRclickListMain)
	ON_NOTIFY(NM_CUSTOMDRAW,	IDC_LIST_MAIN, &_DlgViewSimple::OnNMCustomdraw)	
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &_DlgViewSimple::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// _DlgViewSimple 메시지 처리기입니다.

void _DlgViewSimple::_Init()
{
	DWORD dwExStyle = m_list_MainList.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_MainList.SetExtendedStyle( dwExStyle );

	m_list_MainList.InsertColumn( 0, L"ID", LVCFMT_LEFT, 50 );
	m_list_MainList.InsertColumn( 1, L"MngID", LVCFMT_LEFT, 60 );
	m_list_MainList.InsertColumn( 2, L"Title Tab", LVCFMT_LEFT, 150 );
	m_list_MainList.InsertColumn( 3, L"Title", LVCFMT_LEFT, 300 );
	m_list_MainList.InsertColumn( 4, L"Level", LVCFMT_LEFT, 70 );
	m_list_MainList.InsertColumn( 5, L"Req Money", LVCFMT_LEFT, 70 );
	m_list_MainList.InsertColumn( 6, L"Req Exp", LVCFMT_LEFT, 70 );
	m_list_MainList.InsertColumn( 7, L"Etc1", LVCFMT_LEFT, 120 );
	m_list_MainList.InsertColumn( 8, L"Etc2", LVCFMT_LEFT, 120 );
	m_list_MainList.InsertColumn( 9, L"Etc3", LVCFMT_LEFT, 120 );
	m_list_MainList.InsertColumn( 10, L"Etc4", LVCFMT_LEFT, 120 );
	m_list_MainList.InsertColumn( 11, L"Etc5", LVCFMT_LEFT, 120 );
	m_list_MainList.InsertColumn( 12, L"Active", LVCFMT_LEFT, 60 );
}

void _DlgViewSimple::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		__ReadQuest();
		m_list_MainList.SetFocus();
	}
}

void _DlgViewSimple::__ReadQuest()
{
	m_list_MainList.DeleteAllItems();

	CsQuest::MAP* pMapQuest = nsCsFileTable::g_pQuestMng->GetQuestMap();
	CsQuest::MAP_IT it = pMapQuest->begin();
	for( ; it!=pMapQuest->end(); ++it )
	{
		__InsertQuest( it->second );
	}
}


void _DlgViewSimple::__InsertQuest( CsQuest* pFT )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_MainList.GetItemCount();

	// ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetUniqID() );
	m_list_MainList.InsertItem( nListIndex, szMsg);

	// MngID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetManageID() );
	m_list_MainList.SetItem( nListIndex, 1, LVIF_TEXT, szMsg, 0, 0, 0, 0 );

	// Title Tab
	m_list_MainList.SetItem( nListIndex, 2, LVIF_TEXT, pFT->m_szTitleTab, 0, 0, 0, 0 );

	// Title
	m_list_MainList.SetItem( nListIndex, 3, LVIF_TEXT, pFT->m_szTitleText, 0, 0, 0, 0 );

	// Level
	_stprintf_s( szMsg, 256, L"%d", pFT->GetLevel() );
	m_list_MainList.SetItem( nListIndex, 4, LVIF_TEXT, szMsg, 0, 0, 0, 0 );

	{
		CsQuestRequiteGroup* pGroup = pFT->GetRequiteGroup();
		CsQuestRequite::LIST* pList = pGroup->GetList();
		CsQuestRequite::LIST_IT it = pList->begin();
		CsQuestRequite::LIST_IT itEnd = pList->end();
		CsQuestRequite* pData;
		for( ; it!=itEnd; ++it )
		{
			pData = *it;
			switch( pData->GetType() )
			{
			case CsQuestRequite::MONEY:
				_stprintf_s( szMsg, 256, L"%d", pData->GetTypeID() );
				m_list_MainList.SetItem( nListIndex, 5, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
				break;
			case CsQuestRequite::EXP:
				_stprintf_s( szMsg, 256, L"%d", pData->GetTypeID() );
				m_list_MainList.SetItem( nListIndex, 6, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
				break;
			}
		}
	}

	{
		int nItemIndex = 7;
		CsQuestCompRequireGroup* pGroup = pFT->GetCompRequireGroup();
		CsQuestCompRequire::LIST* pList = pGroup->GetList();
		CsQuestCompRequire::LIST_IT it = pList->begin();
		CsQuestCompRequire::LIST_IT itEnd = pList->end();
		CsQuestCompRequire* pData;
		for( ; it!=itEnd; ++it )
		{
			pData = *it;
			switch( pData->GetType() )
			{
			case CsQuestCompRequire::MONSTER:
				{
					CsMonster::sINFO* pFT =	nsCsFileTable::g_pMonsterMng->GetMonster( pData->GetTypeID() )->GetInfo();
					_stprintf_s( szMsg, 256, L"(%d) %s x %d", pFT->s_nLevel, pFT->s_szName, pData->GetTypeCount() );
					m_list_MainList.SetItem( nListIndex, nItemIndex, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
					++nItemIndex;
				}				
				break;
			}

			if( nItemIndex == 12 )
				break;
		}
	}

	m_list_MainList.SetItemData( nListIndex, pFT->GetUniqID() );

	if( pFT->IsActive() )	_stprintf_s( szMsg, 4, L"O" );
	else					_stprintf_s( szMsg, 4, L"X" );
	m_list_MainList.SetItem( nListIndex, 12, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
}


void _DlgViewSimple::OnNMClickListMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_MainList.GetFirstSelectedItemPosition();
	int nSel = m_list_MainList.GetNextSelectedItem( pos );
	if( nSel < 0 )
		return;

	cQuestObj* pSelObject = g_pViewer->_GetQuestMng()->FindNode( (DWORD)m_list_MainList.GetItemData( nSel ) );
	g_pDiscript->_SetSelQuest( pSelObject );
	
	*pResult = 0;
}

void _DlgViewSimple::OnLvnItemchangedListMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	OnNMClickListMain( pNMHDR, pResult );
	*pResult = 0;
}

void _DlgViewSimple::OnLvnColumnclickListMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	static bool bIncrease = true;
	bIncrease = !bIncrease;

	m_list_MainList.DeleteAllItems();

	CsQuest::MAP* pMap = nsCsFileTable::g_pQuestMng->GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	std::list< CsQuest* > listSort;
	for( ; it!=pMap->end(); ++it )
	{
		listSort.push_back( it->second );
	}

	while( listSort.size() != 0 )
	{
		std::list< CsQuest* >::iterator itInput = listSort.begin();
		std::list< CsQuest* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( pNMLV->iSubItem )
			{
				// ID
			case 0:
				if( ( (*it)->GetUniqID() < (*itInput)->GetUniqID() ) == bIncrease )
					itInput = it;
				break;

				// MngID
			case 1:
				if( ( (*it)->GetManageID() < (*itInput)->GetManageID() ) == bIncrease )
					itInput = it;
				break;

				// TitleTab
			case 2:
				if( ( _tcscmp( (*it)->m_szTitleTab, (*itInput)->m_szTitleTab ) < 0 ) == bIncrease )
					itInput = it;
				break;
				// Title
			case 3:
				if( ( _tcscmp( (*it)->m_szTitleText, (*itInput)->m_szTitleText ) < 0 ) == bIncrease )
					itInput = it;
				break;
				// Level
			case 4:
				if( ( (*it)->GetLevel() < (*itInput)->GetLevel() ) == bIncrease )
					itInput = it;
				break;
			case 12:// 활성/비활성
				if( ( (*it)->IsActive() < (*itInput)->IsActive() ) == bIncrease )
					itInput = it;
				break;
			}
		}

		__InsertQuest( (*itInput ) );
		listSort.erase( itInput );
	}

	*pResult = 0;
}

void _DlgViewSimple::OnNMDblclkListMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_MainList.GetFirstSelectedItemPosition();
	int nSel = m_list_MainList.GetNextSelectedItem( pos );
	if( nSel < 0 )
		return;

	cQuestObj* pSelObject = g_pViewer->_GetQuestMng()->FindNode( (DWORD)m_list_MainList.GetItemData( nSel ) );
	g_pDiscript->_SetSelQuest( pSelObject );
	g_pTop->_SetViewState( WinViewer::VS_VIEWER );
	g_pViewer->_SetSelectCenter();

	*pResult = 0;
}

void _DlgViewSimple::OnNMRclickListMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_MainList.GetFirstSelectedItemPosition();
	int nSel = m_list_MainList.GetNextSelectedItem( pos );
	if( nSel < 0 )
		return;

	cQuestObj* pSelObject = g_pViewer->_GetQuestMng()->FindNode( (DWORD)m_list_MainList.GetItemData( nSel ) );
	g_pDiscript->_SetSelQuest( pSelObject );

	_DlgModal_Simple dlg( (DWORD)m_list_MainList.GetItemData( nSel ) );
	if( dlg.DoModal() == IDOK )
	{		
		g_pViewer->_GetNodeMng()->ReleaseQuestObj( pSelObject, false );
		pSelObject->GetFT()->SetToolPos( CsPoint( -1, -1 ) );
		g_pViewer->_GetNodeMng()->ResistQuestObj( pSelObject, true );
		pSelObject->ResetMiddleTail_ParentChild();
		pSelObject->ResetControl();


		// Level
		CsQuest* pFT = pSelObject->GetFT();
		TCHAR szMsg[ 256 ];
		_stprintf_s( szMsg, 256, L"%d", pFT->GetLevel() );
		m_list_MainList.SetItem( nSel, 3, LVIF_TEXT, szMsg, 0, 0, 0, 0 );

		CsQuestRequiteGroup* pGroup = pFT->GetRequiteGroup();
		CsQuestRequite::LIST* pList = pGroup->GetList();
		CsQuestRequite::LIST_IT it = pList->begin();
		CsQuestRequite::LIST_IT itEnd = pList->end();
		CsQuestRequite* pData;
		for( ; it!=itEnd; ++it )
		{
			pData = *it;
			switch( pData->GetType() )
			{
			case CsQuestRequite::MONEY:
				_stprintf_s( szMsg, 256, L"%d", pData->GetTypeID() );
				m_list_MainList.SetItem( nSel, 4, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
				break;
			case CsQuestRequite::EXP:
				_stprintf_s( szMsg, 256, L"%d", pData->GetTypeID() );
				m_list_MainList.SetItem( nSel, 5, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
				break;
			}
		}		
	}

	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void _DlgViewSimple::OnBnClickedButtonDelete()
{
	POSITION pos = m_list_MainList.GetFirstSelectedItemPosition();
	int nSel = m_list_MainList.GetNextSelectedItem( pos );
	if( nSel < 0 )
		return;

	cQuestObj* pSelObject = g_pViewer->_GetQuestMng()->FindNode( (DWORD)m_list_MainList.GetItemData( nSel ) );
	if( pSelObject )
	{
		pSelObject->UnlinkParent();
		pSelObject->UnlinkChild();
		g_pViewer->_GetNodeMng()->ReleaseQuestObj( pSelObject, true );
		g_pViewer->_GetQuestMng()->DeleteQuestObj( pSelObject );
	}	
	nsCsFileTable::g_pQuestMng->DeleteQuest( (DWORD)m_list_MainList.GetItemData( nSel ) );
	g_pDiscript->_SetSelQuest( NULL );

	// 리스트에서 제거
	m_list_MainList.DeleteItem( nSel );
}

void _DlgViewSimple::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)   
{  
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR;
	BOOL bDoColoring = TRUE;  

	// List Control의 해당 열에서 문자열을 받아옵니다.  
	// 해당 행의 1(2번째)열에 출력된 문자열에 따라서 행의 배경색을 바꿀겁니다.  
	CString strWhatToDo = m_list_MainList.GetItemText(pLVCD->nmcd.dwItemSpec, 12);  

	// 내가 알아서 조건을 주어야겠죠.  
	if( strWhatToDo == _T("O"))   
	{
		bDoColoring = FALSE;  
	}     
	else if( strWhatToDo == _T("X") ) 
	{  
		bDoColoring = TRUE;  
	}  

	*pResult = 0;  

	if( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )   
	{  
		*pResult = CDRF_NOTIFYITEMDRAW;  

	}   
	else if( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )   
	{  

		if( bDoColoring == TRUE )
			pLVCD->clrTextBk = RGB(255, 200, 200);   // 배경색 지정  

		*pResult = CDRF_DODEFAULT;  
	}  
}  