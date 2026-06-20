#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class cGotchaMachineContents : public IUIContentSubject< cGotchaMachineContents >
{
public:
	enum eEventType
	{
		eRefresh,
		eSetEnable,
	};


public:
	cGotchaMachineContents();
	virtual ~cGotchaMachineContents();

	static int const IsContentsIdentity(void);

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

public:
	enum eMachineState{	NONE, WAIT_RECV, RUN, RUN_RARE, };
	enum eMachineType
	{ 
		GOTCHA = 0,
		MYSTERY_NORMAL = 1,		// Mystery_Coin - Group section min 이랑 동일하게 해둠
		MYSTERY_EGG3 = 100,
		MYSTERY_EGG4 = 200,
		MYSTERY_EGG5 = 300,
	};
	enum eShowFlag
	{
		allhide = 0,		//모두 가림
		nonehide= 1,		//가리지 않음
		rarehide = 2,		//레어아이템 가림
		normalhide = 3,		//총량 가림
		mystery,			//미스테리 머신
	};
	struct sUseItemInfo
	{
		sUseItemInfo() : nUseItemType( 0 ), nUseItemCount( 0 ), nUsePrice( 0 )	{}
		~sUseItemInfo() {}

		void	Set( uint nItemType, uint nItemCount, u8 nPrice = 0 );

		uint	nUseItemType;
		uint	nUseItemCount;
		u8		nUsePrice;
	};

private:
	CNpc*		m_pNpc;
	cItemInfo	m_GotchaItem;			// 가챠로 획득한 아이템
	std::map< int, cItemInfo >		m_mapRareItems;		// 가챠로 얻을 수 있는 레어 아이템
	std::map< int, sUseItemInfo >	m_mapUseItems;		// 필요 아이템 idx, item
	std::map< int, int >			m_mapUseInvenSlot;	// 인벤에서 사용될 아이템 slot, count
	uint		m_nResetCount;			// 초기화 횟수
	uint		m_nNormalItemCount;		// 일반 아이템 개수
	uint		m_nUseItemType;			// 필요 아이템의 id
	uint		m_nUseItemSlot;			// 필요 아이템의 인벤 위치
	uint		m_nRemainItemCount;		// 뽑기 아이템 남은 수량
	u8			m_nUseMoney;			// 뽑기에 필요한 금액 - 수수께끼 머신(알)

	eMachineState	m_eState;
	eMachineType	m_eType;
	eShowFlag		m_eShowUI;

private:
	void		OpenMysteryMachine( void* pData );		// 수수께끼머신 오픈

	void		SendOpenGotchaMachine( void* pData );	// 레어머신 오픈 요청
	void		RecvOpenGotchaMachine( void* pData );	// 레어머신 오픈 결과

	void		SendGetItemMachine( void* pData );
	void		RecvGetItemGotchaMachine( void* pData );	// 레어머신 뽑기 결과
	void		RecvGetItemMysteryMachine( void* pData );	// 수수께끼머신 뽑기 결과
	void		RecvErrorGotchaMachine( void* pData );

	void		CloseMachine( void* pData );
	void		GetGotchaItem( void* pData );

	void		_UpdateGotchaMachine( std::list< cItemData >& lRareItems, int nResetCount, int nNormalItemCount );
	void		_UpdateUseItemGotchaMachine( uint nNpcID );
	void		_UpdateUseItemMysteryMachine();
	void		_UpdateInventory();

	void		_DecreaseItemGotcha();
	void		_DecreateItemMystery();

	bool		_CheckEnableGotchaMachine();
	bool		_CheckEnableInvenItem();

	void		_OpenMachineUI();
	void		_ResetMachineData();
	void		_SetEnableWindowUI( bool bEnable );
	bool		_SetMachineAni( bool bIsRare );

	bool		_SendGetItemGotcha();
	bool		_SendGetItemMystery();

public:
	void			StartGotcha();
	void			CloseMachineUI();

	eShowFlag		GetShowFlag() const;
	eMachineType	GetMachineType() const;
	int				GetRemainItemCount() const;
	int				GetRareItemCountAll() const;
	cItemInfo*		GetRareItem( int nIndex );
	std::wstring	GetMachineName() const;
	std::wstring	GetMysteryItemName() const;
	int				GetUseItemCountAll() const;
	int				GetUseInvenCount( uint nItemType ) const;
	sUseItemInfo*	GetUseItem( int nIndex );
	u8				GetEnableUseMoney() const;

	bool			IsGotchaPlaying() const;
};