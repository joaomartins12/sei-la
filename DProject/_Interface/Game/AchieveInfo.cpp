
#include "stdafx.h"
#include "AchieveInfo.h"


void cAchieveInfo::sINFO::Init( TCHAR const* szText )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_24 );
	ti.SetText( szText );
	assert_cs( s_pText == NULL );
	s_pText = NiNew cText;
	s_pText->Init( NULL, CsPoint::ZERO, &ti, false );

	assert_cs( s_pTextLayer == NULL );
	s_pTextLayer = NiNew cSprite;
	s_pTextLayer->Init( NULL, CsPoint( 25, 0 ), CsPoint( 700, 94 ), "Region\\Achieve.tga", false );
	s_eState = ALPHA_INCREASE;
	s_fAlpha = 0.0f;
}

void cAchieveInfo::sINFO::Delete()
{
	SAFE_NIDELETE( s_pText );
	SAFE_NIDELETE( s_pTextLayer );

	s_fAlpha = 0.0f;
}

cAchieveInfo::eState cAchieveInfo::sINFO::Update(float const& fDeltaTime)
{
	assert_cs( s_pText != NULL );
	assert_cs( s_pTextLayer != NULL );

	switch( s_eState )
	{
	case ALPHA_INCREASE:
		if( ( s_fAlpha += fDeltaTime / 1.0f ) > 1.0f )
		{
			s_fAlpha = 1.0f;
			s_fTime = 3.0f;
			s_eState = LIVE;
			g_pCharMng->GetTamerUser()->GetProp_Effect()->AddEffect_FT( "system\\achievement0001.nif" );
		}
		break;
	case LIVE:
		if( ( s_fTime -= fDeltaTime ) < 0.0f )
		{
			s_eState = ALPHA_DECREASE;
		}
		break;
	case ALPHA_DECREASE:
		if( ( s_fAlpha -= fDeltaTime / 0.5f ) < 0.0f )
		{			
			Delete();
			s_eState = DIE;
		}
		break;
	}
	return s_eState;
}

void cAchieveInfo::sINFO::Render()
{
	assert_cs( s_pText != NULL );
	assert_cs( s_pTextLayer != NULL );

	int nX = (g_nScreenWidth-700)/2;
	s_pTextLayer->Render( CsPoint( nX - 25, g_nScreenHeight/4 - 24 ), s_fAlpha );
	s_pText->SetAlpha( s_fAlpha );
	s_pText->Render( CsPoint( g_nScreenWidth/2, g_nScreenHeight/4+10 ), DT_CENTER );
}




cAchieveInfo*		g_pAchieveInfo = NULL;

void cAchieveInfo::GlobalInit()
{
	assert_cs( g_pAchieveInfo == NULL );
	g_pAchieveInfo = NiNew cAchieveInfo;	
}

void cAchieveInfo::GlobalShotDown()
{
	SAFE_NIDELETE( g_pAchieveInfo );
}

cAchieveInfo::cAchieveInfo()
{
}

cAchieveInfo::~cAchieveInfo()
{
	Delete();
}

void cAchieveInfo::Delete()
{
	std::list< sINFO* >::iterator it = m_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = m_listInfo.end();
	for( ; it!=itEnd; ++it )
	{
		NiDelete *it;
	}
	m_listInfo.clear();
}

void cAchieveInfo::SetAchieveInfo( TCHAR const* szText )
{
	if( szText[ 0 ] == NULL )
		return;

	sINFO* pInfo = NiNew sINFO;
	pInfo->Init( szText );
	m_listInfo.push_back( pInfo );
}
 
void cAchieveInfo::Update(float const& fDeltaTime)
{
	std::list< sINFO* >::iterator it = m_listInfo.begin();
	std::list< sINFO* >::iterator itEnd = m_listInfo.end();

	while( it!=itEnd )
	{
		switch( (*it)->Update(fDeltaTime) )
		{
		case ALPHA_INCREASE:
		case LIVE:
			return;

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
}

bool cAchieveInfo::Render()
{
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