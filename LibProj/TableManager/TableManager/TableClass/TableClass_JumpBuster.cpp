
#include "stdafx.h"
#include "TableClass_JumpBuster.h"

BEGIN_NAME_TABLE

CJumpBusterTable::CJumpBusterTable()
{

}

CJumpBusterTable::~CJumpBusterTable()
{

}

void CJumpBusterTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"JumpBuster.bin", this, &CJumpBusterTable::_LoadData);
}

void CJumpBusterTable::ReleaseTable()
{
	m_Items.clear();
}

bool CJumpBusterTable::_LoadData(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		DWORD dwItemCode = 0;
		pReader.pop(dwItemCode);
		if (0 == dwItemCode)
			break;
		
		DWORD dwMapID = 0;
		pReader.pop(dwMapID);
		if( 0 == dwMapID)
			continue;

		m_Items.insert(std::make_pair(dwItemCode, dwMapID));
		bSuccess = pReader.MoveNext();
	}
	return true;
}

END_NAME_TABLE