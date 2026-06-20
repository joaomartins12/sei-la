#include "stdafx.h"
#include "CardEvent.h"

#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"

cCardEvent::cCardEvent():m_pTarget(NULL)
{

}

cCardEvent::~cCardEvent()
{
	m_pTarget = NULL;
}

void cCardEvent::Destroy()
{
	if( g_pNpcMng->Get_RenderPos() == true )
	{
		g_pNpcMng->Set_RenderPos( false );
		g_pNpcMng->Reset_RenderAni();
	}

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();
}

void cCardEvent::DeleteResource()
{
	cSeparate::DeleteInstance( GetWindowType(), 0 );

	DeleteScript();

	if( m_pTarget )
	{
		CsC_AniProp* pAni = m_pTarget->GetProp_Animation();
		if( pAni )
			pAni->SetUseIdleAni(true);	// Idle Ani 해제
	}

	m_pTarget = NULL;

	SAFE_NIDELETE( m_pCardBack );	
	for(int i=0; i< EVENT_MAX_CARD; ++i)
		SAFE_NIDELETE( m_CardFront[i].s_pCardFront );
	SAFE_NIDELETE( m_pCompButton );
	SAFE_NIDELETE( m_pTakeButton );

	SAFE_NIDELETE( m_pAniCardSelect );

}

void cCardEvent::Create( int nValue /*= 0*/ )
{
	if( cBaseWindow::Init() == false )
		return;

	m_pTarget = g_pNpcMng->GetNpc( GetSystem()->getNpcID() );
	m_nItemID = GetSystem()->getResultItemCode();

	InitScript( "CardGame\\Card_Table.tga", CsPoint( (int)(g_nScreenWidth * 0.05f), (int)(g_nScreenHeight * 0.45f) ), CsPoint( (int)(g_nScreenWidth * 0.9f), (int)(g_nScreenHeight * 0.38f) ), false, IFREGION_X::LEFT, IFREGION_Y::JUST_CENTER );

	if( g_pNpcMng->Get_RenderPos() == false )
		g_pNpcMng->Set_RenderPos( true );

	int nBetweenWidth = Card_Init();	// nBetweenWidth : 카드 사이 폭

	m_pCompButton = NiNew cButton;	// 완료 확인 버튼
	m_pCompButton->Init( m_pRoot, CsPoint( (m_EventCard[2].s_rcCard.left + (m_EventCard[2].s_rcCard.right - m_EventCard[2].s_rcCard.left) / 2) - 40 , 
		m_EventCard[4].s_rcCard.bottom + ( ( (int)(g_nScreenHeight * 0.38f - CARD_HEIGHT) / 2 ) / 2) - 6 ), CsPoint( BUTTON_WIDTH, BUTTON_HEIGHT ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "CARDEVENT_BTN_CLOSE" ).c_str(), false );

	m_pTakeButton = NiNew cButton;	// 아이템 받기 버튼
	m_pTakeButton->Init( m_pRoot, CsPoint( (m_EventCard[2].s_rcCard.left + (m_EventCard[2].s_rcCard.right - m_EventCard[2].s_rcCard.left) / 2) - 40 , 
		m_EventCard[4].s_rcCard.bottom + ( ( (int)(g_nScreenHeight * 0.38f - CARD_HEIGHT) / 2 ) / 2) - 6 ), CsPoint( BUTTON_WIDTH, BUTTON_HEIGHT ), cButton::IMAGE_NOR_1, UISTRING_TEXT( "CARDEVENT_BTN_GET_ITEM" ).c_str(), false );

	m_pTakeButton->AddEvent( cButton::BUTTON_LBDOWN_EVENT, this, &cCardEvent::getResultItemIven );
}

int cCardEvent::Card_Init()
{
	m_pCardBack = NiNew cSprite;	// 카드 뒷면
	m_pCardBack->Init( NULL, CsPoint::ZERO, CsPoint( CARD_WIDTH, CARD_HEIGHT ), "CardGame\\Card_Back.tga", false );

	/************************************************************************/
	/*                               아이템                                 */
	/************************************************************************/
	// "CardGame\\Card_Item1.tga" : 에보류터
	// "CardGame\\Card_Item2.tga" : 모드셀렉터
	// "CardGame\\Card_Item3.tga" : 1Bit
	// "CardGame\\Card_Item4.tga" : 부화백업디스크
	// "CardGame\\Card_Item5.tga" : 엑셀레이터
	// "CardGame\\Card_Item6.tga" : 백업디스크
	// "CardGame\\Card_Item7.tga" : 수수알
	// "CardGame\\Card_Item8.tga" : 치킨콤보
	// 위의 순서는 언제든 바뀔 수 있음

#ifdef VERSION_USA
	// 카드 아이템 : 에보류터, 모드셀렉터, 1Bit, 부화백업디스크, 엑셀레이터
	int nItemArry[ EVENT_MAX_CARD ] = {0, };
	
	CsNpcMng::sNPC_EVENT_INFO* pInfo = nsCsFileTable::g_pNpcMng->GetCardGameInfo( GetSystem()->getNpcID() );
	assert_cs( pInfo != NULL );
	for( int i=0; i<EVENT_MAX_CARD; ++i )
	{
		nItemArry[i] = pInfo->s_aItemInfo[i].s_nItemID;
	}

	m_CardFront[0].s_Path = "CardGame\\Card_Item1.tga";
	m_CardFront[1].s_Path = "CardGame\\Card_Item2.tga";
	m_CardFront[2].s_Path = "CardGame\\Card_Item3.tga";
	m_CardFront[3].s_Path = "CardGame\\Card_Item4.tga";
	m_CardFront[4].s_Path = "CardGame\\Card_Item5.tga";

	for( int i = 0 ; i < EVENT_MAX_CARD ; i++ )
	{
		m_CardFront[i].s_pCardFront = NiNew cSprite;
		m_CardFront[i].s_pCardFront->Init( NULL, CsPoint::ZERO, CsPoint( CARD_WIDTH, CARD_HEIGHT ), m_CardFront[i].s_Path, false );
		m_CardFront[i].s_nItemID = nItemArry[i];
	}
#else
	// 카드 아이템 ID
	int nItemArry[ EVENT_MAX_CARD ] = {0, };
	CsNpcMng::sNPC_EVENT_INFO* pInfo = nsCsFileTable::g_pNpcMng->GetCardGameInfo( GetSystem()->getNpcID() );
	assert_cs( pInfo != NULL );
	for( int i=0; i<EVENT_MAX_CARD; ++i )
	{
		nItemArry[i] = pInfo->s_aItemInfo[i].s_nItemID;
	}

	m_CardFront[0].s_Path = "CardGame\\Card_Item1.tga";
	m_CardFront[1].s_Path = "CardGame\\Card_Item2.tga";
	m_CardFront[2].s_Path = "CardGame\\Card_Item3.tga";
	m_CardFront[3].s_Path = "CardGame\\Card_Item4.tga";
	m_CardFront[4].s_Path = "CardGame\\Card_Item5.tga";

	for( int i = 0 ; i < EVENT_MAX_CARD ; i++ )
	{
		m_CardFront[i].s_pCardFront = NiNew cSprite;
		m_CardFront[i].s_pCardFront->Init( NULL, CsPoint::ZERO, CsPoint( CARD_WIDTH, CARD_HEIGHT ), m_CardFront[i].s_Path, false );
		m_CardFront[i].s_nItemID = nItemArry[i];
	}
#endif

	
	m_pAniCardSelect = NiNew cSpriteAni;
	m_pAniCardSelect->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( CARD_WIDTH + 40, CARD_HEIGHT + 110), "EnchantTactics\\SelectAni\\Card_C_", "tga", 20, false );
	m_pAniCardSelect->SetAniTime( 0.05f );

	float fTotalWidth = (g_nScreenWidth * 0.9f);	// 카드테이블 폭
	float fCardTotalWidth = CARD_WIDTH * EVENT_MAX_CARD;
	float fWidth = (fTotalWidth - fCardTotalWidth) / (EVENT_MAX_CARD + 1);	// 카드와 카드 사이 폭
	float fStartWidthPos = (g_nScreenWidth * 0.05f) + fWidth;	// 카드가 그려질 폭의 시작 위치
	float fAddWidth = 0.0f;
	float fStartHeightPos = 0.0f;
	{
		//float fRate = ( (g_nScreenHeight * 0.45f) + (g_nScreenHeight * 0.38f) ) / (float)g_pEngine->m_nOldScreenHeight;
		float fRate = (g_nScreenHeight / 2) + ( (g_nScreenHeight * 0.45f) - DEFAULT_SCREEN_HEIGHT / 2 );
		fStartHeightPos = fRate + ( (g_nScreenHeight * 0.38f - CARD_HEIGHT) / 2 );
	}

	for( int i = 0; i < EVENT_MAX_CARD; i++ )
	{
		m_EventCard[ i ].s_rcCard  = CsRect( CsPoint( (int)(fAddWidth + fWidth), (int)( (g_nScreenHeight * 0.38f - CARD_HEIGHT) / 2 ) ), CsSIZE( CARD_WIDTH, CARD_HEIGHT ) ); 
		m_EventCard[ i ].s_ptPos   = NiPoint2( fStartWidthPos + fAddWidth, fStartHeightPos );
		m_EventCard[ i ].s_bSelect = false;
		m_EventCard[ i ].s_nCardIndex = EVENT_MAX_CARD;
		fAddWidth += fWidth + CARD_WIDTH;
	}

	m_nTiming = m_nSelectCount = m_SwitchCount = 0;
	m_fCurTime = m_fOldTime = 0.0f;
	m_nResultIndex = m_nResultID = EVENT_MAX_CARD;
	m_bFirstRender = m_bTakeStatus = m_bPause = m_bSelectAni = false;

	for( int i=0; i< EVENT_MAX_CARD; ++i )
		m_Switch[i] = 0;

	return (int)fWidth;
}

bool cCardEvent::Close( bool bSound )
{ 
	return cBaseWindow::Close( bSound ); 
}

void cCardEvent::Update(float const& fDeltaTime)
{
	cSeparate::UpdateInstance( GetWindowType(), 0 );

	if( m_pTarget != NULL )
	{
		g_pNpcMng->SetZoomNpc( m_pTarget );
		//g_pNpcMng->SetZoomNpcType( cBaseWindow::WT_CARD_EVENT );

		// 테이머와의 거리 체크
		assert_cs( m_pTarget->GetLeafRTTI() == RTTI_NPC );
		if( ( (CNpc*)m_pTarget )->CheckTamerDist() == false )
		{
			//Close();
			return;
		}
	}
	
	if( m_nSelectCount == EVENT_MAX_CARD )
		m_pAniCardSelect->Update( fDeltaTime );	// 마지막 카드 선택 시 테두리 애니메이션 업데이트

}

cBaseWindow::eMU_TYPE cCardEvent::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cSeparate::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	muReturn = cBaseWindow::Update_ForMouse();
	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( (m_bTakeStatus == false) && (m_nSelectCount >= EVENT_MAX_CARD) )
	{
		// 아이템 받기 버튼 상태 업데이트
		if( m_pTakeButton->Update_ForMouse() != cButton::ACTION_NONE )
			return muReturn;
	}
	else if( m_bTakeStatus == true )
	{
		// 완료 버튼 상태 업데이트
		switch( m_pCompButton->Update_ForMouse() )
		{
		case cButton::ACTION_DOWN:
			return muReturn;
		case cButton::ACTION_CLICK:
			Close();
			return muReturn;
		default:
			return muReturn;
		}
	}	

	return muReturn;
}

cBaseWindow::eMU_TYPE cCardEvent::Update_ForMouse_Disable()	// ActiveWindow 이외에 마우스 Click 안되도록... 
{
	cBaseWindow::eMU_TYPE muReturn;
	muReturn = cItemSeparate::Update_ForMouseInstance( GetWindowType(), 0 );
	if( muReturn != MUT_OUT_WINDOW )	
		return muReturn;

	return cBaseWindow::Update_ForMouse_Disable();
}

void cCardEvent::Render()
{
	RenderScript();

	// 유저에게 줄 카드 인덱스값 결정
	if( m_nResultID == EVENT_MAX_CARD )
	{
		for(int i=0; i<EVENT_MAX_CARD; ++i)
		{
			if( m_CardFront[i].s_nItemID == m_nItemID )
			{
				m_nResultID = i;
				if( m_pTarget )
				{
					CsC_AniProp* pAni = m_pTarget->GetProp_Animation();
					if( pAni )
						pAni->SetUseIdleAni(false);	// Idle Ani 설정
				}
			}
		}
	}

	// 완료 버튼 랜더링
	if( m_bTakeStatus == false)
		m_pTakeButton->Render();
	else
		m_pCompButton->Render();


	if( m_bFirstRender == false )	// 처음 피에몬 손동작에 의해 그려지는 뒷면 카드
	{
		m_fOldTime = g_fDeltaTime;

		m_fCurTime += g_fDeltaTime;

		if( m_fCurTime - m_fOldTime > 1.0f )
		{
			m_fOldTime = m_fCurTime;
			m_nTiming++;
		}

		for( int i=0; i<m_nTiming; ++i )
		{
			m_pCardBack->Render( CsPoint( (int)m_EventCard[ i ].s_ptPos.x , (int)m_EventCard[ i ].s_ptPos.y ) );
		}

		if( m_nTiming == EVENT_MAX_CARD )
			m_bFirstRender = true;
	}
	else	// 손동작 완료 후 랜더링
	{
		srand( (unsigned)time(NULL) );

		for( int i = 0; i < EVENT_MAX_CARD; i++ )
		{
			if( m_EventCard[i].s_bSelect == false )	// 뒷면 랜더링
				m_pCardBack->Render( CsPoint( (int)m_EventCard[ i ].s_ptPos.x , (int)m_EventCard[ i ].s_ptPos.y ) );
			else	// 앞면 랜더링
			{
				int TempIndex;

				while( m_nSelectCount != EVENT_MAX_CARD && m_SwitchCount < 4 )
				{
					TempIndex = rand() % EVENT_MAX_CARD;	// 카드 선택 시 이미 결정된 카드 이외에 랜덤하게 카드를 보여줌

					if( TempIndex == m_nResultID )
						continue;

					if( m_Switch[TempIndex] == 1 )
						continue;
					else
						break;
				}

				if( m_EventCard[i].s_nCardIndex == EVENT_MAX_CARD && m_SwitchCount < 4 )
				{
					m_EventCard[i].s_nCardIndex = TempIndex;
					m_Switch[TempIndex] = 1;
					m_SwitchCount++;
				}

				if( m_EventCard[ i ].s_nCardIndex != EVENT_MAX_CARD )	// 유저에게 보여줄 카드 랜더링
					m_CardFront[m_EventCard[ i ].s_nCardIndex].s_pCardFront->Render( CsPoint( (int)m_EventCard[ i ].s_ptPos.x , (int)m_EventCard[ i ].s_ptPos.y ) );
				else
				{	// 유저에게 줄 카드를 랜더링
					m_CardFront[m_nResultID].s_pCardFront->Render( CsPoint( (int)m_EventCard[ m_nResultIndex ].s_ptPos.x , (int)m_EventCard[ m_nResultIndex ].s_ptPos.y ) );
					m_pAniCardSelect->Render( CsPoint( (int)m_EventCard[ m_nResultIndex ].s_ptPos.x - 5, (int)m_EventCard[ m_nResultIndex ].s_ptPos.y - 30) );
					if( m_bSelectAni == false )
					{
						if( m_pTarget )
						{
							CsC_AniProp* pAni = m_pTarget->GetProp_Animation();
							if( pAni )
							{
								pAni->SetAnimation( ANI::NPC_TALK );
								m_bSelectAni = true;
								pAni->SetUseIdleAni(false);	// Idle Ani 설정
							}
						}
					}
				}
			}
		}
	}

	EndRenderScript();
	cSeparate::RenderInstance( GetWindowType(), 0 );
}

void cCardEvent::OnMouseOutWindow()
{
	
}

void cCardEvent::OnLButtonDown( CsPoint pos )
{
	// 마우스 위치값 얻어오기
	CsPoint ptMouseLocalPos = MousePosToWindowPos( CURSOR_ST.GetPos() );		
	for( int i=0; i<EVENT_MAX_CARD; ++i )
	{
		if( m_EventCard[ i ].s_rcCard.PtInRect( ptMouseLocalPos ) == FALSE )
			continue;

		if( m_nSelectCount != EVENT_MAX_CARD && m_EventCard[ i ].s_bSelect == false )
		{
			m_nSelectCount++;
			m_EventCard[i].s_bSelect = true;

			if( m_nSelectCount == EVENT_MAX_CARD )
			{
				m_nResultIndex = i;
		
				sEventCard TempCard = m_EventCard[i];

				char* TempPath = m_CardFront[m_nResultID].s_Path;

				SAFE_NIDELETE( m_CardFront[m_nResultID].s_pCardFront );

				m_CardFront[m_nResultID].s_pCardFront = NiNew cSprite;
				m_CardFront[m_nResultID].s_pCardFront->Init( NULL, CsPoint::ZERO, CsPoint( (int)(CARD_WIDTH + 20), CARD_HEIGHT + 30 ), TempPath, false );

				m_EventCard[i].s_ptPos = NiPoint2( TempCard.s_ptPos.x - 10, TempCard.s_ptPos.y - 15 );
			}
		}
	}
}

void cCardEvent::OnLButtonUp( CsPoint pos )
{

}


void cCardEvent::ResetDevice()
{
	
}

void cCardEvent::getResultItemIven(void* pSender, void* pData)
{
	m_bTakeStatus = true;
	GetSystem()->SendCardEventEnd();
}

bool cCardEvent::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	//GetSystem()->Register( SystemType::E_OPEN_WINDOW, this );

	return true;
}

void cCardEvent::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
// 	switch(iNotifiedEvt)
// 	{
// 	case SystemType::E_OPEN_WINDOW:
// 		break;
// 	}
}