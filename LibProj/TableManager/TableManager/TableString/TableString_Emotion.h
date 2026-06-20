
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Emotion : public ITableEvent
{
public:
	typedef std::map< int, std::list<std::wstring> > ItemCont;

public:
	CSTable_Emotion();
	~CSTable_Emotion();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE