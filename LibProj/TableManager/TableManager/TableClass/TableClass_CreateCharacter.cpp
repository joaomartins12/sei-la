
#include "stdafx.h"
#include "TableClass_CreateCharacter.h"

BEGIN_NAME_TABLE


CCreateCharacterTable::CCreateCharacterTable()
{

}
CCreateCharacterTable::~CCreateCharacterTable()
{

}

void CCreateCharacterTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent( L"CreateList_Tamer.bin", this, &CCreateCharacterTable::LoadTableData_Tamer );
	AddLoadTableEvent( L"CreateList_Digimon.bin", this, &CCreateCharacterTable::LoadTableData_Digimon );
}

void CCreateCharacterTable::ReleaseTable()
{
	m_listTamerCreateInfos.clear();
	m_listDigimonCreateInfos.clear();
}


CCreateCharacterTable::LIST_DIGIMON_CREATE_INFO const& CCreateCharacterTable::GetDigimonCreateInfo() const
{
	return m_listDigimonCreateInfos;
}

CCreateCharacterTable::LIST_TAMER_CREATE_INFO const& CCreateCharacterTable::GetTamerCreateInfo() const
{
	return m_listTamerCreateInfos;
}

//////////////////////////////////////////////////////////////////////////

bool CCreateCharacterTable::LoadTableData_Tamer(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		sTamerCreateInfo addInfo;
		pReader.pop( addInfo.m_dwTalbeID );
		if( 0 == addInfo.m_dwTalbeID )
			break;

		pReader.pop( addInfo.m_bShow );
		pReader.pop( addInfo.m_bEnable );
		pReader.pop( addInfo.m_nSeasonType );
		pReader.pop( addInfo.m_sVoiceFile );
		pReader.pop( addInfo.m_nIconIdx );

		do 
		{
			DWORD dwCostumeID = 0;
			pReader.pop( dwCostumeID );
			if( 0 == dwCostumeID )
				break;
			addInfo.m_listCostume.push_back( dwCostumeID );
		} while ( 1 );

		m_listTamerCreateInfos.push_back(addInfo);
		bSuccess = pReader.MoveNext();
	}

	return true;
}

bool CCreateCharacterTable::LoadTableData_Digimon(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while( bSuccess )
	{
		sDigimonCreateInfo addInfo;
		pReader.pop( addInfo.m_dwTalbeID );
		if( 0 == addInfo.m_dwTalbeID )
			break;

		pReader.pop( addInfo.m_bShow );
		pReader.pop( addInfo.m_bEnable );
		pReader.pop( addInfo.m_sVoiceFile );
		m_listDigimonCreateInfos.push_back( addInfo );
		bSuccess = pReader.MoveNext();
	}

	return true;
}
END_NAME_TABLE