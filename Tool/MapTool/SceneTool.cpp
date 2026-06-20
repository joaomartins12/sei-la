

#include "stdafx.h"
#include "SceneTool.h"

cSceneTool::cSceneTool()
{
	m_bWireFrame = false;
	m_bEnablePickingForMapping = false;
}

cSceneTool::~cSceneTool()
{
}

//==========================================================================================
//
//		Delete
//
//==========================================================================================
void cSceneTool::Delete()
{
	g_RollBack.Delete();	
}


//==========================================================================================
//
//		Init
//
//==========================================================================================
void cSceneTool::Init()
{
	g_RollBack.Init();

	m_mapAlphaPixelData.clear();
	m_bWireFrame = false;
	m_bEnablePickingForMapping = false;
}

void cSceneTool::Reset()
{
	m_bWireFrame = false;
	m_bEnablePickingForMapping = false;

	g_RollBack.Delete();
	g_RollBack.Init();
}

//==========================================================================================
//
//		Update
//
//==========================================================================================

void cSceneTool::ToggleWireFrame()
{ 
	m_bWireFrame = !m_bWireFrame; 

	if( nsCsGBTerrain::g_pCurRoot )
		nsCsGBTerrain::g_pCurRoot->SetWireframe( m_bWireFrame );	
}

//==========================================================================================
//
//		Render
//
//==========================================================================================

void cSceneTool::Render()
{	
	g_pRenderer->SetCameraData( CAMERA_ST.GetCameraObj() );
	g_MapMng.BeginRender();	

	if( g_Actor.IsEnableActor() == true )
	{
		g_MapMng.Render( true );
	}
	else
	{
		g_MapMng.RenderObject_ByTool( g_fElapsedTime, true );
		g_MapMng.RenderTerrain_ByTool( GET_MV_MAINTAB_DLG->_GetDlgPlayer()->_IsShowSkyBos() );
	}

	g_Actor.Render();

	g_Sorting.Render( CAMERA_ST.GetCameraObj() );

	g_MapMng.EndRender();

// 	if( m_bWireFrame == true )
// 	{
// 		for( int i=0; i<nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount; ++i )
// 		{
// 			CsGBTerrainNode* pNode = nsCsGBTerrain::g_pCurRoot->GetNode( i );
// 			if( ( pNode->IsExistTerrain() == true )&&( pNode->GetNiNode()->GetParent() != NULL ) )
// 			{
// 				cRender::RenderWireFrame( pNode->GetNiNode() );
// 			}
// 		}
// 	}
}

//==========================================================================================
//
//		Mouse
//
//==========================================================================================
void cSceneTool::OnLButtonDown( CPoint pt )
{
	if( m_bEnableZoomIn == true )
	{
		GET_MV_NODE->_SelectChildNode( pt );
	}
	else if( _IsEnablePickingForMapping() == true )
	{
		m_bEnablePickingForMapping = true;
		_PickStart( pt );
	}
}

void cSceneTool::OnLButtonUp( CPoint pt )
{
	if( m_bEnablePickingForMapping == true )
	{
		_PickEnd( pt );
		m_bEnablePickingForMapping = false;
	}
}

void cSceneTool::OnMButtonDown( CPoint pt )
{
}

void cSceneTool::OnMButtonUp( CPoint pt )
{
}

void cSceneTool::OnRButtonDown( CPoint pt )
{
}

void cSceneTool::OnRButtonUp( CPoint pt )
{
}

void cSceneTool::OnMouseMove( CPoint pt )
{
	if( m_bEnablePickingForMapping == true )
	{
		_Picking( pt );
	}	
}

void cSceneTool::OnMouseWheel( short zDelta )
{
}


//==========================================================================================
//
//		Picking
//
//==========================================================================================

void cSceneTool::_PickStart( CPoint pt )
{
	g_RollBack.Begin();
	assert_cs( m_mapAlphaChangeObject.empty() == true );

	switch( m_ePickType )
	{
	case PT_ALPHA:		_PickingMapping( pt );		break;
	default:			assert_cs( false );
	}	
}

void cSceneTool::_Picking( CPoint pt )
{
	switch( m_ePickType )
	{
	case PT_ALPHA:		_PickingMapping( pt );			break;
	default:			assert_cs( false );
	}
}

void cSceneTool::_PickEnd( CPoint pt )
{
	g_RollBack.End();
	m_mapAlphaChangeObject.clear();
}

bool cSceneTool::_IsEnablePickingForMapping()
{
	int nSelTabIndex = GET_MV_MAINTAB_DLG->_GetSelTabIndex();
	switch( nSelTabIndex )
	{
	case _DlgMainTab::TT_MAPPING:
		m_ePickType = PT_ALPHA;
		return true;
	}

	return false;
}

//==========================================================================================
//
//		Picking Alpha
//
//==========================================================================================
void cSceneTool::_PickingMapping( CPoint pt )
{

	//====================================================================================================
	// ====== 선택 된 맵이 있는가
	int nShaderIndex = GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetCheckMapIndex();
	if( nShaderIndex == -1 )
	{
		return;
	}

	//====================================================================================================
	// ====== 픽킹
	NiPoint3 origin, dir;
	CAMERA_ST.WindowPointToRay( pt.x, pt.y, origin, dir );
	CsGBPick pick;
	if( pick.Pick( nsCsGBTerrain::g_pCurRoot->GetRootNode()->GetNiNode(), origin, dir, false ) == false )
		return;

	NiPick::Record* pRecord = pick.GetRecord();
	assert_cs( pRecord != NULL );

	sPICK_INFO	pi;

	// 이름 검사해서 오브젝트 판별
	NiFixedString fixStr = pRecord->GetAVObject()->GetName();
	pi.s_nObjectIndex = atoi( fixStr );

	//====================================================================================================
	// ====== Map Info
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	CsGBTerrainLeaf::sNiTriINFO* pLeafTriInfo = nsCsGBTerrain::g_pCurRoot->GetNode( pi.s_nObjectIndex )->GetNiTriInfo();	

	// 텍스쳐 프로퍼티
	pi.s_pTexProp = ( NiTexturingProperty* )pRecord->GetAVObject()->GetProperty( NiTexturingProperty::GetType() );	

	//====================================================================================================
	// ====== 감쇠 값
	float fDecrement = GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetAlphaDecrement()*0.05f -
						GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetAlphaSize()*0.05f;
	if( fDecrement == 0.0f )
		fDecrement = 0.1f;

	pi.s_fDecrementPow = GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetAlphaDecrementConstant();
	pi.s_fSizeConstant = 1.0f/pow( fDecrement, pi.s_fDecrementPow );

	if( nShaderIndex >= 5 )
		_PickingGray( pRecord, &pi );
	else
		_PickingAlpha( pRecord, &pi );
}

void cSceneTool::_PickingAlpha( NiPick::Record* pRecord, sPICK_INFO* pi )
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	pi->s_nNTM = ALPHA_MAP_SHADER_NTM;
	pi->s_nStride = ALPHA_MAP_STRIDE;
	pi->s_pPData = GET_TP_SHADERMAP_PIXELDATA( pi->s_pTexProp, pi->s_nNTM );
	pi->s_pPixel = (BYTE*)pi->s_pPData->GetPixels();
	pi->s_nWidth = pi->s_pPData->GetWidth();
	pi->s_nPPG = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nAlphaMPPG;

	//====================================================================================================
	// ====== 선택된 맵 픽셀 좌표를 찾는다
	// PickPoint
	NiPoint3 ptPick = pRecord->GetIntersection();	
	// 월드 좌표를 픽셀 좌표로
	int xPixel, yPixel;
	pInfo->WorldPosToTexturePixel( &xPixel, &yPixel, NiPoint2( ptPick.x, ptPick.y ), pi->s_nObjectIndex, pi->s_nPPG );
	pi->s_ptMap = CPoint( xPixel, yPixel );
	pi->s_ptMapOrg = pi->s_ptMap;

	int nSelShaderIndex = GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetCheckMapIndex() - 1;
	assert_cs( m_mapAlphaPixelData.empty() == true );
	pi->s_nChannel = nSelShaderIndex;
	pi->s_btColor = (BYTE)( 0xff*GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetOpercity() );
	pi->s_eMT = MT_ALPHA;

	// Self
	_SetAlphaValue( *pi );
	// 주변에 영향
	_PickingAlphaUp( *pi );
	_PickingAlphaDown( *pi );
	_PickingAlphaRight( *pi );
	_PickingAlphaLeft( *pi );

	// 영향 받은 텍스쳐들은 변경
	std::map< DWORD, NiPixelData* >::iterator it = m_mapAlphaPixelData.begin();
	for( ; it!=m_mapAlphaPixelData.end(); ++it )
	{
		it->second->MarkAsChanged();
	}
	m_mapAlphaPixelData.clear();
}

void cSceneTool::_PickingGray( NiPick::Record* pRecord, sPICK_INFO* pi )
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	pi->s_nNTM = EFFECT_MAP_SHADER_NTM;
	pi->s_nStride = EFFECT_MAP_STRIDE;	
	pi->s_pPData = GET_TP_SHADERMAP_PIXELDATA( pi->s_pTexProp, pi->s_nNTM );
	pi->s_pPixel = (BYTE*)pi->s_pPData->GetPixels();
	pi->s_nWidth = pi->s_pPData->GetWidth();
	pi->s_nPPG = pInfo->s_nEffectMPPG;

	//====================================================================================================
	// ====== 선택된 맵 픽셀 좌표를 찾는다
	// PickPoint
	NiPoint3 ptPick = pRecord->GetIntersection();	
	// 월드 좌표를 픽셀 좌표로
	int xPixel, yPixel;
	pInfo->WorldPosToTexturePixel( &xPixel, &yPixel, NiPoint2( ptPick.x, ptPick.y ), pi->s_nObjectIndex, pi->s_nPPG );
	pi->s_ptMap = CPoint( xPixel, yPixel );
	pi->s_ptMapOrg = pi->s_ptMap;

	int nSelShaderIndex = GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetCheckMapIndex();	
	assert_cs( m_mapAlphaPixelData.empty() == true );

	for( int i=0; i<3; ++i )
	{
		pi->s_nChannel = i;

		if( nSelShaderIndex == 5 )
		{
			pi->s_eMT = MT_GRAY_PLUS;
			switch( i )
			{
			case 0:		pi->s_btColor = CsF2B( g_Resist.GetMapping()->s_GrayColor.r );		break;
			case 1:		pi->s_btColor = CsF2B( g_Resist.GetMapping()->s_GrayColor.g );		break;
			case 2:		pi->s_btColor = CsF2B( g_Resist.GetMapping()->s_GrayColor.b );		break;
			default:	assert_cs( false );
			}
			pi->s_btColor = (BYTE)( CsMin( 0xff, pi->s_btColor/GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetOpercity() ) );
		}
		else
		{
			pi->s_eMT = MT_GRAY_MINUS;
			pi->s_btColor = (BYTE)( 0xff*GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetOpercity() );
		}

		_SetAlphaValue( *pi );

		// 주변에 영향
		_PickingAlphaUp( *pi );
		_PickingAlphaDown( *pi );
		_PickingAlphaRight( *pi );
		_PickingAlphaLeft( *pi );
	}	

	// 영향 받은 텍스쳐들은 변경
	std::map< DWORD, NiPixelData* >::iterator it = m_mapAlphaPixelData.begin();
	for( ; it!=m_mapAlphaPixelData.end(); ++it )
	{
		it->second->MarkAsChanged();
	}
	m_mapAlphaPixelData.clear();
}

void cSceneTool::_SetAlphaValue( sPICK_INFO pi )
{
	int nPixelIndex = ( pi.s_ptMap.y*pi.s_nWidth + pi.s_ptMap.x )*pi.s_nStride + pi.s_nChannel;
	int nPixelStart = ( pi.s_ptMap.y*pi.s_nWidth + pi.s_ptMap.x )*pi.s_nStride;

	// ============= 색 입력
	// 기본색 산출
	float fAlpha0 = CsB2F( pi.s_pPixel[ nPixelStart + 0 ] );
	float fAlpha1 = CsB2F( pi.s_pPixel[ nPixelStart + 1 ] );
	float fAlpha2 = CsB2F( pi.s_pPixel[ nPixelStart + 2 ] );
	float fAlpha3 = CsB2F( pi.s_pPixel[ nPixelStart + 3 ] );
	float fAlphaTotal = fAlpha0 + fAlpha1 + fAlpha2 + fAlpha3;
	if( fAlphaTotal > 1.0f )
	{
		fAlpha0 = fAlpha0/fAlphaTotal;
		fAlpha1 = fAlpha1/fAlphaTotal;
		fAlpha2 = fAlpha2/fAlphaTotal;
		fAlpha3 = fAlpha3/fAlphaTotal;
		fAlphaTotal = 1.0f;
	}	
	float fNewColor_R = 1.0f - CsB2F( pi.s_btColor );

	
	switch( pi.s_eMT )
	{
	case MT_ALPHA:
	case MT_GRAY_MINUS:
		if( pi.s_nChannel >= 0 )
		{
			if( pi.s_btColor < pi.s_pPixel[ nPixelIndex ] )		return;
		}
		else
		{
			float fBase = 1.0f - fAlphaTotal;
			if( pi.s_btColor < CsF2B( fBase ) )				return;
		}
		break;	
	case MT_GRAY_PLUS:	
		assert_cs( pi.s_nChannel >= 0 );
		if( pi.s_btColor > pi.s_pPixel[ nPixelIndex ] )		return;
		break;
	}

	// 롤백 추가
	if( m_mapAlphaChangeObject.find( pi.s_nObjectIndex )  == m_mapAlphaChangeObject.end() )
	{
		m_mapAlphaChangeObject[ pi.s_nObjectIndex ] = 1;

		switch( pi.s_eMT )
		{
		case MT_ALPHA:
			{
				cRollBack::sRT_AM_INFO info;
				info.s_nObjectID = pi.s_nObjectIndex;
				info.s_nNTM = pi.s_nNTM;
				info.s_nStride = pi.s_nStride;
				g_RollBack.Push( &info );
			}			
			break;
		case MT_GRAY_PLUS:
		case MT_GRAY_MINUS:
			{
				cRollBack::sRT_EM_INFO info;
				info.s_nObjectID = pi.s_nObjectIndex;
				info.s_nNTM = pi.s_nNTM;
				info.s_nStride = pi.s_nStride;
				g_RollBack.Push( &info );
			}			
			break;
		}
	}
	if( m_mapAlphaPixelData.find( pi.s_nObjectIndex ) == m_mapAlphaPixelData.end() )
	{
		m_mapAlphaPixelData[ pi.s_nObjectIndex ] = pi.s_pPData;
	}
	
	switch( pi.s_eMT )
	{
	case MT_ALPHA:
		{
			switch( pi.s_nChannel )
			{
			case -1:
				{
					float fBase = 1.0f - fAlphaTotal;
					float fInvRate = 1.0f/( 1.0f - fBase );
					pi.s_pPixel[ nPixelStart + 0 ] = CsF2B( fNewColor_R*fInvRate*fAlpha0 );
					pi.s_pPixel[ nPixelStart + 1 ] = CsF2B( fNewColor_R*fInvRate*fAlpha1 );
					pi.s_pPixel[ nPixelStart + 2 ] = CsF2B( fNewColor_R*fInvRate*fAlpha2 );
					pi.s_pPixel[ nPixelStart + 3 ] = CsF2B( fNewColor_R*fInvRate*fAlpha3 );
				}
				break;
			case 0:
				{
					float fInvRate = 1.0f/( 1.0f - fAlpha0 );
					pi.s_pPixel[ nPixelStart + 0 ] = pi.s_btColor;
					pi.s_pPixel[ nPixelStart + 1 ] = CsF2B( fNewColor_R*fInvRate*fAlpha1 );
					pi.s_pPixel[ nPixelStart + 2 ] = CsF2B( fNewColor_R*fInvRate*fAlpha2 );
					pi.s_pPixel[ nPixelStart + 3 ] = CsF2B( fNewColor_R*fInvRate*fAlpha3 );
				}
				break;
			case 1:
				{
					float fInvRate = 1.0f/( 1.0f - fAlpha1 );
					pi.s_pPixel[ nPixelStart + 0 ] = CsF2B( fNewColor_R*fInvRate*fAlpha0 );
					pi.s_pPixel[ nPixelStart + 1 ] = pi.s_btColor;
					pi.s_pPixel[ nPixelStart + 2 ] = CsF2B( fNewColor_R*fInvRate*fAlpha2 );
					pi.s_pPixel[ nPixelStart + 3 ] = CsF2B( fNewColor_R*fInvRate*fAlpha3 );
				}
				break;
			case 2:
				{
					float fInvRate = 1.0f/( 1.0f - fAlpha2 );
					pi.s_pPixel[ nPixelStart + 0 ] = CsF2B( fNewColor_R*fInvRate*fAlpha0 );
					pi.s_pPixel[ nPixelStart + 1 ] = CsF2B( fNewColor_R*fInvRate*fAlpha1 );
					pi.s_pPixel[ nPixelStart + 2 ] = pi.s_btColor;
					pi.s_pPixel[ nPixelStart + 3 ] = CsF2B( fNewColor_R*fInvRate*fAlpha3 );
				}
				break;
			case 3:
				{
					float fInvRate = 1.0f/( 1.0f - fAlpha3 );
					pi.s_pPixel[ nPixelStart + 0 ] = CsF2B( fNewColor_R*fInvRate*fAlpha0 );
					pi.s_pPixel[ nPixelStart + 1 ] = CsF2B( fNewColor_R*fInvRate*fAlpha1 );
					pi.s_pPixel[ nPixelStart + 2 ] = CsF2B( fNewColor_R*fInvRate*fAlpha2 );
					pi.s_pPixel[ nPixelStart + 3 ] = pi.s_btColor;
				}
				break;
			default:
				assert_cs( false );
			}
		}			
		break;
	case MT_GRAY_PLUS:
	case MT_GRAY_MINUS:
		{
			pi.s_pPixel[ nPixelStart + pi.s_nChannel ] = pi.s_btColor;
		}			
		break;
	}
	
	
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	// 위로
	// 노드 사이즈 * PPG + 마지막 인덱스
	if( pi.s_ptMap.y == pInfo->GetTextureTopFigure( pi.s_nPPG ) )
	{
		int nYObject = pi.s_nObjectIndex/pInfo->s_nNodeCountInSide;
		// 경계 오브젝트가 존재 하지않는다면
		if( ++nYObject != pInfo->s_nNodeCountInSide )
		{
			assert_cs( nYObject < pInfo->s_nNodeCountInSide );

			// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
			sPICK_INFO piNew;
			memcpy( &piNew, &pi, sizeof( sPICK_INFO ) );
			piNew.s_nObjectIndex += pInfo->s_nNodeCountInSide;

			// 데이터 변경
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( piNew.s_nObjectIndex );
			piNew.s_pTexProp = (NiTexturingProperty*)pLeaf->GetNiNode()->GetAt( 0 )->GetProperty( NiTexturingProperty::GetType() );
			piNew.s_pPData = GET_TP_SHADERMAP_PIXELDATA( piNew.s_pTexProp, piNew.s_nNTM );			
			piNew.s_pPixel = (BYTE*)piNew.s_pPData->GetPixels();

			piNew.s_ptMap.y = pInfo->GetTextureBottomFigure( piNew.s_nPPG )+1;
			_SetAlphaValue( piNew );
		}		
	}
	// 아래로
	if( pi.s_ptMap.y == pInfo->GetTextureBottomFigure( pi.s_nPPG ) )
	{
		int nYObject = pi.s_nObjectIndex/pInfo->s_nNodeCountInSide;
		// 경계 오브젝트가 존재 한다면
		if( nYObject > 0 )
		{
			// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
			sPICK_INFO piNew;
			memcpy( &piNew, &pi, sizeof( sPICK_INFO ) );
			piNew.s_nObjectIndex -= pInfo->s_nNodeCountInSide;
			// 데이터 변경
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( piNew.s_nObjectIndex );
			piNew.s_pTexProp = (NiTexturingProperty*)pLeaf->GetNiNode()->GetAt( 0 )->GetProperty( NiTexturingProperty::GetType() );
			piNew.s_pPData = GET_TP_SHADERMAP_PIXELDATA( piNew.s_pTexProp, piNew.s_nNTM );			
			piNew.s_pPixel = (BYTE*)piNew.s_pPData->GetPixels();

			piNew.s_ptMap.y = pInfo->GetTextureTopFigure( piNew.s_nPPG ) - 1;
			_SetAlphaValue( piNew );
		}
	}
	// 오른쪽으로
	if( pi.s_ptMap.x == pInfo->GetTextureRightFigure( pi.s_nPPG ) )
	{
		int nXObject = pi.s_nObjectIndex%pInfo->s_nNodeCountInSide;
		// 경계 오브젝트가 존재 한다면
		if( ++nXObject != pInfo->s_nNodeCountInSide )
		{
			// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
			sPICK_INFO piNew;
			memcpy( &piNew, &pi, sizeof( sPICK_INFO ) );
			piNew.s_nObjectIndex += 1;

			// 데이터 변경
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( piNew.s_nObjectIndex );
			piNew.s_pTexProp = (NiTexturingProperty*)pLeaf->GetNiNode()->GetAt( 0 )->GetProperty( NiTexturingProperty::GetType() );
			piNew.s_pPData = GET_TP_SHADERMAP_PIXELDATA( piNew.s_pTexProp, piNew.s_nNTM );			
			piNew.s_pPixel = (BYTE*)piNew.s_pPData->GetPixels();

			piNew.s_ptMap.x = pInfo->GetTextureLeftFigure( piNew.s_nPPG ) - 1;
			_SetAlphaValue( piNew );
		}	
	}
	// 왼쪽으로
	if( pi.s_ptMap.x == pInfo->GetTextureLeftFigure( pi.s_nPPG ) ) 
	{
		int nXObject = pi.s_nObjectIndex%pInfo->s_nNodeCountInSide;
		// 경계 오브젝트가 존재 하지않는다면
		if( nXObject > 0 )
		{
			// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
			sPICK_INFO piNew;
			memcpy( &piNew, &pi, sizeof( sPICK_INFO ) );
			piNew.s_nObjectIndex -= 1;

			// 데이터 변경
			CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( piNew.s_nObjectIndex );
			piNew.s_pTexProp = (NiTexturingProperty*)pLeaf->GetNiNode()->GetAt( 0 )->GetProperty( NiTexturingProperty::GetType() );
			piNew.s_pPData = GET_TP_SHADERMAP_PIXELDATA( piNew.s_pTexProp, piNew.s_nNTM );			
			piNew.s_pPixel = (BYTE*)piNew.s_pPData->GetPixels();

			piNew.s_ptMap.x = pInfo->GetTextureRightFigure( piNew.s_nPPG ) + 1;
			_SetAlphaValue( piNew );
		}	
	}
}

bool cSceneTool::_PickingAlphaDirection( sPICK_INFO* pi )
{
	float fDist = sqrt( (float)pow( pi->s_ptMap.x - pi->s_ptMapOrg.x, 2 ) + (float)pow( pi->s_ptMap.y - pi->s_ptMapOrg.y, 2 ) );
	fDist -= GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetAlphaSize()*0.05f;

	if( fDist < 0.0f )
	{
		switch( pi->s_eMT )
		{
		case MT_ALPHA:
		case MT_GRAY_MINUS:
			pi->s_btColor = (BYTE)( 0xff*GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetOpercity() );
			break;
		case MT_GRAY_PLUS:
			switch( pi->s_nChannel )
			{
			case 0:		pi->s_btColor = CsF2B( g_Resist.GetMapping()->s_GrayColor.r );		break;
			case 1:		pi->s_btColor = CsF2B( g_Resist.GetMapping()->s_GrayColor.g );		break;
			case 2:		pi->s_btColor = CsF2B( g_Resist.GetMapping()->s_GrayColor.b );		break;
			default:	assert_cs( false );
			}
			pi->s_btColor = (BYTE)( CsMin( 0xff, pi->s_btColor/GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetOpercity() ) );
			break;
		}		
		return true;
	}

	float fDistConstant = pow( fDist, pi->s_fDecrementPow );	
	float fColorConstant = 1.0f - ( fDistConstant * pi->s_fSizeConstant );
	if( fColorConstant <= 0.0 )
	{
		return false;
	}

	switch( pi->s_eMT )
	{	
	case MT_ALPHA:	
		pi->s_btColor = (BYTE)( 0xff*fColorConstant );
		pi->s_btColor = (BYTE)( pi->s_btColor*GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetOpercity() );
		break;
	case MT_GRAY_PLUS:
		{
			BYTE bt;
			switch( pi->s_nChannel )
			{
			case 0:		bt = CsF2B( g_Resist.GetMapping()->s_GrayColor.r );		break;
			case 1:		bt = CsF2B( g_Resist.GetMapping()->s_GrayColor.g );		break;
			case 2:		bt = CsF2B( g_Resist.GetMapping()->s_GrayColor.b );		break;
			default:	assert_cs( false );
			}			 
			pi->s_btColor = CsMin( 255, bt + (BYTE)( (255-bt)*( 1.0f - fColorConstant ) ) );
			pi->s_btColor = (BYTE)( CsMin( 0xff, pi->s_btColor/GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetOpercity() ) );
		}
		break;
	case MT_GRAY_MINUS:
		{
			BYTE bt;
			switch( pi->s_nChannel )
			{
			case 0:		bt = CsF2B( g_Resist.GetMapping()->s_GrayColor.r );		break;
			case 1:		bt = CsF2B( g_Resist.GetMapping()->s_GrayColor.g );		break;
			case 2:		bt = CsF2B( g_Resist.GetMapping()->s_GrayColor.b );		break;
			default:	assert_cs( false );
			}
			pi->s_btColor = 255 - (BYTE)( ( 255-bt )*( 1.0f - fColorConstant ) );
			pi->s_btColor = (BYTE)( pi->s_btColor*GET_MV_MAINTAB_DLG->_GetDlgMapping()->_GetOpercity() );
		}
		break;
	}

	return true;
}

void cSceneTool::_PickingAlphaUp( sPICK_INFO pi )
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	// 위로 한칸 증가 ( -가 증가이다 )
	--pi.s_ptMap.y;

	if( _PickingAlphaDirection( &pi ) == false )
		return;

	// 인덱스가 넘어 갔다
	if( pi.s_ptMap.y < pInfo->GetTextureTopFigure( pi.s_nPPG ) )
	{
		int nYObject = pi.s_nObjectIndex/pInfo->s_nNodeCountInSide;
		// 경계에 오브젝트가 존재 하지않는다면
		if( ++nYObject == pInfo->s_nNodeCountInSide )
		{
			return;
		}
		assert_cs( nYObject < pInfo->s_nNodeCountInSide );

		// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
		sPICK_INFO piNew;
		memcpy( &piNew, &pi, sizeof( sPICK_INFO ) );
		piNew.s_nObjectIndex += pInfo->s_nNodeCountInSide;
		int nOldMap = piNew.s_ptMap.y;
		piNew.s_ptMap.y = pInfo->GetTextureBottomFigure( piNew.s_nPPG ) + 1;
		// 변화량 합
		piNew.s_ptMapOrg.y += ( piNew.s_ptMap.y - nOldMap - 1 );

		// 데이터 변경
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( piNew.s_nObjectIndex );
		piNew.s_pTexProp = (NiTexturingProperty*)pLeaf->GetNiNode()->GetAt( 0 )->GetProperty( NiTexturingProperty::GetType() );
		piNew.s_pPData = GET_TP_SHADERMAP_PIXELDATA( piNew.s_pTexProp, piNew.s_nNTM );			
		piNew.s_pPixel = (BYTE*)piNew.s_pPData->GetPixels();
		_PickingAlphaUp( piNew );
		return;
	}
	assert_cs( pi.s_ptMap.y >= 0 );

	_SetAlphaValue( pi );	
	_PickingAlphaUp( pi );
	_PickingAlphaRight( pi );
	_PickingAlphaLeft( pi );
}

void cSceneTool::_PickingAlphaDown( sPICK_INFO pi )
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	// 아래로 한칸 증가
	++pi.s_ptMap.y;

	if( _PickingAlphaDirection( &pi ) == false )
		return;

	// 인덱스가 넘어 갔다	
	if( pi.s_ptMap.y > pInfo->GetTextureBottomFigure( pi.s_nPPG ) )
	{
		int nYObject = pi.s_nObjectIndex/pInfo->s_nNodeCountInSide;
		// 경계에 오브젝트가 존재 하지않는다면
		if( nYObject <= 0  )
		{
			return;
		}
		// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
		sPICK_INFO piNew;
		memcpy( &piNew, &pi, sizeof( sPICK_INFO ) );
		piNew.s_nObjectIndex -= pInfo->s_nNodeCountInSide;
		int nOldMap = piNew.s_ptMap.y;
		piNew.s_ptMap.y = pInfo->GetTextureTopFigure( piNew.s_nPPG ) - 1;
		// 변화량 합
		piNew.s_ptMapOrg.y += ( piNew.s_ptMap.y - nOldMap + 1 );

		// 데이터 변경
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( piNew.s_nObjectIndex );
		piNew.s_pTexProp = (NiTexturingProperty*)pLeaf->GetNiNode()->GetAt( 0 )->GetProperty( NiTexturingProperty::GetType() );
		piNew.s_pPData = GET_TP_SHADERMAP_PIXELDATA( piNew.s_pTexProp, piNew.s_nNTM );			
		piNew.s_pPixel = (BYTE*)piNew.s_pPData->GetPixels();
		_PickingAlphaDown( piNew );
		return;
	}
	assert_cs( pi.s_ptMap.y >= 0 );

	_SetAlphaValue( pi );
			
	_PickingAlphaDown( pi );
	_PickingAlphaRight( pi );
	_PickingAlphaLeft( pi );
}

void cSceneTool::_PickingAlphaRight( sPICK_INFO pi )
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	// 오른쪽
	++pi.s_ptMap.x;

	if( _PickingAlphaDirection( &pi ) == false )
		return;

	// 인덱스가 넘어 갔다
	if( pi.s_ptMap.x > pInfo->GetTextureRightFigure( pi.s_nPPG ) )
	{
		int nXObject = pi.s_nObjectIndex%pInfo->s_nNodeCountInSide;
		// 경계에 오브젝트가 존재 하지않는다면
		if( ++nXObject == pInfo->s_nNodeCountInSide )
		{
			return;
		}
		assert_cs( nXObject < pInfo->s_nNodeCountInSide );

		// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
		sPICK_INFO piNew;
		memcpy( &piNew, &pi, sizeof( sPICK_INFO ) );
		++piNew.s_nObjectIndex;		
		int nOldMap = piNew.s_ptMap.x;
		piNew.s_ptMap.x = pInfo->GetTextureLeftFigure( piNew.s_nPPG ) - 1;
		// 변화량 합
		piNew.s_ptMapOrg.x += ( piNew.s_ptMap.x + 1 - nOldMap );		

		// 데이터 변경
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( piNew.s_nObjectIndex );
		piNew.s_pTexProp = (NiTexturingProperty*)pLeaf->GetNiNode()->GetAt( 0 )->GetProperty( NiTexturingProperty::GetType() );
		piNew.s_pPData = GET_TP_SHADERMAP_PIXELDATA( piNew.s_pTexProp, piNew.s_nNTM );			
		piNew.s_pPixel = (BYTE*)piNew.s_pPData->GetPixels();
		_PickingAlphaRight( piNew );
		return;
	}
	assert_cs( pi.s_ptMap.x >= 0 );

	_SetAlphaValue( pi );
	
	_PickingAlphaRight( pi );	
}

void cSceneTool::_PickingAlphaLeft( sPICK_INFO pi )
{
	CsGBTerrainRoot::sINFO* pInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	// 오른쪽
	--pi.s_ptMap.x;

	if( _PickingAlphaDirection( &pi ) == false )
		return;

	// 인덱스가 넘어 갔다
	if( pi.s_ptMap.x < pInfo->GetTextureLeftFigure( pi.s_nPPG ) )
	{
		int nXObject = pi.s_nObjectIndex%pInfo->s_nNodeCountInSide;
		// 경계에 오브젝트가 존재 하지않는다면
		if( nXObject <= 0 )
		{
			return;
		}
		// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
		sPICK_INFO piNew;
		memcpy( &piNew, &pi, sizeof( sPICK_INFO ) );
		--piNew.s_nObjectIndex;		
		int nOldMap = piNew.s_ptMap.x;
		piNew.s_ptMap.x = pInfo->GetTextureRightFigure( piNew.s_nPPG ) + 1;
		// 변화량 합
		piNew.s_ptMapOrg.x += ( piNew.s_ptMap.x - 1 - nOldMap );		

		// 데이터 변경
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( piNew.s_nObjectIndex );
		piNew.s_pTexProp = (NiTexturingProperty*)pLeaf->GetNiNode()->GetAt( 0 )->GetProperty( NiTexturingProperty::GetType() );
		piNew.s_pPData = GET_TP_SHADERMAP_PIXELDATA( piNew.s_pTexProp, piNew.s_nNTM );			
		piNew.s_pPixel = (BYTE*)piNew.s_pPData->GetPixels();
		_PickingAlphaLeft( piNew );
		return;
	}
	assert_cs( pi.s_ptMap.x >= 0 );

	_SetAlphaValue( pi );
	
	_PickingAlphaLeft( pi );
}

