#include "StdAfx.h"
#include "CDigimonEvolution.h"

#define LINE_LEN		2048


CDigimonEvolution::CDigimonEvolution(void)
{

}

void CDigimonEvolution::Delete()
{
	CDigimonEvolveInfo::MAP_IT it = m_Map.begin();
	CDigimonEvolveInfo::MAP_IT itEnd = m_Map.end();
	for ( ; it!=itEnd ; ++it )
	{
		it->second->Destroy();
		delete it->second;
	}
	m_Map.clear();
}

// =========================================================
// 디지몬 아이디로 진화 정보 가져오기
// =========================================================
CDigimonEvolveInfo*	CDigimonEvolution::GetEvolveInfo(DWORD nID)
{
	CDigimonEvolveInfo::MAP_IT it = m_Map.find( nID );
	if( it == m_Map.end() )
	{
		assert_csm1( m_Map.find( nID ) != m_Map.end(), _T( "nID = %d" ), nID );
		return NULL;
	}
	
	return it->second;	
}

CDigimonEvolveObj* CDigimonEvolution::GetEvolObj( DWORD nID )
{
	CDigimonEvolveInfo::MAP_IT it = m_Map.begin();
	CDigimonEvolveInfo::MAP_IT itEnd = m_Map.end();
	
	for ( ; it!=itEnd ; ++it )
	{		
		if( it->second->IsEvolveObjByID( nID ) == true )
			return it->second->GetEvolveObjByID( nID ); 
	}
	assert_csm( false, L"진화체 정보 없음" );
	return NULL;
}

// =========================================================
// 
// =========================================================
bool CDigimonEvolution::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();
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


// =========================================================
// 
// =========================================================
void CDigimonEvolution::_LoadExcel()
{
	char cBuffer[ CEXCLE_CELL_LEN ];
	char cSheet[ 32 ];
	bool bSuccess;
	cExcelReader Excel;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
	case nsCsFileTable::ENGLISH:
	case nsCsFileTable::ENGLISH_A:
		bSuccess = Excel.Open( nsCsFileTable::GetPath_DM_Evolution_01(), "진화트리정보" );
		assert_cs( bSuccess == true );
		break;
	case nsCsFileTable::THAILAND:
	case nsCsFileTable::TAIWAN:
	case nsCsFileTable::HONGKONG:
		bSuccess = Excel.Open( nsCsFileTable::GetPath_DM_Evolution_01(), "Evolution_Max_Setting"  /*"진화트리정보"*/ );
		assert_cs( bSuccess == true );
		break;
	default:
		assert_cs( false );
	}
	Excel.MoveFirst();
	Excel.MoveNext();
	Excel.MoveNext();
	int nMaxSlot = atoi( Excel.GetData( 1, cBuffer ) );

	for( int i=1; i<=nMaxSlot; ++i )
	{
		sprintf_s( cSheet, 32, "%02d", i );

		if( i<=50 )
			_LoadExcel( nsCsFileTable::GetPath_DM_Evolution_01(), cSheet );
		else
			_LoadExcel( nsCsFileTable::GetPath_DM_Evolution_02(), cSheet );
	}

	if( nsCsFileTable::g_bUseEvoExtra )
		_LoadExcel_Extra();
	if( nsCsFileTable::g_bUseEvoBattle )
		_LoadExcel_BattleEvo();
}

#define EVO_NEXT 3

void CDigimonEvolution::_LoadExcel( char* cPath, char* cSheet )
{
	cExcelReader Excel;
	bool bSuccess = Excel.Open( cPath, cSheet );
	if( !bSuccess )
	{
		CsMessageBoxA( MB_OK, "진화 테이블의 시트가 존재하지 않습니다. %s, %s", cPath, cSheet );
		return;
	}
	bSuccess = Excel.MoveFirst();

	CDigimonEvolveInfo * pEvolveInfo = csnew CDigimonEvolveInfo;

	// "start" 진화체 정보, "next" 진화정보트리
	char nType[CEXCLE_CELL_LEN];

	memset(nType,0,sizeof(nType));

	int nRow = (int)Excel.GetRowCount();
	bSuccess = Excel.MoveNext();
	bSuccess = Excel.MoveNext();

	char cBuffer[ CEXCLE_CELL_LEN ] = {0,};
	pEvolveInfo->GetInfo()->s_nDigimonID = atoi( Excel.GetData( 2, cBuffer ) );
	DWORD dwModelIDArray[ 30 ] = {0, };

	for(int line=2; line<nRow; line++) // 예) 아구몬 ~ 샤인 그레이몬까지
	{	
		if( atoi( Excel.GetData( 0, cBuffer ) ) <= 0)
			break;

		int Col=0;
		int nSlot = atoi( Excel.GetData( Col++, cBuffer ) );	// 진화 슬롯 Ev0_num
		Col++;
		DWORD dwID = atoi( Excel.GetData( Col++, cBuffer ) );	// 고유번호Di_No

		if( ( nSlot == 0 )||( dwID == 0 ) )
		{
			Excel.MoveNext();
			continue;
		}

		CDigimonEvolveObj *pEvolveObj = csnew CDigimonEvolveObj;
		pEvolveObj->m_nEvoSlot = nSlot; // 진화 슬롯 Ev0_num
		pEvolveObj->m_dwID = dwID; // 고유번호Di_No

		// 배열에 ModelID 담아 놓는다~ 나중에 연결
		dwModelIDArray[ pEvolveObj->m_nEvoSlot ] = pEvolveObj->m_dwID;

		for(int i=0; i<MAX_EVOLUTION; i++)	// 진화체 슬롯 1~6
		{
			pEvolveObj->m_nEvolutionList[i].nSlot = atoi( Excel.GetData( Col+(i*EVO_NEXT), cBuffer ) );	// 진화체 슬롯 n
			pEvolveObj->m_nEvolutionList[i].nType = atoi( Excel.GetData( Col+2+(i*EVO_NEXT), cBuffer ) );	// 변화
		}

		Col = 30;

		pEvolveObj->m_IconPos.x = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_IconPos.y = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nEnableSlot = atoi( Excel.GetData( Col++, cBuffer ) );			
		pEvolveObj->m_nOpenQualification = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nOpenLevel = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nOpenQuest = atoi( Excel.GetData( Col++, cBuffer ) );

		pEvolveObj->m_nOpenItemTypeS = atoi( Excel.GetData( Col++, cBuffer ) );			
		Col++;
		pEvolveObj->m_nOpenItemNum = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nUseItem = atoi( Excel.GetData( Col++, cBuffer ) );
		Col++;
		pEvolveObj->m_nUseItemNum = atoi( Excel.GetData( Col++, cBuffer ) );

		pEvolveObj->m_nIntimacy = atoi( Excel.GetData( Col++, cBuffer ) );

		pEvolveObj->m_nOpenCrest = atoi( Excel.GetData( Col++, cBuffer ) );
		Col++;
		pEvolveObj->m_EvoCard1 = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_EvoCard2 = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_EvoCard3 = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nEvoDigimental = atoi( Excel.GetData( Col++, cBuffer ) );
		Col++;
		pEvolveObj->m_nEvoTamerDS = atoi( Excel.GetData( Col++, cBuffer ) );

		Col += 4;		

		pEvolveObj->m_bRender = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_TargetStartPos.x = atoi( Excel.GetData( Col++, cBuffer ) );			
		pEvolveObj->m_TargetStartPos.y = atoi( Excel.GetData( Col++, cBuffer ) );			
		pEvolveObj->m_nStartHegiht = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nStartRot = atoi( Excel.GetData( Col++, cBuffer ) );

		pEvolveObj->m_TargetEndPos.x = atoi( Excel.GetData( Col++, cBuffer ) );			
		pEvolveObj->m_TargetEndPos.y = atoi( Excel.GetData( Col++, cBuffer ) );			
		pEvolveObj->m_nEndHegiht= atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nEndRot = atoi( Excel.GetData( Col++, cBuffer ) );

		pEvolveObj->m_nSpeed = atoi( Excel.GetData( Col++, cBuffer ) );				
		pEvolveObj->m_dwAni = atoi( Excel.GetData( Col++, cBuffer ) );			
		pEvolveObj->m_fStartTime = atof( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_fEndTime = atof( Excel.GetData( Col++, cBuffer ) );

		pEvolveObj->m_nR = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nG = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nB = atoi( Excel.GetData( Col++, cBuffer ) );

		strcpy_s( pEvolveObj->m_szLevel, MAX_EVO_EFFECT_LEN, Excel.GetData( Col++, cBuffer ) );
		strcpy_s( pEvolveObj->m_szEnchant, MAX_EVO_EFFECT_LEN, Excel.GetData( Col++, cBuffer ) );
		strcpy_s( pEvolveObj->m_szSize, MAX_EVO_EFFECT_LEN, Excel.GetData( Col++, cBuffer ) );

		//캡슐진화에서 진화 트리 사용 chu8820
		pEvolveObj->m_nEvolutionTree = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nJoGressQuestCheck = atoi( Excel.GetData( Col++, cBuffer ) );

		pEvolveObj->m_nChipsetType = atoi( Excel.GetData( Col++, cBuffer ) );	
		pEvolveObj->m_nChipsetTypeC = atoi( Excel.GetData( Col++, cBuffer ) );		
		pEvolveObj->m_nChipsetNum = atoi( Excel.GetData( Col++, cBuffer ) );
		pEvolveObj->m_nChipsetTypeP = atoi( Excel.GetData( Col++, cBuffer ) );	

		pEvolveObj->m_nJoGressesNum = atoi( Excel.GetData( Col++, cBuffer ) );
		for(int i=0; i<MAX_JOINT_TACTICS_LEN; i++)
			pEvolveObj->m_nJoGress_Tacticses[i] = atoi( Excel.GetData( Col++, cBuffer ) );		

		pEvolveInfo->Add( pEvolveObj );

		bSuccess = Excel.MoveNext();
	}

	// 슬롯에 DigimonID 연결 시켜 놓자
	CDigimonEvolveObj::MAP_IT itObj = pEvolveInfo->m_Map.begin();
	CDigimonEvolveObj::MAP_IT itObjEnd = pEvolveInfo->m_Map.end();
	for ( ; itObj!=itObjEnd ; ++itObj )
	{
		for( int evol=0; evol<MAX_EVOLUTION; ++evol )
			itObj->second->m_nEvolutionList[ evol ].dwDigimonID = dwModelIDArray[ itObj->second->m_nEvolutionList[ evol ].nSlot ];
	}

	m_Map[ pEvolveInfo->GetInfo()->s_nDigimonID ] = pEvolveInfo;
}

void CDigimonEvolution::_LoadExcel_Extra()
{
	char cBuffer[ CEXCLE_CELL_LEN ];
	DWORD nDigimonID;
	bool bSuccess;

	cExcelReader Excel;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
	case nsCsFileTable::ENGLISH:
	case nsCsFileTable::ENGLISH_A:
		bSuccess = Excel.Open( nsCsFileTable::GetPath_DM_Evolution_01(), "디지몬기본체" );
		assert_cs( bSuccess == true );
		break;
	case nsCsFileTable::THAILAND:
	case nsCsFileTable::TAIWAN:
	case nsCsFileTable::HONGKONG:
		bSuccess = Excel.Open( nsCsFileTable::GetPath_DM_Evolution_01(), "Digimon_Basics_Setting"  /*"디지몬기본체"*/ );
		assert_cs( bSuccess == true );
		break;
	default:
		assert_cs( false );
	}
	Excel.MoveFirst();
	Excel.MoveNext();
	Excel.MoveNext();
	while( Excel.MoveNext() )
	{
		nDigimonID = atoi( Excel.GetData( 1, cBuffer ) );
		if( nDigimonID == 0 )
			continue;

		GetEvolveInfo( nDigimonID )->GetInfo()->s_nBattleEvoType = atoi( Excel.GetData( 3, cBuffer ) );
	}
}

void CDigimonEvolution::_LoadExcel_BattleEvo()
{
	char cBuffer[ CEXCLE_CELL_LEN ];
	int nEvoStep;
	int nElementIndex;
	
	for( int i=0; i<FT_BATTLE_EVO_TYPE_CNT; ++i )
	{
		char cSheet[ 32 ];
		sprintf_s( cSheet, 32, "DigisoulCharge_%02d", i+1 );

		sBATTLE_EVO_INFO* pInfo = &m_BattleEvoInfo[ i ];

		nElementIndex = 0;

		cExcelReader Excel;
		bool b = Excel.Open( nsCsFileTable::GetPath_DM_Battle_Evo(), cSheet );
		Excel.MoveFirst();
		Excel.MoveNext();		
		while( Excel.MoveNext() )
		{			
			nEvoStep = atoi( Excel.GetData( 2, cBuffer ) );
			if( nEvoStep == 0 )
				continue;
			
			sBATTLE_EVO_INFO::sELEMENT* pEle = &pInfo->s_Element[ nElementIndex ];

			pEle->s_bEnable		= true;
			pEle->s_nEvolStep	= nEvoStep;
			pEle->s_nReqEvoEP	= atoi( Excel.GetData( 4, cBuffer ) );
			pEle->s_nDecEP		= atoi( Excel.GetData( 5, cBuffer ) );
			pInfo->s_nMaxEP		= pEle->s_nReqEvoEP;

			++nElementIndex;
		}
	}	
}

void CDigimonEvolution::Reload()
{
	static bool bIng = false;
	if( bIng )
		return;
	bIng = true;

	char cBuffer[ CEXCLE_CELL_LEN ];
	char cSheet[ 32 ];
	bool bSuccess;
	cExcelReader Excel;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
	case nsCsFileTable::ENGLISH:
	case nsCsFileTable::ENGLISH_A:
		bSuccess = Excel.Open( nsCsFileTable::GetPath_DM_Evolution_01(), "진화트리정보" );
		assert_cs( bSuccess == true );
		break;
	case nsCsFileTable::THAILAND:
	case nsCsFileTable::TAIWAN:
	case nsCsFileTable::HONGKONG:
		bSuccess = Excel.Open( nsCsFileTable::GetPath_DM_Evolution_01(), "Evolution_Max_Setting"  /*"진화트리정보"*/ );
		assert_cs( bSuccess == true );
		break;
	default:
		assert_cs( false );
	}
	Excel.MoveFirst();
	Excel.MoveNext();
	Excel.MoveNext();
	int nMaxSlot = atoi( Excel.GetData( 1, cBuffer ) );

	for( int i=1; i<=nMaxSlot; ++i )
	{
		sprintf_s( cSheet, 32, "%02d", i );

		if( i<=50 )
			_Reload( nsCsFileTable::GetPath_DM_Evolution_01(), cSheet );
		else
			_Reload( nsCsFileTable::GetPath_DM_Evolution_02(), cSheet );
	}	

	bIng = false;

}

void CDigimonEvolution::_Reload( char* cPath, char* cSheet )
{
	cExcelReader Excel;
	char cBuffer[ CEXCLE_CELL_LEN ];	

	Excel.Open( cPath, cSheet );
	Excel.MoveFirst();

	Excel.MoveNext();		
	Excel.MoveNext();

	int DigimonID = atoi( Excel.GetData( 2, cBuffer ) );
	CDigimonEvolveInfo* EvoInfo = GetEvolveInfo( DigimonID );

	int nIdx = 1;
	while(true)
	{			
		CDigimonEvolveObj* pObject = EvoInfo->GetEvolveObjByEvoSlot( nIdx );

		if( pObject == NULL )
			break;

		int	Col = 46;	
		pObject->m_bRender = atoi( Excel.GetData( Col++, cBuffer ) );
		pObject->m_TargetStartPos.x = atoi( Excel.GetData( Col++, cBuffer ) );			
		pObject->m_TargetStartPos.y = atoi( Excel.GetData( Col++, cBuffer ) );			
		pObject->m_nStartHegiht = atoi( Excel.GetData( Col++, cBuffer ) );
		pObject->m_nStartRot = atoi( Excel.GetData( Col++, cBuffer ) );

		pObject->m_TargetEndPos.x = atoi( Excel.GetData( Col++, cBuffer ) );			
		pObject->m_TargetEndPos.y = atoi( Excel.GetData( Col++, cBuffer ) );			
		pObject->m_nEndHegiht = atoi( Excel.GetData( Col++, cBuffer ) );
		pObject->m_nEndRot = atoi( Excel.GetData( Col++, cBuffer ) );

		pObject->m_nSpeed = atoi( Excel.GetData( Col++, cBuffer ) );				
		pObject->m_dwAni = atoi( Excel.GetData( Col++, cBuffer ) );			
		pObject->m_fStartTime = atof( Excel.GetData( Col++, cBuffer ) );
		pObject->m_fEndTime = atof( Excel.GetData( Col++, cBuffer ) );

		pObject->m_nR = atoi( Excel.GetData( Col++, cBuffer ) );
		pObject->m_nG = atoi( Excel.GetData( Col++, cBuffer ) );
		pObject->m_nB = atoi( Excel.GetData( Col++, cBuffer ) );

		strcpy_s( pObject->m_szLevel, MAX_EVO_EFFECT_LEN, Excel.GetData( Col++, cBuffer ) );
		strcpy_s( pObject->m_szEnchant, MAX_EVO_EFFECT_LEN, Excel.GetData( Col++, cBuffer ) );
		strcpy_s( pObject->m_szSize, MAX_EVO_EFFECT_LEN, Excel.GetData( Col++, cBuffer ) );

		nIdx++;
		Excel.MoveNext();
	}		
}


#define DIGIMON_EVO_BIN		"DigimonEvo.bin"

void CDigimonEvolution::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DIGIMON_EVO_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_Map.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CDigimonEvolveInfo::MAP_IT it = m_Map.begin();
	CDigimonEvolveInfo::MAP_IT itEnd = m_Map.end();
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second->GetInfo(), sizeof( CDigimonEvolveInfo::sINFO ), 1, fp );

		int nCountEvo = (int)it->second->m_Map.size();
		fwrite( &nCountEvo, sizeof( int ), 1, fp );
		CDigimonEvolveObj::MAP_IT itEvo = it->second->m_Map.begin();
		CDigimonEvolveObj::MAP_IT itEvoEnd = it->second->m_Map.end();
		for( ; itEvo!=itEvoEnd; ++itEvo )
		{
			fwrite( itEvo->second, sizeof( CDigimonEvolveObj ), 1, fp );
		}
	}

	nCount = FT_BATTLE_EVO_TYPE_CNT;
	fwrite( &nCount, sizeof( int ), 1, fp );
	for( int i=0; i<nCount; ++i )
		fwrite( &m_BattleEvoInfo[ i ], sizeof( sBATTLE_EVO_INFO ), 1, fp );

	fclose( fp );
}

bool CDigimonEvolution::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DIGIMON_EVO_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	for( int i=0; i<nCount; ++i )
	{
		CDigimonEvolveInfo* pObject = csnew CDigimonEvolveInfo;
		fread( pObject->GetInfo(), sizeof( CDigimonEvolveInfo::sINFO ), 1, fp );

		int nCountEvo;
		fread( &nCountEvo, sizeof( int ), 1, fp );
		
		for( int e=0; e<nCountEvo; ++e )
		{
			CDigimonEvolveObj* pObjectEvo = csnew CDigimonEvolveObj;
			fread( pObjectEvo, sizeof( CDigimonEvolveObj ), 1, fp );
			pObject->m_Map[ pObjectEvo->m_dwID ] = pObjectEvo;
		}

		assert_cs( m_Map.find( pObject->GetInfo()->s_nDigimonID ) == m_Map.end() );
		m_Map[ pObject->GetInfo()->s_nDigimonID ] = pObject;
	}

	fread( &nCount, sizeof( int ), 1, fp );
	for( int i=0; i<nCount; ++i )
		fread( &m_BattleEvoInfo[ i ], sizeof( sBATTLE_EVO_INFO ), 1, fp );

	fclose( fp );

	return true;
}

void CDigimonEvolution::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, DIGIMON_EVO_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		CDigimonEvolveInfo* pObject = csnew CDigimonEvolveInfo;
		_read( nHandle, pObject->GetInfo(), sizeof( CDigimonEvolveInfo::sINFO ) );

		int nCountEvo;
		_read( nHandle, &nCountEvo, sizeof( int ) );

		for( int e=0; e<nCountEvo; ++e )
		{
			CDigimonEvolveObj* pObjectEvo = csnew CDigimonEvolveObj;
			_read( nHandle, pObjectEvo, sizeof( CDigimonEvolveObj ) );
			pObject->m_Map[ pObjectEvo->m_dwID ] = pObjectEvo;
		}

		assert_cs( m_Map.find( pObject->GetInfo()->s_nDigimonID ) == m_Map.end() );
		m_Map[ pObject->GetInfo()->s_nDigimonID ] = pObject;
	}

	_read( nHandle, &nCount, sizeof( int ) );
	for( int i=0; i<nCount; ++i )
		_read( nHandle, &m_BattleEvoInfo[ i ], sizeof( sBATTLE_EVO_INFO ) );

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}
