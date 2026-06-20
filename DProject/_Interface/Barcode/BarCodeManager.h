
#ifdef USE_BARCODE_REDER

#pragma once

//
//#ifdef BARCODEMANAGER_EXPORTS
//#define BARCODEMANAGER_API __declspec(dllexport)
//#else
//#define BARCODEMANAGER_API __declspec(dllimport)
//#endif
//
//// 통신 사용되는 버퍼의 크기
//#define BUFFER_SIZE 64
//
//// 메시지에서 사용되는 인수 정의
//#define FW_VERSION			0x00  // 펌웨어 버젼을 나타냄
//#define SCAN_DATA			0x01  // 스캔된 바코드의 번호를 나타냄
//#define SERIAL_DATA			0x02  // 시리얼 번호를 나타냄
//#define USB_CONNECTED		0x03  // USB가 연결되었음을 나타냄
//#define USB_NOT_CONNECTED	0x04  // USB가 연결되지 않았음을 나타냄
//
//// BarCode Reader로부터 읽은 데이터를 전달하는 메시지
//#define WM_USER_CARD_DATA WM_USER+100
//
//// BarCode Reader와의 통신을 모니터링을 시작하는 함수
//BARCODEMANAGER_API int BarCodeStart(HWND hWnd);
//// BarCode Reader와의 통신을 모니터링을 종료하는 함수
//BARCODEMANAGER_API void BarCodeStop();
//// BarCode Reader의 펌웨어 버젼을 확인하는 함수
//BARCODEMANAGER_API int BarCodeVersion();
//// BarCode Reader의 시리얼을 확인하는 함수
//BARCODEMANAGER_API int BarCodeSerial();
//// BarCode Reader의 연결 상태를 확인하는 함수
//BARCODEMANAGER_API int BarCodeConnectStatus();
//// BarCode Reader의 시리얼을 모듈의 ROM에 저장하는 함수
//BARCODEMANAGER_API int BarCodeWriteSerial(BYTE *pSerial);

// BarCode Reader로부터 읽은 데이터를 전달하는 메시지
#define WM_USER_CARD_DATA WM_USER+100

// 메시지에서 사용되는 인수 정의
#define FW_VERSION			0x00  // 펌웨어 버젼을 나타냄
#define SCAN_DATA			0x01  // 스캔된 바코드의 번호를 나타냄
#define SERIAL_DATA			0x02  // 시리얼 번호를 나타냄
#define USB_CONNECTED		0x03  // USB가 연결되었음을 나타냄
#define USB_NOT_CONNECTED	0x04  // USB가 연결되지 않았음을 나타냄

#define BC_SERIAL_NUM_COUNT		13
#define BC_CARD_NUM_COUNT		15


namespace nsBARCODE
{
	enum eSTATE{ BC_NONE, BC_INIT, BC_WAIT_CONNECT, BC_CONNECT, BC_UNCONNECT, };
	enum eLOAD_LIB{ LL_NO_USE, LL_LOADED, LL_SUCCESS, LL_NOT_INSTALL, LL_LOAD_FAIL_DLL_FILE, LL_LOAD_FAIL_FUNC_ADDRESS, };

	// BarCode Reader와의 통신을 모니터링을 시작하는 함수
	typedef int (*pFunc_BarcodeStart)( HWND hWnd );
	// BarCode Reader와의 통신을 모니터링을 종료하는 함수
	typedef void (*pFunc_BarcodeStop)();
	// BarCode Reader의 펌웨어 버젼을 확인하는 함수
	typedef int (*pFunc_BarcodeVersion)();
	// BarCode Reader의 시리얼을 확인하는 함수
	typedef int (*pFunc_BarcodeSerial)();
	// BarCode Reader의 연결 상태를 확인하는 함수
	typedef int (*pFunc_BarcodeConnectStatus)();

	extern pFunc_BarcodeStart			g_pFuncStart;
	extern pFunc_BarcodeStop			g_pFuncStop;
	extern pFunc_BarcodeVersion			g_pFuncVersion;
	extern pFunc_BarcodeSerial			g_pFuncSerial;
	extern pFunc_BarcodeConnectStatus	g_pFuncConnectStatus;
	extern eSTATE						g_eState;
	extern eSTATE						g_eOldState;
	extern HMODULE						g_hDll;
	extern TCHAR						g_szSerial[ BC_SERIAL_NUM_COUNT + 1 ];
	extern bool							g_bFirstEquipBarcode;
	extern bool							g_bServerConnect;
	extern CsTimeSeq					g_ConnectCheckTimeSeq;
	extern int							g_nConnectCount;

	void		FreeLibrary();
	eLOAD_LIB	LoadLibrary();

	bool	IsConnet();
	bool	IsInstallDrive();
	bool	IsConnectSend();
	bool	IsCardSend();

	NiPoint2 CalItemPos();

	void	ResetOldState();
	void	Update();
	bool	SendBarcodeOn();
	void	SendBarcodeOff();
	void	SendBarcodeResist();
	void	SendBarcodeRelease();

	void	SendCardScan( TCHAR* szCard );

	void	LogOut();

	LRESULT Proc( WPARAM wParam, LPARAM lParam );
}

#endif