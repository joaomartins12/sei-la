#pragma once

#include "common_vs2019/pItem.h"
#include "common_vs2019/Protocol/ItemScan_Result.h"
#include "common_vs2019/Protocol/ItemScan_Define.h"

class C2GS_SEND_ITEMSCAN
{
public:
	C2GS_SEND_ITEMSCAN() :nProtocol(pItem::ItemScan), uInvenActiveItemSlot(0), uNpcIDX(0), uInvenPos(0), u2ScanCount(0)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD nProtocol;
	bool bVIPMemberMode;
	u4 uInvenActiveItemSlot;	// 휴대용 스캐너 위치
	u4 uNpcIDX;
	u4 uInvenPos;				// 스캔할 아이템 인벤 위치
	u2 u2ScanCount;				// 스캔할 아이템 개수
};

class GS2C_RECV_ITEMSCAN
{
public:
	GS2C_RECV_ITEMSCAN() : nProtocol(pItem::ItemScan), nResult(eSCANRESULT::SUCCESS), uBeforeScanPos(0), uBeforeScanType(0), u2BeforeScanCount(0), uItemCount(0)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD nProtocol;
	n4 nResult;
	n8 n8ScanMoney;			// 스캔한 총비용
	n8 n8NowMoney;			// 스캔 후 현재 돈
	u4 uBeforeScanPos;		// 스캔한 아이템 위치
	u4 uBeforeScanType;		// 스캔한 아이템 번호
	u2 u2BeforeScanCount;	// 스캔한 아이템 개수

	u4 uItemCount;			// 획득한 아이템 종류 개수
	std::list< nsItemScan::sScanInfo >		lScanList;		// 획득한 아이템 리스트
};