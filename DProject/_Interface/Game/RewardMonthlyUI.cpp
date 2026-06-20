
#include "stdafx.h"
#include "RewardMonthlyUI.h"

cRewardMonthlyUI::cRewardMonthlyUI():m_pItemGride(NULL)
{
	Construct();
}

cRewardMonthlyUI::~cRewardMonthlyUI()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cRewardMonthlyUI::Create(cWindow* pkRoot, int nValue /* = 0 */ )
{
	IEventViewer::Create( pkRoot, nValue );

	//////////////////////////////////////////////////////////////////////////////////
	//월간접속보상224,56
	{
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_14, NiColor(1.0f, 0.7f, 0.0f) );
		textInfo.SetText(UISTRING_TEXT( "REWARD_MENU_TEXT2" ).c_str());
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(274, 21));
	}

	{
		//월간 접속일에 따라 보상이 지급됩니다
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_14, NiColor(0.5f, 0.8f, 1.0f) );
		textInfo.SetText(UISTRING_TEXT( "REWARD_EXPLAIN_TEXT2" ).c_str());
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(274, 83));
	}

	{// 이벤트 메시지
		cText::sTEXTINFO	textInfo;
		textInfo.Init( CFont::FS_12, NiColor( 1.0f, 0.0f, 0.0f ) );
		textInfo.s_eTextAlign = DT_CENTER;
		textInfo.SetText( L" " );
		cText* pText = AddText(&textInfo, CsPoint(274, 159));
		if( pText )
			GetSystem()->SetMonthMessage( pText );
	}

	{
		//1주차
		cText::sTEXTINFO	textInfo;
		textInfo.Init(CFont::FS_12, NiColor( 1.0f, 0.7f, 0.0f ));
		std::wstring wsWeek = UISTRING_TEXT( "COMMON_TXT_N_WEEK" );
		DmCS::StringFn::Replace( wsWeek, L"#Week#", 1 );
		textInfo.SetText( wsWeek.c_str() );
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(59, 263));
	}

	{
		//2주차
		cText::sTEXTINFO	textInfo;
		textInfo.Init(CFont::FS_12, NiColor( 1.0f, 0.7f, 0.0f ));
		std::wstring wsWeek = UISTRING_TEXT( "COMMON_TXT_N_WEEK" );
		DmCS::StringFn::Replace( wsWeek, L"#Week#", 2 );
		textInfo.SetText( wsWeek.c_str() );
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(59, 313));
	}

	{
		//3주차
		cText::sTEXTINFO	textInfo;
		textInfo.Init(CFont::FS_12, NiColor( 1.0f, 0.7f, 0.0f ));
		std::wstring wsWeek = UISTRING_TEXT( "COMMON_TXT_N_WEEK" );
		DmCS::StringFn::Replace( wsWeek, L"#Week#", 3 );
		textInfo.SetText( wsWeek.c_str() );
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(59, 363));
	}

	{
		//4주차
		cText::sTEXTINFO	textInfo;
		textInfo.Init(CFont::FS_12, NiColor( 1.0f, 0.7f, 0.0f ));
		std::wstring wsWeek = UISTRING_TEXT( "COMMON_TXT_N_WEEK" );
		DmCS::StringFn::Replace( wsWeek, L"#Week#", 4 );
		textInfo.SetText( wsWeek.c_str() );
		textInfo.s_eTextAlign = DT_CENTER;
		AddText(&textInfo, CsPoint(59, 413));
	}

// 	// 아이템 슬롯 배경
// 	AddSprite(CsPoint( 107, 246 ), CsPoint( 384, 198 ), "Event\\event_slot_03.tga");

	m_pItemGride = NiNew cGridListBox;
	if( m_pItemGride )
	{
		m_pItemGride->Init( m_pRoot, CsPoint(102, 241), CsPoint( 384, 228 ), CsPoint( 8, 2 ), CsPoint(48,48), cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );
		m_pItemGride->SetMouseOverImg( "Event\\event_select.tga" );
		m_pItemGride->SetBackOverAndSelectedImgRender(false);
		m_pItemGride->SetAutoSelection(false);
		AddChildControl(m_pItemGride);
	}

	UpdateRewardData();

	AddSprite(CsPoint( 112, 251 ), CsPoint( 351, 162 ), "Event\\event_slot_num.tga");
	int iComplete = GetSystem()->GetMonthlyEventAttendCnt();
	UpdateTimeMsg( iComplete );
}

BOOL cRewardMonthlyUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;
	
	if( m_pItemGride && m_pItemGride->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cGridListBoxItem const* pOverItem = m_pItemGride->GetMouseOverItem();
		SAFE_POINTER_RETVAL( pOverItem, TRUE );

		sRewardItemData* pInfo = static_cast<sRewardItemData*>(pOverItem->GetUserData());
		SAFE_POINTER_RETVAL(pInfo, TRUE);

		cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
		SAFE_POINTER_RETVAL(pToolTip, TRUE);

		if( 0 != pInfo->s_nItemCode )
			pToolTip->SetTooltip( pOverItem->GetWorldPos(), CsPoint( 44, 44 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->s_nItemCode, cBaseWindow::WT_STORE );

		return TRUE;
	}

	return FALSE;
}

void cRewardMonthlyUI::RenderScript()
{
	cScriptUI::RenderScript();
	if( IsVisible() )
		sDateText.Render(GetTotalPosition() + CsPoint( 272, 113 ), 2, DT_CENTER);
}

void cRewardMonthlyUI::UpdateTimeMsg( int const& nDate )
{
	sDateText.Delete();
	
	SAFE_POINTER_RET( g_pStringAnalysis );

	std::wstring textMsg = UISTRING_TEXT( "REWARD_MONTH");
	DmCS::StringFn::ReplaceAll( textMsg, L"#day#", nDate );

	cText::sTEXTINFO textInfo;
	textInfo.Init( CFont::FS_12, NiColor(0.5f, 0.8f, 1.0f) );
	g_pStringAnalysis->Cut_Parcing( &sDateText, 310, const_cast<TCHAR*>(textMsg.c_str()) , &textInfo );
}

void cRewardMonthlyUI::UpdateRewardData()
{
	SAFE_POINTER_RET( m_pItemGride );
	m_pItemGride->RemoveAllItem();

	byte iComplete = GetSystem()->GetMonthlyEventAttendCnt();
	bool bNotify = GetSystem()->GetMonthlyEventNotify();

	for( int n = 0; n < MONTHLY_ITEM_MAXCNT; ++n )
	{
		int iItemCode = GetSystem()->GetMonthItemID(n);
		int iItemCount = GetSystem()->GetMonthItemCnt(n);
		if( 0 == iItemCode || 0 == iItemCount )
			continue;

		cString * pItem = NiNew cString;

		cSprite* pSlotBackImg = NiNew cSprite;
		if( pSlotBackImg )
		{
			pSlotBackImg->Init( NULL, CsPoint::ZERO, CsPoint(48,48), "Event\\event_slot_single.tga", false );
			cString::sSPRITE* pBack = pItem->AddSprite( pSlotBackImg );
			if( pBack )
				pBack->SetAutoPointerDelete(true);
		}

		cString::sICON* pIcon = pItem->AddIcon( CsPoint(34,34), ICONITEM::ITEM_ICON, iItemCode, iItemCount, CsPoint(7,7) );
		if( pIcon )
			pIcon->SetAutoPointerDelete(true);

// 		if( bNotify && (n == (iComplete - 1)))
// 		{
// 		}
// 		else 
		if( n < iComplete )
		{
			cSprite* pCheckImg = NiNew cSprite;
			pCheckImg->Init( NULL, CsPoint::ZERO, CsPoint(44,44), "Event\\event_check.tga", false );
			cString::sSPRITE* pImg = pItem->AddSprite( pCheckImg, CsPoint(2,2) );
			if( pImg )
				pImg->SetAutoPointerDelete(true);
		}

		cGridListBoxItem * addItem  = NiNew cGridListBoxItem(n, CsPoint( 48, 48 ));
		addItem->SetItem( pItem );
		addItem->SetUserData( new sRewardItemData( iItemCode ) );
		m_pItemGride->AddItem( addItem );
	}
}

//////////////////////////////////////////////////////////////////////////

bool cRewardMonthlyUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eMonthlyEvent_Update, this );

	return true;
}

void cRewardMonthlyUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
	switch( iNotifiedEvt )
	{
	case SystemType::eMonthlyEvent_Update:	
		{
			UpdateRewardData();		
			int iComplete = GetSystem()->GetMonthlyEventAttendCnt();
			UpdateTimeMsg( iComplete );
		}
		break;
	}
}