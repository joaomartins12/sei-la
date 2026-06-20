#include "stdafx.h"
#include "Alime.h"

cAlime::cAlime()
{
	m_eType = NONE;
	m_nMainID = m_nSubID_1 = m_nSubID_2 = m_nSubID_3 = -1;
	m_pData = NULL;

	m_bUp = true;	
}

void cAlime::ResetMap()
{
	while( m_QueueAlime.empty() == false ) 
	{ 
		m_QueueAlime.pop();
	} 
}

void cAlime::Delete()
{	
	SAFE_NIDELETE( m_pPopupWindow );
	SAFE_NIDELETE( m_pPost );
	SAFE_NIDELETE( m_pPcMark );

	m_StringList.Delete();		

	while( m_QueueAlime.empty() == false )
	{
		m_QueueAlime.pop();
	}
}

void cAlime::Init()
{
	m_pPopupWindow = NiNew cPopUp;
	m_pPopupWindow->Init( NULL, CsPoint::ZERO, CsPoint( 10, 10 ), CsPoint( 16, 16 ), "Control_G\\Popup\\N2Dlg", "tga", false );	

	m_pPost = NiNew cSprite;	
	m_pPost->Init( NULL, CsPoint::ZERO, CsPoint( 32, 19 ), "Mail\\Letter_One.tga", false );	

	m_pPcMark = NiNew cSprite;	
	m_pPcMark->Init( NULL, CsPoint::ZERO, CsPoint( 22, 22 ), "Event\\PC_Bang.tga", false );
}

void cAlime::_Update( sALIME* pAlime )
{	
	if( pAlime->s_bSet == false )
	{
		m_StringList.Delete();
		m_StringList.AddTail( pAlime->s_pString );
		m_pPopupWindow->ReSize( pAlime->s_ptMaxSize );
		pAlime->s_bSet = true;
	}	

	float fHeight = g_nScreenHeight - 120.0f;			//빼는 값이 높을수록 더 많이 올라간다.
	if( pAlime->s_bDir == false )
	{
		pAlime->s_ptRenderPos.y -= g_fDeltaTime * 18; 		  
		if( pAlime->s_ptRenderPos.y < fHeight )
		{
			pAlime->s_bDir = true;
		}
	}
	else 
	{
		pAlime->s_ptRenderPos.y += g_fDeltaTime * 18;
		if( pAlime->s_ptRenderPos.y > g_nScreenHeight )
		{
			m_QueueAlime.pop();
		}		
	}
}

void cAlime::Render()
{
	if( m_QueueAlime.empty() == true )
		return;

	sALIME* pAlime = &m_QueueAlime.front();

	_Update( pAlime );

	CsPoint pos( (int)pAlime->s_ptRenderPos.x, (int)pAlime->s_ptRenderPos.y );	
	// 우측으로 나갔다면
	if( ( pAlime->s_ptMaxSize.x + pos.x ) > g_nScreenWidth )
	{
		pos.x = (int)pAlime->s_ptRenderPos.x - pAlime->s_ptMaxSize.x;
	}
	//// 아래로 나갔다면
	//if( ( m_ptMaxSize.y + pos.y ) > g_nScreenHeight )
	//{
	//	pos.y = (int)pAlime->s_ptRenderPos.y - m_ptMaxSize.y;
	//}
	//// 좌나 위로 나갔다면
	//if( pos.y < 0 )		pos.y = 0;
	//if( pos.x < 0 )		pos.x = 0;

	m_pPopupWindow->Render( pos );	
	m_StringList.Render( pos + CsPoint( 10, 10 ), 7 );
}

void cAlime::ResetDevice()
{
	m_pPopupWindow->ResetDevice();
	m_StringList.ResetDevice();

	if( m_pPost )
		m_pPost->ResetDevice();	

	if( m_pPcMark )
		m_pPcMark->ResetDevice();	
}

void cAlime::Release()
{
	ResetMap();
}

void cAlime::SetAlime( int nMaxSizeX, eTYPE type, int nMainID, int nSubID_1 /*=0*/, int nSubID_2 /*=0*/, int nSubID_3 /*=0*/, void* pData /*=NULL*/ )
{	
	m_eType = type;
	m_nMainID = nMainID;
	m_nSubID_1 = nSubID_1;
	m_nSubID_2 = nSubID_2;
	m_nSubID_3 = nSubID_3;
	m_pData = pData;	

	sALIME pAlime;
	switch( m_eType )
	{
	case MAIL:			pAlime.s_pString = _MakeAlime_Mail();			break;
	case PCBANG:		pAlime.s_pString = _MakeAlime_PCbang();			break;
	case EVENT:			pAlime.s_pString = _MakeAlime_Event();			break;
	default:			assert_cs( false );
	}

	// 마무리 설정
	//m_ptMaxSize = m_StringList.CalMaxSize( 7 ) + CsPoint( 20, 38 );
	pAlime.s_ptMaxSize = CsPoint( 20, 38 ); //일단 줄바꿈 안하고 한줄로만 만든다..
	pAlime.s_ptMaxSize.x = CsMax( nMaxSizeX, pAlime.s_ptMaxSize.x );	

	pAlime.s_ptRenderPos = NiPoint2( (float)(g_nScreenWidth - pAlime.s_ptMaxSize.x), (float)(g_nScreenHeight + pAlime.s_ptMaxSize.y) );

	m_QueueAlime.push( pAlime );
}

cString* cAlime::_MakeAlime_Mail()
{
	cString* pString;

	pString = NiNew cString;
	pString->AddSprite( m_pPost );
	pString->TailAddSizeX( 10 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_WHITE );	
	ti.SetText( UISTRING_TEXT( "ALIME_MAIL_ARRIVED" ).c_str() );
	pString->AddText( &ti );		

	return pString;
}

cString* cAlime::_MakeAlime_PCbang()
{
	cString* pString;

	pString = NiNew cString;
	pString->AddSprite( m_pPcMark );
	pString->TailAddSizeX( 10 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_WHITE );	
	ti.SetText( _ONLY_KOR( "프리미엄 PC방에 접속중입니다." ) );
	pString->AddText( &ti );		

	return pString;
}

cString* cAlime::_MakeAlime_Event()
{
	cString* pString;

	pString = NiNew cString;
	pString->AddSprite( m_pPost );
	pString->TailAddSizeX( 10 );

	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, CFont::FS_12, FONT_WHITE );	
	ti.SetText( UISTRING_TEXT( "ALIME_PROVIDE_EVENT_ITEM" ).c_str() );
	pString->AddText( &ti );		

	return pString;
}