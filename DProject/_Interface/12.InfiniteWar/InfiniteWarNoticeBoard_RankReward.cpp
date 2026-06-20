#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "InfiniteWarNoticeBoard_PreRankList.h"


CInfiniteNBRankRewardWindow::CInfiniteNBRankRewardWindow():m_pCloseBtn(NULL),m_pRewardItemLIst(NULL),m_pTitleName(NULL)
{

}

CInfiniteNBRankRewardWindow::~CInfiniteNBRankRewardWindow()
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE CInfiniteNBRankRewardWindow::GetWindowType()
{ 
	return WT_INFINITEWAR_RANKREWARD_WINDOW; 
}

void CInfiniteNBRankRewardWindow::Destroy()
{
	cBaseWindow::Delete();	
}

void CInfiniteNBRankRewardWindow::DeleteResource()
{
	DeleteScript();	
}

void CInfiniteNBRankRewardWindow::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint winPos( (g_nScreenWidth >> 1) - (632 >> 1), (g_nScreenHeight>>1) - (582>>1) );
	InitScript( "InfiniteNoticeBoard\\Infinite_Borad_popup.tga", winPos, CsPoint( 632, 582 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	m_pTitleName = AddTitle( L" ", CsPoint(0,20), CFont::FS_16 );

	AddSprite( CsPoint(26,74), CsPoint( 544, 38 ), "InfiniteNoticeBoard\\Infinite_Borad_top_Itemlist.tga" );
	{  //L"랭킹" 
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_RANK_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		AddText( &ti, CsPoint( 154, 93 ) );
	}

	{//L"보상 아이템"
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_REWORDITEM_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		AddText( &ti, CsPoint( 427, 93 ) );
	}

	m_pCloseBtn = AddButton( CsPoint( 592, 22 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );	
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteNBRankRewardWindow::_CloseWindow );

	m_pRewardItemLIst = NiNew cListBox;
	if( m_pRewardItemLIst )
	{
		m_pRewardItemLIst->Init( m_pRoot, CsPoint(26, 115), CsPoint( 580, 450 ), NULL, false );
		m_pRewardItemLIst->SetItemHeight( 45 );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint( 0, 0 ), CsPoint( 16, 450 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint( 580, 450 ) ), 10, false );
			m_pRewardItemLIst->SetScrollBar( pScrollBar );
		}

		AddChildControl( m_pRewardItemLIst );
	}
}

void CInfiniteNBRankRewardWindow::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
	_UpdateMoveWindow();
}

bool CInfiniteNBRankRewardWindow::OnEscapeKey()
{
	_CloseWindow(NULL, NULL);
	return true;
}

cBaseWindow::eMU_TYPE CInfiniteNBRankRewardWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( !IsEnableWindow() )
		return muReturn;

	if( m_pCloseBtn && m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	if( m_pRewardItemLIst && m_pRewardItemLIst->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{	
		cListBoxItem const* pOverItem = m_pRewardItemLIst->GetMouseOverItem();
		if( pOverItem )
		{
			cString* pItem = pOverItem->GetItem();
			if( pItem )
			{
				cString::sELEMENT* pEl = pItem->Update_ForMouseOver_Control( CURSOR_ST.GetPos() );
				if( pEl )
				{
					if( cString::sELEMENT::ICON == pEl->GetType() )
					{
						cString::sICON* pCheckBox = (cString::sICON*)pEl;
						cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
						if(pToolTip)
							pToolTip->SetTooltip( pEl->GetWorldPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pCheckBox->s_nIndex_1 );
					}
				}
			}			
		}	
		return muReturn;
	}

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 582, 55 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();			

	return muReturn;
}

void CInfiniteNBRankRewardWindow::_CloseWindow( void* pSender, void* pData )
{
	Close(false);
	// 	if( GetSystem() )
	// 		GetSystem()->OpenCloseNoticeWindow(false);
}

void CInfiniteNBRankRewardWindow::Render()
{
	RenderScript();

	EndRenderScript();
}

void CInfiniteNBRankRewardWindow::ResetDevice()
{	
	ResetDeviceScript();
}

void CInfiniteNBRankRewardWindow::_SetRewardItemlist(int const& nType)
{
	SAFE_POINTER_RET( m_pRewardItemLIst );
	m_pRewardItemLIst->RemoveAllItem();

	std::list<TB::sRankRewardInfo> const* pRankReward = GetSystem()->GetRewardItemListFromType( nType );
	std::list<TB::sRankRewardInfo>::const_iterator it = pRankReward->begin();
	for( ; it != pRankReward->end(); ++it )
	{
		cListBoxItem* pAddItem = NiNew cListBoxItem;
		SAFE_POINTER_RET( pAddItem );
		cString * pItem = NiNew cString;
		SAFE_POINTER_RET( pItem );

		bool bAbs = (0 == it->s_nRankType)?true : false;

		if( bAbs )// 절대 순위
		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER | DT_VCENTER;

			std::wstring msg;
			if( it->s_nRankMin == it->s_nRankMax )
			{
				msg = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_REWORDITEM_RANK_ABSOLUTE_TEXT" );
				DmCS::StringFn::ReplaceAll( msg, L"#absrank#", it->s_nRankMax );
			}
			else
			{
				msg = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_REWORDITEM_RANK_ABSOLUTE_RANGE_TEXT" );
				DmCS::StringFn::ReplaceAll( msg, L"#absrankMin#", it->s_nRankMin );
				DmCS::StringFn::ReplaceAll( msg, L"#absrankMax#", it->s_nRankMax );
			}

			ti.SetText( msg.c_str() );
			pItem->AddText( &ti, CsPoint( 128, 22) );
		}
		else
		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER | DT_VCENTER;

			std::wstring msg;
			if( it->s_nRankMin == it->s_nRankMax )
			{
				msg = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_REWORDITEM_RANK_PERCENT_TEXT" );
				DmCS::StringFn::ReplaceAll( msg, L"#percentrank#", it->s_nRankMax );
			}
			else
			{
				msg = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_REWORDITEM_RANK_PERCENT_RANGE_TEXT" );
				DmCS::StringFn::ReplaceAll( msg, L"#percentrankMin#", it->s_nRankMin );
				DmCS::StringFn::ReplaceAll( msg, L"#percentrankMax#", it->s_nRankMax );
			}
			ti.SetText( msg.c_str() );
			pItem->AddText( &ti, CsPoint( 128, 22) );
		}

		std::list<TB::sRewardItemInfo>::const_iterator itemIt = it->s_listRewardItems.begin();
		for( int n = 0; itemIt != (*it).s_listRewardItems.end(); ++itemIt, ++n )
		{
			cSprite* pIconBackImg = NiNew cSprite;
			if( pIconBackImg )
			{
				pIconBackImg->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), "InfiniteNoticeBoard\\Infinite_NoticeBoard_RewardItemSlot_Back.tga", false );
				cString::sSPRITE* pImg = pItem->AddSprite( pIconBackImg, CsPoint( 284 + (n*45), 0), CsPoint( 44, 44 )  );
				if( pImg )
					pImg->SetAutoPointerDelete(true);
			}

			pItem->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, (*itemIt).s_dwItemCode, (*itemIt).s_nCount, CsPoint(284 + 6 + (n*45), 6 ) );			
		}

		cSprite* pLineBack = NiNew cSprite;
		if( pLineBack )
		{
			pLineBack->Init( NULL, CsPoint::ZERO, CsPoint( 560, 7 ), "InfiniteMatch\\Infinite_Match_line.tga", false );
			cString::sSPRITE* paddImg = pItem->AddSprite( pLineBack, CsPoint(0,38), CsPoint( 560, 7 )  );
			if( paddImg )
				paddImg->SetAutoPointerDelete(true);
		}

		pAddItem->SetItem( pItem );
		m_pRewardItemLIst->AddItem( pAddItem );
	}
}

void CInfiniteNBRankRewardWindow::_SetTitleName( int const& nType )
{
	SAFE_POINTER_RET( m_pTitleName );
	std::wstring title;
	switch( nType )
	{
	case TB::eInfiniteWarRewardType::eWeek:		title = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_REWORDITEM_WEEK_TITLE" ); break;//L"주간 랭킹별 보상 아이템";		
	case TB::eInfiniteWarRewardType::eMonth:	title = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_REWORDITEM_MONTHLY_TITLE" ); break;//L"월간 랭킹별 보상 아이템";		
	case TB::eInfiniteWarRewardType::eSeason:	title = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_REWORDITEM_SEASON_TITLE" ); break;//L"시즌 랭킹별 보상 아이템";		
	}

	m_pTitleName->SetText( title.c_str() );
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CInfiniteNBRankRewardWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eOpenRankRewardItemWindow, this );

	return true;
}

void CInfiniteNBRankRewardWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eOpenRankRewardItemWindow:
		{
			int nType;
			kStream >> nType;
			_SetTitleName(nType);
			_SetRewardItemlist( nType );
		}
		break;
	}
}