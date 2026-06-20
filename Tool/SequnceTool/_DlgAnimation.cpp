// _DlgAnimation.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SequnceTool.h"
#include "_DlgAnimation.h"


// _DlgAnimation 대화 상자입니다.

IMPLEMENT_DYNCREATE(_DlgAnimation, CFormView)

_DlgAnimation::_DlgAnimation()
	: CFormView(_DlgAnimation::IDD)
{
	m_pSelectSequenceInfo = NULL;
	m_pCsSequenceMng = NULL;
	m_dwSelectSequenceInfo = INVALIDE_LISTID;
	m_dwSelectEventInfo = INVALIDE_LISTID;
}

_DlgAnimation::~_DlgAnimation()
{
}

void _DlgAnimation::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEQUENCE, m_list_Sequence);
	DDX_Control(pDX, IDC_LIST_EVENT, m_list_Event);
	DDX_Control(pDX, IDC_SLIDER_TIME, m_sld_Time);
	DDX_Control(pDX, IDC_EDIT_TIME, m_edit_Time);
	DDX_Control(pDX, IDC_CHECK_PLAY, m_chk_Play);
	DDX_Control(pDX, IDC_BUTTON_SETTIME, m_btn_SetTime);
	DDX_Control(pDX, IDC_RADIO_LOOP, m_rdo_Loop);
	DDX_Control(pDX, IDC_RADIO_ONCE, m_rdo_Once);	
	DDX_Control(pDX, IDC_CHECK_CHARPOS, m_chk_CharPos);
	DDX_Control(pDX, IDC_CHECK_GAMESCALE, m_chk_GameScale);
	DDX_Control(pDX, IDC_CHECK_USEOFFSET, m_chk_Offset);
	DDX_Control(pDX, IDC_EDIT_SPEED, m_edit_Speed);
	DDX_Control(pDX, IDC_SPIN_SPEED, m_spin_Speed);
	DDX_Control(pDX, IDC_CHECK_LOOPPLAY, m_chk_LoopPlay);
	DDX_Control(pDX, IDC_EDIT_OFFSET_X, m_edit_OffsetX);
	DDX_Control(pDX, IDC_SPIN_OFFSET_X, m_spin_OffsetX);
	DDX_Control(pDX, IDC_EDIT_OFFSET_Y, m_edit_OffsetY);
	DDX_Control(pDX, IDC_SPIN_OFFSET_Y, m_spin_OffsetY);
	DDX_Control(pDX, IDC_EDIT_OFFSET_Z, m_edit_OffsetZ);
	DDX_Control(pDX, IDC_SPIN_OFFSET_Z, m_spin_OffsetZ);
	DDX_Control(pDX, IDC_EDIT_EFFECTSCALE, m_edit_EffectScale);
	DDX_Control(pDX, IDC_SPIN_EFFECTSCALE, m_spin_EffectScale);
	DDX_Control(pDX, IDC_COMBO_STARTPOS, m_combo_StartPos);
	DDX_Control(pDX, IDC_EDIT_EFF0_X, m_edit_Eff0X);
	DDX_Control(pDX, IDC_SPIN_EFF0_X, m_spin_Eff0X);
	DDX_Control(pDX, IDC_EDIT_EFF0_Y, m_edit_Eff0Y);
	DDX_Control(pDX, IDC_SPIN_EFF0_Y, m_spin_Eff0Y);
	DDX_Control(pDX, IDC_EDIT_EFF0_Z, m_edit_Eff0Z);
	DDX_Control(pDX, IDC_SPIN_EFF0_Z, m_spin_Eff0Z);
	DDX_Control(pDX, IDC_EDIT_EFF1_X, m_edit_Eff1X);
	DDX_Control(pDX, IDC_SPIN_EFF1_X, m_spin_Eff1X);
	DDX_Control(pDX, IDC_EDIT_EFF1_Y, m_edit_Eff1Y);
	DDX_Control(pDX, IDC_SPIN_EFF1_Y, m_spin_Eff1Y);
	DDX_Control(pDX, IDC_EDIT_EFF1_Z, m_edit_Eff1Z);
	DDX_Control(pDX, IDC_SPIN_EFF1_Z, m_spin_Eff1Z);
	DDX_Control(pDX, IDC_EDIT_FADETIME, m_edit_FadeTime);
	DDX_Control(pDX, IDC_SPIN_FADETIME, m_spin_FadeTime);
	DDX_Control(pDX, IDC_CHECK_PLAN_VIEW, m_chk_PlanView);
	DDX_Control(pDX, IDC_CHECK_PLAN_TOWSIDE, m_chk_Plan_TwoSide);
	DDX_Control(pDX, IDC_CHECK_PLAN_WIRE, m_chk_Plan_Wire);
	DDX_Control(pDX, IDC_CHECK_VIEWOFFSET, m_chk_ViewOffset);
	DDX_Control(pDX, IDC_EDIT_ANILOOP, m_edit_LoopCnt);
	DDX_Control(pDX, IDC_COMBO_EVENTTYPE, m_EventType);
	DDX_Control(pDX, IDC_COMBO_DEL, m_ComboEventDel);
	DDX_Control(pDX, IDC_COMBO_EFF_LEVEL, m_cmb_EffLevel);
	DDX_Control(pDX, IDC_COMBO_ROT, m_Combo_Dir);
	DDX_Control(pDX, IDC_LIST_SHADER, m_list_Shader);
}


BEGIN_MESSAGE_MAP(_DlgAnimation, CFormView)
	ON_NOTIFY(NM_CLICK, IDC_LIST_SEQUENCE, &_DlgAnimation::OnNMClickListSequence)
	ON_WM_HSCROLL()
	ON_NOTIFY(NM_CLICK, IDC_LIST_EVENT, &_DlgAnimation::OnNMClickListEvent)
	ON_BN_CLICKED(IDC_BUTTON_SETTIME, &_DlgAnimation::OnBnClickedButtonSettime)
	ON_BN_CLICKED(IDC_BUTTON_NEW_EVENT, &_DlgAnimation::OnBnClickedButtonNewEvent)
	ON_BN_CLICKED(IDC_RADIO_LOOP, &_DlgAnimation::OnBnClickedRadioLoop)
	ON_BN_CLICKED(IDC_RADIO_ONCE, &_DlgAnimation::OnBnClickedRadioOnce)
	ON_EN_CHANGE(IDC_EDIT_SPEED, &_DlgAnimation::OnEnChangeEditSpeed)
	ON_BN_CLICKED(IDC_BUTTON_DEL_EVENT, &_DlgAnimation::OnBnClickedButtonDelEvent)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SEQUENCE, &_DlgAnimation::OnLvnItemchangedListSequence)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_EVENT, &_DlgAnimation::OnLvnItemchangedListEvent)
	ON_BN_CLICKED(IDC_CHECK_USEOFFSET, &_DlgAnimation::OnBnClickedCheckUseoffset)
	ON_EN_CHANGE(IDC_EDIT_OFFSET_X, &_DlgAnimation::OnEnChangeEditOffsetX)
	ON_EN_CHANGE(IDC_EDIT_OFFSET_Y, &_DlgAnimation::OnEnChangeEditOffsetY)
	ON_EN_CHANGE(IDC_EDIT_OFFSET_Z, &_DlgAnimation::OnEnChangeEditOffsetZ)
	ON_BN_CLICKED(IDC_CHECK_CHARPOS, &_DlgAnimation::OnBnClickedCheckCharPos)
	ON_BN_CLICKED(IDC_CHECK_GAMESCALE, &_DlgAnimation::OnBnClickedCheckGameScale)	
	ON_EN_CHANGE(IDC_EDIT_EFFECTSCALE, &_DlgAnimation::OnEnChangeEditEffectscale)	
	ON_CBN_SELCHANGE(IDC_COMBO_STARTPOS, &_DlgAnimation::OnCbnSelchangeComboStartpos)
	ON_EN_CHANGE(IDC_EDIT_EFF0_X, &_DlgAnimation::OnEnChangeEditEff0X)
	ON_EN_CHANGE(IDC_EDIT_EFF0_Y, &_DlgAnimation::OnEnChangeEditEff0Y)
	ON_EN_CHANGE(IDC_EDIT_EFF0_Z, &_DlgAnimation::OnEnChangeEditEff0Z)
	ON_EN_CHANGE(IDC_EDIT_EFF1_X, &_DlgAnimation::OnEnChangeEditEff1X)
	ON_EN_CHANGE(IDC_EDIT_EFF1_Y, &_DlgAnimation::OnEnChangeEditEff1Y)
	ON_EN_CHANGE(IDC_EDIT_EFF1_Z, &_DlgAnimation::OnEnChangeEditEff1Z)
	ON_EN_CHANGE(IDC_EDIT_FADETIME, &_DlgAnimation::OnEnChangeEditFadeTime)
	ON_BN_CLICKED(IDC_CHECK_PLAN_TOWSIDE, &_DlgAnimation::OnBnClickedCheckPlanTowside)
	ON_BN_CLICKED(IDC_CHECK_PLAN_WIRE, &_DlgAnimation::OnBnClickedCheckPlanWire)
	ON_EN_CHANGE(IDC_EDIT_ANILOOP, &_DlgAnimation::OnEnChangeEditAniloop)
	ON_CBN_SELCHANGE(IDC_COMBO_EVENTTYPE, &_DlgAnimation::OnCbnSelchangeComboEventtype)
	ON_CBN_SELCHANGE(IDC_COMBO_DEL, &_DlgAnimation::OnCbnSelchangeComboDel)
	ON_CBN_SELCHANGE(IDC_COMBO_EFF_LEVEL, &_DlgAnimation::OnCbnSelchangeComboEffLevel)
	ON_CBN_SELCHANGE(IDC_COMBO_ROT, &_DlgAnimation::OnCbnSelchangeComboRot)
	ON_BN_CLICKED(IDC_BUTTON_DEL_EFFECT, &_DlgAnimation::OnBnClickedButtonDelEffect)
	ON_BN_CLICKED(IDC_BUTTON_NEW_SHADER, &_DlgAnimation::OnBnClickedButtonNewShader)
	ON_BN_CLICKED(IDC_BUTTON_DEL_SHADER, &_DlgAnimation::OnBnClickedButtonDelShader)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SHADER, &_DlgAnimation::OnNMDblclkListShader)
END_MESSAGE_MAP()

void _DlgAnimation::_Delete()
{
	__ListDeleteAllItem();	
}

void _DlgAnimation::_Init()
{
	DWORD dwExStyle = m_list_Sequence.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Sequence.SetExtendedStyle( dwExStyle );
	m_list_Sequence.InsertColumn( 0, L"Name", LVCFMT_RIGHT, 115 );
	m_list_Sequence.InsertColumn( 1, L"Evt", LVCFMT_CENTER, 35 );
	m_list_Sequence.InsertColumn( 2, L"Loop", LVCFMT_CENTER, 38 );
	m_list_Sequence.InsertColumn( 3, L"Time", LVCFMT_CENTER, 45 );
	m_list_Sequence.InsertColumn( 4, L"SeqID", LVCFMT_LEFT, 55 );

	dwExStyle = m_list_Event.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Event.SetExtendedStyle( dwExStyle );
	m_list_Event.InsertColumn( 0, L"T", LVCFMT_RIGHT, 20 );
	m_list_Event.InsertColumn( 1, L"I", LVCFMT_CENTER, 75 );
	m_list_Event.InsertColumn( 2, L"Text", LVCFMT_RIGHT, 145 );
	m_list_Event.InsertColumn( 3, L"Time", LVCFMT_CENTER, 50 );

	dwExStyle = m_list_Shader.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Shader.SetExtendedStyle( dwExStyle );
	m_list_Shader.InsertColumn( 0, L"Shader", LVCFMT_RIGHT, 200 );

	m_Combo_Dir.InsertString( 0, L"방향성 없음" );
	m_Combo_Dir.SetItemData( 0, nsEFFECT::DIR_NONE );
	m_Combo_Dir.InsertString( 1, L"캐릭터 방향" );
	m_Combo_Dir.SetItemData( 1, nsEFFECT::DIR_CHAR );
	m_Combo_Dir.InsertString( 2, L"카메라 x축" );
	m_Combo_Dir.SetItemData( 2, nsEFFECT::DIR_CAMERA_AXIS_Z );
	m_Combo_Dir.InsertString( 3, L"캐릭터 모든방향" );
	m_Combo_Dir.SetItemData( 3, nsEFFECT::DIR_CHAR_ALL );
	m_Combo_Dir.SetCurSel( 0 );

	m_combo_StartPos.InsertString( 0, L"케릭터위치" );
	m_combo_StartPos.SetItemData( 0, nsEFFECT::SPOS_CHARWORLD );
	m_combo_StartPos.InsertString( 1, L"모델중심" );
	m_combo_StartPos.SetItemData( 1, nsEFFECT::SPOS_BOUND_CENTER );
	m_combo_StartPos.InsertString( 2, L"툴높이값중심" );
	m_combo_StartPos.SetItemData( 2, nsEFFECT::SPOS_HALF_THEIGHT );
	m_combo_StartPos.InsertString( 3, L"툴높이값" );
	m_combo_StartPos.SetItemData( 3, nsEFFECT::SPOS_THEIGHT );
	m_combo_StartPos.SetCurSel( 0 );

	m_EventType.InsertString( 0, L"매번실행" );
	m_EventType.SetItemData( 0, nsEFFECT::EVENT_NONE );
	m_EventType.InsertString( 1, L"변경전까지 한번만" );
	m_EventType.SetItemData( 1, nsEFFECT::EVENT_ONCE );
	m_EventType.InsertString( 2, L"종료시까지 한번만" );
	m_EventType.SetItemData( 2, nsEFFECT::EVENT_ONLY_ONE );	
	m_EventType.SetCurSel( 0 );

	m_ComboEventDel.InsertString( 0, L"자동삭제" );
	m_ComboEventDel.SetItemData( 0, nsEFFECT::EVENT_DEL_NONE );
	m_ComboEventDel.InsertString( 1, L"애니메이션 변경시삭제" );
	m_ComboEventDel.SetItemData( 1, nsEFFECT::EVENT_DEL_ANI );	
	m_ComboEventDel.SetCurSel( 0 );

	m_cmb_EffLevel.InsertString( 0, L"없음" );
	m_cmb_EffLevel.InsertString( 1, L"46" );
	m_cmb_EffLevel.InsertString( 2, L"51" );
	m_cmb_EffLevel.InsertString( 3, L"56" );
	m_cmb_EffLevel.InsertString( 4, L"61" );
	m_cmb_EffLevel.InsertString( 5, L"65" );
	m_cmb_EffLevel.InsertString( 6, L"70" );
	m_cmb_EffLevel.InsertString( 7, L"75" );
	m_cmb_EffLevel.SetCurSel( 0 );

	m_spin_Speed.SetCsBuddy( &m_edit_Speed, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_edit_Speed, 1.0f, 3 );

	m_spin_OffsetX.SetCsBuddy( &m_edit_OffsetX, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_edit_OffsetX, 0.0f, 3 );
	m_spin_OffsetY.SetCsBuddy( &m_edit_OffsetY, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_edit_OffsetY, 0.0f, 3 );
	m_spin_OffsetZ.SetCsBuddy( &m_edit_OffsetZ, 0.001f, 3 );
	nsCsMfcFunc::Float2Control( &m_edit_OffsetZ, 0.0f, 3 );

	m_spin_Eff0X.SetCsBuddy( &m_edit_Eff0X, 0.01f, 2 );
	m_spin_Eff0Y.SetCsBuddy( &m_edit_Eff0Y, 0.01f, 2 );
	m_spin_Eff0Z.SetCsBuddy( &m_edit_Eff0Z, 0.01f, 2 );
	m_spin_Eff1X.SetCsBuddy( &m_edit_Eff1X, 0.01f, 2 );
	m_spin_Eff1Y.SetCsBuddy( &m_edit_Eff1Y, 0.01f, 2 );
	m_spin_Eff1Z.SetCsBuddy( &m_edit_Eff1Z, 0.01f, 2 );

	m_spin_FadeTime.SetCsBuddy( &m_edit_FadeTime, 0.001f, 3 );

	m_spin_EffectScale.SetCsBuddy( &m_edit_EffectScale, 0.01f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_EffectScale, 1.0f, 2 );

	__EnableOffset( FALSE );

	m_chk_ViewOffset.SetCheck( BST_CHECKED );
	m_rdo_Once.SetCheck( BST_CHECKED );
	m_chk_PlanView.SetCheck( BST_CHECKED );

	_ReleaseTarget();		
}

void _DlgAnimation::_Update()
{
	static float fOldTimeInSec = NiGetCurrentTimeInSec();
	float fCurTimeInSec = NiGetCurrentTimeInSec();
	m_fDeltaTime = fCurTimeInSec - fOldTimeInSec;
	fOldTimeInSec = fCurTimeInSec;

	CAMERA_ST.Update_ByTool( m_fDeltaTime, false );
	m_fDeltaTime *= CsMax( 0.002f, nsCsMfcFunc::Control2Float( &m_edit_Speed ) );

	g_fAccumTime += m_fDeltaTime;

	if( m_pTarget == NULL )
		return;

	if( m_pTarget->IsLoad() == false )
		return;

	if( m_bReadInfo == false )
	{
		__ReadTargetInfo();
		m_bReadInfo = true;
	}
	else if( g_pMainWnd->_GetDlgEtc()->_IsAction() == true )
	{
		m_sld_Time.SetPos( 0 );
		m_edit_Time.SetWindowText( L"" );
	}
	else if( ( m_pCsSequenceMng )&&( m_chk_Play.GetCheck() == BST_CHECKED ) )
	{
		int nPos = (int)( m_pCsSequenceMng->GetCurTime()*1000.0f );
	
		m_sld_Time.SetPos( nPos );

		TCHAR sz[ 64 ];
		_stprintf_s( sz, 64, L"%.3f / %.3f", m_pCsSequenceMng->GetCurTime(), m_pCsSequenceMng->GetEndTime() );
		m_edit_Time.SetWindowText( sz );		
	}
}

float _DlgAnimation::_GetDeltaTime()
{
	return m_fDeltaTime;
}

float _DlgAnimation::_GetAniTime()
{
	if( ( m_list_Sequence.GetFirstSelectedItemPosition() == NULL )&&( g_pMainWnd->_GetDlgEtc()->_IsAction() == false ) )
		return 0.0f;

	if( m_chk_Play.GetCheck() == BST_UNCHECKED )
	{
		float fReturn = m_fAniDeltaTime;
		m_fAniDeltaTime = 0.0f;
		return fReturn;
	}

	m_fAniDeltaTime = 0.0f;
	return m_fDeltaTime;
}

void _DlgAnimation::_ReleaseTarget()
{
	__ListDeleteAllItem();

	g_ViewModel.ReleaseSpline();
	__SetShowControl_Eff0_Eff1( SW_HIDE );
	m_pCsSequenceMng = NULL;
	m_pTarget = NULL;
	m_bReadInfo = false;

	m_fAniDeltaTime = 0.0f;
}

void _DlgAnimation::_ResetTarget( CsC_AvObject* pActor )
{
	_ReleaseTarget();

	m_pTarget = pActor;
	m_bReadInfo = false;

	OnCbnSelchangeComboEffLevel();
}

void _DlgAnimation::__ReadTargetInfo()
{
	m_list_Sequence.EnableWindow( TRUE );
	__ListDeleteAllItem();

	__WriteSequenceList();
}

void _DlgAnimation::__WriteSequenceList( bool bSelectExist )
{
	POSITION pos = m_list_Sequence.GetFirstSelectedItemPosition();
	int nSelect = m_list_Sequence.GetNextSelectedItem( pos );
	
	m_list_Sequence.DeleteAllItems();

	NiActorManager* pActorMng = m_pTarget->GetProp_Animation()->GetActorMng();
	m_pCsSequenceMng = m_pTarget->GetProp_Animation()->GetSequenceMng();

	std::map< DWORD, CsC_LoadSequence::sSEQUENCE_INFO* >* pMap = m_pCsSequenceMng->GetLoadSequence()->GetSequenceMap();
	std::map< DWORD, CsC_LoadSequence::sSEQUENCE_INFO* >::iterator it = pMap->begin();
	std::map< DWORD, CsC_LoadSequence::sSEQUENCE_INFO* >::iterator itEnd = pMap->end();
	TCHAR szName[ OBJECT_NAME_LEN ];
	TCHAR szInt[ 32 ];
	TCHAR* szShotName;
	for( ; it!=itEnd; ++it )
	{
		NiControllerSequence* pNiSequence = pActorMng->GetSequence( it->first );		

		// 리스트에 데이터 기입
		M2W( szName, pNiSequence->GetName(), OBJECT_NAME_LEN );
		size_t nLen = _tcslen( szName );
		szShotName = NULL;
		for( size_t i=0; i<nLen; ++i )
		{
			if( szName[ i ] == '_' )
			{
				szShotName = &szName[ i+1 ];
				break;
			}
		}
		int nIndex = m_list_Sequence.GetItemCount();
		m_list_Sequence.InsertItem( nIndex, szShotName != NULL ? szShotName : szName );

		_stprintf_s( szInt, 32, L"%d", it->second->s_vpTextKeyEvent.Size() );
		m_list_Sequence.SetItem( nIndex, 1, LVIF_TEXT, szInt, 0, 0, 0, 0 );
		m_list_Sequence.SetItem( nIndex, 2, LVIF_TEXT, it->second->s_eCyleType == NiTimeController::LOOP ? L"O" : L"X", 0, 0, 0, 0 );
		_stprintf_s( szInt, 32, L"%.2f", it->second->s_fEndTime );
		m_list_Sequence.SetItem( nIndex, 3, LVIF_TEXT, szInt, 0, 0, 0, 0 );
		_stprintf_s( szInt, 32, L"%d", it->first );
		m_list_Sequence.SetItem( nIndex, 4, LVIF_TEXT, szInt, 0, 0, 0, 0 );
		m_list_Sequence.SetItemData( nIndex, it->first );		
	}

	if( ( nSelect >= 0 )&&( bSelectExist == true ) )
	{
		m_list_Sequence.SetItemState( nSelect, LVIS_SELECTED, LVIS_SELECTED );
	}
}

void _DlgAnimation::__ListDeleteAllItem()
{
	m_list_Sequence.DeleteAllItems();
	__ShaderDeleteAllItem();
	__EventDeleteAllItem();
}

void _DlgAnimation::OnNMClickListSequence(NMHDR *pNMHDR, LRESULT *pResult)
{
	__ShaderDeleteAllItem();
	__EventDeleteAllItem();

	POSITION pos = m_list_Sequence.GetFirstSelectedItemPosition();
	int nSelect = m_list_Sequence.GetNextSelectedItem( pos );
	if( nSelect < 0 )
	{
		m_dwSelectSequenceInfo = INVALIDE_LISTID;
		return;
	}

	g_pMainWnd->_GetDlgEtc()->_ReleaseAction();	

	if( m_pTarget->GetProp_Animation()->GetAnimationID() == m_dwSelectSequenceInfo )
		return;

	m_dwSelectSequenceInfo = (DWORD)m_list_Sequence.GetItemData( nSelect );
	m_pTarget->SetAnimation( m_dwSelectSequenceInfo );	
	m_chk_Play.SetCheck( BST_CHECKED );

	m_pSelectSequenceInfo = m_pCsSequenceMng->GetSequence( m_dwSelectSequenceInfo );
	m_sld_Time.SetRange( 0, (int)( m_pSelectSequenceInfo->s_fEndTime*1000.0f ) );

	__ResetLoopCount();
	__ResetShaderList();
	__ResetEventList();
	
	*pResult = 0;
}

void _DlgAnimation::OnLvnItemchangedListSequence(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	/*LRESULT result;
	OnNMClickListSequence( NULL, &result );*/	
	
	*pResult = 0;
}


void _DlgAnimation::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_Time.GetSafeHwnd() )
	{
		if( m_list_Sequence.GetFirstSelectedItemPosition() != NULL )
		{			
			//if( g_ViewModel.GetSrcMode()->GetProp_Animation()->GetSequenceMng()->IsStartTimeSetting() == true )
			{
				m_chk_Play.SetCheck( BST_UNCHECKED );
				m_fAniDeltaTime = m_sld_Time.GetPos()*0.001f - m_pCsSequenceMng->GetCurTime();

				TCHAR sz[ 64 ];
				_stprintf_s( sz, 64, L"%.3f / %.3f", m_pCsSequenceMng->GetCurTime(), m_pCsSequenceMng->GetEndTime() );
				m_edit_Time.SetWindowText( sz );
			}			
		}
	}

	CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
}




void _DlgAnimation::__EventDeleteAllItem()
{
	m_btn_SetTime.EnableWindow( FALSE );

	if( m_pSelectSequenceInfo == NULL )
	{
		assert_cs( m_list_Event.GetItemCount() == 0 );
		assert_cs( m_dwSelectSequenceInfo == INVALIDE_LISTID );
		return;
	}

	m_pSelectSequenceInfo = NULL;
	m_dwSelectSequenceInfo = INVALIDE_LISTID;
	m_dwSelectEventInfo = INVALIDE_LISTID;

	m_list_Event.DeleteAllItems();
}

void _DlgAnimation::__ResetEventList()
{
	assert_cs( m_pSelectSequenceInfo != NULL );	
	assert_cs( m_dwSelectSequenceInfo != INVALIDE_LISTID );
	assert_cs( m_list_Event.GetItemCount() == 0 );

	__WriteEventList();	
}

void _DlgAnimation::__WriteEventList()
{
	m_list_Event.DeleteAllItems();

	TCHAR sz[ 128 ];
	int nSize = m_pSelectSequenceInfo->s_vpTextKeyEvent.Size();
	for( int i=0; i<nSize; ++i )
	{
		CsC_LoadSequence::sTEXTKEY_EVENT* pEvent = m_pSelectSequenceInfo->s_vpTextKeyEvent[ i ];
		int nIndex = m_list_Event.GetItemCount();

		switch( pEvent->s_eType )
		{
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_E_FILE:
			{
				m_list_Event.InsertItem( nIndex, L"Ef" );
				m_list_Event.SetItem( nIndex, 1, LVIF_TEXT, L"파일", 0, 0, 0, 0 );
			}
			break;
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC:
			{
				m_list_Event.InsertItem( nIndex, L"Ef" );
				switch( pEvent->s_nStaticIndex )
				{
				case nsSTATIC_EVENT::DYE_NORMAL_ATTACK:
					m_list_Event.SetItem( nIndex, 1, LVIF_TEXT, L"일반어택", 0, 0, 0, 0 );
					break;
				case nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE:
					m_list_Event.SetItem( nIndex, 1, LVIF_TEXT, L"파일어택", 0, 0, 0, 0 );
					break;
				case nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE:
					m_list_Event.SetItem( nIndex, 1, LVIF_TEXT, L"일반어택파일", 0, 0, 0, 0 );
					break;
				case nsSTATIC_EVENT::DYE_RANGE_ATTACK_START:
					m_list_Event.SetItem( nIndex, 1, LVIF_TEXT, L"Range시작", 0, 0, 0, 0 );
					break;
				case nsSTATIC_EVENT::DYE_RANGE_ATTACK_FILE:
					m_list_Event.SetItem( nIndex, 1, LVIF_TEXT, L"Range파일", 0, 0, 0, 0 );
					break;
				case nsSTATIC_EVENT::DYE_RANGE_ATTACK_END:
					m_list_Event.SetItem( nIndex, 1, LVIF_TEXT, L"Range끝", 0, 0, 0, 0 );
					break;
				default:
					assert_cs( false );
				}				
			}
			break;
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_S_FILE:
			{
				m_list_Event.InsertItem( nIndex, L"So" );
				m_list_Event.SetItem( nIndex, 1, LVIF_TEXT, L"Fsound", 0, 0, 0, 0 );
			}
			break;
		case CsC_LoadSequence::sTEXTKEY_EVENT::ET_DS_STATIC:
			{
				m_list_Event.InsertItem( nIndex, L"So" );
				m_list_Event.SetItem( nIndex, 1, LVIF_TEXT, L"Ssound", 0, 0, 0, 0 );
			}
			break;
		default:
			assert_cs( false );
		}

		M2W( sz, pEvent->s_cText, 128 );		
		m_list_Event.SetItem( nIndex, 2, LVIF_TEXT, nsCSFILE::GetFileName( sz ), 0, 0, 0, 0 );

		_stprintf_s( sz, 128, L"%.3f", pEvent->s_fEventTime );
		m_list_Event.SetItem( nIndex, 3, LVIF_TEXT, sz, 0, 0, 0, 0 );
	}	
}

void _DlgAnimation::OnNMClickListEvent(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_Event.GetFirstSelectedItemPosition();
	int nSelect = m_list_Event.GetNextSelectedItem( pos );
	if( nSelect < 0 )
	{
		m_dwSelectEventInfo = INVALIDE_LISTID;
		m_btn_SetTime.EnableWindow( FALSE );
		g_ViewModel.ReleaseSpline();
		__SetShowControl_Eff0_Eff1( SW_HIDE );
		return;
	}

	m_dwSelectEventInfo = nSelect;

	// 같은거 두번 클릭 하면 그쪽으로 바를 보낸다.
	//if( m_dwSelectEventInfo == nSelect )
	{
		m_sld_Time.SetPos( (int)( m_pSelectSequenceInfo->s_vpTextKeyEvent[ m_dwSelectEventInfo ]->s_fEventTime*1000.0f ) );
		m_fAniDeltaTime = m_sld_Time.GetPos()*0.001f - m_pCsSequenceMng->GetCurTime();
		TCHAR sz[ 64 ];
		_stprintf_s( sz, 64, L"%.3f / %.3f", m_sld_Time.GetPos()*0.001f, m_pCsSequenceMng->GetEndTime() );
		m_edit_Time.SetWindowText( sz );

		m_chk_Play.SetCheck( BST_UNCHECKED );
	}	

	m_btn_SetTime.EnableWindow( TRUE );	
	__SetEventControl();

	CsC_LoadSequence::sTEXTKEY_EVENT* pTextKey = m_pSelectSequenceInfo->s_vpTextKeyEvent[ m_dwSelectEventInfo ];
	if( ( pTextKey->s_eType == CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC )&&
		( pTextKey->s_nStaticIndex == nsSTATIC_EVENT::DYE_RANGE_ATTACK_FILE ) )
	{
		g_ViewModel.SetSplinePos( pTextKey->s_vValue1, pTextKey->s_vValue2 );
		__SetShowControl_Eff0_Eff1( SW_SHOW );
		__SetEffect0( pTextKey->s_vValue1 );
		__SetEffect1( pTextKey->s_vValue2 );
	}
	else
	{
		g_ViewModel.ReleaseSpline();
		__SetShowControl_Eff0_Eff1( SW_HIDE );
	}
}



void _DlgAnimation::OnLvnItemchangedListEvent(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	
	/*LRESULT result;
	OnNMClickListEvent( NULL, &result );*/	

	*pResult = 0;
}


void _DlgAnimation::OnBnClickedButtonSettime()
{
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	if( pModelData->IsSequence( m_dwSelectSequenceInfo ) == false )
	{
		pModelData->AddSequence( m_dwSelectSequenceInfo );
	}
	pModelData->GetSequence( m_dwSelectSequenceInfo )->GetEvent( m_dwSelectEventInfo )->s_fEventTime = m_sld_Time.GetPos()*0.001f;

	// 그 정보를 LoadSequence 쪽에 Apply시킨다.	
	m_pCsSequenceMng->GetLoadSequence()->ResetApplyModelData( m_pTarget->GetModelID() );

	__WriteEventList();
}

void _DlgAnimation::OnBnClickedButtonNewEvent()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		CsMessageBox( MB_OK, L"애니메이션을 클릭하셔야 이벤트를 추가 할 수 있습니다." );
		return;
	}

	assert_cs( m_pTarget != NULL );
	assert_cs( m_pCsSequenceMng != NULL );

	int nType;
	char cTextKey[ MAX_TEXTKEY_LEN ];
	int nStaticIndex;

	_DlgInsertEvent ie( &nType, cTextKey, &nStaticIndex );
	if( ie.DoModal() == IDCANCEL )
		return;

	// 모델데이터 쪽에 정보 넣고
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	if( pModelData->IsSequence( m_dwSelectSequenceInfo ) == false )
	{
		pModelData->AddSequence( m_dwSelectSequenceInfo );
	}

	CsModelData::sEVENT* pEvent = NiNew CsModelData::sEVENT;	
	pEvent->s_nType = nType;
	pEvent->s_nStaticIndex = nStaticIndex;
	pEvent->s_fEventTime = m_sld_Time.GetPos()*0.001f;
	strcpy_s( pEvent->s_cText, MAX_TEXTKEY_LEN, cTextKey );

	pEvent->s_dwPlag &= ~ nsEFFECT::LIVE_MASK;
	pEvent->s_dwPlag |= m_rdo_Loop.GetCheck() == BST_CHECKED ? nsEFFECT::LIVE_LOOP : nsEFFECT::LIVE_ONCE;
	
	pEvent->s_dwPlag &= ~ nsEFFECT::POS_MASK;
	pEvent->s_dwPlag |= m_chk_CharPos.GetCheck() == BST_CHECKED ? nsEFFECT::POS_CHARPOS : nsEFFECT::POS_NORMAL;	

	pEvent->s_dwPlag &= ~ nsEFFECT::OFFSET_MASK;
	pEvent->s_dwPlag |= m_chk_Offset.GetCheck() == BST_CHECKED ? nsEFFECT::OFFSET_USE : nsEFFECT::OFFSET_NONE;	

	pEvent->s_dwPlag &= ~ nsEFFECT::DIR_MASK;
	pEvent->s_dwPlag |= m_Combo_Dir.GetItemData( 0 );

	pEvent->s_dwPlag &= ~ nsEFFECT::SPOS_MASK;
	pEvent->s_dwPlag |= m_combo_StartPos.GetItemData( 0 );

	pEvent->s_dwPlag &= ~ nsEFFECT::EVENT_TYPE_MASK;
	pEvent->s_dwPlag |= m_EventType.GetItemData( 0 );

	pEvent->s_dwPlag &= ~ nsEFFECT::EVENT_DEL_MASK;
	pEvent->s_dwPlag |= m_ComboEventDel.GetItemData( 0 );

	pEvent->s_fEffectScale = nsCsMfcFunc::Control2Float( &m_edit_EffectScale );	
	pEvent->s_bParentScale = ( m_chk_GameScale.GetCheck() == BST_CHECKED );

	if( m_chk_Offset.GetCheck() == BST_CHECKED )
	{
		pEvent->s_vOffset = _GetOffset();
	}

	pModelData->GetSequence( m_dwSelectSequenceInfo )->InsertEvent( pEvent );

	// 그 정보를 LoadSequence 쪽에 Apply시킨다.	
	m_pCsSequenceMng->GetLoadSequence()->ResetApplyModelData( m_pTarget->GetModelID() );
	
	__WriteSequenceList( true );
	__WriteEventList();
}


void _DlgAnimation::OnBnClickedButtonDelEvent()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		CsMessageBox( MB_OK, L"애니메이션을 클릭하셔야 합니다." );
		return;
	}

	if( m_dwSelectEventInfo == INVALIDE_LISTID )
		return;

	assert_cs( m_pTarget != NULL );
	assert_cs( m_pCsSequenceMng != NULL );

	// 모델데이터 쪽에 정보 넣고
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	pModelData->GetSequence( m_dwSelectSequenceInfo )->DeleteEvent( m_dwSelectEventInfo );

	// 그 정보를 LoadSequence 쪽에 Apply시킨다.	
	m_pCsSequenceMng->GetLoadSequence()->ResetApplyModelData( m_pTarget->GetModelID() );

	__WriteSequenceList( true );
	__WriteEventList();

	m_dwSelectEventInfo = INVALIDE_LISTID;
}

void _DlgAnimation::OnBnClickedRadioLoop()
{
	__ChangeEvent();
}

void _DlgAnimation::OnBnClickedRadioOnce()
{
	__ChangeEvent();
}

void _DlgAnimation::OnBnClickedCheckCharPos()
{
	__ChangeEvent();
}

void _DlgAnimation::OnBnClickedCheckGameScale()
{
	__ChangeEvent();
}

void _DlgAnimation::OnCbnSelchangeComboStartpos()
{
	__ChangeEvent();
}

void _DlgAnimation::OnCbnSelchangeComboEventtype()
{
	__ChangeEvent();
}

void _DlgAnimation::OnCbnSelchangeComboDel()
{
	__ChangeEvent();
}

void _DlgAnimation::OnCbnSelchangeComboRot()
{
	__ChangeEvent();
}

void _DlgAnimation::OnEnChangeEditFadeTime()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_FadeTime.GetSafeHwnd() )
		return;

	if( nsCsMfcFunc::Control2Float( &m_edit_FadeTime ) < 0.02f )
		nsCsMfcFunc::Float2Control( &m_edit_FadeTime, 0.02f );

	__ChangeEvent();
}

void _DlgAnimation::__ChangeEvent()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		return;
	}

	if( m_dwSelectEventInfo == INVALIDE_LISTID )
	{
		return;
	}

	// 모델데이터 쪽에 정보 넣고
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	CsModelData::sEVENT* pModelEvent = pModelData->GetSequence( m_dwSelectSequenceInfo )->GetEvent( m_dwSelectEventInfo );

	pModelEvent->s_dwPlag &= ~ nsEFFECT::LIVE_MASK;
	pModelEvent->s_dwPlag |= m_rdo_Loop.GetCheck() == BST_CHECKED ? nsEFFECT::LIVE_LOOP : nsEFFECT::LIVE_ONCE;

	pModelEvent->s_dwPlag &= ~ nsEFFECT::POS_MASK;
	pModelEvent->s_dwPlag |= m_chk_CharPos.GetCheck() == BST_CHECKED ? nsEFFECT::POS_CHARPOS : nsEFFECT::POS_NORMAL;	

	pModelEvent->s_dwPlag &= ~ nsEFFECT::OFFSET_MASK;
	pModelEvent->s_dwPlag |= m_chk_Offset.GetCheck() == BST_CHECKED ? nsEFFECT::OFFSET_USE : nsEFFECT::OFFSET_NONE;

	pModelEvent->s_dwPlag &= ~ nsEFFECT::DIR_MASK;
	pModelEvent->s_dwPlag |= m_Combo_Dir.GetItemData( m_Combo_Dir.GetCurSel() );

	pModelEvent->s_dwPlag &= ~ nsEFFECT::SPOS_MASK;
	pModelEvent->s_dwPlag |= m_combo_StartPos.GetItemData( m_combo_StartPos.GetCurSel() );

	pModelEvent->s_dwPlag &= ~ nsEFFECT::EVENT_TYPE_MASK;
	pModelEvent->s_dwPlag |= m_EventType.GetItemData( m_EventType.GetCurSel() );

	pModelEvent->s_dwPlag &= ~ nsEFFECT::EVENT_DEL_MASK;
	pModelEvent->s_dwPlag |= m_ComboEventDel.GetItemData( m_ComboEventDel.GetCurSel() );

	if( m_chk_Offset.GetCheck() == BST_CHECKED )
	{
		pModelEvent->s_vOffset = _GetOffset();
	}

	pModelEvent->s_fEffectScale = nsCsMfcFunc::Control2Float( &m_edit_EffectScale );
	if( pModelEvent->s_fEffectScale == 0.0f )
		pModelEvent->s_fEffectScale = 1.0f;

	pModelEvent->s_fFadeoutTime = nsCsMfcFunc::Control2Float( &m_edit_FadeTime );
	pModelEvent->s_bParentScale = ( m_chk_GameScale.GetCheck() == BST_CHECKED );

	CsC_LoadSequence::sTEXTKEY_EVENT* pTextKey = m_pCsSequenceMng->GetLoadSequence()->GetSequence( m_dwSelectSequenceInfo )->s_vpTextKeyEvent[ m_dwSelectEventInfo ];
	pTextKey->s_dwPlag = pModelEvent->s_dwPlag;	
	pTextKey->s_vOffset = pModelEvent->s_vOffset;
	pTextKey->s_fEffectScale = pModelEvent->s_fEffectScale;
	pTextKey->s_fFadeoutTime = pModelEvent->s_fFadeoutTime;
}

void _DlgAnimation::__SetEventControl()
{
	assert_cs( m_pSelectSequenceInfo != NULL );
	assert_cs( m_dwSelectEventInfo != INVALIDE_LISTID );

	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	CsModelData::sEVENT* pModelEvent = pModelData->GetSequence( m_dwSelectSequenceInfo )->GetEvent( m_dwSelectEventInfo );

	DWORD dwPlag = pModelEvent->s_dwPlag;

	if( ( dwPlag & nsEFFECT::LIVE_LOOP ) == nsEFFECT::LIVE_LOOP )
	{
		m_rdo_Loop.SetCheck( BST_CHECKED );
		m_rdo_Once.SetCheck( BST_UNCHECKED );
	}
	else
	{
		m_rdo_Loop.SetCheck( BST_UNCHECKED );
		m_rdo_Once.SetCheck( BST_CHECKED );
	}

	int nBtnState;
	nBtnState = ( ( dwPlag & nsEFFECT::POS_MASK ) == nsEFFECT::POS_CHARPOS ) ? BST_CHECKED : BST_UNCHECKED;
	m_chk_CharPos.SetCheck( nBtnState );	

	nBtnState = ( ( dwPlag & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE ) ? BST_CHECKED : BST_UNCHECKED;
	m_chk_Offset.SetCheck( nBtnState );

	nsCsMfcFunc::ComboSelect_DWORD_Data( &m_combo_StartPos, pModelEvent->s_dwPlag & nsEFFECT::SPOS_MASK );
	nsCsMfcFunc::ComboSelect_DWORD_Data( &m_Combo_Dir, pModelEvent->s_dwPlag & nsEFFECT::DIR_MASK );	
	nsCsMfcFunc::ComboSelect_DWORD_Data( &m_EventType, pModelEvent->s_dwPlag & nsEFFECT::EVENT_TYPE_MASK );
	nsCsMfcFunc::ComboSelect_DWORD_Data( &m_ComboEventDel, pModelEvent->s_dwPlag & nsEFFECT::EVENT_DEL_MASK );	

	nBtnState = pModelEvent->s_bParentScale == true ? BST_CHECKED : BST_UNCHECKED;
	m_chk_GameScale.SetCheck( nBtnState );

	nsCsMfcFunc::Float2Control( &m_edit_FadeTime, pModelEvent->s_fFadeoutTime, 3 );

	if( m_chk_Offset.GetCheck() == BST_CHECKED )
	{
		__EnableOffset( TRUE );
		__SetOffset( pModelEvent->s_vOffset );
	}
	else
	{
		__SetOffset( NiPoint3::ZERO );
		__EnableOffset( FALSE );		
	}

	nsCsMfcFunc::Float2Control( &m_edit_EffectScale, pModelEvent->s_fEffectScale );
}


void _DlgAnimation::OnEnChangeEditSpeed()
{
	if( GetFocus()->GetSafeHwnd() == m_edit_Speed.GetSafeHwnd() )
	{		
	}
}


void _DlgAnimation::__EnableOffset( BOOL bEnable )
{
	m_edit_OffsetX.EnableWindow( bEnable );
	m_spin_OffsetX.EnableWindow( bEnable );
	m_edit_OffsetY.EnableWindow( bEnable );
	m_spin_OffsetY.EnableWindow( bEnable );
	m_edit_OffsetZ.EnableWindow( bEnable );
	m_spin_OffsetZ.EnableWindow( bEnable );
}

NiPoint3 _DlgAnimation::_GetOffset()
{
	return NiPoint3(	nsCsMfcFunc::Control2Float( &m_edit_OffsetX )*100.0f,
						nsCsMfcFunc::Control2Float( &m_edit_OffsetY )*100.0f,
						nsCsMfcFunc::Control2Float( &m_edit_OffsetZ )*100.0f	);
}

void _DlgAnimation::__SetOffset( NiPoint3 vOffset )
{
	nsCsMfcFunc::Float2Control( &m_edit_OffsetX, vOffset.x*0.01f );
	nsCsMfcFunc::Float2Control( &m_edit_OffsetY, vOffset.y*0.01f );
	nsCsMfcFunc::Float2Control( &m_edit_OffsetZ, vOffset.z*0.01f );
}

void _DlgAnimation::OnBnClickedCheckUseoffset()
{
	__EnableOffset( m_chk_Offset.GetCheck() == BST_CHECKED );

	__ChangeEvent();
}

void _DlgAnimation::OnEnChangeEditOffsetX()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_OffsetX.GetSafeHwnd() )
		return;

	__ChangeOffset();
}

void _DlgAnimation::OnEnChangeEditOffsetY()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_OffsetY.GetSafeHwnd() )
		return;

	__ChangeOffset();
}

void _DlgAnimation::OnEnChangeEditOffsetZ()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_OffsetZ.GetSafeHwnd() )
		return;

	__ChangeOffset();
}

void _DlgAnimation::__ChangeOffset()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		return;
	}

	if( m_dwSelectEventInfo == INVALIDE_LISTID )
	{
		return;
	}

	assert_cs( m_chk_Offset.GetCheck() == BST_CHECKED );

	// 모델데이터 쪽에 정보 넣고
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	CsModelData::sEVENT* pModelEvent = pModelData->GetSequence( m_dwSelectSequenceInfo )->GetEvent( m_dwSelectEventInfo );
	pModelEvent->s_vOffset = _GetOffset();

	// 시퀀스 쪽에 정보 넣는다
	CsC_LoadSequence::sTEXTKEY_EVENT* pTextKey = m_pCsSequenceMng->GetLoadSequence()->GetSequence( m_dwSelectSequenceInfo )->s_vpTextKeyEvent[ m_dwSelectEventInfo ];
	pTextKey->s_vOffset = pModelEvent->s_vOffset;

	// 지금 있는 이펙트 중에 offset 적용 되어 잇는거는 옴겨준다.
	CsC_EffectObject* pEffect;
	CsC_EffectProp* pEffectProp = g_ViewModel.GetSrcMode()->GetProp_Effect();
	int nCount = pEffectProp->GetEffectCount();
	for( int i=0; i<nCount; ++i )
	{
		pEffect = (CsC_EffectObject*)pEffectProp->GetEffect( i );
		if( ( pEffect->GetEffectPlag() & nsEFFECT::OFFSET_MASK ) == nsEFFECT::OFFSET_USE )
		{
			pEffect->SetOffset( _GetOffset() );
		}
	}
}


void _DlgAnimation::OnEnChangeEditEff0X()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_Eff0X.GetSafeHwnd() )
		return;

	__ChangeEffect0();
}

void _DlgAnimation::OnEnChangeEditEff0Y()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_Eff0Y.GetSafeHwnd() )
		return;

	__ChangeEffect0();
}

void _DlgAnimation::OnEnChangeEditEff0Z()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_Eff0Z.GetSafeHwnd() )
		return;

	__ChangeEffect0();
}

void _DlgAnimation::OnEnChangeEditEff1X()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_Eff1X.GetSafeHwnd() )
		return;

	__ChangeEffect1();
}

void _DlgAnimation::OnEnChangeEditEff1Y()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_Eff1Y.GetSafeHwnd() )
		return;

	__ChangeEffect1();
}

void _DlgAnimation::OnEnChangeEditEff1Z()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_Eff1Z.GetSafeHwnd() )
		return;

	__ChangeEffect1();
}


NiPoint3 _DlgAnimation::__GetEffect0()
{
	return NiPoint3(	nsCsMfcFunc::Control2Float( &m_edit_Eff0X )*100.0f,
						nsCsMfcFunc::Control2Float( &m_edit_Eff0Y )*100.0f,
						nsCsMfcFunc::Control2Float( &m_edit_Eff0Z )*100.0f	);
}

void _DlgAnimation::__SetEffect0( NiPoint3 v )
{
	nsCsMfcFunc::Float2Control( &m_edit_Eff0X, v.x*0.01f );
	nsCsMfcFunc::Float2Control( &m_edit_Eff0Y, v.y*0.01f );
	nsCsMfcFunc::Float2Control( &m_edit_Eff0Z, v.z*0.01f );
}

void _DlgAnimation::__ChangeEffect0()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		return;
	}

	if( m_dwSelectEventInfo == INVALIDE_LISTID )
	{
		return;
	}

	// 모델데이터 쪽에 정보 넣고
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	CsModelData::sEVENT* pModelEvent = pModelData->GetSequence( m_dwSelectSequenceInfo )->GetEvent( m_dwSelectEventInfo );
	pModelEvent->s_vValue1 = __GetEffect0();

	// 시퀀스 쪽에 정보 넣는다
	CsC_LoadSequence::sTEXTKEY_EVENT* pTextKey = m_pCsSequenceMng->GetLoadSequence()->GetSequence( m_dwSelectSequenceInfo )->s_vpTextKeyEvent[ m_dwSelectEventInfo ];
	pTextKey->s_vValue1 = pModelEvent->s_vValue1;

	g_ViewModel.SetSplinePos( pModelEvent->s_vValue1, __GetEffect1() );
}


NiPoint3 _DlgAnimation::__GetEffect1()
{
	return NiPoint3(	nsCsMfcFunc::Control2Float( &m_edit_Eff1X )*100.0f,
		nsCsMfcFunc::Control2Float( &m_edit_Eff1Y )*100.0f,
		nsCsMfcFunc::Control2Float( &m_edit_Eff1Z )*100.0f	);
}

void _DlgAnimation::__SetEffect1( NiPoint3 v )
{
	nsCsMfcFunc::Float2Control( &m_edit_Eff1X, v.x*0.01f );
	nsCsMfcFunc::Float2Control( &m_edit_Eff1Y, v.y*0.01f );
	nsCsMfcFunc::Float2Control( &m_edit_Eff1Z, v.z*0.01f );
}

void _DlgAnimation::__ChangeEffect1()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		return;
	}

	if( m_dwSelectEventInfo == INVALIDE_LISTID )
	{
		return;
	}

	// 모델데이터 쪽에 정보 넣고
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	CsModelData::sEVENT* pModelEvent = pModelData->GetSequence( m_dwSelectSequenceInfo )->GetEvent( m_dwSelectEventInfo );
	pModelEvent->s_vValue2 = __GetEffect1();

	// 시퀀스 쪽에 정보 넣는다
	CsC_LoadSequence::sTEXTKEY_EVENT* pTextKey = m_pCsSequenceMng->GetLoadSequence()->GetSequence( m_dwSelectSequenceInfo )->s_vpTextKeyEvent[ m_dwSelectEventInfo ];
	pTextKey->s_vValue2 = pModelEvent->s_vValue2;

	g_ViewModel.SetSplinePos( __GetEffect0(), pModelEvent->s_vValue2 );
}

void _DlgAnimation::__SetShowControl_Eff0_Eff1( int nCmdShow )
{
	m_edit_Eff0X.ShowWindow( nCmdShow );
	m_edit_Eff0Y.ShowWindow( nCmdShow );
	m_edit_Eff0Z.ShowWindow( nCmdShow );
	m_edit_Eff1X.ShowWindow( nCmdShow );
	m_edit_Eff1Y.ShowWindow( nCmdShow );
	m_edit_Eff1Z.ShowWindow( nCmdShow );
	m_spin_Eff0X.ShowWindow( nCmdShow );
	m_spin_Eff0Y.ShowWindow( nCmdShow );
	m_spin_Eff0Z.ShowWindow( nCmdShow );
	m_spin_Eff1X.ShowWindow( nCmdShow );
	m_spin_Eff1Y.ShowWindow( nCmdShow );
	m_spin_Eff1Z.ShowWindow( nCmdShow );
}

void _DlgAnimation::OnEnChangeEditEffectscale()
{
	if( GetFocus()->GetSafeHwnd() != m_edit_EffectScale.GetSafeHwnd() )
		return;

	if( nsCsMfcFunc::Control2Float( &m_edit_EffectScale ) < 0.02f )
		nsCsMfcFunc::Float2Control( &m_edit_EffectScale, 0.02f );

	__ChangeEffectScale();
}


void _DlgAnimation::__ChangeEffectScale()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		return;
	}

	if( m_dwSelectEventInfo == INVALIDE_LISTID )
	{
		return;
	}

	// 모델데이터 쪽에 정보 넣고
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	CsModelData::sEVENT* pModelEvent = pModelData->GetSequence( m_dwSelectSequenceInfo )->GetEvent( m_dwSelectEventInfo );
	pModelEvent->s_fEffectScale = nsCsMfcFunc::Control2Float( &m_edit_EffectScale );

	// 시퀀스 쪽에 정보 넣는다
	CsC_LoadSequence::sTEXTKEY_EVENT* pTextKey = m_pCsSequenceMng->GetLoadSequence()->GetSequence( m_dwSelectSequenceInfo )->s_vpTextKeyEvent[ m_dwSelectEventInfo ];
	pTextKey->s_fEffectScale = pModelEvent->s_fEffectScale;
}


void _DlgAnimation::OnBnClickedCheckPlanTowside()
{
	g_ViewModel.SetPlan_TwoSide( m_chk_Plan_TwoSide.GetCheck() == BST_CHECKED );
}

void _DlgAnimation::OnBnClickedCheckPlanWire()
{
	g_ViewModel.SetPlan_WireFrame( m_chk_Plan_Wire.GetCheck() == BST_CHECKED );
}

void _DlgAnimation::__ResetLoopCount()
{
	nsCsMfcFunc::Int2Control( &m_edit_LoopCnt, m_pSelectSequenceInfo->s_nLoopCount );
}

void _DlgAnimation::OnEnChangeEditAniloop()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		return;
	}

	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	if( pModelData->IsSequence( m_dwSelectSequenceInfo ) == false )
	{
		pModelData->AddSequence( m_dwSelectSequenceInfo );
	}
	pModelData->GetSequence( m_dwSelectSequenceInfo )->s_SequenceInfo.s_nLoopCnt = nsCsMfcFunc::Control2Int( &m_edit_LoopCnt );

	// 그 정보를 LoadSequence 쪽에 Apply시킨다.	
	m_pCsSequenceMng->GetLoadSequence()->ResetApplyModelData( m_pTarget->GetModelID() );
}

void _DlgAnimation::OnCbnSelchangeComboEffLevel()
{
	if( m_pTarget == NULL )
		return;

	m_pTarget->GetProp_Effect()->DeleteLoopEffect( CsC_EffectProp::LE_DLEVEL );

	switch( m_cmb_EffLevel.GetCurSel() )
	{
	case 0:
		break;
	case 1:
		m_pTarget->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\46FinalityEffect.nif" );
		break;
	case 2:
		m_pTarget->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\51FinalityEffect.nif" );
		break;
	case 3:
		m_pTarget->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\56FinalityEffect.nif" );
		break;
	case 4:
		m_pTarget->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\61FinalityEffect.nif" );
		break;
	case 5:
		m_pTarget->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\66FinalityEffect.nif" );
		break;
	case 6:
		m_pTarget->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\71FinalityEffect.nif" );
		break;
	case 7:
		m_pTarget->GetProp_Effect()->AddLoopEffect( CsC_EffectProp::LE_DLEVEL, "system\\LevelEffect\\75FinalityEffect.nif" );
		break;
	default:
		assert_cs( false );
	}
}



void _DlgAnimation::OnBnClickedButtonDelEffect()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pTarget->GetProp_Effect()->Delete();
	m_pTarget->GetProp_Effect()->Init( m_pTarget );
}

void _DlgAnimation::OnBnClickedButtonNewShader()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		CsMessageBox( MB_OK, L"애니메이션을 클릭하셔야 쉐이더를 추가 할 수 있습니다." );
		return;
	}

	CsModelData::sSHADER ms;
	ms.Reset();
	_DlgInsertShader dlg( m_pTarget, &ms );
	if( dlg.DoModal() == IDCANCEL )
		return;

	// 모델데이터 쪽에 정보 넣고
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	if( pModelData->IsSequence( m_dwSelectSequenceInfo ) == false )
	{
		pModelData->AddSequence( m_dwSelectSequenceInfo );
	}

	CsModelData::sSHADER* pShader = NiNew CsModelData::sSHADER;
	*pShader = ms;

	pModelData->GetSequence( m_dwSelectSequenceInfo )->InsertShader( pShader );

	// 그 정보를 LoadSequence 쪽에 Apply시킨다.	
	m_pCsSequenceMng->GetLoadSequence()->ResetApplyModelData( m_pTarget->GetModelID() );

	__WriteShaderList();
}

void _DlgAnimation::OnNMDblclkListShader(NMHDR *pNMHDR, LRESULT *pResult)
{
	POSITION pos = m_list_Shader.GetFirstSelectedItemPosition();
	int nSelect = m_list_Shader.GetNextSelectedItem( pos );
	if( nSelect < 0 )
	{
		CsMessageBox( MB_OK, L"리스트에서 항목을 선택하여 주십시요." );
		return;
	}

	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	assert_cs( pModelData->IsSequence( m_dwSelectSequenceInfo ) );

	CsModelData::sSHADER* pOrg = pModelData->GetSequence( m_dwSelectSequenceInfo )->GetShader( nSelect );
	CsModelData::sSHADER ts = *pOrg;
	_DlgInsertShader dlg( m_pTarget, &ts );
	if( dlg.DoModal() == IDCANCEL )
		return;

	*pOrg = ts;

	// 그 정보를 LoadSequence 쪽에 Apply시킨다.	
	m_pCsSequenceMng->GetLoadSequence()->ResetApplyModelData( m_pTarget->GetModelID() );
	__WriteShaderList();

	*pResult = 0;
}


void _DlgAnimation::OnBnClickedButtonDelShader()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		CsMessageBox( MB_OK, L"애니메이션을 클릭하셔야 합니다." );
		return;
	}

	assert_cs( m_pTarget != NULL );
	assert_cs( m_pCsSequenceMng != NULL );

	POSITION pos = m_list_Shader.GetFirstSelectedItemPosition();
	int nSelect = m_list_Shader.GetNextSelectedItem( pos );
	if( nSelect < 0 )
	{
		CsMessageBox( MB_OK, L"왼쪽 리스트에서 항목을 선택하여 주십시요." );
		return;
	}

	// 모델데이터 쪽에 정보 넣고
	CsModelData* pModelData = g_pModelDataMng->GetData( m_pTarget->GetModelID() );
	pModelData->GetSequence( m_dwSelectSequenceInfo )->DeleteShader( nSelect );

	// 그 정보를 LoadSequence 쪽에 Apply시킨다.	
	m_pCsSequenceMng->GetLoadSequence()->ResetApplyModelData( m_pTarget->GetModelID() );

	__WriteShaderList();
}

void _DlgAnimation::__ShaderDeleteAllItem()
{
	if( m_pSelectSequenceInfo == NULL )
	{
		return;
	}
	m_list_Shader.DeleteAllItems();
}

void _DlgAnimation::__ResetShaderList()
{
	__WriteShaderList();	
}


void _DlgAnimation::__WriteShaderList()
{
	m_list_Shader.DeleteAllItems();

	int nSize = m_pSelectSequenceInfo->s_vpTextKeyShader.Size();
	for( int i=0; i<nSize; ++i )
	{
		CsC_LoadSequence::sTEXTKEY_SHADER* pShader = m_pSelectSequenceInfo->s_vpTextKeyShader[ i ];
		int nIndex = m_list_Shader.GetItemCount();

		switch( pShader->s_eShaderType )
		{
		case CsC_LoadSequence::sTEXTKEY_SHADER::ST_ALPHATEX:
			{
				m_list_Shader.InsertItem( nIndex, L"알파텍스쳐" );
			}
			break;
		case CsC_LoadSequence::sTEXTKEY_SHADER::ST_BLUR:
			{
				m_list_Shader.InsertItem( nIndex, L"블러" );
			}
			break;
		default:
			assert_cs( false );
		}
	}
}
