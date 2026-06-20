
#include "stdafx.h"

#ifdef MINIGAME
#include "MiniGame.h" 

cMinigame::cMinigame()
:m_pOutBtn(NULL)
, m_pStopBtn(NULL)
, m_pLetters(NULL)
, m_pUp(NULL)
, m_CurrGuageIndex(0)
, m_nSuccessCnt(0)
, m_bStartGaugeUpdate(false)
, m_bServerRecv(false)
, m_bStartReadyCountingDown(false)
{
	BarSpriteName[0] = "D";
	BarSpriteName[1] = "I";
	BarSpriteName[2] = "G";
	BarSpriteName[3] = "I_2";
	BarSpriteName[4] = "M";
	BarSpriteName[5] = "O";
	BarSpriteName[6] = "N";
}
cBaseWindow::eWINDOW_TYPE	 cMinigame::GetWindowType(void)
{
	return WT_MINIGAME; 
}
cBaseWindow::eMU_TYPE cMinigame::Update_ForMouse()
{
	cBaseWindow::eMU_TYPE muReturn = cBaseWindow::Update_ForMouse();

	if( !m_bStartGaugeUpdate )
		return MUT_OUT_WINDOW;

	if( muReturn == MUT_OUT_WINDOW )	
		return muReturn;

	if( m_pStopBtn && m_pStopBtn->Update_ForMouse() == cButton::ACTION_DOWN)
	{
		return muReturn;
	}
	if( m_pOutBtn && m_pOutBtn->Update_ForMouse() == cButton::ACTION_CLICK)
	{
		return muReturn;
	}
	return muReturn;
}
bool	cMinigame::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::EventMiniGame::_Start, this );
	GetSystem()->Register( SystemType::EventMiniGame::_ClickResult, this );
	GetSystem()->Register( SystemType::EventMiniGame::_TimeOut, this );
	GetSystem()->Register( SystemType::EventMiniGame::_OnEndMiniGame, this );
	return true;
}
bool	cMinigame::OnEscapeKey()
{
	return false; 
}
void	cMinigame::Destroy()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );

	cBaseWindow::Delete();	
}
void	cMinigame::DeleteResource()
{
	DeleteScript();

	m_pStopBtn = NULL;
	m_pOutBtn = NULL;

	for(int i = 0 ; i <MAX_BAR_BUTTON_COUNT ; i++)
		m_sBar[ i ].Delete();
}
void	cMinigame::ResetDevice()
{	
	ResetDeviceScript();

	m_pOutBtn->ResetDevice();
	m_pStopBtn->ResetDevice();

	for(int i = 0 ; i < MAX_BAR_BUTTON_COUNT ; i++)
		m_sBar[ i ].Delete();
}
bool	cMinigame::Close( bool bSound /* = true */  )
{
	if( IsEnableWindow() == false )
		return false;
	if( bSound )
		PlayCloseSound();

	if( GetSystem() != NULL){
		GetSystem()->Send_MiniGame_End(0);
		g_pGameIF->CloseDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_MINIGAME_COUNT, true );
	}
	SetShowWindow( false );
	Destroy();
	return true;
}
void	cMinigame::PlayOpenSound(void)
{ 
	cWindow::PlaySound( cWindow::SD_Ba1 ); 
}
void	cMinigame::PlayCloseSound(void)
{ 
	cWindow::PlaySound( cWindow::SD_Ba2 ); 
}
void	cMinigame::PreResetMap(void)
{ 
	Close( false ); 
}

void	cMinigame::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{ 
	switch(iNotifiedEvt)
	{
	case SystemType::EventMiniGame::_Start:
		OnMiniGameStart();		
		break;
	case SystemType::EventMiniGame::_ClickResult:
		OnRecvClickResult();
		break;
	case SystemType::EventMiniGame::_TimeOut:
		OnRecvClickTimeOut();
		break;
	case SystemType::EventMiniGame::_OnEndMiniGame:
		OnMiniGameEnd();
		break;
	}
}
void	cMinigame::OnMiniGameStart()
{
	SetGaugeChagingTime( 0, GetSystem()->GetStartBarTime() );
	g_pGameIF->GetDynamicIF( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_MINIGAME_COUNT, cBGSprite::MINIGAME_COUNT );
	SetGaugeStartTime();
	SetServRecv( true );
	m_bStartReadyCountingDown = true;

}

void	cMinigame::OnRecvClickResult()
{
	if(GetSystem()->GetClickResult())		//성공이면
		SetSuccessCnt( GetSuccessCnt() + 1 );

	const int iNextBarIndex = GetSystem()->GetClickResultNextBarIdx();
	const int iBarChargingTime = GetSystem()->GetNextBarChargingTime();
	SetCurrGaugeIdx( iNextBarIndex );

	u4 nClick = GetTickCount() - GetGauge( iNextBarIndex - 1 )->m_nStartTick;
	GetGauge( iNextBarIndex - 1 )->m_nStartTick = nClick;

	if( GetSystem()->GetClickResult() )
		SetGaugeState( iNextBarIndex - 1, cButton::SUCCESS );	// 바 상태 성공
	else
		SetGaugeState( iNextBarIndex - 1, cButton::FAIL );	// 바 상태 실패

	if(iNextBarIndex == 7)	//마지막 놈의 결과를 받으면
		GetSystem()->Send_MiniGame_End(1);
	else
	{
		SetGaugeStartTime();		//다음 바 채우기 위해 시작시간 셋팅
		SetGaugeChagingTime( iNextBarIndex , iBarChargingTime );
	}
	SetServRecv( true );
}
void	cMinigame::OnRecvClickTimeOut()
{
	ushort nCurBarIdx = GetCurrGaugeIdx();
	int nClick = GetTickCount() - GetGauge( nCurBarIdx )->m_nStartTick;
	GetGauge( nCurBarIdx )->m_nStartTick = nClick;

	SetGaugeState( nCurBarIdx, cButton::FAIL );	// 바 상태 실패로 

	SetCurrGaugeIdx( nCurBarIdx + 1 );	//인덱스 증가

	if( GetCurrGaugeIdx() == 7 )	//마지막 놈의 결과를 받으면
		GetSystem()->Send_MiniGame_End( 1 );	//정상 종료
	else
	{
		SetGaugeStartTime();		//다음 바 채우기 위해 시작시간 셋팅
		SetGaugeChagingTime( nCurBarIdx + 1 , GetSystem()->GetTimeOutNextBarTime() );
	}

	SetServRecv( true );

}
void	cMinigame::OnMiniGameEnd(void)
{
	SetSuccessCnt( GetSystem()->GetSuccessCnt() );
	SetStartGauge( true );	//카운트다운 완료 
}
void	cMinigame::OnClickStopKey(void* pSender, void* pData)
{
	if( m_bServerRecv == true && m_CurrGuageIndex < MAX_BAR_BUTTON_COUNT )	//성공실패여부 결정 나기 전 
	{
		if( 	GetTickCount() - m_sBar[ m_CurrGuageIndex ].m_nStartTick <= 100 )// 시작한지 0.1초 내에 다시 누른건 패스
			return;

		m_bServerRecv = false;
		GetSystem()->Send_MiniGame_ClickBt( HATCH, m_CurrGuageIndex);
	}
}

void	cMinigame::OnClickCloseKey(void* pSender, void* pData)
{
	Close();
}
void	cMinigame::Create( int nValue )
{
	if( cBaseWindow::Init() == false )
		return;
	CsPoint OriginSize = CsPoint( 280, 350 );
	CsPoint InitPos = CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 3 )- OriginSize / 2;
	InitScript( "MiniGame\\main.tga", InitPos, OriginSize, false, IFREGION_X::LEFT, IFREGION_Y::TOP );

	CreateButtons();
	CreateBars();

	m_nSuccessCnt = 0;
	m_bStartGaugeUpdate = false;
	m_bStartReadyCountingDown = false;
	m_bServerRecv = false;
}
void	cMinigame::CreateButtons(void)
{
	m_pStopBtn = AddButton( CsPoint( 108, 245 ), CsPoint( 70, 70 ), CsPoint( 0, 80 ), "MiniGame\\Mini_Stop.tga" );	
	m_pStopBtn->AddEvent(cButton::BUTTON_LBDOWN_EVENT, this, &cMinigame::OnClickStopKey);

	m_pOutBtn = AddButton( CsPoint( 195, 285 ), CsPoint( 70, 42 ), CsPoint( 0, 36 ), "MiniGame\\Mini_Out.tga" );	
	m_pOutBtn->AddEvent(cButton::BUTTON_LBUP_EVENT, this, &cMinigame::OnClickCloseKey);

	m_pLetters = AddButton(CsPoint(17, 250), CsPoint(72, 16),CsPoint(0, 12), "MiniGame\\Mini_Letters.tga" );
	m_pUp = AddButton(CsPoint(17, 278), CsPoint(72, 16),CsPoint(0, 12), "MiniGame\\Mini_Up.tga" );
}
void	cMinigame::CreateBars(void)
{
	std::string szTextureName;
	const int TextureWidth = 31;
	const int TextureHeight = 32;
	const int InitPosX = 32;
	const int InitPosY = 40;

	NiStencilPropertyPtr spPropStencilSrc = NiNew NiStencilProperty;
	spPropStencilSrc->SetStencilReference( 3 );
	spPropStencilSrc->SetStencilFunction( NiStencilProperty::TEST_ALWAYS );
	spPropStencilSrc->SetStencilPassAction( NiStencilProperty::ACTION_REPLACE );	
	spPropStencilSrc->SetStencilOn( true );

	NiStencilPropertyPtr spPropStencil = NiNew NiStencilProperty;
	spPropStencil->SetStencilReference( 3 );
	spPropStencil->SetStencilFunction( NiStencilProperty::TEST_EQUAL );
	spPropStencil->SetStencilPassAction( NiStencilProperty::ACTION_KEEP );
	spPropStencil->SetStencilOn( true );

	for(int i = 0 ; i < MAX_BAR_BUTTON_COUNT ; i++)//DIGIMON 7개 프로그래스바
	{

		DmCS::StringFn::FormatA(szTextureName, "MiniGame\\%s.tga", BarSpriteName[i].c_str());
		m_sBar[i].m_pBtn = AddButton(CsPoint(InitPosX + (i*TextureWidth), InitPosY), CsPoint( TextureWidth, TextureHeight ), CsPoint( 0, TextureHeight ), szTextureName.c_str() );//32

		m_sBar[ i ].m_pGaugeStencil = NiNew cSprite;
		m_sBar[ i ].m_pGaugeStencil->InitStencil( m_pRoot, CsPoint::ZERO, CsPoint( 31, 152 ), true, spPropStencilSrc );
		m_sBar[ i ].m_pGaugeStencil->SetAlpha( 0.0f );	

		m_sBar[ i ].m_pGaugeBar = NiNew cSprite;
		m_sBar[ i ].m_pGaugeBar->InitStencil( m_pRoot, CsPoint( 32 + (i*31), 73 ), CsPoint( 31, 152 ), "MiniGame\\bar_net.tga", false, spPropStencil );
	}
	SetCurrGaugeIdx(0);
}
void	cMinigame::Update(float const& fDeltaTime)
{	
	if(m_bStartReadyCountingDown == true )
		UpdateReadyCountdown(fDeltaTime);
	else if(m_bStartReadyCountingDown == false && m_bStartGaugeUpdate == true)
	{		
		UpdateGauges(fDeltaTime);
		_UpdateMoveWindow();
	}
}
void	cMinigame::UpdateReadyCountdown(const float& fDeltaTime)
{
	cBGSprite* pkCount = static_cast<cBGSprite*>(g_pGameIF->_GetPointer( cBaseWindow::WT_BGSPRITE, cBGSprite::POINTER_MINIGAME_COUNT));
	if(pkCount != null)
	{
		const float fCurrCountDown = UpdateCurrReadyCount();
		pkCount->SetfValue1(fCurrCountDown);
		if( fCurrCountDown <= 0 )//카운트 종료 할 때가 되었다
		{
			SetGaugeStartTime();
			SetStartGauge( true );
			GS2C_RECV_MAKE_DIGITAMA_MINIGAME_INIT kRecv;
			kRecv.nResult = true;
			kRecv.nSuccessCnt = 0;
			GAME_EVENT_ST.OnEvent( EVENT_CODE::RECV_MAKETACTICS_MINIGAME_INIT, &kRecv );
			m_bStartReadyCountingDown = false;
		}
	}
}
void	cMinigame::UpdateGauges(const float& fDeltaTime)
{
	//바 하나씩 채워 갑시다
	if( m_CurrGuageIndex <MAX_BAR_BUTTON_COUNT )
	{
		if( ( GetTickCount() - m_sBar[ m_CurrGuageIndex ].m_nStartTick ) >= m_sBar[ m_CurrGuageIndex ].m_nBarTime 
			&& m_sBar[ m_CurrGuageIndex ].m_nState == cButton::READY		// 바 상태가 대기 상태 일 경우에만( 1번만 )
			&& m_bServerRecv )		//서버 응답 대기중이 아님
			SetGaugeState( m_CurrGuageIndex, cButton::SUCCESS);

		if( ( GetTickCount() - m_sBar[ m_CurrGuageIndex ].m_nStartTick ) > m_sBar[ m_CurrGuageIndex ].m_nBarTime + 400 && m_bServerRecv )	// 클릭 대기시간 0.4초
		{
			m_bServerRecv = false;
			GetSystem()->Send_MiniGame_TimeOut();
		}
	}
	for(int i = 0 ; i < MAX_BAR_BUTTON_COUNT ; i++)
	{
		switch( m_sBar[ i ].m_nState )
		{
		case cButton::SUCCESS:
			m_sBar[ i ].m_pBtn->setTexTokenState( cButton::SUCCESS );
			break;
		case cButton::FAIL:
			m_sBar[ i ].m_pBtn->setTexTokenState( cButton::FAIL );
			break;
		case cButton::READY:
			m_sBar[ i ].m_pBtn->setTexTokenState( cButton::READY );
			break;
		}
	}	
	if(m_nSuccessCnt != 0)
	{
		cButton::eSTATE kState = cButton::SUCCESS;
		switch(m_nSuccessCnt)//cnt = 0 : -4%
		{
		case 1: kState = cButton::SUCCESS; break;//-3%
		case 2: kState = cButton::FAIL; break;//-2%
		case 3: kState = cButton::THREE; break;//-1%
		case 4: kState = cButton::FOUR; break;//1%
		case 5: kState = cButton::FIVE; break;//2%
		case 6: kState = cButton::SIX; break;//3%
		case 7: kState = cButton::SEVEN; break;//4%
		default: kState = cButton::SUCCESS; break;
		}
		m_pLetters->setTexTokenState(kState);
		m_pUp->setTexTokenState(kState);
	}

}

void	cMinigame::Render()
{
	RenderScript();
	RenderGauge();
	EndRenderScript();
}


void	cMinigame::RenderGauge(void)
{
	if(m_bStartGaugeUpdate)
	{
		//바 위치 설정
		for(int i = 0 ; i < MAX_BAR_BUTTON_COUNT; i++)
		{
			CsPoint ptPos;
			if( i < GetCurrGaugeIdx() )
				ptPos = CaclGaugeChargingRate( m_sBar[i].m_nStartTick , m_sBar[i].m_nBarTime, CsPoint( 31, 151 ) );
			else if( i == GetCurrGaugeIdx() )
				ptPos = CaclGaugeChargingRate( GetTickCount() - m_sBar[i].m_nStartTick , m_sBar[i].m_nBarTime, CsPoint( 31, 151 ) );

			m_sBar[ i ].m_pGaugeStencil->Render( CsPoint( 32 + (i*31), 73 + 152 - ptPos.y ), ptPos ) ;
			m_sBar[ i ].m_pGaugeBar->Render();
		}
	}
}

CsPoint cMinigame::CaclGaugeChargingRate( uint nCurGauge, uint nMaxGauge, CsPoint Size )
{
	float dwGaugeRate;
	CsPoint	Pos = CsPoint::ZERO;
	if( nCurGauge > nMaxGauge )//nMaxGauge(ex:3500) 이면 완료 상태
		dwGaugeRate = 100.0f;
	else//아니라면 3500에 가까워질 수록 채워짐
		dwGaugeRate =  (float)nCurGauge / (float)nMaxGauge;
	Size.y = (long)( (float)(Size.y) * dwGaugeRate );

	return Size;
}

void	cMinigame::SetGaugeChagingTime( u2 nBarIdx, u2 nBarTime )
{
	if( nBarIdx >= MAX_BAR_BUTTON_COUNT )
		return;

	m_sBar[ nBarIdx ].m_nBarTime = nBarTime;
}

void	cMinigame::SetGaugeState( u2 nBarIdx, cButton::eSTATE eType )
{
	if( nBarIdx >= MAX_BAR_BUTTON_COUNT )
		return;

	m_sBar[ nBarIdx ].m_nState = eType;	// 바 상태 실패
	m_sBar[ nBarIdx ].m_pBtn->setTexTokenState( eType );
}



#endif

