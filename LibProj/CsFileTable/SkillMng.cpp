
#include "stdafx.h"
#include "SkillMng.h"


void CsSkillMng::Delete()
{
	CsSkill::MAP_IT it = m_mapSkill.begin();
	CsSkill::MAP_IT itEnd = m_mapSkill.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapSkill.clear();
}

bool CsSkillMng::Init( char* cPath )
{
	char cNewPath[ MAX_PATH ];
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
		strcpy_s( cNewPath, MAX_PATH, "Data\\Bin\\Korea_Org" );
		break;
	default:
		strcpy_s( cNewPath, MAX_PATH, cPath );
	}

	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();
		break;
	case nsCsFileTable::FT_BIN:
		if( _LoadBin( cNewPath ) == false )
			return false;
		break;
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack( cNewPath );
		break;
	default:
		assert_cs( false );
	}

	return true;
}

bool CsSkillMng::IsSkill( DWORD dwID )
{ 
	CsSkill::MAP::const_iterator it = m_mapSkill.find( dwID );
	if( it == m_mapSkill.end() )
		return false;
	return true;
}

CsSkill* CsSkillMng::GetSkill( DWORD dwID )
{
	//assert_csm1( IsSkill( dwID ) == true, _T( "스킬 코드 %d는 존재하지 않습니다." ),dwID );

	CsSkill::MAP::iterator it = m_mapSkill.find( dwID );
	if( it == m_mapSkill.end() )
	{
		//assert_csm1( false, _T( "스킬 코드 %d는 존재하지 않습니다." ),dwID );
		return NULL;
	}

	return it->second;
}

CsTamerSkill* CsSkillMng::GetTamer_Skill( int nIndex, int dwSkillCode )
{
	if( -1 == nIndex && -1 == dwSkillCode )
		return NULL;

	CsTamerSkill::TSKILL_MAP_IT it = m_mapTamerSkill.begin();
	for( ; it != m_mapTamerSkill.end(); ++it)
	{
		SAFE_POINTER_CON( it->second );
		CsTamerSkill::sTSkill_INFO* pInfo = it->second->Get_TSkill_Info();
		SAFE_POINTER_CON( pInfo );
		if( nIndex != -1 )
		{
			if( pInfo->s_nIndex == nIndex )
				return it->second;
		}
		else
		{
			if( pInfo->s_dwSkillCode == dwSkillCode )
				return it->second;
		}		
	}
// 	int nCount = (int)m_mapTamerSkill.size();
// 	for( int i = 1 ; i <nCount+1 ; ++i )
// 	{
// 		if( nIndex != -1 )
// 		{
// 			if( nIndex == m_mapTamerSkill[i]->Get_TSkill_Info()->s_nIndex )
// 				return m_mapTamerSkill[i];
// 		}
// 		else if( dwSkillCode != -1 )
// 		{
// 			if( dwSkillCode == m_mapTamerSkill[i]->Get_TSkill_Info()->s_dwSkillCode )
// 				return m_mapTamerSkill[i];
// 		}		
// 	}

	assert_csm1( false, _T( " 스킬 인덱스 %d 존재하지 않습니다." ), nIndex );
	assert_csm1( false, _T( " 스킬 코드 %d 존재하지 않습니다." ), dwSkillCode );
	return NULL;
}

bool CsSkillMng::IsTamer_Skill( DWORD dwSkillCode )
{
	CsTamerSkill::TSKILL_MAP_IT it = m_mapTamerSkill.begin();
	for( ; it != m_mapTamerSkill.end(); ++it)
	{
		SAFE_POINTER_CON( it->second );
		CsTamerSkill::sTSkill_INFO* pInfo = it->second->Get_TSkill_Info();
		SAFE_POINTER_CON( pInfo );
		if( pInfo->s_dwSkillCode == dwSkillCode )
			return true;
	}
// 	int nCount = (int)m_mapTamerSkill.size();
// 	for( int i=1; i<nCount+1; ++i )
// 	{
// 		if( dwSkillCode == m_mapTamerSkill[i]->Get_TSkill_Info()->s_dwSkillCode )
// 			return true;
// 	}

	return false;
}

CsAreaCheck* CsSkillMng::GetArea_Check( DWORD dwIndex )
{
	CsAreaCheck::AREACHECK_MAP_IT it = m_mapAreaCheck.find( dwIndex );
	if( it == m_mapAreaCheck.end() )
	{
		assert_csm1( false, _T( "스킬 코드 %d는 존재하지 않습니다." ), dwIndex );
		return NULL;
	}

	return it->second;

// 	assert_csm1( IsArea_Check( dwIndex ) == true, _T( "스킬 코드 %d는 존재하지 않습니다." ), dwIndex );
// 	return m_mapAreaCheck[ dwIndex ];
}

bool CsSkillMng::FindArea_Check( DWORD dwIndex, USHORT nMapID )
{
	CsAreaCheck::AREACHECK_MAP_IT it = m_mapAreaCheck.find( dwIndex );
	if( it == m_mapAreaCheck.end() )
		return false;

	SAFE_POINTER_RETVAL( it->second, false );

	CsAreaCheck::sArea_Check* pAreaInfo = it->second->Get_Area_Info();
	SAFE_POINTER_RETVAL( pAreaInfo, false );

	for(int i=0; i<30; ++i)
	{
		if( pAreaInfo->s_nArea[i] == nMapID )
			return true;
	}
	
	return false;
}

void CsSkillMng::_LoadExcel()
{
	_LoadExcel( nsCsFileTable::GetPath_DM_Tamer(), "Skill_Code", CsSkill::ST_TAMER );
	_LoadExcel( nsCsFileTable::GetPath_DM_Digimon(), "Skill_Code", CsSkill::ST_DIGIMON );
	_LoadExcel( nsCsFileTable::GetPath_DM_Item(), "Skill_Code", CsSkill::ST_ITEM );
	_LoadExcel( nsCsFileTable::GetPath_Tamer_Skill(), "Tamer_Skill" );
	_LoadAreaCheck( nsCsFileTable::GetPath_Tamer_Skill(), "Area_Check" );
}

void CsSkillMng::_LoadExcel( char* cExcel, char* cSheet, CsSkill::eSKILL_TYPE st )
{
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cExcel );
		return;
	}

	#define IL_COL_ID					1
	#define IL_COL_NAME_KOR				IL_COL_ID + 2
	#define IL_COL_NAME_ENG				IL_COL_NAME_KOR + 1
	#define IL_COL_COMMENT				IL_COL_NAME_ENG + 1
	#define IL_COL_APPLY_START			IL_COL_COMMENT+1
	#define IL_COL_LEVELUP_POINT		IL_COL_APPLY_START+(8*SKILL_APPLY_MAX_COUNT)
	#define IL_COL_MAX_LEVEL			IL_COL_LEVELUP_POINT+1
	#define IL_COL_APPLY_INCRESE_B		IL_COL_MAX_LEVEL+1	
	#define IL_COL_ATTRIBUTE			IL_COL_APPLY_INCRESE_B+3
	#define IL_COL_NATURE				IL_COL_ATTRIBUTE+2
	#define IL_COL_FAMILY				IL_COL_NATURE+2
	#define IL_COL_USEHP				IL_COL_FAMILY+2
	#define IL_COL_USEDS				IL_COL_USEHP+1
	#define IL_COL_ICON					IL_COL_USEDS+1
	#define IL_COL_TARGET				IL_COL_ICON+1
	#define IL_COL_ATT_TYPE				IL_COL_TARGET+2
	#define IL_COL_ATT_RANGE			IL_COL_ATT_TYPE+2
	#define IL_COL_ATT_RANGE_MINDMG		IL_COL_ATT_RANGE+1
	#define IL_COL_ATT_RANGE_NORDMG		IL_COL_ATT_RANGE_MINDMG+1
	#define IL_COL_ATT_RANGE_MAXDMG		IL_COL_ATT_RANGE_NORDMG+1
	#define IL_COL_ATT_SPHERE			IL_COL_ATT_RANGE_MAXDMG+1
	#define IL_COL_TIME_CASTING			IL_COL_ATT_SPHERE+1
	#define IL_COL_TIME_DAMAGE_S		IL_COL_TIME_CASTING+1
	#define IL_COL_TIME_DAMAGE_M		IL_COL_TIME_DAMAGE_S+1
	#define IL_COL_TIME_DAMAGE_DIST_S	IL_COL_TIME_DAMAGE_M+1
	#define IL_COL_TIME_COOLDOWN_S		IL_COL_TIME_DAMAGE_DIST_S+1
	#define IL_COL_TIME_COOLDOWN_M		IL_COL_TIME_COOLDOWN_S+1
	#define IL_COL_SKILL_VELOCITY		IL_COL_TIME_COOLDOWN_M+6
	#define IL_COL_SKILL_ACCEL			IL_COL_SKILL_VELOCITY+1	
	#define IL_COL_SKILL_TYPE			IL_COL_SKILL_ACCEL+2
	#define IL_COL_SKILL_LIMIT_LEVEL	IL_COL_SKILL_TYPE+3
	#define IL_COL_SKILL_LEARN_MONEY	IL_COL_SKILL_LIMIT_LEVEL+1
	#define IL_COL_BUFF_CODE			IL_COL_SKILL_LEARN_MONEY + 1	

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( cExcel, cSheet );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsSkill::sINFO info;
	DWORD dwTime;
	while( excel.MoveNext() == true )
	{
		// ID
		info.s_dwID = atoi( excel.GetData( IL_COL_ID, cBuffer ) );
		if( info.s_dwID == 0 )
			continue;

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			// name
			M2W( info.s_szName, excel.GetData( IL_COL_NAME_KOR, cBuffer ), SKILL_NAME_LEN );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			// name
			M2W( info.s_szName, excel.GetData( IL_COL_NAME_ENG, cBuffer ), SKILL_NAME_LEN );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			// name
			//M2W( info.s_szName, excel.GetData( IL_COL_NAME_ENG, cBuffer ), SKILL_NAME_LEN );
			wcscpy_s(info.s_szName, SKILL_NAME_LEN, excel.GetData( IL_COL_NAME_ENG, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			// comment
			M2W( info.s_szComment, excel.GetData( IL_COL_COMMENT, cBuffer ), ITEM_COMMENT_LEN );
			__CsStringChange::NewLineCheck( info.s_szComment );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			// comment
			//M2W( info.s_szComment, excel.GetData( IL_COL_COMMENT, cBuffer ), ITEM_COMMENT_LEN );
			wcscpy_s(info.s_szComment, ITEM_COMMENT_LEN, excel.GetData( IL_COL_COMMENT, wBuffer ));
			__CsStringChange::NewLineCheck( info.s_szComment );
			break;
		default:
			assert_cs( false );
		}

		// Apply
		long applyIndex = IL_COL_APPLY_START;
		long applyB_IncreaseIndex = IL_COL_APPLY_INCRESE_B;
		for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
		{
			info.s_Apply[ i ].s_nID = atoi( excel.GetData( applyIndex, cBuffer ) );
			applyIndex += 2;
			info.s_Apply[ i ].s_nA = atoi( excel.GetData( applyIndex, cBuffer ) );

			if( st == CsSkill::ST_DIGIMON )
			{
				applyIndex += 1;
				info.s_Apply[ i ].s_nInvoke_Rate = atoi( excel.GetData( applyIndex, cBuffer));	// 스킬 발동 확률
				applyIndex += 1;
			}
			else
			{
				applyIndex += 2;
			}

			info.s_Apply[ i ].s_nB = atoi( excel.GetData( applyIndex, cBuffer ) );

			++applyIndex;
			info.s_Apply[ i ].s_nC = atoi( excel.GetData( applyIndex, cBuffer ) );
			++applyIndex;
			info.s_Apply[ i ].s_nBuffCode = atoi( excel.GetData( applyIndex, cBuffer ) );
			applyIndex += 2;

			info.s_Apply[ i ].s_nIncrease_B_Point = atoi( excel.GetData( applyB_IncreaseIndex, cBuffer ) );
			++applyB_IncreaseIndex;

			if( ( info.s_Apply[ i ].s_nA == 15 )&&( info.s_Apply[ i ].s_nID == 101 ) )
			{
				info.s_Apply[ i ].s_nB = CsFloat2Int( info.s_Apply[ i ].s_nB * 0.01 );
			}
		}

		// Levelup Point
		info.s_nLevelupPoint = atoi( excel.GetData( IL_COL_LEVELUP_POINT, cBuffer ) );

		// Max Level
		info.s_nMaxLevel = atoi( excel.GetData( IL_COL_MAX_LEVEL, cBuffer ) );

		// Attribute
		info.s_nAttributeType = atoi( excel.GetData( IL_COL_ATTRIBUTE, cBuffer ) );

		// Nature
		info.s_nNatureType = atoi( excel.GetData( IL_COL_NATURE, cBuffer ) );

		// Family
		info.s_nFamilyType = atoi( excel.GetData( IL_COL_FAMILY, cBuffer ) );

		// Use HP
		info.s_nUseHP = atoi( excel.GetData( IL_COL_USEHP, cBuffer ) );

		// Use DS
		info.s_nUseDS = atoi( excel.GetData( IL_COL_USEDS, cBuffer ) );

		// Icon
		info.s_nIcon = atoi( excel.GetData( IL_COL_ICON, cBuffer ) );

		// Target
		info.s_nTarget = atoi( excel.GetData( IL_COL_TARGET, cBuffer ) );

		// AttType
		info.s_nAttType = atoi( excel.GetData( IL_COL_ATT_TYPE, cBuffer ) );

		// AttRange
		info.s_fAttRange = (float)atoi( excel.GetData( IL_COL_ATT_RANGE, cBuffer ) );

		// AttRange_MinDmg
		info.s_fAttRange_MinDmg = info.s_fAttRange*atoi( excel.GetData( IL_COL_ATT_RANGE_MINDMG, cBuffer ) )*0.01f;

		// AttRange_NorDmg
		info.s_fAttRange_NorDmg = info.s_fAttRange*atoi( excel.GetData( IL_COL_ATT_RANGE_NORDMG, cBuffer ) )*0.01f;

		// AttRange_MaxDmg
		info.s_fAttRange_MaxDmg = info.s_fAttRange*atoi( excel.GetData( IL_COL_ATT_RANGE_MAXDMG, cBuffer ) )*0.01f;


		// AttSphere
		info.s_nAttSphere = atoi( excel.GetData( IL_COL_ATT_SPHERE, cBuffer ) );

		// Casting Time
		info.s_fCastingTime = atoi( excel.GetData( IL_COL_TIME_CASTING, cBuffer ) )*0.001f;

		// Damage Time
		if( ( dwTime = atoi( excel.GetData( IL_COL_TIME_DAMAGE_S, cBuffer ) ) ) == 0 )
		{
			dwTime = atoi( excel.GetData( IL_COL_TIME_DAMAGE_M, cBuffer ) )*60;
		}
		assert_csm1( ( atoi( excel.GetData( IL_COL_TIME_DAMAGE_S, cBuffer ) ) == 0 )||( atoi( excel.GetData( IL_COL_TIME_DAMAGE_M, cBuffer ) ) == 0 ),
					_T( "%d ID의 스킬의 데미지 타임이 2개다 존재 합니다." ), info.s_dwID );
#ifdef GUILD_RENEWAL
		// 적용시간이 1일을 넘어가더라도 모두 초로 저장
		info.s_fDamageTime = (float)(dwTime);;
#else
		// 하루는 86400 초
		info.s_fDamageTime = (float)(dwTime%86400);
		info.s_nDamageDay = (USHORT)( dwTime/86400 );
#endif
		info.s_nDistanceTime = (float)(atoi( excel.GetData( IL_COL_TIME_DAMAGE_DIST_S, cBuffer ) )%86400);

		// Cooldown Time
		if( ( dwTime = atoi( excel.GetData( IL_COL_TIME_COOLDOWN_S, cBuffer ) ) ) == 0 )
		{
			dwTime = atoi( excel.GetData( IL_COL_TIME_COOLDOWN_M, cBuffer ) )*60;
		}
		assert_csm1( ( atoi( excel.GetData( IL_COL_TIME_COOLDOWN_S, cBuffer ) ) == 0 )||( atoi( excel.GetData( IL_COL_TIME_COOLDOWN_M, cBuffer ) ) == 0 ),
			_T( "%d ID의 스킬의 쿨다운 타임이 2개다 존재 합니다." ), info.s_dwID );
#ifdef GUILD_RENEWAL
		// 적용시간이 1일을 넘어가더라도 모두 초로 저장
		info.s_fCooldownTime = (float)(dwTime);
#else
		// 하루는 86400 초
		info.s_fCooldownTime = (float)(dwTime%86400000);
		info.s_nCooldownDay = (USHORT)( dwTime/86400000 );
#endif

		info.s_nSkillGroup = atoi( excel.GetData( IL_COL_TIME_COOLDOWN_M + 1, cBuffer ) );
		info.s_nSkillRank = atoi( excel.GetData( IL_COL_TIME_COOLDOWN_M + 2, cBuffer ) );
		info.s_nMemorySkill = atoi( excel.GetData( IL_COL_TIME_COOLDOWN_M + 3, cBuffer ) );

		info.s_fSkill_Velocity = (float)atoi( excel.GetData( IL_COL_SKILL_VELOCITY, cBuffer ) );
		info.s_fSkill_Accel = (float)atoi( excel.GetData( IL_COL_SKILL_ACCEL, cBuffer ) );

		info.s_nSkillType = atoi( excel.GetData( IL_COL_SKILL_TYPE, cBuffer ) );
		info.s_nLimitLevel = atoi( excel.GetData( IL_COL_SKILL_LIMIT_LEVEL, cBuffer ) );

		// 디지몬 스킬 사용 시 필요 메모리칩 개수
		info.s_nReq_Item = atoi( excel.GetData( IL_COL_SKILL_LEARN_MONEY, cBuffer ) );

		CsSkill* pSkill = csnew CsSkill;
		pSkill->Init( &info );

		assert_csm1( m_mapSkill.find( info.s_dwID ) == m_mapSkill.end(), _T( "%d ID의 스킬은 이미 존재 합니다." ), info.s_dwID );
		m_mapSkill[ info.s_dwID ] = pSkill;
	}
	excel.Close();
}

void CsSkillMng::_LoadExcel( char* cExcel, char* cSheet )
{
	char cBuffer[ CEXCLE_CELL_LEN ] = {0, };

	cExcelReader excel;
	bool bSuccess = excel.Open( cExcel, cSheet );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD col = 0;
	CsTamerSkill::sTSkill_INFO TamerInfo;
	while( excel.MoveNext() == true )
	{
		col = 0;
		// index
		TamerInfo.s_nIndex = atoi( excel.GetData( col, cBuffer ) );
		assert_cs( TamerInfo.s_nIndex != 0 );
		++col;

		// SkillCode
		TamerInfo.s_dwSkillCode = atoi( excel.GetData( col, cBuffer ) );
		//assert_cs( TamerInfo.s_dwSkillCode != 0 );
		col += 2;

		// skill type
		TamerInfo.s_nType = atoi( excel.GetData( col, cBuffer ) );
		++col;

		// Factor1, 2
		TamerInfo.s_dwFactor1 = atoi( excel.GetData( col, cBuffer ) );
		col += 2;
		TamerInfo.s_dwFactor2 = atoi( excel.GetData( col, cBuffer ) );
		col += 2;

		// Tamer Sequence ID
		TamerInfo.s_dwTamer_SeqID = atoi( excel.GetData( col, cBuffer ) );
		++col;

		// Digimon Sequence ID
		TamerInfo.s_dwDigimon_SeqID = atoi( excel.GetData( col, cBuffer ) );
		++col;

		// 사용 가능 상태 ( 0: 항상 사용, 1: 전투 중 사용 불가 )
		TamerInfo.s_nUseState = atoi( excel.GetData( col, cBuffer ) );
		++col;

		//  지역설정 사용여부 ( 1: 스킬을 사용할 수 있는 지역을 설정하겠다, 0: 아무곳에서나 사용가능 )
		TamerInfo.s_nUse_Are_Check = atoi( excel.GetData( col, cBuffer ) );
		++col;

		// 가능/불가능( 1: 해당 지역에서 사용 불가능, 2: 해당 지역에서 사용 가능)
		TamerInfo.s_nAvailable = atoi( excel.GetData( col, cBuffer ) );

		CsTamerSkill* pTamerSkill = new CsTamerSkill;
		pTamerSkill->TSkill_Init( &TamerInfo );

		assert_csm1( m_mapTamerSkill.find( TamerInfo.s_nIndex ) == m_mapTamerSkill.end(), _T( "%d ID의 스킬은 이미 존재 합니다." ), TamerInfo.s_nIndex );
		m_mapTamerSkill[ TamerInfo.s_nIndex ] = pTamerSkill;
	}
	excel.Close();
}

void CsSkillMng::_LoadAreaCheck( char* cExcel, char* cSheet )
{
	char cBuffer[ CEXCLE_CELL_LEN ] = {0, };

	cExcelReader excel;
	bool bSuccess = excel.Open( cExcel, cSheet );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD col = 0;
	CsAreaCheck::sArea_Check AreaInfo;
	while( excel.MoveNext() == true )
	{
		col = 0;

		// index
		AreaInfo.s_dwIndex = atoi( excel.GetData( col, cBuffer ) );
		++col;

		for( int i=0; i<30; ++i )
		{
			// 지역 맵번호
			AreaInfo.s_nArea[i] = atoi( excel.GetData( col, cBuffer ) );
			++col;
		}

		CsAreaCheck* pAreaCheck = new CsAreaCheck;
		pAreaCheck->AreaCheck_Init( &AreaInfo );

		assert_csm1( m_mapAreaCheck.find( AreaInfo.s_dwIndex ) == m_mapAreaCheck.end(), _T( "%d ID의 스킬은 이미 존재 합니다." ), AreaInfo.s_dwIndex );
		m_mapAreaCheck[ AreaInfo.s_dwIndex ] = pAreaCheck;
	}
	excel.Close();
}


#define SKILL_BIN		"Skill.bin"
void CsSkillMng::SaveBin( char* cPath )
{
	char cNewPath[ MAX_PATH ];
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
		strcpy_s( cNewPath, MAX_PATH, "Data\\Bin\\Korea_Org" );
		break;
	default:
		strcpy_s( cNewPath, MAX_PATH, cPath );
	}

	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cNewPath, SKILL_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapSkill.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsSkill::MAP_IT it = m_mapSkill.begin();
	CsSkill::MAP_IT itEnd = m_mapSkill.end();
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second->GetInfo(), sizeof( CsSkill::sINFO ), 1, fp );
	}

	nCount = (int)m_mapTamerSkill.size();
	fwrite( &nCount, sizeof(int), 1, fp );

	CsTamerSkill::TSKILL_MAP_IT Tamer_it = m_mapTamerSkill.begin();
	CsTamerSkill::TSKILL_MAP_IT Tamer_itEnd = m_mapTamerSkill.end();
	for( ; Tamer_it != Tamer_itEnd; ++Tamer_it )
	{
		fwrite( Tamer_it->second->Get_TSkill_Info(), sizeof( CsTamerSkill::sTSkill_INFO ), 1, fp );
	}

	/// 테이머 스킬 지역 체크
	nCount = (int)m_mapAreaCheck.size();
	fwrite( &nCount, sizeof(int), 1, fp );

	CsAreaCheck::AREACHECK_MAP_IT Area_it = m_mapAreaCheck.begin();
	CsAreaCheck::AREACHECK_MAP_IT Area_itEnd = m_mapAreaCheck.end();
	for( ; Area_it != Area_itEnd; ++Area_it )
	{
		fwrite( Area_it->second->Get_Area_Info(), sizeof( CsAreaCheck::sArea_Check ), 1, fp );
	}

	fclose( fp );
}

bool CsSkillMng::_LoadBin( char* cPath )
{
	char cNewPath[ MAX_PATH ];
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
		strcpy_s( cNewPath, MAX_PATH, "Data\\Bin\\Korea_Org" );
		break;
	default:
		strcpy_s( cNewPath, MAX_PATH, cPath );
	}
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cNewPath, SKILL_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsSkill::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &info, sizeof( CsSkill::sINFO ), 1, fp );

		CsSkill* pObject = csnew CsSkill;
		pObject->Init( &info );
		// Pack data sometimes contains duplicate skill rows; keep first, skip later dups.
		if( m_mapSkill.find( info.s_dwID ) != m_mapSkill.end() )
		{
			delete pObject;
			continue;
		}
		m_mapSkill[ info.s_dwID ] = pObject;
	}

	fread( &nCount, sizeof(int), 1, fp );

	CsTamerSkill::sTSkill_INFO Tamer_Info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &Tamer_Info, sizeof(CsTamerSkill::sTSkill_INFO), 1, fp );

		CsTamerSkill* pTamerObject = csnew CsTamerSkill;
		pTamerObject->TSkill_Init( &Tamer_Info );
		assert_cs( m_mapTamerSkill.find( Tamer_Info.s_nIndex ) == m_mapTamerSkill.end() );
		m_mapTamerSkill[ Tamer_Info.s_nIndex ] = pTamerObject;
	}

	/// 테이머 스킬 지역 체크
	fread( &nCount, sizeof(int), 1, fp );

	CsAreaCheck::sArea_Check Area_Info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &Area_Info, sizeof(CsAreaCheck::sArea_Check), 1, fp );

		CsAreaCheck* pAreaCheck = csnew CsAreaCheck;
		pAreaCheck->AreaCheck_Init( &Area_Info );
		// Some pack files contain duplicate Area_Check rows (production data quirk).
		// Keep the first mapping; free the duplicate; don't assert.
		if( m_mapAreaCheck.find( Area_Info.s_dwIndex ) != m_mapAreaCheck.end() )
		{
			delete pAreaCheck;
			continue;
		}
		m_mapAreaCheck[ Area_Info.s_dwIndex ] = pAreaCheck;
	}

	fclose( fp );

	return true;
}

void CsSkillMng::Reload( char* cPath )
{
	char cNewPath[ MAX_PATH ];
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
		strcpy_s( cNewPath, MAX_PATH, "Data\\Bin\\Korea_Org" );
		break;
	default:
		strcpy_s( cNewPath, MAX_PATH, cPath );
	}

	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cNewPath, SKILL_BIN );


	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:		
	case nsCsFileTable::FT_BIN:
		{
			FILE *fp = NULL;
			fopen_s( &fp, cName, "rb" );
			if( fp == NULL )
			{
				CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
				return;
			}

			int nCount;
			fread( &nCount, sizeof( int ), 1, fp );

			CsSkill::sINFO info;
			for( int i=0; i<nCount; ++i )
			{
				fread( &info, sizeof( CsSkill::sINFO ), 1, fp );
				m_mapSkill[ info.s_dwID ]->Init( &info );
			}

			fread( &nCount, sizeof(int), 1, fp );

			CsTamerSkill::sTSkill_INFO Tamer_Info;
			for( int i=0; i<nCount; ++i )
			{
				fread( &Tamer_Info, sizeof(CsTamerSkill::sTSkill_INFO), 1, fp );
				m_mapTamerSkill[ Tamer_Info.s_nIndex ]->TSkill_Init( &Tamer_Info );
			}

			// 스킬 지역 체크
			fread( &nCount, sizeof(int), 1, fp );

			CsAreaCheck::sArea_Check Area_Info;
			for( int i=0; i<nCount; ++i )
			{
				fread( &Area_Info, sizeof(CsAreaCheck::sArea_Check), 1, fp );
				m_mapAreaCheck[ Area_Info.s_dwIndex ]->AreaCheck_Init( &Area_Info );
			}

			fclose( fp );
		}
		break;
	case nsCsFileTable::FT_FILEPACK:
		{
			if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
				break;

			int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

			int nCount;
			_read( nHandle, &nCount, sizeof( int ) );

			CsSkill::sINFO info;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &info, sizeof( CsSkill::sINFO ) );
				m_mapSkill[ info.s_dwID ]->Init( &info );
			}

			_read( nHandle, &nCount, sizeof(int) );

			CsTamerSkill::sTSkill_INFO Tamer_Info;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &Tamer_Info, sizeof(CsTamerSkill::sTSkill_INFO) );
				m_mapTamerSkill[ Tamer_Info.s_nIndex ]->TSkill_Init( &Tamer_Info );
			}

			// 스킬 지역 체크
			_read( nHandle, &nCount, sizeof(int) );

			CsAreaCheck::sArea_Check Area_Info;
			for( int i=0; i<nCount; ++i )
			{
				_read( nHandle, &Area_Info, sizeof(CsAreaCheck::sArea_Check) );
				m_mapAreaCheck[ Area_Info.s_dwIndex ]->AreaCheck_Init( &Area_Info );
			}

			CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
		}
		break;
	default:
		assert_cs( false );
	}	
}

void CsSkillMng::_LoadFilePack( char* cPath )
{
	char cNewPath[ MAX_PATH ];
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
		strcpy_s( cNewPath, MAX_PATH, "Data\\Bin\\Korea_Org" );
		break;
	default:
		strcpy_s( cNewPath, MAX_PATH, cPath );
	}
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cNewPath, SKILL_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	CsSkill::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info, sizeof( CsSkill::sINFO ) );

		CsSkill* pObject = csnew CsSkill;
		pObject->Init( &info );
		// Pack data sometimes contains duplicate skill rows; keep first, skip later dups.
		if( m_mapSkill.find( info.s_dwID ) != m_mapSkill.end() )
		{
			delete pObject;
			continue;
		}
		m_mapSkill[ info.s_dwID ] = pObject;
	}

	_read( nHandle, &nCount, sizeof(int) );

	CsTamerSkill::sTSkill_INFO Tamer_Info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &Tamer_Info, sizeof(CsTamerSkill::sTSkill_INFO) );

		CsTamerSkill* pTamerObject = csnew CsTamerSkill;
		pTamerObject->TSkill_Init( &Tamer_Info );
		assert_cs( m_mapTamerSkill.find( Tamer_Info.s_nIndex ) == m_mapTamerSkill.end() );
		m_mapTamerSkill[ Tamer_Info.s_nIndex ] = pTamerObject;
	}

	/// 스킬 지역 체크
	_read( nHandle, &nCount, sizeof(int) );

	CsAreaCheck::sArea_Check Area_Info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &Area_Info, sizeof(CsAreaCheck::sArea_Check) );

		CsAreaCheck* pAreaCheck = csnew CsAreaCheck;
		pAreaCheck->AreaCheck_Init( &Area_Info );
		// Some pack files contain duplicate Area_Check rows (production data quirk).
		// Keep the first mapping; free the duplicate; don't assert.
		if( m_mapAreaCheck.find( Area_Info.s_dwIndex ) != m_mapAreaCheck.end() )
		{
			delete pAreaCheck;
			continue;
		}
		m_mapAreaCheck[ Area_Info.s_dwIndex ] = pAreaCheck;
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}


void CsSkillMng::StringChange( __CsStringChange* pSC )
{
	CsSkill::MAP_IT it = m_mapSkill.begin();
	CsSkill::MAP_IT itEnd = m_mapSkill.end();
	for( ; it!=itEnd; ++it )
	{
		pSC->Change( it->second->GetInfo()->s_szName, SKILL_NAME_LEN );
		pSC->Change( it->second->GetInfo()->s_szComment, SKILL_COMMENT_LEN );
	}
}

