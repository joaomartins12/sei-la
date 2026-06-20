

#include "stdafx.h"
#include "CashShop__Etc.h"

#ifdef FT_USE_CASHSHOP

cCashShop_Pay::cCashShop_Pay()
{		
	memset( m_szMessage, 0, sizeof( TCHAR ) * MAX_PATH );
}


void cCashShop_Pay::Destroy()
{
	cBaseWindow::Delete();
}

void cCashShop_Pay::DeleteResource()
{	
	//SAFE_NIDELETE( m_pEditName );
	//SAFE_NIDELETE( m_pEditMessage );
	SAFE_NIDELETE( m_pComboPay );
//	SAFE_NIDELETE( m_pPay );
	//SAFE_NIDELETE( m_pName );
	m_pStrList.Delete();

	DeleteScript();		
	m_pScrollBar = NULL;

	m_eType = PT_NONE;
	m_pItem = NULL;

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_CASHSHOP ) )
		g_pGameIF->GetCashShop()->SetEnableWindow( true );
}

void cCashShop_Pay::Create( int nValue /*=0*/ )
{
	m_eType = PT_NONE;
	//m_pEditName = NULL;
	//m_pEditMessage = NULL;
	m_pComboPay = NULL;
//	m_pPay = NULL;
	m_pScrollBar = NULL;
	m_pItem = NULL;	
	//m_pName = NULL;

	// 인터페이스 위치정보
	CsPoint pt; 
	CsSIZE size( 32, 32 );
	for( int i=0; i<3; ++i )
	{
		pt.x = 32;
		pt.y = i * 108 + 49;
		m_BasketIFIcon[ i ].Init( CsRect( pt, size ) );		
	}	
}

void cCashShop_Pay::Update(float const& fDeltaTime)
{
	if( m_eType == PT_NONE )
		return;

	switch( m_eType )
	{
	case PT_ONE:
		{
			if( m_pComboPay )
				m_pComboPay->Update(fDeltaTime);				
		}
		break;
	case PT_PRESENT:
		{
			assert_csm( false, L"아직 사용되지 아니함" );
			if( m_pComboPay )
				m_pComboPay->Update(fDeltaTime);		

			//if( m_pEditMessage->IsFocus() == true )
			//{		
			//	if( _tcscmp( m_szMessage, (TCHAR*)m_pEditMessage->GetTextAll() ) != 0 ) 
			//	{
			//		_stprintf_s( m_szMessage, MAX_PATH, _T( "%s" ), (TCHAR*)m_pEditMessage->GetTextAll() );	

			//		m_pStrList.Delete();

			//		cText::sTEXTINFO ti;		
			//		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
			//		g_pStringAnalysis->Cut( &m_pStrList, 282, m_szMessage, &ti );
			//		m_pStrList.SetUnderLineCursor(); 					
			//	}						
			//}	
			//else
			//	m_pStrList.ReleaseCursor();												
		}
		break;
	default:
		assert_cs( false );
		break;
	}
}

bool cCashShop_Pay::OnEnterKey()
{
	if( m_eType == PT_PRESENT )
	{
		//if( m_pEditName->IsFocus() == true )
		//{
		//	_tcscpy_s( g_pDataMng->GetCashShop()->m_szName, Language::pLength::name + 1, (TCHAR*)m_pEditName->GetText()->GetText() );
		//	m_pName->SetText( (TCHAR*)m_pEditName->GetText()->GetText() );
		//}

		//if( m_pEditMessage->IsFocus() == true )
		//{
		//	_tcscpy_s( g_pDataMng->GetCashShop()->m_szMessage, MAX_PATH, m_szMessage );
		//}

		if( g_pFocusEdit != NULL )
			g_pFocusEdit->ReleaseFocus();
	}

	return false;
}


cBaseWindow::eMU_TYPE
cCashShop_Pay::Update_ForMouse()
{	
	if( m_eType == PT_NONE )
		return MUT_OUT_WINDOW;

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	

	switch( m_eType )
	{
	case PT_ONE:
		{
			assert_cs( m_pItem != NULL );

			if(m_pComboPay)
			{
				if( m_pItem->s_vpElement.Size() != 1 )
					m_pComboPay->Update_ForMouse();
			}

/*			if( m_pComboPay != NULL )
			{
				if( m_pItem->s_vpElement.Size() != 1 )
				{
					int nComboSel = m_pComboPay->Update_ForMouse();
					if( nComboSel != cComboBox::CI_INVALIDE )
					{
						switch( nComboSel )
						{
						case cComboBox::CI_INRECT:			
							return MUT_NONE;
						case cComboBox::CI_CLICK:
						case cComboBox::CI_SAME_CLICK:
							return MUT_CLICK_FOCUS;
						default:
							{
								m_pComboPay->GetCurSel_Data();						
							}				
							return MUT_CLICK_FOCUS;
						}
					}
				}
			}
*/
			switch( m_pBuy->Update_ForMouse() )
			{
			case cButton::ACTION_NONE:
				break;
			case cButton::ACTION_CLICK:
				{
					DWORD dwID;
					if( m_pComboPay != NULL )
					{						
						dwID = m_pComboPay->GetCurSel_Data();
					}
					else
					{
						dwID = m_pItem->s_vpElement[ 0 ]->s_pFT->GetInfo()->s_nID;						
					}

					cPrintMsg::PrintMsg( 30312 );
					cMessageBox* pMBox = cMessageBox::GetFirstMessageBox();
					pMBox->SetValue1( 1 );
					pMBox->SetValue2( dwID );				
				}								
				return muReturn;
			default:
				return muReturn;
			}
		}
		break;	
	case PT_PRESENT:
		{
			assert_cs( m_pItem != NULL );

			//CsPoint ptStart = GetRootClient() + CsPoint( 104, 41 );			
			//if( CURSOR_ST.CheckClickBox( CsRect( ptStart, CsSIZE( 156, 19 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
			//{ 
			//	if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN )
			//	{	
			//		if( g_pFocusEdit != NULL )
			//			g_pFocusEdit->ReleaseFocus();

			//		//m_pEditName->SetFocus();
			//		return muReturn;
			//	}
			//}

			//ptStart = GetRootClient() + CsPoint( 19, 218 );			
			//if( CURSOR_ST.CheckClickBox( CsRect( ptStart, CsSIZE( 293, 105 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
			//{ 
			//	if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN )
			//	{	
			//		if( g_pFocusEdit != NULL )
			//			g_pFocusEdit->ReleaseFocus();

			//		m_pEditMessage->SetFocus();
			//		m_pStrList.SetUnderLineCursor();
			//		return muReturn;
			//	}
			//}

			//if( m_pComboPay != NULL )
			//{
			//	int nComboSel = m_pComboPay->Update_ForMouse();
			//	if( nComboSel != cComboBox::CI_INVALIDE )
			//	{
			//		switch( nComboSel )
			//		{
			//		case cComboBox::CI_INRECT:			
			//			return MUT_NONE;
			//		case cComboBox::CI_CLICK:
			//		case cComboBox::CI_SAME_CLICK:
			//			return MUT_CLICK_FOCUS;
			//		default:
			//			{
			//				m_pComboPay->GetCurSel_Data();						
			//			}				
			//			return MUT_CLICK_FOCUS;
			//		}
			//	}
			//}			

			//switch( m_pBuy->Update_ForMouse() )
			//{
			//case cButton::ACTION_NONE:
			//	break;
			//case cButton::ACTION_CLICK:
			//	{			
			//		assert_cs( m_pItem != NULL );	
			//		DWORD dwID;
			//		if( m_pComboPay != NULL )
			//		{						
			//			dwID = m_pComboPay->GetCurSel_Data();
			//		}
			//		else
			//		{
			//			dwID = m_pItem->s_vpElement[ 0 ]->s_pFT->GetInfo()->s_nID;						
			//		}	

			//		//_tcscpy_s( g_pDataMng->GetCashShop()->m_szName, Language::pLength::name + 1, (TCHAR*)m_pEditName->GetText()->GetText() );										
			//		//_tcscpy_s( g_pDataMng->GetCashShop()->m_szMessage, MAX_PATH, m_szMessage );				

			//		//g_pDataMng->GetCashShop()->SetPresent( dwID );				
			//		
			//	}
			//	return muReturn;
			//default:
			//	return muReturn;
			//}
		}
		break;
	default:
		assert_cs( false );
		break;
	}	


	switch( m_pCancel->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{			
			Close();
		}
		return muReturn;
	default:
		return muReturn;
	}

	if( m_pScrollBar )
	{
		if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;	
	}

	return muReturn;
}

void cCashShop_Pay::Render()
{
	if( m_eType == PT_NONE )
		return;

	RenderScript();	

	switch( m_eType )
	{
	case PT_ONE:
		{
			assert_cs( m_pItem != NULL );				
			m_pStrList.Render( GetRootClient() + CsPoint( 23, 145 ), 1 );							

			sCASHSHOP_ELEMENT* pEle = m_pItem->s_vpElement[ 0 ];

			int nType = pEle->s_pFT->GetInfo()->s_nIconID / 100;
			int nIdx = pEle->s_pFT->GetInfo()->s_nIconID % 100;		
			g_pIconMng->RenderCashShopIcon( (ICON_Dynamic::eTYPE)nType, GetRootClient() + CsPoint( 21, 45 ), nIdx );

			if( pEle->s_pFT->GetInfo()->s_nSalePersent > 0 )
			{
				int nSaleIdx = ( pEle->s_pFT->GetInfo()->s_nSalePersent / 5 ) + 5;
				g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + CsPoint( 21, 45 ), nSaleIdx );			
				g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + CsPoint( 21, 45 ), 0 );
			}
			else
			{
				if( pEle->s_pFT->GetInfo()->s_nMaskType != 0 )
				{
					g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + CsPoint( 21, 45 ), pEle->s_pFT->GetInfo()->s_nMaskType );			
				}
			}		
			CsPoint ptNameRenderPos = CsPoint::ZERO;
			CsPoint ptComboRenderPos = CsPoint::ZERO;
#ifdef CASHSHOP_PAY_UI_RENEWAL
			ptNameRenderPos.x = 50;
			ptNameRenderPos.y = 8;
			ptComboRenderPos.x = 120;
			ptComboRenderPos.y = 2;
#endif

			m_pItem->s_NameStr.Render( GetRootClient() + CsPoint( 202, 40 ) + ptNameRenderPos, 0, DT_CENTER );


			if(m_pComboPay)
			{
/*				if( m_pItem->s_vpElement.Size() == 1 )
				{	
					assert_cs( m_pComboPay );
					m_pComboPay->Render( GetRootClient() + CsPoint( 210, 107 ) , DT_CENTER );
				}	
				else
				{*/
					assert_cs( m_pComboPay );
					m_pComboPay->Render( GetRootClient() + CsPoint( 111, 101 ) + ptComboRenderPos );
//				}
			}
		}
		break;
	case PT_PRESENT:
		{
			assert_csm( false, L"아직 선물 기능 없음" );
			//if( m_pEditName->IsFocus() == true )
			//	m_pEditName->Render( GetRootClient() + CsPoint( 112, 42 ) );			
			//else
			//	m_pName->Render( GetRootClient() + CsPoint( 112, 44 ) );

			//assert_cs( m_pItem != NULL );		
			//m_pStrList.Render( GetRootClient() + CsPoint( 23, 220 ), 1 );							

			//sCASHSHOP_ELEMENT* pEle = m_pItem->s_vpElement[ 0 ];
			//int nType = pEle->s_pFT->GetInfo()->s_nIconID / 100;
			//int nIdx = pEle->s_pFT->GetInfo()->s_nIconID % 100;		
			//g_pIconMng->RenderCashShopIcon( (ICON_Dynamic::eTYPE)nType, GetRootClient() + CsPoint( 32, 85 ), nIdx );

			//if( pEle->s_pFT->GetInfo()->s_nSalePersent > 0 )
			//{
			//	int nSaleIdx = ( pEle->s_pFT->GetInfo()->s_nSalePersent / 5 ) + 5;
			//	g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + CsPoint( 32, 85 ), nSaleIdx );			
			//}

			//if( pEle->s_pFT->GetInfo()->s_nMaskType != 0 )
			//{
			//	g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + CsPoint( 32, 85 ), pEle->s_pFT->GetInfo()->s_nMaskType );			
			//}

			//m_pItem->s_NameStr.Render( GetRootClient() + CsPoint( 222, 82 ), 0, DT_CENTER );

			//if( m_pItem->s_vpElement.Size() == 1 )
			//{
			//	assert_cs( m_pPay );
			//	m_pPay->Render( GetRootClient() + CsPoint( 125, 154 ), DT_CENTER );
			//}	
			//else
			//{
			//	assert_cs( m_pComboPay );
			//	m_pComboPay->Render( GetRootClient() + CsPoint( 125, 154 ) );			
			//}
		}
		break;
	default:
		assert_cs( false );
		break;
	}	
	
}


void cCashShop_Pay::ResetDevice()
{		
	ResetDeviceScript();

	if( m_pComboPay ) 
		m_pComboPay->ResetDevice();

//	if( m_pPay )
//		m_pPay->ResetDevice();

	m_pStrList.ResetDevice();	

	//if( m_pEditName )
	//	m_pEditName->ResetDevice();

	//if( m_pEditMessage )
	//	m_pEditMessage->ResetDevice();

	//if( m_pName )
	//	m_pName->ResetDevice();
}

void cCashShop_Pay::BuyOne( sCASHSHOP_ITEM* pItem, 
						   const int& iRealCash, const int& iBonusCash, const int& iTotalCash )
{
	m_pItem = pItem;
	m_eType = PT_ONE;	

	SetRootClient( CsPoint( 512 - 151, 384 - 167 ) );

	cBaseWindow::Init();

	char* pImg = NULL;
	if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
		pImg = "cash\\Shop\\Store_Buy_Big_Aeria.tga";
	else if( GLOBALDATA_ST.IsCountry( GData::eCountry_Steam ) )
		pImg = "cash\\Shop\\Store_Buy_Big_Steam.tga";	
	else if( GLOBALDATA_ST.IsCountry( GData::eCountry_GSP ) )
		pImg = "cash\\Shop\\Store_Buy_Big.tga";	
	else
		pImg = "cash\\Shop\\Store_Buy_01.tga";	

	int nScrollRenderCount = 6;
	CsPoint ptWinSize = CsPoint::ZERO;
	CsPoint ptBuyString = CsPoint::ZERO;
	CsPoint ptMoney = CsPoint::ZERO;


	CsPoint ptCancelButton = CsPoint::ZERO;
	CsPoint ptBuyButton = CsPoint::ZERO;

	CsPoint ptSCrollPos = CsPoint::ZERO;
	CsPoint ptSCrollSize = CsPoint::ZERO;

#ifdef CASHSHOP_PAY_UI_RENEWAL
	nScrollRenderCount = 8;

	ptWinSize.x = 123;
	ptWinSize.y = 57;

	ptBuyString.x = 60;
	ptBuyString.y = 1; 

	ptMoney.x = 122;
	ptMoney.y = 58;

	ptSCrollPos.x = 125;
	ptSCrollPos.y = 10;

	ptSCrollSize.y = 50;
#ifdef VERSION_KOR
	ptCancelButton.x = 120;
	ptCancelButton.y = 1;

	ptBuyButton.x = 60;
	ptBuyButton.y = 55;
 
#else
	ptCancelButton.x = 120;
	ptCancelButton.y = 62;

	ptBuyButton.x = 100;
	ptBuyButton.y = 62;
#endif 

#endif

	InitScript( pImg, m_ptRootClient, CsPoint(303, 362) + ptWinSize, false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );				

	cText::sTEXTINFO ti; 
	ti.Init( CFont::FS_12 );
	ti.s_eTextAlign = DT_RIGHT;	
#ifdef VERSION_KOR
	ti.SetText( _LAN( "아이템 구매" ) ); 
#else
	ti.SetText( _LAN( "상품 구매" ) ); 
#endif

	AddText( &ti, CsPoint(190, 10) + ptBuyString );	

	TCHAR szMoney[ 128 ] = {0,};

#ifdef VERSION_KOR
	int nRealHaveCash = iRealCash;//g_pDataMng->GetCashShop()->GetCashMoney( CsCashShopMng::CT_Real_Cash );
	nsCSFILE::GetNumberStr( szMoney, 128, nRealHaveCash );
	ti.SetText( szMoney ); 
	AddText( &ti, CsPoint( 255, 258 ) + ptMoney );

	int nBonusHaveCash = iBonusCash;//g_pDataMng->GetCashShop()->GetCashMoney( CsCashShopMng::CT_Bonus_Cash );
	nsCSFILE::GetNumberStr( szMoney, 128, nBonusHaveCash );
	ti.SetText( szMoney );
	AddText( &ti, CsPoint( 255, 285 ) + ptMoney);
#else
	#ifndef VERSION_STEAM
		int nTotalHaveCash = iTotalCash;// g_pDataMng->GetCashShop()->GetHaveTotalCash();
		nsCSFILE::GetNumberStr( szMoney, 128, nTotalHaveCash );
		ti.SetText( szMoney );
		AddText( &ti, CsPoint( 255, 285 ) + ptMoney );
	#endif
#endif

	int nSize = m_pItem->s_vpElement.Size();
	if( nSize > 0 )
		_CreateComboControl(true);
	else
		_CreateComboControl(false);

	ti.s_eTextAlign = DT_CENTER;
	ti.s_Color = NiColor::BLACK;
	ti.s_bOutLine = false;
		
	ti.SetText( _LAN( "구매" ) ); 

#ifdef VERSION_KOR
	m_pBuy = AddButton( CsPoint( 120, 322 ) + ptBuyButton, CsPoint( 67, 26 ), cButton::IMAGE_NOR_14, &ti );
	m_pCancel = AddButton( CsPoint( 278, 8 ) + ptCancelButton, CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );
#else
	m_pBuy = AddButton( CsPoint( 75, 320 ) + ptBuyButton, CsPoint( 67, 26 ), cButton::IMAGE_NOR_14, &ti );
	ti.SetText( _LAN( "취소" ) );
	m_pCancel = AddButton( CsPoint( 213, 320 ) + ptCancelButton, CsPoint( 67, 26 ) , cButton::IMAGE_NOR_14, &ti ); 	
#endif

	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 267, 140 ) + ptSCrollPos, CsPoint( 16, 103 ) + ptSCrollSize, CsPoint( 16, 15 ), CsRect( CsPoint( 0, 130 ), CsSIZE( 303 + ptWinSize.x, 130 + ptWinSize.y ) ), nScrollRenderCount );
	m_pStrList.SetBuddy( m_pScrollBar );

	m_pScrollBar->SetRange( CsPoint( 0, m_pStrList.GetSize() ) );
}

void cCashShop_Pay::_MakeString( CsCashShop::sINFO* pCashItem, cString* pString )
{
	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;

	// 갯수 입력
	switch( pCashItem->s_nDispType )
	{
	case CsCashShop::DT_NONE:
	case CsCashShop::DT_PERENNITY:
		break;

	case CsCashShop::DT_COUNT:
	case CsCashShop::DT_DAY:
		{
			ti.SetText( L" " );
			cString::sTEXT* pFirstEleText = pString->AddText( &ti );
			pFirstEleText->s_ptSize.x = 10;		

			ti.SetText( pCashItem->s_nDispCount );
			cString::sTEXT* pEleText = pString->AddText( &ti );
			pFirstEleText->s_ptSize.x -= pEleText->s_Text.GetStringSize().x;
			pEleText->s_ptSize.x += 3;

			pString->CalMaxSize();
		}
		break;
	
	default:
		assert_cs( false );
	}

	switch( pCashItem->s_nDispType )
	{
	case CsCashShop::DT_NONE:
		{
		}			
		break;

#ifndef VERSION_KOR
	case CsCashShop::DT_COUNT:
		{
			pString->AddSprite( g_pGameIF->GetCashShop()->GetSprite( cCashShop::U_Each ), CsPoint( 0, 3 ) );
		}
		break;
	case CsCashShop::DT_DAY:
		{
			pString->AddSprite( g_pGameIF->GetCashShop()->GetSprite( cCashShop::U_Day ) );
		}
		break;
	case CsCashShop::DT_PERENNITY:
		{
			pString->AddSprite( g_pGameIF->GetCashShop()->GetSprite( cCashShop::U_Long ), CsPoint( 0, 4 ) ); 
		}
		break;
#else
	case CsCashShop::DT_COUNT:
		{
			ti.SetText(_T("개") );
			pString->AddText(&ti);
		}
		break;
	case CsCashShop::DT_DAY:
		{
			ti.SetText(_T("일"));
			pString->AddText(&ti);
		}
		break;
	case CsCashShop::DT_PERENNITY:
		{
			ti.SetText(_T("영 구"));
			pString->AddText(&ti);
		}
		break;

#endif

	default:
		assert_cs( false );
	}

	int nSizeX = 40 - pString->GetMaxSize().x;
	pString->TailAddSizeX( nSizeX );

	ti.SetText( L" " );
	cString::sTEXT* pFirstEleText = pString->AddText( &ti );
	pFirstEleText->s_ptSize.x = 40;

#ifdef VERSION_KOR
	ti.SetText( L" " );
	pString->AddText( &ti );
	std::wstring sPrice = DmCS::StringFn::getNumberFormatW(pCashItem->s_nPrice);
	ti.SetText(sPrice.c_str());
#else
	#ifdef VERSION_STEAM
		TCHAR sz[ 128 ] = {0,};
		swprintf_s( sz, L"%.2f", pCashItem->s_nPrice * 0.01);
		ti.SetText( sz );
	#else
		ti.SetText( pCashItem->s_nPrice );
	#endif
#endif

	cString::sTEXT* pEleText = pString->AddText( &ti );
	pFirstEleText->s_ptSize.x -= pEleText->s_Text.GetStringSize().x;
	pEleText->s_ptSize.x += 3;


	pString->AddImage( g_pGameIF->GetCashShop()->GetMoneyImg(), g_pGameIF->GetCashShop()->GetCashType(), CsPoint( 2, -2 ), CsPoint( 18, 18 ) );
}

void cCashShop_Pay::_CreateComboControl(bool bShow)
{
	m_pComboPay = NiNew cComboBox;

	m_pComboPay->Init( NULL, cComboBox::CT_NORMAL2, CsPoint::ZERO, CsPoint( 187, 26 ), CFont::FS_10, DT_CENTER, false, false, false, true );												
	m_pComboPay->SetVisible(bShow);

	int nSize = m_pItem->s_vpElement.Size();
	for( int i = 0; i < nSize; i++ )
	{
		cString* pString = NiNew cString;				

		_MakeString( m_pItem->s_vpElement[ i ]->s_pFT->GetInfo(), pString );	

		m_pComboPay->AddItem( pString, m_pItem->s_vpElement[ i ]->s_pFT->GetInfo()->s_nID , FONT_WHITE );
	}
	m_pComboPay->SetCurSel( 0 );
	m_pComboPay->AddEvent( cComboBox::COMBO_SELECTITEM, this, &cCashShop_Pay::ComboBoxSelect );

	cText::sTEXTINFO ti;
	int iWidth = 0;
#ifdef CASHSHOP_PAY_UI_RENEWAL
	ti.Init( CFont::FS_11 );
	iWidth = 340;
#else
	ti.Init( CFont::FS_10 );
	iWidth = 220;
#endif
	//ti.s_eTextAlign = DT_CENTER;
	g_pStringAnalysis->Cut_Parcing( &m_pStrList, iWidth, m_pItem->s_vpElement[ m_pComboPay->GetCurSel_Idx() ]->s_pFT->GetInfo()->s_szComment, &ti );
}

void cCashShop_Pay::ComboBoxSelect(void* pSender, void* pData)
{
//	int nComboBoxIdx = *static_cast<int*>(pData);	
}
#endif