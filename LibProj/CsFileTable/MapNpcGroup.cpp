
#include "stdafx.h"
#include "MapNpcGroup.h"

void CsMapNpcGroup::Delete()
{
	m_mapMapNpc.clear();
}

void CsMapNpcGroup::Init()
{

}

void CsMapNpcGroup::AddNpc( CsMapNpc* pSrcNpc )
{
	m_mapMapNpc[ pSrcNpc->GetInfo()->s_dwNpcID ] = pSrcNpc;
}

void CsMapNpcGroup::DeleteNpc( DWORD dwNpcID )
{
	m_mapMapNpc.erase( dwNpcID );
}