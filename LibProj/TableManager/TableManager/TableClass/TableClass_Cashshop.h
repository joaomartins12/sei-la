
#pragma once

BEGIN_NAME_TABLE

class CCashshopTable : public IDataTableEvent
{
public:
	CCashshopTable();
	~CCashshopTable();

	virtual void				InitTable(int const& nServiceType) override;
	virtual void				ReleaseTable() override;

	static	int					GetLargeType(DWORD const& dwProductID)		{ return dwProductID / 10000000; }			// 대분류 번호
	static	int					GetSelCashType(DWORD const& dwProductID)	{ return (dwProductID / 1000000) % 10; }	// 판매 캐쉬 타입
	static	int					GetSmallType(DWORD const& dwProductID)		{ return (dwProductID / 10000) %100; }

	MAP_CASHSHOP_CATEGORY const& GetCashShopInfo() const;
	LIST_CASHSHOP_WEBDATA const& GetCashshopWebData() const;

private:
	bool						LoadData_Category(DataStream& pReader);
	bool						LoadData_Main(DataStream& pReader);
	bool						LoadData_Best(DataStream& pReader);
	bool						LoadData_Items(DataStream& pReader);
	bool						LoadData_Weblink(DataStream& pReader);
	bool						InsertString(ITbImpl const* pData);
	
	void						_InsertItem( int const& LType, int const& SType, DWORD const& dwGroupID,  sCashshop_Item const & addItem );

	sCashshop_MainCategory &	_GetMainCategory( int const& nHitType );

private:
	MAP_CASHSHOP_CATEGORY		m_mapCashshopData;
	LIST_CASHSHOP_WEBDATA		m_mapWebData;
};

END_NAME_TABLE