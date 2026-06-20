
#include "stdafx.h"
#include "Resist.h"
#include "io.h"

cResist*			g_pResist = NULL;

cResist::cResist()
{
	m_szID[ 0 ] = NULL;
	m_szServer[ 0 ] = NULL;
	m_szChar[ 0 ] = NULL;
	m_bLoadChar = false;
//	m_bTutorial = false;
	m_bEnablePortal = false;
	bMovePortal = false;
	m_nDeleteCharIndex = -1;

	dwCharMacro_Ver = 0x00000101;
	dwCharMacro_Ver += 0x00000001;

#ifdef DAMAGE_METER
	dwCharMacro_Ver += 0x00000001;
#endif
#ifdef KEYBOARD_MOVE
	dwCharMacro_Ver += 0x00000001;
#endif
	dwCharMacro_Ver += 0x00000001;
}

void cResist::sGLOBAL::Reset()
{
	SIZE_SET;
	s_dwVersion = RES_GLOBAL_VERSION;

	s_nLeft  = 0;
	s_nTop = 0;

#ifdef _DEBUG
	s_bFullScreen = false;
#else
	s_bFullScreen = true;
#endif

	s_bSaveID = true;
	s_szLastConnectID[ 0 ] = NULL;

	s_bCharOutLine = true;	
	s_nShadowType = SHADOW_ON;	

	s_nWeather = 1;

	s_eTexFilter = (int)NiTexturingProperty::FILTER_TRILERP;
	s_nAntialicing = (DWORD)NiDX9Renderer::FBMODE_MULTISAMPLES_2;

	s_bHSync = true;
	s_bCell = true;

	s_fObjectSightRate = 1.0f;

	s_bUseBright = FALSE;
	s_fBright = 1.0f;

	s_nOptionDefIndex = -1;

	s_bUseMyShader = true;

	s_bUseBarcode = FALSE;

	s_bRightToolTip = TRUE;
	s_bBossWarning = TRUE;

	s_nRefreshTime = 0;
	s_nPostEffect = 0;
	s_SkipSPWTime = 0;
	
	ResetSound();
	ResetResolution();
	ResetInterface();	
}

void cResist::sGLOBAL::SetSkip2ndPassword(BOOL bValue)
{
	if( bValue )
		s_SkipSPWTime = net::net->GetTimeTS();
	else
		s_SkipSPWTime = 0;
}

void cResist::sGLOBAL::ResetSound()
{
	s_fMusic = 0.7f;
	s_bMusic = true;
	s_fSound = 1.0f;
	s_bSound = true;
	s_fSystem = 1.0f;
	s_bSystem = true;

	s_bEnableVoice = true;

	s_bEnableSound_3DMove = false;
}

void cResist::sGLOBAL::ResetResolution()
{
	assert_cs( ( g_pResolutionInfo == NULL ) || ( g_pResolutionInfo->GetResolution( GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN) ) != NULL ) );
	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);

	//if( x < 1024 )
	{
		s_nResolutionWidth = 1024;
		s_nResolutionHeight = 768;
	}
	/*else
	{
		s_nResolutionWidth = x;
		s_nResolutionHeight = y;
	}*/
}

bool cResist::sGLOBAL::CheckResolution()
{
	assert_cs( g_pResolutionInfo != NULL );
	if( g_pResolutionInfo->FindResolutionIndex( g_nScreenWidth, g_nScreenHeight ) != -1 )
		return true;

	int x = GetSystemMetrics(SM_CXSCREEN);
	int y = GetSystemMetrics(SM_CYSCREEN);
	if( g_pResolutionInfo->FindResolutionIndex( x, y ) != -1 )
	{
		s_nResolutionWidth = x;
		s_nResolutionHeight = y;
		return false;
	}

	s_nResolutionWidth = g_pResolutionInfo->GetResolution( 0 )->Width;
	s_nResolutionHeight = g_pResolutionInfo->GetResolution( 0 )->Height;
	return false;
}

void cResist::sGLOBAL::ResetInterface()
{
	s_bMyRenderName			= true;
	s_bOtherRenderName		= true;
	s_bMonsterRenderName	= true;
	s_bNpcRenderName		= true;

	s_bViewHpBar			= true;

	s_bChatBalloone			= true;
	s_bTipBalloone			= true;
	s_bDigimonBalloone		= true;

	s_bFigureTamer			= false;
	s_bFigureDigimon		= false;
	s_bFigureEmployment		= false;
	s_bFigurePat			= false;
	s_eFTLanguage			= nsCsFileTable::KOREA_TRANSLATION;
	s_bRightHandMouse		= true;

	s_bTradeActive			= TRUE;
	s_bPartyActive			= TRUE;
	s_bDualActive			= TRUE;
	s_bGuildInvitation		= TRUE;

	s_bViewLevelEffect		= TRUE;

	s_bCameraEnemyCritical	= TRUE;

	s_eAttackEffectViewMode	= AEM_ALL_ON;

	s_bRightToolTip  = TRUE;
	s_bBossWarning = TRUE;

	//g_pResist->m_CenterBar.Reset();
	if(g_pResist)	g_pResist->m_CenterBar.s_bInitial = false;
	if(g_pResist)	g_pResist->m_CenterBar.s_fAlpha = 0.5f;
	if(g_pResist)	g_pResist->m_CenterBar.s_bIsOpenQuick = false;
#ifdef KEYBOARD_MOVE
	s_bMoveDigimon = false;
	s_bMove_KeyBoard = true;
	s_bMove_Mouse = true;
#endif

	// 기본값 변경 안되던 값들 추가.
	if(g_pResist)	g_pResist->m_Character.Reset();	// 카메라 회전. 테이머와의 거리
	if(g_pCharMng)
	{
		CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();		
		if( pTamerUser )	pTamerUser->ApplySyncOption( 0 );	// 코스튬 보임
	}
}

void cResist::sACCOUNT_GLOBAL::Reset()
{
	SIZE_SET;
	s_dwVersion = RES_ACCOUNT_GLOBAL_VERSION;

	s_nLastConnetServer = 0;
	s_szServerName[ 0 ] = NULL;
}

void cResist::sACCOUNT_SERVER::Reset()
{
	SIZE_SET;
	s_dwVersion = RES_ACCOUNT_SERVER_VERSION;

	s_nLastSelChar = -1;
}


void cResist::sCHAR::Reset()
{
	SIZE_SET;
	s_dwVersion = RES_CHAR_VERSION;	
	
	for( int i=0; i<20; ++i )
	{
		s_bQuestHelperViewID[ i ] = true;
	}

	s_bStaticCamera = false;
	s_nCameraMaxDistLevel = 4;
	s_fStaticCameraRotSpeed = 1.3f;
}

void cResist::sCHAR_HOTKEY::Reset()
{
	SIZE_SET;
	s_dwVersion = g_pResist->dwCharMacro_Ver;
	m_HotKey.Reset();
}


void cResist::sDIGIMON_TALK::Reset()
{
	SIZE_SET;
	s_dwVersion = RES_DIGIMON_TALK_VERSION;	

	s_DigimonTalk.Reset();

}

void cResist::sCHAR_QUICKSLOT::Reset()
{
	SIZE_SET;
	s_dwVersion = RES_CHAR_QUICK_SLOT_VERSION;

	for( int i=0; i<20; ++i )
		s_QuickSlotChar[ i ].Reset();
}
// 
// void cResist::sMAIN::Reset()
// {
// 
// // #ifdef VERSION_USA
// // 	strcpy_s( s_SystemFont, MAX_FILENAME, "data\\font\\tahoma.ttf" );
// // 	strcpy_s( s_TextFont, MAX_FILENAME, "data\\font\\tahoma.ttf" );
// // #elif VERSION_TH
// // 	strcpy_s( s_SystemFont, MAX_FILENAME, "data\\font\\tahoma.ttf" );
// // 	strcpy_s( s_TextFont, MAX_FILENAME, "data\\font\\tahoma.ttf" );
// // #elif VERSION_TW
// // 	strcpy_s( s_SystemFont, MAX_FILENAME, "data\\font\\msjh.ttc" );
// // 	strcpy_s( s_TextFont, MAX_FILENAME, "data\\font\\msjh.ttc" );
// // #elif VERSION_HK
// // 	strcpy_s( s_SystemFont, MAX_FILENAME, "data\\font\\msjh.ttc" );
// // 	strcpy_s( s_TextFont, MAX_FILENAME, "data\\font\\msjh.ttc" );
// // #else
// // 	strcpy_s( s_SystemFont, MAX_FILENAME, "data\\font\\NanumGothicBold.ttf" );
// // 	strcpy_s( s_TextFont, MAX_FILENAME, "data\\font\\NanumGothicBold.ttf" );
// // #endif
// 	
// #ifdef USE_DMO_INI
// 	// 계정
// 	cProfile x("./dmo.ini");
// 	char netLocal[255] = {0,};
// 
// #ifdef VERSION_USA
// 	strcpy_s( netLocal, 255, "network_eng" );
// #elif VERSION_TW
// 	strcpy_s( netLocal, 255, "network_taiwan" );
// #elif VERSION_HK
// 	strcpy_s( netLocal, 255, "network_hongkong" );
// #elif VERSION_TH
// 	strcpy_s( netLocal, 255, "network_tha" );
// #else
// 	strcpy_s( netLocal, 255, "network" );
// #endif
// 
// 	strcpy(net::account_ip, cNetBase::GetIP( x.GetStr(netLocal, "ip") ) );
// 	net::account_port = x.GetInt(netLocal, "port");
// 
// 	if(stricmp("on", x.GetStr(netLocal, "power")))
// 	{
// 		//SetWindowTitle(false);
// 		net::disable = true;
// 	}
// 	else
// 	{
// 		//SetWindowTitle(true);
// 		net::disable = false;
// 	}
// #else //USE_DMO_INI
// 	Csini x( "./__ChangeIP.ini" );
// 	if( ( x.IsFile() == false ) || ( x.GetStr( "network", "ip" ) == NULL ) )
// 	{
// #ifdef VERSION_USA
// 		if( GLOBALDATA_ST.IsCountry( GData::eCountry_Aeria ) )
// 			strcpy_s( s_IP, Language::pLength::ip + 1, cNetBase::GetIP( ACCOUNT_IP_Aeria ) );
// 		else
// #endif
// 			strcpy_s( s_IP, Language::pLength::ip + 1, cNetBase::GetIP( ACCOUNT_IP) );
// 
// 		strcpy_s(net::account_ip, Language::pLength::ip+1, s_IP );
// 		net::account_port = ACCOUNT_PORT;
// 		net::disable = false;
// 	}
// 	else
// 	{			
// 		s_nPort = x.GetInt("network", "port");
// 		strcpy_s( s_IP, Language::pLength::ip + 1, x.GetStr( "network", "ip" ) );
// 		strcpy_s(net::account_ip, Language::pLength::ip+1, s_IP );
// 		net::account_port = s_nPort;
// 		net::disable = false;
// 	}	
// #endif //USE_DMO_INI
// }

void cResist::Delete()
{
	sCHAR_QUICKSLOT* pDel = NULL;
	NiTMapIterator itObj = m_pmapQuickSlot->GetFirstPos();
	while( itObj != NULL )
	{
		const char* pcName;
		m_pmapQuickSlot->GetNext( itObj, pcName, pDel );
		delete pDel;
	}	
	SAFE_NIDELETE( m_pmapQuickSlot );
}

void cResist::Init()
{
	m_pmapQuickSlot = NiNew NiTStringPointerMap< sCHAR_QUICKSLOT* >( 8 );

	// 광역정보 로드
	_LoadGlobal();	
}

void cResist::_LoadGlobal()
{
	m_Global.Reset();

	// 접근 안된다면 패스
	if( _access_s( RES_GLOBAL_PATH, 0 ) != 0 )
		return;

	FILE* fp = NULL;
	fopen_s( &fp, RES_GLOBAL_PATH, "rb" );
	if( fp == NULL )
		return;

	READ_RESIST_FILE( m_Global, RES_GLOBAL_VERSION );
	fclose( fp );
}

void cResist::LoadAccountGlobal( TCHAR const* szID )
{
	_tcscpy_s( m_szID, MAX_FILENAME, szID );
	m_AccountGlobal.Reset();

	TCHAR szAccount[ MAX_PATH ];
	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\AccountG.arf" ), m_szID );

	// 접근 안된다면 패스
	if( _taccess_s( szAccount, 0 ) != 0 )
		return;

	FILE* fp = NULL;
	_tfopen_s( &fp, szAccount, _T( "rb" ) );
	if( fp == NULL )
		return;

	READ_RESIST_FILE( m_AccountGlobal, RES_ACCOUNT_GLOBAL_VERSION );

	fclose( fp );
}

void cResist::LoadAccountServer( TCHAR const* szServer )
{
	SaveAccountGlobal();

	_tcscpy_s( m_szServer, MAX_FILENAME, szServer );
	m_AccountServer.Reset();

	TCHAR szAccount[ MAX_PATH ];
	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\%s\\AccountS.arf" ), m_szID, m_szServer );

	// 접근 안된다면 패스
	if( _taccess_s( szAccount, 0 ) != 0 )
		return;

	FILE* fp = NULL;
	_tfopen_s( &fp, szAccount, _T( "rb" ) );
	if( fp == NULL )
		return;

	READ_RESIST_FILE( m_AccountServer, RES_ACCOUNT_SERVER_VERSION );

	fclose( fp );
}

void cResist::LoadChar( TCHAR const* szChar )
{
	SaveAccountServer();

	m_bLoadChar = true;
	_tcscpy_s( m_szChar, MAX_FILENAME, szChar );	

	size_t nHashCode = CsFPS::GetHashCode( m_szChar );

	TCHAR sz[ MAX_PATH ] = {0,};
	_stprintf_s( sz, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\AccountC.arf" ), m_szID, m_szServer, nHashCode );

	LoadGameEvent();

	m_Character.Reset();
	// 접근 안된다면 패스
	if( _taccess_s( sz, 0 ) == 0 )
	{
		FILE* fp = NULL;
		_tfopen_s( &fp, sz, _T( "rb" ) );
		if( fp != NULL )
		{
			READ_RESIST_FILE( m_Character, RES_CHAR_VERSION );
			fclose( fp );
		}		
	}

	//단축키 정보 로드
	mCharHotKey.Reset();

	_stprintf_s( sz, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\AccountMacroK.arf" ), m_szID, m_szServer, nHashCode );
	// 접근 안된다면 패스
	if( _taccess_s( sz, 0 ) == 0 )
	{
		FILE* fp = NULL;
		_tfopen_s( &fp, sz, _T( "rb" ) );
		if( fp != NULL )
		{
			READ_RESIST_FILE( mCharHotKey, dwCharMacro_Ver );
			fclose( fp );
		}
	}

	GLOBALINPUT_ST.SetHotKey(&mCharHotKey.m_HotKey);


#ifdef USE_CENTER_BAR
	LoadCenterBar( szChar );
#endif	
}

void cResist::sCHAR_CENTERBAR::Reset()
{
	SIZE_SET;
	s_dwVersion = RES_CHAR_CENTERBAR_VERSION;

	for( int i=0; i<12; ++i ) 
		s_CenterBarInfo[ i ].Reset(); 
	
	s_bInitial = false; 
	s_fAlpha = 0.5f; 	
	s_bIsOpenQuick = false;
}

void cResist::sCHAR_CENTERBAR::SetQuickSlotOpenClose(bool bValue)
{
	s_bIsOpenQuick = bValue;
	GAME_EVENT_ST.OnEvent( EVENT_CODE::CHANGE_QUICK_EXPAND, &s_bIsOpenQuick );
}

void cResist::LoadCenterBar( TCHAR const* szCharName )
{
	m_CenterBar.Reset();

	size_t nHashCode = CsFPS::GetHashCode( szCharName );

	TCHAR szCenter[ MAX_PATH ];	
	_stprintf_s( szCenter, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\CenterBar.cs" ), m_szID, m_szServer, nHashCode );

	// 접근 안된다면 패스
	if( _taccess_s( szCenter, 0 ) == 0 )
	{
		FILE* fp = NULL;
		_tfopen_s( &fp, szCenter, _T( "rb" ) );
		if( fp != NULL )
		{
			READ_RESIST_FILE( m_CenterBar, RES_CHAR_CENTERBAR_VERSION );
			fclose( fp );
		}		
	}
}

cResist::sCHAR_QUICKSLOT * cResist::LoadCharQuickSlot( TCHAR const* szCharName )
{
	TCHAR szKey[ 128 ] = { 0, };

	size_t nHashCode = CsFPS::GetHashCode( m_szChar );
	size_t nHashCode2 = CsFPS::GetHashCode( szCharName );

	_stprintf_s( szKey, 128, _T( "%s\\%s\\%u\\%u.qs" ), m_szID, m_szServer, nHashCode, nHashCode2 );

	char cKey[ 128 ] = {0, };
	W2M( cKey, szKey, 128 );

	// 기존에 불러 들인게 있나 검색
	sCHAR_QUICKSLOT* pSrc;
	bool bFind = m_pmapQuickSlot->GetAt( cKey, pSrc );

	// 기존에 불러 들인 정보 있음
	if( bFind == true )
		return pSrc;

	// 기존에 불러 들인 정보 없음
		// 파일이 존재 하는지 찾아 본다.
	TCHAR sz[ MAX_PATH ];
	_stprintf_s( sz, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\%u.qs" ), m_szID, m_szServer, nHashCode, nHashCode2 );

	// 파일이 존재 하지 않는다면 NULL 을 반환
	if( _taccess_s( sz, 0 ) != 0 )
		return NULL;

	FILE* fp = NULL;
	_tfopen_s( &fp, sz, _T( "rb" ) );
	assert_cs( fp != NULL );

	sCHAR_QUICKSLOT* pNew = csnew sCHAR_QUICKSLOT;

	fread( pNew, sizeof( sSTRUCT_HEADER ), 1, fp );
	fread( ( (BYTE*)pNew ) + sizeof( sSTRUCT_HEADER ), pNew->s_dwStructSize, 1, fp );
	fclose( fp );

	if( pNew->s_dwVersion != RES_CHAR_QUICK_SLOT_VERSION )
	{
		delete pNew;
		return NULL;
	}
	m_pmapQuickSlot->SetAt( cKey, pNew );
	return pNew;
}

void cResist::LoadGameEvent()
{
	m_CharEvent.Reset();

	size_t nHashCode = CsFPS::GetHashCode( m_szChar );
	TCHAR sz[ MAX_PATH ]={0,};
	_stprintf_s( sz, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\GameEvent.arf" ), m_szID, m_szServer, nHashCode );

	// 접근 안된다면 패스
	if( _taccess_s( sz, 0 ) != 0 )
		return;	

	FILE* fp = NULL;
	_tfopen_s( &fp, sz, _T( "rb" ) );
	if( fp == NULL )
		return;

	READ_RESIST_FILE( m_CharEvent, RES_GAMEEVENT_VERSION );	

	fclose( fp );
}

void cResist::LoadDigimonTalk( TCHAR const* szDigiName )
{	
	m_Talk.Reset();	

	TCHAR sz[ MAX_PATH ];
	_stprintf_s( sz, MAX_PATH, _T( "UserData\\%s\\DT_%s.arf" ), m_szID, szDigiName );

	// 접근 안된다면 패스
	if( _taccess_s( sz, 0 ) == 0 )
	{
		FILE* fp = NULL;
		_tfopen_s( &fp, sz, _T( "rb" ) );
		if( fp != NULL )
		{
			READ_RESIST_FILE( m_Talk, RES_DIGIMON_TALK_VERSION );
			fclose( fp );
		}
	}	
}

void cResist::SaveGlobal()
{
#ifdef VERSION_USA
	m_Global.s_eFTLanguage = nsCsFileTable::KOREA_TRANSLATION;
#endif

	// 폴더생성
	char cFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( cFolder, MAX_PATH, RES_GLOBAL_PATH );
	nsCSFILE::CreateDirectory( cFolder );

	// 아이디 저장 안한다면 기존에 있던 정보도 날려 버린다
#ifdef USE_ID_SAVE
	if( m_Global.s_bSaveID == false )
		m_Global.ReleaseLastConnectID();
#else
	m_Global.ReleaseLastConnectID();
#endif

	// 글로벌 정보 세이브
	FILE* fp = NULL;
	fopen_s( &fp, RES_GLOBAL_PATH, "wb" );
	if( fp != NULL )
	{
		fwrite( &m_Global, sizeof( sGLOBAL ), 1, fp );
		fclose( fp );
	}

	SaveChar();
}

void cResist::CheckTalk()
{	
	CDigimonEvolveInfo* pFTEvolInfo;
	CDigimonEvolveObj::MAP_IT it;
	CDigimonEvolveObj::MAP_IT itEnd;
	
	char szDestFile[ MAX_PATH ];
	char szSrcFile[ MAX_PATH ];
	char szID[ MAX_FILENAME ];
	W2M( szID, m_szID, MAX_FILENAME );
	DWORD dwID = 0;

	//아구몬 Talk 체크
	pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( CODE_AGUMON );
	it = pFTEvolInfo->m_Map.begin(); 
	itEnd = pFTEvolInfo->m_Map.end();
	for( ;it != itEnd; it++ )
	{
		dwID = it->second->m_dwID;
		sprintf_s( szDestFile, MAX_PATH, "UserData\\%s\\DT_%s.arf", szID, nsCsFileTable::g_pDigimonMng->GetDigimon( dwID )->GetInfo()->s_cSoundDirName );		
		
		if( _access_s( szDestFile, 0 ) != 0 )
		{
			sprintf_s( szSrcFile, MAX_PATH, "Data\\NP\\DefaultTalk\\DT_%s.arf", nsCsFileTable::g_pDigimonMng->GetDigimon( dwID )->GetInfo()->s_cSoundDirName );		
			CopyFileA( szSrcFile, szDestFile, FALSE );			
		}
	}

	//가오몬 Talk 체크
	pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( CODE_GAOMON );
	it = pFTEvolInfo->m_Map.begin(); 
	itEnd = pFTEvolInfo->m_Map.end();
	for( ;it != itEnd; it++ )
	{
		dwID = it->second->m_dwID;
		sprintf_s( szDestFile, MAX_PATH, "UserData\\%s\\DT_%s.arf", szID, nsCsFileTable::g_pDigimonMng->GetDigimon( dwID )->GetInfo()->s_cSoundDirName );		

		if( _access_s( szDestFile, 0 ) != 0 )
		{
			sprintf_s( szSrcFile, MAX_PATH, "Data\\NP\\DefaultTalk\\DT_%s.arf", nsCsFileTable::g_pDigimonMng->GetDigimon( dwID )->GetInfo()->s_cSoundDirName );		
			CopyFileA( szSrcFile, szDestFile, FALSE );			
		}
	}

	//라라몬 Talk 체크
	pFTEvolInfo = nsCsFileTable::g_pEvolMng->GetEvolveInfo( CODE_LALAMON );
	it = pFTEvolInfo->m_Map.begin(); 
	itEnd = pFTEvolInfo->m_Map.end();
	for( ;it != itEnd; it++ )
	{
		dwID = it->second->m_dwID;
		sprintf_s( szDestFile, MAX_PATH, "UserData\\%s\\DT_%s.arf", szID, nsCsFileTable::g_pDigimonMng->GetDigimon( dwID )->GetInfo()->s_cSoundDirName );		

		if( _access_s( szDestFile, 0 ) != 0 )
		{
			sprintf_s( szSrcFile, MAX_PATH, "Data\\NP\\DefaultTalk\\DT_%s.arf", nsCsFileTable::g_pDigimonMng->GetDigimon( dwID )->GetInfo()->s_cSoundDirName );		
			CopyFileA( szSrcFile, szDestFile, FALSE );			
		}
	}

}

void cResist::SaveAccountGlobal()
{
	TCHAR szAccount[ MAX_PATH ];
	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\AccountG.arf" ), m_szID );

	// 폴더생성
	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, szAccount );
	nsCSFILE::CreateDirectory( szFolder );

	FILE* fp = NULL;
	_tfopen_s( &fp, szAccount, _T( "wb" ) );
	if( fp != NULL )
	{
		fwrite( &m_AccountGlobal, sizeof( sACCOUNT_GLOBAL ), 1, fp );
		fclose( fp );
	}	
}

void cResist::SaveAccountServer()
{
	TCHAR szAccount[ MAX_PATH ];
	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\%s\\AccountS.arf" ), m_szID, m_szServer );
	
	// 폴더생성
	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, szAccount );
	nsCSFILE::CreateDirectory( szFolder );

	FILE* fp = NULL;
	_tfopen_s( &fp, szAccount, _T( "wb" ) );
	if( fp != NULL )
	{
		fwrite( &m_AccountServer, sizeof( sACCOUNT_SERVER ), 1, fp );
		fclose( fp );
	}
}

void cResist::SaveChar()
{
	if( m_bLoadChar == false )
		return;

	m_bLoadChar = false;

	size_t nHashCode = CsFPS::GetHashCode( m_szChar );

	TCHAR szAccount[ MAX_PATH ] ={0,};
	// 폴더생성
	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\AccountC.arf" ), m_szID, m_szServer, nHashCode );
	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, szAccount );
	nsCSFILE::CreateDirectory( szFolder );

	// 케릭터 정보
	FILE* fp = NULL;
	_tfopen_s( &fp, szAccount, _T( "wb" ) );
	if( fp != NULL )
	{
		fwrite( &m_Character, sizeof( sCHAR ), 1, fp );
		fclose( fp );
	}

	// 단축키 정보

	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\AccountMacroK.arf" ), m_szID, m_szServer, nHashCode );
	fp = NULL;
	_tfopen_s( &fp, szAccount, _T( "wb" ) );
	if( fp != NULL )
	{
		fwrite( &mCharHotKey, sizeof( sCHAR_HOTKEY ), 1, fp );
		fclose( fp );
	}

#ifdef USE_CENTER_BAR
	SaveCenterBar();
#endif	
	SaveCharQuickSlot();
}

void cResist::SaveDigimonTalk( sDIGIMON_TALK sTalk, TCHAR* szDigiName )
{
	TCHAR szAccount[ MAX_PATH ];
	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\DT_%s.arf" ), m_szID, szDigiName );

	// 폴더생성
	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, szAccount );
	nsCSFILE::CreateDirectory( szFolder );	

	FILE* fp = NULL;
	_tfopen_s( &fp, szAccount, _T( "wb" ) );
	if( fp != NULL )
	{
		fwrite( &sTalk, sizeof( sDIGIMON_TALK ), 1, fp );		
		fclose( fp );
	}

}

void cResist::SaveGameEvent()
{
	size_t nHashCode = CsFPS::GetHashCode( m_szChar );
	
	TCHAR szAccount[ MAX_PATH ];
	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\GameEvent.arf" ), m_szID, m_szServer, nHashCode );

	// 폴더생성
	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, szAccount );
	nsCSFILE::CreateDirectory( szFolder );

	FILE* fp = NULL;
	_tfopen_s( &fp, szAccount, _T( "wb" ) );
	if( fp != NULL )
	{
		fwrite( &m_CharEvent, sizeof( sCHAR_EVENT ), 1, fp );
		fclose( fp );
	}
}

void cResist::sCHAR_EVENT::Reset()
{
	SIZE_SET;
	s_dwVersion = RES_GAMEEVENT_VERSION;

	memset( &s_dwEventSave, 0, sizeof( DWORD ) * 32 );
}

void cResist::sCHAR_EVENT::on( uint nEventIDX )
{
	--nEventIDX;
	uint nBitIDX = nEventIDX % 32;
	uint nArrIDX = nEventIDX / 32;

	s_dwEventSave[nArrIDX] |= (1<<nBitIDX);
}


void cResist::sCHAR_EVENT::off( uint nEventIDX )
{
	--nEventIDX;
	uint nBitIDX = nEventIDX % 32;
	uint nArrIDX = nEventIDX / 32;

	s_dwEventSave[nArrIDX] &= ~(1<<nBitIDX);
}

bool cResist::sCHAR_EVENT::Check( uint nEventIDX )
{
	if( nEventIDX <= 0 || nEventIDX > 1000 )
	{
		assert_csm1( false, L"잘못된 이벤트 번호 : %d", nEventIDX );
		return false;
	}

	uint x = nEventIDX - 1;
	uint nBitIDX = x % 32;
	uint nArrIDX = x / 32;

	if(s_dwEventSave[nArrIDX] & (1<<nBitIDX))
	{
		return true; //완료
	}	
	return false; //미완료	
}

void cResist::SaveCharQuickSlot()
{
	TCHAR szFile[ MAX_PATH ] = {0,};
	TCHAR szAccount[ MAX_PATH ]= {0,};

	size_t nHashCode = CsFPS::GetHashCode( m_szChar );
	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\" ), m_szID, m_szServer, nHashCode );

	// 폴더생성
	nsCSFILE::CreateDirectory( szAccount );

// 	TCHAR szName[ 64 ];
// 	char* cNameOnly;

	sCHAR_QUICKSLOT* pQuickSlot = NULL;
	NiTMapIterator itObj = m_pmapQuickSlot->GetFirstPos();
	while( itObj != NULL )
	{
		const char* pcName;
		m_pmapQuickSlot->GetNext( itObj, pcName, pQuickSlot );

		M2W( szAccount, pcName, MAX_PATH );
		_stprintf_s( szFile, MAX_PATH, _T( "UserData\\%s" ), szAccount );

// 		cNameOnly = nsCSFILE::GetFileName( (char*)pcName );
// 		M2W( szName, cNameOnly, 64 );

// 		_stprintf_s( szFile, MAX_PATH, _T( "%s%s" ), szAccount, szName );

		FILE* fp = NULL;
		_tfopen_s( &fp, szFile, _T( "wb" ) );
		if( fp != NULL )
		{
			fwrite( pQuickSlot, sizeof( sCHAR_QUICKSLOT ), 1, fp );
			fclose( fp );
		}
	}
}

void cResist::SaveCenterBar()
{	
	TCHAR szCenter[ MAX_PATH ]={0,};
	size_t nHashCode = CsFPS::GetHashCode( m_szChar );
	_stprintf_s( szCenter, MAX_PATH, _T( "UserData\\%s\\%s\\%u\\CenterBar.cs" ), m_szID, m_szServer, nHashCode );

	// 폴더생성
	TCHAR szFolder[ MAX_PATH ] = {0, };
	nsCSFILE::GetFilePathOnly( szFolder, MAX_PATH, szCenter );
	nsCSFILE::CreateDirectory( szFolder );

	FILE* fp = NULL;
	_tfopen_s( &fp, szCenter, _T( "wb" ) );
	if( fp != NULL )
	{
		fwrite( &m_CenterBar, sizeof( sCHAR_CENTERBAR ), 1, fp );
		fclose( fp );
	}
}

void cResist::SetCharQuickSlot( TCHAR const* szCharName, sCHAR_QUICKSLOT* pQuickSlot )
{
	TCHAR szKey[ 128 ] = { 0, };
	size_t nHashCode = CsFPS::GetHashCode( m_szChar );
	size_t nHashCode2 = CsFPS::GetHashCode( szCharName );
	_stprintf_s( szKey, 128, _T( "%s\\%s\\%u\\%u.qs" ), m_szID, m_szServer, nHashCode, nHashCode2 );
 	char cKey[ 128 ] = {0, };
 	W2M( cKey, szKey, 128 );

	m_pmapQuickSlot->SetAt( cKey, pQuickSlot );
}

void cResist::CharDelete( TCHAR const* szChar )
{
	m_bLoadChar = false;

	// 퀵슬롯 원본 날려주기 위해서
	if( m_pmapQuickSlot->GetCount() != 0 )
	{
		SaveCharQuickSlot();
	}	

	size_t nHashCode = CsFPS::GetHashCode( szChar );

	TCHAR szAccount[ MAX_PATH ];
	_stprintf_s( szAccount, MAX_PATH, _T( "UserData\\%s\\%s\\%u" ), m_szID, m_szServer, nHashCode );

	if( _taccess_s( szAccount, 0 ) == 0 )
	{
		nsCSFILE::DeleteDirectory( szAccount );
	}
}

void cResist::SetMovePortal(bool bPortal)
{ 
	if(!bPortal)
		GLOBALINPUT_ST.ResetModifier();	//2017-04-12-nova Ctrl+마우스클릭 맵이동시 이동후 키보드 입력 안되는 문제

	bMovePortal = bPortal; 
}