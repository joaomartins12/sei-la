#pragma once
#include "afxwin.h"


// _DlgTab_Setting 대화 상자입니다.

class _DlgTab_Setting : public CDialog
{
	DECLARE_DYNAMIC(_DlgTab_Setting)



public:
	void			_Init();
	void			_SetQuest( cQuestObj* pQuestObj );


	// 시작 타겟
protected:
	void			__SetStartTarget( cQuestObj* pQuestObj );

	// 완료 타겟
protected:
	void			__SetCompTarget( cQuestObj* pQuestObj );

	// 수행지급
protected:
	CsQuestApplyRequite		m_ApplyRequite;
protected:
	void			__SetApplyRequite( cQuestObj* pQuestObj );
	void			__ReadAPList( CsQuest* pFTQuest );

	// 선행 조건
protected:
	CsQuestPreRequire		m_PreRequire;
protected:
	void			__SetPreRequire( cQuestObj* pQuestObj );
	void			__ReadPreRequireList( CsQuest* pFTQuest );

	// 완료 조건
protected:
	CsQuestCompRequire		m_CompRequire;
protected:
	void			__SetCompRequire( cQuestObj* pQuestObj );
	void			__ReadCompRequireList( CsQuest* pFTQuest );

	// 보상
protected:
	CsQuestRequite			m_Requite;
protected:
	void			__SetRequite( cQuestObj* pQuestObj );
	void			__ReadRequiteList( CsQuest* pFTQuest );

public:
	virtual void	OnOK(){}
	virtual void	OnCancel(){}



public:
	_DlgTab_Setting(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgTab_Setting();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DIALOG_TAB_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CComboBox m_combo_MainType;
	CEdit m_edit_UniqID;
	CEdit m_edit_MngID;
	CComboBox m_combo_StartTarget;
	CEdit m_edit_Title;
	CEdit m_edit_ST_ID;
	CEdit m_edit_ST_Cnt;
	CButton m_btn_ST_Table;
public:
	afx_msg void OnCbnSelchangeComboStartTarget();
protected:
	CEdit m_edit_ST_Discript;
public:
	afx_msg void OnBnClickedButtonStTable();
	afx_msg void OnEnChangeEditStId();
protected:
	CComboBox m_combo_AP;
	CEdit m_edit_AP_ID;	
	CButton m_btn_AP_Table;
	CEdit m_edit_AP_Cnt;
public:
	CEdit m_edit_AP_Discript;
protected:
	CListBox m_list_AP_List;
public:
	afx_msg void OnBnClickedButtonApAdd();
	afx_msg void OnBnClickedButtonApDel();
	afx_msg void OnCbnSelchangeComboAp();
	afx_msg void OnBnClickedButtonApTable();
	afx_msg void OnEnChangeEditApId();
protected:
	CButton m_btn_AP_Add;
	CButton m_btn_AP_Del;
public:
	afx_msg void OnEnChangeEditApCnt();
protected:
	CComboBox m_combo_PreReq;
	CEdit m_edit_PreReqID;
	CButton m_btn_PreReqTable;
	CEdit m_edit_PreReqCnt;
	CEdit m_edit_PreReqDiscript;
	CButton m_btn_PreReqAdd;
	CButton m_btn_PreReqDel;
	CListBox m_list_PreReqList;
public:
	afx_msg void OnCbnSelchangeComboPrereq();
	afx_msg void OnEnChangeEditPrereqId();
	afx_msg void OnBnClickedButtonPrereqTable();
	afx_msg void OnBnClickedButtonPrereqAdd();
	afx_msg void OnBnClickedButtonPrereqDel();
	afx_msg void OnEnChangeEditPrereqCnt();
protected:
	CComboBox m_combo_CompRequ;
	CEdit m_edit_CompRequ_ID;
	CButton m_btn_CompRequ_Table;
	CEdit m_edit_CompRequ_Cnt;
	CEdit m_edit_CompRequ_Discript;
	CButton m_btn_CompRequ_Add;
	CButton m_btn_CompRequ_Del;
	CListBox m_list_CompRequ_List;
public:
	afx_msg void OnCbnSelchangeComboCompRequ();
	afx_msg void OnEnChangeEditCompRequId();
	afx_msg void OnEnChangeEditCompRequCnt();
	afx_msg void OnBnClickedButtonCompRequTable();
	afx_msg void OnBnClickedButtonCompRequAdd();
	afx_msg void OnBnClickedButtonCompRequDel();
	afx_msg void OnClickCompRequListBoxItem();
protected:
	CEdit m_edit_CompRequTarget_ID;
	CButton m_btn_CompRequTarget_Table;
public:
	afx_msg void OnEnChangeEditCompRequTargetId();
	afx_msg void OnBnClickedButtonCompRequTargetTable();
	afx_msg void OnCbnSelchangeComboCt();
	afx_msg void OnEnChangeEditCtId();
	afx_msg void OnBnClickedButtonCtTable();
	afx_msg void OnEnChangeEditCompRequSubValue();
	afx_msg void OnEnChangeEditCompRequSubValue2();
protected:
	CComboBox m_combo_CT;
	CEdit m_edit_CT_ID;
	CButton m_btn_CT_Table;
	CEdit m_edit_CT_Discript;
	CComboBox m_combo_Reqt_Method;
	CComboBox m_combo_Reqt_Type;
	CEdit m_edit_Reqt_ID;
	CButton m_btn_Reqt_Table;
	CEdit m_edit_Reqt_Cnt;
	CEdit m_edit_Reqt_Discript;
	CButton m_btn_Reqt_Add;
	CListBox m_list_Reqt_List;
	CButton m_btn_Reqt_Del;

	CEdit m_edit_Reqt_SubValue1;
	CEdit m_edit_Reqt_SubValue2;

public:
	afx_msg void OnCbnSelchangeComboReqtType();
	afx_msg void OnEnChangeEditReqtId();
	afx_msg void OnEnChangeEditReqtCnt();
	afx_msg void OnBnClickedButtonReqtTable2();
	afx_msg void OnBnClickedButtonReqtAdd();
	afx_msg void OnBnClickedButtonReqtDel();
protected:
	CEdit m_edit_Level;
public:
	afx_msg void OnEnChangeEditExpLevel();
	afx_msg void OnEnChangeEditMngid();
	afx_msg void OnEnChangeEditModelID_Recv();
	afx_msg void OnEnChangeEditModelID_Comp();
	afx_msg void OnCbnSelchangeComboMaintype();
protected:
	CButton m_chk_Active;
public:
	afx_msg void OnBnClickedCheckActive();
protected:
	CButton m_chk_Immediate;
public:
	afx_msg void OnBnClickedCheckImmediate();
	afx_msg void OnBnClickedButtonUniqidApply();
protected:
	CEdit m_edit_ModelID_Recv;
	CEdit m_edit_ModelID_Comp;

public:
	afx_msg void OnLbnKillfocusListQuestDest();
	afx_msg void OnBnClickedButtonQuestInfoEdit();
	CButton m_QuestInfoEditButton;
	CButton m_isReset;
	afx_msg void OnBnClickedCheckIsResetQuest();
};
