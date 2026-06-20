#pragma once
#include "afxwin.h"


// _DlgMTHeight 대화 상자입니다.

class _DlgMTHeight : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTHeight)


	//====================================================================================
	//
	//		Virtual
	//
	//====================================================================================
public:
	virtual void		_PostViewReset();
	virtual void		_UpdateView();
	virtual void		_PreRenderView();
	virtual void		_PostRenderView();
	virtual void		_OnLButtonDown( CPoint pt );
	virtual void		_OnLButtonUp( CPoint pt );
	virtual void		_OnMouseMove( CPoint pt );
	virtual void		OnKeyUp(const DWORD msg);
	virtual bool		_PreCheckKey( DWORD vKeyCode, bool bDown );


	//====================================================================================
	//
	//		Base
	//
	//====================================================================================

	//====================================================================================
	//
	//		User Height Region
	//
	//====================================================================================
protected:
	_DlgMTHeight_Region		m_DlgMTHeight_Region;
	_DlgMTHeight_Polygon	m_DlgMTHeight_Polygon;
	
public:
	_DlgMTHeight(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTHeight();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_HEIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CsEditFosus m_edit_HeightMap;
	CButton m_btn_ReApply;
	CsEditFosus m_edit_HeightMapRate;
public:
	afx_msg void OnBnClickedBtnHeightmapLoad();
	afx_msg void OnBnClickedBtnHeightmapReapply();
	virtual BOOL OnInitDialog();
protected:
	CComboBox m_combo_UserHeightType;
public:
	afx_msg void OnCbnSelchangeComboUserheightType();
	afx_msg void OnBnClickedBtnHeightmapReapplyShadowmap();
	
	HCURSOR mh_Spoid;
protected:
	CButton m_btn_Reapply_ShadowMap;
	bool m_bSpoidCursor;
};
