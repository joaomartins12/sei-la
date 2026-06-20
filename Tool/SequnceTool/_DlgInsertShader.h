#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define VAL_INT_CNT			1
#define VAL_FLOAT_CNT		4
#define VAL_STR_CNT			1

// _DlgInsertShader 대화 상자입니다.

class _DlgInsertShader : public CDialog
{
	DECLARE_DYNAMIC(_DlgInsertShader)

public:
	_DlgInsertShader( CsC_AvObject* pTarget, CsModelData::sSHADER* pTextKeyShader, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgInsertShader();

protected:
	CsModelData::sSHADER*	m_pTextKeyShader;
	CsC_AvObject*			m_pTarget;

protected:
	void				__CheckListObject();
	int					__GetSelectListObject();



public:
	virtual void		OnOK(){}
	virtual void		OnCancel();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_INSERT_SHADER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CComboBox m_cmb_Type;
	CStatic m_static_Int1;
	CStatic m_static_float1;
	CStatic m_static_float2;
	CStatic m_static_float3;
	CStatic m_static_float4;
	CStatic m_static_Str1;
	CEdit m_edit_Int[ VAL_INT_CNT ];
	CEdit m_edit_Float[ VAL_FLOAT_CNT ];
	CEdit m_edit_Str[ VAL_STR_CNT ];
	CButton m_btn_Str[ VAL_STR_CNT ];
public:
	virtual BOOL OnInitDialog();
public:
	afx_msg void OnCbnSelchangeComboShader();
	
protected:
	CListCtrl m_list_Object;
public:
	afx_msg void OnBnClickedButtonOk();
	afx_msg void OnBnClickedButtonStr1();
};
