#include "StdAfx.h"
#include "cNetwork.h"
#include "cCliGame.h"
#include "common_vs2019/pGame.h"
#include "common_vs2019/pItem.h"
#include "common_vs2019/pServer.h"
#include "common_vs2019/pSession.h"
#include "common_vs2019/pTrade.h"
#include "common_vs2019/pScanner.h"
#include "common_vs2019/pSkill.h"
#include "common_vs2019/pMaster.h"
#include "common_vs2019/pTamer.h"
#include "common_vs2019/pPass2.h"
#include "common_vs2019/pBuddy.h"
#include "common_vs2019/pParty.h"
#include "common_vs2019/pGuild.h"
#include "common_vs2019/pEvent.h"
#include "common_vs2019/pDigimon.h"
#include "common_vs2019/pCashShop.h"
#include "common_vs2019/pManager.h"
#include "common_vs2019/pInsMap.h"
#include "common_vs2019/pBuff.h"
#include "common_vs2019/pmonster.h"
#include "common_vs2019/cVersion.h"

#include "../Flow/Flow.h"
#include "../Flow/FlowMgr.h"



cCliGame::cCliGame(void) : cClient(nSession::User),
	m_bIsReadyCompleted(false),
	m_bPortalProcessing(false),
	m_bPortalRequesting(false),
	m_nIsChangeServer(false)
{
}


cCliGame::~cCliGame(void)
{
	cClient::Stop();

	DBG("destroyed...");
}



bool cCliGame::start(void)
{
	if(IsNetOff())
	{
		DBG("NETWORK IS DISABLED...");
		return true;
	}

	int nConnectTryCount = 0;

nextwork:
	if(Bind() == false)
	{
		LOG("bind failure");
		PostQuitMessage(nErr::Bind);
		return false;
	}

	EnableEncryption( cVersion::GetVersion() );

	if(!Connect(ip, port))
	{
		if(++nConnectTryCount < 5)
		{
			LOG("connect(%s:%d) try:%d failure", ip, port, nConnectTryCount);
			Sleep( 999 );
			goto nextwork;
		}

		PostQuitMessage(nErr::Connect);
		LOG("connect(%s:%d) failure...", ip, port);
		return false;
	}

//	if(Run(2) == false) // 2 work thread
	if(Run(0) == false)
	{
		DBG("run failure");
		PostQuitMessage(nErr::Run);
		return false;
	}
	m_nConnectedTickCount = GetTickCount();

	return true;
}


void cCliGame::send(void)
{
	if(IsSendBlock())
	{
		return;
	}

	cClient::send();
}


void cCliGame::OnConnected(void)
{
#ifndef _GIVE
	LOG("게임 서버 접속");
#endif //_GIVE

	SendAccessCode();
	SetWindowTitle(true);

//#ifndef _GIVE
	//Sleep(333);
//#endif //_GIVE

	//SetNextGameStep(GAME_LOADING);
}



void cCliGame::OnDisconnected(char *msg)
{
//#ifndef _GIVE
	LOG("Disconnected -> %s", msg);
//#endif //_GIVE

	if(stricmp( msg, "999" ) == 0)// 다른 곳에서 접속을 시도하였을 경우 처리
	{
		cPrintMsg::PrintMsg( 10013 );
		return;
	}

	if(stricmp( msg, "888" ) == 0)
	{
		GLOBALDATA_ST.ResetAccountInfo();
		FLOWMGR_ST.ChangePopAllFlow(Flow::CFlow::FLW_LOGIN);
		//SetNextGameStep( GAME_LOGOUT );
		return;
	}


	if(m_nIsChangeServer)
	{
		return;
	}

	SetWindowTitle(false);

	cPrintMsg::PrintMsg( cPrintMsg::SERVER_DISCONNET );
	if( g_pGameIF )
	{		
		wstring str = nBase::m2w( msg );
		if( !str.empty() )
		{
			ContentsStream kStream;
			bool bParcing = true;	bool bCheckSameString = false;
			kStream << str << bParcing << bCheckSameString;
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_SYSTEM_PROCESS, &kStream );
		}
	}

#ifndef _GIVE
	if(!strstr(msg, "error code"))
	{
		MessageBoxA(GAMEAPP_ST.GetHWnd(), msg, "Digimon Masters...", MB_OK);
	}
#endif //!_GIVE
}

void cCliGame::OnExecute(void)
{
	WORD Protocol = GetProtocol();

	if(!net::receive_active)
	{
		switch( Protocol )
		{
		case pSvr::Change				: RecvChangeServer();			return;
		case pGameTwo::GameGuardCheck	: Recv_NProtectCheck();			return;		// nprotect 게임 가드 결과
		case pGameTwo::XignCodeCheck	: Recv_XignCodeCheck();			return;
		}

		return;
	}

	switch(Protocol)
	{
#ifdef HANDSHAKE_DISABLE
	case 65535						: RecvHandshake487(); return;
#endif
	case pNet::VersionError			: RecvVersionError();			return;
	case pGame::StartMessage		: RecvStartMessage();			return;
	case pGame::InitData			: RecvInitGameData();			return;	
	case pGame::Emoticon			: RecvEmoticon();				return;

	case pSvr::ChannelInfo			: RecvChannelInfo();			return;
	case pItem::CropMoney			: RecvCropMoney();				return;
	case pItem::PartyCrop			: RecvItemCropParty();			return;

	case pGame::AdvertBossCount		: RecvAdvertBossCount();		return;
	case pGame::AdvertBossDie		: RecvAdvertBossDie();			return;
	case pGame::AdvertBossCreate	: RecvAdvertBossCreate();		return;

//	case pDigimon::AdvertBurstMode	: RecvAdvertBurstMode();		return;

	case pGame::Sync				: RecvSyncData();				return;
	case pGame::DiedAlready			: RecvDiedAlready();			return;
	case pItem::Buy					: RecvItemBuySuccess();			return;
	case pItem::Make				: RecvItemMake();				return;
	case pItem::Sell				: RecvItemSellSuccess();		return;
	case pItem::DigitamaToMoney		: RecvDigitamaSellResult();		return;

// 	case pInsMap::BattleJoin		: RecvBattleJoin();				return;
// 	case pInsMap::BattleReadyComplete:RecvBattleReadyComplete();	return;

#ifdef SELECT_SERVER_ERROR
	case pSvr::AccessCode			: RecvSelectServer_Er();		return;
#endif
	case pGameTwo::GameGuardCheck	: Recv_NProtectCheck();			return;		// nprotect 게임 가드 결과
	case pGameTwo::XignCodeCheck	: Recv_XignCodeCheck();			return;
	}

	if(!m_bIsReadyCompleted)
	{
		DBG("(protocol:%d) -> Protocol error", Protocol);
		return;
	}

	switch(Protocol)
	{
#ifdef HANDSHAKE_DISABLE
	case 65535: RecvHandshake487(); return;
#endif
	case 1087:	return;
	case pScanner::Register			: RecvScannerRegister();		return;
	case pScanner::Release			: RecvScannerRelease();			return;

	case pScanner::Read				: RecvCardScan();				return;

	case pScanner::Equip			: RecvScannerEquip();			return;	// 스캐너 장착을 주변에 알림, 본인에게는 오지 아니함
	case pScanner::On				: RecvScannerOn();				return;	// 스캐너 장착되었음을 알림
	case pScanner::Off				: RecvScannerOff();				return;

	case pScanner::Error			: RecvScannerError();			return;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 매트로 프로텍트 관련 패킷
	//////////////////////////////////////////////////////////////////////////////////////////
	case pGame::MacroFailure		: RecvMacroFailure();			return;
	case pGame::MacroSuccess		: RecvMacroSuccess();			return;
	case pGame::MacroResult			: RecvMacroResult();			return;
	case pGame::MacroQuestion		: RecvMacroQuestion();			return;
	//////////////////////////////////////////////////////////////////////////////////////////

	case pGame::MsgAllFailure		: RecvMsgAllByItemFailure();	return;

	//////////////////////////////////////////////////////////////////////////////////////////
	// 유저간 거래 처리
	case pTrade::Request			: RecvTradeRequest();			return; // 거래 요청
	case pTrade::Approval			: RecvTradeApproval();			return; // 상대가 거래 승인
	case pTrade::Impossible			: RecvTradeImpossible();		return; // 거래 불가한 상태
	case pTrade::Reject				: RecvTradeReject();			return;	// 거래 거부
	case pTrade::AddItem			: RecvTradeAddItem();			return; // 아이템 추가
	case pTrade::CancelItem			: RecvTradeCancelItem();		return; // 아이템 추가 취소
	case pTrade::AddMoney			: RecvTradeAddMoney();			return;	// 겜머니 추가

	case pTrade::InvenLock			: RecvTradeInvenLock();			return;	// 거래 인벤토리 잠금.
	case pTrade::CompleteCancel		: RecvTradeUnInvenLock();		return;	// 거래 인벤토리 해제.	

	case pTrade::Complete			: RecvTradeComplete();			return; // 최종 거래 확인 패킷
	case pTrade::CompleteFinal		: RecvTradeCompleteFinal();		return; // 거래 완료 최종
	
	case pDigimon::SetMasterCardLeader	: RecvSetMasterCardLeader();	return;
	case pDigimon::UnSetMasterCardLeader: RecvUnSetMasterCardLeader();	return;
	case pTamer::CardInfoAll : RecvCardInfoAll();	return;
	case pTamer::CardBookmark: RecvCardBookmark();	return;
//////////////////////////////////////////////////////////////////////////////////////////
// 테이머 개인 상점
	case pTrade::TamerShopReady		: RecvTamerShopReadyResult();	return; //개인 상점 오픈 준비
	case pTrade::TamerShopOpen		: RecvTamerShopOpenResult();	return; // 개인 상점 오픈 요청 결과
	case pTrade::TamerShopList		: RecvTamerShopItemList();		return; // 개인 상점 상품 리스트
	case pTrade::TamerShopBuy		: RecvTamerShopBuy();			return;	// 개인 상점 아이템 구매 결과
	case pTrade::TamerShopClose		: RecvTamerShopCloseResult();	return; // 테이머 상점 닫기 결과
	case pTrade::TamerShopSell		: RecvTamerShopSellResult();	return; // 개인상점 아이템 판매 결과

//////////////////////////////////////////////////////////////////////////////////////////
// 위탁 상점
	case pTrade::ShopList			: RecvCommissionShopItemListResult();		return;
	case pTrade::ShopOpen			: RecvCommissionShopOpenResult();			return;
	case pTrade::ShopWithdraw		: RecvCommissionShopItemWithDrawResult();	return;
	case pTrade::ShopNPCItemList	: RecvCommissionShopNpcItemList();			return;
	case pTrade::ShopSell			: RecvCommissionShopSellResult();			return;
	case pTrade::ShopBuy			: RecvCommissionShopBuyResult();			return;
	case pTrade::ShopClose			: RecvCommissionShopCloseResult();			return;


//////////////////////////////////////////////////////////////////////////////////////////
// 부화 보정
	//case pItem::HatchOptionAdd		: RecvHatchOptionItemAddResult();			return;


	case pGame::DeletePartner		: RecvDeletePartner();			return;	// 파트너몬 제거 성공 또는 실패

	case pTamer::Change				: RecvChangeTamer();			return;	// 테이머 영구 변경
	case pMaster::PartnerScale		: RecvChangePartnerScale();		return;	// 현재 파트너몬의 크기를 임시 변경한다.

	case pSvr::SelectPortal			: RecvSelectPortalSuccess();	return;
	case pSvr::SelectPortalFailure	: RecvSelectPortalFailure();	return;
	case pSvr::Change				: RecvChangeServer();			return;
	case pSvr::LocalPortal			: RecvLocalPortalSuccess();		return;
	case pInsMap::DungeonFail		: RecvInsMap_DungeonFail();		return;
	case pItem::CropFailure			: RecvCropFailure();			return;

	case pGame::Ting				: RecvTing();					return;
	case pGame::Attack				: RecvAttack( false ); 			return;
	case pGame::AttackMiss			: RecvAttackMiss();				return;
	case pGame::TargetInfo			: RecvTargetInfo();				return;
	case pGame::Die					: RecvAttack( true );			return;
	case pGame::SkillDie			: RecvSkillDie();				return;
	case pGame::GetExp				: RecvGetExp();					return;
	case pGame::LevelUp				: RecvLevelUp();				return;

	case pGame::HealUp				: RecvHealUp();					return;
	case pGame::HpRate				: RecvHpRate();					return;
//	case pGame::FsChange			: RecvFsChange();				return;

	//case pItem::Buy					: RecvItemBuy();				return;
	case pItem::Sell				: RecvItemSell();				return;
	case pItem::Crop				: RecvItemCropSuccess();		return;
	case pItem::Analysis			: RecvItemAnalysis();			return;

#ifdef CHAT_BAN
	case pMaster::ChatBanUser		: RecvChatBanUser();				return;
#endif
	case pMaster::BanUser			: RecvBanUser();				return;
	case pMaster::DiscUser			: RecvDiscUser();				return;
	case pMaster::InitSkillStat		: RecvInitSkillStat();			return;
	case pDigimon::InitSkillPoint	: RecvInitDigimonSkillPoint();	return;

	case pMaster::SetMoveSpeed		: RecvSetMoveSpeed();			return;
	case pMaster::CreateMonsterF	: RecvCreateMonsterFailure();	return;

	case pGame::SyncCondition		: RecvSyncCondition();			return;
	case pGame::Skill				: RecvSkill();					return;

	case pSkill::Apply				: RecvSkillApply();				return;
	case pSkill::ApplyAround		: RecvSkillApplyAround();		return;
	case pSkill::Repeat				: RecvSkillRepeat();			return;
	case pSkill::PointUp			: RecvSkillPointUp();			return;
	case pSkill::Error				: RecvSkillError();				return;	
	case pSkill::AroundSkill		: RecvWideSkillApply();			return;
	case pMonster::MsgRaidRankingList : RecvRaidRankingList();		return;
	case pSkill::AroundSkillAni		: RecvAroundSkillAni();			return;
	case pSkill::AroundSkillDmg		: RecvAroundSkillDmg();			return;

	case pGameTwo::SkillAction		: RecvSkillAction();			return;
	case pGameTwo::SkillHit			: RecvSkillHit();				return;
	case pGameTwo::SkillHitEffect		: RecvRaidChainSkill();		return;

	case pSkill::RangeSkillDmg		: _RecvDigimonSkill_ApplyAround();	return;

	case pTamer::SkillSuccess		: RecvTamerSkillSuccess();		return;
	case pSkill::Summon				: RecvTamerSummon();			return;

	case pTamer::ItemSkillApply		: RecvTamerCashSkillUseResult();return;
	case pTamer::ItemSkillExpire	: RecvTamerCashSkillExpire();	return;
	case pTamer::ItemSkillRemove	: RecvTamerCashSkillRemove();	return;

	case pBuff::AbnormalDamage		: RecvDotSkillSuccess();		return;
	case pBuff::BuffSkill			: RecvBuffSkill();				return;

	case pSkill::ActiveSkillEff		: RecvActiveSkill();			return;

	case pBuff::Stun				: RecvStunSkillSuccess();		return;

	case pSkill::AddSkillChip		: RecvDigimonAddSkillSuccess();	return;		// 스킬 등록 성공
	case pSkill::RemoveSkillChip	: RecvDigimonSkillRemove();		return;		// 스킬 삭제
	case pSkill::ChipCoolTime		: RecvDigimonMemorySkill_UseSuccess();	return;		// 스킬 사용 성공
	case pSkill::ResultSkillChip	: RecvMemorySkill_EffectSync();	return;		// 스킬 이펙트 싱크

	case pSkill::MonsterSkillEffect	: RecvMonsterSkill_Use(0);		return;		// 몬스터 스킬 사용
	case pSkill::Range_PersistEnd	: RecvMonsterSkillSync_PersistEnd();	return;	// 바닥 스킬 만료

	case pGame::Evolution			: RecvEvolution();				return;
	case pGame::EvoFailure			: RecvEvoFailure();				return;
	case pGame::ChangeStatValue		: RecvChangeStatValue();		return;

	case pGame::AllStat				: RecvAllStat();				return;
#ifdef UI_ITEMSCAN_RENEWAL
	case pItem::ItemScan			: RecvDigitamaScanResult();		return;
#else
	case pItem::DigitamaToItem		: RecvDigitamaScanResult();		return;
#endif
	case pItem::CapsuleToItem		: RecvCapsuleToItem();			return;

	case pItem::SocketIn			: RecvSocketInSuccess();		return;
	case pItem::SocketOut			: RecvSocketOutSuccess();		return;
	case pItem::SocketClear			: RecvSocketClearSuccess();		return;

	case pQuest::ChangeInfo			: RecvQuestChangeInfo();		return;
	case pQuest::DailyUpdate		: RecvUpdateDailyQuest();		return;
	case pQuest::Canceled			: RecvQuestCancel();			return;

	// game master
	case pMaster::UserInfo			: RecvUserInfo();				return;
	case pMaster::UserList			: RecvUserList();				return;
	case pMaster::UserInfoNotFound	: RecvUserInfoNotFound();		return;
	case pMaster::ConnCountInfo		: RecvConnCountInfo();			return;
	case pMaster::InsertItemToShop	: RecvInsertItemToShop();		return;
	case pMaster::Init2ndPass		: RecvInit2ndPass();			return;
	case pMaster::CheatLevel		: RecvCheatLevel();				return;
	case pMaster::LieKillMonster	: RecvKillMonster();			return;
	case pMaster::MasterConn		: RecvMasterConnCount();		return;
	case pMaster::MasterPayCash		: RecvMasterPayCash();			return;

	case pMaster::GMHatchTry		: RecvHatchTry();				return;
#ifdef CHEAT_ITEMRANK
	case pMaster::RankItemRun		: RecvItemRank();				return;		//랭크아이템  돌리기 치트
#endif

	case pMgr::PlayTimeSet			: RecvPlayTime();				return;

	// debug info
	case pDebug::SyncInfo			: RecvSyncInfo();				return;
	case pDebug::LoseTarget			: RecvLoseTarget();				return;
	case pDebug::InsufficientRange	: RecvInsufficientRange();		return;

	case pTamer::EquipItem			: RecvEquipItem();				return;
	case pTamer::Option				: RecvTamerOption();			return;
	case pSync::Achievement			: RecvTamerAchievement();		return;

	case pGame::BattleOn			: RecvBattleOn();				return;
	case pGame::BattleOff			: RecvBattleOff();				return;

	case pItem::Use					: RecvItemUseSuccess();			return;
	case pItem::UseFailure			: RecvItemUseFailure();			return;

	case pItem::Move						: RecvItemMoveSuccess();				return;
	case pItem::MoveFailure					: RecvItemMoveFailure();				return;
	case pItem::ChangeLimitedAttribute		: RecvChangeItemLimitedAttribute();		return;		

	case pGame::HatchIn				: RecvHatchIn();				return;
	case pGame::HatchUp				: RecvHatchUpSuccess();			return;
	case pGame::HatchFailure		: RecvHatchUpFailure();			return;
	case pGame::HatchOut			: RecvHatchOut();				return;

#ifdef CROSSWARS_SYSTEM
	case pItem::CrossWarsUse		: RecvCrossItemUseSuccess();	return;
	case pItem::CrossWarsUseFailure	: RecvCrossItemUseFailure();	return;
	case pItem::CrossInvenSlotSize	: RecvCrossInvenSlotSize();		return;
	case pDigimon::CrossWarsHatch	: RecvCrossWarsHatch();			return;
	case pDigimon::CrossWarsEnergize: RecvCrossWarsEnergize();		return;
	case pDigimon::CrossWarsNotReg	: RecvCrossNotReg();			return;
	case pDigimon::CrossWarsRecover : RecvCrossWarsRecover();		return;
	case pDigimon::CrossWarsLoader	: RecvCrossWarsLoader();		return;
	case pDigimon::CrossWarsCrownHeal:RecvCrossWarsCrownHeal();		return;
	case pDigimon::CrossWarsCombine	: RecvCrossChangePartner2();	return;
	case pDigimon::CrossWarsChangeOne:RecvCrossChangePartner();		return;
#endif

	case pItem::HatchBackupDiskAdd	: RecvHatchBackupDisk();		return;
	case pItem::HatchBackupDiskCancel:RecvHatchBackupDiskCancel();	return;

	case pGame::ChangePartner		: RecvChangePartner();			return;
	case pDigimon::BattleTagSuccess	: RecvBattleTagSuccess();		return;
	case pDigimon::BattleTagFailure	: RecvBattleTagFailure();		return;

	case pMaster::SetMoney			: RecvSetMoney();				return;
	case pItem::InvenSlotSize		: RecvInvenSlotSize();			return;
	case pItem::WarehouseSlotSize	: RecvWarehouseSlotSize();		return;

	case pGame::MsgShout			: RecvMsgShoutFailure();		return;
	case pTamer::ChangeName			: RecvChangeTamerName();		return;
	case pGame::QueryMoneyInven		: RecvQueryMoneyInven();		return;
	case pItem::ItemSort			: RecvItemSort();				return;
	case pGameTwo::InventoryInfo	: RecvInvenResult();			return;

/////////////////////////////// buddy system //////////////////////////////////
	case pBuddy::List				: RecvBuddyList();				return;
	case pBuddy::On					: RecvBuddyOn();				return;
	case pBuddy::Off				: RecvBuddyOff();				return;
	case pBuddy::Add				: RecvBuddyAdd();				return;
	case pBuddy::Ban				: RecvBuddyBan();				return;
	case pBuddy::NotFound			: RecvBuddyNotFound();			return;
	case pBuddy::ChangeName			: RecvBuddyChangeName();		return;

/////////////////////////////// guild system //////////////////////////////////
	case pGuild::Create				: RecvGuildCreate();			return;	// 길드 생성 성공 여부
	case pGuild::Close				: RecvGuildClose();				return;	// 길드 파괴

	case pGuild::Info				: RecvGuildInfo();				return;
	case pGuild::ChangeNotice		: RecvGuildChangeNotice();		return;

	case pGuild::Reject				: RecvGuildReject();			return;
	case pGuild::AllowFailure		: RecvGuildAllowFailure();		return;

	case pGuild::On					: RecvGuildOn();				return;
	case pGuild::Off				: RecvGuildOff();				return;

	case pGuild::Add				: RecvGuildAdd();				return;
	case pGuild::ReqAdd				: RecvGuildReqAdd();			return;
	case pGuild::ReqAddFailure		: RecvGuildReqAddFailure();		return;

	case pGuild::Chat				: RecvGuildChat();				return;

	case pGuild::RankChange			: RecvGuildRankChange();		return;

	case pGuild::Leave				: RecvGuildLeave();				return;
	case pGuild::Delete				: RecvGuildDeleted();			return;

	case pGuild::ToMaster			: RecvGuildToMaster();			return;
	case pGuild::ToSubMaster		: RecvGuildToSubMaster();		return;
	case pGuild::ToMember			: RecvGuildToMember();			return;
	case pGuild::ToSubMember		: RecvGuildToSubMember();		return;
	case pGuild::ToDatsMember		: RecvGuildToDatsMember();		return;

	case pGuild::DeletedTamer		: RecvGuildDeletedTamer();		return;
	case pGuild::ChangeTName		: RecvGuildChangeTamerName();	return;
	case pGuild::LevelUp			: RecvGuildLevelUp();			return;
	case pGuild::MemberLevelUp		: RecvGuildMemberLevelUp();		return;
	case pGuild::History			: RecvGuildHistory();			return;
	case pGuild::ChangeClassName	: RecvGuildChangeClassName();	return;
	case pGuild::IncMember			: RecvGuildIncMember();			return;

	case pSession::Message			: RecvUserMessage();			return;
	case pTamer::Message			: RecvTamerMessage();			return;
	case pTamer::MessageConfirm		: RecvTamerMessageConfirm();	return;

#ifdef GUILD_RENEWAL
	case pGuild::GSPLevelUp			: RecvGuildSkillPoint();		return;
	case pMaster::GMGuildlv			: RecvGuildLevelUp();			return;
	case pGuild::GSkill				: RecvUseGuildSkill();			return;
	case pGuild::GSkillRemove		: RecvRemoveUseGuildSkill();	return;
	case pGuild::Summon				: RecvGuildSkill_Summon();		return;
	case pGuild::SkillError			: RecvGuildSkillError();		return;
	case pGuild::ChangeMemo			: RecvGuildChangeMemo();		return;		
#endif
/////////////////////////////// party system //////////////////////////////////
	case pParty::Created			: RecvPartyCreated();			return;
	case pParty::Invite				: RecvPartyInvite();			return;
	case pParty::InviteResult		: RecvPartyInviteResult();		return;

	case pParty::Exp				: RecvPartyExp();				return;
	case pParty::ChangeCropType		: RecvPartyChangeCropType();	return;

	case pParty::Chat				: RecvPartyChat();				return;
	case pParty::Join				: RecvPartyJoin();				return;
	case pParty::Kick				: RecvPartyKick();				return;
	case pParty::Leave				: RecvPartyLeave();				return;

	case pParty::MemberList			: RecvPartyMemberList();		return;

	case pParty::Die				: RecvPartyDie();				return;
	case pParty::Info				: RecvPartySyncInfo();			return;
	case pParty::Position			: RecvPartySyncPosition();		return;

	case pParty::ChangeMap			: RecvPartyChangeMap();			return;
	case pParty::ChangeMaster		: RecvPartyChangeMaster();		return;
	case pParty::ChangeDigimon		: RecvPartyChangeDigimon();		return;
	case pParty::Disconnected		: RecvPartyDisconnected();		return;

	case pParty::SyncPartyBuff		: RecvPartySyncBuff();			return;		
/////////////////////////////// 용병 보관소 processing ////////////////////////
	case pDigimon::CareSlotSize		: RecvDigimonCareSlotSize();	return;
	case pDigimon::CareSlotList		: RecvDigimonCareSlotList();	return;


/////////////////////////////// item processing ///////////////////////////////
	case pItem::Add					: RecvItemAdd();				return;
	case pItem::Update				: RecvItemUpdate();				return;
	case pItem::Delete				: RecvItemDelete();				return;

	case pItem::Merge				: RecvItemMerge();				return;
	case pItem::Split				: RecvItemSplit();				return;
//	case pItem::Move				: RecvItemMove();				return;
	case pItem::Exchange			: RecvItemExchange();			return;

	case pItem::CashShop			: RecvCashShop();				return;
	case pItem::CropCashItem		: RecvCropCashItemSuccess();	return;
	case pCashShop::BuyRequest		: RecvBuyCashItemRequest_Steam();return;
	//case pCashShop::Buy				: RecvBuyCashItem();			return;
	case pCashShop::MultiBuy		: RecvBuyCashItem();			return;
	case pCashShop::Gift			: RecvGiftCashItem();			return;
	case pCashShop::Cart			: RecvCashShopCart();			return;
	case pCashShop::Balance			: RecvCashShopBalance();		return;
	case pCashShop::BuyHistory		: RecvCashshopBuyhistory();		return;

	case pItem::GiftShop			: RecvGiftShop();				return;
	case pItem::CropGiftItem		: RecvCropGiftItemSuccess();	return;

	case pItem::Timeout				: RecvItemTimeout();			return;
	case pItem::RaidItemReward		: RecvRaidItemReward();			return;
/////////////////////////////// event processing /////////////////////////////
	case pGame::ChangeState			: RecvChangeGameState();		return;

	case pEvent::DailyCheck			: RecvEventDailyCheck();		return;
	case pEvent::DailyEventInfo		: RecvDailyEventInfo();			return;		// 매일 이벤트 정보
	case pEvent::MonthlyAttendEventInfo: RecvMonthlyEventInfo();	return;		// 2017-02-16-nova 월간 이벤트 정보
	case pEvent::Attendance			: RecvAttendance();				return;

	case pGame::Event				: RecvGameEvent();				return;

	case pEvent::HotTimeEvent		: RecvNtfHotTimeEvent();		return;
	case pEvent::HotTimeItemRequest	: RecvHotTimeGetItemResult();	return;

	case pItem::PCbangInven			: RecvPCBangInven();			return;
	case pItem::CropPCBangItem		: RecvPCBangItemCropResult();	return;

	case pSvr::PrimiumStart			: RecvPreminum();				return;		// PC방 프리미엄 서비스 시작
	case pSvr::TimeExpire			: RecvPreminumEnd();			return;		// PC방 만료로 인한 일반 사용자로 전환
	case pParty::PCBangExpBuff		: RecvPartyPCBangExpBuff();		return;		// 파티에 PC방 멤버가 있을 경우 추가 경험치률 수신	

	case pGame::DigimonInchant		: RecvDigimonInchantResult();	return;		// 디지몬 강화 결과
	case pItem::PortableItemFail	: RecvPortableItemFail();		return;		// 포터블 아이템 사용 실패
	case pDigimon::MoveTo			: RecvDigimonMoveResult();		return;		// 디지몬 이동 결과
	case pDigimon::CareDigimonMove: RecvDigimonMoveResultInArchive(); return; //디지몬 이동 결과(보관함내에서)

	case pGame::PlayTimePaneltyChange:RecvPlayTimePaneltyChange();	return;	// 플레이 타임 패널티 변경

////////////////////////////////////////////////////////////////////////////
// 신규 버프
////////////////////////////////////////////////////////////////////////////
	case pBuff::CreateBuff			: RecvBuffCreate();				return;
	case pBuff::BuffClear			: RecvBuffClear();				return;	
	case pGame::ChangeStat			: RecvStatChange();				return;
	case pBuff::BuffChange			: RecvBuffChange();				return;
	case pBuff::ReturnFail			: RecvBuffApplyFail();			return;
	//case pBuff::ManualRemove		: RecvBuffApplyFail();			return;
	case pMaster::GMQuest			: RecvGmQuest();				return;
	case pMaster::GMCoolReSet		: RecvGMCoolReSet();			return;
	case pMaster::GMGuildSkillReset	: RecvGuildSkillReSet();		return;		//2017-06-28-nova
#ifdef MONSTER_SKILL_GROWTH
	case pSkill::MonsterBuff		: RecvMonBuffSet();				return;		//	몬스터 성장 스킬 셋
#endif

////////////////////////////////////////////////////////////////////////////
		// 명수 - 버프 연결이 안되어 있어서 일단은 여기다 넣어놨습니다.
	case pTamer::BuffInfo			: RecvTamerBuffInfo();			return;
	case pTamer::BuffStart			: RecvTamerBuffStart();			return;
	case pTamer::BuffEnd			: RecvTamerBuffEnd();			return;

	case pTamer::BuffActive			: RecvTamerBuffActive();		return;
	case pTamer::BuffInactive		: RecvTamerBuffInactive();		return;

	case pTamer::RideOn				: RecvRideOn();					return;
	case pTamer::RideOff			: RecvRideOff();				return;
	case pGame::DecreaseDigimonInchant:RecvDecreaseDigimonInchantResult(); return;

//////////////////////////////////////////////////////////////////////////
// Battle Match
	case pDuel::Info				: RecvMyBattleMatchInfo();		return;		// 내 배틀 정보를 받는다.
	case pDuel::MatchRegister		: RecvBattleRegistRequest();	return;		// 배틀 등록 결과를 받는다.
	case pDuel::MatchRegiCancel		: RecvBattleRegistCancel();		return;		// 배틀 등록 취소 결과를 받는다.
	case pDuel::TeamReadyInfo		: NtfBattleRegistReadyInfo();	return;		// 배틀 준비 체크 데이터를 받는다.
	case pDuel::Ready				: RecvBattleRegistReadyResult();return;		// 배틀 등록 준비 상태 결과를 받는다.
	case pDuel::ReadyResultNoti		: NtfBattleRegistReadyResult();	return;		// 다른 유저의 준비 상태 결과를 받는다.
	case pDuel::RemoveDuel			: NtfBattleMatchCancel();		return;		// 배틀 매칭 후 입장 수락/취소시 다른 유저가 취소 했을 때 날라오는 패킷
	case pDuel::Start				: NtfBattleStart();				return;
	case pDuel::End					: NtfBattleEnd();				return;
	case pDuel::Reward				: NtfBattleReward();			return;
	case pDuel::MemberSync			: NtfBattleMemberSync();		return;
	case pDuel::MatchSync			: NtfBattleMatchSync();			return;
	case pDuel::ReadyComplete		: NtfBattleMatchComplete();		return;
	case pGame::BattleEvolPoint		: RecvBattleEvoPoint();			return;	// 기존 패킷
//////////////////////////////////////////////////////////////////////////

	case pGame::DotDie				: RecvDotDie();					return;

////////////////////////////////////////////////////////////////////////////
// 친구 초대 이벤트
////////////////////////////////////////////////////////////////////////////
	case pEvent::AddFriendly		: RecvAddFriendly();			return;
	case pEvent::AddFriendlyAnswer	: RecvAddFriendlyAnswer();		return;
	case pEvent::FriendlyMark		: RecvFriendlyMark();			return;
	case pEvent::FriendlyMarkItem	: RecvFriendlyMarkItem();		return;
	case pEvent::FriendlyStateChange: RecvFriendlyStateChange();	return;
	case pEvent::FriendlyInfo		: RecvFriendlyInfo();			return;
	case pEvent::FriendlyNameChange	: RecvFriendlyNameChange();		return;

	case pEvent::MastersMatchingOpen: RecvMastersMatchOpen();		return;
	case pEvent::MastersMatching	: RecvMastersMatchInsert();		return;
	case pItem::CropGiftItemFail	: RecvCropGiftInvenFail();		return;

	case pItem::ScanItemUse			: RecvGiftBoxUseSuccess();		return;

	// 카드 이벤트 시작 시 서버에서 데이터를 받음
	case pMiniEvent::PiemonGameStart: RecvCardEventStart();			return;
	case pMiniEvent::PiemonGameEnd	: RecvCardEventEnd();			return;

	case pMonster::NatualExp		: RecvNatureExp();				return;
	// 오른쪽 버튼으로 일부 알(이벤트) 부화되는 기능
	case pItem::DirectHatch			: RecvRClickDigimonHatch();		return;

#ifdef MINIGAME
	case pMiniGame::MiniGameStart	: RecvMinigameStart();			return;
	case pMiniGame::MiniGameClickBtn: RecvMinigameClickBtn();		return;
	case pMiniGame::MiniGameTimeOut	: RecvMinigameTimeOut();		return;
	case pMiniGame::MiniGameEnd		: RecvMinigameEnd();			return;
	case pMiniGame::MiniGameLimit		: RecvMinigameLimit();			return;

#endif
	case pItem::GotchaOpen			: RecvGotchaOpen();				return;
	case pItem::GotchaExecute		: RecvGotchaGet();				return;
 	case pItem::GotchaError			: RecvGotchaError();			return;
	//case pItem::MsgAllGotchaMachine	: RecvMsgAllGotchaMachine();	return;
	case pItem::MysteryMachine		: RecvMysteryGet();				return;

	case pMaster::MachineRun		: RecvRunGotchaMachine();		return;	//머신 돌리기 치트

#ifdef ADVENTURE_TAMER_CHANGE
	case pItem::MoveClear			: RecvMoveClear();				return;
	case pTamer::ChangeAround		: RecvChangeTamerAround();		return;
#endif

	case pItem::FireWork			: RecvFireWork();				return;

	case pDigimon::DigimonInchantInfo:RecvSyncEnchantEffect();		return;

	case pGameTwo::RecompenseList	: RecvRequestRewardList();		return;
	case pGameTwo::RecompenseGain	: RecvRequestRewards();			return;
	case pGameTwo::RecompenseGainFail:RecvRequestRewardFail();		return;
	case pGameTwo::RecompenseChange	:						 		return;

	case pItem::AccessoryCheck		: RecvEnchantItemIdentify();			return;
	case pItem::AccStoneUse			: RecvEnchantItemEnchant();		return;

	case pDigimon::IncubaterList	: RecvIncubaterList();			return;
	case pDigimon::Facilitation		: RecvIncubatorFacilitation();	return;		// 촉진
 	case pDigimon::IncubaterError	: RecvIncubatorError();			return;		// 에러
	case pDigimon::Extraction		: RecvIncubatorExtraction();	return;		// 추출( 꺼내기 )
	case pDigimon::IncubaterSlotChange:RecvIncubaterSlotChange();	return;		// 촉진 중인 인큐베이터 슬롯에 디지몬 이동
#ifdef REPURCHASE
	case pItem::ItemRepurchaseOpen	: RecvItemRepurchaseOpen();		return;		// 재구매 창 오픈
	case pItem::ItemRepurchase		: RecvItemRepurchase();			return;		// 재구매 시도
#endif

	case pDigimon::DigimonBookInfo	: RecvEncyclopediaOpen();		return;		// 디지몬 도감 목록 요청(최초 1회만)
	case pDigimon::DigimonBookItem	: RecvEncyclopediaGetItem();	return;		// 디지몬 도감 완료 보상 아이템 요청
	case pDigimon::DigimonBookDeckEff:RecvEncyclopediaBuff();		return;		// 일정 시간동안 적용되는 발동형 버프 정보
	case pDigimon::DigimonBookDeckUse:RecvEncyclopediaGetBuffStat();return;		// 버프로 인해 스탯
	case pGameTwo::AllMsg			: RecvGlobalMsg();				return;		// 서버의 모든 유저에게 메세지 보여줌
	case pDigimon::CapsuleEvoSlotResult:RecvCapsuleEvol_Result();	return;		// 캡슐진화 시도 결과

	case pDigimon::SpiritToDigimon			:	RecvSpirit_SpritToDigimon();	return;	// 스피릿아이템 -> 하이브리드디지몬
	case pDigimon::DigimonToSpirit			:	RecvSpirit_DigimonToSpirit();	return;	// 하이브리드디지몬 -> 스피릿아이템
	case pDigimon::AncientSpiritEvolution	:	RecvSpirit_AncientSpiritEvolution();	return;	// 카이젤그레이몬 + 매그너가루루몬 -> 스사노오몬
	case pGameTwo::DoorObjectOpen	: RecvActorObjectFactor();		return;		// Actor 오브젝트( 문 ) 개폐를 위해 현재 맵의 몬스터 죽을 시 수신
	case pEvent::PinochiGetInfo		: RecvPinokimonInfoReq();		return;		// 피노키몬 카드게임 Npc Open / 갱신 시간에 목록 갱신 요청에 대한 응답
	case pEvent::PinochiVote		: RecvPinokimonVote();			return;		// 투표 정상 완료	( 서버 응답 대기 해제 )
	case pEvent::PinochiItemDelete	: RecvRemoveVoteItem();			return;		// 투표권 아이템 리셋
	case pDigimon::DigimonDataChange: RecvDigimonDataChange();		return;
	case pGameTwo::ErrorMessage		: RecvErrorMessage();			return;
	case pItem::CashItemTradeTimeOut:	RecvTradeLimitedTimeOut();	return;

#ifdef ITEM_USE_TIME_PASS
	case pItem::UseActive: RecvItemActive();	return;	//2017-04-04-nova
#endif	
	case pItem::RelocateTamer:				Recv_ServerRelocate_ItemUse();	return;
	case pRelocateTamer::CheckTamerName:	Recv_ServerRelocate_CheckTamerName();	return;


	case pInsMap::ChallengeStageClear:		Recv_InfiniteWar_ChallengeStageClear();			return;
	case pInsMap::ChallengeTryNextStage:	Recv_InfiniteWar_ChallengeTryNextStage();		return;
	case pInsMap::ChallengeGiveup:			Recv_InfiniteWar_ChallengeGiveup();				return;
	case pInsMap::ChallengePartyAlreadyPlay:Recv_InfiniteWar_ChallengePartyAlreadyPlay();	return;
	//case pInsMap::ChallengeStageTimeSave:	Recv_InfiniteWar_ChallengeStageClear();			return;
	case pInsMap::GetChallengeDailyPoint:	Recv_ChallengeData();				return;
	case pInsMap::PutChallengeDailyPoint:	Recv_ChallengeData_PointItem_Put();	return;
	case pInsMap::ChallengeDailyPointUpdate:Ntf_ChallengeDailyUpdate();			return;

	case pGameTwo::ArenaRequestRank:	Recv_ArenaRequestRank();	return;
	case pGameTwo::ArenaRequestOldRank:	Recv_ArenaRequestOldRank(); return;
	case pGameTwo::ArenaRequestFame:	Recv_ArenaRequestFame();	return;
	case pGameTwo::ArenaRequestFameList:Recv_ArenaRequestFameSeasonList();return;

	case pGameTwo::XGToCrystal:		Recv_XGToCrystal();		return;//X게이지 -> 크리스탈
	case pGameTwo::CrystalToXG:		Recv_CrystalToXG();		return;//크리스탈-> X게이지
	case pGameTwo::XGuageUpdate:	Recv_XGuageUpdate();	return;//X게이지 양 갱신	
	case pGameTwo::XGuageMaxUpdate:	Recv_XGuageMaxUpdate();	return;//X게이지/크리스탈 MAX 양 갱신	
	case pGameTwo::NeonDisplay:		NTF_NeonDisplay();		return; // 전광판 데이터 받음.
	case pCashShop::VIPAutoPayment: RecvVipMemberShipData();return;
	case pDigimon::DigimonSkillLimitOpen:	RecvSkillMaxLvUp(); return;
	case pDigimon::UpdateDigimonSkillCoolTime:NTF_DigimonSkillCoolTime(); return;

#if COMMON_LIB_FIXED
	case pDigimon::SkillLevelUp:	RecvDigimonSkillLevelUp();	return;
#endif

	case pGameTwo::DigimonTranscendenceChargeEXP:	RecvDigimonTranscendenceChargeExp();	return;
	case pGameTwo::DigimonTranscendence:		RecvDigimonTranscendence(); return;

#if COMMON_LIB_FIXED
		// Time Charge
	case pGameTwo::TimeChargeItemList:		RecvChargeListResult();			return;
	case pGameTwo::TimeChargeNormal:		RecvNormalChargeItemResult();	return;
	case pGameTwo::TimeChargeImmediate:		RecvQuickChargeItemResult();	return;
	case pGameTwo::TimeChargeCancel:		RecvChargeCancelResult();		return;
	case pGameTwo::TimeChargeWithdraw:		RecvChargeItemWithdrawResult();	return;
	case pGameTwo::TimeChargeSlotOpen:		RecvUnlockChargeListResult();	return;

		// Daily Check Event
	case pEvent::DailyCheckEvent:			RecvDailyCheckEventResult();		return;
	case pEvent::DailyCheckEventRequest:	RecvDailyCheckEventItemGetResult();	return;

	case pGameTwo::QuestRenew:				RecvQuestAllInfo();				return;
		// 디지몬 이팩트 변경
	case pDigimon::ChangeDigimonEffect:		RecvChangeDigimonEffect();		return;
	case pDigimon::SyncDigimonEffect:		NTF_DigimonEffectChange();		return;
#endif
	}
	
#ifdef CRASHDUMP_0326
	// 미확인된 프로토콜 발 생 시 로그 남김		lks007
	DUMPLOG( "L0003 - unknown protocol : %d", Protocol );
#endif
	xstop1(false, "unknown protocol(%d)", Protocol);
}
