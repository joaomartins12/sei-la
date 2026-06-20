#pragma once

typedef struct sFilesInfo
{
	std::string		m_sFullPath;
	std::string		m_sPackPath;
}SPFileInfo;

typedef std::list<SPFileInfo>					LIST_FILEINFO;
typedef std::list<SPFileInfo>::iterator			LIST_FILEINFO_IT;
typedef std::list<SPFileInfo>::const_iterator	LIST_FILEINFO_CIT;

typedef struct tagPatchFileInfo
{
public:
	tagPatchFileInfo();
	tagPatchFileInfo(char const* pFolder);
	~tagPatchFileInfo();

	size_t					LoadPatchFileListFromFolder( char const* pFolder );
	size_t					LoadPatchFileList(char const* pFolder);
	DWORD					CheckNewFileCount( CsFileHash* pHashFile );

	int						GetNewFileCount() const;
	LIST_FILEINFO const&	GetFileInfo() const;

private:
	std::string				replace(std::string const& sOrg, std::string const& sFind, std::string const& sReplace);

private:
	LIST_FILEINFO			m_FileInfo;
	int						m_nNewFileCount;

}SPatchFileInfo;

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


typedef struct sPatchPackInfo
{
	sPatchPackInfo():m_pFilePack(NULL),m_pFileHash(NULL)
	{};

	~sPatchPackInfo()
	{
		Delete();
	};

	void		Delete();
	void		DeleteFile();

	std::string		m_sPackFile;		// 패치 패킹 파일
	std::string		m_sPackHader;		// 패치 패킹 헤더 파일

	CsFilePack*		m_pFilePack;
	CsFileHash*		m_pFileHash;
}SPPackInfo;

typedef std::map<int, SPPackInfo>					MAP_FILEINFO;
typedef std::map<int, SPPackInfo>::iterator			MAP_FILEINFO_IT;
typedef std::map<int, SPPackInfo>::const_iterator	MAP_FILEINFO_CIT;

typedef struct tagPatchPackInfo
{
public:
	tagPatchPackInfo();
	tagPatchPackInfo(char const* pFolder);
	~tagPatchPackInfo();

	size_t					LoadPatchPackFileList(char const* pFolder);
	size_t					LoadPatchPackFile( char const* pFolder, int nPackNum );
	DWORD					CheckNewFileCount( CsFileHash* pHashFile );
	int						GetNewFileCount() const;

	MAP_FILEINFO const&		GetFileInfo() const;

	void					DeleteData();

private:
	MAP_FILEINFO			m_FileInfo;
	int						m_nNewFileCount;

}SPatchPackInfo;