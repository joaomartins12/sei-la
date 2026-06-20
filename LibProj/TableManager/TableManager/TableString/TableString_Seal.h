
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Seal : public ITableEvent
{
public:
	typedef std::map< DWORD, std::wstring> ItemCont;

public:
	CSTable_Seal();
	~CSTable_Seal();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE