#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "ItemRewardContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptTutorialQuest.h"

int const ItemRewardContents::IsContentsIdentity(void)
{
	return E_CT_ITEM_REWARD;
}

ItemRewardContents::ItemRewardContents(void):m_AdaptTutorialQuest(0)
{
	mGiftData.clear();
	mRewardData.clear();

	GAME_EVENT_ST.AddEvent( EVENT_CODE::REWARD_ITEM_TOGGLE, this, &ItemRewardContents::Reward_ToggleCheck); 

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::REWARD_RECEIVE_GIFT, this, &ItemRewardContents::ReceiveGift);	
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::REWARD_RECEIVE_REWARD, this, &ItemRewardContents::ReceiveReward);
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::REWARD_SEND_ACCPET_DATA, this, &ItemRewardContents::ReceiveSendAccpetData);
}

ItemRewardContents::~ItemRewardContents(void)
{
	m_AdaptTutorialQuest = 0;
	GAME_EVENT_ST.DeleteEventAll( this );
}

int const ItemRewardContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool ItemRewardContents::Initialize(void)
{
	return true;
}

void ItemRewardContents::UnInitialize(void)
{
}

bool ItemRewardContents::IntraConnection(ContentsSystem& System)
{
	m_AdaptTutorialQuest = new AdaptTutorialQuest;
	if( m_AdaptTutorialQuest && !m_AdaptTutorialQuest->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;

	return true;
}

void ItemRewardContents::Update(float const& fElapsedTime)
{
}

void ItemRewardContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void ItemRewardContents::MakeMainActorData(void)
{
}

void ItemRewardContents::ClearMainActorData(void)
{
}

void ItemRewardContents::ClearWorldData(void)
{

}

void ItemRewardContents::Refresh(void)
{
}

void ItemRewardContents::FullReceipt(void)
{
}

void ItemRewardContents::CloseItemRewardUI()
{
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_REWARD, 0 );
}

void ItemRewardContents::Reward_ToggleCheck(void* pData)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_REWARD, 0 ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_REWARD, 0 );
	else
	{
		if( m_AdaptTutorialQuest && m_AdaptTutorialQuest->IsTutorialPlaying() )
		{
			cPrintMsg::PrintMsg( 30412 );
			return;
		}
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_REWARD, 0 );
	}
}

void ItemRewardContents::ReceiveGift(void* pData)
{
	mGiftData.clear();

	GS2C_RECV_GIFT_DATA* data = static_cast<GS2C_RECV_GIFT_DATA*>(pData);
	data->CopyData(mGiftData);
}

void ItemRewardContents::ReceiveReward(void* pData)
{
	mRewardData.clear();

	GS2C_RECV_REWARD_DATA* data = static_cast<GS2C_RECV_REWARD_DATA*>(pData);
	data->CopyData(mRewardData);
}

void ItemRewardContents::ReceiveSendAccpetData(void* pData)
{
	GS2C_RECV_REWARD_SEND_TYPE* data = static_cast<GS2C_RECV_REWARD_SEND_TYPE*>(pData); 

	switch(data->iType)
	{
	case 1:	//선물
		{
			if(data->bSendAll)
			{
				SendAllGiftItem();
			}
			else
			{
				if(data->iIndex != -1)
					SendCropGiftItem(data->iIndex);
			}
		}
		break;

	case 2:	//선물삭제
		{
			DeleteGiftItem(data->iIndex);
		}
		break;

	case 3:	//보상
		{
			if(data->bSendAll)
			{
				SendRequestRewardAll();
			}
			else
			{
				if(data->iIndex != -1)
					SendRequestRewards(data->iCnt, data->iIndex);
			}
		}
		break;

	case 4: //보상삭제
		{
			DeleteRewardItem(data->iIndex);
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
//선물

GIFT_ITEM_LIST* ItemRewardContents::GetGiftData()
{
	return &mGiftData;
}

cItemInfo* ItemRewardContents::GiftItem( int nIndex )
{
	GIFT_ITEM_LIST::iterator miter = mGiftData.find(nIndex);
	if(miter != mGiftData.end())
		return	&miter->second;
	return NULL;
}

int	ItemRewardContents::GetCurrentGiftSize()
{
	int size = 0;
	GIFT_ITEM_LIST::iterator mit = mGiftData.begin();
	for( mit; mit!=mGiftData.end(); ++mit )
	{
		cItemInfo* pGiftItem = &mit->second;
		if( pGiftItem->IsEnable() == false )
			continue;

		size++;
	}
	return size;
}

void ItemRewardContents::DeleteGiftItem(int idx)
{
	GIFT_ITEM_LIST::iterator	miter = mGiftData.find(idx);
	if(miter != mGiftData.end())
	{
		miter->second.Reset();	
	}

	if( GetCurrentGiftSize() == 0 )	
		mGiftData.clear();
}

void ItemRewardContents::SendCropGiftItem(int nIndex)
{
	cItemInfo* pGift = GiftItem( nIndex );
	SAFE_POINTER_RET(pGift);

	if( pGift->IsEnable() == false )
		return;

	CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem(pGift->GetType())->GetInfo();
	SAFE_POINTER_RET(pFTItem);

	if( pFTItem->IsPCBangType() ) 
	{
		if( g_pDataMng->GetPCbang()->GetPcBang() == false )
		{
			cPrintMsg::PrintMsg( 20203 );
			return;
		}
	}		

	if( net::game )
	{
		net::game->SendCropGiftItem( nIndex );
	}

	// 아이템 빼자
	pGift->Reset();
}

void ItemRewardContents::SendAllGiftItem()
{	
	GIFT_ITEM_LIST* GiftInfo = &mGiftData;
	SAFE_POINTER_RET(GiftInfo);

	u2 count = (u2)GetCurrentGiftSize();	// 빈 공간 제외한 실제 갯수.

	if( 0 == count)
		return;

	// 인벤토리 여유공간체크
	g_pDataMng->GetInven()->Backup_Save();

	GIFT_ITEM_LIST::iterator	mit = GiftInfo->begin();
	for( mit; mit!=GiftInfo->end(); ++mit )	//for( int i=0; i<nItemPostalCount; ++i )
	{
		cItemInfo* pGiftItem = &mit->second;
		if( pGiftItem->IsEnable() == false )
			continue;

		CsItem::sINFO* pFTItem = nsCsFileTable::g_pItemMng->GetItem( pGiftItem->GetType() )->GetInfo();
		if( pFTItem->IsPCBangType() ) //PC방 아이템인경우 
		{
			if( g_pDataMng->GetPCbang()->GetPcBang() == false )
			{
				cPrintMsg::PrintMsg( 20203 );
				return;
			}
		}	

		if( g_pDataMng->GetInven()->ItemCrop(pGiftItem->GetType(), pGiftItem->GetCount() ) != 0 ) // ID,cnt
		{
			cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
			g_pDataMng->GetInven()->Backup_Load();		
			return;
		}
	}
	g_pDataMng->GetInven()->Backup_Load();	

	cPacket p;
	p.newp(pItem::CropGiftItem);	
	p.push(count);	// 요청 아이템 카운트	

	for( mit = GiftInfo->begin(); mit!=GiftInfo->end(); ++mit )
	{
		cItemInfo* pGiftItem = &mit->second;
		if( pGiftItem->IsEnable() == false )
			continue;

		count = mit->first;
		g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GET_REWARD, count );
		p.push(count); // 요청 아이템 테이블 인덱스

	}

	p.endp(pItem::CropGiftItem);
	net::game->DoSend(p);

	net::game->SetSendBlock(true);
}


////////////////////////////////////////////////////////////////////////////////
//보상

REWARD_ITEM_LIST* ItemRewardContents::GetRewardData()
{
	return &mRewardData;
}

CsReward* ItemRewardContents::GetReward( int nFTID )
{
	CsReward::MAP*	pMapReward = nsCsFileTable::g_pRewardMng->GetReward( g_pResist->m_AccountGlobal.s_nLastConnetServer );
	assert_cs( pMapReward != NULL );
	SAFE_POINTER_RETVAL( pMapReward, NULL );

	CsReward::MAP_IT	it = pMapReward->find(nFTID);
	if(it != pMapReward->end())
		return it->second;

	return NULL;
}

int	ItemRewardContents::GetCurrentRewardSize()
{
	int size = 0;
	REWARD_ITEM_LIST::iterator	mit = mRewardData.begin();
	for( mit; mit!=mRewardData.end(); ++mit )
	{
		int Reward_Idx = mit->second;
		if( Reward_Idx == 0 )
			continue;

		size++;
	}
	return size;
}

void ItemRewardContents::DeleteRewardItem( int idx )
{
	REWARD_ITEM_LIST::iterator	miter = mRewardData.begin();
	for(miter; miter != mRewardData.end(); ++miter)
	{
		int	Reward_Idx = miter->second;
		if( Reward_Idx == idx )
			miter->second = 0;	
	}

	if( GetCurrentRewardSize() == 0 )	
		mRewardData.clear();
}

void ItemRewardContents::SendRequestRewards(u1 nReqCnt, u1 nReqIdx)
{
	if(NULL == GetReward(nReqIdx))
		return;
	CsReward::sINFO* pInfo = GetReward(nReqIdx)->GetInfo();

	// 인벤토리 여유공간체크
	g_pDataMng->GetInven()->Backup_Save();	

	if( g_pDataMng->GetInven()->ItemCrop(pInfo->s_nItemCode1, pInfo->s_nItemCount1 ) != 0 ) // ID,cnt
	{
		cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
		g_pDataMng->GetInven()->Backup_Load();		
		return;
	}

	g_pDataMng->GetInven()->Backup_Load();

	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GET_REWARD, 0 );

	cPacket p;
	p.newp(pGameTwo::RecompenseGain);	
	p.push(nReqCnt);	// 요청 아이템 카운트	
	p.push(nReqIdx); // 요청 아이템 테이블 인덱스
	p.endp(pGameTwo::RecompenseGain);
	net::game->DoSend(p);
}

void ItemRewardContents::SendRequestRewardAll()
{
	u1 idx = 0;

	u1 count = (u1)GetCurrentRewardSize();
	if( 0 == count)
		return;

	// 인벤토리 여유공간체크
	g_pDataMng->GetInven()->Backup_Save();

	REWARD_ITEM_LIST::iterator miter = mRewardData.begin();
	for( miter; miter!=mRewardData.end(); ++miter)
	{
		idx = miter->second;
		if( idx == 0 )
			continue;

		if(NULL == GetReward(idx))
			continue;

		CsReward::sINFO*	pInfo = GetReward(idx)->GetInfo();

		if( g_pDataMng->GetInven()->ItemCrop(pInfo->s_nItemCode1, pInfo->s_nItemCount1 ) != 0 ) // ID,cnt
		{
			cPrintMsg::PrintMsg( cPrintMsg::GAME_INVEN_LIMIT_COUNT );
			g_pDataMng->GetInven()->Backup_Load();		
			return;
		}
	}
	g_pDataMng->GetInven()->Backup_Load();

	g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::GET_REWARD, 0 );

	cPacket p;
	p.newp(pGameTwo::RecompenseGain);	
	p.push(count);	// 요청 아이템 카운트	

	miter = mRewardData.begin();
	for( miter; miter!=mRewardData.end(); ++miter)
	{
		idx = miter->second;
		if( idx == 0 )
			continue;

		p.push(idx); // 요청 아이템 테이블 인덱스
	}

	p.endp(pGameTwo::RecompenseGain);
	net::game->DoSend(p);
}
