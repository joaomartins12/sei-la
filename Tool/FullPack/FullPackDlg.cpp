// FullPackDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "FullPackDlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif

#include <sstream>
//#include <shlwapi.h>

//#include "Ziplib\\DmZip.h"

// CFullPackDlg 대화 상자
#include "..\\..\\LibProj\\CsFilePack\\CsFilePackSystem.h"


inline int CsSelectMessageAlarm( bool isConsole, UINT uType, LPTSTR szApp , ... )
{
	TCHAR App[CSMESSAGEBOX_LEN] = {0 };
	va_list args;
	va_start(args, szApp);
	_vsntprintf_s( App, CSMESSAGEBOX_LEN, szApp, args );
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	if (isConsole)
	{
		_tprintf(_T("%s\n"), App);
		return MB_OK;
	}
	return MessageBox( NULL , App, _T( "CsMessage" ), uType );	
}

inline int CsSelectMessageAlarmA( bool isConsole, UINT uType, char* cApp , ... )
{
	char App[CSMESSAGEBOX_LEN] = {0 };
	va_list args;
	va_start(args, cApp);
	_vsnprintf_s( App, CSMESSAGEBOX_LEN, cApp, args );
	va_end(args);

#ifdef _DEBUG
	//assert_csm( false, App );
#endif

	if (isConsole)
	{
		printf("%s\n", App);
		return MB_OK;
	}
	return MessageBoxA( NULL , App, "CsMessage", uType );	
}

CFullPackDlg::CFullPackDlg(CWnd* pParent /*=NULL*/)
: CDialog(CFullPackDlg::IDD, pParent),m_nAddFileTSize(0),m_nAddFileCount(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFullPackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SRC_DIR, m_list_SrcDir);
	DDX_Control(pDX, IDC_LIST_DEST_DIR, m_list_DestDir);
	DDX_Control(pDX, IDC_EDIT_PACK_ID_START, m_edit_PackID_Start);
	DDX_Control(pDX, IDC_EDIT_PACK_ID_END, m_edit_PackID_End);
	DDX_Control(pDX, IDC_LIST_PACKFILE, m_list_PackFile);
	DDX_Control(pDX, IDC_EDIT_PACK_ID2, m_edit_AddFile_Cnt);
	DDX_Control(pDX, IDC_EDIT_PACK_ID3, m_edit_AddFile_Size);

	DDX_Control(pDX, IDC_KOR_PACKING, m_PackBtn[CFullPackDlg::eKOR_Pack_Btn]);
	DDX_Control(pDX, IDC_USA_PACKING, m_PackBtn[CFullPackDlg::eUSA_Pack_Btn]);
	DDX_Control(pDX, IDC_USA_PACKING_AERIA, m_PackBtn[CFullPackDlg::eAERIA_Pack_Btn]);
	DDX_Control(pDX, IDC_USA_PACKING_STEAM, m_PackBtn[CFullPackDlg::eSteam_Pack_Btn]);
	DDX_Control(pDX, IDC_THA_PACKING, m_PackBtn[CFullPackDlg::eTHA_Pack_Btn]);
	DDX_Control(pDX, IDC_TAI_PACKING, m_PackBtn[CFullPackDlg::eTAI_Pack_Btn]);
	DDX_Control(pDX, IDC_HK_PACKING, m_PackBtn[CFullPackDlg::eHK_Pack_Btn]);
	DDX_Control(pDX, IDC_BUTTON_DO_PATCHPACK, m_PackBtn[CFullPackDlg::ePatch_Pack_Btn]);
	DDX_Control(pDX, IDC_DO_ZIP, m_PackBtn[CFullPackDlg::eZip_Btn]);
}

BEGIN_MESSAGE_MAP(CFullPackDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_LBN_DBLCLK(IDC_LIST_SRC_DIR, &CFullPackDlg::OnLbnDblclkListSrcDir)
	ON_BN_CLICKED(IDC_BUTTON_ADD_DIR, &CFullPackDlg::OnBnClickedButtonAddDir)
	ON_BN_CLICKED(IDC_BUTTON_DEL_DIR, &CFullPackDlg::OnBnClickedButtonDelDir)
	ON_BN_CLICKED(IDC_BUTTON_TO_FILE, &CFullPackDlg::OnBnClickedButtonToFile)
	ON_BN_CLICKED(IDC_BUTTON_DEL_PACKFILE, &CFullPackDlg::OnBnClickedButtonDelPackfile)
	ON_BN_CLICKED(IDC_BUTTON_DELALL_PACKFILE, &CFullPackDlg::OnBnClickedButtonDelallPackfile)

	// Kor
	ON_BN_CLICKED(IDC_KOR_TABLE_LOAD, &CFullPackDlg::OnTableLoad_Kor)
	ON_BN_CLICKED(IDC_KOR_PACKING, &CFullPackDlg::OnTablePacking_Kor)
	ON_BN_CLICKED(IDC_KOR_UNPACKING, &CFullPackDlg::OnTableUnPacking_Kor)
	
	// Usa
	ON_BN_CLICKED(IDC_USA_TABLE_LOAD, &CFullPackDlg::OnTableLoad_USA)
	ON_BN_CLICKED(IDC_USA_PACKING, &CFullPackDlg::OnTablePacking_USA)
	ON_BN_CLICKED(IDC_USA_UNPACKING, &CFullPackDlg::OnTableUnPacking_USA)

	ON_BN_CLICKED(IDC_USA_TABLE_LOAD_AERIA, &CFullPackDlg::OnTableLoad_AERIA)
	ON_BN_CLICKED(IDC_USA_PACKING_AERIA, &CFullPackDlg::OnTablePacking_AERIA)
	ON_BN_CLICKED(IDC_USA_UNPACKING_AERIA, &CFullPackDlg::OnTableUnPacking_AERIA)

	ON_BN_CLICKED(IDC_USA_TABLE_LOAD_STEAM, &CFullPackDlg::OnTableLoad_STEAM)
	ON_BN_CLICKED(IDC_USA_PACKING_STEAM, &CFullPackDlg::OnTablePacking_STEAM)
	ON_BN_CLICKED(IDC_USA_UNPACKING_STEAM, &CFullPackDlg::OnTableUnPacking_STEAM)

	
	// Thailand
	ON_BN_CLICKED(IDC_THA_TABLE_LOAD, &CFullPackDlg::OnTableLoad_Thai)
	ON_BN_CLICKED(IDC_THA_PACKING, &CFullPackDlg::OnTablePacking_Thai)
	ON_BN_CLICKED(IDC_THA_UNPACKING, &CFullPackDlg::OnTableUnPacking_Thai)

	// TAIWAN
	ON_BN_CLICKED(IDC_TAI_TABLE_LOAD, &CFullPackDlg::OnTableLoad_Tw)
	ON_BN_CLICKED(IDC_TAI_PACKING, &CFullPackDlg::OnTablePacking_Tw)
	ON_BN_CLICKED(IDC_TAI_UNPACKING, &CFullPackDlg::OnTableUnPacking_Tw)

	// HONGKONG
	ON_BN_CLICKED(IDC_HK_TABLE_LOAD, &CFullPackDlg::OnTableLoad_Hk)
	ON_BN_CLICKED(IDC_HK_PACKING, &CFullPackDlg::OnTablePacking_Hk)
	ON_BN_CLICKED(IDC_HK_UNPACKING, &CFullPackDlg::OnTableUnPacking_Hk)

	// 패치 데이터 로드
	ON_BN_CLICKED(IDC_PATCH_DATA_LOAD, &CFullPackDlg::OnPatchDataLoad)
	// 패치 팩킹
	ON_BN_CLICKED(IDC_BUTTON_DO_PATCHPACK, &CFullPackDlg::OnDoPatchPacking)
	// 패치 파일 언팩킹
	ON_BN_CLICKED(IDC_BUTTON_DO_PATCH_UNPACKING, &CFullPackDlg::OnDoPatchUnPacking)	

	ON_BN_CLICKED(IDC_INIT_DATA, &CFullPackDlg::OnInitData)		

	ON_BN_CLICKED(IDC_DO_ZIP, &CFullPackDlg::OnDoZip)
	ON_BN_CLICKED(IDC_RESOURCE_UNPACK, &CFullPackDlg::OnResourceUnPacking)
	ON_BN_CLICKED(IDC_FULL_PACK, &CFullPackDlg::OnBnClickedFullPack)
END_MESSAGE_MAP()

void CFullPackDlg::EnableAllPackBtn( BOOL bValue )
{
	for( int n = CFullPackDlg::eKOR_Pack_Btn; n < CFullPackDlg::eMax; ++n )
		m_PackBtn[n].EnableWindow(bValue);
}

void CFullPackDlg::EnablePackBtn( ePackBtn eBtnType, BOOL bValue )
{
	for( int n = CFullPackDlg::eKOR_Pack_Btn; n < CFullPackDlg::eMax; ++n )
	{
		if( eBtnType == n )
		{
			m_PackBtn[n].EnableWindow(bValue);
			break;
		}
	}
}

void CFullPackDlg::EnableOnlyPackBtn(ePackBtn eBtnType)
{
	for( int n = CFullPackDlg::eKOR_Pack_Btn; n < CFullPackDlg::eMax; ++n )
	{
		if( eBtnType == n )
			m_PackBtn[n].EnableWindow(TRUE);
		else
			m_PackBtn[n].EnableWindow(FALSE);
	}
}

// CFullPackDlg 메시지 처리기

BOOL CFullPackDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	TCHAR szCurDir[ MAX_PATH ];
	GetCurrentDirectory( MAX_PATH, szCurDir );
	m_strRootDir = szCurDir;
	__Dir( szCurDir );

	nsCsMfcFunc::Int2Control( &m_edit_AddFile_Cnt, 0 );
	nsCsMfcFunc::Float2Control( &m_edit_AddFile_Size, 0, 3 );

	EnableAllPackBtn(FALSE);
	__SetStatusText( m_strCurDir.GetBuffer() );

	m_ExceptionString.push_back( L" [ . ]" );
	m_ExceptionString.push_back( L" [ .. ]" );
	m_ExceptionString.push_back( L" [ music ]" );
	m_ExceptionString.push_back( L" [ np ]" );
	m_ExceptionString.push_back( L" [ shaders ]" );
	m_ExceptionString.push_back( L" [ sound ]" );
	m_ExceptionString.push_back( L" [ staticsound ]" );
	m_ExceptionString.push_back( L".pf" );
	m_ExceptionString.push_back( L".hf" );
	m_ExceptionString.push_back( L".exe" );
	m_ExceptionString.push_back( L"data\\music\\" );
	m_ExceptionString.push_back( L"data\\np\\" );
	m_ExceptionString.push_back( L"data\\shaders\\" );
	m_ExceptionString.push_back( L"data\\sound\\" );
	m_ExceptionString.push_back( L"data\\staticsound\\" );
	m_ExceptionString.push_back( L"thumbs.db" );

	if(theApp.isConsole())
	{		
		nsCsMfcFunc::Int2Control( &m_edit_PackID_End, theApp.GetPatchNumber() );
		OnPatchDataLoad();
		
		AfxGetMainWnd()->SendMessage(WM_CLOSE);
	}			

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CFullPackDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
}

void CFullPackDlg::OnInitData()
{
	m_list_DestDir.ResetContent();
	m_list_PackFile.ResetContent();
	::SetCurrentDirectory( m_strRootDir.GetBuffer() );
	__Dir( m_strRootDir.GetBuffer() );
	__SetStatusText( m_strCurDir.GetBuffer() );
	EnableAllPackBtn(FALSE);
	
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CFullPackDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CFullPackDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CFullPackDlg::__Dir( TCHAR* szPathOnly )
{
	m_list_SrcDir.ResetContent();

	m_strCurDir = szPathOnly;

	WIN32_FIND_DATA wfd;
	TCHAR szFind[ MAX_PATH ] = {0, };
	_stprintf_s( szFind, MAX_PATH, L"%s\\*.*", szPathOnly );
	HANDLE dh = FindFirstFile( szFind, &wfd);	
	if( dh != INVALID_HANDLE_VALUE )
	{
		TCHAR szTemp[ MAX_PATH ] = {0,};
		do 
		{
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)// 디렉토리
			{
				_stprintf_s( szTemp, MAX_PATH, L" [ %s ]", wfd.cFileName );			
			}
			else
			{
				// 파일이면
				_tcscpy_s( szTemp, MAX_PATH, wfd.cFileName );
			}

			_tcslwr_s( szTemp, MAX_PATH );
			m_list_SrcDir.AddString( szTemp );
		} while ( TRUE == ::FindNextFile(dh, &wfd) );
		FindClose(dh);	
	}	
}

void CFullPackDlg::OnLbnDblclkListSrcDir()
{
	int nSel = m_list_SrcDir.GetCurSel();
	if( nSel == -1 )
	{
		return;
	}

	TCHAR szTemp[ MAX_PATH ] = {0, };
	m_list_SrcDir.GetText( nSel, szTemp );

	// 루트 경로로 이동
	if( _tcscmp( szTemp, L" [ . ]" ) == 0 )
	{
		if( _tcscmp( m_strCurDir, m_strRootDir ) == 0 )
			return;

		__Dir( m_strRootDir.GetBuffer() );
	}
	// 상위 폴더로 이동 - 루트라면 상위로 이동 안됨
	else if( _tcscmp( szTemp, L" [ .. ]" ) == 0 )
	{		
		if( _tcscmp( m_strCurDir, m_strRootDir ) == 0 )
			return;

		TCHAR szFind[ MAX_PATH ];
		__Dir( nsCSFILE::GetFilePathOnly( szFind, MAX_PATH, m_strCurDir, true ) );
	}
	// 디렉토리
	else if( (szTemp[ 1 ]==L'[') && (szTemp[ _tcslen( szTemp )-1 ]==L']')) //디렉토리
	{
		szTemp[ _tcslen( szTemp )-2 ] = NULL;

		if( _tcscmp( m_strCurDir, L".\\" ) == 0 )
			m_strCurDir = &szTemp[ 3 ];
		else
			m_strCurDir = m_strCurDir + L"\\" + &szTemp[ 3 ];

		TCHAR szFind[ MAX_PATH ] = {0, };
		__Dir( nsCSFILE::GetFilePathOnly( szFind, MAX_PATH, m_strCurDir, false ) );
	}
	// 파일 - 무시하겠다.
	else
	{		
	}
}


std::wstring CutString( TCHAR* pOrgDir, TCHAR* pCutStr )
{
	std::wstring orgDir = pOrgDir;
	std::wstring cutStr = pCutStr;
	
	size_t pos = orgDir.find( cutStr );
	if( pos != std::wstring::npos )
		return orgDir.substr(pos);
	return orgDir;
}

BOOL CFullPackDlg::PreTranslateMessage(MSG* pMsg)
{
	switch( pMsg->message )
	{
	case WM_KEYDOWN:
		{
			switch(pMsg->wParam)
			{
			case VK_RETURN:
				{
					if( pMsg->hwnd == GetDlgItem( IDC_EDIT_PACK_ID_END )->GetSafeHwnd() )
					{
						OnPatchDataLoad();
						return FALSE;
					}
				}
				break;
			}
		}
		break;
	}

	return CDialog::PreTranslateMessage(pMsg);
}

// 혹시 몰라서 여기에 추가
// 
LPTSTR CFullPackDlg::GetRelativePath( LPTSTR szOutPath, int nOutSize, LPCTSTR szSubDirectory )
{
	if( szSubDirectory[ 1 ] != L':' && szSubDirectory[ 0 ] != L'\\' && szSubDirectory[ 1 ] != L'\\' )// 네트웍 디렉토리 검사추가
	{
		_tcscpy_s( szOutPath, nOutSize, szSubDirectory );
		return szOutPath;
	}

	assert_cs( szOutPath != NULL );
	assert_cs( szSubDirectory != NULL );

	TCHAR szCurDir[ MAX_PATH ] = {0, };
	GetCurrentDirectory( MAX_PATH, szCurDir );
	int nCurDirLen = (int)_tcslen( szCurDir );

	if( nCurDirLen == _tcslen( szSubDirectory ) )
	{
		_stprintf_s( szOutPath, nOutSize, _T( "" ) );
		return szOutPath;
	}

	_stprintf_s( szOutPath, nOutSize, _T( "%s" ), &szSubDirectory[ nCurDirLen + 1 ] );
	return szOutPath;
}

void CFullPackDlg::OnBnClickedButtonAddDir()
{
	TCHAR szTemp[ MAX_PATH ];
	int nCount = m_list_SrcDir.GetCount();
	for( int i=0; i<nCount; i++ )
	{
		if ( m_list_SrcDir.GetSel(i) <= 0 )
			continue;

		m_list_SrcDir.GetText( i, szTemp );			

		if( (szTemp[ 1 ]==L'[') && (szTemp[ _tcslen( szTemp )-1 ]==L']') ) //디렉토리
		{
			if( ( _tcscmp( szTemp, L" [ . ]" ) == 0 )||( _tcscmp( szTemp, L" [ .. ]" ) == 0 ) )
			{
				continue;
			}

			TCHAR szDir[ MAX_PATH ] = {0, };
			_tcscpy_s( szDir, MAX_PATH, &szTemp[ 3 ] );				
			szDir[ _tcslen( szDir )-2 ] = NULL;

			TCHAR szAdd[ MAX_PATH ];
			_stprintf_s( szAdd, MAX_PATH, L"%s\\%s", m_strCurDir.GetBuffer(), szDir );
			_tcslwr_s( szAdd, MAX_PATH );

			TCHAR szRelative[ MAX_PATH ];
			GetRelativePath( szRelative, MAX_PATH, szAdd );
			if( m_list_DestDir.FindStringExact( -1, szRelative ) == LB_ERR )
			{
				int nInsertIndex = m_list_DestDir.AddString( szRelative );
				// 디렉토리는 1
				m_list_DestDir.SetItemData( nInsertIndex, 1 );
			}
		}
		else
		{
			TCHAR szAdd[ MAX_PATH ];
			_stprintf_s( szAdd, MAX_PATH, L"%s\\%s", m_strCurDir.GetBuffer(), szTemp );
			_tcslwr_s( szAdd, MAX_PATH );

			TCHAR szRelative[ MAX_PATH ];
			GetRelativePath( szRelative, MAX_PATH, szAdd );

			if( m_list_DestDir.FindStringExact( -1, szRelative ) == LB_ERR )
			{
				int nInsertIndex = m_list_DestDir.AddString( szRelative );
				// 파일은 2
				m_list_DestDir.SetItemData( nInsertIndex, 2 );
			}
		}
	}
}

void CFullPackDlg::OnBnClickedButtonDelDir()
{
	int nCount = m_list_DestDir.GetCount();
	for( int i=nCount-1; i>=0; --i )
	{
		if ( m_list_DestDir.GetSel(i) <= 0 )
			continue;

		m_list_DestDir.DeleteString( i );
	}
}

void CFullPackDlg::__SetStatusText( LPTSTR szText , ... )
{
	TCHAR App[ 512 ] = {0 };
	va_list args;
	va_start(args, szText);
	_vsntprintf_s( App, CSMESSAGEBOX_LEN, szText, args );
	va_end(args);
	SetWindowText( App );
}


void CFullPackDlg::OnBnClickedButtonToFile()
{
	int nCount = m_list_DestDir.GetCount();
	if( nCount == 0 )
	{
		CsMessageBox( MB_OK, L"추출 하려는 대상 파일이 없습니다." );
		return;
	}

	__SetStatusText( L"팩킹 파일 목록 창에 파일을 추가하고 있습니다." );
	m_nAddFileCount = 0;
	m_nAddFileTSize = sizeof( CsFPS::FPHeader );

	TCHAR szTemp[ MAX_PATH ];	
	for( int i=0; i<nCount; i++ )
	{
		m_list_DestDir.GetText( i, szTemp );

		switch( m_list_DestDir.GetItemData( i ) )
		{
		case 1:	__ToPackFileList_Dir( szTemp );	break;// 디렉토리
		case 2:	__ToPackFileList_File( szTemp );break;// 파일
		default:assert_cs( false );				break;
		}
	}

	__SetStatusText( m_strCurDir.GetBuffer() );

	nsCsMfcFunc::Int2Control( &m_edit_AddFile_Cnt, m_nAddFileCount );
	nsCsMfcFunc::Float2Control( &m_edit_AddFile_Size, (int)( m_nAddFileTSize/1024 )*0.001f, 3 );
}

void CFullPackDlg::__ToPackFileList_Dir( TCHAR* szDir )
{
	if( !CheckException( szDir ) )
		return;

	WIN32_FIND_DATA wfd;
	TCHAR szSurchPath[ MAX_PATH ] = {0, };
	_stprintf_s( szSurchPath, MAX_PATH, L"%s\\*.*", szDir );
	HANDLE dh = ::FindFirstFile( szSurchPath, &wfd );
	if( dh != INVALID_HANDLE_VALUE )
	{
		TCHAR szFullPath[ MAX_PATH ];
		do 
		{
			if ( !(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )// 디렉토리가 아니면...
			{
				_stprintf_s( szFullPath, MAX_PATH, L"%s\\%s", szDir, wfd.cFileName );
				__ToPackFileList_File( szFullPath );
			}
			// 디렉토리 이면....
			else if ( (_tcscmp(wfd.cFileName, L".") != 0) && (_tcscmp(wfd.cFileName, L"..") != 0) )
			{
				_stprintf_s( szFullPath, MAX_PATH, L"%s\\%s", szDir, wfd.cFileName );
				__ToPackFileList_Dir( szFullPath );
			}
		} while ( TRUE == ::FindNextFile(dh, &wfd) );
		FindClose(dh);
	}
}

void CFullPackDlg::__ToPackFileList_File( TCHAR* szFile )
{
	if( !CheckException(szFile) )
		return;

	if( m_list_PackFile.FindStringExact( -1, szFile ) != LB_ERR )
		return;

	HANDLE hFile = CreateFile( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	assert_csm1( hFile != INVALID_HANDLE_VALUE, L"file = %s", szFile );

	m_nAddFileTSize += ( GetFileSize( hFile, NULL ) + sizeof( CsFPS::sCHUNK ) );
	CloseHandle( hFile );

	++m_nAddFileCount;	
	m_list_PackFile.AddString( szFile );
}


void CFullPackDlg::OnBnClickedButtonDelPackfile()
{
	TCHAR szFile[ MAX_PATH ];
	int nCount = m_list_PackFile.GetCount();
	for( int i=nCount-1; i>=0; --i )
	{
		if ( m_list_PackFile.GetSel(i) <= 0 )
			continue;

		m_list_PackFile.GetText( i, szFile );
		HANDLE hFile = CreateFile( szFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
		assert_cs( hFile != INVALID_HANDLE_VALUE );

		--m_nAddFileCount;	
		m_nAddFileTSize -= ( GetFileSize( hFile, NULL ) + sizeof( CsFPS::sCHUNK ) );
		CloseHandle( hFile );

		m_list_PackFile.DeleteString( i );
	}

	nsCsMfcFunc::Int2Control( &m_edit_AddFile_Cnt, m_nAddFileCount );
	nsCsMfcFunc::Float2Control( &m_edit_AddFile_Size, (int)( m_nAddFileTSize/1024 )*0.001f, 3 );
}

void CFullPackDlg::OnBnClickedButtonDelallPackfile()
{
	m_list_PackFile.ResetContent();

	m_nAddFileCount = 0;
	m_nAddFileTSize = 0;
	nsCsMfcFunc::Int2Control( &m_edit_AddFile_Cnt, 0 );
	nsCsMfcFunc::Float2Control( &m_edit_AddFile_Size, 0, 3 );
}


LRESULT CFullPackDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_CLOSE )
	{
		CDialog::OnCancel();
		return 0;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////
// 테이블 데이터 로드 함수 
//////////////////////////////////////////////////////////////////////////
//Kor
void CFullPackDlg::OnTableLoad_Kor()	// 한국 테이블 데이터 로드
{
	m_list_DestDir.ResetContent();
	m_list_PackFile.ResetContent();

	::SetCurrentDirectory( m_strRootDir.GetBuffer() );

	CString gotoDir = m_strRootDir;
	gotoDir += L"\\data\\bin";
	__Dir( gotoDir.GetBuffer());

	EnableAllPackBtn(FALSE);

	TCHAR szTemp[ MAX_PATH ]={0,};
	int nCount = m_list_SrcDir.GetCount();
	for( int n = 0; n < nCount; ++n )
	{
		m_list_SrcDir.GetText( n, szTemp );
		
		if( nsCSFILE::IsSameString( szTemp, L" [ Korea_Tr ]" ) )
			m_list_SrcDir.SetSel( n, TRUE );
		else if( nsCSFILE::IsSameString( szTemp, L" [ Korea_Org ]" ) )
			m_list_SrcDir.SetSel( n, TRUE );
	}	

	if( 0 == m_list_SrcDir.GetSelCount() )
		return;

	EnableOnlyPackBtn( eKOR_Pack_Btn );
	OnBnClickedButtonAddDir();
	OnBnClickedButtonToFile();
}

// Usa
void CFullPackDlg::OnTableLoad_USA()
{
	m_list_DestDir.ResetContent();
	m_list_PackFile.ResetContent();

	::SetCurrentDirectory( m_strRootDir.GetBuffer() );
	CString gotoDir = m_strRootDir;
	gotoDir += L"\\data\\bin";
	__Dir( gotoDir.GetBuffer());

	EnableAllPackBtn(FALSE);

	TCHAR szTemp[ MAX_PATH ]={0,};
	int nCount = m_list_SrcDir.GetCount();
	for( int n = 0; n < nCount; ++n )
	{
		m_list_SrcDir.GetText( n, szTemp );

		if( nsCSFILE::IsSameString( szTemp, L" [ english ]" ) )
			m_list_SrcDir.SetSel( n, TRUE );
	}	

	if( 0 == m_list_SrcDir.GetSelCount() )
		return;

	EnableOnlyPackBtn( eUSA_Pack_Btn );
	OnBnClickedButtonAddDir();
	OnBnClickedButtonToFile();
}

void CFullPackDlg::OnTableLoad_AERIA()
{
	m_list_DestDir.ResetContent();
	m_list_PackFile.ResetContent();

	::SetCurrentDirectory( m_strRootDir.GetBuffer() );
	CString gotoDir = m_strRootDir;
	gotoDir += L"\\data\\bin";
	__Dir( gotoDir.GetBuffer());

	EnableAllPackBtn(FALSE);

	TCHAR szTemp[ MAX_PATH ]={0,};
	int nCount = m_list_SrcDir.GetCount();
	for( int n = 0; n < nCount; ++n )
	{
		m_list_SrcDir.GetText( n, szTemp );

		if( nsCSFILE::IsSameString( szTemp, L" [ english_a ]" ) )
			m_list_SrcDir.SetSel( n, TRUE );
	}	

	if( 0 == m_list_SrcDir.GetSelCount() )
		return;

	EnableOnlyPackBtn( eAERIA_Pack_Btn );
	OnBnClickedButtonAddDir();
	OnBnClickedButtonToFile();
}

void CFullPackDlg::OnTableLoad_STEAM()
{
	m_list_DestDir.ResetContent();
	m_list_PackFile.ResetContent();

	::SetCurrentDirectory( m_strRootDir.GetBuffer() );
	CString gotoDir = m_strRootDir;
	gotoDir += L"\\data\\bin";
	__Dir( gotoDir.GetBuffer());

	EnableAllPackBtn(FALSE);

	TCHAR szTemp[ MAX_PATH ]={0,};
	int nCount = m_list_SrcDir.GetCount();
	for( int n = 0; n < nCount; ++n )
	{
		m_list_SrcDir.GetText( n, szTemp );

		if( nsCSFILE::IsSameString( szTemp, L" [ english_steam ]" ) )
			m_list_SrcDir.SetSel( n, TRUE );
	}	

	if( 0 == m_list_SrcDir.GetSelCount() )
		return;

	EnableOnlyPackBtn( eSteam_Pack_Btn );
	OnBnClickedButtonAddDir();
	OnBnClickedButtonToFile();
}


// Thailand
void CFullPackDlg::OnTableLoad_Thai()
{
	m_list_DestDir.ResetContent();
	m_list_PackFile.ResetContent();

	::SetCurrentDirectory( m_strRootDir.GetBuffer() );
	CString gotoDir = m_strRootDir;
	gotoDir += L"\\data\\bin";
	__Dir( gotoDir.GetBuffer());

	EnableAllPackBtn(FALSE);

	TCHAR szTemp[ MAX_PATH ]={0,};
	int nCount = m_list_SrcDir.GetCount();
	for( int n = 0; n < nCount; ++n )
	{
		m_list_SrcDir.GetText( n, szTemp );

		if( nsCSFILE::IsSameString( szTemp, L" [ thailand ]" ) )
			m_list_SrcDir.SetSel( n, TRUE );
	}	

	if( 0 == m_list_SrcDir.GetSelCount() )
		return;

	EnableOnlyPackBtn( eTHA_Pack_Btn );
	OnBnClickedButtonAddDir();
	OnBnClickedButtonToFile();
}

// TAIWAN
void CFullPackDlg::OnTableLoad_Tw()
{
	m_list_DestDir.ResetContent();
	m_list_PackFile.ResetContent();

	::SetCurrentDirectory( m_strRootDir.GetBuffer() );
	CString gotoDir = m_strRootDir;
	gotoDir += L"\\data\\bin";
	__Dir( gotoDir.GetBuffer());

	EnableAllPackBtn(FALSE);

	TCHAR szTemp[ MAX_PATH ]={0,};
	int nCount = m_list_SrcDir.GetCount();
	for( int n = 0; n < nCount; ++n )
	{
		m_list_SrcDir.GetText( n, szTemp );

		if( nsCSFILE::IsSameString( szTemp, L" [ Taiwan ]" ) )
			m_list_SrcDir.SetSel( n, TRUE );
	}	

	if( 0 == m_list_SrcDir.GetSelCount() )
		return;

	EnableOnlyPackBtn( eTAI_Pack_Btn );
	OnBnClickedButtonAddDir();
	OnBnClickedButtonToFile();
}

// HONGKONG
void CFullPackDlg::OnTableLoad_Hk()
{
	m_list_DestDir.ResetContent();
	m_list_PackFile.ResetContent();

	::SetCurrentDirectory( m_strRootDir.GetBuffer() );
	CString gotoDir = m_strRootDir;
	gotoDir += L"\\data\\bin";
	__Dir( gotoDir.GetBuffer());

	EnableAllPackBtn(FALSE);
	
	TCHAR szTemp[ MAX_PATH ]={0,};
	int nCount = m_list_SrcDir.GetCount();
	for( int n = 0; n < nCount; ++n )
	{
		m_list_SrcDir.GetText( n, szTemp );

		if( nsCSFILE::IsSameString( szTemp, L" [ HongKong ]" ) )
			m_list_SrcDir.SetSel( n, TRUE );
	}	

	if( 0 == m_list_SrcDir.GetSelCount() )
		return;

	EnableOnlyPackBtn( eHK_Pack_Btn );
	OnBnClickedButtonAddDir();
	OnBnClickedButtonToFile();

}

//////////////////////////////////////////////////////////////////////////
// 팩킹 관련 함수 모음.
//////////////////////////////////////////////////////////////////////////
void CFullPackDlg::OnTablePacking_Kor()	// 한국 테이블 팩킹(Pack02)
{
	int nCount = m_list_PackFile.GetCount();
	if( nCount == 0 )
	{
		CsMessageBox( MB_OK, L"팩킹할 목록이 없습니다." );
		return;
	}

	if( CsMessageBox( MB_OKCANCEL, L"한국 테이블을 팩킹 하시겠습니까?" ) != IDOK )
		return;

	std::list<std::string> files;
	char cPath[ MAX_PATH ];
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	for( int i=0; i<nCount; i++ )
	{
		m_list_PackFile.GetText( i, szFullPath );
		W2M( cPath, szFullPath, MAX_PATH );
		files.push_back( cPath );
	}

	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack02";
	std::string file = packFile.str();
	if( !DoFilePacking( file, files ) )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 폴더를 여시겠습니까?" )  == IDOK )
		OpenWindow( DEFAULT_FOLDER_NAME );
}

void CFullPackDlg::OnTablePacking_USA()
{
	int nCount = m_list_PackFile.GetCount();
	if( nCount == 0 )
	{
		CsMessageBox( MB_OK, L"팩킹할 목록이 없습니다." );
		return;
	}

	if( CsMessageBox( MB_OKCANCEL, L"북미(GSP) 테이블을 팩킹 하시겠습니까?" ) != IDOK )
		return;

	std::list<std::string> files;
	char cPath[ MAX_PATH ];
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	for( int i=0; i<nCount; i++ )
	{
		m_list_PackFile.GetText( i, szFullPath );
		W2M( cPath, szFullPath, MAX_PATH );
		files.push_back( cPath );
	}

	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack03";
	std::string file = packFile.str();
	if( !DoFilePacking( file, files ) )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 폴더를 여시겠습니까?" )  == IDOK )
		OpenWindow( DEFAULT_FOLDER_NAME );
}

void CFullPackDlg::OnTablePacking_AERIA()
{
	int nCount = m_list_PackFile.GetCount();
	if( nCount == 0 )
	{
		CsMessageBox( MB_OK, L"팩킹할 목록이 없습니다." );
		return;
	}

	if( CsMessageBox( MB_OKCANCEL, L"북미(Aeria) 테이블을 팩킹 하시겠습니까?" ) != IDOK )
		return;

	std::list<std::string> files;
	char cPath[ MAX_PATH ];
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	for( int i=0; i<nCount; i++ )
	{
		m_list_PackFile.GetText( i, szFullPath );
		W2M( cPath, szFullPath, MAX_PATH );
		files.push_back( cPath );
	}

	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack03";
	std::string file = packFile.str();
	if( !DoFilePacking( file, files ) )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 폴더를 여시겠습니까?" )  == IDOK )
		OpenWindow( DEFAULT_FOLDER_NAME );
}

void CFullPackDlg::OnTablePacking_STEAM()
{
	int nCount = m_list_PackFile.GetCount();
	if( nCount == 0 )
	{
		CsMessageBox( MB_OK, L"팩킹할 목록이 없습니다." );
		return;
	}

	if( CsMessageBox( MB_OKCANCEL, L"북미(Steam) 테이블을 팩킹 하시겠습니까?" ) != IDOK )
		return;

	std::list<std::string> files;
	char cPath[ MAX_PATH ];
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	for( int i=0; i<nCount; i++ )
	{
		m_list_PackFile.GetText( i, szFullPath );
		W2M( cPath, szFullPath, MAX_PATH );
		files.push_back( cPath );
	}

	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack03";
	std::string file = packFile.str();
	if( !DoFilePacking( file, files ) )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 폴더를 여시겠습니까?" )  == IDOK )
		OpenWindow( DEFAULT_FOLDER_NAME );
}

void CFullPackDlg::OnTablePacking_Thai()
{
	int nCount = m_list_PackFile.GetCount();
	if( nCount == 0 )
	{
		CsMessageBox( MB_OK, L"팩킹할 목록이 없습니다." );
		return;
	}

	if( CsMessageBox( MB_OKCANCEL, L"태국 테이블을 팩킹 하시겠습니까?" ) != IDOK )
		return;

	std::list<std::string> files;
	char cPath[ MAX_PATH ];
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	for( int i=0; i<nCount; i++ )
	{
		m_list_PackFile.GetText( i, szFullPath );
		W2M( cPath, szFullPath, MAX_PATH );
		files.push_back( cPath );
	}

	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack04";
	std::string file = packFile.str();
	if( !DoFilePacking( file, files ) )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 폴더를 여시겠습니까?" )  == IDOK )
		OpenWindow( DEFAULT_FOLDER_NAME );
}


void CFullPackDlg::OnTablePacking_Tw()
{
	int nCount = m_list_PackFile.GetCount();
	if( nCount == 0 )
	{
		CsMessageBox( MB_OK, L"팩킹할 목록이 없습니다." );
		return;
	}

	if( CsMessageBox( MB_OKCANCEL, L"대만 테이블을 팩킹 하시겠습니까?" ) != IDOK )
		return;

	std::list<std::string> files;
	char cPath[ MAX_PATH ];
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	for( int i=0; i<nCount; i++ )
	{
		m_list_PackFile.GetText( i, szFullPath );
		W2M( cPath, szFullPath, MAX_PATH );
		files.push_back( cPath );
	}

	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack06";
	std::string file = packFile.str();
	if( !DoFilePacking( file, files ) )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 폴더를 여시겠습니까?" )  == IDOK )
		OpenWindow( DEFAULT_FOLDER_NAME );
}

void CFullPackDlg::OnTablePacking_Hk()
{
	int nCount = m_list_PackFile.GetCount();
	if( nCount == 0 )
	{
		CsMessageBox( MB_OK, L"팩킹할 목록이 없습니다." );
		return;
	}

	if( CsMessageBox( MB_OKCANCEL, L"홍콩 테이블을 팩킹 하시겠습니까?" ) != IDOK )
		return;

	std::list<std::string> files;
	char cPath[ MAX_PATH ];
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	for( int i=0; i<nCount; i++ )
	{
		m_list_PackFile.GetText( i, szFullPath );
		W2M( cPath, szFullPath, MAX_PATH );
		files.push_back( cPath );
	}

	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack07";
	std::string file = packFile.str();
	if( !DoFilePacking( file, files ) )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 폴더를 여시겠습니까?" )  == IDOK )
		OpenWindow( DEFAULT_FOLDER_NAME );
}

//////////////////////////////////////////////////////////////////////////
// 언팩킹 관련 함수 모음.
//////////////////////////////////////////////////////////////////////////
void CFullPackDlg::OnTableUnPacking_Kor()// 한국 테이블 언팩킹(Pack02)
{
	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack02";
	std::string file = packFile.str();
	DoUnPacking( file );
}

void CFullPackDlg::OnTableUnPacking_USA()
{
	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack03";
	std::string file = packFile.str();
	DoUnPacking( file );
}

void CFullPackDlg::OnTableUnPacking_AERIA()
{
	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack03";
	std::string file = packFile.str();
	DoUnPacking( file );
}

void CFullPackDlg::OnTableUnPacking_STEAM()
{
	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack03";
	std::string file = packFile.str();
	DoUnPacking( file );
}

void CFullPackDlg::OnTableUnPacking_Thai()
{
	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack04";
	std::string file = packFile.str();
	DoUnPacking( file );
}

void CFullPackDlg::OnTableUnPacking_Tw()
{
	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack06";
	std::string file = packFile.str();
	DoUnPacking( file );
}

void CFullPackDlg::OnTableUnPacking_Hk()
{
	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack07";
	std::string file = packFile.str();
	DoUnPacking( file );
}

void CFullPackDlg::OnResourceUnPacking()
{
	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack01";
	std::string file = packFile.str();
	DoUnPacking( file );
}

//////////////////////////////////////////////////////////////////////////
// 패치 팩킹
//////////////////////////////////////////////////////////////////////////

bool CFullPackDlg::CheckException( TCHAR const* pName )
{
	std::wstring str = pName;
	std::list<std::wstring>::iterator it = m_ExceptionString.begin();
	for( ; it != m_ExceptionString.end(); ++it )
	{
		if( std::wstring::npos != str.find( (*it) ) )
			return false;
	}

	return true;
}

CString CFullPackDlg::GetPatchFolderName( CString const& RootDir, int nPatchNum )
{
	CString sFolder = RootDir;
	sFolder += L"\\*.*";

	CString sFindPatchFolderName;

	WIN32_FIND_DATA wfd;
	HANDLE dh = ::FindFirstFile( sFolder.GetBuffer(), &wfd );
	if( dh != INVALID_HANDLE_VALUE )
	{
		TCHAR pPatchNum[MAX_PATH] = {0,};
		_stprintf_s( pPatchNum, MAX_PATH, _T("%d"), nPatchNum );
		TCHAR szFullPath[ MAX_PATH ] = {0,};
		do 
		{
			if ( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )// 디렉토리 이면....
			{
				std::wstring folderName = wfd.cFileName;
				if( std::wstring::npos != folderName.find( pPatchNum ) )
				{
					sFindPatchFolderName = folderName.c_str();
					break;
				}
			}
		} while ( TRUE == ::FindNextFile(dh, &wfd) );
		FindClose(dh);
	}

	return sFindPatchFolderName;
}

void CFullPackDlg::MoveFolder(CString OldPath, CString NewPath)
{
	CFileFind finder;
	BOOL bContinue = TRUE;
	CString StrOldPath = OldPath;
	if(StrOldPath.Right(1) != _T("\\"))
		StrOldPath += _T("\\");

	StrOldPath += _T("*.*");
	bContinue = finder.FindFile(StrOldPath);
	while(bContinue)
	{
		bContinue = finder.FindNextFile();
		if(finder.IsDots()) // Ignore this item.
		{
			continue;
		}
		else if(finder.IsDirectory()) // Directory....
		{
			CString PathName;
			PathName = finder.GetFilePath();
			PathName.Replace(OldPath, NewPath);

			CreateDirectory(PathName, NULL);
			RemoveDirectory(OldPath);
			MoveFolder(finder.GetFilePath(), PathName);
		}
		else // File....
		{
			CString OldFileName, NewFileName;
			OldFileName = finder.GetFilePath();
			NewFileName = finder.GetFilePath();
			NewFileName.Replace(OldPath, NewPath);

			MoveFile(OldFileName, NewFileName);
		}
	}
	finder.Close();
	::RemoveDirectory(OldPath);
}

void CFullPackDlg::OnPatchDataLoad()
{
	int nEndPackID = nsCsMfcFunc::Control2Int( &m_edit_PackID_End );
	if( nEndPackID == 0 )
	{
		CsSelectMessageAlarm( theApp.isConsole(), MB_OK, L"패치 번호가 지정 되지 않았습니다." );
		return;
	} 

	int nStartPackID = nsCsMfcFunc::Control2Int( &m_edit_PackID_Start );
	if( nStartPackID == 0 )
		nStartPackID = nEndPackID;
	
	for( int nPackID = nStartPackID; nPackID <= nEndPackID; ++nPackID )
	{
		m_list_DestDir.ResetContent();
		m_list_PackFile.ResetContent();

		CString sFindPatchFolderName = GetPatchFolderName( m_strRootDir, nPackID );
		if( sFindPatchFolderName.IsEmpty() )
			return;

		CString gotoDir = m_strRootDir;
		gotoDir += L"\\";
		gotoDir += sFindPatchFolderName;

		CString DestDir = gotoDir;
		CString SourDir = gotoDir + L"\\Pack01";
		MoveFolder( SourDir, DestDir );			
		::RemoveDirectory(SourDir);

		gotoDir += L"\\data";

		__Dir( gotoDir.GetBuffer());

		EnableAllPackBtn(FALSE);

		TCHAR szTemp[ MAX_PATH ]={0,};
		int nCount = m_list_SrcDir.GetCount();
		for( int n = 0; n < nCount; ++n )
		{
			m_list_SrcDir.GetText( n, szTemp );

			if( !CheckException( szTemp ) )
				continue;

			m_list_SrcDir.SetSel( n, TRUE );
		}

		__SetStatusText( gotoDir.GetBuffer() );

		TCHAR setDir[MAX_PATH] = {0,};
		nsCSFILE::GetFilePathOnly( setDir, MAX_PATH, gotoDir.GetBuffer(), true );
		::SetCurrentDirectory( setDir );

		if( 0 != m_list_SrcDir.GetSelCount() )
		{
			OnBnClickedButtonAddDir();
			OnBnClickedButtonToFile();
		}

		TCHAR packIdStr[32];
		_itot(nPackID,packIdStr, 10);
		m_edit_PackID_End.SetWindowText( packIdStr );

		CString MakeHashDataPath;
		CString MakePackFilePath;
		_OnDoPatchPacking(MakeHashDataPath, MakePackFilePath);

		OnDoZip();

		if( !MakePackFilePath.IsEmpty() )// 패킹 파일이 생성 瑛?때만 
		{
			TCHAR curDir[_MAX_PATH] = _T("");
			::GetCurrentDirectory( _countof(curDir), curDir );

			TCHAR szDrive[_MAX_DRIVE] = _T("");
			TCHAR szDir[_MAX_DIR] = _T("");
			TCHAR szFName[_MAX_FNAME] = _T("");
			TCHAR szExt[_MAX_EXT] = _T("");
			TCHAR szZipFileFullPath[_MAX_PATH]=_T("");

			_tsplitpath_s(curDir, szDrive, szDir, szFName, szExt);
			_tmakepath_s( szZipFileFullPath, szDrive, szDir, szFName, _T("zip") );

			int nRE = CsMessageBox( MB_OKCANCEL, L"해쉬 파일 및 / 압축 파일을 아카마이에 올리시겠습니까?\n압축파일 : %s", szZipFileFullPath );
			if( IDOK == nRE )
			{
				CString cCommand = MakeHashDataPath + _T(" ") + MakePackFilePath += _T(" ");
				cCommand += szZipFileFullPath;
				_DoHashDataTool(  cCommand.GetBuffer() );
			}
		}
		else
		{
			TCHAR curDir[_MAX_PATH] = _T("");
			::GetCurrentDirectory( _countof(curDir), curDir );

			TCHAR szDrive[_MAX_DRIVE] = _T("");
			TCHAR szDir[_MAX_DIR] = _T("");
			TCHAR szFName[_MAX_FNAME] = _T("");
			TCHAR szExt[_MAX_EXT] = _T("");
			TCHAR szZipFileFullPath[_MAX_PATH]=_T("");

			_tsplitpath_s(curDir, szDrive, szDir, szFName, szExt);
			_tmakepath_s( szZipFileFullPath, szDrive, szDir, szFName, _T("zip") );

			int nRE = CsMessageBox( MB_OKCANCEL, L"압축 파일을 아카마이에 올리시겠습니까?\n%s", szZipFileFullPath );
			if( IDOK == nRE )
			{
				CString cCommand = szZipFileFullPath;
				_DoHashDataTool( cCommand.GetBuffer() );
			}
		}
	}

	CsSelectMessageAlarm( theApp.isConsole(), MB_OK, L"패치 패킹이 완료 되었습니다." );
}

void CFullPackDlg::_OnDoPatchPacking(CString& hashPath, CString& PackFile)
{

	int nCount = m_list_PackFile.GetCount();
	if( nCount == 0 )
	{
		//CsMessageBox( MB_OK, L"팩킹할 목록이 없습니다." );
		return;
	}

	int nPackID = nsCsMfcFunc::Control2Int( &m_edit_PackID_End );
	if( nPackID == 0 )
	{
		CsMessageBox( MB_OK, L"패치 번호가 지정 되지 않았습니다." );
		return;
	}

	std::list<std::string> files;
	char cPath[ MAX_PATH ];
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	for( int i=0; i<nCount; i++ )
	{
		m_list_PackFile.GetText( i, szFullPath );
		W2M( cPath, szFullPath, MAX_PATH );
		files.push_back( cPath );
	}

	std::string dir = CT2CA( m_strCurDir.operator LPCWSTR());
	std::stringstream packFile;
	packFile << dir.c_str() <<  ASB_FD_MARK << PATCH_PACK_NAME << nPackID;
	std::string file = packFile.str();
	if( !DoFilePacking( file, files ) )
		return;

	//	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 기존 파일을 지우시겠습니까?" )  == IDOK )
	nsCSFILE::DeletePathTillRoot( files, DEFAULT_FOLDER_NAME );

	hashPath = m_strCurDir + _T("\\");
	PackFile = CA2CT(file.c_str());
	PackFile += _T(PACK_EX_NAME);

	// 	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 폴더를 여시겠습니까?" )  == IDOK )
	// 		OpenWindow( DEFAULT_FOLDER_NAME );

	//	EnablePackBtn(ePackBtn::eZip_Btn, TRUE);
}

void CFullPackDlg::OnDoPatchPacking()
{
	CString MakeHashDataPath;
	CString MakePackFilePath;
	_OnDoPatchPacking(MakeHashDataPath, MakePackFilePath);	
	
	if( !MakePackFilePath.IsEmpty() )// 패킹 파일이 생성 瑛?때만 
	{
		CString cCommand = MakeHashDataPath + _T(" ") + MakePackFilePath;
		_DoHashDataTool( cCommand.GetBuffer() );
	}
}

//////////////////////////////////////////////////////////////////////////
// 패치 파일 언팩킹
void CFullPackDlg::OnDoPatchUnPacking()
{
	int nPackID = nsCsMfcFunc::Control2Int( &m_edit_PackID_End );
	if( nPackID == 0 )
	{
		CsMessageBox( MB_OK, L"언팩킹할 패치 번호가 지정 되지 않았습니다." );
		return;
	}

	CString sFindPatchFolderName = GetPatchFolderName( m_strRootDir, nPackID );
	if( sFindPatchFolderName.IsEmpty() )
		return;

	CString gotoDir = m_strRootDir;
	gotoDir += L"\\";
	gotoDir += sFindPatchFolderName;

	::SetCurrentDirectory( gotoDir.GetBuffer() );

	gotoDir += L"\\data\\";

	char cPath[MAX_PATH] = {0,};
	W2M( cPath, gotoDir.GetBuffer(), MAX_PATH );

	std::stringstream packFile;
	packFile << cPath << PATCH_PACK_NAME << nPackID;
	std::string file = packFile.str();
	DoUnPacking( file );
}

//////////////////////////////////////////////////////////////////////////

void CFullPackDlg::DoUnPacking( std::string const& sUnPackFileName )
{
	std::string Packfile = sUnPackFileName;
	Packfile += PACK_EX_NAME;
	std::string Hashfile = sUnPackFileName;
	Hashfile += HASH_EX_NAME;
	if( _access_s( Packfile.c_str(), 0 ) != 0 || _access_s( Hashfile.c_str(), 0 ) != 0 )
	{
 		CsMessageBox( MB_OK, L"지정된 파일이 존재하지 않습니다." );
		return;
	}

	// 패킹 파일 오픈
	if( !CsFPS::CsFPSystem::Initialize( true, sUnPackFileName.c_str(), false ) )
	{
		CsMessageBoxA( MB_OK, "%s 파일을 Unpacking 실패", sUnPackFileName.c_str() );
		return;
	}

	// 패킹 파일 언팩킹
	CsFPS::CsFPSystem::UnPacking();
	CsFPS::CsFPSystem::Destroy();

	__SetStatusText( m_strCurDir.GetBuffer() );

	if( CsMessageBox( MB_OKCANCEL, L"언패킹이 완료 되었습니다. 원본 팩파일을 지우시겠습니까?" ) == IDOK )
	{
		DWORD mode = GetFileAttributesA( Packfile.c_str() );
		mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY );
		SetFileAttributesA( Packfile.c_str(), mode );
		DeleteFileA( Packfile.c_str() );

		mode = GetFileAttributesA( Hashfile.c_str() );
		mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY );
		SetFileAttributesA( Hashfile.c_str(), mode );
		DeleteFileA( Hashfile.c_str() );
	}
}

bool CFullPackDlg::DoFilePacking( std::string const& sPackFile, std::list<std::string> const& fileList )
{
	if( CsFPS::CsFPSystem::IsExistPackFile( sPackFile.c_str() ) )
	{
		if( CsMessageBox( MB_OKCANCEL, L"같은 이름을 가진 화일이 있습니다. 제거 하시고 계속 진행하시겠습니까?" ) == IDCANCEL )
			return false;

		std::string Packfile = sPackFile;
		Packfile += PACK_EX_NAME;
		std::string Hashfile = sPackFile;
		Hashfile += HASH_EX_NAME;

		DWORD mode = GetFileAttributesA( sPackFile.c_str() );
		mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY );
		SetFileAttributesA( sPackFile.c_str(), mode );
		DeleteFileA( sPackFile.c_str() );

		mode = GetFileAttributesA( Hashfile.c_str() );
		mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY );
		SetFileAttributesA( Hashfile.c_str(), mode );
		DeleteFileA( Hashfile.c_str() );
	}

	if( !CsFPS::CsFPSystem::Initialize( true, sPackFile.c_str(), true ) )
	{
		CsMessageBox( MB_OK, L"패킹파일을 생성하지 못하였습니다." );
		CsFPS::CsFPSystem::Destroy();
		return false;
	}

	int fileSize = (int)fileList.size();
	CsFPS::CsFPSystem::SetAllocCount( fileSize );

	std::list<std::string>::const_iterator it = fileList.begin();
	for( int n = 0; it != fileList.end(); ++it, ++n )
	{
		if( !CsFPS::CsFPSystem::AddFile( (*it).c_str(), (*it).c_str() ) )
		{
			CsMessageBoxA( MB_OK, "팩킹 실패 : %s", (*it).c_str() );
			CsFPS::CsFPSystem::Destroy();
			return false;
		}

		__SetStatusText( L"파일을 패킹 하고 있습니다. [ %d / %d ]", n+1, fileSize );
	}

	CsFPS::CsFPSystem::SaveHashPack();

	m_nAddFileCount = 0;
	m_nAddFileTSize = 0;

	__SetStatusText( m_strCurDir.GetBuffer() );
	CsFPS::CsFPSystem::Destroy();
	return true;
}

void CFullPackDlg::OpenWindow( char const* pFolder)
{
	char curDir[1024] = {0,};
	::GetCurrentDirectoryA( 1024, curDir );		
	std::string openDir = curDir;
	openDir += ASB_FD_MARK;
	openDir += DEFAULT_FOLDER_NAME;
	openDir += pFolder;
	ShellExecuteA(NULL, "open", "explorer.exe", pFolder, NULL, SW_SHOWNORMAL);
}

void CFullPackDlg::OnDoZip()
{
	CHAR curDir[_MAX_PATH] = { 0, };
	::GetCurrentDirectoryA(_countof(curDir), curDir);

	std::string zipFolder = curDir;
	std::string zipFileName = DmCS::FileFn::FindFilePath(zipFolder.c_str());
	zipFileName += DmCS::FileFn::FindFileName(zipFolder.c_str());
	zipFileName += ".zip";

	DmCS::CZipper zipper;
	if (!zipper.OpenZip(zipFileName.c_str(), curDir, false))
	{
		zipper.CloseZip();
		CsMessageBoxA(MB_OK, "압축 파일 생성 실패 : %s", zipFileName.c_str());
		return;
	}

	zipFolder += "\\";

	if (!zipper.AddFolderToZip(zipFolder.c_str()))
	{
		zipper.CloseZip();
		CsMessageBoxA(MB_OK, "압축 실패 : %s", curDir);
		return;
	}

	zipper.CloseZip();
	EnablePackBtn(eZip_Btn, FALSE);
}
void CFullPackDlg::OnBnClickedFullPack()
{
	m_list_DestDir.ResetContent();
	m_list_PackFile.ResetContent();

	CString gotoDir = m_strRootDir;
	gotoDir += L"\\data";

	__Dir( gotoDir.GetBuffer());

	EnableAllPackBtn(FALSE);

	TCHAR szTemp[ MAX_PATH ]={0,};
	int nCount = m_list_SrcDir.GetCount();
	for( int n = 0; n < nCount; ++n )
	{
		m_list_SrcDir.GetText( n, szTemp );

		if( !CheckException( szTemp ) )
			continue;

		m_list_SrcDir.SetSel( n, TRUE );
	}

	__SetStatusText( gotoDir.GetBuffer() );

	TCHAR setDir[MAX_PATH] = {0,};
	nsCSFILE::GetFilePathOnly( setDir, MAX_PATH, gotoDir.GetBuffer(), true );
	::SetCurrentDirectory( setDir );

	if( 0 != m_list_SrcDir.GetSelCount() )
	{
		OnBnClickedButtonAddDir();
		OnBnClickedButtonToFile();
	}

	std::list<std::string> files;
	char cPath[ MAX_PATH ];
	TCHAR szFullPath[ MAX_PATH ] = {0, };
	nCount = m_list_PackFile.GetCount();
	for( int i=0; i<nCount; i++ )
	{
		m_list_PackFile.GetText( i, szFullPath );
		W2M( cPath, szFullPath, MAX_PATH );
		files.push_back( cPath );
	}

	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME <<  ASB_FD_MARK << "Pack01";
	std::string file = packFile.str();
	if( !DoFilePacking( file, files ) )
		return;

	if( CsMessageBox( MB_OKCANCEL, L"패킹이 완료 되었습니다. 폴더를 여시겠습니까?" )  == IDOK )
		OpenWindow( DEFAULT_FOLDER_NAME );
// 
// 	TCHAR packIdStr[32];
// 	_itot(nPackID,packIdStr, 10);
// 	m_edit_PackID_End.SetWindowText( packIdStr );
// 	OnDoPatchPacking();
// 
// 	OnDoZip();

}


bool CFullPackDlg::_DoHashDataTool( TCHAR* cCommand )
{
	CString exeFilePath = m_strRootDir + _T("\\") + _T("ResourceIntegrityTool.exe");

	SHELLEXECUTEINFO si;                             // SHELLEXECUTEINFO 구조체를 하나 만든다
	ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));       // si 구조체 초기화 
	si.cbSize = sizeof(SHELLEXECUTEINFO);            // 크기 설정

	si.fMask = SEE_MASK_NOCLOSEPROCESS;              // ??
	si.lpVerb = L"runas";                             // 동작 형식(파일열기)
	si.lpFile = exeFilePath.GetBuffer();							     // szPath;  // 실행할 파일 경로 지정
	si.lpParameters = cCommand;						 // szArg;    // 인자값 지정	
	si.nShow = SW_SHOWNORMAL;                        // 표시 형식	

	if( ShellExecuteEx(&si) == FALSE )               // 외부 프로그램 호출
		return false;
	return true;
}