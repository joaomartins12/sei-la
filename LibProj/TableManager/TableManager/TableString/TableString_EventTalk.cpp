
#include "stdafx.h"
#include "TableString_EventTalk.h"

BEGIN_NAME_TABLE


CSTable_EventTalk::CSTable_EventTalk()
{

}
CSTable_EventTalk::~CSTable_EventTalk()
{

}

void CSTable_EventTalk::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"EventTalk_Str.bin", this, &CSTable_EventTalk::ReadItemData );
}

void CSTable_EventTalk::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_EventTalk::ItemCont const& CSTable_EventTalk::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_EventTalk::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nKey = 0;
		pReader.pop( nKey );
		std::wstring name;
		pReader.pop( name );

		m_kItems.insert( make_pair( nKey, name ) );		
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE