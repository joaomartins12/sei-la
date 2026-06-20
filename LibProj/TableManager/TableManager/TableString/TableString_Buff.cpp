
#include "stdafx.h"
#include "TableString_Buff.h"

BEGIN_NAME_TABLE


CSTable_Buff::CSTable_Buff()
{

}
CSTable_Buff::~CSTable_Buff()
{

}

void CSTable_Buff::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"Buff_Str.bin", this, &CSTable_Buff::ReadItemData );
}

void CSTable_Buff::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Buff::sStrings const* CSTable_Buff::GetItem(DWORD const& dwBuffCode) const
{
	ItemCont::const_iterator it = m_kItems.find(dwBuffCode);
	if (it == m_kItems.end())
		return NULL;
	return &it->second;
}


//////////////////////////////////////////////////////////////////////////

bool CSTable_Buff::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		sStrings str;
		pReader.pop( str.s_szName );
		pReader.pop( str.s_szComment );

		m_kItems.insert(std::make_pair( dwKey, str ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE