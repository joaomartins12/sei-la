
#include "stdafx.h"
#include "TableString_DigiEvo.h"

BEGIN_NAME_TABLE


CSTable_DigiEvo::CSTable_DigiEvo()
{

}
CSTable_DigiEvo::~CSTable_DigiEvo()
{

}

void CSTable_DigiEvo::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"DigimonEvoName_Str.bin", this, &CSTable_DigiEvo::ReadItemData );
}

void CSTable_DigiEvo::ReleaseTable()
{
	m_kItems.clear();
}

std::wstring Tb::CSTable_DigiEvo::GetData(USHORT const& nKey) const
{
	ItemCont::const_iterator it = m_kItems.find(nKey);
	if (it == m_kItems.end())
		return L"";
	return it->second;
}

//////////////////////////////////////////////////////////////////////////

bool CSTable_DigiEvo::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		USHORT nKey = 0;
		pReader.pop( nKey );
		std::wstring name;
		pReader.pop( name );

		m_kItems.insert( make_pair( nKey, name ) );		
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE