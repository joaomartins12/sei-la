#include "StdAfx.h"
#include ".\downenginesdk.h"



CDownEngineSDK::CDownEngineSDK(void) : m_hDownEngineAPI(NULL)
{
	m_hDownEngineAPI = NULL;
}

CDownEngineSDK::~CDownEngineSDK(void)
{
	Close();
}

int CDownEngineSDK::Create(IN const CString& strDllPath)
{
	CFileStatus fs;
	if( CFile::GetStatus(strDllPath, fs) == NULL ) 
		return CS_FAIL_NOTEXIST_FILE;

	DWORD dwAttr = GetFileAttributes((LPCTSTR)strDllPath);
	dwAttr &= ~ FILE_ATTRIBUTE_READONLY;
	SetFileAttributes((LPCTSTR)strDllPath, dwAttr);

	m_hDownEngineAPI = LoadLibrary( strDllPath.operator LPCTSTR() );
	if( m_hDownEngineAPI == NULL )
		return CS_FAIL_LOAD_DLL;

	// 함수 어드레스 검사
	LPFNCREATE lpfnCreate = (LPFNCREATE)GetProcAddress(m_hDownEngineAPI, "NOWCDN_Create");
	if( lpfnCreate == NULL )
		return CS_FAIL_PROCADDRESS;

	LPFNAUTOPATCHSTART lpfnAutoPatchStart = (LPFNAUTOPATCHSTART)GetProcAddress(m_hDownEngineAPI, "NOWCDN_AutoPatchStart");
	if( lpfnAutoPatchStart == NULL )
		return CS_FAIL_PROCADDRESS;

	LPFNSETDOWNLOADFILEINFO lpfnSetDownloadInfo = (LPFNSETDOWNLOADFILEINFO)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetDownloadFileInfo");
	if( lpfnSetDownloadInfo == NULL )
		return CS_FAIL_PROCADDRESS;

	LPFNSETDOWNLOADFOLDERINFO lpfnSetDownloadFolderInfo = (LPFNSETDOWNLOADFOLDERINFO)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetDownloadFolderInfo");
	if( lpfnSetDownloadFolderInfo == NULL )
		return CS_FAIL_PROCADDRESS;

	LPFNCLOSE lpfnClose = (LPFNCLOSE)GetProcAddress(m_hDownEngineAPI, "NOWCDN_Close");
	if( lpfnClose == NULL )
		return CS_FAIL_PROCADDRESS;

	LPFNGETDOWNLOADINFO	lpfnGetDownloadInfo = (LPFNGETDOWNLOADINFO)GetProcAddress(m_hDownEngineAPI, "NOWCDN_GetDownloadInfo");
	if( lpfnGetDownloadInfo == NULL )
		return CS_FAIL_PROCADDRESS;

	LPFNSETPARENTWINDOW lpfnSetParentWindow = (LPFNSETPARENTWINDOW)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetParentWindow");
	if( lpfnSetParentWindow == NULL )
		return CS_FAIL_PROCADDRESS;

	LPFNSETDRAWFILEINFO lpfnSetDrawFileInfo = (LPFNSETDRAWFILEINFO)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetDrawFileInfoCallback");
	if( lpfnSetDrawFileInfo == NULL )
		return CS_FAIL_PROCADDRESS;

	LPFNUPDATESTARTBUTTON lpfnUpdateStartButton = (LPFNUPDATESTARTBUTTON)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetUpdateStartButtonCallBack");
	if( lpfnUpdateStartButton == NULL )
		return CS_FAIL_PROCADDRESS;

	LPFNNOTIFYDOWNCOMPLETE lpfnNotifyDownComplete = (LPFNNOTIFYDOWNCOMPLETE)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetNotifyDownCompleteCallBack");
	if( lpfnNotifyDownComplete == NULL )
		return CS_FAIL_PROCADDRESS;

	return CS_SUCCESS;
}

void CDownEngineSDK::Begin( IN const CString& strDllPath )
{
	LPFNCREATE lpfnCreate = (LPFNCREATE)GetProcAddress(m_hDownEngineAPI, "NOWCDN_Create");
	lpfnCreate( StringConvert_T2A(strDllPath) );
}

int CDownEngineSDK::AutoPatchStart()
{
	LPFNAUTOPATCHSTART lpfnAutoPatchStart = (LPFNAUTOPATCHSTART)GetProcAddress(m_hDownEngineAPI, "NOWCDN_AutoPatchStart");
	return lpfnAutoPatchStart();
}

int CDownEngineSDK::SetDownloadFileInfo(IN const CString& strDownloadFullPath, IN const CString& strDownloadURL)
{
	LPFNSETDOWNLOADFILEINFO lpfnSetDownloadInfo = (LPFNSETDOWNLOADFILEINFO)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetDownloadFileInfo");
	return lpfnSetDownloadInfo(StringConvert_T2A(strDownloadFullPath), StringConvert_T2A(strDownloadURL));
}

int CDownEngineSDK::SetDownloadFolderInfo(IN const CString& strDownloadFolderPath, IN const CString& strDownloadURL)
{
	LPFNSETDOWNLOADFOLDERINFO lpfnSetDownloadFolderInfo = (LPFNSETDOWNLOADFOLDERINFO)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetDownloadFolderInfo");
	return lpfnSetDownloadFolderInfo(StringConvert_T2A(strDownloadFolderPath), StringConvert_T2A(strDownloadURL));
}

void CDownEngineSDK::Close()
{
	if( m_hDownEngineAPI )
	{
		LPFNCLOSE lpfnClose;
		if( lpfnClose = (LPFNCLOSE)GetProcAddress(m_hDownEngineAPI, "NOWCDN_Close") )
		{
			lpfnClose();
		}
		FreeLibrary(m_hDownEngineAPI);
	}
}


//--------------------------------------------------------------------------------------------------//
// FUNCTION		:	GetDownloadInfo
// PARAMETER	:	ulDownCompleteCount		- 다운로드 완료한 파일 개수
//					ulDownloadSpeed			- 다운로드 속도
//					ullDownloadSize			- 다운로드 크기
//					ulExtraTime				- 남은 시간
//					ulLapseTime				- 경과 시간
// RETURN		:	None
// NOTE			:	다운로드 진행상황에 대한 정보를 얻는다.
//--------------------------------------------------------------------------------------------------//
void CDownEngineSDK::GetDownloadInfo(OUT ULONG* pulDownCompleteCount, 
	OUT ULONG* pulDownloadSpeed, OUT ULONGLONG* pullDownloadSize, OUT ULONG* pulExtraTime, 
	OUT ULONG* pulLapseTime)
{
	LPFNGETDOWNLOADINFO	lpfnGetDownloadInfo = (LPFNGETDOWNLOADINFO)GetProcAddress(m_hDownEngineAPI, "NOWCDN_GetDownloadInfo");
	lpfnGetDownloadInfo(pulDownCompleteCount, pulDownloadSpeed, pullDownloadSize, pulExtraTime, pulLapseTime);
}




void CDownEngineSDK::SetParentWindow(IN void* pParentWindow)
{
	LPFNSETPARENTWINDOW lpfnSetParentWindow = (LPFNSETPARENTWINDOW)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetParentWindow");
	lpfnSetParentWindow(pParentWindow);
}

void CDownEngineSDK::SetDrawFileInfoCallback(IN void* pfnDraw)
{
	LPFNSETDRAWFILEINFO lpfnSetDrawFileInfo = (LPFNSETDRAWFILEINFO)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetDrawFileInfoCallback");
	lpfnSetDrawFileInfo(pfnDraw);
}

void CDownEngineSDK::SetNotifyDownStartCallBack(IN void* pfnDraw)
{
	LPFNUPDATESTARTBUTTON lpfnUpdateStartButton = (LPFNUPDATESTARTBUTTON)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetUpdateStartButtonCallBack");
	lpfnUpdateStartButton(pfnDraw);
}

void CDownEngineSDK::SetNotifyDownCompleteCallBack(IN void* pfnDraw)
{
	LPFNNOTIFYDOWNCOMPLETE lpfnNotifyDownComplete = (LPFNNOTIFYDOWNCOMPLETE)GetProcAddress(m_hDownEngineAPI, "NOWCDN_SetNotifyDownCompleteCallBack");
	lpfnNotifyDownComplete(pfnDraw);
}



