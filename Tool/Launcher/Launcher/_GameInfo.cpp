#include "stdafx.h"
#include "_GameInfo.h"

cGameInfo*	g_GameInfo = NULL;

cGameInfo::cGameInfo()
{
	m_eGameType = GAME_DIGIMONMASTERS;
	m_eServerType = ST_REAL;

	memset( m_szID, 0, sizeof( TCHAR ) * 64 );
	memset( m_szKey, 0, sizeof( TCHAR ) * 128 );
	memset( m_szUsrType, 0, sizeof( TCHAR ) * 8 );
	memset( m_szSrvType, 0, sizeof( TCHAR ) * 8 );
	memset( &m_GameInfo, 0, sizeof( sGame ) * GAME_DIGIMONRPG );
}

bool cGameInfo::IsUserType( TCHAR* pUserType ) const
{
	 if( _tcsicmp( m_szUsrType, pUserType ) == 0 )
		 return true;
	
	 return false;
}

bool cGameInfo::Init( TCHAR* szParm, sConfiguration const& sConfigInfo )
{
	if( NULL == szParm || _tcslen( szParm ) == 0 )
		return false;

	m_GameInfo[ GAME_DIGIMONMASTERS ].s_bPack = true;

	TCHAR* szToken;
	szToken = wcstok_s( NULL, L" ", &szParm );
	if( NULL == szToken )// ID
		return false;

	_tcscpy_s( m_szID, 64, szToken );

	szToken = wcstok_s( NULL, L" ", &szParm );
	if( NULL == szToken )	// KeyValue
		return false;

	_tcscpy_s( m_szKey, 128, szToken );

	szToken = wcstok_s( NULL, L" ", &szParm );
	if( NULL == szToken )	// 게임 타입
		return false;

	int nGameType = _ttoi( szToken ) - 1;
	m_eGameType = (eGameType)( (nGameType < 0 ) ? 0 : nGameType);

	szToken = wcstok_s( NULL, L" ", &szParm );	
	if( NULL == szToken )	// UserType
		return false;

	_tcscpy_s( m_szUsrType, 8, szToken );

	szToken = wcstok_s( NULL, L" ", &szParm );	
	if( NULL == szToken )// 서버 타입
		return false;

	_tcscpy_s( m_szSrvType, 8, szToken );

	// 서버타입 체크
	if( _tcsicmp( m_szSrvType, L"REAL" ) == 0 ) // 본서버
	{
		m_eServerType = ST_REAL;
	}
	else if( _tcsicmp( m_szSrvType, L"BETA" ) == 0 ||
		_tcsicmp( m_szSrvType, L"DEVE" ) == 0) // 베타서버
	{
		m_eServerType = ST_BETA;
	}
	else if( _tcsicmp( m_szSrvType, L"ALPH" ) == 0 ||
		_tcsicmp( m_szSrvType, L"TEST" ) == 0) // 알파서버
	{
		m_eServerType = ST_ALPH;
	}
	else
	{
		g_pDlg->Quit( SERVER_ERROR );
		return false;
	}	

	if( !sConfigInfo.IsUserTypeInfo(m_szUsrType) )
	{
		g_pDlg->Quit( USERTYPE_ERROR );
		return false;
	}
 
 	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szPatchInfo,				MAX_PATH, CLIENT_PATCHINFO_FILE );		// 패치 인포 경로및 파일명	
	//_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szGameDefaultDirectory,	MAX_PATH, DEFALUT_CLIENT_DIR );			// 게임 기본 폴더명
	//_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szGameIconName,			MAX_PATH, CLIENT_ICON_FILE_NAME );		// 아이콘 파일명
	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szRegistryPath,			MAX_PATH, REG_CLIENT_PATH );		// 레지스트리에 기록될 주소

	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szInstallServer,			MAX_PATH, sConfigInfo.GetFullClientDownloadUrl().operator LPCTSTR() );
	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szGameFileName,			MAX_PATH, sConfigInfo.GetRunExeFileName_Launcher().operator LPCTSTR() );			// 실행 파일명
 	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szServerName,			MAX_PATH, sConfigInfo.GetDownloadUrl_PatchFile().operator LPCTSTR() );

	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szHomePageUrl,			MAX_PATH, sConfigInfo.GetHomePageUrl(m_szUsrType).operator LPCTSTR() );
	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szPatchPageUrl,			MAX_PATH, sConfigInfo.GetPatchPageUrl(m_szUsrType).operator LPCTSTR());
	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szMainUrl,				MAX_PATH, sConfigInfo.GetMainPageUrl(m_szUsrType).operator LPCTSTR());
	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szSubUrl,				MAX_PATH, sConfigInfo.GetBanerUrl(m_szUsrType).operator LPCTSTR());
	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szInstallURL,			MAX_PATH, sConfigInfo.GetGameDownloadUrl(m_szUsrType).operator LPCTSTR());
	_tcscpy_s( m_GameInfo[ GAME_DIGIMONMASTERS ].s_szInstallerName,			MAX_PATH, sConfigInfo.GetFullClientFileName(m_szUsrType).operator LPCTSTR() );	// 게임 설치 파일명	
	return true;
}
