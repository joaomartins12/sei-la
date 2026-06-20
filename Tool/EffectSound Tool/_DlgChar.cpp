// _DlgChar.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EffectSound Tool.h"
#include "_DlgChar.h"

 
// _DlgChar

IMPLEMENT_DYNCREATE(_DlgChar, CFormView)

_DlgChar::_DlgChar()
	: CFormView(_DlgChar::IDD)
{

}

_DlgChar::~_DlgChar()
{
}

void _DlgChar::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHAR, m_List_Char);
	DDX_Control(pDX, IDC_CHECK_CHRLOOK, m_Check_ChrLook);
}




// _DlgChar 진단입니다.

#ifdef _DEBUG
void _DlgChar::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void _DlgChar::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG



BEGIN_MESSAGE_MAP(_DlgChar, CFormView)	
	ON_BN_CLICKED(IDC_CHECK_CHRLOOK, &_DlgChar::OnBnClickedCheckChrlook)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CHAR, &_DlgChar::OnLvnItemchangedListChar)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &_DlgChar::OnHdnItemdblclickListChar)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_CHAR, &_DlgChar::OnLvnColumnclickListChar)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CHAR, &_DlgChar::OnNMClickListChar)
END_MESSAGE_MAP()



void _DlgChar::Init()
{
	DWORD dwExStyle = m_List_Char.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_List_Char.SetExtendedStyle( dwExStyle );
	m_List_Char.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 65 );
	m_List_Char.InsertColumn( 1, L"이름", LVCFMT_CENTER, 300 );

	TCHAR szID[ 32 ];
	TCHAR szPath[ OBJECT_PATH_LEN ];
	std::map< DWORD, CsModelData* >::iterator it = g_pModelDataMng->GetDataMap()->begin();
	std::map< DWORD, CsModelData* >::iterator itEnd = g_pModelDataMng->GetDataMap()->end();
	for( ; it!=itEnd; ++it )
	{
		int nIndex = m_List_Char.GetItemCount();

		_stprintf_s( szID, 32, L"%d", it->second->GetInfo()->s_dwID );

		M2W( szPath, it->second->GetInfo()->s_cKfmPath, OBJECT_PATH_LEN );
		TCHAR* szName = (TCHAR*)nsCSFILE::GetFileName( szPath );

		size_t nLen = _tcslen( szName );
		for( size_t i=0; i<nLen; ++i )
		{
			if( szName[ i ] == '_' )
			{
				szName = &szName[ i+1 ];
				break;
			}
		}

		m_List_Char.InsertItem( nIndex, szID );
		m_List_Char.SetItem( nIndex, 1, LVIF_TEXT, szName, 0, 0, 0, 0 );
		m_List_Char.SetItemData( nIndex, it->second->GetInfo()->s_dwID );
	}

}

void _DlgChar::OnBnClickedCheckChrlook()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void _DlgChar::OnLvnItemchangedListChar(NMHDR *pNMHDR, LRESULT *pResult)
{	
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);	
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	

	*pResult = 0;
}

void _DlgChar::OnHdnItemdblclickListChar(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	

	*pResult = 0;
}

static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CString* strItem1 = (CString*)lParam1;
	CString* strItem2 = (CString*)lParam2;
	BOOL bSort = (BOOL)lParamSort;

	int result = _tcscmp(*strItem1, *strItem2);    

	if( bSort == TRUE )
	{
		return result;
	}
	else
	{
		return -result;
	}

	return 0; 
}  


void _DlgChar::OnLvnColumnclickListChar(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int iColumn = pNMLV->iSubItem;
	if( iColumn == m_nSortColumn )
	{
		m_bSort = !m_bSort;
	}
	else
	{
		m_bSort = TRUE;
	}
	m_nSortColumn = iColumn;

	int nitem = m_List_Char.GetItemCount();
	CString** arStr =  new CString*[ nitem ];
	for( int i = 0; i < nitem; i++ )
	{	
		arStr[ i ]= new CString( m_List_Char.GetItemText( i, iColumn ) );
		m_List_Char.SetItemData( i, (LPARAM)arStr[ i ] );
	}

	m_List_Char.SortItems( CompareFunc, m_bSort );

	for(int i = 0; i < nitem; i++)
	{
		delete arStr[ i ];
	}
	delete []arStr;

	*pResult = 0;
}



void _DlgChar::OnNMClickListChar(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_List_Char.SetFocus();

	POSITION pos = m_List_Char.GetFirstSelectedItemPosition();
	int nSelect = m_List_Char.GetNextSelectedItem( pos );
	if( nSelect < 0 )
	{
		g_ViewEffect.DeleteDestModel();
		return;
	}

	int nID = _tstoi( m_List_Char.GetItemText( nSelect, 0 ) );
	g_ViewEffect.SetDestModel( nID );	

	*pResult = 0;
}
