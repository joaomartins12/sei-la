#pragma once
#include "afxwin.h"


// _DlgMTLight_Direction 대화 상자입니다.

class _DlgMTLight_Direction : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTLight_Direction)

protected:	
	CsGBLight_Dir*		m_pSelectLight;
	CsGBLight_Dir		m_CreateLight;
	sCOLORDLG_INFO		m_Diffuse;
	sCOLORDLG_INFO		m_Ambient;
	sCOLORDLG_INFO		m_Specular;
	NiNodePtr			m_NiNode;
	NiNodePtr			m_NiNodeCenter;

	bool				m_bCreateMode;

public:
	void		_Delete();
	void		_ViewInit();
	void		_PreViewReset();
	void		_PostRenderView();

public:
	void		_Render( CsGBLight_Dir* pLight );
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
	void		__ReadLightInfo( CsGBLight::sBASE_INFO* pBaseInfo, CsGBLight_Dir::sINFO* pInfo );
	void		__SetCenterColor( NiColor color );




public:
	_DlgMTLight_Direction(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTLight_Direction();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_LIGHT_DIRECTION };

protected:

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CButton m_chk_TR;
	CButton m_chk_Object;
	CButton m_chk_Char;
	CButton m_chk_ObjectShadow;
	CButton m_chk_DynamicShadow;

	CsEditFloatOnly m_edit_TL[ 3 ];
	CsEditFloatOnly m_edit_Rot[ 3 ];
	CsEditFloatOnly m_edit_Scale;
	CsEditFloatOnly m_edit_Power;
	CsSpinCtrl	m_spin_Rot[ 3 ];
	CsSpinCtrl	m_spin_TL[ 3 ];
	CsSpinCtrl	m_spin_Scale;
	CsSpinCtrl	m_spin_Power;
public:
	afx_msg void OnStnClickedStaticSlA();
	afx_msg void OnEnSetfocusEditTlX();
	afx_msg void OnEnSetfocusEditTlY();
	afx_msg void OnEnSetfocusEditTlZ();
	afx_msg void OnEnSetfocusEditRotX();
	afx_msg void OnEnSetfocusEditRotY();
	afx_msg void OnEnSetfocusEditRotZ();	
	afx_msg void OnEnSetfocusEditScale();
	afx_msg void OnEnSetfocusEditPower();
	afx_msg void OnEnChangeEditRotX();
	afx_msg void OnEnChangeEditRotY();
	afx_msg void OnEnChangeEditRotZ();
	afx_msg void OnEnChangeEditTlX();
	afx_msg void OnEnChangeEditTlY();
	afx_msg void OnEnChangeEditTlZ();
	afx_msg void OnEnChangeEditScale();
	afx_msg void OnEnChangeEditPower();
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedStaticSlD();
	afx_msg void OnStnClickedStaticSlS();
protected:
	CStatic m_static_Diffuse;
	CStatic m_static_Ambient;
	CStatic m_static_Specular;
public:
	afx_msg void OnBnClickedCheckTr();
	afx_msg void OnBnClickedCheckObjectShadow();
	afx_msg void OnBnClickedCheckObject();
	afx_msg void OnBnClickedCheckChar();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedCheckDynamicShadow();
};
