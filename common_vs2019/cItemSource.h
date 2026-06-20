

#pragma once


#include "pItem.h"
#include "pLength.h"
#include "pLimit.h"
#include "cItemData.h"
#include "nlib/base.h"

class cItemCreateMonster;

class cItemSource
{
public:
	cItemSource(void);
   ~cItemSource(void);

public:
	int get(uint nAttr);
	void set(uint nAttr, int nVal);

	enum eClass
	{
		C_PCBang, C_Cash,
	};
	bool IsClass( eClass c );				// 테이블의 Class 컬럼중 특정 Class 의 여부

public:
	uint GetIDX(void) { return m_nIDX; }		// 아이템 고유 번호
	char *GetName(void) { return m_szName; }

public:
	void SetIDX(uint idx) { m_nIDX = idx; }
	void SetName(char *name) { strcpy_s(m_szName, name); }

public: // item portal 관련 처리
	uint GetNextMapIDX(void) { return m_nNextMapIDX; }
	uint GetNextRX(void);
	uint GetNextRY(void);

public:
//#ifdef ITEM_TIME_PASS_ACTIVE
	bool IsTimePassActive() { return (m_nAttribute[nItem::TimePassActive] == 1) ? true : false;}	
	uint GetNoActiveEndTime() { return 0xFFFFFFF0; }
	//uint GetOverlapMaxCount(cItemData *itemdata = NULL);		// 중복카운트 변경(1고정이 아님, 기획데이터에서 1이상 변경가능)으로 안쓰임
//#endif

//////////////////////////////////////////////////////
// internal data
private:
	uint m_nIDX;
	char m_szName[64];

private:
	int m_nAttribute[nItem::MaxBaseAttribute];

//
private:
	u4 m_nBarterItemCode[ nLimit::Max_BarterCount ];
	u4 m_nBarterItemCount[ nLimit::Max_BarterCount ];

public:
	void SetBarterItemCode( u1 nPos, u4 nCode )		{ m_nBarterItemCode[ nPos ] = nCode; }
	void SetBarterItemCount( u1 nPos, u4 nCount )	{ m_nBarterItemCount[ nPos ] = nCount; }

	u4 GetBarterItemCode( u1 nPos )		{ return m_nBarterItemCode[ nPos ]; }
	u4 GetBarterItemCount( u1 nPos )	{ return m_nBarterItemCount[ nPos ]; }
	int GetAttribute(int iNo) { return m_nAttribute[iNo]; }

public: // item portal 관련 정보
	uint m_nNextMapIDX;
	uint m_nNextPosX;
	uint m_nNextPosY;
	uint m_nNextRadius; // 위 x,y위치로부터 해당 Radius내의 위치로 이동한다.

public:
	cItemCreateMonster *m_pCreateMonster;

};




