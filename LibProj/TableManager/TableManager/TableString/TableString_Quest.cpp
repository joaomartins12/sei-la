
#include "stdafx.h"
#include "TableString_Quest.h"

BEGIN_NAME_TABLE


CSTable_Quest::CSTable_Quest()
{

}
CSTable_Quest::~CSTable_Quest()
{

}

void CSTable_Quest::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"Quest_Str.bin", this, &CSTable_Quest::ReadItemData );
}

void CSTable_Quest::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Quest::ItemCont const& CSTable_Quest::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Quest::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		sStrings str;
		pReader.pop( str.s_szTitleTab );
		pReader.pop( str.s_szTitle );
		pReader.pop( str.s_szBody );
		pReader.pop( str.s_szSimple );
		pReader.pop( str.s_szHelper );
		pReader.pop( str.s_szProcess );
		pReader.pop( str.s_szCompleate );
		pReader.pop( str.s_szExpert );
		m_kItems.insert(std::make_pair( dwKey, str ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE