#include "stdafx.h"
#include "EquipSystemContents.h"

cEquipSystemContents::cEquipSystemContents() :
m_pNpc( NULL ),
m_nPortableIndex( 0 ),
m_eCurTab( eTabState::eTabNone ),
m_nSelectChargeSlot( -1 ),
m_bWaitRecvPacket( false ),
m_fDeltaTime( 0.0f )
{
	m_niOpenPos = NiPoint2( 0, 0 );

#ifdef LJW_EQUIPSYSTEM_190724
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_EQUIPSYSTEM_NPC, this, &cEquipSystemContents::OpenEquipSystemNpc );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_EQUIPSYSTEM_PORTABLE, this, &cEquipSystemContents::OpenEquipSystemPortable );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CLOSE_EQUIPSYSTEM, this, &cEquipSystemContents::CloseEquipSystem );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHARGE_ITEM_LIST, this, &cEquipSystemContents::RecvChargeListResult );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHARGE_ITEM_NORMAL_RESULT, this, &cEquipSystemContents::RecvNormalChargeTimeResult );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHARGE_ITEM_QUICK_RESULT, this, &cEquipSystemContents::RecvQuickChargeTimeResult );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_CHARGE_ITEM_CANCEL, this, &cEquipSystemContents::SendNormalChargeCancel );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHARGE_ITEM_CANCEL, this, &cEquipSystemContents::RecvNormalChargeCancel );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_CHARGE_ITEM_WITHDRAW, this, &cEquipSystemContents::SendWithdrawChargeItme );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHARGE_ITEM_WITHDRAW, this, &cEquipSystemContents::RecvWithdrawChargeItemResult );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_CHARGE_ITEM_UNLOCK, this, &cEquipSystemContents::SendUnlockChargeList );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_CHARGE_ITEM_UNLOCK, this, &cEquipSystemContents::RecvUnlockChargeListResult );
#else
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_ELEMENT_ITEM_NPC, this, &cEquipSystemContents::OpenElementItemNpc );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_ELEMENT_ITEM_PORTABLE, this, &cEquipSystemContents::OpenElementItemPortable );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CLOSE_ELEMENT_ITEM, this, &cEquipSystemContents::CloseElementItem );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::REGIST_ELEMENT_ITEM_CHECK, this, &cEquipSystemContents::RegistElementCheckItem );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REGIST_ELEMENT_ITEM_EQUIP, this, &cEquipSystemContents::RegistElementAttachItem );
#endif

	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_ELEMENT_ITEM_CHECK, this, &cEquipSystemContents::AnalysisElementItem );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ELEMENT_ITEM_CHECK, this, &cEquipSystemContents::RecvElementCheckSuccess );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ELEMENT_ITEM_ATTACH, this, &cEquipSystemContents::RecvElementAttachSuccess );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ELEMENT_ITEM_DETACH, this, &cEquipSystemContents::RecvElementDetachSuccess );

	if( EventRouter() )
		EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Inventory_UseItem, this );
}

cEquipSystemContents::~cEquipSystemContents()
{
	GAME_EVENT_ST.DeleteEventAll( this );
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
}

int const cEquipSystemContents::IsContentsIdentity(void)
{
	return E_CT_ELEMENTITEM_CONTENTS;
}

int const cEquipSystemContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool cEquipSystemContents::Initialize(void)
{
	return true;
}

void cEquipSystemContents::UnInitialize(void)
{

}

bool cEquipSystemContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void cEquipSystemContents::Update(float const& fElapsedTime)
{
	// 시간 갱신
	m_fDeltaTime += fElapsedTime;
	if( m_fDeltaTime >= 1.0f )
	{
		m_fDeltaTime = 0.0f;
		_UpdateChargeTime();
	}

	if( m_niOpenPos != NiPoint2( 0, 0 ) )
	{
		if( g_pCharMng )
		{
			CTamerUser* pTamer = g_pCharMng->GetTamerUser();
			if( pTamer )
			{
				if( ( m_niOpenPos - pTamer->GetPos2D() ).Length() > IF_TARGET_TAMER_MAX_DIST )
					CloseEquipSystem( NULL );
			}
		}
	}

	if( m_pNpc && !m_pNpc->CheckTamerDist() )
	{
#ifdef LJW_EQUIPSYSTEM_190724
		CloseEquipSystem( NULL );
#else
		CloseElementItem( NULL );
#endif
	}
}

void cEquipSystemContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void cEquipSystemContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Inventory_UseItem:
		{
#ifdef LJW_EQUIPSYSTEM_190724
			int nInvenIndex = 0;
			kStream >> nInvenIndex;
			RegisterItemEquipSystem( nInvenIndex );
#endif
		}
		break;
	}
}

void cEquipSystemContents::MakeMainActorData(void)
{

}

void cEquipSystemContents::ClearMainActorData(void)
{
	m_sElemEquip.eState = eUIState::eNoneState;
	m_sElemCheck.eState = eUIState::eNoneState;
#ifdef LJW_EQUIPSYSTEM_190724
	CloseEquipSystem( NULL );
#else
	CloseElementItem( NULL );
#endif
}

void cEquipSystemContents::MakeWorldData(void)
{

}

void cEquipSystemContents::ClearWorldData(void)
{

}

void cEquipSystemContents::AnalysisElementItem(void* pData)
{
	int nFTID = 0;
	if( m_pNpc )
		nFTID = m_pNpc->GetFTID();

	if( -1 == m_nPortableIndex )
		net::game->SendItemAnalysis( nFTID, m_sElemCheck.nRegistItemIdx, 0, true );
	else
		net::game->SendItemAnalysis( nFTID, m_sElemCheck.nRegistItemIdx, m_nPortableIndex );

	m_sElemCheck.eState = eUIState::eWaitRecvData;
}

void cEquipSystemContents::RecvElementCheckSuccess(void* pData)
{
	SAFE_POINTER_RET( pData );
	ContentsStream kStream = *static_cast< ContentsStream* >( pData );

	u1 nRate = 0;
	n8 nRemainMoney = 0;
	kStream >> nRate >> nRemainMoney;

	m_sElemCheck.eState = eUIState::eNoneState;

	// 인벤 돈 감소
	sMONEY sRemainMoney = _DecreaseInvenMoney( m_sElemCheck.nNeedMoney );
	// 남은 돈 서버 데이터랑 확인
	assert_csm2( sRemainMoney.GetInt() == nRemainMoney, _T( "Client = %d, Server = %d" ), sRemainMoney.GetInt(), nRemainMoney );
	if( sRemainMoney.GetInt() != nRemainMoney )
	{
		if( g_pDataMng )
		{
			cData_Inven* pInven = g_pDataMng->GetInven();
			if( pInven )
				pInven->SetMoney( nRemainMoney, true ); // 일단 서버에서 준 정보로 세팅
		}
	}

	// 속성 아이템 레이트 세팅
	cItemInfo* pElemItem = GetInvenItem( m_sElemCheck.nRegistItemIdx );
	if( pElemItem )
		pElemItem->m_nRate = nRate;

	// 만약 UI 닫혀 있으면
#ifdef LJW_EQUIPSYSTEM_190724
	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_EQUIPMENT_SYSTEM ) )
#else
	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_ELEMENTITEM_CHECK ) )
#endif
	{
		m_pNpc = NULL;
		m_nPortableIndex = 0;
		m_eCurTab = eTabState::eTabNone;
		m_sElemCheck.Reset();
		return;
	}

	ContentsStream kSend;
	kSend << nRate;
	Notify( eSuccessAnalysis, kSend );

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	pUser->SetAnimation( ANI::NPC_YES );
	pUser->PlaySound( "_Quest\\Quest_Mission.wav" );
	CsC_EffectProp* pEftProp = pUser->GetProp_Effect();
	SAFE_POINTER_RET( pEftProp );
	pEftProp->AddEffect( "system\\TradeEnd.nif", 1.0f, nsEFFECT::POS_CHARPOS );
}

void cEquipSystemContents::OpenElementItemNpc(void* pData)
{
	SAFE_POINTER_RET( pData );
	pair< int, CsC_AvObject* > pairRecv = *static_cast< pair< int, CsC_AvObject* >* >( pData );

	CsC_AvObject* pTarget = pairRecv.second;
	SAFE_POINTER_RET( pTarget );
	if( RTTI_NPC != pTarget->GetLeafRTTI() )
		return;

	SAFE_POINTER_RET( g_pGameIF );

	// 같이 켜져 있을 수 없다
	switch( pairRecv.first )
	{
	case cBaseWindow::WT_ELEMENTITEM_CHECK:
		m_sElemEquip.Reset();
		break;
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
		m_sElemCheck.Reset();
		break;
	}

	m_pNpc = dynamic_cast< CNpc* >( pTarget );
	m_nPortableIndex = 0;

	g_pGameIF->GetDynamicIF( static_cast< cBaseWindow::eWINDOW_TYPE >( pairRecv.first ) );

	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
		{
			pInven->ItemLock_ItemType( nItem::Portable_Item, eANALYSOR );
			pInven->ItemLock_ItemType( nItem::Portable_Item, eFUSION );
		}
	}

	if( g_pNpcMng )
		g_pNpcMng->SetZoomNpcType( pairRecv.first );

	bool bSound = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );
}

void cEquipSystemContents::OpenElementItemPortable(void* pData)
{
	SAFE_POINTER_RET( pData );
	pair< int, int > pairRecv = *static_cast< pair< int, int >* >( pData );

	SAFE_POINTER_RET( g_pGameIF );

	// 같이 켜져 있을 수 없다
	switch( pairRecv.first )
	{
	case cBaseWindow::WT_ELEMENTITEM_CHECK:
		m_sElemEquip.Reset();
		break;
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
		m_sElemCheck.Reset();
		break;
	}

	m_pNpc = NULL;
	m_nPortableIndex = pairRecv.second;

	g_pGameIF->GetDynamicIF( static_cast< cBaseWindow::eWINDOW_TYPE >( pairRecv.first ) );

	if( g_pNpcMng )
	{
		g_pNpcMng->ResetZoomNpc();
		g_pNpcMng->SetZoomNpcType( pairRecv.first );
	}

	bool bSound = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );
}

void cEquipSystemContents::CloseElementItem(void* pData)
{
	m_pNpc = NULL;
	m_nPortableIndex = 0;

	m_sElemCheck.Reset();
	m_sElemEquip.Reset();

	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
		{
			pInven->ItemUnlock_ItemType( nItem::Portable_Item, eANALYSOR );
			pInven->ItemUnlock_ItemType( nItem::Portable_Item, eFUSION );
		}
	}

	if( g_pNpcMng )
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_NONE );
}

void cEquipSystemContents::RegistElementCheckItem(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nInvenIndex = *static_cast< int* >( pData );

	SetRegistAnalysis( nInvenIndex );
}

bool cEquipSystemContents::_CheckEnableRegist_Check(int const& nInvenIndex)
{
	// 분석 중인 경우 || 서버에 데이터 보내고 대기 상태
	switch( m_sElemCheck.eState )
	{
	case eUIState::eProcessing:
	case eUIState::eWaitRecvData:
		cPrintMsg::PrintMsg( 10004 );
		return false;
	}

	// 등록하려는 아이템 확인
	if( !_CheckEnableItem( nInvenIndex ) )
		return false;

	cItemInfo* pInvenItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RETVAL( pInvenItem, false );

	CsItem::sINFO* pFTInfo = GetFTItemInfo( pInvenItem->GetType() );
	SAFE_POINTER_RETVAL( pFTInfo, false );

	// 속성 아이템이 아닌 경우
	if( ( pFTInfo->s_nType_L < nItem::EItem_All )
		|| ( pFTInfo->s_nType_L > nItem::EItem_Glass ) )
	{
		cPrintMsg::PrintMsg( 11014 );
		return false;
	}

	// 이미 분석된 아이템인 경우
	if( 0 != pInvenItem->m_nRate )
	{
		cPrintMsg::PrintMsg( 11025 );
		return false;
	}

	return true;
}

bool cEquipSystemContents::_CheckEnableMoney(u8 const& nNeedMoney)
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );
	if( pInven->GetMoney() < nNeedMoney )
		return false;
	return true;
}

bool cEquipSystemContents::_CheckEnableItem(int const& nInvenIndex)
{
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	if( pInven->GetInvenSlotCount() <= nInvenIndex )
		return false;

	cItemInfo* pRegistItem = pInven->GetData( nInvenIndex );
	SAFE_POINTER_RETVAL( pRegistItem, false );
	return pRegistItem->IsEnable();
}

void cEquipSystemContents::SetRegistAnalysis(int const& nInvenIndex)
{
	if( !_CheckEnableRegist_Check( nInvenIndex ) )
		return;

	// 등록되어 있는 아이템 확인
	if( _CheckEnableItem( m_sElemCheck.nRegistItemIdx ) )
		m_sElemCheck.ResetData();

	uint nItemType = GetInvenItemType( nInvenIndex );
	if( 0 == nItemType )
	{
		Notify( eResetWindow );
		return;
	}

	// 아이템 등록 + 등록한 아이템 락
	m_sElemCheck.nRegistItemIdx = nInvenIndex;
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->Itemlock_InvenSlot( m_sElemCheck.nRegistItemIdx );
	}

	// 분석 필요 금액 계산
	m_sElemCheck.nNeedMoney = _CalcNeedMoney( nItemType );
	bool bEnable = _CheckEnableMoney( m_sElemCheck.nNeedMoney );

	std::wstring wsItemName;
	CsItem::sINFO* pFTInfo = GetFTItemInfo( nItemType );
	if( pFTInfo )
		wsItemName = pFTInfo->s_szName;
	if( wsItemName.empty() )
		wsItemName = L"???";

	ContentsStream kSend;
	kSend << nItemType << m_sElemCheck.nNeedMoney << bEnable << wsItemName << m_sElemCheck.nRegistItemIdx;
	Notify( eRegistCheckItem, kSend );
}

bool cEquipSystemContents::StartAnalysis()
{
	if( !_CheckEnableItem( m_sElemCheck.nRegistItemIdx ) )
		return false;

	if( !_CheckEnableMoney( m_sElemCheck.nNeedMoney ) )
		return false;

	m_sElemCheck.eState = eUIState::eProcessing;
	return true;
}

void cEquipSystemContents::EndAnalysis()
{
	int nFTID = 0;
	if( m_pNpc )
		nFTID = m_pNpc->GetFTID();

	SAFE_POINTER_RET( g_pDataMng );
	cData_ServerSync* pSrvSync = g_pDataMng->GetServerSync();
	SAFE_POINTER_RET( pSrvSync );
	pSrvSync->SendServer( cData_ServerSync::ELEMENT_ITEM_CHECK, 0 );
}

bool cEquipSystemContents::CancelAnalysis()
{
	// 이미 패킷을 보내고 대기 중이면
	if( eUIState::eWaitRecvData == m_sElemCheck.eState )
	{
		cPrintMsg::PrintMsg( 10004 );
		return false;
	}

	m_sElemCheck.eState = eNoneState;
	return true;
}

void cEquipSystemContents::ResetRegistItem_Check()
{
	m_sElemCheck.ResetData();
	Notify( eResetWindow );
}

void cEquipSystemContents::ResetRegistItem_Equip()
{
	m_sElemEquip.ResetData();
	Notify( eResetWindow );
}

cItemInfo* cEquipSystemContents::GetRegistItem_Check()
{
	return GetInvenItem( m_sElemCheck.nRegistItemIdx );
}

cItemInfo* cEquipSystemContents::GetRegistItem_Equip()
{
	return GetInvenItem( m_sElemEquip.nRegistItemIdx );
}

CsItem::sINFO* cEquipSystemContents::GetFTItemInfo(int const& nItemType)
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pItemMng, NULL );
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( nItemType );
	SAFE_POINTER_RETVAL( pFTItem, NULL );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RETVAL( pFTInfo, NULL );
	return pFTInfo;
}

cItemInfo* cEquipSystemContents::GetInvenItem(int const& nInvenIndex)
{
	SAFE_POINTER_RETVAL( g_pDataMng, NULL );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, NULL );

	if( pInven->GetInvenSlotCount() <= nInvenIndex )
		return NULL;

	cItemInfo* pRegistItem = pInven->GetData( nInvenIndex );
	SAFE_POINTER_RETVAL( pRegistItem, NULL );
	if( !pRegistItem->IsEnable() )
		return NULL;

	return pRegistItem;
}

cItemInfo* cEquipSystemContents::GetInvenItem_NotCheckEnable(int const& nInvenIndex)
{
	SAFE_POINTER_RETVAL( g_pDataMng, NULL );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, NULL );

	if( pInven->GetInvenSlotCount() <= nInvenIndex )
		return NULL;

	return pInven->GetData( nInvenIndex );
}

uint cEquipSystemContents::GetInvenItemType(int const& nInvenIndex)
{
	cItemInfo* pRegistItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RETVAL( pRegistItem, 0 );
	return pRegistItem->GetType();
}

u8 cEquipSystemContents::_CalcNeedMoney(uint nItemType, bool bIsDetach /*= false*/)
{
	CsItem::sINFO* pFTInfo = GetFTItemInfo( nItemType );
	SAFE_POINTER_RETVAL( pFTInfo, 0 );

	float fCalc = 0.5f;
	if( bIsDetach )
		fCalc = 1.5f;

	return static_cast< u8 >( pFTInfo->s_dwScanPrice * fCalc );
}

sMONEY cEquipSystemContents::_DecreaseInvenMoney(u8 const& nNeedMoney)
{
	sMONEY sRemain( (u8)0 );
	SAFE_POINTER_RETVAL( g_pDataMng, sRemain );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, sRemain );
	sRemain = pInven->DecreaseMoney( nNeedMoney );
	return sRemain;
}

int cEquipSystemContents::_GetEmptyInvenSlot()
{
	SAFE_POINTER_RETVAL( g_pDataMng, cData_Inven::INVALIDE_INVEN_INDEX );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, cData_Inven::INVALIDE_INVEN_INDEX );
	return pInven->FindEmptySlot( -1 );
}

void cEquipSystemContents::_SetLimited(cItemInfo* pRegistItem)
{
	SAFE_POINTER_RET( pRegistItem );
	if( !pRegistItem->IsEnable() )
		return;

	if( 0 != pRegistItem->m_nLimited )
		return;

	CsItem::sINFO* pFTInfo = GetFTItemInfo( pRegistItem->GetType() );
	SAFE_POINTER_RET( pFTInfo );
	if( 0 == pFTInfo->s_nBelonging )
		pRegistItem->m_nLimited = 0;
	else
		pRegistItem->m_nLimited = 1;
}

/////////////////////////////////////////////////////////////////////////////////////
void cEquipSystemContents::RecvElementAttachSuccess(void* pData)
{
	if( 0 < --m_sElemEquip.nWaitRecvAttach )
		return;

	m_sElemEquip.eState = eNoneState;

	// 인벤 돈 감소
	_DecreaseInvenMoney( m_sElemEquip.nNeedAttachMoney );
	m_sElemEquip.nNeedAttachMoney = 0;

	cItemInfo* pRegistItem = GetInvenItem( m_sElemEquip.nRegistItemIdx );
	if( !pRegistItem )
	{
		m_sElemEquip.eState = eUIState::eNoneState;
		m_sElemEquip.Reset();
		return;
	}

	// 장비 + 소켓 합체
	std::map< int, sSocketInfo >::iterator it = m_sElemEquip.mapSocketItems.begin();
	std::map< int, sSocketInfo >::iterator itEnd = m_sElemEquip.mapSocketItems.end();
	for( ; it != itEnd; ++it )
	{
		if( eSocketState::eRegist == it->second.eState )
		{
			it->second.eState = eSocketState::eAttached;
			cItemInfo* pSocketItem = GetInvenItem( it->second.nInvenIndex );
			SAFE_POINTER_CON( pSocketItem );
			pRegistItem->m_nSockItemType[ it->first ] = pSocketItem->GetType();
			pRegistItem->m_nSockAppRate[ it->first ] = pSocketItem->m_nRate;
			pSocketItem->DecreaseCount( 1 );

			// 귀속 여부 체크
			_SetLimited( pSocketItem );

			if( g_pDataMng )
				g_pDataMng->ItemUnlock( it->second.nInvenIndex );

			it->second.nInvenIndex = -1;
		}
	}

	// 만약 UI 닫혀 있으면
#ifdef LJW_EQUIPSYSTEM_190724
	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_EQUIPMENT_SYSTEM ) )
#else
	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_ELEMENTITEM_ATTACH ) )
#endif
	{
		m_pNpc = NULL;
		m_nPortableIndex = 0;
		m_sElemEquip.Reset();
		return;
	}

	Notify( eSuccessAttach );

	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pUser );
	pUser->SetAnimation( ANI::NPC_YES );
	pUser->PlaySound( "System\\System_quest_01.wav" );
	CsC_EffectProp* pEftProp = pUser->GetProp_Effect();
	SAFE_POINTER_RET( pEftProp );
	pEftProp->AddEffect( "System\\Tactics_success.nif", 1.0f, nsEFFECT::POS_CHARPOS );
}

void cEquipSystemContents::RecvElementDetachSuccess(void* pData)
{
	m_sElemEquip.eState = eNoneState;

	// 인벤 돈 감소
	_DecreaseInvenMoney( m_sElemEquip.nNeedDetachMoney );

	cItemInfo* pRegistItem = GetInvenItem( m_sElemEquip.nRegistItemIdx );
	if( !pRegistItem )
	{
		m_sElemEquip.eState = eUIState::eNoneState;
		m_sElemEquip.Reset();
		return;
	}

	cItemInfo* pEmptySlot = GetInvenItem_NotCheckEnable( m_sElemEquip.nEmptyInvenIndex );
	if( !pEmptySlot )
	{
		m_sElemEquip.eState = eUIState::eNoneState;
		m_sElemEquip.Reset();
		return;
	}

	if( pEmptySlot->IsEnable() )
	{
		m_sElemEquip.eState = eUIState::eNoneState;
		m_sElemEquip.Reset();
		return;
	}

	// 장비 - 소켓 해제
	std::map< int, sSocketInfo >::iterator it = m_sElemEquip.mapSocketItems.find( m_sElemEquip.nSelectedSocket );
	if( it != m_sElemEquip.mapSocketItems.end() )
	{
		pEmptySlot->m_nType = pRegistItem->m_nSockItemType[ m_sElemEquip.nSelectedSocket ];
		pEmptySlot->m_nCount = 1;
		pEmptySlot->m_nRate = pRegistItem->m_nSockAppRate[ m_sElemEquip.nSelectedSocket ];
		CsItem::sINFO* pFTInfo = GetFTItemInfo( pEmptySlot->m_nType );
		if( pFTInfo )
			pEmptySlot->m_nLimited = ( 0 == pFTInfo->s_nBelonging ) ? 0 : 1;

		pRegistItem->m_nSockItemType[ m_sElemEquip.nSelectedSocket ] = 0;
		pRegistItem->m_nSockAppRate[ m_sElemEquip.nSelectedSocket ] = 0;

		it->second.Reset();
	}

	if( g_pDataMng )
		g_pDataMng->ItemUnlock( m_sElemEquip.nEmptyInvenIndex );

	// 만약 UI 닫혀 있으면
#ifdef LJW_EQUIPSYSTEM_190724
	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_EQUIPMENT_SYSTEM ) )
#else
	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_ELEMENTITEM_ATTACH ) )
#endif
	{
		m_pNpc = NULL;
		m_nPortableIndex = 0;
		m_sElemEquip.Reset();
		return;
	}

	Notify( eSuccessDetach );
}

void cEquipSystemContents::RegistElementAttachItem(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nInvenIndex = *static_cast< int* >( pData );

	// 패킷 기다리는 중
	if( eUIState::eWaitRecvData == m_sElemEquip.eState )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	eRegistState eState = _CheckRegistItemKind( nInvenIndex );
	switch( eState )
	{
	case eRegistState::eElem:
		SetRegistAttach_Elem( nInvenIndex );
		return;
	case eRegistState::eEquip:
		SetRegistAttach_Equip( nInvenIndex );
		return;
	}
}

cEquipSystemContents::eRegistState cEquipSystemContents::_CheckRegistItemKind(int const& nInvenIndex)
{
	uint nItemType = GetInvenItemType( nInvenIndex );
	CsItem::sINFO* pFTInfo = GetFTItemInfo( nItemType );
	SAFE_POINTER_RETVAL( pFTInfo, eRegistState::eNotRegist );

	switch( pFTInfo->s_nType_L )
	{
	case nItem::Head:
	case nItem::Glass:
	case nItem::Coat:
	case nItem::Glove:
	case nItem::Pants:
	case nItem::Shoes:
	//case nItem::EquipAura:
		return eRegistState::eEquip;// 장비

	case nItem::EItem_All:
	case nItem::EItem_Glass:
	case nItem::EItem_Coat:
	case nItem::EItem_Glove:
	case nItem::EItem_Pants:
	case nItem::EItem_Shoes:
		return eRegistState::eElem;	// 속성템

#if COMMON_LIB_FIXED
	case nItem::ItemCharge:
		return eRegistState::eCharge;	// 충전 아이템
#endif
	}

	return eRegistState::eNotRegist;
}

bool cEquipSystemContents::_CheckEnableEquip(CsItem::sINFO* pFTInfo)
{
	SAFE_POINTER_RETVAL( pFTInfo, false );

// 	if( nItem::EquipAura == pFTInfo->s_nType_L )
// 	{
// 		cPrintMsg::PrintMsg( 11014 );
// 		return false;
// 	}

	if( 0 >= pFTInfo->s_nSocketCount )	// 소켓은 꼭 있어야 한다.	
		return false;

	return true;
}

bool cEquipSystemContents::_CheckEnableElement(u2 nElemTypeL, u1 nElemRate)
{
	// 등록된 장비가 없으면
	if( !_CheckEnableItem( m_sElemEquip.nRegistItemIdx ) )
	{
		cPrintMsg::PrintMsg( 11033 );
		return false;
	}

	cItemInfo* pRegistItem = GetRegistItem_Equip();
	SAFE_POINTER_RETVAL( pRegistItem, false );
	CsItem::sINFO* pFTRegistInfo = GetFTItemInfo( pRegistItem->GetType() );
	SAFE_POINTER_RETVAL( pFTRegistInfo, false );

	// 분석 안 된 속성 아이템
	if( 0 == nElemRate )
	{
		cPrintMsg::PrintMsg( 11035 );
		return false;
	}

	// 등록된 장비에 비어 있는 소켓이 있는지
	int nSoketIndex = _GetSocketEmptySlot();
	if( -1 == nSoketIndex )
	{
		cPrintMsg::PrintMsg( 11032 );
		return false;
	}

	// 부위별 체크
	switch( nElemTypeL )
	{
	case nItem::EItem_All:
		break;	
	case nItem::EItem_Coat:
	case nItem::EItem_Glove:
	case nItem::EItem_Pants:
	case nItem::EItem_Shoes:
		{
			// 속성 대분류 값 - 장비 대분류 값 = 100
			if( ( nElemTypeL - pFTRegistInfo->s_nType_L ) != 100 )
			{
				cPrintMsg::PrintMsg( 11039 );
				return false;
			}
		}
		break;
	case nItem::EItem_Glass:
		switch( pFTRegistInfo->s_nType_L )
		{
		case nItem::Head:
		case nItem::Glass:
			break;
		default:
			cPrintMsg::PrintMsg( 11039 );
			return false;
		}
		break;
	default:
		assert_cs( false );
	}

	return true;
}

bool cEquipSystemContents::_CheckEnableSocket(int const& nSocketIndex)
{
	if( 0 > nSocketIndex )
		return false;

	if( nLimit::SocketSlot <= nSocketIndex )
		return false;

	if( m_sElemEquip.mapSocketItems.size() <= m_sElemEquip.nSelectedSocket )
		return false;

	return true;
}

bool cEquipSystemContents::_CheckReleaseAllSocket()
{
	std::map< int, sSocketInfo >::iterator itChck = m_sElemEquip.mapSocketItems.begin();
	std::map< int, sSocketInfo >::iterator itChckEnd = m_sElemEquip.mapSocketItems.end();
	for( ; itChck != itChckEnd; ++itChck )
	{
		if( eSocketState::eRegist == itChck->second.eState )
			return false;
	}

	return true;
}

void cEquipSystemContents::_LoadAttachedSocket(int nSocketCount, cItemInfo* pInvenItem)
{
	m_sElemEquip.mapSocketItems.clear();

	if( nLimit::SocketSlot < nSocketCount )
		return;

	for( int i = 0; i < nSocketCount; ++i )
	{
		sSocketInfo sSocket;
		if( 0 != pInvenItem->m_nSockItemType[ i ] )
			sSocket.Set( eSocketState::eAttached, pInvenItem->m_nSockItemType[ i ], pInvenItem->m_nSockAppRate[ i ], -1 );
		m_sElemEquip.mapSocketItems.insert( std::make_pair( i, sSocket ) );
	}
}

int cEquipSystemContents::_GetSocketEmptySlot()
{
	std::map< int, sSocketInfo >::iterator it = m_sElemEquip.mapSocketItems.begin();
	std::map< int, sSocketInfo >::iterator itEnd = m_sElemEquip.mapSocketItems.end();
	for( ; it != itEnd; ++it )
	{
		// 비어 있는 소켓
		if( eSocketState::eNoneRegist == it->second.eState )
			return it->first;
	}

	return -1;
}

void cEquipSystemContents::AttachElementItem()
{
	SAFE_POINTER_RET( net::game );

	// 패킷 기다리는 중
	if( eUIState::eWaitRecvData == m_sElemEquip.eState )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	if( !_CheckEnableItem( m_sElemEquip.nRegistItemIdx ) )
		return;

	std::map< int, sSocketInfo >::iterator it = m_sElemEquip.mapSocketItems.begin();
	std::map< int, sSocketInfo >::iterator itEnd = m_sElemEquip.mapSocketItems.end();
	for( ; it != itEnd; ++it )
	{
		if( eSocketState::eRegist == it->second.eState )
		{
			uint nNpcFTID = 0;
			if( m_pNpc )
				nNpcFTID = m_pNpc->GetFTID();

			if( -1 == m_nPortableIndex )
				net::game->SendSocketIn( nNpcFTID, it->second.nInvenIndex, m_sElemEquip.nRegistItemIdx, it->first, 0, true );
			else
				net::game->SendSocketIn( nNpcFTID, it->second.nInvenIndex, m_sElemEquip.nRegistItemIdx, it->first, m_nPortableIndex );

			m_sElemEquip.nWaitRecvAttach++;			// 대기 중인 패킷 개수
			m_sElemEquip.eState = eUIState::eWaitRecvData;
		}
	}
}

void cEquipSystemContents::DetachElementItem()
{
	SAFE_POINTER_RET( net::game );

	// 패킷 기다리는 중
	if( eUIState::eWaitRecvData == m_sElemEquip.eState )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	if( !_CheckEnableItem( m_sElemEquip.nRegistItemIdx ) )
		return;

	// 선택된 소켓이 없다
	if( !_CheckEnableSocket( m_sElemEquip.nSelectedSocket ) )
		return;

	// 인벤 빈 슬롯 있는지
	m_sElemEquip.nEmptyInvenIndex = _GetEmptyInvenSlot();
	if( cData_Inven::INVALIDE_INVEN_INDEX == m_sElemEquip.nEmptyInvenIndex )
	{
		// 인벤 빈 슬롯 없음
		cPrintMsg::PrintMsg( 11015 );
		return;
	}

	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->Itemlock_InvenSlot( m_sElemEquip.nEmptyInvenIndex );
	}

	uint nNpcFTID = 0;
	if( m_pNpc )
		nNpcFTID = m_pNpc->GetFTID();

	if( -1 == m_nPortableIndex )
		net::game->SendSocketOut( nNpcFTID, m_sElemEquip.nRegistItemIdx, m_sElemEquip.nEmptyInvenIndex, m_sElemEquip.nSelectedSocket, 0, true );
	else
		net::game->SendSocketOut( nNpcFTID, m_sElemEquip.nRegistItemIdx, m_sElemEquip.nEmptyInvenIndex, m_sElemEquip.nSelectedSocket, m_nPortableIndex );

	m_sElemEquip.eState = eUIState::eWaitRecvData;
}

bool cEquipSystemContents::SetRegistAttach_Equip(int const& nInvenIndex)
{
	if( eRegistState::eEquip != _CheckRegistItemKind( nInvenIndex ) )
	{
		cPrintMsg::PrintMsg( 11014 );
		return false;
	}

	cItemInfo* pInvenItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RETVAL( pInvenItem, false );

	CsItem::sINFO* pFTInfo = GetFTItemInfo( pInvenItem->GetType() );
	SAFE_POINTER_RETVAL( pFTInfo, false );

	if( !_CheckEnableEquip( pFTInfo ) )
		return false;

	// 등록된 장비 지우고
	m_sElemEquip.ResetData();

	// 등록할 장비 인덱스 저장
	m_sElemEquip.nRegistItemIdx = nInvenIndex;

	// 아이템 락
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->Itemlock_InvenSlot( m_sElemEquip.nRegistItemIdx );
	}

	// 새로운 장비의 소켓 정보 로드
	_LoadAttachedSocket( pFTInfo->s_nSocketCount, pInvenItem );

	uint nItemType = pInvenItem->GetType();
	int nSocketCount = pFTInfo->s_nSocketCount;
	std::wstring wsItemName = pFTInfo->s_szName;
	if( wsItemName.empty() )
		wsItemName = L"???";
	ContentsStream kSend;
	kSend << m_sElemEquip.nRegistItemIdx << nSocketCount << wsItemName << nItemType;
	Notify( eRegistEquip, kSend );

	return true;
}

bool cEquipSystemContents::SetRegistAttach_Elem(int const& nInvenIndex)
{
	if( eRegistState::eElem != _CheckRegistItemKind( nInvenIndex ) )
	{
		cPrintMsg::PrintMsg( 11014 );
		return false;
	}

	cItemInfo* pInvenItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RETVAL( pInvenItem, false );

	CsItem::sINFO* pFTInfo = GetFTItemInfo( pInvenItem->GetType() );
	SAFE_POINTER_RETVAL( pFTInfo, false );

	if( !_CheckEnableElement( pFTInfo->s_nType_L, pInvenItem->m_nRate ) )
		return false;

	int nSocketIndex = _GetSocketEmptySlot();
	if( 0 > nSocketIndex )
		return false;

	std::map< int, sSocketInfo >::iterator it = m_sElemEquip.mapSocketItems.find( nSocketIndex );
	if( it == m_sElemEquip.mapSocketItems.end() )
		return false;

	it->second.Set( eSocketState::eRegist, pInvenItem->GetType(), pInvenItem->m_nRate, nInvenIndex );

	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->Itemlock_InvenSlot( nInvenIndex );
	}

	m_sElemEquip.nNeedAttachMoney += _CalcNeedMoney( pInvenItem->GetType() );
	bool bEnable = _CheckEnableMoney( m_sElemEquip.nNeedAttachMoney );

	ContentsStream kSend;
	kSend << nSocketIndex << m_sElemEquip.nNeedAttachMoney << bEnable;
	Notify( eRegistElem, kSend );

	return true;
}

void cEquipSystemContents::ReleaseRegistSocket(int const& nSocketIndex)
{
	// 패킷 기다리는 중
	if( eUIState::eWaitRecvData == m_sElemEquip.eState )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	// 등록된 장비 있는지
	if( !_CheckEnableItem( m_sElemEquip.nRegistItemIdx ) )
		return;

	std::map< int, sSocketInfo >::iterator it = m_sElemEquip.mapSocketItems.find( nSocketIndex );
	if( it == m_sElemEquip.mapSocketItems.end() )
		return;

	if( eSocketState::eRegist != it->second.eState )
		return;

	// 합성 필요 금액 차감
	m_sElemEquip.nNeedAttachMoney -= _CalcNeedMoney( it->second.nItemType );
	bool bEnable = _CheckEnableMoney( m_sElemEquip.nNeedAttachMoney );

	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->ItemUnlock_InvenSlot( it->second.nInvenIndex );
	}

	it->second.Reset();

	// 소켓 전부 해제했는지 확인
	bool bReleaseAll = _CheckReleaseAllSocket();

	ContentsStream kSend;
	kSend << nSocketIndex << m_sElemEquip.nNeedAttachMoney << bEnable << bReleaseAll;
	Notify( eReleaseSocket, kSend );
}

cEquipSystemContents::sSocketInfo* cEquipSystemContents::GetSocketInfo(int const& nSocketIndex)
{
	std::map< int, sSocketInfo >::iterator it = m_sElemEquip.mapSocketItems.find( nSocketIndex );
	if( it == m_sElemEquip.mapSocketItems.end() )
		return NULL;

	return &( it->second );
}

cItemInfo cEquipSystemContents::GetSocketItem(int const& nSocketIndex)
{
	cItemInfo info;

	cItemInfo* pRegistItem = GetInvenItem( m_sElemEquip.nRegistItemIdx );
	SAFE_POINTER_RETVAL( pRegistItem, info );

	// 등록된 장비에서 소켓 정보 복사해온다
	info.m_nType = pRegistItem->m_nSockItemType[ nSocketIndex ];
	info.m_nRate = pRegistItem->m_nSockAppRate[ nSocketIndex ];
	info.m_nCount = 1;
	_SetLimited( &info );

	return info;
}

void cEquipSystemContents::SelectAttachedSocket(int const& nSocketIndex)
{
	// 패킷 기다리는 중
	if( eUIState::eWaitRecvData == m_sElemEquip.eState )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	// 등록된 아이템이 없으면
	if( !_CheckEnableItem( m_sElemEquip.nRegistItemIdx ) )
		return;

	// 등록된 소켓 존재하는지
	std::map< int, sSocketInfo >::iterator it = m_sElemEquip.mapSocketItems.find( nSocketIndex );
	if( it == m_sElemEquip.mapSocketItems.end() )
		return;

	if( eSocketState::eAttached != it->second.eState )
		return;

	m_sElemEquip.nSelectedSocket = it->first;

	// 해제 비용 계산
	m_sElemEquip.nNeedDetachMoney = _CalcNeedMoney( it->second.nItemType, true );
	bool bEnable = _CheckEnableMoney( m_sElemEquip.nNeedDetachMoney );

	ContentsStream kSend;
	kSend << m_sElemEquip.nSelectedSocket << m_sElemEquip.nNeedDetachMoney << bEnable;
	Notify( eSelectSocket, kSend );
}

std::string cEquipSystemContents::GetSlotImagePath()
{
	uint nItemType = GetInvenItemType( m_sElemEquip.nRegistItemIdx );
	CsItem::sINFO* pFTInfo = GetFTItemInfo( nItemType );
	SAFE_POINTER_RETVAL( pFTInfo, "" );

	switch( pFTInfo->s_nType_L + 100 )
	{
#if COMMON_LIB_FIXED
	case nItem::EItem_Head:
#endif
	case nItem::EItem_Glass:
		return "EI_Item\\Outbox_1.tga";

	case nItem::EItem_Coat:		return "EI_Item\\Outbox_2.tga";
	case nItem::EItem_Glove:	return "EI_Item\\Outbox_3.tga";
	case nItem::EItem_Pants:	return "EI_Item\\Outbox_4.tga";
	case nItem::EItem_Shoes:	return "EI_Item\\Outbox_5.tga";
	}

	return "";
}
/////////////////////////////////////////////////////////////////////////////////
void cEquipSystemContents::sElementCheck::Reset()
{
#ifndef LJW_EQUIPSYSTEM_190724
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ELEMENTITEM_CHECK );
#endif

	if( eUIState::eWaitRecvData == eState )
		return;

	eState = eNoneState;
	ResetData();
}

void cEquipSystemContents::sElementCheck::ResetData()
{
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->ItemUnlock_InvenSlot( nRegistItemIdx );
	}
	nRegistItemIdx = INT_MAX;
	nNeedMoney = 0;
}

bool cEquipSystemContents::sElementCheck::IsRegist() const
{
	if( INT_MAX != nRegistItemIdx )
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////////
void cEquipSystemContents::sElementEquip::Reset()
{
#ifndef LJW_EQUIPSYSTEM_190724
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_ELEMENTITEM_ATTACH );
#endif

	if( eUIState::eWaitRecvData == eState )
		return;

	eState = eUIState::eNoneState;
	ResetData();
}

void cEquipSystemContents::sElementEquip::ResetData()
{
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
		{
			pInven->ItemUnlock_InvenSlot( nRegistItemIdx );
			nRegistItemIdx = INT_MAX;

			std::map< int, sSocketInfo >::iterator it = mapSocketItems.begin();
			std::map< int, sSocketInfo >::iterator itEnd = mapSocketItems.end();
			for( ; it != itEnd; ++it )
			{
				if( eSocketState::eRegist == it->second.eState )
					pInven->ItemUnlock_InvenSlot( it->second.nInvenIndex );
			}
			mapSocketItems.clear();
		}
	}

	nNeedAttachMoney = 0;
	nNeedDetachMoney = 0;
	nSelectedSocket = -1;
	nEmptyInvenIndex = -1;
	nWaitRecvAttach = 0;
}

bool cEquipSystemContents::sElementEquip::IsRegist() const
{
	if( INT_MAX != nRegistItemIdx )
		return true;
	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////
// Element + Equipment
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void cEquipSystemContents::OpenEquipSystemNpc(void* pData)
{
	SAFE_POINTER_RET( pData );
	CsC_AvObject* pTarget = static_cast< CsC_AvObject* >( pData );

	if( RTTI_NPC != pTarget->GetLeafRTTI() )
		return;

	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_EQUIPMENT_SYSTEM );

	m_pNpc = dynamic_cast< CNpc* >( pTarget );
	m_nPortableIndex = 0;
	m_eCurTab = eTabState::eTabNormal;

	_LockInventory();

	if( g_pNpcMng )
	{
		g_pNpcMng->ResetZoomNpc();
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_EQUIPMENT_SYSTEM );
	}

	bool bSound = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

	SendRequestChargeList();
}

void cEquipSystemContents::OpenEquipSystemPortable(void* pData)
{
	SAFE_POINTER_RET( pData );
	int nPortableIndex = *static_cast< int* >( pData );

	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_EQUIPMENT_SYSTEM );

	m_pNpc = NULL;
	m_nPortableIndex = nPortableIndex;
	m_eCurTab = eTabState::eTabNormal;

	_LockInventory();

	if( g_pCharMng )
	{
		CTamerUser* pUser = g_pCharMng->GetTamerUser();
		if( pUser )
			m_niOpenPos = pUser->GetPos2D();
	}


	if( g_pNpcMng )
	{
		g_pNpcMng->ResetZoomNpc();
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_EQUIPMENT_SYSTEM );
	}

	bool bSound = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

	SendRequestChargeList();
}

void cEquipSystemContents::RegisterItemEquipSystem(int const& nInvenIndex)
{
	switch( m_eCurTab )
	{
	case eTabState::eTabElem:
		RegisterItem_ElementManage( nInvenIndex );
		break;
	case eTabState::eTabNormal:
		RegisterItem_NormalCharge( nInvenIndex );
		break;
	case eTabState::eTabQuick:
		RegisterItem_QuickCharge( nInvenIndex );
		break;
	}
}

void cEquipSystemContents::RegisterItem_ElementManage(int const& nInvenIndex)
{
	// 패킷을 기다리는 중이면
	if( _CheckWaitReceivePacket() )
	{
		cPrintMsg::PrintMsg( 10004 );
		return;
	}

	eRegistState eState = _CheckRegistItemKind( nInvenIndex );
	// 아이템 등록
	switch( eState )
	{
	case eRegistState::eEquip:
		{
			// 등록된 속성 아이템이 있는 경우
			if( m_sElemCheck.IsRegist() )
			{
				if( eUIState::eProcessing == m_sElemCheck.eState )
				{
					cPrintMsg::PrintMsg( 10004 );
					return;
				}
				m_sElemCheck.ResetData();
			}

			if( SetRegistAttach_Equip( nInvenIndex ) )
				return;
		}
		return;
	case eRegistState::eElem:
		{
			if( m_sElemEquip.IsRegist() )
			{
				SetRegistAttach_Elem( nInvenIndex );
				return;
			}

			SetRegistAnalysis( nInvenIndex );
		}
		return;
	}

	cPrintMsg::PrintMsg( 11014 );
}

// 일반 충전 아이템 등록
void cEquipSystemContents::RegisterItem_NormalCharge(int const& nInvenIndex)
{
#if COMMON_LIB_FIXED
	eRegistState eState = _CheckRegistItemKind( nInvenIndex );
	if( eRegistState::eEquip != eState )
	{
		// 보관 가능한 아이템이 아닙니다.
		cPrintMsg::PrintMsg( nsTimeChargeResult::ItemCantCharge );
		return;
	}

	cItemInfo* pChargeItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RET( pChargeItem );

	SAFE_POINTER_RET( nsCsFileTable::g_pTimeChargeMng );
	if( !nsCsFileTable::g_pTimeChargeMng->IsEnableChargeItem( pChargeItem->GetType() ) )
	{
		// 보관 가능한 아이템이 아닙니다.
		cPrintMsg::PrintMsg( nsTimeChargeResult::ItemCantCharge );
		return;
	}

	if( pChargeItem->IsActive() )
	{
		// 보관 가능한 아이템이 아닙니다.
		cPrintMsg::PrintMsg( nsTimeChargeResult::ItemHasTime );
		return;
	}

	sBitCodeInfo const* pChargeInfo = 
		nsCsFileTable::g_pTimeChargeMng->GetBitCodeInfo( static_cast<DWORD>(pChargeItem->GetType()) );
	SAFE_POINTER_RET( pChargeInfo );

	int nSlot = _GetEnableKeepIndex();
	if( -1 == nSlot )
	{
		cPrintMsg::PrintMsg( nsTimeChargeResult::SlotAlreadyFull );
		return;
	}

	std::map< int, sChargeItem >::iterator it = m_mapChargeList.find( nSlot );
	if( it == m_mapChargeList.end() )
	{
		cPrintMsg::PrintMsg( nsTimeChargeResult::SlotAlreadyFull );
		return;
	}

	it->second.SetItemInfo( pChargeItem, nInvenIndex );
	it->second.SetChargeInfo( pChargeInfo->nChargeTime * 60, pChargeInfo->nKeepTime * 60, pChargeInfo->dwKeepPrice );

	// 아이템 락
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->Itemlock_InvenSlot( nInvenIndex );
	}

	bool bEnableMoney = _CheckEnableMoney( pChargeInfo->dwKeepPrice );

	ContentsStream kSend;
	kSend << nSlot << bEnableMoney;
	Notify( eRegistChargeItem, kSend );
#endif
}

// 즉시 충전 아이템 등록
void cEquipSystemContents::RegisterItem_QuickCharge(int const& nInvenIndex)
{
#if COMMON_LIB_FIXED
	eRegistState eState = _CheckRegistItemKind( nInvenIndex );
	switch( eState )
	{
	case eRegistState::eEquip:
		_RegistChargeItem( nInvenIndex );
		return;
	case eRegistState::eCharge:
		_RegistQuickItem( nInvenIndex );
		return;
	}

	// 보관 가능한 아이템이 아닙니다.
	cPrintMsg::PrintMsg( nsTimeChargeResult::ItemCantCharge );
#endif
}

// 즉시 충전 - 충전할 아이템 등록
void cEquipSystemContents::_RegistChargeItem(int const& nInvenIndex)
{
#if COMMON_LIB_FIXED
	cItemInfo* pChargeItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RET( pChargeItem );

	SAFE_POINTER_RET( nsCsFileTable::g_pTimeChargeMng );
	sChargeItemInfo const* pCInfo = nsCsFileTable::g_pTimeChargeMng->GetChargeItemInfo( pChargeItem->GetType() );
	if( !pCInfo )
	{
		cPrintMsg::PrintMsg( nsTimeChargeResult::ItemCantCharge );
		return;

	}

	if( pChargeItem->IsActive() )
	{
		cPrintMsg::PrintMsg( nsTimeChargeResult::ItemHasTime );
		return;
	}

	// 등록된 아이템이 있으면 날린다
	if( m_sQuickChargeItem.IsRegistChargeItem() )
		m_sQuickChargeItem.Reset();

	m_sQuickChargeItem.cChargeItem = *pChargeItem;
	m_sQuickChargeItem.nItemIndex = nInvenIndex;
	m_sQuickChargeItem.nCashCode = pCInfo->nCashCode;

	// 아이템 락
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->Itemlock_InvenSlot( nInvenIndex );
	}

	uint nItemType = pChargeItem->GetType();
	std::wstring wsItemName;
	CsItem::sINFO* pFTInfo = GetFTItemInfo( nItemType );
	if( pFTInfo )
		wsItemName = pFTInfo->s_szName;
	if( wsItemName.empty() )
		wsItemName = L"???";
	ContentsStream kSend;
	kSend << nItemType << wsItemName;
	Notify( eRegistChargeItem_Quick, kSend );
#endif
}

// 즉시 충전 - 즉시 충전 아이템 등록
void cEquipSystemContents::_RegistQuickItem(int const& nInvenIndex)
{
#if COMMON_LIB_FIXED
	if( !m_sQuickChargeItem.IsRegistChargeItem() )
	{
		BHPRT( "충전할 아이템이 등록되어 있지 않습니다." );
		return;
	}

	cItemInfo* pQuickItem = GetInvenItem( nInvenIndex );
	SAFE_POINTER_RET( pQuickItem );

	CsItem::sINFO* pFTQInfo = GetFTItemInfo( pQuickItem->GetType() );
	SAFE_POINTER_RET( pFTQInfo );
	if( nItem::ItemCharge != pFTQInfo->s_nType_L )
	{
		cPrintMsg::PrintMsg( nsTimeChargeResult::ItemCantCharge );
		return;
	}

	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	DWORD dwQuickItemCount = pInven->GetCount_Item_ID( pQuickItem->GetType() );
	// pos, count
	std::map< int, int > mapQuickItems;
	pInven->GetItemDatas_ItemID( pQuickItem->GetType(), dwQuickItemCount, mapQuickItems );

	// 즉시 충전 아이템 등록되어 있으면 날리고
	if( m_sQuickChargeItem.IsRegistQuickItem() )
		m_sQuickChargeItem.ResetQuickItem();

	// 즉시 충전 아이템 정보 등록~
	m_sQuickChargeItem.nQuickItemType = pQuickItem->GetType();

	std::map< int, int >::iterator it = mapQuickItems.begin();
	std::map< int, int >::iterator itEnd = mapQuickItems.end();
	for( ; it != itEnd; ++it )
	{
		nsTimeCharge::sChargeItemInfo sQuickInfo;
		sQuickInfo.nItemPos = it->first;
		sQuickInfo.nItemCnt = it->second;
		m_sQuickChargeItem.lQuickItems.push_back( sQuickInfo );

		// 아이템 락
		if( g_pDataMng )
		{
			cData_Inven* pInven = g_pDataMng->GetInven();
			if( pInven )
				pInven->Itemlock_InvenSlot( it->first );
		}
	}

	std::wstring wsItemName = pFTQInfo->s_szName;
	if( wsItemName.empty() )
		wsItemName = L"???";
	ContentsStream kSend;
	kSend << m_sQuickChargeItem.nQuickItemType << dwQuickItemCount << wsItemName;
	Notify( eRegistQuickCharge, kSend );
#endif
}

void cEquipSystemContents::_LockInventory()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	pInven->ItemUnlock_AllItem();

	// 휴대용 아이템 락
	pInven->ItemLock_ItemType( nItem::Portable_Item, eANALYSOR );
	pInven->ItemLock_ItemType( nItem::Portable_Item, eFUSION );

	int nInvenSlotCount = pInven->GetInvenSlotCount();
	for( int i = 0; i < nInvenSlotCount; ++i )
	{
		cItemInfo* pItemInfo = pInven->GetData( i );
		SAFE_POINTER_CON( pItemInfo );
		if( !pItemInfo->IsEnable() )
			continue;

		switch( m_eCurTab )
		{
		case eTabState::eTabNormal:
			{
				if( eRegistState::eEquip == _CheckRegistItemKind( i ) )
					continue;
			}
			break;
		case eTabState::eTabQuick:
			{
				if( eRegistState::eEquip == _CheckRegistItemKind( i )
					|| eRegistState::eCharge == _CheckRegistItemKind( i ) )
					continue;
			}
			break;
		case eTabState::eTabElem:
			{
				if( eRegistState::eEquip == _CheckRegistItemKind( i )
					|| eRegistState::eElem == _CheckRegistItemKind( i ) )
					continue;
			}
			break;
		}
		pInven->Itemlock_InvenSlot( TO_INVEN_SID( i ) );
	}
}

void cEquipSystemContents::_UnlockInventory()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	pInven->ItemUnlock_ItemType( nItem::Portable_Item, eANALYSOR );
	pInven->ItemUnlock_ItemType( nItem::Portable_Item, eFUSION );
	pInven->ItemUnlock_AllItem();

	// 데이터를 기다리는 중이면
	if( eUIState::eWaitRecvData == m_sElemCheck.eState )
		pInven->Itemlock_InvenSlot( m_sElemCheck.nRegistItemIdx );
	else
		m_sElemCheck.ResetData();

	if( eUIState::eWaitRecvData == m_sElemEquip.eState )
		pInven->Itemlock_InvenSlot( m_sElemEquip.nRegistItemIdx );
	else
		m_sElemEquip.ResetData();

	if( true == m_bWaitRecvPacket )
	{
		std::map< int, sChargeItem >::iterator itCL = m_mapChargeList.find( m_nSelectChargeSlot );
		if( itCL != m_mapChargeList.end() )
			pInven->Itemlock_InvenSlot( itCL->second.nInvenIndex );

		if( m_sQuickChargeItem.IsRegistChargeItem() )
			pInven->Itemlock_InvenSlot( m_sQuickChargeItem.nItemIndex );

#if COMMON_LIB_FIXED
		if( m_sQuickChargeItem.IsRegistQuickItem() )
		{
			std::list< nsTimeCharge::sChargeItemInfo >::iterator it = m_sQuickChargeItem.lQuickItems.begin();
			std::list< nsTimeCharge::sChargeItemInfo >::iterator itEnd = m_sQuickChargeItem.lQuickItems.end();
			for( ; it != itEnd; ++it )
				pInven->Itemlock_InvenSlot( it->nItemPos );
		}
#endif
	}
	else
	{
		m_sQuickChargeItem.ResetData();
		m_mapChargeList.clear();
	}
}

bool cEquipSystemContents::_CheckWaitReceivePacket()
{
	if( eUIState::eWaitRecvData == m_sElemEquip.eState
		|| eUIState::eWaitRecvData == m_sElemCheck.eState )
		return true;

	return false;
}

void cEquipSystemContents::CloseEquipSystem(void* pData)
{
	m_pNpc = NULL;
	m_nPortableIndex = 0;
	m_eCurTab = eTabState::eTabNone;
	m_niOpenPos = NiPoint2( 0, 0 );

	if( g_pGameIF )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_EQUIPMENT_SYSTEM );
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_EQUIPMENT_SYSTEM_HELP );
	}

	_UnlockInventory();

	if( g_pNpcMng )
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_NONE );
}

bool cEquipSystemContents::SetTabState(eTabState eTab)
{
	if( true == m_bWaitRecvPacket )
		return false;

	if( _CheckWaitReceivePacket() )
		return false;

	if( m_eCurTab == eTab )
		return false;

	switch( eTab )
	{
	case eTabState::eTabNormal:
		{
			m_sQuickChargeItem.Reset();
			m_sElemCheck.Reset();
			m_sElemEquip.Reset();
		}
		break;

	case eTabState::eTabQuick:
		{
			m_sElemCheck.Reset();
			m_sElemEquip.Reset();
			std::map< int, sChargeItem >::iterator it = m_mapChargeList.begin();
			std::map< int, sChargeItem >::iterator itEnd = m_mapChargeList.end();
			for( ; it != itEnd; ++it )
			{
				// 등록만 되어 있는 경우 지워
				if( it->second.IsRegist() )
					it->second.Reset();
			}
		}
		break;

	case eTabState::eTabElem:
		{
			m_sQuickChargeItem.Reset();
			std::map< int, sChargeItem >::iterator it = m_mapChargeList.begin();
			std::map< int, sChargeItem >::iterator itEnd = m_mapChargeList.end();
			for( ; it != itEnd; ++it )
			{
				// 등록만 되어 있는 경우 지워
				if( it->second.IsRegist() )
					it->second.Reset();
			}
		}
		break;
	}

	m_eCurTab = eTab;
	_LockInventory();

	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_EQUIPMENT_SYSTEM_HELP );

	return true;
}

void cEquipSystemContents::HelpWindow()
{
	SAFE_POINTER_RET( g_pGameIF );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_EQUIPMENT_SYSTEM_HELP ) )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_EQUIPMENT_SYSTEM_HELP );
	else
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_EQUIPMENT_SYSTEM_HELP );

	ContentsStream kSend;
	kSend << m_eCurTab;
	Notify( eOpenHelp, kSend );
}

int cEquipSystemContents::_GetEnableKeepIndex()
{
	std::map< int, sChargeItem >::iterator it = m_mapChargeList.begin();
	std::map< int, sChargeItem >::iterator itEnd = m_mapChargeList.end();
	for( ; it != itEnd; ++it )
	{
		if( false == it->second.IsEmptySlot() )
			continue;

		return it->first;
	}

	return -1;
}

#if COMMON_LIB_FIXED
void cEquipSystemContents::_UpdateChargeList(n2 const& nSlotCnt, std::list< nsTimeCharge::sChargingInfo >& lCharingList)
{
	m_mapChargeList.clear();

	// 열려 있는 슬롯 개수 리스트 할당
	for( int i = 0; i < nSlotCnt; ++i )
		m_mapChargeList.insert( std::make_pair( i, sChargeItem() ) );

	std::list< nsTimeCharge::sChargingInfo >::iterator it = lCharingList.begin();
	std::list< nsTimeCharge::sChargingInfo >::iterator itEnd = lCharingList.end();
	for( ; it != itEnd; ++it )
	{
		std::map< int, sChargeItem >::iterator itCList = m_mapChargeList.find( it->nSlot );
		if( itCList == m_mapChargeList.end() )
			continue;

		itCList->second.cChargeItem = it->cItem;
		itCList->second.nChargeTime = GetChargeTime( it->cItem.m_nType );
		itCList->second.nKeepTime = it->nTimeLeftTS;
		itCList->second.eState = eUIState::eProcessing;
	}
}
#endif

void cEquipSystemContents::_UpdateChargeTime()
{
	std::map< int, sChargeItem >::iterator it = m_mapChargeList.begin();
	std::map< int, sChargeItem >::iterator itEnd = m_mapChargeList.end();
	for( ; it != itEnd; ++it )
	{
		if( eUIState::eProcessing != it->second.eState )
			continue;

		bool bComplete = false;

		if( 0 < it->second.nKeepTime )
			it->second.nKeepTime--;

		if( 0 < ( it->second.nKeepTime / 60/*1분*/ ) )
		{
			if( 59 == ( it->second.nKeepTime % 60 ) )
			{
				ContentsStream kSend;
				kSend << it->first << bComplete;
				Notify( eUpdateChargeTime, kSend );
			}
			continue;
		}

		// 충전 완료시
		if( 0 == it->second.nKeepTime )
		{
			it->second.nKeepTime = 0;
			bComplete = true;

			ContentsStream kSend;
			kSend << it->first << bComplete;
			Notify( eUpdateChargeTime, kSend );
		}
	}
}

//==========================================================================================
// 충전 목록 요청
void cEquipSystemContents::SendRequestChargeList()
{
	SAFE_POINTER_RET( net::game );
	net::game->SendRequestChargeList();

	m_bWaitRecvPacket = true;
}

void cEquipSystemContents::RecvChargeListResult(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TimeCharge_ItemList_Result recv = *static_cast< GS2C_RECV_TimeCharge_ItemList_Result* >( pData );

	m_bWaitRecvPacket = false;

	if( nsTimeChargeResult::Success != recv.nResult )
	{
		cPrintMsg::PrintMsg( recv.nResult );
		CloseEquipSystem( NULL );
		return;
	}

	// 충전 목록 갱신
	_UpdateChargeList( recv.nSlotCnt, recv.lCharingList );

	// 닫혀있으면
	if( eTabState::eTabNone == m_eCurTab )
	{
		_UnlockInventory();
		return;
	}

	int nUnlockCount = recv.nSlotCnt;
	ContentsStream kSend;
	kSend << nUnlockCount;
	Notify( eUpdateChargeList, kSend );
#endif
}

//==========================================================================================
// 일반 충전 요청
bool cEquipSystemContents::SendNormalChargeTime()
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RETVAL( net::game, false );

	std::map< int, sChargeItem >::iterator it = m_mapChargeList.find( m_nSelectChargeSlot );
	if( it == m_mapChargeList.end() )
		return false;

	// 등록된 아이템 유효한지
	if( !_CheckEnableItem( it->second.nInvenIndex ) )
		return false;

	// 충전 가능한 아이템인지
	if( !nsCsFileTable::g_pTimeChargeMng->IsEnableChargeItem( it->second.GetItemType() ) )
	{
		cPrintMsg::PrintMsg( nsTimeChargeResult::ItemCantCharge );
		return false;
	}

	// 충전 비용 확인
	if( !_CheckEnableMoney( it->second.nKeepMoney ) )
	{
		cPrintMsg::PrintMsg( nsTimeChargeResult::NonEnoughMoney );
		return false;
	}

	net::game->SendNormalChargeItem( m_nSelectChargeSlot, it->second.nInvenIndex, it->second.GetItemType() );

	m_bWaitRecvPacket = true;

	return true;
#else
	return false;
#endif
}

void cEquipSystemContents::RecvNormalChargeTimeResult(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TimeCharge_Normal_Result recv = *static_cast< GS2C_RECV_TimeCharge_Normal_Result* >( pData );

	m_bWaitRecvPacket = false;

	if( nsTimeChargeResult::Success != recv.nResult )
	{
		cPrintMsg::PrintMsg( recv.nResult );
		CloseEquipSystem( NULL );
		return;
	}

	std::map< int, sChargeItem >::iterator it = m_mapChargeList.find( recv.nChargeSlot );
	if( it == m_mapChargeList.end() )
		return;

	// 인벤에서 충전 아이템 제거
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	cItemInfo* pKeepItem = pInven->GetData( recv.nDeletedItemPos );
	SAFE_POINTER_RET( pKeepItem );
	pInven->ItemUnlock_InvenSlot( recv.nDeletedItemPos );	// 지우기 전에 언락
	pKeepItem->Reset();			// 충전 보관함으로 이동했기 때문에 지운다

	// 충전 비용 차감
	sMONEY nMoney = pInven->DecreaseMoney( it->second.nKeepMoney );
	if( nMoney.GetInt() != recv.nMoneyLeft )		// 서버에서 남은 돈이랑 다르면 갱신
		pInven->SetMoney( recv.nMoneyLeft, true );

	// 충전 아이템 갱신
	it->second.cChargeItem = recv.cItem;
	it->second.nKeepTime = recv.nTimeLeft;
	it->second.eState = eUIState::eProcessing;

	// 닫혀있으면
	if( eTabState::eTabNone == m_eCurTab )
	{
		_UnlockInventory();
		return;
	}

	ContentsStream kSend;
	kSend << m_nSelectChargeSlot;
	Notify( eNormalChargeItem, kSend );
#endif
}

// 즉시 충전 요청
void cEquipSystemContents::SendQuickChargeTime()
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( net::game );

	// 등록된 아이템 유효한지
	if( !_CheckEnableItem( m_sQuickChargeItem.nItemIndex ) )
		return;

	// 충전 시간당 아이템 개수 선택 안함
	if( 0 == m_sQuickChargeItem.nTimeChargeIndex )
		return;

	std::list< nsTimeCharge::sChargeItemInfo >::iterator it = m_sQuickChargeItem.lQuickItems.begin();
	std::list< nsTimeCharge::sChargeItemInfo >::iterator itEnd = m_sQuickChargeItem.lQuickItems.end();
	for( ; it != itEnd; ++it )
	{
		if( 0 > it->nItemPos )
			continue;

		// 충전 아이템이 하나라도 유효하지 않으면 안 된다
		if( !_CheckEnableItem( it->nItemPos ) )
			return;
	}

	// 충전 가능한 아이템인지
	if( !nsCsFileTable::g_pTimeChargeMng->IsEnableChargeItem( m_sQuickChargeItem.GetItemType() ) )
	{
		cPrintMsg::PrintMsg( nsTimeChargeResult::ItemCantCharge );
		return;
	}

	// 충전 아이템이 맞는지 - 대분류값으로 확인
	CsItem::sINFO* pFTQItem = GetFTItemInfo( m_sQuickChargeItem.nQuickItemType );
	SAFE_POINTER_RET( pFTQItem );
	if( nItem::ItemCharge != pFTQItem->s_nType_L )
	{
		cPrintMsg::PrintMsg( nsTimeChargeResult::InvalidItem );
		return;
	}

	if( m_sQuickChargeItem.nQuickUseCnt > m_sQuickChargeItem.GetItemCount() )
	{
		// 충전 아이템 부족
		cPrintMsg::PrintMsg( 11040, pFTQItem->s_szName );
		return;
	}

	net::game->SendQuickChargeItem( m_sQuickChargeItem.nItemIndex, m_sQuickChargeItem.GetItemType(),
		m_sQuickChargeItem.nQuickItemType, m_sQuickChargeItem.nTimeChargeIndex, m_sQuickChargeItem.lQuickItems );

	m_bWaitRecvPacket = true;
#endif
}

void cEquipSystemContents::RecvQuickChargeTimeResult(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TimeCharge_Immediate_Result recv = *static_cast< GS2C_RECV_TimeCharge_Immediate_Result* >( pData );

	m_bWaitRecvPacket = false;

	if( nsTimeChargeResult::Success != recv.nResult )
	{
		cPrintMsg::PrintMsg( recv.nResult );
		CloseEquipSystem( NULL );
		return;
	}

	cItemInfo* pChargedItem = GetInvenItem( recv.nChargedItemPos );
	SAFE_POINTER_RET( pChargedItem );
	if( !pChargedItem->IsEnable() )
		return;

// 	SAFE_POINTER_RET( g_pDataMng );
// 	cData_Inven* pInven = g_pDataMng->GetInven();
// 	SAFE_POINTER_RET( pInven );
// 	pInven->SetMoney( recv.nMoneyLeft, true );		// 남은 돈으로 갱신

	// 충전된 아이템 정보 갱신
	*pChargedItem = recv.cItem;

	// 소모된 아이템 갱신
	std::list< nsTimeCharge::sChargeItemInfo >::iterator it = recv.lChargeItmes.begin();
	std::list< nsTimeCharge::sChargeItemInfo >::iterator itEnd = recv.lChargeItmes.end();
	for( ; it != itEnd; ++it )
	{
		if( 0 > it->nItemPos )
			continue;

		cItemInfo* pUsedItem = GetInvenItem( it->nItemPos );
		SAFE_POINTER_CON( pUsedItem );
		if( !pUsedItem->IsEnable() )
			continue;

		pUsedItem->DecreaseCount( it->nItemCnt );
	}

	m_sQuickChargeItem.Reset();

	// 닫혀있으면
	if( eTabState::eTabNone == m_eCurTab )
	{
		_UnlockInventory();
		return;
	}

	Notify( eQuickChargeItem );
#endif
}

//==========================================================================================
// 충전 슬롯 잠금 해제
void cEquipSystemContents::SendUnlockChargeList(void* pData)
{
	SAFE_POINTER_RET( net::game );

	if( m_mapChargeList.size() > m_nSelectChargeSlot )
		return;

	// 잠금 해제 비용 확인
	u8 nNeedMoney = nsCsFileTable::g_pTimeChargeMng->GetUnlockMoney( m_nSelectChargeSlot );
	if( !_CheckEnableMoney( nNeedMoney ) )
	{
#if COMMON_LIB_FIXED
		cPrintMsg::PrintMsg( nsTimeChargeResult::SlotOpenNoMoney );
#endif
		return;
	}

	net::game->SendUnlockChargeList( m_nSelectChargeSlot );

	m_bWaitRecvPacket = true;
}

void cEquipSystemContents::RecvUnlockChargeListResult(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TimeCharge_SlotOpen_Result recv = *static_cast< GS2C_RECV_TimeCharge_SlotOpen_Result* >( pData );

	m_bWaitRecvPacket = false;

	if( nsTimeChargeResult::Success != recv.nResult )
	{
		cPrintMsg::PrintMsg( recv.nResult );
		CloseEquipSystem( NULL );
		return;
	}

	int nOpenedSlot = recv.nOpenedSlotCount - 1;

	// 이미 열려 있는 슬롯임
	std::map< int, sChargeItem >::iterator it = m_mapChargeList.find( nOpenedSlot );
	if( it != m_mapChargeList.end() )
		return;

	// 슬롯 정보 추가 - 빈 슬롯
	m_mapChargeList.insert( std::make_pair( nOpenedSlot, sChargeItem() ) );

	// 남은 돈 갱신
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	pInven->SetMoney( recv.nMoneyLeft, true );

	// 닫혀있으면
	if( eTabState::eTabNone == m_eCurTab )
	{
		_UnlockInventory();
		return;
	}

	ContentsStream kSend;
	kSend << nOpenedSlot;
	Notify( eUnlockSlot, kSend );
#endif
}

//==========================================================================================
// 충전 완료 회수
void cEquipSystemContents::SendWithdrawChargeItme(void* pData)
{
	SAFE_POINTER_RET( net::game );

	std::map< int, sChargeItem >::iterator it = m_mapChargeList.find( m_nSelectChargeSlot );
	if( it == m_mapChargeList.end() )
		return;

	if( it->second.IsEmptySlot() )
		return;

	if( 0 != it->second.nKeepTime )
		return;

	net::game->SendChargeItemWithdraw( m_nSelectChargeSlot );

	m_bWaitRecvPacket = true;
}

void cEquipSystemContents::RecvWithdrawChargeItemResult(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TimeCharge_Withdraw_Result recv = *static_cast<GS2C_RECV_TimeCharge_Withdraw_Result*>(pData);

	m_bWaitRecvPacket = false;

	if( nsTimeChargeResult::Success != recv.nResult )
	{
		cPrintMsg::PrintMsg( recv.nResult );
		CloseEquipSystem( NULL );
		return;
	}

	std::map< int, sChargeItem >::iterator it = m_mapChargeList.find( recv.nChargeSlot );
	if( it == m_mapChargeList.end() )
		return;

	// 회수 아이템 갱신
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	cItemInfo* pRecvItem = pInven->GetData( recv.nItemPos );
	SAFE_POINTER_RET( pRecvItem );
	*pRecvItem = recv.cItem;

	// 리스트에서 초기화
	it->second.Reset();

	// 닫혀있으면
	if( eTabState::eTabNone == m_eCurTab )
	{
		_UnlockInventory();
		return;
	}

	int nWithdrawSlot = recv.nChargeSlot;
	ContentsStream kSend;
	kSend << nWithdrawSlot;
	Notify( eWithdrawItem, kSend );
#endif
}

//==========================================================================================
// 충전 취소
void cEquipSystemContents::SendNormalChargeCancel(void* pData)
{
	SAFE_POINTER_RET( net::game );

	std::map< int, sChargeItem >::iterator it = m_mapChargeList.find( m_nSelectChargeSlot );
	if( it == m_mapChargeList.end() )
		return;

	if( eUIState::eProcessing != it->second.eState )
		return;

	// 이미 충전 완료
	if( 0 == it->second.nKeepTime)
		return;

	net::game->SendChargeCancel( m_nSelectChargeSlot );

	m_bWaitRecvPacket = true;
}

void cEquipSystemContents::RecvNormalChargeCancel(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TimeCharge_Cancel_Result recv = *static_cast<GS2C_RECV_TimeCharge_Cancel_Result*>(pData);

	m_bWaitRecvPacket = false;

	if( nsTimeChargeResult::Success != recv.nResult )
	{
		cPrintMsg::PrintMsg( recv.nResult );
		CloseEquipSystem( NULL );
		return;
	}

	std::map< int, sChargeItem >::iterator it = m_mapChargeList.find( recv.nChargeSlot );
	if( it == m_mapChargeList.end() )
		return;

	// 취소하고 돌려 받은 아이템 추가
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );
	cItemInfo* pRecvItem = pInven->GetData( recv.nItemPos );
	SAFE_POINTER_RET( pRecvItem );
	*pRecvItem = recv.cItem;

	// 리스트에서 초기화
	it->second.Reset();

	// 닫혀있으면
	if( eTabState::eTabNone == m_eCurTab )
	{
		_UnlockInventory();
		return;
	}

	int nCancelSlot = recv.nChargeSlot;
	ContentsStream kSend;
	kSend << nCancelSlot;
	Notify( eChargeCancel, kSend );
#endif
}

//==========================================================================================

bool cEquipSystemContents::NormalCharge(int const& nSlotIndex)
{
	if( true == m_bWaitRecvPacket )
		return false;

	m_nSelectChargeSlot = nSlotIndex;

	if( !SendNormalChargeTime() )
		return false;

	return true;
}

void cEquipSystemContents::CancelCharge(int const& nSlotIndex)
{
	if( true == m_bWaitRecvPacket )
		return;

	m_nSelectChargeSlot = nSlotIndex;

	cPrintMsg::PrintMsg( cPrintMsg::EQUIPSYS_CANCEL_CHARGE );
}

void cEquipSystemContents::WithdrawItem(int const& nSlotIndex)
{
	if( true == m_bWaitRecvPacket )
		return;

	m_nSelectChargeSlot = nSlotIndex;

	cPrintMsg::PrintMsg( cPrintMsg::EQUIPSYS_WITHDRAW_ITEM );
}

void cEquipSystemContents::QuickCharge()
{
	if( true == m_bWaitRecvPacket )
		return;

	SendQuickChargeTime();
}

void cEquipSystemContents::UnlockChargeList(int nSlotIndex)
{
	if( true == m_bWaitRecvPacket )
		return;

	if( nSlotIndex > m_mapChargeList.size() )
		return;

	SAFE_POINTER_RET( nsCsFileTable::g_pTimeChargeMng );
	sMONEY nNeedMoney = nsCsFileTable::g_pTimeChargeMng->GetUnlockMoney( nSlotIndex );
	if( !_CheckEnableMoney( nNeedMoney.GetInt() ) )
	{
#if COMMON_LIB_FIXED
		cPrintMsg::PrintMsg( nsTimeChargeResult::SlotOpenNoMoney );
#endif
		return;
	}

	m_nSelectChargeSlot = nSlotIndex;

	cPrintMsg::PrintMsg( cPrintMsg::EQUIPSYS_UNLOCK_SLOT, &nNeedMoney );
}

void cEquipSystemContents::SelectQuickChargeItemCount(int nSelectIndex)
{
	if( true == m_bWaitRecvPacket )
		return;

	// 0 은 충전 선택 메시지임~
	if( 0 == nSelectIndex )
	{
		m_sQuickChargeItem.nTimeChargeIndex = 0;
		m_sQuickChargeItem.nQuickUseCnt = 0;
		return;
	}

	// 등록된 충전 아이템이 없으면
	if( !m_sQuickChargeItem.IsRegistChargeItem() )
		return;

	// 등록된 즉시 충전 아이템이 없으면
	if( !m_sQuickChargeItem.IsRegistQuickItem() )
		return;

	m_sQuickChargeItem.nTimeChargeIndex = nSelectIndex;
	m_sQuickChargeItem.nQuickUseCnt = GetQuickChargeCountByIndex( nSelectIndex );
}

bool cEquipSystemContents::ResetChargeItem()
{
	if( true == m_bWaitRecvPacket )
		return false;

	m_sQuickChargeItem.Reset();
	return true;
}

bool cEquipSystemContents::ResetQuickItem()
{
	if( true == m_bWaitRecvPacket )
		return false;

	m_sQuickChargeItem.ResetQuickItem();
	return true;
}

bool cEquipSystemContents::ResetChargeListItem(int const& nSlotIndex)
{
	std::map< int, sChargeItem >::iterator it = m_mapChargeList.find( nSlotIndex );
	if( it == m_mapChargeList.end() )
		return false;

	if( eUIState::eProcessing == it->second.eState )
		return false;

	it->second.Reset();
	return true;
}

cEquipSystemContents::sChargeItem const* cEquipSystemContents::GetChargeItem(int const& nSlotIndex) const
{
	std::map< int, sChargeItem >::const_iterator cit = m_mapChargeList.find( nSlotIndex );
	if( cit == m_mapChargeList.end() )
		return NULL;

	return &(cit->second);
}

uint cEquipSystemContents::GetChargeTime(uint nItemType) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTimeChargeMng, 0 );
	sBitCodeInfo const* pChargeInfo = 
		nsCsFileTable::g_pTimeChargeMng->GetBitCodeInfo( static_cast<DWORD>( nItemType ) );
	SAFE_POINTER_RETVAL( pChargeInfo, 0 );

	return pChargeInfo->nChargeTime * 60;
}

uint cEquipSystemContents::GetUnlockMoney(int const& nSlotIndex) const
{
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTimeChargeMng, 0 );
	return nsCsFileTable::g_pTimeChargeMng->GetUnlockMoney( nSlotIndex );
}

int cEquipSystemContents::GetChargeTimeCount() const
{
	// 등록된 충전 아이템이 없으면
	if( !m_sQuickChargeItem.IsRegistChargeItem() )
		return 0;

	// 등록된 즉시 충전 아이템이 없으면
	if( !m_sQuickChargeItem.IsRegistQuickItem() )
		return 0;

	SAFE_POINTER_RETVAL( nsCsFileTable::g_pTimeChargeMng, 0 );
	return nsCsFileTable::g_pTimeChargeMng->GetCashCodeCount( m_sQuickChargeItem.GetItemType() );
}

int cEquipSystemContents::GetQuickChargeCountByIndex(int const& nIndex) const
{
	// 등록된 아이템이 없으면
	if( !m_sQuickChargeItem.IsRegistChargeItem() )
		return 0;

	return nsCsFileTable::g_pTimeChargeMng->GetCashNeedCount( m_sQuickChargeItem.nCashCode, nIndex );
}

//////////////////////////////////////////////////////////////////////////

bool cEquipSystemContents::sChargeItem::IsEmptySlot() const
{
	if( INT_MAX != nInvenIndex )
		return false;

	if( cChargeItem.IsEnable() )
		return false;

	return true;
}

bool cEquipSystemContents::sChargeItem::IsComplete() const
{
	if( !cChargeItem.IsEnable() )
		return false;

	if( eUIState::eProcessing != eState )
		return false;

	if( 0 != nKeepTime )
		return false;

	return true;
}

bool cEquipSystemContents::sChargeItem::IsRegist() const
{
	if( !cChargeItem.IsEnable() )
		return false;

	if( eUIState::eNoneState != eState )
		return false;

	if( 0 == nKeepTime )
		return false;

	return true;
}

uint cEquipSystemContents::sChargeItem::GetItemType() const
{
	if( !cChargeItem.IsEnable() )
		return 0;

	return cChargeItem.m_nType;
}

void cEquipSystemContents::sChargeItem::SetItemInfo(cItemInfo* pItem, int nIndex)
{
	SAFE_POINTER_RET( pItem );
	cChargeItem = *pItem;
	nInvenIndex = nIndex;
}

void cEquipSystemContents::sChargeItem::SetChargeInfo(uint nCTime, uint KTime, u8 nKMoney)
{
	nChargeTime = nCTime;
	nKeepTime = KTime;
	nKeepMoney = nKMoney;
}

void cEquipSystemContents::sChargeItem::Reset()
{
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
			pInven->ItemUnlock_InvenSlot( nInvenIndex );
	}

	cChargeItem.Clear();
	nKeepMoney = 0;
	nKeepTime = 0;
	nChargeTime = 0;
	nInvenIndex = INT_MAX;
	eState = eUIState::eNoneState;
}

//////////////////////////////////////////////////////////////////////////
uint cEquipSystemContents::sQuickChargeItem::GetItemType() const
{
	if( !cChargeItem.IsEnable() )
		return 0;

	return cChargeItem.m_nType;
}

uint cEquipSystemContents::sQuickChargeItem::GetItemCount() const
{
#if COMMON_LIB_FIXED
	uint nCount = 0;
	std::list< nsTimeCharge::sChargeItemInfo >::const_iterator cit = lQuickItems.begin();
	std::list< nsTimeCharge::sChargeItemInfo >::const_iterator citEnd = lQuickItems.end();
	for( ; cit != citEnd; ++cit )
		nCount += cit->nItemCnt;
	return nCount;
#else
	return 0;
#endif
}

bool cEquipSystemContents::sQuickChargeItem::IsRegistChargeItem() const
{
	if( !cChargeItem.IsEnable() )
		return false;
	if( INT_MAX == nItemIndex )
		return false;
	return true;
}

bool cEquipSystemContents::sQuickChargeItem::IsRegistQuickItem() const
{
	if( 0 == nQuickItemType )
		return false;

	return true;
}

void cEquipSystemContents::sQuickChargeItem::Reset()
{
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
		{
			pInven->ItemUnlock_InvenSlot( nItemIndex );

#if COMMON_LIB_FIXED
			std::list< nsTimeCharge::sChargeItemInfo >::iterator it = lQuickItems.begin();
			std::list< nsTimeCharge::sChargeItemInfo >::iterator itEnd = lQuickItems.end();
			for( ; it != itEnd; ++it )
				pInven->ItemUnlock_InvenSlot( it->nItemPos );
#endif
		}
	}
	cChargeItem.Reset();	// 충전할 아이템
	nItemIndex = INT_MAX;	// 충전할 아이템 인벤 위치

#if COMMON_LIB_FIXED
	lQuickItems.clear();
#endif

	nQuickItemType = 0;		// 충전 아이템 타입
	nQuickUseCnt = 0;		// 충전 아이템 사용 개수
	nCashCode = 0;			// 충전 일별 아이템 필요 개수 찾는 코드 번호
	nTimeChargeIndex = 0;	// 충전 일수 인덱스 1~
}

void cEquipSystemContents::sQuickChargeItem::ResetQuickItem()
{
#if COMMON_LIB_FIXED
	if( g_pDataMng )
	{
		cData_Inven* pInven = g_pDataMng->GetInven();
		if( pInven )
		{
			std::list< nsTimeCharge::sChargeItemInfo >::iterator it = lQuickItems.begin();
			std::list< nsTimeCharge::sChargeItemInfo >::iterator itEnd = lQuickItems.end();
			for( ; it != itEnd; ++it )
				pInven->ItemUnlock_InvenSlot( it->nItemPos );
		}
	}
	lQuickItems.clear();
#endif

	nQuickItemType = 0;
	nQuickUseCnt = 0;
	nTimeChargeIndex = 0;
}

void cEquipSystemContents::sQuickChargeItem::ResetData()
{
	cChargeItem.Reset();	// 충전할 아이템
	nItemIndex = INT_MAX;	// 충전할 아이템 인벤 위치

#if COMMON_LIB_FIXED
	lQuickItems.clear();
#endif

	nQuickItemType = 0;		// 충전 아이템 타입
	nQuickUseCnt = 0;		// 충전 아이템 사용 개수
	nCashCode = 0;			// 충전 일별 아이템 필요 개수 찾는 코드 번호
	nTimeChargeIndex = 0;	// 충전 일수 인덱스 1~
}
