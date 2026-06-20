
#include "stdafx.h"
#include "TableString_Message.h"

BEGIN_NAME_TABLE


CSTable_Message::CSTable_Message()
{

}
CSTable_Message::~CSTable_Message()
{

}

void CSTable_Message::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"Message_Str.bin", this, &CSTable_Message::ReadItemData );
}

void CSTable_Message::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Message::ItemCont const& CSTable_Message::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Message::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		sStrings str;
		pReader.pop( str.s_szTitle );
		pReader.pop( str.s_szMessage );
		m_kItems.insert(std::make_pair( dwKey, str ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE