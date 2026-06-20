
#include "stdafx.h"
#include "RegionInfo.h"


void cRegionInfo::sINFO::Init( TCHAR* szText, cRegionInfo::eTYPE type )
{
	s_eType = type;

	cText::sTEXTINFO ti;
#ifdef VERSION_USA
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_20 );
#else
	switch( type )
	{
	case cRegionInfo::MAIN_REGION:
	case cRegionInfo::SUB_RESION:
		ti.Init( &g_pEngine->m_FontSystem, CFont::FS_32 );
		break;
	default:
		assert_cs( false );
	}
#endif

	ti.SetText( szText );
	assert_cs( s_pText == NULL );
	s_pText = NiNew cText;
	s_pText->Init( NULL, CsPoint::ZERO, &ti, false );

	assert_cs( s_pTextLayer == NULL );
	s_pTextLayer = NiNew cSprite;
#ifdef VERSION_USA
	switch( type )
	{
	case cRegionInfo::MAIN_REGION:	s_pTextLayer->Init( NULL, CsPoint::ZERO, CsPoint( 1024, 55 ), "Region\\Region01.tga", false );		break;
	case cRegionInfo::SUB_RESION:	s_pTextLayer->Init( NULL, CsPoint::ZERO, CsPoint( 1024, 50 ), "Region\\Region02.tga", false );		break;
	default:			assert_cs( false );
	}
#else
	switch( type )
	{
	case cRegionInfo::MAIN_REGION:	s_pTextLayer->Init( NULL, CsPoint::ZERO, CsPoint( 1024, 70 ), "Region\\Region01.tga", false );		break;
	case cRegionInfo::SUB_RESION:	s_pTextLayer->Init( NULL, CsPoint::ZERO, CsPoint( 1024, 70 ), "Region\\Region02.tga", false );		break;
	default:			assert_cs( false );
	}
#endif	

	s_eState = ALPHA_INCREASE;
	s_fAlpha = 0.0f;
}

void cRegionInfo::sINFO::Delete()
{
	SAFE_NIDELETE( s_pText );
	SAFE_NIDELETE( s_pTextLayer );
	s_fAlpha = 0.0f;
}

bool cRegionInfo::sINFO::Update(float const& fDeltaTime)
{
	assert_cs( s_pText != NULL );
	assert_cs( s_pTextLayer != NULL );

	switch( s_eState )
	{
	case ALPHA_INCREASE:
		if( ( s_fAlpha += fDeltaTime / 1.0f ) > 1.0f )
		{
			s_fAlpha = 1.0f;
			
			s_fTime = 1.5f;
			s_eState = LIVE;
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
			return false;
		}
		break;
	}
	return true;
}

void cRegionInfo::sINFO::Render()
{
	assert_cs( s_pText != NULL );
	assert_cs( s_pTextLayer != NULL );

	int nX = 0;
	switch( s_eType )
	{
	case MAIN_REGION:
	case SUB_RESION:
		{
			nX = g_nScreenWidth-1024;
			if( nX > 0 )
				nX = nX / 2;

			s_pTextLayer->Render( CsPoint( nX, g_nScreenHeight/4 ), s_fAlpha );
			s_pText->SetAlpha( s_fAlpha );
			s_pText->Render( CsPoint( g_nScreenWidth/2, g_nScreenHeight/4+15 ), DT_CENTER );
		}
		break;
	default:
		assert_cs( false );
	}
	
}




cRegionInfo*		g_pRegionInfo = NULL;

void cRegionInfo::GlobalInit()
{
	assert_cs( g_pRegionInfo == NULL );
	g_pRegionInfo = NiNew cRegionInfo;	
}

void cRegionInfo::GlobalShotDown()
{
	SAFE_NIDELETE( g_pRegionInfo );
}

cRegionInfo::cRegionInfo()
{
	m_pCurInfo = m_pOldInfo = NULL;
}

cRegionInfo::~cRegionInfo()
{
	Delete();
}

void cRegionInfo::Delete()
{
	SAFE_NIDELETE( m_pCurInfo );
	SAFE_NIDELETE( m_pOldInfo );
}

void cRegionInfo::SetRegionInfo( TCHAR const* szText, cRegionInfo::eTYPE type )
{
	if( szText == NULL )
		return;

	if( _tcscmp( m_szText, szText ) == 0 )
		return;
	
	// 현재게 남아 있다면
	if( m_pCurInfo )
	{
		// 예전것은 제거
		SAFE_NIDELETE( m_pOldInfo );
		// 현재거 예전거로 넘기고 사라지게 하자
		m_pOldInfo = m_pCurInfo;
		m_pOldInfo->s_eState = ALPHA_DECREASE;
	}

	_tcscpy_s( m_szText, MAX_PATH, szText );
	m_pCurInfo = NiNew sINFO;
	m_pCurInfo->Init( m_szText, type );
}

void cRegionInfo::Update(float const& fDeltaTime)
{
	if( m_pCurInfo )
	{
		if( m_pCurInfo->Update(fDeltaTime) == false )
			SAFE_NIDELETE( m_pCurInfo );
	}
	if( m_pOldInfo )
	{
		if( m_pOldInfo->Update(fDeltaTime) == false )
			SAFE_NIDELETE( m_pOldInfo );
	}
}

void cRegionInfo::Render()
{
	if( m_pOldInfo )
		m_pOldInfo->Render();
	if( m_pCurInfo )
		m_pCurInfo->Render();
}