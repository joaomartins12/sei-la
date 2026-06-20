
#pragma once 

#define FT_RESOURCE_HANDLE	0
#define FT_PACKHANDLE		1
#define STRING_CHANGE_LEN	64

class CsFileTableMng;
class CsQuestMng;
class CsDigimonMng;
class CsTamerMng;
class CsMonsterMng;
class CsNpcMng;
class CsMapMng;
class CsItemMng;
class CsBaseMng;
class CsSkillMng;
class CsNatureMng;
class CsMessageMng;
class CsCuidMng;
class CDigimonEvolution;
class CsTalkMng;
class CsTacticsMng;
class CsWorldMapMng;
class CsLocalizingMng;
class CsHelpMng;
class CsMoveObjectMng;
class CsTotalExcelMng;
class CsCashShopMng;
//class CsCashShopMng_Steam;
class CsEventTable;
class CsAddExpMng;
class CsAchieveMng;
class CsSceneDataMng;
class CsBuffMng;
class CsRewardMng;
//20120705:최지현:GSP 서버정렬
class CsServerLineUpTable;
//END:20120705:최지현:GSP 서버정렬
class CsGotchaMng;
class CsMaster_CardMng;
class CsBattleTableMng;
class CsServerRelocateTBMng;
class CsCharCreateTableMng;
class CsUITextMng;
class CsInfiniteWarMng;
class CsDigimonParcelMng;
class CsExtraEvolutionMng;
class CsEffectMng;
class CsTimeChargeMng;

namespace nsCsFileTable
{
	enum eFILE_TYPE{ FT_EXCEL = 0, FT_CLONE, FT_BIN, FT_FILEPACK, };
	enum eLANGUAGE{ KOREA_TRANSLATION = 100, KOREA_ORIGINAL, ENGLISH, THAILAND, ENGLISH_A, TAIWAN, HONGKONG};
	extern eFILE_TYPE				g_eFileType;
	extern eLANGUAGE				g_eLanguage;

	extern CsFileTableMng			g_FileTableMng;

	extern bool						g_bUseQuest;
	extern bool						g_bReadInactiveQuest;
	extern CsQuestMng*				g_pQuestMng;

	extern bool						g_bUseDigimon;
	extern CsDigimonMng*			g_pDigimonMng;

	extern bool						g_bUseMonsterDrop;
	extern bool						g_bUseMonster;
	extern CsMonsterMng*			g_pMonsterMng;

	extern bool						g_bUseNpc;
	extern CsNpcMng*				g_pNpcMng;

	extern bool						g_bUseMap;
	extern CsMapMng*				g_pMapMng;

	extern bool						g_bUseItem;
	extern CsItemMng*				g_pItemMng;

	extern bool						g_bUseBase;
	extern CsBaseMng*				g_pBaseMng;

	extern bool						g_bUseSkill;
	extern CsSkillMng*				g_pSkillMng;

	extern bool						g_bUseNature;
	extern CsNatureMng*				g_pNatureMng;

	extern bool						g_bUseMessage;
	extern CsMessageMng*			g_pMessageMng;

	extern bool						g_bUseCuid;
	extern CsCuidMng*				g_pCuidMng;

	extern bool						g_bUseEvoExtra;
	extern bool						g_bUseEvoBattle;
	extern CDigimonEvolution*		g_pEvolMng;

	extern CsTalkMng*				g_pTalkMng;

	extern CsTacticsMng*			g_pTacticsMng;

	extern CsWorldMapMng*			g_pWorldMapMng;

	extern CsTamerMng*				g_pTamerMng;

	extern CsLocalizingMng*			g_pLocalizingMng;

	extern bool						g_bUseHelp;
	extern CsHelpMng*				g_pHelpMng;

	extern bool						g_bUseMoveObject;
	extern CsMoveObjectMng*			g_pMoveObjectMng;

	extern bool						g_bUseTotalExcel_None;		// 사용되어지지 않아 졌다
	extern CsTotalExcelMng*			g_pTotalExcelMng;

	extern bool						g_bUseCashShop;
	extern CsCashShopMng*			g_pCashShopMng;

	extern bool						g_bUseEvent;
	extern CsEventTable*			g_pEventMng;

	extern bool						g_bAddExp;
	extern CsAddExpMng*				g_pAddExpMng;

	extern bool						g_bUseAchieve;
	extern CsAchieveMng*			g_pAchieveMng;

	extern bool						g_bSceneDataMng;
	extern CsSceneDataMng*			g_pSceneDataMng;

	extern bool						g_bBuffMng;
	extern CsBuffMng*				g_pBuffMng;

 	extern bool						g_bUseGotcha;
 	extern CsGotchaMng*				g_pGotchaMng;

	extern bool						g_bUseReward;
	extern CsRewardMng*				g_pRewardMng;

	extern bool						g_bUseMaster_CardMng;
	extern CsMaster_CardMng*		g_pMaster_CardMng;

	extern bool						g_bBattleTableMng;
	extern CsBattleTableMng*		g_pBattleTableMng;

	extern bool						g_bLoadServerTranferTBMng;
	extern CsServerRelocateTBMng*	g_pServerTranferTBMng;

	extern bool						g_bLoadCharCreateTBMng;
	extern CsCharCreateTableMng*	g_pCharCreateTBMng;

	extern bool						g_bUITextTBMng;
	extern CsUITextMng*				g_pUITextTBMng;

	extern bool						g_bInfiniteWarTBMng;
	extern CsInfiniteWarMng*		g_pInfiniteWarTBMng;

	extern bool						g_bDigimonParcelTBMng;
	extern CsDigimonParcelMng*		g_pDigimonParcelTBMng;

	extern bool						g_bExtraEvolutionTBMng;
	extern CsExtraEvolutionMng*		g_pExtraEvolutionTBMng;

	extern bool						g_bEffectTBMng;
	extern CsEffectMng*				g_pEffectTBMng;

	extern bool						g_bTimeChargeMng;
	extern CsTimeChargeMng*			g_pTimeChargeMng;

	// 경로
	char*		GetPath_DM_CUID();
	char*		GetPath_DM_Base();
	char*		GetPath_DM_Item();
	char*		GetPath_DM_QuestItem();
	char*		GetPath_DM_Digimon();
	char*		GetPath_DM_DigimonANI();
	char*		GetPath_DM_Ride();
	char*		GetPath_DM_ElementUI();
	char*		GetPath_DM_Tamer();
	char*		GetPath_DM_Limit();
	char*		GetPath_DM_CashShop();
	char*		GetPath_DM_CashShop_Steam();

	char*		GetPath_DM_Evolution_01();
	char*		GetPath_DM_Evolution_02();
	char*		GetPath_DM_Battle_Evo();
	char*		GetPath_DM_EvoDigitama();
	char*		GetPath_DM_Event();
	char*		GetPath_DM_AddEffect();
	char*		GetPath_DM_Monster();
	char*		GetPath_DM_Npc();
	char*		GetPath_DM_Quest();
	char*		GetPath_DM_WorldMap();
	char*		GetPath_DM_Parcing();
	char*		GetPath_DM_Talk();
	char*		GetPath_DM_Achieve();
	char*		GetPath_DM_Scene();
	char*		GetPath_DM_Buff();

	char*		GetPath_OT_Help();

	char*		GetPath_DM_Localizing();	
	char*		GetPath_DM_Total();

	char*		GetPath_MAP_CharLight();
	char*		GetPath_MAP_List();
	char*		GetPath_MAP();
	char*		GetPath_MAP_Portal();
	char*		GetPath_DM_MoveObject();

	char*		GetPath_Tamer_Skill();
	
	char*		GetPath_Gotcha();

	char*		GetPath_DM_Reward();
	char*		GetPath_Master_Card();

	char*		GetPath_DM_Moster_Skill();
	char*		GetPath_DM_Monster_Skill_terms();

	char*		GetPath_DM_Digimon_Book();
	
	char*		GetPath_DM_Tutorial();


	char*		GetPath_Battle();

	char*		GetPath_ServerTransfer();
	char*		GetPath_DM_CharCreate();
	char*		GetPath_DM_UIText();
	char*		GetPath_DM_InfiniteWar();
	char*		GetPath_DM_Parcel();
	char*		GetPath_DM_ExtraEvolution();
	char*		GetPath_DM_EffectMng();

	char*		GetPath_DM_Item_Charge();
	
	extern char				_g_cExcelPath[ MAX_PATH ];

	void		AllFalse();
}


#define UISTRING				nsCsFileTable::g_pUITextTBMng
#define UISTRING_TEXT(value)	nsCsFileTable::g_pUITextTBMng->GetUIText(value)
#define UISTRING_VA(value,...)	nsCsFileTable::g_pUITextTBMng->GetUIText_VA( value, __VA_ARGS__ )
