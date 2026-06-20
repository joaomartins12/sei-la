#pragma once
#include "afxwin.h"


// _DlgExtra_Quest 대화 상자입니다.

class _Dlg_Extra_Quest : public CDialog
{
public:
	void			_Init();
	void			_SetData( sNPC_EXTRA_QUEST* pExtra );
	void			_ResetActList( bool bKeepSelect );


protected:
	void			__SetAct( sNPC_EXTRA_QUEST::sACT* pAct );


protected:
	sNPC_EXTRA_QUEST*			m_pExtraData;
	sNPC_EXTRA_QUEST::sACT*		m_pSelectAct;








protected:
	DECLARE_DYNAMIC(_Dlg_Extra_Quest)

public:
	_Dlg_Extra_Quest(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_Dlg_Extra_Quest();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_EXTRA_QUEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CComboBox m_cmb_Act;
public:
	afx_msg void OnBnClickedButtonAdd();
protected:
	CComboBox m_cmb_InitAct;
public:
	afx_msg void OnCbnSelchangeComboInitact();
protected:
	CListBox m_list_Act;
	CListBox m_list_Quest;
	CEdit m_edit_QuestID;
public:
	afx_msg void OnEnChangeEditQuestid();
protected:
	CEdit m_edit_QuestStr;
public:
	afx_msg void OnLbnSelchangeListAct();
protected:
	CButton m_btn_Table;
	CButton m_btn_AddQuest;
public:
	afx_msg void OnBnClickedButtonDel();
	afx_msg void OnBnClickedButtonTable();
	afx_msg void OnBnClickedButtonAddQuest();
protected:
	CButton m_btn_DelQuest;
public:
	afx_msg void OnBnClickedButtonDelQuest();
protected:
	CStatic m_static_QuestDisc;
	CComboBox m_cmb_ActType;
};
