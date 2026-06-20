
#pragma once

#include "nlib/cClient.h"
//#include "cNetwork.h"

#include "common_vs2019/pDebug.h"
#include "common_vs2019/pType.h"
#include "common_vs2019/pSync.h"
#include "common_vs2019/pLength.h"
#include "common_vs2019/pTrade.h"


class CPlayer;
class CDigimon;
class CMonster;

class cCliGame : public cClient
{
public:
	cCliGame(void);
   ~cCliGame(void);

public:
	bool start(void);
	void send(void);

public:
	void OnConnected(void);
	void OnExecute(void);
	void OnDisconnected(char *msg);

public:
	char ip[Language::pLength::ip];
	uint port;
	uint m_nConnectedTickCount;
	uint m_nIsChangeServer;

	TCHAR m_szTamerName[Language::pLength::name+1];

	CsC_AvObject*	m_pTargetObject;

public:
	bool m_bPortalProcessing;
	bool m_bPortalRequesting;
	bool m_bIsReadyCompleted;
/////////////////////////////////////////////////////////
// receive
private:
	void RecvVersionError(void);
	void RecvStartMessage(void);

	void RecvInitGameData(void);
	void RecvBossGenInfo(void);

	void RecvLocalPortalSuccess(void);
	void RecvSelectPortalSuccess(void);
	void RecvSelectPortalFailure(void);
	void RecvChangeServer(void);	// 포탈 이용시, 다음 서버에 접속해도 됨을 수신

	void RecvTing(void);			// 선공몹이 나를 발견한 경우
	void RecvSyncData(void);
	void RecvCropFailure(void);

	void RecvGameEvent(void);		// 폭축 등
	void RecvAttendance(void);		// 출석 인정 + 아이템 지급 내역

	void RecvNtfHotTimeEvent(void);
	void RecvHotTimeGetItemResult(void);

	void RecvAdvertBossCount(void);
	void RecvAdvertBossDie(void);
	void RecvAdvertBossCreate(void);

	void RecvAdvertBurstMode(void);

	void RecvItemTimeout(void);
	void RecvItemBuy(void);
	void RecvItemSell(void);
	void RecvItemCropSuccess(void);
	void RecvCropMoney(void);
	void RecvItemCropParty(void);

	void RecvItemUseSuccess(void);
	void RecvItemUseFailure(void);
#ifdef CROSSWARS_SYSTEM
	void RecvCrossItemUseSuccess(void);
	void RecvCrossItemUseFailure(void);
	void RecvCrossInvenSlotSize(void);
#endif
	void RecvItemMoveSuccess(void);
	void RecvItemMoveFailure(void);
	void RecvChangeItemLimitedAttribute(void);
	void RecvRaidItemReward(void);
	void RecvAttack( bool bDie );
	void RecvAttackMiss(void);
	void RecvTargetInfo(void);

	void RecvSkillDie(void);

	void RecvGetExp(void);
	void RecvLevelUp(void);

	void RecvHealUp(void);
//	void RecvFsChange(void);	// 친밀도 변경

	void RecvCreateMonsterFailure(void);
	void RecvSetMoveSpeed(void);

	void RecvAllStat(void);
	void RecvEquipItem(void);

	void RecvHpRate(void); // 자기 자신에게는 전송되지 아니함, 주변에만 전송됨
	void SetHpRate(u4 nUID, u1 nHpRate);

	void RecvDiedAlready(void);
	void RecvItemBuySuccess(void);
	
	void RecvItemMake(void);

	void RecvItemSellSuccess(void);
	void RecvDigitamaSellResult(void);

	void RecvUserMessage(void);
	void RecvTamerMessage(void);

public:	// Attendance
	void SendAttendance(void);		// 출석 체크

public: // card scanner
	void SendUserMessageConfirm(void);
	void SendTamerMessageConfirm(void);
	void RecvTamerMessageConfirm(void);

	void SendScannerRegister(wchar* szCode);
	void SendScannerRelease(wchar* szCode);

	void RecvScannerRegister(void);
	void RecvScannerRelease(void);
	void RecvHandshake487(void);
	bool SendScannerEquip(wchar* szCode);	// 스캐너 장착시
	void SendScannerOn(wchar* szCode);		// 스캐너 장착중인 상태로 맵이동 완료시
	void SendScannerOff(void);

	void RecvScannerEquip(void);
	void RecvScannerOn(void);
	void RecvScannerOff(void);

	void RecvCardScan(void);
	void SendCardScan(wchar* szCode, nSync::Pos &pos);

	void UpdateScannerState(n4 nScannerState);
	void RecvScannerError(void);

	void Send_ItemMake(	DWORD const& dwNpcIDX, // 제작 NPC IDX
						int const& nMakeCount,			// 제작 횟수
						int const& nMakeItemIDX,		// 제작할 아이템 IDX
						DWORD const& dwRateItemIDX,			// 확률 보조 아이템 IDX
						DWORD const& dwProtectItemIDX);		// 보호 아이템  IDX

	void SendHotTimeGetItemRequest(int const& nTableIDX);

public: // item
	void RecvItemAdd(void);			// 아이템 추가
	void RecvItemUpdate(void);		// 아이템 정보 수정
	void RecvItemDelete(void);		// 아이템 제거

	void RecvItemSplit(void);		// 아이템 분리
	void RecvItemMerge(void);		// 아이템 합하기
	void RecvItemMove(void);		// 아이템 이동
	void RecvItemExchange(void);	// 아이템 교체

public: // skill
	void RecvSkill(void);
	void RecvSkillApply(void);
	void RecvSkillApplyAround(void);
	void RecvWideSkillApply(void);
	void RecvAroundSkillAni(void);
	void RecvAroundSkillDmg(void);
	void _RecvWideSkillApplyAround_ExistHitter(void);

	void RecvSkillAction(void);
	void RecvSkillHit(void);

	void _RecvSkillApplyAround_ExistHitter(void);
	void _RecvSkillApplyAround_NoneHitter(void);
	void _RecvSkillApplyME_NoneHitter(void);

	// 디지몬 광역 스킬
	void _RecvDigimonSkill_ApplyAround(void);

	void RecvSkillRepeat(void);
	void RecvSkillPointUp(void); // 스킬 포인트 레벨업, +2 스킬 포인트 획득

	void RecvSkillError(void);	// Skill error

	void RecvTamerBuffStart(void);
	void RecvTamerBuffEnd(void);
	void RecvTamerBuffInfo(void);

	void RecvTamerBuffActive(void);
	void RecvTamerBuffInactive(void);

	void RecvTamerSkillSuccess(void);
	void SendTamerActiveSkill(u4 nSkillIdx);
	void SendTamerActiveSkillEnd(u4 nSkillIdx );
	void SendTamerSummonSkill( TCHAR const* szName, u4 nIndex, u1 nCase );
	void SendTamerSummonAccept(u1 nCase, u1 nValue);
	void SendTamerSummon_StateCheck(u1 nCase, u1 nValue, u1 nOK );
	void SendTamerSummon_StateCheck2(u1 nCase, u1 nValue, u4 nSkillCode );
	void RecvTamerSummon();

	void RecvTamerCashSkillUseResult(void);		// 테이머 캐쉬 스킬 아이템 사용 후 결과 값
	void RecvTamerCashSkillExpire(void);		// 테이머 캐쉬 스킬 만료 시
	void RecvTamerCashSkillRemove(void);		// 테이머 캐쉬 스킬 삭제 시
	void SendTamerCashSkillDelete( u4 nSkillIndex );	// 테이머 캐쉬 스킬 코드 삭제 시

	void RecvDotSkillSuccess(void);				// Dot 스킬
	void RecvStunSkillSuccess(void);			// 스턴 스킬

	void RecvBuffSkill();						// 스킬 발동 시 추가 버프 효과

	void RecvActiveSkill();						// 액티브스킬( 순간효과 ) 발동 시

	void RecvDigimonAddSkillSuccess();			// 디지몬 메모리 스킬 추가 성공
	void RecvDigimonSkillRemove();				// 디지몬 메모리 스킬 삭제 Recv
	void RecvDigimonMemorySkill_UseSuccess();	// 디지몬 메모리 스킬 사용 성공
	void RecvMemorySkill_EffectSync();			// 디지몬 메모리 스킬 이펙트 싱크

	void SendDigimonSkillDelete( u4 nSkillCode );		// 디지몬 메모리 스킬 삭제 Send
	void SendUseDigimonMemorySkill( u4 nDigimonUID, u1 nEvoStep, u4 nSkillCode, u4 nTargetUID );	// 디지몬 메모리 스킬 사용 Send

	bool RecvMonsterSkill( int nRecvType, int nIdx = 0 );	// 몬스터 스킬 시전 시 
	void RecvMonsterSkill_Use( int nIdx );		// 몬스터 스킬 사용 시( 씨앗깔기, 광폭화 스킬만 -> 징표 이펙트 제거 및 바닥 생성 )
	void RecvRaidChainSkill(void);
	void RecvMonsterSkillSync_PersistEnd();		// 바닥(씨앗) 만료 시

	//////////////////////////////////////////////////////////////////////////
	// 메크로 프로텍트 관련 함수
	//////////////////////////////////////////////////////////////////////////
private:
	void RecvMacroResult(void);
	void RecvMacroFailure(void);
	void RecvMacroSuccess(void);
	void RecvMacroQuestion(void);

public:
	void SendMacroQuestion(std::wstring const& szTamerName);

#ifdef SDM_MACRO_PROTECT_RENEW_20181217
	void SendMacroAnswer( std::wstring const& wsAnswer);
#else
	void SendMacroAnswer( n4 const& nRandNo, n4 const& nAnswer );
#endif
// 	void SendMacroAnswer(void);
// 	void SendMacroAnswerFailure(void);
	//////////////////////////////////////////////////////////////////////////


	void SendAwayTime(void);	// 자리 비움 상태로 변경 요청


public: // evolution
	void RecvEvoOpen(void);
	void RecvEvolution(void);
	void RecvEvoFailure(void);

public: // 거래 처리 수신
	void RecvTradeRequest(void);	// 거래 요청 받음
	void RecvTradeImpossible(void);	// 거래 불가한 상태
	void RecvTradeApproval(void);	// 상대가 거래 승인
	void RecvTradeReject(void);		// 거래 거부 당함
	void RecvTradeAddItem(void);	// 아이템 추가
	void RecvTradeCancelItem(void);	// 아이템 추가 취소
	void RecvTradeAddMoney(void);	// 겜머니 추가

	void RecvTradeInvenLock(void);		// 거래 인벤 잠금
	void RecvTradeUnInvenLock(void);	// 거래 인벤 잠금 해제
	void RecvTradeComplete(void);		// 거래 완료
	void RecvTradeCompleteFinal(void);	// 최종 거래 완료

	void SendSetMasterCardLeader( u2 nCardID );		// 씰 리더 선택 요청
	void SendUnSetMasterCardLeader( u2 nCardID );	// 씰 리더 해제 요청

	void RecvSetMasterCardLeader();					// 씰 리더 선택 결과
	void RecvUnSetMasterCardLeader();				// 씰 리더 해제 결과


	void SendCardUnSeal(u2 nInven_SealIdx);	// 씰 봉인 해제 요청(인벤토리 씰 인덱스, 봉인 해제 사용량)
	void RecvCardUnSeal(void);
	
	void SendReCardSeal(u2 nCardID, u4 nCardAmount);		// 씰 봉인 요청(카드번호, 카드수량)
	void SendCardDrop( short nInvenPos, u2 nCount );

	void RecvCardInfoAll(void);
	void SendCardBookmark(u2 nCardCode, u1 nBookmark);
	void RecvCardBookmark(void);

public:	// 테이머 개인 상점 처리
	void SendTamerShopReady(n4 nRequestType, u4 nSlot = 0);									// 개인 상점 오픈 준비 요청 ( 1 : 상점 오픈 준비, 2 : 상점 편집 상태로 변경 요청 3: 오픈준비 취소 4: 위탁 상점 준비 요청 5: 위탁 상점 준비상태 취소 )
	void SendTamerShopOpen(wchar const* szTitle, n4 nItemNumber, nTrade::Item *pSellItemList);	// 개인 상점 개설 요청
	void SendTamerShopClose(void);															// 개인 상점 폐쇄 요청
	void SendTamerShopItemList(u4 nTargetTamerUID, uint nType = 0);							// 개인 상점 아이템 리스트 요청 ( 타입 0: 개인상점 1: 위탁 상점 )
	void SendTamerShopBuy(u4 nTargetTamerUID, uint nIndex, cItemData *pItem, n8 Price);		// 상점으로 부터 아이템 구매 ( 대상 UID, 상점 아이템 인덱스, 구매개수, 아이템 타입, 개별가격 );

	void RecvTamerShopItemList(void);		// 테이머 샵 아이템 목록 요청 결과
	void RecvTamerShopReadyResult(void);	// 개인 상점 오픈 준비 결과
	void RecvTamerShopOpenResult(void);		// 상점 오픈 결과
	void RecvTamerShopBuy(void);			// 아이템 구매 결과
	void RecvTamerShopSellResult(void);		// 아이템 판매 결과
	void RecvTamerShopCloseResult(void);	// 개인 상점 Close 결과
public:	// 위탁 상점 처리	
	void SendCommissionShopOpen(nSync::Pos pos, float nFloat, wchar const* szTitle, n8 nMoney, uint nItemCount, nTrade::Item *item );	// 위탁 상점 오픈 요청	
	void SendCommissionShopClose(void);															// 위탁 상점 폐쇄 요청	
	void SendCommissionShopBuy(u4 nShopUID, uint nIndex, cItemData *pItem, n8 nUnitPrice);		// 위탁 상점 아이템 구매 ( 대상 UID, 상점 아이템 인덱스, 구매개수 )
	void SendCommissionShopSell(u4 nShopUID, uint nIndex, cItemData *pItem, n8 nUnitPrice);		// 사용 금지 - 개인 아이템 위탁 상점에 판매	(대상 UID, 상점 아이템 인덱스, 판매개수)	
	void SendCommissionShopItemWithDraw();														// 조합 상인 NPC에게서 아이템 & 돈 찾기
	void SendCommissionShopNpcItemList();														// 조합 상인 NPC가 보관중인 아이템 & 돈 리스트 요청
	void RecvCommissionShopCloseResult();

	void RecvCommissionShopOpenResult(void);			// 위탁 상점 오픈 결과	
	void RecvCommissionShopItemWithDrawResult(void);	// 위탁 상점 아이템 회수 결과
	void RecvCommissionShopItemListResult(void);		// 위탁 상점 아이템 리스트 요청 결과
	void RecvCommissionShopNpcItemList(void);			// 조합 상인 아이템 리스트 요청 결과
	void RecvCommissionShopSellResult(void);			// 위탁 상점에 아이템 판매 결과 
	void RecvCommissionShopBuyResult(void);				// 위탁 상점 아이템 구매 결과
public: // 부화 보정 처리
	void SendHatchLostGageProgress(u4 nRequestType, n4 npcIDX, u4 ItemType);				// 손실 보정 게이지 사용 및 충전 처리 ( 요청 타입, npcIDX, 아이템 타입 )
	void SendHatchLostGageProgressResult(void);

	void RecvHatchOptionItemAddResult(void);

public: // 아이템 캐시 관련 처리
	void RecvCashShop(void);
	void SendReqCashShop(void);	// 캐시 창고 갱신 요청

	void SendCashShopCart(void);	// 장바구니 목록 요청
	void RecvCashShopCart(void);	// 장바구니 목록 수신
	void SendCashShopCartSave(int *nItemTypes);	// 장바구니 목록 저장 요청

	void RecvCropCashItemSuccess(void);
	void SendCropCashItem(n2 nCashSlot);

	void SendCashShopBuyRequest_Steam(n4 nTotalPrice, std::list<DWORD> const& nItemNo);
	//void SendCashShopBuyRequest_Steam(int nCashType, n1 nItemCount, n4 nTotalPrice, n4 *nItemNo);
	void RecvBuyCashItemRequest_Steam(void);

	//void SendBuyCashItem( int nCashType, n1 nItemCount, n4 nTotalPrice, n4 *nItemNo, uint64 ui64OrderID = 0);
	void SendBuyCashItem( n4 const& nTotalPrice, std::list<DWORD> const& nItemNo, uint64 ui64OrderID = 0);
	void RecvBuyCashItem(void);

	void SendGiftCashItem(n4 const& nPrice, std::list<DWORD> const& nItemNo, std::wstring const& szPeerTamerName, uint64 ui64OrderID = 0);
	void RecvGiftCashItem(void);

	void SendCashShopBalance(void);
	void RecvCashShopBalance(void);
	void SendCashShopBuyHistory(void);
	void RecvCashshopBuyhistory(void);

public:
	void RecvGiftShop(void);
	void SendReqGiftShop(void);

	void RecvCropGiftItemSuccess(void);
	void SendCropGiftItem(n2 nGiftSlot);
//	void SendCropGiftItemAll(void);
public: // 용병보관소
	void SendDigimonCareSlotList(u4 nNpcType, u4 nInvenIdx = 0, bool bVip = false);	// 용병 보관소 용병 목록 요청
	void RecvDigimonCareSlotList(void);	// 용병 보관소 용병 목록 수신
	void RecvDigimonCareSlotSize(void); // 용병 보관소 확장 아이템 사용에 대한 응답

	void SendDigimonMoveTo(int nSlot1, int nSlot2, u4 nNpcType, bool bVip);
	void SendDigimonMoveBetweenArchive(int Src, int Dest, u4 nNpcType, bool bVip);
	// 기본 용병 슬롯 값은 0~nLimit::DigimonBaseSlot 사용
	// 용병 보관소 슬롯 값은 0~nLimit::DigimonCareSlot값 + 100으로 사용

public: // 거래 처리 송신
	void SendTradeRequest(u4 nTargetTamerUID);		// 상대에게 거래 요청
	void SendTradeApproval(u4 nTargetTamerUID);		// 거래 요청에 대한 승인
	void SendTradeReject(u4 nTargetTamerUID);		// 거래가 거부당함을 알림
	void SendTradeImpossible(u4 nTargetTamerUID);	// 거래가 불가능 상태임을 알림

	void SendTradeAddItem(u2 nInvenPos, u2 nCnt);
	void SendTradeCancelItem(n1 nPos);
	void SendTradeAddMoney(u4 nMoney); // 40억 bit 이상 거래 불가	

	void SendTradeInvenLock( bool bLock );		// 거래 창 인벤트로 잠금/해제 패킷
	void SendTradeCompleteFinal(void);			// 최종 거래 확인 패킷

public:	// option 처리
	void SendTamerOption(u4 nOption);
	void RecvTamerOption(void);
	void SendTamerAchievement(u2 nAchievement);
	void RecvTamerAchievement(void);

public:
	void RecvChangeStatValue(void);

	void RecvDigitamaScanResult(void);
	void RecvCapsuleToItem(void);


public: // game debug command	
	void RecvLoseTarget(void);
	void RecvSyncInfo(void);
	void RecvInsufficientRange(void);

public: // game master command
	void RecvChannelInfo(void);
	void SendChannelInfo(void);
	void RecvCheatLevel(void);

	void RecvHatchTry();

	void RecvUserInfo(void);
	void RecvUserList(void);
	void RecvUserInfoNotFound(void);
	void RecvConnCountInfo(void);

#ifdef CHAT_BAN
	void RecvChatBanUser(void);
#endif
	void RecvBanUser(void);
	void RecvDiscUser(void);
	void RecvInitSkillStat(void);
	void RecvInitDigimonSkillPoint(void);
	
	void RecvInsertItemToShop(void);
	void RecvInit2ndPass(void);
public:
	void RecvBattleOn(void);
	void RecvBattleOff(void);

public: // 용병 부화 관련
	void RecvHatchIn(void);		// 부화 디지타마 등록 결과
	void RecvHatchUpSuccess(void);
	void RecvHatchUpFailure(void);
	void RecvHatchOut(void);	// 부화 확인 결과
#ifdef CROSSWARS_SYSTEM
	void RecvCrossWarsHatch(void);
	void RecvCrossWarsEnergize(void);
	void RecvCrossNotReg(void);
#endif
	void RecvHatchBackupDisk(void);
	void RecvHatchBackupDiskCancel(void);	//백업디스크 등록 취소

public: // 용병 변경
#ifdef CROSSWARS_SYSTEM
	void RecvCrossChangePartner2(void);
	void RecvCrossChangePartner(void);
#endif
	void RecvChangePartner(void);		// 용병 변경
	void RecvBattleTagSuccess(void);	// 전투중 용병 변경 성공
	void RecvBattleTagFailure(void);	// 전투중 용병 변경 실패
	void RecvChangePartnerScale(void);	// 용병의 크기 변경

public:
	void RecvItemAnalysis(void);		// 아이템(소켓용) 분석 완료
	void RecvInvenSlotSize(void);		// 인벤토리 크기 변경시
	void RecvWarehouseSlotSize(void);	// 창고 크기 변경시

public:
	void RecvUpdateDailyQuest(void);	// 일일퀘스트를 초기화한다.
	void RecvQuestCancel(void);			// 퀘스트 취소시 오는 패킷


public: // 이름 변경 관련 처리
	void RecvChangeTamerName(void);
	void SendChangeTamerName(int nInvenSlotNo, wchar const * name);
	void SendChangeDigimonName(int nInvenSlotNo, wchar const * name);

public: // party system
	void RecvPartyCreated(void);

	std::wstring m_szPartyInviteTargetTamerName;
	void SendPartyInvite(std::wstring const & szTamerName);
	void RecvPartyInvite(void);
	void RecvPartyInviteResult(void);

	void SendPartyInviteResult( int const& nResultType, std::wstring const& targetName );
// 	void SendPartyInviteAccept(void);	// szTamerName ==> 초대 신청자, 즉 상대방의 테이머 이름 송신
// 	void SendPartyInviteReject(void);
// 	void SendPartyInviteImpossible(void);	// 다른 작업중으로 초대 요청에 대한 응답 처리 불가능

	void RecvPartyJoin(void);		// 새로운 파티원이 들어옮
	void RecvPartyMemberList(void);	// 새로 가입시, 기존 파티원들의 정보를 수신

	void SendPartyLeave(void);		// 스스로 파티를 탈퇴하는 경우
	void RecvPartyLeave(void);		// 다른 파티원이 파티를 나감

	void RecvPartyKick(void);
	void SendPartyKick(std::wstring const& szTamerName);	// 파티장이 멤버를 강제로 탈퇴시키는 경우

	void RecvPartyChat(void);				// 파티 채팅 수신
	void SendPartyChat(TCHAR const*szMsg);		// 파티 채팅 발신

	void RecvPartyDie(void);			// 파티원이 죽는 경우
	void RecvPartyExp(void);			// 파티 경험치 획득
	void RecvPartySyncInfo(void);		// hp, mhp, ds, mds 동기화
	void RecvPartySyncPosition(void);	// 위치 동기화

	void SendPartyChangeCropType(int nCropType, n1 nItemClass, n1 nItemDispGrade);	// pParty.h ==> nCrop::Base, nCrop::Free, nCrop::Order, nCrop::RareRate
	void RecvPartyChangeCropType(void);

	void RecvPartyChangeMap(void);
	void RecvPartyChangeDigimon(void);	// 용병 변경

	void SendPartyChangeMaster(int nNextMasterSlotNo);
	void RecvPartyChangeMaster(void);

	void RecvPartyDisconnected(void);
	void RecvPartySyncBuff(void);			// 나중에 삭제한 버프정보 받아야한다. 임의 삭제한 버프정보가 있을 때도 받아야 한다.

public: // CBT2 EVENT 관련 프로토콜
	void RecvMsgAllHatchOut(void);			// 용병 부화에 성공한 경우
	void RecvMsgAllCropEventItem(void);		// 이벤트 아이템(문화상품권등)을 습득한 경우

public: // OBT 출석 체크 EVENT 1
	void RecvChangeGameState(void);		// 게임의 상태값
	void Send_NTF_ExpEventTimeCheck(void);	// 일정 시간마다 서버에 경험치 시간 업데이트 체크(서버에 보내기만 한다)

	void RecvEventDailyCheck(void);		// 출석 체크
	void SendEventDailyCheck(void);		// 출석 하기

	void RecvDailyEventInfo(void);		// 매일 이벤트 정보
	void RecvMonthlyEventInfo(void);	// 2017-02-16-nova 월간 이벤트 정보

public:
	void RecvSocketInSuccess(void);		// 소켓에 아이템 넣기 성공
	void RecvSocketOutSuccess(void);	// 소켓에서 아이템 빼기 성공
	void RecvSocketClearSuccess(void);	// 소켓에서 아이템 제거 성공

//////////////////////////////////////////////////////////
// sync
	void SyncNewObject(void);		// 새로 만들어야 하는 경우
	void SyncDeleteObject(void);	// 이동 없이 바로 제거

	void SyncWalkObject(void);		// 다른 obj가 이동(걷기)시
	void SyncMoveObject(void);		// 다른 obj가 이동(뛰기)시

	void SyncInObject(void);		// 다른 obj가 시야에 들어오는 경우
	void SyncOutObject(void);		// 다른 obj가 시야에서 사라지는 경우

	void SyncCondition(void);		// 상태 변화시 처리
	void RecvSyncCondition(void);	// 본인의 상태

	void SyncInTamer(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInTamer(nSync::Pos& pos, int Handle, int ModelType, bool bNew = false);
	//void SyncInTamerAround(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInDigimon(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInDigimon(nSync::Pos& pos, int Handle, int ModelType, bool bNew = false);
	//void SyncInDigimonAround(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInMonster(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInMonster(nSync::Pos& pos, int Model, int Handle, bool bNew);
	void SyncInItem(nSync::Pos &pos, cType &type, bool bNew=false);
	void SyncInCommissionShop(nSync::Pos &pos, cType &type, bool bNew=false);

	void SyncNewTamer(nSync::Pos &pos, cType &type);
	void SyncNewDigimon(nSync::Pos  &pos, cType &type);
	void SyncNewMonster(nSync::Pos &pos, cType &type);
	void SyncNewItem(nSync::Pos &pos, cType &type);
	void SyncNewCommissionShop(nSync::Pos &pos, cType &type);

	void SyncMoveTamer(u4 nIDX, nSync::Pos &pos);
	void SyncMoveDigimon(u4 nIDX, nSync::Pos &pos);
	void SyncMoveMonster(u4 nIDX, nSync::Pos &pos,bool bWalk);

	void SyncMsgChat(void);
	void SyncMsgWhisper(void);
	void SyncMsgAll(void);
	void SyncMsgSystem(void);
	void SyncMsgShout( bool bGuild = false );	// 길드 외치기인지 확인
	void SyncMsgAllByItem(void);

// 	void SyncState(void);
// 	void SyncState(ushort nUID);

	// 버프에 관한 싱크
	void SyncInBuff(void);
	void _SyncInBuffObject(int nObjectCnt);


/////////////////////////////////////////////////////////
// send
private:
	void SendAccessCode(void);	

public:
	void SendSelectPortal(uint nPortalNo);
	void SendJumpGate(u2 nInvenPos, u2 nOrderIDX, bool bVip =false);	// nInvenPos : 사용할 아이템의 인벤 위치, nOrderIDX : 해당 아이템의 몇번째 기능을 사용할지(0~n)

	void SendChangeServer(void); // game server -> game server
	void SendOpenRegion(int nOpenRegionIDX); // 새로운 지역(region)에 들어가는 경우, 해당 영역에 한번 이상 들어 왔음을 기록

	void SendGoBackGate(void); // game server -> gate server

	void SendReady(nSync::Pos &DigimonStartPos); // 게임 로딩 완료 보고
	void SendDieConfirm(void); // 해당 넘을 마을로 강제 이동한다.

	void SendTamerJumpGate( u2 nItemID, u2 nMapID );

public:
	void SendEmoticon(int nType, int nVal = -1);
	void RecvEmoticon(void);
	void SendChangeTamer(int nTamerType, int nInvenSlotNo);
	void RecvChangeTamer(void);
#ifdef ADVENTURE_TAMER_CHANGE
	void RecvChangeTamerAround(void);
#endif
public:
	void SendChatMsg(wchar* msg);
	void SendMoveTo(u4 uid, nSync::Pos tp, float fDirect);	
	void SendMoveTo(u4 uid, nSync::Pos sp, nSync::Pos tp, float fDirect);
	void SendMoveToKnockBack(u4 uid, nSync::Pos tp);

	void SendMsgAllByItem(wchar* msg, n4 nItemSlotNo);	// 현재 서버 전체에 송신(루체섭만, 리바섭만...)
	void RecvMsgAllByItemFailure(void);	// 외치기 사용 실패

	void SendMsgWhisper( TCHAR const* name, TCHAR const* msg );	// 상대 유저에게만 전송	

	void SendMsgShout(wchar const* msg);		// 외치기
	void RecvMsgShoutFailure(void);		// 외치기 실패

public: // game master command
 	void SendJump(int x, int y);
 	void RecvSetMoney(void);

 	void SendChangeChannel(uint nChannelIDX);

 	void SendQueryMoneyInven();
 	void RecvQueryMoneyInven();
 	void RecvKillMonster();
 	void RecvGmQuest();
 	void RecvGmSetSkillPoint();
 	void RecvGMCoolReSet();
 	void RecvGuildSkillReSet();		//2017-06-28-nova

public: // 디지타마 부화 관련 명령
	void SendHatchIn(uint nInvenSlotNo, n4 nNpcIDX, bool bVip = false);	// 부화 디지타마 등록

	void SendHatchBackupDisk(uint nInvenSlotNo, n4 nNpcIDX, bool bVip = false);

	void SendHatchUp(n4 nNpcIDX, n1 nDataLv = 0, bool bVip = false);	// 부화 디지몬 데이타 입력

	void SendHatchCancel(n4 nNpcIDX, bool bVip = false);

	void SendHatchBackupDiskCancel(n4 nNpcIDX, bool bVip = false);

	void SendHatchOut(wchar* szName, n4 nNpcIDX, u4 nInvenPortablePos = 0, bool bVip = false);	// 부화 요청

#ifdef CROSSWARS_SYSTEM
	void SendCrossWarsHatch(u2 IDX, wchar* szName);	// 부화 요청

	void RecvCrossWarsRelease(void);
	void RecvCrossWarsCRRelease(void);

	void RecvCrossWarsCrownHeal(void);
	void RecvCrossWarsLoader(void);
	void SendCrossWarsRecover(void);
	void RecvCrossWarsRecover(void);
	void SendCrossChangePartner(int nArrIDX);
	void SendCrossChangePartner2( int nArrIDX, int nArr[]);
#endif

	void SendChangePartner(u1 nArrIDX);
	void SendDeletePartner(u1 nArrIDX, char *pass2); // 해당 디지몬 제거
	void RecvDeletePartner(void);

public: // inventory command
	void SendItemMove(n2 nSrcPos, n2 nDstPos, bool bCheckBelonging = true );
	void SendItemSplit(n2 nSrcPos, n2 nDstPos, u2 nCount);
	void SendItemDrop(n2 nInvenPos, nSync::Pos &FieldPos, u2 nCount);	

	void SendItemCrop(n4 nItemIDX);
	void SendItemUse(u4 nUID, n2 nInvenPos);
#ifdef CROSSWARS_SYSTEM
	void SendItemUseCross(u4 nUID, n2 nInvenPos);
#endif
	void SendItemAnalysis(u4 nNpcIDX, u2 nInvenPos, u4 nInvenPortablePos = 0, bool bVip = false);

public: // shop command
	void SendBuy( u4 NpcFTID, n4 ShopSlotIndex, u2 ItemCount );
	void SendSell( u4 NpcFTID, u1 InvenSlotIndex, u2 ItemCount );
	void SendPropertyBuy( u4 NpcFTID, n4 ShopSlotIndex, u2 ItemCount, u2 ItemIndex );


public: // 디지타마 확인 및 아이템 반환 새버전
	void SendDigitamaScanItem( bool bIsVip, u4 uPortableIdx, u4 uNpcIdx, u4 uInvenIdx, u2 uScanCount );	// new digitama scan
	void SendDigitamaToItem(u4 nNpcIDX, u4 nInvenPos, u4 nInvenPortablePos = 0, bool bVip = false);	// scan digitama
	void SendDigitamaToMoney(u4 nNpcIDX, u4 nInvenPos, u4 nInvenPortablePos = 0, bool bVip = false );	// sell digitama

	void SendCapsuleToItem(u4 nNpcIDX, u4 nInvenPos);	// scan capsule

public: // 소켓 장착 및 해제
	void SendSocketIn(u4 nNpcIDX, u2 nInvenSrcPos, u2 nInvenDstPos, u1 nSocketOrder, u4 nInvenPortablePos = 0, bool bVip = false);
	// nInvenSrcPos 위치의 소켓아이템을 nInvenDstPos위치의 장비 아이템에 장착한다.
	// nInvenSrcPos 위치의 아이템은 제거된다.

	void SendSocketOut(u4 nNpcIDX, u2 nInvenSrcPos, u2 nInvenDstPos, u1 nSocketOrder, u4 nInvenPortablePos = 0, bool bVip = false);
	// nInvenSrcPos 위치의 장비 아이템에서 nSocketOrder(left->right, 0~nLimit::SocketSlot) 위치에 박힌 소켓 아이템 분리
	// nInvenDstPos 위치에 뽑아낸 아이템 생성

	void SendSocketClear(u4 nNpcIDX, u2 nInvenPos, u1 nSocketOrder);


public:
	void SendSetTarget(u4 nHitterUID, u4 nTargetUID);
	void SendAttack(u4 nHitterUID, u4 nTargetUID);
	void SendStopAttack(u4 nUID);
	bool SendSkill(u1 nSkillArrIDX, u4 nHitterUID, u4 nTargetUID);
//	void SendSkill(u1 nSkillArrIDX, u2 nHitterUID, nSync::Pos &TargetPosition);
	void SendActiveSkill(u4 nSkillArrIDX, u4 nTargetUID);

public: // evolution
	void SendEvolution(uint nDigimonUID, uint nSlotIDX);
	void SendEvoCard(uint nDigimonUID, uint nInvenSlot);
	void SendEvoCrest(uint nDigimonUID, uint nInvenSlot);


public: // quest
	void SendQuestRequire(uint nQuestIDX);	// 퀘스트 요청
	void SendQuestCancel(uint nQuestIDX);	// 퀘스트 취소
	void SendQuestComplete(uint nQuestIDX);	// 퀘스트 완료

	void RecvQuestChangeInfo(void);
	void SendQuestChangeInfo(uint nQuestIDX, uint nCondIDX, uint nValue);

	void SendQuestAchievement(uint nQuestIDX);

public: // buddy
	void SendBuddyList(void);	// 친구 및 차단 목록 요청
	void SendBuddyAdd(std::wstring const& szTamerName);
	void SendBuddyDelete(std::wstring const& szTamerName);
	void SendBuddyMemo(std::wstring const& szTamer, std::wstring const& szMemo);
	void SendBuddyBan(std::wstring const& szTamerName);	// 기존에 친구 목록에 있는넘도 같은 메세지로 처리

	void RecvBuddyOn(void);
	void RecvBuddyOff(void);

	void RecvBuddyList(void);			// 친구 및 차단 목록
	void RecvBuddyAdd(void);			// 친구 등록 성공
	void RecvBuddyBan(void);			// 차단 등록 성공
	void RecvBuddyNotFound(void);		// 친구, 차단 등록 실패 - 해당 이름을 가진 테이머는 존재하지 않습니다.
	void RecvBuddyChangeName(void);		// 친구 이름 변경

public: // guild
	void SendGuildCreate(TCHAR const*title, u2 nInvenSlot, u4 nNpcIDX);	// 새로운 길드를 생성함, nInvenSlot 길드 생성시 사용한 아이템의 인벤 위치
	void SendGuildClose(void);	// 자신의 길드를 폐쇄함, 폐쇄 완료됨 응답은 불필요

	void SendGuildAllow(TCHAR const*szTamerName, u4 nCertifiedCode);	// pGuild::ReqAdd의 응답 - 허가
	void SendGuildReject(TCHAR const*szTamerName, u4 nCertifiedCode);	// pGuild::ReqAdd의 응답 - 거부

	void SendGuildLeave(void);	// 길드에서 스스로 탈퇴하는 경우
	void SendGuildDelete(std::wstring* szTamerName);	// 상대를 강퇴함(자신이 길드장이어야 함)

	void RecvGuildChangeNotice(void);
	void RecvGuildCreate(void);	// 길드 생성 성공 여부

	bool SendGuildReqAdd(TCHAR const*szTamerName);	// 해당 테이머 등록
	void RecvGuildReqAdd(void);			// 길드에 해당 테이머 등록 요청

	void SendGuildReqAddFailure(TCHAR const*szTamerName);
	void RecvGuildReqAddFailure(void);	// 길드에 등록 요청 실패

	void RecvGuildReject(void);
	void RecvGuildAllowFailure(void);	// 길드 가입 완료 처리 실패

	void RecvGuildClose(void);	// 길드 해체

	void SendGuildInfo(void);	// 길드 정보 및 멤버 목록을 요청, 가급적 자주 호출하지 아니한다.
	void RecvGuildInfo(void);	// 길드 정보 및 멤버 목록 수신

	void SendGuildHistory(void);
	void RecvGuildHistory(void);

	void SendGuildChat(TCHAR *msg);
	void RecvGuildChat(void);

	void RecvGuildOn(void);
	void RecvGuildOff(void);

	void RecvGuildRankChange(void);

	void RecvGuildAdd(void);		// 해당 테이머가 길드에 추가됨
	void RecvGuildLeave(void);		// 해당 테이머가 길드에서 탈퇴함
	void RecvGuildDeleted(void);	// 해당 테이머가 길드에서 강퇴당함

	void RecvGuildToMaster(void);
	void RecvGuildToSubMaster(void);
	void RecvGuildToMember(void);
	void RecvGuildToSubMember(void);
	void RecvGuildToDatsMember(void);

	void SendGuildToMaster(std::wstring* szNextMasterTamerName);
	void SendGuildToSubMaster(std::wstring* szNextSubMasterTamerName);
	void SendGuildToMember(std::wstring* szNextMemberTamerName);
	void SendGuildToSubMember(std::wstring* szNextMemberTamerName);
	void SendGuildToDatsMember(std::wstring* szNextMemberTamerName);

	void RecvGuildDeletedTamer(void);
	void RecvGuildChangeTamerName(void);

	void RecvGuildLevelUp(void);
	void RecvGuildMemberLevelUp(void);

	bool SendChangeGuildNotice(wchar const* msg);

	void SendGuildChangeClassName(int nClass, wchar const* szName, wchar const* szDesc);
// #ifdef TAIWAN_NAME_LENGTH
// 	void SendGuildChangeClassName2(int nClass, wchar* szName, wchar* szDesc, bool bClass);
// #endif
	void RecvGuildChangeClassName(void);

	void RecvGuildIncMember(void);

#ifdef GUILD_RENEWAL
	void RecvGuildSkillPoint();						// 길드 스킬 포인트(GSP) 변경 시 수신( 치트때만? )O
	void SendGuildMemoAdd( TCHAR const* szMemo );			// 길드원 메모(본인) 저장
	void SendUseGuildSkill( int nType, void* pData = NULL );	// 길드 스킬 사용 ( 스킬Apply, 스킬코드 )
	void RecvUseGuildSkill();									// 길드 스킬 사용 recv
	void RecvRemoveUseGuildSkill();					// 길드 스킬 기간 만료 or 상위 스킬 사용으로 하위 스킬 삭제 시

	void SendGuildSkill_Summon( u4 nSkillCode );	// 길드원이 소환 수락 할 때 보냄

	void RecvGuildSkill_Summon();					// 길드장이 썼을 때 길드원이 수신
	void RecvGuildSkillError();						// 스킬 사용 에러 발생 시 수신
	void RecvGuildChangeMemo();						// 길드 메모 수정 되면 모두 받음.#endif
#endif
public: // stat and skill
	void SendStatUp(uint nUID, uint nStat);
	void SendDigimonSkillLevelUp(uint nUID, uint nEvoUnitArrIDX, uint nSkillArrIDX);
	void SendOpenEvoSlot(uint nEvoUnitArrIDX, n2 nInvenIdx = -1);
	void SendOpenRideMode(uint nEvoUnitArrIDX, int nItemType);

	void SendRideOn(void);		// 탑승 요청
	void RecvRideOn(void);		// 탑승

	void SendRideOff(void);		// 하차 요청
	void RecvRideOff(void);		// 하차
public:	// PC 방 관련
	void SendPCBangInvenRequest(); // PC방 인벤토리 정보 요청
	void SendCropPCBangInvenItem(uint nSlot, uint nType, uint nCount);// PC방 인벤토리 아이템 획득

	void RecvPCBangItemCropResult(); // PC방 인벤토리에서 아이템 획득 결과
	void RecvPCBangInven(void);	// PC방 인벤 토리 정보
	void RecvPreminum(void);	// 유저 PC방 프리미엄 서비스 시작
	void RecvPreminumEnd(void);	// 유저 PC방 프리미엄 서비스 만료
	void RecvPartyPCBangExpBuff(void); // 파티에 PC방 유저수와 추가 경험치률 수신

public:
	void SendDigimonInchant(uint nType, uint ItemSlot, int RecoverDisk); // 강화 타입, 디지클론 슬롯, 리커버리 아이템 슬롯
	void RecvDigimonInchantResult();

	void SendDecreaseDigimonInchant(const UINT& nType, const int& nItemSlot); //약화
	void RecvDecreaseDigimonInchantResult();

public:
	void SendClientOptionSave(u4 nOption);	
	void SendViewEventSeen();

private:
	void Recv_NProtectCheck(void);	// nprotect 게임가드 체크

public:
	void ACK_Send_nProtect( UINT unUniqueIdx, BYTE* byPacket, DWORD dwSize );
	void NTF_Send_nProtect_BotDetect( UINT unUniqueIdx, BYTE* byBotPacket, DWORD dwSize);

public: // option
	void SendRefreshScreen(void);
public:
	void RecvPortableItemFail(void);
	void RecvDigimonMoveResult(void);
	void RecvDigimonMoveResultInArchive(void);
public: // PlayTime Set
	void RecvPlayTime();
	void RecvPlayTimePaneltyChange();
public:
	void RecvBuffCreate();		// 버프 생성됨
	void RecvBuffChange();		// 버프 변경됨
	void RecvStatChange();		// 스탯 변경됨
	void RecvBuffClear();		// 버프 만료 처리

	void SendManualRemove(u1 nType,u2 nBuffIDX);	// 버프 수동 제거
	void RecvBuffApplyFail();

	void _RecvDotSkillAround();
	void RecvBuffSyncInit();	// 초기 버프 싱크 정보
	void RecvDotDie();
#ifdef MONSTER_SKILL_GROWTH
	void RecvMonBuffSet();				// 몬스터 성장 스킬 셋
	void RecvMonBuffRelease();		// 몬스터 성장 스킬 해제 ( 전투종료 )
#endif

public: // Master Cheat Result
	void RecvMasterConnCount();
	void RecvMasterPayCash();

public:	// Spirit Evolution
	void SendSpirit_ItemToDigimon( int nModelID, std::wstring const&, int nNpcID );
	void SendSpirit_DigimonToSpirit( u1 nDelSlot, char const* pass2, int nNpcID );
	void SendSpirit_AncientSpiritEvolution( u1 nDelSlot, u1 nDelSlot2, char* pass2, TCHAR* szName, int nNpcID );

	void RecvSpirit_SpritToDigimon();
	void RecvSpirit_DigimonToSpirit();
	void RecvSpirit_AncientSpiritEvolution();

	//=============================================================================================================================
	//
	//		InsMap
	//
	//=============================================================================================================================
public:
	void	SetInsMap_DungeonRestoreFlag();
	void	SetInsMap_DungeonMSG(uint nPortalNo);

	void	SendInsMap_DungeonIN(uint nPortalNo);
	void	RecvInsMap_DungeonFail();
	void	RecvBattleEvoPoint();


/////////////////////////////////////////////////////////
// 
	//static void SetAttackInfo(	ushort nHitterUID, 
	//							ushort nTargetUID, 
	//							int nDamage,								
	//							CsC_AttackProp::eHIT_TYPE ht,
	//							CsC_AttackProp::eDAMAGE_TYPE dt,
	//							int nEffectType );			// 어택정보 설정

	static void SetAttackInfo( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, bool bLastAttack );			// 어택정보 설정

	static void SetSkillInfo( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, bool bLastAttack );				// 스킬정보 설정
	static void SetSkillInfo2( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, bool bLastAttack, u4 nTargetUID, int nSkillSpeed, int nSkillAceel );				// 스킬정보 설정
	void	RecvRaidRankingList();

	static void SetDamage( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo);

	static void SetSkillInfo( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, CsC_AvObject* pTargetMark, bool bLastAttack, bool &bMaxCount, bool bRecvComp );	// 디지몬 광역 스킬 정보
	static void SetDotInfo( sRECV_HITTER_INFO* pHitterInfo, sRECV_TARGET_INFO* pTargetInfo, int nBuffCode, bool bLastAttack );				// 도트정보 설정

	static void	SetDieInfo( CsC_AvObject* pTarget );

	//////////////////////////////////////////////////////////////////////////
	// 배틀 매칭 관련 함수
	//////////////////////////////////////////////////////////////////////////
	// Recver
	void RecvMyBattleMatchInfo();			// 내 배틀 정보		
	void RecvBattleRegistRequest();			// 배틀 등록 요청 결과 패킷
	void RecvBattleRegistCancel();			// 배틀 등록 취소 결과 패킷
	void NtfBattleRegistReadyInfo();
	void RecvBattleRegistReadyResult();
	void NtfBattleRegistReadyResult();
	void NtfBattleMatchCancel();
	void NtfBattleMatchComplete();
	void NtfBattleStart();
	void NtfBattleEnd();
	void NtfBattleReward();
	void NtfBattleMemberSync();
	void NtfBattleMatchSync();

	// Sender
	void SendMyBattleMatchInfo();			// 서버에 내 배틀 정보를 요청 한다.
	void SendBattleMatchRequest( n1 const& nMainMode, n1 const& nSubMode, u4 const& nNpcID, u4 const& nMapID );// 배틀 신청
	void SendBattleMatchCancel();			// 배틀 취소
 	void SendBattleReady( n1 nIs, n4 const& nBattleIdx );			// 응답

// 	void RecvBattleUpdateRoundInfo();
// 	void RecvBattleResult();
// 	void RecvBattleMatchFail();				// 배틀 매치 실패
// 	void RecvBattleMatchReady();			// 배틀 유무
// 	void RecvBattleMemberInfo();			// 경기 정보( 팀과 이름 )
// 	void RecvBattleEnd();					// 경기 끝날경우 보냄(보상도 보냄)
// 	void RecvBattleReadyComplete();			// 경기 시작해도 됨
// 	void RecvBattleJoin();					// 경기에 들어옴
// 	void RecvDualBattleRequest();
// 	void RecvDualBattleFail();
// 	void RecvDaulBattleSucc();
// 	void RecvBattleTeamSync();

// 	void SendBattleGiveUp();				// 배틀 포기
// 	void SendBattleCheat(char *cmd);
// 
// 	void SendDualBattleRequest( TCHAR *szTamerName );
// 	void SendDualBattleResponse( n1 nIs );

	//////////////////////////////////////////////////////////////////////////

// 친구 초대 이벤트 처리
private:
	void RecvAddFriendly();
	void RecvAddFriendlyAnswer();
	void RecvFriendlyMark();
	void RecvFriendlyMarkItem();
	void RecvFriendlyStateChange();
	void RecvFriendlyInfo();
	void RecvFriendlyNameChange();

public:
	void SendAddFriendly(TCHAR const * szName);
	void SendAddFriendlyAnswer(u2 nAnswer, TCHAR const * szName);
	void SendMarkItem(u1 nFriendlyType, u2 nMarkIndex, TCHAR const * szName );
	void SendFriendInfo(void);

	void RecvCropGiftInvenFail();

	void RecvGiftBoxUseSuccess();

	void SendCardEventStart();	// 카드 이벤트 시작을 서버에 알림
	void RecvCardEventStart();	// 카드 이벤트 시작 시 서버에서 데이터를 받음

	void SendCardEventEnd();
	void RecvCardEventEnd();

	void RecvNatureExp();
	void SendGetNatureItem( n1 nElementType, n1 nNatureType );

	// 오른쪽 버튼으로 일부 알(이벤트) 부화되는 기능
	void SendRClickDigimonHatch( int nType, int nInvenSlotNo, wchar const * name );
	void RecvRClickDigimonHatch();

#ifdef MINIGAME
	void SendMinigameStart( u1 nGameKind, u1 nlevel );//미니게임 시작
	void RecvMinigameStart();

	void SendMinigameClickBtn( u1 nGameKind, u2 nBarIndex );	
	void RecvMinigameClickBtn();

	void SendMinigameTimeOut();	//stop버튼 안누르고 시간 지나면 타임아웃 보내줘
	void RecvMinigameTimeOut();

	void SendMinigameEnd( u1 nEndResult);//미니게임 끝 .. 정상종료 : 1, 비정상종료 : 0
	void RecvMinigameEnd();

	void RecvMinigameLimit();			//미니게임 실행 못하는 경우의 에러 메세지들


	void SendMiniGameIsStartReq();		// 미니게임 가능 한지 여부 요청.
#endif

	void SendGotchaOpen(u4 nNpcIDX);//레어머신 오픈
	void RecvGotchaOpen();

	void SendGotchaGet( u4 nUIDX , u4 nNpcIDX, u4 nInvenIndex, u4 nResetCnt );//뽑기 버튼 눌렀을때
	void RecvGotchaGet();

	void SendGotchaItem( u4 nNpcIDX, u4 nItemID, /*u4 nItemCnt,*/ u4 nAllItemCnt, u4 nInvenSlotCnt, u4* nDecreaseItemSlot/*, n4 nGroupID*/ );//획득한 아이템 뭔지 알려주기(로그용)
 	void RecvGotchaError();//에러코드

	void RecvMsgAllGotchaMachine();//전광판 메세지 
	void RecvGotchaRefresh();//새로고침 (안씀)

#ifdef EGG_PREFERMENT
	void SendMysteryGet( u4 nUIDX, u4 nNpcIDX, std::map< int, int >& mapItems, u4 nItemID );//수수께끼 뽑기
#else
	void SendMysteryGet( u4 nUIDX, u4 nNpcIDX, std::map< int, int >& mapItems );//수수께끼 뽑기
#endif
	void RecvMysteryGet();

	void RecvRunGotchaMachine();					//레어머신 가상으로 돌리기 치트키

#ifdef ADVENTURE_TAMER_CHANGE
	void RecvMoveClear();	//테이머체인지 아이템 사용시 장비창 벗기자.
#endif

// #ifdef FIREWORK
// 	void SendFireWork( u2 nUIDX, u4 nItemID, u2 nInvenPos );		//테이머 유아이디랑 사용 아이템ID 보낸다.
	void RecvFireWork();
//#endif

	void RecvSyncEnchantEffect();	// 닷본 강화 후 나왔을때 다른 유저에게 강화 이펙트 안보이는 현상 lks007	13.07.01

	void SendRequestRewardList();
	void RecvRequestRewardList();
	void SendRequestRewardAll();
	void SendRequestRewards( u1 nReqCnt, u1 nReqIdx );
	void RecvRequestRewards();

	void RecvRequestRewardFail();

	void SendMastersMatchOpen( u4 nTamerIDX, u4 nNPCIDX );
	void RecvMastersMatchOpen();

	void SendMastersMatchInsert( u4 nUserID, u4 nNpcID, u4 nSlot, u4 nGiveCnt, u1 nFTNpcIdx/*Npc테이블에 있는 아이템 인덱스*/ );
	void RecvMastersMatchInsert();

	//악세서리 감정
	void SendAccessoryCheck( u4 nUID, u2 nInvenPos );
	void RecvEnchantItemIdentify();
	//악세서리 강화
	void SendAccessoryEnchant( u4 nUID, u2 nItemPos, u2 nAccPos, int nComboIdx = -1 );
	void RecvEnchantItemEnchant();

#ifdef CHEAT_ITEMRANK
	void RecvItemRank();			// 아이템랭크 가상으로 돌리기 치트키
#endif

	void SendIncubaterList();						//	인큐베이터 오픈
	void RecvIncubaterList();						//	인큐베이터 오픈

	void SendIncubatorFacilitation( u1 nIncuSlot, u4 nHouseSlot, u4 nInvenSlot );	//	촉진 - 인큐베이터 슬롯(0~4), 디보 슬롯(1000~), 등록아이템 인벤 슬롯
	void RecvIncubatorFacilitation();				//	촉진

	void SendIncubatorExtraction( u1 nIncuSlot );				//	추출 ( 꺼내기 ) - 인큐베이터 슬롯(0~4)
	void RecvIncubatorExtraction();				//	추출 ( 꺼내기 )

	void RecvIncubatorError();				// 예외상황 처리
	
	void SendIncubaterSlotChange( n4 nIncuSlot, n4 nHouseSlot );	//	촉진 중인 디지몬 교체 - 인큐베이터 슬롯(0~4), 디보 슬롯(1000~)
	void RecvIncubaterSlotChange();			// 촉진 중인 디지몬 교체

	void SendTacticsOpen( u4 nUID, n2 nInvenPos );
	void RecvTacticsOpen();

#ifdef SELECT_SERVER_ERROR
	void RecvSelectServer_Er();
#endif
#ifdef REPURCHASE
	void SendItemRepurchaseOpen( u4 nNpcID );		// 재구매창 오픈
	void RecvItemRepurchaseOpen();						// 재구매창 오픈 응답

	void SendItemRepurchase( u4 nNpcID, u2 nItemIdx );		// 재구매
	void RecvItemRepurchase();											// 재구매 응답
#endif

	void SendEncyclopediaOpen( );
	void RecvEncyclopediaOpen( );

	void SendEncyclopediaGetItem( u4 nDigimonID );
	void RecvEncyclopediaGetItem( );

	void SendEncyclopediaGetDeck( u4 nDeckIdx );	// 덱 사용 결정 ( 1001 ~ )
	void RecvEncyclopediaGetBuffStat();				// 발동형 버프 발동 시 변동 스탯 Recv

	void RecvEncyclopediaBuff();					// 발동형 버프 시간 정보

	void RecvGlobalMsg();	// 서버의 모든 유저에게 메세지 보여줌
	void RecvCapsuleEvol_Result();	// 캡슐진화 시도 결과

	void RecvActorObjectFactor_Init();	// Actor 오브젝트( 문 ) 개폐를 위해 현재 맵에 살아있는 몬스터 전체 Init
	void RecvActorObjectFactor();		// Actor 오브젝트( 문 ) 개폐를 위해 현재 맵의 몬스터 죽을 시 수신

	void SendPinokimonInfoReq();	// 카드 투표수 갱신을 위한 요청
	void RecvPinokimonInfoReq();	// 갱신 요청에 대한 응답

	void SendPinokimonVote( u1 nCardIdx, u4 nVoteCnt, u4 nNpcID );	// 카드에 투표	: 투표 카드 인덱스, 투표 아이템 수, NpcID	( 서버 응답 대기 )
	void RecvPinokimonVote();		// 투표 정상 완료	: 인벤 슬롯 카운트, 인벤 슬롯 인덱스, 투표 아이템 수 ( 서버 응답 대기 해제 )

	void RecvRemoveVoteItem();		// 투표권 아이템 리셋
	
	// 디지몬 데이터 교환
	void SendDigimonDataChange( u4 nNPCIDX, nDigimon::eDataChangeType eType, u1 nSrcArrIDX, u1 nDstArrIDX );
	void RecvDigimonDataChange();

	void RecvErrorMessage();

	void AttackProcesses(	u4 nHitterUID, u4 nTargetUID, n4 nDamage, u4 nBattleOption, n4 hp, n4 Mhp, bool bDie);

	void TamerSkillErrInEpicDungeon();

	void RecvTradeLimitedTimeOut(void);

	void SendItemActive(u2 invenPos);	//2017-04-04-nova	시간제 아이템 활성화

	void RecvItemActive(void);			//2017-04-04-nova	시간제 아이템 활성화되었을경우 결과값


	//////////////////////////////////////////////////////////////////////////
	void Send_ServerRelocate_CheckTamerName( int const& nServerIDX, std::wstring const& wsTamerName );
	void Recv_ServerRelocate_CheckTamerName();
	void Send_ServerRelocate_ItemUse( int const& nInvenPos, int const& nServerIDX, std::wstring const& wsTamerName );
	void Recv_ServerRelocate_ItemUse();

	//////////////////////////////////////////////////////////////////////////
	void Recv_InfiniteWar_ChallengeStageClear();
	void Recv_InfiniteWar_ChallengeTryNextStage();
	void Recv_InfiniteWar_ChallengeGiveup();
	void Recv_InfiniteWar_ChallengePartyAlreadyPlay();

	void Send_InfiniteWar_ChallengeTryNextStage(n4 const& nNpcIdx);
	void Send_InfiniteWar_ChallengeGiveup(n4 const& nNpcIdx);

	void Send_Request_ChallengeData();
	void Recv_ChallengeData();
	void Send_Challenge_PointItem_Put(std::list<nChallengeSt::st_ItemInfo> const& items);
	void Recv_ChallengeData_PointItem_Put();
	void Ntf_ChallengeDailyUpdate();

	void Recv_ArenaRequestRank();	// 아레나 랭킹 리스트 패킷
	void Recv_ArenaRequestOldRank();// 아레나 이전 랭킹 리스트 패킷
	void Recv_ArenaRequestFame();// 아레나 명예의 전당 리스트 패킷
	void Recv_ArenaRequestFameSeasonList();
	void Send_ArenaRequestRank(unsigned char const& nRequestRankType);
	void Send_ArenaRequestOldRank(unsigned char const& nRequestRankType);
	void Send_ArenaRequestFame(unsigned short const& nRequestSeasonIdx);
	void Send_ArenaRequestFameSeasonList();
	//////////////////////////////////////////////////////////////////////////

	void Recv_XGToCrystal();
	void Recv_CrystalToXG();
	void Recv_XGuageUpdate();
	void Recv_XGuageMaxUpdate();
	void Send_XGToCrystal();
	void Send_CrystalToXG();
	//////////////////////////////////////////////////////////////////////////

	void Send_VipMemberShipData();
	void RecvVipMemberShipData();

	//////////////////////////////////////////////////////////////////////////
	void RecvDigimonTranscendenceChargeExp();
	void SendDigimonTranscendenceChargeExp( u1 const& nIsVip, u4 const& invenSlotIdx
										   ,DWORD const& dwNpcIDX, u1 const& nChargeType
										   ,u1 const& nChargeDigimonTaticsIdx, std::list<u2> const& listMaterialDigimons
										   ,std::list<stTranscendenceItem> const& nSubMaterialItems);

	void SendDigimonTranscendence(u1 const& nIsVip, u4 const& invenSlotIdx, 
								  DWORD const& dwNpcIDX, u1 const& nDigimonTaticsIdx,
								  u8 const& nNeedMoney);
	void RecvDigimonTranscendence();

	//////////////////////////////////////////////////////////////////////////
	void NTF_NeonDisplay(); // 전광판 데이터 받음.
	//////////////////////////////////////////////////////////////////////////

	void SendSkillMaxLvUp( int const& nUseItemInvenPos, int const& nUseItemType, int const& nEvoSlotNum );
	void RecvSkillMaxLvUp();
	void NTF_DigimonSkillCoolTime();
	void RecvDigimonSkillLevelUp();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Xigncode
	void NTF_Send_Xigncode( char* pBuffer, SIZE_T BufferSize );
	void Recv_XignCodeCheck(void);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Inven Sort
	void SendItemSort( u1 eSortFlag );
	void RecvItemSort();
	void RecvInvenResult();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Time Charge
	// 충전 목록 요청 / 결과
	void SendRequestChargeList();
	void RecvChargeListResult();

	// 아이템 충전 요청 / 결과
	void SendNormalChargeItem( n2 nSlotPos, n2 nItemPos, n4 nItemType );			// 일반 충전
	void RecvNormalChargeItemResult();

#if COMMON_LIB_FIXED
	void SendQuickChargeItem( n2 nItemPos, n4 nItemType, n4 nChargeType, n1 nChargeTimeIndex,
								std::list< nsTimeCharge::sChargeItemInfo >& lChargeItems );	// 즉시 충전
#endif

	void RecvQuickChargeItemResult();

	// 아이템 충전 취소 / 결과
	void SendChargeCancel( n2 nSlotIndex );
	void RecvChargeCancelResult();

	// 충전 완료 아이템 회수 / 결과
	void SendChargeItemWithdraw( n2 nSlotIndex );
	void RecvChargeItemWithdrawResult();

	// 충전 목록 잠금 해제 / 결과
	void SendUnlockChargeList( n2 nSlotIndex );
	void RecvUnlockChargeListResult();
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// Daily Check Event
	void SendDailyCheckEventRequest();
	void RecvDailyCheckEventResult();
	void SendDailyCheckEventItemGet(n4 nGroupID);
	void RecvDailyCheckEventItemGetResult();
	//////////////////////////////////////////////////////////////////////////

	// 전체 퀘스트 정보 받아서 갱신
	void RecvQuestAllInfo();

	//////////////////////////////////////////////////////////////////////////
	void SendChangeDigimonEffect( u2 nItemPos, u4 nItemType, n2 nItemCount );
	void RecvChangeDigimonEffect();
	void NTF_DigimonEffectChange();

};

