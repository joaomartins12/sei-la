// _DlgModel_Create.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgModel_Create.h"


// _DlgModel_Create 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgModel_Create, CDialog)

_DlgModel_Create::_DlgModel_Create(CsQuest** ppFTQuest, int nMapID /*=-1*/, int nLevel /*=-1*/, CWnd* pParent /*=NULL*/)
	: CDialog(_DlgModel_Create::IDD, pParent)
{
	m_ppFTQuest = ppFTQuest;
	m_nMapID = nMapID;
	m_nLevel = nLevel;
}

_DlgModel_Create::~_DlgModel_Create()
{
}


BOOL _DlgModel_Create::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_combo_ST.InsertString( CsQuest::ST_NPC, L"엔피씨" );
	m_combo_ST.InsertString( CsQuest::ST_DIGIVICE, L"디지바이스" );

	*m_ppFTQuest = NULL;
	DWORD nUniqID = nsCsFileTable::g_pQuestMng->GetLastUniqID();	
	nsCsMfcFunc::Int2Control( &m_edit_UniqID, nUniqID );

	if( m_nLevel == -1 )
	{
		nsCsMfcFunc::Int2Control( &m_edit_Level, 1 );
	}
	else
	{
		nsCsMfcFunc::Int2Control( &m_edit_Level, m_nLevel );
		m_edit_Level.SetReadOnly( TRUE );
	}


	m_combo_ST.SetCurSel( CsQuest::ST_NPC );
	OnCbnSelchangeComboSt();

	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgModel_Create::OnCbnSelchangeComboSt()
{
	m_edit_ST_ID.ShowWindow( SW_HIDE );
	m_btn_ST_Table.ShowWindow( SW_HIDE );

	switch( m_combo_ST.GetCurSel() )
	{
	case CsQuest::ST_NPC:
		m_edit_ST_ID.ShowWindow( SW_SHOW );
		m_btn_ST_Table.ShowWindow( SW_SHOW );
		break;
	case CsQuest::ST_DIGIVICE:
		break;
	default:
		assert_cs( false );
	}
}

void _DlgModel_Create::OnBnClickedButtonStTable()
{
	DWORD dwValue;
	_DlgModal_List dlg( _DlgModal_List::LT_NPC, &dwValue, NULL, m_nMapID );
	if( dlg.DoModal() == IDOK )
	{		
		nsCsMfcFunc::Int2Control( &m_edit_ST_ID, dwValue );
	}
}


void _DlgModel_Create::OnBnClickedButtonOk()
{
	switch( m_combo_ST.GetCurSel() )
	{
	case CsQuest::ST_NPC:
		if( nsCsMfcFunc::Control2Int( &m_edit_ST_ID ) == 0 )
		{
			CsMessageBox( MB_OK, L"엔피씨를 지정해야 합니다." );
			return;
		}
		break;
	case CsQuest::ST_DIGIVICE:
		break;
	default:
		assert_cs( false );
	}

	DWORD nUniqID = nsCsMfcFunc::Control2Int( &m_edit_UniqID );
	if( nsCsFileTable::g_pQuestMng->IsQuest( nUniqID ) == true )
	{
		CsMessageBox( MB_OK, L"이미 사용하고 있는 ID 입니다." );
		return;
	}

	*m_ppFTQuest = nsCsFileTable::g_pQuestMng->AddQuest( nUniqID );

	SHORT nLevel = (SHORT)nsCsMfcFunc::Control2Int( &m_edit_Level );
	( *m_ppFTQuest )->SetLevel( nLevel );
	( *m_ppFTQuest )->SetStartTarget_ID( nsCsMfcFunc::Control2Int( &m_edit_ST_ID ) );
	( *m_ppFTQuest )->SetStartTarget_Type( (CsQuest::eST_TYPE)m_combo_ST.GetCurSel() );	

	CDialog::OnOK();
}

void _DlgModel_Create::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_UNIQID, m_edit_UniqID);
	DDX_Control(pDX, IDC_COMBO_ST, m_combo_ST);
	DDX_Control(pDX, IDC_EDIT_ST_ID, m_edit_ST_ID);
	DDX_Control(pDX, IDC_BUTTON_ST_TABLE, m_btn_ST_Table);
	DDX_Control(pDX, IDC_BUTTON_OK, m_btn_Create);
	DDX_Control(pDX, IDC_EDIT_LEVEL, m_edit_Level);
}

BEGIN_MESSAGE_MAP(_DlgModel_Create, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_ST, &_DlgModel_Create::OnCbnSelchangeComboSt)
	ON_BN_CLICKED(IDC_BUTTON_ST_TABLE, &_DlgModel_Create::OnBnClickedButtonStTable)
	ON_BN_CLICKED(IDC_BUTTON_OK, &_DlgModel_Create::OnBnClickedButtonOk)
END_MESSAGE_MAP()




