
#pragma once 

#include "Buff.h"

class CsBuffMng
{
public:
	void			Delete();
	bool			Init( char* cPath );	

protected:
	CsBuff::MAP		m_mapBuff;

public:
	void			_LoadExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

public:
	bool			IsBuff( USHORT nID ){ return ( m_mapBuff.find( nID ) != m_mapBuff.end() ); }
	CsBuff*			GetBuff( USHORT nID );
	CsBuff::MAP*	GetBuffMap(){ return &m_mapBuff; }

public:
	void			SaveBin( char* cPath );	
};