#pragma once

BEGIN_NAME_TABLE

class CSTable_Guild : public ITableEvent
{
public:
	struct sItemString
	{
		std::wstring s_szName;
		std::wstring s_szComment;
	};

public:
	CSTable_Guild();
	~CSTable_Guild();
	typedef std::map<DWORD, sItemString> ItemCont;

	virtual void			InitTable(int const& nServiceType) override;
	virtual void			ReleaseTable() override;

	sItemString const*		GetItemString(DWORD const& dwItemCode) const;

private:
	bool					_LoadData_GuildSkill(DataStream& pReader);

private:
	ItemCont			m_kItems;
};


END_NAME_TABLE
