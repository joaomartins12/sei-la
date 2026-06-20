
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Buff : public ITableEvent
{
public:
	struct sStrings
	{
		std::wstring s_szName;
		std::wstring s_szComment;
	};

	typedef std::map< DWORD, sStrings> ItemCont;

public:
	CSTable_Buff();
	~CSTable_Buff();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	sStrings const* GetItem(DWORD const& dwBuffCode) const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE