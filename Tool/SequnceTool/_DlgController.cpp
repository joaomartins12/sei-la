// _DlgController.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "SequnceTool.h"
#include "_DlgController.h"
#include "_Dlg_Copy_Digimon.h"

// _DlgController 대화 상자입니다.

IMPLEMENT_DYNCREATE(_DlgController, CFormView)

_DlgController::_DlgController()
	: CFormView( _DlgController::IDD )
{

}

_DlgController::~_DlgController()
{
}

void _DlgController::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MODEL, m_list_Model);
	DDX_Control(pDX, IDC_EDIT_SCALE, m_edit_Scale);
	DDX_Control(pDX, IDC_SPIN_SCALE, m_spin_Scale);
	DDX_Control(pDX, IDC_EDIT_HEIGHT, m_edit_Height);
	DDX_Control(pDX, IDC_SPIN_HEIGHT, m_spin_Height);
	DDX_Control(pDX, IDC_EDIT_WIDTH, m_edit_Width);
	DDX_Control(pDX, IDC_SPIN_WIDTH, m_spin_Width);
	DDX_Control(pDX, IDC_CHECK_HIDE_ABV, m_chk_HideABV);
	DDX_Control(pDX, IDC_CHECK_HIDE_CLIENT, m_chk_ClientBox);
	DDX_Control(pDX, IDC_CHECK_VIEW_DEST, m_chk_DestView);
	DDX_Control(pDX, IDC_EDIT_DEST_DIST, m_edit_DestDist);
	DDX_Control(pDX, IDC_SPIN_DEST_DIST, m_spin_DestDist);
	DDX_Control(pDX, IDC_COMBO_DEST_ANI, m_combo_DestAni);
	DDX_Control(pDX, IDC_EDIT_CHAR_HEIGHT, m_edit_CharHeight);
	DDX_Control(pDX, IDC_EDIT_FIND, m_edit_Find);
}


BEGIN_MESSAGE_MAP(_DlgController, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_RESETMODEL, &_DlgController::OnBnClickedButtonResetmodel)
	ON_NOTIFY(NM_CLICK, IDC_LIST_MODEL, &_DlgController::OnNMClickListModel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &_DlgController::OnBnClickedButtonSave)
	ON_EN_CHANGE(IDC_EDIT_SCALE, &_DlgController::OnEnChangeEditScale)
	ON_EN_CHANGE(IDC_EDIT_HEIGHT, &_DlgController::OnEnChangeEditHeight)
	ON_EN_CHANGE(IDC_EDIT_WIDTH, &_DlgController::OnEnChangeEditWidth)	
	ON_BN_CLICKED(IDC_CHECK_HIDE_CLIENT, &_DlgController::OnBnClickedCheckHideClient)
	ON_BN_CLICKED(IDC_CHECK_HIDE_ABV, &_DlgController::OnBnClickedCheckHideAbv)
	ON_BN_CLICKED(IDC_BUTTON_SRC_ATTDIST, &_DlgController::OnBnClickedButtonSrcAttdist)
	ON_BN_CLICKED(IDC_BUTTON_SRC_ATTDIST2, &_DlgController::OnBnClickedButtonSrcAttdist2)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_MODEL, &_DlgController::OnLvnKeydownListModel)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_MODEL, &_DlgController::OnLvnItemchangedListModel)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_EXCEL, &_DlgController::OnBnClickedButtonSaveExcel)	
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_MODEL, &_DlgController::OnLvnColumnclickListModel)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_MODEL, &_DlgController::OnNMRclickListModel)
	ON_BN_CLICKED(IDC_BUTTON_FOLDER, &_DlgController::OnBnClickedButtonFolder)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_ANI, &_DlgController::OnBnClickedButtonCheckAni)
	ON_BN_CLICKED(IDC_BUTTON_CHECK_EVENT, &_DlgController::OnBnClickedButtonCheckEvent)
	ON_BN_CLICKED(IDC_BUTTON_BG, &_DlgController::OnBnClickedButtonBG)
	ON_BN_CLICKED(IDC_BUTTON_FIND, &_DlgController::OnBnClickedButtonFind)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_HEIGHTEXCEL, &_DlgController::OnBnClickedButtonSaveHeightexcel)
	ON_BN_CLICKED(IDC_BUTTON_SOUND_FOLDER, &_DlgController::OnBnClickedButtonSoundFolder)
	ON_BN_CLICKED(IDC_BUTTON_COPY_DIGIMON, &_DlgController::OnBnClickedButtonCopyDigimon)
END_MESSAGE_MAP()


void _DlgController::_Init()
{
	DWORD dwExStyle = m_list_Model.GetExtendedStyle();
	dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES;
	m_list_Model.SetExtendedStyle( dwExStyle );

	m_list_Model.InsertColumn( 0, L"Type", LVCFMT_RIGHT, 60 );
	m_list_Model.InsertColumn( 1, L"Type ID", LVCFMT_RIGHT, 60 );
	m_list_Model.InsertColumn( 2, L"ModelID", LVCFMT_RIGHT, 60 );
	m_list_Model.InsertColumn( 3, L"이름", LVCFMT_LEFT, 125 );
	m_list_Model.InsertColumn( 4, L"Kfm", LVCFMT_LEFT, 125 );

	m_spin_Scale.SetCsBuddy( &m_edit_Scale, 0.002f, 3 );
	nsCsMfcFunc::Float2Control( &m_edit_Scale, 1.0f, 3 );

	m_spin_Width.SetCsBuddy( &m_edit_Width, 0.01f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_Width, 1.0f, 2 );

	m_spin_Height.SetCsBuddy( &m_edit_Height, 0.01f, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_Height, 1.0f, 2 );

	m_spin_DestDist.SetCsBuddy( &m_edit_DestDist, 1 );
	nsCsMfcFunc::Int2Control( &m_edit_DestDist, 200 );

	std::map< DWORD, CsModelData* >::iterator it = g_pModelDataMng->GetDataMap()->begin();
	std::map< DWORD, CsModelData* >::iterator itEnd = g_pModelDataMng->GetDataMap()->end();
	std::list< DWORD > listDelete;
	for( ; it!=itEnd; ++it )
	{
		if( __InsertListControl( it->second ) == false )
		{
			listDelete.push_back( it->second->GetInfo()->s_dwID );
		}
	}

	m_chk_HideABV.SetCheck( BST_CHECKED );
	m_chk_ClientBox.SetCheck( BST_CHECKED );

	m_combo_DestAni.InsertString( 0, L"기본" );
	m_combo_DestAni.SetItemData( 0, ANI::IDLE_NORMAL );
	m_combo_DestAni.InsertString( 1, L"공격1" );
	m_combo_DestAni.SetItemData( 1, ANI::ATTACK_ATTACK1 );
	m_combo_DestAni.InsertString( 2, L"공격2" );
	m_combo_DestAni.SetItemData( 2, ANI::ATTACK_ATTACK2 );
	m_combo_DestAni.InsertString( 3, L"데미지1" );
	m_combo_DestAni.SetItemData( 3, ANI::DAMAGE_DAMAGE1 );
	m_combo_DestAni.InsertString( 4, L"데미지2" );
	m_combo_DestAni.SetItemData( 4, ANI::DAMAGE_DAMAGE2 );
	m_combo_DestAni.SetCurSel( 0 );

	m_pSelectModel = NULL;

	m_DlgPart.Create( _DlgPartObject::IDD, this );
	m_DlgPart.ShowWindow( SW_HIDE );

	std::list< DWORD >::iterator itDel = listDelete.begin();
	std::list< DWORD >::iterator itDelEnd = listDelete.end();
	for( ; itDel!=itDelEnd; ++itDel )
	{
		g_pModelDataMng->DeleteData( *itDel );
	}	
}

#include "shlwapi.h"

void _DlgController::OnBnClickedButtonFind()
{
	TCHAR szEdit[ 128 ];
	m_edit_Find.GetWindowText( szEdit, 128 );

	TCHAR sz[ 512 ];
	int nCount = m_list_Model.GetItemCount();

	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nSelect = m_list_Model.GetNextSelectedItem( pos );
	if( nSelect == nCount - 1 )
		nSelect = -1;

	for( int i=nSelect+1; i<nCount; ++i )
	{
		for( int nSub=0; nSub<5; ++nSub )
		{
			m_list_Model.GetItemText( i, nSub, sz, 512 );
			TCHAR* pFind = StrStrIW( sz, szEdit );
			if( pFind != NULL )
			{
				m_list_Model.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
				m_list_Model.EnsureVisible( i, FALSE );

				LONG result;
				OnNMClickListModel( NULL, &result );
				return;
			}
		}		
	}

	for( int i=0; i<nSelect+1; ++i )
	{
		for( int nSub=0; nSub<5; ++nSub )
		{
			m_list_Model.GetItemText( i, nSub, sz, 512 );
			TCHAR* pFind = StrStrIW( sz, szEdit );
			if( pFind != NULL )
			{
				m_list_Model.SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
				m_list_Model.EnsureVisible( i, FALSE );

				LONG result;
				OnNMClickListModel( NULL, &result );
				return;
			}
		}		
	}

	CsMessageBox( MB_OK, L"해당 검색으로 찾을 수 없습니다." );
}

void _DlgController::OnBnClickedButtonResetmodel()
{
	m_list_Model.DeleteAllItems();
	__Search( L"Data\\*.*" );
}

void _DlgController::__InsertList( LPTSTR path )
{
	TCHAR* szName = (TCHAR*)nsCSFILE::GetFileName( path );

	int nIndex = m_list_Model.GetItemCount();
	int nID = _ttoi( szName );
	if( nID == 0 )
		return;

	nID %= 100000;

	// 동일한 ID를 가진 리스트가 존재 하면 안된다.
	for( int i=0; i<nIndex; ++i )
	{
		if( nID == m_list_Model.GetItemData( i ) )
		{
			CsMessageBox( MB_OK, L"ID = %d 가 중복 되므로 건너 뛰겠습니다.", nID );
			return;
		}
	}

	// 없다면 새로운 데이터 생성
	if( g_pModelDataMng->IsData( nID ) == false )
	{
		CsModelData::sINFO info;
		info.Init();
		info.s_dwID = nID;
		W2M( info.s_cKfmPath, path, OBJECT_PATH_LEN );
		g_pModelDataMng->InsertData( &info );		
	}
	// 이미 있다면 기존값과 동일 한지 비교
	else
	{
		CsModelData* pMD = g_pModelDataMng->GetData( nID );

		char cPath[ OBJECT_PATH_LEN ];
		W2M( cPath, path, OBJECT_PATH_LEN );
		// 기존 값과 같지 않다면
		if( _stricmp( pMD->GetInfo()->s_cKfmPath, cPath ) != 0 )
		{
			switch( CsMessageBoxA( MB_YESNOCANCEL, "동일 한 ID 를 발견 하였지만 KFM 경로가 예전과 다릅니다.\n\nYes = 경로만교체\nNo = 지우고 새로생성\nCancel = 지울지 건너 띄울지 선택\n\nID=%d \nInsertKfmPath = %s \nExistKfmPath = %s", nID, cPath, g_pModelDataMng->GetData( nID )->GetInfo()->s_cKfmPath ) )
			{
			case IDYES:
				strcpy_s( pMD->GetInfo()->s_cKfmPath, OBJECT_PATH_LEN, cPath );
				break;
			case IDNO:
				{
					g_pModelDataMng->DeleteData( nID );
					CsModelData::sINFO info;
					info.Init();
					info.s_dwID = nID;
					W2M( info.s_cKfmPath, path, OBJECT_PATH_LEN );
					g_pModelDataMng->InsertData( &info );
				}
				break;
			case IDCANCEL:
				{
					switch( CsMessageBoxA( MB_OKCANCEL, "해당데이터를 지우시겠습니까?\n\nOk = 지운다.\nCancel = 데이터는 남겨둔체로 읽지 않는다.\n\nID=%d \nInsertKfmPath = %s \nExistKfmPath = %s", nID, cPath, g_pModelDataMng->GetData( nID )->GetInfo()->s_cKfmPath ) )
					{
					case IDOK:
						g_pModelDataMng->DeleteData( nID );
						break;
					case IDCANCEL:
						break;
					}
				}
				return;
			}
		}
	}

	TCHAR szID[ 32 ];
	_stprintf_s( szID, 32, L"%d", nID );	
	size_t nLen = _tcslen( szName );
	for( size_t i=0; i<nLen; ++i )
	{
		if( szName[ i ] == '_' )
		{
			szName = &szName[ i+1 ];
			break;
		}
	}

	for( int i=0; i<nIndex; ++i )
	{
		if( nID == (int)m_list_Model.GetItemData( i ) )
		{
			assert_cs( false );
			return;
		}
	}

	__InsertListControl( g_pModelDataMng->GetData( nID ) );
}

//=========================================================
//
// 조건별로 하위 디렉토리 까지 검색 
//
//=========================================================
void _DlgController::__Search( LPTSTR path )
{
	int nLen;
	WIN32_FIND_DATA wfd;
	HANDLE hSrch = FindFirstFile( path, &wfd );
	TCHAR dir[ MAX_PATH ] = {0, };

	while( hSrch != INVALID_HANDLE_VALUE  )
	{
		// 디렉토리가 아니라면
		if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FALSE )
		{
			// kfm 파일이라면
			nLen = (int)_tcslen( wfd.cFileName ) - 3;
			if( _tcsicmp( &wfd.cFileName[ nLen ], L"kfm" ) == 0 )
			{
				nLen = (int)_tcslen( path ) - 3;
				_tcsncpy_s( dir, MAX_PATH, path, nLen );
				_tcscat_s( dir, MAX_PATH, wfd.cFileName );
				__InsertList( dir );				
			}
		}
		// 디렉토리라면 재귀호출
		else
		{
			if( ( _tcsicmp( wfd.cFileName, L"." ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L".." ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"Cursor" ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"Effect" ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"EtcObject" ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"Font" ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"Interface" ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"LUA" ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"Map" ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"Music" ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"Shaders" ) != 0 )&&
				( _tcsicmp( wfd.cFileName, L"Sound" ) != 0 )	)
			{				
				nLen = (int)_tcslen( path ) - 3;
				_tcsncpy_s( dir, MAX_PATH, path, nLen );
				_tcscat_s( dir, MAX_PATH, wfd.cFileName );
				_tcscat_s( dir, MAX_PATH, L"\\*.*" );
				__Search( dir );
			}			
		}
		if( FindNextFile( hSrch, &wfd ) == false )
		{
			FindClose( hSrch );
			hSrch = INVALID_HANDLE_VALUE;
		}
	}
	FindClose( hSrch );
}

void _DlgController::__CancelSelect()
{
	// 리스트 선택 풀어 준다
	m_list_Model.SetSelectionMark( -1 );
	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nSelectItem = m_list_Model.GetNextSelectedItem( pos );
	if( nSelectItem >= 0 )
	{
		m_list_Model.SetItemState( nSelectItem, 0, LVIS_SELECTED );
	}
	// 포인터 초기화
	m_pSelectModel = NULL;
	// 모델뷰어에서 모델 삭제
	g_ViewModel.DeleteSrcModel();


	g_pMainWnd->_GetDlgAnimation()->_ReleaseTarget();
}

void _DlgController::OnNMClickListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_DlgPart.ShowWindow( SW_HIDE );

	m_list_Model.SetFocus();

	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nSelect = m_list_Model.GetNextSelectedItem( pos );
	if( nSelect < 0 )
	{
		__CancelSelect();
		return;
	}

	int nID = (int)m_list_Model.GetItemData( nSelect );

	// 파일이 존재하는 모델인지 확인
	CsModelData* pModelData = g_pModelDataMng->GetData( nID );
	if( _access_s( pModelData->GetInfo()->s_cKfmPath, 0 ) != 0 )
	{
		CsMessageBoxA( MB_OK, "%s\n파일이 존재 하지 않아 제거합니다.", pModelData->GetInfo()->s_cKfmPath );
		g_pModelDataMng->DeleteData( nID );
		m_list_Model.DeleteAllItems();
		__Search( L"Data\\*.*" );
		return;
	}

	CsC_AvObject* pModel = g_ViewModel.SetSrcModel( nID, ( m_chk_HideABV.GetCheck() == BST_UNCHECKED ) );
	g_pMainWnd->_GetDlgAnimation()->_ResetTarget( pModel );

	m_pSelectModel = g_pModelDataMng->GetData( nID );
	nsCsMfcFunc::Float2Control( &m_edit_Scale, m_pSelectModel->GetInfo()->s_fScale, 3 );
	nsCsMfcFunc::Float2Control( &m_edit_Width, m_pSelectModel->GetInfo()->s_fWidth, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_Height, m_pSelectModel->GetInfo()->s_fHeight, 2 );
	nsCsMfcFunc::Float2Control( &m_edit_CharHeight, m_pSelectModel->GetInfo()->s_fScale*m_pSelectModel->GetInfo()->s_fHeight, 3 );

	if( nID/10000 == 8 )
	{
		//m_DlgPart.ShowWindow( SW_SHOW );

		char cPath[ MAX_PATH ] = {0, };
		nsCSFILE::GetFilePathOnly( cPath, MAX_PATH, m_pSelectModel->GetInfo()->s_cKfmPath, true );
		m_DlgPart._Init( cPath );
	}

	*pResult = 0;
}

void _DlgController::OnLvnItemchangedListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	/*if( m_DlgPart.GetSafeHwnd() != NULL )
	{		
		LRESULT result;
		OnNMClickListModel( NULL, &result );
	}*/	
	
	*pResult = 0;
}

void _DlgController::OnNMRclickListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_list_Model.SetFocus();

	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nSelect = m_list_Model.GetNextSelectedItem( pos );
	if( nSelect < 0 )
	{
		g_ViewModel.DeleteDestModel();
		return;
	}

	int nID = (int)m_list_Model.GetItemData( nSelect );
	g_ViewModel.SetDestModel( nID );
	m_chk_DestView.SetCheck( BST_CHECKED );

	*pResult = 0;
}

void _DlgController::OnLvnKeydownListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

	if( pLVKeyDow->wVKey == VK_UP )
	{
		POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
		int nSelect = m_list_Model.GetNextSelectedItem( pos );
		if( nSelect > 0 )
		{
			m_list_Model.SetItemState( nSelect-1, LVIS_SELECTED, LVIS_SELECTED );
			LRESULT result;
			OnNMClickListModel( NULL, &result );
		}
	}
	else if( pLVKeyDow->wVKey == VK_DOWN )
	{
		POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
		int nSelect = m_list_Model.GetNextSelectedItem( pos );
		if( nSelect < m_list_Model.GetItemCount()-1 )
		{
			m_list_Model.SetItemState( nSelect+1, LVIS_SELECTED, LVIS_SELECTED );
			LRESULT result;
			OnNMClickListModel( NULL, &result );
		}
	}

	*pResult = 0;
}


void _DlgController::OnBnClickedButtonSave()
{
	g_pModelDataMng->Save();

	CsMessageBox( MB_OK, L"저장 완료" );
}

void _DlgController::OnBnClickedButtonSaveExcel()
{
	std::wstring loadFile;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:	loadFile = L".\\GameData\\Korea\\ModelScale.xls";		break;
	case nsCsFileTable::ENGLISH:
	case nsCsFileTable::ENGLISH_A:			loadFile = L".\\GameData\\English_A\\ModelScale.xls";	break;
	case nsCsFileTable::THAILAND:			loadFile = L".\\GameData\\Thailand\\ModelScale.xls";	break;
	case nsCsFileTable::TAIWAN:				loadFile = L".\\GameData\\Taiwan\\ModelScale.xls";		break;
	case nsCsFileTable::HONGKONG:			loadFile = L".\\GameData\\HongKong\\ModelScale.xls";	break;
	default:
		assert_cs( false );
	}

	// 이미 파일이 존재 한다면 백업 시키다
	if( _taccess_s( loadFile.c_str(), 0 ) == 0 )
	{
		// 폴더가 존재 하지 않는다면 폴더 생성
		nsCSFILE::CreateDirectory( L".\\GameData\\ModelScale_Backup" );

		TCHAR szNewFile[ MAX_PATH ];
		SYSTEMTIME st;
		GetLocalTime( &st );
		//st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond;
		_stprintf_s( szNewFile, MAX_PATH, L".\\GameData\\ModelScale_Backup\\ModelScale_%.2d%.2d%.2d_%.2d%.2d%.2d.xls", st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
		// 이미 파일이 존재 한다면
		if( _taccess_s( szNewFile, 0 ) == 0 )
			DeleteFile( szNewFile );
		_trename( loadFile.c_str(), szNewFile );
	}

	YExcel::BasicExcel excel;
	excel.New( 1 );
	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( (size_t)0 );
	pSheet->Rename( "Scale" );
	pSheet->Cell( 0, 0 )->SetWString( L"이름" );
	pSheet->Cell( 0, 1 )->SetString( "ModelID" );
	pSheet->Cell( 0, 2 )->SetString( "Scale" );
	pSheet->Cell( 0, 3 )->SetString( "Size" );
	pSheet->Cell( 0, 4 )->SetWString( L"원거리 이벤트" );	
	pSheet->Cell( 0, 5 )->SetString( "Skill_1_Event" );
	pSheet->Cell( 0, 6 )->SetString( "Skill_1_End" );
	pSheet->Cell( 0, 7 )->SetString( "Skill_2_Event" );
	pSheet->Cell( 0, 8 )->SetString( "Skill_2_End" );
	pSheet->Cell( 0, 9 )->SetString( "Skill_3_Event" );
	pSheet->Cell( 0, 10 )->SetString( "Skill_3_End" );
	pSheet->Cell( 0, 11 )->SetString( "Skill_4_Event" );
	pSheet->Cell( 0, 12 )->SetString( "Skill_4_End" );
	pSheet->Cell( 0, 13 )->SetString( "Skill_5_Event" );
	pSheet->Cell( 0, 14 )->SetString( "Skill_5_End" );

	int nRowIndex = 1;
	std::map< DWORD, CsModelData* >* pMap = g_pModelDataMng->GetDataMap();
	std::map< DWORD, CsModelData* >::iterator it =  pMap->begin();
	std::map< DWORD, CsModelData* >::iterator itEnd = pMap->end();
	for( ; it!=itEnd; ++it )
	{
		SAFE_POINTER_CON( it->second );

		CsModelData::sINFO* pInfo = it->second->GetInfo();
		SAFE_POINTER_CON( pInfo );

		bool bRange = false;
		if( nsCsFileTable::g_pDigimonMng->IsDigimon( pInfo->s_dwID ) == true )
		{
			CsDigimon* pDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( pInfo->s_dwID );
			if( pDigimon )
			{
				CsDigimon::sINFO * pDigimonInfo = pDigimon->GetInfo();
				if( pDigimonInfo )
				{
					pSheet->Cell( nRowIndex, 0 )->SetWString( pDigimonInfo->s_szName );
					bRange = (pDigimonInfo->s_nDigimonType == 2);
				}
				else
					pSheet->Cell( nRowIndex, 0 )->SetWString( L"디지몬FT에 정보가 없다" );
			}
			else
			{
				pSheet->Cell( nRowIndex, 0 )->SetWString( L"디지몬FT에 정보가 없다" );
			}
		}
		else if( nsCsFileTable::g_pNpcMng->IsNpc( pInfo->s_dwID ) )
		{
			CsNpc* pNpcTable = nsCsFileTable::g_pNpcMng->GetNpc( pInfo->s_dwID );
			if( pNpcTable )
			{
				CsNpc::sINFO* ptbInfo = pNpcTable->GetInfo();
				if( ptbInfo )
					pSheet->Cell( nRowIndex, 0 )->SetWString( ptbInfo->s_szName );
				else
					pSheet->Cell( nRowIndex, 0 )->SetWString( L"NPC FT에 정보가 없다" );
			}
			else
				pSheet->Cell( nRowIndex, 0 )->SetWString( L"NPC FT에 정보가 없다" );
		}
		else if( nsCsFileTable::g_pTamerMng->IsTamer( pInfo->s_dwID ) )
		{
			CsTamer* pTamerTable = nsCsFileTable::g_pTamerMng->GetTamer( pInfo->s_dwID );
			if( pTamerTable )
			{
				CsTamer::sINFO* pFTInfo = pTamerTable->GetInfo();
				if( pFTInfo )
					pSheet->Cell( nRowIndex, 0 )->SetWString( pFTInfo->s_szName );
				else
					pSheet->Cell( nRowIndex, 0 )->SetWString( L"테이머 FT에 정보가 없다" );
			}
			else
				pSheet->Cell( nRowIndex, 0 )->SetWString( L"테이머 FT에 정보가 없다" );
		}
		else
			pSheet->Cell( nRowIndex, 0 )->SetWString( L"FT에 정보가 없다" );

		
		pSheet->Cell( nRowIndex, 1 )->SetInteger( pInfo->s_dwID );
		pSheet->Cell( nRowIndex, 2 )->SetDouble( pInfo->s_fScale );
		pSheet->Cell( nRowIndex, 3 )->SetDouble( pInfo->s_fHeight );
	
		//=====================================================================================================
		//
		// 원거리라면 원거리 공격 이벤트
		//
		//=====================================================================================================
		if( bRange == true )
		{
			if( it->second->IsSequence( ANI::ATTACK_ATTACK1 ) == true )
			{
				int nSize = it->second->GetSequence( ANI::ATTACK_ATTACK1 )->s_vpEvent.Size();
				for( int nEvent=0; nEvent<nSize; ++nEvent )
				{
					CsModelData::sEVENT* pEvent = it->second->GetSequence( ANI::ATTACK_ATTACK1 )->GetEvent( nEvent );
					if( pEvent->s_nType == CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC )
					{
						if( ( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_NORMAL_ATTACK )||
							( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE ) )
						{
							// 첫번째 이벤트 발동시간
							pSheet->Cell( nRowIndex, 4 )->SetInteger( (int)( pEvent->s_fEventTime*1000.0f ) );
							break;
						}
					}
				}
			}
			else
			{
				// 첫번째 이벤트 발동시간
				pSheet->Cell( nRowIndex, 4 )->SetInteger( 0 );
			}
		}
		else
		{
			pSheet->Cell( nRowIndex, 4 )->SetInteger( 0 );
		}

		//=====================================================================================================
		//
		//		스킬
		//
		//=====================================================================================================
		for( int i=0; i<5; ++i )
		{
			if( it->second->IsSequence( ANI::ATTACK_SKILL1 + i ) == true )
			{
				int nSize = it->second->GetSequence( ANI::ATTACK_SKILL1 + i )->s_vpEvent.Size();
				for( int nEvent=0; nEvent<nSize; ++nEvent )
				{
					CsModelData::sEVENT* pEvent = it->second->GetSequence( ANI::ATTACK_SKILL1 + i )->GetEvent( nEvent );

					if( pEvent->s_nType == CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC )
					{
						if( ( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_NORMAL_ATTACK )||
							( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE )||
							( pEvent->s_nStaticIndex == nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE ) )

						{
							// 첫번째 이벤트 발동시간
							pSheet->Cell( nRowIndex, (i*2)+5 )->SetInteger( (int)( pEvent->s_fEventTime*1000.0f ) );
							break;
						}
					}
				}

				// 애니메이션 길이
				NiActorManagerPtr pActor = NiActorManager::Create( it->second->GetInfo()->s_cKfmPath );		
				NiControllerSequence* pNiSequence = pActor->GetSequence( ANI::ATTACK_SKILL1 + i );
				if( pNiSequence )
				{
					pSheet->Cell( nRowIndex, (i*2)+5+1 )->SetInteger( (int)( pNiSequence->GetEndKeyTime()*1000.0f ) );
				}
				else
				{
					//assert_csm2( false, "%s 의 SequenceID 가 이상합니다.", it->second->GetInfo()->s_cKfmPath, ANI::ATTACK_SKILL1 + i );
					pSheet->Cell( nRowIndex, (i*2)+5 )->SetInteger( 0 );
				}
			}
			else
			{
				pSheet->Cell( nRowIndex, (i*2)+5 )->SetInteger( 0 );
				pSheet->Cell( nRowIndex, (i*2)+5+1 )->SetInteger( 0 );
			}
		}
		++nRowIndex;
	}

	char cSavePath[ MAX_PATH ];
	W2M( cSavePath, loadFile.c_str(), MAX_PATH );
	excel.SaveAs( cSavePath );

	CsMessageBox( MB_OK, L"엑셀 저장 완료" );
}

void _DlgController::OnBnClickedButtonSaveHeightexcel()
{
	std::string loadFile;
	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:	loadFile = "GameData\\Korea\\__TacticsOrgHeight.xls";		break;
	case nsCsFileTable::ENGLISH:			loadFile = "GameData\\English\\__TacticsOrgHeight.xls";		break;
	case nsCsFileTable::ENGLISH_A:			loadFile = "GameData\\English_A\\__TacticsOrgHeight.xls";	break;
	case nsCsFileTable::THAILAND:			loadFile = "GameData\\Thailand\\__TacticsOrgHeight.xls";	break;
	case nsCsFileTable::TAIWAN:				loadFile = "GameData\\Taiwan\\__TacticsOrgHeight.xls";		break;
	case nsCsFileTable::HONGKONG:			loadFile = "GameData\\HongKong\\__TacticsOrgHeight.xls";	break;
	default:
		assert_cs( false );
	}

	YExcel::BasicExcel excel;
	bool bLoad = excel.Load( loadFile.c_str() );
	if( bLoad == false )
		return;

	YExcel::BasicExcelWorksheet* pSheet = excel.GetWorksheet( (size_t)0 );
	if( pSheet == NULL )
		return;

	size_t siTotalRow = pSheet->GetTotalRows();
	for( size_t row=2; row<siTotalRow; ++row )
	{
		int nID = pSheet->Cell( row, 2 )->GetInteger();
		if( g_pModelDataMng->IsData( nID ) == false )
		{
			CsMessageBox( MB_OK, L"__TacticsOrgHeight.xls 의 ID : %d 는 존재하지 아니함", nID );
			continue;
		}

		CsModelData::sINFO* pModelInfo = g_pModelDataMng->GetData( nID )->GetInfo();
		pSheet->Cell( row, 4 )->SetDouble( pModelInfo->s_fHeight*pModelInfo->s_fScale*100.0f );
	}
	excel.Save();

	CsMessageBox( MB_OK, L"엑셀 저장 완료" );
}

void _DlgController::OnEnChangeEditScale()
{
	if( m_pSelectModel == NULL )
		return;

	if( GetFocus()->GetSafeHwnd() == m_edit_Scale.GetSafeHwnd() )
	{
		float fScale = CsMax( 0.002f, nsCsMfcFunc::Control2Float( &m_edit_Scale ) );

		m_pSelectModel->GetInfo()->s_fScale = fScale;
		g_ViewModel.ApplyScale();

		nsCsMfcFunc::Float2Control( &m_edit_CharHeight, m_pSelectModel->GetInfo()->s_fScale*m_pSelectModel->GetInfo()->s_fHeight, 3 );
	}
}

void _DlgController::OnEnChangeEditHeight()
{
	if( m_pSelectModel == NULL )
		return;

	if( GetFocus()->GetSafeHwnd() == m_edit_Height.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_Height ) < 0.02f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_Height, 0.02f, 2 );
		}

		float fHeight = nsCsMfcFunc::Control2Float( &m_edit_Height );

		m_pSelectModel->GetInfo()->s_fHeight = fHeight;
		g_ViewModel.ApplyScale();

		g_ViewModel.GetSrcMode()->SetToolHeight( fHeight );

		nsCsMfcFunc::Float2Control( &m_edit_CharHeight, m_pSelectModel->GetInfo()->s_fScale*m_pSelectModel->GetInfo()->s_fHeight, 3 );
	}
}

void _DlgController::OnEnChangeEditWidth()
{
	if( m_pSelectModel == NULL )
		return;

	if( GetFocus()->GetSafeHwnd() == m_edit_Width.GetSafeHwnd() )
	{
		if( nsCsMfcFunc::Control2Float( &m_edit_Width ) < 0.02f )
		{
			nsCsMfcFunc::Float2Control( &m_edit_Width, 0.02f, 2 );
		}

		float fWidth = nsCsMfcFunc::Control2Float( &m_edit_Width );

		m_pSelectModel->GetInfo()->s_fWidth = fWidth;
		g_ViewModel.ApplyScale();

		g_ViewModel.GetSrcMode()->SetToolWidth( fWidth*100.0f );
	}
}

void _DlgController::OnBnClickedCheckHideAbv()
{
	//__CancelSelect();
}

void _DlgController::OnBnClickedCheckHideClient()
{
	g_ViewModel.SetRenderClientBox( m_chk_ClientBox.GetCheck() == BST_UNCHECKED );
}

void _DlgController::OnBnClickedButtonSrcAttdist()
{
	if( g_ViewModel.GetSrcMode() == NULL )
		return;

	if( g_ViewModel.GetDestMode() == NULL )
		return;

	float fAttackEnableDist = g_ViewModel.GetSrcMode()->GetAttackRange() + g_ViewModel.GetDestMode()->GetAttackCharSize();
	nsCsMfcFunc::Int2Control( &m_edit_DestDist, (int)fAttackEnableDist );
}

void _DlgController::OnBnClickedButtonSrcAttdist2()
{
	if( g_ViewModel.GetSrcMode() == NULL )
		return;

	if( g_ViewModel.GetDestMode() == NULL )
		return;

	float fAttackEnableDist = g_ViewModel.GetDestMode()->GetAttackRange() + g_ViewModel.GetSrcMode()->GetAttackCharSize();
	nsCsMfcFunc::Int2Control( &m_edit_DestDist, (int)fAttackEnableDist );
}


void _DlgController::OnLvnColumnclickListModel(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	static bool bIncrease = true;
	bIncrease = !bIncrease;

	// 선택된 것이 있는가
	POSITION pos = m_list_Model.GetFirstSelectedItemPosition();
	int nIndex = m_list_Model.GetNextSelectedItem( pos );
	DWORD dwSelID = 0;
	if( nIndex >= 0 )
		dwSelID = (DWORD)m_list_Model.GetItemData( nIndex );

	__SortModelList( pNMLV->iSubItem, bIncrease );


	// 선택된 넘은 소팅후에 다시 선택 해준다.
	if( dwSelID != 0 )
	{
		int nCount = m_list_Model.GetItemCount();
		for( int c=0; c<nCount; ++c )
		{
			if( dwSelID == (DWORD)m_list_Model.GetItemData( c ) )
			{
				m_list_Model.SetItemState( c, LVIS_SELECTED, LVIS_SELECTED );
				return;
			}
		}
	}

	
	*pResult = 0;
}




struct sSORTINFO : public NiMemObject
{
	TCHAR		s_szType[ 32 ];
	TCHAR		s_szTypeID[ 32 ];
	TCHAR		s_szModelID[ 32 ];
	DWORD		s_dwModelID;
	//DWORD		s_dwTypeID;
	TCHAR		s_szName[ OBJECT_NAME_LEN ];
	TCHAR		s_szKfm[ OBJECT_NAME_LEN ];
};

void _DlgController::__SortModelList( int nColume, bool bIncrease )
{
	std::list< sSORTINFO* > listSort;

	{
		int nListCount = m_list_Model.GetItemCount();
		for( int i=0; i<nListCount; ++i )
		{
			sSORTINFO* pSort = NiNew sSORTINFO;
			m_list_Model.GetItemText( i, 0, pSort->s_szType, 32 );
			m_list_Model.GetItemText( i, 1, pSort->s_szTypeID, OBJECT_NAME_LEN );		
			m_list_Model.GetItemText( i, 2, pSort->s_szModelID, OBJECT_NAME_LEN );	
			m_list_Model.GetItemText( i, 3, pSort->s_szName, OBJECT_NAME_LEN );			
			m_list_Model.GetItemText( i, 4, pSort->s_szKfm, OBJECT_NAME_LEN );	
			pSort->s_dwModelID = _ttoi( pSort->s_szModelID );
			//pSort->s_dwTypeID = _ttoi( pSort->s_szTypeID );

			listSort.push_back( pSort );
		}		
		m_list_Model.DeleteAllItems();
	}	

	while( listSort.size() != 0 )
	{
		std::list< sSORTINFO* >::iterator itInput = listSort.begin();
		std::list< sSORTINFO* >::iterator it = itInput;
		for( ; it!=listSort.end(); ++it )
		{
			switch( nColume )
			{
				// Type
			case 0:
				if( ( _tcscmp( (*it)->s_szType, (*itInput)->s_szType ) < 0 ) == bIncrease )
					itInput = it;				
				break;
				// Type ID
			case 1:
				if( ( _tcscmp( (*it)->s_szTypeID, (*itInput)->s_szTypeID ) < 0 ) == bIncrease )
					itInput = it;
				break;				
			case 2:// 모델 ID
				if( ( _tcscmp( (*it)->s_szModelID, (*itInput)->s_szModelID ) < 0 ) == bIncrease )
					itInput = it;
				break;
			case 3:// 이름
				if( ( _tcscmp( (*it)->s_szName, (*itInput)->s_szName ) < 0 ) == bIncrease )
					itInput = it;
				break;
			case 4:// Kfm
				if( ( _tcscmp( (*it)->s_szKfm, (*itInput)->s_szKfm ) < 0 ) == bIncrease )
					itInput = it;
				break;
			default:
				assert_cs( false );
			}
		}

		int nIndex = m_list_Model.GetItemCount();
// 		m_list_Model.InsertItem( nIndex, (*itInput)->s_szType );
// 		m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, (*itInput)->s_szKoreaName, 0, 0, 0, 0 );
// 		m_list_Model.SetItem( nIndex, 2, LVIF_TEXT, (*itInput)->s_szName, 0, 0, 0, 0 );
// 		m_list_Model.SetItemData( nIndex, (*itInput)->s_dwID );

		m_list_Model.InsertItem( nIndex, (*itInput)->s_szType );
		m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, (*itInput)->s_szTypeID, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 2, LVIF_TEXT, (*itInput)->s_szModelID, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 3, LVIF_TEXT, (*itInput)->s_szName, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 4, LVIF_TEXT,  (*itInput)->s_szKfm, 0, 0, 0, 0 );
		m_list_Model.SetItemData( nIndex,  (*itInput)->s_dwModelID );

		NiDelete *itInput;
		listSort.erase( itInput );
	}
}

bool _DlgController::__InsertListControl( CsModelData* pModelData )
{
	CsModelData::sINFO* pModelInfo = pModelData->GetInfo();


	// 파일이 존재하는 모델인지 확인
	// 존재 하지 않으면 패스
	if( _access_s( pModelInfo->s_cKfmPath, 0 ) != 0 )
	{
		if( CsMessageBoxA( MB_OKCANCEL, "%s\n파일이 존재 하지 않습니다. \nOK : 제거 \nCancel : 패쓰", pModelInfo->s_cKfmPath ) == IDOK )
		{
			return false;
		}
		return true;
	}

	TCHAR szPath[ OBJECT_PATH_LEN ];
	M2W( szPath, pModelInfo->s_cKfmPath, OBJECT_PATH_LEN );
	TCHAR* szKfm = (TCHAR*)nsCSFILE::GetFileName( szPath );

	size_t nLen = _tcslen( szKfm );
	for( size_t i=0; i<nLen; ++i )
	{
		if( szKfm[ i ] == '_' )
		{
			szKfm = &szKfm[ i+1 ];
			break;
		}
	}

	DWORD dwModelID = pModelInfo->s_dwID;

	int nIndex = m_list_Model.GetItemCount();

	bool bInser = false;

	TCHAR szModelID[ 32 ];
	_stprintf_s( szModelID, 32, L"%d", pModelInfo->s_dwID );

	std::list<DWORD> sameObject;
	nsCsFileTable::g_pDigimonMng->FindDigimon_FromModelID( dwModelID, sameObject );
	size_t nCount = sameObject.size();
	std::list<DWORD>::iterator it = sameObject.begin();	
	for( ; it != sameObject.end(); ++it )
	{
		CsDigimon* pNpcTable = nsCsFileTable::g_pDigimonMng->GetDigimon( (*it) );
		SAFE_POINTER_CON( pNpcTable );
		CsDigimon::sINFO* pInfo = pNpcTable->GetInfo();
		SAFE_POINTER_CON( pInfo );

		m_list_Model.InsertItem( nIndex, L"Digimon");
		
		TCHAR szTableID[ 32 ] = {0,};
		_stprintf_s( szTableID, 32, L"%d", pInfo->s_dwDigimonID );

		m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, szTableID, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 2, LVIF_TEXT, szModelID, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 3, LVIF_TEXT, pInfo->s_szName, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 4, LVIF_TEXT, szKfm, 0, 0, 0, 0 );
		m_list_Model.SetItemData( nIndex, dwModelID );
		nIndex++;
		bInser = true;
	}
	
	sameObject.clear();
	nsCsFileTable::g_pNpcMng->FindNpc_FromModelID( dwModelID, sameObject );
	nCount = sameObject.size();
	it = sameObject.begin();
	for( ; it != sameObject.end(); ++it )
	{
		CsNpc* pNpcTable = nsCsFileTable::g_pNpcMng->GetNpc( (*it) );
		SAFE_POINTER_CON( pNpcTable );
		CsNpc::sINFO* pInfo = pNpcTable->GetInfo();
		SAFE_POINTER_CON( pInfo );

		m_list_Model.InsertItem( nIndex, L"Npc");
		
		TCHAR szTableID[ 32 ] = {0,};
		_stprintf_s( szTableID, 32, L"%d", pInfo->s_dwNpcID );

		m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, szTableID, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 2, LVIF_TEXT, szModelID, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 3, LVIF_TEXT, pInfo->s_szName, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 4, LVIF_TEXT, szKfm, 0, 0, 0, 0 );
		m_list_Model.SetItemData( nIndex, dwModelID );
		nIndex++;
		bInser = true;
	}

	if( nsCsFileTable::g_pTamerMng->IsTamer( dwModelID ) )
	{
		CsTamer* pTamerTable = nsCsFileTable::g_pTamerMng->GetTamer( dwModelID );
		CsTamer::sINFO* pInfo = pTamerTable->GetInfo();
		if( pInfo )
		{
			m_list_Model.InsertItem( nIndex, L"Tamer");

			TCHAR szTableID[ 32 ] = {0,};
			_stprintf_s( szTableID, 32, L"%d", pInfo->s_dwTamerID );

			m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, szTableID, 0, 0, 0, 0 );
			m_list_Model.SetItem( nIndex, 2, LVIF_TEXT, szModelID, 0, 0, 0, 0 );
			m_list_Model.SetItem( nIndex, 3, LVIF_TEXT, pInfo->s_szName, 0, 0, 0, 0 );
			m_list_Model.SetItem( nIndex, 4, LVIF_TEXT, szKfm, 0, 0, 0, 0 );
			m_list_Model.SetItemData( nIndex, dwModelID );	
			bInser = true;
		}
	}

	if( !bInser )
	{
		m_list_Model.InsertItem( nIndex, L"");
		m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, L"", 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 2, LVIF_TEXT, szModelID, 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 3, LVIF_TEXT, L"", 0, 0, 0, 0 );
		m_list_Model.SetItem( nIndex, 4, LVIF_TEXT, szKfm, 0, 0, 0, 0 );
		m_list_Model.SetItemData( nIndex, dwModelID );	
	}
	
// 	if( nsCsFileTable::g_pDigimonMng->IsDigimon(dwModelID) )
// 	{
// 		CsDigimon::sINFO* pFTDigimon = nsCsFileTable::g_pDigimonMng->GetDigimon( dwModelID )->GetInfo();
// 		m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, pFTDigimon->s_szName, 0, 0, 0, 0 );
// 	}
// 	if( nsCsFileTable::g_pNpcMng->IsNpc( dwModelID ) )
// 	{
// 		CsNpc::sINFO* pFTNpc = nsCsFileTable::g_pNpcMng->GetNpc( dwModelID )->GetInfo();			
// 		m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, pFTNpc->s_szName, 0, 0, 0, 0 );
// 	}
// 	
// 	if( nsCsFileTable::g_pTamerMng->IsTamer( dwModelID ) )
// 	{
// 		CsTamer::sINFO* pFTTamer = nsCsFileTable::g_pTamerMng->GetTamer( dwModelID )->GetInfo();
// 		m_list_Model.SetItem( nIndex, 1, LVIF_TEXT, pFTTamer->s_szName, 0, 0, 0, 0 );
// 	}



	return true;
}

void _DlgController::OnBnClickedButtonFolder()
{
	if( m_pSelectModel == NULL )
	{
		CsMessageBox( MB_OK, L"선택된 모델이 없습니다." );
		return;

	}

	TCHAR szModel[ MAX_PATH ];
	TCHAR szPath[ MAX_PATH ];
	M2W( szModel, m_pSelectModel->GetInfo()->s_cKfmPath, MAX_PATH );

	GetCurrentDirectory( MAX_PATH, szPath );
	_tcscat_s( szPath, MAX_PATH, L"\\" );
	_tcscat_s( szPath, MAX_PATH, nsCSFILE::GetFilePathOnly( szModel, MAX_PATH, szModel, true ) );

	ShellExecute( NULL, L"open", NULL, NULL, szPath, SW_SHOWDEFAULT );
}

void _DlgController::OnBnClickedButtonCheckAni()
{
	std::map< DWORD, CsModelData* >* pMap = g_pModelDataMng->GetDataMap();
	std::map< DWORD, CsModelData* >::iterator it = pMap->begin();
	std::map< DWORD, CsModelData* >::iterator itEnd = pMap->end();
	CsModelData::sINFO* pMD;
	TCHAR szModel[ MAX_PATH ];
	TCHAR szPath[ MAX_PATH ];

	for( ; it!=itEnd; ++it )
	{
		pMD = it->second->GetInfo();

		// 타입별 분리
		int nType = pMD->s_dwID/10000;

		// 엔피씨는 체크안함
		if( nType == 9 )
			continue;

		// kfm로드
		NiActorManagerPtr pActor = NiActorManager::Create( pMD->s_cKfmPath );


#define CHECK_ANI( ani )		{\
		if( pActor->GetSequence( ani ) == NULL )\
		{\
			if( CsMessageBoxA( MB_OKCANCEL, "( %d ) 애니메이션 없음 ID = %d\n\n%s", ani, pMD->s_dwID, pMD->s_cKfmPath ) == IDOK )\
			{\
				M2W( szModel, pMD->s_cKfmPath, MAX_PATH );\
				GetCurrentDirectory( MAX_PATH, szPath );\
				_tcscat_s( szPath, MAX_PATH, L"\\" );\
				_tcscat_s( szPath, MAX_PATH, nsCSFILE::GetFilePathOnly( szModel, MAX_PATH, szModel, true ) );\
				ShellExecute( NULL, L"open", NULL, NULL, szPath, SW_SHOWDEFAULT );\
			}\
		}\
	}

		// 공용
		CHECK_ANI( ANI::IDLE_NORMAL );
		CHECK_ANI( ANI::IDLE_SHAKE );
		CHECK_ANI( ANI::IDLE_ATTACK );
		CHECK_ANI( ANI::IDLE_CHAR );
		CHECK_ANI( ANI::MOVE_RUN );
		CHECK_ANI( ANI::DAMAGE_DEAD );		

		switch( nType )
		{
		case 8:				// 테이머
			{
				CHECK_ANI( ANI::IDLE_ITEMCROP );
				CHECK_ANI( ANI::IDLE_FATIGUE );
				CHECK_ANI( ANI::MOVE_FATIGUE );
				CHECK_ANI( ANI::DIGIVICE_CHAGE );
			}
			break;
		default:			// 디지몬
			{
				CHECK_ANI( ANI::MOVE_WALK );
				CHECK_ANI( ANI::MOVE_ATTACKRUN );

				CHECK_ANI( ANI::ATTACK_ATTACK1 );
				CHECK_ANI( ANI::ATTACK_ATTACK2 );

				CHECK_ANI( ANI::DAMAGE_DAMAGE1 );
				CHECK_ANI( ANI::DAMAGE_DAMAGE2 );
				CHECK_ANI( ANI::DAMAGE_EVADE1 );
			}
		}
	}
}


void _DlgController::OnBnClickedButtonCheckEvent()
{
#define CHECK_EVENT( ani )		\
{\
	if( pActor->GetSequence( ani ) != NULL )\
	{\
		if( it->second->IsSequence( ani ) == false )\
		{\
			if( CsMessageBoxA( MB_OKCANCEL, "( %d ) 이벤트 없음 ID = %d\n\n%s", ani, pMD->s_dwID, pMD->s_cKfmPath ) == IDOK )\
			{\
				int nListCount = m_list_Model.GetItemCount();\
				for( int lm=0; lm<nListCount; ++lm )\
				{\
					if( pMD->s_dwID == (DWORD)m_list_Model.GetItemData( lm ) )\
					{\
						m_list_Model.SetItemState( lm, LVIS_SELECTED, LVIS_SELECTED );\
						LRESULT result;\
						OnNMClickListModel( NULL, &result );\
						return;\
					}\
				}\
			}\
		}\
		else\
		{\
			int nSize = it->second->GetSequence( ani )->s_vpEvent.Size();\
			bool bFindEvent = false;\
			for( int i=0; i<nSize; ++i )\
			{\
				pEvent = it->second->GetSequence( ani )->s_vpEvent[ i ];\
				if( pEvent->s_nType == CsC_LoadSequence::sTEXTKEY_EVENT::ET_DE_STATIC )\
				{\
					switch( pEvent->s_nStaticIndex )\
					{\
					case nsSTATIC_EVENT::DYE_NORMAL_ATTACK:\
					case nsSTATIC_EVENT::DYE_NORMAL_ATTACK_FILE:\
					case nsSTATIC_EVENT::DYE_ATTACK_CLASH_FILE:\
						bFindEvent = true;\
						break;\
					}\
					if( bFindEvent == true )\
					{\
						break;\
					}\
				}\
			}\
			if( bFindEvent == false )\
			{\
				if( CsMessageBoxA( MB_OKCANCEL, "( %d ) 공격 타점이 없음 ID = %d\n\n%s", ani, pMD->s_dwID, pMD->s_cKfmPath ) == IDOK )\
				{\
					int nListCount = m_list_Model.GetItemCount();\
					for( int lm=0; lm<nListCount; ++lm )\
					{\
						if( pMD->s_dwID == (DWORD)m_list_Model.GetItemData( lm ) )\
						{\
							m_list_Model.SetItemState( lm, LVIS_SELECTED, LVIS_SELECTED );\
							LRESULT result;\
							OnNMClickListModel( NULL, &result );\
							return;\
						}\
					}\
				}\
			}\
		}\
	}\
}

	std::map< DWORD, CsModelData* >* pMap = g_pModelDataMng->GetDataMap();
	std::map< DWORD, CsModelData* >::iterator it = pMap->begin();
	std::map< DWORD, CsModelData* >::iterator itEnd = pMap->end();
	CsModelData::sINFO* pMD;
	CsModelData::sEVENT* pEvent;	

	for( ; it!=itEnd; ++it )
	{
		pMD = it->second->GetInfo();

		// 타입별 분리
		int nType = pMD->s_dwID/10000;

		// kfm로드
		NiActorManagerPtr pActor = NiActorManager::Create( pMD->s_cKfmPath );	
		// 공용 이벤트 체크
		CHECK_EVENT( ANI::ATTACK_ATTACK1 );
		CHECK_EVENT( ANI::ATTACK_ATTACK2 );
		CHECK_EVENT( ANI::ATTACK_MISS );		
	}
}

void _DlgController::OnBnClickedButtonBG()
{
	NiColorA color;
	g_pRenderer->GetBackgroundColor( color );
	if( color == NiColorA::WHITE )
		g_pRenderer->SetBackgroundColor( NiColorA( 0.5f, 0.5f, 0.5f, 0.5f ) );
	else if( color == NiColorA::BLACK )
		g_pRenderer->SetBackgroundColor( NiColorA::WHITE );
	else
		g_pRenderer->SetBackgroundColor( NiColorA::BLACK );
}

BOOL _DlgController::PreTranslateMessage(MSG* pMsg)
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if( pMsg->wParam == VK_RETURN )
		{
			if( GetFocus()->GetSafeHwnd() == m_edit_Find.GetSafeHwnd() )
			{
				OnBnClickedButtonFind();
				m_edit_Find.SetFocus();
				return TRUE;
			}
		}		
	}

	return CFormView::PreTranslateMessage(pMsg);
}

char* GetCharSoundDir( DWORD nModelID )
{
	int nConstance = nModelID/10000;
	switch( nConstance )
	{
	case 9:		// NPC
		return NULL;
	case 8:		// Tamer
		{
			// 테이머는 테이머 ID 와 모델 ID 가 동일하다.
			return nsCsFileTable::g_pTamerMng->GetTamer( nModelID )->GetInfo()->s_cSoundDirName;			
		}
		break;
	default:	// Digimon
		{
			CsDigimon::MAP* pMap = nsCsFileTable::g_pDigimonMng->GetDigimonMap();
			CsDigimon::MAP_IT it = pMap->begin();
			CsDigimon::MAP_IT itEnd = pMap->end();
			for( ; it!=itEnd; ++it )
			{
				if( nModelID == it->second->GetInfo()->s_dwModelID )
				{
					return it->second->GetInfo()->s_cSoundDirName;
				}
			}
		}
	}
	CsMessageBox( MB_OK, L"모델 데이터를 찾을 수 없습니다. Model ID = %d", nModelID );
	return NULL;
}

void _DlgController::OnBnClickedButtonSoundFolder()
{
	nsCSFILE::CreateDirectory( L"Data\\StaticSound" );

	switch( nsCsFileTable::g_eLanguage )
	{
	case nsCsFileTable::KOREA_ORIGINAL:
	case nsCsFileTable::KOREA_TRANSLATION:
		nsCSFILE::CreateDirectory( L"Data\\_StaticSound_Kor" );
		break;
	case nsCsFileTable::ENGLISH:
		nsCSFILE::CreateDirectory( L"Data\\_StaticSound_Eng" );
		break;
	default:
		assert_cs( false );
	}
	
	

	TCHAR szPath[ MAX_PATH ];
	TCHAR szFile[ MAX_PATH ];
	// 모든 모델들 돌아가면서 사운드 체크
	std::map< DWORD, CsModelData* >* pMap = g_pModelDataMng->GetDataMap();
	std::map< DWORD, CsModelData* >::iterator it = pMap->begin();
	std::map< DWORD, CsModelData* >::iterator itEnd = pMap->end();
	CsModelData::sINFO* pInfo = NULL;
	for( ; it!=itEnd; ++it )
	{
		pInfo = it->second->GetInfo();
		char* pDir = GetCharSoundDir( pInfo->s_dwID );
		if( pDir == NULL )
			continue;

		TCHAR szDir[ MAX_PATH ];
		M2W( szDir, pDir, MAX_PATH );

		_stprintf_s( szPath, MAX_PATH, L"Data\\StaticSound\\%s", szDir );
		// 해당 폴더가 이미 존재한다면
		if( _taccess_s( szPath, 0 ) == 0 )
			continue;

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			_stprintf_s( szPath, MAX_PATH, L"Data\\_StaticSound_Kor\\%s", szDir );
			break;
		case nsCsFileTable::ENGLISH:
			_stprintf_s( szPath, MAX_PATH, L"Data\\_StaticSound_Eng\\%s", szDir );
			break;
		default:
			assert_cs( false );
		}
		
		nsCSFILE::CreateDirectory( szPath );

		switch( nsCsFileTable::g_eLanguage )
		{
		case nsCsFileTable::KOREA_ORIGINAL:
		case nsCsFileTable::KOREA_TRANSLATION:
			{
				// 파일들 복사
				switch( pInfo->s_dwID/10000 )
				{
				case 9:		// NPC
					assert_cs( false );
					break;
				case 8:		// Tamer
					{
						_stprintf_s( szFile, MAX_PATH, L"%s\\Idle_Char.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Idle_Shake.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Die.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Tactics_Fail.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\DS_Charge01.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\DS_Charge02.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\DS_Charge03.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\DS_Charge04.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );
					}
					break;
				default:	// Digimon
					{
						_stprintf_s( szFile, MAX_PATH, L"%s\\Idle_Char.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Idle_Shake.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill01.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill02.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill03.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill04.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill05.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill01_tran.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill02_tran.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill03_tran.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill04_tran.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill05_tran.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Die.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Evo01.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Evo02.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );
					}
				}
			}
			break;
		case nsCsFileTable::ENGLISH:
			{
				// 파일들 복사
				switch( pInfo->s_dwID/10000 )
				{
				case 9:		// NPC
					assert_cs( false );
					break;
				case 8:		// Tamer
					{
						_stprintf_s( szFile, MAX_PATH, L"%s\\Idle_Char.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Idle_Shake.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Die.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Tactics_Fail.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\DS_Charge01.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\DS_Charge02.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\DS_Charge03.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\DS_Charge04.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );
					}
					break;
				default:	// Digimon
					{
						_stprintf_s( szFile, MAX_PATH, L"%s\\Idle_Char.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Idle_Shake.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill01.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill02.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill03.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill04.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Skill05.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Die.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Evo01.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );

						_stprintf_s( szFile, MAX_PATH, L"%s\\Evo02.wav", szPath );
						CopyFile( L"ToolData\\Sequence\\dummy.wav", szFile, TRUE );
					}
				}
			}
			break;
		default:
			assert_cs( false );
		}		
	}

	CsMessageBox( MB_OK, L"폴더 생성 완료" );
}

void _DlgController::OnBnClickedButtonCopyDigimon()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	_Dlg_Copy_Digimon kDigimon;
	kDigimon.DoModal();

	kDigimon.DestroyWindow();
}
