// NpcToolDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "NpcTool.h"
#include "NpcToolDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CNpcToolDlg 대화 상자




CNpcToolDlg::CNpcToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNpcToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pSelNpc = NULL;
}

void CNpcToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_NPCID, m_edit_NpcID);
	DDX_Control(pDX, IDC_COMBO_MAPID, m_combo_MapID);
	DDX_Control(pDX, IDC_EDIT_MAPDISCRIPTE, m_edit_MapDiscript);
	DDX_Control(pDX, IDC_COMBO_NPCTYPE, m_combo_NpcType);
	DDX_Control(pDX, IDC_EDIT_NPCNAME, m_edit_NpcName);
	DDX_Control(pDX, IDC_EDIT_NPCTALK, m_edit_NpcTalk);
	DDX_Control(pDX, IDC_EDIT_NPCMODEL, m_edit_NpcModelID);
	DDX_Control(pDX, IDC_EDIT_NPCMODELNAME, m_edit_NpcModelName);
	DDX_Control(pDX, IDC_EDIT_NPCID_DISCRIPT, m_edit_NpcID_Discript);
	DDX_Control(pDX, IDC_LIST_NPCLIST, m_list_NpcList);
	DDX_Control(pDX, IDC_EDIT_NPCPENNAME, m_edit_NpcPenName);
	DDX_Control(pDX, IDC_BUTTON_ADD, m_pBtnAdd);
	DDX_Control(pDX, IDC_COMBO_NPCTYPE2, m_Combo_MoveType);
	DDX_Control(pDX, IDC_COMBO_EXTRA_TYPE, m_cmb_ExtraType);
	DDX_Control(pDX, IDC_LIST_EXTRA, m_list_Extra);
	DDX_Control(pDX, IDC_BUTTON_EXTRA_ADD, m_btn_ExtraAdd);
	DDX_Control(pDX, IDC_BUTTON_EXTRA_DEL, m_btn_ExtraDel);
	DDX_Control(pDX, IDC_COMBO_DISPINFO, m_cmb_DispInfo);
	DDX_Control(pDX, IDC_COMBO_ZOOMVIEW, m_cmb_ZoomView);
	DDX_Control(pDX, IDC_COMBO_SUBTYPE, m_combo_SubType);
}

BEGIN_MESSAGE_MAP(CNpcToolDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_CBN_SELCHANGE(IDC_COMBO_MAPID, &CNpcToolDlg::OnCbnSelchangeComboMapid)
	ON_EN_CHANGE(IDC_EDIT_NPCMODEL, &CNpcToolDlg::OnEnChangeEditNpcmodel)
	ON_EN_CHANGE(IDC_EDIT_NPCID, &CNpcToolDlg::OnEnChangeEditNpcid)
	ON_EN_CHANGE(IDC_EDIT_NPCNAME, &CNpcToolDlg::OnEnChangeEditNpcname)
	ON_EN_CHANGE(IDC_EDIT_NPCTALK, &CNpcToolDlg::OnEnChangeEditNpctalk)
	ON_CBN_SELCHANGE(IDC_COMBO_NPCTYPE, &CNpcToolDlg::OnCbnSelchangeComboNpctype)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CNpcToolDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CNpcToolDlg::OnBnClickedButtonSave)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_NPCLIST, &CNpcToolDlg::OnLvnColumnclickListNpclist)
	ON_NOTIFY(NM_CLICK, IDC_LIST_NPCLIST, &CNpcToolDlg::OnNMClickListNpclist)
	ON_BN_CLICKED(IDC_BUTTON_DELLIST, &CNpcToolDlg::OnBnClickedButtonDellist)
	ON_WM_MOVING()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_CREATE()
	ON_EN_CHANGE(IDC_EDIT_NPCPENNAME, &CNpcToolDlg::OnEnChangeEditNpcpenname)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_STORE, &CNpcToolDlg::OnBnClickedButtonCheckStore)
	ON_BN_CLICKED(IDC_BUTTON_SEL_RELEASE, &CNpcToolDlg::OnBnClickedButtonSelRelease)
	ON_CBN_SELCHANGE(IDC_COMBO_NPCTYPE2, &CNpcToolDlg::OnCbnSelchangeComboNpctype2)
	ON_BN_CLICKED(IDC_BUTTON_EXTRA_ADD, &CNpcToolDlg::OnBnClickedButtonExtraAdd)
	ON_BN_CLICKED(IDC_BUTTON_EXTRA_DEL, &CNpcToolDlg::OnBnClickedButtonExtraDel)
	ON_LBN_SELCHANGE(IDC_LIST_EXTRA, &CNpcToolDlg::OnLbnSelchangeListExtra)
	ON_CBN_SELCHANGE(IDC_COMBO_DISPINFO, &CNpcToolDlg::OnCbnSelchangeComboDispinfo)
	ON_CBN_SELCHANGE(IDC_COMBO_ZOOMVIEW, &CNpcToolDlg::OnCbnSelchangeComboZoomview)
	ON_BN_CLICKED(IDC_BUTTON_MODELINFO, &CNpcToolDlg::OnBnClickedButtonModelinfo)
	ON_CBN_SELCHANGE(IDC_COMBO_SUBTYPE, &CNpcToolDlg::OnCbnSelchangeComboSubType)
END_MESSAGE_MAP()


// CNpcToolDlg 메시지 처리기

BOOL CNpcToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	__Init();

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CNpcToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CNpcToolDlg::OnPaint()
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

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CNpcToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL CNpcToolDlg::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if( (DWORD)pMsg->wParam == VK_ESCAPE )
			return true;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

