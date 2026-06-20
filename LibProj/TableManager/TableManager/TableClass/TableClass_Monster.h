
#pragma once 

BEGIN_NAME_TABLE

class CMonsterTable : public IDataTableEvent
{
public:
	CMonsterTable();
	~CMonsterTable();

	virtual void				InitTable(int const& nServiceType) override;
	virtual void				ReleaseTable() override;

private:

private:
};

END_NAME_TABLE