
#include "stdafx.h"
#include "Npc.h"

THREAD_MEMPOOL_CPP( CNpc )

#define		PARENT_CLASS		CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CNpc, RTTI_MAPNPC )

void CNpc::Init( UINT nIDX, UINT nFileTableID, NiPoint3 vPos, float fRot, sCREATEINFO* pProp )
{
	m_pNpcFT = nsCsFileTable::g_pNpcMng->GetNpc( nFileTableID );

	PARENT_CLASS::Init( nIDX, nFileTableID, vPos, fRot, pProp );	
}

UINT CNpc::GetModelID() const 
{ 
	return m_pNpcFT->GetInfo()->s_dwModelID; 
}