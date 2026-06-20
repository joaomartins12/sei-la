
#include "stdafx.h"
#include "TableString_Tip.h"

BEGIN_NAME_TABLE


CSTable_Tip::CSTable_Tip()
{

}
CSTable_Tip::~CSTable_Tip()
{

}

void CSTable_Tip::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"Tip_Str.bin", this, &CSTable_Tip::ReadItemData );
}

void CSTable_Tip::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Tip::ItemCont const& CSTable_Tip::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Tip::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nKey = 0;
		pReader.pop( nKey );
		std::wstring value;
		pReader.pop( value );
		m_kItems.insert( make_pair( nKey, value ) );		
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE