//#pragma once
//#include "afxcmn.h"
//
//
//// CDlg_Type_DigiCore 대화 상자입니다.
//
//class CDlg_Type_DigiCore : public CDialog
//{
//	DECLARE_DYNAMIC(CDlg_Type_DigiCore)
//
//public:
//	CDlg_Type_DigiCore(CWnd* pParent = NULL);   // 표준 생성자입니다.
//	virtual ~CDlg_Type_DigiCore();
//
//// 대화 상자 데이터입니다.
//	enum { IDD = IDD_NPCTOOL_DIGICORE };
//
//protected:
//	sNPC_TYPE_DIGICORE*	m_pCurDigiCore;
//
//public:
//	void			SetNpcType( sNPC_TYPE_BASE* pCurType );
//
//protected:
//	void			__Init();	
//	void			__InsertList( CListCtrl* pListCtrl, CsItem* pItem );	
//
//	// Item
//protected:
//	void			__InitItem();
//	void			__SortList( int nColume, bool bIncrease );
//
//	// Trade
//protected:
//	void			__ResetTrade();
//	void			__ApplyNpc( CsItem* pItem );
//	bool			__IsExistTrade( DWORD dwSrcItemID );
//	void			__SortTrade( int nColume, bool bIncrease );
//
//protected:
//	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
//
//	DECLARE_MESSAGE_MAP()
//public:
//	virtual BOOL OnInitDialog();
//	afx_msg void OnBnClickedButtonAdddigicore();
//	CListCtrl m_List_DigiCore;
//	CListCtrl m_List_Trade;
//	afx_msg void OnLvnColumnclickListDigicoretable(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnLvnColumnclickListMainlist(NMHDR *pNMHDR, LRESULT *pResult);
//	afx_msg void OnBnClickedButtonDeldigicore();
//};
