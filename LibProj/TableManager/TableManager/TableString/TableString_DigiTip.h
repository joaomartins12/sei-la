
#pragma once 

BEGIN_NAME_TABLE

class CSTable_DigiTip : public ITableEvent
{
public:
	typedef std::map< int, std::wstring > ItemCont;

public:
	CSTable_DigiTip();
	~CSTable_DigiTip();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE