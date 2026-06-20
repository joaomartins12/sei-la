
#pragma once 

#include "common_vs2019/cMapRegion.h"

class cData_MapRegion
{
public:
	cData_MapRegion();

public:
	void	Delete();
	void	Init();
	void	InitNetOff();
	void	LogOut();

protected:
	bool			m_bFirstLoad;
	cMapRegion		m_MapRegion;
	cMapRegion		m_MapRegionBackup;

public:
	void			ToBackup(){ m_MapRegionBackup = m_MapRegion; }
	bool			IsFirstLoad(){ return m_bFirstLoad; }
	void			FirstLoadComp(){ m_bFirstLoad = false; }

	cMapRegion*		GetMapRegion(){ return &m_MapRegion; }
	void			InRegion( int nRegionIndex );

	bool			IsOpenedWorld( int nWorldID );		// nWorldID ´Â WorldMap.xls Á¤º¸´Ù
	bool			IsOpenedMap( int nMapID );
};