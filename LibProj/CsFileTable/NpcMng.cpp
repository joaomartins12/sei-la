
#include "stdafx.h"
#include "NpcMng.h"


void CsNpcMng::Delete()
{
	CsNpc::MAP_IT itNpc = m_mapNpc.begin();
	CsNpc::MAP_IT itNpcEnd = m_mapNpc.end();
	for( ; itNpc!=itNpcEnd; ++itNpc )
	{
		itNpc->second->Delete();
		delete itNpc->second;
	}
	m_mapNpc.clear();

	CsNpcGroup::MAP_IT itGroup = m_mapNpcGroup.begin();
	CsNpcGroup::MAP_IT itGroupEnd = m_mapNpcGroup.end();
	for( ; itGroup!=itGroupEnd; ++itGroup )
	{
		itGroup->second->Delete();
		delete itGroup->second;
	}
	m_mapNpcGroup.clear();


	std::map< DWORD, sMODEL_INFO* >::iterator it = m_mapModelInfo.begin();
	std::map< DWORD, sMODEL_INFO* >::iterator itEnd = m_mapModelInfo.end();
	for( ; it!=itEnd; ++it )
	{
		delete it->second;
	}
	m_mapModelInfo.clear();
}

bool CsNpcMng::Init( char* cPath )
{
	assert_cs( m_mapNpcGroup.size() == 0 );
	assert_cs( m_mapNpc.size() == 0 );

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

CsNpc* CsNpcMng::GetNpc( DWORD dwNpcTableID )
{
	CsNpc::MAP_IT itNpc = m_mapNpc.find( dwNpcTableID );
	if( itNpc == m_mapNpc.end() )
		return NULL;
	return itNpc->second;
// 	assert_cs( m_mapNpc.find( dwNpcTableID ) != m_mapNpc.end() );
// 	return m_mapNpc[ dwNpcTableID ];
}

bool CsNpcMng::IsNpc( DWORD dwNpcID )
{ 
	return ( m_mapNpc.find( dwNpcID ) != m_mapNpc.end() ); 
}	

void CsNpcMng::FindNpc_FromModelID(DWORD const& dwModelID, std::list<DWORD>& listNpcID )
{
	CsNpc::MAP_IT itNpc = m_mapNpc.begin();
	for( ; itNpc != m_mapNpc.end(); ++itNpc )
	{
		SAFE_POINTER_CON( itNpc->second );
		CsNpc::sINFO* pNpcInfo = itNpc->second->GetInfo();
		SAFE_POINTER_CON( pNpcInfo );

		if( pNpcInfo->s_dwModelID != dwModelID )
			continue;
		listNpcID.push_back( itNpc->first );
	}
}

CsNpc* CsNpcMng::FindNpc( DWORD nModelID )
{
	CsNpc::MAP_IT itNpc = m_mapNpc.begin();
	for( ; itNpc != m_mapNpc.end(); ++itNpc )
	{
		if( NULL == itNpc->second )
			continue;

		CsNpc::sINFO* pNpcInfo = itNpc->second->GetInfo();
		if( NULL == pNpcInfo )
			continue;

		if( pNpcInfo->s_dwModelID == nModelID )
			return itNpc->second;
	}
	return NULL;
}

void CsNpcMng::DeleteNpc( DWORD dwNpcTableID )
{
	CsNpc* pNpc = GetNpc( dwNpcTableID );
	if( NULL == pNpc )
		return;

	CsNpc::sINFO* pNpcInfo = pNpc->GetInfo();
	if( pNpcInfo )
	{
		CsNpcGroup* pNpcGrop = GetGroup( pNpcInfo->s_dwMapID );
		if( pNpcGrop )
			pNpcGrop->DeleteNpc( dwNpcTableID );
	}	

	pNpc->Delete();
	delete pNpc;
	m_mapNpc.erase( dwNpcTableID );
}

void CsNpcMng::AddNpc( CsNpc* pSrcNpc )
{
	if( NULL == pSrcNpc )
		return;

	CsNpc::sINFO* pInfo = pSrcNpc->GetInfo();
	if( NULL == pInfo )
		return;

	if( IsNpc( pInfo->s_dwNpcID ) )
		return;

	CsNpcGroup* pNpcGroup = GetGroup( pInfo->s_dwMapID );
	if( pNpcGroup )
		pNpcGroup->AddNpc( pSrcNpc );

	m_mapNpc.insert( std::make_pair( pInfo->s_dwNpcID, pSrcNpc ));

//	assert_cs( IsNpc( pSrcNpc->GetInfo()->s_dwNpcID ) == false );
// 	GetGroup( pSrcNpc->GetInfo()->s_dwMapID )->AddNpc( pSrcNpc );
// 	m_mapNpc[ pSrcNpc->GetInfo()->s_dwNpcID ] = pSrcNpc;	
}

bool CsNpcMng::IsGroup( DWORD dwMapID )
{ 
	return ( m_mapNpcGroup.find( dwMapID ) != m_mapNpcGroup.end() ); 
}

CsNpcGroup* CsNpcMng::GetGroup( DWORD dwMapID )
{
	CsNpcGroup::MAP_IT it = m_mapNpcGroup.find( dwMapID );
	if( it == m_mapNpcGroup.end() )
	{
		CsNpcGroup* pGroup = csnew CsNpcGroup;
		pGroup->Init();
		auto rib = m_mapNpcGroup.insert( std::make_pair( dwMapID, pGroup ) );
		if( !rib.second )
			return NULL;

		it = rib.first;
	}
	return it->second;

// 	if( IsGroup( dwMapID ) == false )
// 	{
// 		CsNpcGroup* pGroup = csnew CsNpcGroup;
// 		pGroup->Init();
// 		m_mapNpcGroup.insert( std::make_pair( dwMapID, pGroup ) );
// 		//m_mapNpcGroup[ dwMapID ] = pGroup;
// 	}
// 
// 	return m_mapNpcGroup[ dwMapID ];
}

void CsNpcMng::_PreSave()
{
	if( _access_s( nsCsFileTable::GetPath_DM_Npc(), 0 ) != 0 )
	{
		YExcel::BasicExcel excel;
		excel.New( 2 );
		excel.GetWorksheet( (size_t)0 )->Rename( NPC_WORKSHEET_NAME );
		excel.GetWorksheet( (size_t)1 )->Rename( NPC_MODELINFO_WORKSHEET_NAME );
		excel.SaveAs( nsCsFileTable::GetPath_DM_Npc() );
	}
	else
	{
		YExcel::BasicExcel excel;
		bool bLoad = excel.Load( nsCsFileTable::GetPath_DM_Npc() );
		assert_cs( bLoad == true );

		if( excel.GetWorksheet( NPC_WORKSHEET_NAME ) == NULL )
			excel.AddWorksheet( NPC_WORKSHEET_NAME, 0 );

		if( excel.GetWorksheet( NPC_MODELINFO_WORKSHEET_NAME ) == NULL )
			excel.AddWorksheet( NPC_MODELINFO_WORKSHEET_NAME, 1 );

		excel.Save();
	}
}

void CsNpcMng::Save_Npc()
{
	_PreSave();

	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_DM_Npc() );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( NPC_WORKSHEET_NAME );
	assert_cs( pSheet != NULL );
	pSheet->DeleteAllRows();

	pSheet->Cell( 0, 0 )->SetInteger( NPC_EXCEL_VERSION );

	pSheet->Cell( 1, 0 )->SetString( "ID" );
	pSheet->Cell( 1, 1 )->SetString( "Map" );
	pSheet->Cell( 1, 2 )->SetString( "Type" );
	pSheet->Cell( 1, 3 )->SetString( "MoveType" );
	pSheet->Cell( 1, 4 )->SetString( "DisplayType" );
	pSheet->Cell( 1, 5 )->SetString( "PenName" );
	pSheet->Cell( 1, 6 )->SetString( "Name" );
	pSheet->Cell( 1, 7 )->SetString( "Model" );
	pSheet->Cell( 1, 8 )->SetString( "Talk" );
	
	size_t row = 2;
	size_t col = 0;

	size_t MaxCol = 0;

	CsNpcGroup::MAP_IT itGroup = m_mapNpcGroup.begin();
	CsNpcGroup::MAP_IT itGroupEnd = m_mapNpcGroup.end();
	for( ; itGroup!=itGroupEnd; ++itGroup )
	{				
		CsNpc::MAP* pMap = itGroup->second->GetNpcMap();
		CsNpc::MAP_IT it = pMap->begin();
		CsNpc::MAP_IT itEnd = pMap->end();
		CsNpc* pNpc;
		CsNpc::sINFO* pInfo;
		for( ; it!=itEnd; ++it )
		{
			col = 0;

			pNpc = it->second;
			pInfo = pNpc->GetInfo();

			// ID
			pSheet->Cell( row, col )->SetInteger( pInfo->s_dwNpcID );
			++col;

			// Map
			pSheet->Cell( row, col )->SetInteger( pInfo->s_dwMapID );
			++col;

			// Type
			pSheet->Cell( row, col )->SetInteger( pInfo->s_eType );
			++col;

			// MoveType
			pSheet->Cell( row, col )->SetInteger( pInfo->s_eMoveType );
			++col;

			// DisplayType
			pSheet->Cell( row, col )->SetInteger( pInfo->s_nDisplayPlag );
			++col;

			// PenName
			pSheet->Cell( row, col )->SetWString( pInfo->s_szPenName );
			++col;

			// Name
			pSheet->Cell( row, col )->SetWString( pInfo->s_szName );
			++col;

			// Model
			pSheet->Cell( row, col )->SetInteger( pInfo->s_dwModelID );
			++col;

			// Talk
			pSheet->Cell( row, col )->SetWString( pInfo->s_szTalk );						
			++col;

			// Type
			switch( pNpc->GetInfo()->s_eType )
			{
			case nsCsNpcTable::NT_NONE:
			case nsCsNpcTable::NT_MAKE_TACTICS:
			case nsCsNpcTable::NT_ELEMENT_ITEM:
			case nsCsNpcTable::NT_WAREHOUSE:
			case nsCsNpcTable::NT_TACTICSHOUSE:
			case nsCsNpcTable::NT_DIGITAMA_TRADE:
			case nsCsNpcTable::NT_CAPSULE_MACHINE:
			case nsCsNpcTable::NT_SKILL:	
			case nsCsNpcTable::NT_DATS_PORTAL:
			case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
			case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
			case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
			case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
			case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
			case nsCsNpcTable::NT_GOTCHA_MACHINE:
			case nsCsNpcTable::NT_MYSTERY_MACHINE:
			case nsCsNpcTable::NT_SPIRIT_EVO:
				break;
			case nsCsNpcTable::NT_TRADE:
			case nsCsNpcTable::NT_GUILD:
			case nsCsNpcTable::NT_DIGICORE:
			case nsCsNpcTable::NT_EVENT_STORE:
			case nsCsNpcTable::NT_PROPERTY_STORE:
				{
					sNPC_TYPE_TRADE* pType = (sNPC_TYPE_TRADE*)pNpc->GetType();
					assert_cs( pType->s_eNpcType == pInfo->s_eType );
					// 갯수
					pSheet->Cell( row, col )->SetInteger( pType->GetItemCount() );
					++col;
					for( int c=0; c<pType->GetItemCount(); ++c )
					{
						pSheet->Cell( row, col )->SetInteger( pType->GetItemID( c ) );
						++col;
					}
				}
				break;		

			case nsCsNpcTable::NT_PORTAL:
				{
					sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)pNpc->GetType();
					assert_cs( pType->s_eNpcType == pInfo->s_eType );
					pSheet->Cell( row, col )->SetInteger( pType->s_nPortalType );
					++col;
					// 갯수
					pSheet->Cell( row, col )->SetInteger( pType->s_nPortalCount );
					++col;
					for( int p=0; p<pType->s_nPortalCount; ++p )
					{
						sNPC_TYPE_PORTAL::sPORTAL* pPortal = pType->GetPortal( p );
						for( int q=0; q<NPC_TYPE_PORTAL_MAX_REQ_COUNT; ++q )
						{
							// 타입
							pSheet->Cell( row, col )->SetInteger( pPortal->s_ReqArray[ q ].s_eEnableType );
							++col;
							// ID
							pSheet->Cell( row, col )->SetInteger( pPortal->s_ReqArray[ q ].s_nEnableID );
							++col;
							// Count
							pSheet->Cell( row, col )->SetInteger( pPortal->s_ReqArray[ q ].s_nEnableCount );
							++col;
						}

						// 이벤트가동 ID
						pSheet->Cell( row, col )->SetInteger( pPortal->s_dwEventID );
						++col;
					}
				}
				break;		
			case nsCsNpcTable::NT_MASTERS_MATCHING:
				{
					sNPC_TYPE_MASTER_MATCHING* pType = (sNPC_TYPE_MASTER_MATCHING*)pNpc->GetType();
					assert_cs( pType->s_eNpcType == pInfo->s_eType );
					// 갯수
					pSheet->Cell( row, col )->SetInteger( pType->GetItemCount() );
					++col;
					for( int c=0; c<pType->GetItemCount(); ++c )
					{
						pSheet->Cell( row, col )->SetInteger( pType->GetItemID( c ) );
						++col;
					}
				}
				break;
			case nsCsNpcTable::NT_SPECIAL_EVENT:
				{
					sNPC_TYPE_SPECIAL_EVENT* pType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetType();
					assert_cs( pType->s_eNpcType == pInfo->s_eType );
					// 이벤트에 따라 동작
					int nEvType = pType->GetSubType();
					pSheet->Cell( row, col )->SetInteger( nEvType );
					++col;
					switch( nEvType )
					{
					case sNPC_TYPE_SPECIAL_EVENT::EVENT_CARDGAME:		// 피에몬 카드 게임
						break;
					case sNPC_TYPE_SPECIAL_EVENT::EVENT_PINOKIMON:		// 피노키몬의 카드게임(2014 겨울이벤트)
						{	
							// 아이템 갯수
							int nItemCnt = pType->GetItemCount();
							pSheet->Cell( row, col )->SetInteger( nItemCnt );
							++col;
							// 투표 아이템 ID
							for( int i = 0 ; i < nItemCnt ; i++ )
							{
								pSheet->Cell( row, col )->SetInteger( pType->GetItemID( i ) );
								++col;
							}
						}
						break;
					default:
						break;
					}
				}
				break;
			default:
				assert_cs( false );
			}

			// Extra Data
			int nExtraDataCnt = 0;
			sNPC_EXTRA_BASE** pArray = pNpc->GetExtraArray();
			for( int a=0; a<nsCsNpcTable::NE_MAX_CNT; ++a )
			{
				if( pArray[ a ] != NULL )
					++nExtraDataCnt;
			}

			pSheet->Cell( row, col )->SetInteger( nExtraDataCnt );
			++col;

			for( int a=0; a<nsCsNpcTable::NE_MAX_CNT; ++a )
			{
				if( pArray[ a ] == NULL )
					continue;

				pSheet->Cell( row, col )->SetInteger( pArray[ a ]->s_eNpcExtra );
				++col;

				switch( pArray[ a ]->s_eNpcExtra )
				{
				case nsCsNpcTable::NE_QUEST:
					{
						sNPC_EXTRA_QUEST* pExtra = (sNPC_EXTRA_QUEST*)pArray[ a ];

						// 초기 상태
						pSheet->Cell( row, col )->SetInteger( pExtra->s_eInitState );
						++col;

						int nActCnt = pExtra->GetVector()->Size();

						// 행동 갯수
						pSheet->Cell( row, col )->SetInteger( nActCnt );
						++col;
						for( int nAct=0; nAct<nActCnt; ++nAct )
						{
							sNPC_EXTRA_QUEST::sACT* pAct = pExtra->GetVector()->GetData( nAct );

							// 행동 타입
							pSheet->Cell( row, col )->SetInteger( pAct->s_eActType );
							++col;

							// 완료 상태
							pSheet->Cell( row, col )->SetInteger( pAct->s_eCompState );
							++col;

							// 퀘스트 갯수
							int nQuestCnt = (int)pAct->s_list.size();
							pSheet->Cell( row, col )->SetInteger( nQuestCnt );
							++col;

							std::list< int >::iterator it = pAct->s_list.begin();
							std::list< int >::iterator itEnd = pAct->s_list.end();
							for( ; it!=itEnd; ++it )
							{
								pSheet->Cell( row, col )->SetInteger( *it );
								++col;
							}
						}
					}
					break;
				default:
					assert_cs( false );
				}
			}

			if( MaxCol < col )
				MaxCol = col;

			++row;
		}		
	}

	for( size_t i=1; i<MaxCol; ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}

	bool bSave = excel.SaveAs( nsCsFileTable::GetPath_DM_Npc() );
	assert_cs( bSave == true );
}

void CsNpcMng::Save_ModelInfo()
{
	_PreSave();

	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( nsCsFileTable::GetPath_DM_Npc() );

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( NPC_MODELINFO_WORKSHEET_NAME );	
	assert_cs( pSheet != NULL );
	pSheet->DeleteAllRows();

	pSheet->Cell( 0, 0 )->SetInteger( NPC_EXCEL_VERSION );
	for( size_t i=1; i<5; ++i )
	{
		pSheet->Cell( 0, i )->SetInteger( 1 );
	}

	pSheet->Cell( 1, 0 )->SetString( "ModelID" );
	pSheet->Cell( 1, 1 )->SetString( "Comment" );
	pSheet->Cell( 1, 2 )->SetString( "X" );
	pSheet->Cell( 1, 3 )->SetString( "Y" );
	pSheet->Cell( 1, 4 )->SetString( "Z" );

	size_t row = 2;
	size_t col = 0;

	std::map< DWORD, sMODEL_INFO* >::iterator it = m_mapModelInfo.begin();
	std::map< DWORD, sMODEL_INFO* >::iterator itEnd = m_mapModelInfo.end();
	sMODEL_INFO* pInfo = NULL;
	for( ; it!=itEnd; ++it )
	{
		pInfo = it->second;

		col = 0;

		// ModelID
		pSheet->Cell( row, col )->SetInteger( pInfo->s_nModelID );
		++col;

		// Comment
		pSheet->Cell( row, col )->SetWString( pInfo->s_szComment );
		++col;

		// Offset
		pSheet->Cell( row, col )->SetInteger( pInfo->s_nOffset[ 0 ] );
		++col;
		pSheet->Cell( row, col )->SetInteger( pInfo->s_nOffset[ 1 ] );
		++col;
		pSheet->Cell( row, col )->SetInteger( pInfo->s_nOffset[ 2 ] );
		++col;

		++row;
	}	

	bool bSave = excel.SaveAs( nsCsFileTable::GetPath_DM_Npc() );
	assert_cs( bSave == true );
}

void CsNpcMng::_LoadExcel()
{
	_LoadExcel_Npc();
	_LoadExcel_ModelInfo();
	_LoadExcel_Npc_Event();
}

void CsNpcMng::_LoadExcel_Npc()
{
	char cBuffer[ CEXCLE_CELL_LEN ];
	wchar_t wBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Npc(), NPC_WORKSHEET_NAME );
	if( bSuccess == false )
		return;

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != NPC_EXCEL_VERSION )
	{
		MessageBox( NULL, _T( "Npc.xls 버젼이 틀립니다. 송명수에게 문의해 주세요!!!" ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD col = 0;	
	CsNpc::sINFO* pInfo;
	while( excel.MoveNext() == true )
	{
		col = 0;
		// ID
		DWORD nNpcID = atol( excel.GetData( col, cBuffer ) );
		if( nNpcID == 0 )
			continue;

		CsNpc* pNew = csnew CsNpc;
		pNew->Init();
		pInfo = pNew->GetInfo();
		pInfo->s_dwNpcID = nNpcID;

		++col;

		// Map
		pInfo->s_dwMapID = atoi( excel.GetData( col, cBuffer ) );
		++col;

		// Type
		pInfo->s_eType = (nsCsNpcTable::eNPC_TYPE)atoi( excel.GetData( col, cBuffer ) );
		++col;

		// MoveType
		pInfo->s_eMoveType = (nsCsNpcTable::eNPC_MOVE)atoi( excel.GetData( col, cBuffer ) );
		++col;

		// DisplayType
		pInfo->s_nDisplayPlag = atoi( excel.GetData( col, cBuffer ) );
		++col;

		std::wstring wsPenName = excel.GetData( col, wBuffer );
		if( wsPenName.length() > NPC_NAME_LEN - 1 )
			wsPenName = wsPenName.substr(0, NPC_NAME_LEN-1);

		wcscpy_s(pInfo->s_szPenName, NPC_NAME_LEN, wsPenName.c_str());
		++col;

		std::wstring wsNpcName = excel.GetData( col, wBuffer );
		if( wsNpcName.length() > NPC_NAME_LEN - 1 )
			wsNpcName = wsNpcName.substr(0, NPC_NAME_LEN-1);

		wcscpy_s(pInfo->s_szName, NPC_NAME_LEN, wsNpcName.c_str());
		++col;

// 		if(nsCsFileTable::g_eLanguage == nsCsFileTable::THAILAND || nsCsFileTable::g_eLanguage == nsCsFileTable::TAIWAN || nsCsFileTable::g_eLanguage == nsCsFileTable::HONGKONG )
// 		{
// 			// PenName
// 			//M2W( pInfo->s_szPenName, excel.GetData( col, cBuffer ), NPC_NAME_LEN );
// 			wcscpy_s(pInfo->s_szPenName, NPC_NAME_LEN, excel.GetData( col, wBuffer ));
// 			++col;
// 
// 			// Name
// 			//M2W( pInfo->s_szName, excel.GetData( col, cBuffer ), NPC_NAME_LEN );
// 			wcscpy_s(pInfo->s_szName, NPC_NAME_LEN, excel.GetData( col, wBuffer ));
// 			++col;
// 		}
// 		else
// 		{
// 			// PenName
// 			M2W( pInfo->s_szPenName, excel.GetData( col, cBuffer ), NPC_NAME_LEN );
// 			++col;
// 
// 			// Name
// 			M2W( pInfo->s_szName, excel.GetData( col, cBuffer ), NPC_NAME_LEN );
// 			++col;
// 		}

		// Model
		pInfo->s_dwModelID = atoi( excel.GetData( col, cBuffer ) );
		++col;

		std::wstring wsNpcTalk = excel.GetData( col, wBuffer );
		if( wsNpcTalk.length() > NPC_TALK_LEN - 1 )
			wsNpcTalk = wsNpcTalk.substr(0, NPC_TALK_LEN-1);

		wcscpy_s(pInfo->s_szTalk, NPC_TALK_LEN, wsNpcTalk.c_str());
		++col;

/*		if(nsCsFileTable::g_eLanguage == nsCsFileTable::THAILAND || nsCsFileTable::g_eLanguage == nsCsFileTable::TAIWAN || nsCsFileTable::g_eLanguage == nsCsFileTable::HONGKONG )
		{
			// Talk
			//M2W( pInfo->s_szTalk, excel.GetData( col, cBuffer ), NPC_TALK_LEN );
			wcscpy_s(pInfo->s_szTalk, NPC_TALK_LEN, excel.GetData( col, wBuffer ));
			++col;
		}
		else
		{
			//아이템테이블 제한 글자 수 체크 수정 chu8820
//			M2W( pInfo->s_szTalk, excel.GetData( col, cBuffer ), NPC_TALK_LEN );
			TCHAR szBuffer[ 2048 ];
			M2W( szBuffer, excel.GetData( col, cBuffer ), 2048 );
			if( (int)_tcslen( szBuffer ) >= NPC_TALK_LEN -1 )
				CsMessageBox( MB_OK, L"NPC =%d  Talk의 문자열이 너무 깁니다. \n현재길이 : %d / 제한길이 : %d", pInfo->s_dwNpcID , (int)_tcslen( szBuffer ), NPC_TALK_LEN );
			else
				_tcscpy_s( pInfo->s_szTalk, NPC_TALK_LEN, szBuffer  );
		
			++col;
		}
*/
		// Type
		pNew->SetType();
		switch( pInfo->s_eType )
		{
		case nsCsNpcTable::NT_NONE:			
		case nsCsNpcTable::NT_DIGITAMA_TRADE:
		case nsCsNpcTable::NT_MAKE_TACTICS:
		case nsCsNpcTable::NT_ELEMENT_ITEM:
		case nsCsNpcTable::NT_WAREHOUSE:
		case nsCsNpcTable::NT_TACTICSHOUSE:
		case nsCsNpcTable::NT_CAPSULE_MACHINE:
		case nsCsNpcTable::NT_SKILL:
		case nsCsNpcTable::NT_DATS_PORTAL:
		case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
		case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
		case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
		case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
		case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
		case nsCsNpcTable::NT_GOTCHA_MACHINE:
		case nsCsNpcTable::NT_MYSTERY_MACHINE:
		case nsCsNpcTable::NT_SPIRIT_EVO:
			break;
		case nsCsNpcTable::NT_TRADE:
		case nsCsNpcTable::NT_GUILD:
		case nsCsNpcTable::NT_DIGICORE:
		case nsCsNpcTable::NT_EVENT_STORE:
		case nsCsNpcTable::NT_PROPERTY_STORE:
			{
				sNPC_TYPE_TRADE* pType = (sNPC_TYPE_TRADE*)pNew->GetType();
				assert_cs( pType->s_eNpcType == pInfo->s_eType );

				// 갯수
				int nTypeCount = atoi( excel.GetData( col, cBuffer ) );
				++col;
				for( int c=0; c<nTypeCount; ++c )
				{
					pType->AddItem( atoi( excel.GetData( col, cBuffer ) ) );
					++col;
				}
			}
			break;

		case nsCsNpcTable::NT_PORTAL:
			{
				sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)pNew->GetType();
				assert_cs( pType->s_eNpcType == pInfo->s_eType );
				pType->s_nPortalType = atoi( excel.GetData( col, cBuffer ) );
				++col;
				// 갯수
				pType->s_nPortalCount = atoi( excel.GetData( col, cBuffer ) );
				++col;

				for( int p=0; p<pType->s_nPortalCount; ++p )
				{
					sNPC_TYPE_PORTAL::sPORTAL* pPortal = pType->NewPortal( false );

					for( int q=0; q<NPC_TYPE_PORTAL_MAX_REQ_COUNT; ++q )
					{
						// 타입
						pPortal->s_ReqArray[ q ].s_eEnableType = (sNPC_TYPE_PORTAL::eENABLE_TYPE)atoi( excel.GetData( col, cBuffer ) );
						++col;
						// ID
						pPortal->s_ReqArray[ q ].s_nEnableID = atoi( excel.GetData( col, cBuffer ) );
						++col;
						// Count
						pPortal->s_ReqArray[ q ].s_nEnableCount = atoi( excel.GetData( col, cBuffer ) );
						++col;
					}

					// 이벤트가동 ID
					pPortal->s_dwEventID = atoi( excel.GetData( col, cBuffer ) );
					++col;
				}
			}
			break;
		case nsCsNpcTable::NT_MASTERS_MATCHING:
			{
				sNPC_TYPE_MASTER_MATCHING* pType = (sNPC_TYPE_MASTER_MATCHING*)pNew->GetType();
				assert_cs( pType->s_eNpcType == pInfo->s_eType );
				// 갯수
				int nTypeCount = atoi( excel.GetData( col, cBuffer ) );
				++col;
				for( int c=0; c<nTypeCount; ++c )
				{
					pType->AddItem( atoi( excel.GetData( col, cBuffer ) ) );
					++col;
				}
			}
			break;
		case nsCsNpcTable::NT_SPECIAL_EVENT:
			{
				sNPC_TYPE_SPECIAL_EVENT* pType = (sNPC_TYPE_SPECIAL_EVENT*)pNew->GetType();
				assert_cs( pType->s_eNpcType == pInfo->s_eType );
				// 이벤트에 따라 동작
				int nEventType = atoi( excel.GetData( col, cBuffer ) );
				++col;
				pType->SetSubType( (sNPC_TYPE_SPECIAL_EVENT::eSUBTYPE)nEventType );

				switch( nEventType )
				{
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_CARDGAME:		// 피에몬 카드 게임
					break;
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_PINOKIMON:		// 피노키몬의 카드게임(2014 겨울이벤트)
					{
						// 아이템 갯수
						int nItemCnt = atoi( excel.GetData( col, cBuffer ) );
						++col;
						// 투표 아이템 ID
						for( int i = 0 ; i < nItemCnt ; i++ )
						{
							pType->AddItem( atoi( excel.GetData( col, cBuffer ) ) );
							++col;
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			assert_cs( false );
		}

		// Extra Data
		int nExtraDataCnt = atoi( excel.GetData( col, cBuffer ) );
		++col;
		for( int extra=0; extra<nExtraDataCnt; ++extra )
		{
			int nExtraType = atoi( excel.GetData( col, cBuffer ) );
			++col;

			sNPC_EXTRA_BASE* pBase = pNew->AddExtra( (nsCsNpcTable::eNPC_EXTRA)nExtraType );

			switch( nExtraType )
			{
			case nsCsNpcTable::NE_QUEST:
				{
					sNPC_EXTRA_QUEST* pQuest = (sNPC_EXTRA_QUEST*)pBase;

					// 초기 상태
					pQuest->s_eInitState = (sNPC_EXTRA_QUEST::eSTATE)atoi( excel.GetData( col, cBuffer ) );
					++col;

					// 행동 갯수
					int nActCnt = atoi( excel.GetData( col, cBuffer ) );
					++col;

					for( int nAct=0; nAct<nActCnt; ++nAct )
					{
						sNPC_EXTRA_QUEST::sACT* pNewAct = csnew sNPC_EXTRA_QUEST::sACT;						
						pQuest->s_vpAct.PushBack( pNewAct );						

						// 행동 타입
						pNewAct->s_eActType = (sNPC_EXTRA_QUEST::eACT_TYPE)atoi( excel.GetData( col, cBuffer ) );
						++col;

						// 완료 상태
						pNewAct->s_eCompState = (sNPC_EXTRA_QUEST::eSTATE)atoi( excel.GetData( col, cBuffer ) );
						++col;

						// 퀘스트 갯수
						int nQuestCnt = atoi( excel.GetData( col, cBuffer ) );
						++col;

						for( int nQuest=0; nQuest<nQuestCnt; ++nQuest )
						{							
							pNewAct->s_list.push_back( atoi( excel.GetData( col, cBuffer ) ) );
							++col;
						}
					}
				}
				break;
			default:
				assert_cs( false );
			}
		}

		AddNpc( pNew );		
	}
}

void CsNpcMng::_LoadExcel_ModelInfo()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Npc(), NPC_MODELINFO_WORKSHEET_NAME );
	if( bSuccess == false )
		return;

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != NPC_EXCEL_VERSION )
	{
		MessageBox( NULL, _T( "Npc.xls 버젼이 틀립니다. 송명수에게 문의해 주세요!!!" ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD col = 0;	
	while( excel.MoveNext() == true )
	{
		col = 0;
		// ID
		DWORD nModelID = atol( excel.GetData( col, cBuffer ) );
		if( nModelID == 0 )
			continue;

		sMODEL_INFO* pInfo = csnew sMODEL_INFO;
		pInfo->s_nModelID = nModelID;
		++col;

		// Comment
		M2W( pInfo->s_szComment, excel.GetData( col, cBuffer ), 64 );
		++col;

		// Offset
		pInfo->s_nOffset[ 0 ] = atoi( excel.GetData( col, cBuffer ) );
		++col;
		pInfo->s_nOffset[ 1 ] = atoi( excel.GetData( col, cBuffer ) );
		++col;
		pInfo->s_nOffset[ 2 ] = atoi( excel.GetData( col, cBuffer ) );
		++col;

		m_mapModelInfo[ pInfo->s_nModelID ] = pInfo;
	}
}

void CsNpcMng::_LoadExcel_Npc_Event()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Npc(), "Npc_Event" );
	if( bSuccess == false )
		return;

	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD col = 0;	
	while( excel.MoveNext() == true )
	{
		col = 0;
		// ID
		DWORD nNpcID = atol( excel.GetData( col++, cBuffer ) );
		if( nNpcID == 0 )
			continue;

		sNPC_EVENT_INFO* pInfo = csnew sNPC_EVENT_INFO;
		pInfo->s_nNpcID = nNpcID;
		col += 3;

		// try
		pInfo->s_nTry = atoi( excel.GetData( col++, cBuffer ) );

		// 소모 bit
		pInfo->s_nExhaustMoney = atoi( excel.GetData( col++, cBuffer ) );
		col++;

		// 소모 아이템 코드 번호
		pInfo->s_dwExhaustItem	= atoi( excel.GetData( col++, cBuffer ) );	

		// 소모 아이템 갯수
		pInfo->s_unItemCount	= atoi( excel.GetData( col++, cBuffer ) );	

		// 지급 item 정보
		for( int i=0; i<MAX_CARD_ITEM; ++i )
		{
			pInfo->s_aItemInfo[i].s_nItemID = atoi( excel.GetData( col++, cBuffer ) );
			col += 2;

			pInfo->s_aItemInfo[i].s_nCount = atoi( excel.GetData( col++, cBuffer ) );
		}

		m_mapCardGameInfo[ pInfo->s_nNpcID ] = pInfo;
	}
}

CsNpcMng::sNPC_EVENT_INFO* CsNpcMng::GetCardGameInfo( DWORD nNpcID )
{
	std::map< DWORD, sNPC_EVENT_INFO* >::iterator it = m_mapCardGameInfo.begin();
	std::map< DWORD, sNPC_EVENT_INFO* >::iterator itEnd = m_mapCardGameInfo.end();

	for( ; it != itEnd; ++it )
	{
		if( nNpcID != it->second->s_nNpcID )
			continue;

		return m_mapCardGameInfo[ nNpcID ];
	}

	return NULL;
}

void CsNpcMng::Reload_ZoomOffset()
{
	char cBuffer[ CEXCLE_CELL_LEN ];

	cExcelReader excel;
	bool bSuccess = excel.Open( nsCsFileTable::GetPath_DM_Npc(), NPC_MODELINFO_WORKSHEET_NAME );
	assert_cs( bSuccess == true );
	// 2줄은 버린다.
	bSuccess = excel.MoveFirst();
	assert_cs( bSuccess == true );

	if( atoi( excel.GetData( ME_VERSION_COL, cBuffer ) ) != NPC_EXCEL_VERSION )
	{
		MessageBox( NULL, _T( "Npc.xls 버젼이 틀립니다. 송명수에게 문의해 주세요!!!" ), NULL, MB_OK );
		return;
	}

	bSuccess = excel.MoveNext();
	assert_cs( bSuccess == true );

	DWORD col = 0;	
	int nID = 0;
	while( excel.MoveNext() == true )
	{
		col = 0;

		// ID
		nID = atol( excel.GetData( col, cBuffer ) );
		if( nID == 0 )
			continue;

		if( m_mapModelInfo.find( nID ) == m_mapModelInfo.end() )
			continue;

		++col;
		++col;

		m_mapModelInfo[ nID ]->s_nOffset[ 0 ] = atoi( excel.GetData( col, cBuffer ) );
		++col;
		m_mapModelInfo[ nID ]->s_nOffset[ 1 ] = atoi( excel.GetData( col, cBuffer ) );
		++col;
		m_mapModelInfo[ nID ]->s_nOffset[ 2 ] = atoi( excel.GetData( col, cBuffer ) );
		++col;
	}
}

#define NPC_BIN		"Npc.bin"
void CsNpcMng::SaveBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, NPC_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "wb" );
	assert_cs( fp != NULL );

	_SaveBin_Npc( fp );
	_SaveBin_ModelInfo( fp );
	_SaveBin_NpcEvent( fp );

	fclose( fp );
}

void CsNpcMng::_SaveBin_Npc( FILE *fp )
{
	int nCount = (int)m_mapNpc.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsNpc* pNpc;
	CsNpc::MAP_IT itNpc = m_mapNpc.begin();
	CsNpc::MAP_IT itNpcEnd = m_mapNpc.end();
	for( ; itNpc!=itNpcEnd; ++itNpc )
	{
		pNpc = itNpc->second;

		fwrite( pNpc->GetInfo(), sizeof( CsNpc::sINFO ), 1, fp );

		switch( pNpc->GetInfo()->s_eType )
		{
		case nsCsNpcTable::NT_NONE:
		case nsCsNpcTable::NT_DIGITAMA_TRADE:
		case nsCsNpcTable::NT_MAKE_TACTICS:
		case nsCsNpcTable::NT_ELEMENT_ITEM:
		case nsCsNpcTable::NT_WAREHOUSE:
		case nsCsNpcTable::NT_TACTICSHOUSE:
		case nsCsNpcTable::NT_CAPSULE_MACHINE:
		case nsCsNpcTable::NT_SKILL:
		case nsCsNpcTable::NT_DATS_PORTAL:
		case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
		case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
		case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
		case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
		case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
		case nsCsNpcTable::NT_GOTCHA_MACHINE:
		case nsCsNpcTable::NT_MYSTERY_MACHINE:
		case nsCsNpcTable::NT_SPIRIT_EVO:
			break;
		case nsCsNpcTable::NT_TRADE:
		case nsCsNpcTable::NT_GUILD:
		case nsCsNpcTable::NT_DIGICORE:
		case nsCsNpcTable::NT_EVENT_STORE:
		case nsCsNpcTable::NT_PROPERTY_STORE:
			{
				sNPC_TYPE_TRADE* pType = (sNPC_TYPE_TRADE*)pNpc->GetType();
				int cnt = pType->GetItemCount();
				fwrite( &cnt, sizeof( int ), 1, fp );

				for( int n=0; n<cnt; ++n )
				{
					DWORD dwItem = pType->GetItemID( n );
					fwrite( &dwItem, sizeof( DWORD ), 1, fp );
				}
			}
			break;

		case nsCsNpcTable::NT_PORTAL:
			{
				sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)pNpc->GetType();
				int typ = pType->s_nPortalType;
				fwrite( &typ, sizeof( int ), 1, fp );
				int cnt = pType->s_nPortalCount;
				fwrite( &cnt, sizeof( int ), 1, fp );
				for( int p=0; p<cnt; ++p )
				{
					sNPC_TYPE_PORTAL::sPORTAL* pPortal = pType->GetPortal( p );
					fwrite( pPortal, sizeof( sNPC_TYPE_PORTAL::sPORTAL ), 1, fp );
				}
			}
			break;		
		case nsCsNpcTable::NT_MASTERS_MATCHING: // 마스터즈 매치
			{
				sNPC_TYPE_MASTER_MATCHING* pType = (sNPC_TYPE_MASTER_MATCHING*)pNpc->GetType();
				int cnt = pType->GetItemCount();
				fwrite( &cnt, sizeof( int ), 1, fp );

				for( int n=0; n<cnt; ++n )
				{
					DWORD dwItem = pType->GetItemID( n ); // 기부 아이템 번
					fwrite( &dwItem, sizeof( DWORD ), 1, fp );
				}
			}
			break;
		case nsCsNpcTable::NT_SPECIAL_EVENT:
			{
				sNPC_TYPE_SPECIAL_EVENT* pType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetType();
				int nEvType = pType->GetSubType();

				fwrite( &nEvType, sizeof( int ), 1, fp );

				switch( nEvType )
				{
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_CARDGAME:		// 피에몬 카드 게임
					break;
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_PINOKIMON:	// 피노키몬의 카드게임(2014 겨울이벤트)
					{
						// 아이템 갯수
						int nItemCnt = pType->GetItemCount();
						fwrite( &nItemCnt, sizeof( int ), 1, fp );
						// 투표 아이템 ID
						for( int i = 0 ; i < nItemCnt ; i++ )
						{
							DWORD dwItem = pType->GetItemID( i );
							fwrite( &dwItem, sizeof( DWORD ), 1, fp );
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			assert_cs( false );
		}

		// Extra Data
		int nExtraDataCnt = 0;
		sNPC_EXTRA_BASE** pArray = pNpc->GetExtraArray();
		for( int a=0; a<nsCsNpcTable::NE_MAX_CNT; ++a )
		{
			if( pArray[ a ] != NULL )
				++nExtraDataCnt;
		}
		fwrite( &nExtraDataCnt, sizeof( int ), 1, fp );

		for( int extra=0; extra<nsCsNpcTable::NE_MAX_CNT; ++extra )
		{
			if( pArray[ extra ] == NULL )
				continue;

			int nExtraType = pArray[ extra ]->s_eNpcExtra;
			fwrite( &nExtraType, sizeof( int ), 1, fp );

			switch( nExtraType )
			{
			case nsCsNpcTable::NE_QUEST:
				{
					sNPC_EXTRA_QUEST* pQuest = (sNPC_EXTRA_QUEST*)pArray[ extra ];

					// 초기 상태
					fwrite( &pQuest->s_eInitState, sizeof( DWORD ), 1, fp );

					// 행동 갯수
					int nActCnt = pQuest->s_vpAct.Size();
					fwrite( &nActCnt, sizeof( int ), 1, fp );

					for( int nAct=0; nAct<nActCnt; ++nAct )
					{
						sNPC_EXTRA_QUEST::sACT* pAct = pQuest->s_vpAct[ nAct ];

						// 행동 타입
						fwrite( &pAct->s_eActType, sizeof( DWORD ), 1, fp );

						// 완료 상태
						fwrite( &pAct->s_eCompState, sizeof( DWORD ), 1, fp );

						// 퀘스트 갯수
						int nQuestCnt = (int)pAct->s_list.size();
						fwrite( &nQuestCnt, sizeof( int ), 1, fp );

						std::list< int >::iterator it = pAct->s_list.begin();
						std::list< int >::iterator itEnd = pAct->s_list.end();
						for( ; it!=itEnd; ++it )
						{
							int nQuestID = *it;
							fwrite( &nQuestID, sizeof( int ), 1, fp );
						}
					}
				}
				break;
			default:
				assert_cs( false );
			}
		}
	}	
}

void CsNpcMng::_SaveBin_ModelInfo( FILE *fp )
{
	int nCount = (int)m_mapModelInfo.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< DWORD, sMODEL_INFO* >::iterator it = m_mapModelInfo.begin();
	std::map< DWORD, sMODEL_INFO* >::iterator itEnd = m_mapModelInfo.end();
	sMODEL_INFO* pInfo = NULL;
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second, sizeof( sMODEL_INFO ), 1, fp );
	}
}

void CsNpcMng::_SaveBin_NpcEvent( FILE* fp )
{
	int nCount = (int)m_mapCardGameInfo.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	std::map< DWORD, sNPC_EVENT_INFO* >::iterator it = m_mapCardGameInfo.begin();
	std::map< DWORD, sNPC_EVENT_INFO* >::iterator itEnd = m_mapCardGameInfo.end();
	sNPC_EVENT_INFO* pInfo = NULL;
	for( ; it!=itEnd; ++it )
	{
		fwrite( it->second, sizeof( sNPC_EVENT_INFO ), 1, fp );
	}
}

bool CsNpcMng::_LoadBin( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, NPC_BIN );

	FILE *fp = NULL;
	fopen_s( &fp, cName, "rb" );
	if( fp == NULL )
	{
		CsMessageBoxA( MB_OK, "%s 파일이 존재 하지 않습니다.", cName );
		return false;
	}

	_LoadBin_Npc( fp );
	_LoadBin_ModelInfo( fp );
	_LoadBin_NpcEvent( fp );

	fclose( fp );
	return true;
}

void CsNpcMng::_LoadBin_Npc( FILE* fp )
{
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	CsNpc* pNpc = NULL;
	CsNpc::sINFO* pInfo;
	for( int i=0; i<nCount; ++i )
	{
		pNpc = csnew CsNpc;
		pNpc->Init();		
		pInfo = pNpc->GetInfo();

		fread( pInfo, sizeof( CsNpc::sINFO ), 1, fp );

		// Type
		pNpc->SetType();
		switch( pNpc->GetInfo()->s_eType )
		{
		case nsCsNpcTable::NT_NONE:
		case nsCsNpcTable::NT_DIGITAMA_TRADE:
		case nsCsNpcTable::NT_MAKE_TACTICS:
		case nsCsNpcTable::NT_ELEMENT_ITEM:
		case nsCsNpcTable::NT_WAREHOUSE:
		case nsCsNpcTable::NT_TACTICSHOUSE:
		case nsCsNpcTable::NT_CAPSULE_MACHINE:
		case nsCsNpcTable::NT_SKILL:
		case nsCsNpcTable::NT_DATS_PORTAL:
		case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
		case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
		case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
		case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
		case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
		case nsCsNpcTable::NT_GOTCHA_MACHINE:
		case nsCsNpcTable::NT_MYSTERY_MACHINE:
		case nsCsNpcTable::NT_SPIRIT_EVO:
			break;
		case nsCsNpcTable::NT_TRADE:
		case nsCsNpcTable::NT_GUILD:
		case nsCsNpcTable::NT_DIGICORE:
		case nsCsNpcTable::NT_EVENT_STORE:
		case nsCsNpcTable::NT_PROPERTY_STORE:
			{
				int cnt;
				fread( &cnt, sizeof( int ), 1, fp );

				sNPC_TYPE_TRADE* pType = (sNPC_TYPE_TRADE*)pNpc->GetType();
				for( int n=0; n<cnt; ++n )
				{
					DWORD dwItem;
					fread( &dwItem, sizeof( DWORD ), 1, fp );
					pType->AddItem( dwItem );
				}				
			}
			break;		
		case nsCsNpcTable::NT_PORTAL:
			{
				sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)pNpc->GetType();
				fread( &pType->s_nPortalType, sizeof( int ), 1, fp );
				fread( &pType->s_nPortalCount, sizeof( int ), 1, fp );
				for( int p=0; p<pType->s_nPortalCount; ++p )
				{
					sNPC_TYPE_PORTAL::sPORTAL* pPortal = pType->NewPortal( false );
					fread( pPortal, sizeof( sNPC_TYPE_PORTAL::sPORTAL ), 1, fp );
				}
			}
			break;		
		case nsCsNpcTable::NT_MASTERS_MATCHING:
			{
				int cnt;
				fread( &cnt, sizeof( int ), 1, fp );

				sNPC_TYPE_MASTER_MATCHING* pType = (sNPC_TYPE_MASTER_MATCHING*)pNpc->GetType();
				for( int n=0; n<cnt; ++n )
				{
					DWORD dwItem;
					fread( &dwItem, sizeof( DWORD ), 1, fp );
					pType->AddItem( dwItem );
				}				
			}
			break;	
		case nsCsNpcTable::NT_SPECIAL_EVENT:
			{
				int nEvType;
				fread( &nEvType, sizeof( int ), 1, fp );

				sNPC_TYPE_SPECIAL_EVENT* pType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetType();
				pType->SetSubType( (sNPC_TYPE_SPECIAL_EVENT::eSUBTYPE)nEvType );

				switch( nEvType )
				{
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_CARDGAME:		// 피에몬 카드 게임
					break;
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_PINOKIMON:	// 피노키몬의 카드게임(2014 겨울이벤트)
					{
						int nItemCnt = pType->GetItemCount();
						fread( &nItemCnt, sizeof( DWORD ), 1, fp );

						for( int i = 0 ; i < nItemCnt ; i++ )
						{
							DWORD dwItem;
							fread( &dwItem, sizeof( DWORD ), 1, fp );
							pType->AddItem( dwItem );
						}
					}
					break;
				default:
					break;
				}
			}
			break;
		default:
			assert_cs( false );
		}

		// Extra Data
		int nExtraDataCnt = 0;
		fread( &nExtraDataCnt, sizeof( int ), 1, fp );

		for( int extra=0; extra<nExtraDataCnt; ++extra )
		{
			int nExtraType;
			fread( &nExtraType, sizeof( int ), 1, fp );
			sNPC_EXTRA_BASE* pBase = pNpc->AddExtra( (nsCsNpcTable::eNPC_EXTRA)nExtraType );

			switch( nExtraType )
			{
			case nsCsNpcTable::NE_QUEST:
				{
					sNPC_EXTRA_QUEST* pQuest = (sNPC_EXTRA_QUEST*)pBase;

					// 초기 상태
					fread( &pQuest->s_eInitState, sizeof( DWORD ), 1, fp );

					// 행동 갯수
					int nActCnt;
					fread( &nActCnt, sizeof( int ), 1, fp );

					for( int nAct=0; nAct<nActCnt; ++nAct )
					{
						sNPC_EXTRA_QUEST::sACT* pAct = csnew sNPC_EXTRA_QUEST::sACT;
						pQuest->s_vpAct.PushBack( pAct );

						// 행동 타입
						fread( &pAct->s_eActType, sizeof( DWORD ), 1, fp );

						// 완료 상태
						fread( &pAct->s_eCompState, sizeof( DWORD ), 1, fp );

						// 퀘스트 갯수
						int nQuestCnt;
						fread( &nQuestCnt, sizeof( int ), 1, fp );
						for( int nQuest=0; nQuest<nQuestCnt; ++nQuest )
						{
							int nQuestID;
							fread( &nQuestID, sizeof( int ), 1, fp );
							pAct->s_list.push_back( nQuestID );
						}
					}
				}
				break;
			default:
				assert_cs( false );
			}
		}
		AddNpc( pNpc );
	}
}

void CsNpcMng::_LoadBin_ModelInfo( FILE* fp )
{
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	for( int i=0; i<nCount; ++i )
	{
		sMODEL_INFO* pInfo = csnew sMODEL_INFO;
		fread( pInfo, sizeof( sMODEL_INFO ), 1, fp );
		m_mapModelInfo[ pInfo->s_nModelID ] = pInfo;
	}
}

void CsNpcMng::_LoadBin_NpcEvent( FILE* fp )
{
	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	for( int i=0; i<nCount; ++i )
	{
		sNPC_EVENT_INFO* pInfo = csnew sNPC_EVENT_INFO;
		fread( pInfo, sizeof( sNPC_EVENT_INFO ), 1, fp );
		m_mapCardGameInfo[ pInfo->s_nNpcID ] = pInfo;
	}
}

void CsNpcMng::_LoadFilePack( char* cPath )
{
	char cName[ MAX_PATH ];
	sprintf_s( cName, MAX_PATH, "%s\\%s", cPath, NPC_BIN );

	if( !CsFPS::CsFPSystem::SeekLock( FT_PACKHANDLE ) )
		return;

	int nHandle = CsFPS::CsFPSystem::GetFileHandle( FT_PACKHANDLE, cName );

	_LoadFilePack_Npc( nHandle );
	_LoadFilePack_ModelInfo( nHandle );
	_LoadFilePack_NpcEvent( nHandle );

	CsFPS::CsFPSystem::SeekUnLock( FT_PACKHANDLE );
}

void CsNpcMng::_LoadFilePack_Npc( int nHandle )
{
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	CsNpc* pNpc = NULL;
	CsNpc::sINFO* pInfo;

	for( int i=0; i<nCount; ++i )
	{
		pNpc = csnew CsNpc;
		pNpc->Init();		
		pInfo = pNpc->GetInfo();

		_read( nHandle, pInfo, sizeof( CsNpc::sINFO ) );

		// Type
		pNpc->SetType();
		switch( pNpc->GetInfo()->s_eType )
		{
		case nsCsNpcTable::NT_NONE:			
		case nsCsNpcTable::NT_DIGITAMA_TRADE:
		case nsCsNpcTable::NT_MAKE_TACTICS:
		case nsCsNpcTable::NT_ELEMENT_ITEM:
		case nsCsNpcTable::NT_WAREHOUSE:
		case nsCsNpcTable::NT_TACTICSHOUSE:
		case nsCsNpcTable::NT_CAPSULE_MACHINE:
		case nsCsNpcTable::NT_SKILL:
		case nsCsNpcTable::NT_DATS_PORTAL:
		case nsCsNpcTable::NT_ITEM_PRODUCTION_NPC:
		case nsCsNpcTable::NT_BATTLE_REGISTRANT_NPC:
		case nsCsNpcTable::NT_INFINITEWAR_MANAGER_NPC:
		case nsCsNpcTable::NT_INFINITEWAR_NOTICEBOARD_NPC:
		case nsCsNpcTable::NT_EXTRA_EVOLUTION_NPC:
		case nsCsNpcTable::NT_GOTCHA_MACHINE:
		case nsCsNpcTable::NT_MYSTERY_MACHINE:
		case nsCsNpcTable::NT_SPIRIT_EVO:
			break;
		case nsCsNpcTable::NT_TRADE:
		case nsCsNpcTable::NT_GUILD:
		case nsCsNpcTable::NT_DIGICORE:
		case nsCsNpcTable::NT_EVENT_STORE:
		case nsCsNpcTable::NT_PROPERTY_STORE:
			{
				int cnt;
				_read( nHandle, &cnt, sizeof( int ) );

				DWORD dwItem;
				sNPC_TYPE_TRADE* pType = (sNPC_TYPE_TRADE*)pNpc->GetType();
				for( int n=0; n<cnt; ++n )
				{
					_read( nHandle, &dwItem, sizeof( DWORD ) );
					pType->AddItem( dwItem );
				}				
			}
			break;		
		case nsCsNpcTable::NT_PORTAL:
			{
				sNPC_TYPE_PORTAL* pType = (sNPC_TYPE_PORTAL*)pNpc->GetType();
				_read( nHandle, &pType->s_nPortalType, sizeof( int ) );
				_read( nHandle, &pType->s_nPortalCount, sizeof( int ) );
				for( int p=0; p<pType->s_nPortalCount; ++p )
				{
					sNPC_TYPE_PORTAL::sPORTAL* pPortal = pType->NewPortal( false );
					_read( nHandle, pPortal, sizeof( sNPC_TYPE_PORTAL::sPORTAL ) );
				}
			}
			break;			
		case nsCsNpcTable::NT_MASTERS_MATCHING:
			{
				int cnt;
				_read( nHandle, &cnt, sizeof( int ) );

				DWORD dwItem;
				sNPC_TYPE_MASTER_MATCHING* pType = (sNPC_TYPE_MASTER_MATCHING*)pNpc->GetType();
				for( int n=0; n<cnt; ++n )
				{
					_read( nHandle, &dwItem, sizeof( DWORD ) );
					pType->AddItem( dwItem );
				}				
			}
			break;		
		case nsCsNpcTable::NT_SPECIAL_EVENT:
			{
				int nEvType;
				_read( nHandle, &nEvType, sizeof( int ) );

				sNPC_TYPE_SPECIAL_EVENT* pType = (sNPC_TYPE_SPECIAL_EVENT*)pNpc->GetType();
				pType->SetSubType( (sNPC_TYPE_SPECIAL_EVENT::eSUBTYPE)nEvType );

				DWORD dwItem;
				switch( nEvType )
				{
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_CARDGAME:		// 피에몬 카드 게임
					break;
				case sNPC_TYPE_SPECIAL_EVENT::EVENT_PINOKIMON:	// 피노키몬의 카드게임(2014 겨울이벤트)
					{
						int nItemCnt;
						_read( nHandle, &nItemCnt, sizeof( int ) );

						for( int i = 0 ; i < nItemCnt ; i++ )
						{
							_read( nHandle, &dwItem, sizeof( DWORD ) );
							pType->AddItem( dwItem );
						}
					}
					break;
				default:
					break;
				}
			}
			break;	
		default:
			assert_cs( false );
		}

		// Extra Data
		int nExtraDataCnt = 0;
		_read( nHandle, &nExtraDataCnt, sizeof( int ) );
		for( int extra=0; extra<nExtraDataCnt; ++extra )
		{
			int nExtraType;
			_read( nHandle, &nExtraType, sizeof( int ) );

			sNPC_EXTRA_BASE* pBase = pNpc->AddExtra( (nsCsNpcTable::eNPC_EXTRA)nExtraType );
			switch( nExtraType )
			{
			case nsCsNpcTable::NE_QUEST:
				{
					sNPC_EXTRA_QUEST* pQuest = (sNPC_EXTRA_QUEST*)pBase;

					// 초기 상태
					_read( nHandle, &pQuest->s_eInitState, sizeof( DWORD ) );

					// 행동 갯수
					int nActCnt;
					_read( nHandle, &nActCnt, sizeof( int ) );

					for( int nAct=0; nAct<nActCnt; ++nAct )
					{
						sNPC_EXTRA_QUEST::sACT* pAct = csnew sNPC_EXTRA_QUEST::sACT;
						pQuest->s_vpAct.PushBack( pAct );

						// 행동 타입
						_read( nHandle, &pAct->s_eActType, sizeof( DWORD ) );

						// 완료 상태
						_read( nHandle, &pAct->s_eCompState, sizeof( DWORD ) );

						// 퀘스트 갯수
						int nQuestCnt;
						_read( nHandle, &nQuestCnt, sizeof( int ) );
						for( int nQuest=0; nQuest<nQuestCnt; ++nQuest )
						{
							int nQuestID;
							_read( nHandle, &nQuestID, sizeof( int ) );
							pAct->s_list.push_back( nQuestID );
						}
					}
				}
				break;
			default:
				assert_cs( false );
			}
		}

		AddNpc( pNpc );
	}	
}

void CsNpcMng::_LoadFilePack_ModelInfo( int nHandle )
{
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );
	for( int i=0; i<nCount; ++i )
	{
		sMODEL_INFO* pInfo = csnew sMODEL_INFO;
		_read( nHandle, pInfo, sizeof( sMODEL_INFO ) );
		m_mapModelInfo[ pInfo->s_nModelID ] = pInfo;
	}
}

void CsNpcMng::_LoadFilePack_NpcEvent( int nHandle )
{
	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );
	for( int i=0; i<nCount; ++i )
	{
		sNPC_EVENT_INFO* pInfo = csnew sNPC_EVENT_INFO;
		_read( nHandle, pInfo, sizeof( sNPC_EVENT_INFO ) );
		m_mapCardGameInfo[ pInfo->s_nNpcID ] = pInfo;
	}
}
