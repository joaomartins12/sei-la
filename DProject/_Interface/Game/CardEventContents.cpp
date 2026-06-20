#include "StdAfx.h"

#include "../../ContentsSystem/ContentsSystem.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

#include "CardEventContents.h"

CardEventContents::CardEventContents(void):m_dwNpcID(0),m_dwResultItemCode(0),m_nItemCount(0)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_CARD_EVENT_UI, this, &CardEventContents::openCardEventUI );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CARD_EVENT_RESULT, this, &CardEventContents::recvCardEventResult );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CARD_EVENT_END, this, &CardEventContents::recvCardEventEnd );
}

CardEventContents::~CardEventContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);	
}

int const CardEventContents::IsContentsIdentity(void)
{
	return E_CT_CARD_EVENT;	// 카드 이벤트 시스템
}

int const CardEventContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CardEventContents::Initialize(void)
{
	return true;
}

void CardEventContents::UnInitialize(void)
{
}

bool CardEventContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void CardEventContents::Update(float const& fElapsedTime)
{
}

void CardEventContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void CardEventContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

void CardEventContents::MakeMainActorData(void)
{
}

void CardEventContents::ClearMainActorData(void)
{
}

void CardEventContents::ClearWorldData(void)
{
}

void CardEventContents::MakeWorldData(void)
{
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

DWORD CardEventContents::getNpcID() const
{
	return m_dwNpcID;
}

DWORD CardEventContents::getResultItemCode() const
{
	return m_dwResultItemCode;
}

int CardEventContents::getItemCount() const
{
	return m_nItemCount;
}

void CardEventContents::openCardEventUI(void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	m_dwNpcID = *static_cast<DWORD*>(pData);

	if( 1 > pInven->GetEmptySlotCount() )	// 비어있는 인벤 개수 확인
	{
		cPrintMsg::PrintMsg( 11015 );
		return;
	}

	CsNpcMng::sNPC_EVENT_INFO* pInfo = nsCsFileTable::g_pNpcMng->GetCardGameInfo( m_dwNpcID );
	SAFE_POINTER_RET( pInfo );

	if( pInven->GetMoney().GetInt() < pInfo->s_nExhaustMoney )
	{
		cPrintMsg::PrintMsg( 11003 );	// 돈 모자르다 메세지 출력
		return;
	}

	if( pInfo->s_dwExhaustItem > 0 )
	{
		DWORD dwHaveItemCount = pInven->GetCount_Item_ID( pInfo->s_dwExhaustItem );
		if( dwHaveItemCount < pInfo->s_unItemCount )
		{
			CsItem* pItem = nsCsFileTable::g_pItemMng->GetItem( pInfo->s_dwExhaustItem );
			if( pItem )
			{
				CsItem::sINFO* pItemInfo = pItem->GetInfo();
				if( pItemInfo )
					cPrintMsg::PrintMsg( 11040, pItemInfo->s_szName );// 아이템이 부족 메세지 출력
			}			
			return;
		}
	}

	net::game->SendCardEventStart();
}

void CardEventContents::recvCardEventResult(void* pData)
{
	SAFE_POINTER_RET( pData );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	CsNpcMng::sNPC_EVENT_INFO* pInfo = nsCsFileTable::g_pNpcMng->GetCardGameInfo( m_dwNpcID );
	SAFE_POINTER_RET( pInfo );

	if( pInfo->s_dwExhaustItem > 0 )
		pInven->DecreaseItem( pInfo->s_dwExhaustItem, pInfo->s_unItemCount, false, false ) ;

	std::pair<DWORD,int> result = *static_cast<std::pair<DWORD,int>*>(pData);
	m_dwResultItemCode = result.first;
	m_nItemCount = result.second;

	// 창을 생성하고 보여 주지는 않는다.
	cBaseWindow* pBaseWin = g_pGameIF->GetDynamicIF( cBaseWindow::WT_CARD_EVENT, 0 ,true );
	SAFE_POINTER_RET( pBaseWin );
}

void CardEventContents::SendCardEventEnd()
{
	CsItem* pTempItem = nsCsFileTable::g_pItemMng->GetItem( m_dwResultItemCode );
	if( pTempItem )
	{
		CsItem::sINFO * TempInfo = pTempItem->GetInfo();
		if( TempInfo )
			cPrintMsg::PrintMsg( 30517, TempInfo->s_szName );
	}

	if( net::game )
		net::game->SendCardEventEnd();
}

void CardEventContents::recvCardEventEnd(void* pData)
{
	SAFE_POINTER_RET( pData );

	int nResult = *static_cast<int*>(pData);
	if( nResult == 0 )
	{
		SAFE_POINTER_RET( g_pDataMng );
		cData_Inven* pInven = g_pDataMng->GetInven();
		SAFE_POINTER_RET( pInven );

		cItemData data;
		data.m_nType  = m_dwResultItemCode;
		data.m_nRate  = 100;
		data.m_nCount = m_nItemCount;
		data.m_nLevel = 1;

		pInven->ItemCrop( &data );
	}
	else if( nResult == 1 )
		cPrintMsg::PrintMsg( 30518 );

	InitData();
}

void CardEventContents::InitData()
{
	m_dwNpcID = 0;
	m_dwResultItemCode = 0;
	m_nItemCount = 0;
}