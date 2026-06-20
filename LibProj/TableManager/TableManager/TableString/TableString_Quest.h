
#pragma once 

BEGIN_NAME_TABLE

class CSTable_Quest : public ITableEvent
{
public:
	struct sStrings
	{
		std::wstring s_szTitleTab;
		std::wstring s_szTitle;
		std::wstring s_szBody;
		std::wstring s_szSimple;
		std::wstring s_szHelper;
		std::wstring s_szProcess;
		std::wstring s_szCompleate;
		std::wstring s_szExpert;
	};
	typedef std::map< DWORD, sStrings> ItemCont;

public:
	CSTable_Quest();
	~CSTable_Quest();

	virtual void	InitTable(int const& nServiceType);
	virtual void	ReleaseTable();

	ItemCont const&	GetItem() const;

private:
	bool			ReadItemData( DataStream& pReader );

private:
	ItemCont		m_kItems;
};

END_NAME_TABLE