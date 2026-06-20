// _DlgPartObject.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SequnceTool.h"
#include "_DlgPartObject.h"


// _DlgPartObject 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgPartObject, CDialog)

_DlgPartObject::_DlgPartObject(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgPartObject::IDD, pParent)
{

}

_DlgPartObject::~_DlgPartObject()
{
}

void _DlgPartObject::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree_Part);
}


BEGIN_MESSAGE_MAP(_DlgPartObject, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &_DlgPartObject::OnTvnSelchangedTreeObject)
END_MESSAGE_MAP()


// _DlgPartObject 메시지 처리기입니다.









void _DlgPartObject::_Init( char* cWorkingFolder )
{
	M2W( m_szWorkingFolder, cWorkingFolder, MAX_PATH );

	m_tree_Part.DeleteAllItems();

	HTREEITEM hRoot = m_tree_Part.InsertItem( m_szWorkingFolder, TVI_ROOT, TVI_LAST );
	
	HTREEITEM hHead = m_tree_Part.InsertItem( L"Head", hRoot );	
	HTREEITEM hUp = m_tree_Part.InsertItem( L"Up", hRoot );
	HTREEITEM hDown = m_tree_Part.InsertItem( L"Down", hRoot );
	HTREEITEM hHand = m_tree_Part.InsertItem( L"Hand", hRoot );
	HTREEITEM hShoes = m_tree_Part.InsertItem( L"Shoes", hRoot );
	HTREEITEM hCostume = m_tree_Part.InsertItem( L"costume", hRoot );

	m_tree_Part.SetItemData( hRoot, (int)-1 );
	m_tree_Part.SetItemData( hHead, (int)-1 );
	m_tree_Part.SetItemData( hUp, (int)-1 );
	m_tree_Part.SetItemData( hDown, (int)-1 );
	m_tree_Part.SetItemData( hHand, (int)-1 );
	m_tree_Part.SetItemData( hShoes, (int)-1 );
	m_tree_Part.SetItemData( hCostume, (int)-1 );
	

	TCHAR sz[ MAX_PATH ];
	TCHAR szResult[ MAX_PATH ];
	HTREEITEM hSell;
	for( int i=0; i<nsPART::MAX_CHANGE_PART_COUNT; ++i )
	{
		switch( i )
		{
		case 0:
			_stprintf_s( sz, MAX_PATH, L"%s\\Head", m_szWorkingFolder );
			hSell = hHead;
			break;
		case 1:
			_stprintf_s( sz, MAX_PATH, L"%s\\Up", m_szWorkingFolder );
			hSell = hUp;
			break;
		case 2:
			_stprintf_s( sz, MAX_PATH, L"%s\\Hand", m_szWorkingFolder );
			hSell = hHand;
			break;
		case 3:
			_stprintf_s( sz, MAX_PATH, L"%s\\Down", m_szWorkingFolder );
			hSell = hDown;
			break;		
		case 4:
			_stprintf_s( sz, MAX_PATH, L"%s\\Shoes", m_szWorkingFolder );
			hSell = hShoes;
			break;
		case 5:
			_stprintf_s( sz, MAX_PATH, L"%s\\Costume", m_szWorkingFolder );
			hSell = hCostume;
			break;
		}

		int nIndex = 0;
		while( true )
		{
			nIndex > 9 ? _stprintf_s( szResult, MAX_PATH, L"%s%d.nif", sz, nIndex ) : _stprintf_s( szResult, MAX_PATH, L"%s0%d.nif", sz, nIndex );
			if( _taccess( szResult, 0 ) == 0 )
			{
				HTREEITEM hChild = m_tree_Part.InsertItem( nsCSFILE::GetFileName( szResult ), hSell );
				m_tree_Part.SetItemData( hChild, (int)i );
				++nIndex;
			}
			else
			{
				break;
			}
		}
	}

	m_tree_Part.Expand( hRoot, TVE_EXPAND );
	m_tree_Part.Expand( hHead, TVE_EXPAND );
	m_tree_Part.Expand( hUp, TVE_EXPAND );
	m_tree_Part.Expand( hDown, TVE_EXPAND );
	m_tree_Part.Expand( hHand, TVE_EXPAND );
	m_tree_Part.Expand( hShoes, TVE_EXPAND );
	m_tree_Part.Expand( hCostume, TVE_EXPAND );
}

void _DlgPartObject::OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
	if( hSelItem == NULL )
	{		
		return;
	}

	int nPartIndex = (int)m_tree_Part.GetItemData( hSelItem );

	if( nPartIndex == -1 )
		return;

	CString str = m_tree_Part.GetItemText( hSelItem );
	g_ViewModel.ChangePart( nPartIndex, str.GetBuffer() );
}