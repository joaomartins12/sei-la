
#pragma once 

class cBaseWindow : public cScriptUI
{
public:
	enum eWINDOW_TYPE{
		WT_MESSAGE_BOX,
		WT_MAIN_BAR,
		WT_PLAYTIME,
		WT_TAMER_WINDOW,
		WT_MAIN_DIGIMON_WINDOW,
		WT_TARGET_WINDOW,
		WT_TACTICS_WINDOW,
		WT_XAIGUAGE_WINDOW,
		WT_QUICKSLOT,
		WT_QUICKEVOL,
		WT_CENTERBAR,
		WT_BATTLE_EVOL,
		WT_CHATOPTION,
		WT_MINIMAP,
		WT_ZONEMAP,
		WT_DIGIVICEQUEST,
		WT_PARTY_WINDOW,
		WT_BUFFINFO,
		WT_LOOTING,
		WT_MYLOOTING,

		WT_INVENTORY		=	50,
		WT_PCBANGINVEN,
		WT_TALK,
		WT_STORE,
		WT_PERSONSTORE,
		WT_DIGITAMA_SELL,
		WT_DIGITAMA_SCAN,
		WT_QUEST_REV,
		WT_QUEST_COMP,
		WT_QUEST_LIST,
		WT_QUEST_DISCRIPT,
		WT_QUEST_HELPER,
		WT_MAKE_TACTICS,
		WT_MAKE_CROSSTACTICS,
		WT_ENCHANT_TACTICS,

		WT_MAINOPTION,
		WT_OPTION_SOUND,
		WT_OPTION_MACRO,
		WT_OPTION_GRAPHIC,
		WT_OPTION_INTERFACE,
		WT_OPTION_USERGRADE,
		WT_ELEMENTITEM_CHECK,
		WT_ELEMENTITEM_ATTACH,
		WT_WAREHOUSE,
		WT_UNIONSTORE,
		WT_TRADE,
		WT_NPCFIND,		
		WT_COMMUNITY,
		WT_GUILDSET,
		WT_GUILDRANK_VIEW,
		//WT_CASH,
		//WT_CASHSHOP,
		WT_CASHSHOP_BUTTON,
		WT_TACTICSHOUSE,
		WT_BGSPRITE,
		WT_JUMPBUSTER,
		WT_CHANGETAMER,

		WT_TUTORIAL,
		WT_TUTORIAL_TRACE,
		WT_SEPARATE,
		WT_MONEY_SEPARATE,
		WT_ITEM_SEPARATE,
		WT_CREATE_NAME,
		WT_PUBLICITY,
		WT_CHANNEL,
		WT_HELPER,
		WT_SMALLMAP,
		WT_WEBWIN,
		WT_MAIL,
		WT_MAIL_BUTTON,
		WT_LETTER,
		WT_DIGIMON_TALK,
		WT_PROTECT,
		WT_ACHIEVE,
		WT_DIVISION,		
		WT_SHOUT_CASH,
		//친구초대 이벤트
		WT_RECOMMENDEVENT,
		WT_RECOMMENDEVENT_BUTTON,
		// 포인터 참조를 위한 맥스 갯수
		WT_MAXCOUNT,

		// 인터페이스는 없지만 외부적으로 쓰여서
		WT_PORTAL_ENABLE,
		WT_PORTAL_DISABLE,
		WT_BARCODE_CARD_SCRATCH,
		WT_CHANGE_D_NAME,
		WT_CHANGE_T_NAME,
		WT_CREATE_GUILD_NAME,
		WT_INVITE_GUILD_NAME,
		WT_FRIEND_MEMO,
		WT_SHUTOUT_MEMO,
		WT_GUILDRANK_NAME,
		WT_GUILDRANK_EXPLAIN,
		WT_UPGRADE_GUILD,
		WT_CAPSULE_MACHINE,
		WT_RECOMMEND_NAME, //친구추천 이벤트
		WT_MINIGAME,
		WT_CARD_EVENT,	// 129  // 카드 이벤트 윈도우 타입 추가
		WT_RAIDRANK,
		WT_CHANGE_ADVENTURE_TAMER,
		WT_SIMPLE_BTN,

		WT_TAMER_SUMMON,
		WT_REWARD,
		WT_MASTERS_MATCHING,

		WT_DIGIMON_INCUBATOR,
		WT_CARDINVENTORY,
		WT_CARDIDENTIFY,
		WT_CARDRESEAL,
		WT_ENCYCLOPEDIA,
		WT_ENCYCLOPEDIA_STATS,
		WT_CAPSULE_EVOLUTION,
		WT_FRIEND_LIST,			// 친구 UI
		WT_GUILD_MEMO,			// 커뮤니티 - 길드 정보 중 메모 UI

		WT_SPIRIT_EVOLUTION,		// 스피릿 진화
		WT_DAMAGE_METER,		// 데미지 미터기
		WT_EVENT_PINOKIMON,		// 2014 겨울(크리스마스)이벤트 - 피노키몬의 카드게임
		WT_ITEM_PRODUCTION_SHOP,	// 아이템 제작 샵

		WT_CHAT_EDIT_WINDOW,
		WT_CHAT_MAIN_WINDOW,
		WT_CHAT_SYSTEM_WINDOW,
		WT_CHAT_BATTLELOG_WINDOW,

		WT_DATA_TRADE_SELECT,			// 데이터 교환 선택창.
		WT_DATA_TRADE_DIGIMON_SCALE,	// 데이터 교환 : 디지몬 크기
		WT_DATA_TRADE_DIGIMON_UPGRADE,	// 데이터 교환 : 디지몬 강화
		WT_DATA_TRADE_DIGIMON_EVOLUTION,// 데이터 교환 : 디지몬 진화

		WT_EXP_EVENT_UI,
		WT_DAILY_EVENT_UI,
		WT_REWARD_EVENT_UI,				//2017-02-16-nova	접속보상시스템

		//* 배틀 관련 UI */
		WT_BATTLE_MATCHING_SYSTEM,			// 사용 안함.
		WT_BATTLE_MATCHING_WAIT_BUTTON,		// 배틀 신청 중에 띄우는 버튼
		WT_BATTLE_MATCHING_WAIT_WINDOW,		// 배틀 참가 대기중
		WT_BATTLEROUND_TIMER,
		WT_BATTLESTART_TIMER,
		WT_BATTLESTART_END,
		//* */

		// 무한대전 관련 UI 
		WT_INFINITEWAR_START_WINDOW,
		WT_INFINITEWAR_GIVEUP_WINDOW,
		WT_INFINITEWAR_NOTICEBOARD_WINDOW,	// 무한대전 포인트 적립 윈도우
		WT_INFINITEWAR_NEXTSTAGE_WINDOW,	// 무한대전 다음 스테이지 이동 창
		WT_INFINITEWAR_ALLCLEAR_WINDOW,		// 무한대전 모든 스테이지 클리어 창
		WT_INFINITEWAR_START_TIMER_WINDOW,	// 무한대전 스테이지 시작 카운트 창
		WT_INFINITEWAR_PRERANKLIST_WINDOW,	// 무한대전 지난 랭킹 보기 윈도우
		WT_INFINITEWAR_RANKREWARD_WINDOW,	// 무한대전 랭킹별보상 윈도우
		//

		WT_NEW_CASHSHOP,
		WT_NEW_CASH_WAREHOUSE,

		WT_VIP_BUTTON,		// MEMBERSHIP BUTTON

		WT_ACTIONKEY,
		WT_GIFTBOX,
		WT_NEW_TAMERSTATUS,
		WT_DELETEDIGIMON,
		WT_NEW_DIGIMONSTATUS,
		WT_DIGIMONSTATUS_HELP,
		WT_TRANSCENDENCE_HELP,
		WT_NEW_GOTCHA_MACHINE,
		WT_EQUIPMENT_SYSTEM,
		WT_EQUIPMENT_SYSTEM_HELP,
		WT_ENCHANT_OPTION,

		//
		WT_TUTORIAL_HELP_UI = 181,
		WT_TUTORIAL_SKIP_UI,
		WT_SCREENOUT_UI,
		
		WT_NOTICE_BUTTON,			// NOTICE BUTTON
		WT_NOTICE_WINDOW,			// NOTICE WINDOW
		WT_SEASONPASS_BUTTON,		// SEASONPASS BUTTON
		WT_SEASONPASS_WINDOW,		// SEASONPASS WINDOW

		WT_NONE,
	};

	// Mouse Update Type
	enum eMU_TYPE{
		MUT_OUT_WINDOW,		
		MUT_NONE,
		MUT_CLICK_FOCUS,
	};

public:
	static void				DestroyWindow( cBaseWindow* pWindow );
	static cBaseWindow*		NewWindow( cBaseWindow::eWINDOW_TYPE wt, int nPointerIndex = 0 );
	
	template <typename T>
	static cBaseWindow* CreateNewWindow( const char *strWindowName, int const& nIdx )
	{
		cBaseWindow* pReturn = NiNew T;				
		pReturn->SetWindowName( strWindowName );
		pReturn->Construct();
		pReturn->__SetPointerIndex( nIdx );
		return pReturn;
	}

public:
	cBaseWindow();
	cBaseWindow(char* pWindowName);
	virtual ~cBaseWindow() {};
	

private:
	bool					m_bInitialize;
	bool					m_bShowWindow;
	bool					m_bStaticWindow;	

	int						m_nPointerIndex;					// 지울때 필요 - 다른데서는 쓰지말자

protected:
	bool					m_bMouseInWindow;					// 마우스가 윈도우 안에 존재

public:
	virtual	bool			Construct();						//처음 로드됨 - UI가 연결되기 전에 한번 호출하기로 하자.//오버라이드 하지 않으면 항상 성공이다.
	bool					IsLive();
	virtual bool			IsShowWindow();
	void					SetShowWindow( bool bShow );
	void					SetStaticWindow( bool bStatic );
	bool					IsStaticWindow();
	void					__SetPointerIndex( int nPointerIndex );
	int						__GetPointerIndex();

public:
	virtual eWINDOW_TYPE	GetWindowType() = 0;
	virtual void			Destroy();						// 마지막 한번만 호출
	virtual void			DeleteResource() = 0;
	virtual void			Create( int nValue = 0 ) = 0;	// 초기에 한번만 호출

	virtual void			UserDie();
	virtual void			RecvDiedAlready();

	virtual void			Open( int nValue = 0, bool bSound = true );
	virtual bool			Close( bool bSound = true );
	virtual void			PlayOpenSound();
	virtual void			PlayCloseSound();

	void					Delete();
	bool					Init();

	virtual void			Update(float const& fDeltaTime) = 0;	
	virtual eMU_TYPE		Update_ForMouse();
	virtual eMU_TYPE		Update_ForMouse_Disable(){ return cBaseWindow::Update_ForMouse(); }
	virtual void			Render() = 0;

	virtual void			ResetDevice() = 0;
	virtual void			ResetMap(){}
	virtual void			PreResetMap(){}

	virtual void			OnLButtonUp( CsPoint pos ){}
	virtual void			OnRButtonUp( CsPoint pos ){}
	virtual void			OnLButtonDown( CsPoint pos ){}
	virtual bool			CursorIconLBtnUp( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount ){ return false; }
	virtual bool			CursorIconLBtnDown( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount ){ return false; }

	virtual void			OnSetFocus(){ _OnSetFocusScript(); }
	virtual void			OnKillFocus(){ _OnKillFocusScript(); }

	virtual bool			OnMacroKey(){ return false; }
	virtual bool			OnMacroKey(const MSG& p_kMsg) { return false; } //2016-01-18-nova 윈도우Msg로 처리
	virtual bool			OnEscapeKey(){ return false; }
	virtual bool			OnEnterKey(){ return false; }
	virtual bool			OnUpKey(){ return false; }
	virtual bool			OnDownKey(){ return false; }
	virtual bool			OnRightKey(){ return false; }
	virtual bool			OnLeftKey(){ return false; }	

	virtual void			ChangeMacroKey(){}

	virtual void			OnMoveWindow(){ _OnMoveWindowScript(); }


	const char*				GetWindowName() const;
	void					SetWindowName( const char* pWinName );
	bool					IsSameWindow( const char* pWinName ) const;
	//=====================================================================================================
	//
	//	무브 윈도우
	//
	//=====================================================================================================
protected:
	bool					m_bActiveMoveWindow;
	CsPoint					m_ptBackupMousePos;

private:
	std::string				m_strWindowName;
	bool					m_bHaveNotFocus;// 포커스를 가질 수 없는 윈도우 셋팅
	bool					m_bAlwayTop;

protected:
	virtual CsPoint			_UpdateMoveWindow();
	void					_UpdateMoveWindow_ForMouse();
	
public:
	void					SetHaveNotFocus(bool bValue)	{ m_bHaveNotFocus = bValue; }
	bool					IsHaveFocus() const				{ return !m_bHaveNotFocus;	}
	void					SetAlwaysTop( bool bValue )		{ m_bAlwayTop = bValue;		}
	bool					IsAlwaysTop() const				{ return m_bAlwayTop;		}
};

