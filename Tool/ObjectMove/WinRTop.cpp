// WinRTop.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ObjectMove.h"
#include "WinRTop.h"


WinRTop*		g_pWinRTop = NULL;

// WinRTop

IMPLEMENT_DYNCREATE(WinRTop, CFormView)

WinRTop::WinRTop()
	: CFormView(WinRTop::IDD)
{

}

WinRTop::~WinRTop()
{
}

void WinRTop::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_OBJECT, m_list_Object);
	DDX_Control(pDX, IDC_LIST_OBJECT2, m_list_Object2);
}

BEGIN_MESSAGE_MAP(WinRTop, CFormView)
	ON_WM_SIZE()	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_OBJECT, &WinRTop::OnLvnItemchangedListObject)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_OBJECT2, &WinRTop::OnLvnItemchangedListObject2)
END_MESSAGE_MAP()


// WinRTop 진단입니다.

#ifdef _DEBUG
void WinRTop::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void WinRTop::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// WinRTop 메시지 처리기입니다.

void WinRTop::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if( this == g_pProc3DWin )
	{
		g_ptScreen = CsPoint( cx, cy );
		if( g_pRenderer )
		{		
			g_pRenderer->Recreate( cx, cy );		
		}
	}
}

void WinRTop::_Init()
{
	DWORD dwExStyle = m_list_Object.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Object.SetExtendedStyle( dwExStyle );
	m_list_Object.InsertColumn( 0, L"npcID", LVCFMT_LEFT, 50 );
	m_list_Object.InsertColumn( 1, L"name", LVCFMT_LEFT, 140 );	

	DWORD dwExStyle2 = m_list_Object2.GetExtendedStyle();
	dwExStyle2 |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Object2.SetExtendedStyle( dwExStyle2 );
	m_list_Object2.InsertColumn( 0, L"monID", LVCFMT_LEFT, 50 );
	m_list_Object2.InsertColumn( 1, L"name", LVCFMT_LEFT, 140 );	
}

void WinRTop::_ResetMap( DWORD dwMapID )
{
	m_list_Object.DeleteAllItems();

	TCHAR szMsg[ 1024 ];
	CsNpc::MAP* pMap = nsCsFileTable::g_pNpcMng->GetGroup( dwMapID )->GetNpcMap();
	CsNpc::MAP_IT it = pMap->begin();
	CsNpc::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		CsNpc::sINFO* pInfoFT = it->second->GetInfo();
		if( pInfoFT->s_eMoveType != nsCsNpcTable::MT_MOVE )
			continue;		

		int nListIndex = m_list_Object.GetItemCount();
		_stprintf_s( szMsg, 1024, L"%d", pInfoFT->s_dwNpcID );
		m_list_Object.InsertItem( nListIndex, szMsg );
		m_list_Object.SetItem( nListIndex, 1, LVIF_TEXT, pInfoFT->s_szName, 0, 0, 0, 0 );
		m_list_Object.SetItemData( nListIndex, pInfoFT->s_dwNpcID );
	}	

	{
		m_list_Object2.DeleteAllItems();

		TCHAR szMsg[ 1024 ];
		CsMapMonster::LIST* pMapMonster = nsCsMapTable::g_pMapMonsterMng->GetGroup( dwMapID )->GetMonsterList();
		CsMapMonster::LIST_IT it = pMapMonster->begin();
		for( ; it!=pMapMonster->end(); ++it )
		{
			CsMapMonster::sINFO* pInfoFT = (*it)->GetInfo();
			if(CsMapMonster::MT_MOVE != pInfoFT->s_nMoveType)
				continue;

			CsMonster* pMonster = nsCsFileTable::g_pMonsterMng->GetMonster(pInfoFT->s_dwMonsterTableID );

			//if( pInfoFT->s_eMoveType != nsCsNpcTable::MT_MOVE )
			//	continue;		

			int nListIndex = m_list_Object2.GetItemCount();
			_stprintf_s( szMsg, 1024, L"%d", pMonster->GetInfo()->s_dwMonsterID );
			m_list_Object2.InsertItem( nListIndex, szMsg );
			m_list_Object2.SetItem( nListIndex, 1, LVIF_TEXT, pMonster->GetInfo()->s_szName, 0, 0, 0, 0 );
			m_list_Object2.SetItemData( nListIndex, pMonster->GetInfo()->s_dwMonsterID );
			
		}
	}
}

void WinRTop::OnLvnItemchangedListObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	POSITION pos = m_list_Object.GetFirstSelectedItemPosition();
	int nSel = m_list_Object.GetNextSelectedItem( pos );
	if( nSel < 0 )
		return;

	DWORD dwNpcID = (DWORD)m_list_Object.GetItemData( nSel );
	CsC_AvObject* pMoveObject = g_pProc3D->GetMap()->GetMoveMng()->LoadNpc( dwNpcID );
	g_pWinLBottom->_SetTarget_Npc( pMoveObject );

	*pResult = 0;
}

void WinRTop::OnLvnItemchangedListObject2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	POSITION pos = m_list_Object2.GetFirstSelectedItemPosition();
	int nSel = m_list_Object2.GetNextSelectedItem( pos );
	if( nSel < 0 )
		return;

	DWORD dwMonID = (DWORD)m_list_Object2.GetItemData( nSel );
	CsC_AvObject* pMoveObject = g_pProc3D->GetMap()->GetMoveMng()->LoadMonster( dwMonID );
	g_pWinLBottom->_SetTarget_Npc( pMoveObject );
	*pResult = 0;
}
