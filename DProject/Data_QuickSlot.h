
#pragma once


class cData_QuickSlot
{
public:
	cData_QuickSlot();

public:
	void		SetChar( CsC_AvObject* pParent );

protected:
	void		_InitSlot_Tamer( CsC_AvObject* pParent );
	void		_InitSlot_Digimon( CsC_AvObject* pParent );

protected:
	int							m_nIFIndex;
	cResist::sCHAR_QUICKSLOT*	m_pQuickSlot;	

public:
	cResist::sCHAR_QUICKSLOT::sCHAR*	GetData( int nEvoSlot ){ return &m_pQuickSlot->s_QuickSlotChar[ nEvoSlot ]; }


public:
	static void		QuickSlotUse( CsC_AvObject* pParent, cResist::sCHAR_QUICKSLOT::sCHAR* pData, int nSlotIndex );
	static void		CheckAllSlot( CsC_AvObject* pParent, cResist::sCHAR_QUICKSLOT::sCHAR* pData, int nSkillCount );
	static void		CheckItemCount_AllUser();
	static void		CheckItemCount( cResist::sCHAR_QUICKSLOT::sCHAR* pData );
	static void		CheckSkill( cResist::sCHAR_QUICKSLOT::sCHAR* pData );

	static int		InvenToQuickSlot( int nInvenIndex, int nDestParentIndex, cResist::sCHAR_QUICKSLOT::sCHAR* pData, int nSlotIndex );
	static int		SkillToQuickSlot( int nSrcParentIndex, int nSkillIndex,
									  int nDestParentIndex, cResist::sCHAR_QUICKSLOT::sCHAR* pData, int nSlotIndex );
	static int		QuickSlotToQuickSlot(	int nSrcParentIndex, int nSrcType, int nSrcID,
											int nDestParentIndex, cResist::sCHAR_QUICKSLOT::sCHAR* pDestData, int nDestSlotIndex );

	cResist::sCHAR_QUICKSLOT*	GetQuickSlot() { return m_pQuickSlot; }

	void			_InitSlot_Added( CsC_AvObject* pParent );

};