
#include "stdafx.h"
#include "TacticsCharge.h"

#ifdef USE_REVISION

#define IF_TACTICS_PROCESS_TIME		1.5f

void cTactics_Charge::Destroy()
{
	cBaseWindow::Delete();	
}

void cTactics_Charge::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pPBarProcess );
	SAFE_NIDELETE( m_pBtnOk );
	SAFE_NIDELETE( m_pBtnCancel );
	SAFE_NIDELETE( m_pBtnExit );
	SAFE_NIDELETE( m_pTextResistCount );

	if( g_pDataMng )
	{
		if( m_ResistItem.IsEnable() == true )
		{
			g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nResistInvenIndex ) );
		}
		m_ResistItem.Reset();
	}
}

void cTactics_Charge::PreResetMap()
{	
	if( g_pDataMng )
	{
		if( m_ResistItem.IsEnable() == true )
		{
			g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nResistInvenIndex ) );
		}
		m_ResistItem.Reset();
	}
}

void cTactics_Charge::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;	

	InitScript( "MakeTactics\\Tactics_Charge.tga", CsPoint( 530, 325 ), CsPoint( 255, 146 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );
	AddTitle( _LAN( "보정 게이지 충전" ) );

	m_pCancelButton = AddButton( CsPoint( 233, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	m_pPBarProcess = NiNew cProgressBar;
	m_pPBarProcess->Init( m_pRoot, cProgressBar::BAR_1IMAGE, CsPoint( 72, 56 ), CsPoint( 162, 19 ), CsPoint::ZERO, "DigitamaTrade\\Digitama_ScanBar.tga", false );
	m_pPBarProcess->SetRange( CsPoint( 0, (int)( IF_TACTICS_PROCESS_TIME*1000.0f ) ) );

	m_pBtnAutoResist = AddButton( CsPoint( 94, 84 ), CsPoint( 61, 26 ), cButton::IMAGE_NOR_1, _LAN( "자동등록" ) );

	m_pBtnOk = NiNew cButton;
	m_pBtnOk->Init( m_pRoot, CsPoint( 198, 88 ), CsPoint( 48, 48 ), cButton::IMAGE_NOR_2, _LAN( "확인" ), false );

	m_pBtnCancel = NiNew cButton;
	m_pBtnCancel->Init( m_pRoot, CsPoint( 198, 88 ), CsPoint( 48, 48 ), cButton::IMAGE_NOR_2, _LAN( "취소" ), false );
	m_pBtnCancel->SetSound( cWindow::SD_Wa1 );

	m_pBtnExit = NiNew cButton;
	m_pBtnExit->Init( m_pRoot, CsPoint( 198, 88 ), CsPoint( 48, 48 ), cButton::IMAGE_NOR_2, _LAN( "닫기" ), false );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.SetText( 1 );
	ti.s_eTextAlign = DT_RIGHT;
	m_pTextResistCount = NiNew cText;
	m_pTextResistCount->Init( m_pRoot, CsPoint( 72, 83 ), &ti, false );	

	m_pTarget = NULL;
	m_rcResistItem.SetRect( CsPoint( 30, 49 ), CsSIZE( 32, 32 ) );
	_ProcessCancel();

	m_nSearchInvenIndex = 0;
	m_bProcessing = false;
	m_bWaitRecvServer = false;	
}

void cTactics_Charge::Update()
{
	g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_MAKE_TACTICS );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) == false )
	{
		Close();
		return;
	}

	// 테이머와의 거리 체크
	assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
	if( ( (CNpc*)m_pTarget )->CheckTamerDist() == false )
	{		
		Close();
		return;
	}

	_UpdateMoveWindow();
	_Update_Process();
}

cBaseWindow::eMU_TYPE
cTactics_Charge::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pBtnActive->Update_ForMouse() )
	{
	case cButton::ACTION_DOWN:
		return muReturn;
	case cButton::ACTION_CLICK:
		// 확인 버튼 클릭
		if( m_pBtnActive == m_pBtnOk )
		{
			CsItem::sINFO* pItemFT = nsCsFileTable::g_pItemMng->GetItem( m_ResistItem.GetType() )->GetInfo();
			// 용병알이면서 케쉬템일 경우~ 메세지표현
			if( ( pItemFT->s_nType_L == nItem::Digitama3 )&&pItemFT->IsCashType() )
			{
				cPrintMsg::PrintMsg( 20081, pItemFT->s_szName );
			}
			else
			{
				StartProcess();
			}		
		}
		// 취소 버튼 클릭
		else if( m_pBtnActive == m_pBtnCancel )
		{
			_ProcessCancel();
		}
		// 나가기 버튼 클릭
		else
		{
			assert_cs( m_pBtnActive == m_pBtnExit );
			Close();
		}
		return muReturn;
	}

	// 자동 등록
	switch( m_pBtnAutoResist->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:	
		_AutoResist();
		return muReturn;
	case cButton::ACTION_DOWN:
		return muReturn;
	}

	// 툴팁용
	CsPoint ptMouseLocalPos = MousePosToWindowPos( g_Cursor.GetPos() );
	if( m_ResistItem.IsEnable() == true )
	{		
		if( m_rcResistItem.PtInRect( ptMouseLocalPos ) == TRUE )
		{
			CsPoint pos = m_rcResistItem.GetPos() + GetRootClient();			
			g_pGameIF->GetTooltip()->SetTooltip( pos, CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_ResistItem.GetType(), cBaseWindow::WT_DIGITAMA_SCAN );
			return muReturn;
		}
	}

	if( g_Cursor.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 255, 27 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();		

	return muReturn;
}

void cTactics_Charge::Render()
{
	RenderScript();

	m_pBtnActive->Render();

	// 등록된 디지타마가 있다면
	if( m_ResistItem.IsEnable() == true )
	{
		g_pIconMng->RenderItem( &m_ResistItem, m_rcResistItem.GetPos() + GetRootClient(), false );
		m_pTextResistCount->SetText( m_ResistItem.GetCount() );
		m_pTextResistCount->Render();
		m_pPBarProcess->Render();
	}
}

bool cTactics_Charge::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{	
			// 분리해서
			if( g_pDataMng->GetInven()->GetData( nIconSlot )->GetCount() != nIconCount )
			{
				cPrintMsg::PrintMsg( 20030 );
			}
			else if( IsResist( nIconSlot ) )
			{
				SetResist( nIconSlot, nIconCount );
			}
			g_Cursor.ReleaseIcon();
			return true;
		}
		break;
	}	

	return false;
}

void cTactics_Charge::ResetDevice()
{	
	ResetDeviceScript();

	m_pPBarProcess->ResetDevice();
	m_pBtnOk->ResetDevice();
	m_pBtnCancel->ResetDevice();
	m_pBtnExit->ResetDevice();
	m_pTextResistCount->ResetDevice();
}

void cTactics_Charge::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();

	m_pPBarProcess->ResetDevice();
	m_pBtnOk->ResetDevice();
	m_pBtnCancel->ResetDevice();
	m_pBtnExit->ResetDevice();
	m_pTextResistCount->ResetDevice();
}

//========================================================================================================
//
//	게이지 충전
//
//========================================================================================================

void cTactics_Charge::StartProcess()
{
	cWindow::PlaySound( "System\\Item_Disc_02.wav" );

	m_pBtnActive = m_pBtnCancel;
	m_bProcessing = true;
	m_fProcessTime = 0.0f;
	m_pBtnAutoResist->SetEnable( false );	
}

void cTactics_Charge::_Update_Process()
{
	if( m_bProcessing == false )
		return;	

	// 서버에서 대이터 받기를 대기중
	if( m_bWaitRecvServer == true )
		return;

	m_fProcessTime += g_fDeltaTime;
	m_pPBarProcess->SetPos( (int)( m_fProcessTime*1000.0f ) );

	if( m_fProcessTime > IF_TACTICS_PROCESS_TIME )
	{
		_ProcessSuccess();
	}
}

void cTactics_Charge::_ProcessSuccess()
{
	m_bProcessing = false;
	m_bWaitRecvServer = true;

	// 동기화
	if( net::game )
	{
		
	}
	else
	{
		RecvServer();
	}
}

void cTactics_Charge::_ProcessCancel()
{
	if( m_bProcessing == false )
	{
		assert_cs( m_bWaitRecvServer == true );
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	m_bProcessing = false;
	if( m_ResistItem.IsEnable() == true )
	{
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nResistInvenIndex ) );	
		m_ResistItem.Reset();
	}
	m_nResistInvenIndex = 0;	

	m_pBtnActive = m_pBtnExit;
	m_pPBarProcess->SetPos( 0 );
	m_pBtnAutoResist->SetEnable( true );	
}

void cTactics_Charge::RecvServer()
{
	m_bWaitRecvServer = false;
	m_bProcessing = true;

	cItemInfo *pii = g_pDataMng->GetInven()->GetData(m_nResistInvenIndex);

	// 아이템 빼주고
	assert_cs( pii->IsEnable() );
	pii->DecreaseCount( m_ResistItem.GetCount() );		

	_ProcessCancel();
}

//========================================================================================================
//
//	자동 등록
//
//========================================================================================================

void cTactics_Charge::_AutoResist()
{	
	int nOrgIndex = m_nSearchInvenIndex;

	// 인벤에서 부터 찾는다
	cData_Inven* pInven = g_pDataMng->GetInven();
	int nInvenSlotCount = pInven->GetInvenSlotCount();

	// 해당 되는 위치부터 찾는다.
	CsItem::sINFO* pFTItemInfo;
	for( int i=m_nSearchInvenIndex; i<nInvenSlotCount; ++i )
	{
		if( pInven->GetData( i )->IsEnable() == false )
			continue;

		pFTItemInfo = pInven->GetFTData( i );		

		if( IsResist( i ) == true )
		{
			m_nSearchInvenIndex = i+1;
			SetResist( i, -1 );
			return;
		}
	}

	// 못찾았다면
	// 처음부터 원래 있던 곳까지 찾는다.
	for( int i=0; i<nOrgIndex; ++i )
	{
		if( pInven->GetData( i )->IsEnable() == false )
			continue;

		pFTItemInfo = pInven->GetFTData( i );		

		if( IsResist( i ) == true )
		{
			m_nSearchInvenIndex = i+1;
			SetResist( i, -1 );
			return;
		}
	}

	// 그래도 못찾았다면 없는거다
	m_nSearchInvenIndex = 0;
}

bool cTactics_Charge::IsResist( int nInvenIndex )
{
	if( m_bProcessing == true )
	{
		cPrintMsg::PrintMsg( 10004 );
		return false;
	}

	if( m_bWaitRecvServer == true )
	{
		cPrintMsg::PrintMsg( 10004 );
		return false;
	}

	if( g_pDataMng->IsItemLock( nInvenIndex ) == true )
		return false;

	cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( nInvenIndex );
	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() )->GetInfo();

	if( ( pFTItem->s_nType_L != nItem::Matrial2 ) &&
		( pFTItem->s_nType_L != nItem::Digitama2 ) &&
		( pFTItem->s_nType_L != nItem::Digitama3 ) )
	{	
		return false;
	}		
	return true;
}

void cTactics_Charge::SetResist( int nInvenIndex )
{
	SetResist( nInvenIndex, g_pDataMng->GetInven()->GetData( nInvenIndex )->GetCount() );
}

void cTactics_Charge::SetResist( int nInvenIndex, int nCount )
{
	assert_cs( m_bProcessing == false );
	if( m_ResistItem.IsEnable() )
	{
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_nResistInvenIndex ) );
	}

	m_nResistInvenIndex = nInvenIndex;
	g_pDataMng->ItemLock( TO_INVEN_SID( m_nResistInvenIndex ) );

	m_ResistItem = *g_pDataMng->GetInven()->GetData( nInvenIndex );
	if( nCount > 0 )
	{
		m_ResistItem.m_nCount = nCount;
	}
	m_pBtnActive = m_pBtnOk;	

	cWindow::PlaySound( "System\\interface_person_01.wav" );
}

#endif