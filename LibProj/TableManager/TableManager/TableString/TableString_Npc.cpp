
#include "stdafx.h"
#include "TableString_Npc.h"

BEGIN_NAME_TABLE


CSTable_Npc::CSTable_Npc()
{

}
CSTable_Npc::~CSTable_Npc()
{

}

void CSTable_Npc::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"Npc_Str.bin", this, &CSTable_Npc::ReadItemData );
}

void CSTable_Npc::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Npc::ItemCont const& CSTable_Npc::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Npc::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		sStrings str;
		pReader.pop( str.s_szPenName );
		pReader.pop( str.s_szName );
		pReader.pop( str.s_szTalk );
		m_kItems.insert(std::make_pair( dwKey, str ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE