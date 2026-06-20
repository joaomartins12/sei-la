
#include "stdafx.h"
#include "TableString_Scene.h"

BEGIN_NAME_TABLE


CSTable_Scene::CSTable_Scene()
{

}
CSTable_Scene::~CSTable_Scene()
{

}

void CSTable_Scene::InitTable(int const& nServiceType)
{
	//AddLoadTableEvent( L"SceneInfo_Str.bin", this, &CSTable_Scene::ReadItemData );
	//AddLoadTableEvent( L"SceneData_Str.bin", this, &CSTable_Scene::ReadItemData2 );
}

void CSTable_Scene::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Scene::ItemCont const& CSTable_Scene::GetItem() const
{	
	return m_kItems;
}	

//////////////////////////////////////////////////////////////////////////

bool CSTable_Scene::ReadItemData(DataStream& pReader)
{
// 	bool bSuccess = pReader.MoveFirst();
// 	while( bSuccess )
// 	{
// 		DWORD dwKey = 0;
// 		pReader.pop( dwKey );
// 		std::wstring name;
// 		pReader.pop( name );
// 		m_kItems.insert( make_pair( dwKey, name ) );		
// 		bSuccess = pReader.MoveNext();
// 	}

	return true;
}

bool CSTable_Scene::ReadItemData2(DataStream& pReader)
{
// 	bool bSuccess = pReader.MoveFirst();
// 	while( bSuccess )
// 	{
// 		DWORD dwKey = 0;
// 		pReader.pop( dwKey );
// 		std::wstring name;
// 		pReader.pop( name );
// 		m_kItems.insert( make_pair( dwKey, name ) );		
// 		bSuccess = pReader.MoveNext();
// 	}

	return true;
}
END_NAME_TABLE