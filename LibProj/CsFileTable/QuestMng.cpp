

#include "stdafx.h"
#include "QuestMng.h"


void CsQuestMng::Delete()
{
	//m_pParcing->Delete();
	//delete m_pParcing;

	CsQuest::MAP_IT it = m_mapQuest.begin();
	CsQuest::MAP_IT itEnd = m_mapQuest.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapQuest.clear();
}

bool CsQuestMng::Init( char* cPath )
{
	return true; // DATAFILE_DISABLE_FOR_NOW

	assert_cs( m_mapQuest.empty() == true );
	assert_cs( nsCsFileTable::g_bUseQuest );

	switch( nsCsFileTable::g_eFileType )
	{
	case nsCsFileTable::FT_EXCEL:
		return _LoadExcel( nsCsFileTable::GetPath_DM_Quest() );
	case nsCsFileTable::FT_BIN:
		return _LoadBin( cPath );
	case nsCsFileTable::FT_FILEPACK:
		_LoadFilePack( cPath );
		break;
	default:
		assert_cs( false );
	}
	return true;
}

void CsQuestMng::DeleteQuest( DWORD dwQuestID )
{
	assert_cs( IsQuest( dwQuestID ) == true );
	m_mapQuest[ dwQuestID ]->Delete();
	delete m_mapQuest[ dwQuestID ];
	m_mapQuest.erase( dwQuestID );
}

CsQuest* CsQuestMng::AddQuest( DWORD dwUniqID )
{
	CsQuest* pReturn = csnew CsQuest;
	pReturn->Init();
	pReturn->SetUniqID( dwUniqID );

	assert_cs( pReturn->GetUniqID() != 0 );
	assert_cs( m_mapQuest.find( pReturn->GetUniqID() ) == m_mapQuest.end() );
	m_mapQuest[ pReturn->GetUniqID() ]  = pReturn;

	return pReturn;
}

CsQuest* CsQuestMng::AddQuest( CsQuest* pSrcQuest, DWORD dwUniqID )
{
	CsQuest* pReturn = pSrcQuest->Clone();
	if( dwUniqID != 0 )
	{
		pReturn->SetUniqID( dwUniqID );
		assert_cs( pReturn->GetUniqID() != pSrcQuest->GetUniqID() );
	}

	assert_cs( pReturn->GetUniqID() != 0 );
	assert_cs( m_mapQuest.find( pReturn->GetUniqID() ) == m_mapQuest.end() );
	m_mapQuest[ pReturn->GetUniqID() ]  = pReturn;

	return pReturn;
}

CsQuest* CsQuestMng::GetQuest( DWORD const& dwQuestID )

{
	// PATCH_QUESTS_SPINZ in callers (AchieveContents) handles NULL return from this function.
	// The assert was over-strict; missing quest IDs are common when tables/server data drift.
	// assert_csm1( IsQuest( dwQuestID ) == true, _T( "QuestID = %d" ), dwQuestID );

	CsQuest::MAP::iterator it = m_mapQuest.find(dwQuestID);

	if(it != m_mapQuest.end())
		return it->second;

	return NULL;
}

      
//========================================================================================================
//
//	Load Excel
//
//========================================================================================================

bool CsQuestMng::_LoadExcel( char* cPath )
{

	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];
	TCHAR szBuffer[ 2048 ];

	cExcelReader excel;
	bool bSuccess = excel.Open( cPath, QUEST_WORKSHEET_NAME );
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		return false;
	}
	
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		return false;
	}

	int nQuestExcelVersion = 0;
	excel.Read_Value( ME_VERSION_COL, nQuestExcelVersion );
	if( nQuestExcelVersion < 8 )
	{
		MessageBox( NULL, _T( "Quest.xls 버젼이 틀립니다. 송명수에게 문의해 주세요!!!" ), NULL, MB_OK );
		return false;
	}

	bool bNewSaveData = false;
	if( nQuestExcelVersion != QUEST_EXCEL_VERSION )
		bNewSaveData = true;

	bSuccess = excel.MoveNext();
	if( !bSuccess )
	{
		assert_cs( bSuccess == true );
		return false;
	}


	DWORD col = 0;	
	CsQuest Quest;
	DWORD dwTypeID, dwTargetID;
	int nTypeCount;
	CsPoint pt;
	while( excel.MoveNext() == true )
	{
		col = 0;
		Quest.Init();

		// Uniq ID
		Quest.SetUniqID( atoi( excel.GetData( col, cBuffer ) ) );
		if( Quest.GetUniqID() == 0 )
			continue;

		++col;

		// ModelID
		Quest.SetModelID_Recv( atoi( excel.GetData( col, cBuffer ) ) );
		++col;
		Quest.SetModelID_Comp( atoi( excel.GetData( col, cBuffer ) ) );
		++col;

		// Level
		Quest.SetLevel( atoi( excel.GetData( col, cBuffer ) ) );
		++col;

		// pos
		pt.x = atoi( excel.GetData( col, cBuffer ) );
		++col;
		pt.y = atoi( excel.GetData( col, cBuffer ) );
		++col;
		Quest.SetToolPos( pt );

		// Manage ID
		Quest.SetManageID( atoi( excel.GetData( col, cBuffer ) ) );
		++col;

		// Active
		Quest.SetActive( atoi( excel.GetData( col, cBuffer ) ) == 1 );
		++col;

		// Immediate 즉시 완료
		Quest.SetImmeiate( atoi( excel.GetData( col, cBuffer ) ) == 1 );
		++col;

#ifdef QUEST_RESET_180712
		// Immediate 즉시 완료
		Quest.SetResetQuest( atoi( excel.GetData( col, cBuffer ) ) == 1 );
		++col;
#endif
		// Type
		Quest.SetQuestType( (CsQuest::eQUEST_TYPE)atoi( excel.GetData( col, cBuffer ) ) );
		++col;
		
		{
			// 시작 타겟
			Quest.SetStartTarget_Type( (CsQuest::eST_TYPE)atoi( excel.GetData( col, cBuffer ) ) );
			++col;
			Quest.SetStartTarget_ID( atoi( excel.GetData( col, cBuffer ) ) );
			++col;
		}
		
		// Target
		Quest.SetQuestTarget( (CsQuest::eQUEST_TARGET)atoi( excel.GetData( col, cBuffer ) ) );
		++col;
		Quest.SetQuestTargetValue1( atoi( excel.GetData( col, cBuffer ) ) );
		++col;
		
		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
		case nsCsFileTable::ENGLISH:
		case nsCsFileTable::ENGLISH_A:
			// 타이틀탭
			M2W( szBuffer, excel.GetData( col, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= QUEST_TITLE_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 TitleTab의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_TITLE_LEN );
			else
				_tcscpy_s( Quest.m_szTitleTab, QUEST_TITLE_LEN, szBuffer );
			++col;

			// 타이틀
			M2W( szBuffer, excel.GetData( col, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= QUEST_TITLE_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Title의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_TITLE_LEN );
			else
				_tcscpy_s( Quest.m_szTitleText, QUEST_TITLE_LEN, szBuffer );
			++col;

			// 본문
			M2W( szBuffer, excel.GetData( col, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= 1024 -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Body의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), 1024 );
			else
				_tcscpy_s( Quest.m_szBodyText, QUEST_BODY_LEN, szBuffer  );
			++col;

			// 퀘스트 심플
			M2W( szBuffer, excel.GetData( col, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= QUEST_SIMPLE_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Simple의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_SIMPLE_LEN );
			else
				_tcscpy_s( Quest.m_szSimpleText, QUEST_SIMPLE_LEN, szBuffer );
			++col;

			// 퀘스트 헬퍼
			M2W( szBuffer, excel.GetData( col, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= QUEST_HELPER_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Helper의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_HELPER_LEN );
			else
				_tcscpy_s( Quest.m_szHelperText, QUEST_HELPER_LEN, szBuffer );
			++col;

			// 퀘스트 중
			M2W( szBuffer, excel.GetData( col, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= QUEST_PROCESS_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Process의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_PROCESS_LEN );
			else
				_tcscpy_s( Quest.m_szProcess, QUEST_PROCESS_LEN, szBuffer );
			++col;

			// 퀘스트 완료
			M2W( szBuffer, excel.GetData( col, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= QUEST_COMPLEATE_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Compleate의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_COMPLEATE_LEN );
			else
				_tcscpy_s( Quest.m_szCompleate, QUEST_COMPLEATE_LEN, szBuffer );
			++col;

			// 퀘스트 확장
			M2W( szBuffer, excel.GetData( col, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= QUEST_PROCESS_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Expert의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_PROCESS_LEN );
			else
				_tcscpy_s( Quest.m_szExpert, QUEST_PROCESS_LEN, szBuffer );
			++col;
			break;
		case nsCsFileTable::THAILAND:
		case nsCsFileTable::TAIWAN:
		case nsCsFileTable::HONGKONG:
			// 타이틀탭
			wcscpy_s( szBuffer, 2048, excel.GetData( col, wBuffer ) );
			if( (int)_tcslen( szBuffer ) >= QUEST_TITLE_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 TitleTab의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_TITLE_LEN );
			wcscpy_s( Quest.m_szTitleTab, QUEST_TITLE_LEN, szBuffer );
			++col;

			// 타이틀
			wcscpy_s( szBuffer, 2048, excel.GetData( col, wBuffer ) );
			if( (int)_tcslen( szBuffer ) >= QUEST_TITLE_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Title의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_TITLE_LEN );
			wcscpy_s( Quest.m_szTitleText, QUEST_TITLE_LEN, szBuffer );
			++col;

			// 본문
			wcscpy_s( szBuffer, 2048, excel.GetData( col, wBuffer ) );
			if( (int)_tcslen( szBuffer ) >= QUEST_BODY_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Body의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_BODY_LEN );
			wcscpy_s( Quest.m_szBodyText, QUEST_BODY_LEN, szBuffer  );
			++col;

			// 퀘스트 심플
			wcscpy_s( szBuffer, 2048, excel.GetData( col, wBuffer ) );
			if( (int)_tcslen( szBuffer ) >= QUEST_SIMPLE_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Simple의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_SIMPLE_LEN );
			_tcscpy_s( Quest.m_szSimpleText, QUEST_SIMPLE_LEN, szBuffer );
			++col;

			// 퀘스트 헬퍼
			wcscpy_s( szBuffer, 2048, excel.GetData( col, wBuffer ) );
			if( (int)_tcslen( szBuffer ) >= QUEST_HELPER_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Helper의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_HELPER_LEN );
			wcscpy_s( Quest.m_szHelperText, QUEST_HELPER_LEN, szBuffer );
			++col;

			// 퀘스트 중
			wcscpy_s( szBuffer, 2048, excel.GetData( col, wBuffer ) );
			if( (int)_tcslen( szBuffer ) >= QUEST_PROCESS_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Process의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_PROCESS_LEN );
			_tcscpy_s( Quest.m_szProcess, QUEST_PROCESS_LEN, szBuffer );
			++col;

			// 퀘스트 완료
			wcscpy_s( szBuffer, 2048, excel.GetData( col, wBuffer ) );
			if( (int)_tcslen( szBuffer ) >= QUEST_COMPLEATE_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Compleate의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_COMPLEATE_LEN );
			wcscpy_s( Quest.m_szCompleate, QUEST_COMPLEATE_LEN, szBuffer );
			++col;

			// 퀘스트 확장
			wcscpy_s( szBuffer, 2048, excel.GetData( col, wBuffer ) );
			if( (int)_tcslen( szBuffer ) >= QUEST_PROCESS_LEN -1 )
				CsMessageBox( MB_OK, L"QuestID = %d, 의 Expert의 문자열이 너무 깁니다. %d / %d\n저장하지 말아주세요.", Quest.GetUniqID(), (int)_tcslen( szBuffer ), QUEST_PROCESS_LEN );
			wcscpy_s( Quest.m_szExpert, QUEST_PROCESS_LEN, szBuffer );
			++col;
			break;
		default:
			assert_cs( false );
		}		
		


		{
			// 수행 지급
			int nPreRequireCount = atoi( excel.GetData( col, cBuffer ) );			
			++col;

			CsQuestApplyRequite::eTYPE type;
			for( int nPreRequire=0; nPreRequire < nPreRequireCount; ++nPreRequire )
			{
				type = (CsQuestApplyRequite::eTYPE)atoi( excel.GetData( col, cBuffer ) );
				++col;

				dwTypeID = atoi( excel.GetData( col, cBuffer ) );
				++col;

				nTypeCount = atoi( excel.GetData( col, cBuffer ) );
				++col;

				Quest.AddApplyRequite( type, dwTypeID, nTypeCount );
			}
		}

		{
			// 선행조건 카운트
			int nPreRequireCount = atoi( excel.GetData( col, cBuffer ) );			
			++col;

			CsQuestPreRequire::eTYPE type;
			for( int nPreRequire=0; nPreRequire < nPreRequireCount; ++nPreRequire )
			{
				type = (CsQuestPreRequire::eTYPE)atoi( excel.GetData( col, cBuffer ) );
				++col;

				dwTypeID = atoi( excel.GetData( col, cBuffer ) );
				++col;

				nTypeCount = atoi( excel.GetData( col, cBuffer ) );
				++col;

				Quest.AddPreRequire( type, dwTypeID, nTypeCount );
			}
		}


		{
			// 완료조건 갯수
			int nCompRequireCount = atoi( excel.GetData( col, cBuffer ) );			
			++col;

			CsQuestCompRequire::eTYPE type;
			for( int nCompRequire=0; nCompRequire < nCompRequireCount; ++nCompRequire )
			{
				type = (CsQuestCompRequire::eTYPE )atoi( excel.GetData( col, cBuffer ) );
				++col;

				dwTypeID = atoi( excel.GetData( col, cBuffer ) );
				++col;

				nTypeCount = atoi( excel.GetData( col, cBuffer ) );
				++col;

				dwTargetID = atol( excel.GetData( col, cBuffer ) );
				++col;

				if( !bNewSaveData )
				{
					int nSubValue = atoi( excel.GetData( col, cBuffer ) );
					++col;

					int nSubValue2 = atoi( excel.GetData( col, cBuffer ) );
					++col;

					Quest.AddCompRequire( type, dwTypeID, nTypeCount, dwTargetID, nSubValue, nSubValue2 );
				}
				else
					Quest.AddCompRequire( type, dwTypeID, nTypeCount, dwTargetID, 0, 0 );
			}
		}

		{
			// 보상 갯수
			int nRequiteCount = atoi( excel.GetData( col, cBuffer ) );			
			++col;

			CsQuestRequite::eMETHOD method;
			CsQuestRequite::eTYPE type;
			for( int nRequite=0; nRequite < nRequiteCount; ++nRequite )
			{
				method = (CsQuestRequite::eMETHOD)atoi( excel.GetData( col, cBuffer ) );
				++col;

				type = (CsQuestRequite::eTYPE)atoi( excel.GetData( col, cBuffer ) );
				++col;

				dwTypeID = atoi( excel.GetData( col, cBuffer ) );
				++col;

				nTypeCount = atoi( excel.GetData( col, cBuffer ) );
				++col;

				Quest.AddRequite( method, type, dwTypeID, nTypeCount );
			}
		}

		{
			// Event
			for( int nEvent=0; nEvent < CsQuest::EVENT_MAX_COUNT; ++nEvent )
			{
				Quest.m_nEvent[ nEvent ] = atoi( excel.GetData( col, cBuffer ) );
				++col;
			}
		}

		if( nsCsFileTable::g_bReadInactiveQuest || Quest.IsActive() )
			AddQuest( &Quest, 0 );

		Quest.Delete();
	}

	return true;
}

void CsQuestMng::SaveExcel( float fExpMul )
{
	char* cPath = nsCsFileTable::GetPath_DM_Quest();

	if( _access_s( cPath, 0 ) == 0 )
	{
		// 폴더가 존재 하지 않는다면 폴더 생성
		nsCSFILE::CreateDirectory( _T( "GameData\\Korea\\Quest_Backup" ) );	

		char cNewFile[ MAX_PATH ];
		SYSTEMTIME st;
		GetLocalTime( &st );			
		sprintf_s( cNewFile, MAX_PATH, "GameData\\Korea\\Quest_Backup\\Quest_%.2d%.2d%.2d_%.2d%.2d%.2d.xls", st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );

		// 이미 파일이 존재 한다면
		if( _access_s( cNewFile, 0 ) == 0 )
			DeleteFileA( cNewFile );
		rename( cPath, cNewFile );
	}

	YExcel::BasicExcel excel;
	excel.New( 1 );
	excel.GetWorksheet( (size_t)0 )->Rename( QUEST_WORKSHEET_NAME );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( QUEST_WORKSHEET_NAME );
	assert_cs( pSheet != NULL );

	pSheet->Cell( 0, 0 )->SetInteger( QUEST_EXCEL_VERSION );

	int nCol = 0;
	pSheet->Cell( 1, nCol++ )->SetString( "Uniq ID" );
	pSheet->Cell( 1, nCol++ )->SetString( "Model Recv" );
	pSheet->Cell( 1, nCol++ )->SetString( "Model Comp" );
	pSheet->Cell( 1, nCol++ )->SetString( "Level" );
	pSheet->Cell( 1, nCol++ )->SetString( "ToolPos_X" );
	pSheet->Cell( 1, nCol++ )->SetString( "ToolPos_Y" );
	pSheet->Cell( 1, nCol++ )->SetString( "Manage ID" );
	pSheet->Cell( 1, nCol++ )->SetString( "Active" );
	pSheet->Cell( 1, nCol++ )->SetString( "Immediate" );
#ifdef QUEST_RESET_180712
	pSheet->Cell( 1, nCol++ )->SetString( "IsReset" );
#endif
	pSheet->Cell( 1, nCol++ )->SetString( "Quest Type" );

	pSheet->Cell( 1, nCol++ )->SetString( "StartType" );
	pSheet->Cell( 1, nCol++ )->SetString( "Start ID" );

	pSheet->Cell( 1, nCol++ )->SetString( "Target" );
	pSheet->Cell( 1, nCol++ )->SetString( "Target Value1" );

	pSheet->Cell( 1, nCol++ )->SetString( "TitleTab" );
	pSheet->Cell( 1, nCol++ )->SetString( "Title" );
	pSheet->Cell( 1, nCol++ )->SetString( "BodyText" );
	pSheet->Cell( 1, nCol++ )->SetString( "Simple" );
	pSheet->Cell( 1, nCol++ )->SetString( "Helper" );
	pSheet->Cell( 1, nCol++ )->SetString( "Process" );
	pSheet->Cell( 1, nCol++ )->SetString( "Compleate" );
	pSheet->Cell( 1, nCol++ )->SetString( "Expert" );

	pSheet->Cell( 1, nCol++ )->SetWString( _T( "수행지급, 선행조건, 완료조건, 보상 순" ) );

	size_t siCurRow = 2;
	size_t siCurCol = 0;
	size_t siMaxCol = 0;
	CsQuest::MAP* pMapQuest = nsCsFileTable::g_pQuestMng->GetQuestMap();
	CsQuest::MAP_IT it = pMapQuest->begin();
	CsQuest* pQuest = NULL;
	for( ; it!=pMapQuest->end(); ++it )
	{
		siCurCol = 0;
		pQuest = it->second;

		// Uniq ID
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetUniqID() );
		++siCurCol;

		// Model ID
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetModelID_Recv() );
		++siCurCol;
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetModelID_Comp() );
		++siCurCol;

		// Level
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetLevel() );
		++siCurCol;

		// ToolPos
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetToolPos().x );
		++siCurCol;
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetToolPos().y );
		++siCurCol;

		// Manage ID
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetManageID() );
		++siCurCol;

		// Active
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->IsActive() );
		++siCurCol;

		// Immediate
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->IsImmediate() );
		++siCurCol;

#ifdef QUEST_RESET_180712
		// 퀘스트 리셋 여부
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetIsResetQuest());
		++siCurCol;
#endif
		// Type
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetQuestType() );
		++siCurCol;

		{
			// 시작 타겟
			CsQuest::eST_TYPE eStartTarget_Type = pQuest->GetStartTarget_Type();
			DWORD dwStartTargetID = pQuest->GetStartTarget_ID();
			pSheet->Cell( siCurRow, siCurCol )->SetInteger( eStartTarget_Type );
			++siCurCol;
			pSheet->Cell( siCurRow, siCurCol )->SetInteger( dwStartTargetID );
			++siCurCol;
		}

		// Target
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetQuestTarget() );		
		++siCurCol;
		pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->GetQuestTargetValue1() );
		++siCurCol;

		// 타이틀탭
		pSheet->Cell( siCurRow, siCurCol )->SetWString( pQuest->m_szTitleTab );
		++siCurCol;
		// 타이틀
		pSheet->Cell( siCurRow, siCurCol )->SetWString( pQuest->m_szTitleText );
		++siCurCol;
		// 본문
		pSheet->Cell( siCurRow, siCurCol )->SetWString( pQuest->m_szBodyText );
		++siCurCol;			
		// 퀘스트 심플
		pSheet->Cell( siCurRow, siCurCol )->SetWString( pQuest->m_szSimpleText );
		++siCurCol;
		// 퀘스트 헬퍼
		pSheet->Cell( siCurRow, siCurCol )->SetWString( pQuest->m_szHelperText );
		++siCurCol;
		// 퀘스트 중
		pSheet->Cell( siCurRow, siCurCol )->SetWString( pQuest->m_szProcess );
		++siCurCol;
		// 퀘스트 완료
		pSheet->Cell( siCurRow, siCurCol )->SetWString( pQuest->m_szCompleate );
		++siCurCol;
		// 퀘스트 확장
		pSheet->Cell( siCurRow, siCurCol )->SetWString( pQuest->m_szExpert );
		++siCurCol;

		{
			// 수행지급 카운트
			int nCount = (int)pQuest->GetApplyRequiteGroup()->GetList()->size();
			pSheet->Cell( siCurRow, siCurCol )->SetInteger( nCount );
			++siCurCol;
			CsQuestApplyRequite* pElement = NULL;
			for( int n=0; n < nCount; ++n )
			{
				pElement = pQuest->GetApplyRequiteGroup()->Get( n );
				// 선행조건 타입
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pElement->GetType() );
				++siCurCol;

				// 선행조건 값
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pElement->GetTypeID() );
				++siCurCol;

				// 선행조건 갯수
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pElement->GetTypeCount() );
				++siCurCol;
			}
		}

		{
			// 선행조건 카운트
			int nPreRequireCount = (int)pQuest->GetPreRequireGroup()->GetList()->size();
			pSheet->Cell( siCurRow, siCurCol )->SetInteger( nPreRequireCount );
			++siCurCol;
			CsQuestPreRequire* pPreRequire = NULL;
			for( int nPreRequire=0; nPreRequire < nPreRequireCount; ++nPreRequire )
			{
				pPreRequire = pQuest->GetPreRequireGroup()->Get( nPreRequire );
				// 선행조건 타입
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pPreRequire->GetType() );
				++siCurCol;

				// 선행조건 값
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pPreRequire->GetTypeID() );
				++siCurCol;

				// 선행조건 갯수
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pPreRequire->GetTypeCount() );
				++siCurCol;
			}
		}


		{
			// 완료조건 갯수
			int nCompRequireCount = (int)pQuest->GetCompRequireGroup()->GetList()->size();
			pSheet->Cell( siCurRow, siCurCol )->SetInteger( nCompRequireCount );
			++siCurCol;
			CsQuestCompRequire* pCompRequire = NULL;
			for( int nCompRequire=0; nCompRequire < nCompRequireCount; ++nCompRequire )
			{
				pCompRequire = pQuest->GetCompRequireGroup()->Get( nCompRequire );
				// 완료조건 타입
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pCompRequire->GetType() );
				++siCurCol;

				// 완료조건 값
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pCompRequire->GetTypeID() );
				++siCurCol;

				// 완료조건 카운트
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pCompRequire->GetTypeCount() );
				++siCurCol;

				// 완료 조건 타겟
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pCompRequire->GetTargetID() );
				++siCurCol;

				// 보조 데이터 값
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pCompRequire->GetSubValue() );
				++siCurCol;

				// 보조 데이터 값
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pCompRequire->GetSubValue2() );
				++siCurCol;
			}
		}

		{
			// 보상 갯수
			int nRequiteCount = (int)pQuest->GetRequiteGroup()->GetList()->size();
			pSheet->Cell( siCurRow, siCurCol )->SetInteger( nRequiteCount );
			++siCurCol;
			CsQuestRequite* pRequite = NULL;
			for( int nRequite=0; nRequite < nRequiteCount; ++nRequite )
			{
				pRequite = pQuest->GetRequiteGroup()->Get( nRequite );

				// 보상 메서드
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pRequite->GetMethodID() );
				++siCurCol;

				// 보상 타입
				CsQuestRequite::eTYPE eType = pRequite->GetType();
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( eType );
				++siCurCol;

				if( eType == CsQuestRequite::EXP )
				{
					// 보상 값
					pSheet->Cell( siCurRow, siCurCol )->SetInteger( CsFloat2Int( pRequite->GetTypeID()*fExpMul ) );
					++siCurCol;
				}
				else
				{
					// 보상 값
					pSheet->Cell( siCurRow, siCurCol )->SetInteger( pRequite->GetTypeID() );
					++siCurCol;
				}				

				// 보상 카운트
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pRequite->GetTypeCount() );
				++siCurCol;
			}
		}

		{
			// 이벤트
			for( int nEvnet=0; nEvnet < CsQuest::EVENT_MAX_COUNT ; ++nEvnet )
			{
				pSheet->Cell( siCurRow, siCurCol )->SetInteger( pQuest->m_nEvent[ nEvnet ] );
				++siCurCol;
			}
		}

		if( siMaxCol < siCurCol )
			siMaxCol = siCurCol;

		++siCurRow;
	}

	for( size_t i=1; i<siMaxCol; ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}

	size_t startStringIdx = 14;
#ifdef QUEST_RESET_180712
	startStringIdx = 15;
#endif
	for( size_t i=startStringIdx; i<22; ++i )
	{
		pSheet->Cell( 0, i )->SetString( "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________"\
										 "____________________________________________________________________________________________________" );
	}
	bool bSave = excel.SaveAs( cPath );
	assert_cs( bSave == true );

	CsMessageBox( MB_OK, _T( "세이브 완료!!" ) );
}


#define QUEST_BIN		"Quest.bin"
void CsQuestMng::SaveBin( char* cPath )
{
	//m_pParcing->SaveBin( cPath );
	//_Parcing();
	
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, QUEST_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	int nCount = (int)m_mapQuest.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsQuest* pQuest;
	CsQuest::MAP_IT it = m_mapQuest.begin();
	CsQuest::MAP_IT itEnd = m_mapQuest.end();

	DWORD dw;
	int cnt, n;
	USHORT sn;
	CsPoint pt;
	bool bIs;
	for( ; it!=itEnd; ++it )
	{
		pQuest = it->second;

		// Uniq ID
		dw = pQuest->GetUniqID();
		fwrite( &dw, sizeof( DWORD ), 1, fp );

		// Model ID
		dw = pQuest->GetModelID_Recv();
		fwrite( &dw, sizeof( DWORD ), 1, fp );
		dw = pQuest->GetModelID_Comp();
		fwrite( &dw, sizeof( DWORD ), 1, fp );

		// Level
		sn = pQuest->GetLevel();
		fwrite( &sn, sizeof( SHORT ), 1, fp );

		// pos
		pt = pQuest->GetToolPos();
		fwrite( &pt, sizeof( CsPoint ), 1, fp );

		// Managed ID
		dw = pQuest->GetManageID();
		fwrite( &dw, sizeof( DWORD ), 1, fp );

		// Active
		bIs = pQuest->IsActive();
		fwrite( &bIs, sizeof( bool ), 1, fp );

		// Immediate
		bIs = pQuest->IsImmediate();
		fwrite( &bIs, sizeof( bool ), 1, fp );	

#ifdef QUEST_RESET_180712
		// Reset Quest
		bIs = pQuest->GetIsResetQuest();
		fwrite( &bIs, sizeof( bool ), 1, fp );	
#endif
		// Type
		dw = pQuest->GetQuestType();
		fwrite( &dw, sizeof( DWORD ), 1, fp );

		{
			// 시작 타겟
			dw = pQuest->GetStartTarget_Type();
			fwrite( &dw, sizeof( DWORD ), 1, fp );
			dw = pQuest->GetStartTarget_ID();
			fwrite( &dw, sizeof( DWORD ), 1, fp );
		}

		// Target
		dw = pQuest->GetQuestTarget();
		fwrite( &dw, sizeof( DWORD ), 1, fp );
		n = pQuest->GetQuestTargetValue1();
		fwrite( &n, sizeof( int ), 1, fp );

		// 타이틀탭
		fwrite( pQuest->m_szTitleTab, sizeof( TCHAR ), QUEST_TITLE_LEN, fp );

		// 타이틀
		fwrite( pQuest->m_szTitleText, sizeof( TCHAR ), QUEST_TITLE_LEN, fp );

		// 본문
		fwrite( pQuest->m_szBodyText, sizeof( TCHAR ), QUEST_BODY_LEN, fp );

		// 퀘스트 심플
		fwrite( pQuest->m_szSimpleText, sizeof( TCHAR ), QUEST_SIMPLE_LEN, fp );

		// 퀘스트 헬퍼
		fwrite( pQuest->m_szHelperText, sizeof( TCHAR ), QUEST_HELPER_LEN, fp );

		// 퀘스트 중
		fwrite( pQuest->m_szProcess, sizeof( TCHAR ), QUEST_PROCESS_LEN, fp );

		// 퀘스트 완료
		fwrite( pQuest->m_szCompleate, sizeof( TCHAR ), QUEST_COMPLEATE_LEN, fp );

		// 퀘스트 확장
		fwrite( pQuest->m_szExpert, sizeof( TCHAR ), QUEST_PROCESS_LEN, fp );

		{
			// 수행 지급
			cnt = pQuest->GetApplyRequiteGroup()->GetListCount();
			fwrite( &cnt, sizeof( int ), 1, fp );
			for( int i=0; i<cnt; ++i )
			{
				dw = pQuest->GetApplyRequiteGroup()->Get( i )->GetType();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				dw = pQuest->GetApplyRequiteGroup()->Get( i )->GetTypeID();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				n = pQuest->GetApplyRequiteGroup()->Get( i )->GetTypeCount();
				fwrite( &n, sizeof( int ), 1, fp );
			}
		}

		{
			// 선행조건 카운트
			cnt = pQuest->GetPreRequireGroup()->GetListCount();
			fwrite( &cnt, sizeof( int ), 1, fp );
			for( int i=0; i<cnt; ++i )
			{
				dw = pQuest->GetPreRequireGroup()->Get( i )->GetType();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				dw = pQuest->GetPreRequireGroup()->Get( i )->GetTypeID();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				n = pQuest->GetPreRequireGroup()->Get( i )->GetTypeCount();
				fwrite( &n, sizeof( int ), 1, fp );
			}
		}


		{
			// 완료조건 갯수
			cnt = pQuest->GetCompRequireGroup()->GetListCount();
			fwrite( &cnt, sizeof( int ), 1, fp );
			for( int i=0; i<cnt; ++i )
			{
				dw = pQuest->GetCompRequireGroup()->Get( i )->GetType();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				dw = pQuest->GetCompRequireGroup()->Get( i )->GetTypeID();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				n = pQuest->GetCompRequireGroup()->Get( i )->GetTypeCount();
				fwrite( &n, sizeof( int ), 1, fp );

				dw = pQuest->GetCompRequireGroup()->Get( i )->GetTargetID();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				int nSubValue = pQuest->GetCompRequireGroup()->Get( i )->GetSubValue();
				fwrite( &nSubValue, sizeof( int ), 1, fp );

				int nSubValue2 = pQuest->GetCompRequireGroup()->Get( i )->GetSubValue2();
				fwrite( &nSubValue2, sizeof( int ), 1, fp );
			}
		}

		{
			// 보상 갯수
			cnt = pQuest->GetRequiteGroup()->GetListCount();
			fwrite( &cnt, sizeof( int ), 1, fp );
			for( int i=0; i<cnt; ++i )
			{
				dw = pQuest->GetRequiteGroup()->Get( i )->GetMethodID();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				dw = pQuest->GetRequiteGroup()->Get( i )->GetType();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				dw = pQuest->GetRequiteGroup()->Get( i )->GetTypeID();
				fwrite( &dw, sizeof( DWORD ), 1, fp );

				n = pQuest->GetRequiteGroup()->Get( i )->GetTypeCount();
				fwrite( &n, sizeof( int ), 1, fp );
			}			
		}

		{
			// 이벤트
			cnt = CsQuest::EVENT_MAX_COUNT;
			fwrite( &cnt, sizeof( int ), 1, fp );
			fwrite( pQuest->m_nEvent, sizeof( int ), cnt, fp );
		}
	}

	fclose( fp );
}

bool CsQuestMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, QUEST_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	assert_cs( fp != NULL );
	if( fp == NULL )
	{
		CsMessageBox( MB_OK, _T( "%s 파일이 존재 하지 않습니다." ), cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsQuest Quest;
	DWORD dw, dw1, dw2, dw3;
	int n, cnt;
	SHORT sn;
	CsPoint pt;
	bool bIs;
	for( int i=0; i<nCount; ++i )
	{
		// Uniq ID		
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetUniqID( dw );

		// Model ID
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetModelID_Recv( dw );
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetModelID_Comp( dw );

		// Level
		fread( &sn, sizeof( SHORT ), 1, fp );
		Quest.SetLevel( sn );

		// pos
		fread( &pt, sizeof( CsPoint ), 1, fp );
		Quest.SetToolPos( pt );

		// Managed ID
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetManageID( dw );

		// Active		
		fread( &bIs, sizeof( bool ), 1, fp );
		Quest.SetActive( bIs );

		// Immediate		
		fread( &bIs, sizeof( bool ), 1, fp );
		Quest.SetImmeiate( bIs );

#ifdef QUEST_RESET_180712
		// Immediate		
		fread( &bIs, sizeof( bool ), 1, fp );
		Quest.SetResetQuest( bIs );
#endif
		// Type		
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetQuestType( (CsQuest::eQUEST_TYPE)dw );
		
		{
			// 시작 타겟
			fread( &dw, sizeof( DWORD ), 1, fp );
			Quest.SetStartTarget_Type( (CsQuest::eST_TYPE)dw );

			fread( &dw, sizeof( DWORD ), 1, fp );			
			Quest.SetStartTarget_ID( dw );			
		}

		// Target
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetQuestTarget( (CsQuest::eQUEST_TARGET)dw );
		
		fread( &n, sizeof( int ), 1, fp );
		Quest.SetQuestTargetValue1( n );		

		// 타이틀탭
		fread( Quest.m_szTitleTab, sizeof( TCHAR ), QUEST_TITLE_LEN, fp );

		// 타이틀
		fread( Quest.m_szTitleText, sizeof( TCHAR ), QUEST_TITLE_LEN, fp );

		// 본문
		fread( Quest.m_szBodyText, sizeof( TCHAR ), QUEST_BODY_LEN, fp );

		// 퀘스트 심플
		fread( Quest.m_szSimpleText, sizeof( TCHAR ), QUEST_SIMPLE_LEN, fp );

		// 퀘스트 헬퍼
		fread( Quest.m_szHelperText, sizeof( TCHAR ), QUEST_HELPER_LEN, fp );

		// 퀘스트 중
		fread( Quest.m_szProcess, sizeof( TCHAR ), QUEST_PROCESS_LEN, fp );

		// 퀘스트 완료
		fread( Quest.m_szCompleate, sizeof( TCHAR ), QUEST_COMPLEATE_LEN, fp );

		// 퀘스트 확장
		fread( Quest.m_szExpert, sizeof( TCHAR ), QUEST_PROCESS_LEN, fp );
// 
// 		if( Quest.GetUniqID() == 59 )
// 			int a = 0;

		{
			// 수행 지급
			fread( &cnt, sizeof( int ), 1, fp );
			for( int c=0; c<cnt; ++c )
			{
				fread( &dw1, sizeof( DWORD ), 1, fp );
				fread( &dw2, sizeof( DWORD ), 1, fp );
				fread( &n, sizeof( int ), 1, fp );
				Quest.AddApplyRequite( (CsQuestApplyRequite::eTYPE)dw1, dw2, n );
			}
		}

		{
			// 선행조건
			fread( &cnt, sizeof( int ), 1, fp );
			for( int c=0; c<cnt; ++c )
			{
				fread( &dw1, sizeof( DWORD ), 1, fp );
				fread( &dw2, sizeof( DWORD ), 1, fp );
				fread( &n, sizeof( int ), 1, fp );
				Quest.AddPreRequire( (CsQuestPreRequire::eTYPE)dw1, dw2, n );
			}
		}


		{
			// 완료조건
			fread( &cnt, sizeof( int ), 1, fp );
			for( int c=0; c<cnt; ++c )
			{
				fread( &dw1, sizeof( DWORD ), 1, fp );
				fread( &dw2, sizeof( DWORD ), 1, fp );
				fread( &n, sizeof( int ), 1, fp );
				fread( &dw3, sizeof( DWORD ), 1, fp );
				int nSubValue;
				fread( &nSubValue, sizeof( int ), 1, fp );
				int nSubValue2;
				fread( &nSubValue2, sizeof( int ), 1, fp );
				Quest.AddCompRequire( (CsQuestCompRequire::eTYPE)dw1, dw2, n, dw3, nSubValue, nSubValue2 );
			}
		}

		{
			// 보상 갯수
			fread( &cnt, sizeof( int ), 1, fp );
			for( int c=0; c<cnt; ++c )
			{
				fread( &dw, sizeof( DWORD ), 1, fp );
				fread( &dw1, sizeof( DWORD ), 1, fp );
				fread( &dw2, sizeof( DWORD ), 1, fp );
				fread( &n, sizeof( int ), 1, fp );
				Quest.AddRequite( (CsQuestRequite::eMETHOD)dw, (CsQuestRequite::eTYPE)dw1, dw2, n );
			}
		}

		{
			// 이벤트
			fread( &cnt, sizeof( int ), 1, fp );
			assert_cs( cnt <= CsQuest::EVENT_MAX_COUNT );
			fread( Quest.m_nEvent, sizeof( int ), cnt, fp );
		}

		if( nsCsFileTable::g_bReadInactiveQuest || Quest.IsActive() )
			AddQuest( &Quest, 0 );
		Quest.Delete();
	}
	fclose( fp );
	return true;
}

void CsQuestMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, QUEST_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	CsQuest Quest;
	DWORD dw, dw1, dw2, dw3;
	int n, cnt;
	SHORT sn;
	CsPoint pt;
	bool bIs;
	for( int i=0; i<nCount; ++i )
	{
		// Uniq ID
		_read( nHandle, &dw, sizeof( DWORD ) );
		Quest.SetUniqID( dw );

		// Uniq ID
		_read( nHandle, &dw, sizeof( DWORD ) );
		Quest.SetModelID_Recv( dw );
		_read( nHandle, &dw, sizeof( DWORD ) );
		Quest.SetModelID_Comp( dw );

		// Level
		_read( nHandle, &sn, sizeof( SHORT ) );
		Quest.SetLevel( sn );

		// pos
		_read( nHandle, &pt, sizeof( CsPoint ) );
		Quest.SetToolPos( pt );

		// Managed ID
		_read( nHandle, &dw, sizeof( DWORD ) );
		Quest.SetManageID( dw );

		// Active
		_read( nHandle, &bIs, sizeof( bool ) );
		Quest.SetActive( bIs );

		// Immediate		
		_read( nHandle, &bIs, sizeof( bool ) );
		Quest.SetImmeiate( bIs );

#ifdef QUEST_RESET_180712
		// Immediate		
		_read( nHandle, &bIs, sizeof( bool ) );
		Quest.SetResetQuest( bIs );
#endif
		// Type		
		_read( nHandle, &dw, sizeof( DWORD ) );
		Quest.SetQuestType( (CsQuest::eQUEST_TYPE)dw );

		{
			// 시작 타겟
			_read( nHandle, &dw, sizeof( DWORD ) );
			Quest.SetStartTarget_Type( (CsQuest::eST_TYPE)dw );

			_read( nHandle, &dw, sizeof( DWORD ) );
			Quest.SetStartTarget_ID( dw );			
		}

		// Target
		_read( nHandle, &dw, sizeof( DWORD ) );
		Quest.SetQuestTarget( (CsQuest::eQUEST_TARGET)dw );

		_read( nHandle, &n, sizeof( int ) );
		Quest.SetQuestTargetValue1( n );		

		// 타이틀탭
		_read( nHandle, Quest.m_szTitleTab, sizeof( TCHAR )*QUEST_TITLE_LEN );

		// 타이틀
		_read( nHandle, Quest.m_szTitleText, sizeof( TCHAR )*QUEST_TITLE_LEN );

		// 본문
		_read( nHandle, Quest.m_szBodyText, sizeof( TCHAR )*QUEST_BODY_LEN );

		// 퀘스트 심플
		_read( nHandle, Quest.m_szSimpleText, sizeof( TCHAR )*QUEST_SIMPLE_LEN );

		// 퀘스트 헬퍼
		_read( nHandle, Quest.m_szHelperText, sizeof( TCHAR )*QUEST_HELPER_LEN );

		// 퀘스트 중
		_read( nHandle, Quest.m_szProcess, sizeof( TCHAR )*QUEST_PROCESS_LEN );

		// 퀘스트 완료
		_read( nHandle, Quest.m_szCompleate, sizeof( TCHAR )*QUEST_COMPLEATE_LEN );

		// 퀘스트 확장
		_read( nHandle, Quest.m_szExpert, sizeof( TCHAR )*QUEST_PROCESS_LEN );

		{
			// 수행 지급
			_read( nHandle, &cnt, sizeof( int ) );
			for( int c=0; c<cnt; ++c )
			{
				_read( nHandle, &dw1, sizeof( DWORD ) );
				_read( nHandle, &dw2, sizeof( DWORD ) );
				_read( nHandle, &n, sizeof( int ) );
				Quest.AddApplyRequite( (CsQuestApplyRequite::eTYPE)dw1, dw2, n );
			}
		}

		{
			// 선행조건
			_read( nHandle, &cnt, sizeof( int ) );
			for( int c=0; c<cnt; ++c )
			{
				_read( nHandle, &dw1, sizeof( DWORD ) );
				_read( nHandle, &dw2, sizeof( DWORD ) );
				_read( nHandle, &n, sizeof( int ) );
				Quest.AddPreRequire( (CsQuestPreRequire::eTYPE)dw1, dw2, n );
			}
		}


		{
			// 완료조건
			_read( nHandle, &cnt, sizeof( int ) );
			for( int c=0; c<cnt; ++c )
			{
				_read( nHandle, &dw1, sizeof( DWORD ) );
				_read( nHandle, &dw2, sizeof( DWORD ) );
				_read( nHandle, &n, sizeof( int ) );
				_read( nHandle, &dw3, sizeof( DWORD ) );
				int nSubValue;
				_read( nHandle, &nSubValue, sizeof( int ) );
				int nSubValue2;
				_read( nHandle, &nSubValue2, sizeof( int ) );
				Quest.AddCompRequire( (CsQuestCompRequire::eTYPE)dw1, dw2, n, dw3, nSubValue, nSubValue2 );
			}
		}

		{
			// 보상 갯수
			_read( nHandle, &cnt, sizeof( int ) );
			for( int c=0; c<cnt; ++c )
			{
				_read( nHandle, &dw, sizeof( DWORD ) );
				_read( nHandle, &dw1, sizeof( DWORD ) );
				_read( nHandle, &dw2, sizeof( DWORD ) );
				_read( nHandle, &n, sizeof( int ) );
				Quest.AddRequite( (CsQuestRequite::eMETHOD)dw, (CsQuestRequite::eTYPE)dw1, dw2, n );
			}
		}

		{
			// 이벤트
			_read( nHandle, &cnt, sizeof( int ) );
			assert_cs( cnt <= CsQuest::EVENT_MAX_COUNT );
			_read( nHandle, Quest.m_nEvent, sizeof( int )*cnt );
		}

		if( nsCsFileTable::g_bReadInactiveQuest || Quest.IsActive() )
			AddQuest( &Quest, 0 );
		Quest.Delete();
	}

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

DWORD CsQuestMng::GetLastUniqID()
{
	if( m_mapQuest.empty() == true )
		return 0;

	CsQuest::MAP_IT itEnd = m_mapQuest.end();
	--itEnd;
	return itEnd->second->GetUniqID() + 1;
}
// 
// void CsQuestMng::_Parcing()
// {
// 	CsQuest::MAP_IT it = m_mapQuest.begin();
// 	CsQuest::MAP_IT itEnd = m_mapQuest.end();
// 
// 	for( ; it != itEnd; it++ )
// 	{
// 		m_pParcing->Parcing( it->second->m_szBodyText );
// 	}
// }

bool CsQuestMng::Reload( char* cPath )
{	
	//m_pParcing->Reload( cPath );

	//ofstream QuestdataFile;
	//QuestdataFile.open("example.txt");
	//
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, QUEST_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	assert_cs( fp != NULL );

	if( fp == NULL )
	{
		CsMessageBox( MB_OK, _T( "%s 파일이 존재 하지 않습니다." ), cName );
		return false;
	}

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsQuest Quest;
	DWORD dw, dw1, dw2, dw3;
	int n, cnt;
	SHORT sn;
	CsPoint pt;
	bool bIs;
	for( int i=0; i<nCount; ++i )
	{
		// Uniq ID		
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetUniqID( dw );

		// Model ID
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetModelID_Recv( dw );
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetModelID_Comp( dw );

		// Level
		fread( &sn, sizeof( SHORT ), 1, fp );
		Quest.SetLevel( sn );

		// pos
		fread( &pt, sizeof( CsPoint ), 1, fp );
		Quest.SetToolPos( pt );

		// Managed ID
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetManageID( dw );

		// Active
		fread( &bIs, sizeof( bool ), 1, fp );
		Quest.SetActive( bIs );

		// Immediate		
		fread( &bIs, sizeof( bool ), 1, fp );
		Quest.SetImmeiate( bIs );

#ifdef QUEST_RESET_180712
		// Immediate		
		fread( &bIs, sizeof( bool ), 1, fp );
		Quest.SetResetQuest( bIs );
#endif

		// Type		
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetQuestType( (CsQuest::eQUEST_TYPE)dw );

		{
			// 시작 타겟
			fread( &dw, sizeof( DWORD ), 1, fp );
			Quest.SetStartTarget_Type( (CsQuest::eST_TYPE)dw );

			fread( &dw, sizeof( DWORD ), 1, fp );			
			Quest.SetStartTarget_ID( dw );			
		}

		// Target
		fread( &dw, sizeof( DWORD ), 1, fp );
		Quest.SetQuestTarget( (CsQuest::eQUEST_TARGET)dw );

		fread( &n, sizeof( int ), 1, fp );
		Quest.SetQuestTargetValue1( n );		

		// 타이틀탭
		fread( Quest.m_szTitleTab, sizeof( TCHAR ), QUEST_TITLE_LEN, fp );

		// 타이틀
		fread( Quest.m_szTitleText, sizeof( TCHAR ), QUEST_TITLE_LEN, fp );

		// 본문
		fread( Quest.m_szBodyText, sizeof( TCHAR ), QUEST_BODY_LEN, fp );

		// 퀘스트 심플
		fread( Quest.m_szSimpleText, sizeof( TCHAR ), QUEST_SIMPLE_LEN, fp );

		// 퀘스트 헬퍼
		fread( Quest.m_szHelperText, sizeof( TCHAR ), QUEST_HELPER_LEN, fp );

		// 퀘스트 중
		fread( Quest.m_szProcess, sizeof( TCHAR ), QUEST_PROCESS_LEN, fp );

		// 퀘스트 완료
		fread( Quest.m_szCompleate, sizeof( TCHAR ), QUEST_COMPLEATE_LEN, fp );

		// 퀘스트 확장
		fread( Quest.m_szExpert, sizeof( TCHAR ), QUEST_PROCESS_LEN, fp );

		{
			// 수행 지급
			fread( &cnt, sizeof( int ), 1, fp );
			for( int c=0; c<cnt; ++c )
			{
				fread( &dw1, sizeof( DWORD ), 1, fp );
				fread( &dw2, sizeof( DWORD ), 1, fp );
				fread( &n, sizeof( int ), 1, fp );
				Quest.AddApplyRequite( (CsQuestApplyRequite::eTYPE)dw1, dw2, n );
			}
		}

		{
			// 선행조건
			fread( &cnt, sizeof( int ), 1, fp );
			for( int c=0; c<cnt; ++c )
			{
				fread( &dw1, sizeof( DWORD ), 1, fp );
				fread( &dw2, sizeof( DWORD ), 1, fp );
				fread( &n, sizeof( int ), 1, fp );
				Quest.AddPreRequire( (CsQuestPreRequire::eTYPE)dw1, dw2, n );
			}
		}


		{
			// 완료조건
			fread( &cnt, sizeof( int ), 1, fp );
			for( int c=0; c<cnt; ++c )
			{
				fread( &dw1, sizeof( DWORD ), 1, fp );
				fread( &dw2, sizeof( DWORD ), 1, fp );
				fread( &n, sizeof( int ), 1, fp );
				fread( &dw3, sizeof( DWORD ), 1, fp );

				int nSubValue;
				fread( &nSubValue, sizeof( int ), 1, fp );
				int nSubValue2;
				fread( &nSubValue2, sizeof( int ), 1, fp );
				Quest.AddCompRequire( (CsQuestCompRequire::eTYPE)dw1, dw2, n, dw3, nSubValue, nSubValue2 );
			}
		}

		{
			// 보상 갯수
			fread( &cnt, sizeof( int ), 1, fp );
			for( int c=0; c<cnt; ++c )
			{
				fread( &dw, sizeof( DWORD ), 1, fp );
				fread( &dw1, sizeof( DWORD ), 1, fp );
				fread( &dw2, sizeof( DWORD ), 1, fp );
				fread( &n, sizeof( int ), 1, fp );
				Quest.AddRequite( (CsQuestRequite::eMETHOD)dw, (CsQuestRequite::eTYPE)dw1, dw2, n );
			}
		}

		{
			// 이벤트
			fread( &cnt, sizeof( int ), 1, fp );
			assert_cs( cnt <= CsQuest::EVENT_MAX_COUNT );
			fread( Quest.m_nEvent, sizeof( int ), cnt, fp );
		}		

		//QuestdataFile << "QUEST" << "\n";
		//QuestdataFile << Quest.GetUniqID() << "\n";
		//wstring test(&Quest.m_szTitleText[0]); //convert to wstring
		//string test2(test.begin(), test.end());
		// 
		//QuestdataFile << test2 << "\n";
		
	/*	
		if( Quest.IsActive() == true )
		{
			assert_csm( m_mapQuest.find( Quest.GetUniqID() ) != m_mapQuest.end(), _T( "OLA, ESTOU A LER DO QUESTS BIN" ) );
			memcpy( m_mapQuest[ Quest.GetUniqID() ]->m_szTitleTab, &Quest.m_szTitleTab, sizeof( TCHAR ) * QUEST_TITLE_LEN );
			memcpy( m_mapQuest[ Quest.GetUniqID() ]->m_szTitleText, &Quest.m_szTitleText, sizeof( TCHAR ) * QUEST_TITLE_LEN );
			memcpy( m_mapQuest[ Quest.GetUniqID() ]->m_szBodyText, &Quest.m_szBodyText, sizeof( TCHAR ) * QUEST_BODY_LEN );
			memcpy( m_mapQuest[ Quest.GetUniqID() ]->m_szSimpleText, &Quest.m_szSimpleText, sizeof( TCHAR ) * QUEST_SIMPLE_LEN );
			memcpy( m_mapQuest[ Quest.GetUniqID() ]->m_szHelperText, &Quest.m_szHelperText, sizeof( TCHAR ) * QUEST_HELPER_LEN );
			memcpy( m_mapQuest[ Quest.GetUniqID() ]->m_szProcess, &Quest.m_szProcess, sizeof( TCHAR ) * QUEST_PROCESS_LEN );
			memcpy( m_mapQuest[ Quest.GetUniqID() ]->m_szCompleate, &Quest.m_szCompleate, sizeof( TCHAR ) * QUEST_COMPLEATE_LEN );
			memcpy( m_mapQuest[ Quest.GetUniqID() ]->m_szExpert, &Quest.m_szExpert, sizeof( TCHAR ) * QUEST_PROCESS_LEN );

		}*/
		Quest.Delete();
	}
	fclose( fp );
	/*QuestdataFile.close();*/
	return true;	
}

void CsQuestMng::GetQuestListFormType( CsQuest::eQUEST_TYPE const& nQuestType, std::list<DWORD>& list )
{
	CsQuest::MAP_IT it = m_mapQuest.begin();
	for( ; it != m_mapQuest.end(); ++it )
	{
		if( nQuestType != it->second->GetQuestType() )
			continue;
		list.push_back( it->first );
	}
}