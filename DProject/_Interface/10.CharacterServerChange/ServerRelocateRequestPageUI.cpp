
#include "stdafx.h"
#include "ServerRelocateRequestPageUI.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

CRequestPageUI::CRequestPageUI() : 
m_pBtnClose(NULL), 
m_pBackBtn(NULL), 
m_pRequestBtn(NULL), 
m_pCheckNameBtn(NULL),
m_pSelectServer(NULL),
m_pTamerSlotCheck(NULL),
m_pLimitMoneyText(NULL),
m_pLimitMoney(NULL),
m_pCheckNameMsg(NULL),
m_pCheckBox(NULL),
m_pEditBox(NULL)
{
}

CRequestPageUI::~CRequestPageUI()
{ 
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	Destroy(); 
}

bool CRequestPageUI::Create()
{
	InitScript( "ServerRelocate\\server_popup.tga", CsPoint( (g_nScreenWidth - 1024) / 2, (g_nScreenHeight - 768) / 2 ), CsPoint( 1024, 768 ), false );
	AddTitle( UISTRING_TEXT("TAMER_SERVER_RELOCATE_TITLE").c_str(), CsPoint(0, 13 ), CFont::FS_16, NiColor::WHITE );

	m_pBtnClose = AddButton( CsPoint( 1024 - 30 - 10, 12 ), CsPoint( 30, 28 ), CsPoint( 0, 28 ), "Event\\event_close.tga" );
	if( m_pBtnClose )
		m_pBtnClose->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CRequestPageUI::_CloseWindow );

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_SELECT_SERVER_TITLE").c_str());
		AddText( &ti, CsPoint(60, 88));
	}
	AddSprite( CsPoint( 54, 113 ), CsPoint( 912, 36 ), "ServerRelocate\\server_sub_02.tga");
	{	// 이전 서버
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_SELECT_SERVER").c_str());
		AddText( &ti, CsPoint(149, 124));
	}

	{	// 현재 서버
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_RIGHT;
		std::wstring serverName = GLOBALDATA_ST.GetCurrentServerName();
		std::wstring msg = UISTRING_VA( "TAMER_SERVER_RELOCATE_REGISTPAGE_CURRENT_SERVERNAME", serverName.c_str() );
		ti.SetText(msg.c_str());
		AddText( &ti, CsPoint(946, 124));
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14 );
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHECK_TITLE").c_str() );
		AddText( &ti, CsPoint(60, 198));
	}	

	//////////////////////////////////////////////////////////////////////////
	// 길드
	AddSprite( CsPoint( 54, 223 ), CsPoint( 912, 36 ), "ServerRelocate\\server_sub_02.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CONFIRM_GUILD").c_str() );
		AddText( &ti, CsPoint(149, 234));	
	}
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CONFIRM_GUILD_MSG").c_str() );
		AddText( &ti, CsPoint(264, 234));	
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_RIGHT;
		if( GetSystem()->IHaveGuild() )
		{
			ti.s_Color = FONT_RED;
			ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHANGE_DISABLE").c_str() );
		}
		else
		{
			ti.s_Color = FONT_GREEN;
			ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHANGE_ENABLE").c_str() );
		}
		AddText( &ti, CsPoint(946, 234));	
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 위탁 상점
	AddSprite( CsPoint( 54, 259 ), CsPoint( 912, 36 ), "ServerRelocate\\server_sub_02.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CONFIRM_LIMITSTORESHOP").c_str() );
		AddText( &ti, CsPoint(149, 270));	
	}
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CONFIRM_LIMITSTORESHOP_MSG").c_str() );
		AddText( &ti, CsPoint(264, 270));	
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_RIGHT;
		if( GetSystem()->IHaveUnionStore() )
		{
			ti.s_Color = FONT_RED;
			ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHANGE_DISABLE").c_str() );
		}
		else
		{
			ti.s_Color = NiColor(0.0f,1.0f,0.0f);
			ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHANGE_ENABLE").c_str() );
		}
		AddText( &ti, CsPoint(946, 270));	
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 제한 시간
	AddSprite( CsPoint( 54, 295 ), CsPoint( 912, 36 ), "ServerRelocate\\server_sub_02.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12);
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CONFIRM_LIMITTIME").c_str() );
		AddText( &ti, CsPoint(149, 306));	
	}
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CONFIRM_LIMITTIME_MSG").c_str() );
		AddText( &ti, CsPoint(264, 306));	
	}

	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_RIGHT;
		if( !GetSystem()->IsEnableServerRelocateTime() )
		{
			ti.s_Color = FONT_RED;
			ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHANGE_DISABLE").c_str() );
		}
		else
		{
			ti.s_Color = FONT_GREEN;
			ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHANGE_ENABLE").c_str() );
		}

		AddText( &ti, CsPoint(946, 306));	
	}
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// 계정 공유 창고
	AddSprite( CsPoint( 54, 331 ), CsPoint( 912, 36 ), "ServerRelocate\\server_sub_02.tga");
	{	
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_SHAREDWAREHOUSE").c_str() );
		AddText( &ti, CsPoint(149, 342));
	}
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, FONT_RED );
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_SHAREDWAREHOUSE_MSG").c_str() );
		AddText( &ti, CsPoint(264, 342));
	}
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, FONT_GREEN );
		ti.s_eTextAlign = DT_RIGHT;
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_SHAREDWAREHOUSE_CHECK").c_str() );
		AddText( &ti, CsPoint(920, 342));
		m_pCheckBox = AddCheckBox(  CsPoint(926, 340), CsPoint( 19, 19 ), CsPoint( 0, 19 ), "CheckBT.tga", cWindow::SD_Wi1 );
		if( m_pCheckBox )
		{
			m_pCheckBox->SetCheck( GetSystem()->GetShareHouseLimitCheck() );
			m_pCheckBox->AddEvent(cCheckBox::BUTTON_LBUP_EVENT, this, &CRequestPageUI::_PressCheckBox);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 테이머 슬롯
	AddSprite( CsPoint( 54, 367 ), CsPoint( 912, 36 ), "ServerRelocate\\server_sub_02.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CONFIRM_SLOT").c_str() );
		AddText( &ti, CsPoint(149, 378));	
	}
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CONFIRM_SLOT_MSG").c_str() );
		AddText( &ti, CsPoint(264, 378));	
	}

	{
		cText::sTEXTINFO ti;// 서버를 선택하세요.
		ti.Init( CFont::FS_12, FONT_RED );
		ti.s_eTextAlign = DT_RIGHT;		
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_SELECTED_SERVER_MSG").c_str() );
		m_pTamerSlotCheck = AddText( &ti, CsPoint(946, 378));	
	}
	//////////////////////////////////////////////////////////////////////////
	// 이전 가능 금액
	AddSprite( CsPoint( 54, 403 ), CsPoint( 912, 36 ), "ServerRelocate\\server_sub_02.tga");
	{	
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_LIMIT_MAXMONEY_TITLE").c_str() );
		AddText( &ti, CsPoint(149, 414));
	}
	{
		cText::sTEXTINFO ti;// 서버를 선택하세요.
		ti.Init( CFont::FS_12, FONT_RED );
		ti.s_eTextAlign = DT_RIGHT;		
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_SELECTED_SERVER_MSG").c_str() );
		m_pLimitMoneyText = AddText( &ti, CsPoint(946, 414));

		m_pLimitMoney = NiNew cMoneyIF;
		m_pLimitMoney->Init( cMoneyIF::DISP_FULL, sMONEY( u8(0)), false, FONT_GREEN, DT_LEFT, CFont::FS_12 );
		m_pLimitMoney->SetParentWindow( m_pRoot );
		m_pLimitMoney->SetPos( CsPoint(264, 414) );
		m_pLimitMoney->SetVisible(false);
		AddChildControl( m_pLimitMoney );
	}

#ifndef SDM_TAMER_SERVERRELOCATE_NOCHECK_TAMERNAME_20170914
	//////////////////////////////////////////////////////////////////////////
	{//테이머 명 등록
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14 );
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_REGIST_NAME_TITLE").c_str());
		AddText( &ti, CsPoint(60, 484));
	}

	{	// 이름 체크 문자
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_11, FONT_YELLOW );
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_CHECK_NAME_MSG").c_str() );
		m_pCheckNameMsg = AddText( &ti, CsPoint(264, 489));	
	}

	AddSprite( CsPoint( 54, 509 ), CsPoint( 912, 36 ), "ServerRelocate\\server_sub_02.tga");
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12 );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText(UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_INPUT_NAME").c_str() );
		AddText( &ti, CsPoint(149, 520));	
	}


	AddSprite( CsPoint( 256, 514 ), CsPoint( 570, 26 ), "ServerRelocate\\server_name_field.tga");
	m_pEditBox = NiNew cEditBox;
	if( m_pEditBox )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12 );
		ti.s_Color = NiColor( 0.9f, 0.9f, 0.9f );
		ti.s_eTextAlign = DT_LEFT;
		m_pEditBox->Init( GetRoot(),  CsPoint( 260, 517 ), CsPoint( 570, 28 ), &ti, false );	
		m_pEditBox->EnableUnderline();
		m_pEditBox->SetFontLength( NAME_MAX_LEN );
		m_pEditBox->SetFocus();
		m_pEditBox->AddEvent( cEditBox::eEditbox_ChangeText, this, &CRequestPageUI::_ChangeTamerName );
		AddChildControl( m_pEditBox );
	}

	// 조회 버튼
	m_pCheckNameBtn = AddButton( CsPoint( 838, 512 ), CsPoint( 110, 32 ), CsPoint( 0, 32 ), "ServerRelocate\\common_s_btn.tga");	
	if( m_pCheckNameBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHECK_NAME_BTN").c_str() );
		m_pCheckNameBtn->SetText( &ti );
		m_pCheckNameBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CRequestPageUI::_SendCheckName );
	}
#endif
	{	// ETC
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_12, NiColor(0.3f,0.56f,0.85f) );
		ti.SetText(L"");
		ti.SetLineGabHeight( 5 );
		cText* pMsg = AddText( &ti, CsPoint(54, 572));	
		pMsg->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_ETC").c_str(), 904 );		
	}

	// 뒤로 버튼
	m_pBackBtn = AddButton( CsPoint( ( 1024 - 128 ) / 2 - ((128 / 2) + 10) , 768 - 57 ), CsPoint( 128, 48 ), CsPoint( 0, 48 ), "Event\\event_btn.tga");	
	if( m_pBackBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_BACK_BTN").c_str() );
		m_pBackBtn->SetText( &ti );
		m_pBackBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CRequestPageUI::_BackPage );
	}

	// 신청 버튼
	m_pRequestBtn = AddButton( CsPoint( ( 1024 - 128 ) / 2 + ((128 / 2) + 10), 768 - 57 ), CsPoint( 128, 48 ), CsPoint( 0, 48 ), "Event\\event_btn.tga");	
	if( m_pRequestBtn )
	{
		cText::sTEXTINFO ti;
		ti.Init( CFont::FS_14, NiColor::WHITE );
		ti.s_eTextAlign = DT_CENTER;
		ti.SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGIST_BTN").c_str() );
		m_pRequestBtn->SetText( &ti );
		m_pRequestBtn->AddEvent( cButton::BUTTON_LBUP_EVENT, this, &CRequestPageUI::_SendRequest );
	}

	m_pSelectServer = NiNew cComboBox;
	if( m_pSelectServer )
	{
		m_pSelectServer->Init( GetRoot(), cComboBox::CT_NORMAL, CsPoint(264, 120), CsPoint( 300, 22 ), CFont::FS_12, DT_CENTER, false, false, false);
		m_pSelectServer->ChangeComboBoxImg( "SealMaster\\btn_combobox.tga", CsPoint(0, 22));
		m_pSelectServer->SetComboListWindow( "SealMaster\\dropdown_list_0" );

		int nSIDX = GLOBALDATA_ST.GetCurrentServerIDX();
		std::list<GData::sServerDataInfo> const& pServerInfo = GLOBALDATA_ST.GetServerListInfo();
		std::list<GData::sServerDataInfo>::const_iterator it = pServerInfo.begin();
		for( ; it != pServerInfo.end(); ++it )
		{
			if( nSIDX == (*it).m_nServerIDX )
				continue;

			if( !GetSystem()->IsServerRelocateEnable( (*it).m_nServerIDX ) )
				continue;

			m_pSelectServer->AddItem( (*it).m_ServerName.c_str(), (*it).m_nServerIDX );
		}

		m_pSelectServer->AddEvent(cComboBox::COMBO_SELECTITEM, this, &CRequestPageUI::_SelectChangeServer);
		AddChildControl( m_pSelectServer );
	}

	return true;
}

void CRequestPageUI::Destroy()
{
	DeleteScript();	
}

BOOL CRequestPageUI::UpdateMouse()
{
	if( !IsVisible() )
		return FALSE;

	if(m_pSelectServer && m_pSelectServer->Update_ForMouse() != cComboBox::CI_INVALIDE )
		return TRUE;

	if( m_pBtnClose && m_pBtnClose->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pBackBtn && m_pBackBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pRequestBtn && m_pRequestBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;

	if( m_pCheckNameBtn && m_pCheckNameBtn->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;	

	if( m_pCheckBox && m_pCheckBox->Update_ForMouse() == cButton::ACTION_CLICK )
		return TRUE;
	return FALSE;
}

void CRequestPageUI::Render()
{
	RenderScript();
}

void CRequestPageUI::ResetDevice()
{
	ResetDeviceScript();
}

void CRequestPageUI::_CloseWindow(void* pSender, void* pData)
{
	SAFE_POINTER_RET( GetSystem() );
	GetSystem()->CloseServerChangePage();
}

void CRequestPageUI::_BackPage(void* pSender, void* pData)
{
	SAFE_POINTER_RET( GetSystem() );
	GetSystem()->BackStep();
}

void CRequestPageUI::_SelectChangeServer(void* pSender, void* pData)
{
	SAFE_POINTER_RET( pSender );
	SAFE_POINTER_RET( pData );
	SAFE_POINTER_RET( GetSystem() );

	cComboBox* pComboBox = static_cast<cComboBox*>(pSender);
	int nSelectedIdx = *static_cast<int*>(pData);
	int nSetSelectedIDX = pComboBox->GetData( nSelectedIdx );
	GetSystem()->SetChangedServerIDX( nSetSelectedIDX );
}


void CRequestPageUI::_PressCheckBox(void* pSender, void* pData)
{
	SAFE_POINTER_RET(pSender);
	SAFE_POINTER_RET( GetSystem() );
	cCheckBox* pCheck = static_cast<cCheckBox*>(pSender);
	GetSystem()->SetShareHouseLimitCheck( pCheck->IsCheck() );
}

//////////////////////////////////////////////////////////////////////////
// 이전할 서버에서 사용할 이름이 사용가능하지 체크
//////////////////////////////////////////////////////////////////////////
void CRequestPageUI::_SendCheckName(void* pSender, void* pData)
{
	SAFE_POINTER_RET( m_pEditBox );
	SAFE_POINTER_RET( pSender );

	cButton* pBtn = static_cast<cButton*>(pSender);	
	std::wstring wsInputName = m_pEditBox->GetTextAll();
	if( GetSystem()->SendCheckTamerName( wsInputName ) )
		pBtn->SetEnable(false);
}

void CRequestPageUI::_ReCheckEnable(int const& nServerIdx)
{
	if( m_pTamerSlotCheck )
	{
		int nEmptySlot = GLOBALDATA_ST.GetEmptySlotCountFromServer( nServerIdx );
		if( nEmptySlot <= 0 )
		{
			m_pTamerSlotCheck->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHANGE_DISABLE").c_str() );
			m_pTamerSlotCheck->SetColor( NiColor(1.0f,0.0f,0.0f) );
		}
		else
		{
			m_pTamerSlotCheck->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_REGISTPAGE_CHANGE_ENABLE").c_str() );
			m_pTamerSlotCheck->SetColor( NiColor(0.0f,1.0f,0.0f) );			
		}
	}	

	if( m_pLimitMoneyText )
		m_pLimitMoneyText->SetVisible(false);

	if( m_pLimitMoney )
	{
		UINT64 nMAxMoney = GetSystem()->GetLimitMaxMoney( nServerIdx );
		m_pLimitMoney->SetMoney( nMAxMoney );
		m_pLimitMoney->SetVisible(true);
	}
}

void CRequestPageUI::_SetUILock( bool bValue )
{
	if( m_pRequestBtn )
		m_pRequestBtn->SetEnable(!bValue);

	if( m_pBackBtn )
		m_pBackBtn->SetEnable(!bValue);

	if( m_pBtnClose )
		m_pBtnClose->SetEnable(!bValue);

	if( m_pCheckNameBtn )
		m_pCheckNameBtn->SetEnable(!bValue);

	if( m_pSelectServer )
		m_pSelectServer->SetEnable(!bValue);

	if( m_pEditBox )
		m_pEditBox->SetEnable(!bValue);	
}
//////////////////////////////////////////////////////////////////////////
// 서버이전 한다고 서버에 요청 보냄
//////////////////////////////////////////////////////////////////////////
void CRequestPageUI::_SendRequest(void* pSender, void* pData)
{
	if( !GetSystem()->SendServerChangeRequest() )
		return;

	_SetUILock(true);
}

void CRequestPageUI::_RecvNameCheckResult(bool const& bEnable)
{
	if( m_pCheckNameBtn )
		m_pCheckNameBtn->SetEnable(true);

	SAFE_POINTER_RET( m_pCheckNameMsg );
	if( bEnable )
	{
		m_pCheckNameMsg->SetColor( FONT_GREEN );
		m_pCheckNameMsg->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_ENABLE_NAME_MSG").c_str() );
	}
	else
	{
		m_pCheckNameMsg->SetColor( FONT_RED );
		m_pCheckNameMsg->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_DISABLE_NAME_MSG").c_str() );
	}
}

void CRequestPageUI::_ChangeTamerName(void* pSender, void* pData)
{
	// 테이머 이름 변경시 기존 체크 했던 데이터 삭제
	if( GetSystem() )
		GetSystem()->ReCheckTamerName();
}

//////////////////////////////////////////////////////////////////////////////////////////////

bool CRequestPageUI::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eChangeServerSelected, this );
	GetSystem()->Register( SystemType::eRecvNameCheckPacket, this );
	GetSystem()->Register( SystemType::eRecvItemUseResult, this );
	GetSystem()->Register( SystemType::eReCheckTamerName, this );
	
	return true;
}

void CRequestPageUI::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eChangeServerSelected:
		{
			int nSelectedServer = -1;
			kStream >> nSelectedServer;
			_ReCheckEnable( nSelectedServer );
		}
		break;
	case SystemType::eRecvNameCheckPacket:
		{
			bool bEnableName;
			kStream >> bEnableName;
			
			_RecvNameCheckResult( bEnableName );
		}
		break;
	case SystemType::eRecvItemUseResult:
		{
			bool bResult;
			kStream >> bResult;
			if( !bResult )
				_SetUILock(false);
		}
		break;
	case SystemType::eReCheckTamerName:
		{
			if( m_pCheckNameBtn )
				m_pCheckNameBtn->SetEnable(true);

			if( m_pCheckNameMsg )
			{
				m_pCheckNameMsg->SetColor( FONT_YELLOW );
				m_pCheckNameMsg->SetText( UISTRING_TEXT("TAMER_SERVER_RELOCATE_CHECK_NAME_MSG").c_str() );
			}
		}
		break;
	}
}