
#include "stdafx.h"
#include "_BaseWindow.h"

bool cBaseWindow::Construct()
{
	return true;
}

bool cBaseWindow::IsLive()
{
	return m_bInitialize;
}

bool cBaseWindow::IsShowWindow()
{
	return m_bShowWindow;
}

void cBaseWindow::SetShowWindow( bool bShow )
{
	m_bShowWindow = bShow;
}

void cBaseWindow::SetStaticWindow( bool bStatic )
{
	m_bStaticWindow = bStatic;
}

bool cBaseWindow::IsStaticWindow()
{ 
	return m_bStaticWindow;
}

void cBaseWindow::__SetPointerIndex( int nPointerIndex )
{
	assert_cs( nPointerIndex >= 0 ); m_nPointerIndex = nPointerIndex;
}

int	 cBaseWindow::__GetPointerIndex()
{ 
	return m_nPointerIndex;
}

void cBaseWindow::DestroyWindow( cBaseWindow* pWindow )
{
	SAFE_POINTER_RET( pWindow );
	pWindow->Destroy();
	NISAFE_DELETE( pWindow );
}

cBaseWindow* cBaseWindow::NewWindow( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex /*=0*/ )
{
	switch( wt )
	{
	case WT_MAIN_BAR:					return CreateNewWindow<cMainBar>( "MainBar", nPointerIndex);
#ifdef PLAY_PANELTY
	case WT_PLAYTIME:					return CreateNewWindow<cPlayTime>( "PlayTime", nPointerIndex);
#endif
	case WT_TAMER_WINDOW:				return CreateNewWindow<cTamerWindow>( "TamerWindow", nPointerIndex);
	case WT_MAIN_DIGIMON_WINDOW:		return CreateNewWindow<cMainDigimonWindow>( "MainDigimonWindow", nPointerIndex);
	case WT_TARGET_WINDOW:				return CreateNewWindow<cTargetUI>( "TargetUI", nPointerIndex);
 	case WT_TACTICS_WINDOW:				return CreateNewWindow<cTacticsWindow>( "TacticsWindow", nPointerIndex);
 	case WT_XAIGUAGE_WINDOW:			return CreateNewWindow<cXaiGuageUI>( "XaiGuageWindow", nPointerIndex);
 	case WT_QUICKSLOT:					return CreateNewWindow<cQuickSlot>( "QuickSlot", nPointerIndex);
 	case WT_QUICKEVOL:					return CreateNewWindow<cQuickEvol>( "QuickEvol", nPointerIndex);
 	case WT_CHATOPTION:					return CreateNewWindow<cChatOption>( "ChatOption", nPointerIndex);
 	case WT_MINIMAP:					return CreateNewWindow<cMiniMap>( "MiniMap", nPointerIndex);
 	case WT_DIGIVICEQUEST:				return CreateNewWindow<cDigiviceQuest>( "DigiviceQuest", nPointerIndex);
 	case WT_PARTY_WINDOW:				return CreateNewWindow<cPartyWindow>( "PartyWindow", nPointerIndex);
 	case WT_BUFFINFO:					return CreateNewWindow<cBuffInfo>( "BuffInfo", nPointerIndex);
 	case WT_INVENTORY:					return CreateNewWindow<cInventory>( "Inventory", nPointerIndex);
 	case WT_TALK:						return CreateNewWindow<cTalk>( "Talk", nPointerIndex);
 	case WT_STORE:						return CreateNewWindow<cStore>( "Store", nPointerIndex);
	case WT_PERSONSTORE:				return CreateNewWindow<cPersonStore>( "PersonStore", nPointerIndex);
	case WT_DIGITAMA_SELL:				return CreateNewWindow<cDigitama_Sell>( "DigitamaSell", nPointerIndex);
	case WT_DIGITAMA_SCAN:				return CreateNewWindow<cDigitama_Scan>( "DigitamaScan", nPointerIndex);
	case WT_QUEST_REV:					return CreateNewWindow<cQuestRev>( "QuestRev", nPointerIndex);
	case WT_QUEST_COMP:					return CreateNewWindow<cQuestComp>( "QuestComp", nPointerIndex);
	case WT_QUEST_LIST:					return CreateNewWindow<cQuestList>( "QuestList", nPointerIndex);
	case WT_QUEST_DISCRIPT:				return CreateNewWindow<cQuestDiscript>( "QuestDiscript", nPointerIndex);
	case WT_QUEST_HELPER:				return CreateNewWindow<cQuestHelper>( "QuestHelper", nPointerIndex);
	case WT_MAKE_TACTICS:				return CreateNewWindow<cMakeTactics>( "MakeTactics", nPointerIndex);
	case WT_ENCHANT_TACTICS:			return CreateNewWindow<cEnchantTactics>( "EnchantTactics", nPointerIndex);
	case WT_ZONEMAP:					return CreateNewWindow<cZoneMap>( "ZoneMap", nPointerIndex);
	case WT_MAINOPTION:					return CreateNewWindow<cMainOption>( "MainOption", nPointerIndex);
	case WT_OPTION_SOUND:				return CreateNewWindow<cOptionSound>( "OptionSound", nPointerIndex);
	case WT_OPTION_MACRO:				return CreateNewWindow<cOptionMacro>( "OptionMacro", nPointerIndex);
	case WT_OPTION_GRAPHIC:				return CreateNewWindow<cOptionGraphic>( "OptionGraphic", nPointerIndex);
	case WT_OPTION_INTERFACE:			return CreateNewWindow<cOptionInterface>( "OptionInterface", nPointerIndex);
	case WT_OPTION_USERGRADE:			return CreateNewWindow<cOptionUserGrade>( "OptionUserGrade", nPointerIndex);
	case WT_ELEMENTITEM_CHECK:			return CreateNewWindow<cElementItem_Check>( "ElementItemCheck", nPointerIndex);
	case WT_ELEMENTITEM_ATTACH:			return CreateNewWindow<cElementItem_Attach>( "ElementItemAttach", nPointerIndex);
	case WT_WAREHOUSE:					return CreateNewWindow<cWarehouse>( "Warehouse", nPointerIndex);
	case WT_UNIONSTORE:					return CreateNewWindow<cUnionStore>( "UnionStore", nPointerIndex);
	case WT_TRADE:						return CreateNewWindow<cTrade>( "Trade", nPointerIndex);
	case WT_NPCFIND:					return CreateNewWindow<cNpcFind>( "NpcFind", nPointerIndex);
	case WT_COMMUNITY:					return CreateNewWindow<cCommunity>( "Community", nPointerIndex);
	case WT_GUILDSET:					return CreateNewWindow<cGuildSet>( "GuildSet", nPointerIndex);
	case WT_GUILDRANK_VIEW:				return CreateNewWindow<cRankView>( "RankView", nPointerIndex);
	case WT_CASHSHOP_BUTTON:			return CreateNewWindow<cCashShopButton>( "CashShopButton", nPointerIndex);
	case WT_NEW_CASHSHOP:				return CreateNewWindow<NewCashshopViewer>( "NewCashShop", nPointerIndex);
	case WT_NEW_CASH_WAREHOUSE:			return CreateNewWindow<NewCashshopStorageViewer>( "NewCashWarehouse", nPointerIndex);
	case WT_TACTICSHOUSE:				return CreateNewWindow<CDigimonArchiveViewer>( "TacticsHouse", nPointerIndex);
	case WT_BGSPRITE:					return CreateNewWindow<cBGSprite>( "BGSprite", nPointerIndex);
	case WT_JUMPBUSTER:					return CreateNewWindow<cJumpBuster>( "JumpBuster", nPointerIndex);
	case WT_CHANGETAMER:				return CreateNewWindow<cChangeTamer>( "ChangeTamer", nPointerIndex);
	case WT_TUTORIAL:					return CreateNewWindow<cTutorialGuideUI>( "Tutorial", nPointerIndex);
	case WT_TUTORIAL_TRACE:				return CreateNewWindow<cTutorialTraceWindow>( "TutorialTrace", nPointerIndex);
	case WT_MAIL:						return CreateNewWindow<cMail>( "Mail", nPointerIndex);
	case WT_MAIL_BUTTON:				return CreateNewWindow<cMailButton>( "MailButton", nPointerIndex);
	case WT_LETTER:						return CreateNewWindow<cLetter>( "Letter", nPointerIndex);
	case WT_PUBLICITY:					return CreateNewWindow<cPublicity>( "Publicity", nPointerIndex);
	case WT_CHANNEL:					return CreateNewWindow<cChannel>( "Channel", nPointerIndex);
	case WT_SMALLMAP:					return CreateNewWindow<cSmallMap>( "SmallMap", nPointerIndex);
	case WT_WEBWIN:						return CreateNewWindow<cWebWin>( "WebWin", nPointerIndex);
	case WT_DIGIMON_TALK:				return CreateNewWindow<cDigimonTalk>( "DigimonTalk", nPointerIndex);
	case WT_PROTECT:					return CreateNewWindow<cProtect>( "Protect", nPointerIndex);
	case WT_ACHIEVE:					return CreateNewWindow<cAchieve>( "Achieve", nPointerIndex);
	case WT_DIVISION:					return CreateNewWindow<cDivision>( "Division", nPointerIndex);
	case WT_LOOTING:					return CreateNewWindow<cPartyItemLoot>( "PartyItemLoot", nPointerIndex);
	case WT_MYLOOTING:					return CreateNewWindow<cMyItemLoot>( "MyItemLoot", nPointerIndex);
	case WT_PCBANGINVEN:				return CreateNewWindow<cPcbangInven>( "PcbangInven", nPointerIndex);
	case WT_SHOUT_CASH:					return CreateNewWindow<cShout>( "Shout", nPointerIndex);
	case WT_CENTERBAR:					return CreateNewWindow<cCenterbar>( "Centerbar", nPointerIndex);
	case WT_RECOMMENDEVENT:				return CreateNewWindow<cRecommendEvent>( "RecommendEvent", nPointerIndex);
	case WT_RECOMMENDEVENT_BUTTON:		return CreateNewWindow<CRecommendEventButton>( "RecommendEventButton", nPointerIndex);
#ifdef MINIGAME
	case WT_MINIGAME:						return CreateNewWindow<cMinigame>( "Minigame", nPointerIndex);
#endif

	case WT_CARD_EVENT:						return CreateNewWindow<cCardEvent>( "CardEvent", nPointerIndex);
	case WT_RAIDRANK:						return CreateNewWindow<cRaidRank>( "RaidRank", nPointerIndex);
#ifdef SIMPLEFICATION
	case WT_SIMPLE_BTN:						return CreateNewWindow<cSimplification>( "Simplification", nPointerIndex);
#endif
	case WT_DAILY_EVENT_UI:					return CreateNewWindow<cDailyEventUI>( "DailyEventUI", nPointerIndex);

	case WT_REWARD_EVENT_UI:				return CreateNewWindow<cRewardEventUI>( "RewardEventUI", nPointerIndex);
	case WT_TAMER_SUMMON:					return CreateNewWindow<cTamerSummon>( "TamerSummon", nPointerIndex);
	case WT_REWARD:							return CreateNewWindow<cServer_Reward>( "ServerReward", nPointerIndex);

	case WT_MASTERS_MATCHING:				return CreateNewWindow<cMastersMatching>( "MastersMatching", nPointerIndex);

	case WT_CARDINVENTORY:					return CreateNewWindow<cSealMaster>( "SealMaster", nPointerIndex);
	
	case WT_CARDIDENTIFY:					return CreateNewWindow<cCardIdentify>( "CardIdentify", nPointerIndex);
	case WT_CARDRESEAL:						return CreateNewWindow<cCardReSeal>( "CardReSeal", nPointerIndex);
	
	case WT_ENCYCLOPEDIA:					return CreateNewWindow<cEncyclopedia>( "Encyclopedia", nPointerIndex);
	case WT_ENCYCLOPEDIA_STATS:				return CreateNewWindow<cEncyclopedia_Statistics>( "EncyclopediaStatistics", nPointerIndex);
	case WT_CAPSULE_EVOLUTION:				return CreateNewWindow<cCapsuleEvolution>( "CapsuleEvolution", nPointerIndex);
	case WT_FRIEND_LIST:					return CreateNewWindow<cFriendList>( "FriendList", nPointerIndex);
#ifdef GUILD_RENEWAL
	case WT_GUILD_MEMO:						return CreateNewWindow<cGuildMemo>( "GuildMemo", nPointerIndex);
#endif

	case WT_SPIRIT_EVOLUTION:				return CreateNewWindow<cExtraEvolutionUI>( "SpiritEvo", nPointerIndex);

#ifdef DAMAGE_METER
	case WT_DAMAGE_METER:					return CreateNewWindow<cDamageMeter>( "DamageMeter", nPointerIndex);
#endif
	case WT_EVENT_PINOKIMON:				return CreateNewWindow<cEvent_Pinokimon>( "Event_Pinokimon", nPointerIndex);

	case WT_ITEM_PRODUCTION_SHOP:			return CreateNewWindow<cItemProductionShop>( "ItemProduction", nPointerIndex);
	case WT_CHAT_EDIT_WINDOW:				return CreateNewWindow<ChatEditWindow>( "ChatEditWindow", nPointerIndex);
	case WT_CHAT_MAIN_WINDOW:				return CreateNewWindow<ChatMainWindow>( "ChatMainWindow", nPointerIndex);
	case WT_CHAT_SYSTEM_WINDOW:				return CreateNewWindow<ChatSystemWindow>( "ChatSystemWindow", nPointerIndex);
	case WT_CHAT_BATTLELOG_WINDOW:			return CreateNewWindow<ChatBattleLogWindow>( "ChatBattleLogWindow", nPointerIndex);

	case WT_DATA_TRADE_SELECT:				return CreateNewWindow<DataTradeSelect>( "DataTradeSelect", nPointerIndex);
	case WT_DATA_TRADE_DIGIMON_SCALE:		return CreateNewWindow<DataTradeDigimonScale>( "DataTradeDigimonScale", nPointerIndex);
	case WT_DATA_TRADE_DIGIMON_UPGRADE:		return CreateNewWindow<DataTradeDigimonUpgrade>( "DataTradeDigimonUpgrade", nPointerIndex);
	case WT_DATA_TRADE_DIGIMON_EVOLUTION:	return CreateNewWindow<DataTradeDigimonEvolution>( "DataTradeDigimonEvolution", nPointerIndex);

	case WT_EXP_EVENT_UI:					return CreateNewWindow<cExpEventUI>( "ExpWindowUI", nPointerIndex);

		//* 배틀 관련 UI */
	case WT_BATTLE_MATCHING_WAIT_BUTTON:	return CreateNewWindow<CBattleWaitingBtn>( "BattleMatchingWaitButton", nPointerIndex);
	case WT_BATTLE_MATCHING_WAIT_WINDOW:	return CreateNewWindow<CBattleWaitWindow>( "BattleWaitWindow", nPointerIndex);
	case WT_BATTLEROUND_TIMER:				return CreateNewWindow<cBattleRoundTimer>( "BattleRoundTimer", nPointerIndex);
	case WT_BATTLESTART_TIMER:				return CreateNewWindow<cBattleStartCounter>( "BattleStartCounter", nPointerIndex);
	case WT_BATTLESTART_END:				return CreateNewWindow<cBattleEndWindow>( "BattleEndWindow", nPointerIndex);
	case WT_BATTLE_EVOL:					return CreateNewWindow<cBattleEvol>( "BattleEvol", nPointerIndex);
		
		//  무한대전 관련 UI
	case WT_INFINITEWAR_START_WINDOW:		return CreateNewWindow<CInfiniteWarStartWindow>( "InfiniteWarStartWindow", nPointerIndex);
	case WT_INFINITEWAR_GIVEUP_WINDOW:		return CreateNewWindow<CInfiniteWarGiveupWindow>( "InfiniteWarGiveupWindow", nPointerIndex);
	case WT_INFINITEWAR_NOTICEBOARD_WINDOW:	return CreateNewWindow<CInfiniteWarNoticeBoardWindow>( "InfiniteWarNoticeBoardWindow", nPointerIndex);
	case WT_INFINITEWAR_NEXTSTAGE_WINDOW:	return CreateNewWindow<CInfiniteWarNextStagedWindow>( "InfiniteWarNextStageWindow", nPointerIndex);
	case WT_INFINITEWAR_ALLCLEAR_WINDOW:	return CreateNewWindow<CInfiniteWarAllClearWindow>( "InfiniteWarAllClearWindow", nPointerIndex);
	case WT_INFINITEWAR_START_TIMER_WINDOW:	return CreateNewWindow<CInfiniteWarStartTimerWindow>( "InfiniteWarStartTimerWindow", nPointerIndex);
	case WT_INFINITEWAR_PRERANKLIST_WINDOW:	return CreateNewWindow<CInfiniteNBPreRankListWindow>( "InfiniteNoticeBoardPreRankListWindow", nPointerIndex);
	case WT_INFINITEWAR_RANKREWARD_WINDOW:	return CreateNewWindow<CInfiniteNBRankRewardWindow>( "InfiniteNoticeBoardRankRewardWindow", nPointerIndex);
	
	case WT_ACTIONKEY:						return CreateNewWindow<cActionKey>( "ActionKey", nPointerIndex);
	case WT_GIFTBOX:						return CreateNewWindow<cGiftBoxUI>( "GiftBoxUI", nPointerIndex);
	case WT_NEW_TAMERSTATUS:				return CreateNewWindow<cTamerStatusUI>( "TamerStatusUI", nPointerIndex);
	case WT_DELETEDIGIMON:					return CreateNewWindow<cDeleteDigimonUI>( "DeleteDigimonUI", nPointerIndex );
	case WT_NEW_DIGIMONSTATUS:				return CreateNewWindow<cDigimonStatusUI>( "DigimonStatusUI", nPointerIndex);
	case WT_DIGIMONSTATUS_HELP:				return CreateNewWindow<cDigimonStatusUI_Help>( "DigimonStatusUI_Help", nPointerIndex);
	case WT_TRANSCENDENCE_HELP:				return CreateNewWindow<cDigimonTranscendenceHelp>( "DigimonTranscendenceHelp", nPointerIndex );
	case WT_NEW_GOTCHA_MACHINE:				return CreateNewWindow<cNewGotchaMachine>( "NewGotchaMachine", nPointerIndex );
	case WT_EQUIPMENT_SYSTEM:				return CreateNewWindow<cEquipSystemViewer>( "EquipmentSystem", nPointerIndex );
	case WT_EQUIPMENT_SYSTEM_HELP:			return CreateNewWindow<cEquipSystemHelp>( "EquipmentSystemHelp", nPointerIndex );
	case WT_ENCHANT_OPTION:					return CreateNewWindow<cEnchantOption>( "EnchantOption", nPointerIndex );

	case WT_TUTORIAL_HELP_UI:				return CreateNewWindow<cTutorialHelpUI>( "TutorialHelpUI", nPointerIndex );
	case WT_TUTORIAL_SKIP_UI:				return CreateNewWindow<cTutorialSkipUI>( "TutorialSkipUI", nPointerIndex );
	case WT_SCREENOUT_UI:					return CreateNewWindow<cScreenOutUI>( "ScreenOutUI", nPointerIndex );

	// MAIN GAME BUTTONS
	case WT_VIP_BUTTON:						return CreateNewWindow<CVipSystemWindow>("VipSystemWindow", nPointerIndex);
	case WT_NOTICE_BUTTON:					return CreateNewWindow<CEventNoticeButton>("EventNoticeButton", nPointerIndex);
	case WT_NOTICE_WINDOW:					return CreateNewWindow<EventNoticeWindow>("EventNoticeWindow", nPointerIndex);
	case WT_SEASONPASS_BUTTON:				return CreateNewWindow<cSeasonPassButton>("SeasonPassButton", nPointerIndex);
	case WT_SEASONPASS_WINDOW:				return CreateNewWindow<SeasonPassWindow>("SeasonPassWindow", nPointerIndex);

	default:
		assert_csm( false,  "Fail to Create cBaseWindow::NewWindow");
		return NULL;
	}
}

cBaseWindow::cBaseWindow()
:m_bInitialize(false),m_bHaveNotFocus(false),m_bAlwayTop(false)
,m_bMouseInWindow(false),m_bShowWindow(false),m_bActiveMoveWindow(false)
{
}

cBaseWindow::cBaseWindow(char* pWindowName)
:m_bInitialize(false),m_bHaveNotFocus(false),m_bAlwayTop(false)
,m_bMouseInWindow(false),m_bShowWindow(false),m_bActiveMoveWindow(false)
{
}

cBaseWindow::eMU_TYPE cBaseWindow::Update_ForMouse()
{
	if( IsShowWindow() == false )
		return MUT_OUT_WINDOW;
	if (m_pRoot) {

	
	CURSOR::eTYPE type = m_pRoot->Click();

	switch( type )
	{
	case CURSOR::BUTTON_OUTWINDOW:
		if( m_bMouseInWindow == true )
		{
			OnMouseOutWindow();
			m_bMouseInWindow = false;
		}
		return MUT_OUT_WINDOW;

	case CURSOR::LBUTTON_DOWN:
	case CURSOR::RBUTTON_DOWN:
	case CURSOR::MBUTTON_DOWN:
		m_bMouseInWindow = true;
		return MUT_CLICK_FOCUS;
	}
	m_bMouseInWindow = true;
	}

	return MUT_NONE;
}

CsPoint cBaseWindow::_UpdateMoveWindow()
{
	CsPoint ptDelta = CsPoint::ZERO; //포인트를 0으로 초기화

	if( m_bActiveMoveWindow == false )
		return ptDelta;

	if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_PRESS )
	{
		ptDelta = CURSOR_ST.GetPos() - m_ptBackupMousePos;
		if( SetMove( ptDelta ) == true )
		{
			m_ptBackupMousePos = CURSOR_ST.GetPos();
			OnMoveWindow();
		}			
		return ptDelta;
	}
	m_bActiveMoveWindow = false;
	return ptDelta;
}

// Update_ForMouse 에서 호출 하여야 함
void cBaseWindow::_UpdateMoveWindow_ForMouse()
{
	if( m_bMoveWindow == false )
		return;

	assert_cs( m_pRoot->Click() != CURSOR::BUTTON_OUTWINDOW );
	if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN )
	{		
		m_bActiveMoveWindow = true;
		m_ptBackupMousePos = CURSOR_ST.GetPos();
	}
}


void cBaseWindow::Destroy()
{
	cBaseWindow::Delete();
	cBaseWindow::OnKillFocus();
}		
void cBaseWindow::UserDie(){}
void cBaseWindow::RecvDiedAlready(){}

void cBaseWindow::Open( int nValue, bool bSound )
{
	if( bSound )
		PlayOpenSound();

	Create( nValue );
}
bool cBaseWindow::Close( bool bSound /* = true */ )
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
		PlayCloseSound();

	Destroy();
	return true;
}
void cBaseWindow::PlayOpenSound()
{ 
	cWindow::PlaySound( cWindow::SD_Wi1 );
}
void cBaseWindow::PlayCloseSound()
{ 
	cWindow::PlaySound( cWindow::SD_Wi2 );
}

void cBaseWindow::Delete()
{
	if( IsLive() )
		DeleteResource();

	m_ptOldScreenSize = CsPoint::ZERO; m_bInitialize = m_bShowWindow = false;
}
bool cBaseWindow::Init()
{
	assert_cs( m_bShowWindow == false );
	bool bReturn = !m_bInitialize;  m_bInitialize = m_bShowWindow = true;
	return bReturn;
}
const char* cBaseWindow::GetWindowName() const 
{ 
	return m_strWindowName.c_str(); 
}

void cBaseWindow::SetWindowName( const char * pWinName )
{
	m_strWindowName = pWinName;
}

bool cBaseWindow::IsSameWindow( const char* pWinName ) const
{
	if( m_strWindowName.empty() )
		return false;

	if( m_strWindowName.compare( pWinName ) == 0 )
		return true;
	return false;
}