#include "stdafx.h"
#include "InfiniteWarNoticeBoard_Daily.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

//////////////////////////////////////////////////////////////////////////

CInfinitewarNoticeBoardDailyUI::sRewardUIContiner::sRewardUIContiner()
:s_pNeedPointText(NULL),s_nNeedPoint(0)
{

}

CInfinitewarNoticeBoardDailyUI::sRewardUIContiner::~sRewardUIContiner()
{

}

void CInfinitewarNoticeBoardDailyUI::sRewardUIContiner::SetRewardItemGet(DWORD const& dwTotalPoint) 
{
	bool bIsGet = (s_nNeedPoint <= dwTotalPoint)?true:false;
	if( s_pNeedPointText )
	{
		std::wstring pointMsg = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_DAILY_NEED_POINT_TEXT" );
		DmCS::StringFn::ReplaceAll( pointMsg, L"#point#", s_nNeedPoint );

		std::wstring result;
		NiColor niTextColor;
		if( bIsGet )
		{
			result = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_DAILY_REWARDITEM_COMPLETE_TEXT" );
			niTextColor = NiColor( 0.0f, 1.0f, 1.0f );
		}
		else
		{
			result = UISTRING_TEXT( "INFINITEWAR_NOTICEBOARD_WINDOW_DAILY_REWARDITEM_MISSINGPOINT_TEXT" );
			niTextColor = NiColor( 0.5f, 0.5f, 0.5f );
		}

		DmCS::StringFn::ReplaceAll( pointMsg, L"#result#", result );
		s_pNeedPointText->SetColor( niTextColor );
		s_pNeedPointText->SetText( pointMsg.c_str() );
	}
	
	if( bIsGet )
	{
		std::list<cString::sSPRITE*>::iterator it =	s_pCheckImgs.begin();
		for( ; it != s_pCheckImgs.end(); ++it )
		{
			SAFE_POINTER_CON( (*it) );
			(*it)->SetVisible( true );
		}
	}
}


//////////////////////////////////////////////////////////////////////////

CInfinitewarNoticeBoardDailyUI::CInfinitewarNoticeBoardDailyUI() : 
m_pRewardItemsList(NULL),m_pTodayInsertedPoint(NULL),m_pNeedItemName(NULL),
m_pInsertPointBtn(NULL),m_pLeftBtn(NULL),m_pPreviousBtn(NULL),m_pRightBtn(NULL),m_pNextBtn(NULL),
m_pEditCount(NULL),m_dwMaxNeedItemCount(0)
{
}

CInfinitewarNoticeBoardDailyUI::~CInfinitewarNoticeBoardDailyUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

void CInfinitewarNoticeBoardDailyUI::Destroy()
{
	m_ControlsContiner.clear();
	DeleteScript();
}

bool CInfinitewarNoticeBoardDailyUI::Create(cWindow* pRoot)
{
	InitScript( pRoot, NULL, CsPoint( 24, 104 ), CsPoint( 972, 487 ), false );
	AddSprite( CsPoint::ZERO, CsPoint( 705, 487 ), "InfiniteNoticeBoard\\Infinite_NoticeBoard_Today_LBack.tga" );
	AddSprite( CsPoint(714, 0), CsPoint( 258, 487 ), "InfiniteNoticeBoard\\Infinite_NoticeBoard_Today_RBack.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, NiColor(0.0f,0.7176f,0.93f) );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_DAILY_NOTICE_TEXT").c_str() );//닷트본부 공지사항
		AddText( &ti, CsPoint( 187, 12 ) );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.SetText(L" ");
		ti.SetLineGabHeight( 5 );
		cText* pMsg = AddText( &ti, CsPoint(20, 47));
		pMsg->SetText( UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_DAILY_NOTICE_MSG").c_str(), 333 );		
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12, NiColor(0.0f,0.7176f,0.93f) );
		ti.SetText( UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_DAILY_TODAYREWARD_TEXT").c_str() );//오늘의 보상
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 535, 12 ) );	
	}

	m_pRewardItemsList = NiNew cListBox;
	if( m_pRewardItemsList )
	{
		m_pRewardItemsList->Init( m_pRoot, CsPoint(390, 47), CsPoint( 310, 428 ), NULL, false );
		m_pRewardItemsList->SetItemHeight( 80 );
		m_pRewardItemsList->SetItemGab( CsPoint(0,6) );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if(pScrollBar)
		{
			pScrollBar->Init( cScrollBar::TYPE_SEALMASTER, NULL, CsPoint( 0, 0 ), CsPoint( 16, 260 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint(0,0), CsPoint( 310, 428 ) ), 5, false );
			m_pRewardItemsList->SetScrollBar( pScrollBar );
		}

		AddChildControl( m_pRewardItemsList );
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_DAILY_TODAY_TOTALPOINT_TEXT").c_str() );//오늘 투입한 총 포인트
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 843, 12 ) );	
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_24, NiColor(0.0f,1.0f,0.0f) );
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER;
		m_pTodayInsertedPoint = AddText( &ti, CsPoint( 843, 65 ) );	
	}

	//////////////////////////////////////////////////////////////////////////
	// 아이템 
	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( L" " );
		ti.s_eTextAlign = DT_CENTER;
		m_pNeedItemName = AddText( &ti, CsPoint( 843, 133 ) );	
	}

	AddSprite( CsPoint(793, 155), CsPoint(100,112),"InfiniteNoticeBoard\\Infinite_NoticeBoard_InsertItemSlot.tga" );

	m_pLeftBtn = AddButton( CsPoint(790, 238), CsPoint(20, 24), CsPoint( 0, 24 ),"InfiniteNoticeBoard\\Infinite_NoticeBoard_LArrow.tga" );
	if( m_pLeftBtn )
		m_pLeftBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardDailyUI::_LeftBtnEvent );

	m_pPreviousBtn = AddButton( CsPoint(767, 238), CsPoint(24, 24), CsPoint( 0, 24 ),"InfiniteNoticeBoard\\Infinite_NoticeBoard_LArrow_Previous.tga" );
	if( m_pPreviousBtn )
		m_pPreviousBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardDailyUI::_PreviousBtnEvent );

	m_pRightBtn = AddButton( CsPoint(878, 238), CsPoint(20, 24), CsPoint( 0, 24 ),"InfiniteNoticeBoard\\Infinite_NoticeBoard_RArrow.tga" );
	if( m_pRightBtn )
		m_pRightBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardDailyUI::_RightBtnEvent );

	m_pNextBtn = AddButton( CsPoint(896, 238), CsPoint(24, 24), CsPoint( 0, 24 ),"InfiniteNoticeBoard\\Infinite_NoticeBoard_RArrow_Next.tga" );
	if( m_pNextBtn )
		m_pNextBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardDailyUI::_NextBtnEvent );

	// 에딧박스	
	m_pEditCount = NiNew cEditBox;
	if( m_pEditCount )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12, NiColor::WHITE );
		ti.SetText( _T( "0" ) );
		ti.s_bOutLine = false;
		ti.s_eTextAlign = DT_CENTER;

		m_pEditCount->Init( GetRoot(), CsPoint( 810, 236 ), CsPoint( 68, 24 ), &ti, false );
		m_pEditCount->SetFontLength( 6 );
		m_pEditCount->EnableUnderline( ti.s_Color );
		m_pEditCount->AddEvent( cEditBox::eEditbox_SetFocus, this, &CInfinitewarNoticeBoardDailyUI::_SetEditBoxFocus);
		m_pEditCount->AddEvent( cEditBox::eEditbox_ReleaseFocus, this, &CInfinitewarNoticeBoardDailyUI::_ReleaseEditBoxFocus);
		AddChildControl( m_pEditCount );	
	}


	m_pInsertPointBtn = AddButton( CsPoint(727,276), CsPoint( 230, 54 ), CsPoint( 0, 54 ), "CommonUI\\Menu_btn.tga", cWindow::SD_Bu3 );
	if( m_pInsertPointBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
		ti.SetText( UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_DAILY_INSERTPOINT_BTN").c_str() );//포인트 투입하기
		ti.s_eTextAlign = DT_CENTER;
		m_pInsertPointBtn->SetText( &ti );
		m_pInsertPointBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CInfinitewarNoticeBoardDailyUI::_InsertPoint);
	}

	{
		cText::sTEXTINFO ti;
		ti.Init(CFont::FS_12 );
		ti.SetText( UISTRING_TEXT("INFINITEWAR_NOTICEBOARD_WINDOW_DAILY_TODAY_POINTRESET_REMAINTIME_TEXT").c_str() );//초기화 남은 시간
		ti.s_eTextAlign = DT_CENTER;
		AddText( &ti, CsPoint( 843, 390 ) );	
	}

	m_pTimerBar = NiNew cUI_TimerProgressBar;
	if( m_pTimerBar )
	{
		m_pTimerBar->Init(GetRoot(), CsPoint(725, 415), CsPoint(243, 40), false);

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_24 );
		ti.s_Color = NiColor(0.86f,0.45f,0.0f);
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		ti.SetText(L"00 : 00 : 00");	
		m_pTimerBar->SetStringControl( &ti );
		m_pTimerBar->SetShowValueType( cUI_TimerProgressBar::eRemainingTime );			
		m_pTimerBar->SetTimerText( L" : ", L" : ", L" : ", L"");
		m_pTimerBar->SetTimerType(cUI_TimerProgressBar::eHourMinSec);
		m_pTimerBar->SetRenderText(true);
		m_pTimerBar->AddEvent( cUI_ProgressBar::eProgressbar_Done, this, &CInfinitewarNoticeBoardDailyUI::_SendRequestDataReset );
		AddChildControl( m_pTimerBar );
	}

	_UpdateRewardItemList();
	_UpdateTodayTotalInjectedPoint( GetSystem()->GetTodayTotalInjectedPoint() );
	_SetNeedItem( GetSystem()->GetCurrentNeedItemCode() );
	_ResetRemainTime( GetSystem()->GetRestRemainTime() );
	m_dwMaxNeedItemCount = GetSystem()->GetCurrentNeedItemCount();

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 보상 아이템 리스트를 만든다.
void CInfinitewarNoticeBoardDailyUI::_UpdateRewardItemList()
{
	SAFE_POINTER_RET( m_pRewardItemsList );
	m_pRewardItemsList->RemoveAllItem();
	m_ControlsContiner.clear();
	CsInfiniteWarMng::DailyRewardItems const * pRewardList = GetSystem()->GetTodayRewardItemLIst();
	SAFE_POINTER_RET( pRewardList );

	DWORD dwTotalPoint = GetSystem()->GetTodayTotalInjectedPoint();
	CsInfiniteWarMng::DailyRewardItems_CIT it = pRewardList->begin();
	for( ; it != pRewardList->end(); ++it )
	{
		cListBoxItem* pAddItem = NiNew cListBoxItem;
		SAFE_POINTER_RET( pAddItem );
		cString * pItem = NiNew cString;
		SAFE_POINTER_RET( pItem );

		sRewardUIContiner addUI;
		addUI.s_nNeedPoint = (*it).s_nNeedPoint;

		{
			cText::sTEXTINFO ti;
			ti.Init(CFont::FS_12 );
			ti.SetText( L" " );
			addUI.s_pNeedPointText = pItem->AddText( &ti, CsPoint( 5, 5) );
		}

		std::list<TB::sRewardItemInfo>::const_iterator itemIt = (*it).s_listRewardItems.begin();
		for( int n = 0; itemIt != (*it).s_listRewardItems.end(); ++itemIt, ++n )
		{
			cSprite* pIconBackImg = NiNew cSprite;
			if( pIconBackImg )
			{
				pIconBackImg->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), "InfiniteNoticeBoard\\Infinite_NoticeBoard_RewardItemSlot_Back.tga", false );
				cString::sSPRITE* pImg = pItem->AddSprite( pIconBackImg, CsPoint( 5 + (n*45), 25), CsPoint( 44, 44 )  );
				if( pImg )
					pImg->SetAutoPointerDelete(true);;
			}

			pItem->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, (*itemIt).s_dwItemCode, (*itemIt).s_nCount, CsPoint( 11 + (n*45), 31 ) );

 			cSprite* pCheckImg = NiNew cSprite;
 			if( pCheckImg )
 			{
 				pCheckImg->Init( NULL, CsPoint::ZERO, CsPoint( 44, 44 ), "InfiniteNoticeBoard\\Infinite_NoticeBoard_check.tga", false );
 				cString::sSPRITE* pImg = pItem->AddSprite( pCheckImg, CsPoint( 5 + (n*45), 25), CsPoint( 44, 44 )  );
 				if( pImg )
				{
 					pImg->SetAutoPointerDelete(true);
					pImg->SetVisible( false );
					addUI.s_pCheckImgs.push_back( pImg );
				}
 			}
		}

		m_ControlsContiner.push_back( addUI );
		pAddItem->SetItem( pItem );
		m_pRewardItemsList->AddItem( pAddItem );
	}
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// 투입한 포인트에 따라 UI에 보유 포인트 갱신 및 보상 아이템 리스트 UI 업데이트
void CInfinitewarNoticeBoardDailyUI::_UpdateTodayTotalInjectedPoint( DWORD const& dwPoint )
{
	SAFE_POINTER_RET( m_pTodayInsertedPoint );
	std::wstring pointStr = DmCS::StringFn::getNumberFormatW( dwPoint );		
	m_pTodayInsertedPoint->SetText( pointStr.c_str() );

	std::list<sRewardUIContiner>::iterator it = m_ControlsContiner.begin();
	for( ; it != m_ControlsContiner.end(); ++it )
		(*it).SetRewardItemGet( dwPoint );
}
//////////////////////////////////////////////////////////////////////////

void CInfinitewarNoticeBoardDailyUI::_SetNeedItem(DWORD const& dwItemCode)
{
	m_dwRenderItemCode = dwItemCode;
	if( m_pNeedItemName )
	{
		std::wstring wItemName = GetSystem()->GetItemName( dwItemCode );
		m_pNeedItemName->SetText( wItemName.c_str() );
	}
}

BOOL CInfinitewarNoticeBoardDailyUI::UpdateMouse()
{
	if( !IsVisible() || !IsEnableWindow() )
		return FALSE;

	if( m_pRewardItemsList && m_pRewardItemsList->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{	
		cListBoxItem const* pOverItem = m_pRewardItemsList->GetMouseOverItem();
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
		return TRUE;
	}

	if( m_pInsertPointBtn && m_pInsertPointBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pLeftBtn && m_pLeftBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pPreviousBtn && m_pPreviousBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pRightBtn && m_pRightBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pNextBtn && m_pNextBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pEditCount && m_pEditCount->Update_ForMouse( CURSOR_ST.GetPos() ) == cEditBox::ACTION_CLICK)
		return TRUE;

	if( m_dwRenderItemCode != 0 )
	{
		CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
		CsRect rc( CsPoint( 827, 189 ), CsPoint( 827, 189 ) + CsPoint( 32, 32 ) );
		if( rc.PtInRect( ptMouseLocalPos ) && TOOLTIPMNG_STPTR )
		{
			cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
			if( pToolTip )
				pToolTip->SetTooltip( GetTotalPosition() +CsPoint( 827, 189 ),  CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_dwRenderItemCode );
		}
	}

	// 아이템 갯수 입력창 활성화시. 엔터키 or ESC키 누르면 활성화 해제.
	if( GLOBALINPUT_ST.IsEscape() && m_pEditCount->IsFocus() )
	{
		m_pEditCount->ReleaseFocus(false);
	}
	else if( GLOBALINPUT_ST.IsEnter() && m_pEditCount->IsFocus() )
	{
		m_pEditCount->ReleaseFocus(false);
		SendMessage(GAMEAPP_ST.GetHWnd(), WM_KEYDOWN, VK_RETURN, 0);
	}
	else if( m_pEditCount->Update_ForMouse( CsPoint::ZERO ) == cEditBox::ACTION_NONE )
	{
		if( CURSOR_ST.GetButtonState() == CURSOR::LBUTTON_DOWN)
			m_pEditCount->ReleaseFocus(false);
	}

	return FALSE;
}

BOOL CInfinitewarNoticeBoardDailyUI::UpdateKeyboard(const MSG& p_kMsg)
{
	return FALSE;
}	

void CInfinitewarNoticeBoardDailyUI::RenderScript()
{
	cScriptUI::RenderScript();
	if( m_bVisible && m_dwRenderItemCode > 0 )
		g_pIconMng->RenderItem( m_dwRenderItemCode, GetTotalPosition() + CsPoint( 827, 189 ) );
	cScriptUI::EndRenderScript();
}

void CInfinitewarNoticeBoardDailyUI::_InsertPoint(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pEditCount );
	std::wstring wsValue = m_pEditCount->GetString();
	int nCount = DmCS::StringFn::ToI( wsValue );
	if( 0 >= nCount )
		return;

	DWORD dwItemCount = GetSystem()->GetCurrentNeedItemCount();
	if( nCount > dwItemCount )
	{
		nCount = dwItemCount;
		_SetEditBoxCount( nCount );
	}

	if( 0 != nCount )
		GetSystem()->SendPointInject( nCount );
}

void CInfinitewarNoticeBoardDailyUI::_LeftBtnEvent(void* pSender, void* pData)
{
	_EditBoxCountChange(-1);
}
void CInfinitewarNoticeBoardDailyUI::_PreviousBtnEvent(void* pSender, void* pData)
{
	_EditBoxCountChange(-10);
}
void CInfinitewarNoticeBoardDailyUI::_RightBtnEvent(void* pSender, void* pData)
{
	_EditBoxCountChange(1);
}
void CInfinitewarNoticeBoardDailyUI::_NextBtnEvent(void* pSender, void* pData)
{
	_EditBoxCountChange(10);
}

void CInfinitewarNoticeBoardDailyUI::_SetEditBoxCount( int nValue )
{
	SAFE_POINTER_RET( m_pEditCount );
	m_pEditCount->SetText( nValue );
}

void CInfinitewarNoticeBoardDailyUI::_EditBoxCountChange( int nValue )
{
	SAFE_POINTER_RET( m_pEditCount );
	std::wstring wsValue = m_pEditCount->GetString();
	int nCount = DmCS::StringFn::ToI( wsValue );
	int nResult = nCount + nValue;

	if( nValue < 0 )
	{
		if( nResult < 0 )
			nResult = 0;
	}
	else
	{
		DWORD dwItemCount = GetSystem()->GetCurrentNeedItemCount();
		if( dwItemCount < nResult )
			nResult = dwItemCount;
	}

	m_pEditCount->SetText( nResult );
}

void CInfinitewarNoticeBoardDailyUI::_ResetRemainTime( __int64 const& nTime )
{
	SAFE_POINTER_RET( m_pTimerBar );
	m_pTimerBar->Reset();
	m_pTimerBar->SetTimer( 0.0f, nTime );
	m_pTimerBar->SetProgressPos( 0.0f );
	m_pTimerBar->Start();
}

void CInfinitewarNoticeBoardDailyUI::_SetEditBoxFocus(void* pSender, void* pData)
{
	g_IME.SetNumberOnly( true );
}

void CInfinitewarNoticeBoardDailyUI::_ReleaseEditBoxFocus(void* pSender, void* pData)
{
	g_IME.SetNumberOnly( false );
}

// 초기화 시간 타이머가 다 瑛?때 서버에 정보 갱신 요청을 보낸다.
void CInfinitewarNoticeBoardDailyUI::_SendRequestDataReset(void* pSender, void* pData)
{
	GetSystem()->SendRequestInfiniteWarData();
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////

bool CInfinitewarNoticeBoardDailyUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eUpdate_Today_Injected_Point, this );
	GetSystem()->Register( SystemType::eUpdate_NoticeBoard_Data, this );
	return true;
}

void CInfinitewarNoticeBoardDailyUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eUpdate_Today_Injected_Point:
		{
			DWORD dwChangePoint = 0;
			kStream >> dwChangePoint;
			_UpdateTodayTotalInjectedPoint( dwChangePoint );
			_EditBoxCountChange(0);
		}
		break;
	case SystemType::eUpdate_NoticeBoard_Data:
		{
			_UpdateRewardItemList();
			_UpdateTodayTotalInjectedPoint( GetSystem()->GetTodayTotalInjectedPoint() );		
			_SetNeedItem( GetSystem()->GetCurrentNeedItemCode() );
			_EditBoxCountChange(0);
			_ResetRemainTime( GetSystem()->GetRestRemainTime() );
		}
		break;
	}
}