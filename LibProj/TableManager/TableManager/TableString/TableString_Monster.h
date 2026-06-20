
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Monster : public ITableEvent
{
public:
	struct sStrings
	{
		std::wstring s_szName;
		std::wstring s_szTypename;

	};
	typedef std::map< DWORD, sStrings> ItemCont;

public:
	CSTable_Monster();
	~CSTable_Monster();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE