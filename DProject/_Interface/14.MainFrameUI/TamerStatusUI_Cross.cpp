#include "stdafx.h"
#include "TamerStatusUI_Cross.h"

#ifdef CROSSWARS_SYSTEM

#define IF_SKILL_COUNT 12

cTamerStatusUI_Cross::cTamerStatusUI_Cross()
: m_pCrossInven(NULL),
m_pCrossTactics(NULL),
m_pRenderDigimon(NULL),
m_pDigimonRenderTex(NULL),
m_pDigiCrossBtn(NULL),
m_pLevelText(NULL),
m_pNameText(NULL),
m_pChipsetText(NULL),
m_pCrossProgress(NULL)
{
	Construct();
}

cTamerStatusUI_Cross::~cTamerStatusUI_Cross()
{
	if( m_pDigimonRenderTex )
		SAFE_NIDELETE( m_pDigimonRenderTex );
	if( m_pRenderDigimon )
	{
		if( g_pCharMng )
		{
			g_pCharMng->DeleteTempChar( m_pRenderDigimon );
			m_pRenderDigimon = NULL;
		}
	}

	Destroy();
}

bool cTamerStatusUI_Cross::Construct(void)
{
	if( !cTamerStatusUI_SubViewer::Construct() )
		return false;

	GetSystem()->Register( SystemType::eChangeCInvenSize, this );
	GetSystem()->Register( SystemType::eChangeCrossTactics, this );

	return true;
}

void cTamerStatusUI_Cross::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eChangeCInvenSize:
		{
			int nCount = 0;
			kStream >> nCount;
			_ChangeCInvenSize( nCount );
		}
		break;
	case SystemType::eChangeCrossTactics:
		{
			int nIndex = 0;
			kStream >> nIndex;
			_ChangeCrossTactics( nIndex );
		}
		break;
	}
}

void cTamerStatusUI_Cross::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	DeleteResource();
}

void cTamerStatusUI_Cross::DeleteResource()
{
	DeleteScript();
}

void cTamerStatusUI_Cross::Create(cWindow* pkRoot, int nValue /*= 0 */)
{
	InitScript( pkRoot, NULL, CsPoint::ZERO, CsPoint( 406, 676 ), true );

	m_pCrossTactics = NiNew cGridListBox;
	if( m_pCrossTactics )
	{
		m_pCrossTactics->Init( pkRoot, CsPoint( 37, 341 ), CsPoint( 333, 75 ), CsPoint( 11, 9 ), CsPoint( 32, 32 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 8 );
		m_pCrossTactics->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pCrossTactics->SetBackOverAndSelectedImgRender( false );
		m_pCrossTactics->SetAutoSelection( false );
		m_pCrossTactics->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM, this, &cTamerStatusUI_Cross::_OnClickTactics );
		AddChildControl( m_pCrossTactics );
	}
	_MakeCrossTactics();

	m_pCrossInven = NiNew cGridListBox;
	if( m_pCrossInven )
	{
		m_pCrossInven->Init( pkRoot, CsPoint( 37, 547 ), CsPoint( 333, 92 ), CsPoint( 11, 24 ), CsPoint( 32, 32 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 8 );
		m_pCrossInven->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pCrossInven->SetBackOverAndSelectedImgRender( false );
		m_pCrossInven->SetAutoSelection( false );
		m_pCrossInven->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM, this, &cTamerStatusUI_Cross::_OnLCLickCrossInven );
		m_pCrossInven->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT, this, &cTamerStatusUI_Cross::_OnRClickCrossInven );
		AddChildControl( m_pCrossInven );
	}
	_MakeCrossInven();

	{	// 디지몬 레벨
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( L"" );
		m_pLevelText = AddText( &ti, CsPoint( 30, 88 ) );
	}

	{	// 디지몬명
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, NiColor( 76.f/255.f, 242.f/255.f, 1 ) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( L"" );
		m_pNameText = AddText( &ti, CsPoint( 203, 88 ) );
	}

	{	// 크로스워즈 칩셋
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 240.f/255.f, 214.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "TAMERSTATUS_CROSSCHIPSET_TEXT" ).c_str() );
		m_pChipsetText = AddText( &ti, CsPoint( 20, 504 ) );
	}

	{
		// 디지크로스 버튼
		cText::sTEXTINFO textInfo;
		textInfo.Init( CFont::FS_12, NiColor::WHITE);
		textInfo.s_eTextAlign = DT_CENTER;
		textInfo.SetText( UISTRING_TEXT( "TAMERSTATUS_DIGICROSS_BTN" ).c_str() );

		m_pDigiCrossBtn = AddButton( CsPoint( 148, 431 ), CsPoint( 109, 37 ), CsPoint( 0, 37 ), "Scan_New\\Scan_btn_C.tga" );
		if( m_pDigiCrossBtn )
		{
			m_pDigiCrossBtn->SetText( &textInfo );
			m_pDigiCrossBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cTamerStatusUI_Cross::_OnClickDigiCross );
		}
	}

	m_pCrossProgress = NiNew cUI_ProgressBar;
	if( m_pCrossProgress )
	{
		m_pCrossProgress->Init( m_pRoot, CsPoint( 386, 74 ), CsPoint( 7, 402 ), false );
		m_pCrossProgress->SetDirType( cUI_ProgressBar::BOTTOM_TOP );
		m_pCrossProgress->SetViewTextType( cUI_ProgressBar::NONE );
		m_pCrossProgress->SetGuageImage( "TamerStatus_New\\TamerStatus_Codecrown_Gauge.tga" );
		AddChildControl( m_pCrossProgress );
	}

	m_pDigimonRenderTex = NiNew cRenderTex;
	if( m_pDigimonRenderTex )
		m_pDigimonRenderTex->Init( CsPoint( 386, 218 ), "TamerStatus_New\\TamerStatus_Win_04_bg.tga" );
}

void cTamerStatusUI_Cross::Update(float const& fDeltaTime)
{
	cSeparate::UpdateInstance( cBaseWindow::WT_NEW_TAMERSTATUS, 0 );
	cCreateName::UpdateInstance( cBaseWindow::WT_MAKE_CROSSTACTICS, 0 );

	UpdateScript( fDeltaTime );

	if( m_pCrossProgress )
	{
		m_pCrossProgress->SetRange( GetSystem()->GetTamerStat( SystemType::eMaxCR ) );
		m_pCrossProgress->SetProgressPos( GetSystem()->GetTamerStat( SystemType::eCurCR ) );
		CsPoint size = m_pCrossProgress->GetSize();
	}

	if( !m_pRenderDigimon )
		return;

	if( m_pRenderDigimon->IsLoad() )
	{
		CsNodeObj* pCsNode = m_pRenderDigimon->GetCsNode();
		if( pCsNode )
		{
			CsC_AniProp* pAni = m_pRenderDigimon->GetProp_Animation();
			if( pAni )
				pAni->Update( fDeltaTime );
			CsC_AlphaProp* pAlpha = m_pRenderDigimon->GetProp_Alpha();
			if( pAlpha )
				pAlpha->Update( fDeltaTime );

			float fAniTime = m_pRenderDigimon->GetAniTime();
			NiNodePtr niNode = pCsNode->m_pNiNode;
			if( niNode )
				niNode->Update( fAniTime );
		}
	}
	else
	{
		m_pRenderDigimon->CsC_AvObject::Update( fDeltaTime );
		m_pRenderDigimon->SetViewSight( true );
	}
}

BOOL cTamerStatusUI_Cross::Update_ForMouse()
{
	if( cBaseWindow::eMU_TYPE::MUT_OUT_WINDOW != cSeparate::Update_ForMouseInstance( cBaseWindow::WT_NEW_TAMERSTATUS, 0 ) )
		return TRUE;

	if( cBaseWindow::eMU_TYPE::MUT_OUT_WINDOW != cCreateName::Update_ForMouseInstance( cBaseWindow::WT_MAKE_CROSSTACTICS, 0 ) )
		return TRUE;

	if( m_pDigiCrossBtn && m_pDigiCrossBtn->Update_ForMouse() )
		return TRUE;

	if( m_pCrossInven && m_pCrossInven->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return _SetCrossInvenToolTip( m_pCrossInven->GetMouseOverItem() );

	if( m_pCrossTactics && m_pCrossTactics->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return TRUE;

	return FALSE;
}

void cTamerStatusUI_Cross::RenderScript()
{
	if( m_pDigimonRenderTex && IsVisible() )
	{
		m_pRenderDigimon = GetSystem()->GetRenderDigimon();
		if( m_pRenderDigimon )
		{
			m_pDigimonRenderTex->BeginRender();
			m_pDigimonRenderTex->RenderObject( GET_SUBCAMERA(CAMERA_02), m_pRenderDigimon );
			m_pDigimonRenderTex->EndRender( GET_SUBCAMERA(CAMERA_02), GetTotalPosition() + CsPoint( 10, 115 ) );
		}
	}

	cScriptUI::RenderScript();

	if( !IsVisible() )
		return;

	cSeparate::RenderInstance( cBaseWindow::WT_NEW_TAMERSTATUS, 0 );
	cCreateName::RenderInstance( cBaseWindow::WT_MAKE_CROSSTACTICS, 0 );
}

void cTamerStatusUI_Cross::ResetDevice()
{
	ResetDeviceScript();
	if( m_pDigimonRenderTex )
		m_pDigimonRenderTex->ResetDevice();
}

void cTamerStatusUI_Cross::ReleaseSelect()
{

}

void cTamerStatusUI_Cross::UpdateViewer()
{
	int nCount = GetSystem()->GetCrossInvenMaxCount();
	int nCInvenCount = GetSystem()->GetCrossInvenCount();
	for( int i = 0; i < nCount; ++i )
	{
		std::map< int, sCrossInven >::iterator it = m_mapCrossInven.find( i );
		if( it != m_mapCrossInven.end() )
		{
			cItemInfo* pCrossChip = GetSystem()->GetCrossInvenItem( i );
			if( pCrossChip )
				it->second.ChangeCrossItem( pCrossChip->GetType(), pCrossChip->GetCount() );
			else
				it->second.ChangeCrossItem( 0, 1 );
		}
	}

	nCount = GetSystem()->GetCrossTacticsMaxCount();
	int nCrossCount = GetSystem()->GetCrossTacticsCount();
	bool bIsCross = GetSystem()->IsCross();
	for( int i = 0; i < nCount; ++i )
	{
		cData_CrossTactics::sCross* pCrossInfo = GetSystem()->GetCrossTacticsInfo( i );
		SAFE_POINTER_CON( pCrossInfo );

		bool bIsSelected = GetSystem()->IsSelected( i );
		bool bIsConnected = bIsCross ? bIsSelected : false;

		std::map< int, sCrossTactics >::iterator it = m_mapCrossTactics.find( i );
		if( it != m_mapCrossTactics.end() )
		{
			std::string strPath = GetSystem()->GetCrossTacticsFileName( pCrossInfo->s_dwBaseDigimonID );
			it->second.UpdateCrossTactics( strPath, pCrossInfo->s_dwBaseDigimonID, bIsConnected, pCrossInfo->s_nEnable );
			if( !bIsCross )
				it->second.pSelectImg->SetVisible( bIsSelected );
		}
	}

	if( bIsCross )
		m_pDigiCrossBtn->SetText( UISTRING_TEXT( "TAMERSTATUS_DISCONNECT_BTN" ).c_str() );
	else
		m_pDigiCrossBtn->SetText( UISTRING_TEXT( "TAMERSTATUS_DIGICROSS_BTN" ).c_str() );
}


bool cTamerStatusUI_Cross::_OnClickCrossInven(cGridListBoxItem const* pOverItem)
{
	if( !pOverItem )
		return false;

	sCrossInvenInfo* pCInvenInfo = dynamic_cast< sCrossInvenInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pCInvenInfo, false );

	cItemInfo* pCInvenItem = GetSystem()->GetCrossInvenItem( pCInvenInfo->nCrossIndex );
	SAFE_POINTER_RETVAL( pCInvenItem, false );
	if( !pCInvenItem->IsEnable() )
		return false;

	if( ( GLOBALINPUT_ST.IsShift() == false )
		|| ( pCInvenItem->GetCount() == 1 )
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
		|| cData_Inven::IsTradeLimitedItem( pCInvenItem )
#endif
		)
	{
		CURSOR_ST.SetIcon( CURSOR_ICON::CI_CROSS, pCInvenInfo->nCrossIndex, pCInvenItem->GetCount(), NULL );
		cSeparate::DeleteInstance( cBaseWindow::WT_NEW_TAMERSTATUS, 0 );
		return true;
	}
	else
	{
		cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_D_NAME, 0 );
		cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_T_NAME, 0 );					
		cSeparate::NewInstance( cBaseWindow::WT_NEW_TAMERSTATUS, 0, pOverItem->GetWorldPos(), 1, pCInvenItem->GetCount(), NULL, pCInvenInfo->nCrossIndex );
		return false;
	}
	return false;
}

void cTamerStatusUI_Cross::OnLButtonUp(CsPoint pos)
{
}

bool cTamerStatusUI_Cross::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	SAFE_POINTER_RETVAL( m_pCrossInven, false );
	CsPoint ptPos = m_pCrossInven->MousePosToWindowPos( CURSOR_ST.GetPos() );
	cGridListBoxItem const* pOverItem = m_pCrossInven->getItemAtPoint( ptPos );
	if( !pOverItem )
	{
		CURSOR_ST.ReleaseIcon();
		return false;
	}

	sCrossInvenInfo* pCInvenInfo = dynamic_cast< sCrossInvenInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pCInvenInfo, false );

	bool bIsMove = false;
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			GetSystem()->MoveCrossInvenItem( nIconSlot, pCInvenInfo->nCrossIndex, nIconCount );
			bIsMove = true;
		}
		break;
	case CURSOR_ICON::CI_CROSS:
		{
			if( TO_ID( nIconSlot ) == pCInvenInfo->nCrossIndex )
				return false;

			GetSystem()->MoveCrossInvenItem( nIconSlot, pCInvenInfo->nCrossIndex, nIconCount );
			bIsMove = true;
		}
		break;

	case CURSOR_ICON::CI_TEQUIP:
	case CURSOR_ICON::CI_CHIPSET:
		break;
	default:
		cPrintMsg::PrintMsg( 11014 );
		break;
	}

	CURSOR_ST.ReleaseIcon();
	return bIsMove;
}

void cTamerStatusUI_Cross::_MakeCrossInven()
{
	SAFE_POINTER_RET( m_pCrossInven );
	m_pCrossInven->RemoveAllItem();

	int nCount = GetSystem()->GetCrossInvenMaxCount();
	for( int i = 0; i < nCount; ++i )
	{
		_AddCrossItem( i );
	}
}

void cTamerStatusUI_Cross::_AddCrossItem(int nIndex)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	uint nItemType = 0;
	int nItemCount = 0;
	cItemInfo* pCrossChip = GetSystem()->GetCrossInvenItem( nIndex );
	if( pCrossChip )
	{
		nItemType = pCrossChip->GetType();
		nItemCount = pCrossChip->GetCount();
	}

	sCrossInven sCInven;

	int nCrossInvenCount = GetSystem()->GetCrossInvenCount();
	cSprite* pChipSlot = NiNew cSprite;
	if( pChipSlot )
	{
		pChipSlot->Init( NULL, CsPoint::ZERO, CsPoint( 42, 42 ), "TamerStatus_New\\TamerStatus_C_slot.tga", false );
		sCInven.pSlotImg = pControl->AddSprite( pChipSlot, CsPoint( -5, -5 ) );
		if( sCInven.pSlotImg )
		{
			sCInven.pSlotImg->SetAutoPointerDelete( true );
			if( nIndex >= nCrossInvenCount )
			{
				sCInven.pSlotImg->SetVisible( false );
				sCInven.pSlotImg->SetEnable( false );
			}
		}
	}

	sCInven.pCrossIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, nItemType, nItemCount );
	if( sCInven.pCrossIcon )
	{
		sCInven.pCrossIcon->SetAutoPointerDelete( true );
		if( 0 == nItemType )
			sCInven.pCrossIcon->SetEnable( false );
	}

	sCInven.pGridItem = NiNew cGridListBoxItem( nIndex, CsPoint( 32, 32 ) );
	sCInven.pGridItem->SetUserData( new sCrossInvenInfo( nItemType, nItemCount, nIndex ) );
	if( sCInven.pGridItem )
	{
		sCInven.pGridItem->SetItem( pControl );
		if( m_pCrossInven )
			m_pCrossInven->AddItem( sCInven.pGridItem );
	}

	m_mapCrossInven.insert( std::make_pair( nIndex, sCInven ) );
}

void cTamerStatusUI_Cross::_MakeCrossTactics()
{
	SAFE_POINTER_RET( m_pCrossTactics );
	m_pCrossTactics->RemoveAllItem();

	for( int i = 0; i < IF_SKILL_COUNT; ++i )
	{
		_AddCrossTactics( i );
	}
}

void cTamerStatusUI_Cross::_AddCrossTactics(int nIndex)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	DWORD dwTacticsID = 0;
	bool bEnable = false;
	cData_CrossTactics::sCross* pCrossTactics = GetSystem()->GetCrossTacticsInfo( nIndex );
	if( pCrossTactics )
	{
		dwTacticsID = pCrossTactics->s_dwBaseDigimonID;
		bEnable = pCrossTactics->s_nEnable;
	}

	sCrossTactics sTactics;

	// Enable Tactics
	cSprite* pEnable = NiNew cSprite;
	if( pEnable )
	{
		pEnable->Init( NULL, CsPoint::ZERO, CsPoint( 38, 38 ), "TamerStatus_New\\TamerStatus_Select_02.tga", false );
		sTactics.pEnableImg = pControl->AddSprite( pEnable, CsPoint( -3, -3 ) );
		if( sTactics.pEnableImg )
		{
			sTactics.pEnableImg->SetAutoPointerDelete( true );
			sTactics.pEnableImg->SetVisible( bEnable );
		}
	}
	
	cSprite* pTactics = NiNew cSprite;
	if( pTactics )
	{
		std::string strPath = GetSystem()->GetCrossTacticsFileName( dwTacticsID );
		pTactics->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), strPath.c_str(), false, NiColor::WHITE, false );
		sTactics.pTacticsImg = pControl->AddSprite( pTactics );
		if( sTactics.pTacticsImg )
		{
			sTactics.pTacticsImg->SetAutoPointerDelete( true );
			if( strPath.empty() )
				sTactics.pTacticsImg->SetVisible( false );
		}
	}

	// Select Tactics
	cSprite* pSelect = NiNew cSprite;
	if( pSelect )
	{
		pSelect->Init( NULL, CsPoint::ZERO, CsPoint( 38, 38 ), "TamerStatus_New\\TamerStatus_Select.tga", false );
		sTactics.pSelectImg = pControl->AddSprite( pSelect, CsPoint( -3, -3 ) );
		if( sTactics.pSelectImg )
		{
			sTactics.pSelectImg->SetAutoPointerDelete( true );
			sTactics.pSelectImg->SetVisible( false );
		}
	}

	// Connect Tactics
	cSprite* pConnect = NiNew cSprite;
	if( pConnect )
	{
		pConnect->Init( NULL, CsPoint::ZERO, CsPoint( 38, 38 ), "TamerStatus_New\\TamerStatus_Disconnect.tga", false );
		sTactics.pConnectImg = pControl->AddSprite( pConnect, CsPoint( -3, -3 ) );
		if( sTactics.pConnectImg )
		{
			sTactics.pConnectImg->SetAutoPointerDelete( true );
			sTactics.pConnectImg->SetVisible( false );
		}
	}

	sTactics.pGridItem = NiNew cGridListBoxItem( nIndex, CsPoint( 32, 32 ) );
	if( sTactics.pGridItem )
	{
		sTactics.pGridItem->SetUserData( new sCrossTacticsInfo( dwTacticsID, nIndex ) );
		sTactics.pGridItem->SetItem( pControl );
		if( m_pCrossTactics )
			m_pCrossTactics->AddItem( sTactics.pGridItem );
	}

	m_mapCrossTactics.insert( std::make_pair( nIndex, sTactics ) );
}

BOOL cTamerStatusUI_Cross::_SetCrossInvenToolTip(cGridListBoxItem const* pOverItem)
{
	SAFE_POINTER_RETVAL( pOverItem, FALSE );
	sCrossInvenInfo* pCInvenInfo = dynamic_cast< sCrossInvenInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pCInvenInfo, FALSE );

	if( 0 == pCInvenInfo->nCrossItemType )
		return FALSE;

	cItemInfo* pCInvenItem = GetSystem()->GetCrossInvenItem( pCInvenInfo->nCrossIndex );
	SAFE_POINTER_RETVAL( pCInvenItem, FALSE );
	if( !pCInvenItem->IsEnable() )
		return FALSE;

	int nCInvenCount = GetSystem()->GetCrossInvenCount();
	cTooltip::eSLOT eSlot = ( pCInvenInfo->nCrossIndex < nCInvenCount ) ? cTooltip::OPEN_SLOT : cTooltip::CLOSE_SLOT;
	cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RETVAL( pToolTip, FALSE );
	pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pCInvenItem->GetType(), cBaseWindow::WT_NEW_TAMERSTATUS, eSlot, 0, pCInvenItem );

	return TRUE;
}

void cTamerStatusUI_Cross::_UseCrossInvenItem(int nIndex)
{
	GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_SETITEM, &nIndex );

	SAFE_POINTER_RET( g_pGameIF );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_STORE ) ||
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SCAN ) ||
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SELL ) )
	{
		return;
	}

	SAFE_POINTER_RET( g_pNpcMng );
	switch( g_pNpcMng->GetZoomNpcType() )
	{
	case cBaseWindow::WT_BARCODE_CARD_SCRATCH:
	case cBaseWindow::WT_TALK:
	case cBaseWindow::WT_QUEST_REV:
	case cBaseWindow::WT_QUEST_COMP:
	case cBaseWindow::WT_NONE:
	case cBaseWindow::WT_TACTICSHOUSE:
		{
			if( g_pDataMng )
				g_pDataMng->SendItemUse( TO_INVENCROSS_SID( nIndex ) );
		}
		break;
	case cBaseWindow::WT_MAKE_TACTICS:
		{
			if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) )
			{
				cMakeTactics* pMakeTactics = g_pGameIF->GetMakeTactics();
				if( pMakeTactics )
					pMakeTactics->Resist( nIndex );
			}
		}					
		break;
	case cBaseWindow::WT_ELEMENTITEM_CHECK:
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
		break;
	case cBaseWindow::WT_WAREHOUSE:
		{
			if( g_pGameIF && g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE ) )
			{
				cWarehouse* pWareHouse = g_pGameIF->GetWarehouse();
				// 빈슬롯에 현재 아이템의 개수 만큼 넣겠다
				if( pWareHouse )
					pWareHouse->SetResist( nIndex, -1, 0 );
			}
		}
		break;
	case cBaseWindow::WT_TRADE:
		{
			GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_TRADE_ITEM_REGIST, &nIndex );
		}
		break;
	case cBaseWindow::WT_UNIONSTORE:
	case cBaseWindow::WT_NEW_GOTCHA_MACHINE:
	case cBaseWindow::WT_MASTERS_MATCHING:
	case cBaseWindow::WT_ENCHANT_OPTION:
	case cBaseWindow::WT_DIGIMON_INCUBATOR:
		break;

	default:
		assert_cs( false );
	}
}

void cTamerStatusUI_Cross::_ChangeCInvenSize(int nCount)
{
	for( int i = 0; i < nCount; ++i )
	{
		std::map< int, sCrossInven >::iterator it = m_mapCrossInven.find( i );
		if( it != m_mapCrossInven.end() )
		{
			SAFE_POINTER_CON( it->second.pSlotImg );
			it->second.pSlotImg->SetVisible( true );
			it->second.pSlotImg->SetEnable( true );
		}
	}
}

void cTamerStatusUI_Cross::_ChangeCrossTactics(int nIndex)
{
	if( -1 == nIndex )
	{
		if( m_pLevelText )
			m_pLevelText->SetText( L"" );
		if( m_pNameText )
			m_pNameText->SetText( L"" );
		return;
	}

	cData_CrossTactics::sCross* pCrossInfo = GetSystem()->GetCrossTacticsInfo( nIndex );
	SAFE_POINTER_RET( pCrossInfo );
	std::wstring wsLevel;
	DmCS::StringFn::Format( wsLevel, L"%s%d", L"Lv.", pCrossInfo->s_nLevel );
	if( m_pLevelText )
		m_pLevelText->SetText( wsLevel.c_str() );
	if( m_pNameText )
		m_pNameText->SetText( pCrossInfo->s_szName );
}

void cTamerStatusUI_Cross::_OnClickDigiCross(void* pSender, void* pData)
{
	if( m_pRenderDigimon && !m_pRenderDigimon->IsLoad() )
		return;

	GetSystem()->OnClickDigiCross();
}

void cTamerStatusUI_Cross::_OnClickTactics(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem const* pOverItem = static_cast< cGridListBoxItem* >( pData );
	sCrossTacticsInfo* pCrossInfo = dynamic_cast< sCrossTacticsInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pCrossInfo );
	bool bSelect = GetSystem()->SelectCrossTactics( pCrossInfo->nTacticsIndex );
	std::map< int, sCrossTactics >::iterator it = m_mapCrossTactics.find( pCrossInfo->nTacticsIndex );
	if( it != m_mapCrossTactics.end() )
		it->second.pSelectImg->SetVisible( bSelect );
}

void cTamerStatusUI_Cross::_OnLCLickCrossInven(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pOverItem = static_cast< cGridListBoxItem* >( pData );

	sCrossInvenInfo* pCInvenInfo = dynamic_cast< sCrossInvenInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pCInvenInfo );

	if( CURSOR_ST.IsIcon() )
	{
		if( TO_ID( CURSOR_ST.GetIconSlot() ) == pCInvenInfo->nCrossIndex )
			CURSOR_ST.ReleaseIcon();
		return;
	}

	_OnClickCrossInven( pOverItem );
}

void cTamerStatusUI_Cross::_OnRClickCrossInven(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pOverItem = static_cast< cGridListBoxItem* >( pData );
	sCrossInvenInfo* pCInvenInfo = dynamic_cast< sCrossInvenInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pCInvenInfo );
	
	int nCInvenCount = GetSystem()->GetCrossInvenMaxCount();
	if( nCInvenCount <= pCInvenInfo->nCrossIndex )
		return;

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven_Cross* pCInven = g_pDataMng->GetInvenCross();
	SAFE_POINTER_RET( pCInven );
	cItemInfo* pCInvenItem = pCInven->GetData( pCInvenInfo->nCrossIndex );
	SAFE_POINTER_RET( pCInvenItem );
	if( pCInvenItem->IsEnable() )
	{
		if( g_pDataMng->IsItemLock( TO_INVENCROSS_SID( pCInvenInfo->nCrossIndex ) ) )
		{
			cPrintMsg::PrintMsg( 10004 );
			return;
		}

		cSeparate::DeleteInstance( cBaseWindow::WT_NEW_TAMERSTATUS, 0 );

		_UseCrossInvenItem( pCInvenInfo->nCrossIndex );
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void cTamerStatusUI_Cross::sCrossInven::ChangeCrossItem(uint nItemType, int nItemCount)
{
	SAFE_POINTER_RET( pGridItem );
	if( pCrossIcon )
		pCrossIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemType, nItemCount );

	sCrossInvenInfo* pCInvenInfo = dynamic_cast< sCrossInvenInfo* >( pGridItem->GetUserData() );
	if( pCInvenInfo )
	{
		pCInvenInfo->nCrossItemType = nItemType;
		pCInvenInfo->nCrossItemCount = nItemCount;
	}
}

void cTamerStatusUI_Cross::sCrossTactics::UpdateCrossTactics(std::string strPath, DWORD dwBaseID, bool bConnect, bool bEnable)
{
	SAFE_POINTER_RET( pGridItem );
	if( pTacticsImg )
	{
		pTacticsImg->SetVisible( true );
		pTacticsImg->ChangeTexture( strPath.c_str(), false );
		if( strPath.empty() )
			pTacticsImg->SetVisible( false );
	}

	sCrossTacticsInfo* pTacticsInfo = dynamic_cast< sCrossTacticsInfo* >( pGridItem->GetUserData() );
	if( pTacticsInfo )
		pTacticsInfo->dwTacticsID = dwBaseID;

	if( pConnectImg )
		pConnectImg->SetVisible( bConnect );

	if( pEnableImg )
		pEnableImg->SetVisible( bEnable );
}

#endif // CROSSWARS_SYSTEM
