#include "StdAfx.h"
#include "TacticsObject.h"

#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, cTacticsObject, RTTI_TACTICSOBJECT )

cTacticsObject::cTacticsObject(void)
{
}

cTacticsObject::~cTacticsObject(void)
{
}


void cTacticsObject::Delete()
{
	PARENT_CLASS::Delete();	
}

void cTacticsObject::Init( UINT nFileTableID, NiPoint3 vPos, float fRot, float fStartTime, sCREATEINFO* pProp )
{
	PARENT_CLASS::Init( 0, nFileTableID, vPos, fRot, pProp );
	m_bUpdate = true;
}

void cTacticsObject::Update( float fDeltaTime, bool bAnimation /* = true  */ )
{	
	m_fCurTime += fDeltaTime;	

	if( CheckModel() == CHECK_NOT_LOAD )
		return;

	if( m_bUpdate )
		_FirstUpdate();

	GetProp_Alpha()->Update( fDeltaTime );	
	GetProp_Effect()->Update( fDeltaTime );	
	if( m_fCurTime < m_fEndTime )
	{
		GetProp_Animation()->Update( fDeltaTime );	

		float fAniTime = GetAniTime();		
		m_Node.m_pNiNode->_UpdateSelect( fAniTime, true );			
	}
	else
	{
		_SetAniTime( m_fEndTime );				
	}	

}

void cTacticsObject::_FirstUpdate()
{
	SetAnimation( m_dwAniID );
	_SetAniTime( m_fStartTime );
	m_bUpdate = false;
}

void cTacticsObject::_SetAniTime( float fTime )
{
	GetProp_Animation()->GetSequenceMng()->SetCurTime( fTime ); 	
}

void cTacticsObject::SetAniOption( DWORD dwAni, float fStartTime, float fEndTime )
{
	m_dwAniID = dwAni;
	m_fStartTime = fStartTime;
	m_fEndTime = fEndTime;
	m_fCurTime = 0.0f;

	assert_cs( m_fStartTime < fEndTime );
	assert_cs( m_dwAniID >= ANI::IDLE_NORMAL && m_dwAniID < ANI::EMOTION_START );
}
