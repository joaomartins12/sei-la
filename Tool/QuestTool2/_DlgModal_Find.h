#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgModal_Find 대화 상자입니다.

class _DlgModal_Find : public CDialog
{
	DECLARE_DYNAMIC(_DlgModal_Find)

protected:

protected:
	bool			__FIndTitleText( CsQuest* pQuest, TCHAR* sz );
	bool			__FIndBody( CsQuest* pQuest, TCHAR* sz );
	bool			__FIndHelper( CsQuest* pQuest, TCHAR* sz );
	bool			__FIndStartTarget( CsQuest* pQuest, TCHAR* sz );
	bool			__FIndCompTarget( CsQuest* pQuest, TCHAR* sz );


	void			__InsertList( CsQuest* pQuest );

public:
	virtual void	OnOK(){}


public:
	_DlgModal_Find(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgModal_Find();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_FIND };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CComboBox m_combo_FindType;
public:
	afx_msg void OnBnClickedButtonDofind();
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	CEdit m_edit_FindStr;
	CListCtrl m_list_Find;
public:
	afx_msg void OnNMDblclkListFind(NMHDR *pNMHDR, LRESULT *pResult);
};
