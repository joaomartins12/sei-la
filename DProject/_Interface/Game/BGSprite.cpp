

#include "stdafx.h"
#include "BGSprite.h"

cBGSprite::cBGSprite()
{
	for( int i=0; i<IF_BG_SPRITE_CNT; ++i )
		m_pSpriteArray[ i ] = NULL;

	for( int i=0; i<IF_BG_IMAGE_CNT; ++i )
		m_pImageArray[ i ] = NULL;

	for( int i=0; i<IF_BG_BUTTON_CNT; ++i )
		m_pButtonArray[ i ] = NULL;

	for( int i=0; i<IF_BG_SPRITE_ANI_CNT; ++i )
		m_pSpriteAniArray[ i ] = NULL;
	m_vValue1.clear();

	m_pCancelButton = NULL;

	m_nRenderDest = 0;
	m_nRenderStart = DAILY_MAX_SIZE;
	m_fCurPos = (float)m_nRenderStart;
	m_fDestPos = (float)m_nRenderDest;
	m_fStart = 0;
	m_fMinute = 0;
	m_fTime = 0;
	m_fSecond = 0;
	
	m_nValue1 = 0;
	m_nValue2 = 0;
	m_nValue3 = 0;

	m_fValue1 = 0.0f;
	m_bValue1 = false;
}

void cBGSprite::Destroy()
{
	cBaseWindow::Delete();
}

void cBGSprite::DeleteResource()
{	
	m_strlist.Delete();
	DeleteScript();

	for( int i=0; i<IF_BG_SPRITE_CNT; ++i )
	{
		SAFE_NIDELETE( m_pSpriteArray[ i ] );
	}

	for( int i=0; i<IF_BG_IMAGE_CNT; ++i )
	{
		SAFE_NIDELETE( m_pImageArray[ i ] );
	}

	for( int i=0; i<IF_BG_BUTTON_CNT; ++i )
	{
		SAFE_NIDELETE( m_pButtonArray[ i ] );
	}	

	for( int i=0; i<IF_BG_SPRITE_ANI_CNT; ++i )
	{
		SAFE_NIDELETE( m_pSpriteAniArray[ i ] );
	}	

	m_vValue1.clear();

	SAFE_NIDELETE( m_pCancelButton );
}

void cBGSprite::Create( int nValue /*=0*/ )
{
	if( IsLive() == true )
	{
		Destroy();
	}

	m_eValueType = (eVALUE_TYPE)nValue;

	bool bInitial = cBaseWindow::Init();
	assert_cs( bInitial == true );

	switch( m_eValueType )
	{	
	case SPRITE_MAKE_TACTICE_UP:
		{		
			//InitScript( "Event\\HatchEvent.tga", CsPoint( 244, 12 ), CsPoint( 142, 39 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );
			
			//InitScript( "Event\\DropRate.tga", CsPoint( 809, 691 - 18 ), CsPoint( 142, 39 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );
			InitScript( "Event\\DropRate.tga", CsPoint( 631, 610 ), CsPoint( 142, 39 ), false, IFREGION_X::LEFT, IFREGION_Y::BOTTOM );
			m_eLifeType = LIFE_ALWAYS;
		}
		break;
// 	case DAILY_EVENT:
// 		{	
// 			m_bIncreaseAlpha = true;
// 			m_fAccumTime = 0.0f;
// 			m_fAlpha = 0.0f;
// 			m_fCurPos = (float)m_nRenderStart;
// 			InitScript( "EventButton\\PlayTime_edge.tga", CsPoint( 905, 650 ), CsPoint( 115, 30 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );								
// 			//( (cSprite*)m_pRoot )->SetAlpha( 0.8f );
// 
// 			m_pSpriteArray[ 0 ] = NiNew cSprite;
// 			m_pSpriteArray[ 0 ]->Init( m_pRoot, CsPoint::ZERO, CsPoint( 115, 30 ), "EventButton\\PlayTime_back.tga", false );
// 
// 			m_pSpriteArray[ 1 ] = NiNew cSprite;
// 			m_pSpriteArray[ 1 ]->Init( m_pRoot, CsPoint::ZERO, CsPoint( 115, 30 ), "EventButton\\Event_gage.tga", false );
// 
// 			m_pButtonArray[ 0 ] = NiNew cButton;
// 			m_pButtonArray[ 0 ]->Init( GetRoot(), CsPoint::ZERO, CsPoint( 115, 30 ), "EventButton\\PlayTime_button.tga", false );
// 			m_pButtonArray[ 0 ]->SetTexToken( CsPoint( 0, 30 ) );
// 		}
// 		break;
	case ATTENDANCE_EVENT:
		{
			m_eLifeType = LIFE_ALWAYS;
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DAILY_EVENT_UI ) == false )
			{
				InitScript( "Event\\Daily.tga", CsPoint( 809, 641 ), CsPoint( 212, 39 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );
				m_bValue1 = false;
			}
			else
			{
				InitScript( "Event\\Daily.tga", CsPoint( 809, 600 ), CsPoint( 212, 39 ), false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );
				m_bValue1 = true;
			}

			( (cSprite*)m_pRoot )->SetAlpha( 0.8f );			

			MakeAttendance();
		}
		break;

	/*case CHRISTMAS_EVENT:
		{
			InitScript( "Event\\ChristmasEvent.tga", CsPoint( 244, 12 ), CsPoint( 292, 507 ), false, IFREGION_X::LEFT, IFREGION_Y::TOP );

			m_eLifeType = LIFE_ALWAYS;
		}
		break;*/

	case CAPSULE_COMP:
		{
			int nImageIdx =	rand()%3 + 1;
			
			char cBG[ MAX_FILENAME ];
			sprintf_s( cBG, MAX_FILENAME, "_Etc\\Capsule_Win%02d.bmp", nImageIdx );

			CsPoint vInitPos = CsPoint( (int)( ( g_nScreenWidth - 310 )*0.5f ), (int)( ( g_nScreenHeight - 270 )*0.5f ) );
			InitScript( cBG, vInitPos, CsPoint( 310, 270 ), false, IFREGION_X::CENTER, IFREGION_Y::CENTER, false );

			CsPoint ptImagePos( 0, 0 );
			switch( nImageIdx )
			{
			case 1:		ptImagePos = CsPoint( 33, 74 );		break;
			case 2:		ptImagePos = CsPoint( 168, 60 );	break;
			case 3:		ptImagePos = CsPoint( 170, 60 );	break;
			default:	assert_cs( false );
			}
			m_pImageArray[ 0 ] = NiNew cImage;
			m_pImageArray[ 0 ]->Init( m_pRoot, ptImagePos, CsPoint( 128, 128 ), "_Etc\\Capsule.tga", false );
			m_pImageArray[ 0 ]->SetTexToken( CsPoint( 128, 0 ) );			

			SetTime( 2500 );

			m_fValue1 = 0.0f;
			( (cSprite*)m_pRoot )->SetAlpha( m_fValue1 );
			m_pImageArray[ 0 ]->SetAlpha( m_fValue1 );
		}
		break;

	case PCBANG:
		{
			InitScript( "Event\\PC_bigbanner.bmp", CsPoint( 200, 100 ), CsPoint( 682, 358 ), false, IFREGION_X::JUST_CENTER, IFREGION_Y::JUST_CENTER, false );
			m_pCancelButton = NiNew cButton;
			m_pCancelButton->Init( GetRoot(), CsPoint( 662, 5 ), CsPoint( 16, 16 ), "System\\Ch_Close.tga", false );
			m_pCancelButton->SetTexToken( CsPoint( 0, 16 ) );

			m_eLifeType = LIFE_ALWAYS;
		}
		break;

// 	case TUTORIAL:
// 		{
// 			InitScript( NULL, CsPoint( 10, g_nScreenHeight - 500 ), CsPoint( 386, 480 ), true, IFREGION_X::LEFT, IFREGION_Y::TOP, false );
// 			m_eLifeType = LIFE_ALWAYS;
// 			
// 			//m_pButtonArray[ 0 ] = AddButton(CsPoint( 287, 430 ), CsPoint( 80, 30 ),cButton::IMAGE_NOR_3, _CUR_ENG( "Skip" ));
// 			m_pButtonArray[ 0 ] = NiNew cButton;
// 			m_pButtonArray[ 0 ]->Init( NULL , CsPoint( 287, 430 ), CsPoint( 80, 30 ), cButton::IMAGE_NOR_3, _CUR_ENG( "Skip" ), false );
// 		}
// 		break;
#ifdef CROSSWARS_SYSTEM
	case CROSSBUTTON:
		{
			InitScript( NULL, CsPoint( 800, 650 ), CsPoint( 59, 54 ), false, IFREGION_X::LEFT, IFREGION_Y::BOTTOM, true );

			m_eLifeType = LIFE_ALWAYS;

			m_pSpriteArray[ 0 ] = NiNew cSprite;
			m_pSpriteArray[ 0 ]->Init( GetRoot(), CsPoint( -4, -4 ), CsPoint( 59, 54 ), "CrossWars\\130729\\CrossRoder_BtnYellow.tga", false );

			m_pSpriteArray[ 1 ] = NiNew cSprite;
			m_pSpriteArray[ 1 ]->Init( GetRoot(), CsPoint::ZERO, CsPoint( 51, 46 ), "CrossWars\\130729\\CrossRoder_BtnGrey.tga", false );
		
			m_pSpriteArray[ 2 ] = NiNew cSprite;
			m_pSpriteArray[ 2 ]->Init( GetRoot(), CsPoint::ZERO, CsPoint( 19, 10 ), "CrossWars\\130729\\CrossRoder_GaugeCheck.tga", false);		
			m_pSpriteArray[ 2 ]->SetVisible(false);

			m_pButtonArray[ 0 ] = NiNew cButton;
			m_pButtonArray[ 0 ]->Init( GetRoot(), CsPoint::ZERO, CsPoint( 51, 46 ), cButton::IMAGE_CROSSBUTTON, _T( "" ), true );
		}
		break;
#endif
// 	case MATCH_START_COUNT:
// 		{
// 			InitScript( NULL, CsPoint::ZERO, CsPoint::ZERO, false, IFREGION_X::LEFT, IFREGION_Y::TOP, false );
// 			m_eLifeType = LIFE_ALWAYS;
// 
// 			//m_fValue1 = g_pDataMng->GetBattle()->GetMatchStartTime();
// 
// 			m_pSpriteArray[ 0 ] = NiNew cSprite;								
// 			m_pSpriteArray[ 0 ]->Init( NULL, CsPoint::ZERO, CsPoint( 375, 178 ), "EnchantTactics\\NO.tga", false );
// 			m_pSpriteArray[ 0 ]->SetColor( NiColor( 1, 1, 0 ) );
// 
// 			m_pSpriteArray[ 1 ] = NiNew cSprite;								
// 			m_pSpriteArray[ 1 ]->Init( NULL, CsPoint::ZERO, CsPoint( 306, 71 ), "Battle_G\\Match\\BattleStart.tga", false );
// 		}
// 		break;
// 
// 	case MATCH_PLAY_COUNT:
// 		{
// 			InitScript( NULL, CsPoint( 800, 30 ), CsPoint::ZERO, false, IFREGION_X::RIGHT, IFREGION_Y::TOP, true );
// 			m_eLifeType = LIFE_ALWAYS;
// 
// 			//m_fValue1 = g_pDataMng->GetBattle()->GetMatchEndTime();
// 
// 			m_pSpriteArray[ 0 ] = NiNew cSprite;								
// 			m_pSpriteArray[ 0 ]->Init( NULL, CsPoint::ZERO, CsPoint( 375, 178 ), "EnchantTactics\\NO.tga", false );
// 			m_pSpriteArray[ 0 ]->SetColor( NiColor( 1, 1, 0 ) );
// 
// 			m_pSpriteArray[ 1 ] = NiNew cSprite;								
// 			m_pSpriteArray[ 1 ]->Init( NULL, CsPoint::ZERO, CsPoint( 22, 49 ), "EnchantTactics\\NO2.tga", false );
// 			m_pSpriteArray[ 1 ]->SetColor( NiColor( 1, 1, 0 ) );
// 		}
// 		break;
// 
// 	case MATCH_NEXT_COUNT:
// 		{
// 			InitScript( NULL, CsPoint::ZERO, CsPoint::ZERO, false, IFREGION_X::LEFT, IFREGION_Y::TOP, false );
// 			m_eLifeType = LIFE_ALWAYS;
// 
// 			//cData_Battle::sMATCH_INFO* pMatchInfo = g_pDataMng->GetBattle()->GetMatchInfo_ID( g_pDataMng->GetBattle()->GetJoinMatchID() );
// 
// 			//m_fValue1 = (float)( pMatchInfo->s_nRoundStartTS - _TIME_TS );
// 
// 			m_pSpriteArray[ 0 ] = NiNew cSprite;								
// 			m_pSpriteArray[ 0 ]->Init( NULL, CsPoint::ZERO, CsPoint( 375, 178 ), "EnchantTactics\\NO.tga", false );
// 			m_pSpriteArray[ 0 ]->SetColor( NiColor( 1, 1, 0 ) );
// 
// 			m_pSpriteArray[ 1 ] = NiNew cSprite;								
// 			m_pSpriteArray[ 1 ]->Init( NULL, CsPoint::ZERO, CsPoint( 22, 49 ), "EnchantTactics\\NO2.tga", false );
// 			m_pSpriteArray[ 1 ]->SetColor( NiColor( 1, 1, 0 ) );
// 
// 			m_pSpriteArray[ 2 ] = NiNew cSprite;
// 			m_pSpriteArray[ 2 ]->Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), NiColorA( 0, 0, 0, 0.6f ), false );
// 		}
// 		break;

#ifdef MINIGAME
	case MINIGAME_COUNT:
		{
			InitScript( NULL, CsPoint::ZERO, CsPoint::ZERO, false, IFREGION_X::LEFT, IFREGION_Y::TOP, false );
			m_eLifeType = LIFE_ALWAYS;

			m_fValue1 = 2.999f;//3초부터 시작

			m_pSpriteArray[ 0 ] = NiNew cSprite;								
			m_pSpriteArray[ 0 ]->Init( NULL, CsPoint::ZERO, CsPoint( 375, 178 ), "EnchantTactics\\NO.tga", false );
			m_pSpriteArray[ 0 ]->SetColor( NiColor( 1, 1, 0 ) );
		}
		break;
#endif
#ifdef TRICK_ORTREAT
	case TIME_ITEM:
		{
			InitScript( NULL, CsPoint( 623, 593 ), CsPoint::ZERO, false, IFREGION_X::LEFT, IFREGION_Y::BOTTOM, true );			

			m_eLifeType = LIFE_ALWAYS;
			m_fValue1 = g_pDataMng->GetInven()->GetEventTime();			

			// 라이프타입 시퀀스설정?
			//SetTime( ( g_pDataMng->GetInven()->GetEventTime()/ 60) *1000 );

			m_pSpriteArray[ 0 ] = NiNew cSprite;								
			m_pSpriteArray[ 0 ]->Init( GetRoot(), CsPoint::ZERO, CsPoint( 375, 178 ), "System\\DigitalNo.tga", false );
			m_pSpriteArray[ 0 ]->SetColor( NiColor( 1, 0, 0 ) );

			m_pSpriteArray[ 1 ] = NiNew cSprite;								
			m_pSpriteArray[ 1 ]->Init( GetRoot(), CsPoint::ZERO, CsPoint( 22, 49 ), "System\\Dot.tga", false );
			m_pSpriteArray[ 1 ]->SetColor( NiColor( 1, 0, 0 ) );

			m_pImageArray[ 0 ] = NiNew cImage;
			m_pImageArray[ 0 ]->Init( GetRoot(), CsPoint::ZERO, CsPoint( 150, 56 ), "System\\Back.tga", false );
			//m_pImageArray[ 0 ]->SetTexToken( CsPoint( 128, 0 ) );	
		}
		break;
#endif
	case RAID_REWARD:
		{			

			InitScript( NULL, CsPoint( 871, 736 ), CsPoint::ZERO, false, IFREGION_X::RIGHT, IFREGION_Y::BOTTOM );
			m_eLifeType = LIFE_ALWAYS;			

			m_pSpriteAniArray[ 0 ] = NiNew cSpriteAni;								
			m_pSpriteAniArray[ 0 ]->Init( cSpriteAni::ONCE, GetRoot(), CsPoint( -32, -54 ), CsPoint( 100, 100 ), "MainBar\\star\\star_000", "tga", 19, false );
			m_pSpriteAniArray[ 0 ]->SetAniTime( 0.04f );
			m_pSpriteAniArray[ 0 ]->PlayOff();

			m_InitPos  = GetRootClient()+ CsPoint( 3, -100 );
			m_nValue1 = 0;	// Current 인덱스값
			m_fValue1 = (float)GetRootClient().y - 4.0f;		// 아이콘이 리셋되는 오프셋값
			m_fValue2 = 1.0f;	// 알파값..
			m_bValue1 = true;

		}
		break;
	default:
		assert_cs( false );
	}
}

bool cBGSprite::Close( bool bSound /* = true  */ )
{
	return cBaseWindow::Close( false );
}

void cBGSprite::Update(float const& fDeltaTime)
{
	switch( m_eValueType )
	{
// 	case TUTORIAL:
// 		_UpdateMoveWindow();
// 
// 		if(m_pButtonArray[0])
// 		{ 
// 			m_pButtonArray[ 0 ]->SetPos(GetRootClient() + CsPoint(287 , 430) );
// 			m_pButtonArray[ 0 ]->SetPosText(GetRootClient() + CsPoint( 327, 438 ) );
// 		}
// 
// 		break;
	case CAPSULE_COMP:
		m_fValue1 += fDeltaTime;
		( (cSprite*)m_pRoot )->SetAlpha( CsMin( 1.0f, m_fValue1*2.0f ) );
		m_pImageArray[ 0 ]->SetAlpha( CsMin( 1.0f, CsMax( 0.0f, m_fValue1*2.0f - 2.0f ) ) );
		break;
// 	case DAILY_EVENT:
// 		{
// 			cString::sTEXT* pText = ( (cString::sTEXT*)m_strlist.GetTail()->GetElement( 0 ) );
// 			if( m_LifeTimeSeq.IsEnableTime() )
// 			{
// 				if( m_fCurPos == 0 )
// 				{
// 					m_fCurPos = (float)m_nRenderStart;
// 				}
// 				TCHAR sz[ 128 ];				
// 				_stprintf_s( sz, 128, _LAN( "남은시간'%.2d:%.2d'" ), ( m_LifeTimeSeq.RemainDeltaTime() / 1000 ) / 60, ( m_LifeTimeSeq.RemainDeltaTime() / 1000 ) % 60 );
// 				pText->s_Text.SetText( sz );
// 
// 				if( m_fMinute != 0 && m_fSecond != 0 )
// 				{
// 					m_pButtonArray[ 0 ]->setTexTokenState(cButton::NORMAL);
// 				}
// 
// 				uint tmpTime = GetTickCount();
// 				m_fMinute = ( m_LifeTimeSeq.RemainDeltaTime() / 1000 ) / 60;		//바 채워지는 시간 (분단위)
// 				m_fSecond = ( m_LifeTimeSeq.RemainDeltaTime() / 1000 ) % 60;
// 				m_fTime = m_fStart / 60;
// 
// 				//(float)m_fTime = (float)m_fStart - (float)m_fMinute;				//전체시간에서 남은시간을 빼서 흐른시간 구한다.
// 
// 				float per;
// 				per = (float)m_fMinute / (float)m_fTime ;		//start = 전체시간, minute = 남은시간
// 
// 				float CurPos = (float)DAILY_MAX_SIZE * per;
// 
// 				CurPos = (float)DAILY_MAX_SIZE - CurPos;
// 
// 				if( (this->m_fCurPos > m_fDestPos) && (CurPos >= 0) ) //바 채우기
// 					this->m_fCurPos = CurPos;					 //s_nDeltaPos*fDeltaTime;
// 
// 			}
// 			
// 		}
// 		break;
	case ATTENDANCE_EVENT:
		{
			cString::sTEXT* pText = ( (cString::sTEXT*)m_strlist.GetTail()->GetElement( 0 ) );

			if( g_pDataMng->GetAttendance()->IsEnableAttendance() == false )
			{				
				u4 timeTS = _TIME_TS;
				std::wstring wsRemain;
				if( g_pDataMng->GetAttendance()->GetTodayAttendanceTimeTS() >= timeTS )
				{
					u4 nTotalSec = g_pDataMng->GetAttendance()->GetTodayAttendanceTimeTS() - timeTS;
					assert_cs( nTotalSec / ( 60 * 60 ) == 0 );
					u4 nMin = nTotalSec / 60;
					u4 nSec = nTotalSec % 60;
					if( nMin )
					{
						wsRemain = UISTRING_TEXT( "BGSPRITE_REMAIN_TIME_MIN_SEC" );
						DmCS::StringFn::Replace( wsRemain, L"#Min#", nMin );
						DmCS::StringFn::Replace( wsRemain, L"#Sec#", nSec );
					}
					else
					{
						wsRemain = UISTRING_TEXT( "BGSPRITE_REMAIN_TIME_SEC" );
						DmCS::StringFn::Replace( wsRemain, L"#Sec#", nSec );
					}
				}
				else
				{
					wsRemain = UISTRING_TEXT( "BGSPRITE_REMAIN_TIME_SEC_ZERO" );
				}
				pText->s_Text.SetText( wsRemain.c_str() );
				g_pDataMng->GetAttendance()->SetEnableAttendance( false );
			}
			else
			{
				pText->s_Text.SetText( UISTRING_TEXT( "BGSPRITE_REMAIN_TIME_NONE" ).c_str() );
				g_pDataMng->GetAttendance()->SetEnableAttendance( true );
			}
			

			// 위치 변경 정보
			if( g_pGameIF->IsActiveWindow( cBaseWindow::WT_DAILY_EVENT_UI ) == false )
			{
				if( m_bValue1 )
				{
					SetPos( CsPoint( 809, 641 ) );
					ResetDevice();
					m_bValue1 = false;
				}
			}				
			else if( m_bValue1 == false )
			{
				SetPos( CsPoint( 809, 600 ) );
				ResetDevice();
				m_bValue1 = true;
			}			
		}
		break;
#ifdef CROSSWARS_SYSTEM
	case CROSSBUTTON:
		{

			cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();
			if(pCrossTactics && 0 != pCrossTactics->GetCrossID())
			{
				CsCrossDigimon::sINFO *info = nsCsFileTable::g_pDigimonMng->GetCrossDigimon( pCrossTactics->GetCrossID() )->GetInfo();



				CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
				CTamerUser::sUSER_STAT* pUserStat = (CTamerUser::sUSER_STAT*)pTamerUser->GetBaseStat();
				//pUserStat->SetCR(3000);
				//pUserStat->SetMaxCR(10000);

				CsPoint Size( 0, 46 );

				// Size 값이 높을수록 0%에 가까움 0(100%) ~ 64(0%)
				//dwExp = 9999;

				DWORD dwCRRate, dwSizeRate;
				CsPoint	Pos = CsPoint::ZERO;


				dwCRRate =  (DWORD)(((float)info->s_nCodeCrownRequire/pUserStat->GetMaxCR()) * 100);
				dwSizeRate = (DWORD)((Size.y * 0.01) * dwCRRate);
				Pos = CsPoint(0, (Size.y - dwSizeRate));
				

				m_pSpriteArray[ 2 ]->SetPos( Pos + CsPoint( -10, -5 ) );

				if(0 != pCrossTactics->GetCrossID())
					m_pSpriteArray[ 2 ]->SetVisible(true);
			}


		}
		break;
#endif
	case RAID_REWARD:
		{
			
			// 이펙트 업데이트
			m_pSpriteAniArray[ 0 ]->Update( fDeltaTime );

			// 아이콘 좌표 갱신
			m_InitPos += CsPoint( 0, 2 );

			// 아이콘좌표가 (가방좌표-40)를 넘어서면 알파값 시작 
			if((m_fValue1 - 40.0f) < m_InitPos.y)
			{
				m_fValue2 -= (fDeltaTime * 1.0f );
			}
			// 아이콘좌표가 (가방좌표-30)를 넘어서면 이펙트 시작 && 플래그 개방되있나?
			if(((m_fValue1 - 30.0f) < m_InitPos.y) && m_bValue1)
			{
				// 리셋타임..
				m_pSpriteAniArray[ 0 ]->PlayOn(true);		// 이펙트 시작
				m_bValue1 = false; // 플래그 닫기

				// 사운드 시작
				CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
				if( pTamerUser )
				{
					if( (uint)m_nValue1 < m_vValue1.size() )
					{
						// 현재값의 사운드
						pTamerUser->ItemCropSound( m_vValue1[m_nValue1] );
					}
				}
				
			}

			// 아이콘좌표가 (가방좌표)를 넘어서면 && 인덱스값이 사이즈 안이면
			if( (m_fValue1 < m_InitPos.y) && ( (uint)m_nValue1 < m_vValue1.size()))
			{
				// 리셋 타임..
				m_InitPos  = GetRootClient() + CsPoint( 3, -100 );	// 아이콘 좌표 리셋
				m_nValue1++;	// 인덱스 증가
				m_fValue2 = 1.0f;	// 알파값 리셋
				m_bValue1 = true;	// 플래그 리셋
			}			

			// 모든 애니 끝날때.
			if( (uint)m_nValue1 >= m_vValue1.size() && (!m_pSpriteAniArray[ 0 ]->IsPlay()))
			{
				m_bValue1 = false; // 플래그 닫기
				_EndTime();
				Close();
			}
			
			
		}
		break;
#ifdef TRICK_ORTREAT
	case TIME_ITEM:
		{
			//m_fValue1 -= fDeltaTime;
			m_fValue1 = g_pDataMng->GetInven()->GetEventTime();
			if(-1 == m_fValue1)
			{
				GAME_EVENT_ST.OnEvent( EVENT_CODE::WAREHOUSE_GET_EVENTTIME, &m_fValue1 );
//				m_fValue1 = g_pDataMng->GetWarehouse()->GetEventTime();
			}
			if( m_fValue1 < 0.0f )
			{				
				m_fValue1 = 0.0f;				
				_EndTime();				
			}
		}
		break;
#endif
#ifdef MINIGAME
	case MINIGAME_COUNT:
		{
			SAFE_POINTER_BEK(g_pGameIF->GetMiniGame());
			if( m_eLifeType == LIFE_ALWAYS )
			{
				//m_fValue1 = g_pGameIF->GetMiniGame()->GetCountDown();	//카운트다운 남은 시간
				//if( m_fValue1 <= 0.0f )//카운트 종료 할 때가 되었다
				//{
				//	GS2C_RECV_MAKE_DIGITAMA_MINIGAME_INIT kRecv;
				//	kRecv.nResult = true;
				//	kRecv.nSuccessCnt = 0;
				//	GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_MAKETACTICS_MINIGAME_INIT, &kRecv );
				//	m_fValue1 = 0;
				//}
			}
		}
		break;
#endif
	}

	switch( m_eLifeType )
	{
	case LIFE_TIME:
		if( m_LifeTimeSeq.IsEnable() == true )
		{
			_EndTime();
			Close();
		}
		break;
	case LIFE_ALWAYS:
		{
			
		}
		break;
	default:
		assert_cs( false );
		break;
	}	

}

void cBGSprite::_EndTime()
{
	// 후처리
	switch( m_eValueType )
	{
	case CAPSULE_COMP:
		{
			g_pDataMng->GetServerSync()->RecvServer( cData_ServerSync::CASH_CAPSULE_COIN_COMPLEATE, 0 );
			g_pCharMng->GetTamerUser()->SetPauseAll( false );

			cTalk* pTalk = (cTalk*)g_pGameIF->GetDynamicIF( cBaseWindow::WT_TALK );					
			if( pTalk )
			{
				// 토크 중복 생성 되지 않게
				if( pTalk->GetTarget() != g_pNpcMng->GetZoomNpc() )
					pTalk->SetTarget( g_pNpcMng->GetZoomNpc(), false );
			}
		}				
		break;
	}
}

cBaseWindow::eMU_TYPE
cBGSprite::Update_ForMouse()
{
	if( !m_bVisible )
		return MUT_OUT_WINDOW;	

	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();
//	if( muReturn == MUT_OUT_WINDOW )	
//		return muReturn;

	switch( m_eValueType )
	{
// 	case DAILY_EVENT:		//아이템 툴팁 뿌려준다.
// 		{
// 			CsRect			rc, rc2, rc3;
// 			rc.SetRect( CsPoint( -39, 0 ), CsSIZE( 32, 32 ) );	
// 			rc2.SetRect( CsPoint( -72, 0 ), CsSIZE( 32, 32 ) );
// 			rc3.SetRect( CsPoint( -105, 0 ), CsSIZE( 32, 32) );
// 			CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );
// 			if( m_nValue1 != 0 )
// 			{
// 				if( rc.PtInRect( ptMouseLocalPos ) == TRUE )
// 				{
// 					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_nValue1, cBaseWindow::WT_STORE );
// 				}
// 			}
// 			if( m_nValue2 != 0 )
// 			{
// 				if( rc2.PtInRect( ptMouseLocalPos ) == TRUE )
// 				{
// 					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_nValue2, cBaseWindow::WT_STORE );
// 				}
// 			}
// 			if( m_nValue3 != 0 )
// 			{
// 				if( rc3.PtInRect( ptMouseLocalPos ) == TRUE )
// 				{
// 					TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip( GetRootClient(), CsPoint( 32, 32 ), TOOLTIP_MAX_SIZE, cTooltip::ITEM, m_nValue3, cBaseWindow::WT_STORE );
// 				}
// 			}
// 		}				
// 		break;
	case ATTENDANCE_EVENT:
		{
			tm s = nsCsFileTable::g_pEventMng->GetAttendanceInfo()->s_StartTime;
			tm e = nsCsFileTable::g_pEventMng->GetAttendanceInfo()->s_EndTime;
			std::wstring wsTime = UISTRING_TEXT( "BGSPRITE_ATTENDANCE_EVENT_TIME" );
			DmCS::StringFn::Replace( wsTime, L"#SYear#",	s.tm_year + 1900 );
			DmCS::StringFn::Replace( wsTime, L"#SMon#",		s.tm_mon + 1 );
			DmCS::StringFn::Replace( wsTime, L"#SDay#",		s.tm_mday );
			DmCS::StringFn::Replace( wsTime, L"#EYear#",	e.tm_year + 1900 );
			DmCS::StringFn::Replace( wsTime, L"#EMon#",		e.tm_mon + 1 );
			DmCS::StringFn::Replace( wsTime, L"#EDay#",		e.tm_mday );

			TOOLTIPMNG_STPTR->GetTooltip()->SetTooltip_Text( CURSOR_ST.GetPos(), CsPoint( 0, 0 ), wsTime.c_str() );

			if( g_pDataMng->GetAttendance()->IsEnableAttendance() )
			{
				if( m_pButtonArray[ 0 ]->Update_ForMouse() == cButton::ACTION_CLICK )
				{
					if( net::game )
						net::game->SendAttendance();
				}
			}
			else
			{
				if( m_pButtonArray[ 1 ]->Update_ForMouse() == cButton::ACTION_CLICK )
				{
				}
			}
		}
		break;
	case PCBANG:
		if( m_pCancelButton->Update_ForMouse() == cButton::ACTION_CLICK )
			Close();
		break;
// 	case TUTORIAL:
// 		{
// 			// 위에 바 드래그해야만 이동 시키도록 하겠다.
// 			if(CURSOR_ST.CheckClickBox( CsRect(GetRootClient(), GetRootClient()+m_pSpriteArray[0]->GetSize()))  
// 				!= CURSOR::BUTTON_OUTWINDOW )
// 			{
// 				_UpdateMoveWindow_ForMouse();
// 			}
// 			if(m_pButtonArray[ 0 ])
// 			{
// 				if( m_pButtonArray[ 0 ]->Update_ForMouse() == cButton::ACTION_CLICK )
// 				{
// 					cPrintMsg::PrintMsg( 30410 );
// 				}
// 			}
// 		}
// 		break;
#ifdef CROSSWARS_SYSTEM
	case CROSSBUTTON:
		{
			if( m_pButtonArray[ 0 ]->Update_ForMouse() == cButton::ACTION_CLICK )
			{
				{	//크로스워즈도 진화제한구역에선 진화 불가능하도록 수정 chu8820 14.04.09
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
								return muReturn;
							}
						}
					}
				}

				if( g_pMngCollector->GetSceneState() == CMngCollector::eSCENE_DATS ) {
					return muReturn;
				}

				CDigimonUser* pUser = g_pCharMng->GetDigimonUser( 0 );
				if( pUser->IsBattle() == true )
				{
					cPrintMsg::PrintMsg( 16002 );
					return muReturn;
				}
				if( pUser->IsRide() )
				{
					cPrintMsg::PrintMsg( 30014 );
					return muReturn;
				}
				if(nsCsGBTerrain::g_nSvrLibType == nLIB::SVR_BATTLE)
					return muReturn;


				if( g_pDataMng->GetServerSync()->IsChageDigimon() == true )
				{
					cWindow::PlaySound( cWindow::SD_MSS1 );
					return muReturn;
				}


				cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();

				// 크로스 테이블 뒤져서 자기가 데리고있는디지몬이 크로스 디지몬이면..
				DWORD dwMyDigimon = g_pCharMng->GetDigimonUser(0)->GetBaseDigimonFTID();
				bool bSummoning = nsCsFileTable::g_pDigimonMng->IsCrossDigimon(dwMyDigimon);	

				if(bSummoning)
				{					
					// 해제 패켓보낸다.
					g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CROSSRECOVER_PARTNER, 0);
					cWindow::PlaySound( cWindow::SD_Bu4 );
					return muReturn;
				}					
				// 크로스워즈 소환이 준비됐다.
				else if(pCrossTactics->GetCrossEnable())//
				{
					// 코드 크라운 체크
					if(pCrossTactics->CheckCodeCrown())//
					{
						int nCnt = pCrossTactics->GetCrossCnt();

						g_pDataMng->GetServerSync()->SendServer( cData_ServerSync::CROSSCHANGE_PARTNER, 0, &nCnt, pCrossTactics->GetCrossIdx() ); 	
						cWindow::PlaySound( cWindow::SD_Bu4 );
					}
					else
					{
						// 선택 리셋
						pCrossTactics->ResetCrossIdx();
						cWindow::PlaySound( cWindow::SD_MSS1 );
					}
					return muReturn;
				}
				else
				{
					cWindow::PlaySound( cWindow::SD_MSS1 );
					return muReturn;
				}
			


				
			}
		}
		break;
#endif

	}

	return muReturn;
}

void cBGSprite::Render()
{
	if( !m_bVisible )
		return;	

	switch( m_eValueType )
	{
	case CAPSULE_COMP:
		switch( nsCsFileTable::g_pItemMng->GetItem( m_nValue1 )->GetInfo()->s_nType_S )
		{		
		case 12:		// 캡슐 B
			m_pImageArray[ 0 ]->SetState( cImage::NORMAL1 );
			break;
		case 13:		// 캡슐 C
			m_pImageArray[ 0 ]->SetState( cImage::NORMAL2 );
			break;
		case 14:		// 캡슐 D
			m_pImageArray[ 0 ]->SetState( cImage::NORMAL3 );
			break;
		default:			// 캡슐 A
			m_pImageArray[ 0 ]->SetState( cImage::NORMAL0 );
		}
		break;
	}

	RenderScript();

	switch( m_eValueType )
	{	
// 	case DAILY_EVENT:		//아이템 아이콘 뿌려준다.
// 		{
// 			//m_strlist.Render( GetRootClient() + CsPoint( 42, 3 ), 2 );
// 
// 			if( m_nValue1 != 0 )
// 			{
// 				g_pIconMng->RenderItem( m_nValue1, GetRootClient() + CsPoint( -39, 0 ) );	
// 			}
// 			if( m_nValue2 != 0 )
// 			{
// 				g_pIconMng->RenderItem( m_nValue2, GetRootClient() + CsPoint( -72, 0 ) );
// 			}
// 			if( m_nValue3 != 0 )
// 			{
// 				g_pIconMng->RenderItem( m_nValue3, GetRootClient() + CsPoint( -105, 0 ) );
// 			}
// 
// 			CsPoint pos = CsPoint( 0, 0 );
// 			CsPoint size = CsPoint( 114, 30 );//MAX_BAR_LEN
// 			pos.x += (int)m_fCurPos;
// 			size.x -= (int)m_fCurPos;
// 
// 
// 			if( m_fMinute != 0 || m_fSecond != 0 )
// 			{
// 				m_pSpriteArray[ 1 ]->Render();					//EventButton\\Event_gage.tga
// 				m_pSpriteArray[ 0 ]->Render( pos, size );		//EventButton\\PlayTime_back.tga
// 			}
// 
// 			if( m_fMinute == 0 && m_fSecond == 0 )
// 			{
// 				m_pButtonArray[ 0 ]->Render();						// EventButton\\PlayTime_button.tga
// 			}
// 							
// 			m_strlist.Render( GetRootClient() + CsPoint( 42, 3 ), 2 );
// 		}
// 		break;
	case ATTENDANCE_EVENT:
		{
			if( g_pDataMng->GetAttendance()->IsEnableAttendance() )
				m_pButtonArray[ 0 ]->Render();
			else
				m_pButtonArray[ 1 ]->Render();

			m_strlist.Render( GetRootClient() + CsPoint( 42, 3 ), 2 );
		}
		break;
	case PCBANG:
		{
			m_pCancelButton->Render();			
		}
		break; 

	case CAPSULE_COMP:
		{
			m_pImageArray[ 0 ]->Render();
		}
		break;

// 	case TUTORIAL:
// 		{	
// 			if(m_pSpriteArray[0])
// 				m_pSpriteArray[ 0 ]->Render( GetRootClient() );
// 			if(m_pButtonArray[0])
// 				m_pButtonArray[ 0 ]->Render( );
// 		}
// 		break;
#ifdef CROSSWARS_SYSTEM
	case CROSSBUTTON:
		{
			// 크로스 테이블 뒤져서 자기가 데리고있는디지몬이 크로스 디지몬이면..
			DWORD dwMyDigimon = g_pCharMng->GetDigimonUser(0)->GetBaseDigimonFTID();
			bool bSummoning = nsCsFileTable::g_pDigimonMng->IsCrossDigimon(dwMyDigimon);

			cData_CrossTactics* pCrossTactics = g_pDataMng->GetCrossTatics();

			CTamerUser* pTamerUser = g_pCharMng->GetTamerUser();
			CTamerUser::sUSER_STAT* pUserStat = (CTamerUser::sUSER_STAT*)pTamerUser->GetBaseStat();

			//pUserStat->SetCR(4000);
			//pUserStat->SetMaxCR(10000);

			if(bSummoning)
			{		

				CsPoint Size( 59, 54 );

				// Size 값이 높을수록 0%에 가까움 0(100%) ~ 64(0%)
				//dwExp = 9999;

				DWORD dwCRRate, dwSizeRate;
				CsPoint	Pos = CsPoint::ZERO;

				int nRequre = nsCsFileTable::g_pDigimonMng->GetCrossDigimon(dwMyDigimon)->GetInfo()->s_nCodeCrownRequire;
				float CR =  (float)(pUserStat->GetCR() - nRequre);

				dwCRRate =  (DWORD)(( CR/pUserStat->GetMaxCR() ) * 100);
				dwSizeRate = (DWORD)((Size.y * 0.01) * dwCRRate);
				Pos = CsPoint(0, (Size.y - dwSizeRate) );
				
				m_pSpriteArray[ 0 ]->RenderSquare( CsPoint(GetRootClient().x -4, GetRootClient().y -4),
					NiPoint2( 0.0f, (float)Pos.y ),
					NiPoint2( 0.0f, 54.0f ),
					NiPoint2( 59.0f, 54.0f ),
					NiPoint2( 59.0f, (float)Pos.y ));
			}

			

			if(0 >= pUserStat->GetCR())
				m_pButtonArray[ 0 ]->SetColor( FONT_GLAY );
			else
				m_pButtonArray[ 0 ]->SetColor( FONT_WHITE );

			m_pButtonArray[ 0 ]->Render();

			{
				// CsPoint( 54, 49 )
				CsPoint Size( 51, 46 );			

				DWORD dwCRRate, dwSizeRate;
				CsPoint	Pos = CsPoint::ZERO;


				dwCRRate =  (DWORD)(pUserStat->GetCRRate() * 100);				
				dwSizeRate = (DWORD)((Size.y * 0.01) * dwCRRate);
				Pos = CsPoint(0, (Size.y - dwSizeRate));
				//Pos = CsPoint(0, (Size.y - dwSizeRate) - 46);


				m_pSpriteArray[ 1 ]->RenderSquare(CsPoint(GetRootClient().x, GetRootClient().y), 
					NiPoint2( 0.0f, 0.0f ),
					NiPoint2( 0.0f, (float)Pos.y ),
					NiPoint2( 51.0f, (float)Pos.y ),
					NiPoint2( 51.0f, 0.0f ));
			}

			if(0 != pCrossTactics->GetCrossID())
				m_pSpriteArray[ 2 ]->Render();
			
		}
		break;
#endif
	case RAID_REWARD:
		{			
			if( (uint)m_nValue1 < m_vValue1.size()){				
				g_pIconMng->RenderItem( m_vValue1[m_nValue1], m_InitPos, CsPoint( 26, 26 ), m_fValue2 );
				
			}
			m_pSpriteAniArray[ 0 ]->Render();
		}
		break;
#ifdef TRICK_ORTREAT
	case TIME_ITEM:
		{
			int nSizeX = 28;
			int nSizeY = 40;
			int nTick = (int)( m_fValue1 * 1000 );
			int nRes = nTick / 1000;				//	초로 환산
			int nRes_nMin = nRes / 60;				//	분
			int nRes_nMin_10 = nRes_nMin / 10;		//	분 ( 10 의 자리 )
			int nRes_nMin_1 = nRes_nMin % 10;		//	분 ( 1  의 자리 )
			int nRes_nSec = nRes % 60;				//	남은 초
			int nRes_nSec_10 = nRes_nSec / 10;		//	남은 초 ( 10 의 자리 )
			int nRes_nSec_1 = nRes_nSec % 10;		//	남은 초 ( 1 의 자리 )		

			//CsPoint pos = GetRootClient();
			//CsPoint pos = CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 2 ) - CsPoint( -62, 123 );
			//CsPoint pos2 = CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 2 ) - CsPoint(-50, 130 );
	
			CsPoint pos =  CsPoint( 12, 7 );
			//CsPoint pos2 =  CsPoint( 0, 0 );

			m_pImageArray[ 0 ]->Render();

			if( nRes_nMin_10 > 4 )			
				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_10/5.0f, 0.5f ), NiPoint2( ( nRes_nMin_10 + 1 )/5.0f, 1.0f ) );
			else
				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_10/5.0f, 0 ), NiPoint2( ( nRes_nMin_10 + 1 )/5.0f, 0.5f ) );

			pos.x += nSizeX + 3;			
			if( nRes_nMin_1 > 4 )			
				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_1/5.0f, 0.5f ), NiPoint2( ( nRes_nMin_1 + 1 )/5.0f, 1.0f ) );
			else
				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_1/5.0f, 0 ), NiPoint2( ( nRes_nMin_1 + 1 )/5.0f, 0.5f ) );

			pos.x += nSizeX + 1;
			m_pSpriteArray[ 1 ]->Render( CsPoint( pos.x, pos.y + ( nSizeY - nSizeY/3 )/2 + 2 ), CsPoint( nSizeX/4, nSizeY/3 ) );
			pos.x += nSizeX/4 + 1;

			if( nRes_nSec_10 > 4 )			
				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_10/5.0f, 0.5f ), NiPoint2( ( nRes_nSec_10 + 1 )/5.0f, 1.0f ) );
			else
				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_10/5.0f, 0 ), NiPoint2( ( nRes_nSec_10 + 1 )/5.0f, 0.5f ) );

			pos.x += nSizeX + 3;
			if( nRes_nSec_1 > 4 )			
				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_1/5.0f, 0.5f ), NiPoint2( ( nRes_nSec_1 + 1 )/5.0f, 1.0f ) );
			else
				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_1/5.0f, 0 ), NiPoint2( ( nRes_nSec_1 + 1 )/5.0f, 0.5f ) );
			
		}
		break;
#endif

// 	case MATCH_PLAY_COUNT:
// 		{
// 			int nSizeX = 28;
// 			int nSizeY = 40;
// 			int nTick = (int)( m_fValue1 * 1000 );
// 			int nRes = nTick / 1000;				//	초로 환산
// 			int nRes_nMin = nRes / 60;				//	분
// 			int nRes_nMin_10 = nRes_nMin / 10;		//	분 ( 10 의 자리 )
// 			int nRes_nMin_1 = nRes_nMin % 10;		//	분 ( 1  의 자리 )
// 			int nRes_nSec = nRes % 60;				//	남은 초
// 			int nRes_nSec_10 = nRes_nSec / 10;		//	남은 초 ( 10 의 자리 )
// 			int nRes_nSec_1 = nRes_nSec % 10;		//	남은 초 ( 1 의 자리 )
// 			int nRes_LowSec = ( nTick % 1000 ) / 10;
// 			int nRes_LowSec_10 = nRes_LowSec / 10;
// 			int nRes_LowSec_1 = nRes_LowSec % 10;
// 			
// 			CsPoint pos = GetRootClient();
// 			if( m_fValue1 < 16.0f )
// 			{
// 				pos = CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 6 ) - CsPoint( 380/2, 0 );
// 				nSizeX = 56;
// 				nSizeY = 80;
// 
// 				m_pSpriteArray[ 0 ]->SetColor( NiColor( 1, 0, 0 ) );
// 				m_pSpriteArray[ 1 ]->SetColor( NiColor( 1, 0, 0 ) );
// 			}
// 
// 			if( nRes_nMin_10 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_10/5.0f, 0.5f ), NiPoint2( ( nRes_nMin_10 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_10/5.0f, 0 ), NiPoint2( ( nRes_nMin_10 + 1 )/5.0f, 0.5f ) );
// 
// 			pos.x += nSizeX + 3;			
// 			if( nRes_nMin_1 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_1/5.0f, 0.5f ), NiPoint2( ( nRes_nMin_1 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_1/5.0f, 0 ), NiPoint2( ( nRes_nMin_1 + 1 )/5.0f, 0.5f ) );
// 
// 			pos.x += nSizeX + 1;
// 			m_pSpriteArray[ 1 ]->Render( CsPoint( pos.x, pos.y + ( nSizeY - nSizeY/3 )/2 + 2 ), CsPoint( nSizeX/4, nSizeY/3 ) );
// 			pos.x += nSizeX/4 + 1;
// 
// 			if( nRes_nSec_10 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_10/5.0f, 0.5f ), NiPoint2( ( nRes_nSec_10 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_10/5.0f, 0 ), NiPoint2( ( nRes_nSec_10 + 1 )/5.0f, 0.5f ) );
// 
// 			pos.x += nSizeX + 3;
// 			if( nRes_nSec_1 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_1/5.0f, 0.5f ), NiPoint2( ( nRes_nSec_1 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_1/5.0f, 0 ), NiPoint2( ( nRes_nSec_1 + 1 )/5.0f, 0.5f ) );
// 
// 			pos.x += nSizeX + 1;
// 			m_pSpriteArray[ 1 ]->Render( CsPoint( pos.x, pos.y + ( nSizeY - nSizeY/3 )/2 + 2 ), CsPoint( nSizeX/4, nSizeY/3 ) );
// 			pos.x += nSizeX/4 + 1;
// 
// 			if( nRes_LowSec_10 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_LowSec_10/5.0f, 0.5f ), NiPoint2( ( nRes_LowSec_10 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_LowSec_10/5.0f, 0 ), NiPoint2( ( nRes_LowSec_10 + 1 )/5.0f, 0.5f ) );
// 
// 			pos.x += nSizeX + 3;
// 			if( nRes_LowSec_1 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_LowSec_1/5.0f, 0.5f ), NiPoint2( ( nRes_LowSec_1 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_LowSec_1/5.0f, 0 ), NiPoint2( ( nRes_LowSec_1 + 1 )/5.0f, 0.5f ) );
// 		}
// 		break;
// 
// 	case MATCH_NEXT_COUNT:
// 		{
// 			m_pSpriteArray[ 2 ]->Render();
// 
// 			int nSizeX = 56;
// 			int nSizeY = 80;
// 			int nTick = (int)( m_fValue1 * 1000 );
// 			int nRes = nTick / 1000;				//	초로 환산
// 			int nRes_nMin = nRes / 60;				//	분
// 			int nRes_nMin_10 = nRes_nMin / 10;		//	분 ( 10 의 자리 )
// 			int nRes_nMin_1 = nRes_nMin % 10;		//	분 ( 1  의 자리 )
// 			int nRes_nSec = nRes % 60;				//	남은 초
// 			int nRes_nSec_10 = nRes_nSec / 10;		//	남은 초 ( 10 의 자리 )
// 			int nRes_nSec_1 = nRes_nSec % 10;		//	남은 초 ( 1 의 자리 )
// 
// 			CsPoint pos = CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 6 ) - CsPoint( 250/2, 0 );
// 			nSizeX = 56;
// 			nSizeY = 80;
// 
// 			if( nRes_nMin_10 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_10/5.0f, 0.5f ), NiPoint2( ( nRes_nMin_10 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_10/5.0f, 0 ), NiPoint2( ( nRes_nMin_10 + 1 )/5.0f, 0.5f ) );
// 
// 			pos.x += nSizeX + 3;			
// 			if( nRes_nMin_1 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_1/5.0f, 0.5f ), NiPoint2( ( nRes_nMin_1 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nMin_1/5.0f, 0 ), NiPoint2( ( nRes_nMin_1 + 1 )/5.0f, 0.5f ) );
// 
// 			pos.x += nSizeX + 1;
// 			m_pSpriteArray[ 1 ]->Render( CsPoint( pos.x, pos.y + ( nSizeY - nSizeY/3 )/2 + 2 ), CsPoint( nSizeX/4, nSizeY/3 ) );
// 			pos.x += nSizeX/4 + 1;
// 
// 			if( nRes_nSec_10 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_10/5.0f, 0.5f ), NiPoint2( ( nRes_nSec_10 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_10/5.0f, 0 ), NiPoint2( ( nRes_nSec_10 + 1 )/5.0f, 0.5f ) );
// 
// 			pos.x += nSizeX + 3;
// 			if( nRes_nSec_1 > 4 )			
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_1/5.0f, 0.5f ), NiPoint2( ( nRes_nSec_1 + 1 )/5.0f, 1.0f ) );
// 			else
// 				m_pSpriteArray[ 0 ]->Render( pos, CsPoint( nSizeX, nSizeY ), NiPoint2( nRes_nSec_1/5.0f, 0 ), NiPoint2( ( nRes_nSec_1 + 1 )/5.0f, 0.5f ) );
// 		}
// 		break;

#ifdef MINIGAME
	case MINIGAME_COUNT:
		{
			switch( m_eLifeType )
			{
			case LIFE_ALWAYS:
				{
					int iScale = 120;
					int nSec = ( (int)m_fValue1 ) % 10;
					CsPoint kOriginSize = CsPoint( 118, 134 );
					kOriginSize = (kOriginSize * iScale) / 100;
					CsPoint InitPos = CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 3 ) - kOriginSize / 2;
					if( nSec > 4 )			
						m_pSpriteArray[ 0 ]->Render( InitPos, kOriginSize, NiPoint2( ( nSec + 1 )/5.0f, 0.5f ), NiPoint2( ( nSec + 2 )/5.0f, 1.0f ) );
					else
						m_pSpriteArray[ 0 ]->Render( InitPos, kOriginSize, NiPoint2( ( nSec + 1 )/5.0f, 0 ), NiPoint2( ( nSec + 2 )/5.0f, 0.5f ) );
				}
				break;
			}			
		}
		break;
#endif
	}
}

void cBGSprite::ResetDevice()
{	
	ResetDeviceScript(); 

	for( int i=0; i<IF_BG_SPRITE_CNT; ++i )
	{
		if( m_pSpriteArray[ i ] != NULL )
		{
			m_pSpriteArray[ i ]->ResetDevice();
		}
	}

	for( int i=0; i<IF_BG_IMAGE_CNT; ++i )
	{
		if( m_pImageArray[ i ] != NULL )
		{
			m_pImageArray[ i ]->ResetDevice();
		}
	}

	for( int i=0; i<IF_BG_BUTTON_CNT; ++i )
	{
		if( m_pButtonArray[ i ] != NULL )
		{
			m_pButtonArray[ i ]->ResetDevice();
		}
	}

	m_strlist.ResetDevice();
}

//===============================================================================================================
//
//	공용
//
//===============================================================================================================

void cBGSprite::SetTime( DWORD dwTime )
{	
	m_eLifeType = LIFE_TIME;
	m_LifeTimeSeq.SetDeltaTime( dwTime );
	m_LifeTimeSeq.OffReset();
}

void cBGSprite::SetValue1( int nValue )
{ 
	m_nValue1 = nValue;
}
void cBGSprite::SetValue2( int nValue )
{
	m_nValue2 = nValue;
}

void cBGSprite::SetValue3( int nValue )
{
	m_nValue3 = nValue;
}
void cBGSprite::SetfValue1(const float& fValue)
{
	m_fValue1 = fValue;
}
int cBGSprite::GetValue1() const
{
	return m_nValue1;
}

int cBGSprite::GetValue2() const
{
	return m_nValue2;
}

int cBGSprite::GetValue3() const
{
	return m_nValue3;
}

void cBGSprite::PushValue1( int nValue )
{ 
	m_vValue1.push_back( nValue );
}

//===============================================================================================================
//
//	선물 지급
//
//===============================================================================================================

// void cBGSprite::MakeDailyEvent()
// {
// 	cText::sTEXTINFO ti;
// 	ti.Init( &g_pEngine->m_FontSystem );
// 	ti.s_Color = FONT_GREEN;
// 
// 	TCHAR sz[ 128 ];		
// 
// 	//cString* pString = NiNew cString;
// 	//_stprintf_s( sz, 128, _LAN( "다음 이벤트 아이템 지급까지" ) );
// 	//ti.SetText( sz );
// 	//pString->AddText( &ti );
// 	//m_strlist.AddTail( pString );
// 
// 	cString* pString = NiNew cString;
// 	_stprintf_s( sz, 128, _LAN( "'%.2d분 %.2d초'" ), ( m_LifeTimeSeq.RemainDeltaTime() / 1000 ) / 60, ( m_LifeTimeSeq.RemainDeltaTime() / 1000 ) % 60 );		
// 	ti.SetText( sz );
// 	pString->AddText( &ti, CsPoint(-35, 8) );
// 	m_strlist.AddTail( pString );
// }

//===============================================================================================================
//
//	출석
//
//===============================================================================================================

void cBGSprite::MakeAttendance()
{
	assert_cs( m_pButtonArray[ 0 ] == NULL );
	assert_cs( m_pButtonArray[ 1 ] == NULL );
	m_pButtonArray[ 0 ] = NiNew cButton;
	m_pButtonArray[ 0 ]->Init( GetRoot(), CsPoint( 5, 3 ), CsPoint( 32, 32 ), "Event\\AttendanceBtnEn.tga", false );
	m_pButtonArray[ 0 ]->SetTexToken( CsPoint( 0, 38 ) );
	m_pButtonArray[ 1 ] = NiNew cButton;
	m_pButtonArray[ 1 ]->Init( GetRoot(), CsPoint( 5, 3 ), CsPoint( 32, 32 ), "Event\\AttendanceBtnDis.tga", false );
	m_pButtonArray[ 1 ]->SetTexToken( CsPoint( 0, 38 ) );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem );
	ti.s_Color = FONT_GREEN;
	
	bool bAttendance = g_pDataMng->GetAttendance()->IsEnableAttendance();

	cString* pString = NiNew cString;
	ti.SetText( UISTRING_TEXT( "BGSPRITE_ATTENDANCE_EVENT_TIME_CHECK" ).c_str() );
	pString->AddText( &ti );
	m_strlist.AddTail( pString );

	if( bAttendance == false )
	{
		std::wstring wsRemain;
		u4 timeTS = _TIME_TS;
		if( g_pDataMng->GetAttendance()->GetTodayAttendanceTimeTS() >= timeTS )
		{
			u4 nTotalSec = g_pDataMng->GetAttendance()->GetTodayAttendanceTimeTS() - timeTS;
			assert_cs( nTotalSec / ( 60 * 60 ) == 0 );
			u4 nMin = nTotalSec / 60;
			u4 nSec = nTotalSec % 60;
			if( nMin )
			{
				wsRemain = UISTRING_TEXT( "BGSPRITE_REMAIN_TIME_MIN_SEC" );
				DmCS::StringFn::Replace( wsRemain, L"#Min#", nMin );
				DmCS::StringFn::Replace( wsRemain, L"#Sec#", nSec );
			}
			else
			{
				wsRemain = UISTRING_TEXT( "BGSPRITE_REMAIN_TIME_SEC" );
				DmCS::StringFn::Replace( wsRemain, L"#Sec#", nSec );
			}
		}
		else
		{
			wsRemain = UISTRING_TEXT( "BGSPRITE_REMAIN_TIME_SEC_ZERO" );
		}
		
		ti.SetText( wsRemain.c_str() );
		pString->AddText( &ti );
		m_strlist.AddTail( pString );
	}
	else
	{
		cString* pString = NiNew cString;
		ti.SetText( UISTRING_TEXT( "BGSPRITE_REMAIN_TIME_NONE" ).c_str() );
		pString->AddText( &ti );
		m_strlist.AddTail( pString );
	}		
}

void cBGSprite::PassTime( int nSec )
{
	m_LifeTimeSeq.PassDeltaTime( nSec );
}

