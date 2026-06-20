
#include "stdafx.h"
#include "TableString_Guild.h"

BEGIN_NAME_TABLE


CSTable_Guild::CSTable_Guild()
{
}

CSTable_Guild::~CSTable_Guild()
{
}

void CSTable_Guild::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"Buff_Str.bin", this, &CSTable_Guild::_LoadData_GuildSkill);
}

void CSTable_Guild::ReleaseTable()
{
	m_kItems.clear();
}

CSTable_Guild::sItemString const* CSTable_Guild::GetItemString(DWORD const& dwItemCode) const
{
	ItemCont::const_iterator it = m_kItems.find(dwItemCode);
	if (it == m_kItems.end())
		return NULL;
	return &it->second;
}

//////////////////////////////////////////////////////////////////////////

bool CSTable_Guild::_LoadData_GuildSkill(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		DWORD dwKey = 0;
		pReader.pop(dwKey);

		sItemString str;
		pReader.pop(str.s_szName);
		pReader.pop(str.s_szComment);
		m_kItems.insert(std::make_pair(dwKey, str));
		bSuccess = pReader.MoveNext();
	}

	return true;
}

END_NAME_TABLE