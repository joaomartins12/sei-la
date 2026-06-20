
#include "stdafx.h"
#include "TableClass_MapsInfo.h"

BEGIN_NAME_TABLE

CMapsTable::CMapsTable()
{

}

CMapsTable::~CMapsTable()
{

}

void CMapsTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"MapsInfo.bin", this, &CMapsTable::_LoadData_MapsInfo);
}

void CMapsTable::ReleaseTable()
{
	m_LimitInfos.clear();
}

bool CMapsTable::_LoadData_MapsInfo(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		DWORD nMapIdx = 0;
		pReader.pop(nMapIdx);
		if (0 == nMapIdx)
			break;
		sMapLimitInfo info;
		pReader.pop(info.s_nShoutSec);
		pReader.pop(info.s_bEnableCheckMacro);
		m_LimitInfos.insert(std::make_pair(nMapIdx, info));
		bSuccess = pReader.MoveNext();
	}
	return true;
}


END_NAME_TABLE