#include "StdAfx.h"
#include "Trade_Contents.h"
#include "../Adapt/AdaptTutorialQuest.h"
#include "../Adapt/AdaptMacroProtectSystem.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CTradeContents::sTradeInfo::sTradeInfo():m_Money(0),m_bInvenLock(false),m_bStandBy(false)
{
	for( int n=0; n < MAX_TRADE_ITEM_SLOT; ++n )
		m_vecItems.push_back( sINFO() );
}

CTradeContents::sTradeInfo::~sTradeInfo()
{
	m_vecItems.clear();
}

void CTradeContents::sTradeInfo::Reset()
{
	for( int n=0; n < MAX_TRADE_ITEM_SLOT; ++n )
	{
		if( m_vecItems[n].s_nInvenIndex != -1 )
			g_pDataMng->ItemUnlock( TO_INVEN_SID( m_vecItems[n].s_nInvenIndex ) );
		m_vecItems[n].Reset();
	}
	
	m_Money = 0;		// 거래 금액
	m_bInvenLock = false;		// 준비 상태
	m_bStandBy = false;
}

bool CTradeContents::sTradeInfo::ItemRegist( cItemData const& registItem, n1 const& TradeInvenSlotNum, u2 const& InvenSlotNum, bool bIsMyItemRegist )
{
	if( TradeInvenSlotNum < 0 || TradeInvenSlotNum > MAX_TRADE_ITEM_SLOT )
		return false;

	m_vecItems[TradeInvenSlotNum].s_Item = registItem;
	m_vecItems[TradeInvenSlotNum].s_nItemType = registItem.GetType();
	m_vecItems[TradeInvenSlotNum].s_nItemCount = registItem.GetCount();
	if( m_vecItems[TradeInvenSlotNum].s_nInvenIndex != -1 )
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_vecItems[TradeInvenSlotNum].s_nInvenIndex ) );

	if( bIsMyItemRegist )
	{
		m_vecItems[TradeInvenSlotNum].s_nInvenIndex = InvenSlotNum;
		if( m_vecItems[TradeInvenSlotNum].s_nInvenIndex != -1 )
			g_pDataMng->ItemLock( TO_INVEN_SID( m_vecItems[TradeInvenSlotNum].s_nInvenIndex ) );
	}

	return true;
}

void CTradeContents::sTradeInfo::UnRegistItem( int const& nSlot )
{
	if( MAX_TRADE_ITEM_SLOT <= nSlot || nSlot < 0 )
		return;

	if( m_vecItems[nSlot].s_nInvenIndex != -1 )
		g_pDataMng->ItemUnlock( TO_INVEN_SID( m_vecItems[nSlot].s_nInvenIndex ) );
	m_vecItems[nSlot].Reset();
}

void CTradeContents::sTradeInfo::ChangeMoney(u8 const& nChangeMoney)
{
	m_Money = nChangeMoney;
}

void CTradeContents::sTradeInfo::SetInvenLock( bool bLock )
{
	m_bInvenLock = bLock;
	if( !bLock )
		m_bStandBy = false;
}

u8 CTradeContents::sTradeInfo::GetTradeMoney() const
{
	return m_Money;
}

bool CTradeContents::sTradeInfo::IsInvenLock() const
{
	return m_bInvenLock;
}

int	CTradeContents::sTradeInfo::GetEmptySlotNum() const
{
	for( int n = 0; n < MAX_TRADE_ITEM_SLOT; ++n )
	{
		if( m_vecItems[n].s_Item.m_nType != 0 )
			continue;
		return n;
	}
	return -1;
}

bool CTradeContents::sTradeInfo::HaveEmptySlot() const
{
	return GetEmptySlotNum() != -1 ? true : false;
}

bool CTradeContents::sTradeInfo::IsEmptySlot(int const& nSlotNum) const
{
	if( MAX_TRADE_ITEM_SLOT <= nSlotNum || nSlotNum < 0 )
		return false;
	return m_vecItems[nSlotNum].s_Item.m_nType == 0 ? true : false;
}

cItemData const * CTradeContents::sTradeInfo::GetItemData( int const& nSlotNum ) const
{
	if( MAX_TRADE_ITEM_SLOT <= nSlotNum || nSlotNum < 0 )
		return NULL;

	if( m_vecItems[nSlotNum].s_Item.m_nType == 0 )
		return NULL;
		 
	return &m_vecItems[nSlotNum].s_Item;
}

void CTradeContents::sTradeInfo::RemoveTradeItemToInven( cData_Inven * pInven )
{
	size_t totalSize = m_vecItems.size();
	for( size_t n =0; n < totalSize; ++n )
	{
		if( m_vecItems[n].s_nInvenIndex < 0 )
			continue;
		if( 0 == m_vecItems[n].s_nItemType || 0 == m_vecItems[n].s_nItemCount )
			continue;

		cItemInfo* pItemInfo = pInven->GetData( m_vecItems[n].s_nInvenIndex );
		SAFE_POINTER_CON(pItemInfo);
		if( m_vecItems[n].s_nItemType != pItemInfo->GetType() )
			continue;

		if( pItemInfo->GetCount() < m_vecItems[n].s_nItemCount )
			continue;

		pItemInfo->DecreaseCount( m_vecItems[n].s_nItemCount );
	}
}

void CTradeContents::sTradeInfo::AddTradeItemToInven( cData_Inven * pInven )
{
	size_t totalSize = m_vecItems.size();
	for( size_t n =0; n < totalSize; ++n )
	{
		if( 0 == m_vecItems[n].s_Item.m_nType )
			continue;

		pInven->ItemCrop( &m_vecItems[n].s_Item );
	}
}

void CTradeContents::sTradeInfo::SetStandBy(bool bValue)
{
	m_bStandBy = bValue;
}

bool CTradeContents::sTradeInfo::IsStandBy() const
{
	return m_bStandBy;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int const CTradeContents::IsContentsIdentity(void)
{
	return E_CT_TRADE;
}

CTradeContents::CTradeContents(void):m_TradeTargetObjectUIDX(0),m_AdaptTutorialQuest(0),m_AdaptMacroProtect(0)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_TRADE_REQUEST_TAMERNAME, this, &CTradeContents::Send_Trade_Request_Name );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_TRADE_REQUEST_TAMEROBJECTIDX, this, &CTradeContents::Send_Trade_Request_UIDX );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_TRADE_REQUEST_CANCEL, this, &CTradeContents::Send_Trade_Request_Cancel );
	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_TRADE_REQUEST_RESULT, this, &CTradeContents::Send_TradeRequest_Result );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_TRADE_MONEY_CHANGE, this, &CTradeContents::Send_Trade_MoneyChange );


	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_TRADE_ITEM_REGIST, this, &CTradeContents::Send_TradeItem_Regist);


	//////////////////////////////////////////////////////////////////////////
	// 거래 요청 받음.
	//////////////////////////////////////////////////////////////////////////			   
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_RECEIVE_REQUEST, this, &CTradeContents::Recv_TradeReceiveRequest );	

	//////////////////////////////////////////////////////////////////////////
	// 거래 요청에 대한 답변 패킷
	//////////////////////////////////////////////////////////////////////////
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_REQUEST_RESULT, this, &CTradeContents::Recv_TradeRequest_Result );	
	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_ITEM_REGIST, this, &CTradeContents::Recv_TradeItemRegist);	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_ITEM_UNREGIST, this, &CTradeContents::Recv_TradeItemUnregist );	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_MONEY_CHANGE, this, &CTradeContents::Recv_TradeMoneyChange );	


	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_INVEN_LOCK_UNLOCK, this, &CTradeContents::Recv_TradeInvenLock_UnLock );	
	
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_STANDBY_RESULT, this, &CTradeContents::Recv_Trade_StandBy );
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_TRADE_FINAL_RESULT, this, &CTradeContents::Recv_TradeFinal_Result );	

	if( EventRouter() )
	{
		EventRouter()->Register( CONTENTS_EVENT::EEvt_MacroProtect_Start, this );
		EventRouter()->Register( CONTENTS_EVENT::EStreamEvt_Inventory_UseItem, this );
	}
}

CTradeContents::~CTradeContents(void)
{
	m_AdaptTutorialQuest = 0;
	m_AdaptMacroProtect = 0;
	GAME_EVENT_STPTR->DeleteEventAll(this);
	if( EventRouter() )
		EventRouter()->UnRegisterAll( this );
}

int const CTradeContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CTradeContents::Initialize(void)
{
	return true;
}

void CTradeContents::UnInitialize(void)
{
}

bool CTradeContents::IntraConnection(ContentsSystem& System)
{
	m_AdaptTutorialQuest = new AdaptTutorialQuest;
	if( m_AdaptTutorialQuest && !m_AdaptTutorialQuest->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;

	m_AdaptMacroProtect = new AdaptMacroProtect;
	if( m_AdaptMacroProtect && !m_AdaptMacroProtect->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_MACROPROTECT ) ) )
		return false;

	return true;
}

void CTradeContents::Update(float const& fElapsedTime)
{
}

void CTradeContents::NotifyEvent(int const& iNotifiedEvt)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EEvt_MacroProtect_Start:
		{
			cMessageBox::DelMsg( 30018, false );
			if( IsTrading() )// 거래중 매크로 프로택트 질문이 왔을 경우 거래 중지 시키자
				SendTradeCancel();
		}break;
	}
}

void CTradeContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case CONTENTS_EVENT::EStreamEvt_Inventory_UseItem:
		{
			int nInvenIndex = 0;
			kStream >> nInvenIndex;
			Send_TradeItem_Regist( &nInvenIndex );
		}
		break;
	}
}

void CTradeContents::MakeMainActorData(void)
{
}

void CTradeContents::ClearMainActorData(void)
{
}

void CTradeContents::ClearWorldData(void)
{
	ResetData();
}

void CTradeContents::MakeWorldData(void)
{
}

void CTradeContents::ResetData(void)
{
	m_TradeTargetObjectUIDX = 0;	// 거래 대상의 인덱스	
	m_MyTradeData.Reset();
	m_TargetTradeData.Reset();
}

void CTradeContents::CloseTrade(void)
{
	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::EVENT_CHAT_PROCESS, this );	

	ResetData();
	if( g_pGameIF )
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TRADE );	
}
//////////////////////////////////////////////////////////////////////////

std::wstring CTradeContents::GetMyName()
{
	std::wstring myName;
	SAFE_POINTER_RETVAL(g_pCharMng, myName);

	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL(pUser, myName);
	myName = pUser->GetName();
	return myName;
}

std::wstring CTradeContents::GetTradeTamerName()
{
	std::wstring tamerName;
	SAFE_POINTER_RETVAL(g_pCharMng, tamerName);

	// 객체가 존재 하는가
	CsC_AvObject* pTamer = g_pMngCollector->GetObject( m_TradeTargetObjectUIDX );
	SAFE_POINTER_RETVAL(pTamer, tamerName);
	tamerName = pTamer->GetName();
	return tamerName;
}

bool CTradeContents::IsTrading() const // 거래 중인지 체크
{
	return m_TradeTargetObjectUIDX != 0 ? true : false;
}

void CTradeContents::SetTradeTargetObjectIdx( uint const& nTargetIdx )
{
	m_TradeTargetObjectUIDX = nTargetIdx;
}

u8 CTradeContents::GetMyTradeMoney() const
{
	return m_MyTradeData.GetTradeMoney();
}

u8 CTradeContents::GetTradeTamerMoney() const
{
	return m_TargetTradeData.GetTradeMoney();
}

bool CTradeContents::IsMyTradeInvenLock() const
{
	return m_MyTradeData.IsInvenLock();
}

bool CTradeContents::IsTradeTamerInvenLock() const
{
	return m_TargetTradeData.IsInvenLock();
}

cItemData const * CTradeContents::GetMyRegistItemData(int const& nSlot) const
{
	return m_MyTradeData.GetItemData( nSlot );
}

cItemData const * CTradeContents::GetTargetRegistItemData(int const& nSlot) const
{
	return m_TargetTradeData.GetItemData( nSlot );
}

uint CTradeContents::GetTradeTamerUID() const
{
	return m_TradeTargetObjectUIDX;
}

//////////////////////////////////////////////////////////////////////////
// 거래 요청 중인 테이머 거래 요청 취소
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Send_Trade_Request_Cancel(void* pData)
{
	SAFE_POINTER_RET(pData);

	int nTargetUIDX = *static_cast<int*>(pData);

	net::game->SendTradeReject( nTargetUIDX );
}

void CTradeContents::SendTradeCancel(void)
{
	if( 0 == m_TradeTargetObjectUIDX )
	{
		CloseTrade();
	}
	else
	{
		net::game->SendTradeReject( m_TradeTargetObjectUIDX );
	}
}

//////////////////////////////////////////////////////////////////////////
// 거래 인벤 잠금/해제
//////////////////////////////////////////////////////////////////////////
void CTradeContents::SendTradeInvenLock(bool bLock)
{
	if( 0 == m_TradeTargetObjectUIDX )		
		return;

	if( bLock == m_MyTradeData.IsInvenLock() )
		return;

	net::game->SendTradeInvenLock( bLock );	
}

//////////////////////////////////////////////////////////////////////////
// 최종 거래 승인
//////////////////////////////////////////////////////////////////////////
void CTradeContents::SendTradeFinal()
{
	if( 0 == m_TradeTargetObjectUIDX )		
		return;

	if( !m_MyTradeData.IsInvenLock() )
		return;

	net::game->SendTradeCompleteFinal();
}

//////////////////////////////////////////////////////////////////////////
// 테이머 이름으로 거래 요청
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Send_Trade_Request_Name(void* pData)
{
	SAFE_POINTER_RET(pData);

	std::wstring targetName = *static_cast<std::wstring*>(pData);

	CsC_AvObject* pTargetObj = g_pMngCollector->GetObject( RTTI_TAMER, targetName.c_str());

	RequestTrade( pTargetObj );
}
//////////////////////////////////////////////////////////////////////////
// 테이머 UIDX로 거래 요청
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Send_Trade_Request_UIDX(void* pData)
{
	SAFE_POINTER_RET(pData);

	unsigned int nTargetUIDX = *static_cast<unsigned int *>(pData);

	// 객체가 존재 하는가
	CsC_AvObject* pTamer = g_pMngCollector->GetObject( nTargetUIDX );
	RequestTrade( pTamer );
}

void CTradeContents::RequestTrade( CsC_AvObject* pTarget )
{
	if( pTarget == NULL )
	{
		cPrintMsg::PrintMsg( 30063 );
		return;
	}

	// 튜토리얼 중 거래 요청 불가
	if (m_AdaptTutorialQuest && m_AdaptTutorialQuest->IsTutorialPlaying())
	{
		cPrintMsg::PrintMsg(30412);
		return;
	}

	// 현재 상황에서는 거래가 되지 않는다. ( Npc이용, 전투중, 거래중, 진화중, 파트너몬 교체중 )
	if( g_pGameIF->IsOpenNpcWindow() || 
		g_pCharMng->GetDigimonUser( 0 )->IsBattle() || 
		g_pDataMng->GetServerSync()->IsChageDigimon() )
	{
		cPrintMsg::PrintMsg( 30024 );		
		return;
	}

	// 메세지 박스 떠있는게 있다면 거래 실패
	if( cMessageBox::IsMessageBox() )
	{
		cPrintMsg::PrintMsg( 30024 );
		return;
	}	

	// 객체가 테이머인가
	if( pTarget->GetLeafRTTI() != RTTI_TAMER )
	{
		cPrintMsg::PrintMsg( 30064 );
		return;
	}

	int nTargetLv = pTarget->GetBaseStat()->GetLevel();
	if( TRADE_MIN_TAMER_LEVEL > nTargetLv )
	{	// 대상이 거래 가능한 레벨이 아니다.
		cPrintMsg::PrintMsg( TRADE_RESULT_CANNOT_USE_TARGET_TAMER_LV );
		return;
	}

	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamerUser );

	// 가까운 거리에 있는가
	float fLen = ( pTamerUser->GetPos2D() - pTarget->GetPos2D() ).Length();
	if( fLen > TRADE_DISTANCE )
	{
		cPrintMsg::PrintMsg( 30022 );
		return;
	}
	//20170116 #4354 아이템 정렬중 거래 불가 메시지
	SAFE_POINTER_RET( g_pDataMng );
	if( g_pDataMng->IsSort() )
	{
		cPrintMsg::PrintMsg( 30023 );
		return;
	}

	int nTamerLv = pTamerUser->GetBaseStat()->GetLevel();
	if( TRADE_MIN_TAMER_LEVEL > nTamerLv )
	{// 내가 거래 가능한 테이머 최소 레벨이 아인경우
		cPrintMsg::PrintMsg( TRADE_RESULT_CANNOT_USE_TAMER_LV );
		return;
	}

	SAFE_POINTER_RET( net::game );

	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();	

	net::game->SendTradeRequest( pTarget->GetUniqID() );
	//  요청 하고 요청 대기	
	cPrintMsg::PrintMsg( 30019, pTarget->GetName() );
	cMessageBox* pMsgWin = cMessageBox::GetMessageBox( 30019 );
	if( pMsgWin )
		pMsgWin->SetValue1( pTarget->GetUniqID() );
}

//////////////////////////////////////////////////////////////////////////
// 거래 요청에 대한 답변
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Send_TradeRequest_Result( void* pData )
{
	SAFE_POINTER_RET(pData);

	if( IsTrading() ) // 이미 거래중이면 패스
		return;

	std::pair<bool, unsigned int>* pRe = static_cast<std::pair<bool, unsigned int>*>(pData);
	if( pRe->first )// 거래 수락
	{
		if( m_AdaptTutorialQuest && m_AdaptTutorialQuest->IsTutorialPlaying() )
		{
			cPrintMsg::PrintMsg( 30412 );
			net::game->SendTradeReject( pRe->second );
		}
		else
			net::game->SendTradeApproval( pRe->second );
	}
	else// 거래 거절
	{
		net::game->SendTradeReject( pRe->second );
	}
}

//////////////////////////////////////////////////////////////////////////
// 거래 금액 변경 패킷
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Send_Trade_MoneyChange(void* pData)
{
	SAFE_POINTER_RET(pData);

	if( m_MyTradeData.IsInvenLock() ) // 준비 상태 변경은 안됨.
		return;

	u8 changeMoney = *static_cast<u8*>(pData);
	net::game->SendTradeAddMoney( changeMoney );
}

//////////////////////////////////////////////////////////////////////////
// 거래 내 아이템 등록
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Send_TradeItem_Regist( void* pData )
{// 인벤토리에서 넘어온 이벤트
	SAFE_POINTER_RET(pData);
	int nSlotNum = *static_cast<int*>(pData);

	if( g_pGameIF && !g_pGameIF->IsActiveWindow( cBaseWindow::WT_TRADE ) )
		return;

	SendMyTradeItemRegist( CURSOR_ICON::CI_INVEN, nSlotNum, 0 );
}

void CTradeContents::SendMyTradeItemRegist( CURSOR_ICON::eTYPE eIconType, int const& nInvenPos, int const& nCount )	
{
	CURSOR_ST.ReleaseIcon();

	switch( eIconType )
	{
	case CURSOR_ICON::CI_INVEN:
		{
			int nRegistItemCount = nCount;
			int nItemCode = 0;
			if( CheckIsRegistAble( TO_ID( nInvenPos ), nRegistItemCount, nItemCode ) )
				net::game->SendTradeAddItem( nInvenPos, nRegistItemCount );
		}
		break;
	default:
		cPrintMsg::PrintMsg( 11018 );
		break;
	}
}

bool CTradeContents::CheckIsRegistAble( int const& nInvenSlotNum, int & nCount, int & nItemCode )
{	
// 	if( m_bRecvServerWait == true )
// 	{
// 		cPrintMsg::PrintMsg( 10004 );
// 		return false;
// 	}
	if( !IsTrading() )// 거래 중이 아닌경우
		return false;

	// 거래 준비 상태
	if( m_MyTradeData.IsInvenLock() || m_MyTradeData.IsStandBy() )
	{
 		cPrintMsg::PrintMsg( 11014 );
 		return false;
	}
	
	if( !m_MyTradeData.HaveEmptySlot() )
	{
		cPrintMsg::PrintMsg( 11037 );
		return false;
	}

	// 라이딩 상태인지 체크
	if( g_pCharMng->GetDigimonUser( 0 )->IsRide() )
	{
		cPrintMsg::PrintMsg( 30014 );
		return false;
	}

	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RETVAL( pInven, false );
	
	cItemInfo* pItem = pInven->GetData( nInvenSlotNum );
	SAFE_POINTER_RETVAL( pItem, false );

	if( !g_pDataMng->IsTradeItem( pItem ) )
		return false;

	if( nCount == 0 || nCount > pItem->GetCount() )
		nCount = pItem->GetCount();	

	nItemCode = pItem->GetType();
	return true;
};

bool CTradeContents::IsMacroProtecting() const
{
	SAFE_POINTER_RETVAL( m_AdaptMacroProtect, false );
	return m_AdaptMacroProtect->IsMacroProtectUsing();
}

//////////////////////////////////////////////////////////////////////////
// 거래 내 아이템 제거
//////////////////////////////////////////////////////////////////////////
void CTradeContents::SendMyTradeItemUnRegist( int const& nDelSlotNum )
{
	// 거래 준비 상태
	if( m_MyTradeData.IsInvenLock() || m_MyTradeData.IsStandBy() )
	{
		cPrintMsg::PrintMsg( 11014 );
		return;
	}

	if( m_MyTradeData.IsEmptySlot(nDelSlotNum) )
		return;

	net::game->SendTradeCancelItem( nDelSlotNum );
}

//////////////////////////////////////////////////////////////////////////
// 최종 승인 패킷
//////////////////////////////////////////////////////////////////////////
void CTradeContents::SendTradeCompleteFinal() 
{
	if( !m_MyTradeData.IsInvenLock() || !m_MyTradeData.IsStandBy() ) // 준비 상태 변경은 안됨.
		return;

	if( !m_TargetTradeData.IsInvenLock() ) // 준비 상태 변경은 안됨.
		return;

	net::game->SendTradeCompleteFinal();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CTradeContents::Recv_TradeRequest_Result(void* pData)
{
	cMessageBox::DelMsg( 30018, false );
	cMessageBox::DelMsg( 30019, false );

	SAFE_POINTER_RET( pData );

	GS2C_RECV_TRADE_REQUEST_RESULT * recv = static_cast<GS2C_RECV_TRADE_REQUEST_RESULT*>(pData);
	if( recv->m_dwResult != 0 )
	{	// 창 닫기
		cPrintMsg::PrintMsg( recv->m_dwResult );
		CloseTrade();
		return;
	}	
	
	CsC_AvObject* pTarget = g_pMngCollector->GetObject( recv->m_TargetTamerUID );
	if( pTarget == NULL )
	{
		cPrintMsg::PrintMsg( 30023 );
		CloseTrade();
		return;
	}

	SetTradeTargetObjectIdx( recv->m_TargetTamerUID );	

	// 거래 요청 수락을 받고 거래 창을 띄우려 할 때 
	// 매크로 프로텍터 사용 중이면 거래 취소 패킷을 보내자
	if( IsMacroProtecting() )
	{
		SendTradeCancel();
		return;
	}

	// 거래창 열기
	cBaseWindow* pTradeWindow = g_pGameIF->GetDynamicIF( cBaseWindow::WT_TRADE );
	SAFE_POINTER_RET( pTradeWindow );	

	// 인벤토리 모두 열기
	bool bSound = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

	// 인터페이스 보이게
	g_pGameIF->SetRenderIF( true );

	// 팝업 윈도우 닫아주자
	g_pGameIF->GetPopup()->ClosePopup();

	GAME_EVENT_ST.AddEvent( EVENT_CODE::EVENT_CHAT_PROCESS, this, &CTradeContents::TradeChatMsg );	
}

//////////////////////////////////////////////////////////////////////////
// 거래 창의 채팅창 메시지
//////////////////////////////////////////////////////////////////////////
void CTradeContents::TradeChatMsg(void* pData)
{
	SAFE_POINTER_RET(pData);
	ST_CHAT_PROTOCOL * chatMsg = static_cast<ST_CHAT_PROTOCOL*>(pData);
	
	NiColor color;
	switch( chatMsg->m_Type )
	{
	case NS_CHAT::NORMAL:		color = CHAT_NORMAL_COLOR;		break;
	case NS_CHAT::PARTY:		color = CHAT_PARTY_COLOR;		break;
	case NS_CHAT::GUILD:		color = CHAT_GUILD_COLOR;		break;
	case NS_CHAT::SET_FROMWHISPER:
	case NS_CHAT::WHISPER:		color = CHAT_WHISPER_COLOR;		break;	
	case NS_CHAT::SHOUT:		color = CHAT_SHOUT_COLOR;		break;
	default:	return;
	}

	std::wstring SenderName;
	std::wstring wMsg;
	if( 0 != chatMsg->m_Index )
	{
		CsC_AvObject* pObject = g_pMngCollector->GetObject( chatMsg->m_Index );
		if( pObject == NULL )
			return;

		SenderName = pObject->GetName();
		wMsg = chatMsg->m_wStr;
	}
	else
	{		
		std::wstring::size_type nPos = chatMsg->m_wStr.find(L" : ");
		if( nPos == std::wstring::npos )
			return;

		SenderName = chatMsg->m_wStr.substr( 0, nPos );
		wMsg = chatMsg->m_wStr.substr( nPos + 3 );
	}

	switch( chatMsg->m_Type )
	{
	case NS_CHAT::SET_FROMWHISPER:
		{
			std::wstring::size_type nPos = SenderName.find(L".");
			if( nPos != std::wstring::npos )
			{
				std::wstring name = SenderName.substr(nPos + 1);
				if( 0 != name.compare( g_pCharMng->GetTamerUser()->GetName() ) &&
					0 != name.compare( GetTradeTamerName() ) )
					return;
			}
		}break;
	case NS_CHAT::WHISPER:
		{
			std::wstring::size_type nPos = SenderName.find(L".");
			if( nPos != std::wstring::npos )
			{
				std::wstring name = SenderName.substr(nPos + 1);
				if( 0 != name.compare( g_pCharMng->GetTamerUser()->GetName() ) &&
					0 != name.compare( GetTradeTamerName() ) )
					return;
			}
		}
		break;
	default:
		{
			if( 0 != SenderName.compare( g_pCharMng->GetTamerUser()->GetName() ) &&
				0 != SenderName.compare( GetTradeTamerName() ) )
				return;
		}
		break;
	}



	std::wstring totalMsg;
	DmCS::StringFn::Format( totalMsg, L"%s : %s", SenderName.c_str(), wMsg.c_str() );

	ContentsStream kTmp;
	kTmp << totalMsg << color;

	Notify( eAdd_Trade_Chat_Msg, kTmp);
}

//////////////////////////////////////////////////////////////////////////
// 거래 요청 받았을 때 패킷 처리
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Recv_TradeReceiveRequest(void* pData)
{
	SAFE_POINTER_RET( pData );

	GS2C_RECV_TRADE_REQUEST *recv = static_cast<GS2C_RECV_TRADE_REQUEST*>(pData);
	if( IsTrading() ) // 이미 거래 중에 거래 요청 받았을 때
	{
		net::game->SendTradeImpossible( recv->m_TargetTamerUID );
		return;
	}

	// 매크로 프로텍터 답변 중에는 거래 불가
	if( IsMacroProtecting() )
	{
		net::game->SendTradeImpossible( recv->m_TargetTamerUID );
		return;
	}

	// 유저가 없다면 불가능 거부
	CsC_AvObject* pObject =	g_pMngCollector->GetObject( recv->m_TargetTamerUID );
	if( pObject == NULL )
	{
		net::game->SendTradeImpossible( recv->m_TargetTamerUID );
		return;
	}

	bool bShowRelocatePage = false;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::IS_SERVERR_RELOCATE_PAGE_SHOW, &bShowRelocatePage );

	// 현재 상황에서는 거래가 되지 않는다. ( Npc이용, 전투중, 거래중, 진화중, 파트너몬 교체중, 아이템 정렬 중 )
	if( g_pGameIF->IsOpenNpcWindow() || 
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_SPIRIT_EVOLUTION, 0 ) ||	//2017-01-10-nova 진화창이 열려있을경우 거래가 되지 않도록
		g_pCharMng->GetDigimonUser( 0 )->IsBattle() || 
		g_pDataMng->GetServerSync()->IsChageDigimon() || 
		cMessageBox::IsMessageBox() || 
		g_pDataMng->IsSort() ||
		bShowRelocatePage ) 	
	{
		net::game->SendTradeImpossible( recv->m_TargetTamerUID );
		return;
	}	

	if( g_pResist->m_Global.s_bTradeActive == false )// 거래 차단일 경우.
	{
		net::game->SendTradeReject( recv->m_TargetTamerUID );
		return;
	}

	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTamerUser );

	// 거래 가능한 테이머 최소 레벨
	int nTargetLv = pTamerUser->GetBaseStat()->GetLevel();
	if( TRADE_MIN_TAMER_LEVEL > nTargetLv )// 
	{
		net::game->SendTradeImpossible( recv->m_TargetTamerUID );
		return;
	}

	cCreateName::DeleteInstance();

	// 이동 중지
	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();

	// 거래요청 메세지
	cPrintMsg::PrintMsg( 30018, pObject->GetName() );

	cMessageBox* pMsg = cMessageBox::GetMessageBox(30018);
	if( pMsg )
		pMsg->SetValue1( recv->m_TargetTamerUID );
}

//////////////////////////////////////////////////////////////////////////
// 거래 아이템 등록
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Recv_TradeItemRegist(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TRADE_ITEM_REGIST* pRecv = static_cast<GS2C_RECV_TRADE_ITEM_REGIST*>(pData);
	 
	if( g_pCharMng->IsTamerUserFromUIDX(pRecv->m_TargetTamerUID) )
	{
		if( m_MyTradeData.ItemRegist( pRecv->m_ItemData, pRecv->m_TradeInvenSlotNum, pRecv->m_InventorySlotNum, true ) )
		{
			ContentsStream kTmp;
			int nAddInvenSlotNum = pRecv->m_TradeInvenSlotNum;
			kTmp << nAddInvenSlotNum;
			Notify( eMyTradeItem_Regist, kTmp );
		}
	}
	else if( m_TradeTargetObjectUIDX == pRecv->m_TargetTamerUID )
	{
		if( m_TargetTradeData.ItemRegist( pRecv->m_ItemData, pRecv->m_TradeInvenSlotNum, pRecv->m_InventorySlotNum, false ) )
		{
			ContentsStream kTmp;
			int nAddInvenSlotNum = pRecv->m_TradeInvenSlotNum;
			kTmp << nAddInvenSlotNum;
			Notify( eTargetTradeItem_Regist, kTmp );
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// 거래 아이템 제거
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Recv_TradeItemUnregist(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TRADE_ITEM_UNREGIST* pRecv = static_cast<GS2C_RECV_TRADE_ITEM_UNREGIST*>(pData);

	int nDelSlot = pRecv->m_nItemPosition;
	if( g_pCharMng->IsTamerUserFromUIDX(pRecv->m_TargetTamerUID) )
	{		
		m_MyTradeData.UnRegistItem(nDelSlot);
		ContentsStream kTmp;
		kTmp << nDelSlot;
		Notify( eMyTradeItem_UnRegist, kTmp );
	}
	else if( m_TradeTargetObjectUIDX == pRecv->m_TargetTamerUID )
	{
		m_TargetTradeData.UnRegistItem( nDelSlot );
		ContentsStream kTmp;
		kTmp << nDelSlot;
		Notify( eTargetTradeItem_UnRegist, kTmp );
	}
}

//////////////////////////////////////////////////////////////////////////
// 거래 돈 변경.
//////////////////////////////////////////////////////////////////////////

void CTradeContents::Recv_TradeMoneyChange(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TRADE_MONEY_CHANGE* pRecv = static_cast<GS2C_RECV_TRADE_MONEY_CHANGE*>(pData);

	if( g_pCharMng->IsTamerUserFromUIDX(pRecv->m_TargetTamerUID) )
	{
		m_MyTradeData.ChangeMoney( pRecv->m_nMoney );
		Notify( eMyTrade_Money_Change );
	}
	else if( m_TradeTargetObjectUIDX == pRecv->m_TargetTamerUID )
	{
		m_TargetTradeData.ChangeMoney( pRecv->m_nMoney );
		Notify( eTargetTrade_Money_Change );
	}	
}

//////////////////////////////////////////////////////////////////////////
// 거래 준비 상태 변경.
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Recv_TradeInvenLock_UnLock(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TRADE_INVEN_LOCK* pRecv = static_cast<GS2C_RECV_TRADE_INVEN_LOCK*>(pData);

	if( g_pCharMng->IsTamerUserFromUIDX( pRecv->m_TargetTamerUID ) )
		m_MyTradeData.SetInvenLock(pRecv->m_bLock);
	else if( m_TradeTargetObjectUIDX == pRecv->m_TargetTamerUID )
		m_TargetTradeData.SetInvenLock( pRecv->m_bLock );
	else
		return;

	Notify( eChange_Inven_Lock );
}

//////////////////////////////////////////////////////////////////////////
// 거래 완료 대기중
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Recv_Trade_StandBy(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TRADE_STANDBY* pRecv = static_cast<GS2C_RECV_TRADE_STANDBY*>(pData);

	if( g_pCharMng->IsTamerUserFromUIDX( pRecv->m_TargetTamerUID ) )
	{
		m_MyTradeData.SetStandBy(true);
		Notify(eTrade_StandBy);
	}
	else if( m_TradeTargetObjectUIDX == pRecv->m_TargetTamerUID )
		m_TargetTradeData.SetStandBy(true);
}

//////////////////////////////////////////////////////////////////////////
// 최종 거래 성공 여부
//////////////////////////////////////////////////////////////////////////
void CTradeContents::Recv_TradeFinal_Result(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_TRADE_FINAL_RESULT * pRecv = static_cast<GS2C_RECV_TRADE_FINAL_RESULT*>(pData);

	// 거래 실패
	if( 0 != pRecv->m_dwResult )
		return;

	// 거래 성공
	cData_Inven* pInven = g_pDataMng->GetInven();
	if( pInven )// 돈 증감.
	{
		if( m_MyTradeData.GetTradeMoney() > 0 )
			pInven->DecreaseMoney( m_MyTradeData.GetTradeMoney() );
		if( m_TargetTradeData.GetTradeMoney() > 0 )
			pInven->IncreaseMoney( m_TargetTradeData.GetTradeMoney(), true );
	}

	if( pInven )
	{
		m_MyTradeData.RemoveTradeItemToInven( pInven );
		m_TargetTradeData.AddTradeItemToInven( pInven );
	}

	
	// 거래 성공시 이팩트 출력
	if( g_pCharMng )
	{
		UINT unIdx = g_pCharMng->GetTamerUser()->GetUniqID();
		g_pCharMng->AttachEffectFromTamerUIDX( unIdx, "system\\TradeEnd.nif", 1.0f, 0 );
		g_pCharMng->AttachEffectFromTamerUIDX( m_TradeTargetObjectUIDX, "system\\TradeEnd.nif", 1.0f, 0 );
	}

	cPrintMsg::PrintMsg( 30020 );
	cWindow::PlaySound( "System\\Tactics_done.wav" );

	CloseTrade();

	// 퀵슬롯 체크
	cData_QuickSlot::CheckItemCount_AllUser();
	// 퀘스트 체크
	if( g_pDataMng )
	{
		cData_Quest* pQuestData = g_pDataMng->GetQuest();
		if( pQuestData )
			pQuestData->CalProcess();
	}
}
