// ExcelToBinDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ExcelToBinDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CExcelToBinDlg 대화 상자




CExcelToBinDlg::CExcelToBinDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExcelToBinDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CExcelToBinDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
#ifdef VERSION_USA
BEGIN_MESSAGE_MAP(CExcelToBinDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CExcelToBinDlg::OnBnClickedButtonProcAeria)
	ON_BN_CLICKED(IDC_BUTTON_PROC_ENG, &CExcelToBinDlg::OnBnClickedButtonProcEng)
END_MESSAGE_MAP()
#else
BEGIN_MESSAGE_MAP(CExcelToBinDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_PROCESS, &CExcelToBinDlg::OnBnClickedButtonProcess)
	//ON_BN_CLICKED(IDC_BUTTON_PROC_ENG, &CExcelToBinDlg::OnBnClickedButtonProcEng)
	ON_BN_CLICKED(IDC_BUTTON_PROC_THA, &CExcelToBinDlg::OnBnClickedButtonProcTha)
	ON_BN_CLICKED(IDC_BUTTON_PROC_TAIWAN, &CExcelToBinDlg::OnBnClickedButtonProcTaiwan)
	ON_BN_CLICKED(IDC_BUTTON_PROC_HONGKONG, &CExcelToBinDlg::OnBnClickedButtonProcHongkong)
END_MESSAGE_MAP()
#endif

// CExcelToBinDlg 메시지 처리기

BOOL CExcelToBinDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CExcelToBinDlg::OnPaint()
{	
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CExcelToBinDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CExcelToBinDlg::OnBnClickedButtonProcess()
{
	static bool bStart = false;
	if( bStart )		
	{
		return;
	}
	bStart = true;

	// 로드 안할 목록	
	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapResurrection = false;

	// 로드해야하는 특별 처리
	nsCsFileTable::g_bUseMonsterDrop = true;
	nsCsFileTable::g_bReadInactiveQuest = true;
	nsCsFileTable::g_bAddExp = true;
	nsCsFileTable::g_bUseAchieve = true;
	nsCsFileTable::g_bSceneDataMng = true;
	nsCsFileTable::g_bUseCashShop = true;

#ifdef _DEBUG
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::KOREA_TRANSLATION );
#else
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::KOREA_TRANSLATION );
#endif

	nsCsFileTable::g_FileTableMng.SaveBin( nsCsFileTable::KOREA_TRANSLATION );
	nsCsFileTable::g_FileTableMng.StringChange( nsCsFileTable::KOREA_ORIGINAL );
	nsCsFileTable::g_FileTableMng.SaveBin( nsCsFileTable::KOREA_ORIGINAL );

	nsCsFileTable::g_FileTableMng.Delete();

	CsMessageBox( MB_OK, L"한국어 변환 완료" );	
}


void CExcelToBinDlg::OnBnClickedButtonProcEng()
{
	static bool bStart = false;
	if( bStart )		
	{
		return;
	}
	bStart = true;

	TCHAR sz[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, sz );

	// 로드 안할 목록	
	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapResurrection = false;
	nsCsFileTable::g_bUseEvoBattle = false;

	// 로드해야하는 특별 처리
	nsCsFileTable::g_bUseMonsterDrop = true;
	//nsCsFileTable::g_bUseTotalExcel = true;
	nsCsFileTable::g_bReadInactiveQuest = true;
	nsCsFileTable::g_bUseHelp = true;
	nsCsFileTable::g_bUseCashShop = true;
	nsCsFileTable::g_bAddExp = true;
	nsCsFileTable::g_bUseAchieve = true;
	nsCsFileTable::g_bSceneDataMng = true;

#ifdef _DEBUG
	bool bSuccess = nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::ENGLISH );
#else
	bool bSuccess = nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::ENGLISH );
#endif
	if( bSuccess == false )
	{
		CDialog::EndDialog(true);
		return;
	}

	nsCsFileTable::g_FileTableMng.SaveBin( nsCsFileTable::ENGLISH );

	nsCsFileTable::g_FileTableMng.Delete();
	CsMessageBox( MB_OK, L"GSP 변환 완료" );
	CDialog::EndDialog(true);
}

#ifdef VERSION_USA //Aeria ExcelToBin용 _chu8820
void CExcelToBinDlg::OnBnClickedButtonProcAeria()
{
	static bool bStart = false;
	if( bStart )		
	{
		return;
	}
	bStart = true;

	TCHAR sz[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, sz );

	// 로드 안할 목록	
	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapResurrection = false;
	nsCsFileTable::g_bUseEvoBattle = false;

	// 로드해야하는 특별 처리
	nsCsFileTable::g_bUseMonsterDrop = true;
	//nsCsFileTable::g_bUseTotalExcel = true;
	nsCsFileTable::g_bReadInactiveQuest = true;
	nsCsFileTable::g_bUseHelp = true;
	nsCsFileTable::g_bUseCashShop = true;
	nsCsFileTable::g_bAddExp = true;
	nsCsFileTable::g_bUseAchieve = true;
	nsCsFileTable::g_bSceneDataMng = true;

#ifdef _DEBUG
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::ENGLISH_A );
#else
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::ENGLISH_A );
#endif

	nsCsFileTable::g_FileTableMng.SaveBin( nsCsFileTable::ENGLISH_A );

	nsCsFileTable::g_FileTableMng.Delete();
	CsMessageBox( MB_OK, L"Aeria 변환 완료" );
	CDialog::EndDialog(true);
}
#endif

void CExcelToBinDlg::OnBnClickedButtonProcTha()
{
	static bool bStart = false;
	if( bStart )		
	{
		return;
	}
	bStart = true;

	TCHAR sz[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, sz );

	// 로드 안할 목록	
	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapResurrection = false;
	nsCsFileTable::g_bUseEvoBattle = false;
	
	// 로드해야하는 특별 처리
	nsCsFileTable::g_bUseMonsterDrop = true;
	//nsCsFileTable::g_bUseTotalExcel = true;
	nsCsFileTable::g_bReadInactiveQuest = true;
	nsCsFileTable::g_bUseHelp = true;
	nsCsFileTable::g_bUseCashShop = true;
	nsCsFileTable::g_bAddExp = true;
	nsCsFileTable::g_bUseAchieve = true;
	nsCsFileTable::g_bSceneDataMng = true;

#ifdef _DEBUG
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::THAILAND );
#else
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::THAILAND );
#endif

	nsCsFileTable::g_FileTableMng.SaveBin( nsCsFileTable::THAILAND );

	nsCsFileTable::g_FileTableMng.Delete();
	CsMessageBox( MB_OK, L"태국어 변환 완료" );
}

void CExcelToBinDlg::OnBnClickedButtonProcTaiwan()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static bool bStart = false;
	if( bStart )		
	{
		return;
	}
	bStart = true;

	TCHAR sz[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, sz );

	// 로드 안할 목록	
	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapResurrection = false;
	nsCsFileTable::g_bUseEvoBattle = false;

	// 로드해야하는 특별 처리
	nsCsFileTable::g_bUseMonsterDrop = true;
	//nsCsFileTable::g_bUseTotalExcel = true;
	nsCsFileTable::g_bReadInactiveQuest = true;
	nsCsFileTable::g_bUseHelp = true;
	nsCsFileTable::g_bUseCashShop = true;
	nsCsFileTable::g_bAddExp = true;
	nsCsFileTable::g_bUseAchieve = true;
	nsCsFileTable::g_bSceneDataMng = true;

#ifdef _DEBUG
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::TAIWAN );
#else
	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::TAIWAN );
#endif

	nsCsFileTable::g_FileTableMng.SaveBin( nsCsFileTable::TAIWAN );

	nsCsFileTable::g_FileTableMng.Delete();
	CsMessageBox( MB_OK, L"대만어 변환 완료" );
}

void CExcelToBinDlg::OnBnClickedButtonProcHongkong()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	static bool bStart = false;
	if( bStart )		
	{
		return;
	}
	bStart = true;

	TCHAR sz[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, sz );

	// 로드 안할 목록	
	nsCsMapTable::g_bUseMapStart = false;
	nsCsMapTable::g_bUseMapResurrection = false;
	nsCsFileTable::g_bUseEvoBattle = false;

	// 로드해야하는 특별 처리
	nsCsFileTable::g_bUseMonsterDrop = true;
	//nsCsFileTable::g_bUseTotalExcel = true;
	nsCsFileTable::g_bReadInactiveQuest = true;
	nsCsFileTable::g_bUseHelp = true;
	nsCsFileTable::g_bUseCashShop = true;
	nsCsFileTable::g_bAddExp = true;
	nsCsFileTable::g_bUseAchieve = true;
	nsCsFileTable::g_bSceneDataMng = true;

	nsCsFileTable::g_FileTableMng.Init( nsCsFileTable::FT_EXCEL, nsCsFileTable::HONGKONG );

	nsCsFileTable::g_FileTableMng.SaveBin( nsCsFileTable::HONGKONG );

	nsCsFileTable::g_FileTableMng.Delete();
	CsMessageBox( MB_OK, L"홍 콩 변환 완료" );
}
