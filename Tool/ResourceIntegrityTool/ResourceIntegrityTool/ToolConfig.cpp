#include "stdafx.h"
#include "ToolConfig.h"
#include <fstream>

sFTPInfo::sFTPInfo() : m_Port(0)
{

}

sFTPInfo::~sFTPInfo()
{
}

void sFTPInfo::Reset()
{
	m_Host.clear();
	m_Port = 0;
	m_ID.clear();
	m_PW.clear();
	m_Directory.clear();
	m_FileName.clear();
}

std::wstring sFTPInfo::GetHost() const
{
	std::wstring value;
	DmCS::StringFn::From(value, m_Host);
	return value;
}

std::wstring sFTPInfo::GetPort() const
{
	std::wstring value;
	DmCS::StringFn::From(value, m_Port);
	return value;
}

std::wstring sFTPInfo::GetRequestPort() const
{
	std::wstring value;
	DmCS::StringFn::From(value, m_RequestPort);
	return value;
}


std::string sFTPInfo::GetPortA() const
{
	std::wstring portW = GetPort();
	std::string value;
	DmCS::StringFn::ToMB(portW, value);
	return value;
}

std::wstring sFTPInfo::GetID() const
{
	std::wstring value;
	DmCS::StringFn::From(value, m_ID);
	return value;	
}

std::wstring sFTPInfo::GetPW() const
{
	std::wstring value;
	DmCS::StringFn::From(value, m_PW);
	return value;
}

std::wstring sFTPInfo::GetDirectory() const
{
	std::wstring value;
	DmCS::StringFn::From(value, m_Directory);
	return value;
}

std::wstring sFTPInfo::GetFileName() const
{
	std::wstring value;
	DmCS::StringFn::From(value, m_FileName);
	return value;
}

void sFTPInfo::SetHost( TCHAR const* pHost )
{
	DmCS::StringFn::ToMB( pHost, m_Host );
}

void sFTPInfo::SetPort( TCHAR const* pPort )
{
	m_Port = DmCS::StringFn::ToU( pPort );
}

void sFTPInfo::SetRequestPort(TCHAR const* pPort)
{
	m_RequestPort = DmCS::StringFn::ToU( pPort );
}

void sFTPInfo::SetID( TCHAR const* pID )
{
	DmCS::StringFn::ToMB( pID, m_ID );
}

void sFTPInfo::SetPW( TCHAR const* pPW )
{
	DmCS::StringFn::ToMB( pPW, m_PW );
}

void sFTPInfo::SetDirectory( TCHAR const* pDir )
{
	DmCS::StringFn::ToMB( pDir, m_Directory );
}

void sFTPInfo::SetFileName( TCHAR const* pFile )
{
	DmCS::StringFn::ToMB( pFile, m_FileName );
}

//////////////////////////////////////////////////////////////////////////
sConfige::sConfige():m_bAutoUpdate(false)
{

}

sConfige::~sConfige()
{

}

void sConfige::ResetData()
{
	m_bAutoUpdate = false;
	m_TitleName.clear();

	m_IncludeFiles.clear();
	m_IncludeFolders.clear();
}

bool sConfige::IsAutolUpdate() const
{
	return m_bAutoUpdate;
}

bool sConfige::LoadConfigFile(std::string const& ConfigeFile)
{
	std::ifstream jsonFile( ConfigeFile.c_str() );
	if( jsonFile.fail() )
		return false;

	Json::Reader reader;
	Json::Value root;
	bool parsingSuccessful = reader.parse( jsonFile, root );
	if ( !parsingSuccessful )
		return false;

	m_TitleName			= root.get("TitleName","ResourceIntegrityTool").asWString();
	m_bAutoUpdate		= root.get("AutoUpdate","").asBool();

	m_HashData_FTP_Info.m_Host		= root.get("Hashdata_FTP_Host","").asString();
	m_HashData_FTP_Info.m_Port		= root.get("Hashdata_FTP_Port",0).asUInt();
	m_HashData_FTP_Info.m_ID		= root.get("Hashdata_FTP_ID","").asString();
	m_HashData_FTP_Info.m_PW		= root.get("Hashdata_FTP_PW","").asString();
	m_HashData_FTP_Info.m_Directory	= root.get("Hashdata_FTP_Directory","").asString();
	m_HashData_FTP_Info.m_FileName	= root.get("Hashdata_FTP_FileName","").asString();
	m_UserConnectionID				= root.get("Hashdata_FTP_UserID","").asString();
	m_UserConnectionPW				= root.get("Hashdata_FTP_UserPW","").asString();

	{
		Json::Value const& jFiles = root.get( "Include_Files", "" );	
		Json::Value::iterator it = jFiles.begin();
		for( ; it != jFiles.end(); ++it )
		{
			std::string sFile = (*it).asString();
			DmCS::StringFn::Lwr( sFile );
			m_IncludeFiles.push_back( sFile );
		}
	}

	{
		Json::Value const& jValue = root.get( "Include_Folder", "" );	
		Json::Value::iterator it = jValue.begin();
		for( ; it != jValue.end(); ++it )
		{
			sIncludeFolderInfo addInfo;

			addInfo.m_Folder = (*it).get("path","").asString();
			DmCS::StringFn::Lwr( addInfo.m_Folder );

			Json::Value const& jExt = (*it).get( "Ext", "" );	
			Json::Value::iterator secit = jExt.begin();
			for( ; secit != jExt.end(); ++secit )
			{
				std::string sExt = (*secit).asString();
				DmCS::StringFn::Lwr( sExt );
				addInfo.m_FileExt.push_back( sExt );
			}	

			m_IncludeFolders.push_back( addInfo );
		}
	}

	
	m_Patch_FTP_Info.m_Host	= root.get("PatchUpload_FTP_Host","").asString();
	m_Patch_FTP_Info.m_Port = root.get("PatchUpload_FTP_Port",0).asUInt();
	m_Patch_FTP_Info.m_ID	 = root.get("PatchUpload_FTP_ID","").asString();
	m_Patch_FTP_Info.m_PW	 = root.get("PatchUpload_FTP_PW","").asString();
	m_Patch_FTP_Info.m_Directory = root.get("PatchUpload_FTP_Directory","").asString();

	
	m_Server_FTP_Info.m_Host		= root.get("ServerUpload_FTP_Host","").asString();
	m_Server_FTP_Info.m_Port		= root.get("ServerUpload_FTP_Port",0).asUInt();
	m_Server_FTP_Info.m_ID			= root.get("ServerUpload_FTP_ID","").asString();
	m_Server_FTP_Info.m_PW			= root.get("ServerUpload_FTP_PW","").asString();
	m_Server_FTP_Info.m_Directory	= root.get("ServerUpload_FTP_Directory","").asString();
	m_Server_FTP_Info.m_FileName	= root.get("ServerUpload_FileName","").asString();
	m_Server_FTP_Info.m_RequestPort = root.get("ServerUpload_FTP_RequestPort",0).asUInt();

	m_UserHashLog_FTP_Info.m_Host		= root.get("UserHashLog_FTP_Host","").asString();
	m_UserHashLog_FTP_Info.m_Port		= root.get("UserHashLog_FTP_Port",0).asUInt();
	m_UserHashLog_FTP_Info.m_ID			= root.get("UserHashLog_FTP_ID","").asString();
	m_UserHashLog_FTP_Info.m_PW			= root.get("UserHashLog_FTP_PW","").asString();
	m_UserHashLog_FTP_Info.m_Directory	= root.get("UserHashLog_FTP_Directory","").asString();
	m_UserHashLog_FTP_Info.m_FileName	= root.get("UserHashLog_FTP_FileName","").asString();

	m_ClientFileName					= root.get("ClientFileName","").asString();
	return true;
}

std::wstring sConfige::GetTitleName() const
{
	return m_TitleName;
}

sFTPInfo const& sConfige::GetHashDataFTPInfo() const
{
	return m_HashData_FTP_Info;
}

sFTPInfo const& sConfige::GetServerUploadFTPInfo() const
{
	return m_Server_FTP_Info;
}

sFTPInfo const& sConfige::GetPatchUploadFTPInfo() const
{
	return m_Patch_FTP_Info;
}

sFTPInfo const& sConfige::GetUserHashLogFTPInfo() const
{
	return m_UserHashLog_FTP_Info;
}

//////////////////////////////////////////////////////////////////////////
std::list<std::string> const& sConfige::GetIncludeFiles() const
{
	return m_IncludeFiles;
}

std::list<std::string> const * sConfige::GetIncludeFilesExt(std::string const& folder) const
{
	std::list<sIncludeFolderInfo>::const_iterator it = m_IncludeFolders.begin();
	for( ; it != m_IncludeFolders.end(); ++it )
	{
		if( 0 != (*it).m_Folder.compare( folder ) )
			continue;

		return &(*it).m_FileExt;
	}
	return NULL;
}

std::list<sIncludeFolderInfo> const& sConfige::GetIncludeFolders() const
{
	return m_IncludeFolders;
}

bool sConfige::CheckIncludeFile( std::string const& str )
{
	std::list<std::string>::const_iterator it = m_IncludeFiles.begin();
	for( ; it != m_IncludeFiles.end(); ++it )
	{
		if( 0 == str.compare( (*it) ) )
			return true;
	}
	return false;
}

bool sConfige::CheckIncludeDirectory( std::string const& str )
{
	std::list<sIncludeFolderInfo>::const_iterator it = m_IncludeFolders.begin();
	for( ; it != m_IncludeFolders.end(); ++it )
	{
		if( (*it).m_Folder.empty() )
			continue;

		if( std::string::npos == str.find( (*it).m_Folder ) )
			continue;

		if( (*it).m_FileExt.empty() )
			return true;

		std::string ext = DmCS::FileFn::GetFileExt( str.c_str() );
		if( ext.empty() )
			return false;

		std::list<std::string>::const_iterator secit = (*it).m_FileExt.begin();
		for ( ; secit != (*it).m_FileExt.end(); ++secit )
		{
			if( 0 == (*secit).compare( ext ) )
				return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
void sConfige::SetPatchInfo_Directory(TCHAR const* pDir)
{
	m_Patch_FTP_Info.SetDirectory( pDir );
}

void sConfige::SetPatchInfo_Host(TCHAR const* pHost)
{
	m_Patch_FTP_Info.SetHost( pHost );
}

void sConfige::SetPatchInfo_Port(TCHAR const* pPort)
{
	m_Patch_FTP_Info.SetPort( pPort );
}

void sConfige::SetPatchInfo_ID(TCHAR const* pID)
{
	m_Patch_FTP_Info.SetID( pID );
}

void sConfige::SetPatchInfo_PW(TCHAR const* pPW)
{
	m_Patch_FTP_Info.SetPW( pPW );
}

void sConfige::SetPatchInfo_File(TCHAR const* pFile)
{
	m_Patch_FTP_Info.SetFileName( pFile );
}
//////////////////////////////////////////////////////////////////////////
void sConfige::SetHashFTPInfo_Directory(TCHAR const* pDir)
{
	m_HashData_FTP_Info.SetDirectory( pDir );
}

void sConfige::SetHashFTPInfo_Host(TCHAR const* pHost)
{
	m_HashData_FTP_Info.SetHost( pHost );
}

void sConfige::SetHashFTPInfo_Port(TCHAR const* pPort)
{
	m_HashData_FTP_Info.SetPort( pPort );
}

void sConfige::SetHashFTPInfo_ID(TCHAR const* pID)
{
	m_HashData_FTP_Info.SetID( pID );
}

void sConfige::SetHashFTPInfo_PW(TCHAR const* pPW)
{
	m_HashData_FTP_Info.SetPW( pPW );
}

void sConfige::SetHashFTPInfo_File(TCHAR const* pFile)
{
	m_HashData_FTP_Info.SetFileName( pFile );
}
//////////////////////////////////////////////////////////////////////////
void sConfige::SetServerInfo_Directory(TCHAR const* pDir)
{
	m_Server_FTP_Info.SetDirectory( pDir );
}

void sConfige::SetServerInfo_Host(TCHAR const* pHost)
{
	m_Server_FTP_Info.SetHost( pHost );
}

void sConfige::SetServerInfo_Port(TCHAR const* pPort)
{
	m_Server_FTP_Info.SetPort( pPort );
}

void sConfige::SetServerInfo_RequestPort(TCHAR const* pPort)
{
	m_Server_FTP_Info.SetRequestPort( pPort );
}

void sConfige::SetServerInfo_ID(TCHAR const* pID)
{
	m_Server_FTP_Info.SetID( pID );
}

void sConfige::SetServerInfo_PW(TCHAR const* pPW)
{
	m_Server_FTP_Info.SetPW( pPW );
}

void sConfige::SetServerInfo_File(TCHAR const* pFile)
{
	m_Server_FTP_Info.SetFileName( pFile );
}
//////////////////////////////////////////////////////////////////////////
void sConfige::SetUserHashLogFTPInfo_Host(TCHAR const* pHost)
{
	m_UserHashLog_FTP_Info.SetHost( pHost );
}

void sConfige::SetUserHashLogFTPInfo_Port(TCHAR const* pPort)
{
	m_UserHashLog_FTP_Info.SetPort( pPort );
}

void sConfige::SetUserHashLogFTPInfo_ID(TCHAR const* pID)
{
	m_UserHashLog_FTP_Info.SetID( pID );
}

void sConfige::SetUserHashLogFTPInfo_PW(TCHAR const* pPW)
{
	m_UserHashLog_FTP_Info.SetPW( pPW );
}

void sConfige::SetUserHashLogFTPInfo_Directory(TCHAR const* pDir)
{
	m_UserHashLog_FTP_Info.SetDirectory( pDir );
}

void sConfige::SetUserHashLogFTPInfo_File(TCHAR const* pFile)
{
	m_UserHashLog_FTP_Info.SetFileName( pFile );
}

//////////////////////////////////////////////////////////////////////////
std::string sConfige::GetUserConnectionID() const
{
	return m_UserConnectionID;
}

std::string	sConfige::GetUserConnectionPW() const
{
	return m_UserConnectionPW;
}

std::wstring sConfige::GetUserConnectionIDW() const
{
	std::wstring value;
	DmCS::StringFn::From(value, m_UserConnectionID);
	return value;
}

std::wstring	sConfige::GetUserConnectionPWW() const
{
	std::wstring value;
	DmCS::StringFn::From(value, m_UserConnectionPW);
	return value;
}


void sConfige::SetUserConnectionID(std::string const& id)
{
	m_UserConnectionID = id;
}

void sConfige::SetUserConnectionPW(std::string const& pw)
{
	m_UserConnectionPW = pw;
}

void sConfige::SetUserConnectionID(std::wstring const& id)
{
	DmCS::StringFn::ToMB( id, m_UserConnectionID );
}

void sConfige::SetUserConnectionPW(std::wstring const& pw)
{
	DmCS::StringFn::ToMB( pw, m_UserConnectionPW );
}

std::string	sConfige::GetClientFileName() const
{
	return m_ClientFileName;
}