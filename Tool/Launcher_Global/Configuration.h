#pragma once
#include <map>
#include <list>

struct sConfiguration
{

private:
	typedef std::map<DWORD, CString>					MAP_CONTAINER;
	typedef std::map<DWORD, CString>::iterator			MAP_CONTAINER_IT;
	typedef std::map<DWORD, CString>::const_iterator	MAP_CONTAINER_CIT;

	typedef std::map<DWORD, DWORD>						MAP_UPDATEMNG_CHECK;
	typedef std::map<DWORD, DWORD>::iterator			MAP_UPDATEMNG_CHECK_IT;
	typedef std::map<DWORD, DWORD>::const_iterator		MAP_UPDATEMNG_CHECK_CIT;

	struct sServerInfo
	{
		sServerInfo():m_bMaintenance(true){}

		bool			m_bMaintenance;
		CString			m_maintenaceTitle;
		CString			m_maintenaceMsg;
	};

	struct sPatchInfo
	{
		sPatchInfo():m_dwMinPatchVersion(0), m_dwPatchVersion(0) {}

		CString			m_CDNUrl;
		DWORD			m_dwMinPatchVersion;
		DWORD			m_dwPatchVersion;		
	};

	struct sLauncherInfo
	{
		sLauncherInfo():m_dwPatchVersion(0){}

		DWORD			m_dwPatchVersion;	
		CString			m_DownloadPath;	
		CString			m_ExeFileName;
		CString			m_RunExeFileName;
		MAP_CONTAINER	m_PatchFiles;
		//MAP_UPDATEMNG_CHECK	m_CheckUMVersion;
	};


	struct sUMInfo
	{
		sUMInfo():m_dwPatchVersion(0){}

		DWORD			m_dwPatchVersion;	
		CString			m_DownloadPath;	
		CString			m_ExeFileName;
		CString			m_RunExeFileName;
		MAP_CONTAINER	m_PatchFiles;
	};

	struct sMainUrlInfo
	{
		CString			m_HomePageUrl;		
		CString			m_MainPageUrl;
		CString			m_SecondPageUrl;
		CString			m_BanerUrl;
		CString			m_PatchPageUrl;
		CString			m_ExplorerStartPageUrl;
		CString			m_GameDownloadUrl;
		CString			m_FullClientFileName;
	};

	struct sServiceChangeInfo
	{
		CString			m_Message;
		CString			m_GotoUrl;
	};

	struct sRedistInfo
	{
		std::list<CString>	m_CheckList;
		CString				m_Execute;
	};

public:
	sConfiguration();
	~sConfiguration();

	bool			Parser( std::string const& value );

	bool			_IsValidData();
	bool			IsServerEnable() const;
	std::string		GetOrginString() const;

	DWORD			GetCurrentVersion_Launcher() const;
	void			GetLauncher_DownloadFiles( DWORD const& wCurrent, std::list<CString> & files );
	CString const&	GetExeFileName_Launcher() const;
	CString const&	GetDownloadUrl_Launcher() const;

	// 현재 런처 버전으로 업데이트를 진행할 업데이트 매니져 버젼을 받아온다.
	//DWORD	const&	GetUpdateUMVersion_LauncherVersion( DWORD const& dwCurrentLauncherVersion ) const;

	CString const&	GetExeFileName_UpdateManager() const;
	DWORD const&	GetCurrentVersion_UpdateManager() const;
	CString			GetCurrentVersionFileName_UpdateManager() const;
	CString			GetFileName_UpdateManager(DWORD const& dwUMVersion) const;
	CString const&  GetDownloadPath_UpdateManager() const;

	DWORD const&	GetMinPatchVersion() const;

	CString const&	GetMsgTitle() const;
	CString const&	GetMsg() const;

	DWORD const&	GetPatchVersion() const;
	CString const&	GetDownloadUrl_PatchFile() const;
	CString const&	GetRunExeFileName_Launcher() const;

	CString const&		GetFullClientDownloadUrl() const;

	//sMainUrlInfo const * GetTypeInfo( TCHAR* pUserType ) const;
	bool			IsUserTypeInfo( CString const& userType ) const;
	CString			GetHomePageUrl(CString const& userType) const;
	CString			GetBanerUrl(CString const& userType) const;
	CString			GetGameDownLoadUrl(CString const& userType) const;
	CString			GetPatchPageUrl(CString const& userType) const;
	CString			GetMainPageUrl(CString const& userType) const;
	CString			GetSecondPageUrl(CString const& userType) const;
	CString			GetGameDownloadUrl(CString const& userType) const;
	CString			GetFullClientFileName(CString const& userType) const;

	//////////////////////////////////////////////////////////////////////////
	// Service Change Message
	bool			IsShowServiceChangeMessage() const;
	CString			GetServiceChangeMessage() const;
	CString			GetServiceChangeUrl() const;

	//////////////////////////////////////////////////////////////////////////
	bool						IsRegCheckEnable() const;
	std::list<CString> const&	GetRegCheckList() const;
	CString						GetRegExecute() const;

private:
	void			_GetServerInfo( Json::Value const& root, sServerInfo& sData );
	void			_GetPatchInfo( Json::Value const& root, sPatchInfo & sData );
	void			_GetLauncherInfo( Json::Value const& root, sLauncherInfo & sData );
	void			_GetUpdateManagerInfo( Json::Value const& root, sUMInfo & sData );
	void			_GetMainUrlInfo( Json::Value const& root, std::map<CString,sMainUrlInfo> & sData );
	void			_GetServiceChangeInfo(Json::Value const& root, sServiceChangeInfo & sData);
	void			_GetRedistInfo(Json::Value const& root, sRedistInfo& sData);

private:
	std::string		m_OriginalString;		// json 문자열
	sServerInfo		m_serverInfo;
	sPatchInfo		m_PatchInfo;
	sLauncherInfo	m_LauncherInfo;
	sUMInfo			m_UMInfo;
	CString			m_FullClientDownloadURL;
	CString			m_UserType;
	sRedistInfo		m_RedistInfo;

	sServiceChangeInfo m_ServiceChangeInfo;
	std::map<CString,sMainUrlInfo>	m_mainUrlInfo;
};
