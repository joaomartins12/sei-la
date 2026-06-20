
#include "stdafx.h"
#include "ItemMng.h"
#include "../CsFunc/CrashLogger.h"
#include <io.h>

CsItemMng::CsItemMng()
{

}

CsItemMng::~CsItemMng()
{

}


void CsItemMng::Delete()
{
	{
		CsItem::MAP_IT it = m_Map.begin();
		for( ; it != m_Map.end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
			it->second->Delete();
			delete it->second;
		}
		m_Map.clear();
	}

	{
		CsItemTap::MAP_IT it = m_ItemCategory.begin();
		for( ; it != m_ItemCategory.end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
			it->second->Delete();
			delete it->second;
		}
		m_ItemCategory.clear();
	}

	{
		CsItemCoolTime::MAP_IT it = m_mapCoolTime.begin();
		for( ; it != m_mapCoolTime.end(); ++it )
		{
			SAFE_POINTER_CON( it->second );
			it->second->Delete();
			delete it->second;
		}
		m_mapCoolTime.clear();
	}

	{	// Rank테이블 load관련 추가	lks007
		CsItemRank::MAP_IT it = m_mapRank.begin();
		for( ; it != m_mapRank.end(); ++it )
			delete it->second;
		m_mapRank.clear();
	}

	m_MapDisp.clear();
	m_MapTypeName.clear();	// Data 시트 소분류 타입 설명에서 스트링 얻어옴 chu8820
	m_mapProductionItem.clear();
	m_XaiItem_Data.clear();
	m_mapAssistItemGroup.clear();
	m_mapRankEffect.clear();
	m_DPartItemTables.clear();
}

bool CsItemMng::Init( char* cPath )
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

void CsItemMng::_LoadExcel()
{	
	_LoadCoolTime( nsCsFileTable::GetPath_DM_Item() );
	_LoadExcel( nsCsFileTable::GetPath_DM_Item() );
	_LoadExcel( nsCsFileTable::GetPath_DM_QuestItem() );
	_LoadCatagory( nsCsFileTable::GetPath_DM_Item() );	
	_LoadData( nsCsFileTable::GetPath_DM_Item() );
	_LoadRank( nsCsFileTable::GetPath_DM_Item() );	// Rank테이블 load관련 추가	lks007

	_LoadAttributeData( nsCsFileTable::GetPath_DM_Item() );
	_LoadExchange( nsCsFileTable::GetPath_DM_Item() );		//물물교환상점
	_LoadAccessory( nsCsFileTable::GetPath_DM_Item() );	//악세서리

	_LoadItemProductionTable( nsCsFileTable::GetPath_DM_Item() );	// 아이템 제작 테이블
	_LoadItemProductionAssistTable( nsCsFileTable::GetPath_DM_Item() ); // 아이템 제작시 보조 아이템 테이블
	_LoadItemXAI_SystemTable( nsCsFileTable::GetPath_DM_Item() );	
	
	_LoadRankEffect( nsCsFileTable::GetPath_DM_Item() );
	_LoadDigimonPartsItem(nsCsFileTable::GetPath_DM_Item());
}

void CsItemMng::ReloadName()
{
	_ReloadName( nsCsFileTable::GetPath_DM_Item() );
	_ReloadName( nsCsFileTable::GetPath_DM_QuestItem() );
}

void CsItemMng::_LoadExcel( char* cExcel )
{
#define ITEM_SHEET_NAME					"Item"
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;
	bool bSuccess = excel.Open( cExcel, ITEM_SHEET_NAME );
	if( !bSuccess )
	{
		CsMessageBoxA( MB_OK, "%s 시트를 열지 못하였습니다.", ITEM_SHEET_NAME );
		return;
	}

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
	{
		CsMessageBoxA( MB_OK, "%s 시트를 열지 못하였습니다.", ITEM_SHEET_NAME );
		return;
	}

	while( excel.MoveNext() == true )
	{
		// ID
		DWORD dwItemID = 0;
		excel.Read_Value( 1, dwItemID );
		
		if( dwItemID == 0 )
			continue;

		CsItem::MAP_IT it = m_Map.find( dwItemID );
		if( it != m_Map.end() )
		{
			assert_csm1( false, _T( "아이템테이블 :  이미 존재하는 ID = %d" ), dwItemID );
			continue;
		}

		CsItem::sINFO info;
		info.s_dwItemID = dwItemID;

		// name
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:	excel.Read_String( 2, info.s_szName, sizeof( info.s_szName ) );	break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:			excel.Read_String( 3, info.s_szName, sizeof( info.s_szName ) );	break;
			break;
		}

		__CsStringChange::NewLineCheck( info.s_szName );

		
		excel.Read_Value( 4, info.s_nIcon );						// icon
		excel.Read_String( 5, info.s_cNif, sizeof( info.s_cNif ) );	// Nif		
		excel.Read_Value( 6, info.s_nSkillCodeType );				// 기능 코드 타입 - 1: 스킬코드, 2: 옵션코드
		excel.Read_Value( 7, info.s_dwSkill );						// Skill
		excel.Read_Value( 9, info.s_nSocketCount );					// 소켓 갯수
		excel.Read_Value( 10, info.s_btApplyRateMin );				// ApplyRate
		excel.Read_Value( 11, info.s_btApplyRateMax );
		excel.Read_Value( 12, info.s_btApplyElement );
		excel.Read_Value( 13, info.s_nClass );						// Class
		
		// type comment
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			excel.Read_String( 15, info.s_szTypeComment, sizeof( info.s_szTypeComment ) );// type comment
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			excel.Read_String( 16, info.s_szTypeComment, sizeof( info.s_szTypeComment ) );// type comment
			break;
		}

		int nType = 0;
		excel.Read_Value( 17, nType );						// Type
		if( 0 == nType )
		{// TYPE_OLD
			excel.Read_Value( 21, nType );						// Type
			nType = nType * 100;
		}

		excel.Read_Value( 19, info.s_nTypeValue );
		excel.Read_Value( 20, info.s_btCodeTag );						// Tag

#ifdef DEBUFF_ITEM
		info.s_nSection = nType;
#endif
		info.s_nType_L = nType/100;
		info.s_nType_S = nType%100;

		switch( info.s_nType_L )
		{
		case 120:
		case 121:
		case 122:
		case 123:
		case 124:
		case 125:
			assert_csm1( info.s_dwItemID <= 60000, _T( "속성 아이템은 ID가 60000 이 넘으면 안되요 ID = %d" ), info.s_dwItemID );
			break;
		}
		if( strcmp( info.s_cNif,"" ) == 0 )
		{
			switch( info.s_nType_L )
			{
			case 21:/*Head*/	strcpy_s( info.s_cNif, ITEM_FILE_LEN, "Head" );			break;
			case 22:/*Coat*/	strcpy_s( info.s_cNif, ITEM_FILE_LEN, "Up" );			break;
			case 23:/*Glove*/	strcpy_s( info.s_cNif, ITEM_FILE_LEN, "Hand" );			break;
			case 24:/*Pants*/	strcpy_s( info.s_cNif, ITEM_FILE_LEN, "Down" );			break;
			case 25:/*Shoes*/	strcpy_s( info.s_cNif, ITEM_FILE_LEN, "Shoes" );		break;
			case 26:/*Costume*/	strcpy_s( info.s_cNif, ITEM_FILE_LEN, "NoneCostume" );	break;
			case 27:/*Glass*/	strcpy_s( info.s_cNif, ITEM_FILE_LEN, "Head" );			break;
			default:			break;
			}
		}

		excel.Read_Value( 22, info.s_nSellType );						// Sell Type
		excel.Read_Value( 24, info.s_nUseMode );						// UseMode

		// 
		excel.Read_Value( 26, info.s_nUseTimeGroup );					// UseTimeGroup
		if( info.s_nUseTimeGroup != 0 )
			assert_csm1( IsCoolTime( info.s_nUseTimeGroup ) == true, _T( "정의된 타임 그룹이 Time_Group 쉬트에 존재하지 않습니다. ItemID = %d" ), info.s_dwItemID );

		excel.Read_Value( 28, info.s_nOverlap );					// Overlap
		switch( info.s_nType_L )
		{
		case 21:/*Head*/
		case 22:/*Coat*/
		case 23:/*Glove*/
		case 24:/*Pants*/
		case 25:/*Shoes*/
		case 26:/*Costume*/
		case 27:/*Glass*/
		case 28:/*Necklace*/
		case 29:/*Ring*/
		case 30:/*Earring*/
		case 31:/*EquipAura*/
		case 32:/*XAI*/
			{
				assert_csm2( info.s_nOverlap == 1, _T( "장비 아이템의 오버랩 갯수가 1이 아닙니다. ItemID = %d, Item Name = %s" ), info.s_dwItemID, info.s_szName );
			}
			break;
		}
	
		excel.Read_Value( 29, info.s_nTamerReqMinLevel );		// Tamer Requirement Min Level
		excel.Read_Value( 30, info.s_nTamerReqMaxLevel );		// Tamer Requirement Max Level
		excel.Read_Value( 31, info.s_nDigimonReqMinLevel );		// Digimon Requirement Min Level
		excel.Read_Value( 32, info.s_nDigimonReqMaxLevel );		// Digimon Requirement Max Level


		excel.Read_Value( 33, info.s_nPossess );				// Possess
		excel.Read_Value( 34, info.s_nEquipSeries );			// EquipSeries
		excel.Read_Value( 35, info.s_nUseCharacter );			// Use Character
		excel.Read_Value( 37, info.s_nDrop );					// Drop
		excel.Read_Value( 38, info.s_dwDigiCorePrice );			// DigiCore
		excel.Read_Value( 39, info.s_nEventItemType );			// Event Sell
		excel.Read_Value( 40, info.s_dwEventItemPrice );		// Event Sell Price
		excel.Read_Value( 41, info.s_dwScanPrice );				// StanPrice
		excel.Read_Value( 42, info.s_dwSale );					// Sale
		if( info.s_dwSale == 0 && info.s_dwItemID / 10000 != 9 )
			info.s_dwSale = (DWORD)( info.s_dwScanPrice*0.1f );
		
		// AA - ModelID - 없는 놈도 있다
		excel.Read_String( 43, info.s_cModel_Nif, sizeof(info.s_cModel_Nif) );			// Model Nif
		excel.Read_String( 44, info.s_cModel_Effect, sizeof(info.s_cModel_Effect) );	// Model Effect Nif
		excel.Read_Value( 45, info.s_bModel_Loop );				// Model Loop 
		excel.Read_Value( 46, info.s_bModel_Shader );			// Model Shader
		excel.Read_Value( 47, info.s_dwSoundID );				// Sound

		excel.Read_String( 48, info.s_szComment, sizeof(info.s_szComment) );
		__CsStringChange::NewLineCheck( info.s_szComment );
				
		excel.Read_Value( 49, info.s_nBelonging );				// 귀속

		// 관련 퀘스트
		excel.Read_Value( 52, info.s_nQuest1 );
		excel.Read_Value( 53, info.s_nQuest2 );
		excel.Read_Value( 54, info.s_nQuest3 );
		excel.Read_Value( 55, info.s_nQuestRequire );			// 퀘스트 사용 제한
		excel.Read_Value( 56, info.s_nDigiviceSkillSlot );		// 디지바이스 아이템 스킬 슬롯
		excel.Read_Value( 57, info.s_nDigiviceChipsetSlot );	// 디지바이스 아이템 칩셋 슬롯
		excel.Read_Value( 58, info.s_btUseTimeType );	// 사용시간적용타입
		excel.Read_Value( 59, info.s_nUseTime_Min );	// 사용시간적용시간 ( 분 )

#ifdef ITEM_USE_TIME_PASS_FILE
		excel.Read_Value( 60, info.s_bUseTimeType );	// 사용시간적용시간 ( 분 )
		excel.Read_Value( 61, info.s_nUseBattle );		// 배틀시 사용가능한지
		excel.Read_Value( 62, info.s_nDoNotUseType );
#else
		excel.Read_Value( 60, info.s_nUseBattle );	// 사용시간적용시간 ( 분 )
		excel.Read_Value( 61, info.s_nDoNotUseType );	// 사용시간적용시간 ( 분 )
#endif

		CsItem* pItem = csnew CsItem;
		pItem->Init( &info );

		m_Map.insert( std::make_pair( info.s_dwItemID, pItem ) );

		// 조건 체크		
#ifndef _GIVE
		if( ( info.s_nType_L != 80 )&&( info.s_nType_L != 86 ) )
		{
			if( ( info.s_nOverlap > 1 )&&( info.s_btCodeTag != 3 ) )
			{			
				// - 오버랩 갯수가 1 이상일때 랜덤값적용은 없다
				assert_csm2( info.s_btApplyRateMin == info.s_btApplyRateMax, _T( "ID = %d, 오버랩 갯수(%d)가 1 이상일때 랜덤값적용은 없다" ), info.s_dwItemID, info.s_nOverlap );
				// - 오버랩 갯수가 1 이상일때 귀속 타입이 변할 수 없다 ( 겹칠 수 없기 때문에 장착시 귀속은 안됨 )
//				assert_csm1( info.s_nBelonging != 1, _T( "ID = %d, 오버랩 갯수가 1 이상일때 겹칠 수 없기 때문에 장착시 귀속은 안됨" ), info.s_dwItemID );
			}
		}		
#endif		
	}     

	excel.Close();	
}

//물물교환상점
void CsItemMng::_LoadExchange( char* cExcel )
{
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;	
	bool bSuccess = excel.Open( cExcel, "Item_Exchange" );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];

	CsItemExchange::sINFO sExchange;

	while(excel.MoveNext() == true)
	{
		if( atoi( excel.GetData(0, cBuffer) ) == 0 )
			break;
		sExchange.s_dwCount = 0;

		sExchange.s_dwNpcID				= atoi( excel.GetData(0, cBuffer) );
		sExchange.s_dwItemIndex			= atoi( excel.GetData(1, cBuffer) );
		sExchange.s_dwItemID			= atoi( excel.GetData(2, cBuffer) );
		sExchange.s_dwExchange_Code_A	= atoi( excel.GetData(3, cBuffer) );
		if( sExchange.s_dwExchange_Code_A != 0 )
			sExchange.s_dwCount++;
		sExchange.s_dwPropertyA_Price	= atoi( excel.GetData(4, cBuffer) );

		sExchange.s_dwExchange_Code_B	= atoi( excel.GetData(5, cBuffer) );
		if( sExchange.s_dwExchange_Code_B != 0 )
			sExchange.s_dwCount++;
		sExchange.s_dwPropertyB_Price	= atoi( excel.GetData(6, cBuffer) );

		sExchange.s_dwExchange_Code_C	= atoi( excel.GetData(7, cBuffer) );
		if( sExchange.s_dwExchange_Code_C != 0 )
			sExchange.s_dwCount++;
		sExchange.s_dwPropertyC_Price	= atoi( excel.GetData(8, cBuffer) );

		sExchange.s_dwExchange_Code_D	= atoi( excel.GetData(9, cBuffer) );
		if( sExchange.s_dwExchange_Code_D != 0 )
			sExchange.s_dwCount++;
		sExchange.s_dwPropertyD_Price	= atoi( excel.GetData(10, cBuffer) );

		CsItemExchange* pExchange = csnew CsItemExchange;
		pExchange->Init(&sExchange);
		m_mapExchange[sExchange.s_dwItemIndex] = pExchange;
	}

	excel.Close();
}

void CsItemMng::_LoadCatagory( char* cExcel )
{
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;	
	bool bSuccess = excel.Open( cExcel, "Data" );
	assert_cs( bSuccess == true );	

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	CsItemTap::sINFO infoCategory;
	while( excel.MoveNext() == true )
	{
		// ID
		infoCategory.s_nSellClass = atoi( excel.GetData( 25, cBuffer ) );
		if( infoCategory.s_nSellClass == 0 )
			continue;

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( infoCategory.s_szName, excel.GetData( 26, cBuffer ), 32 );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			//M2W( info.s_szTypeComment, excel.GetData( IL_COL_TYPE_COMMENT_ENG, cBuffer ), ITEM_NAME_LEN );
			wcscpy_s( infoCategory.s_szName, 32, excel.GetData(26, wBuffer) );
			break;
		default:
			assert_cs( false );
		}
		//M2W( infoCategory.s_szName, excel.GetData( 26, cBuffer ), 32 );

		CsItemTap* pTap = csnew CsItemTap;
		pTap->Init( &infoCategory );
		m_ItemCategory[ infoCategory.s_nSellClass ] = pTap;
	}

	excel.Close();
}

void CsItemMng::_LoadCoolTime( char* cExcel )
{
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;	
	bool bSuccess = excel.Open( cExcel, "Time_Group" );
	assert_cs( bSuccess == true );	

	// 3줄은 버린다.
	bSuccess = excel.MoveFirst();
	bSuccess = excel.MoveNext();
	bSuccess = excel.MoveNext();

	char cBuffer[ CEXCLE_CELL_LEN ];

	CsItemCoolTime::sINFO info;
	while( excel.MoveNext() == true )
	{
		info.s_nGroupID = atoi( excel.GetData( 1, cBuffer ) );
		if( info.s_nGroupID == 0 )
			continue;

		int nSec = atoi( excel.GetData( 3, cBuffer ) );
		if( nSec != 0 )
		{
			info.s_bNetwork = false;
			info.s_fTimeSec = (float)nSec;
		}
		else
		{
			info.s_bNetwork = true;
			info.s_fTimeSec = atoi( excel.GetData( 4, cBuffer ) ) + atoi( excel.GetData( 5, cBuffer ) )*60 + atoi( excel.GetData( 6, cBuffer ) )*60*60;
		}

		if( info.s_fTimeSec == 0 )
			continue;

		CsItemCoolTime* pData = csnew CsItemCoolTime;
		pData->Init( &info );
		assert_cs( m_mapCoolTime.find( info.s_nGroupID ) == m_mapCoolTime.end() );
		m_mapCoolTime[ info.s_nGroupID ] = pData;
	}

	excel.Close();
}

void CsItemMng::_LoadData( char* cExcel )
{
#define DATA_SHEET_NAME					"Data"
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;
	bool bSuccess = excel.Open( cExcel, DATA_SHEET_NAME );
	assert_cs( bSuccess == true );	

#define IL_COL_DISPID		3			
#define IL_COL_ITEM_S		2			

	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD nItemS;
	DWORD dwDispID;	
	while( excel.MoveNext() == true )
	{
		nItemS = atoi( excel.GetData( IL_COL_ITEM_S, cBuffer ) );
		if( nItemS == 0 )
			continue;

		// Data 시트 소분류 타입 설명에서 스트링 얻어옴 chu8820
		TCHAR szBuffer[ CEXCLE_CELL_LEN ];
		sTYPENAME* pTypeName = csnew sTYPENAME;

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			{
				M2W( szBuffer, excel.GetData( IL_COL_ITEM_S + 2, cBuffer ), ITEM_NAME_LEN );
				if( (int)_tcslen( szBuffer ) >= ITEM_NAME_LEN -1 )
					CsMessageBox( MB_OK, L"ItemList.xls_Data시트\n소분류ID = %d, 의 소분류 타입 설명 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", nItemS, (int)_tcslen( szBuffer ), ITEM_NAME_LEN );
				else
					_tcscpy_s( pTypeName->s_szName, ITEM_NAME_LEN, szBuffer  );
			}
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			{
				wchar_t wBuffer[ CEXCLE_CELL_LEN ];

				wcscpy_s( szBuffer, ITEM_NAME_LEN, excel.GetData( IL_COL_ITEM_S + 2 , wBuffer) );

				_tcscpy_s( pTypeName->s_szName, ITEM_NAME_LEN, szBuffer  );
			}
			break;
		}
		m_MapTypeName[ nItemS ] = pTypeName;

		// ID
		dwDispID = atoi( excel.GetData( IL_COL_DISPID, cBuffer ) );

		if( dwDispID == 0 )
			continue;

		m_MapDisp[ nItemS ] = dwDispID;
	}     

	excel.Close();

}

// Rank테이블 load관련 추가	lks007
void CsItemMng::_LoadRank( char* cExcel )
{
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;	
	bool bSuccess = excel.Open( cExcel, "Item_Rank" );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];

	CsItemRank::sRANK rank;

	while(excel.MoveNext() == true)
	{
		USHORT nDropRank = 6;
		int nItemNo			= atoi( excel.GetData(1, cBuffer) );
		rank.s_nDropType	= atoi( excel.GetData(4, cBuffer) );
		rank.s_nDorpCount	= atoi( excel.GetData(5, cBuffer) );

		CsItemRank* pRank = csnew CsItemRank;
		pRank->Init(&rank/*, &list_Item*/);
		m_mapRank[nItemNo] = pRank;
	}
	
	excel.Close();
}
// 여기까지

void CsItemMng::_LoadAttributeData( char* cExcel )
{
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;	
	bool bSuccess = excel.Open( cExcel, "Attribute_Data" );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];

	#define IL_COMMON_DISPID1		0	
	#define IL_COMMON_DISPID2		1

	CsElementItem::sINFO sElementItem;
	DWORD dwType;
	DWORD dwItemID;
	while(excel.MoveNext() == true)
	{
		
		dwType		= atoi( excel.GetData(0, cBuffer) );
		dwItemID		= atoi( excel.GetData(1, cBuffer) );		
		sElementItem.s_dwItemID	= atoi( excel.GetData(6, cBuffer) );
		

		CsElementItem* pElem = csnew CsElementItem;
		pElem->Init(&sElementItem);


		if(IL_COMMON_DISPID1 == dwType)
			m_mapElementItem1[dwItemID] = pElem;
		else if(IL_COMMON_DISPID2 == dwType)
			m_mapElementItem2[dwItemID] = pElem;

	}

	excel.Close();
}

void CsItemMng::_LoadAccessory( char* cExcel )
{
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}
//Option
	cExcelReader excel;	
	bool bSuccess = excel.Open( cExcel, "Accessory_Option" );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	char cBuffer[ CEXCLE_CELL_LEN ];

#define IL_COL_OPTION_IDX						0
#define IL_COL_OPTION_GAIN					1
#define IL_COL_OPTION_ENCHANT_LIMIT	2
#define IL_COL_OPTION_OPTION				3
#define IL_COL_OPTION_MIN_VALUE			4
#define IL_COL_OPTION_MAX_VALUE			5

	CsAccessory_Option::sINFO pAccessory;
	while(excel.MoveNext() == true)
	{
		pAccessory.s_nIdx = atoi( excel.GetData( IL_COL_OPTION_IDX, cBuffer) );
		pAccessory.s_nGainOpt = atoi( excel.GetData( IL_COL_OPTION_GAIN, cBuffer) );
		pAccessory.s_nLimit = atoi( excel.GetData( IL_COL_OPTION_ENCHANT_LIMIT, cBuffer) );

		int nOptIdx = IL_COL_OPTION_OPTION;
		int nMinValue = IL_COL_OPTION_MIN_VALUE;
		int nMaxValue = IL_COL_OPTION_MAX_VALUE;

		for( int i = 0 ; i < MAX_OPT_COUNT ; i++ )
		{
			pAccessory.m_OptInfo[i].s_nOptIdx = atoi( excel.GetData( nOptIdx, cBuffer) );
			pAccessory.m_OptInfo[i].s_nMin = atoi( excel.GetData( nMinValue, cBuffer) );
			pAccessory.m_OptInfo[i].s_nMax = atoi( excel.GetData( nMaxValue, cBuffer) );

			nOptIdx += 3;
			nMinValue += 3;
			nMaxValue += 3;
		}

		CsAccessory_Option* pAccessoryOption = csnew CsAccessory_Option;
		pAccessoryOption->Init( &pAccessory );
		
		m_mapAccessoryOption[ pAccessory.s_nIdx ] = pAccessoryOption;
	}

	excel.Close();

//Enchant
	bSuccess = excel.Open( cExcel, "Accessory_Enchant" );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

#define IL_COL_ENCHANT_IDX						0
#define IL_COL_ENCHANT_OPTION				1
#define IL_COL_ENCHANT_VALUE					4

	CsAccessory_Enchant::sINFO pAccessory_E;
	while(excel.MoveNext() == true)
	{
		pAccessory_E.s_nIdx = atoi( excel.GetData( IL_COL_ENCHANT_IDX, cBuffer) );
		pAccessory_E.s_nOpt = atoi( excel.GetData( IL_COL_ENCHANT_OPTION, cBuffer) );
		pAccessory_E.s_nVal = atoi( excel.GetData( IL_COL_ENCHANT_VALUE, cBuffer) );

		CsAccessory_Enchant* pAccessoryEnchant = csnew CsAccessory_Enchant;
		pAccessoryEnchant->Init( &pAccessory_E );

		m_mapAccessoryEnchant[ pAccessory_E.s_nIdx ] = pAccessoryEnchant;
	}

	excel.Close();
}

void CsItemMng::_ReloadName( char* cExcel )
{

#define ITEM_SHEET_NAME					"Item"
	if( _access_s( cExcel, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s 이 존재 하지 않습니다.", cExcel );
		return;
	}

	cExcelReader excel;
	bool bSuccess = excel.Open( cExcel, ITEM_SHEET_NAME );
	assert_cs( bSuccess == true );

#define IL_COL_ITEMID					1
#define IL_COL_NAME_KOR					IL_COL_ITEMID + 1
#define IL_COL_NAME_ENG					IL_COL_NAME_KOR + 1

	/*assert_cs( excel.GetColCount() >= IL_COL_SKILL );*/

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD dwItemID;
	CsItem::sINFO* pInfo;
	while( excel.MoveNext() == true )
	{
		// ID
		dwItemID = atoi( excel.GetData( IL_COL_ITEMID, cBuffer ) );
		if( dwItemID == 0 )
			continue;

		pInfo = nsCsFileTable::g_pItemMng->GetItem( dwItemID )->GetInfo();

		// name
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( pInfo->s_szName, excel.GetData( IL_COL_NAME_KOR, cBuffer ), ITEM_NAME_LEN );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( pInfo->s_szName, excel.GetData( IL_COL_NAME_ENG, cBuffer ), ITEM_NAME_LEN );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			//M2W( pInfo->s_szName, excel.GetData( IL_COL_NAME_ENG, cBuffer ), ITEM_NAME_LEN );
			wcscpy_s(pInfo->s_szName, ITEM_NAME_LEN, excel.GetData( IL_COL_NAME_ENG, wBuffer ));
			break;
		default:
			assert_cs( false );
		}
		__CsStringChange::NewLineCheck( pInfo->s_szName );
	}     

	excel.Close();
}

CsItem* CsItemMng::GetItem( DWORD dwItemID )
{
	//assert_csm1( m_Map.find( dwItemID ) != m_Map.end(), _T( "ID = %d" ), dwItemID );
	CsItem::MAP_IT it = m_Map.find( dwItemID );
	if( it == m_Map.end() )
		return NULL;
	return it->second;
}


CsItemExchange* CsItemMng::GetExchangeItem( DWORD dwNpcID, DWORD dwItemID )
{
	CsItemExchange::MAP_IT	it = m_mapExchange.begin();
	CsItemExchange::MAP_IT	itEnd = m_mapExchange.end();

	int nIndex = 0;
	for( ; it != itEnd; ++it )
	{
		if( dwNpcID != it->second->GetInfo()->s_dwNpcID )		//npc ID 비교
		{
			nIndex++;
			continue;
		}
		
		if( it->second->GetInfo()->s_dwItemID != dwItemID)		//아이템 비교
		{
			nIndex++;
			continue;
		}
		else	
			return m_mapExchange[ nIndex ];
	}
	//assert_csm1( m_mapExchange.find( dwItemID ) != m_mapExchange.end(), _T( "ID = %d" ), dwItemID );
	return NULL;
}

TCHAR* CsItemMng::GetCategory( DWORD dwCategory )
{
	CsItemTap::MAP_IT it = m_ItemCategory.find( dwCategory );
	if( it == m_ItemCategory.end() )
		return NULL;
	
	SAFE_POINTER_RETVAL( it->second, NULL );

	CsItemTap::sINFO* pInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, NULL );

	return pInfo->s_szName;
}

CsItemCoolTime* CsItemMng::GetCoolTime( DWORD nGroupID )
{
	CsItemCoolTime::MAP_IT it = m_mapCoolTime.find( nGroupID );
	if( it == m_mapCoolTime.end() )
		return NULL;
	return it->second;
// 	assert_csm1( m_mapCoolTime.find( nGroupID ) != m_mapCoolTime.end(), _T( "GetCoolTime() GroupID = %d" ), nGroupID );
// 	return m_mapCoolTime[ nGroupID ];
}

// Rank테이블 load관련 추가	lks007
int CsItemMng::GetDropCount(int nItemID)
{
	CsItemRank::MAP_IT it = m_mapRank.find(nItemID);
	if( it == m_mapRank.end() )
		return 0;

	SAFE_POINTER_RETVAL( it->second, 0 );

	return it->second->GetCount();
}
// 여기까지

#define BASE_ITEM_LIST_BIN		"ItemList.bin"
void CsItemMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, BASE_ITEM_LIST_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_Map.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsItem::MAP_IT it = m_Map.begin();
	CsItem::MAP_IT itEnd = m_Map.end();
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second->GetInfo(), sizeof( CsItem::sINFO ), 1, fp );
	}

	nCount = (int)m_ItemCategory.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsItemTap::MAP_IT it2 = m_ItemCategory.begin();
	CsItemTap::MAP_IT it2End = m_ItemCategory.end();
	for( ; it2!=it2End; ++it2 )
	{
		fwrite( it2->second->GetInfo(), sizeof( CsItemTap::sINFO ), 1, fp );
	}

	nCount = (int)m_mapCoolTime.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsItemCoolTime::MAP_IT it3 = m_mapCoolTime.begin();
	CsItemCoolTime::MAP_IT it3End = m_mapCoolTime.end();
	for( ; it3!=it3End; ++it3 )
	{
		fwrite( it3->second->GetInfo(), sizeof( CsItemCoolTime::sINFO ), 1, fp );
	}

	nCount = (int)m_MapDisp.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< DWORD, DWORD >::iterator it4 = m_MapDisp.begin();
	std::map< DWORD, DWORD >::iterator it4End = m_MapDisp.end();

	DWORD dwType;
	for( ; it4!=it4End; ++it4 )
	{
		dwType = it4->first;
		fwrite( &dwType, sizeof( DWORD ), 1, fp );
		fwrite( &it4->second, sizeof( DWORD ), 1, fp );
	}

	//Data 시트 소분류 타입 설명에서 스트링 얻어옴 chu8820
	nCount = (int)m_MapTypeName.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< DWORD, sTYPENAME* >::iterator itN = m_MapTypeName.begin();
	std::map< DWORD, sTYPENAME* >::iterator itNEnd = m_MapTypeName.end();

	for( ; itN!=itNEnd; ++itN )
	{
		dwType = itN->first;
		fwrite( &dwType, sizeof( DWORD ), 1, fp );

		fwrite( itN->second, sizeof( sTYPENAME ), 1, fp );
	}

	// Rank테이블 load관련 추가	lks007
	nCount = (int)m_mapRank.size();
	fwrite( &nCount, sizeof(int), 1, fp );

	CsItemRank::MAP_IT it5 = m_mapRank.begin();
	CsItemRank::MAP_IT it5End = m_mapRank.end();

	for( ; it5 != it5End; ++it5 )
	{
		fwrite( &it5->first, sizeof(int), 1, fp );
		fwrite( it5->second->GetRank(), sizeof(CsItemRank::sRANK), 1, fp );
	}
	// 여기까지
	
	{
		nCount = (int)m_mapElementItem1.size();
		fwrite( &nCount, sizeof(int), 1, fp );

		CsElementItem::MAP_IT it = m_mapElementItem1.begin();
		CsElementItem::MAP_IT itEnd = m_mapElementItem1.end();

		for( ; it != itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof(CsElementItem::sINFO), 1, fp );
		}
	}

	{
		nCount = (int)m_mapElementItem2.size();
		fwrite( &nCount, sizeof(int), 1, fp );

		CsElementItem::MAP_IT it = m_mapElementItem2.begin();
		CsElementItem::MAP_IT itEnd = m_mapElementItem2.end();

		for( ; it != itEnd; ++it )
		{
			fwrite( it->second->GetInfo(), sizeof(CsElementItem::sINFO), 1, fp );
		}
	}

	//물물교환상점

	nCount = (int)m_mapExchange.size();
	fwrite( &nCount, sizeof(int), 1, fp );

	CsItemExchange::MAP_IT itEx = m_mapExchange.begin();
	CsItemExchange::MAP_IT itExEnd = m_mapExchange.end();

	for( ; itEx != itExEnd; ++itEx )
	{
		fwrite( &itEx->first, sizeof(int), 1, fp );
		fwrite( itEx->second->GetInfo(), sizeof(CsItemExchange::sINFO), 1, fp );
	}

	// 악세서리
	//Option
	nCount = (int)m_mapAccessoryOption.size();
	fwrite( &nCount, sizeof(int), 1, fp );

	CsAccessory_Option::MAP_IT itAcc = m_mapAccessoryOption.begin();
	CsAccessory_Option::MAP_IT itAccEnd = m_mapAccessoryOption.end();

	for( ; itAcc != itAccEnd; ++itAcc )
	{
		fwrite( &itAcc->first, sizeof(int), 1, fp );
		fwrite( itAcc->second->GetInfo(), sizeof(CsAccessory_Option::sINFO), 1, fp );
	}
	//Enchant
	nCount = (int)m_mapAccessoryEnchant.size();
	fwrite( &nCount, sizeof(int), 1, fp );

	CsAccessory_Enchant::MAP_IT itAcc2 = m_mapAccessoryEnchant.begin();
	CsAccessory_Enchant::MAP_IT itAccEnd2 = m_mapAccessoryEnchant.end();

	for( ; itAcc2 != itAccEnd2; ++itAcc2 )
	{
		fwrite( &itAcc2->first, sizeof(int), 1, fp );
		fwrite( itAcc2->second->GetInfo(), sizeof(CsAccessory_Enchant::sINFO), 1, fp );
	}

	_SaveBin_ItemProductionTable(fp);
	_SaveBin_AssistItemGroupTable(fp);
	_SaveBin_ItemXAISystemTable(fp);

	_SaveBin_RankEffect(fp);
	_SaveBin_DpartsItem(fp);
			
	fclose( fp );
}

bool CsItemMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, BASE_ITEM_LIST_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsItem::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		fread( &info, sizeof( CsItem::sINFO ), 1, fp );

		CsItem* pObject = csnew CsItem;
		pObject->Init( &info );
		assert_cs( m_Map.find( info.s_dwItemID ) == m_Map.end() );
		m_Map[ info.s_dwItemID ] = pObject;		
	}
	
	fread( &nCount, sizeof( int ), 1, fp );

	CsItemTap::sINFO infoCategory;
	for( int i=0; i<nCount; ++i )
	{
		fread( &infoCategory, sizeof( CsItemTap::sINFO ), 1, fp );

		CsItemTap* pObject = csnew CsItemTap;
		pObject->Init( &infoCategory );
		assert_cs( m_ItemCategory.find( infoCategory.s_nSellClass ) == m_ItemCategory.end() );
		m_ItemCategory[ infoCategory.s_nSellClass ] = pObject;
	}

	fread( &nCount, sizeof( int ), 1, fp );

	CsItemCoolTime::sINFO infoCoolTime;
	for( int i=0; i<nCount; ++i )
	{
		fread( &infoCoolTime, sizeof( CsItemCoolTime::sINFO ), 1, fp );

		CsItemCoolTime* pObject = csnew CsItemCoolTime;
		pObject->Init( &infoCoolTime );
		assert_cs( IsCoolTime( infoCoolTime.s_nGroupID ) == false );
		m_mapCoolTime[ infoCoolTime.s_nGroupID ] = pObject;
	}	

	fread( &nCount, sizeof( int ), 1, fp );

	DWORD dwType;
	DWORD dwDispID;	
	
	for( int i=0; i<nCount; ++i )
	{
		fread( &dwType, sizeof( DWORD ), 1, fp );
		fread( &dwDispID, sizeof( DWORD ), 1, fp );

		assert_cs( m_MapDisp.find( dwType ) == m_MapDisp.end() );
		m_MapDisp[ dwType ] = dwDispID;
	}

	//Data 시트 소분류 타입 설명에서 스트링 얻어옴 chu8820
	fread( &nCount, sizeof( int ), 1, fp );

	for( int i=0; i<nCount; ++i )
	{
		sTYPENAME* pInfo = csnew sTYPENAME;

		fread( &dwType, sizeof( DWORD ), 1, fp );
		fread( pInfo, sizeof( sTYPENAME ), 1, fp );

		assert_cs( m_MapTypeName.find( dwType ) == m_MapTypeName.end() );
		m_MapTypeName[ dwType ] = pInfo;
	}

	// Rank테이블 load관련 추가	lks007
	fread( &nCount, sizeof( int ), 1, fp );

	CsItemRank::sRANK rank;

	int Item_No = 0;
	for( int i=0; i<nCount; ++i )
	{
		fread( &Item_No, sizeof(int), 1, fp);
		fread( &rank, sizeof(CsItemRank::sRANK), 1, fp );
	
		CsItemRank* pItemRank = csnew CsItemRank;
		pItemRank->Init( &rank );
		m_mapRank[Item_No] = pItemRank;
	}
	// 여기까지

	{
		fread( &nCount, sizeof( int ), 1, fp );
		CsElementItem::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsElementItem::sINFO ), 1, fp );

			CsElementItem* pObject = csnew CsElementItem;
			pObject->Init( &info );
			assert_cs( m_mapElementItem1.find( info.s_dwItemID ) == m_mapElementItem1.end() );
			m_mapElementItem1[ i ] = pObject;
		}
	}


	{
		fread( &nCount, sizeof( int ), 1, fp );
		CsElementItem::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			fread( &info, sizeof( CsElementItem::sINFO ), 1, fp );

			CsElementItem* pObject = csnew CsElementItem;
			pObject->Init( &info );
			assert_cs( m_mapElementItem2.find( info.s_dwItemID ) == m_mapElementItem2.end() );
			m_mapElementItem2[ i ] = pObject;
		}
	}

	//물물교환 상점 khc0729
	fread( &nCount, sizeof( int ), 1, fp );
	CsItemExchange::sINFO info1;
	int ExchangeItem_No = 0;
	for( int i=0; i<nCount; ++i )
	{
		fread( &ExchangeItem_No, sizeof(int), 1, fp);
		fread( &info1, sizeof( CsItemExchange::sINFO ), 1, fp );

		CsItemExchange* pObject = csnew CsItemExchange;
		pObject->Init( &info1 );
		//assert_cs( m_mapExchange.find( info1.s_dwItemID ) == m_mapExchange.end() );
		m_mapExchange[ ExchangeItem_No ] = pObject;
	}

	//악세서리
	//Option
	fread( &nCount, sizeof( int ), 1, fp );
	CsAccessory_Option::sINFO infoAccessory_Opt;
	int infoAccessory_Idx = 0;
	for( int i=0; i<nCount; ++i )
	{
		fread( &infoAccessory_Idx, sizeof(int), 1, fp);
		fread( &infoAccessory_Opt, sizeof( CsAccessory_Option::sINFO ), 1, fp );

		CsAccessory_Option* pObject = csnew CsAccessory_Option;
		pObject->Init( &infoAccessory_Opt );
		m_mapAccessoryOption[ infoAccessory_Idx ] = pObject;
	}

	//Enchant
	fread( &nCount, sizeof( int ), 1, fp );
	CsAccessory_Enchant::sINFO infoAccessory_Enchant;
	infoAccessory_Idx = 0;
	for( int i=0; i<nCount; ++i )
	{
		fread( &infoAccessory_Idx, sizeof(int), 1, fp);
		fread( &infoAccessory_Enchant, sizeof( CsAccessory_Enchant::sINFO ), 1, fp );

		CsAccessory_Enchant* pObject = csnew CsAccessory_Enchant;
		pObject->Init( &infoAccessory_Enchant );
		m_mapAccessoryEnchant[ infoAccessory_Idx ] = pObject;
	}

	_LoadBin_ItemProductionTable( fp );
	_LoadBin_AssistItemGroupTable( fp );
	_LoadBin_ItemXAISystemTable( fp );

	_LoadBin_RankEffect( fp );
	_LoadBin_DpartsItem(fp);

	fclose( fp );

	return true;
}

void CsItemMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, BASE_ITEM_LIST_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;

	long offBeforeItems = _tell( nHandle );
	_read( nHandle, &nCount, sizeof( int ) );

	CsItem::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info, sizeof( CsItem::sINFO ) );

		CsItem* pObject = csnew CsItem;
		pObject->Init( &info );
		assert_cs( m_Map.find( info.s_dwItemID ) == m_Map.end() );
		m_Map[ info.s_dwItemID ] = pObject;
	}

	long offBeforeCats = _tell( nHandle );
	_read( nHandle, &nCount, sizeof( int ) );

	CsItemTap::sINFO infoCategory;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &infoCategory, sizeof( CsItemTap::sINFO ) );

		CsItemTap* pObject = csnew CsItemTap;
		pObject->Init( &infoCategory );
		assert_cs( m_ItemCategory.find( infoCategory.s_nSellClass ) == m_ItemCategory.end() );
		m_ItemCategory[ infoCategory.s_nSellClass ] = pObject;
	}

	long offBeforeCool = _tell( nHandle );
	_read( nHandle, &nCount, sizeof( int ) );

	CsItemCoolTime::sINFO infoCoolTime;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &infoCoolTime, sizeof( CsItemCoolTime::sINFO ) );

		CsItemCoolTime* pObject = csnew CsItemCoolTime;
		pObject->Init( &infoCoolTime );
		assert_cs( IsCoolTime( infoCoolTime.s_nGroupID ) == false );
		m_mapCoolTime[ infoCoolTime.s_nGroupID ] = pObject;
	}

	long offBeforeDisp = _tell( nHandle );
	_read( nHandle, &nCount, sizeof( int ) );

	DWORD dwType;
	DWORD dwDispID;

	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &dwType, sizeof( DWORD ) );
		_read( nHandle, &dwDispID, sizeof( DWORD ) );

		auto found = m_MapDisp.find( dwType );
		if( found != m_MapDisp.end() )
		{
			// The bin legitimately contains duplicate disp-type rows (production
			// data has overlapping rows for some categories). Keep the first
			// mapping, don't assert.
			continue;
		}
		m_MapDisp[ dwType ] = dwDispID;
	}

	//Data 시트 소분류 타입 설명에서 스트링 얻어옴 chu8820
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		sTYPENAME* pInfo = csnew sTYPENAME;
		_read( nHandle, &dwType, sizeof( DWORD ) );
		_read( nHandle, pInfo, sizeof( sTYPENAME ));

		// Pack data sometimes contains duplicate type-name rows; keep first, skip dups.
		if( m_MapTypeName.find( dwType ) != m_MapTypeName.end() )
		{
			delete pInfo;
			continue;
		}
		m_MapTypeName[ dwType ] = pInfo;
	}

	// Rank테이블 load관련 추가	lks007
	_read( nHandle, &nCount, sizeof(int) );

	CsItemRank::sRANK rank;

	int Item_No = 0;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &Item_No, sizeof(int) );
		_read( nHandle, &rank, sizeof(CsItemRank::sRANK) );

		CsItemRank* pItemRank = csnew CsItemRank;
		pItemRank->Init( &rank/*, &Drop_Item*/ );
		m_mapRank[Item_No] = pItemRank;
	}
	// 여기까지

	{
		_read( nHandle, &nCount, sizeof( int ) );
		CsElementItem::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsElementItem::sINFO ) );

			CsElementItem* pObject = csnew CsElementItem;
			pObject->Init( &info );
			assert_cs( m_mapElementItem1.find( info.s_dwItemID ) == m_mapElementItem1.end() );
			m_mapElementItem1[ i ] = pObject;
		}
	}

	{
		_read( nHandle, &nCount, sizeof( int ) );
		CsElementItem::sINFO info;
		for( int i=0; i<nCount; ++i )
		{
			_read( nHandle, &info, sizeof( CsElementItem::sINFO ) );

			CsElementItem* pObject = csnew CsElementItem;
			pObject->Init( &info );
			assert_cs( m_mapElementItem2.find( info.s_dwItemID ) == m_mapElementItem2.end() );
			m_mapElementItem2[ i ] = pObject;
		}
	}

	//물물교환상점
	_read( nHandle, &nCount, sizeof( int ) );
	CsItemExchange::sINFO info1;
	int ExchangeItem_No = 0;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &ExchangeItem_No, sizeof(int) );
		_read( nHandle, &info1, sizeof( CsItemExchange::sINFO ) );

		CsItemExchange* pObject = csnew CsItemExchange;
		pObject->Init( &info1 );
		//assert_cs( m_mapExchange.find( info1.s_dwItemID ) == m_mapExchange.end() );
		m_mapExchange[ i ] = pObject;
	}

	//악세서리
	//Option
	_read( nHandle, &nCount, sizeof( int ) );
	CsAccessory_Option::sINFO infoAccessory_Opt;
	int AccessoryItem_Idx = 0;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &AccessoryItem_Idx, sizeof(int) );
		_read( nHandle, &infoAccessory_Opt, sizeof( CsAccessory_Option::sINFO ) );

		CsAccessory_Option* pObject = csnew CsAccessory_Option;
		pObject->Init( &infoAccessory_Opt );
		m_mapAccessoryOption[ AccessoryItem_Idx ] = pObject;
	}

	//Enchant
	_read( nHandle, &nCount, sizeof( int ) );
	CsAccessory_Enchant::sINFO infoAccessory_Enchant;
	AccessoryItem_Idx = 0;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &AccessoryItem_Idx, sizeof(int) );
		_read( nHandle, &infoAccessory_Enchant, sizeof( CsAccessory_Enchant::sINFO ) );

		CsAccessory_Enchant* pObject = csnew CsAccessory_Enchant;
		pObject->Init( &infoAccessory_Enchant );
		m_mapAccessoryEnchant[ AccessoryItem_Idx ] = pObject;
	}

	_LoadFilePack_ItemProductionTable( nHandle );
	_LoadFilePack_AssistItemGroupTable( nHandle );
	_LoadFilePack_ItemXAISystemTable(nHandle);

	_LoadFilePack_RankEffect( nHandle );
	_LoadFilePack_DpartsItem(nHandle);

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}



bool CsItemMng::IsItem( DWORD const & dwItemID ) const
{ 
	CsItem::MAP::const_iterator it = m_Map.find( dwItemID );
	if( it == m_Map.end() )
		return false;

	return true; 
}

std::wstring CsItemMng::GetItemName( DWORD const & dwItemID ) const
{
	CsItem::MAP::const_iterator it = m_Map.find( dwItemID );
	if( it == m_Map.end() )
		return L"";
	
	SAFE_POINTER_RETVAL( it->second, L"" );
	return it->second->GetName();
}

bool CsItemMng::IsCoolTime( DWORD const & nGroupID ) const
{ 
	CsItemCoolTime::MAP::const_iterator it = m_mapCoolTime.find( nGroupID );
	if( it == m_mapCoolTime.end() )
		return false;

	return true;
}

DWORD CsItemMng::TypeT_to_Disp( DWORD nTypeT )
{ 
	std::map< DWORD, DWORD >::iterator it = m_MapDisp.find(nTypeT);
	if( it == m_MapDisp.end() )
		return 0;

	return it->second;
}

DWORD CsItemMng::TypeLS_to_Disp( DWORD nTypeL, DWORD nTypeS ) 
{ 
	DWORD t = nTypeL * 100 + nTypeS; 
	return TypeT_to_Disp( t );
}

// CAPSULE_EVOLUTION : Data 시트 소분류 타입 설명에서 스트링 얻어옴 chu8820
TCHAR* CsItemMng::GetTypeName( DWORD nTypeS ) 
{ 
	std::map< DWORD, sTYPENAME* >::iterator it = m_MapTypeName.find( nTypeS );
	if( it == m_MapTypeName.end() )
		return NULL;

	SAFE_POINTER_RETVAL( it->second, NULL );
	return it->second->s_szName;	
}

////////////////////////////////////////////////////////////////////////////////////
void CsItemMng::_LoadRankEffect(char* cExcel)
{
	cExcelReader excel;	
	bool bSuccess = excel.Open( cExcel, "Item_Rank_Effect" );
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
		DWORD nItemCode = 0;
		excel.Read_Value( 1, nItemCode );

		if( 0 == nItemCode )
			break;

		LIST_RANK_INFO lRankInfo;

		int nInterval = 4, i = 0;
		while( 1 )
		{
			sRankInfo rankInfo;
			excel.Read_Value( 3 + (nInterval * i), rankInfo.dwItemCode );

			if( 0 == rankInfo.dwItemCode )
				break;

			excel.Read_Value( 5 + (nInterval * i), rankInfo.dwIconNo );
			excel.Read_Value( 6 + (nInterval * i), rankInfo.nRank );
			lRankInfo.push_back( rankInfo );
			++i;
		}
		auto ib = m_mapRankEffect.insert( std::make_pair( nItemCode, lRankInfo ) );
		assert_csm( ib.second, _T( "Key already exists." ) );
	}

	excel.Close();
}

void CsItemMng::_SaveBin_RankEffect(FILE* fp)
{
	int nCount = static_cast<int>( m_mapRankEffect.size() );
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	MAP_RANK_EFFECT_IT it = m_mapRankEffect.begin();
	MAP_RANK_EFFECT_IT itEnd = m_mapRankEffect.end();
	for( ; it != itEnd; ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );

		int nListCount = (int)it->second.size();
		CmUtil::CmBinSave::Save_Value( fp, &nListCount );
		
		LIST_RANK_INFO_IT itRank = it->second.begin();
		LIST_RANK_INFO_IT itEndRank = it->second.end();
		for( ; itRank != itEndRank; ++itRank )
		{
			CmUtil::CmBinSave::Save_Value( fp, &itRank->dwItemCode );
			CmUtil::CmBinSave::Save_Value( fp, &itRank->dwIconNo );
			CmUtil::CmBinSave::Save_Value( fp, &itRank->nRank );
		}
	}
}

void CsItemMng::_LoadBin_RankEffect(FILE* fp)
{
	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i = 0; i < nCount; ++i )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmBinRead::Read_Value( fp, &dwItemCode );
		if( 0 == dwItemCode )
			continue;

		MAP_RANK_EFFECT_IT it = m_mapRankEffect.find( dwItemCode );
		if( m_mapRankEffect.end() == it )
		{
			auto ib = m_mapRankEffect.insert( std::make_pair( dwItemCode, LIST_RANK_INFO() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "Insertion failure." ) );
				continue;
			}
			it = ib.first;
		}
		
		int nListCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nListCount );
		for( int j = 0; j < nListCount; ++j )
		{
			sRankInfo rankInfo;
			CmUtil::CmBinRead::Read_Value( fp, &rankInfo.dwItemCode );
			CmUtil::CmBinRead::Read_Value( fp, &rankInfo.dwIconNo );
			CmUtil::CmBinRead::Read_Value( fp, &rankInfo.nRank );
			it->second.push_back( rankInfo );
		}
	}
}

void CsItemMng::_LoadFilePack_RankEffect(int nHandle)
{
	int nCount;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i = 0; i < nCount; ++i )
	{
		DWORD dwItemCode = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &dwItemCode );
		if( 0 == dwItemCode )
			continue;

		LIST_RANK_INFO lRankInfo;
		MAP_RANK_EFFECT_IT it = m_mapRankEffect.find( dwItemCode );
		if( m_mapRankEffect.end() == it )
		{
			auto ib = m_mapRankEffect.insert( std::make_pair( dwItemCode, lRankInfo ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "Insertion failure." ) );
				continue;
			}
			it = ib.first;
		}

		int nListCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nListCount );
		for( int j = 0; j < nListCount; ++j )
		{
			sRankInfo rankInfo;
			CmUtil::CmPackRead::Read_Value( nHandle, &rankInfo.dwItemCode );
			CmUtil::CmPackRead::Read_Value( nHandle, &rankInfo.dwIconNo );
			CmUtil::CmPackRead::Read_Value( nHandle, &rankInfo.nRank );
			it->second.push_back( rankInfo );
		}
	}
}

sRankInfo const * CsItemMng::GetRankInfo(DWORD dwItemCode, DWORD dwRankItemCode) const
{
	MAP_RANK_EFFECT_CIT it = m_mapRankEffect.find( dwItemCode );
	if( it != m_mapRankEffect.end() )
	{
		LIST_RANK_INFO_CIT itRank = it->second.begin();
		LIST_RANK_INFO_CIT itEndRank = it->second.end();
		for( ; itRank != itEndRank; ++itRank )
		{
			if( dwRankItemCode == itRank->dwItemCode )
				return &(*itRank);
		}
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
void CsItemMng::_LoadDigimonPartsItem(char* cExcel)
{
	cExcelReader excel;
	bool bSuccess = excel.Open(cExcel, "Look_Item");
	if (!bSuccess)
		return;

	bSuccess = excel.FirstOfSkip(2);
	if (!bSuccess)
	{
		excel.Close();
		return;
	}

	while (excel.MoveNext() == true)
	{
		DWORD dwItemCode = 0;
		excel.Read_Value(1, dwItemCode);
		if (0 == dwItemCode)
			break;

		std::map<DWORD, sDEquipStruct>::const_iterator it = m_DPartItemTables.find(dwItemCode);
		if (it != m_DPartItemTables.end())
			break;

		sDEquipStruct item;
		excel.Read_Value(3, item.dwDigimonTBIdx);
		excel.Read_Value(5, item.nPartIndex);
		excel.Read_Value(6, item.fileName);

		m_DPartItemTables.insert(std::make_pair(dwItemCode, item));
	}
	excel.Close();
}

void CsItemMng::_SaveBin_DpartsItem(FILE* fp)
{
	int nCount = (int)m_DPartItemTables.size();
	CmUtil::CmBinSave::Save_Value(fp, &nCount);

	std::map<DWORD, sDEquipStruct>::const_iterator it = m_DPartItemTables.begin();
	for (; it != m_DPartItemTables.end(); ++it)
	{
		CmUtil::CmBinSave::Save_Value(fp, &it->first);
		CmUtil::CmBinSave::Save_Value(fp, &(it->second.dwDigimonTBIdx));
		CmUtil::CmBinSave::Save_Value(fp, &(it->second.nPartIndex));
		CmUtil::CmBinSave::Save_Value(fp, it->second.fileName);
	}
}

void CsItemMng::_LoadBin_DpartsItem(FILE* fp)
{
	int nCount = 0;
	CmUtil::CmBinRead::Read_Value(fp, &nCount);
	for (int n = 0; n < nCount; ++n)
	{
		DWORD dwItemCode = 0;
		CmUtil::CmBinRead::Read_Value(fp, &dwItemCode);

		sDEquipStruct addItem;
		CmUtil::CmBinRead::Read_Value(fp, &addItem.dwDigimonTBIdx);
		CmUtil::CmBinRead::Read_Value(fp, &addItem.nPartIndex);
		CmUtil::CmBinRead::Read_Value(fp, addItem.fileName);
		m_DPartItemTables.insert(std::make_pair(dwItemCode, addItem));
	}
}

void CsItemMng::_LoadFilePack_DpartsItem(int nHandle)
{
	int nCount = 0;
	CmUtil::CmPackRead::Read_Value(nHandle, &nCount);
	for (int n = 0; n < nCount; ++n)
	{
		DWORD dwItemCode = 0;
		CmUtil::CmPackRead::Read_Value(nHandle, &dwItemCode);

		sDEquipStruct addItem;
		CmUtil::CmPackRead::Read_Value(nHandle, &addItem.dwDigimonTBIdx);
		CmUtil::CmPackRead::Read_Value(nHandle, &addItem.nPartIndex);
		CmUtil::CmPackRead::Read_Value(nHandle, addItem.fileName);
		m_DPartItemTables.insert(std::make_pair(dwItemCode, addItem));
	}
}

bool CsItemMng::_IsDPartsItem(DWORD const& dwItemCode, DWORD const& dwDTbIdx) const
{
	std::map<DWORD, sDEquipStruct>::const_iterator it = m_DPartItemTables.find(dwItemCode);
	if (it == m_DPartItemTables.end())
		return false;

	if (it->second.dwDigimonTBIdx != dwDTbIdx)
		return false;

	return true;
}

bool CsItemMng::GetDPartsInfo(DWORD const& dwItemCode, DWORD const& dwDTbIdx, int& nPartsIdx, std::string & file)
{
	std::map<DWORD, sDEquipStruct>::const_iterator it = m_DPartItemTables.find(dwItemCode);
	if (it == m_DPartItemTables.end())
		return false;

	if (it->second.dwDigimonTBIdx != dwDTbIdx)
		return false;

	nPartsIdx = it->second.nPartIndex;
	file = it->second.fileName;
	return true;
}

// 사용 가능한 디지몬 파츠 이펙트 아이템인지
bool CsItemMng::IsEnableDigimonPartsItem(DWORD const& dwItemCode, DWORD const& dwDTbIdx, int const& nPartsIdx ) const
{
	std::map<DWORD, sDEquipStruct>::const_iterator it = m_DPartItemTables.find(dwItemCode);
	if (it == m_DPartItemTables.end())
		return false;

	if (it->second.dwDigimonTBIdx != dwDTbIdx)
		return false;

	if (it->second.nPartIndex != nPartsIdx)
		return false;

	return true;
}

bool CsItemMng::GetPartsIndex(DWORD const& dwItemCode, int & nPartsIdx) const
{
	std::map<DWORD, sDEquipStruct>::const_iterator it = m_DPartItemTables.find(dwItemCode);
	if (it == m_DPartItemTables.end())
		return false;

	nPartsIdx = it->second.nPartIndex;
	return true;
}
