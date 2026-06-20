#pragma once

// 충전 아이템
struct sChargeItemInfo
{
	sChargeItemInfo() : nBitCode(0), nCashCode(0) {}
	~sChargeItemInfo() {}

	int				nBitCode;			// 비트 사용 충전 정보 인덱스
	int				nCashCode;			// 아이템 사용 정보 인덱스
};

typedef std::map< DWORD, sChargeItemInfo >	MAP_CHARGEITEM;
typedef MAP_CHARGEITEM::iterator			MAP_CHARGEITEM_IT;
typedef MAP_CHARGEITEM::const_iterator		MAP_CHARGEITEM_CIT;

// 비트 사용 충전 정보
struct sBitCodeInfo
{
	sBitCodeInfo() : nKeepTime(0), nChargeTime(0), dwKeepPrice(0) {}
	~sBitCodeInfo() {}

	int				nKeepTime;		// 보관 시간
	int				nChargeTime;	// 충전 시간
	DWORD			dwKeepPrice;	// 충전 비용
};

typedef std::map< int, sBitCodeInfo >	MAP_BITCODE;
typedef MAP_BITCODE::iterator			MAP_BITCODE_IT;
typedef MAP_BITCODE::const_iterator		MAP_BITCODE_CIT;

// 충전 일당 필요 아이템 개수
//			TimeIndex, NeedCount
typedef std::map< int, int >			MAP_NEEDINFO;
typedef MAP_NEEDINFO::iterator			MAP_NEEDINFO_IT;
typedef MAP_NEEDINFO::const_iterator	MAP_NEEDINFO_CIT;

// 아이템 사용 충전 정보
typedef std::map< int, MAP_NEEDINFO >	MAP_CASHCODE;
typedef MAP_CASHCODE::iterator			MAP_CASHCODE_IT;
typedef MAP_CASHCODE::const_iterator	MAP_CASHCODE_CIT;