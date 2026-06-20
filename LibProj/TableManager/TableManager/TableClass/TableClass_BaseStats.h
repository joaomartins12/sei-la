#pragma once

BEGIN_NAME_TABLE

class CBaseStatsTable : public IDataTableEvent
{
public:
	CBaseStatsTable();
	~CBaseStatsTable();

	typedef std::map<int, std::map<int, sTamerBaseStats>>		TBASE_STATS;
	typedef std::map<int, std::map<int, sDigimonBaseStats>>	DBASE_STATS;
	
	virtual void	InitTable(int const& nServiceType) override;
	virtual void	ReleaseTable() override;

private:
	bool			_LoadData_Tamer(DataStream& pReader);
	bool			_LoadData_Digimon(DataStream& pReader);

private:
	TBASE_STATS		m_TamerBaseStats;
	DBASE_STATS		m_DigimonBaseStats;
};

END_NAME_TABLE
