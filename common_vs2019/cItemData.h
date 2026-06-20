

#pragma once


#include "pLimit.h"
#include "nlib/base.h"

class cItemSlot // db io시 사용되는 정보
{
public:
	cItemSlot(void) : idx(0), maker(0) { }
   ~cItemSlot(void) { }

public:
	n8 idx;		// 해당 아이템의 db_idx
	n4 maker;	// 해당 아이템 생성자
};

#pragma pack(push)
#pragma pack(4)

class cItemData
{
public:
	cItemData(void) { Clear(); }
   ~cItemData(void) { }

public:
	uint GetType(void) const  { return m_nType; }
	uint GetCount(void) const { return m_nCount; }

	void SetType(uint nType);
	void SetCount(uint nCount);

	void Clear(void) { memset(this, 0, sizeof(cItemData)); }
	void Set( int nType, int nCount ){ m_nType = nType; m_nCount = nCount; m_nRate = 100; m_nLevel = 1; }
	void Set( int nType, int nCount, int nRate, int nLevel ){ m_nType = nType; m_nCount = nCount; m_nRate = nRate; m_nLevel = nLevel; }
	
public:
	//void operator=(cItemData &x) { memcpy(this, &x, sizeof(x)); }
	void operator=( cItemData const &x)
	{
		m_nAll = x.m_nAll;
		m_nRate = x.m_nRate;
		m_nLevel = x.m_nLevel;
		m_nLimited = x.m_nLimited;
		for( int i = 0; i < nLimit::SocketSlot; ++i )
		{
			m_nSockItemType[i] = x.m_nSockItemType[i];
			m_nSockAppRate[i] = x.m_nSockAppRate[i];
		}
		for( int i = 0; i < nLimit::MAX_ACCESSORY_OPTION; ++i )
		{
			m_nAccOption[i] = x.m_nAccOption[i];
			m_nAccValues[i] = x.m_nAccValues[i];
		}
		m_nEndTime = x.m_nEndTime;
		m_nRemainTradeLimitTime = x.m_nRemainTradeLimitTime;
#ifdef COMPAT_487
		ExtraBytes = x.ExtraBytes;
#endif
	}

public:
	bool IsNoActive() { return (m_nEndTime == 0xFFFFFFF0) ? true : false;}  
	bool IsActive() 
	{
		if (m_nEndTime != 0 && m_nEndTime != 0xFFFFFFFF && m_nEndTime != 0xFFFFFFF0)
			return true;
		else
			return false;
	}
	void SetEndTime(u4 time) { m_nEndTime = time; }
	u4   GetEndTime()        { return m_nEndTime;}

public: // 기본 정보
	union
	{
		struct
		{
			u4 m_nType      : 17; // tipo de item (igual a cType::m_nType), 0 não é usado.
			u4 m_nCount     : 15; // Número de itens da pilha
		};

		struct
		{
			u4              : 17;
			u4 m_nEndurance : 15; // 내구도
		};

		u4 m_nAll;
	};

public: // 추가 정보
	u1 m_nRate;		// 능력치 적용 비율
	u1 m_nLevel;	// 아이템 강화시 사용
	u1 m_nLimited;	// 귀속 여부, 1.소켓을 박으면 귀속, 2.획득시 귀속

	u2 m_nSockItemType[nLimit::SocketSlot]; // 박혀있는 소켓 아이템의 ItemIDX (dm_item_list.xls - Item - ItemNo), 1~65535
	u1 m_nSockAppRate[nLimit::SocketSlot];	// 적용 비율

//#ifdef JHB_ADD_ACCESSORYSYSTEM_20131028	
	u2 m_nAccOption[nLimit::MAX_ACCESSORY_OPTION];	//악세사리 옵션 8개
	u2 m_nAccValues[nLimit::MAX_ACCESSORY_OPTION];	//악세사리 값 8개
//#endif
public:
	u4 m_nEndTime;	// 초단위 처리
	u4 m_nRemainTradeLimitTime;

#ifdef COMPAT_487
	u8 ExtraBytes;
#endif

//	pragma pack(4) 관련 4byte 처리 고려
};
//#ifdef JHB_ADD_ACCESSORYSYSTEM_20131028
struct stAccessory
{
	u4 nSkillCode;		//악세사리 스킬코드
	u1 nGainCnt;		//옵션 갯수
	u1 nEnchantCnt;		//주문서 사용 카운트
	//u1 nOptionNo;		//1 공격력, 2 방어력, 3 최대HP, 4 최대 DS, 5 스킬뎀지증가,6 기본속성뎀지, 7 크리티컬
	u2 nOption[nLimit::MAX_ACCESSORY_EXCELOPTION];		//Accessory_Option 시트에서 옵션종류 읽어와서 저장
	u2 nMinValue[nLimit::MAX_ACCESSORY_EXCELOPTION];	//해당 옵션의 최소값
	u2 nMaxValue[nLimit::MAX_ACCESSORY_EXCELOPTION];	//해당 옵션의 최대값
};
struct stAccEnchant
{
	u4 nSkillCode;		//강화스톤 아이템 스킬 번호
	u1 nEnchantOption;	//인챈트 옵션 1디지터리파워, 2리뉴얼횟수, 3옵션변경, 4수치변경
	u2 SuccessPer;		//성공확률 - 만분율
	u2 SucValue;		//성공시 증가값
	u2 FailPer;			//실패확률 - 만분율
	u2 FailValue;		//실패시 하락값
};
//#endif
typedef struct _stItemInfo
{
	u2 slotNo;
	n4 slotType;
	u4 idx;
	u4 cnt;
} stItemInfo;

#pragma pack(pop)





