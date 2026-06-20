#include "stdafx.h"
#include "ResourceChecker.h"
#include "../../../SimpleSecurity/SecuritySelf.h"
#include "../../../FtpClient/FTPClient.h"

#define NEWLINE "\r\n"
#define TOKKEN ":"

struct sFTPErrorLog : public nsFTP::CFTPClient::CNotification
{
public:
	sFTPErrorLog() {};
	virtual ~sFTPErrorLog() {};

	virtual void OnSendCommand(const tstring& strCommand)
	{
		if( strCommand.length()==0 )
			return;

		if( strCommand.length()>4 && strCommand.substr(5)==_T("PASS ") )
			tErrorLog += _T("< PASS **********\r\n");
		else
		{
			tErrorLog +=_T("> ");
			tErrorLog += strCommand;
			tErrorLog +=_T(NEWLINE);
		}
	}

	virtual void OnResponse(const nsFTP::CReply& Reply)
	{
		if( Reply.Value().length()==0 )
			return;

		tErrorLog +=_T("< ");
		tErrorLog += Reply.Value();
		tErrorLog +=_T(NEWLINE);
	}

	std::string GetLog() 
	{
		std::string log;
		log.assign( tErrorLog.begin(), tErrorLog.end() );
		return log;
	}

	void clear()
	{
		tErrorLog.clear();
	}

private:
	tstring tErrorLog;
};

std::string makeSaveFile( std::string const& saveFile )
{
	char saveFileName[MAX_PATH] = {0,};
	strcat_s( saveFileName, MAX_PATH, saveFile.c_str() );

	SYSTEMTIME t;
	::GetLocalTime(&t);

	// 시간 문자열을 준비한다.
	char szTail[MAX_PATH] = {0,};
	sprintf_s(	szTail,"%04d-%02d-%02d %02d-%02d-%02d-%03d", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond , t.wMilliseconds );

	// 덤프 파일 이름 += 시간 문자열
	::lstrcatA(saveFileName, szTail);
	// 덤프 파일 이름 += 확장자
	::lstrcatA(saveFileName, ".txt");
	return saveFileName;
}

bool SaveSendFile( std::string const& saveFile, std::string const& saveValue )
{
	FILE *file = NULL;
	fopen_s(&file, saveFile.c_str(), "wb");
	if( NULL == file )
		return false;

	fwrite( saveValue.c_str(), sizeof( char ), saveValue.size(), file );
	fclose( file );
	return true;
}


sFTPErrorLog g_FTPLoger;

//////////////////////////////////////////////////////////////////////////
bool _FTPFileDownload( std::string const& sIp,			// 업로드 ftp IP
	unsigned int const& nPort,		// 업로드 ftp Port
	std::string const& sID,		// 업로드 ID
	std::string const& sPW,	// 업로드 PW
	std::string const& sPath,		// 업로드 폴더
	std::string const& sDownloadFile,
	std::string & downloadString)
{
	downloadString.clear();

	tstring wsIP;
	wsIP.assign( sIp.begin(), sIp.end() );

	tstring wsID;
	wsID.assign( sID.begin(), sID.end() );

	tstring wsPW;
	wsPW.assign( sPW.begin(), sPW.end() );

	tstring wsFilePath;
	std::string pathFile = sPath + sDownloadFile;
	wsFilePath.assign( pathFile.begin(), pathFile.end() );

	nsFTP::CLogonInfo logonInfoSource;
	logonInfoSource.SetHost( wsIP, nPort, wsID, wsPW );

	nsFTP::CFTPClient ftpClientsource;
	ftpClientsource.AttachObserver( &g_FTPLoger );
	if( !ftpClientsource.Login( logonInfoSource ) )
		return false;

	nsFTP::COutputStream outputStream(_T(NEWLINE), _T("fileName"));
	// do file operations
	if( !ftpClientsource.DownloadFile(wsFilePath, outputStream, nsFTP::CRepresentation(nsFTP::CType::Image()), true) )
	{
		ftpClientsource.Logout();
		return false;
	}

	tstring output = outputStream.GetBuffer();

	downloadString.assign(output.begin(), output.end());

	// disconnect
	ftpClientsource.Logout();
	return true;
}

// URL 로부터 해쉬 데이터 파일 받음
bool _FTPFileUpload( std::string const& sIp,			// 업로드 ftp IP
	unsigned int const& nFtpPort,		// 업로드 ftp Port
	std::string const& userId,		// 업로드 ID
	std::string const& userPassword,	// 업로드 PW
	std::string const& ftpPath,		// 업로드 폴더
	std::string const& uploadFile )	// 업로드 파일
{
	tstring wsIP;
	wsIP.assign( sIp.begin(), sIp.end() );

	tstring wsID;
	wsID.assign( userId.begin(), userId.end() );

	tstring wsPW;
	wsPW.assign( userPassword.begin(), userPassword.end() );

	tstring wsUploadDir;
	wsUploadDir.assign( ftpPath.begin(), ftpPath.end() );

	tstring wsUploadFile;
	wsUploadFile.assign( uploadFile.begin(), uploadFile.end() );

	nsFTP::CFTPClient ftpClientsource;
	nsFTP::CLogonInfo logonInfoSource;
	logonInfoSource.SetHost( wsIP, nFtpPort, wsID, wsPW );
	if( !ftpClientsource.Login( logonInfoSource ) )
		return false;
	ftpClientsource.MakeDirectory( wsUploadDir );

	if( !ftpClientsource.UploadFile(wsUploadFile, wsUploadDir + wsUploadFile, false, nsFTP::CRepresentation(nsFTP::CType::Image()), true) )
	{
		ftpClientsource.Logout();
		return false;
	}

	// disconnect
	ftpClientsource.Logout();
	return true;
}

//////////////////////////////////////////////////////////////////////////

CResurecChecker::CResurecChecker():s_bStop(false),s_hThread(0),s_nThreadID(0),s_UploadDir("/%sLog_v1.%04d/%s/")
{
}

CResurecChecker::~CResurecChecker()
{
	StopThread();
}

void CResurecChecker::CreateThread()
{
	if( !IsThreadActive() )
	{
		s_hThread = (HANDLE)_beginthreadex( NULL, 0, &CResurecChecker::ThreadHashChecker, this, 0, &s_nThreadID );
		BHPRT( "sIntegrityChecker Start" );
	}
}

bool CResurecChecker::IsThreadActive() const
{
	if( 0 == s_hThread )
		return false;
	
	DWORD dwCode = -1;
	GetExitCodeThread( s_hThread, &dwCode );
	if( STILL_ACTIVE == dwCode )
		return true;

	return false;
}

void CResurecChecker::StopThread()
{
	if( 0 == s_hThread )
		return;

	s_bStop = true;

	DWORD dwSuspendedCount = 0;
	do 
	{
		dwSuspendedCount = ResumeThread( s_hThread );
		BHPRT( "Thread Sesume Count : %d", dwSuspendedCount );
	} while (dwSuspendedCount > 0 );
	
	DWORD dwWaitResult = WaitForSingleObject( s_hThread, INFINITE );
	if( WAIT_FAILED == dwWaitResult )
		BHPRT( "WaitForSingleObject Error : %d", GetLastError() );
	else
		BHPRT( "WaitForSingleObject dwWaitResult : %x", dwWaitResult );

	if( FALSE == CloseHandle(s_hThread) )
		BHPRT( "CloseHandle FASLE");

	s_hThread = 0;
}

void CResurecChecker::SetCallbackFunc(cResourectIntegrityChecker* pPointer, MemberFunc pCallBackFunc)
{
	s_TargetPointer = pPointer;
	resultCallbackFunc = pCallBackFunc;
}

void CResurecChecker::_SaveErrorMsg( std::string const& strMsg )
{
	s_ResultString += strMsg;
	s_ResultString += NEWLINE;	
}

void CResurecChecker::_SaveErrorMsg(std::list<std::string> const& strMsg)
{
	std::list<std::string>::const_iterator it = strMsg.begin();
	for( ; it != strMsg.end(); ++it )
		_SaveErrorMsg( *it );
}

void CResurecChecker::SetHashInfo(std::string const& strHashInfo, sFTPInfo defaultFTP)
{
	s_ServerData = strHashInfo;	
	s_DefaultFtpInfo = defaultFTP;
}

int CResurecChecker::CheckServerData()
{
	if( s_ServerData.empty() )// 서버에서 받은 해쉬 정보가 없다.
		return eSERVER_HASH_DOWNLOADURL_EMPTY;

	std::string hashInfo;
	if( !SimpleSecurity::SecuritySelf::Decript( s_ServerData, hashInfo ) )// 암호해체 실패
		return eSERVER_HASH_DOWNLOADURL_DECRIPT_ERROR;

	dm_stringvec infos;
	DmCS::StringFn::Cut( hashInfo, ",", infos);
	if( infos.size() != 13 )	// 파싱 정보가 다르다.
		return eSERVER_HASH_CUT_SIZE_ERROR;

	s_Serverhash = infos[0];// 리소스 파일일 해쉬값
	s_DownloadFtpInfo.s_Host = infos[1];// FTP Host
	s_DownloadFtpInfo.s_port = atoi(infos[2].c_str());// FTP Port
	s_DownloadFtpInfo.s_id = infos[3];// FTP ID
	s_DownloadFtpInfo.s_pw = infos[4];// FTP PW
	s_DownloadFtpInfo.s_dir = infos[5];// FTP Dir
	s_DownloadFtpInfo.s_file = infos[6];// FTP FileName	

	s_UploadFtpInfo.s_Host = infos[7];// FTP Host
	s_UploadFtpInfo.s_port = atoi(infos[8].c_str());// FTP Port
	s_UploadFtpInfo.s_id = infos[9];
	s_UploadFtpInfo.s_pw = infos[10];
	s_UploadFtpInfo.s_dir = infos[11];
	s_UploadFtpInfo.s_file = infos[12];

	if( !s_DownloadFtpInfo.IsValid() )
		return eSERVER_HASH_DOWNLOADURL_EMPTY;

	return eSUCCESS;
}

int CResurecChecker::DownLoadHashData(std::string & downLoadData)
{	
	g_FTPLoger.clear();
	bool bResult = false;
	int nTryCount = 0;
	do
	{
		bResult = _FTPFileDownload( s_DownloadFtpInfo.s_Host, s_DownloadFtpInfo.s_port, s_DownloadFtpInfo.s_id, s_DownloadFtpInfo.s_pw, 
									s_DownloadFtpInfo.s_dir, s_DownloadFtpInfo.s_file, downLoadData );
		if( bResult )
			break;

		nTryCount++;
		if( 3 <= nTryCount )
			break;

		Sleep(10000);
	}while( !bResult );

	if( !bResult )
	{
		_SaveErrorMsg( s_DownloadFtpInfo.GetString() );
		_SaveErrorMsg( g_FTPLoger.GetLog() );
		g_FTPLoger.clear();
		return eFILE_DOWNLOAD_FAIL;
	}
	
	g_FTPLoger.clear();
	return eSUCCESS;
}

int CResurecChecker::makeCheckData( std::string const& downloadData )
{
	s_PackHashData.clear(); 
	s_FileHashData.clear();

	if( downloadData.empty() )
		return eDownloadHashDataEmpty;

	if( !gzip::is_compressed( downloadData.c_str(), downloadData.size() ) )
		return eRESOURCE_HASHDATA_NOT_compressed;

	if( s_Serverhash.empty() )
		return eSERVER_HASH_DATA_EMPTY;
	
	std::string sHash = DmCS::sha256( downloadData );
	if( 0 != s_Serverhash.compare( sHash ) )
		return eRESOURCE_HASH_NOT_EQUAL;

	std::string org = gzip::decompress( downloadData.c_str(), downloadData.size() );

	while( !org.empty() )
	{
		std::string line = DmCS::StringFn::tokenize( org, NEWLINE );
		dm_stringvec value;
		DmCS::StringFn::Cut( line, TOKKEN, value );
		if( value.size() < 2 )
			break;

		dm_string name;
		dm_string exe;		 
		DmCS::FileFn::FindFileNameExt( value[0].c_str(), name, exe );

		if( 0 == exe.compare( ".dll" ) )
			s_FileHashData.insert( std::make_pair( value[0], value[1] ) );
		else if( 0 == exe.compare(".exe") )
			s_ClientExehash = value[1];
		else
			s_PackHashData.insert( std::make_pair( value[0], value[1] ) );
	}

	return eSUCCESS;
}

int CResurecChecker::CheckHashData_File()
{
	std::list<std::string> resultList;
	if( !s_ClientExehash.empty() )
	{
		TCHAR name[MAX_PATH] = {0,};
		GetModuleFileName( NULL, name, MAX_PATH);
		std::string sHash = DmCS::sha256_file( name );
		if( 0 != s_ClientExehash.compare( sHash ) )
		{
			std::string filePath;
			DmCS::StringFn::ToMB(name, filePath);
			resultList.push_back( filePath + TOKKEN + sHash );
		}
	}

	std::map<std::string,std::string>::const_iterator it = s_FileHashData.begin();
	for( ; it != s_FileHashData.end(); ++it )
	{
		std::string	sHash = DmCS::sha256_file(it->first.c_str());
		if( sHash.empty() )
			continue;

		if( 0 != sHash.compare( it->second ) )
			resultList.push_back( it->first + TOKKEN + sHash );
	}

	s_FileHashData.clear();

	if( resultList.empty() )
		return eSUCCESS;

	_SaveErrorMsg( resultList );

	return eCLIENT_HASH_DATA_NOT_EQUAL;
}

int CResurecChecker::CheckHashData_Pack(volatile bool const& bStop)
{
	char* pAlloc = NULL;
	size_t nAllocSize = 0;

	std::list<std::string> lostFile;
	std::list<std::string> missmatchFile;

	std::map<std::string,std::string>::const_iterator it = s_PackHashData.begin();
	for( ; it != s_PackHashData.end(); ++it )
	{
		if( bStop )
			break;

		int nCheckIdx = FT_RESOURCE_HANDLE;
		CsFPS::CsFileHash::sINFO* pHashInfo = CsFPS::CsFPSystem::GetHashData(nCheckIdx, it->first.c_str() );
		if( NULL == pHashInfo )
		{
			nCheckIdx = FT_PACKHANDLE;
			pHashInfo = CsFPS::CsFPSystem::GetHashData(nCheckIdx, it->first.c_str() );
		}

		if( NULL == pHashInfo )
		{
			lostFile.push_back( it->first );
			continue;
		}

		if( nAllocSize < pHashInfo->s_nDataSize )
		{
			nAllocSize = pHashInfo->s_nDataSize;
			NiFree( pAlloc );
			pAlloc = NiAlloc( char, nAllocSize );
		}
		CsFPS::CsFPSystem::GetFileData( nCheckIdx, &pAlloc, pHashInfo->s_nOffset, pHashInfo->s_nDataSize );
		std::string	sHash = DmCS::sha256_buffer( (const unsigned char*)pAlloc, pHashInfo->s_nDataSize );

		if( 0 != sHash.compare( it->second ) )
			missmatchFile.push_back( it->first + TOKKEN + sHash );
	}		

	if( pAlloc != NULL )
	{
		NiFree( pAlloc );
		pAlloc = NULL;
	}

	s_PackHashData.clear();

	if( !lostFile.empty() || !missmatchFile.empty() )
	{
		_SaveErrorMsg( missmatchFile );
		_SaveErrorMsg( lostFile );
		return eRESOURCE_MODEULATION;
	}

	return eSUCCESS;
}


void CResurecChecker::SendResultCallback(int const& nResult)
{
	BHPRT( "SendResultCallback Start" );
	if( s_TargetPointer )
		(s_TargetPointer->*resultCallbackFunc)( nResult );
	BHPRT( "SendResultCallback End" );
}

void CResurecChecker::SetAccountID(std::string const& strID)
{
	s_AccountID = strID;
}

//////////////////////////////////////////////////////////////////////////
void CResurecChecker::SendErrorResultFTP(int const& nErrorType)
{
	if( eSUCCESS == nErrorType )
		return;

	GAMEAPP_ST.SetProcessState( App::CWin32App::PROCESS_STOP );

	int nVersion = GLOBALDATA_ST.GetDownloadPatchVersion();
	std::string userKey = DmCS::sha256(s_AccountID);
	std::string uploadDir;
	DmCS::StringFn::FormatA( uploadDir, s_UploadDir.c_str(), s_UploadFtpInfo.s_dir.c_str(), nVersion, userKey.c_str() );

	std::string saveData;
	saveData = s_AccountID;
	saveData += NEWLINE;
	char buffer[65] = {0};
	itoa( nErrorType, buffer, 10 );
	saveData += buffer;
	saveData += NEWLINE;
	saveData += s_ResultString;
	s_ResultString.clear();

	std::string saveFileName = makeSaveFile( s_UploadFtpInfo.s_file );
	if( SaveSendFile( saveFileName,  saveData ) )
		_FTPFileUpload( s_UploadFtpInfo.s_Host, s_UploadFtpInfo.s_port, s_UploadFtpInfo.s_id, s_UploadFtpInfo.s_pw, uploadDir, saveFileName);
	::DeleteFileA( saveFileName.c_str() );	
}

unsigned __stdcall CResurecChecker::ThreadHashChecker(void* pParams)
{
	if( pParams )
	{
		CResurecChecker* pSelf = static_cast<CResurecChecker*>(pParams);
		int nLastError = eRESULT_WAIT;
		int nErrorType = pSelf->CheckServerData();
		if( eSUCCESS == nErrorType )
		{
			std::string downloadData;
			nErrorType =  pSelf->DownLoadHashData(downloadData);
			if( eSUCCESS == nErrorType )
			{
				nErrorType = pSelf->makeCheckData( downloadData );
				if( eSUCCESS == nErrorType )
				{
					nErrorType = pSelf->CheckHashData_File();
					if( eSUCCESS == nErrorType )
						nLastError = pSelf->CheckHashData_Pack(pSelf->s_bStop);
					else
						nLastError = nErrorType;
				}
				else
					nLastError = nErrorType;
			}
			else
				nLastError = nErrorType;
		}
		else
			nLastError = nErrorType;

		pSelf->SendErrorResultFTP(nLastError);
		pSelf->SendResultCallback(nLastError);
	}
	_endthreadex(0);
	return 0;
}


