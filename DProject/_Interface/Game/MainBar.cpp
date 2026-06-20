
#include "stdafx.h"
#include "MainBar.h"

cMainBar::cMainBar():m_pMainBarGrid(NULL),m_pMainBarBg(NULL),m_pMainBarSubBg(NULL)
{
}

cMainBar::~cMainBar()
{
}

bool cMainBar::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

#ifdef UI_INVENTORY_RENEWAL
	GetSystem()->Register( SystemType::eNotifyNew, this );
#endif
	return true;
}

void cMainBar::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
#ifdef UI_INVENTORY_RENEWAL
	switch( iNotifiedEvt )
	{
	case SystemType::eNotifyNew:
		{
			bool bIsNew = false;
			kStream >> bIsNew;
			_SetNotifyNew( MAIN_BT_INVEN, bIsNew );
		}
		break;
	}
#endif
}

void cMainBar::Destroy()
{
	cBaseWindow::Delete();	
	m_pmapControls.clear();
}

void cMainBar::DeleteResource()
{
#ifdef UI_INVENTORY_RENEWAL
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
#endif
	DeleteScript();
}

// Create the MainBar Grid
void cMainBar::Create(int nValue /* = 0  */)
{
	if( !cBaseWindow::Init() )
		return;

	InitScript( NULL, CsPoint(0,g_nScreenHeight - 33), CsPoint(g_nScreenWidth, 33), false );
	m_pMainBarBg = AddSprite( CsPoint(0, 33-16), CsPoint(g_nScreenWidth, 16), "MainBar\\Main_Bar.tga" );

	int nGridWidth = MAIN_BT_MAX * (32 + 5);	// 32 is the icon size
	m_pMainBarSubBg = AddSprite( CsPoint( m_ptRootSize.x-nGridWidth - 8, 0 ), CsPoint(nGridWidth + 8, 33), "MainBar\\Main_SubBar.tga" );

	m_pMainBarGrid = NiNew cGridListBox;
	m_pMainBarGrid->Init( GetRoot(), CsPoint( m_ptRootSize.x - nGridWidth - 5, 0 ), CsPoint(nGridWidth, 33), CsPoint(5,0), CsPoint(32, 32), cGridListBox::LowLeftDown, cGridListBox::RightBottom, NULL, false, 0 );
	m_pMainBarGrid->AddEvent( cGridListBox::GRID_CLICKED_ITEM, this, &cMainBar::_MainBarBtnClick );
	AddChildControl(m_pMainBarGrid);
	_CreateMainBarBtn();
}

// Create the MainBar Buttons
void cMainBar::_CreateMainBarBtn()
{
	SAFE_POINTER_RET( m_pMainBarGrid );
	m_pMainBarGrid->RemoveAllItem();
	m_pmapControls.clear();

	for( int n = MAIN_BT_MENU; n < MAIN_BT_MAX; ++n )
	{
		std::string sImgFile;
		std::wstring wsTooltipName;
		int nHotkeyID = -1;
		switch( n )
		{
			// Menu Button
			case MAIN_BT_MENU:
				{
					sImgFile = "QuickMenu\\quickmenu_button_menu.png";
					wsTooltipName=	UISTRING_TEXT("Menu");		// MAINBAR_MENU_OPTION_ESC
				}break;
			// Warehouse Button
			case MAIN_BT_WARE:
				{
					sImgFile = "QuickMenu\\quickmenu_button_box.png";		
					wsTooltipName=	UISTRING_TEXT("Warehouse");			// MAINBAR_MEMU_STORAGE
				}break;
			// Community Button
			case MAIN_BT_COMMUNITY:
				{
					sImgFile = "QuickMenu\\quickmenu_button_community.png";
					wsTooltipName=	UISTRING_TEXT("Community");			// COMMON_TXT_COMMUNITY
				}break;
			// Inventory Button
			case MAIN_BT_INVEN:
				{
					sImgFile = "QuickMenu\\quickmenu_button_inventory.png";
					wsTooltipName = UISTRING_TEXT("Inventory");		// INVENTORY_TITLE_INVENTORY
				}break;
			// Data Button
			case MAIN_BT_DATA:
				{
					sImgFile = "QuickMenu\\quickmenu_button_data.png";
					wsTooltipName=	UISTRING_TEXT("Data");		// INVENTORY_TITLE_INVENTORY
				}break;
			// Character Button
			case MAIN_BT_CHAR:
				{
					sImgFile = "QuickMenu\\quickmenu_button_character.png";
					wsTooltipName =	UISTRING_TEXT("Character");	// MAINBAR_MEMU_ENCYCLOPEDIA
				}break;
			// Event Button
			case MAIN_BT_EVENT:
				{
					sImgFile = "QuickMenu\\quickmenu_button_event.png";
					wsTooltipName =	UISTRING_TEXT("Event");	// MAINBAR_MEMU_SEAL_MASTER
					nHotkeyID = DMKEY::MENU_REWARDSYSTEM;
				}break;
			// No button
			default:
				continue;
		}

		cString * pString = NiNew cString;
		SAFE_POINTER_BEK( pString );

		cButton* pBtn = NiNew cButton;
		if( pBtn )
		{
			pBtn->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), sImgFile.c_str(), false );		
			pBtn->SetTexToken( CsPoint( 0, 56 ) );
			cString::sBUTTON* pAddBtn = pString->AddButton( pBtn, n );
			if( pAddBtn )
				pAddBtn->SetAutoPointerDelete(true);
		}

#ifdef UI_INVENTORY_RENEWAL
		cSprite* pNotifyNew = NiNew cSprite;
		pNotifyNew->Init( NULL, CsPoint::ZERO, CsPoint( 16, 16 ), "NewInventory\\new_notification.tga", false );
		cString::sSPRITE* pSprite = pString->AddSprite( pNotifyNew, CsPoint( 13, -3 ) );
		if( pSprite )
		{
			pSprite->SetVisible( false );
			pSprite->SetAutoPointerDelete( true );
		}
#endif

		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(n, CsPoint( 32, 36 ));
		addItem->SetItem( pString );
		addItem->SetUserData( new sMainBtnData(n, nHotkeyID, wsTooltipName) );
		m_pMainBarGrid->AddItem( addItem );
		m_pmapControls.insert( std::make_pair(n,addItem) );
	}
}

void cMainBar::Update(float const& fDeltaTime)
{
}

cBaseWindow::eMU_TYPE cMainBar::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if( m_pMainBarGrid && m_pMainBarGrid->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cGridListBoxItem const* pOverGrid = m_pMainBarGrid->GetMouseOverItem();
		if(pOverGrid)
		{
			sMainBtnData* pUserData = dynamic_cast<sMainBtnData*>(pOverGrid->GetUserData());
			cString* pString= pOverGrid->GetItem();
			if( pString && pUserData )
				_SetToolTip( pUserData->m_nHotkeyID, pUserData->wsName, pString->GetWorldPos(), CsPoint( 26, 30 ) );
		}
		return muReturn;
	}

	return MUT_OUT_WINDOW;
}

// MainBar Button Click
void cMainBar::_MainBarBtnClick( void* pSender, void* pData )
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pItem = static_cast<cGridListBoxItem*>(pData);
	cString* pString= pItem->GetItem();
	SAFE_POINTER_RET( pString );

	sMainBtnData* pUserData = dynamic_cast<sMainBtnData*>(pItem->GetUserData());
	SAFE_POINTER_RET( pUserData );

	SAFE_POINTER_RET( g_pGameIF );

	// ---------- New MainBar UI 2024
	switch( pUserData->m_nBtnID )
	{
		// Menu Button Click
		case MAIN_BT_MENU:
		{
			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_MAINOPTION))
				g_pGameIF->CloseDynamicIF(cBaseWindow::WT_MAINOPTION);
			else
				g_pGameIF->GetDynamicIF(cBaseWindow::WT_MAINOPTION);
		}
		break;
		// Warehouse Button Click
		case MAIN_BT_WARE:
		{
			cPopUpWindow* pPopupWin = g_pGameIF->GetPopup();
			SAFE_POINTER_RET(pPopupWin);
			pPopupWin->SetPopup(pString->GetWorldPos() + CsPoint(26, 0), CsPoint(120, 0), cPopUpWindow::MAINBAR_STORE);
		}
		break;
		// Community Button Click
		case MAIN_BT_COMMUNITY:
		{
			cPopUpWindow* pPopupWin = g_pGameIF->GetPopup();
			SAFE_POINTER_RET(pPopupWin);
			pPopupWin->SetPopup(pString->GetWorldPos() + CsPoint(26, 0), CsPoint(120, 0), cPopUpWindow::MAINBAR_COMMUNITY);
		}
		break;
		// Inventory Button Click
		case MAIN_BT_INVEN:
		{
			cPopUpWindow* pPopupWin = g_pGameIF->GetPopup();
			SAFE_POINTER_RET(pPopupWin);
			pPopupWin->SetPopup(pString->GetWorldPos() + CsPoint(26, 0), CsPoint(120, 0), cPopUpWindow::MAINBAR_INVENTORY);
		}
		break;
		// Data Button Click
		case MAIN_BT_DATA:
		{
			cPopUpWindow* pPopupWin = g_pGameIF->GetPopup();
			SAFE_POINTER_RET(pPopupWin);
			pPopupWin->SetPopup(pString->GetWorldPos() + CsPoint(26, 0), CsPoint(120, 0), cPopUpWindow::MAINBAR_DATA);
		}
		break;
		// Character Button Click
		case MAIN_BT_CHAR:
		{
			cPopUpWindow* pPopupWin = g_pGameIF->GetPopup();
			SAFE_POINTER_RET(pPopupWin);
			pPopupWin->SetPopup(pString->GetWorldPos() + CsPoint(26, 0), CsPoint(120, 0), cPopUpWindow::MAINBAR_CHARACTER);
		}
		break;
		// Event Button Click
		case MAIN_BT_EVENT:
		{
			if (g_pGameIF->IsActiveWindow(cBaseWindow::WT_REWARD_EVENT_UI, 0))
			{
				GAME_EVENT_ST.OnEvent(EVENT_CODE::REWARD_UI_TOGGLE);
			}
			else
			{
				GAME_EVENT_ST.OnEvent(EVENT_CODE::REWARD_UI_TOGGLE);
			}
		}
		break;
	}
}

void cMainBar::Render()
{
	RenderScript();
}

void cMainBar::ResetDevice()
{
	SetPosSize( CsPoint(0,g_nScreenHeight - 33), CsPoint(g_nScreenWidth, 33) );
	if( m_pMainBarBg )
		m_pMainBarBg->SetSize( CsPoint(g_nScreenWidth, 16) );
	int nGridWidth = MAIN_BT_MAX * (26+5);
	if( m_pMainBarSubBg )
		m_pMainBarSubBg->SetPos( CsPoint( m_ptRootSize.x - nGridWidth - 8, 0 ));
	if( m_pMainBarGrid )
		m_pMainBarGrid->SetPos( CsPoint( m_ptRootSize.x - nGridWidth - 5, 0 ) );
	ResetDeviceScript();
}

void cMainBar::_SetToolTip( int const& hotkeytype, std::wstring const& pText, CsPoint pos, CsPoint patch )
{	
	std::wstring wsTooltipMsg = pText;
	if( -1 != hotkeytype && GLOBALINPUT_ST.GetHotKeyPtr() )
	{
		TCHAR szKey[ MAX_FILENAME ] = {0,};
		GLOBALINPUT_ST.GetHotKeyPtr()->GetText( (DMKEY::eTYPE)hotkeytype, szKey, MAX_FILENAME );
		DmCS::StringFn::Format( wsTooltipMsg, _T( "%s [%s]" ), pText.c_str(), szKey );
	}

	SAFE_POINTER_RET( TOOLTIPMNG_STPTR );
	cTooltip * Tooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( Tooltip );
	Tooltip->SetTooltip_Text( pos + CsPoint(patch.x,-patch.y), patch, wsTooltipMsg.c_str() );
}

void cMainBar::SetButtonEnable( eButtonID ebtID, bool bEnable )
{
	std::map<int,cGridListBoxItem*>::iterator it = m_pmapControls.find( (int)ebtID );
	if( it == m_pmapControls.end() )
		return;
	SAFE_POINTER_RET( it->second );
	it->second->SetEnable( bEnable );	
}

#ifdef UI_INVENTORY_RENEWAL
void cMainBar::_SetNotifyNew(eButtonID ebtID, bool bEnable)
{
	std::map<int,cGridListBoxItem*>::iterator it = m_pmapControls.find( (int)ebtID );
	if( it == m_pmapControls.end() )
		return;
	cGridListBoxItem* pGridItem = it->second;
	SAFE_POINTER_RET( pGridItem );
	cString* pString = pGridItem->GetItem();
	SAFE_POINTER_RET( pString );
	cString::sELEMENT* pElement = pString->FindElement( cString::sELEMENT::SPRITE );
	pElement->SetVisible( bEnable );
}
#endif