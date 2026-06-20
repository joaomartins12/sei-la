
// WinLBottom.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ObjectMove.h"
#include "WinLBottom.h"

enum eSETMODE{ SM_ADD, SM_EDIT, };


WinLBottom*		g_pWinLBottom = NULL;

// WinLBottom

IMPLEMENT_DYNCREATE(WinLBottom, CFormView)

WinLBottom::WinLBottom()
	: CFormView(WinLBottom::IDD)
{
	m_pEditAct = NULL;
}

WinLBottom::~WinLBottom()
{
}

void WinLBottom::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_COMBO_SET_MODE, m_cmb_SM);
	DDX_Control(pDX, IDC_COMBO_SM_TYPE, m_cmb_SM_Type);
	DDX_Control(pDX, IDC_EDIT_SM_POSX, m_edit_SM_PosX);
	DDX_Control(pDX, IDC_EDIT_SM_POSY, m_edit_SM_PosY);
	DDX_Control(pDX, IDC_COMBO_SM_ANI, m_cmb_SM_Ani);
	DDX_Control(pDX, IDC_EDIT_SM_TIME, m_edit_SM_Time);
	DDX_Control(pDX, IDC_BUTTON_SM_ADD, m_btn_SM_Add);
	DDX_Control(pDX, IDC_LIST_ACT, m_list_Act);
	DDX_Control(pDX, IDC_STATIC_SM_TIME, m_static_SMTime);
	DDX_Control(pDX, IDC_BUTTON_PV_PLAY, m_btn_PV_Play);
	DDX_Control(pDX, IDC_COMBO_PV_SPEED, m_cmb_PV_Speed);
	DDX_Control(pDX, IDC_EDIT_PV_TIME, m_edit_PV_Time);
}

BEGIN_MESSAGE_MAP(WinLBottom, CFormView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_SM_ADD, &WinLBottom::OnBnClickedButtonSmAdd)
	ON_CBN_SELCHANGE(IDC_COMBO_SET_MODE, &WinLBottom::OnCbnSelchangeComboSetMode)
	ON_CBN_SELCHANGE(IDC_COMBO_SM_TYPE, &WinLBottom::OnCbnSelchangeComboSmType)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ACT, &WinLBottom::OnLvnItemchangedListAct)
	ON_NOTIFY(NM_CLICK, IDC_LIST_ACT, &WinLBottom::OnNMClickListAct)
	ON_CBN_SELCHANGE(IDC_COMBO_SM_ANI, &WinLBottom::OnCbnSelchangeComboSmAni)
	ON_EN_CHANGE(IDC_EDIT_SM_TIME, &WinLBottom::OnEnChangeEditSmTime)
	ON_BN_CLICKED(IDC_BUTTON_DEL_ALLACT, &WinLBottom::OnBnClickedButtonDelAllact)
	ON_BN_CLICKED(IDC_BUTTON_PV_PLAY, &WinLBottom::OnBnClickedButtonPvPlay)
	ON_BN_CLICKED(IDC_BUTTON_PV_REWIND, &WinLBottom::OnBnClickedButtonPvRewind)
END_MESSAGE_MAP()


// WinLBottom 진단입니다.

#ifdef _DEBUG
void WinLBottom::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void WinLBottom::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// WinLBottom 메시지 처리기입니다.

void WinLBottom::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	if( this == g_pProc3DWin )
	{
		g_ptScreen = CsPoint( cx, cy );
		if( g_pRenderer )
		{		
			g_pRenderer->Recreate( cx, cy );		
		}
	}
}

void WinLBottom::_Init()
{
	DWORD dwExStyle = m_list_Act.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Act.SetExtendedStyle( dwExStyle );
	m_list_Act.InsertColumn( 0, L"idx", LVCFMT_LEFT, 30 );
	m_list_Act.InsertColumn( 1, L"행동", LVCFMT_LEFT, 90 );
	m_list_Act.InsertColumn( 2, L"누적시간(분)", LVCFMT_LEFT, 90 );

	m_cmb_SM.InsertString( SM_ADD, L"추가" );
	m_cmb_SM.InsertString( SM_EDIT, L"수정" );
	m_cmb_SM.SetCurSel( SM_ADD );

	m_cmb_SM_Type.InsertString( CsMoveObject::AT_START, L"시작점" );
	m_cmb_SM_Type.InsertString( CsMoveObject::AT_WAIT, L"대기" );
	m_cmb_SM_Type.InsertString( CsMoveObject::AT_MOVE, L"이동" );
	m_cmb_SM_Type.InsertString( CsMoveObject::AT_GOTO_FIRST_LOOP, L"초기로 루프" );
	m_cmb_SM_Type.SetCurSel( CsMoveObject::AT_START );


	m_cmb_PV_Speed.InsertString( 0, L"0.5" );
	m_cmb_PV_Speed.InsertString( 1, L"1" );
	m_cmb_PV_Speed.InsertString( 2, L"1.5" );
	m_cmb_PV_Speed.InsertString( 3, L"2" );
	m_cmb_PV_Speed.InsertString( 4, L"3" );
	m_cmb_PV_Speed.InsertString( 5, L"5" );
	m_cmb_PV_Speed.InsertString( 6, L"10" );
	m_cmb_PV_Speed.InsertString( 7, L"20" );
	m_cmb_PV_Speed.SetCurSel( 1 );

	_SM_Enable_All( false );
}

void WinLBottom::_ResetMap( DWORD dwMapID )
{
	_SM_Enable_All( false );

	m_list_Act.DeleteAllItems();
}

void WinLBottom::_Update()
{
	CsC_AvObject* pTarget = g_pProc3D->GetMap()->GetMoveMng()->GetMoveObject();
	if( pTarget == NULL )
	{
		return;
	}

	TCHAR szMsg[ 128 ];
	_stprintf_s( szMsg, 128, L"%.2f", pTarget->GetProp_MoveControl()->GetAccumTime()/60.0f );
	m_edit_PV_Time.SetWindowText( szMsg );
}

void WinLBottom::_SetTarget_Npc( CsC_AvObject* pObject )
{
	_SM_Enable_All( true );

	// 기존 목록 삭제 후 애니메이션 등록
	m_cmb_SM_Ani.ResetContent();
	NiActorManager* pActorMng = pObject->GetProp_Animation()->GetActorMng();
	NiKFMTool* pKFMTool = pActorMng->GetKFMTool();
	UINT* nIDs = NULL;
	UINT nSeqCnt = 0;
	pKFMTool->GetSequenceIDs( nIDs, nSeqCnt );
	if( nIDs == NULL )
		return;

	NiControllerSequence* pContSeq;
	UINT nSrcSeqID;
	TCHAR sz[ 32 ];
	for(UINT nSrc=0; nSrc<nSeqCnt; ++nSrc)
	{
		if(!pObject->GetProp_Animation()->IsMoveObjectAni(nIDs[ nSrc ]))
			continue;
		nSrcSeqID = pKFMTool->GetSequence( nIDs[ nSrc ] )->GetSequenceID();
		pContSeq = pActorMng->GetSequence( nSrcSeqID );
		if( pContSeq == NULL )
			continue;

		int nIndex = m_cmb_SM_Ani.GetCount();
		_stprintf_s( sz, 32, L"%d", nSrcSeqID );
		m_cmb_SM_Ani.InsertString( nIndex, sz );
		m_cmb_SM_Ani.SetItemData( nIndex, nSrcSeqID );
	}
	m_cmb_SM_Ani.SetCurSel( 0 );

	NiFree ( nIDs );

	__ResetActList( false );
}

void WinLBottom::__ResetActList( bool bKeepSelect )
{
	POSITION pos = m_list_Act.GetFirstSelectedItemPosition();
	int nSel = m_list_Act.GetNextSelectedItem( pos );
	CsMoveObject::sACT_BASE* pOldAct = NULL;
	int nScrollCtrl = m_list_Act.GetScrollPos( SB_VERT );
	if( nSel >= 0 )
	{
		pOldAct = (CsMoveObject::sACT_BASE*)m_list_Act.GetItemData( nSel );
	}	
	m_list_Act.DeleteAllItems();

	CsC_AvObject* pTarget = g_pProc3D->GetMap()->GetMoveMng()->GetMoveObject();
	CsMoveObject::eKIND kind;
	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_MOVE_NPC:
		kind = CsMoveObject::MO_NPC;
		break;
	case RTTI_MOVE_MON:
		kind = CsMoveObject::MO_MONSTER;
		break;
	default:
		assert_cs( false );
	}
	CsMoveObject* pMOFT = nsCsFileTable::g_pMoveObjectMng->GetObject( kind, pTarget->GetFTID() );

	TCHAR szMsg[ 128 ];
	std::list< CsMoveObject::sACT_BASE* >* pList = pMOFT->GetActList();
	std::list< CsMoveObject::sACT_BASE* >::iterator it = pList->begin();
	std::list< CsMoveObject::sACT_BASE* >::iterator itEnd = pList->end();
	CsPoint ptPrev;
	CsPoint ptStart;
	float fTotalSec = 0;
	int nSelectIndex = -1;
	for( ; it!=itEnd; ++it )
	{
		int nListIndex = m_list_Act.GetItemCount();		
		_stprintf_s( szMsg, 128, L"%d", nListIndex );
		m_list_Act.InsertItem( nListIndex, szMsg );
		
		switch( (*it)->GetType() )
		{
		case CsMoveObject::AT_START:
			{
				CsMoveObject::sACT_START* pAct = (CsMoveObject::sACT_START*)(*it);
				ptPrev = CsPoint( pAct->s_PosX, pAct->s_PosY );
				ptStart = ptPrev;
				m_list_Act.SetItem( nListIndex, 1, LVIF_TEXT, L"시작점", 0, 0, 0, 0 );			
			}
			break;
		case CsMoveObject::AT_WAIT:
			fTotalSec += (*it)->GetTime( 0 );
			m_list_Act.SetItem( nListIndex, 1, LVIF_TEXT, L"대기", 0, 0, 0, 0 );
			break;
		case CsMoveObject::AT_MOVE:
			{
				CsMoveObject::sACT_MOVE* pAct = (CsMoveObject::sACT_MOVE*)(*it);
				fTotalSec += pAct->GetTime( ( ptPrev - CsPoint( pAct->s_PosX, pAct->s_PosY ) ).Length() );
				ptPrev = CsPoint( pAct->s_PosX, pAct->s_PosY );
				m_list_Act.SetItem( nListIndex, 1, LVIF_TEXT, L"이동", 0, 0, 0, 0 );
			}			
			break;
		case CsMoveObject::AT_GOTO_FIRST_LOOP:
			{
				CsMoveObject::sACT_GOTO_FIRST_LOOP* pAct = (CsMoveObject::sACT_GOTO_FIRST_LOOP*)(*it);
				fTotalSec += pAct->GetTime( ( ptStart - ptPrev ).Length() );
				m_list_Act.SetItem( nListIndex, 1, LVIF_TEXT, L"초기로 루프", 0, 0, 0, 0 );
			}		
			break;
		default:
			assert_cs( false );
		}		
		_stprintf_s( szMsg, 128, L"%.2f", fTotalSec/60.0f );
		m_list_Act.SetItem( nListIndex, 2, LVIF_TEXT, szMsg, 0, 0, 0, 0 );
		m_list_Act.SetItemData( nListIndex, (DWORD_PTR)*it );

		if( pOldAct == (*it) )
		{
			nSelectIndex = nListIndex;
		}
	}
	pMOFT->SetTotalTime( fTotalSec );
	pMOFT->SetMapID( g_pProc3D->GetMap()->GetCurMapID() );
	if( bKeepSelect )
	{
		if( nSelectIndex >= 0 )
		{
			m_list_Act.SetItemState( nSelectIndex, LVIS_SELECTED, LVIS_SELECTED );			

			CRect rc;
			m_list_Act.GetItemRect( 0, rc, LVIR_BOUNDS );
			CSize size( 0, rc.Height()*nScrollCtrl );
			m_list_Act.Scroll( size );
			m_list_Act.SetScrollPos( SB_VERT, nScrollCtrl );
		}
	}
	else
	{
		m_pEditAct = NULL;
	}
}

bool WinLBottom::_IsAllActEnableMove()
{
	CsC_AvObject* pTarget = g_pProc3D->GetMap()->GetMoveMng()->GetMoveObject();
	assert_cs( pTarget->GetLeafRTTI() == RTTI_MOVE_NPC );
	CsMoveObject* pMOFT  = NULL;
	if( RTTI_MOVE_NPC == pTarget->GetLeafRTTI())
		pMOFT = nsCsFileTable::g_pMoveObjectMng->GetObject( CsMoveObject::MO_NPC, pTarget->GetFTID() );
	else if( RTTI_MOVE_MON == pTarget->GetLeafRTTI() )
		pMOFT = nsCsFileTable::g_pMoveObjectMng->GetObject( CsMoveObject::MO_MONSTER, pTarget->GetFTID() );

	std::list< CsMoveObject::sACT_BASE* >* pList = pMOFT->GetActList();
	std::list< CsMoveObject::sACT_BASE* >::iterator it = pList->begin();
	std::list< CsMoveObject::sACT_BASE* >::iterator itEnd = pList->end();
	CsPoint pt1, pt2, ptStart;
	for( ; it!=itEnd; ++it )
	{
		switch( (*it)->GetType() )
		{
		case CsMoveObject::AT_START:
			pt1.x = (*it)->GetPosX();
			pt1.y = (*it)->GetPosY();
			ptStart = pt1;
			break;
		case CsMoveObject::AT_WAIT:
			break;
		case CsMoveObject::AT_MOVE:
			{
				pt2.x = (*it)->GetPosX();
				pt2.y = (*it)->GetPosY();
				if( nsCsGBTerrain::g_pCurRoot->IsReachableEmr( pt1.x, pt1.y, pt2.x, pt2.y ) == false )
					return false;
				pt1 = pt2;
			}
			break;
		case CsMoveObject::AT_GOTO_FIRST_LOOP:
			{
				if( nsCsGBTerrain::g_pCurRoot->IsReachableEmr( pt1.x, pt1.y, ptStart.x, ptStart.y ) == false )
					return false;
			}
			break;
		}
	}
	return true;
}

void WinLBottom::__EditModeActList()
{
	POSITION pos = m_list_Act.GetFirstSelectedItemPosition();
	int nSel = m_list_Act.GetNextSelectedItem( pos );
	if( nSel < 0 )
	{
		m_pEditAct = NULL;
		m_cmb_SM_Type.EnableWindow( FALSE );
		_SM_Enable_Pos( false );
		_SM_Enable_Ani( false );
		_SM_Enable_Time( false );
		return;
	}

	m_cmb_SM_Type.EnableWindow( TRUE );
	m_pEditAct = (CsMoveObject::sACT_BASE*)m_list_Act.GetItemData( nSel );
	m_cmb_SM_Type.SetCurSel( m_pEditAct->GetType() );
	OnCbnSelchangeComboSmType();
	m_cmb_SM_Type.EnableWindow( FALSE );

	_SM_Set_Pos( CsPoint( m_pEditAct->GetPosX(), m_pEditAct->GetPosY() ) );
	_SM_Set_Ani( m_pEditAct->GetAni() );
	_SM_Set_Time( m_pEditAct->GetTime() );	
}


//==============================================================================================================
//
//		모드
//
//==============================================================================================================
void WinLBottom::OnCbnSelchangeComboSetMode()
{
	assert_cs( m_cmb_SM.IsWindowEnabled() );
	switch( m_cmb_SM.GetCurSel() )
	{
	case SM_ADD:
		m_btn_SM_Add.ShowWindow( SW_SHOW );
		m_pEditAct = NULL;
		_SM_Enable_Type( true );
		break;
	case SM_EDIT:
		m_btn_SM_Add.ShowWindow( SW_HIDE );
		__EditModeActList();
		break;
	default:
		assert_cs( false );
	}
}

void WinLBottom::_SM_Enable_All( bool bEnable )
{
	m_cmb_SM.EnableWindow( bEnable == true );
	_SM_Enable_Type( bEnable );

	if( bEnable == true )
	{
		OnCbnSelchangeComboSetMode();
	}
	else
	{
		m_btn_SM_Add.ShowWindow( SW_HIDE );
		m_pEditAct = NULL;
	}
}

//==============================================================================================================
//
//		타입
//
//==============================================================================================================

void WinLBottom::OnCbnSelchangeComboSmType()
{
	assert_cs( m_cmb_SM_Type.IsWindowEnabled() );
	switch( m_cmb_SM_Type.GetCurSel() )
	{
	case CsMoveObject::AT_START:
		_SM_Enable_Pos( true );
		_SM_Enable_Ani( false );
		_SM_Enable_Time( false );
		m_static_SMTime.SetWindowText( L"x" );
		break;
	case CsMoveObject::AT_WAIT:
		_SM_Enable_Pos( false );
		_SM_Enable_Ani( true );
		_SM_Enable_Time( true );
		m_static_SMTime.SetWindowText( L"시간" );
		break;
	case CsMoveObject::AT_MOVE:
		_SM_Enable_Pos( true );
		_SM_Enable_Ani( true );
		_SM_Enable_Time( true );
		m_static_SMTime.SetWindowText( L"속도" );
		break;
	case CsMoveObject::AT_GOTO_FIRST_LOOP:
		_SM_Enable_Pos( false );
		_SM_Enable_Ani( true );
		_SM_Enable_Time( true );
		m_static_SMTime.SetWindowText( L"속도" );
		break;
	default:
		assert_cs( false );
	}
}


void WinLBottom::_SM_Enable_Type( bool bEnable )
{
	m_cmb_SM_Type.EnableWindow( bEnable == true );
	if( bEnable == true )
	{
		OnCbnSelchangeComboSmType();
	}
	else
	{
		_SM_Enable_Pos( bEnable );
		_SM_Enable_Ani( bEnable );
		_SM_Enable_Time( bEnable );
	}	
}

//==============================================================================================================
//
//		타입 _ 위치
//
//==============================================================================================================
bool WinLBottom::_SM_Set_Pos( CsPoint pos )
{
	if( m_edit_SM_PosX.IsWindowEnabled() == FALSE )
		return false;

	nsCsMfcFunc::Int2Control( &m_edit_SM_PosX, pos.x );
	nsCsMfcFunc::Int2Control( &m_edit_SM_PosY, pos.y );
	switch( m_cmb_SM.GetCurSel() )
	{
	case SM_ADD:
		break;
	case SM_EDIT:
		{
			// 모든 경로에서 제대로 움직여 지는지 체크
			int nOldX = m_pEditAct->GetPosX();
			int nOldY = m_pEditAct->GetPosY();

			assert_cs( m_pEditAct );		
			m_pEditAct->SetPosX( pos.x );
			m_pEditAct->SetPosY( pos.y );

			if( _IsAllActEnableMove() == true )
			{
				__ResetActList( true );
			}
			else
			{
				m_pEditAct->SetPosX( nOldX );
				m_pEditAct->SetPosY( nOldY );
				nsCsMfcFunc::Int2Control( &m_edit_SM_PosX, nOldX );
				nsCsMfcFunc::Int2Control( &m_edit_SM_PosY, nOldY );

				CsMessageBox( MB_OK, L"이동 지역 사이에 장애물이 존재합니다." );
			}
		}		
		break;
	default:
		assert_cs( false );
	}
	
	return true;	
}

void WinLBottom::_SM_Enable_Pos( bool bEnable )
{
	m_edit_SM_PosX.EnableWindow( bEnable == true );
	m_edit_SM_PosY.EnableWindow( bEnable == true );
}

//==============================================================================================================
//
//		타입 _ 애니메이션
//
//==============================================================================================================

void WinLBottom::_SM_Enable_Ani( bool bEnable )
{
	m_cmb_SM_Ani.EnableWindow( bEnable == true );
}

bool WinLBottom::_SM_Set_Ani( DWORD dwAniID )
{
	if( m_cmb_SM_Ani.IsWindowEnabled() == FALSE )
		return false;

	nsCsMfcFunc::ComboSelect_DWORD_Data( &m_cmb_SM_Ani, dwAniID );
	return true;
}

void WinLBottom::OnCbnSelchangeComboSmAni()
{
	assert_cs( m_cmb_SM_Ani.IsWindowEnabled() );
	switch( m_cmb_SM.GetCurSel() )
	{
	case SM_ADD:
		break;
	case SM_EDIT:
		assert_cs( m_pEditAct );
		m_pEditAct->SetAni( (DWORD)m_cmb_SM_Ani.GetItemData( m_cmb_SM_Ani.GetCurSel() ) );		
		break;
	default:
		assert_cs( false );
	}
}

//==============================================================================================================
//
//		타입 _ 시간
//
//==============================================================================================================

void WinLBottom::_SM_Enable_Time( bool bEnable )
{
	m_edit_SM_Time.EnableWindow( bEnable == true );
}

bool WinLBottom::_SM_Set_Time( int nSec )
{
	if( m_edit_SM_Time.IsWindowEnabled() == FALSE )
		return false;

	nsCsMfcFunc::Int2Control( &m_edit_SM_Time, nSec );
	return true;
}

void WinLBottom::OnEnChangeEditSmTime()
{	
	assert_cs( m_edit_SM_Time.IsWindowEnabled() );

	if( m_edit_SM_Time.GetSafeHwnd() != GetFocus()->GetSafeHwnd() )
		return;

	switch( m_cmb_SM.GetCurSel() )
	{
	case SM_ADD:
		break;
	case SM_EDIT:
		{
			assert_cs( m_pEditAct );
			int nTime = CsMax( 1, nsCsMfcFunc::Control2Int( &m_edit_SM_Time ) );
			m_pEditAct->SetTime( nTime );
			__ResetActList( true );
		}		
		break;
	default:
		assert_cs( false );
	}
}

//==============================================================================================================
//
//		기타
//
//==============================================================================================================

// 추가
void WinLBottom::OnBnClickedButtonSmAdd()
{
	// 현재 선택된 오브젝트의 파일테이블을 가져온다
	CsC_AvObject* pTarget = g_pProc3D->GetMap()->GetMoveMng()->GetMoveObject();
	assert_cs( pTarget != NULL );	
	CsMoveObject::eKIND kind;
	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_MOVE_MON:
		kind = CsMoveObject::MO_MONSTER;
		break;
	case RTTI_MOVE_NPC:
		kind = CsMoveObject::MO_NPC;
		break;
	default:
		assert_cs( false );
	}
	CsMoveObject* pMOFT = nsCsFileTable::g_pMoveObjectMng->GetObject( kind, pTarget->GetFTID() );

	// 현재 설정된 값
	switch( m_cmb_SM_Type.GetCurSel() )
	{
	case CsMoveObject::AT_START:
		{
			// 기존에 존재 하는지 찾아 본다
			if( pMOFT->IsAct( CsMoveObject::AT_START ) == false )
			{				
				CsMoveObject::sACT_START act;
				act.s_PosX = _SM_Get_PosX();
				act.s_PosY = _SM_Get_PosY();
				pMOFT->AddAct( &act );
			}
			else
			{
				CsMessageBox( MB_OK, L"이미 시작점을 설정 하였습니다" );
			}
		}
		break;
	case CsMoveObject::AT_WAIT:
		{
			if( pMOFT->IsAct( CsMoveObject::AT_START ) )
			{
				if( pMOFT->IsAct( CsMoveObject::AT_GOTO_FIRST_LOOP ) == false )
				{
					if( _SM_Get_Time() > 0 )
					{
						CsMoveObject::sACT_WAIT act;
						act.s_dwAni = _SM_Get_Ani();
						act.s_nTimeSec = _SM_Get_Time();
						pMOFT->AddAct( &act );
					}					
					else
					{
						CsMessageBox( MB_OK, L"시간은 1초이상의 값으로 설정되야 합니다." );
					}
				}
				else
				{
					CsMessageBox( MB_OK, L"초기로 루프 이후에는 설정 하실 수 없습니다." );
				}
			}
			else
			{
				CsMessageBox( MB_OK, L"시작점을 먼저 설정 하여주십시요." );
			}
		}
		break;
	case CsMoveObject::AT_MOVE:
		{
			if( pMOFT->IsAct( CsMoveObject::AT_START ) )
			{
				if( pMOFT->IsAct( CsMoveObject::AT_GOTO_FIRST_LOOP ) == false )
				{
					if( _SM_Get_Time() > 0 )
					{
						// 장애물 체크
						int x1, y1;
						pMOFT->GetLastMovePoint( x1, y1 );
						if( nsCsGBTerrain::g_pCurRoot->IsReachableEmr( x1, y1, _SM_Get_PosX(), _SM_Get_PosY() ) == true )
						{
							CsMoveObject::sACT_MOVE act;
							act.s_PosX = _SM_Get_PosX();
							act.s_PosY = _SM_Get_PosY();
							act.s_dwAni = _SM_Get_Ani();
							act.s_nVelocity = _SM_Get_Time();
							pMOFT->AddAct( &act );
						}
						else
						{
							CsMessageBox( MB_OK, L"이동 지역 사이에 장애물이 존재합니다." );
						}
					}
					else
					{
						CsMessageBox( MB_OK, L"속도는 1이상의 값으로 설정되야 합니다." );
					}
				}
				else
				{
					CsMessageBox( MB_OK, L"초기로 루프 이후에는 설정 하실 수 없습니다." );
				}
			}
			else
			{
				CsMessageBox( MB_OK, L"시작점을 먼저 설정 하여주십시요." );
			}
		}
		break;
	case CsMoveObject::AT_GOTO_FIRST_LOOP:
		{
			if( pMOFT->IsAct( CsMoveObject::AT_START ) )
			{
				if( pMOFT->IsAct( CsMoveObject::AT_GOTO_FIRST_LOOP ) == false )
				{
					if( _SM_Get_Time() > 0 )
					{
						int x1, y1, x2, y2;
						pMOFT->GetFirstMovePoint( x1, y1 );
						pMOFT->GetLastMovePoint( x2, y2 );
						if( nsCsGBTerrain::g_pCurRoot->IsReachableEmr( x1, y1, x2, y2 ) == true )
						{
							CsMoveObject::sACT_GOTO_FIRST_LOOP act;
							act.s_dwAni = _SM_Get_Ani();
							act.s_nVelocity = _SM_Get_Time();
							pMOFT->AddAct( &act );
						}						
						else
						{
							CsMessageBox( MB_OK, L"이동 지역 사이에 장애물이 존재합니다." );
						}
					}
					else
					{
						CsMessageBox( MB_OK, L"속도는 1이상의 값으로 설정되야 합니다." );
					}
				}
				else
				{
					CsMessageBox( MB_OK, L"이미 초기로 루프가 설정 되어 있습니다." );
				}
			}
			else
			{
				CsMessageBox( MB_OK, L"시작점을 먼저 설정 하여주십시요." );
			}
		}
		break;
	default:
		assert_cs( false );
	}

	__ResetActList( true );
}
void WinLBottom::OnLvnItemchangedListAct(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	*pResult = 0;
}

void WinLBottom::OnNMClickListAct(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( m_cmb_SM.GetCurSel() == SM_EDIT )
		__EditModeActList();

	*pResult = 0;
}


void WinLBottom::OnBnClickedButtonDelAllact()
{
	CsC_AvObject* pTarget = g_pProc3D->GetMap()->GetMoveMng()->GetMoveObject();
	if( pTarget == NULL )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"모든 행동을 제거 하시겠습니까?" ) == IDCANCEL )
		return;
	
	CsMoveObject::eKIND kind;
	switch( pTarget->GetLeafRTTI() )
	{
	case RTTI_MOVE_MON:
		kind = CsMoveObject::MO_MONSTER;
		break;
	case RTTI_MOVE_NPC:
		kind = CsMoveObject::MO_NPC;
		break;
	default:
		assert_cs( false );
	}
	CsMoveObject* pMOFT = nsCsFileTable::g_pMoveObjectMng->GetObject( kind, pTarget->GetFTID() );
	pMOFT->Delete();

	__ResetActList( false );
}

void WinLBottom::OnBnClickedButtonPvPlay()
{
	_PreViewPlay( _IsPlay() == false );
}

void WinLBottom::OnBnClickedButtonPvRewind()
{
	CsC_AvObject* pTarget = g_pProc3D->GetMap()->GetMoveMng()->GetMoveObject();
	if( pTarget == NULL )
	{
		return;
	}

	pTarget->GetProp_MoveControl()->ResetAccumTime();
}

bool WinLBottom::_IsPlay()
{
	TCHAR sz[ 32 ];
	m_btn_PV_Play.GetWindowText( sz, 32 );
	if( _tcscmp( sz, L"▶" ) == 0 )
	{
		return false;
	}
	return true;
}

void WinLBottom::_PreViewPlay( bool bPlay )
{
	CsC_AvObject* pTarget = g_pProc3D->GetMap()->GetMoveMng()->GetMoveObject();
	if( pTarget == NULL )
	{
		bPlay = false;
	}

	if( bPlay )
	{
		m_btn_PV_Play.SetWindowText( L"Ⅱ" );
	}
	else
	{
		m_btn_PV_Play.SetWindowText( L"▶" );
	}
}

