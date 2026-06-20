#include "StdAfx.h"

#include "../../ContentsSystem/ContentsSystem.h"
#include "../../ContentsSystem/ContentsSystemDef.h"

#include "ChatContents.h"
#include "ChatBase.h"
#include "ChatMainWindow.h"


#include <string.h>

//////////////////////////////////////////////////////////////////////////
//////////////////////		ChatEditWindow		//////////////////////////
//////////////////////////////////////////////////////////////////////////
ChatEditWindow::ChatEditWindow(void) :m_pEditTypeText(NULL)
{
	m_pEditTypeText = NULL;

	for(int i=0; i<3; ++i)
		m_pInputBGSprite[i]=NULL;
}

ChatEditWindow::~ChatEditWindow(void)
{
	Destroy();
}
void ChatEditWindow::Destroy()
{	
	cBaseWindow::Delete();
}

void ChatEditWindow::DeleteResource()
{
	DeleteScript();
}

void ChatEditWindow::ResetDevice()
{
	ResetDeviceScript();
}

void ChatEditWindow::ResetMap()
{
}

bool ChatEditWindow::Close( bool bSound /* = true  */ )
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound )
	{
		PlayCloseSound();
	}

	DeleteResource();
	return true;
}

void ChatEditWindow::InitWindow(cWindow*	pWindow)
{

}
void ChatEditWindow::Create( int nValue /*=0*/ )
{
	if( IsLive() == false )	
	{
	}
	cBaseWindow::Init();

	CsPoint ptBGSize = CsPoint(388-18-9, 27);

	//InitScript( pParent, NULL, m_ptRootClient, ptBGSize, true, IFREGION_X::LEFT, IFREGION_Y::TOP);

	InitScript( NULL, "ChatEditWindowParent", m_ptRootClient, ptBGSize, true, IFREGION_X::LEFT, IFREGION_Y::TOP, false);

	m_pInputBGSprite[ 0 ] = AddSprite( CsPoint::ZERO, CsPoint( 9, 27 ), "chat\\Input\\Input_L.tga" );
	m_pInputBGSprite[ 1 ] = AddSprite( CsPoint::ZERO, CsPoint( 388-18, 27 ), "chat\\Input\\Input_C.tga" );
	m_pInputBGSprite[ 2 ] = AddSprite( CsPoint::ZERO, CsPoint( 9, 27 ), "chat\\Input\\Input_R.tga" );

	_SetInputBGPos(CsPoint( 0, 0));	//	m_pInputBGSprite 들 위치 재설정

	//==========================================================================================================================
	cText::sTEXTINFO ti;
#ifdef VERSION_USA
	ti.Init( &g_pEngine->m_FontText, CFont::FS_9, CHAT_NORMAL_COLOR );
#else
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10, CHAT_NORMAL_COLOR );
#endif
	ti.s_eTextAlign = DT_LEFT;
	m_pEditTypeText = AddText( &ti, CsPoint( 10, 7));
	if(m_pEditTypeText)
	{
		std::wstring wsType;
		DmCS::StringFn::Format( wsType, L"%s : ", UISTRING_TEXT( "COMMON_TXT_NORMAL" ).c_str() );
		m_pEditTypeText->SetText( wsType.c_str() );
	}
}


void ChatEditWindow::Update(float const& fDeltaTime)
{
	_UpdateMoveWindow();
}


cBaseWindow::eMU_TYPE ChatEditWindow::Update_ForMouse()
{
	// 키 입력중이 아닐 때	
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();	

	return muReturn;
}

void ChatEditWindow::Render()
{
	RenderScript();
}

void ChatEditWindow::OnLButtonUp( CsPoint pos )
{
	ChatMainWindow*	pChatWindow = g_pGameIF->GetChatMain();
	SAFE_POINTER_RET(pChatWindow);
	if( pChatWindow->IsEnableWindow() )
		pChatWindow->OnLButtonUp(pos);
}
void ChatEditWindow::OnSetFocus()
{
	cBaseWindow::OnSetFocus();
}

void ChatEditWindow::OnKillFocus()
{ 
	cBaseWindow::OnKillFocus();	
}

void ChatEditWindow::_SetShow(bool bShow)
{
	float showAlpha = (bShow ? 1.0f : 0.0f);

	if(m_pEditTypeText)		m_pEditTypeText->SetAlpha(showAlpha);
	if(m_pInputBGSprite[ 0 ])
		m_pInputBGSprite[ 0 ]->SetVisible(bShow);
	if(m_pInputBGSprite[ 1 ])
		m_pInputBGSprite[ 1 ]->SetVisible(bShow);
	if(m_pInputBGSprite[ 2 ])
		m_pInputBGSprite[ 2 ]->SetVisible(bShow);

	SetShowWindow(bShow);
}

void ChatEditWindow::_SetTypePos(CsPoint ePos)
{
	m_pEditTypeText->SetPos( ePos );
}
void ChatEditWindow::_SetTypeText(TCHAR* pText)
{	
	if( m_pEditTypeText == NULL )
		return;

	if( pText == NULL || pText[0] == 0 )
	{
		m_pEditTypeText->SetText( _T( "" ) );
		return;
	}

	m_pEditTypeText->SetText( pText );
}
void ChatEditWindow::_SetTypeColor(NiColor FontColor)
{
	m_pEditTypeText->SetColor( FontColor );
}


void ChatEditWindow::_SetInputBGPos(CsPoint bPos)
{
	m_pInputBGSprite[ 0 ]->SetPos( CsPoint( bPos.x, bPos.y ) );
	m_pInputBGSprite[ 1 ]->SetPos( CsPoint( bPos.x+9, bPos.y ) );
	m_pInputBGSprite[ 2 ]->SetPos( CsPoint( bPos.x+388-9, bPos.y ) );
}

CsRect ChatEditWindow::_GetChatTypePos(CsPoint	tPos)
{
	int nX = tPos.x + 10 + m_pEditTypeText->GetStringSize().x;
	int nY = tPos.y;
	int nW = 345 - m_pEditTypeText->GetStringSize().x;
#ifdef VERSION_USA
	int nH = 11;
#else
	int nH = 12;
#endif

	return	CsRect(CsPoint(nX, nY), CsSIZE(nW, nH));
}

cText* ChatEditWindow::_GetEditTypeText()
{
	return m_pEditTypeText;

}
CsPoint ChatEditWindow::_GetTypePos()
{
	if( m_pEditTypeText )
		return m_pEditTypeText->GetStringSize();

	return CsPoint::ZERO;
}
NiColor ChatEditWindow::_GetTypeColor()	
{
	SAFE_POINTER_RETVAL(m_pEditTypeText, NULL);
	return m_pEditTypeText->GetColor();
}
void ChatEditWindow::_MoveWindow(bool	bMove)
{
	cBaseWindow::m_bActiveMoveWindow = bMove;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////		ChatMainWindow		//////////////////////////
//////////////////////////////////////////////////////////////////////////
void ChatMainWindow::stMegaphonSprite::Allocate()
{
	if(!m_pIcon)
		m_pIcon = NiNew cSprite;
	if(!m_pBack)
		m_pBack = NiNew cSprite;
}
ChatMainWindow::ChatMainWindow(void) :m_EditWindow(NULL),
m_pChatOptionCheck(NULL), m_pEmotion(NULL), m_pBattleLog(NULL)
{
	SetStringPos( CsPoint(8, -40));	
}

ChatMainWindow::~ChatMainWindow(void)
{
	SaveUIOption(XML_CHAT_MAIN);

	Destroy();

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}
void ChatMainWindow::Destroy()
{	
	ChatBase::Destroy();
}

void ChatMainWindow::DeleteResource()
{
	m_EditWindow = NULL;

	ChatBase::DeleteResource();

	GetSystem()->_SetShowInput(false);

	GetSystem()->_OpenChatEditWindow(false);	
}

void ChatMainWindow::ResetDevice()
{
	for(int i=MEGAPHONE_NOR; i<MAX_MEGAPHONE; ++i)
	{
		m_ShoutSpr[i].m_pIcon->ResetDevice();
		m_ShoutSpr[i].m_pBack->ResetDevice();
	}
	ChatBase::ResetDevice();

	if(m_EditWindow)
		m_EditWindow->ResetDevice();

	_ChatTypeUpdate(CsPoint(0, SizelevelHeight(CHAT_SIZE_HEIGHT)+7));
}

void ChatMainWindow::ResetMap()
{
	ChatBase::ResetMap();

	GetSystem()->_ShoutCoolTimeReset();

	//#ifdef DEFAULT_IME_HANGEUL
	//	m_bHangeul = false;
	//#endif
}
void ChatMainWindow::PreResetMap()
{
	GetSystem()->_SetShowInput( false );
}


bool ChatMainWindow::Close( bool bSound /* = true  */ )
{
	ChatBase::Close( bSound );
	return true;
}

void ChatMainWindow::Create( int nValue /*=0*/ )
{
	if( IsLive() == false )	
	{
	}

	_InitData();

	LoadUIOption(XML_CHAT_MAIN);	//OPTIONMNG_PTR->GetPlayerUIOption(XML_CHAT_MAIN);	//OPTIONMNG_PTR->GetPlayerUIOptionValue(XML_CHAT_MAIN, eOPTION_UI_POS_X);

	ChatBase::Create( cBaseWindow::WT_CHAT_MAIN_WINDOW );

	// ChatBase::Create()함수 이후에 실행 
	m_pScrollBar->SetRenderCount( m_Expand.GetLineCnt() -1 );	//	기본 줄수 1개 감소



	//SetMovePosY(-LINE_HEIGHT);	

	//==========================================================================================================================
	// 입력창
	//==========================================================================================================================
	cText::sTEXTINFO ti;
#ifdef VERSION_USA
	ti.Init( &g_pEngine->m_FontText, CFont::FS_9, CHAT_NORMAL_COLOR );
#else
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10, CHAT_NORMAL_COLOR );
#endif
	ti.s_eTextAlign = DT_CENTER;

	CsPoint ptStart = CsPoint( 3, CHAT_SIZE_HEIGHT+SizelevelHeight(0)-18 );
	m_pChatTypeRadio = AddRadioButton( ptStart, CsPoint( 46, 15 ), cCheckBox::IMAGE_RADIO_CHAT1, &ti );	// cCheckBox::IMAGE_RADIO_CHAT1 여기에서 "일반" 글씨 추가
	if(m_pChatTypeRadio)
	{
		ptStart.x += 46;
		m_pChatTypeRadio->AddNode( ptStart, CsPoint( 46, 15 ), cCheckBox::IMAGE_RADIO_CHAT2, &ti );
		ptStart.x += 46;
		m_pChatTypeRadio->AddNode( ptStart, CsPoint( 46, 15 ), cCheckBox::IMAGE_RADIO_CHAT3, &ti );
		ptStart.x += 46;
		m_pChatTypeRadio->AddNode( ptStart, CsPoint( 46, 15 ), cCheckBox::IMAGE_RADIO_CHAT4, &ti );
		ptStart.x += 46;
		m_pChatTypeRadio->AddNode( ptStart, CsPoint( 46, 15 ), cCheckBox::IMAGE_RADIO_CHAT5, &ti );
		m_pChatTypeRadio->SetCheckIndex(0);
	}

	// 채팅옵션
	ptStart.x += 46;
	m_pChatOptionCheck = AddCheckBox( ptStart, CsPoint( 16, 15 ), CsPoint( 0, 16 ), "Chat\\ChatType\\Ch_Optionbox.tga" );
	if(m_pChatOptionCheck)
		m_pChatOptionCheck->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &ChatMainWindow::_OptionCheck);

#ifdef VERSION_USA
	ptStart.x += 22;	m_pEmotion = AddButton( ptStart, CsPoint( 61, 15 ), CsPoint( 0, 15 ), "Chat\\ChatType\\Ch_Nomal.tga" );	// 동작 버튼
	ptStart.x += 61;	m_pBattleLog = AddCheckBox( ptStart, CsPoint( 52, 15 ), CsPoint( 0, 15 ),"Chat\\ChatType\\Ch_Nomal.tga");	// 전투정보 버튼
#else
	ptStart.x += 37;	m_pEmotion = AddButton( ptStart, CsPoint( 46, 15 ), CsPoint( 0, 15 ),"Chat\\ChatType\\Ch_Nomal.tga" );	// 동작 버튼
	ptStart.x += 46;	m_pBattleLog = AddCheckBox( ptStart, CsPoint( 52, 15 ), CsPoint( 0, 15 ),"Chat\\ChatType\\Ch_Nomal.tga" );	// 전투정보 버튼
#endif
	
	if( m_pEmotion )
		m_pEmotion->SetText( cButton::DefTextInfo( UISTRING_TEXT( "CHAT_BTN_EMOTICON" ).c_str() ) );

	if(m_pBattleLog)
	{
		m_pBattleLog->SetCheckBoxText( cButton::DefTextInfo( UISTRING_TEXT( "CHAT_CHECKBOX_BATTLEINFO" ).c_str() ) );
		m_pBattleLog->SetCheck(m_ShowAddWindow);
		GetSystem()->_OpneChatBattleLog(m_ShowAddWindow);
	}

	m_ShoutSpr[MEGAPHONE_NOR].Allocate();
	m_ShoutSpr[MEGAPHONE_NOR].m_pIcon->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14 ), "Chat\\Ye_Icon.tga", false );
	m_ShoutSpr[MEGAPHONE_NOR].m_pBack->Init( NULL, CsPoint::ZERO, CsPoint( 366, LINE_HEIGHT ), "Chat\\Normal.tga", false );
	m_ShoutSpr[MEGAPHONE_NOR].m_pBack->SetAlpha( 0.8f );

	m_ShoutSpr[MEGAPHONE_SPE].Allocate();
	m_ShoutSpr[MEGAPHONE_SPE].m_pIcon->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14 ), "Chat\\Re_Icon.tga", false );
	m_ShoutSpr[MEGAPHONE_SPE].m_pBack->Init( NULL, CsPoint::ZERO, CsPoint( 366, LINE_HEIGHT ), "Chat\\Special.tga", false );
	m_ShoutSpr[MEGAPHONE_SPE].m_pBack->SetAlpha( 0.8f );

	m_ShoutSpr[MEGAPHONE_NOR_TIME].Allocate();
	m_ShoutSpr[MEGAPHONE_NOR_TIME].m_pIcon->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14 ), "Chat\\Gr_Icon.tga", false );
	m_ShoutSpr[MEGAPHONE_NOR_TIME].m_pBack->Init( NULL, CsPoint::ZERO, CsPoint( 366, LINE_HEIGHT ), "Chat\\Green.tga", false );
	m_ShoutSpr[MEGAPHONE_NOR_TIME].m_pBack->SetAlpha( 0.8f );

	m_ShoutSpr[MEGAPHONE_SPE_TIME].Allocate();
	m_ShoutSpr[MEGAPHONE_SPE_TIME].m_pIcon->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14 ), "Chat\\Bl_Icon.tga", false );
	m_ShoutSpr[MEGAPHONE_SPE_TIME].m_pBack->Init( NULL, CsPoint::ZERO, CsPoint( 366, LINE_HEIGHT ), "Chat\\Blue.tga", false );
	m_ShoutSpr[MEGAPHONE_SPE_TIME].m_pBack->SetAlpha( 0.8f );

	m_ShoutSpr[MEGAPHONE_GM].Allocate();
	m_ShoutSpr[MEGAPHONE_GM].m_pIcon->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14 ), "Chat\\Wh_Icon.tga", false );
	m_ShoutSpr[MEGAPHONE_GM].m_pBack->Init( NULL, CsPoint::ZERO, CsPoint( 366, LINE_HEIGHT ), "Chat\\White.tga", false );
	m_ShoutSpr[MEGAPHONE_GM].m_pBack->SetAlpha( 0.8f );
	

#ifdef GUILD_RENEWAL
	// 길드 확성기
	m_ShoutSpr[MEGAPHONE_GUILD].Allocate();
	m_ShoutSpr[MEGAPHONE_GUILD].m_pIcon->Init( NULL, CsPoint::ZERO, CsPoint( 14, 14 ), "Chat\\Re_Icon.tga", false );
	m_ShoutSpr[MEGAPHONE_GUILD].m_pBack->Init( NULL, CsPoint::ZERO, CsPoint( 366, LINE_HEIGHT ), "Chat\\Blue.tga", false );
	m_ShoutSpr[MEGAPHONE_GUILD].m_pBack->SetAlpha( 0.8f );
	m_nCurColor = 1;	// 빨강이 기본값.
#endif
	GetSystem()->_SetChatType( NS_CHAT::NORMAL, 0, NULL, false );

	GetSystem()->_OpenChatEditWindow(true);
	m_EditWindow = g_pGameIF->GetChatEdit();
	if(m_EditWindow)
		m_EditWindow->SetShowWindow(false);

}


void ChatMainWindow::Update(float const& fDeltaTime)
{
	ChatBase::Update(fDeltaTime);
#ifdef GUILD_RENEWAL
	_GuildColorUpdate();
#endif
}

cBaseWindow::eMU_TYPE ChatMainWindow::Update_ForMouse()
{
	// 키 입력중이 아닐 때	
	cBaseWindow::eMU_TYPE muReturn = ChatBase::Update_ForMouse();	

	// 라디오 버튼 클릭
	if( m_pChatTypeRadio->Update_ForMouse() == cCheckBox::ACTION_CLICK )
	{
		GetSystem()->_SetChatTypeToWhisper( (NS_CHAT::TYPE)m_pChatTypeRadio->GetCheckIndex(), 0);
		return muReturn;
	}

	// 전투 로그
	if( m_pBattleLog->Update_ForMouse() == cCheckBox::ACTION_CLICK )
	{
		m_ShowAddWindow = m_pBattleLog->IsCheck();
		GetSystem()->_OpneChatBattleLog(m_ShowAddWindow);
		return muReturn;
	}

	// 동작
	if( m_pEmotion->Update_ForMouse() == cButton::ACTION_CLICK )
	{
		GetSystem()->_SetShowInput(false);
		g_pGameIF->GetPopup()->SetPopup( m_pEmotion->GetClient(), CsPoint( 46, 15 ), cPopUpWindow::EMOTION );
		return muReturn;
	}

	// 윈도우창 이동중일때
	if( cBaseWindow::m_bActiveMoveWindow == true)
		_ChatTypeUpdate(CsPoint(0, SizelevelHeight(CHAT_SIZE_HEIGHT)+7));

	// 채팅 옵션
	if( m_pChatOptionCheck->Update_ForMouse() == cCheckBox::ACTION_CLICK )
	{
		GetSystem()->_OpneChatOption(m_pChatOptionCheck->IsCheck());
		return muReturn;
	}
	return muReturn;
}

void ChatMainWindow::Render()
{
	ChatBase::Render();
}

void ChatMainWindow::StringListRender()
{
	CsPoint pos = m_ptRootClient + CsPoint( GetStringPos().x, SizelevelHeight(CHAT_SIZE_HEIGHT) + GetStringPos().y );

	int nRemainPos = m_pScrollBar->GetMaxPosIndex() - m_pScrollBar->GetCurPosIndex();
	int nRender = m_pScrollBar->GetRenderCount();

	LIST_STRING* pList = m_String.GetList();	
	for(LIST_STRING_IT it = pList->begin(); it != pList->end(); ++it )
	{
		cString*	pStr = *it;
		int			eChatType= pStr->GetValue1();	// 채팅 타입

		if( GetSystem()->_ChatEnableShow(eChatType) )
			continue;

		if( nRemainPos > 0 )
		{
			--nRemainPos;
			continue;
		}
		pStr->Render( pos );	// 채팅 출력
		pos.y -= LINE_HEIGHT;	// 한줄 사이즈만큼
		if( --nRender < 1 )
			break;
	}
}

void ChatMainWindow::ScrollBarUpdate(int SclrollSize)
{
	while( m_String.GetSize() > MAX_CHAT_LINE )
	{
		m_String.RemoveTail();		
	}

	for( int i=0; i<SclrollSize; ++i )
	{
		m_pScrollBar->IncreaseMaxCount( MAX_CHAT_LINE );
	}

	if( m_pScrollBar->IsEnableScroll() == true )
	{
		if( m_pScrollBar->GetCurPosIndex() == ( m_pScrollBar->GetMaxPosIndex() - SclrollSize ) )
		{
			m_pScrollBar->SetCurPosIndex( m_pScrollBar->GetMaxPosIndex() );
		}
	}
	else
	{
		if( m_pScrollBar->GetMaxPosIndex() != 0 )
		{
			m_pScrollBar->SetCurPosIndex( m_pScrollBar->GetMaxPosIndex() );
		}
	}
}

void ChatMainWindow::OnSetFocus()
{
	cBaseWindow::OnSetFocus();

	_InputFocusUpdate();
}

void ChatMainWindow::OnKillFocus()
{ 
	_InputFocusUpdate();

	cBaseWindow::OnKillFocus();
}


void ChatMainWindow::OnLButtonUp( CsPoint pos )
{
	if( GLOBALINPUT_ST.IsAlt() == false )
		return;

	// 채팅창 클릭후 유저에게 귓말 보내기
	CsPoint posChat = m_ptRootClient + CsPoint( GetStringPos().x, SizelevelHeight(CHAT_SIZE_HEIGHT) + GetStringPos().y );
	CsSIZE	ClickSize( 346, LINE_HEIGHT );

	int nRemainPos = m_pScrollBar->GetMaxPosIndex() - m_pScrollBar->GetCurPosIndex();
	int nRender = m_pScrollBar->GetRenderCount();

	// 귓속말 가능 여부 검사
	LIST_STRING* pList = m_String.GetList();	
	for(LIST_STRING_IT it = pList->begin(); it != pList->end(); ++it )
	{
		int eChatType = (*it)->GetValue1();

		if( nRemainPos > 0 )	// 표시할 줄수
		{
			--nRemainPos;
			continue;
		}

		if( GetSystem()->_ChatEnableShow(eChatType) )
			continue;

		if( CURSOR_ST.CheckClickBox( CsRect( posChat, ClickSize ) ) != CURSOR::BUTTON_OUTWINDOW )
		{
			int	Whisper = (*it)->GetValue2();	// 귓말 가능 여부
			if(Whisper== 0)
				return;

			cString::sDATA_STR* pDataStr = (cString::sDATA_STR*)(*it)->FindElement( cString::sELEMENT::DATA_STR );
			if( pDataStr == NULL )
				return;

			GetSystem()->_SetChatType( NS_CHAT::WHISPER, 0, pDataStr->s_szStr );
			return;
		}

		posChat.y -= 17;
		if( --nRender < 1 )
			break;
	}
}

void ChatMainWindow::_MoveUi( void* sender, void* pData )
{
	m_pChatOptionCheck->SetCheck( false);
	GetSystem()->_OpneChatOption(false);

	ChatBase::_MoveUi( NULL, pData );
}


void ChatMainWindow::_ChageBGSize( void* sender, void* pData )
{
	ChatBase::_ChageBGSize(NULL, pData);

	_ChageBGUpdate(sender, pData);
}

void ChatMainWindow::_ChageBGUpdate( void* sender, void* pData )
{
	CsPoint ptStart = CsPoint( 3, CHAT_SIZE_HEIGHT+SizelevelHeight(0)-18 );
	//CsPoint ptStart = CsPoint( 3, 112+SizelevelHeight(0) );
	m_pChatTypeRadio->SetPos( ptStart, CsPoint( 46, 0 ));	
	m_pChatTypeRadio->ResetDevice();

	// 채팅옵션
	ptStart.x += 46*4;	// 라디오 버튼 4개더 있기 때문에 값 더해준다.
	ptStart.x += 46;
	m_pChatOptionCheck->SetPos(ptStart);	
	m_pChatOptionCheck->ResetDevice();
#ifdef VERSION_USA
	ptStart.x += 22;	m_pEmotion->SetPos(ptStart);
	ptStart.x += 61;	m_pBattleLog->SetPos(ptStart);
#else
	ptStart.x += 37;	m_pEmotion->SetPos(ptStart);
	ptStart.x += 46;	m_pBattleLog->SetPos(ptStart);
#endif	
	m_pEmotion->ResetDevice();
	m_pBattleLog->ResetDevice();

	// cScriptUI 사이즈 조절
	if( GetSystem()->_IsShowInput() )
	{
		CsPoint ptBGSize = CsPoint(CHAT_SIZE_WIDTH, SizelevelHeight(CHAT_SIZE_HEIGHT));
		ptBGSize.y += 27;	// 채팅입력창 27 사이즈 포함
		__SetRootSize(ptBGSize);
		ResetDevice();
	}	

	// 스크롤바 재설정
	m_pScrollBar->SetRenderCount( m_Expand.GetLineCnt() -1 );
	m_pScrollBar->ReSize( CsPoint( 16, SizelevelHeight(CHAT_SIZE_HEIGHT) - 31 - 3 -15), CsRect( CsPoint::ZERO, m_ptRootSize ) );
	m_pScrollBar->SetCurPosIndex( m_pScrollBar->GetMaxPosIndex() );	
	m_pScrollBar->SetRange( CsPoint( 0, m_String.GetSize() ) );

	// 채팅 입력 표시
	_ChatTypeUpdate(CsPoint(0, SizelevelHeight(CHAT_SIZE_HEIGHT)+7));
}
bool ChatMainWindow::OnUpKey()
{
	if( GetSystem()->_IsShowInput() == false )
		return false;

	TCHAR* str = GetSystem()->_GetFastChat_Up();
	GetSystem()->_SetInputText(str);
	return true;
}
bool ChatMainWindow::OnDownKey()
{
	if( GetSystem()->_IsShowInput() == false )
		return false;

	TCHAR* str = GetSystem()->_GetFastChat_Down();
	GetSystem()->_SetInputText(str);
	return true;
}

bool ChatMainWindow::OnEscapeKey()
{
	GetSystem()->_SetShowInput(false);

	// 채팅 옵션창이 켜져 있다면 꺼준다
	if( m_pChatOptionCheck->IsCheck() == true )
	{	
		m_pChatOptionCheck->SetCheck( false );
		return true;
	}	
	return false;
}
bool ChatMainWindow::OnEnterKey()
{
	return	GetSystem()->_OnEnterKey();
}

void ChatMainWindow::_InitData()
{
	m_pChatTypeRadio = NULL;
	m_pChatOptionCheck = NULL;
	m_pEmotion = NULL;
	m_pBattleLog = NULL;

	GetSystem()->_InitData();
}

void ChatMainWindow::_OptionCheck( void* pSender, void *pData )
{
	GetSystem()->_OpneChatOption(m_pChatOptionCheck->IsCheck());
}

void ChatMainWindow::_SetEmotion( int nID )
{
	if( nsCsFileTable::g_pTamerMng->IsEmotion( nID ) == false )
		return;

	CsEmotion::sINFO* pEmotionFT = nsCsFileTable::g_pTamerMng->GetEmotion( nID )->GetInfo();

	wstring	wStr = pEmotionFT->s_szCmd[ 0 ];

	// 입력창이 없다면 
	if( GetSystem()->_IsShowInput() == false )
	{
		GetSystem()->_SetShowInput(true);		
		GetSystem()->_SetInputText(const_cast<TCHAR*>(wStr.c_str()));
		return;
	}

	// 입력창 오픈~	
	TCHAR szInput[ LIMIT_IME_LENGTH + 1 ]={0};
	GetWindowText( GetSystem()->_GetEditHwnd(), szInput, LIMIT_IME_LENGTH );
	// 접두어에 이미 명령어가 적혀 있다면 제거
	int nCatInputIndex = 0;
	while( szInput[ nCatInputIndex ] == L'/' )
	{
		int nFindIndex;
		CsEmotion::sINFO* pInfo = nsCsFileTable::g_pTamerMng->FindEmotion( &szInput[ nCatInputIndex + 1 ], nFindIndex );
		if( pInfo )
		{
			nCatInputIndex += (int)_tcslen( pInfo->s_szCmd[ nFindIndex ] );
			if( szInput[ nCatInputIndex ] == L' ' )
				nCatInputIndex += 1;
		}
		else
		{
			break;
		}
	}

	// 접두어에 동작커맨드 입력
	wStr += &szInput[ nCatInputIndex ];
	GetSystem()->_SetInputText(const_cast<TCHAR*>(wStr.c_str()));
}

void ChatMainWindow::_InputFocusUpdate()
{
	CsPoint ptBGSize = CsPoint(CHAT_SIZE_WIDTH, SizelevelHeight(CHAT_SIZE_HEIGHT));

	if( GetSystem()->_IsShowInput() )
	{
		CsPoint	rootPos = GetRootClient();
		ptBGSize.y += 27;

		if(rootPos.y+ptBGSize.y >= g_nScreenHeight)	// 입력창의 위치가 하단보다 더 낮으면
			SetPos(CsPoint(rootPos.x, g_nScreenHeight-ptBGSize.y));		// 채팅창을 위로 올려준다.

		g_pGameIF->SetFocusWindow(m_EditWindow);

		SetFocus( GetSystem()->_GetEditHwnd() );
		ShowWindow( GetSystem()->_GetEditHwnd(), SW_SHOW );
	}
	else
	{		
		//SetFocus( GAMEAPP_ST.GetHWnd() );
		ShowWindow( GetSystem()->_GetEditHwnd(), SW_HIDE );
	}
	__SetRootSize(ptBGSize);
	ResetDevice();

	_ChatTypeUpdate(CsPoint(0, SizelevelHeight(CHAT_SIZE_HEIGHT)+7));

	SAFE_POINTER_RET(m_EditWindow);

	m_EditWindow->_SetShow(GetSystem()->_IsShowInput());	
}

bool ChatMainWindow::_IsShowInput()
{
	return GetSystem()->_IsShowInput();
}
void ChatMainWindow::_AddListStringHead(LIST_STRING* _pList, int _type, int _num, TCHAR* _szName, cText::sTEXTINFO* _TextInfo)
{
	// 귓속말 가능한지 검사
	bool	bWhisper = _CheckWhisper(_type, _num, _szName);

	// 확성기 검사
	int		MegaphonType = _GetMegaphonType(_type);

	// 확성기 레벨 표시 아이콘 사용 여부
	bool	bIcon = false;
	if( MegaphonType == MEGAPHONE_SPE || MegaphonType == MEGAPHONE_SPE_TIME)
		bIcon = true;

	int Line = 0;
	for(LIST_STRING_IT it = _pList->begin() ; it != _pList->end(); ++it )
	{		
		cString* pStr = *it;

		//	SetValue1 == 채팅 타입값(eCHAT_TYPE)	//	SetValue3 == 미사용
		//	SetValue2 == 귓말가능여부 ==>  0 : 1	//	SetValue4 == 미사용
		pStr->SetValue1( _type );
		pStr->SetValue2( bWhisper );
		int AddIconSize = 0;
		// 확성기 설정
		if(MegaphonType != -1)	// 확성기인 경우
		{
			cString::sTEXT* pText = (cString::sTEXT*)pStr->GetElement(0);
			if(Line ==0)
			{
				if(bIcon)
				{
					CsPoint ptPos;		int nBase;
					cText::GetStringSize( _TextInfo, ptPos, nBase, _szName );
					pStr->AddIcon_Head(CsPoint( 14, 14 ), ICONITEM::CHAT_LV, _num / 10, 0, CsPoint( -366+ptPos.x-4, 0 ));
					AddIconSize = 14;
				}
				pStr->AddSprite_Head(m_ShoutSpr[MegaphonType].m_pIcon, CsPoint( -366,0 ));	// 14 14			
			}
			pStr->AddSprite_Head(m_ShoutSpr[MegaphonType].m_pBack, CsPoint( -5, -2 ));	// 366, LINE_HEIGHT

			if(pText)
				pText->s_ptDeltaPos = CsPoint(-366-AddIconSize,0);
		}

		//귓속말할 ID보관
		if(bWhisper)	pStr->AddData_Str( 0, _szName, Language::pLength::GuildName );

		m_String.AddHead( pStr );	// 실 데이터 추가.
		++Line;
	}
}
bool ChatMainWindow::_CheckWhisper(int _type, int _num, TCHAR*	_szName)
{
	bool	result = true;

	if( _szName == NULL )
		return false;

	CTamerUser*	pTamer = (g_pCharMng) ? g_pCharMng->GetTamerUser() : NULL;
	if( pTamer && ( _tcscmp( pTamer->GetName(), _szName ) == 0 ) )	// 자신과 이름 같은지비교. 
		result = false;	
	else if( _type == NS_CHAT::NOTICE)		// 공지인 경우도 불가능
		result = false;
	else if( _type == NS_CHAT::GUILD_RANK || _type == NS_CHAT::GUILD_NOTICE )	// 길드 공지, 길드 랭크인 경우도 불가능
		result = false;
	else if( _type == NS_CHAT::NORMAL_TEXT || _type == NS_CHAT::DIGIMON_TIP)	// 유저가 아닌 일반말인 경우.
		result = false;
	else if( _type == NS_CHAT::DEBUG_TEXT)	// 디버깅용 텍스트 일 경우.
		result = false;

	return result;
}

int	 ChatMainWindow::_GetMegaphonType(int _type)
{
	int		MegaphonType = -1;
	switch(_type)
	{			
	case NS_CHAT::CASH_NOR:		MegaphonType = MEGAPHONE_NOR;		break;
	case NS_CHAT::CASH_NOR_TIME:MegaphonType = MEGAPHONE_NOR_TIME;	break;
	case NS_CHAT::CASH_SPE:		MegaphonType = MEGAPHONE_SPE;		break;
	case NS_CHAT::CASH_SPE_TIME:MegaphonType = MEGAPHONE_SPE_TIME;	break;
#ifdef GUILD_RENEWAL
	case NS_CHAT::GUILD_SHOUT:	MegaphonType = MEGAPHONE_GUILD;		break;
#endif
	case NS_CHAT::GM_MEGAPHONE: MegaphonType = MEGAPHONE_GM;		break;
	default:
		break;
	}
	return MegaphonType;
}


void ChatMainWindow::_ResetString()
{
	m_String.Delete();
	m_pScrollBar->SetRange( CsPoint( 0, 1 ) );
}

void ChatMainWindow::_ChatTypeUpdate(CsPoint	tPos)
{
	SAFE_POINTER_RET(m_EditWindow);


	int	SizeHeight = SizelevelHeight(CHAT_SIZE_HEIGHT);
	CsPoint	RootPos = CsPoint(m_pRoot->GetClient().x, m_pRoot->GetClient().y+SizeHeight);

	CsRect	csRect = m_EditWindow->_GetChatTypePos(RootPos+CsPoint(0,7));	//	+7 채팅창과 입력창 사이의 간격

	m_EditWindow->SetPos(RootPos);
	m_EditWindow->ResetDevice();

	GetSystem()->_SetEditColor(NICOLOR_TO_COLORREF( m_EditWindow->_GetTypeColor() ));
	GetSystem()->_SetEditMoveWindow(csRect.GetPos().x, csRect.GetPos().y, csRect.GetSize().x, csRect.GetSize().y);

}

void ChatMainWindow::_ResetScroll()
{
	bool bLast = (  m_pScrollBar->GetCurPosIndex() == m_pScrollBar->GetMaxPosIndex() );

	int nCount = 0;
	LIST_STRING* pList = m_String.GetList();	
	for(LIST_STRING_IT it = pList->begin(); it != pList->end(); ++it )
	{
		if(GetSystem()->_ChatEnableShow((*it)->GetValue1()))
			continue;
		++nCount;
	}
	m_pScrollBar->SetRange( CsPoint( 0, nCount ) );
	// 스크롤마지막 처리 또는 넘어간 처리
	if( ( bLast == true )||( m_pScrollBar->GetCurPosIndex() > m_pScrollBar->GetMaxPosIndex() ) )
	{
		m_pScrollBar->SetCurPosIndex( m_pScrollBar->GetMaxPosIndex() );
	}
}
NiColor ChatMainWindow::_ChangeColor( NiColor pCurColor, float fR, float fG, float fB )
{
	float fDelta = g_fDeltaTime*0.6f;
	float fAdded;
	//R
	if( pCurColor.r < fR )
	{
		fAdded = pCurColor.r + fDelta;
		if( fAdded >= fR )
			pCurColor.r = fR;
		else
			pCurColor.r = fAdded;
	}
	else if( pCurColor.r > fR )
	{
		fAdded = pCurColor.r - fDelta;
		if( fAdded <= fR )
			pCurColor.r = fR;
		else
			pCurColor.r = fAdded;
	}
	//G
	if( pCurColor.g < fG )
	{
		fAdded = pCurColor.g + fDelta;
		if( fAdded >= fG )
			pCurColor.g = fG;
		else
			pCurColor.g = fAdded;
	}
	else if( pCurColor.g > fG )
	{
		fAdded = pCurColor.g - fDelta;
		if( fAdded <= fG )
			pCurColor.g = fG;
		else
			pCurColor.g = fAdded;
	}
	//B
	if( pCurColor.b < fB )
	{
		fAdded = pCurColor.b + fDelta;
		if( fAdded >= fB )
			pCurColor.b = fB;
		else
			pCurColor.b = fAdded;
	}
	else if( pCurColor.b > fB )
	{
		fAdded = pCurColor.b - fDelta;
		if( fAdded <= fB )
			pCurColor.b = fB;
		else
			pCurColor.b = fAdded;
	}
	return pCurColor;
}

void ChatMainWindow::_GuildColorUpdate()
{
	float fR = 255.0f/255.0f;
	float fG = 255.0f/255.0f;
	float fB = 255.0f/255.0f;	

	switch( m_nCurColor )	// 확성기 BG색 변경
	{
	case 1:	//ㅃ
		fR = 255.0f/255.0f;
		fG = 90.0f/255.0f;
		fB = 90.0f/255.0f;
		break;
	case 2:	//ㅈ
		fR = 255.0f/255.0f;
		fG = 148.0f/255.0f;
		fB = 54.0f/255.0f;
		break;
	case 3:	//ㄴ
		fR = 255.0f/255.0f;
		fG = 255.0f/255.0f;
		fB = 36.0f/255.0f;
		break;
	case 4:	//ㅊ
		fR = 171.0f/255.0f;
		fG = 242.0f/255.0f;
		fB = 0.0f/255.0f;
		break;
	case 5:	//ㅍ
		fR = 0.0f/255.0f;
		fG = 216.0f/255.0f;
		fB = 255.0f/255.0f;
		break;
	case 6:	//ㄴ
		fR = 0.0f/255.0f;
		fG = 84.0f/255.0f;
		fB = 255.0f/255.0f;
		break;
	case 7:	//ㅂ
		fR = 185.0f/255.0f;
		fG = 90.0f/255.0f;
		fB = 255.0f/255.0f;
		break;
	}

#ifdef GUILD_RENEWAL
	NiColor pOnMaskColor = _ChangeColor( m_ShoutSpr[MEGAPHONE_GUILD].m_pBack->GetColor(), fR, fG, fB );
	NiColor pDestColor = NiColor( fR, fG, fB );

	if( pOnMaskColor == pDestColor )
		m_nCurColor++;
	if( m_nCurColor > 7 )
		m_nCurColor = 1;
	m_ShoutSpr[MEGAPHONE_GUILD].m_pBack->SetColor( pOnMaskColor );
#endif
}

bool ChatMainWindow::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( NS_CHAT::ADDSTRING_UPDATE, this );
	GetSystem()->Register( NS_CHAT::FOCUS_UPDATE, this );
	GetSystem()->Register( NS_CHAT::ADD_EMOTION, this );
	GetSystem()->Register( NS_CHAT::CHAT_CLEAR, this );
	GetSystem()->Register( NS_CHAT::SET_ENABLE_RESET_SCROLL, this );
	GetSystem()->Register( NS_CHAT::CHAT_NEWLIST, this );

	GetSystem()->Register( SystemType::eSetTypeRadioButton, this );
	GetSystem()->Register( SystemType::eSetEditWindow, this );
	GetSystem()->Register( SystemType::eChatTypeUpdate, this );

	return true;
}


void ChatMainWindow::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	kStream.SetRp(0);	// 혹시 모를 일을 대비해 처음부터 읽는다.

	UINT	uID=0;
	wstring str;
	NS_CHAT::TYPE	eType = NS_CHAT::NORMAL;
	int		nValue = 0;
	bool	bEnable = false;

	switch(iNotifiedEvt)
	{

	case NS_CHAT::ADDSTRING_UPDATE:
		{
			// 채팅 데이터 입력
			bool	bOutline = true;	
			int		nCurCut = 346;		//채팅 내용 보여줄 폭

			NiColor		color;
			TCHAR		szOwnerName[ Language::pLength::GuildName ] = {0, };

			kStream >> bOutline >> nCurCut >> color >> eType >> nValue >> str;

			// 타입 체크해서 이름을 얻어 내자
			GetSystem()->_GetUserName(eType, szOwnerName, (TCHAR*)str.c_str());

			cText::sTEXTINFO ti;
			ti.Init( &g_pEngine->m_FontText, CFont::FS_10, color );
			ti.s_bOutLine = bOutline;

			cStringList sl;
			g_pStringAnalysis->Cut( &sl, nCurCut, const_cast<TCHAR*>(str.c_str()), &ti );

			// 문자열을 길이이에 맞게 자름.
			int nIncreaseCount = sl.GetSize();

			LIST_STRING* pList = sl.GetList();

			_AddListStringHead(pList, eType, nValue, szOwnerName, &ti);

			pList->clear();

			ScrollBarUpdate(nIncreaseCount);
		}
		break;

	case NS_CHAT::FOCUS_UPDATE:
		_InputFocusUpdate();
		break;

	case NS_CHAT::ADD_EMOTION:
		kStream >> nValue;
		_SetEmotion( nValue );
		break;
	case NS_CHAT::CHAT_CLEAR:
		_ResetString();
		break;
	case NS_CHAT::SET_ENABLE_RESET_SCROLL:
		_ResetScroll();
		break;

	case NS_CHAT::CHAT_NEWLIST:
		LastScrollClick();
		break;

	case SystemType::eSetTypeRadioButton:
		{
			kStream >> eType;
			m_pChatTypeRadio->SetCheckIndex( eType );
		}
		break;
	case SystemType::eSetEditWindow:
		{
			SAFE_POINTER_BEK(m_EditWindow);

			NiColor	color;
			kStream >> color >> str;
			m_EditWindow->_SetTypeColor( color );
			m_EditWindow->_SetTypeText( const_cast<TCHAR*>(str.c_str()) );
		}
		break;
	case SystemType::eChatTypeUpdate:
		_ChatTypeUpdate(CsPoint(0, SizelevelHeight(CHAT_SIZE_HEIGHT)+7));
		break;
	}
}
