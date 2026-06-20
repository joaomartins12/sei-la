
#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// _DlgMTObject 대화 상자입니다.

class _DlgMTObject : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTObject)

public:
	_DlgMTObject(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTObject();

	// =================================================================================================
	//
	//		virtual
	//
	// =================================================================================================
public:
	virtual void	_PreSave( const char* cFile, bool bAutoSave );
	virtual void	_PostViewReset();
	virtual void	_UpdateView();
	virtual void	_PostRenderView();
	virtual void	_OnLButtonDown( CPoint pt );
	virtual void	_OnLButtonUp( CPoint pt );
	virtual void	_OnMButtonDown( CPoint pt ){}
	virtual void	_OnMButtonUp( CPoint pt ){}
	virtual void	_OnMouseMove( CPoint pt );
	virtual void	_OnMouseWheel( short zDelta );
	virtual void	_OnLButtonDbClick( CPoint pt );
	virtual void	_KeyDownEscape();
	virtual void  OnKeyUp(const DWORD msg);

public:
	enum eTREE_TYPE
	{
		TT_ROOT,
		TT_NODE,
	};
public:
	void		_Reset();
	void		_Resist();	
	void		_ToggleRenderObject();
protected:
	void		__DeleteTree();
	void		__OneDelete();
friend cToolResource;
	void		__InsertTree( LPCTSTR szRootPath, LPCTSTR szFile, bool bLastSelection = false);

public:
	void		Get__InsertTree(LPCTSTR path, LPCTSTR name)	{ return __InsertTree( path, name ); }
	void		Get__DeleteTree() { __DeleteTree(); }
	void		Get__OneDelete()  { __OneDelete(); }

public:
	void		_ReleaseObjectTree(){ m_tree_Object.SelectItem( NULL ); }

	// =================================================================================================
	//
	//		UniqObj
	//
	// =================================================================================================
protected:
	CsGBObject*		m_pUniqCsGBObject;
public:
	CsGBObject*		_GetUniqCsGBObject(){ return m_pUniqCsGBObject; }


public:
	void			_SetUniqObject( CsGBObject* pCsGBObject );
protected:
	void			__SetUniqObject_Path();
	void			__SetUniqObject_UserDefine();
	void			__SetUniqObject_ObjectType();
	void			__SetUniqObject_Scale();
	void			__SetUniqObject_ObjectID();

	// =================================================================================================
	//
	//		Dialog
	//
	// =================================================================================================
protected:
	_DlgMTObj_Portal	m_DlgPortal;
	_DlgMTObj_TexType	m_DlgTexType;
	_DlgMTObj_Merge		m_DlgMerge;
	_DlgMTObj_Ani		m_DlgAni;
public:
	_DlgMTObj_Portal*	_GetDlgPortal(){ return &m_DlgPortal; }
	bool				_IsFocus();


	// =================================================================================================
	//
	//		컨트롤
	//
	// =================================================================================================
public:
	CTreeCtrl*			_GetTreeObject(){ return &m_tree_Object; }
	bool				_IsUseSnap(){ return ( m_chk_UseSnap.GetCheck() == BST_CHECKED ); }
	int					_GetSnapSize(){ return nsCsMfcFunc::Control2Int( &m_edit_SnapSize ); }


// 대화 상자 데이터입니다.
public:
	enum { IDD = IDD_DLG_MT_OBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
protected:
	CTreeCtrl m_tree_Object;
	CButton m_check_RenderObject;	
	CButton m_check_RenderMonsterRegion;
	bool m_bRenderMonsterRegion;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnAddObject();
	afx_msg void OnBnClickedBtnAddObject( TCHAR* Name );
	afx_msg void OnObjectRouteChange(TCHAR* Name);
	afx_msg void OnBnClickedBtnDeleteAll();
	afx_msg void OnTvnSelchangedTreeObject(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnKeyDonwTreeObject( NMHDR* pNMHDR, LRESULT* pResult );

	afx_msg void OnBnClickedCheckRenderobject();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);	
	afx_msg void OnBnClickedUseObjectSnap();
protected:
	CsEditFosus m_edit_ObjectPath;
	CsEditFosus m_edit_ObjectName;
	CListBox m_list_UserDefine;
	CComboBox m_combo_ObjType;
public:
	afx_msg void OnCbnSelchangeComboObjectType();
	afx_msg void OnEnSetfocusEditScale();
	afx_msg void OnEnChangeEditScale();
protected:
	CButton m_chk_UseSnap;
	CsEditFosus m_edit_SnapSize;
	CEdit m_edit_SelCount;
	CsEditFloatOnly m_edit_Scale;
	CsSpinCtrl	m_spin_Scale;	
	CEdit m_edit_ObjectID;
	CButton m_Chk_Use_Toggle;
	CButton m_Chk_Use_ObjectSnap;
	CButton m_Chk_Show_CollisionBox;
	CButton m_Chk_Copy_NoMove;
	CSliderCtrl m_GizmoSensor;
	CSliderCtrl m_SelectedOpacity;
	CComboBox	m_CoordBox;
	bool			m_fUseNoMove;
	CButton m_Chk_Show_Occluder;

protected:
	TCHAR	m_RouteChangePath[MAX_PATH];	//경로 변경용 멤버변수

	public:
		bool GetCheckToggle() {return ( m_Chk_Use_Toggle.GetCheck() == BST_CHECKED ); }
public:
	//CsEditFloatOnly Get_EditScale() { return m_edit_Scale; }

	afx_msg void OnBnClickedCheckUseToggle();
	afx_msg void OnNMReleasedcaptureSliderGizmoSence(NMHDR *pNMHDR, LRESULT *pResult);

	void UpdateViewer(const NiTransform& kTr);
	afx_msg void OnNMReleasedcaptureSliderSelectOpacity(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelchangeBasicCoord();
	afx_msg void OnBnClickedCheckRenderMonsterregion();
	afx_msg void OnNMCustomdrawSliderGizmoSence(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckShowCollisionbox();
	afx_msg void OnBnClickedCheckCopyNomove();
	afx_msg void OnBnClickedCheckShowOccluder();
};

