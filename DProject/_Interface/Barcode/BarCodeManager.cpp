

#include "stdafx.h"

#ifdef USE_BARCODE_REDER
#include "../../Flow/FlowMgr.h"
#include "../../Flow/Flow.h"
#include "BarCodeManager.h"

nsBARCODE::pFunc_BarcodeStart			nsBARCODE::g_pFuncStart = NULL;
nsBARCODE::pFunc_BarcodeStop			nsBARCODE::g_pFuncStop = NULL;
nsBARCODE::pFunc_BarcodeVersion			nsBARCODE::g_pFuncVersion = NULL;
nsBARCODE::pFunc_BarcodeSerial			nsBARCODE::g_pFuncSerial = NULL;
nsBARCODE::pFunc_BarcodeConnectStatus	nsBARCODE::g_pFuncConnectStatus = NULL;
nsBARCODE::eSTATE						nsBARCODE::g_eState = nsBARCODE::BC_NONE;
nsBARCODE::eSTATE						nsBARCODE::g_eOldState = nsBARCODE::BC_NONE;
CsTimeSeq								nsBARCODE::g_ConnectCheckTimeSeq;

TCHAR									nsBARCODE::g_szSerial[ BC_SERIAL_NUM_COUNT + 1 ] = {0, };
bool									nsBARCODE::g_bFirstEquipBarcode = false;
bool									nsBARCODE::g_bServerConnect = false;
int										nsBARCODE::g_nConnectCount = 0;
HMODULE nsBARCODE::g_hDll = NULL;


void nsBARCODE::FreeLibrary()
{
	if( g_hDll == NULL )
		return;

	SendBarcodeOff();

	assert_cs( g_pFuncStop );
	g_pFuncStop();

	::FreeLibrary( g_hDll );

	g_eState				= BC_NONE;

	g_hDll					= NULL;
	g_pFuncStart			= NULL;
	g_pFuncStop				= NULL;
	g_pFuncVersion			= NULL;
	g_pFuncSerial			= NULL;
	g_pFuncConnectStatus	= NULL;
}

nsBARCODE::eLOAD_LIB nsBARCODE::LoadLibrary()
{
	if( IsInstallDrive() == false )
		return LL_NOT_INSTALL;

	if( g_hDll != NULL )
		return LL_LOADED;

	g_hDll = ::LoadLibrary( _T( "BarCodeManager.dll" ) );
	if( g_hDll == NULL )
		return LL_LOAD_FAIL_DLL_FILE;

	// GetProcAddress 의 두번째 인자 함수이름 알아내는 방법
	// 1. 비쥬얼 스튜디오의 VC\Bin 폴더에 mspdb80.dll 파일을 가져다 놓는다.
	// 2. 해당 폴더에 실제 쓸 BarCodeManager.dll 파일도 가져다 놓는다.
	// 3. 도스창으로 VC\Bin 폴더에 이동후 dumpbin /EXPORTS BarCodeManager.dll > BCFunc.txt	입력
	// 4. 해당 폴더의 BCFunc.txt 참조
	// 5. 전부 완료후 꼭 해당 폴더의 mspdb80.dll 을 지워주거나 이름을 바꾸어 주어야 한다. ( 안그러면 컴파일이 안됨 )	

	g_pFuncStart = (pFunc_BarcodeStart)GetProcAddress( g_hDll, "?BarCodeStart@@YAHPAUHWND__@@@Z" );
	if( g_pFuncStart == NULL )
		return LL_LOAD_FAIL_FUNC_ADDRESS;
	
	g_pFuncStop = (pFunc_BarcodeStop)GetProcAddress( g_hDll, "?BarCodeStop@@YAXXZ" );
	if( g_pFuncStop == NULL )
		return LL_LOAD_FAIL_FUNC_ADDRESS;

	g_pFuncVersion = (pFunc_BarcodeVersion)GetProcAddress( g_hDll, "?BarCodeVersion@@YAHXZ" );
	if( g_pFuncVersion == NULL )
		return LL_LOAD_FAIL_FUNC_ADDRESS;

	g_pFuncSerial = (pFunc_BarcodeSerial)GetProcAddress( g_hDll, "?BarCodeSerial@@YAHXZ" );
	if( g_pFuncSerial == NULL )
		return LL_LOAD_FAIL_FUNC_ADDRESS;

	g_pFuncConnectStatus = (pFunc_BarcodeConnectStatus)GetProcAddress( g_hDll, "?BarCodeConnectStatus@@YAHXZ" );
	if( g_pFuncConnectStatus == NULL )
		return LL_LOAD_FAIL_FUNC_ADDRESS;

	g_pFuncStart( GAMEAPP_ST.GetHWnd() );

	g_ConnectCheckTimeSeq.SetDeltaTime( 5000 );
	g_ConnectCheckTimeSeq.Reset();

	return LL_SUCCESS;
}

LRESULT nsBARCODE::Proc( WPARAM wParam, LPARAM lParam )
{
	switch( wParam )
	{
	case USB_CONNECTED:
		{
			switch( g_eState )
			{
			case BC_NONE:
			case BC_UNCONNECT:
				g_eState = BC_INIT;
				g_bFirstEquipBarcode = true;
				break;
			}
		}
		break;
	case USB_NOT_CONNECTED:
		{
			SendBarcodeOff();
			g_eState = BC_UNCONNECT;
		}
		break;
	case SCAN_DATA:
		{
			if( g_pCharMng->GetTamerUser() )
			{
				if( g_pCharMng->GetTamerUser()->GetProp_Animation()->IsBarcodeCardAni() == false )
				{
					SendCardScan( (TCHAR*)lParam );
				}
				else
				{
					cPrintMsg::PrintMsg( 20044 );
				}
			}			
		}
		break;
	case FW_VERSION:
		{
			// 번호 확인후
			// 바코드 시리얼 번호 요청
			g_pFuncSerial();
		}
		break;
	case SERIAL_DATA:
		{
			// 시리얼 번호 확인 후 - 서버에 연결 榮募?신호는 ( 시리얼 번호, ID? 또는 계정 정보 같이 보내자 )
			// 임시 - 서버에서 데이터 받으면 값이렇게 변경
			g_eState = BC_CONNECT;

			char c[ BC_SERIAL_NUM_COUNT + 1 ] = {0, };
			memcpy( c, (char*)lParam, sizeof( char )*BC_SERIAL_NUM_COUNT );
			M2W( g_szSerial, c, BC_SERIAL_NUM_COUNT + 1 );

//#pragma todo( "임시시리얼넘버" )
//			_tcscpy_s( g_szSerial, BC_SERIAL_NUM_COUNT + 1, _T( "BCBKYG1HOEDSA" ) );
		}
		break;
	}
	return 0;
}

void nsBARCODE::Update()
{
	if( g_pResist->m_Global.s_bUseBarcode == false )
	{
		FreeLibrary();
		return;
	}	
	else
	{
		nsBARCODE::LoadLibrary();
	}

	switch( g_eState )
	{
	case BC_CONNECT:
		{
			if( g_eOldState != g_eState )
			{
				if( g_ConnectCheckTimeSeq.IsEnable() == true )
				{
					if( SendBarcodeOn() == false )
						return;
				}
				else
				{
					return;
				}
			}
		}
		break;
	case BC_NONE:
	case BC_WAIT_CONNECT:
	case BC_UNCONNECT:
		if( g_bServerConnect == true )
		{
			SendBarcodeOff();
		}
		break;
	case BC_INIT:
		g_eState = BC_WAIT_CONNECT;
		// 펌웨어 버젼 확인
		g_pFuncSerial();
		//g_pFuncVersion();
		break;
	}

	g_eOldState = g_eState;
}

bool nsBARCODE::IsConnet()
{
	return ( nsBARCODE::g_eState == BC_CONNECT );
}

bool nsBARCODE::IsConnectSend()
{
	// ( Npc이용, 거래중 )
	if( g_pGameIF->IsOpenNpcWindow() )
	{
		cPrintMsg::PrintMsg( 30024 );
		return false;
	}
	if( g_pCharMng->GetDigimonUser( 0 )->IsServerDie() )
	{
		cPrintMsg::PrintMsg( 20050 );
		return false;
	}	
	return true;
}

bool nsBARCODE::IsCardSend()
{
	if( g_pEventSceneMng->IsCameraScene() == true )
	{
		cPrintMsg::PrintMsg( 20049 );
		return false;
	}

	// 라이딩
	if( g_pCharMng->GetTamerUser()->IsRide() )
	{
		cPrintMsg::PrintMsg( 30014 );
		return false;
	}

	// 전투중
	if( g_pCharMng->GetDigimonUser( 0 )->IsBattle() )
	{
		cPrintMsg::PrintMsg( 20048 );
		return false;
	}
	// ( Npc이용, 거래중 )
	if( g_pGameIF->IsOpenNpcWindow() )
	{
		cPrintMsg::PrintMsg( 30024 );
		return false;
	}
	if( g_pCharMng->GetDigimonUser( 0 )->IsServerDie() )
	{
		cPrintMsg::PrintMsg( 20050 );
		return false;
	}	
	return true;
}

void nsBARCODE::ResetOldState()
{
	g_eOldState = BC_NONE;
	nsBARCODE::g_nConnectCount = 0;
	nsBARCODE::g_bServerConnect = false;
	g_ConnectCheckTimeSeq.Reset();
}

bool nsBARCODE::SendBarcodeOn()
{
	if( net::game == NULL )
		return true;

	if( g_pResist->m_Global.s_bUseBarcode == false )
		return true;

	if( g_eState != BC_CONNECT )
		return true;

	if( _tcslen( g_szSerial ) != BC_SERIAL_NUM_COUNT )
	{
		++g_nConnectCount;

		if( g_nConnectCount > 5 )
		{
			g_pFuncSerial();

			cPrintMsg::PrintMsg( 20061 );
			g_ConnectCheckTimeSeq.SetDeltaTime( 5000 );
			g_ConnectCheckTimeSeq.OffReset();
		}
		else
		{
			g_pFuncSerial();

			g_ConnectCheckTimeSeq.SetDeltaTime( 500 );
			g_ConnectCheckTimeSeq.OffReset();
		}
		return false;
	}

	if( IsConnectSend() == false )
	{
		return false;
	}

	// 시리얼 번호 송신
	if( g_bFirstEquipBarcode == true )
	{
		if( net::game->SendScannerEquip( g_szSerial ) == false )
		{
			++g_nConnectCount;

			if( g_nConnectCount > 5 )
			{
				cPrintMsg::PrintMsg( 20061 );
				g_ConnectCheckTimeSeq.SetDeltaTime( 5000 );
				g_ConnectCheckTimeSeq.OffReset();
			}
			else
			{
				g_ConnectCheckTimeSeq.SetDeltaTime( 500 );
				g_ConnectCheckTimeSeq.OffReset();
			}
			return false;
		}
	}
	else
	{
		net::game->SendScannerOn( g_szSerial );
	}

	nsBARCODE::g_nConnectCount = 0;
	nsBARCODE::g_bServerConnect = true;	

	return true;
}

void nsBARCODE::SendBarcodeOff()
{
	if( net::game == NULL )
		return;

	if( g_eState != BC_CONNECT )
		return;

	if( IsConnectSend() == false )
		return;

	cMessageBox::DelMsg( 20039, false );
	cMessageBox::DelMsg( 20040, false );
	cMessageBox::DelMsg( 20041, false );
	net::game->SendScannerOff();

	nsBARCODE::g_nConnectCount = 0;
	nsBARCODE::g_bServerConnect = false;
}


void nsBARCODE::SendBarcodeResist()
{
	if( net::game == NULL )
		return;

	if( g_pResist->m_Global.s_bUseBarcode == false )
		return;

	if( g_eState != BC_CONNECT )
	{
		cPrintMsg::PrintMsg( 20042 );
		return;
	}

	if( _tcslen( g_szSerial ) != BC_SERIAL_NUM_COUNT )
	{
		cPrintMsg::PrintMsg( 20042 );
		g_pFuncSerial();
		return;
	}

	if( IsCardSend() == false )
	{
		return;
	}

	// 시리얼 번호 송신
	net::game->SendScannerRegister( nsBARCODE::g_szSerial );
}

void nsBARCODE::SendBarcodeRelease()
{
	if( net::game == NULL )
		return;

	if( g_pResist->m_Global.s_bUseBarcode == false )
		return;

	if( g_eState != BC_CONNECT )
	{
		cPrintMsg::PrintMsg( 20042 );
		return;
	}

	if( _tcslen( g_szSerial ) != BC_SERIAL_NUM_COUNT )
	{
		cPrintMsg::PrintMsg( 20042 );
		g_pFuncSerial();
		return;
	}

	if( IsCardSend() == false )
	{
		return;
	}

	// 시리얼 번호 송신
	net::game->SendScannerRelease( nsBARCODE::g_szSerial );
}

NiPoint2 nsBARCODE::CalItemPos()
{
	CTamerUser* pTamer = g_pCharMng->GetTamerUser();
	NiPoint2 pos = pTamer->GetPos2D();
	NiPoint3 dir = pTamer->GetDir();
	pos.x += dir.x*150.0f;
	pos.y += dir.y*150.0f;

	int nCount = 0;
	while( nsCsGBTerrain::g_pCurRoot->IsValidEmr( (UINT)pos.x, (UINT)pos.y ) == false )
	{
		pos.x -= ( dir.x * 2.0f );
		pos.y -= ( dir.y * 2.0f );
		++nCount;
		if( nCount >= 75 )
		{
			return pTamer->GetPos2D();
		}
	}
	return pos;
}

void nsBARCODE::SendCardScan( TCHAR* szCard )
{
	if( net::game == NULL )
		return;

	if(FLOWMGR_ST.IsCurrentFlow(Flow::CFlow::FLW_LOADING))
		return; 

	if( g_pResist->m_Global.s_bUseBarcode == false )
		return;

	if( _tcslen( szCard ) != BC_CARD_NUM_COUNT )
		return;

	if( IsCardSend() == false )
	{
		return;
	}
	
	// 시리얼 번호 송신
	NiPoint2 v = CalItemPos();
	nSync::Pos pos( (uint)v.x, (uint)v.y );
	net::game->SendCardScan( szCard, pos );
}

void nsBARCODE::LogOut()
{
	g_bFirstEquipBarcode = true;
	g_ConnectCheckTimeSeq.Reset();
}





//typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);
//typedef BOOL (WINAPI *lpfnWow64DisableWow64FsRedirection)(PVOID *);
//typedef BOOL (WINAPI *lpfnWow64RevertWow64FsRedirection)(PVOID);
//PVOID DriverSwitchOldValue;
//
//#define SCANNER_DRIVER_NAME TEXT("chorus2usb.sys")
//
//
//BOOL IsWow64()
//{
//	BOOL bIsWow64 = FALSE;
//	LPFN_ISWOW64PROCESS fnIsWow64Process;
//
//	fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
//	if (NULL != fnIsWow64Process)
//	{
//		if (!fnIsWow64Process(GetCurrentProcess(), &bIsWow64))
//		{
//			// handle error
//		}
//	}
//
//	return bIsWow64;
//}
//
//BOOL SwitchWow64Driver(BOOL bSwitchOn)
//{
//	BOOL bSuccessSwitch = FALSE;
//	lpfnWow64DisableWow64FsRedirection fnWow64DisableWow64FsRedirection;
//	lpfnWow64RevertWow64FsRedirection fnWow64RevertWow64FsRedirection;
//
//	fnWow64DisableWow64FsRedirection = (lpfnWow64DisableWow64FsRedirection)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Wow64DisableWow64FsRedirection");
//	fnWow64RevertWow64FsRedirection = (lpfnWow64RevertWow64FsRedirection)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "Wow64RevertWow64FsRedirection");
//	if (bSwitchOn)
//	{
//		if (NULL != fnWow64DisableWow64FsRedirection)
//		{
//			bSuccessSwitch = fnWow64DisableWow64FsRedirection(&DriverSwitchOldValue);
//		}
//		else
//		{
//			bSuccessSwitch = FALSE;
//		}
//	}
//	else
//	{
//		if (NULL != fnWow64RevertWow64FsRedirection)
//		{
//			bSuccessSwitch = fnWow64RevertWow64FsRedirection(DriverSwitchOldValue);
//		}
//		else
//		{
//			bSuccessSwitch = FALSE;
//		}
//	}
//
//	return bSuccessSwitch;
//}

bool nsBARCODE::IsInstallDrive()
{
	TCHAR lpSubKey[512];
	//LPTSTR lpSubKey[512];

	HKEY hKey;
	LONG returnStatus;

	wsprintf(lpSubKey, _T("SYSTEM\\CURRENTCONTROLSET\\ENUM\\USB\\VID_0E18&PID_0001"));

	/* Open Key */
	returnStatus = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		lpSubKey,
		0L,
		KEY_QUERY_VALUE,        //Changed from v1.0.  Used to be "KEY_ALL_ACCESS" which requires administrator logon (and application administrator execution level to work in Vista).
		&hKey);
	RegCloseKey(hKey);

	return ( returnStatus == ERROR_SUCCESS );

	/*wchar_t szDriverFileName[MAX_PATH];
	wchar_t szWindowsDir[MAX_PATH];
	bool bIsOk = false;

	if (GetWindowsDirectory(szWindowsDir, sizeof(wchar_t) * MAX_PATH))
	{
		OSVERSIONINFO osvi;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		WIN32_FIND_DATA wfd;
		BOOL bIsWow64 = FALSE;

		ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		GetVersionEx(&osvi);
		if ((osvi.dwMajorVersion >= 5) && IsWow64())
		{
			bIsWow64 = TRUE;
			SwitchWow64Driver(TRUE);
		}
		swprintf(szDriverFileName, TEXT("\\\\?\\%s\\System32\\drivers\\*.sys"), szWindowsDir);
		hFind = FindFirstFile(szDriverFileName, &wfd);
		if (hFind == INVALID_HANDLE_VALUE) 
		{
			bIsOk = false;
		}
		else
		{
			do {
				if (wcsicmp(wfd.cFileName, SCANNER_DRIVER_NAME) == 0)
				{
					bIsOk = true;
					break;
				}
			} while (FindNextFile(hFind, &wfd));
			FindClose(hFind);
		}
		if (bIsWow64)
		{
			SwitchWow64Driver(FALSE);
		}
	}

	return bIsOk;*/
}

#endif