
#include "stdafx.h"
#include "TableString_DigiTip.h"

BEGIN_NAME_TABLE


CSTable_DigiTip::CSTable_DigiTip()
{

}
CSTable_DigiTip::~CSTable_DigiTip()
{

}

void CSTable_DigiTip::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"DigimonTip_Str.bin", this, &CSTable_DigiTip::ReadItemData );
}

void CSTable_DigiTip::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_DigiTip::ItemCont const& CSTable_DigiTip::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_DigiTip::ReadItemData(DataStream& pReader)
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