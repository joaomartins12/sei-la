
#include "stdafx.h"
#include "TableClass_Exp.h"

BEGIN_NAME_TABLE

CExpTable::CExpTable()
{

}

CExpTable::~CExpTable()
{

}

void CExpTable::InitTable(int const& nServiceType)
{
 	AddLoadTableEvent(L"DigimonExp.bin", this, &CExpTable::_LoadData_Tamer);
	AddLoadTableEvent(L"TamerExp.bin", this, &CExpTable::_LoadData_Digimon);
}

void CExpTable::ReleaseTable()
{
	m_kDigimonExp.clear();
	m_kTamerExp.clear();
}

bool CExpTable::_LoadData_Tamer(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		int nLv = 0;
		pReader.pop(nLv);
		if (0 == nLv)
			break;

		unsigned __int64 ui64Exp = 0;
		pReader.pop(ui64Exp);

		m_kTamerExp.insert(std::make_pair(nLv, ui64Exp));
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CExpTable::_LoadData_Digimon(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		int nLv = 0;
		pReader.pop(nLv);
		if (0 == nLv)
			break;

		unsigned __int64 ui64Exp = 0;
		pReader.pop(ui64Exp);

		m_kDigimonExp.insert(std::make_pair(nLv, ui64Exp));
		bSuccess = pReader.MoveNext();
	}
	return true;
}

END_NAME_TABLE