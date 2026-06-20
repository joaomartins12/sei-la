#include "stdafx.h"
#include "GuildSet.h"

void cGuildSet::Destroy()
{	
	cBaseWindow::Delete();	
}

void cGuildSet::ResetMap()
{
	Close( false );
}

void cGuildSet::DeleteResource()
{	
	SAFE_NIDELETE( m_pCombo );	
	DeleteScript();
}

void cGuildSet::Create(int nValue /* = 0  */)
{	
	cCommunityContent::sGUILD_PERSON_INFO myInfo;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILD_MYINFO, &myInfo );
	assert_cs( myInfo.s_eRank == cCommunityContent::GR_MASTER );

	SetRootClient( CsPoint( g_nScreenWidth / 2 - 155, g_nScreenHeight / 2 - 40 ) );
	m_pCombo = NULL;		

	cBaseWindow::Init();
	
	InitScript( "Guild\\Guild_RankSet.tga", m_ptRootClient, CsPoint( 310, 80 ), false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( UISTRING_TEXT( "COMMUNITY_SET_CHANGE_DUTY" ).c_str() );
	AddText( &ti, CsPoint( 16, 10 ) );

	m_pCancelButton = AddButton( CsPoint( 280, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	
	m_pChangeButton = AddButton( CsPoint( 185, 41 ), CsPoint( 58, 24 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMUNITY_SET_BTN_TITLE_CHANGE" ).c_str(), cWindow::SD_Ba1 ); 	
	m_pExplainButton = AddButton( CsPoint( 245, 41 ), CsPoint( 58, 24 ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "COMMUNITY_SET_BTN_DUTY_CHANGE" ).c_str(), cWindow::SD_Ba1 ); 	

	m_pCombo = NiNew cComboBox;
	m_pCombo->Init( NULL, cComboBox::CT_NORMAL, CsPoint::ZERO, CsPoint( 165, 27 ), CFont::FS_10, DT_CENTER, true, false, false );
	for( int i = 1; i < 6; i++ )
	{
		std::pair<cCommunityContent::eRANK, std::wstring*> guildRankName;
		guildRankName.first = static_cast<cCommunityContent::eRANK>(i);
		GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDRANK_NAME, &guildRankName );
		m_pCombo->AddItem( guildRankName.second, i, FONT_WHITE );
	}

	m_pCombo->SetCurSel( 0 );
	m_nValue = 1;
}

cBaseWindow::eMU_TYPE
cGuildSet::Update_ForMouse()
{
	if( m_pCombo != NULL )
	{
		int nComboSel = m_pCombo->Update_ForMouse();
		if( nComboSel != cComboBox::CI_INVALIDE )
		{
			switch( nComboSel )
			{
			case cComboBox::CI_INRECT:			
				return MUT_NONE;
			case cComboBox::CI_CLICK:
			case cComboBox::CI_SAME_CLICK:
				return MUT_CLICK_FOCUS;
			default:
				{
					m_nValue = m_pCombo->GetCurSel_Data();						
				}				
				return MUT_CLICK_FOCUS;
			}
		}
	}	

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

	switch( m_pChangeButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			if( g_pCreateName )
			{
				// 이름 짓는 윈도우 닫아준다.
				cCreateName::DeleteInstance();	
			}
			cCreateName* pCName = cCreateName::NewInstance( cBaseWindow::WT_GUILDRANK_NAME, 0, CsPoint( -1, -1 ) );
			if( pCName )
				pCName->SetValue1( m_nValue );
			Close();	
		}		
		return muReturn;
	default:
		return muReturn;
	}	

	switch( m_pExplainButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			if( g_pCreateName )
			{
				// 이름 짓는 윈도우 닫아준다.
				cCreateName::DeleteInstance();	
			}
			cCreateName* pCName = cCreateName::NewInstance( cBaseWindow::WT_GUILDRANK_EXPLAIN, 0, CsPoint( -1, -1 ) );
			if( pCName )
				pCName->SetValue1( m_nValue );
			Close();
		}		
		return muReturn;
	default:
		return muReturn;
	}	


	return muReturn;
}

void cGuildSet::CloseWindow()
{
	Close();
}

bool cGuildSet::OnEscapeKey()
{
	CloseWindow();
	return true;
}

void cGuildSet::Update(float const& fDeltaTime)
{
	if( m_pCombo )
		m_pCombo->Update(fDeltaTime);	
}

void cGuildSet::Render()
{
	RenderScript();		

	if( m_pCombo )
		m_pCombo->Render( CsPoint( 15, 40 ) + GetRootClient() );
}

void cGuildSet::ResetDevice()
{	
	ResetDeviceScript();		

	if( m_pCombo != NULL )
		m_pCombo->ResetDevice();
}