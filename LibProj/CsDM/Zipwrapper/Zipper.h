// Zipper.h: interface for the CZipper class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _DM_USE_COMPRESSZIP_

#if !defined(AFX_ZIPPER_H__4249275D_B50B_4AAE_8715_B706D1CA0F2F__INCLUDED_)
#define AFX_ZIPPER_H__4249275D_B50B_4AAE_8715_B706D1CA0F2F__INCLUDED_

//#if _MSC_VER > 1000
#pragma once
//#endif // _MSC_VER > 1000


namespace DmCS
{
	struct Z_FileInfo
	{
		int nFileCount;
		int nFolderCount;
		DWORD dwUncompressedSize;
	};


	class CZipper  
	{
	public:
		CZipper(LPCSTR szFilePath = NULL, LPCSTR szRootFolder = NULL, bool bAppend = false);
		virtual ~CZipper();

		// simple interface
		static bool ZipFile(LPCSTR szFilePath); // saves as same name with .zip
		static bool ZipFolder(LPCSTR szFilePath, bool bIgnoreFilePath); // saves as same name with .zip

		// works with prior opened zip
		bool AddFileToZip(LPCSTR szFilePath, bool bIgnoreFilePath = false);
		bool AddFileToZip(LPCSTR szFilePath, LPCSTR szRelFolderPath); // replaces path info from szFilePath with szFolder
		bool AddFolderToZip(LPCSTR szFolderPath, bool bIgnoreFilePath = false);

		// extended interface
		bool OpenZip(LPCSTR szFilePath, LPCSTR szRootFolder = NULL, bool bAppend = false);
		bool CloseZip(); // for multiple reuse
		void GetFileInfo(Z_FileInfo& info);
	
	protected:
		void* m_uzFile;
		char m_szRootFolder[MAX_PATH + 1];
		Z_FileInfo m_info;

	protected:
		void PrepareSourcePath(LPSTR szPath);
	};
}
#endif // !defined(AFX_ZIPPER_H__4249275D_B50B_4AAE_8715_B706D1CA0F2F__INCLUDED_)
#endif