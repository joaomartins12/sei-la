
#include "stdafx.h"
#include "CashShop__Etc.h"

#ifdef FT_USE_CASHSHOP

void sCASHSHOP_ELEMENT::Init( CsCashShop* pFT )
{
	s_pFT = pFT;
}

BOOL sCASHSHOP_ITEM::IsRect( CsPoint ptPos )
{
	return s_rcArea.PtInRect( ptPos );
}

void sCASHSHOP_ITEM::Delete()
{
	s_pPriceStr->Delete();
	SAFE_NIDELETE( s_pPriceStr ); 
	s_NameStr.Delete();

	int nSize = s_vpElement.Size();
	for( int i=0; i<nSize; ++i )
	{
		NiDelete s_vpElement[ i ];
	}
	s_vpElement.Destroy();
}

void sCASHSHOP_ITEM::ResetDevice()
{
	s_NameStr.ResetDevice();
	if( s_pPriceStr )
		s_pPriceStr->ResetDevice();
}

void sCASHSHOP_ITEM::SetPrice( CsCashShop* pFT, int nSize )
{			
	TCHAR sz[ 128 ] = {0,};

	SAFE_POINTER_RET(pFT);
		
	if( s_vpElement.Size() == 0 )
	{
		cText::sTEXTINFO ti;
		ti.Init( (CFont::eFACE_SIZE)nSize );
		//ti.s_eTextAlign = DT_CENTER;

		SAFE_POINTER_RET(pFT->GetInfo());
#ifdef VERSION_STEAM
		swprintf_s( sz, L"%.2f", pFT->GetInfo()->s_nPrice * 0.01);
#else
		nsCSFILE::GetNumberStr( sz, 128, pFT->GetInfo()->s_nPrice );
#endif
		ti.SetText( sz );

		s_pPriceStr = NiNew cString;

		s_pPriceStr->AddText( &ti );

		SAFE_POINTER_RET(g_pGameIF->GetCashShop());

		cImage* pImage = g_pGameIF->GetCashShop()->GetMoneyImg();
		SAFE_POINTER_RET(pImage);
		int nImgIndex = g_pGameIF->GetCashShop()->GetCashType();
		s_pPriceStr->AddImage( pImage, nImgIndex, CsPoint( 2, -2 ), CsPoint( 18, 18 ) );
	}	
	else		
	{
		assert_cs( s_pPriceStr != NULL );

		SAFE_POINTER_RET(pFT->GetInfo());

		switch(pFT->GetInfo()->s_nDispType)
		{
		case 1:
			_tcscpy_s( sz, 128, _LAN( "수량선택" ) );
			break;
		case 2:
		case 3:
			_tcscpy_s( sz, 128, _LAN( "기간선택" ) );
			break;
		default:
			break;
		}

		s_pPriceStr->SetText( 0, sz );
		s_pPriceStr->DeleteElement( 1 );
	}
}

CsCashShop* sCASHSHOP_ITEM::Find( DWORD dwID )
{
	int nSize = s_vpElement.Size();
	for( int i = 0; i < nSize; i++ )
	{
		if( s_vpElement[ i ]->s_pFT->GetInfo()->s_nID == dwID )
		{
			return s_vpElement[ i ]->s_pFT;
		}
	}

	assert_cs( false );
	return NULL;
}

void sCASHSHOP_GROUP::Delete()
{
	int nSize = s_vpItem.Size();
	for( int i=0; i<nSize; ++i )
	{
		NiDelete s_vpItem[ i ];
	}
	s_vpItem.Destroy();
}

void sCASHSHOP_GROUP::ResetDevice()
{
	int nSize = s_vpItem.Size();
	for( int i=0; i<nSize; ++i )
	{
		s_vpItem[ i ]->ResetDevice();
	}
}

sCASHSHOP_ITEM* sCASHSHOP_GROUP::GetItem( int nGroup )
{
	int nSize = s_vpItem.Size();
	for( int i = 0; i < nSize; i++ )
	{
		if( s_vpItem[ i ]->s_nGroup == nGroup )
		{
			return s_vpItem[ i ];
		}
	}

	return NULL;
}

bool sCASHSHOP_GROUP::Add( CsCashShop* pFT, CsRect rcArea, int nCutSize, int nSize /* = CFont::FS_10 */ )
{
	SAFE_POINTER_RETVAL( pFT, false ); 

	if( pFT->GetInfo()->s_bActive == false )
		return false;

	sCASHSHOP_ELEMENT* pEle = NiNew sCASHSHOP_ELEMENT;
	pEle->Init( pFT );

	sCASHSHOP_ITEM* pItem = NULL;
	assert_cs( pFT->GetInfo()->s_nGroup != 0 );	
	
	pItem = GetItem( pFT->GetInfo()->s_nGroup );
	if( pItem != NULL )
	{
		pItem->SetPrice( pFT, nSize );
		pItem->s_vpElement.PushBack( pEle );		
	}
	else
	{
		pItem = NiNew sCASHSHOP_ITEM;		
		pItem->s_rcArea = rcArea;
		pItem->s_nGroup = pFT->GetInfo()->s_nGroup;				
		 
		cText::sTEXTINFO ti;
		ti.Init( (CFont::eFACE_SIZE)nSize );
		//ti.s_eTextAlign = DT_CENTER;			
		
		// 캐쉬샵 상품컬러 코드 변경
		g_pStringAnalysis->Cut_Parcing( &pItem->s_NameStr, nCutSize, pFT->GetInfo()->s_szName, &ti );
				
		pItem->SetPrice( pFT, nSize );
		pItem->s_vpElement.PushBack( pEle );
		s_vpItem.PushBack( pItem );		

		return true;
	}	

	return false;
}

CsPoint sCASHSHOP_GROUP::Update_Mouse( CsPoint ptPos, CsPoint ptPage, int& nGroup )
{	
	for( int i=ptPage.x; i<ptPage.y; ++i )
	{
		if( IsItem( i ) == false )
			break;

		if( s_vpItem[ i ]->IsRect( ptPos ) == TRUE )
		{
			nGroup = s_vpItem[ i ]->s_nGroup;
			return s_vpItem[ i ]->s_rcArea.GetPos();
		}
	}	

	nGroup = -1;
	return CsPoint::ZERO;
}

#endif