
#pragma once 

class CsMapRegionGroup
{
public:
	typedef	std::map< DWORD, CsMapRegionGroup* >			MAP;
	typedef	std::map< DWORD, CsMapRegionGroup* >::iterator	MAP_IT;

protected:
	CsMapRegion::LIST	m_list_MapRegion;

public:
	void				Delete();
	void				Init();

	void				AddRegion( CsMapRegion::sINFO* pSrcInfo );
	void				DeleteRegion( CsMapRegion* pMapRegion );
	CsMapRegion::LIST*	GetRegionList(){ return &m_list_MapRegion; }

	void				Clone( CsMapRegionGroup* pSrcGroup );

	CsMapRegion*		Get( int nIndex );
	int					GetRegionCount(){ return (int)m_list_MapRegion.size(); }

	int					FindIndex( CsMapRegion* pFT );
};


class CsMap_LimitEvoGroup
{
public:
	typedef	std::map< DWORD, CsMap_LimitEvoGroup* >				MAP;
	typedef	std::map< DWORD, CsMap_LimitEvoGroup* >::iterator	MAP_IT;

protected:
	CsMap_LimitEvo::LIST		m_list_MapRegion;

public:
	void						Delete();
	void						Init();

	void						AddRegion( CsMap_LimitEvo::sINFO* pSrcInfo );
	void						DeleteRegion( CsMap_LimitEvo* pMapRegion );
	CsMap_LimitEvo::LIST*		GetList(){ return &m_list_MapRegion; }

	void						Clone( CsMap_LimitEvoGroup* pSrcGroup );

	CsMap_LimitEvo*				Get( int nIndex );
	int							GetRegionCount(){ return (int)m_list_MapRegion.size(); }

	int							FindIndex( CsMap_LimitEvo* pFT );


	bool						IsInRegion( CsPoint pt );
};

class CsMap_Buff_Region
{
public:
	typedef	std::map< DWORD, CsMap_Buff_Region* >				MAP;
	typedef	std::map< DWORD, CsMap_Buff_Region* >::iterator	MAP_IT;

	//void				Init();
	void				Delete();
	void				Clone( CsMap_Buff_Region* pSrcGroup );

	void				AddRegion( CsMap_BuffRegion::sINFO* pkBuffRegionInfo );
	CsMap_BuffRegion*		AddRegionInfo( CsMap_BuffRegion::sINFO* pkBuffRegionInfo );
	void				DeleteRegion( CsMap_BuffRegion* pkBuffRegion );

//Get Setter
	CsMap_BuffRegion::LIST*		GetList(){ return &m_BuffRegions; }
	CsMap_BuffRegion*				Get( int nIndex );
	int									GetRegionCount(){ return (int)m_BuffRegions.size(); }
	int								FindIndex( CsMap_BuffRegion* pFT );
	bool							IsInRegion( CsPoint pt );
private:
	CsMap_BuffRegion::LIST m_BuffRegions;



};

