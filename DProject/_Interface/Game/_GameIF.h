
#pragma once 

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "MapContents.h"

#include "_BaseWindow.h"
#include "DigimonTalk.h"
#include "MoneyIF.h"
#include "MessageBox.h"
#include "SystemMsg.h"
#include "PrintMsg.h"
#include "TamerWindow.h"
#include "MainBar.h"
#include "MainDigimonWindow.h"
#include "QuickSlot.h"
#include "QuickEvol.h"
#include "ChatOption.h"
#include "Inventory.h"
#include "Talk.h"
#include "Store.h"
#include "Digitama_Sell.h"
#include "Digitama_Scan.h"
#include "ToolTip.h"
#include "ToolTip_RightBottom.h"
#include "Separate.h"
#include "ItemSeparate.h"
#include "CreateName.h"
#include "QuestRev.h"
#include "QuestComp.h"
#include "QuestList.h"
#include "QuestDiscript.h"
#include "QuestHelper.h"
#include "RegionInfo.h"
#include "AchieveInfo.h"
#include "MiniMap.h"
#include "TacticsWindow.h"
#include "MakeTactics.h"
#include "TacticsAni.h"
#include "Popup.h"
#include "ZoneMap.h"
#include "MainOption.h"
#include "OptionBase.h"
#include "OptionSound.h"
#include "OptionMacro.h"
#include "OptionGraphic.h"
#include "OptionInterface.h"
#include "OptionUserGrade.h"
#include "TalkBalloon.h"
#include "HpBar.h"
#include "Tip.h"
#include "Fade.h"
//#include "Loading.h"
#include "Publicity.h"
#include "Channel.h"
#include "DigiviceQuest.h"
#include "ElementItem_Check.h"
#include "ElementItem_Attach.h"
#include "Warehouse.h"
#include "Weather.h"
#include "Trade.h"
#include "NpcFind.h"
#include "MoneySeparate.h"
#include "Community.h"
//#include "Cash.h"
#include "DigimonArchiveViewer.h"
#include "BGSprite.h"
#include "JumpBuster.h"
#include "PartyWindow.h"
#include "BuffInfo.h"
#include "HelpArrow.h"
#include "UserGrade.h"
#include "SmallMap.h"
//#include "SecondPassword.h"
#include "WebWin.h"
#include "Digimon_Boss.h"
#include "ChangeTamer.h"
#include "GuildSet.h"
#include "RankView.h"
#include "Mail.h"
#include "Letter.h"
#include "Mail_Button.h"
#include "Alime.h"
//#include "CashShop.h"
#include "CashShopButton.h"
#include "DigimonTalk.h"
#include "Protect.h"
#include "Achieve.h"
#include "Division.h"
#include "BaseLootingWindow.h"
#include "Looting.h"
#include "MyLooting.h"
#include "UnionStore.h"
#include "PersonStore.h"
#include "PcbangInven.h"
#include "Shout.h"
#include "TacticsEnchant.h"
#include "PlayTime.h"


#include "Centerbar.h"
#include "ItemProduction_Contents.h"
#include "ItemProductionShop.h"

#include "CardEventContents.h"
#include "CardEvent.h"

#include "ChatContents.h"
#include "ChatBase.h"
#include "ChatMainWindow.h"
#include "ChatSystemWindow.h"
#include "ChatBattleLogWindow.h"

#include "DataTradeContents.h"
#include "DataTradeCenter.h"
#include "DataTradeBase.h"
#include "DataTradeSelect.h"
#include "DataTradeDigimonScale.h"
#include "DataTradeDigimonUpgrade.h"
#include "DataTradeDigimonEvolution.h"

#include "ItemRewardContents.h"

#include "RaidRank.h"

#include "RecommendEvent.h"
#include "RecommendEventButton.h"

#ifdef MINIGAME
#include "MiniGame.h"
#endif

#ifdef SIMPLEFICATION
#include "Simplification.h"
#endif

#include "DailyEventUI.h"

#include "RewardEventUI.h"	//2017-02-16-nova

#include "TamerSummon.h"

#include "Server_Reward.h"

#include "MastersMatching.h"

#include "Card_Identify.h"
#include "Card_ReSeal.h"

#include "Encyclopedia.h"
#include "Encyclopedia_Statistics.h"

#include "GlobalMessage.h"

#include "CapsuleEvol.h"

#include "FriendList.h"
#ifdef GUILD_RENEWAL
#include "GuildMemo.h"
#endif

#ifdef DAMAGE_METER
#include "DamageMeter.h"
#endif

#include "Event_Pinokimon.h"


#include "QuestFindWay.h" // [10/26/2016 hyun] 퀘스트 길찾기 개선

#include "ExpEvent.h"

#include "SealMaster.h"

//* 배틀 관련 UI	*/
#include "..\\09.BattleSystem\\BattleWaitingWindow.h"
#include "..\\09.BattleSystem\\BattleWaitingButton.h"
#include "..\\09.BattleSystem\\BattleRoundTimer.h"
#include "..\\09.BattleSystem\\BattleStartCounterUI.h"
#include "..\\09.BattleSystem\\BattleEndWindow.h"
#include "..\\09.BattleSystem\\BattleEvol.h"
//* 	*/

#include "../11.Tutorial/TutorialGuideWindow.h"
#include "../11.Tutorial/TutorialTraceWindow.h"

#include "../12.InfiniteWar/InfiniteWarStartWindow.h"
#include "../12.InfiniteWar/InfiniteWarGiveupWindow.h"
#include "../12.InfiniteWar/InfiniteWarNoticeBoard.h"
#include "../12.InfiniteWar/InfiniteWarNextStageWindow.h"
#include "../12.InfiniteWar/InfiniteWarAllClearWindow.h"
#include "../12.InfiniteWar/InfiniteWarStartTimerWindow.h"
#include "../12.InfiniteWar/InfiniteWarNoticeBoard_PreRankList.h"
#include "../12.InfiniteWar/InfiniteWarNoticeBoard_RankReward.h"
#include "../14.MainFrameUI/MainFrame_XaiGuageUI.h"
#include "../15.CashShop/NewCashshopViewer.h"
#include "../15.CashShop/NewCashshopStorageViewer.h"
#include "../16.EvolutionStore/EvolutionStore_Window.h"
#include "../17.VipMemberShip/VipSystemWindow.h"
#include "../18.SeasonPass/SeasonPassWindow.h"
#include "../19.EventNotice/EventNoticeWindow.h"

#include "../14.MainFrameUI/ActionKey.h"
#include "../14.MainFrameUI/GiftBoxUI.h"
#include "../14.MainFrameUI/MainFrame_TamerStatusUI.h"
#include "../14.MainFrameUI/MainFrame_DeleteDigimonUI.h"
#include "../14.MainFrameUI/MainFrame_DigimonStatusUI.h"
#include "../14.MainFrameUI/MainFrame_DigimonStatusHelpUI.h"
#include "../14.MainFrameUI/MainFrame_TargetUI.h"

#include "DigimonTranscendenceHelp.h"
#include "NewGotchaMachine.h"
#include "EquipSystemViewer.h"
#include "EquipSystemHelp.h"
#include "EnchantOption.h"

#include "../11.Tutorial/TutorialHelpUI.h"
#include "../11.Tutorial/TutorialSkipUI.h"
#include "ScreenOutUI.h"

#include "EventNoticeButton.h"
#include "SeasonPassButton.h"

#define IF_MAX_SUB_INVENTORY_COUNT		2
#define IF_MAX_QUICKSLOT_LINE_COUNT		3

class cGameInterface : public NiMemObject
{
public:
	cGameInterface();
	~cGameInterface();
	static void		GlobalInit();
	static void		GlobalShotDown();

public:
	void			Destroy();
	void			Init();

	void			PreResetMap();				// 존 이동 하기 전에 호출
	void			ResetMap();					// 존 이동 후 ( 로딩완료 되자마자 ( 로딩화면에서 ) 호출 )
	void			UserDie();
	void			RecvDiedAlready();

	cGlobalInput::eMOUSE_INPUT	Update(float const& fDeltaTime);
	void						Render();
	
	void			OnLButtonUp( CsPoint pos );
	void			OnRButtonUp( CsPoint pos );

	bool			CursorIconLBtnUp( CsPoint pos, CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );
	bool			CursorIconLBtnDown( CsPoint pos, CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount );

	void			ResetDevice();
	
	// 키보드
	bool			OnMacroKey();
	bool			OnMacroKey(const MSG& p_kMsg);	//2016-01-18-nova 윈도우Msg로 처리

	bool			OnEscapeKey();
	bool			OnEnterKey();

	void			ChangeMacroKey();

	bool			IsInWindow( CsPoint pos );

protected:
	void			_UpdateReserveFocusWindow();

protected:
	bool			m_bRenderIF;	
public:
	void			SetRenderIF( bool bRender ){ m_bRenderIF = bRender; }
	bool			IsRenderIF(){ return m_bRenderIF; }

	void			SetBright();

	// 초기 메모리 할당
protected:	
	cAlime*					m_pAlime;
	cPopUpWindow*			m_pPopupWindow;
	cBossScene*				m_pBossScene;
	cSprite*				m_pBright;
	CsVectorPB< cBaseWindow* >	m_vpReserveFocusWindow;
	CsVectorPB< bool >			m_vpReserveFocusCallFunc;

	cGlobalMessage*			m_pGlobalMsg;
public:
	cPopUpWindow*			GetPopup(){ return m_pPopupWindow; }
	cAlime*					GetAlime(){ return m_pAlime; }
	cBossScene*				GetBossScene() { return m_pBossScene; }
	cGlobalMessage*			GetGlobalMsg() { return m_pGlobalMsg; }

protected:
	std::map< DWORD, cBaseWindow* >		m_mapWindowArray;

public:
	cMainBar*				GetMainBar(){ return (cMainBar*)_GetPointer( cBaseWindow::WT_MAIN_BAR, 0 ); }
	cTamerWindow*			GetTamerWindow(){ return (cTamerWindow*)_GetPointer( cBaseWindow::WT_TAMER_WINDOW, 0 ); }
	cMainDigimonWindow*		GetMainDigimonWindow(){ return (cMainDigimonWindow*)_GetPointer( cBaseWindow::WT_MAIN_DIGIMON_WINDOW, 0 ); }
	cChatOption*			GetChatOption(){ return (cChatOption*)_GetPointer( cBaseWindow::WT_CHATOPTION, 0 ); }
	cQuickSlot*				GetQuickSlot( int nIndex ){ return (cQuickSlot*)_GetPointer( cBaseWindow::WT_QUICKSLOT, nIndex ); }
	cQuickEvol*				GetQuickEvol(){ return (cQuickEvol*)_GetPointer( cBaseWindow::WT_QUICKEVOL, 0 ); }

	cQuestHelper*			GetQuestHelper(){ return (cQuestHelper*)_GetPointer( cBaseWindow::WT_QUEST_HELPER, 0 ); }
	cMiniMap*				GetMiniMap(){ return (cMiniMap*)_GetPointer( cBaseWindow::WT_MINIMAP, 0 ); }
	cZoneMap*				GetZoneMap(){ return (cZoneMap*)_GetPointer( cBaseWindow::WT_ZONEMAP, 0 ); }
	cSmallMap*				GetSmallMap(){ return (cSmallMap*)_GetPointer( cBaseWindow::WT_SMALLMAP, 0 ); }
	cTacticsWindow*			GetTacticsWindow( int nTacticsIndex ){ return (cTacticsWindow*)_GetPointer( cBaseWindow::WT_TACTICS_WINDOW, nTacticsIndex ); }
	cInventory*				GetInventory( int nIndex ){ return (cInventory*)_GetPointer( cBaseWindow::WT_INVENTORY, nIndex ); }
	cDigiviceQuest*			GetDigiviceQuest(){ return (cDigiviceQuest*)_GetPointer( cBaseWindow::WT_DIGIVICEQUEST, 0 ); }
	
	cQuestList*				GetQuestList(){ return (cQuestList*)_GetPointer( cBaseWindow::WT_QUEST_LIST, 0 ); }
	cQuestDiscript*			GetQuestDiscript(){ return (cQuestDiscript*)_GetPointer( cBaseWindow::WT_QUEST_DISCRIPT, 0 ); }	
	cOptionSound*			GetOptionSound(){ return (cOptionSound*)_GetPointer( cBaseWindow::WT_OPTION_SOUND, 0 ); }
	cOptionMacro*			GetOptionMacro(){ return (cOptionMacro*)_GetPointer( cBaseWindow::WT_OPTION_MACRO, 0 ); }
	cOptionGraphic*			GetOptionGraphic(){ return (cOptionGraphic*)_GetPointer( cBaseWindow::WT_OPTION_GRAPHIC, 0 ); }
	cOptionInterface*		GetOptionInterface(){ return (cOptionInterface*)_GetPointer( cBaseWindow::WT_OPTION_INTERFACE, 0 ); }
	cOptionUserGrade*		GetOptionUserGrade(){ return (cOptionUserGrade*)_GetPointer( cBaseWindow::WT_OPTION_USERGRADE, 0 ); }
	cMainOption*			GetMainOption(){ return (cMainOption*)_GetPointer( cBaseWindow::WT_MAINOPTION, 0 ); }
	cElementItem_Check*		GetEI_Check(){ return (cElementItem_Check*)_GetPointer( cBaseWindow::WT_ELEMENTITEM_CHECK, 0 ); }
	cElementItem_Attach*	GetEI_Attach(){ return (cElementItem_Attach*)_GetPointer( cBaseWindow::WT_ELEMENTITEM_ATTACH, 0 ); }
	cWarehouse*				GetWarehouse(){ return (cWarehouse*)_GetPointer( cBaseWindow::WT_WAREHOUSE, 0 ); }
	cTrade*					GetTrade(){ return (cTrade*)_GetPointer( cBaseWindow::WT_TRADE, 0 ); }
	cTalk*					GetTalk(){ return (cTalk*)_GetPointer( cBaseWindow::WT_TALK, 0 ); }
	cQuestRev*				GetQuestRev(){ return (cQuestRev*)_GetPointer( cBaseWindow::WT_QUEST_REV, 0 ); }
	cQuestComp*				GetQuestComp(){ return (cQuestComp*)_GetPointer( cBaseWindow::WT_QUEST_COMP, 0 ); }
	cDigitama_Scan*			GetDigitamaScan(){ return (cDigitama_Scan*)_GetPointer( cBaseWindow::WT_DIGITAMA_SCAN, 0 ); }
	cDigitama_Sell*			GetDigitamaSell(){ return (cDigitama_Sell*)_GetPointer( cBaseWindow::WT_DIGITAMA_SELL, 0 ); }
	cMakeTactics*			GetMakeTactics(){ return (cMakeTactics*)_GetPointer( cBaseWindow::WT_MAKE_TACTICS, 0 ); }
	cEnchantTactics*		GetEnchantTactics(){ return (cEnchantTactics*)_GetPointer( cBaseWindow::WT_ENCHANT_TACTICS, 0 ); }
	cJumpBuster*			GetJumpBuster(){ return (cJumpBuster*)_GetPointer( cBaseWindow::WT_JUMPBUSTER, 0 ); }
	cWebWin*				GetWebWin(){ return (cWebWin*)_GetPointer( cBaseWindow::WT_WEBWIN, 0 ); }
	cBGSprite*				GetBGSprite( int nIndex ){ return (cBGSprite*)_GetPointer( cBaseWindow::WT_BGSPRITE, nIndex ); }		
	cCommunity*				GetCommunity(){ return (cCommunity*)_GetPointer( cBaseWindow::WT_COMMUNITY, 0 ); }
	cRankView*				GetGuildRank(){ return (cRankView*)_GetPointer( cBaseWindow::WT_GUILDRANK_VIEW, 0 ); }
	cMail*					GetMail() { return (cMail*)_GetPointer( cBaseWindow::WT_MAIL, 0 ); }
	cMailButton*			GetMailButton() { return (cMailButton*)_GetPointer( cBaseWindow::WT_MAIL_BUTTON, 0 ); }
	cAchieve*				GetAchieve() { return (cAchieve*)_GetPointer( cBaseWindow::WT_ACHIEVE, 0 ); }
	cUnionStore*			GetUnionStore(){ return (cUnionStore*)_GetPointer( cBaseWindow::WT_UNIONSTORE, 0 ); }
	
	cCenterbar*				GetCenterBar( int nIndex ) { return (cCenterbar*)_GetPointer( cBaseWindow::WT_CENTERBAR, nIndex ); }	

#ifdef MINIGAME
	cMinigame*				GetMiniGame(){ return (cMinigame*)_GetPointer( cBaseWindow::WT_MINIGAME, 0); }
#endif

#ifdef PLAY_PANELTY
	cPlayTime*				GetPlayTime(){ return (cPlayTime*)_GetPointer( cBaseWindow::WT_PLAYTIME, 0 ); }
#endif

	cBuffInfo*				GetBuffInfo( int nIndex ){ return (cBuffInfo*)_GetPointer( cBaseWindow::WT_BUFFINFO, nIndex ); }		

	cRaidRank*				GetRaidRank(){ return (cRaidRank*)_GetPointer( cBaseWindow::WT_RAIDRANK, 0 ); }
#ifdef SIMPLEFICATION
	cSimplification*		GetSimple(){ return (cSimplification*)_GetPointer( cBaseWindow::WT_SIMPLE_BTN, 0 ); }
#endif
	
	cTamerSummon*			GetTamerSummon(){ return (cTamerSummon*)_GetPointer( cBaseWindow::WT_TAMER_SUMMON, 0 ); }

public:
	cServer_Reward*			GetServer_Reward(){ return (cServer_Reward*)_GetPointer( cBaseWindow::WT_REWARD, 0 ); }

	cCardIdentify*			GetCardIdentify(){ return (cCardIdentify*)_GetPointer( cBaseWindow::WT_CARDIDENTIFY, 0 ); }
	cCardReSeal*			GetCardReSeal(){ return (cCardReSeal*)_GetPointer( cBaseWindow::WT_CARDRESEAL, 0 ); }

protected:
	CsC_AvObject*			m_pMastersMatchTarget;

public:
	void					SetMastersMatchTarget( CsC_AvObject* target ) { m_pMastersMatchTarget = target; }
	CsC_AvObject*			GetMastersMatchTarget(){ return m_pMastersMatchTarget; }

	cMastersMatching*		GetMastersMatching(){ return (cMastersMatching*)_GetPointer( cBaseWindow::WT_MASTERS_MATCHING, 0 ); }
	sMastersMatch_MyInfo	m_sMyInfo;
	sMastersMatch_RankInfo	m_sRankInfo;

	cEncyclopedia*			GetEncyclopedia(){ return (cEncyclopedia*)_GetPointer( cBaseWindow::WT_ENCYCLOPEDIA, 0 ); }
	cEncyclopedia_Statistics*	GetEncyclopedia_Statistics(){ return (cEncyclopedia_Statistics*)_GetPointer( cBaseWindow::WT_ENCYCLOPEDIA_STATS, 0 ); }

	cCapsuleEvolution*		GetCapsuleEvol(){ return (cCapsuleEvolution*)_GetPointer( cBaseWindow::WT_CAPSULE_EVOLUTION, 0 ); }

	cFriendList*			GetFriendList(){ return (cFriendList*)_GetPointer( cBaseWindow::WT_FRIEND_LIST, 0 ); }
#ifdef GUILD_RENEWAL
	cGuildMemo*				GetGuildMemo(){ return (cGuildMemo*)_GetPointer( cBaseWindow::WT_GUILD_MEMO, 0 ); }
#endif

	cEvent_Pinokimon*		GetEvent_Pinokimon(){ return (cEvent_Pinokimon*)_GetPointer( cBaseWindow::WT_EVENT_PINOKIMON, 0 ); }
	sEvent_PinokimonInfo	m_sEventInfo;

	ChatEditWindow*			GetChatEdit()	{ return (ChatEditWindow*)_GetPointer( cBaseWindow::WT_CHAT_EDIT_WINDOW, 0 ); }
	ChatMainWindow*			GetChatMain()	{ return (ChatMainWindow*)_GetPointer( cBaseWindow::WT_CHAT_MAIN_WINDOW, 0 ); }
	ChatBattleLogWindow*	GetChatBattle()	{ return (ChatBattleLogWindow*)_GetPointer( cBaseWindow::WT_CHAT_BATTLELOG_WINDOW, 0 ); }

	bool					IsOpenNpcWindow( bool bCheckTalk = true );
	bool					IsSpeacialAct();


	struct stWinType
	{
		cBaseWindow::eWINDOW_TYPE	type;
		int	index;
		stWinType(cBaseWindow::eWINDOW_TYPE _type, int _index=0) : type(_type), index(_index){}
	};

protected:
	CsVectorPB< cBaseWindow* >	m_vpDynamicUI;
	CsVectorPB< cBaseWindow* >	m_vpStaticUI;
	CsVectorPB< cBaseWindow* >	m_vpDelete;

	typedef	std::vector< stWinType >::iterator	WINTYPE_ITER;

	std::vector< stWinType >	m_vMouseKey;
	std::vector< stWinType >	m_vEnterKey;

private:
	void						EnterKeyCreate();	// 키보드 엔터키의 동작 순서를 생성순서로 결정해준다.		bool	OnEnterKey();					함수에서 사용된다.
	bool						EnterKeyUpdate();	


	// 동작
protected:
	cBaseWindow*		m_pStaticFocusWindow;

protected:
	void			_PointerDetach( cBaseWindow* pWindow );	

public:
	cBaseWindow*	_GetPointer( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex );

	bool			IsActiveWindow( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex = 0 );
	bool			IsActiveWindow( const char* pWinName ) const;
	cBaseWindow::eWINDOW_TYPE	GetWindowType( const char* pWinName ) const;
	cBaseWindow*	GetWindow( const char* pWinName );
	

public:
	// Dynamic
public:
	cBaseWindow*	GetDynamicIF( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex = 0, int nValue = 0, bool bOpenSound = true );
	bool			CloseDynamicIF( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex = 0, bool bIncludeDisableWindow = false );
protected:
	cBaseWindow*	_NewDynamicIF( cBaseWindow::eWINDOW_TYPE wt, bool bFocus, int nPointerIndex );
	void			_InsertDynamicWindow( cBaseWindow* pWindow, bool bFocus = false, int nPointerIndex = 0 );
	void			_InsertStaticWindow( cBaseWindow* pWindow, bool bFocus = false, int nPointerIndex = 0 );	// Static

private:
	cBaseWindow*	CreateDynamicWindow( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex = 0, int value = 0, bool bFocus = false, bool bShow = true, bool bSound = false);
	cBaseWindow*	CreateStaticWindow( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex = 0, int value = 0, bool bFocus = false, bool bShow = true, bool bSound = false);

	void			DeleteDynamicWindow( cBaseWindow::eWINDOW_TYPE wt);
	void			DeleteStaticWindow( cBaseWindow::eWINDOW_TYPE wt);
public:
	bool			GetFocusWindow( cBaseWindow* pWindow );
	void			SetFocusWindow( cBaseWindow* pWindow, bool bCallFounsFunc = true );

public:
 	void			OnLButtonDown( CsPoint pos );

	cCardEvent*		GetCardEvent() { return (cCardEvent*)_GetPointer( cBaseWindow::WT_CARD_EVENT, 0 ); }

public:
	std::map<DWORD, bool>	m_MapHideWindow;		//	윈도우 숨김 여부
	void					HideWindow();			// 윈도우 숨기기
	void					HideWindowRestore();	// 숨겼던 윈도우 복구
	void					HideWindowRestore( DWORD const& dwWinType, int nPtrIndex = 0 );


	void					DeleteWindowUpdate();
};

extern cGameInterface*		g_pGameIF;