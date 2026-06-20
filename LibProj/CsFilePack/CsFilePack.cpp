
#include "stdafx.h"
#include "CsFilePack.h"

namespace CsFPS
{
	CsFilePack::CsFilePack()
	{
		m_nHandle = FP_INVALIDE_HANDLE;
		m_pBuddyHash = NULL;
		ZeroMemory(&m_szFilePath, MAX_PATH);
		//InitializeCriticalSection( &m_CS );
		m_CS = new CSSpinLock();
	}

	CsFilePack::~CsFilePack()
	{
		Delete();
	}

	void CsFilePack::Delete()
	{
		if( m_nHandle != FP_INVALIDE_HANDLE )
		{
			_close( m_nHandle );
			m_nHandle = FP_INVALIDE_HANDLE;
		}

		m_pBuddyHash = NULL;
		SAFE_DELETE( m_CS );
		//DeleteCriticalSection( &m_CS );
	}	

	void CsFilePack::New( CsFileHash* pBuddyHash, char const* szPath )
	{
		SeekLock();

		m_pBuddyHash = pBuddyHash;
		strcpy_s( m_szFilePath, MAX_PATH, szPath );

		m_Header.s_nVersion = FILE_PACK_VERSION;
		m_Header.s_Plag = 0;

		//assert( m_nHandle == FP_INVALIDE_HANDLE );

		if( _access_s( m_szFilePath, 0 ) == 0 )
		{
			DWORD mode = GetFileAttributesA( m_szFilePath );
			mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY );
			SetFileAttributesA( m_szFilePath, mode );
		}	
		
		DWORD op = O_CREAT | O_BINARY | O_TRUNC | O_RDWR;
		_sopen_s( &m_nHandle, m_szFilePath, op, _SH_DENYNO, _S_IREAD | _S_IWRITE ); //저장할 소스파일 열기.
		if( m_nHandle == FP_INVALIDE_HANDLE )
		{
			//CsMessageBox( MB_OK, _T( "PackFile Error" ) );
			return;
		}

		// 파일헤더 기록
		_write( m_nHandle, &m_Header, (UINT)sizeof( FPHeader ) );

		SeekUnlock();
	}

	void CsFilePack::SeekLock()
	{ 
		//EnterCriticalSection( &m_CS );
		if( m_CS )
			m_CS->Enter();
	}

	void CsFilePack::SeekUnlock()
	{ 
		//LeaveCriticalSection( &m_CS );
		if( m_CS ) 
			m_CS->Leave();
	}

	DWORD CsFilePack::Load( CsFileHash* pBuddyHash, char const* szPath, bool bWrite )
	{
		if( m_nHandle != FP_INVALIDE_HANDLE )
			return ERROR_PACK_FILE_OPENED;

		SeekLock();

		m_pBuddyHash = pBuddyHash;
		strcpy_s( m_szFilePath, MAX_PATH, szPath );

		DWORD mode = GetFileAttributesA( m_szFilePath );
		mode &= ~( FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY );
		SetFileAttributesA( m_szFilePath, mode );

		DWORD op = O_BINARY;
		if( bWrite == true )
		{
			_chmod( m_szFilePath, S_IWRITE | S_IREAD );
			op |= O_RDWR;
		}
		else
		{
			op |= O_RDONLY;
		}

		_sopen_s( &m_nHandle, m_szFilePath, op, _SH_DENYRW, _S_IREAD | _S_IWRITE ); //저장할 소스파일 열기.
		if( m_nHandle == FP_INVALIDE_HANDLE )
		{
			//CsMessageBox( MB_OK, _T( "PackFile Error" ) );
			SeekUnlock();
			return ERROR_PACK_FILE_OPEN;
		}

		// 파일헤더 리딩
		_read( m_nHandle, &m_Header, (UINT)sizeof( FPHeader ) );	

		SeekUnlock();

		return ERROR_OK;
	}

	bool CsFilePack::_AddFile( char const* cDataPath, char const* cFilePath, char* pBuffer )
	{
		assert( m_nHandle != FP_INVALIDE_HANDLE );

		HANDLE tempHandle = CreateFileA( cDataPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
		if( tempHandle == INVALID_HANDLE_VALUE )
		{
			return false;
		}
		size_t tSize = GetFileSize( tempHandle, NULL );

		// 신규 데이터는 무조건 끝에 삽입
		UINT64 nInsertOffset = _lseeki64( m_nHandle, 0, SEEK_END );

		// 청크 기록
		sCHUNK chunk;
		strcpy_s( chunk.s_szPath, MAX_PATH, cFilePath );
		chunk.s_nNameLen = (int)strlen( cFilePath );	
		chunk.s_Plag = 0;
		for( int i=0; i<chunk.s_nNameLen; ++i )
		{
			*(chunk.s_szPath+i) ^= FP_XOR_VALUE; 
		}

		for( int i=chunk.s_nNameLen; i<MAX_PATH; ++i )
		{
			*(chunk.s_szPath+i) = rand()%256;
		}
		_write( m_nHandle, &chunk, (UINT)sizeof( sCHUNK ) );	

		// Copy the data in the file ( 열은 파일을 파일팩 파일에 복사 )
		//char* pBuffer =FPMNG_PTR->__Buffer();	
		DWORD nReadByte;
		while(1)
		{
			ReadFile( tempHandle, pBuffer, FP_BLOCK_SIZE, &nReadByte, NULL );
			if( nReadByte == 0 )
			{
				break;  //파일을 다 읽으면 빠져나옴.
			}

			_write( m_nHandle, pBuffer, nReadByte );

			//읽은 파일이 파일끝이면 자동 루프 탈출.
			if( nReadByte < FP_BLOCK_SIZE )
				break;
		}
		CloseHandle( tempHandle );

		m_pBuddyHash->_AddFile( cFilePath, nInsertOffset + sizeof( sCHUNK ), tSize );

		return true;
	}

	bool CsFilePack::_EditFile( char const* cDataPath, char const* cFilePath, char* pBuffer )
	{
		assert( m_nHandle != FP_INVALIDE_HANDLE );

		HANDLE tempHandle = CreateFileA( cDataPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
		if( tempHandle == INVALID_HANDLE_VALUE )
		{
			return false;
		}
		size_t tSize = GetFileSize( tempHandle, NULL );

		// 사이에 우겨넣을 곳이 있는지 체크
		bool bNew = false;
		size_t nDeltaOffset = 0;
		UINT64 nInsertOffset = m_pBuddyHash->CalOffset( cFilePath, tSize );
		switch( nInsertOffset )
		{
		case FP_INVALIDE_SIZE:
			{
				nInsertOffset = _lseeki64( m_nHandle, 0, SEEK_END );
				// 청크 기록
				sCHUNK chunk;
				strcpy_s( chunk.s_szPath, MAX_PATH, cFilePath );
				chunk.s_nNameLen = (int)strlen( cFilePath );	
				chunk.s_Plag = 0;
				for( int i=0; i<chunk.s_nNameLen; ++i )
				{
					*(chunk.s_szPath+i) ^= FP_XOR_VALUE;
				}

				for( int i=chunk.s_nNameLen; i<MAX_PATH; ++i )
				{
					*(chunk.s_szPath+i) = rand()%256;
				}
				_write( m_nHandle, &chunk, (UINT)sizeof( sCHUNK ) );

				bNew = true;
				nDeltaOffset = sizeof( sCHUNK );
			}
			break;
		case FP_CHANGE_SIZE:
			{
				nInsertOffset = _lseeki64( m_nHandle, 0, SEEK_END );
				// 청크 기록
				sCHUNK chunk;
				strcpy_s( chunk.s_szPath, MAX_PATH, cFilePath );
				chunk.s_nNameLen = (int)strlen( cFilePath );	
				chunk.s_Plag = 0;
				for( int i=0; i<chunk.s_nNameLen; ++i )
				{
					*(chunk.s_szPath+i) ^= FP_XOR_VALUE;
				}

				for( int i=chunk.s_nNameLen; i<MAX_PATH; ++i )
				{
					*(chunk.s_szPath+i) = rand()%256;
				}
				_write( m_nHandle, &chunk, (UINT)sizeof( sCHUNK ) );

				nDeltaOffset = sizeof( sCHUNK );

			}
			break;
		default:
			nDeltaOffset = 0;
			nInsertOffset = _lseeki64( m_nHandle, nInsertOffset, SEEK_SET );
			break;
		}
		
		// Copy the data in the file ( 열은 파일을 파일팩 파일에 복사 )
		//char* pBuffer =FPMNG_PTR->__Buffer();	
		DWORD nReadByte;
		while(1)
		{
			ReadFile( tempHandle, pBuffer, FP_BLOCK_SIZE, &nReadByte, NULL );
			if( nReadByte == 0 )
			{
				break;  //파일을 다 읽으면 빠져나옴.
			}

			_write( m_nHandle, pBuffer, nReadByte );

			//읽은 파일이 파일끝이면 자동 루프 탈출.
			if( nReadByte < FP_BLOCK_SIZE )
				break;
		}
		CloseHandle( tempHandle );

		if( bNew == true )
		{
			m_pBuddyHash->_AddFile( cFilePath, nInsertOffset + nDeltaOffset, tSize );
		}
		else
		{
			m_pBuddyHash->_EditFile( cFilePath, nInsertOffset + nDeltaOffset, tSize );
		}

		return true;
	}

	// ida: _EditFile_2
	bool CsFilePack::_EditFile( CsFilePack* pPatch_Pack, CsFileHash* pPatch_Hash )
	{
		std::map< size_t, CsFileHash::sINFO* >* pHashInfo = pPatch_Hash->GetHashMap();
		std::map< size_t, CsFileHash::sINFO* >::iterator it = pHashInfo->begin();
		for( ; it != pHashInfo->end(); ++it )
		{
			if( !_EditFile( pPatch_Pack, it->second ) )
				return false;
		}

		return true;
	}

	bool CsFilePack::_EditFile( CsFilePack* pPatch_Pack, CsFileHash::sINFO* pPatch_HashInfo )
	{
		assert( m_nHandle != FP_INVALIDE_HANDLE );

		CsFPS::sCHUNK Chunk;

		pPatch_Pack->_GetChunk( &Chunk, pPatch_HashInfo->s_nOffset );
		char cFilePath[ MAX_PATH ];
		strcpy_s( cFilePath, MAX_PATH, Chunk.s_szPath );

		// 사이에 우겨넣을 곳이 있는지 체크
		bool bNew = false;
		size_t nDeltaOffset = 0;
		UINT64 nInsertOffset = m_pBuddyHash->CalOffset( Chunk.s_szPath, pPatch_HashInfo->s_nAllocSize );

		switch( nInsertOffset )
		{
		case FP_INVALIDE_SIZE:
			{
				nInsertOffset = _lseeki64( m_nHandle, 0, SEEK_END );

				for( int i=0; i<Chunk.s_nNameLen; ++i )
				{
					*(Chunk.s_szPath+i) ^= FP_XOR_VALUE;
				}

				for( int i=Chunk.s_nNameLen; i<MAX_PATH; ++i )
				{
					*(Chunk.s_szPath+i) = rand()%256;
				}
				_write( m_nHandle, &Chunk, (UINT)sizeof( sCHUNK ) );

				bNew = true;
				nDeltaOffset = sizeof( sCHUNK );
			}
			break;
		case FP_CHANGE_SIZE:
			{
				nInsertOffset = _lseeki64( m_nHandle, 0, SEEK_END );

				for( int i=0; i<Chunk.s_nNameLen; ++i )
				{
					*(Chunk.s_szPath+i) ^= FP_XOR_VALUE;
				}

				for( int i=Chunk.s_nNameLen; i<MAX_PATH; ++i )
				{
					*(Chunk.s_szPath+i) = rand()%256;
				}
				_write( m_nHandle, &Chunk, (UINT)sizeof( sCHUNK ) );

				nDeltaOffset = sizeof( sCHUNK );

			}
			break;
		default:
			{
				nDeltaOffset = 0;
				nInsertOffset = _lseeki64( m_nHandle, nInsertOffset, SEEK_SET );
			}
			break;
		}


		char* pAlloc = NULL;
		size_t nAllocSize = 0;

		free( pAlloc );
		pAlloc = (char*)malloc( pPatch_HashInfo->s_nAllocSize );
		nAllocSize = pPatch_HashInfo->s_nAllocSize;

		pPatch_Pack->_GetData( &pAlloc, pPatch_HashInfo->s_nOffset, nAllocSize );

		_write( m_nHandle, pAlloc, (UINT)nAllocSize );

		if( bNew == true )
		{
			m_pBuddyHash->_AddFile( cFilePath, nInsertOffset + nDeltaOffset, nAllocSize );
		}
		else
		{
			m_pBuddyHash->_EditFile( cFilePath, nInsertOffset + nDeltaOffset, nAllocSize );
		}

		if( pAlloc != NULL )
		{
			free( pAlloc );
			pAlloc = NULL;
		}
		return true;
	}

	CsFPS::sCHUNK* CsFilePack::_GetChunk( sCHUNK* pChunk, UINT64 nOffset )
	{
		SeekLock();

		assert( pChunk != NULL );
		assert( m_nHandle != FP_INVALIDE_HANDLE );
		_lseeki64( m_nHandle, nOffset - sizeof( sCHUNK ), SEEK_SET );
		_read( m_nHandle, pChunk, sizeof( sCHUNK ) );

		SeekUnlock();

		assert( pChunk->s_nNameLen > 0 );

		for( int i=0; i<pChunk->s_nNameLen; ++i )
		{
			*(pChunk->s_szPath+i) ^= FP_XOR_VALUE;
		}
		pChunk->s_szPath[ pChunk->s_nNameLen ] = NULL;
		return pChunk;
	}

	char* CsFilePack::_GetData( char** ppData, UINT64 nOffset, size_t nSize )
	{
		SeekLock();

		assert( m_nHandle != FP_INVALIDE_HANDLE );	
		_lseeki64( m_nHandle, nOffset, SEEK_SET );
		_read( m_nHandle, *ppData, (UINT)nSize );

		SeekUnlock();
		
		return *ppData;
	}

	void CsFilePack::_GetData( std::vector<unsigned char>& ppData, UINT64 nOffset, size_t nSize )
	{
		SeekLock();

		assert( m_nHandle != FP_INVALIDE_HANDLE );	
		_lseeki64( m_nHandle, nOffset, SEEK_SET );
		_read( m_nHandle, &ppData.at(0), (UINT)nSize );

		SeekUnlock();
	}

	int CsFilePack::_GetFileHandle( UINT64 nOffset )
	{
		assert( m_nHandle != FP_INVALIDE_HANDLE );	
		_lseeki64( m_nHandle, nOffset, SEEK_SET );
		return m_nHandle;
	}
}