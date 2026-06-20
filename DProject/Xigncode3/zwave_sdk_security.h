// Copyright (C) 2007 - 2018 Wellbia.com Co., Ltd.

#ifndef ZWAVE_SDK_SECURITY_H
#define ZWAVE_SDK_SECURITY_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>

#include "xtypes.h"
#include "zcwave_errors.h"
#include "zwave_sdk_client.h"

typedef struct _XIGNCODE_SS_HANDLE
{
	xulong magic;
	HMODULE module;
	XxxQueryFunctionT XxxQueryFunction;
	XxxSendCommandVaT XxxSendCommandVa;		
} XIGNCODE_SS_HANDLE, *PXIGNCODE_SS_HANDLE;

#define XIGNCODE_SS_HANDLE_MAGIC 0x17532018

XINLINE
xstatus
XCALL
ZCWAVE_SecurityEnter(xphandle sh)
{
	try
	{
		xstatus s;
		XIGNCODE_SS_HANDLE tsh;

		tsh.magic = XIGNCODE_SS_HANDLE_MAGIC;
		tsh.module = GetModuleHandleW(L"x3.xem");
		if(!tsh.module)
			return X_E_UNSUCCESSFUL;

		tsh.XxxQueryFunction = (XxxQueryFunctionT)(xpvoid) GetProcAddress(tsh.module, MAKEINTRESOURCEA(1));
		if(!tsh.XxxQueryFunction)
			return X_E_UNSUCCESSFUL;

		s = tsh.XxxQueryFunction((xpvoid *) &tsh.XxxSendCommandVa, XclioFidSendCommandVa);
		if(!X_SUCCEEDED(s))
			return s;

		PXIGNCODE_SS_HANDLE psh = (PXIGNCODE_SS_HANDLE) malloc(sizeof(tsh));
		if(!psh)
			return X_E_INSUFFICIENT_RESOURCES;

		memcpy(psh, &tsh, sizeof(*psh));
		*sh = psh;
	}
	catch(...)
	{
		return X_E_UNSUCCESSFUL;
	}

	return X_SUCCESS;
}

XINLINE
xstatus
XCALL
ZCWAVE_SecurityExit(xhandle sh)
{
	try
	{
		PXIGNCODE_SS_HANDLE psh;

		if(!sh)
			return X_E_INVALID_PARAMETER;

		psh = (PXIGNCODE_SS_HANDLE) sh;

		if(psh->magic != XIGNCODE_SS_HANDLE_MAGIC)
			return X_E_INVALID_PARAMETER;

		free(psh);
	}
	catch(...)
	{
		return X_E_UNSUCCESSFUL;
	}

	return X_SUCCESS;
}

XINLINE
xstatus
XCALL
ZCWAVE_SecuritySendCommand(xhandle sh, xulong cid, ...)
{
	try
	{
		va_list ap;
		PXIGNCODE_SS_HANDLE psh;

		if(!sh)
			return X_E_INVALID_PARAMETER;

		psh = (PXIGNCODE_SS_HANDLE) sh;

		if(psh->magic != XIGNCODE_SS_HANDLE_MAGIC)
			return X_E_INVALID_PARAMETER;

		va_start(ap, cid);
		xbool bret = psh->XxxSendCommandVa(cid, ap);
		va_end(ap);

		if(!bret)
		{
			xstatus s = GetLastError();
			if(!X_SUCCEEDED(s))
				return s;

			return X_E_UNSUCCESSFUL;
		}
	}
	catch(...)
	{
		return X_E_UNSUCCESSFUL;
	}

	return X_SUCCESS;
}

XINLINE
xhandle
XCALL
XxxOpenProcessStub(xhandle sh, xulong am, xbool inherit, xulong pid)
{
	try
	{
		xstatus s;
		xhandle process = NULL;
		xulong seed = GetTickCount();
		xulong out_seed;
		
		s = ZCWAVE_SecuritySendCommand(sh, ZCMD_GET_NEXT_SEED, seed, &out_seed);
		if(!X_SUCCEEDED(s))
		{
			SetLastError(s);
			return NULL;
		}

		s = ZCWAVE_SecuritySendCommand(sh, ZCMD_OPEN_PROCESS, &process, am, inherit, pid, seed, &out_seed);
		if(!X_SUCCEEDED(s))
		{
			SetLastError(s);
			return NULL;
		}

		s = ZCWAVE_SecuritySendCommand(sh, ZCMD_CHECK_SEED, seed, out_seed);
		if(!X_SUCCEEDED(s))
		{
			if(process)
				CloseHandle(process);

			SetLastError(s);
			return NULL;
		}

		return process;
	}
	catch(...)
	{
	}

	return NULL;
}

#define XOPENPROCESS(sh, am, inherit, pid) XxxOpenProcessStub(sh, am, inherit, pid)

#endif
