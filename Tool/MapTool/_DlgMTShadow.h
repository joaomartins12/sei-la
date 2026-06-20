#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// _DlgMTShadow 대화 상자입니다.

class _DlgMTShadow : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTShadow)

public:
	_DlgMTShadow(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTShadow();

public:
	virtual void	_Delete();
	virtual void	_ViewInit();
	virtual void	_PostViewReset();
	virtual void	_KeyDownEscape();

protected:
	sCOLORDLG_INFO	m_ShadowColor;
	
protected:
	void		__ReadInfo();

	//===========================================================================================
	//
	//		Shadow Map Blur
	//
	//===========================================================================================
protected:
	int			m_nSMBlurNodeCount;
	BYTE**		m_ppbtSMBlurOrg;

	std::map< DWORD, BYTE* >		m_mapSMBlure_TBObj;

public:
	struct sBLUR_INFO{
		int s_nSizeSqare;
		int s_nSrcX;
		int s_nSrcY;
		int s_nDestX;
		int s_nDestY;
		int s_nNodeIndex;
		int s_nPixelWidth;
		int s_nLimitLeft;
		int s_nLimitTop;
		int s_nLimitRight;
		int s_nLimitBottom;
		int	s_nPixelStride;
		enum eCOLOR_TYPE{ RED, GREEN, BLUE };
		BYTE GetPixelColor( BYTE** ppPixel, int nColorType ){
			return ppPixel[ s_nNodeIndex ][ ( s_nDestY*s_nPixelWidth + s_nDestX )*s_nPixelStride + nColorType ];
		}
	};

	struct sBLUR_TBOBJ_INFO{
		int		s_nSizeSqare;
		int		s_nSrcX;
		int		s_nSrcY;
		int		s_nDestX;
		int		s_nDestY;
		int		s_nPixelWidth;
		int		s_nPixelHeight;
		int		s_nPixelStride;
		BYTE*	s_pOrgPixel;
		enum eCOLOR_TYPE{ RED, GREEN, BLUE };
		BYTE GetPixelColor( int nColorType ){
			return s_pOrgPixel[ ( s_nDestY*s_nPixelWidth + s_nDestX )*s_nPixelStride + nColorType ];
		}
	};

public:
	void		_ShadowMapBlur( int nSize );

protected:
	void		__InitSMBlur();
	void		__DeleteSMBlur();

	void		__DeleteDarkMapObjMap();
	
	void		__SMBlurUp( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_INFO blur );
	void		__SMBlurDown( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_INFO blur );
	void		__SMBlurLeft( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_INFO blur );
	void		__SMBlurRight( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_INFO blur );

	void		__SMBlurTBObj( DWORD dwObjID, BYTE* pOrgPixel, int nSize );
	void		__SMBlurTBObjUp( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_TBOBJ_INFO blur );
	void		__SMBlurTBObjDown( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_TBOBJ_INFO blur );
	void		__SMBlurTBObjLeft( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_TBOBJ_INFO blur );
	void		__SMBlurTBObjRight( UINT& uiSumCount, UINT& uiSumColorR, UINT& uiSumColorG, UINT& uiSumColorB, sBLUR_TBOBJ_INFO blur );

public:
// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_SHADOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:	
	CStatic m_static_ShadowColor;	
	CComboBox m_combo_ShadowMPPG;
public:	
	virtual BOOL OnInitDialog();

public:
	afx_msg void OnBnClickedButtonApply();
protected:		
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);	
	afx_msg void OnStnClickedStaticShadowcolor();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
public:
	afx_msg void OnBnClickedButtonSmblurApply();
	afx_msg void OnBnClickedButtonSmblurBack();
protected:
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	CComboBox m_combo_SMBlur_Size;
public:
	afx_msg void OnCbnSelchangeComboSmblurSize();
protected:
	CButton m_chk_IncludeTR;
public:
	afx_msg void OnBnClickedButtonApplyLightAmbient();
};
