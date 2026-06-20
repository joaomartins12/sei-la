#pragma once
#include "afxwin.h"


// _DlgMTPlayer 대화 상자입니다.

class _DlgMTPlayer : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTPlayer)

	// ===== View Player
protected:
	bool		m_bViewPlayer;

public:
	_DlgMTPlayer(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTPlayer();

	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual void	_PreSave( const char* cFile, bool bAutoSave );

public:
	virtual void	_Delete();
	virtual void	_UpdateView();
	virtual void	_PreRenderView();

	virtual void	_OnLButtonDown( CPoint pt );
	virtual void	_OnLButtonUp( CPoint pt ){}
	virtual void	_OnMouseMove( CPoint pt ){}

	// 스카이박스
protected:
	void			__ResistSkyBoxPath();
public:
	bool			_IsShowSkyBos(){ return ( m_chk_ShowSkyBox.GetCheck() == BST_CHECKED ); }

	// 포그
protected:
	sCOLORDLG_INFO	m_FogColor;
protected:
	void			__ChangeFogColor();
	void			__ChangeBGColor();
	void			__ReadFogValue();

	// 백그라운드 컬러
protected:
	sCOLORDLG_INFO	m_BGColor;

	// 클리핑
protected:
	void			__ReadClipValue();

	// 뷰
public:
	bool			_IsViewDigimon(){ return ( m_chk_ViewDigimon.GetCheck() == BST_CHECKED ); }
	
// 대화 상자 데이터입니다.
public:
	enum { IDD = IDD_DLG_MT_PLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CsEditFosus m_edit_SkyBoxPath;
	CStatic m_static_FogColor;
	CSliderCtrl m_sld_FogStart;
	CsEditFosus m_edit_FogStart;
	CSliderCtrl m_sld_FogEndObject;
	CsEditFosus m_edit_FogEndObject;
	CSliderCtrl m_sld_FogEndTerrain;
	CsEditFosus m_edit_FogEndTerrain;
	CSliderCtrl m_sld_ClipCell;
	CsEditFosus m_edit_ClipCell;
	CSliderCtrl m_sld_ClipObject;
	CsEditFosus m_edit_ClipObject;
	CSliderCtrl m_sld_ClipTerrain;
	CsEditFosus m_edit_ClipTerrain;

	CsEditFosus m_edit_SkyBoxHeight;
	CsSpinCtrl m_spin_SkyBoxHeight;

	CSliderCtrl m_PlayerMoveSpeed;
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonApplyCollision();
	afx_msg void OnBnClickedBtnLoadskybox();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnClickedStaticFogColor();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnEnChangeEditSkyboxHeight();
protected:
	CStatic m_static_BGColor;
public:
	afx_msg void OnStnClickedStaticBgColor();
protected:
	CButton m_chk_ShowSkyBox;
	CButton m_chk_ViewDigimon;
	float m_fPlayerSpeed;
public:
	afx_msg void OnNMReleasedcaptureSliderRadius(NMHDR *pNMHDR, LRESULT *pResult);
};
