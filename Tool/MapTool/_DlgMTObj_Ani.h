#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgMTObj_Ani 대화 상자입니다.

class _DlgMTObj_Ani : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTObj_Ani)


public:
	virtual void	_UpdateView();
	//======================================================================
	//
	//		Object
	//
	//======================================================================
protected:
	float			m_fEndAniTime;
	cObjectModel*	m_pObjectModel;
protected:	
	bool			__ReadAniInfo();



public:
	_DlgMTObj_Ani(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTObj_Ani();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_OBJECT_ANI };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
protected:
	CButton m_chk_DoAnimation;
	CSliderCtrl m_sld_AniSpeed;
	CSliderCtrl m_sld_AniFrame;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedCheck1();
protected:
	CEdit m_edit_AniSpeed;
	CEdit m_edit_AniFrame;
};
