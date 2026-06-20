#include "StdAfx.h"
#include "CashShopContents.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../Adapt/AdaptBattleSystem.h"
#include "../Adapt/AdaptTutorialQuest.h"

int const CashShopContents::IsContentsIdentity(void)
{
#ifdef FT_USE_CASHSHOP
	return E_CT_CASHSHOP;
#else
	return E_CT_END;
#endif
}

CashShopContents::CashShopContents(void):m_bUpdateCashInfo(false) ,m_dwSteamTmpTableIdx(0), m_AdaptBattleSystem(0), m_AdaptTutorialQuest(0)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::CASHSHOP_TOGGLE, this, &CashShopContents::CashShop_ToggleCheck); 
	GAME_EVENT_ST.AddEvent( EVENT_CODE::REQUEST_STEAM_CASHITEM_PURCHASE, this, &CashShopContents::Callback_CashItemPurchase_Steam);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_REQUEST_STEAM_CASHITEM_PURCHASE, this, &CashShopContents::Recv_RequestCashItemPurchase_Steam);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::SEND_CASHSHOP_ITEM_PURCHASE, this, &CashShopContents::SendCashItemPurchase);

	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_RESULT_CASHITEM_PURCHASE, this, &CashShopContents::Recv_Result_ItemPurchase);
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_RESULT_HAVE_CASH_REFRESH, this, &CashShopContents::Recv_Result_CashRefresh);	

	GAME_EVENT_ST.AddEvent( EVENT_CODE::CASH_TOGGLE, this, &CashShopContents::Cash_ToggleCheck); 
}

CashShopContents::~CashShopContents(void)
{
	m_AdaptBattleSystem = 0;
	m_AdaptTutorialQuest = 0;
	GAME_EVENT_ST.DeleteEventAll( this );
}

int const CashShopContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CashShopContents::Initialize(void)
{
	InitCashMoney();
	return true;
}

void CashShopContents::UnInitialize(void)
{
}

bool CashShopContents::IntraConnection(ContentsSystem& System)
{
	m_AdaptBattleSystem = new AdaptBattleSystem;
	if( m_AdaptBattleSystem && !m_AdaptBattleSystem->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_BATTLE_SYSTEM ) ) )
		return false;

	m_AdaptTutorialQuest = new AdaptTutorialQuest;
	if( m_AdaptTutorialQuest && !m_AdaptTutorialQuest->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CONTENTS_TYPE::E_CT_TUTORIAL_CONTENTS ) ) )
		return false;

	return true;
}

void CashShopContents::Update(float fElapsedTime)
{
}

void CashShopContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void CashShopContents::MakeMainActorData(void)
{
}

void CashShopContents::ClearMainActorData(void)
{
	m_bUpdateCashInfo = false;
}

void CashShopContents::ClearWorldData(void)
{
	m_bUpdateCashInfo = false;
}

void CashShopContents::Cash_ToggleCheck(void* pData)
{
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CASH, 0 ) )					
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CASH, 0 );
	else
	{
		if( m_AdaptTutorialQuest && m_AdaptTutorialQuest->IsTutorialPlaying() )
		{
			cPrintMsg::PrintMsg( 30412 );
			return;
		}
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_CASH, 0 );	
	}
}

void CashShopContents::CashShop_ToggleCheck(void* pData)
{
	if( GLOBALDATA_ST.IsChannelingService() ) // 채널링 계정으로 접속시 캐시샵 단축키 오픈 제어
	{
		cMessageBox::DelMsg( 20089, false );
		cPrintMsg::PrintMsg( 20089 );
		return ;
	}

	if( m_AdaptBattleSystem )
	{
// 		if( m_AdaptBattleSystem->IsBattleMatchingRegisted() )
// 			return;

		// 배틀 중에는 캐쉬샵 창을 열지 못하게함.
		if( m_AdaptBattleSystem->IsBattlePlaying() )
			return;
	}

	// 튜토리얼 중에는 캐쉬샵 오픈 금지
	if( m_AdaptTutorialQuest && m_AdaptTutorialQuest->IsTutorialPlaying() )
	{
		cPrintMsg::PrintMsg( 30412 );
		return;
	}

	if(m_bUpdateCashInfo == false )
	{
		CashShop_Refresh();
		m_bUpdateCashInfo = true;
		g_pDataMng->GetCashShopRefreshCoolTime()->Start();
	}

	// 이동 중지	
	if(g_pServerMoveOwner)
		g_pServerMoveOwner->KeyReset();

	g_pCharMng->GetTamerUser()->ReleaseArriveTarget();
	g_pCharMng->GetTamerUser()->DeletePath();		
	g_pCharMng->GetDigimonUser( 0 )->DeletePath();	

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_CASHSHOP, 0 ) )		
	{
		GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_HIDE_NEWCASHSHOP, NULL );	//close	
	}
	else if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_NEW_CASHSHOP, 0 ))
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CASHSHOP, 0 );

	}
	else
	{
		//g_pGameIF->GetDynamicIF( cBaseWindow::WT_CASHSHOP, 0 );
		if(GLOBALINPUT_ST.IsShift())
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_CASHSHOP, 0 );
		else
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_SHOW_NEWCASHSHOP, NULL );	//close
	}
}

int CashShopContents::GetCashType()
{
	if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
		return CsCashShopMng::CT_Real_Cash;
	else
		return CsCashShopMng::CT_Bonus_Cash;
}

//////////////////////////////////////////////////////////////////////////

void CashShopContents::SendCashItemPurchase( void* pData )
{
	SAFE_POINTER_RET( net::game );
	SAFE_POINTER_RET( pData );
	
	DWORD dwItemTableIdx = *static_cast<DWORD*>(pData);

#ifdef VERSION_STEAM
	if( STEAM_ST.IsOverlayEnable() )
		Request_CashItemPurchase_Steam( dwItemTableIdx );
	else
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_ITEM_BUY, 0 );
		cPrintMsg::PrintMsg(cPrintMsg::CASHSHOP_STEAM_OVERLAY_SYSTEM_DIABLE);
	}
#else
	SendCashItemPurchase( dwItemTableIdx );
#endif
}

void CashShopContents::Recv_RequestCashItemPurchase_Steam(void*pData)
{
	SAFE_POINTER_RET( pData );
	pair<int,int> result = *static_cast<pair<int,int>*>(pData);

	switch( result.first )
	{
	case 0: 
		break;
	case -2:
		{
			m_dwSteamTmpTableIdx = 0;
			cMessageBox::DelMsg( 14010, false );
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_ITEM_BUY, 0 );
			cPrintMsg::PrintMsg( 30310 );	
		}
		break;
	default:
		{
			m_dwSteamTmpTableIdx = 0;
			cMessageBox::DelMsg( 14010, false );
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_ITEM_BUY, 0 );

			cPrintMsg::PrintMsg( 30311 );	// 처리 완료 실패 캐시창 닫힘.
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CASHSHOP ) )
			{
				g_pGameIF->GetCashShop()->SetEnableWindow( true );
				CashShop_ToggleCheck(NULL);
			}
		}break;
	}
}


void CashShopContents::Callback_CashItemPurchase_Steam(void* pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( net::game );

	pair<unsigned char,uint64>	pairResult = *static_cast<pair<unsigned char,uint64>*>(pData);

	if( pairResult.first )
	{
		int nCashType = GetCashType();
		CsCashShop* pItemInfo = nsCsFileTable::g_pCashShopMng_Steam->GetCashShop( nCashType, m_dwSteamTmpTableIdx );
		SAFE_POINTER_RET( pItemInfo );

		n4 nItemID = pItemInfo->GetInfo()->s_nID;
		net::game->SendBuyCashItem( nCashType, 1, pItemInfo->GetInfo()->s_nPrice, &nItemID, pairResult.second );
	}
	else
	{
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_ITEM_BUY, 0 );
		cMessageBox::DelMsg( 14010, false );
	}
	m_dwSteamTmpTableIdx = 0;
}

//////////////////////////////////////////////////////////////////////////
// 아이템 구매 결과 
//////////////////////////////////////////////////////////////////////////
void CashShopContents::Recv_Result_ItemPurchase(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_CASHITEM_PURCHASE* pRecv = static_cast<GS2C_RECV_CASHITEM_PURCHASE*>(pData);

	g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_ITEM_BUY, 0 );
	cMessageBox::DelMsg( 14010, false );

	if( 0 != pRecv->m_dwResult )
	{
		cPrintMsg::PrintMsg( pRecv->m_dwResult );	// 처리 완료 실패 캐시창 닫힘.
		if( pRecv->m_bCloseCashShop )
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CASHSHOP ) )
			{
				g_pGameIF->GetCashShop()->SetEnableWindow( true );
				CashShop_ToggleCheck(NULL);
			}
		}
		return;
	}

	int nTotalCash = pRecv->m_nBonusCash + pRecv->m_nHaveCash;
	if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
		cPrintMsg::PrintMsg( 30318, &nTotalCash );
	else
	{
#ifdef VERSION_STEAM
		cPrintMsg::PrintMsg( 30321 );
#else
		cPrintMsg::PrintMsg( 30313, &nTotalCash );
#endif
	}

	SetCashMoney( CsCashShopMng::CT_Real_Cash, pRecv->m_nHaveCash );
	SetCashMoney( CsCashShopMng::CT_Bonus_Cash, pRecv->m_nBonusCash );
	Notify(eRefresh_CashMoney);
}

void CashShopContents::Recv_Result_CashRefresh(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_CASHITEM_PURCHASE* pRecv = static_cast<GS2C_RECV_CASHITEM_PURCHASE*>(pData);

	if( 0 != pRecv->m_dwResult )
	{
		cPrintMsg::PrintMsg( pRecv->m_dwResult );	// 처리 완료 실패 캐시창 닫힘.
		if( pRecv->m_bCloseCashShop )
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CASHSHOP ) )
			{
				g_pGameIF->GetCashShop()->SetEnableWindow( true );
				CashShop_ToggleCheck(NULL);
			}
		}
		return;
	}

	SetCashMoney( CsCashShopMng::CT_Real_Cash, pRecv->m_nHaveCash );
	SetCashMoney( CsCashShopMng::CT_Bonus_Cash, pRecv->m_nBonusCash );
	Notify(eRefresh_CashMoney);
}

//////////////////////////////////////////////////////////////////////////
// 캐쉬 아이템 구매 가능 시간 체크
//////////////////////////////////////////////////////////////////////////
bool CashShopContents::IsItemPurchaseTime( CsCashShop* pItemInfo )
{
	SAFE_POINTER_RETVAL( pItemInfo, false );
	CsCashShop::sINFO* pInfo = pItemInfo->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, false );

	uint nStartTime = nBase::GetTime( pInfo->s_szStartTime );
	uint nEndTime = nBase::GetTime( pInfo->s_szEndTime );
	uint nCurTime = net::game->GetTimeTS();
	if( nCurTime < nStartTime || nCurTime > nEndTime )
		return false;
	return true;
}


// 캐쉬 아이템 구매시 구매 가능 시간 체크하여
// 시간이 지난 경우 캐쉬샵UI를 닫는다.
bool CashShopContents::_CheckItemPurchaseTime( CsCashShop* pItemInfo )
{
	if( !IsItemPurchaseTime(pItemInfo) )
	{	
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_ITEM_BUY, 0 );
		Notify( eRefresh_Data );
		cPrintMsg::PrintMsg( 20093 );
		return false;
	}
	return true;
}

void CashShopContents::Request_CashItemPurchase_Steam( DWORD const& dwTbIdx )
{
	if( 0 != m_dwSteamTmpTableIdx )// 이미 서버에 요청중인 아이템이 있는경우 예외 처리
		return;

	int nCashType = GetCashType();

	m_dwSteamTmpTableIdx = 0;

	CsCashShop* pItemInfo = nsCsFileTable::g_pCashShopMng_Steam->GetCashShop( nCashType, dwTbIdx );
	SAFE_POINTER_RET( pItemInfo );

	if( !_CheckItemPurchaseTime( pItemInfo ) )
		return;

	m_dwSteamTmpTableIdx = dwTbIdx;

	n4 nItemID = pItemInfo->GetInfo()->s_nID;
	net::game->SendCashShopBuyRequest_Steam( nCashType, 1, pItemInfo->GetInfo()->s_nPrice, &nItemID );	
	cPrintMsg::PrintMsg( 14010 );
}

void CashShopContents::SendCashItemPurchase( DWORD const& dwTbIdx )
{
	int nCashType = GetCashType();
	CsCashShop* pItemInfo = nsCsFileTable::g_pCashShopMng->GetCashShop( nCashType, dwTbIdx );
	SAFE_POINTER_RET( pItemInfo );

	if( !_CheckItemPurchaseTime( pItemInfo ) )
		return;

	int nTotalHaveCash = GetHaveTotalCash();
	if( nTotalHaveCash < pItemInfo->GetInfo()->s_nPrice )
	{						
		g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_ITEM_BUY, 0 );
		if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
			cPrintMsg::PrintMsg( 30319 );
		else
			cPrintMsg::PrintMsg( 30310 );
		return;
	}

	n4 nItemID = pItemInfo->GetInfo()->s_nID;
	net::game->SendBuyCashItem( nCashType, 1, pItemInfo->GetInfo()->s_nPrice, &nItemID );	
	cPrintMsg::PrintMsg( 14010 );
}

void CashShopContents::CashShop_Refresh()
{
	if( net::game )
		net::game->SendCashShopBalance();
} 

extern void spawn_browser(	const char *uri,	// URL
							int nWidth,				// Window Width
							int nHeight,			// Window Height
							HICON hIcon);			// Handle to the icon

void CashShopContents::OpenCashChargeUrl()
{
	std::string cCashChargeUrl = GLOBALDATA_ST.GetCashChargeUrl();

	if(GLOBALDATA_ST.IsCountry( GData::eCountry_Steam ))
	{
#ifdef VERSION_STEAM
		STEAM_ST.SteamCashChageWebOpen();
#endif
	}
	else if( GLOBALDATA_ST.IsCountry( GData::eCountry_GSP ) )
	{
		char szEncryption[1024] = {0,};
		sprintf_s( szEncryption, 256, cCashChargeUrl.c_str(), GLOBALDATA_ST.GetAccountID().c_str(), net::MailAddress, net::cTicketingtime, net::cAccessToken);
		spawn_browser( szEncryption, 700, 800, NULL );
	}
	else
	{
		if( !cCashChargeUrl.empty() )
			ShellExecuteA(NULL, "open","iexplore",cCashChargeUrl.c_str(), NULL, SW_SHOWDEFAULT );
	}
};

void CashShopContents::InitCashMoney()
{
	m_nCashMoney[ CsCashShopMng::CT_Real_Cash ] = 0;
	m_nCashMoney[ CsCashShopMng::CT_Bonus_Cash ] = 0;

}

int CashShopContents::GetCashMoney( int const& nCashType ) const
{
	if( CsCashShopMng::CT_Real_Cash <= nCashType && nCashType < CsCashShopMng::CT_MAXCNT)
		return m_nCashMoney[ nCashType ];
	return 0;
}

void CashShopContents::SetCashMoney(int const& nCashType, int const& nMoney )
{
	if( CsCashShopMng::CT_Real_Cash <= nCashType && nCashType < CsCashShopMng::CT_MAXCNT)
		m_nCashMoney[ nCashType ] = nMoney; 
}

int CashShopContents::GetHaveTotalCash() const
{
	return m_nCashMoney[ CsCashShopMng::CT_Real_Cash ] + m_nCashMoney[ CsCashShopMng::CT_Bonus_Cash ];
}