
#pragma once 

BEGIN_NAME_TABLE

class CSTable_EventTalk : public ITableEvent
{
public:
	typedef std::map< int, std::wstring> ItemCont;

public:
	CSTable_EventTalk();
	~CSTable_EventTalk();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE