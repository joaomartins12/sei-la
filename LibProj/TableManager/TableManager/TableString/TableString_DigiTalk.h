
#pragma once 

BEGIN_NAME_TABLE

class CSTable_DigiTalk : public ITableEvent
{
public:
	struct sStrings
	{
		std::wstring s_szMessage;
		std::wstring s_szMessageTip;
	};
	typedef std::map< DWORD, sStrings > ItemCont;

public:
	CSTable_DigiTalk();
	~CSTable_DigiTalk();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE