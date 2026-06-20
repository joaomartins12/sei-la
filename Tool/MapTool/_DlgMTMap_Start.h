#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgMTMap_Start 대화 상자입니다.

class _DlgMTMap_Start : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTMap_Start)

public:
	_DlgMTMap_Start(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTMap_Start();


	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual void	_PostRenderView();
	//=====================================================================================================
	//
	//		Input
	//
	//=====================================================================================================
public:
	bool		_IsLButtonDown( CPoint pt );

	//=====================================================================================================
	//
	//		Start
	//
	//=====================================================================================================
protected:
	CsMapStart*		m_pSelStart;

public:
	void			_SetStart( CsMapStart* pStart );
	CsMapStart*		_GetStart(){ return m_pSelStart; }

	//=====================================================================================================
	//
	//		Control
	//
	//=====================================================================================================
protected:
	void		__ReleaseControl();	

public:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MAP_START };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
protected:
	CsEditFosus m_edit_StartPosX;
	CsEditFosus m_edit_StartPosY;
	CSliderCtrl m_sld_Radius;
	CsEditFosus m_edit_Radius;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	CComboBox m_combo_Type;
public:
	afx_msg void OnCbnSelchangeComboType();
};
