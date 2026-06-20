#include "StdAfx.h"
#include "VipMemberShip_Contents.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include <shellapi.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

int const CVipMemberShipContents::IsContentsIdentity(void)
{
#ifdef SDM_VIP_SYSTEM_20181105
	return E_CT_VIPMEMBERSHIP_CONTENTS;
#else
	return E_CT_END;
#endif
}

CVipMemberShipContents::CVipMemberShipContents(void):m_bIsVip(true),m_fVipEndTime(0.0f)
{
	GAME_EVENT_ST.AddEvent( EVENT_CODE::RECV_VIP_SYSTEM_DATA, this, &CVipMemberShipContents::RecvVipSystemData );
}

CVipMemberShipContents::~CVipMemberShipContents(void)
{
	GAME_EVENT_ST.DeleteEventAll( this );
	EventRouter()->UnRegisterAll( this );
}

int const CVipMemberShipContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool CVipMemberShipContents::Initialize(void)
{

	return true;
}

void CVipMemberShipContents::UnInitialize(void)
{
}

bool CVipMemberShipContents::IntraConnection(ContentsSystem& System)
{ 
	return true; 
}

void CVipMemberShipContents::Update(float const& fElapsedTime)
{
	
}

void CVipMemberShipContents::NotifyEvent(int const& iNotifiedEvt)
{
}

void CVipMemberShipContents::MakeMainActorData(void)
{
}

void CVipMemberShipContents::ClearMainActorData(void)
{
}

void CVipMemberShipContents::MakeWorldData(void)
{
#ifdef SDM_VIP_SYSTEM_20181105
	GAME_EVENT_ST.AddEvent( EVENT_CODE::ON_VIP_SYSTEM_CONTENTS, this, &CVipMemberShipContents::OnVipSystemContents );
	if( net::game )
		net::game->Send_VipMemberShipData();
#endif
}

void CVipMemberShipContents::ClearWorldData(void)
{
	GAME_EVENT_ST.DeleteEvent( EVENT_CODE::ON_VIP_SYSTEM_CONTENTS, this );
	_CloseVipSystem();
}

//////////////////////////////////////////////////////////////////////////

void CVipMemberShipContents::OnClick_OpenBanner(std::string const& sOpenUrl)
{
	if( !sOpenUrl.empty() )
		ShellExecuteA(NULL, "open","iexplore",sOpenUrl.c_str(), NULL, SW_SHOWDEFAULT );
}

void CVipMemberShipContents::ShowInGameCashshopItemSelect()
{
	// 채널링 유저인 경우 웹 membership 아이템 웹 페이지 오픈
	if( GLOBALDATA_ST.IsChannelingService() )
	{
		//OnClick_OpenBanner( "http://www.digimonmasters.com/shop/ItemShop.aspx?mc=Tamer" );
	}
	else
	{
		DWORD dwVipMemberShopProductID = 51010300;
		ContentsStream ktmp;
		ktmp << dwVipMemberShopProductID;
		NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_Cashshop_ShowProductItem, ktmp );
	}
}

void CVipMemberShipContents::_ShowAndCloseMemberShipContents( bool const& bValue, __int64 const& i64EndTime )
{
	if( bValue )	_ShowVipSystem( i64EndTime );
	else			_CloseVipSystem();

// 	ContentsStream kTmp;
// 	kTmp << m_bIsVip;
// 	NotifyContentsAndStream( CONTENTS_EVENT::EStreamEvt_VipMemberShip_Change, kTmp);
	
#ifdef SDM_VIP_SYSTEM_20181105
	if( g_pGameIF )
	{
		cBaseWindow* pBaseWin = g_pGameIF->_GetPointer( cBaseWindow::WT_TAMER_WINDOW, 0 );
		if( pBaseWin )
		{
			cTamerWindow* pTamerWin = dynamic_cast<cTamerWindow*>(pBaseWin);
			if( pTamerWin )
				pTamerWin->ChangeVipMemberShip( m_bIsVip );
		}

		pBaseWin = g_pGameIF->_GetPointer( cBaseWindow::WT_MAIN_BAR, 0 );
		if( pBaseWin )
		{
			cMainBar* pMainBar = dynamic_cast<cMainBar*>(pBaseWin);
			if( pMainBar )
				pMainBar->SetButtonEnable(cMainBar::MAIN_BT_MEMBERSHIP, m_bIsVip);
		}
	}
#endif
}

void CVipMemberShipContents::_ShowVipSystem(__int64 const& i64VipEndTime)
{
	m_bIsVip = true;
	m_fVipEndTime = i64VipEndTime - net::net->GetTimeTS();

	ContentsStream kTmp;
	kTmp << m_fVipEndTime;
	Notify( eOnMemberShip, kTmp );

// 	SAFE_POINTER_RET( g_pGameIF );
// 
// 	cBaseWindow* pWin = g_pGameIF->_GetPointer(cBaseWindow::WT_VIP_BUTTON, 0);
// 	if( pWin )
// 		pWin->SetShowWindow( true );
}

void CVipMemberShipContents::_CloseVipSystem()
{
	m_bIsVip = false;
	m_fVipEndTime = 0.0f;

	ContentsStream kTmp;
	kTmp << m_fVipEndTime;
	Notify( eOffMemberShip );

	SAFE_POINTER_RET( g_pGameIF );

	cPopUpWindow* pPopupWindow = g_pGameIF->GetPopup();
	if( pPopupWindow )
		pPopupWindow->ClosePopup( cPopUpWindow::VIP_SYSTEM_MENU );

// 	cBaseWindow* pWin = g_pGameIF->_GetPointer(cBaseWindow::WT_VIP_BUTTON, 0);
// 	if( pWin )
// 		pWin->SetShowWindow( false );

	UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nScanWT );
	UINT nSellWT = cBaseWindow::WT_DIGITAMA_SELL;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nSellWT );

	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MAKE_TACTICS );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TACTICSHOUSE );
#ifdef LJW_EQUIPSYSTEM_190724
	GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_EQUIPSYSTEM, NULL );
#else
	GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_ELEMENT_ITEM, NULL );
#endif
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_JUMPBUSTER );
}

void CVipMemberShipContents::ShopVipPopupMemu(CsPoint csPosition)
{
	if( !m_bIsVip )
	{
		_CloseVipSystem();
		return;
	}

	SAFE_POINTER_RET( g_pGameIF );
	cPopUpWindow* pPopupWindow = g_pGameIF->GetPopup();
	SAFE_POINTER_RET( pPopupWindow );
	pPopupWindow->SetPopup( csPosition, CsPoint( 120, 0 ), cPopUpWindow::VIP_SYSTEM_MENU );
}

void CVipMemberShipContents::RecvVipSystemData(void* pData)
{
	SAFE_POINTER_RET( pData );
	GS2C_RECV_VIP_SYSTEM_DATA* pRecv = static_cast<GS2C_RECV_VIP_SYSTEM_DATA*>(pData);

	_ShowAndCloseMemberShipContents( pRecv->bIsVip, pRecv->unVipEndTime );	
}

void CVipMemberShipContents::OnVipSystemContents(void*pData)
{
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( g_pGameIF );

	if( !m_bIsVip )
	{
		_CloseVipSystem();
		return;
	}

	// 라이딩 중이거나 전투 중이면 사용 불가
	SAFE_POINTER_RET( g_pCharMng );
	CTamerUser* pTUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RET( pTUser );
	if( pTUser->IsRide() )
	{
		cPrintMsg::PrintMsg( 30014 );
		return;
	}
	if( pTUser->IsBattle() )
	{
		cPrintMsg::PrintMsg( 16002 );
		return;
	}

	int nActive = *static_cast<int*>(pData);
	switch( nActive )
	{
	case cBaseWindow::WT_DIGITAMA_SCAN:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SCAN ) )
				return;

			UINT nSellWT = cBaseWindow::WT_DIGITAMA_SELL;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nSellWT );

			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MAKE_TACTICS );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TACTICSHOUSE );
#ifdef LJW_EQUIPSYSTEM_190724
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_EQUIPSYSTEM, NULL );
#else
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_ELEMENT_ITEM, NULL );
#endif
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_JUMPBUSTER );

#ifdef SDM_VIP_SYSTEM_20181105
			int nIndex = -1;
#else
			int nIndex = 0;
#endif

			bool bSound = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

			SAFE_POINTER_RET( g_pCharMng );
			CTamerUser* pTamer = g_pCharMng->GetTamerUser();
			SAFE_POINTER_RET( pTamer );
			UINT nWindowType = cBaseWindow::WT_DIGITAMA_SCAN;
			ContentsStream kTmp;
			kTmp << nWindowType << nIndex << pTamer->GetPos2D();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_OPEN_PORTABLE, &kTmp );
		}
		break;	
	case cBaseWindow::WT_DIGITAMA_SELL:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SELL ) )
				return;

			UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nScanWT );

			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MAKE_TACTICS );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TACTICSHOUSE );
#ifdef LJW_EQUIPSYSTEM_190724
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_EQUIPSYSTEM, NULL );
#else
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_ELEMENT_ITEM, NULL );
#endif
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_JUMPBUSTER );

#ifdef SDM_VIP_SYSTEM_20181105
			int nIndex = -1;
#else
			int nIndex = 0;
#endif

			bool bSound = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

			SAFE_POINTER_RET( g_pCharMng );
			CTamerUser* pTamer = g_pCharMng->GetTamerUser();
			SAFE_POINTER_RET( pTamer );
			UINT nWindowType = cBaseWindow::WT_DIGITAMA_SELL;
			ContentsStream kTmp;
			kTmp << nWindowType << nIndex << pTamer->GetPos2D();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_OPEN_PORTABLE, &kTmp );
		}
		break;
	case cBaseWindow::WT_MAKE_TACTICS:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) )
				return;

			UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nScanWT );
			UINT nSellWT = cBaseWindow::WT_DIGITAMA_SELL;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nSellWT );

			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TACTICSHOUSE );
#ifdef LJW_EQUIPSYSTEM_190724
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_EQUIPSYSTEM, NULL );
#else
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_ELEMENT_ITEM, NULL );
#endif
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_JUMPBUSTER );

#ifdef SDM_VIP_SYSTEM_20181105
			cMakeTactics* pWin = dynamic_cast<cMakeTactics*>(g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAKE_TACTICS, 0 , -1 ));
#else
			cMakeTactics* pWin = dynamic_cast<cMakeTactics*>(g_pGameIF->GetDynamicIF( cBaseWindow::WT_MAKE_TACTICS, 0, 0 ));
#endif
			SAFE_POINTER_RET( pWin );

			bool bSound = false;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_INVENTORY, &bSound );

			CsPoint csPos = pWin->GetRootSize();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_POS_DEPENDON_INVEN, &csPos );
			pWin->SetPos( csPos );
			pWin->ResetDevice();
			pWin->SetTarget( NULL, true );
		}
		break;	
	case cBaseWindow::WT_TACTICSHOUSE:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_TACTICSHOUSE ) )
				return;

			UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nScanWT );
			UINT nSellWT = cBaseWindow::WT_DIGITAMA_SELL;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nSellWT );

			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MAKE_TACTICS );
#ifdef LJW_EQUIPSYSTEM_190724
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_EQUIPSYSTEM, NULL );
#else
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_ELEMENT_ITEM, NULL );
#endif
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_JUMPBUSTER );

			std::pair<int, NiPoint2> pairData;
			pairData.first = -1;
			pairData.second = g_pCharMng->GetTamerUser()->GetPos2D(); 
			GAME_EVENT_ST.OnEvent( EVENT_CODE::REQ_SHOWVIEWER_BT_ITEM, &pairData);
		}
		break;
#ifdef LJW_EQUIPSYSTEM_190724
	case cBaseWindow::WT_EQUIPMENT_SYSTEM:
		{
			UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nScanWT );
			UINT nSellWT = cBaseWindow::WT_DIGITAMA_SELL;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nSellWT );

			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MAKE_TACTICS );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TACTICSHOUSE );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_JUMPBUSTER );

			int nPortable = -1;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_EQUIPSYSTEM_PORTABLE, &nPortable );
		}
		break;
#else // LJW_EQUIPSYSTEM_190724
	case cBaseWindow::WT_ELEMENTITEM_CHECK:
		{
			UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nScanWT );
			UINT nSellWT = cBaseWindow::WT_DIGITAMA_SELL;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nSellWT );

			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MAKE_TACTICS );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TACTICSHOUSE );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_JUMPBUSTER );

			int nPortable = -1;
			std::pair< int, int > pairSend( nActive, nPortable );
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_ELEMENT_ITEM_PORTABLE, &pairSend );
		}
		break;	
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
		{
			UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nScanWT );
			UINT nSellWT = cBaseWindow::WT_DIGITAMA_SELL;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nSellWT );

			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MAKE_TACTICS );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TACTICSHOUSE );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_JUMPBUSTER );

			int nPortable = -1;
			std::pair< int, int > pairSend( nActive, nPortable );
			GAME_EVENT_ST.OnEvent( EVENT_CODE::OPEN_ELEMENT_ITEM_PORTABLE, &pairSend );
		}
		break;
#endif // LJW_EQUIPSYSTEM_190724
	case cBaseWindow::WT_JUMPBUSTER:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_JUMPBUSTER ) )
				return;

			UINT nScanWT = cBaseWindow::WT_DIGITAMA_SCAN;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nScanWT );
			UINT nSellWT = cBaseWindow::WT_DIGITAMA_SELL;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_CLOSE, &nSellWT );

			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_MAKE_TACTICS );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_TACTICSHOUSE );
#ifdef LJW_EQUIPSYSTEM_190724
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_EQUIPSYSTEM, NULL );
#else
			GAME_EVENT_ST.OnEvent( EVENT_CODE::CLOSE_ELEMENT_ITEM, NULL );
#endif

#if COMMON_LIB_FIXED
			cJumpBuster* pJump = static_cast<cJumpBuster*>(g_pGameIF->GetDynamicIF( cBaseWindow::WT_JUMPBUSTER, 0 , nItem::MEMBERSHIP_JUMP ));
			if( pJump )
				pJump->SetVipMemberShip( true );
#endif
		}
		break;
	default:
		return;
	}

	if(g_pCharMng && g_pCharMng->GetTamerUser())
		g_pCharMng->GetTamerUser()->DeletePath();	// 이동중인 패스를 삭제.

	if(g_pCharMng && g_pCharMng->GetDigimonUser( 0 ) )
		g_pCharMng->GetDigimonUser( 0 )->DeletePath();

	if(g_pServerMoveOwner)
		g_pServerMoveOwner->KeyReset();
}
