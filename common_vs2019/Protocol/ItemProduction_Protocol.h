#pragma once

#include "ItemProduction_Define.h"   
#include "common_vs2019/pItem.h"
#include <minwindef.h>
#include <list>


class C2GS_SEND_ITEM_PRODUCTION
{
public:
	C2GS_SEND_ITEM_PRODUCTION()
	:nProtocol(pItem::Make) , nNpcIdx(0) , nMakeIDX(0)
	, nCount(0) , nRateItem(0) , nProtectItem(0)
	{
	}
	~C2GS_SEND_ITEM_PRODUCTION(){};

	WORD GetProtocol(){return nProtocol;}
public:
	WORD				nProtocol;

	u4 nNpcIdx;			//NPCIdx
	n4 nMakeIDX;		//제작IDX
	u2 nCount;			//몇번 시도

	n4 nRateItem;		//확률보정할경우 아이템IDX
	n4 nProtectItem;	//아이템 보호할경우 아이템IDX
};

class GS2C_RECV_ITEMPRODUCTION
{
public:
	GS2C_RECV_ITEMPRODUCTION()
	:nProtocol(pItem::Make),m_dwResult(0), m_sDwItemIdx(0), m_TotalMakedCount(0),m_MoneyResult(0),
	m_GetCount(0), m_nItemCount(0), m_nTradeLimitTime(0) , m_nDelItemCount(0), m_nIsItemProtect(0)
	{
		m_DelItemInfo.clear();
	}
	WORD GetProtocol(){return nProtocol;}
public:
	WORD				nProtocol;

	u4					m_dwResult;
	n4						m_sDwItemIdx;		// 만들어진 아이템 테이블 IDX , 항상 넣어서보낸다
	n4						m_TotalMakedCount;	// 제작 총 개수	 , 모두실패했을경우 0넣어보냄
	n4						m_nTradeLimitTime;	// 아이템 거래 제한 시간
	n8						m_MoneyResult;		// 실제 사용후 보유할 금액
	n4						m_GetCount;			// 획득한 개수( 성공 개수 )
	n4						m_nItemCount;		// 실제 아이템 개수
	n4						m_nDelItemCount;	// 삭제된 아이템 갯수
	std::list<sItemInfo>	m_DelItemInfo;
	n4						m_nIsItemProtect;	//아이템 프로텍터 사용시 해당아이템IDX
};