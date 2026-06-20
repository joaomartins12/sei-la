#include "StdAfx.h"
#include "Loading.h"

CLoading* g_pLoading = NULL;

CLoading::eSTATE	CLoading::g_eState = CLoading::NORMAL;
int					CLoading::g_nStateValue = 0;

CLoading::CLoading(void)
{
	g_eLoopPlayState = LPS_LOADING;

	m_pBack = NULL;
	m_pLoadBar = NULL;
	m_pTip = NULL;
	m_pLoading = NULL;
	m_pPcBang = NULL;
	m_pMatchBG = NULL;
	m_pMatch_UserBG = NULL;
	m_bUseStartFadeIn = true;

#ifdef LOADING_SUB_NOTICE
	m_pSubNotice = NULL;
#endif

	m_nStep = 0;

	switch( g_eState )
	{
	case NORMAL:
		Init_Normal();
		break;
	case WAIT_MATCH:
		Init_WaitMatch();
		break;
	default:
		assert_cs( false );
	}	
}

CLoading::~CLoading(void)
{
	Destroy();
}


void CLoading::Destroy()
{
	SAFE_NIDELETE( m_pBack );
	SAFE_NIDELETE( m_pLoadBar );
	SAFE_NIDELETE( m_pTip );
	SAFE_NIDELETE( m_pLoading );
	SAFE_NIDELETE( m_pPcBang );
	SAFE_NIDELETE( m_pMatchBG );
	SAFE_NIDELETE( m_pMatch_UserBG );

	for( int i=0; i<2; ++i )
	{
		for( int j=0; j<4; ++j )
		{
			m_UserArray[ i ][ j ].s_Name.Delete();
		}
	}

#ifdef LOADING_SUB_NOTICE
	SAFE_NIDELETE( m_pSubNotice );
#endif

	m_pTipString.Delete();

	// 어떠한 로딩이든 제거될땐 원상 복귀
	CLoading::g_eState = CLoading::NORMAL;
	CLoading::g_nStateValue = 0;
}

void CLoading::Init_Normal()
{
	char cFileName[ MAX_PATH ];		
	sprintf_s( cFileName, MAX_PATH, "loading\\loading_%d.dds", net::next_map_no );	

	if( m_pBack == NULL )
	{
		m_pBack = NiNew cSprite;
		m_pBack->Init( NULL, CsPoint::ZERO, CsPoint( 1024, 768 ), cFileName, false );	
	}	

	m_pPcBang = NiNew cSprite;
	m_pPcBang->Init( NULL, CsPoint::ZERO, CsPoint( 146, 125 ), "Event\\PC_Mark.tga", false );
		
	m_pLoadBar = NiNew cProgressBar;
	m_pLoadBar->Init( NULL, cProgressBar::BAR_1IMAGE, CsPoint( 111, 734 ), CsPoint( 804, 11 ), CsPoint::ZERO, "loading\\Bar.tga", true );

	m_pLoadBar->SetRange( CsPoint( 0, 100 ) );
	m_pLoadBar->SetPos( 0 );		

	CsLoading_Tip* pFTTip = nsCsFileTable::g_pTalkMng->GetRandom_LoadingTip( 99 );
	assert_cs( pFTTip != NULL );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_12 );
	ti.s_Color = NiColor( 253.0f/ 255.0f, 1.0f, 0.0f );
	ti.s_eTextAlign = DT_CENTER;	

	float fTipRate = (float)620/ (float)DEFAULT_SCREEN_WIDTH;
	int nWidth = (int)( fTipRate*g_nScreenWidth );
	g_pStringAnalysis->Cut( &m_pTipString, nWidth, pFTTip->GetInfo()->s_szLoadingTip, &ti );

	if( m_pTipString.GetSize() > 0 )
	{
		cString::sTEXT* pText = (cString::sTEXT*)m_pTipString.GetString( 0 )->FindElement( cString::sELEMENT::TEXT );
		if( pText != NULL )
			m_TextSize = pText->s_ptSize / 2;
		else
			m_TextSize = CsPoint::ZERO;
	}
	else
	{
		m_TextSize = CsPoint::ZERO;
	}

	int nCurIndex = rand() % 3 + 1;
	sprintf_s( cFileName, MAX_PATH, "loading\\tip_0%d.tga", nCurIndex );		

	m_pTip = NiNew cSprite;
	m_pTip->Init( NULL, CsPoint( -25, -110 ), CsPoint( 278, 278 ), cFileName, false );	

	m_pLoading = NiNew cText;	
	ti.s_eFontSize = CFont::FS_16; 
	ti.s_Color = FONT_WHITE;
	ti.s_eTextAlign = DT_CENTER;			
	ti.SetText( _T( "Loading...." ) );	
	m_pLoading->Init( NULL, CsPoint( 1024/2, 705 ), &ti, true );

	float fRate = (float)550 / (float)DEFAULT_SCREEN_WIDTH;
	int x = (int)( fRate*g_nScreenWidth );
	fRate = (float)647 / (float)DEFAULT_SCREEN_HEIGHT;
	int y = (int)( fRate*g_nScreenHeight );	
	m_DefaultPoint = CsPoint( x, y );

#ifdef LOADING_SUB_NOTICE
	m_pSubNotice = NiNew cSprite;
	m_pSubNotice->Init( NULL, CsPoint( 38, 278 ), CsPoint( 355, 205 ), "Loading\\SubNotice.bmp", true );	
#endif

	// 배틀 로드인가
	if( g_pDataMng->GetBattle()->IsRenderMember() )
	{
		cData_Battle* pDataBattle = g_pDataMng->GetBattle();

		m_pMatchBG = NiNew cSprite;
		m_pMatchBG->Init( NULL, CsPoint::ZERO, CsPoint( 719, 505 ), "Battle_G\\Loading\\Win.tga", false );

		m_pMatch_UserBG = NiNew cSprite;
		m_pMatch_UserBG->Init( NULL, CsPoint::ZERO, CsPoint( 208, 36 ), "Battle_G\\Loading\\UserBG.tga", false );

		int x = 78;
		for( int i=0; i<2; ++i )
		{
			for( int j=0; j<4; ++j )
			{
				m_UserArray[ i ][ j ].s_ptBGPos = CsPoint( x, 201 + 61*j );
				m_UserArray[ i ][ j ].s_bEnable = false;
				m_UserArray[ i ][ j ].s_bLoadComp = false;
			}
			x += 363;
		}
		
		ti.s_eTextAlign = DT_CENTER;
		
		int nCnt = pDataBattle->GetMyTeamCount();
		for( int i=0; i<nCnt; ++i )
		{
			ti.SetText( pDataBattle->GetMyTeam( i )->s_szName );
			m_UserArray[ 0 ][ i ].s_Name.Init( NULL, CsPoint::ZERO, &ti, false );
			m_UserArray[ 0 ][ i ].s_bEnable = true;
		}

		nCnt = pDataBattle->GetOtherTeamCount();
		for( int i=0; i<nCnt; ++i )
		{
			ti.SetText( pDataBattle->GetOtherTeam( i )->s_szName );
			m_UserArray[ 1 ][ i ].s_Name.Init( NULL, CsPoint::ZERO, &ti, false );
			m_UserArray[ 1 ][ i ].s_bEnable = true;
		}

		g_pDataMng->GetBattle()->SetRenderMember( false );
	}
}

void CLoading::Init_WaitMatch()
{
	char cFileName[ MAX_PATH ];		
	sprintf_s( cFileName, MAX_PATH, "loading\\loading_%d.dds", g_nStateValue );	

	m_pBack = NiNew cSprite;
	m_pBack->Init( NULL, CsPoint::ZERO, CsPoint( 1024, 768 ), cFileName, false );
}

void CLoading::ChangeState( eSTATE next )
{
	switch( g_eState )
	{	
	case WAIT_MATCH:
	case EXIT_WAIT_MATCH:
		{
			switch( next )
			{
			case NORMAL:
				m_bUseStartFadeIn = false;
				Init_Normal();
				break;
			case EXIT_WAIT_MATCH:				
				break;
			default:
				assert_cs( false );
			}
		}
		break;
	default:
		assert_cs( false );
	}

	g_eState = next;
}

void CLoading::Render()
{
	switch( g_eState )
	{
	case NORMAL:
		_Render_Normal();
		break;
	case EXIT_WAIT_MATCH:
	case WAIT_MATCH:
		_Render_WaitMatch();
		break;
	default:
		assert_cs( false );
	}
}

void CLoading::_Render_Normal()
{
	m_pBack->Render( CsPoint( g_nScreenWidth/2 - 1024/2, g_nScreenHeight/2 - 768/2 ) );	
	m_pLoadBar->Render();
	m_pLoading->Render();  			

	if( m_pMatchBG )
	{
		CsPoint ptParent = CsPoint( g_nScreenWidth/2 - 719/2, g_nScreenHeight/2 - 505/2 );
		m_pMatchBG->Render( ptParent );

		for( int i=0; i<2; ++i )
		{
			for( int j=0; j<4; ++j )
			{
				if( m_UserArray[ i ][ j ].s_bEnable == false )
					continue;

				CsPoint pos = m_UserArray[ i ][ j ].s_ptBGPos + ptParent;
				m_pMatch_UserBG->Render( pos );
				m_UserArray[ i ][ j ].s_Name.Render( pos + CsPoint( 105, 8 ), DT_CENTER );
			}
		}		
	}
	else
	{
		if( g_pDataMng->GetPCbang()->GetPcBang() == true )
			m_pPcBang->Render( CsPoint( g_nScreenWidth/2 - 1024/2, g_nScreenHeight/2 - 768/3 ) );

		m_pTip->Render( CsPoint( m_DefaultPoint.x - m_TextSize.x - 143, m_DefaultPoint.y - 70 ) ); 
		m_pTipString.Render( m_DefaultPoint - CsPoint( m_TextSize.x, 0 ) + CsPoint( 30, 0 ) , 5 );	


#ifdef LOADING_SUB_NOTICE
		m_pSubNotice->Render();
#endif
	}
}

void CLoading::_Render_WaitMatch()
{
	m_pBack->Render( CsPoint( g_nScreenWidth/2 - 1024/2, g_nScreenHeight/2 - 768/2 ) );
}

// =========================================================
// 100%로를 기준으로 로딩바 다음 단계 설정
// =========================================================
void CLoading::NextStep(int nStep)
{
	m_nStep = nStep;

	if( m_pLoadBar )
		m_pLoadBar->SetPos( m_nStep );

	g_pRenderer->SetBackgroundColor( NiColorA::BLACK );
	if( g_pEngine->StartRender( CEngine::PE_NONE ) == true )
	{
		g_pEngine->ScreenSpace();
		Render();
		g_pEngine->EndRender();
	}
}

bool CLoading::DataProcess()
{	
	if( m_bUseStartFadeIn &&( g_pFade == NULL ) )
	{
		g_pFade = NiNew CFade( FADE_IN );
		assert_cs( g_pFade );
	}

	// 페이드 인
	while( g_pFade )
	{
		g_pEngine->Update();
		g_pSoundMgr->Update( (float)g_fAccumTime );
		g_pRenderer->SetBackgroundColor( NiColorA::BLACK );
		if( g_pEngine->StartRender( CEngine::PE_NONE ) == true )
		{
			g_pEngine->ScreenSpace();
			Render();
			g_pEngine->EndRender();
		}		
	}

	bool bReturn;
	switch( g_eState )
	{
	case NORMAL:
		bReturn = _DataProcess_Normal();
		break;
	case WAIT_MATCH:
		bReturn = _DataProcess_WaitMatch();
		break;
	default:
		assert_cs( false );
	}
	return bReturn;
}

bool CLoading::_DataProcess_Normal()
{
#ifdef LOADING_PROGRESS		
	NextStep(1);
#else
	NextStep(10);
#endif

	if( g_pGameIF == NULL )
		cGameInterface::GlobalInit();
#ifndef LOADING_PROGRESS	
	NextStep(25);
#endif
	bool bReturn = _Load_Data_Normal();	
	assert_cs( g_pFade == NULL );
	return bReturn;
}

bool CLoading::_DataProcess_WaitMatch()
{
	NextStep( 0 );
	bool bReturn = _Load_Data_WaitMatch();
	assert_cs( g_pFade == NULL );
	return bReturn;
}

bool CLoading::_Load_Data_Normal()
{
#ifdef MAP_CAMERA_DIST_CONTROL
	int nCamMaxLv = 0;
	if( nsCsGBTerrain::g_pCurRoot != NULL )
		nCamMaxLv = nsCsMapTable::g_pMapListMng->GetList( net::next_map_no )->GetInfo()->s_nCamera_Max_Level;
	if( nCamMaxLv == 0 )
		nCamMaxLv = g_pResist->m_Character.s_nCameraMaxDistLevel;
#else
	int nCamMaxLv = g_pResist->m_Character.s_nCameraMaxDistLevel;
#endif

	float fMaxDist = g_Camera.GetDistLevel( nCamMaxLv );
	//float fCurDist = g_Camera.GetDist();

	// 카메라 셋팅
	g_Camera.SetStaticCamera( g_pResist->IsStaticCamera() == TRUE );

	sCAMERAINFO ci;	
	ci.s_fDist = fMaxDist;
	ci.s_fInitRoll = NI_PI;
	ci.s_fInitPitch += NI_PI*0.4f;
	ci.s_fFarPlane = 200000.0f;
	g_Camera.Reset( &ci );
	g_Camera.SetDistLevel( nCamMaxLv, false );
	g_Camera.SetRotationLimit( -CsD2R( 35 ), CsD2R( 70 ) );
	g_Camera.SetDeltaHeight( 120.0f );
#ifdef LOADING_PROGRESS	
	NextStep(56);

	// 쓰레드 리소스 매니져의 외부포인터들 없애 준다
	g_pThread->GetResMng()->ReleaseConnetTerrain();
	NextStep(58);

	g_pMngCollector->ResetMap();
	NextStep(60);
	g_pMngCollector->LoadTerrain( net::next_map_no );
	g_pRenderer->GetBackgroundColor( m_OrgBackColor );

	NextStep(72);

	// 쓰레드 리소스 매니져의 외부포인터 재 연결
	g_pThread->GetResMng()->ApplyConnetTerrain();

	NextStep(80);
#else
	NextStep(40);

	// 쓰레드 리소스 매니져의 외부포인터들 없애 준다
	g_pThread->GetResMng()->ReleaseConnetTerrain();

	g_pMngCollector->ResetMap();
	g_pMngCollector->LoadTerrain( net::next_map_no );
	g_pRenderer->GetBackgroundColor( m_OrgBackColor );

	NextStep(70);

	// 쓰레드 리소스 매니져의 외부포인터 재 연결
	g_pThread->GetResMng()->ApplyConnetTerrain();

	NextStep(95);
#endif
	g_pMngCollector->LoadChar( net::next_map_no );

	// 윈도우 닫아 주자 ?
	NextStep(100);

	// 서버에 동기화
	if( net::game && ( g_bFirstLoding == false ) )
	{
		net::game->SendChangeServer();
	}
	else if( net::gate )
	{
		net::gate->SendChangeServer();
	}

	/*CsTimeSeq time;
	time.SetDeltaTime( 11000 );
	time.OffReset();*/

	bool bLoadComp = false;
	while( bLoadComp == false )
	{
		Sleep( 50 );
		/*if( time.IsEnable() == true )
		{
		SetNextGameStep( GAME_EXIT );
		CsMessageBox( MB_OK, _LAN( "서바와의 응답이 없어서 게임을 종료 합니다." ) );
		return false;
		}*/
		net::execute();

		if( g_eLoopPlayState != LPS_LOADING )
		{
			switch( nsCsGBTerrain::g_nSvrLibType )
			{
#ifdef INSTANT_DUNGEON
			case nLIB::SVR_DUNGEON:
#endif
			case nLIB::SVR_GAME:
				bLoadComp = true;
				break;
			case nLIB::SVR_BATTLE:
				bLoadComp = ( IsNetOff() == true ) ? true : g_pDataMng->GetBattle()->IsMapMoveComp();
				break;
			default:
				assert_cs( false );
			}
		}
	}

	g_pMngCollector->LoadCompleate( net::next_map_no );

	// 페이드 아웃
	assert_cs( g_pFade == NULL );
	g_pFade = NiNew CFade( FADE_OUT, 0.3f );
	assert_cs( g_pFade );

	while( g_pFade )
	{
		g_pEngine->Update();
		g_pSoundMgr->Update( (float)g_fAccumTime );
		g_pRenderer->SetBackgroundColor( NiColorA::BLACK );
		if( g_pEngine->StartRender( CEngine::PE_NONE ) == true )
		{
			g_pEngine->ScreenSpace();
			Render();
			g_pEngine->EndRender();
		}
		net::execute();
	}
	g_pRenderer->SetBackgroundColor( m_OrgBackColor );
	g_pResist->SetEnablePortal( true );

	return true;
}

bool CLoading::_Load_Data_WaitMatch()
{
	while( g_eState == WAIT_MATCH )
	{
		Sleep( 100 );
		net::execute();
	}

	if( g_eState == EXIT_WAIT_MATCH )
	{
		// 페이드 아웃
		assert_cs( g_pFade == NULL );
		g_pFade = NiNew CFade( FADE_OUT, 0.3f );
		assert_cs( g_pFade );

		while( g_pFade )
		{
			g_pEngine->Update();
			g_pSoundMgr->Update( (float)g_fAccumTime );
			g_pRenderer->SetBackgroundColor( NiColorA::BLACK );
			if( g_pEngine->StartRender( CEngine::PE_NONE ) == true )
			{
				g_pEngine->ScreenSpace();
				Render();
				g_pEngine->EndRender();
			}
			net::execute();
		}
		g_pRenderer->SetBackgroundColor( m_OrgBackColor );
		g_pResist->SetEnablePortal( true );
	}
	return ( g_eState == NORMAL );
}

void CLoading::LoadComp( TCHAR* szLoadCompUser )
{
	for( int i=0; i<2; ++i )
	{
		for( int j=0; j<4; ++j )
		{
			if( m_UserArray[ i ][ j ].s_bEnable == false )
				continue;

			if( _tcscmp( m_UserArray[ i ][ j ].s_Name.GetText(), szLoadCompUser ) != 0 )
				continue;

			m_UserArray[ i ][ j ].s_Name.SetColor( FONT_GREEN );
			m_UserArray[ i ][ j ].s_bLoadComp = true;
			NextStep( m_nStep );
			goto END_SEARCH;
		}
	}

END_SEARCH:


	// 전원의 로드가 완료 되었다면 1초 슬립
	for( int i=0; i<2; ++i )
	{
		for( int j=0; j<4; ++j )
		{
			if( m_UserArray[ i ][ j ].s_bEnable == false )
				continue;

			if( m_UserArray[ i ][ j ].s_bLoadComp == false )
				return;
		}
	}

	Sleep( 1000 );
}