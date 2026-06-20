
#include "stdafx.h"
#include "TermsAndConditionUI.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CTermsAndConditionUI::CTermsAndConditionUI() : m_pNextBtn(NULL), m_pCheckBox(NULL), m_pBtnClose(NULL)
{

}

CTermsAndConditionUI::~CTermsAndConditionUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

bool CTermsAndConditionUI::Create()
{
	InitScript( "ServerRelocate\\server_popup.tga", CsPoint( (g_nScreenWidth - 1024) / 2, (g_nScreenHeight - 768) / 2 ), CsPoint( 1024, 768 ), false );
	AddTitle( UISTRING_TEXT("TAMER_SERVER_RELOCATE_TITLE").c_str(), CsPoint(0, 13 ), CFont::FS_16, NiColor::WHITE );

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_14, NiColor(0.0f,0.7176f,0.93f) );
	ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_TERMNCONDITION_TITLE").c_str());
	AddText( &ti, CsPoint(60, 88));

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.SetText(L" ");
		ti.SetLineGabHeight( 5 );
		cText* pMsg = AddText( &ti, CsPoint(60, 120));
		pMsg->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_TERMNCONDITION_MSG").c_str(), 904 );		
	}

	m_pBtnClose = AddButton( CsPoint( 1024 - 30 - 10, 12 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pBtnClose )
		m_pBtnClose->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CTermsAndConditionUI::_CloseWindow );

	m_pCheckBox = AddCheckBox(  CsPoint(1024 - 80, 768 - 110), CsPoint( 19, 19 ), CsPoint( 0, 19 ), "CheckBT.tga", cWindow::SD_Wi1 );
	if( m_pCheckBox )
		m_pCheckBox->AddEvent(cCheckBox::BUTTON_LBUP_EVENT, this, &CTermsAndConditionUI::_PressCheckBox);

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor(0.0f,0.7176f,0.93f) );
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_TERMNCONDITION_AGREE_MSG").c_str());
		AddText( &ti, CsPoint(1024 - 100, 768 - 110));
	}
	
	// ´ÙÀ½ ¹öÆ°
	m_pNextBtn = AddButton( CsPoint( ( 1024 - 128 ) / 2 , 768 - 57 ), CsPoint( 128, 48 ), CsPoint( 0, 48 ), "Event\\event_btn.tga");	
	if( m_pNextBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_NEXT_BTN").c_str() );
		m_pNextBtn->SetText( &ti );
		m_pNextBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CTermsAndConditionUI::_NextPage );
		m_pNextBtn->SetEnable(false);
	}

	return true;
}

void CTermsAndConditionUI::Destroy()
{
	DeleteScript();	
}

BOOL CTermsAndConditionUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	if( m_pBtnClose && m_pBtnClose->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;
	
	if( m_pCheckBox && m_pCheckBox->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pNextBtn && m_pNextBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;


	return FALSE;
}

void CTermsAndConditionUI::Render()
{
	RenderScript();
}

void CTermsAndConditionUI::ResetDevice()
{
	ResetDeviceScript();
}

void CTermsAndConditionUI::_CloseWindow(void* pSender, void* pData)
{
	SAFE_POINTER_RET( GetSystem() );
	GetSystem()->CloseServerChangePage();
}

void CTermsAndConditionUI::_PressCheckBox(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pSender);
	SAFE_POINTER_RET(m_pNextBtn);
	cCheckBox* pCheck = static_cast<cCheckBox*>(pSender);
	if( pCheck->IsCheck() )
		m_pNextBtn->SetEnable(true);
	else
		m_pNextBtn->SetEnable(false);
}

void CTermsAndConditionUI::_NextPage(void* pSender, void* pData)
{
	SAFE_POINTER_RET( GetSystem() );
	GetSystem()->NextStep();
}

//////////////////////////////////////////////////////////////////////////////////////////////

bool CTermsAndConditionUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CTermsAndConditionUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}