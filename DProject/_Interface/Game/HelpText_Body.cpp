
#include "stdafx.h"
#include "HelpText_Body.h"

#define BODY_SCROLL_DELTA			15

void cHelpText_Body::sBODY_IMAGE::Render( CsPoint ptParent )
{	
	CsHelp::sIMAGE* pImageFT = (CsHelp::sIMAGE*)s_pFT;
	assert_cs( pImageFT->GetType() == CsHelp::IMAGE );
	switch( pImageFT->s_nImageType )
	{
	case CsHelp::sIMAGE::IT_NORMAL:
		assert_cs( s_pSprite );
		s_pSprite->Render( s_pFT->s_ptPos + ptParent );
		break;
	case CsHelp::sIMAGE::IT_VIDEO:
		{
			if( s_pSprite && nsDIRECTSHOW::IsPlay() )
			{
				assert_cs( g_pGrabberCallback != NULL );
				_SYNCHRONIZE_THREAD_( g_pGrabberCallback->GetCliticalSection() );
				s_pSprite->Render( s_pFT->s_ptPos + ptParent );
			}
			else
			{
				SAFE_NIDELETE( s_pSprite );
				g_pGameIF->GetHelpText()->GetBody()->GetVideoBG()->Render( s_pFT->s_ptPos + ptParent, pImageFT->s_ptSize );
			}
		}
		break;
	default:
		assert_cs( false );
	}	
}

cHelpText_Body::cHelpText_Body()
{
	m_pVideoBackImage = NULL;
	m_pStencilSprite = NULL;
	m_pScrollBarBody = NULL;
	m_pCurContainerFT = NULL;
	m_pTopBtn = NULL;
}

void cHelpText_Body::Delete()
{
	DeleteBodyResource();

	SAFE_NIDELETE( m_pVideoBackImage );
	SAFE_NIDELETE( m_pStencilSprite );
	SAFE_NIDELETE( m_pScrollBarBody );
	SAFE_NIDELETE( m_pTopBtn );	
}

void cHelpText_Body::DeleteBodyResource()
{
	m_pCurContainerFT = NULL;

	int nCount = m_vpBody.Size();
	for( int i=0; i<nCount; ++i )
	{
		delete m_vpBody[ i ];
	}
	m_vpBody.Destroy();

	if( nsDIRECTSHOW::IsPlay() )
	{
		nsDIRECTSHOW::StopDShow();
	}
}

void cHelpText_Body::Init()
{
	{
		NiStencilProperty* propStencil = NiNew NiStencilProperty;
		propStencil->SetStencilReference( 1 );
		propStencil->SetStencilFunction( NiStencilProperty::TEST_EQUAL );
		propStencil->SetStencilPassAction( NiStencilProperty::ACTION_KEEP );
		propStencil->SetStencilOn( true );

		m_pVideoBackImage = NiNew cSprite;
		m_pVideoBackImage->InitStencil( NULL, CsPoint::ZERO, CsPoint( 1, 1 ), "Help\\VedioBG.bmp", false, propStencil );
	}

	{
		NiStencilProperty* propStencil = NiNew NiStencilProperty;
		propStencil->SetStencilReference( 1 );
		propStencil->SetStencilFunction( NiStencilProperty::TEST_ALWAYS );
		propStencil->SetStencilPassAction( NiStencilProperty::ACTION_REPLACE );	
		propStencil->SetStencilOn( true );

		m_pStencilSprite = NiNew cSprite;
		m_pStencilSprite->InitStencil( NULL, CsPoint::ZERO, cHelpText::GetBodyEndPos() - cHelpText::GetBodyStartPos(), true, propStencil );
		m_pStencilSprite->SetAlpha( 0.0f );	
	}

	CsRect rcScrollBar( cHelpText::GetBodyStartPos(), cHelpText::GetBodyEndPos() );
	m_pScrollBarBody = NiNew cScrollBar;
	m_pScrollBarBody->Init( cScrollBar::TYPE_1, g_pGameIF->GetHelpText()->GetRoot(), CsPoint( 364, 33 ), CsPoint( 16, 441 ), cScrollBar::GetDefaultBtnSize(), rcScrollBar, 480/BODY_SCROLL_DELTA, false );

	m_pTopBtn = NiNew cButton;
	m_pTopBtn->Init( g_pGameIF->GetHelpText()->GetRoot(), CsPoint( 310, 440 ), CsPoint( 50, 26 ), cButton::IMAGE_NOR_10, UISTRING_TEXT( "HELPTEXT_BTN_UP" ).c_str(), false );
}

void cHelpText_Body::Update(float const& fDeltaTime)
{
	if( m_pScrollBarBody->Update_ForMouse() == cScrollBar::ACTION_CLICK )
		return;
}

bool cHelpText_Body::Update_ForMouse( CsPoint ptLocal )
{
	switch( m_pTopBtn->Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		SetLine( 0 );
		return true;
	case cButton::ACTION_NONE:
		break;
	default:
		return true;
	}

	if( CURSOR_ST.GetButtonState() != CURSOR::LBUTTON_DOWN )
		return false;

	CsRect rc( cHelpText::GetBodyStartPos(), cHelpText::GetBodyEndPos() );
	if( rc.PtInRect( ptLocal ) == FALSE )
	{
		return false;
	}	

	CsPoint ptBodyLocal = ptLocal - cHelpText::GetBodyStartPos() + _GetScrollPos();
	_Pick( ptBodyLocal );
	return true;
}

void cHelpText_Body::Render( CsPoint ptParent )
{
	m_pScrollBarBody->Render();
	m_pStencilSprite->Render( ptParent );

	ptParent -= _GetScrollPos();

	int nCount = m_vpBody.Size();
	for( int i=0; i<nCount; ++i )
	{
		m_vpBody[ i ]->Render( ptParent );
	}

	if( m_pScrollBarBody->GetCurPosIndex() != 0 )
	{
		m_pTopBtn->Render();
	}
}

void cHelpText_Body::ResetDevice()
{
	m_pScrollBarBody->ResetDevice();
	m_pStencilSprite->ResetDevice();
	m_pVideoBackImage->ResetDevice();
	m_pTopBtn->ResetDevice();

	int nCount = m_vpBody.Size();
	for( int i=0; i<nCount; ++i )
	{		
		m_vpBody[ i ]->ResetDevice();
	}
}

void cHelpText_Body::SetLine( int nLine )
{
	m_pScrollBarBody->SetCurPosIndex( nLine );
}

void cHelpText_Body::ApplyContainer( CsHelp::sCONTAINER* pConFT )
{
	if( m_pCurContainerFT == pConFT )
		return;

	DeleteBodyResource();
	m_pCurContainerFT = pConFT;

	int nCount = pConFT->s_vpImage.Size();
	for( int i=0; i<nCount; ++i )
	{
		ApplyImage( pConFT->s_vpImage[ i ], false );
	}

	nCount = pConFT->s_vpString.Size();
	for( int i=0; i<nCount; ++i )
	{
		ApplyString( pConFT->s_vpString[ i ], false );
	}

	m_pScrollBarBody->SetCurPosIndex( 0 );
	m_pScrollBarBody->SetRange( CsPoint( 0, pConFT->s_nLimitScroll + m_pScrollBarBody->GetRenderCount() ) );
}


void cHelpText_Body::ApplyImage( CsHelp::sIMAGE* pImageFT, bool bSelectBody )
{
	// 기존에 잇는거에서 찾아본다
	sBODY_IMAGE* pBodyImage = (sBODY_IMAGE*)_GetBody( pImageFT );
	assert_cs( pBodyImage->GetType() == CsHelp::IMAGE );

	if( pBodyImage->s_pSprite == NULL )
	{
		switch( pImageFT->s_nImageType )
		{
		case CsHelp::sIMAGE::IT_NORMAL:
			{
				NiStencilProperty* propStencil = NiNew NiStencilProperty;
				propStencil->SetStencilReference( 1 );
				propStencil->SetStencilFunction( NiStencilProperty::TEST_EQUAL );
				propStencil->SetStencilPassAction( NiStencilProperty::ACTION_KEEP );
				propStencil->SetStencilOn( true );

				pBodyImage->s_pSprite = NiNew cSprite;
				pBodyImage->s_pSprite->InitStencil( NULL, CsPoint::ZERO, pImageFT->s_ptSize, (char*)pImageFT->s_ImagePath.c_str(), true, propStencil );
			}
			break;
		case CsHelp::sIMAGE::IT_VIDEO:
			{				
			}
			break;
		default:
			assert_cs( false );
		}
	}		
}

void cHelpText_Body::ApplyString( CsHelp::sSTRING* pStrFT, bool bSelectBody )
{
	// 기존에 잇는거에서 찾아본다
	sBODY_STR* pBodyStr = (sBODY_STR*)_GetBody( pStrFT );
	assert_cs( pBodyStr->GetType() == CsHelp::STRING );
	pBodyStr->s_StringList.Delete();

	int nTextCnt = pStrFT->s_vpText.Size();	

	cText::sTEXTINFO ti;
	ti.Init();

	CsHelp::sTEXT* pTextFT = NULL;
	for( int i=0; i<nTextCnt; ++i )
	{
		pTextFT = pStrFT->s_vpText[ i ];

		ti.Init( pTextFT );
		g_pStringAnalysis->AddCut( &pBodyStr->s_StringList, 9999, (TCHAR*)pTextFT->s_Text.c_str(), &ti );
	}
}

cHelpText_Body::sBODY_BASE* cHelpText_Body::_GetBody( CsHelp::sELEMENT* pEleFT )
{
	int nCount = m_vpBody.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpBody[ i ]->s_pFT == pEleFT )
			return m_vpBody[ i ];
	}

	cHelpText_Body::sBODY_BASE* pNew = NULL;
	switch( pEleFT->GetType() )
	{
	case CsHelp::STRING:
		{
			pNew = csnew cHelpText_Body::sBODY_STR;
			pNew->s_pFT = pEleFT;			

			sSTR_INFO si;
			si.s_nLineHeight = ( (CsHelp::sSTRING*)pEleFT )->s_nLineDeltaHeight;
			si.s_dwPlag = sSTR_INFO::SF_LINE_HEIGHT | sSTR_INFO::SF_BASE_HEIGHT | sSTR_INFO::SF_STENCIL_USE_1;

			( (sBODY_STR*)pNew )->s_StringList.SetStrInfo( &si );			
		}
		break;
	case CsHelp::IMAGE:
		{
			pNew = csnew cHelpText_Body::sBODY_IMAGE;
			pNew->s_pFT = pEleFT;
		}
		break;
	default:
		assert_cs( false );
	}

	m_vpBody.PushBack( pNew );
	return pNew;
}

CsPoint cHelpText_Body::_GetScrollPos()
{
	return CsPoint( 0, m_pScrollBarBody->GetCurPosIndex() * BODY_SCROLL_DELTA );
}



void cHelpText_Body::_PlayVideo()
{
#ifdef VERSION_USA
	return;
#endif

	if( nsDIRECTSHOW::IsPlay() )
		return;

	// 현재 바디에 이미 동영상이 있다면
	int nBodySize =	m_vpBody.Size();
	sBODY_IMAGE* pBodyImage = NULL;
	for( int i=0; i<nBodySize; ++i )
	{
		if( m_vpBody[ i ]->GetType() == CsHelp::IMAGE )
		{
			if( ( (CsHelp::sIMAGE*)m_vpBody[ i ]->s_pFT )->s_nImageType == CsHelp::sIMAGE::IT_VIDEO )
			{
				pBodyImage = (sBODY_IMAGE*)m_vpBody[ i ];
				break;
			}
		}
	}
	if( pBodyImage == NULL )
		return;

	if( pBodyImage->s_pSprite != NULL )
		return;

	NiStencilProperty* propStencil = NiNew NiStencilProperty;
	propStencil->SetStencilReference( 1 );
	propStencil->SetStencilFunction( NiStencilProperty::TEST_EQUAL );
	propStencil->SetStencilPassAction( NiStencilProperty::ACTION_KEEP );
	propStencil->SetStencilOn( true );

	CsHelp::sIMAGE* pImageFT = (CsHelp::sIMAGE*)pBodyImage->s_pFT;
	if( nsDIRECTSHOW::InitDShowTexture( (char*)pImageFT->s_ImagePath.c_str(), true ) == true )
	{
		pBodyImage->s_pSprite = NiNew cSprite;
		pBodyImage->s_pSprite->InitStencil( NULL, CsPoint::ZERO, pImageFT->s_ptSize, g_pGrabberCallback->GetDShowTex(), false, propStencil );
	}
	else
	{
		SAFE_NIDELETE( pBodyImage->s_pSprite );
	}
}

void cHelpText_Body::_Pick( CsPoint ptLocal )
{
	int nCount = m_vpBody.Size();
	for( int body=0; body<nCount; ++body )
	{
		switch( m_vpBody[ body ]->GetType() )
		{
		case CsHelp::STRING:
			{
				sBODY_STR* pCheckBody = (sBODY_STR*)m_vpBody[ body ];
				CsHelp::sSTRING* pCheckFT = (CsHelp::sSTRING*)pCheckBody->s_pFT;
				assert_cs( pCheckFT->GetType() == CsHelp::STRING );
				CsPoint ptBodyLocal = ptLocal - pCheckFT->s_ptPos;
				int nTextCnt = pCheckFT->s_vpText.Size();
				for( int text=0; text<nTextCnt; ++text )
				{
					CsHelp::sTEXT* pCheckTextFT = pCheckFT->s_vpText[ text ];
					switch( pCheckTextFT->s_TextType )
					{
					case CsHelp::sTEXT::TT_NORMAL:
						continue;
					case CsHelp::sTEXT::TT_LINK:
						if( pCheckTextFT->s_rcRegion.PtInRect( ptBodyLocal ) == TRUE )
						{
							g_pGameIF->GetHelpText()->GetTitle()->SetSelectTitle( pCheckTextFT->s_TextTypeVal1 );
							m_pScrollBarBody->SetCurPosIndex( pCheckTextFT->s_TextTypeVal2 );
							return;
						}
						break;
					default:
						assert_cs( false );
					}
				}
			}
			break;
		case CsHelp::IMAGE:
			{
				sBODY_IMAGE* pCheckBody = (sBODY_IMAGE*)m_vpBody[ body ];
				CsHelp::sIMAGE* pCheckFT = (CsHelp::sIMAGE*)pCheckBody->s_pFT;
				assert_cs( pCheckFT->GetType() == CsHelp::IMAGE );
				CsPoint ptBodyLocal = ptLocal;
				switch( pCheckFT->s_nImageType )
				{
				case CsHelp::sIMAGE::IT_NORMAL:
					continue;
				case CsHelp::sIMAGE::IT_VIDEO:
					break;
				default:
					assert_cs( false );
				}

				CsRect rc( pCheckFT->s_ptPos, pCheckFT->s_ptPos + pCheckFT->s_ptSize );
				if( rc.PtInRect( ptBodyLocal ) == TRUE )
				{
					_PlayVideo();
					return;
				}
			}
			break;
		default:
			assert_cs( false );
		}
	}
}