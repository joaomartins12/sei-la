// _DlgSceneLoadList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgSceneLoadList.h"


// _DlgSceneLoadList 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgSceneLoadList, CDialog)

_DlgSceneLoadList::_DlgSceneLoadList(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgSceneLoadList::IDD, pParent)
{

}

_DlgSceneLoadList::~_DlgSceneLoadList()
{
}

void _DlgSceneLoadList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTSCENELOAD, m_ListSceneLoad);
}


BEGIN_MESSAGE_MAP(_DlgSceneLoadList, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LISTSCENELOAD, &_DlgSceneLoadList::OnNMDblclkListsceneload)
END_MESSAGE_MAP()


// _DlgSceneLoadList 메시지 처리기입니다.

BOOL _DlgSceneLoadList::OnInitDialog()
{	
	CDialog::OnInitDialog();

	DWORD dwExStyle = m_ListSceneLoad.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_ListSceneLoad.SetExtendedStyle( dwExStyle );	
	m_ListSceneLoad.InsertColumn( 0, L"씬번호", LVCFMT_CENTER, 80 );
	m_ListSceneLoad.InsertColumn( 1, L"이벤트 대상수", LVCFMT_CENTER, 100 );
	m_ListSceneLoad.InsertColumn( 2, L"맵번호", LVCFMT_CENTER, 80 );
	
	std::map< UINT, CEventSceneData* >::iterator it = g_pEventDataMng->GetMap()->begin();
	std::map< UINT, CEventSceneData* >::iterator itEnd = g_pEventDataMng->GetMap()->end();

	for( ;it != itEnd; it++)
	{
		TCHAR szID[ 32 ];
		
		int nIndex = m_ListSceneLoad.GetItemCount();		

		_stprintf_s( szID, 32, L"%d", it->second->GetSceneNum() );
		m_ListSceneLoad.InsertItem( nIndex, szID );		
		m_ListSceneLoad.SetItemData( nIndex, it->second->GetSceneNum() );	

		_stprintf_s( szID, 32, L"%d", it->second->GetEventSize() ); 
		m_ListSceneLoad.SetItem( nIndex, 1, LVIF_TEXT, szID, 0, 0, 0, 0 );

		_stprintf_s( szID, 32, L"%d", it->second->GetCondition()->s_dwMapID ); 
		m_ListSceneLoad.SetItem( nIndex, 2, LVIF_TEXT, szID, 0, 0, 0, 0 );

	}	

	return TRUE;  // return TRUE unless you set the focus to a control	
}

void _DlgSceneLoadList::OnNMDblclkListsceneload(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ListSceneLoad.SetFocus();

	POSITION pos = m_ListSceneLoad.GetFirstSelectedItemPosition();
	int nSelect = m_ListSceneLoad.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;		

	switch( m_nType )
	{
	case 0: //플레이
		{
			CString str;
			str = m_ListSceneLoad.GetItemText( nSelect, 2 );
			int nMapID = _tstoi(str.GetBuffer(str.GetLength()));

			if( g_Terrain.m_dwMapId == nMapID )
			{
				g_pMainWnd->_GetDlgController()->_SetSceneNum( (DWORD)m_ListSceneLoad.GetItemData( nSelect ) );	
				OnOK();
			}	
			else
			{
				CsMessageBox( MB_OK, L"현재맵에서 볼수 없는 이벤트 씬 입니다." );	
				OnCancel();
			}
		}
		break;
	case 1: //삭제
		{
			if( CsMessageBox( MB_YESNO, L"삭제할까요?" ) == IDYES )
			{
				g_pEventDataMng->DeleteData( (DWORD)m_ListSceneLoad.GetItemData( nSelect ) );
				g_pEventDataMng->SaveFile();
				OnOK();
			}			
		}
		break;
	default:
		assert_cs( false );
		break;
	}
	
	*pResult = 0;
}
