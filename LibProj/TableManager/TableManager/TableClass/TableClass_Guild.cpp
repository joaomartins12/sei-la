
#include "stdafx.h"
#include "TableClass_Guild.h"

BEGIN_NAME_TABLE

CGuildTable::CGuildTable()
{

}

CGuildTable::~CGuildTable()
{

}

void CGuildTable::InitTable(int const& nServiceType)
{
	AddLoadTableEvent(L"GuildLevel.bin", this, &CGuildTable::_LoadData_GuildLv);
	AddLoadTableEvent(L"GuildBase.bin", this, &CGuildTable::_LoadData_GuildLimit);
	AddLoadTableEvent(L"GuildSkill.bin", this, &CGuildTable::_LoadData_GuildSkill);
	AddChangeStringEvent(ESTRING_TABLE_GUILD, this, &CGuildTable::_InsertString);
}

void CGuildTable::ReleaseTable()
{
	m_LvConts.clear();
}

bool CGuildTable::_InsertString(ITbImpl const* pData)
{
	CSTable_Guild const* pString = static_cast<CSTable_Guild const*>(pData);
	GUILDSKILL_CONTS::iterator it = m_SkillConts.begin();
	for (; it != m_SkillConts.end(); ++it)
	{
		std::map<USHORT, sGuildSkill >::iterator subIT = it->second.begin();
		for (; subIT != it->second.end(); ++subIT)
		{
			CSTable_Guild::sItemString const* pStr = pString->GetItemString(subIT->second.s_dwSkillCode);
			SAFE_POINTER_CON(pStr);

			subIT->second.s_szName = pStr->s_szName;
			subIT->second.s_szComment = pStr->s_szComment;
		}		
	}
	return true;
}

bool CGuildTable::_LoadData_GuildLv(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		USHORT nLv = 0;
		pReader.pop(nLv);
		if (0 == nLv)
			break;

		sGuildLv info;
		pReader.pop(info.s_nNeedFame);
		pReader.pop(info.s_nItemNo1);
		pReader.pop(info.s_nItemCount1);
		pReader.pop(info.s_nItemNo2);
		pReader.pop(info.s_nItemCount2);
		pReader.pop(info.s_nMaxGuildPerson);
		pReader.pop(info.s_nIncMember);
		pReader.pop(info.s_nMaxGuild2Master);
		pReader.pop(info.s_nMasterLevel);
		pReader.pop(info.s_nNeedPerson);

		m_LvConts.insert(std::make_pair(nLv, info));		
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CGuildTable::_LoadData_GuildLimit(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		USHORT limitlv = 0;
		pReader.pop(limitlv);
		if (0 != limitlv)
			m_Limit.s_nLimitLv = limitlv;

		UINT changeRate;
		pReader.pop(changeRate);
		if( 0 != changeRate)
			m_Limit.s_nChangeRate = changeRate;

		USHORT nGuildLv = 0;
		pReader.pop(nGuildLv);
		if (0 != nGuildLv)
		{
			UINT nReqExp = 0;
			pReader.pop(nReqExp);
			m_Limit.s_mapLevelInfo.insert(std::make_pair(nGuildLv, nReqExp));
		}

		USHORT pointType;
		pReader.pop(pointType);
		if (0 != pointType)
		{
			USHORT needPoint;
			pReader.pop(needPoint);
			m_Limit.s_mapPointInfo.insert(std::make_pair(pointType, needPoint));
		}
		bSuccess = pReader.MoveNext();
	}
	return true;
}

bool CGuildTable::_LoadData_GuildSkill(DataStream& pReader)
{
	bool bSuccess = pReader.MoveFirst();
	while (bSuccess)
	{
		USHORT nSkillType = 0;
		pReader.pop(nSkillType);

		USHORT nGroup = 0;
		pReader.pop(nGroup);
		GUILDSKILL_CONTS::iterator it = m_SkillConts.find(nGroup);
		if (it == m_SkillConts.end())
		{
			GUILDSKILL_CONTS::_Pairib rib  = m_SkillConts.insert(std::make_pair(nGroup, std::map<USHORT, sGuildSkill>()));
			it = rib.first;
		}

		USHORT nSkillLv = 0;
		pReader.pop(nSkillLv);

		sGuildSkill info;
		info.s_nSkillType = nSkillType;
		pReader.pop(info.s_nConsGsp);
		pReader.pop(info.s_nReqLevel);
		pReader.pop(info.s_dwSkillCode);
		it->second.insert(std::make_pair(nSkillLv, info));
		bSuccess = pReader.MoveNext();
	}
	return true;
}


END_NAME_TABLE
