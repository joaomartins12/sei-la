
#include "stdafx.h"
#include "TableString_Cashshop.h"

BEGIN_NAME_TABLE


CSTable_Cashshop::CSTable_Cashshop()
{

}
CSTable_Cashshop::~CSTable_Cashshop()
{

}

void CSTable_Cashshop::InitTable(int const& nServiceType)
{
	if( eSteam == nServiceType )
	{
		AddLoadTableEvent( L"cashshop_Steam_category_Str.bin", this, &CSTable_Cashshop::ReadItemData_Category );
		AddLoadTableEvent( L"Cashshop_Steam_Str.bin", this, &CSTable_Cashshop::ReadItemData );		
	}
	else
	{		
		AddLoadTableEvent( L"cashshop_category_Str.bin", this, &CSTable_Cashshop::ReadItemData_Category );
		AddLoadTableEvent( L"Cashshop_Str.bin", this, &CSTable_Cashshop::ReadItemData );
	}
}

void CSTable_Cashshop::ReleaseTable()
{
	m_kItems.clear();
	m_kItemCategory.clear();
}

std::wstring CSTable_Cashshop::GetCategoryNameMain(int const& nMain) const
{
	ItemCategory::const_iterator it = m_kItemCategory.find( nMain );
	if( it == m_kItemCategory.end() )
		return L"";
	return it->second.s_szName;
}

std::wstring CSTable_Cashshop::GetCategoryNameSub(int const& nMain, int const& nSub) const
{
	ItemCategory::const_iterator it = m_kItemCategory.find( nMain );
	if( it == m_kItemCategory.end() )
		return L"";
	std::map<int,std::wstring>::const_iterator subIT = it->second.s_mapSub.find( nSub );
	if( subIT == it->second.s_mapSub.end() )
		return L"";
	return subIT->second;
}

CSTable_Cashshop::ItemCont const& CSTable_Cashshop::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Cashshop::ReadItemData_Category( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int nMainType = 0;
		pReader.Read_Value( 0, nMainType );
		if( 0 == nMainType )
			break;

		sCategory categorys;
		pReader.Read_Value( 1, categorys.s_szName );
		m_kItemCategory.insert( std::make_pair( nMainType, categorys ) );
		bSuccess = pReader.MoveNext();
	}

	ItemCategory::iterator it = m_kItemCategory.begin();
	for( int nCol = 2; it != m_kItemCategory.end(); ++it )
	{
		// ╝м║лищ 
		bSuccess = pReader.MoveFirst();	
		while( bSuccess )
		{
			int nKey;
			pReader.Read_Value( nCol, nKey );
			if( 0 == nKey )
				break;

			std::wstring name;
			pReader.Read_Value( nCol + 1, name );

			it->second.s_mapSub.insert( std::make_pair( nKey, name ) );
			bSuccess = pReader.MoveNext();
		}
		nCol += 2;
	}

	return true;
}

bool CSTable_Cashshop::ReadItemData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwKey = 0;
		pReader.pop( dwKey );
		sMessage strings;
		pReader.pop( strings.s_szName );
		pReader.pop( strings.s_szComment );

		m_kItems.insert(std::make_pair( dwKey, strings ) );
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE