
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Scene : public ITableEvent
{
public:
	typedef std::map< DWORD, std::wstring> ItemCont;

public:
	CSTable_Scene();
	~CSTable_Scene();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );
	bool			ReadItemData2(DataStream& pReader);

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE