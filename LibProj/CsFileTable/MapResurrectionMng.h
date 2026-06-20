
#pragma once 

class CsMapResurrectionMng
{
public:
	void			Delete();
	void			Init( nsCsFileTable::eFILE_TYPE ft = nsCsFileTable::g_eFileType );

	void			Save();

	bool				IsGroup( DWORD dwMapID ){ return ( m_mapMapGroup.find( dwMapID ) != m_mapMapGroup.end() ); }
	CsMapResurrectionGroup*	GetGroup( DWORD dwMapID );

protected:
	CsMapResurrectionGroup::MAP		m_mapMapGroup;

protected:
	void			_LoadExcel();
	void			_SaveExcel();
};