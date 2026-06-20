// UpdateManager.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "UpdateManager.h"
#include "UpdateManagerDlg.h"
#include "ClientOAuthKey.h"

BEGIN_MESSAGE_MAP(CUpdateManagerApp, CWinApp)
END_MESSAGE_MAP()

CUpdateManagerApp::CUpdateManagerApp()
{
}

bool CUpdateManagerApp::_GetParamData(CString const& param, CString & recvData, CString & cRealParam)
{
	int nPos = 0;	
	int nEndPos = param.GetLength() + 1;

	recvData = param.Tokenize(L"||", nPos);
	if( nPos == nEndPos )
		return false;

	cRealParam = param.Tokenize(L"||", nPos);
	return true;
}

CUpdateManagerApp theApp;

BOOL CUpdateManagerApp::InitInstance()
{
	CString sCmdLine = m_lpCmdLine;	
	if( sCmdLine.IsEmpty() )
		sCmdLine.Format(_T("%s%s||"), CONFIGURATION_URL, CONFIGURATION_FILENAME);
	else
	{
		LPTSTR pResult = EnDe::Decryption( sCmdLine );
		if( NULL == pResult )
			sCmdLine.Format(_T("%s%s||"), CONFIGURATION_URL, CONFIGURATION_FILENAME);
		else
			sCmdLine = pResult;
	}

	CString recvData;
	CString cRealParam;
	if( !_GetParamData( sCmdLine, recvData, cRealParam ) )
		recvData.Format(_T("%s%s"), CONFIGURATION_URL, CONFIGURATION_FILENAME);

	InitCommonControls();
	CWinApp::InitInstance();
	SetRegistryKey(_T("UpdateManager"));

	CString currentPath;
	GetCurrentPath( currentPath );
	
	CUpdateManagerDlg dlg;
	if( !dlg.Init( currentPath, recvData, cRealParam ) )
	{
		return FALSE;
	}

	m_pMainWnd = &dlg;
	dlg.DoModal();
	return FALSE;
}

void CUpdateManagerApp::GetCurrentPath( CString & OutPut )
{
	TCHAR szModulePath[MAX_PATH]={0,};
	GetModuleFileName(m_hInstance, szModulePath, MAX_PATH);

	TCHAR szDrive[_MAX_DRIVE] = _T("");
	TCHAR szDir[_MAX_DIR] = _T("");
	TCHAR szFName[_MAX_FNAME] = _T("");
	TCHAR szExt[_MAX_EXT] = _T("");

	_tsplitpath_s(szModulePath, szDrive, szDir, szFName, szExt);

	TCHAR szPath[MAX_PATH]={0,};
	_tmakepath_s( szPath, szDrive, szDir, NULL, NULL );

	OutPut = szPath;
}
