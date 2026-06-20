#pragma once

//////////////////////////////////////////////////////////////////////////
#define	LEVEL_RENDER_COUNT	4	// 렌더할 기본 줄수
#define LINE_EXPAND			3	// 늘어날 줄수
#define LINE_HEIGHT			17	// 한줄당 늘어난 높이.

#define CHAT_SIZE_WIDTH		388	// 채팅창의 1단계의 넓이
#define CHAT_SIZE_HEIGHT	(LEVEL_RENDER_COUNT*LINE_HEIGHT + LINE_HEIGHT)	// 채팅창의 1단계의 높이

#define MAX_LEVEL_SIZE	7

class ChatBase : public cBaseWindow, public ChatContents::ObserverType
{

public:
	ChatBase(void);
	virtual ~ChatBase(void);

public:	// 확대를 위한 조절 변수값
	struct	stExpandData
	{
		stExpandData();
		~stExpandData();
		void			reset(int level=0);
		void			AddLevel();			// 높이 레벨 값 증가
		int				GetLevel() const;
		int				GetHeight(int level=-1) const;	// 현재 확대 레벨에 맞는 높이
		int				GetDefaultHeight()const;	// 현재 확대 레벨과 상관없이 한폭의 높이
		int				GetLineCnt();		// 현재 표시할 줄수
		int				nCurLevel;			// 현재 확대 레벨 0~6까지 7단계; MAX_LEVEL_SIZE
	};
	stExpandData		m_Expand;
////////////							COMMON				//////////////////
protected:
	virtual	bool			Construct(void);
	virtual	void			Notify(int const& iNotifiedEvt, ContentsStream const& kStream);

	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			Create( int nValue = 0 );
	virtual bool			Close( bool bSound /* = true  */ );
	virtual void			ResetDevice();	
	virtual void			ResetMap();
	virtual void			Update(float const& fDeltaTime);	
	virtual void			Render();
	virtual	void			StringListRender();	// 메인에서는 다르게 그려서 따로 만듬.
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			SetAlpha(float alpha);
	virtual void			ScrollBarUpdate(int SclrollSize){}

// event 함수
protected:	
virtual	void			_ChageBGSize( void* pSender, void* pData );	// 확대를 위한 조절 함수
virtual	void			_ChageBGUpdate( void* pSender, void* pData );	// 확대를 위한 조절 함수
virtual	void			_MoveUi( void* pSender, void* pData );			// 이동을 위한 함수

public:
	int						SizelevelHeight(int sizeHegiht) const;

protected:
	cStringList				m_String;
	cScrollBar*				m_pScrollBar;			// 스크롤바
	cPopUp*					m_pChatBG;				// 뒤배경
	cButton*				m_pLastScrollButton;	// 마지막줄 보기
	cButton*				m_pMoveBtn;				// 전체 이동
	cButton*				m_pChatBGSizeButton;	// 채팅창 확대

	int						m_ClassValue;			// 클래스명
	int						m_nCurIdx;	
	int						m_MovePosY;				// 스트링 이동할 값
	CsPoint					m_StringPos;			// 스트링 시작위치

	bool					m_OutWindow;			// 윈도우창에서 클릭이 될지를 결정
	bool					m_ShowAddWindow;		// 추가로 보여줄 창이 있는지.
protected:
	void					SetMovePosY(int y);	
	void					SetOutWindow(bool w);
	void					LastScrollClick();

public:
	void					SetStringPos(CsPoint s);
	void					SetExpandLevel(int	level);	
	int						GetExpandLevel() const;
	CsPoint					GetStringPos() const;

public:
	void					SetAddShowOn(bool	bon);
	bool					GetAddShowOn() const;
	void					CheckExecption();		// 예외 처리

protected:
	ST_CHAT_OPTION			m_Chat_Option;

protected:
	void					LoadUIOption(int type);
	void					SaveUIOption(int type);
};
