#pragma once
#include "afxwin.h"
#include "afxcmn.h"

// COption 대화 상자입니다.

class COption : public CDialog
{
	DECLARE_DYNAMIC(COption)

public:
	COption(std::string const& strGamePath, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~COption();

// 대화 상자 데이터입니다.
	enum { 
#if defined TW_VERSION		// 대만
		IDD = IDD_OPTION_TW
#elif defined HK_VERSION	// 홍콩
		IDD = IDD_OPTION_HK
#elif defined TH_VERSION	// 태국
		IDD = IDD_OPTION_TH
#else						// 북미/Steam
		IDD = IDD_OPTION
#endif
	};

//=============================================================================
// Resist의 Global 저장 함수 가져옴..  
//=============================================================================
public:
	struct sSTRUCT_HEADER{
		DWORD	s_dwVersion;
		DWORD	s_dwStructSize;
	};

#define	RES_GLOBAL_PATH					"UserData\\Global.grf"
#define RES_GLOBAL_VERSION				0x0000010d

	struct sGLOBAL : public sSTRUCT_HEADER
	{
		void Reset();
		void ResetSound();
		void ResetResolution();
		//bool CheckResolution();
		void ResetInterface();

		void ReleaseLastConnectID(){ memset( s_szLastConnectID, 0, sizeof( TCHAR )*MAX_FILENAME ); }

		// 윈도우 위치
		int		s_nLeft;
		int		s_nTop;
		// 풀화면
		bool	s_bFullScreen;
		// 볼륨
		bool	s_bMusic;
		float	s_fMusic;
		bool	s_bSound;
		float	s_fSound;
		bool	s_bSystem;
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
		//nsCsFileTable::eLANGUAGE	s_eFTLanguage;
		int		s_eFTLanguage;

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
		unsigned int	s_nRefreshTime;

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

		// 디지몬이동(true)/테이머이동(false)
		BOOL	s_bMoveDigimon;

		// 마우스 이동 / 키보드 이동 선택
		BOOL	s_bMove_Mouse;
		BOOL	s_bMove_KeyBoard;

		UINT	s_SkipSPWTime;
	};

	sGLOBAL		m_Global;
//=============================================================================
// Resist Global 옵션 끝
//=============================================================================
	
	struct sResolution
	{
		int nWidth;
		int nHeight;
	};

	std::list<sResolution> m_ResolutionData;
	
	//sResolution m_ResolutionArry[12];

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio7();
	afx_msg void OnBnClickedRadio8();
	afx_msg void OnBnClickedRadio9();
	afx_msg void OnBnClickedRadio10();
	afx_msg void OnBnClickedRadio11();
	afx_msg void OnBnClickedRadio12();
	afx_msg void OnBnClickedRadio13();
	afx_msg void OnBnClickedRadio14();
	afx_msg void OnBnClickedRadio15();
	afx_msg void OnBnClickedRadio16();
	afx_msg void OnBnClickedRadio17();


	BOOL OnInitDialog();

	CComboBox m_ResolutionList;		// 해상도 List
	bool	  m_bComboListAdd;		// 해상도 List 추가 시 처음 한번만 되도록

	BOOL m_bScreen;					// 풀 스크린 여부
	BOOL m_bBGSound;				// 배경음 사용 여부
	BOOL m_bGameSound;				// 환경음 사용 여부
	BOOL m_bSoundEffect;			// 효과음 사용 여부
	BOOL m_bUseBright;				// 화면 밝기 사용 여부
	BOOL m_b3DMove;					// 3D 효과 사용 여부
	BOOL m_bVoice;					// 목소리 사용 여부

	CSliderCtrl m_ctrBGVolume;		// 배경은 슬라이드
	CSliderCtrl m_ctrGameSound;		// 환경음 슬라이드
	CSliderCtrl m_ctrSoundEffect;	// 효과음 슬라이드
	CSliderCtrl m_ctrBright;		// 화면 밝기 슬라이드
	int			m_nSpecification;	// 그래픽 사양(저사양:0, 일반사양:1, 고사양:2)

protected:
	void _LoadGlobal();
	void _SaveGlobal();

private:
	void BuildResolutionList();

public:
	sResolution * FindResolutionData(int const& nIdx);
	int		FindResolutionIdx( DWORD const& dwWidth, DWORD const& dwHeight );

private:
	std::string m_GloabalFilePath;
};
