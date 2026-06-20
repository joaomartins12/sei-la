
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Npc : public ITableEvent
{
public:
	struct sStrings
	{
		std::wstring s_szPenName;
		std::wstring s_szName;
		std::wstring s_szTalk;
	};
	typedef std::map< DWORD, sStrings> ItemCont;

public:
	CSTable_Npc();
	~CSTable_Npc();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE