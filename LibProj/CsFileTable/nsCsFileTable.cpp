

#include "stdafx.h"
#include "nsCsFileTable.h"

nsCsFileTable::eFILE_TYPE		nsCsFileTable::g_eFileType;
nsCsFileTable::eLANGUAGE		nsCsFileTable::g_eLanguage;
CsFileTableMng					nsCsFileTable::g_FileTableMng;

bool							nsCsFileTable::g_bUseQuest = true;
bool							nsCsFileTable::g_bReadInactiveQuest = false;
CsQuestMng*						nsCsFileTable::g_pQuestMng = NULL;

bool							nsCsFileTable::g_bUseDigimon = true;
CsDigimonMng*					nsCsFileTable::g_pDigimonMng = NULL;

bool							nsCsFileTable::g_bUseMonsterDrop = false;
bool							nsCsFileTable::g_bUseMonster = true;
CsMonsterMng*					nsCsFileTable::g_pMonsterMng = NULL;

bool							nsCsFileTable::g_bUseNpc = true;
CsNpcMng*						nsCsFileTable::g_pNpcMng = NULL;

bool							nsCsFileTable::g_bUseMap = true;
CsMapMng*						nsCsFileTable::g_pMapMng = NULL;

bool							nsCsFileTable::g_bUseItem = true;
CsItemMng*						nsCsFileTable::g_pItemMng = NULL;

bool							nsCsFileTable::g_bUseBase = true;
CsBaseMng*						nsCsFileTable::g_pBaseMng = NULL;

bool							nsCsFileTable::g_bUseSkill = true;
CsSkillMng*						nsCsFileTable::g_pSkillMng = NULL;

bool							nsCsFileTable::g_bUseNature = true;
CsNatureMng*					nsCsFileTable::g_pNatureMng = NULL;

bool							nsCsFileTable::g_bUseMessage = true;
CsMessageMng*					nsCsFileTable::g_pMessageMng = NULL;

bool							nsCsFileTable::g_bUseCuid = true;
CsCuidMng*						nsCsFileTable::g_pCuidMng = NULL;

CDigimonEvolution*				nsCsFileTable::g_pEvolMng = NULL;
bool							nsCsFileTable::g_bUseEvoExtra = true;
bool							nsCsFileTable::g_bUseEvoBattle = true;

CsTalkMng*						nsCsFileTable::g_pTalkMng = NULL;
CsTacticsMng*					nsCsFileTable::g_pTacticsMng = NULL;
CsWorldMapMng*					nsCsFileTable::g_pWorldMapMng = NULL;
CsTamerMng*						nsCsFileTable::g_pTamerMng = NULL;

bool							nsCsFileTable::g_bUseHelp = false;
CsHelpMng*						nsCsFileTable::g_pHelpMng = NULL;


bool							nsCsFileTable::g_bUseMoveObject = false;
CsMoveObjectMng*				nsCsFileTable::g_pMoveObjectMng = NULL;

bool							nsCsFileTable::g_bUseTotalExcel_None = false;
CsTotalExcelMng*				nsCsFileTable::g_pTotalExcelMng = NULL;

bool							nsCsFileTable::g_bUseCashShop = false;
CsCashShopMng*					nsCsFileTable::g_pCashShopMng = NULL;
//CsCashShopMng_Steam*			nsCsFileTable::g_pCashShopMng_Steam = NULL;

bool							nsCsFileTable::g_bUseEvent = true;
CsEventTable*					nsCsFileTable::g_pEventMng = NULL;

bool							nsCsFileTable::g_bAddExp = false;
CsAddExpMng*					nsCsFileTable::g_pAddExpMng = NULL;


CsLocalizingMng*				nsCsFileTable::g_pLocalizingMng = NULL;

bool							nsCsFileTable::g_bUseAchieve = false;
CsAchieveMng*					nsCsFileTable::g_pAchieveMng = NULL;

bool							nsCsFileTable::g_bSceneDataMng = false;
CsSceneDataMng*					nsCsFileTable::g_pSceneDataMng = NULL;

bool							nsCsFileTable::g_bBuffMng = true;
CsBuffMng*						nsCsFileTable::g_pBuffMng = NULL;

bool							nsCsFileTable::g_bUseGotcha = true;
CsGotchaMng*					nsCsFileTable::g_pGotchaMng = NULL;

bool							nsCsFileTable::g_bUseReward = true;
CsRewardMng*					nsCsFileTable::g_pRewardMng = NULL;

bool							nsCsFileTable::g_bUseMaster_CardMng = true;
CsMaster_CardMng*				nsCsFileTable::g_pMaster_CardMng = NULL;

extern bool						nsCsFileTable::g_bBattleTableMng = true;
extern CsBattleTableMng*		nsCsFileTable::g_pBattleTableMng = NULL;

extern bool						nsCsFileTable::g_bLoadServerTranferTBMng = true;
extern CsServerRelocateTBMng*	nsCsFileTable::g_pServerTranferTBMng = NULL;

extern bool						nsCsFileTable::g_bLoadCharCreateTBMng = true;
extern CsCharCreateTableMng*	nsCsFileTable::g_pCharCreateTBMng = NULL;

extern bool						nsCsFileTable::g_bUITextTBMng = true;
extern CsUITextMng*				nsCsFileTable::g_pUITextTBMng = NULL;

extern bool						nsCsFileTable::g_bInfiniteWarTBMng = true;
extern CsInfiniteWarMng*		nsCsFileTable::g_pInfiniteWarTBMng = NULL;

extern bool						nsCsFileTable::g_bDigimonParcelTBMng = true;
extern CsDigimonParcelMng*		nsCsFileTable::g_pDigimonParcelTBMng = NULL;

extern bool						nsCsFileTable::g_bExtraEvolutionTBMng = true;
extern CsExtraEvolutionMng*		nsCsFileTable::g_pExtraEvolutionTBMng = NULL;

extern bool						nsCsFileTable::g_bEffectTBMng = true;
extern CsEffectMng*				nsCsFileTable::g_pEffectTBMng = NULL;

extern bool						nsCsFileTable::g_bTimeChargeMng = true;
extern CsTimeChargeMng*			nsCsFileTable::g_pTimeChargeMng = NULL;

// 경로
char							nsCsFileTable::_g_cExcelPath[ MAX_PATH ] = {0, };

void nsCsFileTable::AllFalse()
{
	g_bUseQuest = false;
	g_bReadInactiveQuest = false;
	g_bUseDigimon = false;
	g_bUseMonsterDrop = false;
	g_bUseMonster = false;
	g_bUseNpc = false;
	g_bUseMap = false;
	g_bUseItem = false;
	g_bUseBase = false;
	g_bUseSkill = false;
	g_bUseNature = false;
	g_bUseMessage = false;
	g_bUseCuid = false;
	g_bUseEvoExtra = false;
	g_bUseEvoBattle = false;
	g_bUseHelp = false;
	g_bUseMoveObject = false;
	g_bUseTotalExcel_None = false;		// 사용되어지지 않아 졌다
	g_bUseCashShop = false;
	g_bUseEvent = false;
	g_bAddExp = false;
	g_bUseAchieve = false;
	g_bSceneDataMng = false;
	g_bBuffMng = false;
	g_bUseGotcha = false;
	g_bUseReward = false;
	g_bUseMaster_CardMng = false;
	g_bBattleTableMng = false;
	g_bLoadServerTranferTBMng = false;
	g_bLoadCharCreateTBMng = false;
	g_bUITextTBMng = false;
	g_bInfiniteWarTBMng = false;
	g_bDigimonParcelTBMng = false;
	g_bExtraEvolutionTBMng = false;
	g_bEffectTBMng = false;
	g_bTimeChargeMng = false;
}

#define __PATH_DM( kor, eng_A, eng, thai, taiwan, hongkong )	{\
	switch( nsCsFileTable::g_eLanguage ){\
	case nsCsFileTable::KOREA_ORIGINAL:\
	case nsCsFileTable::KOREA_TRANSLATION:\
	strcpy_s( _g_cExcelPath, MAX_FILENAME, kor );\
	break;\
	case nsCsFileTable::ENGLISH:\
	strcpy_s( _g_cExcelPath, MAX_FILENAME, eng );\
	break;\
	case nsCsFileTable::ENGLISH_A:\
	strcpy_s( _g_cExcelPath, MAX_FILENAME, eng_A );\
	break;\
	case nsCsFileTable::THAILAND:\
	strcpy_s( _g_cExcelPath, MAX_FILENAME, thai );\
	break;\
	case nsCsFileTable::TAIWAN:\
	strcpy_s( _g_cExcelPath, MAX_FILENAME, taiwan );\
	break;\
	case nsCsFileTable::HONGKONG:\
	strcpy_s( _g_cExcelPath, MAX_FILENAME, hongkong );\
	break;\
	default:\
	assert_cs( false );\
}\
	return _g_cExcelPath;\
}

#define __PATH_DM_1( path )	{\
	strcpy_s( _g_cExcelPath, MAX_FILENAME, path );\
	return _g_cExcelPath;\
}

char* nsCsFileTable::GetPath_DM_CUID(){				__PATH_DM( "GameData\\Korea\\CUID.xls",						"GameData\\English_A\\CUID_E.xls",					"GameData\\English\\CUID_E.xls",					"GameData\\Thailand\\CUID_T.xls",					"GameData\\Taiwan\\CUID_Tw.xls",					"GameData\\HongKong\\CUID_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Base(){				__PATH_DM( "GameData\\Korea\\DM_Base.xls",					"GameData\\English_A\\DM_Base_E.xls",				"GameData\\English\\DM_Base_E.xls",					"GameData\\Thailand\\DM_Base_T.xls",				"GameData\\Taiwan\\DM_Base_Tw.xls",					"GameData\\HongKong\\DM_Base_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Item(){				__PATH_DM( "GameData\\Korea\\DM_Item_List.xls",				"GameData\\English_A\\DM_Item_List_E.xls",			"GameData\\English\\DM_Item_List_E.xls",			"GameData\\Thailand\\DM_Item_List_T.xls",			"GameData\\Taiwan\\DM_Item_List_Tw.xls",			"GameData\\HongKong\\DM_Item_List_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_QuestItem(){		__PATH_DM( "GameData\\Korea\\DM_QuestItem_List.xls",		"GameData\\English_A\\DM_QuestItem_List_E.xls",		"GameData\\English\\DM_QuestItem_List_E.xls",		"GameData\\Thailand\\DM_QuestItem_List_T.xls",		"GameData\\Taiwan\\DM_QuestItem_List_Tw.xls",		"GameData\\HongKong\\DM_QuestItem_List_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Digimon(){			__PATH_DM( "GameData\\Korea\\DM_Digimon_List.xls",			"GameData\\English_A\\DM_Digimon_List_E.xls",		"GameData\\English\\DM_Digimon_List_E.xls",			"GameData\\Thailand\\DM_Digimon_List_T.xls",		"GameData\\Taiwan\\DM_Digimon_List_Tw.xls",			"GameData\\HongKong\\DM_Digimon_List_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_DigimonANI(){		__PATH_DM( "GameData\\Korea\\DM_DigimonANI.xls",			"GameData\\English_A\\DM_DigimonANI_E.xls",			"GameData\\English\\DM_DigimonANI_E.xls",			"GameData\\Thailand\\DM_DigimonANI_T.xls",			"GameData\\Taiwan\\DM_DigimonANI_Tw.xls",			"GameData\\HongKong\\DM_DigimonANI_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Ride(){				__PATH_DM( "GameData\\Korea\\DM_Riding.xls",				"GameData\\English_A\\DM_Riding_E.xls",				"GameData\\English\\DM_Riding_E.xls",				"GameData\\Thailand\\DM_Riding_T.xls",				"GameData\\Taiwan\\DM_Riding_Tw.xls",				"GameData\\HongKong\\DM_Riding_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_ElementUI(){		__PATH_DM( "GameData\\Korea\\DM_Element_UI.xls",			"GameData\\English_A\\DM_Element_UI_E.xls",			"GameData\\English\\DM_Element_UI_E.xls",			"GameData\\Thailand\\DM_Element_UI_T.xls",			"GameData\\Taiwan\\DM_Element_UI_Tw.xls",			"GameData\\HongKong\\DM_Element_UI_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Tamer(){			__PATH_DM( "GameData\\Korea\\DM_Tamer_List.xls",			"GameData\\English_A\\DM_Tamer_List_E.xls",			"GameData\\English\\DM_Tamer_List_E.xls",			"GameData\\Thailand\\DM_Tamer_List_T.xls",			"GameData\\Taiwan\\DM_Tamer_List_Tw.xls",			"GameData\\HongKong\\DM_Tamer_List_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Limit(){			__PATH_DM( "GameData\\Korea\\DM_Limit.xls",					"GameData\\English_A\\DM_Limit_E.xls",				"GameData\\English\\DM_Limit_E.xls",				"GameData\\Thailand\\DM_Limit_T.xls",				"GameData\\Taiwan\\DM_Limit_Tw.xls",				"GameData\\HongKong\\DM_Limit_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_CashShop(){			__PATH_DM( "GameData\\Korea\\DM_CashShop.xlsm",				"GameData\\English_A\\DM_CashShop_E.xlsm",			"GameData\\English\\DM_CashShop_E.xlsm",				"GameData\\Thailand\\DM_CashShop_T.xlsm",			"GameData\\Taiwan\\DM_CashShop_Tw.xlsm",		"GameData\\HongKong\\DM_CashShop_HK.xlsm" ); }
char* nsCsFileTable::GetPath_DM_CashShop_Steam(){	__PATH_DM_1( "GameData\\English\\\\DM_CashShop_E_Steam.xlsm" ); }
char* nsCsFileTable::GetPath_DM_Evolution_01(){		__PATH_DM( "GameData\\Korea\\DM_Evolution_01.xls",			"GameData\\English_A\\DM_Evolution_01_E.xls",		"GameData\\English\\DM_Evolution_01_E.xls",			"GameData\\Thailand\\DM_Evolution_01_T.xls",		"GameData\\Taiwan\\DM_Evolution_01_Tw.xls",			"GameData\\HongKong\\DM_Evolution_01_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Evolution_02(){		__PATH_DM( "GameData\\Korea\\DM_Evolution_02.xls",			"GameData\\English_A\\DM_Evolution_02_E.xls",		"GameData\\English\\DM_Evolution_02_E.xls",			"GameData\\Thailand\\DM_Evolution_02_T.xls",		"GameData\\Taiwan\\DM_Evolution_02_Tw.xls",			"GameData\\HongKong\\DM_Evolution_02_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Battle_Evo(){		__PATH_DM( "GameData\\Korea\\DM_Battle_Evolution.xls",		"GameData\\English_A\\DM_Battle_Evolution_E.xls",	"GameData\\English\\DM_Battle_Evolution_E.xls",		"GameData\\Thailand\\DM_Battle_Evolution_T.xls",	"GameData\\Taiwan\\DM_Battle_Evolution_Tw.xls",		"GameData\\HongKong\\DM_Battle_Evolution_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_EvoDigitama(){		__PATH_DM( "GameData\\Korea\\DM_Evolution_Digitama.xls",	"GameData\\English_A\\DM_Evolution_Digitama_E.xls",	"GameData\\English\\DM_Evolution_Digitama_E.xls",	"GameData\\Thailand\\DM_Evolution_Digitama_T.xls",	"GameData\\Taiwan\\DM_Evolution_Digitama_Tw.xls",	"GameData\\HongKong\\DM_Evolution_Digitama_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Event(){			__PATH_DM( "GameData\\Korea\\DM_Event.xls",					"GameData\\English_A\\DM_Event_E.xls",				"GameData\\English\\DM_Event_E.xls",				"GameData\\Thailand\\DM_Event_T.xls",				"GameData\\Taiwan\\DM_Event_Tw.xls",				"GameData\\HongKong\\DM_Event_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_AddEffect(){		__PATH_DM( "GameData\\Korea\\DM_Addeffect.xls",				"GameData\\English_A\\DM_Addeffect_E.xls",			"GameData\\English\\DM_Addeffect_E.xls",			"GameData\\Thailand\\DM_Addeffect_T.xls",			"GameData\\Taiwan\\DM_Addeffect_Tw.xls",			"GameData\\HongKong\\DM_Addeffect_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Monster(){			__PATH_DM( "GameData\\Korea\\DM_Monster_List.xls",			"GameData\\English_A\\DM_Monster_List_E.xls",		"GameData\\English\\DM_Monster_List_E.xls",			"GameData\\Thailand\\DM_Monster_List_T.xls",		"GameData\\Taiwan\\DM_Monster_List_Tw.xls",			"GameData\\HongKong\\DM_Monster_List_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Npc(){				__PATH_DM( "GameData\\Korea\\Npc.xls",						"GameData\\English_A\\Npc_E.xls",					"GameData\\English\\Npc_E.xls",						"GameData\\Thailand\\Npc_T.xls",					"GameData\\Taiwan\\Npc_Tw.xls",						"GameData\\HongKong\\Npc_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Quest(){			__PATH_DM( "GameData\\Korea\\DM_Quest.xls",					"GameData\\English_A\\DM_Quest_E.xls",				"GameData\\English\\DM_Quest_E.xls",				"GameData\\Thailand\\DM_Quest_T.xls",				"GameData\\Taiwan\\DM_Quest_Tw.xls",				"GameData\\HongKong\\DM_Quest_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_WorldMap(){			__PATH_DM( "GameData\\Korea\\DM_WorldMap.xls",				"GameData\\English_A\\DM_WorldMap_E.xls",			"GameData\\English\\DM_WorldMap_E.xls",				"GameData\\Thailand\\DM_WorldMap_T.xls",			"GameData\\Taiwan\\DM_WorldMap_Tw.xls",				"GameData\\HongKong\\DM_WorldMap_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Talk(){				__PATH_DM( "GameData\\Korea\\DM_Talk.xls",					"GameData\\English_A\\DM_Talk_E.xls",				"GameData\\English\\DM_Talk_E.xls",					"GameData\\Thailand\\DM_Talk_T.xls",				"GameData\\Taiwan\\DM_Talk_Tw.xls",					"GameData\\HongKong\\DM_Talk_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Parcing(){			__PATH_DM( "GameData\\Korea\\DM_ParcingWord.xls",			"GameData\\English_A\\DM_ParcingWord_E.xls",		"GameData\\English\\DM_ParcingWord_E.xls",			"GameData\\Thailand\\DM_ParcingWord_T.xls",			"GameData\\Taiwan\\DM_ParcingWord_Tw.xls",			"GameData\\HongKong\\DM_ParcingWord_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Achieve(){			__PATH_DM( "GameData\\Korea\\DM_Achieve.xls",				"GameData\\English_A\\DM_Achieve_E.xls",			"GameData\\English\\DM_Achieve_E.xls",				"GameData\\Thailand\\DM_Achieve_T.xls",				"GameData\\Taiwan\\DM_Achieve_Tw.xls",				"GameData\\HongKong\\DM_Achieve_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Scene(){			__PATH_DM( "GameData\\Korea\\DM_Scene.xls",		    		"GameData\\English_A\\DM_Scene_E.xls",				"GameData\\English\\DM_Scene_E.xls",				"GameData\\Thailand\\DM_Scene_T.xls",				"GameData\\Taiwan\\DM_Scene_Tw.xls",				"GameData\\HongKong\\DM_Scene_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Buff(){				__PATH_DM( "GameData\\Korea\\DM_Buff.xls",					"GameData\\English_A\\DM_Buff_E.xls",				"GameData\\English\\DM_Buff_E.xls",					"GameData\\Thailand\\DM_Buff_T.xls",				"GameData\\Taiwan\\DM_Buff_Tw.xls",					"GameData\\HongKong\\DM_Buff_HK.xls" ); }
char* nsCsFileTable::GetPath_OT_Help(){				__PATH_DM( "Data\\Interface\\Help\\Help.bin",				"Data\\Interface\\Help\\HelpEng.bin",				"Data\\Interface\\Help\\HelpEng.bin",				"Data\\Interface\\Help\\HelpEng.bin",				"Data\\Interface\\Help\\HelpEng.bin",				"Data\\Interface\\Help\\HelpEng.bin" ); }
char* nsCsFileTable::GetPath_DM_Localizing(){		__PATH_DM_1( "GameData\\_Etc\\Localizing.xls" ); }
char* nsCsFileTable::GetPath_DM_Total(){			__PATH_DM_1( "GameData\\Thailand\\Total.xls" ); }
char* nsCsFileTable::GetPath_MAP_CharLight(){		__PATH_DM( "GameData\\Korea\\MapCharLight.xls",				"GameData\\English_A\\MapCharLight_E.xls",			"GameData\\English\\MapCharLight_E.xls",			"GameData\\Thailand\\MapCharLight_T.xls",			"GameData\\Taiwan\\MapCharLight_Tw.xls",			"GameData\\HongKong\\MapCharLight_HK.xls" ); }
char* nsCsFileTable::GetPath_MAP_List(){			__PATH_DM( "GameData\\Korea\\MapList.xls",					"GameData\\English_A\\MapList_E.xls",				"GameData\\English\\MapList_E.xls",					"GameData\\Thailand\\MapList_T.xls",				"GameData\\Taiwan\\MapList_Tw.xls",					"GameData\\HongKong\\MapList_HK.xls" ); }
char* nsCsFileTable::GetPath_MAP(){					__PATH_DM( "GameData\\Korea\\Map.xls",						"GameData\\English_A\\Map_E.xls",					"GameData\\English\\Map_E.xls",						"GameData\\Thailand\\Map_T.xls",					"GameData\\Taiwan\\Map_Tw.xls",						"GameData\\HongKong\\Map_HK.xls" ); }
char* nsCsFileTable::GetPath_MAP_Portal(){			__PATH_DM( "GameData\\Korea\\MapPortal.xls",				"GameData\\English_A\\MapPortal_E.xls",				"GameData\\English\\MapPortal_E.xls",				"GameData\\Thailand\\MapPortal_T.xls",				"GameData\\Taiwan\\MapPortal_Tw.xls",				"GameData\\HongKong\\MapPortal_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_MoveObject(){		__PATH_DM( "GameData\\Korea\\DM_MoveObject.xls",			"GameData\\English_A\\DM_MoveObject_E.xls",			"GameData\\English\\DM_MoveObject_E.xls",			"GameData\\Thailand\\DM_MoveObject_T.xls",			"GameData\\Taiwan\\DM_MoveObject_Tw.xls",			"GameData\\HongKong\\DM_MoveObject_HK.xls" ); }
char* nsCsFileTable::GetPath_Tamer_Skill(){			__PATH_DM( "GameData\\Korea\\Tamer_Skill.xls",				"GameData\\English_A\\Tamer_Skill_E.xls",			"GameData\\English\\Tamer_Skill_E.xls" ,			"GameData\\Thailand\\Tamer_Skill_T.xls",			"GameData\\Taiwan\\Tamer_Skill_Tw.xls",				"GameData\\HongKong\\Tamer_Skill_HK.xls" ); }
char* nsCsFileTable::GetPath_Gotcha(){				__PATH_DM("GameData\\Korea\\Gotcha.xls",					"GameData\\English_A\\Gotcha_E.xls",				"GameData\\English\\Gotcha_E.xls" ,					"GameData\\Thailand\\Gotcha_T.xls",					"GameData\\Taiwan\\Gotcha_Tw.xls",					"GameData\\HongKong\\Gotcha_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Reward(){			__PATH_DM( "GameData\\Korea\\Recompense.xls",				"GameData\\English_A\\Recompense_E.xls",			"GameData\\English\\Recompense_E.xls",				"GameData\\Thailand\\Recompense_T.xls", 			"GameData\\Taiwan\\Recompense_Tw.xls",  			"GameData\\HongKong\\Recompense_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Moster_Skill(){		__PATH_DM( "GameData\\Korea\\Monster_Skill.xls",			"GameData\\English_A\\Monster_Skill_E.xls",			"GameData\\English\\Monster_Skill_E.xls",			"GameData\\Thailand\\Monster_Skill_T.xls", 			"GameData\\Taiwan\\Monster_Skill_Tw.xls",  			"GameData\\HongKong\\Monster_Skill_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Monster_Skill_terms(){	__PATH_DM( "GameData\\Korea\\Monster_Skill_terms.xls",	"GameData\\English_A\\Monster_Skill_terms_E.xls",	"GameData\\English\\Monster_Skill_terms_E.xls",		"GameData\\Thailand\\Monster_Skill_terms_T.xls", 	"GameData\\Taiwan\\Monster_Skill_terms_Tw.xls",  	"GameData\\HongKong\\Monster_Skill_terms_HK.xls" ); }
char* nsCsFileTable::GetPath_Master_Card(){			__PATH_DM( "GameData\\Korea\\SealMaster.xls",				"GameData\\English_A\\SealMaster_E.xls",			"GameData\\English\\SealMaster_E.xls",				"GameData\\Thailand\\SealMaster_T.xls", 			"GameData\\Taiwan\\SealMaster_Tw.xls",  			"GameData\\HongKong\\SealMaster_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Digimon_Book(){		__PATH_DM( "GameData\\Korea\\DM_Digimon_Book.xls",			"GameData\\English_A\\DM_Digimon_Book_E.xls",		"GameData\\English\\DM_Digimon_Book_E.xls",			"GameData\\Thailand\\DM_Digimon_Book_T.xls", 		"GameData\\Taiwan\\DM_Digimon_Book_Tw.xls",  		"GameData\\HongKong\\DM_Digimon_Book_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Tutorial(){			__PATH_DM( "GameData\\Korea\\DM_Tutorial.xls",				"GameData\\English_A\\DM_Tutorial_E.xls",			"GameData\\English\\DM_Tutorial_E.xls",				"GameData\\Thailand\\DM_Tutorial_T.xls", 			"GameData\\Taiwan\\DM_Tutorial_Tw.xls",  			"GameData\\HongKong\\DM_Tutorial_HK.xls" ); }
char* nsCsFileTable::GetPath_Battle(){				__PATH_DM( "GameData\\Korea\\DM_Battle.xls",				"GameData\\English_A\\DM_Battle_E.xls",				"GameData\\English\\DM_Battle_E.xls",				"GameData\\Thailand\\DM_Battle_T.xls", 				"GameData\\Taiwan\\DM_Battle_Tw.xls",  				"GameData\\HongKong\\DM_Battle_hk.xls" ); }
char* nsCsFileTable::GetPath_ServerTransfer(){		__PATH_DM( "GameData\\Korea\\DM_Server_Transfer.xls",		"GameData\\English_A\\DM_Server_Transfer_E.xls",	"GameData\\English\\DM_Server_Transfer_E.xls",		"GameData\\Thailand\\DM_Server_Transfer_T.xls", 	"GameData\\Taiwan\\DM_Server_Transfer_Tw.xls",  	"GameData\\HongKong\\DM_Server_Transfer_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_CharCreate(){		__PATH_DM( "GameData\\Korea\\DM_CreatePlayerList.xls",		"GameData\\English_A\\DM_CreatePlayerList_E.xls",	"GameData\\English\\DM_CreatePlayerList_E.xls",		"GameData\\Thailand\\DM_CreatePlayerList_T.xls", 	"GameData\\Taiwan\\DM_CreatePlayerList_Tw.xls",  	"GameData\\HongKong\\DM_CreatePlayerList_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_UIText(){			__PATH_DM( "GameData\\Korea\\DM_UIText.xls",				"GameData\\English_A\\DM_UIText_E.xls",				"GameData\\English\\DM_UIText_E.xls",				"GameData\\Thailand\\DM_UIText_T.xls", 				"GameData\\Taiwan\\DM_UIText_Tw.xls",  				"GameData\\HongKong\\DM_UIText_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_InfiniteWar(){		__PATH_DM( "GameData\\Korea\\DM_Infinite_Match.xls",		"GameData\\English_A\\DM_Infinite_Match_E.xls",		"GameData\\English\\DM_Infinite_Match_E.xls",		"GameData\\Thailand\\DM_Infinite_Match_T.xls", 		"GameData\\Taiwan\\DM_Infinite_Match_Tw.xls",  		"GameData\\HongKong\\DM_Infinite_Match_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Parcel(){			__PATH_DM( "GameData\\Korea\\DM_Parcel_Out.xls",			"GameData\\English_A\\DM_Parcel_Out_E.xls",			"GameData\\English\\DM_Parcel_Out_E.xls",			"GameData\\Thailand\\DM_Parcel_Out_T.xls", 			"GameData\\Taiwan\\DM_Parcel_Out_Tw.xls",  			"GameData\\HongKong\\DM_Parcel_Out_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_ExtraEvolution() {	__PATH_DM( "GameData\\Korea\\DM_ExtraEvolution.xls",		"GameData\\English_A\\DM_ExtraEvolution_E.xls",		"GameData\\English\\DM_ExtraEvolution_E.xls",		"GameData\\Thailand\\DM_ExtraEvolution_T.xls", 		"GameData\\Taiwan\\DM_ExtraEvolution_Tw.xls",  		"GameData\\HongKong\\DM_ExtraEvolution_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_EffectMng(){		__PATH_DM( "GameData\\Korea\\DM_EffectList.xls",			"GameData\\English_A\\DM_EffectList_E.xls",			"GameData\\English\\DM_EffectList_E.xls",			"GameData\\Thailand\\DM_EffectList_T.xls", 			"GameData\\Taiwan\\DM_EffectList_Tw.xls",  			"GameData\\HongKong\\DM_EffectList_HK.xls" ); }
char* nsCsFileTable::GetPath_DM_Item_Charge(){		__PATH_DM( "GameData\\Korea\\DM_Item_Charge.xls",			"GameData\\English_A\\DM_Item_Charge_E.xls",		"GameData\\English\\DM_Item_Charge_E.xls",			"GameData\\Thailand\\DM_Item_Charge_T.xls",			"GameData\\Taiwan\\DM_Item_Charge_Tw.xls",			"GameData\\HongKong\\DM_Item_Charge_HK.xls" ); }