
#include "stdafx.h"
#include "TableClass_DSkillLevelUp.h"

BEGIN_NAME_TABLE

CDSkillLvUpTable::CDSkillLvUpTable()
{

}
CDSkillLvUpTable::~CDSkillLvUpTable()
{

}

void CDSkillLvUpTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"DSkillLv_Start.bin", this, &CDSkillLvUpTable::_LoadData_SkillLvStart);
	AddLoadTableEvent(L"DSkillLv_Open.bin", this, &CDSkillLvUpTable::_LoadData_SkillLvOpen);
}

void CDSkillLvUpTable::ReleaseTable()
{
	m_MaxLvCont.clear();
}

bool CDSkillLvUpTable::_LoadData_SkillLvStart(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		USHORT nEvoType = 0;
		pReader.pop(nEvoType);
		if (0 == nEvoType)
			break;
		sEvoMaxkSkillLv info;
		pReader.pop(info.s_SkillExpStartLv);
		while (1)
		{
			USHORT nMaxLv = 0;
			pReader.pop(nMaxLv);
			if (0 == nMaxLv)
				break;
			info.s_SkillMaxLvs.push_back(nMaxLv);
		}
		m_MaxLvCont.insert(std::make_pair(nEvoType, info));		
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CDSkillLvUpTable::_LoadData_SkillLvOpen(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		UINT itemType = 0;
		pReader.pop(itemType);
		if (0 == itemType)
			break;

		sExpansionCondition info;
		pReader.pop(info.nExpansionRank);
		while (true)
		{
			USHORT evoType;
			pReader.pop(evoType);
			if (0 == evoType)
				break;
			info.digimonEvoTypes.push_back(evoType);
		}

		m_LvOpenCont.insert(std::make_pair(itemType, info));
		bSuccess = pReader.MoveNext();
	}
	return true;
}
END_NAME_TABLE