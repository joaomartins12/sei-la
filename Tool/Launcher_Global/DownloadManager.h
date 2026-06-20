#pragma once

#ifndef __DOWNLOAD_MANAGER_
#define __DOWNLOAD_MANAGER_

#include <string>

namespace CsDownloadMng
{
	enum eResult
	{
		SUCCESS,
		INTERNET_NOT_CONNECT, // 인터넷 연결 실패
		FILE_DOWNLOAD_FAIL, // 파일이 없다
		INTERNET_STATUS_ERROR,
		FILE_CREATE_ERROR,
		FILE_SIZE_ZERO,
	};

	eResult	DownloadToString( CString const& downloadUrl, std::string & outString );
	eResult	DownloadToString( CString const& downloadUrl, CString& outString );
	eResult	DownloadToFile( CString const& downloadUrl, CString const & outFileName );
}

#endif