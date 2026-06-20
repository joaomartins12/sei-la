#pragma once

#ifndef __DOWNLOAD_MANAGER_
#define __DOWNLOAD_MANAGER_

#include <string>

namespace CsDownloadMng
{
	enum eResult
	{
		SUCCESS,
		FILE_DOWNLOAD_SIZE_NOT_SAME,
		INTERNET_NOT_CONNECT, // 인터넷 연결 실패
		FILE_DOWNLOAD_FAIL, // 파일이 없다
		INTERNET_STATUS_ERROR,
		FILE_SIZE_ZERO,
		FILE_SIZE_ZERO_HTTP_QUERY_CONTENT_LENGTH,
		FILE_SIZE_NOT_SAME,
		FILE_DOWNLOAD_ERROR_COUNT_OVER,
	};

	eResult	DownloadToString( std::string const& downloadUrl, std::string & outString );
	eResult	DownloadToStringWhile( std::string const& downloadUrl, std::string & outString );
}

#endif