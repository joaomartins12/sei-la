#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

class AdaptTutorialQuest;

class cInventoryContents : public IUIContentSubject< cInventoryContents >
{
public:
	enum eEventType
	{
		eEnableWindow,
		eFiltering,
		eAdd_GiftBoxWindow,
		eOpenChangeTamer,
		eChangeDiscript,
	};
	
	enum eFilterMode
	{
		eNone,
		eEquip,
		eConsume,
		eDigimon,
		eEtc
	};

	enum eStateUI
	{
		eOpenUI,
		eCloseUI
	};

private:
	eFilterMode					m_eCurFilter;
	std::map< int, int >		m_mapFilterItem;
	std::map< int, bool >		m_mapNewItem;

	eStateUI					m_eState;

	int							m_nPreRankNo;
#ifdef UI_INVENTORY_RENEWAL
	bool						m_bRecvInvenInfo;
#endif

public:
	cInventoryContents();
	virtual ~cInventoryContents();

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

	/////////////////////////////////////////////////////////////////////
	// Event Functions
private:
	void		OpenInventory( void* pData );
	void		CloseInventory( void* pData );
	void		SetEnableInventory( void* pData );
	void		GetPosDependOnInven( void* pData );
	void		SetGiftBoxWindow( void* pData );
	void		UpdateGiftBox( void* pData );
#ifdef UI_INVENTORY_RENEWAL
	void		AddNewItem( void* pData );
	void		RecvSortEnd( void* pData );
	void		RecvInvenReqResult( void* pData );
	/////////////////////////////////////////////////////////////////////
	void		_UpdateInventory( std::list<cItemData>& lItems );
	void		_CheckUserQuickSlot();
#endif

public:
	void				CloseWindow();
	void				CloseGiftBoxWindow();

	void				UseItem( int const& nInvenIndex );

	CsItem::sINFO*		GetFTItemInfo( uint	nItemType ) const;
	cItemInfo*			GetInvenItem( int nInvenIndex ) const;
	cItemInfo*			GetEquipItem( int nIndex ) const;
	uint				GetInvenSlotCount() const;
#ifdef UI_INVENTORY_RENEWAL
	const int			GetFilteringItemIdx( int nIndex ) const;

	void				SetNewItem( int nInvenIndex, bool bIsNew );
	void				SetFilterMode( eFilterMode eMode );

	bool				IsEqualItemType( eFilterMode eMode, int const& nType ) const;
	bool				IsOpenInventory() const;
	bool				IsMapNewItem() const;
	bool				IsNewItem( int nInvenIndex );
	bool				IsFilteringItem( int nInvenIndex );
	bool				IsPlayingTutorial() const;

	void				SendSortStart( nItemSort::eSortFlag eFlag );

private:
	TOwnershipGuard< AdaptTutorialQuest >	m_AdaptTutorialQuest;
#endif

	//////////////////////////////////////////////////////////////////////////
	// Change Tamer
private:
	void				RequestUsingChangeTamer( void* pData );
	void				OpenChangeTamer( void* pData );
	void				SendChangeTamer( void* pData );

private:
	bool				_CheckEquipments();
	void				_GetChangeTamerList( USHORT const& nItemTypeS, std::list< int >& lChangList );

public:
	void				SelectChangeList( int nTamerID );
	void				ChangeTamer( int const& nInvenIndex, int const& nTamerID );
	void				CloseChangeWindow( int const& nInvenIndex );

public:
	std::string			GetTamerIamgeS( int const& nTamerID ) const;
	std::wstring		GetTamerName( int const& nTamerID ) const;
	std::wstring		GetTamerGroup( int const& nTamerID ) const;
	//////////////////////////////////////////////////////////////////////////
};