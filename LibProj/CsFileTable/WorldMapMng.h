
#pragma once 

#include "AreaMap.h"
#include "WorldMap.h"

class CsWorldMapMng
{
public:
	CsWorldMapMng();
	~CsWorldMapMng();

	void			Delete();
	bool			Init( char* cPath );

public:
	void						AddRealWorld( CsWorldMap* pWorld );
	std::list< CsWorldMap* >*	GetRealWorldList();
	void						AddDigitalWorld( CsWorldMap* pWorld );
	std::list< CsWorldMap* >*	GetDigitalWorldList();

protected:
	void				_LoadExcel();
	bool				_LoadBin( char* cPath );
	void				_LoadFilePack( char* cPath );
	void				_ConnectArea();

public:
	bool				IsWorld( int nID ) const;
	bool				IsArea( int nID ) const;

	CsWorldMap*			GetWorld( int nID );
	CsAreaMap*			GetArea( int nID );

	CsAreaMap::MAP*		GetAreaList();
	CsWorldMap::MAP*	GetWorldList();

	std::wstring		GetWorldName( DWORD const& dwWorldID );
	std::wstring		GetAreaName( DWORD const& dwAreaID );

public:
	void				SaveBin( char* cPath );	
	void				ReloadGaussianBlur();

protected:
	CsWorldMap::MAP			m_mapWorld;
	CsAreaMap::MAP			m_mapArea;


	std::list< CsWorldMap*>	m_listRealWorld;
	std::list< CsWorldMap*>	m_listDigitalWorld;
};