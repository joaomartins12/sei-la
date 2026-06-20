#pragma once

#include "../../ContentsSystem/ContentsSystem.h"

//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
class CItemProductionContents
	: public IUIContentSubject< CItemProductionContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eEventType
	{
		eOpen_Window,		// 제작창 오픈
		eClose_Window,		// 제작창 닫기
		eUpdate_ItemCount,	// 재료 아이템 갯수 갱신
		eUpdate_Money,		// 금액 갱신
		eSetAssistItem,		// 보조 재료 아이템 변경
		eRecvMakeItem,		// 
	};

public:
	CItemProductionContents(void);
	virtual ~CItemProductionContents(void);

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


	//bool						SendMakingItem( int const& nMCIdx, int const& nSCIdx, int const& nUniqueIdx, int const& nMakeCount );

	// 아이템을 제작할 수 있는지 검사하는 함수
	bool						StartMakeingProgress( int const& nMCIdx, int const& nSCIdx, int const& nUniqueIdx, int const& nMakeCount, DWORD const& dwPercentageItemCode, DWORD const& dwProtectItemCode );

	void						closeWindow();

	CsItem::sINFO const*		GetItemInfo( DWORD const& dwItemIdx );
	sProductionItemList const*	GetMaterialItemList( const int & nMainCategotyIdx, const int & nSubCategotyIdx, int const& dwUniqueIdx) const;
	LIST_ProductionList const*	GetProductionList( const int & nMainCategotyIdx, const int & nSubCategotyIdx ) const;
	MAP_Main_Categoty const*	GetItemProductionList() const ;
	CsC_AvObject*				GetTargetNpc() const { return m_pTarget; }
	DWORD						GetHaveItemCount( DWORD const& dwItemIdx ) const;

	void						ReSetAssistItem(int const& nAssistType);
	void						InvenItemSlotLock( bool bLock, DWORD const& dwItemCode );
	int							GetAssistItemPercentage(int const& nGroupID, DWORD const& dwItemCode) const;
	bool						IsAssistItem( int const& nGroupID, DWORD const& dwItemCode ) const;

	
	bool						SendMakeItemData();
	void						StopMakeItemProgress();	

private:
	//void						SendMakingItemRun(void* pData);


	void						RecvMakingItemResult(void* pData);

	void						openItemProductionWindow( void* pData );
	void						closeItemProductionWindow( void* pData );
	void						UpdateItemCount( void* pData );
	void						UpdateMoney( void* pData );
	void						InitContentsData(void);
	void						SetAssistItem(void* pData);
	void						_SetInvenItemLockAUnLock(bool bLock, 
														 sProductionItemList const* pItemInfo, 
														 DWORD const& dwMakeItemCount, 
														 DWORD const& dwPrecentageItemCode, 
														 DWORD const& dwProtectItemCode );

	void						_UnLock_MakeWait_InvenItem();
	bool						_CheckCanMakeItem( sProductionItemList const* pItemInfox, int const& nMakeCount, 
												   DWORD const& dwPercentageItemCode, DWORD const& dwProtectItemCode );

private:
	int									m_ItemMakeCount;
	bool								m_bWaitRecvMakeItem;	// 서버에서 제작결과 패킷을 받았는지 여부
	DWORD								m_dwProtectItemIdx;
	DWORD								m_dwPercentageItemIdx;

	sProductionItemList const*			m_pItemInfo;
	MAP_Main_Categoty const*			m_ItemProductionTable;
	DWORD								m_dwTargetNpcIdx;
	CsC_AvObject*						m_pTarget;
};
