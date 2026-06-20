
#include "stdafx.h"
#include "TableString_MapRegion.h"

BEGIN_NAME_TABLE


CSTable_MapRegion::CSTable_MapRegion()
{

}
CSTable_MapRegion::~CSTable_MapRegion()
{

}

void CSTable_MapRegion::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"MapRegion_Str.bin", this, &CSTable_MapRegion::ReadItemData );
}

void CSTable_MapRegion::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_MapRegion::ItemCont const& CSTable_MapRegion::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_MapRegion::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );

		ItemCont::iterator it = m_kItems.find( dwKey );
		if( it == m_kItems.end() )
		{
			ItemCont::_Pairib rib = m_kItems.insert( std::make_pair( dwKey, std::list<sRegion>() ) );
			it = rib.first;
		}

		sRegion value;
		pReader.pop( value.s_dwX );
		pReader.pop( value.s_dwY );
		pReader.pop( value.name );
		it->second.push_back( value );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE