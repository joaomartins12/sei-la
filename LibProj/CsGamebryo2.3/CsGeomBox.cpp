
#include "stdafx.h"
#include "CsGeomBox.h"

NiGeometryPtr CsGeomBox::CreateBox( NiPoint3 ptLeftTopMinZ, NiPoint3 ptRightBottomMaxZ, sINFO* pInfo /* = NULL  */)
{
	sINFO info;
	if( pInfo == NULL )
	{
		pInfo = &info;
		pInfo->Reset();
	}

	NiPoint3* pPtsF = NiNew NiPoint3[ 8 ];
	unsigned short* pIndex = NiAlloc( unsigned short, 36 );

	pPtsF[ 0 ] = ptLeftTopMinZ;

	pPtsF[ 1 ].x = ptRightBottomMaxZ.x;
	pPtsF[ 1 ].y = ptLeftTopMinZ.y;
	pPtsF[ 1 ].z = ptLeftTopMinZ.z;

	pPtsF[ 2 ].x = ptRightBottomMaxZ.x;
	pPtsF[ 2 ].y = ptRightBottomMaxZ.y;
	pPtsF[ 2 ].z = ptLeftTopMinZ.z;

	pPtsF[ 3 ].x = ptLeftTopMinZ.x;
	pPtsF[ 3 ].y = ptRightBottomMaxZ.y;
	pPtsF[ 3 ].z = ptLeftTopMinZ.z;	

	pPtsF[ 4 ].x = ptLeftTopMinZ.x;
	pPtsF[ 4 ].y = ptLeftTopMinZ.y;
	pPtsF[ 4 ].z = ptRightBottomMaxZ.z;

	pPtsF[ 5 ].x = ptRightBottomMaxZ.x;
	pPtsF[ 5 ].y = ptLeftTopMinZ.y;
	pPtsF[ 5 ].z = ptRightBottomMaxZ.z;

	pPtsF[ 6 ] = ptRightBottomMaxZ;

	pPtsF[ 7 ].x = ptLeftTopMinZ.x;
	pPtsF[ 7 ].y = ptRightBottomMaxZ.y;
	pPtsF[ 7 ].z = ptRightBottomMaxZ.z;	

	pIndex[ 0 ] = 0;		pIndex[ 1 ] = 5;		pIndex[ 2 ] = 4;
	pIndex[ 3 ] = 0;		pIndex[ 4 ] = 1;		pIndex[ 5 ] = 5;
	pIndex[ 6 ] = 3;		pIndex[ 7 ] = 0;		pIndex[ 8 ] = 4;
	pIndex[ 9 ] = 3;		pIndex[ 10 ] = 4;		pIndex[ 11 ] = 7;
	pIndex[ 12 ] = 2;		pIndex[ 13 ] = 3;		pIndex[ 14 ] = 7;
	pIndex[ 15 ] = 2;		pIndex[ 16 ] = 7;		pIndex[ 17 ] = 6;
	pIndex[ 18 ] = 1;		pIndex[ 19 ] = 6;		pIndex[ 20 ] = 5;
	pIndex[ 21 ] = 1;		pIndex[ 22 ] = 2;		pIndex[ 23 ] = 6;
	pIndex[ 24 ] = 0;		pIndex[ 25 ] = 2;		pIndex[ 26 ] = 1;
	pIndex[ 27 ] = 0;		pIndex[ 28 ] = 3;		pIndex[ 29 ] = 2;
	pIndex[ 30 ] = 4;		pIndex[ 31 ] = 5;		pIndex[ 32 ] = 6;
	pIndex[ 33 ] = 4;		pIndex[ 34 ] = 6;		pIndex[ 35 ] = 7;

	NiTriShapePtr pShape = NiNew NiTriShape( 8, pPtsF, NULL, NULL, NULL, 0, NiGeometryData::NBT_METHOD_NONE, 12, pIndex );

	NiMaterialPropertyPtr pMapProp = NiNew NiMaterialProperty;
	pMapProp->SetAlpha( pInfo->s_fAlpha );
	pMapProp->SetEmittance( pInfo->s_Color );
	pShape->AttachProperty( pMapProp );	

	if( pInfo->s_fAlpha != 1.0f )
	{
		NiAlphaPropertyPtr pAlphaProp = NiNew NiAlphaProperty;
		pAlphaProp->SetAlphaBlending( true );
		pAlphaProp->SetAlphaTesting( false );
		pShape->AttachProperty( pAlphaProp );
	}

	if( pInfo->s_bTwoSide == true )
	{
		NiStencilPropertyPtr pStencilProp = NiNew NiStencilProperty;
		pStencilProp->SetDrawMode( NiStencilProperty::DRAW_BOTH );
		pShape->AttachProperty( pStencilProp );
	}

	if( pInfo->s_bWireFrame == true )
	{
		NiWireframePropertyPtr pWireProp = NiNew NiWireframeProperty;
		pWireProp->SetWireframe( true );
		pShape->AttachProperty( pWireProp );
	}

	pShape->UpdateProperties();
	pShape->Update( 0.0f );
	return NiDynamicCast( NiGeometry, pShape );
}

NiGeometryPtr CsGeomBox::CreateMesh6( NiPoint3 ptLeftTopMinZ, NiPoint3 ptRightBottomMaxZ, sINFO* pInfo /* = NULL  */)
{
	float fDeltaHeight = ptRightBottomMaxZ.z - ptLeftTopMinZ.z;
	/*float fLen = sqrt( ( ptRightBottomMaxZ.x - ptLeftTopMinZ.x )*( ptRightBottomMaxZ.x - ptLeftTopMinZ.x ) +
					   ( ptRightBottomMaxZ.y - ptLeftTopMinZ.y )*( ptRightBottomMaxZ.y - ptLeftTopMinZ.y )	)*0.5f;*/
	float dx = ( ptRightBottomMaxZ.x - ptLeftTopMinZ.x )*0.5f;
	float dy = ( ptRightBottomMaxZ.y - ptLeftTopMinZ.y )*0.5f;

	NiPoint3 pos[ 6 ];
	pos[ 0 ] = ptLeftTopMinZ;

	pos[ 1 ].x = ptRightBottomMaxZ.x;
	pos[ 1 ].y = ptLeftTopMinZ.y;
	pos[ 1 ].z = ptLeftTopMinZ.z;

	pos[ 2 ].x = pos[ 1 ].x + dx;
	pos[ 2 ].y = pos[ 1 ].y + dy;
	pos[ 2 ].z = ptLeftTopMinZ.z;

	pos[ 3 ].x = pos[ 2 ].x - dx;
	pos[ 3 ].y = pos[ 2 ].y + dy;
	pos[ 3 ].z = ptLeftTopMinZ.z;

	pos[ 4 ].x = ptLeftTopMinZ.x;
	pos[ 4 ].y = ptRightBottomMaxZ.y;
	pos[ 4 ].z = ptLeftTopMinZ.z;

	pos[ 5 ].x = pos[ 4 ].x - dx;
	pos[ 5 ].y = pos[ 4 ].y - dy;
	pos[ 5 ].z = ptLeftTopMinZ.z;

	return CreatePolygon( 6, fDeltaHeight, pos, pInfo );
}

NiGeometryPtr CsGeomBox::CreateMesh8( NiPoint3 ptLeftTopMinZ, NiPoint3 ptRightBottomMaxZ, sINFO* pInfo /* = NULL  */)
{
	float fDeltaHeight = ptRightBottomMaxZ.z - ptLeftTopMinZ.z;
	float fLen = sqrt(  ( ptRightBottomMaxZ.x - ptLeftTopMinZ.x )*( ptRightBottomMaxZ.x - ptLeftTopMinZ.x ) +
						( ptRightBottomMaxZ.y - ptLeftTopMinZ.y )*( ptRightBottomMaxZ.y - ptLeftTopMinZ.y )	)*0.5f;
	float dx = fLen*sin( CsD2R( 45.0f ) );
	float dy = fLen*cos( CsD2R( 45.0f ) );

	float dy2 = ( ptRightBottomMaxZ.y - ptLeftTopMinZ.y ) - dy*2.0f;

	NiPoint3 pos[ 8 ];
	pos[ 0 ] = ptLeftTopMinZ;

	pos[ 1 ].x = ptRightBottomMaxZ.x;
	pos[ 1 ].y = ptLeftTopMinZ.y;
	pos[ 1 ].z = ptLeftTopMinZ.z;

	pos[ 2 ].x = pos[ 1 ].x + dx;
	pos[ 2 ].y = pos[ 1 ].y + dy;
	pos[ 2 ].z = ptLeftTopMinZ.z;

	pos[ 3 ].x = pos[ 2 ].x;
	pos[ 3 ].y = pos[ 2 ].y + dy2;
	pos[ 3 ].z = ptLeftTopMinZ.z;

	pos[ 4 ].x = pos[ 3 ].x - dx;
	pos[ 4 ].y = pos[ 3 ].y + dy;
	pos[ 4 ].z = ptLeftTopMinZ.z;

	pos[ 5 ].x = ptLeftTopMinZ.x;
	pos[ 5 ].y = ptRightBottomMaxZ.y;
	pos[ 5 ].z = ptLeftTopMinZ.z;

	pos[ 6 ].x = pos[ 5 ].x - dx;
	pos[ 6 ].y = pos[ 5 ].y - dy;
	pos[ 6 ].z = ptLeftTopMinZ.z;

	pos[ 7 ].x = pos[ 6 ].x;
	pos[ 7 ].y = pos[ 6 ].y - dy2;
	pos[ 7 ].z = ptLeftTopMinZ.z;

	return CreatePolygon( 8, fDeltaHeight, pos, pInfo );
}

NiGeometryPtr CsGeomBox::CreatePolygon( UINT uiBottomVertCount, float fHeight, NiPoint3* pPos, sINFO* pInfo /* = NULL  */ )
{
	sINFO info;
	if( pInfo == NULL )
	{
		pInfo = &info;
		pInfo->Reset();
	}	

	UINT uiTriCount = ( uiBottomVertCount + uiBottomVertCount - 2 ) * 2;
	//UINT uiTriCount = uiBottomVertCount*2;
	NiPoint3* pPtsF = NiNew NiPoint3[ uiBottomVertCount*2 ];
	unsigned short* pIndex = NiAlloc( unsigned short, uiTriCount*3 );

	for( UINT i=0; i<uiBottomVertCount; ++i )
	{
		pPtsF[ i ] = pPos[ i ];
		pPtsF[ i + uiBottomVertCount ] = pPos[ i ];
		pPtsF[ i + uiBottomVertCount ].z += fHeight;
	}


	for( UINT i=0; i<uiBottomVertCount; ++i )
	{
		int nDest = i+1;
		if( nDest == uiBottomVertCount )
			nDest = 0;

		pIndex[ i*6 + 0 ] = i;
		pIndex[ i*6 + 1 ] = nDest;
		pIndex[ i*6 + 2 ] = nDest+uiBottomVertCount;

		pIndex[ i*6 + 3 ] = i;
		pIndex[ i*6 + 4 ] = i+uiBottomVertCount;
		pIndex[ i*6 + 5 ] = nDest+uiBottomVertCount;
	}

	int nTopIndex = uiBottomVertCount*6;
	for( UINT i=0; i<uiBottomVertCount-2; ++i )
	{
		int nDest = i+1;

		pIndex[ nTopIndex + i*6 + 0 ] = 0;
		pIndex[ nTopIndex + i*6 + 1 ] = nDest + 1;
		pIndex[ nTopIndex + i*6 + 2 ] = nDest;

		pIndex[ nTopIndex + i*6 + 3 ] = uiBottomVertCount;
		pIndex[ nTopIndex + i*6 + 4 ] = uiBottomVertCount + nDest;
		pIndex[ nTopIndex + i*6 + 5 ] = uiBottomVertCount + nDest + 1;
	}

	NiTriShapePtr pShape = NiNew NiTriShape( uiBottomVertCount*2, pPtsF, NULL, NULL, NULL, 0, NiGeometryData::NBT_METHOD_NONE, uiTriCount, pIndex );

	NiMaterialPropertyPtr pMapProp = NiNew NiMaterialProperty;
	pMapProp->SetAlpha( pInfo->s_fAlpha );
	pMapProp->SetEmittance( pInfo->s_Color );
	pShape->AttachProperty( pMapProp );	

	if( pInfo->s_fAlpha != 1.0f )
	{
		NiAlphaPropertyPtr pAlphaProp = NiNew NiAlphaProperty;
		pAlphaProp->SetAlphaBlending( true );
		pAlphaProp->SetAlphaTesting( false );
		pShape->AttachProperty( pAlphaProp );
	}

	if( pInfo->s_bTwoSide == true )
	{
		NiStencilPropertyPtr pStencilProp = NiNew NiStencilProperty;
		pStencilProp->SetDrawMode( NiStencilProperty::DRAW_BOTH );
		pShape->AttachProperty( pStencilProp );
	}

	if( pInfo->s_bWireFrame == true )
	{
		NiWireframePropertyPtr pWireProp = NiNew NiWireframeProperty;
		pWireProp->SetWireframe( true );
		pShape->AttachProperty( pWireProp );
	}

	pShape->UpdateProperties();
	pShape->Update( 0.0f );
	return NiDynamicCast( NiGeometry, pShape );
}


void CsGeomBox::ResetVertexPos( NiGeometry* pGeom, NiPoint2 ptLeftTop, NiPoint2 ptRightBottom )
{
	assert_cs( pGeom != NULL );

	NiPoint3* pPos = pGeom->GetVertices();
	switch( pGeom->GetVertexCount() )
	{
	case 8:
		{
			pPos[ 0 ].x = ptLeftTop.x;
			pPos[ 0 ].y = ptLeftTop.y;

			pPos[ 1 ].x = ptRightBottom.x;
			pPos[ 1 ].y = ptLeftTop.y;

			pPos[ 2 ].x = ptRightBottom.x;
			pPos[ 2 ].y = ptRightBottom.y;

			pPos[ 3 ].x = ptLeftTop.x;
			pPos[ 3 ].y = ptRightBottom.y;	

			pPos[ 4 ].x = ptLeftTop.x;
			pPos[ 4 ].y = ptLeftTop.y;

			pPos[ 5 ].x = ptRightBottom.x;
			pPos[ 5 ].y = ptLeftTop.y;

			pPos[ 6 ].x = ptRightBottom.x;
			pPos[ 6 ].y = ptRightBottom.y;

			pPos[ 7 ].x = ptLeftTop.x;
			pPos[ 7 ].y = ptRightBottom.y;
		}
		break;
	case 12:
		{			
			float dx = ( ptRightBottom.x - ptLeftTop.x )*0.5f;
			float dy = ( ptRightBottom.y - ptLeftTop.y )*0.5f;

			pPos[ 0 ].x = ptLeftTop.x;			pPos[ 0 ].y = ptLeftTop.y;
			pPos[ 1 ].x = ptRightBottom.x;		pPos[ 1 ].y = ptLeftTop.y;
			pPos[ 2 ].x = pPos[ 1 ].x + dx;		pPos[ 2 ].y = pPos[ 1 ].y + dy;
			pPos[ 3 ].x = pPos[ 2 ].x - dx;		pPos[ 3 ].y = pPos[ 2 ].y + dy;
			pPos[ 4 ].x = ptLeftTop.x;			pPos[ 4 ].y = ptRightBottom.y;
			pPos[ 5 ].x = pPos[ 4 ].x - dx;		pPos[ 5 ].y = pPos[ 4 ].y - dy;

			pPos[ 6 ].x = pPos[ 0 ].x;			pPos[ 6 ].y = pPos[ 0 ].y;
			pPos[ 7 ].x = pPos[ 1 ].x;			pPos[ 7 ].y = pPos[ 1 ].y;
			pPos[ 8 ].x = pPos[ 2 ].x;			pPos[ 8 ].y = pPos[ 2 ].y;
			pPos[ 9 ].x = pPos[ 3 ].x;			pPos[ 9 ].y = pPos[ 3 ].y;
			pPos[ 10 ].x = pPos[ 4 ].x;			pPos[ 10 ].y = pPos[ 4 ].y;
			pPos[ 11 ].x = pPos[ 5 ].x;			pPos[ 11 ].y = pPos[ 5 ].y;
		}
		break;
	case 16:
		{			
			float dx = ( ptRightBottom.x - ptLeftTop.x )*0.5f;
			float dy = ( ptRightBottom.y - ptLeftTop.y )*0.5f/1.414f;
			float dy2 = ( ptRightBottom.y - ptLeftTop.y ) - dy*2.0f;

			pPos[ 0 ].x = ptLeftTop.x;			pPos[ 0 ].y = ptLeftTop.y;
			pPos[ 1 ].x = ptRightBottom.x;		pPos[ 1 ].y = ptLeftTop.y;
			pPos[ 2 ].x = pPos[ 1 ].x + dx;		pPos[ 2 ].y = pPos[ 1 ].y + dy;
			pPos[ 3 ].x = pPos[ 2 ].x;			pPos[ 3 ].y = pPos[ 2 ].y + dy2;
			pPos[ 4 ].x = pPos[ 3 ].x - dx;		pPos[ 4 ].y = pPos[ 3 ].y + dy;
			pPos[ 5 ].x = ptLeftTop.x;			pPos[ 5 ].y = ptRightBottom.y;
			pPos[ 6 ].x = pPos[ 5 ].x - dx;		pPos[ 6 ].y = pPos[ 5 ].y - dy;
			pPos[ 7 ].x = pPos[ 6 ].x;			pPos[ 7 ].y = pPos[ 6 ].y - dy2;

			pPos[ 8 ].x = pPos[ 0 ].x;			pPos[ 8 ].y = pPos[ 0 ].y;
			pPos[ 9 ].x = pPos[ 1 ].x;			pPos[ 9 ].y = pPos[ 1 ].y;
			pPos[ 10 ].x = pPos[ 2 ].x;			pPos[ 10 ].y = pPos[ 2 ].y;
			pPos[ 11 ].x = pPos[ 3 ].x;			pPos[ 11 ].y = pPos[ 3 ].y;
			pPos[ 12 ].x = pPos[ 4 ].x;			pPos[ 12 ].y = pPos[ 4 ].y;
			pPos[ 13 ].x = pPos[ 5 ].x;			pPos[ 13 ].y = pPos[ 5 ].y;
			pPos[ 14 ].x = pPos[ 6 ].x;			pPos[ 14 ].y = pPos[ 6 ].y;
			pPos[ 15 ].x = pPos[ 7 ].x;			pPos[ 15 ].y = pPos[ 7 ].y;
		}
		break;
	default:
		assert_cs( false );
	}		

	pGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pGeom->Update( 0.0f );
}

void CsGeomBox::ResetVertexPos( NiGeometry* pGeom, NiPoint3 ptLeftTopMinZ, NiPoint3 ptRightBottomMaxZ )
{
	assert_cs( pGeom != NULL );
	assert_cs( pGeom->GetVertexCount() == 8 );
	NiPoint3* pPos = pGeom->GetVertices();

	pPos[ 0 ] = ptLeftTopMinZ;

	pPos[ 1 ].x = ptRightBottomMaxZ.x;
	pPos[ 1 ].y = ptLeftTopMinZ.y;
	pPos[ 1 ].z = ptLeftTopMinZ.z;

	pPos[ 2 ].x = ptRightBottomMaxZ.x;
	pPos[ 2 ].y = ptRightBottomMaxZ.y;
	pPos[ 2 ].z = ptLeftTopMinZ.z;

	pPos[ 3 ].x = ptLeftTopMinZ.x;
	pPos[ 3 ].y = ptRightBottomMaxZ.y;
	pPos[ 3 ].z = ptLeftTopMinZ.z;	

	pPos[ 4 ].x = ptLeftTopMinZ.x;
	pPos[ 4 ].y = ptLeftTopMinZ.y;
	pPos[ 4 ].z = ptRightBottomMaxZ.z;

	pPos[ 5 ].x = ptRightBottomMaxZ.x;
	pPos[ 5 ].y = ptLeftTopMinZ.y;
	pPos[ 5 ].z = ptRightBottomMaxZ.z;

	pPos[ 6 ] = ptRightBottomMaxZ;

	pPos[ 7 ].x = ptLeftTopMinZ.x;
	pPos[ 7 ].y = ptRightBottomMaxZ.y;
	pPos[ 7 ].z = ptRightBottomMaxZ.z;	

	pGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pGeom->Update( 0.0f );
}

void CsGeomBox::MoveDeltaPos( NiGeometry* pGeom, NiPoint2 vDelta )
{
	NiPoint3* pPos = pGeom->GetVertices();
	int nCount = pGeom->GetVertexCount();

	for( int i=0; i<nCount; ++i )
	{
		pPos[ i ].x += vDelta.x;
		pPos[ i ].y += vDelta.y;
	}
	pGeom->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pGeom->Update( 0.0f );
}