#include "stdafx.h"
#include "Configuration.h"
//////////////////////////////////////////////////////////////////////////

sConfiguration::sConfiguration()
{}

sConfiguration::~sConfiguration()
{}

bool sConfiguration::_IsValidData()
{
	if( m_PatchInfo.m_CDNUrl.IsEmpty() )			return false;	// 패치 파일 받을 CDN Url 주소가 없는 경우 
	if( m_PatchInfo.m_dwPatchVersion == 0 )			return false;	// 패치 버전이 없는 경우

	if( m_LauncherInfo.m_DownloadPath.IsEmpty() )	return false;	// 런처 다운로드 경로가 없는 경우
	if( m_LauncherInfo.m_dwPatchVersion == 0 )		return false;	// 런처 버전 정보가 없는 경우
	if( m_LauncherInfo.m_ExeFileName.IsEmpty() )	return false;	// 런처의 실행 파일 명이 없는 경우
	if( m_LauncherInfo.m_RunExeFileName.IsEmpty() )	return false;	// 런처가 실행할 파일 명이 없는 경우

	if( m_UMInfo.m_DownloadPath.IsEmpty() )			return false;	// UpdateManager의 다운 로드 경로가 없는 경우
	if( m_UMInfo.m_dwPatchVersion == 0 )			return false;	// UpdateManager의 버전 정보가 없는 경우
	if( m_UMInfo.m_ExeFileName.IsEmpty() )			return false;	// UpdateManager의 실행 파일 명이 없는 경우
	if( m_UMInfo.m_RunExeFileName.IsEmpty() )		return false;	// UpdateManager가 실행할 파일 명이 없는 경우

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

void sConfiguration::_GetPatchInfo( Json::Value const& root, sPatchInfo & sData )
{
	if( !root.isMember( "PatchInfo" ) )
		return;

	Json::Value const& jPatchInfo = root.get( "PatchInfo", "" );
	sData.m_CDNUrl = jPatchInfo.get( "DownloadUrl", "" ).asWString().c_str();
	sData.m_dwMinPatchVersion = static_cast<DWORD>(jPatchInfo.get("MinPatchVersion", "").asDouble());
	sData.m_dwPatchVersion =  static_cast<DWORD>(jPatchInfo.get("PatchVersion", "").asDouble());
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

// 	Json::Value lUmList = jLauncherInfo.get("UMVersion", "");
// 	Json::Value::iterator umit = lUmList.begin();
// 	for( ; umit != lUmList.end(); ++umit )
// 	{
// 		DWORD nMaxLauncherVersion = (*umit).get( "LauncherVersionMax", "0" ).asUInt();
// 		DWORD nUMVersion = (*umit).get( "UMVersion", "0" ).asUInt();
// 		sData.m_CheckUMVersion.insert( std::make_pair( nMaxLauncherVersion, nUMVersion ) );
// 	}	
}

void sConfiguration::_GetUpdateManagerInfo( Json::Value const& root, sUMInfo & sData )
{
	if( !root.isMember( "UMInfo" ) )
		return;

	Json::Value const& jUpdateMngInfo = root.get( "UMInfo", "" );	
	sData.m_DownloadPath = jUpdateMngInfo.get("DownloadPath", "").asWString().c_str();
	sData.m_dwPatchVersion = jUpdateMngInfo.get("CurrentVersion", "0").asUInt();
	sData.m_ExeFileName = jUpdateMngInfo.get("ExeFileName", "").asWString().c_str();
	sData.m_RunExeFileName = jUpdateMngInfo.get("RunExeFileName", "").asWString().c_str();

	Json::Value umList = jUpdateMngInfo.get("VersionList", "");
	Json::Value::iterator it = umList.begin();
	for( ; it != umList.end(); ++it )
	{
		DWORD nVersion = (*it).get( "Version","0" ).asUInt();
		CString fileName = (*it).get( "DownloadFile","" ).asWString().c_str();
		sData.m_PatchFiles.insert( std::make_pair( nVersion, fileName ) );
	}
}

void sConfiguration::_GetMainUrlInfo( Json::Value const& root, std::map<CString,sMainUrlInfo> & sData )
{
	if( !root.isMember( "MainUrl" ) )
		return;

	Json::Value const& jUrlInfo = root.get( "MainUrl", "" );	
	Json::Value::iterator it = jUrlInfo.begin();
	for( ; it != jUrlInfo.end(); ++it )
	{
		CString key = (*it).get( "TypeName", "" ).asWString().c_str();
		if( key.IsEmpty() )
			continue;

		sMainUrlInfo addInfo;
		addInfo.m_HomePageUrl			= (*it).get( "HomePageUrl", "" ).asWString().c_str();
		addInfo.m_MainPageUrl			= (*it).get( "MainPageUrl", "" ).asWString().c_str();
		addInfo.m_SecondPageUrl			= (*it).get( "SecondPageUrl", "" ).asWString().c_str();		
		addInfo.m_BanerUrl				= (*it).get( "BanerUrl", "" ).asWString().c_str();
		addInfo.m_PatchPageUrl			= (*it).get( "PatchPageUrl", "" ).asWString().c_str();
		addInfo.m_GameDownloadUrl		= (*it).get( "GameDownloadUrl", "" ).asWString().c_str();
		addInfo.m_ExplorerStartPageUrl	= (*it).get( "ExplorerStartPage", "" ).asWString().c_str();
		addInfo.m_FullClientFileName	= (*it).get( "InstallFileName", "" ).asWString().c_str();

		sData.insert( std::make_pair( key, addInfo ) );
	}
}

void sConfiguration::_GetServiceChangeInfo( Json::Value const& root, sServiceChangeInfo & sData )
{
	if( !root.isMember( "ServiceChange" ) )
		return;

	Json::Value const& jUpdateMngInfo = root.get( "ServiceChange", "" );	
	sData.m_Message = jUpdateMngInfo.get("Message", "").asWString().c_str();
	sData.m_GotoUrl = jUpdateMngInfo.get("GotoUrl", "").asWString().c_str();
}

void sConfiguration::_GetRedistInfo(Json::Value const& root, sRedistInfo& sData)
{
	if (!root.isMember("Redist"))
		return;

	Json::Value const& jInfo = root.get("Redist", "");
	sData.m_Execute = jInfo.get("Execute", "").asWString().c_str();
	Json::Value lList = jInfo.get("Regstry", "");
	Json::Value::iterator it = lList.begin();
	for (; it != lList.end(); ++it)
	{
		CString value = (*it).get("Reg", "").asWString().c_str();
		if (value.IsEmpty())
			continue;
		sData.m_CheckList.push_back(value);
	}
}

bool sConfiguration::Parser( std::string const& value )
{
	Json::Value root;
	Json::Reader reader;
	bool parsingSuccessful = reader.parse( value, root );
	if ( !parsingSuccessful )
		return false;

	Json::FastWriter writer;
	m_OriginalString = writer.write( root );
	m_FullClientDownloadURL = root.get( "FullClinetURL", "" ).asWString().c_str();

	_GetServerInfo( root, m_serverInfo );
	_GetPatchInfo( root, m_PatchInfo );
	_GetLauncherInfo( root, m_LauncherInfo );
	_GetUpdateManagerInfo( root, m_UMInfo );
	_GetMainUrlInfo( root, m_mainUrlInfo );
	_GetServiceChangeInfo(root, m_ServiceChangeInfo);
	_GetRedistInfo(root, m_RedistInfo);
	return _IsValidData();
}

bool sConfiguration::IsServerEnable() const
{
	return !m_serverInfo.m_bMaintenance;
}

std::string sConfiguration::GetOrginString() const
{
	return m_OriginalString;
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

// 현재 런처 버전으로 업데이트를 진행할 업데이트 매니져 버젼을 받아온다.
// 목록에 없는 버젼인 경우 최신 버전을 받는다.
// DWORD	const&	sConfiguration::GetUpdateUMVersion_LauncherVersion( DWORD const& dwCurrentLauncherVersion ) const
// {
// 	MAP_UPDATEMNG_CHECK_CIT it = m_LauncherInfo.m_CheckUMVersion.begin();
// 	for( ; it != m_LauncherInfo.m_CheckUMVersion.end(); ++it )	
// 	{
// 		if( it->first >= dwCurrentLauncherVersion )
// 			return it->second;
// 	}
// 	return m_UMInfo.m_dwPatchVersion;
// }

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

CString sConfiguration::GetCurrentVersionFileName_UpdateManager() const
{
	MAP_CONTAINER_CIT it = m_UMInfo.m_PatchFiles.find( m_UMInfo.m_dwPatchVersion );
	if( it == m_UMInfo.m_PatchFiles.end() )
		return L"";

	return it->second;
}

CString sConfiguration::GetFileName_UpdateManager(DWORD const& dwUMVersion) const
{
	MAP_CONTAINER_CIT it = m_UMInfo.m_PatchFiles.find( dwUMVersion );
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

CString const&	sConfiguration::GetFullClientDownloadUrl() const
{
	return m_FullClientDownloadURL;
}

//////////////////////////////////////////////////////////////////////////

// sConfiguration::sMainUrlInfo const * sConfiguration::GetTypeInfo( TCHAR* pUserType ) const
// {
// 	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find( pUserType );
// 	if( it == m_mainUrlInfo.end() )
// 		return NULL;
// 	return &it->second;
// }

bool sConfiguration::IsUserTypeInfo( CString const& userType ) const
{
	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find(userType);
	if( it == m_mainUrlInfo.end() )
		return false;

	return true;
}

CString sConfiguration::GetHomePageUrl(CString const& userType) const
{
	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find(userType);
	if( it == m_mainUrlInfo.end() )
		return _T("");
	return it->second.m_HomePageUrl;
}


CString sConfiguration::GetPatchPageUrl(CString const& userType) const
{
	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find(userType);
	if( it == m_mainUrlInfo.end() )
		return _T("");
	return it->second.m_PatchPageUrl;
}

CString sConfiguration::GetMainPageUrl(CString const& userType) const
{
	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find(userType);
	if( it == m_mainUrlInfo.end() )
		return _T("");
	return it->second.m_MainPageUrl;
}

CString sConfiguration::GetSecondPageUrl(CString const& userType) const
{
	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find(userType);
	if( it == m_mainUrlInfo.end() )
		return _T("");
	return it->second.m_SecondPageUrl;
}

CString sConfiguration::GetGameDownloadUrl(CString const& userType) const
{
	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find(userType);
	if( it == m_mainUrlInfo.end() )
		return _T("");
	return it->second.m_GameDownloadUrl;
}

CString sConfiguration::GetFullClientFileName(CString const& userType) const
{
	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find(userType);
	if( it == m_mainUrlInfo.end() )
		return _T("");
	return it->second.m_FullClientFileName;
}

CString sConfiguration::GetBanerUrl(CString const& userType) const
{
	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find(userType);
	if( it == m_mainUrlInfo.end() )
		return _T("");
	return it->second.m_BanerUrl;
}

CString sConfiguration::GetGameDownLoadUrl(CString const& userType) const
{
	std::map<CString,sMainUrlInfo>::const_iterator it = m_mainUrlInfo.find(userType);
	if( it == m_mainUrlInfo.end() )
		return _T("");
	return it->second.m_GameDownloadUrl;
}

//////////////////////////////////////////////////////////////////////////

bool sConfiguration::IsShowServiceChangeMessage() const
{
	return !m_ServiceChangeInfo.m_Message.IsEmpty();
}

CString sConfiguration::GetServiceChangeMessage() const
{
	return m_ServiceChangeInfo.m_Message;
}

CString sConfiguration::GetServiceChangeUrl() const
{
	return m_ServiceChangeInfo.m_GotoUrl;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

bool sConfiguration::IsRegCheckEnable() const
{
	if (m_RedistInfo.m_CheckList.empty())
		return false;

	if (m_RedistInfo.m_Execute.IsEmpty())
		return false;

	return true;
}

std::list<CString> const& sConfiguration::GetRegCheckList() const
{
	return m_RedistInfo.m_CheckList;
}

CString sConfiguration::GetRegExecute() const
{
	return m_RedistInfo.m_Execute;
}
