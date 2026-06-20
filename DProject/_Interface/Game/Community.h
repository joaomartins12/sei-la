#pragma once 
#include "../00.Contents/FriendListContents.h"
#include "../00.Contents/CommunityContent.h"

#define ONLINE_COLOR	FONT_WHITE
#define OFFLINE_COLOR	NiColor( 128.0f / 255.0f, 128.0f / 255.0f, 128.0f / 255.0f )
#define STRLIST_EMPTY	-1

class cCommunity : public cBaseWindow, public cCommunityContent::ObserverType
{
public:
	cCommunity();
	virtual ~cCommunity();
#ifdef GUILD_RENEWAL
	enum eTAB { TAB_GUILD = 0, TAB_GUILDSKILL, TAB_HISTROY, TAB_GUILDADMIN,};	// 친구,차단 탭 삭제 & 길드스킬 탭 추가
#else
	enum eTAB { TAB_GUILD = 0, TAB_HISTROY, TAB_GUILDADMIN,};
#endif
	enum eADMIN { ADMIN_INVITE = 0, ADMIN_LEAVE, ADMIN_DESTROY, ADMIN_SET, ADMIN_VIEW, ADMIN_INFO };
	enum eGUILD_COMBO { INFO = 0, ACTIVE, RANK };

public:
	virtual eWINDOW_TYPE	GetWindowType();
	virtual void			Destroy();
	virtual void			DeleteResource();
	virtual void			ResetDevice();
	virtual void			Create( int nValue = 0 );
	
	virtual	bool			Construct(void);
	virtual void			Update(float const& fDeltaTime);
	virtual eMU_TYPE		Update_ForMouse();
	virtual void			Render();

	virtual void			ResetMap();
	virtual void			PreResetMap(){ Close( false ); }
	virtual bool			OnEscapeKey();
	virtual bool			OnEnterKey();
	virtual void			OnMoveWindow();

	virtual void			OnRButtonUp( CsPoint pos );

public:
	eTAB					GetTab() { return m_eRadioIndex; }	
	void					SetTab( eTAB nIDX );
	int						GetCurSelect() { return m_nRealDataIdx; }	// 현재 지정한 테이머 이름
	void					SetNotice( TCHAR const* szNotice );
	void					SetNoticeBtn( bool bUse ) { m_pNotice->SetEnable( bUse ); }

public:	
	void					_DeletePbButton();
	void					_DeletePbText();	
	void					_SetEditBox( int nLineNum );			

	// 길드 정보	
	int						DeletePerson( int nDataIdx );
#ifdef GUILD_RENEWAL
	void					_AddGuildPerson( cCommunityContent::sGUILD_PERSON_INFO* pInfo, int nRealDataIdx );	// 길드 인원 추가
#else
	void					AddGuildPerson( cCommunityContent::sGUILD_PERSON_INFO* pInfo, int nRealDataIdx );
#endif
	bool					MakeGuildInfo();
	
	void					_MakeGuildPersonInfo();	
	void					_RenderGuildInfo();
	cButton::eACTION		_UpdateMouseGuild();
	void					_GuildPersonPopup( int nDataIdx );

	// 히스토리
	void					RefreshHistory();
	void					_MakeHistory();

	// 길드 어드민
	void					_MakeGuildAdmin();
	cButton::eACTION		_UpdateMouseGuildAdmin();
	void					_CheckAdmin();


#ifdef GUILD_RENEWAL
	// 길드 스킬
	void					_MakeGuildSkill();				// 길드 스킬 UI&정보 Init
	cButton::eACTION		_UpdateMouseGuildSkill();		// 길드 스킬 마우스 업데이트
	void					_CheckGuildSkill( int nDataIdx ) { m_nRealDataIdx = nDataIdx; }
	void					_RenderGuildSkill();			// 길드 스킬 랜더링
	bool					_CheckUseState();				// 스킬 사용 가능 상태 체크
	void					_UseBaseSkill( int nIdx );		// Base 스킬 사용 동작
	void					_UseActSkill( int nY, int nX );	// Active 스킬 사용 동작
	void					_SetGuildPersonCnt();			// 길드 최대 인원 변경 셋팅
#endif
private:
	virtual	void	Notify(int const& iNotifiedEvt, ContentsStream const& kStream);
// 	bool					IsOnOff();
// 	void					SetOnOff(bool bOnOff);


	// 메모리잡힌 컨트롤
protected:
	cSprite*				m_pMouseOnMask;	
	cSprite*				m_pNoticeMask;
	cSprite*				m_pGuildMark;
	cStringList				m_strPersonInfo;	
	cStringList				m_strNotice;
	cRadioButton*			m_pRadioBtn;

	std::vector< cButton* >	m_pbButton;
	std::vector< cText* >	m_pbText;
	
	cText*					m_pGuildPersonNum;
	cEditBox*				m_pEditName;	

	CsTimeSeq				m_Time;
	cComboBox*				m_pCombo;

#ifdef GUILD_RENEWAL

#define IF_BASESKILL_COUNT				5
#define IF_ACTIVESKILL_COUNT			12
#define IF_ACTIVESKILL_ROW				3
#define IF_ACTIVESKILL_COL				4
	cText*				m_pGSPoint;			// 길드 스킬 포인트 텍스트
	cText*				m_pOnOffText;		// 오프라인 보기 텍스트

	cStringList			m_strGuildSkill;	// Active 스킬 이름 리스트

	cSprite*			m_pSkillNameBG;		// 스킬 이름 BG
	cSprite*			m_pLine;			// Base 스킬 / Active 스킬 구분선
	cScrollBar*			m_pScrollBar_S;		// 길드 스킬 스크롤바( Active 스킬만 스크롤 하기 위해 사이즈/범위 조정 )

	cSprite*			m_pIconBG;			// 길드 스킬 아이콘 BG
	sIFIcon				m_BaseSkillIcon[ IF_BASESKILL_COUNT ];
	cText*				m_pReqLvText[ IF_BASESKILL_COUNT ];		// 길드 Base스킬 요구레벨 텍스트
	CsSkill*			m_pBaseSkill[ IF_BASESKILL_COUNT ];		// 길드 Base스킬  

	cSprite*			m_pOnMask;			// 사용 중인 Active 스킬 아이콘 마스크
	NiColor				m_pOnMaskColor;		// 온 마스크 색
	bool				m_bPlusColor;		// RGB 값 증/감 플래그
	sIFIcon				m_ActSkillIcon[ IF_ACTIVESKILL_COUNT ];	// Active 스킬 아이콘 위치
#endif

	//cScript 사용
protected:
	cButton*		m_pCancelButton;	//닫기	
	cButton*		m_pNotice;
	cCheckBox*		m_pOnOffCheck;		
	
	cScrollBar*		m_pScrollBar;			//스크롤

	//포인터만 존재
protected:
	cText*			m_pTextNotice;

	//내부 필요변수들
protected:
	bool			m_bSort;	
	bool			m_bNotice;
	eTAB			m_eRadioIndex;	

	CsPoint			m_ptSelMarkPos;
	bool			m_bClickMask;
	int				m_nMouseOnMaskIndex;	// 보이는 인원정보 선택	
	int				m_nRealDataIdx;			// 실제 정보에서 선택
	
	TCHAR			m_szTempNotice[ MAX_PATH ];
	//bool			m_bOnOff;

	//cScriptUI		m_GuildMember;
	//cScriptUI		m_GuildSkill;
	//cScriptUI		m_GuildHistory;
	//cScriptUI		m_GuildManegement;
};