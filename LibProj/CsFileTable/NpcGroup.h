
#pragma once 

class CsNpcGroup
{
public:
	typedef	std::map< DWORD, CsNpcGroup* >					MAP;
	typedef	std::map< DWORD, CsNpcGroup* >::iterator		MAP_IT;

protected:
	CsNpc::MAP		m_mapNpc;
public:
	void			Delete();
	void			Init();

	void			DeleteNpc( DWORD dwNpcTableID );
	void			AddNpc( CsNpc* pSrcNpc );


public:
	CsNpc::MAP*		GetNpcMap(){ return &m_mapNpc; }
};

