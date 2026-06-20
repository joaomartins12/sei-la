
#pragma once 

#define POPUP_STRINGLIST_DELTA_Y		5
#define POPUP_MENU_COLOR				FONT_GOLD
#define POPUP_MENU_SIZE					CFont::FS_12

class cPopUpWindow : public NiMemObject
{
public:	
	enum eTYPE{ NONE, TACTICS, DIGIVICE_TACTICS, CHANNEL_LIST, OTHER_TAMER, OTHER_DIGIMON, PARTY_MENU, GUILD_MENU, EMOTION, FRIEND_LIST, SHUTOUT_LIST,
				MAINBAR_MISSION, MAINBAR_COMMUNITY, MAINBAR_STORE, MAINBAR_INVENTORY, MAINBAR_DATA, MAINBAR_CHARACTER,
				EXP_EVENT,
				ACC_SELECT_OPT,
				SEAL_MENU,
				VIP_SYSTEM_MENU,
				ITEM_MAKE_ASSIST_ITEM_REG,
	};

	enum eTACTIC_TYPE{ TACTICS_NONE, TACTICS_SUMMON, TACTICS_FIRE, };
	enum eOTHER_TAMER{ OTAMER_NONE, OTAMER_TRADE, OTAMER_WHISPER, OTAMER_PARTY, OTAMER_FRIEND, OTAMER_BLOCK, OTAMER_RANK,
						OTAMER_GUILD, OTAMER_MACRO };
	
	enum eOTHER_DIGIMON{ ODIGIMON_NONE, ODIGIMON_TARGET_TAMER };

	enum ePARTY_TYPE{ PARTY_NR_MY, PARTY_NR_OTHER, PARTY_HS_MY, PARTY_HS_OTHER, };
	enum ePARTY_MENU{ PARTY_MENU_NONE, PARTY_MENU_LEAVE, PARTY_MENU_SETHOST, PARTY_MENU_WHISPER, PARTY_MENU_KICK,
					  PARTY_MENU_ITEM_LOOT, PARTY_MENU_ITEM_RATE };	
	enum eGUILD_MENU{ GUILD_NONE, GUILD_AP_MASTER, GUILD_AP_2MASTER, GUILD_AP_DAT, GUILD_AP_NORMAL, GUILD_AP_NEWBI, // 여기까지 길마만 가능한 기능
					GUILD_BAN, GUILD_NONE2, GUILD_PARTY, GUILD_WHISPER };
	enum eFRIEND_MENU{ FRIEND_WHISPER, FRIEND_PARTY, FRIEND_DEL, FRIEND_MEMO };
	enum eSHUTOUT_MENU{ SHUTOUT_DEL, SHUTOUT_MEMO };
	enum eSEAL_MENU{ SEAL_NONE, SEAL_SETLEADER, SEAL_RESEAL, SEAL_CARDSWAP, SEAL_DELETE, SEAL_MAX,};	// 씰 창에서의 우클릭 메뉴

public:
	cPopUpWindow();
	virtual ~cPopUpWindow(){ Delete(); }


protected:
	cPopUp*			m_pPopupWindow;
	cSprite*		m_pLine;

	cSprite*		m_pSelMask;
	bool			m_bRenderSelMask;
	CsPoint			m_ptSelMask;
	CsPoint			m_ptUpdateMouse;
	CsPoint			m_ptMaxSize;
	CsPoint			m_ptPatchSize;

	cStringList		m_StringList;
	eTYPE			m_eType;
	
	CsPoint			m_ptRenderPos;
	bool			m_bRenderPopup;

	int				m_nSubID1;
	int				m_nSubID2;

public:
	void			Delete();
	void			Init();
	bool			Render();
	void			ResetDevice();
	void			PreResetMap();

	void			SetPopup( CsPoint pos, CsPoint patch, eTYPE type, int nSub1 = 0, int nSub2 = 0 );	

	bool			Update_ForMouse();

	bool			OnEscapeKey(){ if( m_bRenderPopup == true ){ m_bRenderPopup = false; return true; } return false; }
	bool			IsRenderPopup(){ return m_bRenderPopup; }

	void			ClosePopup();
	void			ClosePopup( eTYPE type );

protected:	
	void			_AddLine();

protected:
	bool			_Update_ForMouse_Tactics();
	void			_Make_Tactics();

protected:
	void			_Make_DigiviceTactics();	


protected:
	cImage*			m_pChannelImage1;
	cImage*			m_pChannelImage2;
protected:
	bool			_Update_ForMouse_ChannelList();
	void			_Make_ChannelList();

protected:
	void			_Make_OtherTamer();
	bool			_Update_ForMouse_OtherTamer();

protected:
	void			_Make_OtherDigimon();
	bool			_Update_ForMouse_OtherDigimon();	

protected:
	void			_Make_PartyMenu();
	bool			_Update_ForMouse_PartyMenu();

protected:
	void			_Make_GuildMenu();
	bool			_Update_ForMouse_GuildMenu();

protected:
	void			_Make_Emotion();
	bool			_Update_ForMouse_Emotion();

protected:
	void			_Make_Friend();
	bool			_Update_ForMouse_Friend();

protected:
	void			_Make_ShutOut();
	bool			_Update_ForMouse_ShutOut();

protected:
	void			_Make_SealPopMenu();
	bool			_Update_ForMouse_SealPopMenu();

	void			_Make_ExpEventTooltip();

protected:
#define		MAINBAR_LIST	16
	//enum			eMAINBAR { MAINBAR_ACHIVE, MAINBAR_QUEST, MAINBAR_HELP, MAINBAR_GUILD, MAINBAR_INGAME, MAINBAR_CASH, MAINBAR_GIFT, CASHSHOP, MAINBAR_BATTLE };
	enum			eMAINBAR { MAINBAR_CASHWAREHOUSE,
								MAINBAR_GUILD, MAINBAR_FRIEND, MAINBAR_MAIL,
								MAINBAR_INV, MAINBAR_EXCLUSIVEINV,
								MAINBAR_SEAL, MAINBAR_ENCYCLOPEDIA, MAINBAR_EVOLUTIONHELPS, MAINBAR_BATTLE,
								MAINBAR_POTION, MAINBAR_TAMER, MAINBAR_DIGIMON, MAINBAR_ACHIEVEMENT, MAINBAR_QUEST, MAINBAR_EXCHANGE };

	cSprite*		m_pSprMainBar[ MAINBAR_LIST ];
	//cSprite*		m_pBar_CashWarehouse;
	//cSprite*		m_pBar_Community;
	//cSprite*		m_pBar_Friend;
	//cSprite*		m_pBar_Mail;
	//cSprite*		m_pBar_Inventory;
	//cSprite*		m_pBar_ExtraInventory;
	//cSprite*		m_pBar_SealMaster;
	//cSprite*		m_pBar_Encyclopedia;
	//cSprite*		m_pBar_EvolutionHelps;
	//cSprite*		m_pBar_BattleStatistics;
	//cSprite*		m_pBar_AutoPotion;
	//cSprite*		m_pBar_TamerInfo;
	//cSprite*		m_pBar_DigimonInfo;
	//cSprite*		m_pBar_Achievement;
	//cSprite*		m_pBar_Quest;
	//cSprite*		m_pBar_Exchange;

protected:
	void			_Make_Mainbar_Mission();	
	bool			_Update_ForMouse_Mainbar_Mission();

	void			_Make_Mainbar_Store();
	bool			_Update_ForMouse_Mainbar_Store();

	void			_Make_Mainbar_Community();	
	bool			_Update_ForMouse_Mainbar_Community();

	void			_Make_Mainbar_Inventory();
	bool			_Update_ForMouse_Mainbar_Inventory();

	void			_Make_Mainbar_Data();
	bool			_Update_ForMouse_Mainbar_Data();

	void			_Make_Mainbar_Character();
	bool			_Update_ForMouse_Mainbar_Character();

	void			_Make_VipSystemPopMenu();
	bool			_Update_ForMouse_VipSystemPopMenu();

	void			_Make_ItemMake_AssistItem_PopMenu();
	bool			_Update_ForMouse_AssistItem_PopMenu();
};