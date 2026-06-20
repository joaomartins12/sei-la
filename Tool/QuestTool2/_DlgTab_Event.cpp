// _DlgTab_Event.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgTab_Event.h"


// _DlgTab_Event 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgTab_Event, CDialog)

_DlgTab_Event::_DlgTab_Event(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgTab_Event::IDD, pParent)
{

}

_DlgTab_Event::~_DlgTab_Event()
{
}

void _DlgTab_Event::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TITLE, m_edit_Title);
	DDX_Control(pDX, IDC_EDIT_QS_EI, m_edit_QS_EventID);
	DDX_Control(pDX, IDC_EDIT_QS_ET, m_edit_QS_EventID_Discript);
	DDX_Control(pDX, IDC_BUTTON_QS_EI, m_btn_QS_FT);
	DDX_Control(pDX, IDC_EDIT_QP_EI, m_edit_QP_EventID);
	DDX_Control(pDX, IDC_BUTTON_QP_EI, m_btn_QP_FT);
	DDX_Control(pDX, IDC_EDIT_QP_ET, m_edit_QP_EventID_Discript);
	DDX_Control(pDX, IDC_EDIT_QC_EI, m_edit_QC_EventID);
	DDX_Control(pDX, IDC_BUTTON_QC_EI, m_btn_QC_FT);
	DDX_Control(pDX, IDC_EDIT_QC_ET, m_edit_QC_EventID_Discript);
	DDX_Control(pDX, IDC_EDIT_QR_EI, m_edit_QR_EventID);
	DDX_Control(pDX, IDC_BUTTON_QR_EI, m_btn_QR_FT);
	DDX_Control(pDX, IDC_EDIT_QR_ET, m_edit_QR_EventID_Discript);
}


BEGIN_MESSAGE_MAP(_DlgTab_Event, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_QS_EI, &_DlgTab_Event::OnBnClickedButtonQsEi)
	ON_EN_CHANGE(IDC_EDIT_QS_EI, &_DlgTab_Event::OnEnChangeEditQsEi)
	ON_EN_CHANGE(IDC_EDIT_QP_EI, &_DlgTab_Event::OnEnChangeEditQpEi)
	ON_BN_CLICKED(IDC_BUTTON_QP_EI, &_DlgTab_Event::OnBnClickedButtonQpEi)
	ON_EN_CHANGE(IDC_EDIT_QC_EI, &_DlgTab_Event::OnEnChangeEditQcEi)
	ON_BN_CLICKED(IDC_BUTTON_QC_EI, &_DlgTab_Event::OnBnClickedButtonQcEi)
	ON_EN_CHANGE(IDC_EDIT_QR_EI, &_DlgTab_Event::OnEnChangeEditQrEi)
	ON_BN_CLICKED(IDC_BUTTON_QR_EI, &_DlgTab_Event::OnBnClickedButtonQrEi)
END_MESSAGE_MAP()



void _DlgTab_Event::_Init()
{
	m_edit_Title.SetLimitText( QUEST_TITLE_LEN - 1 );	
}

void _DlgTab_Event::_SetQuest( cQuestObj* pQuestObj )
{
	if( pQuestObj == NULL )
	{
		// 제목
		m_edit_Title.ShowWindow( SW_HIDE );

		// 퀘스트 시작
		m_edit_QS_EventID.ShowWindow( SW_HIDE );
		m_edit_QS_EventID_Discript.ShowWindow( SW_HIDE );
		m_btn_QS_FT.ShowWindow( SW_HIDE );

		// 퀘스트 진행중
		m_edit_QP_EventID.ShowWindow( SW_HIDE );
		m_edit_QP_EventID_Discript.ShowWindow( SW_HIDE );
		m_btn_QP_FT.ShowWindow( SW_HIDE );

		// 퀘스트 완료 시점
		m_edit_QC_EventID.ShowWindow( SW_HIDE );
		m_edit_QC_EventID_Discript.ShowWindow( SW_HIDE );
		m_btn_QC_FT.ShowWindow( SW_HIDE );

		// 퀘스트 보상
		m_edit_QR_EventID.ShowWindow( SW_HIDE );
		m_edit_QR_EventID_Discript.ShowWindow( SW_HIDE );
		m_btn_QR_FT.ShowWindow( SW_HIDE );
	}
	else
	{
		CsQuest* pFTQuest = pQuestObj->GetFT();

		// 퀘스트 제목
		m_edit_Title.ShowWindow( SW_SHOW );
		m_edit_Title.SetWindowText( pFTQuest->m_szTitleText );

		// 퀘스트 시작
		m_edit_QS_EventID.ShowWindow( SW_SHOW );
		m_edit_QS_EventID_Discript.ShowWindow( SW_SHOW );
		m_btn_QS_FT.ShowWindow( SW_SHOW );
		UINT nEventSceneID = pFTQuest->m_nEvent[ CsQuest::EVENT_QUEST_START ];		
		if( g_pEventDataMng->IsData( nEventSceneID ) )
		{
			nsCsMfcFunc::Int2Control( &m_edit_QS_EventID, nEventSceneID );
		}
		else
		{
			nsCsMfcFunc::Int2Control( &m_edit_QS_EventID, 0 );
		}

		// 퀘스트 진행중
		m_edit_QP_EventID.ShowWindow( SW_SHOW );
		m_edit_QP_EventID_Discript.ShowWindow( SW_SHOW );
		m_btn_QP_FT.ShowWindow( SW_SHOW );
		nEventSceneID = pFTQuest->m_nEvent[ CsQuest::EVENT_QUEST_PROC ];		
		if( g_pEventDataMng->IsData( nEventSceneID ) )
		{
			nsCsMfcFunc::Int2Control( &m_edit_QP_EventID, nEventSceneID );
		}
		else
		{
			nsCsMfcFunc::Int2Control( &m_edit_QP_EventID, 0 );
		}

		// 퀘스트 완료 시점
		m_edit_QC_EventID.ShowWindow( SW_SHOW );
		m_edit_QC_EventID_Discript.ShowWindow( SW_SHOW );
		m_btn_QC_FT.ShowWindow( SW_SHOW );
		nEventSceneID = pFTQuest->m_nEvent[ CsQuest::EVENT_QUEST_COMPLEAT ];
		if( g_pEventDataMng->IsData( nEventSceneID ) )
		{
			nsCsMfcFunc::Int2Control( &m_edit_QC_EventID, nEventSceneID );
		}
		else
		{
			nsCsMfcFunc::Int2Control( &m_edit_QC_EventID, 0 );
		}

		// 퀘스트 보상
		m_edit_QR_EventID.ShowWindow( SW_SHOW );
		m_edit_QR_EventID_Discript.ShowWindow( SW_SHOW );
		m_btn_QR_FT.ShowWindow( SW_SHOW );
		nEventSceneID = pFTQuest->m_nEvent[ CsQuest::EVENT_QUEST_REQUITE ];
		if( g_pEventDataMng->IsData( nEventSceneID ) )
		{
			nsCsMfcFunc::Int2Control( &m_edit_QR_EventID, nEventSceneID );
		}
		else
		{
			nsCsMfcFunc::Int2Control( &m_edit_QR_EventID, 0 );
		}
	}
}

void _DlgTab_Event::OnEnChangeEditQsEi()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();

	UINT nEventSceneID = nsCsMfcFunc::Control2Int( &m_edit_QS_EventID );
	pFTQuest->m_nEvent[ CsQuest::EVENT_QUEST_START ] = nEventSceneID;

	if( g_pEventDataMng->IsData( nEventSceneID ) )
	{
		CEventSceneData* pEventData = g_pEventDataMng->GetData( nEventSceneID );
		if( pEventData->GetEventSize() > 0 )
		{
			TCHAR sz[ MAX_PATH ];
			_stprintf_s( sz, MAX_PATH, L"[Cnt=%d], 0=%s", pEventData->GetEventSize(), pEventData->GetEventDatabyIdx( 0 )->GetEventInfo()->s_strTitle );
			m_edit_QS_EventID_Discript.SetWindowText( sz );
		}
		else
		{
			m_edit_QS_EventID_Discript.SetWindowText( L"씬은 존재하지만 이벤트가 존재하지 않습니다." );
		}		
	}
	else
	{
		m_edit_QS_EventID_Discript.SetWindowText( L"  x  " );
	}
}


// _DlgTab_Event 메시지 처리기입니다.

void _DlgTab_Event::OnBnClickedButtonQsEi()
{
	DWORD dwReturn;
	_DlgModal_List dlg( _DlgModal_List::LT_EVENT_SCENE, &dwReturn );
	if( dlg.DoModal() == IDOK )
	{
		nsCsMfcFunc::Int2Control( &m_edit_QS_EventID, dwReturn );
	}
}


void _DlgTab_Event::OnEnChangeEditQpEi()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();

	UINT nEventSceneID = nsCsMfcFunc::Control2Int( &m_edit_QP_EventID );
	pFTQuest->m_nEvent[ CsQuest::EVENT_QUEST_PROC ] = nEventSceneID;

	if( g_pEventDataMng->IsData( nEventSceneID ) )
	{
		CEventSceneData* pEventData = g_pEventDataMng->GetData( nEventSceneID );
		if( pEventData->GetEventSize() > 0 )
		{
			TCHAR sz[ MAX_PATH ];
			_stprintf_s( sz, MAX_PATH, L"[Cnt=%d], 0=%s", pEventData->GetEventSize(), pEventData->GetEventDatabyIdx( 0 )->GetEventInfo()->s_strTitle );
			m_edit_QP_EventID_Discript.SetWindowText( sz );
		}
		else
		{
			m_edit_QP_EventID_Discript.SetWindowText( L"씬은 존재하지만 이벤트가 존재하지 않습니다." );
		}		
	}
	else
	{
		m_edit_QP_EventID_Discript.SetWindowText( L"  x  " );
	}
}

void _DlgTab_Event::OnBnClickedButtonQpEi()
{
	DWORD dwReturn;
	_DlgModal_List dlg( _DlgModal_List::LT_EVENT_SCENE, &dwReturn );
	if( dlg.DoModal() == IDOK )
	{
		nsCsMfcFunc::Int2Control( &m_edit_QP_EventID, dwReturn );
	}
}

void _DlgTab_Event::OnEnChangeEditQcEi()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();

	UINT nEventSceneID = nsCsMfcFunc::Control2Int( &m_edit_QC_EventID );
	pFTQuest->m_nEvent[ CsQuest::EVENT_QUEST_COMPLEAT ] = nEventSceneID;

	if( g_pEventDataMng->IsData( nEventSceneID ) )
	{
		CEventSceneData* pEventData = g_pEventDataMng->GetData( nEventSceneID );
		if( pEventData->GetEventSize() > 0 )
		{
			TCHAR sz[ MAX_PATH ];
			_stprintf_s( sz, MAX_PATH, L"[Cnt=%d], 0=%s", pEventData->GetEventSize(), pEventData->GetEventDatabyIdx( 0 )->GetEventInfo()->s_strTitle );
			m_edit_QC_EventID_Discript.SetWindowText( sz );
		}
		else
		{
			m_edit_QC_EventID_Discript.SetWindowText( L"씬은 존재하지만 이벤트가 존재하지 않습니다." );
		}		
	}
	else
	{
		m_edit_QC_EventID_Discript.SetWindowText( L"  x  " );
	}
}

void _DlgTab_Event::OnBnClickedButtonQcEi()
{
	DWORD dwReturn;
	_DlgModal_List dlg( _DlgModal_List::LT_EVENT_SCENE, &dwReturn );
	if( dlg.DoModal() == IDOK )
	{
		nsCsMfcFunc::Int2Control( &m_edit_QC_EventID, dwReturn );
	}
}

void _DlgTab_Event::OnEnChangeEditQrEi()
{
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();

	UINT nEventSceneID = nsCsMfcFunc::Control2Int( &m_edit_QR_EventID );
	pFTQuest->m_nEvent[ CsQuest::EVENT_QUEST_REQUITE ] = nEventSceneID;

	if( g_pEventDataMng->IsData( nEventSceneID ) )
	{
		CEventSceneData* pEventData = g_pEventDataMng->GetData( nEventSceneID );
		if( pEventData->GetEventSize() > 0 )
		{
			TCHAR sz[ MAX_PATH ];
			_stprintf_s( sz, MAX_PATH, L"[Cnt=%d], 0=%s", pEventData->GetEventSize(), pEventData->GetEventDatabyIdx( 0 )->GetEventInfo()->s_strTitle );
			m_edit_QR_EventID_Discript.SetWindowText( sz );
		}
		else
		{
			m_edit_QR_EventID_Discript.SetWindowText( L"씬은 존재하지만 이벤트가 존재하지 않습니다." );
		}		
	}
	else
	{
		m_edit_QR_EventID_Discript.SetWindowText( L"  x  " );
	}
}

void _DlgTab_Event::OnBnClickedButtonQrEi()
{
	DWORD dwReturn;
	_DlgModal_List dlg( _DlgModal_List::LT_EVENT_SCENE, &dwReturn );
	if( dlg.DoModal() == IDOK )
	{
		nsCsMfcFunc::Int2Control( &m_edit_QR_EventID, dwReturn );
	}
}
