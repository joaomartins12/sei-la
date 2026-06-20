#include "stdafx.h"
#include "Looting.h"
#include "common_vs2019/pParty.h"
//////////////////////////////////////////////////////////////////////////
cPartyItemLoot::cPartyItemLoot()
{

}

cPartyItemLoot::~cPartyItemLoot()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}


void cPartyItemLoot::Destroy()
{	
	cBaseLoot::Delete();		
}

void cPartyItemLoot::DeleteResource()
{	
	DeleteScript();
}

void cPartyItemLoot::Create(int nValue /* = 0  */)
{
	cBaseLoot::Init();

	InitScript( NULL , CsPoint( 210, 207 ), CsPoint( 202, 260 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
	m_pLootingList = NiNew cListBox;
	if( m_pLootingList )
	{
		m_pLootingList->Init( GetRoot(), CsPoint::ZERO, CsPoint( 202, 252 ), NULL, false );		
		m_pLootingList->SetItemGab(CsPoint(0,15));
		m_pLootingList->SetItemHeight( 74 );
		m_pLootingList->SetHideItemRender(true);

		AddChildControl( m_pLootingList );
	}
	_MakeLootingUI();
}

void cPartyItemLoot::_MakeLootingUI()
{
	LIST_MEMBER_INFO const& listInfo  = GetSystem()->GetMemberList();
	LIST_MEMBER_INFO_CIT it = listInfo.begin();
	for( ; it != listInfo.end(); ++it )
		_AddLootingWindow( (*it) );
}

bool cPartyItemLoot::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eADD_NEW_PARTY_MEMBER, this );
	GetSystem()->Register( SystemType::eADD_MEMBER_LOOTING_ITEM_VIEW, this );
	GetSystem()->Register( SystemType::eMEMBER_BANISH, this );
	GetSystem()->Register( SystemType::eMEMBER_LEAVE, this );
	return true;
}

void cPartyItemLoot::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eADD_NEW_PARTY_MEMBER:
		{
			kStream.SetRp(0);
			char addSlotNum = 0;
			kStream >> addSlotNum;

			SMemberInfo const * pInfo = GetSystem()->GetMemberInfoFromSlotNo( addSlotNum );
			if( pInfo )
				_AddLootingWindow( pInfo );
		}
		break;
	case SystemType::eADD_MEMBER_LOOTING_ITEM_VIEW:
		{
			CPartyContents::sLootingItems addViewItem;
			std::wstring wsItemName;
			kStream >> addViewItem >> wsItemName;
			_AddGetItemInfo( addViewItem, wsItemName );
		}
		break;
	case SystemType::eMEMBER_BANISH:
	case SystemType::eMEMBER_LEAVE:
		{
			kStream.SetRp(0);
			char delMemberIdx = 0;
			kStream >> delMemberIdx;
			_DeleteMember( delMemberIdx );
		}
		break;
	}
}


