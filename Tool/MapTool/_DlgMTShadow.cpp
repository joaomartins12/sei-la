// _DlgMTShadow.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTShadow.h"

// _DlgMTShadow 대화 상자입니다.
IMPLEMENT_DYNAMIC(_DlgMTShadow, CDialog)

_DlgMTShadow::_DlgMTShadow(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTShadow::IDD, pParent)
{
	m_ppbtSMBlurOrg = NULL;
}

_DlgMTShadow::~_DlgMTShadow()
{
}



void _DlgMTShadow::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_SHADOWCOLOR, m_static_ShadowColor);
	DDX_Control(pDX, IDC_COMBO_SHADOW_MPPG, m_combo_ShadowMPPG);
	DDX_Control(pDX, IDC_COMBO_SMBLUR_SIZE, m_combo_SMBlur_Size);
	DDX_Control(pDX, IDC_CHECK_INCLUDE_TERRAIN, m_chk_IncludeTR);
}


BEGIN_MESSAGE_MAP(_DlgMTShadow, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &_DlgMTShadow::OnBnClickedButtonApply)
	ON_WM_CTLCOLOR()	
	ON_WM_SHOWWINDOW()
	ON_STN_CLICKED(IDC_STATIC_SHADOWCOLOR, &_DlgMTShadow::OnStnClickedStaticShadowcolor)
	ON_BN_CLICKED(IDC_BUTTON_SMBLUR_APPLY, &_DlgMTShadow::OnBnClickedButtonSmblurApply)
	ON_BN_CLICKED(IDC_BUTTON_SMBLUR_BACK, &_DlgMTShadow::OnBnClickedButtonSmblurBack)
	ON_WM_HSCROLL()
	ON_CBN_SELCHANGE(IDC_COMBO_SMBLUR_SIZE, &_DlgMTShadow::OnCbnSelchangeComboSmblurSize)
	ON_BN_CLICKED(IDC_BUTTON_APPLY_LIGHT_AMBIENT, &_DlgMTShadow::OnBnClickedButtonApplyLightAmbient)
END_MESSAGE_MAP()




//===========================================================================================
//
//		virtual
//
//===========================================================================================

void _DlgMTShadow::_Delete()
{
	_DlgBase::_Delete();

	m_ShadowColor.Delete();	

	__DeleteSMBlur();
}

void _DlgMTShadow::_PostViewReset()
{
	// Shadow MPPG
	_ComboSelect( &m_combo_ShadowMPPG, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nShadowMPPG );

	__DeleteSMBlur();
	__InitSMBlur();
}

void _DlgMTShadow::_ViewInit()
{	
}



//===========================================================================================
//
//		Base 
//
//===========================================================================================

BOOL _DlgMTShadow::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ShadowColor.Create( &m_static_ShadowColor, NiColor::BLACK );

	// MPPG
	TCHAR str[ 32 ];
	int nComboIndex = 0;
	for( int i=0; i<=8; ++i )
	{
		int nPow = pow(2, i);
		_stprintf_s( str, 32, L"%d", nPow );
		m_combo_ShadowMPPG.InsertString( nComboIndex, str );
		++nComboIndex;
	}
	m_combo_ShadowMPPG.SetCurSel( 0 );

	// Shadow Map Blur
	m_chk_IncludeTR.SetCheck( BST_CHECKED );
	m_combo_SMBlur_Size.InsertString( 0, L"0" );
	m_combo_SMBlur_Size.SetItemData( 0, 0 );
	m_combo_SMBlur_Size.InsertString( 1, L"1" );
	m_combo_SMBlur_Size.SetItemData( 1, 1 );
	m_combo_SMBlur_Size.InsertString( 2, L"2" );
	m_combo_SMBlur_Size.SetItemData( 2, 2 );
	m_combo_SMBlur_Size.InsertString( 3, L"3" );
	m_combo_SMBlur_Size.SetItemData( 3, 3 );
	m_combo_SMBlur_Size.InsertString( 4, L"4" );
	m_combo_SMBlur_Size.SetItemData( 4, 4 );
	m_combo_SMBlur_Size.InsertString( 5, L"5" );
	m_combo_SMBlur_Size.SetItemData( 5, 5 );
	m_combo_SMBlur_Size.InsertString( 6, L"6" );
	m_combo_SMBlur_Size.SetItemData( 6, 6 );
	m_combo_SMBlur_Size.InsertString( 7, L"7" );
	m_combo_SMBlur_Size.SetItemData( 7, 7 );
	m_combo_SMBlur_Size.InsertString( 8, L"8" );
	m_combo_SMBlur_Size.SetItemData( 8, 8 );
	m_combo_SMBlur_Size.InsertString( 9, L"9" );
	m_combo_SMBlur_Size.SetItemData( 9, 9 );
	m_combo_SMBlur_Size.InsertString( 10, L"10" );
	m_combo_SMBlur_Size.SetItemData( 10, 10 );

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

HBRUSH _DlgMTShadow::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	switch( pWnd->GetDlgCtrlID() )
	{
	case IDC_STATIC_SHADOWCOLOR:	hbr = m_ShadowColor.s_Brush;	break;
	}

	return hbr;
}


void _DlgMTShadow::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		__ReadInfo();
	}
	else
	{		
		g_ToolNiNodeMng.GetAxisRotation()->ReleaseVisible();
	}

	if( IS_ENABLEFRM( nsMAPTOOL::MV_VIEWER ) == true )
		GET_MV_VIEW->SetFocus();
}


void _DlgMTShadow::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

void _DlgMTShadow::__ReadInfo()
{
	// 쉐도우 색	
	m_ShadowColor.Create( &m_static_ShadowColor, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_ShadowColor );

	// Shadow MPPG
	_ComboSelect( &m_combo_ShadowMPPG, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nShadowMPPG );
}

void _DlgMTShadow::OnBnClickedButtonApply()
{
	g_pShadow->Tool_ResetObjectShadowLight();

	// 오브젝트 쉐도우 라이트가 없을 경우 실패
	if( g_pShadow->GetShadowLight() == NULL )
	{
		CsMessageBox( MB_OK, L"오브젝트 쉐도우 체크가 활성화된 라이트가 존재하지 않습니다." );
		return;
	}

	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nShadowMPPG = _Control2Int( &m_combo_ShadowMPPG );
	_DlgNotice dn( _DlgNotice::NT_SHADOWMAP, NULL );
	dn.DoModal();

	__DeleteSMBlur();
	__InitSMBlur();
}

void _DlgMTShadow::OnStnClickedStaticShadowcolor()
{
	m_ShadowColor.OpenDlg( &m_static_ShadowColor );

	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_ShadowColor = m_ShadowColor.s_Color;
	m_ShadowColor.Create( &m_static_ShadowColor, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_ShadowColor );
}


void _DlgMTShadow::OnBnClickedButtonApplyLightAmbient()
{
	g_pShadow->Tool_ResetObjectShadowLight();

	// 오브젝트 쉐도우 라이트가 없을 경우 실패
	if( g_pShadow->GetShadowLight() == NULL )
	{
		CsMessageBox( MB_OK, L"오브젝트 쉐도우 체크가 활성화된 라이트가 존재하지 않습니다." );
		return;
	}

	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_ShadowColor = g_pShadow->GetShadowLight()->GetBaseInfo()->s_Ambient;
	m_ShadowColor.Create( &m_static_ShadowColor, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_ShadowColor );
}

//===========================================================================================
//
//		Shadow Map Blur
//
//===========================================================================================

void _DlgMTShadow::__InitSMBlur()
{
	assert_cs( m_ppbtSMBlurOrg == NULL );
	assert_cs( nsCsGBTerrain::g_pCurRoot != NULL );

	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	int nMapSize = pRootInfo->s_nInitMapSize * pRootInfo->s_nShadowMPPG;
	m_nSMBlurNodeCount = pRootInfo->s_nTotalLeafCount;
	m_ppbtSMBlurOrg = csnew BYTE*[ m_nSMBlurNodeCount ];
	for( int i=0; i<m_nSMBlurNodeCount; ++i )
	{
		m_ppbtSMBlurOrg[ i ] = csnew BYTE[ nMapSize*nMapSize*SHADOW_MAP_STRIDE ];
	}

	assert_cs( m_mapSMBlure_TBObj.size() == 0 );

	OnBnClickedButtonSmblurApply();
}

void _DlgMTShadow::__DeleteSMBlur()
{
	for( int i=0; i<m_nSMBlurNodeCount; ++i )
	{
		SAFE_DELETE_ARRAY( m_ppbtSMBlurOrg[ i ] );
	}
	SAFE_DELETE_ARRAY( m_ppbtSMBlurOrg );

	__DeleteDarkMapObjMap();
}

void _DlgMTShadow::__DeleteDarkMapObjMap()
{
	std::map< DWORD, BYTE* >::iterator it = m_mapSMBlure_TBObj.begin();
	for( ;it!=m_mapSMBlure_TBObj.end(); ++it )
	{
		SAFE_DELETE_ARRAY( it->second );
	}
	m_mapSMBlure_TBObj.clear();
}

void _DlgMTShadow::_ShadowMapBlur( int nSize )
{
	if( nSize == 0 )
	{
		OnBnClickedButtonSmblurBack();
		return;
	}

	// 픽셀 리미트
	if( m_chk_IncludeTR.GetCheck() == BST_CHECKED )
	{
		CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

		int nXStart = pRootInfo->GetTextureLeftFigure( pRootInfo->s_nShadowMPPG );
		int nXEnd = pRootInfo->GetTextureRightFigure( pRootInfo->s_nShadowMPPG ) + 1;
		int nYStart = pRootInfo->GetTextureTopFigure( pRootInfo->s_nShadowMPPG );
		int nYEnd = pRootInfo->GetTextureBottomFigure( pRootInfo->s_nShadowMPPG ) + 1;

		sBLUR_INFO blur;	
		blur.s_nSizeSqare = nSize*nSize;	
		blur.s_nLimitLeft = nXStart;
		blur.s_nLimitTop = nYStart;
		blur.s_nLimitRight = nXEnd;
		blur.s_nLimitBottom = nYEnd;
		blur.s_nPixelStride = SHADOW_MAP_STRIDE;

		assert_cs( m_nSMBlurNodeCount == nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount );
		for( int i=0; i<m_nSMBlurNodeCount; ++i )
		{
			// Texture 관련
			NiPixelData* pPData = GET_SHADERMAP_PIXELDATA( nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetNiNode(), SHADOW_MAP_SHADER_NTM );
			BYTE* pPixel = (BYTE*)pPData->GetPixels();

			blur.s_nNodeIndex = i;
			blur.s_nPixelWidth = pPData->GetWidth();

			for( int y=nYStart; y<nYEnd; ++y )
			{
				for( int x=nXStart; x<nXEnd; ++x )
				{
					blur.s_nSrcX = blur.s_nDestX = x;
					blur.s_nSrcY = blur.s_nDestY = y;
					UINT uiSumCount = 1;
					UINT uiSumColorR = blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::RED );
					UINT uiSumColorG = blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::GREEN );
					UINT uiSumColorB = blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::BLUE );

					--blur.s_nDestY;
					__SMBlurUp( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
					++blur.s_nDestY;

					++blur.s_nDestY;
					__SMBlurDown( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
					--blur.s_nDestY;

					--blur.s_nDestX;
					__SMBlurLeft( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
					++blur.s_nDestX;

					++blur.s_nDestX;
					__SMBlurRight( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );

					pPixel[ ( y*blur.s_nPixelWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::RED ] = (BYTE)( uiSumColorR/uiSumCount );
					pPixel[ ( y*blur.s_nPixelWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::GREEN ] = (BYTE)( uiSumColorG/uiSumCount );
					pPixel[ ( y*blur.s_nPixelWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::BLUE ] = (BYTE)( uiSumColorB/uiSumCount );
				}
			}
			pPData->MarkAsChanged();
		}

		g_pShadow->Tool_CopyBorderShadowMap();
	}
	

	// TB 텍스쳐들 블러
	std::map< DWORD, BYTE* >::iterator it = m_mapSMBlure_TBObj.begin();
	for( ;it!=m_mapSMBlure_TBObj.end(); ++it )
	{
		__SMBlurTBObj( it->first, it->second, nSize );
	}
}

void _DlgMTShadow::__SMBlurUp( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_INFO blur )
{
	if( blur.s_nDestY < blur.s_nLimitTop )
	{
		int nNodeCountInside = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nNodeCountInSide;
		int nYObject = blur.s_nNodeIndex/nNodeCountInside;
		// 경계에 오브젝트가 존재 하지않는다면
		if( ++nYObject == nNodeCountInside )
		{
			return;
		}

		// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
		blur.s_nNodeIndex += nNodeCountInside;

		// Src Dest 값 변경
		int nOldMap = blur.s_nDestY;
		blur.s_nDestY = blur.s_nLimitBottom;
		// 변화량 합
		blur.s_nSrcY += ( blur.s_nDestY - nOldMap - 1 );
		__SMBlurUp( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
		return;
	}

	int nDeltaX = blur.s_nDestX - blur.s_nSrcX;
	int nDeltaY = blur.s_nDestY - blur.s_nSrcY;

	if( ( nDeltaX*nDeltaX + nDeltaY*nDeltaY ) > blur.s_nSizeSqare )
		return;

	uiSumColorR += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::RED );
	uiSumColorG += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::GREEN );
	uiSumColorB += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::BLUE );
	++uiSumCount;

	--blur.s_nDestY;
	__SMBlurUp( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
	++blur.s_nDestY;

	--blur.s_nDestX;
	__SMBlurLeft( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
	++blur.s_nDestX;

	++blur.s_nDestX;
	__SMBlurRight( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
}

void _DlgMTShadow::__SMBlurDown( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_INFO blur )
{
	if( blur.s_nDestY >= blur.s_nLimitBottom )
	{
		int nNodeCountInside = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nNodeCountInSide;
		int nYObject = blur.s_nNodeIndex/nNodeCountInside;
		// 경계에 오브젝트가 존재 하지않는다면
		if( nYObject <= 0 )
		{
			return;
		}

		// 오브젝트가 존재 한다면 오브젝트 변경후 계속 알파값 적용
		blur.s_nNodeIndex -= nNodeCountInside;

		// Src Dest 값 변경
		int nOldMap = blur.s_nDestY;
		blur.s_nDestY = blur.s_nLimitTop - 1;
		// 변화량 합
		blur.s_nSrcY += ( blur.s_nDestY - nOldMap + 1 );
		__SMBlurDown( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
		return;
	}

	int nDeltaX = blur.s_nDestX - blur.s_nSrcX;
	int nDeltaY = blur.s_nDestY - blur.s_nSrcY;

	if( ( nDeltaX*nDeltaX + nDeltaY*nDeltaY ) > blur.s_nSizeSqare )
		return;

	uiSumColorR += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::RED );
	uiSumColorG += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::GREEN );
	uiSumColorB += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::BLUE );
	++uiSumCount;

	++blur.s_nDestY;
	__SMBlurDown( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
	--blur.s_nDestY;

	--blur.s_nDestX;
	__SMBlurLeft( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
	++blur.s_nDestX;

	++blur.s_nDestX;
	__SMBlurRight( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
}

void _DlgMTShadow::__SMBlurLeft( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_INFO blur )
{
	if( blur.s_nDestX < blur.s_nLimitLeft )
	{
		int nNodeCountInside = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nNodeCountInSide;
		int nXObject = blur.s_nNodeIndex%nNodeCountInside;
		// 경계에 오브젝트가 존재 하지않는다면
		if( nXObject <= 0 )
		{
			return;
		}

		--blur.s_nNodeIndex;

		// Src Dest 값 변경
		int nOldMap = blur.s_nDestX;
		blur.s_nDestX = blur.s_nLimitRight;
		// 변화량 합
		blur.s_nSrcX += ( blur.s_nDestX - nOldMap - 1 );
		__SMBlurLeft( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
		return;
	}

	int nDeltaX = blur.s_nDestX - blur.s_nSrcX;
	int nDeltaY = blur.s_nDestY - blur.s_nSrcY;

	if( ( nDeltaX*nDeltaX + nDeltaY*nDeltaY ) > blur.s_nSizeSqare )
		return;

	uiSumColorR += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::RED );
	uiSumColorG += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::GREEN );
	uiSumColorB += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::BLUE );
	++uiSumCount;

	--blur.s_nDestX;
	__SMBlurLeft( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
}

void _DlgMTShadow::__SMBlurRight( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_INFO blur )
{
	if( blur.s_nDestX >= blur.s_nLimitRight )
	{
		int nNodeCountInside = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nNodeCountInSide;
		int nXObject = blur.s_nNodeIndex%nNodeCountInside;
		// 경계에 오브젝트가 존재 하지않는다면
		if( ++nXObject == nNodeCountInside )
		{
			return;
		}

		++blur.s_nNodeIndex;

		// Src Dest 값 변경
		int nOldMap = blur.s_nDestX;
		blur.s_nDestX = blur.s_nLimitLeft - 1;
		// 변화량 합
		blur.s_nSrcX += ( blur.s_nDestX - nOldMap + 1 );
		__SMBlurRight( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
		return;
	}

	int nDeltaX = blur.s_nDestX - blur.s_nSrcX;
	int nDeltaY = blur.s_nDestY - blur.s_nSrcY;

	if( ( nDeltaX*nDeltaX + nDeltaY*nDeltaY ) > blur.s_nSizeSqare )
		return;

	uiSumColorR += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::RED );
	uiSumColorG += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::GREEN );
	uiSumColorB += blur.GetPixelColor( m_ppbtSMBlurOrg, sBLUR_INFO::BLUE );
	++uiSumCount;

	++blur.s_nDestX;
	__SMBlurRight( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
}

void _DlgMTShadow::__SMBlurTBObj( DWORD dwObjID, BYTE* pOrgPixel, int nSize )
{
	assert_cs( nSize != 0 );

	CsGBObject* pCsGBObject = nsCsGBTerrain::g_pCurRoot->FindCsGBObject( dwObjID );
	NiAVObject* pObject = nsCSGBFUNC::GetFirst_HaveTexProp( pCsGBObject->GetNiNode() );
	while( pObject )
	{
		NiTexturingProperty::Map* pDarkMap = ( (NiTexturingProperty*)pObject->GetProperty( NiTexturingProperty::GetType() ) )->GetDarkMap();

		if( pDarkMap != NULL )
		{
			NiSourceTexture* pTexture = (NiSourceTexture*)pDarkMap->GetTexture();
			pTexture->SetStatic( false );
			NiPixelData* pPData = pTexture->GetSourcePixelData();
			BYTE* pPixel = (BYTE*)pPData->GetPixels();

			int xEnd = pPData->GetWidth();
			int yEnd = pPData->GetHeight();

			sBLUR_TBOBJ_INFO blur;
			blur.s_nSizeSqare = nSize*nSize;
			blur.s_nPixelWidth = xEnd;
			blur.s_nPixelHeight = yEnd;
			blur.s_pOrgPixel = pOrgPixel;
			blur.s_nPixelStride = pPData->GetPixelStride();
			for( int y=0; y<yEnd; ++y )
			{
				for( int x=0; x<xEnd; ++x )
				{
					blur.s_nSrcX = blur.s_nDestX = x;
					blur.s_nSrcY = blur.s_nDestY = y;

					UINT uiSumCount = 1;
					UINT uiSumColorR = blur.GetPixelColor( sBLUR_TBOBJ_INFO::RED );
					UINT uiSumColorG = blur.GetPixelColor( sBLUR_TBOBJ_INFO::GREEN );
					UINT uiSumColorB = blur.GetPixelColor( sBLUR_TBOBJ_INFO::BLUE );

					--blur.s_nDestY;
					__SMBlurTBObjUp( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
					++blur.s_nDestY;

					++blur.s_nDestY;
					__SMBlurTBObjDown( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
					--blur.s_nDestY;

					--blur.s_nDestX;
					__SMBlurTBObjLeft( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
					++blur.s_nDestX;

					++blur.s_nDestX;
					__SMBlurTBObjRight( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );

					pPixel[ ( y*blur.s_nPixelWidth + x )*blur.s_nPixelStride + sBLUR_TBOBJ_INFO::RED ] = CsMin( 0xff, (BYTE)( uiSumColorR/uiSumCount ) );
					pPixel[ ( y*blur.s_nPixelWidth + x )*blur.s_nPixelStride + sBLUR_TBOBJ_INFO::GREEN ] = CsMin( 0xff, (BYTE)( uiSumColorG/uiSumCount ) );
					pPixel[ ( y*blur.s_nPixelWidth + x )*blur.s_nPixelStride + sBLUR_TBOBJ_INFO::BLUE ] = CsMin( 0xff, (BYTE)( uiSumColorB/uiSumCount ) );
				}
			}
			pPData->MarkAsChanged();
		}
		pObject = nsCSGBFUNC::GetNext_HaveTexProp( pCsGBObject->GetNiNode(), pObject );
	}
}


void _DlgMTShadow::__SMBlurTBObjUp( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_TBOBJ_INFO blur )
{
	if( blur.s_nDestY < 0 )
	{
		return;
	}

	int nDeltaX = blur.s_nDestX - blur.s_nSrcX;
	int nDeltaY = blur.s_nDestY - blur.s_nSrcY;

	if( ( nDeltaX*nDeltaX + nDeltaY*nDeltaY ) > blur.s_nSizeSqare )
		return;

	uiSumColorR += blur.GetPixelColor( sBLUR_TBOBJ_INFO::RED );
	uiSumColorG += blur.GetPixelColor( sBLUR_TBOBJ_INFO::GREEN );
	uiSumColorB += blur.GetPixelColor( sBLUR_TBOBJ_INFO::BLUE );
	++uiSumCount;

	--blur.s_nDestY;
	__SMBlurTBObjUp( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
	++blur.s_nDestY;

	--blur.s_nDestX;
	__SMBlurTBObjLeft( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
	++blur.s_nDestX;

	++blur.s_nDestX;
	__SMBlurTBObjRight( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
}

void _DlgMTShadow::__SMBlurTBObjDown( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_TBOBJ_INFO blur )
{
	if( blur.s_nDestY >= blur.s_nPixelHeight )
	{
		return;
	}

	int nDeltaX = blur.s_nDestX - blur.s_nSrcX;
	int nDeltaY = blur.s_nDestY - blur.s_nSrcY;

	if( ( nDeltaX*nDeltaX + nDeltaY*nDeltaY ) > blur.s_nSizeSqare )
		return;

	uiSumColorR += blur.GetPixelColor( sBLUR_TBOBJ_INFO::RED );
	uiSumColorG += blur.GetPixelColor( sBLUR_TBOBJ_INFO::GREEN );
	uiSumColorB += blur.GetPixelColor( sBLUR_TBOBJ_INFO::BLUE );
	++uiSumCount;

	++blur.s_nDestY;
	__SMBlurTBObjDown( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
	--blur.s_nDestY;

	--blur.s_nDestX;
	__SMBlurTBObjLeft( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
	++blur.s_nDestX;

	++blur.s_nDestX;
	__SMBlurTBObjRight( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
}

void _DlgMTShadow::__SMBlurTBObjLeft( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_TBOBJ_INFO blur )
{
	if( blur.s_nDestX < 0 )
	{
		return;
	}

	int nDeltaX = blur.s_nDestX - blur.s_nSrcX;
	int nDeltaY = blur.s_nDestY - blur.s_nSrcY;

	if( ( nDeltaX*nDeltaX + nDeltaY*nDeltaY ) > blur.s_nSizeSqare )
		return;

	uiSumColorR += blur.GetPixelColor( sBLUR_TBOBJ_INFO::RED );
	uiSumColorG += blur.GetPixelColor( sBLUR_TBOBJ_INFO::GREEN );
	uiSumColorB += blur.GetPixelColor( sBLUR_TBOBJ_INFO::BLUE );
	++uiSumCount;

	--blur.s_nDestX;
	__SMBlurTBObjLeft( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
}

void _DlgMTShadow::__SMBlurTBObjRight( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_TBOBJ_INFO blur )
{
	if( blur.s_nDestX >= blur.s_nPixelWidth )
	{
		return;
	}

	int nDeltaX = blur.s_nDestX - blur.s_nSrcX;
	int nDeltaY = blur.s_nDestY - blur.s_nSrcY;

	if( ( nDeltaX*nDeltaX + nDeltaY*nDeltaY ) > blur.s_nSizeSqare )
		return;

	uiSumColorR += blur.GetPixelColor( sBLUR_TBOBJ_INFO::RED );
	uiSumColorG += blur.GetPixelColor( sBLUR_TBOBJ_INFO::GREEN );
	uiSumColorB += blur.GetPixelColor( sBLUR_TBOBJ_INFO::BLUE );
	++uiSumCount;

	++blur.s_nDestX;
	__SMBlurTBObjRight( uiSumCount, uiSumColorR, uiSumColorG, uiSumColorB, blur );
}

// 블러 콤보 박스 변경
void _DlgMTShadow::OnCbnSelchangeComboSmblurSize()
{
	int nSize = (int)m_combo_SMBlur_Size.GetItemData( m_combo_SMBlur_Size.GetCurSel() );
	_DlgNotice nt( _DlgNotice::NT_SM_BLUR, &nSize );
	nt.DoModal();
}


// 적용 버튼 클릭
void _DlgMTShadow::OnBnClickedButtonSmblurApply()
{
	nsCsMfcFunc::ComboSelect_DWORD_Data( &m_combo_SMBlur_Size, 0 );

	assert_cs( m_nSMBlurNodeCount == nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount );
	for( int i=0; i<m_nSMBlurNodeCount; ++i )
	{
		// Texture 관련
		NiPixelData* pPData = GET_SHADERMAP_PIXELDATA( nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetNiNode(), SHADOW_MAP_SHADER_NTM );
		BYTE* pPixel = (BYTE*)pPData->GetPixels();
		int nTextureWidth = pPData->GetWidth();
		int nTextureHeight = pPData->GetHeight();
		for( int y=0; y<nTextureHeight; ++y )
		{
			for( int x=0; x<nTextureWidth; ++x )
			{
				m_ppbtSMBlurOrg[ i ][ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::RED ] =
					pPixel[ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE +  + sBLUR_INFO::RED ];
				m_ppbtSMBlurOrg[ i ][ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::GREEN ] =
					pPixel[ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE +  + sBLUR_INFO::GREEN ];
				m_ppbtSMBlurOrg[ i ][ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::BLUE ] =
					pPixel[ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE +  + sBLUR_INFO::BLUE ];
			}			
		}
	}

	// TB, TBP 오브젝트
	__DeleteDarkMapObjMap();

	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	for( int i=0; i<m_nSMBlurNodeCount; ++i )
	{
		MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		for( ;it!=pMap->end(); ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;
			if( it->second->UD_IsDarkMap() == false )
				continue;

			NiAVObject* pObject = (NiAVObject*)nsCSGBFUNC::GetFirst_HaveTexProp( it->second->GetNiNode() );
			NiTexturingProperty* pTexProp = (NiTexturingProperty*)pObject->GetProperty( NiTexturingProperty::GetType() );
			NiTexturingProperty::Map* pDarkMap = pTexProp->GetDarkMap();
			if( pDarkMap == NULL )
				continue;

			NiSourceTexture* pTexture = (NiSourceTexture*)pDarkMap->GetTexture();
			NiPixelData* pPData = pTexture->GetSourcePixelData();

			if( pPData == NULL )
			{				
				CsMessageBoxA( MB_OK, "dm 옵션을 준 오브젝트임에도 다크맵을 가지고 있습니다.\n%s", it->second->GetInfo()->s_cObjectPath );
				continue;
			}

			UINT uiTPixelCount = pPData->GetWidth()*pPData->GetHeight();
			BYTE* pOrgPixel = csnew BYTE[ uiTPixelCount*pPData->GetPixelStride() ];
			BYTE* pPixel = (BYTE*)pPData->GetPixels();
			memcpy( pOrgPixel, pPixel, sizeof( BYTE )*uiTPixelCount*pPData->GetPixelStride() );

			assert_cs( m_mapSMBlure_TBObj.find( it->second->GetInfo()->s_dwUniqID ) == m_mapSMBlure_TBObj.end() );
			m_mapSMBlure_TBObj[ it->second->GetInfo()->s_dwUniqID ] = pOrgPixel;
		}
	}
}

// 적용상태로 되돌리기 버튼 클릭
void _DlgMTShadow::OnBnClickedButtonSmblurBack()
{
	assert_cs( m_nSMBlurNodeCount == nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount );
	for( int i=0; i<m_nSMBlurNodeCount; ++i )
	{
		// Texture 관련
		NiPixelData* pPData = GET_SHADERMAP_PIXELDATA( nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetNiNode(), SHADOW_MAP_SHADER_NTM );
		BYTE* pPixel = (BYTE*)pPData->GetPixels();
		int nTextureWidth = pPData->GetWidth();
		int nTextureHeight = pPData->GetHeight();
		for( int y=0; y<nTextureHeight; ++y )
		{
			for( int x=0; x<nTextureWidth; ++x )
			{
				pPixel[ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::RED ] = 
					m_ppbtSMBlurOrg[ i ][ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::RED ];
				pPixel[ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::GREEN ] =
					m_ppbtSMBlurOrg[ i ][ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::GREEN ];
				pPixel[ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::BLUE ] =
					m_ppbtSMBlurOrg[ i ][ ( y*nTextureWidth + x )*SHADOW_MAP_STRIDE + sBLUR_INFO::BLUE ];
			}			
		}
		pPData->MarkAsChanged();
	}

	// TB, TBP 오브젝트
	nsCsGBTerrain::g_pCurRoot->ObjectResetCallReady();
	for( int i=0; i<m_nSMBlurNodeCount; ++i )
	{
		MAP_GBTERRAIN_OBJECT* pMap = nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetObjectMap();
		MAP_GBTERRAIN_OBJECT_IT it = pMap->begin();
		for( ;it!=pMap->end(); ++it )
		{
			if( it->second->IsCallReady() == false )
				continue;
			if( it->second->UD_IsDarkMap() == false )
				continue;

			NiAVObject* pObject = (NiAVObject*)nsCSGBFUNC::GetFirst_HaveTexProp( it->second->GetNiNode() );
			NiTexturingProperty::Map* pDarkMap = ( (NiTexturingProperty*)pObject->GetProperty( NiTexturingProperty::GetType() ) )->GetDarkMap();
			NiSourceTexture* pTexture = (NiSourceTexture*)pDarkMap->GetTexture();
			NiPixelData* pPData = pTexture->GetSourcePixelData();

			UINT uiTPixelCount = pPData->GetWidth()*pPData->GetHeight();
			BYTE* pPixel = (BYTE*)pPData->GetPixels();
			assert_cs( m_mapSMBlure_TBObj.find( it->second->GetInfo()->s_dwUniqID ) != m_mapSMBlure_TBObj.end() );
			memcpy( pPixel, m_mapSMBlure_TBObj[ it->second->GetInfo()->s_dwUniqID ], sizeof( BYTE )*uiTPixelCount*pPData->GetPixelStride() );
			pPData->MarkAsChanged();
		}
	}

	nsCsMfcFunc::ComboSelect_DWORD_Data( &m_combo_SMBlur_Size, 0 );
}


//===========================================================================================
//
//		Input
//
//===========================================================================================
void _DlgMTShadow::_KeyDownEscape()
{
}


