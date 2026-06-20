#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _DlgMTWeather 대화 상자입니다.

class _DlgMTWeather : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTWeather)

public:
	_DlgMTWeather(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTWeather();

	//============================================================================================================
	//
	//		Base
	//
	//============================================================================================================
public:
	virtual void			_PostViewReset();

	virtual void			_PostRenderView();
	virtual void			_PreSave( const char* cFile, bool bAutoSave );

	virtual void			_KeyDownEscape();
	virtual void			_OnLButtonDown( CPoint pt );

	//============================================================================================================
	//
	//		Zone
	//
	//============================================================================================================
protected:
	CsWeather*				m_pCurSelectZone;

protected:
	void					__ResetZoneList( CsWeather* pSelect );
	void					__SetCurSelectZone( CsWeather* pSelect );

	void					__CheckTimeZone();

	//============================================================================================================
	//
	//		Element
	//
	//============================================================================================================

protected:
	CsWeather::sELEMENT*	m_pCurSelectElement;

protected:
	void					__ResetElementList( CsWeather::sELEMENT* pSelect );
	void					__SetCurSelectElement( CsWeather::sELEMENT* pSelect );

	void					__CheckTimeElement( CsWeather* pWeather );







// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_WEATHER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CListCtrl m_listZone;
	CListCtrl m_listElement;
	CSliderCtrl m_sld_Size;
public:
	afx_msg void OnBnClickedButtonAddZone();
	afx_msg void OnBnClickedButtonDelZone();
	afx_msg void OnBnClickedButtonAddElement();
	afx_msg void OnBnClickedButtonDelElement();
protected:
	CComboBox m_cmb_Type;
	CsEditFosus m_edit_Time;
	CComboBox m_cmb_Level;
	CsEditFosus m_edit_Fatigue;
public:
	virtual BOOL OnInitDialog();
protected:
	CsEditFosus m_edit_LoopTime;
public:
	afx_msg void OnNMClickListZone(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditZoneLooptime();
	afx_msg void OnNMClickListElement(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeComboEleTime();
	afx_msg void OnCbnSelchangeComboEleType();
	afx_msg void OnEnChangeEditEleTime();
	afx_msg void OnCbnSelchangeComboEleLevel();
	afx_msg void OnEnChangeEditFatigue();
};
