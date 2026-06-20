

#include "stdafx.h"

#include "Random_Regen.h"

void cRandomRegen::sINFO::Init( TCHAR const* szFirst, TCHAR const* szMonName, TCHAR const* szSecond )
{
 	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16 );
	ti.SetText( szFirst );
	assert_cs( s_pText == NULL );
	s_pText = NiNew cText;
	s_pText->Init( NULL, CsPoint::ZERO, &ti, false );

	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, FONT_NEWGOLD );
	ti.SetText( szMonName );	// 몬스터 이름
	assert_cs( s_pMonName == NULL );
	s_pMonName = NiNew cText;
	s_pMonName->Init( NULL, CsPoint::ZERO, &ti, false );
	
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16 );
	ti.SetText( szSecond );
	assert_cs( s_pText2 == NULL );
	s_pText2 = NiNew cText;
	s_pText2->Init( NULL, CsPoint::ZERO, &ti, false );

	s_eState = cGlobal::ALPHA_INCREASE;
	s_fAlpha = 0.0f;
}

void cRandomRegen::sINFO::Delete()
{
	SAFE_NIDELETE( s_pText );
	SAFE_NIDELETE( s_pMonName );
	SAFE_NIDELETE( s_pText2 );
	s_fAlpha = 0.0f;
}

cGlobal::eState cRandomRegen::sINFO::Update(float const& fDeltaTime)
{
	assert_cs( s_pText != NULL );
	assert_cs( s_pMonName != NULL );
	assert_cs( s_pText2 != NULL );

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

void cRandomRegen::sINFO::Render()
{
	assert_cs( s_pText != NULL );
	assert_cs( s_pMonName != NULL );
	assert_cs( s_pText2 != NULL );

	CsPoint tempPos = s_pText->GetStringSize();
	CsPoint tempPos2 = s_pMonName->GetStringSize();
	CsPoint tempPos3 = s_pText2->GetStringSize();

	s_pText->SetAlpha( s_fAlpha );
	s_pText->Render( CsPoint( g_nScreenWidth/2 - ((tempPos.x + tempPos2.x + tempPos3.x ) / 2), g_nScreenHeight/5 + 37 ) );

 	s_pMonName->SetAlpha( s_fAlpha );
	s_pMonName->Render( CsPoint( g_nScreenWidth/2 - ((tempPos.x + tempPos2.x + tempPos3.x ) / 2) + tempPos.x, g_nScreenHeight/5 + 37 ) );

	s_pText2->SetAlpha( s_fAlpha );
	s_pText2->Render( CsPoint( g_nScreenWidth/2 - ((tempPos.x + tempPos2.x + tempPos3.x ) / 2) + tempPos.x + tempPos2.x, g_nScreenHeight/5 + 37 ) );
}

//=============================================================================
//
//=============================================================================
cRandomRegen::cRandomRegen()
{
	m_bMsgDie = false;
	m_fAccumTime = 0.0f;
	m_fAlpha = 0.0f;
	m_eState = ALPHA_INCREASE;

	m_pMsgAni = NiNew cSpriteAni;
	m_pMsgAni->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( 840, 370 ), "Random_Regen\\Random_Regen_", "tga", 20, false );
	m_pMsgAni->SetAniTime( 0.05f );
	m_pMsgAni->SetAlpha( 0.0f );
}

cRandomRegen::~cRandomRegen()
{
	Delete();
}

void cRandomRegen::Delete()
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

cGlobal::eState cRandomRegen::Update(float const& fDeltaTime)
{
	if( m_bMsgDie )
	{
		RandomRegendMsgBGUpdate();
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
			//return;
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

bool cRandomRegen::Render()
{
	if( m_bMsgDie == true )
	{
		int nX = (g_nScreenWidth-740)/2;
		//m_pMsgAni->Render( CsPoint( nX - 50, g_nScreenHeight/5 - 165) );
		m_pMsgAni->Render( CsPoint( nX - 50, g_nScreenHeight/5 - 140) );
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

		case ALPHA_DECREASE:	break;
		default:				assert_cs( false );
		}
	}
	return true;
}

void cRandomRegen::SetRandomRegenStart( TCHAR const* szMonsterName )
{
	if( szMonsterName[ 0 ] == NULL )
		return;

	std::wstring wsFirst = UISTRING_TEXT( "RANDOMREGEN_MESSAGE_APPEAR_FIRST" );
	std::wstring wsSecond = UISTRING_TEXT( "RANDOMREGEN_MESSAGE_APPEAR_SECOND" );
	std::wstring wsMonster;
	DmCS::StringFn::Format( wsMonster, L"\'%s\'", szMonsterName );

	if( !m_listInfo.empty() )
		m_fAccumTime += 5.0f;
	else
		m_fAccumTime = 5.0f;

	sINFO* pInfo = NiNew sINFO;
	pInfo->Init( wsFirst.c_str(), wsMonster.c_str(), wsSecond.c_str() );
	m_listInfo.push_back( pInfo );

	m_bMsgDie = true;
	m_eState = ALPHA_INCREASE;
}

void cRandomRegen::SetRandomRegenEnd( TCHAR const* szMonsterName )
{
	if( szMonsterName[ 0 ] == NULL )
		return;

	std::wstring wsFirst = UISTRING_TEXT( "RANDOMREGEN_MESSAGE_DISAPPEAR_FIRST" );
	std::wstring wsSecond = UISTRING_TEXT( "RANDOMREGEN_MESSAGE_DISAPPEAR_SECOND" );
	std::wstring wsMonster;
	DmCS::StringFn::Format( wsMonster, L"\'%s\'", szMonsterName );

	if( !m_listInfo.empty() )
		m_fAccumTime += 5.0f;
	else
		m_fAccumTime = 5.0f;

	sINFO* pInfo = NiNew sINFO;
	pInfo->Init( wsFirst.c_str(), wsMonster.c_str(), wsSecond.c_str() );
	m_listInfo.push_back( pInfo );

	m_bMsgDie = true;
	m_eState = ALPHA_INCREASE;
}

void cRandomRegen::RandomRegendMsgBGUpdate()
{
	switch( m_eState )
	{
	case ALPHA_INCREASE:
		if( ( m_fAlpha += g_fDeltaTime / 1.0f ) > 1.0f )
		{
			m_fAlpha = 1.0f;
			m_eState = LIVE;
		}
		break;
	case LIVE:
		if( ( m_fAccumTime -= g_fDeltaTime ) < 1.0f )
		{
			m_eState = ALPHA_DECREASE;
		}
		break;
	case ALPHA_DECREASE:
		if( ( m_fAlpha -= g_fDeltaTime / 1.0f ) < 0.0f )
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

