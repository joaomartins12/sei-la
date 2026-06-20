#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgMTObj_Portal 대화 상자입니다.

class _DlgMTObj_Portal : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTObj_Portal)

public:
	_DlgMTObj_Portal(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTObj_Portal();

	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual void	_PreSave( const char* cFile, bool bAutoSave );


	// =================================================================================================
	//
	//		포탈 오브젝트
	//
	// =================================================================================================
protected:
	CsGBObj_Portal*			m_pPortalObj;

	// =================================================================================================
	//
	//		맵 정보
	//
	// =================================================================================================
protected:
	void					__ReadMapInfo();

	// =================================================================================================
	//
	//		지형간 이동
	//
	// =================================================================================================

protected:
	CsGBObject::sINFO*		m_pBackupSelectObjectInfo;
protected:
	void					__ReadDestTarget();
public:
	void					_SetDestTargetPos( NiPoint2 pt );
	void					_SetViewTargetPos( NiPoint2 pt );


public:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_OBJECT_PORTAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CComboBox m_combo_DestMap;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonGotoDestmap();
	afx_msg void OnBnClickedButtonBackOrgtr();
	afx_msg void OnCbnSelchangeComboPortalType();
protected:
	CsEditFosus m_edit_DestX;
	CsEditFosus m_edit_DestY;
	CsEditFosus m_edit_DestRadius;
	CSliderCtrl m_sld_DestRadius;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	CsEditFosus m_edit_TotalCount;
	CEdit m_edit_PortalID;
public:
	CComboBox m_combo_Portal_Type;
};
