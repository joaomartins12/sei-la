


#include "stdafx.h"


#include "cFtpClient.h"



cFtpClient::cFtpClient(void) :
	m_hInternet(NULL),
	m_hFtp(NULL)
{


}



cFtpClient::~cFtpClient(void)
{
	Close();
}



void CALLBACK MyFtpCallBack(
	HINTERNET hInternet,
	DWORD_PTR dwContext,
	DWORD dwInternetStatus,
	LPVOID lpvStatusInformation,
	DWORD dwStatusInformationLength)
{
	DWORD_PTR nBytes = 0;

	switch(dwInternetStatus)
	{
	case INTERNET_STATUS_HANDLE_CREATED		:
		break;
	case INTERNET_STATUS_REQUEST_COMPLETE	:
		break;
	case INTERNET_STATUS_RESPONSE_RECEIVED	:
		nBytes = ((INTERNET_ASYNC_RESULT*)lpvStatusInformation)->dwResult;
		break;
	case INTERNET_STATUS_REQUEST_SENT		:
		nBytes = ((INTERNET_ASYNC_RESULT*)lpvStatusInformation)->dwResult;
		break;
	case INTERNET_STATUS_CONNECTION_CLOSED	:
		break;
	}
}


bool cFtpClient::Init(void)
{
//	Sleep(33);	Sleep(33);	Sleep(33);

	TCHAR szAgentName[32] = _T( "FTP_UPDATER" );
//	sprintf(szAgentName, "FTP_%08x", nBase::GetTickCount());

	m_hInternet = InternetOpen(szAgentName, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0); //INTERNET_FLAG_OFFLINE);
//	m_hInternet = InternetOpen(szAgentName, INTERNET_OPEN_TYPE_PRECONFIG, null, null, 0); //INTERNET_FLAG_OFFLINE);

	if(!m_hInternet)
	{
		return false;
	}

//	InternetSetStatusCallback(m_hInternet, MyFtpCallBack);

	return true;
}


bool cFtpClient::Connect(char const *szSvrName, unsigned int nSvrPort, char const *szUserID, char const *szUserPass, bool bIsPassive)
{
	if( m_hInternet == NULL )
		return false;

	m_hFtp = InternetConnectA
	(
		m_hInternet,
		szSvrName,
		nSvrPort,
		szUserID,
		szUserPass,
		INTERNET_SERVICE_FTP,
		bIsPassive ? INTERNET_FLAG_PASSIVE : 0,
		0
	);

	if(!m_hFtp)
	{

		InternetCloseHandle(m_hInternet);
		m_hInternet = NULL;
		return false;
	}

	m_bIsPassive = bIsPassive;

	return true;
}


void cFtpClient::Command(char *cmd)
{
	if(!m_hFtp)
	{
		return;
	}

	BOOL res = FALSE;
	res = FtpCommandA(m_hFtp, FALSE, FTP_TRANSFER_TYPE_BINARY, cmd, 0, NULL);

	if(res == FALSE)
	{
	}
}


void cFtpClient::Close(void)
{
	Command("QUIT");

	if(m_hFtp)
	{
		InternetCloseHandle(m_hFtp);
		m_hFtp = NULL;
	}

	if(m_hInternet)
	{
		InternetCloseHandle(m_hInternet);
		m_hInternet = NULL;
	}
}


bool cFtpClient::PutFile(char const*szRemoteFile, char const*szLocalFile)
{
	BOOL res = FtpPutFileA
	(
		m_hFtp,
		szLocalFile,
		szRemoteFile,
		FTP_TRANSFER_TYPE_BINARY,
		0
	);

	if(!res)
	{
//		ScanError();
	}
	
	return res ? true : false;
}


void cFtpClient::MakeLocalPath(char *path)
{
	int i = 0;
	char *p = path; // current

	while(p[i])
	{
		switch(p[i])
		{
		case '/'  :
		case '\\' :
			p[i] = 0;
			CreateDirectoryA(path, NULL);
			p[i] = '\\';
		}

		++ i;
	}

	i = 0;
	p = path;

	while(p[i])
	{
		if(p[i] == '\\')
		{
			p[i] = '/';
		}
		++ i;
	}
}


bool cFtpClient::GetFile(char const*szRemoteFile, char const*szLocalFile)
{
	char chLocalFile[MAX_PATH]= {0,};
	strcpy_s( chLocalFile, sizeof(chLocalFile),szLocalFile );
	MakeLocalPath(chLocalFile);

	Command("ABOR");
	Command(m_bIsPassive ? "PASV" : "NOOP");
		
	BOOL res = FtpGetFileA
	(
		m_hFtp,
		szRemoteFile,
		chLocalFile,
		false,
		FILE_ATTRIBUTE_NORMAL,
		FTP_TRANSFER_TYPE_BINARY | INTERNET_FLAG_RELOAD,
		0
	);

	static int s_nErrCnt = 0;
	if(res)
	{
		s_nErrCnt = 0;
		return true;
	}
	else
	{
		++ s_nErrCnt;
		//ScanError();
	}

	if(s_nErrCnt <= 5) // 5È¸ Àç½Ãµµ
	{
		Sleep(99); Sleep(99); Sleep(99);
		return GetFile(szRemoteFile, chLocalFile);
	}

	s_nErrCnt = 0;
	return false;
}

bool cFtpClient::GetFile(std::string const& szFileName, std::string& szSaveData)
{
	Command("ABOR");
	Command(m_bIsPassive ? "PASV" : "NOOP");

	HINTERNET hSourec = FtpOpenFileA( m_hFtp, szFileName.c_str(), GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0 );
	if( !hSourec )
		return false;

	DWORD dwReadSize = 0;
	InternetQueryDataAvailable( hSourec, &dwReadSize, 0, 0 );
 	char* pBuffer = new char[dwReadSize];

	bool bResult = false;
	DWORD dwRead = 0;
	do 
	{
		bResult = InternetReadFile(hSourec, pBuffer, dwReadSize, &dwRead);
		if( !bResult && GetLastError() == ERROR_IO_PENDING )
		{
			BHPRT( "InternetReadFileExA ERROR_IO_PENDING");
			bResult = true;
		}

		if( dwRead > 0 )
			szSaveData += std::string(pBuffer, dwRead);

	} while ( bResult && dwRead != 0 );

	SAFE_DELETE_ARRAY( pBuffer );

	return true;
}


void cFtpClient::DelFile(char const*szRemoteFile)
{
	BOOL res = FtpDeleteFileA
	(
		m_hFtp,
		szRemoteFile
	);

	if(!res)
	{
		//ScanError();
	}
}


bool cFtpClient::SetCurDir(char const*szRemoteDirectory)
{
	BOOL res = FtpSetCurrentDirectoryA(m_hFtp, szRemoteDirectory);

	if(!res)
	{
		//ScanError();
	}

	return res ? true : false;
}



bool cFtpClient::GetCurDir(char *szRemoteDirectory)
{
	char buf[255] = "";
	DWORD size = 255;

	BOOL res = FtpGetCurrentDirectoryA(m_hFtp, buf, &size);

	if(!res)
	{
		//ScanError();
	}

	strcpy(szRemoteDirectory, buf);

	return res ? true : false;
}



void cFtpClient::NewDirectory(char const *szRemoteDirectory)
{
	BOOL res = FtpCreateDirectoryA(m_hFtp, szRemoteDirectory);

	if(!res)
	{
		//ScanError();
	}
}


void cFtpClient::DelDirectory(char const *szRemoteDirectory)
{
	BOOL res = FtpRemoveDirectoryA(m_hFtp, szRemoteDirectory);

	if(!res)
	{
		//ScanError();
	}
}











