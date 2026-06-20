
#include "stdafx.h"
#include "NormalChar.h"

THREAD_MEMPOOL_CPP( CNormalChar )

void CNormalChar::_LoadedFile( int nCallRTTI )
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

bool CNormalChar::CallBack_EndofSequence( DWORD dwAniID )
{
	if( g_pMainWnd->_GetDlgEtc()->_IsAction() == false )
	{
		switch( GetIDX() )
		{
		case 0:
			{
				if( g_pMainWnd->_GetDlgAnimation()->_IsLoop() )
				{
					GetProp_Animation()->ResetAnimation();
					return false;
				}
			}
			break;
		case 1:
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
			break;
		}
	}
	else
	{
		switch( dwAniID )
		{
		case ANI::IDLE_CHAR:
		case ANI::IDLE_SHAKE:
			SetAnimation( ANI::IDLE_NORMAL );
			return true;

		case ANI::DIGIVICE_CHAGE:
			SetPauseAll( false );
			GetProp_Animation()->SetAnimation( ANI::IDLE_NORMAL );
			return true;

		case ANI::ATTACK_ATTACK1:
		case ANI::ATTACK_ATTACK2:
			SetPauseAll( false );
			GetProp_Animation()->SetAnimation( ANI::IDLE_ATTACK );
			return true;

			// 스킬쪽은
		case ANI::ATTACK_SKILL1:
		case ANI::ATTACK_SKILL2:
		case ANI::ATTACK_SKILL3:
		case ANI::ATTACK_SKILL4:
		case ANI::ATTACK_SKILL5:		
			GetProp_Attack()->SetHitTarget( NULL );
			// 잠겼던거 다 풀자
			SetPauseAll( false );
			GetProp_Animation()->SetAnimation( ANI::IDLE_ATTACK );
			return true;
		case ANI::DAMAGE_DAMAGE1:
		case ANI::DAMAGE_DAMAGE2:
			GetProp_Animation()->SetAnimation( ANI::IDLE_ATTACK );
			return true;


			// NPC관련
		case ANI::NPC_NO:
		case ANI::NPC_YES:
		case ANI::NPC_TALK:
			SetAnimation( ANI::IDLE_NORMAL );
			return true;
		}
	}
	
	return false;
}


float CNormalChar::GetAttackRange()
{
	if( nsCsFileTable::g_pDigimonMng->IsDigimon( GetFTID() ) == false )
		return 0.0f;

	return FMDigimon::GetAttackRange( GetFTID(), GetScale() );	
}

float CNormalChar::GetAttackCharSize()
{
	if( nsCsFileTable::g_pDigimonMng->IsDigimon( GetFTID() ) == false )
		return 0.0f;

	return nsCsFileTable::g_pDigimonMng->GetDigimon( GetFTID() )->GetInfo()->s_dwCharSize*GetScale();
}

float CNormalChar::GetAttackSpeed()
{
	if( nsCsFileTable::g_pDigimonMng->IsDigimon( GetFTID() ) == false )
		return 0.0f;

	return FMDigimon::GetAttackSpeed( GetFTID() );
}

int CNormalChar::GetAttackRangeType()
{
	if( nsCsFileTable::g_pDigimonMng->IsDigimon( GetFTID() ) == false )
		return 0;

	return nsCsFileTable::g_pDigimonMng->GetDigimon( GetFTID() )->GetInfo()->s_nDigimonType;
}

void* CNormalChar::PlaySound( char* cSoundPath )
{
	g_pSoundMgr->PlaySound( cSoundPath );
	return NULL;
}