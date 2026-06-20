#pragma once

BEGIN_NAME_TABLE

class CDEvolutionTable : public IDataTableEvent
{
public:
	CDEvolutionTable();
	~CDEvolutionTable();
	
	typedef std::map<USHORT/*Evo Type*/, sDigiEvoApplyData>	ItemConts;
	typedef std::map<DWORD, sDEvolutionInfo>	EvoConts;
	typedef std::map<USHORT, DWORD>				EvoGroupConts;

	virtual void		InitTable(int const& nServiceType) override;
	virtual void		ReleaseTable() override;

	std::wstring		GetEvoTypeName(USHORT const& nEvoType) const;
	int					GetEvoApplyPercent(USHORT const& nEvoType) const;

private:
	bool				_InsertString(ITbImpl const* pData);
	bool				_LoadData_BaseGroup(DataStream& pReader);
	bool				_LoadData_EvoApplyData(DataStream& pReader);
	bool				_LoadData_EvolutionList(DataStream& pReader);

private:
	ItemConts			m_EvolApply;
	EvoConts			m_EvoCont;
	EvoGroupConts		m_Group;
};

END_NAME_TABLE