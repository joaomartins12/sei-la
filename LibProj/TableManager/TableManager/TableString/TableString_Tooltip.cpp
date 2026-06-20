
#include "stdafx.h"
#include "TableString_ToolTip.h"

BEGIN_NAME_TABLE


CSTable_ToolTip::CSTable_ToolTip()
{

}
CSTable_ToolTip::~CSTable_ToolTip()
{

}

void CSTable_ToolTip::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"ToolTip_Str.bin", this, &CSTable_ToolTip::ReadItemData );
}

void CSTable_ToolTip::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_ToolTip::ItemCont const& CSTable_ToolTip::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_ToolTip::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		sStrings value;
		pReader.pop( value.s_szTitle );
		pReader.pop( value.s_szMessage );
		m_kItems.insert( std::make_pair( dwKey, value ) );		
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE