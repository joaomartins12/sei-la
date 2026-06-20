
#include "stdafx.h"

#include "cCliGame.h"

void cCliGame::SendPCBangInvenRequest()
{
	newp( pItem::PCbangInven );
	endp( pItem::PCbangInven );
	send();
}

void cCliGame::SendCropPCBangInvenItem(uint nSlot, uint nType, uint nCount)
{
	if( nType <= 0 || nCount <= 0 )
	{
		return;
	}

	if( nSlot > 7 || nSlot < 0 )
	{
		return;
	}

	newp( pItem::CropPCBangItem );
		push( nSlot );
		push( nType );
		push( nCount );
	endp( pItem::CropPCBangItem );
	send();
}

void cCliGame::RecvPCBangInven(void)
{
	// PC방 인벤토리 정보는 7개 모두 송신 됩니다.

	assert_cs( g_pDataMng->GetPCbang()->GetPcBang() );
	cItemInfo* pItemInfo = NULL;
	for(uint i = 0; i < nLimit::PcbangInven; i++ )
	{
		pItemInfo = g_pDataMng->GetPCbang()->GetData( i );
		pop(*pItemInfo);	

#ifdef _DEBUG
		TCHAR msg[ 256 ];
		_stprintf_s( msg, 256, _T( "\nIndex = %d, ID = %d, count = %d" ), i, pItemInfo->GetType(), pItemInfo->GetCount() );		
		
		ST_CHAT_PROTOCOL	CProtocol;
		CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
		CProtocol.m_wStr = msg;
		GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
#endif
	}		
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_PCBANGINVEN );
}

void cCliGame::RecvPreminum(void)
{
	// 테이머 PC방 프리미엄 서비스 시작	

	g_pDataMng->GetPCbang()->SetPcBang( true );	
	cPrintMsg::PrintMsg( 20201 );
}

void cCliGame::RecvPreminumEnd(void)
{
	// 서버에서는 PC방용 아이템을 모두 제거 하고 버프가 있는 경우 버프를 제거 합니다.
	g_pDataMng->GetPCbang()->SetPcBang( false );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PCBANGINVEN ) == true )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_PCBANGINVEN );	

	GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_HIDE_NEWCASHSHOP );

	// 인벤토리 최대 개수 만큼 읽어 옵니다..
	cItemInfo* pItemInfo = NULL;
	CsItem::sINFO* pFTItem = NULL;
	cData_Inven* pInven = g_pDataMng->GetInven();
	for(uint i = 0; i < nLimit::Inven ; i++ )
	{
		pItemInfo = pInven->GetData( i );
		if( pItemInfo->IsEnable() == true )
		{
			pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() )->GetInfo();
			if( pFTItem->IsPCBangType() ) //PC방 아이템인경우 
				pInven->GetData( i )->Reset();		
		}
	}
	
	// 착용 장비 최대 개수 만큼 읽어 옵니다.
	cData_TEquip* pEquip = g_pDataMng->GetTEquip();
	for(uint i = 0; i < nLimit::Equip ; i++ )
	{
		pItemInfo = pEquip->GetData( i );
		if( pItemInfo->IsEnable() == true )
		{
			pFTItem = nsCsFileTable::g_pItemMng->GetItem( pItemInfo->GetType() )->GetInfo();
			if( pFTItem->IsPCBangType() )			
				pEquip->GetData( i )->Reset();		

			if( pFTItem->s_dwItemID == PCBANG_COSTUME_ITEM_ID )
			{
				CsC_PartObject::sCHANGE_PART_INFO cp;				 
				cp.s_nPartIndex = nsPART::Costume;
				if( g_pCharMng && g_pCharMng->GetTamerUser() )
					g_pCharMng->GetTamerUser()->ChangePart( &cp );
			}			
		}
	}			
	
	g_pCharMng->GetTamerUser()->GetBuff()->ReleaseBuff( nsBuff::BK_EQUIP_PC );

	cPrintMsg::PrintMsg( 20202 );	
}

void cCliGame::RecvPartyPCBangExpBuff(void)
{
	uint nCount = 0;
	uint nExpPlusRate = 0;
	
	pop(nCount);			// 파티내의 PC방 인원수
	pop(nExpPlusRate);		// 추가 경험치 획득 률		
}

void cCliGame::RecvPCBangItemCropResult(void)
{
	u1 nSlotNo = 0;
	u4 nItemType = 0;
	u4 nItemCount = 0;
	u1 nItemRate = 0;
	u4 nEndTime = 0;

	pop(nSlotNo);		// 아이템이 들어간 슬롯 번호
	pop(nItemType);		// 아이템 타입
	pop(nItemCount);	// 아이템 개수
	pop(nItemRate);		// 능력치 적용 비율
	pop(nEndTime);		// 만료 시간	

	// pc방 아이템 찾음
	cItemData data;	
	data.m_nType = nItemType;
	data.m_nCount = nItemCount;
	data.m_nRate = nItemRate;
	data.m_nEndTime = nEndTime;
	g_pDataMng->GetInven()->ItemCrop( &data );
}