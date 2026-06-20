

#include "stdafx.h"
#include "ItemSeparate.h"

#define IF_ITEM_SEP_TERA_EDIT_ID			5005		// _EDIT_ID 로 검색후 사용
#define IF_ITEM_SEP_MEGA_EDIT_ID			5006		// _EDIT_ID 로 검색후 사용
#define IF_ITEM_SEP_BIT_EDIT_ID				5007		// _EDIT_ID 로 검색후 사용
#define IF_ITEM_SEP_COUNT_EDIT_ID			5008		// _EDIT_ID 로 검색후 사용

#define IF_PROPERTY_A_EDIT_ID				6650
#define IF_PROPERTY_B_EDIT_ID				6651
#define IF_PROPERTY_C_EDIT_ID				6652
#define IF_PROPERTY_D_EDIT_ID				6653

#define IF_ITEM_SEP_ITEM_COUNT_POS			CsPoint( 67, 132 )
#define IF_ITEM_SEP_ITEM_PRICE_POS			CsPoint( 220, 132 )
#define IF_ITEM_SEP_MONEY_EDITBOX_POS	CsPoint( 136, 157 )
#define IF_ITEM_SEP_MONEY_EDITBOX_SIZE	CsPoint( 173, 20 )
#define IF_ITEM_SEP_EDITBOX_RGB				RGB( 0, 16, 36 )

// [4/6/2016 hyun] 에디트박스 위치
#define WINDOW_ITEMSEPARATE_POS				CsPoint( 300, 180 )

#define EDITBOX_ITEMCOUNT_POS				CsPoint( 46,  158 )
#define EDITBOX_MONEY_TERA_POS				CsPoint( 138, 161 )
#define EDITBOX_MONEY_MEGA_POS				CsPoint( 205, 161 )
#define EDITBOX_MONEY_BIT_POS				CsPoint( 256, 161 )

#define EDITBOX_ITEMCOUNT_SIZE				CsPoint( 40, 13 )
#define EDITBOX_MONEY_TERA_SIZE				CsPoint( 50, 13 )
#define EDITBOX_MONEY_MEGA_SIZE				CsPoint( 34, 13 )
#define EDITBOX_MONEY_BIT_SIZE				CsPoint( 34, 13 )
// @@ 여기까지

LRESULT CALLBACK EditSubProc_ItemSeparate(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
WNDPROC EditProc_ItemSeparate;

LRESULT CALLBACK EditSubProc_ItemSeparate(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{	
	case EM_UNDO:
		{
			return 0;
		}
		break;	
	}

	return CallWindowProc(EditProc_ItemSeparate,hWnd,uMsg,wParam,lParam);
}



cItemSeparate*		g_pItemSeparate = NULL;

void cItemSeparate::NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, eTYPE eSepType, cItemInfo* pItemInfo, int nItemMaxCnt, sMONEY sMoneyPerItem, cMoneyIF::eDISP_TYPE disp )
{
	if( g_pGameIF->IsActiveWindow( wt, nWTValue1 ) == true )
	{
		if( g_pGameIF->_GetPointer( wt, nWTValue1 )->IsEnableWindow() == false )
			return;
	}

	DeleteInstance();

	assert_cs( g_pItemSeparate == NULL );
	g_pItemSeparate = NiNew cItemSeparate;
	g_pItemSeparate->Init( wt, nWTValue1, eSepType, pItemInfo, nItemMaxCnt, sMoneyPerItem, disp );

	g_pItemSeparate->InitPassiveText();
}

void cItemSeparate::NewInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, eTYPE eSepType, cItemInfo* pItemInfo, int nItemMaxCnt, sMONEY sMoneyPerItem, cMoneyIF::eDISP_TYPE disp, DWORD dwPriceItemCode )
{
	if( g_pGameIF->IsActiveWindow( wt, nWTValue1 ) == true )
	{
		if( g_pGameIF->_GetPointer( wt, nWTValue1 )->IsEnableWindow() == false )
			return;
	}

	DeleteInstance();

	assert_cs( g_pItemSeparate == NULL );
	g_pItemSeparate = NiNew cItemSeparate;
	g_pItemSeparate->Init( wt, nWTValue1, eSepType, pItemInfo, nItemMaxCnt, sMoneyPerItem, disp, dwPriceItemCode );

	g_pItemSeparate->InitPassiveText();
}

void cItemSeparate::NewInstance( cBaseWindow::eWINDOW_TYPE wt, DWORD dwNpcID, int nWTValue1, eTYPE eSepType, cItemInfo* pItemInfo, int nItemMaxCnt, sMONEY sMoneyPerItem, cMoneyIF::eDISP_TYPE disp )
{
	if( g_pGameIF->IsActiveWindow( wt, nWTValue1 ) == true )
	{
		if( g_pGameIF->_GetPointer( wt, nWTValue1 )->IsEnableWindow() == false )
			return;
	}

	DeleteInstance();

	assert_cs( g_pItemSeparate == NULL );
	g_pItemSeparate = NiNew cItemSeparate;
	g_pItemSeparate->_Set_NpcID(dwNpcID);
	g_pItemSeparate->Init( wt, nWTValue1, eSepType, pItemInfo, nItemMaxCnt, sMoneyPerItem, disp );
}

bool cItemSeparate::UpdateInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pItemSeparate )
	{
		if( ( g_pItemSeparate->GetTargetWindowType() == wt )&&( g_pItemSeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			g_pItemSeparate->Update(g_fDeltaTime);
			if( g_pItemSeparate->IsShowWindow() == false )
			{
				DeleteInstance();
				return false;
			}
			return true;
		}		
	}
	return false;
}

cBaseWindow::eMU_TYPE
cItemSeparate::Update_ForMouseInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	cBaseWindow::eMU_TYPE muReturn = MUT_OUT_WINDOW;
	if( g_pItemSeparate )
	{		
		if( ( g_pItemSeparate->GetTargetWindowType() == wt )&&( g_pItemSeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			muReturn = g_pItemSeparate->Update_ForMouse();
		}

		if( g_pItemSeparate )
		{
			if( g_pItemSeparate->IsShowWindow() == false )
			{
				DeleteInstance();			
			}
		}
		else
		{
			return MUT_OUT_WINDOW;
		}
	}
	return muReturn;
}

void cItemSeparate::RenderInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pItemSeparate )
	{
		if( ( g_pItemSeparate->GetTargetWindowType() == wt )&&( g_pItemSeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			g_pEngine->ScreenSpace();
			g_pItemSeparate->Render();
		}		
	}
}

bool cItemSeparate::DeleteInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pItemSeparate )
	{
		if( ( g_pItemSeparate->GetTargetWindowType() == wt )&&( g_pItemSeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			SAFE_NIDELETE( g_pItemSeparate );
			return true;
		}		
	}
	return false;
}

bool cItemSeparate::DeleteInstance()
{
	if( g_pItemSeparate )
	{
		SAFE_NIDELETE( g_pItemSeparate );
		return true;
	}
	return false;
}

bool cItemSeparate::IsInstance( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1 )
{
	if( g_pItemSeparate )
	{
		if( ( g_pItemSeparate->GetTargetWindowType() == wt )&&( g_pItemSeparate->GetTargetWTValue1() == nWTValue1 ) )
		{
			return true;
		}		
	}
	return false;
}

cItemSeparate::cItemSeparate()
{
	m_hEditBrush = NULL;
	m_hEditFont = NULL;

	m_hEditCnt = NULL;
	m_hEditTera = NULL;
	m_hEditMega = NULL;
	m_hEditBit = NULL;

	m_EditPropertyA = NULL;
	m_EditPropertyB = NULL;
	m_EditPropertyC = NULL;
	m_EditPropertyD = NULL;

	m_pMoneyIF = NULL;
}

void cItemSeparate::Destroy()
{
	if( g_pGameIF->IsActiveWindow( m_eTargetWindowType ) == true )
	{
		switch( m_eTargetWindowType )
		{
		case cBaseWindow::WT_PERSONSTORE:
			g_pGameIF->GetDynamicIF( m_eTargetWindowType, 0, m_nTargetWTValue1 )->SetEnableWindow( true );
			break;
		case cBaseWindow::WT_STORE:
			break;
		default:
			assert_cs( false );
		}
	}
		

	cBaseWindow::Delete();
}

void cItemSeparate::DeleteResource()
{
	DeleteScript();

	m_strItemName.Delete();
	SAFE_NIDELETE( m_pMoneyIF );

	DeleteObject( m_hEditBrush );
	DeleteObject( m_hEditFont );

	::DestroyWindow( m_hEditCnt );
	if( m_hEditTera )
		::DestroyWindow( m_hEditTera );
	if( m_hEditMega )
		::DestroyWindow( m_hEditMega );
	if( m_hEditBit )
		::DestroyWindow( m_hEditBit );	
	if( m_EditPropertyA )
		::DestroyWindow( m_EditPropertyA );
	if( m_EditPropertyB )
		::DestroyWindow( m_EditPropertyB );
	if( m_EditPropertyC )
		::DestroyWindow( m_EditPropertyC );
	if( m_EditPropertyD )
		::DestroyWindow( m_EditPropertyD );

	SAFE_NIDELETE(m_sDiscountInfo.s_pText);

	// [3/30/2016 hyun]
	for(int i = 0; i < m_vecEditBox.size(); ++i)
		SAFE_NIDELETE(m_vecEditBox[i]);

	g_IME.SetNumberOnly(false);
}

void cItemSeparate::Init( cBaseWindow::eWINDOW_TYPE wt, int nWTValue1, eTYPE eSepType, cItemInfo* pItemInfo, int nItemMaxCnt, sMONEY sMoneyPerItem, cMoneyIF::eDISP_TYPE disp, DWORD dwPriceItemCode )
{
	if( g_pGameIF->IsActiveWindow( wt ) == true )
		g_pGameIF->GetDynamicIF( wt, 0, nWTValue1 )->SetEnableWindow( false );

	if( cBaseWindow::Init() == false )
		return;

	m_ItemInfo = *pItemInfo;
	if( m_ItemInfo.GetCount() > (uint)nItemMaxCnt )
		m_ItemInfo.m_nCount = nItemMaxCnt;

	m_eDispType = disp;
	m_eSepType= eSepType;
	m_MoneyPerItem = sMoneyPerItem;
	
	switch( eSepType )
	{
	case BUY:
	case BUY_RES:	SetMaxBuyCount( nItemMaxCnt );		break;
	case SELL:
	case SELL_RES:	SetMaxSelCount( nItemMaxCnt );		break;
	}

	m_EndTime = pItemInfo->m_nEndTime;//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820

	cText::sTEXTINFO ti;
	ti.Init();
	ti.s_eTextAlign = DT_CENTER;
	ti.SetText( _T( "" ) );

	CsPoint ptSize = CsPoint( 322, 227 );
	CsPoint SepPos = CsPoint( 300, 180 );
	InitScript( "Separate\\Item_Win.tga", SepPos, ptSize, false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );

 	
// [4/5/2016 hyun] 추가
	CreateEditBox();

	m_pBtnCancel = AddButton( CsPoint( 300, 7 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pBtnOk = AddButton( CsPoint( 239, 194 ), CsPoint( 75, 27 ), cButton::IMAGE_NOR_13, &ti );

	m_pCntDecrease = AddButton( CsPoint( 34, 156 ), CsPoint( 10, 21 ), CsPoint( 0, 21 ), "Separate\\Seper_Lbar.tga" );	
	m_pCntIncrease = AddButton( CsPoint( 91 ,156 ), CsPoint( 10, 21 ), CsPoint( 0, 21 ), "Separate\\Seper_Rbar.tga" );	
	m_pCntDecreaseEnd = AddButton( CsPoint( 15, 156 ), CsPoint( 15, 21 ), CsPoint( 0, 21 ), "Separate\\Seper_LEbar.tga" );			
	m_pCntIncreaseEnd = AddButton( CsPoint( 105 ,156 ), CsPoint( 15, 21 ), CsPoint( 0, 21 ), "Separate\\Seper_REbar.tga" );	

	//돈 에디트박스 경로
	char szEditBox[64];
	strcpy_s( szEditBox, 64, "Separate\\Item_Win_MoneyEdit.tga" );

	switch( m_eSepType )
	{
	case SELL_RES:
		{
			AddTitle( UISTRING_TEXT( "ITEMSEPARATE_SALES_REGIST" ).c_str() );

			ti.SetText( UISTRING_TEXT( "ITEMSEPARATE_SALES_QUANTITY" ).c_str() );
			AddText( &ti, IF_ITEM_SEP_ITEM_COUNT_POS );

			ti.SetText( UISTRING_TEXT( "ITEMSEPARATE_SALES_PRICE_PER_UNIT" ).c_str() );
			AddText( &ti, IF_ITEM_SEP_ITEM_PRICE_POS );

			AddSprite( IF_ITEM_SEP_MONEY_EDITBOX_POS, IF_ITEM_SEP_MONEY_EDITBOX_SIZE, szEditBox );			

			m_pBtnOk->SetText( UISTRING_TEXT( "ITEMSEPARATE_SALES" ).c_str() );

			_SetCount( m_ItemInfo.GetCount(), true );
		}		
		break;
	case BUY_RES:
		{
			AddTitle( UISTRING_TEXT( "ITEMSEPARATE_PURCHASE_REGIST" ).c_str() );

			ti.SetText( UISTRING_TEXT( "ITEMSEPARATE_PURCHASE_QUANTITY" ).c_str() );
			AddText( &ti, IF_ITEM_SEP_ITEM_COUNT_POS );

			ti.SetText( UISTRING_TEXT( "ITEMSEPARATE_PURCHASE_PRICE_PER_UNIT" ).c_str() );
			AddText( &ti, IF_ITEM_SEP_ITEM_PRICE_POS );

			AddSprite( IF_ITEM_SEP_MONEY_EDITBOX_POS, IF_ITEM_SEP_MONEY_EDITBOX_SIZE, szEditBox );

			m_pBtnOk->SetText( UISTRING_TEXT( "ITEMSEPARATE_PURCHASE" ).c_str() );

			_SetCount( m_ItemInfo.GetCount(), true );
		}		
		break;
	case SELL:
		{
			AddTitle( UISTRING_TEXT( "ITEMSEPARATE_SALES" ).c_str() );

			ti.SetText( UISTRING_TEXT( "ITEMSEPARATE_SALES_QUANTITY" ).c_str() );
			AddText( &ti, IF_ITEM_SEP_ITEM_COUNT_POS );

			ti.SetText( UISTRING_TEXT( "ITEMSEPARATE_TOTAL_SALES_PRICE" ).c_str() );
			AddText( &ti, IF_ITEM_SEP_ITEM_PRICE_POS );

			m_pBtnOk->SetText( UISTRING_TEXT( "ITEMSEPARATE_SALES" ).c_str() );

			_SetCount( m_ItemInfo.GetCount(), true );

			m_pMoneyIF = NiNew cMoneyIF;
			m_pMoneyIF->Init( m_eDispType, m_MoneyPerItem.GetInt()*1, true );
			::SetFocus( m_hEditCnt );
		}		
		break;
	case BUY:
		{
			AddTitle( UISTRING_TEXT( "ITEMSEPARATE_PURCHASE" ).c_str() );

			ti.SetText( UISTRING_TEXT( "ITEMSEPARATE_PURCHASE_QUANTITY" ).c_str() );
			AddText( &ti, IF_ITEM_SEP_ITEM_COUNT_POS );

			ti.SetText( UISTRING_TEXT( "ITEMSEPARATE_TOTAL_PURCHASE_PRICE" ).c_str() );
			AddText( &ti, IF_ITEM_SEP_ITEM_PRICE_POS );

			m_pBtnOk->SetText( UISTRING_TEXT( "ITEMSEPARATE_PURCHASE" ).c_str() );

			_SetCount( m_ItemInfo.GetCount(), true );

			m_pMoneyIF = NiNew cMoneyIF;
			if( m_eDispType == cMoneyIF::DISP_PROPERTY )
				m_pMoneyIF->Init( m_eDispType, m_MoneyPerItem, &m_ItemInfo, m_dwNpcID, 0, 0, true );
			else
			{
				if( 0 != dwPriceItemCode )
					m_pMoneyIF->Init_ForEventStore( dwPriceItemCode, m_eDispType, m_MoneyPerItem.GetInt()*1, true );
				else
					m_pMoneyIF->Init( m_eDispType, m_MoneyPerItem.GetInt()*1, true );
			}
		}
		break;
	default:
		assert_cs( false );
	}

	CsItem::sINFO* pFTItemInfo = nsCsFileTable::g_pItemMng->GetItem( m_ItemInfo.m_nType )->GetInfo();
	ti.s_eFontSize = CFont::FS_12;
	ti.s_eTextAlign = DT_LEFT;
	ti.s_Color = g_pDataMng->GetItemColor( pFTItemInfo );
	g_pStringAnalysis->Cut( &m_strItemName, 230, pFTItemInfo->s_szName, &ti );
	
	SetMoneyInEditBox( m_MoneyPerItem );
	
	m_eTargetWindowType = wt;
	m_nTargetWTValue1 = nWTValue1;	

	m_sDiscountInfo.s_FontSize = CFont::FS_10;
	m_sDiscountInfo.s_Font.Init(  &g_pEngine->m_FontSystem, m_sDiscountInfo.s_FontSize );	
	m_sDiscountInfo.s_Font.SetBoldLevel( cText::sTEXTINFO::BL_2 );
	m_sDiscountInfo.s_pText = NULL;
}

bool cItemSeparate::OnChangeEdit( DWORD nCtrlID )
{
	HWND hWnd = NULL;
	switch( nCtrlID )
	{
	case IF_ITEM_SEP_TERA_EDIT_ID:		hWnd = m_hEditTera;		break;
	case IF_ITEM_SEP_MEGA_EDIT_ID:		hWnd = m_hEditMega;		break;
	case IF_ITEM_SEP_BIT_EDIT_ID:		hWnd = m_hEditBit;		break;
	case IF_ITEM_SEP_COUNT_EDIT_ID:		hWnd = m_hEditCnt;		break;
	if( m_eDispType == cMoneyIF::DISP_PROPERTY )
	{
		case IF_PROPERTY_A_EDIT_ID:			hWnd = m_EditPropertyA;		break;
		case IF_PROPERTY_B_EDIT_ID:			hWnd = m_EditPropertyB;		break;
		case IF_PROPERTY_C_EDIT_ID:			hWnd = m_EditPropertyC;		break;
		case IF_PROPERTY_D_EDIT_ID:			hWnd = m_EditPropertyD;		break;
	}
	default:							return false;
	}

	if( hWnd == NULL )
		return false;

	TCHAR sz[ 16 ];
	GetWindowText( hWnd, sz, 16 );

	if( hWnd == m_hEditCnt )
	{
		int nCurCnt = _ttoi( sz );
		if( m_EndTime > 0 )//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
		{
			SetWindowText( hWnd, _T( "1" ) );
			_SetCount( 1, false );
		}	
		else if( nCurCnt > m_nItemMaxCount  )
		{
			_SetCount( m_nItemMaxCount, true );
		}
		else
		{
			_SetCount( nCurCnt, false );
		}
	}
	else if( _ttoi( sz ) == 0 )
	{
		SetWindowText( hWnd, _T( "" ) );
	}

	return true;
}

void cItemSeparate::_SetCount( int nCount, bool bSetEditControl )
{
	if( nCount > m_nItemMaxCount )
		nCount = m_nItemMaxCount;
// 	else if( nCount < 1 ) // [4/21/2016 hyun] 주석. 
// 		nCount = 1;

	//디지오라 겹쳐서 올릴 수 있는 현상 수정 chu8820
	m_pCntIncrease->SetEnable( ( nCount < m_nItemMaxCount ) && ( m_EndTime <= 0 ) );
	m_pCntIncreaseEnd->SetEnable( ( nCount < m_nItemMaxCount ) && ( m_EndTime <= 0 ) );
	m_pCntDecrease->SetEnable( nCount > 1 );
	m_pCntDecreaseEnd->SetEnable( nCount > 1 );

	if( bSetEditControl ) // 버튼 클릭으로 바꼈다면
	{
		TCHAR sz[ 32 ];
		if(nCount == 0)
			_stprintf_s( sz, 32, _T(""));
		else
			_stprintf_s( sz, 32, _T( "%d" ), nCount );

		// [4/5/2016 hyun] 추가
		m_vecEditBox[EDITBOX_ITEMCOUNT]->SetText(sz);
	}

	switch( m_eSepType )
	{
	case SELL_RES:
	case BUY_RES:
		break;
	case SELL:
	case BUY:
		{
			if(m_eDispType == cMoneyIF::DISP_PROPERTY )
			{
				sMONEY TempProperty;
				TempProperty.SetProperty_A(m_MoneyPerItem.GetProperty_A() * nCount);
				TempProperty.SetProperty_B(m_MoneyPerItem.GetProperty_B() * nCount);
				TempProperty.SetProperty_C(m_MoneyPerItem.GetProperty_C() * nCount);
				TempProperty.SetProperty_D(m_MoneyPerItem.GetProperty_D() * nCount);
				SetMoneyInEditBox(TempProperty);
			}
			else
				SetMoneyInEditBox(m_MoneyPerItem.GetInt()*nCount);
		}
		break;
	default:
		assert_cs( false );
	}
}

int cItemSeparate::_GetCount()
{
	return _ttoi(m_vecEditBox[EDITBOX_ITEMCOUNT]->GetString());
}

void cItemSeparate::_SetMoney( sMONEY money )
{
	if(m_eDispType != cMoneyIF::DISP_PROPERTY )
	{
		if( m_hEditTera )
		{
			assert_cs( m_hEditMega );
			assert_cs( m_hEditBit );

			TCHAR sz[ 32 ];
			if( money.GetTera() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetTera() );
				SetWindowText( m_hEditTera, sz );
				SendMessage( m_hEditTera, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_hEditTera, _T( "" ) );
			}
			if( money.GetMega() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetMega() );
				SetWindowText( m_hEditMega, sz );
				SendMessage( m_hEditMega, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_hEditMega, _T( "" ) );
			}
			if( money.GetBit() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetBit() );
				SetWindowText( m_hEditBit, sz );
				SendMessage( m_hEditBit, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_hEditBit, _T( "" ) );
			}

			m_MoneyPerItem = money;
		}
		else if( m_pMoneyIF )
		{
			m_pMoneyIF->SetMoney( money );
		}	
	}
	else
	{
		if( m_EditPropertyA )
		{
			assert_cs( m_EditPropertyB );
			assert_cs( m_EditPropertyC );
			assert_cs( m_EditPropertyD );

			TCHAR sz[ 32 ];
			if( money.GetProperty_A() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetProperty_A() );
				SetWindowText( m_EditPropertyA, sz );
				SendMessage( m_EditPropertyA, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_EditPropertyA, _T( "" ) );
			}
			if( money.GetProperty_B() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetProperty_B() );
				SetWindowText( m_EditPropertyB, sz );
				SendMessage( m_EditPropertyB, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_EditPropertyB, _T( "" ) );
			}
			if( money.GetProperty_C() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetProperty_C() );
				SetWindowText( m_EditPropertyC, sz );
				SendMessage( m_EditPropertyC, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_EditPropertyC, _T( "" ) );
			}
			if( money.GetProperty_D() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetProperty_D() );
				SetWindowText( m_EditPropertyD, sz );
				SendMessage( m_EditPropertyD, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_EditPropertyD, _T( "" ) );
			}

			m_MoneyPerItem = money;
		}
		else if( m_pMoneyIF )
		{
			m_pMoneyIF->SetMoney( money, m_ItemInfo );
		}	
	}
}

sMONEY cItemSeparate::_GetMoney()
{
	sMONEY money = (u8)0;

	if( m_hEditTera )
	{
		assert_cs( m_hEditMega );
		assert_cs( m_hEditBit );

		TCHAR sz[ 32 ];
		GetWindowText( m_hEditTera, sz, 32 );
		money.IncreaseTera( _ttoi( sz ) );

		GetWindowText( m_hEditMega, sz, 32 );
		money.IncreaseMega( _ttoi( sz ) );

		GetWindowText( m_hEditBit, sz, 32 );
		money.IncreaseBit( _ttoi( sz ) );
	}

	if( m_eDispType == cMoneyIF::DISP_PROPERTY )
	{
		if( m_EditPropertyA )	//속성상점 돈 가져온다
		{
			assert_cs( m_EditPropertyB );
			assert_cs( m_EditPropertyC );
			assert_cs( m_EditPropertyD );

			TCHAR sz[ 32 ];
			GetWindowText( m_EditPropertyA, sz, 32 );
			money.IncreaseProperty_A( _ttoi( sz ) );

			GetWindowText( m_EditPropertyB, sz, 32 );
			money.IncreaseProperty_B( _ttoi( sz ) );

			GetWindowText( m_EditPropertyC, sz, 32 );
			money.IncreaseProperty_C( _ttoi( sz ) );

			GetWindowText( m_EditPropertyD, sz, 32 );
			money.IncreaseProperty_D( _ttoi( sz ) );
		}
	}
	return money;
}

void cItemSeparate::Update(float const& fDeltaTime)
{
	if( IsShowWindow() == false )
		return;
}


void cItemSeparate::OnOk()
{
	bool bCheckZeroMoney = false;
	bool bCheckZeroCount = true;

	switch( m_eSepType )
	{
	case SELL_RES:
	case BUY_RES:
		bCheckZeroMoney = true;
		break;

	case SELL:
	case BUY:
		break;
	default:
		assert_cs( false );
	}

	if( bCheckZeroMoney == true )
	{
		if(GetMoneyInEditBox() == 0)
		{
			cPrintMsg::PrintMsg( 30363 );
			return;
		}
	}

	if( bCheckZeroCount == true )
	{
		if( _GetCount() == 0 )
		{
			cPrintMsg::PrintMsg( 30364 );
			return;
		}
	}

	m_ItemInfo.m_nCount = _GetCount();

	switch( m_eTargetWindowType )
	{
	case cBaseWindow::WT_PERSONSTORE:
		_OnOk_PersonStore();
		break;
	case cBaseWindow::WT_STORE:
		_OnOk_Store();
		break;
	default:
		assert_cs( false );
	}

	Close();
}

void cItemSeparate::_OnOk_PersonStore()
{
	switch( m_eSepType )
	{
	case SELL_RES:
		// 판매 아이템 등록
		{
			GS2C_RECV_SELL_PERSONITEM recv;
			recv.ItemData = m_ItemInfo;
			recv.MoneyPerItem = GetMoneyInEditBox().GetInt();
			GAME_EVENT_ST.OnEvent( EVENT_CODE::PERSON_ITEM_SELL_ADD, &recv );
		}		
		break;
	case BUY_RES:
		// 구매 아이템 등록
		{
		}		
		break;

	case BUY:
		{
			GS2C_RECV_BUY_PERSONITEM recv;
			recv.buyItemData = m_ItemInfo;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::PERSON_ITEM_BUY, &recv );
		}		
		break;

	case SELL:
		break;

	default:
		assert_cs( false );
	}
}

void cItemSeparate::_OnOk_Store()
{
	switch( m_eSepType )
	{
	case BUY:
		{
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::SEND_STORE_ITEMBUY, &m_ItemInfo );
		}break;

	/*case SELL:
		break;*/

	default:
		assert_cs( false );
	}
}

void cItemSeparate::OnCancel()
{
	if( g_pGameIF->IsActiveWindow( m_eTargetWindowType ) == true )
	{
		switch( m_eTargetWindowType )
		{
		case cBaseWindow::WT_PERSONSTORE:			
			break;
		case cBaseWindow::WT_STORE:
			g_pGameIF->GetDynamicIF( m_eTargetWindowType, 0, m_nTargetWTValue1 )->SetEnableWindow( true );
			break;
		default:
			assert_cs( false );
		}
	}

	Close();
}

cBaseWindow::eMU_TYPE
cItemSeparate::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pBtnOk->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		OnOk();
		return muReturn;
	}

	if( m_pBtnCancel->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		OnCancel();
		return muReturn;
	}

	if( m_pCntDecrease->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		_SetCount( _GetCount() - 1, true );
	}
	if( m_pCntIncrease->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		_SetCount( _GetCount() + 1, true );
	}
	if( m_pCntDecreaseEnd->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		_SetCount( 1, true );
	}
	if( m_pCntIncreaseEnd->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		_SetCount( m_nItemMaxCount, true );
	}

	for(int i = 0; i < m_vecEditBox.size(); ++ i)
		m_vecEditBox[i]->Update_ForMouse(CsPoint());
	
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );

	CsRect rcItem( CsPoint( 18, 56 ), CsSIZE( 32, 32 ) );
	if( rcItem.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		cItemInfo ItemInfo = m_ItemInfo;
		ItemInfo.m_nCount = _GetCount();

		CsPoint pos = rcItem.GetPos() + GetRootClient();		
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, rcItem.GetSize(), TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_ItemInfo.m_nType, cBaseWindow::WT_ITEM_SEPARATE, 0, 0, &ItemInfo );
		return muReturn;
	}

	return muReturn;
}

void cItemSeparate::Render()
{
	if( IsShowWindow() == false )
		return;

	RenderScript();
	g_pIconMng->RenderItem( &m_ItemInfo, GetRootClient() + CsPoint( 18, 56 ), false );
	m_strItemName.Render( GetRootClient() + CsPoint( 74, 49 ), 4 );
	if( m_pMoneyIF != NULL )
	{
		if( m_eDispType != cMoneyIF::DISP_PROPERTY )
			m_pMoneyIF->Render( CsPoint( 154, 156 ) + GetRootClient() );	//일반상점 구매시 아이콘 & 돈 랜더
		else
			m_pMoneyIF->Render( CsPoint( 172, 156 ) + GetRootClient() );	//속성상점 구매시 아이콘 & 돈 랜더
	}

	for(int i = 0; i < m_vecEditBox.size(); ++i)
		m_vecEditBox[i]->Render();

	if( m_sDiscountInfo.s_pText ) 
		m_sDiscountInfo.s_pText->Render();
}

void cItemSeparate::ResetDevice()
{
	if( IsShowWindow() == false )
		return;

	ResetDeviceScript();

	m_strItemName.ResetDevice();

	if( m_pMoneyIF )
		m_pMoneyIF->ResetDevice();
}

LRESULT cItemSeparate::ApplyEditColor( HWND hEdit, HDC dc )
{
	if( ( hEdit != m_hEditTera )&&
		( hEdit != m_hEditMega )&&
		( hEdit != m_hEditBit )&&
		( hEdit != m_hEditCnt )	&&
		( hEdit != m_EditPropertyA )&&
		( hEdit != m_EditPropertyB )&&
		( hEdit != m_EditPropertyC )&&
		( hEdit != m_EditPropertyD )
		)	
	{
		return NULL;
	}

	SetTextColor( dc, RGB( 255, 255, 255 ) );
	SetBkColor( dc, IF_ITEM_SEP_EDITBOX_RGB );
	return (LRESULT)m_hEditBrush;
}

void cItemSeparate::Passive_UpdateDiscount( sMONEY sRefreshCost, int nMaxCount )
{
	m_MoneyPerItem = sRefreshCost;	// 낱개 가격 1개로 변경
	SetMaxBuyCount( nMaxCount );	// 최대 구매 개수 변경
	_SetCount(1, true);			// 낱개 가격에 따른 최대 개수 및 금액 변경, 에디트 박스 문자열 변경 true
}

void cItemSeparate::SetDiscountText( TCHAR* szName, int nValue )
{
	SAFE_NIDELETE(m_sDiscountInfo.s_pText);

	if( !_IsEnableDiscountStore() )
		return ;

	if( nValue <= 0 )		// 할인율이 0이하일 때 text 필요 없음
		return;

	std::wstring wsDC = UISTRING_TEXT( "ITEMSEPARATE_STORE_DISCOUNT_APPLY" );
	DmCS::StringFn::Replace( wsDC, L"#Name#", szName );
	DmCS::StringFn::Replace( wsDC, L"#Discount#", nValue );

	m_sDiscountInfo.s_pText = NiNew cText;
	m_sDiscountInfo.s_Font.SetText( wsDC.c_str() );
	m_sDiscountInfo.s_pText->Init( m_pRoot, CsPoint(10,201) , &m_sDiscountInfo.s_Font, false);
}




void cItemSeparate::InitPassiveText()
{
	if( !_IsEnableDiscountStore() )
		return;

	CDigimonUser* pkDigimon = g_pCharMng->GetDigimonUser(0);
	CsPassive_Ability_Digimon* pkAbility = nsCsFileTable::g_pDigimonMng->GetPassiveAbility(pkDigimon->GetBaseDigimonFTID(), nsCsDigimonTable::PA_NPC_DISCOUNT);
	
	if( !pkAbility )
		return;

	CDigimonEvolveInfo* pFTEvolInfo = pkDigimon->GetFTEvol();	// 파트너몬의 진화체 정보
	CDigimonEvolveObj::MAP mapEvol = pFTEvolInfo->m_Map;

	CDigimonEvolveObj::MAP_IT iter = mapEvol.begin();	// 성장기부터 진화 정보를 모두 검사
	CDigimonEvolveObj::MAP_IT iterEnd = mapEvol.end();


	for( ; iter != iterEnd ; ++iter)
	{
		const CDigimonEvolveObj* pkEvolObj = (iter->second);

		// 순회 중인 슬롯 번호 >= 조건 슬롯
		if( pkEvolObj->m_nEvoSlot >= pkAbility->GetInfo()->s_shEvoNum )
		{
			// 오픈 == 레벨, 에보류터 모두 일치 시
			if( pkDigimon->GetEvoValue( pkEvolObj->m_nEvoSlot, CDigimonUser::eActive ) )
			{
				CsDigimon* pkBaseDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon(pkDigimon->GetBaseDigimonFTID());
				
				if( pkBaseDigimon )
				{
					// 현재 디지몬의 진화단계와 관계 없이 진화 트리의 베이스 디지몬의 이름으로 설정합니다.
					SetDiscountText(pkBaseDigimon->GetInfo()->s_szName, pkAbility->GetInfo()->s_nValue1 );
					return;
				}				
				
			}
		}

	}

}

bool cItemSeparate::_IsEnableDiscountStore() const
{
	// 구매 화폐가 bit가 아니면 리턴
	if( m_eDispType != cMoneyIF::DISP_FULL )
		return false;

	// 상점 이용 목적이 구매가 아니면 리턴
	if( m_eSepType != BUY )
		return false;

	// 상점UI가 열려있지 않으면 리턴
	if( !g_pGameIF->IsActiveWindow(cBaseWindow::WT_STORE, 0) )
		return false;

	return true;
}


void cItemSeparate::CreateEditBox()
{
	bool bMoneyEdit = IsEditMoney(m_eSepType); // 돈이 입력 가능한 확인창인가?

	CsPoint pos = WINDOW_ITEMSEPARATE_POS; // 베이스 윈도우 pos

	cText::sTEXTINFO ti;
	cEditBox* pEditBox = NULL;

	ti.SetText(_T(""));
	ti.Init( &g_pEngine->m_FontText, CFont::FS_9 );
	ti.s_eTextAlign = DT_CENTER;
	ti.s_bOutLine = false;
	pEditBox = NiNew cEditBox;
	pEditBox->Init( m_pRoot, EDITBOX_ITEMCOUNT_POS, EDITBOX_ITEMCOUNT_SIZE, &ti, false );
	pEditBox->SetFontLength(4);
	pEditBox->EnableUnderline();
	pEditBox->AddEvent(cEditBox::eEditbox_ChangeText, this, &cItemSeparate::ChangeInputItemCount);
	pEditBox->SetLimitNumber(m_nItemMaxCount); 
	m_vecEditBox.push_back(pEditBox);
	
	if( bMoneyEdit == true )
	{
		ti.SetText(_T(""));
		ti.Init( &g_pEngine->m_FontText, CFont::FS_9 );
		ti.s_eTextAlign = DT_RIGHT;
		ti.s_bOutLine = false;

		pEditBox = NiNew cEditBox;
		pEditBox->Init( m_pRoot, EDITBOX_MONEY_TERA_POS, EDITBOX_MONEY_TERA_SIZE, &ti, false );
		pEditBox->SetFontLength( 5 );
		pEditBox->EnableUnderline();
		m_vecEditBox.push_back(pEditBox);

		pEditBox = NiNew cEditBox;
		pEditBox->Init( m_pRoot, EDITBOX_MONEY_MEGA_POS, EDITBOX_MONEY_MEGA_SIZE, &ti, false );
		pEditBox->SetFontLength( 3 );
		pEditBox->EnableUnderline();
		m_vecEditBox.push_back(pEditBox);

		pEditBox = NiNew cEditBox;
		pEditBox->Init( m_pRoot, EDITBOX_MONEY_BIT_POS, EDITBOX_MONEY_BIT_SIZE, &ti, false );
		pEditBox->SetFontLength( 3 );
		pEditBox->EnableUnderline();
		m_vecEditBox.push_back(pEditBox);
	}

	// 에디트 박스 포커스 설정
	g_IME.SetLockConidateList( true );
	g_IME.SetNumberOnly( true );

	m_vecEditBox[EDITBOX_ITEMCOUNT]->SetFocus();

	if( bMoneyEdit == true )
	{
		if( m_eDispType == cMoneyIF::DISP_PROPERTY )
		{
			m_EditPropertyA = CreateWindow( _T( "edit" ), NULL, WS_CHILD | ES_NUMBER | ES_RIGHT, 0, 0, 0, 0, GAMEAPP_ST.GetHWnd(), (HMENU)IF_PROPERTY_A_EDIT_ID, GAMEAPP_ST.GetHInstance(), NULL );
			SendMessage( m_EditPropertyA, EM_LIMITTEXT, (WPARAM)(5), 0 );
			MoveWindow( m_EditPropertyA, pos.x + 144, pos.y + 162, 44, 13, FALSE );
			ShowWindow( m_EditPropertyA, SW_SHOW );
			m_EditPropertyB = CreateWindow( _T( "edit" ), NULL, WS_CHILD | ES_NUMBER | ES_RIGHT, 0, 0, 0, 0, GAMEAPP_ST.GetHWnd(), (HMENU)IF_PROPERTY_B_EDIT_ID, GAMEAPP_ST.GetHInstance(), NULL );
			SendMessage( m_EditPropertyB, EM_LIMITTEXT, (WPARAM)(3), 0 );
			MoveWindow( m_EditPropertyB, pos.x + 213, pos.y + 162, 26, 13, FALSE );
			ShowWindow( m_EditPropertyB, SW_SHOW );
			m_EditPropertyC = CreateWindow( _T( "edit" ), NULL, WS_CHILD | ES_NUMBER | ES_RIGHT, 0, 0, 0, 0, GAMEAPP_ST.GetHWnd(), (HMENU)IF_PROPERTY_C_EDIT_ID, GAMEAPP_ST.GetHInstance(), NULL );
			SendMessage( m_EditPropertyC, EM_LIMITTEXT, (WPARAM)(3), 0 );
			MoveWindow( m_EditPropertyC, pos.x + 264, pos.y + 162, 26, 13, FALSE );
			ShowWindow( m_EditPropertyC, SW_SHOW );
			m_EditPropertyD = CreateWindow( _T( "edit" ), NULL, WS_CHILD | ES_NUMBER | ES_RIGHT, 0, 0, 0, 0, GAMEAPP_ST.GetHWnd(), (HMENU)IF_PROPERTY_D_EDIT_ID, GAMEAPP_ST.GetHInstance(), NULL );
			SendMessage( m_EditPropertyD, EM_LIMITTEXT, (WPARAM)(3), 0 );
			MoveWindow( m_EditPropertyD, pos.x + 315, pos.y + 162, 26, 13, FALSE );
			ShowWindow( m_EditPropertyD, SW_SHOW );
		}
	}	

	if( bMoneyEdit == true )
	{
		if( m_eDispType == cMoneyIF::DISP_PROPERTY )
		{
			EditProc_ItemSeparate=(WNDPROC)SetWindowLong(m_EditPropertyA,GWL_WNDPROC,(LONG)EditSubProc_ItemSeparate);
			EditProc_ItemSeparate=(WNDPROC)SetWindowLong(m_EditPropertyB,GWL_WNDPROC,(LONG)EditSubProc_ItemSeparate);
			EditProc_ItemSeparate=(WNDPROC)SetWindowLong(m_EditPropertyC,GWL_WNDPROC,(LONG)EditSubProc_ItemSeparate);
			EditProc_ItemSeparate=(WNDPROC)SetWindowLong(m_EditPropertyD,GWL_WNDPROC,(LONG)EditSubProc_ItemSeparate);
		}
	}	
	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );

	if( bMoneyEdit == false )
	{
		m_hEditTera = m_hEditMega = m_hEditBit = NULL;
		if( m_eDispType == cMoneyIF::DISP_PROPERTY )
			m_EditPropertyA = m_EditPropertyB = m_EditPropertyC = m_EditPropertyD = NULL;
	}
}

bool cItemSeparate::IsEditMoney( eTYPE eSeparateType )
{
	switch( eSeparateType )
	{
	case SELL_RES: // 판매 등록 확인창
	case BUY_RES: // 구매 등록 확인창
		return true; 
	case SELL:
	case BUY:
		return false;

	default:
		assert_cs( false );
	}
	return false;
}

void cItemSeparate::SetMoneyInEditBox( sMONEY money )
{
	if(m_eDispType != cMoneyIF::DISP_PROPERTY )
	{
		if(IsEditMoney(m_eSepType)) // 돈 입력이 가능한 창이라면
		{
			assert_cs( m_vecEditBox[EDITBOX_TERA] );
			assert_cs( m_vecEditBox[EDITBOX_MEGA] );
			assert_cs( m_vecEditBox[EDITBOX_BIT] );

			TCHAR sz[ 32 ];
			if( money.GetTera() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetTera() );
				m_vecEditBox[EDITBOX_TERA]->SetText(sz);
			}
			else
			{
				m_vecEditBox[EDITBOX_TERA]->SetText( _T( "" ));
			}

			if( money.GetMega() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetMega() );
				m_vecEditBox[EDITBOX_MEGA]->SetText(sz);
			}
			else
			{
				m_vecEditBox[EDITBOX_MEGA]->SetText( _T( "" ));
			}

			if( money.GetBit() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetBit() );
				m_vecEditBox[EDITBOX_BIT]->SetText(sz);
			}
			else
			{
				m_vecEditBox[EDITBOX_BIT]->SetText( _T( "" ));
			}

			m_MoneyPerItem = money;
		}
		else if( m_pMoneyIF )
		{
			m_pMoneyIF->SetMoney( money );
		}	
	}
	else // 속성 상점일때
	{
		if( m_EditPropertyA )
		{
			assert_cs( m_EditPropertyB );
			assert_cs( m_EditPropertyC );
			assert_cs( m_EditPropertyD );

			TCHAR sz[ 32 ];
			if( money.GetProperty_A() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetProperty_A() );
				SetWindowText( m_EditPropertyA, sz );
				SendMessage( m_EditPropertyA, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_EditPropertyA, _T( "" ) );
			}
			if( money.GetProperty_B() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetProperty_B() );
				SetWindowText( m_EditPropertyB, sz );
				SendMessage( m_EditPropertyB, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_EditPropertyB, _T( "" ) );
			}
			if( money.GetProperty_C() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetProperty_C() );
				SetWindowText( m_EditPropertyC, sz );
				SendMessage( m_EditPropertyC, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_EditPropertyC, _T( "" ) );
			}
			if( money.GetProperty_D() )
			{
				_stprintf_s( sz, 32, _T( "%d" ), money.GetProperty_D() );
				SetWindowText( m_EditPropertyD, sz );
				SendMessage( m_EditPropertyD, EM_SETSEL, -2, -1 );
			}
			else
			{
				SetWindowText( m_EditPropertyD, _T( "" ) );
			}

			m_MoneyPerItem = money;
		}
		else if( m_pMoneyIF )
		{
			m_pMoneyIF->SetMoney( money, m_ItemInfo );
		}	
	}
}

sMONEY cItemSeparate::GetMoneyInEditBox()
{
	sMONEY money = (u8)0;

	if( m_vecEditBox[EDITBOX_TERA] )
	{
		assert_cs( m_vecEditBox[EDITBOX_MEGA] );
		assert_cs( m_vecEditBox[EDITBOX_BIT] );

		money.IncreaseTera( _ttoi( m_vecEditBox[EDITBOX_TERA]->GetString() ) );
		money.IncreaseMega( _ttoi( m_vecEditBox[EDITBOX_MEGA]->GetString() ) );
		money.IncreaseBit( _ttoi( m_vecEditBox[EDITBOX_BIT]->GetString() ) );
	}

	if( m_eDispType == cMoneyIF::DISP_PROPERTY )
	{
		if( m_EditPropertyA )	//속성상점 돈 가져온다
		{
			assert_cs( m_EditPropertyB );
			assert_cs( m_EditPropertyC );
			assert_cs( m_EditPropertyD );

			TCHAR sz[ 32 ];
			GetWindowText( m_EditPropertyA, sz, 32 );
			money.IncreaseProperty_A( _ttoi( sz ) );

			GetWindowText( m_EditPropertyB, sz, 32 );
			money.IncreaseProperty_B( _ttoi( sz ) );

			GetWindowText( m_EditPropertyC, sz, 32 );
			money.IncreaseProperty_C( _ttoi( sz ) );

			GetWindowText( m_EditPropertyD, sz, 32 );
			money.IncreaseProperty_D( _ttoi( sz ) );
		}
	}
	return money;
}

// [4/8/2016 hyun] 아이템갯수 에디트박스에 문자가 추가입력 됐을 때 호출 됨
void cItemSeparate::ChangeInputItemCount( void* pSender, void* pData )
{
	SAFE_POINTER_RET( pData );

	TCHAR* pInputData = static_cast<TCHAR*>(pData);
	int InputItemCount = _ttoi( pInputData ); // 입력되 아이템 갯수

	_SetCount(InputItemCount, true); // 에디트박스의 수를 갱신하고 그에 따른 아이템 가격도 갱신
}

uint cItemSeparate::GetRegistItemTypeID() const
{
	return m_ItemInfo.m_nType;
}
// 구매할 때 구매할 수 있는 최대 갯수 셋팅
void cItemSeparate::SetMaxBuyCount( int nCount )
{
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET(pInven);

	int nMaxCropCount = pInven->GetCount_Item_EnableCrop( m_ItemInfo.m_nType );
	if(nCount > nMaxCropCount)
		nCount = nMaxCropCount;

	//2017-01-05-nova	개인상점 판매 물품수가 20000이 넘어가면 20000으로 고정(u4 m_nCount : 15 크기 제한이 걸려있어서)
	if(nCount > 20000)
	{
		m_nItemMaxCount = 20000;
	}
	else
	{
		m_nItemMaxCount = nCount;
	}	
}
// 판매할 때 판매할 수 있는 아이템의 최대 갯수 셋팅
void cItemSeparate::SetMaxSelCount( int nCount )
{
	cData_Inven* pInven = g_pDataMng->GetInven();
	SAFE_POINTER_RET(pInven);

	DWORD dwHaveItemCount = pInven->GetCount_Item_ID( m_ItemInfo.m_nType, true );
	if( nCount > dwHaveItemCount )
		nCount = dwHaveItemCount;

	//2017-01-05-nova	개인상점 판매 물품수가 20000이 넘어가면 20000으로 고정(u4 m_nCount : 15 크기 제한이 걸려있어서)
	if(nCount > 20000)
	{
		m_nItemMaxCount = 20000;
	}
	else
	{
		m_nItemMaxCount = nCount;
	}	
}


