

#pragma once


#include "pLength.h"
#include "pMonster.h"

#include "pLimit.h"

#include "nlib/base.h"


struct sDropItem
{
	sDropItem(){ m_nIDX = 0; m_nRate = 0; m_nCount = 0; }

	u4 m_nIDX;		// drop item type no
	u4 m_nRate;
	u4 m_nCount;
};


class cDigimonSource;
class cMonsterSource
{
public:
	cMonsterSource(void);
   ~cMonsterSource(void);
	
   void Release(void);

public:
	uint get(uint nAttr);
	void set(uint nAttr, uint nVal);

public:
	uint GetIDX(void) { return m_nIDX; }
	char *GetName(void) { return m_szName; }

public:
	void SetIDX(uint idx) { m_nIDX = idx; }
	void SetName(char *name) { strcpy_s(m_szName, name); }


//////////////////////////////////////////////////////
// internal data
private:
	uint m_nIDX;
	char m_szName[Language::pLength::MonsterName + 1];

protected:
	n4		m_nAttribute[nMonster::MaxAttribute];
public:
	n4*		GetAttributeArray(){ return m_nAttribute; }

public:
	uint m_nMaxDropItemCount;			// 일반 아이템의 드랍 최대 갯수 제한
	sDropItem m_pDropItem[nLimit::DropItem];
	sDropItem m_pDropQuestItem[nLimit::DropQuestItem];
	uint m_nMaxDropEventItemCount;		// 이벤트 아이템 드랍 최대 갯수
	sDropItem m_pDropEventItem[nLimit::DropEventItem];

public:
	cDigimonSource *m_pDigimonSource;

#ifdef LJS_NEW_RAID_REWARD
	u2 m_nRaidReward;
#endif

#ifdef LJS_NEW_ATTRIBUTE_SYSTEM
	u2 m_nNatualExp[2];	// MIN, MAX
#endif

#ifdef HSS_ADD_MONSTER_LIST_TABLE_BUFF_TYPE_20130115
	u2 m_nRaidBuffType;
	u4 m_nRaidBuffCode;	// 아이템리스트에서 버프에 관련된 것만을 사용하는 방식 // 추후 변경할 것
#endif

#ifdef HSS_NEW_CARD_MASTER_20131113
	u4 m_nMaxDropCardItemCount;
	sDropItem m_pDropCardItem[nLimit::DropCardItem];
#endif
};










