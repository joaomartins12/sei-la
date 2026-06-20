
#pragma once 

BEGIN_NAME_TABLE

class CDigimonTable : public IDataTableEvent
{
public:
	CDigimonTable();
	~CDigimonTable();
	
	typedef std::map<DWORD, sDigimonData>	DigimonCont;

	virtual void		InitTable(int const& nServiceType) override;
	virtual void		ReleaseTable() override;

private:
	bool				_LoadData_Digimon(DataStream& pReader);
	bool				_InsertString(ITbImpl const* pData);

private:
	DigimonCont			m_kDigimons;
};

END_NAME_TABLE