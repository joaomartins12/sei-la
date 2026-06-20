#include "StdAfx.h"
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"

#include "UnionStoreContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

int const UnionStoreContents::IsContentsIdentity(void)
{
	return E_CT_UNIONSTORE_CONTENTS;
}

UnionStoreContents::UnionStoreContents(void)
{	
	m_UnionMoney = 0;

	m_nEmploymentChar_MapID = 0;
	m_vEmploymentChar_Pos = NiPoint2::ZERO;
	m_nEmploymentChar_Channel = 0;
	m_nEmploymentChar_ModelID = 0;

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_ITEM_ADD_DATA,  this, &UnionStoreContents::Add_UnionItem );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_MONEY_SET_DATA, this, &UnionStoreContents::Set_UnionMoney );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_SET_SHOPDATA,   this, &UnionStoreContents::Set_ShopData );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_SET_MODELID,    this, &UnionStoreContents::Set_ModelID );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_RELEASE_DATA,   this, &UnionStoreContents::ReleaseEmploymentChar );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_INITNET_OFF,    this, &UnionStoreContents::InitNetOff );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_DELETE_DATA,    this, &UnionStoreContents::DeleteUnion );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_GET_CHAR_MAPID,   this, &UnionStoreContents::Get_CharMapID );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_GET_CHAR_POS,		this, &UnionStoreContents::Get_CharPos );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::UNION_GET_CHAR_MODELID, this, &UnionStoreContents::Get_CharModelID );
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::RECV_UNION_SHOP_WITHDRAW, this, &UnionStoreContents::Recv_ShopWithDraw);
}

UnionStoreContents::~UnionStoreContents(void)
{
	GAME_EVENT_STPTR->DeleteEventAll(this);
}

int const UnionStoreContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool UnionStoreContents::SendDataUniStore(int type)
{
	switch(type)
	{
	case 0:
		{
			if( net::game )
			{
				net::game->SendCommissionShopNpcItemList();		
				return true;
			}
		}
		break;

	case 1:
		{
			if( net::game )
			{
				net::game->SendCommissionShopClose();
				return true;
			}
		}
		break;

	case 2:
		{
			if( net::game )
			{
				DeleteUnion();	//회수 버튼을 눌렀을때 RecvCommissionShopNpcItemList 한번더 호출됨 
				net::game->SendCommissionShopItemWithDraw();
				return true;
			}
		}
		break;
	}

	return false;
}

void UnionStoreContents::Add_UnionItem( void* pData )
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_ADD_UNIONITEM* pRecv = static_cast<GS2C_RECV_ADD_UNIONITEM*>(pData);

	cItemInfo* pInfo = csnew cItemInfo;
	*pInfo = pRecv->ItemData;

	m_vpUnionItem.PushBack( pInfo );
}

void UnionStoreContents::DeleteUnion()
{
	m_vpUnionItem.Destroy();
}

cItemInfo* UnionStoreContents::GetItemInfo(int num)
{
	if(m_vpUnionItem.Size() <= num)
	{
		return NULL;
	}

	return m_vpUnionItem[num];
}

int UnionStoreContents::GetItemCnt()
{
	return m_vpUnionItem.Size();
}

void UnionStoreContents::Set_UnionMoney(void* pData)
{ 
	SAFE_POINTER_RET(pData);
	GS2C_RECV_SET_UNIONMONEY* pRecv = static_cast<GS2C_RECV_SET_UNIONMONEY*>(pData);

	m_UnionMoney = pRecv->money;
}

void UnionStoreContents::SetUnionMoney(u8 money)
{ 
	m_UnionMoney = money; 
}

sMONEY UnionStoreContents::GetUnionMoney()
{ 
	return m_UnionMoney; 
}

bool UnionStoreContents::IHaveUnionStore() const
{
	return m_nEmploymentChar_MapID != 0 ? true : false;
}

int UnionStoreContents::GetMapID()
{
	return m_nEmploymentChar_MapID;
}

void UnionStoreContents::Set_ShopData(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_SHOP_DATA* pRecv = static_cast<GS2C_RECV_SHOP_DATA*>(pData);
	
	m_nEmploymentChar_MapID = pRecv->iCharMapID; 
	m_vEmploymentChar_Pos = pRecv->iCharPos;
	m_nEmploymentChar_Channel = pRecv->iCharChannel;

	SetModelID(pRecv->iCharModelID);
}

void UnionStoreContents::Set_ModelID(void* pData)
{
	SAFE_POINTER_RET(pData);
	GS2C_RECV_MODEL_ID* pRecv = static_cast<GS2C_RECV_MODEL_ID*>(pData);
	SetModelID(pRecv->iCharModelID);
}

void UnionStoreContents::SetModelID(int modelID)
{
	if(modelID == -1)
		return;

	CsBaseMng::sPERSON_STORE* pPersonStore = nsCsFileTable::g_pBaseMng->GetPersonStore();
	SAFE_POINTER_RET( pPersonStore );

	std::map< int, CsBaseMng::sPERSON_STORE::sOBJECT_INFO* >::iterator it = pPersonStore->s_mapObject.find( modelID );
	if( it == pPersonStore->s_mapObject.end() )
		return;

	CsBaseMng::sPERSON_STORE::sOBJECT_INFO* pObjInfo = it->second;
	SAFE_POINTER_RET( pObjInfo );

	int nDigimonID = pObjInfo->s_nDigimonID;
	CsDigimon* pFindDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID );
	SAFE_POINTER_RET( pFindDigimon );

	CsDigimon::sINFO* pInfo = pFindDigimon->GetInfo();
	SAFE_POINTER_RET( pInfo );

	m_nEmploymentChar_ModelID =	pInfo->s_dwModelID;

	
// 		int nDigimonID = nsCsFileTable::g_pBaseMng->GetPersonStore()->s_mapObject[ modelID ]->s_nDigimonID;
// 		m_nEmploymentChar_ModelID =	nsCsFileTable::g_pDigimonMng->GetDigimon( nDigimonID )->GetInfo()->s_dwModelID;

}

void UnionStoreContents::ReleaseEmploymentChar(void* pData)
{
	m_nEmploymentChar_MapID = 0;
	m_vEmploymentChar_Pos = NiPoint2::ZERO;
	m_nEmploymentChar_Channel = 0;
	m_nEmploymentChar_ModelID = 0;
}

void UnionStoreContents::InitNetOff(void* pData)
{
	m_nEmploymentChar_MapID = 1;
	m_nEmploymentChar_Channel = 2;
	m_vEmploymentChar_Pos = NiPoint2( 29802, 22283 );
	m_nEmploymentChar_ModelID = 31001;
}

void UnionStoreContents::DeleteUnion(void* pData)
{
	m_UnionMoney = 0;

	int nCnt = m_vpUnionItem.Size();
	for( int i=0; i<nCnt; ++i )
		delete m_vpUnionItem[ i ];

	m_vpUnionItem.Destroy();
}

void UnionStoreContents::Get_CharMapID(void* pData)
{
	SAFE_POINTER_RET( pData );
	int* nData = static_cast<int*>(pData);

	*nData = m_nEmploymentChar_MapID;
}

void UnionStoreContents::Get_CharPos(void* pData)
{
	SAFE_POINTER_RET( pData );
	NiPoint2* nData = static_cast<NiPoint2*>(pData);

	*nData = m_vEmploymentChar_Pos;
}

void UnionStoreContents::Get_CharModelID(void* pData)
{
	SAFE_POINTER_RET( pData );
	int* nData = static_cast<int*>(pData);

	*nData = m_nEmploymentChar_ModelID;
}

bool UnionStoreContents::IsEmptySlot()
{
	bool bEmpty = true;
	for( int ec=0; ec<m_vpUnionItem.Size(); ++ec )
	{
		cItemInfo* pInfo = m_vpUnionItem[ec];
		if( !pInfo->IsEnable() )
			continue;

		bEmpty = false;
	}

	return true;
}

bool UnionStoreContents::CollectItem()
{
	// 돈
	g_pDataMng->GetInven()->IncreaseMoney( m_UnionMoney, true );
	m_UnionMoney = 0;

	bool bRemain = false;

	// 아이템	
	for( int ec=0; ec<m_vpUnionItem.Size(); ++ec )
	{
		cItemInfo* pInfo = m_vpUnionItem[ec];

		if( pInfo->IsEnable() == false )
			continue;

		int nRemain = g_pDataMng->GetInven()->ItemCrop( pInfo );
		if( nRemain == 0 )
		{
			pInfo->Reset();
		}
		else
		{
			pInfo->m_nCount = nRemain;
			bRemain = true;
		}
	}

	return bRemain;
}

void UnionStoreContents::Recv_ShopWithDraw(void* pData)
{
	SAFE_POINTER_RET( pData );
	
	int nResult = *static_cast<int*>(pData);

	switch( nResult )
	{
	case 0:	// 해당 테이머의 조합 상인이 소지한 아이템 / 돈이 없습니다.
		break;
	case 100: // 회수 성공		
		{
			if( CollectItem() )// 보관함 아이템을 인벤토리에 넣을 때 빈공간이 없어 아이템을 다 넣지 못한경우
				cPrintMsg::PrintMsg( 11015 );
			net::game->SendQueryMoneyInven();
		}
		break;
	default:
		assert_csm1( false, _ERROR_MSG( "알수 없는 결과값 : %d" ), nResult );
	}

	Notify( eShopWithDraw );	
}


