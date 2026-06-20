#pragma once

#define MAX_CHAT					90			// 가지고 있는 채팅 줄 갯수
#define LIMIT_IME_LENGTH			70			// IME 문자 제한
#define FAST_CHAT_COUNT				20

class ChatEditWindow : public cBaseWindow
{
public:
	ChatEditWindow(void);
	virtual ~ChatEditWindow(void);

	////////////							COMMON				//////////////////
public:
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );
	virtual bool			Close( bool bSound /* = true  */ );
	virtual void			ResetDevice();
	virtual void			ResetMap();
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			OnSetFocus();
	virtual void			OnKillFocus();
public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CHAT_EDIT_WINDOW; }

	////////////						UNCOMMON				//////////////////

public:
	void					_SetShow(bool bShow);
	void					_SetTypePos(CsPoint ePos);
	void					_SetTypeText(TCHAR* pText);
	void					_SetTypeColor(NiColor FontColor);
	void					_SetInputBGPos(CsPoint bPos);
	CsRect					_GetChatTypePos(CsPoint	tPos);
	cText*					_GetEditTypeText();
	CsPoint					_GetTypePos();
	NiColor					_GetTypeColor();
	void					_MoveWindow(bool	bMove);
	void					InitWindow(cWindow*	pWindow);
protected:
	cText*					m_pEditTypeText;		// 입력창 타입 표시 텍스트.
	cSprite*				m_pInputBGSprite[ 3 ];	// 입력창 배경
};

class ChatMainWindow : public ChatBase
{
public:
	ChatMainWindow(void);
	virtual ~ChatMainWindow(void);

private:
	enum eMEGAPHONE_TYPE	{	//	확성기 타입 : 확성기 표시할때의 스프라이트 종류를 결정한다.
		MEGAPHONE_NOR=0,		//	일반형
		MEGAPHONE_SPE,		//	고급형
		MEGAPHONE_NOR_TIME,	//	일반형 기간제
		MEGAPHONE_SPE_TIME,	//	고급형 기간제
#ifdef GUILD_RENEWAL
		MEGAPHONE_GUILD,			// 길드 확성기
#endif
		MEGAPHONE_GM,		// GM 전용 확성기
		MAX_MEGAPHONE,
	};

////////////							COMMON				//////////////////
public:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();	
	virtual void			ResetMap();
	virtual	void			PreResetMap();
	virtual bool			Close( bool bSound /* = true  */ );
	virtual void			Create( int nValue = 0 );
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();
	virtual	void			StringListRender();
	virtual void			OnLButtonUp( CsPoint pos );
	virtual void			ScrollBarUpdate(int SclrollSize);
	virtual void			OnSetFocus();
	virtual void			OnKillFocus();

	
	// event 함수
protected:	
	virtual	void			_ChageBGSize( void* sender, void* pData );	// 확대를 위한 함수
	virtual	void			_ChageBGUpdate(void* sender, void* pData );	// 확대를 위한 조절 함수
	virtual	void			_MoveUi( void* sender, void* pData );			// 이동을 위한 함수

////////////						UNCOMMON				//////////////////

	struct stMegaphonSprite
	{
		stMegaphonSprite():m_pIcon(NULL), m_pBack(NULL)	{}
		~stMegaphonSprite() {SAFE_NIDELETE( m_pIcon );SAFE_NIDELETE( m_pBack );}
		void	Allocate();
		cSprite*		m_pIcon;
		cSprite*		m_pBack;
	};

public:
	virtual eWINDOW_TYPE	GetWindowType(){ return WT_CHAT_MAIN_WINDOW; }

	// 포인터만 존재 하는 컨트롤
private:
	cRadioButton*	m_pChatTypeRadio;		// 채팅 타입 버튼
	cCheckBox*		m_pChatOptionCheck;		// 채팅 옵션
	cButton*		m_pEmotion;				// 동작
	cCheckBox*		m_pBattleLog;			// 전투 정보

private:
	stMegaphonSprite	m_ShoutSpr[MAX_MEGAPHONE];	// 메가폰 스프라이트
	ChatEditWindow*		m_EditWindow;				// 채팅 입력 에디트 윈도우

public:
	virtual bool	OnUpKey();
	virtual bool	OnDownKey();
	virtual bool	OnEscapeKey();
	virtual bool	OnEnterKey();

	void			InputCommandProcess();	//2016-02-04-nova /g + space 사용시
	
	LIST_STRING*	_GetStringList(){ return m_String.GetList(); }

public:								// cChatOption 에서 사용
	CsPoint			_GetOptionButtonPos(){ return m_pChatOptionCheck->GetClient(); }

private:
	void			_InitData();
	void			_OptionCheck( void* pSender, void *pData );	// 채팅 옵션 관리 함수

private:	
	void			_SetEmotion( int nID );		// 동작	

public:		
	void			_InputFocusUpdate();
	bool			_IsShowInput();				// WndProc()에서 사용

private:

	void			_AddListStringHead(LIST_STRING* _pList, int _type, int _num, TCHAR* _szName, cText::sTEXTINFO* _TextInfo);
	bool			_CheckWhisper(int _type, int _num,TCHAR*	_szName);
	int				_GetMegaphonType(int _type);

private:
	void			_ResetScroll();
	void			_ResetString();
	void			_ChatTypeUpdate(CsPoint	tPos);
	//================================================================================================
	// 길드 확성기
	//================================================================================================
private:
		int				m_nCurColor;			// 1: 빨 2: 주 3: 노 4: 초 5:파 6:남 7:보
		NiColor			_ChangeColor( NiColor pCurColor, float fR, float fG, float fB );
		void			_GuildColorUpdate();
};