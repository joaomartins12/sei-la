// _DlgMTMapping.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MapTool.h"
#include "_DlgMTMapping.h"




// _DlgMTMapping 대화 상자입니다.

IMPLEMENT_DYNAMIC(_DlgMTMapping, CDialog)

_DlgMTMapping::_DlgMTMapping(CWnd* pParent /*=NULL*/)
: _DlgBase(_DlgMTMapping::IDD, pParent)
{
	m_ptMousePoint = CPoint(0,0);
}

_DlgMTMapping::~_DlgMTMapping()
{
}

void _DlgMTMapping::_Delete()
{
	_DlgBase::_Delete();

	m_Gray.Delete();
}

LPCTSTR _DlgMTMapping::_GetSelTexturePath()
{
	if( m_szSelTexturePath[ 0 ] == NULL )
		return NULL;
	return m_szSelTexturePath;
}

void _DlgMTMapping::_ResetPreviewTexture()
{
	TCHAR sz[ MAX_FILENAME ];
	if( m_rdo_DMap0.GetCheck() == BST_CHECKED )
	{
		m_rdo_DMap0.GetWindowText( sz, MAX_FILENAME );
		if( ( sz[ 0 ] == NULL )||( _tcscmp( sz, L"..." ) == 0 ) )
		{
			g_Preview.GetTexture()->ReleaseModelView();
		}
		else
		{
			HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap0Item );
			CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );

			g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
		}
	}
	else if( m_rdo_DMap1.GetCheck() == BST_CHECKED )
	{
		m_rdo_DMap1.GetWindowText( sz, MAX_FILENAME );
		if( ( sz[ 0 ] == NULL )||( _tcscmp( sz, L"..." ) == 0 ) )
		{
			g_Preview.GetTexture()->ReleaseModelView();
		}
		else
		{
			HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap1Item );
			CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );

			g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
		}
	}
	else if( m_rdo_DMap2.GetCheck() == BST_CHECKED )
	{
		m_rdo_DMap2.GetWindowText( sz, MAX_FILENAME );
		if( ( sz[ 0 ] == NULL )||( _tcscmp( sz, L"..." ) == 0 ) )
		{
			g_Preview.GetTexture()->ReleaseModelView();
		}
		else
		{
			HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap2Item );
			CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );

			g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
		}
	}
	else if( m_rdo_DMap3.GetCheck() == BST_CHECKED )
	{
		m_rdo_DMap3.GetWindowText( sz, MAX_FILENAME );
		if( ( sz[ 0 ] == NULL )||( _tcscmp( sz, L"..." ) == 0 ) )
		{
			g_Preview.GetTexture()->ReleaseModelView();
		}
		else
		{
			HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap3Item );
			CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );

			g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
		}
	}
	else if( m_rdo_DMap4.GetCheck() == BST_CHECKED )
	{
		m_rdo_DMap4.GetWindowText( sz, MAX_FILENAME );
		if( ( sz[ 0 ] == NULL )||( _tcscmp( sz, L"..." ) == 0 ) )
		{
			g_Preview.GetTexture()->ReleaseModelView();
		}
		else
		{
			HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap4Item );
			CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );

			g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
		}
	}
	else if( m_rdo_Gray.GetCheck() == BST_CHECKED )
	{
		g_Preview.GetTexture()->ReleaseModelView();
	}
	else if( m_rdo_White.GetCheck() == BST_CHECKED )
	{
		g_Preview.GetTexture()->ReleaseModelView();
	}	
}

void _DlgMTMapping::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_TREE_TEXTURE, m_tree_Texture);
	DDX_Control(pDX, IDC_RADIO_DMAP0, m_rdo_DMap0);
	DDX_Control(pDX, IDC_RADIO_DMAP1, m_rdo_DMap1);
	DDX_Control(pDX, IDC_RADIO_DMAP2, m_rdo_DMap2);
	DDX_Control(pDX, IDC_RADIO_DMAP3, m_rdo_DMap3);
	DDX_Control(pDX, IDC_RADIO_DMAP4, m_rdo_DMap4);
	DDX_Control(pDX, IDC_RADIO_GRAY, m_rdo_Gray);
	DDX_Control(pDX, IDC_RADIO_WHITE, m_rdo_White);
	DDX_Control(pDX, IDC_SLD_SIZE, m_sld_Size);
	DDX_Control(pDX, IDC_SLD_DECREMENT, m_sld_Decrement);
	DDX_Control(pDX, IDC_EDIT_SIZE, m_edit_AlphaSize);
	DDX_Control(pDX, IDC_EDIT_DECREMENT, m_edit_AlphaDecrement);
	DDX_Control(pDX, IDC_SLD_MAP_SIZE, m_sld_MapSize);
	DDX_Control(pDX, IDC_EDIT_MAP_SIZE, m_edit_MapSize);
	DDX_Control(pDX, IDC_COMBO_DECREASE_CONSTANT, m_combo_DecreaseConstant);
	DDX_Control(pDX, IDC_STATIC_GRAYCOLOR, m_static_GrayColor);
	DDX_Control(pDX, IDC_COMBO_MPPG, m_combo_MPPG);
	DDX_Control(pDX, IDC_SPIN_GARY, m_spin_Gray);
	DDX_Control(pDX, IDC_SLD_MAP_OP, m_sld_OP);
	DDX_Control(pDX, IDC_EDIT_MAP_OP, m_edit_OP);
	DDX_Control(pDX, IDC_COMBO_BASEMPPG, m_cmb_BaseMPPG);
}

void _DlgMTMapping::_Reset()
{
	m_nCheckMapIndex = -1;

	m_rdo_DMap0.SetWindowText( L"" );
	m_rdo_DMap1.SetWindowText( L"" );
	m_rdo_DMap2.SetWindowText( L"" );
	m_rdo_DMap3.SetWindowText( L"" );
	m_rdo_DMap4.SetWindowText( L"" );

	m_hDMap0Item = NULL;
	m_hDMap1Item = NULL;
	m_hDMap2Item = NULL;
	m_hDMap3Item = NULL;
	m_hDMap4Item = NULL;

	m_rdo_DMap0.SetCheck( BST_UNCHECKED );
	m_rdo_DMap1.SetCheck( BST_UNCHECKED );
	m_rdo_DMap2.SetCheck( BST_UNCHECKED );
	m_rdo_DMap3.SetCheck( BST_UNCHECKED );
	m_rdo_DMap4.SetCheck( BST_UNCHECKED );

	m_rdo_Gray.SetCheck( BST_UNCHECKED );
	m_rdo_Gray.SetCheck( BST_UNCHECKED );
	m_rdo_White.SetCheck( BST_UNCHECKED );
	m_tree_Texture.SelectItem( NULL );
	g_Preview.GetTexture()->ReleaseModelView();	

	_ComboSelect( &m_combo_DecreaseConstant, g_Resist.GetMapping()->s_fAlphaDecreaseConstant );	
}

void _DlgMTMapping::_Resist()
{
	if( nsCsGBTerrain::g_pCurRoot == NULL )
		return;

	TCHAR szFullName[ MAX_PATH ];
	TCHAR szPath[ MAX_PATH ];
	LPCTSTR szFile;

	CsGBTerrainRoot* pRoot = nsCsGBTerrain::g_pCurRoot;
	CsGBTerrainLeaf* pLeaf = NULL;
	CsGBTerrainLeaf::sNiTriINFO* pTriInfo = NULL;
	for( int i=0; i<pRoot->GetInfo()->s_nTotalLeafCount; ++i )
	{
		pLeaf = pRoot->GetNode( i );
		pTriInfo = pLeaf->GetNiTriInfo();

		if( pTriInfo->s_cDetail_Map0[ 0 ] != NULL )
		{
			M2W( szFullName, pTriInfo->s_cDetail_Map0, MAX_PATH );
			if( _taccess_s( szFullName, 0 ) == 0 )
			{
				nsCSFILE::GetFilePathOnly( szPath, MAX_PATH, szFullName, true );
				szFile = nsCSFILE::GetFileName( szFullName );
				m_hDMap0Item = __InsertTree( szPath, szFile );
			}			
		}
		if( pTriInfo->s_cDetail_Map1[ 0 ] != NULL )
		{
			M2W( szFullName, pTriInfo->s_cDetail_Map1, MAX_PATH );
			if( _taccess_s( szFullName, 0 ) == 0 )
			{
				nsCSFILE::GetFilePathOnly( szPath, MAX_PATH, szFullName, true );
				szFile =  nsCSFILE::GetFileName( szFullName );
				m_hDMap1Item = __InsertTree( szPath, szFile );
			}
		}
		if( pTriInfo->s_cDetail_Map2[ 0 ] != NULL )
		{
			M2W( szFullName, pTriInfo->s_cDetail_Map2, MAX_PATH );
			if( _taccess_s( szFullName, 0 ) == 0 )
			{
				nsCSFILE::GetFilePathOnly( szPath, MAX_PATH, szFullName, true );
				szFile =  nsCSFILE::GetFileName( szFullName );
				m_hDMap2Item = __InsertTree( szPath, szFile );
			}
		}
		if( pTriInfo->s_cDetail_Map3[ 0 ] != NULL )
		{
			M2W( szFullName, pTriInfo->s_cDetail_Map3, MAX_PATH );
			if( _taccess_s( szFullName, 0 ) == 0 )
			{
				nsCSFILE::GetFilePathOnly( szPath, MAX_PATH, szFullName, true );
				szFile =  nsCSFILE::GetFileName( szFullName );
				m_hDMap3Item = __InsertTree( szPath, szFile );
			}
		}
		if( pTriInfo->s_cDetail_Map4[ 0 ] != NULL )
		{
			M2W( szFullName, pTriInfo->s_cDetail_Map4, MAX_PATH );
			if( _taccess_s( szFullName, 0 ) == 0 )
			{
				nsCSFILE::GetFilePathOnly( szPath, MAX_PATH, szFullName, true );
				szFile =  nsCSFILE::GetFileName( szFullName );
				m_hDMap4Item = __InsertTree( szPath, szFile );
			}
		}
	}

	g_ToolResource.SaveMapping();	
}

void _DlgMTMapping::_KeyDownEscape()
{
	// 셀렉트 취소
	m_nCheckMapIndex = -1;
	m_rdo_DMap0.SetCheck( BST_UNCHECKED );
	m_rdo_DMap1.SetCheck( BST_UNCHECKED );
	m_rdo_DMap2.SetCheck( BST_UNCHECKED );
	m_rdo_DMap3.SetCheck( BST_UNCHECKED );
	m_rdo_Gray.SetCheck( BST_UNCHECKED );
	m_rdo_White.SetCheck( BST_UNCHECKED );

	m_tree_Texture.SelectItem( NULL );
	g_Preview.GetTexture()->ReleaseModelView();
}

void _DlgMTMapping::__DeleteTree()
{
	m_tree_Texture.DeleteAllItems();
	g_ToolResource.SaveMapping();
}

HTREEITEM _DlgMTMapping::__InsertTree( LPCTSTR szRootPath, LPCTSTR szFile )
{
	LPCTSTR szSub = nsCSFILE::GetSubPath( szRootPath, L"Data\\" );

	// 이미 같은 루트가 존재 하는지 검색해 본다
	HTREEITEM hRoot = NULL;
	HTREEITEM hRootFind = m_tree_Texture.GetRootItem();
	while( hRootFind != NULL )
	{
		if( _tcscmp( szSub, m_tree_Texture.GetItemText( hRootFind ) ) == 0 )
		{
			hRoot = hRootFind;
			break;
		}
		hRootFind = m_tree_Texture.GetNextSiblingItem( hRootFind );
	}

	// 같은 루트가 존재 하지 않는다면 새로 생성
	if( hRoot == NULL )
	{
		hRoot = m_tree_Texture.InsertItem( szSub, NULL, TVI_SORT );
		m_tree_Texture.SetItemData( hRoot, TT_ROOT );
	}

	// 같은 레벨상에 같은이름이 존재 하는지 검사
	HTREEITEM hChildFind = m_tree_Texture.GetChildItem( hRoot );
	while( hChildFind != NULL )
	{
		if( _tcscmp( szFile, m_tree_Texture.GetItemText( hChildFind ) ) == 0 )
		{
			return hChildFind;
		}
		hChildFind = m_tree_Texture.GetNextSiblingItem( hChildFind );
	}
	HTREEITEM hChild = m_tree_Texture.InsertItem( szFile, hRoot, TVI_SORT );
	m_tree_Texture.SetItemData( hChild, TT_NODE );
	return hChild;
}

void _DlgMTMapping::__ReadMapSize( bool bMapping )
{
	if( bMapping == true )
	{
		m_sld_Size.SetPos( g_Resist.GetMapping()->s_nMappingSize );
		m_sld_Decrement.SetPos( g_Resist.GetMapping()->s_nMappingDecrement );	
		_ComboSelect( &m_combo_DecreaseConstant, g_Resist.GetMapping()->s_fAlphaDecreaseConstant );
	}
	else
	{		
		m_sld_Size.SetPos( g_Resist.GetMapping()->s_nGraySize );
		m_sld_Decrement.SetPos( g_Resist.GetMapping()->s_nGrayDecrement );	
		_ComboSelect( &m_combo_DecreaseConstant, g_Resist.GetMapping()->s_fGrayDecreaseConstant );
	}

	m_sld_MapSize.SetPos( (int)( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_fDetailMapSize*1000.0f + 0.5f ) );
	_Float2Control( &m_edit_MapSize, m_sld_MapSize.GetPos()*0.001f, 3 );
	_Float2Control( &m_edit_AlphaSize, m_sld_Size.GetPos()*0.1f );
	_Float2Control( &m_edit_AlphaDecrement, m_sld_Decrement.GetPos()*0.1f );
}

BEGIN_MESSAGE_MAP(_DlgMTMapping, CDialog)
	ON_WM_SHOWWINDOW()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_ADD_TEXTURE, &_DlgMTMapping::OnBnClickedBtnAddTexture)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_TEXTURE, &_DlgMTMapping::OnTvnSelchangedTreeTexture)
	ON_BN_CLICKED(IDC_BTN_DELETE_ALL, &_DlgMTMapping::OnBnClickedBtnDeleteAll)
	ON_BN_CLICKED(IDC_BUTTON_SET_DMAP0, &_DlgMTMapping::OnBnClickedButtonSetDmap0)
	ON_BN_CLICKED(IDC_BUTTON_SET_DMAP1, &_DlgMTMapping::OnBnClickedButtonSetDmap1)	
	ON_BN_CLICKED(IDC_BUTTON_SET_DMAP2, &_DlgMTMapping::OnBnClickedButtonSetDmap2)
	ON_BN_CLICKED(IDC_BUTTON_SET_DMAP3, &_DlgMTMapping::OnBnClickedButtonSetDmap3)
	ON_BN_CLICKED(IDC_BUTTON_SET_DMAP4, &_DlgMTMapping::OnBnClickedButtonSetDmap4)
	ON_BN_CLICKED(IDC_RADIO_DMAP0, &_DlgMTMapping::OnBnClickedRadioDmap0)
	ON_BN_CLICKED(IDC_RADIO_DMAP1, &_DlgMTMapping::OnBnClickedRadioDmap1)
	ON_BN_CLICKED(IDC_RADIO_DMAP2, &_DlgMTMapping::OnBnClickedRadioDmap2)
	ON_BN_CLICKED(IDC_RADIO_DMAP3, &_DlgMTMapping::OnBnClickedRadioDmap3)
	ON_BN_CLICKED(IDC_RADIO_DMAP4, &_DlgMTMapping::OnBnClickedRadioDmap4)
	ON_BN_CLICKED(IDC_RADIO_GRAY, &_DlgMTMapping::OnBnClickedRadioGray)
	ON_BN_CLICKED(IDC_RADIO_WHITE, &_DlgMTMapping::OnBnClickedRadioWhite)
	ON_CBN_SELCHANGE(IDC_COMBO_DECREASE_CONSTANT, &_DlgMTMapping::OnCbnSelchangeComboDecreaseConstant)
	ON_BN_CLICKED(IDC_BUTTON_SWAP_12, &_DlgMTMapping::OnBnClickedButtonSwap12)
	ON_BN_CLICKED(IDC_BUTTON_SWAP_23, &_DlgMTMapping::OnBnClickedButtonSwap23)
	ON_BN_CLICKED(IDC_BUTTON_SWAP_13, &_DlgMTMapping::OnBnClickedButtonSwap13)
	ON_STN_CLICKED(IDC_STATIC_GRAYCOLOR, &_DlgMTMapping::OnStnClickedStaticGraycolor)
	ON_WM_CTLCOLOR()
	ON_CBN_SELCHANGE(IDC_COMBO_MPPG, &_DlgMTMapping::OnCbnSelchangeComboMppg)
	ON_BN_CLICKED(IDC_BUTTON_GRAY_CLEAR, &_DlgMTMapping::OnBnClickedButtonGrayClear)
	ON_CBN_SELCHANGE(IDC_COMBO_BASEMPPG, &_DlgMTMapping::OnCbnSelchangeComboBasemppg)
END_MESSAGE_MAP()


// _DlgMTMapping 메시지 처리기입니다.
BOOL _DlgMTMapping::OnInitDialog()
{
	CDialog::OnInitDialog();

	// MPPG
	TCHAR str[ 16 ];
	for( int i=0; i<=6; ++i )
	{
		int nMPPG = pow( 2, i );
		_stprintf_s( str, 16, L"%d", nMPPG );
		int nComboIndex = m_combo_MPPG.GetCount();
		m_combo_MPPG.InsertString( nComboIndex, str );
		m_combo_MPPG.SetItemData( nComboIndex, nMPPG );

		m_cmb_BaseMPPG.InsertString( nComboIndex, str );
		m_cmb_BaseMPPG.SetItemData( nComboIndex, nMPPG );
	}

	m_spin_Gray.SetCsAccel( 1 );
	m_spin_Gray.ResistCsMoveCallBack( (void*)&MoveCallBack_Gray );

	m_sld_Size.SetRange( 10, 500 );
	m_sld_Decrement.SetRange( 10, 500 );
	m_sld_MapSize.SetRange( 1, 16000 );	
	m_sld_MapSize.SetPos( 1000 );

	_Float2Control( &m_edit_AlphaSize, m_sld_Size.GetPos()*0.1f );
	_Float2Control( &m_edit_AlphaDecrement, m_sld_Decrement.GetPos()*0.1f );
	_Float2Control( &m_edit_MapSize, m_sld_MapSize.GetPos()*0.001f, 3 );

	m_sld_OP.SetRange( 1, 100 );
	m_sld_OP.SetPos( 100 );
	nsCsMfcFunc::Float2Control( &m_edit_OP, 1.0f, 2 );
	m_fOpercity = 1.0f;

	TCHAR sz[ 16 ];
	for( int i=1; i<=9; ++i )
	{
		_stprintf_s( sz, 16, L"%.1f", i*0.1f );
		m_combo_DecreaseConstant.InsertString( i-1, sz );
	}

	m_Gray.Create( &m_static_GrayColor, g_Resist.GetMapping()->s_GrayColor );
	
	g_ToolResource.LoadMapping();

	_Reset();

	return TRUE;
}

void _DlgMTMapping::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialog::OnShowWindow(bShow, nStatus);

	if( bShow == TRUE )
	{
		__ReadMapSize( !( ( m_nCheckMapIndex == 5 )||( m_nCheckMapIndex == 6 ) ) );	
		nsCsMfcFunc::ComboSelect( &m_combo_MPPG, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nAlphaMPPG );
		nsCsMfcFunc::ComboSelect( &m_cmb_BaseMPPG, nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nBaseMPPG );
	}
	else
	{
	}

	if( IS_ENABLEFRM( nsMAPTOOL::MV_VIEWER ) == true )
		GET_MV_VIEW->SetFocus();
}


void _DlgMTMapping::OnBnClickedBtnAddTexture()
{
#define MAP_TEXTURE_PATH		L".\\Data\\Map\\"

	// 폴더가 존재 하지 않는다면 폴더 생성	
	nsCSFILE::CreateDirectory( MAP_TEXTURE_PATH );

	CFileDialog fd( TRUE, NULL, NULL, OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_FILEMUSTEXIST | OFN_ALLOWMULTISELECT | OFN_EXPLORER,
		L"dds Files (*.dds)|*.dds||" );

	if( _taccess_s( g_Resist.GetMapping()->s_szTexturePath, 0 ) != 0 )
		g_Resist.GetMapping()->Reset();
	
	fd.m_ofn.lpstrInitialDir = g_Resist.GetMapping()->s_szTexturePath;
	

	if( fd.DoModal() == IDCANCEL )
		return;

	// 하위폴더 여야만 한다.
	if( nsCSFILE::IsSubDirectory( fd.GetPathName() ) == false )
	{
		MessageBox( L"텍스쳐는 현재 디렉토리의 하위 디렉토리에 존재해야만 합니다." );
		return;
	}

	TCHAR szRootPath[ MAX_PATH ] = {0, };
	TCHAR szRootFullPath[ MAX_PATH ] = {0, };	
	nsCSFILE::GetRelativePath( szRootFullPath, MAX_PATH, fd.GetPathName() );
	nsCSFILE::GetFilePathOnly( szRootPath, MAX_PATH, szRootFullPath );

	// 로드 한거 입력
	for( POSITION pos = fd.GetStartPosition(); pos != NULL; )
	{
		CString strObject = fd.GetNextPathName( pos );
		LPCTSTR szFile = nsCSFILE::GetFileName( strObject );

		__InsertTree( szRootPath, szFile );
	}

	_tcscpy_s( g_Resist.GetMapping()->s_szTexturePath, MAX_PATH, szRootPath );

	g_ToolResource.SaveMapping();
}

void _DlgMTMapping::OnTvnSelchangedTreeTexture(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	HTREEITEM hSelItem = pNMTreeView->itemNew.hItem;
	if( hSelItem == NULL )
	{
		g_Preview.GetTexture()->ReleaseModelView();
		return;
	}

	if( m_tree_Texture.GetItemData( hSelItem ) == TT_ROOT )
	{
		g_Preview.GetTexture()->ReleaseModelView();
		return;
	}

	assert_cs( m_tree_Texture.GetItemData( hSelItem ) == TT_NODE );

	HTREEITEM hParent = m_tree_Texture.GetParentItem( hSelItem );
	CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );
	CString strSel = m_tree_Texture.GetItemText( hSelItem );

	g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), strSel.GetBuffer() );

	*pResult = 0;
}

void _DlgMTMapping::OnBnClickedBtnDeleteAll()
{
	if( m_tree_Texture.GetCount() == NULL )
		return;

	if( MessageBox( L"텍스쳐 창을 초기화 합니다. 계속 하시겠습니까?", NULL, MB_OKCANCEL ) == IDCANCEL )
		return;

	__DeleteTree();
}

void _DlgMTMapping::OnBnClickedButtonSetDmap0()
{
	HTREEITEM hSelItem = m_tree_Texture.GetSelectedItem();
	if( ( hSelItem == NULL )||( m_tree_Texture.GetItemData( hSelItem ) == TT_ROOT ) )
	{
		MessageBox( L"첫번째 디테일 맵은 비울수 없습니다." );
		return;
	}
	assert_cs( m_tree_Texture.GetItemData( hSelItem ) == TT_NODE );

	HTREEITEM hParent = m_tree_Texture.GetParentItem( hSelItem );
	CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );
	CString strSel = m_tree_Texture.GetItemText( hSelItem );

	m_rdo_DMap0.SetWindowText( strSel );

	// ==== Texture Setting
	TCHAR szTexPath[ MAX_PATH ] = {0, };
	_stprintf_s( szTexPath, MAX_PATH, L"%s\\%s", strParent.GetBuffer(), strSel.GetBuffer() );
	GET_MV_NODE->_SetDetail_Map0( szTexPath );

	m_hDMap0Item = hSelItem;

	if( m_rdo_DMap0.GetCheck() == BST_CHECKED )
		m_nCheckMapIndex = DETAIL_MAP_0_SHADER_NTM;
}


void _DlgMTMapping::OnBnClickedButtonSetDmap1()
{
	HTREEITEM hSelItem = m_tree_Texture.GetSelectedItem();
	if( ( hSelItem == NULL )||( m_tree_Texture.GetItemData( hSelItem ) == TT_ROOT ) )
	{
		GET_MV_NODE->_SetDetail_Map1( NULL );
		m_rdo_DMap1.SetWindowText( L"" );
		m_hDMap1Item = NULL;
		return;
	}

	assert_cs( m_tree_Texture.GetItemData( hSelItem ) == TT_NODE );

	HTREEITEM hParent = m_tree_Texture.GetParentItem( hSelItem );
	CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );
	CString strSel = m_tree_Texture.GetItemText( hSelItem );

	m_rdo_DMap1.SetWindowText( strSel );

	// ==== Texture Setting
	TCHAR szTexPath[ MAX_PATH ] = {0, };
	_stprintf_s( szTexPath, MAX_PATH, L"%s\\%s", strParent.GetBuffer(), strSel.GetBuffer());
	GET_MV_NODE->_SetDetail_Map1( szTexPath );

	m_hDMap1Item = hSelItem;

	if( m_rdo_DMap1.GetCheck() == BST_CHECKED )
		m_nCheckMapIndex = 1;
}

void _DlgMTMapping::OnBnClickedButtonSetDmap2()
{
	HTREEITEM hSelItem = m_tree_Texture.GetSelectedItem();
	if( ( hSelItem == NULL )||( m_tree_Texture.GetItemData( hSelItem ) == TT_ROOT ) )
	{
		GET_MV_NODE->_SetDetail_Map2( NULL );
		m_rdo_DMap2.SetWindowText( L"" );
		m_hDMap2Item = NULL;
		return;
	}

	assert_cs( m_tree_Texture.GetItemData( hSelItem ) == TT_NODE );

	HTREEITEM hParent = m_tree_Texture.GetParentItem( hSelItem );
	CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );
	CString strSel = m_tree_Texture.GetItemText( hSelItem );

	m_rdo_DMap2.SetWindowText( strSel );

	// ==== Texture Setting
	TCHAR szTexPath[ MAX_PATH ] = {0, };
	_stprintf_s( szTexPath, MAX_PATH, L"%s\\%s", strParent.GetBuffer(), strSel.GetBuffer());
	GET_MV_NODE->_SetDetail_Map2( szTexPath );

	m_hDMap2Item = hSelItem;

	if( m_rdo_DMap2.GetCheck() == BST_CHECKED )
		m_nCheckMapIndex = 2;
}

void _DlgMTMapping::OnBnClickedButtonSetDmap3()
{
	HTREEITEM hSelItem = m_tree_Texture.GetSelectedItem();
	if( ( hSelItem == NULL )||( m_tree_Texture.GetItemData( hSelItem ) == TT_ROOT ) )
	{
		GET_MV_NODE->_SetDetail_Map3( NULL );
		m_rdo_DMap3.SetWindowText( L"" );
		m_hDMap3Item = NULL;
		return;
	}

	assert_cs( m_tree_Texture.GetItemData( hSelItem ) == TT_NODE );

	HTREEITEM hParent = m_tree_Texture.GetParentItem( hSelItem );
	CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );
	CString strSel = m_tree_Texture.GetItemText( hSelItem );

	m_rdo_DMap3.SetWindowText( strSel );

	// ==== Texture Setting
	TCHAR szTexPath[ MAX_PATH ] = {0, };
	_stprintf_s( szTexPath, MAX_PATH, L"%s\\%s", strParent.GetBuffer(), strSel.GetBuffer());
	GET_MV_NODE->_SetDetail_Map3( szTexPath );

	m_hDMap3Item = hSelItem;

	if( m_rdo_DMap3.GetCheck() == BST_CHECKED )
		m_nCheckMapIndex = 3;
}

void _DlgMTMapping::OnBnClickedButtonSetDmap4()
{
	HTREEITEM hSelItem = m_tree_Texture.GetSelectedItem();
	if( ( hSelItem == NULL )||( m_tree_Texture.GetItemData( hSelItem ) == TT_ROOT ) )
	{
		GET_MV_NODE->_SetDetail_Map4( NULL );
		m_rdo_DMap4.SetWindowText( L"" );
		m_hDMap4Item = NULL;
		return;
	}

	assert_cs( m_tree_Texture.GetItemData( hSelItem ) == TT_NODE );

	HTREEITEM hParent = m_tree_Texture.GetParentItem( hSelItem );
	CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );
	CString strSel = m_tree_Texture.GetItemText( hSelItem );

	m_rdo_DMap4.SetWindowText( strSel );

	// ==== Texture Setting
	TCHAR szTexPath[ MAX_PATH ] = {0, };
	_stprintf_s( szTexPath, MAX_PATH, L"%s\\%s", strParent.GetBuffer(), strSel.GetBuffer());
	GET_MV_NODE->_SetDetail_Map4( szTexPath );

	m_hDMap4Item = hSelItem;

	if( m_rdo_DMap3.GetCheck() == BST_CHECKED )
		m_nCheckMapIndex = 4;
}


void _DlgMTMapping::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if( pScrollBar->GetSafeHwnd() == m_sld_Size.GetSafeHwnd() )
	{
		_Float2Control( &m_edit_AlphaSize, m_sld_Size.GetPos()*0.1f );

		if( ( m_nCheckMapIndex == 6 )||( m_nCheckMapIndex == 5 ) )
			g_Resist.GetMapping()->s_nGraySize = m_sld_Size.GetPos();
		else
			g_Resist.GetMapping()->s_nMappingSize = m_sld_Size.GetPos();

		if( m_sld_Decrement.GetPos() < m_sld_Size.GetPos() )
		{
			m_sld_Decrement.SetPos( m_sld_Size.GetPos() );

			if( ( m_nCheckMapIndex == 6 )||( m_nCheckMapIndex == 5 ) )
				g_Resist.GetMapping()->s_nGrayDecrement = m_sld_Decrement.GetPos();
			else
				g_Resist.GetMapping()->s_nMappingDecrement = m_sld_Decrement.GetPos();
			
			_Float2Control( &m_edit_AlphaDecrement, m_sld_Decrement.GetPos()*0.1f );
		}
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_Decrement.GetSafeHwnd() )
	{
		_Float2Control( &m_edit_AlphaDecrement, m_sld_Decrement.GetPos()*0.1f );

		if( ( m_nCheckMapIndex == 6 )||( m_nCheckMapIndex == 5 ) )
			g_Resist.GetMapping()->s_nGrayDecrement = m_sld_Decrement.GetPos();
		else
			g_Resist.GetMapping()->s_nMappingDecrement = m_sld_Decrement.GetPos();
		
		if( m_sld_Decrement.GetPos() < m_sld_Size.GetPos() )
		{
			m_sld_Size.SetPos( m_sld_Decrement.GetPos() );

			if( ( m_nCheckMapIndex == 6 )||( m_nCheckMapIndex == 5 ) )
				g_Resist.GetMapping()->s_nGraySize = m_sld_Size.GetPos();
			else
				g_Resist.GetMapping()->s_nMappingSize = m_sld_Size.GetPos();
			
			_Float2Control( &m_edit_AlphaSize, m_sld_Size.GetPos()*0.1f );
		}
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_MapSize.GetSafeHwnd() )
	{
		_Float2Control( &m_edit_MapSize, m_sld_MapSize.GetPos()*0.001f, 3 );
		nsCsGBTerrain::g_pCurRoot->ChangeMapUVSize( m_sld_MapSize.GetPos()*0.001f );
	}
	else if( pScrollBar->GetSafeHwnd() == m_sld_OP.GetSafeHwnd() )
	{
		_Float2Control( &m_edit_OP, m_sld_OP.GetPos()*0.01f, 2 );
		m_fOpercity = m_sld_OP.GetPos()*0.01f;
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}


void _DlgMTMapping::OnBnClickedRadioDmap0()
{
	m_rdo_DMap1.SetCheck( BST_UNCHECKED );
	m_rdo_DMap2.SetCheck( BST_UNCHECKED );
	m_rdo_DMap3.SetCheck( BST_UNCHECKED );
	m_rdo_DMap4.SetCheck( BST_UNCHECKED );
	m_rdo_Gray.SetCheck( BST_UNCHECKED );
	m_rdo_White.SetCheck( BST_UNCHECKED );

	if( m_hDMap0Item == NULL )
	{
		m_nCheckMapIndex = -1;
		g_Preview.GetTexture()->ReleaseModelView();
		return;
	}

	m_nCheckMapIndex = DETAIL_MAP_0_SHADER_NTM;
	m_tree_Texture.SelectItem( NULL );

	TCHAR sz[ MAX_FILENAME ];
	m_rdo_DMap0.GetWindowText( sz, MAX_FILENAME );
	if( ( _tcscmp( sz, L"..." ) != 0 )&&( sz[ 0 ] != NULL ) )
	{
		HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap0Item );
		CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );

		g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
	}
	else
	{
		g_Preview.GetTexture()->ReleaseModelView();
	}

	__ReadMapSize( true );
}

void _DlgMTMapping::OnBnClickedRadioDmap1()
{
	m_rdo_DMap0.SetCheck( BST_UNCHECKED );
	m_rdo_DMap2.SetCheck( BST_UNCHECKED );
	m_rdo_DMap3.SetCheck( BST_UNCHECKED );
	m_rdo_DMap4.SetCheck( BST_UNCHECKED );
	m_rdo_Gray.SetCheck( BST_UNCHECKED );
	m_rdo_White.SetCheck( BST_UNCHECKED );

	if( m_hDMap1Item == NULL )
	{
		m_nCheckMapIndex = -1;
		g_Preview.GetTexture()->ReleaseModelView();
		return;
	}

	m_nCheckMapIndex = 1;
	m_tree_Texture.SelectItem( NULL );

	TCHAR sz[ MAX_FILENAME ];
	m_rdo_DMap1.GetWindowText( sz, MAX_FILENAME );
	if( ( _tcscmp( sz, L"..." ) != 0 )&&( sz[ 0 ] != NULL ) )
	{
		HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap1Item );
		CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );

		g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
	}
	else
	{
		g_Preview.GetTexture()->ReleaseModelView();
	}

	__ReadMapSize( true );
}

void _DlgMTMapping::OnBnClickedRadioDmap2()
{
	m_rdo_DMap0.SetCheck( BST_UNCHECKED );
	m_rdo_DMap1.SetCheck( BST_UNCHECKED );
	m_rdo_DMap3.SetCheck( BST_UNCHECKED );
	m_rdo_DMap4.SetCheck( BST_UNCHECKED );
	m_rdo_Gray.SetCheck( BST_UNCHECKED );
	m_rdo_White.SetCheck( BST_UNCHECKED );

	if( m_hDMap2Item == NULL )
	{
		m_nCheckMapIndex = -1;
		g_Preview.GetTexture()->ReleaseModelView();
		return;
	}

	m_nCheckMapIndex = 2;
	m_tree_Texture.SelectItem( NULL );

	TCHAR sz[ MAX_FILENAME ];
	m_rdo_DMap2.GetWindowText( sz, MAX_FILENAME );
	if( ( _tcscmp( sz, L"..." ) != 0 )&&( sz[ 0 ] != NULL ) )
	{
		HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap2Item );
		CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );

		g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
	}
	else
	{
		g_Preview.GetTexture()->ReleaseModelView();
	}

	__ReadMapSize( true );
}

void _DlgMTMapping::OnBnClickedRadioDmap3()
{
	m_rdo_DMap0.SetCheck( BST_UNCHECKED );
	m_rdo_DMap1.SetCheck( BST_UNCHECKED );
	m_rdo_DMap2.SetCheck( BST_UNCHECKED );
	m_rdo_DMap4.SetCheck( BST_UNCHECKED );
	m_rdo_Gray.SetCheck( BST_UNCHECKED );
	m_rdo_White.SetCheck( BST_UNCHECKED );

	if( m_hDMap3Item == NULL )
	{
		m_nCheckMapIndex = -1;
		g_Preview.GetTexture()->ReleaseModelView();
		return;
	}

	m_nCheckMapIndex = 3;
	m_tree_Texture.SelectItem( NULL );

	TCHAR sz[ MAX_FILENAME ];
	m_rdo_DMap3.GetWindowText( sz, MAX_FILENAME );
	if( ( _tcscmp( sz, L"..." ) != 0 )&&( sz[ 0 ] != NULL ) )
	{
		HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap3Item );
		CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );		

		g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
	}
	else
	{
		g_Preview.GetTexture()->ReleaseModelView();
	}

	__ReadMapSize( true );
}


void _DlgMTMapping::OnBnClickedRadioDmap4()
{
	m_rdo_DMap0.SetCheck( BST_UNCHECKED );
	m_rdo_DMap1.SetCheck( BST_UNCHECKED );
	m_rdo_DMap2.SetCheck( BST_UNCHECKED );
	m_rdo_DMap3.SetCheck( BST_UNCHECKED );
	m_rdo_Gray.SetCheck( BST_UNCHECKED );
	m_rdo_White.SetCheck( BST_UNCHECKED );

	if( m_hDMap4Item == NULL )
	{
		m_nCheckMapIndex = -1;
		g_Preview.GetTexture()->ReleaseModelView();
		return;
	}

	m_nCheckMapIndex = 4;
	m_tree_Texture.SelectItem( NULL );

	TCHAR sz[ MAX_FILENAME ];
	m_rdo_DMap4.GetWindowText( sz, MAX_FILENAME );
	if( ( _tcscmp( sz, L"..." ) != 0 )&&( sz[ 0 ] != NULL ) )
	{
		HTREEITEM hParent = m_tree_Texture.GetParentItem( m_hDMap4Item );
		CString strParent = L"Data\\" + m_tree_Texture.GetItemText( hParent );		

		g_Preview.GetTexture()->SetModelView( strParent.GetBuffer(), sz );
	}
	else
	{
		g_Preview.GetTexture()->ReleaseModelView();
	}

	__ReadMapSize( true );
}

void _DlgMTMapping::OnBnClickedRadioGray()
{
	m_rdo_DMap0.SetCheck( BST_UNCHECKED );
	m_rdo_DMap1.SetCheck( BST_UNCHECKED );
	m_rdo_DMap2.SetCheck( BST_UNCHECKED );
	m_rdo_DMap3.SetCheck( BST_UNCHECKED );
	m_rdo_White.SetCheck( BST_UNCHECKED );

	m_tree_Texture.SelectItem( NULL );
	m_nCheckMapIndex = 5;

	g_Preview.GetTexture()->ReleaseModelView();

	__ReadMapSize( false );
}

void _DlgMTMapping::OnBnClickedRadioWhite()
{
	m_rdo_DMap0.SetCheck( BST_UNCHECKED );
	m_rdo_DMap1.SetCheck( BST_UNCHECKED );
	m_rdo_DMap2.SetCheck( BST_UNCHECKED );
	m_rdo_DMap3.SetCheck( BST_UNCHECKED );
	m_rdo_Gray.SetCheck( BST_UNCHECKED );

	m_tree_Texture.SelectItem( NULL );
	m_nCheckMapIndex = 6;

	g_Preview.GetTexture()->ReleaseModelView();

	__ReadMapSize( false );
}


void _DlgMTMapping::OnCbnSelchangeComboDecreaseConstant()
{
	if( ( m_nCheckMapIndex == 6 )||( m_nCheckMapIndex == 5 ) )
		g_Resist.GetMapping()->s_fGrayDecreaseConstant = _Control2float( &m_combo_DecreaseConstant );
	else
		g_Resist.GetMapping()->s_fAlphaDecreaseConstant = _Control2float( &m_combo_DecreaseConstant );
}

void _DlgMTMapping::OnBnClickedButtonSwap12()
{
	GET_MV_NODE->_AlphaSwap12();
}

void _DlgMTMapping::OnBnClickedButtonSwap23()
{
	GET_MV_NODE->_AlphaSwap23();
}

void _DlgMTMapping::OnBnClickedButtonSwap13()
{
	GET_MV_NODE->_AlphaSwap13();
}

void _DlgMTMapping::OnStnClickedStaticGraycolor()
{
	NiColor orgColor = g_Resist.GetMapping()->s_GrayColor;
	CColorDialog colorDlg( RGB( CsF2B( orgColor.r ), CsF2B( orgColor.g ), CsF2B( orgColor.b ) ), CC_FULLOPEN | CC_SOLIDCOLOR );
	if( colorDlg.DoModal() == IDCANCEL )
		return;

	COLORREF color = colorDlg.GetColor();
	g_Resist.GetMapping()->s_GrayColor = NiColor( CsB2F( GetRValue( color ) ), CsB2F( GetGValue( color ) ), CsB2F( GetBValue( color ) ) );
	m_Gray.Create( &m_static_GrayColor, g_Resist.GetMapping()->s_GrayColor );
}

HBRUSH _DlgMTMapping::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = _DlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

	switch( pWnd->GetDlgCtrlID() )
	{
	case IDC_STATIC_GRAYCOLOR:		hbr = m_Gray.s_Brush;			break;
	}

	return hbr;
}

void _DlgMTMapping::OnCbnSelchangeComboMppg()
{
	int nMPPG = (int)m_combo_MPPG.GetItemData( m_combo_MPPG.GetCurSel() );
	if( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nAlphaMPPG == nMPPG )
		return;

	int nLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	for( int i=0; i<nLeafCount; ++i )
	{
		int nNewMapSize = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nInitMapSize * nMPPG;
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );

		NiTexturingProperty* pTexProp = GET_TEXTUREPROPERTY( pLeaf->GetNiNode() );

		//==================================================================================================================
		//	알파 맵
		//==================================================================================================================
		{
			NiPixelData* pPixelData = GET_SHADERMAP_PIXELDATA( pLeaf->GetNiNode(), ALPHA_MAP_SHADER_NTM );
			CsImageInterpolation ip( pPixelData->GetPixels(), pPixelData->GetWidth(), pPixelData->GetHeight(), pPixelData->GetPixelStride() );
			BYTE* pNewData = ip.Liner( nNewMapSize, nNewMapSize );
			NiPixelData* pNewPixelData = NiNew NiPixelData( nNewMapSize, nNewMapSize, ALPHA_MAP_FORMAT );
			BYTE* pNewPixel = (BYTE*)pNewPixelData->GetPixels();
			memcpy( pNewPixel, pNewData, sizeof( BYTE )*nNewMapSize*nNewMapSize*pPixelData->GetPixelStride() );

			NiSourceTexture* pNewTexture = NiSourceTexture::Create( pNewPixelData );
			pNewTexture->SetStatic( false );
			NiTexturingProperty::ShaderMap* pNewMap = NiNew NiTexturingProperty::ShaderMap( pNewTexture, 0, 
				NiTexturingProperty::WRAP_S_WRAP_T,
				nsCsGBTerrain::g_eTexFilter );
			pTexProp->SetShaderMap( ALPHA_MAP_SHADER_NTM, pNewMap );
		}		

		//==================================================================================================================
		//	이펙트 맵
		//==================================================================================================================
		{
			NiPixelData* pPixelData = GET_SHADERMAP_PIXELDATA( pLeaf->GetNiNode(), EFFECT_MAP_SHADER_NTM );
			CsImageInterpolation ip( pPixelData->GetPixels(), pPixelData->GetWidth(), pPixelData->GetHeight(), pPixelData->GetPixelStride() );
			BYTE* pNewData = ip.Liner( nNewMapSize, nNewMapSize );
			NiPixelData* pNewPixelData = NiNew NiPixelData( nNewMapSize, nNewMapSize, EFFECT_MAP_FORMAT );
			BYTE* pNewPixel = (BYTE*)pNewPixelData->GetPixels();
			memcpy( pNewPixel, pNewData, sizeof( BYTE )*nNewMapSize*nNewMapSize*pPixelData->GetPixelStride() );

			NiSourceTexture* pNewTexture = NiSourceTexture::Create( pNewPixelData );
			pNewTexture->SetStatic( false );
			NiTexturingProperty::ShaderMap* pNewMap = NiNew NiTexturingProperty::ShaderMap( pNewTexture, 0, 
														NiTexturingProperty::WRAP_S_WRAP_T,
														nsCsGBTerrain::g_eTexFilter );
			pTexProp->SetShaderMap( EFFECT_MAP_SHADER_NTM, pNewMap );
		}
	}

	// ===== 다 만들었으면 경계선 복사 하자
	NiPixelData** ppPixelData = csnew NiPixelData*[ nLeafCount ];
	for( int i=0; i<nLeafCount; ++i )
		ppPixelData[ i ] = GET_SHADERMAP_PIXELDATA( nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetNiNode(), ALPHA_MAP_SHADER_NTM );
	nsCsGBTerrain::g_pCurRoot->GetInfo()->CopyTextureBorder( nMPPG, ppPixelData );
	for( int i=0; i<nLeafCount; ++i )
		ppPixelData[ i ] = GET_SHADERMAP_PIXELDATA( nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetNiNode(), EFFECT_MAP_SHADER_NTM );
	nsCsGBTerrain::g_pCurRoot->GetInfo()->CopyTextureBorder( nMPPG, ppPixelData );
	SAFE_DELETE_ARRAY( ppPixelData );

	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nAlphaMPPG = nMPPG;
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nEffectMPPG = nMPPG;

	g_RollBack.Delete();
	g_RollBack.Init();
}

void _DlgMTMapping::OnCbnSelchangeComboBasemppg()
{
	nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nBaseMPPG = (int)m_cmb_BaseMPPG.GetItemData( m_cmb_BaseMPPG.GetCurSel() );
}


void _DlgMTMapping::OnBnClickedButtonGrayClear()
{
	int nLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	for( int i=0; i<nLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		if( pLeaf->IsVisible() == false )
			continue;

		NiPixelData* pPixelData = GET_SHADERMAP_PIXELDATA( pLeaf->GetNiNode(), EFFECT_MAP_SHADER_NTM );
		memset( pPixelData->GetPixels(), 0xff, sizeof( BYTE )*pPixelData->GetWidth()*pPixelData->GetHeight()*pPixelData->GetPixelStride() );
		pPixelData->MarkAsChanged();
	}
}

void _DlgMTMapping::MoveCallBack_Gray( float fDelta )
{
	int nLeafCount = nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nTotalLeafCount;
	for( int i=0; i<nLeafCount; ++i )
	{
		CsGBTerrainLeaf* pLeaf = nsCsGBTerrain::g_pCurRoot->GetNode( i );
		if( pLeaf->IsVisible() == false )
			continue;

		NiPixelData* pPixelData = GET_SHADERMAP_PIXELDATA( pLeaf->GetNiNode(), EFFECT_MAP_SHADER_NTM );
		BYTE* pData = (BYTE*)pPixelData->GetPixels();
		int nWidth = pPixelData->GetWidth();
		int nHeight = pPixelData->GetHeight();
		int nStride = pPixelData->GetPixelStride();

		int nIndex;
		int nColor;
		for( int y=0; y<nHeight; ++y )
		{
			for( int x=0; x<nWidth; ++x )
			{
				for( int stride=0; stride<3; ++stride )
				{
					nIndex = ( y*nWidth + x )*nStride + stride;

					if( pData[ nIndex ] == 0xff )
						continue;

					nColor = (int)pData[ nIndex ] + (int)fDelta;
					nColor = CsMax( 0, nColor );
					nColor = CsMin( 0xfe, nColor );
					pData[ nIndex ] = (BYTE)nColor;
				}				
			}
		}
	}

	// ===== 다 만들었으면 경계선 복사 하자
	NiPixelData** ppPixelData = csnew NiPixelData*[ nLeafCount ];
	for( int i=0; i<nLeafCount; ++i )
		ppPixelData[ i ] = GET_SHADERMAP_PIXELDATA( nsCsGBTerrain::g_pCurRoot->GetNode( i )->GetNiNode(), EFFECT_MAP_SHADER_NTM );
	nsCsGBTerrain::g_pCurRoot->GetInfo()->CopyTextureBorder( nsCsGBTerrain::g_pCurRoot->GetInfo()->s_nEffectMPPG, ppPixelData );
	SAFE_DELETE_ARRAY( ppPixelData );
}

void _DlgMTMapping::_OnMouseMove( CPoint pt )
{
	m_ptMousePoint = pt;
}

void _DlgMTMapping::_PostRenderView()
{
	NiPick::Record* pRecord = nsCsGBTerrain::g_pCurRoot->Pick_T_TB_TBP_ByTool( CAMERA_ST.GetCameraObj(), m_ptMousePoint.x, m_ptMousePoint.y );		
	if( pRecord == NULL )
		return;

	NiPoint2 pt = NiPoint2( pRecord->GetIntersection().x, pRecord->GetIntersection().y );

	float fSize = _Control2float(&m_edit_AlphaDecrement) * 50.0f;
	g_ToolNiNodeMng.GetRegion()->Render(  pt, fSize, NiColor( 0, 1, 0 )	);
}
