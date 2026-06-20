
#include "stdafx.h"
#include "CsGBShadow.h"

CsGBShadow*		g_pShadow = NULL;

#define DS_REGION_SIZE		3000.0f		//2015-12-03-nova 그림자 표시 영역

CsGBShadow::CsGBShadow()
{
	m_pRTZBufferGroup = NULL;
	m_pRTZBufferTex = NULL;

	m_pShadowLight = NULL;

	m_pZSAlpha = NULL;
	m_pZSCharObject = NULL;
	m_pZSObject = NULL;
}

CsGBShadow::~CsGBShadow()
{
}

void CsGBShadow::Destroy()
{
	// 강제적으로 레퍼런스 카운트 1 올려주었기 때문에
	// 1낮추어주자
	if( m_pZSAlpha )
	{
		m_pZSAlpha->DecRefCount();
		m_pZSAlpha = NULL;
	}
	if( m_pZSCharObject )
	{
		m_pZSCharObject->DecRefCount();
		m_pZSCharObject = NULL;
	}
	if( m_pZSObject )
	{
		m_pZSObject->DecRefCount();
		m_pZSObject = NULL;
	}

	m_pRTZBufferGroup = 0;
	m_pRTZBufferTex = 0;
	m_pShadowLight = NULL;
}

void CsGBShadow::GlobalShotInit()
{
	assert_cs( g_pShadow == NULL );
	g_pShadow = NiNew CsGBShadow;
	g_pShadow->Init( 1024, 1024 );
}

void CsGBShadow::GlobalShotDown()
{
	if( g_pShadow != NULL )
	{
		g_pShadow->Destroy();
		delete g_pShadow;
		g_pShadow = NULL;	
	}	
}

void CsGBShadow::Init( int sx, int sy )
{
	m_matTexAdjust = D3DXMATRIX(	0.5f,0.0f,0.0f,0.0f,
									0.0f,-0.5f,0.0f,0.0f,
									0.0f,0.0f,1.0f,0.0f,
									0.5f,0.5f,0.0f,1.0f	);

	assert_cs( m_pRTZBufferGroup == NULL );
	assert_cs( m_pRTZBufferTex == NULL );	

	// 깊이 버퍼 그림자
	m_pRTZBufferTex = NiRenderedTexture::Create( (UINT)sx, (UINT)sy, nsCsGBTerrain::g_pRenderer );
	if( m_pRTZBufferTex == NULL )
	{
		m_pRTZBufferTex = NiRenderedTexture::Create( (UINT)(sx/2), (UINT)(sy/2), nsCsGBTerrain::g_pRenderer );
		if( m_pRTZBufferTex == NULL )
			return;
	}
	m_pRTZBufferGroup = NiRenderTargetGroup::Create( m_pRTZBufferTex->GetBuffer(), nsCsGBTerrain::g_pRenderer, false, true );	
	if( m_pRTZBufferGroup == NULL )
		return;

	// 다시 그리는 설정
	/*nsCsGBTerrain::g_pRenderer->BeginUsingRenderTargetGroup( m_pRTZBufferGroup, NiRenderer::CLEAR_ALL );
	nsCsGBTerrain::g_pRenderer->EndUsingRenderTargetGroup();*/

	
	//2015-11-26-nova m_ShadowCamera->GET_SUBCAMERA(CAMERA_01) 변경
 	GET_SUBCAMERA(CAMERA_01)->Create(nsCsGBTerrain::g_pRenderer);
// 	GET_SUBCAMERA(CAMERA_01)->SetOrthoFrustum( DS_REGION_SIZE, DS_REGION_SIZE );

	ResetDevice();
}

void CsGBShadow::ResetDevice()
{
	for( int i=0; i<nsCsGBTerrain::g_Device.g_nMultiTextureCount; ++i )
		nsCsGBTerrain::g_pRenderer->GetD3DDevice()->SetSamplerState( i, D3DSAMP_BORDERCOLOR, 0xffffffff );

//	GET_SUBCAMERA(CAMERA_01)->Create(nsCsGBTerrain::g_pRenderer);
	//2016-01-11-nova 그림자
	GET_SUBCAMERA(CAMERA_01)->SetOrthoFrustum( DS_REGION_SIZE, DS_REGION_SIZE );	

	GET_SUBCAMERA(CAMERA_01)->_UpdateCamera();
}

//==========================================================================================
//
//	Client
//
//==========================================================================================

void CsGBShadow::ResetMap()
{
	// 포인터 가져오기
	m_pShadowLight = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetDynamicShadowLight();
	// 그림자 색 쉐이더 설정	
	if( m_pShadowLight != NULL )
	{
		NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_ZS_ShadowColor", sizeof( float )*3, &m_pShadowLight->GetBaseInfo()->s_Ambient );
		NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_ZS_ShadowColor_TO", sizeof( float )*3, &m_pShadowLight->GetBaseInfo()->s_Ambient );
	}
}

bool CsGBShadow::BeginShadow( NiPoint3 ptActorPos )
{
	if( m_pShadowLight == NULL )
		return false;

	if( m_pRTZBufferGroup == NULL )
		return false;

	nsCsGBTerrain::g_pRenderer->EndUsingRenderTargetGroup();

	/*NiPoint3 ptDelta = CAMERA_ST.GetWorldDirection();
	ptDelta.z = 0.0f;
	ptDelta.Unitize();
	ptActorPos += ptDelta * ( DS_REGION_SIZE - 1400.0f );*/

	// 라이트별 카메라 셋팅
	NiPoint3 ptDir = ( (NiDirectionalLight*)m_pShadowLight->GetNiLight() )->GetWorldDirection();			

	//2015-11-26-nova
	GET_SUBCAMERA(CAMERA_01)->SetRotation( ptDir );			
	GET_SUBCAMERA(CAMERA_01)->SetTranslate( ptActorPos );
	GET_SUBCAMERA(CAMERA_01)->SetDist( 50000.0f );	
	GET_SUBCAMERA(CAMERA_01)->ApplyFrustumPlane( true );
	GET_SUBCAMERA(CAMERA_01)->_UpdateCamera();

	// 다시 그리는 설정
	nsCsGBTerrain::g_pRenderer->SetBackgroundColor( NiColorA::WHITE );
	nsCsGBTerrain::g_pRenderer->BeginUsingRenderTargetGroup( m_pRTZBufferGroup, NiRenderer::CLEAR_ALL );
	nsCsGBTerrain::g_pRenderer->SetCameraData( GET_SUBCAMERA(CAMERA_01)->GetCameraObj() );	//2015-11-26-nova

	return true;
}

void CsGBShadow::EndShadow( NiRenderTargetGroup* pOrgTarget )
{
	// 랜더링 종료
	nsCsGBTerrain::g_pRenderer->EndUsingRenderTargetGroup();

	D3DXMATRIX matViewProj, matViewProjAdj;	
	D3DXMatrixMultiply( &matViewProj, (D3DXMATRIX*)&nsCsGBTerrain::g_pRenderer->GetD3DView(), (D3DXMATRIX*)&nsCsGBTerrain::g_pRenderer->GetD3DProj() );
	D3DXMatrixMultiply( &matViewProjAdj, &matViewProj, &m_matTexAdjust );
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_LightVP", sizeof( float )*16, &matViewProj );	
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_LightVPT", sizeof( float )*16, &matViewProjAdj );
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_LightVP_TO", sizeof( float )*16, &matViewProj );	
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_LightVPT_TO", sizeof( float )*16, &matViewProjAdj );	

	// 설정 원래대로 변경
	nsCsGBTerrain::g_pRenderer->SetBackgroundColor( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_BackGroundColor );
	nsCsGBTerrain::g_pRenderer->BeginUsingRenderTargetGroup( pOrgTarget, NiRenderer::CLEAR_NONE );
	nsCsGBTerrain::g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );	
}


//==========================================================================================
//
//	TOOL
//
//==========================================================================================
void CsGBShadow::Tool_ResetMap( NiColor colorShadow )
{
	// 그림자 색 쉐이더 설정	
	Tool_ResetObjectShadowLight();
}

void CsGBShadow::Tool_ResetObjectShadowLight()
{
	// 그림자 생성 라이트 포인터 연결
	m_pShadowLight = NULL;
	std::list< CsGBLight* >* pLightList = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetLightList();
	CsGBLight* pLight;
	std::list< CsGBLight* >::iterator it = pLightList->begin();
	for( ; it!=pLightList->end(); ++it )
	{
		pLight = *it;
		if( pLight->GetBaseInfo()->s_bEnableLight == false )
			continue;
		if( pLight->GetBaseInfo()->s_bObjectShadow == true )
		{
			m_pShadowLight = pLight;
			break;
		}
	}
}
void CsGBShadow::Tool_RenderTRShadow( NiPoint3 ptTerrainPos )
{
	if( m_pShadowLight == NULL )
		return;

	// 라이트별 카메라 셋팅
	_Tool_BeginLight2Camera( ptTerrainPos );

	// 다시 그리는 설정
	nsCsGBTerrain::g_pRenderer->SetBackgroundColor( NiColorA::WHITE );
	nsCsGBTerrain::g_pRenderer->BeginUsingRenderTargetGroup( m_pRTZBufferGroup, NiRenderer::CLEAR_ALL );
	nsCsGBTerrain::g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );	

	// 쉐이더 변경
	g_Sorting.SetShader_GetterTRObject( GetZSAlphaShader() );

	// 오브젝트 랜더링
	nsCsGBTerrain::g_pTRMng->RenderObject( false );
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	// 쉐이더 다시 원래 대로 복귀
	g_Sorting.SetShader_GetterTRObject( nsCsGBTerrain::g_pRenderer->GetDefaultMaterial() );

	// 랜더링 종료
	nsCsGBTerrain::g_pRenderer->EndUsingRenderTargetGroup();

	D3DXMATRIX matViewProj, matViewProjAdj;	
	D3DXMatrixMultiply( &matViewProj, (D3DXMATRIX*)&nsCsGBTerrain::g_pRenderer->GetD3DView(), (D3DXMATRIX*)&nsCsGBTerrain::g_pRenderer->GetD3DProj() );
	D3DXMatrixMultiply( &matViewProjAdj, &matViewProj, &m_matTexAdjust );
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_LightVP", sizeof( float )*16, &matViewProj );	
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_LightVPT", sizeof( float )*16, &matViewProjAdj );

	_Tool_EndLight2Camera();

	// 설정 원래대로 변경
	nsCsGBTerrain::g_pRenderer->SetBackgroundColor( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_BackGroundColor );
}

void CsGBShadow::_Tool_BeginLight2Camera( NiPoint3 ptActorPos )
{
	CAMERA_ST.SaveCameraState();

	switch( m_pShadowLight->GetType() )
	{
	case CsGBLight::DIRECTION:
		{
			NiPoint3 ptDir = ( (NiDirectionalLight*)m_pShadowLight->GetNiLight() )->GetWorldDirection();			
			NiPoint3 posActor = ptActorPos;
			CAMERA_ST.SetRotation( ptDir );
			CAMERA_ST.SetTranslate( posActor );
			CAMERA_ST.SetDist( 50000.0f );
			CAMERA_ST.SetOrthoFrustum( 6000.0f, 6000.0f );
			CAMERA_ST._UpdateCamera();
		}
		break;
	case CsGBLight::POINT:
		{
			NiPoint3 ptLight = ( (CsGBLight_Point*)m_pShadowLight )->GetInfo()->s_Pos;
			NiPoint3 ptDir = ptActorPos - ptLight;
			CAMERA_ST.SetRotation( ptDir );
			CAMERA_ST.SetTranslate( ptLight );
			CAMERA_ST._UpdateCamera();
		}
		break;
	default:
		assert_cs( false );
	}	
}

void CsGBShadow::_Tool_EndLight2Camera()
{
	switch( m_pShadowLight->GetType() )
	{
	case CsGBLight::DIRECTION:
		{
			CAMERA_ST.SetStandardFrustum();
		}
		break;
	case CsGBLight::POINT:
		{
		}
		break;
	default:
		assert_cs( false );
	}

	CAMERA_ST.LoadCameraState();	
}


//==========================================================================================
//
//		Shadow Map		PreCreate
//
//==========================================================================================

void CsGBShadow::Tool_CopyBorderShadowMap()
{
	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	// ===== 다 만들었으면 경계선 복사 하자
	NiPixelData** ppPixelData = csnew NiPixelData*[ pRootInfo->s_nTotalLeafCount ];
	for( int i=0; i<pRootInfo->s_nTotalLeafCount; ++i )
	{
		ppPixelData[ i ] = GET_SHADERMAP_PIXELDATA( nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetNiNode(), SHADOW_MAP_SHADER_NTM );
	}

	pRootInfo->CopyTextureBorder( pRootInfo->s_nShadowMPPG, ppPixelData );

	SAFE_DELETE_ARRAY( ppPixelData );
}

void CsGBShadow::Tool_InitApplyTRShadowMap( sCS_BMPDATA** ppBmpData, D3DXMATRIX** ppLVP, D3DXMATRIX** ppLVPT )
{		
	nsCsGBTerrain::g_pRenderer->SetBackgroundColor( NiColorA::WHITE );
	CAMERA_ST.SaveCameraState();

	assert_cs( m_pShadowLight != NULL );
	switch( m_pShadowLight->GetType() )
	{
	case CsGBLight::DIRECTION:
		_Tool_InitApplyTRShadowMap_DirectionLight( ppBmpData, ppLVP, ppLVPT );
		break;
	case CsGBLight::POINT:
		_Tool_InitApplyTRShadowMap_PointLight();
		break;
	default:
		assert_cs( false );
	}
}

void CsGBShadow::Tool_ApplyTRShadowMap( int nLeafIndex, sCS_BMPDATA* pBmpData, D3DXMATRIX* pLVP, D3DXMATRIX* pLVPT )
{
	switch( m_pShadowLight->GetType() )
	{
	case CsGBLight::DIRECTION:
		_Tool_ApplyTRShadowMap_DirectionLight( nLeafIndex, pBmpData, pLVP, pLVPT );
		break;
	case CsGBLight::POINT:
		_Tool_ApplyTRShadowMap_PointLight( nLeafIndex );
		break;
	default:
		assert_cs( false );
	}
}


void CsGBShadow::Tool_EndApplyTRShadowMap( sCS_BMPDATA** ppBmpData, D3DXMATRIX** ppLVP, D3DXMATRIX** ppLVPT )
{
	// 셋팅 복귀
	nsCsGBTerrain::g_pRenderer->SetBackgroundColor( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_BackGroundColor );
	CAMERA_ST.SetStandardFrustum();
	CAMERA_ST.LoadCameraState();

	nsCsGBTerrain::g_pRenderer->Recreate( 800, 600 );

	switch( m_pShadowLight->GetType() )
	{
	case CsGBLight::DIRECTION:
		_Tool_EndApplyTRShadowMap_DirectionLight( ppBmpData, ppLVP, ppLVPT );
		break;
	case CsGBLight::POINT:
		_Tool_EndApplyTRShadowMap_PointLight();
		break;
	default:
		assert_cs( false );
	}	
}

void CsGBShadow::Tool_ApplyDMObjectShadowMap()
{
	NiColor color = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_ShadowColor;	
	NiD3DShaderFactory::UpdateGlobalShaderConstant( "g_colorTBObject_Shadow", sizeof( float )*3, &color );

	g_Sorting.ClearGetter();

	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	MAP_GBTERRAIN_OBJECT* pMap;
	MAP_GBTERRAIN_OBJECT_IT it;
	std::map< DWORD, bool > mapExist;
	for( int i=0; i<pInfo->s_nTotalLeafCount; ++i )
	{
		// TB
		pMap = nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetObjectMap();
		it = pMap->begin();
		for( ; it!=pMap->end(); ++it )
		{
			if( it->second->UD_IsDarkMap() == true )
			{
				// 같은 오브젝트 재 호출 안하기 위해서
				if( mapExist.find( it->second->GetInfo()->s_dwUniqID ) == mapExist.end() )
				{
					switch( m_pShadowLight->GetType() )
					{
					case CsGBLight::DIRECTION:
						_Tool_CreateDMObjectShadowMap_DirectionLight( it->second );
						break;
					case CsGBLight::POINT:						
						_Tool_CreateDMObjectShadowMap_PointLight( it->second, i );
						break;
					default:
						assert_cs( false );
					}					
					mapExist[ it->second->GetInfo()->s_dwUniqID ] = true;
				}
			}
		}
	}	
}

void CsGBShadow::_Tool_CalLightTRColor( std::list< CsGBLight* >* pLightList, NiColor& ResultColor, float& fRate, NiPoint3 ptPos, NiPoint3 ptNormal )
{
	CsGBLight::sBASE_INFO* pBaseInfo;
	CsGBLight* pLight;
	NiDirectionalLight* pDirectionLight;
	CsGBLight_Point::sINFO* pPointLightInfo;

	NiPoint3 ptLightPos, ptLightDir;
	float fDist, fLightDot, fPointLightConstant;

	//=======================================================================
	//============= Light 명암
	std::list< CsGBLight* >::iterator it = pLightList->begin();
	for( ; it!=pLightList->end(); ++it )
	{
		pLight = *it;

		if( pLight->GetBaseInfo()->s_bEnableLight == false )
			continue;

		if( pLight->GetBaseInfo()->s_bTRShadow )
		{
			switch( pLight->GetType() )
			{
			case CsGBLight::DIRECTION:
				{
					pBaseInfo = pLight->GetBaseInfo();
					pDirectionLight = (NiDirectionalLight*)pLight->GetNiLight();
					fLightDot = CsMax( 0.0f, ptNormal.Dot( -pDirectionLight->GetWorldDirection() ) );
					fLightDot *= fLightDot;
					fLightDot *= fLightDot;
					ResultColor += ( fLightDot*pBaseInfo->s_Diffuse + pBaseInfo->s_Ambient )*pBaseInfo->s_fPower;
				}
				break;
			case CsGBLight::POINT:
				{
					pBaseInfo = pLight->GetBaseInfo();
					pPointLightInfo = ( (CsGBLight_Point*)pLight )->GetInfo();
					ptLightPos = pPointLightInfo->s_Pos;
					ptLightDir = ptPos - ptLightPos;
					fDist = ptLightDir.Unitize();
					if( fDist > pPointLightInfo->s_Range )
					{
						break;
					}

					fPointLightConstant = pPointLightInfo->CalConstant( fDist );
					fLightDot = CsMax( 0.0f, ptNormal.Dot( -ptLightDir ) );
					ResultColor += ( fLightDot*pBaseInfo->s_Diffuse + pBaseInfo->s_Ambient )*fPointLightConstant*pBaseInfo->s_fPower;
				}
				break;
			}
		}					
	}

	fRate = 1.0f;
	//  가장 높은 값이 2.0보다 높다면 2.0으로 맞춰주고 다른 값을 비율로 낮춰 준다
	if( ResultColor != NiColor::BLACK )
	{
		if( ResultColor.r > ResultColor.g )
		{
			if( ResultColor.r > ResultColor.b )
				fRate = 2.0f/ResultColor.r;
			else
				fRate = 2.0f/ResultColor.b;
		}
		else
		{
			if( ResultColor.g > ResultColor.b )
				fRate = 2.0f/ResultColor.g;
			else
				fRate = 2.0f/ResultColor.b;
		}
	}
}

void CsGBShadow::_Tool_InitApplyTRShadowMap_DirectionLight( sCS_BMPDATA** ppBmpData, D3DXMATRIX** ppLVP, D3DXMATRIX** ppLVPT )
{
	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	// 백버퍼를 텍스쳐 사이즈로
	int nCreateMapSize = pRootInfo->s_nInitMapSize*pRootInfo->s_nShadowMPPG;
	nsCsGBTerrain::g_pRenderer->Recreate( nCreateMapSize, nCreateMapSize );

	// 카메라 상태 저장
	CAMERA_ST.SaveCameraState();

	// 라이트 방향으로 회전
	CAMERA_ST.SetRotation( ( (NiDirectionalLight*)GetShadowLight()->GetNiLight() )->GetWorldDirection() );

	// 직교 투영으로 변환
	float nSizeX = (float)( (pRootInfo->s_nXVertCountInNode-1)*pRootInfo->s_nGridSize );
	float nSizeY = (float)( (pRootInfo->s_nYVertCountInNode-1)*pRootInfo->s_nGridSize );
	NiPoint2 ptOffset = NiPoint2( nSizeX*0.5f, nSizeY*0.5f	);
	NiPoint2 ptOrthoFrustum = ptOffset*1.5f;

	// 데이터 메모리 할당
	int nTexRenderCount = pRootInfo->s_nTotalLeafCount + pRootInfo->s_nNodeCountInSide*4 + 4;	
	*ppBmpData = csnew sCS_BMPDATA[ nTexRenderCount ];
	*ppLVP = csnew D3DXMATRIX[ nTexRenderCount ];
	*ppLVPT = csnew D3DXMATRIX[ nTexRenderCount ];

	// 오브젝트 랜더링 준비 및 업데이트
	nsCsGBTerrain::g_pTRMng->BeginRender();
	nsCsGBTerrain::g_pTRMng->Update_For_Shadow( 0.0f );
	// 업데이트된 오브젝트에 쉐이더 설정
	g_Sorting.SetShader_GetterTRObject( GetZSAlphaShader() );

	int nTotalLeaf = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	CsGBTerrainNode* pRoot = nsCsGBTerrain::g_pCurRoot->GetRootNode();
	for( int i=0; i<nTotalLeaf; ++i )
	{
		nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetNiTriStrip()->ApplyAndSetActiveMaterial( NiSingleShaderMaterial::Create( "TR_WHITE_COLOR_TOOLUSE" ) );
	}

	TCHAR szName[ 32 ];
	NiPoint3 ptPos;
	for( int i=0; i<pRootInfo->s_nTotalLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		//========== 쉐이더 텍스쳐 생성
		pLeafNode->CreateShadowMap_ByTool();

		// 이미지 이름		
		_stprintf_s( szName, 32, _T( "temp%d.bmp" ), i );
		// 위치
		ptPos = NiPoint3( pLeafNode->GetNiTriInfo()->s_ptOffsetPos.x + ptOffset.x, pLeafNode->GetNiTriInfo()->s_ptOffsetPos.y + ptOffset.y, 0 );
		ptPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( ptPos.x, ptPos.y );
		_Tool_ZShadow2Bmp( szName, ptPos, ptOrthoFrustum, &( *ppBmpData )[ i ], &( *ppLVP )[ i ], &( *ppLVPT )[ i ] );		
	}

	int nSizeTexIndex = pRootInfo->s_nTotalLeafCount;
	for( int i=0; i<pRootInfo->s_nTotalLeafCount; ++i )
	{
		int nIX = i%pRootInfo->s_nNodeCountInSide;
		int nIY = i/pRootInfo->s_nNodeCountInSide;

		CsGBTerrainLeaf* pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		ptPos = NiPoint3( pLeafNode->GetNiTriInfo()->s_ptOffsetPos.x + ptOffset.x, pLeafNode->GetNiTriInfo()->s_ptOffsetPos.y + ptOffset.y, 0 );
		ptPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( ptPos.x, ptPos.y );

		if( nIY == 0 )
		{
			if( nIX == 0 )
			{
				_stprintf_s( szName, 32, _T( "temp%d.bmp" ), nSizeTexIndex );
				// 위치
				NiPoint3 ptNewPos = ptPos;
				ptNewPos.x -= nSizeX;
				ptNewPos.y -= nSizeY;
				_Tool_ZShadow2Bmp( szName, ptNewPos, ptOrthoFrustum, &( *ppBmpData )[ nSizeTexIndex ], &( *ppLVP )[ nSizeTexIndex ], &( *ppLVPT )[ nSizeTexIndex ] );
				++nSizeTexIndex;
			}

			_stprintf_s( szName, 32, _T( "temp%d.bmp" ), nSizeTexIndex );
			// 위치
			NiPoint3 ptNewPos = ptPos;
			ptNewPos.y -= nSizeY;			
			_Tool_ZShadow2Bmp( szName, ptNewPos, ptOrthoFrustum, &( *ppBmpData )[ nSizeTexIndex ], &( *ppLVP )[ nSizeTexIndex ], &( *ppLVPT )[ nSizeTexIndex ] );
			++nSizeTexIndex;

			if( nIX == pRootInfo->s_nNodeCountInSide - 1 )
			{
				_stprintf_s( szName, 32, _T( "temp%d.bmp" ), nSizeTexIndex );
				// 위치
				NiPoint3 ptNewPos = ptPos;
				ptNewPos.x += nSizeX;
				ptNewPos.y -= nSizeY;
				_Tool_ZShadow2Bmp( szName, ptNewPos, ptOrthoFrustum, &( *ppBmpData )[ nSizeTexIndex ], &( *ppLVP )[ nSizeTexIndex ], &( *ppLVPT )[ nSizeTexIndex ] );
				++nSizeTexIndex;
			}
		}		
	}

	for( int i=0; i<pRootInfo->s_nTotalLeafCount; ++i )
	{
		int nIX = i%pRootInfo->s_nNodeCountInSide;
		int nIY = i/pRootInfo->s_nNodeCountInSide;

		CsGBTerrainLeaf* pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		ptPos = NiPoint3( pLeafNode->GetNiTriInfo()->s_ptOffsetPos.x + ptOffset.x, pLeafNode->GetNiTriInfo()->s_ptOffsetPos.y + ptOffset.y, 0 );
		ptPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( ptPos.x, ptPos.y );

		if( nIX == 0 )
		{
			_stprintf_s( szName, 32, _T( "temp%d.bmp" ), nSizeTexIndex );
			// 위치
			NiPoint3 ptNewPos = ptPos;
			ptNewPos.x -= nSizeX;			
			_Tool_ZShadow2Bmp( szName, ptNewPos, ptOrthoFrustum, &( *ppBmpData )[ nSizeTexIndex ], &( *ppLVP )[ nSizeTexIndex ], &( *ppLVPT )[ nSizeTexIndex ] );
			++nSizeTexIndex;
		}
		else if( nIX == pRootInfo->s_nNodeCountInSide - 1 )
		{
			_stprintf_s( szName, 32, _T( "temp%d.bmp" ), nSizeTexIndex );
			// 위치
			NiPoint3 ptNewPos = ptPos;
			ptNewPos.x += nSizeX;			
			_Tool_ZShadow2Bmp( szName, ptNewPos, ptOrthoFrustum, &( *ppBmpData )[ nSizeTexIndex ], &( *ppLVP )[ nSizeTexIndex ], &( *ppLVPT )[ nSizeTexIndex ] );
			++nSizeTexIndex;
		}
	}
	for( int i=0; i<pRootInfo->s_nTotalLeafCount; ++i )
	{
		int nIX = i%pRootInfo->s_nNodeCountInSide;
		int nIY = i/pRootInfo->s_nNodeCountInSide;

		CsGBTerrainLeaf* pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		ptPos = NiPoint3( pLeafNode->GetNiTriInfo()->s_ptOffsetPos.x + ptOffset.x, pLeafNode->GetNiTriInfo()->s_ptOffsetPos.y + ptOffset.y, 0 );
		ptPos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( ptPos.x, ptPos.y );

		if( nIY == pRootInfo->s_nNodeCountInSide - 1 )
		{
			if( nIX == 0 )
			{
				_stprintf_s( szName, 32, _T( "temp%d.bmp" ), nSizeTexIndex );
				// 위치
				NiPoint3 ptNewPos = ptPos;
				ptNewPos.x -= nSizeX;
				ptNewPos.y += nSizeY;
				_Tool_ZShadow2Bmp( szName, ptNewPos, ptOrthoFrustum, &( *ppBmpData )[ nSizeTexIndex ], &( *ppLVP )[ nSizeTexIndex ], &( *ppLVPT )[ nSizeTexIndex ] );
				++nSizeTexIndex;
			}

			_stprintf_s( szName, 32, _T( "temp%d.bmp" ), nSizeTexIndex );
			// 위치
			NiPoint3 ptNewPos = ptPos;
			ptNewPos.y += nSizeY;			
			_Tool_ZShadow2Bmp( szName, ptNewPos, ptOrthoFrustum,&( *ppBmpData )[ nSizeTexIndex ], &( *ppLVP )[ nSizeTexIndex ], &( *ppLVPT )[ nSizeTexIndex ] );
			++nSizeTexIndex;

			if( nIX == pRootInfo->s_nNodeCountInSide - 1 )
			{
				_stprintf_s( szName, 32, _T( "temp%d.bmp" ), nSizeTexIndex );
				// 위치
				NiPoint3 ptNewPos = ptPos;
				ptNewPos.x += nSizeX;
				ptNewPos.y += nSizeY;
				_Tool_ZShadow2Bmp( szName, ptNewPos, ptOrthoFrustum, &( *ppBmpData )[ nSizeTexIndex ], &( *ppLVP )[ nSizeTexIndex ], &( *ppLVPT )[ nSizeTexIndex ] );
				++nSizeTexIndex;
			}
		}
	}

	g_Sorting.SetShader_GetterTRObject( nsCsGBTerrain::g_pRenderer->GetDefaultMaterial() );
	g_Sorting.ClearGetter();

	pRoot = nsCsGBTerrain::g_pCurRoot->GetRootNode();
	for( int i=0; i<nTotalLeaf; ++i )
	{
		nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetNiTriStrip()->ApplyAndSetActiveMaterial( NiSingleShaderMaterial::Create( "TR_TOOL_1111_8" ) );
	}
}

bool CsGBShadow::_Tool_ZShadow2Bmp( TCHAR* szBmpName, NiPoint3 ptPos, NiPoint2 ptOrthoSize, sCS_BMPDATA* pBmpData, D3DXMATRIX* pLVP, D3DXMATRIX* pLVPT )
{
	CAMERA_ST.SetTranslate( ptPos );
	CAMERA_ST.SetDist( 100000.0f );
	CAMERA_ST.SetOrthoFrustum( ptOrthoSize.x, ptOrthoSize.y );	
	CAMERA_ST._UpdateCamera();

	//nsCsGBTerrain::g_pCurRoot->GetRootNode()->UpdateTerrain( 0.0f );

	//========== 랜더링 시작
	nsCsGBTerrain::g_pRenderer->BeginOffScreenFrame();
	nsCsGBTerrain::g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );

	//========== 랜더링 설정
	NiRenderer::GetRenderer()->SetCameraData( CAMERA_ST.GetCameraObj() );

	//========== 랜더링
	nsCsGBTerrain::g_pTRMng->RenderObject_ByTool( 0.0, false );
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );
	//CsNiDrawTerrainScene();

	//========== 랜더링 종료
	nsCsGBTerrain::g_pRenderer->EndUsingRenderTargetGroup();
	nsCsGBTerrain::g_pRenderer->EndOffScreenFrame();

	// TexAdjust 행렬
	D3DXMATRIX matTexAdjust = D3DXMATRIX(	0.5f,0.0f,0.0f,0.0f,
		0.0f,-0.5f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.5f,0.5f,0.0f,1.0f	);
	// 라이트의 행렬들을 구해 놓는다
	D3DXMatrixMultiply( pLVP, (D3DXMATRIX*)&nsCsGBTerrain::g_pRenderer->GetD3DView(), (D3DXMATRIX*)&nsCsGBTerrain::g_pRenderer->GetD3DProj() );	
	D3DXMatrixMultiply( pLVPT, pLVP, &matTexAdjust );

	//=========== 파일로 저장
	LPDIRECT3DDEVICE9 pkDevice = nsCsGBTerrain::g_pRenderer->GetD3DDevice();
	LPDIRECT3DSURFACE9 pkDestSurface = NULL;
	HRESULT hr = pkDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pkDestSurface);
	assert_cs( SUCCEEDED( hr ) );

	// 이미지 이름
	hr = D3DXSaveSurfaceToFile( szBmpName, D3DXIFF_BMP, pkDestSurface, NULL, NULL);
	assert_cs( SUCCEEDED( hr ) );
	D3D_POINTER_RELEASE( pkDestSurface );

	if( pBmpData != NULL )
	{
		CsBmp::GetBmpData( szBmpName, pBmpData );
	}

	return true;
}

void CsGBShadow::_Tool_ApplyTRShadowMap_DirectionLight( int nLeafIndex, sCS_BMPDATA* pBmpData, D3DXMATRIX* pLVP, D3DXMATRIX* pLVPT )
{
	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	CsGBTerrainLeaf* pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( nLeafIndex );

	// Texture 관련
	NiPixelData* pPData = GET_SHADERMAP_PIXELDATA( pLeafNode->GetNiNode(), SHADOW_MAP_SHADER_NTM );
	BYTE* pPixel = (BYTE*)pPData->GetPixels();
	int nTextureWidth = pPData->GetWidth();
	int nTextureHeight = pPData->GetHeight();

	// 픽셀 리미트
	int nXStart = pRootInfo->GetTextureLeftFigure( pRootInfo->s_nShadowMPPG );
	int nXEnd = pRootInfo->GetTextureRightFigure( pRootInfo->s_nShadowMPPG ) + 1;
	int nYStart = pRootInfo->GetTextureTopFigure( pRootInfo->s_nShadowMPPG );
	int nYEnd = pRootInfo->GetTextureBottomFigure( pRootInfo->s_nShadowMPPG ) + 1;

	int nPixelIndex;
	NiColor ResultColor;
	NiPoint3 ptNormal;
	float fRate;

	int nBDIndex;

	std::list< CsGBLight* >* pLightList = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetLightList();
	int nTexRenderCount = pRootInfo->s_nTotalLeafCount + pRootInfo->s_nNodeCountInSide*4 + 4;	

	// 픽셀들 돌아 가면서 검사
	for( int y=nYStart; y<nYEnd; ++y )
	{
		for( int x=nXStart; x<nXEnd; ++x )
		{
			nPixelIndex = ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE;

			// 값 초기화
			ResultColor = NiColor::BLACK;

			D3DXVECTOR4 pos, depth, texProj;
			pRootInfo->TexturePixelToWorldPos( &pos.x, &pos.y, x, y, nLeafIndex, pRootInfo->s_nShadowMPPG );
			pos.z = nsCsGBTerrain::g_pCurRoot->GetHeight( pos.x, pos.y, nsCsGBTerrain::g_pCurRoot->GetOrgHeightField() );
			pos.w = 1.0f;

			D3DXVec4Transform( &depth, &pos, &pLVP[ nLeafIndex ] );
			D3DXVec4Transform( &texProj, &pos, &pLVPT[ nLeafIndex ] );

			float ftx = texProj.x / texProj.w;
			float fty = 1.0f - texProj.y / texProj.w;

			nBDIndex = nLeafIndex;


			int nIX, nIY;
			if( nBDIndex < pRootInfo->s_nTotalLeafCount )
			{
				nIX = nBDIndex % pRootInfo->s_nNodeCountInSide;
				nIY = nBDIndex / pRootInfo->s_nNodeCountInSide;
			}
			else
			{
				nIX = nBDIndex % ( pRootInfo->s_nNodeCountInSide + 2 );
				nIY = nBDIndex / ( pRootInfo->s_nNodeCountInSide + 2 );
			}

			int nTempIndex = 0;
			while( ( ftx < 0.0f )||( ftx > 1.0f )||( fty < 0.0f )||( fty > 1.0f ) )
			{
				D3DXVec4Transform( &texProj, &pos, &pLVPT[ nTempIndex ] );
				D3DXVec4Transform( &depth, &pos, &pLVP[ nTempIndex ] );
				ftx = texProj.x * 1/texProj.w;
				fty = 1.0f - texProj.y * 1/texProj.w;

				nBDIndex = nTempIndex;				
				++nTempIndex;

				if( nTempIndex == nTexRenderCount )
					break;
			}
			
			int nSize = pRootInfo->s_nInitMapSize*pRootInfo->s_nShadowMPPG;
			bool bObjectShadow = false;
			if( ( ftx >= 0.0f )&&( ftx <= 1.0f )&&
				( fty >= 0.0f )&&( fty <= 1.0f ) )
			{
				ftx = CsMin( (float)( nSize-1 ), ftx*nSize );
				fty = CsMin( (float)( nSize-1 ), fty*nSize );
				int tx = (int)ftx;
				int ty = (int)fty;
				float rx = ftx - tx;
				float ry = fty - ty;

				int nBit = pBmpData[ nBDIndex ].s_InfoHeader.biBitCount/8;
				float Tdepth;
			
				int tindex = ( ty*nSize + tx )*nBit;
				Tdepth =	CsB2F( pBmpData[ nBDIndex ].s_pData[ tindex + 2 ] )*1000.0f +
							CsB2F( pBmpData[ nBDIndex ].s_pData[ tindex + 1 ] ) +
							CsB2F( pBmpData[ nBDIndex ].s_pData[ tindex + 0 ] )*0.01f;

				if( ( depth.z/depth.w )*1000.0f + 0.5f > Tdepth )
				{
					bObjectShadow = true;
				}
			}

			// 값 초기화
			ResultColor = NiColor::BLACK;
			nsCsGBTerrain::g_pCurRoot->GetHeight_Normal( pos.x, pos.y, ptNormal );

			_Tool_CalLightTRColor( pLightList, ResultColor, fRate, NiPoint3( pos.x, pos.y, pos.z ), ptNormal );

			if( bObjectShadow == true )
			{
				ResultColor *= pRootInfo->s_ShadowColor;
			}

			if( fRate < 1.0f )
			{
				pPixel[ nPixelIndex + 0 ] = CsF2B( ResultColor.r*fRate*0.5f );
				pPixel[ nPixelIndex + 1 ] = CsF2B( ResultColor.g*fRate*0.5f );
				pPixel[ nPixelIndex + 2 ] = CsF2B( ResultColor.b*fRate*0.5f );
			}
			else
			{
				pPixel[ nPixelIndex + 0 ] = CsF2B( ResultColor.r*0.5f );
				pPixel[ nPixelIndex + 1 ] = CsF2B( ResultColor.g*0.5f );
				pPixel[ nPixelIndex + 2 ] = CsF2B( ResultColor.b*0.5f );
			}
		}
	}
	pPData->MarkAsChanged();
}


void CsGBShadow::_Tool_EndApplyTRShadowMap_DirectionLight( sCS_BMPDATA** ppBmpData, D3DXMATRIX** ppLVP, D3DXMATRIX** ppLVPT )
{
	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	int nTexRenderCount = pRootInfo->s_nTotalLeafCount + pRootInfo->s_nNodeCountInSide*4 + 4;	
	for( int i=0; i<nTexRenderCount; ++i )
	{
		CsBmp::DeleteData( &( *ppBmpData )[ i ] );

		// 이미지 이름
		TCHAR szName[ 32 ];
		_stprintf_s( szName, 32, _T( "temp%d.bmp" ), i );
		DeleteFile( szName );
	}

	SAFE_DELETE_ARRAY( *ppBmpData );
	SAFE_DELETE_ARRAY( *ppLVP );
	SAFE_DELETE_ARRAY( *ppLVPT );
}

void CsGBShadow::_Tool_CreateDMObjectShadowMap_DirectionLight( CsGBObject* pBaseObj )
{
	float fMapSize = (float)( (sUDD_DARKMAP*)pBaseObj->GetUserDefine()->GetData( sUDD_DARKMAP::Type() ) )->s_dwTerrainBaseShadowMapSize;

	// 랜더링
	_Tool_RenderBackBufferDMObject_DirectionLight( pBaseObj );

	// dds 로 저장
	LPDIRECT3DDEVICE9 pkDevice = nsCsGBTerrain::g_pRenderer->GetD3DDevice();
	LPDIRECT3DSURFACE9 pkDestSurface = NULL;
	HRESULT hr = pkDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pkDestSurface);
	assert_cs( SUCCEEDED(hr) );
	// Save the image to a file
	HRESULT hr2 = D3DXSaveSurfaceToFile( _T( "temp.dds" ), D3DXIFF_DDS, pkDestSurface, NULL, NULL);
	D3D_POINTER_RELEASE(pkDestSurface);

	// 해당 파일 리딩
	NiTexturingPropertyPtr pTexProp = NiNew NiTexturingProperty( "temp.dds" );
	NiSourceTexture* pTexture = (NiSourceTexture*)pTexProp->GetBaseMap()->GetTexture();
	assert_cs( NiIsKindOf( NiSourceTexture, pTexture ) == true );
	pTexture->LoadPixelDataFromFile();
	NiPixelData* pSrcPixelData = pTexture->GetSourcePixelData();
	int nSrcWidth = pSrcPixelData->GetWidth();
	int nSrcHeight = pSrcPixelData->GetHeight();
	int nSrcStride = pSrcPixelData->GetPixelStride();
	BYTE* pSrcPixel = (BYTE*)pSrcPixelData->GetPixels();

	// 텍스쳐로 저장	
	NiPixelData* pDestPixelData = NiNew NiPixelData( (UINT)fMapSize, (UINT)fMapSize, NiPixelFormat::RGB24 );
	int nDestStride = pDestPixelData->GetPixelStride();
	BYTE* pDestPixel = pDestPixelData->GetPixels();
	for( DWORD y=0; y<fMapSize; ++y )
	{
		for( DWORD x=0; x<fMapSize; ++x )
		{
			pDestPixel[ ( y*(DWORD)fMapSize + x )*nDestStride + 0 ] = pSrcPixel[ ( y*nSrcWidth + x )*nSrcStride + 0 ];
			pDestPixel[ ( y*(DWORD)fMapSize + x )*nDestStride + 1 ] = pSrcPixel[ ( y*nSrcWidth + x )*nSrcStride + 1 ];
			pDestPixel[ ( y*(DWORD)fMapSize + x )*nDestStride + 2 ] = pSrcPixel[ ( y*nSrcWidth + x )*nSrcStride + 2 ];
		}
	}
	assert_cs( nSrcWidth == pDestPixelData->GetWidth() );
	assert_cs( nSrcHeight == pDestPixelData->GetHeight() );

	// 템프 데이터 제거
	pTexProp = 0;
	DeleteFile( _T( "temp.dds" ) );


	// 행렬
	float fTexelOffs = 0.5f + ( 0.5f / fMapSize );	
	D3DXMATRIX matTexAdj(	0.5f, 0.0f, 0.0f, 0.0f,
							0.0f, -0.5f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							fTexelOffs,	 fTexelOffs, 1.0f, 1.0f);

	D3DXMATRIX matView = nsCsGBTerrain::g_pRenderer->GetD3DView();
	D3DXMATRIX matProj = nsCsGBTerrain::g_pRenderer->GetD3DProj();
	D3DXMATRIX matViewProj = matView*matProj;
	D3DXMATRIX matViewProjAdj = matViewProj*matTexAdj;

	// 텍스쳐
	NiSourceTexturePtr pSourceTex = NiSourceTexture::Create( pDestPixelData );

	_Tool_ApplyDrakMap_DMObject_DirectionLight( pBaseObj->GetNiNode(), pSourceTex, matViewProjAdj );				
}

void CsGBShadow::_Tool_RenderBackBufferDMObject_DirectionLight( CsGBObject* pBaseObj )
{
	NiBound Bound;
	float fBoundRadius, fMapSize;
	fMapSize = (float)( (sUDD_DARKMAP*)pBaseObj->GetUserDefine()->GetData( sUDD_DARKMAP::Type() ) )->s_dwTerrainBaseShadowMapSize;
	Bound = pBaseObj->GetNiNode()->GetWorldBound();
	fBoundRadius = Bound.GetRadius();

	// 백버퍼 사이즈 조절
	nsCsGBTerrain::g_pRenderer->Recreate( (int)fMapSize, (int)fMapSize );

	// 직교 투영으로
	CAMERA_ST.SetDist( 100000.0f );
	CAMERA_ST.SetTranslate( Bound.GetCenter() );
	CAMERA_ST.SetOrthoFrustum( fBoundRadius, fBoundRadius );	
	CAMERA_ST.Update_ByTool( 0.0f );

	nsCsGBTerrain::g_pTRMng->BeginRender();
	nsCsGBTerrain::g_pTRMng->Update_For_Shadow_ExceptionObject( 0.0f, pBaseObj );

	g_Sorting.SetShader_GetterTRObject( NiSingleShaderMaterial::Create( "TR_CONSTANT_COLOR_TOOLUSE" ) );

	pBaseObj->Update( 0.0f, false );
	pBaseObj->SetShader( NiSingleShaderMaterial::Create( "TR_WHITE_COLOR_TOOLUSE" ) );

	nsCsGBTerrain::g_pRenderer->BeginOffScreenFrame();
	nsCsGBTerrain::g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );

	//========== 랜더링 설정
	NiRenderer::GetRenderer()->SetCameraData( CAMERA_ST.GetCameraObj() );

	//========== 랜더링
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_F1, false );
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_NOCLIPPING, false );
	g_Sorting.RenderTRObject( false );
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	nsCsGBTerrain::g_pRenderer->EndUsingRenderTargetGroup();
	nsCsGBTerrain::g_pRenderer->EndOffScreenFrame();

	nsCsGBTerrain::g_pTRMng->EndRender();

	// 쉐이더 다시 원래 대로 복귀
	g_Sorting.SetShader_GetterTRObject( nsCsGBTerrain::g_pRenderer->GetDefaultMaterial() );
	g_Sorting.ClearGetter();
}

void CsGBShadow::_Tool_ApplyDrakMap_DMObject_DirectionLight( NiNode* pNiBase, NiSourceTexturePtr pSourceTexture, D3DXMATRIX matViewProjAdj )
{	
	assert_cs( NiIsKindOf( NiNode, pNiBase ) == true );

	int nChildCount = pNiBase->GetArrayCount();
	for( int i=0; i<nChildCount; ++i )
	{
		NiAVObject* pChild = pNiBase->GetAt( i );
		if( NiIsKindOf( NiNode, pChild ) == true )
		{
			_Tool_ApplyDrakMap_DMObject_DirectionLight( (NiNode*)pChild, pSourceTexture, matViewProjAdj );
		}
		else if( ( ( NiIsKindOf( NiTriStrips, pChild ) == true )||( NiIsKindOf( NiTriShape, pChild ) == true ) )&&
			( pChild->GetAppCulled() == false ) )
		{
			NiGeometry* pGeom = ( NiGeometry* )pChild;			
			NiTexturingProperty* pTexProp = (NiTexturingProperty*)pGeom->GetProperty( NiTexturingProperty::GetType() );			
			NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( pSourceTexture, 1, NiTexturingProperty::CLAMP_S_CLAMP_T );

			pTexProp->SetDarkMap( pMap );


			// 텍스쳐 uv셋팅
			NiPoint2* pUV = pGeom->GetTextureSet( 1 );
			NiPoint3* pPos = pGeom->GetVertices();
			NiPoint3* pNormal = pGeom->GetNormals();
			int nVertCount = pGeom->GetVertexCount();

			D3DXMATRIX matWorld, matResult;			
			NiD3DUtility::GetD3DFromNi( matWorld, pGeom->GetWorldTransform() );
			matResult = matWorld*matViewProjAdj;

			NiPoint3 dirLight = CAMERA_ST.GetWorldDirection();
			dirLight = dirLight*pGeom->GetWorldRotate();
			NiPoint3 pos;
			for( int i=0; i<nVertCount; ++i )
			{
				//if( dirLight.Dot( pNormal[ i ] ) <= 0.0f )
				{
					D3DXVec3TransformCoord( (D3DXVECTOR3*)&pos, (D3DXVECTOR3*)&pPos[ i ], &matResult );
					pUV[ i ].x = pos.x;
					pUV[ i ].y = pos.y;
				}				
				/*else
				{
					pUV[ i ].x = pUV[ i ].y = 10.0f;
				}*/
			}
			pGeom->GetModelData()->SetConsistency( NiGeometryData::MUTABLE );
			pGeom->GetModelData()->MarkAsChanged( NiGeometryData::TEXTURE_MASK );
		}
	}
}

void CsGBShadow::_Tool_InitApplyTRShadowMap_PointLight()
{

}

void CsGBShadow::_Tool_ApplyTRShadowMap_PointLight( int nLeafIndex )
{
#define OBJECT_PICK( lefidx )		{\
	pMapObject = nsCsGBTerrain::g_pCurRoot->GetNode( lefidx )->GetObjectMap();\
	itObject = pMapObject->begin();\
	for( ; itObject != pMapObject->end(); ++itObject )\
	{\
		if( itObject->second->IsCallPicked() == true )\
			continue;\
		if( itObject->second->UD_IsNoShadow() == true )\
			continue;\
		if( pick.Pick( itObject->second->GetNiNode(), ptLightPos, ptLightDir, false ) == true )\
		{\
			if( pick.GetRecord()->GetDistance() < fLightDist )\
			{\
				bObjectShadow = true;\
				break;\
			}\
		}\
	}\
}

	// 포인터
	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	CsGBTerrainLeaf* pLeafNode = nsCsGBTerrain::g_pCurRoot->GetNode( nLeafIndex );
	std::list< CsGBLight* >* pLightList = nsCsGBTerrain::g_pCurRoot->GetLightMng()->GetLightList();

	//========== 쉐이더 텍스쳐 생성
	pLeafNode->CreateShadowMap_ByTool();

	// 픽셀 리미트
	int nXStart = pRootInfo->GetTextureLeftFigure( pRootInfo->s_nShadowMPPG );
	int nXEnd = pRootInfo->GetTextureRightFigure( pRootInfo->s_nShadowMPPG ) + 1;
	int nYStart = pRootInfo->GetTextureTopFigure( pRootInfo->s_nShadowMPPG );
	int nYEnd = pRootInfo->GetTextureBottomFigure( pRootInfo->s_nShadowMPPG ) + 1;

	// Texture 관련
	NiPixelData* pPData = GET_SHADERMAP_PIXELDATA( pLeafNode->GetNiNode(), SHADOW_MAP_SHADER_NTM );
	BYTE* pPixel = (BYTE*)pPData->GetPixels();
	int nTextureWidth = pPData->GetWidth();
	int nTextureHeight = pPData->GetHeight();

	// 픽킹
	CsGBPick pick;

	// 위치	
	NiPoint3 ptLightPos = ( (CsGBLight_Point*)m_pShadowLight )->GetInfo()->s_Pos;
	NiPoint3 ptWorld;
	NiPoint3 ptNormal;
	NiPoint3 ptLightDir;
	float fLightDist;
	float fLightRange = ( (CsGBLight_Point*)m_pShadowLight )->GetInfo()->s_Range;
	float fRate;
	int nPixelIndex;

	MAP_GBTERRAIN_OBJECT* pMapObject = NULL;
	MAP_GBTERRAIN_OBJECT_IT itObject;

	bool bObjectShadow;

	int iX = nLeafIndex % pRootInfo->s_nNodeCountInSide;
	int iY = nLeafIndex / pRootInfo->s_nNodeCountInSide;	

	// 색
	NiColor ResultColor;

	// 픽셀들 돌아 가면서 검사
	for( int y=nYStart; y<nYEnd; ++y )
	{
		for( int x=nXStart; x<nXEnd; ++x )
		{
			nPixelIndex = ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE;

			// 값 초기화
			ResultColor = NiColor::BLACK;
			bObjectShadow = false;

			pRootInfo->TexturePixelToWorldPos( &ptWorld.x, &ptWorld.y, x, y, nLeafIndex, pRootInfo->s_nShadowMPPG );

			ptWorld.z = nsCsGBTerrain::g_pCurRoot->GetHeight_Normal( ptWorld.x, ptWorld.y, ptNormal, nsCsGBTerrain::g_pCurRoot->GetOrgHeightField() );

			// 라이트와의 거리
			ptLightDir = ptWorld - ptLightPos;
			fLightDist = ptLightDir.Length();

			nsCsGBTerrain::g_pCurRoot->ResetObject_Picking();

			//=======================================================================
			// Light			
			OBJECT_PICK( nLeafIndex );

			if( iX > 0 )
			{
				OBJECT_PICK( nLeafIndex - 1 );
				if( iY > 0 )
				{
					OBJECT_PICK( nLeafIndex - pRootInfo->s_nNodeCountInSide - 1 );
				}
				if( iY < pRootInfo->s_nNodeCountInSide - 1 )
				{
					OBJECT_PICK( nLeafIndex + pRootInfo->s_nNodeCountInSide - 1 );
				}
			}
			if( iX < pRootInfo->s_nNodeCountInSide - 1 )
			{
				OBJECT_PICK( nLeafIndex + 1 );
				if( iY > 0 )
				{
					OBJECT_PICK( nLeafIndex - pRootInfo->s_nNodeCountInSide + 1 );
				}
				if( iY < pRootInfo->s_nNodeCountInSide - 1 )
				{
					OBJECT_PICK( nLeafIndex + pRootInfo->s_nNodeCountInSide + 1 );
				}
			}
			if( iY > 0 )
			{
				OBJECT_PICK( nLeafIndex - pRootInfo->s_nNodeCountInSide );
			}
			if( iY < pRootInfo->s_nNodeCountInSide - 1 )
			{
				OBJECT_PICK( nLeafIndex + pRootInfo->s_nNodeCountInSide );
			}

			_Tool_CalLightTRColor( pLightList, ResultColor, fRate, ptWorld, ptNormal );

			if( bObjectShadow == true )
			{
				ResultColor *= pRootInfo->s_ShadowColor;				
			}

			if( fRate < 1.0f )
			{
				pPixel[ nPixelIndex + 0 ] = CsF2B( ResultColor.r*fRate*0.5f );
				pPixel[ nPixelIndex + 1 ] = CsF2B( ResultColor.g*fRate*0.5f );
				pPixel[ nPixelIndex + 2 ] = CsF2B( ResultColor.b*fRate*0.5f );
			}
			else
			{
				pPixel[ nPixelIndex + 0 ] = CsF2B( ResultColor.r*0.5f );
				pPixel[ nPixelIndex + 1 ] = CsF2B( ResultColor.g*0.5f );
				pPixel[ nPixelIndex + 2 ] = CsF2B( ResultColor.b*0.5f );
			}
		}
	}
	pPData->MarkAsChanged();	
}

void CsGBShadow::_Tool_EndApplyTRShadowMap_PointLight()
{
}

void CsGBShadow::_Tool_CreateDMObjectShadowMap_PointLight( CsGBObject* pBaseObj, int nLeafIndex )
{
	int nMapSize = ( (sUDD_DARKMAP*)pBaseObj->GetUserDefine()->GetData( sUDD_DARKMAP::Type() ) )->s_dwTerrainBaseShadowMapSize;

	// 랜더링
	NiBound Bound = pBaseObj->GetNiNode()->GetWorldBound();
	float fBoundRadius = Bound.GetRadius();

	// 백버퍼 사이즈 조절
	nsCsGBTerrain::g_pRenderer->Recreate( nMapSize, nMapSize );	

	// 카메라 설정
	NiPoint3 ptLightPos = ( (CsGBLight_Point*)m_pShadowLight )->GetInfo()->s_Pos;
	float fLightRange = ( (CsGBLight_Point*)m_pShadowLight )->GetInfo()->s_Range;
	NiPoint3 ptDestPos = Bound.GetCenter();
	NiPoint3 ptLightDir = ptDestPos - ptLightPos;
	float fLightDist = ptLightDir.Unitize();
	CAMERA_ST.SetTranslate( ptLightPos );
	CAMERA_ST.SetRotation( ptLightDir );
	CAMERA_ST.SetDist( 1.0f );
	CAMERA_ST.SetOrthoFrustum( fBoundRadius, fBoundRadius );
	CAMERA_ST.Update_ByTool( 0.0f );

	// 텍스쳐에 베이스 오브젝트의 위치값을 저장 한다.
	NiSourceTexture* pTexture;
	NiPixelData* pSrcPixelData;
	NiTexturingPropertyPtr pTexPropX;
	NiTexturingPropertyPtr pTexPropY;
	NiTexturingPropertyPtr pTexPropZ;
	NiTexturingPropertyPtr pTexPropSign;
	BYTE* pSrcPixelX;
	BYTE* pSrcPixelY;
	BYTE* pSrcPixelZ;
	BYTE* pSrcPixelSign;

	pTexPropX = _Tool_RenderBackBufferDMObject_PointLight( pBaseObj, NiSingleShaderMaterial::Create( "TR_CONSTANT_COLOR_PL_X_TOOLUSE" ) );
	pTexture = (NiSourceTexture*)pTexPropX->GetBaseMap()->GetTexture();	
	pTexture->LoadPixelDataFromFile();
	pSrcPixelData = pTexture->GetSourcePixelData();	
	pSrcPixelX = (BYTE*)pSrcPixelData->GetPixels();	
	// 텍스쳐 삭제
	DeleteFile( _T( "temp.dds" ) );

	pTexPropY = _Tool_RenderBackBufferDMObject_PointLight( pBaseObj, NiSingleShaderMaterial::Create( "TR_CONSTANT_COLOR_PL_Y_TOOLUSE" ) );
	pTexture = (NiSourceTexture*)pTexPropY->GetBaseMap()->GetTexture();	
	pTexture->LoadPixelDataFromFile();
	pSrcPixelData = pTexture->GetSourcePixelData();	
	pSrcPixelY = (BYTE*)pSrcPixelData->GetPixels();	
	// 텍스쳐 삭제
	DeleteFile( _T( "temp.dds" ) );

	pTexPropZ = _Tool_RenderBackBufferDMObject_PointLight( pBaseObj, NiSingleShaderMaterial::Create( "TR_CONSTANT_COLOR_PL_Z_TOOLUSE" ) );
	pTexture = (NiSourceTexture*)pTexPropZ->GetBaseMap()->GetTexture();	
	pTexture->LoadPixelDataFromFile();
	pSrcPixelData = pTexture->GetSourcePixelData();	
	pSrcPixelZ = (BYTE*)pSrcPixelData->GetPixels();	
	// 텍스쳐 삭제
	DeleteFile( _T( "temp.dds" ) );

	pTexPropSign = _Tool_RenderBackBufferDMObject_PointLight( pBaseObj, NiSingleShaderMaterial::Create( "TR_CONSTANT_COLOR_PL_SIGN_TOOLUSE" ) );
	pTexture = (NiSourceTexture*)pTexPropSign->GetBaseMap()->GetTexture();	
	pTexture->LoadPixelDataFromFile();
	pSrcPixelData = pTexture->GetSourcePixelData();	
	pSrcPixelSign = (BYTE*)pSrcPixelData->GetPixels();	
	// 텍스쳐 삭제
	DeleteFile( _T( "temp.dds" ) );

	// DM 오브젝트에 입힐 텍스쳐 생성
	NiPixelData* pDestPixelData = NiNew NiPixelData( nMapSize, nMapSize, NiPixelFormat::RGB24 );
	int nDestStride = pDestPixelData->GetPixelStride();
	BYTE* pDestPixel = pDestPixelData->GetPixels();
	NiPoint3 ptPos;	
	
	BYTE btR = CsF2B( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_ShadowColor.r );
	BYTE btG = CsF2B( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_ShadowColor.g );
	BYTE btB = CsF2B( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_ShadowColor.b );
	MAP_GBTERRAIN_OBJECT* pMapObject = NULL;
	MAP_GBTERRAIN_OBJECT_IT itObject;
	CsGBPick pick;
	int nPixelIndex;
	int nNodeCount_InSide = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nNodeCountInSide;
	int iX = nLeafIndex % nNodeCount_InSide;
	int iY = nLeafIndex / nNodeCount_InSide;

	for( int y=0; y<nMapSize; ++y )
	{
		for( int x=0; x<nMapSize; ++x )
		{
			// 위치 저장한 텍스쳐는 픽셀당 4바이트를 가지고 있다
			nPixelIndex = ( y*nMapSize + x )*4;
			
			bool bHavePos = false;
			// 가장자리 부근의 계산이 정확하지 않기 때문에 근처에 색이 있다면 그색으로 위치 계산
			for( int i=0; i<9; ++i )
			{
				switch( i )
				{
				case 1:		if( x != 0 )								nPixelIndex -= 1;						break;
				case 2:		if( x < nMapSize - 1 )						nPixelIndex += 1;						break;
				case 3:		if( y != 0 )								nPixelIndex -= nMapSize;				break;
				case 4:		if( y < nMapSize - 1 )						nPixelIndex += nMapSize;				break;
				case 5:		if( ( x != 0 )&&( y != 0 ) )				nPixelIndex += ( -nMapSize -1 );		break;
				case 6:		if( ( x < nMapSize - 1 )&&( y != 0 ) )		nPixelIndex += ( -nMapSize + 1 );		break;
				case 7:		if( ( x != 0 )&&( y < nMapSize - 1 ) )		nPixelIndex += ( nMapSize -1 );		break;
				case 8:		if( ( x<nMapSize-1 )&&( y<nMapSize-1 ) )	nPixelIndex += ( nMapSize + 1 );		break;
				}
				
				// 색을 가지고 있다면 위치 계산해서 제어문 빠져 나가자
				if( pSrcPixelX[ nPixelIndex + 0 ] != 0xff )
				{
					ptPos.x =	(float)(	(int)( ( CsB2F( pSrcPixelX[ nPixelIndex + 0 ] ) + 0.005f )*100.0f )*10000	+
											(int)( ( CsB2F( pSrcPixelX[ nPixelIndex + 1 ] ) + 0.005f )*100.0f )*100	+
											(int)( ( CsB2F( pSrcPixelX[ nPixelIndex + 2 ] ) + 0.005f )*100.0f )		);
					ptPos.y =	(float)(	(int)( ( CsB2F( pSrcPixelY[ nPixelIndex + 0 ] ) + 0.005f )*100.0f )*10000	+
											(int)( ( CsB2F( pSrcPixelY[ nPixelIndex + 1 ] ) + 0.005f )*100.0f )*100	+
											(int)( ( CsB2F( pSrcPixelY[ nPixelIndex + 2 ] ) + 0.005f )*100.0f )		);
					ptPos.z =	(float)(	(int)( ( CsB2F( pSrcPixelZ[ nPixelIndex + 0 ] ) + 0.005f )*100.0f )*10000	+
											(int)( ( CsB2F( pSrcPixelZ[ nPixelIndex + 1 ] ) + 0.005f )*100.0f )*100	+
											(int)( ( CsB2F( pSrcPixelZ[ nPixelIndex + 2 ] ) + 0.005f )*100.0f )		);

					if( pSrcPixelSign[ nPixelIndex + 0 ] < 100 )
						ptPos.x = -ptPos.x;
					if( pSrcPixelSign[ nPixelIndex + 1 ] < 100 )
						ptPos.y = -ptPos.y;
					if( pSrcPixelSign[ nPixelIndex + 2 ] < 100 )
						ptPos.z = -ptPos.z;

					bHavePos = true;
					break;
				}				
			}

			#define OBJECT_PICK_PL( lefidx )		{\
				if( bShadow == false )\
				{\
					pMapObject = nsCsGBTerrain::g_pCurRoot->GetNode( lefidx )->GetObjectMap();\
					itObject = pMapObject->begin();\
					for( ; itObject != pMapObject->end(); ++itObject )\
					{\
						if( itObject->second->IsCallPicked() == true )\
							continue;\
						if( itObject->second->UD_IsNoShadow() == true )\
							continue;\
						if( itObject->second == pBaseObj )\
							continue;\
						if( pick.Pick( itObject->second->GetNiNode(), ptLightPos, ptLightDir, false ) == true )\
						{\
							if( pick.GetRecord()->GetDistance() < fLightDist )\
							{\
								bShadow = true;\
								break;\
							}\
						}\
					}\
				}\
			}

			bool bShadow = false;
			// 계산된 위치가 있다면
			if( bHavePos == true )
			{
				nsCsGBTerrain::g_pCurRoot->ResetObject_Picking();

				// 포인트 라이트 이기 문에 방향성 계산 위치에 대해서 다시 계산
				ptLightDir = ptPos - ptLightPos;
				fLightDist = ptLightDir.Unitize();

				// 픽킹
				OBJECT_PICK_PL( nLeafIndex );

				if( iX > 0 )
				{
					OBJECT_PICK_PL( nLeafIndex - 1 );
					if( iY > 0 )
					{
						OBJECT_PICK_PL( nLeafIndex - nNodeCount_InSide - 1 );
					}
					if( iY < nNodeCount_InSide - 1 )
					{
						OBJECT_PICK_PL( nLeafIndex + nNodeCount_InSide - 1 );
					}
				}
				if( iX < nNodeCount_InSide - 1 )
				{
					OBJECT_PICK_PL( nLeafIndex + 1 );
					if( iY > 0 )
					{
						OBJECT_PICK_PL( nLeafIndex - nNodeCount_InSide + 1 );
					}
					if( iY < nNodeCount_InSide - 1 )
					{
						OBJECT_PICK_PL( nLeafIndex + nNodeCount_InSide + 1 );
					}
				}
				if( iY > 0 )
				{
					OBJECT_PICK_PL( nLeafIndex - nNodeCount_InSide );
				}
				if( iY < nNodeCount_InSide - 1 )
				{
					OBJECT_PICK_PL( nLeafIndex + nNodeCount_InSide );
				}
			}

			// DM 텍스쳐에 계산값 넣어 준다.
			int nDestPixelIndex = ( y*nMapSize + x )*3;
			if( bShadow == false )
			{
				for( int s=0; s<nDestStride; ++s )
				{
					pDestPixel[ nDestPixelIndex + s ] = 0xff;
				}
			}
			else
			{
				pDestPixel[ nDestPixelIndex + 0 ] = btR;
				pDestPixel[ nDestPixelIndex + 1 ] = btG;
				pDestPixel[ nDestPixelIndex + 2 ] = btB;
			}
		}
	}

	// 메모리 제거
	pTexPropX = 0;
	pTexPropY = 0;
	pTexPropZ = 0;
	pTexPropSign = 0;

	// 행렬
	float fTexelOffs = 0.5f + ( 0.5f / (float)nMapSize );	
	D3DXMATRIX matTexAdj(	0.5f, 0.0f, 0.0f, 0.0f,
							0.0f, -0.5f, 0.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 0.0f,
							fTexelOffs,	 fTexelOffs, 1.0f, 1.0f);

	D3DXMATRIX matView = nsCsGBTerrain::g_pRenderer->GetD3DView();
	D3DXMATRIX matProj = nsCsGBTerrain::g_pRenderer->GetD3DProj();
	D3DXMATRIX matViewProj = matView*matProj;
	D3DXMATRIX matViewProjAdj = matViewProj*matTexAdj;

	// 텍스쳐
	NiSourceTexturePtr pSourceTex = NiSourceTexture::Create( pDestPixelData );

	// UV 셋팅후 생성한 텍스쳐를 오브젝트에 씌운다.
	_Tool_ApplyDrakMap_DMObject_PointLight( pBaseObj->GetNiNode(), pSourceTex, matViewProjAdj );	
}

NiTexturingPropertyPtr CsGBShadow::_Tool_RenderBackBufferDMObject_PointLight( CsGBObject* pBaseObj, NiSingleShaderMaterial* pShader )
{
	nsCsGBTerrain::g_pTRMng->BeginRender();

	pBaseObj->Update( 0.0f, false );
	pBaseObj->SetShader( pShader );

	nsCsGBTerrain::g_pRenderer->BeginOffScreenFrame();
	nsCsGBTerrain::g_pRenderer->BeginUsingDefaultRenderTargetGroup( NiRenderer::CLEAR_ALL );
	//========== 랜더링 설정
	NiRenderer::GetRenderer()->SetCameraData( CAMERA_ST.GetCameraObj() );
	//========== 랜더링
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_F1, false );
	g_Sorting.RenderTRObject_Type( CsGBSorting::GT_NOCLIPPING, false );
	g_Sorting.RenderTRObject( false );
	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	nsCsGBTerrain::g_pRenderer->EndUsingRenderTargetGroup();
	nsCsGBTerrain::g_pRenderer->EndOffScreenFrame();
	nsCsGBTerrain::g_pTRMng->EndRender();

	// 쉐이더 다시 원래 대로 복귀
	g_Sorting.SetShader_GetterTRObject( nsCsGBTerrain::g_pRenderer->GetDefaultMaterial() );
	g_Sorting.ClearGetter();

	// dds 로 저장
	LPDIRECT3DDEVICE9 pkDevice = nsCsGBTerrain::g_pRenderer->GetD3DDevice();
	LPDIRECT3DSURFACE9 pkDestSurface = NULL;
	HRESULT hr = pkDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pkDestSurface);
	assert_cs( SUCCEEDED(hr) );
	// Save the image to a file
	HRESULT hr2 = D3DXSaveSurfaceToFile( _T( "temp.dds" ), D3DXIFF_DDS, pkDestSurface, NULL, NULL);
	D3D_POINTER_RELEASE(pkDestSurface);

	// 저장한 텍스쳐 메모리 상으로 옴긴다.
	NiTexturingPropertyPtr pTexProp = NiNew NiTexturingProperty( "temp.dds" );
	return pTexProp;	
}

void CsGBShadow::_Tool_ApplyDrakMap_DMObject_PointLight( NiNode* pNiBase, NiSourceTexturePtr pSourceTexture, D3DXMATRIX matViewProjAdj )
{	
	assert_cs( NiIsKindOf( NiNode, pNiBase ) == true );

	int nChildCount = pNiBase->GetArrayCount();
	for( int i=0; i<nChildCount; ++i )
	{
		NiAVObject* pChild = pNiBase->GetAt( i );
		if( NiIsKindOf( NiNode, pChild ) == true )
		{
			_Tool_ApplyDrakMap_DMObject_PointLight( (NiNode*)pChild, pSourceTexture, matViewProjAdj );
		}
		else if( ( ( NiIsKindOf( NiTriStrips, pChild ) == true )||( NiIsKindOf( NiTriShape, pChild ) == true ) )&&
			( pChild->GetAppCulled() == false ) )
		{
			NiGeometry* pGeom = ( NiGeometry* )pChild;			
			NiTexturingProperty* pTexProp = (NiTexturingProperty*)pGeom->GetProperty( NiTexturingProperty::GetType() );			
			NiTexturingProperty::Map* pMap = NiNew NiTexturingProperty::Map( pSourceTexture, 1, NiTexturingProperty::CLAMP_S_CLAMP_T );

			pTexProp->SetDarkMap( pMap );


			// 텍스쳐 uv셋팅
			NiPoint2* pUV = pGeom->GetTextureSet( 1 );
			NiPoint3* pPos = pGeom->GetVertices();
			NiPoint3* pNormal = pGeom->GetNormals();
			int nVertCount = pGeom->GetVertexCount();

			D3DXMATRIX matWorld, matResult;			
			NiD3DUtility::GetD3DFromNi( matWorld, pGeom->GetWorldTransform() );
			matResult = matWorld*matViewProjAdj;

			/*D3DXMATRIX matWorld, matResult;			
			NiD3DUtility::GetD3DFromNi( matWorld, pGeom->GetWorldTransform() );
			matResult = matWorld*matViewProjAdj;*/

			NiPoint3 pos;
			for( int i=0; i<nVertCount; ++i )
			{
				D3DXVec3TransformCoord( (D3DXVECTOR3*)&pos, (D3DXVECTOR3*)&pPos[ i ], &matResult );
				pUV[ i ].x = pos.x;
				pUV[ i ].y = pos.y;

				/*NiPoint3 dirLight = pGeom->GetWorldTransform()*pPos[ i ] - ( (CsGBLight_Point*)m_pShadowLight )->GetInfo()->s_Pos;
				dirLight = dirLight*pGeom->GetWorldRotate();
				if( dirLight.Dot( pNormal[ i ] ) <= 0.0f )
				{
				D3DXVec3TransformCoord( (D3DXVECTOR3*)&pos, (D3DXVECTOR3*)&pPos[ i ], &matResult );
				pUV[ i ].x = pos.x;
				pUV[ i ].y = pos.y;
				}				
				else
				{
				pUV[ i ].x = pUV[ i ].y = 10.0f;
				}*/
			}
			pGeom->GetModelData()->SetConsistency( NiGeometryData::MUTABLE );
			pGeom->GetModelData()->MarkAsChanged( NiGeometryData::TEXTURE_MASK );
		}
	}
}