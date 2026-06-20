#pragma once

#ifndef DM_SYSTEM_FILE_FN_H
#define DM_SYSTEM_FILE_FN_H

namespace DmCS
{
	namespace FileFn
	{
		enum eStringUpperType
		{
			eUpper = -1,
			eNone = 0,
			eLower = 1
		};

		bool		IsSameFileExt(dm_string conref filename, dm_string conref checkExt);
		bool		IsSameFileExt(dm_string_t conref filename, dm_string_t conref checkExt);

		void		FindFileNameExt( char cptr pzPath, dm_string& fileName, dm_string& exeName );
		void		FindFileNameExt( wchar_t cptr pzPath, dm_string_t& fileName, dm_string_t& extName );
		dm_string	FindFileName( char cptr pzPath );
		dm_string_t FindFileName( wchar_t cptr pzPath );
		dm_string	FindFilePath( char cptr pzPath );
		dm_string_t FindFilePath( wchar_t cptr pzPath );
		bool		IsAccessFile( char cptr p_pcPath );
		bool		IsAccessFile( wchar_t cptr p_pcPath );

		bool		IsExist(char cptr p_pcPath, char cptr p_pcFileName);
		dm_string	GetFilePath( char cptr p_pcFileName, eStringUpperType eCase = eNone );
		dm_string	GetFileFullName(char cptr p_pcFileName, eStringUpperType eCase = eNone);
		dm_string	GetFileName(char cptr p_pcFileName, eStringUpperType eCase = eNone);
		dm_string	GetFileExt(char cptr p_pcFileName, eStringUpperType eCase = eNone);
		void		GatherFileList(char cptr p_pcPath, char cptr p_pcFileExt, bool p_bFolder, bool p_bIncludePath, dm_stringlist& p_vecFileList, eStringUpperType eCase= eNone);

		bool		IsExist(wchar_t cptr p_pcPath, wchar_t cptr p_pcFileName);
		dm_string_t GetFilePath(wchar_t cptr p_pcFileName, eStringUpperType eCase = eNone);
		dm_string_t GetFileFullName(wchar_t cptr p_pcFileName, eStringUpperType eCase= eNone);
		dm_string_t GetFileName(wchar_t cptr p_pcFileName, eStringUpperType eCase = eNone);
		dm_string_t GetFileExt(wchar_t cptr p_pcFileName, eStringUpperType eCase = eNone);
		void		GatherFileList(wchar_t cptr p_pcPath, wchar_t cptr p_pcFileExt, bool p_bFolder, bool p_bIncludePath, dm_stringlist_t& p_vecFileList, eStringUpperType eCase= eNone);

		bool		IsBmpIntegrityCheck( char cptr p_pcPath );

		bool		GetFileVersionInfos( char cptr p_pcPath, dm_word_t & majorVer, dm_word_t& minorVer, dm_word_t& buildNum, dm_word_t& revision );
		bool		GetFileVersionInfos( wchar_t cptr p_pcPath, dm_word_t & majorVer, dm_word_t& minorVer, dm_word_t& buildNum, dm_word_t& revision );
	}
}

#endif//DM_SYSTEM_FILE_FN_H
