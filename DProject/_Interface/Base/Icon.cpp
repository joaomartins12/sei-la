#include "stdafx.h"
#include "Icon.h"

cText*			cIcon::g_pTextNumber[ 10 ] = { NULL, };

float		cIcon::m_flickerTime = 0.0f;

bool		cIcon::bFlicker = false;

#define		FLICKER_TIME	0.5f

void cIcon::GlobalShotdown()
{
	for( int i=0; i<10; ++i )
	{
		SAFE_NIDELETE( g_pTextNumber[ i ] )
	}
}

void cIcon::GlobalInit()
{
	// 숫자
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_9 );
	for( int i=0; i<10; ++i )
	{
		assert_cs( g_pTextNumber[ i ] == NULL );
		g_pTextNumber[ i ] = NiNew cText;
		ti.SetText( i );
		g_pTextNumber[ i ]->Init( NULL, CsPoint::ZERO, &ti, true );
	}
}

void cIcon::GlobalResetDevice()
{
	// 숫자
	for( int i=0; i<10; ++i )
	{
		if( g_pTextNumber[ i ] )
			g_pTextNumber[ i ]->ResetDevice();
	}		
}

void cIcon::GlobalUpdate()
{
	m_flickerTime += g_fDeltaTime;			// 퀘스트 관련 아이콘 깜박임 효과

	if(m_flickerTime >= FLICKER_TIME)
	{
		bFlicker = !bFlicker;
		m_flickerTime = 0.0f;
	}
}

bool cIcon::FlickerMode(int nIndex)
{
	switch(nIndex)					// 퀘스트 관련 아이콘 인덱스
	{
	case 0:		case 1:		case 2:
	case 10:	case 11:	case 12:
		{
			if(bFlicker)
				return true;
		}
	}

	return false;
}

void cIcon::ResetDevice()
{
	m_pSpr->ResetDevice(); 

	std::map< int, sIcon_Text* >::iterator it = m_MapIconText.begin();
	std::map< int, sIcon_Text* >::iterator itEnd = m_MapIconText.end();
	for( ; it != itEnd; it++ )
	{		
		it->second->s_IconText->ResetDevice();
	}
}

void cIcon::Delete()
{
	SAFE_NIDELETE( m_pSpr );

	std::map< int, sIcon_Text* >::iterator it = m_MapIconText.begin();
	std::map< int, sIcon_Text* >::iterator itEnd = m_MapIconText.end();
	for( ; it != itEnd; it++ )
	{
		SAFE_NIDELETE( it->second );
	}
	m_MapIconText.clear();
}

void cIcon::Init( CsPoint patch, CsPoint size, char* cPath )
{
	assert_cs( m_pSpr == NULL );

	m_ptPatchSize = patch;
	m_ptIndexCount = CsPoint( size.x / m_ptPatchSize.x, size.y / m_ptPatchSize.y );

	m_vPatchTex = NiPoint2( 1.0f/(float)m_ptIndexCount.x, 1.0f/(float)m_ptIndexCount.y );

	m_pSpr = NiNew cSprite;
	m_pSpr->Init( NULL, CsPoint::ZERO, m_ptPatchSize, cPath, true );	
}

//====================================================================================
//
//		이미지
//
//====================================================================================
void cIcon::Render( int nIconIdx, CsPoint pos )
{
	if( m_pSpr )
	{
		NiPoint2 tex = NiPoint2( ( nIconIdx%m_ptIndexCount.x )/(float)m_ptIndexCount.x, ( nIconIdx/m_ptIndexCount.x )/(float)m_ptIndexCount.y );
		m_pSpr->Render( pos, tex, m_vPatchTex + tex );
	}

	if( m_MapIconText.empty() )
		return;
	
	std::map< int, sIcon_Text* >::iterator itFind = m_MapIconText.find( nIconIdx );
	if( itFind == m_MapIconText.end() )
		return;

	if( itFind->second && itFind->second->s_IconText )
	{
		CsPoint DeltaPos = itFind->second->s_DeltaPos;
		itFind->second->s_IconText->Render( pos + DeltaPos, DT_CENTER );
	}
}

void cIcon::Render( int nCol, int nRow, CsPoint pos )
{
	if( m_pSpr )
	{
		NiPoint2 tex = NiPoint2( (float)nCol/(float)m_ptIndexCount.x, (float)nRow/(float)m_ptIndexCount.y );
		m_pSpr->Render( pos, tex, m_vPatchTex + tex );
	}

	if( m_MapIconText.empty() )
		return;
	
	int nIconIdx = ( nCol * m_ptIndexCount.x ) + nRow;
	std::map< int, sIcon_Text* >::iterator itFind = m_MapIconText.find( nIconIdx );
	if( itFind == m_MapIconText.end() )
		return;

	if( itFind->second && itFind->second->s_IconText )
	{
		CsPoint DeltaPos = itFind->second->s_DeltaPos;
		itFind->second->s_IconText->Render( pos + DeltaPos, DT_CENTER );				
	}
}

void cIcon::Render( int nCol, int nRow, CsPoint pos, CsPoint size )
{
	assert_cs( m_pSpr != NULL );
	if( m_pSpr )
	{
		NiPoint2 tex = NiPoint2( (float)nCol/(float)m_ptIndexCount.x, (float)nRow/(float)m_ptIndexCount.y );
		m_pSpr->Render( pos, size, tex, m_vPatchTex + tex );
	}

	if(m_MapIconText.empty())
		return;

	int nIconIdx = ( nCol * m_ptIndexCount.x ) + nRow;
	std::map< int, sIcon_Text* >::iterator itFind = m_MapIconText.find( nIconIdx );
	if( itFind == m_MapIconText.end() )
		return;

	if( itFind->second && itFind->second->s_IconText )
	{
		CsPoint DeltaPos = itFind->second->s_DeltaPos;
		itFind->second->s_IconText->Render( pos + DeltaPos, DT_CENTER );			
	}
}

void cIcon::Render( int nIconIdx, CsPoint pos, CsPoint size )
{
	if( m_pSpr )
	{
		NiPoint2 tex = NiPoint2( ( nIconIdx%m_ptIndexCount.x )/(float)m_ptIndexCount.x, ( nIconIdx/m_ptIndexCount.x )/(float)m_ptIndexCount.y );
		m_pSpr->Render( pos, size, tex, m_vPatchTex + tex );
	}

	if( m_MapIconText.empty() )
		return;

	std::map< int, sIcon_Text* >::iterator itFind = m_MapIconText.find( nIconIdx );
	if( itFind == m_MapIconText.end() )
		return;

	if( itFind->second && itFind->second->s_IconText )
	{
		CsPoint DeltaPos = itFind->second->s_DeltaPos;
		itFind->second->s_IconText->Render( pos + DeltaPos, DT_CENTER );		
	}
}


void cIcon::Render( int nIconIdx, CsPoint pos, CsPoint size, float fAlpha )
{
	if( m_pSpr )
	{
		NiPoint2 tex = NiPoint2( ( nIconIdx%m_ptIndexCount.x )/(float)m_ptIndexCount.x, ( nIconIdx/m_ptIndexCount.x )/(float)m_ptIndexCount.y );
		m_pSpr->SetAlpha(fAlpha);
		m_pSpr->Render( pos, size, tex, m_vPatchTex + tex );
	}	

	if( m_MapIconText.empty() )
		return;

	std::map< int, sIcon_Text* >::iterator itFind = m_MapIconText.find( nIconIdx );
	if(  itFind == m_MapIconText.end() )
		return;

	if( itFind->second && itFind->second->s_IconText )
	{
		CsPoint DeltaPos = itFind->second->s_DeltaPos;
		itFind->second->s_IconText->Render( pos + DeltaPos, DT_CENTER );		
	}
}

//====================================================================================
//
//		카운트
//
//====================================================================================
void cIcon::RenderCount( int nCount, CsPoint pos )
{
	// Removed assert_cs( nCount != 0 ) — the very next line already handles nCount<=0
	// gracefully (early-return). The assert was pure dev-warning noise; in Release_English
	// it called _log() → MessageBoxA, popping up a "CsAssert" dialog every time a gift /
	// reward / consigned-shop slot serialized with stack count zero. Server-side serialization
	// filters protect against zero-amount items in the regular wire path, but this UI path
	// (cServer_Reward::RenderGift) reads from cItemInfo where the count can legitimately be 0
	// for empty/expired gift slots that the renderer is supposed to skip silently anyway.

	if( nCount <= 0 || nCount == 1)	// 0이하일 경우는 없는 경우, 1 인경우에는 숫자 1 표시 안함.
		return;

	pos.x += ( m_ptPatchSize.x - 7 );
	pos.y += ( m_ptPatchSize.y - 15 );

	int nRemain = nCount;
	int nNumber = nRemain%10;

	while( nRemain != 0 )
	{
		g_pTextNumber[ nNumber ]->Render( pos, DT_CENTER );
		nRemain = nRemain/10;
		nNumber = nRemain%10;		
		pos.x -= 6;
	}
}

//====================================================================================
//
//		아이콘 위에 텍스트 입히기
//
//====================================================================================
void cIcon::SetIconText( int nIconIdx, TCHAR const* szText, CsPoint ptDeltaPos )
{
	assert_cs( m_pSpr != NULL );
	assert_cs( m_MapIconText[ nIconIdx ] == NULL );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_10, FONT_WHITE );
	ti.SetText( szText );

	sIcon_Text* pIconText = NiNew sIcon_Text;

	pIconText->s_IconText = NiNew cText;
	pIconText->s_IconText->Init( NULL, CsPoint::ZERO, &ti, false );

	pIconText->s_DeltaPos = ptDeltaPos;	
	
	m_MapIconText[ nIconIdx ] = pIconText;
}


