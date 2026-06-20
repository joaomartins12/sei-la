// FullPack.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "FullPack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void __stdcall UnicodeToUTF8(std::wstring const& wsSource, std::string& sDest)
{
	if (wsSource.empty())
		return;

	int iConverted = ::WideCharToMultiByte(CP_UTF8, 0, wsSource.c_str(), (int)wsSource.size(), NULL, 0, NULL, NULL);
	sDest.resize(iConverted);

	::WideCharToMultiByte(CP_UTF8, 0, wsSource.c_str(), (int)wsSource.size(), &sDest.at(0), iConverted, NULL, NULL);
}

void __stdcall UnicodeToMultiByte(std::wstring const& wsSource, std::string& sDest)
{
	if (wsSource.empty())
		return;

	int iConverted = ::WideCharToMultiByte(PO_CP_CODE, 0, wsSource.c_str(), (int)wsSource.size(), NULL, 0, NULL, NULL);
	sDest.resize(iConverted);

	::WideCharToMultiByte(PO_CP_CODE, 0, wsSource.c_str(), (int)wsSource.size(), &sDest.at(0), iConverted, NULL, NULL);
}

void __stdcall UTF8ToUnicode(std::string const& sSource, std::wstring& wsDest)
{
	if (sSource.empty())
		return;

	int iConverted = ::MultiByteToWideChar(CP_UTF8, 0, sSource.c_str(), (int)sSource.size(), NULL, 0);
	wsDest.resize(iConverted);

	::MultiByteToWideChar(CP_UTF8, 0, sSource.c_str(), (int)sSource.size(), &wsDest.at(0), iConverted);
}

void __stdcall MultiByteToUnicode(std::string const& sSource, std::wstring& wsDest)
{
	if (sSource.empty())
		return;

	int iConverted = ::MultiByteToWideChar(PO_CP_CODE, 0, sSource.c_str(), (int)sSource.size(), NULL, 0);
	wsDest.resize(iConverted);

	::MultiByteToWideChar(PO_CP_CODE, 0, sSource.c_str(), (int)sSource.size(), &wsDest.at(0), iConverted);
}


// CFullPackApp

BEGIN_MESSAGE_MAP(CFullPackApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CFullPackApp 생성

CFullPackApp::CFullPackApp() : m_bConsole(FALSE), m_iPatchNumber(-1)
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CFullPackApp 개체입니다.

CFullPackApp theApp;


// CFullPackApp 초기화

BOOL CFullPackApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	int iCnt = 0;
	LPWSTR* pStr = CommandLineToArgvW( GetCommandLine(), &iCnt);
	switch(iCnt)
	{
	case 2:
		{
			m_iPatchNumber = _wtoi(pStr[1]);
			m_bConsole = TRUE;			
		}
		break;
	default:
		break;
	}

	// 문자열 변환 함수 등록
	DmCS::StringFn::SetConvFn(&UnicodeToUTF8, &UnicodeToMultiByte, &UTF8ToUnicode, &MultiByteToUnicode);

	CFullPackDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();	

	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	
	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}
