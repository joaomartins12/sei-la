
#include "stdafx.h"
#include "_GameIF.h"
#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

cGameInterface*		g_pGameIF = NULL;

cGameInterface::cGameInterface() :	m_pStaticFocusWindow(NULL), m_bRenderIF(true), 
									m_pBright(NULL), /*m_pTooltip(NULL),*/ m_pAlime(NULL), /*m_pCompareTooltip(NULL), m_pRightTooltip(NULL), */m_pPopupWindow(NULL), m_pBossScene(NULL)						

{
	m_pGlobalMsg = NULL;
	m_vpReserveFocusWindow.Destroy();
	m_vpReserveFocusCallFunc.Destroy();

	EnterKeyCreate();
}

cGameInterface::~cGameInterface()
{
	Destroy();
}

void cGameInterface::GlobalInit()
{
	assert_cs( g_pGameIF == NULL );

	g_pGameIF = NiNew cGameInterface;
	g_pGameIF->Init();

	cMoneyIF::GlobalInit();
	cRegionInfo::GlobalInit();
	cAchieveInfo::GlobalInit();
	cMiniMap::GlobalInit();
	cTalkBalloon::GlobalInit();
	cHpBar::GlobalInit();
	cTip::GlobalInit();

}

void cGameInterface::GlobalShotDown()
{
	SAFE_NIDELETE( g_pGameIF );

	//cTip::GlobalShotDown();
	//cHpBar::GlobalShotDown();
	//cTalkBalloon::GlobalShotDown();
	//cMiniMap::GlobalShotDown();
	//cMoneyIF::GlobalShotDown();
	//cRegionInfo::GlobalShotDown();
	//cAchieveInfo::GlobalShotDown();

	cMessageBox::DeleteList( false );
	cSeparate::DeleteInstance();
	cMoneySeparate::DeleteInstance();
	cCreateName::DeleteInstance();
	cString::GlobalShotDown();
	cStringAnalysis::GlobalShotDown();
	cWindow::GlobalShotDown();
}

void cGameInterface::Destroy()
{
	SAFE_NIDELETE( m_pBright );
	SAFE_NIDELETE( m_pAlime );
	SAFE_NIDELETE( m_pPopupWindow );
	SAFE_NIDELETE( m_pBossScene );
	SAFE_NIDELETE( m_pGlobalMsg );

	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
		_PointerDetach( m_vpStaticUI[ i ] );
	m_vpStaticUI.Destroy();

	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
		_PointerDetach( m_vpDynamicUI[ i ] );
	m_vpDynamicUI.Destroy();

	
	// m_vpStaticUI, m_vpDynamicUI에서  미리 지워 졌다. 따라서 따로 지워 줄 필요 없다.
	m_vpDelete.Destroy();

	m_pStaticFocusWindow = NULL;

	m_vpReserveFocusWindow.Destroy();
	m_vpReserveFocusCallFunc.Destroy();
	m_mapWindowArray.clear();

	//
	cMiniMap::GlobalShotDown();
	cMoneyIF::GlobalShotDown();
	cRegionInfo::GlobalShotDown();
	cAchieveInfo::GlobalShotDown();
	cTip::GlobalShotDown();
	cHpBar::GlobalShotDown();
	cTalkBalloon::GlobalShotDown();

// #ifdef VERSION_STEAM
// 	nsCsFileTable::g_pCashShopMng_Steam->Delete_Element();
// #else
// 	nsCsFileTable::g_pCashShopMng->Delete_Element();
// #endif
}

void cGameInterface::Init()
{
	m_pBright = NiNew cSprite;		// 화면 밝기
	m_pBright->Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), true, NiColor::BLACK);
	SetBright();

	m_pAlime = NiNew cAlime;		//알리미
	m_pAlime->Init();

	m_pPopupWindow = NiNew cPopUpWindow;	// 팝업
	m_pPopupWindow->Init();	

	m_pBossScene = NiNew cBossScene;		// 보스 출현 연출
	m_pBossScene->Init();

	m_pGlobalMsg = NiNew cGlobalMessage;	// 글로벌 메세지

	CreateStaticWindow(cBaseWindow::WT_MAIN_BAR );						// MAIN BAR (SHORTCUT ICONS)

	CreateStaticWindow(cBaseWindow::WT_NOTICE_BUTTON);					// NOTICE BUTTON
	CreateStaticWindow(cBaseWindow::WT_VIP_BUTTON);						// MEMBERSHIP BUTTON
	CreateStaticWindow(cBaseWindow::WT_SEASONPASS_BUTTON);				// SEASON PASS BUTTON
	CreateStaticWindow(cBaseWindow::WT_CASHSHOP_BUTTON);				// CASH SHOP BUTTON

#ifdef PLAY_PANELTY
	CreateStaticWindow( cBaseWindow::WT_PLAYTIME );	// 플레이타임
#endif

	CreateStaticWindow( cBaseWindow::WT_TAMER_WINDOW );					// TAMER WINDOW UI
	CreateStaticWindow( cBaseWindow::WT_MAIN_DIGIMON_WINDOW );			// DIGIMON WINDOW UI

	CreateStaticWindow( cBaseWindow::WT_TARGET_WINDOW, 0, 0, false, false );	// TARGET WINDOW UI

	// Digimon Slot UI
	for (int i = 0; i < (nLimit::DigimonBaseSlot - 1); i++)
		CreateStaticWindow( cBaseWindow::WT_TACTICS_WINDOW, i , i);		// DIGIMON SLOT UI

	CreateDynamicWindow(cBaseWindow::WT_CHAT_MAIN_WINDOW);				// CHAT MAIN WINDOW UI
	CreateDynamicWindow(cBaseWindow::WT_CHAT_SYSTEM_WINDOW);			// CHAT SYSTEM WINDOW UI

	// Skill Quick Slot
	for( int i = 0 ; i < IF_MAX_QUICKSLOT_LINE_COUNT ; ++i )
		CreateStaticWindow( cBaseWindow::WT_QUICKSLOT, i, i);			// SKILL QUICK SLOT UI

#ifdef USE_CENTER_BAR	// 센터바
	for( int i=0; i<2; ++i )
		CreateStaticWindow( cBaseWindow::WT_CENTERBAR, i, i);			// CENTER BAR UI
#endif	

	CreateStaticWindow( cBaseWindow::WT_QUICKEVOL, 0, 2);				// QUICK EVOLUTION	2 = WT_QUICKSLOT 갯수

	CreateStaticWindow( cBaseWindow::WT_QUEST_HELPER );					// QUEST HELPER UI

	CreateStaticWindow( cBaseWindow::WT_MINIMAP );						// MINIMAP UI

#ifdef PARTY_BUFF_SHOW
	for( int i=0; i<IF_BUFF_PARTY4_T; ++i )
#else
	for( int i=0; i<4; ++i )
#endif
		CreateStaticWindow( cBaseWindow::WT_BUFFINFO, i, i);
	
	ChangeMacroKey();

	m_sMyInfo.s_eMyTeam = NONE;		//본인 팀 정보
	m_sMyInfo.s_nNextResetTime = 0;	//다음 리셋 시간
	m_sEventInfo.Init();

	char cPath[ MAX_PATH ];
	nsCsFileTable::g_FileTableMng.GetLanguagePath( nsCsFileTable::g_eLanguage, cPath );
}

void cGameInterface::PreResetMap()
{
	g_pTalkBallone->PreResetMap();
	g_pHpBar->PreResetMap();
	g_pTip->PreResetMap();
	m_pPopupWindow->PreResetMap();
	m_pBossScene->PreResetMap();
	m_pAlime->ResetMap();

	g_pRegionInfo->Delete();

	cTalkBalloon::sBALLOONE::GlobalShotDown();

	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == true )
			m_vpStaticUI[ i ]->PreResetMap();
	}
	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == true )
			m_vpDynamicUI[ i ]->PreResetMap();
	}
}

void cGameInterface::ResetMap()
{
	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == true )
			m_vpStaticUI[ i ]->ResetMap();
	}
	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == true )
			m_vpDynamicUI[ i ]->ResetMap();
	}
}

void cGameInterface::UserDie()
{
	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == true )
			m_vpStaticUI[ i ]->UserDie();
	}
	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == true )
			m_vpDynamicUI[ i ]->UserDie();
	}	
}

void cGameInterface::RecvDiedAlready()
{
	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == true )
			m_vpStaticUI[ i ]->RecvDiedAlready();
	}
	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == true )
			m_vpDynamicUI[ i ]->RecvDiedAlready();
	}
}

void cGameInterface::DeleteWindowUpdate()
{
	cBaseWindow* pWindow = NULL;
	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		pWindow = m_vpStaticUI[ i ];
		SAFE_POINTER_CON( pWindow );

		if( !pWindow->IsShowWindow() )
			continue;

		if( !pWindow->IsLive() )
			m_vpDelete.PushBack( pWindow );
	}

	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		pWindow = m_vpDynamicUI[ i ];
		SAFE_POINTER_CON( pWindow );

		if( !pWindow->IsLive() )
			m_vpDelete.PushBack( pWindow );
	}

	_UpdateReserveFocusWindow();

	nCount = m_vpDelete.Size();
	for( int i=0; i<nCount; ++i )
	{		
		SAFE_POINTER_CON( m_vpDelete[ i ] );
		if( m_vpDelete[ i ]->IsStaticWindow() == false )
			m_vpDynamicUI.DeleteElement_FromData( m_vpDelete[ i ] );
		else
			m_vpStaticUI.DeleteElement_FromData( m_vpDelete[ i ] );

		_PointerDetach( m_vpDelete[ i ] );
	}
	m_vpDelete.ClearElement();
}


//retun값
//true 캐릭터이동불가 false 캐릭이동
cGlobalInput::eMOUSE_INPUT cGameInterface::Update(float const& fDeltaTime)
{
	cGlobalInput::eMOUSE_INPUT resInput = cGlobalInput::MOUSEINPUT_ENABLE;

	// 파트너몬 변경시에 닫아야 하는 인터페이스 설정
	if( g_pDataMng->GetServerSync()->IsChageDigimon() )
	{
		if( IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE ) == true )
		{
			cMessageBox::DelMsg( 30050, false );
			int NoUseParameter = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_HIDE_VIEWER, &NoUseParameter);
		}

		if( IsActiveWindow( cBaseWindow::WT_SPIRIT_EVOLUTION ) )
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_EXTRA_EVOLUTION_STORE );

		//if( GetTacticsHouse() != NULL )
		//{
		//	cMessageBox::DelMsg( 30050, false );
		//	GetTacticsHouse()->Close( false );
		//}
	}

	// 전투중에 닫히는 인터페이스
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );
	if( pDUser && pDUser->IsBattle() == true )
	{
		if( GetJumpBuster() != NULL )
		{
			GetJumpBuster()->Close( false );
		}
	}

	if( g_pTip )
		g_pTip->Update(fDeltaTime);

	if( g_pTalkBallone )
		g_pTalkBallone->Update(fDeltaTime);
	if( g_pAchieveInfo )
		g_pAchieveInfo->Update(fDeltaTime);
	if( g_pRegionInfo )
		g_pRegionInfo->Update(fDeltaTime);	

	cBaseWindow* pWindow = NULL;
	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		pWindow = m_vpStaticUI[ i ];
		SAFE_POINTER_CON( pWindow );

		if( pWindow->IsShowWindow() == false )
			continue;

		if( pWindow->IsShowWindow() )
			pWindow->Update(fDeltaTime);

		if( pWindow->IsLive() == false )
		{
			m_vpDelete.PushBack( pWindow );
		}
	}

	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		pWindow = m_vpDynamicUI[ i ];
		SAFE_POINTER_CON( pWindow );

		if( pWindow->IsShowWindow() )
			pWindow->Update(fDeltaTime);

		if( pWindow->IsLive() == false )
		{
			m_vpDelete.PushBack( pWindow );
		}
	}

	_UpdateReserveFocusWindow();

	nCount = m_vpDelete.Size();
	for( int i=0; i<nCount; ++i )
	{		
		SAFE_POINTER_CON( m_vpDelete[ i ] );
		if( m_vpDelete[ i ]->IsStaticWindow() == false )
			m_vpDynamicUI.DeleteElement_FromData( m_vpDelete[ i ] );
		else
			m_vpStaticUI.DeleteElement_FromData( m_vpDelete[ i ] );

		_PointerDetach( m_vpDelete[ i ] );
	}
	m_vpDelete.ClearElement();

	if( m_pBossScene )
		m_pBossScene->Update(fDeltaTime);
	if( m_pGlobalMsg )
		m_pGlobalMsg->Update(fDeltaTime);

	if( cMessageBox::UpdateList() == true )
	{
		return cGlobalInput::MOUSEINPUT_DISABLE;
	}

	// 이벤트중
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_EVENT )
		return cGlobalInput::MOUSEINPUT_DISABLE;

	// 디지몬 부화 애니중
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_TACTICSANI )
		return cGlobalInput::MOUSEINPUT_DISABLE;
	
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_NONE )
	{	
		if( m_bRenderIF == false )
			return cGlobalInput::MOUSEINPUT_ENABLE;
	}

	// 닷트 본부
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
	{
		if( g_pDatsCenter->GetState() > CDatsCenter::eSTATE_ENCHENT )
			return cGlobalInput::MOUSEINPUT_DISABLE;
	}
	
	if( GLOBALINPUT_ST.IsCameraRotation() == true )
		return cGlobalInput::MOUSEINPUT_CAMERA;

	cBaseWindow* pPersonStore = _GetPointer( cBaseWindow::WT_PERSONSTORE, 0 );
	cBaseWindow* pGotchaMachine = _GetPointer( cBaseWindow::WT_NEW_GOTCHA_MACHINE, 0 );

#ifdef MINIGAME
	if(GetMiniGame())
	{
		cGlobalInput::eMOUSE_INPUT eReturn = cGlobalInput::MOUSEINPUT_CAMERA;

		if( GetMiniGame()->IsEnableWindow() )
		{
			switch( GetMiniGame()->Update_ForMouse() )
			{		
			case cBaseWindow::MUT_OUT_WINDOW:
				break;
			case cBaseWindow::MUT_NONE:
				eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
				break;
			case cBaseWindow::MUT_CLICK_FOCUS:
				SetFocusWindow( GetMiniGame() );
				eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
				break;
			}
		}
		else
		{
			switch( GetMiniGame()->Update_ForMouse_Disable() )
			{		
			case cBaseWindow::MUT_OUT_WINDOW:
				break;
			case cBaseWindow::MUT_NONE:
				eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
				break;
			case cBaseWindow::MUT_CLICK_FOCUS:
				SetFocusWindow( GetMiniGame() );
				eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
				break;
			}
		}
		return eReturn;
	}
	else
#endif
	if( pGotchaMachine )
	{
		cGlobalInput::eMOUSE_INPUT eReturn = cGlobalInput::MOUSEINPUT_CAMERA;

		if( pGotchaMachine->IsEnableWindow() )
		{
			switch( pGotchaMachine->Update_ForMouse() )
			{		
			case cBaseWindow::MUT_OUT_WINDOW:
				break;
			case cBaseWindow::MUT_NONE:
				eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
				break;
			case cBaseWindow::MUT_CLICK_FOCUS:
				SetFocusWindow( pGotchaMachine );
				eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
				break;
			}
		}
		else
		{
			switch( pGotchaMachine->Update_ForMouse_Disable() )
			{		
			case cBaseWindow::MUT_OUT_WINDOW:
				break;
			case cBaseWindow::MUT_NONE:
				eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
				break;
			case cBaseWindow::MUT_CLICK_FOCUS:
				SetFocusWindow( pGotchaMachine );
				eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
				break;
			}
		}

		for( int i=0; i<g_pDataMng->GetInven()->GetInvenGroupCount(); ++i )
		{
			if( IsActiveWindow( cBaseWindow::WT_INVENTORY, i ) == true )
			{
				if( GetInventory( i )->IsEnableWindow() )
				{
					switch( GetInventory( i )->Update_ForMouse() )
					{		
					case cBaseWindow::MUT_OUT_WINDOW:
						break;
					case cBaseWindow::MUT_NONE:
						eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
						break;
					case cBaseWindow::MUT_CLICK_FOCUS:
						SetFocusWindow( GetInventory( i ) );
						eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
						break;
					}
				}
				else
				{
					switch( GetInventory( i )->Update_ForMouse_Disable() )
					{		
					case cBaseWindow::MUT_OUT_WINDOW:
						break;
					case cBaseWindow::MUT_NONE:
						eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
						break;
					case cBaseWindow::MUT_CLICK_FOCUS:
						SetFocusWindow( GetInventory( i ) );
						eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
						break;
					}
				}				
			}
		}

		if( GetChatOption() )
		{
			if( GetChatOption()->IsEnableWindow() )
			{
				switch( GetChatOption()->Update_ForMouse() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetChatOption() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
			else
			{
				switch( GetChatOption()->Update_ForMouse_Disable() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetChatOption() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
		}

		return eReturn;
	}
	else if( GetTrade() || pPersonStore || GetCardEvent() )	// 카드 이벤트 테이블 외 마우스 입력 안되게... 
	{
		cGlobalInput::eMOUSE_INPUT eReturn = cGlobalInput::MOUSEINPUT_CAMERA;

		if( GetChatOption() )
		{
			if( GetChatOption()->IsEnableWindow() )
			{
				switch( GetChatOption()->Update_ForMouse() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetChatOption() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
			else
			{
				switch( GetChatOption()->Update_ForMouse_Disable() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetChatOption() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
		}

		for( int i=0; i<g_pDataMng->GetInven()->GetInvenGroupCount(); ++i )
		{
			if( IsActiveWindow( cBaseWindow::WT_INVENTORY, i ) == true )
			{
				if( GetInventory( i )->IsEnableWindow() )
				{
					switch( GetInventory( i )->Update_ForMouse() )
					{		
					case cBaseWindow::MUT_OUT_WINDOW:
						break;
					case cBaseWindow::MUT_NONE:
						eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
						break;
					case cBaseWindow::MUT_CLICK_FOCUS:
						SetFocusWindow( GetInventory( i ) );
						eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
						break;
					}
				}
				else
				{
					switch( GetInventory( i )->Update_ForMouse_Disable() )
					{		
					case cBaseWindow::MUT_OUT_WINDOW:
						break;
					case cBaseWindow::MUT_NONE:
						eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
						break;
					case cBaseWindow::MUT_CLICK_FOCUS:
						SetFocusWindow( GetInventory( i ) );
						eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
						break;
					}
				}				
			}
		}
		if( GetTrade() )
		{
			if( GetTrade()->IsEnableWindow() )
			{
				switch( GetTrade()->Update_ForMouse() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetTrade() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
			else
			{
				switch( GetTrade()->Update_ForMouse_Disable() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetTrade() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
		}
		
		// 이벤트 창 마우스 클릭 불가
		if( pPersonStore )
		{
			if( pPersonStore->IsEnableWindow() )
			{
				switch( pPersonStore->Update_ForMouse() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( pPersonStore );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
			else
			{				
				switch( pPersonStore->Update_ForMouse_Disable() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( pPersonStore );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
		}

		if( GetMailButton() )
		{
			if( GetMailButton()->IsEnableWindow() )
			{
				switch( GetMailButton()->Update_ForMouse() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetMailButton() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
			else
			{
				switch( GetMailButton()->Update_ForMouse_Disable() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetMailButton() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}			
		}

		if( GetMail() )
		{
			if( GetMail()->IsEnableWindow() )
			{
				switch( GetMail()->Update_ForMouse() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetMail() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
			else
			{
				switch( GetMail()->Update_ForMouse_Disable() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetMail() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
		}

		if( GetCardEvent() )
		{
			if( GetCardEvent()->IsEnableWindow() )
			{
				switch( GetCardEvent()->Update_ForMouse() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetCardEvent() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
			else
			{
				switch( GetCardEvent()->Update_ForMouse_Disable() )
				{		
				case cBaseWindow::MUT_OUT_WINDOW:
					break;
				case cBaseWindow::MUT_NONE:
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				case cBaseWindow::MUT_CLICK_FOCUS:
					SetFocusWindow( GetCardEvent() );
					eReturn = cGlobalInput::MOUSEINPUT_DISABLE;
					break;
				}
			}
		}
		return eReturn;
	}

	if( m_pPopupWindow->Update_ForMouse() == true )
		return cGlobalInput::MOUSEINPUT_DISABLE;	

	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpDynamicUI[ i ]->IsEnableWindow() )
		{
			switch( m_vpDynamicUI[ i ]->Update_ForMouse() )
			{		
			case cBaseWindow::MUT_OUT_WINDOW:
				break;
			case cBaseWindow::MUT_NONE:
				return cGlobalInput::MOUSEINPUT_DISABLE;
			case cBaseWindow::MUT_CLICK_FOCUS:
				SetFocusWindow( m_vpDynamicUI[ i ] );
				return cGlobalInput::MOUSEINPUT_DISABLE;
			}
		}
		else
		{			
			switch( m_vpDynamicUI[ i ]->Update_ForMouse_Disable() )
			{
			case cBaseWindow::MUT_OUT_WINDOW:
				break;
			case cBaseWindow::MUT_NONE:
				return cGlobalInput::MOUSEINPUT_DISABLE;
			case cBaseWindow::MUT_CLICK_FOCUS:
				SetFocusWindow( m_vpDynamicUI[ i ] );
				return cGlobalInput::MOUSEINPUT_DISABLE;
			}
		}
	}
	
	nCount = m_vpStaticUI.Size();
	for( int i=nCount-1; i>-1; --i )
	//for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->IsEnableWindow() )
		{
			switch( m_vpStaticUI[ i ]->Update_ForMouse() )
			{		
			case cBaseWindow::MUT_OUT_WINDOW:
				break;
			case cBaseWindow::MUT_NONE:
				return cGlobalInput::MOUSEINPUT_DISABLE;
			case cBaseWindow::MUT_CLICK_FOCUS:
				SetFocusWindow( m_vpStaticUI[ i ] );
				return cGlobalInput::MOUSEINPUT_DISABLE;
			}
		}
		else
		{
			switch( m_vpStaticUI[ i ]->Update_ForMouse_Disable() )
			{		
			case cBaseWindow::MUT_OUT_WINDOW:
				break;
			case cBaseWindow::MUT_NONE:
				return cGlobalInput::MOUSEINPUT_DISABLE;
			case cBaseWindow::MUT_CLICK_FOCUS:
				SetFocusWindow( m_vpStaticUI[ i ] );
				return cGlobalInput::MOUSEINPUT_DISABLE;
			}
		}
	}	

	// 닷츠센타 내부
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
		return cGlobalInput::MOUSEINPUT_PICK;

	return resInput;
}

void cGameInterface::Render()
{
	//m_pWeather->Render();
	if( g_pResist->m_Global.s_bUseBright )
	{
		m_pBright->Render();		
		g_pEngine->ScreenSpace();
	}	

	//g_pEngine->ScreenSpace();

	if( m_bRenderIF == false )
	{
		switch( g_pMngCollector->GetSceneState() )
		{
		case CMngCollector::eSCENE_EVENT:
			g_pGameEventMng->RenderText();
			g_pEventScene->Render();
//			g_pSystemMsg->Render();
			break;
#ifdef CROSSWARS_SYSTEM
		case CMngCollector::eSCENE_CROSS:
#endif
		case CMngCollector::eSCENE_TACTICSANI:
		case CMngCollector::eSCENE_DATS:
//			g_pSystemMsg->Render();
			break;
		default:
			g_pTalkBallone->Render();	
//			g_pSystemMsg->Render();
			g_pSkillMsg->Render();
			break;
		}
//		cMessageBox::RenderList();

		return;
	}
	g_pHpBar->Render();
	g_pTalkBallone->Render();	

	cBaseWindow* pWindow = NULL;

	pWindow = GetQuestHelper();
	if( pWindow && pWindow->IsShowWindow() )
		pWindow->Render();

	pWindow = GetMiniMap();
	if( pWindow && pWindow->IsShowWindow() )
		pWindow->Render();

	g_pNpcMng->RenderZoomNpc();

	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		pWindow = m_vpStaticUI[ i ];

		if( pWindow->IsShowWindow() == false )
			continue;

		switch( pWindow->GetWindowType() )
		{
		case cBaseWindow::WT_QUEST_HELPER:
		case cBaseWindow::WT_MINIMAP:		
			continue;
		}
		pWindow->Render();
	}

// 	if( IsActiveWindow( cBaseWindow::WT_BATTLERESULT ) )
// 	{
// 		GetBattleResult()->Render();
// 		return;
// 	}

	m_pBossScene->RenderBossInfo(); //보스출현정보 아이콘 랜더링 위치는 항상여기

	nCount = m_vpDynamicUI.Size();
	for( int i=nCount-1; i>-1; --i )
	{
		pWindow = m_vpDynamicUI[ i ];

		if( pWindow->IsShowWindow() == false )
			continue;

		switch( pWindow->GetWindowType() )
		{
		case cBaseWindow::WT_WEBWIN:		
		case cBaseWindow::WT_PUBLICITY:
			continue;
		}
		
		pWindow->Render();		
	}

	pWindow = GetWebWin();
	if( pWindow && IsActiveWindow( cBaseWindow::WT_WEBWIN ) )
		pWindow->Render();

	m_pPopupWindow->Render();

	if( g_pAchieveInfo && g_pAchieveInfo->Render() == false )
		g_pRegionInfo->Render();

	if( m_pBossScene )
		m_pBossScene->Render();	

	if( m_pGlobalMsg )
		m_pGlobalMsg->Render();
	if( m_pAlime )
		m_pAlime->Render();
}

void cGameInterface::OnLButtonUp( CsPoint pos )
{
	if( cMessageBox::OnLButtonUpList( pos ) == true )
	{
		return;
	}

	if( m_bRenderIF == false )
		return;

	if( m_pStaticFocusWindow && m_pStaticFocusWindow->IsShowWindow() )
	{
		if( m_pStaticFocusWindow->IsEnableWindow() )
			m_pStaticFocusWindow->OnLButtonUp( pos );
	}
	else// if( m_vpDynamicUI.IsExistElement( 0 ) && m_vpDynamicUI[ 0 ]->IsShowWindow() )
	{
// 		if( m_vpDynamicUI[ 0 ]->IsEnableWindow() )
// 			m_vpDynamicUI[ 0 ]->OnLButtonUp( pos );
		int nCount = m_vpDynamicUI.Size();
		for( int i=0; i<nCount; ++i )
		{
			if( m_vpDynamicUI[ i ]->IsHaveFocus() && m_vpDynamicUI[ i ]->IsShowWindow() )
			{
				m_vpDynamicUI[ i ]->OnLButtonUp( pos );
				break;
			}
		}
	}

	/*int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == true )
		{
			m_vpStaticUI[ i ]->OnLButtonUp( pos );
		}
	}

	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == true )
			m_vpDynamicUI[ i ]->OnLButtonUp( pos );
	}*/
}

void cGameInterface::OnLButtonDown( CsPoint pos )
{
	if( cMessageBox::OnLButtonUpList( pos ) == true )
		return;

	if( m_bRenderIF == false )
		return;

	if( m_pStaticFocusWindow && m_pStaticFocusWindow->IsShowWindow() )
	{
		if( m_pStaticFocusWindow->IsEnableWindow() )
			m_pStaticFocusWindow->OnLButtonUp( pos );
	}
	else //if( m_vpDynamicUI.IsExistElement( 0 ) && m_vpDynamicUI[ 0 ]->IsShowWindow() )
	{
// 		if( m_vpDynamicUI[ 0 ]->IsEnableWindow() )
// 			m_vpDynamicUI[ 0 ]->OnLButtonDown( pos );
		int nCount = m_vpDynamicUI.Size();
		for( int i=0; i<nCount; ++i )
		{
			if( m_vpDynamicUI[ i ]->IsHaveFocus() && m_vpDynamicUI[ i ]->IsShowWindow() )
			{
				m_vpDynamicUI[ i ]->OnLButtonDown( pos );
				break;
			}
		}
	}
}


void cGameInterface::OnRButtonUp( CsPoint pos )
{
	if( cMessageBox::OnRButtonUpList( pos ) == true )
	{
		return;
	}

	if( m_bRenderIF == false )
		return;

	if( m_pStaticFocusWindow && m_pStaticFocusWindow->IsShowWindow() )
	{
		if( m_pStaticFocusWindow->IsEnableWindow() )
			m_pStaticFocusWindow->OnRButtonUp( pos );
	}
	else/* if( m_vpDynamicUI.IsExistElement( 0 ) && m_vpDynamicUI[ 0 ]->IsShowWindow() )*/
	{
		int nCount = m_vpDynamicUI.Size();
		for( int i=0; i<nCount; ++i )
		{
			if( m_vpDynamicUI[ i ]->IsHaveFocus() && m_vpDynamicUI[ i ]->IsShowWindow() )
			{
				m_vpDynamicUI[ i ]->OnRButtonUp( pos );
				break;
			}
		}
	}

	/*int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == true )
			m_vpStaticUI[ i ]->OnRButtonUp( pos );
	}

	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == true )
			m_vpDynamicUI[ i ]->OnRButtonUp( pos );
	}*/
}

bool cGameInterface::CursorIconLBtnDown( CsPoint pos, CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	if( cMessageBox::IsMessageBox() )
		return false;

	if( m_bRenderIF == false )
		return false;

	int nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == false )
			continue;

		if( !m_vpDynamicUI[ i ]->IsHaveFocus() )
			continue;

		if( m_vpDynamicUI[ i ]->IsInWindow( pos ) == false )
			continue;

		if( m_vpDynamicUI[ i ]->IsEnableWindow() == false )
			return true;

		if( m_vpDynamicUI[ i ]->CursorIconLBtnDown( eIconType, nIconSlot, nIconCount ) == true )
		{
			SetFocusWindow( m_vpDynamicUI[ i ] );
			return true;
		}
		return false;
	}

	nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->IsInWindow( pos ) == false )
			continue;

		if( m_vpStaticUI[ i ]->IsEnableWindow() == false )
			return true;

		if( m_vpStaticUI[ i ]->CursorIconLBtnDown( eIconType, nIconSlot, nIconCount ) == true )
		{
			SetFocusWindow( m_vpStaticUI[ i ] );
			return true;
		}
		return false;
	}

	CURSOR_ST.IconDropMessageBegin();
	return true;
}


bool cGameInterface::CursorIconLBtnUp( CsPoint pos, CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	if( cMessageBox::IsMessageBox() )
		return false;

	if( m_bRenderIF == false )
		return false;

	int nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == false )
			continue;

		if( !m_vpDynamicUI[ i ]->IsHaveFocus() )
			continue;

		if( m_vpDynamicUI[ i ]->IsInWindow( pos ) == false )
			continue;

		if( m_vpDynamicUI[ i ]->IsEnableWindow() == false )
			return true;

		if( m_vpDynamicUI[ i ]->CursorIconLBtnUp( eIconType, nIconSlot, nIconCount ) == true )
		{
			SetFocusWindow( m_vpDynamicUI[ i ] );
			return true;
		}
		return false;
	}

	nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->IsInWindow( pos ) == false )
			continue;

		if( m_vpStaticUI[ i ]->IsEnableWindow() == false )
			return true;

		if( m_vpStaticUI[ i ]->CursorIconLBtnUp( eIconType, nIconSlot, nIconCount ) == true )
		{
			SetFocusWindow( m_vpStaticUI[ i ] );
			return true;
		}
		return false;
	}

	return CURSOR_ST.IconDropMessageTry();
}


void cGameInterface::ResetDevice()
{
	if( g_pSeparate )
		g_pSeparate->ResetDevice();
	if( g_pCreateName )
		g_pCreateName->ResetDevice();
// 	if( m_pTooltip )
// 		m_pTooltip->ResetDevice();
	if( m_pAlime )
		m_pAlime->ResetDevice();
// 	if( m_pCompareTooltip )
// 		m_pCompareTooltip->ResetDevice();
// 	if( m_pRightTooltip )
// 		m_pRightTooltip->ResetDevice();
	if( m_pPopupWindow )
		m_pPopupWindow->ResetDevice();
	if( g_pMoneySeparate )
		g_pMoneySeparate->ResetDevice();
// 	if( g_pSecondPassword )
// 		g_pSecondPassword->ResetDevice();

	if( g_pTacticsAni )
		g_pTacticsAni->ResetDevice();

	if( m_pBossScene )
		m_pBossScene->ResetDevice();	

	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpStaticUI[ i ]->ResetDevice();
	}

	nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpDynamicUI[ i ]->ResetDevice();
	}
}

bool cGameInterface::OnMacroKey()
{
	int nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == false )
			continue;

		if( !m_vpDynamicUI[ i ]->IsHaveFocus() )
			continue;

		if( m_vpDynamicUI[ i ]->IsEnableWindow() == false )
			continue;

		if( m_vpDynamicUI[ i ]->OnMacroKey() == true )
			return true;
	}

	nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->IsEnableWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->OnMacroKey() == true )
			return true;
	}

	return false;
}

bool cGameInterface::OnMacroKey(const MSG& p_kMsg)
{
	if( cMessageBox::IsMessageBox() )// 팝업창이 띄워져 있을 경우 키보드 입력이 안되게 수정
		return true;

	int nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == false )
			continue;

		if( !m_vpDynamicUI[ i ]->IsHaveFocus() )
			continue;

		if( m_vpDynamicUI[ i ]->IsEnableWindow() == false )
			continue;

		if( m_vpDynamicUI[ i ]->OnMacroKey(p_kMsg) == true )
			return true;
	}

	nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->IsEnableWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->OnMacroKey(p_kMsg) == true )
			return true;
	}

	SAFE_POINTER_RETVAL( g_pCreateName, false );
	if( g_pCreateName->OnMacroKey( p_kMsg ) == true )
		return true;

	return false;
}

bool cGameInterface::OnEscapeKey()
{
	assert_cs( m_bRenderIF == true );

	

	if( cMessageBox::OnEscapeKeyList() == true )
	{
		return true;
	}

	cBaseWindow* pWindow = NULL;

	// 채팅 입력 창이 있다면 먼저 꺼주자
	ChatMainWindow* pChatWindow = GetChatMain();
	if( pChatWindow && pChatWindow->OnEscapeKey() == true )
		return true;

	if( g_pMoneySeparate )
	{
		g_pMoneySeparate->OnCancel();
		return true;
	}

	// 분리창이 있다면 역시 먼저 꺼주자
	if( g_pSeparate )
	{
		g_pSeparate->OnCancel();
		return true;
	}
	// 이름 입력 창이 있다면 먼저 꺼주자
	if( g_pCreateName )
	{
		g_pCreateName->OnCancel();
		return true;
	}

	// 커서아이콘 등록되어 있다면 커서아이콘 해제
	if( CURSOR_ST.IsIcon() == true )
	{
		CURSOR_ST.ReleaseIcon();
		return true;
	}

	// 팝업 있다면 꺼진다
	if( m_pPopupWindow->OnEscapeKey() == true )
	{
		return true;
	}

	// 캐쉬샵 켜져있으면 캐쉬샵부터 닫아주자
	if( IsActiveWindow( cBaseWindow::WT_NEW_CASH_WAREHOUSE ) == true )			// 캐시 창고
	{
		CloseDynamicIF( cBaseWindow::WT_NEW_CASH_WAREHOUSE );
		return true;
	}
	else if( IsActiveWindow( cBaseWindow::WT_NEW_CASHSHOP ) == true )	// 캐시 샵
	{
		if( _GetPointer(cBaseWindow::WT_NEW_CASHSHOP, 0)->OnEscapeKey() == true )
			return true; 
	}

	int nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpDynamicUI[ i ]->IsEnableWindow() == false )
			continue;

		if( m_vpDynamicUI[ i ]->OnEscapeKey() == true )
			return true;
	}

	nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->IsEnableWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->OnEscapeKey() == true )
			return true;
	}

	// 닷트 본부 이동중일때
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ENCHANT_TACTICS ); 
		GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_INVENTORY, NULL );
		FLOWMGR_ST.PopFlow( Flow::CFlow::FLW_DATSCENTER );

		return true;
	}

	return false;
}

bool cGameInterface::OnEnterKey()
{
	if( m_bRenderIF == false )
		return false;

	if( cMessageBox::OnEnterKeyList() == true )
	{
		return true;
	}

	// 포커스 에디트 창이 입력을 받아 들이는 창이라면 처리
	if( g_pFocusEdit && g_pFocusEdit->IsWantReturn() )
	{
		g_IME.AddInput( _T( "\n" ), 1 );
		return true;
	}

	if( g_pMoneySeparate )
	{
		g_pMoneySeparate->OnOk();
		return true;
	}

	// 분리창이 있다면 먼저 꺼주자
	if( g_pSeparate )
	{
		g_pSeparate->OnOk();
		return true;
	}

	// 이름입력창 있다면 먼저 꺼주자
	if( g_pCreateName )
	{
#ifndef NOT_ENTER_CREATENAME	// 대만에서는 글자 조합 선택 시 Enter로 하므로 조합 중 Enter로 생성되지 않도록 함
		g_pCreateName->OnOk();
#endif
		return true;
	}

	if(	EnterKeyUpdate() )
		return true;

	return false;
}

void cGameInterface::ChangeMacroKey()
{
	int nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == false )
			continue;

		m_vpStaticUI[ i ]->ChangeMacroKey();	
	}
}

bool cGameInterface::IsInWindow( CsPoint pos )
{
	int nCount = m_vpDynamicUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpDynamicUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpDynamicUI[ i ]->IsInWindow( pos ) )
		{
			return true;
		}		
	}
	nCount = m_vpStaticUI.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpStaticUI[ i ]->IsShowWindow() == false )
			continue;

		if( m_vpStaticUI[ i ]->IsInWindow( pos ) )
		{
			return true;
		}
	}
	return false;
}

bool cGameInterface::IsOpenNpcWindow( bool bCheckTalk /*=true*/ )
{
	// 상인 관련 창이 열려있다면 패스 하자
	if( IsActiveWindow( cBaseWindow::WT_STORE ) ||
		IsActiveWindow( cBaseWindow::WT_DIGITAMA_SCAN ) ||
		IsActiveWindow( cBaseWindow::WT_DIGITAMA_SELL ) ||
		IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) ||
#ifdef LJW_EQUIPSYSTEM_190724
		IsActiveWindow( cBaseWindow::WT_EQUIPMENT_SYSTEM )||
#else
		IsActiveWindow( cBaseWindow::WT_ELEMENTITEM_CHECK )||
		IsActiveWindow( cBaseWindow::WT_ELEMENTITEM_ATTACH )||
#endif
		IsActiveWindow( cBaseWindow::WT_WAREHOUSE )||		
		IsActiveWindow( cBaseWindow::WT_TRADE )||
		IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE )||
		IsActiveWindow( cBaseWindow::WT_PERSONSTORE )||
		IsActiveWindow( cBaseWindow::WT_CAPSULE_MACHINE ) ||
		IsActiveWindow( cBaseWindow::WT_ITEM_PRODUCTION_SHOP )
		|| IsActiveWindow( cBaseWindow::WT_SPIRIT_EVOLUTION )
		|| IsActiveWindow( cBaseWindow::WT_DIGIMON_INCUBATOR )
		)
	{
		return true;
	}

	if( bCheckTalk )
	{
		if( IsActiveWindow( cBaseWindow::WT_TALK )||
			IsActiveWindow( cBaseWindow::WT_QUEST_REV )||
			IsActiveWindow( cBaseWindow::WT_QUEST_COMP ) )
		{
			return true;
		}
	}

	// 닷트 본부 이동중일때
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS )
	{
		return true;
	}
	
	// 이벤트씬 보는 중일때
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_EVENT )
	{
		return true;
	}

	// 디지몬 부화 보는중
	if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_TACTICSANI )
	{
		return true;
	}

	if( IsActiveWindow( cBaseWindow::WT_CARD_EVENT ) )
	{
		return true;
	}
	if( IsActiveWindow( cBaseWindow::WT_NEW_GOTCHA_MACHINE ) )
	{
		return true;
	}
	if( IsActiveWindow( cBaseWindow::WT_MASTERS_MATCHING ) )
	{
		return true;
	}
	if( IsActiveWindow( cBaseWindow::WT_ENCHANT_OPTION ) )
	{
		return true;
	}
	if( IsActiveWindow( cBaseWindow::WT_DIGIMON_INCUBATOR ) )
	{
		return true;
	}
	return false;
}

bool cGameInterface::IsSpeacialAct()
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_ServerSync* pDataSync = g_pDataMng->GetServerSync();
	SAFE_POINTER_RETVAL( pDataSync, false );

	// 특정행동중
	if( pDataSync->IsEmptyRefCount( cData_ServerSync::CASH_CAPSULE_COIN_COMPLEATE ) == false )		// 캡슐 코인 중
	{
		return true;
	}

	return false;
}

//==========================================================================================================
//
//	Pointer
//
//==========================================================================================================

void cGameInterface::_PointerDetach( cBaseWindow* pWindow )
{
	SAFE_POINTER_RET( pWindow );

	DWORD dwArrayID = pWindow->GetWindowType()*100 + pWindow->__GetPointerIndex();
	std::map< DWORD, cBaseWindow* >::iterator it = m_mapWindowArray.find( dwArrayID );
	if( it == m_mapWindowArray.end() )
		return;

	m_mapWindowArray.erase( it );

	if(m_pStaticFocusWindow && m_pStaticFocusWindow->GetWindowType() == pWindow->GetWindowType())	//
	{
		m_pStaticFocusWindow = NULL;
	}

	// 겜브리오에서 포인터 없으면 디버그 경고나서 - 귀차나서 일일이 찾아서 지워주자;;
	cBaseWindow::DestroyWindow( pWindow );
}


cBaseWindow* cGameInterface::_GetPointer( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex )
{
	assert_cs( nPointerIndex >= 0 );
	DWORD nID = wt*100 + nPointerIndex;
	std::map< DWORD, cBaseWindow* >::iterator it = m_mapWindowArray.find( nID ) ;
	if( it != m_mapWindowArray.end() )
		return it->second;
	return NULL;
}

bool cGameInterface::IsActiveWindow( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex /*=0*/ )
{
	SAFE_POINTER_RETVAL(g_pGameIF, false);
	cBaseWindow* pWindow = _GetPointer( wt, nPointerIndex );
	if( ( pWindow != NULL )&&( pWindow->IsShowWindow() ) )
		return true;

	return false;
}

void cGameInterface::_UpdateReserveFocusWindow()
{
	assert_cs( m_vpReserveFocusWindow.Size() == m_vpReserveFocusCallFunc.Size() );
	int nCount = m_vpReserveFocusWindow.Size();
	for( int i=0; i<nCount; ++i )
	{
		cBaseWindow* pWindow = m_vpReserveFocusWindow[ i ];
		bool bCallFunc = m_vpReserveFocusCallFunc[ i ];
		if( pWindow->IsStaticWindow() == false )
		{
			// 가장 상위가 아니라면
			if( m_vpDynamicUI[ 0 ] != pWindow )
			{
				int nCount = m_vpDynamicUI.Size();
				int nIndex = 1;
				for( ; nIndex<nCount; ++nIndex )
				{
					if( m_vpDynamicUI[ nIndex ] == pWindow )
					{
						break;
					}
				}

				if( m_pStaticFocusWindow != NULL )
				{
					if( bCallFunc == true )
						m_pStaticFocusWindow->OnKillFocus();
					m_pStaticFocusWindow = NULL;
				}
				else
				{
					if( bCallFunc == true )
						m_vpDynamicUI[ 0 ]->OnKillFocus();
				}

				int nStopPos = 0;
				for( int i = nIndex-1; i > -1; --i )
				{
					if( m_vpDynamicUI [ i ]->IsAlwaysTop() )
					{
						nStopPos = i + 1;
						break;
					}
				}

				// 나보다 위에 있는 윈도우들을 한단계 내린다
				for( int n = nIndex-1; n > (nStopPos - 1); --n )
					m_vpDynamicUI[ n + 1 ] = m_vpDynamicUI [ n ];

				m_vpDynamicUI[ nStopPos ] = pWindow;
				if( bCallFunc == true )
					pWindow->OnSetFocus();
			}
			else
			{
				m_pStaticFocusWindow = NULL;
			}
		}
		else
		{
			if( m_pStaticFocusWindow != pWindow )
			{
				if( bCallFunc == true )
				{
					if( m_pStaticFocusWindow != NULL )
					{
						m_pStaticFocusWindow->OnKillFocus();
					}
					else if( m_vpDynamicUI.Size() != 0 )
					{
						m_vpDynamicUI[ 0 ]->OnKillFocus();
					}
				}	

				m_pStaticFocusWindow = pWindow;

				if( bCallFunc == true )
					m_pStaticFocusWindow->OnSetFocus();
			}			
		}
	}

	m_vpReserveFocusWindow.ClearElement();
	m_vpReserveFocusCallFunc.ClearElement();
}


bool cGameInterface::GetFocusWindow(cBaseWindow* pWindow)
{
	SAFE_POINTER_RETVAL(pWindow, true);

	if( m_vpDynamicUI[ 0 ] != pWindow )	// 가장 상위가 아니라면
		return false;
	return true;
}


void cGameInterface::SetFocusWindow( cBaseWindow* pWindow, bool bCallFounsFunc /*=true*/ )
{
	m_vpReserveFocusCallFunc.PushBack( bCallFounsFunc );
	m_vpReserveFocusWindow.PushBack( pWindow );	
}

//==========================================================================================================
//
//	Dynamic
//
//==========================================================================================================
cBaseWindow* cGameInterface::GetDynamicIF( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex /*=0*/, int nValue /*=0*/, bool bOpenSound /*=true*/ )
{
	cBaseWindow* pWindow = NULL;		

	switch( wt )
	{
	case cBaseWindow::WT_STORE:
	case cBaseWindow::WT_DIGITAMA_SCAN:
	case cBaseWindow::WT_DIGITAMA_SELL:	
	case cBaseWindow::WT_ELEMENTITEM_CHECK: 
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
	case cBaseWindow::WT_WAREHOUSE:
	case cBaseWindow::WT_TRADE:		
	case cBaseWindow::WT_PERSONSTORE:
	case cBaseWindow::WT_CAPSULE_MACHINE:
	case cBaseWindow::WT_ITEM_PRODUCTION_SHOP:	// 아이템 제작 샵
		{	
			//인벤 정렬중일때 상인 관련창 사용불가
			if( g_pDataMng->IsSort() == true )
			{
				cPrintMsg::PrintMsg( 30490 );
				return NULL;
			}
		}		
		break;
	case cBaseWindow::WT_TALK:
		{
			// 씰 마스터가 Active되어 있을 때 UI를 종료 시켜버리고 NPC 토크가 가능하도록 수정
			int mode = 0;//닫기
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEALMASTER_UI_OPEN_CLOSE, &mode );
			// 상인 관련 창이 열려있다면 패스 하자
			if( IsOpenNpcWindow( false ) )
				return NULL;

			assert_cs( nPointerIndex == 0 );
		}
		break;
	case cBaseWindow::WT_MAINOPTION:
		{
			// 옵션 관련 창이 열려있다면 패스 하자
			if( IsActiveWindow( cBaseWindow::WT_OPTION_MACRO ) ||
				IsActiveWindow( cBaseWindow::WT_OPTION_SOUND ) ||
				IsActiveWindow( cBaseWindow::WT_OPTION_GRAPHIC ) ||
				IsActiveWindow( cBaseWindow::WT_OPTION_INTERFACE ) ||	
				IsActiveWindow( cBaseWindow::WT_OPTION_USERGRADE) )
			{
				return NULL;
			}

			assert_cs( nPointerIndex == 0 );
		}
		break;
	default:
		break;
	}

	pWindow = _GetPointer( wt, nPointerIndex );
	if( pWindow == NULL )
	{
		pWindow = _NewDynamicIF( wt, true, nPointerIndex );
		assert_cs( pWindow != NULL );
		SAFE_POINTER_RETVAL(pWindow, NULL);
		pWindow->Open( nValue, bOpenSound );
	}
	else
	{
		if( pWindow->IsShowWindow() == false )
			pWindow->Open( nValue, bOpenSound );

		SetFocusWindow( pWindow );
	}
	return pWindow;
}

bool cGameInterface::CloseDynamicIF( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex /*=0*/, bool bIncludeDisableWindow /*=false*/ )
{
	cBaseWindow* pWindow = _GetPointer( wt, nPointerIndex );
	if( ( pWindow != NULL )&&( pWindow->IsShowWindow() == true ) )
	{
		if( bIncludeDisableWindow == true )
			pWindow->SetEnableWindow( true );
		pWindow->Close();
		return true;
	}
	return false;
}

cBaseWindow* cGameInterface::_NewDynamicIF( cBaseWindow::eWINDOW_TYPE wt, bool bFocus, int nPointerIndex )
{
	cBaseWindow* pWindow = cBaseWindow::NewWindow( wt, nPointerIndex );
	SAFE_POINTER_RETVAL(pWindow, NULL);
	_InsertDynamicWindow( pWindow, bFocus, nPointerIndex );
	return pWindow;
}

void cGameInterface::_InsertDynamicWindow( cBaseWindow* pWindow, bool bFocus /*=false*/, int nPointerIndex /*= 0*/ )
{
	DWORD dwArrayID = pWindow->GetWindowType()*100 + nPointerIndex;
	m_mapWindowArray[ dwArrayID ] = pWindow;

	pWindow->SetStaticWindow( false );
	m_vpDynamicUI.PushBack( pWindow );
	if( bFocus == true )
	{
		SetFocusWindow( pWindow );
	}
}

//==========================================================================================================
//
//	Static
//
//==========================================================================================================
void cGameInterface::_InsertStaticWindow( cBaseWindow* pWindow, bool bFocus /*=false*/, int nPointerIndex /*= 0*/ )
{
	DWORD dwArrayID = pWindow->GetWindowType()*100 + nPointerIndex;
	m_mapWindowArray[ dwArrayID ] = pWindow;
	
	pWindow->SetStaticWindow( true );
	m_vpStaticUI.PushBack( pWindow );
	if( bFocus == true )
	{
		SetFocusWindow( pWindow );
	}
}
cBaseWindow* cGameInterface::CreateDynamicWindow( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex, int value, bool bFocus, bool bShow, bool bSound)
{
	cBaseWindow* pWindow = cBaseWindow::NewWindow( wt, nPointerIndex);
	SAFE_POINTER_RETVAL(pWindow, NULL);
	pWindow->Open( value, bSound );
	pWindow->SetShowWindow(bShow);
	_InsertDynamicWindow( pWindow, bFocus, nPointerIndex );
	return	pWindow;
}
cBaseWindow* cGameInterface::CreateStaticWindow( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex, int value, bool bFocus, bool bShow, bool bSound)
{
	cBaseWindow* pWindow = cBaseWindow::NewWindow( wt, nPointerIndex);
	SAFE_POINTER_RETVAL(pWindow, NULL);
	pWindow->Open( value, bSound );
	pWindow->SetShowWindow(bShow);
	_InsertStaticWindow( pWindow, bFocus, nPointerIndex );
	return	pWindow;
}

void cGameInterface::DeleteDynamicWindow( cBaseWindow::eWINDOW_TYPE wt){

	int nCount = m_vpDynamicUI.Size();

	for( int i=0; i<nCount; ++i )
	{
		if(	m_vpDynamicUI[i]->GetWindowType() == wt)
		{
			_PointerDetach( m_vpDynamicUI[i] );
			m_vpDynamicUI.DeleteElement(i);
			nCount--;
			break;
		}
	}

	if(nCount==0)
		m_vpDynamicUI.Destroy();
}
void cGameInterface::DeleteStaticWindow( cBaseWindow::eWINDOW_TYPE wt){

	int nCount = m_vpStaticUI.Size();

	for( int i=0; i<nCount; ++i )
	{
		if(	m_vpStaticUI[i]->GetWindowType() == wt)
		{
			_PointerDetach( m_vpStaticUI[i] );
			m_vpStaticUI.DeleteElement(i);
			nCount--;
			break;
		}
	}

	if(nCount==0)
		m_vpStaticUI.Destroy();
}

void cGameInterface::SetBright()
{	
	float fBright = 1.0f - g_pResist->m_Global.s_fBright;
	fBright /= 5;

	NiColorA vColor( 0.0f, 0.0f, 0.0f, fBright );
	if( fBright < 0 )
	{		
		fBright /= 5;
		vColor = NiColorA( 1.0f, 1.0f, 1.0f, abs(fBright) );		
	}

	m_pBright->SetColorA( vColor );
}

bool cGameInterface::IsActiveWindow( const char* pWinName ) const
{
	std::map< DWORD, cBaseWindow* >::const_iterator it = m_mapWindowArray.begin();
	std::map< DWORD, cBaseWindow* >::const_iterator itend = m_mapWindowArray.end();
	cBaseWindow* pFindWin = NULL;
	for( ; it != itend ; ++it)
	{
		pFindWin = it->second;
		SAFE_POINTER_CON( pFindWin );
		if( pFindWin->IsSameWindow( pWinName ) )
			return pFindWin->IsShowWindow();
	}

	return false;
}

cBaseWindow*	cGameInterface::GetWindow( const char* pWinName )
{
	std::map< DWORD, cBaseWindow* >::const_iterator it = m_mapWindowArray.begin();
	for( ; it != m_mapWindowArray.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( it->second->IsSameWindow( pWinName ) )
			return it->second;
	}
	return NULL;
}

cBaseWindow::eWINDOW_TYPE cGameInterface::GetWindowType( const char* pWinName ) const
{
	std::map< DWORD, cBaseWindow* >::const_iterator it = m_mapWindowArray.begin();
	std::map< DWORD, cBaseWindow* >::const_iterator itend = m_mapWindowArray.end();
	cBaseWindow* pFindWin = NULL;
	for( ; it != itend ; ++it)
	{
		pFindWin = it->second;
		SAFE_POINTER_CON( pFindWin );
		if( pFindWin->IsSameWindow( pWinName ) )
			return pFindWin->GetWindowType();
	}
	return cBaseWindow::WT_NONE;
}

void cGameInterface::HideWindow()
{
	m_MapHideWindow.clear();

	std::map< DWORD, cBaseWindow* >::const_iterator it = m_mapWindowArray.begin();
	std::map< DWORD, cBaseWindow* >::const_iterator itend = m_mapWindowArray.end();
	cBaseWindow* pFindWin = NULL;
	for( ; it != itend ; ++it)
	{
		DWORD	Idx = it->first;
		pFindWin = it->second;
		if(pFindWin==NULL)
		{
			m_MapHideWindow.insert(make_pair(Idx, false));
			continue;
		}
		else if(pFindWin->IsShowWindow())
		{
			m_MapHideWindow.insert(make_pair(Idx, true));
			pFindWin->SetShowWindow(false);
		}		
		else if(!pFindWin->IsShowWindow())
		{
			m_MapHideWindow.insert(make_pair(Idx, false));
		}		

	}
}

void cGameInterface::HideWindowRestore()
{
	cBaseWindow*	pFindWin = NULL;
	bool			bShowWindow=false;

	std::map<DWORD, bool>::const_iterator it = m_MapHideWindow.begin();
	std::map<DWORD, bool>::const_iterator itend = m_MapHideWindow.end();

	for( it; it != itend; ++it)
	{
		int idx = it->first;
		bShowWindow	= it->second;

		pFindWin = m_mapWindowArray[idx];
		
		if(pFindWin==NULL)
		{
			continue;
		}
		else if(bShowWindow)
		{
			pFindWin->SetShowWindow(true);
		}		
		else if(!bShowWindow)
		{
			pFindWin->SetShowWindow(false);
		}
	}

	m_MapHideWindow.clear();
}

void cGameInterface::HideWindowRestore(DWORD const& dwWinType, int nPtrIndex /*= 0*/)
{
	DWORD dwWindowType = dwWinType*100 + nPtrIndex;
	std::map<DWORD, bool>::iterator itHide = m_MapHideWindow.find( dwWindowType );
	if( itHide == m_MapHideWindow.end() )
		return;

	std::map< DWORD, cBaseWindow* >::iterator itWin = m_mapWindowArray.find( itHide->first );
	if( itWin == m_mapWindowArray.end() )
		return;

	SAFE_POINTER_RET( itWin->second );
	itWin->second->SetShowWindow( true );

	m_MapHideWindow.erase( itHide );
}

//////////////////////////////////////////////////////////////////////////
void cGameInterface::EnterKeyCreate()
{
	// 엔터 키 토글.
	m_vEnterKey.clear();

	m_vEnterKey.push_back( stWinType(cBaseWindow::WT_NEW_CASHSHOP) );
	m_vEnterKey.push_back( stWinType(cBaseWindow::WT_TALK) );

#ifndef NOT_ENTER_CREATENAME	// 대만에서는 글자 조합 선택 시 Enter로 하므로 조합 중 Enter로 생성되지 않도록 함
	m_vEnterKey.push_back( stWinType(cBaseWindow::WT_DIGIMON_TALK) );	// 디지몬 대화
	m_vEnterKey.push_back( stWinType(cBaseWindow::WT_COMMUNITY) );		// 길드창 공지사항
	m_vEnterKey.push_back( stWinType(cBaseWindow::WT_SHOUT_CASH) );		// 외치기창
#endif
	m_vEnterKey.push_back( stWinType(cBaseWindow::WT_CHAT_MAIN_WINDOW) );	// 새로운 채팅창 토글
}

bool cGameInterface::EnterKeyUpdate()
{
	cBaseWindow* pWindow = NULL;

	// 엔터 키 토글.
	for(WINTYPE_ITER iter = m_vEnterKey.begin(); iter != m_vEnterKey.end(); ++iter)
	{
		stWinType eData = *iter;

		pWindow = _GetPointer( eData.type, eData.index );

		SAFE_POINTER_CON(pWindow);

		if( pWindow->IsShowWindow() && pWindow->OnEnterKey() )
			return true;
	}

	return false;
}