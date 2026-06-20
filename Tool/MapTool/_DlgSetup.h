#pragma once


// _DlgSetup 대화 상자입니다.

class _DlgSetup : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgSetup)

public:
	_DlgSetup(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgSetup();

	virtual BOOL OnInitDialog();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_SETUP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()


private:
	CSliderCtrl m_GizmoSensor;
	CSliderCtrl m_CamAutoMoveHeightRate;
	CButton m_Chk_Show_CollisionBox;
	CButton m_check_RenderObject;	
	CButton m_check_RenderMonsterRegion;
	CComboBox	m_CoordBox;
public:
	afx_msg void OnNMReleasedcaptureSliderGizmoSence(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMReleasedcaptureSliderRateCamHeight2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckShowCollisionbox();
	afx_msg void OnBnClickedCheckRenderobject();
	afx_msg void OnBnClickedCheckRenderMonsterregion();
	afx_msg void OnCbnSelchangeBasicCoord();
};
