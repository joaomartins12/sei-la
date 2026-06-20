
#include "stdafx.h"
#include "TableString_Item.h"

BEGIN_NAME_TABLE


CSTable_Item::CSTable_Item()
{

}
CSTable_Item::~CSTable_Item()
{

}

void CSTable_Item::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"Item_Kor_Str.bin", this, &CSTable_Item::ReadItemData );
}

void CSTable_Item::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Item::sItemString const* CSTable_Item::GetItemString(DWORD const& dwItemCode) const
{
	ItemCont::const_iterator it = m_kItems.find(dwItemCode);
	if( it == m_kItems.end() )
		return NULL;
	return &it->second;
}

//////////////////////////////////////////////////////////////////////////

bool CSTable_Item::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		sItemString strings;
		pReader.pop( strings.name );
		pReader.pop( strings.typeComm );
		pReader.pop( strings.comment );

		m_kItems.insert(std::make_pair( dwKey, strings ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE