// _DlgViewList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgViewList.h"


// _DlgViewList 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgViewList, CDialog)

_DlgViewList::_DlgViewList(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgViewList::IDD, pParent)
{

}

_DlgViewList::~_DlgViewList()
{
}



void _DlgViewList::_Init()
{
	DWORD dwExStyle = m_list_MainList.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_MainList.SetExtendedStyle( dwExStyle );

	m_list_MainList.InsertColumn( 0, L"Mng ID", LVCFMT_LEFT, 60 );
	m_list_MainList.InsertColumn( 1, L"ID", LVCFMT_LEFT, 60 );
	m_list_MainList.InsertColumn( 2, L"Title Tab", LVCFMT_LEFT, 150 );
	m_list_MainList.InsertColumn( 3, L"Title", LVCFMT_LEFT, 300 );
	m_list_MainList.InsertColumn( 4, L"Start", LVCFMT_LEFT, 150 );
	m_list_MainList.InsertColumn( 5, L"Type", LVCFMT_LEFT, 60 );
	m_list_MainList.InsertColumn( 6, L"Active", LVCFMT_LEFT, 60 );
}

void _DlgViewList::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		__ReadQuest();
		m_list_MainList.SetFocus();
	}
}


void _DlgViewList::__ReadQuest()
{
	m_list_MainList.DeleteAllItems();

	CsQuest::MAP* pMapQuest = nsCsFileTable::g_pQuestMng->GetQuestMap();
	CsQuest::MAP_IT it = pMapQuest->begin();
	for( ; it!=pMapQuest->end(); ++it )
	{
		__InsertQuest( it->second );
	}
}

void _DlgViewList::__InsertQuest( CsQuest* pFT )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_MainList.GetItemCount();

	// Manage ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetManageID() );
	m_list_MainList.InsertItem( nListIndex, szMsg );

	// ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetUniqID() );
	m_list_MainList.SetItem( nListIndex, 1, LVIF_TEXT, szMsg, 0, 0, 0, 0 );

	// Title Tab
	m_list_MainList.SetItem( nListIndex, 2, LVIF_TEXT, pFT->m_szTitleTab, 0, 0, 0, 0 );

	// Title
	m_list_MainList.SetItem( nListIndex, 3, LVIF_TEXT, pFT->m_szTitleText, 0, 0, 0, 0 );

	// 시작 타겟
	switch( pFT->GetStartTarget_Type() )
	{
	case CsQuest::ST_NPC:
		{
			if( nsCsFileTable::g_pNpcMng->IsNpc( pFT->GetStartTarget_ID() ) == true  )
			{
				CsNpc* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( pFT->GetStartTarget_ID() );
				if( pFTNpc != NULL )
				{
					m_list_MainList.SetItem( nListIndex, 4, LVIF_TEXT, pFTNpc->GetInfo()->s_szName, 0, 0, 0, 0 );
				}
			}
			else
			{				
				CsMessageBox( MB_OK, L"시작 타겟 엔피씨가 존재 하지 않습니다. Quest = %d, Npc = %d", pFT->GetUniqID(), pFT->GetStartTarget_ID() );
				pFT->SetStartTarget_ID( 0 );
			}
		}
		break;
	case CsQuest::ST_DIGIVICE:
		m_list_MainList.SetItem( nListIndex, 4, LVIF_TEXT, L"디지바이스", 0, 0, 0, 0 );
		break;
	default:
		assert_cs( false );
	}	

	// 타입
	switch( pFT->GetQuestType() )
	{
	case CsQuest::QT_SUB:			_stprintf_s( szMsg, 16, L"서브" );				break;
	case CsQuest::QT_MAIN:			_stprintf_s( szMsg, 16, L"메인" );				break;
	case CsQuest::QT_DAILY:			_stprintf_s( szMsg, 16, L"일일" );				break;
	case CsQuest::QT_REPEAT:		_stprintf_s( szMsg, 16, L"반복" );				break;
	case CsQuest::QT_EVENTREPEAT:	_stprintf_s( szMsg, 16, L"이벤트반복" );		break;
	case CsQuest::QT_ACHIEVE:		_stprintf_s( szMsg, 16, L"업적" );				break;
	case CsQuest::QT_JOINTPROGRESS:		_stprintf_s( szMsg, 16, L"합체 진화" );				break;
	case CsQuest::QT_TUTORIAL:		_stprintf_s( szMsg, 16, L"튜토리얼" );				break;
//	case CsQuest::QT_TIME:		_stprintf_s( szMsg, 16, L"시간제한" );				break;
	case CsQuest::QT_XANTI_JOINTPROGRESS:		_stprintf_s( szMsg, 16, L"X항체합체진화" );				break;
	default:						assert_cs( false );
	}
	m_list_MainList.SetItem( nListIndex, 5, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	m_list_MainList.SetItemData( nListIndex, pFT->GetUniqID() );

	if( pFT->IsActive() )
		_stprintf_s( szMsg, 4, L"O" );
	else
		_stprintf_s( szMsg, 4, L"X" );
	m_list_MainList.SetItem( nListIndex, 6, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
}


void _DlgViewList::OnNMClickListMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_MainList.GetFirstSelectedItemPosition();
	int nSel = m_list_MainList.GetNextSelectedItem( pos );
	if( nSel < 0 )
		return;

	cQuestObj* pSelObject = g_pViewer->_GetQuestMng()->FindNode( (DWORD)m_list_MainList.GetItemData( nSel ) );
	g_pDiscript->_SetSelQuest( pSelObject );

	*pResult = 0;
}


void _DlgViewList::OnNMDblclkListMain(NMHDR *pNMHDR, LRESULT *pResult)
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


void _DlgViewList::OnLvnItemchangedListMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	OnNMClickListMain( pNMHDR, pResult );

	*pResult = 0;
}


// _DlgViewList 메시지 처리기입니다.

void _DlgViewList::OnLvnColumnclickListMain(NMHDR *pNMHDR, LRESULT *pResult)
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

	TCHAR szSrc[ 128 ];
	TCHAR szDest[ 128 ];
	while( listSort.size() != 0 )
	{
		std::list< CsQuest* >::iterator itInput = listSort.begin();
		std::list< CsQuest* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( pNMLV->iSubItem )
			{
				// MngID
			case 0:
				if( ( (*it)->GetManageID() < (*itInput)->GetManageID() ) == bIncrease )
					itInput = it;
				break;
				// ID
			case 1:
				if( ( (*it)->GetUniqID() < (*itInput)->GetUniqID() ) == bIncrease )
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
				// Start
			case 4:
				{
					szSrc[ 0 ] = NULL;
					switch( (*it)->GetStartTarget_Type() )
					{
					case CsQuest::ST_NPC:
						if( nsCsFileTable::g_pNpcMng->IsNpc( (*it)->GetStartTarget_ID() ) )
							_tcscpy_s( szSrc, 128, nsCsFileTable::g_pNpcMng->GetNpc( (*it)->GetStartTarget_ID() )->GetInfo()->s_szName );
						break;
					case CsQuest::ST_DIGIVICE:
						_tcscpy_s( szSrc, 128, L"_디지바이스" );
						break;
					default:					assert_cs( false );
					}

					switch( (*itInput)->GetStartTarget_Type() )
					{
					case CsQuest::ST_NPC:
						if( nsCsFileTable::g_pNpcMng->IsNpc( (*itInput)->GetStartTarget_ID() ) )
							_tcscpy_s( szDest, 128, nsCsFileTable::g_pNpcMng->GetNpc( (*itInput)->GetStartTarget_ID() )->GetInfo()->s_szName );
						break;
					case CsQuest::ST_DIGIVICE:
						_tcscpy_s( szDest, 128, L"_디지바이스" );
						break;
					default:					assert_cs( false );
					}
					if( ( _tcscmp( szSrc, szDest ) < 0 ) == bIncrease )
						itInput = it;
				}				
				break;
				// 타입
			case 5:
				if( ( (*it)->GetQuestType() < (*itInput)->GetQuestType() ) == bIncrease )
					itInput = it;
				break;
			case 6:// 활성/비활성
				if( ( (*it)->IsActive() < (*itInput)->IsActive() ) == bIncrease )
					itInput = it;
				break;
			default:
				assert_cs( false );
			}
		}

		__InsertQuest( (*itInput ) );
		listSort.erase( itInput );
	}

	*pResult = 0;
}

void _DlgViewList::OnBnClickedButtonDelete()
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


void _DlgViewList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAIN, m_list_MainList);
}

void _DlgViewList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)   
{  
	NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW*)pNMHDR;
 	BOOL bDoColoring = TRUE;  
 
	// List Control의 해당 열에서 문자열을 받아옵니다.  
	// 해당 행의 1(2번째)열에 출력된 문자열에 따라서 행의 배경색을 바꿀겁니다.  
	CString strWhatToDo = m_list_MainList.GetItemText(pLVCD->nmcd.dwItemSpec, 6);  

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


BEGIN_MESSAGE_MAP(_DlgViewList, CDialog)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CLICK,			IDC_LIST_MAIN, &_DlgViewList::OnNMClickListMain)
	ON_NOTIFY(LVN_ITEMCHANGED,	IDC_LIST_MAIN, &_DlgViewList::OnLvnItemchangedListMain)
	ON_NOTIFY(LVN_COLUMNCLICK,	IDC_LIST_MAIN, &_DlgViewList::OnLvnColumnclickListMain)
	ON_NOTIFY(NM_DBLCLK,		IDC_LIST_MAIN, &_DlgViewList::OnNMDblclkListMain)
	ON_NOTIFY(NM_CUSTOMDRAW,	IDC_LIST_MAIN, &_DlgViewList::OnNMCustomdraw)	
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &_DlgViewList::OnBnClickedButtonDelete)
END_MESSAGE_MAP()
