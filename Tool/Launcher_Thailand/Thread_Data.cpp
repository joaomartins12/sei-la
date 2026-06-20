
#include "stdafx.h"
#include "Thread_Data.h"
#include <io.h>
#include <WinInet.h>

#include <shlwapi.h>
#include <sstream>
#include "IniFile.h"

CThread_Data::CThread_Data()
{
	m_hSession = NULL;
	m_nErrorState = 0;
	ResetError();
}

CThread_Data::~CThread_Data()
{
	DeleteQueue();
	//	_DeleteFilePackList();
}

void CThread_Data::ResetError()
{
	m_nErrorState = 0;
}


void CThread_Data::DeleteQueue()
{
	sTHREAD_INPUT* pTU;
	while( m_Queue.Get( pTU ) )
	{
		delete pTU;
	}	
}

DWORD CThread_Data::ThreadProc( THREAD_PROC_ARG* pArg )
{	
	CsEvent *pevStop = pArg->pevStop;

	while( TRUE )
	{
		if( *pArg->m_pbStop == TRUE )
		{
			break;
		}
		else
		{
			pevStop->Wait( 10 );
		}

		if( GetVersion() == U_PROCESS )
		{
			// 쓰레드 실제 처리 구문
			if( m_Queue.Empty() == false )
			{
				sTHREAD_INPUT* pTU;
				m_Queue.Get( pTU );

				// 초기화
				m_nErrorState = 0;
				ResetInfo();

				switch( pTU->s_eType )
				{
					// 압축 해제
				case sTHREAD_INPUT::UNZIP:			_Unzip( pTU );		break;
				case sTHREAD_INPUT::URL_DOWN:		_URLDown( pTU );	break;
#ifdef PATCH_PACK
				case sTHREAD_INPUT::DO_PACK:		__DoPackOnlyOne(pTU); break;
#else
				case sTHREAD_INPUT::DO_PACK:		_DoPack( pTU );		break;
#endif
				default:							assert( false );
				}
				_CheckError( pTU );
				delete pTU;
			}
			else
			{
				Sleep( 100 );
			}
		}
		else
		{
			Sleep( 100 );
		}
	}
	return 0L;
}


void CThread_Data::Add( sTHREAD_INPUT* pTU )
{
	assert( GetVersion() == U_WAIT );

	sTHREAD_INPUT* pNew = new sTHREAD_INPUT;
	*pNew = *pTU;
	m_Queue.Put( pNew );

	SetVersion( U_PROCESS );
}

bool CThread_Data::_Unzip( sTHREAD_INPUT* pTU )
{	
	Sleep( 200 );

	CString			filename;	
	filename.Format(_T("%s%s%d.zip"), pTU->s_szDownPath, FILE_WARD, pTU->s_nVersion);

	SetVersion( pTU->s_nVersion );	

	CT2CA tempString(filename);
	std::string unzipFile(tempString);

	DmCS::CUnzipper unZip;
	if (!unZip.OpenZip(unzipFile.c_str()))
	{
		ThrowError(UNZ_OPENFILE_ERROR);
		CsMessageBox(MB_OK, ERROR_ZIP_OPEN_FALSE, filename.GetBuffer());
		return false;
	}

	int nFileCount = unZip.GetFileCount();
	SetTotalSize(nFileCount * 100);

	for (int n = 0; n < nFileCount; ++n)
	{
		DmCS::UZ_FileInfo info;
		unZip.GetFileInfo(n, info);

		if( info.bFolder )
			continue;

		if (!unZip.UnzipFile(n))
		{
			ThrowError(UNZ_BADZIPFILE);
			CString file(info.szFileName);
			CsMessageBox(MB_OK, ERROR_UNZIP_FALSE, file.GetBuffer());
			break;
		}

		SetProcSize((n + 1) * 100);
	}
	unZip.CloseZip();
	::DeleteFile(filename);

// 	CDmZip unZip;
// 	HZIP hZip = unZip.UnZipOpen( filename.GetBuffer() );
// 	if( NULL == hZip )
// 	{
// 		ThrowError( UNZ_OPENFILE_ERROR );
// 		CsMessageBox( MB_OK, ERROR_ZIP_OPEN_FALSE, filename.GetBuffer() );
// 		return false;
// 	}
// 
// 	std::wstring baseDir = unZip.GetUnZipBaseDir();
// 
// 	ZIPENTRY ze;
// 	GetZipItem( hZip, -1, &ze );
// 	int nFileCount = ze.index;
// 
// 	SetTotalSize( nFileCount * 100 );
// 
// 	for( int i=0; i < nFileCount; ++i )
// 	{ 
// 		GetZipItem( hZip, i, &ze);		
// 
// 		std::wstring fullPathDir = baseDir;
// 		fullPathDir += ze.name;
// 		::SetFileAttributesW( fullPathDir.c_str(), FILE_ATTRIBUTE_NORMAL );
// 
// 		if( ZR_OK != UnzipItem( hZip, i, ze.name) )
// 		{
// 			ThrowError( UNZ_BADZIPFILE );
// 			CsMessageBox( MB_OK, ERROR_UNZIP_FALSE, ze.name );
// 			break;
// 		}
// 
// 		SetProcSize( (i + 1) * 100 );
// 	}
// 
// 	CloseZip(hZip);
// 	::DeleteFile( filename );	

	return true;
}

void CThread_Data::_URLDown( sTHREAD_INPUT* pTU )
{
	HINTERNET hHttpFile = InternetOpenUrl( *m_hSession, pTU->s_szURLPath, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	if( hHttpFile == NULL )
	{
		ThrowError( URL_NOT_FILE );
		return;
	}

	TCHAR szStatusCode[ 128 ];
	DWORD dwCodeSize = 128;
	HttpQueryInfo( hHttpFile, HTTP_QUERY_STATUS_CODE, szStatusCode, &dwCodeSize, NULL );
	long nStatusCode = _ttol( szStatusCode );
	if( nStatusCode != HTTP_STATUS_OK )
	{
		if( nStatusCode == HTTP_STATUS_NOT_FOUND )
		{
			ThrowError( URL_NOT_FILE );
			InternetCloseHandle( hHttpFile );
			return;
		}
		else
		{
			TCHAR cMsg[ 128 ];
			_stprintf_s( cMsg, 128, STR_FAIL_CONNECT_SERVER, nsCSFILE::GetFileName(pTU->s_szURLPath), nStatusCode );
			MessageBox( NULL, cMsg, STR_FAIL, MB_OK );
			ThrowError( URL_NOT_FILE );
			InternetCloseHandle( hHttpFile );
			return;
		}
	}

	TCHAR bufQuery[ 32 ];
	DWORD dwLengthBufQuery = sizeof(bufQuery);
	BOOL bConnect = ::HttpQueryInfo( hHttpFile, HTTP_QUERY_CONTENT_LENGTH, bufQuery, &dwLengthBufQuery, NULL);

	DWORD dwTotalSize = _ttol( bufQuery );

	DWORD dwReadByte = 0;
	SetTotalSize( dwTotalSize );
	SetProcSize( dwReadByte );

	CFile pFile;
	BOOL bSuccess = pFile.Open( pTU->s_szDownPath, CFile::modeCreate | CFile::modeWrite );
	if( bSuccess == FALSE )
	{
		ThrowError( URL_OPENFILE_ERROR );
		InternetCloseHandle( hHttpFile );		
		return;
	}

	switch( pTU->s_nVersion )
	{
	case sTHREAD_INPUT::VU_VERSION:
		g_pDlg->SetDownState( CAutoUpgradeDlg::US_VERSION_INFO, dwTotalSize );
		break;
	case sTHREAD_INPUT::VU_DOWN:
		g_pDlg->SetDownState( CAutoUpgradeDlg::US_FILE_INFO, dwTotalSize );
		break;
	default:
		assert( false );
	}

	DWORD dwRead = 0;
	TCHAR* data = (TCHAR*)malloc( 65536 );

	switch( pTU->s_nVersion )
	{
	case sTHREAD_INPUT::VU_VERSION:
		g_pDlg->SetDownState( CAutoUpgradeDlg::US_VERSION_START, 0 );
		break;
	case sTHREAD_INPUT::VU_DOWN:
		g_pDlg->SetDownState( CAutoUpgradeDlg::US_FILE_START, 0 );
		break;
	default:
		assert( false );
	}

	while (1)
	{
		InternetReadFile( hHttpFile, data, 65536, &dwRead );
		if ( dwRead <= 0)
		{
			break;
		}
		dwReadByte+=dwRead;		
		pFile.Write( data, dwRead );

		SetProcSize( dwReadByte );
	}
	free(data);
	InternetCloseHandle( hHttpFile );	
}

void _MakePatchFileList( std::list<std::string> const& orgList, std::list<std::pair<std::string,std::string>>& makeList, const char * pCutStr )
{
	std::list<std::string>::const_iterator it = orgList.begin();
	for( ; it != orgList.end(); ++it )
	{
		char* pPath = StrStrIA( (*it).c_str(), pCutStr );
		if( NULL == pPath )
			continue;
		makeList.push_back(make_pair( (*it), pPath ));
	}
}

void CThread_Data::_DoPack( sTHREAD_INPUT* pTU )
{
	std::string fileFolder;
	fileFolder = RESOURCE_PACK_NAME;

	std::list<std::string> findFiles;
	nsCSFILE::DirectoryFindFilesA( findFiles, fileFolder.c_str() );

	std::list<std::pair<std::string,std::string>> PackPath;
	_MakePatchFileList( findFiles, PackPath, DEFAULT_FOLDER_NAME );

	// 파일들을 패치 한다.
	size_t fileCount = findFiles.size();
	if( 0 == fileCount )// 패킹할 파일이 없음.
	{
		nsCSFILE::DeleteDirectoryA( fileFolder.c_str() );
		return ;
	}

	std::stringstream packFile;
	packFile << DEFAULT_FOLDER_NAME << ASB_FD_MARK << RESOURCE_PACK_NAME;
	std::string file = packFile.str();

	// 패킹 파일 오픈
	if( !CsFPS::CsFPSystem::Initialize( true, file, true ) )
	{
		CsFPS::CsFPSystem::Destroy();
		ThrowError( FP_OPEN_ERROR );
		return ;
	}


	SetTotalSize( fileCount );

	int nNewFileCount = 0;
	std::list<std::pair<std::string,std::string>>::iterator it = PackPath.begin();
	for( ; it != PackPath.end(); ++it )
	{
		if( !CsFPS::CsFPSystem::IsExistOnlyPack( 0, it->second.c_str() ) )
			nNewFileCount++;
	}

	bool bError = !CsFPS::CsFPSystem::AddAllocCount( nNewFileCount );

	if( !bError )
	{	
		it = PackPath.begin();
		for( int n = 1 ; it != PackPath.end(); ++it, ++n )
		{
			SetProcSize( n );

			if( !CsFPS::CsFPSystem::EditFile( it->first.c_str(), it->second.c_str() ) )
			{
				bError = true;
				break;
			}
		}
	}

	// 파일 오픈 실패시 - 에러 발생시 ( 저장된거까지만의 헤쉬정보만 저장하고 에러 메세지 발생시키자 )
	CsFPS::CsFPSystem::SaveHashPack();
	CsFPS::CsFPSystem::Destroy();

	if( bError == true )
	{
		ThrowError( FP_OPEN_ERROR );
		return ;
	}

	// 해당 폴더 제거
	// 패킹이 끝났으면 팩할 파일들을 지운다.
	nsCSFILE::DeleteDirectoryA( fileFolder.c_str() );
}

void CThread_Data::__DoPackOnlyOne( sTHREAD_INPUT* pTU )
{
	if( NULL == pTU )
		return ;

	std::string fileFolder;
	fileFolder = DEFAULT_FOLDER_NAME;

	std::stringstream patchPackFile;
	patchPackFile << DEFAULT_FOLDER_NAME << ASB_FD_MARK << PATCH_PACK_NAME << pTU->s_dwPatchNum;
	if( !CsFPS::CsFPSystem::IsExistPackFile( patchPackFile.str().c_str() ) )
		return ;

	std::list<std::string> loadPack;
	std::stringstream packFile;
	packFile << ASB_FD_MARK << DEFAULT_FOLDER_NAME << ASB_FD_MARK << RESOURCE_PACK_NAME;	
	loadPack.push_back( packFile.str() );

	loadPack.push_back( patchPackFile.str() );

	if( !CsFPS::CsFPSystem::Initialize( true, loadPack, true ) )
	{
		CsFPS::CsFPSystem::Destroy();
		ThrowError( FP_OPEN_ERROR );
		return ;
	}

	size_t nNewCount = CsFPS::CsFPSystem::CheckNewFileCount(0, 1);
	bool bResult = CsFPS::CsFPSystem::AddAllocCount( nNewCount );

	if( bResult )
	{
		bResult = CsFPS::CsFPSystem::EditFile(0, 1);
		CsFPS::CsFPSystem::SaveHashPack();
	}

	CsFPS::CsFPSystem::Destroy();

	std::string delPackFile = patchPackFile.str();
	delPackFile += PACK_EX_NAME;

	std::string delHeaderFile = patchPackFile.str();
	delHeaderFile += HASH_EX_NAME;

	// 해당 폴더 제거
	// 패킹이 끝났으면 팩할 파일들을 지운다.
	nsCSFILE::DeleteFileA( delPackFile.c_str() );
	nsCSFILE::DeleteFileA( delHeaderFile.c_str() );

	if( !bResult )
	{	// 파일 오픈 실패시 - 에러 발생시 ( 저장된거까지만의 헤쉬정보만 저장하고 에러 메세지 발생시키자 )
		ThrowError( FP_OPEN_ERROR );
		return ;
	}
}

void CThread_Data::_CheckError( sTHREAD_INPUT* pTU )
{
	if( m_nErrorState != 0 )
	{
		SetVersion( m_nErrorState );
		switch( pTU->s_eType )
		{
		case sTHREAD_INPUT::UNZIP:
			{
				switch( m_nErrorState )
				{
				case UNZ_BADZIPFILE:
					{
						g_pDlg->PostMessage(WM_QUIT);
					}					
					break;
				case UNZ_OPENFILE_ERROR:
					{					
						g_pDlg->PostMessage(WM_QUIT);
					}					
					break;
				}
			}
			break;
		case sTHREAD_INPUT::URL_DOWN:
			switch( pTU->s_nVersion )
			{
			case sTHREAD_INPUT::VU_VERSION:
				g_pDlg->SetDownState( CAutoUpgradeDlg::US_VERSION_ERROR, 0 );
				break;
			case sTHREAD_INPUT::VU_DOWN:
				g_pDlg->SetDownState( CAutoUpgradeDlg::US_FILE_ERROR, 0 );
				break;
			default:
				assert( false );
			}			
			break;
		case sTHREAD_INPUT::DO_PACK:
			{
				switch( m_nErrorState )					
				{
				case FP_OPEN_ERROR:
					SetVersion( PACK_FAILED_OPENFILE );
					break;;
				}
			}
			break;
		default:
			assert( false );
		}		
		return;
	}

	switch( pTU->s_eType )
	{
	case sTHREAD_INPUT::UNZIP:
		{
#ifndef PATCH_PACK
			// 파일에 기입 ( 다른데서 오픈되면 안된다 )
			TCHAR cVersion[ 64 ];
			_stprintf_s( cVersion, 64, L"%d", pTU->s_nVersion );
			CString filePath;
			filePath.Format(_T("%s%s"), pTU->s_szDownPath, VERSIONINFO_FILE);
			CIniFile file;
			file.Init(filePath);
			file.WriteString( L"VERSION", L"version", cVersion );
			g_pDlg->SetLastPatchNumber( pTU->s_nVersion );
#endif

			// 쓰레드가 해야할일 종료 - 어쩌다 보니~ 한번에 하나의 일만 처리
			assert( m_Queue.Empty() == true );
			SetVersion( UNZ_SUCCESS );
		}
		break;
	case sTHREAD_INPUT::URL_DOWN:
		{
			// 쓰레드가 해야할일 종료 - 어쩌다 보니~ 한번에 하나의 일만 처리
			assert( m_Queue.Empty() == true );
			SetVersion( URL_SUCCESS );

			switch( pTU->s_nVersion )
			{
			case sTHREAD_INPUT::VU_VERSION:
				g_pDlg->SetDownState( CAutoUpgradeDlg::US_VERSION_END, 0 );
				break;
			case sTHREAD_INPUT::VU_DOWN:
				g_pDlg->SetDownState( CAutoUpgradeDlg::US_FILE_END, 0 );
				break;
			default:
				assert( false );
			}
		}
		break;
	case sTHREAD_INPUT::DO_PACK:
		{
#ifdef PATCH_PACK
			// 파일에 기입 ( 다른데서 오픈되면 안된다 )
			TCHAR cVersion[ 64 ];
			_stprintf_s( cVersion, 64, L"%d", pTU->s_nVersion );
			CString filePath;
			filePath.Format(_T("%s%s"), pTU->s_szDownPath, VERSIONINFO_FILE);
			CIniFile file;
			file.Init(filePath);
			file.WriteString( L"VERSION", L"version", cVersion );
			g_pDlg->SetLastPatchNumber( pTU->s_nVersion );
#endif
			SetVersion( PACK_SUCCESS );		
		}

		break;
	default:
		assert( false );
	}	
}

void CThread_Data::ResetInfo()
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	__m_CurInfo.Reset();
}

void CThread_Data::SetVersion( int nVersion )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	__m_CurInfo.s_nVersion = nVersion;
}

void CThread_Data::SetTotalSize( ULONG nSize )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	__m_CurInfo.s_nTotalSize = nSize;
}

void CThread_Data::SetProcSize( ULONG nSize )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	__m_CurInfo.s_nProcSize = nSize;
}

void CThread_Data::GetInfo( sINFO* pDest )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	memcpy( pDest, &__m_CurInfo, sizeof( sINFO ) );
}

int CThread_Data::GetVersion()
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	return __m_CurInfo.s_nVersion;
}
void CThread_Data::ThrowError( int nErrorState )
{
	m_nErrorState = nErrorState;
}