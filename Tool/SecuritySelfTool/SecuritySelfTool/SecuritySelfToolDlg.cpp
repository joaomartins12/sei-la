// SecuritySelfToolDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "SecuritySelfTool.h"
#include "SecuritySelfToolDlg.h"
#include "..\..\..\SimpleSecurity\SecuritySelf.h"
#include <iostream>
#include <sstream>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSecuritySelfToolDlg 대화 상자




CSecuritySelfToolDlg::CSecuritySelfToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSecuritySelfToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSecuritySelfToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ORG_TEXT,m_InputEdit);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_ENCRIPT,m_OutputEncriptEdit);

	DDX_Control(pDX, IDC_EDIT_ENCRIPT_TEXT,m_InputEncriptEdit);
	DDX_Control(pDX, IDC_EDIT_DECRIPT_TEXT,m_OutputDecriptEdit);

	DDX_Control(pDX, IDC_EDIT_INPUT_TEXT,m_InputMakeTextToHexEdit);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_HEXTEXT,m_OutputMakeTextToHexEdit);

	DDX_Control(pDX, IDC_EDIT_INPUT_HEX_TEXT,m_InputMakeHextToTextEdit);
	DDX_Control(pDX, IDC_EDIT_OUTPUT_HEXTOTEXT,m_OutputMakeHextToTextEdit);
}

BEGIN_MESSAGE_MAP(CSecuritySelfToolDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(ID_ENCRIPT_BTN, &CSecuritySelfToolDlg::OnBnClickedEncriptBtn)
	ON_BN_CLICKED(ID_DECRIPT_BTN, &CSecuritySelfToolDlg::OnBnClickedDecriptBtn)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_HEX, &CSecuritySelfToolDlg::OnBnClickedButtonMakeHex)
	ON_BN_CLICKED(IDC_BUTTON_MAKE_TEXT, &CSecuritySelfToolDlg::OnBnClickedButtonMakeText)
END_MESSAGE_MAP()


// CSecuritySelfToolDlg 메시지 처리기

BOOL CSecuritySelfToolDlg::OnInitDialog()
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

void CSecuritySelfToolDlg::OnPaint()
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
HCURSOR CSecuritySelfToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSecuritySelfToolDlg::OnBnClickedEncriptBtn()
{
	CString input;
	m_InputEdit.GetWindowText( input );
	std::string s = std::string(CT2CA(input.operator LPCWSTR())); 
	std::string output;
	SimpleSecurity::SecuritySelf::Encript( s, output);
	CString cs(output.c_str());
	m_OutputEncriptEdit.SetWindowText( cs );
}

void CSecuritySelfToolDlg::OnBnClickedDecriptBtn()
{
	CString input;
	m_InputEncriptEdit.GetWindowText( input );

	std::string s = std::string(CT2CA(input.operator LPCWSTR())); 

	std::string output;
	SimpleSecurity::SecuritySelf::Decript(s, output);

	CString cs(output.c_str());

	m_OutputDecriptEdit.SetWindowText( cs );
}

void CSecuritySelfToolDlg::OnBnClickedButtonMakeHex()
{
	CString input;
	m_InputMakeTextToHexEdit.GetWindowText( input );
	std::string s = std::string(CT2CA(input.operator LPCWSTR())); 
	std::string output;
	SimpleSecurity::SecuritySelf::EncriptHex( s, output );
	CString cs(output.c_str());
	m_OutputMakeTextToHexEdit.SetWindowText( cs );	
}

void CSecuritySelfToolDlg::OnBnClickedButtonMakeText()
{
	CString input;
	m_InputMakeHextToTextEdit.GetWindowText( input );
	std::string s = std::string(CT2CA(input.operator LPCWSTR())); 
	std::string output;
	SimpleSecurity::SecuritySelf::DecriptHex( s, output );
	CString cs(output.c_str());
	m_OutputMakeHextToTextEdit.SetWindowText( cs );	
}
