
#include "stdafx.h"
#include "TableString_Monster.h"

BEGIN_NAME_TABLE


CSTable_Monster::CSTable_Monster()
{

}
CSTable_Monster::~CSTable_Monster()
{

}

void CSTable_Monster::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"MonsterName_Str.bin", this, &CSTable_Monster::ReadItemData );
}

void CSTable_Monster::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Monster::ItemCont const& CSTable_Monster::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Monster::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		sStrings str;
		pReader.pop( str.s_szName );
		pReader.pop( str.s_szTypename );
		m_kItems.insert(std::make_pair( dwKey, str ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE