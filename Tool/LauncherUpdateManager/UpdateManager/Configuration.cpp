#include "stdafx.h"
#include "Configuration.h"
//////////////////////////////////////////////////////////////////////////

sConfiguration::sConfiguration()
{}

sConfiguration::~sConfiguration()
{}

bool sConfiguration::_IsValidData()
{
	if( m_LauncherInfo.m_DownloadPath.IsEmpty() )	return false;	// 런처 다운로드 경로가 없는 경우
	if( m_LauncherInfo.m_dwPatchVersion == 0 )		return false;	// 런처 버전 정보가 없는 경우
	if( m_LauncherInfo.m_ExeFileName.IsEmpty() )	return false;	// 런처의 실행 파일 명이 없는 경우
	if( m_LauncherInfo.m_RunExeFileName.IsEmpty() )	return false;	// 런처가 실행할 파일 명이 없는 경우

	return true;
}

void sConfiguration::_GetServerInfo( Json::Value const& root, sServerInfo& sData )
{
	if( !root.isMember( "ServerInfo" ) )
		return;

	Json::Value const& jInfo = root.get( "ServerInfo", "" );
	sData.m_bMaintenance = jInfo.get( "Maintenance", "true" ).asBool();

	if( !jInfo.isMember( "Message" ) )
		return;

	Json::Value const& jMessageInfo = jInfo.get( "Message", "" );
	sData.m_maintenaceTitle = jMessageInfo.get("Title","").asWString().c_str();
	sData.m_maintenaceMsg = jMessageInfo.get("Msg","").asWString().c_str();
}

void sConfiguration::_GetLauncherInfo( Json::Value const& root, sLauncherInfo & sData )
{
	if( !root.isMember( "LanuncherInfo" ) )
		return;

	Json::Value const& jLauncherInfo = root.get( "LanuncherInfo", "" );	
	sData.m_DownloadPath = jLauncherInfo.get( "DownloadPath", "" ).asWString().c_str();

	sData.m_dwPatchVersion = jLauncherInfo.get("CurrentVersion", "").asUInt();
	sData.m_ExeFileName = jLauncherInfo.get("ExeFileName", "").asWString().c_str();
	sData.m_RunExeFileName = jLauncherInfo.get("RunExeFileName", "").asWString().c_str();
	
	Json::Value lList = jLauncherInfo.get("VersionList", "");
	Json::Value::iterator it = lList.begin();
	for( ; it != lList.end(); ++it )
	{
		DWORD nVersion = (*it).get( "Version", "0" ).asUInt();
		CString fileName = (*it).get( "DownloadFile", "0" ).asWString().c_str();
		sData.m_PatchFiles.insert( std::make_pair( nVersion, fileName ) );
	}

	Json::Value lUmList = jLauncherInfo.get("UMVersion", "");
	Json::Value::iterator umit = lUmList.begin();
	for( ; umit != lUmList.end(); ++umit )
	{
		DWORD nMaxLauncherVersion = (*umit).get( "LauncherVersionMax", "0" ).asUInt();
		DWORD nUMVersion = (*umit).get( "UMVersion", "0" ).asUInt();
		sData.m_CheckUMVersion.insert( std::make_pair( nMaxLauncherVersion, nUMVersion ) );
	}	
}

bool sConfiguration::Parser( std::string const& value )
{
	Json::Value root;
	Json::Reader reader;
	if ( !reader.parse(value, root))
		return false;
	
	_GetServerInfo( root, m_serverInfo );
	_GetLauncherInfo( root, m_LauncherInfo );

	return _IsValidData();
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

CString sConfiguration::GetLastVersionFile_Launcher() const
{
	auto const& it = m_LauncherInfo.m_PatchFiles.find(m_LauncherInfo.m_dwPatchVersion);
	if (it == m_LauncherInfo.m_PatchFiles.end())
		return _T("");
	return it->second;
}

void sConfiguration::GetLauncher_DownloadFiles( DWORD const& dwVersion, std::list<CString> & files )
{
	for(auto const& it : m_LauncherInfo.m_PatchFiles)
	{
		if( it.first > dwVersion )
			files.push_back( it.second );
	}
}

CString const& sConfiguration::GetMsgTitle() const
{
	return m_serverInfo.m_maintenaceTitle;
}

CString const& sConfiguration::GetMsg() const
{
	return m_serverInfo.m_maintenaceMsg;
}

CString const& sConfiguration::GetRunExeFileName_Launcher() const
{
	return m_LauncherInfo.m_RunExeFileName;
}
