// _DlgObjectList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgObjectList.h"


// _DlgObjectList 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgObjectList, CDialog)

_DlgObjectList::_DlgObjectList(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgObjectList::IDD, pParent)
{

}

_DlgObjectList::~_DlgObjectList()
{
}

void _DlgObjectList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OBJECT, m_list_Object);
	DDX_Control(pDX, IDC_OBJECT_CNT, mObjectCnt);
}


BEGIN_MESSAGE_MAP(_DlgObjectList, CDialog)
	ON_NOTIFY(NM_CLICK, IDC_LIST_OBJECT, &_DlgObjectList::OnNMClickListObject)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_OBJECT, &_DlgObjectList::OnNMDBClickListObject)	
	ON_BN_CLICKED(IDC_BUTTON_RESET, &_DlgObjectList::OnBnClickedButtonReset)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_OBJECT, &_DlgObjectList::OnLvnColumnclickListObject)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// _DlgObjectList 메시지 처리기입니다.

BOOL _DlgObjectList::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	DWORD dwExStyle = m_list_Object.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Object.SetExtendedStyle( dwExStyle );

	m_list_Object.InsertColumn( 0, L"Name", LVCFMT_LEFT, 100 );
	m_list_Object.InsertColumn( 1, L"Path", LVCFMT_LEFT, 100 );
	m_list_Object.InsertColumn( 2, L"Tbp", LVCFMT_CENTER, 45 );
	m_list_Object.InsertColumn( 3, L"Ani", LVCFMT_CENTER, 45 );
	m_list_Object.InsertColumn( 4, L"AwAni", LVCFMT_CENTER, 45 );
	m_list_Object.InsertColumn( 5, L"Dm", LVCFMT_CENTER, 45 );
	m_list_Object.InsertColumn( 6, L"Ef", LVCFMT_CENTER, 45 );
	m_list_Object.InsertColumn( 7, L"Potal", LVCFMT_CENTER, 45 );
	m_list_Object.InsertColumn( 8, L"Alpha", LVCFMT_CENTER, 45 );
	m_list_Object.InsertColumn( 9, L"NoPick", LVCFMT_CENTER, 45 );
	m_list_Object.InsertColumn( 10, L"ActorMng", LVCFMT_CENTER, 45 );

	return TRUE;
}

void _DlgObjectList::_PostViewReset()
{
	_ResetList();
}

void _DlgObjectList::_ResetList()
{
	m_list_Object.DeleteAllItems();
	
	if( CAMERA_STPTR )
		CAMERA_STPTR->RestOcclusionGeometry();

	{
		MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetMap_No_Pick();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			__InsertList( it->second );
		}
	}

	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	int nTotalNodeCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;	
	for( int n=0; n<nTotalNodeCount; ++n )
	{
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( n );
		MAP_GBTERRAIN_OBJECT* pMap = pLeaf->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;

			if( it->second->GetParentID() != 0 )
				continue;

			__InsertList( it->second );

			if( it->second )
			{
				if( CAMERA_STPTR )
					CAMERA_ST.AddOcclusionGeometry( it->second->GetNiNode() );
			}
		}
	}
	wchar_t text[256];
	wsprintf(text,L"MaxObjCnt : [%d]", m_list_Object.GetItemCount() );

	mObjectCnt.SetWindowText(text);
}

void _DlgObjectList::OnNMClickListObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	g_ObjectMng._CancelAllSelectModel();

	POSITION pos = m_list_Object.GetFirstSelectedItemPosition();
	while( pos )
	{
		int nItem = m_list_Object.GetNextSelectedItem( pos );
		CsGBObject* pObject = (CsGBObject*)m_list_Object.GetItemData( nItem );
		if( g_ObjectMng.TerrainObjectToSelectModel( pObject->GetInfo()->s_dwUniqID ) == NULL )
		{
			CsMessageBox( MB_OK, L"선택한 오브젝트가 존재 하지 않습니다.\n\n리 셋 버튼을 눌러서 초기화 해 주세요." );
		}
	}

	*pResult = 0;
}

void _DlgObjectList::OnNMDBClickListObject( NMHDR *pNMHDR, LRESULT *pResult )
{
	POSITION pos = m_list_Object.GetFirstSelectedItemPosition();
	int nItem = m_list_Object.GetNextSelectedItem( pos );
	CsGBObject* pObject = (CsGBObject*)m_list_Object.GetItemData( nItem );
	if(pObject)
	{
		GET_MV_VIEW->ChangeCameraPos( pObject->GetInfo()->s_trObject );
		*pResult = 0;
	}
}

void _DlgObjectList::OnBnClickedButtonReset()
{
	_ResetList();
}

void _DlgObjectList::OnLvnColumnclickListObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	int nColume = pNMLV->iSubItem;
	static bool bIncrease = true;

	// 선택된 것이 있는가
	std::list< CsGBObject* > listSelect;
	POSITION pos = m_list_Object.GetFirstSelectedItemPosition();
	while( pos )
	{
		int nIndex = m_list_Object.GetNextSelectedItem( pos );
		listSelect.push_back( (CsGBObject*)m_list_Object.GetItemData( nIndex ) );
	}

	__Sort( nColume, bIncrease );
	bIncrease = !bIncrease;

	// 선택 된거 재 선택
	std::list< CsGBObject* >::iterator it = listSelect.begin();
	std::list< CsGBObject* >::iterator itEnd = listSelect.end();
	for( ; it!=itEnd; ++it )
	{
		int nCount = m_list_Object.GetItemCount();
		for( int c=0; c<nCount; ++c )
		{
			if( *it == (CsGBObject*)m_list_Object.GetItemData( c ) )
			{
				m_list_Object.SetItemState( c, LVIS_SELECTED, LVIS_SELECTED );
				break;
			}
		}
	}
	
	*pResult = 0;
}

void _DlgObjectList::__Sort(int nColume, bool bIncrease)
{
	m_list_Object.DeleteAllItems();

	std::list< CsGBObject* > listSort;

	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	int nTotalNodeCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;

	{
		MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetMap_No_Pick();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			listSort.push_back( it->second );
		}
	}

	for( int n=0; n<nTotalNodeCount; ++n )
	{
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( n );
		MAP_GBTERRAIN_OBJECT* pMap = pLeaf->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		MAP_GBTERRAIN_OBJECT_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;

			if( it->second->GetParentID() != 0 )
				continue;

			listSort.push_back( it->second );
		}
	}
	
	{
		LIST_OBJMODEL* pModelList = g_ObjectMng.GetListSelectModel();
		LIST_OBJMODEL_IT it = pModelList->begin();
		LIST_OBJMODEL_IT itEnd = pModelList->end();
		for( ; it!=itEnd; ++it )
		{
			listSort.push_back( nsCsGBTerrain::g_ObjectResMgr.GetObject( ( *it )->GetObjectInfo() ) );
		}
	}
	

	while( listSort.size() != 0 )
	{
		std::list< CsGBObject* >::iterator itInput = listSort.begin();
		std::list< CsGBObject* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( nColume )
			{
				// Name
			case 0:
				{
					const char* cIt = nsCSFILE::GetFileName( (*it)->GetInfo()->s_cObjectPath );
					const char* cInput = nsCSFILE::GetFileName( (*itInput)->GetInfo()->s_cObjectPath );
					if( ( strcmp( cIt, cInput ) < 0 ) == bIncrease )
					{
						itInput = it;
					}
				}				
				break;
				// path
			case 1:				
				if( ( strcmp( (*it)->GetInfo()->s_cObjectPath, (*itInput)->GetInfo()->s_cObjectPath ) < 0 ) == bIncrease )
				{
					itInput = it;
				}
				break;
				// tbp
			case 2:
				{
					if( ( (*it)->UD_IsTerrainBasePick()||(*it)->UD_IsTerrainHidePick() ) ==
						bIncrease )
						itInput = it;
				}
				break;
				// Ani
			case 3:
				{
					if( (*it)->UD_IsAnimation() == bIncrease )
						itInput = it;
				}
				break;
				// AwAni
			case 4:
				{
					if( (*it)->UD_IsAlwaysAnimation() == bIncrease )
						itInput = it;
				}
				break;
				// DM
			case 5:
				{
					if( (*it)->UD_IsDarkMap() == bIncrease )
						itInput = it;
				}
				break;
				// Ef
			case 6:
				{
					if( (*it)->UD_IsEffectRender() == bIncrease )
						itInput = it;
				}
				break;
				// Portal
			case 7:
				{
					if( (*it)->UD_IsPortal() == bIncrease )
						itInput = it;
				}
				break;
				// Alpha
			case 8:
				{
					if( nsCSGBFUNC::IsAlphaBlend( (*it)->GetNiNode() ) == bIncrease )
						itInput = it;
				}
				break;
				// NoPick
			case 9:
				{
					if( (*it)->UD_IsNoPick() == bIncrease )
						itInput = it;
				}
				break;
				// ActorObj
			case 10:
				{
					if( (*it)->UD_IsActorManager() == bIncrease )
						itInput = it;
				}
				break;
			default:
				assert_cs( false );
			}
		}
		__InsertList( (*itInput ) );
		listSort.erase( itInput );
	}
}

void _DlgObjectList::__InsertList( CsGBObject* pObject )
{
	TCHAR szFullPath[ OBJECT_PATH_LEN ];

	int nIndex = m_list_Object.GetItemCount();
	M2W( szFullPath, pObject->GetInfo()->s_cObjectPath, OBJECT_PATH_LEN );
	LPCTSTR szName = nsCSFILE::GetFileName( szFullPath );
	m_list_Object.InsertItem( nIndex, szName );
	m_list_Object.SetItem( nIndex, 1, LVIF_TEXT, szFullPath, 0, 0, 0, 0 );

	if( ( pObject->UD_IsTerrainBasePick() )||( pObject->UD_IsTerrainHidePick() ) )
		m_list_Object.SetItem( nIndex, 2, LVIF_TEXT, L"O", 0, 0, 0, 0 );
	else
		m_list_Object.SetItem( nIndex, 2, LVIF_TEXT, L"X", 0, 0, 0, 0 );
	m_list_Object.SetItem( nIndex, 3, LVIF_TEXT, pObject->UD_IsAnimation() == true ? L"O" : L"X", 0, 0, 0, 0 );
	m_list_Object.SetItem( nIndex, 4, LVIF_TEXT, pObject->UD_IsAlwaysAnimation() == true ? L"O" : L"X", 0, 0, 0, 0 );
	m_list_Object.SetItem( nIndex, 5, LVIF_TEXT, pObject->UD_IsDarkMap() == true ? L"O" : L"X", 0, 0, 0, 0 );
	m_list_Object.SetItem( nIndex, 6, LVIF_TEXT, pObject->UD_IsEffectRender() == true ? L"O" : L"X", 0, 0, 0, 0 );
	m_list_Object.SetItem( nIndex, 7, LVIF_TEXT, pObject->UD_IsPortal() == true ? L"O" : L"X", 0, 0, 0, 0 );
	m_list_Object.SetItem( nIndex, 8, LVIF_TEXT, nsCSGBFUNC::IsAlphaBlend( pObject->GetNiNode() ) == true ? L"O" : L"X", 0, 0, 0, 0 );
	m_list_Object.SetItem( nIndex, 9, LVIF_TEXT, pObject->UD_IsNoPick() == true ? L"O" : L"X", 0, 0, 0, 0 );
	m_list_Object.SetItem( nIndex, 10, LVIF_TEXT, pObject->UD_IsActorManager() == true ? L"O" : L"X", 0, 0, 0, 0 );
	m_list_Object.SetItemData( nIndex, (DWORD_PTR)pObject );
}

void _DlgObjectList::_UnselectList()
{
	POSITION pos = m_list_Object.GetFirstSelectedItemPosition();
	while( pos )
	{
		int nItem = m_list_Object.GetNextSelectedItem( pos );
		m_list_Object.SetItemState( nItem, 0, LVIS_SELECTED );
	}
}

void _DlgObjectList::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);
	
}
