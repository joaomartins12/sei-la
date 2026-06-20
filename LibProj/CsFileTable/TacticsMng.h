
#pragma once 

#include "Tactics.h"
#include "TacticsExplain.h"
#include "TacticsEnchant.h"
#include "TacticsTranscend.h"

class CsTacticsMng
{
public:
	CsTacticsMng();
	~CsTacticsMng();

	bool					Init( char* cPath );
	void					Delete();
	void					SaveBin( char* cPath );

public:
	void					_LoadExcel();
	bool					_LoadBin( char* cPath );
	void					_LoadFilePack( char* cPath );

	void					_LoadExcelTactics();
	void					_LoadExcelTacticsExplain();
	void					_LoadExcelTacticsEnchant();
	
	void					_LoadExcelTranscendInfo();
	void					_LoadExcelTranscendEvo();
	void					_LoadExcelTranscendCorrect();
	void					_LoadExcelTranscendCorrectEx();

public:
	bool					IsTactics( DWORD dwDigitamaID ){ return m_mapTactics.find( dwDigitamaID ) != m_mapTactics.end(); }
	CsTactics*				GetTactics( DWORD dwDigitamaID );
	CsTactics::MAP*			GetTacticsMap() { return &m_mapTactics; }

	bool					IsTacticsExplain( DWORD dwDigiMonID ){ return m_mapTacticsExplain.find( dwDigiMonID ) != m_mapTacticsExplain.end(); }
	CsTacticsExplain*		GetTacticsExplain( DWORD dwDigiMonID );
	CsTacticsExplain::MAP*	GetExplainMap(){ return &m_mapTacticsExplain; }
	

	sTranscendInfo const*		GetTranscendInfo(int const& nCurrentEvoIdx, int const& nCurrentGrowth) const;
	MAP_TRANS_USE_TYPES const*	GetTranscendEvo(DWORD const& dwDigimonIdx) const;
	LIST_MATERIALS const*		GetTranscendEvo(DWORD const& dwDigimonIdx, int const& nItemUseType) const;

	sTranscendCorrect const&	GetTranscend_Correct() const;
	float						GetSameCorrectValue(bool bSame, int const& nGrowth) const;

	//////////////////////////////////////////////////////////////////////////
	bool						IsEnchantData( DWORD const& dwItemType ) const;
	sEnchantItemInfo const*		GetEnchantItemInfo( DWORD const& dwItemType ) const;	
	MAP_ENCHANT_ITEM_INFO const& GetEnchantItemData() const;

	sEnchantStatInfo const*		GetEnchantStatInfo( int const& nStatIdx, int const& nEnchantLv ) const;
	bool						IsEnableEnchantHatchLv( int const& nStatIdx, int const& nEnchantLv, int const& nHatchLv ) const;

	//////////////////////////////////////////////////////////////////////////
	bool						IsExpDigimon( DWORD const& dwCheckDigimonIDX, int const& nCurrentGrowth ) const;
	DWORD						GetExpDigimonFixedFigure( DWORD const& dwCheckDigimonIDX, int const& nCurrentGrowth ) const;

private:
	CsTactics::MAP			m_mapTactics;
	CsTacticsExplain::MAP	m_mapTacticsExplain;

	MAP_TRANS_INFO			m_lTacticsTransInfo;// 초월시 대상 디지몬에대한 제한 값
	MAP_TRANS_EVO			m_lTacticsTransEvo;	// 초월시 대상 디지몬에 필요한 아이템 값
	sTranscendCorrect		m_Correct;			// 초월 보정 데이터 

	MAP_ENCHANT_ITEM_INFO	m_mapEnchantItemInfo;	// 강화 아이템 정보
	MAP_ENCHANT_STAT_INFO	m_mapEnchantStatInfo;	// 강화 스텟 정보

	MAP_EXPDIGIMON_GROUP	m_mapExpDigimonGroup;	// 경험치 디지몬 데이터
};