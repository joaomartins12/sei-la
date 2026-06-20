#include "stdafx.h"

#include "MastersMatching.h"

#define IF_MASTERSMATCHING_ITEMPOS			CsPoint( 364, 448 )
#define IF_MASTERSMATCHING_EDITBOX_POS	CsPoint( 364, 533 )
#define IF_MASTERSMATCHING_EDITBOX_SIZE	CsPoint( 32, 13 )

#define IF_DIGIBALL_COUNT_EDIT_ID			5009		// _EDIT_ID 로 검색후 사용

//갯수 입력 에디트박스 콜백함수
LRESULT CALLBACK EditSubProc_MastersMatching(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
WNDPROC EditProc_MastersMatching;

LRESULT CALLBACK EditSubProc_MastersMatching(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{	
	case EM_UNDO:
		{
			return 0;
		}
		break;	
	}

	return CallWindowProc(EditProc_MastersMatching,hWnd,uMsg,wParam,lParam);
}

cMastersMatching::cMastersMatching()
{
	m_pTarget = NULL;

	m_hEditBrush = NULL;
	m_hEditFont = NULL;
	m_hEditCnt = NULL;

	m_pCountEditBox = NULL;
}

void cMastersMatching::Destroy()
{
	cBaseWindow::Delete();
}

void cMastersMatching::DeleteResource()
{
#ifdef UI_ITEMSCAN_RENEWAL
	if( m_pCountEditBox )
		m_pCountEditBox->ReleaseFocus();
#endif
	g_pDataMng->ReleaseItemLock();
	DeleteScript();
	DeleteObject( m_hEditBrush );
	DeleteObject( m_hEditFont );
	::DestroyWindow( m_hEditCnt );

	SAFE_NIDELETE( m_pBG );
// 	SAFE_NIDELETE( m_pTitle );
// 	SAFE_NIDELETE( m_pAlphaTeam );
// 	SAFE_NIDELETE( m_pOmegaTeam );
	SAFE_NIDELETE( m_pATeamScore );
	SAFE_NIDELETE( m_pATeamRank );
	SAFE_NIDELETE( m_pBTeamScore );
	SAFE_NIDELETE( m_pBTeamRank );
	SAFE_NIDELETE( m_pMyScore );
	SAFE_NIDELETE( m_pInsertBG );

	SAFE_NIDELETE( m_pInsertBtn );
	SAFE_NIDELETE( m_szInsertBtn );
	SAFE_NIDELETE( m_szMyCnt );
	SAFE_NIDELETE( m_szMyRank );
	SAFE_NIDELETE( m_szMyTeam );
	SAFE_NIDELETE( m_szTeam_Cnt_A );
	SAFE_NIDELETE( m_szTeam_Cnt_B );
	SAFE_NIDELETE( m_szItemName );
	m_pTarget = NULL;
}

void cMastersMatching::Create( int nValue )
{
	cBaseWindow::Init();
	int nPosX = g_nScreenWidth/2 - 385;
	int nPosY = g_nScreenHeight/2 - 308;
	
	if( nPosX < 0 )
		nPosX = 0;
	if( nPosY < 0 )
		nPosY = 0;

	InitScript( NULL, CsPoint( nPosX, nPosY ) , CsPoint( 770, 616 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP );

	//배경
	m_pBG = NiNew cSprite;
	m_pBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( 770, 616 ), "MastersMatch\\MastersMatch_BG.tga", false );

	//타이틀
// 	m_pTitle = NiNew cSprite;
// 	m_pTitle->Init( m_pRoot, CsPoint( 175, 0 ), CsPoint( 200, 100 ), "MastersMatch\\MastersMatch_Title.tga", false );
//	AddTitle( _LAN("마스터즈 매치") );

	//A팀 - 블랙 알파몬
// 	m_pAlphaTeam = NiNew cSprite;
// 	m_pAlphaTeam->Init( m_pRoot, CsPoint( -100, -100 ), CsPoint( 150, 250 ), "MastersMatch\\MastersMatch_Alphamon.tga", false );
	//B팀 - 화이트 오메가몬
// 	m_pOmegaTeam = NiNew cSprite;
// 	m_pOmegaTeam->Init( m_pRoot, CsPoint( 500, -100 ), CsPoint( 150, 250 ), "MastersMatch\\MastersMatch_Omegamon.tga", false );

	//A팀 스코어보드
	m_pATeamScore = NiNew cSprite;
	m_pATeamScore->Init( m_pRoot, CsPoint( 115, 238 ), CsPoint( 132, 85 ), "MastersMatch\\MastersMatch_TeamScore_A.tga", false );
	//B팀 스코어보드
	m_pBTeamScore = NiNew cSprite;
	m_pBTeamScore->Init( m_pRoot, CsPoint( 525, 238 ), CsPoint( 132, 85 ), "MastersMatch\\MastersMatch_TeamScore_B.tga", false );

	//A팀 랭킹
	m_pATeamRank = NiNew cSprite;
	m_pATeamRank->Init( m_pRoot, CsPoint( 83, 345 ), CsPoint( 188, 231 ), "MastersMatch\\MastersMatch_Rank.tga", false );

	//B팀 랭킹
	m_pBTeamRank = NiNew cSprite;
	m_pBTeamRank->Init( m_pRoot, CsPoint( 501, 345 ), CsPoint( 188, 231 ), "MastersMatch\\MastersMatch_Rank.tga", false );

	//본인 스코어보드
	m_pMyScore = NiNew cSprite;
	m_pMyScore->Init( m_pRoot, CsPoint( 308, 255 ), CsPoint( 159, 99 ), "MastersMatch\\MastersMatch_MyScore.tga", false );

	//기부 아이템 넣는 배경
	m_pInsertBG = NiNew cSprite;
	m_pInsertBG->Init( m_pRoot, CsPoint( 285, 375 ), CsPoint( 196, 197 ), "MastersMatch\\MastersMatch_InsertBG.tga", false );

	//닫기버튼
	m_pCancelBtn = AddButton( CsPoint( 667, 215 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );

	//디지볼 기부 버튼
	m_pInsertBtn = NiNew cButton;
	m_pInsertBtn->Init( m_pRoot, CsPoint( 340, 495 ), CsPoint( 81, 23 ), "MastersMatch\\MastersMatch_InsertBtn.tga", false );
	m_pInsertBtn->SetTexToken( CsPoint( 0, 23 ) );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem );
	ti.s_eTextAlign = DT_CENTER;
	//ti.SetText( _T("???") );

	// 기부 버튼 텍스트
	ti.SetText( UISTRING_TEXT( "MASTERS_MATCH_BTN_INSERT" ).c_str() );
	m_szInsertBtn = NiNew cText;
	m_szInsertBtn->Init( m_pRoot, m_pInsertBtn->GetDefaultPos() + CsPoint( 40, 5 ) , &ti, false );
	ti.s_eTextAlign = DT_RIGHT;

	std::wstring wsText;
	DmCS::StringFn::Format( wsText, L"%s :", UISTRING_TEXT( "COMMON_TXT_EVENT_ITEM" ).c_str() );
	AddText( &ti, CsPoint( 390, 300 ) );

	DmCS::StringFn::Format( wsText, L"%s :", UISTRING_TEXT( "MASTERS_MATCH_RANKING" ).c_str() );
	ti.SetText( wsText.c_str() );
	AddText( &ti, CsPoint( 390, 320 ) );

	//본인 팀
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( g_pGameIF->m_sMyInfo.s_eMyTeam );
	ti.s_eFontSize = CFont::FS_12;
	m_szMyTeam = NiNew cText;
	m_szMyTeam->Init( m_pRoot, CsPoint( 390, 275 ), &ti, false );

	//본인 기부 갯수
	ti.s_eTextAlign = DT_LEFT;
	ti.s_eFontSize = CFont::FS_10;
	ti.SetText( 0 );
	m_szMyCnt = NiNew cText;
	m_szMyCnt->Init( m_pRoot, CsPoint( 391, 300 ), &ti, false );

	//본인 랭크
	ti.SetText( 0 );
	m_szMyRank = NiNew cText;
	m_szMyRank->Init( m_pRoot, CsPoint( 391, 320 ), &ti, false );

	ti.s_eTextAlign = DT_CENTER;
	//1팀 기부 갯수
	ti.SetText( 0 );
// 	ti.SetText( _LAN( "A팀" ) );
// 	AddText( &ti, CsPoint( 185, 270 ) );
// 	ti.SetText( 2354 );
	m_szTeam_Cnt_A = NiNew cText;
	m_szTeam_Cnt_A->Init( m_pRoot, CsPoint( 182, 293 ), &ti, false );

	//2팀 기부 갯수
// 	ti.SetText( _LAN( "B팀" ) );
	m_szTeam_Cnt_B = NiNew cText;
	m_szTeam_Cnt_B->Init( m_pRoot, CsPoint( 590, 293 ), &ti, false );

	//기부아이템 이름
	ti.SetText( _T( " " ) );
	m_szItemName = NiNew cText;
	m_szItemName->Init( m_pRoot, CsPoint( 380, 406 ), &ti, false );

	//증가 감소 버튼
	m_pDecreaseBtn = AddButton( CsPoint( 344 ,532 ), CsPoint( 13, 21 ), CsPoint( 0, 21 ), "MastersMatch\\MastersMatch_LeftBtn.tga" );
	m_pDecreaseBtn->SetEnable( false );
	m_pDecreaseBtn->SetVisible(true);
	m_pIncreaseBtn = AddButton( CsPoint( 403 , 532), CsPoint( 13, 21 ), CsPoint( 0, 21 ), "MastersMatch\\MastersMatch_RightBtn.tga" );
	m_pIncreaseBtn->SetVisible(true);
	m_pDecreaseEndBtn = AddButton( CsPoint( 319 , 532 ), CsPoint( 15, 21 ), CsPoint( 0, 21 ), "MastersMatch\\MastersMatch_Left2Btn.tga" );	
	m_pDecreaseEndBtn->SetEnable( false );
	m_pDecreaseEndBtn->SetVisible(true);
	m_pIncreaseEndBtn = AddButton( CsPoint( 426 , 532 ), CsPoint( 15, 21 ), CsPoint( 0, 21 ), "MastersMatch\\MastersMatch_Right2Btn.tga" );
	m_pIncreaseEndBtn->SetVisible(true);

	//에디트박스 생성
	_CreateEditBox( IF_MASTERSMATCHING_EDITBOX_POS/* + GetRootClient()*/, IF_MASTERSMATCHING_EDITBOX_SIZE );	

	m_nMaxCount = 0;
	_SetCount( 0, true );

	if( nValue == 1 )//서버에게 데이터 안받고 그냥 연 상태, 가진 데이터 셋팅해줘야되
		RefreshList();
}

void cMastersMatching::_CreateEditBox( CsPoint ptPos, CsPoint ptSize )
{
	cText::sTEXTINFO ti;
	ti.SetText(_T(""));
	ti.Init( &g_pEngine->m_FontText, CFont::FS_9 );
	ti.s_eTextAlign = DT_CENTER;
	m_pCountEditBox = NiNew cEditBox;
	m_pCountEditBox->Init( m_pRoot, ptPos, ptSize, &ti, false );
	m_pCountEditBox->SetFontLength( 4 );
	m_pCountEditBox->EnableUnderline();
	m_pCountEditBox->AddEvent( cEditBox::eEditbox_ChangeText, this, &cMastersMatching::ChangEditText );
	AddChildControl( m_pCountEditBox );

	if( g_pFocusEdit != NULL )
		g_pFocusEdit->ReleaseFocus( false );

	g_IME.SetLockConidateList( true );
	g_IME.SetNumberOnly( true );
	m_pCountEditBox->SetFocus();

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value = 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
}

void cMastersMatching::_SetCount( int nCount, bool bSetEditControl )
{
	if( nCount > m_nMaxCount )
		nCount = m_nMaxCount;
	else if( nCount < 0 )
		nCount = 0;

	if( nCount == 0 )
	{
		m_nMaxCount = 0;

		//락 해제
		if( g_pDataMng->IsItemAllLock() )
			g_pDataMng->ReleaseItemLock();
	}

	if( bSetEditControl )
	{
		if( m_pCountEditBox )
			m_pCountEditBox->SetText( nCount );
	}

	m_nItemCnt = nCount;
}

int cMastersMatching::_GetCount()
{
	return _ttoi(m_pCountEditBox->GetString());
}

bool cMastersMatching::OnChangeEdit( DWORD nCtrlID )
{
	HWND hWnd = NULL;
	switch( nCtrlID )
	{
	case IF_DIGIBALL_COUNT_EDIT_ID:		hWnd = m_hEditCnt;		break;
	default:											return false;
	}

	if( m_nItemCnt == 0 )
	{
		_SetCount( 0, true );
		return true;
	}
	TCHAR sz[ 16 ];
	GetWindowText( hWnd, sz, 16 );

	int nCurCnt = _ttoi( sz );
	if( nCurCnt > m_nMaxCount  )
		_SetCount( m_nMaxCount, true );
	else
		_SetCount( nCurCnt, false );

	return true;
}

void cMastersMatching::ChangEditText(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );

	TCHAR* pInputData = static_cast<TCHAR*>(pData);
	int InputItemCount = _ttoi( pInputData );

	_SetCount(InputItemCount, true);
}

LRESULT cMastersMatching::ApplyEditColor( HWND hEdit, HDC dc )
{
	if( hEdit != m_hEditCnt )
		return NULL;

	SetTextColor( dc, RGB( 255, 255, 255 ) );
	SetBkColor( dc, RGB( 0, 16, 36 ) );
	return (LRESULT)m_hEditBrush;
}

void cMastersMatching::Update(float const& fDeltaTime)
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
		//타겟 없으면 타겟 잡아줘
		m_pTarget = g_pGameIF->GetMastersMatchTarget();
	}

	//버튼 상태 셋팅
	m_pIncreaseBtn->SetEnable( ( _GetCount() < m_nMaxCount ) || ( m_ResistItem.IsEnable() == false ) );
	m_pIncreaseEndBtn->SetEnable( ( _GetCount() < m_nMaxCount ) || ( m_ResistItem.IsEnable() == false ) );
	m_pDecreaseBtn->SetEnable( _GetCount() > 0 );
	m_pDecreaseEndBtn->SetEnable( _GetCount() > 0 );

	if( _GetCount() <= 0 )
		m_ResistItem.Clear();

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE
cMastersMatching::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
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

	//기부버튼
	switch( m_pInsertBtn->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			//기부아이템 등록상태 확인
			if( m_ResistItem.IsEnable() == true && m_nItemCnt > 0 )//등록되어 있으면 요청준비
			{
				//유저인덱스
				UINT nUserID =  g_pCharMng->GetTamerUser()->GetUniqID();

				//Npc번호
				CsC_AvObject* m_pTarget = NULL;
				m_pTarget = g_pGameIF->GetMastersMatchTarget();
				DWORD nNpcID = ((CNpc*)m_pTarget)->GetFTNpc()->GetInfo()->s_dwNpcID;

				//요청
				// Npc테이블의 해당 기부아이템 인덱스
				int nFTNpcIdx = m_pFTInsertItem->IsItem( m_ResistItem.m_nType );
				assert_cs( nFTNpcIdx != -1 );//Npc 테이블에 없는데 왔으니 문제가 있네
				net::game->SendMastersMatchInsert( nUserID, nNpcID, m_nItemSlot, m_nItemCnt, (u1)nFTNpcIdx );

				//데이터 수신중 메세지
				cPrintMsg::PrintMsg( 10019 );
			}
			else
			{
				//cPrintMsg::PrintMsg(  );
				//아이템이 등록되지 않았습니다 메세지
			}
		}
		return muReturn;
	default:
		return muReturn;
	}

	if( _GetCount() > 0 )//아이템 등록상태
	{
		if( m_pIncreaseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			_SetCount( _GetCount() + 1, true );		// +1
			return muReturn;
		}
		if( m_pDecreaseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			_SetCount( _GetCount() - 1, true );		// -1
			return muReturn;
		}
		if( m_pDecreaseEndBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			_SetCount( 1, true );
			return muReturn;
		}
		if( m_pIncreaseEndBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			_SetCount( m_nMaxCount, true );		// Max
			return muReturn;
		}
	}
	else	// 아이템 미등록 상태
	{
		if( m_pIncreaseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			//인벤에서 기부아이템 찾아야되
			int nCnt = m_pFTInsertItem->GetItemCount();
			if( _FindItem( nCnt, false ) )
				return muReturn;
		}
		if( m_pIncreaseEndBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			//인벤에서 기부아이템 찾아야되
			int nCnt = m_pFTInsertItem->GetItemCount();
			if( _FindItem( nCnt, true ) )
				return muReturn;
		}

	}

	//기부아이템 툴팁
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient() + IF_MASTERSMATCHING_ITEMPOS , CsSIZE( 32, 32) ) ) != CURSOR::BUTTON_OUTWINDOW 
		&& m_ResistItem.IsEnable() )
	{
		if(CURSOR_ST.GetButtonState() == CURSOR::RBUTTON_DOWN )//우클릭
		{
			_SetCount( 0, true );
			return muReturn;
		}

		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos() + CsPoint( 20, 20 ) , CsPoint::ZERO, TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_ResistItem.m_nType, cBaseWindow::WT_MASTERS_MATCHING, 0, 0, &m_ResistItem );
	}


	//윈도우 이동
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE(GetRoot()->GetSize().x, 260) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cMastersMatching::Render()
{
	//배경 애들 먼저 그림
	m_pBG->Render();
// 	m_pTitle->Render();
// 	m_pAlphaTeam->Render();
// 	m_pOmegaTeam->Render();
	m_pATeamScore->Render();
	m_pATeamRank->Render();
	m_pBTeamScore->Render();
	m_pBTeamRank->Render();
	m_pMyScore->Render();
	m_pInsertBG->Render();
	
	RenderScript();

	m_pInsertBtn->Render();
	m_szInsertBtn->Render();
	m_szMyTeam->Render();
	m_szMyCnt->Render();
	m_szMyRank->Render();
	m_szTeam_Cnt_A->Render();
	m_szTeam_Cnt_B->Render();

	m_pRankList_A->Render( CsPoint( 94, 354 ) + GetRootClient(), 8 );
	m_pRankList_B->Render( CsPoint( 512, 354 ) + GetRootClient(), 8 );

	if( m_ResistItem.IsEnable() && _GetCount() > 0 )
	{
		g_pIconMng->RenderItem( &m_ResistItem, IF_MASTERSMATCHING_ITEMPOS + GetRootClient(), false );
		m_szItemName->Render();
	}

	EndRenderScript();
}

void cMastersMatching::ResetDevice()
{
	ResetDeviceScript();
	m_pBG->ResetDevice();
// 	m_pTitle->ResetDevice();
// 	m_pAlphaTeam->ResetDevice();
// 	m_pOmegaTeam->ResetDevice();
	m_pATeamScore->ResetDevice();
	m_pATeamRank->ResetDevice();
	m_pBTeamScore->ResetDevice();
	m_pBTeamRank->ResetDevice();
	m_pMyScore->ResetDevice();
	m_pInsertBG->ResetDevice();

	m_pInsertBtn->ResetDevice();
	m_szInsertBtn->ResetDevice();
	m_szMyTeam->ResetDevice();
	m_szMyCnt->ResetDevice();
	m_szMyRank->ResetDevice();
	m_szTeam_Cnt_A->ResetDevice();
	m_szTeam_Cnt_B->ResetDevice();
	m_szItemName->ResetDevice();

	m_pIncreaseBtn->ResetDevice();
	m_pIncreaseEndBtn->ResetDevice();
	m_pDecreaseBtn->ResetDevice();
	m_pDecreaseEndBtn->ResetDevice();
}

void cMastersMatching::_SetTeamCnt( int nTeamIdx, u4 nCnt )
{
	if( nTeamIdx == A_TEAM ) 
		m_szTeam_Cnt_A->SetText( (int)nCnt ); 
	else if( nTeamIdx == B_TEAM )
		m_szTeam_Cnt_B->SetText( (int)nCnt );
	else
		assert_cs(false);//팀 이상해
}
void cMastersMatching::_SetMyCnt( u4 nBallCnt, u2 nRank )
{
	m_szMyCnt->SetText( (int)nBallCnt );
	if( nRank == 0 )//아직 서버에 업데이트 안된 유저
		m_szMyRank->SetText( UISTRING_TEXT( "MASTERS_MATCH_NOT_RANKED" ).c_str() );
	else
		m_szMyRank->SetText( (int)nRank );
}

bool cMastersMatching::IsResist( int nInvenIndex )
{	
	if( IsEnableWindow() == false )
	{
		cPrintMsg::PrintMsg( 10004 );//지금은 사용할 수 없습니다
		return false;
	}

	cItemInfo* pInfo = g_pDataMng->GetInven()->GetData( nInvenIndex );
	CsItem::sINFO* pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( pInfo->GetType() )->GetInfo();
	// 기부아이템 체크
	if( m_pFTInsertItem->IsItem( pFTItemInfo->s_dwItemID ) == -1 )
	{
		cPrintMsg::PrintMsg( 11014 );//해당 슬롯에 등록할 수 없습니다
		return false;
	}	

	return true;
}

void cMastersMatching::SetResist( int nInvenIndex )
{
	SetResist( nInvenIndex, g_pDataMng->GetInven()->GetData( nInvenIndex )->GetCount() );
}
void cMastersMatching::SetResist( int nInvenIndex, int nCount )
{
	// - 아이템 등록
	//인벤 번호
	m_nItemSlot = nInvenIndex;
	//등록한 아이템 인벤에서 락
	g_pDataMng->ItemLock( TO_INVEN_SID( nInvenIndex ) );

	//아이템정보 가져옴
	m_ResistItem = *g_pDataMng->GetInven()->GetData( nInvenIndex );
	if( nCount > 0 ) // 1개 이상일 때만 등록 ( 항상 1 이상이겠지만 한번더 확인 )
	{
		//해당 인벤 최대치를 Max로 설정
		m_nMaxCount = m_ResistItem.m_nCount;
		//지정된 갯수로 아이템 갯수 셋팅
		m_nItemCnt = nCount;
		//등록된 아이템정보에 지정된 갯수 셋팅
		m_ResistItem.m_nCount = nCount;
		//에디트박스 셋팅
		_SetCount( nCount, true );

		//아이템 이름 셋팅
		TCHAR		szName[ ITEM_NAME_LEN ];
		cStringList* pStrList = NiNew cStringList;
		cText::sTEXTINFO ti;
		ti.Init();

		int nItemID = m_ResistItem.m_nType;
		_tcscpy_s( szName, ITEM_NAME_LEN, nsCsFileTable::g_pItemMng->GetItem( nItemID )->GetInfo()->s_szName );

		g_pStringAnalysis->Cut_LimitWigth( pStrList, 158, szName, &ti );

		cString::sELEMENT* pEle = pStrList->GetString( 0 )->GetElement( 0 );
		_tcscpy_s( szName, ITEM_NAME_LEN, ( (cString::sTEXT*)pEle )->s_Text.GetText() );

		m_szItemName->SetText( szName );
	}

	cWindow::PlaySound( "System\\interface_person_01.wav" );
}

bool cMastersMatching::_FindItem( int nFTItemCnt, bool bIsMax )	// NPC테이블에 있는 기부아이템이 인벤에 존재하는지 확인
{
	for( int i = 0 ; i < nFTItemCnt ; i++ )
	{
		//기부아이템 번호
		int nItemID =  m_pFTInsertItem->GetItemID( i );

		//해당 아이템 있는지 확인
		int nSlot = 	g_pDataMng->GetInven()->GetFirstSlot_Item_ID( nItemID );

		//존재할 경우
		if( nSlot != cData_Inven::INVALIDE_INVEN_INDEX )
		{
			//해당 아이템 등록 후 동작
			if( bIsMax ) // 최대 증가 버튼일 경우
				SetResist( nSlot );
			else	
				SetResist( nSlot, 1 );
			return true;
		}
	}
	return false;
}
void cMastersMatching::_CheckAchieve( u4 nCount )
{
	//기부 했던 갯수 + 이번에 기부 할 갯수 
	u4 nCurCnt = g_pGameIF->m_sMyInfo.s_nMyCnt + nCount;

	// 업적 체크 - 해당 
	GS2C_RECV_CHECKTYPE recv;
	recv.nType = AchieveContents::CA_Masters_Match;
	recv.nValue1 = nCurCnt;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::ACHIEVE_SET_CHECKTYPE, &recv );
}
void cMastersMatching::RefreshList()
{
	//각 팀 전체 갯수
	for(int i = 0 ; i < B_TEAM ; i ++)
		_SetTeamCnt( i+1, g_pGameIF->m_sRankInfo.s_nCnt[ i ] );

	//팀별 랭커
	_SetRank( g_pGameIF->m_sRankInfo.s_pRankList_A, g_pGameIF->m_sRankInfo.s_pRankList_B );

	//본인 랭킹, 갯수 업데이트
	u4 nMyCnt = g_pGameIF->m_sMyInfo.s_nMyCnt;
	u4 nMyRank = g_pGameIF->m_sMyInfo.s_nMyRank;
	_SetMyCnt(	nMyCnt, nMyRank );

	//본인 팀 이름 셋팅
	if( g_pGameIF->m_sMyInfo.s_eMyTeam == A_TEAM )
		m_szMyTeam->SetText( UISTRING_TEXT( "MASTERSMATCH_A_TEAM_NAME" ).c_str() );
	else if( g_pGameIF->m_sMyInfo.s_eMyTeam == B_TEAM )
		m_szMyTeam->SetText( UISTRING_TEXT( "MASTERSMATCH_B_TEAM_NAME" ).c_str() );

	//리셋 시간 셋팅
	TCHAR cTmpTime[ 32 ];
	memset( m_szResetTime, 0, 32 );
	memset( cTmpTime, 0, 32 );
	_tcscpy_s( cTmpTime, 32, LanConvertT( g_pGameIF->m_sMyInfo.s_szNextResetTime ) );
	//초를 자르라고해서 자름... 
	for( int i = 0; i < 5 ; i++ )
		m_szResetTime[ i ] = cTmpTime[ i ];

	//경고메세지
	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_11, FONT_RED );
	ti.s_eTextAlign = DT_CENTER;
	ti.s_bOutLine = true;

	std::wstring wsUpdate = UISTRING_TEXT( "MASTERS_MATCH_UPDATE_TIME" );
	DmCS::StringFn::Replace( wsUpdate, L"#Time#", m_szResetTime );
	ti.SetText( wsUpdate.c_str() );
	AddText( &ti, CsPoint( 380, 575 ) );

}
bool cMastersMatching::CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			if( g_pDataMng->GetInven()->GetData( nIconSlot )->GetCount() != nIconCount )
				cPrintMsg::PrintMsg( 20030 ); // 분리한 아이템은 바로 회수할 수 없습니다. 인벤토리에 옮긴 후 회수하시기 바랍니다

			if( IsResist( nIconSlot ) == true )
				SetResist( nIconSlot, nIconCount );
			
			CURSOR_ST.ReleaseIcon();

			return true;
		}
		break;
	}

	return false;
}

void cMastersMatching::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
	m_pBG->ResetDevice();
// 	m_pTitle->ResetDevice();
// 	m_pAlphaTeam->ResetDevice();
// 	m_pOmegaTeam->ResetDevice();
	m_pATeamScore->ResetDevice();
	m_pATeamRank->ResetDevice();
	m_pBTeamScore->ResetDevice();
	m_pBTeamRank->ResetDevice();
	m_pMyScore->ResetDevice();
	m_pInsertBG->ResetDevice();

	m_pInsertBtn->ResetDevice();
	m_szInsertBtn->ResetDevice();
	m_szMyTeam->ResetDevice();
	m_szMyCnt->ResetDevice();
	m_szMyRank->ResetDevice();
	m_szTeam_Cnt_A->ResetDevice();
	m_szTeam_Cnt_B->ResetDevice();
	m_szItemName->ResetDevice();

	m_pIncreaseBtn->ResetDevice();
	m_pIncreaseEndBtn->ResetDevice();
	m_pDecreaseBtn->ResetDevice();
	m_pDecreaseEndBtn->ResetDevice();

	CsPoint ptPos = IF_MASTERSMATCHING_EDITBOX_POS + GetRootClient();
	CsPoint ptSize = IF_MASTERSMATCHING_EDITBOX_SIZE;
	MoveWindow( m_hEditCnt, ptPos.x, ptPos.y, ptSize.x, ptSize.y, TRUE );
}
void cMastersMatching::SetTarget( CsC_AvObject* pTarget )
{
	assert_cs( pTarget != NULL );
	//타겟 셋팅
	m_pTarget = pTarget;
	
	//NPC테이블에서 기부아이템 가져오기 위해 정보 얻어옴
	CNpc* pNpc = (CNpc*)pTarget;
	m_pFTInsertItem = (sNPC_TYPE_MASTER_MATCHING*)pNpc->GetFTNpc()->GetType();
}
