/*	배틀 준비 상태 체크 윈도우	*/
#include "stdafx.h"
#include "BattleWaitingWindow.h"

CBattleWaitWindow::sTeamUI::sTeamUI():m_pDigimonImg(NULL),m_pImgCover(NULL),m_pReadyStateImg(NULL),m_nReadyType(eNone)
{
}

CBattleWaitWindow::sTeamUI::~sTeamUI()
{
	m_pDigimonImg = 0;
	m_pImgCover = 0;
	m_pReadyStateImg = 0;
}

void CBattleWaitWindow::sTeamUI::SetReadyType( sTeamUI::eReadyState eType )
{
	m_nReadyType = eType;
	SAFE_POINTER_RET( m_pReadyStateImg );

	bool bVisible = true;
	std::string cImgFile;
	switch(m_nReadyType)
	{
	case eReject:	cImgFile = "Battle\\Wait\\battle_cancle.tga";	break;
	case eAccept:	cImgFile = "Battle\\Wait\\battle_accept.tga";	break;
	case eNone:		break;
	}

	if( cImgFile.empty() )
		m_pReadyStateImg->SetVisible(false);
	else	
	{
		m_pReadyStateImg->ChangeTexture( cImgFile.c_str() );
		m_pReadyStateImg->SetVisible(true);
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CBattleWaitWindow::CBattleWaitWindow():m_pBtnClose(NULL),m_pBtnJoinBattle(NULL),m_pBtnCancelBattle(NULL)
,m_fCloseDelayTime(0.0f),m_pProgressBar(NULL),m_pTextBattleMsg(NULL),m_pTextBattleMsg_Ask(NULL)
,m_pRedTeamList(NULL),m_pBlueTeamList(NULL),m_pVersusType(NULL)
{

}

CBattleWaitWindow::~CBattleWaitWindow()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE	CBattleWaitWindow::GetWindowType()
{
	return WT_BATTLE_MATCHING_WAIT_WINDOW; 
}


void CBattleWaitWindow::Destroy()
{
	m_PlayerUIContiner.clear();
	cBaseWindow::Delete();		
}

void CBattleWaitWindow::DeleteResource()
{	
	DeleteScript();	
}

void CBattleWaitWindow::Create(int nValue /* = 0  */)
{
	if( !cBaseWindow::Init() )
		return;

	InitScript( "Battle\\Wait\\battle_Wait_Window.tga", CsPoint( (1024 /2) - (528 / 2), 100 ), CsPoint( 528, 284 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::TOP );

	std::wstring wcTitle;	
	switch(GetSystem()->GetRegistedBattleType())
	{
	case eDuelCategory::TEAM:		wcTitle = UISTRING_TEXT("DIGIMON_BATTLE_REGIST_TYPE_TEAM");	break;// 팀 배틀
	case eDuelCategory::DEFENSE:	wcTitle = UISTRING_TEXT("DIGIMON_BATTLE_REGIST_TYPE_DEFANCE");	break;// 방어전 ( 미구현 )
	}
	AddTitle( wcTitle.c_str(), CsPoint(0, 8 ), CFont::FS_14, NiColor( 1.0f, 1.0f, 0.0f ) );

	m_pVersusType = NiNew cImage;
	if( m_pVersusType )
	{
		m_pVersusType->Init( GetRoot(), CsPoint((528/2) - (196 / 2), 118), CsPoint(196, 93), "Battle\\BattleModeSelect\\battle_versus.tga", false );
		m_pVersusType->SetTexToken( CsPoint( 0, 93 ) );
		AddChildControl( m_pVersusType );

		m_pVersusType->SetState(GetSystem()->GetRegistedBattleMode() - 1);
// 		switch(GetSystem()->GetRegistedBattleMode())
// 		{
// 		case eDuelSubCategory::SINGLE_DUEL:		m_pVersusType->SetState(cImage::NORMAL0);	break;
// 		case eDuelSubCategory::DOUBLE_DUEL:		m_pVersusType->SetState(cImage::NORMAL1);	break;
// 		case eDuelSubCategory::TRIPLE_DUEL:		m_pVersusType->SetState(cImage::NORMAL2);	break;
// 		case eDuelSubCategory::QUADRUPLE_DUEL:	m_pVersusType->SetState(cImage::NORMAL3);	break;
// 		case eDuelSubCategory::OCTUPLE_DUEL:	m_pVersusType->SetState(cImage::NORMAL4);	break;
// 		case eDuelSubCategory::SEXDECUPLE_DUEL:	m_pVersusType->SetState(cImage::NORMAL5);	break;
// 		}
	}

	m_pBtnClose = AddButton( CsPoint( 482, 12 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pBtnClose )
		m_pBtnClose->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleWaitWindow::_CloseWindow );

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, FONT_WHITE);
		ti.s_bOutLine = false;
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( L" " );
		m_pTextBattleMsg = AddText( &ti, CsPoint( 528 / 2, 55 ) );		
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, FONT_WHITE);
		ti.s_bOutLine = false;
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( L" " );
		m_pTextBattleMsg_Ask = AddText( &ti, CsPoint( 528 / 2, 75 ) );		
	}

	m_pBtnJoinBattle = AddButton( CsPoint( 137, 225 ), CsPoint( 128, 48 ), CsPoint(0, 48), "Event\\event_btn.tga" );
	if( m_pBtnJoinBattle )
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, FONT_WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DIGIMON_BATTLE_ASK_ACCEPT" ).c_str() );
		m_pBtnJoinBattle->SetText( &ti );
		m_pBtnJoinBattle->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleWaitWindow::_BattleAccept );
		m_pBtnJoinBattle->SetEnable(false);
	}

	m_pBtnCancelBattle = AddButton( CsPoint( 264, 225 ), CsPoint( 128, 48 ), CsPoint(0, 48), "Event\\event_btn.tga" );
	if( m_pBtnCancelBattle )
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, FONT_WHITE);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DIGIMON_BATTLE_ASK_REJECT" ).c_str() );		
		m_pBtnCancelBattle->SetText( &ti );
		m_pBtnCancelBattle->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleWaitWindow::_BattleCancel );
	}

	m_pProgressBar = NiNew cUI_TimerProgressBar;
	if( m_pProgressBar )
	{
		m_pProgressBar->Init( m_pRoot, CsPoint( 264 - 100, 200 ), CsPoint( 200, 10 ), false );		
		m_pProgressBar->SetBackImage( "Battle\\Wait\\common_progressbar_bg_02.tga" );
		m_pProgressBar->SetGuageImage( "Battle\\Wait\\common_progressbar_02.tga" );
		m_pProgressBar->SetTimer(10.0f, 10.f);
		m_pProgressBar->SetViewTextType( cUI_ProgressBar::NONE );
		m_pProgressBar->SetDirType( cUI_ProgressBar::RIGHT_LEFT );	
		m_pProgressBar->AddEvent(cUI_ProgressBar::eProgressbar_Done, this, &CBattleWaitWindow::_BattleCancel);
		m_pProgressBar->SetVisible(false);
		AddChildControl( m_pProgressBar );
	}

	m_pBlueTeamList = NiNew cGridListBox;
	if( m_pBlueTeamList )
	{
		m_pBlueTeamList->Init( GetRoot(), CsPoint( (m_ptRootSize.x / 2 - 35 - 200) , 126), CsPoint( 200, 51 ), CsPoint(5,0), CsPoint(44, 51), 
			cGridListBox::LowLeftUp, cGridListBox::RightTop, NULL, false, 4 );
		m_pBlueTeamList->SetVisible(false);
		AddChildControl(m_pBlueTeamList);	
	}

	m_pRedTeamList = NiNew cGridListBox;
	if( m_pRedTeamList )
	{
		m_pRedTeamList->Init( GetRoot(), CsPoint( m_ptRootSize.x / 2 + 35, 126 ), CsPoint(200, 51), CsPoint(5,0), CsPoint(44, 51), 
			cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 4 );
		m_pRedTeamList->SetVisible(false);
		AddChildControl(m_pRedTeamList);
	}

	_MakeAskMsg(eWait);
	_DataUpdate();
}

void CBattleWaitWindow::_DataUpdate()
{
	int nTeamPlayerCount = 0;
	switch( GetSystem()->GetRegistedBattleMode() )
	{
	case eDuelSubCategory::SINGLE_DUEL:		nTeamPlayerCount = eDuelSubCategoryMaxTeamCNT::SINGLE;	break;
	case eDuelSubCategory::DOUBLE_DUEL:		nTeamPlayerCount = eDuelSubCategoryMaxTeamCNT::DOUBLE;	break;
	case eDuelSubCategory::TRIPLE_DUEL:		nTeamPlayerCount = eDuelSubCategoryMaxTeamCNT::TRIPLE;	break;
	case eDuelSubCategory::QUADRUPLE_DUEL:	nTeamPlayerCount = eDuelSubCategoryMaxTeamCNT::QUADRUPLE;	break;
	case eDuelSubCategory::OCTUPLE_DUEL:	nTeamPlayerCount = eDuelSubCategoryMaxTeamCNT::OCTUPLE;	break;
	case eDuelSubCategory::SEXDECUPLE_DUEL:	nTeamPlayerCount = eDuelSubCategoryMaxTeamCNT::SEXDECUPLE;	break;
	}
	_MakeEmptySlot(0, m_pBlueTeamList, nTeamPlayerCount);
	_MakeEmptySlot(1, m_pRedTeamList, nTeamPlayerCount);
}

void CBattleWaitWindow::_MakeEmptySlot(int nTeamCode, cGridListBox * pControl, int count)
{
	SAFE_POINTER_RET( pControl );

	for( int n = 0 ; n < count; ++n )
	{
		cString* pString = NiNew cString;
		SAFE_POINTER_RET(pString);

		cSprite* pDigimonImg = NiNew cSprite;
		if( pDigimonImg )
		{
			pDigimonImg->Init( NULL, CsPoint::ZERO, CsPoint( 44,44 ), " ", false, NiColor::WHITE, false );		
			cString::sSPRITE* sSprite = pString->AddSprite( pDigimonImg );
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);
		}

		cSprite * pImgCover = NiNew cSprite;
		if( pImgCover )
		{
			if( nTeamCode == 0 )
				pImgCover->Init( NULL, CsPoint::ZERO, CsPoint( 44,44 ), " ", false );
			else
				pImgCover->Init( NULL, CsPoint::ZERO, CsPoint( 44,44 ), " ", false );

			cString::sSPRITE* sSprite = pString->AddSprite( pImgCover );
			if( sSprite )
				sSprite->SetAutoPointerDelete(true);
		}

		cGridListBoxItem* addItem  = NiNew cGridListBoxItem(n, CsPoint( 44, 51 ));
		addItem->SetItem(pString);
		pControl->AddItem( addItem );
	}
}

void CBattleWaitWindow::PreResetMap()
{
	Close( false );
}

void CBattleWaitWindow::_CloseWindow(void*pSender, void* pData)
{
	Close( false );
}

bool CBattleWaitWindow::OnEscapeKey()
{
	if( m_pBtnClose && m_pBtnClose->IsEnable() )
		Close( false );
	return true;
}

void CBattleWaitWindow::Update(float const& fDeltaTime)
{	
	UpdateScript(fDeltaTime);

	if( m_fCloseDelayTime > 0.0f )
	{
		m_fCloseDelayTime -= fDeltaTime;
		if( m_fCloseDelayTime <= 0.0f )
		{
			m_fCloseDelayTime = 0.0f;
			Close( false );
		}
	}
}

cBaseWindow::eMU_TYPE CBattleWaitWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pBtnClose && ( m_pBtnClose->Update_ForMouse() == cButton::ACTION_CLICK ) )
		return cBaseWindow::MUT_NONE;

	if(m_pBtnJoinBattle && m_pBtnJoinBattle->Update_ForMouse() == cButton::ACTION_CLICK )
		return cBaseWindow::MUT_NONE;

	if(m_pBtnCancelBattle && m_pBtnCancelBattle->Update_ForMouse() == cButton::ACTION_CLICK )
		return cBaseWindow::MUT_NONE;	

	return muReturn;
}

void CBattleWaitWindow::Render()
{
	RenderScript();				
}

void CBattleWaitWindow::ResetDevice()
{
	ResetDeviceScript();
}

void CBattleWaitWindow::_BattleAccept( void* pSender, void* pData )
{
	_StopTimer();

	if( GetSystem()->Send_Regist_Ready() )
	{
		if( m_pBtnJoinBattle )
			m_pBtnJoinBattle->SetEnable(false);
		if( m_pBtnCancelBattle )
			m_pBtnCancelBattle->SetEnable(false);
	}
}

void CBattleWaitWindow::_BattleCancel(void* pSender, void* pData)
{
	_StopTimer();
	if( GetSystem()->Send_Regist_Cancel() )
	{
		if( m_pBtnJoinBattle )
			m_pBtnJoinBattle->SetEnable(false);
		if( m_pBtnCancelBattle )
			m_pBtnCancelBattle->SetEnable(false);
	}
}

void CBattleWaitWindow::_CloseWindow(float const& fDelayTime)
{
	m_fCloseDelayTime = fDelayTime;
}

CBattleWaitWindow::sTeamUI * CBattleWaitWindow::_GetTeamUI( DWORD const& playerUIDX )
{
	std::map<DWORD/*Player UIDX*/,sTeamUI>::iterator it = m_PlayerUIContiner.find(playerUIDX);
	if( it != m_PlayerUIContiner.end() )
		return &it->second;

	return NULL;
}

void CBattleWaitWindow::_SetRejectPlayer(DWORD const& playerUIDX)
{
	sTeamUI* pUi	= _GetTeamUI( playerUIDX );
	if( pUi )
		pUi->SetReadyType( sTeamUI::eReject );

//	_CloseWindow(2.0f);
}

void CBattleWaitWindow::_SetAcceptPlayer(DWORD const& playerUIDX)
{
	sTeamUI* pUi	= _GetTeamUI( playerUIDX );
	if( pUi )
		pUi->SetReadyType( sTeamUI::eAccept );

// 	if( _IsAllReady() )
// 		_CloseWindow(0.5f);
}

void CBattleWaitWindow::_StopTimer()
{
	if( m_pProgressBar )
	{
		m_pProgressBar->Stop();
		m_pProgressBar->SetVisible(false);
	}
}

bool CBattleWaitWindow::_IsAllReady()
{
	std::map<DWORD,sTeamUI>::iterator it = m_PlayerUIContiner.begin();
	if( it != m_PlayerUIContiner.end() )
	{
		if( it->second.m_nReadyType != sTeamUI::eAccept )
			return false;
	}
	return true;
}

void CBattleWaitWindow::_MakeAskMsg(eWaitType const& nBattleType)
{
	std::wstring msgTitle;
	std::wstring msg;
	switch( nBattleType )
	{
	case eWait:			// 배틀 신청 대기 상태
		{
			msgTitle = UISTRING_TEXT("DIGIMON_BATTLE_REGIST_WAIT_TITLE");
			msg = UISTRING_TEXT("DIGIMON_BATTLE_REGIST_WAIT_MSG");
		}
		break;
	case eAsk:			// 배틀 매칭 후 수락 요청 상태
		{
			msgTitle = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_ASK_TITLE");
			msg = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_ASK_MSG");
		}
		break;
	case eAccpt_My:		// 내가 수락을 했을 때
		{
			msgTitle = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_MY_ACCPT_TITLE");
			msg = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_MY_ACCPT_MSG");
		}
		break;
	case eReject_My:	// 내가 취소를 했을 경우
		{
			msgTitle = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_MY_CANCEL_TITLE");
			msg = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_MY_CANCEL_MSG");
		}
		break;
	case eAccpt_Other:	// 다른 플레이어가 수락 했을 경우
		{
			msgTitle = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_OTHER_ACCPT_TITLE");
			msg = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_OTHER_ACCPT_MSG");
		}
		break;
	case eReject_Other: // 다른 플레이어가 취소 했을 경우
		{
			msgTitle = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_OTHER_CANCEL_TITLE");
			msg = UISTRING_TEXT("DIGIMON_BATTLE_MATCHING_OTHER_CANCEL_MSG");
		}
		break;
	}
	if( m_pTextBattleMsg )
		m_pTextBattleMsg->SetText( msgTitle.c_str() );

	if( m_pTextBattleMsg_Ask)
		m_pTextBattleMsg_Ask->SetText( msg.c_str() );	
}

//////////////////////////////////////////////////////////////////////////
// 배틀 준비 요청 상태
//////////////////////////////////////////////////////////////////////////
void CBattleWaitWindow::_BattleReadRequest()
{
	if( m_pBlueTeamList )
		m_pBlueTeamList->RemoveAllItem();
	if( m_pRedTeamList )
		m_pRedTeamList->RemoveAllItem();

	BWait::sWait::WAIT_PLAYER_LIST_CON const& pList = GetSystem()->GetWaitPlayerInfo();
	BWait::sWait::WAIT_PLAYER_LIST_CON_CIT it = pList.begin();
	for( ; it != pList.end(); ++it )
	{
		cGridListBox* pControl = NULL;
		switch( it->first )
		{
		case eDuelTeamType::RED:	pControl = m_pBlueTeamList;	break;// Blue Team
		case eDuelTeamType::BLUE:	pControl = m_pRedTeamList;	break;	// Red Team
		default:
			continue;
		}

		BWait::sWait::PLAYER_INFO_CIT subIT = it->second.begin();
		for( int n = 0; subIT != it->second.end(); ++subIT, ++n )
			_SetPlayerUI(it->first, pControl, (*subIT), n );
	}
}

void CBattleWaitWindow::_SetPlayerUI( int const& nTeamCode, cGridListBox* pControl, BWait::sWait::sPlayerInfo const& playerInfo, int const& nCount)
{
	SAFE_POINTER_RET( pControl );

	cString* pString = NiNew cString;
	SAFE_POINTER_RET(pString);

	sTeamUI addUI;
	addUI.m_pDigimonImg = NiNew cSprite;
	if( addUI.m_pDigimonImg )
	{
		addUI.m_pDigimonImg->Init( NULL, CsPoint::ZERO, CsPoint( 44,44 ), playerInfo.m_DigimonIconFileName.c_str(), false, NiColor::WHITE, false );		
		cString::sSPRITE* sSprite = pString->AddSprite( addUI.m_pDigimonImg );
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);
	}

	addUI.m_pImgCover = NiNew cSprite;
	if( addUI.m_pImgCover )
	{
		if( nTeamCode == eDuelTeamType::BLUE )
			addUI.m_pImgCover->Init( NULL, CsPoint::ZERO, CsPoint( 44,44 ), "Battle\\Wait\\\\battle_team_Blue.tga", false );		
		else
			addUI.m_pImgCover->Init( NULL, CsPoint::ZERO, CsPoint( 44,44 ), "Battle\\Wait\\\\battle_team_Red.tga", false );		
		cString::sSPRITE* sSprite = pString->AddSprite( addUI.m_pImgCover );
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);
	}

	addUI.m_pReadyStateImg = NiNew cSprite;
	if( addUI.m_pReadyStateImg )
	{
		addUI.m_pReadyStateImg->Init( NULL, CsPoint(1, 45), CsPoint( 42,6 ), "Battle\\Wait\\battle_accept.tga", false );		
		cString::sSPRITE* sSprite = pString->AddSprite( addUI.m_pReadyStateImg );
		if( sSprite )
			sSprite->SetAutoPointerDelete(true);
	}

	cGridListBoxItem* addItem  = NiNew cGridListBoxItem(nCount, CsPoint( 44, 51 ));
	addItem->SetItem(pString);
	pControl->AddItem( addItem );

	m_PlayerUIContiner.insert( std::make_pair( playerInfo.m_uTamerUIDX, addUI ) );
}

void CBattleWaitWindow::_CloseWaitWindow( float const& fDelay )
{
	if( m_pBtnJoinBattle )
		m_pBtnJoinBattle->SetEnable(false);
	if( m_pBtnClose )
		m_pBtnClose->SetEnable(false);
	if( m_pBtnCancelBattle )
		m_pBtnCancelBattle->SetEnable(false);
	_CloseWindow(fDelay);
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CBattleWaitWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eAsk_BattleRequest, this );
	GetSystem()->Register( SystemType::eMy_Ready_Change, this );	
	GetSystem()->Register( SystemType::eOther_Ready_Change, this );
	GetSystem()->Register( SystemType::eBattleMatchCancel, this );
	return true;
}

void CBattleWaitWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{		
	case SystemType::eAsk_BattleRequest:	// 배틀 매칭 플레이어 들의 정보를 받아 각 플레이어 들의 준비 상태 체크
		{
			_MakeAskMsg(eAsk);
			_BattleReadRequest();
			if( m_pProgressBar )
			{
				m_pProgressBar->SetVisible(true);
				m_pProgressBar->Start();
			}
			if( m_pBtnJoinBattle )
				m_pBtnJoinBattle->SetEnable(true);
			if( m_pBtnClose )
				m_pBtnClose->SetEnable(false);
		}break;
	case SystemType::eMy_Ready_Change:	// 나를 포함한 다른 플레이어가 수락했을 때
		{
			DWORD acceptTamerUIDX;
			char cReady;
			kStream >> acceptTamerUIDX >> cReady;
			if( cReady == 0 ) // 취소
			{
				_MakeAskMsg( eReject_My );
				_SetRejectPlayer(acceptTamerUIDX);
			}
			else if( cReady == 1 ) // 승락
			{
				_MakeAskMsg( eAccpt_My );
				_SetAcceptPlayer(acceptTamerUIDX);
			}		

			_StopTimer();
		}	
		break;
	case SystemType::eOther_Ready_Change:	// 나를 포함한 다른 플레이어가 수락했을 때
		{
			DWORD acceptTamerUIDX;
			char cReady;
			kStream >> acceptTamerUIDX >> cReady;
			if( cReady == 0 ) // 취소
			{
				_MakeAskMsg( eReject_Other );
				_SetRejectPlayer(acceptTamerUIDX);
			}
			else if( cReady == 1 ) // 승락
			{
				//_MakeAskMsg(eAccpt_Other);
				_SetAcceptPlayer(acceptTamerUIDX);
			}
		}	
		break;
	case SystemType::eBattleMatchCancel:
		{
			_StopTimer();
			_CloseWaitWindow(2.0f);
		}
		break;
	}
}

