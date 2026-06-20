// Launcher.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH에 대해 이 파일을 포함하기 전에 'stdafx.h'를 포함합니다."
#endif

#include "resource.h"		// 주 기호입니다.


// CLauncherApp:
// 이 클래스의 구현에 대해서는 Launcher.cpp을 참조하십시오.
//

class CLauncherApp : public CWinApp
{
public:
	CLauncherApp();	

public:
	virtual BOOL	InitInstance();
	CString			m_wsCmdLine;

private:
	bool			GetCmdParam( CString& wsCmdLine );

	// 파일의 리비젼 번호를 가져온다
	WORD			GetFileRevisionNumber( CString const& FullPathName );	
	void			GetCurrentPath( CString & OutPut, CString & exeFileName );
	BOOL			DownloadFile(CString const& Url, CString const& fileName );
	BOOL			DownLoadLauncherInfoData( sConfiguration & sInfo );
	BOOL			VersionCheck_UpdateManager(sConfiguration const& downloadUrlInfo, DWORD const& dwCurrentLauncherVersion);
	BOOL			RunUpdateManager(CString const& exeFileName, CString const& wsDownloadUrl, CString const& szParam);

private:
	CString			m_currentPath;

// 구현입니다.
	DECLARE_MESSAGE_MAP()		
};

extern CLauncherApp theApp;