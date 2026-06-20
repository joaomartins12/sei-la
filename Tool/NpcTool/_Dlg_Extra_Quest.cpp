// _DlgExtra_Quest.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_Dlg_Extra_Quest.h"



void _Dlg_Extra_Quest::_Init()
{
	m_pExtraData = NULL;
	m_pSelectAct = NULL;

	m_cmb_InitAct.InsertString( sNPC_EXTRA_QUEST::NEQ_HIDE, L"숨기기" );
	m_cmb_InitAct.SetItemData( sNPC_EXTRA_QUEST::NEQ_HIDE, sNPC_EXTRA_QUEST::NEQ_HIDE );
	m_cmb_InitAct.InsertString( sNPC_EXTRA_QUEST::NEQ_SHOW, L"보이기" );
	m_cmb_InitAct.SetItemData( sNPC_EXTRA_QUEST::NEQ_SHOW, sNPC_EXTRA_QUEST::NEQ_SHOW );
	m_cmb_InitAct.SetCurSel( sNPC_EXTRA_QUEST::NEQ_HIDE );

	m_cmb_ActType.InsertString( sNPC_EXTRA_QUEST::NEA_QUEST_COMP, L"완료" );
	m_cmb_ActType.SetItemData( sNPC_EXTRA_QUEST::NEA_QUEST_COMP, sNPC_EXTRA_QUEST::NEA_QUEST_COMP );
	m_cmb_ActType.InsertString( sNPC_EXTRA_QUEST::NEA_QUEST_PROCCOMP, L"진행완료" );
	m_cmb_ActType.SetItemData( sNPC_EXTRA_QUEST::NEA_QUEST_PROCCOMP, sNPC_EXTRA_QUEST::NEA_QUEST_PROCCOMP );
	m_cmb_ActType.SetCurSel( sNPC_EXTRA_QUEST::NEA_QUEST_COMP );

	m_cmb_Act.InsertString( sNPC_EXTRA_QUEST::NEQ_HIDE, L"숨기기" );
	m_cmb_Act.SetItemData( sNPC_EXTRA_QUEST::NEQ_HIDE, sNPC_EXTRA_QUEST::NEQ_HIDE );
	m_cmb_Act.InsertString( sNPC_EXTRA_QUEST::NEQ_SHOW, L"보이기" );
	m_cmb_Act.SetItemData( sNPC_EXTRA_QUEST::NEQ_SHOW, sNPC_EXTRA_QUEST::NEQ_SHOW );
	m_cmb_Act.SetCurSel( sNPC_EXTRA_QUEST::NEQ_HIDE );
}

void _Dlg_Extra_Quest::_SetData( sNPC_EXTRA_QUEST* pExtra )
{
	m_pExtraData = pExtra;	
	assert_cs( m_pExtraData != NULL );

	m_cmb_InitAct.SetCurSel( m_pExtraData->s_eInitState );
	_ResetActList( false );
}

void _Dlg_Extra_Quest::_ResetActList( bool bKeepSelect )
{
	sNPC_EXTRA_QUEST::sACT* pOldSelect = m_pSelectAct;
	int nOldIndex = m_list_Act.GetCurSel();

	__SetAct( NULL );
	m_list_Act.ResetContent();

	if( m_pExtraData == NULL )
		return;

	CString str1;
	CString str2;
	CsVectorPB< sNPC_EXTRA_QUEST::sACT* >* pVec = m_pExtraData->GetVector();
	int nCnt = pVec->Size();
	for( int i=0; i<nCnt; ++i )
	{
		assert_cs( pVec->GetData( i )->s_eCompState == m_cmb_Act.GetItemData( pVec->GetData( i )->s_eCompState ) );
		
		m_cmb_ActType.GetLBText( pVec->GetData( i )->s_eActType, str1 );
		m_cmb_Act.GetLBText( pVec->GetData( i )->s_eCompState, str2 );
		str1.Append( L" / " );
		str1 += str2;


		str1.Append( L"   [ " );
		std::list< int >::iterator it = pVec->GetData( i )->s_list.begin();
		std::list< int >::iterator itEnd = pVec->GetData( i )->s_list.end();
		for( ; it!=itEnd; ++it )
		{
			str1.AppendFormat( L"%d ", (*it) );
		}		
		str1.Append( L"]" );

		m_list_Act.InsertString( i, str1 );
		m_list_Act.SetItemData( i, (DWORD_PTR)pVec->GetData( i ) );
	}

	if( bKeepSelect == true )
	{
		__SetAct( pOldSelect );
		m_list_Act.SetCurSel( nOldIndex );
	}
}

void _Dlg_Extra_Quest::OnLbnSelchangeListAct()
{
	int nSel = m_list_Act.GetCurSel();
	if( nSel == -1 )
	{
		__SetAct( NULL );
		return;
	}

	__SetAct( (sNPC_EXTRA_QUEST::sACT*)m_list_Act.GetItemData( nSel ) );
}

void _Dlg_Extra_Quest::OnCbnSelchangeComboInitact()
{
	if( m_pExtraData == NULL )
		return;

	m_pExtraData->s_eInitState = (sNPC_EXTRA_QUEST::eSTATE)m_cmb_InitAct.GetItemData( m_cmb_InitAct.GetCurSel() );
}

void _Dlg_Extra_Quest::OnBnClickedButtonAdd()
{
	if( m_pExtraData == NULL )
		return;

	sNPC_EXTRA_QUEST::sACT* pAct = csnew sNPC_EXTRA_QUEST::sACT;
	pAct->s_eActType = (sNPC_EXTRA_QUEST::eACT_TYPE)m_cmb_ActType.GetItemData( m_cmb_ActType.GetCurSel() );	
	pAct->s_eCompState = (sNPC_EXTRA_QUEST::eSTATE)m_cmb_Act.GetItemData( m_cmb_Act.GetCurSel() );	
	m_pExtraData->GetVector()->PushBack( pAct );

	_ResetActList( false );
}

void _Dlg_Extra_Quest::OnBnClickedButtonDel()
{
	if( m_pExtraData == NULL )
		return;

	int nSel = m_list_Act.GetCurSel();
	if( nSel == -1 )
	{
		CsMessageBox( MB_OK, L"좌측 리스트 에서 항목을 선택하여 주십시오" );
		return;
	}

	m_pExtraData->GetVector()->GetData( nSel )->Delete();
	delete m_pExtraData->GetVector()->GetData( nSel );
	m_pExtraData->GetVector()->DeleteElement( nSel );

	_ResetActList( false );
}




void _Dlg_Extra_Quest::__SetAct( sNPC_EXTRA_QUEST::sACT* pAct )
{
	m_pSelectAct = pAct;

	m_list_Quest.ResetContent();
	m_edit_QuestID.SetWindowText( L"" );
	m_edit_QuestStr.SetWindowText( L"" );

	if( m_pSelectAct == NULL )
	{
		m_list_Quest.ShowWindow( SW_HIDE );
		m_edit_QuestID.ShowWindow( SW_HIDE );
		m_edit_QuestStr.ShowWindow( SW_HIDE );
		m_btn_Table.ShowWindow( SW_HIDE );
		m_btn_AddQuest.ShowWindow( SW_HIDE );
		m_btn_DelQuest.ShowWindow( SW_HIDE );
		m_static_QuestDisc.ShowWindow( SW_HIDE );

		return;
	}

	m_list_Quest.ShowWindow( SW_SHOW );
	m_edit_QuestID.ShowWindow( SW_SHOW );
	m_edit_QuestStr.ShowWindow( SW_SHOW );
	m_btn_Table.ShowWindow( SW_SHOW );
	m_btn_AddQuest.ShowWindow( SW_SHOW );
	m_btn_DelQuest.ShowWindow( SW_SHOW );
	m_static_QuestDisc.ShowWindow( SW_SHOW );

	CString str;
	std::list< int >::iterator it = pAct->s_list.begin();
	std::list< int >::iterator itEnd = pAct->s_list.end();
	for( ; it!=itEnd; ++it )
	{
		int nIndex = m_list_Quest.GetCount();
		str.Format( L"%d   ", (*it) );
		if( nsCsFileTable::g_pQuestMng->IsQuest( (*it) ) == false )
			str += L"- Empty -";
		else
			str += nsCsFileTable::g_pQuestMng->GetQuest( *it )->m_szTitleText;

		m_list_Quest.InsertString( nIndex, str );
		m_list_Quest.SetItemData( nIndex, *it );
	}
}


void _Dlg_Extra_Quest::OnEnChangeEditQuestid()
{
	if( m_pExtraData == NULL )
		return;

	int nQuestID = nsCsMfcFunc::Control2Int( &m_edit_QuestID );
	if( nsCsFileTable::g_pQuestMng->IsQuest( nQuestID ) == false )
	{
		m_edit_QuestStr.SetWindowText( L"  - Empty  -" );
		return;
	}

	m_edit_QuestStr.SetWindowText( nsCsFileTable::g_pQuestMng->GetQuest( nQuestID )->m_szTitleText );
}


void _Dlg_Extra_Quest::OnBnClickedButtonTable()
{
	DWORD dwReturn;
	_DlgModal_List dlg( _DlgModal_List::LT_QUEST, &dwReturn );
	if( dlg.DoModal() == IDOK )
	{
		nsCsMfcFunc::Int2Control( &m_edit_QuestID, dwReturn );
	}
}

void _Dlg_Extra_Quest::OnBnClickedButtonAddQuest()
{
	int nQuestID = nsCsMfcFunc::Control2Int( &m_edit_QuestID );
	if( nQuestID == 0 )
	{
		CsMessageBox( MB_OK, L"ID 를 입력하여 주십시오" );
		return;
	}

	m_pSelectAct->s_list.push_back( nQuestID );

	_ResetActList( true );
}

void _Dlg_Extra_Quest::OnBnClickedButtonDelQuest()
{
	int nSel = m_list_Quest.GetCurSel();
	if( nSel == -1 )
	{
		CsMessageBox( MB_OK, L"위 리스트 에서 항목을 선택하여 주십시오" );
		return;
	}

	int nSelQuestID = (int)m_list_Quest.GetItemData( nSel );
	std::list< int >::iterator it = m_pSelectAct->s_list.begin();
	std::list< int >::iterator itEnd = m_pSelectAct->s_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( nSelQuestID == (*it) )
		{
			m_pSelectAct->s_list.erase( it );
			break;
		}
	}

	_ResetActList( true );
}



// _DlgExtra_Quest 대화 상자입니다.

IMPLEMENT_DYNAMIC(_Dlg_Extra_Quest, CDialog)

_Dlg_Extra_Quest::_Dlg_Extra_Quest(CWnd* pParent /*=NULL*/)
	: CDialog(_Dlg_Extra_Quest::IDD, pParent)
{

}

_Dlg_Extra_Quest::~_Dlg_Extra_Quest()
{
}

void _Dlg_Extra_Quest::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_ACT, m_cmb_Act);
	DDX_Control(pDX, IDC_COMBO_INITACT, m_cmb_InitAct);
	DDX_Control(pDX, IDC_LIST_ACT, m_list_Act);
	DDX_Control(pDX, IDC_LIST_QUEST, m_list_Quest);
	DDX_Control(pDX, IDC_EDIT_QUESTID, m_edit_QuestID);
	DDX_Control(pDX, IDC_EDIT_QUESTSTR, m_edit_QuestStr);
	DDX_Control(pDX, IDC_BUTTON_TABLE, m_btn_Table);
	DDX_Control(pDX, IDC_BUTTON_ADD_QUEST, m_btn_AddQuest);
	DDX_Control(pDX, IDC_BUTTON_DEL_QUEST, m_btn_DelQuest);
	DDX_Control(pDX, IDC_STATIC_DISC, m_static_QuestDisc);
	DDX_Control(pDX, IDC_COMBO_ACTTYPE, m_cmb_ActType);
}


BEGIN_MESSAGE_MAP(_Dlg_Extra_Quest, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &_Dlg_Extra_Quest::OnBnClickedButtonAdd)
	ON_CBN_SELCHANGE(IDC_COMBO_INITACT, &_Dlg_Extra_Quest::OnCbnSelchangeComboInitact)
	ON_EN_CHANGE(IDC_EDIT_QUESTID, &_Dlg_Extra_Quest::OnEnChangeEditQuestid)
	ON_LBN_SELCHANGE(IDC_LIST_ACT, &_Dlg_Extra_Quest::OnLbnSelchangeListAct)
	ON_BN_CLICKED(IDC_BUTTON_DEL, &_Dlg_Extra_Quest::OnBnClickedButtonDel)
	ON_BN_CLICKED(IDC_BUTTON_TABLE, &_Dlg_Extra_Quest::OnBnClickedButtonTable)
	ON_BN_CLICKED(IDC_BUTTON_ADD_QUEST, &_Dlg_Extra_Quest::OnBnClickedButtonAddQuest)
	ON_BN_CLICKED(IDC_BUTTON_DEL_QUEST, &_Dlg_Extra_Quest::OnBnClickedButtonDelQuest)
END_MESSAGE_MAP()


// _DlgExtra_Quest 메시지 처리기입니다.


