
#include "stdafx.h"
#include "MonsterMng.h"


void CsMonsterMng::Delete()
{
	{
		CsMonster::MAP_IT it = m_mapMonster.begin();
		CsMonster::MAP_IT itEnd = m_mapMonster.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapMonster.clear();
	}

	{
		CsMonsterDrop::MAP_IT it = m_mapMonsterDrop.begin();
		CsMonsterDrop::MAP_IT itEnd = m_mapMonsterDrop.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapMonsterDrop.clear();
	}

	m_mapMonsterHit.clear();
}

bool CsMonsterMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();	
		_LoadExcelHit();
		_LoadExcelSkill();
		_LoadExcelSkillTerms();
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

void CsMonsterMng::_LoadExcelHit()
{
#define MONSTER_DATA_SHEET_NAME				"Data"	

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Monster(), MONSTER_DATA_SHEET_NAME );
	assert_cs( bSuccess == true );

#define ML_COL_MONSTER_LV				1
#define ML_COL_MONSTER_HT				2

	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	int nLV;
	int nHT;
	while( excel.MoveNext() == true )
	{
		nLV = atoi( excel.GetData( ML_COL_MONSTER_LV, cBuffer ) );
		if( nLV == 0 )
			continue;

		nHT = atoi( excel.GetData( ML_COL_MONSTER_HT, cBuffer ) );

		m_mapMonsterHit[ nLV ] = nHT;
	}
}

void CsMonsterMng::_LoadExcel()
{
#define MONSTER_SHEET_NAME				"Monster"	

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Monster(), MONSTER_SHEET_NAME );
	assert_cs( bSuccess == true );

#define ML_COL_MONSTER_ID				1
#define ML_COL_MODEL_ID					ML_COL_MONSTER_ID + 1
#define ML_COL_NAME						ML_COL_MODEL_ID + 1
#define ML_COL_PEN_NAME					ML_COL_NAME + 1
#define ML_COL_NAME_ENG					ML_COL_PEN_NAME + 1
#define ML_COL_PEN_NAME_ENG				ML_COL_NAME_ENG + 1
#define ML_COL_LEVEL					ML_COL_PEN_NAME_ENG + 1
#define ML_COL_EXP						ML_COL_LEVEL + 1
#define ML_COL_BATTLE					ML_COL_EXP + 5
#define ML_COL_HP						ML_COL_BATTLE + 3
#define ML_COL_DS						ML_COL_HP + 2
#define ML_COL_DP						ML_COL_DS + 2
#define ML_COL_EV						ML_COL_DP + 2
#define ML_COL_MS						ML_COL_EV + 2
#define ML_COL_CA						ML_COL_MS + 2
#define ML_COL_AP						ML_COL_CA + 2
#define ML_COL_AS						ML_COL_AP + 2
#define ML_COL_AR						ML_COL_AS + 2
#define ML_COL_HT						ML_COL_AR + 2
#define ML_COL_SIGHT					ML_COL_HT + 4
#define ML_COL_HUNTRANGE				ML_COL_SIGHT + 2
#define ML_COL_SCALE					ML_COL_HUNTRANGE + 2
#define ML_COL_CLASS					ML_COL_SCALE + 1

// 몬스터리스트 드랍아이템 컬럼 부족 관련 스피릿 추가 시 컬럼 확장
#define ML_COL_CLASS_ICON				ML_COL_CLASS + 153

#define ML_COL_CLASS_ICON2				ML_COL_CLASS_ICON + 1
#define ML_COL_AGRASSIVE_ICON			ML_COL_CLASS_ICON2 + 1
#define ML_COL_ICON4					ML_COL_AGRASSIVE_ICON + 1
#define ML_COL_ICON5					ML_COL_ICON4 + 1
#define ML_COL_ICON6					ML_COL_ICON5 + 1

	#define ML_COL_NATURE_EXP_MIN			ML_COL_ICON6 + 1
	#define ML_COL_NATURE_EXP_MAX			ML_COL_NATURE_EXP_MIN + 1
	assert_cs( excel.GetColCount() >= ML_COL_ICON6 );

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsMonster::sINFO monsInfo;
	while( excel.MoveNext() == true )
	{
		// ID
		monsInfo.s_dwMonsterID = atoi( excel.GetData( ML_COL_MONSTER_ID, cBuffer ) );
		if( monsInfo.s_dwMonsterID == 0 )
			continue;

		assert_csm1( IsMonster( monsInfo.s_dwMonsterID ) == false, _T( "몬스터테이블 :  이미 존재하는 ID = %d" ), monsInfo.s_dwMonsterID );

		// digimonID
		monsInfo.s_dwModelID = atoi( excel.GetData( ML_COL_MODEL_ID, cBuffer ) );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			// name
			M2W( monsInfo.s_szName, excel.GetData( ML_COL_NAME, cBuffer ), MONSTER_NAME_LEN );
			// pen name
			M2W( monsInfo.s_szPenName, excel.GetData( ML_COL_PEN_NAME, cBuffer ), MONSTER_PEN_NAME_LEN );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			// name
			M2W( monsInfo.s_szName, excel.GetData( ML_COL_NAME_ENG, cBuffer ), MONSTER_NAME_LEN );
			// pen name
			M2W( monsInfo.s_szPenName, excel.GetData( ML_COL_PEN_NAME_ENG, cBuffer ), MONSTER_PEN_NAME_LEN );			
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			// name
			//M2W( monsInfo.s_szName, excel.GetData( ML_COL_NAME_ENG, cBuffer ), MONSTER_NAME_LEN );
			wcscpy_s(monsInfo.s_szName, MONSTER_NAME_LEN, excel.GetData( ML_COL_NAME_ENG, wBuffer ));
			// pen name
			//M2W( monsInfo.s_szPenName, excel.GetData( ML_COL_PEN_NAME_ENG, cBuffer ), MONSTER_PEN_NAME_LEN );			
			wcscpy_s(monsInfo.s_szPenName, MONSTER_PEN_NAME_LEN, excel.GetData( ML_COL_PEN_NAME_ENG, wBuffer ));
			break;
		default:
			assert_cs( false );
		}
		

		// level
		monsInfo.s_nLevel = atoi( excel.GetData( ML_COL_LEVEL, cBuffer ) );

		// EXP
		monsInfo.s_nEXP = atoi( excel.GetData( ML_COL_EXP, cBuffer ) );

		// Battle
		monsInfo.s_nBattle = atoi( excel.GetData( ML_COL_BATTLE, cBuffer ) );

		// HP
		monsInfo.s_nHP = atoi( excel.GetData( ML_COL_HP, cBuffer ) );

		// DS
		monsInfo.s_nDS = atoi( excel.GetData( ML_COL_DS, cBuffer ) );

		// DP
		monsInfo.s_nDefPower = atoi( excel.GetData( ML_COL_DP, cBuffer ) );

		// EV
		monsInfo.s_nEvasion = atoi( excel.GetData( ML_COL_EV, cBuffer ) );

		// MS
		monsInfo.s_nMoveSpeed = atoi( excel.GetData( ML_COL_MS, cBuffer ) );

		// CA
		monsInfo.s_nCritcalRate = atoi( excel.GetData( ML_COL_CA, cBuffer ) );

		// AP
		monsInfo.s_nAttPower = atoi( excel.GetData( ML_COL_AP, cBuffer ) );

		// AS
		monsInfo.s_nAttSpeed = atoi( excel.GetData( ML_COL_AS, cBuffer ) );

		// AR
		monsInfo.s_nAttRange = atoi( excel.GetData( ML_COL_AR, cBuffer ) );

		// HT
		monsInfo.s_nHitRate = atoi( excel.GetData( ML_COL_HT, cBuffer ) );

		// Sight
		monsInfo.s_nSight = atoi( excel.GetData( ML_COL_SIGHT, cBuffer ) );

		// Hunt Range
		monsInfo.s_nHuntRange = atoi( excel.GetData( ML_COL_HUNTRANGE, cBuffer ) );

		// Scale
		monsInfo.s_fScale = atoi( excel.GetData( ML_COL_SCALE, cBuffer ) )*0.01f;

		// Class
		monsInfo.s_nClass = atoi( excel.GetData( ML_COL_CLASS, cBuffer ) );
		// Icon	//여기서 테이블을 불러온다....아이콘 num
 		monsInfo.s_nIcon1 = atoi( excel.GetData( ML_COL_CLASS_ICON, cBuffer ) );
 		monsInfo.s_nIcon2 = atoi( excel.GetData( ML_COL_CLASS_ICON2, cBuffer ) );
 		monsInfo.s_nIcon3 = atoi( excel.GetData( ML_COL_AGRASSIVE_ICON, cBuffer ) );
 		monsInfo.s_nIcon4 = atoi( excel.GetData( ML_COL_ICON4, cBuffer ) );
		monsInfo.s_nIcon5 = atoi( excel.GetData( ML_COL_ICON5, cBuffer ) );
		monsInfo.s_nIcon6 = atoi( excel.GetData( ML_COL_ICON6, cBuffer ) );

		//속성 Exp
		monsInfo.s_Exp_Min = atoi( excel.GetData( ML_COL_NATURE_EXP_MIN, cBuffer ) );
		monsInfo.s_Exp_Max = atoi( excel.GetData( ML_COL_NATURE_EXP_MAX, cBuffer ) );

		CsMonster* pMonster = csnew CsMonster;
		pMonster->Init( &monsInfo );

		m_mapMonster[ monsInfo.s_dwMonsterID ] = pMonster;
	}

	excel.Close();
}

void CsMonsterMng::_LoadExcelSkill()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Moster_Skill(), "Monster_Skill" );
	assert_cs( bSuccess == true );

	#define MS_COL_IDX							0
	#define MS_COL_MONSTER_ID					1
	
	//#define MS_COL_GLOBAL_COOLTIME_TYPE		MS_COL_MONSTER_ID + 1			// 2
	//#define MS_COL_GLOBAL_COOLTIME			MS_COL_ + 1						// 3
	#define MS_COL_COOLTIME						MS_COL_MONSTER_ID + 3			// 4
	#define MS_COL_CAST_TIME					MS_COL_COOLTIME + 1				// 5
	#define MS_COL_MOVE_CAST_CHECK				MS_COL_CAST_TIME + 1			// 6
	//#define MS_COL_DISTANCE					MS_COL_ + 1						// 7
	//#define MS_COL_MAXIMUM_USE_COUNT			MS_COL_MOVE_CAST_CHECK + 1		// 8
	//#define MS_COL_CONSUMED_DS				MS_COL_ + 1						// 9
	//#define MS_COL_TARGET_TYPE				MS_COL_ + 1						// 10
	#define MS_COL_TARGET_COUNT					MS_COL_MOVE_CAST_CHECK + 5		// 11
	#define MS_COL_TARGET_MIN_COUNT				MS_COL_TARGET_COUNT + 1			// 12
	#define MS_COL_TARGET_MAX_COUNT				MS_COL_TARGET_MIN_COUNT + 1		// 13
	#define MS_COL_USE_TERMS					MS_COL_TARGET_MAX_COUNT + 1		// 14
	//#define MS_COL_START_TERMS				MS_COL_USE_TERMS + 1			// 15
	//#define MS_COL_END_TERMS					MS_COL_START_TERMS + 1			// 16
	#define MS_COL_SKILL_TYPE					MS_COL_USE_TERMS + 3			// 17
	#define MS_COL_EFFECT_VALUE_MIN				MS_COL_SKILL_TYPE + 1			// 18
	#define MS_COL_EFFECT_VALUE_MAX				MS_COL_EFFECT_VALUE_MIN + 1		// 19
	#define MS_COL_SKILL_RANGE_IDX				MS_COL_EFFECT_VALUE_MAX + 1		// 20
	#define MS_COL_SEQUENCE_ID					MS_COL_SKILL_RANGE_IDX + 1		// 21
	#define MS_COL_ANI_DELAY					MS_COL_SEQUENCE_ID + 1			// 22
	#define MS_COL_VELOCITY						MS_COL_ANI_DELAY + 1			// 23
	#define MS_COL_ACCEL						MS_COL_VELOCITY + 1				// 24
	#define MS_COL_EFFECT_FACTOR				MS_COL_ACCEL + 1				// 25, 27, 29
	#define MS_COL_EFFECT_FACTOR_VALUE			MS_COL_EFFECT_FACTOR + 1		// 26, 28, 30
	#define MS_COL_TALK_ID						MS_COL_EFFECT_FACTOR + 6		// 31

	#define MS_COL_ACTIVE_TYPE						MS_COL_TALK_ID + 1				// 32 Talk_Code 뒤에 붙는다는 가정
	#define MS_COL_NOTICE_TIME						MS_COL_ACTIVE_TYPE + 1				// 32 Talk_Code 뒤에 붙는다는 가정
	#define MS_COL_NOTICE_EFF_FILENAME				MS_COL_NOTICE_TIME + 1				// 32 Talk_Code 뒤에 붙는다는 가정

	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsMonsterSkill::sINFO pInfo;
	while( excel.MoveNext() == true )
	{
		// 인덱스
		pInfo.s_nSkill_IDX = atoi( excel.GetData( MS_COL_IDX, cBuffer ) );

		CsMonsterSkill::MAP::const_iterator it = m_mapMonsterSkill.find( pInfo.s_nSkill_IDX );
		if( it != m_mapMonsterSkill.end() )
		{
			CsMessageBox( MB_OK, L"몬스터 스킬 인텍스 번호 중복 \n\nSkillIdx : %d", pInfo.s_nSkill_IDX );
			continue;
		}
		// 몬스터 ID
		pInfo.s_dwMonsterID = atoi( excel.GetData( MS_COL_MONSTER_ID, cBuffer ) );
		// 쿨타임
		pInfo.s_dwCoolTime = atoi( excel.GetData( MS_COL_COOLTIME, cBuffer ) );
		// 캐스팅타임
		pInfo.s_nCastTime = atoi( excel.GetData( MS_COL_CAST_TIME, cBuffer ) );

		// 캐스팅 체크( 0 - 사용안함, 1 - 스킬 시전 중 이동 X ) 
		pInfo.s_nCastCheck = atoi( excel.GetData( MS_COL_MOVE_CAST_CHECK, cBuffer ) );

		// 스킬 대상 수
		pInfo.s_nTarget_Cnt = atoi( excel.GetData( MS_COL_TARGET_COUNT, cBuffer ) );
		// 스킬 대상 수 (최소)
		pInfo.s_nTarget_MinCnt = atoi( excel.GetData( MS_COL_TARGET_MIN_COUNT, cBuffer ) );
		// 스킬 대상 수 (최대)
		pInfo.s_nTarget_MaxCnt = atoi( excel.GetData( MS_COL_TARGET_MAX_COUNT, cBuffer ) );
	
		// 스킬 사용 조건
		pInfo.s_nUse_Terms = atoi( excel.GetData( MS_COL_USE_TERMS, cBuffer ) );

		// 스킬 타입
		pInfo.s_nSkillType = atoi( excel.GetData( MS_COL_SKILL_TYPE, cBuffer ) );

		// 효과 최소 수치
		pInfo.s_dwEff_Val_Min = atoi( excel.GetData( MS_COL_EFFECT_VALUE_MIN, cBuffer ) );
		// 효과 최대 수치
		pInfo.s_dwEff_Val_Max = atoi( excel.GetData( MS_COL_EFFECT_VALUE_MAX, cBuffer ) );

		// 스킬범위 참조 인덱스( SkillTerms 테이블 인덱스 )
		pInfo.s_nRangeIdx = atoi( excel.GetData( MS_COL_SKILL_RANGE_IDX, cBuffer ) );

		// 스킬 애니 시퀀스 ID
		pInfo.s_dwSequenceID = atoi( excel.GetData( MS_COL_SEQUENCE_ID, cBuffer ) );
		// 애니 딜레이
		pInfo.s_nAni_Delay = atoi( excel.GetData( MS_COL_ANI_DELAY, cBuffer ) );
		// 스킬 속도
		pInfo.s_nValocity = atoi( excel.GetData( MS_COL_VELOCITY, cBuffer ) );
		// 스킬 가속도
		pInfo.s_nAccel = atoi( excel.GetData( MS_COL_ACCEL, cBuffer ) );

		for( int i = 0 ; i < MAX_FACTOR_COUNT ; i++ )
		{
			// 효과 팩터
			int nCol = MS_COL_EFFECT_FACTOR + i*2;
			pInfo.s_nEff_Factor[ i ] = atoi( excel.GetData( nCol , cBuffer ) );
			// 효과 팩터값
			nCol = MS_COL_EFFECT_FACTOR_VALUE + i*2;
			pInfo.s_dwEff_Fact_Val[ i ] = atoi( excel.GetData( nCol , cBuffer ) );
		}
		// 토크 ID
		pInfo.s_nTalkId = atoi( excel.GetData( MS_COL_TALK_ID, cBuffer ) );

		//해당 스킬(이펙트) 위치 타입결정 0=자신, 1=타켓, 2=좌표
		pInfo.s_nActiveType = atoi( excel.GetData( MS_COL_ACTIVE_TYPE, cBuffer ) );
		//예고 시간을 받는 변수
		pInfo.s_fNoticeTime	= (float)atoi( excel.GetData( MS_COL_NOTICE_TIME, cBuffer ) );
		//예고 이팩트 파일명 (폴더명 포함)
		memset( pInfo.s_szNoticeEff_Name, 0, MAX_FILENAME );
		excel.GetData( MS_COL_NOTICE_EFF_FILENAME, cBuffer );
		if( strlen( cBuffer ) != 0 && _stricmp( cBuffer, "none" ) != 0 )
			memcpy( pInfo.s_szNoticeEff_Name, cBuffer, MAX_FILENAME );

		CsMonsterSkill* pMonSkill = csnew CsMonsterSkill;
		pMonSkill->Init( &pInfo );

		m_mapMonsterSkill[ pInfo.s_nSkill_IDX ] = pMonSkill;
	}

	excel.Close();
}

void CsMonsterMng::_LoadExcelSkillTerms()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Monster_Skill_terms(), "Monster_Skill_terms" );
	assert_cs( bSuccess == true );

	#define MST_COL_IDX							0
	#define MST_COL_TARGETING_TYPE				MST_COL_IDX + 1			
	#define MST_COL_DIRECTION					MST_COL_TARGETING_TYPE + 1
	#define MST_COL_REFERENCE_CODE				MST_COL_DIRECTION + 1	
	#define MST_COL_RANGE						MST_COL_REFERENCE_CODE + 1		


	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsMonsterSkillTerms::sINFO pInfo;
	while( excel.MoveNext() == true )
	{
		// 인덱스
		pInfo.s_nIDX = atoi( excel.GetData( MST_COL_IDX, cBuffer ) );
		// 타겟팅 타입
		pInfo.s_nTargetingType = atoi( excel.GetData( MST_COL_TARGETING_TYPE, cBuffer ) );
		// 방향성
		pInfo.s_nDirection = atoi( excel.GetData( MST_COL_DIRECTION, cBuffer ) );
		// 참조 코드
		pInfo.s_nRefCode = atoi( excel.GetData( MST_COL_REFERENCE_CODE, cBuffer ) );
		// 범위, 거리
		pInfo.s_nRange = atoi( excel.GetData( MST_COL_RANGE, cBuffer ) );

		CsMonsterSkillTerms* pMonSkillTerms = csnew CsMonsterSkillTerms;
		pMonSkillTerms->Init( &pInfo );

		m_mapMonsterSkillTerms[ pInfo.s_nIDX ] = pMonSkillTerms;
	}

	excel.Close();
}

void CsMonsterMng::_LoadExcelDrop()
{
//#define MONSTER_TABLE_EXCEL_PATH		"GameData\\DM_Monster_List.xls"
//#define MONSTER_SHEET_NAME				"Monster"
//	if( _access_s( MONSTER_TABLE_EXCEL_PATH, 0 ) != 0 )
//	{
//		MessageBox( NULL, _T( "GameData\\DM_Monster_List.xls 이 존재 하지 않습니다." ), NULL, MB_OK );
//		return;
//	}
//
//	cExcelReader excel;
//	bool bSuccess = excel.Open( MONSTER_TABLE_EXCEL_PATH, MONSTER_SHEET_NAME );
//	assert_cs( bSuccess == true );
//
//#define ML_COL_NORMAL_DROPSTART		45
//#define ML_COL_EXTRA_DROPSTART		93
//
//	assert_cs( excel.GetColCount() >= ML_COL_SCALE );
//
//	char cBuffer[ CEXCLE_CELL_LEN ];
//
//	// 3줄은 버린다.
//	bSuccess = excel.MoveFirst();
//	assert_cs( bSuccess == true );	
//	bSuccess = excel.MoveNext();
//	assert_cs( bSuccess == true );
//	bSuccess = excel.MoveNext();
//	assert_cs( bSuccess == true );
//
//	CsMonsterDrop::sINFO monsInfo;
//	while( excel.MoveNext() == true )
//	{
//		// ID
//		monsInfo.s_dwMonsterID = atoi( excel.GetData( 1, cBuffer ) );
//		if( monsInfo.s_dwMonsterID == 0 )
//			continue;
//
//		for( int i=0; i<DROP_KIND; ++i )
//		{
//			monsInfo.s_nNormalDrop[ i ] = atoi( excel.GetData( ML_COL_NORMAL_DROPSTART+i*4, cBuffer ) );
//			monsInfo.s_nExtraDrop[ i ] = atoi( excel.GetData( ML_COL_EXTRA_DROPSTART+i*4, cBuffer ) );
//		}
//		CsMonsterDrop* pMonster = csnew CsMonsterDrop;
//		pMonster->Init( &monsInfo );
//
//		m_mapMonsterDrop[ monsInfo.s_dwMonsterID ] = pMonster;
//	}
//
//	excel.Close();
}

CsMonster* CsMonsterMng::GetMonster( DWORD dwMonsterTableID )
{
	if( m_mapMonster.find( dwMonsterTableID ) != m_mapMonster.end() )
		return m_mapMonster[ dwMonsterTableID ];
	return NULL;
}

CsMonsterSkill::sINFO* CsMonsterMng::GetMonsterSkill( USHORT nSkill_IDX )
{
	int nMonSkill_Size = (int)m_mapMonsterSkill.size();

	if( nSkill_IDX == 0 )
		return NULL;

 	if( m_mapMonsterSkill.find( nSkill_IDX ) == m_mapMonsterSkill.end() )
	{
#ifndef _GIVE
		CsMessageBox( MB_OK, L"몬스터 스킬 아님\n\nSkillIdx : %d", nSkill_IDX );
#endif
		return NULL;
	}

	return m_mapMonsterSkill[ nSkill_IDX ]->GetInfo();
}

float CsMonsterMng::GetEffectVal_Scale( int nIdx )
{
	//크기 키우기
	float fScale = 0.0f;
	for( int i = 0 ; i < MAX_FACTOR_COUNT ; i++ )
	{
		if( 	m_mapMonsterSkill[ nIdx ]->GetInfo()->s_nEff_Factor[ i ] == CsMonsterSkill::SCALE_INCREASE )	// 효과 팩터 중 스케일 증가값 확인
		{
			DWORD dwVal = m_mapMonsterSkill[ nIdx ]->GetInfo()->s_dwEff_Fact_Val[ i ];
			fScale = (float)dwVal * 0.01f;
			break;
		}
	}

	return fScale;
}

int CsMonsterMng::GetBuffStack( int nIdx, int nCurVal )
{
	// 무조건 효과 팩터값 1을 가져와
	DWORD dwVal = m_mapMonsterSkill[ nIdx ]->GetInfo()->s_dwEff_Fact_Val[ 0 ];

	// 현재 적용치 / 1스택당 적용치
	int nStack = nCurVal / dwVal;
	
	return nStack;
}

CsMonsterSkillTerms::sINFO* CsMonsterMng::GetMonsterSkillTerms( USHORT nIDX )
{
	int nSize = (int)m_mapMonsterSkillTerms.size();

	if( nIDX == 0 )
		return NULL;

	if( m_mapMonsterSkillTerms.find( nIDX ) == m_mapMonsterSkillTerms.end() )
		return NULL;

	return m_mapMonsterSkillTerms[ nIDX ]->GetInfo();
}

#define MONSTER_BIN		"Monster.bin"

void CsMonsterMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MONSTER_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapMonster.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsMonster::MAP_IT it = m_mapMonster.begin();
	CsMonster::MAP_IT itEnd = m_mapMonster.end();
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second->GetInfo(), sizeof( CsMonster::sINFO ), 1, fp );
	}

	nCount = (int)m_mapMonsterHit.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< int, int >::iterator it2 = m_mapMonsterHit.begin(); 
	std::map< int, int >::iterator it2End = m_mapMonsterHit.end();
	for( ; it2!=it2End; ++it2 )
	{		
		fwrite( &it2->first, sizeof( int ), 1, fp );
		fwrite( &it2->second, sizeof( int ), 1, fp );
	}

	nCount = (int)m_mapMonsterSkill.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsMonsterSkill::MAP_IT it3 = m_mapMonsterSkill.begin();
	CsMonsterSkill::MAP_IT itEnd3 = m_mapMonsterSkill.end();
	for( ; it3 != itEnd3 ; ++it3 )
	{
		fwrite( it3->second->GetInfo(), sizeof( CsMonsterSkill::sINFO ), 1, fp );
	}

	nCount = (int)m_mapMonsterSkillTerms.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsMonsterSkillTerms::MAP_IT it4 = m_mapMonsterSkillTerms.begin();
	CsMonsterSkillTerms::MAP_IT itEnd4 = m_mapMonsterSkillTerms.end();
	for( ; it4 != itEnd4 ; ++it4 )
	{
		fwrite( it4->second->GetInfo(), sizeof( CsMonsterSkillTerms::sINFO ), 1, fp );
	}
	fclose( fp );
}

bool CsMonsterMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MONSTER_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBox( MB_OK, _T( "%s 파일이 존재 하지 않습니다." ), cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsMonster::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &info, sizeof( CsMonster::sINFO ), 1, fp );

		CsMonster* pObject = csnew CsMonster;
		pObject->Init( &info );
		assert_cs( m_mapMonster.find( info.s_dwMonsterID ) == m_mapMonster.end() );
		m_mapMonster[ info.s_dwMonsterID ] = pObject;
	}
	
	fread( &nCount, sizeof( int ), 1, fp );
	int nLV;
	int nHT;
	for( int i=0; i<nCount; ++i )
	{
		fread( &nLV, sizeof( int ), 1, fp );
		fread( &nHT, sizeof( int ), 1, fp );
		assert_cs( m_mapMonsterHit.find( nLV ) == m_mapMonsterHit.end() );

		m_mapMonsterHit[ nLV ] = nHT;
	}

	fread( &nCount, sizeof( int ), 1, fp );
	CsMonsterSkill::sINFO info2;
	for( int i=0; i<nCount; ++i )
	{
		fread( &info2, sizeof( CsMonsterSkill::sINFO ), 1, fp );

		CsMonsterSkill* pObject = csnew CsMonsterSkill;
		pObject->Init( &info2 );
		assert_cs( m_mapMonsterSkill.find( info2.s_nSkill_IDX ) == m_mapMonsterSkill.end() );
		m_mapMonsterSkill[ info2.s_nSkill_IDX ] = pObject;
	}

	fread( &nCount, sizeof( int ), 1, fp );
	CsMonsterSkillTerms::sINFO info3;
	for( int i=0; i<nCount; ++i )
	{
		fread( &info3, sizeof( CsMonsterSkillTerms::sINFO ), 1, fp );

		CsMonsterSkillTerms* pObject = csnew CsMonsterSkillTerms;
		pObject->Init( &info3 );
		assert_cs( m_mapMonsterSkillTerms.find( info3.s_nIDX ) == m_mapMonsterSkillTerms.end() );
		m_mapMonsterSkillTerms[ info3.s_nIDX ] = pObject;
	}
	fclose( fp );

	return true;
}

void CsMonsterMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MONSTER_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	CsMonster::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info, sizeof( CsMonster::sINFO ) );
		CsMonster* pObject = csnew CsMonster;
		pObject->Init( &info );
		assert_cs( m_mapMonster.find( info.s_dwMonsterID ) == m_mapMonster.end() );
		m_mapMonster[ info.s_dwMonsterID ] = pObject;
	}

	_read( nHandle, &nCount, sizeof( int ) );

	int nLV;
	int nHT;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &nLV, sizeof( int ) );
		_read( nHandle, &nHT, sizeof( int ) );
		assert_cs( m_mapMonsterHit.find( nLV ) == m_mapMonsterHit.end() );

		m_mapMonsterHit[ nLV ] = nHT;
	}
	_read( nHandle, &nCount, sizeof( int ) );

	CsMonsterSkill::sINFO info2;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info2, sizeof( CsMonsterSkill::sINFO ) );
		CsMonsterSkill* pObject = csnew CsMonsterSkill;
		pObject->Init( &info2 );
		assert_cs( m_mapMonsterSkill.find( info2.s_nSkill_IDX ) == m_mapMonsterSkill.end() );
		m_mapMonsterSkill[ info2.s_nSkill_IDX ] = pObject;
	}

	_read( nHandle, &nCount, sizeof( int ) );

	CsMonsterSkillTerms::sINFO info3;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info3, sizeof( CsMonsterSkillTerms::sINFO ) );
		CsMonsterSkillTerms* pObject = csnew CsMonsterSkillTerms;
		pObject->Init( &info3 );
		assert_cs( m_mapMonsterSkillTerms.find( info3.s_nIDX ) == m_mapMonsterSkillTerms.end() );
		m_mapMonsterSkillTerms[ info3.s_nIDX ] = pObject;
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}
