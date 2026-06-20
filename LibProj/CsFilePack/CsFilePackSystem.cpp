#include "stdafx.h"
#include "CsFilePackSystem.h"
#include "FPHeader.h"
#include "CsFileHash.h"
#include "CsFilePack.h"
#include <CsFunc/CsMessageBox.h>
#include <tchar.h>
#include <iostream>
#include <sstream>

namespace
{
	void PackLogA(const char* msg)
	{
		if (!msg)
			return;

		OutputDebugStringA("[CsFilePackSystem] ");
		OutputDebugStringA(msg);
		OutputDebugStringA("\n");
	}

	void PackLogFileA(const char* prefix, const std::string& file)
	{
		std::string msg = prefix;
		msg += file;
		PackLogA(msg.c_str());
	}

	void PackLogPackageA(const char* prefix, const std::string& packName)
	{
		std::string msg = prefix;
		msg += packName;
		PackLogA(msg.c_str());
	}
}

namespace CsFPS
{
	class ResSupplierImpl
	{
	private:
		struct sFILE
		{
			sFILE()
				: s_pFilePack(NULL)
				, s_pFileHash(NULL)
			{
			}

			~sFILE()
			{
				// Não chamar Destroy() aqui.
				// Este struct é copiado pelo std::vector.
				// A libertação real é feita manualmente em ReleasePackage().
			}

			CsFilePack* s_pFilePack;
			CsFileHash* s_pFileHash;

			bool NewPackage(LPCSTR szPackName)
			{
				Destroy();

				if (NULL == szPackName || 0 == lstrlenA(szPackName))
					return false;

				std::string sPackFileName = szPackName;
				sPackFileName += PACK_EX_NAME;

				std::string sPackHashName = szPackName;
				sPackHashName += HASH_EX_NAME;

				PackLogPackageA("NewPackage start: ", szPackName);

				s_pFilePack = new CsFilePack;
				s_pFileHash = new CsFileHash;

				if (NULL == s_pFilePack || NULL == s_pFileHash)
				{
					PackLogPackageA("NewPackage failed: allocation failed: ", szPackName);
					Destroy();
					return false;
				}

				s_pFilePack->New(s_pFileHash, sPackFileName.c_str());
				s_pFileHash->New(sPackHashName.c_str());
				s_pFileHash->SaveFile();

				PackLogPackageA("NewPackage ok: ", szPackName);

				return true;
			}

			bool LoadPackage(LPCSTR szPackName, bool bWrite)
			{
				Destroy();

				if (NULL == szPackName || 0 == lstrlenA(szPackName))
				{
					PackLogA("LoadPackage failed: empty pack name");
					return false;
				}

				std::string sPackHashName = szPackName;
				sPackHashName += HASH_EX_NAME;

				std::string sPackFileName = szPackName;
				sPackFileName += PACK_EX_NAME;

				PackLogPackageA("LoadPackage start: ", szPackName);
				PackLogFileA("Hash file: ", sPackHashName);
				PackLogFileA("Pack file: ", sPackFileName);

				bool bHashExists = ExistFile(sPackHashName.c_str());
				bool bPackExists = ExistFile(sPackFileName.c_str());

				if (!bHashExists || !bPackExists)
				{
					if (!bHashExists)
						PackLogFileA("Missing hash file: ", sPackHashName);

					if (!bPackExists)
						PackLogFileA("Missing pack file: ", sPackFileName);

					if (bWrite)
					{
						PackLogPackageA("LoadPackage switching to NewPackage: ", szPackName);
						return NewPackage(szPackName);
					}

					PackLogPackageA("LoadPackage failed: missing .hf/.pf: ", szPackName);
					return false;
				}

				s_pFileHash = new CsFileHash;
				if (NULL == s_pFileHash)
				{
					PackLogPackageA("LoadPackage failed: CsFileHash allocation failed: ", szPackName);
					return false;
				}

				int nHashLoadResult = s_pFileHash->Load(sPackHashName.c_str());
				if (ERROR_OK != nHashLoadResult)
				{
					std::ostringstream oss;
					oss << "LoadPackage failed: hash load failed: " << sPackHashName << " result=" << nHashLoadResult;
					PackLogA(oss.str().c_str());

					SAFE_DELETE(s_pFileHash);
					return false;
				}

				s_pFilePack = new CsFilePack;
				if (NULL == s_pFilePack)
				{
					PackLogPackageA("LoadPackage failed: CsFilePack allocation failed: ", szPackName);
					SAFE_DELETE(s_pFileHash);
					return false;
				}

				int nPackLoadResult = s_pFilePack->Load(s_pFileHash, sPackFileName.c_str(), bWrite);
				if (ERROR_OK != nPackLoadResult)
				{
					std::ostringstream oss;
					oss << "LoadPackage failed: pack load failed: " << sPackFileName << " result=" << nPackLoadResult;
					PackLogA(oss.str().c_str());

					SAFE_DELETE(s_pFilePack);
					SAFE_DELETE(s_pFileHash);
					return false;
				}

				std::ostringstream oss;
				oss << "LoadPackage ok: " << szPackName << " files=" << GetTotalCount();
				PackLogA(oss.str().c_str());

				return true;
			}

			void Destroy(void)
			{
				SAFE_DELETE(s_pFilePack);
				SAFE_DELETE(s_pFileHash);
			}

			bool IsExistFile(LPCSTR szFileName) const
			{
				if (NULL == szFileName || 0 == lstrlenA(szFileName))
					return false;

				size_t hashCode = GetHashCode(szFileName);
				if (0 == hashCode)
					return false;

				if (NULL == s_pFileHash)
					return false;

				return s_pFileHash->IsExist(hashCode);
			}

			bool IsExistFile(size_t HashCode) const
			{
				if (0 == HashCode)
					return false;

				if (NULL == s_pFileHash)
					return false;

				return s_pFileHash->IsExist(HashCode);
			}

			int GetFileHandle(size_t HashCode)
			{
				if (0 == HashCode)
					return 0;

				if (NULL == s_pFileHash || NULL == s_pFilePack)
					return 0;

				CsFileHash::sINFO* pInfo = s_pFileHash->GetHashInfo(HashCode);
				if (NULL == pInfo)
					return 0;

				return s_pFilePack->_GetFileHandle(pInfo->s_nOffset);
			}

			size_t GetData(char** ppData, size_t HashCode)
			{
				if (NULL == ppData)
					return 0;

				*ppData = NULL;

				if (0 == HashCode)
					return 0;

				if (NULL == s_pFilePack || NULL == s_pFileHash)
					return 0;

				CsFileHash::sINFO* pInfo = s_pFileHash->GetHashInfo(HashCode);
				if (NULL == pInfo)
					return 0;

				if (0 == pInfo->s_nDataSize)
					return 0;

				*ppData = new char[pInfo->s_nDataSize];
				if (NULL == *ppData)
					return 0;

				s_pFilePack->_GetData(ppData, pInfo->s_nOffset, pInfo->s_nDataSize);

				return pInfo->s_nDataSize;
			}

			size_t GetData(std::vector<unsigned char>& pData, size_t HashCode)
			{
				pData.clear();

				if (0 == HashCode)
					return 0;

				if (NULL == s_pFilePack || NULL == s_pFileHash)
					return 0;

				CsFileHash::sINFO* pInfo = s_pFileHash->GetHashInfo(HashCode);
				if (NULL == pInfo)
					return 0;

				if (0 == pInfo->s_nDataSize)
					return 0;

				pData.resize(pInfo->s_nDataSize);
				s_pFilePack->_GetData(pData, pInfo->s_nOffset, pInfo->s_nDataSize);

				return pInfo->s_nDataSize;
			}

			bool AddFilePackage(LPCSTR cDataPath, LPCSTR cFilePath, char* pBuffer)
			{
				if (NULL == s_pFilePack)
					return false;

				return s_pFilePack->_AddFile(cDataPath, cFilePath, pBuffer);
			}

			bool EditFilePackage(LPCSTR cDataPath, LPCSTR cFilePath, char* pBuffer)
			{
				if (NULL == s_pFilePack)
					return false;

				return s_pFilePack->_EditFile(cDataPath, cFilePath, pBuffer);
			}

			bool EditFilePackage(sFILE& pAddPackage)
			{
				if (NULL == s_pFilePack)
					return false;

				if (NULL == pAddPackage.s_pFilePack || NULL == pAddPackage.s_pFileHash)
					return false;

				return s_pFilePack->_EditFile(pAddPackage.s_pFilePack, pAddPackage.s_pFileHash);
			}

			size_t GetTotalCount()
			{
				if (NULL == s_pFileHash)
					return 0;

				return s_pFileHash->GetTotalFileCount();
			}
		};

		typedef std::vector<sFILE> CONT_PACKAGE_VEC;

	public:
		ResSupplierImpl(void)
		{
		}

		~ResSupplierImpl(void)
		{
		}

		static bool CreateFolder(LPCSTR szFolder)
		{
			if (!szFolder || !lstrlenA(szFolder))
				return FALSE;

			DWORD dwAttrib = GetFileAttributesA(szFolder);

			if (dwAttrib != 0xffffffff)
				return ((dwAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);

			char* szPath = _strdup(szFolder);
			if (NULL == szPath)
				return FALSE;

			char* p = strrchr(szPath, '\\');

			if (p)
			{
				*p = '\0';

				if (!CreateFolder(szPath))
				{
					free(szPath);
					return FALSE;
				}

				free(szPath);

				if (!::CreateDirectoryA(szFolder, NULL))
					return FALSE;
			}
			else
			{
				free(szPath);
			}

			return TRUE;
		}

		static bool CreateFilePath(LPCSTR szFilePath)
		{
			if (NULL == szFilePath)
				return false;

			char* szPath = _strdup(szFilePath);
			if (NULL == szPath)
				return false;

			char* p = strrchr(szPath, '\\');

			bool bRes = FALSE;

			if (p)
			{
				*p = '\0';
				bRes = CreateFolder(szPath);
			}

			free(szPath);

			return bRes;
		}

		bool LoadPackage(std::string const& sPackageName, bool bWrite)
		{
			if (sPackageName.empty())
				return false;

			sFILE loadPackage;

			if (!loadPackage.LoadPackage(sPackageName.c_str(), bWrite))
				return false;

			m_PackageVec.push_back(loadPackage);

			loadPackage.s_pFilePack = NULL;
			loadPackage.s_pFileHash = NULL;

			return true;
		}

		size_t ReadData(int nIndex, size_t FileHashCode, char** pData)
		{
			if (nIndex < 0)
				return 0;

			if ((int)m_PackageVec.size() <= nIndex)
				return 0;

			return m_PackageVec[nIndex].GetData(pData, FileHashCode);
		}

		size_t ReadData(int nIndex, size_t FileHashCode, std::vector<unsigned char>& pData)
		{
			if (nIndex < 0)
				return 0;

			if ((int)m_PackageVec.size() <= nIndex)
				return 0;

			return m_PackageVec[nIndex].GetData(pData, FileHashCode);
		}

		bool IsExistFile(int nIdx, LPCSTR sFileName) const
		{
			if (nIdx < 0)
				return false;

			if (NULL == sFileName || 0 == lstrlenA(sFileName))
				return false;

			size_t HashCode = GetHashCode(sFileName);

			if (0 == HashCode)
				return false;

			return IsExistFile(nIdx, HashCode);
		}

		bool IsExistFile(int nIdx, size_t HashCode) const
		{
			if (nIdx < 0)
				return false;

			if (0 == HashCode)
				return false;

			if ((int)m_PackageVec.size() <= nIdx)
				return false;

			return m_PackageVec[nIdx].IsExistFile(HashCode);
		}

		bool IsExistFile(LPCSTR sFileName) const
		{
			if (NULL == sFileName || 0 == lstrlenA(sFileName))
				return false;

			size_t HashCode = GetHashCode(sFileName);
			if (0 == HashCode)
				return false;

			return IsExistFile(HashCode);
		}

		bool IsExistFile(size_t HashCode) const
		{
			if (0 == HashCode)
				return false;

			CONT_PACKAGE_VEC::const_iterator it = m_PackageVec.begin();
			for (; it != m_PackageVec.end(); ++it)
			{
				if ((*it).IsExistFile(HashCode))
					return true;
			}

			return false;
		}

		int GetFileHandle(int nIdx, LPCSTR sFileName)
		{
			if (nIdx < 0)
				return 0;

			if (NULL == sFileName || 0 == lstrlenA(sFileName))
				return 0;

			size_t HashCode = GetHashCode(sFileName);
			if (0 == HashCode)
				return 0;

			return GetFileHandle(nIdx, HashCode);
		}

		int GetFileHandle(int nIdx, size_t HashCode)
		{
			if (nIdx < 0)
				return 0;

			if (0 == HashCode)
				return 0;

			if ((int)m_PackageVec.size() <= nIdx)
				return 0;

			return m_PackageVec[nIdx].GetFileHandle(HashCode);
		}

		void ReleasePackage(void)
		{
			CONT_PACKAGE_VEC::iterator it = m_PackageVec.begin();
			for (; it != m_PackageVec.end(); ++it)
				(*it).Destroy();

			m_PackageVec.clear();
		}

		bool AddFile(LPCSTR cDataPath, LPCSTR cFilePath, char* pBuffer)
		{
			if (m_PackageVec.empty())
				return false;

			return m_PackageVec[0].AddFilePackage(cDataPath, cFilePath, pBuffer);
		}

		bool EditFilePack(LPCSTR cDataPath, LPCSTR cFilePath, char* pBuffer)
		{
			if (m_PackageVec.empty())
				return false;

			return m_PackageVec[0].EditFilePackage(cDataPath, cFilePath, pBuffer);
		}

		bool EditFilePack(int nDestIdx, int nSourceIdx)
		{
			if (nDestIdx < 0 || nSourceIdx < 0)
				return false;

			if ((int)m_PackageVec.size() <= nDestIdx)
				return false;

			if ((int)m_PackageVec.size() <= nSourceIdx)
				return false;

			return m_PackageVec[nDestIdx].EditFilePackage(m_PackageVec[nSourceIdx]);
		}

		size_t GetTotalFileCount(int nIdx)
		{
			if (nIdx < 0)
				return 0;

			if ((int)m_PackageVec.size() <= nIdx)
				return 0;

			return m_PackageVec[nIdx].GetTotalCount();
		}

		bool SetAllocCount(size_t nCount)
		{
			if (m_PackageVec.empty())
				return false;

			if (NULL == m_PackageVec[0].s_pFileHash)
				return false;

			m_PackageVec[0].s_pFileHash->SetAllocCount((int)nCount);

			return true;
		}

		bool AddAllocCount(size_t nCount)
		{
			if (m_PackageVec.empty())
				return false;

			if (NULL == m_PackageVec[0].s_pFileHash)
				return false;

			CsFPS::FHHeader* pFileHeader = m_PackageVec[0].s_pFileHash->GetHeader();
			if (NULL == pFileHeader)
				return false;

			m_PackageVec[0].s_pFileHash->SetAllocCount(pFileHeader->s_nFileCount + (int)nCount);

			return true;
		}

		bool SaveHashPack(void)
		{
			if (m_PackageVec.empty())
				return false;

			if (NULL == m_PackageVec[0].s_pFileHash)
				return false;

			m_PackageVec[0].s_pFileHash->SaveFile();

			return true;
		}

		bool SeekLock(int nIdx)
		{
			if (nIdx < 0)
				return false;

			if ((int)m_PackageVec.size() <= nIdx)
				return false;

			if (NULL == m_PackageVec[nIdx].s_pFilePack)
				return false;

			m_PackageVec[nIdx].s_pFilePack->SeekLock();

			return true;
		}

		void SeekUnLock(int nIdx)
		{
			if (nIdx < 0)
				return;

			if ((int)m_PackageVec.size() <= nIdx)
				return;

			if (NULL == m_PackageVec[nIdx].s_pFilePack)
				return;

			m_PackageVec[nIdx].s_pFilePack->SeekUnlock();
		}

		CsFileHash::sINFO* GetHashData(int nIdx, LPCSTR szName)
		{
			if (nIdx < 0)
				return NULL;

			if (NULL == szName || 0 == lstrlenA(szName))
				return NULL;

			if ((int)m_PackageVec.size() <= nIdx)
				return NULL;

			if (NULL == m_PackageVec[nIdx].s_pFileHash)
				return NULL;

			size_t hashCode = GetHashCode(szName);
			if (0 == hashCode)
				return NULL;

			if (!m_PackageVec[nIdx].IsExistFile(hashCode))
				return NULL;

			return m_PackageVec[nIdx].s_pFileHash->GetHashInfo(hashCode);
		}

		void GetFileData(int nIdx, char** ppData, UINT64 nOffset, size_t nSize)
		{
			if (NULL == ppData)
				return;

			if (nIdx < 0)
				return;

			if ((int)m_PackageVec.size() <= nIdx)
				return;

			if (NULL == m_PackageVec[nIdx].s_pFilePack)
				return;

			if (0 == nSize)
				return;

			m_PackageVec[nIdx].s_pFilePack->_GetData(ppData, nOffset, nSize);
		}

		size_t CheckNewFileCount(int nDestIdx, int nSourceIdx)
		{
			if (nDestIdx < 0 || nSourceIdx < 0)
				return 0;

			if ((int)m_PackageVec.size() <= nDestIdx)
				return 0;

			if ((int)m_PackageVec.size() <= nSourceIdx)
				return 0;

			if (NULL == m_PackageVec[nSourceIdx].s_pFilePack || NULL == m_PackageVec[nSourceIdx].s_pFileHash)
				return 0;

			size_t newCount = 0;
			CsFPS::sCHUNK Chunk;
			CsFilePack* pPatchPack = m_PackageVec[nSourceIdx].s_pFilePack;
			std::map<size_t, CsFileHash::sINFO*>* pMap = m_PackageVec[nSourceIdx].s_pFileHash->GetHashMap();

			if (NULL == pMap)
				return 0;

			std::map<size_t, CsFileHash::sINFO*>::iterator it = pMap->begin();
			for (; it != pMap->end(); ++it)
			{
				if (NULL == it->second)
					continue;

				pPatchPack->_GetChunk(&Chunk, it->second->s_nOffset);

				if (!IsExistFile(nDestIdx, Chunk.s_szPath))
					newCount++;
			}

			return newCount;
		}

		void GetFileList(int const& nIdx, std::list<std::string>& files)
		{
			if (nIdx < 0)
				return;

			if (m_PackageVec.empty())
				return;

			if ((int)m_PackageVec.size() <= nIdx)
				return;

			CsFileHash* fileHash = m_PackageVec[nIdx].s_pFileHash;
			CsFilePack* filePack = m_PackageVec[nIdx].s_pFilePack;

			if (NULL == fileHash || NULL == filePack)
				return;

			std::map<size_t, CsFileHash::sINFO*>* pMap = fileHash->GetHashMap();

			if (NULL == pMap)
				return;

			std::map<size_t, CsFileHash::sINFO*>::iterator it = pMap->begin();
			for (; it != pMap->end(); ++it)
			{
				if (NULL == it->second)
					continue;

				CsFPS::sCHUNK Chunk;
				filePack->_GetChunk(&Chunk, it->second->s_nOffset);
				files.push_back(Chunk.s_szPath);
			}
		}

		bool UnPacking(void)
		{
			if (m_PackageVec.empty())
				return false;

			CsFileHash* fileHash = m_PackageVec[0].s_pFileHash;
			CsFilePack* filePack = m_PackageVec[0].s_pFilePack;

			if (NULL == fileHash || NULL == filePack)
				return false;

			std::map<size_t, CsFileHash::sINFO*>* pMap = fileHash->GetHashMap();

			if (NULL == pMap)
				return false;

			std::map<size_t, CsFileHash::sINFO*>::iterator it = pMap->begin();
			std::map<size_t, CsFileHash::sINFO*>::iterator itEnd = pMap->end();

			char* pAlloc = NULL;
			size_t nAllocSize = 0;

			for (; it != itEnd; ++it)
			{
				if (NULL == it->second)
					continue;

				CsFPS::sCHUNK Chunk;
				filePack->_GetChunk(&Chunk, it->second->s_nOffset);

				if (nAllocSize < it->second->s_nDataSize)
				{
					nAllocSize = it->second->s_nDataSize;
					free(pAlloc);
					pAlloc = (char*)malloc(nAllocSize);
				}

				if (NULL == pAlloc)
					continue;

				std::string path;
				std::string name;
				Chunk.GetFilePath(path, name);

				if (_stricmp(name.c_str(), "thumbs.db") == 0)
					continue;

				CreateFilePath(path.c_str());

				DWORD modeBackup = GetFileAttributesA(Chunk.s_szPath);
				DWORD mode = modeBackup;
				mode &= ~(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM);
				SetFileAttributesA(Chunk.s_szPath, mode);

				FILE* pCreateFile = NULL;
				std::string fullPath = path;
				fullPath += name;
				fopen_s(&pCreateFile, fullPath.c_str(), "wb");

				if (NULL == pCreateFile)
					continue;

				filePack->_GetData(&pAlloc, it->second->s_nOffset, it->second->s_nDataSize);
				fwrite(pAlloc, sizeof(char), it->second->s_nDataSize, pCreateFile);
				fclose(pCreateFile);
			}

			if (pAlloc != NULL)
			{
				free(pAlloc);
				pAlloc = NULL;
			}

			return true;
		}

	private:
		CONT_PACKAGE_VEC	m_PackageVec;
	};

	ResSupplierImpl* CsFPSystem::ms_pImpl = 0;
	bool				CsFPSystem::ms_bUsePackage = false;
	char* CsFPSystem::m_pBuffer = 0;

	bool CsFPSystem::Initialize(bool bUsePackage, std::list<std::string> const& cnPkgNamevec, bool bWrite)
	{
		if (ms_pImpl)
		{
			PackLogA("Initialize(list) failed: already initialized");
			return false;
		}

		ms_bUsePackage = bUsePackage;

		if (!ms_bUsePackage)
		{
			PackLogA("Initialize(list): file pack disabled");
			return true;
		}

		if (cnPkgNamevec.empty())
		{
			PackLogA("Initialize(list) failed: package list is empty");
			return false;
		}

		ms_pImpl = new ResSupplierImpl;
		if (!ms_pImpl)
		{
			PackLogA("Initialize(list) failed: ResSupplierImpl allocation failed");
			return false;
		}

		std::list<std::string>::const_iterator it = cnPkgNamevec.begin();
		while (it != cnPkgNamevec.end())
		{
			PackLogPackageA("Initialize(list): loading package: ", (*it));

			if (!ms_pImpl->LoadPackage((*it), bWrite))
			{
				std::string msgA = "Failed to load pack: ";
				msgA += (*it);
				PackLogA(msgA.c_str());

				CsMessageBox(MB_OK, _T("Failed to load packs! Check Data\\Pack01.pf/.hf and Data\\Pack03.pf/.hf"));

				ms_pImpl->ReleasePackage();
				SAFE_DELETE(ms_pImpl);
				return false;
			}

			++it;
		}

		if (bWrite)
			m_pBuffer = new char[FP_BLOCK_SIZE];

		PackLogA("Initialize(list) ok");

		return true;
	}

	bool CsFPSystem::Initialize(bool bUsePackage, std::string const& cnPkgNamevec, bool bWrite)
	{
		if (ms_pImpl)
		{
			PackLogA("Initialize(single) failed: already initialized");
			return false;
		}

		ms_bUsePackage = bUsePackage;

		if (!ms_bUsePackage)
		{
			PackLogA("Initialize(single): file pack disabled");
			return true;
		}

		if (0 == cnPkgNamevec.length())
		{
			PackLogA("Initialize(single) failed: package name is empty");
			return false;
		}

		ms_pImpl = new ResSupplierImpl;
		if (!ms_pImpl)
		{
			PackLogA("Initialize(single) failed: ResSupplierImpl allocation failed");
			return false;
		}

		PackLogPackageA("Initialize(single): loading package: ", cnPkgNamevec);

		if (!ms_pImpl->LoadPackage(cnPkgNamevec, bWrite))
		{
			std::string msgA = "Failed to load pack: ";
			msgA += cnPkgNamevec;
			PackLogA(msgA.c_str());

			CsMessageBox(MB_OK, _T("Failed to load pack! Check Data\\Pack01.pf/.hf and Data\\Pack03.pf/.hf"));

			ms_pImpl->ReleasePackage();
			SAFE_DELETE(ms_pImpl);
			return false;
		}

		if (bWrite)
			m_pBuffer = new char[FP_BLOCK_SIZE];

		PackLogA("Initialize(single) ok");

		return true;
	}

	void CsFPSystem::Destroy(void)
	{
		PackLogA("Destroy start");

		if (ms_pImpl)
			ms_pImpl->ReleasePackage();

		SAFE_DELETE(ms_pImpl);
		SAFE_DELETE_ARRAY(m_pBuffer);

		ms_bUsePackage = false;

		PackLogA("Destroy ok");
	}

	bool CsFPSystem::IsUsePackage(void)
	{
		return ms_bUsePackage;
	}

	bool CsFPSystem::IsExist(LPCSTR p_psPath)
	{
		if (NULL == p_psPath || 0 == lstrlenA(p_psPath))
			return false;

		if (ms_pImpl && ms_pImpl->IsExistFile(p_psPath))
			return true;

		return IsExistFromDrive(p_psPath);
	}

	bool CsFPSystem::IsExist(int nIdx, LPCSTR p_psPath)
	{
		if (NULL == p_psPath || 0 == lstrlenA(p_psPath))
			return false;

		if (ms_pImpl && ms_pImpl->IsExistFile(nIdx, p_psPath))
			return true;

		return IsExistFromDrive(p_psPath);
	}

	bool CsFPSystem::IsExistOnlyPack(int nIdx, LPCSTR p_psPath)
	{
		if (NULL == p_psPath || 0 == lstrlenA(p_psPath))
			return false;

		if (ms_pImpl && ms_pImpl->IsExistFile(nIdx, p_psPath))
			return true;

		return false;
	}

	bool CsFPSystem::IsExistFromDrive(LPCSTR p_psPath)
	{
		if (NULL == p_psPath || 0 == lstrlenA(p_psPath))
			return false;

		DWORD dwAttr = ::GetFileAttributesA(p_psPath);
		if (dwAttr == INVALID_FILE_ATTRIBUTES)
			return false;

		return (dwAttr & FILE_ATTRIBUTE_DIRECTORY) ? false : true;
	}

	bool CsFPSystem::IsExistPackFile(LPCSTR pPackName)
	{
		if (NULL == pPackName || 0 == lstrlenA(pPackName))
			return false;

		std::string packFile = pPackName;
		packFile += PACK_EX_NAME;

		if (!IsExistFromDrive(packFile.c_str()))
			return false;

		std::string hashFile = pPackName;
		hashFile += HASH_EX_NAME;

		if (!IsExistFromDrive(hashFile.c_str()))
			return false;

		return true;
	}

	int CsFPSystem::GetFileHandle(int nFilePackIndex, LPCSTR szPath)
	{
		if (NULL == ms_pImpl)
			return 0;

		return ms_pImpl->GetFileHandle(nFilePackIndex, szPath);
	}

	size_t CsFPSystem::Allocate_GetFileData(int nFilePackIndex, char** ppData, size_t nHashCode)
	{
		if (NULL == ppData)
			return 0;

		*ppData = NULL;

		if (NULL == ms_pImpl)
			return 0;

		return ms_pImpl->ReadData(nFilePackIndex, nHashCode, ppData);
	}

	size_t CsFPSystem::Allocate_GetFileData(int nFilePackIndex, char** ppData, LPCSTR szPath)
	{
		if (NULL == ppData)
			return 0;

		*ppData = NULL;

		if (NULL == szPath || 0 == lstrlenA(szPath))
			return 0;

		if (NULL == ms_pImpl)
			return 0;

		return ms_pImpl->ReadData(nFilePackIndex, GetHashCode(szPath), ppData);
	}

	size_t CsFPSystem::GetTotalFileCount(int nIdx)
	{
		if (NULL == ms_pImpl)
			return 0;

		return ms_pImpl->GetTotalFileCount(nIdx);
	}

	bool CsFPSystem::AddFile(LPCSTR cDataPath, LPCSTR cFilePath)
	{
		if (NULL == ms_pImpl)
			return false;

		return ms_pImpl->AddFile(cDataPath, cFilePath, m_pBuffer);
	}

	bool CsFPSystem::EditFile(LPCSTR cDataPath, LPCSTR cFilePath)
	{
		if (NULL == ms_pImpl)
			return false;

		return ms_pImpl->EditFilePack(cDataPath, cFilePath, m_pBuffer);
	}

	bool CsFPSystem::EditFile(int nDestIdx, int nSourceIdx)
	{
		if (NULL == ms_pImpl)
			return false;

		return ms_pImpl->EditFilePack(nDestIdx, nSourceIdx);
	}

	bool CsFPSystem::SetAllocCount(size_t nCount)
	{
		if (NULL == ms_pImpl)
			return false;

		return ms_pImpl->SetAllocCount(nCount);
	}

	bool CsFPSystem::AddAllocCount(size_t nCount)
	{
		if (NULL == ms_pImpl)
			return false;

		return ms_pImpl->AddAllocCount(nCount);
	}

	bool CsFPSystem::SaveHashPack(void)
	{
		if (NULL == ms_pImpl)
			return false;

		return ms_pImpl->SaveHashPack();
	}

	bool CsFPSystem::UnPacking(void)
	{
		if (NULL == ms_pImpl)
			return false;

		return ms_pImpl->UnPacking();
	}

	size_t CsFPSystem::CheckNewFileCount(int nDestIdx, int nSourceIdx)
	{
		if (NULL == ms_pImpl)
			return 0;

		return ms_pImpl->CheckNewFileCount(nDestIdx, nSourceIdx);
	}

	bool CsFPSystem::SeekLock(int nIdx)
	{
		if (NULL == ms_pImpl)
			return false;

		return ms_pImpl->SeekLock(nIdx);
	}

	void CsFPSystem::SeekUnLock(int nIdx)
	{
		if (NULL == ms_pImpl)
			return;

		ms_pImpl->SeekUnLock(nIdx);
	}

	CsFileHash::sINFO* CsFPSystem::GetHashData(int nIdx, LPCSTR szName)
	{
		if (NULL == szName || 0 == lstrlenA(szName))
			return NULL;

		if (NULL == ms_pImpl)
			return NULL;

		return ms_pImpl->GetHashData(nIdx, szName);
	}

	void CsFPSystem::GetFileData(int nIdx, char** ppData, UINT64 nOffset, size_t nSize)
	{
		if (NULL == ppData)
			return;

		if (NULL == ms_pImpl)
			return;

		ms_pImpl->GetFileData(nIdx, ppData, nOffset, nSize);
	}

	size_t CsFPSystem::GetFileData(int nIdx, LPCSTR szName, std::vector<unsigned char>& vecData)
	{
		vecData.clear();

		if (NULL == szName || 0 == lstrlenA(szName))
			return 0;

		if (NULL == ms_pImpl)
			return 0;

		return ms_pImpl->ReadData(nIdx, GetHashCode(szName), vecData);
	}

	void CsFPSystem::GetFileList(int nIdx, std::list<std::string>& files)
	{
		if (NULL == ms_pImpl)
			return;

		ms_pImpl->GetFileList(nIdx, files);
	}
}