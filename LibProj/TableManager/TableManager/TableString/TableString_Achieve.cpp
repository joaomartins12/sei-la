#include "stdafx.h"
#include "TableString_Achieve.h"

BEGIN_NAME_TABLE

CSTable_Achieve::CSTable_Achieve()
{
}

CSTable_Achieve::~CSTable_Achieve()
{
}

void Tb::CSTable_Achieve::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"AchieveTitle_Str.bin", this, &CSTable_Achieve::ReadItemData_Title);
	AddLoadTableEvent(L"Achieve_Str.bin", this, &CSTable_Achieve::ReadItemData);
}

void CSTable_Achieve::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Achieve::sBody const* CSTable_Achieve::GetItem(DWORD const& nKey) const
{
	ItemCont::const_iterator it = m_kItems.find( nKey );
	if( it != m_kItems.end() )
		return &it->second;

	return 0;
}

CSTable_Achieve::ItemCont_Title const& CSTable_Achieve::GetTitle() const
{
	return m_kAchieveTitle;
}

bool CSTable_Achieve::ReadItemData_Title( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	// 대분류
	while( bSuccess )
	{
		int nKey;
		pReader.Read_Value( 0, nKey );
		if( 0 == nKey )
			break;

		ItemCont_Title::_Pairib rib = m_kAchieveTitle.insert( std::make_pair( nKey, sAchieveTitle() ) );
		pReader.Read_Value( 1, rib.first->second.name );
		bSuccess = pReader.MoveNext();
	}

	ItemCont_Title::iterator it = m_kAchieveTitle.begin();
	for( int nCol = 2; it != m_kAchieveTitle.end(); ++it )
	{
		// 소분류 
		bSuccess = pReader.MoveFirst();	
		while( bSuccess )
		{
			int nKey;
			pReader.Read_Value( nCol, nKey );
			if( 0 == nKey )
				break;

			std::wstring name;
			pReader.Read_Value( nCol + 1, name );

			it->second.m_mapSubTitle.insert( std::make_pair( nKey, name) );
			bSuccess = pReader.MoveNext();
		}

		nCol += 2;
	}
	return true;
}

bool CSTable_Achieve::ReadItemData( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwQuestIDX = 0;
		pReader.pop( dwQuestIDX );

		sBody info;
		pReader.pop( info.s_szName );
		pReader.pop( info.s_szTitle );
		pReader.pop( info.s_szComment );
		m_kItems.insert( std::make_pair( dwQuestIDX, info ) );
		bSuccess = pReader.MoveNext();
	}
	return true;
}


END_NAME_TABLE
