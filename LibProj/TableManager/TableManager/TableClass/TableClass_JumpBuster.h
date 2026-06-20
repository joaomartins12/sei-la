
#pragma once 

BEGIN_NAME_TABLE

class CJumpBusterTable : public IDataTableEvent
{
public:
	CJumpBusterTable();
	~CJumpBusterTable();

	typedef std::map<DWORD, DWORD>	JUMPBUSTER_CONT;

	virtual void		InitTable(int const& nServiceType) override;
	virtual void		ReleaseTable() override;
	
private:
	bool				_LoadData(DataStream& pReader);

private:
	JUMPBUSTER_CONT		m_Items;
};

END_NAME_TABLE
