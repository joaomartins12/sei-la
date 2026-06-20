#pragma once


typedef std::map<std::string,std::string>					MAP_HASH_CONTANER;
typedef std::map<std::string,std::string>::iterator			MAP_HASH_CONTANER_IT;
typedef std::map<std::string,std::string>::const_iterator	MAP_HASH_CONTANER_CIT;

// CIntegrityTool 대화 상자입니다.

class CIntegrityTool : public CDialog
{
	DECLARE_DYNAMIC(CIntegrityTool)

public:
	CIntegrityTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CIntegrityTool();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_INTEGRITY_DIALOG };

	virtual BOOL OnInitDialog();
	void		SetMakeHashDirFile( std::wstring const& hashDirFile, std::wstring const& makeHashDataFolder, std::wstring const& uploadFile);
	void		ChangeHashData(MAP_HASH_CONTANER const& changeHashData );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	DECLARE_MESSAGE_MAP()
	
private:
	bool		_LoadConfigeFile();
	void		_CreateUI();

	std::string _MakeHashDataToString( MAP_HASH_CONTANER const& saveData );
	bool		_CheckIncludeFile( dm_stringlist const& orgList, dm_stringlist& pResultList );
	bool		_SaveFile(std::string const& saveData, std::string const& saveFileName, bool bCompress = true);
	void		_MaekCheckData( std::string const& downloadData, MAP_HASH_CONTANER& saveData );
	void		_UpdateNewHashData( MAP_HASH_CONTANER const& newData, MAP_HASH_CONTANER& recvData );

	bool		_LoadFileList_PackFile( std::string const& sPackFile, dm_stringlist& saveData );
	bool		_LoadHashData_FromPackFile( std::string const& sPackFile, MAP_HASH_CONTANER& getHashData );
	bool		_LoadHashData_FromFolderFile( std::string const& rootPath, dm_stringlist const& sPackFile, MAP_HASH_CONTANER& getHashData );

	bool		_SendUpdateHashData();
	bool		_OnMakeHashData(std::wstring const& makePath);

	bool		_OnMakePatchAndHashData();
	void		_MakeClientExeHash( std::string const& makePath );
	CString		_FindFolder( CString const& RootDir, CString const& findFolder, bool bFindDownFolder = false );

	bool		_IsIncludeFolder( std::string const& strFolder, MAP_HASH_CONTANER const& hashData );
	void		_RemoveIncludeFolderData( std::string const& strFolder, MAP_HASH_CONTANER & hashData );


	void		_SetLastFolder();

private:
	void		_CreateHorizontalScroll(CListBox & listBox);
	bool		_FTPFileDownload( std::string const& sIp,			// 업로드 ftp IP
								  unsigned int const& nPort,		// 업로드 ftp Port
								  std::string const& sID,		// 업로드 ID
								  std::string const& sPW,	// 업로드 PW
								  std::string const& sPath,		// 업로드 폴더
								  std::string const& sDownloadFile,
								  std::string & downloadString);
	// URL 로부터 해쉬 데이터 파일 받음
	bool		_FTPFileUpload( std::string const& sIp,			// 업로드 ftp IP
								unsigned int const& nFtpPort,		// 업로드 ftp Port
								std::string const& userId,		// 업로드 ID
								std::string const& userPassword,	// 업로드 PW
								std::string const& ftpPath,		// 업로드 폴더
								std::string const& uploadFile );	// 업로드 파일


private:
	sConfige							m_ConfigeData;
	MAP_HASH_CONTANER					m_Result;
	std::wstring						m_makeDirFile;
	std::wstring						m_saveHashDirDir;
	std::string							m_uploadZipFile;
	std::string							m_ConfigFileName;

	std::string							m_ClientExeHashData;

private:
	//CEdit								m_FolderEdit;
	CEdit								m_IdEdit;
	CEdit								m_PasswordEdit;
	CEdit								m_PortEdit;
	CEdit								m_UploadUrlEdit;
	CEdit								m_UploadDirectoryEdit;
	CEdit								m_UploadHashFileNameEdit;
	CEdit								m_HashFileName;
	CEdit								m_CurrentDirEdit;	
	CEdit								m_Editbox_ClientExeFileName;
	CEdit								m_Editbox_ClientExeHashResult;


	CListBox							m_HashList;	
	CListBox							m_IncludeFolder;
	CListBox							m_IncludeExt;
	CListBox							m_IncludeFileList;

	CEdit								m_PatchNumberInput;

	//////////////////////////////////////////////////////////////////////////
	CEdit								m_ServerConnectID;
	CEdit								m_ServerConnectPW;
	CEdit								m_ServerConnectPort;
	CEdit								m_ServerRequestPort;
	CEdit								m_ServerConnectHost;
	CEdit								m_ServerDirectory;
	CEdit								m_ServerHashFileName;
	//////////////////////////////////////////////////////////////////////////
	CEdit								m_PatchUploadFTP_ID;
	CEdit								m_PatchUploadFTP_PW;
	CEdit								m_PatchUploadFTP_Port;
	CEdit								m_PatchUploadFTP_Host;
	CEdit								m_PatchUploadFTP_Path;
	CEdit								m_PatchUploadFTP_File;
	//////////////////////////////////////////////////////////////////////////
	CEdit								m_UserConnectionID;
	CEdit								m_UserConnectionPW;
	//////////////////////////////////////////////////////////////////////////
	CEdit								m_UserHashLogFTP_ID;
	CEdit								m_UserHashLogFTP_PW;
	CEdit								m_UserHashLogFTP_Port;
	CEdit								m_UserHashLogFTP_Host;
	CEdit								m_UserHashLogFTP_Path;
	CEdit								m_UserHashLogFTP_File;

public:
	afx_msg void OnBnClickedButtonChangedir();
	
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonMakeHash();
	
	afx_msg void OnBnClickedUpdateHashdata();
	afx_msg void OnBnClickedHashfileSave();
	afx_msg void OnLbnSelchangeFolder();
	afx_msg void OnBnClickedButtonReloadConfig();
	afx_msg void OnBnClickedButtonHashdataSave();
	afx_msg void OnBnClickedUpdataServerHash();
	afx_msg void OnBnClickedUpdataPatchData();
	afx_msg void OnBnClickedButton_PatchFileFind();
	afx_msg void OnEnChangeEditUploadurl();
	afx_msg void OnEnChangeEditUploadPort();
	afx_msg void OnEnChangeEditId();
	afx_msg void OnEnChangeEditPassword();
	afx_msg void OnEnChangeEditUploaddirectory();
	afx_msg void OnEnChangeEditUploadFile();
	afx_msg void OnEnChangeEditServerUrl();
	afx_msg void OnEnChangeEditServerPort();
	afx_msg void OnEnChangeEditServerRequestPort();
	afx_msg void OnEnChangeEditServerId();
	afx_msg void OnEnChangeEditServerpw();
	afx_msg void OnEnChangeEditServerHashfilename();
	afx_msg void OnEnChangeEditServerUploadFile();
	afx_msg void OnEnChangeEditPatchUploadHost();
	afx_msg void OnEnChangeEditPatchUploadPort();
	afx_msg void OnEnChangeEditPatchUploadId();
	afx_msg void OnEnChangeEditPatchUploadPw();
	afx_msg void OnEnChangeEditPatchUploadPath();
	afx_msg void OnEnChangeEditPatchUploadFile();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnEnChangeEditUserConnectionId();
	afx_msg void OnEnChangeEditUserConnectionPw();
	afx_msg void OnEnChangeEditHashlogHost();
	afx_msg void OnEnChangeEditHashlogPath();
	afx_msg void OnEnChangeEditHashlogPort();
	afx_msg void OnEnChangeEditHashlogId();
	afx_msg void OnEnChangeEditHashlogPw();
	afx_msg void OnEnChangeEditHashlogFilename();
	afx_msg void OnDropFiles(HDROP hDropInfo);

	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnBnClickedServerhashdataRef();
	afx_msg void OnBnClickedButtonResourceEdit();
};
