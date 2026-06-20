// _DlgMapList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgMapList.h"


// _DlgMapList 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMapList, CDialog)

_DlgMapList::_DlgMapList(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgMapList::IDD, pParent)
{
	
}

_DlgMapList::~_DlgMapList()
{
}

void _DlgMapList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_LIST_MAP, m_MapList);
}


BEGIN_MESSAGE_MAP(_DlgMapList, CDialog)	
	ON_LBN_DBLCLK(IDC_LIST_MAP, &_DlgMapList::OnLbnDblclkListMap)
END_MESSAGE_MAP()

BOOL _DlgMapList::OnInitDialog()
{	
	CDialog::OnInitDialog();
	
	CsMapList::MAP* pmapList = nsCsMapTable::g_pMapListMng->GetListMap();
	CsMapList::MAP_IT it = pmapList->begin();
	for( ; it!=pmapList->end(); ++it )
	{
		if( it->second->GetInfo()->s_cMapPath[ 0 ] == NULL )
			continue;
			
		CString str(it->second->GetInfo()->s_cMapName);
		m_MapList.InsertString( -1, str );		
		m_MapList.SetItemData( m_MapList.GetCount() - 1, it->second->GetInfo()->s_dwMapID );
		
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}


// _DlgMapList 메시지 처리기입니다.

void _DlgMapList::OnLbnDblclkListMap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIdx = m_MapList.GetCurSel();
	DWORD dwMapID = (DWORD)m_MapList.GetItemData( nIdx );
	g_pNpcMng->NpcReset();
	g_EventScene.DeleteCameraTarget();
	g_pMainWnd->_GetDlgController()->_ResetAll();
	g_pMainWnd->_GetDlgController()->_SetChrCamera(false);		
	
	if( g_pTerrainMng->CreateRoot_ByTool( dwMapID ) )
	{		
		g_pNpcMng->LoadMap( dwMapID );

		g_Terrain.m_dwMapId = dwMapID;		
		g_EventScene.ResetFreeCamera();		

		OnCancel();
	}
}
