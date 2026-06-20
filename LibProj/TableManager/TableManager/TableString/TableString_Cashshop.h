
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Cashshop : public ITableEvent
{
public:
	struct sMessage
	{
		std::wstring s_szName;
		std::wstring s_szComment;
	};

	struct sCategory
	{
		std::wstring	s_szName;
		std::map<int,std::wstring>	s_mapSub;
	};

	typedef std::map<int,sCategory> ItemCategory;
	typedef std::map< DWORD,sMessage > ItemCont;

public:
	CSTable_Cashshop();
	~CSTable_Cashshop();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	std::wstring	GetCategoryNameMain(int const& nMain) const;
	std::wstring	GetCategoryNameSub(int const& nMain, int const& nSub) const;

	ItemCont const& GetItem() const;

private:
	bool			ReadItemData_Category( DataStream& pReader );
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
	ItemCategory	m_kItemCategory;
};

END_NAME_TABLE