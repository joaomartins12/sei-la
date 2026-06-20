
#include "stdafx.h"
#include "TableString_Riding.h"

BEGIN_NAME_TABLE


CSTable_Riding::CSTable_Riding()
{

}
CSTable_Riding::~CSTable_Riding()
{

}

void CSTable_Riding::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"DigimonRidingComment_Str.bin", this, &CSTable_Riding::ReadItemData );
}

void CSTable_Riding::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Riding::ItemCont const& CSTable_Riding::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Riding::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		std::wstring comment;
		pReader.pop( comment );
		m_kItems.insert( make_pair( dwKey, comment ) );		
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE