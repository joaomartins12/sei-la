#pragma once
#include "afxwin.h"


// _DlgTab_Text 대화 상자입니다.

class _DlgTab_Text : public CDialog
{
	DECLARE_DYNAMIC(_DlgTab_Text)


public:
	void			_Init();
	void			_SetQuest( cQuestObj* pQuestObj );




public:
	virtual void	OnOK(){}
	virtual void	OnCancel(){}


public:
	_DlgTab_Text(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgTab_Text();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TAB_TEXT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CEdit m_edit_UniqID;
	CEdit m_edit_Title;
	CEdit m_edit_Tab;
	CEdit m_edit_Body;
	CEdit m_edit_BodyLen;
	CEdit m_edit_Ing;
public:
	afx_msg void OnEnChangeEditBody();
	afx_msg void OnEnChangeEditIng();
protected:
	CEdit m_edit_IngLen;
	CEdit m_edit_Comp;
	CEdit m_edit_CompLen;
	CEdit m_edit_Ext;
	CEdit m_edit_ExtLen;
	CEdit m_edit_Simple;
	CEdit m_edit_SimpleLen;
	CEdit m_edit_Helper;
	CEdit m_edit_HelperLen;
public:
	afx_msg void OnEnChangeEditComp();
	afx_msg void OnEnChangeEditExt();
	afx_msg void OnEnChangeEditSimple();
	afx_msg void OnEnChangeEditHelper();
	afx_msg void OnEnChangeEditTitle();
	afx_msg void OnEnChangeEditTab();
};
