
#include "stdafx.h"
#include "Popup.h"


void cPopup::Delete()
{
}

void cPopup::Init()
{
	m_pSrcDataPointer = NULL;
	m_bRenderPopup = false;
}

void cPopup::Render()
{
	if( m_bRenderPopup == false )
		return;

	switch( m_eType )
	{
	case JUMP:
		{
			cQuestObj* pObject = (cQuestObj*)m_pSrcDataPointer;
			pObject->SetAlpha( 0.6f );
			pObject->RenderPos( m_ptPos );
			pObject->SetAlpha( 1.0f );
		}
		break;
	default:
		assert_cs( false );
	}

	m_bRenderPopup = false;
}

void cPopup::SetPopup( eTYPE type, void* pData, CsPoint pos )
{
	m_bRenderPopup = true;

	/*if( ( m_pSrcDataPointer == pData )&&( m_eType == type ) )
		return;*/

	m_pSrcDataPointer = pData;
	m_eType = type;
	m_ptPos = pos;

	/*cText::sTEXTINFO ti;
	ti.Init( cFont::FS_10, NiColor::BLACK );
	ti.s_bOutLine = false;

	switch( type )
	{
	case JUMP:
		{
			cQuestObj* pObject = (cQuestObj*)m_pSrcDataPointer;			
		}
		break;
	default:
		assert_cs( false );
	}*/
}