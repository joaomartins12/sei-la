
#include "stdafx.h"
#include "Popup.h"

cPopUpWindow::cPopUpWindow()
{
	m_eType = NONE;
	m_pChannelImage1 = m_pChannelImage2 = NULL;	 
	for( int i = 0; i < MAINBAR_LIST; i++ )
		m_pSprMainBar[ i ] = NULL;
}

void cPopUpWindow::Delete()
{
	ClosePopup();

	SAFE_NIDELETE( m_pPopupWindow );
	SAFE_NIDELETE( m_pSelMask );
	SAFE_NIDELETE( m_pLine );
	SAFE_NIDELETE( m_pChannelImage1 );
	SAFE_NIDELETE( m_pChannelImage2 );

	for( int i = 0; i < MAINBAR_LIST; i++ )
	{
		SAFE_NIDELETE( m_pSprMainBar[ i ] );
	}
	//SAFE_NIDELETE(m_pSprMainBar[MAINBAR_INV]);
	m_StringList.Delete();
}

// Inicialize PopUps (images path)
void cPopUpWindow::Init()
{
	m_pPopupWindow = NiNew cPopUp;
	m_pPopupWindow->Init( NULL, CsPoint::ZERO, CsPoint( 10, 10 ), CsPoint( 16, 16 ), "Control_G\\Popup\\N2Dlg", "tga", false );

	m_pLine = NiNew cSprite;
	m_pLine->Init( NULL, CsPoint::ZERO, CsPoint( 50, 1 ), "Control_G\\Popup\\N2Dlg_Line.tga", false );

	m_pSelMask = NiNew cSprite;
	m_pSelMask->Init( NULL, CsPoint::ZERO, CsPoint( 220, 22 ), "Talk\\Common_Seletbar.tga", false );

	m_pChannelImage1 = NiNew cImage;
	m_pChannelImage1->Init( NULL, CsPoint::ZERO, CsPoint( 31, 17 ), "Channel\\Image1.tga", true );
	m_pChannelImage1->SetTexToken( CsPoint( 0, 17 ) );
	m_pChannelImage2 = NiNew cImage;
	m_pChannelImage2->Init( NULL, CsPoint::ZERO, CsPoint( 31, 17 ), "Channel\\Image2.tga", true );
	m_pChannelImage2->SetTexToken( CsPoint( 0, 17 ) );

	//m_pSprMainBar[ MAINBAR_ACHIVE ] = NiNew cSprite;
	//m_pSprMainBar[ MAINBAR_ACHIVE ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "MainBar\\Main_Ach_icon.tga" , true );
	//m_pSprMainBar[ MAINBAR_QUEST ] = NiNew cSprite;
	//m_pSprMainBar[ MAINBAR_QUEST ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "MainBar\\Main_Quest_icon.tga" , true );
	//m_pSprMainBar[ MAINBAR_CASH ] = NiNew cSprite;
	//m_pSprMainBar[ MAINBAR_CASH ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "MainBar\\Main_Cash_icon.tga" , true );
	//m_pSprMainBar[ MAINBAR_GUILD ] = NiNew cSprite;
	//m_pSprMainBar[ MAINBAR_GUILD ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "MainBar\\Main_Guild_icon.tga" , true );
	//m_pSprMainBar[ MAINBAR_HELP ] = NiNew cSprite;
	//m_pSprMainBar[ MAINBAR_HELP ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "MainBar\\Main_Help_icon.tga" , true );
	//m_pSprMainBar[ MAINBAR_INGAME ] = NiNew cSprite;
	//m_pSprMainBar[ MAINBAR_INGAME ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "MainBar\\Main_Ingame_icon.tga" , true );
	//m_pSprMainBar[ MAINBAR_BATTLE ] = NiNew cSprite;
	//m_pSprMainBar[ MAINBAR_BATTLE ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "MainBar\\Main_Battle_Info_icon.tga" , true );
	//m_pSprMainBar[ CASHSHOP ] = NiNew cSprite;
	//m_pSprMainBar[ CASHSHOP ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "MainBar\\Main_CashShop_icon.tga" , true );	
	//m_pSprMainBar[ CASHSHOP ]->SetVisible(false);
	//m_pSprMainBar[ MAINBAR_GIFT ] = NiNew cSprite;
	//m_pSprMainBar[ MAINBAR_GIFT ]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "GiftStorage\\Main_Gift_icon.tga" , true );	
	
	#pragma region ---------- WAREHOUSE POPUP IMAGES ----------

	m_pSprMainBar[MAINBAR_CASHWAREHOUSE] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_CASHWAREHOUSE]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_cashWarehouse.png", true);

	#pragma endregion

	#pragma region ---------- COMMUNITY POPUP IMAGES ----------
 
	m_pSprMainBar[MAINBAR_GUILD] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_GUILD]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_community.png", true);

	m_pSprMainBar[MAINBAR_FRIEND] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_FRIEND]->Init( NULL, CsPoint::ZERO, CsPoint( 26, 26 ), "QuickMenu\\Button_friend.png" , true );

	m_pSprMainBar[MAINBAR_MAIL] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_MAIL]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_mail.png", true);

	#pragma endregion

	#pragma region ---------- INVENTORY POPUP IMAGES ----------

	m_pSprMainBar[MAINBAR_INV] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_INV]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_inventory.png", true);

	m_pSprMainBar[MAINBAR_EXCLUSIVEINV] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_EXCLUSIVEINV]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_exclusive_inventory.png", true);
	
	#pragma endregion

	#pragma region ---------- DATA POPUP IMAGES ----------

	m_pSprMainBar[MAINBAR_SEAL] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_SEAL]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_seal.png", true);
	
	m_pSprMainBar[MAINBAR_ENCYCLOPEDIA] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_ENCYCLOPEDIA]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_encyclopedia.png", true);
	
	m_pSprMainBar[MAINBAR_EVOLUTIONHELPS] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_EVOLUTIONHELPS]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_evolution.png", true);
	
	m_pSprMainBar[MAINBAR_BATTLE] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_BATTLE]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_battle.png", true);

	#pragma endregion

	#pragma region ---------- CHARACTER POPUP IMAGES ----------

	m_pSprMainBar[MAINBAR_POTION] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_POTION]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_potion.png", true);

	m_pSprMainBar[MAINBAR_TAMER] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_TAMER]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_tamer.png", true);

	m_pSprMainBar[MAINBAR_DIGIMON] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_DIGIMON]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_digimon.png", true);

	m_pSprMainBar[MAINBAR_ACHIEVEMENT] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_ACHIEVEMENT]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_achievement.png", true);

	m_pSprMainBar[MAINBAR_QUEST] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_QUEST]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_quest.png", true);

	m_pSprMainBar[MAINBAR_EXCHANGE] = NiNew cSprite;
	m_pSprMainBar[MAINBAR_EXCHANGE]->Init(NULL, CsPoint::ZERO, CsPoint(26, 26), "QuickMenu\\Button_exchange.png", true);

	#pragma endregion

	// ---------------------------------------------------

	ClosePopup();
	m_nSubID1 = m_nSubID2 = 0;
}

void cPopUpWindow::ClosePopup()
{
	m_bRenderPopup = false;

#ifdef GUILD_RENEWAL
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_GUILD_MEMO ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILD_MEMO );
#endif
}

void cPopUpWindow::ClosePopup( eTYPE type )
{
	if( m_eType == type )
	{
		m_bRenderPopup = false;
	}
#ifdef GUILD_RENEWAL
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_GUILD_MEMO ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILD_MEMO );
#endif
}

// Update PopUps
bool cPopUpWindow::Update_ForMouse()
{
	if (m_bRenderPopup == false)
		return false;

	if (!m_pPopupWindow)
		return false;

	CsRect rc(m_ptUpdateMouse, m_ptUpdateMouse + m_pPopupWindow->GetSize());
	if (rc.PtInRect(CURSOR_ST.GetPos()) == FALSE)
	{
		CURSOR::eTYPE ct = CURSOR_ST.GetButtonState();
		if ((ct == CURSOR::LBUTTON_DOWN) || (ct == CURSOR::RBUTTON_DOWN))
		{
			ClosePopup();
			return true;
		}

		return false;
	}

	switch (m_eType)
	{
	case TACTICS:
	case DIGIVICE_TACTICS:				return _Update_ForMouse_Tactics();
	case CHANNEL_LIST:					return _Update_ForMouse_ChannelList();
	case OTHER_TAMER:					return _Update_ForMouse_OtherTamer();
	case OTHER_DIGIMON:					return _Update_ForMouse_OtherDigimon();
	case PARTY_MENU:					return _Update_ForMouse_PartyMenu();
	case GUILD_MENU:					return _Update_ForMouse_GuildMenu();
	case EMOTION:						return _Update_ForMouse_Emotion();
	case FRIEND_LIST:					return _Update_ForMouse_Friend();
	case SHUTOUT_LIST:					return _Update_ForMouse_ShutOut();
	case MAINBAR_MISSION:				return _Update_ForMouse_Mainbar_Mission();
	case MAINBAR_COMMUNITY:				return _Update_ForMouse_Mainbar_Community();
	case MAINBAR_STORE:					return _Update_ForMouse_Mainbar_Store();
	case SEAL_MENU:						return _Update_ForMouse_SealPopMenu();
	case VIP_SYSTEM_MENU:				return _Update_ForMouse_VipSystemPopMenu();
	case ITEM_MAKE_ASSIST_ITEM_REG:		return _Update_ForMouse_AssistItem_PopMenu();
	case MAINBAR_INVENTORY:				return _Update_ForMouse_Mainbar_Inventory();
	case MAINBAR_DATA:					return _Update_ForMouse_Mainbar_Data();
	case MAINBAR_CHARACTER:				return _Update_ForMouse_Mainbar_Character();
	case EXP_EVENT:
		return true;
	}

	ClosePopup();
	return false;
}

bool cPopUpWindow::Render()
{
	if (m_bRenderPopup == false)
		return false;

	if (!m_pPopupWindow)
		return false;

	CsPoint pos = m_ptRenderPos;
	pos.y += m_ptPatchSize.y;

	if ((m_ptMaxSize.x + pos.x) > g_nScreenWidth)
		pos.x = m_ptRenderPos.x - m_ptMaxSize.x;

	if ((m_ptMaxSize.y + pos.y) > g_nScreenHeight)
		pos.y = m_ptRenderPos.y - m_ptMaxSize.y;

	m_pPopupWindow->Render(pos);

	if (m_bRenderSelMask == true)
	{
		if (m_pSelMask)
			m_pSelMask->Render(m_ptSelMask);

		m_bRenderSelMask = false;
	}

	m_StringList.Render(pos + CsPoint(10, 10), POPUP_STRINGLIST_DELTA_Y);

	m_ptUpdateMouse = pos;
	return true;
}

void cPopUpWindow::ResetDevice()
{
	if (m_pPopupWindow)
		m_pPopupWindow->ResetDevice();

	if (m_pSelMask)
		m_pSelMask->ResetDevice();

	m_StringList.ResetDevice();
}

void cPopUpWindow::PreResetMap()
{
	ClosePopup();
	m_eType = NONE;
	m_nSubID1 = m_nSubID2 = 0;
}

// Create Popups
void cPopUpWindow::SetPopup(CsPoint pos, CsPoint patch, eTYPE type, int nSub1 /*=0*/, int nSub2 /*=0*/)
{
	if (!g_pGameIF)
		return;

	if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_TRADE))
		return;

	if (!m_pPopupWindow || !m_pSelMask || !m_pLine)
		return;

	m_ptRenderPos = pos;
	m_ptUpdateMouse = pos;
	m_ptPatchSize = patch;
	m_bRenderPopup = true;
	m_bRenderSelMask = false;

	m_StringList.Delete();
	m_eType = type;
	m_nSubID1 = nSub1;
	m_nSubID2 = nSub2;

	m_pLine->SetSize(CsPoint(1, 1));

	switch (m_eType)
	{
	case TACTICS:						_Make_Tactics();						break;
	case DIGIVICE_TACTICS:				_Make_DigiviceTactics();				break;
	case CHANNEL_LIST:					_Make_ChannelList();					break;
	case OTHER_TAMER:					_Make_OtherTamer();						break;
	case OTHER_DIGIMON:					_Make_OtherDigimon();					break;
	case PARTY_MENU:					_Make_PartyMenu();						break;
	case GUILD_MENU:					_Make_GuildMenu();						break;
	case EMOTION:						_Make_Emotion();						break;
	case FRIEND_LIST:					_Make_Friend();							break;
	case SHUTOUT_LIST:					_Make_ShutOut();						break;
	case MAINBAR_MISSION:				_Make_Mainbar_Mission();				break;
	case MAINBAR_COMMUNITY:				_Make_Mainbar_Community();				break;
	case MAINBAR_STORE:					_Make_Mainbar_Store();					break;
	case EXP_EVENT:						_Make_ExpEventTooltip();				break;
	case SEAL_MENU:						_Make_SealPopMenu();					break;
	case VIP_SYSTEM_MENU:				_Make_VipSystemPopMenu();				break;
	case ITEM_MAKE_ASSIST_ITEM_REG:		_Make_ItemMake_AssistItem_PopMenu();	break;
	case MAINBAR_INVENTORY:				_Make_Mainbar_Inventory();				break;
	case MAINBAR_DATA:					_Make_Mainbar_Data();					break;
	case MAINBAR_CHARACTER:				_Make_Mainbar_Character();				break;
	default:
		ClosePopup();
		return;
	}

	m_ptMaxSize = m_StringList.CalMaxSize(POPUP_STRINGLIST_DELTA_Y) + CsPoint(20, 20);
	m_ptMaxSize.x = CsMax(m_ptMaxSize.x, m_ptPatchSize.x);

	m_pPopupWindow->ReSize(m_ptMaxSize);
	m_pSelMask->SetSize(CsPoint(m_ptMaxSize.x - 10, 22));
	m_pLine->SetSize(CsPoint(m_ptMaxSize.x - 16, 1));
}

void cPopUpWindow::_AddLine()
{
	cString* pEmptyString = NiNew cString;
	m_StringList.AddTail( pEmptyString );

	cString* pString = NiNew cString;
	pString->AddSprite( m_pLine, CsPoint( -2, 0 ) );
	pString->AddSizeY( 8 );
	m_StringList.AddTail( pString );
}

// ---------- 

bool cPopUpWindow::_Update_ForMouse_Tactics()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	CDigimonUser* pDUser = g_pCharMng->GetDigimonUser( 0 );		
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			switch( nActive1 )
			{
			case TACTICS_NONE:
				break;
			case TACTICS_SUMMON:
				{
					ClosePopup();

#ifdef CROSSWARS_SYSTEM
					// 현재 디지몬이 크로스 용병이면 일반 채인지 불가
					DWORD dwMyDigimon = pDUser->GetBaseDigimonFTID();
					if(nsCsFileTable::g_pDigimonMng->IsCrossDigimon(dwMyDigimon))
					{
						return true;
					}
#endif
					//현재 조그레스 상태인가?
					if(pDUser->IsJointProgressing() || pDUser->IsCurJointProgress())
					{
						cPrintMsg::PrintMsg( 30511 );
						return true;
					}

#ifdef SDM_TAMER_XGUAGE_20180628
					if(pDUser->GetFTDigimon()->isX_AntiJogress())
					{
						cPrintMsg::PrintMsg( 30511 );
						return true;
					}
#endif
					if( cData_Tactics::Check_EnableChange() == true )
					{			

						int nCPIndex = m_nSubID1 + 1;
						g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CHANGE_PARTNER, 0, &nCPIndex );
					}
				}
				break;
			case TACTICS_FIRE:
				{
					ClosePopup();

					if( g_pDataMng->GetServerSync()->IsChageDigimon() == false )	//1 서버 상태검사 = 채인지 파트너 보낼수있냐?
					{
						// 초기시작 디지몬은 지울수없다.
						if( nsCsFileTable::g_pDigimonMng->IsStartDigimon( g_pDataMng->GetTactics()->GetTactics( m_nSubID1 )->s_Type.m_nType ) )
						{
							cPrintMsg::PrintMsg( 20018 );
							return true;
						}
#ifdef CROSSWARS_SYSTEM
						{

							// 현재 디지몬이 크로스 용병이면 일반 채인지 불가
							DWORD dwMyDigimon = pDUser->GetBaseDigimonFTID();
							if(nsCsFileTable::g_pDigimonMng->IsCrossDigimon(dwMyDigimon))
							{
								return true;
							}
						}
#endif
						// 조그레스 상태인가?
						if(pDUser->IsJointProgressing() || pDUser->IsCurJointProgress())
						{
							cPrintMsg::PrintMsg( 30512 );
							return true;
						}
#ifdef SDM_TAMER_XGUAGE_20180628
						if(pDUser->GetFTDigimon()->isX_AntiJogress())
						{
							cPrintMsg::PrintMsg( 30512 );
							return true;
						}
#endif
						cPrintMsg::PrintMsg( 20017 );
						cMessageBox::GetFirstMessageBox()->SetValue1( m_nSubID1 );
					}
					else	// 2 보낼수없다.
					{
						cPrintMsg::PrintMsg( 10004 );
					}
				}
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	}

	switch( nActive1 )
	{
	case TACTICS_SUMMON:
	case TACTICS_FIRE:
		m_bRenderSelMask = true;
		break;
	}

	return true;
}

void cPopUpWindow::_Make_Tactics()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "POPUP_MERCENARY_MENU" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( TACTICS_NONE );
	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_WHITE;

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_CHANGE_PARTNER" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( TACTICS_SUMMON );
	m_StringList.AddTail( pString );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_DELETE_PARTNER" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( TACTICS_FIRE );
	m_StringList.AddTail( pString );

	TOOLTIPMNG_STPTR->GetTooltip()->ReleaseRenderToolTip( cTooltip::TACTICS );
}

void cPopUpWindow::_Make_DigiviceTactics()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "POPUP_MERCENARY_MENU" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( TACTICS_NONE );
	pString->AddSizeY( 5 );
	m_StringList.AddTail( pString );

	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_WHITE;

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_CHANGE_PARTNER" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( TACTICS_SUMMON );
	m_StringList.AddTail( pString );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_DELETE_PARTNER" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( TACTICS_FIRE );
	m_StringList.AddTail( pString );
}

bool cPopUpWindow::_Update_ForMouse_ChannelList()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			assert_cs( nActive1 != cString::INVALIDE_STRING_VALUE );
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_CHANGE_CHANNEL_POPUP, &nActive1 );		
			ClosePopup();
		}
		break;
	}

	if( nActive1 != cString::INVALIDE_STRING_VALUE )
	{
		m_bRenderSelMask = true;
	}

	return true;
}

void cPopUpWindow::_Make_ChannelList()
{
	CsPoint ptDelta( 0, -1 );

	ChannelContents::CHANNELINFO ChannelData;
	GAME_EVENT_ST.OnEvent(EVENT_CODE::GET_CHANNELINFO, &ChannelData );
	for( int i=0; i < nLimit::Channel; ++i )
	{
		uint nChannelStat = ChannelData.channel[ i ];
		if( nChannelStat == 0xff || nChannelStat == -1)
			continue;

		std::wstring chText;
		DmCS::StringFn::Format( chText, L"%s %d", UISTRING_TEXT( "COMMON_TXT_CHANNEL" ).c_str(), i );

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
		ti.SetText( chText.c_str() );

		cString*  pString = NiNew cString;
		cString::sTEXT* pTextElement = pString->AddText( &ti );
		pTextElement->s_ptSize.x = 80;		

		switch( nChannelStat )
		{		
		case 0:		pString->AddImage( m_pChannelImage1, 0, ptDelta );		break;
		case 1:		pString->AddImage( m_pChannelImage1, 0, ptDelta );		break;
		case 2:		pString->AddImage( m_pChannelImage1, 1, ptDelta );		break;
		case 3:		pString->AddImage( m_pChannelImage1, 1, ptDelta );		break;
		case 4:		pString->AddImage( m_pChannelImage1, 2, ptDelta );		break;
		case 5:		pString->AddImage( m_pChannelImage1, 2, ptDelta );		break;
		case 6:		pString->AddImage( m_pChannelImage1, 3, ptDelta );		break;
		case 7:		pString->AddImage( m_pChannelImage1, 3, ptDelta );		break;
		case 8:		pString->AddImage( m_pChannelImage2, 1, ptDelta );		break;
		case 9:		pString->AddImage( m_pChannelImage2, 1, ptDelta );		break;
		default:	pString->AddImage( m_pChannelImage2, 1, ptDelta );
		}		
#ifdef CHANGE_CHANNEL_LIMIT
		if( nChannelStat >= 20 )	//포화 상태, 해당 채널로 이동 불가
			pString->AddImage( m_pChannelImage2, 0, ptDelta );
#endif

		pString->SetValue1( i );
		pString->AddSizeY( 5 );
		m_StringList.AddTail( pString );		
	}
}

// ----------

void cPopUpWindow::_Make_OtherTamer()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_TAMER_MENU" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( OTAMER_NONE );
	m_StringList.AddTail( pString );

	_AddLine();

	ti.s_eFontSize = CFont::FS_10;

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_TRADE_REQUEST" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 10 );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( OTAMER_TRADE );
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_WHISPER" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 0 );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( OTAMER_WHISPER );
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_REQUEST_PARTY" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 1 );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( OTAMER_PARTY );
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_ADD_TO_FRIEND_LIST" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 2 );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( OTAMER_FRIEND );
	m_StringList.AddTail( pString );	

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_ADD_TO_BLOCK_LIST" ).c_str() );	
	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 3 );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( OTAMER_BLOCK );
	m_StringList.AddTail( pString );
	
#ifdef WEB_INFOMATION
	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_TAMER_INFO" ).c_str() );	
	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 4 );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( OTAMER_RANK );
	m_StringList.AddTail( pString );
#endif
	cCommunityContent::sGUILD_PERSON_INFO myInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILD_MYINFO, &myInfo );
	if( myInfo.s_eRank == cCommunityContent::GR_MASTER || myInfo.s_eRank == cCommunityContent::GR_2NDMASTER )
	{
		pString = NiNew cString;
		ti.s_Color = FONT_WHITE;
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_INVITE_TO_GUILD" ).c_str() );	
		pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 11 );
		pString->TailAddSizeX( 3 );
		pString->AddText( &ti );
		pString->SetValue1( OTAMER_GUILD );
		m_StringList.AddTail( pString );
	}
	pString = NiNew cString;

	// 매크로 프로텍터
	int nInvenIndex = g_pDataMng->GetInven()->GetFirstSlot_Item_TypeLS( ITEM_TYPELS_MACRO_CHECK );	
	ti.s_Color = nInvenIndex != cData_Inven::INVALIDE_INVEN_INDEX ? FONT_WHITE : FONT_RED;
	ti.SetText( UISTRING_TEXT( "POPUP_MACRO_PROTECT_REPORT" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 13 );
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( OTAMER_MACRO );
	m_StringList.AddTail( pString );
}

bool cPopUpWindow::_Update_ForMouse_OtherTamer()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{
	case CURSOR::LBUTTON_UP:
	case CURSOR::RBUTTON_UP:
		{
			switch( nActive1 )
			{
			case OTAMER_NONE:			
				break;

			case OTAMER_PARTY:
				{
					GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_INVITE_FROM_UNIQUE_ID, &m_nSubID1 );
					ClosePopup();
// 					CsC_AvObject* pTamer = g_pMngCollector->GetObject( m_nSubID1 );
// 					if( pTamer )
// 					{
// 						g_pDataMng->TryPartyInvite( pTamer->GetUniqID() );
// 					}
				}				
				break;

			case OTAMER_WHISPER:
				{
					ClosePopup();

					CsC_AvObject* pTamer = g_pMngCollector->GetObject( m_nSubID1 );
					if( pTamer )
					{
						ST_CHAT_PROTOCOL	CProtocol;
						CProtocol.m_Type = NS_CHAT::WHISPER_REPLY;
						CProtocol.m_wStr = pTamer->GetName();
						GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
					}
				}				
				break;
			case OTAMER_TRADE:
				{
					ClosePopup();
					GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_TRADE_REQUEST_TAMEROBJECTIDX, &m_nSubID1 );
				}
				break;
			case OTAMER_FRIEND:
				{
					ClosePopup();
					CsC_AvObject* pTamer = g_pMngCollector->GetObject( m_nSubID1 );
					if( pTamer )
					{
						g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::FRIEND_LIST, 0, pTamer->GetName() );												
					}					
				}
				break;
			case OTAMER_BLOCK:
				{
					ClosePopup();
					CsC_AvObject* pTamer = g_pMngCollector->GetObject( m_nSubID1 );
					if( pTamer )
					{
						g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::SHUTOUT_LIST, 0, pTamer->GetName() );									
					}					
				}					
				break;

#ifdef WEB_INFOMATION
			case OTAMER_RANK:
				{
					ClosePopup();
					CsC_AvObject* pTamer = g_pMngCollector->GetObject( m_nSubID1 );
					if( pTamer )
					{
						if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_WEBWIN ) )
						{
							g_pGameIF->CloseDynamicIF( cBaseWindow::WT_WEBWIN );
						}												
						g_pGameIF->GetDynamicIF( cBaseWindow::WT_WEBWIN );						
						g_pGameIF->GetWebWin()->SetWebControl( cWebWin::eRANK, CsPoint( 50, 200 ), CsPoint( 416, 550 ), pTamer->GetName() );
					}					
				}
				break;
#endif
			case OTAMER_GUILD:
				{
					ClosePopup();
					CsC_AvObject* pTamer = g_pMngCollector->GetObject( m_nSubID1 );
					if( pTamer )
					{
						std::wstring wsTamerName(pTamer->GetName());
						GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GUILD_INVITE, &wsTamerName );
					}
				}
				break;
			case OTAMER_MACRO:
				{
					ClosePopup();
					unsigned int nTargetUIDX = m_nSubID1;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::USING_MACROPROTECT_TARGET_UIDX, &nTargetUIDX );
				}
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	}

	switch( nActive1 )
	{
	case OTAMER_WHISPER:
	case OTAMER_FRIEND:
	case OTAMER_TRADE:
	case OTAMER_BLOCK:
	case OTAMER_PARTY:
#ifdef WEB_INFOMATION
	case OTAMER_RANK:
#endif
	case OTAMER_GUILD:
	case OTAMER_MACRO:
		m_bRenderSelMask = true;		
		break;
	}

	return true;
}

// ----------

void cPopUpWindow::_Make_OtherDigimon()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_DIGIMON_MENU" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( ODIGIMON_NONE );
	m_StringList.AddTail( pString );

	_AddLine();
	
	ti.s_eFontSize = CFont::FS_10;

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_DIGIMON_MENU_SELECT_TAMER" ).c_str() );
// 	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 10 );
// 	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( ODIGIMON_TARGET_TAMER );
	m_StringList.AddTail( pString );
}

bool cPopUpWindow::_Update_ForMouse_OtherDigimon()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{
	case CURSOR::LBUTTON_UP:
	case CURSOR::RBUTTON_UP:
		{
			switch( nActive1 )
			{
			case ODIGIMON_TARGET_TAMER:	
				{
					ClosePopup();
					CsC_AvObject* pObject = g_pMngCollector->GetObject( m_nSubID1 );
					if( pObject )
					{
						if( pObject->GetLeafRTTI() == RTTI_DIGIMON )
						{
							CDigimon* pDigimon = static_cast<CDigimon*>(pObject);
							int nTamerObj = pDigimon->GetTamerLink();
							CsC_AvObject* pTargetObject =g_pCharMng->GetTamer( nTamerObj );
							if( pTargetObject )
								g_pCharResMng->SetTargetMark( pTargetObject );
						}
					}					
				}
				break;
			}
		}break;
	}

	switch( nActive1 )
	{
	case ODIGIMON_TARGET_TAMER:	
		m_bRenderSelMask = true;
		break;		
	}

	return true;
}

// ----------

void cPopUpWindow::_Make_GuildMenu()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 0.0f, 255.0f / 255.0f, 64.0f / 255.0f ) );	

	pString = NiNew cString;
	std::pair<int, cCommunityContent::sGUILD_PERSON_INFO*> guildPersonInfo;
	guildPersonInfo.first = g_pGameIF->GetCommunity()->GetCurSelect();
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILD_PERSON_INFO, &guildPersonInfo );
	cCommunityContent::sGUILD_PERSON_INFO* pInfo = guildPersonInfo.second;
	assert_csm1( (pInfo != NULL), _T( "cur %d" ), g_pGameIF->GetCommunity()->GetCurSelect() );
	ti.SetText( pInfo->s_szName.c_str() );

	pString->AddText( &ti );
	pString->SetValue1( GUILD_NONE );
	m_StringList.AddTail( pString );

	_AddLine();

	if( m_nSubID1 < GUILD_NONE2 )
	{	
		ti.s_Color = POPUP_MENU_COLOR;
		ti.s_eFontSize = CFont::FS_10;		

		pString = NiNew cString;	
		ti.SetText( UISTRING_TEXT( "POPUP_MANAGE_GUILD" ).c_str() );
		pString->AddText( &ti );
		pString->SetValue1( GUILD_NONE );
		m_StringList.AddTail( pString );

		_AddLine();

		ti.s_eFontSize = CFont::FS_9;
		ti.s_Color = FONT_WHITE;

		if( m_nSubID1 == GUILD_AP_MASTER )
		{
			std::wstring wsMsg;

			pString = NiNew cString;

			std::pair<cCommunityContent::eRANK, std::wstring*> guildName;
			guildName.first = cCommunityContent::GR_MASTER;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildName );

			DmCS::StringFn::Format( wsMsg, L"%s %s", (guildName.second)->c_str(), UISTRING_TEXT( "POPUP_PASS_GUILD_MASTER" ).c_str() );
			ti.SetText( wsMsg.c_str() );

			pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 6 );
			pString->TailAddSizeX( 2 );
			pString->AddText( &ti );
			pString->SetValue1( GUILD_AP_MASTER ); 
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			guildName.first = cCommunityContent::GR_2NDMASTER;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildName );

			DmCS::StringFn::Format( wsMsg, L"%s %s", (guildName.second)->c_str(), UISTRING_TEXT( "POPUP_APPOINT_MEMBER" ).c_str() );
			ti.SetText( wsMsg.c_str() );

			pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 7 );
			pString->TailAddSizeX( 2 );
			pString->AddText( &ti );
			pString->SetValue1( GUILD_AP_2MASTER );
			m_StringList.AddTail( pString );
			
			pString = NiNew cString;
			guildName.first = cCommunityContent::GR_DAT;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildName );

			DmCS::StringFn::Format( wsMsg, L"%s %s", (guildName.second)->c_str(), UISTRING_TEXT( "POPUP_APPOINT_MEMBER" ).c_str() );
			ti.SetText( wsMsg.c_str() );

			pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 9 );
			pString->TailAddSizeX( 2 );		
			pString->AddText( &ti );
			pString->SetValue1( GUILD_AP_DAT );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			guildName.first = cCommunityContent::GR_NORMAL;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildName );

			DmCS::StringFn::Format( wsMsg, L"%s %s", (guildName.second)->c_str(), UISTRING_TEXT( "POPUP_APPOINT_MEMBER" ).c_str() );
			ti.SetText( wsMsg.c_str() );

			pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 9 );
			pString->TailAddSizeX( 2 );		
			pString->AddText( &ti );
			pString->SetValue1( GUILD_AP_NORMAL );
			m_StringList.AddTail( pString );

			pString = NiNew cString;
			guildName.first = cCommunityContent::GR_NEW;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildName );

			DmCS::StringFn::Format( wsMsg, L"%s %s", (guildName.second)->c_str(), UISTRING_TEXT( "POPUP_APPOINT_MEMBER" ).c_str() );
			ti.SetText( wsMsg.c_str() );

			pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 9 );
			pString->TailAddSizeX( 2 );		
			pString->AddText( &ti );
			pString->SetValue1( GUILD_AP_NEWBI );
			m_StringList.AddTail( pString );
		}	

		pString = NiNew cString;		
		ti.SetText( UISTRING_TEXT( "POPUP_EXPELLED_FROM_GUILD" ).c_str() );
		pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 5 );
		pString->TailAddSizeX( 2 );
		pString->AddText( &ti );
		pString->SetValue1( GUILD_BAN );
		m_StringList.AddTail( pString );			

		_AddLine();
	}	

	ti.s_Color = POPUP_MENU_COLOR;
	ti.s_eFontSize = CFont::FS_10;	

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_GUILD_MENU" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( GUILD_NONE2 );
	m_StringList.AddTail( pString );

	_AddLine();

	ti.s_eFontSize = CFont::FS_9;

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_WHISPER" ).c_str() );
	pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 0 );
	pString->TailAddSizeX( 2 );
	pString->AddText( &ti );	
	pString->SetValue1( GUILD_WHISPER );
	m_StringList.AddTail( pString );		

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "PARTY_INVITE_TO_PARTY" ).c_str() );		
	pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 1 );
	pString->TailAddSizeX( 2 );
	pString->AddText( &ti );
	pString->SetValue1( GUILD_PARTY );
	m_StringList.AddTail( pString );
	
}

bool cPopUpWindow::_Update_ForMouse_GuildMenu()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			assert_cs( g_pGameIF->GetCommunity() != NULL );
			if( g_pGameIF->GetCommunity() == NULL )
			{
				//DUMPLOG( " C007 - %d, %d ", nActive1, nActive2 );
				ClosePopup();	
				break;
			}
			int nIdx = g_pGameIF->GetCommunity()->GetCurSelect();
			std::pair<int, cCommunityContent::sGUILD_PERSON_INFO*> guildPersonInfo;
			guildPersonInfo.first = g_pGameIF->GetCommunity()->GetCurSelect();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILD_PERSON_INFO, &guildPersonInfo );
			cCommunityContent::sGUILD_PERSON_INFO* pInfo = guildPersonInfo.second;

			switch( nActive1 )
			{
			case GUILD_NONE:
			case GUILD_NONE2:
				break;

			case GUILD_AP_MASTER:
				{
					ClosePopup();					

					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
					{
						std::wstring name = pInfo->s_szName;
						std::pair<cCommunityContent::eRANK, std::wstring*> guildName;
						guildName.first = cCommunityContent::GR_MASTER;
						GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildName );
						cPrintMsg::PrintMsg( 30151, const_cast<TCHAR*>(name.c_str()), (TCHAR*)(guildName.second->c_str()) );
						cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();						
						pMBox->SetCharValue1( const_cast<TCHAR*>(name.c_str()) );
					}														
				}
				break;

			case GUILD_AP_2MASTER:
				{
					ClosePopup();

					cCommunityContent::sGUILD_INFO guildInfo;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
					int n2MasterCount = nsCsFileTable::g_pBaseMng->GetGuildInfo( guildInfo.s_nGuildLevel )->s_nMaxGuild2Master;
					if( guildInfo.s_n2AndMasterNum < n2MasterCount )
					{
						std::wstring name = pInfo->s_szName;
						g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_AP_2MASTER, 0, const_cast<TCHAR*>(name.c_str()) );
					}
					else
					{
						std::pair<cCommunityContent::eRANK, std::wstring*> guildName;
						guildName.first = cCommunityContent::GR_2NDMASTER;
						GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildName );
						cPrintMsg::PrintMsg( 30148, &n2MasterCount, (TCHAR*)(guildName.second->c_str()) );
					}
				}				
				break;

			case GUILD_AP_DAT:
				{
					ClosePopup();		
					std::wstring name = pInfo->s_szName;				
					g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_AP_DAT, 0, const_cast<TCHAR*>(name.c_str()) );
				}
				break;

			case GUILD_AP_NORMAL:
				{
					ClosePopup();					
					std::wstring name = pInfo->s_szName;	
					g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_AP_NORMAL, 0, const_cast<TCHAR*>(name.c_str()) );
				}
				break;

			case GUILD_AP_NEWBI:
				{
					ClosePopup();		
					std::wstring name = pInfo->s_szName;	
					g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GUILD_AP_NEWBI, 0, const_cast<TCHAR*>(name.c_str()) );
				}
				break;

			case GUILD_BAN:
				{
					ClosePopup();	
					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
					{
						std::wstring name = pInfo->s_szName;
						cPrintMsg::PrintMsg( 30145, const_cast<TCHAR*>(name.c_str()) );
						cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();						
						pMBox->SetCharValue1( const_cast<TCHAR*>(name.c_str()) );
					}
				}				
				break;

			case GUILD_WHISPER:
				{
					ClosePopup();	
					if( pInfo->s_szName.compare( g_pCharMng->GetTamerUser()->GetName() ) != 0 )
					{
						if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
						{
							ST_CHAT_PROTOCOL	CProtocol;
							CProtocol.m_Type = NS_CHAT::WHISPER_REPLY;
							CProtocol.m_wStr = pInfo->s_szName;
							GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
						}
					}
				}
				break;

			case GUILD_PARTY:
				{
					std::wstring inviteName = pInfo->s_szName;
					GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_INVITE_FROM_TAMERNAME, &inviteName );
 					ClosePopup();	

// 					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_COMMUNITY ) )
// 					{
// 						g_pDataMng->TryPartyInvite( pInfo->s_szName.c_str() );					
// 					}
				}				
				break;

			default:
				assert_cs( false );
			}
		}
		break;
	}

	switch( nActive1 )
	{
	case GUILD_AP_MASTER:
	case GUILD_AP_2MASTER:
	case GUILD_AP_NORMAL:
	case GUILD_AP_NEWBI:
	case GUILD_AP_DAT:
	case GUILD_BAN:
	case GUILD_PARTY:
	case GUILD_WHISPER:	
		m_bRenderSelMask = true;		
		break;
	}	

	return true;
}

// ----------

void cPopUpWindow::_Make_ExpEventTooltip()
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	{
		std::wstring title = UISTRING_TEXT( "EXP_EVENT_TOOLTIP_TITLE" );
		DmCS::StringFn::ReplaceAll( title, L"#value1#", m_nSubID1 );
		DmCS::StringFn::ReplaceAll( title, L"#value2#", m_nSubID2 );
		g_pStringAnalysis->Cut_Parcing( &m_StringList, 500, title.c_str(), &ti );
	}

	_AddLine();

	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_WHITE;

	{
		std::wstring msg = UISTRING_TEXT( "EXP_EVENT_TOOLTIP_MESSAGE" );
		g_pStringAnalysis->Cut_Parcing( &m_StringList, 500, msg.c_str(), &ti );	
	}
}

void cPopUpWindow::_Make_Emotion()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_EMOTION" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( cString::INVALIDE_STRING_VALUE );
	m_StringList.AddTail( pString );

	_AddLine();

	ti.s_eFontSize = CFont::FS_10;
	ti.s_Color = FONT_WHITE;

	CsEmotion::MAP* pEmotionMap = nsCsFileTable::g_pTamerMng->GetEmotionMap();
	CsEmotion::MAP_IT it = pEmotionMap->begin();
	CsEmotion::MAP_IT itEnd = pEmotionMap->end();
	CsEmotion::sINFO* pInfo = NULL;
	TCHAR sz[ 128 ] = {0, };
	for( ; it!=itEnd; ++it )
	{
		pInfo = it->second->GetInfo();

		_tcscpy_s( sz, 128, pInfo->s_szCmd[ 0 ] );
		if( pInfo->s_nUseCmdCount > 1 )
		{
			_tcscat_s( sz, 128, _T( " [ " ) );
			for( int i=1; i<pInfo->s_nUseCmdCount; ++i )
			{
				_tcscat_s( sz, 128, &pInfo->s_szCmd[ i ][ 1 ] );
				if( i < ( pInfo->s_nUseCmdCount - 1 ) )
				{
					_tcscat_s( sz, 128, _T( ", " ) );
				}
			}
			_tcscat_s( sz, 128, _T( " ]" ) );
		}

		pString = NiNew cString;
		ti.SetText( sz );
		pString->AddText( &ti );
		pString->SetValue1( pInfo->s_nID );
		m_StringList.AddTail( pString );
	}
}

bool cPopUpWindow::_Update_ForMouse_Emotion()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{
	case CURSOR::LBUTTON_DOWN:
		if( nActive1 != cString::INVALIDE_STRING_VALUE )
		{
			ClosePopup();
			ST_CHAT_PROTOCOL	CProtocol;
			CProtocol.m_Type = NS_CHAT::ADD_EMOTION;
			CProtocol.m_value= nActive1;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
		}	
		break;
	case CURSOR::RBUTTON_DOWN:
		if( nActive1 != cString::INVALIDE_STRING_VALUE )
		{
			ClosePopup();
			if( net::game )
			{
				net::game->SendEmoticon( nActive1 );
			}
		}	
		break;
	}

	if( nActive1 != cString::INVALIDE_STRING_VALUE )
	{
		m_bRenderSelMask = true;
	}
	return true;
}

// ----------

void cPopUpWindow::_Make_Friend()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "OPTION_MACRO_MENU_FRIEND_LIST" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( cString::INVALIDE_STRING_VALUE );
	m_StringList.AddTail( pString );

	_AddLine();

	ti.s_eFontSize = CFont::FS_9;

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_WHISPER" ).c_str() );
	pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 0 );
	pString->TailAddSizeX( 2 );
	pString->AddText( &ti );	
	pString->SetValue1( FRIEND_WHISPER );
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "PARTY_INVITE_TO_PARTY" ).c_str() );
	pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 1 );
	pString->TailAddSizeX( 2 );
	pString->AddText( &ti );	
	pString->SetValue1( FRIEND_PARTY );
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_DELETE_FRIEND" ).c_str() );
	pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 5 );
	pString->TailAddSizeX( 2 );
	pString->AddText( &ti );	
	pString->SetValue1( FRIEND_DEL );
	m_StringList.AddTail( pString );	

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_WRITE_MEMO" ).c_str() );
	pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 12 );
	pString->TailAddSizeX( 2 );
	pString->AddText( &ti );	
	pString->SetValue1( FRIEND_MEMO );
	m_StringList.AddTail( pString );
}

bool cPopUpWindow::_Update_ForMouse_Friend()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			int nIdx;
			//현재 선택한 목록의 dataindex를 써준다.
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::GET_CURRENT_LISTINDEX, &nIdx);

			cFriendListContents::FRIENDLIST friendList;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::GET_FRIENDLIST, &friendList);
			cFriendListContents::FITR fItr = friendList.begin();
			for (int i = 0; fItr != friendList.end(); ++fItr, ++i)
			{
				if(i == nIdx)
				{
					break;
				}

			}
			if( fItr == friendList.end() )
				return true;
			 
			switch( nActive1 )
			{
			case FRIEND_WHISPER:
				{
					ClosePopup();
					

					if( fItr->s_szName.compare( g_pCharMng->GetTamerUser()->GetName() ) != 0 )
					{
						if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_FRIEND_LIST ) )
						{
							ST_CHAT_PROTOCOL	CProtocol;
							CProtocol.m_Type = NS_CHAT::WHISPER_REPLY;
							CProtocol.m_wStr = fItr->s_szName;
							GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
						}						
					}
				}				
				break;
			case FRIEND_PARTY:
				{
					ClosePopup();
					GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_INVITE_FROM_TAMERNAME, &fItr->s_szName );
				}
				break;
			case FRIEND_DEL:
				{
					ClosePopup();	

					cPrintMsg::PrintMsg( 30045, const_cast<TCHAR*>( fItr->s_szName.c_str() ) );		
					cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
					pMBox->SetValue1( nIdx );
				}
				break;
			case FRIEND_MEMO:
				{
					ClosePopup();
					
					if( g_pCreateName )
					{
						// 이름 짓는 윈도우 닫아준다.
						cCreateName::DeleteInstance();	
					}
					cCreateName::NewInstance( cBaseWindow::WT_FRIEND_MEMO, 0, CsPoint( -1, -1 ) );
				}
				break;
			default:
				assert_cs( false );
				break;
			}
		}
	}

	switch( nActive1 )
	{	
	case FRIEND_WHISPER:
	case FRIEND_PARTY:
	case FRIEND_DEL:	
	case FRIEND_MEMO:
		m_bRenderSelMask = true;		
		break;
	}	

	return true;
}

// ----------

void cPopUpWindow::_Make_ShutOut()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_BLOCK_LIST" ).c_str() );
	pString->AddText( &ti );
	pString->SetValue1( cString::INVALIDE_STRING_VALUE );
	m_StringList.AddTail( pString );

	_AddLine();

	ti.s_eFontSize = CFont::FS_9;

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_DELETE_FRIEND" ).c_str() );
	pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 5 );
	pString->TailAddSizeX( 2 );
	pString->AddText( &ti );	
	pString->SetValue1( SHUTOUT_DEL	);
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_WRITE_MEMO" ).c_str() );
	pString->AddIcon( CsPoint( 14, 14 ), ICONITEM::Community, 12 );
	pString->TailAddSizeX( 2 );
	pString->AddText( &ti );	
	pString->SetValue1( SHUTOUT_MEMO );
	m_StringList.AddTail( pString );

}

bool cPopUpWindow::_Update_ForMouse_ShutOut()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{		
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			int nIdx;
			//현재 선택한 목록의 dataindex를 써준다.
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::GET_CURRENT_LISTINDEX, &nIdx);
			
			cFriendListContents::SHUTOUTLIST ShutoutList;
			GAME_EVENT_STPTR->OnEvent(EVENT_CODE::GET_SHUTOUTLIST, &ShutoutList);
			cFriendListContents::SITR sItr = ShutoutList.begin();
			for (int i = 0; sItr != ShutoutList.end(); ++sItr, ++i)
			{
				if(i == nIdx)
				{
					break;
				}

			}
			switch( nActive1 )
			{
			case SHUTOUT_DEL:
				{
					ClosePopup();	

					cPrintMsg::PrintMsg( 30046, const_cast<TCHAR*>( sItr->s_szName.c_str() ) );
					cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
					pMBox->SetValue1( nIdx );									
				}
				break;
			case SHUTOUT_MEMO:
				{
					ClosePopup();
					if( g_pCreateName )
					{
						// 이름 짓는 윈도우 닫아준다.
						cCreateName::DeleteInstance();	
					}
					cCreateName::NewInstance( cBaseWindow::WT_SHUTOUT_MEMO, 0, CsPoint( -1, -1 ) );
				}
				break;
			default:
				assert_cs( false );
				break;
			}

		}
	}

	switch( nActive1 )
	{	
	case SHUTOUT_DEL:	
	case SHUTOUT_MEMO:
		m_bRenderSelMask = true;		
		break;
	}	

	return true;
}

// ----------

void cPopUpWindow::_Make_Mainbar_Mission()
{	
	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	
	TCHAR szKey[ MAX_FILENAME ];
	TCHAR sz[ MAX_PATH ];
	
	GLOBALINPUT_ST.GetHotKeyPtr()->GetText( DMKEY::MENU_QUEST_LIST, szKey, MAX_FILENAME );	
	_stprintf_s( sz, MAX_PATH, _T( "%s [%s]" ), UISTRING_TEXT( "POPUP_QUEST" ).c_str(), szKey );

	ti.SetText( sz );
	pString = NiNew cString;			
	pString->AddText( &ti, CsPoint( 4, 4 ) );
	pString->SetValue1( cBaseWindow::WT_QUEST_LIST );

	pString->AddSprite_Head( m_pSprMainBar[ MAINBAR_QUEST ] );

	m_StringList.AddTail( pString );		

		
	GLOBALINPUT_ST.GetHotKeyPtr()->GetText( DMKEY::MENU_ACHIEVE, szKey, MAX_FILENAME );	
	_stprintf_s( sz, MAX_PATH, _T( "%s [%s]" ), UISTRING_TEXT( "COMMON_TXT_ACHIEVEMENT" ).c_str(), szKey );

	ti.SetText( sz );
	pString = NiNew cString;		
	pString->AddText( &ti, CsPoint( 4, 4 ) );
	pString->SetValue1( cBaseWindow::WT_ACHIEVE );

	pString->AddSprite_Head( m_pSprMainBar[ MAINBAR_ACHIEVEMENT ] );

	m_StringList.AddTail( pString );	

#ifdef DAMAGE_METER
	if( m_pSprMainBar[ MAINBAR_BATTLE ] )
	{
		//2017-03-23-nova
		GLOBALINPUT_ST.GetHotKeyPtr()->GetText( DMKEY::MENU_DAMAGEMETER, szKey, MAX_FILENAME );	
		_stprintf_s( sz, MAX_PATH, _T( "%s [%s]" ), UISTRING_TEXT("DAMAGEMETER_TITLE").c_str(), szKey );

		ti.SetText( sz );
		pString = NiNew cString;		
		pString->AddText( &ti, CsPoint( 4, 4 ) );
		pString->SetValue1( cBaseWindow::WT_DAMAGE_METER );

		pString->AddSprite_Head( m_pSprMainBar[ MAINBAR_BATTLE ] );

		m_StringList.AddTail( pString );
	}
#endif
}

bool cPopUpWindow::_Update_ForMouse_Mainbar_Mission()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 12 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{		
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			switch( nActive1 )
			{
			case cBaseWindow::WT_QUEST_LIST:
				{
					ClosePopup();	

					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_QUEST_LIST ) )
						g_pGameIF->CloseDynamicIF( cBaseWindow::WT_QUEST_LIST );
					else
						g_pGameIF->GetDynamicIF( cBaseWindow::WT_QUEST_LIST );
				}
				break;	
			case cBaseWindow::WT_ACHIEVE:
				{
					ClosePopup();

					if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_ACHIEVE ) )
						g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ACHIEVE );
					else
						g_pGameIF->GetDynamicIF( cBaseWindow::WT_ACHIEVE );
				}
				break;

			case cBaseWindow::WT_DAMAGE_METER:	//2016-03-23-nova 전투통계
				{
					ClosePopup();
					GAME_EVENT_ST.OnEvent( EVENT_CODE::TOGGLE_DAMAGEMETER_WINDOW, NULL );
				}
				break;
			default:
				assert_cs( false );
				break;
			}
		}
	}	

	switch( nActive1 )
	{	
	case cBaseWindow::WT_QUEST_LIST:	
	case cBaseWindow::WT_ACHIEVE:
	case cBaseWindow::WT_DAMAGE_METER:	
		m_bRenderSelMask = true;		
		break;
	}

	return true;
}

// ---------- COMMUNITY POPUP MENU

void cPopUpWindow::_Make_Mainbar_Community()	
{
	cString* pString;	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

	TCHAR szKey[ MAX_FILENAME ];
	TCHAR sz[ MAX_PATH ];	

	// ---------- GUILD

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText( DMKEY::MENU_GUILD, szKey, MAX_FILENAME );	
	_stprintf_s( sz, MAX_PATH, _T( "%s [%s]" ), UISTRING_TEXT( "Community" ).c_str(), szKey );	// COMMON_TXT_GUILD

	ti.SetText( sz );
	pString = NiNew cString;		
	pString->AddText( &ti, CsPoint( 4, 4 ) );
	pString->SetValue1( cBaseWindow::WT_COMMUNITY );

	pString->AddSprite_Head( m_pSprMainBar[ MAINBAR_GUILD ] );

	m_StringList.AddTail( pString );
	
	// ---------- FRIEND

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText( DMKEY::MENU_FRIEND_LIST, szKey, MAX_FILENAME );	
	_stprintf_s( sz, MAX_PATH, _T( "%s [%s]" ), UISTRING_TEXT( "Friend" ).c_str(), szKey );	// COMMON_TXT_FRIEND

	ti.SetText( sz );
	pString = NiNew cString;		
	pString->AddText( &ti, CsPoint( 4, 4 ) );
	pString->SetValue1( cBaseWindow::WT_FRIEND_LIST );

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_FRIEND]);

	m_StringList.AddTail( pString );

	// ---------- MAIL

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_MAIL, szKey, MAX_FILENAME);
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Mail Box").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_MAIL);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_MAIL]);

	m_StringList.AddTail(pString);

}

bool cPopUpWindow::_Update_ForMouse_Mainbar_Community()
{
	int nActive1 = cString::INVALIDE_STRING_VALUE;
	int nActive2 = cString::INVALIDE_STRING_VALUE;

	m_ptSelMask = m_ptUpdateMouse + CsPoint(5, 12);

	switch (m_StringList.Update_ForMouse(nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint(m_ptMaxSize.x, 18)))
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
	{
		switch (nActive1)
		{
		case cBaseWindow::WT_COMMUNITY:
		{
			ClosePopup();

			if (!g_pGameIF)
				return true;

			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_COMMUNITY))
			{
				g_pGameIF->CloseDynamicIF(cBaseWindow::WT_COMMUNITY);
			}
			else
			{
				g_pGameIF->GetDynamicIF(cBaseWindow::WT_COMMUNITY);

				cCommunity* pCommunity = g_pGameIF->GetCommunity();
				if (pCommunity)
				{
					pCommunity->SetTab(pCommunity->GetTab());
				}
			}
		}
		break;

		case cBaseWindow::WT_FRIEND_LIST:
		{
			ClosePopup();

			int opentype = 1;

			if (GAME_EVENT_STPTR)
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::TOGGLE_FRIEND_SHUTOUT_UI, &opentype);
		}
		break;

		case cBaseWindow::WT_MAIL:
		{
			ClosePopup();

			if (!g_pGameIF)
				return true;

			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_MAIL))
			{
				g_pGameIF->CloseDynamicIF(cBaseWindow::WT_MAIL);
			}
			else
			{
				g_pGameIF->GetDynamicIF(cBaseWindow::WT_MAIL);

				// Importante:
				// Mail não deve crashar se Community ainda não foi criada.
				cCommunity* pCommunity = g_pGameIF->GetCommunity();
				if (pCommunity)
				{
					pCommunity->SetTab(pCommunity->GetTab());
				}
			}
		}
		break;

		default:
		{
			// Não usar assert aqui.
			// Durante loading/troca de flow pode chegar valor inválido.
			ClosePopup();
			return true;
		}
		}
	}
	break;
	}

	switch (nActive1)
	{
	case cBaseWindow::WT_COMMUNITY:
	case cBaseWindow::WT_FRIEND_LIST:
	case cBaseWindow::WT_MAIL:
		m_bRenderSelMask = true;
		break;
	}

	return true;
}

// ---------- WAREHOUSE POPUP MENU

void cPopUpWindow::_Make_Mainbar_Store()
{
	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

	ti.SetText( UISTRING_TEXT( "POPUP_CASH_WAREHOUSE" ).c_str() );
	pString = NiNew cString;
	pString->AddText( &ti, CsPoint( 4, 4 ) );
	pString->SetValue1( cBaseWindow::WT_NEW_CASH_WAREHOUSE );

	pString->AddSprite_Head( m_pSprMainBar[ MAINBAR_CASHWAREHOUSE ] );

	m_StringList.AddTail( pString );

	// Gift / Reward Storage entry — restored from earlier client builds (the v487 build
	// trimmed this popup down to just Cash Warehouse). The dead WAREHOUSE_TOGGLE event
	// handler in WareHouseContents.cpp:221 was the residue of the original code path.
	// Text-only — the original eMAINBAR enum at Popup.h:131 had a MAINBAR_GIFT sprite
	// slot that no longer exists, and AddSprite_Head with the shared CashWarehouse
	// sprite pointer makes the two entries fight over the single cSprite instance
	// (only one renders). Add a real gift-box icon (e.g. QuickMenu\\Button_giftBox.png)
	// + new sprite slot to restore the icon here.
	ti.SetText( L"Gift / Reward Box" );
	pString = NiNew cString;
	pString->AddText( &ti, CsPoint( 4, 4 ) );
	pString->SetValue1( cBaseWindow::WT_REWARD );
	m_StringList.AddTail( pString );
}

bool cPopUpWindow::_Update_ForMouse_Mainbar_Store()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 12 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{
		case CURSOR::LBUTTON_DOWN:
		case CURSOR::RBUTTON_DOWN:
		{
			switch( nActive1 )
			{
				case cBaseWindow::WT_NEW_CASHSHOP:
					{
						DBG("WT_NEW_CASHSHOP OPEN");
						ClosePopup();
						GAME_EVENT_ST.OnEvent( EVENT_CODE::CASHSHOP_TOGGLE, NULL );
					}
					break;
				case cBaseWindow::WT_NEW_CASH_WAREHOUSE:
				{
					// v487-vintage event-code mismatch: this popup originally fired
					// CASHWAREHOUSE_TOGGLE, but the listener
					// (NewCashshopContents::ToggleCashWarehouse) is bound to
					// CASH_TOGGLE in MakeWorldData. CASHWAREHOUSE_TOGGLE has zero
					// AddEvent registrations anywhere in the v487 tree.
					DBG("WT_NEW_CASH_WAREHOUSE OPEN");
					ClosePopup();
					GAME_EVENT_ST.OnEvent(EVENT_CODE::CASH_TOGGLE, NULL);
				}
				break;
				case cBaseWindow::WT_REWARD:
				{
					// Gift / Reward Storage — fires the same EVENT_CODE::REWARD_ITEM_TOGGLE
					// the Daily Event panel's Receive button uses (handled by
					// ItemRewardContents::Reward_ToggleCheck at ItemRewardContents.cpp:89).
					// Restored entry that v487 trimmed; see _Make_Mainbar_Store header.
					DBG("WT_REWARD OPEN");
					ClosePopup();
					GAME_EVENT_ST.OnEvent(EVENT_CODE::REWARD_ITEM_TOGGLE, NULL);
				}
				break;
				default:
					assert_cs( false );
					break;
			}
		}
	}

	switch( nActive1 )
	{	
		case cBaseWindow::WT_NEW_CASHSHOP:
			m_bRenderSelMask = true;
			break;
		case cBaseWindow::WT_NEW_CASH_WAREHOUSE:
			m_bRenderSelMask = true;
			break;
		case cBaseWindow::WT_REWARD:
			// Hover highlight for the restored Gift / Reward Storage entry.
			m_bRenderSelMask = true;
			break;
	}

	return true;
}

// ---------- INVENTORY POPUP MENU

void cPopUpWindow::_Make_Mainbar_Inventory()
{
	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init(&g_pEngine->m_FontSystem, CFont::FS_10);

	TCHAR szKey[MAX_FILENAME];
	TCHAR sz[MAX_PATH];

	// ---------- INVENTORY

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_INVENTORY_ALL, szKey, MAX_FILENAME);
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Bag").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_INVENTORY);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_INV]);

	m_StringList.AddTail(pString);

	// ---------- EXTRA INVENTORY

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_EXTRA_INVENTORY, szKey, MAX_FILENAME);
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Exclusive Inventory").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_INVENTORY);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_EXCLUSIVEINV]);

	m_StringList.AddTail(pString);
}

bool cPopUpWindow::_Update_ForMouse_Mainbar_Inventory()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint(5, 12);
	switch (m_StringList.Update_ForMouse(nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint(m_ptMaxSize.x, 18)))
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
	{
		switch (nActive1)
		{
		case cBaseWindow::WT_INVENTORY:
		{
			ClosePopup();

			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_INVENTORY, 0))
			{
				GAME_EVENT_ST.OnEvent(EVENT_CODE::CLOSE_INVENTORY, NULL);

				if (g_pDataMng->GetPCbang()->GetPcBang())
					g_pGameIF->CloseDynamicIF(cBaseWindow::WT_PCBANGINVEN);
			}
			else
			{
				bool bSound = true;
				GAME_EVENT_ST.OnEvent(EVENT_CODE::OPEN_INVENTORY, &bSound);
			}
		}
		break;
		//case cBaseWindow::WT_EXTRAINVENTORY:
		//{
			//ClosePopup();
			//int opentype = 1;
			//GAME_EVENT_STPTR->OnEvent(EVENT_CODE::TOGGLE_FRIEND_SHUTOUT_UI, &opentype);
		//}
		//break;
		default:
			assert_cs(false);
			break;
		}
	}
	}

	switch (nActive1)
	{
	case cBaseWindow::WT_INVENTORY:
		m_bRenderSelMask = true;
		break;
	//case cBaseWindow::WT_EXTRAINVENTORY:
		//m_bRenderSelMask = true;
		//break;
	}

	return true;
}

// ---------- DATA POPUP MENU

void cPopUpWindow::_Make_Mainbar_Data()
{
	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init(&g_pEngine->m_FontSystem, CFont::FS_10);

	TCHAR szKey[MAX_FILENAME];
	TCHAR sz[MAX_PATH];

	// ---------- SEAL MASTER

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_CARDINVENTORY, szKey, MAX_FILENAME);
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Seal Master").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_CARDRESEAL);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_SEAL]);

	m_StringList.AddTail(pString);

	// ---------- ENCYCLOPEDIA

	ti.SetText(UISTRING_TEXT("Encyclopedia").c_str());
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_ENCYCLOPEDIA);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_ENCYCLOPEDIA]);

	m_StringList.AddTail(pString);

	// ---------- EVOLUTION HELPS

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_EVOLUTION_HELPS, szKey, MAX_FILENAME);
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Evolution Helps").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_CARDRESEAL);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_EVOLUTIONHELPS]);

	m_StringList.AddTail(pString);

	// ---------- BATTLE STATISTICS

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_DAMAGEMETER, szKey, MAX_FILENAME);
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Battle Statistics").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_DAMAGE_METER);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_BATTLE]);

	m_StringList.AddTail(pString);

}

bool cPopUpWindow::_Update_ForMouse_Mainbar_Data()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint(5, 12);
	switch (m_StringList.Update_ForMouse(nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint(m_ptMaxSize.x, 18)))
	{
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
	{
		switch (nActive1)
		{
		case cBaseWindow::WT_CARDRESEAL:
		{
			ClosePopup();

			int mode = 3;
			GAME_EVENT_ST.OnEvent(EVENT_CODE::SEALMASTER_UI_OPEN_CLOSE, &mode);
		}
		break;
		case cBaseWindow::WT_ENCYCLOPEDIA:
		{
			ClosePopup();
			
			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_ENCYCLOPEDIA))
				g_pGameIF->CloseDynamicIF(cBaseWindow::WT_ENCYCLOPEDIA);
			else
			{
				bool nServerRecv = false;
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::GET_ENCYCLOPEDIA_BISRECV, &nServerRecv);

				if (nServerRecv == false)
				{
					cPrintMsg::PrintMsg(10019);
					net::game->SendEncyclopediaOpen();
				}
				else
					g_pGameIF->GetDynamicIF(cBaseWindow::WT_ENCYCLOPEDIA);
			}
		}
		break;
		//case cBaseWindow::WT_EVOLUTION_HELPS:
		//{
			//ClosePopup();
		//}
		//break;
		case cBaseWindow::WT_DAMAGE_METER:
		{
			ClosePopup();

			GAME_EVENT_ST.OnEvent(EVENT_CODE::TOGGLE_DAMAGEMETER_WINDOW, NULL);
		}
		break;
		default:
			assert_cs(false);
			break;
		}
	}
	}

	switch (nActive1)
	{
		case cBaseWindow::WT_CARDRESEAL:
			m_bRenderSelMask = true;
			break;
		case cBaseWindow::WT_ENCYCLOPEDIA:
			m_bRenderSelMask = true;
			break;
		//case cBaseWindow::WT_EVOLUTION_HELPS:
			//m_bRenderSelMask = true;
			//break;
		case cBaseWindow::WT_DAMAGE_METER:
			m_bRenderSelMask = true;
			break;
	}

	return true;
}

// ---------- CHARACTER POPUP MENU

void cPopUpWindow::_Make_Mainbar_Character()
{
	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init(&g_pEngine->m_FontSystem, CFont::FS_10);

	TCHAR szKey[MAX_FILENAME];
	TCHAR sz[MAX_PATH];

	// ---------- AUTO POTION

	ti.SetText(UISTRING_TEXT("Auto Potion").c_str());
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_NEW_TAMERSTATUS);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_POTION]);

	m_StringList.AddTail(pString);

	// ---------- TAMER

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_TAMER, szKey, MAX_FILENAME);	//	C Key
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Tamer Information").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_NEW_TAMERSTATUS);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_TAMER]);

	m_StringList.AddTail(pString);

	// ---------- DIGIMON

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_DIGIMON_STAT, szKey, MAX_FILENAME);	//	X Key
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Digimon information").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_NEW_DIGIMONSTATUS);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_DIGIMON]);

	m_StringList.AddTail(pString);

	// ---------- ACHIEVEMENT

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_ACHIEVE, szKey, MAX_FILENAME);	//	Y Key
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Achievement").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_ACHIEVE);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_ACHIEVEMENT]);

	m_StringList.AddTail(pString);

	// ---------- QUEST

	GLOBALINPUT_ST.GetHotKeyPtr()->GetText(DMKEY::MENU_QUEST_LIST, szKey, MAX_FILENAME);	//	Y Key
	_stprintf_s(sz, MAX_PATH, _T("%s [%s]"), UISTRING_TEXT("Quest").c_str(), szKey);

	ti.SetText(sz);
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_QUEST_LIST);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_QUEST]);

	m_StringList.AddTail(pString);

	// ---------- EXCHANGE MANAGEMENT

	ti.SetText(UISTRING_TEXT("Exchange Management").c_str());
	pString = NiNew cString;
	pString->AddText(&ti, CsPoint(4, 4));
	pString->SetValue1(cBaseWindow::WT_NEW_TAMERSTATUS);

	pString->AddSprite_Head(m_pSprMainBar[MAINBAR_EXCHANGE]);

	m_StringList.AddTail(pString);

}

bool cPopUpWindow::_Update_ForMouse_Mainbar_Character()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint(5, 12);
	switch (m_StringList.Update_ForMouse(nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint(m_ptMaxSize.x, 18)))
	{
		case CURSOR::LBUTTON_DOWN:
		case CURSOR::RBUTTON_DOWN:
		{
			switch (nActive1)
			{
				/*case cBaseWindow::WT_AUTO_POTION:
				{
					ClosePopup();
					 
					if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_AUTO_POTION))
						g_pGameIF->CloseDynamicIF(cBaseWindow::WT_AUTO_POTION);
					else
						g_pGameIF->GetDynamicIF(cBaseWindow::WT_AUTO_POTION);
				}
				break;*/
				case cBaseWindow::WT_NEW_TAMERSTATUS:
				{
					ClosePopup();

					if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_NEW_TAMERSTATUS))
						g_pGameIF->CloseDynamicIF(cBaseWindow::WT_NEW_TAMERSTATUS);
					else
						g_pGameIF->GetDynamicIF(cBaseWindow::WT_NEW_TAMERSTATUS);
				}
				break;
				case cBaseWindow::WT_NEW_DIGIMONSTATUS:
				{
					ClosePopup();

					if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_NEW_DIGIMONSTATUS))
						g_pGameIF->CloseDynamicIF(cBaseWindow::WT_NEW_DIGIMONSTATUS);
					else
						g_pGameIF->GetDynamicIF(cBaseWindow::WT_NEW_DIGIMONSTATUS);
				}
				break;
				case cBaseWindow::WT_ACHIEVE:
				{
					ClosePopup();

					if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_ACHIEVE))
						g_pGameIF->CloseDynamicIF(cBaseWindow::WT_ACHIEVE);
					else
						g_pGameIF->GetDynamicIF(cBaseWindow::WT_ACHIEVE);
				}
				break;
				case cBaseWindow::WT_QUEST_LIST:
				{
					ClosePopup();

					if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_QUEST_LIST))
						g_pGameIF->CloseDynamicIF(cBaseWindow::WT_QUEST_LIST);
					else
						g_pGameIF->GetDynamicIF(cBaseWindow::WT_QUEST_LIST);
				}
				break;
				/*case cBaseWindow::WT_EXCHANGE:
				{
					ClosePopup();
					 
					if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_EXCHANGE))
						g_pGameIF->CloseDynamicIF(cBaseWindow::WT_EXCHANGE);
					else
						g_pGameIF->GetDynamicIF(cBaseWindow::WT_EXCHANGE);
				}
				break; */
				default:
					assert_cs(false);
					break;
			}
		}
	}

	switch (nActive1)
	{
		//case cBaseWindow::WT_AUTO_POTION:
			//m_bRenderSelMask = true;
			//break;
		case cBaseWindow::WT_NEW_TAMERSTATUS:
			m_bRenderSelMask = true;
			break;
		case cBaseWindow::WT_NEW_DIGIMONSTATUS:
			m_bRenderSelMask = true;
			break;
		case cBaseWindow::WT_ACHIEVE:
			m_bRenderSelMask = true;
			break;
		case cBaseWindow::WT_QUEST_LIST:
			m_bRenderSelMask = true;
			break;
		//case cBaseWindow::WT_EXCHANGE:
			//m_bRenderSelMask = true;
			//break;

	}

	return true;
}

// ---------- SEAL MENU

void cPopUpWindow::_Make_SealPopMenu()
{
	cString* pString;

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );

	pString = NiNew cString;	
	ti.SetText( UISTRING_TEXT( "POPUP_SEAL_MENU" ).c_str() );
	pString->AddText( &ti );
	//pString->SetValue1( SEAL_NONE );		// 제목은 선택되지 않도록 합니다.
	m_StringList.AddTail( pString );

	_AddLine();

	ti.s_eFontSize = CFont::FS_10;

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;

	// 이미 리더로 지정되었을 경우 리더 해제 텍스트를 출력해줍니다.

	if( m_nSubID1 == 1 )		// cCardInventory::LEADER_SELECT
		ti.SetText( UISTRING_TEXT( "POPUP_APPOINT_LEADER" ).c_str() );
	
	else if( m_nSubID1 == 0 )	// cCardInventory::LEADER_NOTSELECT
		ti.SetText( UISTRING_TEXT( "POPUP_DISMISS_LEADER" ).c_str() );

	else if( m_nSubID1 == 2 )	// cCardInventory::LEADER_CHANGE
		ti.SetText( UISTRING_TEXT( "POPUP_CHANGE_LEADER" ).c_str() );

	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 20 );	// 아이콘 변경할 것!!!!
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( SEAL_SETLEADER );
	m_StringList.AddTail( pString );

	pString = NiNew cString;
	ti.s_Color = FONT_WHITE;
	ti.SetText( UISTRING_TEXT( "POPUP_SEAL" ).c_str() );
	pString->AddIcon( CsPoint( 15, 15 ), ICONITEM::Community, 21 );	// 아이콘 변경할 것!!!!
	pString->TailAddSizeX( 3 );
	pString->AddText( &ti );
	pString->SetValue1( SEAL_RESEAL );
	m_StringList.AddTail( pString );
}

bool cPopUpWindow::_Update_ForMouse_SealPopMenu()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 7 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{

	case CURSOR::RBUTTON_DOWN:
		//ClosePopup();
		break;

	case CURSOR::LBUTTON_DOWN:
		{

			assert_cs( nActive1 != cString::INVALIDE_STRING_VALUE );

			switch( nActive1 )
			{
			case SEAL_SETLEADER:		// 리더 설정
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::SEND_SEALMASTER_LEADER, null);
				break;
			case SEAL_RESEAL:			// 밀봉하기
				GAME_EVENT_STPTR->OnEvent(EVENT_CODE::RE_SEALING_UISTART, null);
				break;
			case SEAL_CARDSWAP:			// 카드 이동하기
				break;
			case SEAL_DELETE:			// 카드 버리기
				break;
			}

			ClosePopup();
		}
		break;
	}

	if( nActive1 != cString::INVALIDE_STRING_VALUE )
	{
		m_bRenderSelMask = true;
	}

	return true;
}

// ---------- MEMBERSHIP POPUP MENU

void cPopUpWindow::_Make_VipSystemPopMenu()
{
	cString* pString;	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );

	{
		ti.SetText( UISTRING_TEXT( "POP_VIP_PORTABLE_DIGITAMA_SCANER" ).c_str() );//_LAN( "휴대용 스캐너" )
		pString = NiNew cString;			
		pString->AddText( &ti, CsPoint( 4, 7 ) );
		pString->SetValue1( cBaseWindow::WT_DIGITAMA_SCAN );
		m_StringList.AddTail( pString );
	}

	{
		ti.SetText( UISTRING_TEXT( "POP_VIP_PORTABLE_DIGITAMA_SELL" ).c_str() );//_LAN( "휴대용 회수기" )		
		pString = NiNew cString;		
		pString->AddText( &ti, CsPoint( 4, 7 ) );
		pString->SetValue1( cBaseWindow::WT_DIGITAMA_SELL );
		m_StringList.AddTail( pString );
	}
	
	{
		ti.SetText( UISTRING_TEXT( "POP_VIP_PORTABLE_DIGIMON_TACTICS" ).c_str() );//_LAN( "휴대용 부화기" )	
		pString = NiNew cString;		
		pString->AddText( &ti, CsPoint( 4, 7 ) );
		pString->SetValue1( cBaseWindow::WT_MAKE_TACTICS );
		m_StringList.AddTail( pString );
	}

	{
		ti.SetText( UISTRING_TEXT( "POP_VIP_PORTABLE_DIGIMON_TACTICSHOUSE" ).c_str() );//_LAN( "휴대용 보관소" )	
		pString = NiNew cString;		
		pString->AddText( &ti, CsPoint( 4, 7 ) );
		pString->SetValue1( cBaseWindow::WT_TACTICSHOUSE );
		m_StringList.AddTail( pString );
	}

#ifdef LJW_EQUIPSYSTEM_190724
	{
		ti.SetText( UISTRING_TEXT( "POP_VIP_PORTABLE_EQUIPSYSTEM" ).c_str() );//_LAN( "휴대용 장비 관리" )	
		pString = NiNew cString;		
		pString->AddText( &ti, CsPoint( 4, 7 ) );
		pString->SetValue1( cBaseWindow::WT_EQUIPMENT_SYSTEM );
		m_StringList.AddTail( pString );
	}
#else
	{
		ti.SetText( UISTRING_TEXT( "POP_VIP_PORTABLE_ELEMENTITEM_CHECK" ).c_str() );//_LAN( "휴대용 속성 분석기" )	
		pString = NiNew cString;		
		pString->AddText( &ti, CsPoint( 4, 7 ) );
		pString->SetValue1( cBaseWindow::WT_ELEMENTITEM_CHECK );
		m_StringList.AddTail( pString );
	}

	{
		ti.SetText( UISTRING_TEXT( "POP_VIP_PORTABLE_ELEMENTITEM_ATTACH" ).c_str() );//_LAN( "휴대용 속성 합성기" )	
		pString = NiNew cString;		
		pString->AddText( &ti, CsPoint( 4, 7 ) );
		pString->SetValue1( cBaseWindow::WT_ELEMENTITEM_ATTACH );
		m_StringList.AddTail( pString );
	}
#endif

	{
		ti.SetText( UISTRING_TEXT( "POP_VIP_PORTABLE_JUMPBUSTER" ).c_str() );//_LAN( "점프 부스터" )
		pString = NiNew cString;		
		pString->AddText( &ti, CsPoint( 4, 7 ) );
		pString->SetValue1( cBaseWindow::WT_JUMPBUSTER );
		m_StringList.AddTail( pString );
	}

// 	{
// 		ti.SetText( _LAN( "위탁 상점" ) );
// 		pString = NiNew cString;		
// 		pString->AddText( &ti, CsPoint( 4, 7 ) );
// 		pString->SetValue1( cBaseWindow::WT_JUMPBUSTER );
// 		m_StringList.AddTail( pString );
// 	}
}

bool cPopUpWindow::_Update_ForMouse_VipSystemPopMenu()
{
	int nActive1, nActive2;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 12 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{		
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::ON_VIP_SYSTEM_CONTENTS, &nActive1 );
			ClosePopup();
			break;
		}
	}	

	switch( nActive1 )
	{	
	case cBaseWindow::WT_DIGITAMA_SCAN:	
	case cBaseWindow::WT_DIGITAMA_SELL:
	case cBaseWindow::WT_MAKE_TACTICS:
	case cBaseWindow::WT_TACTICSHOUSE:	
#ifdef LJW_EQUIPSYSTEM_190724
	case cBaseWindow::WT_EQUIPMENT_SYSTEM:
#else
	case cBaseWindow::WT_ELEMENTITEM_CHECK:
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
#endif
	case cBaseWindow::WT_JUMPBUSTER:
		m_bRenderSelMask = true;		
		break;
	}

	return true;
}

// ---------- INVENTORY REORDER ??

void cPopUpWindow::_Make_ItemMake_AssistItem_PopMenu()
{
	SAFE_POINTER_RET( nsCsFileTable::g_pItemMng );
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, POPUP_MENU_SIZE, POPUP_MENU_COLOR );
	pString = NiNew cString;	
	
	ti.SetText( UISTRING_TEXT("POPUP_MENU_ITEMAKE_ASSISTITEM_ORDERLIST").c_str() );//( "사용 가능한 아이템 목록" )
	pString->AddText( &ti );
	m_StringList.AddTail( pString );

	_AddLine();

	LIST_AssistItemList const* pList = nsCsFileTable::g_pItemMng->GetAssistGroupItemList( m_nSubID2 );
	SAFE_POINTER_RET( pList );
	LIST_AssistItemList_CIT it = pList->begin();
	for( ; it != pList->end(); ++it )
	{
		pString = NiNew cString;
		pString->AddIcon( CsPoint( 16, 16 ), ICONITEM::ITEM_ICON, (*it).dwItemCode, 1, CsPoint(0, 4) );
		pString->TailAddSizeX( 3 );		

		CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( (*it).dwItemCode );
		if( pItem && pItem->GetInfo() )
		{
			DWORD dwHaveCount = pInven->GetCount_Item_ID( (*it).dwItemCode );
			cText::sTEXTINFO ti;
			ti.Init( CFont::FS_11 );
			std::wstring wsMsg;
			DmCS::StringFn::Format( wsMsg, L"%s (x%d)", pItem->GetInfo()->s_szName, dwHaveCount );
			ti.SetText( wsMsg.c_str() );
			pString->AddText( &ti, CsPoint(0, 4) );

		}

		pString->SetValue1( m_nSubID1 );
		pString->SetValue2( (*it).dwItemCode );
		m_StringList.AddTail( pString );		
	}
}

bool cPopUpWindow::_Update_ForMouse_AssistItem_PopMenu()
{
	int nActive1 = 0, nActive2 = 0;
	m_ptSelMask = m_ptUpdateMouse + CsPoint( 5, 12 );
	switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMask, POPUP_STRINGLIST_DELTA_Y, CsPoint( m_ptMaxSize.x, 18 ) ) )
	{		
	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
		{
			if( 0 != nActive2 )
			{
				ContentsStream kTmp;
				kTmp << nActive1 << nActive2;
				GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ITEM_PRODUCTION_ASSIST_ITEM_REG, &kTmp );
			}
			ClosePopup();
			break;
		}
	}	

	if( cString::INVALIDE_STRING_VALUE != nActive2 )
		m_bRenderSelMask = true;
	return true;
}