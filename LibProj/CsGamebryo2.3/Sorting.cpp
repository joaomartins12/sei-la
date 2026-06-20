#include "stdafx.h"
#include "Sorting.h"
#include <malloc.h>

CsGBSorting								g_Sorting;

void CsGBSorting::Delete()
{
	SAFE_DELETE_ARRAY(m_ppkItems);
	SAFE_DELETE_ARRAY(m_pfDepths);
	m_nNumItems = 0;
	m_nMaxItems = 0;
	m_vpAlphaGeom.Destroy();	

	int nDestID = m_nMaxTRID + 1 - m_nMinTRID;
	for( int i=0; i<nDestID; ++i )
	{
		if( m_vpGetterInfoArray[ i ] )
		{
			int ecnt = m_vpGetterInfoArray[ i ]->s_vpElement.GetAllocCount();
			for( int e=0; e<ecnt; ++e )
			{
				m_vpGetterInfoArray[ i ]->s_vpElement[ e ].s_vpGeom.Destroy();
			}
			m_vpGetterInfoArray[ i ]->s_vpElement.Destroy();
			delete ( m_vpGetterInfoArray[ i ] );
		}
	}
	m_vpGetterInfoArray.Destroy();
	m_vpNoClipping.Destroy();
	m_vpF1.Destroy();
}

void CsGBSorting::PreMapLoad()
{
	Delete();

	m_vpAlphaGeom.Init( 20, AP_DOUBLE );
	m_vpNoClipping.Init( 10, AP_ADD );
	m_vpF1.Init( 10, AP_ADD );
	_SetMax( 100 );	

	m_nMinTRID = INT_MAX;
	m_nMaxTRID = 0;
}

void CsGBSorting::PostMapLoad()
{
	assert_cs( m_vpGetterInfoArray.Size() == 0 );	

	if( nsCsGBTerrain::g_eCreateType == nsCsGBTerrain::CT_FILELOAD )
	{
		int nCount = m_nMaxTRID + 1 - m_nMinTRID;
		if( nCount <= 0 )
			return;

		m_vpGetterInfoArray.Init( nCount, AP_DOUBLE );
		m_vpGetterInfoArray.SetMemZero();
		m_vpGetterInfoArray.SetElementCount( nCount );
	}
	else
	{
		m_vpGetterInfoArray.Init( 100000, AP_DOUBLE );
		m_vpGetterInfoArray.SetMemZero();
		m_vpGetterInfoArray.SetElementCount( 100000 );
	}

	if( m_nMaxTRID == 0 )
	{
		m_nMinTRID = 0;
		m_nMaxTRID = 0;
	}
}

void  CsGBSorting::SetID( int nID )
{ 
	if( m_nMinTRID > nID ) 
		m_nMinTRID = nID; 
	if( m_nMaxTRID < nID ) 
		m_nMaxTRID = nID; 
}
//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CsGBSorting::_SetMax( int i )
{
	m_nMaxItems = i;
	SAFE_DELETE_ARRAY( m_ppkItems );
    m_ppkItems = csnew CGeometry*[ m_nMaxItems ];
    
    SAFE_DELETE_ARRAY(m_pfDepths);
    m_pfDepths =  csnew float[ m_nMaxItems ];
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CsGBSorting::Sort( NiCamera* pNiCamera )
{
	assert_cs( pNiCamera != NULL );
	m_nNumItems = (UINT)m_vpAlphaGeom.Size();
	if( m_nNumItems == 0 )
		return;

    if ( m_nNumItems > m_nMaxItems )
    {
        _SetMax( m_nNumItems );
    }

	NiPoint3 kViewDir = pNiCamera->GetWorldDirection();
	int nAlphaGeomCount = m_vpAlphaGeom.Size();
	assert_cs( (UINT)nAlphaGeomCount <= m_nMaxItems );

	for( int i=0; i<nAlphaGeomCount; ++i )
	{
		m_ppkItems[ i ] = m_vpAlphaGeom[ i ];
		if( m_vpAlphaGeom[ i ]->m_pGeometry != NULL )
			m_pfDepths[ i ] = m_vpAlphaGeom[ i ]->m_pGeometry->GetWorldBound().GetCenter() * kViewDir;
	}
    _SortObjectsByDepth(0, m_nNumItems - 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
void CsGBSorting::_SortObjectsByDepth(int l, int r)
{
	int i, j;
	float fPivot, fTemp;
	CGeometry* pkObjTemp;

    if (r > l)
    {
		i = l - 1;
        j = r + 1;
        fPivot = _ChoosePivot(l, r);

        for (;;)
        {
            do 
            {
                j--;
            } while (fPivot < m_pfDepths[j]);

            do
            {
                i++;
            } while (m_pfDepths[i] < fPivot);

            if (i < j)
            {
                pkObjTemp = m_ppkItems[i];
                m_ppkItems[i] = m_ppkItems[j];
                m_ppkItems[j] = pkObjTemp;
                fTemp = m_pfDepths[i];
                m_pfDepths[i] = m_pfDepths[j];
                m_pfDepths[j] = fTemp;
            }
            else
            {
                break;
            }
        }

        if (j == r)
        {
            _SortObjectsByDepth(l, j - 1);
        }
        else
        {
            _SortObjectsByDepth(l, j);
            _SortObjectsByDepth(j + 1, r);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////
float CsGBSorting::_ChoosePivot(int l, int r) const
{
    int m = (l + r) >> 1;

    if (m_pfDepths[l] < m_pfDepths[m])
    {
        if (m_pfDepths[m] < m_pfDepths[r])
        {
            return m_pfDepths[m];
        }
        else
        {
            if (m_pfDepths[l] < m_pfDepths[r])
                return m_pfDepths[r];
            else
                return m_pfDepths[l];
        }
    }
    else
    {
        if (m_pfDepths[l] < m_pfDepths[r])
        {
            return m_pfDepths[l];
        }
        else
        {
            if (m_pfDepths[m] < m_pfDepths[r])
                return m_pfDepths[r];
            else
                return m_pfDepths[m];
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////
//																							//
//																							//
//																							//
//////////////////////////////////////////////////////////////////////////////////////////////

void CsGBSorting::ClearGeomList()
{ 
	m_vpAlphaGeom.ClearElement(); 
}

void CsGBSorting::Set( CGeometry* pkGeo )
{ 
	SAFE_POINTER_RET( pkGeo );

	if( pkGeo->IsPlag( CGeometry::GP_ALPHA ) )
		m_vpAlphaGeom.PushBack( pkGeo );
	else
		pkGeo->Render();
}

bool CsGBSorting::empty()
{ 
	return ( m_vpAlphaGeom.Size() == 0 ); 
}

void CsGBSorting::Render( NiCamera* pNiCamera )
{
	Sort( pNiCamera );

	for( int i=m_nNumItems-1; i>-1; --i )
	{
		if( m_ppkItems[i]->m_pGeometry != NULL )
			m_ppkItems[ i ]->Render();
	}

	ClearGeomList();
}

void CsGBSorting::Render_NoClear()
{
	for( int i=m_nNumItems-1; i>-1; --i )
	{
		if( m_ppkItems[i]->m_pGeometry != NULL )
			m_ppkItems[ i ]->Render();
	}
}

void CsGBSorting::GetterTRObject( CsSortObj* pObj, int nGetID, int nType /*=GT_NONE*/ )
{
	// ID를 0번부터 오게 정렬
	int nGetterID = nGetID - m_nMinTRID;
	assert_cs( nGetterID >= 0 );

	// 오브젝트별 묶음 포인터를 가져온다
	// pGetter = 같은 오브젝트 종류는 같은 포인터를 공유 한다
	sGETTER_INFO* pGetter = m_vpGetterInfoArray[ nGetterID ];

	// 만약 기존에 메모리 할당이 된적 없다면
	if( pGetter == NULL )
	{
		// 메모리 할당
		pGetter = csnew sGETTER_INFO;
		pGetter->s_nType = nType;
		pGetter->s_vpElement.Init( 10, AP_DOUBLE );
		// 초기 갯수 할당
		for( int i=0; i<10; ++i )
		{
			pGetter->s_vpElement[ i ].s_vpGeom.Init( 10, AP_DOUBLE );
		}
		m_vpGetterInfoArray[ nGetterID ] = pGetter;
	}
	
	// 할당 되어 잇는것을 전부 사용 가능하게 한다.
	//pGetter->s_vpElement.SetAllocEnalbe();

	int nNodeCount = pObj->m_vpGeom.Size();
	for( int n=0; n<nNodeCount; ++n )
	{
		if( pGetter->s_vpElement.IsExistAlloc( n ) == false )
		{
			int nStart = pGetter->s_vpElement.GetAllocCount();
			pGetter->s_vpElement.Realloc();
			int nEnd = pGetter->s_vpElement.GetAllocCount();
			for( int i=nStart; i<nEnd; ++i )
			{
				pGetter->s_vpElement[ i ].s_vpGeom.Init( 10, AP_DOUBLE );
			}
		}		
		pGetter->s_vpElement[ n ].s_vpGeom.PushBack( pObj->m_vpGeom[ n ] );
	}
	pGetter->s_vpElement.SetElementCount( nNodeCount );

	switch( nType )
	{
	case GT_F1:
		if( m_vpF1.IsExistData( nGetterID ) == false )
			m_vpF1.PushBack( nGetterID );
		break;
	case GT_NONE:
		break;		
	case GT_NOCLIPPING:
		if( m_vpNoClipping.IsExistData( nGetterID ) == false )
			m_vpNoClipping.PushBack( nGetterID );
		break;
	default:
		assert_cs( false );
	}
}

void CsGBSorting::SetShader_GetterTRObject( NiMaterial* pShader )
{
	int nGetterCount = m_vpGetterInfoArray.Size();
	int nElementCount;
	int nGeomCount;
	sGETTER_INFO* pGetterInfo;
	sELEMET_INFO* pElementInfo;
	for( int i=0; i<nGetterCount; ++i )
	{
		pGetterInfo = m_vpGetterInfoArray[ i ];
		if( pGetterInfo )
		{
			nElementCount = pGetterInfo->s_vpElement.Size();
			for( int e=0; e<nElementCount; ++e )
			{
				pElementInfo = &pGetterInfo->s_vpElement[ e ];
				nGeomCount = pElementInfo->s_vpGeom.Size();
				for( int g=0; g<nGeomCount; ++g )
				{

					pElementInfo->s_vpGeom[ g ]->m_pGeometry->ApplyAndSetActiveMaterial( pShader );
				}
			}
		}
	}
}

void CsGBSorting::RenderTRObject( bool bClearGetter )
{
	int nGetterCount = m_vpGetterInfoArray.Size();
	int nElementCount;
	int nGeomCount;
	sGETTER_INFO* pGetterInfo;
	sELEMET_INFO* pElementInfo;
	for( int i=0; i<nGetterCount; ++i )
	{
		pGetterInfo = m_vpGetterInfoArray[ i ];
		if( pGetterInfo )
		{
			switch( pGetterInfo->s_nType )
			{
			case GT_NONE:			
				{
					nElementCount = pGetterInfo->s_vpElement.Size();
					for( int e=0; e<nElementCount; ++e )
					{
						pElementInfo = &pGetterInfo->s_vpElement[ e ];
						nGeomCount = pElementInfo->s_vpGeom.Size();
						for( int g=0; g<nGeomCount; ++g )
						{
							g_Sorting.Set( pElementInfo->s_vpGeom[ g ] );							
						}
						if( bClearGetter == true )
						{
							pElementInfo->s_vpGeom.ClearElement();
						}
					}
					if( bClearGetter == true )
					{
						pGetterInfo->s_vpElement.ClearElement();
					}
				}
				break;
			case GT_F1:
			case GT_NOCLIPPING:
				break;
			default:
				assert_cs( false );
			}			
		}
	}
}

void CsGBSorting::ClearGetter()
{
	int nGetterCount = m_vpGetterInfoArray.Size();
	int nElementCount;
	sGETTER_INFO* pGetterInfo;
	sELEMET_INFO* pElementInfo;
	for( int i=0; i<nGetterCount; ++i )
	{
		pGetterInfo = m_vpGetterInfoArray[ i ];
		if( pGetterInfo )
		{
			nElementCount = pGetterInfo->s_vpElement.Size();
			for( int e=0; e<nElementCount; ++e )
			{						
				pElementInfo = &pGetterInfo->s_vpElement[ e ];
				pElementInfo->s_vpGeom.ClearElement();
			}
			pGetterInfo->s_vpElement.ClearElement();
		}
	}
	m_vpF1.ClearElement();
	m_vpNoClipping.ClearElement();
}

void CsGBSorting::RenderTRObject_Type( int nType, bool bClearGetter )
{
	CsVectorPB< int >* vpType;
	switch( nType )
	{	
	case GT_F1:
		vpType = &m_vpF1;
		break;			
	case GT_NOCLIPPING:
		vpType = &m_vpNoClipping;
		break;
	default:
		assert_cs( false );
	}

	sGETTER_INFO* pGetterInfo;
	sELEMET_INFO* pElementInfo;
	int nCount = vpType->Size();
	int nGeomCount, nElementCount;
	for( int p=0; p<nCount; ++p )
	{
		pGetterInfo = m_vpGetterInfoArray[ (*vpType)[ p ] ];
		assert_cs( pGetterInfo );
		nElementCount = pGetterInfo->s_vpElement.Size();
		for( int e=0; e<nElementCount; ++e )
		{
			pElementInfo = &pGetterInfo->s_vpElement[ e ];
			nGeomCount = pElementInfo->s_vpGeom.Size();
			for( int g=0; g<nGeomCount; ++g )
			{
				g_Sorting.Set( pElementInfo->s_vpGeom[ g ] );
			}
			if( bClearGetter == true )
			{
				pElementInfo->s_vpGeom.ClearElement();
			}
		}

		if( bClearGetter == true )
		{
			pGetterInfo->s_vpElement.ClearElement();
		}
	}

	if( bClearGetter == true )
	{
		vpType->ClearElement();
	}
}


