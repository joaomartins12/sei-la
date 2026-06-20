// _DlgMTHeight_Region.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTHeight_Region.h"


// _DlgMTHeight_Region 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTHeight_Region, CDialog)

_DlgMTHeight_Region::_DlgMTHeight_Region(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTHeight_Region::IDD, pParent),
	UsingUpdateObjPos(false)
{
	m_poMousePoint = CPoint(0,0);
}

_DlgMTHeight_Region::~_DlgMTHeight_Region()
{
}

void _DlgMTHeight_Region::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DELTA_HEIGHT, m_edit_DeltaHeight);
	DDX_Control(pDX, IDC_EDIT_SIZE, m_edit_Size);
	DDX_Control(pDX, IDC_EDIT_CONSTANT, m_edit_Constant);
	DDX_Control(pDX, IDC_SLIDER_DELTA_HEIGHT, m_sld_DeltaHeight);
	DDX_Control(pDX, IDC_SLIDER_SIZE, m_sld_Size);
	DDX_Control(pDX, IDC_SLIDER_CONSTANT, m_sld_Constant);
	DDX_Control(pDX, IDC_CHECK_SET_LIMITE, m_chk_SetLimite);
	DDX_Control(pDX, IDC_EDIT_LIMITE, m_edit_Limite);
	DDX_Control(pDX, IDC_RADIO_UP_LIMITE, m_rdo_UpLimit);
	DDX_Control(pDX, IDC_RADIO_DOWN_LIMITE, m_rdo_DownLimit);
	DDX_Control(pDX, IDC_COMBO_HEIGHT_TYPE, m_combo_Type);
	DDX_Control(pDX, IDC_CHECK_OBJUPDATE, m_UsingUpdateOBjectPos);
}


BEGIN_MESSAGE_MAP(_DlgMTHeight_Region, CDialog)
	ON_WM_SHOWWINDOW()	
	ON_WM_HSCROLL()
	ON_EN_CHANGE(IDC_EDIT_LIMITE, &_DlgMTHeight_Region::OnEnChangeEditLimite)
	ON_BN_CLICKED(IDC_BUTTON_RELEASE_LIMITE, &_DlgMTHeight_Region::OnBnClickedButtonReleaseLimite)
	ON_CBN_SELCHANGE(IDC_COMBO_HEIGHT_TYPE, &_DlgMTHeight_Region::OnCbnSelchangeComboHeightType)
	ON_BN_CLICKED(IDC_RADIO_UP_LIMITE, &_DlgMTHeight_Region::OnBnClickedRadioUpLimite)
	ON_BN_CLICKED(IDC_RADIO_DOWN_LIMITE, &_DlgMTHeight_Region::OnBnClickedRadioDownLimite)
	ON_BN_CLICKED(IDC_CHECK_OBJUPDATE, &_DlgMTHeight_Region::OnBnClickedCheckObjupdate)
END_MESSAGE_MAP()


// _DlgMTHeight_Region 메시지 처리기입니다.


//=============================================================================================
//
//		Virtual
//
//=============================================================================================
void _DlgMTHeight_Region::_UpdateView()
{
	if( GET_MV_VIEW->_IsShift() == true )
		__HeightChange( m_fDeltaHeight, m_fSize );
	else if( GET_MV_VIEW->_IsControl() )
		__HeightChange( -m_fDeltaHeight, m_fSize );
	else if( GET_MV_VIEW->_IsAlt() )
		__HeightChange( m_fDeltaHeight, m_fSize, true );
}

void _DlgMTHeight_Region::_PostRenderView()
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), m_poMousePoint.x, m_poMousePoint.y );		
	if( pRecord == NULL )
	{
		m_CurrIntersectPt= NiPoint3::ZERO;
		return;
	}
	m_CurrIntersectPt = pRecord->GetIntersection();

	//NiGeometry* pkGeo =  (NiGeometry*)pRecord->GetAVObject();
	//pkGeo->CalculateNormals();
	//NiPoint3* kNormal = pkGeo->GetNormals();
	NiPoint2 pt =NiPoint2( pRecord->GetIntersection().x, pRecord->GetIntersection().y );
	g_ToolNiNodeMng.GetRegion()->Render(  pt, m_fSize, NiColor( 0, 1, 0 )	);
}

//=============================================================================================
//
//		Base
//
//=============================================================================================

BOOL _DlgMTHeight_Region::OnInitDialog()
{
	_DlgBase::OnInitDialog();

	// 리미트
	m_rdo_UpLimit.SetCheck( BST_CHECKED );
	m_edit_Limite.SetWindowText( L"" );

	// 타입
	m_combo_Type.InsertString( T_DEFAULT, L"평범" );
	m_combo_Type.InsertString( T_LOWER_CHANGE, L"낮은부분만 변화" );
	m_combo_Type.SetCurSel( T_DEFAULT );
	OnCbnSelchangeComboHeightType();

	// 헤이트변화
	m_fDeltaHeight = 5.0f;
	nsCsMfcFunc::Float2Control( &m_edit_DeltaHeight, m_fDeltaHeight );
	m_sld_DeltaHeight.SetRange( 1, 4000 );
	m_sld_DeltaHeight.SetPos( (int)( m_fDeltaHeight*100.0f ) );

	m_fSize = 500.0f;
	nsCsMfcFunc::Int2Control( &m_edit_Size, (int)m_fSize );
	m_sld_Size.SetRange( 100, 10000 );
	m_sld_Size.SetPos( (int)m_fSize );

	m_fConstant = 1.0f;
	nsCsMfcFunc::Float2Control( &m_edit_Constant, m_fConstant, 3 );
	m_sld_Constant.SetRange( 0, 1000 );
	m_sld_Constant.SetPos( (int)( m_fConstant*1000.0f ) );	
	
	m_UsingUpdateOBjectPos.SetCheck(true);
	UsingUpdateObjPos = true;
	return TRUE;  // return TRUE unless you set the focus to a control
}

void _DlgMTHeight_Region::OnShowWindow(BOOL bShow, UINT nStatus)
{
	_DlgBase::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		m_bLButtonDown = false;
		m_bHeightChange = false;
	}
}
void _DlgMTHeight_Region::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_DeltaHeight.GetSafeHwnd() )
	{
		nsCsMfcFunc::Float2Control( &m_edit_DeltaHeight, m_sld_DeltaHeight.GetPos()*0.01f );
		m_fDeltaHeight = (float)m_sld_DeltaHeight.GetPos()*0.01f;
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_Size.GetSafeHwnd() )
	{
		nsCsMfcFunc::Int2Control( &m_edit_Size, m_sld_Size.GetPos() );
		m_fSize = (float)m_sld_Size.GetPos();
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_Constant.GetSafeHwnd() )
	{
		nsCsMfcFunc::Float2Control( &m_edit_Constant, m_sld_Constant.GetPos()*0.001f, 3 );
		m_fConstant = m_sld_Constant.GetPos()*0.001f;
	}

	_DlgBase::OnHScroll(nSBCode, nPos, pScrollBar);
}


//=============================================================================================
//
//		Height Limite
//
//=============================================================================================

// 리미트 값 에디트 창 변경
void _DlgMTHeight_Region::OnEnChangeEditLimite()
{
	TCHAR szEdit[ MAX_PATH ];
	m_edit_Limite.GetWindowText( szEdit, MAX_PATH );
	if( szEdit[ 0 ] == NULL )
	{
		m_rdo_UpLimit.EnableWindow( FALSE );
		m_rdo_DownLimit.EnableWindow( FALSE );
		m_eLimite = L_NONE;
	}
	else
	{
		m_fLimite = nsCsMfcFunc::Control2Float( &m_edit_Limite );
		m_rdo_UpLimit.EnableWindow( TRUE );
		m_rdo_DownLimit.EnableWindow( TRUE );
		m_eLimite = m_rdo_UpLimit.GetCheck() == BST_CHECKED ? L_UP : L_DOWN;
	}
}

// 해제 버튼 클릭
void _DlgMTHeight_Region::OnBnClickedButtonReleaseLimite()
{
	m_chk_SetLimite.SetCheck( BST_UNCHECKED );
	m_edit_Limite.SetWindowText( L"" );
}

// 위로 리밋 라디오 박스 클릭
void _DlgMTHeight_Region::OnBnClickedRadioUpLimite()
{
	m_eLimite = L_UP;
}

// 아래로 리밋 라디오 박스 클릭
void _DlgMTHeight_Region::OnBnClickedRadioDownLimite()
{
	m_eLimite = L_DOWN;
}

//=============================================================================================
//
//		Height Type
//
//=============================================================================================

// 타입 콤보 박스 변경
void _DlgMTHeight_Region::OnCbnSelchangeComboHeightType()
{
	m_eType = (eTYPE)m_combo_Type.GetCurSel();
}


//=============================================================================================
//
//		Height Change
//
//=============================================================================================

void _DlgMTHeight_Region::__InitHeightChange( CPoint pt )
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
	if( pRecord == NULL )
	{
		m_bHeightChange = false;
		return;
	}

	m_bHeightChange = true;	

	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	NiFixedString fixStr = pRecord->GetAVObject()->GetName();
	m_nTargetNodeIndex = atoi( fixStr );	

	m_pTargetLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( m_nTargetNodeIndex );
	CsGBTerrainLeaf::sNiTriINFO* pLeafNiInfo = m_pTargetLeaf->GetNiTriInfo();

	NiPoint3 ptIntersect = pRecord->GetIntersection();
	m_ptIntersect.x = (int)( ptIntersect.x - pLeafNiInfo->s_ptOffsetPos.x );
	m_ptIntersect.y = (int)( ptIntersect.y - pLeafNiInfo->s_ptOffsetPos.y );

	switch( m_eType )
	{
	case T_DEFAULT:
		break;
	case T_LOWER_CHANGE:
		m_fTypeHeight = ptIntersect.z;
		break;
	default:
		assert_cs( false );
	}
}

void _DlgMTHeight_Region::__HeightChange( float fDeltaHeight, float fSize, bool bSmooth )
{
	if( m_bHeightChange == false )
		return;

	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	sHEIGHT_BASE hb;	
	hb.s_fDeltaHeight = fDeltaHeight;
	hb.s_fGridSize = (float)pRootInfo->s_nGridSize;
	hb.s_fMaxSize = fSize;
	hb.s_ptSrcPos = NiPoint2( (float)m_ptIntersect.x, (float)m_ptIntersect.y );

	sHEIGHT_INFO info;
	info.s_nVertIndex_x = (int)(m_ptIntersect.x + pRootInfo->s_nGridSize*0.5f)/pRootInfo->s_nGridSize;
	info.s_nVertIndex_y = (int)(m_ptIntersect.y + pRootInfo->s_nGridSize*0.5f)/pRootInfo->s_nGridSize;
	info.s_pLeaf = m_pTargetLeaf;
	info.s_nLeafIndex = m_nTargetNodeIndex;
	info.s_ptDestPos = NiPoint2( (float)( info.s_nVertIndex_x*pRootInfo->s_nGridSize),
								 (float)( info.s_nVertIndex_y*pRootInfo->s_nGridSize ) );

	float fLength = ( hb.s_ptSrcPos - info.s_ptDestPos ).Length();

	if( fLength > fSize )
	{
		fLength = fSize - 50;
	}

	__SetHeight( &hb, &info, fLength, bSmooth );
	__HeightChangeUp( &hb, info, bSmooth );
	__HeightChangeDown( &hb, info, bSmooth );
	__HeightChangeRight( &hb, info, bSmooth );
	__HeightChangeLeft( &hb, info, bSmooth );	
}

bool _DlgMTHeight_Region::__GetHeightFromTotalIndex( int const& nPositionX, int const& nPositionY, __int64 const& nTotalIdx, float & fTotoalHeight, int const& nPickLeafNodeIdx )
{
	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
	int wx = pRootInfo->s_nXVertCountInNode;//한 노드의 X 버텍스 갯수
	int wy = pRootInfo->s_nYVertCountInNode;//한 노드의 Y 버텍스 갯수
	int nc = pRootInfo->s_nNodeCountInSide;//한변에서의 노드 갯수

	__int64 nNodeCountX = nc * wx;
	__int64 nNodeCountY = nc * wy;
	if( nTotalIdx < 0 || (nNodeCountX*nNodeCountY) <= nTotalIdx )
		return false;

	
	int nLinecountY = nTotalIdx / (nc * wy);
	int nLinecountX = nTotalIdx % (nc * wx);

	int nNodeY = nLinecountY / wy;
	int nNodeX = nLinecountX / wx;

	int nNodeIdx = nNodeY * nc + nNodeX;

	CsGBTerrainLeaf* pNewLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( nNodeIdx );
	if( NULL == pNewLeaf )
		return false;

	CsGBTerrainLeaf::sNiTriINFO* pNewLeafNiInfo = pNewLeaf->GetNiTriInfo();
	if( NULL == pNewLeafNiInfo )
		return false;

	int nIdxY = nLinecountY % wy;
	int nIdxX = nLinecountX % wx;

	if( nPickLeafNodeIdx  != nNodeIdx )// 다른 노드의 경계면 처리
	{
		if( nPositionX == 0 && nIdxX == (wx - 1) )//왼쪽
			nIdxX--;
		else if( nPositionX == 2 && nIdxX == 0)	// 오른쪽
			nIdxX++;
		
		if( nPositionY == 0 && nIdxY == (wy - 1)  ) // 아래
			nIdxY--;
		else if( nPositionY == 2 && nIdxY == 0 ) // 위
			nIdxY++;
	}

	int nVertexIDX = nIdxY * wx + nIdxX;
	if( nVertexIDX < 0 || nVertexIDX >= pNewLeafNiInfo->s_nVertCount )
		return false;

	fTotoalHeight = pNewLeafNiInfo->s_pfHeight[ nVertexIDX ];
	return true;
}

void _DlgMTHeight_Region::__SetHeight( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO* pInfo, float fLength, bool bSmooth )
{
	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();

	int wx = pRootInfo->s_nXVertCountInNode;//한 노드의 X 버텍스 갯수
	int wy = pRootInfo->s_nYVertCountInNode;//한 노드의 Y 버텍스 갯수
	int nc = pRootInfo->s_nNodeCountInSide;//한변에서의 노드 갯수
	int nTargetNodeIndex_x = pInfo->s_nLeafIndex%nc;
	int nTargetNodeIndex_y = pInfo->s_nLeafIndex/nc;

	// 현재 노드의 인덱스 - 변경되어야 하는 인덱스
	int nVertIndex = pInfo->s_nVertIndex_y*wx + pInfo->s_nVertIndex_x;

	// 실질 데이터 변경
	CsGBTerrainLeaf::sNiTriINFO* pLeafNiInfo = pInfo->s_pLeaf->GetNiTriInfo();

	switch( m_eType )
	{
	case T_DEFAULT:
		break;
	case T_LOWER_CHANGE:
		if( pBaseInfo->s_fDeltaHeight > 0 )
		{
			if( pLeafNiInfo->s_pfHeight[ nVertIndex ] > m_fTypeHeight )
				return;
		}
		else
		{
			if( pLeafNiInfo->s_pfHeight[ nVertIndex ] < m_fTypeHeight )
				return;
		}
		break;
	default:
		assert_cs( false );
	}

	float fOrginHeight = pLeafNiInfo->s_pfHeight[ nVertIndex ];
	// 현재 버택스의 변환값 산출
	float fLengthConstant = fLength/pBaseInfo->s_fMaxSize;
	float fHeight = fOrginHeight;
	if( bSmooth )
	{
		// (현재 노드 번호 % 한번의 전체 노드의 갯수) * 한노드의 버텍스 갯수 + 현재 노드에서의 버텍스 번호
		__int64 nRow = (pInfo->s_nLeafIndex % nc) * wx + pInfo->s_nVertIndex_x;
		// (현재 노드 번호 / 한번의 전체 노드의 갯수) * 한노드의 버텍스 갯수 + 현재 노드에서의 버텍스 번호
 		__int64 nCol =  (pInfo->s_nLeafIndex / nc) * wy + pInfo->s_nVertIndex_y;

 		int nCount = 0;
 		float fAverageHeight=0;
		for( int y = 0; y < 3; ++y )
		{
			for( int x = 0; x < 3; ++x )
			{				
				int nXpos = (nRow - 1 + x);
				int nYpos = (nCol - 1 + y);
				if( nXpos < 0 || nYpos < 0 || nXpos >= (wx*nc) || nYpos >= (wy*nc) )
					continue;
				__int64 nTotalIdx =  nc*wx*nYpos + nXpos;
				float fCurrentHeight = 0;
				if( __GetHeightFromTotalIndex( x, y, nTotalIdx, fCurrentHeight, pInfo->s_nLeafIndex ) )
				{
					fAverageHeight += fCurrentHeight;
					nCount++;
				}
			}
		}		

		if( nCount > 0 )
		{
			fAverageHeight /= (float)nCount;
			fHeight = NiLerp( fLengthConstant, fOrginHeight, fAverageHeight );
		}
	}
	else
	{
		float fDeltaHeight = pBaseInfo->s_fDeltaHeight - pBaseInfo->s_fDeltaHeight*pow(fLengthConstant, 2)*m_fConstant;
		fHeight = fOrginHeight + fDeltaHeight;
	}
	
	switch( m_eLimite )
	{
	case L_NONE:	pLeafNiInfo->s_pfHeight[ nVertIndex ] = fHeight;	break;
	case L_UP:		pLeafNiInfo->s_pfHeight[ nVertIndex ] = (fHeight > m_fLimite)?m_fLimite:fHeight;		break;
	case L_DOWN:	pLeafNiInfo->s_pfHeight[ nVertIndex ] = (fHeight < m_fLimite)?m_fLimite:fHeight;		break;
	default:
		assert_cs( false );
	}	

	NiTriStrips* pTriStrips = pInfo->s_pLeaf->GetNiTriStrip();
	NiPoint3* pPos = pTriStrips->GetVertices();
	pPos[ nVertIndex ].z = pLeafNiInfo->s_pfHeight[ nVertIndex ];
	pTriStrips->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
	pTriStrips->GetModelBound().ComputeFromData( pTriStrips->GetActiveVertexCount(), pPos );
		
	// 경계지역
	if( ( nTargetNodeIndex_y < nc - 1 )&&( pInfo->s_nVertIndex_y == wy - 1 ) )
	{
		CsGBTerrainLeaf* pNewLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nLeafIndex + nc );
		CsGBTerrainLeaf::sNiTriINFO* pNewLeafNiInfo = pNewLeaf->GetNiTriInfo();
		pNewLeafNiInfo->s_pfHeight[ pInfo->s_nVertIndex_x ] = pPos[ nVertIndex ].z;
		NiTriStrips* pNewTriStrip = pNewLeaf->GetNiTriStrip();
		NiPoint3* pNewPos = pNewTriStrip->GetVertices();
		pNewPos[ pInfo->s_nVertIndex_x ].z = pPos[ nVertIndex ].z;		
		pNewTriStrip->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
		pNewTriStrip->GetModelBound().ComputeFromData( pNewTriStrip->GetActiveVertexCount(), pNewPos );

		if( ( nTargetNodeIndex_x < nc - 1 )&&( pInfo->s_nVertIndex_x == wx - 1 ) )
		{
			CsGBTerrainLeaf* pNewLeaf2 = nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nLeafIndex + nc + 1 );
			CsGBTerrainLeaf::sNiTriINFO* pNewLeafNiInfo2 = pNewLeaf2->GetNiTriInfo();		
			pNewLeafNiInfo2->s_pfHeight[ 0 ] = pPos[ nVertIndex ].z;
			NiTriStrips* pNewTriStrip2 = pNewLeaf2->GetNiTriStrip();
			NiPoint3* pNewPos2 = pNewTriStrip2->GetVertices();
			pNewPos2[ 0 ].z	= pPos[ nVertIndex ].z;			
			pNewTriStrip2->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
			pNewTriStrip2->GetModelBound().ComputeFromData( pNewTriStrip2->GetActiveVertexCount(), pNewPos2 );
		}
		if( ( nTargetNodeIndex_x > 0 )&&( pInfo->s_nVertIndex_x == 0 ) )
		{
			CsGBTerrainLeaf* pNewLeaf2 = nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nLeafIndex + nc - 1 );
			CsGBTerrainLeaf::sNiTriINFO* pNewLeafNiInfo2 = pNewLeaf2->GetNiTriInfo();			
			pNewLeafNiInfo2->s_pfHeight[ wx-1 ] = pPos[ nVertIndex ].z;

			NiTriStrips* pNewTriStrip2 = pNewLeaf2->GetNiTriStrip();
			NiPoint3* pNewPos2 = pNewTriStrip2->GetVertices();
			pNewPos2[ wx-1 ].z = pPos[ nVertIndex ].z;			
			pNewTriStrip2->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
			pNewTriStrip2->GetModelBound().ComputeFromData( pNewTriStrip2->GetActiveVertexCount(), pNewPos2 );
		}
	}
	if( ( nTargetNodeIndex_y > 0 )&&( pInfo->s_nVertIndex_y == 0 ) )
	{
		CsGBTerrainLeaf* pNewLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nLeafIndex - nc );
		CsGBTerrainLeaf::sNiTriINFO* pNewLeafNiInfo = pNewLeaf->GetNiTriInfo();
		int nNewVertIndex = (wy-1)*wx + pInfo->s_nVertIndex_x;
		pNewLeafNiInfo->s_pfHeight[ nNewVertIndex ] = pPos[ nVertIndex ].z;

		NiTriStrips* pNewTriStrip = pNewLeaf->GetNiTriStrip();
		NiPoint3* pNewPos = pNewTriStrip->GetVertices();
		pNewPos[ nNewVertIndex ].z = pPos[ nVertIndex ].z;
		pNewTriStrip->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
		pNewTriStrip->GetModelBound().ComputeFromData( pNewTriStrip->GetActiveVertexCount(), pNewPos );

		if( ( nTargetNodeIndex_x < nc - 1 )&&( pInfo->s_nVertIndex_x == wx - 1 ) )
		{
			CsGBTerrainLeaf* pNewLeaf2 = nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nLeafIndex - nc + 1 );
			CsGBTerrainLeaf::sNiTriINFO* pNewLeafNiInfo2 = pNewLeaf2->GetNiTriInfo();		
			pNewLeafNiInfo2->s_pfHeight[ (wy-1)*wx ] = pPos[ nVertIndex ].z;
			NiTriStrips* pNewTriStrip2 = pNewLeaf2->GetNiTriStrip();
			NiPoint3* pNewPos2 = pNewTriStrip2->GetVertices();
			pNewPos2[ (wy-1)*wx ].z	= pPos[ nVertIndex ].z;			
			pNewTriStrip2->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
			pNewTriStrip2->GetModelBound().ComputeFromData( pNewTriStrip2->GetActiveVertexCount(), pNewPos2 );
		}
		if( ( nTargetNodeIndex_x > 0 )&&( pInfo->s_nVertIndex_x == 0 ) )
		{
			CsGBTerrainLeaf* pNewLeaf2 = nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nLeafIndex - nc - 1 );
			CsGBTerrainLeaf::sNiTriINFO* pNewLeafNiInfo2 = pNewLeaf2->GetNiTriInfo();		
			pNewLeafNiInfo2->s_pfHeight[ (wy-1)*wx + wx - 1 ] = pPos[ nVertIndex ].z;

			NiTriStrips* pNewTriStrip2 = pNewLeaf2->GetNiTriStrip();
			NiPoint3* pNewPos2 = pNewTriStrip2->GetVertices();
			pNewPos2[ (wy-1)*wx + wx - 1 ].z = pPos[ nVertIndex ].z;			
			pNewTriStrip2->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
			pNewTriStrip2->GetModelBound().ComputeFromData( pNewTriStrip2->GetActiveVertexCount(), pNewPos2 );
		}
	}
	if( ( nTargetNodeIndex_x < nc - 1 )&&( pInfo->s_nVertIndex_x == wx - 1 ) )
	{
		CsGBTerrainLeaf* pNewLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nLeafIndex + 1 );
		CsGBTerrainLeaf::sNiTriINFO* pNewLeafNiInfo = pNewLeaf->GetNiTriInfo();		
		pNewLeafNiInfo->s_pfHeight[ pInfo->s_nVertIndex_y*wx ] = pPos[ nVertIndex ].z;

		NiTriStrips* pNewTriStrip = pNewLeaf->GetNiTriStrip();
		NiPoint3* pNewPos = pNewTriStrip->GetVertices();
		pNewPos[ pInfo->s_nVertIndex_y*wx ].z	= pPos[ nVertIndex ].z;
		pNewTriStrip->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
		pNewTriStrip->GetModelBound().ComputeFromData( pNewTriStrip->GetActiveVertexCount(), pNewPos );
	}
	if( ( nTargetNodeIndex_x > 0 )&&( pInfo->s_nVertIndex_x == 0 ) )
	{
		CsGBTerrainLeaf* pNewLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( pInfo->s_nLeafIndex - 1 );
		CsGBTerrainLeaf::sNiTriINFO* pNewLeafNiInfo = pNewLeaf->GetNiTriInfo();
		int nNewVertIndex = pInfo->s_nVertIndex_y*wx + wx-1;
		pNewLeafNiInfo->s_pfHeight[ nNewVertIndex ]	= pPos[ nVertIndex ].z;

		NiTriStrips* pNewTriStrip = pNewLeaf->GetNiTriStrip();
		NiPoint3* pNewPos = pNewTriStrip->GetVertices();
		pNewPos[ nNewVertIndex ].z = pPos[ nVertIndex ].z;
		pNewTriStrip->GetModelData()->MarkAsChanged( NiGeometryData::VERTEX_MASK );
		pNewTriStrip->GetModelBound().ComputeFromData( pNewTriStrip->GetActiveVertexCount(), pNewPos );
	}
}

void _DlgMTHeight_Region::__HeightChangeUp( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO info, bool bSmooth )
{
	// 위로 한칸 이동
	++info.s_nVertIndex_y;
	info.s_ptDestPos.y += pBaseInfo->s_fGridSize;

	// 거리 계산
	float fLength = ( pBaseInfo->s_ptSrcPos - info.s_ptDestPos ).Length();
	if( fLength > pBaseInfo->s_fMaxSize )
		return;

	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();	
	// 경계지역
	if( info.s_nVertIndex_y == pRootInfo->s_nYVertCountInNode )
	{
		int nc = pRootInfo->s_nNodeCountInSide;
		int nTargetNodeIndex_y = info.s_nLeafIndex/nc;

		if( nTargetNodeIndex_y < nc - 1 )
		{
			info.s_nVertIndex_y = 1;
			info.s_nLeafIndex += nc;
			info.s_pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( info.s_nLeafIndex );
		}
		else
		{
			return;
		}
	}	

	// 높이값 적용
	__SetHeight( pBaseInfo, &info, fLength, bSmooth );
	__HeightChangeUp( pBaseInfo, info, bSmooth );
	__HeightChangeLeft( pBaseInfo, info, bSmooth );
	__HeightChangeRight( pBaseInfo, info , bSmooth);
}

void _DlgMTHeight_Region::__HeightChangeDown( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO info, bool bSmooth )
{
	// 아래로 한칸 이동
	--info.s_nVertIndex_y;
	info.s_ptDestPos.y -= pBaseInfo->s_fGridSize;

	// 거리 계산
	float fLength = ( pBaseInfo->s_ptSrcPos - info.s_ptDestPos ).Length();
	if( fLength > pBaseInfo->s_fMaxSize )
		return;

	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();	
	// 경계지역
	if( info.s_nVertIndex_y < 0 )
	{
		int nc = pRootInfo->s_nNodeCountInSide;
		int nTargetNodeIndex_y = info.s_nLeafIndex/nc;

		if( nTargetNodeIndex_y > 0 )
		{
			info.s_nVertIndex_y = pRootInfo->s_nYVertCountInNode - 2;
			info.s_nLeafIndex -= nc;
			info.s_pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( info.s_nLeafIndex );
		}
		else
		{
			return;
		}
	}	

	// 높이값 적용
	__SetHeight( pBaseInfo, &info, fLength, bSmooth );
	__HeightChangeDown( pBaseInfo, info, bSmooth );
	__HeightChangeLeft( pBaseInfo, info, bSmooth );
	__HeightChangeRight( pBaseInfo, info, bSmooth );
}

void _DlgMTHeight_Region::__HeightChangeLeft( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO info, bool bSmooth )
{
	// 좌로 한칸 이동
	--info.s_nVertIndex_x;
	info.s_ptDestPos.x -= pBaseInfo->s_fGridSize;

	// 거리 계산
	float fLength = ( pBaseInfo->s_ptSrcPos - info.s_ptDestPos ).Length();
	if( fLength > pBaseInfo->s_fMaxSize )
		return;

	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();	
	// 경계지역
	if( info.s_nVertIndex_x < 0 )
	{
		int nc = pRootInfo->s_nNodeCountInSide;
		int nTargetNodeIndex_x = info.s_nLeafIndex%nc;

		if( nTargetNodeIndex_x > 0 )
		{
			info.s_nVertIndex_x = pRootInfo->s_nXVertCountInNode - 2;
			info.s_nLeafIndex -= 1;
			info.s_pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( info.s_nLeafIndex );
		}
		else
		{
			return;
		}
	}	

	// 높이값 적용
	__SetHeight( pBaseInfo, &info, fLength, bSmooth );
	__HeightChangeLeft( pBaseInfo, info, bSmooth );
}

void _DlgMTHeight_Region::__HeightChangeRight( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO info, bool bSmooth )
{
	// 우로 한칸 이동
	++info.s_nVertIndex_x;
	info.s_ptDestPos.x += pBaseInfo->s_fGridSize;

	// 거리 계산
	float fLength = ( pBaseInfo->s_ptSrcPos - info.s_ptDestPos ).Length();
	if( fLength > pBaseInfo->s_fMaxSize )
		return;

	CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();	
	// 경계지역
	if( info.s_nVertIndex_x == pRootInfo->s_nXVertCountInNode )
	{
		int nc = pRootInfo->s_nNodeCountInSide;
		int nTargetNodeIndex_x = info.s_nLeafIndex%nc;

		if( nTargetNodeIndex_x < nc - 1 )
		{
			info.s_nVertIndex_x = 1;
			info.s_nLeafIndex += 1;
			info.s_pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( info.s_nLeafIndex );
		}
		else
		{
			return;
		}
	}	

	// 높이값 적용
	__SetHeight( pBaseInfo, &info, fLength, bSmooth );
	__HeightChangeRight( pBaseInfo, info, bSmooth );
}

//=============================================================================================
//
//		Control
//
//=============================================================================================




//=============================================================================================
//
//		Input
//
//=============================================================================================

void _DlgMTHeight_Region::_OnLButtonDown( CPoint pt )
{
	if( m_chk_SetLimite.GetCheck() == BST_CHECKED )
	{
		NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_ByTool( CAMERA_ST.GetCameraObj(), pt.x, pt.y );
		if( pRecord == NULL )
			return;

		nsCsMfcFunc::Float2Control( &m_edit_Limite, pRecord->GetIntersection().z, 3 );
		m_chk_SetLimite.SetCheck( BST_UNCHECKED );
	}
	else
	{
		m_bLButtonDown = true;
		__InitHeightChange( pt );

		g_RollBack.Begin();
		cRollBack::sRT_HEIGHT_INFO info;
		info.s_npfHeightCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
		g_RollBack.Push( &info );
	}	
}

void _DlgMTHeight_Region::_OnLButtonUp( CPoint pt )
{
	if( m_bLButtonDown == true )
	{
		nsCsGBTerrain::g_pCurRoot->MakeHeightField(UsingUpdateObjPos);
		//nsCsGBTerrain::g_pCurRoot->CalculateNormal();
		m_bLButtonDown = false;
		m_bHeightChange = false;

		g_RollBack.End();
	}	
}

void _DlgMTHeight_Region::_OnMouseMove( CPoint pt )
{
	m_poMousePoint = pt;
	if( m_bLButtonDown == true )
		__InitHeightChange( pt );
	/*if( ( m_bLButtonDown == true )&&( GET_MV_VIEW->_IsShift() == false ) )
		__InitHeightChange( pt );*/
}


void _DlgMTHeight_Region::SetHeightLimit(const float& fHeight)
{

		nsCsMfcFunc::Float2Control( &m_edit_Limite, fHeight, 3 );
}
void _DlgMTHeight_Region::OnBnClickedCheckObjupdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	bool Checked = (bool)m_UsingUpdateOBjectPos.GetCheck();
	
	m_UsingUpdateOBjectPos.SetCheck(Checked);
	UsingUpdateObjPos = Checked;
}
