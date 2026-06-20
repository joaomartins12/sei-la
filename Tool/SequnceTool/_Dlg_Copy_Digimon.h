#pragma once
#include "afxwin.h"


// _Dlg_Copy_Digimon 대화 상자입니다.

enum COPY_RESUTL
{
	SUCCESS,
	FAIL_NOT_SELECT_SRC_DIGIMON_PATH,
	SUCCESS_SELECT_SRC_DIGIMON_PATH,
	FAIL_WRONG_SELECT_SRC_DIGIMON_PATH,
	SUCCESS_COPY_FOLDER,
	DUPLICATE_KFM,
	DUPLICATE_NIF,
	NO_NIFFILE,
	ISCOPYABLE,
	FAIL_COPY_FOLDER,
	SUCCESS_RENAME_FILES,
	WRONG_KMF_FILE_NAME,
	DUPLICATED_DIGIMON_ID,
	WRONG_BASE_DIGIMONID,
	DUPLICATED_NEW_DIGIMONID,
	SUCCESS_COPY_FINAL,
	FAIL_COPY_FINAL,

};

class _Dlg_Copy_Digimon : public CDialog
{
	DECLARE_DYNAMIC(_Dlg_Copy_Digimon)

public:
	_Dlg_Copy_Digimon(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_Dlg_Copy_Digimon();

// 대화 상자 데이터입니다.
	enum { IDD = IDD__DLG_COPY_DIGIMON };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonOpenDialog();
	afx_msg void OnBnClickedButtonCreateDigimonFromBase();
	virtual BOOL OnInitDialog();
	CString GetSelectFolderPath();

	COPY_RESUTL SetSrcDigimonInfo(CString Path);
	void	SetDigimonPath(void);
	COPY_RESUTL CheckingCopyable(void);
	COPY_RESUTL CopyDigimonFolder(void);
	COPY_RESUTL RenameAnimFiles(void);
	COPY_RESUTL CopyDigimonData(bool ForceCopy = false);
	int InitBasePath(void);
private:
	void OutputResultMessage(COPY_RESUTL eResult);
		


	CEdit mEB_SrcPath;
	CEdit mEB_SrcDigimonID;
	CEdit mEB_SrcName;
	CEdit mEBDestDigimonID;
	CEdit mEBDestDigimonName;

	CString ObsoluteDigimonPath;
	CString RelateveDigimonPath;
	CString SrcPath;

	CString CopyNewPath;
	CString CopyFromPath;
	CString mDestID;
	CString mSrcID;
	CString mDestName;
	CString mSrcName;
	int iSrcID;
	int iDestID;
};
