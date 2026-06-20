

#include "stdafx.h"
#include "CashShop__Etc.h"

#ifdef FT_USE_CASHSHOP

cCashShop_Best::cCashShop_Best()
{
}


void cCashShop_Best::Destroy()
{
	cBaseWindow::Delete();
}

void cCashShop_Best::DeleteResource()
{
	DeleteScript();	

	m_Group.Delete();
	SAFE_NIDELETE( m_pMask );
}

void cCashShop_Best::Create( int nValue /*=0*/ )
{
	cBaseWindow::Init();
	InitScript( NULL, CsPoint( 884, 115 ) , CsPoint( 136, 599 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER );

	// 인터페이스 위치정보
	CsPoint pt( 28, 61 );
	CsSIZE size( 80, 80 );
	for( int i=0; i<IF_CASHSHOP_BEST_CNT; ++i )
	{
		m_IFIcon[ i ].Init( CsRect( pt, size ) );
		pt.y += 108;
	}	

	m_pMask = NiNew cSprite;
	m_pMask->Init( m_pRoot, CsPoint( 0, 0 ), CsPoint( 106, 104 ), "cash\\Shop\\Mask_01.tga", true );	
}

void cCashShop_Best::_GetEnableItemTime( int const& nCashType, std::vector<int>& vec, CsVectorPB< int >* pTable )
{
	SAFE_POINTER_RET( pTable );

	int nTotalSize = pTable->Size();
	for( int n = 0; n < nTotalSize; ++n )
	{
		if( !_EnableItemPurchaseTime( nCashType, (*pTable)[ n ] ) )
			continue;
		vec.push_back( (*pTable)[ n ] );
	}
}

void cCashShop_Best::CreateElement()
{
	m_Group.Delete();

	// 리소스 생성
	int nCashType = g_pGameIF->GetCashShop()->GetCashType();
#ifdef VERSION_STEAM
	CsVectorPB< int >* pVec = nsCsFileTable::g_pCashShopMng_Steam->GetSubWin_BestVP( nCashType );
#else
	CsVectorPB< int >* pVec = nsCsFileTable::g_pCashShopMng->GetSubWin_BestVP( nCashType );
#endif
	std::vector<int> itemList;
	_GetEnableItemTime( nCashType, itemList, pVec );

	if( itemList.empty() )
		return;

	int nEnableItemCount = itemList.size();
	srand((unsigned)time(NULL));
	for( int i = 0; i < nEnableItemCount; i++ )
		swap( itemList[ rand() % nEnableItemCount ], itemList[ rand() % nEnableItemCount ] );

	std::vector<int>::iterator it = itemList.begin();
	for( int nShowCount = 0; it != itemList.end(); ++it, ++nShowCount )
	{
		if( nShowCount >= IF_CASHSHOP_BEST_CNT )
			break;

#ifdef VERSION_STEAM
		CsCashShop* pAddItem = nsCsFileTable::g_pCashShopMng_Steam->GetCashShop( nCashType, (*it) );
#else
		CsCashShop* pAddItem = nsCsFileTable::g_pCashShopMng->GetCashShop( nCashType, (*it) );
#endif
		SAFE_POINTER_CON( pAddItem );

		m_Group.Add( pAddItem, CsRect( CsPoint( 15, 50 + ( 108 * nShowCount ) ), CsSIZE( 106, 104 ) ), 113 );

	}
}

void cCashShop_Best::Update(float const& fDeltaTime)
{
}

cBaseWindow::eMU_TYPE
cCashShop_Best::Update_ForMouse()
{	
	m_ptMask = CsPoint::ZERO;
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	
	
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	m_ptMask = m_Group.Update_Mouse( ptMouseLocalPos, CsPoint( 0, 5 ), m_nGroup );	

	if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_UP )
	{
		if( m_nGroup != -1 )
		{
			g_pGameIF->GetCashShop()->SetItemLink( m_Group.GetItemFind( m_nGroup ) );				
			return muReturn;
		}
	}

	return muReturn;
}

void cCashShop_Best::Render()
{
	RenderScript();
	
	sCASHSHOP_ELEMENT* pEle = NULL;
	for( int i=0; i<IF_CASHSHOP_BEST_CNT; ++i )
	{
		if( m_Group.IsItem( i ) == false )
			break;							

		pEle = m_Group.s_vpItem[ i ]->s_vpElement[ 0 ];		

		CsPoint ptRenderPos = m_IFIcon[ i ].GetPos();		

		int nType = pEle->s_pFT->GetInfo()->s_nIconID / 100;
		int nIdx = pEle->s_pFT->GetInfo()->s_nIconID % 100;		
		g_pIconMng->RenderCashShopIcon( (ICON_Dynamic::eTYPE)nType, GetRootClient() + ptRenderPos, nIdx );

		if( pEle->s_pFT->GetInfo()->s_nSalePersent > 0 )
		{
			int nSaleIdx = ( pEle->s_pFT->GetInfo()->s_nSalePersent / 5 ) + 5;
			g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + ptRenderPos, nSaleIdx );			
			g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + ptRenderPos, 0 );
		}
		else
		{
			if( pEle->s_pFT->GetInfo()->s_nMaskType != 0 )
			{
				g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + ptRenderPos, pEle->s_pFT->GetInfo()->s_nMaskType );			
			}
		}
	}

	if( m_ptMask != CsPoint::ZERO )
	{
		m_pMask->Render( m_ptMask  );
	}		
}



void cCashShop_Best::ResetDevice()
{	
	ResetDeviceScript();

	if( m_pMask )
		m_pMask->ResetDevice();
}


bool cCashShop_Best::_EnableItemPurchaseTime( int const& nType, DWORD const& dwItemCode )
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