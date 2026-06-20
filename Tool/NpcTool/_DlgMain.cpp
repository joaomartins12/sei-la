
#include "stdafx.h"
#include "NpcToolDlg.h"


//=========================================================================================================
//
//		Base
//
//=========================================================================================================

void CNpcToolDlg::_Delete()
{
	m_AddNpc.Delete();
	nsCsFileTable::g_FileTableMng.Delete();
	g_pModelDataMng->Delete();
	SAFE_NIDELETE( g_pModelDataMng );

	NiShutdown();

	const NiInitOptions* pkInitOptions = NiStaticDataManager::GetInitOptions();
	NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
	NiExternalDelete pkInitOptions;
	NiExternalDelete pkAllocator;
}

void CNpcToolDlg::OnDestroy()
{
	/*if( m_Dlg_TalkView.GetSafeHwnd() != NULL )
	{
		m_Dlg_TalkView.SendMessage( WM_DESTROY );
	}*/

	CRect rcWindow;
	GetWindowRect( &rcWindow );
	g_Resist.GetWindowInfo()->s_ptMainPos.x = rcWindow.left;
	g_Resist.GetWindowInfo()->s_ptMainPos.y = rcWindow.top;

	CDialog::OnDestroy();
}



void CNpcToolDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}


int CNpcToolDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CNpcToolDlg::__Init()
{
	nsCsFileTable::g_bUseBase  = false;
	nsCsFileTable::g_bUseMonster = true;
	nsCsFileTable::g_bUseSkill = false;
	nsCsMapTable::g_eModeMapMonster = nsCsMapTable::eMode_None;
	nsCsMapTable::g_bUseMapPortal = false;
	nsCsMapTable::g_bUseMapRegion = false;
	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapNpc = false;
	nsCsMapTable::g_bUseMapResurrection = false;

	Csini x( ".\\__ToolLocal.ini" );	
	nsCsFileTable::eLANGUAGE lan;
#ifdef VERSION_USA	//GSP 버전은 고정으로 영문버전
	lan = nsCsFileTable::ENGLISH;	
	CsMessageBox( MB_OK, L"이것은 영문 버젼이에요.~!" );

#elif VERSION_HK	//홍콩 버전
	lan = nsCsFileTable::HONGKONG;	
	CsMessageBox( MB_OK, L"이것은 홍콩 버젼이에요.~!" );

#elif VERSION_TW	//대만 버전
	lan = nsCsFileTable::TAIWAN;	
	CsMessageBox( MB_OK, L"이것은 대만 버젼이에요.~!" );

#elif VERSION_TH		//태국 버전
	lan = nsCsFileTable::THAILAND;	
	CsMessageBox( MB_OK, L"이것은 태국 버젼이에요.~!" );

#else
	char* cLan = x.GetStr( "LANGUAGE", "tool" );
// 	if( cLan && ( strcmp( cLan, "ENGLISH" ) == 0 ) )
// 	{
// 		lan = nsCsFileTable::ENGLISH;	
// 		CsMessageBox( MB_OK, L"이것은 영문 버젼이에요.~!" );
// 	}
// 	else if( cLan && ( strcmp( cLan, "THAILAND" ) == 0 ) )
// 	{
// 		lan = nsCsFileTable::THAILAND;
// 		CsMessageBox( MB_OK, L"สวัสดีครับ ยินดีที่ได้รู้จักครับ~!" );		
// 	}
//	else
	{
		lan = nsCsFileTable::KOREA_TRANSLATION;
		CsMessageBox( MB_OK, L"이것은 한국 버젼이에요.~!" );		
	}
#endif

#ifdef _DEBUG
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, lan );
#else
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, lan );
#endif /*_DEBUG*/

	NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(	NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(),	false) );
	NiInit(pkInitOptions);

	g_pModelDataMng = NiNew CsModelDataMng;
	g_pModelDataMng->Init();


	// 윈도우 이동
	SetWindowPos( NULL, g_Resist.GetWindowInfo()->s_ptMainPos.x, g_Resist.GetWindowInfo()->s_ptMainPos.y, 0, 0, SWP_NOSIZE );	

	m_AddNpc.Init();

	__InitNpcList();
	__InitNpcID();
	__InitMapID();
	__InitNpcType();
	__InitMoveType();
	__InitNpcPenName();
	__InitNpcName();
	__InitNpcModel();
	__InitNpcTalk();
	__InitDispZoom();
	__InitDispInfo();
	__InitNpcExtra();

	__ResetNpc( true );
}

void CNpcToolDlg::__ResetNpc( bool bResetList )
{
	if( bResetList == true )
		__ReadNpcList();

	POSITION pos = m_list_NpcList.GetFirstSelectedItemPosition();
	int nSelNpcID = m_list_NpcList.GetNextSelectedItem( pos );
	if( nSelNpcID == -1 )
	{
		m_pBtnAdd.EnableWindow( TRUE );
		m_pSelNpc = NULL;
	}
	else
	{
		m_pBtnAdd.EnableWindow( FALSE );
		DWORD dwNpcID = (DWORD)m_list_NpcList.GetItemData( nSelNpcID );
		m_pSelNpc = nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID );
		assert_cs( m_pSelNpc );
	}

	__ReadNpcID();
	__ReadMapID();
	__ReadNpcType();
	__ReadMoveType();
	__ReadNpcPenName();
	__ReadNpcName();
	__ReadNpcModel();
	__ReadNpcTalk();
	__ReadDispZoom();
	__ReadDispInfo();
	__ReadNpcExtra();
}

//=========================================================================================================
//
//		NPC List
//
//=========================================================================================================

void CNpcToolDlg::__InitNpcList()
{
	DWORD dwExStyle = m_list_NpcList.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_NpcList.SetExtendedStyle( dwExStyle );
	m_list_NpcList.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
	m_list_NpcList.InsertColumn( 1, L"맵", LVCFMT_RIGHT, 60 );
	m_list_NpcList.InsertColumn( 2, L"호", LVCFMT_LEFT, 120 );
	m_list_NpcList.InsertColumn( 3, L"이름", LVCFMT_LEFT, 120 );
	m_list_NpcList.InsertColumn( 4, L"타입", LVCFMT_LEFT, 120 );
	m_list_NpcList.InsertColumn( 5, L"이동타입", LVCFMT_LEFT, 120 );
	m_list_NpcList.InsertColumn( 6, L"이벤트타입", LVCFMT_LEFT, 120 );
}

void CNpcToolDlg::__ReadNpcList()
{
	m_list_NpcList.DeleteAllItems();

	CsNpc::MAP* pMap = nsCsFileTable::g_pNpcMng->GetNpcMap();
	CsNpc::MAP_IT it = pMap->begin();
	CsNpc::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		__WriteNpcList( m_list_NpcList.GetItemCount(), it->second );
	}
}

void CNpcToolDlg::__WriteNpcList()
{
	if( m_pSelNpc == NULL )
		return;

	POSITION pos = m_list_NpcList.GetFirstSelectedItemPosition();
	int nListIndex = m_list_NpcList.GetNextSelectedItem( pos );
	assert_cs( nListIndex >= 0 );
	
	__WriteNpcList( nListIndex, m_pSelNpc );
}


void CNpcToolDlg::__WriteNpcList( int nListIndex, CsNpc* pNpc )
{
	TCHAR szMsg[ 512 ];
	_stprintf_s( szMsg, 512, L"%d", pNpc->GetInfo()->s_dwNpcID );

	if( m_list_NpcList.GetItemCount() <= nListIndex )
		m_list_NpcList.InsertItem( nListIndex, szMsg );
	else
		m_list_NpcList.SetItemText( nListIndex, 0, szMsg );

	_stprintf_s( szMsg, 512, L"%d", pNpc->GetInfo()->s_dwMapID );
	m_list_NpcList.SetItem( nListIndex, 1, LVIF_TEXT, szMsg, 0, 0, 0, 0  );
	m_list_NpcList.SetItem( nListIndex, 2, LVIF_TEXT, pNpc->GetInfo()->s_szPenName, 0, 0, 0, 0  );
	m_list_NpcList.SetItem( nListIndex, 3, LVIF_TEXT, pNpc->GetInfo()->s_szName, 0, 0, 0, 0  );
	m_list_NpcList.SetItem( nListIndex, 6, LVIF_TEXT, L" x ", 0, 0, 0, 0  );
	switch( pNpc->GetType()->s_eNpcType )
	{
	case nsCsNpcTable::NT_NONE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L" x ", 0, 0, 0, 0  );
		break;
	case nsCsNpcTable::NT_TRADE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"상인", 0, 0, 0, 0  );
		break;
	case nsCsNpcTable::NT_DIGITAMA_TRADE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"디지타마 상인", 0, 0, 0, 0  );
		break;
	case nsCsNpcTable::NT_PORTAL:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"포탈", 0, 0, 0, 0  );
		break;
	case nsCsNpcTable::NT_MAKE_TACTICS:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"용병생성", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_ELEMENT_ITEM:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"속성상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_WAREHOUSE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"창고", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_TACTICSHOUSE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"용병보관소", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_GUILD:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"길드", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_DIGICORE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"디지코상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_CAPSULE_MACHINE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"캡슐머신", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_SKILL:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"스킬상급자", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_EVENT_STORE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"이벤트상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_DATS_PORTAL:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"닷츠본부이동", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_PROPERTY_STORE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"물물교환상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_GOTCHA_MACHINE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"레어머신", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_MASTERS_MATCHING:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"마스터즈매치", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_MYSTERY_MACHINE:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"수수께끼머신", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_SPIRIT_EVO:
		m_list_NpcList.SetItem( nListIndex, 3, LVIF_TEXT, L"스피릿진화", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_SPECIAL_EVENT:
		{
			m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"이벤트", 0, 0, 0, 0 );
			sNPC_TYPE_SPECIAL_EVENT* pEventType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetType();

			switch( pEventType->GetSubType() )
			{
			case sNPC_TYPE_SPECIAL_EVENT::EVENT_CARDGAME:
				{
					m_list_NpcList.SetItem( nListIndex, 6, LVIF_TEXT, L"피에몬 카드게임", 0, 0, 0, 0  );
				}
				break;
			case sNPC_TYPE_SPECIAL_EVENT::EVENT_PINOKIMON:
				{
					m_list_NpcList.SetItem( nListIndex, 6, LVIF_TEXT, L"피노키몬 카드이벤트", 0, 0, 0, 0  );
				}
				break;
			default:
				{
					m_list_NpcList.SetItem( nListIndex, 6, LVIF_TEXT, L" x ", 0, 0, 0, 0  );
				}
				break;
			}
		}
		break;
	case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"아이템제작상인", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"배틀신청NPC", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
		m_list_NpcList.SetItem( nListIndex, 4, LVIF_TEXT, L"무한대전진행NPC", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
		m_list_NpcList.SetItem( nListIndex, 5, LVIF_TEXT, L"무한대전게시판NPC", 0, 0, 0, 0 );
		break;
	case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
		m_list_NpcList.SetItem( nListIndex, 5, LVIF_TEXT, L"특수 진화 NPC", 0, 0, 0, 0 );
		break;
	default:
		assert_cs( false );
	}

	switch( pNpc->GetInfo()->s_eMoveType )
	{
	case nsCsNpcTable::MT_NONE:
		m_list_NpcList.SetItem( nListIndex, 5, LVIF_TEXT, L" 고정형 ", 0, 0, 0, 0  );
		break;
	case nsCsNpcTable::MT_MOVE:
		m_list_NpcList.SetItem( nListIndex, 5, LVIF_TEXT, L" 이동형 ", 0, 0, 0, 0  );
		break;
	default:
		assert_cs( false );
	}
	m_list_NpcList.SetItemData( nListIndex, pNpc->GetInfo()->s_dwNpcID );	
}

// 컬럼 클릭.. 정렬시 사용
void CNpcToolDlg::OnLvnColumnclickListNpclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nColume = pNMLV->iSubItem;
	static bool bIncrease = true;
	bIncrease = !bIncrease;

	m_list_NpcList.DeleteAllItems();
	CsNpc::MAP* pMap = nsCsFileTable::g_pNpcMng->GetNpcMap();
	CsNpc::MAP_IT it = pMap->begin();
	CsNpc::MAP_IT itEnd = pMap->end();
	
	std::list< CsNpc* > listSort;
	for( ; it!=itEnd; ++it )
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
			case 0:
				if( ( (*it)->GetInfo()->s_dwNpcID < (*itInput)->GetInfo()->s_dwNpcID ) == bIncrease )
					itInput = it;
				break;
			case 1:
				if( ( (*it)->GetInfo()->s_dwMapID < (*itInput)->GetInfo()->s_dwMapID ) == bIncrease )
					itInput = it;				
				break;
			case 2:
				if( ( _tcscmp( (*it)->GetInfo()->s_szPenName, (*itInput)->GetInfo()->s_szPenName ) < 0 ) == bIncrease )
					itInput = it;
				break;
			case 3:
				if( ( _tcscmp( (*it)->GetInfo()->s_szName, (*itInput)->GetInfo()->s_szName ) < 0 ) == bIncrease )
					itInput = it;
				break;
			case 4:
				if( ( (*it)->GetInfo()->s_eType < (*itInput)->GetInfo()->s_eType ) == bIncrease )
					itInput = it;
				break;
			default:
				assert_cs( false );
			}
		}

		__WriteNpcList( m_list_NpcList.GetItemCount(), (*itInput ) );
		listSort.erase( itInput );
	}

	*pResult = 0;
}

// 리스트 변경
void CNpcToolDlg::OnNMClickListNpclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_NpcList.GetFirstSelectedItemPosition();
	int nItem = m_list_NpcList.GetNextSelectedItem( pos );

	__ResetNpc( false );
	*pResult = 0;
}

// 선택 리스트 제거
void CNpcToolDlg::OnBnClickedButtonDellist()
{
	POSITION pos = m_list_NpcList.GetFirstSelectedItemPosition();
	int nListIndex = m_list_NpcList.GetNextSelectedItem( pos );

	if( nListIndex == -1 )
		return;

	nsCsFileTable::g_pNpcMng->DeleteNpc( (DWORD)m_list_NpcList.GetItemData( nListIndex ) );
	m_list_NpcList.DeleteItem( nListIndex );

	m_AddNpc.Delete();
	m_AddNpc.Init();
	__ResetNpc( false );
}


//=========================================================================================================
//
//		NPC ID
//
//=========================================================================================================

void CNpcToolDlg::__InitNpcID()
{
	m_edit_NpcID.SetLimitText( 8 );
	m_edit_NpcID.SetWindowText( L"0" );
}

void CNpcToolDlg::__ReadNpcID()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	_Int2Control( &m_edit_NpcID, pNpc->GetInfo()->s_dwNpcID );
}

// Npc ID 에디트 변경시 호출
void CNpcToolDlg::OnEnChangeEditNpcid()
{
	DWORD dwNpcID = _Control2Int( &m_edit_NpcID );
	if( nsCsFileTable::g_pNpcMng->IsNpc( dwNpcID ) )
	{
		if( m_pSelNpc != nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID ) )
		{
			m_edit_NpcID_Discript.SetWindowText( L"존재ID :적용안됨" );
			return;
		}
	}

	if( ( dwNpcID == 0 )||( nsCsFileTable::g_pNpcMng->IsNpc( dwNpcID ) == true ) )
	{
		m_edit_NpcID_Discript.SetWindowText( L"xx ID 가 이미존재 xx" );
	}
	else
	{
		m_edit_NpcID_Discript.SetWindowText( L"사용 가능" );
	}

	// 정보 입력
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	if( m_pSelNpc )
	{
		nsCsFileTable::g_pNpcMng->GetNpcMap()->erase( m_pSelNpc->GetInfo()->s_dwNpcID );
		( *nsCsFileTable::g_pNpcMng->GetNpcMap() )[ dwNpcID ] = m_pSelNpc;
	}
	pNpc->GetInfo()->s_dwNpcID = dwNpcID;
	__WriteNpcList();
}


//=========================================================================================================
//
//		MAP ID
//
//=========================================================================================================

void CNpcToolDlg::__InitMapID()
{
	m_combo_MapID.ResetContent();

	CsMapList::MAP* pmapMapList = nsCsMapTable::g_pMapListMng->GetListMap();
	CsMapList::MAP_IT it = pmapMapList->begin();
	TCHAR sz[ MAX_FILENAME ];
	TCHAR szMsg[ MAX_PATH ];
	for( ; it!=pmapMapList->end(); ++it )
	{
		int nComboIdx = m_combo_MapID.GetCount();
		M2W( sz, it->second->GetInfo()->s_cMapName.c_str(), MAX_FILENAME );
		_stprintf_s( szMsg, MAX_PATH, L"%d - ", it->second->GetInfo()->s_dwMapID );
		_tcscat_s( szMsg, MAX_PATH, sz );
		m_combo_MapID.InsertString( nComboIdx, szMsg );
		m_combo_MapID.SetItemData( nComboIdx, it->second->GetInfo()->s_dwMapID );
	}

	m_combo_MapID.SetCurSel( 0 );
	OnCbnSelchangeComboMapid();
}

void CNpcToolDlg::__ReadMapID()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	_ComboSelect_DWORD_Data( &m_combo_MapID, pNpc->GetInfo()->s_dwMapID );
	OnCbnSelchangeComboMapid();
}

// MapID 콤보 박스 변경
void CNpcToolDlg::OnCbnSelchangeComboMapid()
{
	DWORD dwMapID = (DWORD)m_combo_MapID.GetItemData( m_combo_MapID.GetCurSel() );

	CsMapList* pMapList = nsCsMapTable::g_pMapListMng->GetList( dwMapID );
	m_edit_MapDiscript.SetWindowText( pMapList->GetInfo()->s_szMapDiscript.c_str() );

	// 정보 입력
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	pNpc->GetInfo()->s_dwMapID = dwMapID;
	__WriteNpcList();
}

enum npcType
{
		eNONE				=	0,
		eTRADE			,
		eDIGITAMA_TRADE	,
		ePORTAL			,
		eMAKE_TACTICS		,
		eELEMENT_ITEM		,
		eWAREHOUSE		,
		eTACTICSHOUSE		,
		eGUILD			,
		eDIGICORE			,
		eCAPSULE_MACHINE	,
		eSKILL			,
		eEVENT_STORE		,
		eDATS_PORTAL		,
		ePROPERTY_STORE	,
		eGOTCHA_MACHINE	,
		eMASTERS_MATCHING	,
		eMYSTERY_MACHINE	,
		eSPIRIT_EVO		,
		eSPECIAL_EVENT	,
		eITEMPRODUCTION_NPC,
		eBATTLE_REGISTRANT_NPC,// 배틀 신청 NPC
		eInfiniteWar_Npc,
		eInfiniteWar_NoticeBoard_Npc,
		eExtra_Evolution_Npc,
};

//=========================================================================================================
//
//		NPC Type
//
//=========================================================================================================

void CNpcToolDlg::__InitNpcType()
{
	CRect rc, rcParent;
	GetClientRect( &rcParent );	
	//m_combo_NpcType.GetWindowRect( &rc );
	m_dlg_Type_Trade.Create( _Dlg_Type_Trade::IDD, this );
	m_dlg_Type_Trade.SetWindowPos( NULL, rcParent.left + 638, rcParent.top+104, 450, rcParent.Height()-33, 0 );
	m_dlg_Type_Trade.ShowWindow( SW_HIDE );

	/*m_dlg_Type_DigiCore.Create( CDlg_Type_DigiCore::IDD, this );
	m_dlg_Type_DigiCore.SetWindowPos( NULL, rcParent.left + 638, rcParent.top+104, 385, rcParent.Height()-33, 0 );
	m_dlg_Type_DigiCore.ShowWindow( SW_HIDE );*/

	m_dlg_Type_Portal.Create( _Dlg_Type_Portal::IDD, this );
	m_dlg_Type_Portal.SetWindowPos( NULL, rcParent.left + 638, rcParent.top+104, 385, rcParent.Height()-33, 0 );
	m_dlg_Type_Portal.ShowWindow( SW_HIDE );

	m_combo_SubType.ResetContent();
	m_combo_SubType.InsertString( sNPC_TYPE_SPECIAL_EVENT::EVENT_NONE, L" x " );
	m_combo_SubType.InsertString( sNPC_TYPE_SPECIAL_EVENT::EVENT_CARDGAME, L"피에몬 카드게임" );
	m_combo_SubType.InsertString( sNPC_TYPE_SPECIAL_EVENT::EVENT_PINOKIMON, L"피노키몬 카드게임" );

	m_combo_NpcType.ResetContent();

	m_combo_NpcType.InsertString( npcType::eNONE, L" x " );
	m_combo_NpcType.InsertString( npcType::eTRADE, L"상인" );
	m_combo_NpcType.InsertString( npcType::eDIGITAMA_TRADE, L"디지타마상인" );
	m_combo_NpcType.InsertString( npcType::ePORTAL, L"포탈 도우미" );
	m_combo_NpcType.InsertString( npcType::eMAKE_TACTICS, L"용병생성" );
	m_combo_NpcType.InsertString( npcType::eELEMENT_ITEM, L"속성상인" );
	m_combo_NpcType.InsertString( npcType::eWAREHOUSE, L"창고" );
	m_combo_NpcType.InsertString( npcType::eTACTICSHOUSE, L"용병보관소" );
	m_combo_NpcType.InsertString( npcType::eGUILD, L"길드" );
	m_combo_NpcType.InsertString( npcType::eDIGICORE, L"디지코어상인" );
	m_combo_NpcType.InsertString( npcType::eCAPSULE_MACHINE, L"캡슐머신" );
	m_combo_NpcType.InsertString( npcType::eSKILL, L"스킬상급자" );
	m_combo_NpcType.InsertString( npcType::eEVENT_STORE, L"이벤트상인" );
	m_combo_NpcType.InsertString( npcType::eDATS_PORTAL, L"닷츠본부이동" );
	m_combo_NpcType.InsertString( npcType::ePROPERTY_STORE, L"물물교환상인" );
	m_combo_NpcType.InsertString( npcType::eGOTCHA_MACHINE, L"레어머신" );
	m_combo_NpcType.InsertString( npcType::eGOTCHA_MACHINE, L"마스터즈 매치" );
	m_combo_NpcType.InsertString( npcType::eGOTCHA_MACHINE, L"수수께끼머신" );
	m_combo_NpcType.InsertString( npcType::eSPIRIT_EVO, L"스피릿진화" );
	m_combo_NpcType.InsertString( npcType::eSPECIAL_EVENT, L"이벤트" );
	m_combo_NpcType.InsertString( npcType::eITEMPRODUCTION_NPC, L"아이템제작상인" );
	m_combo_NpcType.InsertString( npcType::eBATTLE_REGISTRANT_NPC, L"배틀등록NPC" );
	m_combo_NpcType.InsertString( npcType::eInfiniteWar_Npc, L"무한대전NPC" );
	m_combo_NpcType.InsertString( npcType::eInfiniteWar_NoticeBoard_Npc, L"무한대전게시판NPC" );
	m_combo_NpcType.InsertString( npcType::eExtra_Evolution_Npc, L"특수 진화 NPC" );	
	
	m_combo_NpcType.SetCurSel( npcType::eNONE );	
	OnCbnSelchangeComboNpctype();
}

void CNpcToolDlg::__ReadNpcType()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	m_combo_NpcType.SetCurSel( pNpc->GetInfo()->s_eType );	
	OnCbnSelchangeComboNpctype();
}

void CNpcToolDlg::__InitMoveType()
{
	CRect rc, rcParent;
	GetClientRect( &rcParent );		

	m_Combo_MoveType.ResetContent();
	m_Combo_MoveType.InsertString( nsCsNpcTable::MT_NONE, L"고정형NPC" );
	m_Combo_MoveType.InsertString( nsCsNpcTable::MT_MOVE, L"이동형NPC" );

	m_Combo_MoveType.SetCurSel( nsCsNpcTable::MT_NONE );	

	OnCbnSelchangeComboNpctype2();	
}

void CNpcToolDlg::__ReadMoveType()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	m_Combo_MoveType.SetCurSel( pNpc->GetInfo()->s_eMoveType );
	OnCbnSelchangeComboNpctype2();	
}

// 타입 콤보 박스 변경
void CNpcToolDlg::OnCbnSelchangeComboNpctype()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	m_combo_SubType.ShowWindow( SW_HIDE );

	switch( m_combo_NpcType.GetCurSel() )
	{
	case npcType::eNONE:
		pNpc->SetType( nsCsNpcTable::NT_NONE );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eTRADE:
		pNpc->SetType( nsCsNpcTable::NT_TRADE );
		m_dlg_Type_Trade.ShowWindow( SW_SHOW );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		m_dlg_Type_Trade._SetNpcType( pNpc->GetType() );
		break;
	case npcType::eDIGITAMA_TRADE:
		pNpc->SetType( nsCsNpcTable::NT_DIGITAMA_TRADE );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::ePORTAL:
		pNpc->SetType( nsCsNpcTable::NT_PORTAL );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_SHOW );
		m_dlg_Type_Portal._SetNpcType( pNpc->GetType() );
		break;
	case npcType::eMAKE_TACTICS:
		pNpc->SetType( nsCsNpcTable::NT_MAKE_TACTICS );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eELEMENT_ITEM:
		pNpc->SetType( nsCsNpcTable::NT_ELEMENT_ITEM );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eWAREHOUSE:
		pNpc->SetType( nsCsNpcTable::NT_WAREHOUSE );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eTACTICSHOUSE:
		pNpc->SetType( nsCsNpcTable::NT_TACTICSHOUSE );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eGUILD:
		pNpc->SetType( nsCsNpcTable::NT_GUILD );
		m_dlg_Type_Trade.ShowWindow( SW_SHOW );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		m_dlg_Type_Trade._SetNpcType( pNpc->GetType() );
		break;	
	case npcType::eDIGICORE:
		pNpc->SetType( nsCsNpcTable::NT_DIGICORE );		
		m_dlg_Type_Trade.ShowWindow( SW_SHOW );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		m_dlg_Type_Trade._SetNpcType( pNpc->GetType() );
		break;
	case npcType::eEVENT_STORE:
		pNpc->SetType( nsCsNpcTable::NT_EVENT_STORE );		
		m_dlg_Type_Trade.ShowWindow( SW_SHOW );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		m_dlg_Type_Trade._SetNpcType( pNpc->GetType() );
		break;
	case npcType::eCAPSULE_MACHINE:
		pNpc->SetType( nsCsNpcTable::NT_CAPSULE_MACHINE );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eSKILL:
		pNpc->SetType( nsCsNpcTable::NT_SKILL );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eDATS_PORTAL:
		pNpc->SetType( nsCsNpcTable::NT_DATS_PORTAL );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::ePROPERTY_STORE:
		pNpc->SetType( nsCsNpcTable::NT_PROPERTY_STORE );
		m_dlg_Type_Trade.ShowWindow( SW_SHOW );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		m_dlg_Type_Trade._SetNpcType( pNpc->GetType() );
		break;
	case npcType::eGOTCHA_MACHINE:
		pNpc->SetType( nsCsNpcTable::NT_GOTCHA_MACHINE );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eMASTERS_MATCHING:
		pNpc->SetType( nsCsNpcTable::NT_MASTERS_MATCHING );
		m_dlg_Type_Trade.ShowWindow( SW_SHOW );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		m_dlg_Type_Trade._SetNpcType( pNpc->GetType() );
		break;
	case npcType::eMYSTERY_MACHINE:
		pNpc->SetType( nsCsNpcTable::NT_MYSTERY_MACHINE );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eSPIRIT_EVO:
		pNpc->SetType( nsCsNpcTable::NT_SPIRIT_EVO );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eSPECIAL_EVENT:
		pNpc->SetType( nsCsNpcTable::NT_SPECIAL_EVENT );
		m_dlg_Type_Trade.ShowWindow( SW_SHOW );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		m_dlg_Type_Trade._SetNpcType( pNpc->GetType() );
		m_combo_SubType.ShowWindow( SW_SHOW );
		break;
	case npcType::eITEMPRODUCTION_NPC:
		pNpc->SetType( nsCsNpcTable::NT_ITEM_PRODUCTION_NPC );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eBATTLE_REGISTRANT_NPC:
		pNpc->SetType( nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eInfiniteWar_Npc:
		pNpc->SetType( nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eInfiniteWar_NoticeBoard_Npc:
		pNpc->SetType( nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	case npcType::eExtra_Evolution_Npc:
		pNpc->SetType( nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC );
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
		m_dlg_Type_Portal.ShowWindow( SW_HIDE );
		break;
	default:
		assert_cs( false );
	}

	__WriteNpcList();
}

void CNpcToolDlg::OnCbnSelchangeComboNpctype2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;	

	switch( m_Combo_MoveType.GetCurSel() )
	{
	case nsCsNpcTable::MT_NONE:
		pNpc->SetMoveType( nsCsNpcTable::MT_NONE );
		break;
	case nsCsNpcTable::MT_MOVE:
		pNpc->SetMoveType( nsCsNpcTable::MT_MOVE );
		break;
	default:
		assert_cs( false );
		break;
	}
}

//=========================================================================================================
//
//		NPC PenName
//
//=========================================================================================================

void CNpcToolDlg::__InitNpcPenName()
{
	m_edit_NpcPenName.SetLimitText( NPC_NAME_LEN-1 );
}

void CNpcToolDlg::__ReadNpcPenName()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	m_edit_NpcPenName.SetWindowText( pNpc->GetInfo()->s_szPenName );
}

void CNpcToolDlg::OnEnChangeEditNpcpenname()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	m_edit_NpcPenName.GetWindowText( pNpc->GetInfo()->s_szPenName, NPC_NAME_LEN );
	__WriteNpcList();
}

//=========================================================================================================
//
//		NPC Name
//
//=========================================================================================================

void CNpcToolDlg::__InitNpcName()
{
	m_edit_NpcName.SetLimitText( NPC_NAME_LEN-1 );
}

void CNpcToolDlg::__ReadNpcName()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;

	m_edit_NpcName.SetWindowText( pNpc->GetInfo()->s_szName );

#ifdef GBTALK_VIEW
	g_TalkView.SetName( m_CurNpc.GetInfo()->s_szName );
#endif
}

// 이름 에디트 변경
void CNpcToolDlg::OnEnChangeEditNpcname()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	m_edit_NpcName.GetWindowText( pNpc->GetInfo()->s_szName, NPC_NAME_LEN );

#ifdef GBTALK_VIEW
	g_TalkView.SetName( m_CurNpc.GetInfo()->s_szName );
#endif

	__WriteNpcList();
}


//=========================================================================================================
//
//		NPC Model
//
//=========================================================================================================

void CNpcToolDlg::__InitNpcModel()
{
	m_edit_NpcModelID.SetLimitText( 5 );
	m_edit_NpcModelID.SetWindowText( L"0" );

	OnEnChangeEditNpcmodel();
}

void CNpcToolDlg::__ReadNpcModel()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;

	_Int2Control( &m_edit_NpcModelID, pNpc->GetInfo()->s_dwModelID );
}

// 모델 ID 변경시 호출
void CNpcToolDlg::OnEnChangeEditNpcmodel()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;

	DWORD dwModelID = _Control2Int( &m_edit_NpcModelID );
	if( nsCsFileTable::g_pDigimonMng->IsDigimon( dwModelID ) == true )
	{
		CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwModelID );
		m_edit_NpcModelName.SetWindowText( pDigimon->GetInfo()->s_szName );
	}
	else
	{
		m_edit_NpcModelName.SetWindowText( L"empty ID" );
	}
	// 정보 입력
	pNpc->GetInfo()->s_dwModelID = dwModelID;

	__WriteNpcList();
}

//=========================================================================================================
//
//		NPC Talk
//
//=========================================================================================================

void CNpcToolDlg::__InitNpcTalk()
{
	m_edit_NpcTalk.SetLimitText( NPC_TALK_LEN-1 );
}

void CNpcToolDlg::__ReadNpcTalk()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;

	m_edit_NpcTalk.SetWindowText( pNpc->GetInfo()->s_szTalk );
#ifdef GBTALK_VIEW
	g_TalkView.SetTalk( m_CurNpc.GetInfo()->s_szTalk );
#endif
}

void CNpcToolDlg::OnEnChangeEditNpctalk()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;

	m_edit_NpcTalk.GetWindowText( pNpc->GetInfo()->s_szTalk, NPC_TALK_LEN );
#ifdef GBTALK_VIEW
	g_TalkView.SetTalk( m_CurNpc.GetInfo()->s_szTalk );
#endif

	__WriteNpcList();
}

//=========================================================================================================
//
//		Zoom View
//
//=========================================================================================================

void CNpcToolDlg::__InitDispZoom()
{
	m_cmb_ZoomView.InsertString( 0, L"사용 안함" );
	m_cmb_ZoomView.InsertString( 1, L"사용" );
	m_cmb_ZoomView.SetCurSel( 1 );
}

void CNpcToolDlg::__ReadDispZoom()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	if( pNpc->GetInfo()->s_nDisplayPlag & CsNpc::DP_ZOOM_VIEW )
		m_cmb_ZoomView.SetCurSel( 1 );
	else
		m_cmb_ZoomView.SetCurSel( 0 );
}


void CNpcToolDlg::OnCbnSelchangeComboZoomview()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	if( m_cmb_ZoomView.GetCurSel() == 1 )
		pNpc->GetInfo()->s_nDisplayPlag |= CsNpc::DP_ZOOM_VIEW;
	else
		pNpc->GetInfo()->s_nDisplayPlag &= ~CsNpc::DP_ZOOM_VIEW;
}


//=========================================================================================================
//
//		Display Info
//
//=========================================================================================================

void CNpcToolDlg::__InitDispInfo()
{
	m_cmb_DispInfo.InsertString( 0, L"모두 사용 안함" );
	m_cmb_DispInfo.InsertString( 1, L"모두 사용" );
	m_cmb_DispInfo.SetCurSel( 1 );
}

void CNpcToolDlg::__ReadDispInfo()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	if( pNpc->GetInfo()->s_nDisplayPlag & CsNpc::DP_INFOMATION )
		m_cmb_DispInfo.SetCurSel( 1 );
	else
		m_cmb_DispInfo.SetCurSel( 0 );
}


void CNpcToolDlg::OnCbnSelchangeComboDispinfo()
{
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;
	if( m_cmb_DispInfo.GetCurSel() == 1 )
		pNpc->GetInfo()->s_nDisplayPlag |= CsNpc::DP_INFOMATION;
	else
		pNpc->GetInfo()->s_nDisplayPlag &= ~CsNpc::DP_INFOMATION;
}


//=========================================================================================================
//
//		Extra
//
//=========================================================================================================

void CNpcToolDlg::__InitNpcExtra()
{
	CRect rc, rcParent;
	GetClientRect( &rcParent );	
	m_dlg_Extra_Quest.Create( _Dlg_Extra_Quest::IDD, this );
	m_dlg_Extra_Quest.SetWindowPos( NULL, rcParent.left + 638 + 463, rcParent.top+125, 0, 0, SWP_NOSIZE );
	m_dlg_Extra_Quest.ShowWindow( SW_HIDE );
	m_dlg_Extra_Quest._Init();


	m_list_Extra.ShowWindow( SW_HIDE );

	m_cmb_ExtraType.InsertString( nsCsNpcTable::NE_QUEST, L"퀘스트에 의한 보이기,숨기기" );
	m_cmb_ExtraType.SetCurSel( nsCsNpcTable::NE_QUEST );
	m_cmb_ExtraType.ShowWindow( SW_HIDE );

	m_btn_ExtraAdd.ShowWindow( SW_HIDE );
	m_btn_ExtraDel.ShowWindow( SW_HIDE );
}

void CNpcToolDlg::__ReadNpcExtra()
{
	m_list_Extra.ResetContent();
	m_dlg_Extra_Quest.ShowWindow( SW_HIDE );

	// 선택된 NPC가 있을경우에
	if( m_pSelNpc != NULL )
	{
		m_list_Extra.ShowWindow( SW_SHOW );
		m_cmb_ExtraType.ShowWindow( SW_SHOW );
		m_btn_ExtraAdd.ShowWindow( SW_SHOW );
		m_btn_ExtraDel.ShowWindow( SW_SHOW );		
	}
	else
	{
		m_list_Extra.ShowWindow( SW_HIDE );
		m_cmb_ExtraType.ShowWindow( SW_HIDE );
		m_btn_ExtraAdd.ShowWindow( SW_HIDE );
		m_btn_ExtraDel.ShowWindow( SW_HIDE );		
		return;
	}

	sNPC_EXTRA_BASE** pArray = m_pSelNpc->GetExtraArray();
	for( int i=0; i<nsCsNpcTable::NE_MAX_CNT; ++i )
	{
		if( pArray[ i ] == NULL )
			continue;
		
		int nAddIndex = m_list_Extra.GetCount();
		CString strCombo;
		m_cmb_ExtraType.GetLBText( i, strCombo );
		m_list_Extra.InsertString( nAddIndex, strCombo );
		m_list_Extra.SetItemData( nAddIndex, (DWORD_PTR)pArray[ i ] );
	}
}


void CNpcToolDlg::OnBnClickedButtonExtraAdd()
{
	if( m_pSelNpc == NULL )
	{
		CsMessageBox( MB_OK, L"등록된 NPC만 추가 가능합니다." );
		return;
	}

	nsCsNpcTable::eNPC_EXTRA eSel = (nsCsNpcTable::eNPC_EXTRA)m_cmb_ExtraType.GetCurSel();

	if( m_pSelNpc->IsExtra( eSel ) == true ) 
	{
		CsMessageBox( MB_OK, L"이미 같은 타입이 추가되어 있습니다." );
		return;
	}

	m_pSelNpc->AddExtra( eSel );
	__ReadNpcExtra();
}

void CNpcToolDlg::OnBnClickedButtonExtraDel()
{
	int nSel = m_list_Extra.GetCurSel();
	if( nSel == -1 )
	{
		CsMessageBox( MB_OK, L"위 리스트에서 항목을 선택해야만 제거 가능합니다." );
		return;
	}

	if( CsMessageBox( MB_OKCANCEL, L"해당 데이터가 전부 사라집니다. 정말로 지우시겠습니까?" ) == IDCANCEL )
	{
		return;
	}

	m_pSelNpc->DeleteExtra( (sNPC_EXTRA_BASE*)m_list_Extra.GetItemData( nSel ) );
	__ReadNpcExtra();
}


void CNpcToolDlg::OnLbnSelchangeListExtra()
{
	int nSel = m_list_Extra.GetCurSel();
	if( nSel == -1 )
	{
		m_dlg_Extra_Quest.ShowWindow( SW_HIDE );
		return;
	}

	sNPC_EXTRA_BASE* pExtra = (sNPC_EXTRA_BASE*)m_list_Extra.GetItemData( nSel );

	switch( pExtra->s_eNpcExtra )
	{
	case nsCsNpcTable::NE_QUEST:
		m_dlg_Extra_Quest.ShowWindow( SW_SHOW );
		m_dlg_Extra_Quest._SetData( (sNPC_EXTRA_QUEST*)pExtra );
		break;
	default:
		assert_cs( false );
	}
}



//=========================================================================================================
//
//		Menu
//
//=========================================================================================================

// 추가 버튼 클릭
void CNpcToolDlg::OnBnClickedButtonAdd()
{
	if( m_AddNpc.GetInfo()->s_dwNpcID == 0 )
	{
		CsMessageBox( MB_OK, L"Npc ID 를 설정하여 주십시오" );
		return;
	}
	if( nsCsFileTable::g_pNpcMng->IsNpc( m_AddNpc.GetInfo()->s_dwNpcID ) == true )
	{
		CsMessageBox( MB_OK, L"셋팅된 ID = %d 는 이미 존재합니다.", m_AddNpc.GetInfo()->s_dwNpcID );
		return;
	}

	CsNpc* pAddNpc = m_AddNpc.Clone();
	nsCsFileTable::g_pNpcMng->AddNpc( pAddNpc );

	m_AddNpc.Delete();
	m_AddNpc.Init();

	__WriteNpcList( m_list_NpcList.GetItemCount(), pAddNpc );
	__ResetNpc( false );
}


void CNpcToolDlg::OnBnClickedButtonSave()
{
	if( _access_s( nsCsFileTable::GetPath_DM_Npc(), 0 ) == 0 )
	{
		char cNewFile[ MAX_PATH ];

		if( nsCsFileTable::g_eLanguage != nsCsFileTable::ENGLISH )
		{
			// 폴더가 존재 하지 않는다면 폴더 생성
			nsCSFILE::CreateDirectory( L".\\GameData\\Npc_Backup\\Korea" );

			SYSTEMTIME st;
			GetLocalTime( &st );
			sprintf_s( cNewFile, MAX_PATH, ".\\GameData\\Npc_Backup\\Korea\\Npc_%.2d%.2d%.2d_%.2d%.2d%.2d.xls", st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		}
		else
		{
			// 폴더가 존재 하지 않는다면 폴더 생성
			nsCSFILE::CreateDirectory( L".\\GameData\\Npc_Backup\\English" );

			SYSTEMTIME st;
			GetLocalTime( &st );
			sprintf_s( cNewFile, MAX_PATH, ".\\GameData\\Npc_Backup\\English\\Npc_%.2d%.2d%.2d_%.2d%.2d%.2d.xls", st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		}
		
		// 이미 파일이 존재 한다면
		if( _access_s( cNewFile, 0 ) == 0 )
			DeleteFileA( cNewFile );

		::CopyFileA( nsCsFileTable::GetPath_DM_Npc(), cNewFile, FALSE );		
	}

	nsCsFileTable::g_pNpcMng->Save_Npc();

	CsMessageBoxA( MB_OK, "%s 저장 성공!!", nsCsFileTable::GetPath_DM_Npc() );
}


void CNpcToolDlg::OnBnClickedButtonCheckStore()
{
	std::list< DWORD > DelList;

	CsNpc::MAP* pMap = nsCsFileTable::g_pNpcMng->GetNpcMap();
	CsNpc::MAP_IT it = pMap->begin();
	CsNpc::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		if( it->second->GetType()->s_eNpcType != nsCsNpcTable::NT_TRADE &&
			it->second->GetType()->s_eNpcType != nsCsNpcTable::NT_GUILD &&
			it->second->GetType()->s_eNpcType != nsCsNpcTable::NT_DIGICORE &&
			it->second->GetType()->s_eNpcType != nsCsNpcTable::NT_EVENT_STORE
			&& it->second->GetType()->s_eNpcType != nsCsNpcTable::NT_PROPERTY_STORE
			)
			continue;

		DelList.clear();

		sNPC_TYPE_TRADE* pType = (sNPC_TYPE_TRADE*)it->second->GetType();
		int nCount = pType->GetItemCount();
		for( int i=0; i<nCount; ++i )
		{
			if( nsCsFileTable::g_pItemMng->IsItem( pType->GetItemID( i ) ) == false )
			{
				DelList.push_back( pType->GetItemID( i ) );
			}
		}
		std::list< DWORD >::iterator itDel = DelList.begin();
		std::list< DWORD >::iterator itDelEnd = DelList.end();
		for( ; itDel != itDelEnd; ++itDel )
		{
			pType->DelItem( *itDel );
		}
	}

	CsMessageBox( MB_OK, L"정리 완료" );
}

void CNpcToolDlg::OnBnClickedButtonSelRelease()
{
	POSITION pos = m_list_NpcList.GetFirstSelectedItemPosition();
	int nSelNpcID = m_list_NpcList.GetNextSelectedItem( pos );
	if( nSelNpcID >= 0 )
	{
		m_list_NpcList.SetItemState( nSelNpcID, 0, LVIS_SELECTED );
	}
	__ResetNpc( false );
}




void CNpcToolDlg::OnBnClickedButtonModelinfo()
{
	_Dlg_ModelInfo dlg;
	dlg.DoModal();
}

void CNpcToolDlg::OnCbnSelchangeComboSubType()
{
	// 정보 입력
	CsNpc* pNpc = m_pSelNpc ? m_pSelNpc : &m_AddNpc;

	sNPC_TYPE_SPECIAL_EVENT* pType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetType();
	sNPC_TYPE_SPECIAL_EVENT::eSUBTYPE eSubType = (sNPC_TYPE_SPECIAL_EVENT::eSUBTYPE)m_combo_SubType.GetCurSel();

	if( eSubType == sNPC_TYPE_SPECIAL_EVENT::eSUBTYPE::EVENT_CARDGAME )
		m_dlg_Type_Trade.ShowWindow( SW_HIDE );
	else
		m_dlg_Type_Trade.ShowWindow( SW_SHOW );

	pType->SetSubType( eSubType );

	__WriteNpcList();
}
