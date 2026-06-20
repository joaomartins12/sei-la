

#include "stdafx.h"
#include "CsGBObjOpt_DarkMap.h"

CSGBMEMPOOL_CPP( CsGBObjOpt_DarkMap );

#define DM_OBJECT_SHADOW_UVSET_INDEX		1

CsGBObjOpt_DarkMap::CsGBObjOpt_DarkMap()
{
	m_pDMObjectUV = NULL;
	m_pDMObjectPixel = NULL;

	m_uiDMObjectUVCount = 0;
	m_uiDMObjectPixelCount = 0;
}

CsGBObjOpt_DarkMap::~CsGBObjOpt_DarkMap()
{
	m_pDMObjectUV = NULL;
	m_pDMObjectPixel = NULL;
}

CsGBObjOpt* CsGBObjOpt_DarkMap::GetInstance( CsGBObject* pParentObj )
{
	CsGBObjOpt* pOption = CsGBObjOpt_DarkMap::NewInstance();
	pOption->Init( pParentObj );
	return pOption;
}

void CsGBObjOpt_DarkMap::Delete()
{
	CsGBObjOpt::Delete();

	SAFE_NIDELETE_ARRAY( m_pDMObjectUV );	
	SAFE_DELETE_ARRAY( m_pDMObjectPixel );
	m_uiDMObjectUVCount = 0;
	m_uiDMObjectPixelCount = 0;
}

void CsGBObjOpt_DarkMap::ResetOption()
{
	ResetCalDMObjectData();
}

void CsGBObjOpt_DarkMap::SetData( char* pData, UINT& uiOffset )
{
	assert_cs( m_pDMObjectUV == NULL );
	assert_cs( m_pDMObjectPixel == NULL );

	// uv set Load
	memcpy( &m_uiDMObjectUVCount, &pData[ uiOffset ], sizeof( UINT ) );
	uiOffset += sizeof( UINT );
	m_pDMObjectUV = NiNew NiPoint2[ m_uiDMObjectUVCount ];
	memcpy( m_pDMObjectUV, &pData[ uiOffset ], sizeof( NiPoint2 )*m_uiDMObjectUVCount );
	uiOffset += sizeof( NiPoint2 )*m_uiDMObjectUVCount;
	// pixel Save
	memcpy( &m_uiDMObjectPixelCount, &pData[ uiOffset ], sizeof( UINT ) );
	uiOffset += sizeof( UINT );
	m_pDMObjectPixel = csnew BYTE[ m_uiDMObjectPixelCount*3 ];
	memcpy( m_pDMObjectPixel, &pData[ uiOffset ], sizeof( BYTE )*3*m_uiDMObjectPixelCount );
	uiOffset += sizeof( BYTE )*3*m_uiDMObjectPixelCount;

	if( m_pParentUserDefine != NULL )
		_ApplyDMObjectInfo();
}

UINT CsGBObjOpt_DarkMap::GetSaveSize()
{
	if( m_pParentUserDefine != NULL )
		CalDMObjectData();

	return	sizeof( UINT )*2 +
			sizeof( NiPoint2 )*m_uiDMObjectUVCount +
			sizeof( BYTE )*3*m_uiDMObjectPixelCount;			
}

void CsGBObjOpt_DarkMap::Save( char* pData, UINT& uiOffset )
{
	// uv set Save
	memcpy( &pData[ uiOffset ], &m_uiDMObjectUVCount, sizeof( UINT ) );
	uiOffset += sizeof( UINT );
	memcpy( &pData[ uiOffset ], m_pDMObjectUV, sizeof( NiPoint2 )*m_uiDMObjectUVCount );
	uiOffset += sizeof( NiPoint2 )*m_uiDMObjectUVCount;
	// pixel Save
	memcpy( &pData[ uiOffset ], &m_uiDMObjectPixelCount, sizeof( UINT ) );
	uiOffset += sizeof( UINT );
	memcpy( &pData[ uiOffset ], m_pDMObjectPixel, sizeof( BYTE )*3*m_uiDMObjectPixelCount );
	uiOffset += sizeof( BYTE )*3*m_uiDMObjectPixelCount;
}

void CsGBObjOpt_DarkMap::_ApplyDMObjectInfo()
{
	assert_cs( m_pDMObjectUV != NULL );
	assert_cs( m_pDMObjectPixel != NULL );

	DWORD dwMapSize = ( (sUDD_DARKMAP*)m_pParentUserDefine->GetData( sUDD_DARKMAP::Type() ) )->s_dwTerrainBaseShadowMapSize;
	if( m_uiDMObjectPixelCount != ( dwMapSize*dwMapSize ) )
	{
		CsMessageBoxA( MB_OK, "Base의 Texture사이즈가 변경 되었습니다. 다시 세이브해 주세요.\n\n%s", m_pParentObj->GetInfo()->s_cObjectPath );
		SAFE_NIDELETE_ARRAY( m_pDMObjectUV );
		SAFE_DELETE_ARRAY( m_pDMObjectPixel );
		return;
	}

	// 텍스쳐
	NiPixelData* pPixelData = NiNew NiPixelData( dwMapSize, dwMapSize, NiPixelFormat::RGB24 );
	memcpy( pPixelData->GetPixels(), m_pDMObjectPixel, sizeof( BYTE )*3*m_uiDMObjectPixelCount );	
	NiSourceTexturePtr pNiTexture = NiSourceTexture::Create( pPixelData );

	UINT uiUVOffset = 0;
	_ApplyDMObjectInfo( m_pParentObj->GetNiNode(), uiUVOffset, pNiTexture );
}

void CsGBObjOpt_DarkMap::_ApplyDMObjectInfo( NiNode* pNiNode, UINT& uiUVOffset, NiSourceTexturePtr pNiTexture )
{
	assert_cs( NiIsKindOf( NiNode, pNiNode ) == true );
	int nChildCount = pNiNode->GetArrayCount();
	for( int i=0; i<nChildCount; ++i )
	{
		NiAVObject* pChild = pNiNode->GetAt( i );
		if( NiIsKindOf( NiNode, pChild ) == true )
		{
			_ApplyDMObjectInfo( (NiNode*)pChild, uiUVOffset, pNiTexture );
		}
		else if( ( ( NiIsKindOf( NiTriStrips, pChild ) == true )||( NiIsKindOf( NiTriShape, pChild ) == true ) )&&
			( pChild->GetAppCulled() == false ) )
		{
			NiGeometry* pGeom = ( NiGeometry* )pChild;
			assert_cs( NiIsKindOf( NiGeometry, pGeom ) == true );

			// uv Set
			NiPoint2* pUV = pGeom->GetTextureSet( 1 );
			if( pUV )
			{
				UINT nVertCount = pGeom->GetVertexCount();
				memcpy( pUV, &m_pDMObjectUV[ uiUVOffset ], sizeof( NiPoint2 )*nVertCount );
				uiUVOffset += nVertCount;
			}

			// Texture
			NiTexturingProperty* pTexProp = (NiTexturingProperty*)pGeom->GetProperty( NiTexturingProperty::GetType() );
			//assert_cs( pTexProp->GetDarkMap() == NULL );
			assert_cs( pTexProp != NULL );
			NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( pNiTexture, 1, NiTexturingProperty::CLAMP_S_CLAMP_T );
			pTexProp->SetDarkMap( pMap );
		}
	}
}

void CsGBObjOpt_DarkMap::CalDMObjectData()
{
	// 툴에서만 호출하자
	assert_cs( m_pParentObj->UD_IsDarkMap() == true );
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );

	DWORD dwMapSize = ( (sUDD_DARKMAP*)m_pParentUserDefine->GetData( sUDD_DARKMAP::Type() ) )->s_dwTerrainBaseShadowMapSize;

	NiNode* pNiNode = m_pParentObj->GetNiNode();

	if( m_pDMObjectUV == NULL )
	{
		assert_cs( m_pDMObjectPixel == NULL );

		// UV
		m_uiDMObjectUVCount = 0;
		_CalDMObjectUVCount( pNiNode );
		m_pDMObjectUV = NiNew NiPoint2[ m_uiDMObjectUVCount ];

		// Pixel		
		m_uiDMObjectPixelCount = dwMapSize*dwMapSize;
		m_pDMObjectPixel = csnew BYTE[ m_uiDMObjectPixelCount*3 ];
	}
	else if( m_uiDMObjectPixelCount != ( dwMapSize*dwMapSize ) )
	{
		SAFE_DELETE_ARRAY( m_pDMObjectPixel );
		m_uiDMObjectPixelCount = dwMapSize*dwMapSize;
		m_pDMObjectPixel = csnew BYTE[ m_uiDMObjectPixelCount*3 ];
	}

	// UV
	UINT uiUVOffset = 0;
	_CalDMObjectUV( pNiNode, uiUVOffset );
	assert_cs( uiUVOffset == m_uiDMObjectUVCount );

	// Pixel
	_CalDMObjectPixel();
}

void CsGBObjOpt_DarkMap::ResetCalDMObjectData()
{
	if( m_pDMObjectUV != NULL )
	{
		assert_cs( m_pDMObjectPixel != NULL );

		SAFE_NIDELETE_ARRAY( m_pDMObjectUV );
		SAFE_DELETE_ARRAY( m_pDMObjectPixel );
	}

	// 툴에서만 호출하자
	assert_cs( m_pParentObj->UD_IsDarkMap() == true );
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );

	assert_cs( m_pDMObjectUV == NULL );
	assert_cs( m_pDMObjectPixel == NULL );

	NiNode* pNiNode = m_pParentObj->GetNiNode();

	// UV
	m_uiDMObjectUVCount = 0;
	_CalDMObjectUVCount( pNiNode );
	m_pDMObjectUV = NiNew NiPoint2[ m_uiDMObjectUVCount ];

	// Pixel
	DWORD dwMapSize = ( (sUDD_DARKMAP*)m_pParentUserDefine->GetData( sUDD_DARKMAP::Type() ) )->s_dwTerrainBaseShadowMapSize;
	m_uiDMObjectPixelCount = dwMapSize*dwMapSize;
	m_pDMObjectPixel = csnew BYTE[ m_uiDMObjectPixelCount*3 ];

	// UV
	UINT uiUVOffset = 0;
	_CalDMObjectUV( pNiNode, uiUVOffset );
	assert_cs( uiUVOffset == m_uiDMObjectUVCount );

	// Pixel
	_CalDMObjectPixel();
}

void CsGBObjOpt_DarkMap::_CalDMObjectUVCount( NiNode* pNiNode )
{
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );
	assert_cs( NiIsKindOf( NiNode, pNiNode ) == true );
	int nChildCount = pNiNode->GetArrayCount();
	for( int i=0; i<nChildCount; ++i )
	{
		NiAVObject* pChild = pNiNode->GetAt( i );
		if( NiIsKindOf( NiNode, pChild ) == true )
		{
			_CalDMObjectUVCount( (NiNode*)pChild );
		}
		else if( ( ( NiIsKindOf( NiTriStrips, pChild ) == true )||( NiIsKindOf( NiTriShape, pChild ) == true ) )&&
			( pChild->GetAppCulled() == false ) )
		{
			NiGeometry* pGeom = ( NiGeometry* )pChild;
			m_uiDMObjectUVCount += pGeom->GetVertexCount();
		}
	}
}

void CsGBObjOpt_DarkMap::_CalDMObjectUV( NiNode* pNiNode, UINT& uiUVOffset )
{
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );
	assert_cs( NiIsKindOf( NiNode, pNiNode ) == true );
	int nChildCount = pNiNode->GetArrayCount();
	for( int i=0; i<nChildCount; ++i )
	{
		NiAVObject* pChild = pNiNode->GetAt( i );
		if( NiIsKindOf( NiNode, pChild ) == true )
		{
			_CalDMObjectUV( (NiNode*)pChild, uiUVOffset );
		}
		else if( ( ( NiIsKindOf( NiTriStrips, pChild ) == true )||( NiIsKindOf( NiTriShape, pChild ) == true ) )&&
				 ( pChild->GetAppCulled() == false ) )
		{
			NiGeometry* pGeom = ( NiGeometry* )pChild;
			NiPoint2* pUV = pGeom->GetTextureSet( DM_OBJECT_SHADOW_UVSET_INDEX );
			assert_cs( pUV != NULL );
			UINT nVertCount = pGeom->GetVertexCount();
			memcpy( &m_pDMObjectUV[ uiUVOffset ], pUV, sizeof( NiPoint2 )*nVertCount );
			uiUVOffset += nVertCount;
		}
	}
}

void CsGBObjOpt_DarkMap::_CalDMObjectPixel()
{
	NiNode* pNiNode = m_pParentObj->GetNiNode();

	assert_cs( m_pDMObjectPixel != NULL );
	assert_cs( nsCsGBTerrain::g_eCreateType != nsCsGBTerrain::CT_FILELOAD );
	assert_cs( pNiNode->GetArrayCount() != 0 );

	NiAVObject* pObject = nsCSGBFUNC::GetEnableFirstNode( pNiNode );	

	while( NiIsKindOf( NiGeometry, pObject ) == false )
	{
		assert_cs( NiIsKindOf( NiNode, pObject ) == true );
		assert_cs( ( (NiNode*)pObject )->GetArrayCount() != 0 );
		pObject = ( (NiNode*)pObject )->GetAt( 0 );
	}
	assert_cs( NiIsKindOf( NiGeometry, pObject ) == true );
	NiTexturingProperty* pTexProp = (NiTexturingProperty*)pObject->GetProperty( NiTexturingProperty::GetType() );
	assert_cs( pTexProp != NULL );

	NiTexturingProperty::Map* pDarkMap = pTexProp->GetDarkMap();

	// 다크맵이 없다
	if( pDarkMap == NULL )
	{
		memset( m_pDMObjectPixel, 0xff, sizeof( BYTE )*3*m_uiDMObjectPixelCount );
		return;
	}

	NiSourceTexture* pTexture = (NiSourceTexture*)pDarkMap->GetTexture();
	assert_cs( NiIsKindOf( NiSourceTexture, pTexture ) == true );
	NiPixelData* pPixelData = pTexture->GetSourcePixelData();
	assert_cs( m_uiDMObjectPixelCount == ( pPixelData->GetWidth()*pPixelData->GetHeight() ) );
	assert_cs( pPixelData->GetPixelStride() == 3 );
	memcpy( m_pDMObjectPixel, pPixelData->GetPixels(), sizeof( BYTE )*3*m_uiDMObjectPixelCount );
}
