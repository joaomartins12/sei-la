#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgMTMapping 대화 상자입니다.

class _DlgMTMapping : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTMapping)

public:
	_DlgMTMapping(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTMapping();

public:
	enum eTREE_TYPE
	{
		TT_ROOT,
		TT_GROUP,
		TT_NODE,
	};

public:
	virtual void		_Delete();
	virtual void		_KeyDownEscape();
	virtual void		_PostRenderView();
	virtual void		_OnMouseMove( CPoint pt );

protected:
	TCHAR				m_szSelTexturePath[ MAX_PATH ];
	int					m_nCheckMapIndex;
	float				m_fOpercity;

	HTREEITEM			m_hDMap0Item;
	HTREEITEM			m_hDMap1Item;
	HTREEITEM			m_hDMap2Item;
	HTREEITEM			m_hDMap3Item;
	HTREEITEM			m_hDMap4Item;

	sCOLORDLG_INFO		m_Gray;
	CPoint				m_ptMousePoint;

protected:
	void				__DeleteTree();
friend cToolResource;
	HTREEITEM			__InsertTree( LPCTSTR szRootPath, LPCTSTR szFile );
public:	
	void				_Reset();
	void				_Resist();	

	LPCTSTR				_GetSelTexturePath();

	float				_GetOpercity(){ return m_fOpercity; }

	// ==== Base Map
	void				_SetDetail_Map0Text( LPCTSTR szText ){ m_rdo_DMap0.SetWindowText( szText ); }
	void				_SetDetail_Map1Text( LPCTSTR szText ){ m_rdo_DMap1.SetWindowText( szText ); }
	void				_SetDetail_Map2Text( LPCTSTR szText ){ m_rdo_DMap2.SetWindowText( szText ); }
	void				_SetDetail_Map3Text( LPCTSTR szText ){ m_rdo_DMap3.SetWindowText( szText ); }
	void				_SetDetail_Map4Text( LPCTSTR szText ){ m_rdo_DMap4.SetWindowText( szText ); }
	int					_GetCheckMapIndex(){ return m_nCheckMapIndex; }

	// ==== Alpha Slider
	int					_GetAlphaSize(){ return m_sld_Size.GetPos(); }
	int					_GetAlphaDecrement(){ return m_sld_Decrement.GetPos(); }
	float				_GetAlphaDecrementConstant(){ return _Control2float( &m_combo_DecreaseConstant ); }	

	void				_ResetPreviewTexture();

protected:
	// ==== MapSize
	void				__ReadMapSize( bool bMapping );


	// ==== Control
public:
	CTreeCtrl*			_GetTreeTexture(){ return &m_tree_Texture; }


	// ==== 명암
public:
	static void			MoveCallBack_Gray( float fDelta );



	// 대화 상자 데이터입니다.
public:
	enum { IDD = IDD_DLG_MT_MAPING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CTreeCtrl m_tree_Texture;
	CButton m_rdo_DMap0;
	CButton m_rdo_DMap1;
	CButton m_rdo_DMap2;
	CButton m_rdo_DMap3;
	CButton m_rdo_DMap4;
	CButton m_rdo_Gray;
	CButton m_rdo_White;
	CSliderCtrl m_sld_Size;
	CSliderCtrl m_sld_Decrement;
	CSliderCtrl m_sld_MapSize;
	CsEditFosus m_edit_AlphaSize;
	CsEditFosus m_edit_AlphaDecrement;
	CsEditFosus m_edit_MapSize;
	CComboBox m_combo_DecreaseConstant;
	CStatic m_static_GrayColor;
	
public:
	virtual BOOL OnInitDialog();	
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedBtnAddTexture();
	afx_msg void OnTvnSelchangedTreeTexture(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnDeleteAll();
	afx_msg void OnBnClickedButtonSetDmap0();
	afx_msg void OnBnClickedButtonSetDmap1();
	afx_msg void OnBnClickedButtonSetDmap2();
	afx_msg void OnBnClickedButtonSetDmap3();
	afx_msg void OnBnClickedButtonSetDmap4();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedRadioDmap0();
	afx_msg void OnBnClickedRadioDmap1();
	afx_msg void OnBnClickedRadioDmap2();
	afx_msg void OnBnClickedRadioDmap3();
	afx_msg void OnBnClickedRadioDmap4();
	afx_msg void OnBnClickedRadioGray();
	afx_msg void OnBnClickedRadioWhite();
	afx_msg void OnCbnSelchangeComboDecreaseConstant();
	afx_msg void OnBnClickedButtonSwap12();
	afx_msg void OnBnClickedButtonSwap23();
	afx_msg void OnBnClickedButtonSwap13();
	afx_msg void OnStnClickedStaticGraycolor();
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
protected:
	CComboBox m_combo_MPPG;
public:
	afx_msg void OnCbnSelchangeComboMppg();
protected:
	CsSpinCtrl m_spin_Gray;
public:
	afx_msg void OnBnClickedButtonGrayClear();
protected:
	CSliderCtrl m_sld_OP;
	CEdit m_edit_OP;
	CComboBox m_cmb_BaseMPPG;
public:
	afx_msg void OnCbnSelchangeComboBasemppg();
};


