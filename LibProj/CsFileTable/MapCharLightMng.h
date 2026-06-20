
#pragma once 

class CsMapCharLightMng
{
public:
	void					Delete();
	bool					Init( char* cPath, nsCsFileTable::eFILE_TYPE ft = nsCsFileTable::g_eFileType );

	void					Save();

	bool					IsGroup( DWORD dwMapID ){ return ( m_mapGroup.find( dwMapID ) != m_mapGroup.end() ); }
	CsMapCharLightGroup*	GetGroup( DWORD dwMapID );
	void					DeleteGroup( DWORD dwMapID );

protected:
	CsMapCharLightGroup::MAP	m_mapGroup;

protected:
	void					_SaveExcel();

	void					_LoadExcel();
	bool					_LoadBin( char* cPath );
	void					_LoadFilePack( char* cPath );

public:
	void					SaveBin( char* cPath );
};