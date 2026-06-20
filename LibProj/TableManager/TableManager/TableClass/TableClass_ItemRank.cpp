
#include "stdafx.h"
#include "TableClass_ItemRank.h"

BEGIN_NAME_TABLE

CItemRankTable::CItemRankTable()
{

}

CItemRankTable::~CItemRankTable()
{

}


void CItemRankTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"ItemRankData.bin", this, &CItemRankTable::ReadItemData );
	AddLoadTableEvent( L"ItemRankEffectData.bin", this, &CItemRankTable::ReadItemData_Effect );
}

void CItemRankTable::ReleaseTable()
{
	m_kItems.clear();
	m_kEffectItems.clear();
}

CItemRankTable::ItemCont const& CItemRankTable::GetItemRank() const
{ 
	return m_kItems;
}	

sItemRankData const* CItemRankTable::GetItemRank( DWORD const& dwItemID ) const
{
	ItemCont::const_iterator it = m_kItems.find( dwItemID );
	if( it == m_kItems.end() )
		return NULL;

	return &it->second;
}

bool CItemRankTable::IsRankItem( DWORD const& dwItemID ) const
{
	ItemCont::const_iterator it = m_kItems.find( dwItemID );
	if( it == m_kItems.end() )
		return false;
	return true;
}

int CItemRankTable::GetDropCount(DWORD const& dwItemID) const
{
	ItemCont::const_iterator it = m_kItems.find( dwItemID );
	if( it == m_kItems.end() )
		return 0;
	return it->second.s_nDorpCount;
}

sItemRankEffect const * CItemRankTable::GetRankInfo(DWORD const& dwItemCode, DWORD const&  dwRankItemCode) const
{
	RankItemCont::const_iterator it = m_kEffectItems.find( dwItemCode );
	if( it != m_kEffectItems.end() )
	{
		ChildCont::const_iterator itRank = it->second.begin();
		for( ; itRank != it->second.end(); ++itRank )
		{
			if( dwRankItemCode == itRank->dwItemCode )
				return &(*itRank);
		}
	}

	return NULL;
}

bool CItemRankTable::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwItemCode = 0;
		pReader.pop( dwItemCode );
		if( 0 == dwItemCode )
			break;

		sItemRankData rankItem;
		pReader.pop( rankItem.s_nDropType );
		pReader.pop( rankItem.s_nDorpCount );
		
		ItemCont::_Pairib rib = m_kItems.insert( std::make_pair(dwItemCode, rankItem) );
		if( !rib.second )
			_tprintf( _T("Error : Seam ItemCode : %d \n"), dwItemCode);
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CItemRankTable::ReadItemData_Effect(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwItemCode = 0;
		pReader.pop( dwItemCode );
		if( 0 == dwItemCode )
			break;

		ChildCont lists;
		do 
		{
			sItemRankEffect items; 
			pReader.pop( items.dwItemCode);
			if( 0 == items.dwItemCode )
				break;
			pReader.pop( items.dwIconNo);
			pReader.pop( items.nRank);
			lists.push_back( items );
		} while ( 1 );

		RankItemCont::_Pairib rib = m_kEffectItems.insert( std::make_pair(dwItemCode, lists) );
		if( !rib.second )
			_tprintf( _T("Error : Seam ItemCode : %d \n"), dwItemCode);
		bSuccess = pReader.MoveNext();
	}
	return true;
}

END_NAME_TABLE