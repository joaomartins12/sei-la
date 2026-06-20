
#include "stdafx.h"
#include "TableString_Seal.h"

BEGIN_NAME_TABLE


CSTable_Seal::CSTable_Seal()
{

}
CSTable_Seal::~CSTable_Seal()
{

}

void CSTable_Seal::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"SealMasterName_Str.bin", this, &CSTable_Seal::ReadItemData );
}

void CSTable_Seal::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Seal::ItemCont const& CSTable_Seal::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Seal::ReadItemData(DataStream& pReader)
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