
#pragma once 

class CsWeatherMng
{
public:
	void				Delete();
	bool				Init( char* cPath, nsCsFileTable::eFILE_TYPE ft = nsCsFileTable::g_eFileType );

	bool				IsGroup( DWORD dwMapID ){ return ( m_mapGroup.find( dwMapID ) != m_mapGroup.end() ); }
	CsWeatherGroup*		GetGroup( DWORD dwMapID );
	bool				IsEnableCheck(){ return m_CheckTime.IsEnable(); }

protected:
	CsWeatherGroup::MAP	m_mapGroup;
	CsTimeSeq			m_CheckTime;

protected:
	void				_LoadExcel();	
	bool				_LoadBin( char* cPath );
	void				_LoadFilePack( char* cPath );
	
public:
	void				Save();
	void				SaveBin( char* cPath );

protected:
	void				_SaveExcel();
};