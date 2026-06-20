#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class AdaptBattleSystem;
class AdaptTutorialQuest;

class CashShopContents
	: public IUIContentSubject< CashShopContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eEventType
	{
		eRefresh_Data,
		eRefresh_CashMoney,
	};
public:
		CashShopContents(void);
		virtual ~CashShopContents(void);
public:
	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void ClearWorldData(void);

	void		CashShop_Refresh();
	void		OpenCashChargeUrl();

	int			GetCashType();
	//////////////////////////////////////////////////////////////////////////
	// 캐쉬 아이템 구매 가능 시간 체크
	//////////////////////////////////////////////////////////////////////////
	bool		IsItemPurchaseTime( CsCashShop* pItemInfo );

	int 		GetCashMoney( int const& nCashType ) const;
	int 		GetHaveTotalCash() const;
	void		SetCashMoney( int const& nCashType, int const& nMoney );

private:
	void		InitCashMoney();
	void		Cash_ToggleCheck(void* pData);		//2017-03-28-nova
	void		CashShop_ToggleCheck(void* pData);
	void		Callback_CashItemPurchase_Steam(void* pData);
	void		Recv_RequestCashItemPurchase_Steam(void* pData);
	void		Recv_Result_ItemPurchase(void* pData);
	void		Recv_Result_CashRefresh(void* pData);

	void		SendCashItemPurchase(void* pData);
	void		SendCashItemPurchase(DWORD const& dwTbIdx);
	
	void		Request_CashItemPurchase_Steam(DWORD const& dwTbIdx);

	// 캐쉬 아이템 구매시 구매 가능 시간 체크하여
	// 시간이 지난 경우 캐쉬샵UI를 닫는다.
	bool		_CheckItemPurchaseTime( CsCashShop* pItemInfo );

private: 
	bool		m_bUpdateCashInfo;
	DWORD		m_dwSteamTmpTableIdx;	// 스팀용 임시 변수(아이템 구매 시 아이템 테이블 번호를 임시로 저장)
	TOwnershipGuard< AdaptBattleSystem >	m_AdaptBattleSystem;
	TOwnershipGuard< AdaptTutorialQuest >	m_AdaptTutorialQuest;

	int		m_nCashMoney[ CsCashShopMng::CT_MAXCNT ];
};