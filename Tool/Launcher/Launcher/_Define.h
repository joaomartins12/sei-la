#pragma once

#ifndef SAFE_DELETE
#define SAFE_DELETE( p ) { if( p ) { delete p; (p) = NULL; } }
#endif

// Themida sdk 사용
#define USE_THEMIDA_SDK

//#define USE_DOWNENGINE

// 윈도우 10 호환성 모드 실행.
//#define CHECK_WINDOW_10

// 윈도우 사이즈
const int _BackGround_Width			=	800;
const int _BackGround_Height		=	600;

#define MAX_TRY							5
#define MAX_DRIVE_GB					5

#define UNZ_PARAMERROR                  (-102)
#define UNZ_BADZIPFILE                  (-103)
#define UNZ_CRCERROR                    (-105)
#define UNZ_OPENFILE_ERROR				(-106)
#define UNZ_NOT_FILE					(-201)

#define URL_NOT_FILE					(-1001)
#define URL_NOTCONNECT_INTERNET			(-1002)
#define URL_OPENFILE_ERROR				(-1003)
#define URL_FILE_ERROR					(-1004)
#define FP_OPEN_ERROR					(-2001)

#define WISH_CLOSE_WINDOW				L":Downloader-DigimonMasters"

#ifdef USE_DOWNENGINE
#define DE_DLL_FILE						L"DownEngineSDK.dll"		//	dll 경로
#endif

#define FONT_WHITE	RGB( 253, 253, 253 )
#define FONT_OVER	RGB( 137, 191, 245 )

#define PASS_SERVER_CHECK				L"GM마스터.101"	// 서버 정검일 때 패치/게임실행 가능하도록 하는 파일명

#define CONFIGURATION_FILENAME			L"DMO_Configuration_V2.json"

#ifdef VERSION_QA
	#define CONFIGURATION_URL			L"http://www.digimonmasters.com/digimon/cdn/dmo/QA/Configuration/Korea/"
	#define REG_CLIENT_PATH				L"Software\\Digitalic\\DMTest"	
#else
	#define CONFIGURATION_URL			L"http://www.digimonmasters.com/digimon/cdn/dmo/Live/Configuration/Korea/"
	#define REG_CLIENT_PATH				L"Software\\Digitalic\\DigimonMasters"
#endif	

#define REG_CLIENT_PATH_VALUE			L"Path"
#define CHECK_BOX						L"Check.ini"
#define CLIENT_PATCHINFO_FILE			L"\\LauncherLib\\vDMO.ini"
