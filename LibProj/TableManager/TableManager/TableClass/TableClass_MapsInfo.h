#pragma once

BEGIN_NAME_TABLE

class CMapsTable : public IDataTableEvent
{
public:
	CMapsTable();
	~CMapsTable();

	typedef std::map<DWORD, sMapLimitInfo>		LIMITINFO_CONT;

	virtual void	InitTable(int const& nServiceType) override;
	virtual void	ReleaseTable() override;

private:
	bool			_LoadData_MapsInfo(DataStream& pReader);

private:
	LIMITINFO_CONT	m_LimitInfos;
};

END_NAME_TABLE

