/*
	RECODED NLIB, FOR BUILDING DMO LEAKED SOURCES

	@author Arves100
	@date 17/02/2022
	@file base.h
	@brief Main nlib inclusion
*/
#pragma once

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <stdio.h>
#include <wchar.h>

#include <string>
#include <cwctype>

#include <Windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>

#undef PlaySound

typedef uint32_t u4;
typedef uint8_t u1;
typedef uint16_t u2;
typedef char n1;
typedef int16_t n2;
typedef int32_t n4;
typedef uint64_t u8;
typedef int64_t n8;
typedef uint32_t uint;
typedef wchar_t wchar;
typedef int64_t int64;
typedef uint16_t ushort;
typedef unsigned char uchar;
typedef uint16_t uint64;

#define null nullptr
#define xnew new
#define xdelete_array delete[]
#define xdelete delete

void xstop(bool cnd, const char* fmt, ...);
void xassert(bool cnd, const char* fmt, ...);
void xlog(const char* fmt, ...);
void xdbg(const char* fmt, ...);

#define xassert1 xassert
#define xassert2 xassert
#define xstop1 xstop
#define xstop2 xstop
#define xstop3 xstop

#define LOG xlog
#define DBG xdbg
#define DBG2 xdbg
#define LOG2 xlog

namespace nBase
{
	void strcpy(char* dst, const char* src);
	char* toupper(char* src);
	int strlen(const wchar_t* p);

	int rand(int s = 0, int e = INT_MAX);

	std::wstring m2w(std::string in);

	std::string w2m(std::wstring in);

	std::string GetCpuName();

	std::string GetGpuName();

	std::string GetOS();

	std::string GetDxVersion();

	u4 GetTotalMemory();

	void SetCurDir2ExeFilePos();

	// trim from end (in place)
	std::wstring rtrim(std::wstring s);

	std::wstring ltrim(std::wstring s);

	void GetTime2TM(uint32_t unixTimestamp, tm& t);

	uint GetTime(const char* strTime);

	uint GetTickCount();
}

// stub: actually never used
const char* GetDataPath(const char* p);
int GetCurrentACP();

namespace iReceiver
{
	struct R
	{
		uint m_wSize;
	};

	R* GetPacket();
}
