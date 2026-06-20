// _DlgModal_List.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "NpcTool.h"
#include "_DlgModal_List.h"


// _DlgModal_List 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgModal_List, CDialog)

_DlgModal_List::_DlgModal_List( eLIST_TYPE type, void* pReturnValue, CWnd* pParent /*=NULL*/)
: CDialog(_DlgModal_List::IDD, pParent)
{
	m_eListType = type;
	m_pReturnValue = pReturnValue;
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
	case LT_NPC:		__Npc_Sort( pNMLV->iSubItem, bIncrease );		break;
	case LT_MONSTER:	__Monster_Sort( pNMLV->iSubItem, bIncrease );	break;
	case LT_ITEM:		__Item_Sort( pNMLV->iSubItem, bIncrease );		break;
	case LT_DIGIMON:	__Digimon_Sort( pNMLV->iSubItem, bIncrease );	break;
	case LT_QUEST:		__Quest_Sort( pNMLV->iSubItem, bIncrease );		break;
	default:			assert_cs( false );
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
	case nsCsNpcTable::NT_GUILD:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"길드", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_PORTAL:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"포탈", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_DIGICORE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"디지코어상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_MAKE_TACTICS:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"용병생성", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_ELEMENT_ITEM:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"속성상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_WAREHOUSE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"창고", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_TACTICSHOUSE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"용병보관소", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_CAPSULE_MACHINE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"캡슐머신", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_SKILL:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"스킬상급자", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_EVENT_STORE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"이벤트상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_DATS_PORTAL:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"닷츠본부이동", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_PROPERTY_STORE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"물물교환상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_GOTCHA_MACHINE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"레어머신", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_MASTERS_MATCHING:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"마스터즈매치", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_MYSTERY_MACHINE:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"수수께끼머신", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_SPIRIT_EVO:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"스피릿진화", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_SPECIAL_EVENT:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"이벤트", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"아이템제작상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"배틀신청NPC", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"무한대전진행NPC", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"무한대전게시판NPC", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
		m_list_Main.SetItem( nListIndex, 3, LVIF_TEXT, L"특수 진화 NPC", 0, 0, 0, 0 );
		break;
	default:
		assert_cs( false );
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

	// ID
	_stprintf_s( szMsg, 256, L"%d", pFT->GetInfo()->s_dwModelID );
	m_list_Main.InsertItem( nListIndex, szMsg );
	// 이름
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, pFT->GetInfo()->s_szName, 0, 0, 0, 0 );

	m_list_Main.SetItemData( nListIndex, pFT->GetInfo()->s_dwModelID );
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
	m_list_Main.InsertColumn( 1, L"제목", LVCFMT_LEFT, 300 );	
}

void _DlgModal_List::__Quest_Read()
{
	m_list_Main.DeleteAllItems();

	CsQuest::MAP* pMap = nsCsFileTable::g_pQuestMng->GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	for( ; it!=pMap->end(); ++it )
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
	// 이름
	m_list_Main.SetItem( nListIndex, 1, LVIF_TEXT, pFT->m_szTitleText, 0, 0, 0, 0 );

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
				// 이름
			case 1:
				if( ( _tcscmp( (*it)->m_szTitleText, (*itInput)->m_szTitleText ) < 0 ) == bIncrease )
					itInput = it;
				break;			
			default:
				assert_cs( false );
			}
		}

		__Quest_InsertList( (*itInput ) );
		listSort.erase( itInput );
	}
}
