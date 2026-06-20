// _DlgEventTalkList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "EventTool.h"
#include "_DlgEventTalkList.h"


// _DlgEventTalkList 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgEventTalkList, CDialog)

_DlgEventTalkList::_DlgEventTalkList(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgEventTalkList::IDD, pParent)
{

}

_DlgEventTalkList::~_DlgEventTalkList()
{
}

void _DlgEventTalkList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TALKLIST, m_EventTalkList);
}


BEGIN_MESSAGE_MAP(_DlgEventTalkList, CDialog)
	ON_LBN_DBLCLK(IDC_LIST_TALKLIST, &_DlgEventTalkList::OnLbnDblclkListTalklist)
END_MESSAGE_MAP()

BOOL _DlgEventTalkList::OnInitDialog()
{	
	CDialog::OnInitDialog();	

	CsTalk_Event::MAP* pTalkList = nsCsFileTable::g_pTalkMng->GetMap_Event();	
	CsTalk_Event::MAP_IT it = pTalkList->begin();
	
	for( ; it != pTalkList->end(); ++it )
	{
		if( it->second->GetInfo()->s_szText[ 0 ] == NULL )
			continue;

		TCHAR szID[ 32 ];	
		_stprintf_s( szID, 32, L"%d : ", it->second->GetInfo()->s_dwTalkNum );		

		CString str( szID );
		str += it->second->GetInfo()->s_szText;

		m_EventTalkList.InsertString( -1, str );		
		m_EventTalkList.SetItemData( m_EventTalkList.GetCount() - 1, it->second->GetInfo()->s_dwTalkNum );

	}

	return TRUE;  // return TRUE unless you set the focus to a control
}


// _DlgEventTalkList 메시지 처리기입니다.

void _DlgEventTalkList::OnLbnDblclkListTalklist()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int nIdx = m_EventTalkList.GetCurSel();
	DWORD dwID = (DWORD)m_EventTalkList.GetItemData( nIdx );

	if( nIdx < 0 )
		return;

	g_pMainWnd->_GetDlgController()->_SetTalkEvent( dwID );
	OnCancel();
	
}
