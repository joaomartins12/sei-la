//-----------------------------------------------------------------------------
//
//	$File: cLogFile.h $
//	$Author: Cho yong-il, caelus@hanmail.net $
//  
//	$Modtime: 14.01.03 02:12 $
//	$Revision: 0 $
//
//	Description: Interfaces of the classes "cLogFile"
//               (wrapper class to write some data to specified file)
//
//-----------------------------------------------------------------------------
 
#ifndef _LOGFILE_H_
#define _LOGFILE_H_
 
//-----------------------------------------------------------------------------
// Include
//-----------------------------------------------------------------------------
#include <windows.h>
#include <stdio.h>
#include <time.h>

//-----------------------------------------------------------------------------
// Name : class cLogFile
// Desc : log file controller
//-----------------------------------------------------------------------------
class cLogFile
{
public:
	cLogFile();
	~cLogFile();

public:
	BOOL OpenLog(LPCTSTR lpLogFile);
	BOOL WriteLog(LPBYTE lpLog, int nSizeOfLog);
	bool TruncateFile(LPCTSTR lpLogFile);
	void CloseLog();
	
protected:
	HANDLE	hFileLog;
};

bool GetTimeSTR(LPSTR szData,SIZE_T DateSize, LPSTR szTime, SIZE_T TimeSize);
bool GetTimeSTR5Min(LPSTR szData,SIZE_T DateSize, LPSTR szTime, SIZE_T TimeSize);
unsigned int GetTimeMin();

////-----------------------------------------------------------------------------
//// Definitions : functions for time
////-----------------------------------------------------------------------------
//inline tm CurrentDate()
//{
//	time_t ltime;
//    struct tm* today;
//
//	time(&ltime);
//	today = localtime(&ltime);
//
//    return *today;
//}
//
////-----------------------------------------------------------------------------
//// Definitions : functions for logging
////-----------------------------------------------------------------------------
//inline void Log(char* fmt, ...)
//{
//	try
//	{
//		char msg[1024];
//		char strTime[20];
//		char strDate[20];
//		char logmsg[1044];
//		_strtime_s(strTime);
//		_strdate_s(strDate);
//		ZeroMemory(msg, 1024);
//		ZeroMemory(logmsg, 1044);
//
//		va_list argptr;
//		va_start(argptr, fmt);
//		vsprintf_s(msg, fmt, argptr);
//		va_end(argptr);
//		sprintf_s(logmsg, "[%s %s] %s \r\n", strDate, strTime, msg);
//
//		char filename[96];
//		struct tm today = CurrentDate();
//		strftime(strTime, 20, "(%y-%m-%d)", &today);
//		wsprintf(filename, "LogFile\\log%s.txt", strTime);
//
//		cLogFile file;
//		file.OpenLog(filename);
//		file.WriteLog((unsigned char*)logmsg, (int)strlen(logmsg));
//
//		printf("%s", logmsg);
//	}
//	catch (...) { }
//}
//
//inline void SysLog(char* fmt, ...)
//{
//	char logmsg[552];
//	ZeroMemory(logmsg, 552);
//	
//	char msg[512];
//	ZeroMemory(msg, 512);
//	va_list argptr;
//	va_start(argptr, fmt);
//	vsprintf(msg, fmt, argptr);
//	va_end(argptr);
//	
//	char tmpbuf[30];
//	_strtime(tmpbuf);
//	wsprintf(logmsg, "[%s] %s", tmpbuf, msg);
//	strcat(logmsg, "\r\n");
//	
//	printf(logmsg);
//	
//	char filename[96];
//	struct tm today = CurrentDate();
//	strftime(tmpbuf, 40, "(%d %B %Y)", &today);
//	wsprintf(filename, "LogFile\\syslog%s.txt", tmpbuf);
//	
//	cLogFile file;
//	file.OpenLog(filename);
//	file.WriteLog((unsigned char*)logmsg,(int) strlen(logmsg));
//}
//
//inline void WareHouseLog(char* fmt, ...)
//{
//	try
//	{
//		char msg[1024];
//		char strTime[20];
//		char logmsg[1044];
//		_strtime(strTime);
//		ZeroMemory(msg, 1024);
//		ZeroMemory(logmsg, 1044);
//
//		va_list argptr;
//		va_start(argptr, fmt);
//		vsprintf(msg, fmt, argptr);
//		va_end(argptr);
//		wsprintf(logmsg, "[%s] %s \r\n", strTime, msg);
//
//		char filename[96];
//		struct tm today = CurrentDate();
//		strftime(strTime, 20, "(%y-%m-%d)", &today);
//		wsprintf(filename, "LogFile\\창고%s.txt", strTime);
//
//		cLogFile file;
//		file.OpenLog(filename);
//		file.WriteLog((unsigned char*)logmsg,(int) strlen(logmsg));
//
//		//printf("%s", logmsg);
//	}
//	catch (...) { }
//}
//
//inline void ErrLog(char* fmt, ...)
//{
//	try
//	{
//		char msg[1024];
//		char strTime[20];
//		char logmsg[1044];
//		_strtime(strTime);
//		ZeroMemory(msg, 1024);
//		ZeroMemory(logmsg, 1044);
//
//		va_list argptr;
//		va_start(argptr, fmt);
//		vsprintf(msg, fmt, argptr);
//		va_end(argptr);
//		wsprintf(logmsg, "[%s] %s \r\n", strTime, msg);
//
//		char filename[96];
//		struct tm today = CurrentDate();
//		strftime(strTime, 20, "(%y-%m-%d)", &today);
//		wsprintf(filename, "LogFile\\Errlog%s.txt", strTime);
//
//		cLogFile file;
//		file.OpenLog(filename);
//		file.WriteLog((unsigned char*)logmsg,(int) strlen(logmsg));
//	}
//	catch (...) { }
//}

inline void DebugLog(char* fmt, ...)
{
	try
	{
		char msg[1024];
		ZeroMemory(msg, 1024);
		va_list argptr;
		va_start(argptr, fmt);
		vsprintf_s(msg, fmt, argptr);
		va_end(argptr);

		strcat_s(msg, "\r\n");
		OutputDebugString(msg);
	}
	catch (...) { }
}


//// 1015 추가 - 로그 파일
//inline void FileLog(char *szFileName, char* fmt, ...)
//{
//	try
//	{
//		char msg[1024];
//		char strTime[20];
//		char logmsg[1044];
//		_strtime(strTime);
//		ZeroMemory(msg, 1024);
//		ZeroMemory(logmsg, 1044);
//
//		va_list argptr;
//		va_start(argptr, fmt);
//		vsprintf(msg, fmt, argptr);
//		va_end(argptr);
//		wsprintf(logmsg, "%s %s\r\n", strTime, msg);
//		OutputDebugString(msg);
//		OutputDebugString("\n");
//
//		char filename[96];
//		struct tm today = CurrentDate();
//		strftime(strTime, 20, "%Y-%m-%d", &today);
//		wsprintf(filename, "log\\%s-%s.txt", szFileName, strTime);
//
//		cLogFile file;
//		file.OpenLog(filename);
//		file.WriteLog((unsigned char*)logmsg,(int) strlen(logmsg));
//	}
//	catch (...) { }
//}


// 1015 추가 - 로그 파일
inline void File5minLog(char *szFileName, char* fmt, ...)
{
	try
	{
		char msg[1024] = {0,};
		char logmsg[1044] = {9,};

		va_list argptr;
		va_start(argptr, fmt);
		vsprintf_s(msg, fmt, argptr);
		va_end(argptr);

		char tszDate[16] = {0,};
		char tszTime[16] = {0,};				
		if( GetTimeSTR5Min(tszDate, sizeof(tszDate), tszTime, sizeof(tszTime)) == false )
		{
			OutputDebugString("GetTimeSTR FAIL BUFF ERROR");
			return;
		}

		sprintf_s(logmsg, "%s %s\r\n", tszTime, msg);

		OutputDebugString(msg);
		OutputDebugString("\n");

		char filename[256] = {0,};
		sprintf_s(filename, "log\\%s-%s_%s.log", szFileName, tszDate, tszTime);

		cLogFile file;
		file.OpenLog(filename);
		file.WriteLog((unsigned char*)logmsg, (int)strlen(logmsg));
	}
	catch (...) { }
}

// 2005.04.26 추가 - 로그 파일
inline void FileLog2(char *szFileName, char* fmt, ...)
{
	try
	{
		char msg[1024] = {0,};		
		char logmsg[1044] = {0,};
		char tszDate[16] = {0,};
		char tszTime[16] = {0,};		
		char filename[128] = {0,};
		char szPath[32] = {".\\FileLog",};
		
		va_list argptr;
		va_start(argptr, fmt);
		vsprintf_s(msg, fmt, argptr);
		va_end(argptr);

		if( GetTimeSTR(tszDate, sizeof(tszDate), tszTime, sizeof(tszTime)) == false )
		{
			OutputDebugString("GetTimeSTR FAIL BUFF ERROR");
			return;
		}

		sprintf_s(logmsg, "%s%s : %s\r\n",tszDate,tszTime,msg);

		OutputDebugString(msg);
		OutputDebugString("\n");		

		CreateDirectory( szPath, NULL );
		
		sprintf_s(filename, "FileLog\\%s-%s.txt", szFileName, tszDate);
		
		cLogFile file;
		file.OpenLog(filename);
		file.WriteLog((unsigned char*)logmsg,(int) strlen(logmsg));
	}
	catch (...) { }
}

inline bool FileDataTruncate(char *szFolder, char *szFileName)
{
	if( szFolder == NULL )
	{
		return false;
	}

	if( strlen(szFolder) >= 20 )
	{
		return false;
	}

	try
	{				
		char szPath[32] = {0,};
		sprintf_s( szPath, ".\\%s",szFolder);

		CreateDirectory( szPath, NULL );
		
		char filename[128] = {0,};		
		sprintf_s(filename, "%s\\%s",szFolder, szFileName);
		
		cLogFile file;

		bool bResult = file.TruncateFile(filename);
		
		return bResult;
	}
	catch (...) { }

	return false;
}

inline void WriteBulkFile(char *szFolder, char *szFileName, char* fmt, ...)
{
	if( szFolder == NULL )
	{
		return;
	}

	if( strlen(szFolder) >= 20 )
	{
		return;
	}

	try
	{
		char msg[1024] = {0,};		
		char logmsg[1044] = {0,};
				
		va_list argptr;
		va_start(argptr, fmt);
		vsprintf_s(msg, fmt, argptr);
		va_end(argptr);	
		
		sprintf_s(logmsg, "%s\r\n", msg);
		OutputDebugString(msg);
		OutputDebugString("\n");
		
		char szPath[32] = {0,};
		sprintf_s( szPath, ".\\%s",szFolder);

		CreateDirectory( szPath, NULL );
		
		char filename[128] = {0,};		
		sprintf_s(filename, "%s\\%s",szFolder, szFileName);
		
		cLogFile file;
		file.OpenLog(filename);
		file.WriteLog((unsigned char*)logmsg,(int) strlen(logmsg));
	}
	catch (...) { }
}

// 하나의 파일에 공통적으로 로그를 쓸 경우 
inline void ShareFileLog(char *Path, char *szFileName, char* fmt, ...)
{
	try
	{
		char msg[1024] = {0,};
		char logmsg[1044] = {0,};
				
		va_list argptr;
		va_start(argptr, fmt);
		vsprintf_s(msg, fmt, argptr);
		va_end(argptr);

		char tszDate[16] = {0,};
		char tszTime[16] = {0,};				
		if( GetTimeSTR5Min(tszDate, sizeof(tszDate), tszTime, sizeof(tszTime)) == false )
		{
			OutputDebugString("GetTimeSTR FAIL BUFF ERROR");
			return;
		}
		
		sprintf_s(logmsg, "%s%s %s\r\n",tszDate,tszTime, msg);
		OutputDebugString(msg);
		OutputDebugString("\n");

		char szFullPath[280] = {0,};
		sprintf_s(szFullPath,"%s\\FileLog\\",Path);

		CreateDirectory( szFullPath, NULL );
		
		char filename[128] = {0,};		
		sprintf_s(filename, "%s\\%s-%s.txt",szFullPath,szFileName, tszDate);
		
		cLogFile file;
		file.OpenLog(filename);
		file.WriteLog((unsigned char*)logmsg, (int)strlen(logmsg));
	}
	catch (...) { }
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------


#endif