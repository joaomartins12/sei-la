

#include "stdafx.h"

#include "LevelUpMsg.h"

void cLevelUpMsg::sINFO::Init( TCHAR const* szText, TCHAR const* szText2/* = NULL*/, TCHAR const* szText3/* = NULL*/, 
							  TCHAR const* szText4/* = NULL*/, TCHAR const* szText5/*= NULL*/, TCHAR const* szText6/* = NULL*/ )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, NiColor(0/255.0f, 0/255.0f, 255/255.0f) );
	ti.SetText( szText );
	assert_cs( s_pText == NULL );
	s_pText = NiNew cText;
	s_pText->Init( NULL, CsPoint::ZERO, &ti, false );

	if( szText2 != NULL )
	{
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16 );
		ti.SetText( szText2 );
		assert_cs( s_pText2 == NULL );
		s_pText2 = NiNew cText;
		s_pText2->Init( NULL, CsPoint::ZERO, &ti, false );
	}

	if( szText3 != NULL )
	{
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, NiColor(0/255.0f, 0/255.0f, 255/255.0f) );
		ti.SetText( szText3 );
		assert_cs( s_pText3 == NULL );
		s_pText3 = NiNew cText;
		s_pText3->Init( NULL, CsPoint::ZERO, &ti, false );
	}

	if( szText4 != NULL )
	{
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16 );
		ti.SetText( szText4 );
		assert_cs( s_pText4 == NULL );
		s_pText4 = NiNew cText;
		s_pText4->Init( NULL, CsPoint::ZERO, &ti, false );
	}

	if( szText5 != NULL )
	{
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, NiColor(0/255.0f, 0/255.0f, 255/255.0f) );
		ti.SetText( szText5 );
		assert_cs( s_pText5 == NULL );
		s_pText5 = NiNew cText;
		s_pText5->Init( NULL, CsPoint::ZERO, &ti, false );
	}

	if( szText6 != NULL )
	{
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16 );
		ti.SetText( szText6 );
		assert_cs( s_pText6 == NULL );
		s_pText6 = NiNew cText;
		s_pText6->Init( NULL, CsPoint::ZERO, &ti, false );
	}

	s_eState = cGlobal::ALPHA_INCREASE;
	s_fAlpha = 0.0f;
}

void cLevelUpMsg::sINFO::Delete()
{
	SAFE_NIDELETE( s_pText );
	SAFE_NIDELETE( s_pText2 );
	SAFE_NIDELETE( s_pText3 );
	SAFE_NIDELETE( s_pText4 );
	SAFE_NIDELETE( s_pText5 );
	SAFE_NIDELETE( s_pText6 );
	s_fAlpha = 0.0f;
}

cGlobal::eState cLevelUpMsg::sINFO::Update(float const& fDeltaTime)
{
	// 하나의 텍스트는 반드시 존재해야 함. 그 이외에는 메세지에 따라서...
	assert_cs( s_pText != NULL );

	switch( s_eState )
	{
	case cGlobal::ALPHA_INCREASE:
		if( ( s_fAlpha += fDeltaTime / 1.0f ) > 1.0f )
		{
			s_fAlpha = 1.0f;
			s_fTime = 4.0f;
			s_eState = cGlobal::LIVE;
		}
		break;
	case cGlobal::LIVE:
		if( ( s_fTime -= fDeltaTime ) < 0.0f )
		{
			s_eState = cGlobal::ALPHA_DECREASE;
		}
		break;
	case cGlobal::ALPHA_DECREASE:
		if( ( s_fAlpha -= fDeltaTime / 0.5f ) < 0.0f )
		{			
			Delete();
			s_eState = cGlobal::DIE;
		}
		break;
	}
	return s_eState;
}

void cLevelUpMsg::sINFO::Render()
{
	// 하나의 텍스트는 반드시 존재해야 함. 그 이외에는 메세지에 따라서...
	assert_cs( s_pText != NULL );

	CsPoint tempPos = s_pText != NULL ? s_pText->GetStringSize() : CsPoint(0, 0);
	CsPoint tempPos2 = s_pText2 != NULL ? s_pText2->GetStringSize() : CsPoint(0, 0);
	CsPoint tempPos3 = s_pText3 != NULL ? s_pText3->GetStringSize() : CsPoint(0, 0);
	CsPoint tempPos4 = s_pText4 != NULL ? s_pText4->GetStringSize() : CsPoint(0, 0);
	CsPoint tempPos5 = s_pText5 != NULL ? s_pText5->GetStringSize() : CsPoint(0, 0);
	CsPoint tempPos6 = s_pText6 != NULL ? s_pText6->GetStringSize() : CsPoint(0, 0);

	if( s_pText != NULL )
	{
		s_pText->SetAlpha( s_fAlpha );
		s_pText->Render( CsPoint( g_nScreenWidth/2 - ( ( tempPos.x + tempPos2.x + tempPos3.x + tempPos4.x) / 2 ), g_nScreenHeight/5 + 25) );
	}

	if( s_pText2 != NULL )
	{
 		s_pText2->SetAlpha( s_fAlpha );
		s_pText2->Render( CsPoint( g_nScreenWidth/2 - ( ( tempPos.x + tempPos2.x + tempPos3.x + tempPos4.x ) / 2 ) + tempPos.x, g_nScreenHeight/5 + 25 ) );
	}

	if( s_pText3 != NULL )
	{
		s_pText3->SetAlpha( s_fAlpha );
		s_pText3->Render( CsPoint( g_nScreenWidth/2 - ( ( tempPos.x + tempPos2.x + tempPos3.x + tempPos4.x ) / 2 ) + tempPos.x + tempPos2.x, g_nScreenHeight/5 + 25 ) );
	}

	if( s_pText4 != NULL )
	{
		s_pText4->SetAlpha( s_fAlpha );
		s_pText4->Render( CsPoint( g_nScreenWidth/2 - ( ( tempPos.x + tempPos2.x + tempPos3.x + tempPos4.x ) / 2 ) + tempPos.x + tempPos2.x + tempPos3.x, g_nScreenHeight/5 + 25 ) );
	}

	if( s_pText5 != NULL )
	{
		s_pText5->SetAlpha( s_fAlpha );
		s_pText5->Render( CsPoint( g_nScreenWidth/2 - ( ( tempPos5.x + tempPos6.x ) / 2 ), g_nScreenHeight/5 + 60 ) );
	}

	if( s_pText6 != NULL )
	{
		s_pText6->SetAlpha( s_fAlpha );
		s_pText6->Render( CsPoint( g_nScreenWidth/2 - ( ( tempPos5.x + tempPos6.x ) / 2 ) + tempPos5.x, g_nScreenHeight/5 + 60 ) );
	}
}

//=============================================================================
//
//=============================================================================
cLevelUpMsg::cLevelUpMsg()
{
	m_bMsgDie = false;
	m_fAccumTime = 0.0f;
	m_fAlpha = 0.0f;
	m_eState = ALPHA_INCREASE;

	m_pMsgAni = NiNew cSpriteAni;
	m_pMsgAni->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( 1000, 370 ), "Exp_notice\\Exp_Notice_000", "tga", 20, false );
	m_pMsgAni->SetAniTime( 0.05f );
	m_pMsgAni->SetAlpha( 0.0f );
}

cLevelUpMsg::~cLevelUpMsg()
{
	Delete();
}

void cLevelUpMsg::Delete()
{
	SAFE_NIDELETE( m_pMsgAni );

	std::list< sINFO* >::iterator it = m_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = m_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		NiDelete *it;
	}
	m_listInfo.clear();
}

cGlobal::eState cLevelUpMsg::Update(float const& fDeltaTime)
{
	if( m_bMsgDie )
	{
		BackGroundAniUpdate(fDeltaTime);
		m_pMsgAni->SetAlpha(m_fAlpha);
		m_pMsgAni->Update(fDeltaTime);
	}

	std::list< sINFO* >::iterator it = m_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = m_listInfo.end();

	while( it!=itEnd )
	{
		switch( (*it)->Update(fDeltaTime) )
		{
		case ALPHA_INCREASE:
		case LIVE:
			return m_eState;

		case ALPHA_DECREASE:
			++it;
			break;
		case DIE:
			NiDelete *it;
			it = m_listInfo.erase( it );
			break;
		default:
			assert_cs( false );
		}
	}

	return m_eState;
}

bool cLevelUpMsg::Render()
{
	if( m_bMsgDie == true )
	{
		int nX = ( g_pResist->m_Global.s_nResolutionWidth / 2 ) - 500;
		m_pMsgAni->Render( CsPoint( nX, g_nScreenHeight/5 - 140) );
	}

	if( m_listInfo.empty() )
		return false;

	std::list< sINFO* >::iterator it = m_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = m_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Render();
		switch( (*it)->s_eState )
		{
		case ALPHA_INCREASE:
		case LIVE:
			return true;

		case ALPHA_DECREASE:
			break;
		default:
			assert_cs( false );
		}
	}
	return true;
}

void cLevelUpMsg::SetMessage( TCHAR const* szText, TCHAR const* szText2, TCHAR const* szText3 )
{
	if( szText[ 0 ] == NULL )
		return;

	if( !m_listInfo.empty() )
		m_fAccumTime += 5.0f;
	else
		m_fAccumTime = 5.0f;

	std::wstring wsText1 = UISTRING_TEXT( "LEVELUP_MESSAGE_WHO" );	// "님이 "
	std::wstring wsText2 = UISTRING_TEXT( "LEVELUP_MESSAGE_USE" );	// "을(를) 사용하여"
	std::wstring wsText3 = UISTRING_TEXT( "LEVELUP_MESSAGE_LEVEL" );// "레벨이 상승하였습니다."

	sINFO* pInfo = NiNew sINFO;
	pInfo->Init( szText, wsText1.c_str(), szText2, wsText2.c_str(), szText3, wsText3.c_str() );
	m_listInfo.push_back( pInfo );

	m_bMsgDie = true;
	m_eState = ALPHA_INCREASE;
}

void cLevelUpMsg::BackGroundAniUpdate(float const& fDeltaTime)
{
	switch( m_eState )
	{
	case ALPHA_INCREASE:
		if( ( m_fAlpha += fDeltaTime / 1.0f ) > 1.0f )
		{
			m_fAlpha = 1.0f;
			m_eState = LIVE;
		}
		break;
	case LIVE:
		if( ( m_fAccumTime -= fDeltaTime ) < 1.0f )
		{
			m_eState = ALPHA_DECREASE;
		}
		break;
	case ALPHA_DECREASE:
		if( ( m_fAlpha -= fDeltaTime / 1.0f ) < 0.0f )
		{			
			m_fAlpha = m_fAccumTime = 0.0f;
			m_eState = DIE;

		}
		break;
	case DIE:
		m_bMsgDie = false;
		break;
	}
}

