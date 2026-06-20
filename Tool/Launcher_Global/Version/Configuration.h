#pragma once
#include <map>
#include <list>

struct sConfiguration
{
private:
	typedef std::map<DWORD, CString>					MAP_CONTAINER;
	typedef std::map<DWORD, CString>::iterator		MAP_CONTAINER_IT;
	typedef std::map<DWORD, CString>::const_iterator	MAP_CONTAINER_CIT;

	struct sMainUrlInfo
	{
		CString			m_HomePageUrl;
		CString			m_GameDownloadUrl;
		CString			m_BanerUrl;
	};

	struct sServerInfo
	{
		bool			m_bMaintenance;
		CString			m_maintenaceTitle;
		CString			m_maintenaceMsg;
	};

	struct sPatchInfo
	{
		CString			m_CDNUrl;
		DWORD			m_dwMinPatchVersion;
		DWORD			m_dwPatchVersion;		
	};

	struct sLauncherInfo
	{
		CString			m_DownloadPath;
		DWORD			m_dwPatchVersion;		
		CString			m_ExeFileName;
		CString			m_RunExeFileName;
		MAP_CONTAINER	m_PatchFiles;
	};


	struct sUMInfo
	{
		CString			m_DownloadPath;
		DWORD			m_dwPatchVersion;		
		CString			m_ExeFileName;
		CString			m_RunExeFileName;
		MAP_CONTAINER	m_PatchFiles;
	};

public:
	sConfiguration();
	~sConfiguration();

	bool			Parser( std::string const& value );

	bool			IsServerEnable() const;

	DWORD			GetCurrentVersion_Launcher() const;
	void			GetLauncher_DownloadFiles( DWORD const& wCurrent, std::list<CString> & files );
	CString const&	GetExeFileName_Launcher() const;
	CString const&	GetDownloadUrl_Launcher() const;



	CString const&	GetExeFileName_UpdateManager() const;
	DWORD const&	GetCurrentVersion_UpdateManager() const;
	CString const&  GetCurrentVersionFileName_UpdateManager() const;
	CString const&  GetDownloadPath_UpdateManager() const;

	CString const&	GetBanerUrl() const;
	CString const&	GetGameDownLoadUrl() const;
	DWORD const&	GetMinPatchVersion() const;

	CString const&	GetMsgTitle() const;
	CString const&	GetMsg() const;

	DWORD const&	GetPatchVersion() const;
	CString const&	GetDownloadUrl_PatchFile() const;
	CString const&	GetRunExeFileName_Launcher() const;

private:
	std::string		m_OriginalString;		// json ¹®ÀÚ¿­
	sServerInfo		m_serverInfo;
	sPatchInfo		m_PatchInfo;
	sLauncherInfo	m_LauncherInfo;
	sUMInfo			m_UMInfo;
	sMainUrlInfo	m_mainUrlInfo;
};
