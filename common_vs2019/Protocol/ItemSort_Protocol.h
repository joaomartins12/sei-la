#pragma once

#include "common_vs2019/pItem.h"
#include "common_vs2019/Protocol/ItemSort_Result.h"

namespace nItemSort
{
	enum eSortFlag
	{
		eINVEN = 0,
		eWAREHOUSE,
		eShareStash,
		eMAX,
	};
}

class C2GS_SEND_ITEMSORT
{
public:
	C2GS_SEND_ITEMSORT() :nProtocol(pItem::ItemSort), m_u1SortFlag(nItemSort::eINVEN)
	{
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD		nProtocol;
	u1          m_u1SortFlag; // 아이템 정렬을 인벤에서 할지, 창고에서 할지
};

class GS2C_RECV_ITEMSORT
{
public:
	GS2C_RECV_ITEMSORT() : nProtocol(pItem::ItemSort), m_u1SortFlag(nItemSort::eINVEN), m_nResult(eSORTRESULT::SUCCESS), m_u2ItemCount(0) {}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD		nProtocol;
	u1          m_u1SortFlag;	// 인벤 정렬인지, 창고 정렬인지
	n4          m_nResult;
	u2			m_u2ItemCount;	// cItemData 개수
	std::list< cItemData > m_lItems; // m_ppItem[nItem::TabInven], sizeof(cItemData) * m_u2ItemCount)
};