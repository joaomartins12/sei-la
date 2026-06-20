
#include "stdafx.h"
#include "TableClass_ItemCreation.h"

BEGIN_NAME_TABLE


CItemCreationTable::CItemCreationTable()
{

}
CItemCreationTable::~CItemCreationTable()
{

}

void CItemCreationTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"ItemCreation.bin", this, &CItemCreationTable::ReadCreationTable );
	AddLoadTableEvent( L"ItemCreationGroup.bin", this, &CItemCreationTable::ReadAssistGroupTable );
	AddChangeStringEvent( ESTRING_TABLE_ITEM_CREATION, this, &CItemCreationTable::_InsertString );
}

void CItemCreationTable::ReleaseTable()
{
	m_kItems.clear();
	m_kAssistGroup.clear();
}

bool CItemCreationTable::_InsertString(ITbImpl const* pData)
{
	CSTable_ItemCreation const* pString = static_cast<CSTable_ItemCreation const*>(pData);
	
	STL_MAP_ITEMPRODUCTION_IT it = m_kItems.begin();
	for( ; it != m_kItems.end(); ++it )
	{
		MAP_Main_Categoty_IT mainIT = it->second.m_mapMainCategoty.begin();
		for( ; mainIT != it->second.m_mapMainCategoty.end(); ++mainIT )
		{
			mainIT->second.m_wsCategotyName = pString->GetMainName( it->first, mainIT->first );
			MAP_Sub_Categoty_IT subIT = mainIT->second.m_mapSubCategoty.begin();
			for( ; subIT != mainIT->second.m_mapSubCategoty.end(); ++subIT )
				subIT->second.m_wsCategotyName = pString->GetSubName( it->first, mainIT->first, subIT->first );
		}
	}

	return true;
}	

MAP_Main_Categoty const* CItemCreationTable::GetItemProductionCategotyTable(DWORD const& dwNpcIdx) const
{
	STL_MAP_ITEMPRODUCTION_CIT it = m_kItems.find( dwNpcIdx );
	if( it == m_kItems.end() )
		return NULL;

	return &it->second.m_mapMainCategoty;
}

LIST_AssistItemList const* CItemCreationTable::GetAssistGroupItemList(int const& nGroupID) const
{
	MAP_AssistItemGroup_CIT it = m_kAssistGroup.find(nGroupID);
	if( it == m_kAssistGroup.end() )
		return NULL;
	
	return &(it->second.assisteItemList);
}

sAssistanceItemGroup const* CItemCreationTable::GetAssistItemGroup(int const& nGroupID) const
{
	MAP_AssistItemGroup_CIT it = m_kAssistGroup.find(nGroupID);
	if( it == m_kAssistGroup.end() )
		return NULL;

	return &(it->second);

}

STL_MAP_ITEMPRODUCTION const& CItemCreationTable::GetItemProductionTable() const
{
	return m_kItems;
}

bool CItemCreationTable::ReadCreationTable( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		DWORD dwNpcID = 0;
		pReader.pop( dwNpcID );

		int nUniqueIdx = 0;
		pReader.pop( nUniqueIdx );

		STL_MAP_ITEMPRODUCTION_IT it = m_kItems.find( dwNpcID );
		if( it == m_kItems.end() )
		{
			STL_MAP_ITEMPRODUCTION::_Pairib rib = m_kItems.insert( std::make_pair( dwNpcID, Tb::sItemProductionNpcTable(dwNpcID) ) );
			it = rib.first;
		}

		int nMainType = 0;
		pReader.pop( nMainType );

		MAP_Main_Categoty_IT mainIT = it->second.m_mapMainCategoty.find( nMainType );
		if( mainIT == it->second.m_mapMainCategoty.end() )
		{
			MAP_Main_Categoty::_Pairib rib = it->second.m_mapMainCategoty.insert( std::make_pair( nMainType, Tb::sMainCategoty(nMainType) ) );
			mainIT = rib.first;
		}

		int nSubType = 0;
		pReader.pop( nSubType );

		MAP_Sub_Categoty_IT subIT = mainIT->second.m_mapSubCategoty.find( nSubType );
		if( subIT ==  mainIT->second.m_mapSubCategoty.end() )
		{
			MAP_Sub_Categoty::_Pairib rib = mainIT->second.m_mapSubCategoty.insert( std::make_pair( nSubType, Tb::sSubCategoty(nSubType) ) );
			subIT = rib.first;
		}

		sProductionItemList itemInfo;
		itemInfo.m_nUniqueIdx = nUniqueIdx;					// 제작 고유 번호
		pReader.pop( itemInfo.m_dwItemIdx );				// 제작할 아이템 인덱스 번호
		pReader.pop( itemInfo.m_nItemNum );					// 제작하는 아이템의 갯수
		pReader.pop( itemInfo.m_nProbabilityofSuccess );	// 성공확률(만분률)
		pReader.pop( itemInfo.m_nPercentageAssistItemGroup);
		pReader.pop( itemInfo.m_nProtectAssistItemGroup);
		pReader.pop( itemInfo.m_dwItemCost );	// 제작 비용

		do 
		{
			sMaterialItemList materialItems;			// 재료 아이템 목록
			pReader.pop( materialItems.m_dwItemIdx );
			if( 0 == materialItems.m_dwItemIdx )
				break;
			pReader.pop( materialItems.m_nItemNum );
			itemInfo.m_MaterialList.push_back( materialItems );
		} while ( 1 );

		subIT->second.m_listProductionItems.push_back( itemInfo );
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CItemCreationTable::ReadAssistGroupTable( DataStream& pReader )
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		int	nIdx = 0;
		pReader.pop( nIdx );
		if( 0 == nIdx )
			break;

		MAP_AssistItemGroup_IT it = m_kAssistGroup.find( nIdx );
		if( it != m_kAssistGroup.end() )
			continue;
		else
		{
			MAP_AssistItemGroup::_Pairib rib = m_kAssistGroup.insert( std::make_pair( nIdx, sAssistanceItemGroup() ) );
			it = rib.first;
		}	


		pReader.pop( it->second.nAssistType );
		do 
		{
			sAssistanceItem itemInfo;
			pReader.pop( itemInfo.dwItemCode );
			pReader.pop( itemInfo.nSuccessPercentage );
			if( 0 == itemInfo.dwItemCode )
				break;
			it->second.assisteItemList.push_back( itemInfo );
		} while ( 1 );

		bSuccess = pReader.MoveNext();
	}
	return true;
}
END_NAME_TABLE