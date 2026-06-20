// _DlgModal_Simple.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgModal_Simple.h"


// _DlgModal_Simple 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgModal_Simple, CDialog)

_DlgModal_Simple::_DlgModal_Simple( DWORD dwQuestID, CWnd* pParent /*=NULL*/)
	: CDialog(_DlgModal_Simple::IDD, pParent)
{
	m_dwQuestID = dwQuestID;
}

_DlgModal_Simple::~_DlgModal_Simple()
{
}

void _DlgModal_Simple::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LEVEL, m_edit_Level);
	DDX_Control(pDX, IDC_EDIT_MONEY, m_edit_Money);
	DDX_Control(pDX, IDC_EDIT_EXP, m_edit_Exp);
}


BEGIN_MESSAGE_MAP(_DlgModal_Simple, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OK, &_DlgModal_Simple::OnBnClickedButtonOk)
END_MESSAGE_MAP()


BOOL _DlgModal_Simple::OnInitDialog()
{
	CDialog::OnInitDialog();

	CsQuest* pQuest = nsCsFileTable::g_pQuestMng->GetQuest( m_dwQuestID );

	nsCsMfcFunc::Int2Control( &m_edit_Level, pQuest->GetLevel() );
	CsQuestRequiteGroup* pGroup = pQuest->GetRequiteGroup();
	CsQuestRequite::LIST* pList = pGroup->GetList();
	CsQuestRequite::LIST_IT it = pList->begin();
	CsQuestRequite::LIST_IT itEnd = pList->end();
	CsQuestRequite* pData;
	for( ; it!=itEnd; ++it )
	{
		pData = *it;
		switch( pData->GetType() )
		{
		case CsQuestRequite::MONEY:
			nsCsMfcFunc::Int2Control( &m_edit_Money, pData->GetTypeID() );
			break;
		case CsQuestRequite::EXP:
			nsCsMfcFunc::Int2Control( &m_edit_Exp, pData->GetTypeID() );
			break;
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}



// _DlgModal_Simple 메시지 처리기입니다.

void _DlgModal_Simple::OnBnClickedButtonOk()
{
	CsQuest* pQuest = nsCsFileTable::g_pQuestMng->GetQuest( m_dwQuestID );

	// 레벨
	pQuest->SetLevel( nsCsMfcFunc::Control2Int( &m_edit_Level ) );

	CsQuestRequiteGroup* pGroup = pQuest->GetRequiteGroup();
	CsQuestRequite::LIST* pList = pGroup->GetList();	
	CsQuestRequite::LIST_IT itEnd = pList->end();
	CsQuestRequite* pData;

	// 돈이 입력 되어 있나 또는 0인가
	int nMoney = nsCsMfcFunc::Control2Int( &m_edit_Money );
	assert_cs( nMoney >= 0 );

	// 돈
	if( nMoney > 0 )
	{
		bool bFind = false;
		CsQuestRequite::LIST_IT it = pList->begin();		
		for( ; it!=itEnd; ++it )
		{
			pData = *it;
			if( pData->GetType() == CsQuestRequite::MONEY )
			{
				bFind = true;
				pData->SetTypeID( nMoney );
				break;
			}
		}
		// 발견 못햇으면 추가
		if( bFind == false )
		{
			pGroup->Add( CsQuestRequite::RM_GIVE, CsQuestRequite::MONEY, nMoney, 0 );
		}
	}
	// 제거
	else
	{
		CsQuestRequite::LIST_IT it = pList->begin();
		int nIndex = 0;
		for( ; it!=itEnd; ++it )
		{
			pData = *it;
			if( pData->GetType() == CsQuestRequite::MONEY )
			{
				pGroup->Delete( nIndex );
				break;
			}
			++nIndex;
		}
	}

	// 경험치가 입력 되어 있나 또는 0인가
	int nExp = nsCsMfcFunc::Control2Int( &m_edit_Exp );
	assert_cs( nExp >= 0 );

	// 경험치
	if( nExp > 0 )
	{
		bool bFind = false;
		CsQuestRequite::LIST_IT it = pList->begin();		
		for( ; it!=itEnd; ++it )
		{
			pData = *it;
			if( pData->GetType() == CsQuestRequite::EXP )
			{
				bFind = true;
				pData->SetTypeID( nExp );
				break;
			}
		}
		// 발견 못햇으면 추가
		if( bFind == false )
		{
			pGroup->Add( CsQuestRequite::RM_GIVE, CsQuestRequite::EXP, nExp, 0 );
		}
	}
	// 제거
	else
	{
		CsQuestRequite::LIST_IT it = pList->begin();
		int nIndex = 0;
		for( ; it!=itEnd; ++it )
		{
			pData = *it;
			if( pData->GetType() == CsQuestRequite::EXP )
			{
				pGroup->Delete( nIndex );
				break;
			}
			++nIndex;
		}
	}

	return CDialog::OnOK();
}
