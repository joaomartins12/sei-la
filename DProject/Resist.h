
#pragma once 

#include "DigimonTalk.h"

#include "HotKey.h"

#define SIZE_SET	( s_dwStructSize = sizeof( *this ) - sizeof( cResist::sSTRUCT_HEADER ) )
#define READ_RESIST_FILE( ClassMember, version )		{\
	    DWORD dwSize = ClassMember.s_dwStructSize;\
		fread( &##ClassMember, sizeof( cResist::sSTRUCT_HEADER ), 1, fp );\
		if( ClassMember.s_dwVersion == version )\
		{\
			fread( ( (BYTE*)(&##ClassMember ) ) + sizeof( cResist::sSTRUCT_HEADER ), ClassMember.s_dwStructSize, 1, fp );\
			ClassMember.s_dwStructSize = dwSize;\
		}\
		else\
			ClassMember.Reset();\
}

#pragma pack( push )
#pragma pack(4)

class cResist : public NiMemObject
{
public:
	cResist();
	virtual ~cResist(){ Delete(); }
	//================================================================================================
	//
	//		환경 변수
	//
	//================================================================================================
public:
	TCHAR		m_szID[ MAX_FILENAME ];			// 계정 ID
	TCHAR		m_szServer[ MAX_FILENAME ];		// 서버 이름
	TCHAR		m_szChar[ MAX_FILENAME ];		// 케릭터 이름
	bool		m_bLoadChar;					// 케릭터 로드 되었는가



public:
	struct sSTRUCT_HEADER{
		DWORD	s_dwVersion;
		DWORD	s_dwStructSize;
	};

	//================================================================================================
	//
	//		공통 정보
	//
	//================================================================================================
#define	RES_GLOBAL_PATH					"UserData\\Global.grf"
#define RES_GLOBAL_VERSION				0x0000010d

	struct sGLOBAL : public sSTRUCT_HEADER
	{
		void Reset();
		void ResetSound();
		void ResetResolution();
		bool CheckResolution();
		void ResetInterface();
		void SetSkip2ndPassword(BOOL bValue);

		void ReleaseLastConnectID(){ memset( s_szLastConnectID, 0, sizeof( TCHAR )*MAX_FILENAME ); }

		// 윈도우 위치
		int		s_nLeft;
		int		s_nTop;
		// 풀화면
		bool	s_bFullScreen;
		// 볼륨
		bool	s_bMusic;		//	배경음
		float	s_fMusic;
		bool	s_bSound;		//	환경음
		float	s_fSound;
		bool	s_bSystem;		//	효과음
		float	s_fSystem;
		// 아이디저장 체크
		bool	s_bSaveID;
		TCHAR	s_szLastConnectID[ MAX_FILENAME ];

		// 해상도
		int		s_nResolutionWidth;
		int		s_nResolutionHeight;

		// 텍스쳐 필터 디테일
		int		s_eTexFilter;

		// 그림자
		enum eSHADOW{ SHADOW_OFF, SHADOW_ON, };
		int		s_nShadowType;		

		// 안티얼라이싱
		DWORD	s_nAntialicing;

		// 케릭터 아웃라인
		bool	s_bCharOutLine;

		// 수직 동기화
		bool	s_bHSync;

		// 풀 보이기
		bool	s_bCell;

		// 케릭터 시화
		float	s_fDummy1;

		// 오브젝트 시야
		float	s_fObjectSightRate;

		// 옵션 디폴트키
		int		s_nOptionDefIndex;

		// 이름 보이기
		bool	s_bMyRenderName;
		bool	s_bOtherRenderName;
		bool	s_bMonsterRenderName;
		bool	s_bNpcRenderName;

		// 게이지
		bool	s_bViewHpBar;

		// 말풍선
		bool	s_bChatBalloone;
		bool	s_bTipBalloone;
		bool	s_bDigimonBalloone;

		// 피규어
		bool	s_bFigureTamer;
		bool	s_bFigureDigimon;
		bool	s_bFigurePat;
		
		// 언어
		nsCsFileTable::eLANGUAGE	s_eFTLanguage;

		// 마우스 조작
		bool	s_bRightHandMouse;
		// 보이스
		bool	s_bEnableVoice;

		char	s_Temp[ 2 ];

		// 확장쉐이더
		bool	s_bUseMyShader;


		//========================= 여기까지 4바이트 오더링 ====================================
		BOOL	s_bTradeActive;
		BOOL	s_bPartyActive;

		BOOL	s_bViewLevelEffect;

		BOOL	s_bUseBarcode;

		BOOL	s_bCameraEnemyCritical;

		// 날씨
		int		s_nWeather;

		// 화면 밝기
		BOOL	s_bUseBright;
		float	s_fBright;

		// 타격 이펙트
		enum eATT_EFF_MODE{ AEM_ALL_OFF, AEM_ONLY_NORATT_ON, AEM_ALL_ON, };
		eATT_EFF_MODE s_eAttackEffectViewMode;

		// 우하단 툴팁
		BOOL	s_bRightToolTip;

		// 보스출현 경고
		BOOL	s_bBossWarning;

		// 길드 마크 갱신 시간
		uint	s_nRefreshTime;

		// 포스트 이펙트
		int		s_nPostEffect;

		// 개인상점 피규어
		BOOL	s_bFigureEmployment;

		// 사운드 3D효과
		BOOL	s_bEnableSound_3DMove;

		// 결투 신청
		BOOL	s_bDualActive;
	
		// 길드 초대
		BOOL	s_bGuildInvitation;

#ifdef KEYBOARD_MOVE
		// 디지몬이동(true)/테이머이동(false)
		BOOL	s_bMoveDigimon;

		// 마우스 이동 / 키보드 이동 선택
		BOOL	s_bMove_Mouse;
		BOOL	s_bMove_KeyBoard;
#endif

		UINT	s_SkipSPWTime;
	};

	//================================================================================================
	//
	//		계정 공통 정보
	//
	//================================================================================================
#define RES_ACCOUNT_GLOBAL_VERSION			0x00000100
	struct sACCOUNT_GLOBAL : public sSTRUCT_HEADER
	{
		void Reset();

		// 마지막 접속 서버
		int		s_nLastConnetServer;

		// 서버에서 인덱스가 매번 바뀌어 져서 온다 - 최근 접속 서버를 이름으로 체크
		TCHAR	s_szServerName[ 34 ];
	};

	//================================================================================================
	//
	//		계정 서버 정보
	//
	//================================================================================================
#define RES_ACCOUNT_SERVER_VERSION			0x00000100
	struct sACCOUNT_SERVER : public sSTRUCT_HEADER
	{
		void Reset();

		// 마지막 선택 케릭터
		int		s_nLastSelChar;		
	};

	//================================================================================================
	//
	//		케릭터 단축키 정보
	//
	//================================================================================================
	DWORD dwCharMacro_Ver;
#ifdef DAMAGE_METER
#define RES_CHAR_MACRO_VERSION			0x00000103
#else // DAMAGE_METER
#define RES_CHAR_MACRO_VERSION			0x00000102
#endif //DAMAGE_METER

	struct sCHAR_HOTKEY : public sSTRUCT_HEADER		//2016-01-28-nova
	{
		// 이 이외에 다른내용은 존재하면 안된다.
		void Reset();		
		HotKey					m_HotKey;
	};

	//================================================================================================
	//
	//		디지몬 대화
	//
	//================================================================================================
#define RES_DIGIMON_TALK_VERSION			0x00000100
	struct sDIGIMON_TALK : public sSTRUCT_HEADER
	{		
		void Reset();			
		cDigimonTalk_Resist				s_DigimonTalk;
	};

	//================================================================================================
	//
	//		케릭터 정보
	//
	//================================================================================================

#define RES_CHAR_VERSION				0x00000110
	struct sCHAR : public sSTRUCT_HEADER
	{
		void Reset();		
		// 퀘스트 리스트 헬퍼 정보
		bool				s_bQuestHelperViewID[ 20 ];
		BOOL				s_bStaticCamera;
		int					s_nCameraMaxDistLevel;
		float				s_fStaticCameraRotSpeed;
	};

	//================================================================================================
	//
	//		이벤트 정보 저장
	//
	//================================================================================================
#ifdef VERSION_USA
	#define RES_GAMEEVENT_VERSION			0x00000100
#else
	#define RES_GAMEEVENT_VERSION			0x00000102
#endif
	struct sCHAR_EVENT : public sSTRUCT_HEADER
	{
		void Reset();
		void on( uint nEventIDX );
		void off( uint nEventIDX );
		bool Check( uint nEventIDX );

		DWORD s_dwEventSave[ 32 ];
	};


	//================================================================================================
	//
	//		케릭터별 퀵슬롯 정보
	//
	//================================================================================================
#define RES_CHAR_QUICK_SLOT_VERSION		0x00000105
	enum eTYPE{ NONE, ITEM, SKILL, };
	enum eID{ INVALIDE_SLOT_ID = (uint)-1, };
	struct sCHAR_QUICKSLOT : public sSTRUCT_HEADER
	{
		struct sCHAR
		{
			void Reset(){ for( int i=0; i<8; ++i ) s_QuickSlotInfo[ i ].Reset(); s_bInitial = false; }

			struct sINFO
			{
				void		Reset(){ s_eType = NONE; s_nID = INVALIDE_SLOT_ID; s_nCount = 0; }
				eTYPE		s_eType;				
				uint		s_nID;			// 아이템이라면 파일 테이블 ID, 스킬이라면 스킬인덱스
				uint		s_nCount;		// 아이템이라면 아이템 갯수
			};
			sINFO	s_QuickSlotInfo[ 8 ];	// nLimit::QuickSlot
			bool	s_bInitial;
		};
		
		void Reset();
		sCHAR s_QuickSlotChar[ 20 ];		// nLimit::EvoUnit		-	0번째 사용하지 아니함
	};	

	//================================================================================================
	//
	//		테이머별 센터바 정보
	//
	//================================================================================================
#define RES_CHAR_CENTERBAR_VERSION		0x00000101
	struct sCHAR_CENTERBAR : public sSTRUCT_HEADER
	{		
		void Reset();

		struct sINFO
		{
			void		Reset(){ s_eType = NONE; s_nID = INVALIDE_SLOT_ID; s_nCount = 0; }
			eTYPE		s_eType;				
			uint		s_nID;			// 아이템이라면 파일 테이블 ID, 스킬이라면 스킬인덱스
			uint		s_nCount;		// 아이템이라면 아이템 갯수
		};

		sINFO	s_CenterBarInfo[ 12 ];	// nLimit::CenterBar
		bool	s_bInitial;
		float	s_fAlpha;	
		void	SetQuickSlotOpenClose(bool bValue);
		bool	s_bIsOpenQuick;		// 퀵슬롯 오픈 유무
	};	

public:
	sGLOBAL				m_Global;
public:
	bool				IsFullScreen(){ return m_Global.s_bFullScreen; }
	void				SetFullScreen( bool bFullScreen ){ m_Global.s_bFullScreen = bFullScreen; }


	float				GetMusic(){ return m_Global.s_fMusic; }
	float				GetSound(){ return m_Global.s_fSound; }
	float				GetSystemSound(){ return m_Global.s_fSystem; }

	bool				IsMusic(){ return m_Global.s_bMusic; }
	bool				IsSound(){ return m_Global.s_bSound; }
	bool				IsSystemSound(){ return m_Global.s_bSystem; }
	void				SetEnableMusic( bool bEnable ){ m_Global.s_bMusic = bEnable; }
	void				SetEnableSound( bool bEnable ){ m_Global.s_bSound = bEnable; }
	void				SetEnableSystemSound( bool bEnable ){ m_Global.s_bSystem = bEnable; }

	void				SetMusic( float fVolume ){ m_Global.s_fMusic = CsMax( 0.0f, CsMin( 1.0f, fVolume ) ); }
	void				SetSound( float fVolume ){ m_Global.s_fSound = CsMax( 0.0f, CsMin( 1.0f, fVolume ) ); }
	void				SetSystemSound( float fVolume ){ m_Global.s_fSystem = CsMax( 0.0f, CsMin( 1.0f, fVolume ) ); }

#ifdef SIMPLEFICATION
	void				SetFigureTamer( bool bEnable ){ m_Global.s_bFigureTamer = bEnable; }
	void				SetFigureDigimon( bool bEnable ){ m_Global.s_bFigureDigimon = bEnable; }
	void				SetFigurePStore( bool bEnable ){ m_Global.s_bFigureEmployment = bEnable; }

	void				SetFigurePat( bool bEnable ){ m_Global.s_bFigurePat = bEnable; }

	bool				GetFigure() { return m_Global.s_bFigureTamer || m_Global.s_bFigureDigimon || m_Global.s_bFigureEmployment /*|| m_Global.s_bFigurePat*/; }
#endif


public:
	sACCOUNT_GLOBAL		m_AccountGlobal;
	sACCOUNT_SERVER		m_AccountServer;
	sCHAR				m_Character;
	sCHAR_EVENT			m_CharEvent;
	sCHAR_CENTERBAR		m_CenterBar;

	sCHAR_HOTKEY		mCharHotKey;	//2016-01-28-nova	단축키정보 저장

	sDIGIMON_TALK		m_Talk;			

protected:
	int					m_nDeleteCharIndex;
	//bool				m_bTutorial;
	bool				m_bEnablePortal;	// 로딩 나왔을경우에만 포탈 이동 가능하게 처리하겠다
	bool				bMovePortal;		//2017-04-12-nova 포탈이동시 키보드/마우스 입력처리

public:
// 	bool				IsTutorial(){ return m_bTutorial; }
// 	void				SetTutorial( bool bTutorial ){ m_bTutorial = bTutorial; }
	bool				IsEnablePortal(){ return m_bEnablePortal; }
	void				SetEnablePortal( bool bEnablePortal ){ m_bEnablePortal = bEnablePortal; }

	bool				IsMovePortal() { return bMovePortal; }	//2017-04-12-nova
	void				SetMovePortal(bool bPortal);			//2017-04-12-nova
	
	int					GetDeleteCharIndex(){ return m_nDeleteCharIndex; }
	void				SetDeleteCharIndex( int nIndex ){ m_nDeleteCharIndex = nIndex; }

public:
	void				CharDelete( TCHAR const* szChar );

public:
	void		Delete();
	void		Init();
protected:
	void		_LoadGlobal();
public:
	void		LoadAccountGlobal( TCHAR const* szID );
	void		LoadAccountServer( TCHAR const* szServer );
	void		LoadChar( TCHAR const* szChar );	
	void		LoadGameEvent();
	void		LoadDigimonTalk( TCHAR const* szDigiName );	
	void		LoadCenterBar( TCHAR const* szCharName );
	sCHAR_QUICKSLOT*		LoadCharQuickSlot( TCHAR const* szCharName );	
	void					SetCharQuickSlot( TCHAR const* szCharName, sCHAR_QUICKSLOT* pQuickSlot );
	void		CheckTalk();

public:
	void		SaveGlobal();
	void		SaveAccountGlobal();
	void		SaveAccountServer();
	void		SaveChar();
	void		SaveCharQuickSlot();
	void		SaveCenterBar();
	void		SaveGameEvent();
	void		SaveDigimonTalk( sDIGIMON_TALK sTalk, TCHAR* szDigiName );

	BOOL		IsStaticCamera(){ return m_Character.s_bStaticCamera; }

protected:
	NiTStringPointerMap< sCHAR_QUICKSLOT* >*		m_pmapQuickSlot;	
};

#pragma pack( pop )

extern cResist*			g_pResist;