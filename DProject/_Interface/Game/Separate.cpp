

#include "stdafx.h"
#include "Separate.h"

cSeparate*		g_pSeparate = NULL;

void cSeparate::NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, CsPoint pos, int nMinCount, int nMaxCount, sIFIcon* pIFIcon, int nIconValue1 )
{
	if( g_pGameIF->IsActiveWindow( wt, nWTValue1 ) == true )
	{
		if( g_pGameIF->_GetPointer( wt, nWTValue1 )->IsEnableWindow() == false )
			return;
	}

	DeleteInstance();

	assert_cs( g_pSeparate == NULL );
	g_pSeparate = NiNew cSeparate;
	g_pSeparate->Init( wt, nWTValue1, pos, nMinCount, nMaxCount, pIFIcon, nIconValue1 );
}

bool cSeparate::UpdateInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pSeparate )
	{
		if( ( g_pSeparate->GetTargetWindowType() == wt )&&( g_pSeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			g_pSeparate->Update(g_fDeltaTime);
			if( g_pSeparate->IsShowWindow() == false )
			{
				DeleteInstance();
				return false;
			}
			return true;
		}		
	}
	return false;
}

//bool cSeparate::IsInInstance()
//{
//	if( g_pSeparate )
//	{
//		return g_pSeparate->IsInWindow( CURSOR_ST.GetPos() );
//	}
//	return false;
//}

cBaseWindow::eMU_TYPE
cSeparate::Update_ForMouseInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	cBaseWindow::eMU_TYPE muReturn = MUT_OUT_WINDOW;
	if( g_pSeparate )
	{
		if( ( g_pSeparate->GetTargetWindowType() == wt )&&( g_pSeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			muReturn = g_pSeparate->Update_ForMouse();
		}		
		
		if( g_pSeparate )
		{
			if( g_pSeparate->IsShowWindow() == false )
			{
				DeleteInstance();			
			}
		}
		else
		{
			return MUT_OUT_WINDOW;
		}
	}
	return muReturn;
}

void cSeparate::RenderInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pSeparate )
	{
		if( ( g_pSeparate->GetTargetWindowType() == wt )&&( g_pSeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			g_pEngine->ScreenSpace();
			g_pSeparate->Render();
		}		
	}
}

bool cSeparate::DeleteInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pSeparate )
	{
		if( ( g_pSeparate->GetTargetWindowType() == wt )&&( g_pSeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			SAFE_NIDELETE( g_pSeparate );
			return true;
		}		
	}
	return false;
}

bool cSeparate::DeleteInstance()
{
	if( g_pSeparate )
	{
		SAFE_NIDELETE( g_pSeparate );
		return true;
	}
	return false;
}

void cSeparate::Destroy()
{
	cBaseWindow::Delete();
}

void cSeparate::DeleteResource()
{
	m_pEditCount->ReleaseFocus();	
	DeleteScript();

	SAFE_NIDELETE( m_pEditCount );
}

void cSeparate::Init( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, CsPoint pos, int nMinCount, int nMaxCount, sIFIcon* pIFIcon, int nIconValue1 )
{
	if( cBaseWindow::Init() == false )
		return;
	if( wt == cBaseWindow::WT_EVENT_PINOKIMON )	// 피노키몬 카드이벤트 투표 UI
	{
		InitScript( "PinochimonCard\\Card_Vote_Number_Back.tga", pos, CsPoint( 303, 99 ), false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
		wstring msg = UISTRING_TEXT("PINOKIMON_GAME_COUNTERNUM");// 투표수
		AddTitle( const_cast<TCHAR*>(msg.c_str()) );

		msg = UISTRING_TEXT("PINOKIMON_GAME_BUTTON_SELECT");// 투표
		m_pBtnOk = AddButton( CsPoint( 211, 47 ), CsPoint( 74, 36 ), cButton::IMAGE_PINOKIMON_EVENT_VOTE, const_cast<TCHAR*>(msg.c_str()) );
		m_pBtnCancel = AddButton( CsPoint( 279, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

		m_pBtnDecrease = AddButton( CsPoint( 52, 52 ), CsPoint( 17, 24 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Slow_L.tga" );
		m_pBtnDecreaseEnd = AddButton( CsPoint( 18, 52 ), CsPoint( 28, 24 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Fast_L.tga" );
		m_pBtnDecrease->SetEnable( false );
		m_pBtnDecreaseEnd->SetEnable( false );

		m_pBtnIncrease = AddButton( CsPoint( 151, 52 ), CsPoint( 17, 24 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Slow_R.tga" );
		m_pBtnIncreaseEnd = AddButton( CsPoint( 173, 52 ), CsPoint( 28, 24 ), CsPoint( 0, 24 ), "PinochimonCard\\Card_Vote_Number_Arrow_Fast_R.tga" );
		
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		ti.SetText( nMinCount );
		ti.s_eTextAlign = DT_CENTER;

		m_pEditCount = NiNew cEditBox;
		m_pEditCount->Init( m_pRoot, CsPoint( 86, 54 ), CsPoint( 46, 18 ), &ti, false );
		m_pEditCount->SetFontLength( 14 );

		m_nCurCount = nMinCount;
	}
	else
	{
		CsPoint SepPos = pos + CsPoint( 0, -72 );
		if( SepPos.y < 0 )
		{
			SepPos = pos + CsPoint( 0, 32 );
		}
		if( SepPos.x + 125 > g_nScreenWidth )
		{
			SepPos.x -= ( 125 - 32 );
		}
		CsPoint ptEditPos, ptEditSize;

		InitScript( "Separate\\Seper_Win.tga", SepPos, CsPoint( 125, 72 ), false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );

		m_pBtnOk = AddButton( CsPoint( 15 ,43 ), CsPoint( 43, 21 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_OK" ).c_str() );
		m_pBtnCancel = AddButton( CsPoint( 67 ,43 ), CsPoint( 43, 21 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMON_TXT_CANCEL" ).c_str() );

		m_pBtnDecrease = AddButton( CsPoint( 4 ,14 ), CsPoint( 10, 21 ), CsPoint( 0, 21 ), "Separate\\Seper_Lbar.tga" );
		m_pBtnDecrease->SetEnable( false );
		m_pBtnIncrease = AddButton( CsPoint( 111 ,14 ), CsPoint( 10, 21 ), CsPoint( 0, 21 ), "Separate\\Seper_Rbar.tga" );
	
		//에디트박스 크기, 위치
		ptEditPos = CsPoint( 17, 14 );
		ptEditSize = CsPoint( 86, 21 );


		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		ti.SetText( 1 );
		ti.s_eTextAlign = DT_RIGHT;

		m_pEditCount = NiNew cEditBox;
		m_pEditCount->Init( m_pRoot, ptEditPos, ptEditSize, &ti, false );
		m_pEditCount->SetFontLength( 14 );

		m_nCurCount = nMinCount;

	}

	m_eTargetWindowType = wt;
	m_nTargetWTValue1 = nWTValue1;
	m_nMinCount = nMinCount;
	m_nMaxCount = nMaxCount;
	m_pIFIcon = pIFIcon;
	m_nIconValue1 = nIconValue1;
	m_bFirstIMEInput = false;

	if( g_pFocusEdit != NULL )
		g_pFocusEdit->ReleaseFocus();
	m_pEditCount->SetFocus();
	g_IME.SetNumberOnly( true );

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cSeparate::Update(float const& fDeltaTime)
{
	if( IsShowWindow() == false )
		return;

	if( m_pEditCount != g_pFocusEdit )
	{
		OnCancel();
		return;
	}

	_UpdateKeboard();

	// 현재 갯수에 따른 좌우 컨트롤의 변화
	if(m_nCurCount == 1 && m_nMaxCount == 1)
	{
		m_pBtnDecrease->SetEnable( false );
		m_pBtnIncrease->SetEnable( false );
	}
	else if( m_nCurCount == m_nMinCount )
	{
		m_pBtnDecrease->SetEnable( false );
		m_pBtnIncrease->SetEnable( true );
	}
	else if( m_nCurCount == m_nMaxCount )
	{
		m_pBtnDecrease->SetEnable( true );
		m_pBtnIncrease->SetEnable( false );
	}
	else
	{
		m_pBtnDecrease->SetEnable( true );
		m_pBtnIncrease->SetEnable( true );
	}
	if( m_eTargetWindowType == cBaseWindow::WT_EVENT_PINOKIMON )
	{
		if( m_nCurCount == m_nMinCount )
		{
			m_pBtnDecreaseEnd->SetEnable( false );
			m_pBtnIncreaseEnd->SetEnable( true );
		}
		else if( m_nCurCount == m_nMaxCount )
		{
			m_pBtnDecreaseEnd->SetEnable( true );
			m_pBtnIncreaseEnd->SetEnable( false );
		}
		else
		{
			m_pBtnDecreaseEnd->SetEnable( true );
			m_pBtnIncreaseEnd->SetEnable( true );
		}
	}

	// IME 입력으로 들어온거 처리
	// 첨에 들어오는 입력은 리셋 처리 - 기존 숫자 무시 다시 처음부터 기입
	// 처음 입력이 0이라면 무시 처리
	TCHAR szNum[ 32 ];
	g_IME.GetInput( szNum, 32 );
	int nInput = _ttoi( szNum );
	if(  m_nCurCount != nInput )
	{
		if( m_bFirstIMEInput == false )
		{
			// 1의자리 - 최근 입력 받은자리
			int nSrc = nInput%10;

			// 0입력이 아닐경우만
			if( nSrc != 0 )
			{
				m_nCurCount = nSrc;
				m_bFirstIMEInput = true;
			}
		}
		else
		{
			m_nCurCount = nInput;
		}

		if( m_nCurCount == 0 )
		{
			m_nCurCount = m_nMinCount;
			m_bFirstIMEInput = false;
		}
		else if( m_nCurCount > m_nMaxCount )
		{
			m_nCurCount = m_nMaxCount;
		}
		m_pEditCount->SetText( m_nCurCount );
	}
}

void cSeparate::_UpdateKeboard()
{
// 	NiInputKeyboard* pKeyboard = g_pEngine->m_pkKeyboard;
// 
// 	if( ( pKeyboard->KeyWasPressed( NiInputKeyboard::KEY_DOWN ) )||( pKeyboard->KeyWasPressed( NiInputKeyboard::KEY_LEFT ) ) )
// 	{
// 		if( m_nCurCount > m_nMinCount )
// 		{
// 			--m_nCurCount;
// 			m_KeyTimeSeq.SetDeltaTime( 400 );
// 			m_KeyTimeSeq.OffReset();
// 			m_pEditCount->SetText( m_nCurCount );
// 		}
// 	}
// 	else if( ( pKeyboard->KeyIsDown( NiInputKeyboard::KEY_DOWN ) )||( pKeyboard->KeyIsDown( NiInputKeyboard::KEY_LEFT ) ) )
// 	{
// 		if( m_KeyTimeSeq.IsEnable() == true )
// 		{
// 			if( m_nCurCount > m_nMinCount )
// 			{
// 				--m_nCurCount;
// 				m_KeyTimeSeq.SetDeltaTime( 200 );
// 				m_pEditCount->SetText( m_nCurCount );
// 			}
// 		}
// 	}
// 	else if( ( pKeyboard->KeyWasPressed( NiInputKeyboard::KEY_UP ) )||( pKeyboard->KeyWasPressed( NiInputKeyboard::KEY_RIGHT ) ) )
// 	{
// 		if( m_nCurCount < m_nMaxCount )
// 		{
// 			++m_nCurCount;
// 			m_KeyTimeSeq.SetDeltaTime( 400 );
// 			m_KeyTimeSeq.OffReset();
// 			m_pEditCount->SetText( m_nCurCount );
// 		}
// 	}
// 	else if( ( pKeyboard->KeyIsDown( NiInputKeyboard::KEY_UP ) )||( pKeyboard->KeyIsDown( NiInputKeyboard::KEY_RIGHT ) ) )
// 	{
// 		if( m_KeyTimeSeq.IsEnable() == true )
// 		{
// 			if( m_nCurCount < m_nMaxCount )
// 			{
// 				++m_nCurCount;
// 				m_KeyTimeSeq.SetDeltaTime( 200 );
// 				m_pEditCount->SetText( m_nCurCount );
// 			}
// 		}
// 	}
}

void cSeparate::OnOk()
{
	switch( m_eTargetWindowType )
	{
	case cBaseWindow::WT_INVENTORY:
		CURSOR_ST.SetIcon( CURSOR_ICON::CI_INVEN, m_nIconValue1, m_nCurCount, m_pIFIcon );
		break;
	case cBaseWindow::WT_WAREHOUSE:
#ifdef NEW_SHARESTASH
	{	
		if(m_nTargetWTValue1 == 0)
			CURSOR_ST.SetIcon( CURSOR_ICON::CI_WAREHOUSE, m_nIconValue1, m_nCurCount, m_pIFIcon );
		else if(m_nTargetWTValue1 == 1)
			CURSOR_ST.SetIcon( CURSOR_ICON::CI_SHARESTASH, m_nIconValue1, m_nCurCount, m_pIFIcon );
		break;
	}
#else
		CURSOR_ST.SetIcon( CURSOR_ICON::CI_WAREHOUSE, m_nIconValue1, m_nCurCount, m_pIFIcon );
#endif
	case cBaseWindow::WT_STORE:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_STORE ) )
		{
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::SEND_STORE_ITEMBUY, &m_nCurCount );
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case cBaseWindow::WT_NEW_TAMERSTATUS:
		CURSOR_ST.SetIcon( CURSOR_ICON::CI_CROSS, m_nIconValue1, m_nCurCount, m_pIFIcon );
		break;
#endif
	case cBaseWindow::WT_CARDINVENTORY:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CARDINVENTORY ) )
		{
			int nSelectedSealID;// 선택 카드 아이템ID
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::GET_SEALMASTER_SELECTSEALID, &nSelectedSealID );

			// UnSeal UI가 열려 있으면 닫아줍니다.
			g_pGameIF->CloseDynamicIF(cBaseWindow::WT_CARDIDENTIFY);

			if( g_pGameIF->GetDynamicIF( cBaseWindow::WT_CARDRESEAL, 0, nSelectedSealID) )// UI가 정상적으로 생성되었으면
				g_pGameIF->GetCardReSeal()->SetReSealText(nSelectedSealID , m_nCurCount);	// 카드ID, 수량
		}
		break;

	case cBaseWindow::WT_EVENT_PINOKIMON:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_EVENT_PINOKIMON ) )
		{
			// 서버 응답 대기
			cPrintMsg::PrintMsg( 10019 );

			//m_nIconValue1 : 카드 인덱스
			CsC_AvObject* pTarget = g_pGameIF->GetEvent_Pinokimon()->m_pTarget;
			CsNpc* pNpc = ((CNpc*)pTarget)->GetFTNpc();
			net::game->SendPinokimonVote( (u1)m_nIconValue1, (u4)m_nCurCount, (u4)(pNpc->GetInfo()->s_dwNpcID) );

			// 투표 정보 변경
			g_pGameIF->m_sEventInfo.s_nMyCnt[m_nIconValue1] += m_nCurCount;
			g_pGameIF->GetEvent_Pinokimon()->RefreshList();

			OnCancel();
		}
		return;
	default:
		assert_cs( false );
	}

	Close();
}

void cSeparate::OnCancel()
{
	if( m_eTargetWindowType == cBaseWindow::WT_EVENT_PINOKIMON )
	{
		// 인벤 락 해제
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_EVENT_PINOKIMON ) )
		{
			CsC_AvObject* pTarget = g_pGameIF->GetEvent_Pinokimon()->m_pTarget;
			CsNpc* pNpc = ((CNpc*)pTarget)->GetFTNpc();
			sNPC_TYPE_SPECIAL_EVENT* pNpcType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetType();
			for( int j = 0 ; j < pNpcType->GetItemCount() ; j++ )
			{
				int nItemID = pNpcType->GetItemID( j );
				g_pDataMng->GetInven()->ItemUnlock_ItemID( nItemID );
			}
		}
	}
	Close();
}

cBaseWindow::eMU_TYPE
cSeparate::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pBtnOk->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		OnOk();
		return muReturn;
	}
	
	if( m_pBtnCancel->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		OnCancel();
		return muReturn;
	}

	if( m_pBtnIncrease->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		++m_nCurCount;
		assert_cs( m_nCurCount <= m_nMaxCount );
		m_pEditCount->SetText( m_nCurCount );
		return muReturn;
	}
	if( m_pBtnDecrease->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		--m_nCurCount;
		assert_cs( m_nCurCount > 0 );
		m_pEditCount->SetText( m_nCurCount );
		return muReturn;
	}

	if( m_eTargetWindowType == cBaseWindow::WT_EVENT_PINOKIMON )
	{
		if( m_pBtnIncreaseEnd->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			m_nCurCount = m_nMaxCount;
			m_pEditCount->SetText( m_nCurCount );
			return muReturn;
		}
		if( m_pBtnDecreaseEnd->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			m_nCurCount = m_nMinCount;
			m_pEditCount->SetText( m_nCurCount );
			return muReturn;
		}
	}
	return muReturn;
}

void cSeparate::Render()
{
	if( IsShowWindow() == false )
		return;

	RenderScript();
	m_pEditCount->Render();
}

void cSeparate::ResetDevice()
{
	if( IsShowWindow() == false )
		return;

	ResetDeviceScript();
	m_pEditCount->ResetDevice();
}