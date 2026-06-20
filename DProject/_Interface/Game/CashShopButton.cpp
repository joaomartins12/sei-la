#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "CashShopButton.h"

cCashShopButton::cCashShopButton(): m_pOpenButton(NULL)
{
}

cCashShopButton::~cCashShopButton()
{
}

void cCashShopButton::Destroy()
{
	cBaseWindow::Delete();
}

void cCashShopButton::DeleteResource()
{		
	DeleteScript(); 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cCashShopButton::Create(int nValue /* = 0  */)
{		
	if( cBaseWindow::Init() == false )
		return; 

	InitScript( NULL, CsPoint( 829, 687 ), CsPoint( 45, 45 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM, true );

	m_pOpenButton = AddButton( CsPoint::ZERO, CsPoint( 45, 45 ), CsPoint( 45, 0 ), "Cash\\Shop\\Store_Button_Icon.tga" );
	if( m_pOpenButton )
	{
		m_pOpenButton->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cCashShopButton::OpenWindow );
		m_pOpenButton->SetButtonEffect( cSpriteAni::LOOP, CsPoint::ZERO, CsPoint( 45, 45 ), "Cash\\Shop\\Store_Button_Icon_ani.tga", NULL, 8, false, CsPoint(45,0) , cSpriteAni::SPRITE_POS );	
		m_pOpenButton->SetEffectAniTime( 0.2f);
		m_pOpenButton->SetEffectOn( cButton::eSTATE::NORMAL , true );
	}	
}

cBaseWindow::eMU_TYPE
cCashShopButton::Update_ForMouse()
{	
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
	{
		if( m_pOpenButton )
			m_pOpenButton->Update_ForMouse();	//2016-02-15-nova
		return muReturn;	
	}

	if( m_pOpenButton && m_pOpenButton->Update_ForMouse() == cButton::ACTION_ON )
		ShowToolTip(NULL);

	return muReturn;
}

void cCashShopButton::Update(float const& fDeltaTime)
{
	UpdateScript( fDeltaTime );
}

void cCashShopButton::Render()
{
	RenderScript();
}

void cCashShopButton::ResetDevice()
{	
	ResetDeviceScript();	
}

void cCashShopButton::OpenWindow(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->Req_ShowCashshop(NULL);	
}

void cCashShopButton::ShowToolTip(void* pData)
{
	TCHAR szKey[ MAX_FILENAME ];
	GLOBALINPUT_ST.GetHotKeyPtr()->GetText( DMKEY::MENU_CASHSHOP, szKey, MAX_FILENAME );
	std::wstring wsTooltip;
	DmCS::StringFn::Format( wsTooltip, L"%s [%s]", UISTRING_TEXT( "COMMON_TXT_CASHSHOP" ).c_str(), szKey );

	CsPoint ptPos;
	ptPos.x = m_ptRootClient.x + m_ptRootSize.x;
	ptPos.y = m_ptRootClient.y;		//2017-01-19-nova 캐쉬샵 아이콘 위에 표시 되도록		

	TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text(  ptPos, CsPoint::ZERO, wsTooltip.c_str() );
}

bool cCashShopButton::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void cCashShopButton::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
}
