
#include "stdafx.h"
#include "TableString_Tamer.h"

BEGIN_NAME_TABLE


CSTable_Tamer::CSTable_Tamer()
{

}
CSTable_Tamer::~CSTable_Tamer()
{

}

void CSTable_Tamer::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"Tamer_Str.bin", this, &CSTable_Tamer::ReadItemData );
}

void CSTable_Tamer::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Tamer::ItemCont const& CSTable_Tamer::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Tamer::ReadItemData(DataStream& pReader)
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