// _DlgModal_Find.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgModal_Find.h"


// _DlgModal_Find 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgModal_Find, CDialog)

_DlgModal_Find::_DlgModal_Find(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgModal_Find::IDD, pParent)
{

}

_DlgModal_Find::~_DlgModal_Find()
{
}

void _DlgModal_Find::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FIND_TYPE, m_combo_FindType);
	DDX_Control(pDX, IDC_EDIT_FINDSTR, m_edit_FindStr);
	DDX_Control(pDX, IDC_LIST_FIND, m_list_Find);
}


BEGIN_MESSAGE_MAP(_DlgModal_Find, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DOFIND, &_DlgModal_Find::OnBnClickedButtonDofind)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FIND, &_DlgModal_Find::OnNMDblclkListFind)
END_MESSAGE_MAP()


// _DlgModal_Find 메시지 처리기입니다.



BOOL _DlgModal_Find::OnInitDialog()
{
	CDialog::OnInitDialog();

	DWORD dwExStyle = m_list_Find.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Find.SetExtendedStyle( dwExStyle );
	m_list_Find.InsertColumn( 0, L"ID", LVCFMT_LEFT, 60 );
	m_list_Find.InsertColumn( 1, L"Title", LVCFMT_LEFT, 250 );

	m_combo_FindType.InsertString( -1, L"전체" );
	m_combo_FindType.InsertString( -1, L"제목" );
	m_combo_FindType.InsertString( -1, L"본문" );
	m_combo_FindType.InsertString( -1, L"도우미" );
	m_combo_FindType.InsertString( -1, L"시작 타겟" );
	m_combo_FindType.InsertString( -1, L"완료 타겟" );

	m_combo_FindType.SetCurSel( 0 );

	SetWindowText( L"검색창" );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

BOOL _DlgModal_Find::PreTranslateMessage(MSG* pMsg)
{
	if (WM_KEYDOWN == pMsg->message)
	{
		if ( VK_RETURN == pMsg->wParam)
		{
			if( GetFocus()->GetSafeHwnd() == m_edit_FindStr.GetSafeHwnd() )
			{
				OnBnClickedButtonDofind();
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void _DlgModal_Find::OnBnClickedButtonDofind()
{
	SetWindowText( L"검색 중입니다. 잠시 기다려 주십시오" );

	m_list_Find.DeleteAllItems();

	TCHAR sz[ MAX_PATH ];
	m_edit_FindStr.GetWindowText( sz, MAX_PATH );

	if( sz[ 0 ] == NULL )
		return;

	int nFindType = m_combo_FindType.GetCurSel();
	CsQuest::MAP* pMap = nsCsFileTable::g_pQuestMng->GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	CsQuest::MAP_IT itEnd = pMap->end();

	CsQuest* pQuest;
	for( ; it!=itEnd; ++it )
	{
		pQuest = it->second;

		switch( nFindType )
		{
		case 0:
			{
				if( __FIndTitleText( pQuest, sz ) ||
					__FIndBody( pQuest, sz ) ||
					__FIndHelper( pQuest, sz ) ||
					__FIndStartTarget( pQuest, sz ) ||
					__FIndCompTarget( pQuest, sz )		)
				{
					__InsertList( pQuest );
				}
			}
			break;
		case 1:
			{
				if( __FIndTitleText( pQuest, sz ) == true )
					__InsertList( pQuest );
			}
			break;
		case 2:
			{
				if( __FIndBody( pQuest, sz ) == true )
					__InsertList( pQuest );
			}
			break;
		case 3:
			{
				if( __FIndHelper( pQuest, sz ) == true )
					__InsertList( pQuest );
			}
			break;
		case 4:
			{
				if( __FIndStartTarget( pQuest, sz ) == true )
					__InsertList( pQuest );
			}
			break;
		case 5:
			{
				if( __FIndCompTarget( pQuest, sz ) == true )
					__InsertList( pQuest );
			}
			break;
		default:
			assert_cs( false );
		}
	}

	SetWindowText( L"검색 완료" );
}

bool _DlgModal_Find::__FIndTitleText( CsQuest* pQuest, TCHAR* sz )
{
	TCHAR* pFind = _tcsstr( pQuest->m_szTitleText, sz );
	if( pFind != NULL )
	{
		return true;
	}
	return false;
}

bool _DlgModal_Find::__FIndBody( CsQuest* pQuest, TCHAR* sz )
{
	TCHAR* pFind = _tcsstr( pQuest->m_szBodyText, sz );
	if( pFind != NULL )
	{
		return true;
	}
	return false;
}

bool _DlgModal_Find::__FIndHelper( CsQuest* pQuest, TCHAR* sz )
{
	TCHAR* pFind = _tcsstr( pQuest->m_szHelperText, sz );
	if( pFind != NULL )
	{
		return true;
	}
	return false;
}

bool _DlgModal_Find::__FIndStartTarget( CsQuest* pQuest, TCHAR* sz )
{
	TCHAR* pFind = NULL;
	switch( pQuest->GetStartTarget_Type() )
	{
	case CsQuest::ST_NPC:
		{
			if( nsCsFileTable::g_pNpcMng->IsNpc( pQuest->GetStartTarget_ID() ) )
			{
				TCHAR* szName = nsCsFileTable::g_pNpcMng->GetNpc( pQuest->GetStartTarget_ID() )->GetInfo()->s_szName;
				pFind = (TCHAR*)_tcsstr( szName, sz );
			}			
		}
		break;
	case CsQuest::ST_DIGIVICE:
		{
			pFind = (TCHAR*)_tcsstr( L"디지바이스", sz );
		}
		break;
	default:
		assert_cs( false );
	}

	if( pFind != NULL )
	{
		return true;
	}
	return false;
}

bool _DlgModal_Find::__FIndCompTarget( CsQuest* pQuest, TCHAR* sz )
{
	TCHAR* pFind = NULL;
	switch( pQuest->GetQuestTarget() )
	{
	case CsQuest::QTG_SELF:
		{
			return __FIndStartTarget( pQuest, sz );
		}
		break;
	case CsQuest::QTG_NPC:
		{
			if( nsCsFileTable::g_pNpcMng->IsNpc( pQuest->GetQuestTargetValue1() ) )
			{
				TCHAR* szName = nsCsFileTable::g_pNpcMng->GetNpc( pQuest->GetQuestTargetValue1() )->GetInfo()->s_szName;
				pFind = (TCHAR*)_tcsstr( szName, sz );
			}			
		}
		break;
	case CsQuest::QTG_COMPLETE:
		{
			pFind = (TCHAR*)_tcsstr( L"디지바이스", sz );
		}
		break;
	default:
		assert_cs( false );
	}

	if( pFind != NULL )
	{
		return true;
	}
	return false;
}

void _DlgModal_Find::__InsertList( CsQuest* pQuest )
{
	TCHAR szMsg[ 256 ];
	int nListIndex = m_list_Find.GetItemCount();

	// ID
	_stprintf_s( szMsg, 256, L"%d", pQuest->GetUniqID() );
	m_list_Find.InsertItem( nListIndex, szMsg );

	// 제목
	m_list_Find.SetItem( nListIndex, 1, LVIF_TEXT, pQuest->m_szTitleText, 0, 0, 0, 0 );

	m_list_Find.SetItemData( nListIndex, pQuest->GetUniqID() );	
}

void _DlgModal_Find::OnNMDblclkListFind(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_Find.GetFirstSelectedItemPosition();
	int nSel = m_list_Find.GetNextSelectedItem( pos );
	if( nSel < 0 )
		return;

	cQuestObj* pSelObject = g_pViewer->_GetQuestMng()->FindNode( (DWORD)m_list_Find.GetItemData( nSel ) );
	g_pDiscript->_SetSelQuest( pSelObject );
	g_pTop->_SetViewState( WinViewer::VS_VIEWER );
	g_pViewer->_SetSelectCenter();

	*pResult = 0;
}
