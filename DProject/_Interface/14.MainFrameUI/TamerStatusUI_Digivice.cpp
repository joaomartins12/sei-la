#include "stdafx.h"
#include "TamerStatusUI_Digivice.h"


#define IF_DIGIMON_SLOT_COUNT 8
#define	IF_CHIPSET_SLOT_COUNT 8
#define IF_DIGIMON_SLOT_TYPE_OPEN 0
#define IF_DIGIMON_SLOT_TYPE_LOCK 1
#define IF_DIGIMON_SLOT_TYPE_DISABLE 2

cTamerStatusUI_Digivice::cTamerStatusUI_Digivice()
: m_pChipset(NULL), m_pLTactics(NULL), m_pRTactics(NULL)
{
	Construct();
}

cTamerStatusUI_Digivice::~cTamerStatusUI_Digivice()
{
	Destroy();
}

bool cTamerStatusUI_Digivice::Construct(void)
{
	if( !cTamerStatusUI_SubViewer::Construct() )
		return false;
	return true;
}

void cTamerStatusUI_Digivice::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{

}

void cTamerStatusUI_Digivice::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	DeleteResource();
}

void cTamerStatusUI_Digivice::DeleteResource()
{
	DeleteScript();
}

void cTamerStatusUI_Digivice::Create(cWindow* pkRoot, int nValue /*= 0 */)
{
	InitScript( pkRoot, NULL, CsPoint::ZERO, CsPoint( 406, 676 ), true );

	{	// 칩셋
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_10, NiColor( 240.f/255.f, 214.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( UISTRING_TEXT( "TAMERSTATUS_CHIPSET_TEXT" ).c_str() );
		m_pChipsetText = AddText( &ti, CsPoint( 20, 504 ) );
	}

	m_pChipset = NiNew cGridListBox;
	if( m_pChipset )
	{
		m_pChipset->Init( GetRoot(), CsPoint( 37, 547 ), CsPoint( 332, 92 ), CsPoint( 68, 18 ), CsPoint( 32, 32 ), cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 4 );
		m_pChipset->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pChipset->SetBackOverAndSelectedImgRender( false );
		m_pChipset->SetAutoSelection( false );
		m_pChipset->AddEvent( cGridListBox::GRID_SELECTED_ITEM_RIGHT,	this, &cTamerStatusUI_Digivice::_OnRClickChipset );
		m_pChipset->AddEvent( cGridListBox::GRID_CHANGED_MOUSEON_ITEM,	this, &cTamerStatusUI_Digivice::_OnMouseOverChipset );
		m_pChipset->AddEvent( cGridListBox::GRID_MOUSE_DOWN_ON_ITEM,	this, &cTamerStatusUI_Digivice::_OnLClickChipset );
		AddChildControl( m_pChipset );
	}
	_MakeChipsetGrid();

	m_pLTactics = NiNew cListBox;
	if( m_pLTactics )
	{
		m_pLTactics->Init( GetRoot(), CsPoint( 4, 75 ), CsPoint( 195, 440 ), false );
		m_pLTactics->SetItemGab( CsPoint( 0, 40 ) );
		m_pLTactics->AddEvent( cListBox::LIST_SELECTED_ITEM, this, &cTamerStatusUI_Digivice::_OnClickTactics );
		m_pLTactics->AddEvent( cListBox::LIST_ITEM_RIGHT_UP, this, &cTamerStatusUI_Digivice::_OnClickTactics );
		AddChildControl( m_pLTactics );
	}
	m_pRTactics = NiNew cListBox;
	if( m_pRTactics )
	{
		m_pRTactics->Init( GetRoot(), CsPoint( 205, 75 ), CsPoint( 195, 440 ), false );
		m_pRTactics->SetItemGab( CsPoint( 0, 40 ) );
		m_pRTactics->AddEvent( cListBox::LIST_SELECTED_ITEM, this, &cTamerStatusUI_Digivice::_OnClickTactics );
		m_pRTactics->AddEvent( cListBox::LIST_ITEM_RIGHT_UP, this, &cTamerStatusUI_Digivice::_OnClickTactics );
		AddChildControl( m_pRTactics );
	}
	_MakeTacticsList();
	m_pLTactics->changeAllItemHeight( 80 );
	m_pRTactics->changeAllItemHeight( 80 );
}

void cTamerStatusUI_Digivice::Update(float const& fDeltaTime)
{

}

BOOL cTamerStatusUI_Digivice::Update_ForMouse()
{
	if( m_pChipset && m_pChipset->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return TRUE;

	if( m_pLTactics && m_pLTactics->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return _SetTacticsTooltip( m_pLTactics->GetMouseOverItem() );

	if( m_pRTactics && m_pRTactics->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return _SetTacticsTooltip( m_pRTactics->GetMouseOverItem() );
	return FALSE;
}

void cTamerStatusUI_Digivice::RenderScript()
{
	cScriptUI::RenderScript();

	if( !IsVisible() )
		return;
}

void cTamerStatusUI_Digivice::ResetDevice()
{
	ResetDeviceScript();
}

void cTamerStatusUI_Digivice::ReleaseSelect()
{

}

void cTamerStatusUI_Digivice::UpdateViewer()
{
	bool bIsDigivice = GetSystem()->IsEquipDigivice();
	int nCount = GetSystem()->GetOpenedChipsetSlot();
	for( int i = 0; i < IF_CHIPSET_SLOT_COUNT; ++i )
	{
		std::map< int, sChipset >::iterator it = m_mapChipset.find( i );
		if( it != m_mapChipset.end() )
		{
			cItemInfo * pChipset = GetSystem()->GetChipsetItem( i );
			if( !pChipset )
				it->second.ChangeItem( 0 );
			else
				it->second.ChangeItem( pChipset->GetType(), pChipset->GetCount() );

			if( nCount > i )
				it->second.SetChipsetBg( bIsDigivice );
		}
	}

	nCount = GetSystem()->GetTacticsOpenCount();
	for( int i = 0; i < nCount; ++i )
	{
		std::map< int, sTactics >::iterator it = m_mapTactics.find( i );
		if( it != m_mapTactics.end() )
		{
			int nLevel = GetSystem()->GetTacticsLevel( i );
			int nHatchLv = GetSystem()->GetTacticsHatchLv( i );
			std::string strPath = GetSystem()->GetTacticsFileName( i );
			it->second.UpdateDigimon( i, nLevel, true, strPath, nHatchLv );
		}
	}
}

void cTamerStatusUI_Digivice::OnLButtonUp(CsPoint pos)
{
}

bool cTamerStatusUI_Digivice::CursorIconLBtnUp(CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount)
{
	SAFE_POINTER_RETVAL( m_pChipset, false );
	CsPoint ptPos = m_pChipset->MousePosToWindowPos( CURSOR_ST.GetPos() );
	cGridListBoxItem const* pOverItem = m_pChipset->getItemAtPoint( ptPos );
	if( !pOverItem )
	{
		CURSOR_ST.ReleaseIcon();
		return false;
	}

	sChipsetInfo* pChipsetInfo = dynamic_cast< sChipsetInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pChipsetInfo, false );

	bool bIsMove = false;
	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			GetSystem()->MoveChipsetItem( nIconSlot, pChipsetInfo->nChipsetIndex, false, false );
			bIsMove = true;
		}
		break;

	case CURSOR_ICON::CI_CHIPSET:
		{
			if( TO_ID( nIconSlot ) == pChipsetInfo->nChipsetIndex )
				return false;

			GetSystem()->MoveChipsetItem( nIconSlot, pChipsetInfo->nChipsetIndex, false, true );
			bIsMove = true;
		}
		break;

	case CURSOR_ICON::CI_WAREHOUSE:
		cPrintMsg::PrintMsg( 11018 );
		break;

	default:
		cPrintMsg::PrintMsg( 11014 );
		break;
	}
	CURSOR_ST.ReleaseIcon();
	return bIsMove;
}

BOOL cTamerStatusUI_Digivice::_SetTacticsTooltip(cListBoxItem const* pOverItem)
{
	SAFE_POINTER_RETVAL( pOverItem, false );

	sTacticsInfo* pTacticsInfo = dynamic_cast< sTacticsInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RETVAL( pTacticsInfo, FALSE );
	
	std::map< int, sTactics >::iterator it = m_mapTactics.find( pTacticsInfo->nTacticsIndex );
	if( it == m_mapTactics.end() )
		return FALSE;

	cString::sSPRITE* pSprite = it->second.pDigimonImg;
	SAFE_POINTER_RETVAL( pSprite, FALSE );
	CsRect rc( pSprite->s_RenderPos, CsSIZE( pSprite->s_ptSize ) );
	if( rc.PtInRect( CURSOR_ST.GetPos() ) )
	{
		cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL( pTooltip, FALSE );

		// 오픈 가능한 슬롯이 아니면
		int nType = _GetSlotType( pTacticsInfo->nTacticsIndex );
		if( IF_DIGIMON_SLOT_TYPE_DISABLE == nType )
			return FALSE;

		uint nTacticsType = GetSystem()->GetTacticsType( pTacticsInfo->nTacticsIndex );
		if( 0 == nTacticsType )
		{
#ifdef SIMPLE_TOOLTIP
			pTooltip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 16, 0 ), 100, cTooltip::UIHELP, 1042 );
			return TRUE;
#else
			return FALSE;
#endif
		}

		pTooltip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 16, 0 ), 140, cTooltip::TACTICS, pTacticsInfo->nTacticsIndex, nTacticsType );

		return TRUE;
	}

	return FALSE;
}

void cTamerStatusUI_Digivice::_MakeChipsetGrid()
{
	SAFE_POINTER_RET( m_pChipset );
	m_pChipset->RemoveAllItem();

	cItemInfo* pChipset;
	for( int i = 0; i < IF_CHIPSET_SLOT_COUNT; ++i )
	{
		pChipset = GetSystem()->GetChipsetItem( i );
		if( pChipset )
			_AddChipsetGridItem( i, pChipset->GetType(), pChipset->GetSkillRate() );
		else
			_AddChipsetGridItem( i, 0, 0 );
	}
}

void cTamerStatusUI_Digivice::_AddChipsetGridItem(int nIndex, uint nItemType, int nItemRate, int nCount /*= 1*/)
{
	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sChipset sChip;
	cSprite* pSlotBg = NiNew cSprite;
	if( pSlotBg )
	{
		pSlotBg->Init( NULL, CsPoint( -5, -5 ), CsPoint( 42, 42 ), "TamerStatus_New\\TamerStatus_C_slot.tga", false );
		sChip.pChipsetBg = pControl->AddSprite( pSlotBg );
		sChip.pChipsetBg->SetAutoPointerDelete( true );
		sChip.pChipsetBg->SetVisible( false );
	}

	sChip.pChipsetIcon = pControl->AddIcon( CsPoint( 32, 32 ), ICONITEM::ITEM_ICON, nItemType, nCount );
	if( sChip.pChipsetIcon )
	{
		sChip.pChipsetIcon->s_nIndex_3 = nItemRate;
		sChip.pChipsetIcon->SetAutoPointerDelete( true );
	}

	sChip.pGridItem  = NiNew cGridListBoxItem( nIndex, CsPoint( 32, 32 ) );
	if( sChip.pGridItem )
	{
		sChip.pGridItem->SetItem( pControl );
		sChip.pGridItem->SetUserData( new sChipsetInfo( nItemType, nItemRate, nIndex ) );

		m_pChipset->AddItem( sChip.pGridItem );
		m_mapChipset.insert( std::make_pair( nIndex, sChip ) );
	}
}

void cTamerStatusUI_Digivice::_MakeTacticsList()
{
	int nIndex = 0;
	if( m_pLTactics )
	{
		m_pLTactics->RemoveAllItem();

		CsPoint ptPos = CsPoint::ZERO;
		_AddTacticsListItem( m_pLTactics, nIndex++, ptPos, true );

		ptPos.x += 40; ptPos.y -= 25;
		_AddTacticsListItem( m_pLTactics, nIndex++, ptPos, true );

		ptPos.x -= 40; ptPos.y -= 25;
		_AddTacticsListItem( m_pLTactics, nIndex++, ptPos, true );

		ptPos.x += 40; ptPos.y -= 25;
		_AddTacticsListItem( m_pLTactics, nIndex++, ptPos, true );
	}
	if( m_pRTactics )
	{
		m_pRTactics->RemoveAllItem();

		CsPoint ptPos = CsPoint( 90, 0 );
		_AddTacticsListItem( m_pRTactics, nIndex++, ptPos, false );
		ptPos.x -= 40; ptPos.y -= 25;
		_AddTacticsListItem( m_pRTactics, nIndex++, ptPos, false );
		ptPos.x += 40; ptPos.y -= 25;
		_AddTacticsListItem( m_pRTactics, nIndex++, ptPos, false );
		ptPos.x -= 40; ptPos.y -= 25;
		_AddTacticsListItem( m_pRTactics, nIndex++, ptPos, false );
	}
}

void cTamerStatusUI_Digivice::_AddTacticsListItem(cListBox* pListBox, int nIndex, CsPoint ptPos, bool bIsLeft)
{
	SAFE_POINTER_RET( pListBox );

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	sTactics sDigimon;

	int nType = _GetSlotType( nIndex );
	bool bIsEnable = ( nType == IF_DIGIMON_SLOT_TYPE_DISABLE ? false : true );
	// 슬롯 이미지
	cSprite* pSlotImg = NiNew cSprite;
	if( pSlotImg )
	{
		switch( nType )
		{
		case IF_DIGIMON_SLOT_TYPE_OPEN:
			pSlotImg->Init( NULL, CsPoint::ZERO, CsPoint( 106, 112 ), "TamerStatus_New\\TamerStatus_Digimon.tga", false );
			break;
		case IF_DIGIMON_SLOT_TYPE_LOCK:
			pSlotImg->Init( NULL, CsPoint::ZERO, CsPoint( 106, 112 ), "TamerStatus_New\\TamerStatus_Digimon_lock.tga", false );
			break;
		case IF_DIGIMON_SLOT_TYPE_DISABLE:
			pSlotImg->Init( NULL, CsPoint::ZERO, CsPoint( 106, 112 ), "TamerStatus_New\\TamerStatus_Digimon_disable.tga", false );
			break;
		}
		sDigimon.pSlotImg = pControl->AddSprite( pSlotImg, ptPos );
		if( sDigimon.pSlotImg )
		{
			sDigimon.pSlotImg->SetAutoPointerDelete( true );
			if( !bIsEnable )
				sDigimon.pSlotImg->SetAlpha( 0.2f );
		}
	}

	CsPoint ptGab = CsPoint( 24, 26 );
	// 디지몬 얼굴 L size
	cSprite* pDigimonImg = NiNew cSprite;
	if( pDigimonImg )
	{
		std::string strPath = GetSystem()->GetTacticsFileName( nIndex );
		pDigimonImg->Init( NULL, CsPoint::ZERO, CsPoint( 57, 57 ), strPath.c_str(), false, NiColor::WHITE, false );
		sDigimon.pDigimonImg = pControl->AddSprite( pDigimonImg, ptPos + ptGab );
		if( sDigimon.pDigimonImg )
		{
			sDigimon.pDigimonImg->SetAutoPointerDelete( true );
			if( strPath.empty() )
				sDigimon.pDigimonImg->SetVisible( false );
		}
	}

	// 디지몬 얼굴 덮개 이미지
	cSprite* pCanopyImg = NiNew cSprite;
	if( pCanopyImg )
	{
		pCanopyImg->Init( NULL, CsPoint::ZERO, CsPoint( 57, 57 ), "TamerStatus_New\\TamerStatus_Canopy.tga", false );
		sDigimon.pCanopyImg = pControl->AddSprite( pCanopyImg, ptPos + ptGab );
		if( sDigimon.pCanopyImg )
		{
			sDigimon.pCanopyImg->SetAutoPointerDelete( true );
			sDigimon.pCanopyImg->SetVisible( bIsEnable );
		}
	}

	if( bIsLeft )
		ptGab = CsPoint( 70, 55 );
	else
		ptGab = CsPoint( -30, 55 );
	// 레벨 배경
	cSprite* pLevelImg = NiNew cSprite;
	if( pLevelImg )
	{
		pLevelImg->Init( NULL, CsPoint::ZERO, CsPoint( 57, 43 ), "TamerStatus_New\\TamerStatus_Lv_field.tga", false );
		cString::sSPRITE* pSprite = pControl->AddSprite( pLevelImg, ptPos + ptGab );
		if( pSprite )
		{
			pSprite->SetAutoPointerDelete( true );
			pSprite->SetVisible( nType == IF_DIGIMON_SLOT_TYPE_DISABLE ? false : true );
		}
	}

	if( bIsLeft )
		ptGab = CsPoint( 95, 70 );
	else
		ptGab = CsPoint( -7, 70 );
	{	// 레벨 텍스트
		int nLevel = GetSystem()->GetTacticsLevel( nIndex );
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor( 240.f/255.f, 214.f/255.f, 0 ) );
		ti.s_eTextAlign = DT_LEFT;
		ti.SetText( nLevel );
		sDigimon.pLevelText = pControl->AddText( &ti, ptPos + ptGab );
		if( sDigimon.pLevelText )
		{
			sDigimon.pLevelText->SetAutoPointerDelete( true );
 			if( 0 == nLevel )
 				sDigimon.pLevelText->SetVisible( false );
		}
	}

	sDigimon.pListItem = NiNew cListBoxItem;
	if( sDigimon.pListItem )
	{
		sDigimon.pListItem->SetUserData( new sTacticsInfo( nIndex ) );
		sDigimon.pListItem->SetItem( pControl );
		pListBox->AddItem( sDigimon.pListItem );
		m_mapTactics.insert( std::make_pair( nIndex, sDigimon ) );
	}
}

void cTamerStatusUI_Digivice::_OnMouseOverChipset(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pOverItem = static_cast< cGridListBoxItem* >( pData );
	sChipsetInfo* pChipInfo = dynamic_cast< sChipsetInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pChipInfo );

	cTooltip* pTooltip = TOOLTIPMNG_STPTR->GetTooltip();
	SAFE_POINTER_RET( pTooltip );

	if( 0 == pChipInfo->nItemType )
	{
#ifdef SIMPLE_TOOLTIP
		pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 36, 36 ), 100, cTooltip::UIHELP, 1043 );
#endif
		return;
	}

	pTooltip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, 
		pChipInfo->nItemType, cBaseWindow::WT_NEW_TAMERSTATUS, 0, 0, GetSystem()->GetChipsetItem( pChipInfo->nChipsetIndex ) );
}

void cTamerStatusUI_Digivice::_OnLClickChipset(void*pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pOverItem = static_cast< cGridListBoxItem* >( pData );

	if( CURSOR_ST.IsIcon() )
		return;

	if( _OnClickChipset( pOverItem ) )
		return;

	CURSOR_ST.ReleaseIcon();
}

void cTamerStatusUI_Digivice::_OnRClickChipset(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pChipset = static_cast< cGridListBoxItem* >( pData );
	sChipsetInfo* pChipsetInfo = dynamic_cast< sChipsetInfo* >( pChipset->GetUserData() );
	SAFE_POINTER_RET( pChipsetInfo );

	GetSystem()->MoveChipsetItem( CURSOR_ST.GetIconSlot(), pChipsetInfo->nChipsetIndex, true, true );
}

bool cTamerStatusUI_Digivice::_OnClickChipset(cGridListBoxItem const* pOverItem)
{
	bool bSetItem = false;
	if( !pOverItem )
	{
		CURSOR_ST.ReleaseIcon();
		return bSetItem;
	}

	sChipsetInfo* pChipsetInfo = dynamic_cast< sChipsetInfo* >( pOverItem->GetUserData() );
	if( pChipsetInfo )
	{
		CURSOR_ST.SetIcon( CURSOR_ICON::CI_CHIPSET, pChipsetInfo->nChipsetIndex, 1, NULL );
		CURSOR_ST.SetIconSubInfo( pChipsetInfo->nItemRate, 0, 0 );
		bSetItem = true;
	}

	if( !bSetItem )
		CURSOR_ST.ReleaseIcon();

	return bSetItem;
}

int cTamerStatusUI_Digivice::_GetSlotType(int nIndex)
{
	int nSlotCount = GetSystem()->GetTacticsCount();
	int nOpenCount = GetSystem()->GetTacticsOpenCount();
	int nType = IF_DIGIMON_SLOT_TYPE_DISABLE;
	if( nIndex < nSlotCount )
		nType = IF_DIGIMON_SLOT_TYPE_LOCK;
	if( nIndex < nOpenCount )
		nType = IF_DIGIMON_SLOT_TYPE_OPEN;

	return nType;
}

void cTamerStatusUI_Digivice::_OnClickTactics(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cListBoxItem* pOverItem = static_cast< cListBoxItem* >( pData );
	sTacticsInfo* pTacticsInfo = dynamic_cast< sTacticsInfo* >( pOverItem->GetUserData() );
	SAFE_POINTER_RET( pTacticsInfo );

	std::map< int, sTactics >::iterator it = m_mapTactics.find( pTacticsInfo->nTacticsIndex );
	if( it == m_mapTactics.end() )
		return;

	cString::sSPRITE* pSprite = it->second.pDigimonImg;
	SAFE_POINTER_RET( pSprite );
	CsRect rc( pSprite->s_RenderPos, CsSIZE( pSprite->s_ptSize ) );
	if( rc.PtInRect( CURSOR_ST.GetPos() ) )
	{
		uint nTacticsType = GetSystem()->GetTacticsType( pTacticsInfo->nTacticsIndex );
		if( 0 == nTacticsType )
			return;

		GetSystem()->OnClickTactitcs( pTacticsInfo->nTacticsIndex );
	}
}
////////////////////////////////////////////////////////////////////////////////////////
void cTamerStatusUI_Digivice::sChipset::ChangeItem(uint nItemType, uint nItemCount /*= 1*/)
{
	SAFE_POINTER_RET( pChipsetIcon );
	SAFE_POINTER_RET( pChipsetBg );
	SAFE_POINTER_RET( pGridItem );

	sChipsetInfo* pUserData = dynamic_cast< sChipsetInfo* >( pGridItem->GetUserData() );
	if( pUserData )
		pUserData->nItemType = nItemType;

	pChipsetIcon->ChangeIcon( ICONITEM::ITEM_ICON, nItemType, nItemCount );

	if( 0 != nItemType )
	{
		pChipsetBg->SetVisible( true );
		pGridItem->SetEnable( true );
	}
	else
	{
		pChipsetBg->SetVisible( false );
		pGridItem->SetEnable( false );
	}
}

void cTamerStatusUI_Digivice::sChipset::SetChipsetBg(bool bIsVisible)
{
	if( pChipsetBg )
		pChipsetBg->SetVisible( bIsVisible );
}
////////////////////////////////////////////////////////////////////////////////////////
void cTamerStatusUI_Digivice::sTactics::UpdateDigimon(int nTacticsIndex, int nTacticsLevel, bool bIsOpen, std::string strPath, int nHatchLv)
{
	SAFE_POINTER_RET( pListItem );
	SAFE_POINTER_RET( pDigimonImg );
	SAFE_POINTER_RET( pLevelText );
	SAFE_POINTER_RET( pSlotImg );

	// 디지몬 이미지 변경
	pDigimonImg->SetVisible( true );
	pDigimonImg->ChangeTexture( strPath.c_str(), false );
	if( strPath.empty() )
		pDigimonImg->SetVisible( false );

	// 디지몬 인덱스 변경
	sTacticsInfo* pTacticsInfo = dynamic_cast< sTacticsInfo* >( pListItem->GetUserData() );
	if( pTacticsInfo )
		pTacticsInfo->nTacticsIndex = nTacticsIndex;

	// 디지몬 레벨 변경
	std::wstring wsLevel;
	DmCS::StringFn::From( wsLevel, nTacticsLevel );
	pLevelText->SetText( wsLevel.c_str() );
	if( 0 < nTacticsLevel )
		pLevelText->SetVisible( true );
	else
		pLevelText->SetVisible( false );

	// 슬롯 이미지 변경
	if( bIsOpen )
	{
		if( 5 < nHatchLv )
		{
			pSlotImg->ChangeTexture( "TamerStatus_New\\TamerStatus_Digimon_X.tga" );
			pCanopyImg->ChangeTexture( "TamerStatus_New\\TamerStatus_Canopy_X.tga" );
		}
		else
		{
			pSlotImg->ChangeTexture( "TamerStatus_New\\TamerStatus_Digimon.tga" );
			pCanopyImg->ChangeTexture( "TamerStatus_New\\TamerStatus_Canopy.tga" );
		}
	}
}

