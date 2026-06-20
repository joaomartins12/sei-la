#include "stdafx.h"
#include "Fade.h"

#include "../../Flow/Flow.h"
#include "../../Flow/FlowMgr.h"

//CFade* g_pFade = NULL;

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

CFade::CFade():m_fTime(0),m_fMaxTime(0),m_nType(FADE_NONE),m_nState(FADE_STATE_END),m_fDeltaTime(0),m_dwOldTime(0)
{}

CFade::~CFade()
{}

void CFade::Init(eFadeType type, float time)
{
	m_dwOldTime = GetTickCount();
	m_fMaxTime = m_fTime = time;
	m_nType = type;

	m_nState = FADE_STATE_DOING;
	m_Sprite.Init( NULL, CsPoint::ZERO, CsPoint( g_nScreenWidth, g_nScreenHeight ), NiColorA::BLACK, false );

	FLOWMGR_ST.LockFlow();
}
//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CFade::Update(float const& fDeltaTime)
{
	if( m_nState == FADE_STATE_END )
		return;

	assert_cs( m_fTime >= 0.0f );
	m_fDeltaTime = ( GetTickCount() - m_dwOldTime )*0.001f;
	m_dwOldTime = GetTickCount();

	m_fTime = CsMax( 0.0f, m_fTime - m_fDeltaTime );
}

void CFade::Reset( eFadeType type, float time )
{
	m_dwOldTime = GetTickCount();
	m_fMaxTime = m_fTime = time;
	m_nType = type;
	m_nState = FADE_STATE_DOING;
	m_Sprite.SetColor( NiColor::BLACK );

	FLOWMGR_ST.LockFlow();
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CFade::Render()
{
	if( m_nState == FADE_STATE_END )
		return;

	float Alpha = m_fTime * (1.0f / m_fMaxTime);

	if( m_nType == FADE_OUT )
		Alpha = 1.0f - Alpha;

	switch( m_nType )
	{
	case FADE_OUT:
		if( Alpha >= 1.0f ) 
		{
			Alpha = 1.0f;
			m_nState = FADE_STATE_END;
			FLOWMGR_ST.UnLockFlow();
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::FADE_OUT_END );
		}break;
	case FADE_IN:
		if( Alpha <= 0.0f ) 
		{
			Alpha = 0.0f;
			m_nState = FADE_STATE_END;
			FLOWMGR_ST.UnLockFlow();
			GAME_EVENT_STPTR->OnEvent( EVENT_CODE::FADE_IN_END );
		}break;
	}

	m_Sprite.SetAlpha( Alpha );
	m_Sprite.Render();
}