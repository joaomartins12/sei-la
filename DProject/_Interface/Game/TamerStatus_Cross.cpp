
#include "stdafx.h"
#include "TamerStatus.h"

#define IF_TAMER_CROSS_ICON_SIZE CsPoint( 32, 32 )
#define IF_TAMER_CROSS_GAUGE_POS	CsPoint( 207, 38 )
#define IF_TAMER_CROSS_GAUGE_SIZE	CsPoint( 9, 326 )

#ifdef CROSSWARS_SYSTEM
void cTamerStatus::sINFO::Delete()
{
	if( CURSOR_ST.IsIcon() == true )
	{
		if( (n8)( CURSOR_ST.GetIconSubInfo1() ) == (n8)s_pCharImage )
		{
			CURSOR_ST.ReleaseIcon();
		}
	}
	g_pCharResMng->CharImageResDelete( &s_pCharImage );
	SAFE_NIDELETE( s_pMainImg );
	SAFE_NIDELETE( s_pNSotImg );
	SAFE_NIDELETE( s_pEnableImg );
	SAFE_NIDELETE( m_pSpriteAniArray );
	
	s_nRank = s_nEnable = s_nDigimonID = s_nModelID = 0;
	s_nIdx= -1;
}
void cTamerStatus::sINFO::CheckModelID( int nModelID )
{
	if( s_nModelID == nModelID )
		return;

	if( CURSOR_ST.IsIcon() == true )
	{
		if( (n8)( CURSOR_ST.GetIconSubInfo1() ) == (n8)s_pCharImage )
		{
			CURSOR_ST.ReleaseIcon();
		}
	}

	g_pCharResMng->CharImageResDelete( &s_pCharImage );
	s_nModelID = nModelID;
	s_pCharImage = g_pCharResMng->CharImageResLoad( s_nModelID );
}


void cTamerStatus::_DeleteCross()
{
	SAFE_NIDELETE( m_pCrossBG );
	SAFE_NIDELETE( m_pCrossRenderTex );
	SAFE_NIDELETE( m_pHouseOpenBtn );
	SAFE_NIDELETE( m_pHouseCloseBtn );
	SAFE_NIDELETE( m_pCrossBtn );

	SAFE_NIDELETE( m_pCrossHouseBG );

	SAFE_NIDELETE( m_pCrownRequareLine );
	SAFE_NIDELETE( m_pCrownGaugeBar );		// 게이지 bar
	SAFE_NIDELETE( m_pCrownGaugeStencil );		// 게이지 스텐실
	SAFE_NIDELETE( m_pPageRadioBtn );
	SAFE_NIDELETE( m_pCrossEnable );
	SAFE_NIDELETE( m_pCrossEnableAni );	
	SAFE_NIDELETE( m_pBlock );

	SAFE_NIDELETE( m_pDigimonName );
	SAFE_NIDELETE( m_pDigimonHeight );
	m_DigimonElement.Delete();

	for( int i=0; i<nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics; ++i ){
		m_pCrossInfoArray[ i ].Delete();
		m_pSelectInfo[i] = NULL;		
	}
	
	SAFE_DELETE_ARRAY( m_pCrossInfoArray );

	// 설정 변수들 초기화.
	m_bSummoning = false;

	m_VecSelectInfo.clear(); // 셀렉트 리스트 초기화
	m_VecCombineInfo.clear();

	// 랜더창 디지몬 초기화
	if( m_pRenderDigimon )
	{
		assert_cs( g_pCharMng );
		g_pCharMng->DeleteTempChar( m_pRenderDigimon );
		m_pRenderDigimon = NULL;
	}	
	
}

void cTamerStatus::_CreateCross()
{	
	m_pCrownRequareLine = NULL;
	m_pCrownGaugeBar = NULL;
	m_pCrownGaugeStencil = NULL;
	m_pCrossEnableAni = NULL;
	m_bViewHouse = false;	
 	m_pPageRadioBtn = NULL;
	m_pCrossEnable = NULL;

	m_VecSelectInfo.clear();
	m_VecCombineInfo.clear();
}

void cTamerStatus::ResetCrossLoader(void)
{
	// 슬롯 상태 초기화.
	int nTHouseCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics;

	for( int i=0; i<nTHouseCount; ++i )
	{
		m_pSelectInfo[ i ] =  NULL;

		cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
		cData_CrossTactics::sCross* pTactics = pCrossTactics->GetTactics( i );
		if(0 != pTactics->s_dwBaseDigimonID)
		{
			DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( pTactics->s_dwBaseDigimonID )->GetInfo()->s_dwModelID;					
			m_pCrossInfoArray[ i ].CheckModelID( dwModelID );
			m_pCrossInfoArray[ i ].s_nDigimonID = pTactics->s_dwBaseDigimonID;

			m_pCrossInfoArray[ i ].s_nEnable = pTactics->s_nEnable;
			m_pCrossInfoArray[ i ].s_nRank = nsCsFileTable::g_pDigimonMng->GetCrossDigimon( pTactics->s_dwBaseDigimonID )->GetInfo()->s_nCrossType;
		}
	}

	// 설정 변수들 초기화.
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
	pCrossTactics->ResetCrossIdx();
	m_VecSelectInfo.clear(); // 셀렉트 리스트 초기화
	m_VecCombineInfo.clear();
}

void cTamerStatus::_InitCross( /*bool IsCrossLoader*/ )
{
	//공통변수
	CsPoint posPage, pos;			//페이지버튼 위치, 아이콘 위치
	int nTHouseCount = 0, nPage = 0, nCInvenCount = 0;	//크로스로더 아이콘 수, 페이지
	int X = 4,Y = 4;					//크로스로더 아이콘 행, 열

	m_pCrossBG = NiNew cSprite;
	m_pCrossBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( IF_CROSSLOADER_WIDTH, IF_CROSSLOADER_HEIGHT ), "CrossWars\\130729\\CrossRoder_ChipWin.tga", false );

	m_pCrossHouseBG = NiNew cSprite;
	m_pCrossHouseBG->Init( NULL, CsPoint::ZERO, CsPoint( IF_CROSSINVEN_WIDTH, IF_CROSSINVEN_HEIGHT ), "CrossWars\\130729\\CrossRoder_DigiState.tga", false );

	m_pHouseOpenBtn = NiNew cButton;
	m_pHouseOpenBtn->Init( m_pRoot, CsPoint( 235, IF_CROSSLOADER_HEIGHT/2 ), CsPoint( 17, 32 ), "DigimonStatus\\Digi_Expen_Rbut.tga", false );
	m_pHouseOpenBtn->SetTexToken( CsPoint( 0, 32 ) );

	m_pHouseCloseBtn = NiNew cButton;
	m_pHouseCloseBtn->Init( m_pRoot, CsPoint( 235, IF_CROSSLOADER_HEIGHT/2 ), CsPoint( 17, 32 ), "DigimonStatus\\Digi_Expen_Lbut.tga", false );
	m_pHouseCloseBtn->SetTexToken( CsPoint( 0, 32 ) );

	m_pCrossEnable = NiNew cSprite;
	m_pCrossEnable->Init( m_pRoot, CsPoint( 125, 360 ), CsPoint( 69, 68 ), "CrossWars\\CodeCrownGauge_Button.tga", false );	

	m_pCrossBtn = NiNew cButton;
	m_pCrossBtn->Init( m_pRoot, CsPoint( 70, 374 ), CsPoint( 51, 46 ), "CrossWars\\130729\\CrossRoder_Btn.tga", false );
	m_pCrossBtn->SetTexToken( CsPoint( 51, 0 ) );

	m_pCrossEnableAni = NiNew cSpriteAni;	
	m_pCrossEnableAni->Init( cSpriteAni::LOOP, m_pRoot, CsPoint( 70, 374 ), CsPoint( 53, 53 ), "CrossWars\\130729\\BtnParticle\\BtnParticle_", "tga", 6, false );
	m_pCrossEnableAni->SetAniTime( 0.08f );
	m_pCrossEnableAni->PlayOff();

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;

	nTHouseCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics;
	nPage = nTHouseCount / IF_CROSSLOADERSLOT_COUNT;
	assert_cs( ( nTHouseCount % IF_CROSSLOADERSLOT_COUNT ) == 0 );

	// 1페이지면 보여주지 않는다.
	if( 1 < nPage){
		m_pPageRadioBtn = NiNew cRadioButton;
		posPage = CsPoint( 186, 226 );
		posPage.x -= nPage*18;
		for( int i=0; i<nPage; ++i )
		{
			ti.SetText( i+1 );
			if( i == 0 )
				m_pPageRadioBtn->Init( m_pRoot, posPage, CsPoint( 15, 15 ), cCheckBox::IMAGE_RADIO_2, &ti, false );
			else
				m_pPageRadioBtn->AddNode( posPage, CsPoint( 15, 15 ), cCheckBox::IMAGE_RADIO_2, &ti );
			posPage.x += 18;
		}	
		m_pPageRadioBtn->SetCheckIndex( 0 );		
	}

	m_pCrossInfoArray = csnew sINFO[ nTHouseCount ];
	m_pSelectInfo = csnew sINFO*[ nTHouseCount ];

	pos = CsPoint( 12, 55 );

// 	int X = 4;
// 	int Y = 4;

	for( int p=0; p<nPage; ++p )
	{
		pos = CsPoint( 12, 55 );	
		for( int y=0; y<Y; ++y )
		{
			for(int  x=0; x<X; ++x )
			{
				int nArrayIdx = (p * IF_CROSSLOADERSLOT_COUNT) + y*X + x;
				if( nArrayIdx < nTHouseCount )
				{
					m_pCrossInfoArray[ nArrayIdx ].Init( CsRect( pos, (CsSIZE)IF_TAMER_CROSS_ICON_SIZE ) );

					cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
					cData_CrossTactics::sCross* pTactics = pCrossTactics->GetTactics( nArrayIdx );
					if(0 != pTactics->s_dwBaseDigimonID)
					{
						DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( pTactics->s_dwBaseDigimonID )->GetInfo()->s_dwModelID;					
						m_pCrossInfoArray[ nArrayIdx ].CheckModelID( dwModelID );
						m_pCrossInfoArray[ nArrayIdx ].s_nDigimonID = pTactics->s_dwBaseDigimonID;

						m_pCrossInfoArray[ nArrayIdx ].s_nEnable = pTactics->s_nEnable;
						m_pCrossInfoArray[ nArrayIdx ].s_nRank = nsCsFileTable::g_pDigimonMng->GetCrossDigimon( pTactics->s_dwBaseDigimonID )->GetInfo()->s_nCrossType;				
					}

					m_pCrossInfoArray[ nArrayIdx ].s_nIdx = nArrayIdx;

					m_pCrossInfoArray[ nArrayIdx ].s_pEnableImg = NiNew cSprite;
					m_pCrossInfoArray[ nArrayIdx ].s_pEnableImg->Init( NULL, CsPoint::ZERO, CsPoint( 42, 40 ), "CrossWars\\Button_Mask.tga", false );


					m_pCrossInfoArray[ nArrayIdx ].m_pSpriteAniArray = NiNew cSpriteAni;	
					m_pCrossInfoArray[ nArrayIdx ].m_pSpriteAniArray->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( 42, 40 ), "DigimonStatus\\Evol\\Ani\\light_", "tga", 11, false );
					m_pCrossInfoArray[ nArrayIdx ].m_pSpriteAniArray->SetAniTime( 0.05f );
					m_pCrossInfoArray[ nArrayIdx ].m_pSpriteAniArray->PlayOn(true);


					m_pCrossInfoArray[ nArrayIdx ].s_pMainImg = NiNew cSprite;
					m_pCrossInfoArray[ nArrayIdx ].s_pMainImg->Init( NULL, CsPoint::ZERO, CsPoint( 42, 40 ), "CrossWars\\2Slot.tga", false );

					m_pCrossInfoArray[ nArrayIdx ].s_pNSotImg = NiNew cSprite;
					m_pCrossInfoArray[ nArrayIdx ].s_pNSotImg->Init( NULL, CsPoint::ZERO, CsPoint( 42, 40 ), "CrossWars\\NSlot.tga", false );


					m_pSelectInfo[ nArrayIdx ] = NULL;

				}				
				pos.x += 46;
			}
			pos.x = 12;
			pos.y += 43;
		}
	}

	m_pBlock = NiNew cSprite;
	m_pBlock->Init( NULL, CsPoint::ZERO, IF_TAMER_CROSS_ICON_SIZE, "Inventory\\Block.tga", false );

	nCInvenCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossInven;
	m_IFIcon = csnew sIFIcon[ nCInvenCount ];

	pos = CsPoint(  175, 132 );
	for( int y=0; y<IF_CROSSINVEN_Y; ++y )
	{
		for(int  x=0; x<IF_CROSSINVEN_X; ++x )
		{
			m_IFIcon[ y*IF_CROSSINVEN_X + x ].Init( CsRect( pos, (CsSIZE)IF_TAMER_CROSS_ICON_SIZE ) );
			pos.x += 40;
		}
		pos.x = 175;
		pos.y += 40;
	}

	{
		m_pCrossRenderTex = NiNew cRenderTex;
		if( m_pCrossRenderTex->Init( CsPoint( 110, 190 ) ) == false )
			SAFE_NIDELETE( m_pCrossRenderTex );


		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10 );
		ti.s_eTextAlign = DT_CENTER;

		m_pDigimonName = NiNew cText;
		m_pDigimonName->Init( m_pRoot, CsPoint( IF_CROSSLOADER_WIDTH+82, 42 ), &ti, false );

		ti.s_eFontSize = CFont::FS_9;
		m_pDigimonHeight = NiNew cText;
		m_pDigimonHeight->Init( m_pRoot, CsPoint( IF_CROSSLOADER_WIDTH+110, 290 ), &ti, false );

	}

	m_pCurHouseBtn = m_pHouseOpenBtn;

	{
		NiStencilProperty* propStencilSrc = NiNew NiStencilProperty;
		propStencilSrc->SetStencilReference( 3 );
		propStencilSrc->SetStencilFunction( NiStencilProperty::TEST_ALWAYS );
		propStencilSrc->SetStencilPassAction( NiStencilProperty::ACTION_REPLACE );	
		propStencilSrc->SetStencilOn( true );

		// CsPoint( 207, 38 )
		m_pCrownGaugeStencil = NiNew cSprite;
		m_pCrownGaugeStencil->InitStencil( m_pRoot, CsPoint::ZERO, IF_TAMER_CROSS_GAUGE_SIZE, true, propStencilSrc );
		m_pCrownGaugeStencil->SetAlpha( 0.0f );	
	}	

	{
		NiStencilProperty* propStencil = NiNew NiStencilProperty;
		propStencil->SetStencilReference( 3 );
		propStencil->SetStencilFunction( NiStencilProperty::TEST_EQUAL );
		propStencil->SetStencilPassAction( NiStencilProperty::ACTION_KEEP );
		propStencil->SetStencilOn( true );

		m_pCrownGaugeBar = NiNew cSprite;
		m_pCrownGaugeBar->InitStencil( m_pRoot, IF_TAMER_CROSS_GAUGE_POS, IF_TAMER_CROSS_GAUGE_SIZE, "CrossWars\\130729\\CrossRoder_GaugeBar.tga", false, propStencil );
	}

	//m_pCrownGaugeBG = NiNew cSprite;
	//m_pCrownGaugeBG->Init( m_pRoot, CsPoint( 218, 15 ), CsPoint( 25, 376 ), "CrossWars\\Gauge.tga", false );	

	m_pCrownRequareLine = NiNew cSprite;
	m_pCrownRequareLine->Init( m_pRoot, CsPoint( 200,0 ), CsPoint( 19, 10 ), "CrossWars\\130729\\CrossRoder_GaugeCheck.tga", false);
	m_pCrownRequareLine->SetVisible(false);

	m_rcCrownGaugeRegion.SetRect( IF_TAMER_CROSS_GAUGE_POS, (CsSIZE)( IF_TAMER_CROSS_GAUGE_SIZE + CsPoint( 11, 0 ) ) );

	m_nPageIndex = 0;

	StatusDigimon();
	
}


void cTamerStatus::_Update_Cross()
{
	cSeparate::UpdateInstance( GetWindowType(), 0 );

	
	int nStartIndex = m_nPageIndex*IF_CROSSLOADERSLOT_COUNT;
	int nEndIndex = nStartIndex + IF_CROSSLOADERSLOT_COUNT;

	for( int i=nStartIndex; i<nEndIndex; ++i )
	{

		if( m_pSelectInfo[i] == &m_pCrossInfoArray[ i ] )
		{
			m_pCrossInfoArray[ i ].m_pSpriteAniArray->Update( g_fDeltaTime );
		}
	}


	m_pCrossEnableAni->Update( g_fDeltaTime );

	// 크로스 테이블 뒤져서 자기가 데리고있는디지몬이 크로스 디지몬이면..
	DWORD dwMyDigimon = g_pCharMng->GetDigimonUser(0)->GetBaseDigimonFTID();
	m_bSummoning = nsCsFileTable::g_pDigimonMng->IsCrossDigimon(dwMyDigimon);	

	if( m_pRenderDigimon == NULL )
		return;

	if( m_pRenderTex == NULL )
		return;

	if( m_pRenderDigimon->IsLoad() )
	{
		// 오브젝트 위치 설정	
		CsNodeObj* pCsNode = m_pRenderDigimon->GetCsNode();
		if( pCsNode != NULL )
		{					
			m_pRenderDigimon->GetProp_Animation()->Update( g_fDeltaTime );
			m_pRenderDigimon->GetProp_Alpha()->Update( g_fDeltaTime );

			float fAniTime = m_pRenderDigimon->GetAniTime();
			pCsNode->m_pNiNode->Update( fAniTime );
		}
	}
	else
	{
		m_pRenderDigimon->CsC_AvObject::Update( g_fDeltaTime );
		m_pRenderDigimon->SetViewSight( true );
	}
	if(m_bSummoning)
	{
		DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( dwMyDigimon )->GetInfo()->s_dwModelID;	


		CsModelData::sINFO* pModelInfo = g_pModelDataMng->GetData( dwModelID )->GetInfo();
		float fTall = pModelInfo->s_fHeight*pModelInfo->s_fScale*g_pCharMng->GetDigimonUser(0)->GetScale()*100.0f;
		TCHAR szTall[ 32 ] = { 0, };
		_stprintf_s( szTall, 32, _T( "%dcm (%d%%)" ), (int)fTall, CsFloat2Int( g_pCharMng->GetDigimonUser(0)->GetScale()*100.0f ) );
		m_pDigimonName->SetText( g_pCharMng->GetDigimonUser(0)->GetName() );
		m_pDigimonHeight->SetText( szTall );
		m_nDigimonLevel = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();
	}

	//if( m_bViewHouse == true )
		//cCreateName::UpdateInstance( cBaseWindow::WT_MAKE_TACTICS, 0 );
}

bool cTamerStatus::_UpdateIcon_ForMouse_CrossLoader()
{
	bool bCheck = false;

	CURSOR::eTYPE eCursor = CURSOR_ST.GetButtonState();
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
	

	switch( m_pCrossBtn->Update_ForMouse() )
	{	
	case cButton::ACTION_CLICK:
		{
			if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS ) {
				return false;
			}

			if( g_pCharMng->GetTamerUser()->IsBattle() == true )
			{
				cPrintMsg::PrintMsg( 16002 );
				return true;
			}
			CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
			if( pUser->IsRide() )
			{
				cPrintMsg::PrintMsg( 30014 );
				return true;
			}
			if(nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
				return true;
			
			if( g_pDataMng->GetServerSync()->IsChageDigimon() == true )
			{
				cWindow::PlaySound( cWindow::SD_MSS1 );
				return true;
			}

			
			{	//크로스워즈도 진화제한구역에선 진화 불가능하도록 수정 chu8820 14.04.01
				int dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
				if( nsCsMapTable::g_pMapRegionMng->IsLimitEvoGroup( dwMapID ) == true )
				{
					CsMap_LimitEvo::LIST* pList = nsCsMapTable::g_pMapRegionMng->GetLimitEvoGroup( dwMapID )->GetList();
					CsMap_LimitEvo::LIST_IT it = pList->begin();
					CsMap_LimitEvo::LIST_IT itEnd = pList->end();
					CsMap_LimitEvo::sINFO* pLimitEvoInfo = NULL;
					for( ; it!=itEnd; ++it )
					{
						pLimitEvoInfo = (*it)->GetInfo();
						NiPoint2 vLE( (float)pLimitEvoInfo->s_nCenterX, (float)pLimitEvoInfo->s_nCenterY );
						if( ( g_pCharMng->GetDigimonUser( 0 )->GetPos2D() - vLE ).Length() < pLimitEvoInfo->s_nRadius )
						{
							cPrintMsg::PrintMsg( 30110 );
							return true;
						}
					}
				}
			}

			cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();

			// 자기가 들고있는게 크로스워즈면.. 해제
			if(m_bSummoning)
			{
				// 렌터 타겟 지운다.
				if( m_pRenderDigimon )
				{
					assert_cs( g_pCharMng );
					g_pCharMng->DeleteTempChar( m_pRenderDigimon );
					m_pRenderDigimon = NULL;
				}
				// 해제 패켓보낸다.
				g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CROSSRECOVER_PARTNER, 0);
				cWindow::PlaySound( cWindow::SD_Bu4 );
				return true;
			}					
			// 크로스워즈 소환이 준비됐다.
			else if(pCrossTactics->GetCrossEnable())//
			{
				// 코드 크라운 체크
				if(_CheckCodeCrown())//
				{
					/*
					int nCnt = 2;
					int nArr[2];
					nArr[0] = 0;
					nArr[1] =1;
					g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CROSSCHANGE_PARTNER, 0, &nCnt,  nArr); 
*/
					int nCnt = pCrossTactics->GetCrossCnt();
					g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CROSSCHANGE_PARTNER, 0, &nCnt, pCrossTactics->GetCrossIdx() ); 				
					cWindow::PlaySound( cWindow::SD_Bu4 );
				}
				else
				{
					// 선택 리셋
					ResetCrossLoader();
					cWindow::PlaySound( cWindow::SD_MSS1 );
				}
				return true;
			}
			else
			{
				cWindow::PlaySound( cWindow::SD_MSS1 );	
				cPrintMsg::PrintMsg( 30590 );
			}
			// 선택 리셋
			ResetCrossLoader();
			bCheck = true;
		}
		break;
	}
	switch( m_pCurHouseBtn->Update_ForMouse() )
	{
	case cButton::ACTION_NONE:
		break;
	case cButton::ACTION_CLICK:
		{
			if( m_pCurHouseBtn == m_pHouseOpenBtn )
			{			
				_OpenHouseWin();
			}
			else
			{
				_CloseHouseWin();
				//cWindow::PlaySound( cWindow::SD_Wi2 );
			}
			bCheck = true;
		}
		break;
	}


	int nStartIndex = m_nPageIndex*IF_CROSSLOADERSLOT_COUNT;
	int nEndIndex = nStartIndex + IF_CROSSLOADERSLOT_COUNT;


	switch( eCursor )
	{	
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int i=nStartIndex; i<nEndIndex; ++i )
			{
				if( ( bCheck == false )&&( m_pCrossInfoArray[ i ].s_IFIcon.MouseLBtnClick( ptMouseLocalPos ) == true ) )
				{
					bCheck = true;
				}
				else
				{
					m_pCrossInfoArray[ i ].s_IFIcon.ReleaseState();
				}
			}			
		}
		break;
	case CURSOR::LBUTTON_PRESS:
		{
			OnMouseOutWindow();

			for( int i=nStartIndex; i<nEndIndex; ++i )
			{
				if( m_pCrossInfoArray[ i ].s_IFIcon.s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( ( bCheck == false )&&( m_pCrossInfoArray[ i ].s_IFIcon.MouseOn( ptMouseLocalPos ) == true ) )
						bCheck = true;
					else
						m_pCrossInfoArray[ i ].s_IFIcon.ReleaseState();
				}
			}
		}
		break;			
	default:
		{
			for( int i=nStartIndex; i<nEndIndex; ++i )
			{
				if( ( bCheck == false )&&( m_pCrossInfoArray[ i ].s_IFIcon.MouseOn( ptMouseLocalPos ) == true ) )
				{					
					bCheck = true;
				}
				else
				{
					m_pCrossInfoArray[ i ].s_IFIcon.ReleaseState();
				}
			}
		}	
	}

	if( m_pPageRadioBtn )
		if(m_pPageRadioBtn->GetSize() > 1){
			if( m_pPageRadioBtn->Update_ForMouse() == cCheckBox::ACTION_CLICK )
			{
				m_nPageIndex = m_pPageRadioBtn->GetCheckIndex();
				return true;
			}
		}

	// 경험치 툴팁
	if( m_rcCrownGaugeRegion.PtInRect( ptMouseLocalPos ) == TRUE && 3 == m_pTabMenu->GetCheckIndex())
	{
		CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
		CTamerUser::sUSER_STAT* pUserStat = (CTamerUser::sUSER_STAT*)pTamerUser->GetBaseStat();

		int nID = pUserStat->GetCR();
		int n1 = pUserStat->GetMaxCR();

		std::wstring wsCR;
		DmCS::StringFn::Format( wsCR, L"%s %d / %d   ( %.3f %% )", UISTRING_TEXT( "TAMERSTATUS_CODECROWN" ).c_str(), nID, n1, (nID / (float)n1 * 100.0f) );

		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( m_rcCrownGaugeRegion.GetPos() + GetRootClient(), CsPoint( 0, 0 ), wsCR.c_str() );
		/*
		TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip(	
			m_rcCrownGaugeRegion.GetPos() + GetRootClient(), 
			m_rcCrownGaugeRegion.GetSize(), 
			TOOLTIP_MAX_SIZE,
			cTooltip::EXP,
			pUserStat->GetCR(), pUserStat->GetMaxCR() );		
*/
		return true;
	}
	return bCheck;
}



cBaseWindow::eMU_TYPE
cTamerStatus::_UpdateIcon_ForMouse_CrossInven()
{
	if( IsShowWindow() == false )
		return MUT_OUT_WINDOW;

	eMU_TYPE muReturn = MUT_OUT_WINDOW;

	CsRect rc( _GetCrossInvenPos(), _GetCrossInvenPos() + _GetCrossInvenSize() );
	CURSOR::eTYPE eCursor = CURSOR_ST.CheckClickBox( rc );

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() ) - CsPoint( IF_CROSSLOADER_WIDTH, 0 );
	

	muReturn = cSeparate::Update_ForMouseInstance( GetWindowType(), 0 );	
	if( muReturn != MUT_OUT_WINDOW )
		return muReturn;

	//CHAT_TEXT_DEGUG(L"X: %d, Y: %d",ptMouseLocalPos.x,ptMouseLocalPos.y);

	int nCInvenCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossInven;
	switch( eCursor )
	{	
	case CURSOR::BUTTON_OUTWINDOW:
		if( m_bMouseInWindow == true )
		{
			OnMouseOutWindow();
			m_bMouseInWindow = false;
		}
		muReturn = MUT_OUT_WINDOW;
		break;
	case CURSOR::LBUTTON_DOWN:		
		{
			for( int i=0; i<nCInvenCount; ++i )
			{
				if( ( muReturn == MUT_OUT_WINDOW )&&( m_IFIcon[ i ].MouseLBtnClick( ptMouseLocalPos ) == true ) )
				{
					m_bMouseInWindow = true;									
				}
				else
				{
					m_IFIcon[ i ].ReleaseState();
				}
			}
			muReturn = MUT_CLICK_FOCUS;	
		}
		
		break;
	case CURSOR::RBUTTON_DOWN:
		{
			for( int i=0; i<nCInvenCount; ++i )
			{
				if( ( muReturn == MUT_OUT_WINDOW )&&( m_IFIcon[ i ].MouseRBtnClick( ptMouseLocalPos ) == true ) )
				{					
					m_bMouseInWindow = true;
				}
				else
				{
					m_IFIcon[ i ].ReleaseState();
				}
			}
			muReturn = MUT_CLICK_FOCUS;
		}
		
		break;
	case CURSOR::LBUTTON_PRESS:
		{
			OnMouseOutWindow();

			for( int i=0; i<nCInvenCount; ++i )
			{
				if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_LCLICK )
				{
					if( ( muReturn == MUT_OUT_WINDOW )&&( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == true ) ){
						m_bMouseInWindow = true;
					}
					else
						m_IFIcon[ i ].ReleaseState();
				}
			}
			muReturn = MUT_CLICK_FOCUS;
		}
		
		break;			
	case CURSOR::RBUTTON_PRESS:
		{
			for( int i=0; i<nCInvenCount; ++i )
			{
				if( m_IFIcon[ i ].s_eState != sIFIcon::MOUSE_RCLICK )
				{
					if( ( muReturn == MUT_OUT_WINDOW )&&( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == true ) ){
						m_bMouseInWindow = true;
					}
					else
						m_IFIcon[ i ].ReleaseState();
				}
			}
			muReturn = MUT_CLICK_FOCUS;
		}
		
		break;
	case CURSOR::LBUTTON_UP:
	case CURSOR::RBUTTON_UP:
	case CURSOR::BUTTON_NONE:
		{			
			for( int i=0; i<nCInvenCount; ++i )
			{
				if( ( muReturn == MUT_OUT_WINDOW )&&( m_IFIcon[ i ].MouseOn( ptMouseLocalPos ) == true ) )
				{					
					m_bMouseInWindow = true;
				}
				else
				{
					m_IFIcon[ i ].ReleaseState();
				}
			}
			muReturn = MUT_CLICK_FOCUS;
		}
		break;
	default:
		m_bMouseInWindow = true;
		muReturn = MUT_NONE;
	}

	// 툴팁용
	{
		for( int i=0; i<nCInvenCount; ++i )
		{
			cItemInfo* pData = g_pDataMng->GetInvenCross()->GetData( i );			

			if( pData->IsEnable() == false )
				continue;
			
			if( m_IFIcon[ i ].s_Rect.PtInRect( ptMouseLocalPos ) == TRUE )
			{
				int nItemType = nsCsFileTable::g_pItemMng->GetItem( pData->GetType() )->GetInfo()->s_nType_L;
				cTooltip::eSLOT eSlot = ( i < nCInvenCount ) ? cTooltip::OPEN_SLOT : cTooltip::CLOSE_SLOT;
				CsPoint pos = m_IFIcon[ i ].GetPos() + GetRootClient();

				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( pos, IF_TAMER_CROSS_ICON_SIZE, TOOLTIP_MAX_SIZE, cTooltip::ITEM, pData->GetType(), cBaseWindow::WT_TAMERSTATUS, eSlot, 0, pData );

				return MUT_CLICK_FOCUS;
			}
		}	
	}	

	if( muReturn == MUT_OUT_WINDOW )
		return muReturn;

	return muReturn;
}


void cTamerStatus::_RenderCross()
{
	m_pCrossBG->Render();
	m_pCurHouseBtn->Render();

	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	CTamerUser::sUSER_STAT* pUserStat = (CTamerUser::sUSER_STAT*)pTamerUser->GetBaseStat();
	// TEST
	//pUserStat->SetCR(1000);
	//pUserStat->SetMaxCR(10000);
	m_pCrownGaugeStencil->Render( IF_TAMER_CROSS_GAUGE_POS + _CalCrownRate(pUserStat->GetCR(), pUserStat->GetMaxCR(), IF_TAMER_CROSS_GAUGE_SIZE ) );

	m_pCrownGaugeBar->Render();

	if(m_pPageRadioBtn){
		m_pPageRadioBtn->Render();
	}
	m_pCrossBtn->Render();	
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
	if(pCrossTactics->GetCrossEnable()){// 활성화 이미지를 랜더링
		//m_pCrossEnable->Render();

		m_pCrossEnableAni->Render();	
	}

	CsPoint pos,pos2;

	int nStartIndex = m_nPageIndex*IF_CROSSLOADERSLOT_COUNT;
	int nEndIndex = nStartIndex + IF_CROSSLOADERSLOT_COUNT;
	
	for( int i=nStartIndex; i<nEndIndex; ++i )
	{
		pos = m_pCrossInfoArray[ i ].s_IFIcon.GetPos() + GetRootClient();		
		if( m_pCrossInfoArray[ i ].s_nEnable != 0 )
			m_pCrossInfoArray[ i ].s_pEnableImg->Render( pos- CsPoint( 4, 4 ), IF_TAMER_CROSS_ICON_SIZE + CsPoint( 8, 8 ) );

		if( m_pCrossInfoArray[ i ].s_pCharImage )
		{
			if(m_bSummoning)
				m_pCrossInfoArray[ i ].s_pCharImage->s_pSmallImg->SetColor( FONT_GLAY );
			else
				m_pCrossInfoArray[ i ].s_pCharImage->s_pSmallImg->SetColor( FONT_WHITE );

			m_pCrossInfoArray[ i ].s_pCharImage->Render_S( pos, IF_TAMER_CROSS_ICON_SIZE );				
		}				
		if( m_pCrossInfoArray[ i ].s_IFIcon.s_eState != sIFIcon::NONE )
			m_pCrossInfoArray[ i ].s_IFIcon.RenderMask( pos, IF_TAMER_CROSS_ICON_SIZE );
		
		if( m_pSelectInfo[i] == &m_pCrossInfoArray[ i ] )
			m_pCrossInfoArray[ i ].m_pSpriteAniArray->Render( pos- CsPoint( 4, 4 ));
	}

	//선택 디지몬 뿌릴 위치
	CsPoint ptDelta = CsPoint( 4, 4 );
	pos2 = GetRootClient() + CsPoint( -40, 30);

	CsPoint size = CsPoint( 40, 40 );

	vppINFO_IT it = m_VecSelectInfo.begin();	
	for( ; it!=m_VecSelectInfo.end(); ++it )
	{

		if(2 == (*(*it))->s_nRank)
		{
			(*(*it))->s_pMainImg->Render( pos2, size );
		}
		else
			(*(*it))->s_pNSotImg->Render( pos2, size );

		(*(*it))->s_pCharImage->Render_S( pos2 + CsPoint( 4, 4 ), IF_TAMER_CROSS_ICON_SIZE );			
		pos2.y += 40;

	}
	
	if(0 != pCrossTactics->GetCrossID())
		m_pCrownRequareLine->Render();

	
	if( m_bViewHouse == true )
		_RenderCrossInven();
}

void cTamerStatus::_RenderCrossInven()
{
	m_pCrossHouseBG->Render(CsPoint(IF_CROSSLOADER_WIDTH ,0 ) + GetRootClient() );

	int nOpenSlotCount = g_pDataMng->GetInvenCross()->GetInvenSlotCount();
	int nCInvenCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossInven;

	cItemInfo* pItemArray = g_pDataMng->GetInvenCross()->GetDataArray();
	cItemInfo* pData;
	CsPoint pos;
	for(int wc = 0 ; wc<nCInvenCount; ++wc )
	{		
		// 위치
		pos = m_IFIcon[ wc ].GetPos() + GetRootClient() + CsPoint(IF_CROSSLOADER_WIDTH,0);	//

		if( wc < nOpenSlotCount)
		{
			pData = &pItemArray[ wc ];//

			// 아이템이 존재 하는가
			if( pData->IsEnable() == true )//
			{
				// 아이템 랜더링 - 숫자 포함
				g_pIconMng->RenderItem( pData, pos, true );//

				// 쿨타임 중이라면 쿨타임 랜더링
				g_pIconMng->RenderTime( pos, 16, nsCsFileTable::g_pItemMng->GetItem( pData->GetType() )->GetInfo() );//

				if( g_pDataMng->IsItemLock( TO_INVENCROSS_SID( wc ) ) == true )
				{
					g_pIconMng->RenderMask( ICONMASK::IF_ICON_MOUSE_DRAG, pos, IF_TAMER_CROSS_ICON_SIZE );
				}
			}

			// 마스크 있다면 마스크 랜더링
			if( m_IFIcon[ wc ].s_eState != sIFIcon::NONE )
			{			
				m_IFIcon[ wc ].RenderMask( pos, IF_TAMER_CROSS_ICON_SIZE );
			}	
		}
		else
		{
			m_pBlock->Render( pos );
		}
	}

	if( m_pCrossRenderTex != NULL && NULL != m_pRenderDigimon )
	{
		m_pCrossRenderTex->BeginRender();
		m_pCrossRenderTex->RenderObject( GET_SUBCAMERA(CAMERA_08), m_pRenderDigimon );	//2015-11-30-nova

		m_pCrossRenderTex->EndRender( NULL, CsPoint( 28, 70 ) +_GetCrossInvenPos() );

		CsPoint pos = CsPoint( _GetCrossInvenPos().x + 28, _GetCrossInvenPos().y + 296 );
		g_pIconMng->RenderNumber( DT_CENTER, NUMBERSPRITE::WHITE, m_nDigimonLevel, pos , CsPoint( 24, 24 ), false );
		m_DigimonElement.Render( CsPoint( _GetCrossInvenPos().x + 65, _GetCrossInvenPos().y + 312 ) );

		m_pDigimonName->Render();
		m_pDigimonHeight->Render();
	}

	//g_pIconMng->RenderTime( m_pShareStashSort->GetDefaultPos() + GetRootClient(), 10.0f, g_pDataMng->GetShareStashSortCoolTime() );

	cSeparate::RenderInstance( GetWindowType(), 0 );
	//cCreateName::RenderInstance( cBaseWindow::WT_MAKE_TACTICS, 0 );
}
void cTamerStatus::_OnMouseOutWindow_Cross()
{
	
}


void cTamerStatus::_OnLButtonUp_Cross( CsPoint pos )
{
	//CsRect rc( _GetCrossInvenPos(), _GetCrossInvenPos() + _GetCrossInvenSize() );
	//CURSOR_ST.CheckClickBox( rc );

	CsPoint ptLocal = MousePosToWindowPos( pos );
	//CHAT_TEXT_DEGUG(L"X: %d, Y: %d",ptLocal.x,ptLocal.y);

	int nStartIndex = m_nPageIndex*IF_CROSSLOADERSLOT_COUNT;
	int nEndIndex = nStartIndex + IF_CROSSLOADERSLOT_COUNT;

	for( int i=nStartIndex; i<nEndIndex; ++i )
	{
		if( ( m_pCrossInfoArray[ i ].s_IFIcon.s_eState == sIFIcon::MOUSE_LCLICK )&&( m_pCrossInfoArray[ i ].s_IFIcon.s_Rect.PtInRect( ptLocal ) ) )
		{
			if(m_bSummoning)
				return;

			cWindow::PlaySound( cWindow::SD_Bu3 );

			if(m_pCrossInfoArray[ i ].s_nDigimonID != 0 )
				(NULL == m_pSelectInfo[ i ])  ?  _SetSelected(i) : _SetRelease(i);
			
			m_pCrossInfoArray[ i ].s_IFIcon.ReleaseState();	
			
			cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();			

			pCrossTactics->ResetCrossIdx();
			pCrossTactics->SetCrossEnable( _GetCrossEnable() );
			if(pCrossTactics->GetCrossEnable()) {m_pCrossEnableAni->PlayOn(true);}

			SelectDigimon();
			return;
		}		
	}

	ptLocal = MousePosToWindowPos( pos ) - CsPoint( IF_CROSSLOADER_WIDTH, 0 );

	int nCInvenCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossInven;
	cItemInfo* pItem;
	for( int i=0; i<nCInvenCount; ++i )
	{
		if( ( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_LCLICK )&&( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			int nInvenIndex = i;
			pItem = g_pDataMng->GetInvenCross()->GetData( nInvenIndex );
			if( pItem->IsEnable() == true )
			{
				if( g_pDataMng->IsItemLock( TO_INVENCROSS_SID( i ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					m_IFIcon[ i ].ReleaseState();
					return;
				}

				int nItemCount = pItem->GetCount();
				assert_cs( nItemCount > 0 );
				// 분할이 아니다. 또는 아이템 갯수가 1이면 무조건 분활이 아니다
				if( ( GLOBALINPUT_ST.IsShift() == false )||( nItemCount == 1 ) 
#ifdef SDM_CASHITEM_TRADE_LIMITED_20170214
					|| cData_Inven::IsTradeLimitedItem( pItem )
#endif
				)
				{
					CURSOR_ST.SetIcon( CURSOR_ICON::CI_CROSS, nInvenIndex, nItemCount, &m_IFIcon[ i ] );
					cSeparate::DeleteInstance( GetWindowType(), 0 );
				}
				// 분활
				else
				{					
					CsPoint posSeparate = m_IFIcon[ i ].GetPos() + _GetCrossInvenPos();	//
					cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_D_NAME, 0 );
					cCreateName::DeleteInstance( cBaseWindow::WT_CHANGE_T_NAME, 0 );					
					cSeparate::NewInstance( GetWindowType(), 0, posSeparate, 1, nItemCount, &m_IFIcon[ i ], i );
				}
			}
		}

		m_IFIcon[ i ].ReleaseState();
	}
}
void cTamerStatus::_OnRButtonUp_Cross( CsPoint pos )
{
	CsPoint ptLocal = MousePosToWindowPos( pos ) - CsPoint( IF_CROSSLOADER_WIDTH, 0 );

	int nCInvenCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossInven;

	for( int i=0; i<nCInvenCount; ++i )
	{
		if( ( m_IFIcon[ i ].s_eState == sIFIcon::MOUSE_RCLICK )&&( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) ) )
		{
			int nInvenIndex = i;
			if( g_pDataMng->GetInvenCross()->GetData( nInvenIndex )->IsEnable() == true )
			{
				if( g_pDataMng->IsItemLock( TO_INVENCROSS_SID( nInvenIndex ) ) == true )
				{
					cPrintMsg::PrintMsg( 10004 );
					m_IFIcon[ i ].ReleaseState();
					return;
				}

				cSeparate::DeleteInstance( GetWindowType(), 0 );

// 				if( _RButtonUp_UI( nInvenIndex ) == false )
// 				{
					_RButtonUp_Npc( nInvenIndex );
//				}
			}
		}

		m_IFIcon[ i ].ReleaseState();
	}
}
void cTamerStatus::_RButtonUp_Npc( int nInvenIndex )
{
	GAME_EVENT_ST.OnEvent( EVENT_CODE::DIGITAMA_SETITEM, &nInvenIndex );

	SAFE_POINTER_RET( g_pGameIF );
	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_STORE ) ||
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SCAN ) ||
		g_pGameIF->IsActiveWindow( cBaseWindow::WT_DIGITAMA_SELL ) )
	{
		return;
	}
	// 동작
	switch( g_pNpcMng->GetZoomNpcType() )
	{
	case cBaseWindow::WT_BARCODE_CARD_SCRATCH:
	case cBaseWindow::WT_TALK:
	case cBaseWindow::WT_QUEST_REV:
	case cBaseWindow::WT_QUEST_COMP:
	case cBaseWindow::WT_NONE:
	case cBaseWindow::WT_TACTICSHOUSE:
		g_pDataMng->SendItemUse( TO_INVENCROSS_SID( nInvenIndex ) );
		break;
	//case cBaseWindow::WT_STORE:
	//	int test = -1;
	//	GAME_EVENT_STPTR->OnEvent( EVENT_CODE::REQUEST_CHECK_ENABLESELL_ITEM, test);
	//	break;	
	case cBaseWindow::WT_MAKE_TACTICS:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_MAKE_TACTICS ) )
		{
			g_pGameIF->GetMakeTactics()->Resist( nInvenIndex );
		}					
		break;
	case cBaseWindow::WT_ELEMENTITEM_CHECK:
	case cBaseWindow::WT_ELEMENTITEM_ATTACH:
		break;
	case cBaseWindow::WT_WAREHOUSE:
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_WAREHOUSE ) )
		{
			// 빈슬롯에 현재 아이템의 갯수 만큼 넣겠다
			g_pGameIF->GetWarehouse()->SetResist( nInvenIndex, -1, 0 );
		}
		break;
	case cBaseWindow::WT_TRADE:
		GAME_EVENT_ST.OnEvent( EVENT_CODE::SEND_TRADE_ITEM_REGIST, &nInvenIndex );
		break;
	case cBaseWindow::WT_UNIONSTORE:
	case cBaseWindow::WT_NEW_GOTCHA_MACHINE:
	case cBaseWindow::WT_MASTERS_MATCHING:
	case cBaseWindow::WT_ENCHANT_OPTION:
	case cBaseWindow::WT_DIGIMON_INCUBATOR:
		break;

	default:
		assert_cs( false );
	}
}
// 
// bool cTamerStatus::_RButtonUp_UI( int nInvenIndex )
// {
// 	if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_PERSONSTORE ) )
// 	{
// 		cPersonStore* pPerson = g_pGameIF->GetPersonStore();
// 		cItemInfo* pItem = g_pDataMng->GetInven()->GetData( nInvenIndex );
// 		
// 		PersonStoreContents::eVALUE_TYPE valueType;
// 		GAME_EVENT_ST.OnEvent( EVENT_CODE::PERSON_GET_VAULETYPE, &valueType);
// 
// 		switch( valueType )
// 		{
// 		case PersonStoreContents::VT_RES_PERSON:
// 		case PersonStoreContents::VT_RES_EMPLOYMENT:
// 			{
// 				if( g_pDataMng->IsTradeItem( pItem ) == true )
// 				{
// 					int nTotalCnt = g_pDataMng->GetInven()->GetCount_Item_ID_Rate( pItem->GetType(), pItem->m_nRate );
// 					if( nTotalCnt > 0 )
// 						cItemSeparate::NewInstance( cBaseWindow::WT_PERSONSTORE, 0, cItemSeparate::SELL_RES, pItem, nTotalCnt, (u8)0, cMoneyIF::DISP_FULL );
// 					else
// 						cPrintMsg::PrintMsg( 30366 );
// 				}				
// 			}
// 			break;
// 		case PersonStoreContents::VT_RES_PERSON_START:
// 		case PersonStoreContents::VT_RES_EMPLOYMENT_START:
// 			// 이미 오픈한 상태에서는 인벤토리 연계가 필요 없다.
// 			break;
// 
// 		case PersonStoreContents::VT_OPEN_PERSON:
// 		case PersonStoreContents::VT_OPEN_EMPLOYMENT:
// 			// 구매나 판매시에는 인벤토리와 연계가 필요 없다.
// 			break;
// 		default:
// 			assert_cs( false );
// 		}
// 
// 		return true;
// 	}
// 
// 	return false;
// }
bool cTamerStatus::_CursorIconLBtnUp_Cross( CURSOR_ICON::eTYPE eIconType, int nIconSlot, int nIconCount )
{
	
	CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() ) - CsPoint( IF_CROSSLOADER_WIDTH, 0 );
	int nCInvenCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossInven;
	for( int i=0; i<nCInvenCount; ++i )
	{
		if( m_IFIcon[ i ].s_Rect.PtInRect( ptLocal ) )
		{
			switch( eIconType )
			{			
			case CURSOR_ICON::CI_INVEN:
				/*
			case CURSOR_ICON::CI_WAREHOUSE:
#ifdef NEW_SHARESTASH
			case CURSOR_ICON::CI_SHARESTASH:
#endif
			*/
			case CURSOR_ICON::CI_CROSS:
				g_pDataMng->SendItemMove( nIconSlot, TO_INVENCROSS_SID( i ), nIconCount );
				break;

				// 같은 타입이 아니라면 빈곳에 넣는다.
			case CURSOR_ICON::CI_TEQUIP:
				{
					
				}
				break;
			case CURSOR_ICON::CI_CHIPSET:
				{
					
				}
				break;
			default:
				cPrintMsg::PrintMsg( 11014 );
				break;
			}
			CURSOR_ST.ReleaseIcon();
			return true;
		}
	}
	
	return false;	
}

void cTamerStatus::_ResetDevice_Cross()
{
	if( m_pCrossBG )
		m_pCrossBG->ResetDevice();

	if( m_pCrossHouseBG )
		m_pCrossHouseBG->ResetDevice();

	if( m_pCrossRenderTex )
	{
		m_pCrossRenderTex->ResetDevice();
	}
	if( m_pHouseOpenBtn )
	{
		m_pHouseOpenBtn->ResetDevice();
	}
	if( m_pHouseCloseBtn )
	{
		m_pHouseCloseBtn->ResetDevice();
	}
	if( m_pCrossBtn )
	{
		m_pCrossBtn->ResetDevice();
	}
	if(m_pCrossEnableAni)
		m_pCrossEnableAni->ResetDevice();

	if(m_pCrownRequareLine)
		m_pCrownRequareLine->ResetDevice();

	if(m_pCrownGaugeBar)
		m_pCrownGaugeBar->ResetDevice();
	if(m_pCrownGaugeStencil)
		m_pCrownGaugeStencil->ResetDevice();

	if(m_pPageRadioBtn)
		m_pPageRadioBtn->ResetDevice();

	if(m_pBlock)
		m_pBlock->ResetDevice();

	if(m_pDigimonName)
		m_pDigimonName->ResetDevice();
	if(m_pDigimonHeight)
		m_pDigimonHeight->ResetDevice();

	m_DigimonElement.ResetDevice();

	for( int i=0; i<nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics; ++i ){
		m_pCrossInfoArray[ i ].s_pEnableImg->ResetDevice();
		m_pCrossInfoArray[ i ].m_pSpriteAniArray->ResetDevice();		
		
		m_pCrossInfoArray[ i ].s_pMainImg->ResetDevice();
		m_pCrossInfoArray[ i ].s_pNSotImg->ResetDevice();
	}
}

void cTamerStatus::StatusDigimon()
{
	// 크로스 테이블 뒤져서 자기가 데리고있는디지몬이 크로스 디지몬이면..
	DWORD dwMyDigimon = g_pCharMng->GetDigimonUser(0)->GetBaseDigimonFTID();
	bool bCombine = nsCsFileTable::g_pDigimonMng->IsCrossCombine(dwMyDigimon);

	if( m_pRenderDigimon )
	{
		assert_cs( g_pCharMng );
		g_pCharMng->DeleteTempChar( m_pRenderDigimon );
		m_pRenderDigimon = NULL;
	}	

	if(bCombine)
	{
		sCREATEINFO prop;
		prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
		prop.s_dwOptionPlag = OP_LIGHT;
		prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();

		m_pRenderDigimon = g_pCharMng->AddTempDigimon( &prop, dwMyDigimon, NiPoint3::ZERO, 0.0f, NULL );
		m_pRenderDigimon->GetProp_Alpha()->SetUseDistAlpha( false );
		m_pRenderDigimon->SetViewSight( true );

		// 선택시 디지몬 보여주기		
		DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( dwMyDigimon )->GetInfo()->s_dwModelID;
		//cData_CrossTactics::sCross* pData = g_pDataMng->GetCrossTactics()->GetTactics( (*m_VecCombineInfo[0])->s_nIdx );

		CsModelData::sINFO* pModelInfo = g_pModelDataMng->GetData( dwModelID )->GetInfo();
		float fTall = pModelInfo->s_fHeight*pModelInfo->s_fScale*g_pCharMng->GetDigimonUser(0)->GetScale()*100.0f;
		TCHAR szTall[ 32 ] = { 0, };
		_stprintf_s( szTall, 32, _T( "%dcm (%d%%)" ), (int)fTall, CsFloat2Int( g_pCharMng->GetDigimonUser(0)->GetScale()*100.0f ) );
		m_pDigimonName->SetText( g_pCharMng->GetDigimonUser(0)->GetName() );
		m_pDigimonHeight->SetText( szTall );
		m_nDigimonLevel = g_pCharMng->GetDigimonUser(0)->GetBaseStat()->GetLevel();


		CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwMyDigimon )->GetInfo();
		m_DigimonElement.Delete();
		int nIndex;
		// - 패밀리 속성
		for( int i=0; i<3; ++i )
		{
			if( pFTDigimon->s_eFamilyType[ i ] == 0 )
				continue;
			nIndex = pFTDigimon->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
			m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Family, nIndex );
		}
		// - 디지몬 속성
		nIndex = pFTDigimon->s_eAttributeType - nsCsDigimonTable::AT_NONE;
		m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, nIndex );

		// - 자연 속성
		if( pFTDigimon->s_eBaseNatureTypes[0] != 0 )
		{
			nIndex = pFTDigimon->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE;
			m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );	
		}
		// - 자연 속성
		if( pFTDigimon->s_eBaseNatureTypes[1] != 0 )
		{
			nIndex = pFTDigimon->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE;
			m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );	
		}
		// - 자연 속성
		if( pFTDigimon->s_eBaseNatureTypes[2] != 0 )
		{
			nIndex = pFTDigimon->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE;
			m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );	
		}
	}
	else
	{
		if( m_pRenderDigimon )
		{
			assert_cs( g_pCharMng );
			g_pCharMng->DeleteTempChar( m_pRenderDigimon );
			m_pRenderDigimon = NULL;
		}		
	}
}

void cTamerStatus::SelectDigimon()
{
	
	if( m_pRenderDigimon )
	{
		assert_cs( g_pCharMng );
		g_pCharMng->DeleteTempChar( m_pRenderDigimon );
		m_pRenderDigimon = NULL;
	}	

	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
	if(0 != pCrossTactics->GetCrossID())
	{
		sCREATEINFO prop;
		prop.s_dwPropPlag = PR_ANI_ENABLE | PR_EFFECT_ENABLE;
		prop.s_dwOptionPlag = OP_LIGHT;
		prop.s_eInsertGeomType = CGeometry::GetCharOutLineType();		

		m_pRenderDigimon = g_pCharMng->AddTempDigimon( &prop, pCrossTactics->GetCrossID(), NiPoint3::ZERO, 0.0f, NULL );
		m_pRenderDigimon->GetProp_Alpha()->SetUseDistAlpha( false );
		m_pRenderDigimon->SetViewSight( true );

		// 선택시 디지몬 보여주기		
		DWORD dwModelID = nsCsFileTable::g_pDigimonMng->GetDigimon( pCrossTactics->GetCrossID() )->GetInfo()->s_dwModelID;
		cData_CrossTactics::sCross* pData = g_pDataMng->GetCrossTactics()->GetTactics( (*m_VecCombineInfo[0])->s_nIdx );

		CsModelData::sINFO* pModelInfo = g_pModelDataMng->GetData( dwModelID )->GetInfo();
		float fTall = pModelInfo->s_fHeight*pModelInfo->s_fScale*pData->s_fScale*100.0f;
		TCHAR szTall[ 32 ] = { 0, };
		_stprintf_s( szTall, 32, _T( "%dcm (%d%%)" ), (int)fTall, CsFloat2Int( pData->s_fScale*100.0f  ) );
		m_pDigimonName->SetText( pData->s_szName );
		m_pDigimonHeight->SetText( szTall );
		m_nDigimonLevel = pData->s_nLevel;

		CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pCrossTactics->GetCrossID() )->GetInfo();
		m_DigimonElement.Delete();
		int nIndex;
		// - 패밀리 속성
		for( int i=0; i<3; ++i )
		{
			if( pFTDigimon->s_eFamilyType[ i ] == 0 )
				continue;
			nIndex = pFTDigimon->s_eFamilyType[ i ] - nsCsDigimonTable::FT_DR;
			m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Family, nIndex );
		}
		// - 디지몬 속성
		nIndex = pFTDigimon->s_eAttributeType - nsCsDigimonTable::AT_NONE;
		m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::DAttribute, nIndex );

		// - 자연 속성
		if( pFTDigimon->s_eBaseNatureTypes[0] != 0 )
		{
			nIndex = pFTDigimon->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE;
			m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );	
		}
		// - 자연 속성
		if( pFTDigimon->s_eBaseNatureTypes[1] != 0 )
		{
			nIndex = pFTDigimon->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE;
			m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );	
		}
		// - 자연 속성
		if( pFTDigimon->s_eBaseNatureTypes[2] != 0 )
		{
			nIndex = pFTDigimon->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE;
			m_DigimonElement.AddIcon( CsPoint( 19, 19 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex );	
		}
		
		CsCrossDigimon::sINFO *info = nsCsFileTable::g_pDigimonMng->GetCrossDigimon( pCrossTactics->GetCrossID() )->GetInfo();

		CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
		CTamerUser::sUSER_STAT* pUserStat = (CTamerUser::sUSER_STAT*)pTamerUser->GetBaseStat();

		// CsPoint( 207, 38 ), CsPoint( 9, 326 ), //CsPoint( 204, 0 ), CsPoint( 19, 10 ),
		CsPoint ptGaugeSize = IF_TAMER_CROSS_GAUGE_SIZE;
		CsPoint ptGaugePos = IF_TAMER_CROSS_GAUGE_POS;
		m_pCrownRequareLine->SetPos(	_CalCrownRate( info->s_nCodeCrownRequire, pUserStat->GetMaxCR(), CsPoint( 0, ptGaugeSize.y ) ) + CsPoint( ptGaugePos.x - 7, ptGaugePos.y - 5 )  	);
		m_pCrownRequareLine->SetVisible(true);
	}
	else
	{
		if( m_pRenderDigimon )
		{
			assert_cs( g_pCharMng );
			g_pCharMng->DeleteTempChar( m_pRenderDigimon );
			m_pRenderDigimon = NULL;
		}
		m_pCrownRequareLine->SetVisible(false);
	}
}

bool cTamerStatus::_GetCrossEnable()
{

	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();

	int nTHouseCount = nsCsFileTable::g_pBaseMng->GetLimit()->s_nMaxCrossTactics;

	m_VecCombineInfo.clear();

	if(1 == m_VecSelectInfo.size())
	{		
		pCrossTactics->SetCrossCnt(1);
		for( int i=0; i < nTHouseCount; ++i )
		{
			if(NULL == m_pSelectInfo[ i ])
				continue;

			pCrossTactics->GetCrossIdx()[0] = m_pSelectInfo[ i ]->s_nIdx;
			pCrossTactics->SetCrossID( m_pSelectInfo[ i ]->s_nDigimonID );
			
			m_VecCombineInfo.push_back(&m_pSelectInfo[ i ]);
			return true;
		}
		return false;
	}

	CsCrossDigimon::MAP* map = nsCsFileTable::g_pDigimonMng->GetCrossCombineMap();
	CsCrossDigimon::MAP_IT it = map->begin();

	for( ; it!=map->end(); ++it )
	{
		if((*it).second->GetInfo()->s_nCrossRank != m_VecSelectInfo.size())
			continue;
		
		int nFind = 0;
		for( int i=0; i < (*it).second->GetInfo()->s_nCrossRank; i++)
		{			
			for( int j=0; j < nTHouseCount; ++j )
			{
				if(NULL == m_pSelectInfo[ j ])
					continue;
				
				if( (*it).second->GetInfo()->s_nCrossDigimon[i] == m_pSelectInfo[ j ]->s_nDigimonID)
				{
					m_VecCombineInfo.push_back(&m_pSelectInfo[ j ]);

					if( m_pSelectInfo[ j ]->s_nEnable)
					{
						nFind++;
						
						pCrossTactics->GetCrossIdx()[i] = m_pSelectInfo[ j ]->s_nIdx;
					}
					continue;
				}				
			}
		}
		if((*it).second->GetInfo()->s_nCrossRank == m_VecCombineInfo.size())
		{	
			std::sort (m_VecCombineInfo.begin(), m_VecCombineInfo.end(), alg);
			
			pCrossTactics->SetCrossID( (*it).second->GetInfo()->s_dwDigimonID );

			if( (*it).second->GetInfo()->s_nCrossRank == nFind)
			{
				pCrossTactics->SetCrossCnt(nFind);
				return true;
			}
			return false;
		}		
	}
	return false;
}

void cTamerStatus::_SetSelected( int i)
{
	vppINFO_IT it = std::find_if (m_VecSelectInfo.begin(), m_VecSelectInfo.end(), alg);
	//if(it != m_VecSelectInfo.end() && 2 == m_pCrossInfoArray[ i ].s_nRank)
		//return;	

	m_pSelectInfo[ i ]  = &m_pCrossInfoArray[ i ];	

	m_VecSelectInfo.push_back(&m_pSelectInfo[ i ]);

	std::sort (m_VecSelectInfo.begin(), m_VecSelectInfo.end(), alg);
}

void cTamerStatus::_SetRelease( int i )
{
	if(NULL == m_pSelectInfo[ i ])
		return;
	vppINFO_IT it = m_VecSelectInfo.begin();
	vppINFO_IT itDel;
	for( ; it!=m_VecSelectInfo.end(); ++it )
	{
		if( m_pSelectInfo[ i ]->s_nDigimonID == (*(*it))->s_nDigimonID){
			itDel = it;
			break;
		}
	}
	m_VecSelectInfo.erase(itDel);
	
	m_pSelectInfo[ i ] = NULL;
}

bool cTamerStatus::_CheckCodeCrown()
{
	cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();		

	if(0 == pCrossTactics->GetCrossID())
		return false;
	CsCrossDigimon::sINFO* pInfo = nsCsFileTable::g_pDigimonMng->GetCrossDigimon(pCrossTactics->GetCrossID())->GetInfo();

	CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
	CTamerUser::sUSER_STAT* pUserStat = (CTamerUser::sUSER_STAT*)pTamerUser->GetBaseStat();

	if(pInfo->s_nCodeCrownRequire <= pUserStat->GetCR())
		return true;
	else
	{
		// 코드 크라운 게이지가 부족합니다.
		cPrintMsg::PrintMsg( 30587);
	}

	return false;
}

CsPoint cTamerStatus::_CalCrownRate(DWORD dwCurCR, DWORD dwMaxCR, CsPoint Size)
{
	// Size 값이 높을수록 0%에 가까움 0(100%) ~ 64(0%)
	//dwExp = 9999;

	DWORD dwCRRate, dwSizeRate;
	CsPoint	Pos = CsPoint::ZERO;

	dwCRRate =  (DWORD)(((float)dwCurCR/dwMaxCR) * 100);
	dwSizeRate = (DWORD)((Size.y * 0.01) * dwCRRate);
	Pos = CsPoint(0, (Size.y - dwSizeRate));

	return Pos;
}
#endif