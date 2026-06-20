

#include "stdafx.h"
#include "ComboBox.h"

#define COMBO_STRINGLIST_DELTA_Y		2
cComboBox::cComboBox(): m_pPopupWindow(NULL),m_pSelMask(NULL),m_pCurSel(NULL),
m_bRenderPopup(false), m_bRenderSelMask(false),m_ptPopupPos(0, 0), m_ptSelMaskPos(0,0),
m_eFontAlign(0), m_nCurSelIndex(0), m_bAutoSelect(false), m_bStaticCenter(false)
{
}

cComboBox::~cComboBox()
{
	Delete(); 
}

void cComboBox::Delete()
{
	cButton::Delete();

	SAFE_NIDELETE( m_pPopupWindow );
	SAFE_NIDELETE( m_pSelMask );

	m_StringList.Delete();
	m_pCurSel = NULL;
}

void cComboBox::Init( cWindow* pParent, eComboType eType, CsPoint pos, CsPoint size, CFont::eFACE_SIZE eFontSize, int eFontAlign, bool bClear, bool bAutoSelect, bool bApplyWindowSize, bool bStaticCenter /*=false*/ )
{
	switch( eType )
	{
	case CT_NORMAL:
		cButton::Init( pParent, pos, size, "Control_G\\ComboBox\\Combo.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 26 ) );
		break;
	case CT_NORMAL2:
		cButton::Init( pParent, pos, size, "Control_G\\ComboBox\\Combo2.tga", bApplyWindowSize );
		SetTexToken( CsPoint( 0, 26 ) );
		break;
	default:
		assert_cs( false );
		break;
	}	

	m_ptPopupPos = CsPoint( 0,size.y );
	m_pPopupWindow = NiNew cPopUp;
	if( bClear == true )
		m_pPopupWindow->Init( pParent, pos + m_ptPopupPos, CsPoint( size.x, 10 ), CsPoint( 3, 3 ), "Control_G\\Popup\\NDlg", "tga", false );
	else	
		m_pPopupWindow->Init( pParent, pos + m_ptPopupPos, CsPoint( size.x, 10 ), CsPoint( 16, 16 ), "Control_G\\Popup\\ON2Dlg", "tga", false );					

	m_pSelMask = NiNew cSprite;
	switch( eType )
	{
		case CT_NORMAL:
		case CT_NORMAL2:
			m_pSelMask->Init( NULL, CsPoint::ZERO, CsPoint( size.x, g_pEngine->m_FontSystem.GetHeight( eFontSize )+2 ), "Talk\\Common_Seletbar.tga", false );
			break;
	}
	

	m_bRenderPopup = false;
	m_bRenderSelMask = false;

	m_eFontSize = eFontSize;
	m_eFontAlign = eFontAlign;
	m_nCurSelIndex = CI_INVALIDE;
	m_pCurSel = NULL;
	m_bAutoSelect = bAutoSelect; //콤보 갯수가 2개일때 자동바꾸기 사용할것인가?
	m_bStaticCenter = bStaticCenter;

	m_ptDeltaPos.y = ( size.y - g_pEngine->m_FontSystem.GetHeight( eFontSize ) ) / 2;
}

void cComboBox::ChangeComboBoxImg( char const* pPopUpFile, CsPoint tokenSize )
{
	ChangeTexture( pPopUpFile );
	SetTexToken( tokenSize );
}

void cComboBox::SetComboListWindow( char const* pPopUpFile )
{
	SAFE_NIDELETE( m_pPopupWindow );
	m_pPopupWindow = NiNew cPopUp;
	m_pPopupWindow->Init( NULL, CsPoint(0, m_ptSize.y), CsPoint( m_ptSize.x, 10 ), CsPoint( 3, 3 ), const_cast<char*>(pPopUpFile), "tga", false );
}

void cComboBox::Update(float const& fDeltaTime)
{

}

int cComboBox::Update_ForMouse()
{
	if( !m_bVisible )
		return cComboBox::CI_INVALIDE;

	switch( cButton::Update_ForMouse() )
	{
	case cButton::ACTION_CLICK:
		{
			if( m_bAutoSelect && ( GetCount() == 2 ) )
			{
				if( m_nCurSelIndex == 0 )
				{
					SetCurSel( 1 );
					return m_StringList.GetString( 1 )->GetValue1();
				}
				else
				{
					SetCurSel( 0 );
					return m_StringList.GetString( 0 )->GetValue1();
				}
			}
			else
			{
				m_bRenderPopup = !m_bRenderPopup;
			}
		}		
		return cComboBox::CI_CLICK;
	case cButton::ACTION_NONE:
		break;
	default:
		return cComboBox::CI_INRECT;
	}

	if( m_bRenderPopup == true )
	{
		CsPoint ptLocal = MousePosToWindowPos( CURSOR_ST.GetPos() );
		CsRect rc( m_ptPopupPos, m_ptPopupPos + m_pPopupWindow->GetSize() );

		if( rc.PtInRect( ptLocal ) == FALSE )
		{
			switch( CURSOR_ST.GetButtonState() )
			{
			case CURSOR::LBUTTON_UP:
			case CURSOR::RBUTTON_UP:
				{
					m_bRenderPopup = false;
					return cComboBox::CI_INVALIDE;
				}
				
			}
		}

		m_bRenderSelMask = false;
		int nActive1, nActive2;
		m_ptSelMaskPos = GetClient() + CsPoint( 0, 10 + GetSize().y - 1 );
		switch( m_StringList.Update_ForMouse( nActive1, nActive2, m_ptSelMaskPos, COMBO_STRINGLIST_DELTA_Y, CsSIZE( m_pPopupWindow->GetSize().x, 18 ) ) )
		{
		case CURSOR::LBUTTON_UP:
			{
				m_bRenderPopup = false;

				assert_cs( nActive1 != cString::INVALIDE_STRING_VALUE );
				if( SetCurSel( nActive2 ) == true )
				{					
					return nActive1;
				}
				return cComboBox::CI_SAME_CLICK;
			}
			break;
		}
		if( nActive1 != cString::INVALIDE_STRING_VALUE )
			m_bRenderSelMask = true;

		if( rc.PtInRect( ptLocal ) == TRUE )
			return cComboBox::CI_INRECT;
	}
	return cComboBox::CI_INVALIDE;
}

void cComboBox::Render()
{
	if( !m_bVisible )
		return;

	cButton::Render();

	if( m_pCurSel != NULL )
	{
		CsPoint pos = GetClient();
		switch( m_eFontAlign )
		{
		case DT_LEFT:
			break;
		case DT_CENTER:
			pos.x += GetSize().x / 2;
			break;
		case DT_RIGHT:
			pos.x += GetSize().x;
			break;
		}
		m_pCurSel->Render( pos + m_ptDeltaPos, m_eFontAlign );
	}

	if( m_bRenderPopup == true )
	{
		m_pPopupWindow->Render( GetClient() );

		if( m_bRenderSelMask )
			m_pSelMask->Render( m_ptSelMaskPos );

		switch( m_eFontAlign )
		{
		case DT_LEFT:
			m_StringList.Render( GetClient() + CsPoint( 10, 10 + GetSize().y ), COMBO_STRINGLIST_DELTA_Y, m_eFontAlign );					
			break;
		case DT_CENTER:
			m_StringList.Render( GetClient() + CsPoint( GetSize().x/2, 10 + GetSize().y ), COMBO_STRINGLIST_DELTA_Y, m_eFontAlign );
			break;
		case DT_RIGHT:
			m_StringList.Render( GetClient() + CsPoint( GetSize().x - 10, 10 + GetSize().y ), COMBO_STRINGLIST_DELTA_Y, m_eFontAlign );
			break;
		}
	}
}
void cComboBox::Render( CsPoint pos )
{
	SetPos( pos );
	Render(); 
}

cWindow::eTYPE cComboBox::GetType()
{
	return T_ComboBox; 
}

void cComboBox::SetAlpha( float fAlpha )
{
	cButton::SetAlpha( fAlpha );

	m_StringList.SetAlpha( fAlpha );
	m_pSelMask->SetAlpha( fAlpha );
	m_pPopupWindow->SetAlpha( fAlpha );
}

void cComboBox::ResetDevice()
{
	cButton::ResetDevice();

	m_StringList.ResetDevice();
	m_pSelMask->ResetDevice();
	m_pPopupWindow->ResetDevice();
}

void	cComboBox::RemoveAllItem(void)
{
	m_StringList.Delete();
	m_nCurSelIndex = -1;
	m_pCurSel = NULL;
}


void cComboBox::AddItem( TCHAR const* szItem, int nData /*=0*/, NiColor color /*=FONT_WHITE*/ )
{
	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, m_eFontSize, color );
	//ti.s_eTextAlign = m_eFontAlign;
	pString = NiNew cString;
	ti.SetText( szItem );
	pString->AddText( &ti );
	pString->SetValue1( nData );
	pString->SetValue2( m_StringList.GetSize() );
	m_StringList.AddTail( pString );

	CsPoint ptOrgSize = m_StringList.CalMaxSize( COMBO_STRINGLIST_DELTA_Y );
	CsPoint ptMaxSize = ptOrgSize + CsPoint( 20, 20 );
	ptMaxSize.x = CsMax( GetSize().x, ptMaxSize.x );
	m_pPopupWindow->ReSize( ptMaxSize );

	m_pSelMask->SetSize( CsPoint( ptMaxSize.x, g_pEngine->m_FontSystem.GetHeight( m_eFontSize )+2 ) );

	if( m_bStaticCenter == true )
	{
		m_StringList.SetSameMaxWidth( ptOrgSize.x );
	}
}
void cComboBox::AddItem( std::wstring* szItem, int nData /*= 0*/, NiColor color /*= FONT_WHITE*/ )
{
	cString* pString;
	cText::sTEXTINFO ti;
	ti.Init( &g_pEngine->m_FontSystem, m_eFontSize, color );
	//ti.s_eTextAlign = m_eFontAlign;
	pString = NiNew cString;
	ti.SetText( (TCHAR*)szItem->c_str() );
	pString->AddText( &ti );
	pString->SetValue1( nData );
	pString->SetValue2( m_StringList.GetSize() );
	m_StringList.AddTail( pString );

	CsPoint ptOrgSize = m_StringList.CalMaxSize( COMBO_STRINGLIST_DELTA_Y );
	CsPoint ptMaxSize = ptOrgSize + CsPoint( 20, 20 );
	ptMaxSize.x = CsMax( GetSize().x, ptMaxSize.x );
	m_pPopupWindow->ReSize( ptMaxSize );

	m_pSelMask->SetSize( CsPoint( ptMaxSize.x, g_pEngine->m_FontSystem.GetHeight( m_eFontSize )+2 ) );

	if( m_bStaticCenter == true )
	{
		m_StringList.SetSameMaxWidth( ptOrgSize.x );
	}
}
void cComboBox::AddItem( cString* pString, int nData /*=0*/, NiColor color /*=FONT_WHITE*/ )
{	
	assert_cs( pString != NULL );
	pString->SetValue1( nData );
	pString->SetValue2( m_StringList.GetSize() );
	m_StringList.AddTail( pString );

	CsPoint ptOrgSize = m_StringList.CalMaxSize( COMBO_STRINGLIST_DELTA_Y );
	CsPoint ptMaxSize = ptOrgSize + CsPoint( 20, 20 );
	ptMaxSize.x = CsMax( GetSize().x, ptMaxSize.x );
	m_pPopupWindow->ReSize( ptMaxSize );

	m_pSelMask->SetSize( CsPoint( ptMaxSize.x, g_pEngine->m_FontSystem.GetHeight( m_eFontSize )+2 ) );

	if( m_bStaticCenter == true )
	{
		m_StringList.SetSameMaxWidth( ptOrgSize.x );
	}
}

bool cComboBox::SetCurSel( int nSel, bool bEvent )
{
	if( nSel < 0 )							return false;
	if( nSel >= m_StringList.GetSize() )	return false;
	if( nSel == m_nCurSelIndex )			return false;

	m_nCurSelIndex = nSel;
	m_pCurSel = m_StringList.GetString( nSel );

	if( bEvent )
		OnEvent( COMBO_SELECTITEM, &m_nCurSelIndex );
	return true;
}

bool cComboBox::SetCurSel_FromData( int nData )
{
	int nIndex = 0;
	std::list< cString* >* pList = m_StringList.GetList();
	std::list< cString* >::iterator it = pList->begin();
	std::list< cString* >::iterator itEnd = pList->end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetValue1() == nData )
		{
			return SetCurSel( nIndex );
		}
		++nIndex;
	}
	m_pCurSel = NULL;
	return false;
}

int	cComboBox::GetCurSel_Data()
{
	if( m_pCurSel )
		return m_pCurSel->GetValue1();
	return -1;
}

int	cComboBox::GetCurSel_Idx() 
{
	return m_nCurSelIndex; 
}

int cComboBox::GetCount()
{
	return m_StringList.GetSize(); 
}

int cComboBox::GetData( int nSel )
{
	if( nSel >= m_StringList.GetSize() )
		nSel = m_StringList.GetSize() - 1;
	if( nSel < 0 )
		nSel = 0;
	return m_StringList.GetString( nSel )->GetValue1();
}

bool cComboBox::IsRenderPopup()
{
	return m_bRenderPopup; 
}

bool cComboBox::IsSelect()
{
	return ( m_pCurSel != NULL ) ? true : false; 
}
 
void cComboBox::SetVisibleRenderPopup(bool bValue)
{
	m_bRenderPopup = bValue;
}

cString* cComboBox::GetSelectedItem()
{
	return m_pCurSel;
}

void cComboBox::SetDeltaPos(CsPoint ptDeltaPos)
{
	if( DT_RIGHT == m_eFontAlign )
		ptDeltaPos.x *= -1;
	m_ptDeltaPos += ptDeltaPos;
}
