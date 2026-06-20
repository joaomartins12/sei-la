// AutoUpgrade.h : PROJECT_NAME 응용 프로그램에 대한 주 헤더 파일입니다.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH에서 이 파일을 포함하기 전에 'stdafx.h'를 포함하십시오.
#endif

#include "resource.h"		// 주 기호

class CAutoUpgradeApp : public CWinApp
{
public:
	CAutoUpgradeApp();

// 재정의
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()

private:
	BOOL		CheckFile( CString const& strPath, CString const& checkFileName );
	BOOL		DownLoadLauncherInfoData( sConfiguration & sInfo );
	BOOL		DownloadFile(CString const& Url, CString const& fileName );
	BOOL		VersionCheck_UpdateManager(sConfiguration const& downloadUrlInfo);
	BOOL		RunUpdateManager(CString const& exeFileName, CString const& wsSendMsg, CString const& szParam);

	void		GetCurrentPath( CString & OutPut, CString & exeFileName );
	WORD		GetFileRevisionNumber(CString const& FilePath);


private:
	CString		m_currentPath;
};
