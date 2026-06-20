
#include "stdafx.h"
#include "CsGBSpeedCell.h"

CsGBSpeedCell*		g_pSpeedCell = NULL;

CsGBSpeedCell::CsGBSpeedCell()
{
	m_ppCellArray = NULL;
	m_nWidthCellCount = 0;
	m_nHeightCellCount = 0;
	m_nBaseObjectKind = 0;
}

CsGBSpeedCell::~CsGBSpeedCell()
{
}

void CsGBSpeedCell::SaveExtraData( CsNiNodePtr pBaseNiNode )
{
	// ==== 인포 저장	
	assert_cs( sizeof( char ) == 1 );
	// 저장할 사이즈	
	UINT nSize = sizeof( int ) + m_nBaseObjectKind*sizeof( sOBJECT_INFO );
	for( int y=0; y<m_nHeightCellCount; ++y )
	{
		for( int x=0; x<m_nWidthCellCount; ++x )
		{
			nSize += m_ppCellArray[ y ][ x ].GetExtraDataSize();
		}
	}

	// 메모리 할당
	UINT nOffset = 0;
	char* pInfo = NiAlloc( char, nSize );
	memcpy( &pInfo[ nOffset ], &m_nBaseObjectKind, sizeof( int ) );
	nOffset += sizeof( int );

	for( int i=0; i<m_nBaseObjectKind; ++i )
	{
		memcpy( &pInfo[ nOffset ], &m_vpBaseObject[ i ]->s_ObjectInfo, sizeof( sOBJECT_INFO ) );
		nOffset += sizeof( sOBJECT_INFO );
	}

	for( int y=0; y<m_nHeightCellCount; ++y )
	{
		for( int x=0; x<m_nWidthCellCount; ++x )
		{
			m_ppCellArray[ y ][ x ].SaveExtraData( pInfo, nOffset );
		}
	}

	NiBinaryExtraData* pExtData = NiNew NiBinaryExtraData( nSize, pInfo );
	pBaseNiNode->AddExtraData( CsGBCELL_ExtraKey, pExtData );
}

void CsGBSpeedCell::LoadExtraData( CsNiNodePtr pNiNode )
{
	NiBinaryExtraData* pExtData = (NiBinaryExtraData*)pNiNode->GetExtraData( CsGBCELL_ExtraKey );
	if( pExtData == NULL )
	{
		m_nBaseObjectKind = 0;
		m_vpBaseObject.Init( 1, AP_ADD );
		
		Init();

		return;
	}

	UINT nSize;
	UINT nOffset = 0;
	char* pData;
	pExtData->GetValue( nSize, pData );

	memcpy( &m_nBaseObjectKind, &pData[ nOffset ], sizeof( int ) );
	nOffset += sizeof( int );

	m_vpBaseObject.Init( ( m_nBaseObjectKind != 0 ) ? m_nBaseObjectKind : 1, AP_ADD );

	int nObjKind = m_nBaseObjectKind;
	for( int i=0; i<nObjKind; ++i )
	{
		sOBJECT_INFO obj;
		memcpy( &obj, &pData[ nOffset ], sizeof( sOBJECT_INFO ) );
		nOffset += sizeof( sOBJECT_INFO );

		sOBJECT* pObject = AddBaseObject( i, obj.s_cPath );
		memcpy( &pObject->s_ObjectInfo, &obj, sizeof( sOBJECT_INFO ) );
	}
	assert_cs( nObjKind == m_nBaseObjectKind );

	Init();
	for( int y=0; y<m_nHeightCellCount; ++y )
	{
		for( int x=0; x<m_nWidthCellCount; ++x )
		{
			m_ppCellArray[ y ][ x ].LoadExtraData( pData, nOffset );
		}
	}
}

void CsGBSpeedCell::Destroy()
{
	if( m_nWidthCellCount == 0 )
	{
		assert_cs( m_nHeightCellCount == 0 );
		return;
	}

	for( int y=0; y<m_nHeightCellCount; ++y )
	{		
		for( int x=0; x<m_nWidthCellCount; ++x )
		{
			m_ppCellArray[ y ][ x ].Delete();
		}
		delete[] m_ppCellArray[ y ];
	}
	SAFE_DELETE_ARRAY( m_ppCellArray );

	m_RenderCell.Destroy();

	m_nWidthCellCount = 0;
	m_nHeightCellCount = 0;

	assert_cs( m_nBaseObjectKind == m_vpBaseObject.Size() );
	for( int i=0; i<m_nBaseObjectKind; ++i )
	{
		m_vpBaseObject[ i ]->s_Node.Delete();
		NiDelete m_vpBaseObject[ i ];
	}
	m_vpBaseObject.Destroy();
	m_nBaseObjectKind = 0;
}

void CsGBSpeedCell::Init()
{
	CsGBCell::m_ConstantMatrix.MakeXRotation( NI_PI*0.5f );

	if( m_vpBaseObject.IsExistAlloc( 0 ) == false )
		m_vpBaseObject.Init( 1, AP_ADD );

	assert_cs( m_nWidthCellCount == 0 );
	assert_cs( m_nHeightCellCount == 0 );

	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	int nCellSize = pRootInfo->s_nCellObject_Size;

	m_nWidthCellCount = (pRootInfo->s_nXVertCount-1)*pRootInfo->s_nGridSize / nCellSize;
	m_nHeightCellCount = (pRootInfo->s_nYVertCount-1)*pRootInfo->s_nGridSize / nCellSize;

	float fCellRadius = nCellSize*sqrt( 2.0f )/2.0f;
	NiPoint3 ptCellCenter = NiPoint3( nCellSize*0.5f, nCellSize*0.5f, 0.0f );

	assert_cs( m_ppCellArray == NULL );
	m_ppCellArray = csnew CsGBCell*[ m_nHeightCellCount ];
	for( int y=0; y<m_nHeightCellCount; ++y )
	{
		m_ppCellArray[ y ] = NiNew CsGBCell[ m_nWidthCellCount ];
		for( int x=0; x<m_nWidthCellCount; ++x )
		{
			m_ppCellArray[ y ][ x ].Init( ptCellCenter, fCellRadius );
			ptCellCenter.x += nCellSize;
		}
		ptCellCenter.x = nCellSize*0.5f;
		ptCellCenter.y += nCellSize;
	}
	m_RenderCell.Init( 32, AP_ADD );
}

void CsGBSpeedCell::Update( float fElapsedTime )
{
	// 카메라가 걸쳐져 있는 셀을 가져온다.
	NiPoint3 ptCamera = CAMERA_ST.GetWorldTranslate();
	int nViewCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nClipCellObject;
	float fCellSize = (float)nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nCellObject_Size;
	float fViewerHeight = CAMERA_ST.GetViewerPos().z + 50.0f;

	int iX = (int)( ptCamera.x/(float)fCellSize );
	int iY = (int)( ptCamera.y/(float)fCellSize );

	int minX = CsMax( 0, iX - nViewCount );
	int minY = CsMax( 0, iY - nViewCount );
	int maxX = CsMin( m_nWidthCellCount, iX + nViewCount );
	int maxY = CsMin( m_nHeightCellCount, iY + nViewCount );

	for( int y=minY; y<maxY; ++y )
	{
		for( int x=minX; x<maxX; ++x )
		{			
			if( m_ppCellArray[ y ][ x ].Culling( fViewerHeight ) == true )
			{
				m_ppCellArray[ y ][ x ].Update( fElapsedTime );
				m_RenderCell.PushBack( &m_ppCellArray[ y ][ x ] );
			}
		}
	}	
}

void CsGBSpeedCell::Update_ByTool( float fElapsedTime )
{
	for( int y=0; y<m_nHeightCellCount; ++y )
	{
		for( int x=0; x<m_nWidthCellCount; ++x )
		{
			m_ppCellArray[ y ][ x ].Update( fElapsedTime );
			m_RenderCell.PushBack( &m_ppCellArray[ y ][ x ] );
		}
	}	
}

void CsGBSpeedCell::Render()
{
	int nRenderCell = m_RenderCell.Size();
	for( int nBaseObjectCount = 0; nBaseObjectCount<m_nBaseObjectKind; ++nBaseObjectCount )
	{
		bool bFirst = true;
		for( int i=0; i<nRenderCell; ++i )
		{
			m_RenderCell[ i ]->Render( nBaseObjectCount, bFirst );
		}
	}	
	m_RenderCell.ClearElement();
}

//=========================================================================================
//
//		Light
//
//=========================================================================================

void CsGBSpeedCell::ApplyLight( CsGBLight* pLight )
{
	for( int i=0; i<m_vpBaseObject.Size(); ++i )
	{
		m_vpBaseObject[ i ]->s_Node.m_pNiNode->AttachEffect( pLight->GetNiLight() );
		m_vpBaseObject[ i ]->s_Node.m_pNiNode->UpdateEffects();
	}
}

void CsGBSpeedCell::ReleaseLight( CsGBLight* pLight )
{
	for( int i=0; i<m_vpBaseObject.Size(); ++i )
	{
		m_vpBaseObject[ i ]->s_Node.m_pNiNode->DetachEffect( pLight->GetNiLight() );
		m_vpBaseObject[ i ]->s_Node.m_pNiNode->UpdateEffects();
	}
}

//=========================================================================================
//
//		Object
//
//=========================================================================================

void CsGBSpeedCell::DeleteCellObject( int nBaseIndex, CsGBCell::sINFO* pCellInfo )
{
	int nCellSize = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nCellObject_Size;
	NiPoint3 pos = pCellInfo->s_trParent.m_Translate;
	int iX = (int)pos.x/nCellSize;
	int iY = (int)pos.y/nCellSize;

	return m_ppCellArray[ iY ][ iX ].DeleteObject( nBaseIndex, &pCellInfo->s_trParent );
}

CsGBCell::sINFO* CsGBSpeedCell::ResistObject( int nBaseIndex, NiTransform* trParent, NiTransform* trObject )
{
	int nCellSize = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nCellObject_Size;
	NiPoint3 pos = trParent->m_Translate;
	int iX = (int)pos.x/nCellSize;
	int iY = (int)pos.y/nCellSize;

	return m_ppCellArray[ iY ][ iX ].Resist( nBaseIndex, trParent, trObject, m_vpBaseObject[ nBaseIndex ]->s_pTransformController != NULL );
}

//=========================================================================================
//
//		Base Object
//
//=========================================================================================

void CsGBSpeedCell::DeleteBaseObject( int nObjectID )
{
	for( int y=0; y<m_nHeightCellCount; ++y )
	{
		for( int x=0; x<m_nWidthCellCount; ++x )
		{
			m_ppCellArray[ y ][ x ].DeleteBaseObject( nObjectID );
		}
	}

	m_vpBaseObject[ nObjectID ]->s_Node.Delete();
	NiDelete m_vpBaseObject[ nObjectID ];
	m_vpBaseObject.DeleteElement( nObjectID );

	--m_nBaseObjectKind;
}

CsGBSpeedCell::sOBJECT* CsGBSpeedCell::AddBaseObject( int nObjectID, char* cPath )
{
	assert_cs( m_vpBaseObject.Size() == nObjectID );
	sOBJECT* pObject = NiNew sOBJECT;
	
	strcpy_s( pObject->s_ObjectInfo.s_cPath, OBJECT_PATH_LEN, cPath );

	//assert_csm1( _access( pObject->s_ObjectInfo.s_cPath, 0 ) == 0, "CellObject Load Fail = %s", pObject->s_ObjectInfo.s_cPath );

	NiStream stream;
	stream.Load( pObject->s_ObjectInfo.s_cPath );

	NiNodePtr pNiNode = (NiNode*)stream.GetObjectAt( 0 );
	pNiNode->AttachProperty( nsCsGBTerrain::g_pCurRoot->GetFogProperty() );
	nsCsGBTerrain::g_pCurRoot->GetLightMng()->ApplyObject( pNiNode );
	pNiNode->UpdateEffects();
	pNiNode->UpdateProperties();
	pObject->s_Node.SetNiObject( pNiNode, CGeometry::UseTR );
	pObject->s_pGeometry = pObject->s_Node.m_vpGeom[ 0 ]->m_pGeometry;
	pObject->s_pModelData = pObject->s_pGeometry->GetModelData();
	pObject->s_pSkinInstance = NULL;	
	pObject->s_pTransformController = nsCSGBFUNC::GetTransformController( pNiNode );
	if( pObject->s_pTransformController != NULL )
	{
		nsCSGBFUNC::InitAnimation( pNiNode, NiTimeController::APP_TIME, NiTimeController::LOOP );
	}
	pObject->s_pBound = (NiBound*)&pObject->s_pGeometry->GetWorldBound();

	m_vpBaseObject.PushBack( pObject );

	m_nBaseObjectKind = m_vpBaseObject.Size();

	return pObject;
}
