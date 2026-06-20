#include "StdAfx.h"
#include "07_EventTamerObject.h"

#define		PARENT_CLASS		CsC_PartObject
CsCRTTI_CPP( PARENT_CLASS, CsC_EventTamerObject, RTTI_EVENTTAMEROBJECT )
THREAD_MEMPOOL_CPP( CsC_EventTamerObject )

CsC_EventTamerObject::CsC_EventTamerObject(void)
{
}

CsC_EventTamerObject::~CsC_EventTamerObject(void)
{
}


void CsC_EventTamerObject::Delete()
{
	PARENT_CLASS::Delete();
}

void CsC_EventTamerObject::Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp, sCHANGE_PART_INFO* pPartInfo )
{
	PARENT_CLASS::Init( nIDX, nFileTableID, vPos, fRot, pProp );	
	PARENT_CLASS::ThreadLoad_Part( pPartInfo, true );

	m_EventIdx = nIDX;
	m_eObjectState = OBJECT_STOP;	
}

void CsC_EventTamerObject::Update( float fDeltaTime, bool bAnimation /* = true  */ )
{
	switch( m_eObjectState )
	{							
	case OBJECT_STOP:		
		break;
	case OBJECT_NORMAL:
		{
			PARENT_CLASS::Update( fDeltaTime );			
		}			
		break;
	case OBJECT_MOVE:
		{
			PARENT_CLASS::Update( fDeltaTime );
			if( GetProp_MoveControl()->IsMove() == false )
			{
				PARENT_CLASS::SetAnimation( ANI::IDLE_NORMAL );
				m_eObjectState = OBJECT_NORMAL;
			}
		}
		break;
	default:
		assert_cs( false );
		break;
	}		
}

void CsC_EventTamerObject::SetScale( float fScale )
{
	PARENT_CLASS::SetScale( fScale );
}

