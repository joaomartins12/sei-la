
#include "stdafx.h"
#include "TableClass_ItemExchange.h"

BEGIN_NAME_TABLE


CItemExchangeTable::CItemExchangeTable()
{

}
CItemExchangeTable::~CItemExchangeTable()
{

}

void CItemExchangeTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"ItemExchangeData.bin", this, &CItemExchangeTable::ReadItemData );
}

void CItemExchangeTable::ReleaseTable()
{
	m_kItems.clear();
}

sItemExchange const* CItemExchangeTable::GetExchangeItem( DWORD const& dwNpcID, DWORD const& dwItemID ) const
{
	ItemCont::const_iterator it = m_kItems.find(dwNpcID);
	if( it == m_kItems.end() )
		return NULL;

	ItemListCont::const_iterator subIT = it->second.begin();
	for( ; subIT != it->second.end(); ++it )
	{
		if( (*subIT).s_dwItemID == dwItemID )
			return &(*subIT);
	}

	return NULL;
}


bool CItemExchangeTable::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwNpcIDX = 0;
		pReader.pop( dwNpcIDX );
		if( 0 == dwNpcIDX )
			break;

		ItemCont::iterator it = m_kItems.find( dwNpcIDX );
		if( it == m_kItems.end() )
		{
			ItemCont::_Pairib rib = m_kItems.insert( std::make_pair( dwNpcIDX, ItemListCont()) );
			it = rib.first;
		}

		sItemExchange sExchange;
		pReader.pop( sExchange.s_dwItemIndex );
		pReader.pop( sExchange.s_dwItemID );

		pReader.pop( sExchange.s_dwExchange_Code_A );
		if( sExchange.s_dwExchange_Code_A != 0 )
			sExchange.s_dwCount++;

		pReader.pop( sExchange.s_dwPropertyA_Price );
		pReader.pop( sExchange.s_dwExchange_Code_B );
		if( sExchange.s_dwExchange_Code_B != 0 )
			sExchange.s_dwCount++;

		pReader.pop( sExchange.s_dwPropertyB_Price );
		pReader.pop( sExchange.s_dwExchange_Code_C );
		if( sExchange.s_dwExchange_Code_C != 0 )
			sExchange.s_dwCount++;

		pReader.pop( sExchange.s_dwPropertyC_Price );
		pReader.pop( sExchange.s_dwExchange_Code_D );
		if( sExchange.s_dwExchange_Code_D != 0 )
			sExchange.s_dwCount++;

		pReader.pop( sExchange.s_dwPropertyD_Price );
		it->second.push_back( sExchange );
		bSuccess = pReader.MoveNext();
	}
	return true;
}

END_NAME_TABLE