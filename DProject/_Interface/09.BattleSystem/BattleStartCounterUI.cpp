
#include "stdafx.h"
#include "../../ContentsSystem/ContentsSystemDef.h"
#include "../../ContentsSystem/ContentsSystem.h"
#include "BattleStartCounterUI.h"

cBattleStartCounter::cBattleStartCounter():m_pTimer(NULL),m_pStartImg(NULL),m_state(NONE),m_fTime(0.0f)
{

}

cBattleStartCounter::~cBattleStartCounter()
{
	if( GetSystem() )
		GetSystem()->UnRegisterAll( this );
}

cBaseWindow::eWINDOW_TYPE	cBattleStartCounter::GetWindowType()
{ 
	return cBaseWindow::WT_BATTLESTART_TIMER; 
}

void cBattleStartCounter::Destroy()
{	
	cBaseWindow::Delete();		
}

void cBattleStartCounter::DeleteResource()
{	
	DeleteScript();	
	NISAFE_DELETE( m_pTimer );
	NISAFE_DELETE( m_pStartImg );
}

void cBattleStartCounter::Create(int nValue /* = 0  */)
{
	cBaseWindow::Init();
	InitScript( NULL, CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 4 ), CsPoint::ZERO, false, IFREGION_X::CENTER, IFREGION_Y::TOP, false );

	m_pTimer = NiNew cSprite;								
	m_pTimer->Init( NULL, CsPoint::ZERO, CsPoint( 83, 102 ), "Battle\\Timer\\num_count.tga", false );
	//m_pTimer->SetColor( NiColor( 1, 1, 0 ) );

	m_pStartImg = NiNew cSprite;			
	CsPoint pos = CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 7 ) - CsPoint( 502/2, 0 );					
	m_pStartImg->Init( NULL, pos, CsPoint( 502, 97 ), "Battle\\Timer\\batte_start.tga", false );
	m_pStartImg->SetVisible(false);
}

cBaseWindow::eMU_TYPE cBattleStartCounter::Update_ForMouse()
{
	return MUT_OUT_WINDOW;
}

void cBattleStartCounter::Update(float const& fDeltaTime)
{
	if( m_fTime == 0.0f )
		return;

	m_fTime -= fDeltaTime;
	if( m_fTime <= 0.0f )
	{
		m_fTime = 0.0f;
		switch(m_state)
		{
		case READY:// 시작 타이머가 끝났을 때
			{
				m_state = START;
				m_pStartImg->SetVisible(true);
				m_fTime = 3.0f;
				GetSystem()->OnBattleStart();
			}
			break;
		case START:// 시작 알림 이미지 타이머가 끝났을 때
			m_pStartImg->SetVisible(false);
			m_state = NONE;
			Close(false);
			break;
		}
	}
}

void cBattleStartCounter::Render()
{
	RenderScript();	

	switch(m_state)
	{
	case READY:
		{
			SAFE_POINTER_RET( m_pTimer );

			int nTick = (int)m_fTime;
			int nSize = 90;
			int nRes_10 = nTick / 10;
			int nRes_1 = nTick % 10;
			CsPoint InitPos = CsPoint( g_nScreenWidth / 2, g_nScreenHeight / 4 ) - CsPoint( nSize/2, nSize/2 );
			CsPoint pos = InitPos - CsPoint( 2 + nSize/2, 0 );
			if( nRes_10 > 4 )			
				m_pTimer->Render( pos, CsPoint( nSize, nSize ), NiPoint2( nRes_10/5.0f, 0.5f ), NiPoint2( ( nRes_10 + 1 )/5.0f, 1.0f ) );
			else
				m_pTimer->Render( pos, CsPoint( nSize, nSize ), NiPoint2( nRes_10/5.0f, 0 ), NiPoint2( ( nRes_10 + 1 )/5.0f, 0.5f ) );

			pos = InitPos + CsPoint( 2 + nSize/2, 0 );
			if( nRes_1 > 4 )			
				m_pTimer->Render( pos, CsPoint( nSize, nSize ), NiPoint2( nRes_1/5.0f, 0.5f ), NiPoint2( ( nRes_1 + 1 )/5.0f, 1.0f ) );
			else
				m_pTimer->Render( pos, CsPoint( nSize, nSize ), NiPoint2( nRes_1/5.0f, 0 ), NiPoint2( ( nRes_1 + 1 )/5.0f, 0.5f ) );	
		}
		break;
	case START:
		if( m_pStartImg )
			m_pStartImg->Render();
		break;
	}
}

void cBattleStartCounter::ResetDevice()
{
	ResetDeviceScript();
	if( m_pTimer )
		m_pTimer->ResetDevice();
	if( m_pStartImg )
		m_pStartImg->ResetDevice();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool cBattleStartCounter::Construct(void)
{
	if( !CONTENTSSYSTEM_PTR )
		return false;

	SetSystem( CONTENTSSYSTEM_PTR->GetContents< SystemType >( SystemType::IsContentsIdentity() ) );
	if( !GetSystem() )
		return false;

	GetSystem()->Register( SystemType::eBattleReadyTimerStart, this );
	
	return true;
}

void cBattleStartCounter::Notify(int const& iNotifiedEvt, ContentsStream const& kStream)
{
	switch( iNotifiedEvt )
	{
	case SystemType::eBattleReadyTimerStart:
		{
			kStream >> m_fTime;
			m_state = READY;
		}
		break;
	}
}

