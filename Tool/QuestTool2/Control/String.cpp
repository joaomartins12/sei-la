#include "stdafx.h"
#include "String.h"

void cString::Delete()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		switch( (*it)->GetType() )
		{
		case sELEMENT::TEXT:
			{
				sTEXT* pPoint = (sTEXT*)*it;
				SAFE_NIDELETE( pPoint );
			}			
			break;
		case sELEMENT::SPRITE:
			{
				sSPRITE* pPoint = (sSPRITE*)*it;
				SAFE_NIDELETE( pPoint );
			}
			break;
		default:
			assert_cs( false );
		}		
	}
	m_listText.clear();

	_Reset();
}

void cString::ResetDevice()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->ResetDevice();
	}
}

void cString::AddText( cText::sTEXTINFO* pTextInfo )
{
	sTEXT* pText = NiNew sTEXT;
	pText->s_Text.Init( NULL, CsPoint::ZERO, pTextInfo, 1.0f, true );
	pText->s_ptSize = pText->s_Text.GetStringSize();	

 	m_listText.push_back( pText );

	m_nMaxSize.x += pText->s_ptSize.x;
	if( m_nMaxSize.y < pText->s_ptSize.y )
		m_nMaxSize.y = pText->s_ptSize.y;
}

void cString::AddText_Head( cText::sTEXTINFO* pTextInfo )
{
	sTEXT* pText = NiNew sTEXT;
	pText->s_Text.Init( NULL, CsPoint::ZERO, pTextInfo, 1.0f, true );
	pText->s_ptSize = pText->s_Text.GetStringSize();

	m_listText.push_front( pText );

	m_nMaxSize.x += pText->s_ptSize.x;
	if( m_nMaxSize.y < pText->s_ptSize.y )
		m_nMaxSize.y = pText->s_ptSize.y;
}

void cString::AddSprite( cSprite* pSprite, CsPoint deltaPos /*=CsPoint::ZERO*/ )
{
	CsPoint size = pSprite->GetSize();
	sSPRITE* pSpriteInfo = NiNew sSPRITE;
	pSpriteInfo->s_pSprite = pSprite;
	pSpriteInfo->s_ptSize = size;
	pSpriteInfo->s_ptDeltaPos = deltaPos;

	m_listText.push_back( pSpriteInfo );

	m_nMaxSize.x += size.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;
}

int cString::Render( CsPoint pos, UINT Align )
{
	switch(Align)
	{
	case DT_LEFT:													break;
	case DT_CENTER:		pos.x -= (int)( m_nMaxSize.x*0.5f );		break;
	case DT_RIGHT:		pos.x -= m_nMaxSize.x;						break;
	}

	return Render( pos );
}

int cString::Render( CsPoint pos )
{
	sELEMENT* pElement;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();

	for( ; it!=itEnd; ++it )
	{
		pElement = *it;
		pElement->Render( pos );
		pos.x += pElement->s_ptSize.x;
	}	
	return ( pos.y += m_nMaxSize.y );
}

void cString::SetAlpha(float fAlpha /*부하가적다*/ )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->SetAlpha( fAlpha );
	}
}

void cString::TailAddSizeX( int nSizeX )
{
	assert_cs( m_listText.empty() == false );

	std::list< sELEMENT* >::iterator it = m_listText.end();
	--it;
	( *it )->s_ptSize.x += nSizeX;
	m_nMaxSize.x += nSizeX;
}

void cString::CalMaxSize()
{
	m_nMaxSize = CsPoint::ZERO;
	CsPoint size;

	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		size = (*it)->s_ptSize;
		m_nMaxSize.x += size.x;
		if( m_nMaxSize.y < size.y )
			m_nMaxSize.y = size.y;
	}
}

//====================================================================================================
//
//		Find
//
//====================================================================================================

cString::sELEMENT* cString::GetElement( int nIndex )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		if( --nIndex < 0 )
			return *it;
	}
	assert_cs( false );
	return NULL;
}

bool cString::IsText( TCHAR* szText )
{
	sTEXT* pText;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetType() != sELEMENT::TEXT )
			continue;

		pText = (sTEXT*)(*it);
		if( _tcscmp( pText->s_Text.GetTextInfo()->GetText(), szText ) == 0 )
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//		CStringList
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

void cStringList::Delete()
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		SAFE_DELETE( (*it) );
	}
	m_list.clear();	
}

void cStringList::ResetDevice()
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->ResetDevice();
	}
}

int cStringList::Update_ForMouse( int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY, CsSIZE CheckSize )
{
	/*std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetValue1() != cString::INVALIDE_STRING_VALUE )
		{
			CURSOR::eTYPE ct = g_Cursor.CheckClickBox( CsRect( pos, CheckSize ) );
			if( ct != CURSOR::BUTTON_OUTWINDOW )
			{
				nActiveIndex1 = (*it)->GetValue1();
				nActiveIndex2 = (*it)->GetValue2();
				return ct;
			}
		}
		pos.y += ( (*it)->GetMaxSize().y + nDeltaY );
		if( pos.y > g_nScreenHeight )
			break;
	}

	nActiveIndex1 = cString::INVALIDE_STRING_VALUE;
	nActiveIndex2 = cString::INVALIDE_STRING_VALUE;
	return CURSOR::BUTTON_OUTWINDOW;
	*/
	return 0;
}

void cStringList::Render( CsPoint pos, int nDeltaY, UINT Align )
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		pos.y = (*it)->Render( pos, Align );
		pos.y += nDeltaY;
		if( pos.y > g_ptScreen.y )
			break;
	}	
}

void cStringList::SetAlpha(float fAlpha /*부하가적다*/ )
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->SetAlpha( fAlpha );
	}
}

void cStringList::AddHead( cString* pString )
{
	m_list.push_front( pString );
}

void cStringList::AddTail( cString* pString )
{
	m_list.push_back( pString );
}

void cStringList::AddIndexAfter( cString* pString, int nIndex )
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (--nIndex) < 0 )
		{
			m_list.insert( ++it, pString );
			return;
		}
	}
	assert_cs( false );
}

void cStringList::RemoveHead()
{
	assert_cs( m_list.empty() == false );

	std::list< cString* >::iterator it = m_list.begin();
	SAFE_NIDELETE(*it);
	m_list.erase( it );	
}

void cStringList::RemoveTail()
{
	assert_cs( m_list.empty() == false );

	std::list< cString* >::iterator it = m_list.end();
	--it;
	
	SAFE_NIDELETE(*it);
	m_list.erase( it );
}

cString* cStringList::GetString( int nIndex )
{
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (--nIndex) < 0 )
			return (*it);
	}
	assert_cs( false );
	return NULL;
}

cString* cStringList::GetTail()
{
	assert_cs( m_list.empty() == false );

	std::list< cString* >::iterator it = m_list.end();
	--it;
	return *it;
}

CsPoint cStringList::CalMaxSize( int nDeltaY )
{
	CsPoint size = CsPoint::ZERO;
	CsPoint src;
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		src = (*it)->GetMaxSize();
		if( size.x < src.x )
			size.x = src.x;
		size.y += ( src.y + nDeltaY ); 
	}
	return size;
}

//====================================================================================================
//
//		Find
//
//====================================================================================================

int cStringList::FindText( TCHAR* szText )
{
	int nIndex = 0;
	std::list< cString* >::iterator it = m_list.begin();
	std::list< cString* >::iterator itEnd = m_list.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->IsText( szText ) == true )
		{
			return nIndex;
		}

		++nIndex;
	}
	return INVALIDE_FIND;
}





