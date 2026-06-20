#include "stdafx.h"
#include "TutorialGuideWindow.h"

cTutorialGuideUI::cTutorialGuideUI():m_pTooltipImg(NULL),m_pSkipButton(NULL)
{
}

cTutorialGuideUI::~cTutorialGuideUI()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cTutorialGuideUI::Destroy()
{
	cBaseWindow::Delete();	
}

void cTutorialGuideUI::DeleteResource()
{
	DeleteScript();	
}

void cTutorialGuideUI::PreResetMap()
{
	Destroy();
}

void cTutorialGuideUI::ResetMap()
{
	Destroy();
}

void cTutorialGuideUI::Create( int nValue /*=0*/ )
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "Tutorial\\tooltip_bg.tga", CsPoint( 10, g_nScreenHeight - 520 ), CsPoint( 386, 512 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );	
	std::wstring titleName = UISTRING_TEXT("TUTORIAL_WINDOW_TITLE_NAME");
	AddTitle(titleName.c_str(),CsPoint(0, 4));
	m_rectTitleBar = CsRect( CsPoint::ZERO, CsSIZE( 386, 31 ) );
	m_pTooltipImg = AddSprite( CsPoint(1,32), CsPoint( 384, 480 ), " " );

	m_pSkipButton = AddButton( CsPoint( 300, 7 ), CsPoint( 80, 20 ), cButton::IMAGE_NOR_3, _CUR_ENG( "Skip" ) );	
	if( m_pSkipButton )
		m_pSkipButton->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cTutorialGuideUI::_TutorialSkip );
}

void cTutorialGuideUI::_TutorialSkip(void* pSender, void* pData)
{
	if( GetSystem() )
		GetSystem()->TutorialQuest_Skip_Ask();
}

void cTutorialGuideUI::Update(float const& fDeltaTime)
{	

	_UpdateMoveWindow();
}

cBaseWindow::eMU_TYPE cTutorialGuideUI::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if(m_pSkipButton && m_pSkipButton->Update_ForMouse() != cButton::ACTION_NONE )
		return muReturn;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	if( m_rectTitleBar.PtInRect( ptMouseLocalPos ) == TRUE )
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint(32,32), UISTRING_TEXT("TUTORIAL_WINDOW_TITLE_TOOLTIP").c_str(), CFont::FS_12);

	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( 386, 31 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

	return muReturn;
}

void cTutorialGuideUI::Render()
{
	RenderScript();	
}


void cTutorialGuideUI::ResetDevice()
{	
	ResetDeviceScript();
}

void cTutorialGuideUI::_ChangeTooltipImg( std::string const& sChangeImg )
{
	SAFE_POINTER_RET( m_pTooltipImg );
	m_pTooltipImg->ChangeTexture( sChangeImg.c_str() );
#ifdef LJW_TUTORIAL_RENEWAL_20191128
	// 4번 맵에서 툴팁 스킵 막음
	if (m_pSkipButton)
		m_pSkipButton->SetVisible(!GetSystem()->IsTutorialPlaying_ClientMode());
#endif
}

bool cTutorialGuideUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChangeShowImage, this );
	return true;
}

void cTutorialGuideUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eChangeShowImage:
		{
			std::string sValue;
			kStream >> sValue;
			_ChangeTooltipImg( sValue );
		}
		break;
	}
}