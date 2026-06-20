#include "stdafx.h"
#include "BaseLootingWindow.h"
#include "common_vs2019/pParty.h"
//////////////////////////////////////////////////////////////////////////
cBaseLoot::sViewControls::sViewControls():m_pItemIcon(NULL),m_pLootingRuleIcon(NULL),
m_pItemName(NULL),m_pDiceValue(NULL),m_BoxItem(NULL)
{

}

cBaseLoot::sViewControls::~sViewControls()
{
	m_pItemIcon = NULL;
	m_pLootingRuleIcon = NULL;
	m_pItemName = NULL;
	m_pDiceValue = NULL;
	m_BoxItem = NULL;
}

//////////////////////////////////////////////////////////////////////////
cBaseLoot::cBaseLoot():m_pLootingList(NULL)
{

}

cBaseLoot::~cBaseLoot()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}


void cBaseLoot::Destroy()
{	
	cBaseWindow::Delete();		
}

void cBaseLoot::DeleteResource()
{	
	DeleteScript();
}

void cBaseLoot::Create(int nValue /* = 0  */)
{
	cBaseWindow::Init();
}

void cBaseLoot::_AddLootingWindow( SMemberInfo const* pInfo )
{
	SAFE_POINTER_RET( pInfo );
	SAFE_POINTER_RET( m_pLootingList );

	std::map<int,sViewControls>::iterator it = m_mapViewControls.find( pInfo->m_nSlotNo );
	if( it != m_mapViewControls.end() )
		return;

	sViewControls viewControls;
	cString * pItem = NiNew cString;

	// 배경 이미지
	cSprite * pBack = NiNew cSprite;
	if( pBack )
	{
		pBack->Init( NULL, CsPoint(0,6), CsPoint( 201, 73 ), "Party\\Win_looting.tga", false );
		cString::sSPRITE* paddImg = pItem->AddSprite( pBack );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}

	// 아이템 이미지
	viewControls.m_pItemIcon = pItem->AddIcon( CsPoint( 32, 32 ), 0, 0, 0, CsPoint(7,12) );

	// 아이템 이름
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		//ti.s_eTextAlign = DT_CENTER;
		ti.SetString(L" ");
		viewControls.m_pItemName = pItem->AddText( &ti, CsPoint( 52, 12 ) );
	}

	//14, 16, 17, 15
	// 아이템 획득 룰
	viewControls.m_pLootingRuleIcon = pItem->AddIcon( CsPoint( 21, 21 ), ICONITEM::Community, 14, 0, CsPoint(22,46) );

	// 테이머 네임
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		//ti.s_eTextAlign = DT_CENTER;
		ti.SetString(pInfo->m_TamerInfo.m_Name.c_str());
		pItem->AddText( &ti, CsPoint( 52, 45 ) );
	}

	// 주사위 결과
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
		//ti.s_eTextAlign = DT_CENTER;
		ti.SetString( L"100" );
		viewControls.m_pDiceValue = pItem->AddText( &ti, CsPoint( 48, 64 ) );
	}

	viewControls.m_BoxItem  = NiNew cListBoxItem;
	viewControls.m_BoxItem->SetUserData( new sMemberIdx(pInfo->m_nSlotNo) );	
	viewControls.m_BoxItem->SetItem( pItem );
	viewControls.m_BoxItem->SetVisible( false );
	m_pLootingList->AddItem( viewControls.m_BoxItem );

	m_mapViewControls.insert( std::make_pair( pInfo->m_nSlotNo, viewControls ) );
}

void cBaseLoot::_AddViewItem( int const& nSlotNum, sGetItemInfo const& addItem )
{	
	std::map<int,sViewControls>::iterator it = m_mapViewControls.find(nSlotNum);
	if( it == m_mapViewControls.end() )
		return;

	SAFE_POINTER_RET( it->second.m_BoxItem );

	it->second.m_BoxItem->SetVisible(true);

	if( it->second.m_pItemIcon )
		it->second.m_pItemIcon->ChangeIcon( addItem.m_dwItemIconType, addItem.m_dwItemType, addItem.m_nItemCount );

	if( it->second.m_pItemName )
	{
		it->second.m_pItemName->SetColor( addItem.m_ItemColor );
		it->second.m_pItemName->SetText( addItem.m_wsItemName.c_str(), 147 );
	}

	if( it->second.m_pDiceValue )
	{
		std::wstring diceText;
		if( addItem.m_nDiceNumber > 0 )
			DmCS::StringFn::From( diceText, addItem.m_nDiceNumber );
		it->second.m_pDiceValue->SetText( diceText.c_str() );
	}

	if( it->second.m_pLootingRuleIcon )
	{
		int nIconIdx = 0;
		switch (addItem.m_sLootingrule.m_nCropType)
		{
		case nCrop::Base:	nIconIdx = 14;			break;
		case nCrop::Order:	nIconIdx = 16;			break;
		case nCrop::Free:
			{
				if( addItem.m_sLootingrule.m_nRareRate == 0 )
					nIconIdx = 17;
				else
					nIconIdx = 15;
			}
			break;
		}
		it->second.m_pLootingRuleIcon->ChangeIcon( ICONITEM::Community, nIconIdx, 0 );
	}	
}

void cBaseLoot::_DeleteMember(int const& nSlotNum)
{
	std::map<int,sViewControls>::iterator it = m_mapViewControls.find(nSlotNum);
	if( it == m_mapViewControls.end() )
		return;

	SAFE_POINTER_RET( m_pLootingList );
	m_pLootingList->RemoveItem( it->second.m_BoxItem );
	m_mapViewControls.erase( it );

	ItemCont::iterator subit = m_mapItems.find( nSlotNum );
	if( subit != m_mapItems.end() )
		m_mapItems.erase( subit );
}

void cBaseLoot::Update(float const& fDeltaTime)
{
	ItemCont::iterator it = m_mapItems.begin();
	for( ; it != m_mapItems.end(); ++it )
	{
		SubItemCont::iterator subIt = it->second.begin();
		if( subIt != it->second.end() )
		{
			subIt->m_fLifeTime -= fDeltaTime;
			if( subIt->m_fLifeTime <= 0 )
			{
				_HideLootingView( it->first );
				subIt = it->second.erase( subIt );
				if( subIt != it->second.end() )
					_AddViewItem( it->first, *subIt );
				break;
			}
		}
	}
}	

cBaseWindow::eMU_TYPE
cBaseLoot::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;	

	if(m_pLootingList && m_pLootingList->Update_ForMouse(CURSOR_ST.GetPos()) )
		return MUT_OUT_WINDOW;

	return MUT_OUT_WINDOW;
}


void cBaseLoot::Render()
{
	RenderScript();
}

void cBaseLoot::ResetDevice()
{
	ResetDeviceScript();
}

void cBaseLoot::_AddGetItemInfo(CPartyContents::sLootingItems const& getItem, std::wstring const& wsItemName)
{
	ItemCont::iterator it = m_mapItems.find(getItem.m_nSlotNum);
	if( it == m_mapItems.end() )
	{
		auto Rst = m_mapItems.insert( std::make_pair( getItem.m_nSlotNum, SubItemCont() ) );
		if( !Rst.second )
			return;
		it = Rst.first;
	}

	sGetItemInfo addItem;
	addItem.m_dwItemIconIdx	= getItem.m_dwItemIconIdx;
	addItem.m_nItemCount 	= getItem.m_nItemCount;
	addItem.m_dwItemType	= getItem.m_dwItemType;
	addItem.m_dwItemIconType= getItem.m_dwItemIconType;
	addItem.m_nDiceNumber	= getItem.m_nDiceNumber;
	addItem.m_wsItemName	= wsItemName;
	addItem.m_sLootingrule	= getItem.m_sLootingrule;
	addItem.m_ItemColor		= getItem.m_ItemColor;
	it->second.push_back( addItem );

	if( it->second.size() == 1 )
		_AddViewItem( getItem.m_nSlotNum, addItem );
}

void cBaseLoot::_HideLootingView( int const& nSlotNum )
{
	std::map<int,sViewControls>::iterator it = m_mapViewControls.find( nSlotNum );
	if( it == m_mapViewControls.end() )
		return;

	it->second.m_BoxItem->SetVisible(false);
}

bool cBaseLoot::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}