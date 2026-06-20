// Lan_To_HeaderDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Lan_To_HeaderDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


void sRESIST::Save()
{
	FILE* fp = NULL;
	_tfopen_s( &fp, RESIST_FILE_PATH, L"wb" );

	fwrite( this, sizeof( sRESIST ), 1, fp );

	fclose( fp );
}

void sRESIST::Load()
{
	FILE* fp = NULL;
	_tfopen_s( &fp, RESIST_FILE_PATH, L"rb" );

	fread( this, sizeof( sRESIST ), 1, fp );

	fclose( fp );
}


// CLan_To_HeaderDlg 대화 상자

void CLan_To_HeaderDlg::__Init()
{
	// 저장된 레지스트 파일이 있나 체크
	if( _taccess_s( RESIST_FILE_PATH, 0 ) == 0 )
	{
		m_Resist.Load();
	}
	else
	{
		// 디렉토리 생성
		nsCSFILE::CreateDirectory( L"ToolRgs" );
		GetCurrentDirectory( MAX_PATH, m_Resist.s_szWorkDir );
	}

	m_edit_WorkDir.SetWindowText( m_Resist.s_szWorkDir );
}

void CLan_To_HeaderDlg::__Delete()
{
}

void CLan_To_HeaderDlg::__NewLine( FILE* fp, int nNum )
{	
	for( int i=0; i<nNum; ++i )
	{
		fwprintf( fp, L"\n" );
	}
}


void CLan_To_HeaderDlg::OnBnClickedButtonEnglish()
{
#define _ENGLISH_PATH_H		L"_Lan_English.h"
#define _ENGLISH_PATH_CPP	L"_Lan_English.cpp"

	//=========================================================================================================
	//
	//		초기화
	//
	//=========================================================================================================
	// 파일 테이블 로드
	nsCsFileTable::g_eLanguage = nsCsFileTable::ENGLISH;
	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;
	nsCsFileTable::g_pLocalizingMng = new CsLocalizingMng;
	nsCsFileTable::g_pLocalizingMng->Init();

	TCHAR szPath[ MAX_PATH ] = {0, };
	_stprintf_s( szPath, MAX_PATH, L"%s\\%s", m_Resist.s_szWorkDir, _ENGLISH_PATH_H );


	//=========================================================================================================
	//
	//		Source Check
	//
	//=========================================================================================================
	__SourceCheck();

	FILE* fp = NULL;
	//=========================================================================================================
	//
	//		.h
	//
	//=========================================================================================================
	if( _taccess_s( szPath, 0 ) != 0 )
	{
		// 헤더파일이 없을경우에만 만들겠다
		_tfopen_s( &fp, szPath, L"wt,ccs=UNICODE" );
		fwprintf( fp, L"#pragma once" );												__NewLine( fp, 2 );		

		fwprintf( fp, L"#ifndef _LAN" );												__NewLine( fp, 1 );
		fwprintf( fp, L"	#define _LAN( str )		__LanGetString_Eng( L##str )" );	__NewLine( fp, 1 );
		fwprintf( fp, L"#endif //_LAN" );												__NewLine( fp, 2 );

		fwprintf( fp, L"TCHAR*	__LanGetString_Eng( TCHAR* str );" );					__NewLine( fp, 1 );
		fclose( fp );
	}
	

	//=========================================================================================================
	//
	//		.cpp
	//
	//=========================================================================================================
	_stprintf_s( szPath, MAX_PATH, L"%s\\%s", m_Resist.s_szWorkDir, _ENGLISH_PATH_CPP );
	_tfopen_s( &fp, szPath, L"wt,ccs=UNICODE" );
	if( fp == NULL )
	{
		CsMessageBox( MB_OK, L"%s 파일을 체크아웃 해주시고, 다시 실행하여 주십시오.", _ENGLISH_PATH_CPP );
	}
	else
	{
		__NewLine( fp, 3 );
		fwprintf( fp, L"#include \"stdafx.h\"" );										__NewLine( fp, 1 );
		fwprintf( fp, L"#include \"%s\"", _ENGLISH_PATH_H );							__NewLine( fp, 3 );

		fwprintf( fp, L"TCHAR*	__LanGetString_Eng( TCHAR* str )" );					__NewLine( fp, 1 );
		fwprintf( fp, L"{" );															__NewLine( fp, 1 );
		fwprintf( fp, L"	size_t nRes = CsFileHash::GetHashCode( str );" );			__NewLine( fp, 1 );
		fwprintf( fp, L"	switch( nRes )" );											__NewLine( fp, 1 );
		fwprintf( fp, L"	{" );														__NewLine( fp, 1 );

		std::map< size_t, CsLocalizing* >		mapCheck;
		CsLocalizing::LIST* pList = nsCsFileTable::g_pLocalizingMng->GetList();
		CsLocalizing::LIST_IT it = pList->begin();
		CsLocalizing::LIST_IT itEnd = pList->end();
		for( ; it!=itEnd; ++it )
		{
			size_t nHash = CsFileHash::GetHashCode( (TCHAR*)(*it)->GetInfo()->s_szKor.data() );
			if( mapCheck.find( nHash ) != mapCheck.end() )
			{
				// 중복 해쉬 발견
				if( CsMessageBox( MB_OKCANCEL,	L"중복 해쉬 코드 발견 \n\n"
					L"한글1 : %s\n"
					L"한글2 : %s\n"
					L"영문1 : %s\n"
					L"영문2 : %s\n",
					mapCheck[ nHash ]->GetInfo()->s_szKor.data(),
					(*it)->GetInfo()->s_szKor.data(),
					mapCheck[ nHash ]->GetInfo()->s_szEng.data(),
					(*it)->GetInfo()->s_szEng.data() )
					
					== IDCANCEL )
				{
					break;
				}
			}
			else
			{
				mapCheck[ nHash ] = *it;
			}

			fwprintf( fp, L"	case %u:\n						/* %s */", nHash, (*it)->GetInfo()->s_szKor.data() );	__NewLine( fp, 1 );
			fwprintf( fp, L"		return			L\"%s\";", (*it)->GetInfo()->s_szEng.data() );			__NewLine( fp, 2 );
		}

		fwprintf( fp, L"	}" );														__NewLine( fp, 1 );
		fwprintf( fp, L"	assert_cs( false );" );										__NewLine( fp, 1 );
		fwprintf( fp, L"	return L\"NoString\";" );									__NewLine( fp, 1 );
		fwprintf( fp, L"}" );															__NewLine( fp, 1 );


		fclose( fp );

		CsMessageBox( MB_OK, L"%s 헤더 파일 생성 완료", _ENGLISH_PATH_CPP );
	}
	
	//=========================================================================================================
	//
	//		종료
	//
	//=========================================================================================================
	nsCsFileTable::g_pLocalizingMng->Delete();
	SAFE_DELETE( nsCsFileTable::g_pLocalizingMng );	
}


void CLan_To_HeaderDlg::OnBnClickedButtonThailand()
{
#define _THAILAND_PATH_H		L"_Lan_Thailand.h"
#define _THAILAND_PATH_CPP		L"_Lan_Thailand.cpp"

	//=========================================================================================================
	//
	//		초기화
	//
	//=========================================================================================================
	// 파일 테이블 로드
	nsCsFileTable::g_eLanguage = nsCsFileTable::THAILAND;
	nsCsFileTable::g_eFileType = nsCsFileTable::FT_EXCEL;
	nsCsFileTable::g_pLocalizingMng = new CsLocalizingMng;
	nsCsFileTable::g_pLocalizingMng->Init();

	TCHAR szPath[ MAX_PATH ] = {0, };
	_stprintf_s( szPath, MAX_PATH, L"%s\\%s", m_Resist.s_szWorkDir, _THAILAND_PATH_H );


	//=========================================================================================================
	//
	//		Source Check
	//
	//=========================================================================================================
	__SourceCheck();

	FILE* fp = NULL;
	//=========================================================================================================
	//
	//		.h
	//
	//=========================================================================================================
	if( _taccess_s( szPath, 0 ) != 0 )
	{
		// 헤더파일이 없을경우에만 만들겠다
		_tfopen_s( &fp, szPath, L"wt,ccs=UNICODE" );
		fwprintf( fp, L"#pragma once" );												__NewLine( fp, 2 );		

		fwprintf( fp, L"#ifndef _LAN" );												__NewLine( fp, 1 );
		fwprintf( fp, L"	#define _LAN( str )		__LanGetString_Thai( L##str )" );	__NewLine( fp, 1 );
		fwprintf( fp, L"#endif //_LAN" );												__NewLine( fp, 2 );

		fwprintf( fp, L"TCHAR*	__LanGetString_Thai( TCHAR* str );" );					__NewLine( fp, 1 );
		fclose( fp );
	}
	

	//=========================================================================================================
	//
	//		.cpp
	//
	//=========================================================================================================
	_stprintf_s( szPath, MAX_PATH, L"%s\\%s", m_Resist.s_szWorkDir, _THAILAND_PATH_CPP );
	_tfopen_s( &fp, szPath, L"wt,ccs=UNICODE" );
	if( fp == NULL )
	{
		CsMessageBox( MB_OK, L"%s 파일을 체크아웃 해주시고, 다시 실행하여 주십시오.", _THAILAND_PATH_CPP );
	}
	else
	{
		__NewLine( fp, 3 );
		fwprintf( fp, L"#include \"stdafx.h\"" );										__NewLine( fp, 1 );
		fwprintf( fp, L"#include \"%s\"", _THAILAND_PATH_H );							__NewLine( fp, 3 );

		fwprintf( fp, L"TCHAR*	__LanGetString_Thai( TCHAR* str )" );					__NewLine( fp, 1 );
		fwprintf( fp, L"{" );															__NewLine( fp, 1 );
		fwprintf( fp, L"	size_t nRes = CsFileHash::GetHashCode( str );" );			__NewLine( fp, 1 );
		fwprintf( fp, L"	switch( nRes )" );											__NewLine( fp, 1 );
		fwprintf( fp, L"	{" );														__NewLine( fp, 1 );

		std::map< size_t, CsLocalizing* >		mapCheck;
		CsLocalizing::LIST* pList = nsCsFileTable::g_pLocalizingMng->GetList();
		CsLocalizing::LIST_IT it = pList->begin();
		CsLocalizing::LIST_IT itEnd = pList->end();
		for( ; it!=itEnd; ++it )
		{
			size_t nHash = CsFileHash::GetHashCode( (TCHAR*)(*it)->GetInfo()->s_szKor.data() );
			if( mapCheck.find( nHash ) != mapCheck.end() )
			{
				// 중복 해쉬 발견
				if( CsMessageBox( MB_OKCANCEL,	L"중복 해쉬 코드 발견 \n\n"
					L"한글1 : %s\n"
					L"한글2 : %s\n"
					L"영문1 : %s\n"
					L"영문2 : %s\n",
					L"태국1 : %s\n"
					L"태국2 : %s\n",
					mapCheck[ nHash ]->GetInfo()->s_szKor.data(),
					(*it)->GetInfo()->s_szKor.data(),
					mapCheck[ nHash ]->GetInfo()->s_szEng.data(),
					(*it)->GetInfo()->s_szEng.data(),
					mapCheck[ nHash ]->GetInfo()->s_szThai.data(),
					(*it)->GetInfo()->s_szThai.data())
					
					== IDCANCEL )
				{
					break;
				}
			}
			else
			{
				mapCheck[ nHash ] = *it;
			}

			fwprintf( fp, L"	case %u:\n						/* %s */", nHash, (*it)->GetInfo()->s_szKor.data() );	__NewLine( fp, 1 );
			fwprintf( fp, L"		return			L\"%s\";", (*it)->GetInfo()->s_szThai.data() );						__NewLine( fp, 2 );
		}

		fwprintf( fp, L"	}" );														__NewLine( fp, 1 );
		fwprintf( fp, L"	assert_cs( false );" );										__NewLine( fp, 1 );
		fwprintf( fp, L"	return L\"NoString\";" );									__NewLine( fp, 1 );
		fwprintf( fp, L"}" );															__NewLine( fp, 1 );


		fclose( fp );

		CsMessageBox( MB_OK, L"%s 헤더 파일 생성 완료", _THAILAND_PATH_CPP );
	}
	
	//=========================================================================================================
	//
	//		종료
	//
	//=========================================================================================================
	nsCsFileTable::g_pLocalizingMng->Delete();
	SAFE_DELETE( nsCsFileTable::g_pLocalizingMng );	
}

void CLan_To_HeaderDlg::OnBnClickedButtonSetdir()
{
	TCHAR szOut[ MAX_PATH ] = { 0, };
	if( nsCSFILE::DirectoryOpen( szOut, MAX_PATH, m_Resist.s_szWorkDir ) == false )
		return;

	_tcscpy_s( m_Resist.s_szWorkDir, MAX_PATH, szOut );
	m_edit_WorkDir.SetWindowText( m_Resist.s_szWorkDir );
	m_Resist.Save();
}



CLan_To_HeaderDlg::CLan_To_HeaderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLan_To_HeaderDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLan_To_HeaderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_DIR, m_edit_WorkDir);
}

BEGIN_MESSAGE_MAP(CLan_To_HeaderDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_SETDIR, &CLan_To_HeaderDlg::OnBnClickedButtonSetdir)
	ON_BN_CLICKED(IDC_BUTTON_ENGLISH, &CLan_To_HeaderDlg::OnBnClickedButtonEnglish)
	ON_BN_CLICKED(IDC_BUTTON_THAILAND, &CLan_To_HeaderDlg::OnBnClickedButtonThailand)
	ON_BN_CLICKED(IDC_BUTTON_TOTALEXCEL, &CLan_To_HeaderDlg::OnBnClickedButtonTotalexcel)
	ON_BN_CLICKED(IDC_BUTTON_TEMPQUEST, &CLan_To_HeaderDlg::OnBnClickedButtonTempQuest)
END_MESSAGE_MAP()


// CLan_To_HeaderDlg 메시지 처리기

BOOL CLan_To_HeaderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	__Init();

	

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLan_To_HeaderDlg::OnPaint()
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
HCURSOR CLan_To_HeaderDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CLan_To_HeaderDlg::PostNcDestroy()
{
	__Delete();

	CDialog::PostNcDestroy();
}



