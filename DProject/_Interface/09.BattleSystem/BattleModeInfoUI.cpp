
#include "stdafx.h"
#include "BattleModeInfoUI.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CBattleModeInfoUI::CBattleModeInfoUI() :
m_pModeName(NULL),
m_pNeedTicketCount(NULL),
m_pJoinCount(NULL),
m_pMyDigimonLv(NULL),
m_pMyDigimonTBName(NULL),
m_pJoinEnableText(NULL),
m_pWinRewardItemList(NULL),
m_pLoseRewardItemList(NULL)
{
}

CBattleModeInfoUI::~CBattleModeInfoUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

bool CBattleModeInfoUI::Create()
{
	InitScript( NULL, CsPoint(0, 40), CsPoint( 540, 401 ), false );
	AddSprite( CsPoint::ZERO, CsPoint(495, 125), "Battle\\BattleModeSelect\\CurrentMode_Bg.tga" );

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_32, NiColor::WHITE );
		ti.SetText( L" " );
		m_pModeName = AddText( &ti, CsPoint(58,35));
	}

	AddSprite( CsPoint(61,120), CsPoint(46, 276), "Battle\\BattleModeSelect\\Value_Icon.tga" );


	int nStartPosX = 115;
	{	// 필요티켓
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_NEED_TICKET" ).c_str());
		cText* pText = AddText( &ti, CsPoint(nStartPosX,130));

		int textWidth = pText->GetTextInfo()->GetTextWidth();

		cText::sTEXTINFO ti2;	// 필요 티겟
		ti2.Init( CFont::FS_20, NiColor(0,1,0) );
		ti2.SetText(L" ");
		m_pNeedTicketCount = AddText( &ti2, CsPoint(nStartPosX + textWidth + 20, 130 ));
	}

	{	// 금일참여
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_JOIN_LIMIT" ).c_str());
		cText* pText =AddText( &ti, CsPoint(nStartPosX,176));
		int textWidth = pText->GetTextInfo()->GetTextWidth();

		cText::sTEXTINFO ti2;	//참여 횟수
		ti2.Init( CFont::FS_20, NiColor(0,1,0) );
		ti2.SetText(L" ");
		m_pJoinCount = AddText( &ti2, CsPoint(nStartPosX + textWidth + 20, 176));
	}

	{	// 내디지몬
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_MY_DIGIMON_INFO" ).c_str());
		cText* pText =AddText( &ti, CsPoint(nStartPosX,222));
		int textWidth = pText->GetTextInfo()->GetTextWidth();
		textWidth += 20;
		cText::sTEXTINFO ti2;	// 내 디지몬 레벨 및 이름
		ti2.Init( CFont::FS_20, NiColor(0,1,0) );
		ti2.SetText(L" ");
		m_pMyDigimonTBName = AddText( &ti2, CsPoint(nStartPosX + textWidth, 222));
		textWidth += m_pMyDigimonTBName->GetTextInfo()->GetTextWidth();
		textWidth += 20;

		cText::sTEXTINFO ti3;	// 참여 가능 여부
		ti3.Init( CFont::FS_20, NiColor(0,1,0) );
		ti3.SetText(L" ");
		m_pJoinEnableText = AddText( &ti3, CsPoint(nStartPosX + textWidth, 222));	
	}

	{	// 승리보상
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_WIN_REWARD_ITEM" ).c_str());
		cText* pText =AddText( &ti, CsPoint(nStartPosX,268));

		int textWidth = pText->GetTextInfo()->GetTextWidth();

		m_pWinRewardItemList = NiNew cGridListBox;
		if(m_pWinRewardItemList)
		{
			m_pWinRewardItemList->Init( GetRoot(), CsPoint( nStartPosX + textWidth + 20, 264 ), CsPoint( 180, 32), CsPoint(5,0), CsPoint(32, 32), 
				cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );

			AddChildControl(m_pWinRewardItemList);
		}
	}

	{	// 패배보상
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_LOSE_REWARD_ITEM" ).c_str());
		cText* pText =AddText( &ti, CsPoint(nStartPosX,314));

		int textWidth = pText->GetTextInfo()->GetTextWidth();

		m_pLoseRewardItemList = NiNew cGridListBox;
		if(m_pLoseRewardItemList)
		{
			m_pLoseRewardItemList->Init( GetRoot(), CsPoint( nStartPosX + textWidth + 20, 310 ), CsPoint( 180, 32), CsPoint(5,0), CsPoint(32, 32), 
				cGridListBox::LowRightDown, cGridListBox::LeftTop, NULL, false, 0 );

			AddChildControl(m_pLoseRewardItemList);
		}
	}

	{	// 내 디지몬 레벨
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_DIGIMON_LIMIT_LV" ).c_str());
		cText* pText =AddText( &ti, CsPoint(nStartPosX,360));
		int textWidth = pText->GetTextInfo()->GetTextWidth();

		cText::sTEXTINFO ti2;	// 내 디지몬 레벨 및 이름
		ti2.Init( CFont::FS_20, NiColor(0,1,0) );
		ti2.SetText(L" ");
		m_pMyDigimonLv = AddText( &ti2, CsPoint(nStartPosX + textWidth + 20, 360));
	}
	return true;
}

void CBattleModeInfoUI::Destroy()
{
	DeleteScript();	
}

BOOL CBattleModeInfoUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	if( m_pWinRewardItemList && m_pWinRewardItemList->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cGridListBoxItem const* pOverItem = m_pWinRewardItemList->GetMouseOverItem();
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

	if( m_pLoseRewardItemList && m_pLoseRewardItemList->Update_ForMouse( CURSOR_ST.GetPos() ) )
	{
		cGridListBoxItem const* pOverItem = m_pLoseRewardItemList->GetMouseOverItem();
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


void CBattleModeInfoUI::Render()
{
	RenderScript();
}

void CBattleModeInfoUI::ResetDevice()
{
	ResetDeviceScript();
}


void CBattleModeInfoUI::_UpdateModInfoUI( bool bVisible )
{
	if( m_pWinRewardItemList )
		m_pWinRewardItemList->RemoveAllItem();
	if( m_pLoseRewardItemList )
		m_pLoseRewardItemList->RemoveAllItem();

	SetVisible(bVisible);
	if( !bVisible )
		return;

	BRegist::sMapInfos const* pMapInfo = GetSystem()->GetCurrentReigistBattleInfo();
	SAFE_POINTER_RET( pMapInfo );

	if( m_pModeName )
	{
		std::wstring typeStr;
		switch(GetSystem()->GetSelectedType())
		{
		case eDuelCategory::TEAM:		typeStr = UISTRING_TEXT("DIGIMON_BATTLE_REGIST_TYPE_TEAM");	break;// 팀 배틀
		case eDuelCategory::DEFENSE:	typeStr = UISTRING_TEXT("DIGIMON_BATTLE_REGIST_TYPE_DEFANCE");	break;// 방어전 ( 미구현 )
		}

		m_pModeName->SetText( typeStr.c_str() );
	}


	if( m_pNeedTicketCount )
	{
		unsigned int nHaveTicketCount = GetSystem()->GetHaveBattleTicketPoint();
		unsigned int nNeedTicketCount = pMapInfo->GetNeedTicket();

		std::wstring haveTicket = DmCS::StringFn::getNumberFormatW( nHaveTicketCount );
		std::wstring needTicket = DmCS::StringFn::getNumberFormatW( nNeedTicketCount );
		std::wstring wc;
		DmCS::StringFn::Format( wc, L"%s / %s", haveTicket.c_str(), needTicket.c_str() );
		m_pNeedTicketCount->SetText( wc.c_str() );
		if( nHaveTicketCount < nNeedTicketCount )
			m_pNeedTicketCount->SetColor( NiColor(1,0,0) );	// 필요 티켓 충족
		else
			m_pNeedTicketCount->SetColor( NiColor(0,1,0) );	// 필요 티켓 부족
	}

	if( m_pJoinCount )
	{
		std::wstring wc;
		DmCS::StringFn::Format( wc, L"%d / %d", pMapInfo->GetPlayCount(), pMapInfo->GetLimitPlayCount() );
		m_pJoinCount->SetText( wc.c_str() );
		if( pMapInfo->IsEnablePlayCount() )
			m_pJoinCount->SetColor( NiColor(0,1,0) );	// 참여 가능 횟수 미만
		else
			m_pJoinCount->SetColor( NiColor(1,0,0) );	// 참여 가능 횟수 초과
	}

	if( m_pMyDigimonTBName )
	{
		std::wstring dTbName = GetSystem()->GetMyDigimonTableName();
		m_pMyDigimonTBName->SetText( dTbName.c_str() );
		if( !GetSystem()->IsEnoughDigimon() )
			m_pMyDigimonTBName->SetColor( NiColor(1,0,0) );

		int nNextPosX = m_pMyDigimonTBName->GetPos().x + m_pMyDigimonTBName->GetTextInfo()->GetTextWidth();	

		if( m_pJoinEnableText )
		{
			std::wstring joinEnableText;
			NiColor textColor;
			if( GetSystem()->IsJoinBattleHaveDigimon() )// 참여 가능
			{
				joinEnableText = UISTRING_TEXT( "DIGIMON_BATTLE_JOIN_ENABLE");
				textColor = NiColor(0,1,0);
			}
			else
			{
				joinEnableText = UISTRING_TEXT( "DIGIMON_BATTLE_JOIN_DISABLE");
				textColor = NiColor(1,0,0);
			}

			m_pJoinEnableText->SetPos( CsPoint( nNextPosX, 222 ) );
			m_pJoinEnableText->SetColor( textColor );
			m_pJoinEnableText->SetText( joinEnableText.c_str() );
		}
	}


	if( m_pWinRewardItemList )
	{
		BRegist::REWARD_ITEM_CON const& Items = pMapInfo->GetViewWinRewardItems();
		BRegist::REWARD_ITEM_CON_CIT it = Items.begin();
		for( int n =0 ; it != Items.end(); ++it, ++n )
		{
			cString* pString = NiNew cString;
			SAFE_POINTER_RET(pString);

			pString->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, (*it).m_dwItemCode, (*it).m_dwItemCount );

			cGridListBoxItem* addItem  = NiNew cGridListBoxItem(n, CsPoint( 32, 32 ));
			addItem->SetItem(pString);
			addItem->SetUserData( new sRewardItemData((*it).m_dwItemCode) );			
			m_pWinRewardItemList->AddItem( addItem );
		}
	}


	if( m_pLoseRewardItemList )
	{
		BRegist::REWARD_ITEM_CON const& Items = pMapInfo->GetViewLoseRewardItems();
		BRegist::REWARD_ITEM_CON_CIT it = Items.begin();
		for( int n =0 ; it != Items.end(); ++it, ++n )
		{
			cString* pString = NiNew cString;
			SAFE_POINTER_RET(pString);

			pString->AddIcon( CsPoint( 32, 32 ), ICONITEM::ARMOR_1, (*it).m_dwItemCode, (*it).m_dwItemCount );

			cGridListBoxItem* addItem  = NiNew cGridListBoxItem(n, CsPoint( 32, 32 ));
			addItem->SetItem(pString);
			addItem->SetUserData( new sRewardItemData((*it).m_dwItemCode) );			
			m_pLoseRewardItemList->AddItem( addItem );
		}
	}

	int nNextPosX = 0;
	if( m_pMyDigimonLv )
	{
		std::wstring dLv;
		DmCS::StringFn::Format( dLv, L"Lv %d / Lv %d", GetSystem()->GetMyDigimonLv(), pMapInfo->GetLimitDLv() );
		m_pMyDigimonLv->SetText( dLv.c_str() );
		if( !GetSystem()->IsEnoughDigimonLv() )
			m_pMyDigimonLv->SetColor( NiColor(1,0,0) );
	}

}

void CBattleModeInfoUI::_ChangeStep(BRegist::eRegistState const& step)
{
	switch( step )
	{
	case BRegist::eState_Type_Select:	_UpdateModInfoUI(false);	break;
	case BRegist::eState_Mode_Select:	_UpdateModInfoUI(true);		break;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////

bool CBattleModeInfoUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChange_State, this );
	GetSystem()->Register( SystemType::eChange_Mode, this );
	return true;
}

void CBattleModeInfoUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eChange_State:
		{
			kStream.SetRp(0);
			BRegist::eRegistState step;
			kStream >> step;
			_ChangeStep( step );
		}		
		break;
	case SystemType::eChange_Mode:	_UpdateModInfoUI(true);		break;
	}
}