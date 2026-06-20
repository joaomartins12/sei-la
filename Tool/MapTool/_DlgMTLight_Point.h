#pragma once


// _DlgMTLight_Point 대화 상자입니다.

class _DlgMTLight_Point : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTLight_Point)

protected:
	CsGBLight_Point*	m_pSelectLight;
	sCOLORDLG_INFO		m_Diffuse;
	sCOLORDLG_INFO		m_Ambient;
	sCOLORDLG_INFO		m_Specular;
	NiNodePtr			m_NiNodeCenter;
	NiNodePtr			m_NiNodeNear;
	NiNodePtr			m_NiNodeFar;

	CsGBLight_Point		m_CreateLight;

	bool				m_bCreateMode;

public:
	void		_Delete();
	void		_ViewInit();
	void		_PreViewReset();
	void		_PostRenderView();

public:
	void		_Render( CsGBLight_Point* pLight );
	void		_AddLight();	

	bool		_IsFocus();
	void		_SetCreateMode( bool bCreateMode );
	void		_ResetSelect(){ _SetSelectLight( m_pSelectLight ); }
	void		_SetSelectLight( CsGBLight* pLight );
	void		_PickSetPos( NiPoint3 pos );
	bool		_IsSelect(){ return ( m_pSelectLight != NULL ); }

protected:
	void		__ResetCreateLight();
	void		__ReadLightInfo( CsGBLight* pLight );
	void		__ReadLightInfo( CsGBLight::sBASE_INFO* pBaseInfo, CsGBLight_Point::sINFO* pInfo );
	void		__SetCenterColor( NiColor color );






public:
	_DlgMTLight_Point(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTLight_Point();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_LIGHT_POINT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	CButton m_chk_TR;
	CButton m_chk_Object;
	CButton m_chk_Char;
	CButton m_chk_ObjectShadow;
	CButton m_chk_DynamicShadow;

	CsEditFloatOnly m_edit_TL[ 3 ];
	CsSpinCtrl	m_spin_TL[ 3 ];
	CsEditFloatOnly m_edit_Constant;
	CsSpinCtrl	m_spin_Constant;
	CsEditFloatOnly m_edit_Range;
	CsSpinCtrl	m_spin_Range;
	CsEditFloatOnly m_edit_Power;
	CsSpinCtrl	m_spin_Power;
	CStatic m_static_Diffuse;
	CStatic m_static_Ambient;
	CStatic m_static_Specular;

public:
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedStaticSlD();
	afx_msg void OnStnClickedStaticSlA();
	afx_msg void OnStnClickedStaticSlS();
	afx_msg void OnEnSetfocusEditTlX();
	afx_msg void OnEnSetfocusEditTlY();
	afx_msg void OnEnSetfocusEditTlZ();
	afx_msg void OnEnChangeEditTlX();
	afx_msg void OnEnChangeEditTlY();
	afx_msg void OnEnChangeEditTlZ();
	afx_msg void OnEnSetfocusEditC();
	afx_msg void OnEnChangeEditC();
	afx_msg void OnEnSetfocusEditRange();
	afx_msg void OnEnChangeEditRange();
	afx_msg void OnEnSetfocusEditPower();
	afx_msg void OnEnChangeEditPower();
	afx_msg void OnBnClickedCheckTr();
	afx_msg void OnBnClickedCheckObjectShadow();
	afx_msg void OnBnClickedCheckObject();
	afx_msg void OnBnClickedCheckChar();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCheckDynamicShadow();
};
