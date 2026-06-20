#pragma once


// _DlgMTMap_LimitEvo 대화 상자입니다.

class _DlgMTMap_LimitEvo : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTMap_LimitEvo)

public:
	_DlgMTMap_LimitEvo(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTMap_LimitEvo();


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
	CsMap_LimitEvo*			m_pSelLimitEvo;

public:
	void					_SetLimitEvo( CsMap_LimitEvo* pSrc );
	CsMap_LimitEvo*			_GetLimitEvo(){ return m_pSelLimitEvo; }

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




// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_MAP_LIMITEVO };

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
