#include "stdafx.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

#include "ExpEvent.h"

cExpEventUI::cExpEventUI():m_pExpEventBtn(NULL),m_pExpHotEventBtn(NULL),m_nExpNumTextIdx(NUMBERSPRITE::GREEN)
{
}

cExpEventUI::~cExpEventUI()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cExpEventUI::Destroy()
{
	cBaseWindow::Delete();	
}

void cExpEventUI::DeleteResource()
{		
	DeleteScript(); 
}

void cExpEventUI::Create(int nValue /* = 0  */)
{		
	if( cBaseWindow::Init() == false )
		return; 

	InitScript( NULL, CsPoint( 1024 - 146, 691 ), CsPoint( 142, 39 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );

	m_pExpEventBtn = AddButton( CsPoint::ZERO, CsPoint( 142, 39 ), CsPoint( 142, 0 ), "Event\\ExeEvent.tga" );
	if( m_pExpEventBtn )
		m_pExpEventBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cExpEventUI::ShowToolTip);
	
	m_pExpHotEventBtn = AddButton( CsPoint::ZERO, CsPoint( 142, 39 ), CsPoint( 142, 0 ), "Event\\ExeHotEvent.tga" );
 	if( m_pExpHotEventBtn )
 		m_pExpHotEventBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cExpEventUI::ShowToolTip);

	UpdateExpData();
}

cBaseWindow::eMU_TYPE cExpEventUI::Update_ForMouse()
{	
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if(m_pExpEventBtn && m_pExpEventBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return cBaseWindow::MUT_NONE;
	if(m_pExpHotEventBtn && m_pExpHotEventBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return cBaseWindow::MUT_NONE;

	return muReturn;
}

void cExpEventUI::Update(float const& fDeltaTime)
{
}

void cExpEventUI::Render()
{
	RenderScript();	

	unsigned int nExpRate = GetSystem()->GetCurrentExpRate();
	g_pIconMng->RenderNumber( DT_CENTER, m_nExpNumTextIdx, nExpRate, GetRootClient() + CsPoint( 72 + 5, 3 ), CsPoint( 30, 30 ), true );	
}

void cExpEventUI::ResetDevice()
{	
	ResetDeviceScript();
}

void cExpEventUI::UpdateExpData()
{
	switch( GetSystem()->GetExpRateType() )
	{
	case 1:			// 일반 경험치 이벤트
		{
			m_nExpNumTextIdx = NUMBERSPRITE::GREEN;
			if( m_pExpEventBtn )
				m_pExpEventBtn->SetVisible(true);
			if( m_pExpHotEventBtn )
				m_pExpHotEventBtn->SetVisible(false);
		}
		break;
	case 2:			// 버닝 경험치 이벤트
		{
			m_nExpNumTextIdx = NUMBERSPRITE::ORANGE;
			if( m_pExpEventBtn )
				m_pExpEventBtn->SetVisible(false);
			if( m_pExpHotEventBtn )
				m_pExpHotEventBtn->SetVisible(true);
		}
		break;
	}
}

void cExpEventUI::ShowToolTip(void* pSender, void* pData)
{
	SAFE_POINTER_RET( g_pGameIF );
	cPopUpWindow* pPopupWin = g_pGameIF->GetPopup();
	SAFE_POINTER_RET( pPopupWin );
	unsigned int pNextExpRate = GetSystem()->GetNextExpRate();
	if( pNextExpRate > 0 )
		pPopupWin->SetPopup( GetRootClient(), CsPoint( 46, 15 ), cPopUpWindow::EXP_EVENT, GetSystem()->GetCurrentExpRate(), pNextExpRate );	
}

bool cExpEventUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;


	GetSystem()->Register( SystemType::eUPDATE_EXP_EVENT_DATA, this );

	return true;
}

void cExpEventUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
	switch( iNotifiedEvt )
	{
	case SystemType::eUPDATE_EXP_EVENT_DATA:
		{
			UpdateExpData();
		}
		break;
	}
}
