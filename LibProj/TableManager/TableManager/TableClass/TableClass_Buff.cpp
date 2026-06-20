
#include "stdafx.h"
#include "TableClass_Buff.h"

BEGIN_NAME_TABLE

CBuffTable::CBuffTable()
{

}

CBuffTable::~CBuffTable()
{

}

void CBuffTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"Buff_Data.bin", this, &CBuffTable::_LoadData_Buff);
	AddChangeStringEvent(ESTRING_TABLE_BUFF, this, &CBuffTable::_InsertString);
}

void CBuffTable::ReleaseTable()
{
	m_BuffCont.clear();
}

bool CBuffTable::_InsertString(ITbImpl const* pStringData)
{
	CSTable_Buff const* pData = static_cast<CSTable_Buff const*>(pStringData);
	BuffCont::iterator it = m_BuffCont.begin();
	for (; it != m_BuffCont.end(); ++it)
	{
		CSTable_Buff::sStrings const* pStr = pData->GetItem(it->first);
		SAFE_POINTER_CON(pStr);
		it->second.s_szName = pStr->s_szName;
		it->second.s_szComment = pStr->s_szComment;
	}
	return true;
}

bool CBuffTable::_LoadData_Buff(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		sBuffInfo info;
		pReader.pop(info.s_dwID);
		if (0 == info.s_dwID)
			break;

		pReader.pop(info.s_nBuffIcon);
		pReader.pop(info.s_nBuffType);
		pReader.pop(info.s_nBuffLifeType);
		pReader.pop(info.s_nBuffTimeType);
		pReader.pop(info.s_nMinLv);
		pReader.pop(info.s_nBuffClass);
		pReader.pop(info.s_dwSkillCode);
		pReader.pop(info.s_dwDigimonSkillCode);
		pReader.pop(info.s_bDelete);
		pReader.pop(info.s_szEffectFile);
		pReader.pop(info.s_nConditionLv);

		m_BuffCont.insert(std::make_pair(info.s_dwID, info));
		bSuccess = pReader.MoveNext();
	}
	return true;
}

END_NAME_TABLE

