
#include "stdafx.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"
#include "CashShop.h"

#ifdef FT_USE_CASHSHOP

#define IF_CASHSHOP_CTYPE_POS		CsPoint( 194, 135 )

cCashShop::cCashShop() : m_pHaveCashRefresh(NULL),m_pCashRefresh(NULL),m_pFreeCashCharge(NULL)
{
	for( int n = 0; n < CsCashShopMng::CT_MAXCNT; ++n )
		m_pCashMoney[n] = NULL;

#ifndef VERSION_KOR
	for( int n = 0; n < U_MAXCNT; ++n )
		m_pCashUnit[n] = NULL;
#endif
}

cCashShop::~cCashShop()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cCashShop::Destroy()
{
	cBaseWindow::Delete();

	m_SubWin_Main.Destroy();
	m_SubWin_List.Destroy();
	m_SubWin_Best.Destroy();
	m_SubWin_Pay.Destroy();
}

void cCashShop::DeleteResource()
{
// #ifdef VERSION_STEAM
// 	nsCsFileTable::g_pCashShopMng_Steam->Delete_Element();
// #else
// 	nsCsFileTable::g_pCashShopMng->Delete_Element();
// #endif

#ifndef VERSION_KOR
	for( int i=0; i<U_MAXCNT; ++i )
		SAFE_NIDELETE( m_pCashUnit[ i ] );
#endif

	DeleteScript();
	m_SubWin_Char.Delete();
	
	SAFE_NIDELETE( m_pCategory_First );
	SAFE_NIDELETE( m_pMoneyImg );
}

void cCashShop::Create( int nValue /*=0*/ )
{
	m_pMoneyImg = NiNew cImage;

	char* pWinImg = NULL;
	char* pCoinImg = NULL;
	if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
	{
		pCoinImg = "Cash\\Shop\\coinImg_Aeria.tga";
		pWinImg = "Cash\\Shop\\win_Aeria.tga";
	}
	else if( GLOBALDATA_ST.IsCountry( GData::eCountry_Steam ) )
	{
		pCoinImg = "Cash\\Shop\\CoinImg_Steam.tga";
		pWinImg = "Cash\\Shop\\win_Steam.tga";	
	}
	else if( GLOBALDATA_ST.IsCountry( GData::eCountry_Kor ) )
	{
		pCoinImg = "Cash\\Shop\\CoinImg_KOR.tga";
		pWinImg = "Cash\\Shop\\Win_KOR.tga";
	}
	else
	{
		pCoinImg  = "Cash\\Shop\\CoinImg.tga";
		pWinImg = "Cash\\Shop\\Win.tga";
	}

#ifndef VERSION_KOR
	m_pCashUnit[ U_Each ] = NiNew cSprite;
	m_pCashUnit[ U_Each ]->Init( NULL, CsPoint::ZERO, CsPoint( 16, 10 ), "Cash\\Shop\\Each.tga", false );

	m_pCashUnit[ U_Long ] = NiNew cSprite;
	m_pCashUnit[ U_Long ]->Init( NULL, CsPoint::ZERO, CsPoint( 17, 9 ), "Cash\\Shop\\Long.tga", false );

	m_pCashUnit[ U_Day ] = NiNew cSprite;
	m_pCashUnit[ U_Day ]->Init( NULL, CsPoint::ZERO, CsPoint( 14, 15 ), "Cash\\Shop\\Day.tga", false );
#endif

	m_pMoneyImg->Init( NULL, CsPoint::ZERO, CsPoint( 22, 21 ), pCoinImg, false );
	m_pMoneyImg->SetTexToken( CsPoint( 0, 21 ) );
	
	cBaseWindow::Init();
	
	InitScript( pWinImg, CsPoint( 0, 0 ) , CsPoint( 1024, 768 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );

	m_pEquipReset = AddButton( CsPoint( 63, 617 ), CsPoint( 89, 37 ), cButton::IMAGE_NOR_13, _LAN( "원래 대로" ) );
	m_pEquipAllDel = AddButton( CsPoint( 165, 617 ), CsPoint( 89, 37 ), cButton::IMAGE_NOR_13, _LAN( "모두 벗기" ) ); 	
	m_pCancelButton = AddButton( CsPoint( 918, 723 ), CsPoint( 89, 37 ), cButton::IMAGE_NOR_13, _LAN( "나가기" ) );

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_10 );
	ti.s_eTextAlign = DT_RIGHT;
	ti.s_eFontSize = CFont::FS_12;

#ifndef VERSION_STEAM
	CsPoint posCash( 227, 664 );
	for( int i=0; i<CsCashShopMng::CT_MAXCNT; ++i )
	{
#ifdef VERSION_USA
		if( i == CsCashShopMng::CT_Real_Cash )
		{
//			posCash.y += 28;
			posCash.y += 15;
			m_pCashMoney[ i ] = AddText( &ti, posCash );
			if( m_pCashMoney[ i ] )
				m_pCashMoney[ i ]->SetText(0);
		}
#elif VERSION_KOR
		m_pCashMoney[ i ] = AddText( &ti, posCash );
		if( m_pCashMoney[ i ] )
			m_pCashMoney[ i ]->SetText(0);
		posCash.y += 28 ;
#else
		if( i > CsCashShopMng::CT_Real_Cash )
			m_pCashMoney[ i ] = NULL;
		else
		{
			m_pCashMoney[ i ] = AddText( &ti, posCash );
			if( m_pCashMoney[ i ] )
				m_pCashMoney[ i ]->SetText(0);
			posCash.y += 28;
		}
#endif
	}	
#endif
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( g_pCharMng->GetTamerUser()->GetName() );

#ifdef VERSION_KOR
	AddText( &ti, CsPoint( 160, 538 ) );
#else
	AddText( &ti, CsPoint( 155, 538 ) );
#endif
	
	m_SubWin_Main.Create( nValue );	
	m_SubWin_List.Create( nValue );
	m_SubWin_Best.Create( nValue );
	m_SubWin_Pay.Create( nValue );
	m_SubWin_Char.Init( CsPoint( 0, 123 ) );	

	// 카테고리
	CsPoint pos = CsPoint( 343, 54 );
#ifdef VERSION_STEAM
	CsCashShopMng_Steam::sCATEGORY* pCat = nsCsFileTable::g_pCashShopMng_Steam->GetCategory();
#else
	CsCashShopMng::sCATEGORY* pCat = nsCsFileTable::g_pCashShopMng->GetCategory();
#endif
	int nChildCnt = pCat->s_vpChild.Size();
	for( int i=0; i<nChildCnt; ++i )
	{		
		if( i == 0 )
		{
			ti.SetText( pCat->s_vpChild[ i ]->s_szName );

			m_pCategory_First = NiNew cRadioButton;
			m_pCategory_First->Init( m_pRoot, pos, CsPoint( 100, 37 ), cCheckBox::IMAGE_RADIO_5, &ti, false );
		}
		else
		{
			pos.x += 102;
			ti.SetText( pCat->s_vpChild[ i ]->s_szName );
			m_pCategory_First->AddNode( pos, CsPoint( 105, 37 ), cCheckBox::IMAGE_RADIO_5, &ti );
		}
	}
	m_pCategory_First->SetCheckIndex( 0 );

	// 인터페이스 위치정보
	ti.s_Color = NiColor::BLACK;
	ti.s_bOutLine = false;
#ifdef VERSION_KOR
	ti.SetText( _LAN( "캐시 창고" ) );
#else
	ti.SetText( _LAN( "샵 창고" ) );
#endif
	m_pCashWareHouse = AddButton( CsPoint( 52, 720 ), CsPoint( 103, 41 ), cButton::IMAGE_NOR_14, &ti );

	ti.SetText( _LAN( "캐시충전" ) );
	m_pCashRefresh = AddButton( CsPoint( 161, 720 ), CsPoint( 103, 41 ), cButton::IMAGE_NOR_14, &ti );

#ifndef VERSION_STEAM
	m_pHaveCashRefresh = AddButton( CsPoint( 270, 660 ), CsPoint( 38, 38 ),CsPoint( 0, 38 ), "GiftStorage\\Btn_Refresh.tga" );
#endif

#ifdef VERSION_KOR// 한국만 무료 충전 웹 페이지 오픈
// 	ti.SetText( _LAN( "무료충전" ) );
// 	m_pFreeCashCharge = AddButton( CsPoint( 270, 720 ), CsPoint( 103, 41 ), cButton::IMAGE_NOR_14, &ti );
// 	if( m_pFreeCashCharge )
// 		m_pFreeCashCharge->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cCashShop::_OpenFreeChargeWebPage );
#endif

	SetEnableWindow( false );

	CreateElement();

	SetEnableWindow( true );	

	_RefreshCashMoney();
}

void cCashShop::CreateElement()
{
	m_SubWin_Main.CreateElement();
	m_SubWin_Best.CreateElement();
}

void cCashShop::Update(float const& fDeltaTime)
{
	m_SubWin_Char.Update(fDeltaTime);
	m_SubWin_List.Update(fDeltaTime);	
	m_SubWin_Pay.Update(fDeltaTime);

	if(m_pHaveCashRefresh)
	{
		if( g_pDataMng->GetCashShopRefreshCoolTime()->IsCoolTime() == true )
			m_pHaveCashRefresh->SetEnable(false);
		else
			m_pHaveCashRefresh->SetEnable(true);
	}
}

cBaseWindow::eMU_TYPE
cCashShop::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
	{	
		m_SubWin_Best.ReleaseMask();
		m_SubWin_Main.ReleaseMask();
		m_SubWin_List.ReleaseMask();
		return muReturn;
	}	


	cBaseWindow::eMU_TYPE muSubReturn;
	muSubReturn = m_SubWin_Pay.Update_ForMouse();

	if( muSubReturn != MUT_OUT_WINDOW )
		return muSubReturn;

	if( m_SubWin_Pay.GetState() != cCashShop_Pay::PT_NONE )
		return MUT_CLICK_FOCUS;

	switch( m_pCashWareHouse->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CASH ) == true )
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CASH );
			else
				g_pGameIF->GetDynamicIF( cBaseWindow::WT_CASH );			
		}
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pEquipAllDel->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		m_SubWin_Char.SetAvatarDel();
		return muReturn;
	default:
		return muReturn;
	}

	switch( m_pEquipReset->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		m_SubWin_Char.SetAvatarTamer();
		return muReturn;
	default:
		return muReturn;
	}

	if( m_pCashRefresh )
	{
		switch( m_pCashRefresh->Update_ForMouse() )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				GetSystem()->OpenCashChargeUrl();
			}return muReturn;
		default:
			return muReturn;
		}
	}
	
	if(m_pHaveCashRefresh)
	{
		switch(m_pHaveCashRefresh->Update_ForMouse())
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				g_pDataMng->GetCashShopRefreshCoolTime()->Start();
				GetSystem()->CashShop_Refresh();
			}return muReturn;
		default:
			return muReturn;
		}
	}

	if( m_pFreeCashCharge && m_pFreeCashCharge->Update_ForMouse() == cButton::ACTION_CLICK )
		return muReturn;

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		Close();
		return muReturn;
	default:
		return muReturn;
	}
	
	if( m_pCategory_First->Update_ForMouse() == cRadioButton::ACTION_CLICK )
	{
		cWindow::PlaySound( cWindow::SD_At2 );		

		switch( m_pCategory_First->GetCheckIndex() )
		{
		case 0:	// 메인				
			break;
		case 1:	// 테이머
			m_SubWin_List.Init( cCashShop_List::LT_TAMER );	
			break;
		case 2: // 디지몬
			m_SubWin_List.Init( cCashShop_List::LT_DIGIMON );	
			break;
		case 3: // 아바타
			m_SubWin_List.Init( cCashShop_List::LT_AVATAR );
			break;
		case 4:	// 셋트
			m_SubWin_List.Init( cCashShop_List::LT_SET );
			break;
		default:
			assert_cs( false );
			break;
		}			
	}

	m_SubWin_Char.Updatae_Mouse();

	switch( m_pCategory_First->GetCheckIndex() )
	{
	case 0:	// 메인		
		muSubReturn = m_SubWin_Main.Update_ForMouse();									
		break;
	case 1:	// 테이머
	case 2: // 디지몬
	case 3: // 아바타
	case 4:	// 셋트
		muSubReturn = m_SubWin_List.Update_ForMouse();	
		break;
	default:
		assert_cs( false );
		break;
	}		

	muSubReturn = m_SubWin_Best.Update_ForMouse();

	if( muSubReturn != MUT_OUT_WINDOW )
		return muSubReturn;	

	return muReturn;
}


cBaseWindow::eMU_TYPE
cCashShop::Update_ForMouse_Disable()
{
	return m_SubWin_Pay.Update_ForMouse();
}

void cCashShop::Render()
{
	RenderScript();	

	switch( m_pCategory_First->GetCheckIndex() )
	{
	case 0:	// 메인
		m_SubWin_Main.Render();
		break;
	case 1:	// 테이머
	case 2: // 디지몬
	case 3: // 아바타
	case 4:	// 셋트		
		m_SubWin_List.Render();
		break;
	default:
		assert_cs( false );
		break;
	}	

	m_SubWin_Best.Render();
	m_SubWin_Char.Render( GetRootClient() + CsPoint( 0, 123 ) );
	m_pCategory_First->Render();

	EndRenderScript();
	
	m_SubWin_Pay.Render();

	if(m_pHaveCashRefresh)
		g_pIconMng->RenderTime( m_pHaveCashRefresh->GetDefaultPos() + GetRootClient() /*+ CsPoint( 1, 0 )*/, 19.0f, g_pDataMng->GetCashShopRefreshCoolTime() );
}


void cCashShop::OnRButtonUp( CsPoint pos )
{
}

bool cCashShop::OnEscapeKey()
{
	( m_SubWin_Pay.GetState() == cCashShop_Pay::PT_NONE ) ? Close() : m_SubWin_Pay.Close();
	
	return true;
}
bool cCashShop::OnEnterKey()
{
	( m_SubWin_Pay.GetState() == cCashShop_Pay::PT_NONE ) ? false : m_SubWin_Pay.OnEnterKey();

	return true;
}


void cCashShop::ResetDevice()
{	
	ResetDeviceScript();	

	m_SubWin_Best.ResetDevice();
	m_SubWin_Main.ResetDevice();
	m_SubWin_Char.ResetDevice();
	m_SubWin_List.ResetDevice();
	m_SubWin_Pay.ResetDevice();
	m_pCategory_First->ResetDevice();

#ifndef VERSION_KOR
	for( int i=0; i<U_MAXCNT; ++i )		
		m_pCashUnit[ i ]->ResetDevice();
#endif

	m_pMoneyImg->ResetDevice();
}

void cCashShop::SetItemLink( CsCashShop* pItem )
{
	assert_cs( pItem != NULL );
	if( !GetSystem()->IsItemPurchaseTime( pItem ) )
	{
		_ResetCashShopItemData();
		cPrintMsg::PrintMsg( 20093 );
		return;
	}

	m_pCategory_First->SetCheckIndex( pItem->GetInfo()->GetLargrType() - 1 );
	m_SubWin_List.Init( (cCashShop_List::eLIST_TYPE)pItem->GetInfo()->GetLargrType(), pItem->GetInfo()->s_nID );	
}

void cCashShop::SetPay( sCASHSHOP_ITEM* pItem, cCashShop_Pay::PAY_TYPE ePay )
{
	g_pGameIF->GetCashShop()->SetEnableWindow( false );
	switch( ePay )
	{
	case cCashShop_Pay::PT_ONE:
		m_SubWin_Pay.BuyOne( pItem,
			GetSystem()->GetCashMoney( CsCashShopMng::CT_Real_Cash ), 
			GetSystem()->GetCashMoney( CsCashShopMng::CT_Bonus_Cash), 
			GetSystem()->GetHaveTotalCash());	
		break;
	case cCashShop_Pay::PT_PRESENT:
		assert_cs( false );
		//m_SubWin_Pay.BuyPresent( pItem );
		break;
	default:
		assert_cs( false );
		break;
	}		
}

void cCashShop::SetCashMoney( int nCashType, int nMoney )
{
#ifdef VERSION_KOR		// 한국만 캐시머니 2개 표시 나머진 1개만 표시
	TCHAR szMoney[ 64 ] = {0,};	
	nsCSFILE::GetNumberStr( szMoney, 64, nMoney );
	if( m_pCashMoney[nCashType] )
		m_pCashMoney[nCashType]->SetText( szMoney );
#else
	int nTotalHaveCash = GetSystem()->GetHaveTotalCash();
	TCHAR szMoney[ 64 ] = {0,};	
	nsCSFILE::GetNumberStr( szMoney, 64, nTotalHaveCash );
	if( m_pCashMoney[0] )
		m_pCashMoney[0]->SetText( szMoney );

#endif
}

int cCashShop::GetCashType()
{
	if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
		return CsCashShopMng::CT_Real_Cash;
	else
		return CsCashShopMng::CT_Bonus_Cash;
}

void cCashShop::_OpenFreeChargeWebPage(void* pSender, void* pData)
{
	ShellExecuteA(NULL, "open","iexplore", "http://www.digimonmasters.com/cash/reward.aspx", NULL, SW_SHOWDEFAULT );
}

bool cCashShop::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eRefresh_Data, this );
	GetSystem()->Register(SystemType::eRefresh_CashMoney, this);
	return true;
}

void cCashShop::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{		
	switch(iNotifiedEvt)
	{
	case SystemType::eRefresh_Data:		_ResetCashShopItemData();		break;
	case SystemType::eRefresh_CashMoney: _RefreshCashMoney(); break;
	}	
}

void cCashShop::_ResetCashShopItemData()
{
	SetEnableWindow( false );
	CreateElement();
	SetEnableWindow( true );
	m_pCategory_First->SetCheckIndex( 0 );
}

void cCashShop::_RefreshCashMoney()
{
	int nReal_Cash = GetSystem()->GetCashMoney( CsCashShopMng::CT_Real_Cash );
	int nBonus_Cash = GetSystem()->GetCashMoney( CsCashShopMng::CT_Bonus_Cash );

	SetCashMoney( CsCashShopMng::CT_Real_Cash, nReal_Cash );
	SetCashMoney( CsCashShopMng::CT_Bonus_Cash, nBonus_Cash );
}

#endif
