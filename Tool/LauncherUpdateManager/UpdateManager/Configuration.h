#pragma once
#include <map>
#include <list>

struct sConfiguration
{

private:
	struct sServerInfo
	{
		sServerInfo():m_bMaintenance(true){}

		bool			m_bMaintenance;
		CString			m_maintenaceTitle;
		CString			m_maintenaceMsg;
	};	

	struct sLauncherInfo
	{
		sLauncherInfo():m_dwPatchVersion(0){}

		DWORD			m_dwPatchVersion;	
		CString			m_DownloadPath;	
		CString			m_ExeFileName;
		CString			m_RunExeFileName;
		std::map<DWORD, CString>	m_PatchFiles;
		std::map<DWORD, DWORD>	m_CheckUMVersion;
	};

public:
	sConfiguration();
	~sConfiguration();

	bool			Parser( std::string const& value );

	bool			_IsValidData();
	bool			IsServerEnable() const;

	DWORD			GetCurrentVersion_Launcher() const;
	void			GetLauncher_DownloadFiles( DWORD const& wCurrent, std::list<CString> & files );
	CString const&	GetExeFileName_Launcher() const;
	CString const&	GetDownloadUrl_Launcher() const;
	CString			GetLastVersionFile_Launcher() const;
	
	
	CString const&	GetMsgTitle() const;
	CString const&	GetMsg() const;

	CString const&	GetRunExeFileName_Launcher() const;

private:
	void			_GetServerInfo( Json::Value const& root, sServerInfo& sData );
	void			_GetLauncherInfo( Json::Value const& root, sLauncherInfo & sData );

private:
	sServerInfo		m_serverInfo;
	sLauncherInfo	m_LauncherInfo;
};
