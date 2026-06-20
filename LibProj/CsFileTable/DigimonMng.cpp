
#include "stdafx.h"
#include "DigimonMng.h"

CsDigimonMng::CsDigimonMng()
{
	m_mapDigimon.clear();
}

CsDigimonMng::~CsDigimonMng()
{

}

void CsDigimonMng::Delete()
{
	{
		CsDigimon::MAP_IT it = m_mapDigimon.begin();
		CsDigimon::MAP_IT itEnd = m_mapDigimon.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapDigimon.clear();
	}
#ifdef CROSSWARS_SYSTEM
	{
		CsCrossDigimon::MAP_IT it = m_mapCrossDigimon.begin();
		CsCrossDigimon::MAP_IT itEnd = m_mapCrossDigimon.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapCrossDigimon.clear();
	}
	{
		CsCrossDigimon::MAP_IT it = m_mapCrossCombines.begin();
		CsCrossDigimon::MAP_IT itEnd = m_mapCrossCombines.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapCrossCombines.clear();
	}
	{
		CsCrossScene::MAP_IT it = m_mapCrossScene.begin();
		CsCrossScene::MAP_IT itEnd = m_mapCrossScene.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapCrossScene.clear();
	}

#endif
	{
		CsRide::MAP_IT it = m_mapRide.begin();
		CsRide::MAP_IT itEnd = m_mapRide.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapRide.clear();
	}

	{
		CsAttributeTypeUI::MAP_IT it = m_mapAttributeTypeUI.begin();
		CsAttributeTypeUI::MAP_IT itEnd = m_mapAttributeTypeUI.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapAttributeTypeUI.clear();
	}	

	{
		CsNatureTypeUI::MAP_IT it = m_mapNatureTypeUI.begin();
		CsNatureTypeUI::MAP_IT itEnd = m_mapNatureTypeUI.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapNatureTypeUI.clear();
	}

	{
		for(int i = nsCsDigimonTable::PA_START; i < nsCsDigimonTable::PA_MAX; ++i)
		{
			CsPassive_Ability_Digimon::MAP_IT it = m_mapPassiveAbiltiy[i].begin();
			CsPassive_Ability_Digimon::MAP_IT itEnd = m_mapPassiveAbiltiy[i].end();
			for( ; it!=itEnd; ++it )
			{
				delete it->second;
			}
			m_mapPassiveAbiltiy[i].clear();
		}

	}

	{
		CsEncy_OptData::MAP_IT it = m_mapEncy_OptData.begin();
		CsEncy_OptData::MAP_IT itEnd = m_mapEncy_OptData.end();
		for( ; it!=itEnd; ++it )
			delete it->second;

		m_mapEncy_OptData.clear();
	}
	{
		CsEncy_Exception_Digimon::MAP_IT it = m_mapEncy_Exception_Digimon.begin();
		CsEncy_Exception_Digimon::MAP_IT itEnd = m_mapEncy_Exception_Digimon.end();
		for( ; it!=itEnd; ++it )
			delete it->second;

		m_mapEncy_Exception_Digimon.clear();
	}
	{
		CsEncy_Deck::MAP_IT it = m_mapEncy_Deck.begin();
		CsEncy_Deck::MAP_IT itEnd = m_mapEncy_Deck.end();
		for( ; it!=itEnd; ++it )
			delete it->second;

		m_mapEncy_Deck.clear();
	}
	{
		CsEncy_Deck_Composition::MAP_IT it = m_mapEncy_Composition.begin();
		CsEncy_Deck_Composition::MAP_IT itEnd = m_mapEncy_Composition.end();
		for( ; it!=itEnd; ++it )
			delete it->second;

		m_mapEncy_Composition.clear();
	}

	{
		CsData_Exchange::MAP_Iter it = m_mapData_Exchange.begin();
		CsData_Exchange::MAP_Iter itEnd = m_mapData_Exchange.end();

		for( ; it != itEnd; ++it )
			it->second.clear();

		m_mapData_Exchange.clear();

	}
}

bool CsDigimonMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		if( _LoadExcel() == false )
			return false;
		break;
	case nsCsFileTable::FT_BIN:		
		if( _LoadBin( cPath ) == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack( cPath );
		break;
	default:
		assert_cs( false );
	}

	return true;
}

CsDigimonMng::eCOMPARE
CsDigimonMng::NatureCompare( DWORD dwOwnerID, DWORD dwTargetID )
{
	// 내 디지몬과 비교
	int nValue = nsCsFileTable::g_pNatureMng->GetValue( 
						GetDigimon( dwOwnerID )->GetInfo()->s_eBaseNatureType,
						GetDigimon( dwTargetID )->GetInfo()->s_eBaseNatureType );

	if( nValue < -20 )
		return WORST;
	else if( nValue < 0 )
		return DOWN;
	else if( nValue <= 10 )
		return EQUAL;
	else if( nValue > 20 )
		return BEST;
	else if( nValue > 0 )
		return UP;

	return (eCOMPARE)0;
}

CsDigimonMng::eCOMPARE
CsDigimonMng::NatureCompare2( DWORD dwOwnerID, DWORD dwTargetID, int i)
{
	// 내 디지몬과 비교
	int nValue = nsCsFileTable::g_pNatureMng->GetValue( 
		GetDigimon( dwOwnerID )->GetInfo()->s_eBaseNatureTypes[i],
		GetDigimon( dwTargetID )->GetInfo()->s_eBaseNatureTypes[i] );

	if( nValue < -20 )
		return WORST;
	else if( nValue < 0 )
		return DOWN;
	else if( nValue <= 10 )
		return EQUAL;
	else if( nValue > 20 )
		return BEST;
	else if( nValue > 0 )
		return UP;

	return (eCOMPARE)0;
}

CsDigimonMng::eCOMPARE
CsDigimonMng::AttibuteCompare2( DWORD dwOwnerID, DWORD dwTargetID )
{	
	nsCsDigimonTable::eATTRIBUTE_TYPE eOwner = GetDigimon( dwOwnerID )->GetInfo()->s_eAttributeType;
	nsCsDigimonTable::eATTRIBUTE_TYPE eTarget = GetDigimon( dwTargetID )->GetInfo()->s_eAttributeType;
	switch( eTarget )
	{
	case nsCsDigimonTable::AT_NONE:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_NONE:			return EQUAL; // 타겟이 none 이고 자신도 none 이면 이퀄
			default:								return UP; // 타겟이 none 일때 그외 업
			}
		}
		break;
	case nsCsDigimonTable::AT_DATA:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_NONE:
			case nsCsDigimonTable::AT_VACCINE:
				return DOWN;
			case nsCsDigimonTable::AT_VIRUS:
			case nsCsDigimonTable::AT_UNIDENTIFIED:
				return UP;
			case nsCsDigimonTable::AT_DATA:
				return EQUAL;
			}
		}
		break;
	case nsCsDigimonTable::AT_VACCINE:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_NONE:
			case nsCsDigimonTable::AT_VIRUS:
				return DOWN;
			case nsCsDigimonTable::AT_DATA:						
			case nsCsDigimonTable::AT_UNIDENTIFIED:
				return UP;
			case nsCsDigimonTable::AT_VACCINE:
				return EQUAL;
			}
		}
		break;
	case nsCsDigimonTable::AT_VIRUS:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_NONE:
			case nsCsDigimonTable::AT_DATA:
				return DOWN;
			case nsCsDigimonTable::AT_VACCINE:
			case nsCsDigimonTable::AT_UNIDENTIFIED:
				return UP;
			case nsCsDigimonTable::AT_VIRUS:
				return EQUAL;
			}
		}
		break;
	case nsCsDigimonTable::AT_UNIDENTIFIED:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_UNIDENTIFIED:			return EQUAL;
			default:										return DOWN;
			}
		}
		break;		
	}

	assert_cs( false );
	return (eCOMPARE)0;
}


CsNatureTypeUI*	CsDigimonMng::GetNatureTypeUI( DWORD dwNatureTypeID )
{ 
	CsNatureTypeUI::MAP_IT it = m_mapNatureTypeUI.find(dwNatureTypeID);
	if( it == m_mapNatureTypeUI.end() )
		return NULL;
	return it->second;
} 

CsNatureTypeUI::MAP* CsDigimonMng::GetNatureTypeUIMap() 
{ 
	return &m_mapNatureTypeUI; 
}

bool CsDigimonMng::IsNatureTypeUI ( DWORD dwNatureTypeID )
{ 
	return (m_mapNatureTypeUI.find( dwNatureTypeID) != m_mapNatureTypeUI.end()); 
}

CsAttributeTypeUI* CsDigimonMng::GetAttributeTypeUI( DWORD dwAttributeTypeID )
{ 
	CsAttributeTypeUI::MAP_IT it = m_mapAttributeTypeUI.find( dwAttributeTypeID);
	if( it == m_mapAttributeTypeUI.end() )
		return NULL;
	return it->second;
} 

CsAttributeTypeUI::MAP* CsDigimonMng::GetAttributeTypeUIMap() 
{ 
	return &m_mapAttributeTypeUI; 
}

bool CsDigimonMng::IsAttributeTypeUI ( DWORD dwAttributeTypeID )
{ 
	return (m_mapAttributeTypeUI.find( dwAttributeTypeID) != m_mapAttributeTypeUI.end()); 
}

CsDigimonMng::eCOMPARE
CsDigimonMng::AttibuteCompare( DWORD dwOwnerID, DWORD dwTargetID )
{	
	nsCsDigimonTable::eATTRIBUTE_TYPE eOwner = GetDigimon( dwOwnerID )->GetInfo()->s_eAttributeType;
	nsCsDigimonTable::eATTRIBUTE_TYPE eTarget = GetDigimon( dwTargetID )->GetInfo()->s_eAttributeType;
	switch( eTarget )
	{
	case nsCsDigimonTable::AT_NONE:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_NONE:			return EQUAL;
			default:								return UP;
			}
		}
		break;
	case nsCsDigimonTable::AT_DATA:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_NONE:
			case nsCsDigimonTable::AT_VACCINE:
				return DOWN;
			case nsCsDigimonTable::AT_VIRUS:
			case nsCsDigimonTable::AT_UNIDENTIFIED:
				return UP;
			case nsCsDigimonTable::AT_DATA:
				return EQUAL;
			}
		}
		break;
	case nsCsDigimonTable::AT_VACCINE:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_NONE:
			case nsCsDigimonTable::AT_VIRUS:
				return DOWN;
			case nsCsDigimonTable::AT_DATA:						
			case nsCsDigimonTable::AT_UNIDENTIFIED:
				return UP;
			case nsCsDigimonTable::AT_VACCINE:
				return EQUAL;
			}
		}
		break;
	case nsCsDigimonTable::AT_VIRUS:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_NONE:
			case nsCsDigimonTable::AT_DATA:
				return DOWN;
			case nsCsDigimonTable::AT_VACCINE:
			case nsCsDigimonTable::AT_UNIDENTIFIED:
				return UP;
			case nsCsDigimonTable::AT_VIRUS:
				return EQUAL;
			}
		}
		break;
	case nsCsDigimonTable::AT_UNIDENTIFIED:
		{
			switch( eOwner )
			{
			case nsCsDigimonTable::AT_UNIDENTIFIED:			return EQUAL;
			default:										return DOWN;
			}
		}
		break;		
	}

	assert_cs( false );
	return (eCOMPARE)0;
}

bool CsDigimonMng::IsStartDigimon( DWORD dwFTID )
{
	switch( dwFTID )
	{
	case 31001:
	case 31002:
	case 31003:
	case 31004:
		return true;
	}
	return false;
}

bool CsDigimonMng::_LoadExcel()
{
	__LoadExcelDigimon();
#ifdef CROSSWARS_SYSTEM
	__LoadExcelCrossDigimon();
	__LoadExcelCrossScene();
#endif
	__LoadExcelExtraInfo();
	__LoadExcelRide();
	__LoadExcelAttributeTypeUI();
	__LoadExcelNatureTypeUI();
	if(	__LoadExcelDigimonBook() == false )
		return false;

	__LoadExcelPassiveEffect();		// 디지몬 패시브 기능(NPC할인 등..) 
	__LoadExcelData_Exchange();
	return true;
}

void CsDigimonMng::__LoadExcelDigimon()
{
#define DIGIMON_LIST_SHEET_NAME			"Digimon_List"
	
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Digimon(), DIGIMON_LIST_SHEET_NAME );
	assert_cs( bSuccess == true );

#define DL_COL_DIGIMONID				1
#define DL_COL_MODELID					DL_COL_DIGIMONID + 1
#define DL_COL_KOREA_NAME				DL_COL_MODELID+1
#define DL_COL_ENGLISH_NAME				DL_COL_KOREA_NAME + 1
#define DL_COL_DIRECTORY_NAME			DL_COL_ENGLISH_NAME + 1
#define DL_COL_CHINESE_NAME				DL_COL_DIRECTORY_NAME + 1

#define DL_COL_DISIMON_RANK				DL_COL_DIRECTORY_NAME + 3

#define DL_COL_EVOLUTION				DL_COL_CHINESE_NAME + 5
#define DL_COL_ATTRIBUTE				DL_COL_EVOLUTION + 2
#define DL_COL_FAMILY_1					DL_COL_ATTRIBUTE + 2
#define DL_COL_FAMILY_2					DL_COL_FAMILY_1 + 2
#define DL_COL_FAMILY_3					DL_COL_FAMILY_2 + 2
#define DL_COL_BASE_NATURE				DL_COL_FAMILY_3	+ 2

#define DL_COL_BASE_NATURE1				DL_COL_BASE_NATURE	+ 2
#define DL_COL_BASE_NATURE2				DL_COL_BASE_NATURE1	+ 1
#define DL_COL_BASE_NATURE3				DL_COL_BASE_NATURE2	+ 1
#define DL_COL_BASE_LEVEL				DL_COL_BASE_NATURE3 + 3

#define DL_COL_BASE_STAT_HP				DL_COL_BASE_LEVEL + 1
#define DL_COL_BASE_STAT_DS				DL_COL_BASE_STAT_HP + 1
#define DL_COL_BASE_STAT_DP				DL_COL_BASE_STAT_DS + 1
#define DL_COL_BASE_STAT_EV				DL_COL_BASE_STAT_DP + 1
#define DL_COL_BASE_STAT_MS				DL_COL_BASE_STAT_EV + 1
#define DL_COL_BASE_STAT_CA				DL_COL_BASE_STAT_MS + 1
#define DL_COL_BASE_STAT_AP				DL_COL_BASE_STAT_CA + 1
#define DL_COL_BASE_STAT_AS				DL_COL_BASE_STAT_AP + 1
#define DL_COL_BASE_STAT_AR				DL_COL_BASE_STAT_AS + 1
#define DL_COL_BASE_STAT_HT				DL_COL_BASE_STAT_AR + 1
#define DL_COL_DIGIMON_TYPE				DL_COL_BASE_STAT_HT + 2
#define DL_COL_CHAR_SIZE				DL_COL_DIGIMON_TYPE + 2
#define DL_COL_SKILL_START				DL_COL_CHAR_SIZE + 6
#define DL_COL_WAKL_LEN 				DL_COL_SKILL_START + (4 * DIGIMON_MAX_SKILL_COUNT)
#define DL_COL_RUN_LEN 					DL_COL_WAKL_LEN + 1
#define DL_COL_ARUN_LEN 				DL_COL_RUN_LEN + 1
#define DL_COL_DIGIMON_FORM_KOR			DL_COL_ARUN_LEN + 13
#define DL_COL_DIGIMON_FORM_ENG			DL_COL_DIGIMON_FORM_KOR + 1
	assert_cs( excel.GetColCount() >= DL_COL_CHAR_SIZE );

// 	char cBuffer[ CEXCLE_CELL_LEN ];
// 	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	while( excel.MoveNext() == true )
	{
		// Digimon ID
		DWORD dwDigimonID = 0;
		excel.Read_Value( DL_COL_DIGIMONID, dwDigimonID );
		if( 0 == dwDigimonID )
			break;

		CsDigimon::sINFO digiInfo;
		digiInfo.s_dwDigimonID = dwDigimonID;		

		// Check Digimon
		assert_csm1( IsDigimon( digiInfo.s_dwDigimonID ) == false, _T( "디지몬테이블 :  이미 존재하는 ID = %d" ), digiInfo.s_dwDigimonID );

		// ModelID
		excel.Read_Value( DL_COL_MODELID, digiInfo.s_dwModelID );

		// Name
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:	excel.Read_String( DL_COL_KOREA_NAME, digiInfo.s_szName, MAX_FILENAME );		break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:			excel.Read_String( DL_COL_ENGLISH_NAME, digiInfo.s_szName, MAX_FILENAME );		break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:			excel.Read_String( DL_COL_ENGLISH_NAME, digiInfo.s_szName, MAX_FILENAME );		break;
		default:
			assert_cs( false );
		}

		// Sound Directory Name
		excel.Read_String( DL_COL_DIRECTORY_NAME, digiInfo.s_cSoundDirName, MAX_FILENAME );
		excel.Read_Value( DL_COL_DIRECTORY_NAME + 1, digiInfo.s_fSelectScale );
		excel.Read_String( DL_COL_DIRECTORY_NAME + 2, digiInfo.s_szEvoEffectDir, MAX_FILENAME );
		// Digimon Rank
		excel.Read_Value( DL_COL_DIRECTORY_NAME + 3, digiInfo.s_nDigimonRank );

		// Evolution
		excel.Read_Value( DL_COL_EVOLUTION, digiInfo.s_eEvolutionType );


		// Attribute
		excel.Read_Value( DL_COL_ATTRIBUTE, digiInfo.s_eAttributeType );

		// Family
		excel.Read_Value( DL_COL_FAMILY_1, digiInfo.s_eFamilyType[0] );
		excel.Read_Value( DL_COL_FAMILY_2, digiInfo.s_eFamilyType[1] );
		excel.Read_Value( DL_COL_FAMILY_3, digiInfo.s_eFamilyType[2] );

		// Base Nature
		excel.Read_Value( DL_COL_BASE_NATURE, digiInfo.s_eBaseNatureType );
		excel.Read_Value( DL_COL_BASE_NATURE1, digiInfo.s_eBaseNatureTypes[0] );
		excel.Read_Value( DL_COL_BASE_NATURE2, digiInfo.s_eBaseNatureTypes[1] );
		excel.Read_Value( DL_COL_BASE_NATURE3, digiInfo.s_eBaseNatureTypes[2] );
		
		// Base Level
		excel.Read_Value( DL_COL_BASE_LEVEL, digiInfo.s_dwBaseLevel );

		// Base Stat
		excel.Read_Value( DL_COL_BASE_STAT_HP, digiInfo.s_nBaseStat_HP );
		excel.Read_Value( DL_COL_BASE_STAT_DS, digiInfo.s_nBaseStat_DS );
		excel.Read_Value( DL_COL_BASE_STAT_DP, digiInfo.s_nBaseStat_DefPower );
		excel.Read_Value( DL_COL_BASE_STAT_EV, digiInfo.s_nBaseStat_Evasion );
		excel.Read_Value( DL_COL_BASE_STAT_MS, digiInfo.s_nBaseStat_MoveSpeed );
		excel.Read_Value( DL_COL_BASE_STAT_CA, digiInfo.s_nBaseStat_CriticalRate );
		excel.Read_Value( DL_COL_BASE_STAT_AP, digiInfo.s_nBaseStat_AttPower );
		excel.Read_Value( DL_COL_BASE_STAT_AS, digiInfo.s_nBaseStat_AttSpeed );
		excel.Read_Value( DL_COL_BASE_STAT_AR, digiInfo.s_nBaseStat_AttRange );
		excel.Read_Value( DL_COL_BASE_STAT_HT, digiInfo.s_nBaseStat_HitRate );

		// 케릭터 타입
		excel.Read_Value( DL_COL_DIGIMON_TYPE, digiInfo.s_nDigimonType );
		assert_csm1( digiInfo.s_nDigimonType != 0, _T( "케릭터 타입이 0이면 안되요 디지몬 테이블 ID = %d" ), digiInfo.s_dwDigimonID );		

		// Height
		excel.Read_Value( DL_COL_CHAR_SIZE, digiInfo.s_dwCharSize );

		// 스킬
		long Index = DL_COL_SKILL_START;
		for( int i=0; i<DIGIMON_MAX_SKILL_COUNT; ++i )
		{
			excel.Read_Value( Index, digiInfo.s_Skill[ i ].s_dwID );
			Index += 3;
			excel.Read_Value( Index, digiInfo.s_Skill[ i ].s_nReqPrevSkillLevel );
			Index += 1;
		}

		digiInfo.s_fWakkLen = 300.0f;
		digiInfo.s_fRunLen = 300.0f;
		digiInfo.s_fARunLen = 300.0f;

		// 디지몬 유형
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			excel.Read_String( 81, digiInfo.s_szForm, MAX_FILENAME );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			excel.Read_String( 82, digiInfo.s_szForm, MAX_FILENAME );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			excel.Read_String( 82, digiInfo.s_szForm, MAX_FILENAME );
			break;
		default:
			assert_cs( false );
		}

		CsDigimon* pDigimon = csnew CsDigimon;
		pDigimon->Init( &digiInfo );

		assert_cs( m_mapRide.find( digiInfo.s_dwDigimonID ) == m_mapRide.end() );
		m_mapDigimon.insert(make_pair(digiInfo.s_dwDigimonID, pDigimon));
	}	

	excel.Close();
}

#ifdef CROSSWARS_SYSTEM
void CsDigimonMng::__LoadExcelCrossDigimon()
{
#define CROSS_DIGIMON_LIST_SHEET_NAME			"CrossDigimon_List"
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Digimon(), CROSS_DIGIMON_LIST_SHEET_NAME );
	assert_cs( bSuccess == true );	

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsCrossDigimon::sINFO digiInfo;
	while( excel.MoveNext() == true )
	{
		// Digimon ID
		digiInfo.s_dwDigimonID = atoi( excel.GetData( 0, cBuffer ) );

		if( digiInfo.s_dwDigimonID == 0 )
			continue;		

		// Name
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( digiInfo.s_szName, excel.GetData( 1, cBuffer ), MAX_FILENAME );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( digiInfo.s_szName, excel.GetData( 1, cBuffer ), MAX_FILENAME );
			break;
		case nsCsFileTable::THAILAND:
			//M2W( digiInfo.s_szName, excel.GetData( DL_COL_ENGLISH_NAME, cBuffer ), MAX_FILENAME );
			wcscpy_s(digiInfo.s_szName, MAX_FILENAME, excel.GetData(1, wBuffer));
			break;
		default:
			assert_cs( false );
		}

		digiInfo.s_nCodeCrownRequire = atoi( excel.GetData( 2, cBuffer ) );
		digiInfo.s_nCodeCrownTime = atoi( excel.GetData( 3, cBuffer ) );
		digiInfo.s_nCrossType = atoi( excel.GetData( 4, cBuffer ) );
		digiInfo.s_nCrossRank = atoi( excel.GetData( 5, cBuffer ) );

		int cnt = 6;
		for(int i =0; i < digiInfo.s_nCrossRank; i++)
		{
			digiInfo.s_nCrossDigimon[i] = atoi( excel.GetData( cnt++, cBuffer ) );
		}
		
		CsCrossDigimon* pDigimon = csnew CsCrossDigimon;
		pDigimon->Init( &digiInfo );

		assert_cs( m_mapCrossDigimon.find( digiInfo.s_dwDigimonID ) == m_mapCrossDigimon.end() );
		m_mapCrossDigimon.insert(make_pair(digiInfo.s_dwDigimonID, pDigimon));

		if(1 == pDigimon->GetInfo()->s_nCrossType){
			CsCrossDigimon* pDigimon = csnew CsCrossDigimon;
			pDigimon->Init( &digiInfo );

			assert_cs( m_mapCrossCombines.find( digiInfo.s_dwDigimonID ) == m_mapCrossCombines.end() );
			m_mapCrossCombines.insert(make_pair(digiInfo.s_dwDigimonID, pDigimon));
		}
	}	

	excel.Close();
}

void CsDigimonMng::__LoadExcelCrossScene()
{
#define CROSS_SCENE_SHEET_NAME			"CrossScene"
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Digimon(), CROSS_SCENE_SHEET_NAME );
	assert_cs( bSuccess == true );	

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsCrossScene::sINFO digiInfo;
	while( excel.MoveNext() == true )
	{
		// Digimon ID
		digiInfo.s_dwDigimonID = atoi( excel.GetData( 0, cBuffer ) );

		if( digiInfo.s_dwDigimonID == 0 )
			continue;		

		// Name
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( digiInfo.s_szName, excel.GetData( 1, cBuffer ), MAX_FILENAME );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( digiInfo.s_szName, excel.GetData( 1, cBuffer ), MAX_FILENAME );
			break;
		case nsCsFileTable::THAILAND:
			//M2W( digiInfo.s_szName, excel.GetData( DL_COL_ENGLISH_NAME, cBuffer ), MAX_FILENAME );
			wcscpy_s(digiInfo.s_szName, MAX_FILENAME, excel.GetData(1, wBuffer));
			break;
		default:
			assert_cs( false );
		}

		digiInfo.s_nCrossSceneTime = (float)atof( excel.GetData( 2, cBuffer ) );		
		digiInfo.s_nDigimonCount = atoi( excel.GetData( 3, cBuffer ) );	
		strcpy_s(digiInfo.s_szBGPath, MAX_FILENAME, excel.GetData( 4, cBuffer ) );

		int cnt = 5;
		for(int i =0; i < digiInfo.s_nDigimonCount; i++)
		{
			digiInfo.s_Scene[i].s_dwID = atoi( excel.GetData( cnt++, cBuffer ) );
			strcpy_s(digiInfo.s_Scene[i].s_szRegionPath, MAX_FILENAME, excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_rx = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_ry = (float)atof( excel.GetData( cnt++, cBuffer ) );

			digiInfo.s_Scene[i].s_width = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_height = (float)atof( excel.GetData( cnt++, cBuffer ) );

			if( 1.0f < digiInfo.s_Scene[i].s_width || 1.0f < digiInfo.s_Scene[i].s_height )
			{
				CsMessageBox( MB_OK, _T( "비율값이 1.0보다 큽니다." ) );
			}

			digiInfo.s_Scene[i].s_dist = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_high = (float)atof( excel.GetData( cnt++, cBuffer ) );

			digiInfo.s_Scene[i].s_v0.x = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_v0.y = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_v1.x = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_v1.y = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_v2.x = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_v2.y = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_v3.x = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].s_v3.y = (float)atof( excel.GetData( cnt++, cBuffer ) );

			digiInfo.s_Scene[i].m_dwAni =atoi( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].m_fStartTime = (float)atof( excel.GetData( cnt++, cBuffer ) );
			digiInfo.s_Scene[i].m_fEndTime = (float)atof( excel.GetData( cnt++, cBuffer ) );		

		}

		CsCrossScene* pScene = csnew CsCrossScene;
		pScene->Init( &digiInfo );

		assert_cs( m_mapCrossScene.find( digiInfo.s_dwDigimonID ) == m_mapCrossScene.end() );
		m_mapCrossScene.insert(make_pair(digiInfo.s_dwDigimonID, pScene));
	}	

	excel.Close();
}
#endif

void CsDigimonMng::__LoadExcelExtraInfo()
{
#define DIGIMONANI_LIST_SHEET_NAME			"DM_DigimonANI"

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_DigimonANI(), DIGIMONANI_LIST_SHEET_NAME );
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD dwID = 0;
	CsDigimon::sINFO* pInfo = NULL;
	while( excel.MoveNext() == true )
	{
		// Digimon ID
		dwID = atoi( excel.GetData( 0, cBuffer ) );
		if( dwID == 0 )
			continue;

		if( IsDigimon( dwID ) == false )
		{
			CsMessageBox( MB_OK, _T( "DM_DigimonANI.xls 에 존재 하지만 DM_Digimon_List.xls 에 존재 하지 않는 디지몬이 있습니다.\nDigimon ID = %d" ), dwID );
			continue;
		}

		pInfo = GetDigimon( dwID )->GetInfo();		
		pInfo->s_fWakkLen = (float)atof( excel.GetData( 4, cBuffer ) );
		pInfo->s_fRunLen = (float)atof( excel.GetData( 5, cBuffer ) );
		pInfo->s_fARunLen = (float)atof( excel.GetData( 6, cBuffer ) );

		if( pInfo->s_fARunLen <= 0 )
			pInfo->s_fARunLen = pInfo->s_fRunLen;
	}	

	excel.Close();
}

void CsDigimonMng::__LoadExcelRide()
{
#define RIDE_LIST_SHEET_NAME			"DM_Riding"

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Ride(), RIDE_LIST_SHEET_NAME );
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD dwID = 0;
	CsRide::sINFO info;
	while( excel.MoveNext() == true )
	{
		// Digimon ID
		info.s_dwDigimonID = atoi( excel.GetData( 0, cBuffer ) );
		if( info.s_dwDigimonID == 0 )
			continue;

		if( m_mapDigimon.size()&&( IsDigimon( info.s_dwDigimonID ) == false ) )
		{
			CsMessageBox( MB_OK, _T( "DM_Riding.xls 에 존재 하지만 DM_Digimon_List.xls 에 존재 하지 않는 디지몬이 있습니다.\nDigimon ID = %d" ), dwID );
			continue;
		}

		info.s_dwChangeRide = atoi( excel.GetData( 2, cBuffer ) );

		info.s_fMoveSpeed = (float)atof( excel.GetData( 4, cBuffer ) );
		//M2W( info.s_szComment, excel.GetData( 12, cBuffer ), ITEM_COMMENT_LEN );

		// 디지몬 유형
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( info.s_szComment, excel.GetData( 12, cBuffer ), ITEM_COMMENT_LEN );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info.s_szComment, excel.GetData( 12, cBuffer ), ITEM_COMMENT_LEN );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			//M2W( info.s_szComment, excel.GetData( 12, cBuffer ), ITEM_COMMENT_LEN );
			wcscpy_s(info.s_szComment, ITEM_COMMENT_LEN, excel.GetData( 12, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		int nCol = 13;
		for( int i=0; i<FT_RIDE_OPENINFO; ++i )
		{
			info.s_OpenInfo[ i ].s_nItemType_S = atoi( excel.GetData( nCol, cBuffer ) );
			++nCol;
			info.s_OpenInfo[ i ].s_nNeedCount = atoi( excel.GetData( nCol, cBuffer ) );
			++nCol;
		}
		
		info.s_nRideType = atoi( excel.GetData( nCol, cBuffer ) );
		++nCol;
		info.s_fMoveSpeed = (float)atof( excel.GetData( nCol, cBuffer ) )*0.01f;
		++nCol;

		CsRide* pRide = csnew CsRide;
		pRide->Init( &info );

		assert_cs( m_mapRide.find( info.s_dwDigimonID ) == m_mapRide.end() );
		m_mapRide.insert(make_pair(info.s_dwDigimonID, pRide));	//	m_mapRide[ info.s_dwDigimonID ] = pRide; 오류가 있어서 수정.
	}
	excel.Close();
}

void CsDigimonMng::__LoadExcelAttributeTypeUI()
{
#define ATTRIBUTE_SHEET_NAME			"Digimon_Attribute_Type"

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_ElementUI(), ATTRIBUTE_SHEET_NAME );
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD dwID = 0;
	CsAttributeTypeUI::sINFO info;
	while( excel.MoveNext() == true )
	{
		
		info.s_nAttID = atoi( excel.GetData( 1, cBuffer ) );		

		//info.s_cPath = atoi( excel.GetData( 1, cBuffer ) );
		//M2W( info.s_cPath, excel.GetData( 2, cBuffer ), CEXCLE_CELL_LEN );
		strcpy_s(info.s_cPath, 64, excel.GetData( 2, cBuffer ) );

		//info.s_nX = (float)atof( excel.GetData( 2, cBuffer ) );
		info.s_nX = atoi( excel.GetData( 3, cBuffer ) );
		info.s_nY = atoi( excel.GetData( 4, cBuffer ) );
		info.s_nBarX = atoi( excel.GetData( 5, cBuffer ) );
		info.s_nBarY = atoi( excel.GetData( 6, cBuffer ) );

		

		CsAttributeTypeUI* pAttributeTypeUI = csnew CsAttributeTypeUI;
		pAttributeTypeUI->Init( &info );

		assert_cs( m_mapAttributeTypeUI.find( info.s_nAttID ) == m_mapAttributeTypeUI.end() );
		m_mapAttributeTypeUI.insert(make_pair(info.s_nAttID, pAttributeTypeUI));
	}
	excel.Close();
}

void CsDigimonMng::__LoadExcelNatureTypeUI()
{
#define NATURE_SHEET_NAME			"Digimon_Nature_Type"

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_ElementUI(), NATURE_SHEET_NAME );
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD dwID = 0;
	CsNatureTypeUI::sINFO info;
	while( excel.MoveNext() == true )
	{

		info.s_nNatID = atoi( excel.GetData( 1, cBuffer ) );		

		//info.s_cPath = atoi( excel.GetData( 2, cBuffer ) );
		//M2W( info.s_cPath, excel.GetData( 2, cBuffer ), CEXCLE_CELL_LEN );
		strcpy_s(info.s_cPath, 64, excel.GetData( 2, cBuffer ) );

		//info.s_nX = (float)atof( excel.GetData( 3, cBuffer ) );
		info.s_nX = atoi( excel.GetData( 3, cBuffer ) );
		info.s_nY = atoi( excel.GetData( 4, cBuffer ) );
		info.s_nGaugeX = atoi( excel.GetData( 5, cBuffer ) );
		info.s_nGaugeY = atoi( excel.GetData( 6, cBuffer ) );
		
		info.s_nAngle = (float)atof( excel.GetData( 7, cBuffer ) );



		CsNatureTypeUI* pNatureTypeUI = csnew CsNatureTypeUI;
		pNatureTypeUI->Init( &info );

		assert_cs( m_mapNatureTypeUI.find( info.s_nNatID ) == m_mapNatureTypeUI.end() );
		m_mapNatureTypeUI.insert(make_pair( info.s_nNatID, pNatureTypeUI));

	}
	excel.Close();
}

bool CsDigimonMng::__LoadExcelDigimonBook()
{
	{	// 옵션 기본 정보
		#define DATA_SHEET_NAME					"Data"
		#define DATA_COL_OPTION_ID				0
		#define DATA_COL_OPTION_NAME_KOR		DATA_COL_OPTION_ID+1			//1
		#define DATA_COL_OPTION_NAME_ENG		DATA_COL_OPTION_NAME_KOR+1		//2
#ifndef VERSION_KOR
		#define DATA_COL_OPTION_ICON			DATA_COL_OPTION_NAME_ENG+1		//3
#else
		#define DATA_COL_OPTION_ICON			DATA_COL_OPTION_NAME_KOR+1		//2
#endif
		#define DATA_COL_OPTION_EXPLAIN_KOR		DATA_COL_OPTION_ICON+1			//3	( 로컬은 +1 )
		#define DATA_COL_OPTION_EXPLAIN_ENG		DATA_COL_OPTION_EXPLAIN_KOR+1	//4	( 로컬은 +1 )

		TCHAR cTablePath[MAX_FILENAME];
		TCHAR cSheetname[MAX_FILENAME];
		M2W( cTablePath, nsCsFileTable::GetPath_DM_Digimon_Book(), MAX_FILENAME );
		M2W( cSheetname, DATA_SHEET_NAME, MAX_FILENAME );

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Digimon_Book(), DATA_SHEET_NAME );
		if( bSuccess == false )
		{
			CsMessageBox( MB_OK, L"%s\n%s 시트 오픈 실패. Excel 수정이 필요합니다.", cTablePath, cSheetname );
			return false;
		}

		char cBuffer[ CEXCLE_CELL_LEN ];
		wchar_t wBuffer[ CEXCLE_CELL_LEN ];

		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );	
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		CsEncy_OptData::sINFO info;
		while( excel.MoveNext() == true )
		{
			info.s_dwOptID = atoi( excel.GetData(DATA_COL_OPTION_ID, cBuffer ) );	// 옵션 번호
			info.s_nIcon = atoi( excel.GetData( DATA_COL_OPTION_ICON, cBuffer ) );	// 옵션아이콘 번호

			if( info.s_dwOptID == 0 )
				continue;

			// 이름, 설명
			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::KOREA_ORIGINAL:
			case nsCsFileTable::KOREA_TRANSLATION:
				M2W( info.s_szOptName, excel.GetData( DATA_COL_OPTION_NAME_KOR, cBuffer ), ENCY_NAME_LEN );

				TCHAR szBuffer[ 2048 ];
				M2W( szBuffer, excel.GetData( DATA_COL_OPTION_EXPLAIN_KOR, cBuffer ), 2048 );
				if( (int)_tcslen( szBuffer ) >= ENCY_EXPLAIN_LEN )
					CsMessageBox( MB_OK, L"%s\n%s 시트의 옵션설명 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d",nsCsFileTable::GetPath_DM_Digimon_Book(), cSheetname , (int)_tcslen( szBuffer ), ENCY_EXPLAIN_LEN );
				else
					_tcscpy_s( info.s_szOptExplain, ENCY_EXPLAIN_LEN, szBuffer  );

				M2W( info.s_szOptExplain, excel.GetData( DATA_COL_OPTION_EXPLAIN_KOR, cBuffer ), ENCY_EXPLAIN_LEN );
				break;
			case nsCsFileTable::ENGLISH:
			case nsCsFileTable::ENGLISH_A:
				M2W( info.s_szOptName, excel.GetData( DATA_COL_OPTION_NAME_ENG, cBuffer ), ENCY_NAME_LEN );
				M2W( info.s_szOptExplain, excel.GetData( DATA_COL_OPTION_EXPLAIN_ENG, cBuffer ), ENCY_EXPLAIN_LEN );
				break;
			case nsCsFileTable::THAILAND:
			case nsCsFileTable::TAIWAN:
			case nsCsFileTable::HONGKONG:
				wcscpy_s(info.s_szOptName, ENCY_NAME_LEN, excel.GetData( DATA_COL_OPTION_NAME_ENG, wBuffer));
				wcscpy_s(info.s_szOptExplain, ENCY_EXPLAIN_LEN, excel.GetData( DATA_COL_OPTION_EXPLAIN_ENG, wBuffer));
				break;
			default:
				assert_cs( false );
			}

			CsEncy_OptData* pEncy = csnew CsEncy_OptData;
			pEncy->Init( &info );

			assert_cs( m_mapEncy_OptData.find( info.s_dwOptID ) == m_mapEncy_OptData.end() );
			m_mapEncy_OptData.insert(make_pair(info.s_dwOptID, pEncy));
		}
		excel.Close();
	}	// 옵션 기본 정보

	{	// 예외 디지몬
		#define EXCEPTION_SHEET_NAME			"Exception_Digimon"
		#define EXCEPTION_COL_DIGIMON_ID	1
		#define EXCEPTION_COL_NAME_KOR		2
		#define EXCEPTION_COL_NAME_ENG		3

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Digimon_Book(), EXCEPTION_SHEET_NAME );
		if( bSuccess == false )
		{
			TCHAR cTablePath[MAX_FILENAME];
			TCHAR cSheetname[MAX_FILENAME];
			M2W( cTablePath, nsCsFileTable::GetPath_DM_Digimon_Book(), MAX_FILENAME );
			M2W( cSheetname, EXCEPTION_SHEET_NAME, MAX_FILENAME );

			CsMessageBox( MB_OK, L"%s\n%s 시트 오픈 실패. Excel 수정이 필요합니다.", cTablePath, cSheetname );
			return false;
		}

		char cBuffer[ CEXCLE_CELL_LEN ];
		wchar_t wBuffer[ CEXCLE_CELL_LEN ];

		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );	
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		CsEncy_Exception_Digimon::sINFO info;
		while( excel.MoveNext() == true )
		{
			info.s_dwDigimonID = atoi( excel.GetData( EXCEPTION_COL_DIGIMON_ID, cBuffer ) );		// 계열체 번호

			// 이름
			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::KOREA_ORIGINAL:
			case nsCsFileTable::KOREA_TRANSLATION:
				M2W( info.s_szName, excel.GetData( EXCEPTION_COL_NAME_KOR, cBuffer ), MAX_FILENAME );
				break;
			case nsCsFileTable::ENGLISH:
			case nsCsFileTable::ENGLISH_A:
				M2W( info.s_szName, excel.GetData( EXCEPTION_COL_NAME_ENG, cBuffer ), MAX_FILENAME );
				break;
			case nsCsFileTable::THAILAND:
			case nsCsFileTable::TAIWAN:
			case nsCsFileTable::HONGKONG:
				wcscpy_s(info.s_szName, MAX_FILENAME, excel.GetData(EXCEPTION_COL_NAME_ENG, wBuffer));
				break;
			default:
				assert_cs( false );
			}

			CsEncy_Exception_Digimon* pEncy = csnew CsEncy_Exception_Digimon;
			pEncy->Init( &info );

			assert_cs( m_mapEncy_Exception_Digimon.find( info.s_dwDigimonID ) == m_mapEncy_Exception_Digimon.end() );
			m_mapEncy_Exception_Digimon.insert(make_pair(info.s_dwDigimonID, pEncy));
		}
		excel.Close();
	}	// 예외 디지몬

	{	// 덱 정보( 이름, 설명, 옵션 )
		#define DECK_SHEET_NAME			"Book_Deck"
		#define DECK_COL_USE			0
		#define DECK_COL_ID				DECK_COL_USE+1				//1
		#define DECK_COL_NAME_KOR		DECK_COL_ID+1				//2
		#define DECK_COL_NAME_ENG		DECK_COL_NAME_KOR+1			//3
#ifndef VERSION_KOR
		#define DECK_COL_EXPLAIN_KOR	DECK_COL_NAME_ENG+1			//4
		#define DECK_COL_EXPLAIN_ENG	DECK_COL_EXPLAIN_KOR+1		//5
		#define DECK_COL_OPTION1		DECK_COL_EXPLAIN_ENG+1		//6
#else
		#define DECK_COL_EXPLAIN_KOR	DECK_COL_NAME_KOR+1			//3
		#define DECK_COL_EXPLAIN_ENG	DECK_COL_EXPLAIN_KOR+1		//4
		#define DECK_COL_OPTION1		DECK_COL_EXPLAIN_KOR+1		//4
#endif
// 		#define DECK_COL_OPTION1_VAL	DECK_COL_OPTION1+1			//5	( 로컬은 +2 )
// 		#define DECK_COL_OPTION1_PROB	DECK_COL_OPTION1_VAL+1		//6	( 로컬은 +2 )
// 		#define DECK_COL_OPTION1_TIME	DECK_COL_OPTION1_PROB+1		//7	( 로컬은 +2 )

		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Digimon_Book(), DECK_SHEET_NAME );
		if( bSuccess == false )
		{
			TCHAR cTablePath[MAX_FILENAME];
			TCHAR cSheetname[MAX_FILENAME];
			M2W( cTablePath, nsCsFileTable::GetPath_DM_Digimon_Book(), MAX_FILENAME );
			M2W( cSheetname, DECK_SHEET_NAME, MAX_FILENAME );

			CsMessageBox( MB_OK, L"%s\n%s 시트 오픈 실패. Excel 수정이 필요합니다.", cTablePath, cSheetname );
			return false;
		}

		char cBuffer[ CEXCLE_CELL_LEN ];
		wchar_t wBuffer[ CEXCLE_CELL_LEN ];

		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );	
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		while( excel.MoveNext() == true )
		{
			CsEncy_Deck::sINFO* pInfo = csnew CsEncy_Deck::sINFO;

			// 사용여부 확인, 0이면 사용 안함
			if( atoi( excel.GetData( DECK_COL_USE, cBuffer ) ) == 0 )
				continue;

			// 덱 번호
			pInfo->s_nGroupIdx = atoi( excel.GetData( DECK_COL_ID, cBuffer ) );

			// 덱 이름, 설명
			switch( nsCsFileTable::g_eLanguage )
			{
			case nsCsFileTable::KOREA_ORIGINAL:
			case nsCsFileTable::KOREA_TRANSLATION:
				{
					M2W( pInfo->s_szGroupName, excel.GetData( DECK_COL_NAME_KOR, cBuffer ), ENCY_NAME_LEN );
					M2W( pInfo->s_szExplain, excel.GetData( DECK_COL_EXPLAIN_KOR, cBuffer ), ENCY_EXPLAIN_LEN );
				}
				break;
			case nsCsFileTable::ENGLISH:
			case nsCsFileTable::ENGLISH_A:
				{
					M2W( pInfo->s_szGroupName, excel.GetData( DECK_COL_NAME_ENG, cBuffer ), ENCY_NAME_LEN );
					M2W( pInfo->s_szExplain, excel.GetData( DECK_COL_EXPLAIN_ENG, cBuffer ), ENCY_EXPLAIN_LEN );
				}
				break;
			case nsCsFileTable::THAILAND:
			case nsCsFileTable::TAIWAN:
			case nsCsFileTable::HONGKONG:
				{
					wcscpy_s( pInfo->s_szGroupName, ENCY_NAME_LEN, excel.GetData( DECK_COL_NAME_ENG, wBuffer) );
					wcscpy_s( pInfo->s_szExplain, ENCY_EXPLAIN_LEN, excel.GetData( DECK_COL_EXPLAIN_ENG, wBuffer) );
				}
				break;
			default:
				assert_cs( false );
			}

			int nOptIdx = DECK_COL_OPTION1;
			// 덱 옵션 셋팅
			for( int i = 0 ; i < ENCY_MAX_OPTION ; i++ )
			{
				pInfo->s_nCondition[i] = atoi( excel.GetData( nOptIdx++, cBuffer ) );
				pInfo->s_nAT_Type[i] = atoi( excel.GetData( nOptIdx++, cBuffer ) );
				pInfo->s_nOption[i] = atoi( excel.GetData( nOptIdx++, cBuffer ) );
				pInfo->s_nVal[i] = atoi( excel.GetData( nOptIdx++, cBuffer ) );
				pInfo->s_nProb[i] = atoi( excel.GetData( nOptIdx++, cBuffer ) );
				pInfo->s_nTime[i] = atoi( excel.GetData( nOptIdx++, cBuffer ) );
			}

			CsEncy_Deck* pEncy = csnew CsEncy_Deck;
			pEncy->Init( pInfo );
			assert_cs( m_mapEncy_Deck.find( pInfo->s_nGroupIdx ) == m_mapEncy_Deck.end() );
			m_mapEncy_Deck.insert(make_pair(pInfo->s_nGroupIdx, pEncy));
		}
		excel.Close();
	}	// 덱 정보( 이름, 설명, 옵션 )

	{	// 각 덱 구성 디지몬 정보
		#define COMP_SHEET_NAME						"Composition_Deck"
		#define COMP_COL_ID							0
		#define COMP_COL_NAME_KOR					COMP_COL_ID+1			//1
		#define COMP_COL_NAME_ENG					COMP_COL_NAME_KOR+1		//2
#ifndef VERSION_KOR
		#define COMP_COL_BASE_DIGIMON_ID			COMP_COL_NAME_ENG+1		//3
#else
		#define COMP_COL_BASE_DIGIMON_ID			COMP_COL_NAME_KOR+1		//2
#endif
		#define COMP_COL_BASE_DIGIMON_NAME			COMP_COL_BASE_DIGIMON_ID+1			//3	( 로컬은 + 1 )
		#define COMP_COL_DEST_DIGIMON_EVOL_SLOT		COMP_COL_BASE_DIGIMON_NAME+1		//4	( 로컬은 + 1 )
		#define COMP_COL_DEST_DIGIMON_ID			COMP_COL_DEST_DIGIMON_EVOL_SLOT+1	//4	( 로컬은 + 1 )
		#define COMP_COL_DEST_DIGIMON_NAME			COMP_COL_DEST_DIGIMON_ID+1			//5	( 로컬은 + 1 )


		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Digimon_Book(), COMP_SHEET_NAME );
		if( bSuccess == false )
		{
			TCHAR cTablePath[MAX_FILENAME];
			TCHAR cSheetname[MAX_FILENAME];
			M2W( cTablePath, nsCsFileTable::GetPath_DM_Digimon_Book(), MAX_FILENAME );
			M2W( cSheetname, COMP_SHEET_NAME, MAX_FILENAME );

			CsMessageBox( MB_OK, L"%s\n%s 시트 오픈 실패. Excel 수정이 필요합니다.", cTablePath, cSheetname );
			return false;
		}

		char cBuffer[ CEXCLE_CELL_LEN ];
		wchar_t wBuffer[ CEXCLE_CELL_LEN ];

		// 2줄은 버린다.
		bSuccess = excel.MoveFirst();
		assert_cs( bSuccess == true );	
		bSuccess = excel.MoveNext();
		assert_cs( bSuccess == true );

		UINT nIdx = 0;
		CsEncy_Deck_Composition::sINFO* pInfo = NULL;
		CsEncy_Deck_Composition::sDATA* pData = NULL;
		CsEncy_Deck_Composition* pEncy = NULL;
		while( excel.MoveNext() == true )
		{
			// 그룹 번호
			int nFindIdx = atoi( excel.GetData( COMP_COL_ID, cBuffer ) );

			if( 0 == nFindIdx )
				continue;

			if( m_mapEncy_Deck.find( nFindIdx ) == m_mapEncy_Deck.end() )
				continue;

			if( nIdx != nFindIdx  )
			{
				if( pInfo != NULL )
				{
					pEncy = csnew CsEncy_Deck_Composition;
					pEncy->Init( pInfo );
					m_mapEncy_Composition[ nIdx ] = pEncy;
				}
				nIdx = nFindIdx;

				pInfo = csnew CsEncy_Deck_Composition::sINFO;
				pInfo->s_nGroupIdx = nIdx;//atoi( excel.GetData( GROUP_COL_GROUP_ID, cBuffer ) );

				// 덱 이름
				switch( nsCsFileTable::g_eLanguage )
				{
				case nsCsFileTable::KOREA_ORIGINAL:
				case nsCsFileTable::KOREA_TRANSLATION:
					M2W( pInfo->s_szGroupName, excel.GetData( COMP_COL_NAME_KOR, cBuffer ), ENCY_NAME_LEN );
					break;
				case nsCsFileTable::ENGLISH:
				case nsCsFileTable::ENGLISH_A:
					M2W( pInfo->s_szGroupName, excel.GetData( COMP_COL_NAME_ENG, cBuffer ), ENCY_NAME_LEN );
					break;
				case nsCsFileTable::THAILAND:
				case nsCsFileTable::TAIWAN:
				case nsCsFileTable::HONGKONG:
					wcscpy_s( pInfo->s_szGroupName, ENCY_NAME_LEN, excel.GetData( COMP_COL_NAME_ENG, wBuffer) );
					break;
				default:
					assert_cs( false );
				}
			}

			pData = csnew CsEncy_Deck_Composition::sDATA;

			// 디지몬 계열체 번호
			pData->s_dwBaseDigimonID = atoi( excel.GetData( COMP_COL_BASE_DIGIMON_ID, cBuffer ) );
			// 덱 구성 필요 디지몬 진화 슬롯
			pData->s_nEvolslot = atoi( excel.GetData( COMP_COL_DEST_DIGIMON_EVOL_SLOT, cBuffer ) );
			// 덱 구성 필요 디지몬 번호
			pData->s_dwDestDigimonID = atoi( excel.GetData( COMP_COL_DEST_DIGIMON_ID, cBuffer ) );

			// 디지몬 계열체 이름, 필요 디지몬 이름
			M2W( pData->s_szBaseDigimonName, excel.GetData( COMP_COL_BASE_DIGIMON_NAME, cBuffer ), ENCY_NAME_LEN );
			M2W( pData->s_szDestDigimonName, excel.GetData( COMP_COL_DEST_DIGIMON_NAME, cBuffer ), ENCY_NAME_LEN );

			pInfo->InsertListData( pData );
		}
		if( nIdx != 0 )
		{
			pEncy = csnew CsEncy_Deck_Composition;
			pEncy->Init( pInfo );

			assert_cs( m_mapEncy_Composition.find( nIdx ) == m_mapEncy_Composition.end() );
			m_mapEncy_Composition.insert(make_pair(nIdx, pEncy));
		}
		excel.Close();
	}	// 각 덱 구성 디지몬 정보
	return true;
}



void CsDigimonMng::__LoadExcelPassiveEffect()
{
#define PASSIVE_EFFECT_SHEET_NAME			"Passive_effect"


#define	COL_DIGIMONID	0
#define	COL_EVO_NUM		1
#define	COL_ABILITYTYPE	2
#define	COL_VALUE1		3			// 기능 별 데이터

// 여분
// #define	COL_VALUE2		4
// #define	COL_VALUE3		5
// #define	COL_VALUE4		6

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Digimon(), PASSIVE_EFFECT_SHEET_NAME );
	assert_cs( bSuccess == true );	

	char cBuffer[ CEXCLE_CELL_LEN ];
// 	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 첫번째 줄로 이동
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	

	// 1줄 버림
 	bSuccess = excel.MoveNext();
 	assert_cs( bSuccess == true );

	CsPassive_Ability_Digimon::sINFO digiInfo;
	// 2줄 버리고 3번줄 부터 시작
	while( excel.MoveNext() == true )
	{
		// Digimon ID
		digiInfo.s_dwDigimonID = atoi( excel.GetData( COL_DIGIMONID, cBuffer ) );
		digiInfo.s_shEvoNum = atoi( excel.GetData( COL_EVO_NUM, cBuffer ) );
		digiInfo.s_nType = atoi( excel.GetData( COL_ABILITYTYPE, cBuffer ) );
		digiInfo.s_nValue1 = atoi( excel.GetData( COL_VALUE1, cBuffer ) );


		//-------------------- 오류 검사 -------------------//
		// 1. digimonID가 0이면 무시
		if( digiInfo.s_dwDigimonID == 0 )
			continue;

		// 2. 데이터의 범위 검사 0 <= 이면 오류, max >= 이면 오류 
		if( digiInfo.s_nType <= nsCsDigimonTable::PA_NONE || digiInfo.s_nType >= nsCsDigimonTable::PA_MAX )// 현재 범위의 타입의 문제 발생
		{
			assert_csm4( false, _T( "DigimonID : %d, Evo_Num : %d, Type : %d, Value : %d" ),
				digiInfo.s_dwDigimonID, digiInfo.s_shEvoNum, digiInfo.s_nType, digiInfo.s_nValue1 );
		}


		CsPassive_Ability_Digimon* pPassiveAbility = csnew CsPassive_Ability_Digimon;
		pPassiveAbility->Init( &digiInfo );

		// [타입 배열][디지몬ID]
		assert_cs( m_mapPassiveAbiltiy[digiInfo.s_nType].find( digiInfo.s_dwDigimonID ) == m_mapPassiveAbiltiy[digiInfo.s_nType].end() );
		m_mapPassiveAbiltiy[digiInfo.s_nType].insert(make_pair(digiInfo.s_dwDigimonID, pPassiveAbility));
	}	

	excel.Close();
}


#define	DATA_EXCHANGE_SHEET		"Data_Exchange"
void CsDigimonMng::__LoadExcelData_Exchange()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Digimon(), DATA_EXCHANGE_SHEET );
	assert_cs( bSuccess == true );	

	char cBuffer[ CEXCLE_CELL_LEN ];

	// 첫번째 줄로 이동
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	

	// 1줄 버림
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	// 줄 버림
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsData_Exchange::sINFO _Info;
	// 2줄 버리고 3번줄 부터 시작
	do 
	{
		int nColumn = 0;

		// 아이템 번호
		_Info.Item_Index = atoi( excel.GetData( nColumn++, cBuffer ) );

		// 아이템 타입
		_Info.Item_Type = atoi( excel.GetData( nColumn++, cBuffer ) );
		
		// 아이템 코드
		_Info.Item_Code = atoi( excel.GetData( nColumn++, cBuffer ) );

		// 아이템 이름
		sprintf_s(_Info.Item_Name, MAX_PATH, "%s", excel.GetData( nColumn++, cBuffer ));

		// 아이템 소모 우선 순위
		_Info.Use_Rank = atoi( excel.GetData( nColumn++, cBuffer ) );

		// 아이템 필요 개수
		_Info.Req_Num = atoi( excel.GetData( nColumn, cBuffer ) );


		CsData_Exchange::MAP_Iter	miter = m_mapData_Exchange.find(  _Info.Item_Type );

		if( miter == m_mapData_Exchange.end())
		{
			auto _rib = m_mapData_Exchange.insert(make_pair(_Info.Item_Type, CsData_Exchange::LIST_INFO() ));
			 miter = _rib.first;
		}

		miter->second.push_back( _Info );

	} while( excel.MoveNext() == true );

	excel.Close();
}

CsDigimon* CsDigimonMng::GetDigimon( DWORD dwDigimonID )
{
	CsDigimon::MAP_IT it = m_mapDigimon.find( dwDigimonID );
	if( it == m_mapDigimon.end() )
		return NULL;

	return it->second;
// 	assert_csm1( m_mapDigimon.find( dwDigimonID ) != m_mapDigimon.end(), _T( "dwDigimonID = %d" ), dwDigimonID );
// 	return m_mapDigimon[ dwDigimonID ];
}

int CsDigimonMng::GetDigimonRank( DWORD const& dwDigimonID ) const
{
	CsDigimon::MAP_CIT it = m_mapDigimon.find( dwDigimonID );
	if( it == m_mapDigimon.end() )
		return 0;

	SAFE_POINTER_RETVAL( it->second, 0 );
	CsDigimon::sINFO const* pInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, 0 );
	return pInfo->s_nDigimonRank;
}

CsRide* CsDigimonMng::GetRide( DWORD dwDigimonID )
{ 
	CsRide::MAP_IT it = m_mapRide.find( dwDigimonID );
	if( it == m_mapRide.end() )
		return NULL;
	return it->second;
}

bool CsDigimonMng::IsDigimon( DWORD dwDigimonID )
{ 
	return ( m_mapDigimon.find( dwDigimonID ) != m_mapDigimon.end() ); 
}

void CsDigimonMng::FindDigimon_FromModelID( DWORD const& dwModelID, std::list<DWORD>& listDigimonID )
{
	CsDigimon::MAP_IT it = m_mapDigimon.begin();
	for( ; it != m_mapDigimon.end(); ++it )
	{
		SAFE_POINTER_CON(it->second);
		CsDigimon::sINFO* pInfo = it->second->GetInfo();
		SAFE_POINTER_CON(pInfo);
		if( pInfo->s_dwModelID == dwModelID )
			listDigimonID.push_back( it->first );
	}
}

CsDigimon::MAP* CsDigimonMng::GetDigimonMap()
{ 
	return &m_mapDigimon; 
}

CsRide::MAP* CsDigimonMng::GetRideMap()
{ 
	return &m_mapRide;
}

bool CsDigimonMng::IsRide( DWORD dwDigimonID )
{ 
	return ( m_mapRide.find( dwDigimonID ) != m_mapRide.end() ); 
}

#ifdef CROSSWARS_SYSTEM
CsCrossDigimon* CsDigimonMng::GetCrossDigimon( DWORD dwDigimonID )
{
	CsCrossDigimon::MAP_IT it = m_mapCrossDigimon.find( dwDigimonID );
	if( it == m_mapCrossDigimon.end() )
		return NULL;

	return it->second;
}

bool CsDigimonMng::IsCrossDigimon( DWORD dwDigimonID )
{ 
	return( m_mapCrossDigimon.find( dwDigimonID ) != m_mapCrossDigimon.end() ); 
}

CsCrossDigimon::MAP* CsDigimonMng::GetCrossDigimonMap() 
{ 
	return &m_mapCrossDigimon;
}

CsCrossDigimon* CsDigimonMng::GetCrossCombine( DWORD dwDigimonID )
{
	CsCrossDigimon::MAP_IT it = m_mapCrossCombines.find( dwDigimonID );
	if( it == m_mapCrossCombines.end() )
		return NULL;
	return it->second;
}

bool CsDigimonMng::IsCrossCombine( DWORD dwDigimonID )
{ 
	return( m_mapCrossCombines.find( dwDigimonID ) != m_mapCrossCombines.end() ); 
}

CsCrossDigimon::MAP* CsDigimonMng::GetCrossCombineMap() 
{ 
	return &m_mapCrossCombines;
}

CsCrossScene* CsDigimonMng::GetCrossScene( DWORD dwDigimonID )
{
	CsCrossScene::MAP_IT it = m_mapCrossScene.find( dwDigimonID );
	if( it == m_mapCrossScene.end() )
		return NULL;
	return it->second;
}

bool CsDigimonMng::IsCrossScene( DWORD dwDigimonID )
{ 
	return( m_mapCrossScene.find( dwDigimonID ) != m_mapCrossScene.end() ); 
}

CsCrossScene::MAP* CsDigimonMng::GetCrossSceneMap() 
{ 
	return &m_mapCrossScene;
}

#endif

CsPassive_Ability_Digimon* CsDigimonMng::GetPassiveAbility( DWORD dwDigimonID, int nType )
{
	// type 범위 에러

	bool bError = nType >= nsCsDigimonTable::PA_START || nType < nsCsDigimonTable::PA_MAX ? true : false;

	assert_csm2( bError , _T("Type Not Find, DigimonID : %d, Type : %d"), dwDigimonID, nType );
		
	CsPassive_Ability_Digimon::MAP_IT iter = m_mapPassiveAbiltiy[nType].find( dwDigimonID );

	// 패시브 기능이 있는 디지몬인 경우에만 검사하므로 assert를 호출하지 않습니다.
	if( iter == m_mapPassiveAbiltiy[nType].end() )
		return NULL;

	return iter->second;
}

#define DIGIMON_TABLE_BIN		"Digimon_List.bin"
#ifdef CROSSWARS_SYSTEM
#define CROSSDIGIMON_TABLE_BIN	"CrossDigimon_List.bin"
#endif
#define RIDE_TABLE_BIN			"Ride.bin"
#define NEW_ELEMENT_TABLE_BIN	"New_Element.bin"
#define NEW_EFFECT_TABLE_BIN	"Digimon_Improvement_Effect.bin"
#define ENCYCLOPEDIA_BIN		"Digimon_Book.bin"
#define PASSIVE_ABILITY_BIN		"Passive_Ability.bin"
#define	DATA_EXCHAGE_SHEET_BIN	"Data_Exchange.bin"

void CsDigimonMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	FILE *fp = NULL;

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DIGIMON_TABLE_BIN );

		fopen_s( &fp, cName, "wb" );
		assert_cs( fp != NULL );

		int nCount = (int)m_mapDigimon.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsDigimon::MAP_IT it = m_mapDigimon.begin();
		CsDigimon::MAP_IT itEnd = m_mapDigimon.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsDigimon::sINFO ), 1, fp );
		}

		fclose( fp );
	}
#ifdef CROSSWARS_SYSTEM
	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CROSSDIGIMON_TABLE_BIN );

		fopen_s( &fp, cName, "wb" );
		assert_cs( fp != NULL );

		int nCount = (int)m_mapCrossDigimon.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsCrossDigimon::MAP_IT it = m_mapCrossDigimon.begin();
		CsCrossDigimon::MAP_IT itEnd = m_mapCrossDigimon.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsCrossDigimon::sINFO ), 1, fp );
		}

		
		nCount = (int)m_mapCrossCombines.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		it = m_mapCrossCombines.begin();
		itEnd = m_mapCrossCombines.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsCrossDigimon::sINFO ), 1, fp );
		}

		
		nCount = (int)m_mapCrossScene.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsCrossScene::MAP_IT it2 = m_mapCrossScene.begin();
		CsCrossScene::MAP_IT itEnd2 = m_mapCrossScene.end();
		for( ; it2!=itEnd2; ++it2 )
		{
			fwrite( it2->second->GetInfo(), sizeof( CsCrossScene::sINFO ), 1, fp );
		}
		
		fclose( fp );
	}
#endif

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, RIDE_TABLE_BIN );

		fopen_s( &fp, cName, "wb" );
		assert_cs( fp != NULL );

		int nCount = (int)m_mapRide.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsRide::MAP_IT it = m_mapRide.begin();
		CsRide::MAP_IT itEnd = m_mapRide.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsRide::sINFO ), 1, fp );
		}

		fclose( fp );
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, NEW_ELEMENT_TABLE_BIN );

		fopen_s( &fp, cName, "wb" );
		assert_cs( fp != NULL );

		{
			int nCount = (int)m_mapAttributeTypeUI.size();
			fwrite( &nCount, sizeof( int ), 1, fp );

			CsAttributeTypeUI::MAP_IT it = m_mapAttributeTypeUI.begin();
			CsAttributeTypeUI::MAP_IT itEnd = m_mapAttributeTypeUI.end();
			for( ; it!=itEnd; ++it )
			{
				fwrite( it->second->GetInfo(), sizeof( CsAttributeTypeUI::sINFO ), 1, fp );
			}
		}
		{
			int nCount = (int)m_mapNatureTypeUI.size();
			fwrite( &nCount, sizeof( int ), 1, fp );

			CsNatureTypeUI::MAP_IT it = m_mapNatureTypeUI.begin();
			CsNatureTypeUI::MAP_IT itEnd = m_mapNatureTypeUI.end();
			for( ; it!=itEnd; ++it )
			{
				fwrite( it->second->GetInfo(), sizeof( CsNatureTypeUI::sINFO ), 1, fp );
			}
		}


		fclose( fp );
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, ENCYCLOPEDIA_BIN );

		fopen_s( &fp, cName, "wb" );
		assert_cs( fp != NULL );

		//옵션 정보 ( Data 시트 )
		int nCount = (int)m_mapEncy_OptData.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsEncy_OptData::MAP_IT it = m_mapEncy_OptData.begin();
		CsEncy_OptData::MAP_IT itEnd = m_mapEncy_OptData.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsEncy_OptData::sINFO ), 1, fp );
		}

		//제외 디지몬 ( Exception_Digimon 시트 )
		nCount = (int)m_mapEncy_Exception_Digimon.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsEncy_Exception_Digimon::MAP_IT it1 = m_mapEncy_Exception_Digimon.begin();
		CsEncy_Exception_Digimon::MAP_IT itEnd1 = m_mapEncy_Exception_Digimon.end();
		for( ; it1!=itEnd1; ++it1 )
		{
			fwrite( it1->second->GetInfo(), sizeof( CsEncy_Exception_Digimon::sINFO ), 1, fp );
		}

		// 덱 정보 ( Book_Deck 시트 )
		nCount = (int)m_mapEncy_Deck.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsEncy_Deck::MAP_IT it2 = m_mapEncy_Deck.begin();
		CsEncy_Deck::MAP_IT itEnd2 = m_mapEncy_Deck.end();
		for( ; it2!=itEnd2; ++it2 )
		{
			fwrite( it2->second->GetInfo(), sizeof( CsEncy_Deck::sINFO ), 1, fp );
		}

		//그룹 ( Composition_Deck 시트 )
		nCount = (int)m_mapEncy_Composition.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsEncy_Deck_Composition::MAP_IT it3 = m_mapEncy_Composition.begin();
		CsEncy_Deck_Composition::MAP_IT itEnd3 = m_mapEncy_Composition.end();
		for( ; it3!=itEnd3; ++it3 )
		{
			CsEncy_Deck_Composition::sINFO* pInfo = it3->second->GetInfo();
			USHORT nListSize = (USHORT)(pInfo->s_pListData.size());

			fwrite( &(pInfo->s_nGroupIdx), sizeof( USHORT ), 1, fp );
			fwrite( &nListSize, sizeof( USHORT ), 1, fp );

			std::list< CsEncy_Deck_Composition::sDATA* >::iterator itList = pInfo->s_pListData.begin();
			std::list< CsEncy_Deck_Composition::sDATA* >::iterator itListEnd = pInfo->s_pListData.end();

			CsEncy_Deck_Composition::sDATA* pData = csnew CsEncy_Deck_Composition::sDATA;
			
			for( ; itList != itListEnd ; itList++ )
			{
				memcpy( &(pData->s_dwBaseDigimonID), &((*itList)->s_dwBaseDigimonID), sizeof( DWORD ));
				memcpy( &(pData->s_szBaseDigimonName), &((*itList)->s_szBaseDigimonName), sizeof( TCHAR ) * MAX_FILENAME );
 				memcpy( &(pData->s_dwDestDigimonID), &((*itList)->s_dwDestDigimonID), sizeof( DWORD ));
 				memcpy( &(pData->s_szDestDigimonName), &((*itList)->s_szDestDigimonName), sizeof( TCHAR ) * MAX_FILENAME );

				fwrite( pData, sizeof( CsEncy_Deck_Composition::sDATA ), 1, fp );
			}
			
		}

		fclose( fp );
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, PASSIVE_ABILITY_BIN );

		fopen_s( &fp, cName, "wb" );
		assert_cs( fp != NULL );

		for(int i = nsCsDigimonTable::PA_START; i < nsCsDigimonTable::PA_MAX; ++i)
		{
			int nCount = (int)m_mapPassiveAbiltiy[i].size();
			fwrite( &nCount, sizeof( int ), 1, fp );		// 수량 작성

			CsPassive_Ability_Digimon::MAP_IT it = m_mapPassiveAbiltiy[i].begin();
			CsPassive_Ability_Digimon::MAP_IT itEnd = m_mapPassiveAbiltiy[i].end();
			for( ; it!=itEnd; ++it )
			{
				fwrite( it->second->GetInfo(), sizeof( CsPassive_Ability_Digimon::sINFO ), 1, fp );
			}
		}


		fclose( fp );
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DATA_EXCHAGE_SHEET_BIN );

		fopen_s( &fp, cName, "wb" );
		assert_cs( fp != NULL );

		{
			int nCount = (int)m_mapData_Exchange.size();
			fwrite( &nCount, sizeof( int ), 1, fp );

			CsData_Exchange::MAP_cIter it = m_mapData_Exchange.begin(); 
			CsData_Exchange::MAP_cIter itEnd = m_mapData_Exchange.end();
			for( ; it!=itEnd; ++it )
			{
				int nListSize = (int)it->second.size();
				fwrite( &nListSize, sizeof( int ), 1, fp );

				CsData_Exchange::LIST_INFO_CITER	info_it = it->second.begin();
				CsData_Exchange::LIST_INFO_CITER	info_itEnd = it->second.end();
				
				for( ; info_it!=info_itEnd; ++info_it )
				{
					CsData_Exchange::sINFO	datainfo = *info_it;
					fwrite(&datainfo, sizeof( CsData_Exchange::sINFO ), 1, fp );
				}
			}
		}

		fclose( fp );
	}
}

bool CsDigimonMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	FILE *fp = NULL;

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DIGIMON_TABLE_BIN );

		
		fopen_s( &fp, cName, "rb" );
		if( fp == NULL )
		{
			CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
			return false;
		}

		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsDigimon::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsDigimon::sINFO ), 1, fp );

			CsDigimon* pObject = csnew CsDigimon;
			pObject->Init( &info );
			assert_cs( m_mapDigimon.find( info.s_dwDigimonID ) == m_mapDigimon.end() );
			m_mapDigimon.insert(make_pair(info.s_dwDigimonID, pObject));
		}

		fclose( fp );
	}
#ifdef CROSSWARS_SYSTEM
	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CROSSDIGIMON_TABLE_BIN );


		fopen_s( &fp, cName, "rb" );
		if( fp == NULL )
		{
			CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
			return false;
		}

		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsCrossDigimon::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsCrossDigimon::sINFO ), 1, fp );

			CsCrossDigimon* pObject = csnew CsCrossDigimon;
			pObject->Init( &info );
			assert_cs( m_mapCrossDigimon.find( info.s_dwDigimonID ) == m_mapCrossDigimon.end() );
			m_mapCrossDigimon.insert(make_pair(info.s_dwDigimonID, pObject));
		}

	
		fread( &nCount, sizeof( int ), 1, fp );		
		for( int i=0; i<nCount; ++i )
		{
			
			fread( &info, sizeof( CsCrossDigimon::sINFO ), 1, fp );

			CsCrossDigimon* pObject = csnew CsCrossDigimon;
			pObject->Init( &info );
			assert_cs( m_mapCrossCombines.find( info.s_dwDigimonID ) == m_mapCrossCombines.end() );
			m_mapCrossCombines.insert(make_pair(info.s_dwDigimonID, pObject));
		}



		fread( &nCount, sizeof( int ), 1, fp );

		CsCrossScene::sINFO info2;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info2, sizeof( CsCrossScene::sINFO ), 1, fp );

			CsCrossScene* pObject = csnew CsCrossScene;
			pObject->Init( &info2 );
			assert_cs( m_mapCrossScene.find( info2.s_dwDigimonID ) == m_mapCrossScene.end() );
			m_mapCrossScene.insert(make_pair(info2.s_dwDigimonID, pObject));
		}

		fclose( fp );
	}
#endif
	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, RIDE_TABLE_BIN );

		fopen_s( &fp, cName, "rb" );
		if( fp == NULL )
		{
			CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
			return false;
		}

		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsRide::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsRide::sINFO ), 1, fp );

			CsRide* pObject = csnew CsRide;
			pObject->Init( &info );
			assert_cs( m_mapRide.find( info.s_dwDigimonID ) == m_mapRide.end() );
			m_mapRide.insert(make_pair(info.s_dwDigimonID, pObject));
		}

		fclose( fp );
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, NEW_ELEMENT_TABLE_BIN );

		fopen_s( &fp, cName, "rb" );
		if( fp == NULL )
		{
			CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
			return false;
		}

		int nCount;
		{
			fread( &nCount, sizeof( int ), 1, fp );
			CsAttributeTypeUI::sINFO info;
			for( int i=0; i<nCount; ++i )
			{
				fread( &info, sizeof( CsAttributeTypeUI::sINFO ), 1, fp );

				CsAttributeTypeUI* pObject = csnew CsAttributeTypeUI;
				pObject->Init( &info );
				assert_cs( m_mapAttributeTypeUI.find( info.s_nAttID ) == m_mapAttributeTypeUI.end() );
				m_mapAttributeTypeUI.insert(make_pair(info.s_nAttID, pObject));
			}
		}

		{
			fread( &nCount, sizeof( int ), 1, fp );
			CsNatureTypeUI::sINFO Natinfo;
			for( int i=0; i<nCount; ++i )
			{
				fread( &Natinfo, sizeof( CsNatureTypeUI::sINFO ), 1, fp );

				CsNatureTypeUI* pObject = csnew CsNatureTypeUI;
				pObject->Init( &Natinfo );
				assert_cs( m_mapNatureTypeUI.find( Natinfo.s_nNatID ) == m_mapNatureTypeUI.end() );
				m_mapNatureTypeUI.insert(make_pair(Natinfo.s_nNatID, pObject));
			}
		}

		fclose( fp );
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, ENCYCLOPEDIA_BIN );

		fopen_s( &fp, cName, "rb" );
		if( fp == NULL )
		{
			CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
			return false;
		}

		//옵션 정보 (Data 시트)
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsEncy_OptData::sINFO info_Opt;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info_Opt, sizeof( CsEncy_OptData::sINFO ), 1, fp );

			CsEncy_OptData* pObject = csnew CsEncy_OptData;
			pObject->Init( &info_Opt );
			assert_cs( m_mapEncy_OptData.find( info_Opt.s_dwOptID ) == m_mapEncy_OptData.end() );
			m_mapEncy_OptData.insert(make_pair(info_Opt.s_dwOptID, pObject));
		}

		//제외 디지몬 (Exception_Digimon 시트)
		fread( &nCount, sizeof( int ), 1, fp );

		CsEncy_Exception_Digimon::sINFO info_Ex;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info_Ex, sizeof( CsEncy_Exception_Digimon::sINFO ), 1, fp );

			CsEncy_Exception_Digimon* pObject = csnew CsEncy_Exception_Digimon;
			pObject->Init( &info_Ex );
			assert_cs( m_mapEncy_Exception_Digimon.find( info_Ex.s_dwDigimonID ) == m_mapEncy_Exception_Digimon.end() );
			m_mapEncy_Exception_Digimon.insert(make_pair(info_Ex.s_dwDigimonID, pObject));
		}

		//덱 정보 (Book_Deck 시트)
		fread( &nCount, sizeof( int ), 1, fp );

		CsEncy_Deck::sINFO info_Deck;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info_Deck, sizeof( CsEncy_Deck::sINFO ), 1, fp );

			CsEncy_Deck* pObject = csnew CsEncy_Deck;
			pObject->Init( &info_Deck );
			assert_cs( m_mapEncy_Deck.find( info_Deck.s_nGroupIdx ) == m_mapEncy_Deck.end() );
			m_mapEncy_Deck.insert(make_pair(info_Deck.s_nGroupIdx, pObject));
		}

		//그룹 (Composition_Deck 시트)
		fread( &nCount, sizeof( int ), 1, fp );

		for( int i=0; i<nCount; ++i )
		{
			USHORT nVal;
			CsEncy_Deck_Composition::sINFO* info_Group = csnew CsEncy_Deck_Composition::sINFO;

			fread( &nVal, sizeof( USHORT ), 1, fp );
			info_Group->s_nGroupIdx = nVal;

			fread( &nVal, sizeof( USHORT ), 1, fp );

			for( int j = 0; j < nVal ; j++ )
			{
				CsEncy_Deck_Composition::sDATA* pData = csnew CsEncy_Deck_Composition::sDATA;
				fread( pData, sizeof( CsEncy_Deck_Composition::sDATA ), 1, fp );

				info_Group->InsertListData( pData );
			}

			CsEncy_Deck_Composition* pObject = csnew CsEncy_Deck_Composition;
			pObject->Init( info_Group );
			assert_cs( m_mapEncy_Composition.find( info_Group->s_nGroupIdx ) == m_mapEncy_Composition.end() );
			m_mapEncy_Composition.insert(make_pair(info_Group->s_nGroupIdx, pObject));
		}

		fclose( fp );
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, PASSIVE_ABILITY_BIN );

		fopen_s( &fp, cName, "rb" );
		if( fp == NULL )
		{
			CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
			return false;
		}

		for(int nType = nsCsDigimonTable::PA_START; nType < nsCsDigimonTable::PA_MAX; ++nType)
		{
			int nCount;
			fread( &nCount, sizeof( int ), 1, fp );	// 수량 읽기


			CsPassive_Ability_Digimon::sINFO info;
			for( int i = 0; i < nCount; ++i )
			{
				fread( &info, sizeof( CsPassive_Ability_Digimon::sINFO ), 1, fp );

				CsPassive_Ability_Digimon* pObject = csnew CsPassive_Ability_Digimon;
				pObject->Init( &info );
				assert_cs( m_mapPassiveAbiltiy[nType].find( info.s_dwDigimonID ) == m_mapPassiveAbiltiy[nType].end() );
				m_mapPassiveAbiltiy[nType].insert(make_pair(info.s_dwDigimonID, pObject));
			}
		}

		fclose( fp );
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DATA_EXCHAGE_SHEET_BIN );

		fopen_s( &fp, cName, "rb" );
		if( fp == NULL )
		{
			CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
			return false;
		}

		int nCount = 0;
		{
			fread( &nCount, sizeof( int ), 1, fp );
			for( int i=0; i<nCount; ++i )
			{
				int ListSize = 0;
				fread( &ListSize, sizeof( int ), 1, fp );
				for( int j=0; j<ListSize; ++j )
				{
					CsData_Exchange::sINFO info;
					fread( &info, sizeof( CsData_Exchange::sINFO ), 1, fp );

					CsData_Exchange::MAP_Iter	miter = m_mapData_Exchange.find(  info.Item_Type );

					if( miter == m_mapData_Exchange.end())
					{
						auto _rib = m_mapData_Exchange.insert(make_pair(info.Item_Type, CsData_Exchange::LIST_INFO() ));
						miter = _rib.first;
					}
					miter->second.push_back( info );
				}			
			}
		}

		fclose( fp );
	}

	return true;
}

void CsDigimonMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DIGIMON_TABLE_BIN );

		if( CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		{
			int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
			int nCount;
			_read( nHandle, &nCount, sizeof( int ) );

			CsDigimon::sINFO info;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &info, sizeof( CsDigimon::sINFO ) );

				CsDigimon* pObject = csnew CsDigimon;
				pObject->Init( &info );
				assert_cs( m_mapDigimon.find( info.s_dwDigimonID ) == m_mapDigimon.end() );
				m_mapDigimon[ info.s_dwDigimonID ] = pObject;
			}
			CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
		}
	}

#ifdef CROSSWARS_SYSTEM
	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, CROSSDIGIMON_TABLE_BIN );

		if( CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		{
			int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
			int nCount;
			_read( nHandle, &nCount, sizeof( int ) );

			CsCrossDigimon::sINFO info;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &info, sizeof( CsCrossDigimon::sINFO ) );

				CsCrossDigimon* pObject = csnew CsCrossDigimon;
				pObject->Init( &info );
				assert_cs( m_mapCrossDigimon.find( info.s_dwDigimonID ) == m_mapCrossDigimon.end() );
				m_mapCrossDigimon[ info.s_dwDigimonID ] = pObject;
			}


			_read( nHandle, &nCount, sizeof( int ) );

			for( int i=0; i<nCount; ++i )
			{
				CsCrossDigimon::sINFO info;
				_read( nHandle, &info, sizeof( CsCrossDigimon::sINFO ) );

				CsCrossDigimon* pObject = csnew CsCrossDigimon;
				pObject->Init( &info );
				assert_cs( m_mapCrossCombines.find( info.s_dwDigimonID ) == m_mapCrossCombines.end() );
				m_mapCrossCombines[ info.s_dwDigimonID ] = pObject;
			}


			_read( nHandle, &nCount, sizeof( int ) );

			CsCrossScene::sINFO info2;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &info2, sizeof( CsCrossScene::sINFO ) );

				CsCrossScene* pObject = csnew CsCrossScene;
				pObject->Init( &info2 );
				assert_cs( m_mapCrossScene.find( info2.s_dwDigimonID ) == m_mapCrossScene.end() );
				m_mapCrossScene[ info2.s_dwDigimonID ] = pObject;
			}
			CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
		}		
	}
#endif

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, RIDE_TABLE_BIN );

		if( CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		{
			int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

			int nCount;
			_read( nHandle, &nCount, sizeof( int ) );

			CsRide::sINFO info;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &info, sizeof( CsRide::sINFO ) );

				CsRide* pObject = csnew CsRide;
				pObject->Init( &info );
				assert_cs( m_mapRide.find( info.s_dwDigimonID ) == m_mapRide.end() );
				m_mapRide[ info.s_dwDigimonID ] = pObject;
			}
			CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
		}
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, NEW_ELEMENT_TABLE_BIN );

		if( CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		{
			int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
			int nCount;
			{
				_read( nHandle, &nCount, sizeof( int ) );
				CsAttributeTypeUI::sINFO info;
				for( int i=0; i<nCount; ++i )
				{
					_read( nHandle, &info, sizeof( CsAttributeTypeUI::sINFO ) );

					CsAttributeTypeUI* pObject = csnew CsAttributeTypeUI;
					pObject->Init( &info );
					assert_cs( m_mapAttributeTypeUI.find( info.s_nAttID ) == m_mapAttributeTypeUI.end() );
					m_mapAttributeTypeUI[ info.s_nAttID ] = pObject;
				}
			}
			{
				_read( nHandle, &nCount, sizeof( int ) );
				CsNatureTypeUI::sINFO info;
				for( int i=0; i<nCount; ++i )
				{
					_read( nHandle, &info, sizeof( CsNatureTypeUI::sINFO ) );

					CsNatureTypeUI* pObject = csnew CsNatureTypeUI;
					pObject->Init( &info );
					assert_cs( m_mapNatureTypeUI.find( info.s_nNatID ) == m_mapNatureTypeUI.end() );
					m_mapNatureTypeUI[ info.s_nNatID ] = pObject;
				}
			}
			CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
		}
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, ENCYCLOPEDIA_BIN );

		if( CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		{
			int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

			//옵션 정보 (Data 시트)
			int nCount;
			_read( nHandle, &nCount, sizeof( int ) );

			CsEncy_OptData::sINFO info_Opt;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &info_Opt, sizeof( CsEncy_OptData::sINFO ) );

				CsEncy_OptData* pObject = csnew CsEncy_OptData;
				pObject->Init( &info_Opt );
				assert_cs( m_mapEncy_OptData.find( info_Opt.s_dwOptID ) == m_mapEncy_OptData.end() );
				m_mapEncy_OptData[ info_Opt.s_dwOptID ] = pObject;
			}

			//제외 디지몬 (Exception_Digimon 시트)
			_read( nHandle, &nCount, sizeof( int ) );

			CsEncy_Exception_Digimon::sINFO info_Ex;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &info_Ex, sizeof( CsEncy_Exception_Digimon::sINFO ) );

				CsEncy_Exception_Digimon* pObject = csnew CsEncy_Exception_Digimon;
				pObject->Init( &info_Ex );
				assert_cs( m_mapEncy_Exception_Digimon.find( info_Ex.s_dwDigimonID ) == m_mapEncy_Exception_Digimon.end() );
				m_mapEncy_Exception_Digimon[ info_Ex.s_dwDigimonID ] = pObject;
			}

			//덱 정보 (Book_Deck 시트)
			_read( nHandle, &nCount, sizeof( int ) );

			CsEncy_Deck::sINFO info_Deck;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &info_Deck, sizeof( CsEncy_Deck::sINFO ) );

				CsEncy_Deck* pObject = csnew CsEncy_Deck;
				pObject->Init( &info_Deck );
				assert_cs( m_mapEncy_Deck.find( info_Deck.s_nGroupIdx ) == m_mapEncy_Deck.end() );
				m_mapEncy_Deck[ info_Deck.s_nGroupIdx ] = pObject;
			}

			//그룹 (Composition_Deck 시트)
			_read( nHandle, &nCount, sizeof( int ) );

			for( int i=0; i<nCount; ++i )
			{

				USHORT nVal;
				
				CsEncy_Deck_Composition::sINFO* info_Group = csnew CsEncy_Deck_Composition::sINFO;;

				_read( nHandle, &nVal, sizeof( USHORT ) );
				info_Group->s_nGroupIdx = nVal;

				_read( nHandle, &nVal, sizeof( USHORT ) );
				for( int j = 0; j < nVal ; j++ )
				{
					CsEncy_Deck_Composition::sDATA* pData = csnew CsEncy_Deck_Composition::sDATA;
					_read( nHandle, pData, sizeof( CsEncy_Deck_Composition::sDATA ) );
					info_Group->InsertListData( pData );
				}

				CsEncy_Deck_Composition* pObject = csnew CsEncy_Deck_Composition;
				pObject->Init( info_Group );
				assert_cs( m_mapEncy_Composition.find( info_Group->s_nGroupIdx ) == m_mapEncy_Composition.end() );
				m_mapEncy_Composition[ info_Group->s_nGroupIdx ] = pObject;
			}
			CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
		}
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, PASSIVE_ABILITY_BIN );

		if( CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		{
			int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
			for(int nType = nsCsDigimonTable::PA_START; nType < nsCsDigimonTable::PA_MAX; ++nType)
			{
				int nCount = 0;
				_read( nHandle, &nCount, sizeof( int ) );	// 수량 읽기

				CsPassive_Ability_Digimon::sINFO info;
				for( int i = 0; i < nCount; ++i )
				{
					_read( nHandle, &info, sizeof( CsPassive_Ability_Digimon::sINFO ) );

					CsPassive_Ability_Digimon* pObject = csnew CsPassive_Ability_Digimon;
					pObject->Init( &info );
					assert_cs( m_mapPassiveAbiltiy[nType].find( info.s_dwDigimonID ) == m_mapPassiveAbiltiy[nType].end() );
					m_mapPassiveAbiltiy[nType][ info.s_dwDigimonID ] = pObject;
				}

			}
			CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
		}
	}

	{
		sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DATA_EXCHAGE_SHEET_BIN );

		if( CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		{
			int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

			int nCount = 0;
			_read( nHandle, &nCount, sizeof( int ) );	// 수량 읽기

			for( int i=0; i<nCount; ++i )
			{
				int nListCount = 0;
				_read( nHandle, &nListCount, sizeof( int ) );	// 수량 읽기

				for( int j=0; j<nListCount; ++j )
				{
					CsData_Exchange::sINFO info;
					_read( nHandle, &info, sizeof( CsData_Exchange::sINFO ) );

					CsData_Exchange::MAP_Iter	miter = m_mapData_Exchange.find(  info.Item_Type );

					if( miter == m_mapData_Exchange.end())
					{
						auto _rib = m_mapData_Exchange.insert(make_pair(info.Item_Type, CsData_Exchange::LIST_INFO() ));
						miter = _rib.first;
					}

					miter->second.push_back( info );
				}				
			}
		}
		CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
	}
}

DWORD CsDigimonMng::GetDigimonModelIdx( DWORD const& dwFTID ) const
{
	CsDigimon::MAP_CIT it = m_mapDigimon.find( dwFTID );
	if( it == m_mapDigimon.end() )
		return 0;

	SAFE_POINTER_RETVAL( it->second, 0 );
	CsDigimon::sINFO* pInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, 0 );

	return pInfo->s_dwModelID;
}

bool CsDigimonMng::_IsExceptionDigimon( DWORD dwDigimonID )
{ 
	return (m_mapEncy_Exception_Digimon.find( dwDigimonID) != m_mapEncy_Exception_Digimon.end()); 
}

CsEncy_OptData*	CsDigimonMng::GetOptData( int nIdx )
{ 
	CsEncy_OptData::MAP_IT it = m_mapEncy_OptData.find( nIdx );
	if( it == m_mapEncy_OptData.end() )
		return NULL;
	return it->second;
}

CsEncy_Deck* CsDigimonMng::GetDeck( int nIdx )
{ 
	CsEncy_Deck::MAP_IT it = m_mapEncy_Deck.find( nIdx );
	if( it == m_mapEncy_Deck.end() )
		return NULL;
	return it->second;
} 

CsEncy_Deck::MAP* CsDigimonMng::GetDeckMap() 
{
	return &m_mapEncy_Deck; 
}

CsEncy_Deck_Composition* CsDigimonMng::GetGroup( int nIdx )
{ 
	CsEncy_Deck_Composition::MAP_IT it = m_mapEncy_Composition.find( nIdx );
	if( it == m_mapEncy_Composition.end() )
		return NULL;
	return it->second;
} 

CsEncy_Deck_Composition::MAP* CsDigimonMng::GetGroupMap() 
{
	return &m_mapEncy_Composition; 
}