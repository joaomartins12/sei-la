#pragma once

#include "DigimonEvolveObj.h"

class CDigimonEvolveInfo
{
public:
	typedef	std::map< DWORD, CDigimonEvolveInfo* >					MAP;
	typedef	std::map< DWORD, CDigimonEvolveInfo* >::iterator		MAP_IT;

public:
	CDigimonEvolveInfo(void);

	struct sINFO
	{
		sINFO(){ s_nDigimonID = s_nBattleEvoType = 0; }
		DWORD			s_nDigimonID;				// 디지몬 아이디
		DWORD			s_nBattleEvoType;
	};

protected:
	sINFO					m_Info;

public:
	CDigimonEvolveObj::MAP	m_Map;						// 진화체 리스트

public:
	void					Destroy();
	void					Add( CDigimonEvolveObj* pObj ); 

	CDigimonEvolveObj*		GetEvolveObjByEvoSlot(int nEvoSlot);		// 진화 번호로 진화객체 찾기

	// 진화체 아이디로 진화객체 찾기
	CDigimonEvolveObj*		GetEvolveObjByID( DWORD const & nDwID );
	bool					IsEvolveObjByID( DWORD const & nDwID ) const;
	sINFO*					GetInfo(){ return &m_Info; }
};
