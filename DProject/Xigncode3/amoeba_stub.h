// Copyright (C) 2007 - 2018 Wellbia.com Co., Ltd.

#ifndef AMOEBA_STUB_H
#define AMOEBA_STUB_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "xtypes.h"
#include "zcwave_errors.h"

#ifdef _WIN64

#include "amoeba_x64-inl.h"

#else

#include "amoeba_x86-inl.h"

#endif

typedef HMODULE (WINAPI *XLoadLibraryWT)(LPCWSTR);

static
inline
HMODULE
XLoadLibraryW(xcwstr path)
{
	xpvoid vmem = VirtualAlloc(NULL
						, AMOEBA_DATA_SIZE
						, MEM_COMMIT | MEM_RESERVE
						, PAGE_EXECUTE_READWRITE);
	if(!vmem)
		return FALSE;

	xpvoid p = vmem;

	for(ULONG i=0; i< AMOEBA_DATA_CNT; ++i)
	{
		memcpy(p, AMOEBA_DATA_ARR[i], AMOEBA_DATA_ARR_SIZE[i]);
		p = XGetPtr(p, AMOEBA_DATA_ARR_SIZE[i]);
	}

	BYTE *D = (BYTE *) vmem;
	XLoadLibraryWT fn = (XLoadLibraryWT) (D + *(DWORD*)(D + 4));
	HMODULE mod = fn(path);
	xulong ge = GetLastError();

	VirtualFree(vmem, 0, MEM_DECOMMIT | MEM_RELEASE);

	if(!mod)
		SetLastError(ge);

	return mod;
}


#endif
