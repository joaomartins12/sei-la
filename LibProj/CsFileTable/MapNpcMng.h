
#pragma once 

typedef	std::map< DWORD, CsMapNpcGroup* >				MAP_MAPNPCGROUP;
typedef	std::map< DWORD, CsMapNpcGroup* >::iterator		MAP_MAPNPCGROUP_IT;

class CsMapNpcMng
{
public:
	void			Delete();
	bool			Init( char* cPath );

	void			Save();

protected:
	void			_LoadExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

protected:
	MAP_MAPNPC		m_mapMapNpc;

public:
	bool			IsNpc( DWORD dwNpcID ){ return ( m_mapMapNpc.find( dwNpcID ) != m_mapMapNpc.end() ); }
	CsMapNpc*		GetNpc( DWORD dwNpcID );
	CsMapNpc*		AddNpc( CsMapNpc::sINFO* pInfo );	

	void			DeleteNpc( DWORD dwMapID, DWORD dwNpcID );

	MAP_MAPNPC*		GetNpcMap(){ return &m_mapMapNpc; }
	

protected:
	MAP_MAPNPCGROUP	m_mapMapNpcGroup;
public:
	bool				IsGroup( DWORD dwMapID ){ return ( m_mapMapNpcGroup.find( dwMapID ) != m_mapMapNpcGroup.end() ); }
	CsMapNpcGroup*		GetGroup( DWORD dwMapID );

	void				DeleteGroup( DWORD dwMapID );

public:
	void			SaveBin( char* cPath );
};