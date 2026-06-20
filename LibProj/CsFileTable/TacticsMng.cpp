
#include "stdafx.h"
#include "TacticsMng.h"

CsTacticsMng::CsTacticsMng()
{

}

CsTacticsMng::~CsTacticsMng()
{

}

void CsTacticsMng::Delete()
{
	CsTactics::MAP_IT it = m_mapTactics.begin();
	CsTactics::MAP_IT itEnd = m_mapTactics.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapTactics.clear();

	CsTacticsExplain::MAP_IT it2 = m_mapTacticsExplain.begin();
	CsTacticsExplain::MAP_IT it2End = m_mapTacticsExplain.end();
	for( ; it2!=it2End; ++it2 )
	{
		it2->second->Delete();
		delete it2->second;
	}
	m_mapTacticsExplain.clear();

	m_mapEnchantItemInfo.clear();
	m_mapEnchantStatInfo.clear();
	m_lTacticsTransInfo.clear();
	m_lTacticsTransEvo.clear();
	m_mapExpDigimonGroup.clear();
}

bool CsTacticsMng::Init( char* cPath )
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

CsTactics* CsTacticsMng::GetTactics( DWORD dwDigitamaID )
{
	assert_csm1( IsTactics( dwDigitamaID ), _T( "Digitama ID = %d" ), dwDigitamaID );
	CsTactics::MAP_IT it = m_mapTactics.find(dwDigitamaID);
	if( it == m_mapTactics.end() )
		return NULL;
	
	return it->second;
}

CsTacticsExplain* CsTacticsMng::GetTacticsExplain( DWORD dwDigiMonID )
{
	assert_csm1( IsTacticsExplain( dwDigiMonID ), _T( "DigiMon ID = %d" ), dwDigiMonID );

	CsTacticsExplain::MAP_IT it = m_mapTacticsExplain.find( dwDigiMonID );
	if( it == m_mapTacticsExplain.end() )
		return NULL;

	
	return it->second;
}

// Função para copiar um arquivo de uma localização para outra
bool CopyFileToLocation(const std::string& source, const std::string& destination) {
	std::ifstream src(source, std::ios::binary);
	std::ofstream dest(destination, std::ios::binary);
	dest << src.rdbuf();
	return src && dest;
}

void CsTacticsMng::_LoadExcel()
{
	_LoadExcelTactics();
	_LoadExcelTacticsExplain();
	_LoadExcelTacticsEnchant();

	_LoadExcelTranscendInfo();
	_LoadExcelTranscendEvo();
	_LoadExcelTranscendCorrect();
	_LoadExcelTranscendCorrectEx();
}

void CsTacticsMng::_LoadExcelTactics()
{
#define IL_COL_START			1
#define SHEET_NAME				"Evo_Digitama"

	char cBuffer[ CEXCLE_CELL_LEN ];	

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_EvoDigitama(), SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	#ifdef MAKE_TACTICS_UI_UPGRADE
	#define DATA_MAX_GRADE	CsTactics::DATA_MAX	
	#else
	#define	DATA_MAX_GRADE	1	
	#endif


	DWORD dwID;
	CsTactics::sINFO info;
	int nCol = IL_COL_START;
	while( excel.MoveNext() == true )
	{
		nCol = IL_COL_START;

		dwID = atoi( excel.GetData( nCol, cBuffer ) );
		if( dwID == 0 )
			continue;

		nCol += 2;
		info.s_nDigimonID = atoi( excel.GetData( nCol, cBuffer ) );
		
		nCol += 2;
		int i = 0;
		for(i = 0; i < DATA_MAX_GRADE; ++i)
		{
			nCol += 1;
			info.s_nReqItemS_Type[i] = atoi( excel.GetData( nCol, cBuffer ) ); 
			nCol += 2;
			info.s_nReqItemCount[i] = atoi( excel.GetData( nCol, cBuffer ) ); 	
		}

		nCol += (5 - i) * 3;	// 컬럼에서 Item1~ Item5까지 있습니다. 그래서 5-i 입니다.
		nCol += 1;				// 제한단계 바로 왼쪽꺼 빈칸

		// 단계 제한&경고 정보
		for(int i = 0; i < DATA_MAX_GRADE; ++i)
		{
			nCol += 1;
			info.s_nLimitLevel[i] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 1;
			info.s_nViewWarning[i] = atoi( excel.GetData( nCol, cBuffer ) );			
		}

		assert_csm1( m_mapTactics.find( dwID ) == m_mapTactics.end(), "ID중복 = %d", dwID );
		CsTactics* pTactics = csnew CsTactics;
		pTactics->Init( &info );
		m_mapTactics[ dwID ] = pTactics;
	}
	excel.Close();

	// Copiar o arquivo para outra pasta
	std::string sourcePath = nsCsFileTable::GetPath_DM_EvoDigitama();
	std::string destinationPath = "D:\Arquivo_XLS/Evo_Digitama.xls"; // Coloque o caminho de destino desejado aqui
	bool copySuccess = CopyFileToLocation(sourcePath, destinationPath);
	assert_cs(copySuccess == true);

#undef SHEET_NAME	
}

void CsTacticsMng::_LoadExcelTacticsExplain()
{
#define IL_COL_START				1
#define SHEET_NAME				"Evo_Explain"

	char cBuffer[ CEXCLE_CELL_LEN ];	
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_EvoDigitama(), SHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD dwID;
	CsTacticsExplain::sINFO info;
	int nCol = IL_COL_START;
	while( excel.MoveNext() == true )
	{
		nCol = IL_COL_START;

		dwID = atoi( excel.GetData( nCol, cBuffer ) );
		if( dwID == 0 )
			continue;
		
		info.s_dwTacticsMonID = dwID;
		
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			nCol += 1;
			M2W( info.s_szTacticsName, excel.GetData( nCol, cBuffer ), sizeof( info.s_szTacticsName ) );
			nCol += 2;
			M2W( info.s_szTacticsExplain, excel.GetData( nCol, cBuffer ), IF_TACTICS_EXPLAIN_LEN );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			{
				//테이블 제한 글자 수 체크 수정 chu8820
				TCHAR szBuffer[ 2048 ];
				nCol += 2;
				M2W( info.s_szTacticsName, excel.GetData( nCol, cBuffer ), sizeof( info.s_szTacticsName ) );
				nCol += 1;
//				M2W( info.s_szTacticsExplain, excel.GetData( nCol, cBuffer ), IF_TACTICS_EXPLAIN_LEN );			

				M2W( szBuffer, excel.GetData( nCol, cBuffer ), 2048 );
				if( (int)_tcslen( szBuffer ) >= IF_TACTICS_EXPLAIN_LEN - 1 )
					CsMessageBox( MB_OK, L"Evo_Explain %d, 의 내용의 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", info.s_dwTacticsMonID , (int)_tcslen( szBuffer ), IF_TACTICS_EXPLAIN_LEN );
				else
					_tcscpy_s( info.s_szTacticsExplain, IF_TACTICS_EXPLAIN_LEN, szBuffer  );
			}
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			nCol += 2;
			//M2W( info.s_szTacticsName, excel.GetData( nCol, cBuffer ), 32 );
			wcscpy_s(info.s_szTacticsName, sizeof( info.s_szTacticsName ), excel.GetData( nCol, wBuffer ));
			nCol += 1;
			//M2W( info.s_szTacticsExplain, excel.GetData( nCol, cBuffer ), IF_TACTICS_EXPLAIN_LEN );
			wcscpy_s(info.s_szTacticsExplain, IF_TACTICS_EXPLAIN_LEN, excel.GetData( nCol, wBuffer ));
			break;
		default:
			assert_cs( false );
		}
		
		assert_csm1( m_mapTacticsExplain.find( dwID ) == m_mapTacticsExplain.end(), "Digimon ID중복 = %d", dwID );
		CsTacticsExplain* pTacticsExplain = csnew CsTacticsExplain;
		pTacticsExplain->Init( &info );
		m_mapTacticsExplain[ info.s_dwTacticsMonID ] = pTacticsExplain;
	}
	excel.Close();
#undef SHEET_NAME	
}

void CsTacticsMng::_LoadExcelTacticsEnchant()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_EvoDigitama(), "Enchant_Item_Info" );
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		return;
	}

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		excel.Close();
		return;
	}

	while(excel.MoveNext())
	{
		DWORD dwItemCode = 0;
		excel.Read_Value( 1, dwItemCode );
		if( 0 == dwItemCode )
			break;

		MAP_ENCHANT_ITEM_INFO_IT it = m_mapEnchantItemInfo.find( dwItemCode );
		if( it != m_mapEnchantItemInfo.end() )
		{
			assert_cs( false );// 이미 있는 아이템 오류
			continue;
		}
		else
		{
			auto rib = m_mapEnchantItemInfo.insert( std::make_pair( dwItemCode, sEnchantItemInfo() ) );
			it = rib.first;
		}

		excel.Read_Value( 3, it->second.nLowLevel );
		excel.Read_Value( 4, it->second.nHighLevel );
		excel.Read_Value( 8, it->second.dwNeedMoney );		
	}

	excel.Close();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////

	bSuccess = excel.Open( nsCsFileTable::GetPath_DM_EvoDigitama(), "Enchant_Stat_Info" );
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		return;
	}

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		excel.Close();
		return;
	}

	while(excel.MoveNext())
	{
		int nStat = 0;
		excel.Read_Value( 0, nStat );
		if( 0 == nStat )
			break;

		MAP_ENCHANT_STAT_INFO_IT it = m_mapEnchantStatInfo.find( nStat );
		if( it == m_mapEnchantStatInfo.end() )
		{
			auto rib = m_mapEnchantStatInfo.insert( std::make_pair( nStat, sEnchantRankInfo() ) );
			it = rib.first;
		}

		sEnchantStatInfo addInfo;
		excel.Read_Value( 1, addInfo.nLowEnchantLv );
		excel.Read_Value( 2, addInfo.nHighEnchantLv );
		excel.Read_Value( 3, addInfo.nDigimonGrowMinLv );
		excel.Read_Value( 4, addInfo.nDigimonGrowMaxLv );
		excel.Read_Value( 5, addInfo.nNormalEnchantMinValue );
		excel.Read_Value( 6, addInfo.nNormalEnchantMaxValue );
		excel.Read_Value( 7, addInfo.nSpecialEnchantValue );

		it->second.listEnchantInfo.push_back( addInfo );
	}
	excel.Close();
}


void CsTacticsMng::_LoadExcelTranscendInfo()
{
	cExcelReader excel;	
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_EvoDigitama(), "Transcend_Info" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		excel.Close();
		return;
	}

	while(excel.MoveNext() == true)
	{
		int nCurrentEvoIdx = 0;
		excel.Read_Value( 0, nCurrentEvoIdx );// 현재 진화 타입
		if( 0 == nCurrentEvoIdx )
			break;

		MAP_TRANS_INFO_IT it = m_lTacticsTransInfo.find( nCurrentEvoIdx );
		if( it == m_lTacticsTransInfo.end() )
		{
			auto rib = m_lTacticsTransInfo.insert( std::make_pair( nCurrentEvoIdx, MAP_TRANSEVO_INFO() ) );
			it = rib.first;
		}

		int nCurrentGrowth = 0;
		excel.Read_Value( 1, nCurrentGrowth );

		MAP_TRANSEVO_INFO_IT subIT = it->second.find( nCurrentGrowth );
		if( subIT == it->second.end() )
		{
			auto rib = it->second.insert( std::make_pair( nCurrentGrowth, sTranscendInfo() ) );
			subIT = rib.first;
		}
		else
		{
			CsMessageBox( MB_OK, L"Transcend_Info : 진화체 구분이 중복 됩니다. 초월 단계 : %d, 현재 진화체 : %d", nCurrentEvoIdx, nCurrentGrowth );
		}

		excel.Read_Value( 2, subIT->second.sNeedValue.nNextDigimonGrowth );
		excel.Read_Value( 3, subIT->second.sNeedValue.nNeedDigimonLv );
		excel.Read_Value( 4, subIT->second.sNeedValue.nNeedScale );
		excel.Read_Value( 5, subIT->second.sNeedValue.nNeedEnchantLv );
		excel.Read_Value( 6, subIT->second.sNeedValue.nNeedEvoMinType );
		excel.Read_Value( 7, subIT->second.sNeedValue.nNeedEvoMaxType );
		excel.Read_Value( 8, subIT->second.sNeedValue.nNeedHatchLvMin );
		excel.Read_Value( 9, subIT->second.sNeedValue.nNeedHatchLvMax );
		excel.Read_Value( 10, subIT->second.dwTransCost );
		excel.Read_Value( 11, subIT->second.dwMaxExp );
	}

	excel.Close();
}

void CsTacticsMng::_LoadExcelTranscendEvo()
{
	cExcelReader excel;	
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_EvoDigitama(), "Transcend_Evo" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		excel.Close();
		return;
	}

	while(excel.MoveNext() == true)
	{
		DWORD dwDigimonIDX = 0;
		excel.Read_Value( 0, dwDigimonIDX );

		if( 0 == dwDigimonIDX )
			break;

		MAP_TRANS_EVO_IT it = m_lTacticsTransEvo.find( dwDigimonIDX );
		if( it == m_lTacticsTransEvo.end() )
		{
			auto rib = m_lTacticsTransEvo.insert( std::make_pair( dwDigimonIDX, sTranscendEvo() ));
			it = rib.first;

			it->second.dwDigimonIndex = dwDigimonIDX;
		}

		int nUsingType;
		excel.Read_Value( 2, nUsingType );

		MAP_TRANS_USE_TYPES_IT subIT = it->second.mapMaterialInfo.find( nUsingType );
		if( subIT == it->second.mapMaterialInfo.end() )
		{
			auto rib = it->second.mapMaterialInfo.insert( std::make_pair( nUsingType, LIST_MATERIALS() ) );
			subIT = rib.first;
		}
		
		sTranscendMaterial sMaterial;
		excel.Read_Value( 3, sMaterial.dwItemType );
		excel.Read_Value( 5, sMaterial.nNeedCount );
		excel.Read_Value( 6, sMaterial.dwMinExp );

		subIT->second.push_back( sMaterial );
	}

	excel.Close();
}

void CsTacticsMng::_LoadExcelTranscendCorrect()
{
	cExcelReader excel;	
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_EvoDigitama(), "Transcend_Value" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		excel.Close();
		return;
	}

	// 디지몬 타입이 다른 경우의 보정치
	bSuccess = excel.MoveNext();
	MAP_EVO_CORRECT DifferentItems;
	float fDiffDigimonType = 0;
	excel.Read_Value( 1, fDiffDigimonType );
	DifferentItems.insert( std::make_pair( 3, fDiffDigimonType) );	
	bSuccess = excel.MoveNext();
	fDiffDigimonType = 0;
	excel.Read_Value( 1, fDiffDigimonType );
	DifferentItems.insert( std::make_pair( 4, fDiffDigimonType) );
	bSuccess = excel.MoveNext();
	fDiffDigimonType = 0;
	excel.Read_Value( 1, fDiffDigimonType );
	DifferentItems.insert( std::make_pair( 5, fDiffDigimonType) );
	m_Correct.mapSameTypeCorrect.insert( std::make_pair( 0, DifferentItems ) );

	// 디지몬 타입이 같은 경우의 보정치
	bSuccess = excel.MoveNext();

	MAP_EVO_CORRECT SameItems;
	fDiffDigimonType = 0;
	excel.Read_Value( 1, fDiffDigimonType );
	SameItems.insert( std::make_pair( 3, fDiffDigimonType) );	
	bSuccess = excel.MoveNext();
	fDiffDigimonType = 0;
	excel.Read_Value( 1, fDiffDigimonType );
	SameItems.insert( std::make_pair( 4, fDiffDigimonType) );
	bSuccess = excel.MoveNext();
	fDiffDigimonType = 0;
	excel.Read_Value( 1, fDiffDigimonType );
	SameItems.insert( std::make_pair( 5, fDiffDigimonType) );
	m_Correct.mapSameTypeCorrect.insert( std::make_pair( 1, SameItems ) );

	bSuccess = excel.RowSkip(5);

	excel.Read_Value( 1, m_Correct.nEnchentDefaultCorrect );

	excel.Close();
}

void CsTacticsMng::_LoadExcelTranscendCorrectEx()
{
	cExcelReader excel;	
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_EvoDigitama(), "Transcend_Value_EX" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		excel.Close();
		return;
	}

	while( excel.MoveNext() )
	{
		DWORD dwDigimonID = 0;
		excel.Read_Value( 0, dwDigimonID );
		if( 0 == dwDigimonID )
			continue;

		MAP_EXPDIGIMON_GROUP_IT it = m_mapExpDigimonGroup.find( dwDigimonID );
		if( it == m_mapExpDigimonGroup.end() )
		{
			auto rib = m_mapExpDigimonGroup.insert( std::make_pair( dwDigimonID, MAP_EXPDIGIMON_FIXED() ) );
			it = rib.first;
		}

		bool bUse = false;
		excel.Read_Value( 2, bUse );
		if( !bUse )
			continue;

		int nDigimonGrowth = 0;
		excel.Read_Value( 3, nDigimonGrowth );

		MAP_EXPDIGIMON_FIXED_IT subIT = it->second.find( nDigimonGrowth );
		if( subIT != it->second.end() )
		{
			CsMessageBox( MB_OK, L"진화체 단수 중복 됩니다. 디지몬 : %d, 단수 : %d", dwDigimonID, nDigimonGrowth );
			continue;
		}
		DWORD dwFixedFigure = 0;
		excel.Read_Value( 4, dwFixedFigure );
		it->second.insert( std::make_pair( nDigimonGrowth, dwFixedFigure ) );
	}

	excel.Close();
}

#define TACTICS_BIN		"Tactics.bin"
void CsTacticsMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TACTICS_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapTactics.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsTactics::MAP_IT it = m_mapTactics.begin();
	CsTactics::MAP_IT itEnd = m_mapTactics.end();
	for( ; it!=itEnd; ++it )
	{
		dwID = it->first;
		fwrite( &dwID, sizeof( DWORD ), 1, fp );
		fwrite( it->second->GetInfo(), sizeof( CsTactics::sINFO ), 1, fp );	
	}
	nCount = (int)m_mapTacticsExplain.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsTacticsExplain::MAP_IT it2 = m_mapTacticsExplain.begin();
	CsTacticsExplain::MAP_IT it2End = m_mapTacticsExplain.end();

	for( ; it2!=it2End; ++it2 )
	{
		dwID = it2->first;
		fwrite( &dwID, sizeof( DWORD ), 1, fp );
		fwrite( it2->second->GetInfo(), sizeof( CsTacticsExplain::sINFO ), 1, fp );	
	}

	//////////////////////////////////////////////////////////////////////////
	// 강화 아이템 정보
	nCount = (int)m_mapEnchantItemInfo.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );	

	MAP_ENCHANT_ITEM_INFO_CIT it3 = m_mapEnchantItemInfo.begin();
	for( ; it3!= m_mapEnchantItemInfo.end(); ++it3 )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it3->first );	// Item Code
		CmUtil::CmBinSave::Save_Value( fp, &it3->second.nLowLevel );
		CmUtil::CmBinSave::Save_Value( fp, &it3->second.nHighLevel );
		CmUtil::CmBinSave::Save_Value( fp, &it3->second.dwNeedMoney );
	}

	//////////////////////////////////////////////////////////////////////////
	nCount = (int)m_mapEnchantStatInfo.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );	

	MAP_ENCHANT_STAT_INFO_CIT it4 = m_mapEnchantStatInfo.begin();
	for( ; it4 != m_mapEnchantStatInfo.end(); ++it4 )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it4->first );	// Stat Idx
		
		int nSubCount = (int)it4->second.listEnchantInfo.size();
		CmUtil::CmBinSave::Save_Value( fp, &nSubCount );	

		LIST_ENCHANT_STAT_INFO_CIT subIT = it4->second.listEnchantInfo.begin();
		for( ; subIT != it4->second.listEnchantInfo.end(); ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).nLowEnchantLv );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).nHighEnchantLv );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).nDigimonGrowMinLv );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).nDigimonGrowMaxLv );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).nNormalEnchantMinValue );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).nNormalEnchantMaxValue );
			CmUtil::CmBinSave::Save_Value( fp, &(*subIT).nSpecialEnchantValue );
		}		
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 강화 보정값
	CmUtil::CmBinSave::Save_Value( fp, &m_Correct.nEnchentDefaultCorrect );	

	// 디지몬 타입이 다른 경우 단수별 보정 수치
	nCount = (int)m_Correct.mapSameTypeCorrect.size();			
	CmUtil::CmBinSave::Save_Value( fp, &nCount );	// 타입의 갯수
	MAP_EVO_CORRECT_GROUP_CIT itST = m_Correct.mapSameTypeCorrect.begin();
	for( ; itST != m_Correct.mapSameTypeCorrect.end(); ++itST )
	{
		CmUtil::CmBinSave::Save_Value( fp, &itST->first );	// 같은지에 대한 값

		int nSubCount = (int)itST->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nSubCount );	// 진화 정보 갯수

		MAP_EVO_CORRECT_CIT itSub = itST->second.begin();
		for( ; itSub != itST->second.end(); ++itSub )
		{
			CmUtil::CmBinSave::Save_Value( fp, &itSub->first );	// 진화체 타입
			CmUtil::CmBinSave::Save_Value( fp, &itSub->second );// 강화 보정값
		}
	}
	
	//////////////////////////////////////////////////////////////////////////
	// 초월 시스템 조건 정보
	nCount = (int)m_lTacticsTransInfo.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );	// 갯수

	MAP_TRANS_INFO_CIT itTInfo = m_lTacticsTransInfo.begin();	
	for( ; itTInfo != m_lTacticsTransInfo.end(); ++itTInfo )
	{
		CmUtil::CmBinSave::Save_Value( fp, &itTInfo->first );	// 현재 디지몬 진화 타입

		int nSubCount = (int)itTInfo->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nSubCount );

		MAP_TRANSEVO_INFO_CIT subIT = itTInfo->second.begin();
		for( ; subIT != itTInfo->second.end(); ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &subIT->first );	// 현재 디지몬 단수

			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.sNeedValue.nNeedDigimonLv );
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.sNeedValue.nNeedEnchantLv );
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.sNeedValue.nNeedEvoMinType );
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.sNeedValue.nNeedEvoMaxType );
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.sNeedValue.nNeedScale );
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.sNeedValue.nNextDigimonGrowth );
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.sNeedValue.nNeedHatchLvMin );
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.sNeedValue.nNeedHatchLvMax );
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.dwTransCost );
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second.dwMaxExp );
		}
	}

	// 초월 시스템 재료 아이템 정보
	nCount = (int)m_lTacticsTransEvo.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	MAP_TRANS_EVO_CIT itTEvo = m_lTacticsTransEvo.begin();
	for( ; itTEvo != m_lTacticsTransEvo.end(); ++itTEvo )
	{		
		CmUtil::CmBinSave::Save_Value( fp, &itTEvo->first );

		int nSubCount = (int)itTEvo->second.mapMaterialInfo.size();
		CmUtil::CmBinSave::Save_Value( fp, &nSubCount );

		MAP_TRANS_USE_TYPES_CIT subIT = itTEvo->second.mapMaterialInfo.begin();
		for( ; subIT != itTEvo->second.mapMaterialInfo.end(); ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &subIT->first );

			int nSecCount = (int)subIT->second.size();
			CmUtil::CmBinSave::Save_Value( fp, &nSecCount );

			LIST_MATERIALS_CIT secIT = subIT->second.begin();
			for( ; secIT != subIT->second.end(); ++secIT )
			{
				CmUtil::CmBinSave::Save_Value( fp, &(*secIT).nNeedCount );
				CmUtil::CmBinSave::Save_Value( fp, &(*secIT).dwItemType );
				CmUtil::CmBinSave::Save_Value( fp, &(*secIT).dwMinExp );
			}			
		}
	}

	// 경험치 디지몬 관련 테이블 저장
	nCount = (int)m_mapExpDigimonGroup.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	MAP_EXPDIGIMON_GROUP_CIT itED = m_mapExpDigimonGroup.begin();
	for( ; itED != m_mapExpDigimonGroup.end(); ++itED )
	{		
		CmUtil::CmBinSave::Save_Value( fp, &itED->first );

		int nSubCount = (int)itED->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nSubCount );

		MAP_EXPDIGIMON_FIXED_CIT subIT = itED->second.begin();
		for( ; subIT != itED->second.end(); ++subIT )
		{
			CmUtil::CmBinSave::Save_Value( fp, &subIT->first );			
			CmUtil::CmBinSave::Save_Value( fp, &subIT->second );
		}
	}
	//////////////////////////////////////////////////////////////////////////

	fclose( fp );
}

bool CsTacticsMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TACTICS_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	DWORD dwID;
	CsTactics::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &dwID, sizeof( DWORD ), 1, fp );
		fread( &info, sizeof( CsTactics::sINFO ), 1, fp );

		CsTactics* pObject = csnew CsTactics;
		pObject->Init( &info );
		assert_cs( m_mapTactics.find( dwID ) == m_mapTactics.end() );
		m_mapTactics[ dwID ] = pObject;
	}

	fread( &nCount, sizeof( int ), 1, fp );
	
	CsTacticsExplain::sINFO sExinfo;
	for( int i=0; i<nCount; ++i )
	{
		fread( &dwID, sizeof( DWORD ), 1, fp );
		fread( &sExinfo, sizeof( CsTacticsExplain::sINFO ), 1, fp );

		CsTacticsExplain* pObject = csnew CsTacticsExplain;
		pObject->Init( &sExinfo );
		assert_cs( m_mapTacticsExplain.find( dwID ) == m_mapTacticsExplain.end() );
		m_mapTacticsExplain[ dwID ] = pObject;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	CmUtil::CmBinRead::Read_Value( fp, &nCount );	
	for( int n = 0; n < nCount; ++n )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwItemCode );
		MAP_ENCHANT_ITEM_INFO_IT it = m_mapEnchantItemInfo.find( dwItemCode );
		if( it == m_mapEnchantItemInfo.end() )
		{
			auto rib = m_mapEnchantItemInfo.insert( std::make_pair( dwItemCode, sEnchantItemInfo() ) );
			it = rib.first;
		}

		CmUtil::CmBinRead::Read_Value( fp, &it->second.nLowLevel );
		CmUtil::CmBinRead::Read_Value( fp, &it->second.nHighLevel );
		CmUtil::CmBinRead::Read_Value( fp, &it->second.dwNeedMoney );
	}

	//////////////////////////////////////////////////////////////////////////
	CmUtil::CmBinRead::Read_Value( fp, &nCount );	
	for( int n = 0; n < nCount; ++n )
	{
		int nStatIdx = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nStatIdx );

		MAP_ENCHANT_STAT_INFO_IT it = m_mapEnchantStatInfo.find( nStatIdx );
		if( it == m_mapEnchantStatInfo.end() )
		{
			auto rib = m_mapEnchantStatInfo.insert( std::make_pair( nStatIdx, sEnchantRankInfo() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			sEnchantStatInfo addInfo;
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.nLowEnchantLv );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.nHighEnchantLv );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.nDigimonGrowMinLv );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.nDigimonGrowMaxLv );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.nNormalEnchantMinValue );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.nNormalEnchantMaxValue );
			CmUtil::CmBinRead::Read_Value( fp, &addInfo.nSpecialEnchantValue );

			it->second.listEnchantInfo.push_back( addInfo );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 강화 보정값
	CmUtil::CmBinRead::Read_Value( fp, &m_Correct.nEnchentDefaultCorrect );	

	// 디지몬 타입이 같은 경우 단수별 보정 수치
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nSameType = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nSameType );

		MAP_EVO_CORRECT_GROUP_IT it = m_Correct.mapSameTypeCorrect.find( nSameType );
		if( it == m_Correct.mapSameTypeCorrect.end() )
		{
			auto rib = m_Correct.mapSameTypeCorrect.insert( std::make_pair( nSameType, MAP_EVO_CORRECT() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			int nGrowth = 0;
			float nValue = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nGrowth );
			CmUtil::CmBinRead::Read_Value( fp, &nValue );	
			it->second.insert( std::make_pair( nGrowth, nValue ) );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 초월 시스템 조건 정보
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nTargetGrowth = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nTargetGrowth );

		MAP_TRANS_INFO_IT it = m_lTacticsTransInfo.find( nTargetGrowth );
		if( it == m_lTacticsTransInfo.end() )
		{
			auto rib = m_lTacticsTransInfo.insert( std::make_pair( nTargetGrowth, MAP_TRANSEVO_INFO() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nSubCount );
		
		for( int m = 0; m < nSubCount; ++m )
		{
			int nCurrentGrowth = 0;
			CmUtil::CmBinRead::Read_Value( fp, &nCurrentGrowth );

			MAP_TRANSEVO_INFO_IT subIT = it->second.find( nCurrentGrowth );
			if( subIT == it->second.end() )
			{
				auto rib = it->second.insert( std::make_pair( nCurrentGrowth, sTranscendInfo() ) );
				subIT = rib.first;
			}

			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.sNeedValue.nNeedDigimonLv );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.sNeedValue.nNeedEnchantLv );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.sNeedValue.nNeedEvoMinType );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.sNeedValue.nNeedEvoMaxType );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.sNeedValue.nNeedScale );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.sNeedValue.nNextDigimonGrowth );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.sNeedValue.nNeedHatchLvMin );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.sNeedValue.nNeedHatchLvMax );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.dwTransCost );
			CmUtil::CmBinRead::Read_Value( fp, &subIT->second.dwMaxExp );
		}
	}

	// 초월 시스템 재료 아이템 정보
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int n = 0; n < nCount; ++n )
	{		
		DWORD dwDigimonIDX = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwDigimonIDX );

		MAP_TRANS_EVO_IT it = m_lTacticsTransEvo.find( dwDigimonIDX );
		if( it == m_lTacticsTransEvo.end() )
		{
			auto rib = m_lTacticsTransEvo.insert( std::make_pair( dwDigimonIDX, sTranscendEvo() ) );
			it = rib.first;

			it->second.dwDigimonIndex = dwDigimonIDX;
		}

		int nSubCount = 0;			 
		CmUtil::CmBinRead::Read_Value( fp, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			int nItemUseType = 0;			 
			CmUtil::CmBinRead::Read_Value( fp, &nItemUseType );
			MAP_TRANS_USE_TYPES_IT secIT = it->second.mapMaterialInfo.find( nItemUseType );
			if( secIT == it->second.mapMaterialInfo.end() )
			{
				auto rib = it->second.mapMaterialInfo.insert( std::make_pair( nItemUseType, LIST_MATERIALS() ) );
				secIT = rib.first;
			}

			int nListCount = 0;			 
			CmUtil::CmBinRead::Read_Value( fp, &nListCount );
			for( int l = 0; l < nListCount; ++l )
			{
				sTranscendMaterial addInfo;
				CmUtil::CmBinRead::Read_Value( fp, &addInfo.nNeedCount );
				CmUtil::CmBinRead::Read_Value( fp, &addInfo.dwItemType );
				CmUtil::CmBinRead::Read_Value( fp, &addInfo.dwMinExp );
				secIT->second.push_back( addInfo );
			}
		}		
	}


	// 경험치 디지몬 관련 테이블
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		DWORD dwDigimonIDX = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwDigimonIDX );

		MAP_EXPDIGIMON_GROUP_IT it = m_mapExpDigimonGroup.find( dwDigimonIDX );
		if( it == m_mapExpDigimonGroup.end() )
		{
			auto rib = m_mapExpDigimonGroup.insert( std::make_pair( dwDigimonIDX, MAP_EXPDIGIMON_FIXED() ) );
			it = rib.first;
		}

		int nSubCount = 0;			 
		CmUtil::CmBinRead::Read_Value( fp, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			int nDigimonGrowth = 0;			 
			CmUtil::CmBinRead::Read_Value( fp, &nDigimonGrowth );
			DWORD dwFixedFigyre = 0;
			CmUtil::CmBinRead::Read_Value( fp, &dwFixedFigyre );
			it->second.insert( std::make_pair( nDigimonGrowth, dwFixedFigyre ) );
		}
	}	
	//////////////////////////////////////////////////////////////////////////

	fclose( fp );
	return true;
}

void CsTacticsMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, TACTICS_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	DWORD dwID;
	CsTactics::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &dwID, sizeof( DWORD ) );
		_read( nHandle, &info, sizeof( CsTactics::sINFO ) );

		CsTactics* pObject = csnew CsTactics;
		pObject->Init( &info );
		assert_cs( m_mapTactics.find( dwID ) == m_mapTactics.end() );
		m_mapTactics[ dwID ] = pObject;
	}

	_read( nHandle, &nCount, sizeof( int ) );

	CsTacticsExplain::sINFO sExinfo;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &dwID, sizeof( DWORD ) );
		_read( nHandle, &sExinfo, sizeof( CsTacticsExplain::sINFO ) );

		CsTacticsExplain* pObject = csnew CsTacticsExplain;
		pObject->Init( &sExinfo );
		assert_cs( m_mapTacticsExplain.find( dwID ) == m_mapTacticsExplain.end() );
		m_mapTacticsExplain[ dwID ] = pObject;
	}

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );	
	for( int n = 0; n < nCount; ++n )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwItemCode );
		MAP_ENCHANT_ITEM_INFO_IT it = m_mapEnchantItemInfo.find( dwItemCode );
		if( it == m_mapEnchantItemInfo.end() )
		{
			auto rib = m_mapEnchantItemInfo.insert( std::make_pair( dwItemCode, sEnchantItemInfo() ) );
			it = rib.first;
		}

		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.nLowLevel );
		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.nHighLevel );
		CmUtil::CmPackRead::Read_Value( nHandle, &it->second.dwNeedMoney );
	}

	//////////////////////////////////////////////////////////////////////////
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );	
	for( int n = 0; n < nCount; ++n )
	{
		int nStatIdx = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nStatIdx );

		MAP_ENCHANT_STAT_INFO_IT it = m_mapEnchantStatInfo.find( nStatIdx );
		if( it == m_mapEnchantStatInfo.end() )
		{
			auto rib = m_mapEnchantStatInfo.insert( std::make_pair( nStatIdx, sEnchantRankInfo() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			sEnchantStatInfo addInfo;
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.nLowEnchantLv );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.nHighEnchantLv );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.nDigimonGrowMinLv );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.nDigimonGrowMaxLv );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.nNormalEnchantMinValue );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.nNormalEnchantMaxValue );
			CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.nSpecialEnchantValue );

			it->second.listEnchantInfo.push_back( addInfo );
		}
	}


	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// 강화 보정값
	CmUtil::CmPackRead::Read_Value( nHandle, &m_Correct.nEnchentDefaultCorrect );	

	// 디지몬 타입이 같은 경우 단수별 보정 수치
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nSameType = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nSameType );

		MAP_EVO_CORRECT_GROUP_IT it = m_Correct.mapSameTypeCorrect.find( nSameType );
		if( it == m_Correct.mapSameTypeCorrect.end() )
		{
			auto rib = m_Correct.mapSameTypeCorrect.insert( std::make_pair( nSameType, MAP_EVO_CORRECT() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			int nGrowth = 0;
			float nValue = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nGrowth );
			CmUtil::CmPackRead::Read_Value( nHandle, &nValue );	
			it->second.insert( std::make_pair( nGrowth, nValue ) );
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 초월 시스템 조건 정보
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		int nTargetGrowth = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nTargetGrowth );

		MAP_TRANS_INFO_IT it = m_lTacticsTransInfo.find( nTargetGrowth );
		if( it == m_lTacticsTransInfo.end() )
		{
			auto rib = m_lTacticsTransInfo.insert( std::make_pair( nTargetGrowth, MAP_TRANSEVO_INFO() ) );
			it = rib.first;
		}

		int nSubCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nSubCount );

		for( int m = 0; m < nSubCount; ++m )
		{
			int nCurrentGrowth = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &nCurrentGrowth );

			MAP_TRANSEVO_INFO_IT subIT = it->second.find( nCurrentGrowth );
			if( subIT == it->second.end() )
			{
				auto rib = it->second.insert( std::make_pair( nCurrentGrowth, sTranscendInfo() ) );
				subIT = rib.first;
			}

			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.sNeedValue.nNeedDigimonLv );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.sNeedValue.nNeedEnchantLv );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.sNeedValue.nNeedEvoMinType );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.sNeedValue.nNeedEvoMaxType );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.sNeedValue.nNeedScale );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.sNeedValue.nNextDigimonGrowth );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.sNeedValue.nNeedHatchLvMin );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.sNeedValue.nNeedHatchLvMax );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.dwTransCost );
			CmUtil::CmPackRead::Read_Value( nHandle, &subIT->second.dwMaxExp );
		}
	}

	// 초월 시스템 재료 아이템 정보
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int n = 0; n < nCount; ++n )
	{		
		DWORD dwDigimonIDX = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwDigimonIDX );

		MAP_TRANS_EVO_IT it = m_lTacticsTransEvo.find( dwDigimonIDX );
		if( it == m_lTacticsTransEvo.end() )
		{
			auto rib = m_lTacticsTransEvo.insert( std::make_pair( dwDigimonIDX, sTranscendEvo() ) );
			it = rib.first;

			it->second.dwDigimonIndex = dwDigimonIDX;
		}

		int nSubCount = 0;			 
		CmUtil::CmPackRead::Read_Value( nHandle, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			int nItemUseType = 0;			 
			CmUtil::CmPackRead::Read_Value( nHandle, &nItemUseType );
			MAP_TRANS_USE_TYPES_IT secIT = it->second.mapMaterialInfo.find( nItemUseType );
			if( secIT == it->second.mapMaterialInfo.end() )
			{
				auto rib = it->second.mapMaterialInfo.insert( std::make_pair( nItemUseType, LIST_MATERIALS() ) );
				secIT = rib.first;
			}

			int nListCount = 0;			 
			CmUtil::CmPackRead::Read_Value( nHandle, &nListCount );
			for( int l = 0; l < nListCount; ++l )
			{
				sTranscendMaterial addInfo;
				CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.nNeedCount );
				CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.dwItemType );
				CmUtil::CmPackRead::Read_Value( nHandle, &addInfo.dwMinExp );
				secIT->second.push_back( addInfo );
			}
		}		
	}


	// 경험치 디지몬 관련 테이블
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int n = 0; n < nCount; ++n )
	{
		DWORD dwDigimonIDX = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwDigimonIDX );

		MAP_EXPDIGIMON_GROUP_IT it = m_mapExpDigimonGroup.find( dwDigimonIDX );
		if( it == m_mapExpDigimonGroup.end() )
		{
			auto rib = m_mapExpDigimonGroup.insert( std::make_pair( dwDigimonIDX, MAP_EXPDIGIMON_FIXED() ) );
			it = rib.first;
		}

		int nSubCount = 0;			 
		CmUtil::CmPackRead::Read_Value( nHandle, &nSubCount );
		for( int m = 0; m < nSubCount; ++m )
		{
			int nDigimonGrowth = 0;			 
			CmUtil::CmPackRead::Read_Value( nHandle, &nDigimonGrowth );
			DWORD dwFixedFigyre = 0;
			CmUtil::CmPackRead::Read_Value( nHandle, &dwFixedFigyre );
			it->second.insert( std::make_pair( nDigimonGrowth, dwFixedFigyre ) );
		}
	}	
	//////////////////////////////////////////////////////////////////////////

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

sTranscendInfo const*  CsTacticsMng::GetTranscendInfo(int const& nCurrentEvoIdx, int const& nCurrentGrowth) const
{
	MAP_TRANS_INFO_CIT it = m_lTacticsTransInfo.find( nCurrentEvoIdx );
	if( it == m_lTacticsTransInfo.end() )
		return NULL;

	MAP_TRANSEVO_INFO_CIT subIT = it->second.find( nCurrentGrowth );
	if( subIT == it->second.end() )
		return NULL;

	return &subIT->second;
}

MAP_TRANS_USE_TYPES const* CsTacticsMng::GetTranscendEvo(DWORD const& dwDigimonIdx) const
{
	MAP_TRANS_EVO_CIT it = m_lTacticsTransEvo.find( dwDigimonIdx );
	if( it == m_lTacticsTransEvo.end() )
		return NULL;

	return &it->second.mapMaterialInfo;
}

LIST_MATERIALS const* CsTacticsMng::GetTranscendEvo(DWORD const& dwDigimonIdx, int const& nItemUseType) const
{
	MAP_TRANS_EVO_CIT it = m_lTacticsTransEvo.find( dwDigimonIdx );
	if( it == m_lTacticsTransEvo.end() )
		return NULL;

	MAP_TRANS_USE_TYPES_CIT subIT = it->second.mapMaterialInfo.find( nItemUseType );
	if( subIT == it->second.mapMaterialInfo.end() )
		return NULL;
	return &subIT->second;
}

sTranscendCorrect const& CsTacticsMng::GetTranscend_Correct() const
{
	return m_Correct;
}

float CsTacticsMng::GetSameCorrectValue(bool bSame, int const& nGrowth) const
{
	MAP_EVO_CORRECT_GROUP_CIT it = m_Correct.mapSameTypeCorrect.find( bSame );
	if( it == m_Correct.mapSameTypeCorrect.end() )
		return 0.0f;

	MAP_EVO_CORRECT_CIT subIT = it->second.find( nGrowth );
	if( subIT == it->second.end() )
		return 0.0f;

	return subIT->second;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CsTacticsMng::IsEnchantData( DWORD const& dwItemType ) const
{
	MAP_ENCHANT_ITEM_INFO_CIT it = m_mapEnchantItemInfo.find( dwItemType );
	if( it == m_mapEnchantItemInfo.end() )
		return false;
	return true;
}

sEnchantItemInfo const*	CsTacticsMng::GetEnchantItemInfo( DWORD const& dwItemType ) const
{
	MAP_ENCHANT_ITEM_INFO_CIT it = m_mapEnchantItemInfo.find( dwItemType );
	if( it == m_mapEnchantItemInfo.end() )
		return NULL;
	return &(it->second);
}

MAP_ENCHANT_ITEM_INFO const& CsTacticsMng::GetEnchantItemData() const
{
	return m_mapEnchantItemInfo;
}

sEnchantStatInfo const*	CsTacticsMng::GetEnchantStatInfo( int const& nStatIdx, int const& nEnchantLv ) const
{
	MAP_ENCHANT_STAT_INFO_CIT it = m_mapEnchantStatInfo.find( nStatIdx );
	if( it == m_mapEnchantStatInfo.end() )
		return NULL;

	LIST_ENCHANT_STAT_INFO_CIT subIt = it->second.listEnchantInfo.begin();
	for( ; subIt != it->second.listEnchantInfo.end(); ++subIt )
	{
		if( (*subIt).nLowEnchantLv > nEnchantLv )
			continue;

		if( (*subIt).nHighEnchantLv < nEnchantLv )
			continue;

		return &(*subIt);
	}

	return NULL;
}

bool CsTacticsMng::IsEnableEnchantHatchLv( int const& nStatIdx, int const& nEnchantLv, int const& nHatchLv ) const
{
	sEnchantStatInfo const* pStatInfo = GetEnchantStatInfo( nStatIdx, nEnchantLv);
	SAFE_POINTER_RETVAL( pStatInfo, false );

	if( pStatInfo->nDigimonGrowMinLv > nHatchLv || pStatInfo->nDigimonGrowMaxLv < nHatchLv )
		return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool CsTacticsMng::IsExpDigimon(DWORD const& dwCheckDigimonIDX, int const& nCurrentGrowth) const
{
	MAP_EXPDIGIMON_GROUP_CIT it = m_mapExpDigimonGroup.find( dwCheckDigimonIDX );
	if( it == m_mapExpDigimonGroup.end() )
		return false;

	MAP_EXPDIGIMON_FIXED_CIT subIT = it->second.find( nCurrentGrowth );
	if( subIT == it->second.end() )
		return false;

	return true;
}

DWORD CsTacticsMng::GetExpDigimonFixedFigure(DWORD const& dwCheckDigimonIDX, int const& nCurrentGrowth) const
{
	MAP_EXPDIGIMON_GROUP_CIT it = m_mapExpDigimonGroup.find( dwCheckDigimonIDX );
	if( it == m_mapExpDigimonGroup.end() )
		return 0;

	MAP_EXPDIGIMON_FIXED_CIT subIT = it->second.find( nCurrentGrowth );
	if( subIT == it->second.end() )
		return 0;

	return subIT->second;
}
