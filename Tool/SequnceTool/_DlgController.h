#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _DlgController 대화 상자입니다.

class _DlgController : public CFormView
{
	DECLARE_DYNCREATE(_DlgController)

protected:	
	CsModelData*			m_pSelectModel;

public:
	void		_Init();
	void		_Update(){}

	bool		_IsDestView(){ return ( m_chk_DestView.GetCheck() == BST_CHECKED ); }
	float		_GetDestDist(){ return (float)nsCsMfcFunc::Control2Int( &m_edit_DestDist ); } 
	DWORD		_GetDestAni(){ return (DWORD)m_combo_DestAni.GetItemData( m_combo_DestAni.GetCurSel() );  }


protected:
	void		__InsertList( LPTSTR path );
	void		__Search( LPTSTR path );

	void		__CancelSelect();
	void		__SortModelList( int nColume, bool bIncrease );

	bool		__InsertListControl( CsModelData* pModelData );

	void		__SaveHeightExcel();



protected:
	_DlgPartObject		m_DlgPart;





protected:
	_DlgController();   // 표준 생성자입니다.
	virtual ~_DlgController();

public:
	enum { IDD = IDD_DLG_CONTROL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual void OnOK(){}
	virtual void OnCancel(){}
protected:
	CListCtrl m_list_Model;
	CsEdit			m_edit_Scale;
	CsSpinCtrl		m_spin_Scale;
	CsEdit			m_edit_Height;
	CsSpinCtrl		m_spin_Height;
	CsEdit			m_edit_Width;
	CsSpinCtrl		m_spin_Width;
	CButton			m_chk_HideABV;
	CButton			m_chk_ClientBox;
	CButton			m_chk_DestView;
	CsEdit			m_edit_DestDist;
	CsSpinCtrl		m_spin_DestDist;

public:
	afx_msg void OnBnClickedButtonResetmodel();
	afx_msg void OnNMClickListModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnEnChangeEditScale();
	afx_msg void OnEnChangeEditHeight();
	afx_msg void OnEnChangeEditWidth();	
	afx_msg void OnBnClickedCheckHideClient();
	afx_msg void OnBnClickedCheckHideAbv();
protected:
	CComboBox m_combo_DestAni;
public:
	afx_msg void OnBnClickedButtonSrcAttdist();
	afx_msg void OnBnClickedButtonSrcAttdist2();
	afx_msg void OnLvnKeydownListModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonSaveExcel();
	afx_msg void OnLvnColumnclickListModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickListModel(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonFolder();
	afx_msg void OnBnClickedButtonCheckAni();
	afx_msg void OnBnClickedButtonCheckEvent();
	afx_msg void OnBnClickedButtonBG();
protected:
	CEdit m_edit_CharHeight;
	CEdit m_edit_Find;
public:
	afx_msg void OnBnClickedButtonFind();
	afx_msg void OnBnClickedButtonSaveHeightexcel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonSoundFolder();
	afx_msg void OnBnClickedButtonCopyDigimon();
};
