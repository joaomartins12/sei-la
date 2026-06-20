

#include "stdafx.h"

#include "FileIslandBless.h"

void cFileBless::sINFO::Init( TCHAR const* szTamerName, TCHAR const* szDigimonName, TCHAR const* szMonsterName, TCHAR const* szItemBuff )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, FONT_NEWGOLD );
	ti.SetText( szTamerName );	// 테이머 네임
	assert_cs( s_pText == NULL );
	s_pText = NiNew cText;
	s_pText->Init( NULL, CsPoint::ZERO, &ti, false );

	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, NiColor(251/255.0f, 244/255.0f, 189/255.0f) );
	ti.SetText( szDigimonName );		// 디지몬 네임
	assert_cs( s_pText2 == NULL );
	s_pText2 = NiNew cText;
	s_pText2->Init( NULL, CsPoint::ZERO, &ti, false );

	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_16, NiColor(251/255.0f, 244/255.0f, 189/255.0f) );
	ti.SetText( szMonsterName );		// 레이드 몬스터 네임
	assert_cs( s_pText3 == NULL );
	s_pText3 = NiNew cText;
	s_pText3->Init( NULL, CsPoint::ZERO, &ti, false );

	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_20, FONT_NEWGOLD );
	ti.SetText( szItemBuff );		// 아이템(버프 이름)
	assert_cs( s_pText4 == NULL );
	s_pText4 = NiNew cText;
	s_pText4->Init( NULL, CsPoint::ZERO, &ti, false );


	s_eState = cGlobal::ALPHA_INCREASE;
	s_fAlpha = 0.0f;

	s_bRaidDie = true;
}

void cFileBless::sINFO::Delete()
{
	SAFE_NIDELETE( s_pText );
	SAFE_NIDELETE( s_pText2 );
	SAFE_NIDELETE( s_pText3 );
	SAFE_NIDELETE( s_pText4 );

	s_fAlpha = 0.0f;
}

cGlobal::eState cFileBless::sINFO::Update(float const& fDeltaTime)
{
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

void cFileBless::sINFO::Render()
{
	assert_cs( s_pText != NULL );
	assert_cs( s_pText2 != NULL );
	assert_cs( s_pText3 != NULL );
	assert_cs( s_pText4 != NULL );

	CsPoint tempPos = s_pText->GetStringSize();
	CsPoint tempPos2 = s_pText2->GetStringSize();

	s_pText->SetAlpha( s_fAlpha );
	s_pText->Render( CsPoint( g_nScreenWidth/2 - ((tempPos.x + tempPos2.x) / 2), g_nScreenHeight/5 ) );

	s_pText2->SetAlpha( s_fAlpha );
	s_pText2->Render( CsPoint( g_nScreenWidth/2 - ((tempPos.x + tempPos2.x) / 2) + tempPos.x, g_nScreenHeight/5 ) );

	s_pText3->SetAlpha( s_fAlpha );
	s_pText3->Render( CsPoint( g_nScreenWidth/2, g_nScreenHeight/5+24 ), DT_CENTER );

	s_pText4->SetAlpha( s_fAlpha );
	s_pText4->Render( CsPoint( g_nScreenWidth/2, g_nScreenHeight/5+48 ), DT_CENTER );
}

//=============================================================================
//
//=============================================================================
cFileBless::cFileBless()
{
	m_fAccumTime = 0.0f;
	m_fAlpha = 0.0f;
	m_eState = ALPHA_INCREASE;
	m_bRaidDie = false;

	m_pBuffAni = NiNew cSpriteAni;
	m_pBuffAni->Init( cSpriteAni::LOOP, NULL, CsPoint::ZERO, CsPoint( 800, 370 ), "System_Buff\\Buff_Effect_", "tga", 20, false );
	m_pBuffAni->SetAniTime( 0.05f );
	m_pBuffAni->SetAlpha( 0.0f );
}

cFileBless::~cFileBless()
{
	Delete();
}

void cFileBless::Delete()
{
	SAFE_NIDELETE(m_pBuffAni);

	std::list< sINFO* >::iterator it = m_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = m_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		NiDelete *it;
	}
	m_listInfo.clear();
}

void cFileBless::SetRaidInfo( TCHAR const* szMonsterName, TCHAR const* szTamerName, TCHAR const* szDigimonName, TCHAR const* szItemBuff )
{
	if( szMonsterName[ 0 ] == NULL || szTamerName[ 0 ] == NULL || szDigimonName[ 0 ] == NULL )
		return;

	std::wstring wsFirst = UISTRING_TEXT( "FILEISLANDBLESS_KILL_MESSAGE_FIRST" );
	DmCS::StringFn::Replace( wsFirst, L"#Digimon#", szDigimonName );

	std::wstring wsSecond = UISTRING_TEXT( "FILEISLANDBLESS_KILL_MESSAGE_SECOND" );
	DmCS::StringFn::Replace( wsSecond, L"#Monster#", szMonsterName );

	sINFO* pInfo = NiNew sINFO;
	pInfo->Init( szTamerName, wsFirst.c_str(), wsSecond.c_str(), szItemBuff );
	m_listInfo.push_back( pInfo );

	m_bRaidDie = true;
	m_eState = ALPHA_INCREASE;
}

void cFileBless::RaidMsgBGUpdate()
{
	switch( m_eState )
	{
	case ALPHA_INCREASE:
		if( ( m_fAlpha += g_fDeltaTime / 1.0f ) > 1.0f )
		{
			m_fAlpha = 1.0f;
			m_fAccumTime = 5.0f;
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
			m_fAlpha = 0.0f;
			m_eState = DIE;
		}
		break;
	case DIE:
		m_bRaidDie = false;
		break;
	}
}

cGlobal::eState cFileBless::Update(float const& fDeltaTime)
{
	if( m_bRaidDie == true)
	{
		RaidMsgBGUpdate();
		m_pBuffAni->SetAlpha(m_fAlpha);
		m_pBuffAni->Update(fDeltaTime);
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

bool cFileBless::Render()
{
	if( m_bRaidDie == true )
	{
		int nX = (g_nScreenWidth-700)/2;
		m_pBuffAni->Render( CsPoint( nX - 50, g_nScreenHeight/5 - 165) );
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

