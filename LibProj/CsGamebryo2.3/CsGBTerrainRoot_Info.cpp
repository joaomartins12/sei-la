
#include "stdafx.h"
#include "CsGBTerrainRoot.h"

//==========================================================================================
// RootInfo

void CsGBTerrainRoot::sTR_PATHINFO::Delete()
{
	s_PathInfo.Delete();
	SAFE_DELETE_ARRAY( s_pType );

	s_nDataTotalCount = 0;
}

void CsGBTerrainRoot::sTR_PATHINFO::Reset()
{
	Delete();
	s_PathInfo.Reset();

	s_nDataTotalCount = 0;
}

void CsGBTerrainRoot::sFIELD_INFO::Delete()
{
	SAFE_DELETE_ARRAY( s_pfHeightField );
}

void CsGBTerrainRoot::sINFO::Reset()
{
	s_dwMapResVersion = MAP_RES_VERSION;
	s_dwMapID = 0;

	s_nXVertCount = 0;
	s_nXVertCountInNode = 0;

	s_nYVertCount = 0;
	s_nYVertCountInNode = 0;
	s_fHeightRate = 0.0f;

	// Grid
	s_nGridSize = 0;							// 한 패치의 그리드 넓이
	s_nTreeLevel = 0;							// 쿼드트리 레벨
	s_nNodeCountInSide = 0;						// 한변에서의 노드 갯수
	s_nTotalLeafCount = 0;						// 총 리프 갯수 ( 빈노드 포함 )

	memset( s_szHeightMapFile, 0, MAX_PATH );

	s_dwUniqObjectID = OBJECT_UNIQ_ID_START;
	s_nShadowMPPG = 4;

	memset( s_Dummy2, 0, 6 );

	// Fog
	s_FogColor = NiColor( 0.1f, 0.1f, 0.1f );
	s_fFogStart = 10000.0f;
	s_fFogEndTerrain = 30000.0f;
	s_fFogEndObject = 20000.0f;
	s_fClipObject_Far = 15000.0f;
	s_fClipTerrain_Far = 25000.0f;

	// Cell
	s_nClipCellObject = 20;
	s_nCellObject_Size = 500;

	s_BackGroundColor = NiColor( 0.2f, 0.2f, 0.2f );

	//====================================================================================
	// 더미
	//====================================================================================
	memset( s_Dummy, 0, 32 );
	memset( s_Dummy2, 0, 6 );

	// Init Size
	s_nInitMapCoverSize = 0;
	s_nInitMapSize = 0;
	s_fDetailMapSize = 0;

	// BaseMap
	s_nBaseMPPG = 0;

	// AlphaMap
	s_nAlphaMPPG = 0;

	// EffectMap
	s_nEffectMPPG = 0;

	// ==== ShadowColor
	s_ShadowColor = NiColor( 0.2f, 0.2f, 0.2f );
}

void CsGBTerrainRoot::sINFO::WorldPosToTexturePixel( int* xPixel, int* yPixel, NiPoint2 ptWorld, int nLeafIndex, int nMPPG )
{
	NiPoint2 ptLocal = ptWorld - nsCsGBTerrain::g_pCurRoot->GetNode( nLeafIndex )->GetNiTriInfo()->s_ptOffsetPos;
	NiPoint2 ptUVMap = NiPoint2( ( float(ptLocal.x+s_nGridSize)/(float)s_nInitMapCoverSize),
		1.0f - ( (float)( ptLocal.y+s_nGridSize )/(float)s_nInitMapCoverSize ) );
	// Map UV좌표를 픽셀 좌표로
	*xPixel = (int)( ptUVMap.x * s_nInitMapSize * nMPPG );
	*yPixel = (int)( ptUVMap.y * s_nInitMapSize * nMPPG );
}

void CsGBTerrainRoot::sINFO::TexturePixelToWorldPos( float* xPos, float* yPos, int xPixel, int yPixel, int nLeafIndex, int nMPPG )
{
	CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( nLeafIndex );

	// 픽셀 좌표를 uv 좌표로
	float fMsize = (float)( s_nInitMapSize * nMPPG );
	NiPoint2 ptUVMap = NiPoint2( (float)xPixel/fMsize, (float)yPixel/fMsize );
	// uv 좌표를 로칼 좌표로
	NiPoint2 ptLocal = NiPoint2( ptUVMap.x*s_nInitMapCoverSize - s_nGridSize,
		( 1.0f - ptUVMap.y )*s_nInitMapCoverSize - s_nGridSize );
	// 로칼 좌표를 월드 좌표로
	NiPoint2 ptWorld = ptLocal + nsCsGBTerrain::g_pCurRoot->GetNode( nLeafIndex )->GetNiTriInfo()->s_ptOffsetPos;
	*xPos = ptWorld.x;
	*yPos = ptWorld.y;
}

void CsGBTerrainRoot::sINFO::CopyTextureBorder( int nMPPG, NiPixelData** ppPixelDataArray )
{
	assert_cs( ppPixelDataArray != NULL );

	int nx, ny;
	int nStride = 0;
	int nWidth, nHeight;
	BYTE* pDestData = NULL;
	BYTE* pSrcData = NULL;

	for( int i=0; i<s_nTotalLeafCount; ++i )
	{
		if( ppPixelDataArray[ i ] != NULL )
		{
			nStride = ppPixelDataArray[ i ]->GetPixelStride();
			nWidth = ppPixelDataArray[ i ]->GetWidth();
			nHeight = ppPixelDataArray[ i ]->GetHeight();
			break;
		}
	}
	if( nStride == 0 )
		return;

	// 위에 해주는것은 나중에 적용되기 때문에 2번해줘야 한다
	for( int nLoop=0; nLoop<2; ++nLoop )
	{
		for( int i=0; i<s_nTotalLeafCount; ++ i )
		{
			if( ppPixelDataArray[ i ] == NULL )
				continue;

			nx = i%s_nNodeCountInSide;
			ny = i/s_nNodeCountInSide;		

			pDestData = ppPixelDataArray[ i ]->GetPixels();
			// 좌측
			if( ( nx > 0 )&&( ppPixelDataArray[ i - 1 ] != NULL ) )
			{
				pSrcData = ppPixelDataArray[ i - 1 ]->GetPixels();
				for( int i=0; i<nHeight; ++i )
				{
					for( int bp=0; bp<nStride; ++bp )
					{
						int nFigure = GetTextureLeftFigure( nMPPG );
						BYTE rb = pSrcData[ ( i*nWidth + GetTextureRightFigure( nMPPG ) )*nStride + bp ];
						for( int rp=0; rp<nFigure; ++rp )
						{
							pDestData[ ( i*nWidth + rp )*nStride + bp ] = rb;
						}
					}
				}
			}
			// 우측
			if( ( nx < s_nNodeCountInSide - 1 )&&( ppPixelDataArray[ i + 1 ] != NULL ) )
			{
				pSrcData = ppPixelDataArray[ i + 1 ]->GetPixels();
				for( int i=0; i<nHeight; ++i )
				{
					for( int bp=0; bp<nStride; ++bp )
					{
						int nFigure = GetTextureRightFigure( nMPPG );
						BYTE rb = pSrcData[ ( i*nWidth + GetTextureLeftFigure( nMPPG ) )*nStride + bp ];
						for( int rp=nFigure+1; rp<nWidth; ++rp )
						{
							pDestData[ ( i*nWidth + rp )*nStride + bp ] = rb;
						}
					}
				}
			}
			// 아래쪽
			if( ( ny > 0 )&&( ppPixelDataArray[ i - s_nNodeCountInSide ] != NULL ) )
			{
				pSrcData = ppPixelDataArray[ i - s_nNodeCountInSide ]->GetPixels();
				for( int i=0; i<nWidth; ++i )
				{
					for( int bp=0; bp<nStride; ++bp )
					{
						int nFigure = GetTextureBottomFigure( nMPPG );
						BYTE rb = pSrcData[ ( GetTextureTopFigure( nMPPG )*nWidth + i )*nStride + bp ];
						for( int rp=nFigure+1; rp<nHeight; ++rp )
						{
							pDestData[ ( rp*nWidth + i )*nStride + bp ] = rb;
						}
					}				
				}
			}
			// 위쪽
			if( ( ny < s_nNodeCountInSide - 1 )&&( ppPixelDataArray[ i + s_nNodeCountInSide ] != NULL ) )
			{
				pSrcData = ppPixelDataArray[ i + s_nNodeCountInSide ]->GetPixels();
				for( int i=0; i<nWidth; ++i )
				{
					for( int bp=0; bp<nStride; ++bp )
					{
						int nFigure = GetTextureTopFigure( nMPPG );
						BYTE rb = pSrcData[ ( GetTextureBottomFigure( nMPPG )*nWidth + i )*nStride + bp ];
						for( int rp=0; rp<nFigure; ++rp )
						{
							pDestData[ ( rp*nWidth + i )*nStride + bp ] = rb;
						}
					}
				}
			}

			ppPixelDataArray[ i ]->MarkAsChanged();
		}
	}	
}

