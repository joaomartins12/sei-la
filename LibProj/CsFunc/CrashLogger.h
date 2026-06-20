#pragma once

namespace nsCSDEBUG
{
	// Global error / crash logger. Initialize once at app start; thereafter,
	// every assert_cs failure, unhandled SEH exception, unhandled C++ exception,
	// pure-virtual call, or CRT invalid-parameter is appended to the log file
	// with a symbolized stack trace.
	namespace CrashLogger
	{
		// Opens the log file (creates parent dir if needed) and installs the
		// process-wide handlers. Safe to call more than once; only the first call
		// has effect. Pass nullptr or "" to use the default path "logs/crash.log".
		void Init( const char* logPath );

		// Called by assert_cs::_log() to append an assertion entry with stack.
		void LogAssert( const char* file, int line, const char* expr, const char* details );

		// Free-form entry; useful for ad-hoc trace points without an assert.
		void LogMessage( const char* fmt, ... );
	}
}
