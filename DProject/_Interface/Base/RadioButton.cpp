
#include "stdafx.h"
#include "RadioButton.h"

void cRadioButton::Delete()
{
	// 자기 자신을 삭제 하면 안되기 때문에 1부터
	int nSize = m_vpRadioNode.Size();
	for( int i=1; i<nSize; ++i )
	{
		NiDelete m_vpRadioNode[ i ];
	}
	m_vpRadioNode.Destroy();
}

void cRadioButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, CsPoint tex, char* cImagePath, bool bApplyWindowSize )
{
	cCheckBox::Init( pParent, pos, size, cImagePath, bApplyWindowSize );
	SetTexToken( tex );
	m_vpRadioNode.PushBack( this );

	m_nCheckIndex = INVALIDE_RADIO_CLICK;
}

void cRadioButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, cText::sTEXTINFO* ti, bool bApplyWindowSize )
{	
	cCheckBox::Init( pParent, pos, size, it, ti, bApplyWindowSize );
	m_vpRadioNode.PushBack( this );

	m_nCheckIndex = INVALIDE_RADIO_CLICK;
}

void cRadioButton::Init( cWindow* pParent, CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, TCHAR const* cs, bool bApplyWindowSize )
{
	cCheckBox::Init( pParent, pos, size, it, cs, bApplyWindowSize );
	m_vpRadioNode.PushBack( this );

	m_nCheckIndex = INVALIDE_RADIO_CLICK;
}

cRadioButton* cRadioButton::AddNode( CsPoint pos, CsPoint size, CsPoint tex, char* cImagePath )
{
	assert_cs( m_vpRadioNode.Size() > 0 );

	cRadioButton* pRadio = NiNew cRadioButton;
	pRadio->Init( m_pParentWindow, pos, size, tex, cImagePath, m_bApplyWindowSize );

	m_vpRadioNode.PushBack( pRadio );
	return pRadio;
}

cRadioButton* cRadioButton::AddNode( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, TCHAR const* cs )
{
	assert_cs( m_vpRadioNode.Size() > 0 );

	cRadioButton* pRadio = NiNew cRadioButton;
	pRadio->Init( m_pParentWindow, pos, size, it, cs, m_bApplyWindowSize );

	m_vpRadioNode.PushBack( pRadio );
	return pRadio;
}

cRadioButton* cRadioButton::AddNode( CsPoint pos, CsPoint size, cCheckBox::eIMAGE_TYPE it, cText::sTEXTINFO* ti )
{
	assert_cs( m_vpRadioNode.Size() > 0 );
	
	cRadioButton* pRadio = NiNew cRadioButton;
	pRadio->Init( m_pParentWindow, pos, size, it, ti, m_bApplyWindowSize );

	m_vpRadioNode.PushBack( pRadio );
	return pRadio;
}

int cRadioButton::Update_ForMouse()
{
	if( !m_bVisible )
		return INVALIDE_RADIO_CLICK;

	assert_cs( m_vpRadioNode.Size() > 0 );

	int nSize = m_vpRadioNode.Size();
	bool bPress = false;
	for( int i=0; i<nSize; ++i )
	{
		switch( m_vpRadioNode[ i ]->cCheckBox::Update_ForMouse() )
		{
		case cCheckBox::ACTION_CLICK:
			{
				for( int n=0; n<nSize; ++n )
				{
					if( m_vpRadioNode[ n ]->IsEnable() )
						m_vpRadioNode[ n ]->SetCheck( i == n );							
				}
				if( m_nCheckIndex == i )
					return ANI_RADIO_PRESS;

				if( m_nCheckIndex != INVALIDE_RADIO_CLICK )
				{
					m_vpRadioNode[ m_nCheckIndex ]->SetTextColor( FONT_WHITE * 0.85f );
				}
				m_vpRadioNode[ i ]->SetTextColor( FONT_WHITE );				
				m_nCheckIndex = i;
				OnEvent( RadioButton_LBUP_EVENT, m_vpRadioNode[ i ] );
				return cCheckBox::ACTION_CLICK;
			}			
		case cCheckBox::ACTION_PRESS:
		case cCheckBox::ACTION_DOWN:
			bPress = true;
		}
	}
	return bPress == false ? INVALIDE_RADIO_CLICK : ANI_RADIO_PRESS;
}

void cRadioButton::Render()
{
	if( !m_bVisible )
		return;

	int nSize = m_vpRadioNode.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_vpRadioNode[ i ]->cCheckBox::Render();			
	}
}

void cRadioButton::SetCheckIndex( int nIndex, bool bEvent )
{
	if( m_nCheckIndex == nIndex )
		return;

	int nSize = m_vpRadioNode.Size();
	for( int i=0; i<nSize; ++i )
	{
		if( m_vpRadioNode[ i ]->IsEnable() )
		{
			m_vpRadioNode[ i ]->SetCheck( i == nIndex );
			if( i == nIndex )		
				m_vpRadioNode[ i ]->SetTextColor( FONT_WHITE );
			else
				m_vpRadioNode[ i ]->SetTextColor( FONT_WHITE * 0.85f );
		}
		else
		{
			m_vpRadioNode[ i ]->SetTextColor( FONT_WHITE * 0.85f );
		}
	}

	m_nCheckIndex = nIndex;
	if( bEvent )
	{
		if( nSize >= m_nCheckIndex && m_nCheckIndex != INVALIDE_RADIO_CLICK )
			OnEvent( RadioButton_LBUP_EVENT, m_vpRadioNode[ m_nCheckIndex ] );
	}
}

void cRadioButton::SetPos( CsPoint ptPos, CsPoint ptDelta )
{
	int nSize = m_vpRadioNode.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_vpRadioNode[ i ]->cCheckBox::SetPos( ptPos );
		ptPos += ptDelta;
	}
}

void cRadioButton::SetAlpha( float fAlpha )
{
	int nSize = m_vpRadioNode.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_vpRadioNode[ i ]->cCheckBox::SetAlpha( fAlpha );
	}
}

void cRadioButton::ResetDevice()
{
	int nSize = m_vpRadioNode.Size();
	for( int i=0; i<nSize; ++i )
	{
		m_vpRadioNode[ i ]->cCheckBox::ResetDevice();
	}
}

const cRadioButton* cRadioButton::GetSelectedNode()
{
	assert_cs( m_vpRadioNode.IsExistElement( m_nCheckIndex ) == true );
	return m_vpRadioNode.GetData( m_nCheckIndex );
}
