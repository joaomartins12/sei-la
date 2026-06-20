


#pragma once


#include "pNpc.h"
#include "pLength.h"

#include "nlib/base.h"


class cNpcSource
{
public:
	cNpcSource(void);
   ~cNpcSource(void);

public:
	uint GetIDX(void) { return m_nIDX; }
	uint GetAttr(uint nAttr);
	char *GetName(void) { return m_szName; }

public:
	void SetIDX(uint idx) { m_nIDX = idx; }
	void SetAttr(uint nAttr, uint nVal);
	void SetName(char *name) { strcpy_s(m_szName, name); }

public:
	uint GetItemIDX(uint nSlot);
	void SetItemIDX(uint nSlot, uint nItemIDX);

	void NewShopItemArray(uint nCnt);



//////////////////////////////////////////////////////
// internal data
private:
	uint m_nIDX;
	char m_szName[64];

private:
	uint m_nAttribute[nNpc::MaxAttribute];

private: // type is shop npc
	uint *m_nItem;


};





