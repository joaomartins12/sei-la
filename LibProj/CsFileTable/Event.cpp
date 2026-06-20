#include "StdAfx.h"
#include "Event.h"

#define EVENT_BIN		"Event.bin"

CsEventTable::sEVENT::sEVENT():s_nTableNo(0),s_nMinute(0)
{
	memset( s_nItemID, 0, sizeof( int )*EVENT_ITEM_MAXCNT ); 
	memset( s_nItemCnt, 0, sizeof( USHORT )*EVENT_ITEM_MAXCNT ); 
	memset( s_szMessage, 0, sizeof( TCHAR )*EVENT_MSG_LEN ); 
}

CsEventTable::sEVENT::~sEVENT()
{
}

void CsEventTable::Delete()
{
	std::map< int, sEVENT* >::iterator it = m_mapEvent.begin();
	std::map< int, sEVENT* >::iterator itEnd = m_mapEvent.end();

	for( ; it!=itEnd; ++it )
	{		
		delete it->second;
	}

	if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION || 
		nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_ORIGINAL ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH_A )
	{
		std::map< int, sRECOMMENDE* >::iterator it0 = m_mapRecommendE.begin();
		std::map< int, sRECOMMENDE* >::iterator it0End = m_mapRecommendE.end();

		for( ; it0!=it0End; ++it0 )
		{		
			delete it0->second;
		}

		m_mapRecommendE.clear();
	}

	m_mapEvent.clear();		

	std::map< int, sHotTimeEvent* >::iterator itHot = m_mapHotTimeEvent.begin();
	for( ; itHot != m_mapHotTimeEvent.end(); ++itHot )
		delete itHot->second;
	m_mapHotTimeEvent.clear();
}

bool CsEventTable::Init( char* cPath )
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

void CsEventTable::_LoadExcel()
{
	_LoadExcelDaily( ET_DAILY );
	_LoadExcelAttendance( ET_ATTENDANCE );
	_LoadExcelNotLogIn( ET_NOT_LOGIN );
	_LoadExcelPCbang( ET_PCBANG );
	_LoadExcelLevelup( ET_LEVELUP );
	_LoadExcelMonthly(ET_MONTHLY);
	_LoadExcelParcel( ET_DIGIMONPARCEL );
	_LoadExcelHotTime();
	_LoadExcelDailyCheckEvent();

	if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION || 
		nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_ORIGINAL ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH_A ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::THAILAND )
		_LoadExcelFEvent();

}

void CsEventTable::_LoadExcelDaily( DWORD nStartID )
{
#define IL_NO_COL				0

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;

	int	 nStartCol = 0;	
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "Event_Daily"  /*"매일"*/);
	assert_cs( bSuccess == true );

	// 3줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	excel.MoveNext();

	DWORD dwID;	
	while( excel.MoveNext() == true )
	{
		dwID = atoi( excel.GetData( IL_NO_COL + 1, cBuffer ) );
		if( dwID == 0 )
			continue;

		sEVENT* info = csnew sEVENT;
		info->s_nTableNo = atoi( excel.GetData( IL_NO_COL, cBuffer ) ) + nStartID;		

		int nCol = 0;
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			{
				info->s_nMinute = atoi(excel.GetData(13, cBuffer));
				nCol = 16;	//	테이블 변화로 15 컬럼부터 읽는다(아이템수량). 2012/8/9 작성자ID:vf00	 12.9.26. lks007
			}			
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			{
				info->s_nMinute = atoi(excel.GetData(6, cBuffer));
				nCol = 9;
			}			
			break;
		default:
			assert_cs( false );
		}

		for( int i=0; i<EVENT_ITEM_MAXCNT; ++i )
		{
			info->s_nItemID[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 2;
			info->s_nItemCnt[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 1;
		}

		//excel.GetData( nCol, cBuffer );
		//M2W( info->s_szMessage, cBuffer, EVENT_MSG_LEN );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			M2W( info->s_szMessage, excel.GetData( nCol, cBuffer ), EVENT_MSG_LEN );
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			{
//				M2W( info->s_szMessage, excel.GetData( nCol, cBuffer ), EVENT_MSG_LEN );
				//아이템테이블 제한 글자 수 체크 수정 chu8820
				TCHAR szBuffer[ 2048 ];
				M2W( szBuffer, excel.GetData( nCol, cBuffer ), 2048 );
				if( (int)_tcslen( szBuffer ) >= EVENT_MSG_LEN -1 )
					CsMessageBox( MB_OK, L"Event_Daily Number =%d  Message의 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", info->s_nTableNo , (int)_tcslen( szBuffer ), EVENT_MSG_LEN );
				else
					_tcscpy_s( info->s_szMessage, EVENT_MSG_LEN, szBuffer  );
			}
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info->s_szMessage, EVENT_MSG_LEN, excel.GetData( nCol, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		m_mapEvent[ info->s_nTableNo ] = info;
	}	

	excel.Close();
}

void CsEventTable::_LoadExcelAttendance( DWORD nStartID )
{
#define IL_CHECK_COL				3
#define IL_CHECK_ITEM1				6
#define IL_CHECK_ITEM1CNT			8
#define IL_CHECK_ITEM2				9
#define IL_CHECK_ITEM2CNT			11
#define IL_CHECK_ITEM3				12
#define IL_CHECK_ITEM3CNT			14
#define IL_CHECK_MESSAGE			15

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;

	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "Event_Attendance" /*"출석"*/ );		
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	excel.MoveNext();

	excel.MoveNext();
	excel.MoveNext();
	// 시작 시간
	nsCSFILE::Str2Time( excel.GetData( 1, cBuffer ), m_AttendanceInfo.s_StartTime );

	// 종료 시간
	excel.MoveNext();
	nsCSFILE::Str2Time( excel.GetData( 1, cBuffer ), m_AttendanceInfo.s_EndTime );

	bSuccess = excel.MoveFirst();
	excel.MoveNext();

	DWORD dwID;	
	while( excel.MoveNext() == true )
	{
		dwID = atoi( excel.GetData( IL_CHECK_COL + 1, cBuffer ) );
		if( dwID == 0 )
			continue;

		sEVENT* info = csnew sEVENT;

		info->s_nTableNo = atoi( excel.GetData( IL_CHECK_COL, cBuffer ) ) + nStartID;

		int nCol = IL_CHECK_ITEM1;
		for( int i=0; i<3; ++i )
		{
			info->s_nItemID[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 2;
			info->s_nItemCnt[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 1;
		}

		//excel.GetData( IL_CHECK_MESSAGE, cBuffer );
		//M2W( info->s_szMessage, cBuffer, EVENT_MSG_LEN );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info->s_szMessage, excel.GetData( IL_CHECK_MESSAGE, cBuffer ), EVENT_MSG_LEN );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info->s_szMessage, EVENT_MSG_LEN, excel.GetData( IL_CHECK_MESSAGE, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		m_mapEvent[ info->s_nTableNo ] = info;
	}	

	excel.Close();
}

void CsEventTable::_LoadExcelNotLogIn( DWORD nStartID )
{
#define IL_NOCHECK_COL				0
#define IL_NOCHECK_ITEM1			5
#define IL_NOCHECK_ITEM1CNT			7
#define IL_NOCHECK_ITEM2			8
#define IL_NOCHECK_ITEM2CNT			10
#define IL_NOCHECK_ITEM3			11
#define IL_NOCHECK_ITEM3CNT			13
#define IL_NOCHECK_MESSAGE			14

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;

	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "Event_Login_Setting"  /*"장기미접속자"*/ );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	excel.MoveNext();


	DWORD dwID;	
	while( excel.MoveNext() == true )
	{
		dwID = atoi( excel.GetData( IL_NOCHECK_COL + 1, cBuffer ) );
		if( dwID == 0 )
			continue;

		sEVENT* info = csnew sEVENT;
		info->s_nTableNo = atoi( excel.GetData( IL_NOCHECK_COL, cBuffer ) ) + nStartID;

		int nCol = IL_NOCHECK_ITEM1;
		for( int i=0; i<3; ++i )
		{
			info->s_nItemID[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 2;
			info->s_nItemCnt[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 1;
		}

		//excel.GetData( IL_NOCHECK_MESSAGE, cBuffer );
		//M2W( info->s_szMessage, cBuffer, EVENT_MSG_LEN );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info->s_szMessage, excel.GetData( IL_NOCHECK_MESSAGE, cBuffer ), EVENT_MSG_LEN );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info->s_szMessage, EVENT_MSG_LEN, excel.GetData( IL_NOCHECK_MESSAGE, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		m_mapEvent[ info->s_nTableNo ] = info;
	}	

	excel.Close();
}

void CsEventTable::_LoadExcelPCbang( DWORD nStartID )
{
#define IL_PCBANG_COL				0
#define IL_PCBANG_ITEM1				3
#define IL_PCBANG_ITEM1CNT			5
#define IL_PCBANG_ITEM2				6
#define IL_PCBANG_ITEM2CNT			8
#define IL_PCBANG_ITEM3				9
#define IL_PCBANG_ITEM3CNT			11
#define IL_PCBANG_MESSAGE			12

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;

	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "Event_PCroom_Payment"  /*"PC방상품지급"*/ );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	excel.MoveNext();


	DWORD dwID;	
	while( excel.MoveNext() == true )
	{
		dwID = atoi( excel.GetData( IL_PCBANG_COL + 1, cBuffer ) );
		if( dwID == 0 )
			continue;

		sEVENT* info = csnew sEVENT;
		info->s_nTableNo = atoi( excel.GetData( IL_PCBANG_COL, cBuffer ) ) + nStartID;

		int nCol = IL_PCBANG_ITEM1;
		for( int i=0; i<3; ++i )
		{
			info->s_nItemID[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 2;
			info->s_nItemCnt[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 1;
		}

		//excel.GetData( IL_PCBANG_MESSAGE, cBuffer );
		//M2W( info->s_szMessage, cBuffer, EVENT_MSG_LEN );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info->s_szMessage, excel.GetData( IL_PCBANG_MESSAGE, cBuffer ), EVENT_MSG_LEN );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info->s_szMessage, EVENT_MSG_LEN, excel.GetData( IL_PCBANG_MESSAGE, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		m_mapEvent[ info->s_nTableNo ] = info;
	}	

	excel.Close();
}

void CsEventTable::_LoadExcelLevelup( DWORD nStartID )
{
#define IL_LEVELUP_COL				0

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;

	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "Event_Item_Give"  /*"아이템지급이벤트"*/ );
	assert_cs( bSuccess == true );

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	excel.MoveNext();

	DWORD dwID;	
	while( excel.MoveNext() == true )
	{
		dwID = atoi( excel.GetData( IL_LEVELUP_COL + 1, cBuffer ) );
		if( dwID == 0 )
			continue;

		sEVENT* info = csnew sEVENT;
		info->s_nTableNo = atoi( excel.GetData( IL_LEVELUP_COL, cBuffer ) ) + nStartID;

		int nCol = 0;
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			nCol = 16;	//	테이블 변화로 15 컬럼부터 읽는다(아이템수량). 2012/8/9 작성자ID:vf00	12/9/21 lks007
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			nCol = 10;	// 9 -> 10 수정 ( GSP 15.01.27 패치 )
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			nCol = 12;
			break;
		default:
			assert_cs( false );
		}

		int nCount = atoi( excel.GetData( nCol, cBuffer ) );
		++nCol;
		for( int i=0; i<nCount; ++i )
		{
			info->s_nItemID[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 2;
			info->s_nItemCnt[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 1;
		}

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			nCol = 34; //	테이블 변화로 34 컬럼부터 읽는다(메시지). 2012/8/9 작성자ID:vf00	12/9/21 lks007
			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			nCol = 29;	// 28 -> 29 수정 ( GSP 15.01.27 패치 )
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			nCol = 31;
			break;
		default:
			assert_cs( false );
		}
		//M2W( info->s_szMessage, excel.GetData( nCol, cBuffer ), EVENT_MSG_LEN );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			M2W( info->s_szMessage, excel.GetData( nCol, cBuffer ), EVENT_MSG_LEN );
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			wcscpy_s(info->s_szMessage, EVENT_MSG_LEN, excel.GetData( nCol, wBuffer ));
			break;
		default:
			assert_cs( false );
		}

		m_mapEvent[ info->s_nTableNo ] = info;
	}	

	excel.Close();
}

void CsEventTable::_LoadExcelMonthly( DWORD nStartID )
{
	char cBuffer[ CEXCLE_CELL_LEN ];
//	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;

	int	 nStartCol = 0;	
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "Event_Monthly"  /*"매월"*/);
	assert_cs( bSuccess == true );

	// 3줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	excel.MoveNext();

	DWORD dwID;	
	while( excel.MoveNext() == true )
	{
		dwID = atoi( excel.GetData( 1, cBuffer ) );
		if( dwID == 0 )
			continue;

		sMonthlyEvent* info = csnew sMonthlyEvent;
		info->s_nTableNo = atoi( excel.GetData( 0, cBuffer ) ) + nStartID;	
		M2W( info->s_szMessage, excel.GetData( 4, cBuffer ), EVENT_MSG_LEN );

		for( int i=0; i<MONTHLY_ITEM_MAXCNT; ++i )
		{
			info->s_nItemID[i]  = atoi( excel.GetData( 6+(i*3), cBuffer ) );
			info->s_nItemCnt[i] = atoi( excel.GetData( 8+(i*3), cBuffer ) );
		}		

		m_mapMonthlyEvent[ info->s_nTableNo ] = info;
	}	

	excel.Close();
}

void CsEventTable::_LoadExcelParcel( DWORD nStartID )
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "Parcel_Item_Out");
	if( !bSuccess )
		return;

#define IL_START_COL				0
// 	char cBuffer[ CEXCLE_CELL_LEN ];
// 	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	excel.MoveNext();

	while( excel.MoveNext() == true )
	{
		int nUse = 0;
		excel.Read_Value( 1, nUse );// 이벤트 사용 여부
		if( nUse == 0 )
			continue;

		sEVENT* info = csnew sEVENT;
		excel.Read_Value( 0, info->s_nTableNo );
		info->s_nTableNo += nStartID;

		int nCol = 0;
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:		nCol = 12;			break;
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:			nCol = 6;			break;
		default:
			assert_cs( false );
		}

// 		int nCount = 0;
// 		excel.Read_Value( nCol, nCount );
		++nCol;
		for( int i=0; i<EVENT_ITEM_MAXCNT; ++i )
		{
			excel.Read_Value( nCol, info->s_nItemID[ i ] );
			nCol += 2;
			excel.Read_Value( nCol, info->s_nItemCnt[ i ] );
			nCol += 1;
		}

		excel.Read_String( nCol, info->s_szMessage, EVENT_MSG_LEN );
		m_mapEvent.insert( std::make_pair( info->s_nTableNo, info ) );
	}	

	excel.Close();
}

void CsEventTable::_LoadExcelFEvent()
{
#if (defined VERSION_USA || defined VERSION_TH )
	#define EVENT_COL_TYPE						7
	#define EVENT_COL_SUCCESS_TYPE		9
	#define EVENT_COL_SUCCESS_VALUE		10
	#define EVENT_COL_START_TIME			13
	#define EVENT_COL_END_TIME				14
	#define EVENT_COL_ITEM_COUNT			15
	#define EVENT_COL_ITEM						16
#elif (defined VERSION_KOR)
	#define EVENT_COL_TYPE					11
	#define EVENT_COL_SUCCESS_TYPE		12
	#define EVENT_COL_SUCCESS_VALUE		13
	#define EVENT_COL_START_TIME			16
	#define EVENT_COL_END_TIME				17
	#define EVENT_COL_ITEM_COUNT			18
	#define EVENT_COL_ITEM						19
#else
	#define EVENT_COL_TYPE						9
	#define EVENT_COL_SUCCESS_TYPE		10
	#define EVENT_COL_SUCCESS_VALUE		11
	#define EVENT_COL_START_TIME			14
	#define EVENT_COL_END_TIME				15
	#define EVENT_COL_ITEM_COUNT			16
	#define EVENT_COL_ITEM						17
#endif

	char cBuffer[ CEXCLE_CELL_LEN ];
// 	char* szDist;
// 	char szToken[3];
// 	char szBuf[ CEXCLE_CELL_LEN ];
// 	char szYear[ CEXCLE_CELL_LEN ];
// 	char szMonth[ CEXCLE_CELL_LEN ];
// 	char szDay[ CEXCLE_CELL_LEN ];
// 	char szHour[ CEXCLE_CELL_LEN ];
// 	char szMin[ CEXCLE_CELL_LEN ];
// 	char szSec[ CEXCLE_CELL_LEN ];
// 	bool bToken = false;

	cExcelReader excel;

	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "FriendlyEvent" );	

	assert_cs( bSuccess == true );
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );	
	excel.MoveNext();

	DWORD dwID;
	//DWORD dwUse;

	while( excel.MoveNext() == true )
	{
		dwID = atoi( excel.GetData( 1, cBuffer ) );
		if( dwID == 0 )
			continue;

		//	dwUse = atoi( excel.GetData( 2, cBuffer ) );

		//	if( dwUse == 0 ) continue;

		sRECOMMENDE* info = csnew sRECOMMENDE;

		info->s_nTableNo = atoi( excel.GetData( 0, cBuffer ) );
		info->s_nIndex = atoi( excel.GetData( 1, cBuffer ) );
		info->s_nUse = atoi( excel.GetData( 2, cBuffer ) );
		info->s_nType = atoi( excel.GetData( EVENT_COL_TYPE, cBuffer ) );
		info->s_nSuccessType = atoi( excel.GetData( EVENT_COL_SUCCESS_TYPE, cBuffer ) );
		info->s_nSuccessValue = atoi( excel.GetData( EVENT_COL_SUCCESS_VALUE, cBuffer ) );

		// 친구추천 기간 체크 수정 chu8820
		//시작, 종료시간
		M2W( info->s_cStartTime, excel.GetData( EVENT_COL_START_TIME, cBuffer ), 32 );
		M2W( info->s_cEndTime, excel.GetData( EVENT_COL_END_TIME, cBuffer ), 32 );

		info->s_nItemKind = atoi( excel.GetData( EVENT_COL_ITEM_COUNT, cBuffer) );

		int nCol = EVENT_COL_ITEM;

		for( int i=0; i<3; ++i )
		{
			info->s_nItemID[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 2;
			info->s_nItemCnt[ i ] = atoi( excel.GetData( nCol, cBuffer ) );
			nCol += 1;
		}

		m_mapRecommendE[ info->s_nTableNo ] = info;
	}	

	excel.Close();
}

void CsEventTable::_LoadExcelHotTime()
{
	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "HotTimeRewardEvent" );
	if( !bSuccess )
		return;

	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );
	excel.MoveNext();

	while ( excel.MoveNext() == true )
	{
		int nEventNo = 0;
		excel.Read_Value( 0, nEventNo );
		if( 0 == nEventNo )
			break;

		sHotTimeEvent* pInfo = csnew sHotTimeEvent;
		pInfo->s_nEventNo = nEventNo; 
		excel.Read_Value( 1, pInfo->s_szStartDate );
		excel.Read_Value( 2, pInfo->s_szEndDate );
		excel.Read_Value( 3, pInfo->s_nDay );
		excel.Read_Value( 4, pInfo->s_szStartTime );
		excel.Read_Value( 5, pInfo->s_szEndTime );
		excel.Read_Value( 6, pInfo->s_nItemNo );
		excel.Read_Value( 8, pInfo->s_nItemCnt );

		m_mapHotTimeEvent.insert( std::make_pair( pInfo->s_nEventNo, pInfo ) );
	}

	excel.Close();
}

bool CsEventTable::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, EVENT_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );		
		return false;
	}

	fread( &m_AttendanceInfo, sizeof( sATTENDANCE_INFO ), 1, fp );

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );	
	for( int j = 0; j < nCount; j++ )
	{						
		sEVENT* sInfo = csnew sEVENT;
		fread( sInfo, sizeof( sEVENT ), 1, fp );						

		m_mapEvent[ sInfo->s_nTableNo ] = sInfo;	
	}

	if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION || 
		nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_ORIGINAL ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH_A ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::THAILAND )
	{
		fread( &nCount, sizeof( int ), 1, fp );
		for( int j = 0; j < nCount; j++ )
		{
			sRECOMMENDE* sInfo = csnew sRECOMMENDE;
			fread( sInfo, sizeof( sRECOMMENDE ), 1, fp );

			m_mapRecommendE[ j ] = sInfo;
		}
	}

	nCount = 0;
	fread( &nCount, sizeof( int ), 1, fp );	
	for( int j = 0; j < nCount; j++ )
	{						
		sMonthlyEvent* sInfo = csnew sMonthlyEvent;
		fread( sInfo, sizeof( sMonthlyEvent ), 1, fp );						

		m_mapMonthlyEvent[ sInfo->s_nTableNo ] = sInfo;	
	}

	nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i = 0; i < nCount; ++i)
	{
		int nEventNo = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nEventNo );
		if( 0 == nEventNo )
			continue;

		sHotTimeEvent* pInfo = csnew sHotTimeEvent;	

		pInfo->s_nEventNo = nEventNo;
		
		CmUtil::CmBinRead::Read_Value( fp, pInfo->s_szStartDate );
		CmUtil::CmBinRead::Read_Value( fp, pInfo->s_szEndDate );
		CmUtil::CmBinRead::Read_Value( fp, &pInfo->s_nDay );
		CmUtil::CmBinRead::Read_Value( fp, pInfo->s_szStartTime );
		CmUtil::CmBinRead::Read_Value( fp, pInfo->s_szEndTime );
		CmUtil::CmBinRead::Read_Value( fp, &pInfo->s_nItemNo );
		CmUtil::CmBinRead::Read_Value( fp, &pInfo->s_nItemCnt );

		m_mapHotTimeEvent.insert( std::make_pair( pInfo->s_nEventNo, pInfo ) );
	}

	_LoadBinDailyCheckEvent( fp );

	fclose( fp );

	return true;	
}

void CsEventTable::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, EVENT_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	_read( nHandle, &m_AttendanceInfo, sizeof( sATTENDANCE_INFO ) );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );	
	for( int j = 0; j < nCount; j++ )
	{	
		sEVENT* sInfo = csnew sEVENT;
		_read( nHandle, sInfo, sizeof( sEVENT ) );

		m_mapEvent[ sInfo->s_nTableNo ] = sInfo;	
	}

	if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION || 
		nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_ORIGINAL ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH_A ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::THAILAND )
	{
		_read( nHandle, &nCount, sizeof( int ) );
		for( int j = 0; j < nCount; j++ )
		{
			sRECOMMENDE* sInfo = csnew sRECOMMENDE;
			_read( nHandle, sInfo, sizeof( sRECOMMENDE ) );

			m_mapRecommendE[ sInfo->s_nTableNo ] = sInfo;
		}
	}

	nCount = 0;
	_read( nHandle, &nCount, sizeof( int ) );	
	for( int j = 0; j < nCount; j++ )
	{						
		sMonthlyEvent* sInfo = csnew sMonthlyEvent;
		_read( nHandle, sInfo, sizeof( sMonthlyEvent ) );						

		m_mapMonthlyEvent[ sInfo->s_nTableNo ] = sInfo;	
	}

	nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i = 0; i < nCount; ++i)
	{
		int nEventNo = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nEventNo );
		if( 0 == nEventNo )
			continue;

		sHotTimeEvent* pInfo = csnew sHotTimeEvent;

		pInfo->s_nEventNo = nEventNo;

		CmUtil::CmPackRead::Read_Value( nHandle, pInfo->s_szStartDate );
		CmUtil::CmPackRead::Read_Value( nHandle, pInfo->s_szEndDate );
		CmUtil::CmPackRead::Read_Value( nHandle, &pInfo->s_nDay );
		CmUtil::CmPackRead::Read_Value( nHandle, pInfo->s_szStartTime );
		CmUtil::CmPackRead::Read_Value( nHandle, pInfo->s_szEndTime );
		CmUtil::CmPackRead::Read_Value( nHandle, &pInfo->s_nItemNo );
		CmUtil::CmPackRead::Read_Value( nHandle, &pInfo->s_nItemCnt );

		m_mapHotTimeEvent.insert( std::make_pair( pInfo->s_nEventNo, pInfo ) );
	}

	_LoadFilePackDailyCheckEvent( nHandle );

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

void CsEventTable::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, EVENT_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	fwrite( &m_AttendanceInfo, sizeof( sATTENDANCE_INFO ), 1, fp );

	int nCount = (int)m_mapEvent.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< int, sEVENT* >::iterator it = m_mapEvent.begin();
	std::map< int, sEVENT* >::iterator itEnd = m_mapEvent.end();

	for( ; it != itEnd; it++ )
	{	
		fwrite( it->second, sizeof( sEVENT ), 1, fp );								
	}

	if( nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_TRANSLATION || 
		nsCsFileTable::g_eLanguage == nsCsFileTable::KOREA_ORIGINAL ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::ENGLISH_A ||
		nsCsFileTable::g_eLanguage == nsCsFileTable::THAILAND )
	{
		nCount = (int)m_mapRecommendE.size();
		fwrite( &nCount, sizeof( int ), 1, fp );

		std::map< int, sRECOMMENDE* >::iterator it0 = m_mapRecommendE.begin();
		std::map< int, sRECOMMENDE* >::iterator it0End = m_mapRecommendE.end();

		for( ; it0 != it0End; it0++ )
		{	
			fwrite( it0->second, sizeof( sRECOMMENDE ), 1, fp );								
		}
	}

	nCount = (int)m_mapMonthlyEvent.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< int, sMonthlyEvent* >::iterator itM = m_mapMonthlyEvent.begin();
	std::map< int, sMonthlyEvent* >::iterator itMEnd = m_mapMonthlyEvent.end();

 	for( ; itM != itMEnd; itM++ )
 	{	
 		fwrite( itM->second, sizeof( sMonthlyEvent ), 1, fp );								
 	}

	nCount = (int)m_mapHotTimeEvent.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	std::map< int, sHotTimeEvent* >::iterator itHot = m_mapHotTimeEvent.begin();
	std::map< int, sHotTimeEvent* >::iterator itHotEnd = m_mapHotTimeEvent.end();
	for( ; itHot != itHotEnd; ++itHot )
	{
		sHotTimeEvent* pEvent = itHot->second;
		
		CmUtil::CmBinSave::Save_Value( fp, &pEvent->s_nEventNo );
		CmUtil::CmBinSave::Save_Value( fp, pEvent->s_szStartDate );
		CmUtil::CmBinSave::Save_Value( fp, pEvent->s_szEndDate );
		CmUtil::CmBinSave::Save_Value( fp, &pEvent->s_nDay );
		CmUtil::CmBinSave::Save_Value( fp, pEvent->s_szStartTime );
		CmUtil::CmBinSave::Save_Value( fp, pEvent->s_szEndTime );
		CmUtil::CmBinSave::Save_Value( fp, &pEvent->s_nItemNo );
		CmUtil::CmBinSave::Save_Value( fp, &pEvent->s_nItemCnt );
	}

	_SaveBinDailyCheckEvent( fp );

	fclose( fp );
}

int CsEventTable::GetRecommendTableCount() const
{
	 return (int)m_mapRecommendE.size();
}

bool CsEventTable::IsEvent( int nType, int nNO ) const
{ 
	return m_mapEvent.find( nType + nNO ) != m_mapEvent.end(); 
}

CsEventTable::sEVENT* CsEventTable::GetMap( int nType, int nNO )
{ 
	return IsEvent( nType, nNO ) ? m_mapEvent[ nType + nNO ] : NULL; 
}

CsEventTable::sMonthlyEvent* CsEventTable::GetMapMonthly( int nType, int nNO )
{
	return m_mapMonthlyEvent[ nType + nNO ];
}

bool CsEventTable::IsRecommendE(int nNO ) const
{ 
	return m_mapRecommendE.find( nNO ) != m_mapRecommendE.end(); 
}

CsEventTable::sRECOMMENDE* CsEventTable::GetMap( int nNO ) 
{ 
	return IsRecommendE( nNO ) ? m_mapRecommendE[ nNO ] : NULL; 
}

CsEventTable::sATTENDANCE_INFO* CsEventTable::GetAttendanceInfo()
{ 
	return &m_AttendanceInfo; 
}

CsEventTable::sHotTimeEvent const* CsEventTable::GetHotTime( int nEventNo ) const
{
	std::map< int, sHotTimeEvent* >::const_iterator it = m_mapHotTimeEvent.find(nEventNo);
	if( it != m_mapHotTimeEvent.end() )
		return it->second;
	return NULL;
}

//////////////////////////////////////////////////////////////////////////
// 출석부 이벤트
void CsEventTable::_LoadExcelDailyCheckEvent()
{
	cExcelReader excel;

	// 출석부 목록
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "DailyCheckEvent_Time" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
		return;

	while ( excel.MoveNext() == true )
	{
		UINT nGroupNo = 0;
		excel.Read_Value( 0, nGroupNo );
		if( 0 == nGroupNo )
			break;

		sDailyCheckTime sDaily;
		excel.Read_Value( 1, sDaily.wsEventTab );
		excel.Read_Value( 2, sDaily.wsEventTitle );
		excel.Read_Value( 3, sDaily.wsEventDescript );
		excel.Read_Value( 4, sDaily.wsStartTime );
		excel.Read_Value( 5, sDaily.wsEndTime );
		excel.Read_Value( 6, sDaily.wsResetTime );

		m_mapDailyCheck.insert( std::make_pair( nGroupNo, sDaily ) );
	}
	excel.Close();

	// 출석부 아이템 보상 목록
	bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Event(), "DailyCheckEvent_Item" );
	if( !bSuccess )
		return;

	bSuccess = excel.FirstOfSkip( 2 );
	if( !bSuccess )
		return;

	while ( excel.MoveNext() == true )
	{
		UINT nGroupNo = 0;
		excel.Read_Value( 0, nGroupNo );
		if( 0 == nGroupNo )
			break;

		MAP_DAILYCHECK_IT it = m_mapDailyCheck.find( nGroupNo );
		if( it == m_mapDailyCheck.end() )
			continue;

		sDailyCheckItem sItem;
		excel.Read_Value( 1, sItem.wsCheckData );
		excel.Read_Value( 3, sItem.nRewardItemID );
		excel.Read_Value( 5, sItem.nRewardItemCnt );

		it->second.lDailyRewards.push_back( sItem );
	}

	excel.Close();
}

void CsEventTable::_LoadBinDailyCheckEvent(FILE* fp)
{
	SAFE_POINTER_RET( fp );

	int nCount = 0;
	CmUtil::CmBinRead::Read_Value( fp, &nCount );
	for( int i = 0; i < nCount; ++i)
	{
		UINT nGroupNo = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nGroupNo );
		if( 0 == nGroupNo )
			continue;

		MAP_DAILYCHECK_IT it = m_mapDailyCheck.find( nGroupNo );
		if( it == m_mapDailyCheck.end() )
		{
			auto ib = m_mapDailyCheck.insert( std::make_pair( nGroupNo, sDailyCheckTime() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "DailyCheckItem insertion failure." ) );
				continue;
			}
			it = ib.first;
		}
		CmUtil::CmBinRead::Read_Value( fp, it->second.wsEventTab );
		CmUtil::CmBinRead::Read_Value( fp, it->second.wsEventTitle );
		CmUtil::CmBinRead::Read_Value( fp, it->second.wsEventDescript );
		CmUtil::CmBinRead::Read_Value( fp, it->second.wsStartTime );
		CmUtil::CmBinRead::Read_Value( fp, it->second.wsEndTime );
		CmUtil::CmBinRead::Read_Value( fp, it->second.wsResetTime );

		int nListCount = 0;
		CmUtil::CmBinRead::Read_Value( fp, &nListCount );
		for( int j = 0; j < nListCount; ++j )
		{
			sDailyCheckItem sItem;
			CmUtil::CmBinRead::Read_Value( fp, sItem.wsCheckData );
			CmUtil::CmBinRead::Read_Value( fp, &sItem.nRewardItemID );
			CmUtil::CmBinRead::Read_Value( fp, &sItem.nRewardItemCnt );
			it->second.lDailyRewards.push_back( sItem );
		}
	}
}

void CsEventTable::_LoadFilePackDailyCheckEvent(int nHandle)
{
	int nCount = 0;
	CmUtil::CmPackRead::Read_Value( nHandle, &nCount );
	for( int i = 0; i < nCount; ++i)
	{
		UINT nGroupNo = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nGroupNo );
		if( 0 == nGroupNo )
			continue;

		MAP_DAILYCHECK_IT it = m_mapDailyCheck.find( nGroupNo );
		if( it == m_mapDailyCheck.end() )
		{
			auto ib = m_mapDailyCheck.insert( std::make_pair( nGroupNo, sDailyCheckTime() ) );
			if( !ib.second )
			{
				assert_csm( false, _T( "DailyCheckItem insertion failure." ) );
				continue;
			}
			it = ib.first;
		}
		CmUtil::CmPackRead::Read_Value( nHandle, it->second.wsEventTab );
		CmUtil::CmPackRead::Read_Value( nHandle, it->second.wsEventTitle );
		CmUtil::CmPackRead::Read_Value( nHandle, it->second.wsEventDescript );
		CmUtil::CmPackRead::Read_Value( nHandle, it->second.wsStartTime );
		CmUtil::CmPackRead::Read_Value( nHandle, it->second.wsEndTime );
		CmUtil::CmPackRead::Read_Value( nHandle, it->second.wsResetTime );

		int nListCount = 0;
		CmUtil::CmPackRead::Read_Value( nHandle, &nListCount );
		for( int j = 0; j < nListCount; ++j )
		{
			sDailyCheckItem sItem;
			CmUtil::CmPackRead::Read_Value( nHandle, sItem.wsCheckData );
			CmUtil::CmPackRead::Read_Value( nHandle, &sItem.nRewardItemID );
			CmUtil::CmPackRead::Read_Value( nHandle, &sItem.nRewardItemCnt );
			it->second.lDailyRewards.push_back( sItem );
		}
	}
}

void CsEventTable::_SaveBinDailyCheckEvent(FILE* fp)
{
	SAFE_POINTER_RET( fp );

	int nCount = m_mapDailyCheck.size();
	CmUtil::CmBinSave::Save_Value( fp, &nCount );

	MAP_DAILYCHECK_IT it = m_mapDailyCheck.begin();
	for( ; it != m_mapDailyCheck.end(); ++it )
	{
		CmUtil::CmBinSave::Save_Value( fp, &it->first );
		CmUtil::CmBinSave::Save_Value( fp, it->second.wsEventTab );
		CmUtil::CmBinSave::Save_Value( fp, it->second.wsEventTitle );
		CmUtil::CmBinSave::Save_Value( fp, it->second.wsEventDescript );
		CmUtil::CmBinSave::Save_Value( fp, it->second.wsStartTime );
		CmUtil::CmBinSave::Save_Value( fp, it->second.wsEndTime );
		CmUtil::CmBinSave::Save_Value( fp, it->second.wsResetTime );

		int nListCount = it->second.lDailyRewards.size();
		CmUtil::CmBinSave::Save_Value( fp, &nListCount );
		LIST_DCHECK_ITEM_IT itItem = it->second.lDailyRewards.begin();
		for( ; itItem != it->second.lDailyRewards.end(); ++itItem )
		{
			CmUtil::CmBinSave::Save_Value( fp, itItem->wsCheckData );
			CmUtil::CmBinSave::Save_Value( fp, &itItem->nRewardItemID );
			CmUtil::CmBinSave::Save_Value( fp, &itItem->nRewardItemCnt );
		}
	}
}

CsEventTable::sDailyCheckTime const* CsEventTable::GetDailyCheck(UINT const& nGroupNo) const
{
	MAP_DAILYCHECK_CIT cit = m_mapDailyCheck.find( nGroupNo );
	if( cit == m_mapDailyCheck.end() )
		return NULL;
	return &cit->second;
}

CsEventTable::LIST_DCHECK_ITEM const* CsEventTable::GetDailyCheckRewardList(UINT const& nGroupNo) const
{
	sDailyCheckTime const* pDailyCheckInfo = GetDailyCheck( nGroupNo );
	SAFE_POINTER_RETVAL( pDailyCheckInfo, NULL );

	return &pDailyCheckInfo->lDailyRewards;
}
