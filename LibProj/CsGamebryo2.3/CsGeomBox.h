
#pragma once 

class CsGeomBox : public NiMemObject
{
public:
	struct sINFO
	{
		void	Reset(){ s_fAlpha = 1.0f; s_Color = NiColor( 1.0f, 1.0f, 1.0f ); s_bTwoSide = false; s_bWireFrame = false; }
		float		s_fAlpha;
		NiColor		s_Color;
		bool		s_bTwoSide;
		bool		s_bWireFrame;
	};
public:
	static NiGeometryPtr	CreateBox( NiPoint3 ptLeftTopMinZ, NiPoint3 ptRightBottomMaxZ, sINFO* pInfo = NULL );
	static NiGeometryPtr	CreateMesh6( NiPoint3 ptLeftTopMinZ, NiPoint3 ptRightBottomMaxZ, sINFO* pInfo = NULL );
	static NiGeometryPtr	CreateMesh8( NiPoint3 ptLeftTopMinZ, NiPoint3 ptRightBottomMaxZ, sINFO* pInfo = NULL );
	static NiGeometryPtr	CreatePolygon( UINT uiBottomVertCount, float fHeight, NiPoint3* pPos, sINFO* pInfo = NULL );

	static void				ResetVertexPos( NiGeometry* pGeom, NiPoint2 ptLeftTop, NiPoint2 ptRightBottom );
	static void				ResetVertexPos( NiGeometry* pGeom, NiPoint3 ptLeftTopMinZ, NiPoint3 ptRightBottomMaxZ );

	static void				MoveDeltaPos( NiGeometry* pGeom, NiPoint2 vDelta );
};