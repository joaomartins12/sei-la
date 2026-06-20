
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Tip : public ITableEvent
{
public:
	typedef std::map< int, std::wstring > ItemCont;

public:
	CSTable_Tip();
	~CSTable_Tip();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE