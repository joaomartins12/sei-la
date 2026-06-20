

#include "stdafx.h"
#include "Npc.h"

THREAD_MEMPOOL_CPP( CNpc )
#define PARENT_CLASS	CsC_AvObject
CsCRTTI_CPP( PARENT_CLASS, CNpc, RTTI_NPC )

CNpc::CNpc()
{
	m_pFT_MapNpc = NULL;
	m_pFT_Npc = NULL;	
}

void CNpc::Delete()
{
	PARENT_CLASS::Delete();

	m_pFT_MapNpc = NULL;
	m_pFT_Npc = NULL;	
}

void CNpc::PreDelete()
{
	PARENT_CLASS::PreDelete();
}

void CNpc::Init( uint nUID, uint nFileTableID, sCREATEINFO* pProp )
{
	// 파일 테이블	
	assert_cs( m_pFT_MapNpc == NULL );
	assert_cs( m_pFT_Npc == NULL );
	m_pFT_MapNpc = nsCsMapTable::g_pMapNpcMng->GetNpc( nFileTableID );
	m_pFT_Npc = nsCsFileTable::g_pNpcMng->GetNpc( nFileTableID );

	// 위치값
	NiPoint3 vPos = NiPoint3( (float)m_pFT_MapNpc->GetInfo()->s_nInitPosX, (float)m_pFT_MapNpc->GetInfo()->s_nInitPosY, 0.0f );
	vPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( vPos );

	// 회전값
	PARENT_CLASS::Init( nUID, nFileTableID, vPos, m_pFT_MapNpc->GetInfo()->s_fRotation, pProp );		

	GetBaseStat()->SetHP( 255 );
}

void CNpc::Init_NoMapFT( uint nUID, uint nFileTableID, sCREATEINFO* pProp )
{
	// 파일 테이블	
	assert_cs( m_pFT_MapNpc == NULL );
	assert_cs( m_pFT_Npc == NULL );
	m_pFT_Npc = nsCsFileTable::g_pNpcMng->GetNpc( nFileTableID );

	PARENT_CLASS::Init( nUID, nFileTableID, NiPoint3::ZERO, 0.0f, pProp );
}

void CNpc::Update( float fDeltaTime )
{
	PARENT_CLASS::Update( fDeltaTime );				
}

void CNpc::Render()
{
	PARENT_CLASS::Render();
}

void CNpc::ThreadCallBack_LoadKFM( sLOAD_KFM* pLoad )
{
	PARENT_CLASS::ThreadCallBack_LoadKFM( pLoad );
}



#undef PARENT_CLASS
