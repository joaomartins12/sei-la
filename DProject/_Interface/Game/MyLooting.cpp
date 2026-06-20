#include "stdafx.h"
#include "MyLooting.h"
#include "common_vs2019/pParty.h"

//////////////////////////////////////////////////////////////////////////
cMyItemLoot::cMyItemLoot()
{

}

cMyItemLoot::~cMyItemLoot()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}


void cMyItemLoot::Destroy()
{	
	cBaseLoot::Delete();		
}

void cMyItemLoot::DeleteResource()
{	
	DeleteScript();
}

void cMyItemLoot::Create(int nValue /* = 0  */)
{
	cBaseLoot::Init();

	InitScript( NULL, CsPoint( 428, 520 ), CsPoint( 202, 80 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::BOTTOM );
	m_pLootingList = NiNew cListBox;
	if( m_pLootingList )
	{
		m_pLootingList->Init( GetRoot(), CsPoint::ZERO, CsPoint( 202, 80 ), NULL, false );		
		m_pLootingList->SetItemGab(CsPoint(0,15));
		m_pLootingList->SetItemHeight( 74 );
		m_pLootingList->SetHideItemRender(true);

		AddChildControl( m_pLootingList );
	}
	_MakeLootingUI();
}


void cMyItemLoot::_MakeLootingUI()
{
	SMemberInfo myInfo;
	if( GetSystem()->GetMySimpleInfo(myInfo) )
		_AddLootingWindow( &myInfo );
}

bool cMyItemLoot::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

 	GetSystem()->Register( SystemType::eADD_ME_LOOTING_ITEM_VIEW, this );
	return true;
}

void cMyItemLoot::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eADD_ME_LOOTING_ITEM_VIEW:
		{
			CPartyContents::sLootingItems addViewItem;
			std::wstring wsItemName;
			kStream >> addViewItem >> wsItemName;
			_AddGetItemInfo( addViewItem, wsItemName );
		}
		break;
	}
}


