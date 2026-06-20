

//==========================================================================================
//
//		Emr Info
//
//==========================================================================================
inline bool CsGBTerrainRoot::IsBValidEmr( UINT bx, UINT by )
{
	assert_cs( m_ppEmrBlock );

	if(bx>=m_nEmrBlockWidth)
	{
		return false;
	}
	if(by>=m_nEmrBlockHeight)
	{
		return false;
	}
	return m_ppEmrBlock[bx][by] ? false : true;
}

inline bool CsGBTerrainRoot::IsValidEmr( UINT x, UINT y )
{
	UINT bx = x/m_nEmrBlockScale;
	UINT by = y/m_nEmrBlockScale;

	return IsBValidEmr(bx, by);
}


inline bool CsGBTerrainRoot::IsValidEmr2( UINT x, UINT y )
{
	UINT bx = x/m_nEmrBlockScale;
	UINT by = y/m_nEmrBlockScale;

	if(!IsBValidEmr(bx-1, by-1)) return false;
	if(!IsBValidEmr(bx-1, by  )) return false;
	if(!IsBValidEmr(bx-1, by+1)) return false;
	if(!IsBValidEmr(bx  , by-1)) return false;
	if(!IsBValidEmr(bx  , by  )) return false;
	if(!IsBValidEmr(bx  , by+1)) return false;
	if(!IsBValidEmr(bx+1, by-1)) return false;
	if(!IsBValidEmr(bx+1, by  )) return false;
	if(!IsBValidEmr(bx+1, by+1)) return false;

	return true;
}
