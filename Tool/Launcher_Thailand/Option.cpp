// Option.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "AutoUpgrade.h"
#include "Option.h"


#define SIZE_SET	( s_dwStructSize = sizeof( *this ) - sizeof( COption::sSTRUCT_HEADER ) )
#define READ_RESIST_FILE( ClassMember, version )		{\
	DWORD dwSize = ClassMember.s_dwStructSize;\
	fread( &##ClassMember, sizeof( COption::sSTRUCT_HEADER ), 1, fp );\
	if( ClassMember.s_dwVersion == version )\
		{\
		fread( ( (BYTE*)(&##ClassMember ) ) + sizeof( COption::sSTRUCT_HEADER ), dwSize, 1, fp );\
		ClassMember.s_dwStructSize = dwSize;\
		}\
		else\
		ClassMember.Reset();\
}

// COption 대화 상자입니다.

IMPLEMENT_DYNAMIC(COption, CDialog)

COption::COption(CWnd* pParent /*=NULL*/)
	: CDialog(COption::IDD, pParent)
{
	m_bComboListAdd = false;
	m_Global.Reset();
	_LoadGlobal();

	m_ResolutionData.clear();

	if( m_Global.s_bFullScreen )	// 풀스크린
		m_bScreen = FALSE;
	else
		m_bScreen = TRUE;

	if( m_Global.s_bMusic )			// 배경음
		m_bBGSound = FALSE;
	else
		m_bBGSound = TRUE;

	if( m_Global.s_bSound )			// 환경음
		m_bGameSound = FALSE;
	else
		m_bGameSound = TRUE;

	if( m_Global.s_bSystem )		// 효과음
		m_bSoundEffect = FALSE;
	else
		m_bSoundEffect = TRUE;

	if( m_Global.s_bUseBright )		// 화면 밝기
		m_bUseBright = FALSE;
	else
		m_bUseBright = TRUE;

	if( m_Global.s_bEnableSound_3DMove )	// 3D 효과
		m_b3DMove = FALSE;
	else
		m_b3DMove = TRUE;

	if( m_Global.s_bEnableVoice )	// 목소리
		m_bVoice = FALSE;
	else
		m_bVoice = TRUE;

	switch( m_Global.s_nOptionDefIndex )
	{
	case 0:	// 저사양
		m_nSpecification = 0;
		break;
	case 1:	// 일반사양
		m_nSpecification = 1;
		break;
	case 2:	// 고사양
		m_nSpecification = 2;
		break;
	case -1:
		m_nSpecification = 1;
		break;
	default:
		assert_cs(false);
	}
}

COption::~COption()
{
	m_bComboListAdd = false;
}

void COption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO2, m_ResolutionList);
	DDX_Radio(pDX, IDC_RADIO2, m_bScreen);
	DDX_Radio(pDX, IDC_RADIO4, m_bBGSound);
	DDX_Radio(pDX, IDC_RADIO6, m_bGameSound);
	DDX_Radio(pDX, IDC_RADIO8, m_bSoundEffect);
	DDX_Control(pDX, IDC_SLIDER3, m_ctrBGVolume);
	DDX_Control(pDX, IDC_SLIDER1, m_ctrGameSound);
	DDX_Control(pDX, IDC_SLIDER2, m_ctrSoundEffect);
	DDX_Radio(pDX, IDC_RADIO1, m_bUseBright);
	DDX_Control(pDX, IDC_SLIDER4, m_ctrBright);
	DDX_Radio(pDX, IDC_RADIO11, m_nSpecification);
	DDX_Radio(pDX, IDC_RADIO14, m_b3DMove);
	DDX_Radio(pDX, IDC_RADIO16, m_bVoice);
}


BEGIN_MESSAGE_MAP(COption, CDialog)
	ON_BN_CLICKED(IDOK, &COption::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO4, &COption::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &COption::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_RADIO6, &COption::OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_RADIO7, &COption::OnBnClickedRadio7)
	ON_BN_CLICKED(IDC_RADIO8, &COption::OnBnClickedRadio8)
	ON_BN_CLICKED(IDC_RADIO9, &COption::OnBnClickedRadio9)
	ON_BN_CLICKED(IDC_RADIO2, &COption::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &COption::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO1, &COption::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO10, &COption::OnBnClickedRadio10)
	ON_BN_CLICKED(IDC_RADIO11, &COption::OnBnClickedRadio11)
	ON_BN_CLICKED(IDC_RADIO12, &COption::OnBnClickedRadio12)
	ON_BN_CLICKED(IDC_RADIO13, &COption::OnBnClickedRadio13)
	ON_BN_CLICKED(IDC_RADIO14, &COption::OnBnClickedRadio14)
	ON_BN_CLICKED(IDC_RADIO15, &COption::OnBnClickedRadio15)
	ON_BN_CLICKED(IDC_RADIO16, &COption::OnBnClickedRadio16)
	ON_BN_CLICKED(IDC_RADIO17, &COption::OnBnClickedRadio17)
END_MESSAGE_MAP()


// COption 메시지 처리기입니다.

void COption::OnBnClickedOk()	// 옵션 확인 버튼
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nComboSel = m_ResolutionList.GetCurSel();
	sResolution* pInfo = FindResolutionData( nComboSel );
	
	m_Global.s_nResolutionWidth  = pInfo->nWidth;
	m_Global.s_nResolutionHeight = pInfo->nHeight;

	m_Global.s_bFullScreen = !(m_bScreen);
	m_Global.s_bMusic      = !(m_bBGSound);
	m_Global.s_fMusic	   = m_ctrBGVolume.GetPos() * 0.01f;
	m_Global.s_bSound	   = !(m_bGameSound);
	m_Global.s_fSound	   = m_ctrGameSound.GetPos() * 0.01f;
	m_Global.s_bSystem	   = !(m_bSoundEffect);
	m_Global.s_fSystem	   = m_ctrSoundEffect.GetPos() * 0.01f;
	m_Global.s_bUseBright  = !(m_bUseBright);
	m_Global.s_fBright	   = m_ctrBright.GetPos() * 0.01f;

	m_Global.s_bEnableSound_3DMove = !(m_b3DMove);
	m_Global.s_bEnableVoice = !(m_bVoice);

	m_Global.s_nOptionDefIndex = m_nSpecification;
	
	switch( m_Global.s_nOptionDefIndex )
	{
	case 0:
		m_Global.s_eTexFilter = 5;			// 텍스쳐 디테일
		m_Global.s_nShadowType = 0;			// 그림자
		m_Global.s_nWeather = 0;			// 날씨
		m_Global.s_nAntialicing = 0;		// 안티얼라이징
		m_Global.s_bCharOutLine = false;	// 카툰
		m_Global.s_bCell = false;			// 풀 설정
		m_Global.s_bHSync = false;			// 수직 동기화
		m_Global.s_fObjectSightRate = 0.70f;	// 배경 시야
		break;
	case 1:
		m_Global.s_eTexFilter = 4;
		m_Global.s_nShadowType = 1;
		m_Global.s_nWeather = 1;
		m_Global.s_nAntialicing = 65536;
		m_Global.s_bCharOutLine = true;
		m_Global.s_bCell = true;
		m_Global.s_bHSync = true;
		m_Global.s_fObjectSightRate = 0.91f;
		break;
	case 2:
		m_Global.s_eTexFilter = 2;
		m_Global.s_nShadowType = 1;
		m_Global.s_nWeather = 2;
		m_Global.s_nAntialicing = 196608;
		m_Global.s_bCharOutLine = true;
		m_Global.s_bCell = true;
		m_Global.s_bHSync = true;
		m_Global.s_fObjectSightRate = 1.0f;
		break;
	}

	_SaveGlobal();

	OnOK();
}

COption::sResolution * COption::FindResolutionData(int const& nIdx)
{
	std::list<sResolution>::iterator it = m_ResolutionData.begin();
	for( int n = 0; it != m_ResolutionData.end(); ++it, ++n )
	{
		if( nIdx == n )
			return &*it;
	}
	return NULL;
}

int COption::FindResolutionIdx( DWORD const& dwWidth, DWORD const& dwHeight )
{
	std::list<sResolution>::const_iterator it = m_ResolutionData.begin();
	for( int n = 0; it != m_ResolutionData.end(); ++it, ++n )
	{
		if( it->nWidth == dwWidth && it->nHeight == dwHeight )
			return n;
	}

	return -1;
}

void COption::BuildResolutionList()
{
	int iModeIdx = 0;
	DEVMODE devMode;
	devMode.dmSize = sizeof( DEVMODE );

	while( TRUE == ::EnumDisplaySettings( NULL, iModeIdx, &devMode ) )
	{	//	디스플레이 정보 생성
		bool bInsert = true;

		if( devMode.dmBitsPerPel != 32 )
			bInsert = false;
		else if( devMode.dmPelsWidth < 1024 || devMode.dmPelsHeight < 768 )
			bInsert = false;
		// 비율이 비율이 4:3 (0.75) 보다 크면 안됨 
		else if( (float)devMode.dmPelsHeight / (float)devMode.dmPelsWidth > 0.75f )
			bInsert = false;
		
		if( FindResolutionIdx( devMode.dmPelsWidth, devMode.dmPelsHeight ) != -1 )
			bInsert = false;

		if( bInsert )
		{	// 새로 추가
			sResolution add;
			add.nWidth = devMode.dmPelsWidth;
			add.nHeight = devMode.dmPelsHeight;
			m_ResolutionData.push_back( add );

			CString addText;
			addText.Format(L"%d x %d", add.nWidth, add.nHeight);
			m_ResolutionList.AddString( addText );		
		}
		++iModeIdx;
	}
}

BOOL COption::OnInitDialog()
{
	CDialog::OnInitDialog();

	if( m_bComboListAdd == false )
	{
		// 해상도
		BuildResolutionList();
		int selIdx = FindResolutionIdx( m_Global.s_nResolutionWidth, m_Global.s_nResolutionHeight );
		m_ResolutionList.SetCurSel(selIdx);

		// 배경음
		m_ctrBGVolume.SetRange(0, 100);
		m_ctrBGVolume.SetRangeMin(0);
		m_ctrBGVolume.SetRangeMax(100);
		int nBGVolumePos = (int)(m_Global.s_fMusic * 100);
		m_ctrBGVolume.SetPos(nBGVolumePos);

		// 게임 사운드
		m_ctrGameSound.SetRange(0, 100);
		m_ctrGameSound.SetRangeMin(0);
		m_ctrGameSound.SetRangeMax(100);
		int nGameSoundPos = (int)(m_Global.s_fSound * 100);
		m_ctrGameSound.SetPos(nGameSoundPos);

		// 환경음
		m_ctrSoundEffect.SetRange(0, 100);
		m_ctrSoundEffect.SetRangeMin(0);
		m_ctrSoundEffect.SetRangeMax(100);
		int nSoundEffectPos = (int)(m_Global.s_fSystem * 100);
		m_ctrSoundEffect.SetPos(nSoundEffectPos);

		// 화면 밝기 슬라이드
		m_ctrBright.SetRange(0, 200);
		m_ctrBright.SetRangeMin(0);
		m_ctrBright.SetRangeMax(200);
		int ns_fBrightPos = (int)(m_Global.s_fBright * 100);
		m_ctrBright.SetPos(ns_fBrightPos);
		if( m_Global.s_bUseBright )
			m_ctrBright.EnableWindow( TRUE );
		else
			m_ctrBright.EnableWindow( FALSE );

		m_bComboListAdd = true;
	}

	return TRUE;
}

void COption::OnBnClickedRadio4()	// B/G Sound On Mode
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bBGSound = false;
}

void COption::OnBnClickedRadio5()	// B/G Sound Off Mode
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bBGSound = true;
}

void COption::OnBnClickedRadio6()	// Game Sound On Mode
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bGameSound = false;
}

void COption::OnBnClickedRadio7()	// Game Sound Off Mode
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bGameSound = true;
}

void COption::OnBnClickedRadio8()	// Sound Effect On Mode
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSoundEffect = false;
}

void COption::OnBnClickedRadio9()	// Sound Effect Off Mode
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bSoundEffect = true;
}

void COption::OnBnClickedRadio2()	// Full Screen Mode
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bScreen = false;
}

void COption::OnBnClickedRadio3()	// Window Screen Mode
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bScreen = true;
}

void COption::OnBnClickedRadio1()		// 화면 밝기 사용자 지정 사용
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bUseBright = false;
	m_ctrBright.EnableWindow(TRUE);		// 밝기 조정 슬라이드 enable
}

void COption::OnBnClickedRadio10()		// 화면 밝기 사용 안함
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bUseBright = true;
	m_ctrBright.EnableWindow(FALSE);	// 밝기 조정 슬라이드 disable
}

void COption::OnBnClickedRadio11()		// 기본 사양(저사양)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSpecification = 0;
}

void COption::OnBnClickedRadio12()		// 기본 사양(일반 사양)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSpecification = 1;
}

void COption::OnBnClickedRadio13()		// 기본 사양(고사양)
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nSpecification = 2;
}

void COption::OnBnClickedRadio14()	// 3D 효과 On
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_b3DMove = false;
}

void COption::OnBnClickedRadio15()	// 3D 효과 Off
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_b3DMove = true;
}

void COption::OnBnClickedRadio16()	// 목소리 On
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bVoice = false;
}

void COption::OnBnClickedRadio17()	// 목소리 Off
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_bVoice = true;
}

void COption::sGLOBAL::Reset()
{
	SIZE_SET;
	s_dwVersion = RES_GLOBAL_VERSION;
	s_nLeft  = 0;
	s_nTop = 0;
	s_bFullScreen = true;

	s_bSaveID = true;
	s_szLastConnectID[ 0 ] = NULL;

	s_bCharOutLine = true;	
	s_nShadowType = SHADOW_ON;	

	s_nWeather = 1;

	s_eTexFilter = 2;
	s_nAntialicing = 65536;

	s_bHSync = true;
	s_bCell = true;

	s_fObjectSightRate = 1.0f;

	s_bUseBright = FALSE;
	s_fBright = 1.0f;

	s_nOptionDefIndex = -1;

	s_bUseMyShader = true;

	s_bUseBarcode = FALSE;

	s_bRightToolTip = TRUE;
	s_bBossWarning = TRUE;

	s_nRefreshTime = 0;
	s_nPostEffect = 0;

	ResetSound();
	ResetResolution();
	ResetInterface();	
}

void COption::sGLOBAL::ResetSound()
{
	s_fMusic = 0.7f;
	s_bMusic = true;
	s_fSound = 1.0f;
	s_bSound = true;
	s_fSystem = 1.0f;
	s_bSystem = true;

	s_bEnableVoice = true;

	s_bEnableSound_3DMove = false;
}

void COption::sGLOBAL::ResetResolution()
{
	//assert_cs( ( g_pResolutionInfo == NULL ) || ( g_pResolutionInfo->GetResolution( GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) ) != NULL ) );
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);

	if( x < 1024 )
	{
		s_nResolutionWidth = 1024;
		s_nResolutionHeight = 768;
	}
	else
	{
		s_nResolutionWidth = x;
		s_nResolutionHeight = y;
	}
}

void COption::sGLOBAL::ResetInterface()
{
	s_bMyRenderName			= true;
	s_bOtherRenderName		= true;
	s_bMonsterRenderName	= true;
	s_bNpcRenderName		= true;

	s_bViewHpBar			= true;

	s_bChatBalloone			= true;
	s_bTipBalloone			= true;
	s_bDigimonBalloone		= true;

	s_bFigureTamer			= false;
	s_bFigureDigimon		= false;
	s_bFigurePat			= false;
	s_bFigureEmployment		= false;

	s_eFTLanguage			= 100;
	s_bRightHandMouse		= true;

	s_bTradeActive			= TRUE;
	s_bPartyActive			= TRUE;
	s_bDualActive			= TRUE;
	s_bGuildInvitation		= TRUE;

	s_bViewLevelEffect		= TRUE;

	s_bCameraEnemyCritical	= TRUE;

	s_eAttackEffectViewMode	= AEM_ALL_ON;

	s_bRightToolTip  = TRUE;
	s_bBossWarning = TRUE;

	s_bMoveDigimon = FALSE;
	s_bMove_Mouse = FALSE;
	s_bMove_KeyBoard = TRUE;

// 	g_pResist->m_CenterBar.s_bInitial = false;
// 	g_pResist->m_CenterBar.s_fAlpha = 0.5f;
}

void COption::_LoadGlobal()
{
	//m_Global.Reset();

	// 접근 안된다면 패스
	if( _access_s( RES_GLOBAL_PATH, 0 ) != 0 )
		return;

	FILE* fp = NULL;
	fopen_s( &fp, RES_GLOBAL_PATH, "rb" );
	if( fp == NULL )
		return;
	READ_RESIST_FILE( m_Global, RES_GLOBAL_VERSION );
	fclose( fp );
}

void COption::_SaveGlobal()
{
	// 폴더생성
	char cFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( cFolder, MAX_PATH, RES_GLOBAL_PATH );
	nsCSFILE::CreateDirectory( cFolder );

	m_Global.ReleaseLastConnectID();

	// 글로벌 정보 세이브
	FILE* fp = NULL;
	fopen_s( &fp, RES_GLOBAL_PATH, "wb" );
	if( fp != NULL )
	{
		fwrite( &m_Global, sizeof( sGLOBAL ), 1, fp );
		fclose( fp );
	}
}
