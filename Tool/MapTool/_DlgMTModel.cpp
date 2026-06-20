// _DlgMTModel.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTModel.h"



// _DlgMTModel 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTModel, CDialog)

_DlgMTModel::_DlgMTModel(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTModel::IDD, pParent)
{

}

_DlgMTModel::~_DlgMTModel()
{
}


void _DlgMTModel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_NPCLIST, m_list_NpcList);
}

BEGIN_MESSAGE_MAP(_DlgMTModel, CDialog)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(NM_CLICK, IDC_LIST_NPCLIST, &_DlgMTModel::OnNMClickListNpclist)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_NPCLIST, &_DlgMTModel::OnNMDBClickListNpclist)
END_MESSAGE_MAP()


// ==================================================================================
//
//		virtual
//
// ==================================================================================


void _DlgMTModel::_Delete()
{
}

void _DlgMTModel::_ViewInit()
{	
}

void _DlgMTModel::_PostViewReset()
{
	
}

void _DlgMTModel::_PreSave( const char* cFile, bool bAutoSave )
{
	/*if( g_Config.GetEnvironment()->s_bWriteExcel == false )
		return;*/

	m_DlgPortal._PreSave( cFile, bAutoSave );
	g_ModelMng.PreSave( cFile, bAutoSave );
}

void _DlgMTModel::_UpdateView()
{
	m_DlgPortal._UpdateView();
}



// ==================================================================================
//
//		Base
//
// ==================================================================================
BOOL _DlgMTModel::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	CRect rc;
	GetWindowRect( &rc );

	m_DlgPortal.Create( _DlgMTModel_Portal::IDD, this );
	m_DlgPortal.SetWindowPos( NULL, 0, 0, rc.Width(), ( rc.Height() - 250 ), 0 );
	m_DlgPortal.ShowWindow( SW_HIDE );

	DWORD dwExStyle = m_list_NpcList.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_NpcList.SetExtendedStyle( dwExStyle );
	m_list_NpcList.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
	m_list_NpcList.InsertColumn( 1, L"이름", LVCFMT_LEFT, 120 );
	m_list_NpcList.InsertColumn( 2, L"타입", LVCFMT_LEFT, 120 );
	m_list_NpcList.InsertColumn( 3, L"배치", LVCFMT_CENTER, 40 );

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgMTModel::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		m_DlgPortal.ShowWindow( SW_HIDE );
		__ResetMapNpcList();
		//m_DlgNpc.ShowWindow( SW_SHOW );				
		
		OnNMClickListNpclist( NULL, NULL );
	}
	else
	{
		m_DlgPortal.ShowWindow( SW_HIDE );

		g_Preview.GetChar()->ReleaseModelView();
		g_ModelMng.HideModelTab();
		//m_DlgNpc.ShowWindow( SW_HIDE );
	}
}

void _DlgMTModel::__CheckDlg( DWORD dwNpcID )
{
	if( g_TempRoot.IsLoad() == true )
		return;

	m_DlgPortal.ShowWindow( SW_HIDE );

	int nCount = m_list_NpcList.GetItemCount();
	for( int c=0; c<nCount; ++c )
	{
		if( dwNpcID == (DWORD)m_list_NpcList.GetItemData( c ) )
		{
			if( g_ModelMng.IsResistNpc( dwNpcID ) == false )
				continue;

			if( nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID )->GetInfo()->s_eType == nsCsNpcTable::NT_PORTAL )
			{
				m_DlgPortal.ShowWindow( SW_SHOW );
				m_DlgPortal._SetNpc( nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID ) );
			}
			return;
		}
	}
}

void _DlgMTModel::__CheckResist( DWORD dwNpcID, int x, int y )
{
	if( g_TempRoot.IsLoad() == true )
		return;

	CsNpc* pNpc = nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID );
	if( pNpc->GetInfo()->s_eType != nsCsNpcTable::NT_PORTAL )
		return;

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	CsMapPortal::sINFO info;
	info.Init();
	sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)pNpc->GetType();
	assert_cs( pType->s_nPortalCount > 0 );
	for( int i=0; i<pType->s_nPortalCount; ++i )
	{
		DWORD dwPortalID = g_ObjectMng.PopPortalTypeID();
		info.s_dwPortalID = PORTAL_TO_MAPPORTAL( dwMapID, dwPortalID );
		assert_cs( info.s_dwPortalID > PORTAL_MIN_LIMIT );
		info.s_dwSrcMapID = dwMapID;

		info.s_ePortalType = CsMapPortal::NPC;
		info.s_dwUniqObjectID = dwNpcID;

		info.s_nPortalKindIndex = i;
		info.s_nSrcRadius = 2000;
		info.s_nSrcTargetX = x;
		info.s_nSrcTargetY = y;

		info.s_dwDestMapID = 0;

		nsCsMapTable::g_pMapPortalMng->AddPortal( &info, pType->GetPortal( i ) );
	}
}

void _DlgMTModel::__CheckUnResist( DWORD dwNpcID )
{
	if( g_TempRoot.IsLoad() == true )
		return;

	CsNpc* pNpc = nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID );
	if( pNpc->GetInfo()->s_eType != nsCsNpcTable::NT_PORTAL )
		return;

	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)pNpc->GetType();
	assert_cs( pType->s_nPortalCount > 0 );
	for( int i=0; i<pType->s_nPortalCount; ++i )
	{
		DWORD dwPortalID = nsCsMapTable::g_pMapPortalMng->GetGroup( dwMapID )->GetNpcPortal_ByObjectID( dwNpcID, i )->GetInfo()->s_dwPortalID;
		g_ObjectMng.PushPortalTypeID( dwPortalID );
		nsCsMapTable::g_pMapPortalMng->GetGroup( dwMapID )->DeleteNpcPortal_ByObjectID( dwNpcID, i );		
	}
}

// ==================================================================================
//
//		Input
//
// ==================================================================================


void _DlgMTModel::_KeyDownEscape()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	g_Preview.GetChar()->ReleaseModelView();
	if( g_ModelMng.GetSelectNpc() != NULL )
	{
		g_ModelMng.SelectNpcToResistNpc();		
	}
	if( g_ModelMng.GetModelNpc() != NULL )
	{
		g_ModelMng.DeleteModelNpc();
	}

	_CancelMapNpcList();
}

//=========================================================================================
//
//		Npc List
//
//=========================================================================================

void _DlgMTModel::__ResetMapNpcList()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	static int nOldMapID = -1;
	if( nOldMapID == nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID )
		return;
	nOldMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;



	m_list_NpcList.DeleteAllItems();

	TCHAR szMsg[ 512 ];
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	CsNpc::MAP* pMap = nsCsFileTable::g_pNpcMng->GetGroup( dwMapID )->GetNpcMap();
	CsNpc::MAP_IT it = pMap->begin();
	for( ;it!=pMap->end(); ++it )
	{
		SAFE_POINTER_CON(it->second);
		CsNpc::sINFO* pInfo = it->second->GetInfo();
		SAFE_POINTER_CON(pInfo);

		int nListIndex = m_list_NpcList.GetItemCount();
		_stprintf_s( szMsg, 512, L"%d", pInfo->s_dwNpcID );
		m_list_NpcList.InsertItem( nListIndex, szMsg );
		m_list_NpcList.SetItem( nListIndex, 1, LVIF_TEXT, pInfo->s_szName, 0, 0, 0, 0 );

		std::wstring wsType;
		switch( pInfo->s_eType )
		{
		case nsCsNpcTable::NT_NONE:							wsType = L" x ";				break;
		case nsCsNpcTable::NT_TRADE:						wsType = L"상인";				break;
		case nsCsNpcTable::NT_DIGITAMA_TRADE:				wsType = L"디지타마상인";		break;
		case nsCsNpcTable::NT_PORTAL:						wsType = L"포탈 도우미";		break;
		case nsCsNpcTable::NT_MAKE_TACTICS:					wsType = L"용병 생성";			break;
		case nsCsNpcTable::NT_ELEMENT_ITEM:					wsType = L"속성 상인";			break;
		case nsCsNpcTable::NT_WAREHOUSE:					wsType = L"창고";				break;
		case nsCsNpcTable::NT_TACTICSHOUSE:					wsType = L"용병보관소";			break;
		case nsCsNpcTable::NT_DIGICORE:						wsType = L"디지코어상인";		break;
		case nsCsNpcTable::NT_CAPSULE_MACHINE:				wsType = L"캡슐머신";			break;
		case nsCsNpcTable::NT_SKILL:						wsType = L"스킬상인";			break;
		case nsCsNpcTable::NT_EVENT_STORE:					wsType = L"이벤트 상점";		break;
		case nsCsNpcTable::NT_DATS_PORTAL:					wsType = L"닷츠 포탈";			break;	
		case nsCsNpcTable::NT_GUILD:						wsType = L"커뮤니티 상점";		break;
		case nsCsNpcTable::NT_PROPERTY_STORE:				wsType = L"속성 상점 상인";		break;
		case nsCsNpcTable::NT_GOTCHA_MACHINE:				wsType = L"레어머신";			break;
		case nsCsNpcTable::NT_MASTERS_MATCHING:				wsType = L"마스터즈 매치";		break;
		case nsCsNpcTable::NT_MYSTERY_MACHINE:				wsType = L"수수께끼머신";		break;
		case nsCsNpcTable::NT_SPIRIT_EVO:					wsType = L"스피릿 진화";		break;
		case nsCsNpcTable::NT_SPECIAL_EVENT:				wsType = L"이벤트";				break;
		case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:			wsType = L"아이템 제작";		break;
		case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:		wsType = L"배틀 등록";			break;
		case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:		wsType = L"무한 대전 진행";		break;
		case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:	wsType = L"무한 대전 게시판";	break;
		case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:			wsType = L"진화 상점 NPC";		break;
		default:		wsType = L"맵툴에서속성정의안됨";			break;
		}

		if( nsCsNpcTable::MT_MOVE == pInfo->s_eMoveType )
			wsType += L", 이동";

		m_list_NpcList.SetItem( nListIndex, 2, LVIF_TEXT, wsType.c_str(), 0, 0, 0, 0  );		

		if( g_ModelMng.IsResistNpc( it->second->GetInfo()->s_dwNpcID ) == true )
			m_list_NpcList.SetItem( nListIndex, 3, LVIF_TEXT, L"0", 0, 0, 0, 0 );
		m_list_NpcList.SetItemData( nListIndex, it->second->GetInfo()->s_dwNpcID );
	}
}

void _DlgMTModel::_CancelMapNpcList()
{
	if( g_TempRoot.IsLoad() == true )
		return;

	__CheckDlg( 0 );

	POSITION pos = m_list_NpcList.GetFirstSelectedItemPosition();
	int nSel = m_list_NpcList.GetNextSelectedItem( pos );
	if( nSel != -1 )
	{
		m_list_NpcList.SetItemState( nSel, 0, LVIS_SELECTED );
	}
}

void _DlgMTModel::_SetMapNpcList( DWORD dwNpcID )
{
	if( g_TempRoot.IsLoad() == true )
		return;

	__CheckDlg( dwNpcID );

	int nCount = m_list_NpcList.GetItemCount();
	for( int c=0; c<nCount; ++c )
	{
		if( dwNpcID == (DWORD)m_list_NpcList.GetItemData( c ) )
		{
			m_list_NpcList.SetItemState( c, LVIS_SELECTED, LVIS_SELECTED );
			g_Preview.GetChar()->SetModelView( nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID )->GetInfo()->s_dwModelID );			

			return;
		}
	}
	assert_cs( false );
}

void _DlgMTModel::_ResistNpcList( DWORD dwNpcID, int x, int y )
{
	if( g_TempRoot.IsLoad() == true )
		return;

	int nCount = m_list_NpcList.GetItemCount();
	for( int c=0; c<nCount; ++c )
	{
		if( dwNpcID == (DWORD)m_list_NpcList.GetItemData( c ) )
		{
			m_list_NpcList.SetItem( c, 3, LVIF_TEXT, L"0", 0, 0, 0, 0 );
			__CheckResist( dwNpcID, x, y );
			return;
		}
	}
	assert_cs( false );
}

void _DlgMTModel::_UnresistNpcList( DWORD dwNpcID )
{
	if( g_TempRoot.IsLoad() == true )
		return;

	int nCount = m_list_NpcList.GetItemCount();
	for( int c=0; c<nCount; ++c )
	{
		if( dwNpcID == (DWORD)m_list_NpcList.GetItemData( c ) )
		{
			m_list_NpcList.SetItem( c, 3, LVIF_TEXT, L"", 0, 0, 0, 0 );
			__CheckUnResist( dwNpcID );
			_CancelMapNpcList();
			return;
		}
	}
	assert_cs( false );
}

// 리스트 변경
void _DlgMTModel::OnNMClickListNpclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( g_TempRoot.IsLoad() == true )
		return;

	POSITION pos = m_list_NpcList.GetFirstSelectedItemPosition();
	int nItem = m_list_NpcList.GetNextSelectedItem( pos );

	g_Preview.GetChar()->ReleaseModelView();	
	g_ModelMng.SelectNpcToResistNpc();	
	if( nItem >= 0 )
	{
		DWORD dwNpcID = (DWORD)m_list_NpcList.GetItemData( nItem );

		__CheckDlg( dwNpcID );

		CsNpc* pNpc = nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID );
		g_Preview.GetChar()->SetModelView( pNpc->GetInfo()->s_dwModelID );
		g_ModelMng.SetModelView( dwNpcID );
	}
	else
	{
		__CheckDlg( 0 );
	}
}

void _DlgMTModel::OnNMDBClickListNpclist(NMHDR *pNMHDR, LRESULT *pResult)
{
	CNpc* pSelNpc = g_ModelMng.GetSelectNpc();
	if( NULL == pSelNpc )
		return;
	
	GET_MV_VIEW->ChangeCameraPos( pSelNpc->GetCsNode()->m_pNiNode->GetWorldTransform() );
	*pResult = 0;
}

