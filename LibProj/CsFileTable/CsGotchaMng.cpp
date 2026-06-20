
#include "stdafx.h"
#include "CsGotchaMng.h"
#pragma warning(disable:4996)


void CsGotchaMng::Delete()
{
	//Gotcha_Machine
	{
		CsGotcha_Machine::MAP_IT it = m_mapGotcha_Machine.begin();
		CsGotcha_Machine::MAP_IT itEnd = m_mapGotcha_Machine.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapGotcha_Machine.clear();	
	}

	//Gotcha_Item
	{
		CsGotcha_Item::MAP_IT it = m_mapGotcha_Item.begin();
		CsGotcha_Item::MAP_IT itEnd = m_mapGotcha_Item.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapGotcha_Item.clear();	
	}

	//Gotcha_RareItem
	{
		CsGotcha_RareItem::MAP_IT it = m_mapGotcha_RareItem.begin();
		CsGotcha_RareItem::MAP_IT itEnd = m_mapGotcha_RareItem.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->Delete();
			delete it->second;
		}
		m_mapGotcha_RareItem.clear();	
	}

}

bool CsGotchaMng::Init( char* cPath )
{
	R_Idx = 0;
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
void CsGotchaMng::_LoadExcel()
{
	_LoadGotcha_Machine();
	_LoadGotcha_Item();
	_LoadGotcha_Mystery_Item();
	_LoadGotcha_Mystery_Coin();
}
void CsGotchaMng::_LoadGotcha_Machine()
{
	#define GOTCHA_MACHINE_SHEET_NAME			"Gotcha_Machine"

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_Gotcha(), GOTCHA_MACHINE_SHEET_NAME );
	if( !bSuccess )
		return;

	#define IL_COL_NPC_ID						0
	#define IL_COL_USE_ITEM_CODE			IL_COL_NPC_ID + 1 // 1
	#define IL_COL_USE_ITEM_COUNT			IL_COL_USE_ITEM_CODE + 1 // 2
	#define IL_COL_LIMIT							IL_COL_USE_ITEM_COUNT + 1 // 3
	#define IL_COL_START_DATE					IL_COL_LIMIT + 1 // 4
	#define IL_COL_END_DATE					IL_COL_START_DATE + 1 // 5
	#define IL_COL_START_TIME					IL_COL_END_DATE + 1 // 6
	#define IL_COL_END_TIME						IL_COL_START_TIME + 1 // 7
	#define IL_COL_MIN_LEVEL					IL_COL_END_TIME + 1 // 8
	#define IL_COL_MAX_LEVEL					IL_COL_MIN_LEVEL + 1 // 9
	#define IL_COL_RAREITEM_CNT				IL_COL_MAX_LEVEL + 1 // 10
	#define IL_COL_RAREITEM_ITEM				IL_COL_RAREITEM_CNT + 1 // 11
	#define IL_COL_RAREITEM_NAME			IL_COL_RAREITEM_ITEM + 1 // 12
	#define IL_COL_RAREITEM_GIVE				IL_COL_RAREITEM_NAME + 2 // 14
	#define IL_COL_SHOWCOUNT_UI				33

	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsGotcha_Machine::sINFO info;

	USHORT nindex = 0; //각 행 고유 식별 안되 ㅠㅠ 이걸로 인덱스 잡자

	while( excel.MoveNext() == true )
	{
		//NPC 아이디
		info.s_dwNpc_Id = atoi( excel.GetData( IL_COL_NPC_ID , cBuffer ) );
		//가차머신 사용에 필요한 아이템 ID
		info.s_dwUseItem_Code = atoi( excel.GetData( IL_COL_USE_ITEM_CODE , cBuffer ) );
		//필요아이템의 수
		info.s_nUseItem_Cnt = atoi( excel.GetData( IL_COL_USE_ITEM_COUNT , cBuffer ) );
		//사용 가능 여부
		if(atoi( excel.GetData( IL_COL_LIMIT , cBuffer ) ))
			info.s_bLimit = true; 
		else
			info.s_bLimit = false;

		//시작,종료 날짜와 시간

		info.s_nStart_Date = GetDate( excel.GetData( IL_COL_START_DATE , cBuffer ) );
		info.s_nEnd_Date = GetDate( excel.GetData( IL_COL_END_DATE , cBuffer ) );
		info.s_nStart_Time = GetTime( excel.GetData( IL_COL_START_TIME , cBuffer ) );
		info.s_nEnd_Time = GetTime( excel.GetData( IL_COL_END_TIME , cBuffer ) );

		//최소 최대 제한 레벨
		info.s_nMin_Lv = atoi( excel.GetData( IL_COL_MIN_LEVEL , cBuffer ) );
		info.s_nMax_Lv = atoi( excel.GetData( IL_COL_MAX_LEVEL , cBuffer ) );
		
		//레어아이템 갯수
		int nRareItemCnt = atoi( excel.GetData( IL_COL_RAREITEM_CNT , cBuffer ) ) ; 
		//레어아이템 갯수만큼 돌아
		for(int i = 0; i<nRareItemCnt; i++)
		{
			CsGotcha_RareItem::sINFO info_R;

			//NPC 아이디
			info_R.s_nNpcID = info.s_dwNpc_Id;
			//레어아이템 코드(11,16,21)
			info_R.s_nRareItem = atoi( excel.GetData( ( IL_COL_RAREITEM_ITEM + (i*5) ) , cBuffer ) );
			//레어아이템 이름(12,17,22)
			excel.Read_String( IL_COL_RAREITEM_NAME + (i*5), info_R.s_szRareItemName, ITEM_NAME_LEN );
			//M2W( info_R.s_szRareItemName , excel.GetData( ( IL_COL_RAREITEM_NAME + (i*5) ) , cBuffer ), ITEM_NAME_LEN ) ;
			//레어아이템 지급갯수(14,19,24)
			info_R.s_nRareItemGive = atoi( excel.GetData( ( IL_COL_RAREITEM_GIVE + i*5) , cBuffer ) );

			CsGotcha_RareItem* pRare = csnew CsGotcha_RareItem;
			pRare->Init( &info_R );
			m_mapGotcha_RareItem[ R_Idx ] = pRare;
			R_Idx ++;
		}
		info.s_nShowCountUI = atoi( excel.GetData( IL_COL_SHOWCOUNT_UI , cBuffer ) );
		
		CsGotcha_Machine* pGotcha = csnew CsGotcha_Machine;
		pGotcha->Init( &info );
		m_mapGotcha_Machine[ nindex ] = pGotcha;

		nindex++;
	}
	excel.Close();
}
void CsGotchaMng::_LoadGotcha_Item()
{
	#define GOTCHA_ITEM_SHEET_NAME			"Gotcha_Item"
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_Gotcha(), GOTCHA_ITEM_SHEET_NAME );
	if( !bSuccess )
		return;

	#define IL_COL_GROUP_CODE						0
	#define IL_COL_ITEM_CODE							IL_COL_GROUP_CODE + 1	//1
	#define IL_COL_ITEM_VOLUME						IL_COL_ITEM_CODE + 1		//2
	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsGotcha_Item::sINFO info;

// 		int mapIndex = 0;
	while( excel.MoveNext() == true )
	{
		//그룹 아이디
		info.s_nGroup_Id = atoi( excel.GetData( IL_COL_GROUP_CODE , cBuffer ) );
		//아이템
		for(int i = 0; i < MAX_GROUP_ITEM ;i++)
		{
			info.s_dwItem_Code[i] = atoi( excel.GetData( IL_COL_ITEM_CODE + (i*2) , cBuffer ) );
			info.s_nItem_Cnt[i] = atoi( excel.GetData( IL_COL_ITEM_VOLUME + (i*2) , cBuffer ) );
		}
		CsGotcha_Item* pGotcha = csnew CsGotcha_Item;
		pGotcha->Init( &info );
		m_mapGotcha_Item[ info.s_nGroup_Id ] = pGotcha;
// 			m_mapGotcha_Item[ mapIndex ] = pGotcha;
// 			mapIndex++;
	}
	excel.Close();
}

void CsGotchaMng::_LoadGotcha_Mystery_Item()
{
	#define GOTCHA_MYSTERY_ITEM_SHEET_NAME			"Mistery_Machine"
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_Gotcha(), GOTCHA_MYSTERY_ITEM_SHEET_NAME );
	if( !bSuccess )
		return;

	#define IL_COL_GROUP						0
	#define IL_COL_ITEM							IL_COL_GROUP + 1	//1
	#define IL_COL_EFFECT							IL_COL_ITEM + 4	//5

	char cBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsGotcha_Mystery_Item::sINFO info;

	// 		int mapIndex = 0;
	while( excel.MoveNext() == true )
	{
		//그룹번호
		info.s_nGroup = atoi( excel.GetData( IL_COL_GROUP , cBuffer ) );
		//아이템번호
		info.s_nItem = atoi( excel.GetData( IL_COL_ITEM , cBuffer ) );
		//이펙트 여부
		info.s_nEffect = atoi( excel.GetData( IL_COL_EFFECT , cBuffer ) );
		CsGotcha_Mystery_Item* pMystery_Item= csnew CsGotcha_Mystery_Item;
		pMystery_Item->Init( &info );
		m_mapGotcha_Mystery_Item[ info.s_nItem ] = pMystery_Item;
	}
	excel.Close();
}

void CsGotchaMng::_LoadGotcha_Mystery_Coin()
{
	#define GOTCHA_MYSTERY_COIN_SHEET_NAME			"Mistery_Coin"

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_Gotcha(), GOTCHA_MYSTERY_COIN_SHEET_NAME );
	if( !bSuccess )
		return;

	#define IL_COL_COIN_ORDER						0
	#define IL_COL_COIN_CODE							IL_COL_COIN_ORDER + 1	//1
	#define IL_COL_COIN_COUNT						IL_COL_COIN_CODE + 1		//2

	char cBuffer[ CEXCLE_CELL_LEN ];
	
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	CsGotcha_Mystery_Coin::sINFO info;

	// 		int mapIndex = 0;
	while( excel.MoveNext() == true )
	{
		//코인 우선순위
		info.s_nCoinOrder = atoi( excel.GetData( IL_COL_COIN_ORDER , cBuffer ) );
		//코인 아이템 번호
		info.s_nCoinIdx = atoi( excel.GetData( IL_COL_COIN_CODE , cBuffer ) );
		//코인 소모 개수
		info.s_nCoinCnt = atoi( excel.GetData( IL_COL_COIN_COUNT , cBuffer ) );
#ifdef EGG_PREFERMENT
		// 소모되는 돈
		info.s_nMoney = atoi( excel.GetData( 3 , cBuffer ) );
		// 시작 그룹 번호
		info.s_nStartGroupNo = atoi( excel.GetData( 4 , cBuffer ) );
		// 종료 그룹 번호
		info.s_nEndGroupNo = atoi( excel.GetData( 5 , cBuffer ) );
#endif
		CsGotcha_Mystery_Coin* pMystery_Coin = csnew CsGotcha_Mystery_Coin;
		pMystery_Coin->Init( &info );
		m_mapGotcha_Mystery_Coin[ info.s_nCoinOrder ] = pMystery_Coin;
	}
	excel.Close();
}

#define GOTCHA_BIN		"Gotcha.bin"

bool CsGotchaMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, GOTCHA_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	int nCount;

//Gotcha_Machine	
	fread( &nCount, sizeof( int ), 1, fp );

	CsGotcha_Machine::sINFO info;

	for( int i=0; i<nCount; ++i )
	{
		fread( &info, sizeof( CsGotcha_Machine::sINFO ), 1, fp );

		CsGotcha_Machine* pObject = csnew CsGotcha_Machine;
		pObject->Init( &info );
		m_mapGotcha_Machine[ i ] = pObject;
	}


//Gotcha_Item
	fread( &nCount, sizeof( int ), 1, fp );

	CsGotcha_Item::sINFO info_Item;

	for( int i=0; i<nCount; ++i )
	{
		fread( &info_Item, sizeof( CsGotcha_Item::sINFO ), 1, fp );

		CsGotcha_Item* pObject = csnew CsGotcha_Item;
		pObject->Init( &info_Item );
		m_mapGotcha_Item[ info_Item.s_nGroup_Id ] = pObject;
	}
//Gotcha_RareItem
	fread( &nCount, sizeof( int ), 1, fp );

	CsGotcha_RareItem::sINFO info_RareItem;

	for( int i=0; i<nCount; ++i )
	{
		fread( &info_RareItem, sizeof( CsGotcha_RareItem::sINFO ), 1, fp );

		CsGotcha_RareItem* pObject = csnew CsGotcha_RareItem;
		pObject->Init( &info_RareItem );
		m_mapGotcha_RareItem[ i ] = pObject;
	}

//Gotcha_Mystery_Item
	fread( &nCount, sizeof( int ), 1, fp );

	CsGotcha_Mystery_Item::sINFO info_Mystery_Item;

	for( int i=0; i<nCount; ++i )
	{
		fread( &info_Mystery_Item, sizeof( CsGotcha_Mystery_Item::sINFO ), 1, fp );

		CsGotcha_Mystery_Item* pObject = csnew CsGotcha_Mystery_Item;
		pObject->Init( &info_Mystery_Item );
		m_mapGotcha_Mystery_Item[ info_Mystery_Item.s_nItem ] = pObject;
	}
//Gotcha_Mystery_Coin
	fread( &nCount, sizeof( int ), 1, fp );

	CsGotcha_Mystery_Coin::sINFO info_Mystery_Coin;

	for( int i=0; i<nCount; ++i )
	{
		fread( &info_Mystery_Coin, sizeof( CsGotcha_Mystery_Coin::sINFO ), 1, fp );

		CsGotcha_Mystery_Coin* pObject = csnew CsGotcha_Mystery_Coin;
		pObject->Init( &info_Mystery_Coin );
		m_mapGotcha_Mystery_Coin[ info_Mystery_Coin.s_nCoinOrder ] = pObject;
	}

	fclose( fp );
	return true;
}

void CsGotchaMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, GOTCHA_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

//Gotcha_Machine	
	int nCount = (int)m_mapGotcha_Machine.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsGotcha_Machine::MAP_IT it = m_mapGotcha_Machine.begin();
	CsGotcha_Machine::MAP_IT itEnd = m_mapGotcha_Machine.end();
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second->GetInfo(), sizeof( CsGotcha_Machine::sINFO ), 1, fp );
	}

//Gotcha_Item
	nCount = (int)m_mapGotcha_Item.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsGotcha_Item::MAP_IT it2 = m_mapGotcha_Item.begin();
	CsGotcha_Item::MAP_IT itEnd2 = m_mapGotcha_Item.end();
	for( ; it2!=itEnd2; ++it2 )
	{
		fwrite( it2->second->GetInfo(), sizeof( CsGotcha_Item::sINFO ), 1, fp );
	}
//Gotcha_RareItem
	nCount = (int)m_mapGotcha_RareItem.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsGotcha_RareItem::MAP_IT it3 = m_mapGotcha_RareItem.begin();
	CsGotcha_RareItem::MAP_IT itEnd3 = m_mapGotcha_RareItem.end();
	for( ; it3!=itEnd3; ++it3 )
	{
		fwrite( it3->second->GetInfo(), sizeof( CsGotcha_RareItem::sINFO ), 1, fp );
	}

//Gotcha_Mystery_Item
	nCount = (int)m_mapGotcha_Mystery_Item.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsGotcha_Mystery_Item::MAP_IT it4 = m_mapGotcha_Mystery_Item.begin();
	CsGotcha_Mystery_Item::MAP_IT itEnd4 = m_mapGotcha_Mystery_Item.end();
	for( ; it4!=itEnd4; ++it4 )
	{
		fwrite( it4->second->GetInfo(), sizeof( CsGotcha_Mystery_Item::sINFO ), 1, fp );
	}
//Gotcha_Mystery_Coin
	nCount = (int)m_mapGotcha_Mystery_Coin.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsGotcha_Mystery_Coin::MAP_IT it5 = m_mapGotcha_Mystery_Coin.begin();
	CsGotcha_Mystery_Coin::MAP_IT itEnd5 = m_mapGotcha_Mystery_Coin.end();
	for( ; it5!=itEnd5; ++it5 )
	{
		fwrite( it5->second->GetInfo(), sizeof( CsGotcha_Mystery_Coin::sINFO ), 1, fp );
	}

	fclose( fp );
}

void CsGotchaMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, GOTCHA_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
//Gotcha_Machine	
	_read( nHandle, &nCount, sizeof( int ) );

	CsGotcha_Machine::sINFO info;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info, sizeof( CsGotcha_Machine::sINFO ) );

		CsGotcha_Machine* pObject = csnew CsGotcha_Machine;
		pObject->Init( &info );
		m_mapGotcha_Machine[ i ] = pObject;
	}

//Gotcha_Item
	_read( nHandle, &nCount, sizeof( int ) );

	CsGotcha_Item::sINFO info_Item;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info_Item, sizeof( CsGotcha_Item::sINFO ) );

		CsGotcha_Item* pObject = csnew CsGotcha_Item;
		pObject->Init( &info_Item );
		m_mapGotcha_Item[ info_Item.s_nGroup_Id ] = pObject;
	}
//Gotcha_RareItem
	_read( nHandle, &nCount, sizeof( int ) );

	CsGotcha_RareItem::sINFO info_RareItem;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info_RareItem, sizeof( CsGotcha_RareItem::sINFO ) );

		CsGotcha_RareItem* pObject = csnew CsGotcha_RareItem;
		pObject->Init( &info_RareItem );
		m_mapGotcha_RareItem[ i ] = pObject;
	}

//Gotcha_Mystery_Item
	_read( nHandle, &nCount, sizeof( int ) );

	CsGotcha_Mystery_Item::sINFO info_Mystery_Item;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info_Mystery_Item, sizeof( CsGotcha_Mystery_Item::sINFO ) );

		CsGotcha_Mystery_Item* pObject = csnew CsGotcha_Mystery_Item;
		pObject->Init( &info_Mystery_Item );
		m_mapGotcha_Mystery_Item[ info_Mystery_Item.s_nItem ] = pObject;
	}
//Gotcha_Mystery_Coin
	_read( nHandle, &nCount, sizeof( int ) );

	CsGotcha_Mystery_Coin::sINFO info_Mystery_Coin;
	for( int i=0; i<nCount; ++i )
	{
		_read( nHandle, &info_Mystery_Coin, sizeof( CsGotcha_Mystery_Coin::sINFO ) );

		CsGotcha_Mystery_Coin* pObject = csnew CsGotcha_Mystery_Coin;
		pObject->Init( &info_Mystery_Coin );
		m_mapGotcha_Mystery_Coin[ info_Mystery_Coin.s_nCoinOrder ] = pObject;
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );	
}

CsGotcha_RareItem*	 CsGotchaMng::GetGotcha_RareItem(int nindex )
{
	CsGotcha_RareItem::MAP_IT it = m_mapGotcha_RareItem.find( nindex );
	if( it == m_mapGotcha_RareItem.end() )
		return NULL;
	return it->second;	
}

void CsGotchaMng::SetRareItem(CsGotcha_RareItem* m_Rare, int nIndex )
{
	CsGotcha_RareItem::MAP_IT it = m_mapGotcha_RareItem.find( nIndex );
	if( it == m_mapGotcha_RareItem.end() )
		return;
	
	SAFE_POINTER_RET(it->second);
	CsGotcha_RareItem::sINFO* pInfo = it->second->GetInfo();
	SAFE_POINTER_RET(pInfo);

	SAFE_POINTER_RET( m_Rare );
	CsGotcha_RareItem::sINFO* pRareInfo = m_Rare->GetInfo();
	SAFE_POINTER_RET( pRareInfo );

	pInfo->s_nNpcID = pRareInfo->s_nNpcID;
	pInfo->s_nRareItem = pRareInfo->s_nRareItem;
	pInfo->s_nRareItemCnt = pRareInfo->s_nRareItemCnt;
}

int CsGotchaMng::GetRareItemIndex(int n_Npc_ID, int nItemID)
{
	CsGotcha_RareItem::MAP_IT it = m_mapGotcha_RareItem.begin();
	for( int nIdx = 0; it != m_mapGotcha_RareItem.end(); ++it, ++nIdx )
	{
		SAFE_POINTER_CON( it->second );
		CsGotcha_RareItem::sINFO* sInfo	= it->second->GetInfo();
		SAFE_POINTER_CON( sInfo );

		if( nItemID != sInfo->s_nRareItem )
			continue;
		if( n_Npc_ID != sInfo->s_nNpcID )
			continue;

		return nIdx;
	}

	return -1;
}

DWORD CsGotchaMng::GetGotchaMachineFIDX(DWORD dwNpcID) const
{
	CsGotcha_Machine::MAP::const_iterator it = m_mapGotcha_Machine.begin();
	CsGotcha_Machine::MAP::const_iterator itEnd = m_mapGotcha_Machine.end();
	for( ; it != itEnd; ++it )
	{
		SAFE_POINTER_CON( it->second );
		CsGotcha_Machine::sINFO* pInfo = it->second->GetInfo();
		SAFE_POINTER_CON( pInfo );
		if( dwNpcID == pInfo->s_dwNpc_Id )
			return it->first;
	}

	return -1;
}

bool CsGotchaMng::IsEnableGotchaMachine(DWORD dwFIDX) const
{
	CsGotcha_Machine::MAP::const_iterator it = m_mapGotcha_Machine.find( dwFIDX );
	if( it == m_mapGotcha_Machine.end() )
		return false;

	CsGotcha_Machine* pMachine = it->second;
	SAFE_POINTER_RETVAL( pMachine, false );

	CsGotcha_Machine::sINFO* pMachineInfo = pMachine->GetInfo();
	SAFE_POINTER_RETVAL( pMachineInfo, false );

	return pMachineInfo->s_bLimit;
}

bool CsGotchaMng::IsEnableLevel(DWORD dwFIDX, int nCurLevel) const
{
	CsGotcha_Machine::MAP::const_iterator it = m_mapGotcha_Machine.find( dwFIDX );
	if( it == m_mapGotcha_Machine.end() )
		return false;

	CsGotcha_Machine* pMachine = it->second;
	SAFE_POINTER_RETVAL( pMachine, false );

	CsGotcha_Machine::sINFO* pMachineInfo = pMachine->GetInfo();
	SAFE_POINTER_RETVAL( pMachineInfo, false );

	if( nCurLevel < pMachineInfo->s_nMin_Lv )
		return false;
	if( nCurLevel > pMachineInfo->s_nMax_Lv )
		return false;

	return true;
}

CsGotcha_Machine* CsGotchaMng::GetGotcha_Machine( int n_Npc_ID, int nIndex )
{	
	assert_cs( IsGotcha_NPC( n_Npc_ID ) != true );

	CsGotcha_Machine::MAP_IT it = m_mapGotcha_Machine.find( nIndex );
	if( it == m_mapGotcha_Machine.end() )
		return NULL;

	SAFE_POINTER_RETVAL( it->second, NULL );
	CsGotcha_Machine::sINFO * sInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( sInfo, NULL );
	if( n_Npc_ID != sInfo->s_dwNpc_Id )
		return NULL;

	return it->second;
}

USHORT CsGotchaMng::GetGotcha_Item( int n_ItemID )
{
	CsGotcha_Item::MAP_IT it = m_mapGotcha_Item.begin();
	for( ; it != m_mapGotcha_Item.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		CsGotcha_Item::sINFO * sInfo = it->second->GetInfo();
		SAFE_POINTER_CON( sInfo );
		for( int n = 0; n < MAX_GROUP_ITEM; ++n )
		{
			if( n_ItemID == sInfo->s_dwItem_Code[n] )
				return sInfo->s_nItem_Cnt[n];
		}
	}
	return 0;
}

UINT CsGotchaMng::GetDate(char* nDate )
{
	UINT	rDate = 0;

	char* token = NULL;
	char* separator = "-"; // 토큰을 나누는 구분자 
	token = strtok( nDate, separator ); // 년도
	
	if(token == NULL)//날짜 데이터 없엉
		return NULL;

 	rDate += atoi(token)*10000;
	token = strtok( NULL, separator );	// 월
	rDate += atoi(token)*100;
	token = strtok(NULL, separator);	//일
	rDate += atoi(token);

	return rDate;
}
UINT CsGotchaMng::GetTime(char* nTime )
{
	UINT	rTime = 0;

	char* token = NULL;	
	char* separator = ":"; // 토큰을 나누는 구분자 

	token = strtok( nTime, separator );	// 시
	if(token == NULL)//시간 데이터 없엉
		return NULL;
	rTime += atoi(token)*10000;

	token = strtok( NULL, separator );	// 분
	rTime += atoi(token)*100;

	token = strtok(NULL, separator);	//초
	rTime += atoi(token);

	return rTime;
}

bool CsGotchaMng::GetGotcha_Mystery_IsEffect( int nItem )
{
	CsGotcha_Mystery_Item::MAP_IT it = m_mapGotcha_Mystery_Item.find( nItem );
	if( it == m_mapGotcha_Mystery_Item.end() )
		return false;
	SAFE_POINTER_RETVAL( it->second, false );
	CsGotcha_Mystery_Item::sINFO* sInfo = it->second->GetInfo();
	SAFE_POINTER_RETVAL( sInfo, false );
	if( 1 == sInfo->s_nEffect )
		return true;
	return false;
}

CsGotcha_Mystery_Coin* CsGotchaMng::GetGotcha_Mystery_Coin( int nOrder )
{
	CsGotcha_Mystery_Coin::MAP_IT it = m_mapGotcha_Mystery_Coin.find( nOrder );
	if( it == m_mapGotcha_Mystery_Coin.end() )
		return NULL;
	return it->second;
}

#ifdef EGG_PREFERMENT
// 요구 금액 확인 함수
UINT CsGotchaMng::Get_Req_Money( int nMachineNo )
{
	int nGroupSec = ( nMachineNo * 100 ) - 200;
	
	CsGotcha_Mystery_Coin::MAP_IT it = m_mapGotcha_Mystery_Coin.begin();
	for( ; it != m_mapGotcha_Mystery_Coin.end(); ++it )
	{
		SAFE_POINTER_CON( it->second );
		CsGotcha_Mystery_Coin::sINFO* pInfo = it->second->GetInfo();
		SAFE_POINTER_CON( pInfo );
		if( nGroupSec == pInfo->s_nStartGroupNo )
			return pInfo->s_nMoney;
	}

	return 0;
}
#endif