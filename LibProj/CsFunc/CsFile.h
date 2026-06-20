
#pragma once
#include <string>
namespace nsCSFILE
{
	TCHAR*		ToLowerCase( TCHAR* In );
	TCHAR*		NewLine( TCHAR* Out, int nOutLen, TCHAR* In );

	LPCTSTR		GetFileExt( LPCTSTR In );
	LPCSTR		GetFileExt( LPCSTR In );
	LPCTSTR		GetFileName( LPCTSTR In );
	LPCSTR		GetFileName( LPCSTR In );

	LPTSTR		GetFilePathOnly( LPTSTR Out, int nOutLen, LPCTSTR In, bool bIncludeFileName =false );
	LPSTR		GetFilePathOnly( LPSTR Out, int nOutLen, LPCSTR In, bool bIncludeFileName =false );

	bool		IsSameString( LPCTSTR str1, LPCTSTR str2 );
	bool		IsSameString( LPCSTR str1, LPCSTR str2 );

	LPTSTR		GetNumberStr( LPTSTR Out, int nOutLen, int nInNumber );
	LPTSTR		GetNumberStr( LPTSTR Out, int nOutLen, LPCTSTR In );

	bool		DirectoryOpen(	LPTSTR		szOutPath,
								UINT		nOutPathLen,
								LPCTSTR		szInitDir	);

	bool		FileOpen(	LPTSTR		szOutPath,
							UINT		nOutPathLen,
							LPCTSTR		szFilter,
							LPCTSTR		szInitDir,
							LPTSTR		szOutFile		=NULL,
							UINT		nOutFileLen		=0,
							LPTSTR		szOutTitle		=NULL,
							UINT		nOutTitleLen	=0,
							DWORD		dwAddFlags		=0		);

	bool		FileSave(	LPTSTR		szOutPath,
							UINT		nOutPathLen,
							LPCTSTR		szFilter,
							LPCTSTR		szInitDir,
							LPTSTR		szOutFile		=NULL,
							UINT		nOutFileLen		=0,
							LPTSTR		szOutTitle		=NULL,
							UINT		nOutTitleLen	=0,
							DWORD		dwAddFlags		=0		);

	void		CreateDirectory( TCHAR* szRelativePath );
	void		CreateDirectory( char* cRelativePath );

	void		DeleteDirectory( TCHAR* szRelativePath );
	void		DeleteDirectoryA( LPCSTR szRelativePath );

	bool		IsSubDirectory( LPCTSTR szCheckPath );

	LPTSTR		GetRelativePath( LPTSTR szOutPath, int nOutSize, LPCTSTR szSubDirectory );
	char*		GetRelativePath( char* cOutPath, int nOutSize, char* cSubDirectory );

	LPCTSTR		GetSubPath( LPCTSTR szIn, LPCTSTR szSubDirectory );

	int			DeleteFolder( TCHAR* cFolder );	
	int			DeleteFolder( char const* cFolder );
	BOOL		DeleteFile(  TCHAR* szDelFile );
	BOOL		DeleteFileA(  char const* szDelFile );
	BOOL		DeletePathTillRoot( std::list<std::string> const& delFileList, LPCSTR pRootName );
	
	tm			Str2Time( char* sz, tm& tm );
	UINT		Tm2Sec( tm time );
	void		DirectoryFindFilesA( std::list<std::string>& findFileList, char const* pFindFolder );
	void		DirectoryFindFilesW( std::list<std::wstring>& findFileList, wchar_t const* pFindFolder );
	void		FindFilesA( char const* pFindFolder, std::list<std::string>& findFiles);
	

	struct sTOKEN_c
	{
		const char*	s_pSrc;
		int			s_nSrcBufferLen;
		int			s_nSrcBufferIndex;

		const char*	s_cStartApp;
		const char*	s_cEndApp;
		enum eAPP{
			APP_NONE, APP_NONE_START, APP_START, APP_END,
		};
		eAPP		s_AppState;
		

		sTOKEN_c( const char* cSrc );
		sTOKEN_c( const char* cSrc, const char* cStartApp, const char* cEndApp );
		char* Next( char* cOut, int nOutBufferLen );
		char* Next( char* cOut, int nOutBufferLen, const char* cKey, const char* cSeparation, size_t siGarbageSize = 0 );
	};

	struct sTOKEN
	{
		const TCHAR*	s_pSrc;
		int				s_nSrcBufferLen;
		int				s_nSrcBufferIndex;

		sTOKEN( const TCHAR* cSrc );
		TCHAR* Next( TCHAR* cOut, int nOutBufferLen );
		TCHAR* Next_NewLine( TCHAR* cOut, int nOutBufferLen );
		int    NextInt( TCHAR* cOut, int nOutBufferLen );
	};

	struct sFileName_c
	{
		int		GetLength(){ return MAX_FILENAME; }
		char	s_cFileName[ MAX_FILENAME ];
	};

	//========================================================================================================
	//
	//		Å¬¸³º¸µå
	//
	//========================================================================================================
	TCHAR*		GetClipboardFont( HWND hWnd, TCHAR* szReturn, int nReturnSize );
	void		SetClipboardFont( HWND hWnd, TCHAR* szInput, int nInputSize );
}