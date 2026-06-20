#include "stdafx.h"
#include <sstream>
#include "CsPatchFileInfo.h"

void sPatchPackInfo::Delete()
{
	if( m_pFilePack )
	{
		m_pFilePack->Delete();
		SAFE_DELETE( m_pFilePack );
	}
	if( m_pFileHash )
	{
		m_pFileHash->Delete();
		SAFE_DELETE( m_pFileHash );
	}

	DeleteFile();
}

void sPatchPackInfo::DeleteFile()
{
	if(!m_sPackHader.empty())
		DeleteFileA( m_sPackHader.c_str() );
	if(!m_sPackFile.empty())
		DeleteFileA( m_sPackFile.c_str() );
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

tagPatchFileInfo::tagPatchFileInfo():m_nNewFileCount(0)
{
	m_FileInfo.clear();
}

tagPatchFileInfo::tagPatchFileInfo(char const* pFolder):m_nNewFileCount(0)
{
	m_FileInfo.clear();
	LoadPatchFileListFromFolder( pFolder );
}

tagPatchFileInfo::~tagPatchFileInfo()
{
	m_FileInfo.clear();
}

std::string tagPatchFileInfo::replace(std::string const& sOrg, std::string const& sFind, std::string const& sReplace)
{
	std::string org = sOrg;
	for(std::string::size_type findpos = org.find(sFind);findpos != std::string::npos;findpos = org.find(sFind))
		org.replace(findpos, sFind.size(), sReplace);
	return org;
}

size_t tagPatchFileInfo::LoadPatchFileListFromFolder( char const* pFolder )
{
	std::list<std::string> FileList;
	nsCSFILE::DirectoryFindFilesA(FileList,pFolder);

	if( !FileList.empty() )
	{
		std::string delPath = pFolder;
		delPath += "\\";
		
		std::list<std::string>::iterator it = FileList.begin();
		for( ; it != FileList.end(); ++it )
		{
			SPFileInfo addFile;
			addFile.m_sFullPath = (*it);
			addFile.m_sPackPath = replace( addFile.m_sFullPath, delPath, "" );

			m_FileInfo.push_back( addFile );
		}
	}

	return m_FileInfo.size();
}

size_t tagPatchFileInfo::LoadPatchFileList( char const* pFolder )
{
	std::list<std::string> FileList;
	nsCSFILE::FindFilesA(pFolder,FileList);

	if( !FileList.empty() )
	{
		std::string delPath = pFolder;
		delPath += "\\";

		std::list<std::string>::iterator it = FileList.begin();
		for( ; it != FileList.end(); ++it )
		{
			SPFileInfo addFile;
			addFile.m_sFullPath = (*it);
			addFile.m_sPackPath = replace( addFile.m_sFullPath, delPath, "" );

			m_FileInfo.push_back( addFile );
		}
	}

	return m_FileInfo.size();
}

DWORD tagPatchFileInfo::CheckNewFileCount( CsFileHash* pHashFile )
{
	m_nNewFileCount = 0;

	if( NULL == pHashFile )
		return ERROR_HASH_FILE_NULL;

	LIST_FILEINFO_CIT it = m_FileInfo.begin();
	for( ; it != m_FileInfo.end(); ++it )
	{
		if( !pHashFile->IsExistHashCode( (*it).m_sPackPath.c_str() ) )
			m_nNewFileCount++;
	}

	return ERROR_OK;
}

LIST_FILEINFO const& tagPatchFileInfo::GetFileInfo() const
{
	return m_FileInfo;
}

int tagPatchFileInfo::GetNewFileCount() const
{
	return m_nNewFileCount;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


tagPatchPackInfo::tagPatchPackInfo():m_nNewFileCount(0)
{
	m_FileInfo.clear();
}

tagPatchPackInfo::tagPatchPackInfo(char const* pFolder):m_nNewFileCount(0)
{
	m_FileInfo.clear();
	LoadPatchPackFileList( pFolder );
}

tagPatchPackInfo::~tagPatchPackInfo()
{
	m_FileInfo.clear();
}


void tagPatchPackInfo::DeleteData()
{
	m_FileInfo.clear();
}

size_t tagPatchPackInfo::LoadPatchPackFile( char const* pFolder, int nPackNum )
{	
	std::stringstream pf;
	pf << pFolder << ASB_FD_MARK << PATCH_PACK_NAME << nPackNum << PACK_EX_NAME;
	std::string patchPackFile;
	patchPackFile = pf.str();

	
	std::string patchHeaderFile;
	std::stringstream hf;
	hf << pFolder << ASB_FD_MARK << PATCH_PACK_NAME << nPackNum << HASH_EX_NAME;
	patchHeaderFile = hf.str();

	if( 0 != _access_s( patchPackFile.c_str(), 0 ) || 0 != _access_s( patchHeaderFile.c_str(), 0 ) )
		return 0;

	m_FileInfo.clear();

	MAP_FILEINFO::_Pairib result = m_FileInfo.insert( std::make_pair( nPackNum, SPPackInfo() ) );
	if( !result.second )
		return 0;

	result.first->second.m_pFilePack = csnew CsFilePack;
	result.first->second.m_pFileHash = csnew CsFileHash;
	result.first->second.m_sPackFile = patchPackFile;
	result.first->second.m_sPackHader = patchHeaderFile;
	return m_FileInfo.size();
}

size_t tagPatchPackInfo::LoadPatchPackFileList( char const* pFolder )
{	
	std::list<std::string> FileList;
	nsCSFILE::FindFilesA(pFolder, FileList );

	std::list<std::string>::iterator it = FileList.begin();
	for( ; it != FileList.end(); ++it )
	{
		std::string::size_type Spos = (*it).find("Patch_");
		if( std::string::npos == Spos )
			continue;

		std::string::size_type Epos = (*it).find(".", Spos);
		if( std::string::npos == Spos )
			continue;

		if( std::string::npos != (*it).find("pf", Epos) && std::string::npos != (*it).find("hf", Epos) )
			continue;

		std::string patchNum = (*it).substr( Spos + 5, Epos - Spos - 5 );
		int nPatchNum = atoi( patchNum.c_str() );

		MAP_FILEINFO_IT mapIt = m_FileInfo.find( nPatchNum );
		if( mapIt == m_FileInfo.end() )
		{
			MAP_FILEINFO::_Pairib result = m_FileInfo.insert( std::make_pair( nPatchNum, SPPackInfo() ) );
			if( !result.second )
				continue;

			mapIt = result.first;
			mapIt->second.m_pFilePack = csnew CsFilePack;
			mapIt->second.m_pFileHash = csnew CsFileHash;
		}

		if( std::string::npos != (*it).find("pf") )
			mapIt->second.m_sPackFile = (*it);
		else if( std::string::npos != (*it).find("hf") )
			mapIt->second.m_sPackHader = (*it);
	}

	return m_FileInfo.size();
}

DWORD tagPatchPackInfo::CheckNewFileCount( CsFileHash* pHashFile )
{
	m_nNewFileCount = 0;

	if( NULL == pHashFile )
		return ERROR_HASH_FILE_NULL;

	MAP_FILEINFO_CIT it = m_FileInfo.begin();
	for( ; it != m_FileInfo.end(); ++it )
	{
		DWORD dwResult = it->second.m_pFileHash->Load(it->second.m_sPackHader.c_str());
		if( ERROR_OK != dwResult )
			return dwResult;

		dwResult = it->second.m_pFilePack->Load(it->second.m_pFileHash, it->second.m_sPackFile.c_str(), true );
		if( ERROR_OK != dwResult )
			return dwResult;

		std::map< size_t, CsFileHash::sINFO* >* pMap = it->second.m_pFileHash->GetHashMap();
		std::map< size_t, CsFileHash::sINFO* >::iterator mapit = pMap->begin();
		std::map< size_t, CsFileHash::sINFO* >::iterator mapitEnd = pMap->end();
		for( ; mapit != mapitEnd; ++mapit )
		{
			CsFilePack::sCHUNK Chunk;
			it->second.m_pFilePack->_GetChunk( &Chunk, mapit->second->s_nOffset );
			if( !pHashFile->IsExistHashCode( Chunk.s_szPath ) )
				m_nNewFileCount++;
		}
	}

	return ERROR_OK;
}

MAP_FILEINFO const& tagPatchPackInfo::GetFileInfo() const
{
	return m_FileInfo;
}

int tagPatchPackInfo::GetNewFileCount() const
{
	return m_nNewFileCount;
}
