
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Item : public ITableEvent
{
public:
	struct sItemString
	{
		std::wstring name;
		std::wstring typeComm;
		std::wstring comment;
	};

	typedef std::map< DWORD, sItemString > ItemCont;

public:
	CSTable_Item();
	~CSTable_Item();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	sItemString const* GetItemString( DWORD const& dwItemCode ) const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE