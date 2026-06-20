// WinRBottom.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ObjectMove.h"
#include "WinRBottom.h"



WinRBottom*		g_pWinRBottom = NULL;


// WinRBottom

IMPLEMENT_DYNCREATE(WinRBottom, CFormView)

WinRBottom::WinRBottom()
	: CFormView(WinRBottom::IDD)
{

}

WinRBottom::~WinRBottom()
{
}

void WinRBottom::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MAPLIST, m_cmp_MapList);
}

BEGIN_MESSAGE_MAP(WinRBottom, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_MAPLOAD, &WinRBottom::OnBnClickedButtonMapload)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &WinRBottom::OnBnClickedButtonSave)
	ON_BN_CLICKED(IDC_BUTTON_EXCELSAVE, &WinRBottom::OnBnClickedButtonExcelsave)
END_MESSAGE_MAP()


// WinRBottom 진단입니다.

#ifdef _DEBUG
void WinRBottom::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void WinRBottom::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG



void WinRBottom::_Init()
{
	__InitComboMapList();
}

void WinRBottom::__InitComboMapList()
{
	TCHAR sz[ MAX_PATH ];
	CsMapList::MAP* pMap = nsCsMapTable::g_pMapListMng->GetListMap();
	CsMapList::MAP_IT it = pMap->begin();
	CsMapList::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		_stprintf_s( sz, MAX_PATH, L"(%d) %s", it->second->GetInfo()->s_dwMapID, it->second->GetInfo()->s_szMapDiscript.c_str() );
		int nIndex = m_cmp_MapList.GetCount();
		m_cmp_MapList.InsertString( nIndex, sz );
		m_cmp_MapList.SetItemData( nIndex, it->second->GetInfo()->s_dwMapID );
	}
	m_cmp_MapList.SetCurSel( 0 );
}

void WinRBottom::OnBnClickedButtonMapload()
{
	int nSel = m_cmp_MapList.GetCurSel();
	DWORD dwMapID = (DWORD)m_cmp_MapList.GetItemData( nSel );	

	if( nsCsGBTerrain::g_pCurRoot )
	{
		if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID == dwMapID )
			return;

		if( CsMessageBox( MB_OKCANCEL, L"저장되지 않은 정보는 삭제 됩니다. 계속 로드 하시겠습니까?" ) == IDCANCEL )
			return;
	}
	g_pProc3D->GetMap()->LoadMap( dwMapID );
}

// WinRBottom 메시지 처리기입니다.

void WinRBottom::OnSize(UINT nType, int cx, int cy)
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


void WinRBottom::OnBnClickedButtonSave()
{
	if( nsCsGBTerrain::g_pCurRoot == NULL )
		return;

	nsCsFileTable::g_pMoveObjectMng->Save();
	CsMessageBox( MB_OK, L"저장 성공" );
}
void WinRBottom::OnBnClickedButtonExcelsave()
{
	if( nsCsGBTerrain::g_pCurRoot == NULL )
		return;

	nsCsFileTable::g_pMoveObjectMng->ExcelSave();
	CsMessageBox( MB_OK, L"저장 성공" );
}
