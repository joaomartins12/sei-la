#include "stdafx.h"
#include "RewardEventUI.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

cRewardEventUI::cRewardEventUI():CancelButton(NULL),AcceptButton(NULL),m_EventList(NULL)
,iSelMenu(-1),iSelSubMenu(-1)
{
}

cRewardEventUI::~cRewardEventUI()
{
}

cBaseWindow::eWINDOW_TYPE cRewardEventUI::GetWindowType()
{ 
	return WT_REWARD_EVENT_UI; 
}

void cRewardEventUI::Destroy()
{	
	cBaseWindow::Delete();	
}

void cRewardEventUI::DeleteResource()
{
	m_mapEventMap.clear();
	DeleteScript();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cRewardEventUI::Create(int nValue /* = 0  */)
{		
	if( cBaseWindow::Init() == false )
		return;	

	CsPoint winPos( (g_nScreenWidth >> 1) - (803 >> 1), (g_nScreenHeight>>1) - (615>>1) );
	InitScript( "Event\\event_popup.tga", winPos, CsPoint( 803, 615 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	AddTitle( UISTRING_TEXT( "REWARD_TITLE_TEXT" ).c_str(), CsPoint(0,13), CFont::FS_14 );

	CancelButton = AddButton( CsPoint( 757,  16 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );	
	if( CancelButton )
		CancelButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cRewardEventUI::PressCancelButton);
	
	AcceptButton = AddButton( CsPoint( 440, 558 ), CsPoint(128, 48 ), CsPoint( 0, 48 ), "Event\\event_btn.tga" );	
	if( AcceptButton )
	{
		cText::sTEXTINFO textInfo;
		textInfo.Init(&g_pEngine->m_FontText, CFont::FS_12, NiColor::WHITE);
		textInfo.s_eTextAlign = DT_CENTER;
		textInfo.SetText(UISTRING_TEXT( "REWARD_BTN_ACCEPT").c_str());	
		AcceptButton->SetText(&textInfo);
		AcceptButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cRewardEventUI::PressAcceptButton);
	}

	//왼쪽메뉴
	m_EventList = NiNew cTreeBox;
	if( m_EventList )
	{
		m_EventList->Init( GetRoot(),  CsPoint(27, 61), CsPoint( 190, 440 ), NULL, false );
		m_EventList->AddEvent( cTreeBox::TreeEvent_ItemToggle_Changed,	this, &cRewardEventUI::SelectedCategoty );
		m_EventList->AddEvent( cTreeBox::TreeEvent_Item_MouseUp,		this, &cRewardEventUI::SelectedSubCategory );
		m_EventList->SetOnlyOpenMode( true );
		AddChildControl(m_EventList);
	}

	iSelMenu = 0;
	iSelSubMenu = 0;

	if( InsertEventMap(0) )
		MakeCategory(0,UISTRING_TEXT( "REWARD_MENU_TEXT1" ));//일간접속보상
	if( InsertEventMap(1) )
		MakeCategory(1,UISTRING_TEXT( "REWARD_MENU_TEXT2" ));//월간접속보상

	if( GetSystem()->IsHaveHotTimeEvent() )
	{
		if( InsertEventMap(2) )
			MakeCategory(2,UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT" ));//핫 타임
	}

	if( GetSystem()->IsHaveDailyCheckEvent() )
	{
		if( InsertEventMap( 3 ) )
			MakeCategory( 3, UISTRING_TEXT( "REWARD_MENU_DAILYCHECK_EVENT" ), GetSystem()->GetDailyCheckEventCount() );// 출석부 이벤트
	}

	if( m_EventList )
	{
		m_EventList->SetSelectMode( iSelMenu, iSelSubMenu );
		ShowViewer( iSelMenu, iSelSubMenu );
	}
}

cBaseWindow::eMU_TYPE cRewardEventUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	std::map<int, IEventViewer*>::iterator it =	m_mapEventMap.begin();
	for( ; it != m_mapEventMap.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( it->second->UpdateMouse() )
			return muReturn;
	}

	if(m_EventList && m_EventList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return muReturn;

	if(CancelButton && CancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if(AcceptButton && AcceptButton->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 47 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();				
	return muReturn;
}

void cRewardEventUI::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);	
	_UpdateMoveWindow();
}

void cRewardEventUI::Render()
{
	RenderScript();
	EndRenderScript();
}

void cRewardEventUI::ResetDevice()
{	
	 ResetDeviceScript();
}

void cRewardEventUI::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

bool cRewardEventUI::InsertEventMap( int const& nIdx )
{
	std::map<int, IEventViewer*>::iterator it = m_mapEventMap.find( nIdx );
	if( it != m_mapEventMap.end() )
		return false;// 이미 만들어진 윈도우

	IEventViewer* pEventViewer = NULL;
	switch( nIdx )
	{
	case 0:	pEventViewer = NiNew cRewardDailyUI(); break;
	case 1:	pEventViewer = NiNew cRewardMonthlyUI(); break;
	case 2:	pEventViewer = NiNew cRewardHotTimeUI(); break;
	case 3: pEventViewer = NiNew cRewardDailyCheckUI(); break;
	default:
		return false;
	}

	m_mapEventMap.insert( std::make_pair( nIdx, pEventViewer ) );

	pEventViewer->Create( GetRoot() );
	AddChildScriptUI( pEventViewer );
	pEventViewer->SetVisible(false);	
	return true;
}

void cRewardEventUI::DeleteEventMap( int const& nIdx )
{
	std::map<int, IEventViewer*>::iterator it = m_mapEventMap.find( nIdx );
	if( it != m_mapEventMap.end() )
		return;// 이미 만들어진 윈도우

	SAFE_POINTER_RET( it->second );
	it->second->DeleteScript();
	NISAFE_DELETE( it->second );

	m_mapEventMap.erase( it );
}

void cRewardEventUI::MakeCategory( int const& nIdx, std::wstring const& wsTitleName, int nSubCount /*= 0*/ )
{
	SAFE_POINTER_RET( m_EventList );

	// 메인 카테고리
	cTreeBoxItem* pAddItem = NiNew cTreeBoxItem( nIdx );
	SAFE_POINTER_RET( pAddItem );

	cString * pItem = NiNew cString;
	SAFE_POINTER_RET( pItem );

	cButton* pButton = NiNew cButton;
	if( pButton )
	{
		pButton->Init( NULL, CsPoint::ZERO, CsPoint( 188, 31 ), "Event\\event_tap_btn.tga", false );
		pButton->SetTexToken( CsPoint( 0, 31 ) );
		pButton->SetUserData( new sEventType( nSubCount == 0 ? -1 : 0 ) );
		pItem->AddButton( pButton, 0, CsPoint( 0, 0 ), CsPoint( 188, 31 ) );
	}

	cText::sTEXTINFO tiGroup;
	tiGroup.Init( CFont::FS_11, NiColor::WHITE );
	tiGroup.SetText(wsTitleName.c_str());
	pItem->AddText( &tiGroup, CsPoint(25, 8) );

	pAddItem->SetItem( pItem );
	pAddItem->SetUserData( new sEventType(nIdx) );
	m_EventList->AddItem( pAddItem );

	MakeSubCategory( pAddItem, nSubCount );
}

void cRewardEventUI::MakeSubCategory(cTreeBoxItem* pRoot, int const& nCount)
{
	SAFE_POINTER_RET( pRoot );
	int nMainIndex = pRoot->getID();
	for( int i = 0; i < nCount; ++i )
	{
		// 서브 카테고리
		cTreeBoxItem* pAddItem = NiNew cTreeBoxItem( i );
		SAFE_POINTER_CON( pAddItem );

		cString * pItem = NiNew cString;
		SAFE_POINTER_CON( pItem );

		cButton* pButton = NiNew cButton;
		if( pButton )
		{
			pButton->Init( NULL, CsPoint::ZERO, CsPoint( 180, 31), "Control_G\\ComboBox\\Combo.tga", false );
			pButton->SetTexToken( CsPoint( 0, 26 ) );
			pButton->SetUserData( new sEventType( i + 1 ) );		// 서브 카테고리 인덱스( 1~ )
			pItem->AddButton( pButton, 0, CsPoint( 4, 0 ), CsPoint( 180, 31 ) );
		}

		{
			std::wstring wsSubTitle = GetSystem()->GetSubTitleName( nMainIndex, i );
			cText::sTEXTINFO tiGroup;
			tiGroup.Init( CFont::FS_11, NiColor::WHITE );
			tiGroup.SetText( wsSubTitle.c_str() );
			pItem->AddText( &tiGroup, CsPoint( 25, 8 ) );
		}

		pAddItem->SetItem( pItem );
		pAddItem->SetUserData( new sEventType( nMainIndex ) );
		pRoot->AddChildItem( pAddItem );
	}
}

void cRewardEventUI::DeleteCategory( int const& nIdx )
{
	SAFE_POINTER_RET( m_EventList );
	int nDeletePos = -1;
	int itemSize = (int)m_EventList->GetItemCount();
	for( int n = 0; n < itemSize; ++n )
	{
		cTreeBoxItem const* pListItem = m_EventList->GetItem(n);
		SAFE_POINTER_CON( pListItem );
		sEventType* pUserData = dynamic_cast<sEventType*>(pListItem->GetUserData());
		SAFE_POINTER_CON( pUserData );
		if( nIdx != pUserData->nEventType )
			continue;
		nDeletePos = n;		
	}

	if( -1 != nDeletePos )
		m_EventList->RemoveItem( nDeletePos );
}

void cRewardEventUI::ShowViewer( int const& nViewerType, int const& nViewerSubType )
{
	std::map<int, IEventViewer*>::iterator it = m_mapEventMap.begin();
	for( ; it != m_mapEventMap.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		if( nViewerType == it->first )
		{
			it->second->SetVisible(true);
			it->second->SetSubViewer( nViewerSubType );
		}
		else
			it->second->SetVisible(false);
	}
}

void cRewardEventUI::SelectedCategoty(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cTreeBox* pTree = static_cast< cTreeBox* >( pSender );

	SAFE_POINTER_RET(pData);
	cTreeBoxItem* pItem = static_cast<cTreeBoxItem*>(pData);
	sEventType* pUserData = dynamic_cast<sEventType*>(pItem->GetUserData());
	SAFE_POINTER_RET(pUserData);

	// iSelSubMenu == 버튼 타입
	// -1 : 단일 메인 카테고리 / 0 : 서브가 존재하는 메인 카테고리 / 1~ : 서브 카테고리
	iSelMenu = iSelSubMenu == 0 ? iSelMenu : pUserData->nEventType;
	ShowViewer( iSelMenu, iSelSubMenu - 1 );	// 서브 트리 인덱스 0~

	switch( iSelMenu )
	{
	case 0:	//일간접속보상
	case 1:	//월간접속보상
	case 3:	//출석부
		{
			if( AcceptButton )		
				AcceptButton->SetText( UISTRING_TEXT( "REWARD_BTN_ACCEPT").c_str() );
		}
		break;

	case 2:	//핫타임
		{
			if( AcceptButton )		
				AcceptButton->SetText(UISTRING_TEXT( "REWARD_BTN_OPEN_ITEMREWARD" ).c_str());	
		}
		break;
	}
}

void cRewardEventUI::SelectedSubCategory(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cString::sBUTTON* pChildItem = static_cast< cString::sBUTTON* >( pData );
	cButton* pButton = pChildItem->s_pButton;
	SAFE_POINTER_RET( pButton );
	sEventType* pUserData = dynamic_cast< sEventType* >( pButton->GetUserData() );
	SAFE_POINTER_RET(pUserData);

	iSelSubMenu = pUserData->nEventType;
}

void cRewardEventUI::PressCancelButton( void* pSender, void* pData )
{
	SAFE_POINTER_RET(GetSystem());
	GetSystem()->CloseRewardEventUI();
}

void cRewardEventUI::PressAcceptButton( void* pSender, void* pData )
{
	SAFE_POINTER_RET(GetSystem());
	GetSystem()->AcceptButtonEvent( iSelMenu, iSelSubMenu - 1 );
}

//////////////////////////////////////////////////////////////////////////

bool cRewardEventUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eHotTimeEvent_Update, this );
	GetSystem()->Register( SystemType::eDailyCheckEvent_UpdateInfo, this );
	GetSystem()->Register( SystemType::eDailyCheckEvent_NotTime,	this );

	return true;
}

void cRewardEventUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
	switch( iNotifiedEvt )
	{
	case SystemType::eHotTimeEvent_Update:	
		{
			if( GetSystem()->IsHaveHotTimeEvent() )
			{
				if( InsertEventMap(2) )
					MakeCategory(2,UISTRING_TEXT( "REWARD_MENU_HOTTIME_EVENT" ));//핫 타임
			}
			else
			{
				DeleteEventMap( 2 );
				DeleteCategory( 2 );
			}
		}
		break;

	case SystemType::eDailyCheckEvent_UpdateInfo:	
	case SystemType::eDailyCheckEvent_NotTime:
		{
			if( GetSystem()->IsHaveDailyCheckEvent() )
			{
				if( !InsertEventMap( 3 ) )
					DeleteCategory( 3 );
				MakeCategory( 3, UISTRING_TEXT( "REWARD_MENU_DAILYCHECK_EVENT" ), GetSystem()->GetDailyCheckEventCount() );// 출석부 이벤트
			}
			else
			{
				DeleteEventMap( 3 );
				DeleteCategory( 3 );
			}
		}
		break;
	}
}
