#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _Dlg_Type_Portal 대화 상자입니다.

class _Dlg_Type_Portal : public CDialog
{
	DECLARE_DYNAMIC(_Dlg_Type_Portal)

protected:
	sNPC_TYPE_PORTAL*				m_pCurType;
	sNPC_TYPE_PORTAL::sPORTAL_REQ*	m_pCurReq;
	int								m_nCurReqIndex;

public:
	void				_SetNpcType( sNPC_TYPE_BASE* pCurType );


protected:
	void				__Reset();
	void				__InsertPortal( sNPC_TYPE_PORTAL::sPORTAL* pPortal );
	void				__InsertReq( sNPC_TYPE_PORTAL::sPORTAL_REQ* pReq );

	void				__ResetReq();
	void				__ResetReqItemSet();

public:
	DWORD				_GetID(){ return _Control2Int( &m_edit_EnableID ); }
	DWORD				_GetCount(){ return _Control2Int( &m_edit_EnableCount ); }


	// Mfc Virtual
public:
	void			OnOK(){}
	void			OnCancel(){}	




public:
	_Dlg_Type_Portal(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_Dlg_Type_Portal();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCTOOL_PORTAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CComboBox m_combo_Disapprove;
public:
	virtual BOOL OnInitDialog();
protected:
	CEdit m_edit_Discript;
public:
	afx_msg void OnBnClickedButtonTable();
protected:
	CEdit m_edit_EnableID;
	CEdit m_edit_EnableCount;
public:
	afx_msg void OnEnChangeEditEnableid();
	afx_msg void OnEnChangeEditEnablecount();
	afx_msg void OnCbnSelchangeComboDisapprove();
protected:
	CListCtrl m_list_Portal;
	CListCtrl m_list_Req;
public:
	afx_msg void OnBnClickedButtonAddportal();
	afx_msg void OnBnClickedButtonDelportal();
	afx_msg void OnNMClickListPortal(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListReq(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CEdit m_edit_EventID;
public:
	afx_msg void OnEnChangeEditEventid();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	CButton m_Radio_Train;
	CButton m_Radio_Normal;
	CButton m_Radio_None;
};
