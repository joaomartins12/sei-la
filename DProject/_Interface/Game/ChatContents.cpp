#include "StdAfx.h"

#include "ChatContents.h"
// #include "../../Flow/FlowMgr.h"
// #include "../../Flow/Flow.h"

// #include "../../_Lan_English.h"

#include "../Adapt/AdaptFriendShoutOut.h"
#include "../Adapt/AdaptPartySystem.h"
#include "../Adapt/AdaptCommunity.h"

#define IF_CHAT_EDIT_ID				1831		// _EDIT_ID 로 검색후 사용
namespace
{
	static const size_t CHAT_TEXT_MAX_CHARS = 2048;

	inline std::wstring NormalizeChatText( TCHAR const* text )
	{
		if( text == NULL )
			return std::wstring();

		std::wstring safe( text );
		if( safe.length() > CHAT_TEXT_MAX_CHARS )
			safe.resize( CHAT_TEXT_MAX_CHARS );
		return safe;
	}
}

WNDPROC WndProc;
LRESULT CALLBACK ChatContents::EditCallProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	const int isBitSet = (DWORD)lParam & 0x40000000;
	switch(uMsg)
	{
	case WM_KEYDOWN:
		{
			switch(wParam)
			{
			case DMKEYBOARD::KEY_UP:
			case DMKEYBOARD::KEY_DOWN:
				return 0;
			case DMKEYBOARD::KEY_RETURN:
#ifdef TAIWAN_DAYI_IME
				if( g_IME.DaYiComparison() )
				{
					HIMC hIme = ImmGetContext(hWnd);
					ImmNotifyIME( hIme, NI_COMPOSITIONSTR, CPS_CANCEL, 0 );
					ImmReleaseContext(hWnd, hIme);
				}				
#endif
				break;
			}
		}
		break;

	case WM_KEYUP:	//2016-02-04-nova 채팅 이전 입력목록 불러오기
		{
			switch(wParam)
			{
			case DMKEYBOARD::KEY_UP:
				{
					// 분리창내부에서 자체 처리
					if( g_pSeparate )
						return true;
					if( g_pGameIF->GetChatMain()->OnUpKey() == true )
						return true;		
				}
				break;

			case DMKEYBOARD::KEY_DOWN:
				{
					// 분리창내부에서 자체 처리
					if( g_pSeparate )
						return true;

					if(g_pGameIF->GetChatMain()->OnDownKey() == true )
						return true;
				}
				break;

			case DMKEYBOARD::KEY_LEFT:
				{
					// 분리창내부에서 자체 처리
					if( g_pSeparate )
						return true;
				}
				break;

			case DMKEYBOARD::KEY_RIGHT:
				{
					// 분리창내부에서 자체 처리
					if( g_pSeparate )
						return true;
				}
				break;

			case DMKEYBOARD::KEY_SPACE:
				{
					//2016-02-24-nova
					ChatContents* pChatCon = (ChatContents*)CONTENTSSYSTEM_PTR->GetContents(E_CT_CHATTING_STANDARDIZATION);
					pChatCon->_InputCommandProcess();
				}
				break;
			}
		}
		break;

	case WM_CHAR:
		//2015-12-29-nova
		//채팅창이 켜졌을 경우 이쪽으로 메시지가 들어오기 때문에 RETURN과 ESCAPE 처리를 여기서도 해주어야 함
		if(isBitSet == 0)
		{
			switch(wParam)
			{
			case DMKEYBOARD::KEY_RETURN:
				g_pGameIF->OnEnterKey();
				return 0;
			case DMKEYBOARD::KEY_ESCAPE:
				g_pGameIF->OnEscapeKey();
				return 0;
			}
		}
		break;
		/////////////////////////////////////////
	case EM_UNDO:
		return 0;
		break;
	case WM_IME_NOTIFY:
		if( g_IME.OnNotify( hWnd, wParam, lParam ) == true )
			return 0;
		break;
#ifdef NOT_ENTER_CREATENAME
	case WM_IME_COMPOSITION:
		if( lParam&GCS_COMPSTR )
			g_IME.SetCombination(true);
		break;
#endif
	}
	return CallWindowProc(WndProc,hWnd,uMsg,wParam,lParam);
}


ChatContents::ChatContents(void) : m_hEdit(NULL), m_hEditBrush(NULL), m_hEditFont(NULL), m_EditColor(NULL), m_bShowInput(false), m_bShutOut(false),m_pFriendContents(NULL)
,m_pPartyContents(NULL), m_pCommunityContents(NULL)
{	
	// 전역 이벤트 주로 패킷 이동 데이터만 사용한다.	// 채팅 내용을 다른 유저에게 전달하거나 서버에서 받는 채팅 내용은 이 이벤트로 받는다.
	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::EVENT_CHAT_PROCESS, this, &ChatContents::Chat_Process );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::EVENT_SYSTEM_PROCESS, this, &ChatContents::System_Process );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::EVENT_BATTLE_PROCESS, this, &ChatContents::BattleLog_Process );

	GAME_EVENT_STPTR->AddEvent( EVENT_CODE::EVENT_ITEMCROP_PROCESS, this, &ChatContents::ItemCrop_Process );

	_CreateEditWindow();
}

ChatContents::~ChatContents(void)
{
	m_pPartyContents = 0;
	m_pFriendContents = 0;
	m_pCommunityContents = NULL;

	DeleteObject( m_hEditBrush );
	DeleteObject( m_hEditFont );

	GAME_EVENT_STPTR->DeleteEventAll(this);

	m_ChatType_Info.clear();
	m_ChatCommand_Info.clear();
	m_ChatEnterCommand_Info.clear();
}

int const ChatContents::IsContentsIdentity(void)
{
	return E_CT_CHATTING_STANDARDIZATION;
}
int const ChatContents::GetContentsIdentity(void) const
{
	return IsContentsIdentity();
}

bool ChatContents::Initialize(void)
{
#ifdef CHAT_BAN
	m_nBanTime = 0;		//	채팅 금지 남은 시간
	m_nBanEndTime = 0;	// 채팅 금지 끝나는 시간
#endif
	return true;
}
void ChatContents::UnInitialize(void)
{
}

bool ChatContents::IntraConnection(ContentsSystem& System)
{
	m_pFriendContents = new AdaptFriendShoutOut;
	if( m_pFriendContents )
		m_pFriendContents->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_FRIENDLIST ) );

	m_pPartyContents = new AdaptPartySystem;
	if( m_pPartyContents )
		m_pPartyContents->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_PARTYSYSTEM ) );

	m_pCommunityContents = new AdaptCommunityContesnt;
	if( m_pCommunityContents )
		m_pCommunityContents->SetContents( CONTENTSSYSTEM_PTR->GetContents( E_CT_COMMUNITY ) );

	return true;
}

void ChatContents::Update(float const& fElapsedTime)
{
#ifdef CHAT_BAN
	if( _TIME_TS <= m_nBanEndTime )
		m_nBanTime = m_nBanEndTime - _TIME_TS;
	else
		m_nBanTime = 0;
#endif
}
void ChatContents::NotifyEvent(int const& iNotifiedEvt)
{

}

void ChatContents::NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream)
{
}

void ChatContents::MakeMainActorData(void)
{
}

void ChatContents::ClearMainActorData(void)
{
	_SetShowInput(false);
}

void ChatContents::ClearWorldData(void)
{
	InitContentsData();

	m_ChatType_Info.clear();
	m_ChatCommand_Info.clear();
	m_ChatEnterCommand_Info.clear();
}

void ChatContents::MakeWorldData(void)
{
	InitContentsData();
	_LoadDefaultChatInfo();
	_LoadDefaultChatCommandInfo();
	_LoadDefaultChatEnterCommandInfo();	
}

void ChatContents::InitContentsData(void)
{
	m_EnableTS_Emotion.SetDeltaTime( 500 );
	m_nShoutCoolTime = 15500;
	m_ShoutTimeSeq.SetDeltaTime( m_nShoutCoolTime );
	m_ShoutTimeSeq.Reset();
}
ST_CHAT_OPTION* ChatContents::LoadUIOption(int type, bool bLoad)
{
	ST_CHAT_OPTION*	pOpt = NULL;

	pOpt = OPTIONMNG_PTR->LoadPlayerUIOption(type, bLoad);
	SAFE_POINTER_RETVAL(pOpt, NULL);

	return pOpt;
}
void ChatContents::SaveUIOption(int type, ST_CHAT_OPTION*	pThis)
{
	OPTIONMNG_PTR->SavePlayerUIOption(type, pThis);
	return;
}

void ChatContents::LoadUIContents()
{
	COptionUIChat*	pUIChat = OPTIONMNG_PTR->LoadPlayerUIChatClass(XML_CHAT_CONTENTS, true);
	SAFE_POINTER_RET(pUIChat);
	CFileUIContents*	pUIChatContents = static_cast<CFileUIContents*>(pUIChat);
	SAFE_POINTER_RET(pUIChatContents);
	for( int i=0; i<NS_CHAT::MAXCOUNT; ++i)
		m_bChatEnable[i] = pUIChatContents->m_bChatEnable.On[i];
}
void ChatContents::_CreateEditWindow()
{
	// 채팅 입력창 생성.
	m_hEdit = CreateWindow( _T( "edit" ), 
		NULL, 
		WS_CHILD | ES_AUTOHSCROLL, 0, 0, 0, 0, 
		GAMEAPP_ST.GetHWnd(), (HMENU)IF_CHAT_EDIT_ID, 
		GAMEAPP_ST.GetHInstance(), 
		NULL );

	SendMessage( m_hEdit, EM_LIMITTEXT, (WPARAM)(LIMIT_IME_LENGTH-1), 0 );
	m_hEditFont = CreateFont( 12, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, _T( "굴림체" ) );
	SendMessage( m_hEdit, WM_SETFONT, (WPARAM)m_hEditFont, MAKELPARAM( FALSE, 0 ) );
	m_hEditBrush = CreateSolidBrush( RGB(30,44,81) );

#pragma warning( disable : 4311 )
#pragma warning( disable : 4312 )
	WndProc=(WNDPROC)SetWindowLong(m_hEdit,GWL_WNDPROC, (LONG)(EditCallProc) );
#pragma warning( default : 4311 )
#pragma warning( default : 4312 )
}
void ChatContents::_LoadDefaultChatInfo()
{
	// 유저간 일반말. 말풍선 사용.
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::NORMAL,	ST_CHATTYPE_INFO( UISTRING_TEXT( "COMMON_TXT_NORMAL" ), CHAT_NORMAL_COLOR) ) );
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::PARTY,		ST_CHATTYPE_INFO( UISTRING_TEXT( "COMMON_TXT_PARTY" ), CHAT_PARTY_COLOR) ) );
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::GUILD,		ST_CHATTYPE_INFO( UISTRING_TEXT( "COMMON_TXT_GUILD" ), CHAT_GUILD_COLOR) ) );
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::WHISPER,	ST_CHATTYPE_INFO(L"To.%s", CHAT_WHISPER_COLOR) ) );
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::SHOUT,		ST_CHATTYPE_INFO( UISTRING_TEXT( "COMMON_TXT_SHOUT" ), CHAT_SHOUT_COLOR) ) ); // 특정월드채팅
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::CASH_NOR,	ST_CHATTYPE_INFO(L"", CHAT_CASH_COLOR) ) );
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::CASH_SPE,	ST_CHATTYPE_INFO(L"", CHAT_CASH_COLOR) ) );
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::CASH_NOR_TIME,	ST_CHATTYPE_INFO(L"", CHAT_CASH_COLOR) ) );
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::CASH_SPE_TIME,	ST_CHATTYPE_INFO(L"", CHAT_CASH_COLOR) ) );

	m_ChatType_Info.insert( std::make_pair( NS_CHAT::NOTICE,		ST_CHATTYPE_INFO(L"", CHAT_NOTICE_COLOR) ) );
#ifdef GUILD_RENEWAL	// 길드 확성기 스킬
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::GUILD_SHOUT,	ST_CHATTYPE_INFO(L"", CHAT_CASH_COLOR) ) );
#endif
#ifdef CONSIGNMENT_SALE	// 위탁 상점 검색
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::STORE_SERCH,	ST_CHATTYPE_INFO(L"", STORE_SERCH_COLOR) ) );
#endif

	// 일반말 말풍선 사용 안함.
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::NORMAL_TEXT,	ST_CHATTYPE_INFO(L"", CHAT_NORMAL_COLOR) ) );

	// 디지몬 팁
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::DIGIMON_TIP,	ST_CHATTYPE_INFO(L"", CHAT_NORMAL_COLOR) ) );

	// 디버깅용 정보.
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::DEBUG_TEXT,	ST_CHATTYPE_INFO(L"", CHAT_NORMAL_COLOR) ) );

	// 길드랭크, 길드 공지
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::GUILD_RANK,	ST_CHATTYPE_INFO(L"", CHAT_GUILD_COLOR) ) );
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::GUILD_NOTICE,	ST_CHATTYPE_INFO(L"", CHAT_GUILD_COLOR) ) );
	m_ChatType_Info.insert( std::make_pair( NS_CHAT::GM_MEGAPHONE,	ST_CHATTYPE_INFO(L"", NiColor(0.0f, 0.0f, 0.0f)) ) );
}

void ChatContents::_LoadDefaultChatCommandInfo()
{
	list<ST_COMMAND>	ComList;
	ComList.push_back(ST_COMMAND(L"n", NS_CHAT::NORMAL));		// 일반
#if ( defined VERSION_KOR )
	ComList.push_back(ST_COMMAND(L"ㅜ", NS_CHAT::NORMAL));
	ComList.push_back(ST_COMMAND(L"일반", NS_CHAT::NORMAL));
#endif	
	m_ChatCommand_Info.push_back(ComList);

	ComList.clear();
	ComList.push_back(ST_COMMAND(L"p", NS_CHAT::PARTY));			// 파티
#if ( defined VERSION_KOR )
	ComList.push_back(ST_COMMAND(L"ㅔ", NS_CHAT::PARTY));
	ComList.push_back(ST_COMMAND(L"파티", NS_CHAT::PARTY));
#endif
	m_ChatCommand_Info.push_back(ComList);

	ComList.clear();
	ComList.push_back(ST_COMMAND(L"g", NS_CHAT::GUILD));			// 길드
#if ( defined VERSION_KOR )
	ComList.push_back(ST_COMMAND(L"ㅎ", NS_CHAT::GUILD));
	ComList.push_back(ST_COMMAND(L"길드", NS_CHAT::GUILD));
#endif
	m_ChatCommand_Info.push_back(ComList);

	ComList.clear();
	ComList.push_back(ST_COMMAND(L"w", NS_CHAT::WHISPER));		// 귓속말
#if ( defined VERSION_KOR )
	ComList.push_back(ST_COMMAND(L"ㅈ", NS_CHAT::WHISPER));
	ComList.push_back(ST_COMMAND(L"귓속말", NS_CHAT::WHISPER));
#endif
	m_ChatCommand_Info.push_back(ComList);

	ComList.clear();
	ComList.push_back(ST_COMMAND(L"r", NS_CHAT::WHISPER_REPLY));	// 귓속말 응답하기
#if ( defined VERSION_KOR )
	ComList.push_back(ST_COMMAND(L"ㄱ", NS_CHAT::WHISPER_REPLY));
	ComList.push_back(ST_COMMAND(L"응답", NS_CHAT::WHISPER_REPLY));
#endif
	m_ChatCommand_Info.push_back(ComList);

	ComList.clear();
	ComList.push_back(ST_COMMAND(L"s", NS_CHAT::SHOUT));			// 외치기
#if ( defined VERSION_KOR )
	ComList.push_back(ST_COMMAND(L"ㄴ", NS_CHAT::SHOUT));
	ComList.push_back(ST_COMMAND(L"외치기", NS_CHAT::SHOUT));
#endif
	m_ChatCommand_Info.push_back(ComList);
}

//================================================================================================
// 언어별
//================================================================================================
void ChatContents::_LoadDefaultChatEnterCommandInfo()
{
	m_ChatEnterCommand_Info.insert( make_pair( L"trade",	eEC_TRADE ) );
	m_ChatEnterCommand_Info.insert( make_pair( L"invite",	eEC_INVITE ) );
	m_ChatEnterCommand_Info.insert( make_pair( L"leave",	eEC_LEAVE ) );
#ifdef WEB_INFOMATION
	m_ChatEnterCommand_Info.insert( make_pair( L"tamerinfo",	eEC_WEB_INFOMATION ) );
#endif
	m_ChatEnterCommand_Info.insert( make_pair( L"guild",	eEC_GUILD_INVITE ) );
	m_ChatEnterCommand_Info.insert( make_pair( L"friend",	eEC_FRIEND ) );
	m_ChatEnterCommand_Info.insert( make_pair( L"block",	eEC_SHUTOUT ) );
	
#ifdef CONSIGNMENT_SALE
	m_ChatEnterCommand_Info.insert( make_pair( L"shopfinder",	eEC_FIND_STORE ) );
#endif


#ifdef VERSION_KOR
	{
		m_ChatEnterCommand_Info.insert( make_pair( L"거래",		eEC_TRADE ) );
		m_ChatEnterCommand_Info.insert( make_pair( L"초대",		eEC_INVITE ) );
		m_ChatEnterCommand_Info.insert( make_pair( L"탈퇴",		eEC_LEAVE ) );
#ifdef WEB_INFOMATION
		m_ChatEnterCommand_Info.insert( make_pair( L"테이머정보",	eEC_WEB_INFOMATION ) );
#endif
		m_ChatEnterCommand_Info.insert( make_pair( L"길드초대",	eEC_GUILD_INVITE ) );
		m_ChatEnterCommand_Info.insert( make_pair( L"친구",			eEC_FRIEND ) );
		m_ChatEnterCommand_Info.insert( make_pair( L"차단",			eEC_SHUTOUT ) );

#ifdef CONSIGNMENT_SALE
		m_ChatEnterCommand_Info.insert( make_pair( L"상점찾기",	eEC_FIND_STORE ) );
#endif
	}

#elif VERSION_TH
	{
		m_ChatEnterCommand_Info.insert( make_pair( L"แลก",		eEC_TRADE ) );
		m_ChatEnterCommand_Info.insert( make_pair( L"ชวน",		eEC_INVITE ) );
		m_ChatEnterCommand_Info.insert( make_pair( L"ออก",		eEC_LEAVE ) );
		m_ChatEnterCommand_Info.insert( make_pair( L"กิลด์",		eEC_GUILD_INVITE ) );
		m_ChatEnterCommand_Info.insert( make_pair( L"เพื่อน",		eEC_FRIEND ) );
		m_ChatEnterCommand_Info.insert( make_pair( L"ระงับ",		eEC_SHUTOUT ) );
	}
#endif

//#ifdef DUAL_BATTLE														
	//20170706 결투신청 임시 막기 testest
	//m_ChatEnterCommand_Info.insert(make_pair((L"결투신청 "),	eEC_DUAL_BATTLE));
//#endif
// #ifdef OBSERVER_BATTLE	
// 	m_ChatEnterCommand_Info.insert(make_pair((L"obs "), eEC_OBSERVER_BATTLE1));
// 	m_ChatEnterCommand_Info.insert(make_pair((L"obc "), eEC_OBSERVER_BATTLE2));
// #endif

#if ( !defined _GIVE || defined CHEAT_KEY )
	m_ChatEnterCommand_Info.insert( make_pair( (L"find"), eEC_FIND ) );
#endif
}

ChatContents::ST_CHATTYPE_INFO* ChatContents::GetChatType( NS_CHAT::TYPE byType ) const
{
	STL_MAP_ChatInfo_CIT it = m_ChatType_Info.find( byType );
	if( it == m_ChatType_Info.end() )
		return NULL;

	return (ChatContents::ST_CHATTYPE_INFO*)(&it->second);
}

void ChatContents::_WhisperCheck(TCHAR*	pCommandLine, int SendSize)
{
	int nLen = (int)_tcslen( pCommandLine );		
	int nStartIndex = 0;
	int nEndIndex = 0;
	for( int i=1; i<nLen; ++i )
	{
		if( pCommandLine[ i ] == L' ' )
		{
			if( nStartIndex == 0 )
				nStartIndex = i + 1;
			else
				nEndIndex = i;
		}
	}

	if( ( nEndIndex != 0 )&&( nEndIndex - nStartIndex > 1 ) )
	{
		TCHAR szName[ LIMIT_IME_LENGTH ] = { 0, };
		_tcsncpy_s( szName, LIMIT_IME_LENGTH, &pCommandLine[ nStartIndex ], nEndIndex - nStartIndex );

		NS_CHAT::TYPE cType = NS_CHAT::WHISPER;
		_SetChatType( cType, SendSize, szName );
	}
}

void ChatContents::_InputCommandProcess()
{
	TCHAR pCommandLine[ LIMIT_IME_LENGTH + 1 ];
	GetWindowText( m_hEdit, pCommandLine, LIMIT_IME_LENGTH );
	if( pCommandLine[ 0 ] != L'/' )
		return;

	std::wstring wsCmdLine = &pCommandLine[ 1 ];
	std::wstring wsCommand;
	std::wstring wsText;
	_SeparateCommandAndText( wsCmdLine, wsCommand, wsText );

	bool	bCommandCheck = true;
	for( STL_VEC_COMMAND_IT it = m_ChatCommand_Info.begin(); it != m_ChatCommand_Info.end(); ++it)
	{
		LIST_COMMAND	ListCom = *it;	if(bCommandCheck== false) break;
		for( LIST_COMMAND::iterator Lit = ListCom.begin(); Lit != ListCom.end(); ++Lit)
		{
			ST_COMMAND	stCom = *Lit;	if(bCommandCheck== false) break;
			if( wsCommand.compare( stCom.data ) == 0 )
			{
				ST_CHAT_PROTOCOL	CProtocol;
				CProtocol.m_Type = stCom.type;
				INT		startIdx = stCom.size+1;

				if(CProtocol.m_Type == NS_CHAT::WHISPER)
				{
					_WhisperCheck(pCommandLine, stCom.size+1);
				}
				else if( m_szFromWhisper[ 0 ] != NULL && CProtocol.m_Type == NS_CHAT::WHISPER_REPLY)
				{
					_SetChatType( NS_CHAT::WHISPER, startIdx, m_szFromWhisper);
				}
				else
				{
					_SetChatType( CProtocol.m_Type, startIdx );
				}				
				bCommandCheck=false;
			}
		}
	}
	return;
}

void ChatContents::_OpneChatBattleLog(bool	check)
{
	SAFE_POINTER_RET(g_pGameIF);
	if(check)
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_CHAT_BATTLELOG_WINDOW );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CHAT_BATTLELOG_WINDOW );
}

void ChatContents::_OpenChatEditWindow(bool check)
{
	SAFE_POINTER_RET(g_pGameIF);
	if(check)
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_CHAT_EDIT_WINDOW );
	else
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CHAT_EDIT_WINDOW );
}

void ChatContents::_OpneChatOption(bool	check)
{
	SAFE_POINTER_RET(g_pGameIF);

	g_pGameIF->CloseDynamicIF( cBaseWindow::WT_CHATOPTION );

	if(check)
		g_pGameIF->GetDynamicIF( cBaseWindow::WT_CHATOPTION );

}

void ChatContents::_SetEditColor(COLORREF col)
{
	m_EditColor = col;
}
LRESULT ChatContents::_ApplyEditColor( HDC dc )
{
	SetBkColor( dc, RGB( 30, 44, 81 ) );	//SetBkMode(dc, TRANSPARENT);
	SetTextColor( dc, m_EditColor );
	return (LRESULT)m_hEditBrush;
}
void ChatContents::_SetSendData( TCHAR* szStr, NS_CHAT::TYPE eCtype, TCHAR* ToWhisper )
{	
	if( szStr == NULL) return;

#ifdef CHAT_BAN
	if( m_nBanTime > 0 )	//	채팅 금지 중엔 못보내
	{
		std::wstring VaStr = UISTRING_TEXT("CHAT_LIMIT_REMAIN_TIME");
		std::wstring wsHour;
		DmCS::StringFn::From( wsHour, m_nBanTime/3600 );
		DmCS::StringFn::Replace( VaStr, L"#Hour#", wsHour );

		std::wstring wsMin;
		DmCS::StringFn::From( wsMin, (m_nBanTime/60)%60 );
		DmCS::StringFn::Replace( VaStr, L"#Min#", wsMin );

		std::wstring wsSec;
		DmCS::StringFn::From( wsSec, m_nBanTime%60 );
		DmCS::StringFn::Replace( VaStr, L"#Sec#", wsSec );

		_SetText(const_cast<TCHAR*>(VaStr.c_str()), NS_CHAT::NORMAL);
		return;
	}
#endif

	if( szStr[0] != L'/')
	{
		if( IsNetOff() == false )
		{
			nsCsFileTable::g_pCuidMng->CheckChat( szStr );
			uint i=0;	
			while( szStr[i++] == L' ' );	// 띄어쓰기만 있다면 무시한다.
			if( szStr[i-1] != null )
			{
				_SendNetwork(szStr, eCtype, ToWhisper);
			}			
		}
		else
		{
			g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_1, g_pCharMng->GetTamerUser()->GetUniqID(), szStr );
			_SetText( szStr, eCtype );	//_SetText( szStr, eChatType );
		}
	}
	else
	{
		_PreExcute( szStr + 1 );
	}
}


void ChatContents::_SendNetwork( TCHAR* szStr, NS_CHAT::TYPE eCtype, TCHAR* ToWhisper)
{
	if( szStr == NULL) return;

	switch( eCtype )
	{
	case NS_CHAT::NORMAL:
		net::game->SendChatMsg( szStr );
		break;
	case NS_CHAT::SHOUT:
		{
			if( g_pCharMng->GetMyTamerLv() < CHAT_LIMITE_SHOUTING_LV )
			{
				int nLimiteLv = CHAT_LIMITE_SHOUTING_LV;
				cPrintMsg::PrintMsg( 20020, &nLimiteLv );
				return;
			}
			else if( !m_ShoutTimeSeq.IsEnable() )
			{
				int nDeltaTime = CsMax( 1, ( m_nShoutCoolTime - m_ShoutTimeSeq.CalDeltaTime() )/1000 );
				cPrintMsg::PrintMsg( 20026, &nDeltaTime );
			}
			else
				net::game->SendMsgShout( szStr );
		}
		break;

	case NS_CHAT::PARTY:
		if( m_pPartyContents && m_pPartyContents->IsJoinParty() )
			net::game->SendPartyChat( szStr );
		else
			cPrintMsg::PrintMsg( 30078 );
		break;

	case NS_CHAT::GUILD:
		{
			cCommunityContent::sGUILD_INFO guildInfo;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::GET_GUILDINFO, &guildInfo );
			if( guildInfo.s_szGuildName.size() > 0 )
				net::game->SendGuildChat( szStr );
			else
				cPrintMsg::PrintMsg( 30138 );
		}
		break;

	case NS_CHAT::WHISPER:
		assert_cs( ToWhisper != NULL );
		net::game->SendMsgWhisper( ToWhisper, szStr );
		break;
	default:
		assert_cs( false );
	}
}


void ChatContents::_ResetShoutTimeSeq()
{
	m_ShoutTimeSeq.Reset();
}
bool ChatContents::_ClientParcing( TCHAR* str )
{
	if( _tcsncmp( str, _T( "/mc " ), 4 ) == 0 )
	{
		int nFTID = _ttoi( &str[ 4 ] );
		if( nsCsFileTable::g_pDigimonMng->IsDigimon( nFTID ) == true )
			g_pCharMng->GetDigimonUser( 0 )->Scene_ChangePartner( nFTID, -1, _T( "임시용" ), 1.0f, 99 );
		return true;
	}
	return false;
}



void ChatContents::_ShoutCoolTimeReset()
{
	if(nsCsFileTable::g_pBaseMng && nsCsGBTerrain::g_pCurRoot)
	{
		m_nShoutCoolTime = nsCsFileTable::g_pBaseMng->GetShoutSec( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID ) + 500;
		m_ShoutTimeSeq.SetDeltaTime( m_nShoutCoolTime );
	}
	else
	{
		m_ShoutTimeSeq.SetDeltaTime( 15500 );
	}
}

void ChatContents::_SetInputText( TCHAR* sz )
{
	SetWindowText( m_hEdit, sz ); 
	SendMessage( m_hEdit, EM_SETSEL, -2, -1 ); 
}

void ChatContents::_ChanegeInputText( int idx )
{
	TCHAR szEdit[ LIMIT_IME_LENGTH + 1 ] = {0, };
	GetWindowText( m_hEdit, szEdit, LIMIT_IME_LENGTH );
	SetWindowText( m_hEdit,  &szEdit[ idx ]);
	SendMessage( m_hEdit, EM_SETSEL, -2, -1 );
}
void ChatContents::_SendMessage( UINT Msg, WPARAM wParam, LPARAM lParam)
{
	SendMessage( m_hEdit, Msg, wParam, lParam );
}

void ChatContents::_SetEditMoveWindow(int x, int y, int w, int h)
{
	MoveWindow( m_hEdit, x, y, w, h, FALSE );
	InvalidateRect( m_hEdit, NULL, FALSE );
}

void ChatContents::_SeparateCommandAndText(wstring const& wsStr, wstring& wsCommand, wstring& wsText)
{
	size_t nSpaceIdx = wsStr.find_first_of(L" ");
	if( std::wstring::npos == nSpaceIdx )	//	공백이 없으면.
		wsCommand = wsStr;
	else
	{
		wsCommand = wsStr.substr( 0, nSpaceIdx );
		wsText = wsStr.substr( nSpaceIdx + 1, wsStr.size() - nSpaceIdx + 1 );
	}

	//	std::transform(szStr.begin(), szStr.end(), szStr.begin(), toupper);	//	toupper : 소문자 -> 대문자로
	std::transform(wsCommand.begin(), wsCommand.end(), wsCommand.begin(), tolower);		//	tolower : 대문자 -> 소문자로
}

bool ChatContents::_CheckEmotion(wstring const& wsCommand, wstring const& wsText)
{
	SAFE_POINTER_RETVAL( g_pCharMng, false );
	CTamerUser* pUser = g_pCharMng->GetTamerUser();
	SAFE_POINTER_RETVAL( pUser, false );
	// 라이딩 중에는 사용 불가
	if( pUser->IsRide() )
		return false;

	SAFE_POINTER_RETVAL( net::game, false );
	if( m_EnableTS_Emotion.IsEnable() )
	{
		m_EnableTS_Emotion.SetDisable();

		int nFindIndex = -1;
		SAFE_POINTER_RETVAL( nsCsFileTable::g_pTamerMng, false );
		CsEmotion::sINFO* pInfo = nsCsFileTable::g_pTamerMng->FindEmotion( const_cast<TCHAR*>(wsCommand.c_str()), nFindIndex );
		SAFE_POINTER_RETVAL( pInfo, false );
		net::game->SendEmoticon( pInfo->s_nID );

		// 이모션 보내고 남은 채팅 보내기
		if( !wsText.empty() )
			_SetSendData( const_cast<TCHAR*>( wsText.c_str() ) );
	}

	return true;
}

bool ChatContents::_PreExcute( wstring  szStr )
{
	// 명령어/내용 으로 따로 보관.
	wstring	FrontCommand;	// 앞줄 명령어
	wstring	BackText;		// 뒷줄 내용
	_SeparateCommandAndText( szStr, FrontCommand, BackText );

	// 이모션 체크
	if( _CheckEmotion( FrontCommand, BackText ) )
		return true;

	MAP_ENTERCOMMAND_CIT pEnterCommand = m_ChatEnterCommand_Info.find( FrontCommand );
	if( pEnterCommand == m_ChatEnterCommand_Info.end() )
		return false;

	switch(pEnterCommand->second)	// 명령어 타입.
	{
		// 정식 명령어.
	case eEC_TRADE:					// 거래
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_TRADE_REQUEST_TAMERNAME, &BackText );
		return true;

	case eEC_INVITE:				// 파티 초대
		{
			// 뒤에 이름이 없으면
			if( BackText.empty() )
			{
				UINT uniqID = 0;
				CsC_AvObject* pTamer = g_pCharResMng->GetTargetMark();
				if( pTamer )
					uniqID = pTamer->GetUniqID();

				GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_INVITE_FROM_UNIQUE_ID, &uniqID );
			}
			else
				GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_INVITE_FROM_TAMERNAME, &BackText );
		}
		return true;

	case eEC_LEAVE:					// 파티 탈퇴
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_PARTY_LEAVE, NULL );
		return true;

	case eEC_WEB_INFOMATION:	// 테이머정보	//	WEB_INFOMATION
		{
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_WEBWIN ) )
				g_pGameIF->CloseDynamicIF( cBaseWindow::WT_WEBWIN );

			g_pGameIF->GetDynamicIF( cBaseWindow::WT_WEBWIN );
			g_pGameIF->GetWebWin()->SetWebControl( cWebWin::eRANK, CsPoint( 50, 200 ), CsPoint( 416, 550 ), (TCHAR*)&szStr[ 6 ] );	
		}
		return true;

	case eEC_GUILD_INVITE:		// 길드 초대
		{
			// 소속된 길드가 없으면
			if( m_pCommunityContents && !m_pCommunityContents->IhaveGuild() )
				return false;

			if( net::game && BackText.size() )
				GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_GUILD_INVITE, &BackText );
			else
				cPrintMsg::PrintMsg( 30137 );	// 접속중이지 않거나, 존재하지 않는 상대입니다.
		}
		return true;

	case eEC_FRIEND:			// 친구 신청
		{
			if( BackText.empty() )
				return false;

			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::FRIEND_LIST, 0, const_cast<TCHAR*>(BackText.c_str()) );
		}
		return true;

	case eEC_SHUTOUT:			// 차단
		{
			if( BackText.empty() )
				return false;

			g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::SHUTOUT_LIST, 0, const_cast<TCHAR*>(BackText.c_str()) );
		}
		return true;

	case eEC_FIND_STORE:			// 상점 찾기
		{
			int	nCount = 0;
			if( g_pTalkBallone )
				nCount = g_pTalkBallone->FindBallooneName( BackText );

			// 채팅창에 검색한 내용이 몇개가 일치하는지 표시
			std::wstring wsMsg = UISTRING_TEXT( "CHAT_FIND_EMPLOYMENT_STORE" );
			DmCS::StringFn::Replace( wsMsg, L"#Count#", nCount );
			_SetText( wsMsg.c_str(), NS_CHAT::STORE_SERCH);
		}
		return true;
	case eEC_FIND:				// 디버깅용 본섭 제외한 서버에서 동작. 치트 키 정보 찾기.
		{
			FindCommandFunc( BackText );
		}
		return true;
	}
	return false;
}


void ChatContents::_InitData()
{
	m_eChatType = NS_CHAT::NORMAL;

	m_bShowInput = false;
#ifdef DEFAULT_IME_HANGEUL
	m_bHangeul = false;
#endif

	memset( m_szToWhisper, 0, LIMIT_IME_LENGTH );
	memset( m_szFromWhisper, 0, Language::pLength::name + 1 );

	for( int i=0; i<FAST_CHAT_COUNT; ++i )
		memset( m_szFastChat[i], 0, LIMIT_IME_LENGTH );


	_InitFastChat();
	for( int i=0; i<NS_CHAT::MAXCOUNT; ++i)
		m_bChatEnable[i] = true;

	LoadUIContents();
}

void ChatContents::_SetChatType( NS_CHAT::TYPE eChatType, int nStartStringIndex, TCHAR* szSubData /*=NULL*/, bool bfocus /*=true*/ )
{
	_SetShowInput(bfocus);

	ST_CHATTYPE_INFO*	pTypeInfo = GetChatType(eChatType);
	if(pTypeInfo == NULL) return;

	SAFE_POINTER_RET(pTypeInfo);

	m_eChatType = eChatType;

	ContentsStream kStream;
	kStream << eChatType;

	Notify(eSetTypeRadioButton, kStream);
	//m_pChatTypeRadio->SetCheckIndex( eChatType );

	NiColor	color;
	wstring	StrTypeName;

	switch( eChatType )
	{
	case NS_CHAT::NORMAL:
	case NS_CHAT::PARTY:
	case NS_CHAT::GUILD:
	case NS_CHAT::SHOUT:
		color		=	pTypeInfo->GetColor();
		StrTypeName =	pTypeInfo->GetText();
		StrTypeName.append( L" : " );
		break;

	case NS_CHAT::WHISPER:
		{
			_SendMessage(EM_LIMITTEXT, (WPARAM)(LIMIT_IME_LENGTH-1), 0 );
			//m_EditWindow->_SetTypeColor( pTypeInfo->GetColor() );
			if( ( szSubData == NULL )||( szSubData[ 0 ] == NULL ) )
			{
				_FailedMessage(NS_CHAT::WHISPER);
				_SetChatType( NS_CHAT::NORMAL, nStartStringIndex, szSubData );
				return;
			}
			else
			{
				CTamerUser*	pTamer = (g_pCharMng) ? g_pCharMng->GetTamerUser() : NULL;
				if( pTamer && ( _tcscmp( pTamer->GetName(), szSubData ) == 0 ) )	// 자신과 이름 같은지비교. 
				{
					_SetInputText(_T( ""));		//SetWindowText( m_hEdit, _T( "" ) );
					_SetChatType( NS_CHAT::NORMAL, 3 );
					return;
				}

				assert_cs( szSubData != NULL );
				_tcscpy_s( m_szToWhisper, LIMIT_IME_LENGTH, szSubData );
				TCHAR sz[ 128 ];
				_stprintf_s( sz, 128, _T( "To.%s : " ), m_szToWhisper );
				nStartStringIndex += (int)_tcslen( m_szToWhisper );

				color		=	pTypeInfo->GetColor();
				StrTypeName =	sz;
				//m_EditWindow->_SetTypeColor( pTypeInfo->GetColor() );
				//m_EditWindow->_SetTypeText( sz );
			}			
		}
		break;

	case NS_CHAT::CASH_NOR:
	case NS_CHAT::CASH_SPE:
	case NS_CHAT::CASH_NOR_TIME:
	case NS_CHAT::CASH_SPE_TIME:
	case NS_CHAT::GM_MEGAPHONE:
		break;
	default:
		assert_cs( false );
	}

	if( nStartStringIndex > 0 )
	{
		_ChanegeInputText(nStartStringIndex);
	}

	ContentsStream kStream2;
	kStream2 << color << StrTypeName;

	Notify(eSetEditWindow, kStream2);

	//m_EditWindow->_SetTypeColor( pTypeInfo->GetColor() );
	//m_EditWindow->_SetTypeText( pTypeInfo->GetText() );

	Notify(eChatTypeUpdate);	
}

void ChatContents::_SetChatTypeToWhisper( NS_CHAT::TYPE eChatType, int nStartStringIndex, bool bfocus )
{
	_SetChatType(eChatType, nStartStringIndex, m_szToWhisper, bfocus);
}
void ChatContents::_SetChatEnable( NS_CHAT::TYPE ct, bool bEnable )
{
	m_bChatEnable[ ct ] = bEnable;

	Notify(NS_CHAT::SET_ENABLE_RESET_SCROLL);	
}

bool ChatContents::_IsChatEnable( NS_CHAT::TYPE ct )
{
	return m_bChatEnable[ ct ];
}
bool ChatContents::_ChatEnableShow(int _type, bool _megaphone)
{
	switch(_type)
	{
	case NS_CHAT::GUILD_RANK:
	case NS_CHAT::GUILD_NOTICE:
		if( m_bChatEnable[ NS_CHAT::GUILD ] == false )	// 그리기 제외
			return true;
		break;

	case NS_CHAT::NORMAL:
	case NS_CHAT::PARTY:
	case NS_CHAT::GUILD:
	case NS_CHAT::WHISPER:
	case NS_CHAT::SHOUT:	
#ifdef MEGAPHONE_BAN
	case NS_CHAT::MEGAPHONE:
#endif
		if( m_bChatEnable[ _type ] == false )	// 그리기 제외
			return true;
		break;
#ifdef MEGAPHONE_BAN		
	case	NS_CHAT::CASH_NOR:
	case	NS_CHAT::CASH_SPE:
	case	NS_CHAT::CASH_NOR_TIME:
	case	NS_CHAT::CASH_SPE_TIME:
	case	NS_CHAT::GM_MEGAPHONE:
#ifdef GUILD_RENEWAL
	case	NS_CHAT::GUILD_SHOUT:
#endif
		if( m_bChatEnable[ NS_CHAT::MEGAPHONE ] == false )
			return _megaphone;
		break;
#endif
	default:
		return false;
		break;
	}
	return false;
}

bool ChatContents::_IsShowInput()
{
	return m_bShowInput;
}
void ChatContents::_SetShowInput( bool bShow, TCHAR*	pInput)
{
	GLOBALINPUT_ST.KeyDNReset();	//2016-03-03-nova

	m_bShowInput = bShow;

	if( m_bShowInput)
	{
		cCreateName::DeleteInstance();
		cSeparate::DeleteInstance();
		_SetInputText(pInput);
	}
	g_IME.SetUsingEditDlg( m_bShowInput );

	// 채팅 입력창 표시 여부
	Notify(NS_CHAT::FOCUS_UPDATE);
}

void ChatContents::_SetFromWhisper( TCHAR* szFromWhisper )
{
	_tcscpy_s( m_szFromWhisper, Language::pLength::name + 1, szFromWhisper );
}

bool ChatContents::_OnEnterKey()
{
#ifdef DEFAULT_IME_HANGEUL
	if( m_bHangeul == false )
	{
		HIMC hIMC;
		DWORD dwConversion;
		DWORD dwSentence;
		if( hIMC = ImmGetContext( GetActiveWindow() ) )
		{
			ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
			ImmSetConversionStatus( hIMC, dwConversion | IME_CMODE_HANGEUL, IME_SMODE_NONE );
			ImmReleaseContext( GetActiveWindow(), hIMC );
		}
		m_bHangeul = true;
	}
#endif	
	if( _IsShowInput() == false )
	{
		_SetShowInput(true);
		return true;
	}
	else if( _IsShowInput() == true )
	{
#ifdef NOT_ENTER_CREATENAME
		if( g_IME.GetCombination() == true )
		{
			g_IME.SetCombination(false);
			return true;
		}
#endif
		TCHAR str[ LIMIT_IME_LENGTH + 1 ];
		GetWindowText( _GetEditHwnd(), str, LIMIT_IME_LENGTH );

#ifdef VERSION_TH
		for( int i=0; i<LIMIT_IME_LENGTH; ++i )
		{
			if( str[i] != L'◙' )	// ALT + 10 으로 조합되는 아스키 콤보 문자 (개행 기능을 함)
				continue;
			_SetShowInput(false);
			return true;	// ALT + 10 문자가 있다면 보여주지 말고 그냥 리턴
		}
#endif	
		if( str[ 0 ] != NULL )
		{
			_SetFastChat( str );
			_SetSendData(str, m_eChatType, m_szToWhisper);
		}
		_SetShowInput(false);
	}
	return true;
}

void ChatContents::_GetUserName(int _type, TCHAR* _pName, TCHAR const* _pText)
{
	int textsize = lstrlen(_pText);
	bool bStart = true;		
	if(_type == NS_CHAT::WHISPER) bStart = false;	// 귓속말일 경우만 .부터 ID검색시작
	int nOwnerIndex = 0;
	for( int i=0; i<textsize; ++i )
	{
		if( bStart == false )
		{
			if( _pText[ i ] == L'.' )
				bStart = true;						
		}
		else
		{
			if( _pText[ i ] != L' ' )
				_pName[ nOwnerIndex ] = _pText[ i ];
			else
				break;
			++nOwnerIndex;
			if(nOwnerIndex > Language::pLength::GuildName-1)
				break;	// 예외 처리
		}
	}

}
bool ChatContents::_SetText( uint nOwnerUID, TCHAR const* szText, int nType /* = NS_CHAT::NORMAL */, int nValue /* = 0 */ )
{
	std::wstring safeText = NormalizeChatText( szText );
	if( safeText.empty() )
		return false;

	//	그리기 제외인 경우 값 않받는다. 메가폰은 값 받는다.
	if(_ChatEnableShow(nType, false))
		return false;

	CsC_AvObject* pObject = g_pMngCollector->GetObject( nOwnerUID );
	if( pObject == NULL )
		return false;

	// 차단된 ID이면 패쓰
	if( m_pFriendContents->IsShoutOutUser( pObject->GetName() ) )
		return false;

	TCHAR sz[ 1024 ];
	_stprintf_s( sz, 1024, _T( "%s : %s" ), pObject->GetName(), safeText.c_str() );	
	_SetText( sz, nType, nValue );

	return true;
}


void ChatContents::_SetText( TCHAR const* szText, int nType /* = NS_CHAT::NORMAL */, int nValue /* = 0 */ )
{
	std::wstring safeText = NormalizeChatText( szText );
	if( safeText.empty() )
		return;

	// 내용이 없으면 제외.
	if(lstrlen(safeText.c_str())==0)
		return;

	//	그리기 제외인 경우 값 않받는다. 메가폰은 값 받는다.
	if(_ChatEnableShow(nType, false))
		return;

	// 태국 버전을 위해 기본 크기를 name => GuildName 으로크게 설정.
	TCHAR szOwnerName[ Language::pLength::GuildName ] = {0, };

	// 타입 체크해서 이름을 얻어 내자
	_GetUserName(nType, szOwnerName, szText);

	// 채팅 데이터 입력
	bool	bOutline = true;	
	int		nCurCut = 346;		//채팅 내용 보여줄 폭

	ChatContents::ST_CHATTYPE_INFO*	pTypeInfo = GetChatType((NS_CHAT::TYPE)nType);
	if(pTypeInfo == NULL) return;

	NiColor color = pTypeInfo->GetColor();
	if( color == CHAT_CASH_COLOR)
	{
		bOutline = false;	nCurCut = 345;
	}


	// 문자열을 길이이에 맞게 자름.
	ContentsStream kStream;

	wstring		wStr1 = safeText;

	kStream << bOutline << nCurCut << color << nType << nValue << wStr1;

	Notify(NS_CHAT::ADDSTRING_UPDATE, kStream);

	// 자신의 채팅이면 최신 글 보기.
	if(g_pCharMng)
	{
		CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();		
		if( pTamerUser )
		{
			if( lstrcmp( pTamerUser->GetName(), szOwnerName) == 0 )	// 같은 이름이면 최신글 보여주기.
				Notify(NS_CHAT::CHAT_NEWLIST, kStream);				
		}
	}
}

void ChatContents::_SetDebug( TCHAR const* szText, int nType,	int nValue)
{
#if (!defined _GIVE)
	_SetText(szText, nType, nValue);
#endif
}
void ChatContents::_AddSetTextBalloon(ST_CHAT_PROTOCOL CProtocol)
{	
	if( _SetText(CProtocol.m_Index, const_cast<TCHAR*>(CProtocol.m_wStr.c_str()), CProtocol.m_Type, CProtocol.m_value ) == true )
	{
		if( g_pResist && g_pResist->m_Global.s_bChatBalloone == true )
		{
			CsC_AvObject* pObject = g_pMngCollector->GetObject( CProtocol.m_Index );
			if( pObject == NULL )
				return;

			if( pObject->GetProp_Alpha()->IsHideDistOut() == false )
			{
				g_pTalkBallone->SetBalloone( cTalkBalloon::TYPE_1, CProtocol.m_Index, const_cast<TCHAR*>(CProtocol.m_wStr.c_str()) );
			}
		}
	}
}


void ChatContents::_FailedMessage(int type)
{
	switch(type)
	{
	case	NS_CHAT::WHISPER:	// 귓속말 형식이 틀린경우
		{
			_SetText( UISTRING_TEXT( "CHAT_MESSAGE_FAIL_WHISPER_FORMAT" ).c_str() );
		}
		break;
	}

}
void ChatContents::Chat_Process(void* pData)
{
	SAFE_POINTER_RET(pData);
	ST_CHAT_PROTOCOL CProtocol = *static_cast<ST_CHAT_PROTOCOL*>(pData);
	//차단 목록에 추가된 테이머라면 채팅을 보여주지 않는다.
	std::size_t strPos = CProtocol.m_wStr.find(' ');
	std::wstring szTamerName = CProtocol.m_wStr.substr(0, strPos);
	//귓말만 from.Tamername : XXXX 의 형태로 온다. 2017.02.02
	if(CProtocol.m_Type == NS_CHAT::WHISPER)
	{
		std::size_t strPosDot = CProtocol.m_wStr.find('.')+1;
		szTamerName = CProtocol.m_wStr.substr(strPosDot, strPos-strPosDot);
	}
	if( m_pFriendContents->IsShoutOutUser( szTamerName ) )
		return;

	switch(CProtocol.m_Type)
	{
	case NS_CHAT::NORMAL:			// 일반말 + 말풍선
		_AddSetTextBalloon(CProtocol);
		break;
	case NS_CHAT::NORMAL_TEXT:		// 일반말 유저이외의.
		_SetText( const_cast<TCHAR*>(CProtocol.m_wStr.c_str()), CProtocol.m_Type, CProtocol.m_value);
		break;
	case NS_CHAT::DIGIMON_TIP:		// 일반말 디지몬의 팁설명.
		_SetText( CProtocol.m_Index, const_cast<TCHAR*>(CProtocol.m_wStr.c_str()), CProtocol.m_Type, CProtocol.m_value);
		break;

	case NS_CHAT::STORE_SERCH:
	case NS_CHAT::PARTY:
	case NS_CHAT::GUILD:
	case NS_CHAT::WHISPER:
	case NS_CHAT::SHOUT:
	case NS_CHAT::NOTICE:
#ifdef GUILD_RENEWAL
	case NS_CHAT::GUILD_SHOUT:
#endif
	case NS_CHAT::CASH_NOR:
	case NS_CHAT::CASH_SPE:
	case NS_CHAT::CASH_NOR_TIME:
	case NS_CHAT::CASH_SPE_TIME:	
	case NS_CHAT::GUILD_RANK:
	case NS_CHAT::GUILD_NOTICE:
	case NS_CHAT::GM_MEGAPHONE:
		_SetText( const_cast<TCHAR*>(CProtocol.m_wStr.c_str()), CProtocol.m_Type, CProtocol.m_value);
		break;

	case NS_CHAT::WHISPER_REPLY:
		_SetChatType( NS_CHAT::WHISPER, CProtocol.m_wStr.size(), const_cast<TCHAR*>(CProtocol.m_wStr.c_str()) );
		break;

	case NS_CHAT::DEBUG_TEXT:
		_SetDebug( const_cast<TCHAR*>(CProtocol.m_wStr.c_str()), CProtocol.m_Type, CProtocol.m_value);
		break;
	case NS_CHAT::SET_SHOWINPUT:	// 채팅창 visible on/off
		_SetShowInput(CProtocol.m_value ? true : false);
		break;	
	case NS_CHAT::SET_ENABLE:		// 특정 채팅창 enable on/off, 스크롤 맨 끝으로 초기화
		_SetChatEnable( (NS_CHAT::TYPE)CProtocol.m_Index,  CProtocol.m_value ? true : false);
		break;

	case NS_CHAT::ADD_EMOTION:
		{
			ContentsStream kStream;
			kStream << CProtocol.m_value;
			Notify(NS_CHAT::ADD_EMOTION, kStream);
		}
		break;

	case NS_CHAT::SET_BANTIME:		
#ifdef CHAT_BAN
		_SetChatBanTime(CProtocol.m_value);
#endif
		break;
	case NS_CHAT::RESET_SHOUTTIME:
		_ResetShoutTimeSeq();
		break;
	case NS_CHAT::SET_FROMWHISPER:
		_SetFromWhisper(const_cast<TCHAR*>(CProtocol.m_wStr.c_str()));
		break;
	case NS_CHAT::FOCUS_UPDATE:
		Notify(CProtocol.m_Type);
		break;
	default:
		//Notify(CProtocol.m_Type, kStream);
		break;
	}	
}

void ChatContents::System_Process(void* pData)
{
	SAFE_POINTER_RET(pData);
	ContentsStream kStream = *static_cast<ContentsStream*>(pData);

	kStream.SetRp(0);	// 혹시 모를 일을 대비해 처음부터 읽는다.
	wstring str;
	bool bParcing = false;	bool bCheckSameString = false;
	kStream >> str >> bParcing >> bCheckSameString;

	Notify(NS_CHAT::SYSTEM_TEXT, kStream);
}
void ChatContents::BattleLog_Process(void* pData)
{
	SAFE_POINTER_RET(pData);
	ContentsStream kStream = *static_cast<ContentsStream*>(pData);

	Notify(NS_CHAT::BATTLE_TEXT, kStream);
}
void ChatContents::_InitFastChat()
{
	m_nCurFastIndex = -1;
	for( int i=0; i<FAST_CHAT_COUNT; ++i )
		m_szFastChat[ i ][ 0 ] = NULL;
}

void ChatContents::_SetFastChat( TCHAR* szText )
{
	m_nCurFastIndex = -1;

	if( szText[ 0 ] == NULL )
		return;

	// 같다면 리턴
	if( _tcscmp( m_szFastChat[ 0 ], szText ) == 0 )
		return;

	for( int i=FAST_CHAT_COUNT-1; i>0; --i )
	{
		_tcscpy_s( m_szFastChat[ i ], LIMIT_IME_LENGTH, m_szFastChat[ i - 1 ] );
	}
	_tcscpy_s( m_szFastChat[ 0 ], LIMIT_IME_LENGTH, szText );
}

TCHAR* ChatContents::_GetFastChat_Up()
{
	if( (++m_nCurFastIndex) >= FAST_CHAT_COUNT )
	{
		m_nCurFastIndex = FAST_CHAT_COUNT - 1;
	}

	// 반환 값이 존재 하지 않는다면
	if( m_szFastChat[ m_nCurFastIndex ][ 0 ] == NULL )
	{
		if( m_nCurFastIndex > 0 )
		{
			m_nCurFastIndex -= 2;
			return _GetFastChat_Up();
		}
	}	
	return m_szFastChat[ m_nCurFastIndex ];
}

TCHAR* ChatContents::_GetFastChat_Down()
{
	if( (--m_nCurFastIndex) < 0 )
	{
		m_nCurFastIndex = 0;
	}

	// 반환 값이 존재 하지 않는다면
	if( m_szFastChat[ m_nCurFastIndex ][ 0 ] == NULL )
	{
		if( m_nCurFastIndex > 0 )
		{			
			return _GetFastChat_Down();
		}
	}	

	return m_szFastChat[ m_nCurFastIndex ];
}

// 채팅 금지 시간 설정
#ifdef CHAT_BAN
void ChatContents::_SetChatBanTime( u4 nTime )
{
	m_nBanEndTime = nTime + _TIME_TS;
}
#endif

void ChatContents::ItemCrop_Process( void* pData )
{
	SAFE_POINTER_RET(pData);
	ContentsStream kStream = *static_cast<ContentsStream*>(pData);

	Notify(NS_CHAT::SYSTEM_TEXT_ITEMCROP, kStream);
}

void ChatContents::FindCommandFunc( std::wstring const& findText )
{
	if( findText.empty() )
		return;

	CsItem::MAP_IT it = nsCsFileTable::g_pItemMng->GetItemMap()->begin();
	CsItem::MAP_IT itEnd = nsCsFileTable::g_pItemMng->GetItemMap()->end();
	for( ;it != itEnd; it++ )
	{
		CsItem* pItem = it->second;
		SAFE_POINTER_CON( pItem );

		CsItem::sINFO * pInfo = pItem->GetInfo();
		SAFE_POINTER_CON( pInfo );

		TCHAR* p = NULL;
		if(findText.size())
			p = _tcsstr( pInfo->s_szName, const_cast<TCHAR*>(findText.c_str()) );
		if( p != NULL )
		{
			TCHAR sz[ 1024 ] = { 0, };
			_stprintf_s( sz, 1024, _T( "%d ===> %s" ), pInfo->s_dwItemID, pInfo->s_szName );
			_SetText( sz );
		}
	}
}
void ChatContents::SetShutOut(bool bShutOut)
{
	m_bShutOut = bShutOut;
}
bool ChatContents::GetShutOut()
{
	return m_bShutOut;
}
