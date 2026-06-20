#pragma once
#include "../00.Contents/ResourceIntegrityChecker.h"

bool _FTPFileDownload( std::string const& sIp, unsigned int const& nPort,
					   std::string const& sID, std::string const& sPW,
					   std::string const& sPath, std::string const& sDownloadFile, std::string & downloadString);
bool _FTPFileUpload( std::string const& sIp, unsigned int const& nFtpPort, 
					 std::string const& userId, std::string const& userPassword, 
					 std::string const& ftpPath, std::string const& uploadFile );

std::string makeSaveFile( std::string const& saveFile );
bool		SaveSendFile( std::string const& saveFile, std::string const& saveValue );


class CResurecChecker
{
public:
	CResurecChecker();
	~CResurecChecker();

	typedef void (cResourectIntegrityChecker::*MemberFunc)( int const& nErrorCode );

	enum eErrorType
	{
		eRESULT_WAIT = 0,
		eSUCCESS,
		eFILE_DOWNLOAD_FAIL, // 파일 다운로드 실패
		eRESOURCE_HASHDATA_NOT_compressed,
		eRESOURCE_HASH_NOT_EQUAL,
		eSERVER_HASH_DATA_NOT_RECV,
		eSERVER_HASH_DATA_EMPTY,
		eSERVER_HASH_DOWNLOADURL_EMPTY,		
		eSERVER_HASH_DOWNLOADURL_DECRIPT_ERROR,
		eDownloadHashDataEmpty,
		eSERVER_HASH_CUT_SIZE_ERROR,
		
		eThread_Stop, // 외부에서 리소스 체크 스레드가 종료 瑛?경우 
		eResourceCheckTimeOut,		// 리소스 체그 최대 시간 초과 
		eCLIENT_HASH_DATA_NOT_EQUAL,
		eMemoryModulation,
		eRESOURCE_MODEULATION,	// 리소스 변조		
	};
	static unsigned __stdcall ThreadHashChecker(void* pParams);

	void		CreateThread();
	void		StopThread();

	int			CheckServerData();
	int			DownLoadHashData(std::string & downLoadData);
	int			makeCheckData( std::string const& downloadData );
	int			CheckHashData_File();
	int			CheckHashData_Pack(volatile bool const& bStop);
	void		SetCallbackFunc( cResourectIntegrityChecker* pPointer, MemberFunc pCallBackFunc );
	void		SetHashInfo( std::string const& strHashInfo, sFTPInfo defaultFTP );

	bool		IsThreadActive() const;
	bool		SendErrorResultFTP();
	void		SendResultCallback(int const& nResult);
	void		_SaveErrorMsg( std::string const& strMsg );
	void		_SaveErrorMsg( std::list<std::string> const& strMsg );

	void		SetAccountID( std::string const& strID );
	void		SendErrorResultFTP(int const& nErrorType);

private:
	HANDLE								s_hThread;
	unsigned int						s_nThreadID;
	volatile bool						s_bStop;

	sFTPInfo							s_DownloadFtpInfo;
	sFTPInfo							s_UploadFtpInfo;
	sFTPInfo							s_DefaultFtpInfo;
	cResourectIntegrityChecker*			s_TargetPointer;
	MemberFunc							resultCallbackFunc;

	std::string							s_AccountID;
	std::string							s_Serverhash;
	std::string							s_ClientExehash;
	std::string							s_ResultString;
	std::map<std::string, std::string>	s_PackHashData;
	std::map<std::string, std::string>	s_FileHashData;

	std::string							s_ServerData;
	std::string							s_UploadDir;
};