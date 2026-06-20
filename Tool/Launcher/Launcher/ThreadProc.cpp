#include "StdAfx.h"
#include "ThreadProc.h"
#include <sstream>

CThreadProc::CThreadProc(void)
{
	m_nTry = 0;
	m_bStopThread = false;
	m_nErrorState = 0;
	ResetError();	
}

CThreadProc::~CThreadProc(void)
{
	DeleteQueue();	
}

void CThreadProc::DeleteQueue()
{
	sTHREAD_INPUT* pData;
	while( m_Queue.Get( pData ) )
	{
		delete pData;
	}	
}

DWORD CThreadProc::ThreadProc( THREAD_PROC_ARG* pArg )
{
	CsEvent *pevStop = pArg->pevStop;

	while( TRUE )
	{
		DWORD dwRet;
		if( *pArg->m_pbStop == TRUE )		
			break;		
		else
			dwRet = pevStop->Wait( 10 );

		// Stop Event가 Signal 되었는가?
		if( CsKWaitSucceeded( dwRet, 1 ) )
		{
			// 스레드를 종료한다.			
			break;
		}
		else if( CsKWaitTimeout( dwRet ) )
		{
			sTHREAD_INPUT* pThreadData = NULL;
			m_Queue.Get( pThreadData );
			if( pThreadData != NULL )
			{
				switch( pThreadData->s_eType )
				{
#ifdef USE_DOWNENGINE
				case sTHREAD_INPUT::INSTALL:				
					{
						while( IsStopThread() == false )
						{
							if( g_pDlg->IsInstllEnd() == true )
							{
								g_pDlg->Install_End();
								break;
							}
						}
					}
					break;	
#else
				case sTHREAD_INPUT::DOWNLOADED_INSTALL:
					{
						while( IsStopThread() == false )
						{
							if( g_pDlg->IsInstllEnd() == true )
							{
								g_pDlg->Install_End();
								break;
							}
						}
					}
					break;
				case sTHREAD_INPUT::INSTALL:				
					{
						while( IsStopThread() == false )
						{
							_DownLoad( pThreadData );
							_DownLoadCheckFile( pThreadData );
							if( _ErrorCheck() )
								g_pDlg->_FullClientDownLoadComplete();
							break;
						}
					}
					break;			
#endif
				case sTHREAD_INPUT::UPDATE:
					{
						while( IsStopThread() == false	)
						{
							if( m_nTry <= MAX_TRY )
							{						
								g_pDlg->m_strGameState->ChangeStr( STR_IN_DOWN );							

								_DownLoad( pThreadData );
								_DownLoadCheckFile( pThreadData );
								if( _ErrorCheck() == true )
									break;
							}
							else
							{
								// 시도 횟수 초과								
								break;								
							}							
						}

						while( IsStopThread() == false	)
						{
							if( m_nTry <= MAX_TRY )
							{
								g_pDlg->m_strGameState->ChangeStr( STR_IN_INSTALL );								

								_Unzip( pThreadData );
								if( _ErrorCheck() == true )
								{
									break;
								}
							}
							else
							{
								// 시도 횟수 초과								
								break;								
							}							
						}

						if( IsStopThread() == false )
						{
							bool bSuccess = false;
#ifdef USE_THEMIDA_SDK
							VM_START
#endif
							if( __DoPackOnlyOne( pThreadData ) )
							{
								g_pDlg->UpdateVersion();
								bSuccess = true;
							}

#ifdef USE_THEMIDA_SDK
							VM_END
#endif

							if( bSuccess == false )
							{
								StopThread();
								g_pDlg->ReturnVersion();
								g_pDlg->Quit( STR_FAIL_PACK );
							}
							else
							{
								CThreadProc::sINFO Info;
								GetTotalInfo( &Info );
								if( Info.s_nProcSize >= Info.s_nTotalSize )
									g_pDlg->Update_End();
							}
						}
						else
						{// 다운로드중 취소시 다운로드 되고 있는 파일이 남아 있으면 지운다.

							if( pThreadData )
							{
								CString	Downfilename;
								
								Downfilename.Format( _T( "%s\\%s" ), g_GameInfo->GetGameInfo()->s_szGameDirectory, pThreadData->s_szDownFile );
								::DeleteFile( Downfilename );
							}
						}
					}
					break;	
				case sTHREAD_INPUT::DO_PACK:
					{
						g_pDlg->m_strGameState->ChangeStr( STR_IN_PACK );

						CThreadProc::sINFO Info;
						GetTotalInfo( &Info );
						if( Info.s_nProcSize >= Info.s_nTotalSize )
							g_pDlg->Pack_End();
					}
					break;
				}
			}
						
			SAFE_DELETE( pThreadData );			
		}
		else
		{
			// wait가 실패되었으므로 에러메시지를 알린다.
			CsKThrowError( GetLastError() );
			break;
		}

		if( IsStopThread() == false )
		{
			if( m_nTry >= MAX_TRY )
			{
				TCHAR sz[ MAX_PATH ] = { 0, };			
				_stprintf_s( sz, MAX_PATH, STR_FAIL_INSTALL, m_nErrorState );

				g_pDlg->Quit( sz );							
				break;
			}
		}
	}
	return 0L;
}

void CThreadProc::_DownLoad( sTHREAD_INPUT* pData )
{
	// URL (파일 주소 연결)
	CString strURL; 	
	strURL += pData->s_szURLPath;
	strURL += pData->s_szDownFile;
	HINTERNET hHttpFile = InternetOpenUrl( *m_hSession, strURL, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	if( hHttpFile == NULL )
	{
		ThrowError( URL_NOTCONNECT_INTERNET );	
		InternetCloseHandle( hHttpFile );
		return;
	}

	// 연결 정보 확인
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
			ThrowError( URL_NOT_FILE );
			InternetCloseHandle( hHttpFile );	
			return;
		}

	}

	// 파일 용량 확인
	TCHAR bufQuery[ 32 ];
	DWORD dwLengthBufQuery = sizeof(bufQuery);
	BOOL bConnect = ::HttpQueryInfo( hHttpFile, HTTP_QUERY_CONTENT_LENGTH, bufQuery, &dwLengthBufQuery, NULL);

	DWORD dwTotalSize = _ttol( bufQuery );
	DWORD dwReadByte = 0;
	m_CurInfo.Reset();
	SetCurTotalSize( dwTotalSize );	

	// 다운받을 파일 생성
	CString			filename;
#ifdef USE_DOWNENGINE
	filename.Format( _T( "%s/%s" ), g_GameInfo->GetGameInfo()->s_szGameDirectory, pData->s_szDownFile )	;
#else
	switch( pData->s_eType )
	{
	case sTHREAD_INPUT::INSTALL:
		filename.Format( _T( "%s" ), g_pDlg->m_szFullDown )	;
		break;
	case sTHREAD_INPUT::UPDATE:
		filename.Format( _T( "%s/%s" ), g_GameInfo->GetGameInfo()->s_szGameDirectory, pData->s_szDownFile )	;
		break;
	}
#endif

	CFile pFile;	
	BOOL bSuccess = pFile.Open( filename, CFile::modeCreate | CFile::modeWrite );
	if( bSuccess == FALSE )
	{
		ThrowError( URL_OPENFILE_ERROR );
		InternetCloseHandle( hHttpFile );		
		return;
	}
	
	TCHAR sz[ MAX_PATH ] = { 0, };
	TCHAR szArg[ MAX_PATH ] = { 0, };

	if( m_nTry == 0 )
		g_pDlg->m_strGameFile->ChangeStr( pData->s_szDownFile );
	else
	{
		_tcscat_s( szArg, MAX_PATH, pData->s_szDownFile );
		_tcscat_s( szArg, MAX_PATH, STR_IN_RETRY );
		_stprintf_s( sz, MAX_PATH, szArg, m_nTry );
		g_pDlg->m_strGameFile->ChangeStr( sz );
	}	

	DWORD dwRead = 0;
	TCHAR* data = (TCHAR*)malloc( 65536 );
		
	while ( IsStopThread() == false )
	{
		InternetReadFile( hHttpFile, data, 65535, &dwRead );
		if ( dwRead <= 0)
		{
			break;
		}
		dwReadByte+=dwRead;		
		pFile.Write( data, dwRead );	

		SetAddCurProcSize( dwRead );		
		SetAddTotalProcSize( dwRead );	
	}	
	free(data);
	InternetCloseHandle( hHttpFile );		
}

bool CThreadProc::_Unzip( sTHREAD_INPUT* pData )
{	
	g_pDlg->m_strGameState->ChangeStr( STR_IN_INSTALL );

	Sleep(200);

	CString	Downfilename;
	Downfilename.Format( _T( "%s\\%s" ), g_GameInfo->GetGameInfo()->s_szGameDirectory, pData->s_szDownFile );
 
	CT2CA tempString(Downfilename);
	std::string unzipFile(tempString);

	DmCS::CUnzipper unZip;
	if (!unZip.OpenZip(unzipFile.c_str()))
	{
		StopThread();
		::DeleteFile(Downfilename);
		TCHAR szArg[ MAX_PATH ] = { 0, };
		_stprintf_s( szArg, MAX_PATH, UNZIP_FILE_OPEN_ERROR, pData->s_szDownFile );
		g_pDlg->Quit( szArg );
		return false;
	}
	
	int nFileCount = unZip.GetFileCount();		
	if( pData->s_eType == sTHREAD_INPUT::INSTALL )
	{
		m_TotalInfo.Reset();
		SetTotalTotalSize( nFileCount );
	}

	for (int n = 0; n < nFileCount; ++n)
	{
		DmCS::UZ_FileInfo info;
		unZip.GetFileInfo(n, info);
		CString file(info.szFileName);

		if (info.bFolder)
			continue;

		if (m_nTry == 0)
			g_pDlg->m_strGameFile->ChangeStr(file.GetBuffer());
		else
		{
			TCHAR sz[MAX_PATH] = _T("");
			TCHAR szArg[MAX_PATH] = _T("");
			_tcscat_s(szArg, MAX_PATH, file.GetBuffer());
			_tcscat_s(szArg, MAX_PATH, STR_IN_RETRY);
			_stprintf_s(sz, MAX_PATH, szArg, m_nTry);
			g_pDlg->m_strGameFile->ChangeStr(sz);
		}

		if( !unZip.UnzipFile(n) )
		{
			StopThread();
			TCHAR szArg[ MAX_PATH ] = { 0, };
			_stprintf_s( szArg, MAX_PATH, UNZIP_ERROR, file.GetBuffer());
			g_pDlg->Quit( szArg );
			break;
		}
	}

	unZip.CloseZip();
	::DeleteFile( Downfilename );
	return true;
}

void CThreadProc::_DownLoadCheckFile( sTHREAD_INPUT* pData )
{
	g_pDlg->m_strGameState->ChangeStr( STR_IN_FILE_CHECK );

	CString FileName;
#ifdef USE_DOWNENGINE
	FileName.Format( _T( "%s\\%s" ), g_GameInfo->GetGameInfo()->s_szGameDirectory, pData->s_szDownFile  );
#else
	switch( pData->s_eType )
	{
	case sTHREAD_INPUT::INSTALL:
		FileName.Format( _T( "%s" ), g_pDlg->m_szFullDown )	;
		break;
	case sTHREAD_INPUT::UPDATE:
		FileName.Format( _T( "%s/%s" ), g_GameInfo->GetGameInfo()->s_szGameDirectory, pData->s_szDownFile )	;
		break;
	}
#endif
	//FileName.Format( _T( "%s\\%s" ), g_GameInfo->GetGameInfo()->s_szGameDirectory, pData->s_szDownFile  );
	HANDLE hFile = CreateFile( FileName, 0, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		// 해당파일 다운로드 실패
		ThrowError( URL_FILE_ERROR );
		CloseHandle( hFile );
		return;
	}
	else
	{
		DWORD dwFileSize = 0;
		dwFileSize = GetFileSize( hFile, NULL );		
		
		if( dwFileSize != pData->s_dwSize )
		{
			ThrowError( URL_FILE_ERROR );
			CloseHandle( hFile );
			return;
		}
		else
		{
			ResetError();
		}
	}	
	CloseHandle( hFile );
}

void CThreadProc::ThrowError( int nErrorState )
{
	m_nErrorState = nErrorState;
}

void CThreadProc::ResetError()
{
	m_nErrorState = 0;
}

bool CThreadProc::_ErrorCheck()
{
	if( m_nErrorState != 0 )
	{
		m_nTry++;
		return false;
	}	
	else
	{
		ResetError();
		m_nTry = 0;
	}

	return true;
}

void _MakePatchFileList( std::list<std::string> const& orgList, std::list<std::pair<std::string,std::string>>& makeList, const char * pCutStr )
{
	std::list<std::string>::const_iterator it = orgList.begin();
	for( ; it != orgList.end(); ++it )
	{
		char* pPath = StrStrIA( (*it).c_str(), pCutStr );
		makeList.push_back(make_pair( (*it), pPath ));
	}
}

// 파일을 패킹하는 함수
bool CThreadProc::_DoPack( sTHREAD_INPUT* pTU )
{
	char pGameDir[MAX_PATH] = {0,};
	W2M( pGameDir, g_GameInfo->GetGameInfo()->s_szGameDirectory, MAX_PATH);

	std::string fileFolder(pGameDir);
	fileFolder += ASB_FD_MARK;
	fileFolder += RESOURCE_PACK_NAME;

	std::list<std::string> findFiles;
	nsCSFILE::DirectoryFindFilesA( findFiles, fileFolder.c_str() );

	std::list<std::pair<std::string,std::string>> PackPath;
	_MakePatchFileList( findFiles, PackPath, DEFAULT_FOLDER_NAME );

	// 파일들을 패치 한다.
	size_t fileCount = findFiles.size();
	if( 0 == fileCount )// 패킹할 파일이 없음.
		return true;

	std::stringstream packFile;
	packFile << pGameDir << ASB_FD_MARK << DEFAULT_FOLDER_NAME << ASB_FD_MARK << RESOURCE_PACK_NAME;
	std::string file = packFile.str();

	// 패킹 파일 오픈
	if( !CsFPS::CsFPSystem::Initialize( true, file, true ) )
	{
		CsFPS::CsFPSystem::Destroy();
		ThrowError( FP_OPEN_ERROR );
		StopThread();
		return false;
	}

	g_pDlg->m_strGameFile->ChangeStr( L"Pack01 패킹중" );

	SetTotalTotalSize( fileCount );

	int nNewFileCount = 0;
	std::list<std::pair<std::string,std::string>>::iterator it = PackPath.begin();
	for( ; it != PackPath.end(); ++it )
	{
		if( !CsFPS::CsFPSystem::IsExist( it->second.c_str() ) )
			nNewFileCount++;
	}

	CsFPS::CsFPSystem::AddAllocCount( nNewFileCount );
 
	bool bError = false;
	it = PackPath.begin();
	for( ; it != PackPath.end(); ++it )
	{
		m_CurInfo.Reset();
		SetCurTotalSize( 1 );

		if( !CsFPS::CsFPSystem::EditFile( it->first.c_str(), it->second.c_str() ) )
		{
			bError = true;
			break;
		}
		SetAddCurProcSize( 1 );
		SetAddTotalProcSize( 1 );
	}

	CsFPS::CsFPSystem::SaveHashPack();
	CsFPS::CsFPSystem::Destroy();

	// 파일 오픈 실패시 - 에러 발생시 ( 저장된거까지만의 헤쉬정보만 저장하고 에러 메세지 발생시키자 )
	if( bError == true )
	{
		ThrowError( FP_OPEN_ERROR );
		StopThread();
		return false;
	}

	// 해당 폴더 제거
	// 패킹이 끝났으면 팩할 파일들을 지운다.
	nsCSFILE::DeleteFolder( fileFolder.c_str() );
	return true;
}

// 팩킹된 파일을 패치 하는 함수
bool CThreadProc::__DoPack( sTHREAD_INPUT* pTU )
{
	g_pDlg->m_strGameFile->ChangeStr( L"Pack01 패킹중" );

	char pGameDir[MAX_PATH] = {0,};
	W2M( pGameDir, g_GameInfo->GetGameInfo()->s_szGameDirectory, MAX_PATH);

	std::string fileFolder(pGameDir);
	fileFolder += ASB_FD_MARK;	
	fileFolder += DEFAULT_FOLDER_NAME;
	return true;
// 
// 	// 폴더에서 패치 할 파일이 있는지 검사 한다.
// 	size_t fileCount = FPMNG_PTR->LoadPatchFile_PackList( fileFolder.c_str() );
// 	if( 0 == fileCount )
// 	{	// 패킹할 파일이 없음.
// 		FPMNG_PTR->Delete();
// 		return true;
// 	}
// 
// 	std::stringstream packFile;
// 	packFile << pGameDir << ASB_FD_MARK << DEFAULT_FOLDER_NAME << ASB_FD_MARK << RESOURCE_PACK_NAME;
// 	std::string file = packFile.str();
// 	// 패킹할 파일을 로드.
// 	if( !FPMNG_PTR->LoadFilePack( file.c_str(), true ))
// 	{
// 		FPMNG_PTR->Delete();
// 		ThrowError( FP_OPEN_ERROR );
// 		return false;
// 	}
// 
// 	// 패치 파일에서 기존에 없었던 새로운 파일이 있는지 체크 한다.
// 	if( ERROR_OK != FPMNG_PTR->CheckNewFileCount_Pack() )
// 	{
// 		FPMNG_PTR->Delete();
// 		ThrowError( FP_OPEN_ERROR );
// 		return false;
// 	}
// 
// 	// 새로 패치되는 파일의 갯수 만큼 패킹 파일의 사이즈를 추가 한다.
// 	if(!FPMNG_PTR->SetAllocCount_Pack())
// 	{
// 		FPMNG_PTR->Delete();
// 		ThrowError( FP_OPEN_ERROR );
// 		return false;
// 	}	
// 
// 	bool bError = false;
// 	MAP_FILEINFO const& listFileInfo = FPMNG_PTR->GetPatchListInfo();
// 	MAP_FILEINFO_CIT it = listFileInfo.begin();
// 	for( ; it != listFileInfo.end(); ++it )
// 	{
// 		m_CurInfo.Reset();
// 		SetCurTotalSize( 1 );
// 
// 		std::map< size_t, CsFileHash::sINFO* >* pMap = it->second.m_pFileHash->GetHashMap();
// 		std::map< size_t, CsFileHash::sINFO* >::iterator mapIt = pMap->begin();
// 		std::map< size_t, CsFileHash::sINFO* >::iterator mapItEnd = pMap->end();
// 		for( ; mapIt != mapItEnd; ++mapIt )
// 		{
// 			if( FPMNG_PTR->EditFile( it->second.m_pFilePack, mapIt->second ) == false )
// 			{
// 				bError = true;
// 				break;
// 			}
// 		}
// 
// 		SetAddCurProcSize( 1 );
// 		SetAddTotalProcSize( 1 );
// 	}
// 
// 	if( !bError )
// 		FPMNG_PTR->SaveHashFile();
// 
// 	FPMNG_PTR->Delete();

// 	if( bError )
// 	{	// 파일 오픈 실패시 - 에러 발생시 ( 저장된거까지만의 헤쉬정보만 저장하고 에러 메세지 발생시키자 )
// 		ThrowError( FP_OPEN_ERROR );
// 		return false;
// 	}
}

// 팩킹된 파일을 패치 버젼 별로 패치 하는 함수
bool CThreadProc::__DoPackOnlyOne( sTHREAD_INPUT* pTU )
{
	if( NULL == pTU )
		return false;

	g_pDlg->m_strGameFile->ChangeStr( L"Pack01 패킹중" );

	char pGameDir[MAX_PATH] = {0,};
	W2M( pGameDir, g_GameInfo->GetGameInfo()->s_szGameDirectory, MAX_PATH);

	std::stringstream patchPackFile;
	patchPackFile << pGameDir << ASB_FD_MARK << DEFAULT_FOLDER_NAME << ASB_FD_MARK << PATCH_PACK_NAME << pTU->s_dwPatchNum;
	if( !CsFPS::CsFPSystem::IsExistPackFile( patchPackFile.str().c_str() ) )
		return true;

	// Pack01 파일이 있는지 체크 업으면 실패
	std::stringstream packFile;
	packFile << pGameDir << ASB_FD_MARK << DEFAULT_FOLDER_NAME << ASB_FD_MARK << RESOURCE_PACK_NAME;	
	if( !CsFPS::CsFPSystem::IsExistPackFile( packFile.str().c_str() ) )
		return false;

	std::list<std::string> loadPack;
	loadPack.push_back( packFile.str() );
	loadPack.push_back( patchPackFile.str() );

	if( !CsFPS::CsFPSystem::Initialize( true, loadPack, true ) )
	{
		CsFPS::CsFPSystem::Destroy();

		ThrowError( FP_OPEN_ERROR );
		StopThread();
		return false;
	}

	size_t nNewCount = CsFPS::CsFPSystem::CheckNewFileCount(0, 1);
	bool bResult = CsFPS::CsFPSystem::AddAllocCount( nNewCount );

	if( bResult )
		bResult = CsFPS::CsFPSystem::EditFile(0, 1);

	CsFPS::CsFPSystem::SaveHashPack();
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
		StopThread();
		return false;
	}
	return true;
}

void CThreadProc::SetCurTotalSize( ULONGLONG nSize )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	m_CurInfo.s_nTotalSize = nSize;
}

void CThreadProc::SetAddCurProcSize( ULONGLONG nSize )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	m_CurInfo.s_nProcSize += nSize; // 현재 사이즈는 누적이다
}

void CThreadProc::SetCurProcSize( ULONGLONG nSize )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	m_CurInfo.s_nProcSize = nSize; // 현재 사이즈는 누적이다
}

void CThreadProc::GetCurInfo( sINFO* pDest )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	memcpy( pDest, &m_CurInfo, sizeof( sINFO ) );
}

void CThreadProc::ResetCurInfo()
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	memset( &m_CurInfo, 0, sizeof( sINFO ) );
}

void CThreadProc::SetTotalTotalSize( ULONGLONG nSize )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	m_TotalInfo.s_nTotalSize = nSize;
}

void CThreadProc::SetAddTotalProcSize( ULONGLONG nSize )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	m_TotalInfo.s_nProcSize += nSize; // 현재 사이즈는 누적이다
}

void CThreadProc::SetTotalProcSize( ULONGLONG nSize )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	m_TotalInfo.s_nProcSize = nSize; // 현재 사이즈는 누적이다
}

void CThreadProc::GetTotalInfo( sINFO* pDest )
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	memcpy( pDest, &m_TotalInfo, sizeof( sINFO ) );
}

void CThreadProc::ResetTotalInfo()
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	memset( &m_TotalInfo, 0, sizeof( sINFO ) );
}

void CThreadProc::StopThread()
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	m_bStopThread = true;
}

bool CThreadProc::IsStopThread()
{
	_SYNCHRONIZE_THREAD_(&m_CS);
	return m_bStopThread;	
}




