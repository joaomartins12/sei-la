#include "StdAfx.h"
#include "ItemProduction_Contents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"


//-------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------
int const CItemProductionContents::IsContentsIdentity(void)
{
	return E_CT_ITEMPRODUCTION;
}

CItemProductionContents::CItemProductionContents(void):m_ItemProductionTable(NULL), m_pItemInfo(NULL), m_ItemMakeCount(0),
		m_dwTargetNpcIdx(0),m_pTarget(NULL),m_dwProtectItemIdx(0),m_dwPercentageItemIdx(0),m_bWaitRecvMakeItem(false)
{	
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::OPEN_ITEM_PRODUCTION_SHOP, this, &CItemProductionContents::openItemProductionWindow );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::CLOSE_ITEM_PRODUCTION_SHOP, this, &CItemProductionContents::closeItemProductionWindow );

	//GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SEND_ITEM_PRODUCTION_RUN, this, &CItemProductionContents::SendMakingItemRun );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_ITEM_PRODUCTION_RESULT, this, &CItemProductionContents::RecvMakingItemResult );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::SET_ITEM_PRODUCTION_ASSIST_ITEM_REG, this, &CItemProductionContents::SetAssistItem );
}

CItemProductionContents::~CItemProductionContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
	InitContentsData();
}

int const CItemProductionContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CItemProductionContents::Initialize(void)
{
	return true;
}

void CItemProductionContents::UnInitialize(void)
{
}

bool CItemProductionContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void CItemProductionContents::Update(float const& fElapsedTime)
{
}

void CItemProductionContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CItemProductionContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

void CItemProductionContents::MakeMainActorData(void)
{
}

void CItemProductionContents::ClearMainActorData(void)
{
}

void CItemProductionContents::ClearWorldData(void)
{
	InitContentsData();
	m_bWaitRecvMakeItem = false;
}

void CItemProductionContents::MakeWorldData(void)
{
	m_bWaitRecvMakeItem = false;
	InitContentsData();
}

void CItemProductionContents::InitContentsData(void)
{
	m_dwTargetNpcIdx = 0;
	m_ItemProductionTable = NULL;
	m_pTarget = NULL;
	m_pItemInfo = NULL;
	m_dwProtectItemIdx = 0;
	m_dwPercentageItemIdx = 0;

	cMessageBox::DelMsg( cPrintMsg::GAME_ITEM_MAKING, false );
}

MAP_Main_Categoty const* CItemProductionContents::GetItemProductionList() const
{
	return m_ItemProductionTable;
}

void CItemProductionContents::openItemProductionWindow( void* pData )
{
	SAFE_POINTER_RET( pData );

	CsC_AvObject* pTargetObject = static_cast<CsC_AvObject*>(pData);
	assert_cs( pTargetObject->GetLeafRTTI() == RTTI_NPC );
	// NPC 타입에 따라 아이템 제작 리스트를 만들고
	// 윈도우를 보여준다.

	DWORD dwTargetObjIdx = pTargetObject->GetFTID();
	if( m_dwTargetNpcIdx == dwTargetObjIdx )
		return;

	m_ItemProductionTable = nsCsFileTable::g_pItemMng->GetItemProductionCategotyTable( dwTargetObjIdx );
	SAFE_POINTER_RET( m_ItemProductionTable );

	// 창을 생성하고 보여 주지는 않는다.
	cBaseWindow* pBaseWin = g_pGameIF->GetDynamicIF( cBaseWindow::WT_ITEM_PRODUCTION_SHOP, 0, false, false );
	SAFE_POINTER_RET( pBaseWin );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::CHANGE_ITEM_COUNT, this, &CItemProductionContents::UpdateItemCount );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::CHANGE_HAVE_MONEY, this, &CItemProductionContents::UpdateMoney );

	m_pTarget = pTargetObject;
	m_dwTargetNpcIdx = dwTargetObjIdx;
	// ZoomNpc
	g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_ITEM_PRODUCTION_SHOP );	

	Notify( eOpen_Window );
}

void CItemProductionContents::closeItemProductionWindow( void* pData )
{
	closeWindow();
}

void CItemProductionContents::UpdateItemCount( void* pData )
{
	SAFE_POINTER_RET(pData);
	DWORD dwItemIdx = *static_cast<DWORD*>(pData);
	DWORD dwItemCount = GetHaveItemCount( dwItemIdx );

	ContentsStream kSteam;
	kSteam << dwItemIdx << dwItemCount;

	Notify(eUpdate_ItemCount, kSteam);
}

void CItemProductionContents::UpdateMoney( void* pData )
{
	ContentsStream kSteam;
	Notify(eUpdate_Money, kSteam);
}

void CItemProductionContents::closeWindow()
{
	GAME_EVENT_STPTR->DeleteEvent( EVENT_CODE::CHANGE_ITEM_COUNT, this );

	GAME_EVENT_STPTR->DeleteEvent( EVENT_CODE::CHANGE_HAVE_MONEY, this );

	InitContentsData();
	if( !m_bWaitRecvMakeItem )
		_UnLock_MakeWait_InvenItem();

	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ITEM_PRODUCTION_SHOP );
}


LIST_ProductionList const*	CItemProductionContents::GetProductionList( const int & nMainCategotyIdx, const int & nSubCategotyIdx ) const
{
	SAFE_POINTER_RETVAL( m_ItemProductionTable, NULL );

	MAP_Main_Categoty_CIT it = m_ItemProductionTable->find( nMainCategotyIdx );
	if( it == m_ItemProductionTable->end() )
		return NULL;

	MAP_Sub_Categoty_CIT subIt = it->second.m_mapSubCategoty.find( nSubCategotyIdx );
	if( subIt == it->second.m_mapSubCategoty.end() )
		return NULL;

	return &(subIt->second.m_listProductionItems);
}

sProductionItemList const* CItemProductionContents::GetMaterialItemList( const int & nMainCategotyIdx, const int & nSubCategotyIdx, int const& dwUniqueIdx ) const
{
	LIST_ProductionList const* pProductionList = GetProductionList( nMainCategotyIdx, nSubCategotyIdx );
	SAFE_POINTER_RETVAL( pProductionList, NULL );

	LIST_ProductionList_CIT it = pProductionList->begin();
	for( ; it != pProductionList->end(); ++it )
	{
		if( (*it).m_nUniqueIdx == dwUniqueIdx )
			return &(*it);
	}

	return NULL;
}

CsItem::sINFO const* CItemProductionContents::GetItemInfo( DWORD const& dwItemIdx )
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, NULL );
	CsItem* pFindItem = nsCsFileTable::g_pItemMng->GetItem( dwItemIdx );
	SAFE_POINTER_RETVAL( pFindItem, NULL );
	return pFindItem->GetInfo();
}


// void CItemProductionContents::SendMakingItemRun(void* pData)
// {
// 	if(m_ItemMakeCount ==0)
// 		return;
// 
// 	SAFE_POINTER_RET( m_pItemInfo);// 아이템 정보를 찾을 수 없다
// 
// 	u8 TotalCost = m_pItemInfo->m_dwItemCost * m_ItemMakeCount;
// 
// 	cData_Inven* pInven = g_pDataMng->GetInven();
// 	SAFE_POINTER_RET( pInven);	// 인벤토리가 없다
// 
// 	if( pInven->GetMoney().GetInt() < TotalCost )
// 	{
// 		cPrintMsg::PrintMsg( 11071 );
// 		return;// 돈이 모자르다.
// 	}
// 
// 	LIST_MaterialList_CIT itS = m_pItemInfo->m_MaterialList.begin();
// 	LIST_MaterialList_CIT itE = m_pItemInfo->m_MaterialList.end();
// 	for( ; itS != itE ; ++itS )
// 	{
// 		sMaterialItemList	pMatList = (*itS);
// 
// 		DWORD dwHaveItemCount = GetHaveItemCount( pMatList.m_dwItemIdx );
// 
// 		if( dwHaveItemCount < pMatList.m_nItemNum * m_ItemMakeCount )
// 		{
// 			CsItem::sINFO* pInfo = nsCsFileTable::g_pItemMng->GetItem( pMatList.m_dwItemIdx )->GetInfo();
// 			cPrintMsg::PrintMsg( 11046,	pInfo->s_szName);
// 			return;// 아이템이 모자르다.
// 		}
// 	}
// 
// 	int nEmptySlot = pInven->FindEmptySlot(-1);
// 	if( nEmptySlot == cData_Inven::INVALIDE_INVEN_INDEX )
// 	{
// 		cPrintMsg::PrintMsg( 11015 );
// 		return; // 인벤 빈슬롯이 모자름.
// 	}
// 
// 	if( net::game )
// 		net::game->Send_ItemMake( m_dwTargetNpcIdx, m_ItemMakeCount, m_pItemInfo->m_nUniqueIdx, m_dwPercentageItemIdx, m_dwProtectItemIdx );
// 
// 	return;
// }
// bool CItemProductionContents::SendMakingItem( int const& nMCIdx, int const& nSCIdx, int const& nUniqueIdx, int const& nMakeCount )
// {
// 	if( m_dwTargetNpcIdx == 0 )
// 		return false;//타겟 NPC없다.
// 
// 	m_pItemInfo = GetMaterialItemList(nMCIdx, nSCIdx, nUniqueIdx );
// 	SAFE_POINTER_RETVAL( m_pItemInfo, false );// 아이템 정보를 찾을 수 없다
// 
// 	CsItem*	pItem = nsCsFileTable::g_pItemMng->GetItem( m_pItemInfo->m_dwItemIdx );
// 	CsItem::sINFO* pInfo = pItem->GetInfo();
// 	
// 	m_ItemMakeCount = nMakeCount;
// 
// 	cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_MAKING, pInfo->s_szName, &m_ItemMakeCount);
// 
// 	return true;
// }

bool CItemProductionContents::StartMakeingProgress( int const& nMCIdx, int const& nSCIdx, int const& nUniqueIdx, int const& nMakeCount, 
											   DWORD const& dwPercentageItemCode, DWORD const& dwProtectItemCode )
{
	sProductionItemList const* pItemInfo = GetMaterialItemList(nMCIdx, nSCIdx, nUniqueIdx );
	SAFE_POINTER_RETVAL( pItemInfo, false );// 아이템 정보를 찾을 수 없다

	if( !_CheckCanMakeItem( pItemInfo, nMakeCount, dwPercentageItemCode, dwProtectItemCode ) )
		return false;

	_SetInvenItemLockAUnLock( true, pItemInfo, nMakeCount, dwPercentageItemCode, dwProtectItemCode );
	return true;
}


bool CItemProductionContents::_CheckCanMakeItem( sProductionItemList const* pItemInfo, int const& nMakeCount, 
												DWORD const& dwPercentageItemCode, DWORD const& dwProtectItemCode )
{
	if( m_dwTargetNpcIdx == 0 )
		return false;//타겟 NPC없다.

	SAFE_POINTER_RETVAL( pItemInfo, false );// 아이템 정보를 찾을 수 없다

	SAFE_POINTER_RETVAL(nsCsFileTable::g_pItemMng, false);
	SAFE_POINTER_RETVAL(g_pDataMng, false);
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL(pInven, false);	// 인벤토리가 없다

	u8 TotalCost = pItemInfo->m_dwItemCost * nMakeCount;
	if( pInven->GetMoney().GetInt() < TotalCost )
	{
		cPrintMsg::PrintMsg( 11071 );
		return false;// 돈이 모자르다.
	}

	int nEmptySlot = pInven->FindEmptySlot(-1);
	if( nEmptySlot == cData_Inven::INVALIDE_INVEN_INDEX )
	{
		cPrintMsg::PrintMsg( 11015 );
		return false; // 인벤 빈슬롯이 모자름.
	}

	LIST_MaterialList_CIT itS = pItemInfo->m_MaterialList.begin();
	LIST_MaterialList_CIT itE = pItemInfo->m_MaterialList.end();
	for( ; itS != itE ; ++itS )
	{
		sMaterialItemList	pMatList = (*itS);

		DWORD dwHaveItemCount = GetHaveItemCount( pMatList.m_dwItemIdx );

		if( dwHaveItemCount < pMatList.m_nItemNum * nMakeCount )
		{
			CsItem* pItemTable = nsCsFileTable::g_pItemMng->GetItem( pMatList.m_dwItemIdx );
			if( pItemTable )
			{
				CsItem::sINFO* pInfo = pItemTable->GetInfo();
				if( pInfo )
					cPrintMsg::PrintMsg( 11046,	pInfo->s_szName);
			}
			return false;// 아이템이 모자르다.
		}
	}

	if( 0 != dwProtectItemCode )
	{
		if( !IsAssistItem( pItemInfo->m_nProtectAssistItemGroup, dwProtectItemCode ) )
		{	// 등록된 아이템은 테이블상에 등록 안된 아이템 이다.
			return false;
		}

		DWORD dwHaveItemCount = GetHaveItemCount( dwProtectItemCode );
		if( dwHaveItemCount < nMakeCount )
		{//제작 재료 보호 아이템의 보유 갯수가 제작하려는 아이템의 갯수보다 작다
			CsItem* pItemTable = nsCsFileTable::g_pItemMng->GetItem( dwProtectItemCode );
			if( pItemTable )
			{
				CsItem::sINFO* pInfo = pItemTable->GetInfo();
				if( pInfo )
					cPrintMsg::PrintMsg( 11046,	pInfo->s_szName);
			}
			return false;
		}
	}

	if( 0 != dwPercentageItemCode )
	{
		if( !IsAssistItem( pItemInfo->m_nPercentageAssistItemGroup, dwPercentageItemCode ) )
		{
			// 등록된 아이템은 테이블상에 등록 안된 아이템 이다.
			return false;
		}

		DWORD dwHaveItemCount = GetHaveItemCount( dwPercentageItemCode );
		if( dwHaveItemCount < nMakeCount )
		{
			CsItem* pItemTable = nsCsFileTable::g_pItemMng->GetItem( dwPercentageItemCode );
			if( pItemTable )
			{
				CsItem::sINFO* pInfo = pItemTable->GetInfo();
				if( pInfo )
					cPrintMsg::PrintMsg( 11046,	pInfo->s_szName);// 제작 확률 아이템의 보유 갯수가 제작하려는 아이템의 갯수보다 작다
			}
			return false;
		}
	}

	return true;
}

void CItemProductionContents::_SetInvenItemLockAUnLock(bool bLock, 
													   sProductionItemList const* pItemInfo, 
													   DWORD const& dwMakeItemCount, 
													   DWORD const& dwPrecentageItemCode, 
													   DWORD const& dwProtectItemCode )
{	
	SAFE_POINTER_RET( pItemInfo );
	if( bLock )
	{
		m_pItemInfo = pItemInfo;
		m_dwProtectItemIdx = dwProtectItemCode;
		m_dwPercentageItemIdx = dwPrecentageItemCode;
		m_ItemMakeCount = dwMakeItemCount;
	}
	else
	{
		m_pItemInfo = NULL;
		m_dwProtectItemIdx = 0;
		m_dwPercentageItemIdx = 0;
		m_ItemMakeCount = 0;
	}

	LIST_MaterialList_CIT itS = pItemInfo->m_MaterialList.begin();
	LIST_MaterialList_CIT itE = pItemInfo->m_MaterialList.end();
	for( ; itS != itE ; ++itS )
		InvenItemSlotLock( bLock, (*itS).m_dwItemIdx );

	InvenItemSlotLock( bLock, dwProtectItemCode );
	InvenItemSlotLock( bLock, dwPrecentageItemCode );
}

void CItemProductionContents::_UnLock_MakeWait_InvenItem()
{
	if( m_pItemInfo )
	{
		LIST_MaterialList_CIT itS = m_pItemInfo->m_MaterialList.begin();
		LIST_MaterialList_CIT itE = m_pItemInfo->m_MaterialList.end();
		for( ; itS != itE ; ++itS )
			InvenItemSlotLock( false, (*itS).m_dwItemIdx );

		m_pItemInfo = NULL;
	}

	InvenItemSlotLock( false, m_dwProtectItemIdx );
	InvenItemSlotLock( false, m_dwPercentageItemIdx );

	m_dwProtectItemIdx = 0;
	m_dwPercentageItemIdx = 0;
}

// 실제 서버에 패킷을 보내는 함수 
bool CItemProductionContents::SendMakeItemData()
{
	if( !_CheckCanMakeItem( m_pItemInfo, m_ItemMakeCount, m_dwPercentageItemIdx, m_dwProtectItemIdx ) )
		return false;

	if( net::game )
	{
		m_bWaitRecvMakeItem = true;
		net::game->Send_ItemMake( m_dwTargetNpcIdx, m_ItemMakeCount, m_pItemInfo->m_nUniqueIdx, m_dwPercentageItemIdx, m_dwProtectItemIdx );
		return true;
	}

	return false;
}

// 서버에 패킷을 보내기까지 대기 시간중에 UI에서 중지 버튼을 눌렀을 때 호출 되는 함수
// 서버에 이미 패킷을 보낸 상태면 중지 안됨.
// 인벤토리 아이템 락 했던 슬롯 언락 시킴
void CItemProductionContents::StopMakeItemProgress()
{
	if( m_bWaitRecvMakeItem )
	{// 서버에 이미 패킷을 보낸 상태
		return;
	}
	_UnLock_MakeWait_InvenItem();
}

void CItemProductionContents::RecvMakingItemResult(void* pData)
{
	SAFE_POINTER_RET( pData);
	GS2C_RECV_ITEMPRODUCTION * pRecv = static_cast<GS2C_RECV_ITEMPRODUCTION*>(pData);
	SAFE_POINTER_RET( pRecv );

	m_bWaitRecvMakeItem = true;
	_UnLock_MakeWait_InvenItem();
	Notify( eRecvMakeItem );

	if( 0 != pRecv->m_dwResult )// 
	{
		cPrintMsg::PrintMsg( pRecv->m_dwResult );
		return ;
	}

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	pInven->SetMoney( pRecv->m_MoneyResult, false );// 아이템을 만들고 남은 돈

	// 서버상에서 아이템을 넣고 지우기 때문에 클라이언트에서도 같이 해줘야함.
	DWORD dwMakeItemIdx = pRecv->m_sDwItemIdx;
	int	nSuccessItemCount = pRecv->m_GetCount;
	int	nFailItemCount = pRecv->m_TotalMakedCount-nSuccessItemCount;
	int	nMakeItemCount = pRecv->m_nItemCount;
	if( dwMakeItemIdx != 0 && nMakeItemCount != 0)
		pInven->ItemCrop( dwMakeItemIdx, nMakeItemCount, pRecv->m_nTradeLimitTime );

	std::list<sItemInfo>::iterator it = pRecv->m_DelItemInfo.begin();
	for( ; it != pRecv->m_DelItemInfo.end(); ++it )
	{
		int ItemIdx = (*it).sDwItemIdx;
		int Itemcnt = (*it).nItemCount;
		pInven->ItemRemove_NoServer( ItemIdx, Itemcnt, false );
		GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_ITEM_COUNT, &ItemIdx );
	}

	// 보호용 아이템 사용
	if( 0 != pRecv->m_nIsItemProtect )
	{
		CsItem::sINFO const* pFTItemInfo = GetItemInfo( pRecv->m_nIsItemProtect );
		if( pFTItemInfo )
			cPrintMsg::PrintMsg( 11076, (TCHAR*)pFTItemInfo->s_szName );
	}

	CsItem::sINFO const* pFTItemInfo = GetItemInfo( dwMakeItemIdx );
	if(pFTItemInfo)	// 획득 아이템 이름
	{
		cPrintMsg::PrintMsg( 11072, (TCHAR*)pFTItemInfo->s_szName, &nMakeItemCount, &nSuccessItemCount, &nFailItemCount);
		cItemData data;
		data.m_nType = dwMakeItemIdx;
		data.m_nCount = nMakeItemCount;
		pInven->OutputCropMsg( &data, nMakeItemCount ); // [3/24/2016 hyun] 메세지 출력
	}
}

// 인벤토리에서 아이템 인덱스로 총 갯수를 얻는다.
DWORD CItemProductionContents::GetHaveItemCount( DWORD const& dwItemIdx ) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, 0 );
	return pInven->GetCount_Item_ID( dwItemIdx );
}

void CItemProductionContents::SetAssistItem(void* pData)
{
	SAFE_POINTER_RET( pData );
	ContentsStream kTmp = *static_cast<ContentsStream*>(pData);
	int nAssistType;
	int nAssistItemCode;
	kTmp >> nAssistType >> nAssistItemCode;

	DWORD dwHaveItemCount = GetHaveItemCount( nAssistItemCode );
	if( 0 == dwHaveItemCount )
	{
		CsItem* pItemTable = nsCsFileTable::g_pItemMng->GetItem( nAssistItemCode );
		if( pItemTable )
		{
			CsItem::sINFO* pInfo = pItemTable->GetInfo();
			if( pInfo )
				cPrintMsg::PrintMsg( 11046,	pInfo->s_szName);
		}
		return;
	}

	switch( nAssistType )
	{
	case sAssistanceItemGroup::ePercentageAssist:	m_dwPercentageItemIdx = nAssistItemCode;	break;		
	case sAssistanceItemGroup::eProtectAssist:		m_dwProtectItemIdx = nAssistItemCode;		break;		
	default:
		return;
	}

	ContentsStream newTmp;
	newTmp << nAssistType << nAssistItemCode << dwHaveItemCount;
	Notify( eSetAssistItem, newTmp);
}

void CItemProductionContents::ReSetAssistItem(int const& nAssistType)
{
	switch( nAssistType )
	{
	case sAssistanceItemGroup::ePercentageAssist:	
		{
			m_dwPercentageItemIdx = 0;	
			ContentsStream newTmp;
			newTmp << nAssistType << m_dwPercentageItemIdx << int(0);
			Notify( eSetAssistItem, newTmp);
		}break;		
	case sAssistanceItemGroup::eProtectAssist:
		{
			m_dwProtectItemIdx = 0;		
			ContentsStream newTmp;
			newTmp << nAssistType << m_dwProtectItemIdx << int(0);
			Notify( eSetAssistItem, newTmp);
		}break;
	}
}

void CItemProductionContents::InvenItemSlotLock( bool bLock, DWORD const& dwItemCode )
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	if( 0 == dwItemCode )
		return;

	if( bLock )		pInven->ItemLock_ItemID( dwItemCode );
	else			pInven->ItemUnlock_ItemID( dwItemCode );
}

int CItemProductionContents::GetAssistItemPercentage(int const& nGroupID, DWORD const& dwItemCode) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, 0 );

	LIST_AssistItemList const* pList = nsCsFileTable::g_pItemMng->GetAssistGroupItemList( nGroupID );
	SAFE_POINTER_RETVAL( pList, 0 );
	LIST_AssistItemList_CIT it = pList->begin();
	for( ; it != pList->end(); ++it )
	{
		if( dwItemCode == (*it).dwItemCode )
			return (*it).nSuccessPercentage;
	}

	return 0;
}

bool CItemProductionContents::IsAssistItem( int const& nGroupID, DWORD const& dwItemCode ) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, false );

	LIST_AssistItemList const* pList = nsCsFileTable::g_pItemMng->GetAssistGroupItemList( nGroupID );
	SAFE_POINTER_RETVAL( pList, false );
	LIST_AssistItemList_CIT it = pList->begin();
	for( ; it != pList->end(); ++it )
	{
		if( dwItemCode == (*it).dwItemCode )
			return true;
	}

	return false;
}