

#include "stdafx.h"
#include "CashShop__Etc.h"

#ifdef FT_USE_CASHSHOP

cCashShop_Main::cCashShop_Main()
{
	for( int i=0; i<ET_CNT; ++i )
	{
		m_pLeft[ i ] = NULL;
		m_pRight[ i ] = NULL;
		m_pPageText[ i ] = NULL;
	}
}


void cCashShop_Main::Destroy()
{
	cBaseWindow::Delete();
}

void cCashShop_Main::DeleteResource()
{
	DeleteScript();

	for( int i=0; i<ET_CNT; ++i )
	{
		m_Group[ i ].Delete();			
		SAFE_NIDELETE( m_pLeft[ i ] );
		SAFE_NIDELETE( m_pRight[ i ] );
		SAFE_NIDELETE( m_pPageText[ i ] );
	}

	SAFE_NIDELETE( m_pMask );
}

void cCashShop_Main::Create( int nValue /*=0*/ )
{
	cBaseWindow::Init();
	InitScript( m_pRoot, "Cash\\Shop\\SubWin_Main.tga",
		CsPoint( 320, 115 ) , CsPoint( 562, 598 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_12 );
	ti.s_eTextAlign = DT_CENTER;

	// 메인쪽에서의 카테고리
#ifdef VERSION_STEAM
	CsCashShopMng_Steam::sCATEGORY* pTopCategory = nsCsFileTable::g_pCashShopMng_Steam->GetCategory();
	CsCashShopMng_Steam::sCATEGORY* pMainCat = pTopCategory->s_vpChild[ 0 ];
#else
	CsCashShopMng::sCATEGORY* pTopCategory = nsCsFileTable::g_pCashShopMng->GetCategory();
	CsCashShopMng::sCATEGORY* pMainCat = pTopCategory->s_vpChild[ 0 ];
#endif

	ti.SetText( pMainCat->s_vpChild[ 0 ]->s_szName );
	AddText( &ti, CsPoint( 80, 10 ) );

	ti.SetText( pMainCat->s_vpChild[ 1 ]->s_szName );
	AddText( &ti, CsPoint( 80, 208 ) );

	ti.SetText( pMainCat->s_vpChild[ 2 ]->s_szName );
	AddText( &ti, CsPoint( 80, 403 ) );

	// 인터페이스 위치정보	
	CsPoint ptNew( 46, 44 );
	CsPoint ptHit( 46, 242 );
	CsPoint ptEvent( 46, 437 );
	CsSIZE size( 80, 80 );	
	for( int i=0; i<4; ++i )
	{
		m_IFIcon[ ET_NEW ][ i ].Init( CsRect( ptNew, size ) );
		m_IFIcon[ ET_HIT ][ i ].Init( CsRect( ptHit, size ) );
		m_IFIcon[ ET_EVENT ][ i ].Init( CsRect( ptEvent, size ) );

		ptNew.x += 129;
		ptHit.x += 129;
		ptEvent.x += 129;
	}

	// 페이지 컨트롤
	ti.s_eFontSize = CFont::FS_10;
	for( int i=0; i<ET_CNT; ++i )
	{
		m_nCurPage[ i ] = 0;

		if( m_Group[ i ].s_vpItem.Size() <= 4 )
			continue;

		int nY;
		switch( i )
		{
		case ET_NEW:	nY = 5;					break;
		case ET_HIT:	nY = 201;				break;
		case ET_EVENT:	nY = 397;				break;
		default:		assert_cs( false );
		}

		m_pLeft[ i ] = NiNew cButton;
		m_pLeft[ i ]->Init( m_pRoot, CsPoint( 441, nY ), CsPoint( 22, 22 ), "Cash\\Shop\\LBtn.tga", false );
		m_pLeft[ i ]->SetTexToken( CsPoint( 0, 22 ) );
		m_pLeft[ i ]->SetEnable( false );

		m_pRight[ i ] = NiNew cButton;
		m_pRight[ i ]->Init( m_pRoot, CsPoint( 521, nY ), CsPoint( 22, 22 ), "Cash\\Shop\\RBtn.tga", false );
		m_pRight[ i ]->SetTexToken( CsPoint( 0, 22 ) );

		TCHAR szPage[ 32 ];
		_stprintf_s( szPage, 32, _T( "1/%d" ), m_Group[ i ].s_vpItem.Size() / 4 + 1 );
		ti.SetText( szPage );
		m_pPageText[ i ] = NiNew cText;
		m_pPageText[ i ]->Init( m_pRoot, CsPoint( 491, nY + 2 ), &ti, false );
	}

	m_pMask = NiNew cSprite;
	m_pMask->Init( m_pRoot, CsPoint( 0, 0 ), CsPoint( 127, 163 ), "cash\\Shop\\Mask_02.tga", true );	
}

void cCashShop_Main::CreateElement()
{
	for( int i=0; i<ET_CNT; ++i )
	{
		m_Group[ i ].Delete();	
	}

	// 리소스 생성
	int nCashType;
	nCashType = g_pGameIF->GetCashShop()->GetCashType();
#ifdef VERSION_STEAM
	CsVectorPB< int >* VecArray = nsCsFileTable::g_pCashShopMng_Steam->GetSubWin_MainVP_Array( nCashType );
#else
	CsVectorPB< int >* VecArray = nsCsFileTable::g_pCashShopMng->GetSubWin_MainVP_Array( nCashType );
#endif
	SAFE_POINTER_RET( VecArray );

	for( int i=0; i<ET_CNT; ++i )
	{
		int nSize = VecArray[ i ].Size();
		int nShowCount = 0;
		for( int e=0; e<nSize; ++e )
		{
			if( nShowCount >= 4 )// 메인 페이지에 표시되는 최대 아이템 갯수
				break;

			if( !_EnableItemPurchaseTime( nCashType, VecArray[ i ][ e ] ) )
				continue;

#ifdef VERSION_STEAM
			CsCashShop* pAddItem = nsCsFileTable::g_pCashShopMng_Steam->GetCashShop( nCashType, VecArray[ i ][ e ] );
#else
			CsCashShop* pAddItem = nsCsFileTable::g_pCashShopMng->GetCashShop( nCashType, VecArray[ i ][ e ] );
#endif
			SAFE_POINTER_CON( pAddItem );

			switch( i )
			{
			case 0:
				m_Group[ i ].Add( pAddItem,CsRect( CsPoint( 23 + ( 129 * ( nShowCount % 4 ) ), 33 ), CsSIZE( 127, 163 ) ), 113 );
				break;
			case 1:
				m_Group[ i ].Add( pAddItem,	CsRect( CsPoint( 23 + ( 129 * ( nShowCount % 4 ) ), 231 ), CsSIZE( 127, 163 ) ), 113 );
				break;
			case 2:
				m_Group[ i ].Add( pAddItem,	CsRect( CsPoint( 23 + ( 129 * ( nShowCount % 4 ) ), 426 ), CsSIZE( 127, 163 ) ), 113 );
				break;
			default:
				assert_cs( false );
			}			
			nShowCount++;
		}
	}
}

void cCashShop_Main::Update(float const& fDeltaTime)
{
}

cBaseWindow::eMU_TYPE
cCashShop_Main::Update_ForMouse()
{
	m_ptMask = CsPoint::ZERO; 
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )		
		return muReturn;	

	for( int type=0; type<ET_CNT; ++type )
	{
		CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );				
		if( m_ptMask == CsPoint::ZERO )
		{	
			m_ptMask = m_Group[ type ].Update_Mouse( ptMouseLocalPos, CsPoint( m_nCurPage[ type ] * 4, m_nCurPage[ type ] * 4 + 4 ), m_nGroup );			
		}		
		if( m_nGroup != -1 && CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
		{				
			if( g_pGameIF->GetCashShop() ) 
				g_pGameIF->GetCashShop()->SetItemLink( m_Group[ type ].GetItemFind( m_nGroup ) );
			return muReturn;
		}

		if( m_pLeft[ type ] == NULL )
			continue;

		if( m_pLeft[ type ]->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			--m_nCurPage[ type ];
			int nEndPage = m_Group[ type ].s_vpItem.Size() / 4;

			TCHAR szPage[ 32 ];
			_stprintf_s( szPage, 32, _T( "%d/%d" ), m_nCurPage[ type ] + 1, nEndPage + 1 );
			m_pPageText[ type ]->SetText( szPage );

			if( m_nCurPage[ type ] == 0 )
				m_pLeft[ type ]->SetEnable( false );
			m_pRight[ type ]->SetEnable( true );

			return muReturn;
		}
		else if( m_pRight[ type ]->Update_ForMouse() == cButton::ACTION_CLICK )
		{
			++m_nCurPage[ type ];
			int nEndPage = m_Group[ type ].s_vpItem.Size() / 4;

			TCHAR szPage[ 32 ];
			_stprintf_s( szPage, 32, _T( "%d/%d" ), m_nCurPage[ type ] + 1, nEndPage + 1 );
			m_pPageText[ type ]->SetText( szPage );

			if( m_nCurPage[ type ] == nEndPage )
				m_pRight[ type ]->SetEnable( false );
			m_pLeft[ type ]->SetEnable( true );

			return muReturn;
		}					
	}

	return muReturn;
}

void cCashShop_Main::Render()
{
	RenderScript();

	sCASHSHOP_ELEMENT* pEle = NULL;
	for( int type=0; type<ET_CNT; ++type )
	{
		//int nSize = CsMin( 4, m_Group[ type ].s_vpElement.Size() );
		for( int i=0; i<4; ++i )
		{
			int nItemIdx = i + ( m_nCurPage[ type ] * 4 );
			
			if( m_Group[ type ].IsItem( nItemIdx ) == false )
				break;			

			pEle = m_Group[ type ].s_vpItem[ nItemIdx ]->s_vpElement[ 0 ];				

			CsPoint ptRenderPos = m_IFIcon[ type ][ i ].GetPos();			

			int nType = pEle->s_pFT->GetInfo()->s_nIconID / 100;
			int nIdx = pEle->s_pFT->GetInfo()->s_nIconID % 100;		
			g_pIconMng->RenderCashShopIcon( (ICON_Dynamic::eTYPE)nType, GetRootClient() + ptRenderPos, nIdx );

			if( pEle->s_pFT->GetInfo()->s_nSalePersent > 0 )
			{
				int nSaleIdx = ( pEle->s_pFT->GetInfo()->s_nSalePersent / 5 ) + 5;
				g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + ptRenderPos, nSaleIdx );			
				g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + ptRenderPos,  0 );
			}
			else
			{
				if( pEle->s_pFT->GetInfo()->s_nMaskType != 0 )
				{
					g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + ptRenderPos, pEle->s_pFT->GetInfo()->s_nMaskType );			
				}
			}
			
			m_Group[ type ].s_vpItem[ nItemIdx ]->s_NameStr.Render( ptRenderPos + CsPoint( 40, 92 ) + GetRootClient(), 1, DT_CENTER );			
		}

		if( m_pLeft[ type ] )
		{
			m_pLeft[ type ]->Render();
			m_pRight[ type ]->Render();
			m_pPageText[ type ]->Render();
		}
	}

	if( m_ptMask != CsPoint::ZERO )
	{
		m_pMask->Render( m_ptMask  );
	}		
}



void cCashShop_Main::ResetDevice()
{	
	ResetDeviceScript();

	for( int type=0; type<ET_CNT; ++type )
	{
		m_Group[ type ].ResetDevice();
		if( m_pLeft[ type ] )
			m_pLeft[ type ]->ResetDevice();
		if( m_pRight[ type ] )
			m_pRight[ type ]->ResetDevice();
		if( m_pPageText[ type ] )
			m_pPageText[ type ]->ResetDevice();
	}

	if( m_pMask )
		m_pMask->ResetDevice();
}


bool cCashShop_Main::_EnableItemPurchaseTime( int const& nType, DWORD const& dwItemCode )
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