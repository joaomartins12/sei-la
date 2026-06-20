
#pragma once 

class CsMapPortalGroup
{
public:
	typedef	std::map< DWORD, CsMapPortalGroup* >				MAP;
	typedef	std::map< DWORD, CsMapPortalGroup* >::iterator		MAP_IT;

protected:
	CsMapPortal::MAP	m_mapMapPortal;

public:
	void			Delete();
	void			DeleteNpcPortal_ByObjectID( DWORD dwUniqID, int nKindIndex );
	void			Init();

	void				AddPortal( CsMapPortal* pSrcPortal );

	CsMapPortal::MAP*	GetPortalMap(){ return &m_mapMapPortal; }

	CsMapPortal*		GetPortal_ByObjectID( DWORD dwUniqID );
	CsMapPortal*		GetNpcPortal_ByObjectID( DWORD dwUniqID, int nKindIndex );
	CsMapPortal*		GetPortal_ByPortalID( DWORD dwPortalID );
	CsMapPortal*		GetPortal_ByPos( int x, int y );

	CsMapPortal*		GetNearPortal( DWORD nTargetMap, int x, int y );
};

