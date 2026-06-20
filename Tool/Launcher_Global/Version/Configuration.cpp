#include "stdafx.h"
#include "Configuration.h"
//////////////////////////////////////////////////////////////////////////

sConfiguration::sConfiguration()
{}

sConfiguration::~sConfiguration()
{}


bool sConfiguration::Parser( std::string const& value )
{
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( value, root );
	if ( !parsingSuccessful )
		return false;

	m_OriginalString = value;

	m_serverInfo.m_bMaintenance = root["ServerInfo"]["Maintenance"].asBool();
	m_serverInfo.m_maintenaceTitle = root["ServerInfo"]["Message"]["Title"].asWString().c_str();
	m_serverInfo.m_maintenaceMsg = root["ServerInfo"]["Message"]["Msg"].asWString().c_str();

	m_mainUrlInfo.m_HomePageUrl = root["MainUrl"]["HomePageUrl"].asWString().c_str();
	m_mainUrlInfo.m_GameDownloadUrl = root["MainUrl"]["GameDownloadUrl"].asWString().c_str();
	m_mainUrlInfo.m_BanerUrl = root["MainUrl"]["BanerUrl"].asWString().c_str();

	m_PatchInfo.m_CDNUrl = root["PatchInfo"]["DownloadUrl"].asWString().c_str();
	m_PatchInfo.m_dwMinPatchVersion = root["PatchInfo"]["MinPatchVersion"].asDouble();
	m_PatchInfo.m_dwPatchVersion = root["PatchInfo"]["PatchVersion"].asDouble();


	m_LauncherInfo.m_DownloadPath = root["LanuncherInfo"]["DownloadPath"].asWString().c_str();
	m_LauncherInfo.m_dwPatchVersion = root["LanuncherInfo"]["CurrentVersion"].asUInt();
	m_LauncherInfo.m_ExeFileName = root["LanuncherInfo"]["ExeFileName"].asWString().c_str();
	m_LauncherInfo.m_RunExeFileName = root["LanuncherInfo"]["RunExeFileName"].asWString().c_str();
	Json::Value lList = root["LanuncherInfo"]["VersionList"];
	for( int n = 0; n < lList.size(); ++n )
	{
		DWORD nVersion = lList[n]["Version"].asUInt();
		CString fileName = lList[n]["DownloadFile"].asWString().c_str();
		m_LauncherInfo.m_PatchFiles.insert( std::make_pair( nVersion, fileName ) );
	}


	m_UMInfo.m_DownloadPath = root["UMInfo"]["DownloadPath"].asWString().c_str();
	m_UMInfo.m_dwPatchVersion = root["UMInfo"]["CurrentVersion"].asUInt();
	m_UMInfo.m_ExeFileName = root["UMInfo"]["ExeFileName"].asWString().c_str();
	m_UMInfo.m_RunExeFileName = root["UMInfo"]["RunExeFileName"].asWString().c_str();
	Json::Value umList = root["UMInfo"]["VersionList"];
	for( int n = 0; n < umList.size(); ++n )
	{
		DWORD nVersion = umList[n]["Version"].asUInt();
		CString fileName = umList[n]["DownloadFile"].asWString().c_str();
		m_UMInfo.m_PatchFiles.insert( std::make_pair( nVersion, fileName ) );
	}

	return true;
}

bool sConfiguration::IsServerEnable() const
{
	return !m_serverInfo.m_bMaintenance;
}

DWORD sConfiguration::GetCurrentVersion_Launcher() const
{
	return m_LauncherInfo.m_dwPatchVersion;
}

CString const& sConfiguration::GetExeFileName_Launcher() const
{
	return m_LauncherInfo.m_ExeFileName;
}

CString const& sConfiguration::GetDownloadUrl_Launcher() const
{
	return m_LauncherInfo.m_DownloadPath;
}

CString const& sConfiguration::GetExeFileName_UpdateManager() const
{
	return m_UMInfo.m_ExeFileName;
}

DWORD const& sConfiguration::GetCurrentVersion_UpdateManager() const
{
	return m_UMInfo.m_dwPatchVersion;
}

CString const& sConfiguration::GetDownloadPath_UpdateManager() const
{
	return m_UMInfo.m_DownloadPath;
}

CString const& sConfiguration::GetCurrentVersionFileName_UpdateManager() const
{
	MAP_CONTAINER_CIT it = m_UMInfo.m_PatchFiles.find( m_UMInfo.m_dwPatchVersion );
	if( it == m_UMInfo.m_PatchFiles.end() )
		return L"";

	return it->second;
}

void sConfiguration::GetLauncher_DownloadFiles( DWORD const& dwVersion, std::list<CString> & files )
{
	MAP_CONTAINER_CIT it = m_LauncherInfo.m_PatchFiles.begin();
	for( ; it != m_LauncherInfo.m_PatchFiles.end(); ++it )
	{
		if( it->first > dwVersion )
			files.push_back( it->second );
	}
}

CString const&	sConfiguration::GetBanerUrl() const
{
	return m_mainUrlInfo.m_BanerUrl;
}

CString const&	sConfiguration::GetGameDownLoadUrl() const
{
	return m_mainUrlInfo.m_GameDownloadUrl;
}

DWORD const& sConfiguration::GetMinPatchVersion() const
{
	return m_PatchInfo.m_dwMinPatchVersion;
}

CString const& sConfiguration::GetMsgTitle() const
{
	return m_serverInfo.m_maintenaceTitle;
}

CString const& sConfiguration::GetMsg() const
{
	return m_serverInfo.m_maintenaceMsg;
}

DWORD const& sConfiguration::GetPatchVersion() const
{
	return m_PatchInfo.m_dwPatchVersion;
}

CString const& sConfiguration::GetDownloadUrl_PatchFile() const
{
	return m_PatchInfo.m_CDNUrl;
}

CString const& sConfiguration::GetRunExeFileName_Launcher() const
{
	return m_LauncherInfo.m_RunExeFileName;
}