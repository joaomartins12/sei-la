// _DlgTab_Setting.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgTab_Setting.h"


// _DlgTab_Setting 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgTab_Setting, CDialog)

_DlgTab_Setting::_DlgTab_Setting(CWnd* pParent /*=NULL*/)
	: CDialog(_DlgTab_Setting::IDD, pParent)
{

}

_DlgTab_Setting::~_DlgTab_Setting()
{
}

void _DlgTab_Setting::_Init()
{
	// 퀘스트 타입
	m_combo_MainType.InsertString( CsQuest::QT_SUB, L"서브 퀘스트" );
	m_combo_MainType.InsertString( CsQuest::QT_MAIN, L"메인 퀘스트" );
	m_combo_MainType.InsertString( CsQuest::QT_DAILY, L"일일 퀘스트" );
	m_combo_MainType.InsertString( CsQuest::QT_REPEAT, L"반복 퀘스트" );
	m_combo_MainType.InsertString( CsQuest::QT_EVENTREPEAT, L"이벤트반복 퀘스트" );
	m_combo_MainType.InsertString( CsQuest::QT_ACHIEVE, L"업적 퀘스트" );
	m_combo_MainType.InsertString( CsQuest::QT_JOINTPROGRESS, L"합체 진화 퀘스트" );
	m_combo_MainType.InsertString( CsQuest::QT_TUTORIAL, L"튜토리얼 퀘스트" );
	
	m_combo_MainType.InsertString( CsQuest::QT_XANTI_JOINTPROGRESS, L"X항체 합체 퀘스트" );

	//m_combo_MainType.InsertString( CsQuest::QT_TIME, L"시간제한 퀘스트" );

	// 시작 타겟
	m_combo_StartTarget.InsertString( CsQuest::ST_NPC, L"NPC" );
	m_combo_StartTarget.InsertString( CsQuest::ST_DIGIVICE, L"디지바이스" );

	// 완료 대상
	m_combo_CT.InsertString( CsQuest::QTG_SELF, L"시작타겟" );
	m_combo_CT.InsertString( CsQuest::QTG_NPC, L"NPC" );
	m_combo_CT.InsertString( CsQuest::QTG_COMPLETE, L"완료 즉시" );

	// 수행 지급
	m_combo_AP.InsertString( CsQuestApplyRequite::ITEM, L"Item" );
	//m_combo_AP.InsertString( CsQuestApplyRequite::TIME, L"Time" );


	// 선행 조건
	m_combo_PreReq.InsertString( CsQuestPreRequire::QUEST, L"퀘스트" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::T_LEVEL, L"테이머 레벨" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::ITEM, L"아이템" );	
	m_combo_PreReq.InsertString( CsQuestPreRequire::D_LEVEL, L"디지몬 레벨" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::D_TYPE, L"디지몬 타입" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::REGION, L"영역 돌입" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::D_EVO, L"진화 슬롯" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::D_EVO_DISABLE, L"오픈금지진화슬롯" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::D_BASE_TABLE_IDX, L"디지몬계열" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::TAMER_TABLE_IDX, L"테이머 테이블ID" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::QUEST_ID_CANNOT_PROGRESS, L"특정 퀘스트 진행 시 해당 진행 불가" );
	m_combo_PreReq.InsertString( CsQuestPreRequire::QUEST_ID_CANNOT_COMPLETED, L"특정 퀘스트 완료 시 해당 진행 불가" );
	// 완료 조건	
	m_combo_CompRequ.InsertString( CsQuestCompRequire::MONSTER, L"몬스터 사냥" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::ITEM, L"아이템 수집" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::CLIENT, L"클라이언트 처리" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::REGION, L"영역 돌입" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::NPCTALK, L"엔피씨와 대화" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::USE_ITEM, L"아이템 사용" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::USE_ITEM_NPC, L"Npc 에게 아이템 사용" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::USE_ITEM_MONSTER, L"몬스터 에게 아이템 사용" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::USE_ITEM_REGION, L"지역에서 아이템 사용" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::T_LEVEL, L"테이머 레벨" );
	m_combo_CompRequ.InsertString( CsQuestCompRequire::D_TYPE, L"디지몬 타입" );

	// 보상
	m_combo_Reqt_Method.InsertString( CsQuestRequite::RM_GIVE, L"지급" );
	m_combo_Reqt_Method.InsertString( CsQuestRequite::RM_SEL1, L"선택1" );

	m_combo_Reqt_Type.InsertString( CsQuestRequite::MONEY, L"돈" );
	m_combo_Reqt_Type.InsertString( CsQuestRequite::EXP, L"경험치" );
	m_combo_Reqt_Type.InsertString( CsQuestRequite::ITEM, L"아이템" );
	m_combo_Reqt_Type.InsertString( CsQuestRequite::EVOSLOT, L"진화 슬롯" );

	_SetQuest( NULL );
}


void _DlgTab_Setting::_SetQuest( cQuestObj* pQuestObj )
{
	if( pQuestObj == NULL )
	{
		m_chk_Active.ShowWindow( SW_HIDE );
		m_chk_Immediate.ShowWindow( SW_HIDE );

		// 퀘스트 제목
		m_edit_Title.ShowWindow( SW_HIDE );

		// Model ID
		m_edit_ModelID_Recv.ShowWindow( SW_HIDE );
		m_edit_ModelID_Comp.ShowWindow( SW_HIDE );

		// 퀘스트 ID
		m_edit_UniqID.ShowWindow( SW_HIDE );

		// MngID
		m_edit_MngID.ShowWindow( SW_HIDE );

		// 레벨
		m_edit_Level.ShowWindow( SW_HIDE );
		
		// 퀘스트 타입
		m_combo_MainType.ShowWindow( SW_HIDE );

		// 시작 타겟
		__SetStartTarget( NULL );

		// 완료 대상
		__SetCompTarget( NULL );

		// 수행 지급
		__SetApplyRequite( NULL );

		// 선행 조건
		__SetPreRequire( NULL );

		// 완료 조건
		__SetCompRequire( NULL );

		// 보상
		__SetRequite( NULL );
	}
	else
	{
		CsQuest* pFTQuest = pQuestObj->GetFT();
		
		if(pFTQuest == NULL)
			return;
		
		switch( pFTQuest->GetQuestType() )
		{
		case CsQuest::QT_ACHIEVE:
			{
				m_chk_Active.ShowWindow( SW_SHOW );
				m_chk_Active.SetCheck( pFTQuest->IsActive() ? BST_CHECKED : BST_UNCHECKED );

				m_chk_Immediate.ShowWindow( SW_SHOW );
				m_chk_Immediate.SetCheck( pFTQuest->IsImmediate() ? BST_CHECKED : BST_UNCHECKED );

#ifdef QUEST_RESET_180712
				m_isReset.ShowWindow(SW_SHOW);
				m_isReset.SetCheck(pFTQuest->GetIsResetQuest() ? BST_CHECKED : BST_UNCHECKED );
#endif
				// 퀘스트 제목
				m_edit_Title.ShowWindow( SW_SHOW );
				m_edit_Title.SetWindowText( pFTQuest->m_szTitleText );

				// Model ID
				m_edit_ModelID_Recv.ShowWindow( SW_HIDE );
				m_edit_ModelID_Comp.ShowWindow( SW_HIDE );

				// 퀘스트 ID
				m_edit_UniqID.ShowWindow( SW_SHOW );
				nsCsMfcFunc::Int2Control( &m_edit_UniqID, pFTQuest->GetUniqID() );

				// MngID
				m_edit_MngID.ShowWindow( SW_SHOW );
				nsCsMfcFunc::Int2Control( &m_edit_MngID, pFTQuest->GetManageID() );

				// 레벨
				m_edit_Level.ShowWindow( SW_SHOW );
				nsCsMfcFunc::Int2Control( &m_edit_Level, pFTQuest->GetLevel() );

				// 퀘스트 타입
				m_combo_MainType.ShowWindow( SW_SHOW );
				m_combo_MainType.SetCurSel( pFTQuest->GetQuestType() );

				// 시작 타겟
				__SetStartTarget( NULL );

				// 완료 대상
				__SetCompTarget( NULL );

				// 수행 지급
				__SetApplyRequite( NULL );

				// 선행 조건
				__SetPreRequire( NULL );

				// 완료 조건
				__SetCompRequire( pQuestObj );

				// 보상
				__SetRequite( NULL );
			}
			break;
		default:
			{
				m_chk_Active.ShowWindow( SW_SHOW );
				m_chk_Active.SetCheck( pFTQuest->IsActive() ? BST_CHECKED : BST_UNCHECKED );

				m_chk_Immediate.ShowWindow( SW_SHOW );
				m_chk_Immediate.SetCheck( pFTQuest->IsImmediate() ? BST_CHECKED : BST_UNCHECKED );

#ifdef QUEST_RESET_180712
				m_isReset.ShowWindow(SW_SHOW);
				m_isReset.SetCheck(pFTQuest->GetIsResetQuest() ? BST_CHECKED : BST_UNCHECKED );
#endif
				// Model ID
				m_edit_ModelID_Recv.ShowWindow( SW_SHOW );
				nsCsMfcFunc::Int2Control( &m_edit_ModelID_Recv, pFTQuest->GetModelID_Recv() );
				m_edit_ModelID_Comp.ShowWindow( SW_SHOW );
				nsCsMfcFunc::Int2Control( &m_edit_ModelID_Comp, pFTQuest->GetModelID_Comp() );

				// 퀘스트 제목
				m_edit_Title.ShowWindow( SW_SHOW );
				m_edit_Title.SetWindowText( pFTQuest->m_szTitleText );

				// 퀘스트 ID
				m_edit_UniqID.ShowWindow( SW_SHOW );
				nsCsMfcFunc::Int2Control( &m_edit_UniqID, pFTQuest->GetUniqID() );

				// MngID
				m_edit_MngID.ShowWindow( SW_SHOW );
				nsCsMfcFunc::Int2Control( &m_edit_MngID, pFTQuest->GetManageID() );

				// 레벨
				m_edit_Level.ShowWindow( SW_SHOW );
				nsCsMfcFunc::Int2Control( &m_edit_Level, pFTQuest->GetLevel() );

				// 퀘스트 타입
				m_combo_MainType.ShowWindow( SW_SHOW );
				m_combo_MainType.SetCurSel( pFTQuest->GetQuestType() );

				// 시작 타겟
				__SetStartTarget( pQuestObj );

				// 완료 대상
				__SetCompTarget( pQuestObj );

				// 수행 지급
				__SetApplyRequite( pQuestObj );

				// 선행 조건
				__SetPreRequire( pQuestObj );

				// 완료 조건
				__SetCompRequire( pQuestObj );

				// 보상
				__SetRequite( pQuestObj );
			}
		}		
	}
}



void _DlgTab_Setting::OnEnChangeEditMngid()
{
	g_pDiscript->_GetSelQuest()->GetFT()->SetManageID( nsCsMfcFunc::Control2Int( &m_edit_MngID ) );
}

void _DlgTab_Setting::OnEnChangeEditModelID_Recv()
{
	g_pDiscript->_GetSelQuest()->GetFT()->SetModelID_Recv( nsCsMfcFunc::Control2Int( &m_edit_ModelID_Recv ) );
}

void _DlgTab_Setting::OnEnChangeEditModelID_Comp()
{
	g_pDiscript->_GetSelQuest()->GetFT()->SetModelID_Comp( nsCsMfcFunc::Control2Int( &m_edit_ModelID_Comp ) );
}

void _DlgTab_Setting::OnEnChangeEditExpLevel()
{
	USHORT nNewLevel = (USHORT)nsCsMfcFunc::Control2Int( &m_edit_Level );
	if( nNewLevel == 0 )
	{
		nNewLevel = 1;
	}
	else if( nNewLevel > MAX_LEVEL )
	{
		nNewLevel = MAX_LEVEL;
	}
	
	CsQuest* pFTQuest = g_pDiscript->_GetSelQuest()->GetFT();
	if( nNewLevel == pFTQuest->GetLevel() )
		return;

	pFTQuest->SetLevel( nNewLevel );
	cQuestObj* pObject = g_pDiscript->_GetSelQuest();
	g_pViewer->_GetNodeMng()->ReleaseQuestObj( pObject, false );
	pObject->GetFT()->SetToolPos( CsPoint( -1, -1 ) );
	g_pViewer->_GetNodeMng()->ResistQuestObj( pObject, true );
	pObject->ResetMiddleTail_ParentChild();
	pObject->ResetControl();
}

void _DlgTab_Setting::OnCbnSelchangeComboMaintype()
{
	g_pDiscript->_GetSelQuest()->GetFT()->SetQuestType( (CsQuest::eQUEST_TYPE)m_combo_MainType.GetCurSel() );
	g_pDiscript->_GetSelQuest()->ResetControl();

	_SetQuest( g_pDiscript->_GetSelQuest() );
}

void _DlgTab_Setting::OnBnClickedCheckActive()
{
	g_pDiscript->_GetSelQuest()->GetFT()->SetActive( m_chk_Active.GetCheck() == BST_CHECKED );
	g_pDiscript->_GetSelQuest()->ResetBG();
}


void _DlgTab_Setting::OnBnClickedCheckImmediate()
{
	g_pDiscript->_GetSelQuest()->GetFT()->SetImmeiate( m_chk_Immediate.GetCheck() == BST_CHECKED );
}
void _DlgTab_Setting::OnBnClickedCheckIsResetQuest()
{
#ifdef QUEST_RESET_180712
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	g_pDiscript->_GetSelQuest()->GetFT()->SetResetQuest( m_isReset.GetCheck() == BST_CHECKED );
#endif
}


void _DlgTab_Setting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MAINTYPE, m_combo_MainType);
	DDX_Control(pDX, IDC_EDIT_UNIQID, m_edit_UniqID);
	DDX_Control(pDX, IDC_EDIT_MNGID, m_edit_MngID);
	DDX_Control(pDX, IDC_COMBO_START_TARGET, m_combo_StartTarget);
	DDX_Control(pDX, IDC_EDIT_UNIQID2, m_edit_Title);
	DDX_Control(pDX, IDC_EDIT_ST_ID, m_edit_ST_ID);
	DDX_Control(pDX, IDC_EDIT_ST_CNT, m_edit_ST_Cnt);
	DDX_Control(pDX, IDC_BUTTON_ST_TABLE, m_btn_ST_Table);
	DDX_Control(pDX, IDC_EDIT_ST_DISCRIPT, m_edit_ST_Discript);
	DDX_Control(pDX, IDC_COMBO_AP, m_combo_AP);
	DDX_Control(pDX, IDC_EDIT_AP_ID, m_edit_AP_ID);	
	DDX_Control(pDX, IDC_BUTTON_AP_TABLE, m_btn_AP_Table);
	DDX_Control(pDX, IDC_EDIT_AP_CNT, m_edit_AP_Cnt);
	DDX_Control(pDX, IDC_EDIT_AP_DISCRIPT, m_edit_AP_Discript);
	DDX_Control(pDX, IDC_LIST_AP, m_list_AP_List);
	DDX_Control(pDX, IDC_BUTTON_AP_ADD, m_btn_AP_Add);
	DDX_Control(pDX, IDC_BUTTON_AP_DEL, m_btn_AP_Del);
	DDX_Control(pDX, IDC_COMBO_PREREQ, m_combo_PreReq);
	DDX_Control(pDX, IDC_EDIT_PREREQ_ID, m_edit_PreReqID);
	DDX_Control(pDX, IDC_BUTTON_PREREQ_TABLE, m_btn_PreReqTable);
	DDX_Control(pDX, IDC_EDIT_PREREQ_CNT, m_edit_PreReqCnt);
	DDX_Control(pDX, IDC_EDIT_PREREQ_DISCRIPT, m_edit_PreReqDiscript);
	DDX_Control(pDX, IDC_BUTTON_PREREQ_ADD, m_btn_PreReqAdd);
	DDX_Control(pDX, IDC_BUTTON_PREREQ_DEL, m_btn_PreReqDel);
	DDX_Control(pDX, IDC_LIST_PREREQ, m_list_PreReqList);
	DDX_Control(pDX, IDC_COMBO_COMP_REQU, m_combo_CompRequ);
	DDX_Control(pDX, IDC_EDIT_COMP_REQU_ID, m_edit_CompRequ_ID);
	DDX_Control(pDX, IDC_BUTTON_COMP_REQU_TABLE, m_btn_CompRequ_Table);
	DDX_Control(pDX, IDC_EDIT_COMP_REQU_CNT, m_edit_CompRequ_Cnt);
	DDX_Control(pDX, IDC_EDIT_COMP_REQU_DISCRIPT, m_edit_CompRequ_Discript);
	DDX_Control(pDX, IDC_BUTTON_COMP_REQU_ADD, m_btn_CompRequ_Add);
	DDX_Control(pDX, IDC_BUTTON_COMP_REQU_DEL, m_btn_CompRequ_Del);
	DDX_Control(pDX, IDC_LIST_COMP_REQU, m_list_CompRequ_List);
	DDX_Control(pDX, IDC_EDIT_COMP_REQU_TARGET_ID, m_edit_CompRequTarget_ID);
	DDX_Control(pDX, IDC_BUTTON_COMP_REQU_TARGET_TABLE, m_btn_CompRequTarget_Table);
	DDX_Control(pDX, IDC_COMBO_CT, m_combo_CT);
	DDX_Control(pDX, IDC_EDIT_CT_ID, m_edit_CT_ID);
	DDX_Control(pDX, IDC_BUTTON_CT_TABLE, m_btn_CT_Table);
	DDX_Control(pDX, IDC_EDIT_CT_DISCRIPT, m_edit_CT_Discript);
	DDX_Control(pDX, IDC_COMBO_REQT_METHOD, m_combo_Reqt_Method);
	DDX_Control(pDX, IDC_COMBO_REQT_TYPE, m_combo_Reqt_Type);
	DDX_Control(pDX, IDC_EDIT_REQT_ID, m_edit_Reqt_ID);
	DDX_Control(pDX, IDC_BUTTON_REQT_TABLE2, m_btn_Reqt_Table);
	DDX_Control(pDX, IDC_EDIT_REQT_CNT, m_edit_Reqt_Cnt);
	DDX_Control(pDX, IDC_EDIT_REQT_DISCRIPT, m_edit_Reqt_Discript);
	DDX_Control(pDX, IDC_BUTTON_REQT_ADD, m_btn_Reqt_Add);
	DDX_Control(pDX, IDC_LIST_REQT, m_list_Reqt_List);
	DDX_Control(pDX, IDC_BUTTON_REQT_DEL, m_btn_Reqt_Del);
	DDX_Control(pDX, IDC_EDIT_EXP_LEVEL, m_edit_Level);
	DDX_Control(pDX, IDC_CHECK_ACTIVE, m_chk_Active);
	DDX_Control(pDX, IDC_CHECK_IMMEDIATE, m_chk_Immediate);
	DDX_Control(pDX, IDC_EDIT_MODELID_RECV, m_edit_ModelID_Recv);
	DDX_Control(pDX, IDC_EDIT_MODELID_COMP, m_edit_ModelID_Comp);
	DDX_Control(pDX, IDC_BUTTON_QUEST_INFO_EDIT, m_QuestInfoEditButton);

	DDX_Control(pDX, IDC_EDIT_COMP_REQU_SUBVALUE1, m_edit_Reqt_SubValue1);
	DDX_Control(pDX, IDC_EDIT_COMP_REQU_SUBVALUE2, m_edit_Reqt_SubValue2);	
	DDX_Control(pDX, IDC_CHECK_IS_RESET_QUEST, m_isReset);
}


BEGIN_MESSAGE_MAP(_DlgTab_Setting, CDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_START_TARGET, &_DlgTab_Setting::OnCbnSelchangeComboStartTarget)
	ON_BN_CLICKED(IDC_BUTTON_ST_TABLE, &_DlgTab_Setting::OnBnClickedButtonStTable)
	ON_EN_CHANGE(IDC_EDIT_ST_ID, &_DlgTab_Setting::OnEnChangeEditStId)
	ON_BN_CLICKED(IDC_BUTTON_AP_ADD, &_DlgTab_Setting::OnBnClickedButtonApAdd)
	ON_BN_CLICKED(IDC_BUTTON_AP_DEL, &_DlgTab_Setting::OnBnClickedButtonApDel)
	ON_CBN_SELCHANGE(IDC_COMBO_AP, &_DlgTab_Setting::OnCbnSelchangeComboAp)
	ON_BN_CLICKED(IDC_BUTTON_AP_TABLE, &_DlgTab_Setting::OnBnClickedButtonApTable)
	ON_EN_CHANGE(IDC_EDIT_AP_ID, &_DlgTab_Setting::OnEnChangeEditApId)
	ON_EN_CHANGE(IDC_EDIT_AP_CNT, &_DlgTab_Setting::OnEnChangeEditApCnt)
	ON_CBN_SELCHANGE(IDC_COMBO_PREREQ, &_DlgTab_Setting::OnCbnSelchangeComboPrereq)
	ON_EN_CHANGE(IDC_EDIT_PREREQ_ID, &_DlgTab_Setting::OnEnChangeEditPrereqId)
	ON_BN_CLICKED(IDC_BUTTON_PREREQ_TABLE, &_DlgTab_Setting::OnBnClickedButtonPrereqTable)
	ON_BN_CLICKED(IDC_BUTTON_PREREQ_ADD, &_DlgTab_Setting::OnBnClickedButtonPrereqAdd)
	ON_BN_CLICKED(IDC_BUTTON_PREREQ_DEL, &_DlgTab_Setting::OnBnClickedButtonPrereqDel)
	ON_EN_CHANGE(IDC_EDIT_PREREQ_CNT, &_DlgTab_Setting::OnEnChangeEditPrereqCnt)
	ON_CBN_SELCHANGE(IDC_COMBO_COMP_REQU, &_DlgTab_Setting::OnCbnSelchangeComboCompRequ)
	ON_EN_CHANGE(IDC_EDIT_COMP_REQU_ID, &_DlgTab_Setting::OnEnChangeEditCompRequId)
	ON_EN_CHANGE(IDC_EDIT_COMP_REQU_CNT, &_DlgTab_Setting::OnEnChangeEditCompRequCnt)
	ON_BN_CLICKED(IDC_BUTTON_COMP_REQU_TABLE, &_DlgTab_Setting::OnBnClickedButtonCompRequTable)
	ON_BN_CLICKED(IDC_BUTTON_COMP_REQU_ADD, &_DlgTab_Setting::OnBnClickedButtonCompRequAdd)
	ON_BN_CLICKED(IDC_BUTTON_COMP_REQU_DEL, &_DlgTab_Setting::OnBnClickedButtonCompRequDel)
	ON_EN_CHANGE(IDC_EDIT_COMP_REQU_TARGET_ID, &_DlgTab_Setting::OnEnChangeEditCompRequTargetId)

	ON_EN_CHANGE(IDC_EDIT_COMP_REQU_SUBVALUE1, &_DlgTab_Setting::OnEnChangeEditCompRequSubValue)
	ON_EN_CHANGE(IDC_EDIT_COMP_REQU_SUBVALUE2, &_DlgTab_Setting::OnEnChangeEditCompRequSubValue2)


	ON_LBN_SELCHANGE(IDC_LIST_COMP_REQU, &_DlgTab_Setting::OnClickCompRequListBoxItem) // [11/2/2016 hyun] 추가
	ON_BN_CLICKED(IDC_BUTTON_COMP_REQU_TARGET_TABLE, &_DlgTab_Setting::OnBnClickedButtonCompRequTargetTable)
	ON_CBN_SELCHANGE(IDC_COMBO_CT, &_DlgTab_Setting::OnCbnSelchangeComboCt)
	ON_EN_CHANGE(IDC_EDIT_CT_ID, &_DlgTab_Setting::OnEnChangeEditCtId)
	ON_BN_CLICKED(IDC_BUTTON_CT_TABLE, &_DlgTab_Setting::OnBnClickedButtonCtTable)
	ON_CBN_SELCHANGE(IDC_COMBO_REQT_TYPE, &_DlgTab_Setting::OnCbnSelchangeComboReqtType)
	ON_EN_CHANGE(IDC_EDIT_REQT_ID, &_DlgTab_Setting::OnEnChangeEditReqtId)
	ON_EN_CHANGE(IDC_EDIT_REQT_CNT, &_DlgTab_Setting::OnEnChangeEditReqtCnt)
	ON_BN_CLICKED(IDC_BUTTON_REQT_TABLE2, &_DlgTab_Setting::OnBnClickedButtonReqtTable2)
	ON_BN_CLICKED(IDC_BUTTON_REQT_ADD, &_DlgTab_Setting::OnBnClickedButtonReqtAdd)
	ON_BN_CLICKED(IDC_BUTTON_REQT_DEL, &_DlgTab_Setting::OnBnClickedButtonReqtDel)
	ON_EN_CHANGE(IDC_EDIT_EXP_LEVEL, &_DlgTab_Setting::OnEnChangeEditExpLevel)
	ON_EN_CHANGE(IDC_EDIT_MNGID, &_DlgTab_Setting::OnEnChangeEditMngid)
	ON_EN_CHANGE(IDC_EDIT_MODELID_RECV, &_DlgTab_Setting::OnEnChangeEditModelID_Recv)
	ON_EN_CHANGE(IDC_EDIT_MODELID_COMP, &_DlgTab_Setting::OnEnChangeEditModelID_Comp)
	ON_CBN_SELCHANGE(IDC_COMBO_MAINTYPE, &_DlgTab_Setting::OnCbnSelchangeComboMaintype)
	ON_BN_CLICKED(IDC_CHECK_ACTIVE, &_DlgTab_Setting::OnBnClickedCheckActive)
	ON_BN_CLICKED(IDC_CHECK_IMMEDIATE, &_DlgTab_Setting::OnBnClickedCheckImmediate)
	ON_BN_CLICKED(IDC_BUTTON_UNIQID_APPLY, &_DlgTab_Setting::OnBnClickedButtonUniqidApply)
	ON_LBN_SELCANCEL(IDC_LIST_COMP_REQU, &_DlgTab_Setting::OnLbnKillfocusListQuestDest)
	ON_BN_CLICKED(IDC_BUTTON_QUEST_INFO_EDIT, &_DlgTab_Setting::OnBnClickedButtonQuestInfoEdit)
	ON_BN_CLICKED(IDC_CHECK_IS_RESET_QUEST, &_DlgTab_Setting::OnBnClickedCheckIsResetQuest)
END_MESSAGE_MAP()

void _DlgTab_Setting::OnBnClickedButtonUniqidApply()
{
	DWORD nNewID = nsCsMfcFunc::Control2Int( &m_edit_UniqID );
	if( nNewID == 0 )
	{
		CsMessageBox( MB_OK, L"고유 ID는 0일수 없습니다." );
		return;
	}

	if( g_pDiscript->_GetSelQuest() == NULL )
		return;

	DWORD nOldID = g_pDiscript->_GetSelQuest()->GetFT()->GetUniqID();
	if( nNewID == nOldID )
		return;

	if( nsCsFileTable::g_pQuestMng->IsQuest( nNewID ) )
	{
		CsMessageBox( MB_OK, L"이미 할당된 ID 입니다." );
		nsCsMfcFunc::Int2Control( &m_edit_UniqID, nOldID );
		return;
	}

	CsQuest::MAP* pMap = nsCsFileTable::g_pQuestMng->GetQuestMap();

	CsQuest* pFTQuest = nsCsFileTable::g_pQuestMng->GetQuest( nOldID );	
	pMap->erase( nOldID );

	g_pDiscript->_GetSelQuest()->GetFT()->SetUniqID( nNewID );
	( *pMap )[ nNewID ] = pFTQuest;
}

void _DlgTab_Setting::OnClickCompRequListBoxItem()
{
	m_QuestInfoEditButton.ShowWindow(SW_SHOW);

	int nSel = m_list_CompRequ_List.GetCurSel();
	if( nSel < 0 )
		return;

	DWORD nQuestID = g_pDiscript->_GetSelQuest()->GetFT()->GetUniqID();
	CsQuestCompRequireGroup* pGroup = g_pDiscript->_GetSelQuest()->GetFT()->GetCompRequireGroup();
	SAFE_POINTER_RET(pGroup);

	CsQuestCompRequire* pComp = pGroup->Get(nSel);
	SAFE_POINTER_RET(pComp);
	m_combo_CompRequ.SetCurSel( (int)pComp->GetType() ); 
	nsCsMfcFunc::Int2Control( &m_edit_CompRequ_ID, pComp->GetTypeID() );
	nsCsMfcFunc::Int2Control( &m_edit_CompRequ_Cnt, pComp->GetTypeCount() );
	nsCsMfcFunc::Int2Control( &m_edit_CompRequTarget_ID, pComp->GetTargetID() );

	nsCsMfcFunc::Int2Control( &m_edit_Reqt_SubValue1, pComp->GetSubValue() );
	nsCsMfcFunc::Int2Control( &m_edit_Reqt_SubValue2, pComp->GetSubValue2() );
	
	OnCbnSelchangeComboCompRequ();
}

void _DlgTab_Setting::OnLbnKillfocusListQuestDest()
{
	m_QuestInfoEditButton.ShowWindow(SW_HIDE);
	OnCbnSelchangeComboCompRequ();
}

void _DlgTab_Setting::OnBnClickedButtonQuestInfoEdit()
{
	int nSel = m_list_CompRequ_List.GetCurSel();
	if( nSel < 0 )
		return;

	if( m_CompRequire.GetTypeID() == 0 )
	{
		CsMessageBox( MB_OK, L"항목이 올바르게 설정되지 않았습니다." );
		return;
	}

	CsQuest* pQuest = g_pDiscript->_GetSelQuest()->GetFT();
	SAFE_POINTER_RET(pQuest);

	CsQuestCompRequireGroup* pGroup = pQuest->GetCompRequireGroup();
	SAFE_POINTER_RET(pGroup);

	CsQuestCompRequire* pComp = pGroup->Get(nSel);
	pComp->Init((CsQuestCompRequire::eTYPE)m_combo_CompRequ.GetCurSel(),
		nsCsMfcFunc::Control2Int( &m_edit_CompRequ_ID),
		nsCsMfcFunc::Control2Int( &m_edit_CompRequ_Cnt),
		nsCsMfcFunc::Control2Int( &m_edit_CompRequTarget_ID),
		nsCsMfcFunc::Control2Int( &m_edit_Reqt_SubValue1),
		nsCsMfcFunc::Control2Int( &m_edit_Reqt_SubValue2));

	__ReadCompRequireList( pQuest );
}
