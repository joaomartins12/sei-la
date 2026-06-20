
#include "stdafx.h"
#include "TableClass_BaseStats.h"

BEGIN_NAME_TABLE

CBaseStatsTable::CBaseStatsTable()
{

}

CBaseStatsTable::~CBaseStatsTable()
{

}

void CBaseStatsTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"TamerStats.bin", this, &CBaseStatsTable::_LoadData_Tamer);
 	AddLoadTableEvent(L"DigimonStats.bin", this, &CBaseStatsTable::_LoadData_Digimon);
}

void CBaseStatsTable::ReleaseTable()
{
	m_TamerBaseStats.clear();
	m_DigimonBaseStats.clear();
}

bool CBaseStatsTable::_LoadData_Tamer(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		int nLv = 0;
		pReader.pop(nLv);
		if (0 == nLv)
			break;

		TBASE_STATS::_Pairib rib =  m_TamerBaseStats.insert(std::make_pair(nLv, std::map<int, sTamerBaseStats>()));
		
		for (int nType = eTamerType::eMasaru; nType < eTamerType::eMax; ++nType)
		{
			std::map<int, sTamerBaseStats >::_Pairib subrib = rib.first->second.insert(std::make_pair(nType, sTamerBaseStats()));
			for (int nStats = eTBaseStats::eTamer_Hp; nStats < eTBaseStats::eTamer_Max; ++nStats)
			{
				pReader.pop(subrib.first->second.nHP);
				pReader.pop(subrib.first->second.nDS);
				pReader.pop(subrib.first->second.nMS);
				pReader.pop(subrib.first->second.nDE);
				pReader.pop(subrib.first->second.nAT);
				pReader.pop(subrib.first->second.nHT);				
			}
		}
		
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CBaseStatsTable::_LoadData_Digimon(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		int nLv = 0;
		pReader.pop(nLv);
		if (0 == nLv)
			break;

		DBASE_STATS::_Pairib rib = m_DigimonBaseStats.insert(std::make_pair(nLv, std::map<int, sDigimonBaseStats>()));

		for (int nType = eDAttackType::eATT_Short; nType < eDAttackType::eATT_Max; ++nType)
		{
			std::map<int, sDigimonBaseStats >::_Pairib subrib = rib.first->second.insert(std::make_pair(nType, sDigimonBaseStats()));
			for (int nStats = eDBaseStats::eDigimon_Hp; nStats < eDBaseStats::eDigimon_Max; ++nStats)
			{
				pReader.pop(subrib.first->second.nHP);
				pReader.pop(subrib.first->second.nDS);
				pReader.pop(subrib.first->second.nDE);
				pReader.pop(subrib.first->second.nEV);
				pReader.pop(subrib.first->second.nCR);
				pReader.pop(subrib.first->second.nAT);
				pReader.pop(subrib.first->second.nHT);
			}
		}

		bSuccess = pReader.MoveNext();
	}
	return true;
}


END_NAME_TABLE