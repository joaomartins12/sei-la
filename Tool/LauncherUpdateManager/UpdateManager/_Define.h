#pragma once

#define PASS_SERVER_CHECK				_T("GM마스터.101")	// 서버 정검일 때 패치/게임실행 가능하도록 하는 파일명

#define CONFIGURATION_FILENAME			_T("DMO_Configuration_V2.json")

#if defined GSP_STEAM	// 스팀
	#ifdef QA_VERSION
		#define CONFIGURATION_URL		_T("http://dmo.gameking.com/digimon/cdn/dmo/QA/Configuration/Steam/")
	#else // QA_VERSION
		#define CONFIGURATION_URL		_T("http://dmo.gameking.com/digimon/cdn/dmo/Live/Configuration/Steam/")
	#endif//QA_VERSION
#elif defined GSP_VERSION// 게임킹
	#ifdef QA_VERSION
		#define CONFIGURATION_URL		_T("http://dmo.gameking.com/digimon/cdn/dmo/QA/Configuration/GSP/")
	#else // QA_VERSION
		#define CONFIGURATION_URL		_T("http://dmo.gameking.com/digimon/cdn/dmo/Live/Configuration/GSP/")
	#endif//QA_VERSION
#elif defined HK_VERSION	// 홍콩
	#ifdef QA_VERSION
		#define CONFIGURATION_URL		_T("https://hkdmo-image.gameking.com/hk_qa/Configuration/Hongkong/")
	#else // QA_VERSION
		#define CONFIGURATION_URL		_T("https://hkdmo-image.gameking.com/hk_live/Configuration/Hongkong/")
	#endif//QA_VERSION
#elif defined TW_VERSION	// 대만		
	#ifdef QA_VERSION
		#define CONFIGURATION_URL		_T("https://twdmo-image.gameking.com/tw_qa/Configuration/Taiwan/")
	#else // QA_VERSION
		#define CONFIGURATION_URL		_T("https://twdmo-image.gameking.com/tw_live/Configuration/Taiwan/")
	#endif//QA_VERSION
#elif defined TH_VERSION	// 태국
	#undef PASS_SERVER_CHECK
	#define PASS_SERVER_CHECK			_T("GMMaster.101")
	#ifdef QA_VERSION
		#define CONFIGURATION_URL		_T("https://twdmo-image.gameking.com/tw_qa/Configuration/Taiwan/")
	#else // QA_VERSION
		#define CONFIGURATION_URL		_T("https://twdmo-image.gameking.com/tw_live/Configuration/Taiwan/")
	#endif//QA_VERSION
#elif defined KOR_VERSION	// 한국	
	#ifdef QA_VERSION
		#define CONFIGURATION_URL		_T("http://www.digimonmasters.com/digimon/cdn/dmo/QA/Configuration/Korea/")
	#else // QA_VERSION
		#define CONFIGURATION_URL		_T("http://www.digimonmasters.com/digimon/cdn/dmo/Live/Configuration/Korea/")
	#endif//QA_VERSION
#endif//GSP_STEAM	
