
#pragma once 

class CsMapPortalMng
{
public:
	void			Delete();
	bool			Init( char* cPath );

	void			Save();

	CsMapPortal*	GetPortal_ByObjectID( DWORD dwMapID, DWORD dwUniqID );
	CsMapPortal*	GetNpcPortal_ByObjectID( DWORD dwMapID, DWORD dwUniqID, int nKindIndex );
	CsMapPortal*	GetPortal_ByPortalID( DWORD dwMapID, DWORD dwPortalID );
	CsMapPortal*	GetPortal_ByPos( DWORD dwMapID, int x, int y );
	bool			IsPortal_Data( DWORD dwMapID );

protected:
	void			_LoadExcel();
	bool			_LoadBin( char* cPath );
	void			_LoadFilePack( char* cPath );

	// ===== Portal
public:
	void			AddPortal( CsMapPortal::sINFO* pInfo, sNPC_TYPE_PORTAL::sPORTAL* pLink );


	// ===== Group
protected:
	CsMapPortalGroup::MAP	m_mapMapPortalGroup;
public:
	bool				IsGroup( DWORD dwMapID ){ return ( m_mapMapPortalGroup.find( dwMapID ) != m_mapMapPortalGroup.end() ); }
	CsMapPortalGroup*	GetGroup( DWORD dwMapID );
	void				DeleteGroup( DWORD dwMapID );
	void				DeleteGroup_PortalType( DWORD dwMapID, CsMapPortal::ePORTAL_TYPE type );

	CsMapPortalGroup::MAP*	GetGroupMap(){ return &m_mapMapPortalGroup; }

public:
	void			SaveBin( char* cPath );
};