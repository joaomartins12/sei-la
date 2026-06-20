
#include "stdafx.h"
#include "BattleResultPage.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CBattleResult::CBattleResult() : m_CloseBtn(NULL),m_pItemList(NULL)
{
}

CBattleResult::~CBattleResult()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

void CBattleResult::Destroy()
{
	DeleteScript();
	GetSystem()->EndResultPage();
}

bool CBattleResult::Create()
{
	GetSystem()->StartResultPage();

	InitScript( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), false );
	AddSprite( CsPoint(0, 0), CsPoint( g_nScreenWidth, 128 ), "Battle\\Common\\battle_bg_top.tga" );
	AddSprite( CsPoint(g_nScreenWidth / 2 - (1920 / 2), g_nScreenHeight - 128), CsPoint( 1920, 128 ), "Battle\\Common\\battle_bg_bot.tga" );

	AddSprite( CsPoint((g_nScreenWidth - 446)/2 , g_nScreenHeight - 190),  CsPoint( 446, 118 ), "Battle\\BattleModeSelect\\battle_mode_Text.tga" );


	cText::sTEXTINFO tiGroup;
	tiGroup.Init( CFont::FS_16, NiColor::WHITE );
	tiGroup.s_eTextAlign = DT_CENTER;
	
	tiGroup.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_RESULT_REWARD_ITEM_MAIL_CONFIRM" ).c_str());	// L"우편함을 확인하세요."
	AddText( &tiGroup, CsPoint( g_nScreenWidth/2, g_nScreenHeight - 140 ) );

	
	// 닫기 버튼
	m_CloseBtn = AddButton( CsPoint( g_nScreenWidth / 2 - (162 / 2), g_nScreenHeight - 75 ), CsPoint( 162, 42 ), CsPoint( 0, 42 ), "Battle\\Common\\Common_Btn.tga");	
	if( m_CloseBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT( "DIGIMON_BATTLE_RESULT_CLOSE" ).c_str() ); // 확인
		m_CloseBtn->SetText( &ti );
		m_CloseBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleResult::_CloseBattle );
	}

	_MakeResultItem();
	return true;
}


void CBattleResult::_MakeResultItem()
{
	cScriptUI * pRewardUI = NiNew cScriptUI;
	SAFE_POINTER_RET( pRewardUI );

	pRewardUI->InitScript( NULL, CsPoint( 100, (g_nScreenHeight - 628) / 2 - (186/2)), CsPoint( 323, 628), false  );

	switch( GetSystem()->GetMyResult() )
	{
	case BResult::LOSE:	pRewardUI->AddSprite( CsPoint((323-321)/2, 0), CsPoint(321,242), "Battle\\Result\\batte_lose.tga" );	break;
	case BResult::DRAW:	pRewardUI->AddSprite( CsPoint((323-321)/2, 0), CsPoint(321,242), "Battle\\Result\\batte_draw.tga" );	break;
	case BResult::WIN:	pRewardUI->AddSprite( CsPoint((323-321)/2, 0), CsPoint(321,242), "Battle\\Result\\batte_win.tga" );		break;
	}

	pRewardUI->AddSprite( CsPoint(0, 186), CsPoint(323, 442), "Battle\\Result\\battle_result_Back.tga" );
	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_20, NiColor::WHITE );	
	ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_RESULT_REWARD_UI_TITLE" ).c_str());
	int nHafW = ti.GetTextWidth() / 2;
	pRewardUI->AddText( &ti,  CsPoint((323 / 2) - nHafW, 186 + 28) );

	m_pItemList = NiNew cListBox;
	if( m_pItemList )
	{
		m_pItemList->Init( pRewardUI->GetRoot(), CsPoint(47,290), CsPoint( 245, 286 ), NULL, false );		
		m_pItemList->SetItemGab(CsPoint(0,10));
		m_pItemList->SetItemHeight( 64 );
		pRewardUI->AddChildControl( m_pItemList );
		cScrollBar* pScrollBar = NiNew cScrollBar;
		if( pScrollBar )
		{
			pScrollBar->Init( cScrollBar::TYPE_ENCY, NULL, CsPoint( 0, 0 ), CsPoint( 16, 286 ), cScrollBar::GetDefaultBtnSize(), CsRect( CsPoint(47,290), CsPoint( 224, 286 ) ), 4, false );
			pScrollBar->SetEnableRenderFromEnableScroll(true);
			pScrollBar->ChangeGBImg( "Item_Make\\Make_WheelBar_Bag_Mid.tga", false );
			m_pItemList->SetScrollBar( pScrollBar );
		}

		BResult::sResultInfo::RESULT_REWARD_ITEM_CON const& pRewardInfo = GetSystem()->GetRewardItems();
		BResult::sResultInfo::RESULT_REWARD_ITEM_CON_CIT it = pRewardInfo.begin();
		for( ; it != pRewardInfo.end(); ++it )
		{
			cString * pItem = NiNew cString;
			SAFE_POINTER_CON( pItem );

			cSprite * pBack = NiNew cSprite;
			if( pBack )
			{
				pBack->Init( NULL, CsPoint::ZERO, CsPoint( 224, 64 ), "Battle\\Result\\battle_result_slot.tga", false );
				cString::sSPRITE* paddImg = pItem->AddSprite( pBack );
				if( paddImg )
					paddImg->SetAutoPointerDelete(true);
			}

			pItem->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, (*it).m_dwItemCode, (*it).m_dwItemCount, CsPoint(12,16) );

			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
			ti.SetString(L" ");
			cString::sTEXT* pText = pItem->AddText( &ti, CsPoint( 60, 12 ) );
			if( pText )
				pText->SetText( (*it).m_wsItemName.c_str(), 150 );

			cListBoxItem * addItem  = NiNew cListBoxItem;;
			addItem->SetItem( pItem );
			addItem->SetUserData( new sRewardItemData((*it).m_dwItemCode) );
			m_pItemList->AddItem( addItem );
		}		
	}
	AddChildScriptUI( pRewardUI );
}

void CBattleResult::_CloseBattle(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->CloseResultPage();
}

void CBattleResult::Render3DModel()
{
	if( GetSystem() )
		GetSystem()->RenderResultBackMap();
}


void CBattleResult::Update(float const& fDeltaTime)
{
	if( GetSystem() )
		GetSystem()->UpdateResultObject( fDeltaTime );
}

void CBattleResult::Update3DAccumtime( float const& fAccumTime )
{
	if( GetSystem() )
		GetSystem()->UpdateResultBackMap( fAccumTime );
}

BOOL CBattleResult::UpdateMouse()
{
	if(m_CloseBtn && m_CloseBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pItemList && m_pItemList->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cListBoxItem const* pOverItem = m_pItemList->GetMouseOverItem();
		if( pOverItem )
		{
			sRewardItemData* pInfo = dynamic_cast<sRewardItemData*>(pOverItem->GetUserData());				
			if( pInfo )
			{
				cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
				if( pToolTip )
					pToolTip->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, pInfo->m_dwItemIdx );
			}
		}

		return TRUE;
	}

	return FALSE;
}


void CBattleResult::Render()
{
	RenderScript();
}

void CBattleResult::ResetDevice()
{
	ResetDeviceScript();
}

//////////////////////////////////////////////////////////////////////////////////////////////

bool CBattleResult::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CBattleResult::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}
