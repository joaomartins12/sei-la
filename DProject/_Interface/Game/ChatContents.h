#pragma once

#define MAX_CHAT_LINE				100			// 가지고 있는 채팅 줄 갯수
#define LIMIT_IME_LENGTH			70			// IME 문자 제한
#define FAST_CHAT_COUNT				20

// 컬러들
#define CHAT_NORMAL_COLOR	FONT_WHITE
#define CHAT_PARTY_COLOR	NiColor( 0.043f, 0.933f, 1 )
#define CHAT_GUILD_COLOR	NiColor( 0.106f, 0.914f, 0.349f )

#define CHAT_WHISPER_COLOR	NiColor( 224/255.0f, 102/255.0f, 255/255.0f )
#define CHAT_SHOUT_COLOR	NiColor( 247/255.0f, 211/255.0f, 153/255.0f )

// 채팅창에 위탁상점 검색 시 몇개가 검색되어 지는지 표시되는 텍스트 색
#ifdef CONSIGNMENT_SALE
#define STORE_SERCH_COLOR	NiColor( 0.78f, 0.55f, 0 )
#endif

#define NICOLOR_TO_COLORREF( nicolor )		( RGB( (int)(nicolor.r*255), (int)(nicolor.g*255), (int)(nicolor.b*255) ) )

#define CHAT_CASH_COLOR		NiColor( 0.0f, 0.0f, 0.0f )
#define CHAT_TRADE_COLOR	NiColor( 0.99f, 0.73f, 1 )
#define CHAT_NOTICE_COLOR	NiColor( 0.99f, 0.55f, 0 )


typedef	std::list< cString* >			LIST_STRING;
typedef	std::list< cString* >::iterator	LIST_STRING_IT;

class AdaptFriendShoutOut;
class AdaptPartySystem;
class AdaptCommunityContesnt;

class ChatContents
	: public IUIContentSubject< ChatContents >
{
public:
	static int const IsContentsIdentity(void);

	enum eENTER_COMMAND
	{
		eEC_NONE=-1,		// 없음.

		eEC_TRADE=0,		// "거래"

		eEC_INVITE,			// "초대"

		eEC_LEAVE,			// "탈퇴"

		eEC_WEB_INFOMATION,	// "테이머정보"

		eEC_GUILD_INVITE,	//	"길드초대"
		eEC_FRIEND,			//	"친구"
		eEC_SHUTOUT,		//	"차단"

		eEC_FIND_STORE,		//	"상점찾기"

		eEC_FIND,			//	"find"
	};
public:
	// 타입별 컬러, 텍스트
	typedef struct stChatInfo
	{
		stChatInfo( std::wstring head, NiColor color ):	wStrHead( head ),niColor(color)	{}
		stChatInfo(){};
	private:
		NiColor niColor;		// 타입별 색상
		std::wstring wStrHead;	// 타입별 출력해줄 문자열
	public:
		NiColor	GetColor() const {	return niColor;	}
		TCHAR* GetText() const { return const_cast<TCHAR*>(wStrHead.c_str()); }

	}ST_CHATTYPE_INFO;
	typedef std::map<NS_CHAT::TYPE, ST_CHATTYPE_INFO>	STL_MAP_ChatInfo;
	typedef std::map<NS_CHAT::TYPE, ST_CHATTYPE_INFO>::const_iterator	STL_MAP_ChatInfo_CIT;
	STL_MAP_ChatInfo	m_ChatType_Info;	// 채팅 타입별 정보
	

	// 명령어 모음. SpaceBar로 실행하는 명령어.
	typedef struct stCommand
	{
		std::wstring	data;
		NS_CHAT::TYPE	type;	// 현재 커맨드에 해당하는 타입
		int			size;
		stCommand(std::wstring	str, NS_CHAT::TYPE t) : data(str), type(t)	{size = (int)str.size();}
		stCommand(){}
	}ST_COMMAND;
	typedef	list<ST_COMMAND>					LIST_COMMAND;
	typedef std::vector<LIST_COMMAND>			STL_VEC_COMMAND;
	typedef std::vector<LIST_COMMAND>::iterator	STL_VEC_COMMAND_IT;
	STL_VEC_COMMAND				m_ChatCommand_Info;			// 스페이스바로 실행하는 명령어 정보.

	// 명령어 모음. EnterKey로 실행하는 명령어.
	typedef std::map<std::wstring, eENTER_COMMAND>					MAP_ENTERCOMMAND;
	typedef std::map<std::wstring, eENTER_COMMAND>::const_iterator	MAP_ENTERCOMMAND_CIT;
	MAP_ENTERCOMMAND			m_ChatEnterCommand_Info;	// 엔터키로 실행하는 명령어 정보.

	enum eEventType
	{
		eSetTypeRadioButton,
		eSetEditWindow,
		eChatTypeUpdate,
	};
public:
	ChatContents(void);
	virtual ~ChatContents(void);	

	virtual int const GetContentsIdentity(void) const;
	virtual bool Initialize(void);
	virtual void UnInitialize(void);
	virtual bool IntraConnection(ContentsSystem& System);
	virtual void Update(float const& fElapsedTime);
	virtual void NotifyEvent(int const& iNotifiedEvt);
	virtual void NotifyEventStream(int const& iNotifiedEvt, ContentsStream const& kStream);
	virtual void MakeMainActorData(void);
	virtual void ClearMainActorData(void);
	virtual void MakeWorldData(void);
	virtual void ClearWorldData(void);

private:
	DWORD				m_nShoutCoolTime;
	CsTimeSeq			m_ShoutTimeSeq;
	CsEnableTimeSeq		m_EnableTS_Emotion;		// 동작

private://	귓속말가능한지 검사
	void				_WhisperCheck(TCHAR*	pCommandLine, int SendSize);	
public:	//	채팅 입력 명령어(/명령어. 로 시작하는 구문)
	void				_InputCommandProcess();	// 채팅 입력창 명령어 처리.

public:	
	ST_CHATTYPE_INFO*	GetChatType( NS_CHAT::TYPE byType ) const;

	void				_SetSendData( TCHAR* szStr, NS_CHAT::TYPE eCtype= NS_CHAT::NORMAL, TCHAR* ToWhisper=NULL );
	void				_ShoutCoolTimeReset();
	void				_OpneChatOption(bool	check);
	void				_OpneChatBattleLog(bool	check);
	void				_OpenChatEditWindow(bool check);

	void				_SetInputText( TCHAR* sz );
	void				_ChanegeInputText( int idx );
	void				_SendMessage( UINT Msg, WPARAM wParam, LPARAM lParam);
	void				_SetEditMoveWindow(int x, int y, int w, int h);

private:	// EditWondow	
	HWND				m_hEdit;
	HFONT				m_hEditFont;
	HBRUSH				m_hEditBrush;
	COLORREF			m_EditColor;
public:
	void				_CreateEditWindow();
	void				_SetEditColor(COLORREF col);
	LRESULT				_ApplyEditColor( HDC dc );			// OnMsgHandler() 함수에서 사용
	HWND				_GetEditHwnd(){ return m_hEdit; }	// OnMsgHandler() 함수에서 사용
private:
static LRESULT CALLBACK EditCallProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

private:
	void				_LoadDefaultChatInfo();
	void				_LoadDefaultChatCommandInfo();
	void				_LoadDefaultChatEnterCommandInfo();

private:	
	void				_SendNetwork( TCHAR* szStr, NS_CHAT::TYPE eCtype, TCHAR* ToWhisper);
private:
	void				_ResetShoutTimeSeq();
	bool				_ClientParcing( TCHAR* str );
	
	void				_SeparateCommandAndText(wstring const& wsStr, wstring& wsCommand, wstring& wsText);
	bool				_CheckEmotion(wstring const& wsCommand, wstring const& wsText);
	bool				_PreExcute(	wstring szStr );

private:
	void				InitContentsData(void);

public:
	ST_CHAT_OPTION*	LoadUIOption(int type, bool bLoad=true);
	void			SaveUIOption(int type, ST_CHAT_OPTION*	pThis);
	void			LoadUIContents();

private:	
#ifdef DEFAULT_IME_HANGEUL
		bool		m_bHangeul;
#endif	

private:
	NS_CHAT::TYPE	m_eChatType;
	TCHAR			m_szToWhisper[ LIMIT_IME_LENGTH ];
	TCHAR			m_szFromWhisper[ Language::pLength::name + 1 ];	

	bool			m_bShowInput;
	bool			m_bChatEnable[ NS_CHAT::MAXCOUNT ];

public:
	void			_InitData();
	void			_SetChatType( NS_CHAT::TYPE eChatType, int nStartStringIndex, TCHAR* szSubData = NULL, bool bfocus = true );
	void			_SetChatTypeToWhisper( NS_CHAT::TYPE eChatType, int nStartStringIndex, bool bfocus = true );
	void			_SetChatEnable( NS_CHAT::TYPE ct, bool bEnable );
	bool			_IsChatEnable( NS_CHAT::TYPE ct );
	bool			_ChatEnableShow(int _type, bool _megaphone=true);

	bool			_IsShowInput();	// WndProc()에서 사용
	void			_SetShowInput( bool bShow, TCHAR*	pInput = _T( ""));
	void			_SetFromWhisper( TCHAR* szFromWhisper );
	bool			_OnEnterKey();
	void			_GetUserName(int _type, TCHAR* _pName, TCHAR const* _pText);

private:
	bool			_SetText( uint nOwnerUID, TCHAR const* szText, int nType = NS_CHAT::NORMAL, int nValue = 0 );
	void			_SetText( TCHAR const* szText, int nType = NS_CHAT::NORMAL, int nValue = 0 );
	void			_SetDebug( TCHAR const* szText, int nType = NS_CHAT::NORMAL, int nValue = 0 );
	
	
private:
	void			_AddSetTextBalloon(ST_CHAT_PROTOCOL CProtocol);	//	Notify 함수에서 사용.

public:
	void			_FailedMessage(int type);

private:
	void			Chat_Process(void* pData);
	void			System_Process(void* pData);
	void			BattleLog_Process(void* pData);
	void			ItemCrop_Process(void* pData); // [4/14/2016 hyun] 아이템 획득 메세지


	// 디버깅용 Find Command Func
	void			FindCommandFunc( std::wstring const& findText );
	//================================================================================================
	// 업,다운 애로우 버튼으로. 채팅 저장.
	//================================================================================================
private:
	int				m_nCurFastIndex;
	TCHAR			m_szFastChat[ FAST_CHAT_COUNT ][ LIMIT_IME_LENGTH ];

public:
	void			_InitFastChat();
	void			_SetFastChat( TCHAR* szText );
	TCHAR*			_GetFastChat_Up();
	TCHAR*			_GetFastChat_Down();

	//================================================================================================
	// 채팅 금지
	//================================================================================================
#ifdef CHAT_BAN
private:
	u4				m_nBanTime;			// 채팅 금지 남은 시간
	u4				m_nBanEndTime;		// 채팅 금지 끝나는 시간
private:
	void			_SetChatBanTime( u4 nTime );	
#endif
private:
	bool			m_bShutOut;
	void			SetShutOut(bool bShutOut);
	bool			GetShutOut();

private:
	TOwnershipGuard<AdaptFriendShoutOut>	m_pFriendContents;
	TOwnershipGuard<AdaptPartySystem>		m_pPartyContents;
	TOwnershipGuard<AdaptCommunityContesnt> m_pCommunityContents;
};