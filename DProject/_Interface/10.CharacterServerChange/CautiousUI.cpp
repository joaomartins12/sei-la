
#include "stdafx.h"
#include "CautiousUI.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CBeCautiousUI::CBeCautiousUI() : m_pBtnClose(NULL), m_pNextBtn(NULL), m_pBackBtn(NULL)
{
}

CBeCautiousUI::~CBeCautiousUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

bool CBeCautiousUI::Create()
{
	InitScript( "ServerRelocate\\server_popup.tga", CsPoint( (g_nScreenWidth - 1024) / 2, (g_nScreenHeight - 768) / 2 ), CsPoint( 1024, 768 ), false );
	AddTitle( UISTRING_TEXT("TAMER_SERVER_RELOCATE_TITLE").c_str(), CsPoint(0, 13 ), CFont::FS_16, NiColor::WHITE );

	m_pBtnClose = AddButton( CsPoint( 1024 - 30 - 10, 12 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pBtnClose )
		m_pBtnClose->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBeCautiousUI::_CloseWindow );

	AddSprite( CsPoint( 54, 113 ), CsPoint( 912, 519 ), "ServerRelocate\\server_sub_01.tga");

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor(0.0f,1.0f,0.0f) );
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_TENABLE_TITLE").c_str());
		AddText( &ti, CsPoint(60, 88));
	}

	{	// 테이머
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_TENABLE_TAMER_TITLE").c_str());
		 AddText( &ti, CsPoint(110, 177));
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.SetText(L"");
		ti.SetLineGabHeight( 3 );
		cText* pText = AddText( &ti, CsPoint(182, 124));
		pText->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_TENABLE_TAMER_MSG").c_str(), 750 );
	}

	{	// 디지몬
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_TENABLE_DIGIMON_TITLE").c_str());
		AddText( &ti, CsPoint(110, 332));
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.SetText(L"");
		ti.SetLineGabHeight( 3 );
		cText* pText = AddText( &ti, CsPoint(182, 260));
		pText->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_TENABLE_DIGIMON_MSG").c_str(), 750 );
	}

	//////////////////////////////////////////////////////////////////////////

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor(1.0f,0.0f,0.0f) );
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_CHANGE_TITLE").c_str());
		AddText( &ti, CsPoint(60, 457));
	}

	{	// 게임머니
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_CHANGE_MONEY_TITLE").c_str());
		AddText( &ti, CsPoint(110, 502));
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.SetText(L"");
		ti.SetLineGabHeight( 3 );
		cText* pText = AddText( &ti, CsPoint(182, 492));
		pText->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_CHANGE_MONEY_MSG").c_str(), 750 );
	}

	{	// 기타
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_CHANGE_ETC_TITLE").c_str());
		AddText( &ti, CsPoint(110, 579));
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.SetText(L"");
		ti.SetLineGabHeight( 3 );
		cText* pText = AddText( &ti, CsPoint(182, 547));
		pText->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_INFO_CHANGE_ETC_MSG").c_str(), 750 );
	}

	//////////////////////////////////////////////////////////////////////////

	// 뒤로 버튼
	m_pBackBtn = AddButton( CsPoint( ( 1024 - 128 ) / 2 - ((128 / 2) + 10) , 768 - 57 ), CsPoint( 128, 48 ), CsPoint( 0, 48 ), "Event\\event_btn.tga");	
	if( m_pBackBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_BACK_BTN").c_str() );
		m_pBackBtn->SetText( &ti );
		m_pBackBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBeCautiousUI::_BackPage );
	}

	// 다음 버튼
	m_pNextBtn = AddButton( CsPoint( ( 1024 - 128 ) / 2 + ((128 / 2) + 10), 768 - 57 ), CsPoint( 128, 48 ), CsPoint( 0, 48 ), "Event\\event_btn.tga");	
	if( m_pNextBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_NEXT_BTN").c_str());
		m_pNextBtn->SetText( &ti );
		m_pNextBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CBeCautiousUI::_NextPage );
	}

	return true;
}

void CBeCautiousUI::Destroy()
{
	DeleteScript();	
}

BOOL CBeCautiousUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	if( m_pBtnClose && m_pBtnClose->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pBackBtn && m_pBackBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pNextBtn && m_pNextBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	return FALSE;
}

void CBeCautiousUI::Render()
{
	RenderScript();
}

void CBeCautiousUI::ResetDevice()
{
	ResetDeviceScript();
}

void CBeCautiousUI::_CloseWindow(void* pSender, void* pData)
{
	SAFE_POINTER_RET( GetSystem() );
	GetSystem()->CloseServerChangePage();
}

void CBeCautiousUI::_BackPage(void* pSender, void* pData)
{
	SAFE_POINTER_RET( GetSystem() );
	GetSystem()->BackStep();
}

void CBeCautiousUI::_NextPage(void* pSender, void* pData)
{
	SAFE_POINTER_RET( GetSystem() );
	GetSystem()->NextStep();
}
//////////////////////////////////////////////////////////////////////////////////////////////

bool CBeCautiousUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	return true;
}

void CBeCautiousUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}