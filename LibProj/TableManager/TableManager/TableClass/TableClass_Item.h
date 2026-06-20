
#pragma once 

BEGIN_NAME_TABLE

class CItemTable : public IDataTableEvent
{
public:
	CItemTable();
	~CItemTable();

	typedef std::map<DWORD,sItemData>		ItemCont;
	typedef std::map<DWORD, std::wstring>	AuraEffectCont;

	virtual void	InitTable(int const& nServiceType) override;
	virtual void	ReleaseTable() override;

private:
	bool			ReadItemData(DataStream& pReader);
	bool			ReadItemData_AuraEffect(DataStream& pReader);
	bool			InsertString(ITbImpl const* pData);

private:
	ItemCont		m_kItems;
	AuraEffectCont	m_kEffect;
};

END_NAME_TABLE