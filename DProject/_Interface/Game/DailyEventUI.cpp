#include "stdafx.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"
#include "DailyEventUI.h"


cDailyEventUI::cDailyEventUI():m_pTimerBar(NULL)
{
}

cDailyEventUI::~cDailyEventUI()
{
}

cBaseWindow::eWINDOW_TYPE cDailyEventUI::GetWindowType()
{ 
	return WT_DAILY_EVENT_UI; 
}

void cDailyEventUI::Destroy()
{	
	cBaseWindow::Delete();	
}

void cDailyEventUI::DeleteResource()
{
	DeleteScript();
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

void cDailyEventUI::Create(int nValue /* = 0  */)
{		
	if( cBaseWindow::Init() == false )
		return;

	
	InitScript( NULL, CsPoint( 1024 - 220 - 5, 768 - 30 - 88 ), CsPoint( 220, 32 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );

	m_pTimerBar = NiNew cUI_TimerProgressBar;
	if( m_pTimerBar )
	{
		m_pTimerBar->Init(GetRoot(), CsPoint(105, 1), CsPoint(115, 30), false);
		m_pTimerBar->SetBackImage( "EventButton\\PlayTime_back.tga" );
		m_pTimerBar->SetGuageImage( "EventButton\\Event_gage.tga" );
		m_pTimerBar->SetCoverImage( "EventButton\\PlayTime_edge.tga" );

		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		//ti.s_Color = NiColor( 0.9f, 0.9f, 0.9f );
		ti.s_bOutLine = false;
		ti.s_Color = FONT_GREEN;
		ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
		m_pTimerBar->SetStringControl( &ti );
		m_pTimerBar->SetRenderText(true);
		m_pTimerBar->SetShowValueType( cUI_TimerProgressBar::eRemainingTime );
		
		std::wstring msg = UISTRING_TEXT( "DAILY_EVENT_PROGRESS_MSG" );
		msg += L"'";

		m_pTimerBar->SetHeadTailText( msg.c_str(), L"'" );
		m_pTimerBar->SetTimerText( L":", L":", L":", L"");
		m_pTimerBar->SetTimerType(cUI_TimerProgressBar::eMinSec);
		AddChildControl( m_pTimerBar );
	}

	UpdateDailyEventData();
}

cBaseWindow::eMU_TYPE cDailyEventUI::Update_ForMouse()
{	
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	

	if( m_pTimerBar &&  m_pTimerBar->GetRect().PtInRect( CURSOR_ST.GetPos() ) )
		return MUT_NONE;

	SAFE_POINTER_RETVAL( g_pGameIF, MUT_OUT_WINDOW );
	SAFE_POINTER_RETVAL( TOOLTIPMNG_STPTR->GetTooltip(), MUT_OUT_WINDOW );

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	std::list<sDailyEventItem>::iterator it = m_ItemsInfo.begin();
	for( ; it != m_ItemsInfo.end(); ++it )
	{
		CsRect rc( (*it).m_IconPos, (*it).m_IconPos + (*it).m_IconSize );
		if( rc.PtInRect( ptMouseLocalPos ) )
		{
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient() + (*it).m_IconPos, (*it).m_IconSize, TOOLTIP_MAX_SIZE, cTooltip::ITEM, (*it).m_ItemCode, cBaseWindow::WT_STORE );
			return MUT_NONE;
		}

	}

	return MUT_OUT_WINDOW;
}

void cDailyEventUI::Update(float const& fDeltaTime)
{
	UpdateScript(fDeltaTime);
}

void cDailyEventUI::Render()
{
	RenderScript();	
	std::list<sDailyEventItem>::iterator it = m_ItemsInfo.begin();
	for( ; it != m_ItemsInfo.end(); ++it )
		g_pIconMng->RenderItem( (*it).m_ItemCode, m_ptRootClient + (*it).m_IconPos, (*it).m_ItemCount );
}

void cDailyEventUI::ResetDevice()
{	
	ResetDeviceScript();
}

void cDailyEventUI::UpdateDailyEventData()
{
	SAFE_POINTER_RET( m_pTimerBar );
	m_pTimerBar->Reset();
	
	float fMaxTimeS = GetSystem()->GetDailyEventTotalTs();
	m_pTimerBar->SetTimer( 0.0f, fMaxTimeS );

	float fCurrentTimeS = GetSystem()->GetDailyEventCurrentTs();
	m_pTimerBar->SetProgressPos( fCurrentTimeS );
	m_pTimerBar->Start();

	m_ItemsInfo.clear();

	std::list<EventContents::sDailyEventData::sItems> const& items = GetSystem()->GetDailyEventItems();
	std::list<EventContents::sDailyEventData::sItems>::const_iterator it = items.begin();
	for( int n = 0; it != items.end(); ++it, ++n )
	{
		sDailyEventItem addItem;
		addItem.m_IconSize = CsPoint( 32, 32 );
		addItem.m_ItemCount = (*it).m_nItemCount;
		addItem.m_ItemCode = (*it).m_nItemCode;

		switch( n )
		{
		case 0:	addItem.m_IconPos = CsPoint( 70, 0 );			break;
		case 1:	addItem.m_IconPos = CsPoint( 35, 0 );			break;
		case 2:	addItem.m_IconPos = CsPoint( 0, 0 );			break;
		default:
			return;
		}

		m_ItemsInfo.push_back( addItem );
	}
}

bool cDailyEventUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eDailyEvent_Update, this );

	return true;
}

void cDailyEventUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
	switch( iNotifiedEvt )
	{
	case SystemType::eDailyEvent_Update:	UpdateDailyEventData();		break;
	}
}
