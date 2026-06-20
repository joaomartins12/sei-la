

#include "stdafx.h"
#include "01_Property.h"

CsC_Property::CsC_Property():m_pParent(NULL)
{ 
}

CsC_Property::~CsC_Property()
{ 
	m_pParent = NULL; 
}

void CsC_Property::ShotDown()
{
	THREAD_MEMPOOL_DELETE( CsC_AttackProp::sHIT_INFO )
	THREAD_MEMPOOL_DELETE( CsC_AttackProp::sDAMAGE_INFO )
	THREAD_MEMPOOL_DELETE( CsC_AttackProp::sEVENT_INFO )
	THREAD_MEMPOOL_DELETE( CsC_AniProp )	
	THREAD_MEMPOOL_DELETE( CsC_PathProp )
	THREAD_MEMPOOL_DELETE( CsC_AlphaProp )
	THREAD_MEMPOOL_DELETE( CsC_AttackProp )
	THREAD_MEMPOOL_DELETE( CsC_EffectProp )
	THREAD_MEMPOOL_DELETE( CsC_MoveControlProp )
}

CsC_Property* CsC_Property::NewProp( eTYPE type )
{
	switch( type )
	{
	case ANIMATION:		return CsC_AniProp::NewInstance();
	case PATH:			return CsC_PathProp::NewInstance();
	case ALPHA:			return CsC_AlphaProp::NewInstance();
	case ATTACK:		return CsC_AttackProp::NewInstance();
	case EFFECT:		return CsC_EffectProp::NewInstance();
	case MOVE_CONTROL:	return CsC_MoveControlProp::NewInstance();
	}

	assert_cs( false );
	return NULL;
}

void CsC_Property::PushProp( CsC_Property* pProp )
{
	SAFE_POINTER_RET( pProp );

	switch( pProp->GetType() )
	{
	case ANIMATION:		CsC_AniProp::DeleteInstance( (CsC_AniProp*)pProp );					break;
	case PATH:			CsC_PathProp::DeleteInstance( (CsC_PathProp*)pProp );				break;
	case ALPHA:			CsC_AlphaProp::DeleteInstance( (CsC_AlphaProp*)pProp );				break;
	case ATTACK:		CsC_AttackProp::DeleteInstance( (CsC_AttackProp*)pProp );			break;
	case EFFECT:		CsC_EffectProp::DeleteInstance( (CsC_EffectProp*)pProp );			break;
	case MOVE_CONTROL:	CsC_MoveControlProp::DeleteInstance( (CsC_MoveControlProp*)pProp );	break;
	default:			assert_cs( false );
	}
}

