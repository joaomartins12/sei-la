
#pragma once 

class CsSoundMng : public NiMemObject
{
public:
	virtual void*	PlaySound( char const* cSound, NiPoint3 vPos, float fMinDist, float fMaxDist ){ assert_cs( false ); return NULL; }
	virtual void	Update_MoveSound( void* pTarget, NiPoint3 vPos ) { assert_cs( false ); }
}; 