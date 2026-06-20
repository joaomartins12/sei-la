
#pragma once 

class CsMapStartMng
{
public:
	void				Delete();
	void				Init( nsCsFileTable::eFILE_TYPE ft = nsCsFileTable::g_eFileType );

	void				Save();

	bool				IsGroup( DWORD dwMapID ){ return ( m_mapMapStartGroup.find( dwMapID ) != m_mapMapStartGroup.end() ); }
	CsMapStartGroup*	GetGroup( DWORD dwMapID );

protected:
	CsMapStartGroup::MAP	m_mapMapStartGroup;

protected:
	void				_LoadExcel();
	void				_SaveExcel();
};