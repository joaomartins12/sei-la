
#pragma once 

BEGIN_NAME_TABLE

class CCoolTimeTable : public IDataTableEvent
{
public:
	CCoolTimeTable();
	~CCoolTimeTable();

	typedef	std::map< DWORD, sItemCoolTime >			ItemCont;

	virtual void	InitTable(int const& nServiceType) override;
	virtual void	ReleaseTable() override;

private:
	bool			LoadData(DataStream& pReader);

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE