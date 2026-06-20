
#pragma once 

#include "MapTableObjGroup.h"

class CsMapTableObjMng
{
public:
	void				Delete();
	bool				Init();

	void				Save();

	bool				IsGroup( DWORD dwMapID ){ return ( m_mapGroup.find( dwMapID ) != m_mapGroup.end() ); }
	CsMapTableObjGroup*	GetGroup( DWORD dwMapID );

protected:
	CsMapTableObjGroup::MAP	m_mapGroup;

protected:	
	void				_SaveBin();
	bool				_LoadBin();
	bool				_LoadFilePack();
};