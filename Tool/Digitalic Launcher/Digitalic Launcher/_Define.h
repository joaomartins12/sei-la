
#pragma once

#define LAUNCHER_NAME			L"D-Player.exe"	//런쳐 실행 파일

#ifdef VERSION_QA
#define DPlayerSetupVer			242	// 셋업을 파일 실행 버젼
#define EXECUTE_NAME			L"D-PlayerInstall_Alpha_%d.exe"
#define FileDescription_Str		"Movegames Launcher OCX QA"
#define ProductName				"Movegames Launcher OCX QA"
#define CDN_LAUNCHER_PATH		L"http://movegames-cdn.gameking.com/Movegames/431956/DMO/Dev_Alpha/Kor/Launcher"
#define REG_LAUNCHER_PATH		L"Software\\digitalic\\LauncherQA"
#define REG_LAUNCHER_PATH_VALUE	L"Path"
#else
#define DPlayerSetupVer			239	// 셋업을 파일 실행 버젼
#define EXECUTE_NAME			L"D-PlayerInstall_%d.exe"
#define FileDescription_Str		"Movegames Launcher OCX"
#define ProductName				"Movegames Launcher OCX"
#define CDN_LAUNCHER_PATH		L"http://movegames-cdn.gameking.com/Movegames/431956/DMO/Korea/Launcher"
#define REG_LAUNCHER_PATH		L"Software\\digitalic\\Launcher"
#define REG_LAUNCHER_PATH_VALUE	L"Launcher"
#endif

#define  FILE_VERSION			14, 08, 04, 1
#define  FILE_VERSION_STR		"14, 08, 04, 1"
