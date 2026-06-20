// Digitalic LauncherCtrl.cpp : CDigitalicLauncherCtrl ActiveX 컨트롤 클래스의 구현입니다.

#include "stdafx.h"
#include "Digitalic Launcher.h"
#include "Digitalic LauncherCtrl.h"
#include "Digitalic LauncherPropPage.h"
#include "shlwapi.h" 
#include <io.h>
#include <TlHelp32.h> 

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CDigitalicLauncherCtrl, COleControl)



// 메시지 맵입니다.

BEGIN_MESSAGE_MAP(CDigitalicLauncherCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// 디스패치 맵입니다.

BEGIN_DISPATCH_MAP(CDigitalicLauncherCtrl, COleControl)
	DISP_FUNCTION_ID(CDigitalicLauncherCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION_ID(CDigitalicLauncherCtrl, "GameLaunching", dispidGameLaunching, GameLaunching, VT_BOOL, VTS_BSTR)
	DISP_FUNCTION_ID(CDigitalicLauncherCtrl, "IsGameLaunching", dispidIsGameLaunching, IsGameLaunching, VT_BOOL, VTS_NONE)
	DISP_FUNCTION_ID(CDigitalicLauncherCtrl, "GetFileVersion", dispidGetFileVersion, GetFileVersion, VT_BSTR, VTS_NONE)
END_DISPATCH_MAP()



// 이벤트 맵입니다.

BEGIN_EVENT_MAP(CDigitalicLauncherCtrl, COleControl)
END_EVENT_MAP()



// 속성 페이지입니다.

// TODO: 필요에 따라 속성 페이지를 추가합니다. 카운트도 이에 따라 증가해야 합니다.
BEGIN_PROPPAGEIDS(CDigitalicLauncherCtrl, 1)
PROPPAGEID(CDigitalicLauncherPropPage::guid)
END_PROPPAGEIDS(CDigitalicLauncherCtrl)



// 클래스 팩터리와 GUID를 초기화합니다.

IMPLEMENT_OLECREATE_EX(CDigitalicLauncherCtrl, "DIGITALICLAUNCHE.DigitalicLauncheCtrl.1",
					   0x3fed5059, 0x7aa5, 0x46d9, 0x90, 0x2b, 0x12, 0x4c, 0x8c, 0xd9, 0xa1, 0x6);//Active X 교체 시 변경
					   //0x60C904ED, 0xC3AD, 0x44a1, 0xA2, 0x4F, 0x7D, 0x7E, 0xA8, 0x63, 0x06, 0xA4)

					   // 형식 라이브러리 ID와 버전입니다.

					   IMPLEMENT_OLETYPELIB(CDigitalicLauncherCtrl, _tlid, _wVerMajor, _wVerMinor)



					   // 인터페이스 ID입니다.

					   const IID BASED_CODE IID_DDigitalicLauncher =
{ 0xBAB0F659, 0x9BD9, 0x4272, { 0x9F, 0x26, 0xD0, 0x3E, 0x1F, 0x7E, 0xC7, 0x2D } };
const IID BASED_CODE IID_DDigitalicLauncherEvents =
{ 0x1A8FBA6E, 0xE596, 0x4607, { 0x81, 0x67, 0x82, 0x9D, 0x7A, 0x6C, 0x3B, 0xB6 } };



// 컨트롤 형식 정보입니다.

static const DWORD BASED_CODE _dwDigitalicLauncherOleMisc =
OLEMISC_ACTIVATEWHENVISIBLE |
OLEMISC_SETCLIENTSITEFIRST |
OLEMISC_INSIDEOUT |
OLEMISC_CANTLINKINSIDE |
OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CDigitalicLauncherCtrl, IDS_DIGITALICLAUNCHER, _dwDigitalicLauncherOleMisc)



// CDigitalicLauncherCtrl::CDigitalicLauncherCtrlFactory::UpdateRegistry -
// CDigitalicLauncherCtrl에서 시스템 레지스트리 항목을 추가하거나 제거합니다.

BOOL CDigitalicLauncherCtrl::CDigitalicLauncherCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: 컨트롤이 아파트 모델 스레딩 규칙을 준수하는지
	// 확인합니다. 자세한 내용은 MFC Technical Note 64를
	// 참조하십시오. 컨트롤이 아파트 모델 규칙에
	// 맞지 않는 경우 다음에서 여섯 번째 매개 변수를 변경합니다.
	// afxRegApartmentThreading을 0으로 설정합니다.

	if (bRegister)
		return AfxOleRegisterControlClass(
		AfxGetInstanceHandle(),
		m_clsid,
		m_lpszProgID,
		IDS_DIGITALICLAUNCHER,
		IDB_DIGITALICLAUNCHER,
		afxRegApartmentThreading,
		_dwDigitalicLauncherOleMisc,
		_tlid,
		_wVerMajor,
		_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// CDigitalicLauncherCtrl::CDigitalicLauncherCtrl - 생성자입니다.

CDigitalicLauncherCtrl::CDigitalicLauncherCtrl()
{
	InitializeIIDs(&IID_DDigitalicLauncher, &IID_DDigitalicLauncherEvents);
	// TODO: 여기에서 컨트롤의 인스턴스 데이터를 초기화합니다.
}



// CDigitalicLauncherCtrl::~CDigitalicLauncherCtrl - 소멸자입니다.

CDigitalicLauncherCtrl::~CDigitalicLauncherCtrl()
{
	// TODO: 여기에서 컨트롤의 인스턴스 데이터를 정리합니다.
}



// CDigitalicLauncherCtrl::OnDraw - 그리기 함수입니다.

void CDigitalicLauncherCtrl::OnDraw(
									CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;

	// TODO: 다음 코드를 사용자가 직접 작성한 그리기 코드로 바꾸십시오.
	//pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	//pdc->Ellipse(rcBounds);
}



// CDigitalicLauncherCtrl::DoPropExchange - 지속성 지원입니다.

void CDigitalicLauncherCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: 지속적인 사용자 지정 속성 모두에 대해 PX_ functions를 호출합니다.
}



// CDigitalicLauncherCtrl::OnResetState - 컨트롤을 기본 상태로 다시 설정합니다.

void CDigitalicLauncherCtrl::OnResetState()
{
	COleControl::OnResetState();  // DoPropExchange에 들어 있는 기본값을 다시 설정합니다.

	// TODO: 여기에서 다른 모든 컨트롤의 상태를 다시 설정합니다.
}



// CDigitalicLauncherCtrl::AboutBox - "정보" 대화 상자를 사용자에게 보여 줍니다.

void CDigitalicLauncherCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_DIGITALICLAUNCHER);
	dlgAbout.DoModal();
}

void CDigitalicLauncherCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	COleControl::OnLButtonDown(nFlags, point);

#ifdef _DEBUG
	AfxMessageBox( GetFileVersion() );

	if( IsGameLaunching() == VARIANT_TRUE )
		GameLaunching( L"nangbae 430j09gj0934 1 US REAL" );
#endif
}




// CDigitalicLauncherCtrl 메시지 처리기입니다.

VARIANT_BOOL CDigitalicLauncherCtrl::GameLaunching(LPCTSTR szParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 여기에 디스패치 처리기를 추가합니다.	

	m_hSession = ::InternetOpenA( "DMO", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );	
	if (m_hSession == NULL) 
	{	
		AfxMessageBox( NET_FAIL );
		return false;
	}

	::InternetCloseHandle( m_hSession );	

	CString aaa = REG_LAUNCHER_PATH;

	TCHAR szDir[ 1024 ];	
	if( REG::RegReadString( HKEY_CURRENT_USER, REG_LAUNCHER_PATH, REG_LAUNCHER_PATH_VALUE, NULL, szDir, 1024 ) == FALSE )	
	{
		if( REG::RegReadString( HKEY_LOCAL_MACHINE, REG_LAUNCHER_PATH, REG_LAUNCHER_PATH_VALUE, NULL, szDir, 1024 ) == FALSE )
		{
			Setup();
			return VARIANT_FALSE;
		}
	}	

	CString str;
	str += szDir;	
	str += L"\\";
	str += LAUNCHER_NAME;

	if( PathFileExists( str ) == 0 )
	{			
		Setup();
		return VARIANT_FALSE;
	}

	SHELLEXECUTEINFO si;                             // SHELLEXECUTEINFO 구조체를 하나 만든다
	ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));       // si 구조체 초기화 
	si.cbSize = sizeof(SHELLEXECUTEINFO);            // 크기 설정

	si.fMask = SEE_MASK_NOCLOSEPROCESS;              // ??
	si.lpVerb = L"runas";                             // 동작 형식(파일열기)
	si.lpFile = str;							     // szPath;  // 실행할 파일 경로 지정
	si.lpParameters = szParam;					 //szArg;    // 인자값 지정
	si.nShow = SW_SHOWNORMAL;                        // 표시 형식

	if( ShellExecuteEx(&si) == FALSE )                              // 외부 프로그램 호출
	{
		DWORD dwErr = GetLastError();
		switch( dwErr )
		{				
		case ERROR_FILE_NOT_FOUND:   AfxMessageBox( L"지정된 파일을 찾을수 없습니다" );
			break;		
		case ERROR_PATH_NOT_FOUND:    AfxMessageBox( L"지정된 경로를 찾을수 없습니다" );
			break;		
		case ERROR_DDE_FAIL:    AfxMessageBox( L"동적 데이터 교환 트랜잭션에 실패했습니다" );
			break;		
		case ERROR_NO_ASSOCIATION:    AfxMessageBox( L"주어진 파일의 확장명과 연결되는 응용 프로그램이 없습니다" );
			break;		
		case ERROR_ACCESS_DENIED:    AfxMessageBox( L"지정된 파일에 대한 액세스가 거부 되었습니다" );
			break;		
		case ERROR_DLL_NOT_FOUND:    AfxMessageBox( L"응용 프로그램을 실행하는 데 필요한 하나의 라이브러리 파일을 찾을 수없습니다" );
			break;		
		case ERROR_CANCELLED:    AfxMessageBox( L"함수가 사용자를 위한 추가정보를 알려주었지만 사용자가 요청을 취소 했습니다" );
			break;		
		case ERROR_NOT_ENOUGH_MEMORY:    AfxMessageBox( L"그곳에 지정된 작업을 수행하기 위한 충분한 메모리가 없습니다" );
			break;		
		case ERROR_SHARING_VIOLATION:    AfxMessageBox( L"공유 위반이 발생했습니다" );
			break;	
		}
	}	

	return VARIANT_TRUE;
}

HWND hWnd;
// EnumWindows callback함수
BOOL CALLBACK EnumFunc(HWND hwnd, LPARAM lParam) // lParm - D-Player.exe
{                                     
	DWORD processID; // processID - winprocess 
	GetWindowThreadProcessId(hwnd, &processID);

	if(processID != DWORD(lParam))
	{
		return TRUE;
	} 

	hWnd = hwnd; // 해당 proc의 handle
	return FALSE;
}


VARIANT_BOOL CDigitalicLauncherCtrl::IsGameLaunching(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// TODO: 여기에 디스패치 처리기를 추가합니다.	

	bool bExecute = false;
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); 
	PROCESSENTRY32 pe32; 		
	pe32.dwSize = sizeof( PROCESSENTRY32 ); 

	Process32First( hProcessSnap, &pe32 ); 
	do
	{ 
		TCHAR process[255]; 
		_stprintf_s( process, 255, L"%s %5d", pe32.szExeFile, pe32.th32ProcessID ); 

		if( _tcsicmp( pe32.szExeFile, LAUNCHER_NAME ) == 0 )
		{			
			EnumWindows((WNDENUMPROC)EnumFunc, pe32.th32ProcessID);
			::SetForegroundWindow(hWnd); 

			CloseHandle(hProcessSnap);
			return VARIANT_FALSE;
		}				

		TCHAR executeName[ 512 ]={0,};
		_stprintf_s( executeName, 512, EXECUTE_NAME, DPlayerSetupVer );
		if( _tcsicmp( pe32.szExeFile, executeName ) == 0 )
		{				
			CloseHandle(hProcessSnap);
			return VARIANT_FALSE;
		}	

	} while( Process32Next( hProcessSnap, &pe32 ) ); 					

	CloseHandle(hProcessSnap);	

	return VARIANT_TRUE;
}

void CDigitalicLauncherCtrl::Setup()
{
	TCHAR szDir[ MAX_PATH ];
	GetSystemDirectory( szDir, MAX_PATH );

	CString str;
	str += szDir;
	str += L"\\";

	TCHAR executeName[ 512 ]={0,};
	_stprintf_s( executeName, 512, EXECUTE_NAME, DPlayerSetupVer );

	str += executeName;
	if( PathFileExists( str ) != 0 )
	{
		ShellExecute( NULL,  L"runas", str, NULL, NULL, SW_SHOW );
	}
	else
	{
		if( AfxMessageBox( MSG_INST ) == IDOK )
		{
			CString str;
			str += CDN_LAUNCHER_PATH;
			str += L"\\";
			str += executeName;
			ShellExecute( NULL, L"runas", str, NULL, NULL, SW_SHOW );	
		}
	}
}


BSTR CDigitalicLauncherCtrl::GetFileVersion(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString strResult;

	strResult = FILE_VERSION_STR;

	return strResult.AllocSysString();
}
