
#include "stdafx.h"
#include "UI_ProgressBar.h"

cUI_ProgressBar::cUI_ProgressBar()
:m_DirType(LEFT_RIGHT),m_fStep(1.0f),m_fProgress(0.0f),m_fProgressRange(0.0f),m_fCurrentPos(0.0f),
m_pProgressBackImg(NULL),m_pProgressGuageImg(NULL),m_pProgressCoverImg(NULL),m_ViewTextType(INT_SLASH_INT),
m_pRenderText(NULL)
{
}

cUI_ProgressBar::~cUI_ProgressBar()
{
	Delete();
}


void cUI_ProgressBar::Delete()
{
	SAFE_NIDELETE( m_pProgressBackImg );
	SAFE_NIDELETE( m_pProgressGuageImg );
	SAFE_NIDELETE( m_pProgressCoverImg );
	SAFE_NIDELETE( m_pRenderText );
}

void cUI_ProgressBar::Init( cWindow* pParent, CsPoint pos, CsPoint tsize, bool bApplyWindowSize )
{
	cWindow::Init( pParent, pos, tsize, bApplyWindowSize );
	m_fProgressRange = tsize.x;
	m_rectProgress = CsRect(0,0,tsize.x,tsize.y);
}

void cUI_ProgressBar::SetBackImage( char* cImagePath )
{
	SAFE_NIDELETE( m_pProgressBackImg );
	m_pProgressBackImg = NiNew cSprite;
	m_pProgressBackImg->Init( this, CsPoint::ZERO, GetSize(), cImagePath, false );
}

void cUI_ProgressBar::SetGuageImage( char* cImagePath )
{
	SAFE_NIDELETE( m_pProgressGuageImg );
	m_pProgressGuageImg = NiNew cSprite;
	m_pProgressGuageImg->Init( this, CsPoint(m_rectProgress.left,m_rectProgress.top), CsPoint(m_rectProgress.Width(),m_rectProgress.Height()), cImagePath, false );
	UpdateProgressBar();
}

void cUI_ProgressBar::SetCoverImage( char* cImagePath )
{
	m_pProgressCoverImg = NiNew cSprite;
	m_pProgressCoverImg->Init( this, CsPoint::ZERO, GetSize(), cImagePath, false );
}

void cUI_ProgressBar::SetStringControl( cText::sTEXTINFO* ti )
{
	m_TextInfo = *ti;	
}

void cUI_ProgressBar::SetProgressRect( CsRect const& rect )
{
	m_rectProgress = rect;

	if( m_pProgressGuageImg )
	{
		m_pProgressGuageImg->SetSize( CsPoint(m_rectProgress.Width(),m_rectProgress.Height()) );
		m_pProgressGuageImg->SetPos( CsPoint(m_rectProgress.left,m_rectProgress.top) );
		UpdateProgressBar();
	}
}

void cUI_ProgressBar::SetDirType(cUI_ProgressBar::eDIR_TYPE eDirType)
{
	m_DirType = eDirType;
}

void cUI_ProgressBar::ResetDevice()
{
	cWindow::ResetDevice();

	if( m_pProgressBackImg )
		m_pProgressBackImg->ResetDevice();

	if( m_pProgressGuageImg )
		m_pProgressGuageImg->ResetDevice();

	if( m_pProgressCoverImg )
		m_pProgressCoverImg->ResetDevice();

	if( m_pRenderText )
		m_pRenderText->ResetDevice();
}

void cUI_ProgressBar::Step()
{
	SetProgressPos( m_fCurrentPos + m_fStep );
}

void cUI_ProgressBar::AdjustProgress(float delta)
{
	SetProgressPos(m_fCurrentPos + delta);
}

float cUI_ProgressBar::GetCurrentPosition() const
{
	return m_fCurrentPos;
}

void cUI_ProgressBar::SetStepSize(float fStepSize)
{
	m_fStep = fStepSize;
}

void cUI_ProgressBar::SetRange( float fRange )
{
	m_fProgressRange = __max( 0.0f, fRange);
	UpdateProgressBar();
	UpdateShowString();
}

void cUI_ProgressBar::SetString( std::wstring const& str )
{
	m_ProgressString = str;
}

void cUI_ProgressBar::SetProgressPos( float fPos )
{
	fPos = (fPos < 0.0f) ? 0.0f : (fPos > m_fProgressRange) ? m_fProgressRange : fPos;
	if( fPos == m_fCurrentPos )
		return;

	m_fCurrentPos = fPos;

	OnEvent( eProgressbar_Change );

	UpdateShowString();

	UpdateProgressBar();

	if( m_DirType == RIGHT_LEFT || m_DirType == BOTTOM_TOP )
	{
		if( 0.0f == m_fCurrentPos )
			OnEvent( eProgressbar_Done );
	}
	else
	{
		if( m_fProgressRange == m_fCurrentPos )
			OnEvent( eProgressbar_Done );
	}
}

void cUI_ProgressBar::SetViewTextType( eViewTextType eType )
{
	m_ViewTextType = eType;
	m_ProgressString.clear();
}

bool cUI_ProgressBar::IsMaxGuage() const
{
	switch( m_DirType )
	{
	case LEFT_RIGHT:	return m_fCurrentPos == m_fProgressRange;
	case BOTTOM_TOP: case RIGHT_LEFT:	return m_fCurrentPos == 0.0f;
	default:
		return false;
	}	
}

float cUI_ProgressBar::GetCurrentVale() const
{
	return m_fCurrentPos;
}

float cUI_ProgressBar::GetRemainVale() const
{
	return m_fProgressRange - m_fCurrentPos;
}

void cUI_ProgressBar::Update(float fDeltaTime)
{
}


void cUI_ProgressBar::SetRenderText( bool bValue )
{
	SAFE_NIDELETE( m_pRenderText );
	if( !bValue )
		return;

	CsPoint csSize = GetSize();
	CsPoint csPos = CsPoint::ZERO;

	if( m_TextInfo.s_eTextAlign & DT_CENTER )
		csPos.x = csSize.x >> 1;
	else if( m_TextInfo.s_eTextAlign & DT_RIGHT )
		csPos.x = csSize.x;

	if( m_TextInfo.s_eTextAlign & DT_VCENTER )
		csPos.y = csSize.y >> 1;
	else if( m_TextInfo.s_eTextAlign & DT_BOTTOM )
		csPos.y = csSize.y;

	//csPos.y = (int)( ( csSize.y - m_TextInfo.s_eFontSize*1.4f )/2.0f );

	m_pRenderText = NiNew cText;
	m_pRenderText->Init( this, csPos, &m_TextInfo, false );
}

//----------------------------------------------------------------------------------------------------------
void cUI_ProgressBar::UpdateProgressBar()
{
	SAFE_POINTER_RET( m_pProgressGuageImg );
	
	CsPoint TextSize = m_pProgressGuageImg->GetSize();

	if( BOTTOM_TOP == m_DirType )
	{
		TextSize.y = (m_rectProgress.Height() * m_fCurrentPos) / m_fProgressRange;
		m_pProgressGuageImg->SetPos( CsPoint( m_rectProgress.left, m_rectProgress.bottom - TextSize.y ) );
	}
	else
		TextSize.x = (m_rectProgress.Width() * m_fCurrentPos) / m_fProgressRange;
	m_pProgressGuageImg->SetSize( TextSize );
}

void cUI_ProgressBar::SetPos(CsPoint ptPos)
{
	cWindow::SetPos( ptPos );

	if( m_pProgressBackImg )
		m_pProgressBackImg->ResetDevice();

	if( m_pProgressGuageImg )
		m_pProgressGuageImg->ResetDevice();

	if( m_pProgressCoverImg )
		m_pProgressCoverImg->ResetDevice();

	if( m_pRenderText )
		m_pRenderText->ResetDevice();
}

void cUI_ProgressBar::UpdateShowString()
{
	m_ProgressString.clear();
	switch( m_ViewTextType )
	{
	case NONE:
		break;
	case INT_ONLY_CURRENT_VALUE:
		DmCS::StringFn::Format( m_ProgressString, L"%%I64d", (__int64)m_fCurrentPos);
		break;
	case INT_SLASH_INT: //" %d / %d "
		DmCS::StringFn::Format( m_ProgressString, L"%I64d / %I64d", (__int64)m_fCurrentPos , (__int64)m_fProgressRange );
		break;
	case INT_PERSENT: //" %d %% "
		{
			float fPersent = (m_fCurrentPos * 100.0f) / m_fProgressRange;
			DmCS::StringFn::Format( m_ProgressString, L"%d %%", (int)fPersent);
		}
		break;
	case FLOAT_SLASH_FLOT: //" %.02f / %.02f "
		DmCS::StringFn::Format( m_ProgressString, L"%.02f / %.02f", m_fCurrentPos , m_fProgressRange );
		break;
	case INT_PERSENT_SLASH_INT_PERSENT: //" %d %% / %d %%"
		{
			float fPersent = (m_fCurrentPos * 100.0f) / m_fProgressRange;
			DmCS::StringFn::Format( m_ProgressString, L"%d %% / %d %%", (int)fPersent, 100 );
		}
		break;
	}

	if( m_pRenderText )
		m_pRenderText->SetText( m_ProgressString.c_str() );
}

#define ARGB_AT_DWORD(a,r,g,b) ((a<<24)|(r<<16)|(g<<8)|(b))

void cUI_ProgressBar::Render()
{
	if( !m_bVisible )
		return;

	if( m_pProgressBackImg )
		m_pProgressBackImg->Render();

	if( m_pProgressGuageImg )
		m_pProgressGuageImg->Render();

	if( m_pRenderText )
		m_pRenderText->Render();

	if( m_pProgressCoverImg )
		m_pProgressCoverImg->Render();

}