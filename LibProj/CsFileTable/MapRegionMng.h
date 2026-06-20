
#pragma once 

class CsMapRegionMng
{
public:
	void					Delete();
	bool					Init( char* cPath, nsCsFileTable::eFILE_TYPE ft = nsCsFileTable::g_eFileType );

	void					Save();


	//=============================================================================================
	// Region
	//=============================================================================================
public:
	bool					IsRegionGroup( DWORD dwMapID ){ return ( m_mapMapRegionGroup.find( dwMapID ) != m_mapMapRegionGroup.end() ); }
	CsMapRegionGroup*		GetRegionGroup( DWORD dwMapID );
	CsMapRegionGroup::MAP*	GetRegionGroupMap(){ return &m_mapMapRegionGroup; }
protected:
	CsMapRegionGroup::MAP	m_mapMapRegionGroup;


	//=============================================================================================
	// LimitEvo
	//=============================================================================================
public:
	bool						IsLimitEvoGroup( DWORD dwMapID ){ return ( m_mapMapLimitEvoGroup.find( dwMapID ) != m_mapMapLimitEvoGroup.end() ); }
	CsMap_LimitEvoGroup*		GetLimitEvoGroup( DWORD dwMapID );
protected:
	CsMap_LimitEvoGroup::MAP	m_mapMapLimitEvoGroup;

public:
	bool						IsBuffRegionGroup( DWORD dwMapID ){ return ( m_BuffRegions.find( dwMapID ) != m_BuffRegions.end() ); }
	CsMap_Buff_Region*	GetBuffRegionGroup(DWORD dwMapID);
protected:
	CsMap_Buff_Region::MAP m_BuffRegions;



protected:
	//void					_SaveExcel();

	void					_LoadExcel();
	bool					_LoadBin( char* cPath );
	void					_LoadFilePack( char* cPath );

public:
	void					SaveBin( char* cPath );
};