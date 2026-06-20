
#include "stdafx.h"
#include "MoveControlProperty.h"

THREAD_MEMPOOL_CPP( CsC_MoveControlProp )

#define PARENT_CALSS		CsC_Property

CsC_MoveControlProp::CsC_MoveControlProp()
{
	m_eType = CsC_Property::MOVE_CONTROL;

	m_pFTMoveObject = NULL;
	m_fAccumTime = 0.0f;
}

CsC_MoveControlProp::~CsC_MoveControlProp()
{

}

void CsC_MoveControlProp::Delete()
{
	PARENT_CALSS::Delete();

	m_pFTMoveObject = NULL;
	m_fAccumTime = 0.0f;
}

void CsC_MoveControlProp::Init( CsC_AvObject* pParent )
{
	PARENT_CALSS::Init( pParent );
	m_fAccumTime = 0.0f;
}

void CsC_MoveControlProp::ResetAccumTime()
{
	m_fAccumTime = nsCsGBTerrain::g_ServerTime.GetInitLoopAccumTime( m_pFTMoveObject->GetTotalTime() );
	assert( m_fAccumTime >= 0.0f );
}

void CsC_MoveControlProp::PreUpdate( float fDeltaTime )
{
	if( m_pFTMoveObject == NULL )	
		return;

	if( m_pFTMoveObject->GetTotalTime() <= 0.0f )
		return;

	m_fAccumTime += fDeltaTime;
	
	while( m_fAccumTime >= m_pFTMoveObject->GetTotalTime() )
		m_fAccumTime -= m_pFTMoveObject->GetTotalTime();

	DWORD dwAni = ANI::MOVE_WALK;
	NiPoint2 pos;
	NiPoint3 dir;
	int nMoveSpeed = 0;
	if( m_pFTMoveObject->GetPos( m_fAccumTime, pos.x, pos.y, dir.x, dir.y, dwAni, nMoveSpeed ) == true )
	{		
		m_pParent->SetRotation_AniDir( dir );
		m_bMove = true;
	}	
	else
		m_bMove = false;

	if( dwAni != ANI::INVALIDE_ID )
	{
		m_pParent->SetAnimation( dwAni );
		// ¾Æ·¡°Å »¸À½
		//m_pParent->SetAnimation( dwAni, m_pParent->GetProp_Animation()->GetAniConstantTime( (float)nMoveSpeed ) );
	}
	m_pParent->SetPos( pos );
}

void CsC_MoveControlProp::SetFTMoveObject( CsMoveObject* pFTMO )
{
	m_pFTMoveObject = pFTMO;	
	ResetAccumTime();
}

void CsC_MoveControlProp::SetFTEventMoveObject( CsMoveObject* pFTMO )
{
	m_pFTMoveObject = pFTMO;	
	m_fAccumTime = 0.0f;
}

#undef PARENT_CLASS
