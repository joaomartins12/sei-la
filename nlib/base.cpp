/*
	RECODED NLIB, FOR BUILDING DMO LEAKED SOURCES

	@author Arves100
	@date 17/02/2022
	@file base.cpp
	@brief Basic nlib functions
*/

// ltrim/rtrim from https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring

#include "base.h"
#include <time.h>
#include <iomanip>
#include <sstream>

// TODO: replace logging with corsacbase log

void xstop(bool cnd, const char* fmt, ...)
{
	if (!cnd)
	{
		va_list vl;
		va_start(vl, fmt);
		vprintf(fmt, vl);
		va_end(vl);
		assert(false);
		exit(-99);
	}
}

void xassert(bool cnd, const char* fmt, ...)
{
	if (!cnd)
	{
		va_list vl;
		va_start(vl, fmt);
		vprintf(fmt, vl);
		va_end(vl);
		assert(false);
	}
}

void xlog(const char* fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);
}

void xdbg(const char* fmt, ...)
{
	va_list vl;
	va_start(vl, fmt);
	vprintf(fmt, vl);
	va_end(vl);
}

namespace nBase
{
	void strcpy(char* dst, const char* src) { ::strcpy(dst, src); }

	char* toupper(char* src)
	{
		for (size_t i = 0; i < ::strlen(src); i++)
			src[i] = ::toupper(src[i]);

		return src;
	}

	int strlen(const wchar_t* p) { return ::wcslen(p); }

	int rand(int s, int e)
	{
		return (::rand() % (e - s)) + s;
	}

	std::wstring m2w(std::string in)
	{
		auto len = MultiByteToWideChar(949, 0, in.c_str(), in.length(), nullptr, 0);

		std::wstring wsTmp;
		wsTmp.resize(len);

		MultiByteToWideChar(949, 0, in.c_str(), -1, &wsTmp[0], len);

		return wsTmp;
	}

	std::string w2m(std::wstring in)
	{
		BOOL b;
		char dc = ' ';

		auto len = WideCharToMultiByte(949, 0, in.c_str(), -1, nullptr, 0, &dc, &b);

		std::string wsTmp;
		wsTmp.resize(len);

		WideCharToMultiByte(949, 0, in.c_str(), -1, &wsTmp[0], len, &dc, &b);

		return wsTmp;
	}

	std::string GetCpuName()
	{
		return ""; // TODO
	}

	std::string GetGpuName()
	{
		return ""; // TODO
	}

	std::string GetOS()
	{
		return ""; // TODO
	}

	std::string GetDxVersion()
	{
		return ""; // TODO
	}

	u4 GetTotalMemory()
	{
		return 0; // TODO
	}

	uint32_t GetTime(const char* str)
	{
		struct tm t;
		std::stringstream r;
		r >> std::get_time(&t, str);

		if (r.fail())
			return 0;

		return (uint32_t)mktime(&t);
	}

	void SetCurDir2ExeFilePos()
	{
		// TODO
	}

	// trim from end (in place)
	std::wstring rtrim(std::wstring s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), [](wint_t ch) {
			return !std::iswspace(ch);
			}).base(), s.end());

		return s;
	}

	// trim from start (in place)
	std::wstring ltrim(std::wstring s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](wint_t ch) {
			return !std::iswspace(ch);
			}));

		return s;
	}

	const char* GetDataPath(const char* p)
	{
		// TODO: this was actually never used
		return p;
	}

	void GetTime2TM(uint32_t unixTimestamp, tm& t)
	{
		time_t c = (time_t)unixTimestamp;
		t = *localtime(&c);
	}

	unsigned int GetTickCount()
	{
		return ::GetTickCount();
	}
}

int GetCurrentACP()
{
	return GetACP();
}

namespace iReceiver
{
	static R g_rMax = { 4096 };

	R* GetPacket()
	{
		return &g_rMax;
	}
}
