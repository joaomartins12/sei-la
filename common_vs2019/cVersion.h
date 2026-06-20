
#pragma once
#include <stdio.h>
#include <string.h>

// 버전정보 형식(YYYYMMDD##) ## 는 카운트
#ifdef VERSION_KOR
#define VERSION_KOR_VALUE 19052901
const unsigned int g_nNetVersion = VERSION_KOR_VALUE;
#elif defined VERSION_USA
#define VERSION_USA_VALUE 19052801
const unsigned int g_nNetVersion = VERSION_USA_VALUE;
#elif defined VERSION_TH
#define VERSION_TH_VALUE 19051701
const unsigned int g_nNetVersion = VERSION_TH_VALUE;
#elif defined VERSION_HK
#define VERSION_HK_VALUE 19052001
const unsigned int g_nNetVersion = VERSION_HK_VALUE;
#elif defined VERSION_TW
#define VERSION_TW_VALUE 19042401
const unsigned int g_nNetVersion = VERSION_TW_VALUE;
#else
const unsigned int g_nNetVersion = 12102301;
#endif

namespace cVersion
{
	inline unsigned int GetVersion()
	{
		return unsigned int(g_nNetVersion / 100);
	}
	inline unsigned int GetPatch()
	{
		return (g_nNetVersion % 100);
	}

	inline bool isVersion(unsigned int version)
	{
		return GetVersion() == unsigned int(version / 100);
	}
	inline bool isPatch(unsigned int version)
	{
		return GetPatch() == (version % 100);
	}

	inline bool isVersion(unsigned int oldVersion, unsigned int newVersion)
	{
		return unsigned int(oldVersion / 100) == unsigned int(newVersion / 100);
	}
	inline bool isPatch(unsigned int oldVersion, unsigned int newVersion)
	{
		return unsigned int(oldVersion % 100) == unsigned int(newVersion % 100);
	}

	// stl hash 함수 참고
	inline unsigned int _Hash_seq_stl32(const char *_First, size_t _Count)
	{	// FNV-1a hash function for bytes in [_First, _First + _Count)

		const __int64 _FNV_offset_basis = 14695981039346656037ULL;
		const __int64 _FNV_prime = 1099511628211ULL;

		__int64 _Val = _FNV_offset_basis;
		for (size_t _Next = 0; _Next < _Count; ++_Next)
		{	// fold in another byte
			_Val ^= (__int64)_First[_Next];
			_Val *= _FNV_prime;
		}
		return unsigned int(_Val & 0x00000000ffffffff);
	}

	inline __int64 _Hash_seq_stl64(const char *_First, size_t _Count)
	{	// FNV-1a hash function for bytes in [_First, _First + _Count)

		const __int64 _FNV_offset_basis = 14695981039346656037ULL;
		const __int64 _FNV_prime = 1099511628211ULL;

		__int64 _Val = _FNV_offset_basis;
		for (size_t _Next = 0; _Next < _Count; ++_Next)
		{	// fold in another byte
			_Val ^= (__int64)_First[_Next];
			_Val *= _FNV_prime;
		}
		return _Val;
	}

	inline unsigned int MakeAccessCode(unsigned int idx, unsigned int magicNumber)
	{
		char buf[32] = {};
		sprintf_s(buf, "%d%d%d", magicNumber, idx, cVersion::GetVersion());
		return _Hash_seq_stl32(buf, strlen(buf));
	}

	inline bool CheckAccessCode(unsigned int idx, unsigned int magicNumber, unsigned int accessCode)
	{
		unsigned int compCode = MakeAccessCode(idx, magicNumber);
		return (compCode == accessCode);
	}
};