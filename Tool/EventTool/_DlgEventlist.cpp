// _DlgEventlist.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgEventlist.h"


// _DlgEventlist 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgEventlist, CDialog)

_DlgEventlist::_DlgEventlist(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgEventlist::IDD, pParent)
{

}

_DlgEventlist::~_DlgEventlist()
{
}

void _DlgEventlist::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHAR, m_List_Char);	
}


BEGIN_MESSAGE_MAP(_DlgEventlist, CDialog)
	ON_NOTIFY(HDN_ITEMDBLCLICK, 0, &_DlgEventlist::OnHdnItemdblclickListChar)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CHAR, &_DlgEventlist::OnNMDblclkListChar)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_CHAR, &_DlgEventlist::OnLvnItemchangedListChar)
END_MESSAGE_MAP()

BOOL _DlgEventlist::OnInitDialog()
{	
	CDialog::OnInitDialog();

	DWORD dwExStyle = m_List_Char.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_List_Char.SetExtendedStyle( dwExStyle );
	m_List_Char.InsertColumn( 0, L"ID", LVCFMT_RIGHT, 60 );
	m_List_Char.InsertColumn( 1, L"이름", LVCFMT_CENTER, 125 );

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

	return TRUE;  // return TRUE unless you set the focus to a control
}

// _DlgEventlist 메시지 처리기입니다.

void _DlgEventlist::OnHdnItemdblclickListChar(NMHDR *pNMHDR, LRESULT *pResult)
{	
	*pResult = 0;
}

void _DlgEventlist::OnNMDblclkListChar(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_List_Char.SetFocus();

	POSITION pos = m_List_Char.GetFirstSelectedItemPosition();	
	int nSelect = m_List_Char.GetNextSelectedItem( pos );

	g_pMainWnd->_GetDlgController()->_NewEventChar( (DWORD)m_List_Char.GetItemData( nSelect ) );	 
	OnCancel();

	*pResult = 0;
}

void _DlgEventlist::OnLvnItemchangedListChar(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}
