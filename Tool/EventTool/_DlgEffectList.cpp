// _DlgEffectList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgEffectList.h"


// _DlgEffectList 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgEffectList, CDialog)

_DlgEffectList::_DlgEffectList(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgEffectList::IDD, pParent)
{

}

_DlgEffectList::~_DlgEffectList()
{
}

void _DlgEffectList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EFFECT, m_ListEffect);
}


BEGIN_MESSAGE_MAP(_DlgEffectList, CDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_EFFECT, &_DlgEffectList::OnNMDblclkListEffect)
END_MESSAGE_MAP()


// _DlgEffectList 메시지 처리기입니다.

BOOL _DlgEffectList::OnInitDialog()
{	
	CDialog::OnInitDialog();

	DWORD dwExStyle = m_ListEffect.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_ListEffect.SetExtendedStyle( dwExStyle );	
	m_ListEffect.InsertColumn( 0, L"이펙트 고유번호", LVCFMT_CENTER, 100 );
	m_ListEffect.InsertColumn( 1, L"이펙트 파일명", LVCFMT_CENTER, 350 );

	std::map< size_t, CsEffectSound* >::iterator it = g_EffectSoundMng->GetMap()->begin();
	std::map< size_t, CsEffectSound* >::iterator itEnd = g_EffectSoundMng->GetMap()->end();
	for( ; it != itEnd; it++ )
	{
		TCHAR szNum[ 64 ];
		int nIndex = m_ListEffect.GetItemCount();		

		_stprintf_s( szNum, 64, L"%u", it->second->m_sESInfo.HashCode );		
		m_ListEffect.InsertItem( nIndex, szNum );		
		m_ListEffect.SetItemData( nIndex, it->second->m_sESInfo.HashCode );
		
		CString str( it->second->m_sESInfo.szFilePath );		
		m_ListEffect.SetItem( nIndex, 1, LVIF_TEXT, str, 0, 0, 0, 0 );
	}

	return TRUE;  // return TRUE unless you set the focus to a control	
}

void _DlgEffectList::OnNMDblclkListEffect(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_ListEffect.SetFocus();

	POSITION pos = m_ListEffect.GetFirstSelectedItemPosition();
	int nSelect = m_ListEffect.GetNextSelectedItem( pos );
	if( nSelect < 0 )
		return;		
 
	g_pMainWnd->_GetDlgController()->_SetEffectEvent( (DWORD)m_ListEffect.GetItemData( nSelect ) );	

	OnOK();

	*pResult = 0;
}
