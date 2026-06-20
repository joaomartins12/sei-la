// NpcToolDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CNpcToolDlg 대화 상자
class CNpcToolDlg : public CDialog
{
	// Base
public:
	void			_Delete();
protected:	
	void			__Init();

	void			__ResetNpc( bool bResetList );



protected:
	CsNpc			m_AddNpc;
	CsNpc*			m_pSelNpc;


	// Npc List
protected:
	void			__InitNpcList();
	void			__ReadNpcList();
	void			__WriteNpcList();
	void			__WriteNpcList( int nListIndex, CsNpc* pNpc );	

	// Npc ID
protected:
	void			__InitNpcID();
	void			__ReadNpcID();

	// Map ID
protected:
	void			__InitMapID();
	void			__ReadMapID();

	// Npc Type
protected:
	_Dlg_Type_Trade		m_dlg_Type_Trade;
	_Dlg_Type_Portal	m_dlg_Type_Portal;
	//CDlg_Type_DigiCore  m_dlg_Type_DigiCore;
	

protected:
	void			__InitNpcType();
	void			__ReadNpcType();

	// Npc MoveType
protected:
	void			__InitMoveType();
	void			__ReadMoveType();

	// Npc PenName
protected:
	void			__InitNpcPenName();
	void			__ReadNpcPenName();

	// Npc Name
protected:
	void			__InitNpcName();
	void			__ReadNpcName();

	// Npc Model
protected:
	void			__InitNpcModel();
	void			__ReadNpcModel();

	// Npc Talk
protected:
	void			__InitNpcTalk();
	void			__ReadNpcTalk();

	// Zoom View
protected:
	void			__InitDispZoom();
	void			__ReadDispZoom();

	// Infomation
protected:
	void			__InitDispInfo();
	void			__ReadDispInfo();

	// Extra
protected:
	void				__InitNpcExtra();
	void				__ReadNpcExtra();
	_Dlg_Extra_Quest	m_dlg_Extra_Quest;


	// Save
protected:
	void			__SaveExcel();



	// Mfc Virtual
public:
	void			OnOK(){}



// 생성입니다.
public:
	CNpcToolDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCTOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	CEdit m_edit_NpcID;
	CComboBox m_combo_MapID;
	CEdit m_edit_MapDiscript;
	CComboBox m_combo_NpcType;
	CEdit m_edit_NpcName;
	CEdit m_edit_NpcTalk;
public:
	afx_msg void OnCbnSelchangeComboMapid();
protected:
	CEdit m_edit_NpcModelID;
	CEdit m_edit_NpcModelName;
public:
	afx_msg void OnEnChangeEditNpcmodel();
protected:
	CEdit m_edit_NpcID_Discript;
public:
	afx_msg void OnEnChangeEditNpcid();
	afx_msg void OnEnChangeEditNpcname();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnEnChangeEditNpctalk();
	afx_msg void OnCbnSelchangeComboNpctype();
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonSave();
protected:
	CListCtrl m_list_NpcList;
public:
	afx_msg void OnLvnColumnclickListNpclist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListNpclist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDellist();	
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

protected:
	CEdit m_edit_NpcPenName;
public:
	afx_msg void OnEnChangeEditNpcpenname();
	afx_msg void OnBnClickedButtonCheckStore();
protected:
	CButton m_pBtnAdd;
public:
	afx_msg void OnBnClickedButtonSelRelease();
	afx_msg void OnCbnSelchangeComboNpctype2();
	CComboBox m_Combo_MoveType;
protected:
	CComboBox m_cmb_ExtraType;
	CListBox m_list_Extra;
	CButton m_btn_ExtraAdd;
	CButton m_btn_ExtraDel;
public:
	afx_msg void OnBnClickedButtonExtraAdd();
	afx_msg void OnBnClickedButtonExtraDel();
	afx_msg void OnLbnSelchangeListExtra();
protected:
	CComboBox m_cmb_DispInfo;
public:
	afx_msg void OnCbnSelchangeComboDispinfo();
protected:
	CComboBox m_cmb_ZoomView;
public:
	afx_msg void OnCbnSelchangeComboZoomview();
	afx_msg void OnBnClickedButtonModelinfo();

public:
	afx_msg void OnCbnSelchangeComboSubType();
	CComboBox m_combo_SubType;
};
