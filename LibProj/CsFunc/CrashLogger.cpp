#include "stdafx.h"
#include "CrashLogger.h"

#include <windows.h>
#include <dbghelp.h>
#include <psapi.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <eh.h>
#include <new.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

namespace nsCSDEBUG
{
namespace CrashLogger
{

// ---------- module state ----------
static CRITICAL_SECTION   g_cs;
static FILE*              g_fp           = NULL;
static bool               g_inited       = false;
static bool               g_symInited    = false;
static bool               g_isWine       = false;  // skip Sym* on Wine — its dbghelp asserts internally
static char               g_logPath[ MAX_PATH ] = { 0 };

static bool DetectWine()
{
	HMODULE h = GetModuleHandleA( "ntdll.dll" );
	if( !h ) return false;
	return GetProcAddress( h, "wine_get_version" ) != NULL;
}

// Returns true when Sym* / StackWalk64 calls are believed safe to make.
// On Windows: always true.
// On Wine: true ONLY if dbghelp.dll resolved to a copy outside system32/syswow64
// (i.e., a Microsoft-native dbghelp.dll shipped next to the exe). Wine's builtin
// dbghelp asserts internally on PDB section walks, so we skip symbolization there.
static bool DbgHelpUsable()
{
	if( !g_isWine )
		return true;

	HMODULE h = GetModuleHandleA( "dbghelp.dll" );
	if( !h )
	{
		// Force-load via the implicit-linked import. This succeeds because
		// CrashLogger.cpp pragma-links against dbghelp.lib.
		h = LoadLibraryA( "dbghelp.dll" );
		if( !h )
			return false;
	}

	char modPath[ MAX_PATH ] = { 0 };
	if( GetModuleFileNameA( h, modPath, MAX_PATH ) == 0 )
		return false;

	// Lowercase compare for Windows-style "C:\windows\system32\..." paths.
	for( char* p = modPath; *p; ++p )
		if( *p >= 'A' && *p <= 'Z' ) *p = (char)( *p - 'A' + 'a' );

	if( strstr( modPath, "\\system32\\" ) || strstr( modPath, "\\syswow64\\" ) )
		return false;  // Wine's builtin — unsafe

	return true;       // Likely a native dbghelp.dll shipped with the app
}

// Saved previous handlers so we don't stomp anyone we don't have to.
static LPTOP_LEVEL_EXCEPTION_FILTER g_prevSEHFilter   = NULL;
static terminate_handler            g_prevTerminate   = NULL;
static _purecall_handler            g_prevPurecall    = NULL;
static _invalid_parameter_handler   g_prevInvalidParm = NULL;

// ---------- helpers ----------
static void EnsureDir( const char* path )
{
	// Walk the path and CreateDirectoryA each segment. Ignore failures
	// (the dir likely already exists or path is the bare filename).
	char buf[ MAX_PATH ];
	lstrcpynA( buf, path, MAX_PATH );

	for( char* p = buf; *p; ++p )
	{
		if( *p == '\\' || *p == '/' )
		{
			char saved = *p;
			*p = '\0';
			if( buf[ 0 ] != '\0' )
				CreateDirectoryA( buf, NULL );
			*p = saved;
		}
	}
}

static void Timestamp( char* out, size_t cap )
{
	SYSTEMTIME t;
	GetLocalTime( &t );
	_snprintf_s( out, cap, _TRUNCATE, "%04u-%02u-%02u %02u:%02u:%02u.%03u",
		t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds );
}

static void EnsureSymInit()
{
	if( g_symInited )
		return;

	// Wine's builtin dbghelp.dll asserts inside symbol.c when walking PDB sections.
	// Skip Sym* there; the stack trace still resolves to module+offset via
	// GetModuleFileNameA. To get full symbolization on Wine, drop a native
	// Microsoft dbghelp.dll next to the exe (DbgHelpUsable() detects it via path).
	if( !DbgHelpUsable() )
		return;

	SymSetOptions( SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEBUG );

	if( !SymInitialize( GetCurrentProcess(), NULL, TRUE ) )
		return;

	g_symInited = true;

	// SymInitialize's "invade process" param doesn't always enumerate modules
	// reliably under Wine's loader. Walk EnumProcessModules and explicitly
	// SymLoadModuleEx each one — guarantees modules are mapped before SymFromAddr.
	HMODULE hMods[ 256 ];
	DWORD   cbNeeded = 0;
	HANDLE  hProc = GetCurrentProcess();
	if( EnumProcessModules( hProc, hMods, sizeof( hMods ), &cbNeeded ) )
	{
		const DWORD count = cbNeeded / sizeof( HMODULE );
		for( DWORD i = 0; i < count; ++i )
		{
			char modPath[ MAX_PATH ] = { 0 };
			MODULEINFO mi = { 0 };
			GetModuleFileNameExA( hProc, hMods[ i ], modPath, MAX_PATH );
			GetModuleInformation( hProc, hMods[ i ], &mi, sizeof( mi ) );

			SymLoadModuleEx( hProc, NULL, modPath, NULL,
				(DWORD64)(uintptr_t)mi.lpBaseOfDll, mi.SizeOfImage, NULL, 0 );
		}
	}
}

static void WriteLine( const char* line )
{
	if( g_fp == NULL )
		return;
	fputs( line, g_fp );
	fputc( '\n', g_fp );
	fflush( g_fp );
}

// Write a stack trace to g_fp. If pCtx is non-null, walks that context
// (used by the SEH filter); otherwise captures from the current call site.
static void WriteStack( CONTEXT* pCtx, int skipFrames )
{
	if( g_fp == NULL )
		return;

	EnsureSymInit();

	const int kMaxFrames = 62;
	void* frames[ kMaxFrames ] = { 0 };
	USHORT captured = 0;

	if( pCtx == NULL )
	{
		captured = CaptureStackBackTrace( (DWORD)skipFrames, kMaxFrames, frames, NULL );
	}
	else if( !DbgHelpUsable() )
	{
		// StackWalk64 routes through Wine's builtin dbghelp which is the source
		// of the internal assertion. Don't risk it — log the faulting PC only.
#ifdef _M_IX86
		if( pCtx->Eip != 0 )
			frames[ captured++ ] = (void*)(uintptr_t)pCtx->Eip;
#elif defined(_M_X64)
		if( pCtx->Rip != 0 )
			frames[ captured++ ] = (void*)(uintptr_t)pCtx->Rip;
#endif
	}
	else
	{
		// Walk a saved CONTEXT (post-mortem). Use StackWalk64.
		STACKFRAME64 sf = { 0 };
		DWORD machine;

#ifdef _M_IX86
		machine = IMAGE_FILE_MACHINE_I386;
		sf.AddrPC.Offset    = pCtx->Eip;
		sf.AddrPC.Mode      = AddrModeFlat;
		sf.AddrFrame.Offset = pCtx->Ebp;
		sf.AddrFrame.Mode   = AddrModeFlat;
		sf.AddrStack.Offset = pCtx->Esp;
		sf.AddrStack.Mode   = AddrModeFlat;
#elif defined(_M_X64)
		machine = IMAGE_FILE_MACHINE_AMD64;
		sf.AddrPC.Offset    = pCtx->Rip;
		sf.AddrPC.Mode      = AddrModeFlat;
		sf.AddrFrame.Offset = pCtx->Rbp;
		sf.AddrFrame.Mode   = AddrModeFlat;
		sf.AddrStack.Offset = pCtx->Rsp;
		sf.AddrStack.Mode   = AddrModeFlat;
#else
		fputs( "  [stack walk: unsupported architecture]\n", g_fp );
		return;
#endif

		HANDLE hProc = GetCurrentProcess();
		HANDLE hThread = GetCurrentThread();
		while( captured < kMaxFrames &&
			StackWalk64( machine, hProc, hThread, &sf, pCtx,
				NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL ) )
		{
			if( sf.AddrPC.Offset == 0 )
				break;
			frames[ captured++ ] = (void*)(uintptr_t)sf.AddrPC.Offset;
		}
	}

	fputs( "  Stack:\n", g_fp );

	if( captured == 0 )
	{
		fputs( "    [no frames captured]\n", g_fp );
		return;
	}

	// Buffer for SymFromAddr — IMAGEHLP_SYMBOL64 expects MaxNameLength bytes
	// of trailing storage for the demangled name.
	BYTE symBuf[ sizeof( SYMBOL_INFO ) + 256 ];
	SYMBOL_INFO* sym = (SYMBOL_INFO*)symBuf;
	IMAGEHLP_LINE64 line; ZeroMemory( &line, sizeof(line) );

	for( USHORT i = 0; i < captured; ++i )
	{
		DWORD64 addr = (DWORD64)(uintptr_t)frames[ i ];

		ZeroMemory( sym, sizeof( symBuf ) );
		sym->SizeOfStruct = sizeof( SYMBOL_INFO );
		sym->MaxNameLen   = 255;

		const char* symName  = NULL;
		DWORD64     symDisp  = 0;
		bool        haveSym  = ( g_symInited &&
			SymFromAddr( GetCurrentProcess(), addr, &symDisp, sym ) &&
			sym->Name[ 0 ] != '\0' );
		if( haveSym )
			symName = sym->Name;

		// Resolve module + base for fallback when the symbol isn't found.
		char    modName[ MAX_PATH ] = "<module?>";
		DWORD64 modBase = 0;
		HMODULE hMod = NULL;
		if( GetModuleHandleExA( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
		                       GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
		                       (LPCSTR)addr, &hMod ) && hMod )
		{
			char fullPath[ MAX_PATH ];
			if( GetModuleFileNameA( hMod, fullPath, MAX_PATH ) )
			{
				const char* base = strrchr( fullPath, '\\' );
				lstrcpynA( modName, base ? base + 1 : fullPath, MAX_PATH );
			}
			modBase = (DWORD64)(uintptr_t)hMod;
		}

		line.SizeOfStruct = sizeof( line );
		DWORD lineDisp = 0;
		char lineInfo[ MAX_PATH + 16 ] = "";
		if( g_symInited && SymGetLineFromAddr64( GetCurrentProcess(), addr, &lineDisp, &line ) )
		{
			_snprintf_s( lineInfo, _TRUNCATE, "  (%s:%lu)", line.FileName, (unsigned long)line.LineNumber );
		}

		char buf[ 1024 ];
		if( haveSym )
		{
			// Best case: full symbol resolution.
			_snprintf_s( buf, _TRUNCATE, "    [0x%08llx] %s!%s+0x%llx%s",
				(unsigned long long)addr, modName, symName,
				(unsigned long long)symDisp, lineInfo );
		}
		else
		{
			// No symbol — print absolute addr and module-relative offset so the
			// frame can be resolved offline (e.g. dia2dump on the .pdb).
			DWORD64 modOffset = ( modBase && addr >= modBase ) ? ( addr - modBase ) : 0;
			_snprintf_s( buf, _TRUNCATE, "    [0x%08llx] %s+0x%llx (no symbol)",
				(unsigned long long)addr, modName, (unsigned long long)modOffset );
		}
		WriteLine( buf );
	}
}

// ---------- handlers ----------
static const char* SEHCodeName( DWORD code )
{
	switch( code )
	{
	case EXCEPTION_ACCESS_VIOLATION:         return "EXCEPTION_ACCESS_VIOLATION";
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
	case EXCEPTION_BREAKPOINT:               return "EXCEPTION_BREAKPOINT";
	case EXCEPTION_DATATYPE_MISALIGNMENT:    return "EXCEPTION_DATATYPE_MISALIGNMENT";
	case EXCEPTION_FLT_DENORMAL_OPERAND:     return "EXCEPTION_FLT_DENORMAL_OPERAND";
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
	case EXCEPTION_FLT_INEXACT_RESULT:       return "EXCEPTION_FLT_INEXACT_RESULT";
	case EXCEPTION_FLT_INVALID_OPERATION:    return "EXCEPTION_FLT_INVALID_OPERATION";
	case EXCEPTION_FLT_OVERFLOW:             return "EXCEPTION_FLT_OVERFLOW";
	case EXCEPTION_FLT_STACK_CHECK:          return "EXCEPTION_FLT_STACK_CHECK";
	case EXCEPTION_FLT_UNDERFLOW:            return "EXCEPTION_FLT_UNDERFLOW";
	case EXCEPTION_ILLEGAL_INSTRUCTION:      return "EXCEPTION_ILLEGAL_INSTRUCTION";
	case EXCEPTION_IN_PAGE_ERROR:            return "EXCEPTION_IN_PAGE_ERROR";
	case EXCEPTION_INT_DIVIDE_BY_ZERO:       return "EXCEPTION_INT_DIVIDE_BY_ZERO";
	case EXCEPTION_INT_OVERFLOW:             return "EXCEPTION_INT_OVERFLOW";
	case EXCEPTION_INVALID_DISPOSITION:      return "EXCEPTION_INVALID_DISPOSITION";
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return "EXCEPTION_NONCONTINUABLE_EXCEPTION";
	case EXCEPTION_PRIV_INSTRUCTION:         return "EXCEPTION_PRIV_INSTRUCTION";
	case EXCEPTION_SINGLE_STEP:              return "EXCEPTION_SINGLE_STEP";
	case EXCEPTION_STACK_OVERFLOW:           return "EXCEPTION_STACK_OVERFLOW";
	default:                                 return "UNKNOWN_EXCEPTION";
	}
}

static LONG WINAPI OnUnhandledSEH( EXCEPTION_POINTERS* ep )
{
	EnterCriticalSection( &g_cs );

	char ts[ 64 ]; Timestamp( ts, sizeof( ts ) );
	char buf[ 512 ];

	DWORD code = ep && ep->ExceptionRecord ? ep->ExceptionRecord->ExceptionCode : 0;
	void* addr = ep && ep->ExceptionRecord ? ep->ExceptionRecord->ExceptionAddress : NULL;

	_snprintf_s( buf, _TRUNCATE, "[%s] CRASH (SEH): %s code=0x%08lx addr=%p",
		ts, SEHCodeName( code ), (unsigned long)code, addr );
	WriteLine( buf );

	if( ep && ep->ContextRecord )
		WriteStack( ep->ContextRecord, 0 );
	else
		WriteStack( NULL, 1 );

	WriteLine( "" );
	LeaveCriticalSection( &g_cs );

	// Fall through to any previously installed filter, then default OS handling.
	if( g_prevSEHFilter )
		return g_prevSEHFilter( ep );
	return EXCEPTION_CONTINUE_SEARCH;
}

static void OnTerminate()
{
	EnterCriticalSection( &g_cs );
	char ts[ 64 ]; Timestamp( ts, sizeof( ts ) );
	char buf[ 256 ];
	_snprintf_s( buf, _TRUNCATE, "[%s] TERMINATE: unhandled C++ exception", ts );
	WriteLine( buf );
	WriteStack( NULL, 1 );
	WriteLine( "" );
	LeaveCriticalSection( &g_cs );

	if( g_prevTerminate )
		g_prevTerminate();
}

static void OnPurecall()
{
	EnterCriticalSection( &g_cs );
	char ts[ 64 ]; Timestamp( ts, sizeof( ts ) );
	char buf[ 256 ];
	_snprintf_s( buf, _TRUNCATE, "[%s] PURECALL: pure virtual function call", ts );
	WriteLine( buf );
	WriteStack( NULL, 1 );
	WriteLine( "" );
	LeaveCriticalSection( &g_cs );

	if( g_prevPurecall )
		g_prevPurecall();
}

static void OnInvalidParameter( const wchar_t* expression, const wchar_t* function,
                                const wchar_t* file, unsigned int line, uintptr_t /*reserved*/ )
{
	EnterCriticalSection( &g_cs );

	char exprA[ 512 ] = "";
	char funcA[ 512 ] = "";
	char fileA[ MAX_PATH ] = "";
	if( expression ) WideCharToMultiByte( CP_UTF8, 0, expression, -1, exprA, sizeof(exprA), NULL, NULL );
	if( function )   WideCharToMultiByte( CP_UTF8, 0, function,   -1, funcA, sizeof(funcA), NULL, NULL );
	if( file )       WideCharToMultiByte( CP_UTF8, 0, file,       -1, fileA, sizeof(fileA), NULL, NULL );

	char ts[ 64 ]; Timestamp( ts, sizeof( ts ) );
	char buf[ 1024 ];
	_snprintf_s( buf, _TRUNCATE,
		"[%s] CRT INVALID PARAMETER: in %s expr=\"%s\" at %s:%u",
		ts, funcA[ 0 ] ? funcA : "<unknown>", exprA, fileA, line );
	WriteLine( buf );
	WriteStack( NULL, 1 );
	WriteLine( "" );
	LeaveCriticalSection( &g_cs );

	if( g_prevInvalidParm )
		g_prevInvalidParm( expression, function, file, line, 0 );
}

// ---------- public API ----------
void Init( const char* logPath )
{
	if( g_inited )
		return;

	InitializeCriticalSection( &g_cs );

	g_isWine = DetectWine();

	const char* path = ( logPath && *logPath ) ? logPath : "logs\\crash.log";
	lstrcpynA( g_logPath, path, MAX_PATH );

	EnsureDir( g_logPath );
	fopen_s( &g_fp, g_logPath, "a" );

	g_prevSEHFilter   = SetUnhandledExceptionFilter( OnUnhandledSEH );
	g_prevTerminate   = set_terminate( OnTerminate );
	g_prevPurecall    = _set_purecall_handler( OnPurecall );
	g_prevInvalidParm = _set_invalid_parameter_handler( OnInvalidParameter );

	g_inited = true;

	if( g_fp )
	{
		char ts[ 64 ]; Timestamp( ts, sizeof( ts ) );
		char buf[ 256 ];
		_snprintf_s( buf, _TRUNCATE, "[%s] CrashLogger initialized (log: %s, wine=%d, dbghelp_usable=%d)",
			ts, g_logPath, g_isWine ? 1 : 0, DbgHelpUsable() ? 1 : 0 );
		WriteLine( buf );
	}
}

void LogAssert( const char* file, int line, const char* expr, const char* details )
{
	if( !g_inited || !g_fp )
		return;

	EnterCriticalSection( &g_cs );

	char ts[ 64 ]; Timestamp( ts, sizeof( ts ) );
	char buf[ 1024 ];
	if( details && *details )
	{
		_snprintf_s( buf, _TRUNCATE, "[%s] ASSERT: %s:%d  expr: %s  details: %s",
			ts, file ? file : "<?>", line, expr ? expr : "<?>", details );
	}
	else
	{
		_snprintf_s( buf, _TRUNCATE, "[%s] ASSERT: %s:%d  expr: %s",
			ts, file ? file : "<?>", line, expr ? expr : "<?>" );
	}
	WriteLine( buf );

	// skipFrames=2: skip CaptureStackBackTrace + LogAssert itself, so the
	// top frame is the caller (i.e. the assert_cs site).
	WriteStack( NULL, 2 );
	WriteLine( "" );

	LeaveCriticalSection( &g_cs );
}

void LogMessage( const char* fmt, ... )
{
	if( !g_inited || !g_fp || !fmt )
		return;

	EnterCriticalSection( &g_cs );

	char ts[ 64 ]; Timestamp( ts, sizeof( ts ) );
	char body[ 1024 ];
	va_list args;
	va_start( args, fmt );
	_vsnprintf_s( body, _TRUNCATE, fmt, args );
	va_end( args );

	char line[ 1280 ];
	_snprintf_s( line, _TRUNCATE, "[%s] %s", ts, body );
	WriteLine( line );

	LeaveCriticalSection( &g_cs );
}

} // namespace CrashLogger
} // namespace nsCSDEBUG
