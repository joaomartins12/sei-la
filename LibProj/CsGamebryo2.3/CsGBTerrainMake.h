
#pragma once 

class CsGBTerrainMng;
class CsGBTerrainMake : public NiMemObject
{
protected:	
public:
	CsGBTerrainMake();
	~CsGBTerrainMake();

public:
	void		Delete();

	// ==== Create Map
public:
	void		Create( LPCTSTR szBaseDetailMap );
protected:
	NiTriStripsPtr		_CreateCsNiTriPatch( nsCSGBFUNC::sTERRAIN_BOUND& Bound );
	void				_AttachProperty( NiTriStripsPtr pStrips, LPCTSTR szBaseDetailMap );
	void				_AttachEffect( NiTriStripsPtr pStrips );

	void				_CreateBaseNiNode( LPCTSTR szBaseDetailMap );
	void				_CreateNode( NiTriStripsPtr pStrips, nsCSGBFUNC::sTERRAIN_BOUND Bound, LPCTSTR szBaseDetailMap );
	void				_CreateBaseNiNode_ByTool( LPCTSTR szBaseDetailMap );
	

	// ==== Height Map Apply
public:
	bool		ApplyHeightMap( LPCTSTR szMapPath );
protected:
	void		_ApplyHeightMap( sCS_BMPDATA* pBmpData, CsGBTerrainLeaf* pNode );
};