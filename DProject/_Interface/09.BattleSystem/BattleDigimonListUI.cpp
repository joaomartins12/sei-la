#include "stdafx.h"
#include "BattleDigimonListUI.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CBattleDigimonListUI::CBattleDigimonListUI() 
{
}

CBattleDigimonListUI::~CBattleDigimonListUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

void CBattleDigimonListUI::Destroy()
{
	DeleteScript();
}

bool CBattleDigimonListUI::Create()
{
	InitScript( NULL, CsPoint( g_nScreenWidth / 2 - 455, g_nScreenHeight - 272 - 100 ), CsPoint( 910, 272 ), false );
	AddSprite( CsPoint((910/2) - (446 / 2), 0), CsPoint( 446, 118 ), "Battle\\BattleModeSelect\\battle_mode_Text.tga" );

	m_pDigimonListLeftMoveBtn = AddButton( CsPoint( 910 / 2 - (774 / 2) - 57 - 10, 154 ), CsPoint( 57, 64 ), CsPoint( 0, 64 ), "Battle\\BattleModeSelect\\Arrow_Left.tga");
	if( m_pDigimonListLeftMoveBtn )
		m_pDigimonListLeftMoveBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleDigimonListUI::_DigimonLeftMove );
	m_pDigimonListRightMoveBtn = AddButton( CsPoint( 910 / 2 + (774 / 2) + 10, 154 ), CsPoint( 57, 64 ), CsPoint( 0, 64 ), "Battle\\BattleModeSelect\\Arrow_Right.tga");
	if( m_pDigimonListRightMoveBtn )
		m_pDigimonListRightMoveBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleDigimonListUI::_DigimonRightMove );

	cText::sTEXTINFO tiGroup;
	tiGroup.Init( CFont::FS_20, NiColor::WHITE );
	tiGroup.s_eTextAlign = DT_CENTER;
	tiGroup.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_JOINED_DIGIMON" ).c_str());	// L"참여 가능 디지몬"
	AddText( &tiGroup, CsPoint(910/2, 45) );

	m_pDigimonGridListBox = NiNew cGridListBox;
	if( m_pDigimonGridListBox )
	{
		m_pDigimonGridListBox->Init( GetRoot(), CsPoint(910 / 2 - (774 / 2), 100), CsPoint( 774, 172 ), CsPoint::ZERO, CsPoint(86, 86), 
			cGridListBox::LowRightDown, cGridListBox::LeftTop,NULL, false, 0 );
		m_pDigimonGridListBox->SetMouseOverImg( "Battle\\BattleModeSelect\\SelectedSlot_Img.tga" );
		m_pDigimonGridListBox->SetSelectedImg( "Battle\\BattleModeSelect\\SelectedSlot_Img.tga" );
		m_pDigimonGridListBox->SetBackOverAndSelectedImgRender(false);
		//m_pDigimonGridListBox->AddEvent( cGridListBox::GRID_SELECT_CHANGE_ITEM, this, &CBattleRegister::_ChangeSelectDigimon );

		AddChildControl(m_pDigimonGridListBox);
	}

	BRegist::MAP_DIGIMONINFO const & list = GetSystem()->GetRegistableDigimonList();
	BRegist::MAP_DIGIMONINFO_CIT it = list.begin();
	for( int n = 0; it != list.end(); ++it, ++n )
	{
		cString* pString = NiNew cString;
		SAFE_POINTER_CON(pString);

		cSprite* pBackImg = NiNew cSprite;		
		pBackImg->Init( NULL,CsPoint::ZERO, CsPoint( 68, 68 ), "Battle\\BattleModeSelect\\Slot_Bg.tga", true, NiColor::WHITE );
		cString::sSPRITE* pImg = pString->AddSprite( pBackImg, CsPoint(9,9) );
		if( pImg )
			pImg->SetAutoPointerDelete(true);

		cSprite* pDigimonImg = NiNew cSprite;		
		pDigimonImg->Init( NULL, CsPoint::ZERO, CsPoint( 64, 64 ), it->second.m_DigimonImgFile.c_str(), true, NiColor::WHITE, false );
		cString::sSPRITE* paddImg = pString->AddSprite( pDigimonImg,CsPoint(11,11) );
		if( paddImg )
			paddImg->SetAutoPointerDelete(true);

		cGridListBoxItem* addItem  = NiNew cGridListBoxItem(n, CsPoint( 86, 86 ));
		addItem->SetUserData( new sDigimonData(it->first, it->second.m_DigimonTableName) );
		addItem->SetItem(pString);
		m_pDigimonGridListBox->AddItem( addItem );
	}
	return true;
}

BOOL CBattleDigimonListUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	if( m_pDigimonListLeftMoveBtn && m_pDigimonListLeftMoveBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;
	if( m_pDigimonListRightMoveBtn && m_pDigimonListRightMoveBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;
	if( m_pDigimonGridListBox && m_pDigimonGridListBox->Update_ForMouse(CURSOR_ST.GetPos()) )
	{
		cGridListBoxItem const* pOverItem = m_pDigimonGridListBox->GetMouseOverItem();
		if( pOverItem )
		{
			sDigimonData* pInfo = static_cast<sDigimonData*>(pOverItem->GetUserData());
			if( pInfo && !pInfo->m_nDigimonClassName.empty() )
			{
				cTooltip* pToolTip = TOOLTIPMNG_STPTR->GetTooltip();
				if( pToolTip )
					pToolTip->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint::ZERO, pInfo->m_nDigimonClassName.c_str(), CFont::FS_13);
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CBattleDigimonListUI::UpdateKeyboard(const MSG& p_kMsg)
{
	return FALSE;
}	

void CBattleDigimonListUI::Render()
{
	RenderScript();
}

void CBattleDigimonListUI::ResetDevice()
{
	ResetDeviceScript();
}



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void CBattleDigimonListUI::_UpdateDigimonListUI( bool bVisible )
{
	SetVisible(bVisible);
	if( !bVisible )
		return;

	if( m_pDigimonGridListBox )
		m_pDigimonGridListBox->ChangeShowLinePos(0);
}

void CBattleDigimonListUI::_DigimonLeftMove(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pDigimonGridListBox );
	int nCurrentLine = m_pDigimonGridListBox->GetShowLineStartPos();
	m_pDigimonGridListBox->ChangeShowLinePos(nCurrentLine - 2);

}

void CBattleDigimonListUI::_DigimonRightMove(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pDigimonGridListBox );
	int nCurrentLine = m_pDigimonGridListBox->GetShowLineStartPos();
	m_pDigimonGridListBox->ChangeShowLinePos(nCurrentLine + 2);
}

void CBattleDigimonListUI::_ChangeSelectDigimon(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pData );
	cGridListBoxItem* pSelItem = static_cast<cGridListBoxItem*>(pData);
	sDigimonData* pUserData = dynamic_cast<sDigimonData*>(pSelItem->GetUserData());
	SAFE_POINTER_RET( pUserData );
	//GetSystem()->SelectedDigimon( pUserData->m_nDigimonIDX );
}

void CBattleDigimonListUI::_ChangeStep(BRegist::eRegistState const& step)
{
	switch( step )
	{
	case BRegist::eState_Type_Select:	SetVisible(false);		break;
	case BRegist::eState_Mode_Select:	SetVisible(true);		break;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////

bool CBattleDigimonListUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChange_State, this );
	return true;
}

void CBattleDigimonListUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
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
	}
}