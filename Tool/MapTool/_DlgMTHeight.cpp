// _DlgMTHeight.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTHeight.h"
#include "resource.h"


// _DlgMTHeight 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTHeight, CDialog)

_DlgMTHeight::_DlgMTHeight(CWnd* pParent /*=NULL*/)
	: _DlgBase(_DlgMTHeight::IDD, pParent)
	,m_bSpoidCursor(false)
{

}

_DlgMTHeight::~_DlgMTHeight()
{
}

void _DlgMTHeight::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HEIGHTMAP_PATH, m_edit_HeightMap);
	DDX_Control(pDX, IDC_BTN_HEIGHTMAP_REAPPLY, m_btn_ReApply);
	DDX_Control(pDX, IDC_EDIT_HEIGHT_RATE, m_edit_HeightMapRate);
	DDX_Control(pDX, IDC_COMBO_USERHEIGHT_TYPE, m_combo_UserHeightType);
	DDX_Control(pDX, IDC_BTN_HEIGHTMAP_REAPPLY_SHADOWMAP, m_btn_Reapply_ShadowMap);
}


BEGIN_MESSAGE_MAP(_DlgMTHeight, CDialog)
	ON_BN_CLICKED(IDC_BTN_HEIGHTMAP_LOAD, &_DlgMTHeight::OnBnClickedBtnHeightmapLoad)
	ON_BN_CLICKED(IDC_BTN_HEIGHTMAP_REAPPLY, &_DlgMTHeight::OnBnClickedBtnHeightmapReapply)
	ON_CBN_SELCHANGE(IDC_COMBO_USERHEIGHT_TYPE, &_DlgMTHeight::OnCbnSelchangeComboUserheightType)
	ON_BN_CLICKED(IDC_BTN_HEIGHTMAP_REAPPLY_SHADOWMAP, &_DlgMTHeight::OnBnClickedBtnHeightmapReapplyShadowmap)
END_MESSAGE_MAP()

//====================================================================================
//
//		Virtual
//
//====================================================================================

void _DlgMTHeight::_PostViewReset()
{
	if( ( nsCsGBTerrain::g_pCurRoot )&&( nsCsGBTerrain::g_pCurRoot->GetRootNode()->IsExistTerrain() == true ) )
	{
		CsGBTerrainRoot::sINFO* pRootInfo = nsCsGBTerrain::g_pCurRoot->GetInfo();
		if( CsBmp::IsBmpData_GrayMap( pRootInfo->s_szHeightMapFile ) == true )
		{
			m_btn_ReApply.EnableWindow( TRUE );
			m_btn_Reapply_ShadowMap.EnableWindow( TRUE );
			m_edit_HeightMap.SetWindowText( pRootInfo->s_szHeightMapFile );
		}
		else
		{
			m_btn_ReApply.EnableWindow( FALSE );
			m_btn_Reapply_ShadowMap.EnableWindow( FALSE );
			m_edit_HeightMap.SetWindowText( L"" );
		}
		_Float2Control( &m_edit_HeightMapRate, pRootInfo->s_fHeightRate );	
	}
	else
	{
		if( CsBmp::IsBmpData_GrayMap( g_Resist.GetHeightMap()->s_szFile ) == true )
		{
			m_btn_ReApply.EnableWindow( TRUE );
			m_btn_Reapply_ShadowMap.EnableWindow( TRUE );
			m_edit_HeightMap.SetWindowText( g_Resist.GetHeightMap()->s_szFile );
		}
		else
		{
			m_btn_ReApply.EnableWindow( FALSE );
			m_btn_Reapply_ShadowMap.EnableWindow( FALSE );
			m_edit_HeightMap.SetWindowText( L"" );
		}
		_Float2Control( &m_edit_HeightMapRate, g_Resist.GetHeightMap()->s_fHeightMapRate );	
	}	
}

void _DlgMTHeight::_UpdateView()
{
	switch( m_combo_UserHeightType.GetCurSel() )
	{
	case 0:
		m_DlgMTHeight_Region._UpdateView();
		break;
	default:
		assert_cs( false );
	}
}

void _DlgMTHeight::_PreRenderView()
{
	switch( m_combo_UserHeightType.GetCurSel() )
	{
	case 0:
		m_DlgMTHeight_Region._PreRenderView();
		break;
	default:
		assert_cs( false );
	}	
}

void _DlgMTHeight::_PostRenderView()
{
	switch( m_combo_UserHeightType.GetCurSel() )
	{
	case 0:
		m_DlgMTHeight_Region._PostRenderView();
		break;
	default:
		assert_cs( false );
	}	
}

//====================================================================================
//
//		Base
//
//====================================================================================

BOOL _DlgMTHeight::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_btn_ReApply.EnableWindow( FALSE );
	m_btn_Reapply_ShadowMap.EnableWindow( FALSE );
	m_edit_HeightMap.SetWindowText( L"" );
	_Float2Control( &m_edit_HeightMapRate, 1.0f );


	CRect rc, rcCombo;
	GetWindowRect( &rc );
	m_combo_UserHeightType.GetWindowRect( &rcCombo );

	m_DlgMTHeight_Region.Create( _DlgMTHeight_Region::IDD, this );
	m_DlgMTHeight_Region.SetWindowPos( NULL, 0, 150, rc.Width(), rc.Height() - 170, 0 );
	m_DlgMTHeight_Polygon.Create( _DlgMTHeight_Polygon::IDD, this );
	m_DlgMTHeight_Polygon.SetWindowPos( NULL, 0, 150, rc.Width(), rc.Height() - 170, 0 );

	m_combo_UserHeightType.InsertString( 0, L"영   역" );
	//m_combo_UserHeightType.InsertString( 1, L"폴리곤" );
	m_combo_UserHeightType.SetCurSel( 0 );
	OnCbnSelchangeComboUserheightType();
	
	mh_Spoid = (HCURSOR)LoadImage(AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_ICON_SPOID), 
		IMAGE_ICON, GetSystemMetrics(SM_CXICON ), GetSystemMetrics(SM_CXICON), 0); 

	return TRUE;
}


//====================================================================================
//
//		Height Map
//
//====================================================================================

// 로드 버튼 클릭
void _DlgMTHeight::OnBnClickedBtnHeightmapLoad()
{
	TCHAR szOut[ MAX_PATH ] = {0, };
	if( nsCSFILE::FileOpen( szOut, MAX_PATH, L"Height Map File(*.bmp)\0*.bmp\0", g_Resist.GetHeightMap()->s_szPath ) == false )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( szOut ) == false )
	{
		MessageBox( L"텍스쳐는 현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	TCHAR szRootPath[ MAX_PATH ] = {0, };
	TCHAR szRootFullPath[ MAX_PATH ] = {0, };
	nsCSFILE::GetRelativePath( szRootFullPath, MAX_PATH, szOut );
	nsCSFILE::GetFilePathOnly( szRootPath, MAX_PATH, szRootFullPath );

	g_Resist.GetHeightMap()->s_fHeightMapRate = _Control2float( &m_edit_HeightMapRate );
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fHeightRate = _Control2float( &m_edit_HeightMapRate );

	g_RollBack.Begin();
	cRollBack::sRT_HEIGHT_INFO info;
	info.s_npfHeightCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	g_RollBack.Push( &info );

	if( g_MapMng.ApplyHeightMap( szRootFullPath ) == true )
	{
		_tcscpy_s( g_Resist.GetHeightMap()->s_szFile, MAX_PATH, szRootFullPath );
		m_edit_HeightMap.SetWindowText( g_Resist.GetHeightMap()->s_szFile );
		nsCSFILE::GetFilePathOnly( g_Resist.GetHeightMap()->s_szPath, MAX_PATH, g_Resist.GetHeightMap()->s_szFile );
		m_btn_ReApply.EnableWindow( TRUE );
		m_btn_Reapply_ShadowMap.EnableWindow( TRUE );

		//==== Save Map Path
		_tcscpy_s( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_szHeightMapFile, MAX_PATH, g_Resist.GetHeightMap()->s_szFile );
	}

	g_RollBack.End();
}

// 적용 버튼 클릭
void _DlgMTHeight::OnBnClickedBtnHeightmapReapply()
{
	g_Resist.GetHeightMap()->s_fHeightMapRate = _Control2float( &m_edit_HeightMapRate );
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fHeightRate = _Control2float( &m_edit_HeightMapRate );

	TCHAR szHeightMap[ MAX_PATH ] = {0, };
	m_edit_HeightMap.GetWindowText( szHeightMap, MAX_PATH );

	g_RollBack.Begin();
	cRollBack::sRT_HEIGHT_INFO info;
	info.s_npfHeightCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	g_RollBack.Push( &info );

	g_MapMng.ApplyHeightMap( szHeightMap );

	g_RollBack.End();
}

void _DlgMTHeight::OnBnClickedBtnHeightmapReapplyShadowmap()
{
	OnBnClickedBtnHeightmapReapply();
	GET_MV_MAINTAB_DLG->_GetDlgShadow()->OnBnClickedButtonApply();
}


//====================================================================================
//
//		User Height Region
//
//====================================================================================

// 타입 콤보 박스 변경
void _DlgMTHeight::OnCbnSelchangeComboUserheightType()
{
	switch( m_combo_UserHeightType.GetCurSel() )
	{
	case 0:
		m_DlgMTHeight_Region.ShowWindow( SW_SHOW );
		m_DlgMTHeight_Polygon.ShowWindow( SW_HIDE );
		break;
	case 1:
		m_DlgMTHeight_Region.ShowWindow( SW_HIDE );
		m_DlgMTHeight_Polygon.ShowWindow( SW_SHOW );
		break;
	default:
		assert_cs( false );
	}
}



//====================================================================================
//
//		Input
//
//====================================================================================

void _DlgMTHeight::_OnLButtonDown( CPoint pt )
{
	if(m_bSpoidCursor)
	{
		NiPoint3 kHeightPick = m_DlgMTHeight_Region.GetCurrIntersectPT();
		m_DlgMTHeight_Region.SetHeightLimit(kHeightPick.z);
	}
	else
	{
		switch( m_combo_UserHeightType.GetCurSel() )
		{
		case 0:
			m_DlgMTHeight_Region._OnLButtonDown( pt );
			break;
		case 1:
			break;
		default:
			assert_cs( false );
		}
	}
}

void _DlgMTHeight::_OnLButtonUp( CPoint pt )
{
	//SetCursor(LoadCursor(NULL, IDC_ARROW)); 
	switch( m_combo_UserHeightType.GetCurSel() )
	{
	case 0:
		m_DlgMTHeight_Region._OnLButtonUp( pt );
		break;
	case 1:
		break;
	default:
		assert_cs( false );
	}
}

void _DlgMTHeight::_OnMouseMove( CPoint pt )
{
	if(m_bSpoidCursor)
		SetCursor(mh_Spoid); 

	switch( m_combo_UserHeightType.GetCurSel() )
	{
	case 0:
		m_DlgMTHeight_Region._OnMouseMove( pt );
		break;
	case 1:
		break;
	default:
		assert_cs( false );
	}
}
void _DlgMTHeight::OnKeyUp(const DWORD msg)
{
	switch(msg)
	{
	case 'I':
		{
			m_bSpoidCursor = false;
			SetCursor(LoadCursor(NULL, IDC_ARROW)); 
		}
		break;
	default:
		break;
	}
}
bool _DlgMTHeight::_PreCheckKey( DWORD vKeyCode, bool bDown )
{
	if(bDown == true)
	{
		switch(vKeyCode)
		{
		case 'I':
			{
				m_bSpoidCursor = true;
				SetCursor(mh_Spoid); 

			}
			break;
		default:
			break;
		}

	}

	return true;
}
