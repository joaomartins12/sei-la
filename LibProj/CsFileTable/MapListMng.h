
#pragma once 

class CsMapListMng
{
public:
	void			Delete();
	bool			Init( char* cPath, nsCsFileTable::eFILE_TYPE ft = nsCsFileTable::g_eFileType );

	void			Save();

	bool			IsList( DWORD dwMapID );
	void			AddList( CsMapList::sINFO* pInfo );

	CsMapList*		GetList( const char* cMapPath );
	CsMapList*		GetList( DWORD dwMapID );
	CsMapList*		GetListIndex( int nIndex );
	std::string		GetMapPath( DWORD const& dwMapID ) const;

	CsMapList::MAP*	GetListMap(){ return &m_mapMapList; }

	int				MapIDToMapRegionID( DWORD dwMapID );

	void			Clone( CsMapListMng* pSrcMng );
	bool			IsBattleTagUseMap( DWORD const& dwMapID ) const;

protected:
	CsMapList::MAP	m_mapMapList;

protected:
	void			_LoadExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	void			SaveBin( char* cPath );	
};