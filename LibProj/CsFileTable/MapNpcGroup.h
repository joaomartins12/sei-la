

#pragma once 

typedef	std::map< DWORD, CsMapNpc* >				MAP_MAPNPC;
typedef	std::map< DWORD, CsMapNpc* >::iterator		MAP_MAPNPC_IT;

class CsMapNpcGroup
{
protected:
	MAP_MAPNPC		m_mapMapNpc;

public:
	void			Delete();
	void			Init();

	MAP_MAPNPC*			GetNpcMap(){ return &m_mapMapNpc; }

	void				AddNpc( CsMapNpc* pSrcNpc );
	void				DeleteNpc( DWORD dwNpcID );	
};

