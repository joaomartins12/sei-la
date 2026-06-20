

#include "cNpcSource.h"


cNpcSource::cNpcSource(void) : m_nItem(null)
{
	memset(m_nAttribute, 0, sizeof(m_nAttribute));


}



cNpcSource::~cNpcSource(void)
{


	xdelete_array(m_nItem);
}



void cNpcSource::SetAttr(uint nAttr, uint nVal)
{
	xstop2(nAttr<nNpc::MaxAttribute, "(attr:%d, val:%d)", nAttr, nVal);
	m_nAttribute[nAttr] = nVal;
}


uint cNpcSource::GetAttr(uint nAttr)
{
	xstop1(nAttr<nNpc::MaxAttribute, "(attr:%d)", nAttr);
	return m_nAttribute[nAttr];
}


void cNpcSource::NewShopItemArray(uint nCnt)
{
	m_nItem = xnew uint[nCnt];
	memset(m_nItem, 0, sizeof(uint)*nCnt);
}

	
uint cNpcSource::GetItemIDX(uint nSlot)
{
	if(nSlot < GetAttr(nNpc::ItemCount))
	{
		return m_nItem[nSlot];
	}

	return 0;
}


void cNpcSource::SetItemIDX(uint nSlot, uint nItemIDX)
{
	xstop2(nSlot<GetAttr(nNpc::ItemCount), "(nSlot:%d, nItemIDX:%d)", nSlot, nItemIDX);

	m_nItem[nSlot] = nItemIDX;
}


