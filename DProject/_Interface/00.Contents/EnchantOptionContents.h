#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class cEnchantOptionContents : public IUIContentSubject< cEnchantOptionContents >
{
public:
	cEnchantOptionContents();
	virtual ~cEnchantOptionContents();

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
		eIdentifyItem,
		eRegistEnchantItem,
		eRegistEnchantStone,
		eStartEnchant,
		eSuccessEnchant,
	};

	enum eEnchantUI
	{
		eEnchantUI_None,
		eEnchantUI_Identify,
		eEnchantUI_Enchant,
	};

	enum eEnchantState
	{
		eES_None,
		eES_Processing,		// 감정/강화 중
		eES_WaitRecv,		// 서버 패킷 대기중
	};

	enum eEnchantType
	{
		eAT_None,
		eAT_Accessory,
		eAT_Enchant,
	};

private:
	struct sIdentifyItem
	{
		sIdentifyItem() : nInvenIndex( INT_MAX ) {}
		~sIdentifyItem() {}

		void			ResetData();
		void			SetAccessory( cItemInfo* pItem, uint nIndex );

		cItemInfo		cIdentifyItem;		// 감정할 아이템
		uint			nInvenIndex;		// 아이템 인벤 위치
	};

	struct sEnchantItem
	{
		sEnchantItem() : nStoneType( 0 ), nStoneIndex( INT_MAX ), nItemIndex( INT_MAX ), nSelectOption( 0 ) {}
		~sEnchantItem() {}

		bool			IsRegistEnchantItem() const;
		bool			IsRegistEnchantStone() const;

		void			SetEnchantItem( cItemInfo* pItem, uint nIndex );
		void			SetEnchantStone( cItemInfo* pItem, uint nIndex, uint nType );

		void			ResetEnchantItem();
		void			ResetEnchantStone();

		cItemInfo		cEnchantStone;		// 옵션 변경 아이템
		cItemInfo		cEnchantItem;		// 강화할(변경할) 아이템
		uint			nStoneType;			// 옵션 변경 타입
		uint			nStoneIndex;		// 옵변 아이템 인벤 위치
		uint			nItemIndex;			// 강화할 아이템 인벤 위치
		uint			nSelectOption;		// 선택된 옵션 번호
	};

	eEnchantUI		m_eUIState;
	eEnchantState	m_eEnchantState;
	sIdentifyItem	m_sIdentify;
	sEnchantItem	m_sEnchant;

private:
	void				RecvIdentifyItem( void* pData );
	void				RecvEnchantItem( void* pData );
	void				StartEnchantItem( void* pData );

private:
	eEnchantType		_CheckEnchantItemType( uint nItemType );
	void				_SetEnchantItem( cItemInfo* pInvenItem, uint nInvenIndex );
	void				_SetEnchantStone( cItemInfo* pInvenItem, uint nInvenIndex );

	void				_RegistEnchantItem( cItemInfo* pInvenItem, uint nInvenIndex );
	void				_RegistEnchantStone( cItemInfo* pInvenItem, uint nInvenIndex );

public:
	void				RegistEnchantItem( int const& nInvenIndex );

	void				CloseEnchant();

	void				SuccessIdentifyItem();		// 감정 완료 - send packet
	void				SuccessEnchantItem();		// 강화 완료 - send packet

	void				EnchantItem();
	void				SelectOptionValue( uint nSelectIndex );

	bool				ResetEnchantStone();
	bool				ResetEnchantItem();

public:
	bool				IsEnableRegist( uint nInvenIndex, uint nItemCount ) const;
	bool				IsProcessingEnchant() const;
	eEnchantUI			GetUIState() const;
	uint				GetEnchantStoneEnchantType() const;
	uint				GetEnchantStoneItemType() const;
	uint				GetEnchantItemType() const;
	std::wstring		GetEnchantItemTypeName() const;
	uint				GetItemUserUID( uint nItemType ) const;
	const cItemInfo*	GetEnchantStone() const;
	const cItemInfo*	GetEnchantItem() const;
	cItemInfo*			GetInvenItem( uint nInvenIndex ) const;
	CsItem::sINFO*		GetFTItemInfo( uint nItemType ) const;
	CsC_AvObject*		GetEnchantItemEffect() const;
	std::wstring		GetOptionName( ushort nOption ) const;
};