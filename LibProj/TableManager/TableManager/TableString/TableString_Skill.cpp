
#include "stdafx.h"
#include "TableString_Skill.h"

BEGIN_NAME_TABLE


CSTable_Skill::CSTable_Skill()
{

}
CSTable_Skill::~CSTable_Skill()
{

}

void CSTable_Skill::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"Skill_Str.bin", this, &CSTable_Skill::ReadItemData );
}

void CSTable_Skill::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Skill::ItemCont const& CSTable_Skill::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Skill::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		sStrings str;
		pReader.pop( str.s_szName );
		pReader.pop( str.s_szComment );
		m_kItems.insert(std::make_pair( dwKey, str ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE