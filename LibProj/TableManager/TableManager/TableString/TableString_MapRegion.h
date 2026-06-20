
#pragma once 

BEGIN_NAME_TABLE

class CSTable_MapRegion : public ITableEvent
{
public:
	struct sRegion
	{
		DWORD s_dwX;
		DWORD s_dwY;
		std::wstring name;
	};
	typedef std::map< DWORD, std::list<sRegion>> ItemCont;

public:
	CSTable_MapRegion();
	~CSTable_MapRegion();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE