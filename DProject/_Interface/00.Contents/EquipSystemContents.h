#pragma once
#include "../../ContentsSystem/ContentsSystem.h"

class cEquipSystemContents : public IUIContentSubject< cEquipSystemContents >
{
public:
	cEquipSystemContents();
	~cEquipSystemContents();

	static int const IsContentsIdentity(void);

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
	enum eEventType
	{
		eRegistCheckItem,
		eRegistEquip,
		eRegistElem,
		eSelectSocket,
		eReleaseSocket,
		eSuccessAnalysis,
		eSuccessAttach,
		eSuccessDetach,
		eResetWindow,
//////////////////////////////////////////////////////////////////////////
		eRegistChargeItem,
		eRegistChargeItem_Quick,
		eRegistQuickCharge,
		eUpdateChargeList,
		eUpdateChargeTime,
		eNormalChargeItem,
		eQuickChargeItem,
		eWithdrawItem,
		eChargeCancel,
		eUnlockSlot,
		eOpenHelp,
	};

	enum eUIState
	{
		eNoneState,
		eProcessing,
		eWaitRecvData,
	};

	enum eTabState
	{
		eTabNone,
		eTabNormal,
		eTabQuick,
		eTabElem,
	};

	enum eRegistState { eEquip, eElem, eCharge, eNotRegist };
	enum eSocketState { eAttached, eRegist, eNoneRegist };	// 합성, 등록, 미등록

private:
	CNpc*			m_pNpc;
	int				m_nPortableIndex;

	/////////////////////////////////////////////////////
	// Element Check
private:
	struct sElementCheck
	{
		sElementCheck() : nRegistItemIdx(INT_MAX), nNeedMoney(0), eState(eNoneState){}
		~sElementCheck() {}

		void				Reset();
		void				ResetData();
		bool				IsRegist() const;

		uint				nRegistItemIdx;	// 등록한 아이템의 인벤 인덱스
		u8					nNeedMoney;
		eUIState			eState;
	};
	sElementCheck	m_sElemCheck;

	/////////////////////////////////////////////////////
	// Element Equip
public:
	struct sSocketInfo
	{
		sSocketInfo()
			: nInvenIndex( -1 ), nItemType( 0 ), nItemRate( 0 ), eState( eSocketState::eNoneRegist ) {}
		~sSocketInfo() {}

		void Reset()
		{
			nItemType = 0;
			nItemRate = 0;
			nInvenIndex = -1;
			eState = eSocketState::eNoneRegist;
		}
		void Set( eSocketState eSocket, uint nType, u1 nRate, int nIndex )
		{
			eState = eSocket;
			nItemType = nType;
			nItemRate = nRate;
			nInvenIndex = nIndex;
		}

		int				nInvenIndex;
		uint			nItemType;
		u1				nItemRate;
		eSocketState	eState;
	};

private:
	struct sElementEquip
	{
		sElementEquip() : nRegistItemIdx(INT_MAX), eState(eNoneState),
			nNeedAttachMoney(0), nNeedDetachMoney(0), nSelectedSocket(-1), nEmptyInvenIndex(-1), nWaitRecvAttach(0) {}
		~sElementEquip() {}

		void				Reset();
		void				ResetData();
		bool				IsRegist() const;

		uint				nRegistItemIdx;	// 등록한 아이템의 인벤 인덱스
		eUIState			eState;

		std::map< int, sSocketInfo >	mapSocketItems;		// socket idx, socket info
		u8					nNeedAttachMoney;
		u8					nNeedDetachMoney;
		int					nSelectedSocket;
		int					nEmptyInvenIndex;			// 해제된 아이템이 들어갈 인벤 위치
		int					nWaitRecvAttach;			// 받아야 할 패킷 개수
	};
	sElementEquip	m_sElemEquip;

////////////////////////////////////////////////////////////////////////////////////

private:
	void			AnalysisElementItem( void* pData );
	void			RecvElementCheckSuccess( void* pData );
	void			RegistElementCheckItem( void* pData );

private:
	bool			_CheckEnableRegist_Check( int const& nInvenIndex );

public:
	void			SetRegistAnalysis( int const& nInvenIndex );

	bool			StartAnalysis();
	void			EndAnalysis();
	bool			CancelAnalysis();

////////////////////////////////////////////////////////////////////////////////////

private:
	void			OpenElementItemNpc( void* pData );
	void			OpenElementItemPortable( void* pData );

public:
	void			CloseElementItem( void* pData );

private:
	bool			_CheckEnableMoney( u8 const& nNeedMoney );
	bool			_CheckEnableItem( int const& nInvenIndex );

	u8				_CalcNeedMoney( uint nItemType, bool bIsDetach = false );
	sMONEY			_DecreaseInvenMoney( u8 const& nNeedMoney );
	int				_GetEmptyInvenSlot();
	void			_SetLimited( cItemInfo* pRegistItem );

////////////////////////////////////////////////////////////////////////////////////
public:
	void			ResetRegistItem_Check();
	void			ResetRegistItem_Equip();
	cItemInfo*		GetRegistItem_Check();
	cItemInfo*		GetRegistItem_Equip();
	CsItem::sINFO*	GetFTItemInfo( int const& nItemType );
	cItemInfo*		GetInvenItem( int const& nInvenIndex );
	cItemInfo*		GetInvenItem_NotCheckEnable( int const& nInvenIndex );
	uint			GetInvenItemType( int const& nInvenIndex );

////////////////////////////////////////////////////////////////////////////////////

private:
	void			RecvElementAttachSuccess( void* pData );
	void			RecvElementDetachSuccess( void* pData );

public:
	void			RegistElementAttachItem( void* pData );

private:
	eRegistState	_CheckRegistItemKind( int const& nInvenIndex );

	bool			_CheckEnableEquip( CsItem::sINFO* pFTInfo );
	bool			_CheckEnableElement( u2 nElemTypeL, u1 nElemRate );
	bool			_CheckEnableSocket( int const& nSocketIndex );
	bool			_CheckReleaseAllSocket();

	void			_LoadAttachedSocket( int nSocketCount, cItemInfo* pInvenItem );
	int				_GetSocketEmptySlot();

public:
	void			AttachElementItem();
	void			DetachElementItem();
	void			SelectAttachedSocket( int const& nSocketIndex );
	bool			SetRegistAttach_Equip( int const& nInvenIndex );
	bool			SetRegistAttach_Elem( int const& nInvenIndex );
	void			ReleaseRegistSocket( int const& nSocketIndex );

	sSocketInfo*	GetSocketInfo( int const& nSocketIndex );
	cItemInfo		GetSocketItem( int const& nSocketIndex );
	std::string		GetSlotImagePath();

////////////////////////////////////////////////////////////////////////////////////
// 장비 시스템 개선
////////////////////////////////////////////////////////////////////////////////////
private:
	eTabState		m_eCurTab;

private:
	void			OpenEquipSystemNpc( void* pData );
	void			OpenEquipSystemPortable( void* pData );

public:
	void			RegisterItemEquipSystem( int const& nInvenIndex );
	void			RegisterItem_ElementManage( int const& nInvenIndex );
	void			RegisterItem_NormalCharge( int const& nInvenIndex );
	void			RegisterItem_QuickCharge( int const& nInvenIndex );

private:
	void			_RegistChargeItem( int const& nInvenIndex );
	void			_RegistQuickItem( int const& nInvenIndex );
	void			_LockInventory();
	void			_UnlockInventory();
	bool			_CheckWaitReceivePacket();

public:
	void			CloseEquipSystem( void* pData );
	bool			SetTabState( eTabState eTab );
	void			HelpWindow();
////////////////////////////////////////////////////////////////////////////////////
	// 장비 충전
public:
	struct sChargeItem
	{
		sChargeItem() : nKeepMoney(0), nKeepTime(0), nChargeTime(0), nInvenIndex(INT_MAX),
			eState(eUIState::eNoneState) {}
		~sChargeItem() {}

		bool		IsEmptySlot() const;
		bool		IsComplete() const;
		bool		IsRegist() const;
		uint		GetItemType() const;
		void		SetItemInfo( cItemInfo* pItem, int nIndex );
		void		SetChargeInfo( uint nCTime, uint KTime, u8 nKMoney );
		void		Reset();

		cItemInfo	cChargeItem;	// 충전할 아이템 정보
		u8			nKeepMoney;		// 보관 비용
		uint		nKeepTime;		// 보관 시간 - 장비 충전에 소요되는 시간
		uint		nChargeTime;	// 장비 충전 시간
		uint		nInvenIndex;	// 등록할 아이템의 인벤 인덱스
		eUIState	eState;			// 현재	슬롯 상태 - 충전 중인지 아닌지
	};

	struct sQuickChargeItem
	{
		sQuickChargeItem() : nItemIndex(INT_MAX), nCashCode(0),
			nQuickItemType(0), nQuickUseCnt(0), nTimeChargeIndex(0) {}
		~sQuickChargeItem() {}

		uint		GetItemType() const;
		uint		GetItemCount() const;
		bool		IsRegistChargeItem() const;
		bool		IsRegistQuickItem() const;
		void		Reset();
		void		ResetQuickItem();
		void		ResetData();

		cItemInfo	cChargeItem;		// 충전할 아이템
		uint		nItemIndex;			// 충전할 아이템 인벤 위치
		int			nCashCode;			// 충전 일별 아이템 필요 개수 찾는 코드 번호

#if COMMON_LIB_FIXED
		std::list< nsTimeCharge::sChargeItemInfo > lQuickItems;
#endif

		uint		nQuickItemType;		// 충전 아이템 타입
		uint		nQuickUseCnt;		// 충전 아이템 사용 개수
		int			nTimeChargeIndex;	// 충전 일수 인덱스 1~
	};

private:
	std::map< int, sChargeItem >	m_mapChargeList;		// 장비 충전 목록 slot idx, charge item
	sQuickChargeItem				m_sQuickChargeItem;		// 시간 단축 아이템
	int								m_nSelectChargeSlot;	// 충전하려는 아이템 슬롯 번호
	bool							m_bWaitRecvPacket;		// 패킷 대기중
	float							m_fDeltaTime;
	NiPoint2						m_niOpenPos;			// 창 오픈한 위치

private:
	int				_GetEnableKeepIndex();

#if COMMON_LIB_FIXED
	void			_UpdateChargeList( n2 const& nSlotCnt, std::list< nsTimeCharge::sChargingInfo >& lCharingList );
#endif

	void			_UpdateChargeTime();

private:
	void			SendRequestChargeList();					// 충전 목록 요청
	void			RecvChargeListResult( void* pData );
	bool			SendNormalChargeTime();						// 일반 충전
	void			RecvNormalChargeTimeResult( void* pData );
	void			SendQuickChargeTime();						// 즉시 충전
	void			RecvQuickChargeTimeResult( void* pData );
	void			SendUnlockChargeList( void* pData );		// 충전 목록 잠금 해제
	void			RecvUnlockChargeListResult( void* pData );
	void			SendWithdrawChargeItme( void* pData );		// 충전 완료 아이템 회수
	void			RecvWithdrawChargeItemResult( void* pData );
	void			SendNormalChargeCancel( void* pData );		// 충전 취소
	void			RecvNormalChargeCancel( void* pData );

public:
	bool			NormalCharge( int const& nSlotIndex );	// 일반 충전
	void			CancelCharge( int const& nSlotIndex );	// 충전 취소
	void			WithdrawItem( int const& nSlotIndex );	// 충전 완료 아이템 회수

	void			QuickCharge();							// 즉시 충전
	void			UnlockChargeList( int nSlotIndex );		// 충전 목록 잠금 해제
	void			SelectQuickChargeItemCount( int nSelectIndex );		// 충전 아이템 개수 선택

	bool			ResetChargeItem();
	bool			ResetQuickItem();
	bool			ResetChargeListItem( int const& nSlotIndex );

	sChargeItem const*	GetChargeItem( int const& nSlotIndex ) const;
	uint				GetChargeTime( uint nItemType ) const;
	uint				GetUnlockMoney( int const& nSlotIndex ) const;
	int					GetChargeTimeCount() const;
	int					GetQuickChargeCountByIndex( int const& nIndex ) const;
};