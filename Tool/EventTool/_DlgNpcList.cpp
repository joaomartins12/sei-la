// _DlgNpcList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgNpcList.h"


// _DlgNpcList 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgNpcList, CDialog)

_DlgNpcList::_DlgNpcList(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgNpcList::IDD, pParent)
{

}

_DlgNpcList::~_DlgNpcList()
{
}

void _DlgNpcList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LISTNPC, m_NpcList);
}


BEGIN_MESSAGE_MAP(_DlgNpcList, CDialog)	
	ON_NOTIFY(NM_DBLCLK, IDC_LISTNPC, &_DlgNpcList::OnNMDblclkListnpc)
END_MESSAGE_MAP()


// _DlgNpcList 메시지 처리기입니다.


BOOL _DlgNpcList::OnInitDialog()
{	
	CDialog::OnInitDialog();	

	DWORD dwExStyle = m_NpcList.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_NpcList.SetExtendedStyle( dwExStyle );
	m_NpcList.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
	m_NpcList.InsertColumn( 1, L"이름", LVCFMT_CENTER, 140 );

	DWORD dwMapID = g_pNpcMng->GetMapID();

	TCHAR szID[ 32 ];	
	// 엔피씨 생성 - 일단은 걍 다~!!
	MAP_MAPNPC* pmapNpc = nsCsMapTable::g_pMapNpcMng->GetGroup( dwMapID )->GetNpcMap();
	MAP_MAPNPC_IT itmapNpc = pmapNpc->begin();
	MAP_MAPNPC_IT itEndmapNpc = pmapNpc->end();
	for( ;itmapNpc!=itEndmapNpc; ++itmapNpc )
	{
		DWORD dwNpcID = itmapNpc->second->GetInfo()->s_dwNpcID; 
		assert_csm1( nsCsFileTable::g_pNpcMng->IsNpc( dwNpcID ), L"Map.xls에는 존재 하지만 Npc.xls 에 %d Npc가 존재하지 않습니다.", itmapNpc->second->GetInfo()->s_dwNpcID );
		assert_cs( itmapNpc->second->GetInfo()->s_dwNpcID > NPC_FT_MASK );
		assert_cs( itmapNpc->second->GetInfo()->s_dwNpcID%NPC_FT_MASK < OBJECT_MAX_COUNT );	

		int nIndex = m_NpcList.GetItemCount();

		_stprintf_s( szID, 32, L"%d", dwNpcID );		

		m_NpcList.InsertItem( nIndex, szID );
		m_NpcList.SetItem( nIndex, 1, LVIF_TEXT, nsCsFileTable::g_pNpcMng->GetNpc( dwNpcID )->GetInfo()->s_szName, 0, 0, 0, 0 );
		m_NpcList.SetItemData( nIndex, dwNpcID );
		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgNpcList::OnNMDblclkListnpc(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_NpcList.SetFocus();

	POSITION pos = m_NpcList.GetFirstSelectedItemPosition();
	int nSelect = m_NpcList.GetNextSelectedItem( pos );	

	if( nSelect < 0 )
	{		
		return;
	}	

	g_pMainWnd->_GetDlgController()->_SetTarGet( g_pNpcMng->GetNpc( (DWORD)m_NpcList.GetItemData( nSelect ) ) );	

	OnCancel();

	*pResult = 0;
}
