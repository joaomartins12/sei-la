#include "stdafx.h"
#include "Main.h"
#include "Splash.h"
#include "App/KillProcess.h"
#include "..\LibProj\CsFunc\CrashLogger.h"

#ifdef DEF_CORE_NPROTECT
#include "./nProtect/Client_nProtect.h"
#endif

#ifdef SDM_DEF_XIGNCODE3_20181107
#include "./xigncode3/Client_Xigncode3.h"
#endif

//#include ".\Dummper\cDumper.hpp"

bool			g_bInstallStart = false;

CsLog			g_CriticalLog;

int				g_nScreenWidth = DEFAULT_SCREEN_WIDTH;
int				g_nScreenHeight = DEFAULT_SCREEN_HEIGHT;
float			g_fDeltaTime = 0.0f;
NiRenderer*		g_pRenderer = NULL;
//eLOOP_PLAY_STATE	g_eLoopPlayState = LPS_NONE;

volatile bool g_bActive = false;
bool		g_bUseFilePack = false;

#ifdef IME_WINDOW_POSITION
int		g_nIMEWidth  = 0;
int		g_nIMEHeight = 0;
bool	g_bIMEPosition = false;
#endif//IME_WINDOW_POSITION


bool g_bFirstLoding = true;
bool g_bFirstConnetChar = false;

// //////////////////////////////////////////////////////////////////////////////////////////////
// //																							//
// //																							//
// //																							//
// //////////////////////////////////////////////////////////////////////////////////////////////
void Release()
{
#ifdef USE_BARCODE_REDER
	nsBARCODE::FreeLibrary();
#endif//USE_BARCODE_REDER

#ifdef USE_DUMPER
	g_Dumper.SetCodeState( cDumper::SERVER );
#endif//_GIVE

	net::stop();
	net::end();

#ifdef USE_DUMPER
	g_Dumper.SetCodeState( cDumper::NORMAL );
	g_Dumper.SetCodeState( cDumper::GBShutDown );
#endif//_GIVE

	SAFE_NIDELETE( g_pEngine );
	CEngine::ShutDown();
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

bool GetUsingFilePack()
{

#ifdef _DEBUG
	cProfile x1("./dmo.ini");	
	if( x1.GetInt("DEBUG", "FilePack") == 1)
		return true;
	else
		return false;
#elif defined _GIVE
	return true;
#elif defined PC_BANG_SERVICE_TEST	//PC방 테스트 관련 알파서버 접속 클라 생성
	return true;
#else//_DEBUG
	cProfile x1("./dmo.ini");	
	if( x1.GetInt("DEBUG", "FilePack") == 1)
		return true;
	else
		return false;
#endif	//_DEBUG

}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool Kor_Command( std::vector<std::string> const& tokkens, bool bSkipAuth )
{
	GLOBALDATA_ST.SetLanguage( GData::eLanguage_Kor );

	if( bSkipAuth )
		return true;

	size_t size = tokkens.size();
	if( size < 3 )
	{
		CsMessageBox( MB_OK, _LAN( "런쳐에서 실행하여 주십시오!!" ) );
		return false;
	}

	for( size_t n = 0; n < size; ++n )
	{
		std::string value = tokkens[n];
		switch( n )
		{
		case 0:	GLOBALDATA_ST.SetAccountID( value );	break;// User ID
		case 1:	GLOBALDATA_ST.SetAccountPW( value );	break;// User PW
		case 2: GLOBALDATA_ST.SetUserType( value ); break;
		}
	}
	return true;
};

bool USA_Command( std::vector<std::string> const& tokkens, bool bSkipAuth )
{
	GLOBALDATA_ST.SetLanguage( GData::eLanguage_Eng );

#ifdef _GIVE
	bSkipAuth = true;
#endif

	if( bSkipAuth )
		return true;

	if( tokkens.empty() )
	{
		CsMessageBox( MB_OK, _LAN( "z" ) );
		return false;
	}

	if( tokkens[0].compare( "Aeria" ) == 0 )
	{
		size_t size = tokkens.size();
		if( size < 3 )
		{
			CsMessageBox( MB_OK, _LAN( "Y" ) );
			return false;
		}

		GLOBALDATA_ST.SetConnectionCountryType( GData::eCountry_Aeria );
		GLOBALDATA_ST.SetAuthCode( tokkens[1] );
		GLOBALDATA_ST.SetAccountID( tokkens[2] );
		GLOBALDATA_ST.SetAccountPW( "AeriaUser" );
	}
	else if( tokkens[0].compare( DIMA_OAUTH_KEY_VALUE ) != 0 )
	{
		CsMessageBox( MB_OK, _LAN( "S" ) );
		return false;
	}

	return true;
};

bool Steam_Command( std::vector<std::string> const& tokkens, bool bSkipAuth )
{
	GLOBALDATA_ST.SetLanguage( GData::eLanguage_Eng );
#ifdef VERSION_STEAM

	if( !bSkipAuth )
	{
		if( tokkens.empty() || tokkens[0].compare( DIMA_OAUTH_KEY_VALUE ) != 0 )
		{
			CsMessageBox( MB_OK, _LAN( "런쳐에서 실행하여 주십시오!!" ) );
			return false;
		}
	}

	CREATE_SINGLETON( Steam::CSteam_Client );
	if( !STEAM_ST.InitSteam() )
	{
		DESTROY_SINGLETON( STEAM_STPTR );
		return false;
	}

	Steam::UserData sUserData;
	STEAM_ST.GetUserData( sUserData );

	std::string userID;
	DmCS::StringFn::FormatA( userID, "%I64d", sUserData.m_cSteamID.ConvertToUint64() );
	GLOBALDATA_ST.SetAccountID( userID );
	GLOBALDATA_ST.SetAccountPW( "SteamUser" );
	GLOBALDATA_ST.SetUserType("STEAM");
	return true;
#else
	return false;
#endif
};

bool Hongkong_Command( std::vector<std::string> const& tokkens, bool bSkipAuth )
{ 
	GLOBALDATA_ST.SetLanguage( GData::eLanguage_Hongkong );
	if( bSkipAuth )
		return true;

	if( tokkens.empty() || tokkens[0].compare( "true" ) != 0 )
	{
		CsMessageBox( MB_OK, _LAN( "런쳐에서 실행하여 주십시오!!" ) );
		return false;
	}

	return true;
};

bool Taiwan_Command( std::vector<std::string> const& tokkens, bool bSkipAuth )
{
	GLOBALDATA_ST.SetLanguage( GData::eLanguage_Taiwan );
	if( bSkipAuth )
		return true;

	if( tokkens.empty() || tokkens[0].compare( "true" ) != 0 )
	{
		CsMessageBox( MB_OK, _LAN( "런쳐에서 실행하여 주십시오!!" ) );
		return false;
	}
	return true;
};

bool Thailand_Command( std::vector<std::string> const& tokkens, bool bSkipAuth )
{
	GLOBALDATA_ST.SetLanguage( GData::eLanguage_Thailand );
	if( bSkipAuth )
		return true;

	if( tokkens.empty() || tokkens[0].compare( "true" ) != 0 )
	{
		CsMessageBox( MB_OK, _LAN( "런쳐에서 실행하여 주십시오!!" ) );
		return false;
	}
	return true;
};

bool SetCommandParameter( LPTSTR lpszCmdLine )
{
	std::string cmdParam;
	DmCS::StringFn::ToMB( lpszCmdLine, cmdParam );

	std::vector<std::string> tokken;
	DmCS::StringFn::Cut( cmdParam, " ", tokken );

	bool bSkipAuth = false;

#ifndef _GIVE
	bSkipAuth = true;
#endif

	switch( GLOBALDATA_ST.GetConnectionCountryType() )
	{
	case GData::eCountry_Aeria:
	case GData::eCountry_GSP:		return USA_Command( tokken, bSkipAuth );
	case GData::eCountry_Kor:		return Kor_Command( tokken, bSkipAuth );
	case GData::eCountry_Steam:		return Steam_Command( tokken, bSkipAuth );
	case GData::eCountry_Thailand:	return Thailand_Command( tokken, bSkipAuth );
	case GData::eCountry_Taiwan:	return Taiwan_Command( tokken, bSkipAuth );
	case GData::eCountry_Hongkong:	return Hongkong_Command( tokken, bSkipAuth );
	default:
		return false;
	}
}

int APIENTRY _tWinMain( HINSTANCE p_hInstance,
                        HINSTANCE p_hPrevInstance,
                        LPTSTR    p_lpszCmdLine,
                        int       p_iCmdShow )
{
	// Install the global crash / assert logger as early as possible so any
	// failure during startup (asset load, pack open, ADO, etc.) lands in the log.
	nsCSDEBUG::CrashLogger::Init( "logs\\crash.log" );

#ifdef USECONSOLE
	FILE	*pStream = NULL; 
	AllocConsole();
	freopen_s(&pStream, "CONIN$", "r", stdin);
	freopen_s(&pStream, "CONOUT$", "w", stderr);
	freopen_s(&pStream, "CONOUT$", "w", stdout);

	//_crtBreakAlloc = 232352;	//{,,msvcr100d.dll}	_CrtSetBreakAlloc(num);
	//_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif	

	int iRet = 0;
#ifdef _GIVE 	
	int nProcessCount = 1;
#else	
	int nProcessCount = 10;
#endif

	// 중복 실행 체크
	if( ProcessKill(nProcessCount) == false )
		return iRet;

	// 문자열 변환 함수 등록
	DmCS::StringFn::SetConvFn(  &App::CWin32App::UnicodeToUTF8,
								&App::CWin32App::UnicodeToMultiByte,
								&App::CWin32App::UTF8ToUnicode,
								&App::CWin32App::MultiByteToUnicode );

	CREATE_SINGLETON(GData::CGlobalData);	// 글로벌 데이터 메니져

#ifdef VERSION_KOR
	GLOBALDATA_ST.SetConnectionCountryType( GData::eCountry_Kor );
#elif defined(VERSION_USA)
	#ifdef VERSION_STEAM
		GLOBALDATA_ST.SetConnectionCountryType( GData::eCountry_Steam );
	#else
		GLOBALDATA_ST.SetConnectionCountryType( GData::eCountry_GSP );
	#endif
#elif defined(VERSION_HK)
	GLOBALDATA_ST.SetConnectionCountryType( GData::eCountry_Hongkong );
#elif defined(VERSION_TW)
	GLOBALDATA_ST.SetConnectionCountryType( GData::eCountry_Taiwan );
#elif defined(VERSION_TH)
	GLOBALDATA_ST.SetConnectionCountryType( GData::eCountry_Thailand );
#endif

	if( !SetCommandParameter(p_lpszCmdLine) )
	{
		DESTROY_SINGLETON(GLOBALDATA_STPTR);	
		return iRet;
	}

	// 
	nBase::SetCurDir2ExeFilePos();

#ifdef SPLASH_LOGO
	CSplash splash( _T( "Data\\NP\\Splash\\Splash.bmp" ), RGB(255, 0, 255));
	splash.ShowSplash();
#endif//SPLASH_LOGO

#ifdef USE_DUMPER
	g_Dumper;
	EnableNetLog( false );
#endif //_GIVE

#ifdef SDM_DEF_XIGNCODE3_20181107	
	if( !XignCode_Initialize() )
		return iRet;
#endif

	g_CriticalLog.Init( _T( "CriticalLog" ), true, false );
	g_CriticalLog.DeleteFile();	

	g_CriticalLog.Log( _T("g_Dumper Create") );

	g_bInstallStart = ( _taccess_s( _T( "UserData" ), 0 ) != 0 );
	
	cProfile x3( VERSION_FILE );
	int nVersion = x3.GetInt("VERSION", "version");
	GLOBALDATA_ST.SetDownloadPatchVersion( nVersion );
	//DUMPLOGA( "DownloadVersion : %d", nVersion);

	TCHAR name[MAX_PATH] = { 0, };
	GetModuleFileName(NULL, name, MAX_PATH);
	WORD wMajor;	WORD wMinor;	WORD wbuild;	WORD wRev;
	DmCS::FileFn::GetFileVersionInfos(name, wMajor, wMinor, wbuild, wRev);

#ifdef USE_DUMPER
	DUMPLOGW( "Client File Name : %s", name);
	DUMPLOGA( "ClientVersoin : %d.%d.%d.%d", wMajor, wMinor, wbuild, wRev);
	std::string exeHash = DmCS::sha256_file( name );
	DUMPLOGA( "Client Hash : %s", exeHash.c_str() );
#endif

	// Gamebryo Init
	CEngine::GBInit();

	g_bUseFilePack = GetUsingFilePack();
	std::list<std::string> vecReadFiles;
	//CsMessageBox(MB_OK, _T("Will read file pack?"));
	if( g_bUseFilePack )
	{
		//CsMessageBoxA(MB_OK, "Trying to read file pack from %s!", GLOBALDATA_ST.GetLoadResourceFile());
		vecReadFiles.push_back(GLOBALDATA_ST.GetLoadResourceFile());
		vecReadFiles.push_back(GLOBALDATA_ST.GetLoadTableFile());
	}

	if( !CsFPS::CsFPSystem::Initialize( g_bUseFilePack, vecReadFiles, false ) )
	{
#ifdef SPLASH_LOGO
		splash.CloseSplash();
#endif
		DESTROY_SINGLETON(GLOBALDATA_STPTR);
		CsMessageBox( MB_OK, L"Table Load False" );
		return iRet;
	}

	g_pResist = NiNew cResist;	
	g_pResist->Init();


#ifdef SPLASH_LOGO	
	Sleep( 2500 );//  Close the splash screen	
	splash.CloseSplash();
#endif//SPLASH_LOGO

	dm_string_t title;
	DmCS::StringFn::Format(title, _T("Client Version %d.%d.%d(%d)"), wMajor, wMinor, wbuild, wRev);
	TCHAR lpszName[] = _T( "DMO" );
	// App Startup
	CREATE_SINGLETON(App::CGameApp);

	if( GAMEAPP_ST.Startup( lpszName, title.c_str(), 0, 0, g_nScreenWidth, g_nScreenHeight, FALSE ) )
	{
#ifdef DEF_CORE_NPROTECT
		if( nProtect_Init() == TRUE )
		{
			nProtect_SetHwnd(GAMEAPP_ST.GetHWnd());
#endif
			iRet = GAMEAPP_ST.Run();

#ifdef DEF_CORE_NPROTECT
		}
#endif
	}

	if( g_pResist != NULL )
	{
#ifdef GUILD_MARK_DOWN
		if( g_pDataMng && g_pDataMng->GetGuildMark() )
			g_pDataMng->GetGuildMark()->ResetCheck();
#endif
		g_pResist->SaveGlobal();
		SAFE_NIDELETE( g_pResist );
	}

#ifdef VERSION_STEAM
	STEAM_ST.ReleaseSteam();
	DESTROY_SINGLETON( STEAM_STPTR );
#endif

	// App Shutdown
	GAMEAPP_ST.Shutdown();
	CsFPS::CsFPSystem::Destroy();

	if( GAMEAPP_STPTR )
		DESTROY_SINGLETON(GAMEAPP_STPTR);	

	if( GLOBALDATA_STPTR )
		DESTROY_SINGLETON( GLOBALDATA_STPTR );

#ifdef DEF_CORE_NPROTECT
	nProtect_End();
#endif

#ifdef SDM_DEF_XIGNCODE3_20181107
	XignCode_UnInitialize();
#endif

#ifdef USECONSOLE
	fclose( pStream );
	FreeConsole();		//할당된 콘솔 해제
	_CrtCheckMemory();
	_CrtMemDumpAllObjectsSince(0);
#endif

	Release();

	return iRet;
}