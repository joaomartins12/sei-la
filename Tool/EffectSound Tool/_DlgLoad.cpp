// _DlgLoad.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EffectSound Tool.h"
#include "_DlgLoad.h"


// _DlgLoad 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgLoad, CDialog)

_DlgLoad::_DlgLoad(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgLoad::IDD, pParent)
{

}

_DlgLoad::~_DlgLoad()
{
}

void _DlgLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOAD, m_ListLoad);
}


BEGIN_MESSAGE_MAP(_DlgLoad, CDialog)
//	ON_WM_LBUTTONDBLCLK()
ON_WM_LBUTTONDOWN()
ON_NOTIFY(NM_DBLCLK, IDC_LIST_LOAD, &_DlgLoad::OnNMDblclkListLoad)
ON_NOTIFY(NM_RCLICK, IDC_LIST_LOAD, &_DlgLoad::OnNMRclickListLoad)
END_MESSAGE_MAP()


// _DlgLoad 메시지 처리기입니다.

BOOL _DlgLoad::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	DWORD dwExStyle = m_ListLoad.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_ListLoad.SetExtendedStyle( dwExStyle );	
	m_ListLoad.InsertColumn( 0, L"이펙트 고유번호", LVCFMT_CENTER, 100 );
	m_ListLoad.InsertColumn( 1, L"이펙트 파일명", LVCFMT_CENTER, 350 );

	std::map< size_t, CsEffectSound* >::iterator it = g_EffectSoundMng->GetMap()->begin();
	std::map< size_t, CsEffectSound* >::iterator itEnd = g_EffectSoundMng->GetMap()->end();
	for( ; it != itEnd; it++ )
	{
		TCHAR szNum[ 64 ];
		int nIndex = m_ListLoad.GetItemCount();		

		_stprintf_s( szNum, 64, L"%u", it->second->m_sESInfo.HashCode );		
		m_ListLoad.InsertItem( nIndex, szNum );		
		m_ListLoad.SetItemData( nIndex, it->second->m_sESInfo.HashCode );

		CString str( it->second->m_sESInfo.szFilePath );		
		m_ListLoad.SetItem( nIndex, 1, LVIF_TEXT, str, 0, 0, 0, 0 );
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



void _DlgLoad::OnNMDblclkListLoad(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ListLoad.SetFocus();

	POSITION pos = m_ListLoad.GetFirstSelectedItemPosition();
	int nSelect = m_ListLoad.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;		

	g_ViewEffect.LoadEffect( (DWORD)m_ListLoad.GetItemData( nSelect ) );	

	OnOK();	

	*pResult = 0;
}

void _DlgLoad::OnNMRclickListLoad(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ListLoad.SetFocus();

	POSITION pos = m_ListLoad.GetFirstSelectedItemPosition();
	int nSelect = m_ListLoad.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;	

	if( CsMessageBox( MB_YESNO, L"해당 정보를 삭제하나요?" ) == IDYES )
	{
		std::map< size_t, CsEffectSound* >::iterator it = g_EffectSoundMng->GetMap()->begin();
		std::map< size_t, CsEffectSound* >::iterator itEnd = g_EffectSoundMng->GetMap()->end();
		for( ; it != itEnd; it++ )
		{
			if( it->second->m_sESInfo.HashCode == (DWORD)m_ListLoad.GetItemData( nSelect ) )
			{
				SAFE_NIDELETE( it->second );
				g_EffectSoundMng->GetMap()->erase( it );									
				break;
			}
		}
		g_EffectSoundMng->SaveFile();
	}

	OnOK();

	*pResult = 0;
}
