// _DlgMTObj_Merge.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTObj_Merge.h"


// _DlgMTObj_Merge 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTObj_Merge, CDialog)

_DlgMTObj_Merge::_DlgMTObj_Merge(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTObj_Merge::IDD, pParent)
{
	m_bReadingList = false;
}

_DlgMTObj_Merge::~_DlgMTObj_Merge()
{
}

void _DlgMTObj_Merge::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MERGE, m_list_Merge);
}


BEGIN_MESSAGE_MAP(_DlgMTObj_Merge, CDialog)
	ON_WM_SHOWWINDOW()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MERGE, &_DlgMTObj_Merge::OnLvnItemchangedListMerge)
END_MESSAGE_MAP()



//=====================================================================================
//
//		Base
//
//=====================================================================================

BOOL _DlgMTObj_Merge::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	DWORD dwExStyle = m_list_Merge.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_CHECKBOXES;
	m_list_Merge.SetExtendedStyle( dwExStyle );

	m_list_Merge.InsertColumn( 0, L"Index", LVCFMT_LEFT, 50 );
	m_list_Merge.InsertColumn( 1, L"이름", LVCFMT_LEFT, 350 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void _DlgMTObj_Merge::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		m_pCsGBObject = GET_MV_MAINTAB_DLG->_GetDlgObejct()->_GetUniqCsGBObject();
		__ReadListMerge();
	}	
}

//=====================================================================================
//
//		List Control
//
//=====================================================================================

void _DlgMTObj_Merge::__ReadListMerge()
{
	m_bReadingList = true;

	m_list_Merge.DeleteAllItems();

	TCHAR szList[ MAX_PATH ];
	sUDD_Merge* pUDD = (sUDD_Merge*)m_pCsGBObject->GetUserDefine()->GetData( sUDD_Merge::Type() );
	std::map< DWORD, sUDD_Merge::sINFO* >::iterator it = pUDD->s_mapInfo.begin();
	for( ; it!=pUDD->s_mapInfo.end(); ++it )
	{
		int nListIndex = it->second->s_nSlotID;
		_stprintf_s( szList, MAX_PATH, L"%d", it->second->s_nSlotID );
		m_list_Merge.InsertItem( nListIndex, szList );
				
		M2W( szList, it->second->s_cFileName, MAX_PATH );
		m_list_Merge.SetItem( nListIndex, 1, LVIF_TEXT, nsCSFILE::GetFileName( szList ), 0, 0, 0, 0  );

		cObjectModel* pModel = *g_ObjectMng.GetListSelectModel()->begin();
		cObjectModel* pChildModel = pModel->GetChild( it->second->s_nSlotID );

		if( pChildModel->GetObjectState() != cObjectModel::OS_DISABLE )
			m_list_Merge.SetCheck( nListIndex, TRUE );
	}

	m_bReadingList = false;
}

void _DlgMTObj_Merge::OnLvnItemchangedListMerge(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if( ( m_bReadingList == false )&&( pNMLV->uNewState != pNMLV->uOldState ) )
	{
		CPoint p( m_list_Merge.GetCurrentMessage()->pt );
		CPoint pt( p );
		m_list_Merge.ScreenToClient( &pt );
		int nItemIndex = m_list_Merge.HitTest( pt );

		if( ( pNMLV->uNewState == 0x00001000 )&&( pNMLV->uOldState == 0x00002000 ) )
		{
			assert_cs( g_ObjectMng.GetListSelectModel()->size() == 1 );
			cObjectModel* pObject = ( *g_ObjectMng.GetListSelectModel()->begin() );
			cObjectModel* pChild = pObject->GetChild( nItemIndex );
			// 현재거 디스에이블
			pChild->SetObjectState( cObjectModel::OS_DISABLE );			
		}
		else if( ( pNMLV->uNewState == 0x00002000 )&&( pNMLV->uOldState == 0x00001000 ) )
		{
			assert_cs( g_ObjectMng.GetListSelectModel()->size() == 1 );
			cObjectModel* pObject = ( *g_ObjectMng.GetListSelectModel()->begin() );
			cObjectModel* pChild = pObject->GetChild( nItemIndex );

			bool bResist = nsCsGBTerrain::g_ObjectResMgr.IsObject( pChild->GetObjectInfo()->s_dwUniqID );
			pChild->SetObjectState( bResist == true ? cObjectModel::OS_RESIST : cObjectModel::OS_READY );
		}
	}
	*pResult = 0;
}
