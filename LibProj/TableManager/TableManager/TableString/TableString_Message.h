
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Message : public ITableEvent
{
public:
	struct sStrings
	{
		std::wstring s_szTitle;
		std::wstring s_szMessage;

	};
	typedef std::map< DWORD, sStrings> ItemCont;

public:
	CSTable_Message();
	~CSTable_Message();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE