#pragma once
#include "afxwin.h"

class CFullPackDlg : public CDialog
{
public:
	// 대화 상자 데이터입니다.
	enum { IDD = IDD_FULLPACK_DIALOG };

	virtual void	OnOK(){}
	virtual void	OnCancel(){}

	void			OpenWindow( char const* pFolder);

	enum ePackBtn
	{
		eKOR_Pack_Btn = 0,
		eUSA_Pack_Btn,
		eAERIA_Pack_Btn,
		eSteam_Pack_Btn,
		eTHA_Pack_Btn,
		eTAI_Pack_Btn,
		eHK_Pack_Btn,
		ePatch_Pack_Btn,
		eZip_Btn,
		eMax
	};

	// 생성입니다.
public:
	CFullPackDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnInitData();
	afx_msg void OnLbnDblclkListSrcDir();
	afx_msg void OnBnClickedButtonAddDir();
	afx_msg void OnBnClickedButtonDelDir();
	afx_msg void OnBnClickedButtonToFile();
	afx_msg void OnBnClickedButtonDelPackfile();
	afx_msg void OnBnClickedButtonDelallPackfile();

	// Kor
	afx_msg void OnTableLoad_Kor();
	afx_msg void OnTablePacking_Kor();
	afx_msg void OnTableUnPacking_Kor();

	// Usa
	afx_msg void OnTableLoad_USA();
	afx_msg void OnTablePacking_USA();
	afx_msg void OnTableUnPacking_USA();

	afx_msg void OnTableLoad_AERIA();
	afx_msg void OnTablePacking_AERIA();
	afx_msg void OnTableUnPacking_AERIA();

	afx_msg void OnTableLoad_STEAM();
	afx_msg void OnTablePacking_STEAM();
	afx_msg void OnTableUnPacking_STEAM();
	
	// Thailand
	afx_msg void OnTableLoad_Thai();
	afx_msg void OnTablePacking_Thai();
	afx_msg void OnTableUnPacking_Thai();

	// TAIWAN
	afx_msg void OnTableLoad_Tw();
	afx_msg void OnTablePacking_Tw();
	afx_msg void OnTableUnPacking_Tw();

	// HONGKONG
	afx_msg void OnTableLoad_Hk();
	afx_msg void OnTablePacking_Hk();
	afx_msg void OnTableUnPacking_Hk();

	// 패치 데이터 ?
	afx_msg void OnPatchDataLoad();
	// 패치 팩킹
	afx_msg void OnDoPatchPacking();
	// 패치 파일 언팩킹
	afx_msg void OnDoPatchUnPacking();
	// 압축하기
	afx_msg void OnDoZip();

	// 리소스 풀 언팩
	afx_msg void OnResourceUnPacking();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.
	virtual void PostNcDestroy();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	HICON m_hIcon;

private:
	void			__Dir( TCHAR* szPathOnly );
	void			__SetStatusText( LPTSTR szText , ... );

	void			__ToPackFileList_Dir( TCHAR* szDir );
	void			__ToPackFileList_File( TCHAR* szFile );


	bool			DoFilePacking( std::string const& sPackFile, std::list<std::string> const& fileList );
	void			DoUnPacking( std::string const& pUnPackFileName );

	void			EnableAllPackBtn( BOOL bValue );
	void			EnableOnlyPackBtn(ePackBtn eBtnType);
	void			EnablePackBtn( ePackBtn eBtnType , BOOL bValue);
	bool			CheckException( TCHAR const* pName );
	LPTSTR			GetRelativePath( LPTSTR szOutPath, int nOutSize, LPCTSTR szSubDirectory );
	CString			GetPatchFolderName( CString const& RootDir, int nPatchNum );
	void			MoveFolder(CString OldPath, CString NewPath);

	void			_OnDoPatchPacking(CString& hashPath, CString& PackFile);
	bool			_DoHashDataTool( TCHAR* cCommand );

private:
	CListBox	m_list_SrcDir;
	CListBox	m_list_DestDir;
	CEdit		m_edit_PackID_Start;
	CEdit		m_edit_PackID_End;
	CListBox	m_list_PackFile;
	CEdit		m_edit_AddFile_Cnt;
	CEdit		m_edit_AddFile_Size;
	CString		m_strCurDir;
	CString		m_strRootDir;
	int			m_nAddFileCount;
	UINT64		m_nAddFileTSize;

	CButton		m_PackBtn[eMax];	
	CButton		m_ZipBtn;

	std::list<std::wstring> m_ExceptionString;
public:
	afx_msg void OnBnClickedFullPack();
};
