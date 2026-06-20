#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _DlgMTLight 대화 상자입니다.

class _DlgMTLight : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTLight)


public:
	enum eLIGHT_TYPE
	{
		LT_DIRECTION,
		LT_POINT,
	};

protected:	
	_DlgMTLight_Direction	m_DlgDirection;
	_DlgMTLight_Point		m_DlgPoint;

	bool					m_bListLightLock;

public:
	virtual void		_Delete();
	virtual void		_ViewInit();
	virtual void		_PreSave( const char* cFile, bool bAutoSave );

	virtual void		_PreViewReset();
	virtual void		_PostViewReset();
	virtual void		_PostRenderView();
	virtual void		_OnLButtonDown( CPoint pt );


public:
	void		_ReadInfo( bool bSelectKeep );

protected:
	void		__SetNormalMode();
	void		__SetCreateMode();	



public:
	_DlgMTLight(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTLight();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_LIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	CListCtrl m_list_Light;
	CComboBox m_combo_CMType;
	CButton m_btn_CreateMode;
	CButton m_btn_CMOk;
	CButton m_btn_CMCancel;		
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCmCancel();
	afx_msg void OnBnClickedCreatemode();
	afx_msg void OnBnClickedCmOk();
	afx_msg void OnCbnSelchangeComboType();	
	afx_msg void OnNMClickListLight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListLight(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonListdel();
protected:
	CButton m_chk_SetPos;
public:
	afx_msg void OnBnClickedButtonListCopy();
};
