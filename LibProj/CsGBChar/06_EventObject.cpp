#include "StdAfx.h"
#include "06_EventObject.h"

#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CsC_EventObject, RTTI_EVENTOBJECT )
THREAD_MEMPOOL_CPP( CsC_EventObject )

CsC_EventObject::CsC_EventObject(void)
{
	m_EventIdx = 0;
	m_eObjectState = OBJECT_STOP;
	memset( m_szName, 0, sizeof(TCHAR)*32);	
}

CsC_EventObject::~CsC_EventObject(void)
{
}

void CsC_EventObject::Delete()
{
	PARENT_CLASS::Delete();
	m_EventIdx = 0;
	m_eObjectState = OBJECT_STOP;
	memset( m_szName, 0, sizeof(TCHAR)*32);	
}

void CsC_EventObject::Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp )
{
	PARENT_CLASS::Init( nIDX, nFileTableID, vPos, fRot, pProp );

	m_EventIdx = nIDX;
	m_eObjectState = OBJECT_STOP;	
}

void CsC_EventObject::Update( float fDeltaTime, bool bAnimation /* = true  */ )
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

void CsC_EventObject::Render()
{
	PARENT_CLASS::Render();
}

void CsC_EventObject::SetScale( float fScale )
{
	PARENT_CLASS::SetScale( fScale );
}

bool CsC_EventObject::CallBack_EndofSequence( DWORD dwAniID )
{
	bool bReturn = PARENT_CLASS::CallBack_EndofSequence( dwAniID );
	if( bReturn == false )
		SetDefault_IdleAnimation();

	return bReturn;
}