// Digitalic Launcher.cpp : CDigitalicLauncherApp 및 DLL 등록의 구현입니다.

#include "stdafx.h"
#include "Digitalic Launcher.h"
#include "CatHelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDigitalicLauncherApp theApp;

const GUID CDECL BASED_CODE _tlid =
{ 0xF0A67E4, 0xA1E, 0x47E0, { 0x90, 0xD5, 0xA3, 0x9A, 0xA9, 0xBC, 0xC6, 0x5E } };

const CATID CATID_SafeForScripting     =
{0x7dd95801,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}};
const CATID CATID_SafeForInitializing  =
{0x7dd95802,0x9882,0x11cf,{0x9f,0xa9,0x00,0xaa,0x00,0x6c,0x42,0xc4}}; 

// const GUID CDECL BASED_CODE _ctlid = 
// { 0x21cec2fc, 0x24fa, 0x4eeb, {0xa0, 0x43, 0x3c, 0xc2, 0x48, 0x6, 0x8, 0x80} };
// {9A3D12F9-8802-4316-B877-022025566DA1}
//const GUID CDECL BASED_CODE _ctlid = 
//{ 0x9a3d12f9, 0x8802, 0x4316, { 0xb8, 0x77, 0x2, 0x20, 0x25, 0x56, 0x6d, 0xa1 } };//Active X 교체 시 변경

const GUID CDECL BASED_CODE _ctlid = 
{ 0x3fed5059, 0x7aa5, 0x46d9, { 0x90, 0x2b, 0x12, 0x4c, 0x8c, 0xd9, 0xa1, 0x6 } };//Active X 교체 시 변경



// const GUID CDECL BASED_CODE _ctlid = 
// { 0x60C904ED, 0xC3AD, 0x44a1, { 0xA2, 0x4F, 0x7D, 0x7E, 0xA8, 0x63, 0x06, 0xA4 } };//Active X 교체 시 변경


const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CDigitalicLauncherApp::InitInstance - DLL 초기화입니다.

BOOL CDigitalicLauncherApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: 여기에 직접 작성한 모듈 초기화 코드를 추가합니다.
	}

	return bInit;
}



// CDigitalicLauncherApp::ExitInstance - DLL 종료입니다.

int CDigitalicLauncherApp::ExitInstance()
{
	// TODO: 여기에 직접 작성한 모듈 종료 코드를 추가합니다.

	return COleControlModule::ExitInstance();
}



// DllRegisterServer - 시스템 레지스트리에 항목을 추가합니다.

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	if (FAILED( CreateComponentCategory(CATID_SafeForScripting, L"Controls that are safely scriptable") ))
		return ResultFromScode(SELFREG_E_CLASS);

	if (FAILED( CreateComponentCategory(CATID_SafeForInitializing, L"Controls safely initializable from persistent data") ))
		return ResultFromScode(SELFREG_E_CLASS);

	if (FAILED( RegisterCLSIDInCategory(_ctlid, CATID_SafeForScripting) ))
		return ResultFromScode(SELFREG_E_CLASS);

	if (FAILED( RegisterCLSIDInCategory(_ctlid, CATID_SafeForInitializing) ))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}



// DllUnregisterServer - 시스템 레지스트리에서 항목을 제거합니다.

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
