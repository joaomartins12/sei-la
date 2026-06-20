#pragma once

BEGIN_NAME_TABLE

class CSTable_Achieve : public ITableEvent
{
public:
	struct sBody
	{
		std::wstring s_szName;
		std::wstring s_szTitle;
		std::wstring s_szComment;
	};

	typedef std::map<int, sBody>			ItemCont;
	typedef std::map<int, sAchieveTitle>	ItemCont_Title;

public:
	CSTable_Achieve();
	~CSTable_Achieve();

	virtual void			InitTable(int const& nServiceType) override;
	virtual void			ReleaseTable() override;

	sBody const*			GetItem( DWORD const& nKey ) const;
	ItemCont_Title const&	GetTitle() const;

private:
	bool					ReadItemData_Title( DataStream& pReader );
	bool					ReadItemData( DataStream& pReader );

private:
	ItemCont				m_kItems;
	ItemCont_Title			m_kAchieveTitle;
};


END_NAME_TABLE