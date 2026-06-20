
#include "stdafx.h"
#include "CsGBTerrainMake.h"

CsGBTerrainMake::CsGBTerrainMake()
{
}

CsGBTerrainMake::~CsGBTerrainMake()
{

}

void CsGBTerrainMake::Delete()
{

}

void CsGBTerrainMake::Create( LPCTSTR szBaseDetailMap )
{
	_CreateBaseNiNode( szBaseDetailMap );
	_CreateBaseNiNode_ByTool( szBaseDetailMap );

	nsCsGBTerrain::g_pCurRoot->Alloc_HField_Normal();
	nsCsGBTerrain::g_pCurRoot->MakeOrgHeightField();
	nsCsGBTerrain::g_pCurRoot->MakeHeightField();
}

NiTriStripsPtr CsGBTerrainMake::_CreateCsNiTriPatch( nsCSGBFUNC::sTERRAIN_BOUND& Bound )
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	assert_cs( ( ( pInfo->s_nXVertCount - 1 )*pInfo->s_nGridSize ) == ( ( pInfo->s_nXVertCountInNode - 1 )*pInfo->s_nGridSize )*(int)sqrt( (float)pInfo->s_nTotalLeafCount ) );

	int nVertCnt = pInfo->s_nXVertCountInNode * pInfo->s_nYVertCountInNode;
	NiPoint3* pPtsF = NiNew NiPoint3[ nVertCnt ];
	NiPoint2* pTUVsF = NiNew NiPoint2[ nVertCnt*2 ];

	for( int j=0; j<pInfo->s_nYVertCountInNode; ++j )
	{
		for( int i=0; i<pInfo->s_nXVertCountInNode; ++i )
		{
			int nIndex = j*pInfo->s_nXVertCountInNode + i;
			int nTempIndex = pInfo->s_nXVertCountInNode * pInfo->s_nYVertCountInNode + nIndex;

			// Position
			NiPoint3 pos = NiPoint3( (float)pInfo->s_nGridSize*i, (float)pInfo->s_nGridSize*j, 0.0f );
			pPtsF[ nIndex ] = pos;

			// === 바운드 영역 체크
			if( Bound.s_ptMin.x > pPtsF[ nIndex ].x )
				Bound.s_ptMin.x = pPtsF[ nIndex ].x;
			if( Bound.s_ptMax.x < pPtsF[ nIndex ].x )
				Bound.s_ptMax.x = pPtsF[ nIndex ].x;
			if( Bound.s_ptMin.y > pPtsF[ nIndex ].y )
				Bound.s_ptMin.y = pPtsF[ nIndex ].y;
			if( Bound.s_ptMax.y < pPtsF[ nIndex ].y )
				Bound.s_ptMax.y = pPtsF[ nIndex ].y;

			// UV set 0
			pTUVsF[ nIndex ] = NiPoint2( pInfo->s_fDetailMapSize*i, -pInfo->s_fDetailMapSize*j );
			// UV set 1
			float fInitMapEmptySize = (float)pInfo->s_nGridSize;
			pTUVsF[ nTempIndex ] = NiPoint2( (float)( pInfo->s_nGridSize*i+fInitMapEmptySize )/(float)pInfo->s_nInitMapCoverSize,
				1.0f - ( (float)( pInfo->s_nGridSize*j+fInitMapEmptySize )/(float)pInfo->s_nInitMapCoverSize ) );
		}
	}

	int nXPlaneCountInNode = pInfo->s_nXVertCountInNode - 1;
	int nYPlaneCountInNode = pInfo->s_nYVertCountInNode - 1;

	// Tri Strips
	unsigned short usStrips = 1;
	unsigned short* pStripLength = NiAlloc( unsigned short, usStrips );
	int nSLength = 0;
	for( unsigned short i=0; i<usStrips; ++i )
	{
		pStripLength[ i ] = ( nXPlaneCountInNode + 2 )*nYPlaneCountInNode*2 - 2;
		nSLength += pStripLength[ i ];
	}
	unsigned short* pStripLists = NiAlloc( unsigned short, nSLength );
	unsigned short nSLIndex = 0;
	int ix = pInfo->s_nXVertCountInNode - 1;
	for( int j=0; j<nYPlaneCountInNode; ++j )
	{
		if( j%2 == 0 )
		{
			if( j==0 )
			{
				for( int i=0; i<pInfo->s_nXVertCountInNode; ++i )
				{
					pStripLists[ nSLIndex + 0 ] = (j+1)*pInfo->s_nXVertCountInNode + i;
					pStripLists[ nSLIndex + 1 ] = j*pInfo->s_nXVertCountInNode + i;
					nSLIndex += 2;
				}
			}
			else
			{
				for( int i=1; i<pInfo->s_nXVertCountInNode; ++i )
				{
					pStripLists[ nSLIndex + 0 ] = (j+1)*pInfo->s_nXVertCountInNode + i;
					pStripLists[ nSLIndex + 1 ] = j*pInfo->s_nXVertCountInNode + i;
					nSLIndex += 2;
				}
			}
			
			if( j<nYPlaneCountInNode-1)
			{
				// 끝에 더미 삼각형				
				pStripLists[ nSLIndex + 0 ] = (j+1)*pInfo->s_nXVertCountInNode + ix;
				pStripLists[ nSLIndex + 1 ] = (j+1)*pInfo->s_nXVertCountInNode + ix;
				pStripLists[ nSLIndex + 2 ] = (j+1)*pInfo->s_nXVertCountInNode + ix;
				pStripLists[ nSLIndex + 3 ] = (j+2)*pInfo->s_nXVertCountInNode + ix;
				nSLIndex += 4;
			}
		}
		else
		{
			for( int i=1; i<pInfo->s_nXVertCountInNode; ++i )
			{
				pStripLists[ nSLIndex + 0 ] = j*pInfo->s_nXVertCountInNode + ix-i;
				pStripLists[ nSLIndex + 1 ] = (j+1)*pInfo->s_nXVertCountInNode + ix-i;
				nSLIndex += 2;
			}
			if( j<nYPlaneCountInNode-1)
			{
				// 끝에 더미 삼각형
				pStripLists[ nSLIndex + 0 ] = (j+1)*pInfo->s_nXVertCountInNode;
				pStripLists[ nSLIndex + 1 ] = (j+1)*pInfo->s_nXVertCountInNode;
				pStripLists[ nSLIndex + 2 ] = (j+2)*pInfo->s_nXVertCountInNode;
				pStripLists[ nSLIndex + 3 ] = (j+1)*pInfo->s_nXVertCountInNode;
				nSLIndex += 4;
			}
		}
	}

	assert_cs( nSLIndex == nSLength );

	NiTriStripsPtr pStrips = NiNew NiTriStrips( pInfo->s_nXVertCountInNode * pInfo->s_nYVertCountInNode, pPtsF,
		NULL, NULL, pTUVsF, 2, NiGeometryData::NBT_METHOD_NONE,
		nSLength-2, usStrips, pStripLength, pStripLists );

	return pStrips;
}

void CsGBTerrainMake::_AttachProperty( NiTriStripsPtr pStrips, LPCTSTR szBaseDetailMap )
{
	// Base Detail Map
	assert_cs( _taccess_s( szBaseDetailMap, 0 ) == 0 );
	char cTexPath[ MAX_PATH ] = {0, };
	W2M( cTexPath, szBaseDetailMap, MAX_PATH );
	assert_cs( pStrips->GetProperty( NiTexturingProperty::GetType() ) == NULL );

	NiTexturingProperty* pTexProp = nsCsGBTerrain::g_ObjectResMgr.GetTexture();
	pTexProp->SetShaderMap( DETAIL_MAP_0_SHADER_NTM, nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( cTexPath ) );

	pStrips->AttachProperty( pTexProp );
	pStrips->UpdateProperties();
}

void CsGBTerrainMake::_AttachEffect( NiTriStripsPtr pStrips )
{
	if( pStrips->GetActiveMaterial() == NULL )
	{
		pStrips->SetShader( GET_SHADER( "TR_Base" ) );
	}
	pStrips->UpdateEffects();
}

void CsGBTerrainMake::_CreateBaseNiNode( LPCTSTR szBaseDetailMap )
{
	nsCSGBFUNC::sTERRAIN_BOUND Bound;
	NiTriStripsPtr pStrips = _CreateCsNiTriPatch( Bound );

	// Dynamic
	pStrips->GetModelData()->SetConsistency( NiGeometryData::VOLATILE );

	_AttachProperty( pStrips, szBaseDetailMap );
	_AttachEffect( pStrips );	

	nsCsGBTerrain::g_pCurRoot->SetBaseNiNode( pStrips );
}

void CsGBTerrainMake::_CreateBaseNiNode_ByTool( LPCTSTR szBaseDetailMap )
{
	nsCSGBFUNC::sTERRAIN_BOUND Bound;
	NiTriStripsPtr pStrips = _CreateCsNiTriPatch( Bound );

	_CreateNode( pStrips, Bound, szBaseDetailMap );
	
	pStrips = 0;
}

void CsGBTerrainMake::_CreateNode( NiTriStripsPtr pStrips, nsCSGBFUNC::sTERRAIN_BOUND Bound, LPCTSTR szBaseDetailMap )
{
	nsCSGBFUNC::sTERRAIN_BOUND WorldBound;
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	int nAlphaMapIndex = 0;

	// ===== Y 축
	for( int j=0; j < pInfo->s_nNodeCountInSide; ++j )
	{		
		for( int i=0; i< pInfo->s_nNodeCountInSide; ++i )
		{
			int nXPlaneCountInNode	= pInfo->s_nXVertCountInNode - 1;
			int nYPlaneCountInNode	= pInfo->s_nYVertCountInNode - 1;		

			CsGBTerrainLeaf::sNiTriINFO NiTriInfo;
			NiTriInfo.Reset();

			// Pos Offset
			NiTriInfo.s_ptOffsetPos = NiPoint2(	(float)( nXPlaneCountInNode * pInfo->s_nGridSize * i ),
												(float)( nYPlaneCountInNode * pInfo->s_nGridSize * j )	);

			// VertCount
			NiTriInfo.s_nVertCount = pInfo->s_nXVertCountInNode * pInfo->s_nYVertCountInNode;
			//NiTriInfo.s_nVertWidth = pInfo->s_nXVertCountInNode;
			//NiTriInfo.s_fInverseGridSize = 1.0f/(float)pInfo->s_nGridSize;
			NiTriInfo.s_usNiTerrainIndex = j*pInfo->s_nNodeCountInSide + i;

			// Height Field
			int nVertCount = NiTriInfo.s_nVertCount;
			assert_cs( NiTriInfo.s_pfHeight == NULL );
			NiTriInfo.s_pfHeight = csnew float[ nVertCount ];
			memset( NiTriInfo.s_pfHeight, 0, sizeof( float )*nVertCount );

			// Bound
			WorldBound.s_ptMin.x = Bound.s_ptMin.x + NiTriInfo.s_ptOffsetPos.x;
			WorldBound.s_ptMin.y = Bound.s_ptMin.y + NiTriInfo.s_ptOffsetPos.y;
			WorldBound.s_ptMax.x = Bound.s_ptMax.x + NiTriInfo.s_ptOffsetPos.x;
			WorldBound.s_ptMax.y = Bound.s_ptMax.y + NiTriInfo.s_ptOffsetPos.y;

			// Detatil Map0
			W2M( NiTriInfo.s_cDetail_Map0, szBaseDetailMap, MAX_PATH );

			// Leaf에서 NiNode생성
			CsNiNodePtr pNiNode = NiNew CsNiNode;
			pNiNode->AttachChild( NiDynamicCast( NiTriStrips, pStrips->CreateDeepCopy() ) );

			CsGBTerrainLeaf* pTerrainLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i, j );
			pTerrainLeaf->MakeNiTerrain( pNiNode, &NiTriInfo );
			pTerrainLeaf->CalBound( WorldBound );			
		}
	}
}

bool CsGBTerrainMake::ApplyHeightMap( LPCTSTR szMapPath )
{
	sCS_BMPDATA BmpData;
	if( CsBmp::GetBmpData_GrayMap( szMapPath, &BmpData ) == false )
		return false;

	CsGBTerrainLeaf* pLeaf = NULL;
	for( int i=0; i<nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount; ++i )
	{
		pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{
			_ApplyHeightMap( &BmpData, pLeaf );
		}
	}
	nsCsGBTerrain::g_pCurRoot->MakeHeightField();

	return true;
}

void CsGBTerrainMake::_ApplyHeightMap( sCS_BMPDATA* pBmpData, CsGBTerrainLeaf* pNode )
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();	
	CsGBTerrainLeaf::sNiTriINFO* pNiTriInfo = pNode->GetNiTriInfo();
	assert_cs( pNiTriInfo->s_pfHeight != NULL );
	NiTriStrips* pTriObject = (NiTriStrips*)pNode->GetNiNode()->GetAt( 0 );
	assert_cs( NiIsKindOf( NiTriStrips, pTriObject ) == true );
	DWORD dwVertCount = pTriObject->GetVertexCount();
	assert_cs( dwVertCount == pNiTriInfo->s_nVertCount );
	NiPoint3* pPos = pTriObject->GetVertices();

	int nTotalStructWidth = (pInfo->s_nXVertCountInNode-1)*pInfo->s_nGridSize*pInfo->s_nNodeCountInSide;
	int nTotalStructHeight = (pInfo->s_nYVertCountInNode-1)*pInfo->s_nGridSize*pInfo->s_nNodeCountInSide;

	int nTexWidth = pBmpData->s_InfoHeader.biWidth;
	//float fXTexTemp = ( pBmpData->s_InfoHeader.biWidth -1 ) * 0.5f;
	//float fYTexTemp = ( pBmpData->s_InfoHeader.biHeight -1 ) * 0.5f;
	float fXWidthTemp = ( pBmpData->s_InfoHeader.biWidth -1 ) / (float)nTotalStructWidth;
	float fYWidthTemp = ( pBmpData->s_InfoHeader.biHeight -1 ) / (float)nTotalStructHeight;
	float fXPosRate, fYPosRate; 
	int nXPos, nYPos;
	BYTE* pData = pBmpData->s_pData;

	for( DWORD i=0; i<dwVertCount; ++i )
	{
		fXPosRate = ( pPos[ i ].x+pNiTriInfo->s_ptOffsetPos.x ) * fXWidthTemp;
		nXPos = (int)fXPosRate;
		fXPosRate = fXPosRate - nXPos;

		fYPosRate = ( pPos[ i ].y+pNiTriInfo->s_ptOffsetPos.y ) * fYWidthTemp;
		nYPos = (int)fYPosRate;
		fYPosRate = fYPosRate - nYPos;
		
		pNiTriInfo->s_pfHeight[ i ] = ( pData[ nYPos*nTexWidth + nXPos ]*( 1.0f-fXPosRate )*(1.0f-fYPosRate) ) +
			( pData[ nYPos*nTexWidth + nXPos+1 ]*( fXPosRate )*(1.0f-fYPosRate) ) +
			( pData[ (nYPos+1)*nTexWidth + nXPos ]*( 1.0f-fXPosRate )*(fYPosRate) ) +
			( pData[ (nYPos+1)*nTexWidth + nXPos+1 ]*( fXPosRate )*(fYPosRate) );

		pNiTriInfo->s_pfHeight[ i ] -= 123.0f;
		pNiTriInfo->s_pfHeight[ i ] *= nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fHeightRate;
		pPos[ i ].z = pNiTriInfo->s_pfHeight[ i ];
	}

	pTriObject->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );

	// 바운드 박스 재계산	
	pTriObject->GetModelBound().ComputeFromData( dwVertCount, pPos );

	pTriObject->Update( 0.0f );
}



