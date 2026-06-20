
#include "stdafx.h"
#include "TableString_UIText.h"

BEGIN_NAME_TABLE


CSTable_UIText::CSTable_UIText()
{

}
CSTable_UIText::~CSTable_UIText()
{

}

void CSTable_UIText::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"UIText_Str.bin", this, &CSTable_UIText::ReadItemData );
}

void CSTable_UIText::ReleaseTable()
{
	m_kItems.clear();
}

void CSTable_UIText::DataSwap( ItemCont& Dist ) const
{
	m_kItems.swap( Dist );
}

//////////////////////////////////////////////////////////////////////////

bool CSTable_UIText::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		std::wstring uiKey;
		pReader.pop( uiKey );
		std::wstring value;
		pReader.pop( value );

		size_t hashKey = DmCS::GetHashCode( uiKey );
		m_kItems.insert( make_pair( hashKey, value ) );		
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE