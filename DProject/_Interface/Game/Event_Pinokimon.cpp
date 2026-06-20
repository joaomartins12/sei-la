#include "stdafx.h"

#include "Event_Pinokimon.h"

cEvent_Pinokimon::cEvent_Pinokimon()
{
	m_pTarget = NULL;
	m_nCheckTime = 0;
}

void cEvent_Pinokimon::Destroy()
{
	cBaseWindow::Delete();
}

void cEvent_Pinokimon::DeleteResource()
{
	// 투표 팝업 떠있으면 닫아주자( 인벤 락 해제 위해 cancel 호출 )
	if(	g_pSeparate )
	{
		if( g_pSeparate->GetTargetWindowType() == cBaseWindow::WT_EVENT_PINOKIMON )
		{
			g_pSeparate->OnCancel();
		}
	}
	cSeparate::DeleteInstance( GetWindowType(), 0 );

	DeleteScript();
	SAFE_NIDELETE( m_pBG );
	for( int i = 0 ; i < MAX_CARD_COUNT ; i++ )
	{
		SAFE_NIDELETE( m_pInsertBtn[i] );
		SAFE_NIDELETE( m_szAllCnt[i] );
		SAFE_NIDELETE( m_szMyCnt[i] );
	}
	SAFE_NIDELETE( m_szRemainTime );
	m_pTarget = NULL;
}

void cEvent_Pinokimon::Create( int nValue )
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint pSize = CsPoint( 619, 394 );
	CsPoint pPos = CsPoint( g_nScreenWidth/2 - pSize.x/2, g_nScreenHeight/2 - pSize.y/2 );

	InitScript( NULL, pPos , pSize, true, IFREGION_X::LEFT, IFREGION_Y::TOP );

	//배경
	m_pBG = NiNew cSprite;
	m_pBG->Init( m_pRoot, CsPoint::ZERO, pSize, "PinochimonCard\\Card_Vote_Back.tga", false );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem );
	ti.s_eTextAlign = DT_CENTER;
	int nTabX = 117;
	for( int i = 0 ; i < MAX_CARD_COUNT ; i++ )
	{
		// 카드별 투표 갯수( 전체 ) 텍스트
		m_szAllCnt[i] = NiNew cText;
		m_szAllCnt[i]->Init( m_pRoot, CsPoint( 76 + i*nTabX, 69 ), &ti, false );

		// 카드별 투표 갯수( 자신 ) 텍스트
		m_szMyCnt[i] = NiNew cText;
		m_szMyCnt[i]->Init( m_pRoot, CsPoint( 76 + i*nTabX, 119 ), &ti, false );

		// 카드이미지
		char szPath[ MAX_PATH ];
		sprintf_s( szPath, MAX_PATH, "PinochimonCard\\Card_Vote_Card_0%d.tga", i+1 );
		AddSprite( CsPoint( 22 + i*nTabX, 156 ), CsPoint( 107, 149 ), szPath );

		// 선택버튼
		m_pInsertBtn[i] = NiNew cButton;
		wstring msg = UISTRING_TEXT("PINOKIMON_GAME_SELECT");// 선택
		m_pInsertBtn[i]->Init( m_pRoot, CsPoint( 22 + i*nTabX, 331 ), CsPoint( 107, 50 ), cButton::IMAGE_PINOKIMON_EVENT_CHOICE, const_cast<TCHAR*>(msg.c_str()), false );
	}

	// 닫기버튼
	m_pCancelBtn = AddButton( CsPoint( 595, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

	// 다음 갱신 시간 알림 텍스트
	m_szRemainTime = NiNew cText;
	ti.s_eTextAlign = DT_LEFT;
	m_szRemainTime->Init( m_pRoot, CsPoint( 12, 9 ), &ti, false );

	ti.s_eTextAlign = DT_CENTER;
	TCHAR sz[6][512];
	std::wstring msg = UISTRING_TEXT("PINOKIMON_GAME_AGUMON_COUNTERNUM");// 선택
	_stprintf_s( sz[0], 512, _T( "%s" ), msg.c_str() );
	msg = UISTRING_TEXT("PINOKIMON_GAME_PAPIMON_COUNTERNUM");// 선택
	_stprintf_s( sz[1], 512, _T( "%s" ),msg.c_str() );
	msg = UISTRING_TEXT("PINOKIMON_GAME_VMON_COUNTERNUM");// 선택
	_stprintf_s( sz[2], 512, _T( "%s" ),msg.c_str() );
	msg = UISTRING_TEXT("PINOKIMON_GAME_GILMON_COUNTERNUM");// 선택
	_stprintf_s( sz[3], 512, _T( "%s" ),msg.c_str() );
	msg = UISTRING_TEXT("PINOKIMON_GAME_DOLMON_COUNTERNUM");// 선택
	_stprintf_s( sz[4], 512, _T( "%s" ),msg.c_str() );
	msg = UISTRING_TEXT("PINOKIMON_GAME_MY_COUNTERNUM");// 선택
	_stprintf_s( sz[5], 512, _T( "%s" ),msg.c_str() );

	// 각 카드의 전체 투표수
	CsPoint ptCardName = CsPoint( 76, 55 );
	for( int i = 0 ; i < MAX_CARD_COUNT ; i++ )
	{
		ti.SetText( sz[i] );
		AddText( &ti, ptCardName );
		ptCardName.x += nTabX;
	}

	// 자신이 투표한 수
	ptCardName = CsPoint( 76, 105 );
	for( int i = 0 ; i < MAX_CARD_COUNT ; i++ )
	{
		ti.SetText( sz[5] );
		AddText( &ti, ptCardName );
		ptCardName.x += nTabX;
	}

	RefreshList();
}

void cEvent_Pinokimon::Update(float const& fDeltaTime)
{
	//Npc 거리체크
	if( m_pTarget != NULL )
	{
		assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
		if( ( (CNpc*)m_pTarget )->CheckTamerDist() == false )
		{
			SetEnableWindow( true );
			Close();
			return;
		}
	}
	else
	{
		return;
	}

	// 시간 갱신
	if( m_nCheckTime < _TIME_TS )
	{
		// 갱신시간이 0초 이상으로 남았을 때 1초마다 갱신
		if( g_pGameIF->m_sEventInfo.s_nNextResetTime >= _TIME_TS)
			RefreshList();
		else	// 갱신 시간이 瑛릿?요청 보냄
		{
			net::game->SendPinokimonInfoReq();
			m_nCheckTime = _TIME_TS + 10;	// 요청은 10초에 한번씩만
		}
	}

	cSeparate::UpdateInstance( GetWindowType(), 0 );

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cEvent_Pinokimon::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cSeparate::Update_ForMouseInstance( GetWindowType(), 0 );	
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	//닫기버튼
	switch( m_pCancelBtn->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;
	default:
		return muReturn;
	}

	// 투표버튼
	for( int i = 0 ; i < MAX_CARD_COUNT ; i++ )
	{
		switch( m_pInsertBtn[i]->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				CsNpc* pNpc = ((CNpc*)m_pTarget)->GetFTNpc();
				sNPC_TYPE_SPECIAL_EVENT* pNpcType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetType();
				int nItemCnt = 0;
				for( int j = 0 ; j < pNpcType->GetItemCount() ; j++ )
				{
					int nItemID = pNpcType->GetItemID( j );
					nItemCnt += g_pDataMng->GetInven()->GetCount_Item_ID( nItemID );	// 해당 아이템 인벤 소지 수
					g_pDataMng->GetInven()->ItemLock_ItemID( nItemID );					// 해당 아이템 인벤 락
				}

				// 인벤에 아이템 없을 경우
				if( nItemCnt <= 0 )
				{
					cPrintMsg::PrintMsg( 30741 );	// '@<tc:990>인벤토리에 투표권 아이템이 없어 투표에 참여할 수 없습니다.@</tc>% 
					return muReturn;
				}

				// 투표 팝업창 UI
				CsPoint pPos = GetRootClient() + CsPoint( m_pRoot->GetSize().x/2, m_pRoot->GetSize().y/2 );
				CsPoint pSize = CsPoint( 125, 72 );
				pPos = CsPoint( pPos.x - pSize.x/2, pPos.y - pSize.y/2 );

				cSeparate::NewInstance( GetWindowType(), 0, pPos, 1, nItemCnt, NULL, i );
			}
			return muReturn;
		default:
			return muReturn;
		}
	}

	//윈도우 이동
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE(GetRoot()->GetSize().x, 260) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cEvent_Pinokimon::Render()
{
	m_pBG->Render();

	RenderScript();

	for( int i = 0 ; i < MAX_CARD_COUNT ; i++ )
	{
		m_pInsertBtn[i]->Render();
		m_szAllCnt[i]->Render();
		m_szMyCnt[i]->Render();
	}
	m_szRemainTime->Render();
	EndRenderScript();

	cSeparate::RenderInstance( GetWindowType(), 0 );
}

void cEvent_Pinokimon::ResetDevice()
{
	ResetDeviceScript();
	m_pBG->ResetDevice();

	for( int i = 0 ; i < MAX_CARD_COUNT ; i++ )
	{
		m_pInsertBtn[i]->ResetDevice();
		m_szAllCnt[i]->ResetDevice();
		m_szMyCnt[i]->ResetDevice();
	}
	m_szRemainTime->ResetDevice();
}

void cEvent_Pinokimon::RefreshList()
{
	sEvent_PinokimonInfo pInfo = g_pGameIF->m_sEventInfo;
	for( int i = 0 ; i < MAX_CARD_COUNT ; i++ )
	{
		m_szAllCnt[i]->SetText( pInfo.s_nAllCnt[i] );
		m_szMyCnt[i]->SetText( pInfo.s_nMyCnt[i] );
	}

	// 다음 갱신 시간
	TCHAR sz[ 512 ];
	TCHAR szTime[ 64 ];
	_stprintf_s( szTime, 64, cTooltip::Time2Str( g_pGameIF->m_sEventInfo.s_nNextResetTime , _TIME_TS, cTooltip::TT_SIMPLE ) );

	std::wstring msg = UISTRING_TEXT("PINOKIMON_GAME_RENEW_TIME");// 선택
	_stprintf_s( sz, 512, msg.c_str(), szTime );

	m_szRemainTime->SetText( sz );

	m_nCheckTime = _TIME_TS;
}

void cEvent_Pinokimon::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
	m_pBG->ResetDevice();

	for( int i = 0 ; i < MAX_CARD_COUNT ; i++ )
	{
		m_pInsertBtn[i]->ResetDevice();
		m_szAllCnt[i]->ResetDevice();
		m_szMyCnt[i]->ResetDevice();
	}
	m_szRemainTime->ResetDevice();
}

void cEvent_Pinokimon::SetTarget( CsC_AvObject* pTarget )
{
	assert_cs( pTarget != NULL );
	//타겟 셋팅
	m_pTarget = pTarget;

	//NPC테이블에서 기부아이템 가져오기 위해 정보 얻어옴
	CNpc* pNpc = (CNpc*)pTarget;
}
