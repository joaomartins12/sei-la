#pragma once

#include "DigimonEvolveInfo.h"

#define FT_BATTLE_EVO_ELEMENT_CNT		6
#define FT_BATTLE_EVO_TYPE_CNT			5

/*
DigimonEvolveObj - 디지몬 진화체 각 객체당 정보
CDigimonEvolveInfo - 디지몬의 진화트리 정보
*/


class CDigimonEvolution
{
public:
	CDigimonEvolution(void);

public:
	void						Delete();
	bool						Init( char* cPath );


public:
	struct sBATTLE_EVO_INFO
	{
		struct sELEMENT
		{
			sELEMENT(){ s_bEnable = false; }
			bool	s_bEnable;
			USHORT	s_nEvolStep;
			USHORT	s_nReqEvoEP;
			USHORT	s_nDecEP;
		};
		USHORT		s_nMaxEP;
		sELEMENT	s_Element[ FT_BATTLE_EVO_ELEMENT_CNT ];
	};


protected:
	CDigimonEvolveInfo::MAP		m_Map;
	sBATTLE_EVO_INFO			m_BattleEvoInfo[ FT_BATTLE_EVO_TYPE_CNT ];


public:
	bool						IsEvolveInfo( DWORD nID ){ return ( m_Map.find( nID ) != m_Map.end() );}
	CDigimonEvolveInfo*			GetEvolveInfo( DWORD nID );						// 디지몬 아이디로 진화 정보 가져오기
	CDigimonEvolveInfo::MAP*	GetMap(){ return &m_Map; }
	CDigimonEvolveObj*			GetEvolObj( DWORD nID );		// 디지몬 ID로 개체 정보 가져오기

public:
	sBATTLE_EVO_INFO*			GetBattleEvo( int nType ){ return &m_BattleEvoInfo[ nType - 1 ]; }
	

protected:
	void			_LoadExcel();
	void			_LoadExcel( char* cPath, char* cSheet );
	void			_LoadExcel_Extra();
	void			_LoadExcel_BattleEvo();

	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	void			SaveBin( char* cPath );
	void			Reload();

protected:
	void			_Reload( char* cPath, char* cSheet );
};



