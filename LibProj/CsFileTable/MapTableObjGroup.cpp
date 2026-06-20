
#include "stdafx.h"
#include "MapTableObjGroup.h"

void CsMapTableObjGroup::sNODE::Delete()
{
	CsMapTableObj::LIST_IT it = s_list.begin();
	CsMapTableObj::LIST_IT itEnd = s_list.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->Delete();
		delete *it;
	}
	s_list.clear();
}

void CsMapTableObjGroup::sNODE::SaveBin( FILE* fp )
{
	fwrite( &s_nNodeIndex, sizeof( int ), 1, fp );

	int nCount = (int)s_list.size();
	fwrite( &nCount, sizeof( int ), 1, fp );

	CsMapTableObj::LIST_IT it = s_list.begin();
	CsMapTableObj::LIST_IT itEnd = s_list.end();
	for( ; it!=itEnd; ++it )
	{
		(*it)->SaveBin( fp );		
	}
}

void CsMapTableObjGroup::sNODE::LoadBin( FILE* fp )
{
	fread( &s_nNodeIndex, sizeof( int ), 1, fp );

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );

	for( int i=0; i<nCount; ++i )
	{
		CsMapTableObj* pObj = csnew CsMapTableObj;
		pObj->LoadBin( fp );
		s_list.push_back( pObj );
	}
}

void CsMapTableObjGroup::sNODE::LoadFilePack( int nHandle )
{
	_read( nHandle, &s_nNodeIndex, sizeof( int ) );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );

	for( int i=0; i<nCount; ++i )
	{
		CsMapTableObj* pObj = csnew CsMapTableObj;
		pObj->LoadFilePack( nHandle );
		s_list.push_back( pObj );
	}
}

CsMapTableObj* CsMapTableObjGroup::sNODE::Add( CsMapTableObj::sINFO* pInfo )
{
	CsMapTableObj* pObj = csnew CsMapTableObj;
	pObj->Init( pInfo );
	s_list.push_back( pObj );
	return pObj;
}







void CsMapTableObjGroup::Delete()
{
	std::map< int, sNODE* >::iterator it = m_mapNode.begin();
	std::map< int, sNODE* >::iterator itEnd = m_mapNode.end();
	for( ; it!=itEnd; ++it )
	{
		it->second->Delete();
		delete it->second;
	}
	m_mapNode.clear();
}

void CsMapTableObjGroup::Init( DWORD nMapID )
{
	m_nMapID = nMapID;
}


void CsMapTableObjGroup::SaveBin( FILE* fp )
{
	//if(m_nMapID == 1)
	//{
	//	fopen_s( &fp, "Data\\Map\\TableObj2.bin", "wb" );

	//	fwrite( &m_nMapID, sizeof( DWORD ), 1, fp );

	//	int nCount = (int)m_mapNode.size();
	//	fwrite( &nCount, sizeof( int ), 1, fp );	

	//	std::map< int, sNODE* >::iterator it = m_mapNode.begin();
	//	std::map< int, sNODE* >::iterator itEnd = m_mapNode.end();
	//	for( ; it!=itEnd; ++it )
	//	{
	//		it->second->SaveBin( fp );
	//	}
	//}
	//else
	//{
		fwrite( &m_nMapID, sizeof( DWORD ), 1, fp );

		int nCount = (int)m_mapNode.size();
		fwrite( &nCount, sizeof( int ), 1, fp );	

		std::map< int, sNODE* >::iterator it = m_mapNode.begin();
		std::map< int, sNODE* >::iterator itEnd = m_mapNode.end();
		for( ; it!=itEnd; ++it )
		{
			it->second->SaveBin( fp );
		}
	//}
}

void CsMapTableObjGroup::LoadBin( FILE* fp )
{
	fread( &m_nMapID, sizeof( DWORD ), 1, fp );

	int nCount;
	fread( &nCount, sizeof( int ), 1, fp );	

	for( int i=0; i<nCount; ++i )
	{
		sNODE* pNode = csnew sNODE;
		pNode->LoadBin( fp );
		m_mapNode[ pNode->s_nNodeIndex ] = pNode;
	}
}

void CsMapTableObjGroup::LoadFilePack( int nHandle )
{
	_read( nHandle, &m_nMapID, sizeof( DWORD ) );

	int nCount;
	_read( nHandle, &nCount, sizeof( int ) );	

	for( int i=0; i<nCount; ++i )
	{
		sNODE* pNode = csnew sNODE;
		pNode->LoadFilePack( nHandle );
		m_mapNode[ pNode->s_nNodeIndex ] = pNode;
	}
}

void CsMapTableObjGroup::CheckEmpty()
{
	// 정보 없는 그룹은 제거
	std::map< int, sNODE* >::iterator it = m_mapNode.begin();
	std::map< int, sNODE* >::iterator itEnd = m_mapNode.end();
	while( it!=itEnd )
	{
		if( it->second->s_list.empty() == true )
		{
			it->second->Delete();
			delete it->second;
			int nDeleteID = it->first;
			++it;
			m_mapNode.erase( nDeleteID );
		}
		else
		{
			++it;
		}
	}
}

CsMapTableObj* CsMapTableObjGroup::Add( int nNodeID, CsMapTableObj::sINFO* pInfo )
{
	if( IsNode( nNodeID ) == false )
	{
		sNODE* pNode = csnew sNODE;
		pNode->s_nNodeIndex = nNodeID;
		m_mapNode[ pNode->s_nNodeIndex ] = pNode;
	}
	return m_mapNode[ nNodeID ]->Add( pInfo );
}