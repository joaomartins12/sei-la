

#include "stdafx.h"
#include "NpcGroup.h"

void CsNpcGroup::Delete()
{
	m_mapNpc.clear();
}

void CsNpcGroup::Init()
{

}

void CsNpcGroup::DeleteNpc( DWORD dwNpcTableID )
{
	CsNpc::MAP_IT it = m_mapNpc.find( dwNpcTableID );
	if( it == m_mapNpc.end() )
		return;
	//assert_cs( m_mapNpc.find( dwNpcTableID ) != m_mapNpc.end() );
	m_mapNpc.erase( dwNpcTableID );
}

void CsNpcGroup::AddNpc( CsNpc* pSrcNpc )
{
	if( NULL == pSrcNpc )
		return;

	CsNpc::sINFO* pNpcInfo = pSrcNpc->GetInfo();
	if( NULL == pNpcInfo )
		return;

	CsNpc::MAP_IT it = m_mapNpc.find( pNpcInfo->s_dwNpcID );
	if( it != m_mapNpc.end() )
		return;

	m_mapNpc.insert( std::make_pair( pNpcInfo->s_dwNpcID, pSrcNpc ) );

// 	assert_cs( m_mapNpc.find( pSrcNpc->GetInfo()->s_dwNpcID ) == m_mapNpc.end() );
// 	m_mapNpc[ pSrcNpc->GetInfo()->s_dwNpcID ] = pSrcNpc;
}