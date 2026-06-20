
#pragma once

BEGIN_NAME_TABLE

class CBuffTable : public IDataTableEvent
{
public:
	typedef std::map<DWORD, sBuffInfo>	BuffCont;

public:
	CBuffTable();
	~CBuffTable();

	virtual void				InitTable(int const& nServiceType) override;
	virtual void				ReleaseTable() override;

private:
	bool						_LoadData_Buff(DataStream& pReader);
	bool						_InsertString(ITbImpl const* pStringData);

private:
	BuffCont	m_BuffCont;
};

END_NAME_TABLE