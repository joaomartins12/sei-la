
#include "stdafx.h"
#include "TableString_Map.h"

BEGIN_NAME_TABLE


CSTable_Map::CSTable_Map()
{

}
CSTable_Map::~CSTable_Map()
{

}

void CSTable_Map::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"MapName_Str.bin", this, &CSTable_Map::ReadItemData );
}

void CSTable_Map::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Map::ItemCont const& CSTable_Map::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Map::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		std::wstring name;
		pReader.pop( name );

		m_kItems.insert( make_pair( dwKey, name ) );		
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE