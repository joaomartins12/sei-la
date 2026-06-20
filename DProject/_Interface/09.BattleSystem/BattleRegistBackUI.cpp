#include "stdafx.h"
#include "BattleRegistBackUI.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CBattleRegistBackUI::CBattleRegistBackUI() 
{
}

CBattleRegistBackUI::~CBattleRegistBackUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

bool CBattleRegistBackUI::Create()
{
	InitScript( NULL, CsPoint( g_nScreenWidth / 2 - 512, 0 ), CsPoint( 1024, g_nScreenHeight ), false );

	m_pVersusType = NiNew cImage;
	if( m_pVersusType )
	{
		m_pVersusType->Init( GetRoot(), CsPoint((1024/2) - (196 / 2), 10), CsPoint(196, 93), "Battle\\BattleModeSelect\\battle_versus.tga", false );
		m_pVersusType->SetTexToken( CsPoint( 0, 93 ) );
		AddChildControl( m_pVersusType );
	}

	m_pModChangeLeft = AddButton( CsPoint( (1024/2) - (196 / 2) - 57 - 10, 24 ), CsPoint( 57, 64 ), CsPoint( 0, 64 ), "Battle\\BattleModeSelect\\Arrow_Left.tga");
	if( m_pModChangeLeft )
		m_pModChangeLeft->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleRegistBackUI::_ChangeModeDown );
	m_pModChangeRight = AddButton( CsPoint( (1024/2) + (196 / 2) + 10, 24 ), CsPoint( 57, 64 ), CsPoint( 0, 64 ), "Battle\\BattleModeSelect\\Arrow_Right.tga");
	if( m_pModChangeRight )
		m_pModChangeRight->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleRegistBackUI::_ChangeModeUp );


	// 이전 버튼
	m_pBackBtn = AddButton( CsPoint( 1024 / 2 - (162 / 2) - 90, g_nScreenHeight - 75 ), CsPoint( 162, 42 ), CsPoint( 0, 42 ), "Battle\\Common\\Close_Btn.tga");
	if( m_pBackBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_BACK" ).c_str());
		m_pBackBtn->SetText( &ti );
		m_pBackBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleRegistBackUI::_Backward );
	}

	// 등록 버튼
	m_pRegistBtn = AddButton( CsPoint( 1024 / 2 - (162 / 2) + 90, g_nScreenHeight - 75 ), CsPoint( 162, 42 ), CsPoint( 0, 42 ), "Battle\\Common\\Common_Btn.tga");	
	if( m_pRegistBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_20, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT( "DIGIMON_BATTLE_REGIST_MODE_REGIST" ).c_str());
		m_pRegistBtn->SetText( &ti );
		m_pRegistBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBattleRegistBackUI::_SendBattleRegist );
	}
	return true;
}

void CBattleRegistBackUI::Destroy()
{
	DeleteScript();	
}

BOOL CBattleRegistBackUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	if( m_pBackBtn && m_pBackBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pRegistBtn && m_pRegistBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pModChangeLeft && m_pModChangeLeft->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if(m_pModChangeRight && m_pModChangeRight->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	return FALSE;
}

void CBattleRegistBackUI::Render()
{
	RenderScript();
}

void CBattleRegistBackUI::ResetDevice()
{
	ResetDeviceScript();
}

void CBattleRegistBackUI::_Backward(void* pSender, void* pData)
{
	GetSystem()->RegisterBackStep();
}

void CBattleRegistBackUI::_SendBattleRegist(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	cButton* pBtn = static_cast<cButton*>(pSender);
	if(GetSystem()->Send_BattleMatch_Regist())
		pBtn->SetEnable(false);
}

void CBattleRegistBackUI::_ChangeModeDown(void* pSender, void* pData)
{
	GetSystem()->SetCurrentModeDown();
}

void CBattleRegistBackUI::_ChangeModeUp(void* pSender, void* pData)
{
	GetSystem()->SetCurrentModeUp();
}

void CBattleRegistBackUI::_ChangeStep(BRegist::eRegistState const& step)
{
	switch( step )
	{
	case BRegist::eState_Type_Select:	SetVisible(false);		break;
	case BRegist::eState_Mode_Select:	
		SetVisible(true);	
		_ChangeMode();	
		break;
	}
}

void CBattleRegistBackUI::_ChangeMode()
{
	SAFE_POINTER_RET( m_pVersusType );

	m_pVersusType->SetState(GetSystem()->GetSelectedMode() - 1);
// 	switch( GetSystem()->GetSelectedMode() )
// 	{
// 	case eDuelSubCategory::SINGLE_DUEL:		m_pVersusType->SetState(cImage::NORMAL0);	break;
// 	case eDuelSubCategory::DOUBLE_DUEL:		m_pVersusType->SetState(cImage::NORMAL1);	break;
// 	case eDuelSubCategory::TRIPLE_DUEL:		m_pVersusType->SetState(cImage::NORMAL2);	break;
// 	case eDuelSubCategory::QUADRUPLE_DUEL:	m_pVersusType->SetState(cImage::NORMAL3);	break;
// 	case eDuelSubCategory::OCTUPLE_DUEL:	m_pVersusType->SetState(cImage::NORMAL4);	break;
// 	case eDuelSubCategory::SEXDECUPLE_DUEL:	m_pVersusType->SetState(cImage::NORMAL5);	break;
// 	}
}
//////////////////////////////////////////////////////////////////////////////////////////////

bool CBattleRegistBackUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChange_State, this );
	GetSystem()->Register( SystemType::eChange_Mode, this );
	GetSystem()->Register( SystemType::eRegistFales, this );
	
	return true;
}

void CBattleRegistBackUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
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
	case SystemType::eChange_Mode:	_ChangeMode();		break;
	case SystemType::eRegistFales:
		if( m_pRegistBtn )
			m_pRegistBtn->SetEnable(true);
		break;
	}
}