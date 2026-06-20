
#include "stdafx.h"
#include "PartChar.h"

THREAD_MEMPOOL_CPP( CPartChar )

void CPartChar::_LoadedFile( int nCallRTTI )
{
	CsC_AvObject::_LoadedFile( nCallRTTI );

	if( m_pABVWire != NULL )
	{
		m_pABVWire->SetAppCulled( false );
		m_Node.ResetNiObject( m_CharProp.s_eInsertGeomType );
	}

	m_Node.m_pNiNode->Update( 0.0f );
	CAMERA_ST.SetDeltaHeight( m_Node.m_pNiNode->GetWorldBound().GetCenter().z );
	CAMERA_ST.SetTranslate( NiPoint3::ZERO );
	GetProp_Alpha()->SetUseDistAlpha( false );
}


bool CPartChar::CallBack_EndofSequence( DWORD dwAniID )
{
	GetProp_Animation()->GetSequenceMng()->GetEventOption()->s_eHitType = (rand()%10 == 0 ) ? CsC_AttackProp::HT_Critical : CsC_AttackProp::HT_Normal;

	if( g_pMainWnd->_GetDlgAnimation()->_IsLoop() )
	{
		GetProp_Animation()->ResetAnimation();
		return false;
	}

	if( GetIDX() == 1 ) 
	{
		switch( dwAniID )
		{
		case ANI::ATTACK_ATTACK1:
			GetProp_Animation()->ResetAnimation();
			return false;
		case ANI::ATTACK_ATTACK2:
			GetProp_Animation()->ResetAnimation();
			return false;
		}
	}

	return false;
}

float CPartChar::GetAttackRange()
{
	return 0.0f;
}

float CPartChar::GetAttackCharSize()
{
	return 0.0f;
}

void* CPartChar::PlaySound( char* cSoundPath )
{
	g_pSoundMgr->PlaySound( cSoundPath );
	return NULL;
}