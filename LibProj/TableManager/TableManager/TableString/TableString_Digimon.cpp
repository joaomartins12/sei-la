
#include "stdafx.h"
#include "TableString_Digimon.h"

BEGIN_NAME_TABLE


CSTable_Digimon::CSTable_Digimon()
{

}
CSTable_Digimon::~CSTable_Digimon()
{

}

void CSTable_Digimon::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"DigimonName_Str.bin", this, &CSTable_Digimon::ReadItemData );
}

void CSTable_Digimon::ReleaseTable()
{
	m_kItems.clear();
}

std::wstring CSTable_Digimon::GetName( DWORD const& dwDigimonID ) const
{
	ItemCont::const_iterator it = m_kItems.find(dwDigimonID);
	if( it == m_kItems.end() )
		return L"";
	return it->second;
}

CSTable_Digimon::ItemCont const& CSTable_Digimon::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Digimon::ReadItemData(DataStream& pReader)
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