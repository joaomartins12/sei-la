
#include "stdafx.h"
#include "Channel.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

cChannel::cChannel():m_pText(NULL), m_pChannelChangeBtn(NULL)
{
}

cChannel::~cChannel()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
	Destroy();
}

cBaseWindow::eWINDOW_TYPE cChannel::GetWindowType()
{
	return WT_CHANNEL;
}

void cChannel::Destroy()
{
	cBaseWindow::Delete();
}

void cChannel::DeleteResource()
{
	DeleteScript();
}

void cChannel::Create(int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	InitScript( "Channel\\Ch_Bar.tga", CsPoint( 714, 2 ), CsPoint( 128, 28 ), false, IFREGION_X::RIGHT, IFREGION_Y::TOP );
	m_pChannelChangeBtn = AddButton( CsPoint( 101, 5 ), CsPoint( 19, 18 ), CsPoint( 0, 18 ), "Channel\\Ch_But.tga" );
	if( m_pChannelChangeBtn )
		m_pChannelChangeBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &cChannel::ShowChannelList );
	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_bOutLine = false;
	m_pText = AddText( &ti, CsPoint( 12, 7 ) );
	SetChannelTxt();
}

void cChannel::ShowChannelList(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pSender);

	if( !GetSystem()->RequestChannelList() )
		return;

	cButton* pBtn = static_cast<cButton*>(pSender);
	pBtn->SetEnable(false);
	cBaseWindow::PlayOpenSound();
}

void cChannel::Update(float const& fDeltaTime)
{
}

cBaseWindow::eMU_TYPE
cChannel::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;
	if( nsCsGBTerrain::g_nSvrLibType ==  nLIB::SVR_DUNGEON)
		return muReturn;
	m_pChannelChangeBtn->Update_ForMouse();
	return muReturn;
}

void cChannel::Render()
{
	if( nsCsGBTerrain::g_nSvrLibType !=  nLIB::SVR_DUNGEON)
		RenderScript();
}

void cChannel::ResetDevice()
{
	if( nsCsGBTerrain::g_nSvrLibType !=  nLIB::SVR_DUNGEON)
		ResetDeviceScript();
}

void cChannel::RecvChannelList()
{
	if( g_pGameIF->GetPopup() )
		g_pGameIF->GetPopup()->SetPopup( GetRootClient(), CsPoint( 128, 28 ), cPopUpWindow::CHANNEL_LIST );
}


bool cChannel::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;
	GetSystem()->Register( SystemType::eSpreadList, this );
	GetSystem()->Register( SystemType::eSetChannelTxt, this );
	return true;
}
void cChannel::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eSpreadList:
		{
			m_pChannelChangeBtn->SetEnable(true);
			RecvChannelList();
			break;
		}
	case SystemType::eSetChannelTxt:
		{
			SetChannelTxt();
			break;
		}
	default:
		break;
	}
	return;
}
void cChannel::SetChannelTxt()
{
	std::wstring sz;
	DmCS::StringFn::Format( sz, L"%s %d", UISTRING_TEXT( "COMMON_TXT_CHANNEL" ).c_str(), GetSystem()->GetCurrentChannelIdx() );
	m_pText->SetText( sz.c_str() );
}

