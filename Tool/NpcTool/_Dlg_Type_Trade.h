#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _Dlg_Type_Trade 대화 상자입니다.

class _Dlg_Type_Trade : public CDialog
{
	DECLARE_DYNAMIC(_Dlg_Type_Trade)

	// Base
protected:
	sNPC_TYPE_TRADE*	m_pCurTrade;
protected:
	void			__Init();	
public:
	void			_SetNpcType( sNPC_TYPE_BASE* pCurType );

protected:
	void			__InsertList( CListCtrl* pListCtrl, CsItem* pItem );

	// Item
protected:
	void			__InitItem();
	void			__SortList( int nColume, bool bIncrease );

	// Trade
protected:
	void			__ResetTrade();
	void			__ApplyNpc( CsItem* pItem );
	bool			__IsExistTrade( DWORD dwSrcItemID );
	void			__SortTrade( int nColume, bool bIncrease );

	// Mfc Virtual
public:
	void			OnOK(){}
	void			OnCancel(){}	

public:
	_Dlg_Type_Trade(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_Dlg_Type_Trade();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_NPCTOOL_TRADE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	CListCtrl m_list_ItemTable;
public:
	afx_msg void OnLvnColumnclickListItemtable(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	CListCtrl m_list_Trade;
public:
	afx_msg void OnBnClickedButtonAdditem();
	afx_msg void OnLvnColumnclickListItemtrade(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonDelitem();
	afx_msg void OnCbnSelchangeComboSubType();
	CComboBox m_combo_SubType;
};
