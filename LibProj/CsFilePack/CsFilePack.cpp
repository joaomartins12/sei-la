#include "stdafx.h"
#include "CsFilePack.h"

namespace CsFPS
{
	CsFilePack::CsFilePack()
	{
		m_nHandle = FP_INVALIDE_HANDLE;
		m_pBuddyHash = NULL;
		ZeroMemory(&m_szFilePath, MAX_PATH);
		m_CS = new CSSpinLock();
	}

	CsFilePack::~CsFilePack()
	{
		Delete();
	}

	void CsFilePack::Delete()
	{
		if (m_nHandle != FP_INVALIDE_HANDLE)
		{
			_close(m_nHandle);
			m_nHandle = FP_INVALIDE_HANDLE;
		}

		m_pBuddyHash = NULL;
		SAFE_DELETE(m_CS);
	}

	void CsFilePack::New(CsFileHash* pBuddyHash, char const* szPath)
	{
		SeekLock();

		m_pBuddyHash = pBuddyHash;
		strcpy_s(m_szFilePath, MAX_PATH, szPath);

		m_Header.s_nVersion = FILE_PACK_VERSION;
		m_Header.s_Plag = 0;

		if (_access_s(m_szFilePath, 0) == 0)
		{
			DWORD mode = GetFileAttributesA(m_szFilePath);
			mode &= ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY);
			SetFileAttributesA(m_szFilePath, mode);
		}

		DWORD op = O_CREAT | O_BINARY | O_TRUNC | O_RDWR;
		_sopen_s(&m_nHandle, m_szFilePath, op, _SH_DENYNO, _S_IREAD | _S_IWRITE);
		if (m_nHandle == FP_INVALIDE_HANDLE)
		{
			SeekUnlock();
			return;
		}

		_write(m_nHandle, &m_Header, (UINT)sizeof(FPHeader));

		SeekUnlock();
	}

	void CsFilePack::SeekLock()
	{
		if (m_CS)
			m_CS->Enter();
	}

	void CsFilePack::SeekUnlock()
	{
		if (m_CS)
			m_CS->Leave();
	}

	DWORD CsFilePack::Load(CsFileHash* pBuddyHash, char const* szPath, bool bWrite)
	{
		if (m_nHandle != FP_INVALIDE_HANDLE)
			return ERROR_PACK_FILE_OPENED;

		SeekLock();

		m_pBuddyHash = pBuddyHash;
		strcpy_s(m_szFilePath, MAX_PATH, szPath);

		DWORD mode = GetFileAttributesA(m_szFilePath);
		if (mode != INVALID_FILE_ATTRIBUTES)
		{
			mode &= ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY);
			SetFileAttributesA(m_szFilePath, mode);
		}

		DWORD op = O_BINARY;
		if (bWrite == true)
		{
			_chmod(m_szFilePath, S_IWRITE | S_IREAD);
			op |= O_RDWR;
		}
		else
		{
			op |= O_RDONLY;
		}

		_sopen_s(&m_nHandle, m_szFilePath, op, _SH_DENYRW, _S_IREAD | _S_IWRITE);
		if (m_nHandle == FP_INVALIDE_HANDLE)
		{
			SeekUnlock();
			return ERROR_PACK_FILE_OPEN;
		}

		int nRead = _read(m_nHandle, &m_Header, (UINT)sizeof(FPHeader));
		if (nRead != sizeof(FPHeader))
		{
			_close(m_nHandle);
			m_nHandle = FP_INVALIDE_HANDLE;
			SeekUnlock();
			return ERROR_PACK_FILE_OPEN;
		}

		SeekUnlock();

		return ERROR_OK;
	}

	bool CsFilePack::_AddFile(char const* cDataPath, char const* cFilePath, char* pBuffer)
	{
		assert(m_nHandle != FP_INVALIDE_HANDLE);

		HANDLE tempHandle = CreateFileA(cDataPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (tempHandle == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		size_t tSize = GetFileSize(tempHandle, NULL);

		UINT64 nInsertOffset = _lseeki64(m_nHandle, 0, SEEK_END);

		sCHUNK chunk;
		strcpy_s(chunk.s_szPath, MAX_PATH, cFilePath);
		chunk.s_nNameLen = (int)strlen(cFilePath);
		chunk.s_Plag = 0;
		for (int i = 0; i < chunk.s_nNameLen; ++i)
		{
			*(chunk.s_szPath + i) ^= FP_XOR_VALUE;
		}

		for (int i = chunk.s_nNameLen; i < MAX_PATH; ++i)
		{
			*(chunk.s_szPath + i) = rand() % 256;
		}
		_write(m_nHandle, &chunk, (UINT)sizeof(sCHUNK));

		DWORD nReadByte;
		while (1)
		{
			ReadFile(tempHandle, pBuffer, FP_BLOCK_SIZE, &nReadByte, NULL);
			if (nReadByte == 0)
			{
				break;
			}

			_write(m_nHandle, pBuffer, nReadByte);

			if (nReadByte < FP_BLOCK_SIZE)
				break;
		}
		CloseHandle(tempHandle);

		m_pBuddyHash->_AddFile(cFilePath, nInsertOffset + sizeof(sCHUNK), tSize);

		return true;
	}

	bool CsFilePack::_EditFile(char const* cDataPath, char const* cFilePath, char* pBuffer)
	{
		assert(m_nHandle != FP_INVALIDE_HANDLE);

		HANDLE tempHandle = CreateFileA(cDataPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (tempHandle == INVALID_HANDLE_VALUE)
		{
			return false;
		}
		size_t tSize = GetFileSize(tempHandle, NULL);

		bool bNew = false;
		size_t nDeltaOffset = 0;
		UINT64 nInsertOffset = m_pBuddyHash->CalOffset(cFilePath, tSize);
		switch (nInsertOffset)
		{
		case FP_INVALIDE_SIZE:
		{
			nInsertOffset = _lseeki64(m_nHandle, 0, SEEK_END);
			sCHUNK chunk;
			strcpy_s(chunk.s_szPath, MAX_PATH, cFilePath);
			chunk.s_nNameLen = (int)strlen(cFilePath);
			chunk.s_Plag = 0;
			for (int i = 0; i < chunk.s_nNameLen; ++i)
			{
				*(chunk.s_szPath + i) ^= FP_XOR_VALUE;
			}

			for (int i = chunk.s_nNameLen; i < MAX_PATH; ++i)
			{
				*(chunk.s_szPath + i) = rand() % 256;
			}
			_write(m_nHandle, &chunk, (UINT)sizeof(sCHUNK));

			bNew = true;
			nDeltaOffset = sizeof(sCHUNK);
		}
		break;
		case FP_CHANGE_SIZE:
		{
			nInsertOffset = _lseeki64(m_nHandle, 0, SEEK_END);
			sCHUNK chunk;
			strcpy_s(chunk.s_szPath, MAX_PATH, cFilePath);
			chunk.s_nNameLen = (int)strlen(cFilePath);
			chunk.s_Plag = 0;
			for (int i = 0; i < chunk.s_nNameLen; ++i)
			{
				*(chunk.s_szPath + i) ^= FP_XOR_VALUE;
			}

			for (int i = chunk.s_nNameLen; i < MAX_PATH; ++i)
			{
				*(chunk.s_szPath + i) = rand() % 256;
			}
			_write(m_nHandle, &chunk, (UINT)sizeof(sCHUNK));

			nDeltaOffset = sizeof(sCHUNK);

		}
		break;
		default:
			nDeltaOffset = 0;
			nInsertOffset = _lseeki64(m_nHandle, nInsertOffset, SEEK_SET);
			break;
		}

		DWORD nReadByte;
		while (1)
		{
			ReadFile(tempHandle, pBuffer, FP_BLOCK_SIZE, &nReadByte, NULL);
			if (nReadByte == 0)
			{
				break;
			}

			_write(m_nHandle, pBuffer, nReadByte);

			if (nReadByte < FP_BLOCK_SIZE)
				break;
		}
		CloseHandle(tempHandle);

		if (bNew == true)
		{
			m_pBuddyHash->_AddFile(cFilePath, nInsertOffset + nDeltaOffset, tSize);
		}
		else
		{
			m_pBuddyHash->_EditFile(cFilePath, nInsertOffset + nDeltaOffset, tSize);
		}

		return true;
	}

	bool CsFilePack::_EditFile(CsFilePack* pPatch_Pack, CsFileHash* pPatch_Hash)
	{
		std::map< size_t, CsFileHash::sINFO* >* pHashInfo = pPatch_Hash->GetHashMap();
		std::map< size_t, CsFileHash::sINFO* >::iterator it = pHashInfo->begin();
		for (; it != pHashInfo->end(); ++it)
		{
			if (!_EditFile(pPatch_Pack, it->second))
				return false;
		}

		return true;
	}

	bool CsFilePack::_EditFile(CsFilePack* pPatch_Pack, CsFileHash::sINFO* pPatch_HashInfo)
	{
		assert(m_nHandle != FP_INVALIDE_HANDLE);

		CsFPS::sCHUNK Chunk;

		if (pPatch_Pack->_GetChunk(&Chunk, pPatch_HashInfo->s_nOffset) == NULL || Chunk.s_nNameLen <= 0)
			return false;

		char cFilePath[MAX_PATH];
		strcpy_s(cFilePath, MAX_PATH, Chunk.s_szPath);

		bool bNew = false;
		size_t nDeltaOffset = 0;
		UINT64 nInsertOffset = m_pBuddyHash->CalOffset(Chunk.s_szPath, pPatch_HashInfo->s_nAllocSize);

		switch (nInsertOffset)
		{
		case FP_INVALIDE_SIZE:
		{
			nInsertOffset = _lseeki64(m_nHandle, 0, SEEK_END);

			for (int i = 0; i < Chunk.s_nNameLen; ++i)
			{
				*(Chunk.s_szPath + i) ^= FP_XOR_VALUE;
			}

			for (int i = Chunk.s_nNameLen; i < MAX_PATH; ++i)
			{
				*(Chunk.s_szPath + i) = rand() % 256;
			}
			_write(m_nHandle, &Chunk, (UINT)sizeof(sCHUNK));

			bNew = true;
			nDeltaOffset = sizeof(sCHUNK);
		}
		break;
		case FP_CHANGE_SIZE:
		{
			nInsertOffset = _lseeki64(m_nHandle, 0, SEEK_END);

			for (int i = 0; i < Chunk.s_nNameLen; ++i)
			{
				*(Chunk.s_szPath + i) ^= FP_XOR_VALUE;
			}

			for (int i = Chunk.s_nNameLen; i < MAX_PATH; ++i)
			{
				*(Chunk.s_szPath + i) = rand() % 256;
			}
			_write(m_nHandle, &Chunk, (UINT)sizeof(sCHUNK));

			nDeltaOffset = sizeof(sCHUNK);

		}
		break;
		default:
		{
			nDeltaOffset = 0;
			nInsertOffset = _lseeki64(m_nHandle, nInsertOffset, SEEK_SET);
		}
		break;
		}


		char* pAlloc = NULL;
		size_t nAllocSize = 0;

		pAlloc = (char*)malloc(pPatch_HashInfo->s_nAllocSize);
		nAllocSize = pPatch_HashInfo->s_nAllocSize;

		if (pAlloc == NULL)
			return false;

		if (pPatch_Pack->_GetData(&pAlloc, pPatch_HashInfo->s_nOffset, nAllocSize) == NULL)
		{
			free(pAlloc);
			return false;
		}

		_write(m_nHandle, pAlloc, (UINT)nAllocSize);

		if (bNew == true)
		{
			m_pBuddyHash->_AddFile(cFilePath, nInsertOffset + nDeltaOffset, nAllocSize);
		}
		else
		{
			m_pBuddyHash->_EditFile(cFilePath, nInsertOffset + nDeltaOffset, nAllocSize);
		}

		if (pAlloc != NULL)
		{
			free(pAlloc);
			pAlloc = NULL;
		}
		return true;
	}

	CsFPS::sCHUNK* CsFilePack::_GetChunk(sCHUNK* pChunk, UINT64 nOffset)
	{
		if (pChunk == NULL)
			return NULL;

		ZeroMemory(pChunk, sizeof(sCHUNK));

		SeekLock();

		if (m_nHandle == FP_INVALIDE_HANDLE)
		{
			SeekUnlock();
			return NULL;
		}

		if (nOffset < sizeof(sCHUNK))
		{
			OutputDebugStringA("[CsFilePack] _GetChunk invalid offset: smaller than chunk size\n");
			SeekUnlock();
			return NULL;
		}

		__int64 nFileSize = _filelengthi64(m_nHandle);
		__int64 nChunkOffset = (__int64)(nOffset - sizeof(sCHUNK));

		if (nFileSize <= 0 || nChunkOffset < 0 || nChunkOffset + (__int64)sizeof(sCHUNK) > nFileSize)
		{
			OutputDebugStringA("[CsFilePack] _GetChunk invalid offset/file size\n");
			SeekUnlock();
			return NULL;
		}

		if (_lseeki64(m_nHandle, nChunkOffset, SEEK_SET) != nChunkOffset)
		{
			OutputDebugStringA("[CsFilePack] _GetChunk seek failed\n");
			SeekUnlock();
			return NULL;
		}

		int nRead = _read(m_nHandle, pChunk, sizeof(sCHUNK));

		SeekUnlock();

		if (nRead != sizeof(sCHUNK))
		{
			OutputDebugStringA("[CsFilePack] _GetChunk read failed\n");
			ZeroMemory(pChunk, sizeof(sCHUNK));
			return NULL;
		}

		if (pChunk->s_nNameLen <= 0 || pChunk->s_nNameLen >= MAX_PATH)
		{
			OutputDebugStringA("[CsFilePack] _GetChunk invalid name length\n");
			ZeroMemory(pChunk, sizeof(sCHUNK));
			return NULL;
		}

		for (int i = 0; i < pChunk->s_nNameLen; ++i)
		{
			pChunk->s_szPath[i] ^= FP_XOR_VALUE;
		}

		pChunk->s_szPath[pChunk->s_nNameLen] = '\0';
		return pChunk;
	}

	char* CsFilePack::_GetData(char** ppData, UINT64 nOffset, size_t nSize)
	{
		if (ppData == NULL || *ppData == NULL || nSize == 0)
			return NULL;

		SeekLock();

		if (m_nHandle == FP_INVALIDE_HANDLE)
		{
			SeekUnlock();
			return NULL;
		}

		__int64 nFileSize = _filelengthi64(m_nHandle);

		if (nFileSize <= 0 || (__int64)nOffset < 0 || (__int64)(nOffset + nSize) > nFileSize)
		{
			OutputDebugStringA("[CsFilePack] _GetData invalid offset/size\n");
			SeekUnlock();
			return NULL;
		}

		if (_lseeki64(m_nHandle, nOffset, SEEK_SET) != (__int64)nOffset)
		{
			OutputDebugStringA("[CsFilePack] _GetData seek failed\n");
			SeekUnlock();
			return NULL;
		}

		int nRead = _read(m_nHandle, *ppData, (UINT)nSize);

		SeekUnlock();

		if (nRead != (int)nSize)
		{
			OutputDebugStringA("[CsFilePack] _GetData read failed\n");
			return NULL;
		}

		return *ppData;
	}

	void CsFilePack::_GetData(std::vector<unsigned char>& ppData, UINT64 nOffset, size_t nSize)
	{
		if (ppData.empty() || nSize == 0)
			return;

		SeekLock();

		if (m_nHandle == FP_INVALIDE_HANDLE)
		{
			SeekUnlock();
			return;
		}

		__int64 nFileSize = _filelengthi64(m_nHandle);

		if (nFileSize <= 0 || (__int64)nOffset < 0 || (__int64)(nOffset + nSize) > nFileSize)
		{
			OutputDebugStringA("[CsFilePack] _GetData(vector) invalid offset/size\n");
			SeekUnlock();
			return;
		}

		if (_lseeki64(m_nHandle, nOffset, SEEK_SET) != (__int64)nOffset)
		{
			OutputDebugStringA("[CsFilePack] _GetData(vector) seek failed\n");
			SeekUnlock();
			return;
		}

		_read(m_nHandle, &ppData.at(0), (UINT)nSize);

		SeekUnlock();
	}

	int CsFilePack::_GetFileHandle(UINT64 nOffset)
	{
		if (m_nHandle == FP_INVALIDE_HANDLE)
			return FP_INVALIDE_HANDLE;

		__int64 nFileSize = _filelengthi64(m_nHandle);

		if (nFileSize <= 0 || (__int64)nOffset < 0 || (__int64)nOffset >= nFileSize)
		{
			OutputDebugStringA("[CsFilePack] _GetFileHandle invalid offset\n");
			return FP_INVALIDE_HANDLE;
		}

		if (_lseeki64(m_nHandle, nOffset, SEEK_SET) != (__int64)nOffset)
		{
			OutputDebugStringA("[CsFilePack] _GetFileHandle seek failed\n");
			return FP_INVALIDE_HANDLE;
		}

		return m_nHandle;
	}
}
