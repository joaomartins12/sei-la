#include "stdafx.h"
#include "Community.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

cCommunity::cCommunity() //: m_bOnOff(true)
{
	m_Time.SetDeltaTime( 60000 );
}
cCommunity::~cCommunity()
{}
cBaseWindow::eWINDOW_TYPE cCommunity::GetWindowType()
{
	return WT_COMMUNITY;
}

void cCommunity::Destroy()
{
	cBaseWindow::Delete();
}

void cCommunity::DeleteResource()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	if( m_pEditName )
		m_pEditName->ReleaseFocus();	

	// 이름 짓는 윈도우 닫아준다.
	cCreateName::DeleteInstance( cBaseWindow::WT_INVITE_GUILD_NAME, 0 );
	cCreateName::DeleteInstance( cBaseWindow::WT_CREATE_GUILD_NAME, 0 );
	cCreateName::DeleteInstance( cBaseWindow::WT_GUILDRANK_NAME, 0 );
	cCreateName::DeleteInstance( cBaseWindow::WT_GUILDRANK_EXPLAIN, 0 );

	m_strPersonInfo.Delete();
	m_strNotice.Delete();

	_DeletePbButton();
	_DeletePbText();	

	SAFE_NIDELETE( m_pRadioBtn );		
	SAFE_NIDELETE( m_pEditName );
	SAFE_NIDELETE( m_pGuildMark );
#ifdef GUILD_RENEWAL
	if( m_pOnOffCheck )
		SAFE_NIDELETE( m_pOnOffCheck );

	m_strGuildSkill.Delete();
	if( m_pGSPoint )
		SAFE_NIDELETE( m_pGSPoint );
	if( m_pOnOffText )
		SAFE_NIDELETE( m_pOnOffText );
	if( m_pSkillNameBG )
		SAFE_NIDELETE( m_pSkillNameBG );
	if( m_pLine )
		SAFE_NIDELETE( m_pLine );
	if( m_pScrollBar )
		SAFE_NIDELETE( m_pScrollBar );
	if( m_pScrollBar_S )
		SAFE_NIDELETE( m_pScrollBar_S );
	if( m_pIconBG )
		SAFE_NIDELETE( m_pIconBG );
	for( int i = 0 ; i < IF_BASESKILL_COUNT ; i++ )
		SAFE_NIDELETE( m_pReqLvText[i] );
	if( m_pOnMask )
		SAFE_NIDELETE( m_pOnMask );
#else
	SAFE_NIDELETE( m_pCombo );
#endif

	DeleteScript();
}
bool cCommunity::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );

	if( !GetSystem() )
		return false;
	GetSystem()->Register( SystemType::SetNoticeStr, this );
	GetSystem()->Register( SystemType::SetGuildInfo, this );
	GetSystem()->Register( SystemType::SetUITab, this );
	GetSystem()->Register( SystemType::SetGuildPersonCnt, this );
	GetSystem()->Register( SystemType::MakeHistory, this );
	GetSystem()->Register( SystemType::SetGuildPerson, this );
	
	

	return true;
}
void cCommunity::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::SetNoticeStr:
		{
			std::wstring szNotice;
			kStream >> szNotice;
			SetNotice( szNotice.c_str() );
		}
		break;
	case SystemType::SetGuildInfo:
		{
			MakeGuildInfo();
		}
		break;
	case SystemType::SetUITab:
		{
			eTAB enTab;
			kStream>>enTab;
			SetTab(enTab);
		}
		break;
	case  SystemType::CloseGuildMenuPopup:
		{
			int nIdx;
			kStream>>nIdx;
			if( nIdx == GetCurSelect() )
				g_pGameIF->GetPopup()->ClosePopup( cPopUpWindow::GUILD_MENU );
		}
		break;
	case  SystemType::MakeHistory:
		{
			_MakeHistory();
		}
		break;
	case  SystemType::SetGuildPersonCnt:
		{
#ifdef GUILD_RENEWAL
			_SetGuildPersonCnt();
#endif
		}
		break;
	case  SystemType::SetGuildPerson:
		{
			int nIdx;
			kStream >> nIdx;
			cCommunityContent::sGUILD_PERSON_INFO* personInfo = GetSystem()->GetPersonInfo(nIdx);
			if (nIdx == m_nRealDataIdx)
				g_pGameIF->GetPopup()->ClosePopup( cPopUpWindow::GUILD_MENU );	
			if(m_eRadioIndex == TAB_GUILD)
			{
#ifdef GUILD_RENEWAL
				_AddGuildPerson( personInfo, nIdx );
#else
				AddGuildPerson( personInfo, nIdx );
#endif
			}
		}
		break;
	case  SystemType::SetGuildSkill:
		{
#ifdef GUILD_RENEWAL
			if (m_eRadioIndex == TAB_GUILDSKILL)
				_MakeGuildSkill();
#endif
		}
		break;
		
	default:
		break;
	}
}
void cCommunity::_DeletePbButton()
{
	int nSize = static_cast<int>(m_pbButton.size());
	for( int i = 0; i < nSize; i++ )
	{
		cButton* pButton = m_pbButton.at( i );				
		SAFE_NIDELETE( pButton );		
	}
	m_pbButton.clear();
}

void cCommunity::_DeletePbText()
{
	int nSize = static_cast<int>(m_pbText.size());	
	for( int i = 0; i < nSize; i++ )
	{
		cText* pText = m_pbText.at( i );				
		SAFE_NIDELETE( pText );		
	}
	m_pbText.clear();
}

void cCommunity::ResetMap()
{
	Close( false );	
}

void cCommunity::ResetDevice()
{
	if( m_pGuildMark )
		m_pGuildMark->ResetDevice();

	m_strPersonInfo.ResetDevice();	
	m_strNotice.ResetDevice();

	if( m_pRadioBtn )
		m_pRadioBtn->ResetDevice();
	
#ifdef GUILD_RENEWAL
	if( m_pOnOffCheck )
		m_pOnOffCheck->ResetDevice();

	m_strGuildSkill.ResetDevice();
	if( m_pGSPoint )
		m_pGSPoint->ResetDevice();
	if( m_pOnOffText )
		m_pOnOffText->ResetDevice();
	if( m_pSkillNameBG )
		m_pSkillNameBG->ResetDevice();
	if( m_pLine )
		m_pLine->ResetDevice();
	if( m_pScrollBar )
		m_pScrollBar->ResetDevice();
	if( m_pScrollBar_S )
		m_pScrollBar_S->ResetDevice();
	if( m_pIconBG )
		m_pIconBG->ResetDevice();
	for( int i = 0 ; i < IF_BASESKILL_COUNT ; i++ )
		m_pReqLvText[i]->ResetDevice();
	if( m_pOnMask )
		m_pOnMask->ResetDevice();
#else
	if( m_pCombo )
		m_pCombo->ResetDevice();
#endif
	int nSize = static_cast<int>(m_pbButton.size());
	for( int i = 0; i < nSize; i++ )
	{
		cButton* pButton = m_pbButton.at( i );
		pButton->ResetDevice();
	}

	int nTextSize = static_cast<int>(m_pbText.size());
	for( int i = 0; i < nTextSize; i++ )
	{
		cText* pText = m_pbText.at( i );				
		pText->ResetDevice();
	}

	if( m_pEditName )
		m_pEditName->ResetDevice();

	ResetDeviceScript();
}

bool cCommunity::OnEscapeKey()
{
	//글로벌 인풋 레벨에서 cEditBox가 활성화 되있으면 키다운 esc를 패스한다. 즉 들어올일이 없다. 무의미한 코드.
//	if( m_bNotice == true )
//	{	
//		assert_cs( m_pEditName != NULL );	
//
//		SetNotice( GetSystem()->GetGuildInfo()->s_szNotice.c_str() );		
//		m_pEditName->ReleaseFocus();
//		SAFE_NIDELETE( m_pEditName );
//		m_bNotice = false;		
//		m_pNotice->SetText( _LAN( "공지 작성" ) );
//
//		m_strNotice.ReleaseCursor();
//#ifdef IME_WINDOW_POSITION
//		g_bIMEPosition = false;
//#endif
//		return true;
//	}

	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILDSET );
	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILDRANK_VIEW );	
	Close(); 
	return true; 
}

bool cCommunity::OnEnterKey()
{
	if( m_bNotice == true )
	{		
		assert_cs( m_pEditName != NULL );	

		if( NULL == net::game )
			return true;

		std::wstring tempNotice = nBase::ltrim( m_szTempNotice );
		if( tempNotice.empty() )
			tempNotice = L" ";

		bool bReleaseCursor = true;
		if( GetSystem()->IsHaveMyInfo() == false || GetSystem()->GetMyInfo().s_eRank != cCommunityContent::GR_MASTER )
			bReleaseCursor = false;

		if( Language::SUCCEEDED != Language::CheckGuildNotice( tempNotice ) )
		{
			cPrintMsg::PrintMsg(30203);
			bReleaseCursor = false;
		}
		if( bReleaseCursor == true && net::game->SendChangeGuildNotice( tempNotice.c_str() ) )
		{
			m_pEditName->ReleaseFocus();
			SAFE_NIDELETE( m_pEditName );		
			m_bNotice = false;	
			m_pNotice->SetText( UISTRING_TEXT( "COMMUNITY_NOTICE_WRITE" ).c_str() );
		}
		m_strNotice.ReleaseCursor();
		return true;	
	}
	
	return false;
}

void cCommunity::SetNotice( TCHAR const* szNotice )
{
	m_strNotice.Delete();

	cText::sTEXTINFO ti;		
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );	
	g_pStringAnalysis->Cut( &m_strNotice, 372, szNotice, &ti );
}

void cCommunity::Create( int nValue /*=0*/ )
{
	if(!cBaseWindow::Init())
		return;

	SetRootClient( CsPoint( 100, 100 ) );

	m_strPersonInfo.Delete();	
	m_strNotice.Delete();	
	
	m_bSort = false;
	m_bNotice = false;
	m_bClickMask = false;
	m_eRadioIndex = TAB_GUILD;	
	m_pEditName = NULL;
	m_pGuildMark = NULL;
#ifdef GUILD_RENEWAL
	memset( m_szTempNotice, 0, MAX_PATH );
#else
	m_pCombo = NULL;	// 리뉴얼 - 길드 정보의 콤보박스 제외
#endif

	InitScript( "Guild\\GuildWin.tga", m_ptRootClient, CsPoint( 413, 502 ), true, IFREGION_X::CENTER, IFREGION_Y::CENTER );	
	AddTitle( UISTRING_TEXT( "COMMON_TXT_COMMUNITY" ).c_str() );	
	
	if( g_pCharMng->GetTamerUser()->GetCharOption()->IsPlag( CCharOption::GUILD_MARK ) == true )
	{		
		char szFileName[ MAX_PATH ];
		//addsprite는 내부에서 경로를 data//interface로 잡고 있기 때문에 그 뒤의 경로만 붙인다.길드마크는 interface외부에 있기 때문에 addsprite할수 없다.
		sprintf_s( szFileName, MAX_PATH, "Data\\NP\\Guildmark\\%d_%d.bmp", g_pCharMng->GetTamerUser()->GetCharOption()->GetGuildUID(), net::cluster );
		m_pGuildMark = NiNew cSprite;
		m_pGuildMark->Init( m_pRoot, CsPoint( 14, 47 ), CsPoint( 54, 54 ), szFileName, false, NiColor::WHITE, false );
	}	
	m_pCancelButton = AddButton( CsPoint( 390, 8 ), CsPoint( 16, 16 ), CsPoint( 0, 16 ), "System\\Ch_Close.tga" );	

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, NiColor( 1.0f, 1.0f, 0.5f ) );			
	
	ti.SetText( UISTRING_TEXT( "COMMUNITY_GUILD_NAME" ).c_str() );
	AddText( &ti, CsPoint( 83, 48 ) );

	ti.SetText( UISTRING_TEXT( "COMMUNITY_GUILD_MASTER" ).c_str() );
	AddText( &ti, CsPoint( 83, 69 ) );
	
	ti.SetText( UISTRING_TEXT( "COMMUNITY_GUILD_LEVEL" ).c_str() );
	AddText( &ti, CsPoint( 83, 92 ) );

	ti.SetText( UISTRING_TEXT( "COMMUNITY_NOTICE" ).c_str() );
	AddText( &ti, CsPoint( 30, 123 ) );

#ifdef GUILD_RENEWAL
	ti.SetText( UISTRING_TEXT( "COMMUNITY_CONTRIBUTION_GRADE" ).c_str() );
	AddText( &ti, CsPoint( 183, 92 ) );

	CsPoint ptPersonNum = CsPoint( 350, 251 );	// 접속 인원 수 위치
#else
	ti.SetText( UISTRING_TEXT( "COMMUNITY_FAME_LEVEL" ).c_str() );
	AddText( &ti, CsPoint( 183, 92 ) );

	ti.SetText( UISTRING_TEXT( "COMMUNITY_CONTRIBUTION_RANK" ).c_str() );
	AddText( &ti, CsPoint( 283, 92 ) );

	CsPoint ptPersonNum = CsPoint( 234, 251 );
#endif

	ti.SetText( L"" );
	m_pGuildPersonNum = AddText(&ti, ptPersonNum);

	ti.s_Color = FONT_WHITE;
	ti.s_eTextAlign = DT_CENTER;

	ti.SetText( UISTRING_TEXT( "COMMUNITY_NOTICE_WRITE" ).c_str() );
	
	m_pNotice = AddButton( CsPoint( 342, 121 ), CsPoint( 62, 19 ), cButton::IMAGE_NOR_1, &ti );
	bool bEnable =false;
	if (GetSystem()->IsHaveMyInfo() == true)
		bEnable = ( GetSystem()->GetMyInfo().s_eRank == cCommunityContent::GR_MASTER ) ? true : false;
	m_pNotice->SetEnable( bEnable );
	m_strNotice.SetUnderLineCursor();


#ifdef GUILD_RENEWAL
	// 오프라인 보기 체크박스. 텝에 따라 선택적으로 보이게 하기 위해 따로 잡음
	m_pOnOffCheck = NiNew cCheckBox;
	m_pOnOffCheck->Init( m_pRoot, CsPoint( 20, 250 ), CsPoint( 16, 16 ), "System\\Check.tga", false );
	m_pOnOffCheck->SetTexToken( CsPoint( 0, 20 ) );
	m_pOnOffCheck->SetCheck( GetSystem()->IsShowOfflinePerson() );	

	
	// 오프라인 보기 텍스트
	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( UISTRING_TEXT( "COMMUNITY_OFFLINE_MEMBER_INDICATOR" ).c_str() );
	m_pOnOffText = NiNew cText;
	m_pOnOffText->Init( m_pRoot, CsPoint( 42, 251 ), &ti, false );

	// 길드 스킬 아이콘 BG
	m_pIconBG = NiNew cSprite;
	m_pIconBG->Init( NULL, CsPoint::ZERO, CsPoint(44, 44), "Skill\\IconBG.tga", false );

	// 길드 Base스킬 요구레벨 텍스트
	for( int i = 0 ; i < IF_BASESKILL_COUNT ; i++ )
	{
		m_pReqLvText[i] = NiNew cText;
		m_pReqLvText[i]->Init(m_pRoot, CsPoint::ZERO, &ti, false );
	}

	// 길드 스킬 포인트 텍스트
	m_pGSPoint = NiNew cText;
	m_pGSPoint->Init( m_pRoot, CsPoint( 42, 251 ), &ti, false );

	// 스킬 이름 BG
	m_pSkillNameBG = NiNew cSprite;
	m_pSkillNameBG->Init( NULL, CsPoint::ZERO, CsPoint( 115, 32 ), "Guild\\GuildName.tga", false );
	
	// Base 스킬 / Active 스킬 구분선
	m_pLine = NiNew cSprite;
	m_pLine->Init( NULL, CsPoint::ZERO, CsPoint( 350, 5 ), "Control_G\\Popup\\N2Dlg_Line.tga", false );

	// 사용 중인 Active 스킬 아이콘 마스크
	m_pOnMask = NiNew cSprite;
	m_pOnMask->Init( NULL, CsPoint::ZERO, CsPoint( 32, 32 ), "Guild\\Guild_On.tga", false );

	// 길드 UI 탭
	m_pRadioBtn = NiNew cRadioButton;	
	m_pRadioBtn->Init( m_pRoot, CsPoint( 19, 221 ), CsPoint( 68, 24 ), cCheckBox::IMAGE_RADIO_4, UISTRING_TEXT( "COMMUNITY_BTN_MEMBER" ).c_str(), false );
	m_pRadioBtn->AddNode( CsPoint( 87, 221 ), CsPoint( 68, 24 ), cCheckBox::IMAGE_RADIO_4, UISTRING_TEXT( "COMMUNITY_BTN_GUILD_SKILL" ).c_str() );
	m_pRadioBtn->AddNode( CsPoint( 155, 221 ), CsPoint( 68, 24 ), cCheckBox::IMAGE_RADIO_4, UISTRING_TEXT( "COMMUNITY_BTN_HISTORY" ).c_str() );
	m_pRadioBtn->AddNode( CsPoint( 223, 221 ), CsPoint( 68, 24 ), cCheckBox::IMAGE_RADIO_4, UISTRING_TEXT( "COMMUNITY_BTN_MANAGE" ).c_str() );

	// Active 스킬 아이콘 위치
	CsPoint ptActPos( 153, 351 );
	for( int y = 0 ; y < IF_ACTIVESKILL_ROW ; y++ )
	{
		for( int x = 0 ; x < IF_ACTIVESKILL_COL ; x++ )
		{
			m_ActSkillIcon[ (y*IF_ACTIVESKILL_COL) + x ].Init( CsRect( ptActPos, CsSIZE( 32, 32 ) ) );
			ptActPos.x += 58;
		}
		ptActPos.x = 153;
		ptActPos.y += 47;
	}

	// 길드 UI 스크롤바
	m_pScrollBar = NiNew cScrollBar;
	m_pScrollBar->Init( cScrollBar::TYPE_1, m_pRoot, CsPoint( 391, 270 ), CsPoint( 16, 221 ), CsPoint( 16, 15 ), CsRect( CsPoint( 18, 270 ), CsSIZE( 376+16, 230 ) ), 10, false );

	// 길드 스킬 스크롤바( Active 스킬만 스크롤 하기 위해 사이즈/범위 조정 )
	m_pScrollBar_S = NiNew cScrollBar;
	m_pScrollBar_S->Init( cScrollBar::TYPE_1, m_pRoot,  CsPoint( 391, 350 ), CsPoint( 16, 141 ), CsPoint( 16, 15 ), CsRect( CsPoint( 18, 350 ), CsSIZE( 376+16, 130 ) ), 3, false );

	//m_pScrollBar_S = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 391, 350 ), CsPoint( 16, 141), CsPoint( 16, 15 ), CsRect( CsPoint( 18, 350 ), CsSIZE( 376+16, 130 ) ), 4 );

	// Active 스킬 이름 리스트
	m_strGuildSkill.Delete();
	m_strGuildSkill.SetBuddy( m_pScrollBar_S );
#else

	m_pOnOffCheck = AddCheckBox( CsPoint( 20, 250 ), CsPoint( 16, 16 ), CsPoint( 0, 20 ), "System\\Check.tga" );
	m_pOnOffCheck->SetCheck( GetSystem()->IsShowOfflinePerson() );

	ti.s_eTextAlign = DT_LEFT;
	ti.SetText( UISTRING_TEXT( "COMMUNITY_OFFLINE_MEMBER_INDICATOR" ).c_str() );
	AddText( &ti, CsPoint( 42, 252 ) );	

	m_pScrollBar = AddScrollBar( cScrollBar::TYPE_1, CsPoint( 391, 270 ), CsPoint( 16, 221 ), CsPoint( 16, 15 ), CsRect( CsPoint( 18, 270 ), CsSIZE( 376+16, 230 ) ), 10 );

	m_pCombo = NiNew cComboBox;
	m_pCombo->Init( NULL, cComboBox::CT_NORMAL, CsPoint::ZERO, CsPoint( 105, 22 ), CFont::FS_9, DT_CENTER, true, false, false );		
	m_pCombo->AddItem( UISTRING_TEXT( "COMMUNITY_COMBOBOX_PLAYER_INFO" ).c_str(), INFO, FONT_WHITE );			
	m_pCombo->AddItem( UISTRING_TEXT( "COMMUNITY_COMBOBOX_GUILD_ACTIVITY" ).c_str(), ACTIVE, FONT_WHITE );
	m_pCombo->AddItem( UISTRING_TEXT( "COMMUNITY_GUILD_TITLE" ).c_str(), RANK, FONT_WHITE );
	m_pCombo->SetCurSel( 0 );

	m_pRadioBtn = NiNew cRadioButton;	
	m_pRadioBtn->Init( m_pRoot, CsPoint( 19, 221 ), CsPoint( 68, 24 ), cCheckBox::IMAGE_RADIO_4, UISTRING_TEXT( "COMMUNITY_BTN_MEMBER_INFO" ).c_str(), false );
	m_pRadioBtn->AddNode( CsPoint( 87, 221 ), CsPoint( 68, 24 ), cCheckBox::IMAGE_RADIO_4, UISTRING_TEXT( "COMMUNITY_BTN_HISTORY" ).c_str() );
	m_pRadioBtn->AddNode( CsPoint( 155, 221 ), CsPoint( 68, 24 ), cCheckBox::IMAGE_RADIO_4, UISTRING_TEXT( "COMMUNITY_BTN_MANAGE" ).c_str() );			
#endif

	m_pRadioBtn->SetCheckIndex( (int)m_eRadioIndex );

	m_pMouseOnMask = AddSprite(CsPoint::ZERO, CsPoint( 367, 19 ), "Talk\\Common_Seletbar.tga");
	m_pMouseOnMask->SetAlpha( 0.5f );
	m_pMouseOnMask->SetVisible( false );

	m_pNoticeMask = AddSprite(CsPoint( 17, 120 ), CsPoint( 385, 92 ),"Talk\\Common_Seletbar.tga" );
	m_pNoticeMask->SetAlpha( 0.5f );
	
	m_strPersonInfo.SetBuddy( m_pScrollBar );

	MakeGuildInfo();
}

void cCommunity::SetTab(eTAB nIDX )
{
	m_eRadioIndex = nIDX; 
	m_pRadioBtn->SetCheckIndex( (int)nIDX );

	switch( m_eRadioIndex )
	{
	case TAB_GUILD:		
		_MakeGuildPersonInfo();		
		break;
	case TAB_HISTROY:	
		_MakeHistory();
		break;
#ifdef GUILD_RENEWAL
	case TAB_GUILDSKILL:	// 길드 스킬 텝 선택
		_MakeGuildSkill();	// 길드 스킬 UI 초기화		
		break;
#endif
	case TAB_GUILDADMIN:
		_MakeGuildAdmin();
		break;
	default:
		assert_cs( false );
		break;
	}	
}

void cCommunity::OnRButtonUp( CsPoint pos )
{
	if( IsInWindow( pos ) == false )
		return;
}

cBaseWindow::eMU_TYPE
cCommunity::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_INVITE_GUILD_NAME, 0 );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;
	muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_GUILDRANK_NAME, 0 );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;

	muReturn = cCreateName::Update_ForMouseInstance( cBaseWindow::WT_GUILDRANK_EXPLAIN, 0 );
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;


	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	switch( m_pOnOffCheck->Update_ForMouse() )
	{	
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:				
		{
			GetSystem()->SetShowOfflinePerson(m_pOnOffCheck->IsCheck());
			switch( (eTAB)m_pRadioBtn->GetCheckIndex() )
			{
			case TAB_GUILD:
				_MakeGuildPersonInfo();								
				break;			
#ifdef GUILD_RENEWAL
			case TAB_GUILDSKILL:	// 오프라인 보기 체크 시 현재 텝이 길드이면
				_MakeGuildSkill();	// 길드 스킬 UI 초기화				
				break;
#endif	// GUILD_RENEWAL
			case TAB_HISTROY:
				_MakeHistory();
				break;
			case TAB_GUILDADMIN:					
				break;
			default:
				assert_cs( false );
				break;
			}
		}		
		return muReturn;
	}

	switch( m_pCancelButton->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILDSET );
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILDRANK_VIEW );	
			Close();
		}
		return muReturn;	
	}

	switch( m_pNotice->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:	
		{
			if( m_bNotice == false )
			{
				m_bNotice = true;
				m_pNotice->SetText( UISTRING_TEXT( "COMMUNITY_BTN_DONE" ).c_str() );
				_SetEditBox( 0 );			
#ifdef IME_WINDOW_POSITION
				g_bIMEPosition = true;
#endif
			}			
			else
			{
				OnEnterKey();
#ifdef IME_WINDOW_POSITION
				g_bIMEPosition = false;
#endif
			}
		}
		return muReturn;	
	}

	int nRadioAction;
	if( (nRadioAction = m_pRadioBtn->Update_ForMouse()) != cRadioButton::INVALIDE_RADIO_CLICK )
	{
		if( nRadioAction == cRadioButton::ACTION_CLICK )
		{
			cWindow::PlaySound( cWindow::SD_At2 );		
			switch( (eTAB)m_pRadioBtn->GetCheckIndex() )
			{
			case TAB_GUILD:
				_MakeGuildPersonInfo();			
				break;
			case TAB_HISTROY:
				_MakeHistory();				
				break;
#ifdef GUILD_RENEWAL
			case TAB_GUILDSKILL:	// 길드 스킬 텝 선택
				_MakeGuildSkill();	// 길드 스킬 UI 초기화				
				break;
#endif	// GUILD_RENEWAL
			case TAB_GUILDADMIN:	
				_MakeGuildAdmin();
				break;
			default:
				assert_cs( false );
				break;
			}

			m_eRadioIndex = (eTAB)m_pRadioBtn->GetCheckIndex();
		}
		return muReturn;
	}	

	switch( (eTAB)m_pRadioBtn->GetCheckIndex() )
	{
	case TAB_GUILD:	
		{
#ifndef GUILD_RENEWAL
			// 리뉴얼 - 길드 정보의 콤보박스 제외
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
							m_pCombo->GetCurSel_Data();						
							_MakeGuildPersonInfo();
						}				
						return MUT_CLICK_FOCUS;
					}
				}
			}
#endif	// GUILD_RENEWAL
			if( _UpdateMouseGuild() == cButton::ACTION_CLICK )
				return muReturn;			
		}		
		break;
	case TAB_HISTROY:
		break;
#ifdef GUILD_RENEWAL
	case TAB_GUILDSKILL:
		if( _UpdateMouseGuildSkill() == cButton::ACTION_CLICK )
			return muReturn;
		break;
#endif	// GUILD_RENEWAL
	case TAB_GUILDADMIN:	
		if( _UpdateMouseGuildAdmin() == cButton::ACTION_CLICK )
			return muReturn;
		break;
	default:
		assert_cs( false );
		break;
	}	
#ifdef GUILD_RENEWAL
	if( m_eRadioIndex == TAB_GUILD )	// 리뉴얼 - 친구/차단 목록 UI 분리
#endif
	{
		cString* pString;		
		m_ptSelMarkPos = CsPoint( 19, 300 ) + GetRootClient();
		int nActive2;
		switch( m_strPersonInfo.Update_ForMouse( &pString, m_nMouseOnMaskIndex, nActive2, m_ptSelMarkPos, 5, CsPoint( 376, 23 ) ) )
		{
		case CURSOR::BUTTON_OUTWINDOW:
			m_bClickMask = false;
			break;
		case CURSOR::BUTTON_NONE:
		case CURSOR::LBUTTON_DOWN:
#ifdef GUILD_RENEWAL
			// 위치 툴팁 - 온라인 일 때만
			if(GetSystem()->GetPersonInfo( nActive2 )->s_bOnline)
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 0, 0 ), 150, cTooltip::GUILD_LOTATE, nActive2 );
#endif
			m_bClickMask = true;
			break;
		case CURSOR::LBUTTON_UP:
			if( m_bClickMask == true )
			{				
				if( m_eRadioIndex == TAB_GUILD )
					_GuildPersonPopup( nActive2 );
			}
			return muReturn;
		}
	}
	
	// 위에 바 드래그해야만 이동 시키도록 하겠다.
	if( CURSOR_ST.CheckClickBox( CsRect( GetRootClient(), CsSIZE( m_pRoot->GetSize().x, 67 ) ) ) != CURSOR::BUTTON_OUTWINDOW )
		_UpdateMoveWindow_ForMouse();

#ifdef GUILD_RENEWAL
	if( m_eRadioIndex == TAB_GUILDSKILL )	// 길드스킬 탭일 때
	{
		// 길드 스킬 스크롤바 업데이트
		if( m_pScrollBar_S->Update_ForMouse() != cScrollBar::ACTION_NONE )
		{
			_UpdateMouseGuildSkill();
			return muReturn;
		}
	}
	else
	{
		if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
			return muReturn;
	}
#else
	if( m_pScrollBar->Update_ForMouse() != cScrollBar::ACTION_NONE )
		return muReturn;
#endif

	return muReturn;
}

void cCommunity::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();	

	cCreateName::UpdateInstance( cBaseWindow::WT_INVITE_GUILD_NAME, 0 );	
	cCreateName::UpdateInstance( cBaseWindow::WT_GUILDRANK_NAME, 0 );
	cCreateName::UpdateInstance( cBaseWindow::WT_GUILDRANK_EXPLAIN, 0 );

	switch( (eTAB)m_pRadioBtn->GetCheckIndex() )
	{
	case TAB_GUILD:	
	case TAB_HISTROY:
		break;	
	case TAB_GUILDADMIN:	
		break;
#ifdef GUILD_RENEWAL
	case TAB_GUILDSKILL:
		{
			// OnMask 색 변경
			float fAdded;
			if( m_bPlusColor )
			{
				fAdded = m_pOnMaskColor.r + g_fDeltaTime;
				if( fAdded > 0.99f )
				{
					m_pOnMaskColor = NiColor( 0.99f, 0.99f, 0.99f );
					m_bPlusColor = false;
				}
				else
				{
					m_pOnMaskColor.r = fAdded;
					m_pOnMaskColor.g = fAdded;
					m_pOnMaskColor.b = fAdded;
				}
			}
			else
			{
				fAdded = m_pOnMaskColor.r - g_fDeltaTime;
				if( fAdded < 0.25f )
				{
					m_pOnMaskColor = NiColor( 0.25f, 0.25f, 0.25f );
					m_bPlusColor = true;
				}
				else
				{
					m_pOnMaskColor.r = fAdded;
					m_pOnMaskColor.g = fAdded;
					m_pOnMaskColor.b = fAdded;
				}
			}
		}
		break;
#endif	// GUILD_RENEWAL
	default:
		assert_cs( false );
	}	

	if( m_pEditName && m_bNotice == true )
	{		
		if( _tcscmp( m_szTempNotice, const_cast<TCHAR*>( m_pEditName->GetTextAll() ) ) != 0 ) 
		{
 			_stprintf_s( m_szTempNotice, MAX_PATH, _T( "%s" ), const_cast<TCHAR*>( m_pEditName->GetTextAll() ) );			
						
			m_strNotice.Delete( false );

			cText::sTEXTINFO ti;		
			ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );							
			g_pStringAnalysis->Cut( &m_strNotice, 372, m_szTempNotice, &ti );	
		}		

		if( m_pEditName->IsFocus() == false )
		{
			m_strNotice.ReleaseCursor();
			m_pEditName->ReleaseFocus();
			SAFE_NIDELETE( m_pEditName );
			m_bNotice = false;
		}		
	}

#ifndef GUILD_RENEWAL
	// 리뉴얼 - 길드 정보의 콤보박스 제외
	if( (eTAB)m_pRadioBtn->GetCheckIndex() == TAB_GUILD )
	{
		m_pCombo->Update(fDeltaTime);
	}
#endif

	if( GetSystem()->IsHaveMyInfo() == false || GetSystem()->GetMyInfo().s_eRank == cCommunityContent::GR_NONE )
		return;

#ifdef GUILD_RENEWAL
	//길드 레벨 툴팁
	CsRect rcRect( GetRootClient() + CsPoint( 135, 86 ) , CsSIZE( 47, 17 ) );
	if( CURSOR_ST.CheckClickBox( rcRect ) != CURSOR::BUTTON_OUTWINDOW )
	{
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 0, 0 ), 150, cTooltip::GUILD_LEVEL, 0 );
	}

	rcRect = CsRect( GetRootClient() + CsPoint( 235, 86 ) , CsSIZE( 47, 17 ) );
	if( CURSOR_ST.CheckClickBox( rcRect ) != CURSOR::BUTTON_OUTWINDOW )
	{
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos(), CsPoint( 0, 0 ), 150, cTooltip::GUILD_CONTRIBUTION, 0 );
	}
#else	// 리뉴얼 - 기여도, 명성치 툴팁 삭제
	CsRect rcRect( GetRootClient() + CsPoint( 309, 86 ) , CsSIZE( 77, 17 ) );
	if( CURSOR_ST.CheckClickBox( rcRect ) != CURSOR::BUTTON_OUTWINDOW )
	{
		std::wstring wsTooltip;
		DmCS::StringFn::Format( wsTooltip, L"%s : %d", UISTRING_TEXT( "COMMUNITY_CONTRIBUTION" ).c_str(), GetSystem()->GetMyInfo().s_nContribution );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint( 0, 0 ), wsTooltip.c_str(), CFont::FS_10 );	//2017-01-19-nova 마우스 좌표로 툴팁표시
	}

	CsRect rcRect2( GetRootClient() + CsPoint( 209, 86 ) , CsSIZE( 77, 17 ) );
	if( CURSOR_ST.CheckClickBox( rcRect2 ) != CURSOR::BUTTON_OUTWINDOW )
	{
		std::wstring wsTooltip;
		DmCS::StringFn::Format( wsTooltip, L"%s : %d", UISTRING_TEXT( "COMMUNITY_FAME_POINT" ).c_str(), GetSystem()->GetGuildInfo()->s_nExp );
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint( 0, 0 ), wsTooltip.c_str(), CFont::FS_10 );	//2017-01-19-nova 마우스 좌표로 툴팁표시
	}
#endif	// GUILD_RENEWAL
}

void cCommunity::Render()
{
	RenderScript();	
	
	if( m_pGuildMark )
		m_pGuildMark->Render();

	int nSize = static_cast<int>(m_pbButton.size());
	for( int i = 0; i < nSize; i++ )
	{
		cButton* pButton = m_pbButton.at( i );
		pButton->Render();
	}

	int nTextSize = static_cast<int>(m_pbText.size());
	for( int i = 0; i < nTextSize; i++ )
	{
		cText* pText = m_pbText.at( i );				
		pText->Render();
	}
	if (m_bNotice == true)
		m_pNoticeMask->SetVisible(true);
	else
		m_pNoticeMask->SetVisible(false);

#ifdef GUILD_RENEWAL
	if( m_eRadioIndex != TAB_GUILDSKILL && m_eRadioIndex != TAB_GUILDADMIN )
		m_strPersonInfo.Render( CsPoint( 27, 300 ) + GetRootClient(), 5 );
#else
	m_strPersonInfo.Render( CsPoint( 27, 300 ) + GetRootClient(), 5 );
#endif
		m_strNotice.Render( CsPoint( 27, 143 ) + GetRootClient(), 1 );

#ifdef IME_WINDOW_POSITION
		CsPoint StrPos = m_strNotice.GetLastPos(1);
		if( StrPos == CsPoint(0,0) )
		{
			g_nIMEWidth = GetRootClient().x + 31;
			g_nIMEHeight = GetRootClient().y + 159;
		}
		else
		{
			g_nIMEWidth = GetRootClient().x + 31 + StrPos.x;
			g_nIMEHeight = GetRootClient().y + 143 + StrPos.y;
		}
#endif

	if( m_bClickMask )
	{
		int nPosY = m_strPersonInfo.GetStringPosY( 300, m_nMouseOnMaskIndex, 5 );
		if( nPosY != cStringList::INVALIDE_FIND )
		{
			CsPoint pos = CsPoint( 15, nPosY - 2 );
			m_pMouseOnMask->SetPos(pos);
			m_pMouseOnMask->SetVisible(true);
		}	
		else
			m_pMouseOnMask->SetVisible(false);
	}
	else
	{
		m_pMouseOnMask->SetVisible(false);
	}

	m_pRadioBtn->Render();	

	switch( m_pRadioBtn->GetCheckIndex() )		
	{
	case TAB_GUILD:
#ifdef GUILD_RENEWAL
		m_pOnOffCheck->Render();
		m_pOnOffText->Render();
		m_pScrollBar->Render();
#else	// 리뉴얼 - 길드 정보의 콤보박스 제외
		m_pCombo->Render( GetRootClient() + CsPoint( 278, 248 ) );
#endif	// GUILD_RENEWAL
		m_pGuildPersonNum->SetVisible(true);
		break;
	case TAB_GUILDADMIN:
	case TAB_HISTROY:
#ifdef GUILD_RENEWAL
		m_pScrollBar->Render();
#endif
		m_pGuildPersonNum->SetVisible(false);
		break;
#ifdef GUILD_RENEWAL
	case TAB_GUILDSKILL:
		m_pScrollBar_S->Render();
		m_pGSPoint->Render();
		_RenderGuildSkill();
		m_pGuildPersonNum->SetVisible(false);
		break;
#endif
	}

	cCreateName::RenderInstance( cBaseWindow::WT_INVITE_GUILD_NAME, 0 );
	cCreateName::RenderInstance( cBaseWindow::WT_GUILDRANK_NAME, 0 );	
	cCreateName::RenderInstance( cBaseWindow::WT_GUILDRANK_EXPLAIN, 0 );
}

void cCommunity::OnMoveWindow()
{
	cBaseWindow::OnMoveWindow();	

	if( m_pRadioBtn )
		m_pRadioBtn->ResetDevice();

	if( m_pGuildMark )
		m_pGuildMark->ResetDevice();

#ifdef GUILD_RENEWAL
	if( m_pGSPoint )
		m_pGSPoint->ResetDevice();
	if( m_pOnOffText )
		m_pOnOffText->ResetDevice();

	m_strGuildSkill.ResetDevice();
	if( m_pSkillNameBG )
		m_pSkillNameBG->ResetDevice();
	if( m_pLine )
		m_pLine->ResetDevice();
	if( m_pScrollBar )
		m_pScrollBar->ResetDevice();
	if( m_pScrollBar_S )
		m_pScrollBar_S->ResetDevice();
	if( m_pIconBG )
		m_pIconBG->ResetDevice();
	for( int i = 0 ; i < IF_BASESKILL_COUNT ; i++ )
		m_pReqLvText[i]->ResetDevice();
	if( m_pOnMask )
		m_pOnMask->ResetDevice();

	if( m_pOnOffCheck )
		m_pOnOffCheck->ResetDevice();
#endif
	int nSize = static_cast<int>(m_pbButton.size());
	for( int i = 0; i < nSize; i++ )
	{
		cButton* pButton = m_pbButton.at( i );
		pButton->ResetDevice();
	}

	int nTextSize = static_cast<int>(m_pbText.size());
	for( int i = 0; i < nTextSize; i++ )
	{
		cText* pText = m_pbText.at( i );				
		pText->ResetDevice();
	}

	m_strPersonInfo.ResetDevice();
	m_strNotice.ResetDevice();

	if( m_pEditName )
		m_pEditName->ResetDevice();
}

void cCommunity::_SetEditBox( int nLineNum )
{		
	SAFE_NIDELETE( m_pEditName );	

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );

	m_pEditName = NiNew cEditBox;	
	m_pEditName->Init( m_pRoot, CsPoint::ZERO, CsPoint( 417, 23 ), &ti, false );	
	m_pEditName->SetFontLength( 120 );
	m_pEditName->EnableUnderline();		
	m_pEditName->EnablePaste();
	m_pEditName->ReleaseFocus();

	if( g_pFocusEdit != NULL )
		g_pFocusEdit->ReleaseFocus();

	ST_CHAT_PROTOCOL	CProtocol;
	CProtocol.m_Type = NS_CHAT::SET_SHOWINPUT;
	CProtocol.m_value= 0;
	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
	m_pEditName->SetFocus();
#ifdef GUILD_RENEWAL
	std::wstring wsNotice = GetSystem()->GetGuildInfo()->s_szNotice;
	if( !wsNotice.empty() )
	{
		m_pEditName->SetText( wsNotice.c_str() );
		m_pEditName->SetTextAll( wsNotice.c_str() );
	}
	_stprintf_s( m_szTempNotice, MAX_PATH, _T( "%s" ), const_cast<TCHAR*>( m_pEditName->GetTextAll() ) );
#endif
	m_pTextNotice = m_pEditName->GetText();	
}

int cCommunity::DeletePerson( int nDataIdx )
{
	int nSize = m_strPersonInfo.GetSize();

	if( nSize == 0 )
		return 0;

	for( int i = 0; i < nSize; i++ )
	{
		cString* pString = m_strPersonInfo.GetString( i );
		assert_cs( pString != NULL );
		if( pString->GetValue2() == nDataIdx )
		{
			m_strPersonInfo.RemoveIndex( i );
			return i;
		}
	}		
	return m_strPersonInfo.GetSize();
}

#ifdef GUILD_RENEWAL
void cCommunity::_SetGuildPersonCnt()
{
	// 길드 인원수 체크
	TCHAR szNum[ 32 ]={0};
	_stprintf_s( szNum, 32, _T( "%d / %d" ), m_strPersonInfo.GetSize(), GetSystem()->GetGuildInfo()->s_nMaxPersonCount );	
	m_pGuildPersonNum->SetText( szNum );
	// 스크롤바 길이 설정
	m_pScrollBar->SetRange( CsPoint( 0, m_strPersonInfo.GetSize() ) );
}
#endif
// bool cCommunity::IsOnOff()
// {
// 	return m_bOnOff;
// }
// void cCommunity::SetOnOff(bool bOnOff)
// {
// 	m_bOnOff = bOnOff;
// }
//////////////////////////////////////////////////////////////////////////
//
// 길드 정보
//
//////////////////////////////////////////////////////////////////////////

bool cCommunity::MakeGuildInfo()
{	
	// 내 길드정보가 없거나 길드명이 존재하지 않을 때

	
	if( GetSystem()->IsHaveMyInfo() == false || !(GetSystem()->GetGuildInfo()->s_szGuildName.size() > 0) )
	{
		SetNotice( UISTRING_TEXT( "COMMUNITY_NO_GUILD_AFFILIATED" ).c_str() );	// 길드 공지 설정
		return false;
	}

	_DeletePbText();

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
	ti.s_eTextAlign = DT_CENTER;
	CsPoint ptGuildName = CsPoint( 258, 47 );
	CsPoint ptGuildMasterName = CsPoint( 258, 71 );
	CsPoint ptGuildLevel = CsPoint( 157, 92 );
	CsPoint ptGuildExp = CsPoint( 257, 92 );
	CsPoint ptRank = CsPoint( 362, 92 );
	cText* pGuildName = NiNew cText;
	const cCommunityContent::sGUILD_INFO* guildInfo = GetSystem()->GetGuildInfo();
	ti.SetText( guildInfo->s_szGuildName.c_str() );
	pGuildName->Init( m_pRoot, ptGuildName, &ti, false );
	m_pbText.push_back( pGuildName );		

	cText* pGuildMasterName = NiNew cText;
	ti.SetText( guildInfo->s_szMasterName.c_str() );
	pGuildMasterName->Init( m_pRoot, ptGuildMasterName, &ti, false );
	m_pbText.push_back( pGuildMasterName );

	cText* pGuildLevel = NiNew cText;
	TCHAR szNum[ 32 ];
	_stprintf_s( szNum, 32, _T( "Lv%d" ), guildInfo->s_nGuildLevel );	
	ti.SetText( szNum );
	pGuildLevel->Init( m_pRoot, ptGuildLevel, &ti, false );
	m_pbText.push_back( pGuildLevel );	
#ifdef GUILD_RENEWAL
	cText* pGuildExp = NiNew cText;
	std::wstring wsRank = UISTRING_TEXT( "COMMUNITY_USER_RANK" );
	DmCS::StringFn::Replace( wsRank, L"#Rank#", GetSystem()->GetMyInfo().s_nUserRank );
	ti.SetText( wsRank.c_str() );
	pGuildExp->Init( m_pRoot, ptGuildExp, &ti, false );
	m_pbText.push_back( pGuildExp );	
#else
	cText* pGuildExp = NiNew cText;
	_stprintf_s( szNum, 32, _T( "Lv%d" ), ( guildInfo->s_nExp / 600000 ) + 1 );	
	ti.SetText( szNum );
	pGuildExp->Init( m_pRoot, ptGuildExp, &ti, false );
	m_pbText.push_back( pGuildExp );			

	cText* pCon = NiNew cText;
	int nRank = 15 - ( GetSystem()->GetMyInfo().s_nContribution / 800 );
	if( nRank < 0 ) 
		nRank = 1;
	std::wstring wsRank = UISTRING_TEXT( "COMMUNITY_USER_RANK" );
	DmCS::StringFn::Replace( wsRank, L"#Rank#", nRank );
	ti.SetText( wsRank.c_str() );
	pCon->Init( m_pRoot, ptRank, &ti , false );
	m_pbText.push_back( pCon );
#endif
	// 길드 공지 설정
	SetNotice( guildInfo->s_szNotice.c_str() );

	// 길드 인원수 체크
	_stprintf_s( szNum, 32, _T( "%d / %d" ), m_strPersonInfo.GetSize(), guildInfo->s_nMaxPersonCount );	
	m_pGuildPersonNum->SetText( szNum );
	// 스크롤바 길이 설정
	m_pScrollBar->SetRange( CsPoint( 0, m_strPersonInfo.GetSize() ) );

	return true;
}

void cCommunity::_MakeGuildPersonInfo()
{
	_DeletePbButton();
	m_strPersonInfo.Delete();

	//// 내 길드정보가 없거나 길드명이 존재하지 않을 때
	//if( g_pDataMng->GetGuild()->GetMyInfo() == NULL || !g_pDataMng->GetGuild()->IsGuild() )
	//	return;

#ifdef GUILD_RENEWAL
	// 레벨
	CsPoint ptLevel = CsPoint( 23, 275 );
	CsPoint ptLevelSize = CsPoint( 34, 20 );
	cButton* pLevel = NiNew cButton;	
	pLevel->Init( m_pRoot, ptLevel, ptLevelSize, cButton::IMAGE_NOR_11, UISTRING_TEXT( "COMMON_TXT_LEVEL" ).c_str(), false );
	pLevel->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.push_back( pLevel );

	// 이름
	CsPoint ptName = CsPoint( ptLevel.x + ptLevelSize.x , 275 );
	CsPoint ptNameSize = CsPoint( 110, 20 );
	cButton* pName = NiNew cButton;	
	pName->Init( m_pRoot, ptName, ptNameSize, cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMON_TXT_NAME" ).c_str(), false );
	pName->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.push_back( pName );

	// 직급
	CsPoint ptPosition = CsPoint( ptName.x + ptNameSize.x , 275 );
	CsPoint ptPositionSize = CsPoint( 110, 20 );
	cButton* pRank = NiNew cButton;	
	pRank->Init( m_pRoot, ptPosition, ptPositionSize, cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_PREVIOUS" ).c_str(), false );
	pRank->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.push_back( pRank );

	// 계급
	CsPoint ptRank = CsPoint( ptPosition.x + ptPositionSize.x , 275 );
	CsPoint ptRankSize = CsPoint( 36, 20 );
	cButton* pClass = NiNew cButton;	
	pClass->Init( m_pRoot, ptRank, ptRankSize, cButton::IMAGE_NOR_11, UISTRING_TEXT( "COMMUNITY_BTN_GRADE" ).c_str(), false );
	pClass->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.push_back( pClass );

	// 접속상태
	CsPoint ptLocation = CsPoint( ptRank.x + ptRankSize.x , 275 );
	CsPoint ptLocationSize = CsPoint( 70, 20 );
	cButton* pLoc = NiNew cButton;	
	pLoc->Init( m_pRoot, ptLocation, ptLocationSize, cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_ACCESS_STATE" ).c_str(), false );
	pLoc->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.push_back( pLoc );

	// 길드원 정보 - 보여줄 리스트 생성
	cCommunityContent::LIST_GUILD_IT	it = GetSystem()->GetMap()->begin();
	cCommunityContent::LIST_GUILD_IT	itEnd = GetSystem()->GetMap()->end();	
	int nDataIdx = 0;
	for( ; it != itEnd; it++, nDataIdx++ )
	{
		_AddGuildPerson( &(*it), nDataIdx );
	}
#else	// GUILD_RENEWAL
	CsPoint ptLevel = CsPoint( 23, 275 );
	CsPoint ptLevelSize = CsPoint( 34, 20 );

	CsPoint ptName = CsPoint( 57, 275 );
	CsPoint ptNameSize = CsPoint( 117, 20 );

	CsPoint ptPosition = CsPoint( 174, 275 );// = ptRank(랭크) = Previous(직급)
	CsPoint ptPositionSize = CsPoint( 208, 20 );// = Previous(직급)

	CsPoint ptRankSize = CsPoint( 36, 20 );

	CsPoint ptGuildPoint = CsPoint( 210, 275 );
	CsPoint ptGuildPointSize = CsPoint( 174, 20 );

	cButton* pLevel = NiNew cButton;	
	pLevel->Init( m_pRoot, ptLevel, ptLevelSize, cButton::IMAGE_NOR_11, UISTRING_TEXT( "COMMON_TXT_LEVEL" ).c_str(), false );
	pLevel->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.push_back( pLevel );

	cButton* pName = NiNew cButton;	
	pName->Init( m_pRoot, ptName, ptNameSize, cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMON_TXT_NAME" ).c_str(), false );
	pName->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.push_back( pName );

	switch( m_pCombo->GetCurSel_Data() )
	{
	case INFO:
		{
			cButton* pLoc = NiNew cButton;	
			pLoc->Init( m_pRoot, ptPosition, ptPositionSize, cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_LOCATION" ).c_str(), false );
			pLoc->SetTextColor( FONT_WHITE * 0.7f );
			m_pbButton.push_back( pLoc );
		}
		break;
	case ACTIVE:
		{
			cButton* pCon = NiNew cButton;	
			pCon->Init( m_pRoot, ptPosition, ptRankSize, cButton::IMAGE_NOR_11, UISTRING_TEXT( "COMMUNITY_BTN_RANK" ).c_str(), false );
			pCon->SetTextColor( FONT_WHITE * 0.7f );
			m_pbButton.push_back( pCon );

			cButton* pPoint = NiNew cButton;	
			pPoint->Init( m_pRoot, ptGuildPoint, ptGuildPointSize, cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_GUILD_POINT" ).c_str(), false );
			pPoint->SetTextColor( FONT_WHITE * 0.7f );
			m_pbButton.push_back( pPoint );
		}
		break;
	case RANK:
		{
			cButton* pRank = NiNew cButton;	
			pRank->Init( m_pRoot, ptPosition, ptPositionSize, cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_PREVIOUS" ).c_str(), false );
			pRank->SetTextColor( FONT_WHITE * 0.7f );
			m_pbButton.push_back( pRank );
		}
		break;
	default:
		assert_cs( false ); 
		break;
	}

	cCommunityContent::LIST_GUILD_IT	it = GetSystem()->GetMap()->begin();
	cCommunityContent::LIST_GUILD_IT	itEnd = GetSystem()->GetMap()->end();
	int nDataIdx = 0;
	for( ; it != itEnd; it++, nDataIdx++ )
	{
		AddGuildPerson( &(*it), nDataIdx );
	}
#endif	// GUILD_RENEWAL
	
	// 길드 인원수 체크
	TCHAR szNum[ 32 ]={0};
	_stprintf_s( szNum, 32, _T( "%d / %d" ), m_strPersonInfo.GetSize(), GetSystem()->GetGuildInfo()->s_nMaxPersonCount );	
	m_pGuildPersonNum->SetText( szNum );
	// 스크롤바 길이 설정
	m_pScrollBar->SetRange( CsPoint( 0, m_strPersonInfo.GetSize() ) );
	
}

#ifdef GUILD_RENEWAL
void cCommunity::_AddGuildPerson( cCommunityContent::sGUILD_PERSON_INFO* pInfo, int nRealDataIdx )
{
	int nIdx = DeletePerson( nRealDataIdx );

	if( GetSystem()->IsShowOfflinePerson() == false )
	{
		if( pInfo->s_bOnline == false )
			return;	
	}

	cString* pString = NiNew cString;
	CsPoint ptPos;
	TCHAR szTemp[ 64 ];
	int nBase;
	cText::sTEXTINFO ti;
	ti.s_eTextAlign = DT_CENTER;
	if( pInfo->s_bOnline )
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, ONLINE_COLOR );
	else
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, OFFLINE_COLOR );

	// 값 저장
	pString->SetValue1( nIdx );
	pString->SetValue2( nRealDataIdx );
	pString->SetValuePointer1( const_cast<TCHAR*>(pInfo->s_szName.c_str()) );

	// 레벨
	ti.SetText( pInfo->s_nPersonLevel );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
	pString->TailAddSizeX( 34 - ptPos.x );

	// 이름
	ti.SetText( pInfo->s_szName.c_str() );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
	pString->TailAddSizeX( 110 - ptPos.x );
	
	// 직급
	ti.SetText( GetSystem()->GetRankName( pInfo->s_eRank )->c_str() );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
	pString->TailAddSizeX( 110 - ptPos.x );

	// 계급
	_stprintf_s( szTemp, 64, _T( "%dR" ), pInfo->s_nUserRank );
	ti.SetText( szTemp );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
	pString->TailAddSizeX( 36 - ptPos.x );

	// 접속상태
	if( pInfo->s_nLastPlayTime == 0 )
		ti.SetText( UISTRING_TEXT( "COMMUNITY_LAST_PLAY_TIME" ).c_str() );
	else if( _TIME_TS > pInfo->s_nLastPlayTime )
		ti.SetText( cTooltip::Time2Str( _TIME_TS, pInfo->s_nLastPlayTime, cTooltip::TT_LASTTIME ) );
	else
		ti.SetText( UISTRING_TEXT( "COMMON_TXT_ONLINE" ).c_str() );
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );

	if( nIdx == 0 )
		m_strPersonInfo.AddHead( pString );
	else
		m_strPersonInfo.AddIndexAfter( pString, nIdx - 1 );

	if( m_pGuildPersonNum )
	{
		// 길드 인원수 체크
		TCHAR szNum[ 32 ]={0};
		_stprintf_s( szNum, 32, _T( "%d / %d" ), m_strPersonInfo.GetSize(), GetSystem()->GetGuildInfo()->s_nMaxPersonCount );
		m_pGuildPersonNum->SetText( szNum );
	}

	// 스크롤바 길이 설정
	if( m_pScrollBar )
		m_pScrollBar->SetRange( CsPoint( 0, m_strPersonInfo.GetSize() ) );
}
#else	// GUILD_RENEWAL
void cCommunity::AddGuildPerson( cCommunityContent::sGUILD_PERSON_INFO* pInfo, int nRealDataIdx )
{		
	int nIdx = DeletePerson( nRealDataIdx );

	if( GetSystem()->IsShowOfflinePerson() == false )
	{
		if( pInfo->s_bOnline == false )
			return;	
	}
	
	cString* pString = NiNew cString;
	CsPoint ptPos;
	TCHAR szTemp[ 64 ];
	int nBase;
	cText::sTEXTINFO ti;
	ti.s_eTextAlign = DT_CENTER;
	if( pInfo->s_bOnline )
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, ONLINE_COLOR );
	else
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, OFFLINE_COLOR );
	
	pString->SetValue1( nIdx );
	pString->SetValue2( nRealDataIdx );
	pString->SetValuePointer1( const_cast<TCHAR*>(pInfo->s_szName.c_str()) );

	ti.SetText( pInfo->s_nPersonLevel );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
	pString->TailAddSizeX( 30 - ptPos.x );

	ti.SetText( pInfo->s_szName.c_str() );	
	pString->AddText( &ti );	
	cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
#ifdef VERSION_TH		// 태국ver 길드 정보에서 채널 이름이 테두리를 벗어나는 현상으로 문자 그려지는 위치 수정 lks007 12.08.08
	pString->TailAddSizeX( 118 - ptPos.x );
#else
	pString->TailAddSizeX( 121 - ptPos.x );
#endif
	
	switch( m_pCombo->GetCurSel_Data() )
	{
	case INFO:
		{	
			if( pInfo->s_bOnline == true )
			{		
				_tcscpy_s( szTemp, 64, nsCsMapTable::g_pMapListMng->GetList( pInfo->s_dwMapID )->GetInfo()->s_szMapDiscript.c_str() );

			/*	if( _tcslen( szTemp ) > 7 )
				{			
					ti.SetText_Reduce( szTemp, 7 );
				}	
				else	*/	
					ti.SetText( szTemp );

				pString->AddText( &ti );	
				cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
#ifdef VERSION_TH		// 태국ver 길드 정보에서 채널 이름이 테두리를 벗어나는 현상으로 문자 그려지는 위치 수정 lks007 12.08.08
				pString->TailAddSizeX( 151 - ptPos.x );
#else
				pString->TailAddSizeX( 155 - ptPos.x );	
#endif
				std::wstring wsChannel = UISTRING_TEXT( "COMMUNITY_CHANNEL" );
				DmCS::StringFn::Replace( wsChannel, L"#Num#", pInfo->s_nChannel );
				ti.SetText( wsChannel.c_str() );
				pString->AddText( &ti );
			}		
		}
		break;
	case ACTIVE:
		{	
			int nRank = 15 - ( pInfo->s_nContribution / 800 );
			if( nRank < 0 ) 
				nRank = 1;

			_stprintf_s( szTemp, 64, _T( "%dR" ), nRank );
			ti.SetText( szTemp );	
			pString->AddText( &ti );	
			cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );
			pString->TailAddSizeX( 105 - ptPos.x );

			ti.SetText( pInfo->s_nContribution );	
			pString->AddText( &ti );	
		}
		break;
	case RANK:
		{
			ti.SetText( GetSystem()->GetRankName(pInfo->s_eRank)->c_str() );	
			pString->AddText( &ti );	
			cText::GetStringSize( &ti, ptPos, nBase, ti.GetText() );										
		}
		break;
	default:
		assert_cs( false ); 
		break;
	}		

	if( nIdx == 0 )
		m_strPersonInfo.AddHead( pString );
	else
		m_strPersonInfo.AddIndexAfter( pString, nIdx - 1 );

	// 길드 인원수 체크
	TCHAR szNum[ 32 ]={0};
	_stprintf_s( szNum, 32, _T( "%d / %d" ), m_strPersonInfo.GetSize(), GetSystem()->GetGuildInfo()->s_nMaxPersonCount );	
	m_pGuildPersonNum->SetText( szNum );
	// 스크롤바 길이 설정
	m_pScrollBar->SetRange( CsPoint( 0, m_strPersonInfo.GetSize() ) );
}
#endif	// GUILD_RENEWAL

cButton::eACTION
cCommunity::_UpdateMouseGuild()
{	
	cButton::eACTION eAction = cButton::ACTION_NONE;
	int nSize = static_cast<int>(m_pbButton.size());
	for( int i = 0; i < nSize; i++ )
	{
		cButton* pButton = m_pbButton.at( i );		
		eAction = (cButton::eACTION)pButton->Update_ForMouse();
		switch( eAction )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				m_bSort = !m_bSort;				
	
#ifdef GUILD_RENEWAL
				// i == 0: 레벨 1: 이름  2: 직급  3: 계급  4: 접속 상태
				GetSystem()->_PersonSort(i, m_bSort);
#else
				 //0:  이름 정렬				
				 //1:  레벨					
				 //2:  랭크
				 //3:  직급					
				 //4:  위치		
				GetSystem()->PersonSort( m_pCombo->GetCurSel_Data(), i, m_bSort );
#endif
				_DeletePbButton();
				_MakeGuildPersonInfo();
			}
			return eAction;
		}
	}
	return eAction;
}

void cCommunity::_GuildPersonPopup( int nDataIdx )
{
	m_nRealDataIdx = nDataIdx;
	cCommunityContent::sGUILD_PERSON_INFO* pInfo = GetSystem()->GetPersonInfo( nDataIdx );
	
	//본인이면
	if( pInfo->s_szName.compare( g_pCharMng->GetTamerUser()->GetName() ) == 0 )
	{
#ifdef GUILD_RENEWAL
		// 메모 수정 UI
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_GUILD_MEMO ) )
		{
			g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILD_MEMO );
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_GUILD_MEMO, 0, nDataIdx );
		}
		else
			g_pGameIF->GetDynamicIF( cBaseWindow::WT_GUILD_MEMO, 0, nDataIdx );
		return;
#else
		return;		
#endif
	}
	switch( GetSystem()->GetMyInfo().s_eRank)
	{	
	case cCommunityContent::GR_MASTER:
		{	
			cPopUpWindow::eGUILD_MENU pt = cPopUpWindow::GUILD_AP_MASTER;	
			cCommunityContent::eRANK pt2 = pInfo->s_eRank;

			switch( pInfo->s_eRank )
			{
			case cCommunityContent::GR_MASTER:						
				break;
			case cCommunityContent::GR_2NDMASTER:				
			case cCommunityContent::GR_DAT:				
			case cCommunityContent::GR_NORMAL:				
			case cCommunityContent::GR_NEW:	
				g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::GUILD_MENU, pt, pt2 );				
				break;;
			default:
				break;
			}
		}
		break;
	case cCommunityContent::GR_2NDMASTER:
		{
			cPopUpWindow::eGUILD_MENU pt;
			cCommunityContent::eRANK pt2 = pInfo->s_eRank;

			switch( pInfo->s_eRank )
			{
			case cCommunityContent::GR_MASTER:					
			case cCommunityContent::GR_2NDMASTER:
				pt	= cPopUpWindow::GUILD_NONE2;				
				g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::GUILD_MENU, pt, pt2 );				
				break;				
			case cCommunityContent::GR_NORMAL:
			case cCommunityContent::GR_DAT:
			case cCommunityContent::GR_NEW:
				pt	= cPopUpWindow::GUILD_BAN;				
				g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::GUILD_MENU, pt, pt2 );				
				break;
			default:
				break;
			}				
		}
		break;
	case cCommunityContent::GR_NORMAL:
	case cCommunityContent::GR_DAT:
	case cCommunityContent::GR_NEW:
		{				
			cPopUpWindow::eGUILD_MENU pt = cPopUpWindow::GUILD_NONE2;			
			cCommunityContent::eRANK pt2 = pInfo->s_eRank;

			g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::GUILD_MENU, pt, pt2 );										
		}
		break;
	default:
		assert_cs( false );
		break;
	}
#ifdef GUILD_RENEWAL
	// 저장된 메모 보여주기
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_GUILD_MEMO ) )
	{
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILD_MEMO );
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_GUILD_MEMO, 0, nDataIdx );
	}
	else
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_GUILD_MEMO, 0, nDataIdx );
#endif

}

//////////////////////////////////////////////////////////////////////////
//
// 길드 관리
//
//////////////////////////////////////////////////////////////////////////

void cCommunity::_MakeGuildAdmin()
{
	_DeletePbButton();
	m_strPersonInfo.Delete();

	// 내 길드정보가 없거나 길드명이 존재하지 않을 때
	if( GetSystem()->IsHaveMyInfo() == false || !(GetSystem()->GetGuildInfo()->s_szGuildName.size() > 0) )
		return;	

	bool bEnable;

	cButton* pInvate = NiNew cButton;
	pInvate->Init( m_pRoot, CsPoint( 25, 275 ), CsPoint( 110, 23 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_INVITE_TO_GUILD" ).c_str(), false );
	m_pbButton.push_back( pInvate );

	cButton* pBreak = NiNew cButton;
	pBreak->Init( m_pRoot, CsPoint( 140, 275 ), CsPoint( 110, 23 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_LEAVE_GUILD" ).c_str(), false );
	cCommunityContent::eRANK myRank = GetSystem()->GetMyInfo().s_eRank;
	bEnable = ( myRank != cCommunityContent::GR_MASTER ) ? true : false;
	pBreak->SetEnable( bEnable );
	m_pbButton.push_back( pBreak );

	cButton* pDestroy = NiNew cButton;
	pDestroy->Init( m_pRoot, CsPoint( 255, 275 ), CsPoint( 110, 23 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_CLOSE_GUILD" ).c_str(), false );
	bEnable = ( myRank == cCommunityContent::GR_MASTER ) ? true : false;
	pDestroy->SetEnable( bEnable );
	m_pbButton.push_back( pDestroy );

	cButton* pSet = NiNew cButton;	
	pSet->Init( m_pRoot, CsPoint( 25, 303 ), CsPoint( 110, 23 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_AUTHORITY" ).c_str(), false );	
	m_pbButton.push_back( pSet );	

	cButton* pRankView = NiNew cButton;	
	pRankView->Init( m_pRoot, CsPoint( 140, 303 ), CsPoint( 110, 23 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_TITLES_DUTY" ).c_str(), false );	
	m_pbButton.push_back( pRankView );

	cButton* pGuildInfo = NiNew cButton;	
	pGuildInfo->Init( m_pRoot, CsPoint( 255, 303 ), CsPoint( 110, 23 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_GUILD INFORMATION" ).c_str(), false );
#ifdef VERSION_USA
	pGuildInfo->SetEnable( false );
#endif
	m_pbButton.push_back( pGuildInfo );

	// 길드 인원수 체크
	TCHAR szNum[ 32 ]={0};
	_stprintf_s( szNum, 32, _T( "%d / %d" ), m_strPersonInfo.GetSize(), GetSystem()->GetGuildInfo()->s_nMaxPersonCount );
	m_pGuildPersonNum->SetText( szNum );
	// 스크롤바 길이 설정
	m_pScrollBar->SetRange( CsPoint( 0, m_strPersonInfo.GetSize() ) );
}

cButton::eACTION
cCommunity::_UpdateMouseGuildAdmin()
{
	cButton::eACTION eAction = cButton::ACTION_NONE;
	int nSize = static_cast<int>(m_pbButton.size());
	

	for( int i = 0; i < nSize; i++ )
	{
		//size로 컨트롤 되기 때문에 범위 외값이 들어갈 수 없다.
		cButton* pButton = m_pbButton.at(i);
		eAction = (cButton::eACTION)pButton->Update_ForMouse();		

		switch( eAction )
		{
		case cButton::ACTION_NONE:
			break;
		case cButton::ACTION_CLICK:
			{
				_CheckAdmin();
				cCommunityContent::eRANK eMyRank = cCommunityContent::GR_NONE;
				if(GetSystem()->IsHaveMyInfo() == true)
					eMyRank = GetSystem()->GetMyInfo().s_eRank;

				switch( i )
				{
				case ADMIN_INVITE: // 길드 초대
					{
						if(GetSystem()->GetGuildInfo()->s_nPersonCount >= GetSystem()->GetGuildInfo()->s_nMaxPersonCount )
							cPrintMsg::PrintMsg( 30130 );							
						else
							cCreateName::NewInstance( cBaseWindow::WT_INVITE_GUILD_NAME, 0, CsPoint( -1, -1 ) );
					}				
					break;
				case ADMIN_LEAVE: // 길드 탈퇴
					{	
						if( eMyRank != cCommunityContent::GR_MASTER )
							cPrintMsg::PrintMsg( 30136 );
					}
					break;
				case ADMIN_DESTROY: // 길드 폐쇠
					{
						if( eMyRank == cCommunityContent::GR_MASTER )
							cPrintMsg::PrintMsg( 30135 );
						else
							cPrintMsg::PrintMsg( 30158, (TCHAR*)GetSystem()->GetRankName( cCommunityContent::GR_MASTER )->c_str() );

					}
					break;
				case ADMIN_SET: // 권한 설정
					{
						if( eMyRank == cCommunityContent::GR_MASTER )
							g_pGameIF->GetDynamicIF( cBaseWindow::WT_GUILDSET );
						else
							cPrintMsg::PrintMsg( 30158, (TCHAR*)GetSystem()->GetRankName( cCommunityContent::GR_MASTER )->c_str() );
					}
					break;
				case ADMIN_VIEW: // 길드 직급
					{						
						g_pGameIF->GetDynamicIF( cBaseWindow::WT_GUILDRANK_VIEW );						
					}
					break;
				case ADMIN_INFO: // 길드 정보
					{
#ifndef VERSION_TH
#ifndef VERSION_TW
#ifndef VERSION_HK
						TCHAR szLink[ MAX_PATH ];
						switch(net::cluster)
						{	
							/*
						case 100 : 
							_stprintf_s( szLink, MAX_PATH, _T( "http://www.
							/comm/myguild.aspx?v=ruche&o=%d" ), 
								g_pCharMng->GetTamerUser()->GetCharOption()->GetGuildUID() );
							break;
						case 200 : 
							_stprintf_s( szLink, MAX_PATH, _T( "http://www.digimonmasters.com/comm/myguild.aspx?v=riva&o=%d" ), 
								g_pCharMng->GetTamerUser()->GetCharOption()->GetGuildUID() );
							break;
						case 300 : 
							_stprintf_s( szLink, MAX_PATH, _T( "http://www.digimonmasters.com/comm/myguild.aspx?v=riri&o=%d" ), 
								g_pCharMng->GetTamerUser()->GetCharOption()->GetGuildUID() );
							break;
						case 400 : 
							_stprintf_s( szLink, MAX_PATH, _T( "http://www.digimonmasters.com/comm/myguild.aspx?v=barba&o=%d" ), 
								g_pCharMng->GetTamerUser()->GetCharOption()->GetGuildUID() );
							break;
							*/
						}		
						
						//ShellExecute(NULL, _T( "open" ), _T( "iexplore" ), szLink, NULL, SW_SHOW );
#endif
#endif
#endif
					}
					break;
				}
			}
			break;	
		}
	}
	return eAction;
}

void cCommunity::_CheckAdmin()
{	
	if( g_pCreateName )
	{
		// 이름 짓는 윈도우 닫아준다.
		cCreateName::DeleteInstance();	
	}

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_GUILDSET ) == true )	
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILDSET );

	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_GUILDRANK_VIEW ) == true )	
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_GUILDRANK_VIEW );
}

void cCommunity::_MakeHistory()
{
	if( m_Time.IsEnable() == true && (GetSystem()->GetGuildInfo()->s_szGuildName.size() > 0) )
	{
		if( net::game )
			net::game->SendGuildHistory();
	}
	_DeletePbButton();		

	cButton* pMemo = NiNew cButton;	
	pMemo->Init( m_pRoot, CsPoint( 23, 275 ), CsPoint( 359, 19 ), cButton::IMAGE_NOR_12, UISTRING_TEXT( "COMMUNITY_BTN_HISTORY_MEMO" ).c_str(), false );
	pMemo->SetTextColor( FONT_WHITE * 0.7f );
	m_pbButton.push_back(pMemo);

	RefreshHistory();

	// 스크롤바 길이 설정
	m_pScrollBar->SetRange( CsPoint( 0, m_strPersonInfo.GetSize()) );
}

void cCommunity::RefreshHistory()
{	
	m_strPersonInfo.Delete();

	TCHAR szTemp[ 128 ]={0};
	TCHAR szTime[ 128 ]={0};
	
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
	//ti.s_eTextAlign = DT_CENTER;
	if( !(GetSystem()->GetHistoryInfoSize() > 0) )
		return;

	int nHistoryInfoSize = GetSystem()->GetHistoryInfoSize();
	for( int i = 0; i < 30 && i < nHistoryInfoSize; i++ )
	{
		cCommunityContent::sHISTORY_INFO pInfo = GetSystem()->GetHistoryInfo( i );

		uint nTime = pInfo.s_nTime;
		if( nTime < 60 )
		{
			_stprintf_s( szTime, 128, UISTRING_TEXT( "COMMUNITY_HISTORY_LESS_THAN_A_MIN" ).c_str() );
		}
		else 
		{
			if( ( nTime / 60 ) < 60 )
			{
				std::wstring wsTime = UISTRING_TEXT( "COMMUNITY_HISTORY_MIN_PASSED" );
				DmCS::StringFn::Replace( wsTime, L"#Min#", nTime / 60 );
				_stprintf_s( szTime, 128, wsTime.c_str() );
			}
			else
			{
				if( ( nTime / 60 / 60 ) < 24 )
				{
					std::wstring wsTime = UISTRING_TEXT( "COMMUNITY_HISTORY_HOUR_PASSED" );
					DmCS::StringFn::Replace( wsTime, L"#Hour#", nTime / 60 / 60 );
					_stprintf_s( szTime, 128, wsTime.c_str() );
				}
				else
				{
					std::wstring wsTime = UISTRING_TEXT( "COMMUNITY_HISTORY_DAY_PASSED" );
					DmCS::StringFn::Replace( wsTime, L"#Day#", nTime / 60 / 60 / 24 );
					_stprintf_s( szTime, 128, wsTime.c_str() );
				}
			}
		}

		ti.s_Color = FONT_WHITE;
		switch( pInfo.s_nType )
		{
		case 1: // 마스터 임명
			{
				std::wstring wsRank = UISTRING_TEXT( "COMMUNITY_NEW_GUILD_MASTER" );
				DmCS::StringFn::Replace( wsRank, L"#Name#", pInfo.s_szMemberName.c_str() );
				std::wstring * rankName = GetSystem()->GetRankName( cCommunityContent::GR_MASTER );
				if( NULL == rankName )
					DmCS::StringFn::Replace( wsRank, L"#Rank#", L"" );
				else
					DmCS::StringFn::Replace( wsRank, L"#Rank#", rankName->c_str() );

				_stprintf_s( szTemp, 128, wsRank.c_str() );
				_tcscat( szTemp, szTime );
				g_pStringAnalysis->Cut_Parcing( &m_strPersonInfo, 350, szTemp, &ti );
			}
			break;

		case 2: // 부마 임명
		case 3: // 닷트회원 임명
		case 4: // 일반회원 임명 
		case 5: // 준회원 임명
			{
				std::wstring wsRank = UISTRING_TEXT( "COMMUNITY_CHANGE_MEMBER_RANK" );
				DmCS::StringFn::Replace( wsRank, L"#Master#", pInfo.s_szMasterName.c_str() );
				DmCS::StringFn::Replace( wsRank, L"#Member#", pInfo.s_szMemberName.c_str() );
				std::wstring * rankName = GetSystem()->GetRankName( (cCommunityContent::eRANK)pInfo.s_nType );
				if( rankName )
					DmCS::StringFn::Replace( wsRank, L"#Rank#", rankName->c_str() );
				else
					DmCS::StringFn::Replace( wsRank, L"#Rank#", L"" );
				_stprintf_s( szTemp, 128, wsRank.c_str() );
				_tcscat( szTemp, szTime );
				g_pStringAnalysis->Cut_Parcing( &m_strPersonInfo, 350, szTemp, &ti );
			}
			break;			

		case 100: // 길드 창설
			{
				std::wstring wsMsg = UISTRING_TEXT( "COMMUNITY_CREATE_GUILD" );
				DmCS::StringFn::Replace( wsMsg, L"#Master#", pInfo.s_szMasterName.c_str() );
				_stprintf_s( szTemp, 128, wsMsg.c_str() );
				_tcscat( szTemp, szTime );
				g_pStringAnalysis->Cut_Parcing( &m_strPersonInfo, 350, szTemp, &ti );				
			}
			break;

		case 101: // 길드 가입
			{
				std::wstring wsMsg = UISTRING_TEXT( "COMMUNITY_JOINE_GUILD" );
				DmCS::StringFn::Replace( wsMsg, L"#Member#", pInfo.s_szMemberName.c_str() );
				_stprintf_s( szTemp, 128, wsMsg.c_str() );
				_tcscat( szTemp, szTime );
				g_pStringAnalysis->Cut_Parcing( &m_strPersonInfo, 350, szTemp, &ti );
			}
			break;

		case 102: // 길드 탈퇴
			{
				std::wstring wsMsg = UISTRING_TEXT( "COMMUNITY_RESIGNED_GUILD" );
				DmCS::StringFn::Replace( wsMsg, L"#Member#", pInfo.s_szMemberName.c_str() );
				_stprintf_s( szTemp, 128, wsMsg.c_str() );
				_tcscat( szTemp, szTime );
				g_pStringAnalysis->Cut_Parcing( &m_strPersonInfo, 350, szTemp, &ti );
			}
			break;

		case 103: // 길드 강퇴
			{
				std::wstring wsMsg = UISTRING_TEXT( "COMMUNITY_EXPELLED_GUILD" );
				DmCS::StringFn::Replace( wsMsg, L"#Master#", pInfo.s_szMasterName.c_str() );
				DmCS::StringFn::Replace( wsMsg, L"#Member#", pInfo.s_szMemberName.c_str() );
				_stprintf_s( szTemp, 128, wsMsg.c_str() );
				_tcscat( szTemp, szTime );
				g_pStringAnalysis->Cut_Parcing( &m_strPersonInfo, 350, szTemp, &ti );
			}
			break;

		case 104: // 길드 폐쇄
			{
				std::wstring wsMsg = UISTRING_TEXT( "COMMUNITY_CLOSED_GUILD" );
				DmCS::StringFn::Replace( wsMsg, L"#Master#", pInfo.s_szMasterName.c_str() );
				_stprintf_s( szTemp, 128, wsMsg.c_str() );
				_tcscat( szTemp, szTime );
				g_pStringAnalysis->Cut_Parcing( &m_strPersonInfo, 350, szTemp, &ti );
			}
			break;

		}		
	}		
}

#ifdef GUILD_RENEWAL
// 길드 스킬
void cCommunity::_MakeGuildSkill()
{
	_DeletePbButton();
	m_strPersonInfo.Delete();
	m_strGuildSkill.Delete();
 	// 길드 스킬 포인트
	std::wstring wsMsg;
	DmCS::StringFn::Format( wsMsg, L"%s : %d", UISTRING_TEXT( "COMMUNITY_GSP" ).c_str(), GetSystem()->GetGuildInfo()->s_nGuildSkillPoint );
 	m_pGSPoint->SetText( wsMsg.c_str() );
	//=================================================================================================================
	//		Base 스킬
	//=================================================================================================================	
	// 스킬 정보
	CsGuild_Skill* pGuildSkill = nsCsFileTable::g_pBaseMng->GetGuildSkill(0);
	std::list<CsGuild_Skill::sINFO*>::iterator it_Base = pGuildSkill->GetList()->begin();
	// 스킬 아이콘 위치
	CsPoint ptBaseIconPos = CsPoint( 50, 282 );
	// 아이콘/요구레벨 스트링 셋팅
	for( int i = 0 ; i < IF_BASESKILL_COUNT ; i++ )
	{
		m_BaseSkillIcon[i].Init( CsRect( ptBaseIconPos, CsSIZE( 32, 32 ) ) );

		m_pBaseSkill[i] = nsCsFileTable::g_pSkillMng->GetSkill( (*it_Base)->s_nSkillCode );
		
		std::wstring wsLevel;
		DmCS::StringFn::Format( wsLevel, L"%d%s", (*it_Base)->s_nReqLevel, UISTRING_TEXT( "COMMON_TXT_SHORT_LEVEL" ).c_str() );
		m_pReqLvText[i]->SetText( wsLevel.c_str() );

		ptBaseIconPos.x += 67;

		it_Base++;
	}

	//=================================================================================================================
	//		Active 스킬
	//=================================================================================================================	
	cText::sTEXTINFO ti;
	ti.Init();

	// 스킬 이름 스트링리스트에 담기
	int nGroupCnt = static_cast<int>(nsCsFileTable::g_pBaseMng->m_mapGuildSkill.size());
	for( int i = 1 ; i < nGroupCnt ; i++ )	// Active스킬은 그룹 1부터 ( 0은 Base스킬)
	{
		cString* pString = NiNew cString;

		// 스킬 정보
		pGuildSkill = nsCsFileTable::g_pBaseMng->GetGuildSkill(i);
		std::list<CsGuild_Skill::sINFO*>::iterator it_Act = pGuildSkill->GetList()->begin();
		std::list<CsGuild_Skill::sINFO*>::iterator itEnd_Act = pGuildSkill->GetList()->end();

		// 스킬이름 - 같은그룹이면 이름 다 같음
		ti.SetText( (*it_Act)->s_szSkillName );
		ti.s_eFontSize = CFont::FS_10;
		pString->AddText( &ti );
		m_strGuildSkill.AddTail( pString );
	}
	//=================================================================================================================
	//		Etc
	//=================================================================================================================	
	m_pOnMaskColor = NiColor( 0.3f, 0.3f, 0.3f );
	m_bPlusColor = true;

	// 작은 스크롤바 길이 설정
	m_pScrollBar_S->SetRange( CsPoint( 0, m_strGuildSkill.GetSize() ) );
}

cButton::eACTION
cCommunity::_UpdateMouseGuildSkill()
{
	bool bCheck = false;
	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	int nCurScroll = m_pScrollBar_S->GetCurPosIndex();

	//=================================================================================================================
	//		Base 스킬
	//=================================================================================================================	
	for( int i = 0 ; i < IF_BASESKILL_COUNT ; i++ )
	{
		if( ( bCheck == false )&&( m_BaseSkillIcon[ i ].MouseLBtnClick( ptMouseLocalPos ) == true ) )
		{
			bCheck = true;

			// 툴팁 ( 메인ID = 스킬타입(0:Base, 1:Active), 서브1 = 스크롤인덱스, 서브2 = 행, 서브3 = 열 )
			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos()+CsPoint( 14, 0 ), CsPoint( 21, 16 ), 200, cTooltip::GUILD_SKILL, 0, 0, 0, i );

			// 클릭동작
			if( ( eCursor == CURSOR::LBUTTON_UP || eCursor == CURSOR::RBUTTON_UP ) )
			{
				if( _CheckUseState() == false )
					return cButton::ACTION_NONE;
				if( GetSystem()->GetGuildInfo()->s_szGuildName.size() > 0 )
					_UseBaseSkill(i);
				else
					cPrintMsg::PrintMsg( 30214 );	// @<tc:900>길드에 가입하지 않아 길드스킬 효과를 받을 수 없습니다.@</tc>

				return cButton::ACTION_CLICK;
			}
		}
	}

	//=================================================================================================================
	//		Active 스킬
	//=================================================================================================================	
	for( int y = 0 ; y < IF_ACTIVESKILL_ROW ; y++ )
	{

		for( int x = 0 ; x < IF_ACTIVESKILL_COL ; x++ )
		{
			if( ( bCheck == false )&&( m_ActSkillIcon[ (y*IF_ACTIVESKILL_COL) + x ].MouseLBtnClick( ptMouseLocalPos ) == true ) )
			{
				bCheck = true;

				// 툴팁 ( 메인ID = 스킬타입(0:Base, 1:Active), 서브1 = 스크롤인덱스, 서브2 = 행, 서브3 = 열 )
				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( CURSOR_ST.GetPos()+CsPoint( 14, 0 ), CsPoint( 21, 16 ), 200, cTooltip::GUILD_SKILL, 1, nCurScroll, y+1, x );

				// 클릭동작
				if( ( eCursor == CURSOR::LBUTTON_UP || eCursor == CURSOR::RBUTTON_UP ) )
				{
					if( _CheckUseState() == false )
						return cButton::ACTION_NONE;
					if( GetSystem()->GetGuildInfo()->s_szGuildName.size() > 0 )
						_UseActSkill(y, x);
					else
						cPrintMsg::PrintMsg( 30214 );	// @<tc:900>길드에 가입하지 않아 길드스킬 효과를 받을 수 없습니다.@</tc>

					return cButton::ACTION_CLICK;
				}
			}
		}
	}
	return cButton::ACTION_NONE;
}

void cCommunity::_RenderGuildSkill()
{
	// Icon BG 슬롯 Delta값
	CsPoint ptIcoBGDelta = CsPoint( 6, 6 );
	//=================================================================================================================
	//		Base 스킬
	//=================================================================================================================	
	CsPoint ptBaseSkill;	// Base스킬 아이콘 위치
	bool bMask = false;		// 아이콘 마우스 마스크 플래그
	// 길드 레벨
	int nGuildLv = GetSystem()->GetGuildInfo()->s_nGuildLevel;
	// 길드 스킬 포인트
	u4 nGSP = GetSystem()->GetGuildInfo()->s_nGuildSkillPoint;

	// 현재 스크롤바 인덱스
	int nCurScroll = m_pScrollBar_S->GetCurPosIndex();

	std::list< CsGuild_Skill::sINFO* >::iterator it = nsCsFileTable::g_pBaseMng->GetGuildSkill(0)->GetList()->begin();
	for( int i = 0 ; i < IF_BASESKILL_COUNT ; i++, it++ )
	{
		// 아이콘 BG
		m_pIconBG->Render( m_BaseSkillIcon[ i ].GetPos() + GetRootClient() - ptIcoBGDelta );

		// 아이콘
		ptBaseSkill = m_BaseSkillIcon[ i ].GetPos() + GetRootClient();
		g_pIconMng->RenderSkill( m_pBaseSkill[i], ptBaseSkill );

		// 음영( 레벨 부족 )
		if( nGuildLv < (*it)->s_nReqLevel )
			g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, ptBaseSkill, CsPoint(32,32) );

		// 마우스 마스크 랜더링
 		if( m_BaseSkillIcon[ i ].s_eState != sIFIcon::NONE && !bMask )
 		{
 			m_BaseSkillIcon[ i ].RenderMask( ptBaseSkill, CsPoint(32,32) );
 			bMask = true;
 		}

		// 쿨타임 마스크
		cCommunityContent::sGUILDSKILL_INFO const * pInfo = GetSystem()->GetUsingSkill( m_pBaseSkill[i]->GetInfo()->s_dwID );
		if( pInfo && pInfo->_IsCoolTime() )	// 쿨타임인가
			g_pIconMng->RenderTime( ptBaseSkill, 16.0f, pInfo->s_nEndTime - _TIME_TS, m_pBaseSkill[i]->GetInfo()->s_fCooldownTime );

		// 요구레벨
		m_pReqLvText[i]->Render( m_BaseSkillIcon[ i ].GetPos() + CsPoint( 16, 38 ), DT_CENTER );
	}

	// 구분선
	m_pLine->Render( CsPoint( 24, 334 ) + GetRootClient() );

	//=================================================================================================================
	//		Active 스킬
	//=================================================================================================================	
	// 스킬 이름BG
	for( int i = 0 ; i < 3 ; i ++ )	// 3개만 보여줄거야
		m_pSkillNameBG->Render( CsPoint( 24, 351 ) + GetRootClient() + CsPoint( 0, i*47 ) );
	// 스킬 이름
	m_strGuildSkill.Render( CsPoint( 28, 359 ) + GetRootClient(), 34 );

	// 스킬 아이콘
	for( int y = 0 ; y < IF_ACTIVESKILL_ROW ; y++ )
	{
		// 길드 스킬 그룹
		CsGuild_Skill* pGuildSkill = nsCsFileTable::g_pBaseMng->GetGuildSkill( (y+1) + nCurScroll );
		// 액티브 스킬 리스트
		std::list<CsGuild_Skill::sINFO*>::iterator it_Act = pGuildSkill->GetList()->begin();

		// 스킬 아이콘
		for( int x = 0 ; x < IF_ACTIVESKILL_COL ; x++ )
		{
			// 아이콘 위치
			CsPoint ptIcon = m_ActSkillIcon[ (y*IF_ACTIVESKILL_COL) + x ].GetPos() + GetRootClient();
			// 아이콘 BG
			m_pIconBG->Render( ptIcon - ptIcoBGDelta );			
			// 스킬 아이콘
			CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( (*it_Act)->s_nSkillCode );
			g_pIconMng->RenderSkill( pSkill, ptIcon, x+1 );

			// 마우스 마스크 
			if( m_ActSkillIcon[ (y*IF_ACTIVESKILL_COL) + x ].s_eState != sIFIcon::NONE && !bMask )
			{			
				m_ActSkillIcon[ (y*IF_ACTIVESKILL_COL) + x ].RenderMask( ptIcon, CsPoint(32,32) );
				bMask = true;
			}

			// 사용 불가능한 스킬은 음영
			if( nGuildLv < (*it_Act)->s_nReqLevel	// 필요 길드 레벨이 안되거나 
				|| nGSP < (*it_Act)->s_nConsGsp )	// 필요 GSP가 없으면
			{
				g_pIconMng->RenderMask( ICONMASK::IF_ICON_RED, ptIcon, CsPoint(32,32) );
			}

			// 사용중인 스킬은 On 마스크
			
			if( GetSystem()->IsHaveUsingSkill( (*it_Act)->s_nSkillCode ) != NULL )
			{
				m_pOnMask->SetColor( m_pOnMaskColor );
				m_pOnMask->Render( ptIcon );
			}
			it_Act++;
		}
	}
}

bool cCommunity::_CheckUseState()
{
	// 전투 중엔 사용 안되도록 체크
	if( g_pCharMng->GetDigimonUser( 0 )->IsBattle() )
	{
		cPrintMsg::PrintMsg( 16002 );	// @<tc:900>전투 중에는 사용할 수 없습니다@</tc>
		return false;
	}
	// 사용 불가 상태 체크. ( Npc이용, 전투중, 거래중, 진화중, 파트너몬 교체중 )
	if( g_pGameIF->IsOpenNpcWindow() || g_pDataMng->GetServerSync()->IsChageDigimon() || cMessageBox::IsMessageBox() )
	{
		cPrintMsg::PrintMsg( 30024 );	// @<tc:900>선행 행동을 종료하셔야 진행이 가능합니다@</tc>
		return false;
	}
	return true;
}

void cCommunity::_UseBaseSkill( int nIdx )
{
	// 길마/부길마만 스킬 사용할 수 있음
	cCommunityContent::eRANK eMyRank = GetSystem()->GetMyInfo().s_eRank;
	if( eMyRank != cCommunityContent::GR_MASTER && eMyRank != cCommunityContent::GR_2NDMASTER )
	{ 
		cPrintMsg::PrintMsg( 30158, (TCHAR*)GetSystem()->GetRankName( cCommunityContent::GR_2NDMASTER )->c_str() );
		return;
	}
	// Base 스킬 정보
	CsGuild_Skill* pGuildSkill = nsCsFileTable::g_pBaseMng->GetGuildSkill(0);

	// 사용한 스킬 정보 인덱스로 찾기
	std::list<CsGuild_Skill::sINFO*>::iterator it = pGuildSkill->GetList()->begin();
	int nCnt = 0;
	while( nIdx > nCnt)
	{
		it++;
		nCnt++;
	}

	
	if( GetSystem()->IsHaveUsingSkill((*it)->s_nSkillCode) == false )			// 사용중이 아닌 스킬 (길드 레벨이 안되는 경우)
	{
		int nGuildLevel = GetSystem()->GetGuildInfo()->s_nGuildLevel;
		if( nGuildLevel < (*it)->s_nReqLevel )	// 스킬 사용 레벨이 안됨
		{
			cPrintMsg::PrintMsg( 30215 );	// @<tc:900>길드 레벨이 부족@</tc>하여\n스킬을 사용할 수 없습니다.%
		}
		return;
	}

	u4 nGSP = GetSystem()->GetGuildInfo()->s_nGuildSkillPoint;
	if( nGSP < (*it)->s_nConsGsp )
	{
		cPrintMsg::PrintMsg( 30217, (*it)->s_szSkillName );	// GSP가 부족하여 [str]스킬을 활성화 할 수 없습니다.
		return;
	}
	// 현재 사용 중인 스킬에서 정보 가져옴(레벨이 아니면 여기에 없음) - 쿨타임 들어있음
	cCommunityContent::sGUILDSKILL_INFO const* pInfo = GetSystem()->GetUsingSkill( (*it)->s_nSkillCode );
	if( pInfo && pInfo->_IsCoolTime() )	// 쿨타임 중
	{
		cPrintMsg::PrintMsg( 13006 );	// @<tc:900>아직 사용할 수 없습니다@</tc>
		return;
	}

	// 스킬 효과에 따른 동작
	CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( (*it)->s_nSkillCode );
	if( pSkill )
	{
		for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
		{
			if( pSkill->GetInfo()->s_Apply[i].s_nID == 0 )
				break;
			switch( pSkill->GetInfo()->s_Apply[ i ].s_nA )
			{
			case GUILD_SHOUT:	// 확성기
				g_pGameIF->GetDynamicIF(cBaseWindow::WT_SHOUT_CASH, 0, -1 );	// 길드스킬이라면 nValue = -1
				break;
			case GUILD_SUMMON:	// 길드원 소환
				{
					// 테이머스킬 테이블이 있어야 됨. 지역 체크를 테이머스킬 테이블에서 검사.. ㅠㅠ
					USHORT nMapID = (USHORT)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
					if( nsCsFileTable::g_pSkillMng->GetArea_Check( pSkill->GetInfo()->s_Apply[ i ].s_nB ) == NULL )
					{
						ST_CHAT_PROTOCOL	CProtocol;
						CProtocol.m_Type = NS_CHAT::DEBUG_TEXT;
						//CProtocol.m_wStr = GetVAString( _T("TamerSkill.xls - Area Check시트의 인덱스 못찾음 : Idx = %d"), pSkill->GetInfo()->s_Apply[ i ].s_nB );
						DmCS::StringFn::Format(CProtocol.m_wStr,  _T("TamerSkill.xls - Area Check시트의 인덱스 못찾음 : Idx = %d"), pSkill->GetInfo()->s_Apply[ i ].s_nB);
						GAME_EVENT_STPTR->OnEvent( EVENT_CODE::EVENT_CHAT_PROCESS, &CProtocol );
						return;
					}
					if( nsCsFileTable::g_pSkillMng->FindArea_Check( pSkill->GetInfo()->s_Apply[ i ].s_nB, nMapID ) )
					{
						cPrintMsg::PrintMsg( 30555, (*it)->s_szSkillName );	// @<tc:990>[str]@</tc> 스킬을 @<tc:900>사용할 수 없는 지역입니다.@</tc>%
						return;
					}

					cPrintMsg::PrintMsg( 30216, &(*it)->s_szSkillName );	// [str] 스킬을 사용하시겠습니까
					cMessageBox::GetMessageBox( 30216 )->SetValue1( (*it)->s_nSkillCode );
				}
				break;
				// 밑으로는 사용하는 스킬이 아닌것들(Passive)
			case GUILD_MEMBERUP:	// 길드인원확장
			case GUILD_NAMECOLOR:	// 길드원 이름색 변경
				break;
			}
		}
	}
}

void cCommunity::_UseActSkill( int nY, int nX )
{
	// 길마/부길마만 스킬 사용할 수 있음
	cCommunityContent::eRANK eMyRank = GetSystem()->GetMyInfo().s_eRank;
	if( eMyRank != cCommunityContent::GR_MASTER && eMyRank != cCommunityContent::GR_2NDMASTER )
	{ 
		cPrintMsg::PrintMsg( 30158, (TCHAR*)GetSystem()->GetRankName( cCommunityContent::GR_2NDMASTER )->c_str() );
		return;
	}

	// 현재 스크롤 인덱스
	int nCurScroll = m_pScrollBar_S->GetCurPosIndex();
	// 스크롤, y인덱스 로 사용하려는 스킬 그룹 정보 가져오기
	CsGuild_Skill* pGuildSkill = nsCsFileTable::g_pBaseMng->GetGuildSkill( 1 + nY + nCurScroll );	// Active 스킬은 그룹 1부터.. 0은 Base 스킬임

	// 해당 그룹에서 x인덱스 로 사용하려는 스킬 정보 가져오기
	std::list<CsGuild_Skill::sINFO*>::iterator it = pGuildSkill->GetList()->begin();
	std::list<CsGuild_Skill::sINFO*>::iterator itEnd = pGuildSkill->GetList()->end();
	int nCnt = 0;
	while( nX > nCnt)
	{
		it++;
		nCnt++;
	}

	// 사용 하려는 스킬보다 동급/상위 스킬이 적용 중인지 확인
	int nResult = GetSystem()->CheckActSkill( (*it)->s_nSkillCode );//( 0 : 같은그룹 아님, 1: 같은 그룹의 상위 존재, 스킬코드: 같은 그룹이지만 하위 스킬 존재 )

	if( nResult == 1 )
	{
		cPrintMsg::PrintMsg( 30208 );		// 더 좋은 효과가 이미 적용중입니다. 사용이 불가능합니다
		return;
	}

	int nGuildLevel = GetSystem()->GetGuildInfo()->s_nGuildLevel;
	if( nGuildLevel < (*it)->s_nReqLevel )	// 스킬 사용 레벨이 안됨
	{
		cPrintMsg::PrintMsg( 30215 );		// @<tc:900>길드 레벨이 부족@</tc>하여\n스킬을 사용할 수 없습니다.%
		return;
	}

	// 소모 GSP 체크
	u4 nGSP = GetSystem()->GetGuildInfo()->s_nGuildSkillPoint;
	if( nGSP < (*it)->s_nConsGsp )
	{
		cPrintMsg::PrintMsg( 30217, (*it)->s_szSkillName );		// GSP가 부족하여 [str]스킬을 활성화 할 수 없습니다.
		return;
	}

	int nStep = (*it)->s_nSkillLevel;

	CsSkill* pSkill = nsCsFileTable::g_pSkillMng->GetSkill( (*it)->s_nSkillCode );

	for( int i=0; i<SKILL_APPLY_MAX_COUNT; ++i )
	{
		if( pSkill->GetInfo()->s_Apply[i].s_nID == 0 )
			break;
		switch( pSkill->GetInfo()->s_Apply[ i ].s_nA )
		{
		case GUILD_NATUREUP:		// 속성 경험치 추가
		case GUILD_SCANREDUCE:		// 스캔 비용 감소
		case GUILD_SCANBITUP:		// 회수 시 Bit 추가 획득
		case GUILD_HATCHUP:			// 부화 확률 증가
		case GUILD_CROPBITUP:		// 사냥 시 Bit 추가 획득
		case GUILD_MOVEUP:			// 이동 속도 증가
		case GUILD_INCHAPAYREDUCE:	// 강화 비용 감소
		case GUILD_INCHARATEUP:		// 강화 확률 증가
			{
				if( nResult != 0 )
				{
					std::list<CsGuild_Skill::sINFO*>::iterator itTmp = pGuildSkill->GetList()->begin();
					std::list<CsGuild_Skill::sINFO*>::iterator itTmpEnd = pGuildSkill->GetList()->end();
					for( ; itTmp != itTmpEnd ; itTmp++ )
					{
						if( (*itTmp)->s_nSkillCode == nResult )
						{
							// 하위스킬 삭제 확인 토크
							cPrintMsg::PrintMsg( 30207, &(*itTmp)->s_szSkillName );	// 기존에 적용중이던 @<tc:990>[str]@</tc> 효과를 삭제하고 더 좋은 효과를 적용하시겠습니까?
							cMessageBox::GetMessageBox( 30207 )->SetValue1( (*it)->s_nSkillCode );
							break;
						}
					}		
				}
				else
				{
					cPrintMsg::PrintMsg( 30206, &(*it)->s_szSkillName );	//GSP를 소모하여 [str]스킬을 활성화 하시겠습니까?
					cMessageBox::GetMessageBox( 30206 )->SetValue1( (*it)->s_nSkillCode );
				}			
			}
			return;
		default:
			break;
		}
	}
}
#endif