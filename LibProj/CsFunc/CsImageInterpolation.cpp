
#include "stdafx.h"
#include "CsImageInterpolation.h"

CsImageInterpolation::CsImageInterpolation( BYTE* pInData, int nSrcWidth, int nSrcHeight, int nBytePerPixel, bool bAutoDelete /*=true*/ )
{
	assert_cs( pInData != NULL );
	assert_cs( nSrcWidth > 0 );
	assert_cs( nSrcHeight > 0 );
	m_pInData = pInData;

	m_nSrcWidth = nSrcWidth;
	m_nSrcHeight = nSrcHeight;	
	m_nBytePerPixel = nBytePerPixel;

	m_pOutData = NULL;

	m_bAutoDelete = bAutoDelete;
}

CsImageInterpolation::~CsImageInterpolation()
{
	if( m_bAutoDelete == true )
		Delete();
}

void CsImageInterpolation::Delete()
{
	SAFE_DELETE_ARRAY( m_pOutData );
}

BYTE* CsImageInterpolation::Liner( int nOutWidth, int nOutHeight )
{
	assert_cs( m_pOutData == NULL );
	m_pOutData = csnew BYTE[ nOutWidth*nOutHeight*m_nBytePerPixel ];

	float u,v, du, dv;
	int x, y;
	
	for( int h=0; h<nOutHeight; ++h )
	{
		// 입력 좌표로 변경
		v = (float)h/(float)nOutHeight*m_nSrcHeight;
		y = (int)v;
		dv = v - y;
		for( int w=0; w<nOutWidth; ++w )
		{
			// 입력 좌표로 변경
			u = (float)w/(float)nOutWidth*m_nSrcWidth;
			x = (int)u;
			du = u - x;
			for( int bp=0; bp<m_nBytePerPixel; ++bp )
			{
				if( ( x < m_nSrcWidth - 1 )&&( y < m_nSrcHeight - 1 ) )
				{
					m_pOutData[ ( h*nOutWidth + w )*m_nBytePerPixel + bp ] = (BYTE)(
						( (1.0f-du)*(1.0f-dv)*m_pInData[ ( y*m_nSrcWidth+x )*m_nBytePerPixel + bp ] ) +
						( du*(1.0f-dv)*m_pInData[ ( y*m_nSrcWidth+(x+1) )*m_nBytePerPixel + bp ] ) +
						( (1.0f-du)*dv*m_pInData[ ( (y+1)*m_nSrcWidth+x )*m_nBytePerPixel + bp ] ) +
						( du*dv*m_pInData[ ( (y+1)*m_nSrcWidth+(x+1) )*m_nBytePerPixel + bp ] ) );
				}
				else if( x < m_nSrcWidth - 1 )
				{
					m_pOutData[ ( h*nOutWidth + w )*m_nBytePerPixel + bp ] = (BYTE)(
						( (1.0f-du)*m_pInData[ ( y*m_nSrcWidth+x )*m_nBytePerPixel + bp ] ) +
						( du*m_pInData[ ( y*m_nSrcWidth+(x+1) )*m_nBytePerPixel + bp ] ) );					
				}
				else if( y < m_nSrcHeight - 1 )
				{
					m_pOutData[ ( h*nOutWidth + w )*m_nBytePerPixel + bp ] = (BYTE)(
						( (1.0f-dv)*m_pInData[ ( y*m_nSrcWidth+x )*m_nBytePerPixel + bp ] ) +
						( dv*m_pInData[ ( (y+1)*m_nSrcWidth+x )*m_nBytePerPixel + bp ] ) );					
				}
				else
				{
					m_pOutData[ ( h*nOutWidth + w )*m_nBytePerPixel + bp ] =
						(BYTE)( (1.0f-du)*(1.0f-dv)*m_pInData[ ( y*m_nSrcWidth+x )*m_nBytePerPixel + bp ] );
				}
			}	
		}
	}
	return m_pOutData;
}