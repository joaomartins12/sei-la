#pragma once
#include "afxwin.h"
#include "cseditfocus.h"


// _DlgMTMap_Region 대화 상자입니다.

class _DlgMTMap_Region : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTMap_Region)

public:
	_DlgMTMap_Region(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTMap_Region();


	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual void	_PostRenderView();

	//=====================================================================================================
	//
	//		Region
	//
	//=====================================================================================================
protected:
	CsMapRegion*	m_pSelRegion;

public:
	void			_SetRegion( CsMapRegion* pRegion );
	CsMapRegion*	_GetRegion(){ return m_pSelRegion; }

	//=====================================================================================================
	//
	//		Control
	//
	//=====================================================================================================
protected:
	void		__ReleaseControl();
	void		__ReadBGSound();
	void		__ReadDiscript();

	//=====================================================================================================
	//
	//		Input
	//
	//=====================================================================================================
public:
	bool		_IsLButtonDown( CPoint pt );




public:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MAP_REGION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
protected:
	CsEditFosus m_edit_StartPosX;
	CsEditFosus m_edit_StartPosY;
	CSliderCtrl m_sld_Radius;
	CsEditFosus m_edit_Radius;
public:
	afx_msg void OnEnChangeEditMapRegionDiscript();
	afx_msg void OnEnChangeEditMapRegionDiscript_Eng();
protected:
	CsEditFosus m_edit_Discript;
public:
	afx_msg void OnBnClickedButtonLoadRegionBgsound();
protected:
	CsEditFosus m_edit_BGSound;
public:
	afx_msg void OnBnClickedButtonDelRegionBgsound();

protected:
	CsEditFosus m_edit_FG_Type;
	CsEditFosus m_edit_FG_DBuff;
	CsEditFosus m_edit_FG_STime;
	CsEditFosus m_edit_FG_ATime;
	CsEditFosus m_edit_FG_Point;
public:
	afx_msg void OnEnChangeEditFatigueType();
	afx_msg void OnEnChangeEditFatigueDebuff();
	afx_msg void OnEnChangeEditFatigueStime();
	afx_msg void OnEnChangeEditFatigueAtime();
	afx_msg void OnEnChangeEditFatiguePoint();
protected:
	CEdit m_edit_Discript_Eng;
public:
	afx_msg void OnNMCustomdrawSliderRadius(NMHDR *pNMHDR, LRESULT *pResult);
};
