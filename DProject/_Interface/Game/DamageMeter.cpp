#include "stdafx.h"

#include "DamageMeter.h"

cDamageMeter::cDamageMeter():m_pCloseBtn(NULL),m_pNextBtn(NULL),m_pPrevBtn(NULL),m_pListBox(NULL)
,m_ViewTypeString(NULL),m_pResetBtn(NULL)/*,m_szAvDPS(NULL)*/
{
}

cDamageMeter::~cDamageMeter()
{
}

cBaseWindow::eWINDOW_TYPE cDamageMeter::GetWindowType()
{ 
	return WT_DAMAGE_METER; 
}

void cDamageMeter::Destroy()
{
	cBaseWindow::Delete();	
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cDamageMeter::DeleteResource()
{
	DeleteScript();
}

// void cDamageMeter::ResetMap()
// { 
// 	Close( false ); 
// }

bool cDamageMeter::OnEscapeKey()
{ 
	_CloseWindow( NULL, NULL ); 
	return true; 
}

// void cDamageMeter::UserDie()
// { 
// 	_CloseWindow( NULL, NULL ); 
// }

void cDamageMeter::Create( int nValue /*= 0 */ )
{
	if( !cBaseWindow::Init() )
		return;

	int nPosX = g_nScreenWidth - 301;
	int nPosY = ( g_nScreenHeight ) - ( 156 * 2 );
	InitScript( "Ranking\\Battle_Information.bmp", CsPoint( nPosX, nPosY ) , CsPoint( 301, 156 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );

	AddTitle( UISTRING_TEXT("DAMAGEMETER_TITLE").c_str() );
	
	m_pCloseBtn = AddButton( CsPoint( 277, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDamageMeter::_CloseWindow );

	m_pResetBtn = AddButton( CsPoint( 100, 129 ), CsPoint( 100, 22 ), cButton::IMAGE_NOR_12, UISTRING_TEXT("DAMAGEMETER_DATA_RESET").c_str() );
	if( m_pResetBtn )
		m_pResetBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDamageMeter::_DataReset );

	m_pPrevBtn = AddButton( CsPoint( 25, 124 ), CsPoint( 30, 30 ), CsPoint( 0, 30 ), "Ranking\\Target_Arrow_L.tga" );
	if( m_pPrevBtn )
		m_pPrevBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDamageMeter::_PrevVeiwType );

	m_pNextBtn = AddButton( CsPoint( 246, 124 ), CsPoint( 30, 30/*16, 26*/ ), CsPoint( 0, 30/*0, 32*/ ), "Ranking\\Target_Arrow_R.tga" );
	if( m_pNextBtn )
		m_pNextBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cDamageMeter::_NextVeiwType );

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("DAMAGEMETER_RANK").c_str() );
		AddText( &ti, CsPoint( 26, 42 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("DAMAGEMETER_TARGETNAME").c_str() );
		AddText( &ti, CsPoint( 87/*112*/, 42 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.s_eTextAlign = DT_CENTER;
		m_ViewTypeString = AddText( &ti, CsPoint( 213, 42 ) );		
	}

// 	{
// 		cText::sTEXTINFO ti;
// 		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
// 		ti.s_eTextAlign = DT_CENTER;
// 		ti.SetText( _LAN("파티 평균 초당 피해량") );
// 		m_szAvDPS = AddText( &ti, CsPoint( 151, 134 ) );
// 		m_szAvDPS->SetVisible(false);
// 	}

	m_pListBox = NiNew cListBox;
	if( m_pListBox )
	{
		m_pListBox->Init( m_pRoot, CsPoint(11,55), CsPoint( 280, 70 ), NULL, false );		
		m_pListBox->SetItemGab(CsPoint(0,0));
		m_pListBox->SetItemHeight( 16 );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if( pScrollBar )
		{
			pScrollBar->Init( cScrollBar::TYPE_ENCY, NULL, CsPoint( 0, 0 ), CsPoint( 16, 150 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint(70,50), CsPoint( 216, 180 ) ), 5, false );
			pScrollBar->SetEnableRenderFromEnableScroll(true);
			pScrollBar->ChangeGBImg( "Item_Make\\Make_WheelBar_Bag_Mid.tga", false );
			m_pListBox->SetScrollBar( pScrollBar );
		}
		AddChildControl( m_pListBox );
	}

	_SetViewType( GetSystem()->GetCurrentViewType() );
}

void cDamageMeter::Update(float const& fDeltaTime)
{
	//윈도우 이동 업데이트
	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cDamageMeter::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if( m_pResetBtn && m_pResetBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pCloseBtn && m_pCloseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pNextBtn && m_pNextBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if( m_pPrevBtn && m_pPrevBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	if(m_pListBox && m_pListBox->Update_ForMouse(CURSOR_ST.GetPos()) )
		return muReturn;


	// 윈도우 이동
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), (CsSIZE)m_pRoot->GetSize() ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cDamageMeter::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();
}

void cDamageMeter::Render()
{
	RenderScript();
}

void cDamageMeter::ResetDevice()
{
	ResetDeviceScript();
}

void cDamageMeter::_CloseWindow(void* pSender, void* pData)
{
	GetSystem()->CloseMeterWindow();
}

void cDamageMeter::_DataReset(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->ClearDamageMeterData();
}

void cDamageMeter::_PrevVeiwType(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->ChangePrevViewType();
}

void cDamageMeter::_NextVeiwType(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->ChangeNextViewType();
}

void cDamageMeter::_SetViewType( int const& nType )
{
	std::wstring wsViewTypeString;
	switch( nType )
	{
	case DamageMeterContents::DAMAGE_DONE:	wsViewTypeString = UISTRING_TEXT("DAMAGEMETER_DAMAGE_DONE");	break;
	case DamageMeterContents::DAMAGE_DPS:	wsViewTypeString = UISTRING_TEXT("DAMAGEMETER_DAMAGE_DPS");		break;
	case DamageMeterContents::DAMAGE_TAKEN:	wsViewTypeString = UISTRING_TEXT("DAMAGEMETER_DAMAGE_TAKEN");	break;
	}		

	if( m_ViewTypeString )	
		m_ViewTypeString->SetText( wsViewTypeString.c_str() );

	_UpdateInfo(nType);
}

void cDamageMeter::_ResetData()
{
	m_mapControls.clear();
	SAFE_POINTER_RET( m_pListBox );
	m_pListBox->RemoveAllItem();
}

bool cDamageMeter::_makeEmptyItem(int const& nMakeCount )
{
	SAFE_POINTER_RETVAL( m_pListBox, false );

	int nCount = m_pListBox->GetItemCount();
	for( int n = 0; n < nMakeCount; ++n )
	{
		cString * pItem = NiNew cString;
		SAFE_POINTER_RETVAL( pItem, false );
		sControls controls;
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
			ti.SetText( nCount+n+1 );
			pItem->AddText( &ti, CsPoint(5,4) );
		}
		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
			controls.pName = pItem->AddText( &ti, CsPoint(34,4) );
		}

		{
			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
			controls.pValue = pItem->AddText( &ti, CsPoint(137,4) );
		}

		cListBoxItem * addItem  = NiNew cListBoxItem;
		addItem->SetItem( pItem );
		m_pListBox->AddItem( addItem );
		m_mapControls.insert( std::make_pair( nCount+n, controls ) );
	}	
	return true;
}

cDamageMeter::sControls* cDamageMeter::_GetControls(int const& nIdx)
{
	std::map<int,sControls>::iterator it = m_mapControls.find( nIdx );
	if( it == m_mapControls.end() )
		return NULL;

	return &it->second;
}

void cDamageMeter::_ListBoxUpdate( int const& nShowItemCount )
{
	int nRankListCount = nShowItemCount;
	int nItemCount = m_pListBox->GetItemCount();
	if( nRankListCount > nItemCount )
	{
		if( !_makeEmptyItem( nRankListCount - nItemCount ) )
			return;

		nItemCount = m_pListBox->GetItemCount();
	}

	std::map<int, bool> views;
	for( int n = 0; n < nItemCount; ++n )
	{
		if( n < nRankListCount )
			views.insert( make_pair( n, true ) );
		else
			views.insert( make_pair( n, false ) );
	}

	if( views.empty() )
		m_pListBox->SetVisibleItemAll( false );
	else
		m_pListBox->SetVisibleItems( views );
}

void cDamageMeter::_UpdateInfo(int const& nType)
{
	SAFE_POINTER_RET( m_pListBox );

	DamageMeterContents::LIST_RANKDATA const& rankData = GetSystem()->GetViewData(nType);
	
	int listSize = (int)rankData.size();
	_ListBoxUpdate( listSize );

	DamageMeterContents::LIST_RANKDATA_CIT it = rankData.begin();
	for( int n = 0; it != rankData.end(); ++it, ++n )
	{
		sControls* pCon = _GetControls( n );
		SAFE_POINTER_CON( pCon );

		if( pCon->pName )
		{
			pCon->pName->SetText( (*it)->GetName().c_str() );
			if( (*it)->IsMe() )
				pCon->pName->SetColor( NiColor(1.0f, 1.0f, 0.0f) );
			else
				pCon->pName->SetColor( NiColor::WHITE );
		}

		if( pCon->pValue )
		{
			std::wstring value = DmCS::StringFn::getNumberFormatW( (*it)->GetValue() );		
			pCon->pValue->SetText( value.c_str() );
			if( (*it)->IsMe() )
				pCon->pValue->SetColor( NiColor(1.0f, 1.0f, 0.0f) );
			else
				pCon->pValue->SetColor( NiColor::WHITE );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool cDamageMeter::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpdate_View_Type, this );
	GetSystem()->Register( SystemType::eClear_Damage_Data, this );
	GetSystem()->Register( SystemType::eChange_ViewType, this );
	return true;
}

void cDamageMeter::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eClear_Damage_Data:	_ResetData();	break;
	case SystemType::eChange_ViewType:		
		{
			int nViewType;
			kStream >> nViewType;
			_SetViewType(nViewType);
		}
		break;
	case SystemType::eUpdate_View_Type:	
		{
			int nViewType;
			kStream >> nViewType;
			_UpdateInfo(nViewType);	
		}break;
	}
}