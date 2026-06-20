#pragma once

class AdaptTutorialQuest;

class ItemRewardContents: public IUIContentSubject< ItemRewardContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eEventType
	{
		eREFRESH,		// 새로 고침.
		eFULL_RECEITP,	// 전체 수령.
	};

public:
	ItemRewardContents(void);
	virtual ~ItemRewardContents(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void ClearWorldData(void);

private:
	GIFT_ITEM_LIST   mGiftData;
	REWARD_ITEM_LIST mRewardData;

	void	Reward_ToggleCheck(void* pData);	
	void	ReceiveGift(void* pData);
	void	ReceiveReward(void* pData);
	void	ReceiveSendAccpetData(void* pData);
	
public:
	void				Refresh();		// 새로고침
	void				FullReceipt();	// 전체수령
	void				CloseItemRewardUI();

	GIFT_ITEM_LIST*		GetGiftData();
	cItemInfo*			GiftItem(int nIndex);
	int					GetCurrentGiftSize();	
	void				DeleteGiftItem(int idx);
	void				SendCropGiftItem(int nIndex);
	void				SendAllGiftItem();

	REWARD_ITEM_LIST*	GetRewardData();
	CsReward*			GetReward(int nFTID);
	int					GetCurrentRewardSize();
	void				DeleteRewardItem(int idx);
	void				SendRequestRewards(u1 nReqCnt, u1 nReqIdx);
	void				SendRequestRewardAll();

private:
	TOwnershipGuard< AdaptTutorialQuest >	m_AdaptTutorialQuest;
};