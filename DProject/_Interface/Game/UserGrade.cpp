
#include "stdafx.h"
#include "UserGrade.h"

cUserGrade*		g_pUserGrade = NULL;

#ifdef VERSION_ENGLISH
void cUserGrade::BeginWemadeLogo()
{
	if( g_pUserGrade != NULL )
	{
		return;
	}

	g_pUserGrade = NiNew cUserGrade;
	g_pUserGrade->LoopWemadeLogo();
	SAFE_NIDELETE( g_pUserGrade );
}

#else
void cUserGrade::BeginGrade( bool bIncludeString )
{
	if( g_pUserGrade != NULL )
	{
		return;
	}

	g_pUserGrade = NiNew cUserGrade;
	g_pUserGrade->InitGrade( bIncludeString );
}
#endif

void cUserGrade::BeginLogo()
{
	if( g_pUserGrade != NULL )
	{
		return;
	}

	g_pUserGrade = NiNew cUserGrade;
	g_pUserGrade->LoopLogo();
	SAFE_NIDELETE( g_pUserGrade );
}

cUserGrade::cUserGrade()
{
	m_pLogo1 = NULL;
	m_pLogo2 = NULL;
}

void cUserGrade::Delete()
{
	m_StringList.Delete();
	SAFE_NIDELETE( m_pLogo1 );
	SAFE_NIDELETE( m_pLogo2 );
}

#ifndef VERSION_ENGLISH
void cUserGrade::InitGrade( bool bIncludeString )
{
	m_EndTime.SetDeltaTime( 3600 );
	m_EndTime.SetDisable();

	m_fCurAlpha = 0.0f;
	m_nAlphaState = 0;

	if( bIncludeString )
	{
		cText::sTEXTINFO ti;
		ti.Init( &g_pEngine->m_FontText, CFont::FS_20, NiColor::BLACK );
		ti.s_bOutLine = false;
		ti.s_eTextAlign = DT_CENTER;

		cString* pString = NiNew cString;
		ti.SetText( _T( "본 게임물은 12세 이용가 게임으로서" ) );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );

		pString = NiNew cString;
		ti.SetText( _T( "해당 연령 미만의 어린이가 이용하기에는 다소 폭력적인 내용을 포함하고 있으므로" ) );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );

		pString = NiNew cString;
		ti.SetText( _T( "보호자의 지도 감독이 필요한 게임입니다." ) );
		pString->AddText( &ti );
		m_StringList.AddTail( pString );		
	}

	float fRate = bIncludeString ? 0.26f : 0.17f;

	CsPoint size;
	size.x = (int)( g_nScreenWidth*fRate*0.5f );
	size.y = 117 * size.x / 99;

	CsPoint pos( g_nScreenWidth-2, 2 );
	pos.x -= size.x;
	m_pLogo1 = NiNew cSprite;
	m_pLogo1->Init( NULL, pos, size, "LogIn\\NotViolence.tga", false );
	pos.x -= ( size.x + 1 );
	m_pLogo2 = NiNew cSprite;
	m_pLogo2->Init( NULL, pos, size, "LogIn\\TotalityLogo.tga", false );

	m_dwOldTime = GetTickCount();
}
#endif

void cUserGrade::_UpdateLogo()
{
	MSG msg;	
	while( true )
	{
		if( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
		{
			if(msg.message == WM_QUIT )
			{
				SetNextGameStep( GAME_EXIT );
				break;
			}
			if (!TranslateAccelerator(g_hWnd, NULL, &msg))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			_UpdateDeltaTime();

			g_pRenderer->BeginFrame();	
			g_pRenderer->BeginUsingDefaultRenderTargetGroup(NiRenderer::CLEAR_ALL);
			g_pRenderer->SetScreenSpaceCameraData();

			switch( m_nAlphaState )
			{
			case 0:
				if( ( m_fCurAlpha += m_fDeltaTime * 2.0f ) > 1.0f )
				{
					m_fCurAlpha = 1.0f;
					m_nAlphaState = 1;
					m_fTimeValue = 2.0f;
				}
				break;
			case 1:
				if( ( m_fTimeValue -= m_fDeltaTime ) < 0.0f )
				{
					m_nAlphaState = 2;
					m_fCurAlpha = 1.0f;
				}
				else
				{
					Sleep( 20 );
				}				
				break;
			case 2:
				if( ( m_fCurAlpha -= m_fDeltaTime * 2.0f ) < 0.0f )
				{
					m_fCurAlpha = 0.0f;
					m_nAlphaState = 3;					
				}
				break;
			}

			m_pLogo1->SetAlpha( m_fCurAlpha );
			m_pLogo1->Render();

			g_pRenderer->EndUsingRenderTargetGroup();
			g_pRenderer->EndFrame();
			g_pRenderer->DisplayFrame();

			if( m_nAlphaState == 3 )
				break;

			Sleep( 10 );
		}		
	}
}

void cUserGrade::LoopLogo()
{
	m_fCurAlpha = 0.0f;
	m_nAlphaState = 0;
	m_dwOldTime = GetTickCount();

	CsPoint pos( (g_nScreenWidth-500 )/2, (g_nScreenHeight-349)/2 );
	m_pLogo1 = NiNew cSprite;
	m_pLogo1->Init( NULL, pos, CsPoint( 500, 349 ), "LogIn\\Digitalic_logo.tga", false );

	_UpdateLogo();
	Delete();
}

#ifdef VERSION_ENGLISH
void cUserGrade::LoopWemadeLogo()
{
	m_fCurAlpha = 0.0f;
	m_nAlphaState = 0;
	m_dwOldTime = GetTickCount();

	CsPoint pos( (g_nScreenWidth-1024 )/2, (g_nScreenHeight-768)/2 );
	m_pLogo1 = NiNew cSprite;
	m_pLogo1->Init( NULL, pos, CsPoint( 1024, 768 ), "LogIn\\Publish_CI.tga", false );

	_UpdateLogo();
	Delete();
}
#endif

void cUserGrade::Render()
{
	_UpdateDeltaTime();

	switch( m_nAlphaState )
	{
	case 0:
		if( ( m_fCurAlpha += m_fDeltaTime*2.0f ) > 1.0f )
		{
			m_fCurAlpha = 1.0f;
			m_nAlphaState = 1;
			m_fTimeValue = 2.6f;
		}
		break;
	case 1:
		if( ( m_fTimeValue -= m_fDeltaTime ) < 0.0f )
		{
			m_fCurAlpha = 1.0f;		

			if( m_StringList.GetSize() )
			{
				m_nAlphaState = 3;
//				g_pFade = NiNew CFade( FADE_OUT, 0.5f );
			}
			else
			{
				m_nAlphaState = 2;
			}
		}
		break;
	case 2:
		if( ( m_fCurAlpha -= m_fDeltaTime*2.0f ) < 0.0f )
		{
			m_fCurAlpha = 0.0f;
		}
		break;
	}	

	if( m_StringList.GetSize() )
	{
// 		if( g_pFade )
// 		{
// 			g_pFade->Update( m_fDeltaTime );
// 		}

		if( ( m_nAlphaState != 3 ) )//||( g_pFade ) )
		{
			CsPoint pos;
			pos.x = g_nScreenWidth / 2;
			pos.y = (int)( 470 * g_nScreenHeight / 768.0f );
			m_StringList.SetAlpha( m_fCurAlpha );
			m_StringList.Render( pos, 35, DT_CENTER );

			m_pLogo1->SetAlpha( m_fCurAlpha );
			m_pLogo1->Render();
			m_pLogo2->SetAlpha( m_fCurAlpha );
			m_pLogo2->Render();
		}
	}
	else
	{
		m_pLogo1->SetAlpha( m_fCurAlpha );
		m_pLogo1->Render();
		m_pLogo2->SetAlpha( m_fCurAlpha );
		m_pLogo2->Render();
	}
}

void cUserGrade::ResetDevice()
{
	m_StringList.ResetDevice();
	if( m_pLogo1 )
		m_pLogo1->ResetDevice();
	if( m_pLogo2 )
		m_pLogo2->ResetDevice();
}

void cUserGrade::_UpdateDeltaTime()
{
	DWORD dwTime = GetTickCount();
	m_fDeltaTime = CsMax( 2, ( dwTime - m_dwOldTime ) )*0.001f;
	m_dwOldTime = dwTime;
}