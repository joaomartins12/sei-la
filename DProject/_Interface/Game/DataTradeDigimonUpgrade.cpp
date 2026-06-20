#include "StdAfx.h"

#include "DataTradeContents.h"
#include "DataTradeBase.h"
#include "DataTradeDigimonUpgrade.h"


DataTradeDigimonUpgrade::DataTradeDigimonUpgrade(void) :
m_pLeftGrid(NULL),m_pRightGrid(NULL)
{
}

DataTradeDigimonUpgrade::~DataTradeDigimonUpgrade(void)
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}
void DataTradeDigimonUpgrade::Destroy()
{	
	DataTradeBase::Destroy();
}

void DataTradeDigimonUpgrade::DeleteResource()
{
	DataTradeBase::DeleteResource();
	m_pLeftGrid = NULL;
	m_pRightGrid = NULL;
}

void DataTradeDigimonUpgrade::ResetDevice()
{
	DataTradeBase::ResetDevice();
}

bool DataTradeDigimonUpgrade::Close( bool bSound /* = true  */ )
{
	DataTradeBase::Close( bSound );
	return true;
}

void DataTradeDigimonUpgrade::Create( int nValue /*=0*/ )
{
	DataTradeBase::Create();

	m_VarPos		= CsPoint(m_BlockSize.cx/2, 127);
	
	AddTitle( UISTRING_TEXT( "DATA_TRADE_TITLE_UPGRADE" ).c_str(), CsPoint(0, m_TopPos.y + 2), CFont::FS_11 );

	SetVariableName((TCHAR*)UISTRING_TEXT( "DATA_TRADE_UPGRADE" ).c_str());

	_MakeEnchantLvInfo();
}

void DataTradeDigimonUpgrade::Update(float const& fDeltaTime)
{
	DataTradeBase::Update(fDeltaTime);
}

cBaseWindow::eMU_TYPE DataTradeDigimonUpgrade::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = DataTradeBase::Update_ForMouse();

	return muReturn;
}

void DataTradeDigimonUpgrade::Render()
{
	DataTradeBase::Render();

	if(IsFlicker())	// 데이터 교환 성공후 플리커(깜박임) 기능 중일 때
		return;
}


void DataTradeDigimonUpgrade::CreateTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	DataTradeBase::CreateTradeSlotData(etype, pInfo);

	_UpdateChantLvInfo( etype );
}

void DataTradeDigimonUpgrade::DeleteTradeSlotData(eDIGIMONSLOT etype, _DATA_INFO*	pInfo)
{
	DataTradeBase::DeleteTradeSlotData(etype, pInfo);

	_UpdateChantLvInfo(etype);
}
void DataTradeDigimonUpgrade::ResetPosition()
{
	DataTradeBase::ResetPosition();
}
void DataTradeDigimonUpgrade::ResetData()
{
	StartFlicker();
}

void DataTradeDigimonUpgrade::OnFlicker()
{
	DataTradeBase::OnFlicker();

	MAP_ENCHANT_INFO_IT itLeft = m_mapLeftInfo.begin();
	for( ; itLeft != m_mapLeftInfo.end(); ++itLeft )
		itLeft->second.SetVisible( !m_bFlicker );

	MAP_ENCHANT_INFO_IT itRight = m_mapRightInfo.begin();
	for( ; itRight != m_mapRightInfo.end(); ++itRight )
		itRight->second.SetVisible( !m_bFlicker );
}

void DataTradeDigimonUpgrade::_MakeEnchantLvInfo()
{
	CsPoint ptGridSize( 245, 110 );
	CsPoint ptGridGap( 5, 5 );
	CsPoint ptGridItemSize( 120, 28 );
	CsPoint ptLGridPos( m_LeftPos.x + 22, m_LeftPos.y + 120 );
	CsPoint ptRGridPos( m_RightPos.x + 22, m_RightPos.y + 120 );

	if( !m_pLeftGrid )
	{
		m_pLeftGrid = NiNew cGridListBox;
		SAFE_POINTER_RET( m_pLeftGrid );
		AddChildControl( m_pLeftGrid );
	}
	m_pLeftGrid->Init( m_pRoot, ptLGridPos, ptGridSize, ptGridGap, ptGridItemSize, cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 2 );
	m_pLeftGrid->RemoveAllItem();
	m_mapLeftInfo.clear();
	for( int i = 0; i < nExtendState::MaxExtendStat; ++i )
	{
		switch( i )
		{
		case ET_AT: case ET_BL: case ET_CR: case ET_EV:
#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		case ET_HP:
#endif
			_AddEnchantLvInfo( m_pLeftGrid, m_mapLeftInfo, i );
			break;
		}
	}


	if( !m_pRightGrid )
	{
		m_pRightGrid = NiNew cGridListBox;
		SAFE_POINTER_RET( m_pRightGrid );
		AddChildControl( m_pRightGrid );
	}
	m_pRightGrid->Init( m_pRoot, ptRGridPos, ptGridSize, ptGridGap, ptGridItemSize, cGridListBox::LowRightDown, cGridListBox::LeftTop, false, 2 );
	m_pRightGrid->RemoveAllItem();
	m_mapRightInfo.clear();
	for( int i = 0; i < nExtendState::MaxExtendStat; ++i )
	{
		switch( i )
		{
		case ET_AT: case ET_BL: case ET_CR: case ET_EV:
#ifdef SDM_DIGIMON_TRANSCENDENCE_CONTENTS_20190507
		case ET_HP:
#endif
			_AddEnchantLvInfo( m_pRightGrid, m_mapRightInfo, i );
			break;
		}
	}
}

void DataTradeDigimonUpgrade::_AddEnchantLvInfo(cGridListBox* pGrid, MAP_ENCHANT_INFO& mapEnchant, int const& nState)
{
	SAFE_POINTER_RET( pGrid );

	cString* pControl = NiNew cString;
	SAFE_POINTER_RET( pControl );

	// 슬롯 배경
	cSprite* pSlotBg = NiNew cSprite;
	if( pSlotBg )
	{
		pSlotBg->Init( NULL, CsPoint::ZERO, CsPoint( 120, 28 ), "DataExchange\\Data_Exchange_Info_Enchant_BG.tga", false );
		cString::sSPRITE* pSprt = pControl->AddSprite( pSlotBg );
		if( pSprt )
			pSprt->SetAutoPointerDelete( true );
	}

	// 인챈트 배경
	cSprite* pEnchantBg = NiNew cSprite;
	if( pEnchantBg )
	{
		pEnchantBg->Init( NULL, CsPoint::ZERO, CsPoint( 64, 20 ), "DataExchange\\Data_Exchange_Info_Enchant_Mask.tga", false );
		cString::sSPRITE* pSprt = pControl->AddSprite( pEnchantBg, CsPoint( 10, 4 ) );
		if( pSprt )
			pSprt->SetAutoPointerDelete( true );
	}

	{	// 강화 정보
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		switch( nState )
		{
		case ET_AT: ti.SetText( UISTRING_TEXT( "SEALMASTER_STATE_AT" ).c_str() ); break;
		case ET_BL: ti.SetText( UISTRING_TEXT( "SEALMASTER_STATE_BL" ).c_str() ); break;
		case ET_CR: ti.SetText( UISTRING_TEXT( "SEALMASTER_STATE_CT" ).c_str() ); break;
		case ET_EV: ti.SetText( UISTRING_TEXT( "SEALMASTER_STATE_EV" ).c_str() ); break;
		case ET_HP: ti.SetText( UISTRING_TEXT( "SEALMASTER_STATE_HP" ).c_str() ); break;
		}
		cString::sTEXT* pTxt = pControl->AddText( &ti, CsPoint( 42, 14 ) );
		if( pTxt )
			pTxt->SetAutoPointerDelete( true );
	}

	// 레벨 아이콘
	sEnchantLvInfo sInfo;
	sInfo.s_pLvIcon = pControl->AddIcon( CsPoint( 23, 16 ), ICONITEM::Enchant_Lv, 0, 0, CsPoint( 87, 6 ) );
	if( sInfo.s_pLvIcon )
		sInfo.s_pLvIcon->SetAutoPointerDelete( true );

	cGridListBoxItem* pGBItem = NiNew cGridListBoxItem;
	if( pGBItem )
	{
		pGBItem->SetItem( pControl );
		pGrid->AddItem( pGBItem );
		mapEnchant.insert( std::make_pair( nState, sInfo ) );
	}
}

void DataTradeDigimonUpgrade::_UpdateChantLvInfo(eDIGIMONSLOT const& eSlot)
{
	stLoadData* pLoad = GetDataInfo( eSlot )->GetLoadData();

	switch( eSlot )
	{
	case SLOT_LEFT:
		{
			MAP_ENCHANT_INFO_IT it = m_mapLeftInfo.begin();
			for( ; it != m_mapLeftInfo.end(); ++it )
				it->second.ChangeLvIcon( pLoad ? pLoad->s_ExtendAttributeLV[ it->first ] : 0 );
		}
		break;
	case SLOT_RIGHT:
		{
			MAP_ENCHANT_INFO_IT it = m_mapRightInfo.begin();
			for( ; it != m_mapRightInfo.end(); ++it )
				it->second.ChangeLvIcon( pLoad ? pLoad->s_ExtendAttributeLV[ it->first ] : 0 );
		}
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool DataTradeDigimonUpgrade::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpgrade_Reset, this );
	GetSystem()->Register( SystemType::eUpgrade_Trade, this );

	return true;
}

void DataTradeDigimonUpgrade::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eUpgrade_Reset:
		{
			_UpdateChantLvInfo( SLOT_LEFT );
			_UpdateChantLvInfo( SLOT_RIGHT );
			ResetData();
		}
		break;
	case SystemType::eUpgrade_Trade:
		SendToTrade();
		break;
	}
}

//////////////////////////////////////////////////////////////////////////
void DataTradeDigimonUpgrade::sEnchantLvInfo::ChangeLvIcon(int const& nLv)
{
	SAFE_POINTER_RET( s_pLvIcon );
	s_pLvIcon->ChangeIcon( ICONITEM::Enchant_Lv, nLv );
}

void DataTradeDigimonUpgrade::sEnchantLvInfo::SetVisible(bool const& bVisible)
{
	SAFE_POINTER_RET( s_pLvIcon );
	s_pLvIcon->SetVisible( bVisible );
}
