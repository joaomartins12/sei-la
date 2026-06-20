
#pragma once 

BEGIN_NAME_TABLE

class CSTable_ItemCreation : public ITableEvent
{
public:
	struct sItemString
	{
		std::wstring name;
		std::map<int,std::wstring> subType;
	};

	typedef std::map< int, sItemString > ItemCont;

	typedef std::map<DWORD, ItemCont>	NpcItemCon;

public:
	CSTable_ItemCreation();
	~CSTable_ItemCreation();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	std::wstring	GetMainName( DWORD const& dwNpciDX, int const& nType ) const;
	std::wstring	GetSubName( DWORD const& dwNpciDX, int const& nType, int const& subType ) const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	NpcItemCon		m_kItems;
};

END_NAME_TABLE