#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgMTHeight_Region 대화 상자입니다.

class _DlgMTHeight_Region : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTHeight_Region)

	
	//=============================================================================================
	//
	//		Virtual
	//
	//=============================================================================================
public:
	virtual void		_UpdateView();
	virtual void		_PostRenderView();

	//=============================================================================================
	//
	//		Limite
	//
	//=============================================================================================
protected:
	enum eLIMITE{
		L_NONE, L_UP, L_DOWN,
	};
	eLIMITE				m_eLimite;
	float				m_fLimite;

	//=============================================================================================
	//
	//		Type
	//
	//=============================================================================================
protected:
	enum eTYPE{
		T_DEFAULT = 0, T_LOWER_CHANGE,
	};
	eTYPE				m_eType;
	float				m_fTypeHeight;

	//=============================================================================================
	//
	//		Height Change
	//
	//=============================================================================================
protected:
	int					m_nTargetNodeIndex;
	CsGBTerrainLeaf*	m_pTargetLeaf;
	CPoint				m_ptIntersect;

	bool				m_bHeightChange;
	float				m_fDeltaHeight;
	float				m_fSize;
	float				m_fConstant;

	CPoint				m_poMousePoint;
	NiPoint3			m_CurrIntersectPt;
protected:
	struct sHEIGHT_BASE
	{
		float				s_fDeltaHeight;
		float				s_fMaxSize;
		float				s_fGridSize;
		NiPoint2			s_ptSrcPos;
	};
	struct sHEIGHT_INFO
	{
		CsGBTerrainLeaf*	s_pLeaf;
		int					s_nLeafIndex;
		int					s_nVertIndex_x;
		int					s_nVertIndex_y;
		NiPoint2			s_ptDestPos;
	};
protected:
	void				__InitHeightChange( CPoint pt );
	void				__HeightChange( float fDeltaHeight, float fSize, bool bSmooth = false );
	void				__SetHeight( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO* pInfo, float fLength, bool bSmooth );
	void				__HeightChangeUp( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO info, bool bSmooth );
	void				__HeightChangeDown( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO info, bool bSmooth );
	void				__HeightChangeRight( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO info, bool bSmooth );	
	void				__HeightChangeLeft( sHEIGHT_BASE* pBaseInfo, sHEIGHT_INFO info, bool bSmooth );

	//=============================================================================================
	//
	//		Input
	//
	//=============================================================================================
protected:
	bool		m_bLButtonDown;
public:
	void		_OnLButtonDown( CPoint pt );
	void		_OnLButtonUp( CPoint pt );
	void		_OnMouseMove( CPoint pt );
	const NiPoint3& GetCurrIntersectPT(void){return m_CurrIntersectPt;}
	void		SetHeightLimit(const float& fHeight);

private:
	bool		__GetHeightFromTotalIndex( int const& nPositionX, int const& nPositionY, __int64 const& nTotalIdx, float & fTotoalHeight, int const& nPickLeafNodeIdx );





public:
	_DlgMTHeight_Region(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTHeight_Region();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_HEIGHT_REGION };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
protected:
	CsEditFosus m_edit_DeltaHeight;
	CsEditFosus m_edit_Size;
	CsEditFosus m_edit_Constant;
protected:
	CSliderCtrl m_sld_DeltaHeight;
	CSliderCtrl m_sld_Size;
	CSliderCtrl m_sld_Constant;
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	CButton m_chk_SetLimite;
	CsEditFosus m_edit_Limite;
public:
	afx_msg void OnEnChangeEditLimite();
protected:
	CButton m_rdo_UpLimit;
	CButton m_rdo_DownLimit;
public:
	afx_msg void OnBnClickedButtonReleaseLimite();
protected:
	CComboBox m_combo_Type;
	CButton m_UsingUpdateOBjectPos;

	bool UsingUpdateObjPos;
public:
	afx_msg void OnCbnSelchangeComboHeightType();
	afx_msg void OnBnClickedRadioUpLimite();
	afx_msg void OnBnClickedRadioDownLimite();
	afx_msg void OnBnClickedCheckObjupdate();
};
