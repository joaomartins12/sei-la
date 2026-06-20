

#include "cMonsterSource.h"


cMonsterSource::cMonsterSource(void)
{
	m_nIDX = 0;
	ZeroMemory( m_szName, sizeof(m_szName) );
	
	memset(m_nAttribute, 0, sizeof(m_nAttribute));

	m_nMaxDropItemCount = 0;
	ZeroMemory( m_pDropItem, sizeof(m_pDropItem) );
	ZeroMemory( m_pDropQuestItem, sizeof(m_pDropQuestItem) );
	m_nMaxDropEventItemCount = 0;
	ZeroMemory( m_pDropEventItem, sizeof(m_pDropEventItem) );

	m_pDigimonSource = NULL;

#ifdef LJS_NEW_RAID_REWARD
	m_nRaidReward = 0;
#endif

#ifdef LJS_NEW_ATTRIBUTE_SYSTEM
	ZeroMemory( m_nNatualExp, sizeof(m_nNatualExp) );
#endif

#ifdef HSS_ADD_MONSTER_LIST_TABLE_BUFF_TYPE_20130115
	m_nRaidBuffType = 0;
	m_nRaidBuffCode = 0;
#endif

#ifdef HSS_NEW_CARD_MASTER_20131113
	m_nMaxDropCardItemCount = 0;
	ZeroMemory( m_pDropCardItem, sizeof(m_pDropCardItem) );	
#endif
}


cMonsterSource::~cMonsterSource(void)
{
	Release();
}

void cMonsterSource::Release(void)
{
	m_nIDX = 0;
	ZeroMemory( m_szName, sizeof(m_szName) );

	memset(m_nAttribute, 0, sizeof(m_nAttribute));

	m_nMaxDropItemCount = 0;
	ZeroMemory( m_pDropItem, sizeof(m_pDropItem) );
	ZeroMemory( m_pDropQuestItem, sizeof(m_pDropQuestItem) );
	m_nMaxDropEventItemCount = 0;
	ZeroMemory( m_pDropEventItem, sizeof(m_pDropEventItem) );

	m_pDigimonSource = NULL;

#ifdef LJS_NEW_RAID_REWARD
	m_nRaidReward = 0;
#endif

#ifdef LJS_NEW_ATTRIBUTE_SYSTEM
	ZeroMemory( m_nNatualExp, sizeof(m_nNatualExp) );
#endif

#ifdef HSS_ADD_MONSTER_LIST_TABLE_BUFF_TYPE_20130115
	m_nRaidBuffType = 0;
	m_nRaidBuffCode = 0;
#endif

#ifdef HSS_NEW_CARD_MASTER_20131113
	m_nMaxDropCardItemCount = 0;
	ZeroMemory( m_pDropCardItem, sizeof(m_pDropCardItem) );	
#endif
}



void cMonsterSource::set(uint nAttr, uint nVal)
{
	xstop2(nAttr<nMonster::MaxAttribute, "(attr:%d, val:%d)", nAttr, nVal);
	m_nAttribute[nAttr] = nVal;
}


uint cMonsterSource::get(uint nAttr)
{
	xstop1(nAttr<nMonster::MaxAttribute, "(attr:%d)", nAttr);
	return m_nAttribute[nAttr];
}





