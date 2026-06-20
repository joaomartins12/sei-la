
#pragma once 

BEGIN_NAME_TABLE

class CExpTable : public IDataTableEvent
{
public:
	CExpTable();
	~CExpTable();

	typedef std::map<int, unsigned __int64>	ExpCont;

	virtual void		InitTable(int const& nServiceType) override;
	virtual void		ReleaseTable() override;

private:
	bool			_LoadData_Tamer(DataStream& pReader);
	bool			_LoadData_Digimon(DataStream& pReader);

private:
	ExpCont			m_kDigimonExp;
	ExpCont			m_kTamerExp;
};

END_NAME_TABLE
