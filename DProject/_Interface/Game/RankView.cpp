#include "stdafx.h"
#include "RankView.h"

void cRankView::Destroy()
{	
	cBaseWindow::Delete();	
}

void cRankView::ResetMap()
{
	Close( false );
}

void cRankView::DeleteResource()
{		
	m_strExplain.Delete();
	DeleteScript();	
}

void cRankView::Create(int nValue /* = 0  */)
{	
	cCommunityContent::sGUILD_INFO guildInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
	assert_cs( guildInfo.s_szGuildName.size() > 0 );
	SetRootClient( CsPoint( g_nScreenWidth / 2 - 212, g_nScreenHeight / 2 - 40 ) );			

	cBaseWindow::Init();  
	InitScript( "Guild\\GuildWin_B.tga", m_ptRootClient, CsPoint( 424, 180 ), false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );	
	AddTitle( UISTRING_TEXT( "COMMUNITY_GUILD_TITLE" ).c_str() );
	m_pCancelButton = AddButton( CsPoint( 404, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	AddButton( CsPoint( 14, 38 ), CsPoint( 106, 22 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_PREVIOUS" ).c_str(), cWindow::SD_Ba1 ); 	
	AddButton( CsPoint( 122, 38 ), CsPoint( 106, 22 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_RANK_BTN_CURRENT" ).c_str(), cWindow::SD_Ba1 ); 	
	AddButton( CsPoint( 229, 38 ), CsPoint( 181, 22 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_RANK_BTN_DUTY" ).c_str(), cWindow::SD_Ba1 ); 	

	int nStrDeltaX = 109;

	CsPoint ptPos;	
	int nBase;	
	cText::sTEXTINFO ti;
	ti.s_eTextAlign = DT_CENTER;	
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );	

	for( int i = 1; i < cCommunityContent::GR_END; i++ )
	{
		cString* pString = NiNew cString;

		switch( i )
		{
		case 1:
			ti.SetText( UISTRING_TEXT( "COMMUNITY_RANK_MASTER" ).c_str() );	
			break;
		case 2:
			ti.SetText( UISTRING_TEXT( "COMMUNITY_RANK_SUBMASTER" ).c_str() );	
			break;
		case 3:
			ti.SetText( UISTRING_TEXT( "COMMUNITY_RANK_DATS_MEMBER" ).c_str() );	
			break;
		case 4:
			ti.SetText( UISTRING_TEXT( "COMMUNITY_RANK_GENERAL_MEMBER" ).c_str() );	
			break;
		case 5:
			ti.SetText( UISTRING_TEXT( "COMMUNITY_RANK_NEW_MEMBER" ).c_str() );	
			break;
		default:
			assert_cs( false );
			break;
		}

		pString->AddText( &ti );	
		cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
		pString->TailAddSizeX( nStrDeltaX - ptPos.x );		

		std::pair<cCommunityContent::eRANK, std::wstring*> guildRankName;
		guildRankName.first = static_cast<cCommunityContent::eRANK>(i);
		GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildRankName );
		ti.SetText( (TCHAR*)guildRankName.second->c_str() );	
		pString->AddText( &ti );	
		cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
		pString->TailAddSizeX( nStrDeltaX - ptPos.x );

		std::pair<cCommunityContent::eRANK, std::wstring*> guildRankDesc;
		guildRankDesc.first = static_cast<cCommunityContent::eRANK>(i);
		GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_DECRIPT, &guildRankDesc );
		ti.SetText( (TCHAR*)guildRankDesc.second->c_str() );	
		pString->AddText( &ti );	

		m_strExplain.AddTail( pString );
	}	
}

cBaseWindow::eMU_TYPE
cRankView::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;	

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		CloseWindow();
		return muReturn;
	default:
		return muReturn;
	}		

	return muReturn;
}

void cRankView::CloseWindow()
{
	Close();
}

bool cRankView::OnEscapeKey()
{
	CloseWindow();
	return true;
}


void cRankView::Update(float const& fDeltaTime)
{
	
}

void cRankView::Render()
{
	RenderScript();		

	m_strExplain.Render( GetRootClient() + CsPoint( 16, 65 ), 5 );
}

void cRankView::ResetDevice()
{	
	ResetDeviceScript();	
	m_strExplain.ResetDevice();
}