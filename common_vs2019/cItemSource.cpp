#include "nlib/cRandom.h"
#include "cItemSource.h"


cItemSource::cItemSource(void)
{


	memset(this, 0, sizeof(cItemSource));
}


cItemSource::~cItemSource(void)
{


}



int cItemSource::get(uint nAttr)
{
	xstop1(nAttr<nItem::MaxBaseAttribute, "(nAttr:%d)\n", nAttr);

	return m_nAttribute[nAttr];
}


void cItemSource::set(uint nAttr, int nVal)
{
	xstop2(nAttr<nItem::MaxBaseAttribute, "(attr:%d, val:%d)", nAttr, nVal);
	m_nAttribute[nAttr] = nVal;
}

bool cItemSource::IsClass( eClass c )
{
	int nClass = get( nItem::Class );
	switch( c )
	{
	case C_PCBang:
		return nClass == 12;
	case C_Cash:
		return ( nClass == 10 )||( nClass == 13 );
	}
	return false;
}



uint cItemSource::GetNextRX(void)
{
	uint r = nBase::rand() % (m_nNextRadius*2);
	int res = m_nNextPosX + m_nNextRadius - r;

	xassert(res>0, "res < 0");
	return res;
}


uint cItemSource::GetNextRY(void)
{
	uint r = nBase::rand() % (m_nNextRadius*2);
	int res = m_nNextPosY + m_nNextRadius - r;

	xassert(res>0, "res < 0");
	return res;
}
/*
uint cItemSource::GetOverlapMaxCount(cItemData *itemdata)
{
	if (itemdata)
	{
		if (itemdata->IsActive())
			return 1;		// 기간제 미사용 아이템 중복개수 최대값은 999로 고정
	}

	// 보통 아이템 생성시점(처음 인벤으로 추가)에는 ddf 설정 값으로 진행.
	return m_nAttribute[nItem::Overlap];
}
*/