
#include "stdafx.h"
#include "CsGBTerrainRoot.h"

//==========================================================================================
//
//		Load
//
//==========================================================================================

bool CsGBTerrainRoot::Load( const char* cFile, nsCsGBTerrain::eCREATE_TYPE ct )
{
	assert_cs( ct != nsCsGBTerrain::CT_MAKE );
	assert_cs( m_RootNode.GetNiNode() == NULL );

	//assert_csm1( _access_s( cFile, 0 ) == 0, "맵 데이터가 존재하지 않습니다.\n\n %s", cFile );

	NiStream stream;
	if( !stream.Load( cFile ) )
	{
		assert_csm1( false, "맵 데이터가 존재하지 않습니다.\n\n %s", cFile );
		return false;
	}

	int nNodeIndex;
	if( stream.GetObjectCount() == 1 )
	{
		nNodeIndex = 0;
	}
	else
	{
		assert_cs( stream.GetObjectCount() == 2 );
		nNodeIndex = 1;		
	}

	CsNiNodePtr pNiNode = (CsNiNode*)stream.GetObjectAt( nNodeIndex );
	if( LoadExtraData( pNiNode, cFile ) == false )
	{
		return false;
	}

	// 로드 관련 디바이스 셋팅
	//_LoadDeviceSetting();

	// Path Save
	nsCSFILE::GetRelativePath( m_cFilePath, MAX_PATH, (char*)cFile );

	// PathEngine Ground
	_LoadPathEngine( cFile );

	// Create Node
	CreateFromFile( pNiNode, ct );

	switch( ct )
	{
	case nsCsGBTerrain::CT_FILELOAD:
		{
			_LoadBMTexture( cFile );
			_LoadSMTexture( cFile );
			if( nsCsGBTerrain::g_Device.g_bPixelShader2 == true )
				_LoadAMTexture( cFile );

			for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
			{
				GetNode( i )->SetShaderMap( 6, g_pShadow->GetZBufferTex() );
			}

			m_SkyBox.Create();

			LoadEmrFile( cFile );
		}
		break;
	case nsCsGBTerrain::CT_FILELOAD_BYTOOL:
		{
			LoadToolExtraData( pNiNode, cFile );

			//CheckInTerrainObject();
			MakeOrgHeightField();
			MakeHeightField();
			//CalculateNormal();
			if( nsCsGBTerrain::g_Device.g_bPixelShader2 == true )
				_LoadAMTexture( cFile );
			_LoadSMorgTexture( cFile );
			_LoadEMTexture( cFile );
			m_SkyBox.Create();
		}
		break;
	}

	m_RootNode.GetNiNode()->UpdateProperties();
	m_RootNode.GetNiNode()->Update( 0.0f );	

#ifdef TERRAIN_RENDER_DEEPCOPY
	UpdateMapUVSize();
#else
#endif
	//ToggleWireFrame();
	return true;
}

void CsGBTerrainRoot::_LoadDeviceSetting()
{
	nsCsGBTerrain::g_pRenderer->SetBackgroundColor( m_Info.s_BackGroundColor );
}

void CsGBTerrainRoot::_LoadPathEngine( const char* cFile)
{
	if( g_pCsPathEngine == NULL )
		return;

	g_pCsPathEngine->DestroyMesh();

	char cStr[ MAX_PATH ];
	strcpy_s( cStr, MAX_PATH, cFile );
	size_t nLen = strlen( cStr );
	sprintf_s( &cStr[ nLen - 3 ], MAX_PATH - (nLen-3), "xml" );

	g_pCsPathEngine->CreateMesh( cStr );
	ApplyTerrainPath();
}

void CsGBTerrainRoot::_LoadBMTexture( const char* cFile )
{
	char cStr[ MAX_PATH ];
	strcpy_s( cStr, MAX_PATH, cFile );
	size_t nLen = strlen( cStr );
	sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_BM.cst" );

	NiStream stream;
	stream.Load( cStr );
	int nStreamCount = stream.GetObjectCount();

	// 베이스맵
	CsGBTerrainLeaf* pLeaf = NULL;
	int nStreamIndex = 0;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{
			pLeaf->SetBaseMap( (NiPixelData*)stream.GetObjectAt( nStreamIndex ) );
			++nStreamIndex;
		}
	}
	assert_cs( stream.GetObjectCount() == nStreamIndex );
}

void CsGBTerrainRoot::_LoadAMTexture( const char* cFile )
{
	char cStr[ MAX_PATH ];
	strcpy_s( cStr, MAX_PATH, cFile );
	size_t nLen = strlen( cStr );
	sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_AM.cst" );

	NiStream stream;
	bool bLoad = stream.Load( cStr );
	if( bLoad == false )
		return;

	CsGBTerrainLeaf* pLeaf = NULL;
	int nStreamIndex = 0;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{
			pLeaf->SetShaderMap( ALPHA_MAP_SHADER_NTM, (NiPixelData*)stream.GetObjectAt( nStreamIndex ) );
			++nStreamIndex;
		}
	}
	assert_cs( stream.GetObjectCount() == nStreamIndex );
}

void CsGBTerrainRoot::_LoadAMswTexture( const char* cFile )
{
	char cStr[ MAX_PATH ];
	strcpy_s( cStr, MAX_PATH, cFile );
	size_t nLen = strlen( cStr );
	sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_AMsw.cst" );

	NiStream stream;
	bool bLoad = stream.Load( cStr );
	if( bLoad == false )
		return;

	int nStreamCount = stream.GetObjectCount();
	NiNodePtr pNiInfoNode = (NiNode*)stream.GetObjectAt( 0 );
	unsigned short pExtraSize = pNiInfoNode->GetExtraDataSize();
	assert_cs( pExtraSize == stream.GetObjectCount() - 1 );

	// 알파맵 텍스쳐 적용
	char cKey[ 64 ];
	CsGBTerrainLeaf* pLeaf = NULL;

	unsigned int uiSize;
	char* pCSTData;
	NiBinaryExtraData* pExtData = NULL;
	for( int i=1; i<nStreamCount; ++i )
	{
		sprintf_s( cKey, 64, "AMswObject_%d", i );
		pExtData = (NiBinaryExtraData*)pNiInfoNode->GetExtraData( cKey );
		pExtData->GetValue( uiSize, pCSTData );
		
		pLeaf = GetNode( ( (sCST_INFO*)pCSTData )->s_usNodeIndex );
		pLeaf->SetShaderMap( ( (sCST_INFO*)pCSTData )->s_usShaderIndex, (NiPixelData*)stream.GetObjectAt( i ) );
	}	
}

void CsGBTerrainRoot::_LoadSMTexture( const char* cFile )
{
	char cStr[ MAX_PATH ];
	strcpy_s( cStr, MAX_PATH, cFile );
	size_t nLen = strlen( cStr );
	sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_SM.cst" );

	NiStream stream;
	bool bLoad = stream.Load( cStr );
	if( bLoad == false )
		return;

	CsGBTerrainLeaf* pLeaf = NULL;
	int nStreamIndex = 0;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{
			pLeaf->SetShaderMap( SHADOW_MAP_SHADER_NTM, (NiPixelData*)stream.GetObjectAt( nStreamIndex ) );
			++nStreamIndex;
		}
	}
	assert_cs( stream.GetObjectCount() == nStreamIndex );
}

void CsGBTerrainRoot::_LoadSMorgTexture( const char* cFile )
{
	char cStr[ MAX_PATH ];
	strcpy_s( cStr, MAX_PATH, cFile );
	size_t nLen = strlen( cStr );
	sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_SMorg.cst" );

	NiStream stream;
	bool bLoad = stream.Load( cStr );
	if( bLoad == false )
		return;

	CsGBTerrainLeaf* pLeaf = NULL;
	int nStreamIndex = 0;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{
			pLeaf->SetShaderMap( SHADOW_MAP_SHADER_NTM, (NiPixelData*)stream.GetObjectAt( nStreamIndex ) );
			++nStreamIndex;
		}
	}
	assert_cs( stream.GetObjectCount() == nStreamIndex );
}

void CsGBTerrainRoot::_LoadEMTexture( const char* cFile )
{
	char cStr[ MAX_PATH ];
	strcpy_s( cStr, MAX_PATH, cFile );
	size_t nLen = strlen( cStr );
	sprintf_s( &cStr[ nLen - 4 ], MAX_PATH - (nLen-4), "_EM.cst" );

	NiStream stream;
	bool bLoad = stream.Load( cStr );
	if( bLoad == false )
		return;

	CsGBTerrainLeaf* pLeaf = NULL;
	int nStreamIndex = 0;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{
			pLeaf->SetShaderMap( EFFECT_MAP_SHADER_NTM, (NiPixelData*)stream.GetObjectAt( nStreamIndex ) );
			++nStreamIndex;
		}
	}
	assert_cs( stream.GetObjectCount() == nStreamIndex );
}


//==========================================================================================
//
//		Save
//
//==========================================================================================

void CsGBTerrainRoot::PreSave( const char* cFile )
{
	nsCsGBTerrain::g_eCreateType = nsCsGBTerrain::CT_FILELOAD_BYTOOL;
	nsCSFILE::GetRelativePath( m_cFilePath, MAX_PATH, (char*)cFile );
}

#define SAVE_TEXTURE_FILE( nistream, orgFile, extFile )		{\
	char cDefineFile[ MAX_PATH ];\
	W2M( cDefineFile, orgFile, MAX_PATH );\
	size_t nDefineLen = strlen( cDefineFile );\
	sprintf_s( &cDefineFile[ nDefineLen - 4 ], MAX_PATH - (nDefineLen-4), extFile );\
	if( nistream.GetObjectCount() == 0 ){\
		TCHAR szDefineDelete[ MAX_PATH ] = {0, };\
		M2W( szDefineDelete, cDefineFile, MAX_PATH );\
		DeleteFile( szDefineDelete );\
	}\
	else{\
		nistream.Save( cDefineFile );\
	}\
}

void CsGBTerrainRoot::Save( LPCTSTR szFile )
{
	// 테이블 오브젝트 초기화
	DWORD dwMapID = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_dwMapID;
	CsMapTableObjGroup* pGroup = nsCsMapTable::g_pMapTableObjMng->GetGroup( dwMapID );
	pGroup->Delete();
	pGroup->Init( dwMapID );

	NiPixelDataPtr* ppMixSMPixelData = NiNew NiPixelDataPtr[ m_Info.s_nTotalLeafCount ];

	_SaveSMorgTexture( szFile );
	_SaveEMTexture( szFile );
	_SaveSMTexture( szFile, ppMixSMPixelData );
	_SaveBaseTexture( szFile, ppMixSMPixelData );
	_SaveAMTexture( szFile );
	_SaveAMswTexture( szFile );	
	_SaveExtraData();
	_SaveToolExtraData();

	char cSavePath[ MAX_PATH ] = {0, };
	W2M( cSavePath, szFile, MAX_PATH );
	NiStream stream;	
	stream.InsertObject( GetBaseNiNode() );	
	stream.Save( cSavePath );

	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
		ppMixSMPixelData[ i ] = 0;
	SAFE_DELETE_ARRAY( ppMixSMPixelData );

	// Path Save
	nsCSFILE::GetRelativePath( m_cFilePath, MAX_PATH, (char*)cSavePath );

	// 테이블 오브젝트 세이브
	nsCsMapTable::g_pMapTableObjMng->Save();
}

void CsGBTerrainRoot::_SaveSMorgTexture( LPCTSTR szFile )
{
	NiStream stream;
	CsGBTerrainLeaf* pLeaf = NULL;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == false )
		{
			assert_cs( pLeaf->GetNiNode() == NULL );
			continue;
		}
		stream.InsertObject( GET_SHADERMAP_PIXELDATA( pLeaf->GetNiNode(), SHADOW_MAP_SHADER_NTM ) );
	}
	// 저장
	SAVE_TEXTURE_FILE( stream, szFile, "_SMorg.cst" );
}

void CsGBTerrainRoot::_SaveEMTexture( LPCTSTR szFile )
{
	NiStream stream;
	CsGBTerrainLeaf* pLeaf = NULL;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == false )
		{
			continue;
		}

		stream.InsertObject( GET_SHADERMAP_PIXELDATA( pLeaf->GetNiNode(), EFFECT_MAP_SHADER_NTM ) );
	}
	// 저장
	SAVE_TEXTURE_FILE( stream, szFile, "_EM.cst" );
}

void CsGBTerrainRoot::_SaveSMTexture( LPCTSTR szFile, NiPixelDataPtr* ppMixSMPixelData )
{
	// Effect 0채널의 그레이 맵과 쉐도우맵을 합체
	// 쉐도우맵 사이즈
	int nTexSize = m_Info.s_nInitMapSize * m_Info.s_nShadowMPPG;

	NiStream stream;
	CsGBTerrainLeaf* pLeaf = NULL;
	NiPixelData* pEMPixelData = NULL;
	BYTE* pSMPixel = NULL;
	BYTE* pEMPixel = NULL;
	float fInverseColor = 1.0f/255.0f;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == false )
		{
			ppMixSMPixelData[ i ] = NULL;
			continue;
		}
		// 메모리 할당및 저장 준비
		ppMixSMPixelData[ i ] = NiNew NiPixelData( nTexSize, nTexSize, SHADOW_MAP_FORMAT );
		stream.InsertObject( ppMixSMPixelData[ i ] );
		BYTE* pOutPixel = (BYTE*)ppMixSMPixelData[ i ]->GetPixels();
		
		NiTriStrips* pTri = pLeaf->GetNiTriStrip();
		NiTexturingProperty* pTexProp = (NiTexturingProperty*)pTri->GetProperty( NiTexturingProperty::GetType() );
		pSMPixel = (BYTE*)GET_TP_SHADERMAP_PIXELDATA( pTexProp, SHADOW_MAP_SHADER_NTM )->GetPixels();
		pEMPixelData = GET_TP_SHADERMAP_PIXELDATA( pTexProp, EFFECT_MAP_SHADER_NTM );
		CsImageInterpolation ip( (BYTE*)pEMPixelData->GetPixels(), pEMPixelData->GetWidth(), pEMPixelData->GetHeight(),
								pEMPixelData->GetPixelStride(), false );
		pEMPixel = ip.Liner( nTexSize, nTexSize );

		for( int h=0; h<nTexSize; ++h )
		{
			for( int w=0; w<nTexSize; ++w )
			{
				pOutPixel[ ( h*nTexSize + w )*SHADOW_MAP_STRIDE + 0 ] = (BYTE)
						( 255.0f*( ( pSMPixel[ ( h*nTexSize + w )*SHADOW_MAP_STRIDE + 0 ]*fInverseColor )*
						( pEMPixel[ ( h*nTexSize + w )*EFFECT_MAP_STRIDE + 0 ]*fInverseColor ) ) );
				pOutPixel[ ( h*nTexSize + w )*SHADOW_MAP_STRIDE + 1 ] = (BYTE)
						( 255.0f*( ( pSMPixel[ ( h*nTexSize + w )*SHADOW_MAP_STRIDE + 1 ]*fInverseColor )*
						( pEMPixel[ ( h*nTexSize + w )*EFFECT_MAP_STRIDE + 1 ]*fInverseColor ) ) );
				pOutPixel[ ( h*nTexSize + w )*SHADOW_MAP_STRIDE + 2 ] = (BYTE)
						( 255.0f*( ( pSMPixel[ ( h*nTexSize + w )*SHADOW_MAP_STRIDE + 2 ]*fInverseColor )*
						( pEMPixel[ ( h*nTexSize + w )*EFFECT_MAP_STRIDE + 2 ]*fInverseColor ) ) );
			}
		}
		SAFE_DELETE_ARRAY( pEMPixel );
	}

	// ===== 다 만들었으면 경계선 복사 하자
	NiPixelData** ppPixelData = csnew NiPixelData*[ m_Info.s_nTotalLeafCount ];
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		ppPixelData[ i ] = (NiPixelData*)ppMixSMPixelData[ i ];
	}
	m_Info.CopyTextureBorder( m_Info.s_nShadowMPPG, ppPixelData );
	SAFE_DELETE_ARRAY( ppPixelData );

	// 저장
	SAVE_TEXTURE_FILE( stream, szFile, "_SM.cst" );	
}

void CsGBTerrainRoot::_SaveBaseTexture( LPCTSTR szFile, NiPixelDataPtr* ppMixSMPixelData )
{
	int nUseTexSizeX = GetInfo()->GetTextureRightFigure( GetInfo()->s_nBaseMPPG ) - GetInfo()->GetTextureLeftFigure( GetInfo()->s_nBaseMPPG );
	int nUseTexSizeY = GetInfo()->GetTextureBottomFigure( GetInfo()->s_nBaseMPPG ) - GetInfo()->GetTextureTopFigure( GetInfo()->s_nBaseMPPG );
	int nNodeWidth = GetInfo()->CalNodeSizeX();
	int nNodeHeight = GetInfo()->CalNodeSizeY();

	// uv가 반복되는 거리
	float uvLoopDist = GetInfo()->s_nGridSize / GetInfo()->s_fDetailMapSize;

	// 생성 텍스쳐 사이즈
	int nTexSize = m_Info.s_nInitMapSize * m_Info.s_nBaseMPPG; 

	NiStream stream;
	CsGBTerrainLeaf* pLeaf = NULL;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == false )
		{
			assert_cs( ppMixSMPixelData[ i ] == NULL );
			continue;
		}

		// 메모리 할당및 저장 준비
		NiPixelData* pOutPData = NiNew NiPixelData( nTexSize, nTexSize, NiPixelFormat::RGB24 );		
		stream.InsertObject( pOutPData );
		BYTE* pOutPixel = (BYTE*)pOutPData->GetPixels();

		NiTriStrips* pTri = pLeaf->GetNiTriStrip();		
		NiTexturingProperty* pTexProp = (NiTexturingProperty*)pTri->GetProperty( NiTexturingProperty::GetType() );

		BYTE* pAlphaPixel = NULL;
		{
			NiPixelData* pAlphaPixelData = GET_SHADERMAP_PIXELDATA( pLeaf->GetNiNode(), ALPHA_MAP_SHADER_NTM );
			CsImageInterpolation ip( (BYTE*)pAlphaPixelData->GetPixels(), pAlphaPixelData->GetWidth(), pAlphaPixelData->GetHeight(),
				pAlphaPixelData->GetPixelStride(), false );
			pAlphaPixel = ip.Liner( nTexSize, nTexSize );
		}

		BYTE* pShadowPixel = NULL;
		{
			CsImageInterpolation ip( (BYTE*)ppMixSMPixelData[ i ]->GetPixels(), ppMixSMPixelData[ i ]->GetWidth(), ppMixSMPixelData[ i ]->GetHeight(),
				ppMixSMPixelData[ i ]->GetPixelStride(), false );
			pShadowPixel = ip.Liner( nTexSize, nTexSize );
		}

		// 맵 로드
		NiTexturingProperty::ShaderMap* pShaderMap[ 5 ] = { 0, };
		NiPixelDataPtr pDestPixelData[ 5 ] = { 0, };
		{
			pShaderMap[ 0 ] = nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( pLeaf->GetNiTriInfo()->s_cDetail_Map0 );			
			NiSourceTexture* pSourceTex = (NiSourceTexture*)pShaderMap[ 0 ]->GetTexture();
			pSourceTex->LoadPixelDataFromFile();
			NiPixelData* pPData = pSourceTex->GetSourcePixelData();
			pDestPixelData[ 0 ] = NiNew NiPixelData( pPData->GetWidth(), pPData->GetHeight(), NiPixelFormat::RGB24 );
			NiImageConverter::GetImageConverter()->ConvertPixelData( *pPData, NiPixelFormat::RGB24, pDestPixelData[ 0 ], false );
		}		
		
		if( CsFPS::CsFPSystem::IsExist( 0, pLeaf->GetNiTriInfo()->s_cDetail_Map1 ) )
		{
			pShaderMap[ 1 ] = nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( pLeaf->GetNiTriInfo()->s_cDetail_Map1 );
			NiSourceTexture* pSourceTex = (NiSourceTexture*)pShaderMap[ 1 ]->GetTexture();
			pSourceTex->LoadPixelDataFromFile();
			NiPixelData* pPData = pSourceTex->GetSourcePixelData();
			pDestPixelData[ 1 ] = NiNew NiPixelData( pPData->GetWidth(), pPData->GetHeight(), NiPixelFormat::RGB24 );
			NiImageConverter::GetImageConverter()->ConvertPixelData( *pPData, NiPixelFormat::RGB24, pDestPixelData[ 1 ], false );
		}
		if( CsFPS::CsFPSystem::IsExist( 0, pLeaf->GetNiTriInfo()->s_cDetail_Map2 ) )
		{
			pShaderMap[ 2 ] = nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( pLeaf->GetNiTriInfo()->s_cDetail_Map2 );
			NiSourceTexture* pSourceTex = (NiSourceTexture*)pShaderMap[ 2 ]->GetTexture();
			pSourceTex->LoadPixelDataFromFile();
			NiPixelData* pPData = pSourceTex->GetSourcePixelData();
			pDestPixelData[ 2 ] = NiNew NiPixelData( pPData->GetWidth(), pPData->GetHeight(), NiPixelFormat::RGB24 );
			NiImageConverter::GetImageConverter()->ConvertPixelData( *pPData, NiPixelFormat::RGB24, pDestPixelData[ 2 ], false );
		}
		if( CsFPS::CsFPSystem::IsExist( 0, pLeaf->GetNiTriInfo()->s_cDetail_Map3 ) )
		{
			pShaderMap[ 3 ] = nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( pLeaf->GetNiTriInfo()->s_cDetail_Map3 );
			NiSourceTexture* pSourceTex = (NiSourceTexture*)pShaderMap[ 3 ]->GetTexture();
			pSourceTex->LoadPixelDataFromFile();
			NiPixelData* pPData = pSourceTex->GetSourcePixelData();
			pDestPixelData[ 3 ] = NiNew NiPixelData( pPData->GetWidth(), pPData->GetHeight(), NiPixelFormat::RGB24 );
			NiImageConverter::GetImageConverter()->ConvertPixelData( *pPData, NiPixelFormat::RGB24, pDestPixelData[ 3 ], false );
		}
		if( CsFPS::CsFPSystem::IsExist( 0, pLeaf->GetNiTriInfo()->s_cDetail_Map4 ) )
		{
			pShaderMap[ 4 ] = nsCsGBTerrain::g_ObjectResMgr.GetShaderMap( pLeaf->GetNiTriInfo()->s_cDetail_Map4 );
			NiSourceTexture* pSourceTex = (NiSourceTexture*)pShaderMap[ 4 ]->GetTexture();
			pSourceTex->LoadPixelDataFromFile();
			NiPixelData* pPData = pSourceTex->GetSourcePixelData();
			pDestPixelData[ 4 ] = NiNew NiPixelData( pPData->GetWidth(), pPData->GetHeight(), NiPixelFormat::RGB24 );
			NiImageConverter::GetImageConverter()->ConvertPixelData( *pPData, NiPixelFormat::RGB24, pDestPixelData[ 4 ], false );
		}

		// 다 합치자
		float fab = 0.0f;		float fad1 = 0.0f;		float fad2 = 0.0f;		float fad3 = 0.0f;		float fad4 = 0.0f;
		for( int h=0; h<nTexSize; ++h )
		{
			for( int w=0; w<nTexSize; ++w )
			{
				// 실제 노드상위 위치로 변환 / uv가 반복되는거리
				NiPoint2 uv;
				uv.x = ( w / (float)nUseTexSizeX * nNodeWidth ) / uvLoopDist;
				uv.y = ( h / (float)nUseTexSizeY * nNodeHeight ) / uvLoopDist;
				uv.x -= (int)uv.x;
				uv.y -= (int)uv.y;

				BYTE b_r	= 0x00,		b_g		= 0x00,		b_b		= 0x00;
				BYTE d1_r	= 0x00,		d1_g	= 0x00,		d1_b	= 0x00;
				BYTE d2_r	= 0x00,		d2_g	= 0x00,		d2_b	= 0x00;
				BYTE d3_r	= 0x00,		d3_g	= 0x00,		d3_b	= 0x00;
				BYTE d4_r	= 0x00,		d4_g	= 0x00,		d4_b	= 0x00;
				nsCsGBTerrain::g_ObjectResMgr.GetColor( pDestPixelData[ 0 ], uv, b_r, b_g, b_b );
				nsCsGBTerrain::g_ObjectResMgr.GetColor( pDestPixelData[ 1 ], uv, d1_r, d1_g, d1_b );
				nsCsGBTerrain::g_ObjectResMgr.GetColor( pDestPixelData[ 2 ], uv, d2_r, d2_g, d2_b );
				nsCsGBTerrain::g_ObjectResMgr.GetColor( pDestPixelData[ 3 ], uv, d3_r, d3_g, d3_b );
				nsCsGBTerrain::g_ObjectResMgr.GetColor( pDestPixelData[ 4 ], uv, d4_r, d4_g, d4_b );

				assert_cs( pAlphaPixel != NULL );
				fad4 = (float)pAlphaPixel[ ( h*nTexSize + w )*ALPHA_MAP_STRIDE + 3 ]/255.0f;
				fad3 = ( 1.0f - fad4 )*(float)pAlphaPixel[ ( h*nTexSize + w )*ALPHA_MAP_STRIDE + 2 ]/255.0f;
				fad2 = ( 1.0f - fad4 - fad3 )*(float)pAlphaPixel[ ( h*nTexSize + w )*ALPHA_MAP_STRIDE + 1 ]/255.0f;
				fad1 = ( 1.0f - fad4 - fad3 - fad2 )*(float)pAlphaPixel[ ( h*nTexSize + w )*ALPHA_MAP_STRIDE ]/255.0f;
				fab = 1.0f - fad4 - fad3 - fad2 - fad1;
				pOutPixel[ ( h*nTexSize + w )*3 + 0 ] = (BYTE)( b_r*fab + d1_r*fad1 + d2_r*fad2 + d3_r*fad3 + d4_r*fad4 );
				pOutPixel[ ( h*nTexSize + w )*3 + 1 ] = (BYTE)( b_g*fab + d1_g*fad1 + d2_g*fad2 + d3_g*fad3 + d4_g*fad4 );
				pOutPixel[ ( h*nTexSize + w )*3 + 2 ] = (BYTE)( b_b*fab + d1_b*fad1 + d2_b*fad2 + d3_b*fad3 + d4_b*fad4 );

				// 라이트맵과 쉐도우맵 합체
				assert_cs( pShadowPixel != NULL );
				NiColor colorResult;
				float fRate = (float)pShadowPixel[ ( h*nTexSize + w )*SHADOW_MAP_STRIDE + 0 ]/255.0f*2.0f;					
				colorResult.r = pOutPixel[ ( h*nTexSize + w )*3 + 0 ] * fRate;

				fRate = (float)pShadowPixel[ ( h*nTexSize + w )*SHADOW_MAP_STRIDE + 1 ]/255.0f*2.0f;
				colorResult.g = pOutPixel[ ( h*nTexSize + w )*3 + 1 ] * fRate;

				fRate = (float)pShadowPixel[ ( h*nTexSize + w )*SHADOW_MAP_STRIDE + 2 ]/255.0f*2.0f;
				colorResult.b = pOutPixel[ ( h*nTexSize + w )*3 + 2 ] * fRate;

				float fLowerRate = 1.0f;
				////  가장 높은 값을 255로 - 나머진 비율로 낮추어 준다
				//if( ( colorResult.r > 255.0f )||( colorResult.g > 255.0f )||( colorResult.b > 255.0f ) )
				//{
				//	if( colorResult.r > colorResult.g )
				//	{
				//		if( colorResult.r > colorResult.b )
				//			fLowerRate = 255.0f/colorResult.r;
				//		else
				//			fLowerRate = 255.0f/colorResult.b;
				//	}
				//	else
				//	{
				//		if( colorResult.g > colorResult.b )
				//			fLowerRate = 255.0f/colorResult.g;
				//		else
				//			fLowerRate = 255.0f/colorResult.b;
				//	}
				//}
				/*pOutPixel[ ( h*nTexSize + w )*3 + 0 ] = (BYTE)( colorResult.r*fLowerRate );
				pOutPixel[ ( h*nTexSize + w )*3 + 1 ] = (BYTE)( colorResult.g*fLowerRate );
				pOutPixel[ ( h*nTexSize + w )*3 + 2 ] = (BYTE)( colorResult.b*fLowerRate );*/

				pOutPixel[ ( h*nTexSize + w )*3 + 0 ] = (BYTE)( CsMin( 0xff, colorResult.r ) );
				pOutPixel[ ( h*nTexSize + w )*3 + 1 ] = (BYTE)( CsMin( 0xff, colorResult.g ) );
				pOutPixel[ ( h*nTexSize + w )*3 + 2 ] = (BYTE)( CsMin( 0xff, colorResult.b ) );
			}
		}
		SAFE_DELETE_ARRAY( pAlphaPixel );
		SAFE_DELETE_ARRAY( pShadowPixel );

		for( int i=0; i<5; ++i )
		{
			NiDelete pShaderMap[ i ];
			pDestPixelData[ i ] = NULL;
		}
	}

	// ===== 다 만들었으면 경계선 복사 하자
	NiPixelData** ppPixelData = csnew NiPixelData*[ m_Info.s_nTotalLeafCount ];
	int nStreamIndex = 0;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == true )
		{
			ppPixelData[ i ] = (NiPixelData*)stream.GetObjectAt( nStreamIndex );
			assert_cs( ppPixelData[ i ] != NULL );
			++nStreamIndex;
		}
		else
		{
			ppPixelData[ i ] = NULL;
		}
	}
	m_Info.CopyTextureBorder( m_Info.s_nBaseMPPG, ppPixelData );
	SAFE_DELETE_ARRAY( ppPixelData );

	// ===== 저장
	SAVE_TEXTURE_FILE( stream, szFile, "_BM.cst" );

	// 메모리 할당 객체 해제
	int nStreamCount = stream.GetObjectCount();
	for( int i=0; i<nStreamCount; ++i )
	{
		NiPixelDataPtr pd = (NiPixelData*)stream.GetObjectAt( i );
		pd = 0;
	}
}

void CsGBTerrainRoot::_SaveAMTexture( LPCTSTR szFile )
{
	NiStream stream;

	CsGBTerrainLeaf* pLeaf = NULL;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == false )
		{
			continue;
		}

		stream.InsertObject( GET_SHADERMAP_PIXELDATA( pLeaf->GetNiNode(), ALPHA_MAP_SHADER_NTM ) );
	}

	// 저장
	SAVE_TEXTURE_FILE( stream, szFile, "_AM.cst" );
}

void CsGBTerrainRoot::_SaveAMswTexture( LPCTSTR szFile )
{
	NiStream stream;
	NiNodePtr pNiInfoNode = NiNew NiNode;
	stream.InsertObject( pNiInfoNode );

	CsGBTerrainLeaf* pLeaf = NULL;
	CsGBTerrainLeaf::sNiTriINFO* pLeafInfo = NULL;
	for( int i=0; i<m_Info.s_nTotalLeafCount; ++i )
	{
		pLeaf = GetNode( i );
		if( pLeaf->IsExistTerrain() == false )
		{
			continue;
		}

		pLeafInfo = pLeaf->GetNiTriInfo();		
		NiPixelData* pPixelData = GET_SHADERMAP_PIXELDATA( pLeaf->GetNiNode(), ALPHA_MAP_SHADER_NTM );
		int nPixelCount = pPixelData->GetWidth()*pPixelData->GetHeight();
		int nPixelStride = pPixelData->GetPixelStride();
		BYTE* pPixel = (BYTE*)pPixelData->GetPixels();

		for( int ntm=0; ntm<4; ++ntm )
		{
			if( ( ntm == 0 )&&( pLeafInfo->s_cDetail_Map1[ 0 ] == NULL ) )
				continue;
			else if( ( ntm == 1 )&&( pLeafInfo->s_cDetail_Map2[ 0 ] == NULL ) )
				continue;
			else if( ( ntm == 2 )&&( pLeafInfo->s_cDetail_Map3[ 0 ] == NULL ) )
				continue;
			else if( ( ntm == 3 )&&( pLeafInfo->s_cDetail_Map4[ 0 ] == NULL ) )
				continue;

			NiPixelDataPtr pCreatePixelData = NiNew NiPixelData( pPixelData->GetWidth(), pPixelData->GetHeight(), NiPixelFormat::RGBA32 );
			unsigned int nCreatePixelStride = pCreatePixelData->GetPixelStride();
			BYTE* pCreatePixel = pCreatePixelData->GetPixels();
			for( int nCount = 0; nCount < nPixelCount; ++nCount )
			{
				pCreatePixel[ nCount*nCreatePixelStride + 3 ] = pPixel[ nCount*nPixelStride + ntm ];
			}
			
			sCST_INFO* pCstInfo = NiAlloc( sCST_INFO, 1 );
			unsigned int uiSize = sizeof( sCST_INFO );
			pCstInfo->s_usNodeIndex = i;
			pCstInfo->s_usShaderIndex = ALPHA_MAP_SHADER_NTM + ntm;
			NiBinaryExtraData* pExtData = NiNew NiBinaryExtraData( uiSize, (char*)pCstInfo );
			char cKey[ 64 ];
			sprintf_s( cKey, 64, "AMswObject_%d", stream.GetObjectCount() );
			pNiInfoNode->AddExtraData( cKey, pExtData );

			stream.InsertObject( pCreatePixelData );
		}
	}

	// 저장
	if( stream.GetObjectCount() > 1 )
	{		
		SAVE_TEXTURE_FILE( stream, szFile, "_AMsw.cst" );
	}
	// 메모리 제거
	pNiInfoNode = 0;
}



