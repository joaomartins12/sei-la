
#include "stdafx.h"
#include "RewardMonthlyUI.h"


cRewardPromotionUI::cRewardPromotionUI(CsPoint pos)
{
	//pEle = NULL;
	pos = CsPoint::ZERO;
	//iSelect = -1;

	InitScript( NULL, pos, CsPoint::ZERO, true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	AddSprite(CsPoint( 222, 55 ), CsPoint( 110, 110 ), "Event\\event_sale.tga");

	cText::sTEXTINFO textInfo;
	textInfo.Init( &g_pEngine->m_FontText, CFont::FS_14, NiColor(1.0f, 0.7f, 0.0f) );
	textInfo.SetText(UISTRING_TEXT( "REWARD_MENU_TEXT3" ).c_str());
	textInfo.s_eTextAlign = DT_CENTER;
	AddText(&textInfo, CsPoint(498, 77));

	textInfo.Init( &g_pEngine->m_FontText, CFont::FS_16, NiColor(0.0f, 1.0f, 1.0f) );
	textInfo.SetText(UISTRING_TEXT( "REWARD_EXPLAIN_TEXT3" ).c_str());
	textInfo.s_eTextAlign = DT_CENTER;
	AddText(&textInfo, CsPoint(498,150));



// 	for(int i=0; i<4; ++i)
// 	{
// 		iCardPos[i] = CsPoint(238+(i*130),303);
// 	}

// 	for(int i=0; i<3; ++i)
// 	{
// 		sCpyImage[i]	= NiNew cSprite;
// 	}
// 
// 	sCpyImage[0]->Init( NULL, CsPoint::ZERO, CsPoint(130, 195), "Event\\event_promotion_01.tga", false );			//프로모션 내용X 
// 	sCpyImage[1]->Init( NULL, CsPoint::ZERO, CsPoint(130, 195), "Event\\event_promotion_02.tga", false );			//프로모션 내용O
// 	sCpyImage[2]->Init( NULL, CsPoint::ZERO, CsPoint(130, 195), "Event\\event_promotion_glow.tga", false );			//프로모션 선택시
// 
// 	CsCashShop* pAddItem = NULL;
// 
// #ifdef VERSION_STEAM
// 	CsVectorPB< int >* VecArray = nsCsFileTable::g_pCashShopMng_Steam->GetSubWin_MainVP_Array( 1 );
// #else
// 	CsVectorPB< int >* VecArray = nsCsFileTable::g_pCashShopMng->GetSubWin_MainVP_Array( 1 );
// #endif
// 
// 	SAFE_POINTER_RET( VecArray );
// 
// 	iCount = VecArray[2].Size();
// 
// 	for(int i=0; i<iCount; ++i)
// 	{
// #ifdef VERSION_STEAM
// 		pAddItem = nsCsFileTable::g_pCashShopMng_Steam->GetCashShop( 1, VecArray[ 2 ][ i ] );
// #else
// 		pAddItem = nsCsFileTable::g_pCashShopMng->GetCashShop( 1, VecArray[ 2 ][ i ] );
// #endif
//  		
//  		if(pAddItem != NULL)
//  			m_Group.Add( pAddItem,	CsRect( iCardPos[i], CsSIZE( 130, 195 ) ), 113 );
// 	}

}

cRewardPromotionUI::~cRewardPromotionUI()
{
	DeleteScript();

// 	for(int i=0; i<3; ++i)
// 	{
// 		SAFE_NIDELETE(sCpyImage[i]);
// 	}
}

void cRewardPromotionUI::Update(float const& fDeltaTime)
{

	//iSelect = -1;
// 
// 	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
// 
// 	for(int i=0; i<iCount; ++i)
// 	{
// 		if(m_Group.s_vpItem[i] != NULL)
// 		{
// 			if(m_Group.s_vpItem[i]->IsRect(ptMouseLocalPos) )
// 			{
// 				iSelect = i;
// 			}
// 		}
// 	}	

	UpdateScript(fDeltaTime);	
}

void cRewardPromotionUI::Render(CsPoint rootPos)
{
	if(!IsVisible())
		return;

	SetPos(rootPos);
	RenderScript();	
	//ResetDeviceScript();

// 	if(!IsVisible() || iCount == 0)
// 	{
// 		return;
// 	}
// 	
// 	RenderScript();	
// 	EndRenderScript();

// 	for(int i=0; i<iCount; ++i)
// 	{
// 		pEle = m_Group.s_vpItem[ i ]->s_vpElement[ 0 ];	
// 
// 		pos = iCardPos[i]+ CsPoint(25,18);
// 
// 		if(pEle != NULL)
// 		{
// 			if(sCpyImage[1])
// 				sCpyImage[1]->Render(GetRootClient() + iCardPos[i]);
// 		}
// 		else
// 		{
// 			if(sCpyImage[0])
// 				sCpyImage[0]->Render(GetRootClient() + iCardPos[i]);
// 		}
// 
// 		int nType = pEle->s_pFT->GetInfo()->s_nIconID / 100;
// 		int nIdx = pEle->s_pFT->GetInfo()->s_nIconID % 100;		
// 		g_pIconMng->RenderCashShopIcon( (ICON_Dynamic::eTYPE)nType, GetRootClient()+pos, nIdx );
// 
// 		if( pEle->s_pFT->GetInfo()->s_nMaskType != 0 )
// 		{
// 			g_pIconMng->RenderCashShopIcon( ICON_Dynamic::CashShop_Mask, GetRootClient() + pos, pEle->s_pFT->GetInfo()->s_nMaskType );			
// 		}
// 
// 		m_Group.s_vpItem[ i ]->s_NameStr.Render( GetRootClient() + CsPoint( 40, 94 ) + pos , 1, DT_CENTER );		
// 	}
// 
// 	if(iSelect != -1)
// 	{
// 		if(sCpyImage[2])
// 			sCpyImage[2]->Render(GetRootClient() + iCardPos[iSelect]);
// 	}
}
