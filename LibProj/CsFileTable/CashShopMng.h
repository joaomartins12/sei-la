
#pragma once 

#include "CashShop.h"

class CsCashShopMng
{
public:
	enum eTableType
	{
		eDefault = 0,
		eSteam,
	};

	CsCashShopMng();
	~CsCashShopMng();
	
	bool		Init( char* cPath );
	void		SaveBin( char* cPath );
	void		Delete();

	static	int	GetLargeType(DWORD const& dwProductID)		{ return dwProductID / 10000000; }			// 대분류 번호
	static	int	GetSelCashType(DWORD const& dwProductID)	{ return (dwProductID / 1000000) % 10; }	// 판매 캐쉬 타입
	static	int	GetSmallType(DWORD const& dwProductID)		{ return (dwProductID / 10000) %100; }

	MAP_CASHSHOP_CATEGORY const* GetCashShopInfo(int const& nTableType = eDefault) const;
	LIST_CASHSHOP_WEBDATA const* GetCashshopWebData(int const& nTableType = eDefault) const;

private:
	bool		_Load_Excel(char const* pFilePath, int const& nTableType = eDefault);
	bool		_Load_Bin(char* cPath);
	bool		_Load_Pack(char* cPath);
	void		_Load_Excel_CashshopItems( char const* cFilePath, char const* cSheetName, MAP_CASHSHOP_CATEGORY & insertCategory );
	void		_Load_Excel_WebData( char const* cFilePath, char const* cSheetName, LIST_CASHSHOP_WEBDATA & insertCategory );

private:
	std::map<int/*캐쉬 샵 테이블*/, MAP_CASHSHOP_CATEGORY>		m_mapCashshopData;
	std::map<int/*캐쉬 샵 테이블*/, LIST_CASHSHOP_WEBDATA>		m_mapWebData;
};

