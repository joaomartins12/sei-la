#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class AdaptTutorialQuest;
class AdaptMacroProtect;

//-------------------------------------------------------------------------------------------------------------------------------------
#define TRADE_DISTANCE	1500.0f	// 거래 가능한 거리
//-------------------------------------------------------------------------------------------------------------------------------------
class CTradeContents
	: public IUIContentSubject< CTradeContents >
{
	struct sTradeInfo
	{
		//friend class CTradeContents;
		struct sINFO
		{
			sINFO():s_nInvenIndex(-1)
			{};

			~sINFO(){};

			void		Reset()
			{ 
				s_Item.Clear(); 
				s_nInvenIndex = -1; 
				s_nItemType = 0;
				s_nItemCount = 0;
			}

			cItemData		s_Item;
			int				s_nInvenIndex;
			u4				s_nItemType;
			u4				s_nItemCount;
		};

	public:
		sTradeInfo();
		~sTradeInfo();

		void	Reset();
		bool	ItemRegist( cItemData const& registItem, n1 const& TradeInvenSlotNum, u2 const& InvenSlotNum, bool bIsMyItemRegist );
		void	UnRegistItem( int const& nSlot );
		void	ChangeMoney(u8 const& nChangeMoney);
		void	SetInvenLock( bool bLock );
		void	RemoveTradeItemToInven( cData_Inven * pInven );
		void	AddTradeItemToInven( cData_Inven * pInven );
		void	SetStandBy(bool bValue);

		u8		GetTradeMoney() const;
		bool	IsInvenLock() const;
		bool	HaveEmptySlot() const;
		int		GetEmptySlotNum() const;
		bool	IsEmptySlot(int const& nSlotNum) const;
		bool	IsStandBy() const;

		cItemData const * GetItemData( int const& nSlotNum ) const;

	protected:
		std::vector<sINFO>		m_vecItems;		// 거래 아이템
		u8						m_Money;		// 거래 금액
		bool					m_bInvenLock;	// 거래 인벤 잠금/해제
		bool					m_bStandBy;		// 거래 완료 대기 상태
	};	

public:
	enum eTrade_EventType
	{
		eChange_Inven_Lock,
		eTrade_StandBy,

		eMyTrade_Money_Change,
		eMyTradeItem_Regist,
		eMyTradeItem_UnRegist,

		eTargetTrade_Money_Change,
		eTargetTradeItem_Regist,
		eTargetTradeItem_UnRegist,

		eAdd_Trade_Chat_Msg,
	};

	static int const IsContentsIdentity(void);

public:
	CTradeContents(void);
	virtual ~CTradeContents(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

public:
	bool		IsTrading() const; // 거래 중인지 체크
	void		SendTradeCompleteFinal();	// 최종 승인 패킷.
	void		SendTradeMyRegistItemChange(int const& nInvenPos, int const& nCount);	// 거래 아이템 변경 패킷
	std::wstring GetMyName();
	std::wstring GetTradeTamerName();

	u8			GetMyTradeMoney() const;
	u8			GetTradeTamerMoney() const;

	bool		IsMyTradeInvenLock() const;
	bool		IsTradeTamerInvenLock() const;

	void		SendTradeCancel(void);	// 거래 취소
	void		SendTradeInvenLock(bool bLock);	// 거래 인벤 잠금/해제
	void		SendTradeFinal();	// 최종 거래 승인
	void		SendMyTradeItemRegist( CURSOR_ICON::eTYPE eIconType, int const& nInvenPos, int const& nCount );
	void		SendMyTradeItemUnRegist( int const& nDelSlotNum );

	cItemData const * GetMyRegistItemData(int const& nSlot) const;
	cItemData const * GetTargetRegistItemData(int const& nSlot) const;

	uint		GetTradeTamerUID() const;

private:
	void		ResetData(void);
	void		CloseTrade(void);
	void		RequestTrade( CsC_AvObject* pTarget );
	void		SetTradeTargetObjectIdx( uint const& nTargetIdx );
	bool		CheckIsRegistAble( int const& nInvenSlotNum, int & nCount, int & nItemCode );
	bool		IsMacroProtecting() const;

private:
	void		Send_Trade_Request_Name(void* pData);
	void		Send_Trade_Request_UIDX(void* pData);
	void		Send_Trade_Request_Cancel(void* pData);
	void		Send_TradeRequest_Result( void* pData );
	void		Send_Trade_MoneyChange(void* pData);	// 거래 금액 변경 패킷
	void		Send_TradeItem_Regist( void* pData );

	void		Recv_TradeReceiveRequest(void* pData);// 거래 요청 받았을 때 패킷 처리
	void		Recv_TradeRequest_Result(void* pData);

	void		Recv_TradeItemRegist(void* pData);
	void		Recv_TradeItemUnregist(void* pData);
	void		Recv_TradeMoneyChange(void* pData);

	void		Recv_TradeInvenLock_UnLock(void* pData);	// 거래 인벤토리 창 잠금/해제
	void		Recv_Trade_StandBy(void* pData);			// 거래 완료 대기중
	void		Recv_TradeFinal_Result(void* pData);		// 최종 거래 완료

	void		TradeChatMsg(void* pData);


private:
	uint			m_TradeTargetObjectUIDX;	// 거래 대상의 인덱스

	sTradeInfo		m_MyTradeData;
	sTradeInfo		m_TargetTradeData;

	TOwnershipGuard< AdaptTutorialQuest >	m_AdaptTutorialQuest;
	TOwnershipGuard< AdaptMacroProtect >	m_AdaptMacroProtect;	
};
