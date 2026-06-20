#pragma once
#include "afxwin.h"


// _DlgMTModel_Portal 대화 상자입니다.

class _DlgMTModel_Portal : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTModel_Portal)

	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual void		_PreSave( const char* cFile, bool bAutoSave );

	// =================================================================================================
	//
	//		포탈 오브젝트
	//
	// =================================================================================================
protected:
	CsMapPortal*			m_pProtalFT;
	CsNpc*					m_pNpcFT;

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
	DWORD					m_dwBackupSelectNpcID;
	int						m_nBackupKindIndex;
protected:
	void					__ReadDestTarget();
public:
	void					_SetNpc( CsNpc* pNpcFT );
	void					_SetDestTargetPos( int x, int y );
	void					_SetViewTargetPos( int x, int y );





public:
	_DlgMTModel_Portal(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTModel_Portal();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MODEL_PORTAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	CComboBox m_combo_DestMap;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonGotoDestmap();
	afx_msg void OnBnClickedButtonBackOrgtr();
protected:
	CsEditFosus m_edit_DestX;
	CsEditFosus m_edit_DestY;
	CsEditFosus m_edit_DestRadius;
	CSliderCtrl m_sld_DestRadius;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	CComboBox m_combo_Kind;
public:
	afx_msg void OnCbnSelchangeComboKind();
};
