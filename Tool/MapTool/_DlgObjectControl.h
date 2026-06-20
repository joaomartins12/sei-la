#pragma once
#include "afxwin.h"


// _DlgObjectControl 대화 상자입니다.

class _DlgObjectControl : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgObjectControl)

public:
	_DlgObjectControl(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgObjectControl();

public:
	cAxisRotation::eAXIS_TYPE	m_AxisType;	

public:
	bool		_IsFocus();
	void		_SetControl( bool bPush );

	void		_ReleaseValue();
	void		_InsertValue( cAxisRotation::eAXIS_TYPE at, NiTransform tr );

	void		_ResetValue_Act(DWORD const& dwMapID, DWORD const& dwObjID);
	void		__EmptyValue_Act( bool bEnable);
	void		_InsertValue_Act( cObjectModel* pObjectModel );
	void		__SetEditValueApplyToSelectObject_Act();

	//void		_RemoveValue();
	void		_ResetValue( NiTransform tr );
	void		_SetFocusFirstEdit(){ m_edit_Rot[ 0 ].SetFocus(); }

protected:
	void		__EmptyValue();
	void		__SetEditValueApplyToSelectObject();


	//============================================
	//  Texture Type
protected:
	void		__ResetTextureType();
	

// 대화 상자 데이터입니다.
public:
	enum { IDD = IDD_DLG_OBJECT_CONTROL };

	enum eTREE_TYPE
	{
		TT_ROOT,
		TT_FACTOR_ROOT,
		TT_FACTOR_NODE,
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CsEditFloatOnly m_edit_TL[ 3 ];
	CsEditFloatOnly m_edit_Rot[ 3 ];
	CsSpinCtrl		m_spin_Rot[ 3 ];
	CsSpinCtrl		m_spin_TL[ 3 ];	
	CStatic			m_ObjectUniqID;

	CComboBox		m_nOpenType;
	CsEditFosus		m_edit_Factor;
	CsEditFosus		m_edit_FactorCnt;
	CTreeCtrl		m_FactorTree;
	CButton			m_FactorAddBtn;
	CButton			m_FactorChangeBtn;

public:
	enum eACTOBJ_TYPE
	{
		AT_NONE		= 0,
		AT_MONSTER	= 1,
		AT_QUEST_COMPLETE	= 2,
		AT_SERVER_CONTROL = 3,
	};

public:
	afx_msg void OnEnSetfocusEditTlX();
	afx_msg void OnEnKillfocusEditTlX();
	
	afx_msg void OnEnSetfocusEditTlY();
	afx_msg void OnEnSetfocusEditTlZ();
	afx_msg void OnEnSetfocusEditRotX();
	afx_msg void OnEnSetfocusEditRotY();
	afx_msg void OnEnSetfocusEditRotZ();

	afx_msg void OnEnSetfocusEditFactor1();
	afx_msg void OnEnSetfocusEditFactorCnt1();

	afx_msg void OnNMRclickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnPopmenu_TreeItemDel();
	afx_msg void OnBnClickedBtnFactorAdd();
	afx_msg void OnBnClickedBtnFactorChange();

	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditRotX();
	afx_msg void OnEnChangeEditRotY();
	afx_msg void OnEnChangeEditRotZ();
	afx_msg void OnEnChangeEditTlX();
	afx_msg void OnEnChangeEditTlY();
	afx_msg void OnEnChangeEditTlZ();

	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

private:
	DWORD	m_dwCurrentMapID;
	DWORD	m_SelectedObjID;
	int			mLastCUID;
public:
	afx_msg void OnEnChangeEditOrderNumber();
	CsEditFosus		m_edit_OrderNumber;
};
