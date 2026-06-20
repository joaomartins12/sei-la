#pragma once

#include "Store_Define.h"

// 재구매 아이템 정보
struct  ST_RECV_ItemRepurchaseOpenInfo
{
public:
	ST_RECV_ItemRepurchaseOpenInfo() : m_nItemCount( 0 )
	{	
		m_ItemDataList.clear();
	}
public:
	void		AddItemData(const cItemData& rItemData)
	{
		m_ItemDataList.push_back( rItemData );
	}

public:
	u4						m_nItemCount;
	std::list< cItemData >	m_ItemDataList;
};

// 아이템 재구매 성공 확인 정보
struct ST_RECV_ItemRepurchaseSuccessInfo
{
public:
	struct sInvenInfo
	{
	public:
		u1		m_nInvenSlotNumber;
		u4		m_nItemTypeID;
		u2 		m_nInvenItemCount;
		u1		m_nRate;
		u1 		m_nLimited;
		u1		m_nLevel;										//2017-04-10-nova

		u2		m_nAccOption[nLimit::MAX_ACCESSORY_OPTION];		//2017-04-10-nova
		u2		m_nAccValues[nLimit::MAX_ACCESSORY_OPTION];		//2017-04-10-nova
		u4		m_nEndTime;

		sInvenInfo() : m_nInvenSlotNumber(0), m_nItemTypeID(0), m_nInvenItemCount(0), m_nRate(0), m_nLimited(0), m_nEndTime(0)
		{
			//2017-04-10-nova
			m_nLevel= 0;

			for(int i=0; i<nLimit::MAX_ACCESSORY_OPTION; ++i)
			{
				m_nAccOption[i] = 0;
				m_nAccValues[i] = 0;
			}
		}
	};

public:
	ST_RECV_ItemRepurchaseSuccessInfo() : m_nResult(0), m_CurrentMoney(0)
	{
		m_InvenInfoList.clear();
	}

	n1						m_nResult;
	n8						m_CurrentMoney;
	std::list<sInvenInfo>	m_InvenInfoList;
};

// 아이템 구매 성공 확인 정보
struct ST_RECV_ItemBuySuccessInfo
{
public:
	struct sInvenInfo
	{
	public:
		u1		m_nInvenSlotNumber;
		u4		m_nItemTypeID;
		u2 		m_nInvenItemCount;
		u1		m_nRate;
		u1 		m_nLimited;
		u4		m_nRemainTradeLimitTime;

		sInvenInfo() : m_nInvenSlotNumber(0), m_nItemTypeID(0), m_nInvenItemCount(0), m_nRate(0), m_nLimited(0),m_nRemainTradeLimitTime(0)
		{
		}
	};

public:
	ST_RECV_ItemBuySuccessInfo() : m_CurrentMoney(0)
	{
		m_InvenInfoList.clear();
	}

	n8						m_CurrentMoney;
	std::list<sInvenInfo>	m_InvenInfoList;
};

// 아이템 판매 성공 확인 정보
struct ST_RECV_ItemSellSuccessInfo
{
public:
	struct sInvenInfo
	{
	public:
		u1		m_nInvenSlotNumber;
		u4		m_nItemTypeID;
		u2 		m_nInvenItemCount;

		sInvenInfo() : m_nInvenSlotNumber(0), m_nItemTypeID(0), m_nInvenItemCount(0)
		{
		}
	};

public:
	ST_RECV_ItemSellSuccessInfo() : m_CurrentMoney(0)
	{
		m_InvenInfoList.clear();
	}

	n8						m_CurrentMoney;
	std::list<sInvenInfo>	m_InvenInfoList;
};