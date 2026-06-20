// _DlgModal_List.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgModal_List.h"


// _DlgModal_List 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgModal_List, CDialog)

_DlgModal_List::_DlgModal_List( eLIST_TYPE type, void* pReturnValue, void* pReturnValue2 /*=NULL*/, int nSetValue /*=-1*/, CWnd* pParent /*=NULL*/)
	: CDialog(_DlgModal_List::IDD, pParent)
{
	m_eListType = type;
	m_nSetValue = nSetValue;
	m_pReturnValue = pReturnValue;
	m_pReturnValue2 = pReturnValue2;
}

_DlgModal_List::~_DlgModal_List()
{
}

void _DlgModal_List::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MAIN, m_list_Main);
}


BEGIN_MESSAGE_MAP(_DlgModal_List, CDialog)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_MAIN, &_DlgModal_List::OnLvnColumnclickListMain)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_MAIN, &_DlgModal_List::OnNMDblclkListMain)
END_MESSAGE_MAP()


// _DlgModal_List 메시지 처리기입니다.

BOOL _DlgModal_List::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch( m_eListType )
	{
	case LT_NPC:
		{
			__Npc_Init();
			__Npc_Read();
		}
		break;
	case LT_MONSTER:
		{
			__Monster_Init();
			__Monster_Read();
		}
		break;
	case LT_ITEM:
		{
			__Item_Init();
			__Item_Read();
		}
		break;
	case LT_DIGIMON:
		{
			__Digimon_Init();
			__Digimon_Read();
		}
		break;
	case LT_QUEST:
		{
			__Quest_Init();
			__Quest_Read();
		}
		break;
	case LT_REGION:
		{
			__Region_Init();
			__Region_Read();
		}
		break;
	case LT_EVENT_SCENE:
		{
			__Event_Scene_Init();
			__Event_Scene_Read();
		}
		break;
	case LT_TUTORIAL_CONDITION:
		{
			__Tutorial_CondigionType_Init();
			__Tutorial_CondigionType_Read();
		}
		break;
	default:
		assert_cs( false );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}


void _DlgModal_List::OnLvnColumnclickListMain(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	static bool bIncrease = true;
	bIncrease = !bIncrease;

	// 선택된 것이 있는가
	POSITION pos = m_list_Main.GetFirstSelectedItemPosition();
	int nIndex = m_list_Main.GetNextSelectedItem( pos );
	DWORD dwSelID = 0;
	if( nIndex >= 0 )
		dwSelID = (DWORD)m_list_Main.GetItemData( nIndex );

	switch( m_eListType )
	{
	case LT_NPC:			__Npc_Sort( pNMLV->iSubItem, bIncrease );		break;
	case LT_MONSTER:		__Monster_Sort( pNMLV->iSubItem, bIncrease );	break;
	case LT_ITEM:			__Item_Sort( pNMLV->iSubItem, bIncrease );		break;
	case LT_DIGIMON:		__Digimon_Sort( pNMLV->iSubItem, bIncrease );	break;
	case LT_QUEST:			__Quest_Sort( pNMLV->iSubItem, bIncrease );		break;
	case LT_REGION:			return;														// 지역은 소트 없음
	case LT_EVENT_SCENE:	return;
	default:				assert_cs( false );
	}

	// 선택된 넘은 소팅후에 다시 선택 해준다.
	if( dwSelID != 0 )
	{
		int nCount = m_list_Main.GetItemCount();
		for( int c=0; c<nCount; ++c )
		{
			if( dwSelID == (DWORD)m_list_Main.GetItemData( c ) )
			{
				m_list_Main.SetItemState( c, LVIS_SELECTED, LVIS_SELECTED );
				return;
			}
		}
	}

	*pResult = 0;
}


void _DlgModal_List::OnNMDblclkListMain(NMHDR *pNMHDR, LRESULT *pResult)
{	
	POSITION pos = m_list_Main.GetFirstSelectedItemPosition();
	int nIndex = m_list_Main.GetNextSelectedItem( pos );

	if( nIndex >= 0 )
	{
		*(DWORD*)m_pReturnValue = (DWORD)m_list_Main.GetItemData( nIndex );
		CDialog::OnOK();
	}
	*pResult = 0;
}


//=========================================================================================================
//
//		Npc
//
//=========================================================================================================
void _DlgModal_List::__Npc_Init()
{
	SetWindowText( L"Npc Table" );
	DWORD dwExStyle = m_list_Main.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Main.SetExtendedStyle( dwExStyle );
	m_list_Main.InsertColumn( 0, L"ID", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 1, L"MapID", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 2, L"이름", LVCFMT_LEFT, 120 );
	m_list_Main.InsertColumn( 3, L"타입", LVCFMT_LEFT, 100 );
	m_list_Main.InsertColumn( 4, L"모델", LVCFMT_LEFT, 60 );
}

void _DlgModal_List::__Npc_Read()
{
	m_list_Main.DeleteAllItems();

	CsNpc::MAP* pMap = nsCsFileTable::g_pNpcMng->GetNpcMap();
	CsNpc::MAP_IT it = pMap->begin();
	for( ; it!=pMap->end(); ++it )
	{
		__Npc_InsertList( it->second );
	}
}

void _DlgModal_List::__Npc_InsertList( CsNpc* pFT )
{
	if( m_nSetValue != -1 )
	{
		if( pFT->GetInfo()->s_dwMapID != m_nSetValue )
			return;
	}

	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_Main.GetItemCount();

	// ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_dwNpcID );
	m_list_Main.InsertItem( nListIndex, szMsg );
	// Map ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_dwMapID );
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	// 이름
	m_list_Main.SetItem( nListIndex, 2, LVIF_TEXT, pFT->GetInfo()->s_szName, 0, 0, 0, 0 );
	// 타입
	switch( pFT->GetInfo()->s_eType )
	{
	case nsCsNpcTable::NT_NONE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"   x", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_TRADE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_DIGITAMA_TRADE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"디지타마 상인", 0, 0, 0, 0 );
		break;
	}
	// 모델
	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_dwModelID );
	m_list_Main.SetItem( nListIndex, 4, LVIF_TEXT, szMsg, 0, 0, 0, 0 );

	m_list_Main.SetItemData( nListIndex, pFT->GetInfo()->s_dwNpcID );
}

void _DlgModal_List::__Npc_Sort( int nColume, bool bIncrease )
{
	m_list_Main.DeleteAllItems();
	
	CsNpc::MAP* pMap = nsCsFileTable::g_pNpcMng->GetNpcMap();
	CsNpc::MAP_IT it = pMap->begin();
	std::list< CsNpc* > listSort;
	for( ; it!=pMap->end(); ++it )
	{
		listSort.push_back( it->second );
	}

	while( listSort.size() != 0 )
	{
		std::list< CsNpc* >::iterator itInput = listSort.begin();
		std::list< CsNpc* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( nColume )
			{
				// ID
			case 0:
				if( ( (*it)->GetInfo()->s_dwNpcID < (*itInput)->GetInfo()->s_dwNpcID ) == bIncrease )
					itInput = it;
				break;
				// Map ID
			case 1:
				if( ( (*it)->GetInfo()->s_dwMapID < (*itInput)->GetInfo()->s_dwMapID ) == bIncrease )
					itInput = it;
				break;
				// 이름
			case 2:
				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
					itInput = it;
				break;
				// 타입
			case 3:
				if( ( (*it)->GetInfo()->s_eType < (*itInput)->GetInfo()->s_eType ) == bIncrease )
					itInput = it;
				break;
				// 모델
			case 4:
				if( ( (*it)->GetInfo()->s_dwModelID < (*itInput)->GetInfo()->s_dwModelID ) == bIncrease )
					itInput = it;
				break;
			default:
				assert_cs( false );
			}
		}

		__Npc_InsertList( (*itInput ) );
		listSort.erase( itInput );
	}
}

//=========================================================================================================
//
//		Monster
//
//=========================================================================================================

void _DlgModal_List::__Monster_Init()
{
	SetWindowText( L"Monster Table" );
	DWORD dwExStyle = m_list_Main.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Main.SetExtendedStyle( dwExStyle );
	m_list_Main.InsertColumn( 0, L"ID", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 1, L"이름", LVCFMT_LEFT, 120 );
	m_list_Main.InsertColumn( 2, L"레벨", LVCFMT_LEFT, 40 );
	m_list_Main.InsertColumn( 3, L"모델", LVCFMT_LEFT, 60 );
}

void _DlgModal_List::__Monster_Read()
{
	m_list_Main.DeleteAllItems();

	CsMonster::MAP* pMap = nsCsFileTable::g_pMonsterMng->GetMonsterMap();
	CsMonster::MAP_IT it = pMap->begin();
	for( ; it!=pMap->end(); ++it )
	{
		__Monster_InsertList( it->second );
	}
}

void _DlgModal_List::__Monster_InsertList( CsMonster* pFT )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_Main.GetItemCount();

	// ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_dwMonsterID );
	m_list_Main.InsertItem( nListIndex, szMsg );
	// 이름
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, pFT->GetInfo()->s_szName, 0, 0, 0, 0 );
	// 레벨
	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_nLevel );
	m_list_Main.SetItem( nListIndex, 2, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
	// 모델
	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_dwModelID );
	m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, szMsg, 0, 0, 0, 0 );

	m_list_Main.SetItemData( nListIndex, pFT->GetInfo()->s_dwMonsterID );
}

void _DlgModal_List::__Monster_Sort( int nColume, bool bIncrease )
{
	m_list_Main.DeleteAllItems();

	CsMonster::MAP* pMap = nsCsFileTable::g_pMonsterMng->GetMonsterMap();
	CsMonster::MAP_IT it = pMap->begin();
	std::list< CsMonster* > listSort;
	for( ; it!=pMap->end(); ++it )
	{
		listSort.push_back( it->second );
	}

	while( listSort.size() != 0 )
	{
		std::list< CsMonster* >::iterator itInput = listSort.begin();
		std::list< CsMonster* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( nColume )
			{
				// ID
			case 0:
				if( ( (*it)->GetInfo()->s_dwMonsterID < (*itInput)->GetInfo()->s_dwMonsterID ) == bIncrease )
					itInput = it;
				break;				
				// 이름
			case 1:
				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
					itInput = it;
				break;
				// 레벨
			case 2:
				if( ( (*it)->GetInfo()->s_nLevel < (*itInput)->GetInfo()->s_nLevel ) == bIncrease )
					itInput = it;
				break;
				// 모델
			case 3:
				if( ( (*it)->GetInfo()->s_dwModelID < (*itInput)->GetInfo()->s_dwModelID ) == bIncrease )
					itInput = it;
				break;
			default:
				assert_cs( false );
			}
		}

		__Monster_InsertList( (*itInput ) );
		listSort.erase( itInput );
	}
}


//=========================================================================================================
//
//		Item
//
//=========================================================================================================

void _DlgModal_List::__Item_Init()
{
	SetWindowText( L"Item Table" );
	DWORD dwExStyle = m_list_Main.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Main.SetExtendedStyle( dwExStyle );
	m_list_Main.InsertColumn( 0, L"ID", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 1, L"이름", LVCFMT_LEFT, 180 );
}

void _DlgModal_List::__Item_Read()
{
	m_list_Main.DeleteAllItems();

	CsItem::MAP* pMap = nsCsFileTable::g_pItemMng->GetItemMap();
	CsItem::MAP_IT it = pMap->begin();
	for( ; it!=pMap->end(); ++it )
	{
		__Item_InsertList( it->second );
	}
}

void _DlgModal_List::__Item_InsertList( CsItem* pFT )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_Main.GetItemCount();

	// ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_dwItemID );
	m_list_Main.InsertItem( nListIndex, szMsg );
	// 이름
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, pFT->GetInfo()->s_szName, 0, 0, 0, 0 );	

	m_list_Main.SetItemData( nListIndex, pFT->GetInfo()->s_dwItemID );
}

void _DlgModal_List::__Item_Sort( int nColume, bool bIncrease )
{
	m_list_Main.DeleteAllItems();

	CsItem::MAP* pMap = nsCsFileTable::g_pItemMng->GetItemMap();
	CsItem::MAP_IT it = pMap->begin();
	std::list< CsItem* > listSort;
	for( ; it!=pMap->end(); ++it )
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
				// ID
			case 0:
				if( ( (*it)->GetInfo()->s_dwItemID < (*itInput)->GetInfo()->s_dwItemID ) == bIncrease )
					itInput = it;
				break;				
				// 이름
			case 1:
				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
					itInput = it;
				break;
			default:
				assert_cs( false );
			}
		}

		__Item_InsertList( (*itInput ) );
		listSort.erase( itInput );
	}
}


//=========================================================================================================
//
//		Digimon
//
//=========================================================================================================

void _DlgModal_List::__Digimon_Init()
{
	SetWindowText( L"Digimon Table" );
	DWORD dwExStyle = m_list_Main.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Main.SetExtendedStyle( dwExStyle );
	m_list_Main.InsertColumn( 0, L"ID", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 1, L"이름", LVCFMT_LEFT, 120 );	
}

void _DlgModal_List::__Digimon_Read()
{
	m_list_Main.DeleteAllItems();

	CsDigimon::MAP* pMap = nsCsFileTable::g_pDigimonMng->GetDigimonMap();
	CsDigimon::MAP_IT it = pMap->begin();
	for( ; it!=pMap->end(); ++it )
	{
		__Digimon_InsertList( it->second );
	}
}

void _DlgModal_List::__Digimon_InsertList( CsDigimon* pFT )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_Main.GetItemCount();
//#ifdef JOINT_PROGRESS // 모델ID 를 디지몬 ID로 교체
	// ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_dwDigimonID );

	m_list_Main.InsertItem( nListIndex, szMsg );
	// 이름
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, pFT->GetInfo()->s_szName, 0, 0, 0, 0 );
	
	m_list_Main.SetItemData( nListIndex, pFT->GetInfo()->s_dwDigimonID );
// #else
// 	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_dwModelID );
// 	m_list_Main.InsertItem( nListIndex, szMsg );
// 	// 이름
// 	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, pFT->GetInfo()->s_szName, 0, 0, 0, 0 );
// 	
// 	m_list_Main.SetItemData( nListIndex, pFT->GetInfo()->s_dwModelID );
// 
// #endif
}

void _DlgModal_List::__Digimon_Sort( int nColume, bool bIncrease )
{
	m_list_Main.DeleteAllItems();

	CsDigimon::MAP* pMap = nsCsFileTable::g_pDigimonMng->GetDigimonMap();
	CsDigimon::MAP_IT it = pMap->begin();
	std::list< CsDigimon* > listSort;
	for( ; it!=pMap->end(); ++it )
	{
		listSort.push_back( it->second );
	}

	while( listSort.size() != 0 )
	{
		std::list< CsDigimon* >::iterator itInput = listSort.begin();
		std::list< CsDigimon* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( nColume )
			{
				// ID
			case 0:
				if( ( (*it)->GetInfo()->s_dwModelID < (*itInput)->GetInfo()->s_dwModelID ) == bIncrease )
					itInput = it;
				break;				
				// 이름
			case 1:
				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
					itInput = it;
				break;			
			default:
				assert_cs( false );
			}
		}

		__Digimon_InsertList( (*itInput ) );
		listSort.erase( itInput );
	}
}

//=========================================================================================================
//
//		Quest
//
//=========================================================================================================

void _DlgModal_List::__Quest_Init()
{
	SetWindowText( L"Quest Table" );
	DWORD dwExStyle = m_list_Main.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Main.SetExtendedStyle( dwExStyle );
	m_list_Main.InsertColumn( 0, L"ID", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 1, L"관리번호", LVCFMT_LEFT, 80 );
	m_list_Main.InsertColumn( 2, L"제목", LVCFMT_LEFT, 360 );
	m_list_Main.InsertColumn( 3, L"시작타겟", LVCFMT_LEFT, 120 );
}

void _DlgModal_List::__Quest_Read()
{
	m_list_Main.DeleteAllItems();

	CsQuest::MAP* pMap = nsCsFileTable::g_pQuestMng->GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	CsQuest::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		__Quest_InsertList( it->second );
	}
}

void _DlgModal_List::__Quest_InsertList( CsQuest* pFT )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_Main.GetItemCount();

	// ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetUniqID() );
	m_list_Main.InsertItem( nListIndex, szMsg );
	// 관리 번호
	_stprintf_s( szMsg, 256, L"%d", pFT->GetManageID() );
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, szMsg, 0, 0, 0, 0 );

	// 제목
	m_list_Main.SetItem( nListIndex, 2, LVIF_TEXT, pFT->m_szTitleText, 0, 0, 0, 0 );

	// 시작 타겟
	switch( pFT->GetStartTarget_Type() )
	{
	case CsQuest::ST_NPC:
		if( nsCsFileTable::g_pNpcMng->IsNpc( pFT->GetStartTarget_ID() ) )
		{
			m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, nsCsFileTable::g_pNpcMng->GetNpc( pFT->GetStartTarget_ID() )->GetInfo()->s_szName, 0, 0, 0, 0 );
		}		
		break;
	case CsQuest::ST_DIGIVICE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"디지바이스", 0, 0, 0, 0 );
		break;
	default:
		assert_cs( false );
	}

	m_list_Main.SetItemData( nListIndex, pFT->GetUniqID() );
}

void _DlgModal_List::__Quest_Sort( int nColume, bool bIncrease )
{
	m_list_Main.DeleteAllItems();

	CsQuest::MAP* pMap = nsCsFileTable::g_pQuestMng->GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	std::list< CsQuest* > listSort;
	for( ; it!=pMap->end(); ++it )
	{
		listSort.push_back( it->second );
	}

	TCHAR nameSrc[ 128 ];
	TCHAR nameDest[ 128 ];

	while( listSort.size() != 0 )
	{
		std::list< CsQuest* >::iterator itInput = listSort.begin();
		std::list< CsQuest* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( nColume )
			{			
				// ID
			case 0:
				if( ( (*it)->GetUniqID() < (*itInput)->GetUniqID() ) == bIncrease )
					itInput = it;
				break;
				// 관리번호
			case 1:
				if( ( (*it)->GetManageID() < (*itInput)->GetManageID() ) == bIncrease )
					itInput = it;
				break;
				// 제목
			case 2:
				if( ( _tcscmp( (*it)->m_szTitleText, (*itInput)->m_szTitleText ) < 0 ) == bIncrease )
					itInput = it;
				break;
				// 시작 타겟
			case 3:
				{					
					switch( (*it)->GetStartTarget_Type() )
					{
					case CsQuest::ST_NPC:
						_tcscpy_s( nameSrc, 128, nsCsFileTable::g_pNpcMng->GetNpc( (*it)->GetStartTarget_ID() )->GetInfo()->s_szName );
						break;
					case CsQuest::ST_DIGIVICE:
						_tcscpy_s( nameSrc, 128, L"디지바이스" );
						break;
					default:
						assert_cs( false );
					}

					switch( (*itInput)->GetStartTarget_Type() )
					{
					case CsQuest::ST_NPC:
						_tcscpy_s( nameDest, 128, nsCsFileTable::g_pNpcMng->GetNpc( (*itInput)->GetStartTarget_ID() )->GetInfo()->s_szName );
						break;
					case CsQuest::ST_DIGIVICE:
						_tcscpy_s( nameDest, 128, L"디지바이스" );
						break;
					default:
						assert_cs( false );
					}
					if( ( _tcscmp( nameSrc, nameDest ) < 0 ) == bIncrease )
						itInput = it;
				}				
				break;
			default:
				assert_cs( false );
			}
		}

		__Quest_InsertList( (*itInput ) );
		listSort.erase( itInput );
	}
}


//=========================================================================================================
//
//		Region
//
//=========================================================================================================

void _DlgModal_List::__Region_Init()
{
	SetWindowText( L"Region Table" );
	DWORD dwExStyle = m_list_Main.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Main.SetExtendedStyle( dwExStyle );
	m_list_Main.InsertColumn( 0, L"맵", LVCFMT_LEFT, 200 );
	m_list_Main.InsertColumn( 1, L"지역", LVCFMT_LEFT, 300 );
}

void _DlgModal_List::__Region_Read()
{
	m_list_Main.DeleteAllItems();

	CsMapList::MAP* pMap = nsCsMapTable::g_pMapListMng->GetListMap();
	CsMapList::MAP_IT it = pMap->begin();
	CsMapList::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		if( nsCsMapTable::g_pMapRegionMng->IsRegionGroup( it->second->GetInfo()->s_dwMapID ) == false )
			continue;

		CsMapRegionGroup* pGroup = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( it->second->GetInfo()->s_dwMapID );
		CsMapRegion::LIST* pList = pGroup->GetRegionList();
		CsMapRegion::LIST_IT itRegion = pList->begin();
		CsMapRegion::LIST_IT itRegionEnd = pList->end();
	//	int nRegionCount = 0;
		for( ; itRegion!=itRegionEnd; ++itRegion )
		{
			__Region_InsertList( *itRegion );
// 			++nRegionCount;
// 			if( nRegionCount == 7 )
// 				break;
		}
	}
}

void _DlgModal_List::__Region_InsertList( CsMapRegion* pFT )
{
	int nListIndex = m_list_Main.GetItemCount();

	// 맵
	m_list_Main.InsertItem( nListIndex, nsCsMapTable::g_pMapListMng->GetList( pFT->GetInfo()->s_dwMapID )->GetInfo()->s_szMapDiscript.c_str() );

	// 지역
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, pFT->GetInfo()->s_szDiscript, 0, 0, 0, 0 );

	// 지역인덱싱
	int nRegionIndex = nsCsMapTable::g_pMapRegionMng->GetRegionGroup( pFT->GetInfo()->s_dwMapID )->FindIndex( pFT );

	m_list_Main.SetItemData( nListIndex, pFT->GetInfo()->s_dwMapID*1000+nRegionIndex );
}



//=========================================================================================================
//
//		Event Scene
//
//=========================================================================================================

void _DlgModal_List::__Event_Scene_Init()
{
	SetWindowText( L"Event Scene Table" );
	DWORD dwExStyle = m_list_Main.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Main.SetExtendedStyle( dwExStyle );
	m_list_Main.InsertColumn( 0, L"인덱스", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 1, L"갯수", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 2, L"첫번째 이벤트 제목", LVCFMT_LEFT, 200 );
	m_list_Main.InsertColumn( 3, L"두번째 이벤트 제목", LVCFMT_LEFT, 200 );
	m_list_Main.InsertColumn( 4, L"세번째 이벤트 제목", LVCFMT_LEFT, 200 );
}

void _DlgModal_List::__Event_Scene_Read()
{
	m_list_Main.DeleteAllItems();

	std::map< UINT, CEventSceneData* >* pMap = g_pEventDataMng->GetMap();
	std::map< UINT, CEventSceneData* >::iterator it = pMap->begin();
	std::map< UINT, CEventSceneData* >::iterator itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		__Event_Scene_InsertList( it->second );
	}
}

void _DlgModal_List::__Event_Scene_InsertList( CEventSceneData* pEventSceneData )
{
	int nListIndex = m_list_Main.GetItemCount();

	TCHAR sz[ MAX_PATH ];

	// 인덱스
	_stprintf_s( sz, MAX_PATH, L"%d", pEventSceneData->GetSceneNum() );
	m_list_Main.InsertItem( nListIndex, sz );

	// 총이벤트 갯수
	_stprintf_s( sz, MAX_PATH, L"%d", pEventSceneData->GetEventSize() );	
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, sz, 0, 0, 0, 0 );

	// 첫번? 타이틀
	if( pEventSceneData->GetEventSize() >= 3 )
	{
		m_list_Main.SetItem( nListIndex, 2, LVIF_TEXT, pEventSceneData->GetEventDatabyIdx( 0 )->GetEventInfo()->s_strTitle, 0, 0, 0, 0 );
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, pEventSceneData->GetEventDatabyIdx( 1 )->GetEventInfo()->s_strTitle, 0, 0, 0, 0 );
		m_list_Main.SetItem( nListIndex, 4, LVIF_TEXT, pEventSceneData->GetEventDatabyIdx( 2 )->GetEventInfo()->s_strTitle, 0, 0, 0, 0 );
	}
	else if( pEventSceneData->GetEventSize() == 2 )
	{
		m_list_Main.SetItem( nListIndex, 2, LVIF_TEXT, pEventSceneData->GetEventDatabyIdx( 0 )->GetEventInfo()->s_strTitle, 0, 0, 0, 0 );
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, pEventSceneData->GetEventDatabyIdx( 1 )->GetEventInfo()->s_strTitle, 0, 0, 0, 0 );
	}
	else if( pEventSceneData->GetEventSize() == 1 )
	{
		m_list_Main.SetItem( nListIndex, 2, LVIF_TEXT, pEventSceneData->GetEventDatabyIdx( 0 )->GetEventInfo()->s_strTitle, 0, 0, 0, 0 );
	}

	m_list_Main.SetItemData( nListIndex, pEventSceneData->GetSceneNum() );
}

//=========================================================================================================
//
//		Tutorial Condition Type
//
//=========================================================================================================

void _DlgModal_List::__Tutorial_CondigionType_Init()
{
	SetWindowText( L"Event Scene Table" );
	DWORD dwExStyle = m_list_Main.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Main.SetExtendedStyle( dwExStyle );
	m_list_Main.InsertColumn( 0, L"인덱스", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 1, L"타입", LVCFMT_LEFT, 60 );
	m_list_Main.InsertColumn( 2, L"설명", LVCFMT_LEFT, 200 );
}

void _DlgModal_List::__Tutorial_CondigionType_Read()
{
	m_list_Main.DeleteAllItems();

	__Tutorial_CondigionType_InsertList( clientCompType::eAccept, L"퀘스트 수락시 완료", L"퀘스트를 수락하면 완료되는 타입" );
	__Tutorial_CondigionType_InsertList( clientCompType::eMovePosition, L"맵의 특정 위치로 이동", L"맵의 X,Y로 이동해야 완료되는 타입 " );
	__Tutorial_CondigionType_InsertList( clientCompType::eCamerControl, L"카메라 조작", L"퀘스트를 수락하면 완료되는 타입" );
	__Tutorial_CondigionType_InsertList( clientCompType::eTargetTrace, L"NPC/몹 위치로 이동", L"타겟의 위치로 이동함" );
	__Tutorial_CondigionType_InsertList( clientCompType::eAttack, L"공격 타입", L"몬스터 공격 관련 값" );
	__Tutorial_CondigionType_InsertList( clientCompType::eScanAndRetrieve, L"스캔", L"퀘스트를 수락하면 완료되는 타입" );
	__Tutorial_CondigionType_InsertList( clientCompType::eDigimonHatch, L"부화", L"퀘스트를 수락하면 완료되는 타입" );
	__Tutorial_CondigionType_InsertList( clientCompType::eDigimonEnchant, L"강화", L"퀘스트를 수락하면 완료되는 타입" );
	__Tutorial_CondigionType_InsertList( clientCompType::eDigimonChange, L"디지몬 교체", L"퀘스트를 수락하면 완료되는 타입" );
}

void _DlgModal_List::__Tutorial_CondigionType_InsertList( int nType, std::wstring typeName, std::wstring wDisciption )
{
	int nListIndex = m_list_Main.GetItemCount();

	TCHAR sz[ MAX_PATH ];

	// 인덱스
	_stprintf_s( sz, MAX_PATH, L"%d", nType );
	m_list_Main.InsertItem( nListIndex, sz );

	// 타입
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, typeName.c_str(), 0, 0, 0, 0 );

	// 설명
	m_list_Main.SetItem( nListIndex, 2, LVIF_TEXT, wDisciption.c_str(), 0, 0, 0, 0 );

	m_list_Main.SetItemData( nListIndex, nType );
}
