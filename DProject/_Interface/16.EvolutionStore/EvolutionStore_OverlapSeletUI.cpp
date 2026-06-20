#include "stdafx.h"
#include "EvolutionStore_OverlapSeletUI.h"

cOverlapDigimonSelectUI::cOverlapDigimonSelectUI():m_pSelectCancel(NULL),m_pSelectGrid(NULL)
{
	Construct();
}

cOverlapDigimonSelectUI::~cOverlapDigimonSelectUI()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

//////////////////////////////////////////////////////////////////////////
void cOverlapDigimonSelectUI::Init( cWindow* pRootWin )
{
	InitScript(pRootWin, "SpiritEvo\\Spirit_Selectbox.tga", CsPoint(149, 119), CsPoint(180, 98), false );
	AddTitle( UISTRING_TEXT("EVOLUTION_STORE_OVERLAP_DIGIMON_SELECT_WINDOW_TITLE").c_str() );//_LAN( "디지몬 선택" ) 

	// 캔슬 버튼
	m_pSelectCancel = AddButton( CsPoint( 155, 7 ), CsPoint( 16, 16 ),  CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
	if( m_pSelectCancel )
		m_pSelectCancel->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cOverlapDigimonSelectUI::_CloseWindow );

	m_pSelectGrid = NiNew cGridListBox;
	if( m_pSelectGrid )
	{
		m_pSelectGrid->Init( m_pRoot, CsPoint(22, 47), CsPoint(136, 32), CsPoint(20,0), CsPoint(32,32),cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		m_pSelectGrid->AddEvent( cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &cOverlapDigimonSelectUI::OnClicked_SelectDigimon );
		//m_pSelectGrid->SetBackGorundImg( " ");
		m_pSelectGrid->SetMouseOverImg( "Icon\\Mask_Over.tga" );
		m_pSelectGrid->SetSelectedImg( "Icon\\Mask_Over.tga" );
		m_pSelectGrid->SetBackOverAndSelectedImgRender(false);
		AddChildControl( m_pSelectGrid );
	}
}

void cOverlapDigimonSelectUI::SetInitData(DWORD const& dwDigimonTbIDX, int const& nLv)
{
	SAFE_POINTER_RET( m_pSelectGrid );
	m_pSelectGrid->RemoveAllItem();

	std::list<std::pair<int,std::string>> slotInfo;
	GetSystem()->GetTacticsDigimonSlotInfo(dwDigimonTbIDX, nLv, slotInfo);

	std::list<std::pair<int,std::string>>::const_iterator it = slotInfo.begin();
	for( int n = 0; it != slotInfo.end(); ++it, ++n )
	{		
		cString * pItem = NiNew cString;
		cSprite * pkItemIcon = NiNew cSprite;
		if( pkItemIcon )
		{
			pkItemIcon->Init( NULL, CsPoint::ZERO, CsPoint(32,32), (*it).second.c_str(), false, NiColor::WHITE, false);
			cString::sSPRITE* pSp = pItem->AddSprite( pkItemIcon, CsPoint(0,0), CsPoint(32,32));	
			if( pSp )
				pSp->SetAutoPointerDelete(true);
		}

		cGridListBoxItem * pGridItem  = NiNew cGridListBoxItem(n,  CsPoint(32,32));
		pGridItem->SetItem( pItem );
		pGridItem->SetUserData( new sTactisData( (*it).first , dwDigimonTbIDX ) );
		m_pSelectGrid->AddItem( pGridItem );
	}
}

void cOverlapDigimonSelectUI::ResetData()
{
	if( m_pSelectGrid )
		m_pSelectGrid->RemoveAllItem();
}

bool cOverlapDigimonSelectUI::UpdateMouse()
{
	if(!IsVisible())
		return false;

	if( m_pSelectCancel && m_pSelectCancel->Update_ForMouse() != cButton::ACTION_NONE )
		return true;

	if( m_pSelectGrid && m_pSelectGrid->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cGridListBoxItem const* pOverGrid = m_pSelectGrid->GetMouseOverItem();
		SAFE_POINTER_RETVAL(pOverGrid, true);

		sTactisData* pkInfo = dynamic_cast<sTactisData*>(pOverGrid->GetUserData());
		SAFE_POINTER_RETVAL(pkInfo, true);

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, true);

		pToolTip->SetTooltip( pOverGrid->GetWorldPos(), CsPoint( 32, 32 ), 140, cTooltip::TACTICS, pkInfo->nSlotNum, pkInfo->dwTbIDX );
		return true;
	}

	return true;
}

void cOverlapDigimonSelectUI::_CloseWindow(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->CloseOverlapDigimonSelectUI();
}

void cOverlapDigimonSelectUI::OnClicked_SelectDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pSelectGrid );
	cGridListBoxItem const* pSelItem = m_pSelectGrid->GetSelectedItem();
	SAFE_POINTER_RET( pSelItem );
	sTactisData* pkInfo = dynamic_cast<sTactisData*>(pSelItem->GetUserData());
	SAFE_POINTER_RET(pkInfo);

	GetSystem()->DigimonToItem_SelectOverlapDigimon( pkInfo->nSlotNum, pkInfo->dwTbIDX );
}

//////////////////////////////////////////////////////////////////////////

bool cOverlapDigimonSelectUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eOpenOverlapSelectUI, this );
	GetSystem()->Register( SystemType::eCloseOverlapSelectUI, this );

	return true;
}

void cOverlapDigimonSelectUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eOpenOverlapSelectUI:
		{
			DWORD dwShowDigimonTBIDX = 0;
			int nLv = 0;
			kStream >> dwShowDigimonTBIDX >> nLv;
			SetInitData(dwShowDigimonTBIDX, nLv);
			SetVisible(true);
		}
		break;
	case SystemType::eCloseOverlapSelectUI:
		{
			ResetData();
			SetVisible(false);
		}
		break;
	}
}