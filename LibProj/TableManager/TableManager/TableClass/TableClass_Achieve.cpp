
#include "stdafx.h"
#include "TableClass_Achieve.h"

BEGIN_NAME_TABLE

CAchieveTable::CAchieveTable()
{

}

CAchieveTable::~CAchieveTable()
{

}

void CAchieveTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"Achieve_Data.bin", this, &CAchieveTable::LoadData);
	AddChangeStringEvent( ESTRING_TABLE_ACHIEVE, this, &CAchieveTable::InsertString );
}

void CAchieveTable::ReleaseTable()
{
	m_kAchieveData.clear();
	m_kAchieveTitle.clear();
}

bool CAchieveTable::InsertString(ITbImpl const* pStringData)
{
	CSTable_Achieve const* pStrData = static_cast<CSTable_Achieve const*>(pStringData);

	// ¾÷Àû Category º¹»ç
	m_kAchieveTitle = pStrData->GetTitle();

	MAP_ACHIEVE_DATA::iterator it = m_kAchieveData.begin();
	for( ; it != m_kAchieveData.end(); ++it )
	{
		CSTable_Achieve::sBody const* pStr = pStrData->GetItem( it->first );
		if( NULL == pStr )
			continue;

		it->second.s_szName = pStr->s_szName;
		it->second.s_szTitle = pStr->s_szTitle;
		it->second.s_szComment = pStr->s_szComment;
	}
	return true;
}

bool CAchieveTable::IsAchieve(DWORD const& dwQuestID) const
{
	return ( m_kAchieveData.find( dwQuestID ) != m_kAchieveData.end() );
}

CAchieveTable::MAP_ACHIEVE_TITLE const& CAchieveTable::GetTitleData() const
{
	return m_kAchieveTitle;
}

CAchieveTable::MAP_ACHIEVE_DATA const& CAchieveTable::GetAchieveData() const
{
	return m_kAchieveData;
}

sAchieveData const* CAchieveTable::GetAchieve(DWORD const& dwQuestID) const
{
	MAP_ACHIEVE_DATA::const_iterator it = m_kAchieveData.find(dwQuestID);
	if( it == m_kAchieveData.end() )
		return NULL;
	
	return &it->second;
}

bool CAchieveTable::LoadData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		sAchieveData info;
		pReader.pop( info.s_nQuestID );
		pReader.pop( info.s_nIcon );
		pReader.pop( info.s_nPoint );
		pReader.pop( info.s_bDisplay );
		pReader.pop( info.s_nGroup );
		pReader.pop( info.s_nSubGroup );
		pReader.pop( info.s_nBuffCode );
		m_kAchieveData.insert( std::make_pair( info.s_nQuestID, info ) );
		bSuccess = pReader.MoveNext();
	}
	return true;
}
END_NAME_TABLE

