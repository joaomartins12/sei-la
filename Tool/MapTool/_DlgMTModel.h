#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _DlgMTModel 대화 상자입니다.

class _DlgMTModel : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTModel)

public:
	_DlgMTModel(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTModel();

	// ==================================================================================
	//
	//		virtual
	//
	// ==================================================================================
public:
	virtual void			_Delete();
	virtual void			_ViewInit();
	virtual void			_PostViewReset();
	virtual void			_PreSave( const char* cFile, bool bAutoSave );
	virtual void			_UpdateView();
	virtual void			_KeyDownEscape();

	// ==================================================================================
	//
	//		Npc
	//
	// ==================================================================================
protected:
	_DlgMTModel_Portal		m_DlgPortal;

protected:
	void					__CheckDlg( DWORD dwNpcID );
	void					__CheckResist( DWORD dwNpcID, int x, int y );
	void					__CheckUnResist( DWORD dwNpcID );

public:
	_DlgMTModel_Portal*		_GetDlgPortal(){ return &m_DlgPortal; }

	// ==================================================================================
	//		List
	// ==================================================================================
protected:
	void					__ResetMapNpcList();
public:
	void					_CancelMapNpcList();	
	void					_SetMapNpcList( DWORD dwNpcID );
	void					_ResistNpcList( DWORD dwNpcID, int x, int y );
	void					_UnresistNpcList( DWORD dwNpcID );









public:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MODEL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);		
	virtual BOOL OnInitDialog();
protected:
	CListCtrl m_list_NpcList;
public:
	afx_msg void OnNMClickListNpclist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDBClickListNpclist(NMHDR *pNMHDR, LRESULT *pResult);
};
