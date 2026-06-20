
#include "stdafx.h"
#include "FileTableMng.h"

CsFileTableMng::CsFileTableMng()
{
}

void CsFileTableMng::Delete()
{
#define DELETE_MNG( mng )\
	if( mng )\
	{\
		mng->Delete();\
		SAFE_DELETE( mng );\
	}

	DELETE_MNG( nsCsFileTable::g_pQuestMng );
	DELETE_MNG( nsCsFileTable::g_pDigimonMng );
	DELETE_MNG( nsCsFileTable::g_pMonsterMng );
	DELETE_MNG( nsCsFileTable::g_pNpcMng );
	DELETE_MNG( nsCsFileTable::g_pMapMng );
	DELETE_MNG( nsCsFileTable::g_pItemMng );
	DELETE_MNG( nsCsFileTable::g_pBaseMng );
	DELETE_MNG( nsCsFileTable::g_pSkillMng );
	DELETE_MNG( nsCsFileTable::g_pNatureMng );
	DELETE_MNG( nsCsFileTable::g_pCuidMng );
	DELETE_MNG( nsCsFileTable::g_pTalkMng );
	DELETE_MNG( nsCsFileTable::g_pTacticsMng );
	DELETE_MNG( nsCsFileTable::g_pWorldMapMng );
	DELETE_MNG( nsCsFileTable::g_pTamerMng );
	DELETE_MNG( nsCsFileTable::g_pLocalizingMng );
	DELETE_MNG( nsCsFileTable::g_pHelpMng );
	DELETE_MNG( nsCsFileTable::g_pEvolMng );
	DELETE_MNG( nsCsFileTable::g_pMoveObjectMng );
	DELETE_MNG( nsCsFileTable::g_pRewardMng );
	DELETE_MNG( nsCsFileTable::g_pMaster_CardMng );
	DELETE_MNG( nsCsFileTable::g_pEventMng );	
	DELETE_MNG( nsCsFileTable::g_pTotalExcelMng );	
	DELETE_MNG( nsCsFileTable::g_pCashShopMng );
	DELETE_MNG( nsCsFileTable::g_pAchieveMng );
	DELETE_MNG( nsCsFileTable::g_pSceneDataMng );
	DELETE_MNG( nsCsFileTable::g_pBuffMng );
	DELETE_MNG(	nsCsFileTable::g_pAddExpMng );
	DELETE_MNG(	nsCsFileTable::g_pGotchaMng );
	DELETE_MNG(	nsCsFileTable::g_pBattleTableMng );
	DELETE_MNG(	nsCsFileTable::g_pServerTranferTBMng );
	DELETE_MNG(	nsCsFileTable::g_pCharCreateTBMng );
	DELETE_MNG(	nsCsFileTable::g_pUITextTBMng );
	DELETE_MNG(	nsCsFileTable::g_pInfiniteWarTBMng );
	DELETE_MNG(	nsCsFileTable::g_pDigimonParcelTBMng );
	DELETE_MNG(	nsCsFileTable::g_pExtraEvolutionTBMng );
	DELETE_MNG(	nsCsFileTable::g_pEffectTBMng );
	DELETE_MNG(	nsCsFileTable::g_pTimeChargeMng );
}

char* CsFileTableMng::GetLanguagePath( nsCsFileTable::eLANGUAGE eLanguage, char* cBuffer )
{
	switch( eLanguage )
	{
	case nsCsFileTable::KOREA_TRANSLATION:
	case nsCsFileTable::KOREA_ORIGINAL:
		strcpy_s( cBuffer, MAX_PATH, "Data\\Bin\\Korea_Tr" );
		break;
	case nsCsFileTable::ENGLISH:
		strcpy_s( cBuffer, MAX_PATH, "Data\\Bin\\English" );
		break;
	case nsCsFileTable::ENGLISH_A:
		strcpy_s( cBuffer, MAX_PATH, "Data\\Bin\\English_A" );
		break;
	case nsCsFileTable::THAILAND:
		strcpy_s( cBuffer, MAX_PATH, "Data\\Bin\\Thailand" );
		break;
	case nsCsFileTable::TAIWAN:
		strcpy_s( cBuffer, MAX_PATH, "Data\\Bin\\Taiwan" );
		break;
	case nsCsFileTable::HONGKONG:
		strcpy_s( cBuffer, MAX_PATH, "Data\\Bin\\HongKong" );
		break;
	default:
		assert_cs( false );
	}
	return cBuffer;
}

bool CsFileTableMng::Init( nsCsFileTable::eFILE_TYPE eFileType, nsCsFileTable::eLANGUAGE eLanguage )
{
#define INIT_MNG_PATH( class, mng, bUse )\
	if( ( bUse == true )&&( mng == NULL ) )\
	{\
		mng = csnew class;\
		if( mng->Init( cPath ) == false )\
			return false;\
	}
#define INIT_MNG_NOPATH( class, mng, bUse )\
	if( ( bUse == true )&&( mng == NULL ) )\
	{\
		mng = csnew class;\
		if( mng->Init() == false )\
			return false;\
	}

	nsCsFileTable::g_eFileType = eFileType;
	nsCsFileTable::g_eLanguage = eLanguage;

#ifdef _GIVE
	if( nsCsFileTable::g_eFileType != nsCsFileTable::FT_FILEPACK )
	{
		CsMessageBox( MB_OK, _T( "FileTable Type not FT_FILEPACK" ) );
	}	
#endif

	char cPath[ MAX_PATH ];
	GetLanguagePath( eLanguage, cPath );

	INIT_MNG_PATH( CsTotalExcelMng, nsCsFileTable::g_pTotalExcelMng, nsCsFileTable::g_bUseTotalExcel_None );
	INIT_MNG_PATH( CsDigimonMng, nsCsFileTable::g_pDigimonMng, nsCsFileTable::g_bUseDigimon );
	INIT_MNG_PATH( CsMonsterMng, nsCsFileTable::g_pMonsterMng, nsCsFileTable::g_bUseMonster );	
	// Npc는 Map다음에 와야 한다.
	INIT_MNG_PATH( CsMapMng, nsCsFileTable::g_pMapMng, nsCsFileTable::g_bUseMap );
	INIT_MNG_PATH( CsNpcMng, nsCsFileTable::g_pNpcMng, nsCsFileTable::g_bUseNpc );
	INIT_MNG_PATH( CsQuestMng, nsCsFileTable::g_pQuestMng, nsCsFileTable::g_bUseQuest );

	INIT_MNG_PATH( CsItemMng, nsCsFileTable::g_pItemMng, nsCsFileTable::g_bUseItem );
	INIT_MNG_PATH( CsBaseMng, nsCsFileTable::g_pBaseMng, nsCsFileTable::g_bUseBase );
	INIT_MNG_PATH( CsSkillMng, nsCsFileTable::g_pSkillMng, nsCsFileTable::g_bUseSkill );
	INIT_MNG_PATH( CsNatureMng, nsCsFileTable::g_pNatureMng, nsCsFileTable::g_bUseNature );
	INIT_MNG_PATH( CsCuidMng, nsCsFileTable::g_pCuidMng, nsCsFileTable::g_bUseCuid );	
	INIT_MNG_PATH( CsEventTable, nsCsFileTable::g_pEventMng, nsCsFileTable::g_bUseEvent );	
 	INIT_MNG_PATH( CsAddExpMng, nsCsFileTable::g_pAddExpMng, nsCsFileTable::g_bAddExp );
	INIT_MNG_PATH( CsCashShopMng, nsCsFileTable::g_pCashShopMng, nsCsFileTable::g_bUseCashShop );
	// Achieve 는 Quest 다음에 와야 한다.
	INIT_MNG_PATH( CsAchieveMng, nsCsFileTable::g_pAchieveMng, nsCsFileTable::g_bUseAchieve );
	INIT_MNG_PATH( CsSceneDataMng, nsCsFileTable::g_pSceneDataMng, nsCsFileTable::g_bSceneDataMng );
	INIT_MNG_PATH( CsBuffMng, nsCsFileTable::g_pBuffMng, nsCsFileTable::g_bBuffMng );
	INIT_MNG_PATH( CsGotchaMng, nsCsFileTable::g_pGotchaMng, nsCsFileTable::g_bUseGotcha );

	nsCsFileTable::g_pEvolMng = csnew CDigimonEvolution;
	if( nsCsFileTable::g_pEvolMng->Init( cPath ) == false )
		return false;

	nsCsFileTable::g_pTalkMng = csnew CsTalkMng;
	if( nsCsFileTable::g_pTalkMng->Init( cPath ) == false )
		return false;

	nsCsFileTable::g_pTacticsMng = csnew CsTacticsMng;
	if( nsCsFileTable::g_pTacticsMng->Init( cPath ) == false )
		return false;

	nsCsFileTable::g_pWorldMapMng = csnew CsWorldMapMng;
	if( nsCsFileTable::g_pWorldMapMng->Init( cPath ) == false )
		return false;

	nsCsFileTable::g_pTamerMng = csnew CsTamerMng;
	if( nsCsFileTable::g_pTamerMng->Init( cPath ) == false )
		return false;

	INIT_MNG_NOPATH( CsHelpMng, nsCsFileTable::g_pHelpMng, nsCsFileTable::g_bUseHelp );
	INIT_MNG_NOPATH( CsMoveObjectMng, nsCsFileTable::g_pMoveObjectMng, nsCsFileTable::g_bUseMoveObject );	
	INIT_MNG_PATH( CsRewardMng, nsCsFileTable::g_pRewardMng, nsCsFileTable::g_bUseReward );
	INIT_MNG_PATH( CsMaster_CardMng, nsCsFileTable::g_pMaster_CardMng, nsCsFileTable::g_bUseMaster_CardMng );
	INIT_MNG_PATH( CsBattleTableMng, nsCsFileTable::g_pBattleTableMng, nsCsFileTable::g_bBattleTableMng );
	INIT_MNG_PATH( CsServerRelocateTBMng, nsCsFileTable::g_pServerTranferTBMng, nsCsFileTable::g_bLoadServerTranferTBMng );
	INIT_MNG_PATH( CsCharCreateTableMng, nsCsFileTable::g_pCharCreateTBMng, nsCsFileTable::g_bLoadCharCreateTBMng );
	INIT_MNG_PATH( CsUITextMng, nsCsFileTable::g_pUITextTBMng, nsCsFileTable::g_bUITextTBMng);
	INIT_MNG_PATH( CsInfiniteWarMng, nsCsFileTable::g_pInfiniteWarTBMng, nsCsFileTable::g_bInfiniteWarTBMng);
	INIT_MNG_PATH( CsDigimonParcelMng, nsCsFileTable::g_pDigimonParcelTBMng, nsCsFileTable::g_bDigimonParcelTBMng);
	INIT_MNG_PATH( CsExtraEvolutionMng, nsCsFileTable::g_pExtraEvolutionTBMng, nsCsFileTable::g_bExtraEvolutionTBMng);
	INIT_MNG_PATH( CsEffectMng, nsCsFileTable::g_pEffectTBMng, nsCsFileTable::g_bEffectTBMng);
	INIT_MNG_PATH( CsTimeChargeMng, nsCsFileTable::g_pTimeChargeMng, nsCsFileTable::g_bTimeChargeMng );
	return true;
}



void CsFileTableMng::SaveBin( nsCsFileTable::eLANGUAGE eLanguage )
{
	nsCsFileTable::g_eLanguage = eLanguage;
	// bin 폴더가 없다면 폴더 생성

	if( _taccess_s( _T( "Data" ), 0 ) != 0 )
	{
		CreateDirectory( _T( "Data" ), NULL );
	}
	if( _taccess_s( _T( "Data\\Bin" ), 0 ) != 0 )
	{
		CreateDirectory( _T( "Data\\Bin" ), NULL );
	}

	switch( eLanguage )
	{
	case nsCsFileTable::KOREA_TRANSLATION:
		if( _taccess_s( _T( "Data\\Bin\\Korea_Tr" ), 0 ) != 0 )
		{
			CreateDirectory( _T( "Data\\Bin\\Korea_Tr" ), NULL );
		}
		break;

	case nsCsFileTable::KOREA_ORIGINAL:
		if( _taccess_s( _T( "Data\\Bin\\Korea_Org" ), 0 ) != 0 )
		{
			CreateDirectory( _T( "Data\\Bin\\Korea_Org" ), NULL );
		}
		break;

	case nsCsFileTable::ENGLISH:
		if( _taccess_s( _T( "Data\\Bin\\English" ), 0 ) != 0 )
		{
			CreateDirectory( _T( "Data\\Bin\\English" ), NULL );
		}
		break;
	case nsCsFileTable::ENGLISH_A:
		if( _taccess_s( _T( "Data\\Bin\\English_A" ), 0 ) != 0 )
		{
			CreateDirectory( _T( "Data\\Bin\\English_A" ), NULL );
		}
		break;
	case nsCsFileTable::THAILAND:
		if( _taccess_s( _T( "Data\\Bin\\Thailand" ), 0 ) != 0 )
		{
			CreateDirectory( _T( "Data\\Bin\\Thailand" ), NULL );
		}
		break;

	case nsCsFileTable::TAIWAN:
		if( _taccess_s( _T( "Data\\Bin\\Taiwan" ), 0 ) != 0 )
		{
			CreateDirectory( _T( "Data\\Bin\\Taiwan" ), NULL );
		}
		break;

	case nsCsFileTable::HONGKONG:
		if( _taccess_s( _T( "Data\\Bin\\HongKong" ), 0 ) != 0 )
		{
			CreateDirectory( _T( "Data\\Bin\\HongKong" ), NULL );
		}
		break;

	default:
		assert_cs( false );
	}
	

	char cPath[ MAX_PATH ];
	GetLanguagePath( eLanguage, cPath );

#define SAVE_BIN_MNG( mng )	if( mng != NULL ){ mng->SaveBin( cPath ); }

	switch( eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
		SAVE_BIN_MNG( nsCsFileTable::g_pSkillMng );
		return;		
	}
	SAVE_BIN_MNG( nsCsFileTable::g_pDigimonMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pMonsterMng );	
	// Npc는 Map다음에 와야 한다.
	SAVE_BIN_MNG( nsCsFileTable::g_pMapMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pNpcMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pQuestMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pItemMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pBaseMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pSkillMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pNatureMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pCuidMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pEvolMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pTalkMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pTacticsMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pWorldMapMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pTamerMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pEventMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pAddExpMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pCashShopMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pAchieveMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pBuffMng );
	SAVE_BIN_MNG( nsCsFileTable::g_pSceneDataMng );
	SAVE_BIN_MNG(nsCsFileTable::g_pGotchaMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pRewardMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pMaster_CardMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pBattleTableMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pServerTranferTBMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pCharCreateTBMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pUITextTBMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pInfiniteWarTBMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pDigimonParcelTBMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pExtraEvolutionTBMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pEffectTBMng);
	SAVE_BIN_MNG(nsCsFileTable::g_pTimeChargeMng);
}


void CsFileTableMng::StringChange( nsCsFileTable::eLANGUAGE eLanguage )
{
	__CsStringChange sc;
	sc.Load( eLanguage );

	// 테이블 변경
	assert_cs( nsCsFileTable::g_pSkillMng );
	nsCsFileTable::g_pSkillMng->StringChange( &sc );


	sc.Delete();
}

