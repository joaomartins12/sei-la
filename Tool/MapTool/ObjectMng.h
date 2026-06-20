
#pragma once
#include "ToolNiNode/GizmoManager.h"
#include "ToolNiNode/Editable.h"
typedef std::list< cObjectModel* >				LIST_OBJMODEL;
typedef std::list< cObjectModel* >::iterator	LIST_OBJMODEL_IT;
typedef std::map<DWORD, bool> LOCKINFO;
typedef LOCKINFO::iterator LockIter;

class cObjectMng : public GIZMO::EditAble
{
public:
	cObjectMng();
	~cObjectMng();

	//===== BaseFunc
public:
	CsMemPool< cObjectModel >	m_poolModel;
public:
	void				Delete();
	void				DeleteModel();
	void				Init();
	void				PosetViewReset();

public:
	void				Update();
	void				Render();

	void				RotationModelOnKey( bool bPlus );

	// ===== Type
protected:
	std::map< DWORD, DWORD >	m_mapPortalTypeID;
protected:
	void				_DeleteType();
	void				_InitType();

public:
	DWORD				PopPortalTypeID();
	void				PushPortalTypeID( DWORD dwPushID );
	DWORD				UsePortalTypeID( DWORD dwUseID );

	//===== Mouse
protected:
	CPoint				m_ptMouse;
public:
	void				LButtonDown( CPoint pt );
	void				LButtonDbClick( CPoint pt );
	void				LButtonUp( CPoint pt );
	void				MouseMove( CPoint pt );
	void				MouseWheel();

public:
	void				OnClickUseObjectSnap(bool bChecked, const float fInterval);
	void				OnClickUseTerrainSnap(bool bChecked, const float fInterval);

	void				UpdateOpacity(float fValue);

private:
	bool				m_bUseObjectSnap;
	bool				m_bUseTerrainSnap;

	float				m_fSnapInterval;
	float				m_fSelectedOpacity;
	//LOCKINFO		LockingInfos;

protected:
	void				_DeleteTerrainObject( DWORD dwUniqID );

	//===== Model Resist
protected:	
	LIST_OBJMODEL		m_listResistModel;//Deployable Models

public:
	cObjectModel*		InsertModel( TCHAR* szPath, TCHAR* szName );
	cObjectModel*		InsertModel( cObjectModel* pObjectModel );
	DWORD		InsertModel( const std::string& kFullPath, const NiTransform& kLastPos);
	void				SelectModelCopyResistModel();
	CsGBObject*		CreateBuffRegion(NiPoint3 kPos, float fRadius);
protected:
	void				_DeleteAllResistModel();
	void				_UpdateResistModel();
	void				_RenderResistModel();

	bool				_PickResistModel( CPoint pt );
	void				_Resist( CPoint pt );


	void				_SelectModelToResistModel();	

	//===== Model Select
protected:
	LIST_OBJMODEL		m_listSelectModel;	

public:	
	void				DeleteSelectObject();
	void				DetachSelectObject();
	void				ResetTransformSelectObject();

	void				SelectModel_ApplyTransform( NiTransform tr, bool resetViewer );

	LIST_OBJMODEL*		GetListSelectModel(){ return &m_listSelectModel; }	

public:
	bool				DeleteSelectModel( DWORD dwUniqID );
	void				_CancelAllSelectModel();
protected:
	void				_DeleteAllSelectModel();	
	void				_CancelUnSelectModel( NiNode* pSelNode );
	void				_CancelSelectModel( NiNode* pSelNode );
	void				_UpdateSelectModel();
	void				_RenderSelectModel();
	void				_PickSelectModel( CPoint pt );
	void				RenderCollisionData(bool isRender);

public:
	cObjectModel*		TerrainObjectToSelectModel( NiNode* pTRObject );
	cObjectModel*		TerrainObjectToSelectModel( DWORD dwObjectUniqID );

public:
	void				SelectObject_SelectAll();
	void				SelectObject_AttachTerrain();

	//===== Model Save
protected:
	LIST_OBJMODEL		m_listSaveModel[ 10 ];

protected:
	void				_DeleteAllSaveModel();
	void				_DeleteSaveModel( int nIndex );
public:
	void				SelectModelCopySaveModel( int nIndex );
	void				SaveModelCopySelectModel( int nIndex );

	void				SetTranslateByGizmo(const NiPoint3& kMove, bool UpdateObsever = true, bool isTwoSideTranslate = false);
	void				SetSnapByGizmo(const NiPoint3& kSnapPos);
	void				SetRotateByGizmo(const NiMatrix3& kRotate, bool UpdateObsever = true);
	void				SetRotateByGizmo(const NiPoint3& kCenterPos, int iCoord, NiFixedString kAxis, float fAngle);
	void				SetScaleByGizmo(const NiPoint3& vDir, const float& fScale);
	void				SetTransformByOUID(const DWORD& OUID, const NiTransform& kTr);
	int					TakeUndoSnapshot(int eEditType);
	void				TakeRedoSnapshot(int CUID);
	const NiPoint3	GetCenterSelectedObj(void);
	const int			GetSelectModelCnt(void);
	void				UpdateViewer();
	NiPick::Record*	SnapPickingObj(const CPoint& kMousePt);

	int GetSelectedCnt(void);
	const NiPoint3 GetSelectedCenter(void);
	NiNode* pkTestNode;
	const NiPoint3 GetSelectedFront(void);//{return NiPoint3::ZERO;}
	const NiMatrix3 GetSelectedRot(void);
	const NiPoint3 GetWorldDirection(NiAVObject* pkAVObject);
	void				SelectByDrag(const NiPoint3& kLT, const NiPoint3& kRB);
	void				LockSelectedObj();
	void				DeployOnTerrain(void);
	void				LookAt(NiAVObject* pNode, const NiPoint3& kNormal);

	bool				m_bShowCollisionBox;
	bool				m_bCopyObjectNomove;
	void				ShowCollisionBox(bool TOF);
	void				CopyNomove(bool TOF);
	void				UpdateCollisionBox(NiNode* pkNode, bool bShow);
};

extern cObjectMng		g_ObjectMng;