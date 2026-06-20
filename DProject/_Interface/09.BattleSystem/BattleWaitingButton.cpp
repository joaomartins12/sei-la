/* 배틀 신청 중일 경우 표시되는 버튼 */

#include "stdafx.h"
#include "BattleWaitingButton.h"

CBattleWaitingBtn::CBattleWaitingBtn():m_pMatchingBtn(NULL)
{

}

CBattleWaitingBtn::~CBattleWaitingBtn()
{
	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE CBattleWaitingBtn::GetWindowType()
{ 
	return WT_BATTLE_MATCHING_WAIT_BUTTON; 
}

void CBattleWaitingBtn::Destroy()
{	
	cBaseWindow::Delete();		
}

void CBattleWaitingBtn::DeleteResource()
{	
	DeleteScript();
}

void CBattleWaitingBtn::Create(int nValue /* = 0  */)
{
	if( !cBaseWindow::Init() )
		return;

	InitScript( NULL, CsPoint( 1024 /2 - 40, 768 - 300 ), CsPoint( 80, 80 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::BOTTOM, true );

	m_pMatchingBtn = AddButton( CsPoint::ZERO, CsPoint( 80, 80 ), CsPoint( 0, 80 ), "Battle\\Wait\\battle_Wait_Btn.tga" );
	if( m_pMatchingBtn )
	{
		m_pMatchingBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &CBattleWaitingBtn::_CancelBattleMatchingAsk );

		m_pMatchingBtn->SetButtonEffect( cSpriteAni::LOOP, CsPoint::ZERO, CsPoint( 80, 80 ), "Battle\\Wait\\battle_Wait_Btn_Ani16.tga", NULL, 16, false, CsPoint(80,80) , cSpriteAni::SPRITE_POS, 4 );	
		m_pMatchingBtn->SetEffectAniTime( 0.1f);
		m_pMatchingBtn->SetEffectOn( cButton::eSTATE::NORMAL , true );
	}
}

void CBattleWaitingBtn::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
}

cBaseWindow::eMU_TYPE CBattleWaitingBtn::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	if(m_pMatchingBtn && m_pMatchingBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return eMU_TYPE::MUT_NONE;

	return muReturn;
}

void CBattleWaitingBtn::Render()
{
	RenderScript();
}

void CBattleWaitingBtn::ResetDevice()
{
	ResetDeviceScript();
}

void CBattleWaitingBtn::PreResetMap()
{ 
	Close( false );
}

void CBattleWaitingBtn::_CancelBattleMatchingAsk(void* pSender, void* pData)
{
	GetSystem()->ToggleWaitingWindow();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool CBattleWaitingBtn::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CBattleWaitingBtn::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}
