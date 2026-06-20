
#pragma once 

#include "nsCsDigimonTable.h"
#include "DEffect.h"
#include "Digimon.h"
#include "Ride.h"
#include "ElementUI.h"

class CsDigimonMng
{
public:
	CsDigimonMng();
	~CsDigimonMng();

	void			Delete();
	bool			Init( char* cPath );


protected:	
	CsDigimon::MAP			m_mapDigimon;
#ifdef CROSSWARS_SYSTEM
	CsCrossDigimon::MAP		m_mapCrossDigimon;
	CsCrossDigimon::MAP		m_mapCrossCombines;
	CsCrossScene::MAP		m_mapCrossScene;
#endif
	CsRide::MAP				m_mapRide;

	CsNatureTypeUI::MAP		m_mapNatureTypeUI;
	CsAttributeTypeUI::MAP	m_mapAttributeTypeUI;	

	CsEncy_OptData::MAP				m_mapEncy_OptData;				// 덱 옵션 이름, 설명, 아이콘 ( Data 시트 )
	CsEncy_Exception_Digimon::MAP	m_mapEncy_Exception_Digimon;	// 도감 제외 디지몬( Exception_Digimon 시트 )
	CsEncy_Deck::MAP				m_mapEncy_Deck;					// 덱별 옵션 정보 ( Book_Deck 시트 )
	CsEncy_Deck_Composition::MAP	m_mapEncy_Composition;			// 덱 구성 정보 ( Composition_Deck 시트 )

	// 배열 : 기능타입 , Key : 디지몬ID
	CsPassive_Ability_Digimon::MAP		m_mapPassiveAbiltiy[nsCsDigimonTable::PA_MAX];

	CsData_Exchange::MAP			m_mapData_Exchange;		//	DATA_TRADE_SYSTEM

public:
	bool			_LoadExcel();
	void			__LoadExcelDigimon();
#ifdef CROSSWARS_SYSTEM
	void			__LoadExcelCrossDigimon();
	void			__LoadExcelCrossScene();
#endif
	void			__LoadExcelExtraInfo();
	void			__LoadExcelRide();
	void			__LoadExcelAttributeTypeUI();
	void			__LoadExcelNatureTypeUI();

	bool			__LoadExcelDigimonBook();

	void			__LoadExcelPassiveEffect();	//DigimonList.xls > Passive_effect 탭

	void			__LoadExcelData_Exchange();	// DigimonList.xls => Data_Exchange 탭		//	DATA_TRADE_SYSTEM

	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	CsDigimon*		GetDigimon( DWORD dwDigimonID );
	int				GetDigimonRank( DWORD const& dwDigimonID ) const;
	CsRide*			GetRide( DWORD dwDigimonID );
	CsRide::MAP*	GetRideMap();
	bool			IsRide( DWORD dwDigimonID );

	bool			IsDigimon( DWORD dwDigimonID );
	void			FindDigimon_FromModelID( DWORD const& dwModelID, std::list<DWORD>& listDigimonID );

	CsDigimon::MAP*	GetDigimonMap();

#ifdef CROSSWARS_SYSTEM
	CsCrossDigimon*	GetCrossDigimon( DWORD dwDigimonID );
	bool			IsCrossDigimon( DWORD dwDigimonID );
	CsCrossDigimon::MAP* GetCrossDigimonMap();

	CsCrossDigimon*	GetCrossCombine( DWORD dwDigimonID );
	bool			IsCrossCombine( DWORD dwDigimonID );
	CsCrossDigimon::MAP* GetCrossCombineMap();

	CsCrossScene*	GetCrossScene( DWORD dwDigimonID );
	bool			IsCrossScene( DWORD dwDigimonID );
	CsCrossScene::MAP* GetCrossSceneMap();
#endif

	bool							_IsExceptionDigimon( DWORD dwDigimonID );
	CsEncy_OptData*					GetOptData( int nIdx );
	CsEncy_Deck*					GetDeck( int nIdx );
	CsEncy_Deck::MAP*				GetDeckMap();
	CsEncy_Deck_Composition*		GetGroup( int nIdx );
	CsEncy_Deck_Composition::MAP*	GetGroupMap();
	CsPassive_Ability_Digimon*		GetPassiveAbility( DWORD dwDigimonID, int nType );
	const	CsData_Exchange::MAP	&GetData_ExchangeMap() const { return m_mapData_Exchange; }	//	DATA_TRADE_SYSTEM

	void			SaveBin( char* cPath );


	enum eCOMPARE{ UP, DOWN, EQUAL, BEST, WORST, };
	eCOMPARE		NatureCompare( DWORD dwOwnerID, DWORD dwTargetID );
	eCOMPARE		AttibuteCompare( DWORD dwOwnerID, DWORD dwTargetID );
	eCOMPARE		NatureCompare2( DWORD dwOwnerID, DWORD dwTargetID, int i );
	eCOMPARE		AttibuteCompare2( DWORD dwOwnerID, DWORD dwTargetID );
	CsNatureTypeUI*	GetNatureTypeUI( DWORD dwNatureTypeID );
	CsNatureTypeUI::MAP* GetNatureTypeUIMap();
	bool			IsNatureTypeUI ( DWORD dwNatureTypeID );

	CsAttributeTypeUI*	GetAttributeTypeUI( DWORD dwAttributeTypeID );
	CsAttributeTypeUI::MAP* GetAttributeTypeUIMap();
	bool			IsAttributeTypeUI ( DWORD dwAttributeTypeID );
	bool			IsStartDigimon( DWORD dwFTID );	

	// 디지몬 모델 IDX를 얻어온다
	DWORD			GetDigimonModelIdx( DWORD const& dwFTID ) const;
};

