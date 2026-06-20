
#pragma once

struct sIncludeFolderInfo
{
	std::string				m_Folder;
	std::list<std::string>	m_FileExt;
};

struct sFTPInfo
{
	sFTPInfo();
	~sFTPInfo();

	void Reset();

	std::wstring GetHost() const;
	std::wstring GetPort() const;
	std::wstring GetRequestPort() const;
	std::string  GetPortA() const;
	std::wstring GetID() const;
	std::wstring GetPW() const;
	std::wstring GetDirectory() const;
	std::wstring GetFileName() const;

	void	SetHost( TCHAR const* pHost );
	void	SetPort( TCHAR const* pPort );
	void	SetRequestPort( TCHAR const* pPort );	
	void	SetID( TCHAR const* pID );
	void	SetPW( TCHAR const* pPW );
	void	SetDirectory( TCHAR const* pDir );
	void	SetFileName( TCHAR const* pFile );


	std::string m_Host;
	unsigned int m_Port;
	unsigned int m_RequestPort;
	std::string m_ID;		
	std::string m_PW;
	std::string m_Directory;
	std::string m_FileName;
};

struct sConfige
{
public:
	sConfige();
	~sConfige();

	void			ResetData();
	bool			IsAutolUpdate() const;

	bool			LoadConfigFile(std::string const& ConfigeFile);

	std::wstring	GetTitleName() const;

	sFTPInfo const& GetHashDataFTPInfo() const;
	sFTPInfo const& GetServerUploadFTPInfo() const;
	sFTPInfo const& GetPatchUploadFTPInfo() const;
	sFTPInfo const& GetUserHashLogFTPInfo() const;

	std::list<std::string> const& GetIncludeFiles() const;
	std::list<std::string> const* GetIncludeFilesExt(std::string const& folder) const;
	std::list<sIncludeFolderInfo> const& GetIncludeFolders() const;

	bool			CheckIncludeFile( std::string const& str );
	bool			CheckIncludeDirectory( std::string const& str );
//////////////////////////////////////////////////////////////////////////
	void			SetPatchInfo_Host(TCHAR const* pHost);
	void			SetPatchInfo_Port(TCHAR const* pPort);
	void			SetPatchInfo_ID(TCHAR const* pID);
	void			SetPatchInfo_PW(TCHAR const* pPW);
	void			SetPatchInfo_Directory(TCHAR const* pDir);
	void			SetPatchInfo_File(TCHAR const* pFile);
	//////////////////////////////////////////////////////////////////////////
	void			SetHashFTPInfo_Host(TCHAR const* pHost);
	void			SetHashFTPInfo_Port(TCHAR const* pPort);
	void			SetHashFTPInfo_ID(TCHAR const* pID);
	void			SetHashFTPInfo_PW(TCHAR const* pPW);
	void			SetHashFTPInfo_Directory(TCHAR const* pDir);
	void			SetHashFTPInfo_File(TCHAR const* pFile);
	//////////////////////////////////////////////////////////////////////////
	void			SetServerInfo_Host(TCHAR const* pHost);
	void			SetServerInfo_Port(TCHAR const* pPort);
	void			SetServerInfo_RequestPort(TCHAR const* pPort);
	void			SetServerInfo_ID(TCHAR const* pID);
	void			SetServerInfo_PW(TCHAR const* pPW);
	void			SetServerInfo_Directory(TCHAR const* pDir);
	void			SetServerInfo_File(TCHAR const* pFile);
	//////////////////////////////////////////////////////////////////////////
	void			SetUserHashLogFTPInfo_Host(TCHAR const* pHost);
	void			SetUserHashLogFTPInfo_Port(TCHAR const* pPort);
	void			SetUserHashLogFTPInfo_ID(TCHAR const* pID);
	void			SetUserHashLogFTPInfo_PW(TCHAR const* pPW);
	void			SetUserHashLogFTPInfo_Directory(TCHAR const* pDir);
	void			SetUserHashLogFTPInfo_File(TCHAR const* pFile);

	std::string		GetUserConnectionID() const;
	std::string		GetUserConnectionPW() const;
	std::wstring	GetUserConnectionIDW() const;
	std::wstring	GetUserConnectionPWW() const;
	
	void			SetUserConnectionID(std::string const& id);
	void			SetUserConnectionPW(std::string const& pw);
	void			SetUserConnectionID(std::wstring const& id);
	void			SetUserConnectionPW(std::wstring const& pw);

	std::string		GetClientFileName() const;

private:
	bool							m_bAutoUpdate;
	std::wstring					m_TitleName;

	sFTPInfo						m_HashData_FTP_Info;
	sFTPInfo						m_Server_FTP_Info;
	sFTPInfo						m_Patch_FTP_Info;
	sFTPInfo						m_UserHashLog_FTP_Info;	// 해쉬가 다를 경우 로그를 남길 FTP 정보

	std::list<std::string>			m_IncludeFiles;
	std::list<sIncludeFolderInfo>	m_IncludeFolders;

	std::string						m_UserConnectionID;
	std::string						m_UserConnectionPW;

 	std::string						m_ClientFileName;
};
