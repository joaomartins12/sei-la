
#pragma once 

BEGIN_NAME_TABLE

class CSTable_DigiEvo : public ITableEvent
{
public:
	typedef std::map< USHORT, std::wstring> ItemCont;

public:
	CSTable_DigiEvo();
	~CSTable_DigiEvo();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();
	
	std::wstring	GetData(USHORT const& nKey) const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE