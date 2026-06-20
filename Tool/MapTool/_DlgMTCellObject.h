#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "CellObject/CellObjManager.h"

// _DlgMTCellObject 대화 상자입니다.

class _DlgMTCellObject : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTCellObject)

	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual void	_PostViewReset();
	virtual void	_UpdateView();
	virtual void	_PostRenderView();

	virtual void	_OnLButtonDown( CPoint pt );
	virtual void	_OnLButtonUp( CPoint pt );
	virtual void	_OnMouseMove( CPoint pt );
	virtual void	_OnMouseWheel( short zDelta );
	virtual void  OnKeyUp(const DWORD msg);
	virtual void	_KeyDownEscape();
	virtual void	_OnSlide_SLeft();
	virtual void	_OnSlide_SRight();
	virtual void	_OnSlide_BLeft();
	virtual void	_OnSlide_BRight();

protected:
	bool			_IsFocus();

	// =================================================================================================
	//
	//		Mouse
	//
	// =================================================================================================
protected:
	CPoint			m_ptMouse;

	// =================================================================================================
	//
	//		Control
	//
	// =================================================================================================
protected:
	void			__InsertCellObject( LPCTSTR szPath );
public:
	void			__ResetListInfo();

	// =================================================================================================
	//
	//		Object
	//
	// =================================================================================================
protected:
	int					m_nSelectBaseIndex;
	bool				m_bRenderSelectObject;
	CsGBCell::sINFO*	m_pSelectCellObject;
	NiNodePtr			m_pNiSelectObject;	

protected:
	void			__PickObject( CPoint pt );
	void			__ResistObject();
	void			__ResistObject( int nBaseIndex, NiTransform tr, bool bUseRandom, NiPoint3 kNormal);
	void			__SetSelectObject( char* cPath );
public:
	void			__ReleaseSelectObject( bool bReleaseObjectControl = true );	

public:
	void				_SetSelectObject_Transform( NiTransform tr );
	NiNode*				_GetSelectBaseObject();
	NiAVObject*			_GetSelectLeafObject();
	CsGBCell::sINFO*	_GetSelectObject(){ return m_pSelectCellObject; }
	void				_Copy();
	void				_Cut();
	void				_Del();
	void				_Rot( float fRot );

	// =================================================================================================
	//
	//		Group
	//
	// =================================================================================================

protected:
	struct sPASTE_INFO
	{
		int				s_nBaseObjectIndex;
		NiMatrix3		s_Rotation;
		float			s_fScale;
		NiPoint3		s_vOffset;
		float			s_fDeltaHeight;
	};
	CsVectorPB< sPASTE_INFO >		m_vpPasteInfo;

protected:
	void			__GroupResist( CPoint pt );
	void			__GroupDelete( CPoint pt );
	void			__GroupCopy( CPoint pt );
	void			__GroupPaste( CPoint pt );


public:
	_DlgMTCellObject(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTCellObject();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_CELL_OBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CListCtrl m_list_CellObject;
	CButton m_chk_RandomScale;
	CsEditFloatOnly		m_edit_Scale;
	CsSpinCtrl			m_spin_Scale;
	CsEditFloatOnly		m_edit_RScaleMin;
	CsSpinCtrl			m_spin_RScaleMin;
	CsEditFloatOnly		m_edit_RScaleMax;	
	CsSpinCtrl			m_spin_RScaleMax;
	CButton m_chk_RandomXRot;
	CsEditFloatOnly		m_edit_XRotMin;
	CsSpinCtrl			m_spin_XRotMin;
	CsEditFloatOnly		m_edit_XRotMax;	
	CsSpinCtrl			m_spin_XRotMax;
	CButton m_chk_RandomYRot;
	CsEditFloatOnly		m_edit_YRotMin;
	CsSpinCtrl			m_spin_YRotMin;
	CsEditFloatOnly		m_edit_YRotMax;	
	CsSpinCtrl			m_spin_YRotMax;
	CButton m_chk_RandomZRot;
	CsEditFloatOnly		m_edit_ZRotMin;
	CsSpinCtrl			m_spin_ZRotMin;
	CsEditFloatOnly		m_edit_ZRotMax;	
	CsSpinCtrl			m_spin_ZRotMax;
	CButton m_chk_RandomZTrans;
	CsEditFloatOnly		m_edit_ZTransMin;
	CsSpinCtrl			m_spin_ZTransMin;
	CsEditFloatOnly		m_edit_ZTransMax;	
	CsSpinCtrl			m_spin_ZTransMax;

	CsEditFloatOnly		m_edit_CheckTime_Min;
	CsSpinCtrl			m_spin_CheckTime_Min;
	CsEditFloatOnly		m_edit_CheckTime_Max;	
	CsSpinCtrl			m_spin_CheckTime_Max;

	CsEditFloatOnly		m_edit_AniSpeed_Min;
	CsSpinCtrl			m_spin_AniSpeed_Min;
	CsEditFloatOnly		m_edit_AniSpeed_Max;	
	CsSpinCtrl			m_spin_AniSpeed_Max;

	CButton				m_UseTerrainNormal;

public:
	afx_msg void OnBnClickedBtnAddObject();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedCheckRandomScale();
	afx_msg void OnEnChangeEditScale();
	afx_msg void OnEnChangeEditRandomScaleMin();
	afx_msg void OnEnChangeEditRandomScaleMax();
	afx_msg void OnBnClickedCheckRandomXrot();
	afx_msg void OnBnClickedCheckRandomYrot();
	afx_msg void OnBnClickedCheckRandomZrot();
	afx_msg void OnBnClickedCheckRandomZtrans();
	afx_msg void OnEnChangeEditRandomXrotMin();
	afx_msg void OnEnChangeEditRandomXrotMax();
	afx_msg void OnEnChangeEditRandomYrotMin();
	afx_msg void OnEnChangeEditRandomYrotMax();
	afx_msg void OnEnChangeEditRandomZrotMin();
	afx_msg void OnEnChangeEditRandomZrotMax();
	afx_msg void OnEnChangeEditRandomZtransMin();
	afx_msg void OnEnChangeEditRandomZtransMax();
	afx_msg void OnNMClickListCellobject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckGroup();
	afx_msg void OnBnClickedUseTerraInNormal();
protected:
	CsEditFosus m_edit_GroupCount;
	CButton m_chk_Group;
	CSliderCtrl m_sld_GroupRadius;
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	CEdit m_edit_GroupRadius;
public:
	afx_msg void OnBnClickedBtnDelete();
protected:
	CButton m_chk_GroupDelete;
public:
	afx_msg void OnBnClickedCheckGroupDelete();
	afx_msg void OnEnChangeEditCheckTimeMin();
	afx_msg void OnEnChangeEditCheckTimeMax();
	afx_msg void OnEnChangeEditAniSpeedMin();
	afx_msg void OnEnChangeEditAniSpeedMax();
	afx_msg void OnBnClickedCheckGroupCopy();
	afx_msg void OnBnClickedCheckGroupPaste();

	void LookAt(NiAVObject* pkFrom, const NiPoint3& kTargetDir);
	NiPoint3 GetWorldDirection(NiAVObject* pkAVObject);
	NiPoint3 GetWorldUpVector(NiAVObject* pkAVObject);
	NiPoint3 GetWorldRightVector(NiAVObject* pkAVObject);
	float FindDegree(float x, float y);
protected:
	CButton m_chk_GroupCopy;
	CButton m_chk_GroupPaste;
};
