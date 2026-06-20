#pragma once 

BEGIN_NAME_TABLE

class CGuildTable : public IDataTableEvent
{
public:
	CGuildTable();
	~CGuildTable();
	
	typedef std::map<USHORT, sGuildLv>						GUILDLV_CONTS;
	typedef std::map<USHORT, std::map<USHORT, sGuildSkill>>	GUILDSKILL_CONTS;

	virtual void	InitTable(int const& nServiceType) override;
	virtual void	ReleaseTable() override;

private:
	bool			_InsertString(ITbImpl const* pData);
	bool			_LoadData_GuildLv(DataStream& pReader);
	bool			_LoadData_GuildLimit(DataStream& pReader);
	bool			_LoadData_GuildSkill(DataStream& pReader);

private:
	GUILDSKILL_CONTS	m_SkillConts;
	GUILDLV_CONTS		m_LvConts;
	sGuildLimit			m_Limit;
};

END_NAME_TABLE
