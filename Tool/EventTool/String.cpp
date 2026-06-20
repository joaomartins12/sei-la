#include "stdafx.h"
#include "String.h"

/*
void cString::sICON::Render( CsPoint pos )
{
	switch( s_nIconType )
	{	
	case ICONITEM::DEvol:
		{
			if( s_ptIconSize == CsPoint::ZERO )
				g_pIconMng->RenderDEvol( (nsCsDigimonTable::eEVOLUTION_TYPE)s_nIndex_1, pos + s_ptDeltaPos );
			else
				g_pIconMng->RenderDEvol( (nsCsDigimonTable::eEVOLUTION_TYPE)s_nIndex_1, pos + s_ptDeltaPos, s_ptIconSize );
		}
		
		break;
	case ICONITEM::DAttribute:
		{
			if( s_ptIconSize == CsPoint::ZERO )
				g_pIconMng->RenderDAttribute( (cIconMng::eDAttribute)s_nIndex_1, s_nIndex_2, pos + s_ptDeltaPos );
			else
				g_pIconMng->RenderDAttribute( (cIconMng::eDAttribute)s_nIndex_1, s_nIndex_2, pos + s_ptDeltaPos, s_ptIconSize );
		}		
		break;

	case ICONITEM::ARMOR_1:
	case ICONITEM::ARMOR_2:
	case ICONITEM::ELEMENT_NO_CHECK:
	case ICONITEM::ELEMENT_CHECK:
	case ICONITEM::CHIPSET:
	case ICONITEM::ITEM_CARD:
	case ICONITEM::QUEST_ITEM:
	case ICONITEM::DT_MEMORY:
		{
			cItemInfo item;
			item.m_nType = s_nIndex_1;
			item.m_nCount = s_nIndex_2;
			item.m_nRate = s_nIndex_3;

			if( s_ptIconSize == CsPoint::ZERO )
				g_pIconMng->RenderItem( &item, pos + s_ptDeltaPos, true );
			else
				g_pIconMng->RenderItem( &item, pos + s_ptDeltaPos, s_ptIconSize, true );
		}
		break;
	default:
		{
			if( s_ptIconSize == CsPoint::ZERO )
				g_pIconMng->RenderIcon( (ICONITEM::eTYPE)s_nIconType, pos + s_ptDeltaPos, s_nIndex_1 );
			else
				g_pIconMng->RenderIcon( (ICONITEM::eTYPE)s_nIconType, pos + s_ptDeltaPos, s_ptIconSize, s_nIndex_1 );
		}
		break;
	}
}

void cString::sIMAGE::Render( CsPoint pos )
{
	if( s_ptImageSize == CsPoint::ZERO )
	{
		s_pImage->Render( pos + s_ptDeltaPos, s_nImageIndex );
	}
	else
	{
		CsPoint ptOldSize = s_pImage->GetSize();
		s_pImage->SetSize( s_ptImageSize );
		s_pImage->Render( pos + s_ptDeltaPos, s_nImageIndex );
		s_pImage->SetSize( ptOldSize );
	}
}

void cString::sCHECKBOX::Render( CsPoint pos )
{
	s_pCheckBox->Render( pos + s_ptDeltaPos );		
}

void cString::sMONEYIF::Delete()
{
	NiDelete (cMoneyIF*)s_pMoney;
	s_pMoney = NULL;
}

void cString::sMONEYIF::Render( CsPoint pos )
{
	( (cMoneyIF*)s_pMoney )->Render( pos + s_ptDeltaPos );
}

void cString::sMONEYIF::SetAlpha( float fAlpha )
{
	assert_cs( false );
}

void cString::sMONEYIF::SetColor( NiColor color )
{
	( (cMoneyIF*)s_pMoney )->SetColor( color );
}
*/



CSGBMEMPOOL_CPP( cString::sTEXT );
CSGBMEMPOOL_CPP( cString::sSPRITE );
//CSGBMEMPOOL_CPP( cString::sICON );
//CSGBMEMPOOL_CPP( cString::sIMAGE );
//CSGBMEMPOOL_CPP( cString::sCHECKBOX );
CSGBMEMPOOL_CPP( cString::sDATA_STR );

void cString::GlobalInit()
{
	CSGBMEMPOOL_INIT( cString::sTEXT, 16 );
	CSGBMEMPOOL_INIT( cString::sSPRITE, 8 );
//	CSGBMEMPOOL_INIT( cString::sICON, 4 );
//	CSGBMEMPOOL_INIT( cString::sIMAGE, 4 );
//	CSGBMEMPOOL_INIT( cString::sCHECKBOX, 4 );
	CSGBMEMPOOL_INIT( cString::sDATA_STR, 4 );
}

void cString::GlobalShotDown()
{
	CSGBMEMPOOL_DELETE( cString::sTEXT );
	CSGBMEMPOOL_DELETE( cString::sSPRITE );
//	CSGBMEMPOOL_DELETE( cString::sICON );
//	CSGBMEMPOOL_DELETE( cString::sIMAGE );
//	CSGBMEMPOOL_DELETE( cString::sCHECKBOX );
	CSGBMEMPOOL_DELETE( cString::sDATA_STR );
}

void cString::Delete()
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		switch( (*it)->GetType() )
		{
		case sELEMENT::TEXT:		sTEXT::DeleteInstance( (sTEXT*)*it );			break;
		case sELEMENT::SPRITE:		sSPRITE::DeleteInstance( (sSPRITE*)*it );		break;
//		case sELEMENT::ICON:		sICON::DeleteInstance( (sICON*)*it );			break;
//		case sELEMENT::IMAGE:		sIMAGE::DeleteInstance( (sIMAGE*)*it );			break;
//		case sELEMENT::CHECK_BOX:	sCHECKBOX::DeleteInstance( (sCHECKBOX*)*it );	break;
		case sELEMENT::DATA_STR:	sDATA_STR::DeleteInstance( (sDATA_STR*)*it );	break;
//		case sELEMENT::MONEYIF:		NiDelete (sMONEYIF*)*it;						break;
		default:					assert_cs( false );
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

cString::sTEXT* cString::AddText( cText::sTEXTINFO* pTextInfo )
{
	sTEXT* pText = sTEXT::NewInstance();
	pText->s_Text.Init( NULL, CsPoint::ZERO, pTextInfo, true );
	pText->s_ptSize = pText->s_Text.GetStringSize();	

 	m_listText.push_back( pText );

	m_nMaxSize.x += pText->s_ptSize.x;
	if( m_nMaxSize.y < pText->s_ptSize.y )
		m_nMaxSize.y = pText->s_ptSize.y;

	return pText;
}

void cString::AddText_Head( cText::sTEXTINFO* pTextInfo )
{
	sTEXT* pText = sTEXT::NewInstance();
	pText->s_Text.Init( NULL, CsPoint::ZERO, pTextInfo, true );
	pText->s_ptSize = pText->s_Text.GetStringSize();

	m_listText.push_front( pText );

	m_nMaxSize.x += pText->s_ptSize.x;
	if( m_nMaxSize.y < pText->s_ptSize.y )
		m_nMaxSize.y = pText->s_ptSize.y;
}

cString::sSPRITE* cString::AddSprite( cSprite* pSprite, CsPoint deltaPos /*=CsPoint::ZERO*/, CsPoint ptSize /*=CsPoint::ZERO*/ )
{
	sSPRITE* pSpriteInfo = sSPRITE::NewInstance();
	pSpriteInfo->s_bChangeSize = ( ptSize != CsPoint::ZERO );
	if( pSpriteInfo->s_bChangeSize == true )
		pSpriteInfo->s_ptSize = ptSize;
	else
		pSpriteInfo->s_ptSize = pSprite->GetSize();

	pSpriteInfo->s_pSprite = pSprite;	
	pSpriteInfo->s_ptDeltaPos = deltaPos;

	m_listText.push_back( pSpriteInfo );

	m_nMaxSize.x += pSpriteInfo->s_ptSize.x;
	if( m_nMaxSize.y < pSpriteInfo->s_ptSize.y + deltaPos.y )
		m_nMaxSize.y = pSpriteInfo->s_ptSize.y + deltaPos.y;

	return pSpriteInfo;
}

/*
cString::sICON* cString::AddIcon( CsPoint size, int nIconType, int nIndex1, int nIndex2 , CsPoint deltaPos , CsPoint IconSize  )
{
	sICON* pIcon = sICON::NewInstance();
	pIcon->s_ptSize = size;
	pIcon->s_nIconType = nIconType;
	pIcon->s_nIndex_1 = nIndex1;
	pIcon->s_nIndex_2 = nIndex2;
	pIcon->s_nIndex_3 = 0;
	pIcon->s_fAlpha = 1.0f;
	pIcon->s_ptDeltaPos = deltaPos;
	pIcon->s_ptIconSize = IconSize;

	m_listText.push_back( pIcon );

	m_nMaxSize.x += size.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;

	return pIcon;
}

cString::sIMAGE* cString::AddImage( cImage* pImage, int nImageIndex, CsPoint deltaPos , CsPoint imageSize  )
{
	CsPoint size = pImage->GetSize();

	sIMAGE* pImageInfo = sIMAGE::NewInstance();
	pImageInfo->s_pImage = pImage;
	pImageInfo->s_nImageIndex = nImageIndex;
	pImageInfo->s_ptDeltaPos = deltaPos;
	pImageInfo->s_ptSize = size;
	pImageInfo->s_ptImageSize = imageSize;

	m_listText.push_back( pImageInfo );

	m_nMaxSize.x += size.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;

	return pImageInfo;
}

void cString::AddImage_Head( cImage* pImage, int nImageIndex, CsPoint deltaPos  )
{
	CsPoint size = pImage->GetSize();

	sIMAGE* pImageInfo = sIMAGE::NewInstance();
	pImageInfo->s_pImage = pImage;
	pImageInfo->s_nImageIndex = nImageIndex;
	pImageInfo->s_ptDeltaPos = deltaPos;
	pImageInfo->s_ptSize = size;

	m_listText.push_front( pImageInfo );

	m_nMaxSize.x += size.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;
}

cString::sCHECKBOX* cString::AddCheckBox( char* cCheckBoxPath, bool bCheck, CsPoint TexToken, CsPoint CheckBoxSize, CsPoint deltaPos  )
{
	sCHECKBOX* pCheckBox = sCHECKBOX::NewInstance();
	pCheckBox->s_pCheckBox = NiNew cCheckBox;
	pCheckBox->s_pCheckBox->Init( NULL, CsPoint::ZERO, CheckBoxSize, cCheckBoxPath, false );
	pCheckBox->s_pCheckBox->SetTexToken( TexToken );
	pCheckBox->s_pCheckBox->SetCheck( bCheck );
	pCheckBox->s_ptDeltaPos = deltaPos;
	pCheckBox->s_ptSize = CheckBoxSize;

	m_listText.push_back( pCheckBox );

	m_nMaxSize.x += pCheckBox->s_ptSize.x;
	if( m_nMaxSize.y < pCheckBox->s_ptSize.y )
		m_nMaxSize.y = pCheckBox->s_ptSize.y;

	return pCheckBox;
}

void cString::AddMoneyIF( int nDisplayType, u8 nMoney, NiColor color, int nFontSize , int eAllign , CsPoint deltaPos  )
{
	sMONEYIF* pMoney = NiNew sMONEYIF;
	pMoney->s_ptDeltaPos = deltaPos;	
	pMoney->s_pMoney = NiNew cMoneyIF;
	( (cMoneyIF*)pMoney->s_pMoney )->Init( (cMoneyIF::eDISP_TYPE)nDisplayType, nMoney, false, color, eAllign, nFontSize );
	CsPoint size = ( (cMoneyIF*)pMoney->s_pMoney )->GetSize();
	pMoney->s_ptSize = size;

	m_listText.push_back( pMoney );

	m_nMaxSize.x += size.x;
	if( m_nMaxSize.y < size.y )
		m_nMaxSize.y = size.y;
}
*/

void cString::AddData_Str( int nValue, TCHAR* strBuffer, int nBufferSize )
{
	assert_cs( nBufferSize <= 64 );

	sDATA_STR* pStr = sDATA_STR::NewInstance();
	pStr->s_ptSize = CsPoint::ZERO;
	pStr->s_nValue = nValue;
	_tcscpy_s( pStr->s_szStr, nBufferSize, strBuffer );
	m_listText.push_back( pStr );
}

cString::sELEMENT* cString::Update_ForMouse_Control( CsPoint pos )
{
	sELEMENT* pElement;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();

	for( ; it!=itEnd; ++it )
	{
		pElement = *it;

		switch( pElement->GetType() )
		{
		case sELEMENT::CHECK_BOX:
			{
				/*
				sCHECKBOX* pCheckBox = (sCHECKBOX*)pElement;
				pCheckBox->s_pCheckBox->SetPos( pos + pCheckBox->s_ptDeltaPos );
				if( pCheckBox->s_pCheckBox->Update_ForMouse() == cCheckBox::ACTION_CLICK )
					return pCheckBox;
					*/
			}
			break;
		case sELEMENT::TEXT:
		case sELEMENT::SPRITE:
		case sELEMENT::ICON:
		case sELEMENT::IMAGE:		
		case sELEMENT::MONEYIF:
		case sELEMENT::DATA_STR:
			break;
		default:
			assert_cs( false );
		}		
		pos.x += pElement->s_ptSize.x;
	}
	return NULL;
}

int cString::Render( CsPoint pos, UINT Align )
{
	switch(Align)
	{
	case DT_LEFT:														break;
	case DT_CENTER:		pos.x -= CsFloat2Int( m_nMaxSize.x*0.5f );		break;
	case DT_RIGHT:		pos.x -= m_nMaxSize.x;							break;
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

int cString::RenderLimit( CsPoint pos, UINT Align )
{
	switch(Align)
	{
	case DT_LEFT:														break;
	case DT_CENTER:		pos.x -= CsFloat2Int( m_nMaxSize.x*0.5f );		break;
	case DT_RIGHT:		pos.x -= m_nMaxSize.x;							break;
	}

	return RenderLimit( pos );
}

int cString::RenderLimit( CsPoint pos )
{
	sELEMENT* pElement;
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();

	for( ; it!=itEnd; ++it )
	{
		pElement = *it;
		pElement->RenderLimit( pos );
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

void cString::SetColor( NiColor color )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->SetColor( color );
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

cString::sELEMENT* cString::FindElement( int nElementType )
{
	std::list< sELEMENT* >::iterator it = m_listText.begin();
	std::list< sELEMENT* >::iterator itEnd = m_listText.end();
	for( ; it!=itEnd; ++it )
	{
		if( (*it)->GetType() == nElementType )
			return *it;
	}
	return NULL;
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

	//m_pScrollBar = NULL;
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
	/*
	//if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
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
	}
	*/
	/*
	else
	{		
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

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

			if( --nRender < 1 )
				break;
			if( pos.y > g_nScreenHeight )
				break;
		}
	}	

	nActiveIndex1 = cString::INVALIDE_STRING_VALUE;
	nActiveIndex2 = cString::INVALIDE_STRING_VALUE;
	return CURSOR::BUTTON_OUTWINDOW;
	*/
	
	return 0;
}

cString::sELEMENT* 
cStringList::Update_ForMouse_Control( int& nActiveIndex1, int& nActiveIndex2, CsPoint& pos, int nDeltaY )
{
	cString::sELEMENT* pElement = NULL;

	//if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			pElement = (*it)->Update_ForMouse_Control( pos );
			if( pElement != NULL )
			{
				nActiveIndex1 = (*it)->GetValue1();
				nActiveIndex2 = (*it)->GetValue2();
				return pElement;
			}

			pos.y += ( (*it)->GetMaxSize().y + nDeltaY );
			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	/*
	else
	{		
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			pElement = (*it)->Update_ForMouse_Control( pos );
			if( pElement != NULL )
			{
				nActiveIndex1 = (*it)->GetValue1();
				nActiveIndex2 = (*it)->GetValue2();
				return pElement;
			}

			pos.y += ( (*it)->GetMaxSize().y + nDeltaY );

			if( --nRender < 1 )
				break;
			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	*/

	nActiveIndex1 = cString::INVALIDE_STRING_VALUE;
	nActiveIndex2 = cString::INVALIDE_STRING_VALUE;
	return NULL;
}

void cStringList::Render( CsPoint pos, int nDeltaY, UINT Align )
{
	//if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			pos.y = (*it)->Render( pos, Align );
			pos.y += nDeltaY;
			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	/*
	else
	{
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			pos.y = (*it)->Render( pos, Align );
			pos.y += nDeltaY;
			if( --nRender < 1 )
				break;

			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	*/
}

void cStringList::RenderLimit( CsPoint pos, int nDeltaY, UINT Align )
{
	//if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			pos.y = (*it)->RenderLimit( pos, Align );
			pos.y += nDeltaY;
			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	/*
	else
	{
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				continue;
			}

			if( pos.y + (*it)->GetMaxSize().y > nLimitY )
				break;

			pos.y = (*it)->RenderLimit( pos, Align );
			pos.y += nDeltaY;
			if( --nRender < 1 )
				break;

			if( pos.y > g_nScreenHeight )
				break;
		}
	}
	*/
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

void cStringList::AddTailEmpty( int nFontSize )
{
	cText::sTEXTINFO ti;
	ti.Init( &g_EventScene.m_FontText, nFontSize );
	ti.SetText( L"" );

	cString* pString = NiNew cString;
	pString->AddText( &ti );
	AddTail( pString );
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

int cStringList::GetStringPosY( int nStartY, int nIndex, int nDeltaY )
{
	//if( m_pScrollBar == NULL )
	{
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();
		for( ; it!=itEnd; ++it )
		{
			if( (--nIndex) < 0 )
				return nStartY;
			nStartY += (*it)->GetMaxSize().y + nDeltaY;
		}
	}
	/*
	else
	{
		int nRemainPos = m_pScrollBar->GetCurPosIndex();
		int nRender = m_pScrollBar->GetRenderCount();

		int nLimitY = m_pScrollBar->GetRegion().bottom + m_pScrollBar->GetClient().y;
		std::list< cString* >::iterator it = m_list.begin();
		std::list< cString* >::iterator itEnd = m_list.end();		
		for( ;it!=itEnd; ++it )
		{
			if( nRemainPos > 0 )
			{
				--nRemainPos;
				if( (--nIndex) < 0 )
					return INVALIDE_FIND;

				continue;
			}

			if( (--nIndex) < 0 )
				return nStartY;

			nStartY += (*it)->GetMaxSize().y + nDeltaY;

			if( --nRender < 1 )
				break;
		}
	}
	*/
	return INVALIDE_FIND;
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





