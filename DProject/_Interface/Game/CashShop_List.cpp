

#include "stdafx.h"
#include "CashShop__Etc.h"

#ifdef FT_USE_CASHSHOP

cCashShop_List::cCashShop_List()
{
	m_pCategory = NULL;
}


void cCashShop_List::Destroy()
{
	_DeleteButton();
	cBaseWindow::Delete();
}

void cCashShop_List::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pLeft );
	SAFE_NIDELETE( m_pRight );	
	SAFE_NIDELETE( m_pPageText );
	SAFE_NIDELETE( m_pMask );
	SAFE_NIDELETE( m_pCategory );	
}

void cCashShop_List::_DeleteButton()
{
	for( int i = 0; i < m_vpBuy.Size(); i++ )
	{
		SAFE_NIDELETE( m_vpBuy[ i ] );
	}

	/*for( int i = 0; i < m_vpPresent.Size(); i++ )
	{
		SAFE_NIDELETE( m_vpPresent[ i ] );
	}*/
}

void cCashShop_List::Create( int nValue /*=0*/ )
{
	cBaseWindow::Init();
	InitScript( m_pRoot, "Cash\\Shop\\Store_Item.tga",
		CsPoint( 320, 115 ) , CsPoint( 562, 598 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );

	m_nCurPage = 0;

	cText::sTEXTINFO ti; 
	ti.Init( CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;

	m_pLeft = NiNew cButton;
	m_pLeft->Init( m_pRoot, CsPoint( 228, 569 ), CsPoint( 22, 22 ), "Cash\\Shop\\LBtn.tga", false );
	m_pLeft->SetTexToken( CsPoint( 0, 22 ) );	

	m_pRight = NiNew cButton;
	m_pRight->Init( m_pRoot, CsPoint( 315, 569 ), CsPoint( 22, 22 ), "Cash\\Shop\\RBtn.tga", false );
	m_pRight->SetTexToken( CsPoint( 0, 22 ) );

	m_pPageText = NiNew cText;
	m_pPageText->Init( m_pRoot, CsPoint( 283, 572 ), &ti, false );	

	m_pMask = NiNew cSprite;
	m_pMask->Init( m_pRoot, CsPoint( 0, 0 ), CsPoint( 266, 118 ), "cash\\Shop\\Mask_03.tga", true );		

	ti.s_Color = NiColor::BLACK;
	ti.s_bOutLine = false;
	CsPoint pt;
	for( int i = 0; i < IF_CASHSHOP_LIST_CNT; i++ )
	{
		/*pt.x = ( i % 2 ) * 271 + 134;
		pt.y = ( i / 2 ) * 122 + 160;*/

		pt.x = ( i % 2 ) * 271 + 167;
		pt.y = ( i / 2 ) * 122 + 160;

		ti.SetText( _LAN( "구매" ) );
		cButton* pBuy = NiNew cButton; 
		pBuy->Init( m_pRoot, pt, CsPoint( 65, 24 ), cButton::IMAGE_NOR_14, &ti, false );
		pBuy->SetTexToken( CsPoint( 0, 47 ) );	
		m_vpBuy.PushBack( pBuy );

		/*ti.SetText( _LAN( "선물" ) );
		pt.x += 65;
		cButton* pPresent = NiNew cButton;
		pPresent->Init( m_pRoot, pt, CsPoint( 65, 24 ), cButton::IMAGE_NOR_14, &ti, false );
		pPresent->SetTexToken( CsPoint( 0, 47 ) );	
		m_vpPresent.PushBack( pPresent );		*/
	}	

	// 인터페이스 위치정보	
	CsSIZE size( 68, 68 );
	for( int i=0; i<IF_CASHSHOP_LIST_CNT; ++i )
	{
		pt.x = ( i % 2 ) * 271 + 40;
		pt.y = ( i / 2 ) * 122 + 92;
		m_IFIcon[ i ].Init( CsRect( pt, size ) );		
	}	
}

void cCashShop_List::Init( eLIST_TYPE eType, DWORD dwItemID )
{
	SAFE_NIDELETE( m_pCategory );
	m_eCurType = eType;

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;	

	// 소분류 카테고리
	CsPoint pos;

#ifdef VERSION_STEAM
	int nChildCnt = nsCsFileTable::g_pCashShopMng_Steam->GetCategory()->s_vpChild[ m_eCurType - 1 ]->s_vpChild.Size();
#else
	int nChildCnt = nsCsFileTable::g_pCashShopMng->GetCategory()->s_vpChild[ m_eCurType - 1 ]->s_vpChild.Size();
#endif
	for( int i=0; i<nChildCnt; ++i )
	{	
#ifdef VERSION_STEAM
		CsCashShopMng_Steam::sCATEGORY* pCat = nsCsFileTable::g_pCashShopMng_Steam->GetCategory()->s_vpChild[ m_eCurType - 1 ]->s_vpChild[ i ];	
#else
		CsCashShopMng::sCATEGORY* pCat = nsCsFileTable::g_pCashShopMng->GetCategory()->s_vpChild[ m_eCurType - 1 ]->s_vpChild[ i ];	
#endif	
		if( nChildCnt > 6 )
		{
			if( i + 1 > 6 )			
				pos.y = 43;			
			else
				pos.y = 9;
			
			pos.x = 15 + ( (i % 6) * 87 );

			if( i == 0 )
			{
				ti.SetText( pCat->s_szName );

				m_pCategory = NiNew cRadioButton;
				m_pCategory->Init( m_pRoot, pos, CsPoint( 85, 33 ), cCheckBox::IMAGE_RADIO_5, &ti, false );
			}
			else
			{
				ti.SetText( pCat->s_szName );
				m_pCategory->AddNode( pos, CsPoint( 85, 33 ), cCheckBox::IMAGE_RADIO_5, &ti );
			}
		}
		else
		{
			pos.y = 43;
			pos.x = 15 + ( (i % 6) * 87 );

			if( i == 0 )
			{
				ti.SetText( pCat->s_szName );

				m_pCategory = NiNew cRadioButton;
				m_pCategory->Init( m_pRoot, pos, CsPoint( 85, 33 ), cCheckBox::IMAGE_RADIO_5, &ti, false );
			}
			else
			{
				ti.SetText( pCat->s_szName );
				m_pCategory->AddNode( pos, CsPoint( 85, 33 ), cCheckBox::IMAGE_RADIO_5, &ti );
			}
		}
	}
	m_pCategory->SetCheckIndex( 0 );			

	_AddItem( m_pCategory->GetCheckIndex(), dwItemID );
}

void cCashShop_List::_AddItem( int nSmallType, DWORD dwItemID )
{
	CsPoint pt;

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;

	m_Group.Delete();
	// 리소스 생성
	int nCashType = g_pGameIF->GetCashShop()->GetCashType();
#ifdef VERSION_STEAM
	CsCashShop::MAP::iterator it = nsCsFileTable::g_pCashShopMng_Steam->GetMapCashShop( nCashType )->begin();		
	CsCashShop::MAP::iterator itEnd = nsCsFileTable::g_pCashShopMng_Steam->GetMapCashShop( nCashType )->end();
#else
	CsCashShop::MAP::iterator it = nsCsFileTable::g_pCashShopMng->GetMapCashShop( nCashType )->begin();		
	CsCashShop::MAP::iterator itEnd = nsCsFileTable::g_pCashShopMng->GetMapCashShop( nCashType )->end();
#endif

	std::map< int, CsCashShop* >	CurCartagorylist;
	for( ; it != itEnd; it++ )
	{
		if( it->second->GetInfo()->GetLargrType() != m_eCurType )
			continue;

		CurCartagorylist[ it->second->GetInfo()->s_nSlot ] = it->second;
	}


	std::map< int, CsCashShop* >::reverse_iterator rit = CurCartagorylist.rbegin();
	std::map< int, CsCashShop* >::reverse_iterator ritEnd = CurCartagorylist.rend();		
	
	int nIdx = 0;
	m_nCurPage = 0;
	for( ; rit != ritEnd; rit++ )
	{	
		CsCashShop::sINFO* pp = rit->second->GetInfo();

		//m_nCurPage = 0;	// 캐쉬샵에서 item 클릭 시 해당 아이템이 있는 페이지로 이동해야 하는데 반복문 안에 초기화문이 있어서
							// 첫페이지만 보이는 문제 있음. 초기화문 위로 뺌.
		if( nSmallType != 0 )
		{
			if( nSmallType != ( rit->second->GetInfo()->GetSmallType() - 1 ) )
				continue;
		}

		if( dwItemID == rit->second->GetInfo()->s_nID )
		{
			m_nCurPage = nIdx / IF_CASHSHOP_LIST_CNT;
		}				

		pt.x = ( nIdx % 2 ) * 271 + 14;
		pt.y = ( ( nIdx % IF_CASHSHOP_LIST_CNT ) / 2 ) * 122 + 80;

		if( !_EnableItemPurchaseTime( nCashType, rit->second->GetInfo()->s_nID ) )
			continue;

#ifdef VERSION_STEAM
		CsCashShop* pItemInfo = nsCsFileTable::g_pCashShopMng_Steam->GetCashShop( nCashType, rit->second->GetInfo()->s_nID );
#else
		CsCashShop* pItemInfo = nsCsFileTable::g_pCashShopMng->GetCashShop( nCashType, rit->second->GetInfo()->s_nID );
#endif
		SAFE_POINTER_CON( pItemInfo );

		if( m_Group.Add( pItemInfo, CsRect( pt, CsSIZE( 266, 118 ) ), 133, CFont::FS_12 ) )
			nIdx++;
	}	
	

	m_nMaxPage = (m_Group.s_vpItem.Size() - 1) / IF_CASHSHOP_LIST_CNT + 1;

	TCHAR szPage[ 32 ];
	_stprintf_s( szPage, 32, _T( "%d/%d" ), m_nCurPage + 1, m_nMaxPage );
	ti.SetText( szPage );
	m_pPageText->SetText( szPage );

	if( m_Group.s_vpItem.Size() <= IF_CASHSHOP_LIST_CNT  )
	{
		m_pLeft->SetEnable( false );
		m_pRight->SetEnable( false );
	}
	else
	{
		if( m_nCurPage == 0 )
		{
			m_pLeft->SetEnable( false );
			m_pRight->SetEnable( true );
		}
		else if( m_nCurPage == m_nMaxPage - 1 )
		{
			m_pLeft->SetEnable( true );
			m_pRight->SetEnable( false );
		}
		else if( ( m_nCurPage > 0 ) && ( m_nCurPage < m_nMaxPage - 1 ) )
		{
			m_pLeft->SetEnable( true );
			m_pRight->SetEnable( true );
		}		
		else
		{
			m_pLeft->SetEnable( false );
			m_pRight->SetEnable( false );
		}
	}
}

void cCashShop_List::Update(float const& fDeltaTime)
{
}

cBaseWindow::eMU_TYPE
cCashShop_List::Update_ForMouse()
{
	m_ptMask = CsPoint::ZERO; 
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	int nRadioAction;
	if( (nRadioAction = m_pCategory->Update_ForMouse()) != cRadioButton::INVALIDE_RADIO_CLICK )
	{
		if( nRadioAction == cRadioButton::ACTION_CLICK )
		{
			cWindow::PlaySound( cWindow::SD_At2 );		
			_AddItem( m_pCategory->GetCheckIndex(), 0 );
			return muReturn;
		}		
	}			
	
	if( m_pLeft->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		--m_nCurPage;		

		TCHAR szPage[ 32 ];
		_stprintf_s( szPage, 32, _T( "%d/%d" ), m_nCurPage + 1, m_nMaxPage );
		m_pPageText->SetText( szPage );

		if( m_nCurPage == 0 )
			m_pLeft->SetEnable( false );
		m_pRight->SetEnable( true );

		return muReturn;
	}
	else if( m_pRight->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		++m_nCurPage;		

		TCHAR szPage[ 32 ];
		_stprintf_s( szPage, 32, _T( "%d/%d" ), m_nCurPage + 1, m_nMaxPage );
		m_pPageText->SetText( szPage );

		if( m_nCurPage == m_nMaxPage - 1 )
			m_pRight->SetEnable( false );
		m_pLeft->SetEnable( true );

		return muReturn;
	}
	
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	m_ptMask = m_Group.Update_Mouse( ptMouseLocalPos, CsPoint( m_nCurPage * IF_CASHSHOP_LIST_CNT,
		m_nCurPage * IF_CASHSHOP_LIST_CNT + IF_CASHSHOP_LIST_CNT ), m_nGroup );	

	if( m_nGroup != -1 )
	{
		for( int i = 0; i < IF_CASHSHOP_LIST_CNT; i++ )
		{
			// 아이템 구매 
			if( m_vpBuy[ i ]->Update_ForMouse() == cButton::ACTION_CLICK )
			{
				g_pGameIF->GetCashShop()->SetPay( m_Group.GetItem( m_nGroup ), cCashShop_Pay::PT_ONE );
				return muReturn;
			}

			// 아이템 선물
			/*if( m_vpPresent[ i ]->Update_ForMouse() == cButton::ACTION_CLICK )
			{	
				g_pGameIF->GetCashShop()->SetPay( m_Group.GetItem( m_nGroup ), cCashShop_Pay::PT_PRESENT );
				return muReturn;
			}*/		
		}
	}

	if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
	{
		if( m_nGroup != -1 )
		{
			CsCashShop* pInfo = m_Group.GetItemFind( m_nGroup );		
			if( pInfo->GetInfo()->GetLargrType() == CsCashShop::eAvatar )
			{
				DWORD dwItemID = pInfo->GetInfo()->s_spItem[ 0 ].s_nItemID;			
				int nItemType = nsCsFileTable::g_pItemMng->GetItem( dwItemID )->GetInfo()->s_nType_L;

				g_pGameIF->GetCashShop()->SetAvatarItem( dwItemID, nItemType - nItem::Head );		
			}
		}
	}	

	return muReturn;
}

void cCashShop_List::Render()
{
	RenderScript();

	sCASHSHOP_ELEMENT* pEle = NULL;
	for( int i=m_nCurPage * IF_CASHSHOP_LIST_CNT; i<m_nCurPage * IF_CASHSHOP_LIST_CNT + IF_CASHSHOP_LIST_CNT; ++i )
	{
		if( m_Group.IsItem( i ) == false )
			break;							

		pEle = m_Group.s_vpItem[ i ]->s_vpElement[ 0 ];		

		CsPoint ptRenderPos = m_IFIcon[ i%IF_CASHSHOP_LIST_CNT ].GetPos();		
		CsPoint ptRenderSize = m_IFIcon[ i%IF_CASHSHOP_LIST_CNT ].GetSize();				

		int nType = pEle->s_pFT->GetInfo()->s_nIconID / 100;
		int nIdx = pEle->s_pFT->GetInfo()->s_nIconID % 100;		
		g_pIconMng->RenderCashShopIcon( (ICON_Dynamic::eTYPE)nType, GetRootClient() + ptRenderPos, CsPoint( 68, 68 ), nIdx );

		if( pEle->s_pFT->GetInfo()->s_nSalePersent > 0 )
		{
			int nSaleIdx = ( pEle->s_pFT->GetInfo()->s_nSalePersent / 5 ) + 5;
			g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + ptRenderPos, CsPoint( 68, 68 ), nSaleIdx );			
			g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + ptRenderPos, CsPoint( 68, 68 ), 0 );
		}
		else
		{
			if( pEle->s_pFT->GetInfo()->s_nMaskType != 0 )
			{
				g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + ptRenderPos, CsPoint( 68, 68 ), pEle->s_pFT->GetInfo()->s_nMaskType );			
			}
		}

		m_Group.s_vpItem[ i ]->s_NameStr.Render( ptRenderPos + CsPoint( 160, 5 ) + GetRootClient(), 0, DT_CENTER );
		
		if( m_Group.s_vpItem[ i ]->s_pPriceStr->GetListSize() == 1 )
			m_Group.s_vpItem[ i ]->s_pPriceStr->Render( ptRenderPos + CsPoint( 34, 76 ) + GetRootClient(), DT_CENTER );
		else
			m_Group.s_vpItem[ i ]->s_pPriceStr->Render( ptRenderPos + CsPoint( 75, 76 ) + GetRootClient(), DT_RIGHT );
	}		

	if( ( m_nCurPage + 1 ) == m_nMaxPage )
	{
		int nCount = m_Group.s_vpItem.Size() - ( m_nCurPage * IF_CASHSHOP_LIST_CNT );
		for( int i=0; i < nCount; i++ )
		{
			m_vpBuy[ i ]->Render();
			//m_vpPresent[ i ]->Render();
		}
	}
	else
	{
		for( int i=0; i < IF_CASHSHOP_LIST_CNT; i++ )
		{
			m_vpBuy[ i ]->Render();
			//m_vpPresent[ i ]->Render();
		}
	}

	m_pLeft->Render();
	m_pRight->Render();	
	m_pPageText->Render();
	
	if( m_pCategory )	
		m_pCategory->Render();

	if( m_ptMask != CsPoint::ZERO )
	{
		m_pMask->Render( m_ptMask  );
	}
}



void cCashShop_List::ResetDevice()
{	
	ResetDeviceScript();
	
	if( m_pLeft )
		m_pLeft->ResetDevice();
	if(	m_pRight )
		m_pRight->ResetDevice();
	if( m_pPageText )
		m_pPageText->ResetDevice();
	if( m_pMask )
		m_pMask->ResetDevice();
	if( m_pCategory )
		m_pCategory->ResetDevice();

	m_Group.ResetDevice();

	for( int i = 0; i < IF_CASHSHOP_LIST_CNT; i++ )
	{
		m_vpBuy[ i ]->ResetDevice();
		//m_vpPresent[ i ]->ResetDevice();
	}
}

bool cCashShop_List::_EnableItemPurchaseTime( int const& nType, DWORD const& dwItemCode )
{
#ifdef VERSION_STEAM
	CsCashShop* pItemInfo = nsCsFileTable::g_pCashShopMng_Steam->GetCashShop( nType, dwItemCode );
#else
	CsCashShop* pItemInfo = nsCsFileTable::g_pCashShopMng->GetCashShop( nType, dwItemCode );
#endif

	SAFE_POINTER_RETVAL( pItemInfo, NULL );
	CsCashShop::sINFO* pInfo = pItemInfo->GetInfo();
	SAFE_POINTER_RETVAL( pInfo, false );

	uint nStartTime = nBase::GetTime( pInfo->s_szStartTime );
	uint nEndTime = nBase::GetTime( pInfo->s_szEndTime );
	uint nCurTime = net::game->GetTimeTS();
	if( nCurTime < nStartTime || nCurTime > nEndTime )
		return false;
	return true;
}

#endif