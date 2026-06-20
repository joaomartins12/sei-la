

#include "stdafx.h"
#include "CsFilePackMng.h"

bool CsFilePackMng::ReadFilePack( std::vector<std::string> const& vecReadFiles )
{
	std::vector<std::string>::const_iterator it = vecReadFiles.begin();
	for( ; it != vecReadFiles.end(); ++it )
	{
		if( NULL == LoadFilePack( (*it).c_str() ) )
			return false;
	}

	return true;
}

CsFilePackMng::CsFilePackMng()
{
	m_pOpendFile = NULL;
	m_pBuffer = NULL;
	Init();
}

CsFilePackMng::~CsFilePackMng()
{
	Delete();
}


void CsFilePackMng::Init()
{
	assert_cs( m_pBuffer == NULL );
	SAFE_DELETE_ARRAY( m_pBuffer );
	m_pBuffer = csnew char[ FP_BLOCK_SIZE ];
}

void CsFilePackMng::Delete()
{
	m_LoadPackInfo.DeleteData();
	m_pOpendFile = NULL;
	DeleteVPFile();

	SAFE_DELETE_ARRAY( m_pBuffer );
}

void CsFilePackMng::DeleteVPFile()
{
	int nCount = m_vpFile.Size();
	for( int i=0; i<nCount; ++i )
	{
		delete m_vpFile[ i ]->s_pFilePack;
		delete m_vpFile[ i ]->s_pFileHash;
		delete m_vpFile[ i ];
	}
	m_vpFile.Destroy();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
// 
// 패치할 파일을 읽어 들인다.
size_t CsFilePackMng::LoadPackingFile_Files( LPCSTR pPath )
{
	return m_LoadFileInfo.LoadPatchFileListFromFolder( pPath );
}


size_t CsFilePackMng::LoadPatchFile_Pack( LPCSTR pPath, int nPatchNum )
{
	return m_LoadPackInfo.LoadPatchPackFile( pPath, nPatchNum );
}

size_t CsFilePackMng::LoadPatchFile_PackList( LPCSTR pPath )
{
	return m_LoadPackInfo.LoadPatchPackFileList( pPath );
}

void CsFilePackMng::DeletePackFile( LPCSTR szPackName )
{
	std::string sPackFileName = szPackName;
	sPackFileName += PACK_EX_NAME;

	DWORD mode = GetFileAttributesA( sPackFileName.c_str() );
	mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY );
	SetFileAttributesA( sPackFileName.c_str(), mode );

	DeleteFileA( sPackFileName.c_str() );

	std::string sPackHashName = szPackName;
	sPackHashName += HASH_EX_NAME;
	mode = GetFileAttributesA( sPackHashName.c_str() );
	mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY );
	SetFileAttributesA( sPackHashName.c_str(), mode );

	DeleteFileA( sPackHashName.c_str() );
}

bool CsFilePackMng::CheckPackFile( LPCSTR szPackName )
{
	std::string sPackFileName = szPackName;
	sPackFileName += PACK_EX_NAME;
	std::string sPackHashName = szPackName;
	sPackHashName += HASH_EX_NAME;

	if( _access_s( sPackFileName.c_str(), 0 ) != 0 || _access_s( sPackHashName.c_str(), 0 ) != 0 )
		return false;
	
	return true;
}

bool CsFilePackMng::LoadFilePack( LPCSTR szPackName, bool bWrite )
{
	if( !CheckPackFile( szPackName ) )
	{
		if( bWrite )
			return NewFilePack( szPackName );// 파일에 적근할 수 없는 경우 새로 파일 생성
		else
			return false;
	}

	std::string sPackFileName = szPackName;
	sPackFileName += PACK_EX_NAME;
	sFILE* pFile = csnew sFILE;

	pFile->s_pFileHash = csnew CsFileHash;
	std::string sPackHashName = szPackName;
	sPackHashName += HASH_EX_NAME;
	if( ERROR_OK != pFile->s_pFileHash->Load( sPackHashName.c_str() ) )
	{
		SAFE_DELETE( pFile->s_pFilePack );
		SAFE_DELETE( pFile->s_pFileHash );
		return false;
	}

	pFile->s_pFilePack = csnew CsFilePack;
	if( ERROR_OK != pFile->s_pFilePack->Load( pFile->s_pFileHash, sPackFileName.c_str(), bWrite ) )
	{
		SAFE_DELETE( pFile->s_pFilePack );
		return false;
	}

	m_vpFile.PushBack( pFile );

	m_pOpendFile = pFile;
	
	return true;
}

bool CsFilePackMng::NewFilePack( LPCSTR szPackName )
{	
	std::string sPackFileName = szPackName;
	sPackFileName += PACK_EX_NAME;
	std::string sPackHashName = szPackName;
	sPackHashName += HASH_EX_NAME;

	sFILE* pFile = csnew sFILE;
	pFile->s_pFilePack = csnew CsFilePack;
	pFile->s_pFileHash = csnew CsFileHash;

	// 파일 패킹 데이터 생성	
	pFile->s_pFilePack->New( pFile->s_pFileHash, sPackFileName.c_str() );
	pFile->s_pFileHash->New( sPackHashName.c_str() );
	pFile->s_pFileHash->SaveFile();
	m_vpFile.PushBack( pFile );

	m_pOpendFile = pFile;
	return true;
}

DWORD CsFilePackMng::CheckNewFileCount_Pack()
{
	if(NULL == m_pOpendFile )
		return ERROR_NOT_OPENED_PACKFILE;

	return m_LoadPackInfo.CheckNewFileCount( m_pOpendFile->s_pFileHash );
}

DWORD CsFilePackMng::CheckNewFileCount_File()
{
	if(NULL == m_pOpendFile )
		return ERROR_NOT_OPENED_PACKFILE;

	return m_LoadFileInfo.CheckNewFileCount( m_pOpendFile->s_pFileHash );
}

MAP_FILEINFO const& CsFilePackMng::GetPatchListInfo() const
{
	return m_LoadPackInfo.GetFileInfo();
}

LIST_FILEINFO const& CsFilePackMng::GetFileInfo() const
{
	return m_LoadFileInfo.GetFileInfo();
}

bool CsFilePackMng::SetAllocCount_File()
{
	return SetAllocCount( m_LoadFileInfo.GetNewFileCount() );
}

bool CsFilePackMng::SetAllocCount_Pack()
{
	return SetAllocCount( m_LoadPackInfo.GetNewFileCount() );
}

bool CsFilePackMng::SetAllocCount(int nCount)
{
	if(NULL == m_pOpendFile )
		return false;

	CsFileHash* pFileHash = m_pOpendFile->s_pFileHash;
	if( NULL == pFileHash )
		return false;

	CsFileHash::sHEADER* pFileHeader = pFileHash->GetHeader();
	if( NULL == pFileHeader )
		return false;

	pFileHash->SetAllocCount( pFileHeader->s_nFileCount + nCount );
	return true;
}

bool CsFilePackMng::SaveHashFile()
{
	if(NULL == m_pOpendFile )
		return false;

	if(NULL == m_pOpendFile->s_pFileHash )
		return false;
	
	m_pOpendFile->s_pFileHash->SaveFile();
	return true;
}

bool CsFilePackMng::DoPatchPacking_Pack()
{
	if(NULL == m_pOpendFile )
		return false;

	CsFilePack* pFilePack = m_pOpendFile->s_pFilePack;
	if( NULL == pFilePack )
		return false;

	MAP_FILEINFO const& listFileInfo = m_LoadPackInfo.GetFileInfo();
	MAP_FILEINFO_CIT it = listFileInfo.begin();
	for( ; it != listFileInfo.end(); ++it )
	{
		std::map< size_t, CsFileHash::sINFO* >* pMap = it->second.m_pFileHash->GetHashMap();
		std::map< size_t, CsFileHash::sINFO* >::iterator mapIt = pMap->begin();
		std::map< size_t, CsFileHash::sINFO* >::iterator mapItEnd = pMap->end();
		for( ; mapIt != mapItEnd; ++mapIt )
		{
			if( !pFilePack->_EditFile( it->second.m_pFilePack, mapIt->second ) )
				return false;
		}
	}
	return true;
}

bool CsFilePackMng::DoUnPack()
{
	if(NULL == m_pOpendFile )
		return false;

	if(NULL == m_pOpendFile->s_pFileHash )
		return false;

	std::map< size_t, CsFileHash::sINFO* >* pMap = m_pOpendFile->s_pFileHash->GetHashMap();
	std::map< size_t, CsFileHash::sINFO* >::iterator it = pMap->begin();
	std::map< size_t, CsFileHash::sINFO* >::iterator itEnd = pMap->end();
	CsFilePack::sCHUNK Chunk;
	FILE* pCreateFile = NULL;
	char* pAlloc = NULL;
	size_t nAllocSize = 0;
	char cSubDir[ 512 ];
	for( ; it!=itEnd; ++it )
	{
		GetChunk( &Chunk, 0, it->second->s_nOffset );		

		if( nAllocSize < it->second->s_nDataSize )
		{
			nAllocSize = it->second->s_nDataSize;
			free( pAlloc );
			pAlloc = (char*)malloc( nAllocSize );
		}

		nsCSFILE::GetFilePathOnly( cSubDir, 512, Chunk.s_szPath, true );
		nsCSFILE::CreateDirectory( cSubDir );

		DWORD modeBackup = GetFileAttributesA( Chunk.s_szPath );
		DWORD mode = modeBackup;
		mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM );
		SetFileAttributesA( Chunk.s_szPath, mode );

		fopen_s( &pCreateFile, Chunk.s_szPath, "wb" );
		assert_csm1( pCreateFile != NULL, "%s", Chunk.s_szPath );

		GetFileData( 0, &pAlloc, it->second->s_nOffset, it->second->s_nDataSize );
		fwrite( pAlloc, sizeof( char ), it->second->s_nDataSize, pCreateFile );
		fclose( pCreateFile );
	}

	if( pAlloc != NULL )
	{
		free( pAlloc );
		pAlloc = NULL;
	}

	return true;
}

CsFilePackMng::sFILE* CsFilePackMng::GetFilePack( int nIndex )
{ 
	assert_cs( m_vpFile.IsExistElement( nIndex ) ); 
	return m_vpFile[ nIndex ]; 
}

bool CsFilePackMng::IsFilePack( int nIndex )
{ 
	return m_vpFile.IsExistElement( nIndex ); 
}

bool CsFilePackMng::SeekLock( int nFilePackIndex )
{ 
	sFILE* pFile = GetFilePack( nFilePackIndex );
	if( NULL == pFile )
		return false;

	if( NULL == pFile->s_pFilePack )
		return false;

	pFile->s_pFilePack->SeekLock(); 
	return true;
}

bool CsFilePackMng::SeekUnlock( int nFilePackIndex )
{ 
	sFILE* pFile = GetFilePack( nFilePackIndex );
	if( NULL == pFile )
		return false;

	if( NULL == pFile->s_pFilePack )
		return false;

	pFile->s_pFilePack->SeekUnlock(); 
	return true;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
bool CsFilePackMng::AddFile( LPCSTR cDataPath, LPCSTR cFilePath )
{
	if(NULL == m_pOpendFile )
		return false;

	CsFilePack* pFilePack = m_pOpendFile->s_pFilePack;
	if( NULL == pFilePack )
		return false;

	return pFilePack->_AddFile( cDataPath, cFilePath, m_pBuffer );	
}

bool CsFilePackMng::EditFile( LPCSTR cDataPath, LPCSTR cFilePath )
{
	if(NULL == m_pOpendFile )
		return false;

	CsFilePack* pFilePack = m_pOpendFile->s_pFilePack;
	if( NULL == pFilePack )
		return false;

	return pFilePack->_EditFile( cDataPath, cFilePath, m_pBuffer );	
}

bool CsFilePackMng::EditFile( CsFilePack* pPatch_Pack, CsFileHash::sINFO* pPatch_HashInfo )
{
	if(NULL == m_pOpendFile )
		return false;

	CsFilePack* pFilePack = m_pOpendFile->s_pFilePack;
	if( NULL == pFilePack )
		return false;

	return pFilePack->_EditFile( pPatch_Pack, pPatch_HashInfo );	
}

int CsFilePackMng::GetFilePackIndex( sFILE* pFilePack )
{
	int nCount = m_vpFile.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpFile[ i ] == pFilePack )
			return i;
	}
	assert_cs( false );
	return 0;
}

bool CsFilePackMng::IsExistHashCode( int nFilePackIndex, LPCSTR szPath )
{
	size_t hash = CsFileHash::GetHashCode( szPath );
	if( 0 == hash )
		return false;

	if( nFilePackIndex == -1 )
	{
		int nCount = m_vpFile.Size();
		for( int i=0; i<nCount; ++i )
		{
			if( GetFilePack( i )->s_pFileHash->IsExistHashCode( hash ) == true )
				return true;
		}
		return false;
	}
	return GetFilePack( nFilePackIndex )->s_pFileHash->IsExistHashCode( hash );
}

bool CsFilePackMng::IsExistHashCode( int nFilePackIndex, size_t nHashCode )
{	
	if( 0 == nHashCode )
		return false;

	assert_cs( nFilePackIndex >= 0 );
	return GetFilePack( nFilePackIndex )->s_pFileHash->IsExistHashCode( nHashCode );
}

CsFilePack::sCHUNK* CsFilePackMng::GetChunk( CsFilePack::sCHUNK* pChunk, int nFilePackIndex, char* szPath )
{
	size_t hash = CsFileHash::GetHashCode( szPath );
	if( nFilePackIndex >= 0 )
	{
		sFILE* pFile = GetFilePack( nFilePackIndex );
		assert_cs( pFile->s_pFileHash->IsExistHashCode( hash ) );
		return pFile->s_pFilePack->_GetChunk( pChunk, pFile->s_pFileHash->GetHashInfo( hash )->s_nOffset );
	}	

	int nCount = m_vpFile.Size();
	for( int i=0; i<nCount; ++i )
	{
		if( m_vpFile[ i ]->s_pFileHash->IsExistHashCode( hash ) == true )
		{
			return m_vpFile[ i ]->s_pFilePack->_GetChunk( pChunk, m_vpFile[ i ]->s_pFileHash->GetHashInfo( hash )->s_nOffset );
		}
	}

	assert_cs( false );
	return NULL;
}

CsFilePack::sCHUNK* CsFilePackMng::GetChunk( CsFilePack::sCHUNK* pChunk, int nFilePackIndex, UINT64 nOffset )
{
	assert_cs( nFilePackIndex >= 0 );
	sFILE* pFile = GetFilePack( nFilePackIndex );	
	return pFile->s_pFilePack->_GetChunk( pChunk, nOffset );
}

CsFilePack::sCHUNK* CsFilePackMng::GetChunk( CsFilePack* pPatchPack, CsFilePack::sCHUNK* pChunk, UINT64 nOffset )
{
	return pPatchPack->_GetChunk( pChunk, nOffset );
}


CsFileHash::sINFO* CsFilePackMng::GetHashData( int nFilePackIndex, size_t nHashCode )
{
	assert_cs( nFilePackIndex >= 0 );

	sFILE* pFile = GetFilePack( nFilePackIndex );
	if( pFile->s_pFileHash->IsExistHashCode( nHashCode ) == false )
		return NULL;

	return pFile->s_pFileHash->GetHashInfo( nHashCode );
}

CsFileHash::sINFO* CsFilePackMng::GetHashData( int nFilePackIndex, char* szPath )
{
	return GetHashData( nFilePackIndex, CsFileHash::GetHashCode( szPath ) );
}

char* CsFilePackMng::GetFileData( int nFilePackIndex, char** ppData, UINT64 nOffset, size_t nSize )
{
	assert_cs( *ppData != NULL );
	assert_cs( nFilePackIndex >= 0 );
	return GetFilePack( nFilePackIndex )->s_pFilePack->_GetData( ppData, nOffset, nSize );
}

int CsFilePackMng::GetFileHandle( int nFilePackIndex, char* szPath )
{
	char c[ MAX_PATH ];
	strcpy_s( c, MAX_PATH, szPath );
	CsFileHash::sINFO* pHashInfo = GetHashData( nFilePackIndex, CsFileHash::GetHashCode( c ) );
	assert_cs( pHashInfo != NULL );

	return GetFilePack( nFilePackIndex )->s_pFilePack->_GetFileHandle( pHashInfo->s_nOffset );
}

size_t CsFilePackMng::Allocate_GetFileData( int nFilePackIndex, char** ppData, size_t nHashCode )
{
	CsFileHash::sINFO* pHashInfo = GetHashData( nFilePackIndex, nHashCode );
	assert_cs( pHashInfo != NULL );
	*ppData = csnew char[ pHashInfo->s_nDataSize ];
	GetFileData( nFilePackIndex, ppData, pHashInfo->s_nOffset, pHashInfo->s_nDataSize );
	return pHashInfo->s_nDataSize;
}

size_t CsFilePackMng::Allocate_GetFileData( int nFilePackIndex, char** ppData, char* szPath )
{
	char c[ MAX_PATH ];
	strcpy_s( c, MAX_PATH, szPath );
	assert_cs( *ppData == NULL );
	return Allocate_GetFileData( nFilePackIndex, ppData, CsFileHash::GetHashCode( c ) );
}
