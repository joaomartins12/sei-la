
#include "stdafx.h"
#include "ProgressBar.h"

cProgressBar::cProgressBar() : m_pSpriteStart(NULL),m_pSpriteCenter(NULL),m_pSpriteEnd(NULL),
m_eBarType(BAR_3IMAGE),m_ePosTextType(TEXT_NONE),m_pText(NULL),m_nCurPos(0),m_nRenderWidth(0),m_fWidthRate(0.0f)
{
	m_ptRange = CsPoint( 0, 1 );
}

void cProgressBar::Delete()
{
	SAFE_NIDELETE( m_pSpriteStart );
	SAFE_NIDELETE( m_pSpriteCenter );
	SAFE_NIDELETE( m_pSpriteEnd );
	SAFE_NIDELETE( m_pText );
}

void cProgressBar::Init( cWindow* pParent, eBAR_TYPE Bar_Type, CsPoint pos, CsPoint tsize, CsPoint SE_TexSX,
						char* cImagePath, bool bApplyWindowSize )
{
	cWindow::Init( pParent, pos, tsize, bApplyWindowSize );

	assert_cs( m_pSpriteStart == NULL );
	assert_cs( m_pSpriteCenter == NULL );
	assert_cs( m_pSpriteEnd == NULL );

	m_eBarType = Bar_Type;
	m_ptStart_End_TextureSizeX = SE_TexSX;

	m_pSpriteStart = NiNew cSprite;
	m_pSpriteStart->Init( this, CsPoint::ZERO, CsPoint( m_ptStart_End_TextureSizeX.x, tsize.y ), cImagePath, bApplyWindowSize );

	if( m_eBarType == BAR_3IMAGE )
	{
		char cTex[ MAX_PATH ];
		strcpy_s( cTex, MAX_PATH, cImagePath );
		size_t len = strlen( cTex );
		cTex[ len - 5 ] = '2';
		m_pSpriteCenter = NiNew cSprite;
		m_pSpriteCenter->Init( this, CsPoint::ZERO, CsPoint( 1, tsize.y ), cTex, bApplyWindowSize );

		cTex[ len - 5 ] = '3';
		m_pSpriteEnd = NiNew cSprite;
		m_pSpriteEnd->Init( this, CsPoint::ZERO, CsPoint( m_ptStart_End_TextureSizeX.y, tsize.y ), cTex, bApplyWindowSize );
	}	
}

void cProgressBar::SetAlpha( float fAlpha )
{
	if( m_pSpriteStart )	m_pSpriteStart->SetAlpha( fAlpha );
	if( m_pSpriteCenter )	m_pSpriteCenter->SetAlpha( fAlpha );
	if( m_pSpriteEnd )		m_pSpriteEnd->SetAlpha( fAlpha );	
}

void cProgressBar::Update(float const& fDeltaTime)
{

}

void cProgressBar::Render()
{
	if( !m_bVisible )
		return;

	if( ( m_nRenderWidth != 0 )&&( m_nCurPos != INVALIDE_VALUE ) )
	{
		switch( m_eBarType )
		{
		case BAR_1IMAGE:
			m_pSpriteStart->Render( CsPoint::ZERO, CsPoint( m_nRenderWidth, GetSize().y ) );
			break;
		case BAR_3IMAGE:		
			if( m_ptStart_End_TextureSizeX.x >= m_nRenderWidth )
			{
				m_pSpriteStart->Render( CsPoint::ZERO, CsPoint( m_nRenderWidth, GetSize().y ) );
			}
			else if( m_ptStart_End_TextureSizeX.Sum() >= m_nRenderWidth )
			{
				m_pSpriteStart->Render( CsPoint::ZERO, CsPoint( m_ptStart_End_TextureSizeX.x, GetSize().y ) );
				m_pSpriteCenter->Render( CsPoint( m_ptStart_End_TextureSizeX.x, 0 ), CsPoint( m_nRenderWidth - m_ptStart_End_TextureSizeX.x, GetSize().y ) );
			}
			else
			{
				m_pSpriteStart->Render( CsPoint::ZERO, CsPoint( m_ptStart_End_TextureSizeX.x, GetSize().y ) );
				m_pSpriteCenter->Render( CsPoint( m_ptStart_End_TextureSizeX.x, 0 ), CsPoint( m_nRenderWidth - m_ptStart_End_TextureSizeX.Sum(), GetSize().y ) );
				m_pSpriteEnd->Render( CsPoint( m_nRenderWidth-m_ptStart_End_TextureSizeX.y, 0 ), CsPoint( m_ptStart_End_TextureSizeX.y, GetSize().y ) );
			}
			break;
		case BAR_STATIC:
			m_pSpriteStart->Render( CsPoint::ZERO, CsPoint( m_nRenderWidth, GetSize().y ), NiPoint2( 0, 0 ), NiPoint2( m_fWidthRate, 1 ) );
			break;
		default:
			assert_cs( false );
		}
	}

	if( m_pText )
		m_pText->Render();
}

void cProgressBar::Render(CsPoint pos)
{
	if( !m_bVisible )
		return;

	if( ( m_nRenderWidth != 0 )&&( m_nCurPos != INVALIDE_VALUE ) )
	{
		switch( m_eBarType )
		{
		case BAR_1IMAGE:
			m_pSpriteStart->Render( pos, CsPoint( m_nRenderWidth, GetSize().y ) );
			break;
		case BAR_3IMAGE:		
			if( m_ptStart_End_TextureSizeX.x >= m_nRenderWidth )
			{
				m_pSpriteStart->Render( pos, CsPoint( m_nRenderWidth, GetSize().y ) );
			}
			else if( m_ptStart_End_TextureSizeX.Sum() >= m_nRenderWidth )
			{
				m_pSpriteStart->Render( pos, CsPoint( m_ptStart_End_TextureSizeX.x, GetSize().y ) );
				m_pSpriteCenter->Render( pos+ CsPoint( m_ptStart_End_TextureSizeX.x, 0 ), CsPoint( m_nRenderWidth - m_ptStart_End_TextureSizeX.x, GetSize().y ) );
			}
			else
			{
				m_pSpriteStart->Render( pos, CsPoint( m_ptStart_End_TextureSizeX.x, GetSize().y ) );
				m_pSpriteCenter->Render( pos + CsPoint( m_ptStart_End_TextureSizeX.x, 0 ), CsPoint( m_nRenderWidth - m_ptStart_End_TextureSizeX.Sum(), GetSize().y ) );
				m_pSpriteEnd->Render( pos + CsPoint( m_nRenderWidth-m_ptStart_End_TextureSizeX.y, 0 ), CsPoint( m_ptStart_End_TextureSizeX.y, GetSize().y ) );
			}
			break;
		case BAR_STATIC:
			m_pSpriteStart->Render( pos, CsPoint( m_nRenderWidth, GetSize().y ), NiPoint2( 0, 0 ), NiPoint2( m_fWidthRate, 1 ) );
			break;
		default:
			assert_cs( false );
		}
	}

	if( m_pText )
		m_pText->Render( pos );
}

void cProgressBar::ResetDevice()
{
	cWindow::ResetDevice();

	if( m_pSpriteStart )
		m_pSpriteStart->ResetDevice();
	if( m_pSpriteCenter )
		m_pSpriteCenter->ResetDevice();
	if( m_pSpriteEnd )
		m_pSpriteEnd->ResetDevice();
	if( m_pText )
		m_pText->ResetDevice();
}

bool cProgressBar::SetBarPos( int nPos )
{
	if( ( m_nCurPos == nPos )&&( m_ptBackupRange == m_ptRange ) )
		return false;

	m_ptRange = m_ptBackupRange;
	m_nCurPos = nPos;

	assert_cs( m_ptRange.x <= m_ptRange.y );
	m_nCurPos = CsMax( m_nCurPos, m_ptRange.x );
	m_nCurPos = CsMin( m_nCurPos, m_ptRange.y );

	int nDeltaRange = m_ptRange.y - m_ptRange.x;
	int nDeltaPos = m_nCurPos - m_ptRange.x;
	float fRate = CsMin( 1.0f, (float)nDeltaPos/(float)nDeltaRange );
	float fWindow = 1.0f;
	if( m_bApplyWindowSize == true )
	{
		fWindow = (float)g_nScreenWidth/(float)DEFAULT_SCREEN_WIDTH;
	}
	m_fWidthRate = fRate*fWindow;
	m_nRenderWidth = (int)( GetSize().x*m_fWidthRate );

	std::wstring wsText;
	switch( m_ePosTextType )
	{
	case TEXT_NONE:
		return true;
	case TEXT_VALUE:
		{
			if( m_nCurPos != INVALIDE_VALUE )
				DmCS::StringFn::Format( wsText, L"%d / %d", m_nCurPos, m_ptRange.y );
			else
				wsText = L"? / ?";
		}
		break;
	case TEXT_PERCENT:
		{
			if( m_nCurPos != INVALIDE_VALUE )
				DmCS::StringFn::Format( wsText, L"%d %%", ((m_nCurPos*100.0f)/(float)m_ptRange.y) );
			else
				wsText = L"? %%";
		}
		break;
	case TEXT_PERCENT_F3:
		{
			DmCS::StringFn::Format( wsText, L"%.3f %%", ((m_nCurPos*100.0f)/(float)m_ptRange.y) );
		}
		break;
	default:
		assert_cs( false );
	}
	SAFE_POINTER_RETVAL( m_pText, false );
	m_pText->SetText( wsText.c_str() );

	return true;
}

void cProgressBar::SetRenderPosText(ePOS_TEXT_TYPE type)
{
	m_ePosTextType = type;
	if( m_ePosTextType == TEXT_NONE )
	{
		SAFE_NIDELETE( m_pText );
		return;
	}

	if( !m_pText )
	{
		m_pText = NiNew cText;
		SAFE_POINTER_RET( m_pText );
	}

	cText::sTEXTINFO ti;
	ti.Init( CFont::FS_9, NiColor::WHITE );
	ti.s_eTextAlign = DT_CENTER | DT_VCENTER;
	m_pText->Init( this, CsPoint( m_ptSize.x >> 1, m_ptSize.y >> 1 ), &ti, false );
}

void cProgressBar::SetColor( NiColor color )
{
	if( m_pSpriteStart )
		m_pSpriteStart->SetColor( color );

	if( m_pSpriteCenter )
		m_pSpriteCenter->SetColor( color );

	if( m_pSpriteEnd )
		m_pSpriteEnd->SetColor( color );
}