
#pragma once 
//
//#define IF_THOUSE_MY_TACTICS_COUNT		3
//#define IF_THOUSE_MY_SLOT_COUNT			10

//#define IF_THOUSE_MY_PAGE_COUNT		2
//#define IF_THOUSE_HOUSE_MAX_COUNT		IF_THOUSE_MY_SLOT_COUNT * IF_THOUSE_MY_PAGE_COUNT

//class cData_TacticsHouse
//{
//public:
//	cData_TacticsHouse();
//
//public:
//	void	Delete();
//	void	Reset();
//
//	void	Init();
//	void	InitNetOff();
//
//	// ¿ëº´
//protected:
//	int							m_nUseSlotCount;
//	cData_PostLoad::sDATA*		m_pTacticsArray;
//
//public:
//	void						SetUseSlotCount( int nCount ){ m_nUseSlotCount = nCount; }
//	int							GetUseSlotCount(){ return m_nUseSlotCount; }
//	
//	bool						IsTactics( int nTacticsIndex );//{ return ( m_pTacticsArray[ nTacticsIndex ].s_dwBaseDigimonID != 0 ); }
//
//	cData_PostLoad::sDATA*		GetTactics( int nTacticsIndex );//{ assert_cs( nTacticsIndex < m_nUseSlotCount ); return &m_pTacticsArray[ nTacticsIndex ]; }
//
////#ifdef DIGIMON_INCUBATOR
////protected:
////	int			m_nIn_IncuDigimon_Slot[ 3 ];
////public:
////	int			_GetIncuSlot( int nIdx ) { return m_nIn_IncuDigimon_Slot[ nIdx ]; }
////	void		_SetIncuSlot( int nIdx, int nSlot ) { m_nIn_IncuDigimon_Slot[ nIdx ] = nSlot; }
////#endif
//};