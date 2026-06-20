
#include "stdafx.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"
#include "BattleRoundTimer.h"

cBattleRoundTimer::cBattleRoundTimer():m_fTime(0.0f),m_bHurryup(false)
{
	for( int n = 0; n < 3; ++n )
		m_pDot[n] = NULL;

	for( int n = 0; n < 2; ++n )
	{
		m_pMin[n] = NULL;
		m_pSec[n] = NULL;
		m_pMs[n] = NULL;
	}
}

cBattleRoundTimer::~cBattleRoundTimer()
{
	for( int n = 0; n < 3; ++n )
		m_pDot[n] = NULL;

	for( int n = 0; n < 2; ++n )
	{
		m_pMin[n] = NULL;
		m_pSec[n] = NULL;
		m_pMs[n] = NULL;
	}

	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE	cBattleRoundTimer::GetWindowType()
{ 
	return cBaseWindow::WT_BATTLEROUND_TIMER; 
}

void cBattleRoundTimer::Destroy()
{	
	cBaseWindow::Delete();		
}

void cBattleRoundTimer::DeleteResource()
{	
	DeleteScript();	
}

void cBattleRoundTimer::Create(int nValue /* = 0  */)
{
	cBaseWindow::Init();
	InitScript( NULL, CsPoint( 780, 30 ), CsPoint(253, 51), false, IFREGION_X::RIGHT, IFREGION_Y::TOP, true );

	m_pTimerBg = AddSprite( CsPoint::ZERO, CsPoint(253, 51), "Battle\\Timer\\battle_timer_bg_blue.tga" );

	m_pDot[0] = AddSprite( CsPoint( 99, 5 ), CsPoint( 23, 23 ), "Battle\\Timer\\batte_timer_dot_blue.tga" );
	m_pDot[1] = AddSprite( CsPoint( 99, 23 ), CsPoint( 23, 23 ), "Battle\\Timer\\batte_timer_dot_blue.tga" );
	m_pDot[2] = AddSprite( CsPoint( 172, 33 ), CsPoint( 23, 23 ), "Battle\\Timer\\batte_timer_dot_blue.tga" );

	{
		m_pMin[0] = AddSprite( CsPoint( 41, 0 ), CsPoint( 34, 51 ), "Battle\\Timer\\batte_timer_blue.tga" );
		m_pMin[1] = AddSprite( CsPoint( 69, 0 ), CsPoint( 34, 51 ), "Battle\\Timer\\batte_timer_blue.tga" );
	}

	{
		m_pSec[0] = AddSprite( CsPoint( 117, 0 ), CsPoint( 34, 51 ), "Battle\\Timer\\batte_timer_blue.tga" );
		m_pSec[1] = AddSprite( CsPoint( 147, 0 ), CsPoint( 34, 51 ), "Battle\\Timer\\batte_timer_blue.tga" );
	}

	{
		m_pMs[0] = AddSprite( CsPoint( 186, 6 ), CsPoint( 31, 45 ), "Battle\\Timer\\batte_timer_blue.tga" );
		m_pMs[1] = AddSprite( CsPoint( 208, 6 ), CsPoint( 31, 45 ), "Battle\\Timer\\batte_timer_blue.tga" );
	}

	SetVisible( false );
}

void cBattleRoundTimer::SetHurryup()
{
	SetRegionType( IFREGION_X::JUST_CENTER, IFREGION_Y::TOP );
	CsPoint pos = CsPoint( 512 - (253 /2), DEFAULT_SCREEN_HEIGHT / 6 );
	SetPos( pos );

	if( m_pTimerBg )
		m_pTimerBg->ChangeTexture( "Battle\\Timer\\battle_timer_bg_red.tga" );

	for( int n = 0; n < 3; ++n )
	{
		if( m_pDot[n] )
			m_pDot[n]->ChangeTexture( "Battle\\Timer\\batte_timer_dot_red.tga" );
	}

	for( int n = 0; n < 2; ++n )
	{
		if( m_pMin[n] )
			m_pMin[n]->ChangeTexture( "Battle\\Timer\\batte_timer_red.tga" );
		if( m_pSec[n] )
			m_pSec[n]->ChangeTexture( "Battle\\Timer\\batte_timer_red.tga" );
		if( m_pMs[n] )
			m_pMs[n]->ChangeTexture( "Battle\\Timer\\batte_timer_red.tga" );
	}

	ResetDevice();
}

void cBattleRoundTimer::Update(float const& fDeltaTime)
{
	if( m_fTime == 0.0f )
		return;

	m_fTime -= fDeltaTime;
	if( m_fTime < 0.0f )
		m_fTime = 0.0f;				
	else if( m_fTime < 16.0f )
	{
		if( !m_bHurryup )
		{
			SetHurryup();
			m_bHurryup = true;
		}
	}

	int nTick = (int)( m_fTime * 1000 );
	int nRes = nTick / 1000;				//	초로 환산

	int nRes_nMin = nRes / 60;				//	분
	int nRes_nMin_10 = nRes_nMin / 10;		//	분 ( 10 의 자리 )
	int nRes_nMin_1 = nRes_nMin % 10;		//	분 ( 1  의 자리 )

	int nRes_nSec = nRes % 60;				//	남은 초
	int nRes_nSec_10 = nRes_nSec / 10;		//	남은 초 ( 10 의 자리 )
	int nRes_nSec_1 = nRes_nSec % 10;		//	남은 초 ( 1 의 자리 )

	int nRes_LowSec = ( nTick % 1000 ) / 10;
	int nRes_LowSec_10 = nRes_LowSec / 10;
	int nRes_LowSec_1 = nRes_LowSec % 10;

	if( m_pMin[0] )
	{
		if( nRes_nMin_10 > 4 )
			m_pMin[0]->SetTextureUV( NiPoint2( nRes_nMin_10/5.0f, 0.5f ), NiPoint2( ( nRes_nMin_10 + 1 )/5.0f, 1.0f ) );
		else
			m_pMin[0]->SetTextureUV( NiPoint2( nRes_nMin_10/5.0f, 0 ), NiPoint2( ( nRes_nMin_10 + 1 )/5.0f, 0.5f ) );
	}

	if( m_pMin[1] )
	{
		if( nRes_nMin_1 > 4 )			
			m_pMin[1]->SetTextureUV( NiPoint2( nRes_nMin_1/5.0f, 0.5f ), NiPoint2( ( nRes_nMin_1 + 1 )/5.0f, 1.0f ) );
		else
			m_pMin[1]->SetTextureUV(NiPoint2( nRes_nMin_1/5.0f, 0 ), NiPoint2( ( nRes_nMin_1 + 1 )/5.0f, 0.5f ) );
	}

	if( m_pSec[0] )
	{
		if( nRes_nSec_10 > 4 )			
			m_pSec[0]->SetTextureUV( NiPoint2( nRes_nSec_10/5.0f, 0.5f ), NiPoint2( ( nRes_nSec_10 + 1 )/5.0f, 1.0f ) );
		else
			m_pSec[0]->SetTextureUV( NiPoint2( nRes_nSec_10/5.0f, 0 ), NiPoint2( ( nRes_nSec_10 + 1 )/5.0f, 0.5f ) );
	}


	if( m_pSec[1] )
	{
		if( nRes_nSec_1 > 4 )			
			m_pSec[1]->SetTextureUV( NiPoint2( nRes_nSec_1/5.0f, 0.5f ), NiPoint2( ( nRes_nSec_1 + 1 )/5.0f, 1.0f ) );
		else
			m_pSec[1]->SetTextureUV( NiPoint2( nRes_nSec_1/5.0f, 0 ), NiPoint2( ( nRes_nSec_1 + 1 )/5.0f, 0.5f ) );
	}


	if( m_pMs[0] )
	{
		if( nRes_LowSec_10 > 4 )			
			m_pMs[0]->SetTextureUV( NiPoint2( nRes_LowSec_10/5.0f, 0.5f ), NiPoint2( ( nRes_LowSec_10 + 1 )/5.0f, 1.0f ) );
		else
			m_pMs[0]->SetTextureUV( NiPoint2( nRes_LowSec_10/5.0f, 0 ), NiPoint2( ( nRes_LowSec_10 + 1 )/5.0f, 0.5f ) );
	}


	if( m_pMs[1] )
	{
		if( nRes_LowSec_1 > 4 )			
			m_pMs[1]->SetTextureUV( NiPoint2( nRes_LowSec_1/5.0f, 0.5f ), NiPoint2( ( nRes_LowSec_1 + 1 )/5.0f, 1.0f ) );
		else
			m_pMs[1]->SetTextureUV( NiPoint2( nRes_LowSec_1/5.0f, 0 ), NiPoint2( ( nRes_LowSec_1 + 1 )/5.0f, 0.5f ) );
	}

}

void cBattleRoundTimer::Render()
{
	RenderScript();
}

void cBattleRoundTimer::ResetDevice()
{
	ResetDeviceScript();
}

bool cBattleRoundTimer::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register(SystemType::eBattleRoundStart, this);

	return true;
}

void cBattleRoundTimer::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch(iNotifiedEvt)
	{
	case SystemType::eBattleRoundStart:
		kStream >> m_fTime;
		SetVisible( true );
		break;
	}
}

