// Copyright (C) 2007 - 2018 Wellbia.com Co., Ltd.

#ifndef ZCWAVE_SDK_CLIENT_H
#define ZCWAVE_SDK_CLIENT_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <shellapi.h>

#include "xtypes.h"
#include "zcwave_errors.h"

#ifdef XIGNCODE_STATIC_LINK

#include "zwave_sdk_client_stub.h"

#endif

#include "amoeba_stub.h"

#ifndef _ERRCODE_DEFINED
#ifndef _ERRNO_T_DEFINED

typedef int errno_t;

#endif
#endif

#ifdef XIGNCODE_STATIC_LINK

#define STUB_NAME L""
#define STUB_FILE_NAME L""

#define IPC_STUB_NAME L""
#define IPC_STUB_FILE_NAME L""


#else

#ifdef _WIN64
#define STUB_NAME L"x3_x64.xem"
#define STUB_FILE_NAME L"\\" STUB_NAME

#define IPC_STUB_NAME L"xcorona_x64.xem"
#define IPC_STUB_FILE_NAME L"\\" IPC_STUB_NAME

#else

#define STUB_NAME L"x3.xem"
#define STUB_FILE_NAME L"\\" STUB_NAME

#define IPC_STUB_NAME L"xcorona.xem"
#define IPC_STUB_FILE_NAME L"\\" IPC_STUB_NAME

#endif

#endif

#define XINLINE __inline

#define ZSOPT_USESYSGUARD				0x01
#define ZSOPT_SHOWSPLASH				0x02
#define ZSOPT_SHOWTRAY					0x04
#define ZSOPT_IPC						0x08
#define ZSOPT_ASYNC						0x10
#define ZSOPT_CHILD_PROCESS				0x20
#define ZSOPT_NO_UPDATE					0x40
#define ZSOPT_CAPTURE_LOL_USER_NAME		0x80
#define ZSOPT_NO_HOST_CHECKER			0x100
#define ZSOPT_SILENT					0x200
#define ZSOPT_INIT_EX					0x400
#define ZSOPT_STATIC					0x800
#define ZSOPT_SKIP_CHECKING_ENV			0x1000
#define ZSOPT_SAFE_XMOD					0x2000
#define ZSOPT_COMPAT_WITH_RP			0x4000

#ifndef _CryptType
#define _CryptType
typedef enum _CryptType 
{ 
 Blowfish
 , Rijndael
 , Rc4 
} CryptType;
#endif 

enum
{
	XIGNCODE_REQUEST_EXIT = 0x1200
};

enum
{
	XSC_INTERNAL_STATE_CHANGED = 0x3120
	, XSC_FILE_MODIFIED
	, XSC_VASHJ_LOAD_FAIL
	, XSC_VASHJ_INIT_FAIL
	, XSC_XSG_CONNECT_FAIL
};

#ifndef XCLIO_FID
#define XCLIO_FID

typedef enum _XclioFid
{
	XclioFidSysEnterA
	, XclioFidSysEnterW
	, XclioFidSysExit
	, XclioFidInit
	, XclioFidCleanup
	, XclioFidProbe
	, XclioFidProbeEx
	, XclioFidCreateCodeBox
	, XclioFidCloseCodeBox
	, XclioFidProbeCodeBox
	, XclioFidProbeCodeBoxEx
	, XclioFidEncrypt
	, XclioFidDecrypt
	, XclioFidRsaCreate
	, XclioFidRsaClose
	, XclioFidRsaSetPublicKey
	, XclioFidRsaSetPrivateKey
	, XclioFidRsaPublicEncrypt
	, XclioFidRsaPublicDecrypt
	, XclioFidRsaPrivateEncrypt
	, XclioFidRsaPrivateDecrypt
	, XclioFidCheck
	, XclioFidRegisterCallback
	, XclioFidRsaGenerateKey
	, XclioFidRsaFreeBuffer
	, XclioFidSetup
	, XclioFidSendCommandVa

	, XclioFidReserved1
	, XclioFidReserved2
	, XclioFidReserved3
	, XclioFidReserved4

	, XclioFidSysEnterExW
} XclioFid;

#endif

#define REPORT_FLAG_TERM			0x04000000
#define REPORT_FLAG_EXIT			0x80000000
#define REPORT_FLAG_NOTIFY			0x20000000

#define ZCWAVE_EQ_REMOTE_IP			0x1
#define ZCWAVE_EQ_REMOTE_PORT		0x2
#define ZCWAVE_EQ_LOCAL_IP			0x4
#define ZCWAVE_EQ_LOCAL_PORT		0x8
#define ZCWAVE_EQ_PID				0x10
#define ZCWAVE_EQ_PROTOCOL			0x20
#define ZCWAVE_EQ_DIRECTION			0x40

#define ZCWAVE_NEQ_REMOTE_IP		0x00010000
#define ZCWAVE_NEQ_REMOTE_PORT		0x00020000
#define ZCWAVE_NEQ_LOCAL_IP			0x00040000
#define ZCWAVE_NEQ_LOCAL_PORT		0x00080000
#define ZCWAVE_NEQ_PID				0x00100000
#define ZCWAVE_NEQ_PROTOCOL			0x00200000
#define ZCWAVE_NEQ_DIRECTION		0x00400000


#define ZCWAVE_ACT_ALLOW			1
#define ZCWAVE_ACT_DENY				2
#define ZCWAVE_ACT_PROMPT			4

#define ZCWAVE_DIR_IN				1
#define ZCWAVE_DIR_OUT				2

#define ZCWAVE_PROTO_UDP			1

#define ZCWAVE_AM_REMOTE_IP			0x1
#define ZCWAVE_AM_REMOTE_PORT		0x2
#define ZCWAVE_AM_LOCAL_IP			0x4
#define ZCWAVE_AM_LOCAL_PORT		0x8
#define ZCWAVE_AM_PID				0x10
#define ZCWAVE_AM_PROTOCOL			0x20
#define ZCWAVE_AM_DIRECTION			0x40

typedef struct _ZCWAVE_NET_RULE
{
	ULONG		conditions;
	ULONG		action;
	ULONG		direction;

	ULONG		remote_ip;
	USHORT		remote_port;

	ULONG		local_ip;
	USHORT		local_port;

	ULONG		pid;
	ULONG		protocol;
	
} ZCWAVE_NET_RULE, *PZCWAVE_NET_RULE;

typedef char *PACKETDATA;
typedef const char *CPACKETDATA;


typedef xbool (CALLBACK *XigncodeCallback)(ULONG code, ULONG w, PVOID l, PVOID context);

typedef void (CALLBACK *ProbeCallback)(xhandle CodeBox
									    , CPACKETDATA Packet
										, PACKETDATA Buffer
										, SIZE_T BufferSize
										, PVOID Context);




typedef xbool (XCALL *XxxSysEnterAT)(xcstr License, xcstr Dir, xulong Flags);

typedef xbool (XCALL *XxxSysEnterWT)(xcwstr License, xcwstr path, xulong opt);

typedef xbool (XCALL *XxxSysExitT)();

typedef xbool (XCALL *XxxInitT)();

typedef xbool (XCALL *XxxCleanupT)();

typedef xbool (XCALL *XxxProbeT)(xpcch data, xpch buf, xsize size);

typedef xbool (XCALL *XxxProbeExT)(xpcch Packet
		, xsize ReplySize
		, ProbeCallback Callback
		, xpvoid Context);

typedef xhandle (XCALL *XxxCreateCodeBoxT)();

typedef xbool (XCALL *XxxCloseCodeBoxT)(xhandle h);

typedef xbool (XCALL *XxxProbeCodeBoxT)(xhandle h, xpcch data, xpch buf, xsize size);

typedef xbool (XCALL *XxxProbeCodeBoxExT)(xhandle h
				, xpcch Packet
				, xsize ReplySize
				, ProbeCallback Callback
				, xpvoid Context);

typedef void (XCALL *XxxRegisterCallbackT)(XigncodeCallback cb, xpvoid context);

typedef void (XCALL *XxxPumpLoopT)();


typedef xstatus (XCALL *XxxQueryFunctionT)(xpvoid *If, XclioFid Id);
typedef xbool (XCALL *XxxSendCommandVaT)(xuint cid, va_list ap);

typedef xbool (XCALL *XxxEncryptT)
(
	xpcuch Input
	, xsize InputSize
	, xpcuch Key
	, xsize KeySize
	, xpuch Output
	, xsize OutputSize
	, xsize *Required
	, int Type
);

typedef xbool (XCALL *XxxDecryptT)
(
	xpcuch Input
	, xsize InputSize
	, xpcuch Key
	, xsize KeySize
	, xpuch Output
	, xsize OutputSize
	, xsize *Required
	, int Type
);

typedef xbool (XCALL *XxxRsaGenerateKeyT)
(
	xpuch PublicKey
	, xsize PublicKeySize
	, xpuch PrivateKey
	, xsize PrivateKeySize
	, int Nbits
	, int Exponent
);

typedef xbool (XCALL *XxxRsaSetPrivateKeyT)(xhandle H, xpcuch Key, xsize KeySize);

typedef xbool (XCALL *XxxRsaSetPublicKeyT)(xhandle H, xpcuch Key, xsize KeySize);

typedef xbool (XCALL *XxxRsaPrivateEncryptT)
(
	xhandle H
	, xpcuch Input
	, xsize InputSize
	, xpuch *Output
	, xsize *OutputSize
);

typedef xbool (XCALL *XxxRsaPrivateDecryptT)
(
	xhandle H
	, xpcuch Input
	, xsize InputSize
	, xpuch *Output
	, xsize *OutputSize
);

typedef xbool (XCALL *XxxRsaPublicEncryptT)
(
	xhandle H
	, xpcuch Input
	, xsize InputSize
	, xpuch *Output
	, xsize *OutputSize
);

typedef xbool (XCALL *XxxRsaPublicDecryptT)
(
	xhandle H
	, xpcuch Input
	, xsize InputSize
	, xpuch *Output
	, xsize *OutputSize
);

typedef xbool (XCALL *XxxRsaCloseT)(xhandle H);

typedef xhandle (XCALL *XxxRsaCreateT)();

typedef xbool (XCALL *XxxRsaFreeBufferT)(xhandle H, xpuch Buffer);

typedef xbool (XCALL *ProbeThreadEnterCallbackT)(xpvoid ctx);
typedef void (XCALL *ProbeThreadExitCallbackT)(xpvoid ctx);



#define XUID_USER_ACCOUNT			1
#define XUID_USER_NAME				2
#define XUID_USER_MAP_POINT			3
#define XUID_USER_EXTRA				4

#define ZCMD_VERIFY_GAME_PARAM_A				80
#define ZCMD_VERIFY_GAME_PARAM_W				81
#define ZCMD_CHECK_EXCEPTION_INFO				82
#define ZCMD_SET_GAME_STAGE						90
#define ZCMD_SET_USER_INFO_CALLBACKA			100
#define ZCMD_SET_USER_INFO_CALLBACKW			200
#define ZCMD_GET_USER_INFO_CALLBACKA			300
#define ZCMD_GET_USER_INFO_CALLBACKW			400
#define ZCMD_GET_USER_INFO_CONTEXTA				500
#define ZCMD_GET_USER_INFO_CONTEXTW				600
#define ZCMD_SET_NOTIFICATION_CALLBACK			700
#define ZCMD_GET_NOTIFICATION_CALLBACK			800
#define ZCMD_GET_NOTIFICATION_CONTEXT			900
#define ZCMD_SET_PROBE_THREAD_ENTER_CALLBACK	910
#define ZCMD_SET_PROBE_THREAD_EXIT_CALLBACK		920
#define ZCMD_ADD_WHITE_THREAD_ID				930
#define ZCMD_DELETE_WHITE_THREAD_ID				940
#define ZCMD_IS_WHITE_THREAD_ID					950
#define ZCMD_IS_ALLOWED_PROCESS_ID				960
#define ZCMD_IS_XIGNCODE_PTR					970
#define ZCMD_GET_SYSTEM_ID						971	// ZCMD_GET_SYSTEM_ID, &guid
#define ZCMD_GET_CORONA_SYSTEM_VER				972

#define ZCMD_CP_REGISTER_CALLBACK				980
#define ZCMD_CP_IS_XIGNCODE_RUNNING				981
#define ZCMD_CP_ENTER							982
#define ZCMD_CP_EXIT							983


#define ZCMD_NET_ADD_RULE				1000
#define ZCMD_NET_REMOVE_RULE			1100
#define ZCMD_PROTECT_FUNCTION			1200
#define ZCMD_UNPROTECT_FUNCTION			1300
#define ZCMD_SET_DIRECTX_INTERFACE		1400

#define ZCMD_DPM_ADD					1500
#define ZCMD_DPM_DELETE					1600
#define ZCMD_DPM_BEGIN_UPDATE			1700
#define ZCMD_DPM_END_UPDATE				1800
#define ZCMD_DPM_VERIFY					1900

#define ZCMD_LOAD_LIBRARYA				2000
#define ZCMD_LOAD_LIBRARYW				2100
#define ZCMD_LOAD_LIBRARY_EXA			2200
#define ZCMD_LOAD_LIBRARY_EXW			2300
#define ZCMD_FREE_LIBRARY				2400
#define ZCMD_GET_PROC_ADDRESS			2500
#define ZCMD_LOAD_TRINITY_LIBRARYA		2600
#define ZCMD_LOAD_TRINITY_LIBRARYW		2700
#define ZCMD_LOAD_TRINITY_LIBRARY_EXA	2800
#define ZCMD_LOAD_TRINITY_LIBRARY_EXW	2900
#define ZCMD_CHECK_SEED					3000
#define ZCMD_GET_NEXT_SEED				3050
#define ZCMD_MALLOC						3100
#define ZCMD_FREE						3200
#define ZCMD_FOPEN						3300
#define ZCMD_FCLOSE						3400
#define ZCMD_FREAD						3500
#define ZCMD_FWRITE						3600
#define ZCMD_FSEEK						3700
#define ZCMD_FTELL						3800

#define ZCMD_CREATE_FILE_A				3810
#define ZCMD_CREATE_FILE_W				3820
#define ZCMD_READ_FILE					3830
#define ZCMD_WRITE_FILE					3840
#define ZCMD_SET_FILE_POINTER			3850
#define ZCMD_VFSCANFS					3860
#define ZCMD_VFWSCANFS					3870
#define ZCMD_FOPENS						3880
#define ZCMD_WFOPENS					3890
#define ZCMD_VFPRINTF					3900
#define ZCMD_VFWPRINTF					3910
#define ZCMD_SHELLEXECUTE_A				3920
#define ZCMD_SHELLEXECUTE_W				3930
#define ZCMD_SHELLEXECUTE_EX_A			3940
#define ZCMD_SHELLEXECUTE_EX_W			3950
#define ZCMD_GETTICKCOUNT				3960
#define ZCMD_GETTICKCOUNT64				3970
#define ZCMD_GETVOLUMEINFORMATION_A		3980
#define ZCMD_GETVOLUMEINFORMATION_W		3990


#define ZCMD_CALL_FUNC					7000
#define ZCMD_GET_FAST_SAPI_ADDRESS		7100

#define ZCMD_OPEN_PROCESS				10000

#define ZCMD_BEGIN_PROBING_XIGNCODE_SERVER	11000
#define ZCMD_END_PROBING_XIGNCODE_SERVER	11100
#define ZCMD_CHECK_XIGNCODE_SYSTEM			11200

#define ZCMD_SR_GET_PATH				20000
#define ZCMD_SR_GET_MAJOR_CODE			20001
#define ZCMD_SR_GET_MINOR_CODE			20002
#define ZCMD_SR_GET_EXTRA_INFO			20003
#define ZCMD_SR_GET_FULL_DETECTION_LOG	20004
#define ZCMD_SR_GET_SYSTEM_ID			20005
#define ZCMD_SR_GET_USER_ACCOUNT		20006
#define ZCMD_SR_GET_TIME_STAMP			20007
#define ZCMD_SR_GET_TIME_STRING			20008
#define ZCMD_SR_GET_TYPE				20009
#define ZCMD_SR_GET_MD5_STRING			20010
#define ZCMD_GAME_SERVER_CONNECT		20011

#define XCORONA_SYSTEM_VERSION			0xA1

typedef 
xstatus 
(XCALL *ZCWAVE_GetUserInfoCallbackAT)(xuint info_id
										, xstr buffer
										, xsize buffer_size
										, xpvoid context);

typedef 
xstatus 
(XCALL *ZCWAVE_GetUserInfoCallbackWT)(xuint info_id
										, xwstr buffer
										, xsize buffer_size
										, xpvoid context);



typedef void (CALLBACK *WriteXigncodeLogWT)(xcwstr msg, xpvoid context);

#define X_MAX_LICENSE 15
#define X_MAX_PLATFORM 15

typedef enum _X3_GAME_STAGE
{
	kGsBegin = 17
	
	, kGsLaunched
	, kGsLoginComplete
	, kGsConnectComplete
	, kGsSecurityPacketComplete

	, kGsEnd
} X3_GAME_STAGE;

typedef struct _XIGNCODE_INIT_CONTEXTW
{
	xulong version;

	WriteXigncodeLogWT WriteSecurityLogW;
	XigncodeCallback OnHackDetected;
	ZCWAVE_GetUserInfoCallbackWT GetUserInfoW;

	xpvoid callback_context;
	xulong flags;

	xwch license[X_MAX_LICENSE];
	xwch platform[X_MAX_PLATFORM];

	xwch idata_path[X_MAX_PATH];
	xwch rdata_path[X_MAX_PATH];
	xwch log_path[X_MAX_PATH];

	xwch initial_dir[X_MAX_PATH];
	xwch runtime_dir[X_MAX_PATH];

} XIGNCODE_INIT_CONTEXTW, *PXIGNCODE_INIT_CONTEXTW;

typedef xbool (XCALL *XxxSysEnterExWT)(PXIGNCODE_INIT_CONTEXTW xic);

static HMODULE s_x3_module = NULL;
static XxxQueryFunctionT XxxQueryFunction = NULL;
static XxxInitT XxxInit = NULL;
static XxxCleanupT XxxCleanup = NULL;
static XxxProbeT XxxProbe = NULL;
static XxxCreateCodeBoxT XxxCreateCodeBox = NULL;
static XxxCloseCodeBoxT XxxCloseCodeBox = NULL;
static XxxProbeCodeBoxT XxxProbeCodeBox = NULL;
static XxxSendCommandVaT XxxSendCommandVa = NULL;

static XxxSysEnterWT XxxSysEnterW = NULL;
static XxxSysEnterExWT XxxSysEnterExW = NULL;

static XxxSysExitT XxxSysExit = NULL;
static XxxRegisterCallbackT XxxRegisterCallback = NULL;
static XxxProbeCodeBoxExT XxxProbeCodeBoxEx = NULL;
static XxxProbeExT XxxProbeEx = NULL;
static XxxEncryptT XxxEncrypt = NULL;
static XxxDecryptT XxxDecrypt = NULL;
static XxxRsaGenerateKeyT XxxRsaGenerateKey = NULL;
static XxxRsaSetPrivateKeyT XxxRsaSetPrivateKey = NULL;
static XxxRsaSetPublicKeyT XxxRsaSetPublicKey = NULL;
static XxxRsaPrivateEncryptT XxxRsaPrivateEncrypt = NULL;
static XxxRsaPrivateDecryptT XxxRsaPrivateDecrypt = NULL;
static XxxRsaPublicEncryptT XxxRsaPublicEncrypt = NULL;
static XxxRsaPublicDecryptT XxxRsaPublicDecrypt = NULL;
static XxxRsaCloseT XxxRsaClose = NULL;
static XxxRsaCreateT XxxRsaCreate = NULL;
static XxxRsaFreeBufferT XxxRsaFreeBuffer = NULL;

static HMODULE (XCALL *XxxFastLoadLibraryAWorker)(xcstr) = NULL;
static HMODULE (XCALL *XxxFastLoadLibraryWWorker)(xcwstr) = NULL;
static xbool (XCALL *XxxFastFreeLibraryWorker)(HMODULE) = NULL;
static FARPROC (XCALL *XxxFastGetProcAddressWorker)(HMODULE, xcstr) = NULL;
static xpvoid (XCALL *XxxFastMallocWorker)(xsize) = NULL;
static xbool (XCALL *XxxFastFreeWorker)(xpvoid) = NULL;
static FILE * (XCALL *XxxFastFopenWorker)(xcstr, xcstr) = NULL;
static xbool (XCALL *XxxFastFcloseWorker)(FILE *) = NULL;
static xsize (XCALL *XxxFastFreadWorker)(xpvoid, xsize, xsize, FILE *) = NULL;
static xsize (XCALL *XxxFastFwriteWorker)(xpcvoid, xsize, xsize, FILE *) = NULL;
static xint (XCALL *XxxFastFseekWorker)(FILE *, xlong, xint) = NULL;
static xlong (XCALL *XxxFastFtellWorker)(FILE *) = NULL;
static HANDLE (XCALL *XxxFastCreateFileAWorker)(LPCSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6) = NULL;
static HANDLE (XCALL *XxxFastCreateFileWWorker)(LPCWSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6) = NULL;
static BOOL (XCALL *XxxFastReadFileWorker)(HANDLE a0, LPVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4) = NULL;
static BOOL (XCALL *XxxFastWriteFileWorker)(HANDLE a0, LPCVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4) = NULL;
static DWORD (XCALL *XxxFastSetFilePointerWorker)(HANDLE a0, LONG a1, PLONG a2, DWORD a3) = NULL;
static int (XCALL *XxxFastFscanfsWorker)(FILE *a0, const char *a1, va_list ap) = NULL;
static int (XCALL *XxxFastFwscanfsWorker)(FILE *a0, const wchar_t *a1, va_list ap) = NULL;
static errno_t (XCALL *XxxFastFopensWorker)(FILE** a0, const char *a1, const char *a2) = NULL;
static errno_t (XCALL *XxxFastWfopensWorker)(FILE** a0, const wchar_t *a1, const wchar_t *a2) = NULL;
static int (XCALL *XxxFastFprintfWorker)(FILE *a0, const char *a1, va_list ap) = NULL;
static int (XCALL *XxxFastFwprintfWorker)(FILE *a0, const wchar_t *a1, va_list ap) = NULL;
static HINSTANCE (XCALL *XxxFastShellExecuteAWorker)(HWND a0, xcstr a1, xcstr a2, xcstr a3, xcstr a4, xint a5) = NULL;
static HINSTANCE (XCALL *XxxFastShellExecuteWWorker)(HWND a0, xcwstr a1, xcwstr a2, xcwstr a3, xcwstr a4, xint a5) = NULL;
static xbool (XCALL *XxxFastShellExecuteExAWorker)(LPSHELLEXECUTEINFOA a0) = NULL;
static xbool (XCALL *XxxFastShellExecuteExWWorker)(LPSHELLEXECUTEINFOW a0) = NULL;
static xulong (XCALL *XxxFastGetTickCountWorker)(xvoid) = NULL;
static xuint64 (XCALL *XxxFastGetTickCount64Worker)(xvoid) = NULL;
static xbool (XCALL *XxxFastGetVolumeInformationAWorker)(xcstr a0, xstr a1, xulong a2, xulong *a3, xulong *a4, xulong *a5, xstr a6, xulong a7) = NULL;
static xbool (XCALL *XxxFastGetVolumeInformationWWorker)(xcwstr a0, xwstr a1, xulong a2, xulong *a3, xulong *a4, xulong *a5, xwstr a6, xulong a7) = NULL;

XINLINE
xbool
XCALL
XxxGetProcAddress(xvoid **Address, XclioFid Fid)
{
	xstatus s;

	if(!s_x3_module || !XxxQueryFunction)
	{
#ifdef XIGNCODE_STATIC_LINK

		XxxQueryFunction = (XxxQueryFunctionT)(xpvoid) FindThunk(MAKEINTRESOURCEA(1));

#else
		s_x3_module = GetModuleHandleW(STUB_NAME);
		if(!s_x3_module)
		{
			s_x3_module = GetModuleHandleW(IPC_STUB_NAME);
		}

		if(!s_x3_module)
		{
			SetLastError(ZCWAVE_E_INVALID_OPERATION);
			return X_FALSE;
		}

		XxxQueryFunction = (XxxQueryFunctionT)(xpvoid) GetProcAddress(s_x3_module, MAKEINTRESOURCEA(1));
#endif
	}

	if(!XxxQueryFunction)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_QUERYFUNCTION);
		return X_FALSE;
	}

	s = XxxQueryFunction(Address, Fid);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return X_FALSE;
	}

	return X_TRUE;
}

XINLINE
void
XCALL
GetXigncodeDirectoryW(xwstr Dir, xsize DirSize, xcwstr InputDir)
{
	WCHAR *DirMark;

	if(!InputDir)
	{
		GetModuleFileNameW(X_NULL, Dir, (xulong)(DirSize / sizeof(WCHAR)));
		DirMark = wcsrchr(Dir, X_DIR_MARKW);
		if(DirMark)
			*DirMark = X_EOSW;
	}
	else
	{
		WCHAR *DriveMark = (WCHAR *) wcschr(InputDir, L':');
		if(DriveMark)
		{
			#ifdef _STRSAFE_H_INCLUDED_
			StringCbCopyW(Dir, DirSize, InputDir);
			#else
			lstrcpyW(Dir, InputDir);
			#endif
		}
		else
		{
			GetModuleFileNameW(X_NULL, Dir, (xulong)(DirSize / sizeof(WCHAR)));
			DirMark = wcsrchr(Dir, X_DIR_MARKW);
			if(DirMark)
			{
				if(*InputDir == X_DIR_MARKW)
					*DirMark = X_EOSW;
				else
					*(DirMark + 1) = X_EOSW;
			}

			#ifdef _STRSAFE_H_INCLUDED_
			StringCbCatW(Dir, DirSize, InputDir);
			#else
			lstrcatW(Dir, InputDir);
			#endif
		}

		if(Dir[lstrlenW(Dir) - 1] == X_DIR_MARKW)
			Dir[lstrlenW(Dir) - 1] = X_EOSW;
	}
}

XINLINE
xbool
XCALL
ZCWAVE_SysInit()
{
	return X_TRUE;
}

XINLINE
xbool
XCALL
ZCWAVE_SysCleanup()
{
	return X_TRUE;
}

XINLINE
xbool
XCALL
ZCWAVE_Init()
{
	if(!XxxInit)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxInit, XclioFidInit))
			return X_FALSE;
	}

	if(!XxxInit)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_INIT);
		return X_FALSE;
	}

	return XxxInit();
}

XINLINE
xbool
XCALL
ZCWAVE_Cleanup()
{
	if(!XxxCleanup)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxCleanup, XclioFidCleanup))
			return X_FALSE;
	}

	if(!XxxCleanup)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_CLEANUP);
		return X_FALSE;
	}

	return XxxCleanup();
}

XINLINE
xbool
XCALL
ZCWAVE_Probe(xpcch ReceivedPacket, xpch Buffer, xsize BufferSize)
{
	if(!XxxProbe)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxProbe, XclioFidProbe))
			return X_FALSE;
	}

	if(!XxxProbe)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_PROBE);
		return X_FALSE;
	}

	return XxxProbe(ReceivedPacket, Buffer, BufferSize);
}

XINLINE
xhandle
XCALL
ZCWAVE_CreateCodeBox()
{
	if(!XxxCreateCodeBox)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxCreateCodeBox, XclioFidCreateCodeBox))
			return X_FALSE;
	}

	if(!XxxCreateCodeBox)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_CREATECODEBOX);
		return X_FALSE;
	}

	return XxxCreateCodeBox();
}

XINLINE
xbool
XCALL
ZCWAVE_CloseCodeBox(xhandle CodeBox)
{
	if(!XxxCloseCodeBox)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxCloseCodeBox, XclioFidCloseCodeBox))
			return X_FALSE;
	}

	if(!XxxCloseCodeBox)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_CLOSECODEBOX);
		return X_FALSE;
	}

	return XxxCloseCodeBox(CodeBox);
}

XINLINE
xbool
XCALL
ZCWAVE_ProbeCodeBox(xhandle CodeBox, xpcch ReceivedPacket, xpch Buffer, xsize BufferSize)
{
	if(!XxxProbeCodeBox)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxProbeCodeBox, XclioFidProbeCodeBox))
			return X_FALSE;
	}

	if(!XxxProbeCodeBox)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_PROBECODEBOX);
		return X_FALSE;
	}

	return XxxProbeCodeBox(CodeBox, ReceivedPacket, Buffer, BufferSize);
}

XINLINE
xbool
XCALL
ZCWAVE_SendCommandVa(xuint cid, va_list ap)
{
	if(!XxxSendCommandVa)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxSendCommandVa, XclioFidSendCommandVa))
			return X_FALSE;
	}
	
	if(!XxxSendCommandVa)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_SENDCOMMANDVA);
		return X_FALSE;
	}
	
	return XxxSendCommandVa(cid, ap);
}

XINLINE
xbool
XCALL
ZCWAVE_SendCommand(xuint cid, ...)
{
	va_list ap;
	xbool r;

	va_start(ap, cid);
	r = ZCWAVE_SendCommandVa(cid, ap);
	va_end(ap);
	
	return r;
}

XINLINE
xbool
XCALL
ZCWAVE_CpEnterW(xcwstr Path, xulong Flags)
{
	xwch StubPath[MAX_PATH], XigncodeDir[MAX_PATH];
	xcwstr stub_name = STUB_FILE_NAME;

	if(Flags & ZSOPT_IPC)
		stub_name = IPC_STUB_FILE_NAME;

	if(!GetModuleHandleW(stub_name))
	{
		GetXigncodeDirectoryW(XigncodeDir, sizeof(XigncodeDir), Path);
		
		#ifdef _STRSAFE_H_INCLUDED_
		StringCbCopyW(StubPath, sizeof(StubPath), XigncodeDir);
		StringCbCatW(StubPath, sizeof(StubPath), stub_name);
		#else
		lstrcpyW(StubPath, XigncodeDir);
		lstrcatW(StubPath, stub_name);
		#endif

#ifdef XIGNCODE_STATIC_LINK

		Flags |= ZSOPT_STATIC;

		if(!LinkThunk(Flags))
			return X_FALSE;

#else

		PVOID Module = XLoadLibraryW(StubPath);
		if(!Module)
		{
			return X_FALSE;
		}

#endif
	}

	return ZCWAVE_SendCommand(ZCMD_CP_ENTER, Flags);
}

XINLINE
xbool
XCALL
ZCWAVE_CpEnterA(xcstr Path, xulong Flags)
{
	xwch wPathBuffer[MAX_PATH];
	xwstr wPath = X_NULL;

	if(Path)
	{
		MultiByteToWideChar(CP_ACP, 0, Path, -1, wPathBuffer, XArraySize(wPathBuffer));
		wPath = wPathBuffer;
	}

	return ZCWAVE_CpEnterW(wPath, Flags);
}

XINLINE
xbool
XCALL
ZCWAVE_CpExit()
{
	return ZCWAVE_SendCommand(ZCMD_CP_EXIT);
}


XINLINE
xbool
XCALL
ZCWAVE_SysEnterW(xcwstr License, xcwstr Path, xulong Flags)
{
	xwch StubPath[MAX_PATH], XigncodeDir[MAX_PATH];
	xcwstr stub_name = STUB_FILE_NAME;
	
	GetXigncodeDirectoryW(XigncodeDir, sizeof(XigncodeDir), Path);

	if(!XxxSysEnterW)
	{
		if(Flags & ZSOPT_IPC)
			stub_name = IPC_STUB_FILE_NAME;

		#ifdef _STRSAFE_H_INCLUDED_
		StringCbCopyW(StubPath, sizeof(StubPath), XigncodeDir);
		StringCbCatW(StubPath, sizeof(StubPath), stub_name);
		#else
		lstrcpyW(StubPath, XigncodeDir);
		lstrcatW(StubPath, stub_name);
		#endif

#ifdef XIGNCODE_STATIC_LINK

		Flags |= ZSOPT_STATIC;

		if(!LinkThunk(Flags))
			return X_FALSE;

#else

		PVOID Module = XLoadLibraryW(StubPath);
		if(!Module)
		{
			return X_FALSE;
		}

#endif

		if(!XxxGetProcAddress((xpvoid *)&XxxSysEnterW, XclioFidSysEnterW))
		{
			return X_FALSE;
		}

	}

	if(!XxxSysEnterW)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_SYSENTERW);
		return X_FALSE;
	}

	if(!XxxSysEnterW(License, XigncodeDir, Flags))
		return X_FALSE;

	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_LOAD_LIBRARYA, &XxxFastLoadLibraryAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_LOAD_LIBRARYW, &XxxFastLoadLibraryWWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FREE_LIBRARY, &XxxFastFreeLibraryWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GET_PROC_ADDRESS, &XxxFastGetProcAddressWorker);

	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_CREATE_FILE_A, &XxxFastCreateFileAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_CREATE_FILE_W, &XxxFastCreateFileWWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_READ_FILE, &XxxFastReadFileWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_WRITE_FILE, &XxxFastWriteFileWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SET_FILE_POINTER, &XxxFastSetFilePointerWorker);

	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_MALLOC, &XxxFastMallocWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FREE, &XxxFastFreeWorker);

	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FOPEN, &XxxFastFopenWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FCLOSE, &XxxFastFcloseWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FREAD, &XxxFastFreadWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FWRITE, &XxxFastFwriteWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FSEEK, &XxxFastFseekWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FTELL, &XxxFastFtellWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFSCANFS, &XxxFastFscanfsWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFWSCANFS, &XxxFastFwscanfsWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FOPENS, &XxxFastFopensWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_WFOPENS, &XxxFastWfopensWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFPRINTF, &XxxFastFprintfWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFWPRINTF, &XxxFastFwprintfWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_A, &XxxFastShellExecuteAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_W, &XxxFastShellExecuteWWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_EX_A, &XxxFastShellExecuteExAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_EX_W, &XxxFastShellExecuteExWWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETTICKCOUNT, &XxxFastGetTickCountWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETTICKCOUNT64, &XxxFastGetTickCount64Worker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETVOLUMEINFORMATION_A, &XxxFastGetVolumeInformationAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETVOLUMEINFORMATION_W, &XxxFastGetVolumeInformationWWorker);

	return X_TRUE;
}


XINLINE
xbool
XCALL
ZCWAVE_SysEnterExW(PXIGNCODE_INIT_CONTEXTW xic)
{
	xwch StubPath[MAX_PATH], XigncodeDir[MAX_PATH];
	xcwstr stub_name = STUB_FILE_NAME;
	
	GetXigncodeDirectoryW(XigncodeDir, sizeof(XigncodeDir), xic->runtime_dir);

	xic->flags |= ZSOPT_INIT_EX;

#ifdef XIGNCODE_STATIC_LINK
	xic->flags |= ZSOPT_STATIC;
#endif

	if(!XxxSysEnterExW)
	{
		if(xic->flags & ZSOPT_IPC)
			stub_name = IPC_STUB_FILE_NAME;

		#ifdef _STRSAFE_H_INCLUDED_
		StringCbCopyW(StubPath, sizeof(StubPath), XigncodeDir);
		StringCbCatW(StubPath, sizeof(StubPath), stub_name);
		#else
		lstrcpyW(StubPath, XigncodeDir);
		lstrcatW(StubPath, stub_name);
		#endif
	
#ifdef XIGNCODE_STATIC_LINK

		if(!LinkThunk(xic->flags))
			return X_FALSE;

#else

		PVOID Module = XLoadLibraryW(StubPath);
		if(!Module)
		{
			return X_FALSE;
		}

#endif

		if(!XxxGetProcAddress((xpvoid *)&XxxSysEnterExW, XclioFidSysEnterExW))
		{
			return X_FALSE;
		}
	}

	if(!XxxSysEnterExW)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_SYSENTEREXW);
		return X_FALSE;
	}

	if(!XxxSysEnterExW(xic))
	{
		return X_FALSE;
	}

	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_LOAD_LIBRARYA, &XxxFastLoadLibraryAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_LOAD_LIBRARYW, &XxxFastLoadLibraryWWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FREE_LIBRARY, &XxxFastFreeLibraryWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GET_PROC_ADDRESS, &XxxFastGetProcAddressWorker);

	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_CREATE_FILE_A, &XxxFastCreateFileAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_CREATE_FILE_W, &XxxFastCreateFileWWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_READ_FILE, &XxxFastReadFileWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_WRITE_FILE, &XxxFastWriteFileWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SET_FILE_POINTER, &XxxFastSetFilePointerWorker);

	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_MALLOC, &XxxFastMallocWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FREE, &XxxFastFreeWorker);

	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FOPEN, &XxxFastFopenWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FCLOSE, &XxxFastFcloseWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FREAD, &XxxFastFreadWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FWRITE, &XxxFastFwriteWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FSEEK, &XxxFastFseekWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FTELL, &XxxFastFtellWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFSCANFS, &XxxFastFscanfsWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFWSCANFS, &XxxFastFwscanfsWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FOPENS, &XxxFastFopensWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_WFOPENS, &XxxFastWfopensWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFPRINTF, &XxxFastFprintfWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFWPRINTF, &XxxFastFwprintfWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_A, &XxxFastShellExecuteAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_W, &XxxFastShellExecuteWWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_EX_A, &XxxFastShellExecuteExAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_EX_W, &XxxFastShellExecuteExWWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETTICKCOUNT, &XxxFastGetTickCountWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETTICKCOUNT64, &XxxFastGetTickCount64Worker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETVOLUMEINFORMATION_A, &XxxFastGetVolumeInformationAWorker);
	ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETVOLUMEINFORMATION_W, &XxxFastGetVolumeInformationWWorker);
	
	return X_TRUE;
}


XINLINE
xbool
XCALL
ZCWAVE_SysEnterA(xcstr License, xcstr Path, xulong Flags)
{
	xwch wLicense[80];
	xwch wPathBuffer[MAX_PATH];
	xwstr wPath = X_NULL;

	MultiByteToWideChar(CP_ACP, 0, License, -1, wLicense, XArraySize(wLicense));

	if(Path)
	{
		MultiByteToWideChar(CP_ACP, 0, Path, -1, wPathBuffer, XArraySize(wPathBuffer));
		wPath = wPathBuffer;
	}

	return ZCWAVE_SysEnterW(wLicense, wPath, Flags);
}


XINLINE
xbool
XCALL
ZCWAVE_SysExit()
{
	if(!XxxSysExit)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxSysExit, XclioFidSysExit))
			return X_FALSE;
	}

	if(!XxxSysExit)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_SYSEXIT);
		return X_FALSE;
	}

	// Not calling FreeLibrary is intended.

	return XxxSysExit();
}



XINLINE
xbool
XCALL
ZCWAVE_RegisterCallback(XigncodeCallback Callback, xpvoid Context)
{
	if(!XxxRegisterCallback)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRegisterCallback, XclioFidRegisterCallback))
			return X_FALSE;
	}

	if(!XxxRegisterCallback)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_REGISTERCALLBACK);
		return X_FALSE;
	}

	XxxRegisterCallback(Callback, Context);
	return X_TRUE;
}

XINLINE
xbool
XCALL
ZCWAVE_ProbeCodeBoxEx(xhandle CodeBox, xpcch ReceivedPacket, xsize ReplySize, ProbeCallback Callback, xpvoid Context)
{
	if(!XxxProbeCodeBoxEx)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxProbeCodeBoxEx, XclioFidProbeCodeBoxEx))
			return X_FALSE;
	}

	if(!XxxProbeCodeBoxEx)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_PROBECODEBOXEX);
		return X_FALSE;
	}

	return XxxProbeCodeBoxEx(CodeBox, ReceivedPacket, ReplySize, Callback, Context);
}

XINLINE
xbool
XCALL
ZCWAVE_ProbeEx(xpcch ReceivedPacket, xsize ReplySize, ProbeCallback Callback, xpvoid Context)
{
	if(!XxxProbeEx)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxProbeEx, XclioFidProbeEx))
			return X_FALSE;
	}

	if(!XxxProbeEx)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_PROBEEX);
		return X_FALSE;
	}

	return XxxProbeEx(ReceivedPacket, ReplySize, Callback, Context);
}

XINLINE
xbool
XCALL
ZCWAVE_Encrypt
(
	xpcuch Input
	, xsize InputSize
	, xpcuch Key
	, xsize KeySize
	, xpuch Output
	, xsize OutputSize
	, xsize *Required
	, int Type
)
{
	if(!XxxEncrypt)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxEncrypt, XclioFidEncrypt))
			return X_FALSE;
	}

	if(!XxxEncrypt)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_ENCRYPT);
		return X_FALSE;	
	}

	return XxxEncrypt(Input, InputSize, Key, KeySize, Output, OutputSize, Required, Type);
}

XINLINE
xbool
XCALL
ZCWAVE_Decrypt
(
	xpcuch Input
	, xsize InputSize
	, xpcuch Key
	, xsize KeySize
	, xpuch Output
	, xsize OutputSize
	, xsize *Required
	, int Type
)
{
	if(!XxxDecrypt)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxDecrypt, XclioFidDecrypt))
			return X_FALSE;
	}

	if(!XxxDecrypt)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_DECRYPT);
		return X_FALSE;
	}

	return XxxDecrypt(Input, InputSize, Key, KeySize, Output, OutputSize, Required, Type);

}

XINLINE
xbool
XCALL
ZCWAVE_RsaGenerateKey
(
	xpuch PublicKey
	, xsize PublicKeySize
	, xpuch PrivateKey
	, xsize PrivateKeySize
	, int Nbits
	, int Exponent	
)
{
	if(!XxxRsaGenerateKey)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaGenerateKey, XclioFidRsaGenerateKey))
			return X_FALSE;
	}

	if(!XxxRsaGenerateKey)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSAGENERATEKEY);
		return X_FALSE;
	}

	return XxxRsaGenerateKey(PublicKey, PublicKeySize, PrivateKey, PrivateKeySize, Nbits, Exponent);

}

XINLINE
xbool
XCALL
ZCWAVE_RsaSetPrivateKey(xhandle H, xpcuch Key, xsize KeySize)
{
	if(!XxxRsaSetPrivateKey)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaSetPrivateKey, XclioFidRsaSetPrivateKey))
			return X_FALSE;
	}

	if(!XxxRsaSetPrivateKey)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSASETPRIVATEKEY);
		return X_FALSE;
	}

	return XxxRsaSetPrivateKey(H, Key, KeySize);

}

XINLINE
xbool
XCALL
ZCWAVE_RsaSetPublicKey(xhandle H, xpcuch Key, xsize KeySize)
{
	if(!XxxRsaSetPublicKey)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaSetPublicKey, XclioFidRsaSetPublicKey))
			return X_FALSE;
	}

	if(!XxxRsaSetPublicKey)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSASETPUBLICKEY);
		return X_FALSE;
	}

	return XxxRsaSetPublicKey(H, Key, KeySize);
}

XINLINE
xbool
XCALL
ZCWAVE_RsaPrivateEncrypt
(
	xhandle H
	, xpcuch Input
	, xsize InputSize
	, xpuch *Output
	, xsize *OutputSize
)
{
	if(!XxxRsaPrivateEncrypt)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaPrivateEncrypt, XclioFidRsaPrivateEncrypt))
			return X_FALSE;
	}

	if(!XxxRsaPrivateEncrypt)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSAPRIVATEENCRYPT);
		return X_FALSE;
	}

	return XxxRsaPrivateEncrypt(H, Input, InputSize, Output, OutputSize);
}

XINLINE
xbool
XCALL
ZCWAVE_RsaPrivateDecrypt
(
	xhandle H
	, xpcuch Input
	, xsize InputSize
	, xpuch *Output
	, xsize *OutputSize
)
{
	if(!XxxRsaPrivateDecrypt)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaPrivateDecrypt, XclioFidRsaPrivateDecrypt))
			return X_FALSE;
	}

	if(!XxxRsaPrivateDecrypt)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSAPRIVATEDESCRYPT);
		return X_FALSE;
	}

	return XxxRsaPrivateDecrypt(H, Input, InputSize, Output, OutputSize);
}

XINLINE
xbool
XCALL
ZCWAVE_RsaPublicEncrypt
(
	xhandle H
	, xpcuch Input
	, xsize InputSize
	, xpuch *Output
	, xsize *OutputSize
)
{
	if(!XxxRsaPublicEncrypt)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaPublicEncrypt, XclioFidRsaPublicEncrypt))
			return X_FALSE;
	}

	if(!XxxRsaPublicEncrypt)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSAPUBLICENCRYPT);
		return X_FALSE;
	}

	return XxxRsaPublicEncrypt(H, Input, InputSize, Output, OutputSize);
}

XINLINE
xbool
XCALL
ZCWAVE_RsaPublicDecrypt
(
	xhandle H
	, xpcuch Input
	, xsize InputSize
	, xpuch *Output
	, xsize *OutputSize
)
{
	if(!XxxRsaPublicDecrypt)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaPublicDecrypt, XclioFidRsaPublicDecrypt))
			return X_FALSE;
	}

	if(!XxxRsaPublicDecrypt)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSAPUBLICDECRYPT);
		return X_FALSE;
	}

	return XxxRsaPublicDecrypt(H, Input, InputSize, Output, OutputSize);
}

XINLINE
xbool
XCALL
ZCWAVE_RsaClose(xhandle H)
{
	if(!XxxRsaClose)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaClose, XclioFidRsaClose))
			return X_FALSE;
	}

	if(!XxxRsaClose)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSACLOSE);
		return X_FALSE;
	}

	return XxxRsaClose(H);
}

XINLINE
xhandle 
XCALL 
ZCWAVE_RsaCreate()
{
	if(!XxxRsaCreate)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaCreate, XclioFidRsaCreate))
			return X_FALSE;
	}

	if(!XxxRsaCreate)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSACREATE);
		return X_FALSE;
	}

	return XxxRsaCreate();
}

XINLINE
xbool
XCALL
ZCWAVE_RsaFreeBuffer(xhandle H, xpuch Buffer)
{
	if(!XxxRsaFreeBuffer)
	{
		if(!XxxGetProcAddress((xpvoid *)&XxxRsaFreeBuffer, XclioFidRsaFreeBuffer))
			return X_FALSE;
	}

	if(!XxxRsaFreeBuffer)
	{
		SetLastError(ZCWAVE_E_CANT_RESOLVE_RSAFREEBUFFER);
		return X_FALSE;
	}

	return XxxRsaFreeBuffer(H, Buffer);
}

#ifdef _UNICODE
#define ZCWAVE_SysEnter ZCWAVE_SysEnterW
#else
#define ZCWAVE_SysEnter ZCWAVE_SysEnterA
#endif

typedef xstatus (__cdecl *CallGateT)(va_list);

#define X_PREPARE_SEED(rtn);												\
			xulong in_seed = GetTickCount();								\
			xulong out_seed = 0;											\
			xulong mid_seed = 0;											\
			if(!ZCWAVE_SendCommand(ZCMD_GET_NEXT_SEED, in_seed, &out_seed))	\
			{																\
				SetLastError(X_E_UNSUCCESSFUL);								\
				return (rtn);												\
			}																\
			mid_seed = out_seed;


#define X_CHECK_SEED(rtn);													\
			if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, mid_seed, out_seed))	\
				return (rtn);

XINLINE
HMODULE
XCALL
XxxLoadLibraryA(xcstr path)
{
	HMODULE module;
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_LOAD_LIBRARYA, &module, path, in_seed, &out_seed))
		return NULL;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return NULL;
		
	return module;
}

XINLINE
HMODULE
XCALL
XxxLoadLibraryW(xcwstr path)
{
	HMODULE module;
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_LOAD_LIBRARYW, &module, path, in_seed, &out_seed))
		return NULL;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return NULL;

	return module;
}

XINLINE
xbool
XCALL
XxxFreeLibrary(HMODULE module)
{
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_FREE_LIBRARY, module, in_seed, &out_seed))
		return X_FALSE;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return X_FALSE;
	return X_TRUE;
}


XINLINE
FARPROC
XCALL
XxxGetProcAddress(HMODULE module, xcstr name)
{
	FARPROC fn;
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_GET_PROC_ADDRESS, &fn, module, name, in_seed, &out_seed))
		return NULL;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return NULL;

	return fn;
}

XINLINE
HANDLE
XCALL
XxxCreateFileA(LPCSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6)
{
	HANDLE rtn;
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_CREATE_FILE_A, &rtn, a0, a1, a2, a3, a4, a5, a6, in_seed, &out_seed))
		return NULL;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return NULL;

	return rtn;
}

XINLINE
HANDLE
XCALL
XxxCreateFileW(LPCWSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6)
{
	HANDLE rtn;
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_CREATE_FILE_W, &rtn, a0, a1, a2, a3, a4, a5, a6, in_seed, &out_seed))
		return NULL;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return NULL;

	return rtn;
}

XINLINE
BOOL
XCALL
XxxReadFile(HANDLE a0, LPVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4)
{
	BOOL rtn;
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_READ_FILE, &rtn, a0, a1, a2, a3, a4, in_seed, &out_seed))
		return NULL;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return NULL;

	return rtn;
}

XINLINE
BOOL
XCALL
XxxWriteFile(HANDLE a0, LPCVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4)
{
	BOOL rtn;
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_WRITE_FILE, &rtn, a0, a1, a2, a3, a4, in_seed, &out_seed))
		return NULL;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return NULL;

	return rtn;
}

XINLINE
DWORD
XCALL
XxxSetFilePointer(HANDLE a0, LONG a1, PLONG a2, DWORD a3)
{
	DWORD rtn;
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_SET_FILE_POINTER, &rtn, a0, a1, a2, a3, in_seed, &out_seed))
		return NULL;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return NULL;

	return rtn;
}

XINLINE
FARPROC
XCALL
XxxMalloc(xsize size)
{
	xpvoid ptr;
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_MALLOC, &ptr, size, in_seed, &out_seed))
		return NULL;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return NULL;

	return (FARPROC) ptr;
}

XINLINE
xbool
XCALL
XxxFree(xpvoid ptr)
{
	xulong out_seed;
	xulong in_seed = GetTickCount();
	
	if(!ZCWAVE_SendCommand(ZCMD_FREE, ptr, in_seed, &out_seed))
		return X_FALSE;

	if(!ZCWAVE_SendCommand(ZCMD_CHECK_SEED, in_seed, out_seed))
		return X_FALSE;

	return X_TRUE;
}

XINLINE
xbool
XCALL
FreeStub(xpvoid ptr)
{
	free(ptr);
	return X_TRUE;
}

XINLINE
FILE *
XCALL
XxxFopen(xcstr path, xcstr mode)
{
	FILE *result;

	X_PREPARE_SEED(NULL);
	
	if(!ZCWAVE_SendCommand(ZCMD_FOPEN, &result, path, mode, in_seed, &out_seed))
		return NULL;

	X_CHECK_SEED(NULL);
	
	return result;
}

XINLINE
xint
XCALL
XxxFclose(FILE *file)
{
	xint result = EOF;
	
	X_PREPARE_SEED(EOF);

	if(!ZCWAVE_SendCommand(ZCMD_FCLOSE, &result, file, in_seed, &out_seed))
		return EOF;

	X_CHECK_SEED(EOF);
		
	return result;
}

XINLINE
xsize
XCALL
XxxFread(xpvoid buffer, xsize size, xsize count, FILE *file)
{
	xsize result = 0;
	
	X_PREPARE_SEED(0);

	if(!ZCWAVE_SendCommand(ZCMD_FREAD, &result, buffer, size, count, file, in_seed, &out_seed))
		return 0;

	X_CHECK_SEED(0);

	return result;
}

XINLINE
xsize
XCALL
XxxFwrite(xpcvoid buffer, xsize size, xsize count, FILE *file)
{
	xsize result = 0;
	
	X_PREPARE_SEED(0);

	if(!ZCWAVE_SendCommand(ZCMD_FWRITE, &result, buffer, size, count, file, in_seed, &out_seed))
		return 0;

	X_CHECK_SEED(0);
	
	return result;
}

XINLINE
xint
XCALL
XxxFseek(FILE *file, xlong offset, xint origin)
{
	xint result = -2;
	
	X_PREPARE_SEED(-2);

	if(!ZCWAVE_SendCommand(ZCMD_FSEEK, &result, file, offset, origin, in_seed, &out_seed))
		return -2;

	X_CHECK_SEED(-2);

	return result;
}

XINLINE
xlong
XCALL
XxxFtell(FILE *file)
{
	xlong result = -1L;
	
	X_PREPARE_SEED(-1L);

	if(!ZCWAVE_SendCommand(ZCMD_FTELL, &result, file, in_seed, &out_seed))
		return -1L;

	X_CHECK_SEED(-1L);

	return result;
}

XINLINE
int
XCALL
XxxFscanfs(FILE *a0, const char *a1, ...)
{
	int result = -1L;
	va_list ap;
	
	X_PREPARE_SEED(-1L);

	va_start(ap, a1);
	if(!ZCWAVE_SendCommand(ZCMD_VFSCANFS, &result, a0, a1, ap, in_seed, &out_seed))
		return -1L;
	va_end(ap);

	X_CHECK_SEED(-1L);

	return result;
}

XINLINE
int
XCALL
XxxFwscanfs(FILE *a0, const wchar_t *a1, ...)
{
	int result = -1L;
	va_list ap;
	
	X_PREPARE_SEED(-1L);

	va_start(ap, a1);
	if(!ZCWAVE_SendCommand(ZCMD_VFWSCANFS, &result, a0, a1, ap, in_seed, &out_seed))
		return -1L;
	va_end(ap);

	X_CHECK_SEED(-1L);

	return result;
}

XINLINE
errno_t
XCALL
XxxFopens(FILE** a0, const char *a1, const char *a2)
{
	errno_t result = -1L;
	
	X_PREPARE_SEED(-1L);

	if(!ZCWAVE_SendCommand(ZCMD_FOPENS, &result, a0, a1, a2, in_seed, &out_seed))
		return -1L;

	X_CHECK_SEED(-1L);

	return result;
}

XINLINE
errno_t
XCALL
XxxWfopens(FILE** a0, const wchar_t *a1, const wchar_t *a2)
{
	errno_t result = -1L;
	
	X_PREPARE_SEED(-1L);

	if(!ZCWAVE_SendCommand(ZCMD_WFOPENS, &result, a0, a1, a2, in_seed, &out_seed))
		return -1L;

	X_CHECK_SEED(-1L);

	return result;

}

XINLINE
int
XCALL
XxxFprintf(FILE* a0, const char *a1, ...)
{
	int result = -1L;
	va_list ap;
	
	X_PREPARE_SEED(-1L);

	va_start(ap, a1);
	if(!ZCWAVE_SendCommand(ZCMD_VFPRINTF, &result, a0, a1, ap, in_seed, &out_seed))
		return -1L;
	va_end(ap);

	X_CHECK_SEED(-1L);

	return result;
}

XINLINE
int
XCALL
XxxFwprintf(FILE* a0, const wchar_t *a1, ...)
{
	int result = -1L;
	va_list ap;
	
	X_PREPARE_SEED(-1L);

	va_start(ap, a1);
	if(!ZCWAVE_SendCommand(ZCMD_VFWPRINTF, &result, a0, a1, ap, in_seed, &out_seed))
		return -1L;
	va_end(ap);

	X_CHECK_SEED(-1L);

	return result;
}

XINLINE
HINSTANCE
XCALL
XxxShellExecuteA(HWND a0, xcstr a1, xcstr a2, xcstr a3, xcstr a4, xint a5)
{
	HINSTANCE result;

	X_PREPARE_SEED(X_NULL);

	if(!ZCWAVE_SendCommand(ZCMD_SHELLEXECUTE_A, &result, a0, a1, a2, a3, a4, a5, in_seed, &out_seed))	
		return X_NULL;
	
	X_CHECK_SEED(X_NULL);

	return result;
}

XINLINE
HINSTANCE
XCALL
XxxShellExecuteW(HWND a0, xcwstr a1, xcwstr a2, xcwstr a3, xcwstr a4, xint a5)
{
	HINSTANCE result;

	X_PREPARE_SEED(X_NULL);

	if(!ZCWAVE_SendCommand(ZCMD_SHELLEXECUTE_W, &result, a0, a1, a2, a3, a4, a5, in_seed, &out_seed))	
		return X_NULL;
	
	X_CHECK_SEED(X_NULL);

	return result;
}

XINLINE
xbool
XCALL
XxxShellExecuteExA(LPSHELLEXECUTEINFOA a0)
{
	xbool result = X_FALSE;

	X_PREPARE_SEED(X_FALSE);

	if(!ZCWAVE_SendCommand(ZCMD_SHELLEXECUTE_EX_A, &result, a0, in_seed, &out_seed))
		return X_FALSE;
	
	X_CHECK_SEED(X_FALSE);
	
	return result;
}

XINLINE
xbool
XCALL
XxxShellExecuteExW(LPSHELLEXECUTEINFOW a0)
{
	xbool result = X_FALSE;

	X_PREPARE_SEED(X_FALSE);

	if(!ZCWAVE_SendCommand(ZCMD_SHELLEXECUTE_EX_W, &result, a0, in_seed, &out_seed))
		return X_FALSE;
	
	X_CHECK_SEED(X_FALSE);
	
	return result;
}

XINLINE
xulong
XCALL
XxxGetTickCount(xvoid)
{
	xulong result = 0;
	
	X_PREPARE_SEED(0);

	if(!ZCWAVE_SendCommand(ZCMD_GETTICKCOUNT, &result, in_seed, &out_seed))
		return 0;

	X_CHECK_SEED(0);

	return result;
}

XINLINE
xuint64
XCALL
XxxGetTickCount64(xvoid)
{
	xuint64 result = 0;
	
	X_PREPARE_SEED(0);

	if(!ZCWAVE_SendCommand(ZCMD_GETTICKCOUNT64, &result, in_seed, &out_seed))
		return 0;

	X_CHECK_SEED(0);

	return result;
}

XINLINE
xbool
XCALL
XxxGetVolumeInformationA(xcstr a0, xstr a1, xulong a2, xulong *a3, xulong *a4, xulong *a5, xstr a6, xulong a7)
{
	xbool result = X_FALSE;

	X_PREPARE_SEED(X_FALSE);
	
	if(!ZCWAVE_SendCommand(ZCMD_GETVOLUMEINFORMATION_A, &result, a0, a1, a2, a3, a4, a5, a6, a7, in_seed, &out_seed))
		return X_FALSE;

	X_CHECK_SEED(X_FALSE);

	return result;
}

XINLINE
xbool
XCALL
XxxGetVolumeInformationW(xcwstr a0, xwstr a1, xulong a2, xulong *a3, xulong *a4, xulong *a5, xwstr a6, xulong a7)
{
	xbool result = X_FALSE;

	X_PREPARE_SEED(X_FALSE);

	if(!ZCWAVE_SendCommand(ZCMD_GETVOLUMEINFORMATION_W, &result, a0, a1, a2, a3, a4, a5, a6, a7, in_seed, &out_seed))
		return X_FALSE;

	X_CHECK_SEED(X_FALSE);

	return result;
}

// FAST Stub

XINLINE
HMODULE
XCALL
XxxFastLoadLibraryA(xcstr a0)
{
	if(!XxxFastLoadLibraryAWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_LOAD_LIBRARYA, &XxxFastLoadLibraryAWorker);
	
	return XxxFastLoadLibraryAWorker(a0);
}

XINLINE
HMODULE
XCALL
XxxFastLoadLibraryW(xcwstr a0)
{
	if(!XxxFastLoadLibraryWWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_LOAD_LIBRARYW, &XxxFastLoadLibraryWWorker);

	return XxxFastLoadLibraryWWorker(a0);
}

XINLINE
xbool
XCALL
XxxFastFreeLibrary(HMODULE a0)
{
	if(!XxxFastFreeLibraryWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FREE_LIBRARY, &XxxFastFreeLibraryWorker);

	return XxxFastFreeLibraryWorker(a0);
}

XINLINE
FARPROC
XCALL
XxxFastGetProcAddress(HMODULE a0, xcstr a1)
{
	if(!XxxFastGetProcAddressWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GET_PROC_ADDRESS, &XxxFastGetProcAddressWorker);

	return XxxFastGetProcAddressWorker(a0, a1);
}

XINLINE
HANDLE
XCALL
XxxFastCreateFileA(LPCSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6)
{
	if(!XxxFastCreateFileAWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_CREATE_FILE_A, &XxxFastCreateFileAWorker);

	return XxxFastCreateFileAWorker(a0, a1, a2, a3, a4, a5, a6);
}

XINLINE
HANDLE
XCALL
XxxFastCreateFileW(LPCWSTR a0, DWORD a1, DWORD a2, LPSECURITY_ATTRIBUTES a3, DWORD a4, DWORD a5, HANDLE a6)
{
	if(!XxxFastCreateFileWWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_CREATE_FILE_W, &XxxFastCreateFileWWorker);

	return XxxFastCreateFileWWorker(a0, a1, a2, a3, a4, a5, a6);
}

XINLINE
BOOL
XCALL
XxxFastReadFile(HANDLE a0, LPVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4)
{
	if(!XxxFastReadFileWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_READ_FILE, &XxxFastReadFileWorker);

	return XxxFastReadFileWorker(a0, a1, a2, a3, a4);
}

XINLINE
BOOL
XCALL
XxxFastWriteFile(HANDLE a0, LPCVOID a1, DWORD a2, LPDWORD a3, LPOVERLAPPED a4)
{
	if(!XxxFastWriteFileWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_WRITE_FILE, &XxxFastWriteFileWorker);

	return XxxFastWriteFileWorker(a0, a1, a2, a3, a4);
}

XINLINE
DWORD
XCALL
XxxFastSetFilePointer(HANDLE a0, LONG a1, PLONG a2, DWORD a3)
{
	if(!XxxFastSetFilePointerWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SET_FILE_POINTER, &XxxFastSetFilePointerWorker);

	return XxxFastSetFilePointerWorker(a0, a1, a2, a3);
}


XINLINE
xpvoid
XCALL
XxxFastMalloc(xsize a0)
{
	if(!XxxFastMallocWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_MALLOC, &XxxFastMallocWorker);

	return XxxFastMallocWorker(a0);
}

XINLINE
xbool
XCALL
XxxFastFree(xpvoid a0)
{
	if(!XxxFastFreeWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FREE, &XxxFastFreeWorker);

	return XxxFastFreeWorker(a0);
}

XINLINE
FILE *
XCALL
XxxFastFopen(xcstr a0, xcstr a1)
{
	if(!XxxFastFopenWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FOPEN, &XxxFastFopenWorker);

	return XxxFastFopenWorker(a0, a1);
}

XINLINE
xbool
XCALL
XxxFastFclose(FILE *a0)
{
	if(!XxxFastFcloseWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FCLOSE, &XxxFastFcloseWorker);

	return XxxFastFcloseWorker(a0);
}

XINLINE
xsize
XCALL
XxxFastFread(xpvoid a0, xsize a1, xsize a2, FILE *a3)
{
	if(!XxxFastFreadWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FREAD, &XxxFastFreadWorker);

	return XxxFastFreadWorker(a0, a1, a2, a3);
}

XINLINE
xsize
XCALL
XxxFastFwrite(xpcvoid a0, xsize a1, xsize a2, FILE *a3)
{
	if(!XxxFastFwriteWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FWRITE, &XxxFastFwriteWorker);

	return XxxFastFwriteWorker(a0, a1, a2, a3);
}

XINLINE
xint
XCALL
XxxFastFseek(FILE *a0, xlong a1, xint a2)
{
	if(!XxxFastFseekWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FSEEK, &XxxFastFseekWorker);

	return XxxFastFseekWorker(a0, a1, a2);
}

XINLINE
xlong
XCALL
XxxFastFtell(FILE *a0)
{
	if(!XxxFastFtellWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FTELL, &XxxFastFtellWorker);

	return XxxFastFtellWorker(a0);
}

XINLINE
int
XCALL
XxxFastFscanfs(FILE *a0, const char *a1, ...)
{
	va_list ap;
	int rtn;

	if(!XxxFastFscanfsWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFSCANFS, &XxxFastFscanfsWorker);

	va_start(ap, a1);
	rtn = XxxFastFscanfsWorker(a0, a1, ap);
	va_end(ap);

	return rtn;
}

XINLINE
int
XCALL
XxxFastFwscanfs(FILE *a0, const wchar_t *a1, ...)
{
	va_list ap;
	int rtn;

	if(!XxxFastFwscanfsWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFWSCANFS, &XxxFastFwscanfsWorker);

	va_start(ap, a1);
	rtn = XxxFastFwscanfsWorker(a0, a1, ap);
	va_end(ap);

	return rtn;
}

XINLINE
errno_t
XCALL
XxxFastFopens(FILE** a0, const char *a1, const char *a2)
{
	if(!XxxFastFopensWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_FOPENS, &XxxFastFopensWorker);

	return XxxFastFopensWorker(a0, a1, a2);
}

XINLINE
errno_t
XCALL
XxxFastWfopens(FILE** a0, const wchar_t *a1, const wchar_t *a2)
{
	if(!XxxFastWfopensWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_WFOPENS, &XxxFastWfopensWorker);

	return XxxFastWfopensWorker(a0, a1, a2);

}

XINLINE
int
XCALL
XxxFastFprintf(FILE *a0, const char *a1, ...)
{
	va_list ap;
	int rtn;

	if(!XxxFastFprintfWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFPRINTF, &XxxFastFprintfWorker);

	va_start(ap, a1);
	rtn = XxxFastFprintfWorker(a0, a1, ap);
	va_end(ap);

	return rtn;
}

XINLINE
int
XCALL
XxxFastFwprintf(FILE *a0, const wchar_t *a1, ...)
{
	va_list ap;
	int rtn;

	if(!XxxFastFwprintfWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_VFWPRINTF, &XxxFastFwprintfWorker);

	va_start(ap, a1);
	rtn = XxxFastFwprintfWorker(a0, a1, ap);
	va_end(ap);

	return rtn;
}

XINLINE
HINSTANCE
XCALL
XxxFastShellExecuteA(HWND a0, xcstr a1, xcstr a2, xcstr a3, xcstr a4, xint a5)
{
	HINSTANCE rtn;

	if(!XxxFastShellExecuteAWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_A, &XxxFastShellExecuteAWorker);

	rtn = XxxFastShellExecuteAWorker(a0, a1, a2, a3, a4, a5);

	return rtn;
}

XINLINE
HINSTANCE
XCALL
XxxFastShellExecuteW(HWND a0, xcwstr a1, xcwstr a2, xcwstr a3, xcwstr a4, xint a5)
{
	HINSTANCE rtn;

	if(!XxxFastShellExecuteWWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_W, &XxxFastShellExecuteWWorker);

	rtn = XxxFastShellExecuteWWorker(a0, a1, a2, a3, a4, a5);

	return rtn;
}

XINLINE
xbool
XCALL
XxxFastShellExecuteExA(LPSHELLEXECUTEINFOA a0)
{
	xbool rtn;

	if(!XxxFastShellExecuteExAWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_EX_A, &XxxFastShellExecuteExAWorker);

	rtn = XxxFastShellExecuteExAWorker(a0);

	return rtn;
}

XINLINE
xbool
XCALL
XxxFastShellExecuteExW(LPSHELLEXECUTEINFOW a0)
{
	xbool rtn;

	if(!XxxFastShellExecuteExWWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_SHELLEXECUTE_EX_W, &XxxFastShellExecuteExWWorker);

	rtn = XxxFastShellExecuteExWWorker(a0);

	return rtn;
}

XINLINE
xulong
XCALL
XxxFastGetTickCount(xvoid)
{
	xulong rtn;

	if(!XxxFastGetTickCountWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETTICKCOUNT, &XxxFastGetTickCountWorker);

	rtn = XxxFastGetTickCountWorker();

	return rtn;
}

XINLINE
xuint64
XCALL
XxxFastGetTickCount64(xvoid)
{
	xuint64 rtn;

	if(!XxxFastGetTickCount64Worker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETTICKCOUNT64, &XxxFastGetTickCount64Worker);

	rtn = XxxFastGetTickCount64Worker();

	return rtn;
}

XINLINE
xbool
XCALL 
XxxFastGetVolumeInformationA(xcstr a0, xstr a1, xulong a2, xulong *a3, xulong *a4, xulong *a5, xstr a6, xulong a7)
{
	xbool rtn;

	if(!XxxFastGetVolumeInformationAWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETVOLUMEINFORMATION_A, &XxxFastGetVolumeInformationAWorker);

	rtn = XxxFastGetVolumeInformationAWorker(a0, a1, a2, a3, a4, a5, a6, a7);

	return rtn;
}

XINLINE
xbool
XCALL 
XxxFastGetVolumeInformationW(xcwstr a0, xwstr a1, xulong a2, xulong *a3, xulong *a4, xulong *a5, xwstr a6, xulong a7)
{
	xbool rtn;

	if(!XxxFastGetVolumeInformationWWorker)
		ZCWAVE_SendCommand(ZCMD_GET_FAST_SAPI_ADDRESS, ZCMD_GETVOLUMEINFORMATION_W, &XxxFastGetVolumeInformationWWorker);

	rtn = XxxFastGetVolumeInformationWWorker(a0, a1, a2, a3, a4, a5, a6, a7);

	return rtn;
}

#if defined(USE_XIGNCODE3_SECURE_API)

#define ZCWAVE_LoadLibraryA					XxxLoadLibraryA
#define ZCWAVE_LoadLibraryW					XxxLoadLibraryW
#define ZCWAVE_FreeLibrary					XxxFreeLibrary
#define ZCWAVE_GetProcAddress				XxxGetProcAddress
#define ZCWAVE_CreateFileA					XxxCreateFileA
#define ZCWAVE_CreateFileW					XxxCreateFileW
#define ZCWAVE_ReadFile						XxxReadFile
#define ZCWAVE_WriteFile					XxxWriteFile
#define ZCWAVE_SetFilePointer				XxxSetFilePointer

#define ZCWAVE_Malloc						XxxMalloc
#define ZCWAVE_Free							XxxFree
#define ZCWAVE_malloc						XxxMalloc
#define ZCWAVE_free							XxxFree
#define ZCWAVE_fopen						XxxFopen
#define ZCWAVE_fclose						XxxFclose
#define ZCWAVE_fread						XxxFread
#define ZCWAVE_fwrite						XxxFwrite
#define ZCWAVE_fseek						XxxFseek
#define ZCWAVE_ftell						XxxFtell
#define ZCWAVE_fscanf_s						XxxFscanfs
#define ZCWAVE_fwscanf_s					XxxFwscanfs
#define ZCWAVE_fopen_s						XxxFopens
#define ZCWAVE_wfopen_s						XxxWfopens
#define ZCWAVE_fprintf						XxxFprintf
#define ZCWAVE_fwprintf						XxxFwprintf
#define ZCWAVE_ShellExecuteA				XxxShellExecuteA
#define ZCWAVE_ShellExecuteW				XxxShellExecuteW
#define ZCWAVE_ShellExecuteExA				XxxShellExecuteExA
#define ZCWAVE_ShellExecuteExW				XxxShellExecuteExW
#define ZCWAVE_GetTickCount					XxxGetTickCount
#define ZCWAVE_GetTickCount64				XxxGetTickCount64
#define ZCWAVE_GetVolumeInformationA		XxxGetVolumeInformationA
#define ZCWAVE_GetVolumeInformationW		XxxGetVolumeInformationW

// FAST

#define ZCWAVE_Fast_LoadLibraryA			XxxFastLoadLibraryA
#define ZCWAVE_Fast_LoadLibraryW			XxxFastLoadLibraryW
#define ZCWAVE_Fast_FreeLibrary				XxxFastFreeLibrary
#define ZCWAVE_Fast_GetProcAddress			XxxFastGetProcAddress
#define ZCWAVE_Fast_CreateFileA				XxxFastCreateFileA
#define ZCWAVE_Fast_CreateFileW				XxxFastCreateFileW
#define ZCWAVE_FastReadFile					XxxFastReadFile
#define ZCWAVE_FastWriteFile				XxxFastWriteFile
#define ZCWAVE_Fast_SetFilePointer			XxxFastSetFilePointer

#define ZCWAVE_Fast_malloc					XxxFastMalloc
#define ZCWAVE_Fast_free					XxxFastFree
#define ZCWAVE_Fast_fopen					XxxFastFopen
#define ZCWAVE_Fast_fclose					XxxFastFclose
#define ZCWAVE_Fast_fread					XxxFastFread
#define ZCWAVE_Fast_fwrite					XxxFastFwrite
#define ZCWAVE_Fast_fseek					XxxFastFseek
#define ZCWAVE_Fast_ftell					XxxFastFtell
#define ZCWAVE_Fast_fscanf_s				XxxFastFscanfs
#define ZCWAVE_Fast_fwscanf_s				XxxFastFwscanfs
#define ZCWAVE_Fast_fopen_s					XxxFastFopens
#define ZCWAVE_Fast_wfopen_s				XxxFastWfopens
#define ZCWAVE_Fast_fprintf					XxxFastFprintf
#define ZCWAVE_Fast_fwprintf				XxxFastFwprintf
#define ZCWAVE_Fast_ShellExecuteA			XxxFastShellExecuteA
#define ZCWAVE_Fast_ShellExecuteW			XxxFastShellExecuteW
#define ZCWAVE_Fast_ShellExecuteExA			XxxFastShellExecuteExA
#define ZCWAVE_Fast_ShellExecuteExW			XxxFastShellExecuteExW
#define ZCWAVE_Fast_GetTickCount			XxxFastGetTickCount
#define ZCWAVE_Fast_GetTickCount64			XxxFastGetTickCount64
#define ZCWAVE_Fast_GetVolumeInformationA	XxxFastGetVolumeInformationA
#define ZCWAVE_Fast_GetVolumeInformationW	XxxFastGetVolumeInformationW

#else

#define ZCWAVE_LoadLibraryA					LoadLibraryA
#define ZCWAVE_LoadLibraryW					LoadLibraryW
#define ZCWAVE_FreeLibrary					FreeLibrary
#define ZCWAVE_GetProcAddress				GetProcAddress
#define ZCWAVE_CreateFileA					CreateFileA
#define ZCWAVE_CreateFileW					CreateFileW
#define ZCWAVE_ReadFile						ReadFile
#define ZCWAVE_WriteFile					WriteFile
#define ZCWAVE_SetFilePointer				SetFilePointer

#define ZCWAVE_Malloc						malloc
#define ZCWAVE_Free							FreeStub
#define ZCWAVE_malloc						malloc
#define ZCWAVE_free							FreeStub
#define ZCWAVE_fopen						fopen
#define ZCWAVE_fclose						fclose
#define ZCWAVE_fread						fread
#define ZCWAVE_fwrite						fwrite
#define ZCWAVE_fseek						fseek
#define ZCWAVE_ftell						ftell
#define ZCWAVE_fscanf_s						fscanf_s
#define ZCWAVE_fwscanf_s					fwscanf_s
#define ZCWAVE_fopen_s						fopen_s
#define ZCWAVE_wfopen_s						_wfopen_s
#define ZCWAVE_fprintf						fprintf
#define ZCWAVE_fwprintf						fwprintf
#define ZCWAVE_ShellExecuteA				ShellExecuteA
#define ZCWAVE_ShellExecuteW				ShellExecuteW
#define ZCWAVE_ShellExecuteExA				ShellExecuteExA
#define ZCWAVE_ShellExecuteExW				ShellExecuteExW
#define ZCWAVE_GetTickCount					GetTickCount
#define ZCWAVE_GetTickCount64				GetTickCount64
#define ZCWAVE_GetVolumeInformationA		GetVolumeInformationA
#define ZCWAVE_GetVolumeInformationW		GetVolumeInformationW

// FAST

#define ZCWAVE_Fast_LoadLibraryA			LoadLibraryA
#define ZCWAVE_Fast_LoadLibraryW			LoadLibraryW
#define ZCWAVE_Fast_FreeLibrary				FreeLibrary
#define ZCWAVE_Fast_GetProcAddress			GetProcAddress
#define ZCWAVE_Fast_CreateFileA				CreateFileA
#define ZCWAVE_Fast_CreateFileW				CreateFileW
#define ZCWAVE_FastReadFile					ReadFile
#define ZCWAVE_FastWriteFile				WriteFile
#define ZCWAVE_Fast_SetFilePointer			SetFilePointer

#define ZCWAVE_Fast_malloc					malloc
#define ZCWAVE_Fast_free					FreeStub
#define ZCWAVE_Fast_fopen					fopen
#define ZCWAVE_Fast_fclose					fclose
#define ZCWAVE_Fast_fread					fread
#define ZCWAVE_Fast_fwrite					fwrite
#define ZCWAVE_Fast_fseek					fseek
#define ZCWAVE_Fast_ftell					ftell
#define ZCWAVE_Fast_fscanf_s				fscanf_s
#define ZCWAVE_Fast_fwscanf_s				fwscanf_s
#define ZCWAVE_Fast_fopen_s					fopen_s
#define ZCWAVE_Fast_wfopen_s				_wfopen_s
#define ZCWAVE_Fast_fprintf					fprintf
#define ZCWAVE_Fast_fwprintf				fwprintf
#define ZCWAVE_Fast_ShellExecuteA			ShellExecuteA
#define ZCWAVE_Fast_ShellExecuteW			ShellExecuteW
#define ZCWAVE_Fast_ShellExecuteExA			ShellExecuteExA
#define ZCWAVE_Fast_ShellExecuteExW			ShellExecuteExW
#define ZCWAVE_Fast_GetTickCount			GetTickCount
#define ZCWAVE_Fast_GetTickCount64			GetTickCount64
#define ZCWAVE_Fast_GetVolumeInformationA	GetVolumeInformationA
#define ZCWAVE_Fast_GetVolumeInformationW	GetVolumeInformationW

#endif

#if defined(__cplusplus)

#if defined(USE_XIGNCODE3_SECURE_API)

// 0 param

template <class ReturnType, class Func>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);

	*result = fn();
	return X_SUCCESS;
}

template <class ReturnType, class Func>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn)
{
	xstatus s;
	ReturnType result;

	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func>, &result, fn);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}

// 1 param

template <class ReturnType, class Func, class A0>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);

	*result = fn(a0);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0>, &result, fn, a0);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}

// 2 param

template <class ReturnType, class Func, class A0, class A1>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);
	A1 a1 = va_arg(ap, A1);

	*result = fn(a0, a1);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0, class A1>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0, A1 a1)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0, A1>, &result, fn, a0, a1);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}

// 3 param

template <class ReturnType, class Func, class A0, class A1, class A2>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);
	A1 a1 = va_arg(ap, A1);
	A2 a2 = va_arg(ap, A2);

	*result = fn(a0, a1, a2);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0, class A1, class A2>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0, A1 a1, A2 a2)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0, A1, A2>, &result, fn, a0, a1, a2);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}


// 4 param

template <class ReturnType, class Func, class A0, class A1, class A2, class A3>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);
	A1 a1 = va_arg(ap, A1);
	A2 a2 = va_arg(ap, A2);
	A3 a3 = va_arg(ap, A3);

	*result = fn(a0, a1, a2, a3);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0, A1 a1, A2 a2, A3 a3)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0, A1, A2, A3>, &result, fn, a0, a1, a2, a3);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}

// 5 param


template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);
	A1 a1 = va_arg(ap, A1);
	A2 a2 = va_arg(ap, A2);
	A3 a3 = va_arg(ap, A3);
	A4 a4 = va_arg(ap, A4);

	*result = fn(a0, a1, a2, a3, a4);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0, A1, A2, A3, A4>, &result, fn, a0, a1, a2, a3, a4);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}


// 6 param


template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);
	A1 a1 = va_arg(ap, A1);
	A2 a2 = va_arg(ap, A2);
	A3 a3 = va_arg(ap, A3);
	A4 a4 = va_arg(ap, A4);
	A5 a5 = va_arg(ap, A5);

	*result = fn(a0, a1, a2, a3, a4, a5);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0, A1, A2, A3, A4, A5>, &result, fn, a0, a1, a2, a3, a4, a5);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}



// 7 param


template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);
	A1 a1 = va_arg(ap, A1);
	A2 a2 = va_arg(ap, A2);
	A3 a3 = va_arg(ap, A3);
	A4 a4 = va_arg(ap, A4);
	A5 a5 = va_arg(ap, A5);
	A6 a6 = va_arg(ap, A6);

	*result = fn(a0, a1, a2, a3, a4, a5, a6);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0, A1, A2, A3, A4, A5, A6>, &result, fn, a0, a1, a2, a3, a4, a5, a6);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}



// 8 param


template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);
	A1 a1 = va_arg(ap, A1);
	A2 a2 = va_arg(ap, A2);
	A3 a3 = va_arg(ap, A3);
	A4 a4 = va_arg(ap, A4);
	A5 a5 = va_arg(ap, A5);
	A6 a6 = va_arg(ap, A6);
	A7 a7 = va_arg(ap, A7);

	*result = fn(a0, a1, a2, a3, a4, a5, a6, a7);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0, A1, A2, A3, A4, A5, A6, A7>, &result, fn, a0, a1, a2, a3, a4, a5, a6, a7);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}


// 9 param


template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);
	A1 a1 = va_arg(ap, A1);
	A2 a2 = va_arg(ap, A2);
	A3 a3 = va_arg(ap, A3);
	A4 a4 = va_arg(ap, A4);
	A5 a5 = va_arg(ap, A5);
	A6 a6 = va_arg(ap, A6);
	A7 a7 = va_arg(ap, A7);
	A8 a8 = va_arg(ap, A8);

	*result = fn(a0, a1, a2, a3, a4, a5, a6, a7, a8);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0, A1, A2, A3, A4, A5, A6, A7, A8>, &result, fn, a0, a1, a2, a3, a4, a5, a6, a7, a8);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);

	return result;
}


// 10 param


template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
xstatus ZCWAVE_CallBinder(va_list ap)
{
	ReturnType *result = va_arg(ap, ReturnType *);
	Func fn = va_arg(ap, Func);
	A0 a0 = va_arg(ap, A0);
	A1 a1 = va_arg(ap, A1);
	A2 a2 = va_arg(ap, A2);
	A3 a3 = va_arg(ap, A3);
	A4 a4 = va_arg(ap, A4);
	A5 a5 = va_arg(ap, A5);
	A6 a6 = va_arg(ap, A6);
	A7 a7 = va_arg(ap, A7);
	A8 a8 = va_arg(ap, A8);
	A9 a9 = va_arg(ap, A9);

	*result = fn(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	return X_SUCCESS;
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9)
{
	xstatus s;
	ReturnType result;
	
	X_PREPARE_SEED(frtn);

	s = ZCWAVE_SendCommand(ZCMD_CALL_FUNC, in_seed, &out_seed, (CallGateT) ZCWAVE_CallBinder<ReturnType, Func, A0, A1, A2, A3, A4, A5, A6, A7, A8, A9>, &result, fn, a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
	if(!X_SUCCEEDED(s))
	{
		SetLastError(s);
		return frtn;
	}

	X_CHECK_SEED(frtn);
	
	return result;
}


#else


template <class ReturnType, class Func>
ReturnType ZCWAVE_Call(Func fn, ReturnType frtn)
{
	return fn();
}

template <class ReturnType, class Func, class A0>
ReturnType ZCWAVE_Call(Func fn, A0 a0, ReturnType frtn)
{
	return fn(a0);
}

template <class ReturnType, class Func, class A0, class A1>
ReturnType ZCWAVE_Call(Func fn, A0 a0, A1 a1, ReturnType frtn)
{
	return fn(a0, a1);
}

template <class ReturnType, class Func, class A0, class A1, class A2>
ReturnType ZCWAVE_Call(Func fn, A0 a0, A1 a1, A2 a2, ReturnType frtn)
{
	return fn(a0, a1, a2);
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3>
ReturnType ZCWAVE_Call(Func fn, A0 a0, A1 a1, A2 a2, A3 a3, ReturnType frtn)
{
	return fn(a0, a1, a2, a3);
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4>
ReturnType ZCWAVE_Call(Func fn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, ReturnType frtn)
{
	return fn(a0, a1, a2, a3, a4);
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5>
ReturnType ZCWAVE_Call(Func fn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, ReturnType frtn)
{
	return fn(a0, a1, a2, a3, a4, a5);
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6>
ReturnType ZCWAVE_Call(Func fn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, ReturnType frtn)
{
	return fn(a0, a1, a2, a3, a4, a5, a6);
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
ReturnType ZCWAVE_Call(Func fn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, ReturnType frtn)
{
	return fn(a0, a1, a2, a3, a4, a5, a6, a7);
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
ReturnType ZCWAVE_Call(Func fn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, ReturnType frtn)
{
	return fn(a0, a1, a2, a3, a4, a5, a6, a7, a8);
}

template <class ReturnType, class Func, class A0, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
ReturnType ZCWAVE_Call(Func fn, A0 a0, A1 a1, A2 a2, A3 a3, A4 a4, A5 a5, A6 a6, A7 a7, A8 a8, A9 a9, ReturnType frtn)
{
	return fn(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9);
}


#endif // USE_XIGNCODE3_SECURE_API

#endif // __cplusplus

#endif
