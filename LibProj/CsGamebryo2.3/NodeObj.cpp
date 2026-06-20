
#include "stdafx.h"
#include "NodeObj.h"

void CsSortObj::Delete()
{
	int nCount = m_vpGeom.Size();
	for( int i=0; i<nCount; ++i )
	{
		CGeometry::DelInstance( m_vpGeom[ i ] );
	}
	m_vpGeom.Destroy();
	m_bTempAlpha = false;
}

void CsSortObj::Delete_IsFlag( DWORD dwPlag )
{
	int nCount = m_vpGeom.Size();
	for( int i=nCount-1; i>-1; --i )
	{
		if( ( m_vpGeom[ i ]->m_nPlag & dwPlag ) != 0 )
		{
			CGeometry::DelInstance( m_vpGeom[ i ] );
			m_vpGeom.DeleteElement( i );
		}
	}
}

float CsSortObj::CalCameraDist()
{
	int nCount = m_vpGeom.Size();
	CGeometry* pGeom = NULL;
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		switch( pGeom->GetType() )
		{
		case CGeometry::CharOutLine_Gray:
		case CGeometry::CharOutLine:
		case CGeometry::MeshOutLine:
		case CGeometry::MeshOutLine_Gray:			
			return pGeom->RenderBegin();
		}
	}
	return -1.0f;
}

void CsSortObj::Render()
{
	int nCount = m_vpGeom.Size();
	if( nCount == 0 )
		return;

	CGeometry* pGeom = NULL;
	float fCameraDist = CalCameraDist();
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		pGeom->SetCameraDist( fCameraDist );		
		
		g_Sorting.Set( pGeom );
	}
}

void CsSortObj::RenderSort(float camDist)
{
	int nCount = m_vpGeom.Size();

	if( nCount == 0 )
		return;

	CGeometry* pGeom = NULL;

	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		pGeom->SetCameraDist( camDist );		

		g_Sorting.Set( pGeom );
	}
}

void CsSortObj::RenderAbsolute()
{
	int nCount = m_vpGeom.Size();

	CGeometry* pGeom = NULL;	
	float fCameraDist = CalCameraDist();
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		pGeom->SetCameraDist( fCameraDist );
		pGeom->Render();
	}
}

void CsSortObj::RenderAbsolute_ExceptPlag( DWORD dwPlag )
{
	int nCount = m_vpGeom.Size();

	CGeometry* pGeom = NULL;	
	float fCameraDist = CalCameraDist();
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		if( pGeom->IsPlag( dwPlag ) )
			continue;
		pGeom->SetCameraDist( fCameraDist );
		pGeom->Render();
	}
}


void CsSortObj::SetTempAlpha( bool bAlpha )
{ 
	if( m_bTempAlpha == bAlpha )
		return;
	m_bTempAlpha = bAlpha;
	int nSize = m_vpGeom.Size();
	for( int i=0; i<nSize; ++ i )
		m_vpGeom[ i ]->ResetAlpha();
}

void CsSortObj::SetShader( char* cSkinShader, char* cMeshShader )
{
	int nCount = m_vpGeom.Size();
	CGeometry* pGeom;
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		if( pGeom->IsPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE ) )
			continue;

		switch( pGeom->GetType() )
		{
		case CGeometry::CharOutLine:
		case CGeometry::CharOutLine_Gray:
			pGeom->m_pGeometry->ApplyAndSetActiveMaterial( cSkinShader );
			break;
		case CGeometry::MeshOutLine:
		case CGeometry::MeshOutLine_Gray:
			pGeom->m_pGeometry->ApplyAndSetActiveMaterial( cMeshShader );
			break;
		case CGeometry::Normal:
			{				
				NiSkinInstance* pSkin = pGeom->m_pGeometry->GetSkinInstance();
				if( pSkin && pSkin->GetSkinPartition() )
				{
					pGeom->m_pGeometry->ApplyAndSetActiveMaterial( cSkinShader );
				}
				else
				{
					pGeom->m_pGeometry->ApplyAndSetActiveMaterial( cMeshShader );
				}
			}			
			break;
		default:
			assert_cs( false );
		}
	}
}

void CsSortObj::SetShader( NiSingleShaderMaterial* pSkinShader, NiSingleShaderMaterial* pMeshShader )
{
	int nCount = m_vpGeom.Size();
	CGeometry* pGeom;
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		if( pGeom->IsPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE ) )
			continue;
#ifdef CRASHDUMP_0326		// chu8820 - SetShader 실패 ( C015 )
		if(pGeom == NULL)
			continue;
		
		int nGeomType = pGeom->GetType();

		switch( nGeomType )
		{
		case CGeometry::CharOutLine:
		case CGeometry::CharOutLine_Gray:
			if( pSkinShader == NULL )
			{
				DUMPLOG( " C015 - 1 " );
				return;
			}
			break;
		case CGeometry::MeshOutLine:
		case CGeometry::MeshOutLine_Gray:
		case CGeometry::UseTR:
			if( pMeshShader == NULL )
			{
				DUMPLOG( " C015 - 2 " );
				return;
			}
			break;
		case CGeometry::Normal:
			{
				NiSkinInstance* pSkin = pGeom->m_pGeometry->GetSkinInstance();
				if( pSkin && pSkin->GetSkinPartition() )
				{
					if( pSkinShader == NULL )
					{
						DUMPLOG( " C015 - 1 " );
						return;
					}
				}
				else
				{
					if( pMeshShader == NULL )
					{
						DUMPLOG( " C015 - 2 " );
						return;
					}
				}
			}
			break;
		}
#endif

		switch( pGeom->GetType() )
		{
		case CGeometry::CharOutLine:
		case CGeometry::CharOutLine_Gray:
			pGeom->m_pGeometry->ApplyAndSetActiveMaterial( pSkinShader );
			break;
		case CGeometry::MeshOutLine:
		case CGeometry::MeshOutLine_Gray:
			pGeom->m_pGeometry->ApplyAndSetActiveMaterial( pMeshShader );
			break;
		case CGeometry::Normal:
			{
				NiSkinInstance* pSkin = pGeom->m_pGeometry->GetSkinInstance();
				if( pSkin && pSkin->GetSkinPartition() )
				{
					pGeom->m_pGeometry->ApplyAndSetActiveMaterial( pSkinShader );
				}
				else
				{
					pGeom->m_pGeometry->ApplyAndSetActiveMaterial( pMeshShader );
				}
			}			
			break;
		case CGeometry::UseTR:
			{
				pGeom->m_pGeometry->ApplyAndSetActiveMaterial( pMeshShader );
			}
			break;
		default:
			assert_cs( false );
		}		
	}	
}

void CsNodeObj::SetNiObject( NiNodePtr pObject, int nGeomType, bool bSorting /*=false*/ )
{
	m_bTempAlpha = false;

	assert_cs( NiIsKindOf(NiNode, pObject) == true );
	m_pNiNode = pObject;
	m_vpGeom.Init( 4, AP_DOUBLE );

	m_pNiNode->UpdateProperties();

	if( bSorting )
		nsCSGBFUNC::InsertList_Geom_Sorting( &m_vpGeom, (NiAVObject*)m_pNiNode, nGeomType );
	else
		nsCSGBFUNC::InsertList_Geom( &m_vpGeom, (NiAVObject*)m_pNiNode, nGeomType );	
}

void CsNodeObj::SetNiObject_IncludeHide( NiNodePtr pObject, int nGeomType )
{
	m_bTempAlpha = false;

	assert_cs( NiIsKindOf(NiNode, pObject) == true );
	m_pNiNode = pObject;
	m_vpGeom.Init( 4, AP_DOUBLE );

	m_pNiNode->UpdateProperties();
	nsCSGBFUNC::InsertList_Geom_IncludeHide( &m_vpGeom, (NiAVObject*)m_pNiNode, nGeomType );
	
}

void CsNodeObj::ResetNiObject( int nGeomType )
{
	m_bTempAlpha = false;
	int nCount = m_vpGeom.Size();
	for( int i=0; i<nCount; ++i )
	{
		CGeometry::DelInstance( m_vpGeom[ i ] );
	}
	m_vpGeom.ClearElement();

	m_pNiNode->UpdateProperties();
	nsCSGBFUNC::InsertList_Geom( &m_vpGeom, (NiAVObject*)m_pNiNode, nGeomType );		
}

void CsNodeObj::Render_GeomData()
{
	int nCount = m_vpGeom.Size();
	for( int i=0; i<nCount; ++i )
	{
		( (CGeometryData*)m_vpGeom[ i ] )->CGeometryData::Render();
	}
}

void CsNodeObj::Render_GeomDataShadow_ExceptPlag( DWORD dwPlag )
{	
	int nCount = m_vpGeom.Size();	
	CGeometryData* pGeomData = NULL;
	for( int i=0; i<nCount; ++i )
	{
		pGeomData = (CGeometryData*)m_vpGeom[ i ];
		if( pGeomData->IsPlag( dwPlag ) == true )
			continue;

		pGeomData->CGeometryData::Render();
	}
}

void CsNodeObj::Render_GeomData_ExceptPlag( DWORD dwPlag )
{	
	int nCount = m_vpGeom.Size();	
	CGeometry* pGeom = NULL;
	//float fCameraDist = CalCameraDist();
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		if( pGeom->IsPlag( dwPlag ) == true )
			continue;

		pGeom->SetCameraDist( 1 );	// 2015-11-30-nova fCameraDist -> 1로 변경
		pGeom->Render();		
	}
}


void CsNodeObj::ChangeShader( int nGeomType, bool bSorting )
{
	int nCount = m_vpGeom.Size();
	for( int i=0; i<nCount; ++i )
	{
		CGeometry::DelInstance( m_vpGeom[ i ] );
	}
	m_vpGeom.Destroy();
	SetNiObject( m_pNiNode, nGeomType, bSorting );
	ActiveShader();
}

void CsNodeObj::ActiveShader()
{
	char cShader[ 64 ];
	CGeometry* pGeom;
	int nCount = m_vpGeom.Size();
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		if( pGeom->IsPlag( CGeometry::GP_PARTICLE | CGeometry::GP_NO_ZBUFFER_WRITE ) )
			continue;

		ShaderPlagStr( pGeom, cShader, 64 );
		if( cShader[ 0 ] != NULL )
			pGeom->m_pGeometry->ApplyAndSetActiveMaterial( cShader );
		else
			pGeom->m_pGeometry->ApplyAndSetActiveMaterial( nsCsGBTerrain::g_pRenderer->GetDefaultMaterial() );
	}

	m_pNiNode->UpdateEffects();	
}

void CsNodeObj::SaveNiBit()
{
	CGeometry* pGeom;
	int nCount = m_vpGeom.Size();
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		pGeom->m_nOrgNiBit = pGeom->m_pGeometry->GetAllBit();
	}
}

bool CsNodeObj::LoadNiBit()
{
	bool bChange = false;
	CGeometry* pGeom;
	int nCount = m_vpGeom.Size();
	for( int i=0; i<nCount; ++i )
	{
		pGeom = m_vpGeom[ i ];
		if( pGeom->m_pGeometry->SetAllBit( pGeom->m_nOrgNiBit ) == true )
			bChange = true;
	}
	return bChange;
}

void CsGeomObj::SetNiObject( NiGeometryPtr pObject, int nGeomType )
{
	m_bTempAlpha = false;

	assert_cs( NiIsKindOf(NiGeometry, pObject) == true );
	m_pNiGeom = pObject;
	m_vpGeom.Init( 4, AP_DOUBLE );
	nsCSGBFUNC::InsertList_Geom( &m_vpGeom, (NiAVObject*)m_pNiGeom, nGeomType );

	m_pNiGeom->UpdateProperties();
}


char* CsSortObj::ShaderPlagStr( CGeometry* pGeom, char* cShader, int nShaderLen )
{
	memset( cShader, 0, sizeof( char )*nShaderLen );

	if( pGeom->m_pGeometry->GetBit( NiAVObject::IS_BLUR_SHADER ) )
	{
		strcpy_s( cShader, 64, "BlurEffect" );
		return cShader;
	}

	switch( pGeom->GetType() )
	{
	case CGeometry::Normal:
		return cShader;

	case CGeometry::CharOutLine:
	case CGeometry::CharOutLine_Gray:
		strcat_s( cShader, 64, "Char_" );
		break;

	case CGeometry::MeshOutLine:
	case CGeometry::MeshOutLine_Gray:
		break;

	default:
		assert_cs( false );
	}

	switch( pGeom->GetType() )
	{
	case CGeometry::CharOutLine:
	case CGeometry::CharOutLine_Gray:
	case CGeometry::MeshOutLine:
	case CGeometry::MeshOutLine_Gray:
		if( nsCsGBTerrain::g_bCharOutLine && ( pGeom->m_pGeometry->GetBit( NiAVObject::IS_ALPHATEX_SHADER ) == false ) )
		{
			strcat_s( cShader, 64, "OutLine" );
		}
		else
		{
			strcat_s( cShader, 64, "NoOutLine" );
		}
		break;

	default:
		assert_cs( false );
	}

	switch( pGeom->GetType() )
	{
	case CGeometry::CharOutLine:	
	case CGeometry::MeshOutLine:		strcat_s( cShader, 64, "_Normal" );		break;	
	case CGeometry::CharOutLine_Gray:
	case CGeometry::MeshOutLine_Gray:	strcat_s( cShader, 64, "_Gray" );		break;

	default:
		assert_cs( false );
	}

	if( pGeom->m_pGeometry->GetBit( NiAVObject::IS_ALPHATEX_SHADER ) )
	{
		strcat_s( cShader, 64, "_AT" );
	}
	else if( pGeom->IsPlag( CGeometry::GP_SPECULAR ) )
	{
		strcat_s( cShader, 64, "_Spec" );
	}

	if( pGeom->GetStateType() == CGeometry::Color_Sampling)
	{
		strcat_s( cShader, 64, "_Color" );
	}

	else if( pGeom->GetStateType() == CGeometry::Color_Gray)
	{
		strcat_s( cShader, 64, "_Gray" );
	}
	return cShader;
}