#include "stdafx.h"
#include "DmFileFn.h"
#include <io.h>

namespace DmCS
{
	namespace FileFn
	{

		bool IsSameFileExt(dm_string conref filename, dm_string conref checkExt)
		{
			dm_string name;
			dm_string ext;
			FindFileNameExt(filename.c_str(), name, ext);

			return StringFn::iCmp(ext, checkExt);
		}

		bool IsSameFileExt(dm_string_t conref filename, dm_string_t conref checkExt)
		{
			dm_string_t name;
			dm_string_t ext;
			FindFileNameExt(filename.c_str(), name, ext);

			return StringFn::iCmp(ext, checkExt);
		}

		void FindFileNameExt(char cptr pzPath, dm_string& fileName, dm_string& extName)
		{
			char szDrive[_MAX_DRIVE] = "";
			char szDir[_MAX_DIR] = "";
			char szFName[_MAX_FNAME] = "";
			char szExt[_MAX_EXT] = "";

			_splitpath_s(pzPath, szDrive, szDir, szFName, szExt);
			fileName = szFName;			extName = szExt;
		}

		void FindFileNameExt( wchar_t cptr pzPath, dm_string_t& fileName, dm_string_t& extName )
		{
			wchar_t szDrive[_MAX_DRIVE] = L"";
			wchar_t szDir[_MAX_DIR] = L"";
			wchar_t szFName[_MAX_FNAME] = L"";
			wchar_t szExt[_MAX_EXT] = L"";

			_wsplitpath_s(pzPath, szDrive, szDir, szFName, szExt);
			fileName = szFName;			extName = szExt;
		}

		dm_string FindFileName( char cptr pzPath )
		{
			char szDrive[_MAX_DRIVE] = "";
			char szDir[_MAX_DIR] = "";
			char szFName[_MAX_FNAME] = "";
			char szExt[_MAX_EXT] = "";

			_splitpath_s(pzPath, szDrive, szDir, szFName, szExt);
			return szFName;
		}

		dm_string_t FindFileName( wchar_t cptr pzPath )
		{
			wchar_t szDrive[_MAX_DRIVE] = L"";
			wchar_t szDir[_MAX_DIR] = L"";
			wchar_t szFName[_MAX_FNAME] = L"";
			wchar_t szExt[_MAX_EXT] = L"";

			_wsplitpath_s(pzPath, szDrive, szDir, szFName, szExt);
			return szFName;
		}

		dm_string FindFilePath( char cptr pzPath )
		{
			char szDrive[_MAX_DRIVE] = "";
			char szDir[_MAX_DIR] = "";
			char szFName[_MAX_FNAME] = "";
			char szExt[_MAX_EXT] = "";

			_splitpath_s(pzPath, szDrive, szDir, szFName, szExt);
			dm_string value(szDrive);	
			value.append( szDir );
			return value;
		}

		dm_string_t FindFilePath( wchar_t cptr pzPath )
		{
			wchar_t szDrive[_MAX_DRIVE] = L"";
			wchar_t szDir[_MAX_DIR] = L"";
			wchar_t szFName[_MAX_FNAME] = L"";
			wchar_t szExt[_MAX_EXT] = L"";

			_wsplitpath_s(pzPath, szDrive, szDir, szFName, szExt);

			dm_string_t value(szDrive);	
			value.append( szDir );
			return value;
		}

		//---------------------------------------------------------------------------
		bool IsExist(char cptr p_pcPath, char cptr p_pcFileName)
		{
			// path
			char cFullPath[MAX_PATH] = {0,};
			strcpy_s(cFullPath, sizeof(cFullPath), p_pcPath);
			strcat_s(cFullPath, sizeof(cFullPath), "\\");
			strcat_s(cFullPath, sizeof(cFullPath), p_pcFileName);

			// open
			FILE* file = NULL;
			fopen_s(&file, cFullPath, "r");
			bool bExists = (file != NULL);
			if(file)
				fclose(file);

			return bExists;
		}

		bool IsAccessFile( char cptr p_pcPath )
		{
			errno_t erro = 0;
			erro = _access_s( p_pcPath, 0 );
			if( erro == 0 )
				return true;
			return false;
		}
		
		bool IsAccessFile( wchar_t cptr p_pcPath )
		{
			errno_t erro = 0;
			erro = _waccess_s( p_pcPath, 0 );
			if( erro == 0 )
				return true;
			return false;
		}

		//---------------------------------------------------------------------------
		dm_string GetFilePath(char cptr p_pcFileName, eStringUpperType eCase)
		{
			dm_string strname(p_pcFileName);
			if(strname.empty()) 
				return strname;

			dm_string::size_type uiPos = strname.find_last_of("/\\");

			if (uiPos != dm_string::npos)
				strname = strname.substr(0, uiPos + 1);
			else
				strname = "";

			switch( eCase )
			{
			case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
			case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
			}

			return strname;
		}
		//---------------------------------------------------------------------------
		dm_string GetFileFullName(char cptr p_pcFileName, eStringUpperType eCase)
		{
			dm_string strname(p_pcFileName);
			if(strname.empty()) 
				return strname;

			dm_string::size_type uiPos = strname.find_last_of("/\\");

			if (uiPos != dm_string::npos)
				strname = strname.substr(uiPos + 1);

			switch( eCase )
			{
			case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
			case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
			}

			return strname;
		}
		//---------------------------------------------------------------------------
		dm_string  GetFileName(char cptr p_pcFileName, eStringUpperType eCase)
		{
			dm_string strname = GetFileFullName(p_pcFileName, eCase);
			if(strname.empty()) 
				return strname;

			dm_string::size_type uiPos = strname.find_last_of('.');

			if (uiPos != dm_string::npos)
				strname = strname.substr(0, uiPos);

			switch( eCase )
			{
			case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
			case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
			}

			return strname;
		}
		//---------------------------------------------------------------------------
		dm_string GetFileExt(char cptr p_pcFileName, eStringUpperType eCase)
		{
			dm_string strname = GetFileFullName(p_pcFileName, eCase);
			if(strname.empty()) 
				return strname;

			dm_string::size_type uiPos = strname.find_last_of('.');

			if (uiPos != dm_string::npos)
				strname = strname.substr(uiPos + 1);
			else
				strname = "";

			switch( eCase )
			{
			case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
			case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
			}

			return strname;
		}
		//---------------------------------------------------------------------------
		void GatherFileList(char cptr p_pcPath, char cptr p_pcFileExt, bool p_bFolder, bool p_bIncludePath, dm_stringlist& p_vecFileList, eStringUpperType eCase)
		{
			WIN32_FIND_DATAA wfd;
			bool bFolder = false;
			char cPath[MAX_PATH] = {0,};
			char cPathFilter[MAX_PATH] = {0,};

			strcpy_s(cPath, sizeof(cPath), p_pcPath);
			strcat_s(cPath, sizeof(cPath), "\\");
			strcpy_s(cPathFilter, sizeof(cPathFilter), p_pcPath);
			strcat_s(cPathFilter, sizeof(cPathFilter), "\\*.*");

			HANDLE hSrch = FindFirstFileA(cPathFilter, &wfd);
			if( INVALID_HANDLE_VALUE != hSrch )
			{
				do
				{
					bFolder = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? true : false;

					if(bFolder)
					{
						// 폴더리스트
						if(	_stricmp(wfd.cFileName, ".") != 0 &&
							_stricmp(wfd.cFileName, "..") != 0 &&
							_stricmp(wfd.cFileName, ".svn") != 0 )
						{
							if(p_bFolder)
							{
								char cNextPath[MAX_PATH] = {0,};
								strcpy_s(cNextPath, sizeof(cNextPath), p_pcPath);
								strcat_s(cNextPath, sizeof(cNextPath), "\\");
								strcat_s(cNextPath, sizeof(cNextPath), wfd.cFileName);
								GatherFileList( cNextPath, p_pcFileExt, p_bFolder, p_bIncludePath, p_vecFileList, eCase );
							}
						}
					}
					else
					{
						if(!bFolder)
						{
							if(	_stricmp(wfd.cFileName, "thumbs.db") != 0 )
							{
								dm_string strname;
								if( p_bIncludePath )
								{
									strname = cPath; 
									strname += wfd.cFileName;
								}
								else
									strname = wfd.cFileName;

								switch( eCase )
								{
								case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
								case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
								}

								if( p_pcFileExt )
								{
									dm_string strExt = GetFileExt(strname.c_str());
									if(0 == _stricmp(strExt.c_str(), p_pcFileExt) )
										p_vecFileList.push_back(strname);
								}
								else
									p_vecFileList.push_back(strname);
							}							
						}
					}

				}while( TRUE == FindNextFileA(hSrch, &wfd) );
			}
			FindClose(hSrch);
		}

		//---------------------------------------------------------------------------
		bool IsExist(wchar_t cptr p_pcPath, wchar_t cptr p_pcFileName)
		{
			// path
			wchar_t cFullPath[MAX_PATH] = {0,};
			wcscpy_s(cFullPath, sizeof(cFullPath), p_pcPath);			
			wcscat_s(cFullPath, sizeof(cFullPath), L"\\");
			wcscat_s(cFullPath, sizeof(cFullPath), p_pcFileName);

			// open
			FILE* file = NULL;
			_wfopen_s(&file, cFullPath, L"r");
			bool bExists = (file != NULL);
			if(file)
				fclose(file);

			return bExists;
		}
		//---------------------------------------------------------------------------
		dm_string_t GetFilePath(wchar_t cptr p_pcFileName, eStringUpperType eCase)
		{
			dm_string_t strname(p_pcFileName);
			if(strname.empty()) 
				return strname;

			dm_string_t::size_type uiPos = strname.find_last_of(L"/\\");

			if (uiPos != dm_string_t::npos)
				strname = strname.substr(0, uiPos + 1);
			else
				strname = L"";

			switch( eCase )
			{
			case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
			case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
			}

			return strname;
		}
		//---------------------------------------------------------------------------
		dm_string_t GetFileFullName(wchar_t cptr p_pcFileName, eStringUpperType eCase)
		{
			dm_string_t strname(p_pcFileName);
			if(strname.empty()) 
				return strname;

			dm_string_t::size_type uiPos = strname.find_last_of(L"/\\");

			if (uiPos != dm_string_t::npos)
				strname = strname.substr(uiPos + 1);
			switch( eCase )
			{
			case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
			case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
			}

			return strname;
		}
		//---------------------------------------------------------------------------
		dm_string_t  GetFileName(wchar_t cptr p_pcFileName, eStringUpperType eCase)
		{
			dm_string_t strname = GetFileFullName(p_pcFileName, eCase);
			if(strname.empty()) 
				return strname;

			dm_string_t::size_type uiPos = strname.find_last_of(L'.');

			if (uiPos != dm_string_t::npos)
				strname = strname.substr(0, uiPos);
			switch( eCase )
			{
			case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
			case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
			}

			return strname;
		}
		//---------------------------------------------------------------------------
		dm_string_t GetFileExt(wchar_t cptr p_pcFileName, eStringUpperType eCase)
		{
			dm_string_t strname = GetFileFullName(p_pcFileName, eCase);
			if(strname.empty()) 
				return strname;

			dm_string_t::size_type uiPos = strname.find_last_of(L'.');

			if (uiPos != dm_string_t::npos)
				strname = strname.substr(uiPos + 1);
			else
				strname = L"";

			switch( eCase )
			{
			case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
			case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
			}

			return strname;
		}
		//---------------------------------------------------------------------------
		void GatherFileList(wchar_t cptr p_pcPath, wchar_t cptr p_pcFileExt, bool p_bFolder, bool p_bIncludePath, dm_stringlist_t& p_vecFileList, eStringUpperType eCase)
		{
			WIN32_FIND_DATAW wfd;
			bool bFolder = false;
			wchar_t cPath[MAX_PATH] = {0,};
			wchar_t cPathFilter[MAX_PATH] = {0,};

			wcscpy_s(cPath, sizeof(cPath), p_pcPath);
			wcscat_s(cPath, sizeof(cPath), L"\\");
			wcscpy_s(cPathFilter, sizeof(cPathFilter), p_pcPath);
			wcscat_s(cPathFilter, sizeof(cPathFilter), L"\\*.*");

			HANDLE hSrch = FindFirstFileW(cPathFilter, &wfd);
			if( INVALID_HANDLE_VALUE != hSrch )
			{
				do
				{
					bFolder = wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ? true : false;

					if(bFolder)
					{
						// 폴더리스트
						if(	_wcsicmp(wfd.cFileName, L".") != 0 &&
							_wcsicmp(wfd.cFileName, L"..") != 0 &&
							_wcsicmp(wfd.cFileName, L".svn") != 0 )
						{
							if(p_bFolder)
							{
								wchar_t cNextPath[MAX_PATH] = {0,};
								wcscpy_s(cNextPath, sizeof(cNextPath), p_pcPath);
								wcscat_s(cNextPath, sizeof(cNextPath), L"\\");
								wcscat_s(cNextPath, sizeof(cNextPath), wfd.cFileName);
								GatherFileList( cNextPath, p_pcFileExt, p_bFolder, p_bIncludePath, p_vecFileList, eCase );
							}
						}
					}
					else
					{
						if(!bFolder)
						{
							if(	_wcsicmp(wfd.cFileName, L"thumbs.db") != 0 )
							{
								dm_string_t strname;
								if( p_bIncludePath )
								{
									strname = cPath;
									strname += wfd.cFileName;
								}
								else
									strname = wfd.cFileName;

								switch( eCase )
								{
								case eUpper: std::transform(strname.begin(), strname.end(), strname.begin(), toupper); break;
								case eLower: std::transform(strname.begin(), strname.end(), strname.begin(), tolower); break;
								}

								if( p_pcFileExt )
								{
									dm_string_t strExt = GetFileExt(strname.c_str());
									if( 0 == _wcsicmp( strExt.c_str(), p_pcFileExt ) )
										p_vecFileList.push_back(strname);
								}
								else
									p_vecFileList.push_back(strname);
							}
						}
					}

				}while( TRUE == FindNextFileW(hSrch, &wfd) );
			}
			FindClose(hSrch);
		}

		bool IsBmpIntegrityCheck( char cptr p_pcPath )
		{
			bool bResult = false;
			if( NULL == p_pcPath )
				return bResult;

			FILE* fp = NULL;
			fopen_s( &fp, p_pcPath, "rb" );
			if( NULL == fp )
				return bResult;

			BITMAPFILEHEADER bh;
			fread( &bh, sizeof(BITMAPFILEHEADER), 1, fp );
			if( bh.bfType == 0x4D42 )
			{
				fseek(fp, 0, SEEK_END);
				DWORD nSize = ftell(fp);
				if( nSize == bh.bfSize )
					bResult = true;
			}

			fclose( fp );
			
			return bResult;
		}

		bool GetFileVersionInfos( char cptr p_pcPath, dm_word_t & majorVer, dm_word_t& minorVer, dm_word_t& buildNum, dm_word_t& revision )
		{
			majorVer = 0;			minorVer = 0;			buildNum = 0;			revision = 0;

			DWORD dwHandle;
			// 파일로부터 버전정보데이터의 크기가 얼마인지를 구합니다.
			DWORD infoSize = GetFileVersionInfoSizeA(p_pcPath, &dwHandle);
			if(infoSize==0) 
				return false;

			// 버퍼할당
			char* buffer = new char[infoSize];
			if(buffer)
			{
				// 버전정보데이터를 가져옵니다.
				if(GetFileVersionInfoA(p_pcPath,dwHandle, infoSize, buffer)!=0)
				{
					VS_FIXEDFILEINFO* pFineInfo;
					UINT bufLen = 0;
					// buffer로 부터 VS_FIXEDFILEINFO 정보를 가져옵니다.
					if(VerQueryValueA(buffer,"\\",(LPVOID*)&pFineInfo, &bufLen) !=0)
					{    
						majorVer = HIWORD(pFineInfo->dwFileVersionMS);
						minorVer = LOWORD(pFineInfo->dwFileVersionMS);
						buildNum = HIWORD(pFineInfo->dwFileVersionLS);
						revision = LOWORD(pFineInfo->dwFileVersionLS);
					}
				}
				delete[] buffer;
			}

			return true;
		}

		bool GetFileVersionInfos( wchar_t cptr p_pcPath, dm_word_t & majorVer, dm_word_t& minorVer, dm_word_t& buildNum, dm_word_t& revision )
		{
			majorVer = 0;			minorVer = 0;			buildNum = 0;			revision = 0;
			DWORD dwHandle;
			// 파일로부터 버전정보데이터의 크기가 얼마인지를 구합니다.
			DWORD infoSize = GetFileVersionInfoSizeW(p_pcPath, &dwHandle);
			if(infoSize==0) 
				return false;

			// 버퍼할당
			char* buffer = new char[infoSize];
			if(buffer)
			{
				// 버전정보데이터를 가져옵니다.
				if(GetFileVersionInfoW(p_pcPath,dwHandle, infoSize, buffer)!=0)
				{
					VS_FIXEDFILEINFO* pFineInfo;
					UINT bufLen = 0;
					// buffer로 부터 VS_FIXEDFILEINFO 정보를 가져옵니다.
					if(VerQueryValueW(buffer,L"\\",(LPVOID*)&pFineInfo, &bufLen) !=0)
					{    
						majorVer = HIWORD(pFineInfo->dwFileVersionMS);
						minorVer = LOWORD(pFineInfo->dwFileVersionMS);
						buildNum = HIWORD(pFineInfo->dwFileVersionLS);
						revision = LOWORD(pFineInfo->dwFileVersionLS);
					}
				}
				delete[] buffer;
			}

			return true;
		}
	}
}