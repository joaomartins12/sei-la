// _DlgNewTerrain.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "_DlgNewTerrain.h"


// _DlgNewTerrain 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgNewTerrain, CDialog)

_DlgNewTerrain::_DlgNewTerrain( CsGBTerrainRoot::sINFO* pRootInfo, CWnd* pParent /*=NULL*/)
	: CDialog(_DlgNewTerrain::IDD, pParent)
{
	assert_cs( pRootInfo != NULL );
	m_pRootInfo = pRootInfo;
}

_DlgNewTerrain::~_DlgNewTerrain()
{
}

void _DlgNewTerrain::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_TWIDTH, m_edit_TWidth);
	DDX_Control(pDX, IDC_EDIT_THEIGHT, m_edit_THeight);
	DDX_Control(pDX, IDC_EDIT_GRIDWIDTH, m_edit_GridWidth);
	DDX_Control(pDX, IDC_COMBO_TREELEVEL, m_combo_TreeLevel);
	DDX_Control(pDX, IDC_EDIT_XVERT, m_edit_XVertCount);
	DDX_Control(pDX, IDC_EDIT_YVERT, m_edit_YVertCount);
	DDX_Control(pDX, IDC_BTN_OK, m_btn_Ok);
	DDX_Control(pDX, IDC_COMBO_BASE_MPPG, m_combo_BaseMPPG);
	DDX_Control(pDX, IDC_COMBO_ALPHA_MPPG, m_combo_AlphaMPPG);
}


BEGIN_MESSAGE_MAP(_DlgNewTerrain, CDialog)
	ON_BN_CLICKED(IDC_BTN_APPLY_MAKESET, &_DlgNewTerrain::OnBnClickedBtnApplyMakeset)
	ON_BN_CLICKED(IDC_BTN_OK, &_DlgNewTerrain::OnBnClickedBtnOk)
END_MESSAGE_MAP()


// _DlgNewTerrain 메시지 처리기입니다.

BOOL _DlgNewTerrain::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Total Range
	_Int2Control( &m_edit_TWidth, g_Resist.GetNewTerrain()->s_nWidth/METER_SIZE );
	_Int2Control( &m_edit_THeight, g_Resist.GetNewTerrain()->s_nHeight/METER_SIZE );

	// Grid
	_Int2Control( &m_edit_GridWidth, g_Resist.GetNewTerrain()->s_nGrid/METER_SIZE );

	// Tree Level
	int nComboIndex = 0;
	TCHAR str[ 32 ] = {0, };
	for( int i=1; i<=7; ++i )
	{
		_stprintf_s( str, 32, L"%d", i );
		m_combo_TreeLevel.InsertString( nComboIndex, str );
		++nComboIndex;
	}
	_ComboSelect( &m_combo_TreeLevel, g_Resist.GetNewTerrain()->s_nTreeLevel );

	// MPPG
	nComboIndex = 0;
	for( int i=0; i<=5; ++i )
	{
		int npow = pow(2, i);
		_stprintf_s( str, 32, L"%d", npow);
		m_combo_BaseMPPG.InsertString( nComboIndex, str );
		m_combo_AlphaMPPG.InsertString( nComboIndex, str );
		++nComboIndex;
	}
	_ComboSelect( &m_combo_BaseMPPG, g_Resist.GetNewTerrain()->s_nBaseMPPG );
	_ComboSelect( &m_combo_AlphaMPPG, g_Resist.GetNewTerrain()->s_nAlphaMPPG );

	// Ok Button
	m_btn_Ok.EnableWindow( false );

	return TRUE;
}

void _DlgNewTerrain::OnBnClickedBtnApplyMakeset()
{
	if( __CalVertCount() == true )
	{
		m_btn_Ok.EnableWindow( true );
	}
}

bool _DlgNewTerrain::__CalVertCount()
{
	TCHAR strMsg[ 1024 ] = {0, };
	int nTWidth, nTHeight, nGridWidth, nTreeLevel, nXVertCount, nYVertCount;

	// == Total Range
	nTWidth = _Control2Int( &m_edit_TWidth )*METER_SIZE;
	nTHeight = _Control2Int( &m_edit_THeight )*METER_SIZE;
	// = 오류
	if( ( nTWidth <= 0 )||( nTHeight <= 0 ) )
	{
		MessageBox( L"총 길이를 입력하세요 !!");
		return false;
	}

	// == Grid
	nGridWidth = _Control2Int( &m_edit_GridWidth )*METER_SIZE;
	// = 오류
	if( nGridWidth <= 0 )
	{
		MessageBox( L"그리드 사이즈를 입력하세요 !!");
		return false;
	}

	// == MPPG
	if( _Control2Int( &m_combo_BaseMPPG ) <= 0 )
	{
		MessageBox( L"기본맵의 MPPG는 1이상이여야 합니다!!");
		return false;
	}	
	if( _Control2Int( &m_combo_AlphaMPPG ) <= 0 )
	{
		MessageBox( L"알파채널의 MPPG는 1이상이여야 합니다!!");
		return false;
	}

	// == 수치 변경 처리
	// = x축
	int nTemp = nTWidth % nGridWidth;
	if( nTemp != 0 )
	{
		int nOldTWidth = nTWidth;
		nTWidth += nGridWidth - nTemp;
		_stprintf_s( strMsg, 1024, L"Total Range 의 Width 크기가 맞지 않아 %d -> %d 로 변경", nOldTWidth/METER_SIZE, nTWidth/METER_SIZE );
		MessageBox( strMsg );
		_Int2Control( &m_edit_TWidth, nTWidth/METER_SIZE );
	}
	assert_cs( nTWidth % nGridWidth == 0 );
	nXVertCount = nTWidth / nGridWidth + 1;	
	if( nXVertCount % 2 == 0 )
	{
		nXVertCount += 1;		
		int nOldTWidth = nTWidth;
		nTWidth += nGridWidth;
		_stprintf_s( strMsg, 1024, L"X축 Vertex의 산출 방법에 맞지 않아 Width 크기를 %d -> %d 로 변경", nOldTWidth/METER_SIZE, nTWidth/METER_SIZE );
		MessageBox( strMsg );

		_Int2Control( &m_edit_TWidth, nTWidth/METER_SIZE );
	}
	

	// = y축
	nTemp = nTHeight % nGridWidth;
	if( nTemp != 0 )
	{
		int nOldTHeight = nTHeight;
		nTHeight += nGridWidth - nTemp;
		_stprintf_s( strMsg, 1024, L"Total Range 의 Height 크기가 맞지 않아 %d -> %d 로 변경", nOldTHeight/METER_SIZE, nTHeight/METER_SIZE );
		MessageBox( strMsg );

		_Int2Control( &m_edit_THeight, nTHeight/METER_SIZE );
	}
	assert_cs( nTHeight % nGridWidth == 0 );
	nYVertCount = nTHeight / nGridWidth + 1;
	if( nYVertCount % 2 == 0 )
	{
		nYVertCount += 1;		
		int nOldTHeight = nTHeight;
		nTHeight += nGridWidth;
		_stprintf_s( strMsg, 1024, L"Y축 Vertex의 산출 방법에 맞지 않아 Height 크기를 %d -> %d 로 변경", nOldTHeight/METER_SIZE, nTHeight/METER_SIZE );
		MessageBox( strMsg );

		_Int2Control( &m_edit_THeight, nTHeight/METER_SIZE );
	}

	// == Tree Level	
	nTreeLevel = _Control2Int( &m_combo_TreeLevel );
	// = 오류
	int nPow = pow( 2, nTreeLevel ) * nGridWidth * 2;	// 마지막 2는 플랜 규격을 위한 최하 2개 이상의 짝수 이상이여야 하기문에
	if( ( nTWidth < nPow ) || ( nTHeight < nPow ) )
	{
		MessageBox( L"Total Range 가 TreeLevel과 Grid Width 에 비해 너무 작다" );
		return false;
	}

	// == Cal Vert Count	
	// = XVertCount
	nPow = 1;
	for( ; ( pow( 2, nPow ) + 1 ) < nXVertCount; ++nPow )
	{
	}
	_Int2Control( &m_edit_XVertCount, nXVertCount );
	
	// = YVertCount
	nPow = 1;
	for( ; ( pow( 2, nPow ) + 1 ) < nYVertCount; ++nPow )
	{
	}
	_Int2Control( &m_edit_YVertCount, nYVertCount );

	//// 인덱스 초과 범위
	int nSrcLimit = ( ( _Control2Int( &m_edit_XVertCount ) - 2 ) / ( pow( 2, _Control2Int( &m_combo_TreeLevel ) ) ) + 2 )*
		( ( _Control2Int( &m_edit_YVertCount ) - 2 ) / ( pow( 2, _Control2Int( &m_combo_TreeLevel ) ) ) + 2 );

	int nDestLimit = pow( 2, 16 ) - 1;
	if( nSrcLimit > nDestLimit )
	{
		TCHAR str[ 1024 ] = {0, };
		_stprintf_s( str, 1024, L"한 노드에 설정된 Vertex의 갯수가 너무 많습니다. 다시 설정하여 주십시오\n\n현   재 갯수 : %d\n\n사용가능 갯수 : %d", nSrcLimit, nDestLimit );
		MessageBox( str );
		return false;
	}

	return true;
}



void _DlgNewTerrain::OnBnClickedBtnOk()
{
	assert_cs( _Control2Int( &m_edit_XVertCount ) % 2 == 1 );
	assert_cs( _Control2Int( &m_edit_YVertCount ) % 2 == 1 );

	m_pRootInfo->s_nXVertCountInNode	=	( _Control2Int( &m_edit_XVertCount ) - 2 ) / ( pow( 2, _Control2Int( &m_combo_TreeLevel ) ) ) + 2;
	m_pRootInfo->s_nYVertCountInNode	=	( _Control2Int( &m_edit_YVertCount ) - 2 ) / ( pow( 2, _Control2Int( &m_combo_TreeLevel ) ) ) + 2;

	m_pRootInfo->s_nGridSize			=	_Control2Int( &m_edit_GridWidth )*METER_SIZE;
	m_pRootInfo->s_nTreeLevel			=	_Control2Int( &m_combo_TreeLevel );
	m_pRootInfo->s_nNodeCountInSide		=	pow( 2, _Control2Int( &m_combo_TreeLevel ) );
	m_pRootInfo->s_nTotalLeafCount		=	pow( m_pRootInfo->s_nNodeCountInSide, 2 );

	m_pRootInfo->s_nXVertCount			=	( m_pRootInfo->s_nXVertCountInNode - 1 )*m_pRootInfo->s_nNodeCountInSide + 1;
	m_pRootInfo->s_nYVertCount			=	( m_pRootInfo->s_nYVertCountInNode - 1 )*m_pRootInfo->s_nNodeCountInSide + 1;

	m_pRootInfo->s_fHeightRate			=	1.0f;

	// ==== Init Map
	int nTotalWidth = ( m_pRootInfo->s_nXVertCount - 1 )*m_pRootInfo->s_nGridSize;
	int nTotalHeight = ( m_pRootInfo->s_nYVertCount - 1 )*m_pRootInfo->s_nGridSize;
	int nNodeWidth = ( m_pRootInfo->s_nXVertCountInNode - 1 )*m_pRootInfo->s_nGridSize;
	int nNodeHeight = ( m_pRootInfo->s_nYVertCountInNode - 1 )*m_pRootInfo->s_nGridSize;

	int nNodeSize;
	if( nNodeWidth > nNodeHeight )	nNodeSize = nNodeWidth;
	else							nNodeSize = nNodeHeight;

	int nInitMapCoverSize = m_pRootInfo->s_nGridSize;
	while( nInitMapCoverSize-(m_pRootInfo->s_nGridSize*2) < nNodeSize )
	{
		nInitMapCoverSize *= 2;
	}	
	m_pRootInfo->s_nInitMapCoverSize = nInitMapCoverSize;
	m_pRootInfo->s_nInitMapSize = nInitMapCoverSize/m_pRootInfo->s_nGridSize;
	m_pRootInfo->s_fDetailMapSize = 1.0f;

	// ==== MPPG
	m_pRootInfo->s_nBaseMPPG = _Control2Int( &m_combo_BaseMPPG );
	m_pRootInfo->s_nAlphaMPPG = _Control2Int( &m_combo_AlphaMPPG );
	m_pRootInfo->s_nEffectMPPG = m_pRootInfo->s_nAlphaMPPG;

	// 레지스트리에 입력
	g_Resist.GetNewTerrain()->s_nWidth = _Control2Int( &m_edit_TWidth )*METER_SIZE;
	g_Resist.GetNewTerrain()->s_nHeight = _Control2Int( &m_edit_THeight )*METER_SIZE;
	g_Resist.GetNewTerrain()->s_nGrid = _Control2Int( &m_edit_GridWidth )*METER_SIZE;
	g_Resist.GetNewTerrain()->s_nBaseMPPG = _Control2Int( &m_combo_BaseMPPG );
	g_Resist.GetNewTerrain()->s_nAlphaMPPG = _Control2Int( &m_combo_AlphaMPPG );
	g_Resist.GetNewTerrain()->s_nTreeLevel = _Control2Int( &m_combo_TreeLevel );

	CDialog::OnOK();
}

