

#include "stdafx.h"
#include "Lan_To_HeaderDlg.h"

CsParcingWord	g_ParcingWordEng;
CsParcingWord	g_ParcingWordKor;


struct sTEMP
{
	sTEMP(){ memset( str, 0, sizeof( TCHAR )*2048 ); }
	TCHAR		str[ 2048 ];
};
std::map< DWORD, sTEMP* >		mapTemp;

CsQuestMng EngMng_Org;

void CLan_To_HeaderDlg::__TitleTab( YExcel::BasicExcelWorksheet* pSheet, int nIndex )
{
	pSheet->DeleteAllRows();
	pSheet->SetTotalRows( 10 + EngMng_Org.GetQuestMap()->size() );

	int nRow = 2;
	CsQuest::MAP* pMap = EngMng_Org.GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	CsQuest::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		DWORD dwID = it->second->GetUniqID();
		pSheet->Cell( nRow, 0 )->SetInteger( dwID );

		int nNewID = dwID*100 + nIndex;
		if( mapTemp.find( nNewID ) != mapTemp.end() )
			pSheet->Cell( nRow, 1 )->SetWString( mapTemp[ nNewID ]->str );
		++nRow;
	}
}

void CLan_To_HeaderDlg::OnBnClickedButtonTempQuest()
{
	YExcel::BasicExcel excel;
	excel.New( 8 );

	excel.GetWorksheet( (size_t)0 )->Rename( "TitleTab" );
	excel.GetWorksheet( (size_t)1 )->Rename( "Title" );
	excel.GetWorksheet( (size_t)2 )->Rename( "Body" );
	excel.GetWorksheet( (size_t)3 )->Rename( "Simple" );
	excel.GetWorksheet( (size_t)4 )->Rename( "Helper" );
	excel.GetWorksheet( (size_t)5 )->Rename( "Process" );
	excel.GetWorksheet( (size_t)6 )->Rename( "Compleate" );
	excel.GetWorksheet( (size_t)7 )->Rename( "Expert" );

	char cBuf[ CEXCLE_CELL_LEN ];
	cExcelReader excelTemp;
	excelTemp.Open( "GameData\\_Etc\\_Total.xls", "Quest" );
	excelTemp.MoveFirst();
	while( excelTemp.MoveNext() )
	{		
		DWORD nID = atoi( excelTemp.GetData( 0, cBuf ) );
		if( nID == 0 )
			continue;

		sTEMP* pTemp = csnew sTEMP;
		M2W( pTemp->str, excelTemp.GetData( 2, cBuf ), 2048 );
		mapTemp[ nID ] = pTemp;
	}
	excelTemp.Close();


	nsCsFileTable::g_bUseQuest_00000 = true;
	nsCsFileTable::g_bUseQuest_02501 = false;
	nsCsFileTable::g_bUseQuestAchieve = false;
	nsCsFileTable::g_bReadInactiveQuest = true;
	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;
	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	EngMng_Org.Init( NULL );

	__TitleTab( excel.GetWorksheet( (size_t)0 ), 0 );
	__TitleTab( excel.GetWorksheet( (size_t)1 ), 1 );
	__TitleTab( excel.GetWorksheet( (size_t)2 ), 2 );
	__TitleTab( excel.GetWorksheet( (size_t)3 ), 3 );
	__TitleTab( excel.GetWorksheet( (size_t)4 ), 4 );
	__TitleTab( excel.GetWorksheet( (size_t)5 ), 5 );
	__TitleTab( excel.GetWorksheet( (size_t)6 ), 6 );
	__TitleTab( excel.GetWorksheet( (size_t)7 ), 7 );


	std::map< DWORD, sTEMP* >::iterator it = mapTemp.begin();
	std::map< DWORD, sTEMP* >::iterator itEnd = mapTemp.end();
	for( ; it!=itEnd; ++it )
	{
		delete it->second;
	}
	mapTemp.clear();

	EngMng_Org.Delete();

	excel.SaveAs( "GameData\\_Etc\\___Quest.xls" );	
	CsMessageBox( MB_OK, L"완 료" );
}

void CLan_To_HeaderDlg::OnBnClickedButtonTotalexcel()
{
	YExcel::BasicExcel excel;
	excel.New( 15 );

	excel.GetWorksheet( (size_t)0 )->Rename( "DM_Addeffect_E.xls" );
	excel.GetWorksheet( (size_t)1 )->Rename( "Digimon_Skill" );
	excel.GetWorksheet( (size_t)2 )->Rename( "Item_Skill" );
	excel.GetWorksheet( (size_t)3 )->Rename( "DM_Evolution_Digitama_E.xls" );
	excel.GetWorksheet( (size_t)4 )->Rename( "DM_Item_List_E.xls" );
	excel.GetWorksheet( (size_t)5 )->Rename( "DM_QuestItem_List_E.xls" );
	excel.GetWorksheet( (size_t)6 )->Rename( "DM_Riding_E.xls" );
	excel.GetWorksheet( (size_t)7 )->Rename( "DM_WorldMap_E.xls" );
	excel.GetWorksheet( (size_t)8 )->Rename( "DM_Talk_E.xls" );
	excel.GetWorksheet( (size_t)9 )->Rename( "Map_E.xls" );
	excel.GetWorksheet( (size_t)10 )->Rename( "DM_ParcingWord_E.xls" );
	excel.GetWorksheet( (size_t)11 )->Rename( "Npc_E.xls" );
	excel.GetWorksheet( (size_t)12 )->Rename( "Quest_00000_E.xls" );
	excel.GetWorksheet( (size_t)13 )->Rename( "Quest_02501_E.xls" );
	excel.GetWorksheet( (size_t)14 )->Rename( "QuestAchieve_E.xls" );


	SetWindowText( L"ParcingWord 로드중 ..." );
	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;
	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	g_ParcingWordKor.Init( NULL );
	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	g_ParcingWordEng.Init( NULL );


	SetWindowText( L"DM_Addeffect_E.xls 처리중 ..." );
	__DM_Addeffect_E( excel.GetWorksheet( (size_t)0 ) );

	SetWindowText( L"Digimon_Skill 처리중 ..." );
	__DM_Digimon_List_E_Skill( excel.GetWorksheet( (size_t)1 ) );

	SetWindowText( L"Item_Skill 처리중 ..." );
	__DM_Item_List_E_Skill( excel.GetWorksheet( (size_t)2 ) );

	SetWindowText( L"DM_Evolution_Digitama_E.xls 처리중 ..." );
	__DM_Evolution_Digitama_E( excel.GetWorksheet( (size_t)3 ) );

	SetWindowText( L"DM_Item_List_E.xls 처리중 ..." );
	__DM_Item_List_E( excel.GetWorksheet( (size_t)4 ) );

	SetWindowText( L"DM_QuestItem_List_E.xls 처리중 ..." );
	__DM_QuestItem_List_E( excel.GetWorksheet( (size_t)5 ) );

	SetWindowText( L"DM_Riding_E.xls 처리중 ..." );
	__DM_Riding_E( excel.GetWorksheet( (size_t)6 ) );

	SetWindowText( L"DM_WorldMap_E.xls 처리중 ..." );
	__DM_WorldMap_E( excel.GetWorksheet( (size_t)7 ) );

	SetWindowText( L"DM_Talk_E.xls 처리중 ..." );
	__DM_Talk_E( excel.GetWorksheet( (size_t)8 ) );

	SetWindowText( L"Map_E.xls 처리중 ..." );
	__DM_Map_E( excel.GetWorksheet( (size_t)9 ) );

	SetWindowText( L"DM_ParcingWord_E.xls 처리중 ..." );
	__DM_ParcingWord_E( excel.GetWorksheet( (size_t)10 ) );

	SetWindowText( L"Npc_E.xls 처리중 ..." );
	__Npc_E( excel.GetWorksheet( (size_t)11 ) );

	SetWindowText( L"Quest_00000_E.xls 처리중 ..." );
	__Quest_00000_E( excel.GetWorksheet( (size_t)12 ) );

	SetWindowText( L"Quest_02501_E.xls 처리중 ..." );
	__Quest_02501_E( excel.GetWorksheet( (size_t)13 ) );

	SetWindowText( L"QuestAchieve_E.xls 처리중 ..." );
	__QuestAchieve_E( excel.GetWorksheet( (size_t)14 ) );

	SetWindowText( L"Save ..." );
	excel.SaveAs( "GameData\\_Etc\\TotalExcel.xls" );

	SetWindowText( L"Lan_To_Header" );
	CsMessageBox( MB_OK, L"완 료" );

	g_ParcingWordKor.Delete();
	g_ParcingWordEng.Delete();
}

void CLan_To_HeaderDlg::__DM_Addeffect_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsAddExpMng KorMng;
	KorMng.Init( NULL );

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsAddExpMng EngMng;
	EngMng.Init( NULL );

	int nRow = 3;
	std::map< DWORD, CsAddExpMng::sAddExp* >* pMap = KorMng.GetMap();
	pSheet->SetTotalRows( 10+pMap->size() );

	std::map< DWORD, CsAddExpMng::sAddExp* >::iterator it = pMap->begin();
	std::map< DWORD, CsAddExpMng::sAddExp* >::iterator itEnd = pMap->end();
	int nID = 2;
	for( ; it!=itEnd; ++it )
	{
		DWORD nBit = (DWORD)pow( (double)2, (double)( nID - 1 ) );
		while( nBit != it->second->s_nID )
		{
			++nID;
			++nRow;

			nBit = (DWORD)pow( (double)2, (double)( nID - 1 ) );
		}

		pSheet->Cell( nRow, 0 )->SetInteger( nID );
		pSheet->Cell( nRow, 1 )->SetInteger( it->second->s_nID );
		pSheet->Cell( nRow, 2 )->SetWString( it->second->s_szTitle );

		pSheet->Cell( nRow, 3 )->SetWString( EngMng.GetData( it->second->s_nID )->s_szTitle );

		pSheet->Cell( nRow, 4 )->SetWString( it->second->s_szComment );

		pSheet->Cell( nRow, 5 )->SetWString( EngMng.GetData( it->second->s_nID )->s_szComment );

		++nID;
		++nRow;
	}

	KorMng.Delete();
	EngMng.Delete();
}

void CLan_To_HeaderDlg::__DM_Digimon_List_E_Skill( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsSkillMng KorMng;
	KorMng._LoadExcel( nsCsFileTable::GetPath_DM_Digimon(), "Skill_Code", CsSkill::ST_DIGIMON );

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsSkillMng EngMng;
	EngMng._LoadExcel( nsCsFileTable::GetPath_DM_Digimon(), "Skill_Code", CsSkill::ST_DIGIMON );

	int nRow = 2;
	CsSkill::MAP* pMap = KorMng.GetSkillMap();
	pSheet->SetTotalRows( 10+pMap->size() );

	CsSkill::MAP_IT it = pMap->begin();
	CsSkill::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		pSheet->Cell( nRow, 0 )->SetInteger( it->second->GetInfo()->s_dwID );
		pSheet->Cell( nRow, 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szComment ) );

		if( EngMng.IsSkill( it->second->GetInfo()->s_dwID ) )
		{
			pSheet->Cell( nRow, 2 )->SetWString( EngMng.GetSkill( it->second->GetInfo()->s_dwID )->GetInfo()->s_szComment );
		}

		++nRow;
	}

	KorMng.Delete();
	EngMng.Delete();
}

void CLan_To_HeaderDlg::__DM_Item_List_E_Skill( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsSkillMng KorMng;
	KorMng._LoadExcel( nsCsFileTable::GetPath_DM_Item(), "Skill_Code", CsSkill::ST_ITEM );

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsSkillMng EngMng;
	EngMng._LoadExcel( nsCsFileTable::GetPath_DM_Item(), "Skill_Code", CsSkill::ST_ITEM );

	int nRow = 2;
	CsSkill::MAP* pMap = KorMng.GetSkillMap();
	pSheet->SetTotalRows( 10+pMap->size() );

	CsSkill::MAP_IT it = pMap->begin();
	CsSkill::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		pSheet->Cell( nRow, 0 )->SetInteger( it->second->GetInfo()->s_dwID );
		pSheet->Cell( nRow, 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szComment ) );

		if( EngMng.IsSkill( it->second->GetInfo()->s_dwID ) )
		{
			pSheet->Cell( nRow, 2 )->SetWString( EngMng.GetSkill( it->second->GetInfo()->s_dwID )->GetInfo()->s_szComment );
		}

		++nRow;
	}

	KorMng.Delete();
	EngMng.Delete();
}

void CLan_To_HeaderDlg::__DM_Evolution_Digitama_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsTacticsMng KorMng;
	KorMng._LoadExcelTacticsExplain();
	
	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsTacticsMng EngMng;
	EngMng._LoadExcelTacticsExplain();

	int nRow = 2;
	CsTacticsExplain::MAP* pMap = KorMng.GetExplainMap();
	pSheet->SetTotalRows( 10+pMap->size() );

	CsTacticsExplain::MAP_IT it = pMap->begin();
	CsTacticsExplain::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{		
		pSheet->Cell( nRow, 0 )->SetInteger( it->second->GetInfo()->s_dwTacticsMonID );
		pSheet->Cell( nRow, 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szTacticsExplain ) );

		if( EngMng.IsTacticsExplain( it->second->GetInfo()->s_dwTacticsMonID ) )
		{
			pSheet->Cell( nRow, 2 )->SetWString( EngMng.GetTacticsExplain( it->second->GetInfo()->s_dwTacticsMonID )->GetInfo()->s_szTacticsExplain );
		}

		++nRow;
	}

	KorMng.Delete();
	EngMng.Delete();
}


void CLan_To_HeaderDlg::__DM_Item_List_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	char KorItemPath[ MAX_PATH ];
	char EngItemPath[ MAX_PATH ];
	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	strcpy_s( KorItemPath, MAX_PATH, nsCsFileTable::GetPath_DM_Item() );
	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	strcpy_s( EngItemPath, MAX_PATH, nsCsFileTable::GetPath_DM_Item() );


	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsItemMng KorMng;
	KorMng._LoadCoolTime( KorItemPath );
	KorMng._LoadExcel( KorItemPath );


	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsItemMng EngMng_Kor;
	EngMng_Kor._LoadCoolTime( EngItemPath );
	EngMng_Kor._LoadExcel( EngItemPath );


	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsItemMng EngMng_Eng;
	EngMng_Eng._LoadCoolTime( EngItemPath );
	EngMng_Eng._LoadExcel( EngItemPath );	

	int nRow = 2;
	CsItem::MAP* pMap = KorMng.GetItemMap();
	pSheet->SetTotalRows( 10+pMap->size() );
	CsItem::MAP_IT it = pMap->begin();
	CsItem::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		DWORD nItemID = it->second->GetInfo()->s_dwItemID;
		pSheet->Cell( nRow, 0 )->SetInteger( nItemID );
		pSheet->Cell( nRow, 1 )->SetWString( it->second->GetInfo()->s_szName );
		pSheet->Cell( nRow, 3 )->SetWString( it->second->GetInfo()->s_szTypeComment );

		pSheet->Cell( nRow, 5 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szComment ) );

		if( EngMng_Kor.IsItem( nItemID ) )
		{
			// 한글 이름이 같을 경우에 같은 아이템 으로 취급
			if( _tcscmp( it->second->GetInfo()->s_szName, EngMng_Kor.GetItem( nItemID )->GetInfo()->s_szName ) == 0 )
			{
				pSheet->Cell( nRow, 2 )->SetWString( EngMng_Eng.GetItem( nItemID )->GetInfo()->s_szName );
				pSheet->Cell( nRow, 6 )->SetWString( EngMng_Eng.GetItem( nItemID )->GetInfo()->s_szComment );
			}

			// 한글 유형 이름이 같을 경우에 같은 아이템 으로 취급
			if( _tcscmp( it->second->GetInfo()->s_szTypeComment, EngMng_Kor.GetItem( nItemID )->GetInfo()->s_szTypeComment ) == 0 )
			{
				pSheet->Cell( nRow, 4 )->SetWString( EngMng_Eng.GetItem( nItemID )->GetInfo()->s_szTypeComment );				
			}
		}		
		++nRow;
	}

	EngMng_Kor.Delete();
	EngMng_Eng.Delete();
	KorMng.Delete();
}

void CLan_To_HeaderDlg::__DM_QuestItem_List_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	char KorItemPath[ MAX_PATH ];
	char EngItemPath[ MAX_PATH ];
	char KorQuestItemPath[ MAX_PATH ];
	char EngQuestItemPath[ MAX_PATH ];
	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	strcpy_s( KorItemPath, MAX_PATH, nsCsFileTable::GetPath_DM_Item() );
	strcpy_s( KorQuestItemPath, MAX_PATH, nsCsFileTable::GetPath_DM_QuestItem() );
	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	strcpy_s( EngItemPath, MAX_PATH, nsCsFileTable::GetPath_DM_Item() );
	strcpy_s( EngQuestItemPath, MAX_PATH, nsCsFileTable::GetPath_DM_QuestItem() );


	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsItemMng KorMng;
	KorMng._LoadCoolTime( KorItemPath );
	KorMng._LoadExcel( KorQuestItemPath );


	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsItemMng EngMng_Kor;
	EngMng_Kor._LoadCoolTime( EngItemPath );
	EngMng_Kor._LoadExcel( EngQuestItemPath );


	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsItemMng EngMng_Eng;
	EngMng_Eng._LoadCoolTime( EngItemPath );
	EngMng_Eng._LoadExcel( EngQuestItemPath );

	
	int nRow = 2;
	CsItem::MAP* pMap = KorMng.GetItemMap();
	pSheet->SetTotalRows( 10+pMap->size() );

	CsItem::MAP_IT it = pMap->begin();
	CsItem::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		DWORD nItemID = it->second->GetInfo()->s_dwItemID;
		pSheet->Cell( nRow, 0 )->SetInteger( nItemID );
		pSheet->Cell( nRow, 1 )->SetWString( it->second->GetInfo()->s_szName );
		pSheet->Cell( nRow, 3 )->SetWString( it->second->GetInfo()->s_szTypeComment );

		pSheet->Cell( nRow, 5 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szComment ) );

		if( EngMng_Kor.IsItem( nItemID ) )
		{
			// 한글 이름이 같을 경우에 같은 아이템 으로 취급
			if( _tcscmp( it->second->GetInfo()->s_szName, EngMng_Kor.GetItem( nItemID )->GetInfo()->s_szName ) == 0 )
			{
				pSheet->Cell( nRow, 2 )->SetWString( EngMng_Eng.GetItem( nItemID )->GetInfo()->s_szName );
				pSheet->Cell( nRow, 6 )->SetWString( EngMng_Eng.GetItem( nItemID )->GetInfo()->s_szComment );
			}

			// 한글 유형 이름이 같을 경우에 같은 아이템 으로 취급
			if( _tcscmp( it->second->GetInfo()->s_szTypeComment, EngMng_Kor.GetItem( nItemID )->GetInfo()->s_szTypeComment ) == 0 )
			{
				pSheet->Cell( nRow, 4 )->SetWString( EngMng_Eng.GetItem( nItemID )->GetInfo()->s_szTypeComment );
			}
		}		
		++nRow;
	}
	

	EngMng_Kor.Delete();
	EngMng_Eng.Delete();
	KorMng.Delete();
}

void CLan_To_HeaderDlg::__DM_Riding_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsDigimonMng KorMng;
	KorMng.__LoadExcelRide();

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsDigimonMng EngMng;
	EngMng.__LoadExcelRide();

	int nRow = 2;
	CsRide::MAP* pMap = KorMng.GetRideMap();
	pSheet->SetTotalRows( 10+pMap->size() );

	CsRide::MAP_IT it = pMap->begin();
	CsRide::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{		
		pSheet->Cell( nRow, 0 )->SetInteger( it->second->GetInfo()->s_dwDigimonID );
		pSheet->Cell( nRow, 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szComment ) );

		if( EngMng.IsRide( it->second->GetInfo()->s_dwDigimonID ) )
		{
			pSheet->Cell( nRow, 2 )->SetWString( EngMng.GetRide( it->second->GetInfo()->s_dwDigimonID )->GetInfo()->s_szComment );
		}
		++nRow;
	}

	KorMng.Delete();
	EngMng.Delete();
}

void CLan_To_HeaderDlg::__DM_WorldMap_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsWorldMapMng KorMng;
	KorMng.Init( NULL );

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsWorldMapMng EngMng;
	EngMng.Init( NULL );

	pSheet->SetTotalRows( 10 + CsMax( KorMng.GetWorldList()->size(), KorMng.GetAreaList()->size() ) );
	{
		int nRow = 2;
		CsWorldMap::MAP* pMap = KorMng.GetWorldList();
		CsWorldMap::MAP_IT it = pMap->begin();
		CsWorldMap::MAP_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szComment ) );
			if( EngMng.IsWorld( it->second->GetInfo()->s_nID ) )
			{
				pSheet->Cell( nRow, 2 )->SetWString( EngMng.GetWorld( it->second->GetInfo()->s_nID )->GetInfo()->s_szComment );
			}
			++nRow;
		}
	}

	{
		int nRow = 2;
		CsAreaMap::MAP* pMap = KorMng.GetAreaList();
		CsAreaMap::MAP_IT it = pMap->begin();
		CsAreaMap::MAP_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, 3 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szComment ) );
			if( EngMng.IsArea( it->second->GetInfo()->s_nMapID ) )
			{
				pSheet->Cell( nRow, 4 )->SetWString( EngMng.GetArea( it->second->GetInfo()->s_nMapID )->GetInfo()->s_szComment );
			}
			++nRow;
		}		
	}
	
	KorMng.Delete();
	EngMng.Delete();
}

void CLan_To_HeaderDlg::__DM_Talk_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsTalkMng KorMng;
	KorMng.Init( NULL );

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsTalkMng EngMng;
	EngMng.Init( NULL );

	int nMax = CsMax( (int)KorMng.GetMap_Digimon()->size(), (int)KorMng.GetMap_Event()->size() );
	nMax = CsMax( (int)KorMng.GetMap_LoadingTip()->size(), nMax );
	nMax = CsMax( (int)KorMng.GetMap_Message()->size(), nMax );
	nMax = CsMax( (int)KorMng.GetMap_Tip()->size(), nMax );

	pSheet->SetTotalRows( 10 + nMax );

	int nCol = 0;
	{
		int nRow = 2;
		CsTalk_Digimon::MAP* pMap = KorMng.GetMap_Digimon();
		CsTalk_Digimon::MAP_IT it = pMap->begin();
		CsTalk_Digimon::MAP_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, nCol + 0 )->SetInteger( it->first );
			pSheet->Cell( nRow, nCol + 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szText ) );
			pSheet->Cell( nRow, nCol + 3 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szList ) );

			if( EngMng.IsExist_Digimon( it->first ) == true )
			{
				pSheet->Cell( nRow, nCol + 2 )->SetWString( EngMng.GetTalk_Digimon( it->first )->GetInfo()->s_szText );
				pSheet->Cell( nRow, nCol + 4 )->SetWString( EngMng.GetTalk_Digimon( it->first )->GetInfo()->s_szList );
			}
			++nRow;
		}
	}

	{
		nCol += 6;
		int nRow = 2;
		CsTalk_Event::MAP* pMap = KorMng.GetMap_Event();
		CsTalk_Event::MAP_IT it = pMap->begin();
		CsTalk_Event::MAP_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, nCol + 0 )->SetInteger( it->second->GetInfo()->s_dwTalkNum );
			pSheet->Cell( nRow, nCol + 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szText ) );

			if( EngMng.IsExist_Event( it->second->GetInfo()->s_dwTalkNum ) == true )
			{
				pSheet->Cell( nRow, nCol + 2 )->SetWString( EngMng.GetTalk_Event( it->second->GetInfo()->s_dwTalkNum )->GetInfo()->s_szText );
			}
			++nRow;
		}
	}

	{
		nCol += 4;
		int nRow = 2;
		CsLoading_Tip::MAP* pMap = KorMng.GetMap_LoadingTip();
		CsLoading_Tip::MAP_IT it = pMap->begin();
		CsLoading_Tip::MAP_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, nCol + 0 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szLoadingTip ) );
			if( EngMng.IsExist_LoadingTip( it->first ) == true )
			{
				pSheet->Cell( nRow, nCol + 1 )->SetWString( EngMng.GetTalk_LoadingTip( it->first )->GetInfo()->s_szLoadingTip );
			}
			++nRow;
		}
	}

	{
		nCol += 3;
		int nRow = 2;
		CsTalk_Message::MAP* pMap = KorMng.GetMap_Message();
		CsTalk_Message::MAP_IT it = pMap->begin();
		CsTalk_Message::MAP_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, nCol + 0 )->SetInteger( it->second->GetInfo()->s_dwID );
			pSheet->Cell( nRow, nCol + 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_Message ) );

			if( EngMng.IsExist_Message( it->second->GetInfo()->s_dwID ) == true )
			{
				pSheet->Cell( nRow, nCol + 2 )->SetWString( EngMng.GetTalk_Message( it->second->GetInfo()->s_dwID )->GetInfo()->s_Message );
			}
			++nRow;
		}
	}

	{
		nCol += 4;
		int nRow = 2;
		CsTalk_Tip::MAP* pMap = KorMng.GetMap_Tip();
		CsTalk_Tip::MAP_IT it = pMap->begin();
		CsTalk_Tip::MAP_IT itEnd = pMap->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, nCol + 0 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szTip ) );

			if( EngMng.IsExist_Tip( it->first ) == true )
			{
				pSheet->Cell( nRow, nCol + 1 )->SetWString( EngMng.GetTalk_Tip( it->first )->GetInfo()->s_szTip );
			}
			++nRow;
		}
	}

	KorMng.Delete();
	EngMng.Delete();
}

void CLan_To_HeaderDlg::__DM_Map_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsMapRegionMng KorMng;
	KorMng.Init( NULL );

	CsMapRegionGroup::MAP* pMapGroup = KorMng.GetRegionGroupMap();
	CsMapRegionGroup::MAP_IT itGroup = pMapGroup->begin();
	CsMapRegionGroup::MAP_IT itEndGroup = pMapGroup->end();
	int nCnt = 0;
	for( ; itGroup != itEndGroup; ++itGroup )
	{
		nCnt += itGroup->second->GetRegionCount();
	}
	pSheet->SetTotalRows( 10 + nCnt );
	

	int nRow = 2;
	itGroup = pMapGroup->begin();
	itEndGroup = pMapGroup->end();
	for( ; itGroup != itEndGroup; ++itGroup )
	{
		CsMapRegion::LIST* pList = itGroup->second->GetRegionList();
		CsMapRegion::LIST_IT it = pList->begin();
		CsMapRegion::LIST_IT itEnd = pList->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, 0 )->SetWString( (*it)->GetInfo()->s_szDiscript );
			pSheet->Cell( nRow, 1 )->SetWString( (*it)->GetInfo()->s_szDiscript_Eng );
			++nRow;
		}
	}

	KorMng.Delete();
}

void CLan_To_HeaderDlg::__DM_ParcingWord_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	CsParcingWord::MAP* pMapNpc = g_ParcingWordKor.GetMap( CsParcingWord::TYPE_NPC );
	CsParcingWord::MAP* pMapMon = g_ParcingWordKor.GetMap( CsParcingWord::TYPE_MON );
	CsParcingWord::MAP* pMapMap = g_ParcingWordKor.GetMap( CsParcingWord::TYPE_MAP );

	int nMax = CsMax( (int)pMapNpc->size(), (int)pMapMon->size() );
	nMax = CsMax( nMax, (int)pMapMap->size() );
	pSheet->SetTotalRows( 10 + nMax );

	int nCol = 0;
	{
		int nRow = 2;

		CsParcingWord::MAP_IT it = pMapNpc->begin();
		CsParcingWord::MAP_IT itEnd = pMapNpc->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, nCol + 0 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->s_szBody ) );
			CsParcingWord::sINFO* pEngInfo = g_ParcingWordEng.GetMap( CsParcingWord::TYPE_NPC )->find( it->second->s_dwPacingNum )->second;
			pSheet->Cell( nRow, nCol + 1 )->SetWString( pEngInfo->s_szBody );

			++nRow;
		}

		nCol += 3;
	}

	{
		int nRow = 2;

		CsParcingWord::MAP_IT it = pMapMon->begin();
		CsParcingWord::MAP_IT itEnd = pMapMon->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, nCol + 0 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->s_szBody ) );			
			CsParcingWord::sINFO* pEngInfo = g_ParcingWordEng.GetMap( CsParcingWord::TYPE_MON )->find( it->second->s_dwPacingNum )->second;
			pSheet->Cell( nRow, nCol + 1 )->SetWString( pEngInfo->s_szBody );

			++nRow;
		}

		nCol += 3;
	}

	{
		int nRow = 2;

		CsParcingWord::MAP_IT it = pMapMap->begin();
		CsParcingWord::MAP_IT itEnd = pMapMap->end();
		for( ; it!=itEnd; ++it )
		{
			pSheet->Cell( nRow, nCol + 0 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->s_szBody ) );			
			CsParcingWord::sINFO* pEngInfo = g_ParcingWordEng.GetMap( CsParcingWord::TYPE_MAP )->find( it->second->s_dwPacingNum )->second;
			pSheet->Cell( nRow, nCol + 1 )->SetWString( pEngInfo->s_szBody );

			++nRow;
		}

		nCol += 3;
	}
}


void CLan_To_HeaderDlg::__Npc_E( YExcel::BasicExcelWorksheet* pSheet )
{
	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsNpcMng KorMng;
	KorMng.Init( NULL );

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsNpcMng EngMng;
	EngMng.Init( NULL );

	pSheet->SetTotalRows( 10 + KorMng.GetNpcMap()->size() );

	int nRow = 2;
	CsNpc::MAP* pMap = KorMng.GetNpcMap();
	CsNpc::MAP_IT it = pMap->begin();
	CsNpc::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		DWORD dwID = it->second->GetInfo()->s_dwNpcID;
		pSheet->Cell( nRow, 0 )->SetInteger( dwID );
		pSheet->Cell( nRow, 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szPenName ) );
		pSheet->Cell( nRow, 4 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szName ) );
		pSheet->Cell( nRow, 7 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->GetInfo()->s_szTalk ) );

		if( EngMng.IsNpc( dwID ) )
		{
			pSheet->Cell( nRow, 2 )->SetWString( EngMng.GetNpc( dwID )->GetInfo()->s_szPenName );
			pSheet->Cell( nRow, 5 )->SetWString( EngMng.GetNpc( dwID )->GetInfo()->s_szName );
			pSheet->Cell( nRow, 8 )->SetWString( EngMng.GetNpc( dwID )->GetInfo()->s_szTalk );
		}
		++nRow;
	}

	KorMng.Delete();
	EngMng.Delete();
}

void CLan_To_HeaderDlg::__Quest_00000_E( YExcel::BasicExcelWorksheet* pSheet )
{
	nsCsFileTable::g_bUseQuest_00000 = true;
	nsCsFileTable::g_bUseQuest_02501 = false;
	nsCsFileTable::g_bUseQuestAchieve = false;
	nsCsFileTable::g_bReadInactiveQuest = true;

	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsQuestMng KorMng;
	KorMng.Init( NULL );

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsQuestMng EngMng;
	EngMng.Init( NULL );

	pSheet->SetTotalRows( 10 + KorMng.GetQuestMap()->size() );

	int nRow = 2;
	CsQuest::MAP* pMap = KorMng.GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	CsQuest::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		DWORD dwID = it->second->GetUniqID();
		pSheet->Cell( nRow, 0 )->SetInteger( dwID );
		pSheet->Cell( nRow, 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szTitleTab ) );
		pSheet->Cell( nRow, 4 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szTitleText ) );
		pSheet->Cell( nRow, 7 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szBodyText ) );
		pSheet->Cell( nRow, 10 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szSimpleText ) );
		pSheet->Cell( nRow, 13 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szHelperText ) );
		pSheet->Cell( nRow, 16 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szProcess ) );
		pSheet->Cell( nRow, 19 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szCompleate ) );
		pSheet->Cell( nRow, 22 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szExpert ) );

		if( EngMng.IsQuest( dwID ) )
		{
			pSheet->Cell( nRow, 2 )->SetWString( EngMng.GetQuest( dwID )->m_szTitleTab );
			pSheet->Cell( nRow, 5 )->SetWString( EngMng.GetQuest( dwID )->m_szTitleText );
			pSheet->Cell( nRow, 8 )->SetWString( EngMng.GetQuest( dwID )->m_szBodyText );
			pSheet->Cell( nRow, 11 )->SetWString( EngMng.GetQuest( dwID )->m_szSimpleText );
			pSheet->Cell( nRow, 14 )->SetWString( EngMng.GetQuest( dwID )->m_szHelperText );
			pSheet->Cell( nRow, 17 )->SetWString( EngMng.GetQuest( dwID )->m_szProcess );
			pSheet->Cell( nRow, 20 )->SetWString( EngMng.GetQuest( dwID )->m_szCompleate );
			pSheet->Cell( nRow, 23 )->SetWString( EngMng.GetQuest( dwID )->m_szExpert );
		}
		++nRow;
	}

	KorMng.Delete();
	EngMng.Delete();
}

void CLan_To_HeaderDlg::__Quest_02501_E( YExcel::BasicExcelWorksheet* pSheet )
{
	nsCsFileTable::g_bUseQuest_00000 = false;
	nsCsFileTable::g_bUseQuest_02501 = true;
	nsCsFileTable::g_bUseQuestAchieve = false;
	nsCsFileTable::g_bReadInactiveQuest = true;

	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsQuestMng KorMng;
	KorMng.Init( NULL );

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsQuestMng EngMng;
	EngMng.Init( NULL );

	pSheet->SetTotalRows( 10 + KorMng.GetQuestMap()->size() );

	int nRow = 2;
	CsQuest::MAP* pMap = KorMng.GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	CsQuest::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		DWORD dwID = it->second->GetUniqID();
		pSheet->Cell( nRow, 0 )->SetInteger( dwID );
		pSheet->Cell( nRow, 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szTitleTab ) );
		pSheet->Cell( nRow, 4 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szTitleText ) );
		pSheet->Cell( nRow, 7 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szBodyText ) );
		pSheet->Cell( nRow, 10 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szSimpleText ) );
		pSheet->Cell( nRow, 13 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szHelperText ) );
		pSheet->Cell( nRow, 16 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szProcess ) );
		pSheet->Cell( nRow, 19 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szCompleate ) );
		pSheet->Cell( nRow, 22 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szExpert ) );

		if( EngMng.IsQuest( dwID ) )
		{
			pSheet->Cell( nRow, 2 )->SetWString( EngMng.GetQuest( dwID )->m_szTitleTab );
			pSheet->Cell( nRow, 5 )->SetWString( EngMng.GetQuest( dwID )->m_szTitleText );
			pSheet->Cell( nRow, 8 )->SetWString( EngMng.GetQuest( dwID )->m_szBodyText );
			pSheet->Cell( nRow, 11 )->SetWString( EngMng.GetQuest( dwID )->m_szSimpleText );
			pSheet->Cell( nRow, 14 )->SetWString( EngMng.GetQuest( dwID )->m_szHelperText );
			pSheet->Cell( nRow, 17 )->SetWString( EngMng.GetQuest( dwID )->m_szProcess );
			pSheet->Cell( nRow, 20 )->SetWString( EngMng.GetQuest( dwID )->m_szCompleate );
			pSheet->Cell( nRow, 23 )->SetWString( EngMng.GetQuest( dwID )->m_szExpert );
		}
		++nRow;
	}

	KorMng.Delete();
	EngMng.Delete();
}

void CLan_To_HeaderDlg::__QuestAchieve_E( YExcel::BasicExcelWorksheet* pSheet )
{
	nsCsFileTable::g_bUseQuest_00000 = false;
	nsCsFileTable::g_bUseQuest_02501 = false;
	nsCsFileTable::g_bUseQuestAchieve = true;
	nsCsFileTable::g_bReadInactiveQuest = true;

	pSheet->DeleteAllRows();

	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;

	nsCsFileTable::g_eLanguage = nsCsFileTable::KOREA_TRANSLATION;
	CsQuestMng KorMng;
	KorMng.Init( NULL );

	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	CsQuestMng EngMng;
	EngMng.Init( NULL );

	pSheet->SetTotalRows( 10 + KorMng.GetQuestMap()->size() );

	int nRow = 2;
	CsQuest::MAP* pMap = KorMng.GetQuestMap();
	CsQuest::MAP_IT it = pMap->begin();
	CsQuest::MAP_IT itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		DWORD dwID = it->second->GetUniqID();
		pSheet->Cell( nRow, 0 )->SetInteger( dwID );
		pSheet->Cell( nRow, 1 )->SetWString( g_ParcingWordKor.Parcing_To_Eng( &g_ParcingWordEng, it->second->m_szTitleText ) );

		if( EngMng.IsQuest( dwID ) )
		{
			pSheet->Cell( nRow, 2 )->SetWString( EngMng.GetQuest( dwID )->m_szTitleText );
		}
		++nRow;
	}

	KorMng.Delete();
	EngMng.Delete();
}