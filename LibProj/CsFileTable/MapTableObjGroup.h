
#pragma once 

#include "MapTableObj.h"

class CsMapTableObjGroup
{
public:
	typedef	std::map< DWORD, CsMapTableObjGroup* >				MAP;
	typedef	std::map< DWORD, CsMapTableObjGroup* >::iterator	MAP_IT;

public:
	struct sNODE
	{
		sNODE(){ s_nNodeIndex = 0; }

		void					Delete();
		void					SaveBin( FILE* fp );
		void					LoadBin( FILE* fp );
		void					LoadFilePack( int nHandle );
		CsMapTableObj*			Add( CsMapTableObj::sINFO* pInfo );

		int						s_nNodeIndex;
		CsMapTableObj::LIST		s_list;
	};

public:
	void				Delete();
	void				Init( DWORD nMapID );

	void				SaveBin( FILE* fp );
	void				LoadBin( FILE* fp );	
	void				LoadFilePack( int nHandle );

protected:
	DWORD							m_nMapID;
	std::map< int, sNODE* >			m_mapNode;

public:
	void				CheckEmpty();

	DWORD				GetMapID(){ return m_nMapID; }
	bool				IsNode( int nNodeID ){ return ( m_mapNode.find( nNodeID ) != m_mapNode.end() ); }
	sNODE*				GetNode( int nNodeID ){ return m_mapNode[ nNodeID ]; }
	int					GetNodeSize(){ return (int)m_mapNode.size(); }

	CsMapTableObj*		Add( int nNodeID, CsMapTableObj::sINFO* pInfo );
};

