#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "InfiniteWarNoticeBoard_PreRankList.h"


CInfiniteNBPreRankListWindow::CInfiniteNBPreRankListWindow()
:m_pCloseBtn(NULL),m_pRankList(NULL),m_pTitleName(NULL),m_pMyRank(NULL)
{

}

CInfiniteNBPreRankListWindow::~CInfiniteNBPreRankListWindow()
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE CInfiniteNBPreRankListWindow::GetWindowType()
{ 
	return WT_INFINITEWAR_PRERANKLIST_WINDOW; 
}

void CInfiniteNBPreRankListWindow::Destroy()
{
	cBaseWindow::Delete();	
}

void CInfiniteNBPreRankListWindow::DeleteResource()
{
	DeleteScript();	
}

void CInfiniteNBPreRankListWindow::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	CsPoint winPos( (g_nScreenWidth >> 1) - (632 >> 1), (g_nScreenHeight>>1) - (582>>1) );
	InitScript( "InfiniteNoticeBoard\\Infinite_Borad_popup.tga", winPos, CsPoint( 632, 582 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );
	m_pTitleName = AddTitle( L" ", CsPoint(0,20), CFont::FS_16 );

	AddSprite( CsPoint(26,74), CsPoint( 544, 38 ), "InfiniteNoticeBoard\\Infinite_Borad_top_Previously.tga" );
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText(UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_RANK_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		AddText( &ti, CsPoint( 52, 93 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_TAMERNAME_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		AddText( &ti, CsPoint( 171, 93 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_GUILD_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		AddText( &ti, CsPoint( 375, 93 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_POINT_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		AddText( &ti, CsPoint( 529, 93 ) );
	}

	m_pCloseBtn = AddButton( CsPoint( 592, 22 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "CommonUI\\CommonUI_close.tga" );	
	if( m_pCloseBtn )
		m_pCloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfiniteNBPreRankListWindow::_CloseWindow );

	m_pRankList = NiNew cListBox;
	if( m_pRankList )
	{
		m_pRankList->Init( m_pRoot, CsPoint(26, 125), CsPoint( 580, 400 ), NULL, false );
		m_pRankList->SetItemHeight( 40 );
		//m_pRankList->SetBackGorundImg(" ");
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint( 0, 0 ), CsPoint( 16, 400 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint( 580, 400 ) ), 10, false );
			m_pRankList->SetScrollBar( pScrollBar );
		}

		AddChildControl( m_pRankList );
	}

	AddSprite( CsPoint(227,529), CsPoint( 342, 37 ), "InfiniteNoticeBoard\\Infinite_Borad_ranking.tga" );
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_PREVIOUSLY_MYRANK_TEXT" ).c_str() );		
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		AddText( &ti, CsPoint( 306, 549 ) );
	}
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		m_pMyRank = AddText( &ti, CsPoint( 477, 549 ) );
	}
}

void CInfiniteNBPreRankListWindow::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
	_UpdateMoveWindow();
}

bool CInfiniteNBPreRankListWindow::OnEscapeKey()
{
	_CloseWindow(NULL, NULL);
	return true;
}

cBaseWindow::eMU_TYPE CInfiniteNBPreRankListWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( !IsEnableWindow() )
		return muReturn;

	if( m_pCloseBtn && m_pCloseBtn->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;
	
	if( m_pRankList && m_pRankList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return muReturn;
	
	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 632, 55 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();			

	return muReturn;
}

void CInfiniteNBPreRankListWindow::_CloseWindow( void* pSender, void* pData )
{
	Close(false);
}

void CInfiniteNBPreRankListWindow::Render()
{
	RenderScript();

	EndRenderScript();
}

void CInfiniteNBPreRankListWindow::ResetDevice()
{	
	ResetDeviceScript();
}

void CInfiniteNBPreRankListWindow::_SetPreviouslyRankData( int const& nType )
{
	SAFE_POINTER_RET( m_pRankList );
	m_pRankList->RemoveAllItem();

	CInfiniteWarRankRewardContents::sRankInfos const* pInfo = GetSystem()->GetPreviouslyRankerInfoByType(nType);
	SAFE_POINTER_RET( pInfo );
	
	_SetMyRank(pInfo->nPreviouslyMyRanking);	
	
	std::list<CInfiniteWarRankRewardContents::sPreviouslyRankerInfo>::const_iterator it = pInfo->PriviouslyData.listPreviouslyRanker.begin();
	for( ; it != pInfo->PriviouslyData.listPreviouslyRanker.end(); ++it )
	{
		cListBoxItem* pAddItem = NiNew cListBoxItem;
		SAFE_POINTER_RET( pAddItem );
		cString * pItem = NiNew cString;
		SAFE_POINTER_RET( pItem );

		if( (*it).nRank > 0 && (*it).nRank < 4 )
		{
			cImage* pTopRankImg = NiNew cImage;
			if( pTopRankImg )
			{
				pTopRankImg->Init( NULL, CsPoint(2,2), CsPoint( 48, 42 ), "InfiniteNoticeBoard\\Infinite_Borad_top3.tga", false );
				pTopRankImg->SetTexToken( CsPoint( 0, 42 ) );
				cString::sIMAGE* paddImg = pItem->AddImage( pTopRankImg, (*it).nRank - 1, CsPoint::ZERO, CsPoint( 48, 42 )  );	
				if( paddImg )
					paddImg->SetAutoPointerDelete(true);
			}
		}
		else
		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;
			ti.SetText( (*it).nRank );
			pItem->AddText( &ti, CsPoint( 26, 13) );
		}

		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;
			ti.SetText( (*it).wsTamerName.c_str() );
			pItem->AddText( &ti, CsPoint( 145, 13) );
		}

		if( !(*it).wsGuildName.empty() )
		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12, NiColor( 243.0f / 255.0f, 228.0f / 255.0f, 42.0f / 255.0f ) );
			ti.s_eTextAlign = DT_CENTER;
			wstring guild;
			DmCS::StringFn::Format( guild, L"<%s>", (*it).wsGuildName.c_str() );
			ti.SetText( guild.c_str() );
			pItem->AddText( &ti, CsPoint( 349, 13) );

		}

		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;
			std::wstring wsPoint = DmCS::StringFn::getNumberFormatW( (*it).dwPoint );
			ti.SetText( wsPoint.c_str() );
			pItem->AddText( &ti, CsPoint( 503, 13) );
		}
		cSprite* pLineBack = NiNew cSprite;
		if( pLineBack )
		{
			pLineBack->Init( NULL, CsPoint::ZERO, CsPoint( 560, 7 ), "InfiniteMatch\\Infinite_Match_line.tga", false );
			cString::sSPRITE* paddImg = pItem->AddSprite( pLineBack, CsPoint(0,33), CsPoint( 560, 7 )  );
			if( paddImg )
				paddImg->SetAutoPointerDelete(true);
		}
		pAddItem->SetItem( pItem );
		m_pRankList->AddItem(pAddItem);
	}
}

void CInfiniteNBPreRankListWindow::_SetTitleName( int const& nType )
{
	SAFE_POINTER_RET( m_pTitleName );
	std::wstring title;
	switch( nType )
	{
	case TB::eInfiniteWarRewardType::eWeek:		title = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_WEEK_TITLE" );		break;// L"전주 랭킹"
	case TB::eInfiniteWarRewardType::eMonth:	title = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_MONTHLY_TITLE" );	break;//L"전월 랭킹"
	case TB::eInfiniteWarRewardType::eSeason:	title = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_POPUP_SEASON_TITLE" );	break;//L"전시즌 랭킹"
	}

	m_pTitleName->SetText( title.c_str() );
}

void CInfiniteNBPreRankListWindow::_SetMyRank( int const& nMyPreviouslyRank )
{
	SAFE_POINTER_RET( m_pMyRank );

	m_pMyRank->SetText( nMyPreviouslyRank );
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CInfiniteNBPreRankListWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpdate_Previously_RankerList, this );	
	return true;
}

void CInfiniteNBPreRankListWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eUpdate_Previously_RankerList:
		{
			int nType;
			kStream >> nType;
			_SetTitleName(nType);
			_SetPreviouslyRankData(nType);
		}
		break;
	}
}