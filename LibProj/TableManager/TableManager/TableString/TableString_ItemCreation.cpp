
#include "stdafx.h"
#include "TableString_Item.h"

BEGIN_NAME_TABLE


CSTable_ItemCreation::CSTable_ItemCreation()
{

}
CSTable_ItemCreation::~CSTable_ItemCreation()
{

}

void CSTable_ItemCreation::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"ItemCreation_Str.bin", this, &CSTable_ItemCreation::ReadItemData );
}

void CSTable_ItemCreation::ReleaseTable()
{
	m_kItems.clear();
}

std::wstring CSTable_ItemCreation::GetMainName(DWORD const& dwNpcIdx, int const& nType) const
{
	NpcItemCon::const_iterator it = m_kItems.find(dwNpcIdx);
	if( it == m_kItems.end() )
		return L"";

	ItemCont::const_iterator it2 = it->second.find( nType );
	if( it2 == it->second.end() )
		return L"";

	return it2->second.name;
}

std::wstring CSTable_ItemCreation::GetSubName(DWORD const& dwNpcIdx, int const& nType, int const& subType) const
{
	NpcItemCon::const_iterator it = m_kItems.find(dwNpcIdx);
	if( it == m_kItems.end() )
		return L"";

	ItemCont::const_iterator it2 = it->second.find( nType );
	if( it2 == it->second.end() )
		return L"";

	std::map<int,std::wstring>::const_iterator subIT = it2->second.subType.find(subType);
	if( subIT == it2->second.subType.end() )
		return L"";
	return subIT->second;
}

//////////////////////////////////////////////////////////////////////////

bool CSTable_ItemCreation::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwNPCIDX = 0;
		pReader.pop( dwNPCIDX );
		NpcItemCon::iterator it = m_kItems.find( dwNPCIDX );
		if( it == m_kItems.end() )
		{
			NpcItemCon::_Pairib rib = m_kItems.insert( std::make_pair( dwNPCIDX, ItemCont() ) );
			it = rib.first;
		}

		int nKey = 0;
		pReader.pop( nKey );		
		ItemCont::iterator it2 = it->second.find( nKey );
		if( it2 == it->second.end() )
		{
			ItemCont::_Pairib rib = it->second.insert( std::make_pair( nKey, sItemString() ) );
			it2 = rib.first;
		}
		
		pReader.pop( it2->second.name );

		int nSubKey = 0;
		pReader.pop( nSubKey );
		std::wstring subName;
		pReader.pop( subName );
		
		it2->second.subType.insert( std::make_pair( nSubKey, subName ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE