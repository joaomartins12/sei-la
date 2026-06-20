#pragma once

#ifndef __AUTOUPDAFINE_H__
#define __AUTOUPDAFINE_H__

#ifdef GSP_VERSION
#define USER_TYPE_KEY	_T("GSP")
#elif GSP_STEAM
#define USER_TYPE_KEY	_T("Steam")
#elif HK_VERSION
#define USER_TYPE_KEY	_T("Hongkong")
#elif TW_VERSION
#define USER_TYPE_KEY	_T("Taiwan")
#elif TH_VERSION
#define USER_TYPE_KEY	_T("Thailand")
#endif

// Themida sdk 사용
#define USE_THEMIDA_SDK

// 윈도우 10 호환성 모드 실행.
//#define CHECK_WINDOW_10
// 호환성 모드를 사용할 경우 UAC를 켜야함

	enum
	{
		eSTATE_CONNECT = 0,
		eSTATE_VERSION_DOWN,
		eSTATE_VERSION_COMP,
		eSTATE_FILE_DOWN,
		eSTATE_DO_PACK,
		eSTATE_UPDATE_COMP,
	};

	#define U_PROCESS						(0)
	#define U_WAIT							(-1)
	#define UNZ_SUCCESS						(-2)
	#define URL_SUCCESS						(-3)
	#define PACK_SUCCESS					(-4)
	#define PACK_FAILED_OPENFILE			(-5)

	#define U_ERROR_MIN						(-100)

	#define UNZ_PARAMERROR                  (-102)
	#define UNZ_BADZIPFILE                  (-103)
	#define UNZ_CRCERROR                    (-105)
	#define UNZ_OPENFILE_ERROR				(-106)
	#define UNZ_NOT_FILE					(-201)
	#define UNZ_FAIL_DOWNLOAD				(-202)
	#define UNZ_FAIL_DOWN_VERSION			(-203)

	#define URL_NOT_FILE					(-1001)
	#define URL_OPENFILE_ERROR				(-1002)

	#define FP_OPEN_ERROR					(-2001)

	#define WISH_CLOSE_WINDOW				L":Downloader-DigimonMasters"
	#define PASS_SERVER_CHECK				L"GM마스터.101"	// 서버 정검일 때 패치/게임실행 가능하도록 하는 파일명

	// 패치파일을 팩으로 패치함.
	//#define PATCH_PACK

	#define COPYLIGHT		L"ⓒ ©Alpha Digimon World 2024.. Co..Ltd. All rights Reserved."

	
	#define INI_READ_DEFAULT_STRING			L"NULL"									//	INI 에서 읽어들일때 초기값

	#define CONFIGURATION_FILENAME			L"DMO_Configuration_V2.json"

	#if defined GSP_STEAM	// 스팀
		#define FILE_WARD					L"GDMO"									//  버전정보(숫자)를 제외한 파일명 ex) GDMO132.zip
		#define VERSIONINFO_FILE			L"LauncherLib\\vGDMO.ini"				//	버전 정보 파일
		#define DIMA_OAUTH_KEY_VALUE		"DiMaOAuthKey.value"
		#ifdef QA_VERSION
			#define CONFIGURATION_URL		L"http://dmo.gameking.com/digimon/cdn/dmo/QA/Configuration/Steam/"
		#else // QA_VERSION
			#define CONFIGURATION_URL		L"http://dmo.gameking.com/digimon/cdn/dmo/Live/Configuration/Steam/"
		#endif//QA_VERSION
	#elif defined GSP_VERSION// 게임킹
		#define FILE_WARD					L"GDMO"									//  버전정보(숫자)를 제외한 파일명 ex) GDMO132.zip
		#define VERSIONINFO_FILE			L"LauncherLib\\vGDMO.ini"				//	버전 정보 파일
		#define DIMA_OAUTH_KEY_VALUE		"DiMaOAuthKey.value"
		#ifdef QA_VERSION
			#define CONFIGURATION_URL		L"http://dmo.gameking.com/digimon/cdn/dmo/QA/Configuration/GSP/"
		#else // QA_VERSION
			#define CONFIGURATION_URL		L"http://dmo.gameking.com/digimon/cdn/dmo/Live/Configuration/GSP/"
		#endif//QA_VERSION
	#elif defined HK_VERSION	// 홍콩
		#define FILE_WARD					L"HODMO"									//  버전정보(숫자)를 제외한 파일명 ex) GDMO132.zip
		#define VERSIONINFO_FILE			L"LauncherLib\\vHDMO.ini"				//	버전 정보 파일
		#define DIMA_OAUTH_KEY_VALUE		"true"
		#ifdef QA_VERSION
			#define CONFIGURATION_URL		L"https://hkdmo-image.gameking.com/hk_qa/Configuration/Hongkong/"
		#else // QA_VERSION
			#define CONFIGURATION_URL		L"https://hkdmo-image.gameking.com/hk_live/Configuration/Hongkong/"
		#endif//QA_VERSION
	#elif defined TW_VERSION	// 대만
		#define FILE_WARD					L"TAIDMO"								//  버전정보(숫자)를 제외한 파일명 ex) GDMO132.zip
		#define VERSIONINFO_FILE			L"LauncherLib\\vTDMO.ini"				//	버전 정보 파일	
		#define DIMA_OAUTH_KEY_VALUE		"true"
		#ifdef QA_VERSION
			#define CONFIGURATION_URL		L"https://twdmo-image.gameking.com/tw_qa/Configuration/Taiwan/"
		#else // QA_VERSION
			#define CONFIGURATION_URL		L"https://twdmo-image.gameking.com/tw_live/Configuration/Taiwan/"
		#endif//QA_VERSION
	#elif defined TH_VERSION	// 태국
		#define FILE_WARD					L"TDMO"								//  버전정보(숫자)를 제외한 파일명 ex) GDMO132.zip
		#define VERSIONINFO_FILE			L"LauncherLib\\vTDMO.ini"				//	버전 정보 파일		
		#define DIMA_OAUTH_KEY_VALUE		"true"
		#undef PASS_SERVER_CHECK
		#define PASS_SERVER_CHECK			L"GMMaster.101"	// 서버 정검일 때 패치/게임실행 가능하도록 하는 파일명
		#ifdef QA_VERSION
			#define CONFIGURATION_URL		L"https://twdmo-image.gameking.com/QA/Configuration/Taiwan/"
		#else // QA_VERSION
			#define CONFIGURATION_URL		L"https://twdmo-image.gameking.com/Live/Configuration/Taiwan/"
		#endif//QA_VERSION
	#endif//GSP_STEAM	

	// 윈도우 사이즈
	const int _BackGround_Width			=	742;
	const int _BackGround_Height		=	525;

	// 메인 상태 창 ( 제일 위에 스트링 )
	const int _MainState_Pos_x			=	237;
	const int _MainState_Pos_y			=	423;
	const int _MainState_Size_x			=	600;
	const int _MainState_Size_y			=	12;

	// 서브 상태 창 ( 다운로드 스트링 )
	const int _SubState_Pos_x			=	217;
	const int _SubState_Pos_y			=	469;
	const int _SubState_Size_x			=	600;
	const int _SubState_Size_y			=	12;

	// 서브 상태 창 ( 언집 스트링 )
	const int _UnzipState_Pos_x			=	217;
	const int _UnzipState_Pos_y			=	470;
	const int _UnzipState_Size_x		=	600;
	const int _UnzipState_Size_y		=	12;

	//업데이트 진행 상태 바 길이
	const int _ProgressWidth			=	446;
	const int _ProgressHeight			=	12;

	// 메인 프로그래스 바
	const int _ProgressMainX			=	217;
	const int _ProgressMainY			=	437;
	// 다운로드 프로그래스 바
	const int _ProgressSubDownX			=	217;
	const int _ProgressSubDownY			=	453;
	// 언집 프로그래스 바
	const int _ProgressSubUnzipX		=	217;
	const int _ProgressSubUnzipY		=	453;

	const int _iFileCountX				= 410 - 88;
	const int _iFileCountY				= 290 + 53;
	const int _iFileSizeX				= 410 - 88;
	const int _iFileSizeY				= 290 + 53;

#endif