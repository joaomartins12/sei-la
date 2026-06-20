#include "stdafx.h"
#include "InfiniteWarNoticeBoard_HallOfFame.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CInfinitewarNoticeBoardHallOfFameUI::CInfinitewarNoticeBoardHallOfFameUI() : m_pRankList(NULL),m_pSeasonList(NULL)
{
}

CInfinitewarNoticeBoardHallOfFameUI::~CInfinitewarNoticeBoardHallOfFameUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

void CInfinitewarNoticeBoardHallOfFameUI::Destroy()
{
	DeleteScript();
}

bool CInfinitewarNoticeBoardHallOfFameUI::Create(cWindow* pRoot)
{
	InitScript( pRoot, NULL, CsPoint( 24, 104 ), CsPoint( 972, 487 ), false );
	AddSprite( CsPoint(9,8), CsPoint( 574, 38 ), "InfiniteNoticeBoard\\Infinite_Borad_top_Fame.tga" );

	{	// 랭킹
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_RANK_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 35, 20 ) );
	}

	{	//테이머 이름
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12);
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_TAMERNAME_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 164, 20 ) );
	}

	{//소속 길드
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_GUILD_TEXT" ).c_str() );
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 385, 20 ) );
	}

	{//L"포인트"
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_RANK_BASE_POINT_TEXT" ).c_str()  );
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 542, 20 ) );
	}

	m_pRankList = NiNew cListBox;
	if( m_pRankList )
	{
		m_pRankList->Init( m_pRoot, CsPoint(11, 50), CsPoint( 680, 430 ), NULL, false );
		m_pRankList->SetItemHeight( 43 );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint( 0, 0 ), CsPoint( 16, 430 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint( 680, 430 ) ), 10, false );
			m_pRankList->SetScrollBar( pScrollBar );
		}

		AddChildControl( m_pRankList );
	}

	m_pSeasonList = NiNew cListBox;
	if( m_pSeasonList )
	{
		m_pSeasonList->Init( m_pRoot, CsPoint(722, 20), CsPoint( 244, 450 ), NULL, false );
		m_pSeasonList->SetAutoItemSelectStateChange(true);
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint( 0, 0 ), CsPoint( 16, 475 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint::ZERO, CsPoint( 244, 450 ) ), 15, false );
			m_pSeasonList->SetScrollBar( pScrollBar );
		}
		m_pSeasonList->AddEvent( cListBox::LIST_SELECT_CHANGE_ITEM, this, &CInfinitewarNoticeBoardHallOfFameUI::_ResetList );
		AddChildControl( m_pSeasonList );
	}
	_UpdatePreSeasonList();
	return true;
}

BOOL CInfinitewarNoticeBoardHallOfFameUI::UpdateMouse()
{
	if( !IsVisible() || !IsEnableWindow() )
		return FALSE;

	if( m_pRankList && m_pRankList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return TRUE;

	if( m_pSeasonList && m_pSeasonList->Update_ForMouse( CURSOR_ST.GetPos() ) )
		return TRUE;

	return FALSE;
}

BOOL CInfinitewarNoticeBoardHallOfFameUI::UpdateKeyboard(const MSG& p_kMsg)
{
	return FALSE;
}

void CInfinitewarNoticeBoardHallOfFameUI::_UpdatePreSeasonList()
{
	SAFE_POINTER_RET( m_pSeasonList );
	m_pSeasonList->RemoveAllItem();

	std::list<int> seasonList;
	GetSystem()->GetHallofFameSeasonList( seasonList );
	if( seasonList.empty() )
		return;

	std::list<int>::const_iterator it = seasonList.begin();
	for( ; it != seasonList.end(); ++it )
	{
		std::wstring name = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_SEASON_COMBO_TEXT" );		
		DmCS::StringFn::ReplaceAll(name, L"#season#", (*it));
		_AddPreSeasonList( name, (*it) );
	}

	m_pSeasonList->SetSelectedItemFromIdx(0);
}

void CInfinitewarNoticeBoardHallOfFameUI::_ResetList(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cListBoxItem* pSeletedItem = static_cast<cListBoxItem*>(pData);
	sSeasonIndex* pSeasonData = dynamic_cast<sSeasonIndex*>(pSeletedItem->GetUserData());
	SAFE_POINTER_RET( pSeasonData );

	GetSystem()->SelectedHallofFameBySeason( pSeasonData->m_nIdx );
}

void CInfinitewarNoticeBoardHallOfFameUI::_UpdateRenkerList( int const& nSeasonIdx )
{
	SAFE_POINTER_RET( m_pSeasonList );
	SAFE_POINTER_RET( m_pRankList );

	cListBoxItem const* pSelItem = m_pSeasonList->GetSelectedItem();
	SAFE_POINTER_RET( pSelItem );
	sSeasonIndex* pSeasonData = dynamic_cast<sSeasonIndex*>(pSelItem->GetUserData());
	SAFE_POINTER_RET( pSeasonData );

	if( nSeasonIdx != pSeasonData->m_nIdx )
		return;

	m_pRankList->RemoveAllItem();

	std::list<CInfiniteWarRankRewardContents::sFameRankerInfo> const* pRankerList = GetSystem()->GetHallofFameRankerList( nSeasonIdx );
	SAFE_POINTER_RET( pRankerList );

	std::list<CInfiniteWarRankRewardContents::sFameRankerInfo>::const_iterator it = pRankerList->begin();
	for( ; it != pRankerList->end(); ++it )
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
				pTopRankImg->Init( NULL, CsPoint::ZERO, CsPoint( 48, 42 ), "InfiniteNoticeBoard\\Infinite_Borad_top3.tga", false );
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
			pItem->AddText( &ti, CsPoint( 24, 15) );
		}

		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;
			ti.SetText( (*it).wsTamerName.c_str() );
			pItem->AddText( &ti, CsPoint( 153, 15) );
		}

		if( !(*it).wsGuildName.empty() )
		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12, NiColor( 243.0f / 255.0f, 228.0f / 255.0f, 42.0f / 255.0f ) );
			ti.s_eTextAlign = DT_CENTER;
			wstring guild;
			DmCS::StringFn::Format( guild, L"<%s>", (*it).wsGuildName.c_str() );
			ti.SetText( guild.c_str() );
			pItem->AddText( &ti, CsPoint( 374, 15) );
		}

		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.s_eTextAlign = DT_CENTER;
			std::wstring wsPoint = DmCS::StringFn::getNumberFormatW( (*it).dwPoint );
			ti.SetText( wsPoint.c_str() );
			pItem->AddText( &ti, CsPoint( 531, 15) );
		}

		cSprite* pLineBack = NiNew cSprite;
		if( pLineBack )
		{
			pLineBack->Init( NULL, CsPoint::ZERO, CsPoint( 680, 7 ), "InfiniteMatch\\Infinite_Match_line.tga", false );
			cString::sSPRITE* paddImg = pItem->AddSprite( pLineBack, CsPoint(0,36), CsPoint( 680, 7 )  );
			if( paddImg )
				paddImg->SetAutoPointerDelete(true);
		}

		pAddItem->SetItem( pItem );
		m_pRankList->AddItem( pAddItem );
	}
}

void CInfinitewarNoticeBoardHallOfFameUI::_AddPreSeasonList( std::wstring const& name, int const& nSeasonIdx )
{
	SAFE_POINTER_RET( m_pSeasonList );

	cListBoxItem* pAddItem = NiNew cListBoxItem;
	SAFE_POINTER_RET( pAddItem );
	cString * pItem = NiNew cString;
	SAFE_POINTER_RET( pItem );

	// 배경 버튼
	cButton* pSlotBtn = NiNew cButton;
	if( pSlotBtn )
	{
		pSlotBtn->Init( NULL, CsPoint::ZERO, CsPoint( 224, 30 ), "InfiniteNoticeBoard\\Infinite_Borad_radio_btn.tga", false );
		pSlotBtn->SetTexToken( CsPoint( 0, 30 ));
		cString::sBUTTON* paddImg = pItem->AddButton( pSlotBtn, 0, CsPoint::ZERO, CsPoint( 224, 30 ) );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);
	}

	cText::sTEXTINFO ti;
	ti.Init(CFont::FS_12 );
	ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
	ti.SetText( name.c_str() );
	pItem->AddText( &ti, CsPoint( 112, 15) );
	pAddItem->SetItem( pItem );
	pAddItem->SetUserData( new sSeasonIndex(nSeasonIdx) );
	//m_pSeasonList->AddItem( pAddItem );
	m_pSeasonList->InsertItem(0, pAddItem );
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////

bool CInfinitewarNoticeBoardHallOfFameUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpdate_FameRankerList, this );	
	GetSystem()->Register( SystemType::eUpdate_FameSeasonList, this );	

	return true;
}

void CInfinitewarNoticeBoardHallOfFameUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eUpdate_FameRankerList:
		{
			int nSeasonIdx;
			kStream >> nSeasonIdx;
			_UpdateRenkerList( nSeasonIdx );
		}
		break;
	case SystemType::eUpdate_FameSeasonList:
		{
			_UpdatePreSeasonList();
		}
		break;
	}
}