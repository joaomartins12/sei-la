// Copyright (C) 2007 - 2018 Wellbia.com Co., Ltd.

#ifndef ZCWAVE_SDK_CLIENT_STUB_H
#define ZCWAVE_SDK_CLIENT_STUB_H

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>

#include "xtypes.h"
#include "zcwave_errors.h"

#ifdef _WIN64
#include "zwave_sdk_client_stub_x64-inl.h"
#else
#include "zwave_sdk_client_stub_x86-inl.h"
#endif

typedef BOOL (WINAPI *LinkThunkT)(xpvoid pfn, xpuch arr[], xpulong arr_size, xsize cnt, xulong flags);

static FARPROC s_pqfn = NULL;

static
BOOL
LinkThunk(xulong flags)
{
	xpvoid code = VirtualAlloc(NULL
						, sizeof(X_SECURITY_DATA1)
						, MEM_COMMIT | MEM_RESERVE
						, PAGE_EXECUTE_READWRITE);
	if(!code)
		return X_E_INSUFFICIENT_RESOURCES;

	memcpy(code, X_SECURITY_DATA1, sizeof(X_SECURITY_DATA1));

	xpuch pb = (xpuch) code;
	LinkThunkT fn = (LinkThunkT) (pb + *(xpulong)(X_SECURITY_DATA0 + 20) - sizeof(X_SECURITY_DATA0));
 
	xstatus s = fn(&s_pqfn, X_SECURITY_DATA_ARR, X_SECURITY_DATA_ARR_SIZE, X_SECURITY_DATA_CNT, flags);
	VirtualFree(code, 0, MEM_DECOMMIT | MEM_RELEASE);

	return X_SUCCEEDED(s);
}

static
FARPROC
FindThunk(LPCSTR name)
{
	return s_pqfn;
}

#endif
