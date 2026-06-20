#include "stdafx.h"
#include "NewCashshopPurchaseSubViewer.h"

void NewCashshopPurchaseSubViewer::_MakeString( sCashshop_Item const* pCashItem, cString* pString )
{
	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_12 );

	// °¹¼ö ÀÔ·Â
	switch( pCashItem->nDispType )
	{
	case neCashshopPurchaseType::DT_NONE:
	case neCashshopPurchaseType::DT_PERENNITY:
		break;
	case neCashshopPurchaseType::DT_COUNT:
	case neCashshopPurchaseType::DT_DAY:
		{
			ti.SetText( L" " );
			cString::sTEXT* pFirstEleText = pString->AddText( &ti );
			pFirstEleText->s_ptSize.x = 10;		

			ti.SetText( pCashItem->nDispCount );
			cString::sTEXT* pEleText = pString->AddText( &ti );
			pFirstEleText->s_ptSize.x -= pEleText->s_Text.GetStringSize().x;
			pEleText->s_ptSize.x += 3;

			pString->CalMaxSize();
		}
		break;

	default:
		assert_cs( false );
	}

	switch( pCashItem->nDispType )
	{
	case neCashshopPurchaseType::DT_NONE:
		{
		}			
		break;

#ifndef VERSION_KOR
	case neCashshopPurchaseType::DT_COUNT:
		{
			cSprite* pkEachIcon = NiNew cSprite;
			if( pkEachIcon )
				pkEachIcon->Init( NULL, CsPoint::ZERO, CsPoint( 16, 10 ), "Cash\\Shop\\Each.tga", false );

			cString::sSPRITE* pEachIcon = pString->AddSprite( pkEachIcon, CsPoint( 0, 3 ) );
			if( pEachIcon )
				pEachIcon->SetAutoPointerDelete(true);
		}
		break;
	case neCashshopPurchaseType::DT_DAY:
		{
			cSprite* pkDayIcon = NiNew cSprite;
			if( pkDayIcon )
				pkDayIcon->Init( NULL, CsPoint::ZERO, CsPoint( 14, 15 ), "Cash\\Shop\\Day.tga", false );

			cString::sSPRITE* pDayIcon = pString->AddSprite( pkDayIcon );
			if( pDayIcon )
				pDayIcon->SetAutoPointerDelete(true);
		}
		break;
	case neCashshopPurchaseType::DT_PERENNITY:
		{
			cSprite* pkLongIcon = NiNew cSprite;
			if( pkLongIcon )
				pkLongIcon->Init( NULL, CsPoint::ZERO, CsPoint( 17, 9 ), "Cash\\Shop\\Long.tga", false );

			cString::sSPRITE* pLongIcon = pString->AddSprite( pkLongIcon, CsPoint( 0, 4 ) ); 
			if( pLongIcon )
				pLongIcon->SetAutoPointerDelete(true);
		}
		break;
#else
	case neCashshopPurchaseType::DT_COUNT:
		{
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_UNIT" ).c_str() );
			pString->AddText(&ti);
		}
		break;
	case neCashshopPurchaseType::DT_DAY:
		{
			ti.SetText( UISTRING_TEXT( "COMMON_TXT_DAY" ).c_str() );
			pString->AddText(&ti);
		}
		break;
	case neCashshopPurchaseType::DT_PERENNITY:
		{
			ti.SetText( UISTRING_TEXT( "CASHSHOP_PURCHASE_PERENNITY" ).c_str() );
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

	ti.s_Color = FONT_YELLOW;

	ti.SetText( L" " );
	pString->AddText( &ti );

	std::wstring sPrice;
#ifdef VERSION_STEAM	
	DmCS::StringFn::Format( sPrice, L"%.2f", pCashItem->nRealSellingPrice * 0.01 );	
#else
	sPrice = DmCS::StringFn::getNumberFormatW(pCashItem->nRealSellingPrice);
#endif

	ti.SetText(sPrice.c_str());

	cString::sTEXT* pEleText = pString->AddText( &ti );
	pFirstEleText->s_ptSize.x -= pEleText->s_Text.GetStringSize().x;
	pEleText->s_ptSize.x += 3;

	cSprite* pkCashIcon = NiNew cSprite;
	if( pkCashIcon )
	{
#ifndef VERSION_STEAM
		pkCashIcon->Init( NULL, CsPoint::ZERO, CsPoint(24, 24),  "Cash\\NewCashShop\\Store_icon_C.tga", false );
#else
		pkCashIcon->Init( NULL, CsPoint::ZERO, CsPoint(24, 24),  "Cash\\NewCashShop\\Store_icon_S.tga", false );
#endif
		cString::sSPRITE* pCashIcon = pString->AddSprite( pkCashIcon, CsPoint( 2, -2 ), CsPoint( 18, 18 ) );
		if( pCashIcon )
			pCashIcon->SetAutoPointerDelete(true);
	}
}