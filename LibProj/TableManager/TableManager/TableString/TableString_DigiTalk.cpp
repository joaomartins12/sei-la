
#include "stdafx.h"
#include "TableString_DigiTalk.h"

BEGIN_NAME_TABLE


CSTable_DigiTalk::CSTable_DigiTalk()
{

}
CSTable_DigiTalk::~CSTable_DigiTalk()
{

}

void CSTable_DigiTalk::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"DigimonTalk_Str.bin", this, &CSTable_DigiTalk::ReadItemData );
}

void CSTable_DigiTalk::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_DigiTalk::ItemCont const& CSTable_DigiTalk::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_DigiTalk::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD nKey = 0;
		pReader.pop( nKey );
		sStrings value;
		pReader.pop( value.s_szMessage );
		pReader.pop( value.s_szMessageTip );
		m_kItems.insert(std::make_pair( nKey, value ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE