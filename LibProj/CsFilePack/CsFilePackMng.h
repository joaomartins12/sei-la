
#pragma once

class CsFilePackMng
{
public:
	CsFilePackMng();
	~CsFilePackMng();

	struct sFILE
	{
		sFILE():s_pFilePack(NULL),s_pFileHash(NULL)
		{};

		CsFilePack*		s_pFilePack;
		CsFileHash*		s_pFileHash;
	};

public:
	bool				ReadFilePack( std::vector<std::string> const& vecReadFiles );
	void				Delete();
	void				DeleteVPFile();
	void				Init();

	bool				LoadFilePack( LPCSTR szPackName, bool bWrite = false );
	bool				NewFilePack( LPCSTR szPackName );

	// 폰트 및 PathEngine에서 사용하고 있음.//////////////////////////////////
	size_t				Allocate_GetFileData( int nFilePackIndex, char** ppData, size_t nHashCode );		// 메모리 할당하고 데이터 입력 까지
	size_t				Allocate_GetFileData( int nFilePackIndex, char** ppData, char* szPath );		// 메모리 할당하고 데이터 입력 까지
	//////////////////////////////////////////////////////////////////////////

	// 패치할 파일을 읽어 들인다.
	size_t				LoadPackingFile_Files( LPCSTR pPath );
	size_t				LoadPatchFile_Pack( LPCSTR pPath, int nPatchNum );
	size_t				LoadPatchFile_PackList( LPCSTR pPath );

	void				DeletePackFile( LPCSTR szPackName );
	DWORD				CheckNewFileCount_Pack();
	DWORD				CheckNewFileCount_File();

	LIST_FILEINFO const& GetFileInfo() const;
	MAP_FILEINFO const&	GetPatchListInfo() const;

	bool				DoPatchPacking_Pack();	

	bool				DoUnPack();

	bool				SetAllocCount_File();
	bool				SetAllocCount_Pack();
	bool				SetAllocCount(int nCount);

	bool				SaveHashFile();

	bool				CheckPackFile( LPCSTR szPackName );

	sFILE*				GetFilePack( int nIndex );
	bool				IsFilePack( int nIndex );

	bool				AddFile( LPCSTR cDataPath, LPCSTR cFilePath );
	bool				EditFile( LPCSTR cDataPath, LPCSTR cFilePath );
	bool				EditFile( CsFilePack* pPatch_Pack, CsFileHash::sINFO* pPatch_HashInfo );

	int					GetFilePackIndex( sFILE* pFilePack );

	bool				IsExistHashCode( int nFilePackIndex, LPCSTR szPath );
	bool				IsExistHashCode( int nFilePackIndex, size_t nHashCode );

	CsFileHash::sINFO*	GetHashData( int nFilePackIndex, char* szPath );
	CsFileHash::sINFO*	GetHashData( int nFilePackIndex, size_t nHashCode );

	char*				GetFileData( int nFilePackIndex, char** ppData, UINT64 nOffset, size_t nSize );
	int					GetFileHandle( int nFilePackIndex, char* szPath );


	bool				SeekLock( int nFilePackIndex );
	bool				SeekUnlock( int nFilePackIndex );

private:
	CsFilePack::sCHUNK*	GetChunk( CsFilePack::sCHUNK* pChunk, int nFilePackIndex, char* szPath );
	CsFilePack::sCHUNK*	GetChunk( CsFilePack::sCHUNK* pChunk, int nFilePackIndex, UINT64 nOffset );
	CsFilePack::sCHUNK* GetChunk( CsFilePack* pPatchPack, CsFilePack::sCHUNK* pChunk, UINT64 nOffset );

private:
	CsVectorPB< sFILE* >	m_vpFile;
	char*					m_pBuffer;

	SPatchPackInfo			m_LoadPackInfo;
	SPatchFileInfo			m_LoadFileInfo;

	sFILE*					m_pOpendFile;// 로드한 파일 정보
};