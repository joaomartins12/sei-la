#include "StdAfx.h"
#include "GotchaMachineContents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

#define TARGET_MAX_DIST 300.0f

cGotchaMachineContents::cGotchaMachineContents() :
m_pNpc(NULL),
m_nResetCount( 0 ),
m_nNormalItemCount( 0 ),
m_nUseItemType( 0 ),
m_nUseItemSlot( -1 ),
m_nRemainItemCount( 0 ),
m_nUseMoney( 0 ),
m_eState( eMachineState::NONE ),
m_eType( eMachineType::GOTCHA ),
m_eShowUI( eShowFlag::allhide )
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_OPEN_GOTCHAMACHINE, this, &cGotchaMachineContents::SendOpenGotchaMachine );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_OPEN_GOTCHAMACHINE, this, &cGotchaMachineContents::RecvOpenGotchaMachine );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_GET_ITEM_MACHINE, this, &cGotchaMachineContents::SendGetItemMachine );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GET_ITEM_GOTCHAMACHINE, this, &cGotchaMachineContents::RecvGetItemGotchaMachine );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_GET_ITEM_MYSTERYMACHINE, this, &cGotchaMachineContents::RecvGetItemMysteryMachine );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_ERROR_GOTCHAMACHINE, this,	&cGotchaMachineContents::RecvErrorGotchaMachine );

	GAME_EVENT_ST.AddEvent( EVENT_CODE::GET_ITEM_GOTCHAMACHINE, this, &cGotchaMachineContents::GetGotchaItem );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CLOSE_MACHINE, this, &cGotchaMachineContents::CloseMachine );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::OPEN_MYSTERYMACHINE, this, &cGotchaMachineContents::OpenMysteryMachine );
}

cGotchaMachineContents::~cGotchaMachineContents()
{
	GAME_EVENT_STPTR->DeleteEventAll( this );
}

int const cGotchaMachineContents::IsContentsIdentity(void)
{
	return E_CT_GOTCHAMACHINE_CONTENTS;
}

int const cGotchaMachineContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool cGotchaMachineContents::Initialize(void)
{
	return true;
}

void cGotchaMachineContents::UnInitialize(void)
{

}

bool cGotchaMachineContents::IntraConnection(ContentsSystem& System)
{
	return true;
}

void cGotchaMachineContents::Update(float const& fElapsedTime)
{
	SAFE_POINTER_RET( m_pNpc );

	if( eMachineState::NONE != m_eState )
		return;

	if( !m_pNpc->CheckTamerDist( TARGET_MAX_DIST ) )
		CloseMachine( NULL );
}

void cGotchaMachineContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void cGotchaMachineContents::MakeMainActorData(void)
{

}

void cGotchaMachineContents::ClearMainActorData(void)
{
	CloseMachine( NULL );
}

void cGotchaMachineContents::MakeWorldData(void)
{

}

void cGotchaMachineContents::ClearWorldData(void)
{

}

void cGotchaMachineContents::OpenMysteryMachine(void* pData)
{
	SAFE_POINTER_RET( pData );
	std::pair< CsC_AvObject*, int >* pMachine = static_cast< std::pair< CsC_AvObject*, int >* >( pData );

	SAFE_POINTER_RET( pMachine->first );
	if( RTTI_NPC != pMachine->first->GetLeafRTTI() )
		return;

	m_pNpc = dynamic_cast< CNpc* >( pMachine->first );
	m_eState = eMachineState::NONE;
	m_eShowUI = eShowFlag::mystery;

	// 레어 아이템 목록 정보 세팅
	cItemInfo itemInfo;
	switch( pMachine->second )
	{
	case 3:		// 디지코어 3단 -> 4단 강용알로..
		m_eType = MYSTERY_EGG3;
		itemInfo.Set( 9117, 1 );
		break;
	case 4:		// 디지코어 4단 -> 5단 강용알로..
		m_eType = MYSTERY_EGG4;
		itemInfo.Set( 9118, 1 );
		break;
	case 5:		// 디지코어 5단 -> 5단 강용알로..
		m_eType = MYSTERY_EGG5;
		itemInfo.Set( 9118, 1 );
		break;
	default:	// 일반 수수께끼 머신
		m_eType = MYSTERY_NORMAL;
		itemInfo.Set( 9100, 1 );
		break;
	}
	m_mapRareItems.insert( std::make_pair( 0, itemInfo ) );

	// 뽑기에 필요한 아이템 세팅
	_UpdateUseItemMysteryMachine();

	_OpenMachineUI();

	// 오픈 중 메시지 삭제
	cMessageBox::DelMsg( 10019, false );
}

void cGotchaMachineContents::SendOpenGotchaMachine(void* pData)
{
	SAFE_POINTER_RET( net::game );

	SAFE_POINTER_RET( pData );
	CsC_AvObject* pMachine = static_cast< CsC_AvObject* >( pData );

	if( RTTI_NPC != pMachine->GetLeafRTTI() )
		return;

	m_pNpc = dynamic_cast< CNpc* >( pMachine );
	m_eType = eMachineType::GOTCHA;
	m_eState = eMachineState::NONE;

	if( !_CheckEnableGotchaMachine() )
	{
		m_pNpc = NULL;
		return;
	}

	net::game->SendGotchaOpen( m_pNpc->GetFTID() );

	//오픈 중 메세지 띄워
	cPrintMsg::PrintMsg( 10019 );
}

void cGotchaMachineContents::RecvOpenGotchaMachine(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_OPEN_GOTCHAMACHINE* pRecv = static_cast< GS2C_RECV_OPEN_GOTCHAMACHINE* >( pData );

	SAFE_POINTER_RET( m_pNpc );

	// 서버로부터 받아온 뽑기 머신 정보
	_UpdateGotchaMachine( pRecv->lRareItems, pRecv->nResetCount, pRecv->nNormalItemCount );

	// 뽑기에 필요한 아이템 정보 세팅
	_UpdateUseItemGotchaMachine( m_pNpc->GetFTID() );

	_OpenMachineUI();

	// 오픈 중 메시지 삭제
	cMessageBox::DelMsg( 10019, false );
#endif
}

void cGotchaMachineContents::SendGetItemMachine(void* pData)
{
	if( eMachineType::GOTCHA == m_eType )
	{
		if( !_SendGetItemGotcha() )
			return;
	}
	else
	{
		if( !_SendGetItemMystery() )
			return;
	}

	m_eState = eMachineState::WAIT_RECV;

	// UI 비활성화
	_SetEnableWindowUI( false );

	//뽑기 중 메세지 띄워 
	cPrintMsg::PrintMsg( 10019 );
}

void cGotchaMachineContents::RecvGetItemGotchaMachine(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_GET_ITEM_GOTCHAMACHINE* pRecv = static_cast<GS2C_RECV_GET_ITEM_GOTCHAMACHINE*>(pData);

	m_eState = eMachineState::NONE;

	// 뽑기 중 메세지 끄기
	cMessageBox::DelMsg( 10019, false );

	if( 0 == pRecv->nItemID || 0 == pRecv->nItemCount )
	{
		_SetEnableWindowUI( true );
		return;
	}

	// 뽑기로 얻은 아이템 저장
	m_GotchaItem.Set( pRecv->nItemID, pRecv->nItemCount );
	m_GotchaItem.m_nRemainTradeLimitTime = pRecv->nRemainTradeLimitTime;

	// 레어머신 갱신 - UI 갱신은 뽑기 애니 끝나고 한다.
	_UpdateGotchaMachine( pRecv->lRareItems, pRecv->nResetCount, pRecv->nNormalItemCount );

	if( !_SetMachineAni( !pRecv->nKind ) )
	{
		GetGotchaItem( NULL );
		CloseMachine( NULL );
	}
#endif
}

void cGotchaMachineContents::RecvGetItemMysteryMachine(void* pData)
{
#if COMMON_LIB_FIXED
	SAFE_POINTER_RET( pData );
	GS2C_RECV_GET_ITEM_MYSTERYMACHINE* pRecv = static_cast< GS2C_RECV_GET_ITEM_MYSTERYMACHINE* >( pData );

	m_eState = eMachineState::NONE;

	// 뽑기 중 메세지 끄기
	cMessageBox::DelMsg( 10019, false );

	if( 0 == pRecv->nItemID || 0 == pRecv->nItemCount )
	{
		_SetEnableWindowUI( true );
		return;
	}

	// 뽑기로 얻은 아이템 저장
	m_GotchaItem.Set( pRecv->nItemID, pRecv->nItemCount );
	m_GotchaItem.m_nRemainTradeLimitTime = pRecv->nRemainTradeLimitTime;

	if( !_SetMachineAni( nsCsFileTable::g_pGotchaMng->GetGotcha_Mystery_IsEffect( pRecv->nItemID ) ) )
	{
		GetGotchaItem( NULL );
		CloseMachine( NULL );
	}
#endif
}

void cGotchaMachineContents::RecvErrorGotchaMachine(void* pData)
{
	SAFE_POINTER_RET( pData );
	uint nRecvError = *static_cast< uint* >( pData );

	m_eState = eMachineState::NONE;

	cMessageBox::DelMsg( 10019, false );

	SAFE_POINTER_RET( m_pNpc );
	DWORD nNpcID = m_pNpc->GetFTID();
	switch( nRecvError )
	{
	case 0://리셋 카운트 불일치
		cPrintMsg::PrintMsg( 30566, m_pNpc->GetName() );
		break;
	case 1://레어머신 남은 아이템 없을 때 Open / Execute
		cPrintMsg::PrintMsg( 30565, m_pNpc->GetName() );
		break;
	case 2://인벤 빈칸 없을 때 
		cPrintMsg::PrintMsg( 30560, m_pNpc->GetName() );
		break;
	case 3://시간,기간 안맞을때
		cPrintMsg::PrintMsg( 30563, m_pNpc->GetName() );
		break;
	case 4://재사용 시간이 아직 지나지 않았을 때
		cPrintMsg::PrintMsg( 30567, m_pNpc->GetName() );
		break;
	}

	CloseMachine( NULL );
}

void cGotchaMachineContents::CloseMachine(void* pData)
{
	_SetEnableWindowUI( true );

	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_NEW_GOTCHA_MACHINE );

	switch( m_eState )
	{
	case eMachineState::WAIT_RECV:	// 아직 패킷 못 받았을 때
		m_pNpc = NULL;
		return;
	case eMachineState::RUN:		// 뽑기 애니 중일 때
	case eMachineState::RUN_RARE:
		GetGotchaItem( NULL );
		break;
	}

	_ResetMachineData();
}

void cGotchaMachineContents::GetGotchaItem(void* pData)
{
	// 뽑기 애니 종료
	m_eState = eMachineState::NONE;

	// 인벤토리 갱신 - 사용 아이템 감소, 뽑은 아이템 저장
	_UpdateInventory();

	// UI 활성화
	_SetEnableWindowUI( true );

	// UI 갱신
	Notify( eRefresh );

	// 시스템 창이랑 가운데에 획득 메세지 뿌리기
	CsItem* pFTItem = nsCsFileTable::g_pItemMng->GetItem( m_GotchaItem.m_nType );
	SAFE_POINTER_RET( pFTItem );
	CsItem::sINFO* pFTInfo = pFTItem->GetInfo();
	SAFE_POINTER_RET( pFTInfo );
	int nItemCount = m_GotchaItem.GetCount();
	if( 1 == nItemCount )
		cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_CROP, pFTInfo->s_szName, &g_pDataMng->GetItemColor( pFTInfo ) );
	else if( 1 < nItemCount )
		cPrintMsg::PrintMsg( cPrintMsg::GAME_ITEM_MANYCROP, pFTInfo->s_szName, &nItemCount, &g_pDataMng->GetItemColor( pFTInfo ) );
	cPrintMsg::PrintMsg( 30517, pFTInfo->s_szName );
	m_GotchaItem.Clear();	// 초기화
}

void cGotchaMachineContents::_UpdateGotchaMachine(std::list< cItemData >& lRareItems, int nResetCount, int nNormalItemCount)
{
	m_nRemainItemCount = 0;
	m_mapRareItems.clear();
	std::list< cItemData >::iterator it = lRareItems.begin();
	std::list< cItemData >::iterator itEnd = lRareItems.end();
	cItemInfo itemInfo;
	for( int i = 0; it != itEnd; ++it, ++i )
	{
		itemInfo = *it;
		m_mapRareItems.insert( std::make_pair( i, itemInfo ) );
		m_nRemainItemCount += it->GetCount();
	}
	m_nResetCount = nResetCount;
	m_nNormalItemCount = nNormalItemCount;
	m_nRemainItemCount += nNormalItemCount;
}

void cGotchaMachineContents::_UpdateUseItemGotchaMachine(uint nNpcID)
{
	m_mapUseItems.clear();
	SAFE_POINTER_RET( nsCsFileTable::g_pGotchaMng );
	DWORD dwFTIDX = nsCsFileTable::g_pGotchaMng->GetGotchaMachineFIDX( nNpcID );
	CsGotcha_Machine* pMachine = nsCsFileTable::g_pGotchaMng->GetGotcha_Machine( nNpcID, dwFTIDX );
	SAFE_POINTER_RET( pMachine );
	int nIndex = 0;
	while( NULL != pMachine )
	{
		CsGotcha_Machine::sINFO* pMachineInfo = pMachine->GetInfo();
		if( !pMachineInfo )
		{
			pMachine = nsCsFileTable::g_pGotchaMng->GetGotcha_Machine( nNpcID, ++dwFTIDX );
			continue;
		}
		if( 0 == pMachineInfo->s_dwUseItem_Code )
		{
			pMachine = nsCsFileTable::g_pGotchaMng->GetGotcha_Machine( nNpcID, ++dwFTIDX );
			continue;
		}

		m_eShowUI = static_cast< eShowFlag >( pMachineInfo->s_nShowCountUI );

		bool bIsExist = false;
		std::map< int, sUseItemInfo >::iterator it = m_mapUseItems.begin();
		std::map< int, sUseItemInfo >::iterator itEnd = m_mapUseItems.end();
		for( ; it != itEnd; ++it )
		{
			if( pMachineInfo->s_dwUseItem_Code == it->second.nUseItemType )
			{
				bIsExist = true;
				break;
			}
		}

		if( !bIsExist )
		{
			sUseItemInfo useItem;
			useItem.Set( pMachineInfo->s_dwUseItem_Code, pMachineInfo->s_nUseItem_Cnt );
			m_mapUseItems.insert( std::make_pair( nIndex++, useItem ) );
		}
		else
			pMachine = nsCsFileTable::g_pGotchaMng->GetGotcha_Machine( m_pNpc->GetFTID(), ++dwFTIDX );
	}
}

void cGotchaMachineContents::_UpdateUseItemMysteryMachine()
{
	m_mapUseItems.clear();
	SAFE_POINTER_RET( nsCsFileTable::g_pGotchaMng );
	int nOrder = 1;
	CsGotcha_Mystery_Coin* pMysteryCoin = nsCsFileTable::g_pGotchaMng->GetGotcha_Mystery_Coin( nOrder );
	SAFE_POINTER_RET( pMysteryCoin );
	int nIndex = 0;
	while( NULL != pMysteryCoin )
	{
		CsGotcha_Mystery_Coin::sINFO* pInfo = pMysteryCoin->GetInfo();
		if( !pInfo )
		{
			pMysteryCoin = nsCsFileTable::g_pGotchaMng->GetGotcha_Mystery_Coin( ++nOrder );
			continue;
		}
		if( 0 == pInfo->s_nCoinIdx )
		{
			pMysteryCoin = nsCsFileTable::g_pGotchaMng->GetGotcha_Mystery_Coin( ++nOrder );
			continue;
		}
#ifdef EGG_PREFERMENT
		if( m_eType != pInfo->s_nStartGroupNo )
		{
			pMysteryCoin = nsCsFileTable::g_pGotchaMng->GetGotcha_Mystery_Coin( ++nOrder );
			continue;
		}
#endif

		sUseItemInfo useItem;
#ifdef EGG_PREFERMENT
		useItem.Set( pInfo->s_nCoinIdx, pInfo->s_nCoinCnt, pInfo->s_nMoney );
#else
		useItem.Set( pInfo->s_nCoinIdx, pInfo->s_nCoinCnt );
#endif
		m_mapUseItems.insert( std::make_pair( nIndex++, useItem ) );
		pMysteryCoin = nsCsFileTable::g_pGotchaMng->GetGotcha_Mystery_Coin( ++nOrder );
	}
}

void cGotchaMachineContents::_UpdateInventory()
{
	SAFE_POINTER_RET( g_pDataMng );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET( pInven );

	// 뽑은 아이템 인벤에 넣기
	pInven->ItemCrop( m_GotchaItem.m_nType, m_GotchaItem.GetCount(), m_GotchaItem.m_nRemainTradeLimitTime );

	// 인벤에 있는 필요 아이템 감소
	if( eMachineType::GOTCHA == m_eType )
	{
		cItemInfo* pUseItem = pInven->GetData( m_nUseItemSlot );
		SAFE_POINTER_RET( pUseItem );
		if( !pUseItem->IsEnable() )
			return;

		std::map< int, sUseItemInfo >::iterator it = m_mapUseItems.begin();
		std::map< int, sUseItemInfo >::iterator itEnd = m_mapUseItems.end();
		for( ; it != itEnd; ++it )
		{
			if( pUseItem->GetType() == it->second.nUseItemType )
			{
				pUseItem->DecreaseCount( it->second.nUseItemCount );
				break;
			}
		}
	}
	else
	{
		std::map< int, int >::iterator it = m_mapUseInvenSlot.begin();
		std::map< int, int >::iterator itEnd = m_mapUseInvenSlot.end();
		for( ; it != itEnd; ++it )
		{
			cItemInfo* pUseItem = pInven->GetData( it->first );
			SAFE_POINTER_CON( pUseItem );
			if( !pUseItem->IsEnable() )
				continue;
			if( m_nUseItemType != pUseItem->GetType() )
				continue;

			pUseItem->DecreaseCount( it->second );
		}

		pInven->DecreaseMoney( m_nUseMoney );
	}
}

bool cGotchaMachineContents::_CheckEnableGotchaMachine()
{
	SAFE_POINTER_RETVAL( m_pNpc, false );
	SAFE_POINTER_RETVAL( nsCsFileTable::g_pGotchaMng, false );
	DWORD dwGotchaFIDX = nsCsFileTable::g_pGotchaMng->GetGotchaMachineFIDX( m_pNpc->GetFTID() );
	if( -1 == dwGotchaFIDX )
		return false;

	// 사용 제한 확인
	if( !nsCsFileTable::g_pGotchaMng->IsEnableGotchaMachine( dwGotchaFIDX ) )
	{
		cPrintMsg::PrintMsg( 30563, m_pNpc->GetName() );
		return false;
	}
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, false );
	CTamerUser::sBASE_STAT* pStat = pUser->GetBaseStat();
	SAFE_POINTER_RETVAL( pStat, false );
	// 레벨 제한 확인
	if( !nsCsFileTable::g_pGotchaMng->IsEnableLevel( dwGotchaFIDX, pStat->GetLevel() ) )
	{
		cPrintMsg::PrintMsg( 30564, m_pNpc->GetName() );
		return false;
	}

	return true;
}

bool cGotchaMachineContents::_CheckEnableInvenItem()
{
	SAFE_POINTER_RETVAL( m_pNpc, false );
	SAFE_POINTER_RETVAL( g_pDataMng, false );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );

	// 인벤 부족
	if( pInven->GetEmptySlotCount() < 1 )
	{
		cPrintMsg::PrintMsg( 30560, m_pNpc->GetName() );
		return false;
	}

	// 필요 아이템 부족 or 없음
	m_mapUseInvenSlot.clear();
	m_nUseItemType = 0;
	m_nUseItemSlot = -1;
	m_nUseMoney = 0;
	bool bEnableMoney = true;
	std::map< int, sUseItemInfo >::iterator it = m_mapUseItems.begin();
	std::map< int, sUseItemInfo >::iterator itEnd = m_mapUseItems.end();
	for( ; it != itEnd; ++it )
	{
		int nCount = pInven->GetCount_Item_ID( it->second.nUseItemType );
		if( nCount < it->second.nUseItemCount )
			continue;

		if( eMachineType::GOTCHA == m_eType )
		{
			// 서버에 슬롯 위치 하나만 보내기 때문에
			// 필요 개수 충족하는 아이템 슬롯 하나를 찾아야 함
			int nSlot = pInven->GetSlot_Item_ID_Count( it->second.nUseItemType, it->second.nUseItemCount );
			if( nSlot >= pInven->GetInvenSlotCount() )
				continue;
			m_nUseItemType = it->second.nUseItemType;
			m_nUseItemSlot = nSlot;
			return true;
		}
		else
		{
			if( pInven->GetMoney() < it->second.nUsePrice )
			{
				bEnableMoney = false;
				continue;
			}
			pInven->GetItemDatas_ItemID( it->second.nUseItemType, it->second.nUseItemCount, m_mapUseInvenSlot, true );
			m_nUseItemType = it->second.nUseItemType;
			m_nUseMoney = it->second.nUsePrice;
			return true;
		}
	}

	// 금액이 충분치 않다면
	if( !bEnableMoney )
	{
		cPrintMsg::PrintMsg( 11003 );
		return false;
	}

	cPrintMsg::PrintMsg( 30560, m_pNpc->GetName() );
	return false;
}

void cGotchaMachineContents::_OpenMachineUI()
{
	SAFE_POINTER_RET( g_pGameIF );
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_NEW_GOTCHA_MACHINE );

	// 머신 오픈 요청하고 ui가 생성되기 전에 IsOpenNpcWindow()를 체크해서
	// EndZoomNpc() 호출해버리기 때문에 다시 SetZoomNpc() 해줘야 함
	if( g_pNpcMng )
	{
		// m_pZoomNpc가 현재 세팅하려는 npc와 동일하기 때문에 NULL로 바꿔줘야 다시 세팅 가능
		g_pNpcMng->ResetZoomNpc();
		g_pNpcMng->SetZoomNpc( m_pNpc );
		g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_NEW_GOTCHA_MACHINE );
	}

	bool bSound = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );
}

void cGotchaMachineContents::_ResetMachineData()
{
	m_pNpc = NULL;
	m_GotchaItem.Clear();
	m_mapRareItems.clear();
	m_mapUseItems.clear();
	m_mapUseInvenSlot.clear();
	m_nResetCount = 0;
	m_nNormalItemCount = 0;
	m_nUseItemType = 0;
	m_nUseItemSlot = -1;
	m_nRemainItemCount = 0;
	m_nUseMoney = 0;
	m_eState = eMachineState::NONE;
	m_eType = eMachineType::GOTCHA;
	m_eShowUI = eShowFlag::allhide;

	if( g_pServerMoveOwner )
		g_pServerMoveOwner->SetMoveLock( 0 );
}

void cGotchaMachineContents::_SetEnableWindowUI(bool bEnable)
{
	if( g_pServerMoveOwner )
	{
		// 뽑기 중 이동 입력 제한
		if( !bEnable )
			g_pServerMoveOwner->SetMoveLock( CUserServerMove::eKeyboardLock | CUserServerMove::eMouseLock );
		else
			g_pServerMoveOwner->SetMoveLock( 0 );
	}

	if( g_pDataMng )
	{
		// 뽑기 중 아이템 락
		if( !bEnable )
			g_pDataMng->ItemAllLock();
		else
			g_pDataMng->ReleaseItemLock();
	}

	// 인벤
	GAME_EVENT_ST.OnEvent( EVENT_CODE::SET_ENABLE_INVENTORY, &bEnable );
	// 가챠 머신
	ContentsStream kSend;
	kSend << bEnable;
	Notify( eSetEnable, kSend );
}

bool cGotchaMachineContents::_SetMachineAni(bool bIsRare)
{
	SAFE_POINTER_RETVAL( m_pNpc, false );
	if( bIsRare )
	{
		m_pNpc->SetAnimation( ANI::NPC_TACTICS_SUCCESS );
		m_eState = RUN_RARE;
	}
	else
	{
		m_pNpc->SetAnimation( ANI::NPC_YES );
		m_eState = RUN;
	}

	return true;
}

bool cGotchaMachineContents::_SendGetItemGotcha()
{
	SAFE_POINTER_RETVAL( net::game, false );

	if( !_CheckEnableGotchaMachine() )
	{
		CloseMachine( NULL );
		return false;
	}

	if( !_CheckEnableInvenItem() )
		return false;

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, false );
	SAFE_POINTER_RETVAL( m_pNpc, false );
	net::game->SendGotchaGet( pUser->GetUniqID(), m_pNpc->GetFTID(), m_nUseItemSlot, m_nResetCount );
	return true;
}

bool cGotchaMachineContents::_SendGetItemMystery()
{
	SAFE_POINTER_RETVAL( net::game, false );

	if( !_CheckEnableInvenItem() )
		return false;

	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, false );
	SAFE_POINTER_RETVAL( m_pNpc, false );
#ifdef EGG_PREFERMENT
	net::game->SendMysteryGet( pUser->GetUniqID(), m_pNpc->GetFTID(), m_mapUseInvenSlot, m_nUseItemType );
#else
	net::game->SendMysteryGet( pUser->GetUniqID(), m_pNpc->GetFTID(), m_mapUseInvenSlot );
#endif
	return true;
}

void cGotchaMachineContents::StartGotcha()
{
	SAFE_POINTER_RET( m_pNpc );

	if( eMachineType::GOTCHA == m_eType )
	{
		if( !_CheckEnableGotchaMachine() )
			return;
	}

	if( !_CheckEnableInvenItem() )
		return;

	TCHAR szItemName[ ITEM_NAME_LEN ] = {0, };
	memcpy( szItemName, nsCsFileTable::g_pItemMng->GetItemName( m_nUseItemType ).c_str(), ITEM_NAME_LEN );
	cPrintMsg::PrintMsg( 30561, szItemName, m_pNpc->GetName() );
}

void cGotchaMachineContents::CloseMachineUI()
{
	if( eMachineState::NONE != m_eState )
		return;

	SAFE_POINTER_RET( m_pNpc );
	cPrintMsg::PrintMsg( 30562, m_pNpc->GetName() );
}

cGotchaMachineContents::eShowFlag cGotchaMachineContents::GetShowFlag() const
{
	return m_eShowUI;
}

cGotchaMachineContents::eMachineType cGotchaMachineContents::GetMachineType() const
{
	return m_eType;
}

int cGotchaMachineContents::GetRemainItemCount() const
{
	return m_nRemainItemCount;
}

int cGotchaMachineContents::GetRareItemCountAll() const
{
	return m_mapRareItems.size();
}

cItemInfo* cGotchaMachineContents::GetRareItem(int nIndex)
{
	std::map< int, cItemInfo >::iterator it = m_mapRareItems.find( nIndex );
	if( it != m_mapRareItems.end() )
		return &(it->second);

	return NULL;
}

std::wstring cGotchaMachineContents::GetMachineName() const
{
	SAFE_POINTER_RETVAL( m_pNpc, L"" );
	return m_pNpc->GetName();
}

std::wstring cGotchaMachineContents::GetMysteryItemName() const
{
	std::wstring wsItemName = UISTRING_TEXT( "GOTCHAMACHINE_MYSTERYEGG_ITEM_NAME" );
	switch( m_eType )
	{
	case MYSTERY_NORMAL:
		return L"???";
	case MYSTERY_EGG3:
		{
			DmCS::StringFn::Replace( wsItemName, L"#Num#", 4 );	// 랜덤 4단 용병알
			return wsItemName;
		}
	case MYSTERY_EGG4:
	case MYSTERY_EGG5:
		{
			DmCS::StringFn::Replace( wsItemName, L"#Num#", 5 );	// 랜덤 5단 용병알
			return wsItemName;
		}
	}

	return L"";
}

int cGotchaMachineContents::GetUseItemCountAll() const
{
	return m_mapUseItems.size();
}

int cGotchaMachineContents::GetUseInvenCount(uint nItemType) const
{
	SAFE_POINTER_RETVAL( g_pDataMng, 0 );
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, 0 );
	return pInven->GetCount_Item_ID( nItemType );
}

cGotchaMachineContents::sUseItemInfo* cGotchaMachineContents::GetUseItem(int nIndex)
{
	std::map< int, sUseItemInfo >::iterator it = m_mapUseItems.find( nIndex );
	if( it != m_mapUseItems.end() )
		return &(it->second);

	return NULL;
}

u8 cGotchaMachineContents::GetEnableUseMoney() const
{
	std::map< int, sUseItemInfo >::const_iterator cit = m_mapUseItems.begin();
	std::map< int, sUseItemInfo >::const_iterator citEnd = m_mapUseItems.end();
	for( ; cit != citEnd; ++cit )
	{
		int nCount = GetUseInvenCount( cit->second.nUseItemType );
		if( 0 == nCount )
			continue;

		return cit->second.nUsePrice;
	}

	// 조건을 충족하는 아이템이 인벤에 없으면
	// 첫 번째 필요 아이템의 금액으로 세팅
	cit = m_mapUseItems.begin();
	if( cit != citEnd )
		return cit->second.nUsePrice;

	// 그것마저도 없으면
	return 0;
}

bool cGotchaMachineContents::IsGotchaPlaying() const
{
// 	if( eMachineState::NONE == m_eState )
// 		return false;
	if( !m_pNpc )
		return false;
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
void cGotchaMachineContents::sUseItemInfo::Set(uint nItemType, uint nItemCount, u8 nPrice /*= 0 */)
{
	nUseItemType = nItemType;
	nUseItemCount = nItemCount;
	nUsePrice = nPrice;
}
