
#pragma once 

class CsMapMonsterMng
{
public:
	void			Delete();
	bool			Init( char* cPath, nsCsFileTable::eFILE_TYPE ft = nsCsFileTable::g_eFileType );

	void			Save();
	void			SaveBin( char* cPath );
	void			Reload( char* cPath ) { _LoadBin( cPath ); }

protected:
	void			_LoadExcel();
	void			_SaveExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

	// ===== Monster
public:
	struct sMapMonster
	{
	public:
		void					Delete();
		void					Init( DWORD dwMapID, CsMapMonster* Mon );				
		int						GetMapSize() { return (int)s_MonList.size(); }

		std::map< DWORD, CsMapMonster::LIST >*		GetMonsterList() { return &s_MonList; }

	protected:
		std::map< DWORD, CsMapMonster::LIST >	s_MonList;
	};

	sMapMonster* GetMonster( DWORD dwMonID );

protected:	
	typedef	std::map< DWORD, sMapMonster* >					MAP;
	typedef	std::map< DWORD, sMapMonster* >::iterator		MAP_IT;

	MAP		m_mapMapMonster;
	void	_SaveData();
	bool	_AddMapMonster( DWORD dwId, CsMapMonster* Mon );


public:
	void			AddMonster( CsMapMonster::sINFO* pInfo );
	void			DeleteMonster( DWORD dwMapID, int nDeleteListIndex );
	

	// ===== Group
protected:
	CsMapMonsterGroup::MAP		m_mapMapMonsterGroup;
public:
	bool				IsGroup( DWORD dwMapID ){ return ( m_mapMapMonsterGroup.find( dwMapID ) != m_mapMapMonsterGroup.end() ); }
	CsMapMonsterGroup*	GetGroup( DWORD dwMapID );
	void				DeleteGroup( DWORD dwMapID );		


};