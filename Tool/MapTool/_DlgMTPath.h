#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "Path/PathManager.h"

enum MoveDir
{
	DIR_X,
	DIR_Y,
	DIR_END,
};
// _DlgMTPath 대화 상자입니다.

class _DlgMTPath : public _DlgBase
{
	DECLARE_DYNAMIC(_DlgMTPath)

	//====================================================================================================
	//
	//		virtual
	//
	//====================================================================================================
public:
	virtual void		_PreViewReset();
	virtual void		_PostViewReset();
	virtual void		_Delete();
	virtual void		_PostRenderView();
	virtual void		_ViewInit();
	virtual void		_PreSave( const char* cFile, bool bAutoSave );
	virtual void		_UpdateView(void);
	virtual void		_KeyDownEscape();
	virtual void		_OnLButtonDown( CPoint pt );
	virtual void		_OnLButtonUp( CPoint pt );
	virtual void		_OnMouseMove( CPoint pt );
	virtual bool		_PreCheckKey( DWORD vKeyCode, bool bDown );
	virtual void		OnKeyUp(const DWORD msg);

	//====================================================================================================
	//
	//		Base
	//
	//====================================================================================================

	//====================================================================================================
	//
	//		Path NiObject Parent
	//
	//====================================================================================================
protected:
	NiNodePtr					m_pNiTerrainRoot;
	NiNodePtr					m_pNiObjectRoot;
	//NiNodePtr					m_pPathNode;
	NiNodePtr					m_pNiMaxRoot;
protected:
	void				__ToTerrainRoot();
	void				UpdatePath(void);
	//====================================================================================================
	//
	//		Terrain Path NiObject
	//
	//====================================================================================================
protected:
	std::list< NiGeometryPtr >	m_listTerrainPath;
	NiGeometry*					m_pCurTerrainPath;
	NiPoint3					m_ptOldIntersect;


public:
	NiNode*	GetPathObjRoot(void){return m_pNiTerrainRoot;}
	void				_ResetTerrainPathList();
	void				_SetCurTerrainPath( int nIndex );
	//NiNode*				GetNiNode(){ return m_pPathNode; }	
protected:	
	void				__CalPathData( int nType, NiGeometry* pSrcGeom, long& x, long&y, long* pData );

	void				__DeleteTerrainPath();
	void				__InitTerrainPath();

	NiGeometry*			__AddTerrainPath( int nType, NiPoint3 ptSrcLeftTopMinZ );

	void				_PickSelectPath( CPoint pt );

public:
	NiGeometry*			_GetCurPathGeom(){ return m_pCurTerrainPath; }
	void				_ReleaseCurTerrainPath();
	void				_ReleaseCheckResistPath(){ m_chk_ResistPath.SetCheck( BST_UNCHECKED ); }
	void				RemovePathObject(int Key);
	NiGeometry*			__AddTerrainPath( int nType, NiPoint3 ptSrcLeftTopMinZ, const UNDO::ModelVtInfo& kPos );
	// 스핀 컨트롤 콜백 함수
public:
	static void			MoveCallBack_ET( float fDelta );
	static void			MoveCallBack_ER( float fDelta );
	static void			MoveCallBack_EL( float fDelta );
	static void			MoveCallBack_EB( float fDelta );
	static void			MoveCallBack_ERot( float fDelta, bool bUseControl = true );
	static void			MoveCallBack_PX( float fDelta );
	static void			MoveCallBack_PY( float fDelta );

	static void			OnMouseDown(float fDelta);
	static void			OnMouseDown_PX(float fDelta);
	static void			OnMouseUp_PX(float fDelta);
	static void			OnMouseDown_PY(float fDelta);
	static void			OnMouseUp_PY(float fDelta);

	void				Scale( float fDeltaScale );


	//====================================================================================================
	//
	//		Object Path NiObject
	//
	//====================================================================================================
protected:
	std::list< NiGeometryPtr >	m_listObjectPath;
protected:
	void				__DeleteObjectPath();
	void				__InitObjectPath();
	void				DeleteObjectPath(void);

	void				__AddObjectPath( UINT uiBottomVertCount, NiPoint3* pPos );

	//====================================================================================================
	//
	//		Max Path NiObject
	//
	//====================================================================================================
protected:
	std::list< NiGeometryPtr >	m_listMaxPath;
protected:
	void				__DeleteMaxPath();
	void				__InitMaxPath();

	void				__AddMaxPath( UINT uiBottomVertCount, NiPoint3* pPos );


	//====================================================================================================
	//
	//		Input
	//
	//====================================================================================================
protected:
	bool				m_bLButtonDown;


public:
	_DlgMTPath(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~_DlgMTPath();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_DLG_MT_PATH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	CButton m_chk_ResistPath;
	CsEditFosus m_edit_TerrainPathHeight;
	CsSpinCtrl m_spin_TerrainPathHeight;
	CsEditFosus m_edit_ObjectPathHeight;
	CsSpinCtrl m_spin_ObjectPathHeight;
	CsEditFosus m_edit_MaxPathHeight;
	CsSpinCtrl m_spin_MaxPathHeight;

public:
	CsSpinCtrl	m_spin_ET;
	CsSpinCtrl	m_spin_ER;
	CsSpinCtrl	m_spin_EL;
	CsSpinCtrl	m_spin_EB;
	CsSpinCtrl	m_spin_ERot;
	CsSpinCtrl	m_spin_PX;
	CsSpinCtrl	m_spin_PY;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditPathobjHeight();
	afx_msg void OnBnClickedButtonResetObjectpath();
	afx_msg void OnBnClickedButtonResetMaxpath();
	afx_msg void OnEnChangeEditObjectpathHeight();
	afx_msg void OnEnChangeEditMaxpathHeight();
public:
	CListBox m_list_TerrainPath;
public:
	afx_msg void OnBnClickedButtonResetTerrainpath();
	afx_msg void OnLbnSelchangeListTerrainpath();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnBnClickedButtonDelPath();
protected:
	CButton m_rdo_Mesh4;
	CButton m_rdo_Mesh6;
	CButton m_rdo_Mesh8;

	static int mLastCUID;
	static float m_fDownDelta;
	static bool MovePathOBjSpin;
	static bool DownPathOBjSpin;
	static MoveDir eCurrMoveDIR;
	void CheckingDownPathObjXSpin(float fDeltaTime);
	float fDownAccumTime;
	void ResetSpinControllInfo(void);
public:
	afx_msg void OnBnClickedRadioMesh4();
	afx_msg void OnBnClickedRadioMesh6();
	afx_msg void OnBnClickedRadioMesh8();

	void OnPickPathObject(NiAVObject* pkObj);
	void ResetVertexPos( NiGeometry* pGeom, NiPoint2 ptLeftTop, NiPoint2 ptRightBottom );
	bool m_bCreateSquare;

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnStnClickedPathcnt();
	void OnBnClickedButtonDelPathAll();
	CStatic mPathCnt;
};
