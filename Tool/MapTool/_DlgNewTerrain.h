#pragma once
#include "afxwin.h"


// _DlgNewTerrain 대화 상자입니다.

class _DlgNewTerrain : public CDialog
{
	DECLARE_DYNAMIC(_DlgNewTerrain)

public:
	_DlgNewTerrain( CsGBTerrainRoot::sINFO* pRootInfo, CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgNewTerrain();

protected:	
	CsGBTerrainRoot::sINFO*		m_pRootInfo;
protected:	
	bool	__CalVertCount();	

public:
	void	OnOK(){}

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_NEWTERRAIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CsEditFosus m_edit_TWidth;
	CsEditFosus m_edit_THeight;
	CsEditFosus m_edit_GridWidth;
	CComboBox m_combo_TreeLevel;
	CsEditFosus m_edit_XVertCount;
	CsEditFosus m_edit_YVertCount;
	CButton m_btn_Ok;
	CComboBox m_combo_BaseMPPG;
	CComboBox m_combo_AlphaMPPG;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnApplyMakeset();
	afx_msg void OnBnClickedBtnOk();
};
