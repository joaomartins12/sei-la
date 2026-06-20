
#include "stdafx.h"
#include "TargetWindow.h"


cTargetWindow::cTargetWindow()
{
	m_pTargetObject = NULL;
// 	m_pPartyInfo = NULL;
// 	m_pPartyInfoStat = NULL;
	m_pWindowBG = NULL;	

#ifdef TARGET_WIN_MULTYLINE
	m_pWindowBG_2 = NULL;
#endif
}

void cTargetWindow::Destroy()
{
	cBaseWindow::Delete();

	m_pTargetObject = NULL;
// 	m_pPartyInfo = NULL;
// 	m_pPartyInfoStat = NULL;	
}

void cTargetWindow::DeleteResource()
{
	DeleteScript();

	SAFE_NIDELETE( m_pHPBar );
	SAFE_NIDELETE( m_pWindowBG );
	SAFE_NIDELETE( m_pWindowBG_Mask );
#ifdef TARGET_WIN_MULTYLINE
	SAFE_NIDELETE( m_pWindowBG_2 );
	SAFE_NIDELETE( m_pWindowBG_Mask_2 );
#endif
	SAFE_NIDELETE( m_pLevelBG );
	SAFE_NIDELETE( m_pLevelText );
	SAFE_NIDELETE( m_pCMonsterMark );
	SAFE_NIDELETE( m_pCMonsterLevel );
	SAFE_NIDELETE( m_pBMonsterMark );
	SAFE_NIDELETE( m_pBMonsterLevel );

#ifdef IF_TARGET_ELEMENT
	for( int i=0; i<COMPARE_MAX; ++i )
		SAFE_NIDELETE( m_pCompareSprite[ i ] );
#else
	SAFE_NIDELETE( m_pUnknown );
#endif


	for( int i=0; i<3; ++i ){
		SAFE_NIDELETE( m_pNatureOutLineSprite[i] );	
		SAFE_NIDELETE( m_pAttributeRateSprite[i] );
	}
	m_AttributeIcon.Delete();
	m_NatureIcon.Delete();
	
	m_MonsterAI.Delete();

	//===============================================================================================
	//
	//		시간
	//
	//===============================================================================================
	SAFE_NIDELETE( m_pTimeBar );
	SAFE_NIDELETE( m_pTimeBarBG );
	SAFE_NIDELETE( m_pTimeBarBGMask );
	SAFE_NIDELETE( m_pTimeDeco );
}

void cTargetWindow::Create( int nValue /* = 0  */)
{
	if( cBaseWindow::Init() == false )
		return;

	assert_cs( m_pTargetObject == NULL );
// 	assert_cs( m_pPartyInfo == NULL );
// 	assert_cs( m_pPartyInfoStat == NULL );
	InitScript( NULL, CsPoint( 394, 12 ), CsPoint( 223, 72 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::CENTER );
	// 뒷배경
	assert_cs( m_pWindowBG == NULL );
	m_pWindowBG = NiNew cSprite;
	m_pWindowBG->Init( m_pRoot, CsPoint::ZERO, CsPoint( 223, 65 ), "TargetWindow\\MonsterTarget.tga", false );

	// 레벨 배경
	m_pLevelBG = NiNew cSprite;
	m_pLevelBG->Init( m_pRoot, CsPoint( 203, 46 ), CsPoint( 29, 29 ), "TargetWindow\\DigimnMonsterlevel.tga", false );

	m_pCMonsterMark = NiNew cSprite;
	m_pCMonsterMark->Init( m_pRoot, CsPoint( 152, -7 ), CsPoint( 77, 80 ), "TargetWindow\\CMonster_Mark.tga", false );
	m_pCMonsterLevel = NiNew cSprite;
	m_pCMonsterLevel->Init( m_pRoot, CsPoint( 203, 46 ), CsPoint( 29, 29 ), "TargetWindow\\CMonster_Level.tga", false );

	m_pBMonsterMark = NiNew cSprite;
	m_pBMonsterMark->Init( m_pRoot, CsPoint( 152, -7 ), CsPoint( 77, 80 ), "TargetWindow\\BMonster_Mark.tga", false );
	m_pBMonsterLevel = NiNew cSprite;
	m_pBMonsterLevel->Init( m_pRoot, CsPoint( 203, 46 ), CsPoint( 29, 29 ), "TargetWindow\\BMonster_Level.tga", false );

	m_pWindowBG_Mask = NiNew cSprite;
	m_pWindowBG_Mask->Init( m_pRoot, CsPoint::ZERO, CsPoint( 232, 75 ), "TargetWindow\\Mask_1.tga", false );
	m_pWindowBG_Mask->SetAlpha( 0.6f );	

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontText, CFont::FS_10 );
	ti.s_eTextAlign = DT_CENTER;

	// 이름
	m_pNameText = AddText( &ti, CsPoint( 80, 15 ) );	

	// 레벨
	m_pLevelText = NiNew cText;
	m_pLevelText->Init( m_pRoot, CsPoint( 217, 54 ), &ti, false );

	// hp
	m_pHPBar = NiNew cProgressBar;
	m_pHPBar->Init( m_pRoot, cProgressBar::BAR_3IMAGE, CsPoint( 7, 40 ), CsPoint( 146, 8 ), CsPoint( 2, 2 ), "TargetWindow\\HpBar1.tga", false );
	m_pHPBar->SetRenderPosText( cProgressBar::TEXT_PERCENT );

	// 비교 구문 텍스트
#ifdef IF_TARGET_ELEMENT
	m_pCompareSprite[ UP ] = NiNew cSprite;
	m_pCompareSprite[ UP ]->Init( NULL, CsPoint::ZERO, CsPoint( 19, 19 ), "DigimonStatus\\Up.tga", false );
	m_pCompareSprite[ DOWN ] = NiNew cSprite;
	m_pCompareSprite[ DOWN ]->Init( NULL, CsPoint::ZERO, CsPoint( 19, 19 ), "DigimonStatus\\Down.tga", false );
	m_pCompareSprite[ EQUAL ] = NiNew cSprite;
	m_pCompareSprite[ EQUAL ]->Init( NULL, CsPoint::ZERO, CsPoint( 19, 19 ), "DigimonStatus\\Eq.tga", false );
	m_pCompareSprite[ BEST ] = NiNew cSprite;
	m_pCompareSprite[ BEST ]->Init( NULL, CsPoint::ZERO, CsPoint( 19, 19 ), "DigimonStatus\\Best.tga", false );
	m_pCompareSprite[ WORST ] = NiNew cSprite;
	m_pCompareSprite[ WORST ]->Init( NULL, CsPoint::ZERO, CsPoint( 19, 19 ), "DigimonStatus\\Worst.tga", false );
#else
	m_pUnknown = NiNew cSprite;
	m_pUnknown->Init( NULL, CsPoint::ZERO, CsPoint( 109, 18 ), "DigimonStatus\\unknown.tga", false );
#endif

	for( int i =0; i < 3; i++ ){
		m_pNatureOutLineSprite[i] = NiNew cSprite; 
		m_pNatureOutLineSprite[i]->Init( NULL, CsPoint::ZERO, CsPoint( 27, 27 ), "DigimonStatus\\Element\\button_gold_small.tga", false );		
	}

	m_pAttributeRateSprite[0] = NiNew cSprite;
	m_pAttributeRateSprite[0]->Init( NULL, CsPoint::ZERO, CsPoint( 40, 20 ), "DigimonStatus\\Element\\data_arrow_up.tga", false );

	m_pAttributeRateSprite[1] = NiNew cSprite;
	m_pAttributeRateSprite[1]->Init( NULL, CsPoint::ZERO, CsPoint( 20, 40 ), "DigimonStatus\\Element\\data_arrow_left.tga", false );

	m_pAttributeRateSprite[2] = NiNew cSprite;
	m_pAttributeRateSprite[2]->Init( NULL, CsPoint::ZERO, CsPoint( 40, 20 ), "DigimonStatus\\Element\\data_arrow_down.tga", false );


	m_cpAttributeStatus[0] = CsPoint(0,-20);// 상
	m_cpAttributeStatus[1] = CsPoint(-20,0);// 중
	m_cpAttributeStatus[2] = CsPoint(0,40); // 하
	//	m_AttributeIcon === CsPoint(-40, 14 ) );
	//m_NatureIcon === CsPoint(220, 24 ) );

	m_rcAI.SetRect( CsPoint( 0, 54 ), CsSIZE( 19 , 19 ) );
	m_rcAttribute.SetRect(  CsPoint(-40, 14 ), CsSIZE( 40 , 40 ) );
	m_rcNature.SetRect(  CsPoint(220, 24 ), CsSIZE( 25 * ElementCount, 25 ) );


#ifdef TARGET_WIN_MULTYLINE
	assert_cs( m_pWindowBG_2 == NULL );
	m_pWindowBG_2 = NiNew cSprite;
	m_pWindowBG_2->Init( m_pRoot, CsPoint::ZERO, CsPoint( 223, 65 ), "TargetWindow\\MonsterTarget2.tga", false );
	m_pWindowBG_Mask_2 = NiNew cSprite;
	m_pWindowBG_Mask_2->Init( m_pRoot, CsPoint::ZERO, CsPoint( 232, 75 ), "TargetWindow\\Mask_2.tga", false );
	m_pWindowBG_Mask_2->SetAlpha( 0.6f );

	m_pPenNameText = AddText( &ti, CsPoint( 80, 1 ) );	
#endif

	//===============================================================================================
	//
	//		시간
	//
	//===============================================================================================
	m_bEnableTimeBar = false;

	CsPoint ptTimeBar( 150, 79 );
	m_pTimeBar = NiNew cProgressBar;
	m_pTimeBar->Init( m_pRoot, cProgressBar::BAR_1IMAGE, ptTimeBar + CsPoint( 3, 3 ), CsPoint( 73, 9 ), CsPoint( 1, 9 ), "TargetWindow\\Time\\TimeBar.tga", false );

	m_pTimeBarBG = NiNew cSprite;
	m_pTimeBarBG->Init( m_pRoot, ptTimeBar, CsPoint( 79, 15 ), "TargetWindow\\Time\\TimeBar_BG.tga", false );

	m_pTimeBarBGMask = NiNew cSprite;
	m_pTimeBarBGMask->Init( m_pRoot, ptTimeBar, CsPoint( 79, 15 ), "TargetWindow\\Time\\TimeBar_BGMask.tga", false );

	m_pTimeDeco = NiNew cSpriteAni;
	m_pTimeDeco->Init( cSpriteAni::LOOP, m_pRoot, ptTimeBar - CsPoint( 24, 3 ), CsPoint( 23, 23 ), NULL, NULL, 0, false );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_01.tga" );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_01.tga" );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_01.tga" );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_01.tga" );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_01.tga" );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_01.tga" );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_01.tga" );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_02.tga" );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_03.tga" );
	m_pTimeDeco->AddNode( "TargetWindow\\Time\\Ani_04.tga" );
	m_pTimeDeco->SetAniTime( 0.1f );
}

bool cTargetWindow::Close( bool bSound )
{
	if( IsEnableWindow() == false )
		return false;

	if( bSound == true )
	{
		PlayCloseSound();
	}
	SetShowWindow( false );
	return true;
}

void cTargetWindow::Update(float const& fDeltaTime)
{
	//assert_cs( m_pTargetObject || m_pPartyInfo || m_pPartyInfoStat );

	if( m_pTargetObject )
	{
		CsC_AvObject::sBASE_STAT* pBaseStat = m_pTargetObject->GetBaseStat();
		assert_cs( pBaseStat != NULL );

		// 레벨
		if( pBaseStat->GetLevel() != CsC_AvObject::INVALIDE_STAT )
			m_pLevelText->SetText( pBaseStat->GetLevel() );
		else
			m_pLevelText->SetText( _T( "?" ) );

		// hp
		if( pBaseStat->GetHP() != CsC_AvObject::INVALIDE_STAT )
		{
			m_pHPBar->SetRange( CsPoint( 0, pBaseStat->GetMaxHP() ) );
			// 죽은 상태가 아니면 0%로 보이게 하지 말자
			if( m_pTargetObject->IsServerDie() == false )
				m_pHPBar->SetBarPos( CsMax( 3, pBaseStat->GetHP() ) );
			else
				m_pHPBar->SetBarPos( pBaseStat->GetHP() );
		}
		else
		{
			m_pHPBar->SetRange( CsPoint( 0, CsC_AvObject::INVALIDE_STAT ) );
			m_pHPBar->SetBarPos( CsC_AvObject::INVALIDE_STAT );
		}
	}

	for( int i=0; i < m_nOutLineCount; i++)
	{
		m_pNatureOutLineSprite[i]->SetPos( GetRootClient() + CsPoint(220 + (i*27), 24 )  );
	}

	if( -1 < m_nAttributeRateIndex && m_nAttributeRateIndex < 3 )
		if( m_pAttributeRateSprite[ m_nAttributeRateIndex ] != NULL )
			m_pAttributeRateSprite[m_nAttributeRateIndex]->SetPos( GetRootClient() + CsPoint(-40, 14 ) + m_cpAttributeStatus[m_nAttributeRateIndex]  );

	//===============================================================================================
	//
	//		시간
	//
	//===============================================================================================
	m_bEnableTimeBar = false;

	if( m_pTargetObject != NULL )
	{
		if( m_pTargetObject->GetLeafRTTI() == RTTI_MONSTER )
		{
			CMonster* pMonster = (CMonster*)m_pTargetObject;
			if( pMonster->GetExpireTimeTS() )
			{
				m_bEnableTimeBar = true;

				// 초기 스프라이트 시간 때에만 시간 변화 주겠다
				if( m_pTimeDeco->GetCurSpriteIndex() == 0 )
				{
					u4 nTS = _TIME_TS;
					n4 nETime = (n4)( pMonster->GetExpireTimeTS() - nTS );
					u4 nTotalETime = pMonster->GetTotalExpireTime();				

					m_pTimeBar->SetRange( CsPoint( 0, nTotalETime ) );
					if( m_pTimeBar->SetBarPos( nETime ) == true )
					{
						if( ( nETime <= 5 )&&( nETime > 0 ) )
						{
							cPrintMsg::PrintMsg( 30059, pMonster->GetName(), &nETime );
						}					
					}

					float fRate = (float)nETime / (float)nTotalETime;
					m_pTimeBar->SetColor( fRate > 0.2f ? FONT_GREEN : FONT_RED );
				}

				m_pTimeDeco->Update( g_fDeltaTime );
			}
		}	
	}
}

cBaseWindow::eMU_TYPE
cTargetWindow::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
	if( m_pTargetObject == NULL )
		return muReturn;

	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );	

	if( m_rcAI.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		switch( m_pTargetObject->GetLeafRTTI() )
		{
		case RTTI_MONSTER:
			{
				CsDigimon::sINFO* pFTDigimonInfo = ( (CMonster*)m_pTargetObject )->GetDigimonFT()->GetInfo();
				CsMonster::sINFO* pFTMonsterInfo = ( (CMonster*)m_pTargetObject )->GetMonsterFT()->GetInfo();

				int nIdx = ptMouseLocalPos.x / 19;
				switch( nIdx )
				{
				case 0:
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient() + CsPoint( 19*(nIdx + 1), 54 ),
						CsPoint::ZERO, 210, cTooltip::TARGET_ATTACK, pFTMonsterInfo->s_nBattle );					
					break;
				default:
					break;;
				}		
			}
			break;
		}
	}

	if( m_rcAttribute.PtInRect( ptMouseLocalPos ) == TRUE )
	{
#ifdef BUFFINFO_FIX
		bool bIsRenderPopUp_Buff = false;
		if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_BUFFINFO ) )
		{
			if( g_pGameIF->GetBuffInfo(0) ->_IsRenderPopUp(0) )// 0 : 버프
				bIsRenderPopUp_Buff = true;
// 			else if( g_pGameIF->GetBuffInfo(i) ->_IsRenderPopUp(1) )// 1 : 디버프
// 				bIsRenderPopUp_Buff = true;
		}
		if( !bIsRenderPopUp_Buff )//추가 버프창 열려있으면 속성 툴팁은 Pass
		{
			switch( m_pTargetObject->GetLeafRTTI() )
			{
			case RTTI_MONSTER:
				{
					CsDigimon::sINFO* pFTDigimonInfo = ( (CMonster*)m_pTargetObject )->GetDigimonFT()->GetInfo();
					CsMonster::sINFO* pFTMonsterInfo = ( (CMonster*)m_pTargetObject )->GetMonsterFT()->GetInfo();			

					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient() + CsPoint( 40, 54 ), 
						CsPoint::ZERO, 210, cTooltip::TARGET_ELEMENT, pFTDigimonInfo->s_eAttributeType );
				}
				break;
			}
		}
#else	// BUFFINFO_FIX
		switch( m_pTargetObject->GetLeafRTTI() )
		{
		case RTTI_MONSTER:
			{
				CsDigimon::sINFO* pFTDigimonInfo = ( (CMonster*)m_pTargetObject )->GetDigimonFT()->GetInfo();
				CsMonster::sINFO* pFTMonsterInfo = ( (CMonster*)m_pTargetObject )->GetMonsterFT()->GetInfo();			
				

				TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient() + CsPoint( 40, 54 ), 
					CsPoint::ZERO, 210, cTooltip::TARGET_ELEMENT, pFTDigimonInfo->s_eAttributeType );	


				
			}
			break;
		}
#endif	// BUFFINFO_FIX
	}

	if( m_rcNature.PtInRect( ptMouseLocalPos ) == TRUE )
	{
		switch( m_pTargetObject->GetLeafRTTI() )
		{
		case RTTI_MONSTER:
			{
				CsDigimon::sINFO* pFTDigimonInfo = ( (CMonster*)m_pTargetObject )->GetDigimonFT()->GetInfo();
				CsMonster::sINFO* pFTMonsterInfo = ( (CMonster*)m_pTargetObject )->GetMonsterFT()->GetInfo();

				int nIdx = (ptMouseLocalPos.x- 220) / 25;
				if( nIdx < 3 && -1 != m_bNatureTooltip[nIdx])
				{
					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient() + CsPoint( 25*(nIdx + 1), 54 ),
						CsPoint::ZERO, 210, cTooltip::TARGET_NATURAL, pFTDigimonInfo->s_eBaseNatureTypes[m_bNatureTooltip[nIdx]] );
				}
			}
			break;
		}
	}

#ifdef BUFFINFO_FIX
	if( IsInWindow( ptMouseLocalPos ) == false )
		return muReturn;
	else
		return MUT_OUT_WINDOW;
#else
	return muReturn;
#endif
}
	
void cTargetWindow::Render()
{

	// 뒷 배경
#ifdef TARGET_WIN_MULTYLINE
	if( m_pPenNameText->GetTextInfo()->GetText()[ 0 ] != NULL )
	{
		m_pWindowBG_2->Render();
	}
	else
	{
		m_pWindowBG->Render();
	}	
#else
	m_pWindowBG->Render();
#endif	

	
	// 얼굴 이미지
	if( m_pTargetObject != NULL )
	{
		switch( m_nMonsterType )
		{
		case 0:
		case 1:			break;
		case 2:			m_pCMonsterMark->Render();			break;
		case 3:			
		case 4:			m_pBMonsterMark->Render();			break;
		case 7:
		case 5:			m_pCMonsterMark->Render();			break;
		case 8:
		case 6:			m_pBMonsterMark->Render();			break;
		default:		assert_cs( false );
		}

		sCHAR_IMAGE* pCharImage = NULL;		
		switch( m_pTargetObject->GetLeafRTTI() )
		{
		case RTTI_TAMER:
		case RTTI_TAMER_USER:
			pCharImage = ( (CTamer*)m_pTargetObject )->GetCharImage();			
			break;
		case RTTI_DIGIMON:
		case RTTI_DIGIMON_USER:
			pCharImage = ( (CDigimon*)m_pTargetObject )->GetCharImage();
			break;
		case RTTI_MONSTER:
			pCharImage = ( (CMonster*)m_pTargetObject )->GetCharImage();			
			break;
		case RTTI_NPC:
			pCharImage = ( (CNpc*)m_pTargetObject )->GetCharImage();
			break;
		default:
			assert_cs( false );
		}

		if( pCharImage != NULL )
		{
			pCharImage->Render_L( CsPoint( m_pRoot->GetClient().x + 162, m_pRoot->GetClient().y + 4 ), CsPoint( 57, 57 ) );
		}

		RenderScript();
		
		switch( m_nMonsterType )
		{
		case 0:
		case 1:
			m_pLevelBG->Render();
			m_pLevelText->Render();
			break;
		case 2:
		case 5:
		case 7:
			m_pCMonsterLevel->Render();
			m_pLevelText->Render();
			break;
		case 3:
		case 4:
		case 8:
		case 6:
			m_pBMonsterLevel->Render();
			break;
		default:
			assert_cs( false );
		}
				
		m_pHPBar->Render();
		if( _CheckOwner() == false )
		{
#ifdef TARGET_WIN_MULTYLINE
			m_pWindowBG_Mask_2->Render();
#else
			m_pWindowBG_Mask->Render();
#endif
		}

		for( int i=0; i < m_nOutLineCount; i++)
		{			
			m_pNatureOutLineSprite[i]->SetRotation2( CsD2R( m_dwAngles[i]) );
			m_pNatureOutLineSprite[i]->Render();
		}

		m_AttributeIcon.Render(GetRootClient() + CsPoint(-40, 14 ) );
		m_NatureIcon.Render(GetRootClient() + CsPoint(220, 24 ) );
		
		m_MonsterAI.Render( GetRootClient() + CsPoint( 0, 54 ) );

		
		if(-1 != m_nAttributeRateIndex)
			m_pAttributeRateSprite[m_nAttributeRateIndex]->Render();
	}
	else
	{
		RenderScript();
		m_pLevelBG->Render();
		m_pLevelText->Render();
		m_pHPBar->Render();

		m_pWindowBG_Mask->Render();
	}

	//===============================================================================================
	//
	//		시간
	//
	//===============================================================================================
	if( m_bEnableTimeBar )
	{
		m_pTimeBarBG->Render();
		m_pTimeBar->Render();	
		m_pTimeBarBGMask->Render();
		m_pTimeDeco->Render();
	}	
}

bool cTargetWindow::IsTarget( uint nUniqID )
{
	if( m_pTargetObject )
	{
		if( m_pTargetObject->GetUniqID() == nUniqID )
			return true;
	}
// 	else if( m_pPartyInfoStat )
// 	{
// 		if( m_pPartyInfoStat->s_nUniqID == nUniqID )
// 			return true;
// 	}

	return false;
}

void cTargetWindow::SetTargetObject( CsC_AvObject* pTarget )
{
	m_pTargetObject = NULL;
// 	m_pPartyInfo = NULL;
// 	m_pPartyInfoStat = NULL;
	m_nMonsterType = 0;

	if( pTarget != NULL )
	{
		SetShowWindow( true );

		m_pTargetObject = pTarget;
		m_bOwner = true;

		// 이름
#ifdef TARGET_WIN_MULTYLINE
		switch( m_pTargetObject->GetLeafRTTI() )
		{
		case RTTI_NPC:
			m_pNameText->SetText( m_pTargetObject->GetName() );
			m_pPenNameText->SetText( ( (CNpc*)m_pTargetObject )->GetFTNpc()->GetInfo()->s_szPenName );
			break;
		case RTTI_MONSTER:
			m_pNameText->SetText( ( (CMonster*)m_pTargetObject )->GetMonsterFT()->GetInfo()->s_szName );
			m_pPenNameText->SetText( ( (CMonster*)m_pTargetObject )->GetMonsterFT()->GetInfo()->s_szPenName );
			break;
		default:
			m_pNameText->SetText( m_pTargetObject->GetName() );
			m_pPenNameText->SetText( _T( "" ) );
		}		
#else
		m_pNameText->SetText( m_pTargetObject->GetName() );
#endif

#ifdef PARTY_BUFF_SHOW
		// 버프창 연결시 해당 타입이 RTTI_TAMER_USER인지 확인하기 위해 추가.
		g_pGameIF->GetBuffInfo( IF_BUFF_TARGET )->SetBuffUser(m_pTargetObject->GetLeafRTTI());
#endif
		// 버프창에 연결
		switch( m_pTargetObject->GetLeafRTTI() )
		{
		case RTTI_TAMER:
		case RTTI_TAMER_USER:			
			g_pGameIF->GetBuffInfo( IF_BUFF_TARGET )->SetBuffData( ( (CTamer*)m_pTargetObject )->GetBuff() );
			break;
		case RTTI_DIGIMON:
		case RTTI_DIGIMON_USER:
			g_pGameIF->GetBuffInfo( IF_BUFF_TARGET )->SetBuffData( ( (CDigimon*)m_pTargetObject )->GetBuff() );
			break;
		case RTTI_MONSTER:
			g_pGameIF->GetBuffInfo( IF_BUFF_TARGET )->SetBuffData( ( (CMonster*)m_pTargetObject )->GetBuff() );
			break;
		default:
			g_pGameIF->GetBuffInfo( IF_BUFF_TARGET )->SetBuffData( NULL );
			break;
		}

		// 속성
		m_AttributeIcon.Delete();
		m_NatureIcon.Delete();
		
		m_MonsterAI.Delete();

		memset(m_bNatureTooltip, -1, sizeof(int)*3);
		memset(m_dwAngles, 0, sizeof(DWORD)*3);
		
		int i=0;
		m_nOutLineCount = 0;
		m_nAttributeRateIndex = -1;

		switch( m_pTargetObject->GetLeafRTTI() )
		{
		case RTTI_MONSTER:
			{
#ifdef IF_TARGET_ELEMENT
				CsDigimon::sINFO* pFTDigimonInfo = ( (CMonster*)m_pTargetObject )->GetDigimonFT()->GetInfo();
				CsMonster::sINFO* pFTMonsterInfo = ( (CMonster*)m_pTargetObject )->GetMonsterFT()->GetInfo();

				m_nMonsterType = pFTMonsterInfo->s_nClass;

				//=========================================================================================================
				// AI 속성
				//=========================================================================================================
				int nAIIndex;
				switch( pFTMonsterInfo->s_nBattle )
				{
				case 1:		nAIIndex = 0;		break;
				case 3:		nAIIndex = 1;		break;
				default:	assert_cs( false );
				}

				m_MonsterAI.AddIcon( CsPoint( 19, 19 ), ICONITEM::MonsterAI, nAIIndex );
				m_MonsterAI.TailAddSizeX( 20 );

				//=========================================================================================================
				// 네이쳐 속성
				//=========================================================================================================
		
				if(0 != pFTDigimonInfo->s_eBaseNatureTypes[0])
				{
					int nIndex = pFTDigimonInfo->s_eBaseNatureTypes[0] - nsCsDigimonTable::NT_ICE;
					// 아이콘 DAttribute 타입의 Nature인덱스를 참조
					m_NatureIcon.AddIcon( CsPoint( 25, 25 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex ); // 자연속성					
					m_dwAngles[m_nOutLineCount++] = _GetNatureCompare2(0);					
					
					m_bNatureTooltip[i++] = 0;
				}
				if(0 != pFTDigimonInfo->s_eBaseNatureTypes[1])
				{
					int nIndex = pFTDigimonInfo->s_eBaseNatureTypes[1] - nsCsDigimonTable::NT_ICE;
					// 아이콘 DAttribute 타입의 Nature인덱스를 참조
					m_NatureIcon.AddIcon( CsPoint( 25, 25 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex ); // 자연속성					
					m_dwAngles[m_nOutLineCount++] = _GetNatureCompare2(1);					

					m_bNatureTooltip[i++] = 1;
				}

				if(0 != pFTDigimonInfo->s_eBaseNatureTypes[2])
				{
					int nIndex = pFTDigimonInfo->s_eBaseNatureTypes[2] - nsCsDigimonTable::NT_ICE;
					// 아이콘 DAttribute 타입의 Nature인덱스를 참조					
					
					m_NatureIcon.AddIcon( CsPoint( 25, 25 ), ICONITEM::DAttribute, cIconMng::Nature, nIndex ); // 자연속성					
					m_dwAngles[m_nOutLineCount++] = _GetNatureCompare2(2);	

					m_bNatureTooltip[i++] = 2;
				}

				//=========================================================================================================
				// 디지몬 속성
				//=========================================================================================================
				int nDigimonAtt = pFTDigimonInfo->s_eAttributeType - 1;		// 0기반 인덱스로

				m_AttributeIcon.AddIcon( CsPoint( 40, 40 ), ICONITEM::New_Taget_DAttribute, nDigimonAtt );
				m_nAttributeRateIndex = _GetAttributeCompare2();
#else
				m_MonsterAI.AddSprite( m_pUnknown, CsPoint( 16, 1 ) );
#endif
			}
			break;
		}		
	}
	else
	{
		SetShowWindow( false );
		g_pGameIF->GetBuffInfo( IF_BUFF_TARGET )->SetBuffData( NULL );  // 위탁상점 클릭 시 버프 안사라지는것 관련 추가 코드	lks007
		g_pGameIF->GetPopup()->ClosePopup( cPopUpWindow::OTHER_TAMER );
	}

	m_pTimeDeco->ResetAni();
}


// void cTargetWindow::SetTargetObject_Party( cData_Party::sINFO* pPartyInfo, bool bTamer )
// {
// 	m_pTargetObject = NULL;
// 	m_pPartyInfo = NULL;
// 	m_pPartyInfoStat = NULL;
// 	m_nMonsterType = 0;
// 
// 	if( pPartyInfo != NULL )
// 	{
// 		SetShowWindow( true );
// 		
// 		m_pPartyInfo = pPartyInfo;
// 		m_bPartyStatTamer = bTamer;
// 		m_bOwner = true;
// 
// 		m_pPartyInfoStat = bTamer == true ? &m_pPartyInfo->s_TamerStat : &m_pPartyInfo->s_DigimonStat;
// 
// 		// 이름
// 		if(m_pNameText)
// 			m_pNameText->SetText( m_pPartyInfoStat->s_szName );
// #ifdef TARGET_WIN_MULTYLINE
// 		if(m_pPenNameText)
// 			m_pPenNameText->SetText( _T( "" ) );
// #endif
// 
// 		// 속성
// 		m_AttributeIcon.Delete();
// 		m_NatureIcon.Delete();	
// 		m_MonsterAI.Delete();
// 	}
// 	else
// 	{
// 		SetShowWindow( false );
// 		g_pGameIF->GetPopup()->ClosePopup( cPopUpWindow::OTHER_TAMER );
// 	}
// }



cTargetWindow::eCOMPARE
cTargetWindow::_GetAttributeCompare()
{
	CsDigimonMng::eCOMPARE eFTCompare = nsCsFileTable::g_pDigimonMng->AttibuteCompare( 
															g_pCharMng->GetDigimonUser( 0 )->GetFTDigimon()->GetInfo()->s_dwDigimonID,
															( (CMonster*)m_pTargetObject )->GetDigimonFT()->GetInfo()->s_dwDigimonID );

	switch( eFTCompare )
	{
	case CsDigimonMng::UP:		return cTargetWindow::UP;
	case CsDigimonMng::DOWN:	return cTargetWindow::DOWN;
	case CsDigimonMng::EQUAL:	return cTargetWindow::EQUAL;
	case CsDigimonMng::BEST:	return cTargetWindow::BEST;
	case CsDigimonMng::WORST:	return cTargetWindow::WORST;
	default:					assert_cs( false );
	}
	return (eCOMPARE)0;
}

int cTargetWindow::_GetAttributeCompare2()
{
	CsDigimonMng::eCOMPARE eFTCompare = nsCsFileTable::g_pDigimonMng->AttibuteCompare2( 
		g_pCharMng->GetDigimonUser( 0 )->GetFTDigimon()->GetInfo()->s_dwDigimonID,
		( (CMonster*)m_pTargetObject )->GetDigimonFT()->GetInfo()->s_dwDigimonID );

	switch( eFTCompare )
	{
	case CsDigimonMng::UP:		return 0;
	case CsDigimonMng::DOWN:	return 2;
	case CsDigimonMng::EQUAL:	return 1;
	case CsDigimonMng::BEST:	return 0;
	case CsDigimonMng::WORST:	return 2;
	default:					assert_cs( false );
	}
	return 0;
}


cTargetWindow::eCOMPARE
cTargetWindow::_GetNatureCompare()
{
	CsDigimonMng::eCOMPARE eFTCompare = nsCsFileTable::g_pDigimonMng->NatureCompare( 
															g_pCharMng->GetDigimonUser( 0 )->GetFTDigimon()->GetInfo()->s_dwDigimonID,
															( (CMonster*)m_pTargetObject )->GetDigimonFT()->GetInfo()->s_dwDigimonID );

	switch( eFTCompare )
	{
	case CsDigimonMng::UP:		return cTargetWindow::UP;
	case CsDigimonMng::DOWN:	return cTargetWindow::DOWN;
	case CsDigimonMng::EQUAL:	return cTargetWindow::EQUAL;
	case CsDigimonMng::BEST:	return cTargetWindow::BEST;
	case CsDigimonMng::WORST:	return cTargetWindow::WORST;
	default:					assert_cs( false );
	}
	return (eCOMPARE)0;
}

DWORD cTargetWindow::_GetNatureCompare2(int i)
{	
	CsDigimonMng::eCOMPARE eFTCompare = nsCsFileTable::g_pDigimonMng->NatureCompare2( 
		g_pCharMng->GetDigimonUser( 0 )->GetFTDigimon()->GetInfo()->s_dwDigimonID,
		( (CMonster*)m_pTargetObject )->GetDigimonFT()->GetInfo()->s_dwDigimonID, i );

	switch( eFTCompare )
	{
	case CsDigimonMng::UP:		return 315;
	case CsDigimonMng::DOWN:	return 135;
	case CsDigimonMng::EQUAL:	return 45;
	case CsDigimonMng::BEST:	return 315;
	case CsDigimonMng::WORST:	return 135;
	default:					assert_cs( false );
	}
	return 45;
}

void cTargetWindow::OnRButtonUp( CsPoint pos )
{
	if( m_pTargetObject == NULL )
		return;

	if( IsInWindow( pos ) == false )
		return;

	if( nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE )
		return;

	switch( m_pTargetObject->GetLeafRTTI() )
	{
	case RTTI_TAMER:
		g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::OTHER_TAMER, m_pTargetObject->GetUniqID() );
		break;
	case RTTI_DIGIMON:
		g_pGameIF->GetPopup()->SetPopup( CURSOR_ST.GetPos() + CsPoint( 15, 0 ), CsPoint( 130, 0 ), cPopUpWindow::OTHER_DIGIMON, m_pTargetObject->GetUniqID() );
		break;
	}	
}

void cTargetWindow::ResetDevice()
{	
	ResetDeviceScript();

	m_pWindowBG->ResetDevice();
	m_pWindowBG_Mask->ResetDevice();
#ifdef TARGET_WIN_MULTYLINE
	m_pWindowBG_2->ResetDevice();
	m_pWindowBG_Mask_2->ResetDevice();
#endif	
	m_pLevelBG->ResetDevice();
	m_pLevelText->ResetDevice();
	m_pHPBar->ResetDevice();
	m_pCMonsterMark->ResetDevice();
	m_pCMonsterLevel->ResetDevice();
	m_pBMonsterMark->ResetDevice();
	m_pBMonsterLevel->ResetDevice();

	m_pTimeBar->ResetDevice();
	m_pTimeBarBG->ResetDevice();
	m_pTimeBarBGMask->ResetDevice();
	m_pTimeDeco->ResetDevice();
}

bool cTargetWindow::OnEscapeKey()
{
	g_pCharResMng->ReleaseTargetMark( true );
	g_pGameIF->GetBuffInfo( IF_BUFF_TARGET )->SetBuffData( NULL );

	return true;
}

bool cTargetWindow::_CheckOwner()
{
	if( m_pTargetObject == NULL )
		return true;

	if( m_pTargetObject->GetLeafRTTI() != RTTI_MONSTER )
		return true;

	uint nOwnerID = ( (CMonster*)m_pTargetObject )->GetOwnerID();
	if( nOwnerID == 0 )
		return true;

	CsC_AvObject* pOwner = g_pMngCollector->GetObject( nOwnerID );
	if( pOwner == NULL )
		return false;

	if( pOwner->GetLeafRTTI() == RTTI_TAMER_USER )
		return true;
	if( pOwner->GetLeafRTTI() == RTTI_DIGIMON_USER )
		return true;

	return false;
}