#pragma once


// _DlgMTMap_Resurrection 대화 상자입니다.

class _DlgMTMap_Resurrection : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTMap_Resurrection)


	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual void	_PostRenderView();

	//=====================================================================================================
	//
	//		Start
	//
	//=====================================================================================================
protected:
	CsMapResurrection*		m_pSelResurrection;

public:
	void					_SetResurrection( CsMapResurrection* pSrc );
	CsMapResurrection*		_GetResurrection(){ return m_pSelResurrection; }

	//=====================================================================================================
	//
	//		Control
	//
	//=====================================================================================================
protected:
	void		__ReleaseControl();

	//=====================================================================================================
	//
	//		Input
	//
	//=====================================================================================================
public:
	bool		_IsLButtonDown( CPoint pt );




public:
	_DlgMTMap_Resurrection(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTMap_Resurrection();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MAP_RESURRECTION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
protected:
	CsEditFosus m_edit_StartPosX;
	CsEditFosus m_edit_StartPosY;
	CSliderCtrl m_sld_Radius;
	CsEditFosus m_edit_Radius;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
};
