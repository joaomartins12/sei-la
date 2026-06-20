#ifndef __AUTOUPDAFINE_H__
#define __AUTOUPDAFINE_H__

//#define LAUNCHER_VERSION		L"0.205"

#define WISH_CLOSE_WINDOW		L":Downloader-DigimonMasters"

#define CHECK_FILE				L"check.txt"
#define OLD_VERSIONINFO_FILE	L"vDMO_1.ini"							//	예전 버전 정보 파일
#define EXECFILE_ARG			L" true"								//  실행화일 뒤에 붙일 이름

#define INI_READ_DEFAULT_STRING	L"NULL"									//	INI 에서 읽어들일때 초기값
#define PATCH_MIN_VERSION		0										//	패치를 받을수 있는 최하 버젼

#define NEW_EXE_TEMP			L"DMLauncher.tmp"							//	업데이트되는 런처파일이름
#define NEW_EXE_JUST_TEMP		L"DMLauncher.jtmp"							//	즉시 업데이트되는 런처파일이름
#define DEL_LAUNCHERFILENAME	L"DMLauncher.del"							//	제거할 파일
#define EXE_LAUNCHERFILENAME	L"DMLauncher.exe"							//	런쳐 실행 화일

#define COPYLIGHT				L"ⓒ Akiyoshi Hongo, Toei Animation Gameⓒ 2009 Move Games Co., Ltd. All Rights Reserved."


#ifdef QA_VERSION
	#define SERVER_NAME					L"http://gdmo-cdn.gameking.com/Partner/moveon/DMO/Patch/QA/"		//태국 QA
// 	#define SERVER_NAME					L"http://ptr.dmm.vplay.in.th/TestPatches/" //태국 QA
#else // QA_VERSION
	#define SERVER_NAME					L"http://gdmo-cdn.gameking.com/Partner/moveon/DMO/Patch/Live/"		// 태국 본섭 패치
// 	#define SERVER_NAME					L"http://ptr.dmm.vplay.in.th/Patches/"		// 태국 본섭 패치
#endif // QA_VERSION	

	#define BANER_URL					L"http://static.vplay.in.th/dmm/inGame/launcher/index.html"	
	
	#define FILE_WARD					L"TDMO"									//  버전정보(숫자)를 제외한 파일명 ex) GDMO132.zip
	#define UPGRADEINFO_FILE			L"PatchInfo_TDMO.ini"					//  패치 정보 파일(서버에 올라가있는 버전정보 파일)
	#define VERSIONINFO_FILE			L"LauncherLib\\vTDMO.ini"				//	버전 정보 파일(내부에 존재하는 버전정보 파일)
	#define EXECFILE					L"DigimonMastersOnline.exe"				//	실행할 파일(vPlay\DigimonMasters) 참고
	#define HOME_DOWN_SITE				"http://dmm.vplay.in.th/content.php?page=download_game.html"
	#define HOME_PAGE_SITE				"http://dmm.vplay.in.th/home.php"

	#define PUBLISHED					L"COPYRIGHTⓒ Move On (2012) Co., Ltd. All Right Reserved."


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

#endif