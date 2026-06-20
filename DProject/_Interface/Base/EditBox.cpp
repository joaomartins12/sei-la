
#include "stdafx.h"
#include "EditBox.h"
#include <wchar.h>
#include <string.h>

cEditBox*		g_pFocusEdit = NULL;

void cEditBox::Delete()
{ 
	m_Text.Delete(); 
	m_EmptyText.Delete(); 
	NISAFE_DELETE(m_pTextUnderline);
}	
void cEditBox::SetAlpha( float fAlpha )
{ 
	m_Text.SetAlpha( fAlpha ); 
	m_EmptyText.SetAlpha( fAlpha ); 
	if( m_pTextUnderline )
		m_pTextUnderline->SetAlpha( fAlpha ); 
}

void cEditBox::UpdateFocusEdit()
{
	if( g_pFocusEdit == NULL )
		return;

#if ( defined VERSION_TW || defined VERSION_HK )
	std::wstring str;
	TCHAR cmp[100];		// 중문 구글 IME 입력법에서 한번에 10자 이상 버퍼에 저장하여 Error 발생으로 버퍼 사이즈 변경  lks007
#else
	std::wstring str;
	TCHAR cmp[10];
#endif
	
	g_IME.GetInput( &str );
#ifdef TAIWAN_DAYI_IME
	if( !g_IME.DaYiComparison() )
	{
		g_IME.GetComp(cmp);
		str.append( cmp );
	}
#else
	g_IME.GetComp(cmp);
	str.append( cmp );
#endif
	
	if( g_pFocusEdit->SetText( &str[ g_pFocusEdit->GetTempInputIndex() ], true ) == true )
	{
		if( g_pFocusEdit->IsEnableSound() == true )
		{
			cWindow::PlaySound( cWindow::SD_At2 );
		}		

		//========================================================================
		// 에디트 박스 밀리는 처리
		//========================================================================		
		while( g_pFocusEdit->CalTempInputIndex() )
		{
			g_pFocusEdit->SetText( &str[ g_pFocusEdit->GetTempInputIndex() ], true );
		}					
		g_pFocusEdit->SetText( &str[ g_pFocusEdit->GetTempInputIndex() ], true );				
	}	
	g_pFocusEdit->SetTextAll( str.c_str() );

}

bool cEditBox::SetText( TCHAR const* szText, bool bCallFocus )
{ 
	if( ( bCallFocus == false )&&( IsFocus() == true ) )
		g_IME.SetEdit( true, m_nFontLength, szText ); 

	bool bResult = m_Text.SetText( szText ); 
	if( bResult ) // 입력된 문자가 바꼈을 때
		OnEvent(eEditbox_ChangeText, const_cast<wchar_t*>(m_Text.GetText()) );

	SetTextAll( szText );
	return bResult;
}

void cEditBox::SetEmptyMsgText( TCHAR const* szText, NiColor niColor )
{
	m_EmptyText.SetText( szText );
	m_EmptyText.SetColor( niColor );
}

bool cEditBox::SetText( int nText )
{ 
	TCHAR sz[ 16 ] = {0,}; 
	_stprintf_s( sz, 16, _T( "%d" ), nText ); 
	return SetText( sz ); 
}

void cEditBox::SetTextAll( TCHAR const* szText ) 
{ 
	m_strText = szText; 
} 

void cEditBox::SetFocus()
{ 
	g_pFocusEdit = this; 
	g_IME.SetEdit( true, m_nFontLength, m_Text.GetTextInfo()->GetText() ); 
	m_fCursorTime = 0.0f; 
	OnEvent( eEditbox_SetFocus );
}

void cEditBox::OnPaste()
{
	if( g_pFocusEdit->IsPaste() == false )
		return;

	int nLength = g_pFocusEdit->GetFontLength();	

	TCHAR szMessage[ 1024 ] = { 0, };
#ifdef THAI_INPUT_PASTE
	TCHAR sz_Unicode[ 1024 ] = { 0, };
#else
	char sz[ 1024 ] = { 0, };
#endif

	if( OpenClipboard(0) )
	{
#ifdef THAI_INPUT_PASTE	 // 태국어 문자 붙여넣기 시 기존 input이 ANSI형식에서 인식이 되지 않아 unicode 형식 추가함	lks007	12.08.20
		if( ::IsClipboardFormatAvailable( CF_UNICODETEXT ) == TRUE )
		{
			HGLOBAL hGlobal = GetClipboardData(CF_UNICODETEXT); 
			if(hGlobal) 
			{ 
				TCHAR* pTemp = (TCHAR*)GlobalLock(hGlobal);
				_tcsncpy_s( sz_Unicode, pTemp, nLength - 1 );		
				GlobalUnlock(hGlobal); 

				int nLen = wcslen( sz_Unicode );
				for( int i = 0; i < nLen; i++ )
				{
					if( ( sz_Unicode[ i ] == (char)10 )
						|| ( sz_Unicode[ i ] == (char)13 ) )
					{
						sz_Unicode[ i ] = 0;
					}
				}

				_tcsncpy_s( szMessage, (TCHAR*)g_pFocusEdit->GetTextAll(), nLength - 1 );
				_tcsncat_s( szMessage, sz_Unicode, nLength - _tcslen(szMessage) - 1 );				
				g_IME.SetEdit( true, nLength, szMessage );
				g_pFocusEdit->UpdateFocusEdit();
			} 
		}
		CloseClipboard();
#else
		if( ::IsClipboardFormatAvailable( CF_TEXT ) == TRUE )
		{
			HGLOBAL hGlobal = GetClipboardData(CF_TEXT); 
			if(hGlobal) 
			{ 
				char* pGlobal = (char*)GlobalLock(hGlobal); 
				strncpy( sz, pGlobal, nLength - 1 );				
				GlobalUnlock(hGlobal); 

				int nLen = strlen( sz );
				for( int i = 0; i < nLen; i++ )
				{
					if( ( sz[ i ] == (char)10 )
						|| ( sz[ i ] == (char)13 ) )
					{
						sz[ i ] = 0;
					}
				}

				_tcsncpy_s( szMessage, (TCHAR*)g_pFocusEdit->GetTextAll(), nLength - 1 );
				_tcsncat_s( szMessage, _countof(szMessage), nBase::m2w(sz).c_str(), nLength - _tcslen(szMessage) - 1);
				g_IME.SetEdit( true, nLength, szMessage );
				g_pFocusEdit->UpdateFocusEdit();
			} 
		}
		CloseClipboard();
#endif
	}
}

void cEditBox::Init( cWindow* pParent, CsPoint pos, CsPoint size, cText::sTEXTINFO* pTextInfo, bool bApplyWindowSize )
{
	cWindow::Init( pParent, pos, size, bApplyWindowSize );

	m_Text.Init( this, CsPoint::ZERO, pTextInfo, bApplyWindowSize );

	cText::sTEXTINFO emptyText = *pTextInfo;
	emptyText.SetText(L"");
	emptyText.s_Color = NiColor(0.5f,0.5f,0.5f);
	m_EmptyText.Init( this, CsPoint::ZERO, &emptyText, bApplyWindowSize );

	m_fCursorTime = 0.0f;
	m_nTempInputIndex = 0;			
}

int cEditBox::Update_ForMouse( CsPoint ptParent )
{
	if( !m_bVisible || !m_bEnable )
		return ACTION_NONE;

	int nType = CURSOR_ST.CheckClickBox( m_ptClient.x + ptParent.x, m_ptClient.y + ptParent.y, m_ptSize.x, m_ptSize.y );

	switch( nType )
	{
	case CURSOR::LBUTTON_DOWN:
		{
			if( g_pFocusEdit != this )
				SetFocus();
		}
		return ACTION_CLICK;
	case CURSOR::BUTTON_OUTWINDOW:
		{
			switch( CURSOR_ST.GetButtonState() )
			{
			case CURSOR::LBUTTON_DOWN:
			case CURSOR::RBUTTON_DOWN:
				ReleaseFocus(false);
				break;
			}
		}
		break;
	}

	return ACTION_NONE;
}

void cEditBox::Render()
{
	if( !m_bVisible )
		return;

	CsPoint	StartPos = CsPoint::ZERO;
	UINT	align = m_Text.GetTextInfo()->s_eTextAlign;
	switch(align)
	{
	case DT_LEFT:
		StartPos = CsPoint( 0, 0 );
		break;
	case DT_RIGHT:
		StartPos = CsPoint( m_ptSize.x, 0 );
		break;
	case DT_CENTER:
		StartPos = CsPoint( (int)(m_ptSize.x*0.5f), m_ptSize.y*0.5f-m_ptSize.y*0.25f );
		break;
	}

	if( m_Text.GetTextLen() != 0 )
		m_Text.Render( StartPos, align );
	else if( m_EmptyText.GetTextLen() != 0 )
		m_EmptyText.Render( StartPos, align );

	RenderUnderLine( StartPos );
}


void cEditBox::Render( CsPoint pos )
{
	if( !m_bVisible )
		return;

	switch( m_Text.GetTextInfo()->s_eTextAlign )
	{
	case DT_LEFT:
		{
			if( m_Text.GetTextLen() != 0 )
				m_Text.Render( pos + CsPoint( 0, 2 ), DT_LEFT );
			else if( m_EmptyText.GetTextLen() != 0 )
				m_EmptyText.Render( pos + CsPoint( 0, 2 ), DT_LEFT );
		}
		break;
	case DT_RIGHT:
		{
			if( m_Text.GetTextLen() != 0 )
				m_Text.Render( pos + CsPoint( m_ptSize.x, 2 ), DT_RIGHT );
			else if( m_EmptyText.GetTextLen() != 0 )
				m_EmptyText.Render( pos + CsPoint( m_ptSize.x, 2 ), DT_RIGHT );
		}
		break;
	case DT_CENTER:
		{
			if( m_Text.GetTextLen() != 0 )
				m_Text.Render( pos + CsPoint( (int)(m_ptSize.x*0.5f), 2 ), DT_CENTER );
			else if( m_EmptyText.GetTextLen() != 0 )
				m_EmptyText.Render( pos + CsPoint( (int)(m_ptSize.x*0.5f), 2 ), DT_CENTER );
		}
		break;
	}	

	RenderUnderLine( pos );
}

void cEditBox::RenderUnderLine( CsPoint pos )
{
	if( !m_bVisible )
		return;

	// 언더 라인
	if( g_pFocusEdit == this )
	{
		if( m_pTextUnderline != NULL )
		{
			m_fCursorTime += g_fDeltaTime;
			if( m_fCursorTime > 1.0f )
			{
				m_fCursorTime = 0;
			}

			// 커서 깜박임
			if( m_fCursorTime < 0.5f )
			{
				CsPoint	Addpos = m_Text.GetStringSize();
				UINT	align = m_Text.GetTextInfo()->s_eTextAlign;
				switch(align)
				{
				case DT_LEFT:
					Addpos = CsPoint(Addpos.x-2, 2);
					break;
				case DT_RIGHT:
					Addpos = CsPoint(-2, 0); // [4/21/2016 hyun] 수정
					break;
				case DT_CENTER:
					Addpos = CsPoint(Addpos.x/2-2, 0);
					break;
				}

				m_pTextUnderline->Render( pos + Addpos );
			}
		}
	}
}

void cEditBox::AddText( TCHAR const* szText )
{
	m_Text.GetTextInfo()->AddText( szText );
}

void cEditBox::EnableUnderline( NiColor color /* = FONT_WHITE */ )
{
	assert_cs( m_pTextUnderline == NULL );

	cText::sTEXTINFO ti;	
	ti.Init( m_Text.GetTextInfo()->s_pFont, m_Text.GetTextInfo()->s_eFontSize );
	ti.SetText( _T( "|" ) );
	ti.s_bOutLine = false;
	ti.s_Color = color;

	m_pTextUnderline = NiNew cText;
	m_pTextUnderline->Init( this, CsPoint::ZERO, &ti, m_bApplyWindowSize );
}

void cEditBox::ReleaseFocus( bool bClearText /*=true*/ )
{
	if( g_pFocusEdit != this )
		return;

	g_IME.SetEdit( false );
	if( bClearText == true )
	{
		m_nTempInputIndex = 0;
		m_Text.SetText( _T( "" ) );
	}		
	g_pFocusEdit = NULL;
	OnEvent( eEditbox_ReleaseFocus );	
}

//=======================================================================================
//
//		Geti
//
//=======================================================================================

bool cEditBox::CalTempInputIndex()
{
	int nTemp = m_Text.GetStringSize().x - 1;
	// 에디트창 넘어갈경우의 처리

#ifdef THAI_PASSWORDLENGTH
	if( nTemp > (int)( m_ptSize.x*0.91f ) )
#else
	if( nTemp > (int)( m_ptSize.x*0.85f ) )
#endif
	{
		if( ( m_nTempInputIndex += CsMax( 1, m_nFontLength/32 ) ) > (m_nFontLength-1) )
			m_nTempInputIndex = m_nFontLength - 1;

		return true;
	}
	else if( nTemp < (int)( m_ptSize.x*0.6f ) )
	{
		if( ( m_nTempInputIndex -= CsMax( 1, m_nFontLength/22 ) ) < 0 )
		{
			m_nTempInputIndex = 0;
			return false;
		}
		return true;
	}
	return false;
}

//=======================================================================================
//
//		ResetDevice
//
//=======================================================================================

void cEditBox::ResetDevice()
{
	cWindow::ResetDevice();
	m_Text.ResetDevice();
	m_EmptyText.ResetDevice();
	if( m_pTextUnderline )
		m_pTextUnderline->ResetDevice();
}

TCHAR const* cEditBox::GetString()
{	
	return m_Text.GetText();
}

void cEditBox::SetLimitNumber( int Num )
{
	m_nLimitNumber = Num;
}

void cEditBox::SetFontColor(NiColor color)
{
	m_Text.SetColor(color);
}

void cEditBox::SetVisible( bool bValue )
{
	cWindow::SetVisible( bValue );
	if( !bValue )
		ReleaseFocus(true);
}

void cEditBox::SetEnable( bool bValue )
{
	m_bEnable = bValue;
}
