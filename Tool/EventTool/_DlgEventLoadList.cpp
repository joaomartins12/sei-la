// _DlgEventLoadList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgEventLoadList.h"


// _DlgEventLoadList 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgEventLoadList, CDialog)

_DlgEventLoadList::_DlgEventLoadList(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgEventLoadList::IDD, pParent)
{

}

_DlgEventLoadList::~_DlgEventLoadList()
{
}

void _DlgEventLoadList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTLOAD, m_List_LoadEvent);
	DDX_Control(pDX, IDC_SCENEMAKE, m_Button_SceneMake);
}


BEGIN_MESSAGE_MAP(_DlgEventLoadList, CDialog)	
	ON_NOTIFY(NM_DBLCLK, IDC_LISTLOAD, &_DlgEventLoadList::OnNMDblclkListload)
	ON_NOTIFY(NM_RDBLCLK, IDC_LISTLOAD, &_DlgEventLoadList::OnNMRdblclkListload)
	ON_BN_CLICKED(IDC_SCENEMAKE, &_DlgEventLoadList::OnBnClickedScenemake)
END_MESSAGE_MAP()


// _DlgEventLoadList 메시지 처리기입니다.

void _DlgEventLoadList::Init( int nType )
{
	assert_cs( nType < 2 ); 
	m_nType = nType; 
}

BOOL _DlgEventLoadList::OnInitDialog()
{	
	CDialog::OnInitDialog();

	DWORD dwExStyle = m_List_LoadEvent.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_List_LoadEvent.SetExtendedStyle( dwExStyle );	
	m_List_LoadEvent.InsertColumn( 0, L"이벤트번호", LVCFMT_CENTER, 80 );	
	m_List_LoadEvent.InsertColumn( 1, L"타이틀", LVCFMT_CENTER, 350 );
	m_List_LoadEvent.InsertColumn( 2, L"타입", LVCFMT_CENTER, 80 );	
	m_List_LoadEvent.InsertColumn( 3, L"총시간", LVCFMT_CENTER, 80 );	
	m_List_LoadEvent.InsertColumn( 4, L"행동수", LVCFMT_CENTER, 80 );

	TCHAR szID[ 32 ];

	std::map< DWORD, CEventScene::MAP_EvntData >::iterator itMap = g_EventScene.m_mapTempData.find( g_Terrain.m_dwMapId );
	if( itMap != g_EventScene.m_mapTempData.end() )
	{
		if( itMap->second.size() > 0 )
		{
			std::map< UINT, CEventData* >::iterator it = itMap->second.begin();
			std::map< UINT, CEventData* >::iterator itEnd = itMap->second.end();

			CEventData* pEvent;			 		
			for( ; it != itEnd; it++ )
			{
				pEvent = it->second;
				assert_cs( pEvent != NULL );

				int nIndex = m_List_LoadEvent.GetItemCount();			

				_stprintf_s( szID, 32, L"%u", pEvent->GetEventInfo()->s_nEventNum );
				m_List_LoadEvent.InsertItem( nIndex, szID );		
				m_List_LoadEvent.SetItemData( nIndex, pEvent->GetEventInfo()->s_nEventNum );			

				m_List_LoadEvent.SetItem( nIndex, 1, LVIF_TEXT, pEvent->GetEventInfo()->s_strTitle, 0, 0, 0, 0 );

				switch( pEvent->GetObjectInfo()->s_sType )
				{
				case CEventData::EO_New:
					m_List_LoadEvent.SetItem( nIndex, 2, LVIF_TEXT, L"New", 0, 0, 0, 0 );
					break;
				case CEventData::EO_Npc:
					m_List_LoadEvent.SetItem( nIndex, 2, LVIF_TEXT, L"NPC", 0, 0, 0, 0 );
					break;
				default:
					assert_cs( false );
					break;
				}				

				_stprintf_s( szID, 32, L"%.2f", pEvent->GetEventInfo()->s_fEventTime );
				m_List_LoadEvent.SetItem( nIndex, 3, LVIF_TEXT, szID, 0, 0, 0, 0 );

				_stprintf_s( szID, 32, L"%d", pEvent->GetEventSize() );
				m_List_LoadEvent.SetItem( nIndex, 4, LVIF_TEXT, szID, 0, 0, 0, 0 );				

			}	
		}	
	}

	return TRUE;  // return TRUE unless you set the focus to a control	
}

void _DlgEventLoadList::OnNMDblclkListload(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_List_LoadEvent.SetFocus();

	if( m_nType == 0 )
	{
		POSITION pos = m_List_LoadEvent.GetFirstSelectedItemPosition();
		int nSelect = m_List_LoadEvent.GetNextSelectedItem( pos );
		if( nSelect < 0 )
			return;	

		CString Str = m_List_LoadEvent.GetItemText( nSelect, 1 );
		int nIdx = (UINT)_tstoi(Str.GetBuffer(Str.GetLength()));
		g_pMainWnd->_GetDlgController()->_SetEventModify( (DWORD)m_List_LoadEvent.GetItemData( nSelect ) );				

		OnOK();
	}

	*pResult = 0;
}

void _DlgEventLoadList::OnNMRdblclkListload(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//if( m_nType == 0 )
	//{
	//	m_List_LoadEvent.SetFocus();

	//	POSITION pos = m_List_LoadEvent.GetFirstSelectedItemPosition();
	//	int nSelect = m_List_LoadEvent.GetNextSelectedItem( pos );
	//	if( nSelect < 0 )
	//		return;	

	//	if( CsMessageBox( MB_YESNO, L"삭제할까요?" ) == IDNO )
	//	{
	//		return;
	//	}

	//	g_EventScene.DeleteTempEvent( (UINT)m_List_LoadEvent.GetItemData( nSelect ) );
	//	m_List_LoadEvent.DeleteItem( nSelect );

	//}
	//
	*pResult = 0;
}


void _DlgEventLoadList::OnBnClickedScenemake()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if( m_nType == 1)
	{
		std::map< DWORD, CEventScene::MAP_EvntData >::iterator it = g_EventScene.m_mapTempData.find( g_Terrain.m_dwMapId );
		if( it == g_EventScene.m_mapTempData.end() )
		{
			CsMessageBox( MB_OK, L"해당맵에 만들어져있는 이벤트가 없네요.." );
			return;
		}

		std::stack< int > stackSceneMake;
		POSITION pos = m_List_LoadEvent.GetFirstSelectedItemPosition();
		while( pos )
		{
			stackSceneMake.push( m_List_LoadEvent.GetNextSelectedItem( pos ) );
		}

		while( stackSceneMake.size() != 0 )
		{
			int nIndex = stackSceneMake.top();
			stackSceneMake.pop();			

			CEventScene::MAP_EvntData_IT itData = it->second.find( (DWORD)m_List_LoadEvent.GetItemData( nIndex ) );
			CEventData* pEvent = itData->second;	

			g_EventScene.EventSetComplete( pEvent );
		}

		OnOK();
	}		
}
