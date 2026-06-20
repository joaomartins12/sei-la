#include "stdafx.h"
#include "Master_CardMng.h"

void CsMaster_CardMng::Delete()
{
	{
		CsMaster_Card::MAP_IT it = m_mapMaster_Card.begin();
		for( ; it!=m_mapMaster_Card.end(); ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapMaster_Card.clear();	
	}

	{
		CsMaster_CardLeader::MAP_IT it = m_mapMaster_CardLeader.begin();
		for( ; it!= m_mapMaster_CardLeader.end(); ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapMaster_CardLeader.clear();	
	}

	{
		CsMaster_CardLeaderAbility::MAP_IT it = m_mapMaster_CardLeaderAbility.begin();
		for( ; it!=m_mapMaster_CardLeaderAbility.end(); ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapMaster_CardLeaderAbility.clear();	
	}

	{
		CsMaster_Filter::MapGroup_IT groupItr = m_mapMaster_Filter.begin();
		CsMaster_Filter::MapGroup_IT groupItrEnd = m_mapMaster_Filter.end();
		for (;groupItr != groupItrEnd; ++groupItr)
		{
			CsMaster_Filter::Map_IT mapItr = groupItr->second->s_sMap.begin();
			CsMaster_Filter::Map_IT mapItrEnd = groupItr->second->s_sMap.end();
			for (;mapItr != mapItrEnd; ++mapItr)
			{
				CsMaster_Filter::Seal_IT sealItr = mapItr->second->s_sSeal.begin();
				CsMaster_Filter::Seal_IT sealItrEnd = mapItr->second->s_sSeal.end();
				for (; sealItr != sealItrEnd; ++sealItr)
				{
					SAFE_DELETE(sealItr->second);
				}
				mapItr->second->s_sSeal.clear();
				SAFE_DELETE(mapItr->second);
			}
			groupItr->second->s_sMap.clear();
			SAFE_DELETE(groupItr->second);
		}
		m_mapMaster_Filter.clear();
	}

	m_mapMaster_PathDigimon.clear();
	m_mapMaster_PathPlate.clear();
	m_mapMaster_PathElemental.clear();
	m_mapMaster_PathAttribute.clear();
}
bool CsMaster_CardMng::Init( char* cPath )
{
	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		_LoadExcel();
		_LoadExcelLeader();
		_LoadExcelLeaderAbility();
		_LoadExcelPath();
		_LoadExcelFilter();
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

CsMaster_Card*		CsMaster_CardMng::GetMasterCard( int nID )
{
	CsMaster_Card::MAP_IT it = m_mapMaster_Card.find( nID );
	if( it == m_mapMaster_Card.end() )
		return NULL;

	return it->second;
}

bool CsMaster_CardMng::IsMasterCardLeader( int nID ) 
{ 
	return ( m_mapMaster_CardLeader.find( nID ) != m_mapMaster_CardLeader.end() ); 
}

CsMaster_CardLeader*		CsMaster_CardMng::GetMasterCardLeader( int nID )
{
	CsMaster_CardLeader::MAP_IT it = m_mapMaster_CardLeader.find( nID );
	if( it == m_mapMaster_CardLeader.end() )
		return NULL;

	return it->second;
}

CsMaster_CardLeader::MAP* CsMaster_CardMng::GetMasterCardLeaderMap()
{
	return &m_mapMaster_CardLeader;
}

bool CsMaster_CardMng::IsMasterCardLeaderAbility( int nID )
{ 
	return ( m_mapMaster_CardLeaderAbility.find( nID ) != m_mapMaster_CardLeaderAbility.end() ); 
}

CsMaster_CardLeaderAbility*	CsMaster_CardMng::GetMasterCardLeaderAbility( int nID )
{
	CsMaster_CardLeaderAbility::MAP_IT it = m_mapMaster_CardLeaderAbility.find( nID );
	if( it == m_mapMaster_CardLeaderAbility.end() )
		return NULL;

	return it->second;
}


CsMaster_CardLeaderAbility::MAP* CsMaster_CardMng::GetMasterCardLeaderAbilityMap()
{
	return &m_mapMaster_CardLeaderAbility;
}

// 
// CsMaster_Path_Digimon*		CsMaster_CardMng::GetMasterCardPathDigimon( int nID )
// {
// 	CsMaster_Path_Digimon::MAP_IT it = m_mapMaster_PathDigimon.find( nID );
// 	if( it == m_mapMaster_PathDigimon.end() )
// 		return NULL;
// 
// 	return it->second;
// }
// 
// CsMaster_Path_Plate*		CsMaster_CardMng::GetMasterCardPathPlate( int nID )
// {
// 	CsMaster_Path_Plate::MAP_IT it = m_mapMaster_PathPlate.find( nID );
// 	if( it == m_mapMaster_PathPlate.end() )
// 		return NULL;
// 
// 	return it->second;
// }

// CsMaster_Frame*		CsMaster_CardMng::GetMasterFrame( int nID )
// {
// 	CsMaster_Frame::MAP_IT it = m_mapMaster_Frame.find( nID );
// 	if( it == m_mapMaster_Frame.end() )
// 		return NULL;
// 
// 	return it->second;
// }

void	CsMaster_CardMng::_LoadExcel()
{
	#define SHEETNAME_MASTERCARD			"Master_Seal"
	#define IL_COL_DI_NAME				1
	#define IL_COL_DI_NO				2
	#define IL_COL_ICON					3
	#define IL_COL_LEADER				4
	#define IL_COL_SCALE				5
	#define IL_COL_STARTGRADEINFO		6


	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
	case nsCsFileTable::ENGLISH:
	case nsCsFileTable::ENGLISH_A:
		bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), SHEETNAME_MASTERCARD );
		assert_cs( bSuccess == true );
		break;
	case nsCsFileTable::THAILAND:
	case nsCsFileTable::TAIWAN:		// 카드 마스터 태국도 추가
	case nsCsFileTable::HONGKONG:
		bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), SHEETNAME_MASTERCARD );
		assert_cs( bSuccess == true );
		break;
	default:
		assert_cs( false );
	}
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsMaster_Card::sINFO info;
	while( excel.MoveNext() == true )
	{


		info.s_nID = atoi( excel.GetData( 0, cBuffer ) );	
		if( info.s_nID == 0 )
			continue;	

		// Name
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( info.s_szName, excel.GetData( IL_COL_DI_NAME, cBuffer ), MAX_FILENAME );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info.s_szName, excel.GetData( IL_COL_DI_NAME, cBuffer ), MAX_FILENAME );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			//M2W( info.s_szName, excel.GetData( IL_COL_DI_NAME, cBuffer ), MAX_FILENAME );
			wcscpy_s(info.s_szName, MAX_FILENAME, excel.GetData(IL_COL_DI_NAME, wBuffer));
			break;
		default:
			assert_cs( false );
		}

		info.s_nDigimonID = atoi( excel.GetData( IL_COL_DI_NO, cBuffer ) );
		info.s_nIcon = atoi( excel.GetData( IL_COL_ICON, cBuffer ) );	
		info.s_nLeader = atoi( excel.GetData( IL_COL_LEADER, cBuffer ) );
		info.s_nScale = atoi( excel.GetData( IL_COL_SCALE, cBuffer ) );	


		int cnt = IL_COL_STARTGRADEINFO;
		int Seal_ItemIdx = atoi( excel.GetData( cnt++, cBuffer ) );
		assert_cs(Seal_ItemIdx != 0);

		for( int i = 0; i < CsMaster_Card::FT_CARD_MAX; ++i)
		{
			info.s_stGradeInfo[i].s_nItem = Seal_ItemIdx;
			//info.s_stGradeInfo[i].s_nItem = atoi( excel.GetData( cnt++, cBuffer ) );
			info.s_stGradeInfo[i].s_nIcon = atoi( excel.GetData( cnt++, cBuffer ) );
			info.s_stGradeInfo[i].s_nMax = atoi( excel.GetData( cnt++, cBuffer ) );
			//info.s_stGradeInfo[i].s_nIdentiQuantity = atoi( excel.GetData( cnt++, cBuffer ) );			

			info.s_stGradeInfo[i].s_nEff1 = atoi( excel.GetData( cnt++, cBuffer ) );
			info.s_stGradeInfo[i].s_nEff1val = atoi( excel.GetData( cnt++, cBuffer ) );
			info.s_stGradeInfo[i].s_nEff2 = atoi( excel.GetData( cnt++, cBuffer ) );
			info.s_stGradeInfo[i].s_nEff2val = atoi( excel.GetData( cnt++, cBuffer ) );	
		}

		CsMaster_Card* pCard = csnew CsMaster_Card;
		pCard->Init( &info );
		m_mapMaster_Card[ info.s_stGradeInfo[ CsMaster_Card::FT_CARD_NORMAL ].s_nItem ] = pCard;
	}
	excel.Close();
}


void	CsMaster_CardMng::_LoadExcelLeader()
{
#define SHEETNAME_MASTERCARDLEADER			"Master_Seal_Leader"

 	char cBuffer[ CEXCLE_CELL_LEN ];
// 	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
	case nsCsFileTable::ENGLISH:
	case nsCsFileTable::ENGLISH_A:
		bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), SHEETNAME_MASTERCARDLEADER );
		assert_cs( bSuccess == true );
		break;
	case nsCsFileTable::THAILAND:
	case nsCsFileTable::TAIWAN:		// 카드 마스터 태국도 추가
	case nsCsFileTable::HONGKONG:
		bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), SHEETNAME_MASTERCARDLEADER );
		assert_cs( bSuccess == true );
		break;
	default:
		assert_cs( false );
	}
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsMaster_CardLeader::sINFO info;
	while( excel.MoveNext() == true )
	{
		info.s_nID = atoi( excel.GetData( 0, cBuffer ) );	
		if( info.s_nID == 0 )
			continue;	

		info.s_nDigimonID = atoi( excel.GetData( 2, cBuffer ) );	
		info.s_nPetID = atoi( excel.GetData( 3, cBuffer ) );	
		info.s_nAni1 = atoi( excel.GetData( 4, cBuffer ) );	
		info.s_nAni2 = atoi( excel.GetData( 5, cBuffer ) );
		info.s_nSpecial1 = atoi( excel.GetData( 6, cBuffer ) );	
		info.s_nSpecial2 = atoi( excel.GetData( 7, cBuffer ) );	
		info.s_nAbil1 = atoi( excel.GetData( 8, cBuffer ) );	
		info.s_nAbil2 = atoi( excel.GetData( 9, cBuffer ) );
		info.s_nAbil3 = atoi( excel.GetData( 10, cBuffer ) );	
		info.s_nAbil4 = atoi( excel.GetData( 11, cBuffer ) );



		CsMaster_CardLeader* pLeader = csnew CsMaster_CardLeader;
		pLeader->Init( &info );
		m_mapMaster_CardLeader[ info.s_nID ] = pLeader;
	}
	excel.Close();
}


void	CsMaster_CardMng::_LoadExcelLeaderAbility()
{
#define SHEETNAME_MASTERCARDLEADERABILITY			"Master_Seal_Leader_Ability"

	char cBuffer[ CEXCLE_CELL_LEN ];
//	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
	case nsCsFileTable::ENGLISH:
	case nsCsFileTable::ENGLISH_A:
		bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), SHEETNAME_MASTERCARDLEADERABILITY );
		assert_cs( bSuccess == true );
		break;
	case nsCsFileTable::THAILAND:
	case nsCsFileTable::TAIWAN:		// 카드 마스터 태국도 추가
	case nsCsFileTable::HONGKONG:
		bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), SHEETNAME_MASTERCARDLEADERABILITY );
		assert_cs( bSuccess == true );
		break;
	default:
		assert_cs( false );
	}
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsMaster_CardLeaderAbility::sINFO info;
	while( excel.MoveNext() == true )
	{
		info.s_nID = atoi( excel.GetData( 0, cBuffer ) );	
		if( info.s_nID == 0 )
			continue;	

		int		nCnt = 2;
		for( int i =0; i < ABILITY_MAX; ++i )
		{
			info.s_stTerms[i].s_nterm = atoi( excel.GetData( nCnt++, cBuffer ) );
			info.s_stTerms[i].s_ntermval = atoi( excel.GetData( nCnt++, cBuffer ) );
			info.s_stTerms[i].s_nEff = atoi( excel.GetData( nCnt++, cBuffer ) );
		}
		


		CsMaster_CardLeaderAbility* pAbility = csnew CsMaster_CardLeaderAbility;
		pAbility->Init( &info );
		m_mapMaster_CardLeaderAbility[ info.s_nID ] = pAbility;
	}
	excel.Close();
}


void	CsMaster_CardMng::_LoadExcelPath()
{
	{
		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), "Path_Digimon" );
		assert_cs( bSuccess == true );

		if( bSuccess )
		{
			bSuccess = excel.MoveFirst();
			assert_cs( bSuccess == true );	
			bSuccess = excel.MoveNext();
			assert_cs( bSuccess == true );
			bSuccess = excel.MoveNext();
			assert_cs( bSuccess == true );

			while( excel.MoveNext() == true )
			{
				neSealMaster::sDigimonImgPath info;
				excel.Read_Value( 0, info.s_nDigimon );
				excel.Read_Value( 2, info.s_szDigimonSealImgPath );
				m_mapMaster_PathDigimon.insert( std::make_pair( info.s_nDigimon, info ) );			
			}
			excel.Close();
		}
	}

	{		
		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), "Path_Plate" );
		assert_cs( bSuccess == true );
		if( bSuccess )
		{
			bSuccess = excel.MoveFirst();
			assert_cs( bSuccess == true );	
			bSuccess = excel.MoveNext();
			assert_cs( bSuccess == true );
			bSuccess = excel.MoveNext();
			assert_cs( bSuccess == true );

			while( excel.MoveNext() == true )
			{
				neSealMaster::sPlatePath info;
				excel.Read_Value( 0, info.s_nID );
				excel.Read_Value( 1, info.s_szName );
				excel.Read_Value( 2, info.s_szNifFilePath );
				excel.Read_Value( 3, info.s_szGradeBackImagePath );
				m_mapMaster_PathPlate.insert( std::make_pair( info.s_nID, info ) );
			}
			excel.Close();
		}
	}

	{
		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), "NatureType" );
		assert_cs( bSuccess == true );

		if( bSuccess )
		{
			bSuccess = excel.MoveFirst();
			assert_cs( bSuccess == true );	
			bSuccess = excel.MoveNext();
			assert_cs( bSuccess == true );
			bSuccess = excel.MoveNext();
			assert_cs( bSuccess == true );

			while( excel.MoveNext() == true )
			{
				neSealMaster::sElemental info;
				excel.Read_Value( 1, info.s_nType );
				excel.Read_Value( 2, info.s_nFilePath );
				m_mapMaster_PathElemental.insert( std::make_pair( info.s_nType, info ) );			
			}
			excel.Close();
		}
	}

	{
		cExcelReader excel;
		bool bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), "AttributeType" );
		assert_cs( bSuccess == true );

		if( bSuccess )
		{
			bSuccess = excel.MoveFirst();
			assert_cs( bSuccess == true );	
			bSuccess = excel.MoveNext();
			assert_cs( bSuccess == true );
			bSuccess = excel.MoveNext();
			assert_cs( bSuccess == true );

			while( excel.MoveNext() == true )
			{
				neSealMaster::sAttribtue info;
				excel.Read_Value( 1, info.s_nType );
				excel.Read_Value( 2, info.s_nFilePath );
				m_mapMaster_PathAttribute.insert( std::make_pair( info.s_nType, info ) );			
			}
			excel.Close();
		}
	}
}

void CsMaster_CardMng::_LoadExcelFilter()
{
	#define SHEETNAME_MASTERCARD_FILTER		"Seal_Filter"
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_Master_Card(), SHEETNAME_MASTERCARD_FILTER );
	assert_cs( bSuccess == true );
	if( !bSuccess )
		return;

	// 1줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );
	while( excel.MoveNext() == true )
	{
		int index = 0;
		excel.Read_Value( 0, index );
		if (index == 0)
			break;

		CsMaster_Filter::sINFO info;
		excel.Read_Value( 1, info.s_nMapGroupID );
		excel.Read_Value( 4, info.s_nMapAreaID );
		excel.Read_Value( 7, info.s_nSealItemID );

		CsMaster_Filter::sINFO* pPath = csnew CsMaster_Filter::sINFO;
		pPath->SetInfo(info);
		InsertMasterFilter(pPath);
	}
	excel.Close();
}



#define MASTERCARD_BIN		"MasterCard.bin"
void CsMaster_CardMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MASTERCARD_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	{
		int nCount = (int)m_mapMaster_Card.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsMaster_Card::MAP_IT it = m_mapMaster_Card.begin();
		CsMaster_Card::MAP_IT itEnd = m_mapMaster_Card.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsMaster_Card::sINFO ), 1, fp );
		}
	}


	{
		int nCount = (int)m_mapMaster_CardLeader.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsMaster_CardLeader::MAP_IT it = m_mapMaster_CardLeader.begin();
		CsMaster_CardLeader::MAP_IT itEnd = m_mapMaster_CardLeader.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsMaster_CardLeader::sINFO ), 1, fp );
		}
	}

	{
		int nCount = (int)m_mapMaster_CardLeaderAbility.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		CsMaster_CardLeaderAbility::MAP_IT it = m_mapMaster_CardLeaderAbility.begin();
		CsMaster_CardLeaderAbility::MAP_IT itEnd = m_mapMaster_CardLeaderAbility.end();
		for( ; it!=itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof( CsMaster_CardLeaderAbility::sINFO ), 1, fp );
		}
	}


	{
		int nCount = (int)m_mapMaster_PathDigimon.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );

		MAP_SEALMASTER_DIGIIMGPATH_IT it = m_mapMaster_PathDigimon.begin();
		for( ; it!=m_mapMaster_PathDigimon.end(); ++it )
		{
			CmUtil::CmBinSave::Save_Value(fp, &(it->second.s_nDigimon));
			CmUtil::CmBinSave::Save_Value(fp, it->second.s_szDigimonSealImgPath);
		}
	}

	{
		int nCount = (int)m_mapMaster_PathPlate.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );

		MAP_SEALMASTER_PLATHPATH_IT it = m_mapMaster_PathPlate.begin();
		for( ; it!=m_mapMaster_PathPlate.end(); ++it )
		{
			CmUtil::CmBinSave::Save_Value(fp, &(it->second.s_nID));
			CmUtil::CmBinSave::Save_Value(fp, it->second.s_szName);
			CmUtil::CmBinSave::Save_Value(fp, it->second.s_szNifFilePath);
			CmUtil::CmBinSave::Save_Value(fp, it->second.s_szGradeBackImagePath);
		}
	}

	{
		int nCount = (int)m_mapMaster_PathElemental.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );

		MAP_SEALMASTER_ELEMENTAL_IT it = m_mapMaster_PathElemental.begin();
		for( ; it!=m_mapMaster_PathElemental.end(); ++it )
		{
			CmUtil::CmBinSave::Save_Value(fp, &(it->second.s_nType));
			CmUtil::CmBinSave::Save_Value(fp, it->second.s_nFilePath);
		}
	}

	{
		int nCount = (int)m_mapMaster_PathAttribute.size();
		CmUtil::CmBinSave::Save_Value( fp, &nCount );

		MAP_SEALMASTER_ATTRIBUTE_IT it = m_mapMaster_PathAttribute.begin();
		for( ; it!=m_mapMaster_PathAttribute.end(); ++it )
		{
			CmUtil::CmBinSave::Save_Value(fp, &(it->second.s_nType));
			CmUtil::CmBinSave::Save_Value(fp, it->second.s_nFilePath);
		}
	}

	SaveBinFilter(fp);
	fclose( fp );
}

bool	CsMaster_CardMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MASTERCARD_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsMaster_Card::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsMaster_Card::sINFO ), 1, fp );

			CsMaster_Card* pObject = csnew CsMaster_Card;
			pObject->Init( &info );
			assert_cs( m_mapMaster_Card.find( info.s_nDigimonID ) == m_mapMaster_Card.end() );
			m_mapMaster_Card[ info.s_stGradeInfo[ CsMaster_Card::FT_CARD_NORMAL ].s_nItem ] = pObject;
		}
	}


	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsMaster_CardLeader::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsMaster_CardLeader::sINFO ), 1, fp );

			CsMaster_CardLeader* pObject = csnew CsMaster_CardLeader;
			pObject->Init( &info );
			assert_cs( m_mapMaster_CardLeader.find( info.s_nID ) == m_mapMaster_CardLeader.end() );
			m_mapMaster_CardLeader[ info.s_nID ] = pObject;
		}
	}

	{
		int nCount;
		fread( &nCount, sizeof( int ), 1, fp );

		CsMaster_CardLeaderAbility::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsMaster_CardLeaderAbility::sINFO ), 1, fp );

			CsMaster_CardLeaderAbility* pObject = csnew CsMaster_CardLeaderAbility;
			pObject->Init( &info );
			assert_cs( m_mapMaster_CardLeaderAbility.find( info.s_nID ) == m_mapMaster_CardLeaderAbility.end() );
			m_mapMaster_CardLeaderAbility[ info.s_nID ] = pObject;
		}
	}

	{
		int nCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			neSealMaster::sDigimonImgPath info;
			CmUtil::CmBinRead::Read_Value( fp, &info.s_nDigimon );
			CmUtil::CmBinRead::Read_Value( fp, info.s_szDigimonSealImgPath );
			m_mapMaster_PathDigimon.insert(std::make_pair(info.s_nDigimon, info));
		}
	}

	{
		int nCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			neSealMaster::sPlatePath info;
			CmUtil::CmBinRead::Read_Value( fp, &info.s_nID );
			CmUtil::CmBinRead::Read_Value( fp, info.s_szName );
			CmUtil::CmBinRead::Read_Value( fp, info.s_szNifFilePath );
			CmUtil::CmBinRead::Read_Value( fp, info.s_szGradeBackImagePath );
			m_mapMaster_PathPlate.insert(std::make_pair(info.s_nID, info));
		}
	}

	{
		int nCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			neSealMaster::sElemental info;
			CmUtil::CmBinRead::Read_Value( fp, &info.s_nType );
			CmUtil::CmBinRead::Read_Value( fp, info.s_nFilePath );
			m_mapMaster_PathElemental.insert(std::make_pair(info.s_nType, info));
		}
	}

	{
		int nCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			neSealMaster::sAttribtue info;
			CmUtil::CmBinRead::Read_Value( fp, &info.s_nType );
			CmUtil::CmBinRead::Read_Value( fp, info.s_nFilePath );
			m_mapMaster_PathAttribute.insert(std::make_pair(info.s_nType, info));
		}
	}
	   
	LoadBinFilter(fp);

	fclose( fp );
	return true;
}
void	CsMaster_CardMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, MASTERCARD_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );

		CsMaster_Card::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsMaster_Card::sINFO ) );

			CsMaster_Card* pObject = csnew CsMaster_Card;
			pObject->Init( &info );
			assert_cs( m_mapMaster_Card.find( info.s_nDigimonID ) == m_mapMaster_Card.end() );
			m_mapMaster_Card[ info.s_stGradeInfo[ CsMaster_Card::FT_CARD_NORMAL ].s_nItem ] = pObject;
		}
	}

	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );

		CsMaster_CardLeader::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsMaster_CardLeader::sINFO ) );

			CsMaster_CardLeader* pObject = csnew CsMaster_CardLeader;
			pObject->Init( &info );
			assert_cs( m_mapMaster_CardLeader.find( info.s_nID ) == m_mapMaster_CardLeader.end() );
			m_mapMaster_CardLeader[ info.s_nID ] = pObject;
		}
	}


	{
		int nCount;
		_read( nHandle, &nCount, sizeof( int ) );

		CsMaster_CardLeaderAbility::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsMaster_CardLeaderAbility::sINFO ) );

			CsMaster_CardLeaderAbility* pObject = csnew CsMaster_CardLeaderAbility;
			pObject->Init( &info );
			assert_cs( m_mapMaster_CardLeaderAbility.find( info.s_nID ) == m_mapMaster_CardLeaderAbility.end() );
			m_mapMaster_CardLeaderAbility[ info.s_nID ] = pObject;
		}
	}


	{
		int nCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			neSealMaster::sDigimonImgPath info;
			CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nDigimon );
			CmUtil::CmPackRead::Read_Value( nHandle, info.s_szDigimonSealImgPath );
			m_mapMaster_PathDigimon.insert(std::make_pair(info.s_nDigimon, info));
		}
	}

	{
		int nCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			neSealMaster::sPlatePath info;
			CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nID );
			CmUtil::CmPackRead::Read_Value( nHandle, info.s_szName );
			CmUtil::CmPackRead::Read_Value( nHandle, info.s_szNifFilePath );
			CmUtil::CmPackRead::Read_Value( nHandle, info.s_szGradeBackImagePath );
			m_mapMaster_PathPlate.insert(std::make_pair(info.s_nID, info));
		}
	}

	{
		int nCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			neSealMaster::sElemental info;
			CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nType );
			CmUtil::CmPackRead::Read_Value( nHandle, info.s_nFilePath );
			m_mapMaster_PathElemental.insert(std::make_pair(info.s_nType, info));
		}
	}

	{
		int nCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
		for( int i=0; i<nCount; ++i )
		{
			neSealMaster::sAttribtue info;
			CmUtil::CmPackRead::Read_Value( nHandle, &info.s_nType );
			CmUtil::CmPackRead::Read_Value( nHandle, info.s_nFilePath );
			m_mapMaster_PathAttribute.insert(std::make_pair(info.s_nType, info));
		}
	}

	_LoadFilePackFilter(nHandle);

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}
bool CsMaster_CardMng::IsMasterCard( int nID )
{
	return ( m_mapMaster_Card.find( nID ) != m_mapMaster_Card.end() );
}
bool CsMaster_CardMng::IsMasterCardbyMonsterID( int nID )
{
	for( CsMaster_Card::MAP_IT itr = m_mapMaster_Card.begin(); itr != m_mapMaster_Card.end(); ++itr)
	{
		if(itr->second->GetInfo()->s_nDigimonID == nID)
			return true;
	}
	return false;
}

CsMaster_Card* CsMaster_CardMng::GetMasterCardbyMonsterID( int nID )
{
	for( CsMaster_Card::MAP_IT itr = m_mapMaster_Card.begin(); itr != m_mapMaster_Card.end(); ++itr)
	{
		if(itr->second->GetInfo()->s_nDigimonID == nID)
			return (*itr).second;
	}
	return NULL;
}

CsMaster_Card::MAP* CsMaster_CardMng::GetMasterCardMap()
{
	return &m_mapMaster_Card;
}

CsMaster_Filter::sINFO* CsMaster_CardMng::GetMasterFilter( DWORD mapGroupID, DWORD mapID, DWORD sealID )
{
	assert_cs( IsMasterFilter( mapGroupID, mapID, sealID ) == true );
	CsMaster_Filter::MapGroup_IT mapGroupItr = m_mapMaster_Filter.find(mapGroupID);
	if( mapGroupItr != m_mapMaster_Filter.end() )
	{
		CsMaster_Filter::Map_IT mapItr = mapGroupItr->second->s_sMap.find(mapID);
		if ( mapItr != mapGroupItr->second->s_sMap.end() )
		{
			CsMaster_Filter::Seal_IT sealItr = mapItr->second->s_sSeal.find(sealID);
			if ( sealItr != mapItr->second->s_sSeal.end() )
				return sealItr->second;
		}
	}
	return NULL;
}

CsMaster_Filter::MapGroup* CsMaster_CardMng::GetMasterFilterMap()
{
	return &m_mapMaster_Filter;
}

void CsMaster_CardMng::SaveBinFilter(FILE* fp)
{
	int nCount = GetMasterFilterSize();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsMaster_Filter::MapGroup_IT mapGroupItr = m_mapMaster_Filter.begin();
	CsMaster_Filter::MapGroup_IT mapGroupItrEnd = m_mapMaster_Filter.end();
	for( ; mapGroupItr!=mapGroupItrEnd; ++mapGroupItr )
	{
		CsMaster_Filter::Map_IT mapItr = mapGroupItr->second->s_sMap.begin();
		CsMaster_Filter::Map_IT mapItrEnd = mapGroupItr->second->s_sMap.end();
		for( ; mapItr!=mapItrEnd; ++mapItr )
		{
			CsMaster_Filter::Seal_IT sealItr = mapItr->second->s_sSeal.begin();
			CsMaster_Filter::Seal_IT sealItrEnd = mapItr->second->s_sSeal.end();
			for( ; sealItr!=sealItrEnd; ++sealItr )
			{
				fwrite( &(sealItr->second->s_nMapGroupID), sizeof( DWORD ), 1, fp );
				fwrite( &(sealItr->second->s_nMapAreaID), sizeof( DWORD ), 1, fp );
				fwrite( &(sealItr->second->s_nSealItemID), sizeof( DWORD ), 1, fp );
			}
		}
	}
}
void CsMaster_CardMng::LoadBinFilter(FILE* fp)
{
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );


	for( int i=0; i<nCount; ++i )
	{
		CsMaster_Filter::sINFO* pObject = csnew CsMaster_Filter::sINFO;
		fread( &(pObject->s_nMapGroupID), sizeof(DWORD), 1, fp );
		fread( &(pObject->s_nMapAreaID), sizeof(DWORD), 1, fp );
		fread( &(pObject->s_nSealItemID), sizeof(DWORD), 1, fp );
		int group = pObject->s_nMapGroupID;
		int map = pObject->s_nMapAreaID;
		int seal = pObject->s_nSealItemID;

		InsertMasterFilter(pObject);
	}
}
void CsMaster_CardMng::_LoadFilePackFilter(int nHandle)
{
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	
	for( int i=0; i<nCount; ++i )
	{
		CsMaster_Filter::sINFO* pObject = csnew CsMaster_Filter::sINFO;
		_read( nHandle, &(pObject->s_nMapGroupID), sizeof(DWORD) );
		_read( nHandle, &(pObject->s_nMapAreaID), sizeof(DWORD) );
		_read( nHandle, &(pObject->s_nSealItemID), sizeof(DWORD) );

		InsertMasterFilter(pObject);
	}

}

void CsMaster_Filter::sINFO::SetInfo(CsMaster_Filter::sINFO& src)
{
	s_nMapGroupID = src.s_nMapGroupID;
	s_nMapAreaID = src.s_nMapAreaID;
	s_nSealItemID = src.s_nSealItemID;
}

bool CsMaster_CardMng::IsMasterFilter( DWORD mapGroupID, DWORD mapID, DWORD sealID )
{
	CsMaster_Filter::MapGroup_IT mapGroupItr = m_mapMaster_Filter.find(mapGroupID);
	if( mapGroupItr != m_mapMaster_Filter.end() )
	{
		CsMaster_Filter::Map_IT mapItr = mapGroupItr->second->s_sMap.find(mapID);
		CsMaster_Filter::Map_IT mapItrEnd = mapGroupItr->second->s_sMap.end();
		if ( mapItr != mapItrEnd )
		{
			CsMaster_Filter::Seal_IT sealItr = mapItr->second->s_sSeal.find(sealID);
			CsMaster_Filter::Seal_IT sealItrEnd = mapItr->second->s_sSeal.end();
			if ( sealItr != sealItrEnd )
				return true;
		}
	}
	return false;
}
bool CsMaster_CardMng::InsertMasterFilter( CsMaster_Filter::sINFO* pPath )
{
	DWORD nMapGroupID = pPath->s_nMapGroupID;
	DWORD nMapID = pPath->s_nMapAreaID;
	DWORD nSealID = pPath->s_nSealItemID;
	if ( IsMasterFilter(nMapGroupID, nMapID, nSealID) == true )
		return false;
	CsMaster_Filter::MapGroup_IT mapGroupItr = m_mapMaster_Filter.find(nMapGroupID);
	if( mapGroupItr != m_mapMaster_Filter.end() )
	{
		CsMaster_Filter::Map_IT mapItr = mapGroupItr->second->s_sMap.find(nMapID);
		CsMaster_Filter::Map_IT mapItrEnd = mapGroupItr->second->s_sMap.end();
		if ( mapItr != mapItrEnd )
		{
			std::pair<DWORD, CsMaster_Filter::sINFO*> sealpair(nSealID, pPath);
			mapItr->second->s_sSeal.insert(sealpair);
			return true;
		}
		else
		{
			CsMaster_Filter::sMap* newMap = csnew CsMaster_Filter::sMap;
			newMap->s_nMapID = nMapID;
		
			std::pair<DWORD, CsMaster_Filter::sINFO*> sealPair(nSealID, pPath);
			newMap->s_sSeal.insert(sealPair);
		
			std::pair<DWORD, CsMaster_Filter::sMap*> mapPair(nMapID, newMap);
			mapGroupItr->second->s_sMap.insert(mapPair);
			return true;
		}
	}
	else
	{
		CsMaster_Filter::sMapGroup* newGroup = csnew CsMaster_Filter::sMapGroup;
		newGroup->s_nMapGroupID = nMapGroupID;
		
		CsMaster_Filter::sMap* newMap = csnew CsMaster_Filter::sMap;
		newMap->s_nMapID = nMapID;
		
		std::pair<DWORD, CsMaster_Filter::sINFO*> sealPair(nSealID, pPath);
		newMap->s_sSeal.insert(sealPair);

		std::pair<DWORD, CsMaster_Filter::sMap*> mapPair(nMapID, newMap);
		newGroup->s_sMap.insert(mapPair);

		std::pair<DWORD, CsMaster_Filter::sMapGroup*> groupPair(nMapGroupID, newGroup);
		m_mapMaster_Filter.insert(groupPair);
		return true;
	}
	return false;
}

int CsMaster_CardMng::GetMasterFilterSize()
{
	int count = 0;
	CsMaster_Filter::MapGroup_IT mapGroupItr = m_mapMaster_Filter.begin();
	CsMaster_Filter::MapGroup_IT mapGroupItrEnd = m_mapMaster_Filter.end();
	for( ; mapGroupItr!=mapGroupItrEnd; ++mapGroupItr )
	{
		CsMaster_Filter::Map_IT mapItr = mapGroupItr->second->s_sMap.begin();
		CsMaster_Filter::Map_IT mapItrEnd = mapGroupItr->second->s_sMap.end();
		for( ; mapItr!=mapItrEnd; ++mapItr )
			count += (int)mapItr->second->s_sSeal.size();
	}
	return count;
}


std::string CsMaster_CardMng::GetSealDigimonImgPath(DWORD const& dwDigimonID) const
{
	MAP_SEALMASTER_DIGIIMGPATH_CIT it = m_mapMaster_PathDigimon.find( dwDigimonID );
	if( it == m_mapMaster_PathDigimon.end() )
		return std::string();

	return it->second.s_szDigimonSealImgPath;
}	

std::string CsMaster_CardMng::GetSealGridNifFile(int const& nGridType) const
{
	int nType = nGridType == -1 ? 0 : nGridType;

	MAP_SEALMASTER_PLATHPATH_CIT it = m_mapMaster_PathPlate.find( nType );
	if( it == m_mapMaster_PathPlate.end() )
		return std::string();

	return it->second.s_szNifFilePath;
}

std::string CsMaster_CardMng::GetSealGridBackFile(int const& nGridType) const
{
	int nType = nGridType == -1 ? 0 : nGridType;
	MAP_SEALMASTER_PLATHPATH_CIT it = m_mapMaster_PathPlate.find( nType );
	if( it == m_mapMaster_PathPlate.end() )
		return std::string();

	return it->second.s_szGradeBackImagePath;
}

std::string CsMaster_CardMng::GetSealElementalImgPath(int const& nElType) const
{
	MAP_SEALMASTER_ELEMENTAL_CIT it = m_mapMaster_PathElemental.find( nElType );
	if( it == m_mapMaster_PathElemental.end() )
		return std::string();

	return it->second.s_nFilePath;
}

std::string CsMaster_CardMng::GetSealAttributeImgPath(int const& nAttriType) const
{
	MAP_SEALMASTER_ATTRIBUTE_CIT it = m_mapMaster_PathAttribute.find( nAttriType );
	if( it == m_mapMaster_PathAttribute.end() )
		return std::string();

	return it->second.s_nFilePath;
}
